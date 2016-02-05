#define LIBMSR_SOURCE
#include "mzDataInterface.h"
#include "ScanConverter.h"
#include "regex.h"
#include "filesystem.h"

#define READ_BUF_SIZE 32768

bool mzDataInterface::setInputFile(const string& filename, const ProcessingOptions& options)
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

void mzDataInterface::createIndex()
{
	XML_SetElementHandler(m_pParser, CreateIndexStartElement, CreateIndexEndElement);

	m_isDone = XML_FALSE;
	std::streamsize bytesRead;

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
}

Scan* mzDataInterface::getScan(long scanNumber, bool getPeaks)
{
	if( scanNumber == 0 )
	{
		if( curScanNumber_ < firstScanNumber_ )
			m_curScanIndexItr = m_scanIndex.begin();
		else
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
	if(!parseNextScan(m_curScanIndexItr->second))
	{
		cerr << "Warning: scan " << m_curScanIndexItr->first << " indexed but not parseable" << endl;
		return NULL; // scan could not be parsed
	}

	if( m_pCurScan->retentionTimeInSec_ < 0 )
		m_pCurScan->retentionTimeInSec_ = (double) m_curScanIndexItr->first / (double) m_scanIndex.rbegin()->first;
	setDefaultScanInfo(m_pCurScan); // fill out uninitialized fields in the scan from instrument settings
	return m_pCurScan;
}

bool mzDataInterface::parseNextScan(const XML_Index& scanByteOffset)
{
	std::streamsize bytesRead = 0;
	m_inputFile.clear();
	m_inputFile.seekg(scanByteOffset);
	m_pCurScan = NULL;
	XML_Status status;
	XML_ParserReset(m_pParser, m_xmlEncoding.c_str());
	XML_SetUserData(m_pParser, this);
	XML_SetElementHandler(m_pParser, BodyStartElement, BodyEndElement);

	do
	{
		char* buf = (char*) XML_GetBuffer(m_pParser, READ_BUF_SIZE);
		if(buf == NULL)
			return false;

		if( bytesRead == 0 )
		{
			std::copy( m_xmlDecl.begin(), m_xmlDecl.end(), buf );
			m_inputFile.read(buf + m_xmlDecl.length(), READ_BUF_SIZE - m_xmlDecl.length());
		} else
			m_inputFile.read(buf, READ_BUF_SIZE);

		bytesRead = m_inputFile.gcount();
		m_isDone = bytesRead < READ_BUF_SIZE;

		status = XML_ParseBuffer(m_pParser, (int) bytesRead, 0);

		switch(status)
		{
			case XML_STATUS_ERROR:
				cerr << XML_ErrorString( XML_GetErrorCode(m_pParser) ) << " at line " <<
						XML_GetCurrentLineNumber(m_pParser) << endl;
				return false;
			case XML_STATUS_OK:
				if(m_isDone == XML_TRUE)
					return true;
				break;
			case XML_STATUS_SUSPENDED:
				return true;
		}
	} while(m_isDone == XML_FALSE);

	// finished parsing
	m_pCurScan = NULL;
	return true;
}

bool mzDataInterface::parseHeader()
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

		XML_Status status = XML_ParseBuffer(m_pParser, (int) bytesRead, m_isDone);

		switch(status)
		{
			case XML_STATUS_OK:
				break;
			case XML_STATUS_ERROR:
				cout << XML_ErrorString( XML_GetErrorCode(m_pParser) ) << " at line " <<
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

bool mzDataInterface::parseScan(mzDataInterface*& pInstance, const char **atts, int attsCount)
{
	Scan*& pCurScan = pInstance->m_pCurScan;

	pCurScan = new Scan();
	pCurScan->source_ = pInstance->curSourceName_;
	pCurScan->num_ = lexical_cast<long>(GET_ATTR("id"));
	return true;
}

bool mzDataInterface::parseCvParam(mzDataInterface*& pInstance, const CVAccession& id, const string& name, const string& value)
{
	Scan*& pCurScan = pInstance->m_pCurScan;
	InstrumentInfo& instrumentInfo = pInstance->instrumentInfo_;

	if(id.label_ == "psi")
	{
		if(id.number_ == 1000008)
		{
			instrumentInfo.ionSource_ = MSIonizationTypeFromOBOText(to_lower_copy(value));
		}
		else if(id.number_ == 1000010)
		{
			if(pCurScan == NULL)
				instrumentInfo.analyzerList_.push_back( MSAnalyzerTypeFromOBOText(to_lower_copy(value)) );
			else
				pCurScan->analyzer_ = MSAnalyzerTypeFromOBOText(to_lower_copy(value));
		}
		else if(id.number_ == 1000026)
		{
			instrumentInfo.detector_ = MSDetectorTypeFromOBOText(to_lower_copy(value));
		}
		else if(id.number_ == 1000033)
		{
			pInstance->doDeisotope_ = toBool(value);
		}
		else if(id.number_ == 1000035)
		{
			if(value == "discrete")
				pInstance->doCentroid_ = true;
			else if(value == "continuous")
				pInstance->doCentroid_ = false;
			else
				cerr << "Error parsing line " << XML_GetCurrentLineNumber(pInstance->m_pParser) <<
						": invalid value for CV accession '" << (string) id << "':" << value << endl;
		}
		else if(id.number_ == 1000037)
		{
			pCurScan->polarity_ = MSPolarityTypeFromOBOText(to_lower_copy(value));
		}
		else if(id.number_ == 1000038)
		{
			pCurScan->retentionTimeInSec_ = lexical_cast<double>(value) * 60.0;
		}
		else if(id.number_ == 1000039)
		{
			pCurScan->retentionTimeInSec_ = lexical_cast<double>(value);
		}
		else if(id.number_ == 1000040)
		{
			pCurScan->precursorList_.back().MZ_ = lexical_cast<double>(value);
		}
		else if(id.number_ == 1000041)
		{
			pCurScan->precursorList_.back().charge_ = lexical_cast<int>(value);
		}
		else if(id.number_ == 1000042)
		{
			pCurScan->precursorList_.back().intensity_ = lexical_cast<double>(value);
		}
		else if(id.number_ == 1000044)
		{
			pCurScan->activation_ = MSActivationTypeFromOBOText(to_lower_copy(value));
		}
		else if(id.number_ == 1000045)
		{
			pCurScan->precursorList_.back().collisionEnergy_ = lexical_cast<double>(value);
		}
		else if(id.number_ == 1000210)
		{
			pCurScan->basePeakMZ_ = lexical_cast<double>(value);
		}
		else if(id.number_ == 1000285)
		{
			pCurScan->totalIonCurrent_ = lexical_cast<double>(value);
		}
		return true;
	}
	else
	{
		// ignore unknown cvParams, but report them as warnings
		if(pInstance->m_unknownCvCounts.count(id) > 0)
			++pInstance->m_unknownCvCounts[id].second;
		else
			pInstance->m_unknownCvCounts[id] = pair<string, int>(name, 1);
	}

	return false;
}

void mzDataInterface::CreateIndexStartElement(void *userData, const char *name, const char **atts)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	int attsCount = XML_GetSpecifiedAttributeCount(pInstance->m_pParser);
	string tag(name);

	if(tag == "spectrum")
	{
		long scanNumber = lexical_cast<long>(GET_ATTR("id"));
		XML_Index byteOffset = XML_GetCurrentByteIndex(pInstance->m_pParser);
		pInstance->m_scanIndex[scanNumber] = byteOffset;
	}
}

void mzDataInterface::CreateIndexEndElement(void *userData, const char *name)
{
}

void mzDataInterface::HeaderStartElement(void *userData, const char *name, const char **atts)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	int attsCount = XML_GetSpecifiedAttributeCount(pInstance->m_pParser);
	string tag(name);

	//InstrumentInfo& instrumentInfo = pInstance->instrumentInfo_;
	//Scan*& pCurScan = pInstance->m_pCurScan;

	if(tag == "cvParam")
	{
		CVAccession id(GET_ATTR("accession"));
		parseCvParam(pInstance, id, GET_ATTR("name"), GET_ATTR("value"));

	} else if(tag == "nameOfFile")
	{
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandleSourceFileName);
	} else if(tag == "pathToFile")
	{
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandleSourcePath);
	} else if(tag == "instrumentName")
	{
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandleInstrumentName);
	} else if(tag == "name")
	{
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandleName);
	} else if(tag == "version")
	{
		XML_SetCharacterDataHandler(pInstance->m_pParser, HandleVersion);
	} else if(tag == "spectrum")
	{
		XML_StopParser(pInstance->m_pParser, XML_TRUE);
	} else if(tag == "spectrumList")
	{
	}
}

