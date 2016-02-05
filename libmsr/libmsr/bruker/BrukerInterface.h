#ifndef _INCLUDED_BRUKERINTERFACE_H_
#define _INCLUDED_BRUKERINTERFACE_H_

#include "common.h"
#include "CompassXport2.h"
#include "BaseInstrumentInterface.h"
#include "../xml/mzXMLInterface.h"

struct BrukerScanIndexEntry { int msLevel; bool isCentroidAvailable; bool isProfileAvailable; };
typedef map< long, BrukerScanIndexEntry > BrukerScanIndex;

class BrukerInterface : public BaseInstrumentInterface {
private:
	// COleDispatchDriver object instance to CompassXport2 dll
	//ICompassXport2 compassXport2_;
	mzXMLInterface m_cXmlInterface; // interface to centroided intermediate data
	mzXMLInterface m_pXmlInterface; // interface to profile-mode intermediate data

	bool preferVendorCentroid_;
	BrukerScanIndex m_scanIndex; // keep track of scan availability in the XML files

	string m_inputFilepathStr;
	string m_cOutputFilepathStr;
	string m_pOutputFilepathStr;

public:
	BrukerInterface();
	~BrukerInterface();

	bool initInterface();
	bool setInputFile(const string& fileName, const ProcessingOptions& options);
	void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor);
	void setDeisotoping(bool deisotope);
	void setVerbose(bool verbose);

	Scan* getScan()							{ return getScan(0, true); }
	Scan* getScanHeader()					{ return getScan(0, false); }

	Scan* getScan(long scanNumber)			{ return getScan(scanNumber, true); }
	Scan* getScanHeader(long scanNumber)	{ return getScan(scanNumber, false); }

private:
	Scan* getScan(long scanNumber, bool getPeaks);
};

#endif // _INCLUDED_BRUKERINTERFACE_H_
