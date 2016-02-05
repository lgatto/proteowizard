#define LIBMSR_SOURCE
#include "mzXMLInterface.h"
#include "ScanConverter.h"
#include "regex.h"
#include "filesystem.h"

#define READ_BUF_SIZE 32768

bool mzXMLInterface::setInputFile(const string& filename, const ProcessingOptions& options)
{
	// reinitialize parser
	if(!initInterface())
		return false;

	// close previous input file
	m_inputFile.close();
	m_inputFile.clear();

	m_inputFilename = filename;
	m_inputFile.open(m_inputFilename.c_str(), ios::binary);
	if(!m_inputFile)
		return false;

	m_scanIndex.clear();
	createIndex();
	curScanNumber_ = 0;
	totalNumScans_ = m_scanIndex.size();
	if(totalNumScans_ > 0)
	{
		firstScanNumber_ = m_scanIndex.begin()->first;
		lastScanNumber_ = m_scanIndex.rbegin()->first;
	} else
	{
		firstScanNumber_ = 0;
		lastScanNumber_ = 0;
	}

	curSourceName_ = basename(path(filename, filesystem::native));
	instrumentInfo_.analyzerList_.clear();
	m_isSuspended = XML_FALSE;
	m_isDone = XML_FALSE;

	return parseHeader();
}

void mzXMLInterface::createIndex()
{
	XML_SetElementHandler(m_pParser, CreateIndexStartElement, CreateIndexEndElement);

	m_isDone = XML_FALSE;
	std::streamsize bytesRead;
	m_curScanIndexItr = m_scanIndex.end();

	do
	{
		char* buf = (char*) XML_GetBuffer(m_pParser, READ_BUF_SIZE);
		if(buf == NULL)
			break;
		m_inputFile.read(buf, READ_BUF_SIZE);
		bytesRead = m_inputFile.gcount();
		m_isDone = bytesRead < READ_BUF_SIZE;

		XML_ParseBuffer(m_pParser, (int) bytesRead, m_isDone);
	} while( m_isDone == XML_FALSE );

	if( !m_scanIndex.empty() && m_scanIndex.rbegin()->second.second == 0 )
		throw runtime_error("failed to create mzXML index (msRun element not closed)");
}

Scan* mzXMLInterface::getScan(long scanNumber, bool getPeaks)
{
	if( scanNumber == 0 )
	{
		if( curScanNumber_ < firstScanNumber_ )
			m_curScanIndexItr = m_scanIndex.begin();
		else if( m_curScanIndexItr != m_scanIndex.end() )
		{
			++m_curScanIndexItr;
		}
	} else
	{
		curScanNumber_ = scanNumber;
		m_curScanIndexItr = m_scanIndex.find(curScanNumber_);
	}

	if( m_curScanIndexItr == m_scanIndex.end() )
		return NULL; // scan not available

	if( scanNumber == 0 )
		curScanNumber_ = m_curScanIndexItr->first;

	m_curGetPeaks = getPeaks;
	if( !parseCurScan() || m_pCurScan == NULL )
	{
		cerr << "Warning: scan " << m_curScanIndexItr->first << " indexed but not parseable" << endl;
		return NULL; // scan could not be parsed
	}

	if( m_pCurScan->retentionTimeInSec_ < 0 )
		m_pCurScan->retentionTimeInSec_ = (double) m_curScanIndexItr->first / (double) m_scanIndex.rbegin()->first;
	setDefaultScanInfo(m_pCurScan); // fill out uninitialized fields in the scan from instrument settings
	return m_pCurScan;
}

bool mzXMLInterface::parseCurScan()
{
	std::streamsize bytesRead = 0;
	m_inputFile.clear();
	//large_seekg(m_inputFile, m_curScanIndexItr->second.first);
	m_inputFile.seekg(m_curScanIndexItr->second.first);
	if(!m_inputFile)
		throw runtime_error("bad file seek position of " + lexical_cast<string>(m_curScanIndexItr->second.first));
	m_pCurScan = NULL;
	XML_Status status;
	XML_ParserReset(m_pParser, NULL);
	XML_SetUserData(m_pParser, this);
	XML_SetElementHandler(m_pParser, BodyStartElement, BodyEndElement);

	size_t bytesToRead = m_curScanIndexItr->second.second;
	size_t bytesToParse = m_xmlDecl.length() + bytesToRead;
	char* buf = (char*) XML_GetBuffer(m_pParser, bytesToParse);
	if(buf == NULL)
		return false;

	strcpy( buf, m_xmlDecl.c_str() );
	m_inputFile.read(buf + m_xmlDecl.length(), bytesToRead);
	if(!m_inputFile)
		throw runtime_error("bad file read");
	bytesRead = m_xmlDecl.length() + m_inputFile.gcount();
	status = XML_ParseBuffer(m_pParser, (int) bytesRead, 0);

	switch(status)
	{
		default:
		case XML_STATUS_ERROR:
			cerr << XML_ErrorString( XML_GetErrorCode(m_pParser) ) << " at line " <<
					XML_GetCurrentLineNumber(m_pParser) << endl;
			return false;

		case XML_STATUS_OK:
		case XML_STATUS_SUSPENDED:
			return true;
	}
}