void mzDataInterface::HeaderEndElement(void *userData, const char *name)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	string tag(name);

	if(tag == "nameOfFile" || tag == "pathToFile")
	{
		XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);
	} else if(tag == "sourceFile")
	{
		static regex fileURIRegex("^file://(\\S+)/(\\S+?)/(\\S+)");
		smatch fileURI;
		boost::replace_all(pInstance->m_curSourcePath, "\\", "/");
		trim_right_if(pInstance->m_curSourcePath, boost::is_any_of("/"));
		string filePathURI = pInstance->m_curSourcePath + '/' + pInstance->m_curSourceFileName;
		regex_match(filePathURI, fileURI, fileURIRegex);
		pInstance->inputFileNameList_.push_back(fileURI[3]);
		pInstance->m_curSourcePath.clear();
		pInstance->m_curSourceFileName.clear();
	} else if(tag == "instrumentName")
	{
		pInstance->instrumentInfo_.instrumentModel_ = MSInstrumentModelTypeFromString(pInstance->m_curInstrumentName);
		pInstance->m_curInstrumentName.clear();
		XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);
	} else if(tag == "name")
	{
		pInstance->instrumentInfo_.acquisitionSoftware_ = MSAcquisitionSoftwareTypeFromString(pInstance->m_curName);
		pInstance->m_curName.clear();
		XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);
	} else if(tag == "version")
	{
		pInstance->instrumentInfo_.acquisitionSoftwareVersion_ = pInstance->m_curVersion;
		pInstance->m_curVersion.clear();
		XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);
	}
}


