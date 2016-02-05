#define LIBMSR_SOURCE
#include "BrukerInterface.h"
#include "MSUtilities.h"
#include "filesystem.h"

BrukerInterface::BrukerInterface()
{
}

BrukerInterface::~BrukerInterface()
{
	initInterface();
}

bool BrukerInterface::initInterface()
{
	// reset existing XML interface (detach from the intermediate XML file)
	bool rv = m_cXmlInterface.initInterface() && m_pXmlInterface.initInterface();
	if(rv)
	{
		m_cXmlInterface.setInputFile("", ProcessingOptions());
		m_pXmlInterface.setInputFile("", ProcessingOptions());
	}

	if( !m_cOutputFilepathStr.empty() && exists(m_cOutputFilepathStr) )
		filesystem::remove(m_cOutputFilepathStr);
	if( !m_pOutputFilepathStr.empty() && exists(m_pOutputFilepathStr) )
		filesystem::remove(m_pOutputFilepathStr);
	return rv;
}

bool BrukerInterface::setInputFile(const string& filename, const ProcessingOptions& options)
{
	if(!initInterface())
		return false;

	path inputFilepath(filename, filesystem::native);
	path cOutputFilepath, pOutputFilepath;
	if( extension(inputFilepath).empty() )
	{
		cOutputFilepath = path(filename + "-centroid.mzXML", filesystem::native);
		pOutputFilepath = path(filename + "-profile.mzXML", filesystem::native);
	} else
	{
		cOutputFilepath = path(filename, filesystem::native);
		cOutputFilepath = change_extension(cOutputFilepath, "-centroid.mzXML");
		pOutputFilepath = path(filename, filesystem::native);
		pOutputFilepath = change_extension(pOutputFilepath, "-profile.mzXML");
	}

	m_inputFilepathStr = inputFilepath.native_file_string();
	m_cOutputFilepathStr = cOutputFilepath.native_file_string();
	m_pOutputFilepathStr = pOutputFilepath.native_file_string();
	//std::cin.get();
	string cmd = "@\"c:\\Program Files\\Common Files\\Bruker Daltonik\\AIDA\\export\\compassxport.exe\"";
	string ccmd = cmd + " -log non -raw 0 -a \"" + m_inputFilepathStr + "\" -o \"" + m_cOutputFilepathStr + "\" >nul";
	string pcmd = cmd + " -log non -raw 1 -a \"" + m_inputFilepathStr + "\" -o \"" + m_pOutputFilepathStr + "\" >nul";
	system( ccmd.c_str() );
	system( pcmd.c_str() );
	if( !exists(m_cOutputFilepathStr) || !exists(m_pOutputFilepathStr) )
	{
		cerr << "Error: CompassXport failed to convert input file" << endl;
		return false;
	}
	//cout << "(Bruker converted file " << m_inputFilepathStr << ")" << endl;

	if( !m_cXmlInterface.setInputFile(m_cOutputFilepathStr, options) ||
		!m_pXmlInterface.setInputFile(m_pOutputFilepathStr, options) ||
		m_cXmlInterface.totalNumScans_ == 0 ||
		m_pXmlInterface.totalNumScans_ == 0 )
	{
		cerr << "Error: failed to read one or both intermediate mzXML files" << endl;
		return false;
	}

	Scan* tmp;
	while(true)
	{
		tmp = m_cXmlInterface.getScanHeader();
		if(tmp != NULL)
		{
			m_scanIndex[tmp->num_].msLevel = tmp->msLevel_;
			m_scanIndex[tmp->num_].isCentroidAvailable = true;
			delete tmp;
		} else
			break;
	}

	while(true)
	{
		tmp = m_pXmlInterface.getScanHeader();
		if(tmp != NULL)
		{
			m_scanIndex[tmp->num_].msLevel = tmp->msLevel_;
			m_scanIndex[tmp->num_].isProfileAvailable = true;
			delete tmp;
		} else
			break;
	}

	curSourceName_ = basename(m_inputFilepathStr);

	totalNumScans_ = m_cXmlInterface.totalNumScans_;
	firstScanNumber_ = m_cXmlInterface.firstScanNumber_;
	lastScanNumber_ = m_cXmlInterface.lastScanNumber_;
	curScanNumber_ = 0;

	return true;
}

void BrukerInterface::setVerbose(bool verbose)
{
	verbose_ = verbose;
	m_cXmlInterface.setVerbose(verbose);
	m_pXmlInterface.setVerbose(verbose);
}

void BrukerInterface::setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor)
{
	doCentroid_ = centroid;
	doTandemCentroid_ = tandemCentroid;
	preferVendorCentroid_ = preferVendor;
	m_pXmlInterface.setCentroiding(centroid, tandemCentroid, preferVendor);
}


void BrukerInterface::setDeisotoping(bool deisotope)
{
	doDeisotope_ = deisotope;
	m_cXmlInterface.setDeisotoping(deisotope);
	m_pXmlInterface.setDeisotoping(deisotope);
}

Scan* BrukerInterface::getScan(long scanNumber, bool getPeaks)
{
	if(scanNumber == 0)
	{
		if(curScanNumber_ < firstScanNumber_)
			curScanNumber_ = firstScanNumber_;
		else
			++curScanNumber_;
	} else
		curScanNumber_ = scanNumber;

	bool isCentroidAvailable = m_scanIndex[curScanNumber_].isCentroidAvailable;
	bool isProfileAvailable = m_scanIndex[curScanNumber_].isProfileAvailable;

	bool centroidCurScan = false;
	if( (doTandemCentroid_ && m_scanIndex[curScanNumber_].msLevel > 1) ||
		(doCentroid_ && m_scanIndex[curScanNumber_].msLevel == 1) )
		centroidCurScan = true;

	if( isCentroidAvailable &&
		((preferVendorCentroid_ && centroidCurScan) ||
		!isProfileAvailable) )
	{
		if(getPeaks)
			return m_cXmlInterface.getScan(curScanNumber_);
		else
			return m_cXmlInterface.getScanHeader(curScanNumber_);
	} else if(isProfileAvailable)
	{
		if(getPeaks)
			return m_pXmlInterface.getScan(curScanNumber_);
		else
			return m_pXmlInterface.getScanHeader(curScanNumber_);
	} else
		return NULL;
}