bool mzXMLInterface::parseHeader()
{
	m_inputFile.clear();
	m_inputFile.seekg(0);
	XML_ParserReset(m_pParser, NULL);
	XML_SetUserData(m_pParser, this);
	XML_SetElementHandler(m_pParser, HeaderStartElement, HeaderEndElement);
	XML_SetXmlDeclHandler(m_pParser, BaseXMLInterface::DocumentXmlDeclHandler);

	std::streamsize bytesRead;
	do
	{
		char* buf = (char*) XML_GetBuffer(m_pParser, READ_BUF_SIZE);
		if(buf == NULL)
			return false;
		m_inputFile.read(buf, READ_BUF_SIZE);
		bytesRead = m_inputFile.gcount();
		m_isDone = bytesRead < READ_BUF_SIZE;

		XML_Status status = XML_ParseBuffer(m_pParser, (int) bytesRead, 0);

		switch(status)
		{
			case XML_STATUS_OK:
				break;
			case XML_STATUS_ERROR:
				cerr << XML_ErrorString( XML_GetErrorCode(m_pParser) ) << " at line " <<
						XML_GetCurrentLineNumber(m_pParser) << endl;
				return false;
			case XML_STATUS_SUSPENDED:
				return true;
		}
	} while(m_isDone == XML_FALSE);

	// parsing finished without being suspended (incomplete header or no scans in file)
	cerr << "Error: unable to parse header or read first scan from file \"" << m_inputFilename << "\"" << endl;
	return false;
}

bool mzXMLInterface::parseScan(mzXMLInterface*& pInstance, const char **atts, int attsCount)
{
	Scan*& pCurScan = pInstance->m_pCurScan;

	pCurScan = new Scan();
	pCurScan->source_ = pInstance->curSourceName_;
	pCurScan->num_ = lexical_cast<long>(GET_ATTR("num"));
	pCurScan->msLevel_ = lexical_cast<int>(GET_ATTR("msLevel"));
	pCurScan->setNumDataPoints( lexical_cast<int>(GET_ATTR("peaksCount")), pInstance->m_curGetPeaks );

	if(HAS_ATTR("retentionTime"))
	{
		string duration = GET_ATTR("retentionTime");
		duration = duration.substr(2, duration.length()-3);
		pCurScan->retentionTimeInSec_ = lexical_cast<double>(duration);
	}
	if(HAS_ATTR("polarity")) pCurScan->polarity_ = MSPolarityTypeFromString(GET_ATTR("polarity"));
	if(HAS_ATTR("scanType")) pCurScan->scanType_ = MSScanTypeFromString(GET_ATTR("scanType"));
	if(HAS_ATTR("collisionEnergy")) pInstance->m_curCollisionEnergy = lexical_cast<double>(GET_ATTR("collisionEnergy"));
	if(HAS_ATTR("lowMz")) pCurScan->minObservedMZ_ = lexical_cast<double>(GET_ATTR("lowMz"));
	if(HAS_ATTR("highMz")) pCurScan->maxObservedMZ_ = lexical_cast<double>(GET_ATTR("highMz"));
	if(HAS_ATTR("basePeakMz")) pCurScan->basePeakMZ_ = lexical_cast<double>(GET_ATTR("basePeakMz"));
	if(HAS_ATTR("basePeakIntensity")) pCurScan->basePeakIntensity_ = lexical_cast<double>(GET_ATTR("basePeakIntensity"));
	if(HAS_ATTR("totIonCurrent")) pCurScan->totalIonCurrent_ = lexical_cast<double>(GET_ATTR("totIonCurrent"));
	if(HAS_ATTR("msInstrumentID")) pCurScan->analyzer_ = pInstance->m_instrumentIdToAnalyzer[GET_ATTR("msInstrumentID")];

	if(HAS_ATTR("centroided"))
		pCurScan->isCentroided_ = toBool(GET_ATTR("centroided"));
	else
		pCurScan->isCentroided_ = pInstance->m_allScansCentroided;

	if(HAS_ATTR("deisotoped"))
		pCurScan->isDeisotoped_ = toBool(GET_ATTR("deisotoped"));
	else
		pCurScan->isDeisotoped_ = pInstance->m_allScansDeisotoped;

	if(HAS_ATTR("startMz"))
		pCurScan->startMZ_ = lexical_cast<double>(GET_ATTR("startMz"));
	else if(pCurScan->minObservedMZ_ >= 0)
		pCurScan->startMZ_ = pCurScan->minObservedMZ_;
	else
		pCurScan->startMZ_ = 0;

	if(HAS_ATTR("endMz"))
		pCurScan->endMZ_ = lexical_cast<double>(GET_ATTR("endMz"));
	else if(pCurScan->maxObservedMZ_ >= 0)
		pCurScan->endMZ_ = pCurScan->maxObservedMZ_;
	else
		pCurScan->endMZ_ = 0;

	return true;
}

