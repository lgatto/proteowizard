#ifndef _INCLUDED_INSTRUMENTINTERFACE_H_
#define _INCLUDED_INSTRUMENTINTERFACE_H_

#include "common/common.h"
#include "common/BaseInstrumentInterface.h"

class LIBMSR_DECL InstrumentInterface
{
	class InstrumentInterfaceImpl* pimpl_;

public:
	InstrumentInterface();
	~InstrumentInterface();

	BaseInstrumentInterface* getBaseInterface() const;

	bool initInterface();
	void freeInterface();

	bool setInputFile(const string& fileName, const ProcessingOptions& options = ProcessingOptions());

	static ProcessingOptions getDefaultProcessingOptions(const string& formatExtension);
	void setProcessingOptions(const ProcessingOptions& options);
	void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor = false);
	void setDeisotoping(bool deisotope);
	void setCompression(bool compression);
	void setVerbose(bool verbose);

	long getFirstScanNumber();
	long getLastScanNumber();
	long getTotalScanCount();

	Scan* getScan(); // returns next available scan (first, initally)
	Scan* getScanHeader(); // same as above, but without populating the peak arrays

	Scan* getScan(long scanNumber); // returns requested scan by number, NULL if unavailable
	Scan* getScanHeader(long scanNumber); // same as above, but without populating the peak arrays
};

#endif
