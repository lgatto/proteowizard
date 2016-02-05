#define LIBMSR_SOURCE
#include "MGFInterface.h"
#include "filesystem.h"

bool MGFInterface::initInterface()
{
	setCentroiding(true, true, true); // MGF peak lists should always be centroided
	return true;
}

bool MGFInterface::setInputFile(const string& fileName, const ProcessingOptions& options)
{
	// close previous input file
	m_inputFile.close();
	m_inputFile.clear();

	m_inputFilename = fileName;
	m_inputFile.open(m_inputFilename.c_str(), ios::binary);
	if(!m_inputFile)
		return false;

	if( file_size(path(fileName, filesystem::native)) > (boost::uintmax_t) std::numeric_limits<std::streamsize>::max() )
	{
		cerr << "Error: input file \"" << fileName << "\" is too large to be indexed" << endl;
		return false;
	}

	curSourceName_ = basename(path(fileName, filesystem::native));

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

	return true;
}

void MGFInterface::createIndex()
{
	string lineStr;
	size_t lineCount = 0;
	bool inBeginIons = false;
	long curScanNum = 0;
	std::streampos beginIonsByteOffset;
	while(std::getline(m_inputFile, lineStr))
	{
		++lineCount;
		if(lineStr.find("BEGIN IONS") == 0)
		{
			if(inBeginIons)
			{
				cerr << "Warning: BEGIN IONS tag found without previous BEGIN IONS being closed at line " << lineCount << endl;
				for(string::reverse_iterator itr = lineStr.rbegin(); itr != lineStr.rend(); ++itr)
					m_inputFile.putback(*itr);
				break;
			}
			++curScanNum;
			beginIonsByteOffset = m_inputFile.tellg();
			inBeginIons = true;
		} else if(lineStr.find("TITLE=") == 0)
		{
			string titleStr = GetStringByKey(lineStr, "TITLE");
			boost::trim_right(titleStr);
			// TITLE=<source>.<scan>.<scan>.<charge>.dta
			if(titleStr.find(".dta", titleStr.length()-4) == titleStr.length()-4)
			{
				vector<string> tokens;
				boost::split(tokens, titleStr, boost::is_any_of("."));
				if(tokens.size() == 5)
				{
					curScanNum = lexical_cast<long>(tokens[1]);
				}
			}
		} else if(lineStr.find("SCANS=") == 0)
		{
			string scanStr = GetStringByKey(lineStr, "SCANS", ",- \r\n"); // first scan number of first range
			curScanNum = lexical_cast<long>(scanStr);
		} else if(lineStr.find("END IONS") == 0)
		{
			if(!inBeginIons)
				cerr << "Error: END IONS tag found without opening BEGIN IONS tag at line " << lineCount << endl;
			else
				m_scanIndex[curScanNum] = beginIonsByteOffset;
			inBeginIons = false;
		}
	}
}

Scan* MGFInterface::getScan(long scanNumber, bool getPeaks)
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

	m_inputFile.clear(); // clear EOF flag
	m_inputFile.seekg(m_curScanIndexItr->second);

	Scan* curScan = new Scan();
	curScan->num_ = m_curScanIndexItr->first;
	curScan->retentionTimeInSec_ = (double) m_curScanIndexItr->first / (double) m_scanIndex.rbegin()->first;
	curScan->msLevel_ = 2; // MGF only supports MS/MS unambiguously
	curScan->totalIonCurrent_ = 0; // calculate by integral
	curScan->precursorList_.push_back(PrecursorScanInfo(0, 1, 0));
	curScan->source_ = curSourceName_;
	curScan->isCentroided_ = true;

	int numPeaks = 0;
	string lineStr;
	bool inPeakList = false;
	while(std::getline(m_inputFile, lineStr))
	{
		if(lineStr.find("END IONS") == 0)
		{
			break;
		} else
		{
			if(!inPeakList)
			{
				if(lineStr.find("PEPMASS=") == 0)
				{
					string pepMassStr = GetStringByKey(lineStr, "PEPMASS");
					curScan->precursorList_.back().MZ_ = lexical_cast<Scan::MzValueType>(pepMassStr);
				} else if(lineStr.find("CHARGE=") == 0)
				{
					string pepChargeStr = GetStringByKey(lineStr, "CHARGE", "+- \r\n");
					curScan->precursorList_.back().charge_ = lexical_cast<int>(pepChargeStr);
				} else if(lineStr.find("RTINSECONDS=") == 0)
				{
					string rtStr = GetStringByKey(lineStr, "RTINSECONDS", ",- \r\n"); // first time of first range
					curScan->retentionTimeInSec_ = lexical_cast<double>(rtStr);
				} else if(lineStr.find('=') != string::npos)
				{
					continue; // an ignored attribute
				} else
				{
					inPeakList = true;
				}
			}

			if(inPeakList)
			{
				size_t delim = lineStr.find_first_of(" \t");
				if(delim == string::npos)
					continue;
				size_t delim2 = lineStr.find_first_of("\r\n", delim+1);
				if(delim2 == string::npos)
					continue;

				Scan::IntenValueType inten = lexical_cast<Scan::IntenValueType>(lineStr.substr(delim+1, delim2-delim-1));
				curScan->totalIonCurrent_ += inten;
				++numPeaks;

				if(getPeaks)
				{
					Scan::MzValueType mz = lexical_cast<Scan::MzValueType>(lineStr.substr(0, delim));
					curScan->addDataPoint(mz, inten);
				}
			}
		}
	}

	if(!getPeaks)
		curScan->setNumDataPoints(numPeaks, false);

	return curScan;
}

string GetStringByKey( const string& str, const string& key, const string& lineDelim )
{
	size_t keyIdx = str.find( key );
	while( keyIdx != string::npos )
	{
		if( ( keyIdx == 0 || str[keyIdx-1] == ' ' || str[keyIdx-1] == '\t' || lineDelim.find( str[keyIdx-1] ) != string::npos ) &&
			( str[keyIdx + key.length()] == ' ' || str[keyIdx + key.length()] == '\t' || str[keyIdx + key.length()] == '=' ) )
			break;
		keyIdx = str.find( key, keyIdx + key.length() );
	}
	if( keyIdx == string::npos )
		return "";

	size_t predIdx = str.find_first_of( '=', keyIdx ) + 1;
	size_t valIdx = str.find_first_not_of( "\t ", predIdx );
	size_t valCount = str.find_first_of( lineDelim, valIdx ) - valIdx;
	return str.substr( valIdx, valCount );
}
