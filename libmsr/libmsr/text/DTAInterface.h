#ifndef _INCLUDED_DTAINTERFACE_H_
#define _INCLUDED_DTAINTERFACE_H_

#include "common.h"
#include "BaseInstrumentInterface.h"

struct MetaDTA
{
	string filepath;
	string sourceName;
	long scanNumber;
	double dummyTime;
	vector<int> precursorCharges;
	vector<Scan::MzValueType> precursorMasses; // charged mass from file (not neutral)
	int peakCount;
	Scan::IntenValueType totalIntensity;
};

typedef map< long, MetaDTA > DTAIndex;

class DTAInterface : public BaseInstrumentInterface
{
public:
	DTAInterface()
	{
	}

	~DTAInterface()
	{
	}

	bool initInterface();
	bool setInputFile(const string& fileName, const ProcessingOptions& options);

protected:
	string m_originalInputFilepath;

	DTAIndex m_scanIndex; // maps scan number to index entry
	DTAIndex::const_iterator m_curScanIndexItr;
	//void readIndex();
	bool createIndex();

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

#endif // _INCLUDED_DTAINTERFACE_H_