void mzDataInterface::HandleSourceFileName(void *userData, const XML_Char *s, int len)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	if(len > 0) pInstance->m_curSourceFileName.append(s, s+len);
}

void mzDataInterface::HandleSourcePath(void *userData, const XML_Char *s, int len)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	if(len > 0) pInstance->m_curSourcePath.append(s, s+len);
}

void mzDataInterface::HandleInstrumentName(void *userData, const XML_Char *s, int len)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	if(len > 0) pInstance->m_curInstrumentName.append(s, s+len);
}

void mzDataInterface::HandleName(void *userData, const XML_Char *s, int len)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	if(len > 0) pInstance->m_curName.append(s, s+len);
}

void mzDataInterface::HandleVersion(void *userData, const XML_Char *s, int len)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	if(len > 0) pInstance->m_curVersion.append(s, s+len);
}

void mzDataInterface::HandlePeakData(void *userData, const XML_Char *s, int len)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	if(len > 0) pInstance->m_curPeakData.append(s, s+len);
}



void mzDataInterface::BodyStartElement(void *userData, const char *name, const char **atts)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	int attsCount = XML_GetSpecifiedAttributeCount(pInstance->m_pParser);
	string tag(name);
	Scan*& pCurScan = pInstance->m_pCurScan;

	if(tag == "cvParam")
	{
		CVAccession id(GET_ATTR("accession"));
		parseCvParam(pInstance, id, GET_ATTR("name"), GET_ATTR("value"));
	} else if(tag == "userParam")
	{
		string name = to_lower_copy(string(GET_ATTR("name")));
		if(name == "scan type" || name == "scantype")
			pCurScan->scanType_ = MSScanTypeFromString(GET_ATTR("value"));
		else if(name == "base peak intensity")
			pCurScan->basePeakIntensity_ = lexical_cast<double>(GET_ATTR("value"));
		else if(name == "lowest observed m/z")
			pCurScan->minObservedMZ_ = lexical_cast<double>(GET_ATTR("value"));
		else if(name == "highest observed m/z")
			pCurScan->maxObservedMZ_ = lexical_cast<double>(GET_ATTR("value"));

	} else if(tag == "spectrum")
	{
		parseScan(pInstance, atts, attsCount);
	} else if(tag == "acqSpecification")
	{
		if(HAS_ATTR("spectrumType"))
		{
			string type = GET_ATTR("spectrumType");
			if(type == "discrete")
				pCurScan->isCentroided_ = true;
			else if(type == "continuous")
				pCurScan->isCentroided_ = false;
			else
				cerr << "Error parsing line " << XML_GetCurrentLineNumber(pInstance->m_pParser) <<
						": invalid value for attribute 'spectrumType': " << type << endl;
		} else
			pCurScan->isCentroided_ = pInstance->doCentroid_;

		if(HAS_ATTR("count"))
			pCurScan->numScanOrigins_ = lexical_cast<int>(GET_ATTR("count"));
	} else if(tag == "acquisition")
	{
		pCurScan->scanOriginNums.push_back( lexical_cast<long>(GET_ATTR("acqNumber")) );
	} else if(tag == "spectrumInstrument")
	{
		pCurScan->msLevel_ = lexical_cast<int>(GET_ATTR("msLevel"));
		if(HAS_ATTR("mzRangeStart")) pCurScan->startMZ_ = lexical_cast<double>(GET_ATTR("mzRangeStart"));
		if(HAS_ATTR("mzRangeStop")) pCurScan->endMZ_ = lexical_cast<double>(GET_ATTR("mzRangeStop"));
	} else if(tag == "precursor")
	{
		pCurScan->precursorList_.push_back( PrecursorScanInfo( lexical_cast<long>(GET_ATTR("spectrumRef")), lexical_cast<int>(GET_ATTR("msLevel")), 0 ) );
	} else if(tag == "mzArrayBinary")
	{
		pInstance->m_curBinaryArrayType = MZ_BINARY_ARRAY;

	} else if(tag == "intenArrayBinary")
	{
		pInstance->m_curBinaryArrayType = INTEN_BINARY_ARRAY;

	} else if(pInstance->m_curBinaryArrayType != NO_BINARY_ARRAY && tag == "data")
	{
		// call setNumDataPoints only once (for the mzArrayBinary, since it must come first)
		if(pInstance->m_curBinaryArrayType == MZ_BINARY_ARRAY)
			pCurScan->setNumDataPoints( lexical_cast<int>(GET_ATTR("length")), pInstance->m_curGetPeaks );

		if(pInstance->m_curGetPeaks)
		{
			pInstance->m_curIsDoublePrecision = XMLIsDoublePrecision(GET_ATTR("precision"));
			pInstance->m_curBinaryEndianType = XMLEndianType(GET_ATTR("endian"));
			XML_SetCharacterDataHandler(pInstance->m_pParser, HandlePeakData);
		}
	}
}

