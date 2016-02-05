// -*- mode: c++ -*-


/*
File: ThermoInterface.cpp
Description: Encapsulation for Thermo Xcalibur interface.
Date: July 25, 2007

Copyright (C) 2007 Joshua Tasman, ISB Seattle


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#define LIBMSR_SOURCE
#include "ThermoInterface.h"
#include "Scan.h"
#include "MSUtilities.h"
#include "filesystem.h"

ThermoInterface::ThermoInterface(void)
{
	// BaseInstrumentInterface members
	instrumentInfo_.manufacturer_ = THERMO;
	instrumentInfo_.acquisitionSoftware_ = XCALIBUR;

	instrumentInfo_.cvModel_ = CVAccession("MS:1000492");
	instrumentInfo_.cvAcquisitionSoftware_ = CVAccession("MS:1000532");

	// ThermoInterface members
	msControllerType_ = 0;
	startTimeInSec_ = -1;
	endTimeInSec_ = -1;
}

ThermoInterface::~ThermoInterface(void)
{
	xrawfile2_.Close();
}

bool ThermoInterface::initInterface(void) {
	try
	{
		// first, init MFC or return false
		if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		{
			cerr << _T("Fatal Error: MFC initialization failed") << endl;
			return false;
		}
		else {
			// Initializes the COM library on the current thread 
			// and identifies the concurrency model as single-thread 
			// apartment (STA)
			CoInitialize( NULL );

			// Creates an IDispatch interface object and attaches it to the 
			// COleDispatchDriver object
			if( !xrawfile2_.CreateDispatch( "XRawfile.XRawfile.1" , NULL ) ) {
				cerr << "Could not create XRawfile dispatch - missing Xcalibur or DLLs not registered?" << endl;
				return false;
			}
		}
	} catch( ... )
	{
		// any exception means a failed initialization
		return false;
	}

	return true;
}

bool ThermoInterface::setInputFile(const string& filename, const ProcessingOptions& options) {

	// try to open raw file or die
	if( xrawfile2_.Open(filename.c_str()) != 0 ) {
		//cerr << "Cannot open file " << filename << endl;
		return false;
	}

	//cout << "(Thermo lib opened file " << filename << ")" << endl;

	curSourceName_ = basename(path(filename, filesystem::native));

	xrawfile2_.SetCurrentController(msControllerType_, 1);
	// Get the total number of scans

	xrawfile2_.GetFirstSpectrumNumber(&firstScanNumber_);
	xrawfile2_.GetLastSpectrumNumber(&lastScanNumber_);
	//cout << "file should contain scan numbers " << firstScanNumber_ << " through " << lastScanNumber_ << endl;

	totalNumScans_ = (lastScanNumber_ - firstScanNumber_) + 1;
	curScanNumber_ = 0;

	// get the start and the end time of the run
	double startTime, endTime;
	xrawfile2_.GetStartTime(&startTime);
	xrawfile2_.GetEndTime(&endTime);
	// convert from minutes to seconds
	startTimeInSec_ = 60.0 * startTime;
	endTimeInSec_ = 60.0 * endTime;

	// get the instrument model
	BSTR bstrInstModel = NULL;
	xrawfile2_.GetInstModel(&bstrInstModel);
	if (bstrInstModel == NULL)
		throw runtime_error("failed to read instrument model from file");
	string instModel = to_upper_copy(convertBstrToString(bstrInstModel));
	SysFreeString(bstrInstModel);
	if (instModel == "LTQ") {
		instrumentInfo_.instrumentModel_ = LTQ;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000447"); // LTQ
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}
	else if (instModel == "LTQ FT") {
		instrumentInfo_.instrumentModel_ = LTQ_FT;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000557"); // LTQ FT
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}
	else if (instModel == "LTQ FT ULTRA") {
		instrumentInfo_.instrumentModel_ = LTQ_FT_ULTRA;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000447"); // LTQ FT Ultra
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}
	else if (instModel == "LTQ ORBITRAP") {
		instrumentInfo_.instrumentModel_ = LTQ_ORBITRAP;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000449"); // LTQ Orbitrap
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}
	else if (instModel == "LTQ ORBITRAP DISCOVERY") {
		instrumentInfo_.instrumentModel_ = LTQ_ORBITRAP_DISCOVERY;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000555"); // LTQ Orbitrap Discovery
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}
	else if (instModel == "LTQ ORBITRAP XL") {
		instrumentInfo_.instrumentModel_ = LTQ_ORBITRAP_XL;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000556"); // LTQ Orbitrap XL
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}
	else if (instModel == "LXQ") {
		instrumentInfo_.instrumentModel_ = LXQ;
		instrumentInfo_.manufacturer_ = THERMO_SCIENTIFIC;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000450"); // LXQ
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000494"); // Thermo Scientific
	}	
	else if (instModel == "LCQ ADVANTAGE") {
		instrumentInfo_.instrumentModel_ = LCQ_ADVANTAGE;
		instrumentInfo_.manufacturer_ = THERMO_FINNIGAN;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000167"); // LTQ Advantage
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000125"); // Thermo Finnigan
	}
	else if (instModel == "LCQ CLASSIC") {
		instrumentInfo_.instrumentModel_ = LCQ_CLASSIC;
		instrumentInfo_.manufacturer_ = THERMO_FINNIGAN;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000168"); // LCQ Classic
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000125"); // Thermo Finnigan
	}
	else if (instModel == "LCQ DECA") {
		instrumentInfo_.instrumentModel_ = LCQ_DECA;
		instrumentInfo_.manufacturer_ = THERMO_FINNIGAN;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000554"); // LCQ Deca
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000125"); // Thermo Finnigan
	}	
	else if (instModel == "LCQ DECA XP PLUS") {
		instrumentInfo_.instrumentModel_ = LCQ_DECA_XP_PLUS;
		instrumentInfo_.manufacturer_ = THERMO_FINNIGAN;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000169"); // LCQ Deca XP
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000125"); // Thermo Finnigan
	}
	else if (instModel == "LCQ FLEET") {
		instrumentInfo_.instrumentModel_ = LCQ_FLEET;
		instrumentInfo_.manufacturer_ = THERMO_FINNIGAN;

		instrumentInfo_.cvModel_ = CVAccession("MS:1000578"); // LCQ Fleet
		instrumentInfo_.cvManufacturer_ = CVAccession("MS:1000125"); // Thermo Finnigan
	}
	else if (instModel.find("TSQ QUANTUM") != string::npos) {
		instrumentInfo_.instrumentModel_ = TSQ_QUANTUM;
		instrumentInfo_.manufacturer_ = THERMO_FINNIGAN;
	}
	else {
		cerr << "unknown instrument: " << instModel << endl;
		return false;
	}


	// get instrument name
	BSTR bstrInstName = NULL;
	xrawfile2_.GetInstName(&bstrInstName);
	if (bstrInstName == NULL)
		throw runtime_error("failed to read instrument name from file");
	//cout << convertBstrToString(bstrInstName) << endl;
	instrumentInfo_.instrumentName_ = convertBstrToString(bstrInstName);
	SysFreeString(bstrInstName);

	// TODO: is this the Xcalibur or inst version?
	// get acquisition software version
	BSTR bstrAcquSoftwareVersion = NULL;
	xrawfile2_.GetInstSoftwareVersion(&bstrAcquSoftwareVersion);
	if (bstrAcquSoftwareVersion == NULL)
		throw runtime_error("failed to read instrument software version from file");
	instrumentInfo_.acquisitionSoftwareVersion_ = convertBstrToString(bstrAcquSoftwareVersion);
	SysFreeString(bstrAcquSoftwareVersion);

	// get instrument hardware version
	BSTR bstrInstHardwareVersion = NULL;
	xrawfile2_.GetInstHardwareVersion(&bstrInstHardwareVersion);
	if (bstrInstHardwareVersion == NULL)
		throw runtime_error("failed to read instrument hardware version from file");
	//cout << convertBstrToString(bstrInstHardwareVersion) << endl;
	instrumentInfo_.instrumentHardwareVersion_ = convertBstrToString(bstrInstHardwareVersion);
	SysFreeString(bstrInstHardwareVersion);

	// get instrument description
	BSTR bstrInstDescription = NULL;
	xrawfile2_.GetInstrumentDescription(&bstrInstDescription);
	if (bstrInstDescription == NULL)
		throw runtime_error("failed to read instrument description from file");
	//cout << convertBstrToString(bstrInstDescription) << endl;
	SysFreeString(bstrInstDescription);

	// get instrument serial number
	BSTR bstrInstSerialNumber = NULL;
	xrawfile2_.GetInstSerialNumber(&bstrInstSerialNumber);
	if (bstrInstSerialNumber == NULL)
		throw runtime_error("failed to read instrument serial number from file");
	//cout << convertBstrToString(bstrInstSerialNumber) << endl;
	SysFreeString(bstrInstSerialNumber);

	// get instrument ion source, analyzer for all scans to fill out instrument info
	// (thanks for not including the analyzer list in the file header, Thermo!)
	// also check if any scan is not centroided (i.e. if centroiding was done at acquisition time)

	long filterArraySize;
	VARIANT filterArray;
	VariantInit(&filterArray);
	xrawfile2_.GetFilters(&filterArray, &filterArraySize);
	if (!filterArraySize || filterArray.vt != (VT_ARRAY | VT_BSTR))
	{
		throw runtime_error("no scan filters found");
	}

	// Get a pointer to the SafeArray
	SAFEARRAY FAR* psa = filterArray.parray;
	filterArray.parray = NULL;

	BSTR* pbstrFilters = NULL;
	if (FAILED(SafeArrayAccessData( psa, (void**)(&pbstrFilters) ) ))
	{
		SafeArrayUnaccessData( psa );
		SafeArrayDestroy( psa );
		throw runtime_error("failed to read scan filters");
	}

	bool centroidedAtAcquisition = true;
	for (long i=0; i < filterArraySize; ++i)
	{
		string filterString = convertBstrToString(pbstrFilters[i]);
		FilterLine filterLine;
		if (!filterLine.parse(filterString))
			throw runtime_error(string("failed to parse filter line: ") + filterString);

		// if this scan's analyzer is not in the instrument's analyzer list, add it
		if (std::find(	instrumentInfo_.analyzerList_.begin(),
						instrumentInfo_.analyzerList_.end(),
						filterLine.analyzer_) == instrumentInfo_.analyzerList_.end())
		{
			instrumentInfo_.analyzerList_.push_back(filterLine.analyzer_);
		}

		instrumentInfo_.ionSource_ = filterLine.ionizationMode_;

		if(centroidedAtAcquisition && filterLine.scanData_ != CENTROID)
			centroidedAtAcquisition = false;
	}

	if(centroidedAtAcquisition) {
		doCentroid_ = true;
		doTandemCentroid_ = true;
	}

	// Delete the SafeArray
	SafeArrayUnaccessData( psa );
	SafeArrayDestroy( psa );

	// get time from file
	DATE date = NULL;
	xrawfile2_.GetCreationDate(&date);
	//instrumentInfo_.runTime_ = ""; // FIX
	//CTime c(date);
	COleDateTime dateTime(date);
	string dateStr = dateTime.Format("%Y-%m-%dT%H:%M:%S");
	timeStamp_ = dateStr;

	return true;
}

void ThermoInterface::setVerbose(bool verbose) {
	verbose_ = verbose;
}

void ThermoInterface::setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor) {
	doCentroid_ = centroid;
	doTandemCentroid_ = tandemCentroid;
	preferVendorCentroid_ = preferVendor;
}


void ThermoInterface::setDeisotoping(bool deisotope) {
	doDeisotope_ = deisotope;
}

Scan* ThermoInterface::getScan(long scanNumber, bool getPeaks) {

	if (scanNumber == 0) {
		if (curScanNumber_ < firstScanNumber_)
			curScanNumber_ = firstScanNumber_;
		else
			++curScanNumber_;
	} else
		curScanNumber_ = scanNumber;

	// Get the scan filter
	// (ex: "ITMS + c NSI Full ms [ 300.00-2000.00]")
	BSTR bstrFilter = NULL;
	xrawfile2_.GetFilterForScanNum(curScanNumber_, &bstrFilter);
	if (bstrFilter == NULL) // scan filter not found
		return NULL;
	string filterLineStr = convertBstrToString(bstrFilter);
	FilterLine filterLine;
	if (!filterLine.parse(filterLineStr)) {
		cerr << "error parsing filter line. exiting." << endl;
		cerr << "line: " << filterLineStr << endl;
		exit(-1);
	}

	Scan* curScan = new Scan();
	curScan->isThermo_ = true;
	curScan->source_ = curSourceName_;
	curScan->num_ = curScanNumber_;
	curScan->thermoFilterLine_ = filterLineStr;

	// we should now have:
	// msLevel
	// polarity
	// scanType (full, zoom, etc)
	// ionization type
	// analyzer
	// scan range
	// parent mass and CID energy, if MS >=2 

	// record msLevel from filter line
	curScan->msLevel_ = filterLine.msLevel_;

	// record scan ranges from filter line
	curScan->startMZ_ = filterLine.scanRangeMin_.back();
	curScan->endMZ_ = filterLine.scanRangeMax_.back();
	for(size_t i=0; i < filterLine.scanRangeMin_.size(); ++i)
		curScan->mzRanges_.push_back(Scan::MzRange(filterLine.scanRangeMin_[i], filterLine.scanRangeMax_[i]));

	// record polarity from filter line
	curScan->polarity_ = filterLine.polarity_;

	// record analyzer from filter line
	curScan->analyzer_ = filterLine.analyzer_;

	// record ionization from filter line
	curScan->ionization_ = filterLine.ionizationMode_;

	// record scan type from filter line
	// (zoom, full, srm, etc)
	curScan->scanType_ = filterLine.scanType_;

	// record activation (CID, etc)
	curScan->activation_ = filterLine.activationMethod_;


	// get additional header information through Xcalibur:
	// retention time
	// min/max observed mz
	// total ion current
	// base peak mz and intensity

	long numDataPoints = -1; // points in both the m/z and intensity arrays
	double retentionTimeInMinutes = -1;
	double minObservedMZ, maxObservedMZ, totalIonCurrent, basePeakMZ, basePeakIntensity;
	long channel; // unused
	long uniformTime; // unused
	double frequency; // unused

	xrawfile2_.GetScanHeaderInfoForScanNum(
		curScan->num_, 
		&numDataPoints, 
		&retentionTimeInMinutes, 
		&minObservedMZ,
		&maxObservedMZ,
		&totalIonCurrent,
		&basePeakMZ,
		&basePeakIntensity,
		&channel, // unused
		&uniformTime, // unused
		&frequency // unused
		);

	// NOTE! the returned numDataPoints is the number of data packets, not points!
	// use the value from GetMassListFromScanNum below

	// record the retention time
	curScan->retentionTimeInSec_ = retentionTimeInMinutes * 60.0;
	curScan->minObservedMZ_ = (Scan::MzValueType) minObservedMZ;
	curScan->maxObservedMZ_ = (Scan::MzValueType) maxObservedMZ;
	curScan->totalIonCurrent_ = (Scan::IntenValueType) totalIonCurrent;
	curScan->basePeakMZ_ = (Scan::MzValueType) basePeakMZ;
	curScan->basePeakIntensity_ = (Scan::IntenValueType) basePeakIntensity;

	// if ms level 2 or above, get precursor info
	
	if (curScan->msLevel_ > 1) {
		curScan->precursorList_.push_back( PrecursorScanInfo(0, curScan->msLevel_-1, 0) ); // default precursor info
		getPrecursorInfo(*curScan, filterLine);
	}


	//
	// get the m/z intensity pairs list for the current scan
	// !and correct min/max observed m/z info here!
	//

	//if( curScan->getNumDataPoints() != 0 ) {
	// cout << "reading data points for scan " << curScanNumber_ << endl;

	long scanNum = curScan->num_;

	// record centroiding info
	//
	// scan may have been centroided at accquision time,
	// rather than conversion time (now)
	// even if user didn't request it.
	bool centroidCurScan = preferVendorCentroid_ &&
		(curScan->msLevel_ == 1 && doCentroid_) || (curScan->msLevel_ > 1 && doTandemCentroid_);

	if (filterLine.scanData_ == CENTROID || centroidCurScan )
			curScan->isCentroided_ = true;

	if (getPeaks)
	if (centroidCurScan && curScan->analyzer_ == FTMS)
	{
		// use GetLabelData to workaround bug in Thermo centroiding of FT profile data

		VARIANT	varLabels;
		VariantInit(&varLabels);

		VARIANT varPeakFlags; // unused
		// initialize variant to VT_EMPTY
		VariantInit(&varPeakFlags);

		xrawfile2_.GetLabelData(&varLabels, &varPeakFlags, &scanNum);
		int dataPoints = varLabels.parray->rgsabound[0].cElements;

		// record the number of data point (allocates memory for arrays)
		curScan->setNumDataPoints(dataPoints, getPeaks);

		if(getPeaks)
		{
			double* pdval = (double*) varLabels.parray->pvData;
			for(int i=0; i < dataPoints; ++i)
			{
				curScan->mzArray_[i] = (Scan::MzValueType) pdval[(i*6)+0];
				curScan->intensityArray_[i] = (Scan::IntenValueType) pdval[(i*6)+1];
			}
		}

		VariantClear(&varLabels); // Delete all memory associated with the variant
		VariantClear(&varPeakFlags); // and reinitialize to VT_EMPTY

	} else
	{
		VARIANT varMassList;
		// initiallize variant to VT_EMPTY
		VariantInit(&varMassList);

		VARIANT varPeakFlags; // unused
		// initiallize variant to VT_EMPTY
		VariantInit(&varPeakFlags);

		// set up the parameters to read the scan
		// TODO make centroid parameter user customizable
		long dataPoints = 0;
		LPCTSTR szFilter = NULL;		// No filter
		long intensityCutoffType = 0;	// No cutoff
		long intensityCutoffValue = 0;	// No cutoff
		long maxNumberOfPeaks = 0;		// 0 : return all data peaks
		double centroidPeakWidth = 0;	// No centroiding

		xrawfile2_.GetMassListFromScanNum(
			&scanNum,
			szFilter,			 // filter
			intensityCutoffType, // intensityCutoffType
			intensityCutoffValue, // intensityCutoffValue
			maxNumberOfPeaks,	 // maxNumberOfPeaks
			centroidCurScan,	// centroidResult?
			&centroidPeakWidth,	// centroidingPeakWidth
			&varMassList,		// massList
			&varPeakFlags,		// peakFlags
			&dataPoints);		// array size

		// record the number of data point (allocates memory for arrays)
		curScan->setNumDataPoints(dataPoints, getPeaks);

		if(getPeaks)
		{
			// Get a pointer to the SafeArray
			SAFEARRAY FAR* psa = varMassList.parray;
			DataPeak* pDataPeaks = NULL;
			SafeArrayAccessData(psa, (void**)(&pDataPeaks));

			// record mass list information in scan object
			for (long j=0; j < dataPoints; j++) {
				curScan->mzArray_[j] = pDataPeaks[j].dMass;
				curScan->intensityArray_[j] = pDataPeaks[j].dIntensity;
			}

			// cleanup
			SafeArrayUnaccessData(psa); // Release the data handle
		}

		VariantClear(&varMassList); // Delete all memory associated with the variant
		VariantClear(&varPeakFlags); // and reinitialize to VT_EMPTY

		if( varMassList.vt != VT_EMPTY ) {
			SAFEARRAY FAR* psa = varMassList.parray;
			varMassList.parray = NULL;
			SafeArrayDestroy( psa ); // Delete the SafeArray
		}

		if(varPeakFlags.vt != VT_EMPTY ) {
			SAFEARRAY FAR* psa = varPeakFlags.parray;
			varPeakFlags.parray = NULL;
			SafeArrayDestroy( psa ); // Delete the SafeArray
		}
	}

	// Fix to overcome bug in ThermoFinnigan library GetScanHeaderInfoForScanNum() function
	if(curScan->mzArray_.size() > 0) {
		curScan->minObservedMZ_ = curScan->mzArray_.front();
		curScan->maxObservedMZ_ = curScan->mzArray_.back();
	} else {
		curScan->minObservedMZ_ = 0;
		curScan->maxObservedMZ_ = 0;
	}

	if(!curScan->isCentroided_ &&
		((curScan->msLevel_ == 1 && doCentroid_) || (curScan->msLevel_ > 1 && doTandemCentroid_)))
	{
		doCentroidScan(curScan);
	}

	return curScan;
}




// get precursor m/z, collision energy, precursor charge, and precursor intensity
void
ThermoInterface::getPrecursorInfo(Scan& scan, FilterLine& filterLine) {

	VARIANT varValue;
	VariantInit(&varValue);

	//
	// precursor m/z
	//

	// we'll try to get the "accurate mass" recorded by the machine, and fall back to the filter line value otherwise.
	//scan.accuratePrecursorMZ_ = false;
	PrecursorScanInfo& precursorScanInfo = scan.precursorList_.back();

	// see if we can get an accurate value from the machine

	double precursorMZ = 0;
	if (xrawfile2_.GetTrailerExtraValueForScanNum((long)scan.num_, "Monoisotopic M/Z:" , &varValue)) { 
		if( varValue.vt == VT_R4 ) {
			precursorMZ = varValue.fltVal;
		}
		else if( varValue.vt == VT_R8 ) {
			precursorMZ = varValue.dblVal;
		}
		else if ( varValue.vt != VT_ERROR ) {
			cerr << "Scan: " << scan.num_ << " MS level: " << scan.msLevel_ 
				<< " unexpected type when looking for precursorMz\n";
			exit(-1);
		}
	}
	if( precursorMZ != 0 ) {
		// success-- higher accuracy m/z recorded by machine
		accurateMasses_++;
		precursorScanInfo.MZ_ = precursorMZ;
		precursorScanInfo.accurateMZ_ = true;
	}
	else { 
		// use the low-precision parent mass in the filter line
		inaccurateMasses_++;
		precursorScanInfo.MZ_ = filterLine.cidParentMass_.back();
		precursorScanInfo.accurateMZ_ = false;			
	}

	//
	// collision energy, trying first from the machine
	//

	double collisionEnergy = 0;
	VariantClear(&varValue);
	if (xrawfile2_.GetTrailerExtraValueForScanNum((long)scan.num_, "API Source CID Energy:" , &varValue)) {
		if( varValue.vt == VT_R4 ) {
			// VT_R4: OLE float type
			collisionEnergy = varValue.fltVal;
		}		
		else if ( varValue.vt != VT_ERROR ) {
			cerr << "Unexpected type when looking for CE\n";
			exit(-1);
		}
	}
	if (collisionEnergy != 0) {
		// sucess-- collision energy record by machine
		precursorScanInfo.collisionEnergy_ = collisionEnergy;
	}
	else {
		// use the low-precision collision energy recorded in the filter line
		precursorScanInfo.collisionEnergy_ = filterLine.cidEnergy_.back();
	}


	// precursor charge state, again trying from the machine
	VariantClear(&varValue);
	int precursorCharge = 0;
	if (xrawfile2_.GetTrailerExtraValueForScanNum((long)scan.num_, "Charge State:" , &varValue)) {
		// First try to use the OCX
		if( varValue.vt == VT_I2 ) {
			precursorCharge = varValue.iVal;
		}
	}

	precursorScanInfo.charge_ = precursorCharge;
	chargeCounts_[precursorCharge]++; // with 0 being undetermined

	// JKE: don't look for info in prev scan if this is the first scan
	if( scan.num_ == firstScanNumber_ )
		return;

	//
	// precursor intensity determination
	//

	// go to precursor scan and try to find highest intensity around precursor m/z.
	// (could this be improved to handle more complex acquisition methods?)

	//if( numDataPoints != 0 ) { // if this isn't an empty scan
	/*VARIANT varMassList;
	VariantInit(&varMassList);	// initiallize variant to VT_EMPTY
	VARIANT varPeakFlags; // unused
	VariantInit(&varPeakFlags);	// initiallize variant to VT_EMPTY



	// set up the parameters to read the precursor scan
	long numPrecursorDataPoints = 0;

	// MCC: this is wrong for MS levels greater than 2:
	// LPCTSTR szFilter = "!d";		// First previous not-dependent scan

	// First previous scan with an MS level one less than the current one
	string precursorScanFilter = "ms" + (scan.msLevel_ > 2 ? lexical_cast<string>(scan.msLevel_-1) : "");
	long intensityCutoffType = 0;	// No cutoff
	long intensityCutoffValue = 0;	// No cutoff
	long maxNumberOfPeaks = 0;		// Return all data peaks
	BOOL centroidResult = FALSE;	// No centroiding of the precursor
	double centroidPeakWidth = 0;	// (see above: no centroiding)

	// cout << "reading scan " << scan.num_ << endl;

	long curScanNum = scan.num_;

	// the goal is to get the parent scan's info
	xrawfile2_.GetPrevMassListFromScanNum(
		&curScanNum,
		precursorScanFilter.c_str(),	// filter
		intensityCutoffType,			// intensityCutoffType
		intensityCutoffValue,			// intensityCutoffValue
		maxNumberOfPeaks,				// maxNumberOfPeaks
		centroidResult,					// centroidResult
		&centroidPeakWidth,				// centroidingPeakWidth
		&varMassList,					// massList
		&varPeakFlags,					// peakFlags -- unused
		&numPrecursorDataPoints);		// arraySize

	// record the precursor scan number
	precursorScanInfo.num_ = curScanNum; // set during last xrawfile2 call

	// numPrecursorDataPoints: number of mass/intensity pairs for precursor scan
	// if empty, no precursor scan
	if (numPrecursorDataPoints != 0) {
		// Get a pointer to the SafeArray
		SAFEARRAY FAR* psa = varMassList.parray;
		DataPeak* pDataPeaks = NULL;
		// use our pDataPeaks array to access the SafeArray data.
		SafeArrayAccessData(psa, (void**)(&pDataPeaks) );
		double precursorIntensity = 0;

		// Find most intense peak around precursorMz:		
		// search the precursor's scan data near precursorMZ
		// to get the intensity.
		for (long j=0; j<numPrecursorDataPoints; j++) {
			double dMass = pDataPeaks[j].dMass;

			if (fabs(precursorScanInfo.MZ_ - dMass) < 0.05 ) {
				if( pDataPeaks[j].dIntensity > precursorIntensity ) {
					precursorScanInfo.intensity_ = pDataPeaks[j].dIntensity;
				}
			}
			// stop if we've gone too far
			if (dMass - precursorScanInfo.MZ_ > 0.05) {
				break;
			}
		}
		// record the precursor intensity
		precursorScanInfo.intensity_ = precursorIntensity;

		// Release the data handle
		SafeArrayUnaccessData(psa);
		// Delete all memory associated with the variant and
		// reinitialize to VT_EMPTY
		VariantClear(&varMassList);	
		VariantClear(&varPeakFlags);
	}

	// double check on deallocating memory
	if (varMassList.vt != VT_EMPTY) {
		SAFEARRAY FAR* psa = varMassList.parray;
		varMassList.parray = NULL;
		// Delete the SafeArray
		SafeArrayDestroy( psa );
	}

	if (varPeakFlags.vt != VT_EMPTY) {
		SAFEARRAY FAR* psa = varPeakFlags.parray;
		varPeakFlags.parray = NULL;
		// Delete the SafeArray
		SafeArrayDestroy(psa);
	}*/
}