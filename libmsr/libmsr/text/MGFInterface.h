#ifndef _INCLUDED_MGFINTERFACE_H_
#define _INCLUDED_MGFINTERFACE_H_

#include "common.h"
#include "BaseInstrumentInterface.h"

typedef map< long, std::streampos > MGFIndex;

class MGFInterface : public BaseInstrumentInterface
{
public:
	MGFInterface()
	{
	}

	~MGFInterface()
	{
	}

	bool initInterface();
	bool setInputFile(const string& fileName, const ProcessingOptions& options);

protected:
	string		m_inputFilename;
	ifstream	m_inputFile;

	MGFIndex m_scanIndex; // maps scan number to index entry
	MGFIndex::const_iterator m_curScanIndexItr;
	void readIndex();
	void createIndex();

	Scan* getScan(long scanNumber, bool getPeaks);

public:
	void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor) {}
	void setDeisotoping(bool deisotope) {}
	void setCompression(bool compression) {}
	void setProcessingOptions(const ProcessingOptions& options) {}

	void setVerbose(bool verbose)
	{
		verbose_ = verbose;
	}

	Scan* getScan()							{ return getScan(0, true); }
	Scan* getScanHeader()					{ return getScan(0, false); }

	Scan* getScan(long scanNumber)			{ return getScan(scanNumber, true); }
	Scan* getScanHeader(long scanNumber)	{ return getScan(scanNumber, false); }
};

string GetStringByKey( const string& str, const string& key, const string& lineDelim = " \r\n" );

#endif // _INCLUDED_MGFINTERFACE_H_