void mzDataInterface::BodyEndElement(void *userData, const char *name)
{
	mzDataInterface* pInstance = static_cast<mzDataInterface*>(userData);
	string tag(name);
	Scan*& pCurScan = pInstance->m_pCurScan;

	if(tag == "spectrum")
	{
		XML_StopParser(pInstance->m_pParser, XML_TRUE);

	} else if(pInstance->m_curGetPeaks && pInstance->m_curBinaryArrayType != NO_BINARY_ARRAY && tag == "data")
	{
		if(pCurScan->getNumDataPoints() > 0)
		{
			boost::trim(pInstance->m_curPeakData); // trim flanking whitespace from base64
			if(pInstance->m_curBinaryArrayType == MZ_BINARY_ARRAY)
			{
				ScanConverter decoder(	pCurScan,
										pInstance->m_curPeakData.c_str(), NULL,
										false, false,
										pInstance->m_curIsDoublePrecision, false,
										pInstance->m_curBinaryEndianType, COMMON_BIG_ENDIAN );

				if(pCurScan->minObservedMZ_ < 0)
					pCurScan->minObservedMZ_ = pCurScan->mzArray_[0];
				if(pCurScan->maxObservedMZ_ < 0)
					pCurScan->maxObservedMZ_ = pCurScan->mzArray_[ pCurScan->getNumDataPoints()-1 ];
			} else
			{
				ScanConverter decoder(	pCurScan,
										NULL, pInstance->m_curPeakData.c_str(),
										false, false,
										false, pInstance->m_curIsDoublePrecision,
										COMMON_BIG_ENDIAN, pInstance->m_curBinaryEndianType );
			}
		}
		pInstance->m_curPeakData.clear();
		XML_SetCharacterDataHandler(pInstance->m_pParser, NULL);
		pInstance->m_curBinaryArrayType = NO_BINARY_ARRAY;
	}
}