void mzXMLInterface::CreateIndexStartElement(void *userData, const char *name, const char **atts)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	int attsCount = XML_GetSpecifiedAttributeCount(pInstance->m_pParser);
	string tag(name);

	if(tag == "scan")
	{
		long scanNumber = lexical_cast<long>(GET_ATTR("num"));
		XML_Index byteOffset = XML_GetCurrentByteIndex(pInstance->m_pParser);

		if(pInstance->m_curScanIndexItr != pInstance->m_scanIndex.end())
			pInstance->m_curScanIndexItr->second.second = byteOffset - pInstance->m_curScanIndexItr->second.first;

		pair< mzXMLIndex::iterator, bool > insertPair = pInstance->m_scanIndex.insert(
			mzXMLIndex::value_type( scanNumber, mzXMLIndexEntry( byteOffset, 0 ) ) );
		pInstance->m_curScanIndexItr = insertPair.first;
	}
}

void mzXMLInterface::CreateIndexEndElement(void *userData, const char *name)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	string tag(name);

	if(tag == "msRun")
	{
		XML_Index byteOffset = XML_GetCurrentByteIndex(pInstance->m_pParser);
		if(pInstance->m_curScanIndexItr != pInstance->m_scanIndex.end())
			pInstance->m_curScanIndexItr->second.second = byteOffset - pInstance->m_curScanIndexItr->second.first;
	}
}

void mzXMLInterface::HeaderStartElement(void *userData, const char *name, const char **atts)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	int attsCount = XML_GetSpecifiedAttributeCount(pInstance->m_pParser);
	string tag(name);

	InstrumentInfo& instrumentInfo = pInstance->instrumentInfo_;
	//Scan*& pCurScan = pInstance->m_pCurScan;

	if(tag == "parentFile")
	{
		static regex fileURIRegex("^file://(\\S+)/(\\S+?)/(\\S+)");
		smatch fileURI;
		string fileNameStr = GET_ATTR("fileName");
		regex_match(fileNameStr, fileURI, fileURIRegex);
		pInstance->inputFileNameList_.push_back(fileURI[3]);
	} else if(tag == "msInstrument")
	{
		if(HAS_ATTR("msInstrumentID")) pInstance->m_curInstrumentId = GET_ATTR("msInstrumentID");
	} else if(tag == "msManufacturer")
	{
		instrumentInfo.manufacturer_ = MSManufacturerTypeFromString(GET_ATTR("value"));
	} else if(tag == "msModel")
	{
		instrumentInfo.instrumentModel_ = MSInstrumentModelTypeFromString(GET_ATTR("value"));
	} else if(tag == "msIonisation")
	{
		instrumentInfo.ionSource_ = MSIonizationTypeFromString(GET_ATTR("value"));
	} else if(tag == "msMassAnalyzer")
	{
		MSAnalyzerType analyzer = MSAnalyzerTypeFromString(GET_ATTR("value"));
		pInstance->m_instrumentIdToAnalyzer[pInstance->m_curInstrumentId] = analyzer;
		instrumentInfo.analyzerList_.push_back(analyzer);
	} else if(tag == "msDetector")
	{
		instrumentInfo.detector_ = MSDetectorTypeFromString(GET_ATTR("value"));
	} else if(tag == "software")
	{
		string softwareType = GET_ATTR("type");
		if(softwareType == "acquisition")
		{
			instrumentInfo.acquisitionSoftware_ = MSAcquisitionSoftwareTypeFromString(GET_ATTR("name"));
			instrumentInfo.acquisitionSoftwareVersion_ = GET_ATTR("version");
		}
	} else if(tag == "scan")
	{
		XML_StopParser(pInstance->m_pParser, XML_TRUE);
	} else if(tag == "dataProcessing")
	{
		if(HAS_ATTR("centroided"))
		{
			pInstance->m_allScansCentroided = toBool(GET_ATTR("centroided"));
			pInstance->doCentroid_ = pInstance->m_allScansCentroided;
		}

		if(HAS_ATTR("deisotoped"))
		{
			pInstance->m_allScansDeisotoped = toBool(GET_ATTR("deisotoped"));
			pInstance->doDeisotope_ = pInstance->m_allScansDeisotoped;
		}
	}
}

