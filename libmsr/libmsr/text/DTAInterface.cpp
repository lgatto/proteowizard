#define LIBMSR_SOURCE
#include "DTAInterface.h"
#include "filesystem.h"
#include "regex.h"

#ifdef LIBMSR_NEEDS_FILESYSTEM_WORKAROUND
BOOST_FILESYSTEM_DECL filesystem::system_error_type filesystem::detail::not_found_error = ERROR_PATH_NOT_FOUND;
#endif

bool DTAInterface::initInterface()
{
	setCentroiding(true, true, true); // DTA peak lists should always be centroided
	return true;
}

bool DTAInterface::setInputFile(const string& fileName, const ProcessingOptions& options)
{
	m_originalInputFilepath = fileName;

	if(!createIndex())
		return false;

	curScanNumber_ = 0;
	totalNumScans_ = m_scanIndex.size();
	firstScanNumber_ = m_scanIndex.begin()->first;
	lastScanNumber_ = m_scanIndex.rbegin()->first;

	return true;
}

bool DTAInterface::createIndex()
{
	//**************************** source  . scan   (.*)*    .dta
	regex DTAFilenameParsingRegex("(\\S+?)\\.(\\d+)(?:\\..+)*\\.dta");

	path originalDTAFilepath(m_originalInputFilepath);
	string originalDTAFilename = originalDTAFilepath.leaf();
	smatch DTAMatch;
	bool canParse = regex_match(originalDTAFilename, DTAMatch, DTAFilenameParsingRegex);
	if(!canParse)
		return false;

	// take source name from original input filename
	curSourceName_ = (string) DTAMatch[1];

	// all sibling DTAs must have the same source name
	regex CurSourceDTAFilenameParsingRegex(curSourceName_ + "\\.(\\d+)(?:\\..+)*\\.dta");
	MetaDTA metaDTA;
	metaDTA.sourceName = curSourceName_;

	Scan::MzValueType mz;
	Scan::IntenValueType intensity;

	// Iterate through all the files in the original file's branch path;
	// Add any DTA with the same source name to the index.
	// Note: multiple DTAs with the same scan number but different precursor masses and/or charges
	//       so the logic here looks a little strange.
	for(directory_iterator itr(originalDTAFilepath.branch_path()); itr != directory_iterator(); ++itr)
	{
		const string& filename = itr->path().leaf();
		if(!regex_match(filename, DTAMatch, CurSourceDTAFilenameParsingRegex))
			continue;
		
		metaDTA.filepath = itr->path().native_file_string();
		metaDTA.scanNumber = lexical_cast<long>((string) DTAMatch[1]);
		metaDTA.peakCount = 0;
		metaDTA.totalIntensity = 0;
		metaDTA.precursorCharges.clear();
		metaDTA.precursorMasses.clear();

		ifstream DTAFile(metaDTA.filepath.c_str(), ios::binary);

		Scan::MzValueType mass;
		int charge;
		string lineStr;
		std::getline(DTAFile, lineStr);
		stringstream lineStream(lineStr);
		lineStream >> mass >> charge;
		pair<DTAIndex::iterator, bool> indexInsertPair = m_scanIndex.insert(DTAIndex::value_type(metaDTA.scanNumber, metaDTA));
		indexInsertPair.first->second.precursorMasses.push_back(mass);
		indexInsertPair.first->second.precursorCharges.push_back(charge);

		if(indexInsertPair.second) // scan not yet in index
		{
			while(DTAFile >> mz >> intensity)
			{
				++ indexInsertPair.first->second.peakCount;
				indexInsertPair.first->second.totalIntensity += intensity;
			}
		}
	}

	size_t i = 1;
	for(DTAIndex::iterator itr = m_scanIndex.begin(); itr != m_scanIndex.end(); ++itr, ++i)
		itr->second.dummyTime = (double) i / (double) m_scanIndex.size();

	return true;
}

Scan* DTAInterface::getScan(long scanNumber, bool getPeaks)
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

	const MetaDTA& m = m_curScanIndexItr->second;

	if( scanNumber == 0 )
		curScanNumber_ = m.scanNumber;

	Scan* curScan = new Scan();
	curScan->num_ = m.scanNumber;
	curScan->source_ = m.sourceName;
	curScan->retentionTimeInSec_ = m.dummyTime;
	curScan->msLevel_ = 2; // DTA only supports MS/MS unambiguously
	curScan->totalIonCurrent_ = m.totalIntensity;
	curScan->isCentroided_ = true;
	for(size_t i=0; i < m.precursorMasses.size(); ++i)
	{
		const static double protonMass = 1.007276;
		double precursorMz = m.precursorMasses[i] / m.precursorCharges[i];
		curScan->precursorList_.push_back(PrecursorScanInfo(0, 1, precursorMz, m.precursorCharges[i]));
	}
	curScan->setNumDataPoints(m.peakCount, getPeaks);

	if(getPeaks)
	{
		ifstream DTAFile(m.filepath.c_str(), ios::binary);
		int numPeaks = 0;
		string lineStr;
		std::getline(DTAFile, lineStr); // skip MZ and charge line
		Scan::MzValueType mz;
		Scan::IntenValueType intensity;
		while(DTAFile >> mz >> intensity)
		{
			curScan->mzArray_[numPeaks] = mz;
			curScan->intensityArray_[numPeaks] = intensity;
			++numPeaks;
		}
	}

	return curScan;
}