void mzXMLInterface::HeaderEndElement(void *userData, const char *name)
{
	//mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	//string tag(name);
}

void mzXMLInterface::HandlePrecursorMz(void *userData, const XML_Char *s, int len)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	if(len > 0) pInstance->m_curPrecursorMz.append(s, s+len);
}

void mzXMLInterface::HandlePeakData(void *userData, const XML_Char *s, int len)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	if(pInstance->m_curBinaryCompressionType != BINARY_COMPRESSION_UNDEF && len > 0) pInstance->m_curPeakData.append(s, s+len);
}

void mzXMLInterface::BodyStartElement(void *userData, const char *name, const char **atts)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	int attsCount = XML_GetSpecifiedAttributeCount(pInstance->m_pParser);
	string tag(name);
	Scan*& pCurScan = pInstance->m_pCurScan;

	if(tag == "scan")
	{
		if(pCurScan != NULL)
			XML_StopParser(pInstance->m_pParser, XML_TRUE);
		else
			parseScan(pInstance, atts, attsCount);
	} else if(tag == "precursorMz")
	{
		pCurScan->precursorList_.push_back( PrecursorScanInfo(0, 0, 0) );
		pCurScan->precursorList_.back().collisionEnergy_ = pInstance->m_curCollisionEnergy;
		pCurScan->precursorList_.back().msLevel_ = pCurScan->msLevel_ - 1;
		pCurScan->activation_ = CID; // mzXML seems to only support this activation type
		if(HAS_ATTR("precursorScanNum")) pCurScan->precursorList_.back().num_ = lexical_cast<long>(GET_ATTR("precursorScanNum"));
		if(HAS_ATTR("precursorCharge")) pCurScan->precursorList_.back().charge_ = lexical_cast<int>(GET_ATTR("precursorCharge"));
		if(HAS_ATTR("precursorIntensity")) pCurScan->precursorList_.back().intensity_ = lexical_cast<double>(GET_ATTR("precursorIntensity"));
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandlePrecursorMz);

	} else if(pInstance->m_curGetPeaks && tag == "peaks")
	{
		pInstance->m_curIsDoublePrecision = XMLIsDoublePrecision(GET_ATTR("precision"));
		if(HAS_ATTR("compressionType"))
		{
			pInstance->m_curBinaryCompressionType = toBinaryCompressionType(GET_ATTR("compressionType"));
			if(pInstance->m_curBinaryCompressionType == BINARY_COMPRESSION_UNDEF)
				cerr << "Error: unknown compression type for scan " << pCurScan->num_ << ": " << GET_ATTR("compressionType") << endl;
		}
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandlePeakData);
	}
}

void mzXMLInterface::BodyEndElement(void *userData, const char *name)
{
	mzXMLInterface* pInstance = static_cast<mzXMLInterface*>(userData);
	string tag(name);

	if(tag == "scan")
	{
		XML_StopParser(pInstance->m_pParser, XML_TRUE);
	} else if(tag == "precursorMz")
	{
		pInstance->m_pCurScan->precursorList_.back().MZ_ = lexical_cast<double>(pInstance->m_curPrecursorMz);
		pInstance->m_curPrecursorMz.clear();
		XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);

	} else if(tag == "peaks")
	{
		if( pInstance->m_curGetPeaks &&
			pInstance->m_curBinaryCompressionType != BINARY_COMPRESSION_UNDEF &&
			pInstance->m_pCurScan->getNumDataPoints() > 0 )
		{
			boost::trim(pInstance->m_curPeakData); // trim flanking whitespace from base64
			ScanConverter decoder(	pInstance->m_pCurScan,
									pInstance->m_curPeakData.c_str(),
									pInstance->m_curBinaryCompressionType == BINARY_COMPRESSION_ZLIB,
									pInstance->m_curIsDoublePrecision,
									COMMON_BIG_ENDIAN );
			pInstance->m_curPeakData.clear();
			XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);
		}
	} else if(tag == "msRun")
	{
		XML_SetElementHandler(pInstance->m_pParser, NULL, NULL);
	}
}
