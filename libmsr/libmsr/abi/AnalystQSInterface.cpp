// -*- mode: c++ -*-


/*
    File: AnalystQSInterface.cpp
    Description: Implementation of instrument interface for data acquired 
                 with AnalystQS.  The program depends on the general framework 
                 developed by Joshua Tasman, ISB Seattle
    Date: July 31, 2007

    Copyright (C) 2007 Chee Hong WONG, Bioinformatics Institute


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
#include "common.h"
#include "AnalystQSInterface.h"
#include "Scan.h"
#include "MSUtilities.h"
#include "filesystem.h"

#include <float.h>	// _controlfp
#include <math.h>	// fabs

using namespace AnalystQS;

AnalystQSInterface::AnalystQSInterface(void) :
	m_fInitialized(false),
	m_fCOMInitialized(false),
	m_enumLibraryType(LIBRARY_AnalystQS),
	m_pScan(NULL),
	m_dChromotographRTInSec(-1),
	m_fUseExperiment0RT(true),
	m_iVerbose(0)
{
	instrumentInfo_.manufacturer_ = ABI_SCIEX;
	instrumentInfo_.analyzerList_.push_back(ANALYZER_UNDEF);

	// AnalystQSInterface members
	startTimeInSec_ = -1;
	endTimeInSec_ = -1;

	firstTime_ = true;
}

AnalystQSInterface::~AnalystQSInterface(void)
{
	termInterface();
}

void AnalystQSInterface::setVerbose(int iVerbose) {
	m_iVerbose = iVerbose;
}

bool AnalystQSInterface::initInterface(void) {
	// Initializes the COM library on the current thread 
	// and identifies the concurrency model as single-thread 
	// apartment (STA)
	if (!m_fCOMInitialized) {
		if (S_OK==CoInitializeEx( NULL , COINIT_APARTMENTTHREADED)) {
			m_fCOMInitialized = true;
		}
	}

	//----------------------------------------

	m_paramsString=SysAllocString(L"");

	HRESULT hr;

	hr = m_ipFMANChromData.CreateInstance(__uuidof(FMANChromData));
	if (FAILED(hr)) {
		LPOLESTR lpolestrCLSID;
		StringFromCLSID(__uuidof(FMANChromData), &lpolestrCLSID);
		CW2A pszACLSID( lpolestrCLSID );
		std::cerr << "Could not create FMANChromData" << pszACLSID << " instance, hr("
			<< std::hex << hr << std::dec << ")" << std::endl;
		CoTaskMemFree(lpolestrCLSID);

		return false;
	}

	hr = m_ipFMANSpecData.CreateInstance(__uuidof(FMANSpecData));
	if (FAILED(hr)) {
		LPOLESTR lpolestrCLSID;
		StringFromCLSID(__uuidof(FMANSpecData), &lpolestrCLSID);
		CW2A pszACLSID( lpolestrCLSID );
		std::cerr << "Could not create FMANSpecData" << pszACLSID << " instance, hr("
			<< std::hex << hr << std::dec << ")" << std::endl;
		CoTaskMemFree(lpolestrCLSID);

		return false;
	}

	hr = m_ipFMANSpecDataToMerge.CreateInstance(__uuidof(FMANSpecData));
	if (FAILED(hr)) {
		LPOLESTR lpolestrCLSID;
		StringFromCLSID(__uuidof(FMANSpecData), &lpolestrCLSID);
		CW2A pszACLSID( lpolestrCLSID );
		std::cerr << "Could not create FMANSpecData" << pszACLSID << " instance for merging, hr("
			<< std::hex << hr << std::dec << ")" << std::endl;
		CoTaskMemFree(lpolestrCLSID);

		return false;
	}

	hr = m_ipPeakFinderFactory.CreateInstance(__uuidof(PeakFinderFactory));
	if (FAILED(hr)) {
		LPOLESTR lpolestrCLSID;
		StringFromCLSID(__uuidof(PeakFinderFactory), &lpolestrCLSID);
		CW2A pszACLSID( lpolestrCLSID );
		std::cerr << "Could not create PeakFinderFactory" << pszACLSID << " instance, hr("
			<< std::hex << hr << std::dec << ")" << std::endl;
		CoTaskMemFree(lpolestrCLSID);

		return false;
	}

	// this initialization must be the last statement
	// statement which can fail should not be done after initialization
	m_fInitialized = true;

	return true;
}

bool AnalystQSInterface::setInputFile(const std::string& filename, const ProcessingOptions& options) {
	//----------------------------------------
	{
		// check that we are using the right software to read the data
		std::string strLibraryVersion;
		int nSWRet = getLibraryVersion (m_enumLibraryType, strLibraryVersion);
		if (0 != nSWRet) {
			// We do not proceed as the library DLLs are not available
			std::cerr << "Error: "<<getLibraryName(m_enumLibraryType)<<" library is not installed." << std::endl;
			return false;
		} else {
			if (m_iVerbose>=VERBOSE_INFO) {
				std::cout << "INFO: " << getLibraryName(m_enumLibraryType);
				if (!strLibraryVersion.empty()) {
					std::cout << " Version " << strLibraryVersion;
				}
				std::cout << " is installed." << std::endl;
			}
		}

		std::string strCreator;
		int nFileRet = getCreator (filename, strCreator);
		if (0 == nFileRet) {
			if (isCompatible(m_enumLibraryType,strCreator)) {
				// the file is created with "Analyst QS" software
				if (m_iVerbose>=VERBOSE_INFO) {
					std::cout << "INFO: " << filename.c_str() << " is created with " << strCreator.c_str() << std::endl;
				}
			} else {
				// we generally cannot read file created with Analyst
				// but seem to be be okay with "FMAN MAC" and "FMAN NT"
				// We will just warn the user if there is potential compatibility issue
				std::cerr << "WARNING: " << filename.c_str() << " is created with " << strCreator.c_str() << std::endl;
				std::cerr << "WARNING: There could be compatibility issue accessing it with "<<getLibraryName(m_enumLibraryType)<<" library" << std::endl;
			}

		} else {
			// we can't tell what software the file is created with
			std::cerr << "WARNING: Fail to retrieve " << filename.c_str() << "'s creator software." << std::endl;
			std::cerr << "WARNING: Skipping compatibility check" << std::endl;
		}
	}
	
	curSourceName_ = basename(path(filename, filesystem::native));

	// cached the filename for buildAcquisitionSoftwareInfo
	m_strAcquisitionFileName = filename;
	m_bstrWiffFileName = filename.c_str();
	m_ipFMANChromData->WiffFileName = m_bstrWiffFileName;

	IUnknownPtr ipUnknown(m_ipFMANChromData->GetWiffFileObject());
	ipUnknown->QueryInterface(&m_ipFMANWiffFile);

	m_ipFMANSpecData->WiffFileName = m_bstrWiffFileName;
	m_ipFMANSpecDataToMerge->WiffFileName = m_bstrWiffFileName;

	m_DPSettings.init(options);

	buildInstrumentInfo ();
	buildAcquisitionSoftwareInfo ();

	getMSRunTime();
	computeScansCount();

	sha1_.Reset();
	sha1Report_[0] = 0;
	if (0!=m_DPSettings.m_dGroupPrecursorMassTolerance) {
		if ( !(sha1_.HashFile(m_strAcquisitionFileName.c_str()))) {
			std::cerr << "ERRORO: Cannot open file '" << m_strAcquisitionFileName.c_str() << "' for sha-1 calculation" <<std::endl;
			return false;// Cannot open file for sha1
		}
		sha1_.Final();
		sha1_.ReportHash(sha1Report_, SHA1::REPORT_HEX);
	}

	//WCH: this is not necessary as getFirstScan() will have done so via getScan()
	//initScanIteratorState();

	return true;
}

void AnalystQSInterface::setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor) {
	doCentroid_ = centroid;
	doTandemCentroid_ = tandemCentroid;
	preferVendorCentroid_ = preferVendor;
}

void AnalystQSInterface::setDeisotoping(bool deisotope) {
	doDeisotope_ = deisotope;
}

Scan* AnalystQSInterface::getScan(void) {
	if (firstTime_) {
		firstTime_ = false;
		return getFirstScan();
	} else {
		return getNextScan();
	}
}

void AnalystQSInterface::termInterface(void) {

	if (m_fCOMInitialized) {
		SysFreeString(m_paramsString);

		m_ipFMANChromData = NULL;
		m_ipFMANSpecData = NULL;
		m_ipFMANWiffFile = NULL;
		m_ipPeakFinderFactory = NULL;
		m_ipPeakFinder = NULL;
		m_ipFMANSpecDataToMerge = NULL;

		m_fInitialized = false;

		CoUninitialize();
		m_fCOMInitialized = false;
	}
}

//----------

int AnalystQSInterface::getCreator(const std::string &strFilename, std::string &strCreator) {

	IFMANWiffFilePtr ipFMANWiffFile;
	HRESULT hr = ipFMANWiffFile.CreateInstance(__uuidof(FMANWiffFile));
	if (FAILED(hr)) {
		LPOLESTR lpolestrCLSID;
		StringFromCLSID(__uuidof(FMANWiffFile), &lpolestrCLSID);
		CW2A pszACLSID( lpolestrCLSID );
		std::cerr << "Could not create FMANWiffFile" << pszACLSID << " instance, hr("
			<< std::hex << hr << std::dec <<")" << std::endl;
		CoTaskMemFree(lpolestrCLSID);
		return -1;
	}

	_bstr_t wiffFileName(strFilename.c_str());
	hr = ipFMANWiffFile->OpenWiffFile(wiffFileName);
	if (FAILED(hr)) {
		std::cerr << "ipFMANWiffFile->OpenWiffFile(" << strFilename.c_str() << ") returns " 
			<< std::hex << hr << std::dec << std::endl;
		return -1;
	}

	int nRet = -1;

	IUnknownPtr ipUnknownFileManagerObject;
	ipUnknownFileManagerObject = ipFMANWiffFile->GetFileManagerObject();

	IFMWIFFPtr ipIFMWIFF;
	hr = ipUnknownFileManagerObject->QueryInterface(&ipIFMWIFF);
	if (SUCCEEDED(hr)) {
		unsigned long dwFileHandle=0;
		hr = ipIFMWIFF->FMANGetWIFFDriverFileHandle(&dwFileHandle);
		if (SUCCEEDED(hr)) {
			IUnknownPtr ipUnknownWTS;
			hr = ipIFMWIFF->FMANWIFFGetWTS (&ipUnknownWTS);
			if (SUCCEEDED(hr)) {
				IWIFFTransServerPtr ipIWIFFTransServer(ipUnknownWTS);

				void *pObject=NULL;
				hr = ipIWIFFTransServer->QueryInterface(__uuidof(IWIFFTSvrFileRec_Str), &pObject);
				if (SUCCEEDED(hr)) {
					IWIFFTSvrFileRec_StrPtr ipIWIFFTSvrFileRec_Str((IWIFFTSvrFileRec_Str*)pObject);

					long lStreamDataObject=0;
					hr = ipIWIFFTSvrFileRec_Str->OpenArrStrCLS_FileRec_Str(dwFileHandle, &lStreamDataObject);
					if (SUCCEEDED(hr)) {

						LPWSTR lpwstr=NULL;
						hr = ipIWIFFTSvrFileRec_Str->raw_GetFieldCLS_csSignature(dwFileHandle, &lpwstr);
						if (SUCCEEDED(hr)) {
							USES_CONVERSION;
							strCreator = W2A(lpwstr);
							nRet = 0;
						} else {
							std::cerr << "IWIFFTSvrFileRec_Str->raw_GetFieldCLS_csSignature() returns " 
								<< std::hex << hr << std::dec << std::endl;
						}
						::CoTaskMemFree(lpwstr);

						hr = ipIWIFFTSvrFileRec_Str->CloseArrStrCLS_FileRec_Str(lStreamDataObject);
						if (FAILED(hr)) {
							std::cerr << "IWIFFTSvrFileRec_Str->CloseArrStrCLS_FileRec_Str() returns " 
								<< std::hex << hr << std::dec << std::endl;
						}
					} else {
						std::cerr << "IWIFFTSvrFileRec_Str->OpenArrStrCLS_FileRec_Str() returns " 
							<< std::hex << hr << std::dec << std::endl;
					}
				}
			} else {
				std::cerr << "IFMWIFF->FMANWIFFGetWTS() returns " 
					<< std::hex << hr << std::dec << std::endl;
			}
		} else {
			std::cerr << "IFMWIFF->FMANGetWIFFDriverFileHandle() returns " 
				<< std::hex << hr << std::dec << std::endl;
		}
	}

	hr = ipFMANWiffFile->CloseWiffFile();
	if (FAILED(hr)) {
		std::cerr << "ipFMANWiffFile->CloseWiffFile(" << strFilename.c_str() << ") returns " 
			<< std::hex << hr << std::dec << std::endl;
	}

	return nRet;
}

// Return the mass spectrometer manufacturer (return 0 as success)
void AnalystQSInterface::getMSManufacturer(std::string &strManufacturer) {

	strManufacturer.resize(0);

	getSampleLog();

	getSampleLogEntry (m_strSampleLog, "Manufacturer: ", strManufacturer);
	std::string::size_type nStartPos = strManufacturer.find_first_of(",\r\n");
	if (strManufacturer.npos != nStartPos) {
		strManufacturer.erase (nStartPos);
	}

	if (strManufacturer.empty()) {
		// hardcoded to "ABI" as the information seems unavailable from file
		strManufacturer = "ABI";
	}
}

// Return the mass spectrometer model (return 0 as success)
void AnalystQSInterface::getMSModel(long &lModelId, std::string &strModel) {
	strModel.resize(0);

	// TODO: figure out how to incorporate m_lCurrentSampleIndex
	try {
		IUnknownPtr ipUnknown(m_ipFMANWiffFile->GetMassSpecMethod(1));
		IMassSpecMethodPtr ipMassSpecMethod;
		HRESULT hr = ipUnknown->QueryInterface(&ipMassSpecMethod);
		long numQuads;
		BSTR configVer=NULL;
		ipMassSpecMethod->GetMassSpecConfigInfo(&numQuads, &lModelId, &configVer);
		if (NULL!=configVer) {
			SysFreeString(configVer);
		}
		if (m_iVerbose>=VERBOSE_DEBUG) {
			std::cout << "DEBUG: MassSpecConfigInfo(" << numQuads << ", " << lModelId << ")";
		}
		getModelById (lModelId, strModel);
		if (m_iVerbose>=VERBOSE_DEBUG) {
			std::cout << "=" << strModel.c_str() << std::endl;
		}
	} catch (_com_error &com_error) {
		std::cerr << "WARNING: Failed to retrieve Mass Spectrometry Method info. COM error " << std::hex << com_error.Error() << std::dec << std::endl;
	}
}

void AnalystQSInterface::getMSSerialNumber(std::string &strSerialNumber) {

	strSerialNumber.resize(0);

	getSampleLog();

	getSampleLogEntry (m_strSampleLog, "Serial Number: ", strSerialNumber);
	if (strSerialNumber.empty ()) {
		getSampleLogEntry (m_strSampleLog, "S/N: ", strSerialNumber);
	}
	std::string::size_type nStartPos = strSerialNumber.find_first_of(",\r\n");
	if (strSerialNumber.npos != nStartPos) {
		strSerialNumber.erase (nStartPos);
	}
}

void AnalystQSInterface::getMSHardwareVersion(std::string &strHardwareVersion) {

	strHardwareVersion.resize(0);

	getSampleLog();

	getSampleLogEntry (m_strSampleLog, "Firmware Version: ", strHardwareVersion);
	if (strHardwareVersion.empty ()) {
		getSampleLogEntry (m_strSampleLog, "Firmware Ver: ", strHardwareVersion);
	}
	std::string::size_type nStartPos = strHardwareVersion.find_first_of(",\r\n");
	if (strHardwareVersion.npos != nStartPos) {
		strHardwareVersion.erase (nStartPos);
	}
}

// Return the mass spectrometer ionisation (return 0 as success)
void AnalystQSInterface::getMSIonisation(std::string &strIonisation) {
	// TODO:
	// does not seem to find this recorded much
	strIonisation.resize(0);
}

// Return the mass spectrometer mass analyzer (return 0 as success)
void AnalystQSInterface::getMSMassAnalyzer(std::string &strMassAnalyzer) {
	// TODO:
	// does not seem to find this recorded much
	// maybe can guess from the model?
	strMassAnalyzer.resize(0);

	/*
	// TODO:
	// Seems like valid options for ABI instruments are:
	// "Q-TOF", "TOF", "TOF-TOF", "Q1", "Q3"
	// Question: Can we guess?
	getSampleLog();

	const std::string prefix("Component ID: ");
	getSampleLogEntry (m_strSampleLog, prefix, strMassAnalyzer);
	*/
}

// Return the mass spectrometer detector (return 0 as success)
void AnalystQSInterface::getMSDetector(std::string &strDetector) {
	// TODO:
	// does not seem to find this recorded much
	strDetector.resize(0);
}

Scan* AnalystQSInterface::getFirstScan(void)
{
	initScanIteratorState ();
	if (isValidScan()) {
		// return cache result!!!
		return m_pScan;
	} else {
		return getNextScan ();
	}
}

Scan* AnalystQSInterface::getNextScan(void)
{
	if (nextScanIteratorState()) {
		// create the scan instance
		while (!isValidScan()) {
			if (!nextScanIteratorState()) {
				return NULL;
			}
		}
		// return cache result!!!
		return m_pScan;
	}
	return NULL;
}

bool AnalystQSInterface::isValidScan(void) {

	m_pScan = m_iteratorState.popMRMScan ();
	if (NULL != m_pScan) {
		curScanNumber_++;
		return true;
	}

	m_pScan = m_iteratorState.popMergedScan ();
	if (NULL != m_pScan) {
		curScanNumber_++;
		return true;
	}

	if (m_ipFMANChromData->GetDataPointYValue(m_iteratorState.m_lCurrentDatapointIndex)>0) {
		// TODO: time reported should be from experiment#0
		float fxValue = (float) m_ipFMANChromData->GetDataPointXValue(m_iteratorState.m_lCurrentDatapointIndex) * 60;
		HRESULT hr = m_ipFMANSpecData->raw_SetSpectrum(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex, m_iteratorState.m_lCurrentExperimentIndex, fxValue, fxValue);
		if (SUCCEEDED(hr)) {
			m_pScan = new Scan();
			processSpectrum(m_pScan);

			curScanNumber_++;

			long lMergedScanNum = m_scanOriginToMergeTable.getMergedScanNum(curScanNumber_);
			if (lMergedScanNum != curScanNumber_) {
				m_pScan->isMerged_ = true;
				m_pScan->mergedScanNum_ = lMergedScanNum;
			}

			m_pScan->num_ = curScanNumber_;
			return true;

		} else {
			std::cerr << "FATAL: Could not retrieve Spectrum("
				<< m_iteratorState.m_lCurrentSampleIndex 
				<< ", " << m_iteratorState.m_lCurrentPeriodIndex
				<< ", " << m_iteratorState.m_lCurrentExperimentIndex
				<< ", " << fxValue
				<< ", " << fxValue << ") info." << std::endl;
			_com_issue_errorex(hr, m_ipFMANSpecData.GetInterfacePtr (), m_ipFMANSpecData.GetIID());
		}
	}
	return false;
}

inline void AnalystQSInterface::determineRetentionTimeType(long lSampleIndex, long lPeriodIndex) {
	m_fUseExperiment0RT = true;
	try {
		IUnknownPtr ipUnknown(m_ipFMANWiffFile->GetExperimentObject(lSampleIndex, lPeriodIndex, 0));
		IExperimentPtr ipExperiment;
		ipUnknown->QueryInterface(&ipExperiment);
		m_fUseExperiment0RT = (1==getMSLevel (ipExperiment->ScanType));
	} catch (... /*_com_error &com_error*/) {
	}
}

inline void AnalystQSInterface::processSpectrum(Scan *pScan) {

	IUnknownPtr ipUnknown(m_ipFMANWiffFile->GetExperimentObject(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex, m_iteratorState.m_lCurrentExperimentIndex));
	IExperimentPtr ipExperiment;
	ipUnknown->QueryInterface(&ipExperiment);
	pScan->polarity_ = (0==ipExperiment->Polarity?POSITIVE:NEGATIVE);
	long lScanType = ipExperiment->ScanType;
	pScan->scanType_ = getMSScanType(lScanType);
	pScan->msLevel_ = getMSLevel (lScanType);
	pScan->precursorList_.push_back( PrecursorScanInfo(0, 0, 0) );

	if (m_DPSettings.m_fCoordinate) {
		pScan->nativeScanRef_.setCoordinateType(instrumentInfo_.manufacturer_);
		pScan->nativeScanRef_.addCoordinate(ABI_COORDINATE_SAMPLE, toString(m_iteratorState.m_lCurrentSampleIndex));
		pScan->nativeScanRef_.addCoordinate(ABI_COORDINATE_PERIOD, toString(m_iteratorState.m_lCurrentPeriodIndex));
		pScan->nativeScanRef_.addCoordinate(ABI_COORDINATE_EXPERIMENT, toString(m_iteratorState.m_lCurrentExperimentIndex));
		pScan->nativeScanRef_.addCoordinate(ABI_COORDINATE_CYCLE, toString(m_iteratorState.m_lCurrentDatapointIndex));
	}

	if (isMRMScan(lScanType)) {
		//MRM experiment
		processMRMSpectrum (pScan);
	} else {
		processMSSpectrum (pScan);
	}
}

inline void AnalystQSInterface::processMSSpectrum(Scan *pScan) {

	double startMZ, endMZ;
	if (SUCCEEDED(m_ipFMANSpecData->raw_GetStartMass(&startMZ)) &&
		SUCCEEDED(m_ipFMANSpecData->raw_GetStopMass(&endMZ))) {
			pScan->startMZ_ = (Scan::MzValueType) startMZ;
			pScan->endMZ_ = (Scan::MzValueType) endMZ;
	} else {
			pScan->startMZ_ = pScan->endMZ_ = 0;
	}

	if (1 == pScan->msLevel_) {
		m_dChromotographRTInSec = pScan->retentionTimeInSec_ = m_ipFMANChromData->GetDataPointXValue(m_iteratorState.m_lCurrentDatapointIndex) * 60;
	} else {
		if (m_fUseExperiment0RT)
			pScan->retentionTimeInSec_ = m_dChromotographRTInSec;
		else
			pScan->retentionTimeInSec_ = m_ipFMANChromData->GetDataPointXValue(m_iteratorState.m_lCurrentDatapointIndex) * 60;

		m_sampleDDE.getPrecursorInfo(
			m_iteratorState.m_lCurrentExperimentIndex, m_iteratorState.m_lCurrentDatapointIndex, 
			pScan->precursorList_.back().MZ_, pScan->precursorList_.back().intensity_);
		m_sampleDDE.getPrecursorInfoEx(
			m_iteratorState.m_lCurrentExperimentIndex, m_iteratorState.m_lCurrentDatapointIndex, 
			pScan->precursorList_.back().charge_, pScan->precursorList_.back().collisionEnergy_);

		m_sampleScanMap.getPrecursorScanNumber( m_iteratorState.m_lCurrentExperimentIndex,
												m_iteratorState.m_lCurrentDatapointIndex,
												pScan->precursorList_.back().num_);

		if (0 == pScan->precursorList_.back().MZ_) {
			_bstr_t bstrParamID(m_ipFMANSpecData->DataTitle);
			const char *szPrecursorMZ = strstr((const char *)bstrParamID, "(");
			if (NULL != szPrecursorMZ) {
				pScan->precursorList_.back().MZ_ = atof(szPrecursorMZ+1);
			}
		}

		if (0 == pScan->precursorList_.back().intensity_) {
			pScan->precursorList_.back().intensity_ = m_ipFMANChromData->GetDataPointYValue(m_iteratorState.m_lCurrentDatapointIndex);
		}
	}

#ifndef NODEBUGDETAIL
	if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
		std::cout << "MS(time=" << pScan->retentionTimeInSec_
			<< ")" << std::endl;
	}
#endif

	processMSSpecData(pScan);
}

inline void AnalystQSInterface::processMSSpecData(Scan *pScan) {

	double maxIntensity = 0;
	bool fToReport = true;
	if (1 == pScan->msLevel_) {
		if (doCentroid_) {
			centroidSpecData(false, maxIntensity);
		} else {
			double minIntensity;
			HRESULT hr = m_ipFMANSpecData->GetYValueRange (&minIntensity, &maxIntensity);
		}
	} else {
		if (doTandemCentroid_) {
			centroidSpecData(doDeisotope_, maxIntensity);
		} else {
			double minIntensity;
			HRESULT hr = m_ipFMANSpecData->GetYValueRange (&minIntensity, &maxIntensity);
		}
	}

	pScan->setNumDataPoints(0);
	pScan->totalIonCurrent_ = 0;
	pScan->basePeakMZ_ = 0;
	pScan->basePeakIntensity_ = 0;

	if (fToReport) {
		// Data Processing: filtering on absolute intensity or % of max intensity
		//                  apply to MS2 or higher
		double thresholdIntensity;
		if (1 < pScan->msLevel_) {
			thresholdIntensity = m_DPSettings.getPeakCutOff(maxIntensity);
		} else {
			thresholdIntensity = 0;
		}
		long lSpecDataPoints = m_ipFMANSpecData->GetNumberOfDataPoints();

		pScan->setNumDataPoints (lSpecDataPoints);

		double dpiXVal, dpiYVal;
		long lPeaksCount=0;
		if (thresholdIntensity > 0.0) {
			for (long dpi=1; dpi<=lSpecDataPoints; dpi++) {
				m_ipFMANSpecData->GetDataPoint(dpi, &dpiXVal, &dpiYVal);
				if (dpiYVal<thresholdIntensity) {
					// noise or no peak
				} else {
					pScan->mzArray_[lPeaksCount] = dpiXVal;
					pScan->totalIonCurrent_ += (pScan->intensityArray_[lPeaksCount]=dpiYVal);
					lPeaksCount++;

					if (pScan->basePeakIntensity_<dpiYVal) {
						pScan->basePeakMZ_ = dpiXVal;
						pScan->basePeakIntensity_ = dpiYVal;
					}
				}
			}
		} else {
			for (long dpi=1; dpi<=lSpecDataPoints; dpi++) {
				m_ipFMANSpecData->GetDataPoint(dpi, &dpiXVal, &dpiYVal);
				if (dpiYVal>0.0) {
					pScan->mzArray_[lPeaksCount] = dpiXVal;
					pScan->totalIonCurrent_ += (pScan->intensityArray_[lPeaksCount]=dpiYVal);
					lPeaksCount++;

					if (pScan->basePeakIntensity_<dpiYVal) {
						pScan->basePeakMZ_ = dpiXVal;
						pScan->basePeakIntensity_ = dpiYVal;
					}
				}
			}
		}

		// Data Processing: filtering on min. peak count
		if (1 < pScan->msLevel_ && m_DPSettings.m_iFilterMinPeakCount > 0) {
			if (lPeaksCount<m_DPSettings.m_iFilterMinPeakCount) {
				//The number of peaks is not meeting the min. threshold
				//We ignore all peaks in this spectra then
				lPeaksCount = 0;
			}
		}
		pScan->setNumDataPoints (lPeaksCount);
		if (lPeaksCount > 0) {
			pScan->minObservedMZ_ = pScan->mzArray_[0];
			pScan->maxObservedMZ_ = pScan->mzArray_[lPeaksCount-1];
		} else {
			pScan->minObservedMZ_ = pScan->maxObservedMZ_ = 0;
		}
	} else {
		pScan->minObservedMZ_ = pScan->maxObservedMZ_ = 0;
	}
}

inline void AnalystQSInterface::centroidSpecData(bool doDeisotope, double &maxIntensity) {

	maxIntensity = 0;

	unsigned int control_word_fp;
	control_word_fp = _controlfp(_PC_64, _MCW_PC);

#ifndef NODEBUGDETAIL
	if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
		std::cout << "Centroid " << curScanNumber_ << " (" << m_pScan->msLevel_ << ") (" << m_ipFMANSpecData->GetNumberOfDataPoints();
	}
#endif
	IUnknownPtr ipUnknownPL(m_ipPeakFinder->FindPeaks(SPECTRUM, m_ipFMANSpecData, -1, 0, m_paramsString));
	IPeakList2Ptr ipPeakList;
	ipUnknownPL->QueryInterface(&ipPeakList);

	HRESULT hr;
	// go thru' the peaklist to construct the final FMANSpecData
	m_ipFMANSpecData->SetNumberOfDataPoints(0);
	m_ipFMANSpecData->XValuesAreSorted = 0;
	long lNumPeaks = ipPeakList->getNumPeaks();
#ifndef NODEBUGDETAIL
	if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
		std::cout << "," << lNumPeaks;
	}
#endif

	double precursorCentroidValue;
	double precursorCentroidIntensity;
	_variant_t isMonoIsotopic;
	_variant_t chargeState;
	long pointNumber=0;
	bool toAdd=false;
	bool fHeaderReported=false;
	for (long peakIndex=1; peakIndex<=lNumPeaks; peakIndex++) {

		ipPeakList->getPeakDataAsDouble(peakIndex, PEAK_DATA_CENTROID_VALUE, &precursorCentroidValue);
		ipPeakList->getPeakDataAsDouble(peakIndex, PEAK_DATA_AREA, &precursorCentroidIntensity);
		isMonoIsotopic = ipPeakList->getPeakData (peakIndex, PEAK_DATA_IS_MONOISOTOPIC);
		chargeState = ipPeakList->getPeakData (peakIndex, PEAK_DATA_CHARGE_STATE);

		if (doDeisotope) {
			toAdd = (0!=isMonoIsotopic.lVal);
		} else {
			toAdd = true;
		}

		if (toAdd) {
			m_ipFMANSpecData->AppendDataPoint(precursorCentroidValue, precursorCentroidIntensity);
			pointNumber++;
			//with some specific .wiff file, this return E_INVALIDARG
			//m_ipFMANSpecData->SetUserValue(pointNumber, isMonoIsotopic.lVal);
			if (FAILED(hr=m_ipFMANSpecData->raw_SetUserValue(pointNumber, isMonoIsotopic.lVal))) {
				if (!fHeaderReported) {
					std::cerr << "COM error " << std::hex << hr << std::dec 
						<< " spectrum("
						<< m_iteratorState.m_lCurrentSampleIndex 
						<< ", " << m_iteratorState.m_lCurrentPeriodIndex
						<< ", " << m_iteratorState.m_lCurrentExperimentIndex
						<< ", " << m_iteratorState.m_lCurrentDatapointIndex
						<< ") Scan#" << curScanNumber_;
						fHeaderReported = true;
				}
				std::cerr << " peak#" << peakIndex;
			}
		} else {
			long lpointNumber=0;
			ipPeakList->getPeakDataAsDouble(peakIndex, PEAK_DATA_MONOISOTOPIC_MASS, &precursorCentroidValue);
			m_ipFMANSpecData->GetClosestPointNumberForXValue(precursorCentroidValue, CLOSEST_POINT, &lpointNumber);
			double yValue = m_ipFMANSpecData->GetDataPointYValue(lpointNumber);
			precursorCentroidIntensity+=yValue;
			m_ipFMANSpecData->SetDataPoint(lpointNumber, precursorCentroidValue, precursorCentroidIntensity);
		}

		if (precursorCentroidIntensity>maxIntensity) {
			maxIntensity = precursorCentroidIntensity;
		}
	}
	m_ipFMANSpecData->XValuesAreSorted = -1;
	m_ipFMANSpecData->put_IsCentroided(-1);

#ifndef NODEBUGDETAIL
	if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
		std::cout << "," << m_ipFMANSpecData->GetNumberOfDataPoints() << ")";
	}
#endif

	if (fHeaderReported) {
		std::cerr << std::endl;
	}
}

inline void AnalystQSInterface::processMRMSpectrum(Scan *pScan) {

	IUnknownPtr ipUnknown(m_ipFMANWiffFile->GetExperimentObject(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex, m_iteratorState.m_lCurrentExperimentIndex));
	IExperimentPtr ipExperiment;
	ipUnknown->QueryInterface(&ipExperiment);
	double dPhonyRetention = m_ipFMANChromData->GetDataPointXValue(m_iteratorState.m_lCurrentDatapointIndex) * 60;
	double dPauseBetweenMassRanges = ipExperiment->PauseBetweenMassRanges;

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << "MRM(time=" << dPhonyRetention
					<< ")" << std::endl;
			}
#endif

	double thresholdIntensity = m_DPSettings.getPeakCutOff();
	long lSpecDataPoints = m_ipFMANSpecData->GetNumberOfDataPoints();

	double dpiXVal, dpiYVal;
	if (thresholdIntensity > 0.0) {
		for (long dpi=1; dpi<=lSpecDataPoints; dpi++) {
			m_ipFMANSpecData->GetDataPoint(dpi, &dpiXVal, &dpiYVal);

			IUnknownPtr ipUnknown(ipExperiment->GetMassRange((long)dpiXVal - 1));
			IMassRangePtr ipMassRange;
			ipUnknown->QueryInterface(&ipMassRange);

			if (dpiYVal>0)
				dPhonyRetention += ((ipMassRange->GetDwellTime()+dPauseBetweenMassRanges)/1000.0);

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << "MRM(" << dpi 
					<< "," << dpiXVal
					<< "," << dpiYVal
					<< "," << dPhonyRetention
					<< "," << ipMassRange->GetDwellTime()
					<< "," << dPauseBetweenMassRanges
					<< ")" << std::endl;
			}
#endif

			if (dpiYVal<thresholdIntensity) {
				// noise or no peak
			} else {
				// (x,y) = (ipMassRange->GetQstepMass(), dpiYVal)
				Scan *pMRMScan = new Scan ();

				//we use the cached value
				pMRMScan->polarity_ = pScan->polarity_;
				pMRMScan->scanType_ = pScan->scanType_;
				pMRMScan->msLevel_ = pScan->msLevel_;

				pMRMScan->setNumDataPoints(1);
				pMRMScan->mzArray_[0] 
					= pMRMScan->minObservedMZ_ 
					= pMRMScan->maxObservedMZ_ 
					= ipMassRange->GetQstepMass();
				pMRMScan->intensityArray_[0] = dpiYVal;
				pMRMScan->totalIonCurrent_ += dpiYVal;

				pMRMScan->basePeakIntensity_ = dpiYVal;
				pMRMScan->basePeakMZ_ 
					= pMRMScan->startMZ_ 
					= pMRMScan->endMZ_ 
					= ipMassRange->GetQstepMass();
				pMRMScan->precursorList_.back().intensity_ = 0;
				pMRMScan->precursorList_.back().MZ_ = ipMassRange->GetQstartMass();
				pMRMScan->retentionTimeInSec_ = dPhonyRetention;

				if (ipMassRange->GetMassDepParamCount()>0) {
					IUnknownPtr ipUnknown(ipMassRange->GetMassDepParamTbl());
					IParamDataCollPtr ipPDC;
					ipUnknown->QueryInterface(&ipPDC);
					_bstr_t bstrParamID("CE");
					short sIdx=0;
					IUnknownPtr ipUnknownParameter(ipPDC->FindParameter(bstrParamID, &sIdx));
					IParameterDataPtr ipPD;
					ipUnknownParameter->QueryInterface(&ipPD);
					pMRMScan->precursorList_.back().collisionEnergy_ = ipPD->GetstartVal();
				}

				pMRMScan->nativeScanRef_ = pScan->nativeScanRef_;

				m_iteratorState.pushMRMScan(pMRMScan);
			}
		}
	} else {
		for (long dpi=1; dpi<=lSpecDataPoints; dpi++) {
			m_ipFMANSpecData->GetDataPoint(dpi, &dpiXVal, &dpiYVal);

			IUnknownPtr ipUnknown(ipExperiment->GetMassRange((long)dpiXVal - 1));
			IMassRangePtr ipMassRange;
			ipUnknown->QueryInterface(&ipMassRange);

			if (dpiYVal>0)
				dPhonyRetention += ((ipMassRange->GetDwellTime()+dPauseBetweenMassRanges)/1000.0);

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << "MRM(" << dpi 
					<< "," << dpiXVal
					<< "," << dpiYVal
					<< "," << dPhonyRetention
					<< "," << ipMassRange->GetDwellTime()
					<< "," << dPauseBetweenMassRanges
					<< ")" << std::endl;
			}
#endif

			if (dpiYVal>0.0) {
				// (x,y) = (ipMassRange->GetQstepMass(), dpiYVal)
				Scan *pMRMScan = new Scan ();

				//we use the cached value
				pMRMScan->polarity_ = pScan->polarity_;
				pMRMScan->scanType_ = pScan->scanType_;
				pMRMScan->msLevel_ = pScan->msLevel_;

				pMRMScan->setNumDataPoints(1);
				pMRMScan->mzArray_[0] 
					= pMRMScan->minObservedMZ_ 
					= pMRMScan->maxObservedMZ_ 
					= ipMassRange->GetQstepMass();
				pMRMScan->intensityArray_[0] = dpiYVal;
				pMRMScan->totalIonCurrent_ += dpiYVal;

				pMRMScan->basePeakIntensity_ = dpiYVal;
				pMRMScan->basePeakMZ_ 
					= pMRMScan->startMZ_ 
					= pMRMScan->endMZ_ 
					= ipMassRange->GetQstepMass();
				pMRMScan->precursorList_.back().intensity_ = 0;
				pMRMScan->precursorList_.back().MZ_ = ipMassRange->GetQstartMass();
				pMRMScan->retentionTimeInSec_ = dPhonyRetention;

				if (ipMassRange->GetMassDepParamCount()>0) {
					IUnknownPtr ipUnknown(ipMassRange->GetMassDepParamTbl());
					IParamDataCollPtr ipPDC;
					ipUnknown->QueryInterface(&ipPDC);
					_bstr_t bstrParamID("CE");
					short sIdx=0;
					IUnknownPtr ipUnknownParameter(ipPDC->FindParameter(bstrParamID, &sIdx));
					IParameterDataPtr ipPD;
					ipUnknownParameter->QueryInterface(&ipPD);
					pMRMScan->precursorList_.back().collisionEnergy_ = ipPD->GetstartVal();
				}

				pMRMScan->nativeScanRef_ = pScan->nativeScanRef_;

				m_iteratorState.pushMRMScan(pMRMScan);
			}
		}
	}

	delete m_pScan;
	m_pScan = NULL;
	if (m_iteratorState.isMRMScanAvailable()) {
		m_pScan = m_iteratorState.popMRMScan ();
	}
}

inline void AnalystQSInterface::getMSRunTime () {
	if (-1==startTimeInSec_ || -1==endTimeInSec_) {
		HRESULT hr;
		if (SUCCEEDED(hr=m_ipFMANChromData->raw_SetToTIC(1, 0, 0))) {
			startTimeInSec_ = m_ipFMANChromData->GetDataPointXValue(1) * 60; // min-to-sec
			endTimeInSec_ = m_ipFMANChromData->GetDataPointXValue(m_ipFMANChromData->GetNumberOfDataPoints ()) * 60; // min-to-sec
		} else {
			std::cerr << "FATAL: Chromatogram.TIC(1, 0, 0), COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}
	}
}

void AnalystQSInterface::computeScansCount () {
	if (0>=totalNumScans_) {

		totalNumScans_ = 0;

		long lActualNumberOfSamples = m_ipFMANWiffFile->GetActualNumberOfSamples();

		if (m_iVerbose>=VERBOSE_INFO) {
			std::cout << "INFO: " << lActualNumberOfSamples << " sample(s) recorded." << std::endl;
		}

		bool fMCAData = false;
		bool fToMergeScan = true;
		bool fToWarn = false;
		double dMRMThreshold = m_DPSettings.getPeakCutOff();

		for (long lSampleIndex=1; lSampleIndex<=lActualNumberOfSamples; lSampleIndex++) {

			long lActualNumberOfPeriods = m_ipFMANWiffFile->GetActualNumberOfPeriods(lSampleIndex);
			if (m_iVerbose>=VERBOSE_INFO) {
				_bstr_t bstrSampleName(m_ipFMANWiffFile->GetSampleName(lSampleIndex));
				std::cout << "INFO: Sample#" << lSampleIndex << "(";
				std::cout << ((const char *) bstrSampleName);
				std::cout << "), has " << lActualNumberOfPeriods << " period(s) recorded." << std::endl;
			}

			for (long lPeriodIndex=0; lPeriodIndex<lActualNumberOfPeriods; lPeriodIndex++) {
				long lActualNumberOfExperiments = m_ipFMANWiffFile->GetNumberOfExperiments(lSampleIndex,lPeriodIndex);

				if (m_iVerbose>=VERBOSE_INFO) {
					std::cout << "INFO:    Period#" << lPeriodIndex << ", has " << lActualNumberOfExperiments << " experiment(s) recorded." << std::endl;
				}

				// keep the scan type for each experiment
				std::vector<MSScanType> experimentsScanType;
				std::vector<long> experimentsPolarity;
				std::vector<long> experimentsUseMCAScans;
				if (getExperimentsInfo(lSampleIndex, lPeriodIndex, experimentsScanType, experimentsPolarity, experimentsUseMCAScans)) {
					fToWarn = true;
				}

				std::vector<long> experimentsTotalDPs(lActualNumberOfExperiments, 0);
				for (long lExperimentIndex=0; lExperimentIndex<lActualNumberOfExperiments; lExperimentIndex++) {
					HRESULT hr = m_ipFMANChromData->raw_SetToTIC(lSampleIndex, lPeriodIndex, lExperimentIndex);
					if (FAILED(hr)) {
						std::cerr << "Error: Chromatogram.TIC("<<lSampleIndex<<", "<<lPeriodIndex<<", "<<lExperimentIndex<<"), COM error " 
							<< std::hex << hr << std::dec << std::endl;
					}
					experimentsTotalDPs[lExperimentIndex] = m_ipFMANChromData->GetNumberOfDataPoints ();
				}
				// END - keep the scan type for each experiment

				long lNumberOfDataPoints = experimentsTotalDPs[0];
				m_sampleScanMap.initNewSample (lSampleIndex, lPeriodIndex, lActualNumberOfExperiments, lNumberOfDataPoints, totalNumScans_);
				for (long lExperimentIndex=0; lExperimentIndex<lActualNumberOfExperiments; lExperimentIndex++) {
					HRESULT hr = m_ipFMANChromData->raw_SetToTIC(lSampleIndex, lPeriodIndex, lExperimentIndex);
					if (FAILED(hr)) {
						std::cerr << "Error: Chromatogram.TIC("<<lSampleIndex<<", "<<lPeriodIndex<<", "<<lExperimentIndex<<"), COM error " 
							<< std::hex << hr << std::dec << std::endl;
					}

					if (m_iVerbose>=VERBOSE_INFO) {
						std::cout << "INFO:       Experiment#" << lExperimentIndex << ":"
							<< " Scan(" 
							<< toString(experimentsScanType[lExperimentIndex]).c_str()
							<< "/" 
							<< getPolarityString(experimentsPolarity[lExperimentIndex])
							<< ")";
					}

					if (0!=experimentsUseMCAScans[lExperimentIndex]) {
						fMCAData = true;
					}

					long lScansCount = 0;
					for (long lDataPointsIndex=1; lDataPointsIndex<=lNumberOfDataPoints; lDataPointsIndex++) {
						if (m_ipFMANChromData->GetDataPointYValue(lDataPointsIndex)>0) {
							if (MRM!=experimentsScanType[lExperimentIndex]) {
								// non-MRM scan count determination
								m_sampleScanMap.cacheSampleScanNumber (lExperimentIndex, lDataPointsIndex);
								lScansCount++;
								continue;
							}

							fToMergeScan = false;

							// MRM scan count determination
							float fxValue = (float) m_ipFMANChromData->GetDataPointXValue(lDataPointsIndex) * 60;
							HRESULT hr = m_ipFMANSpecData->raw_SetSpectrum(lSampleIndex, lPeriodIndex, lExperimentIndex, fxValue, fxValue);
							if (FAILED(hr)) {
								// best estimate possible
								lScansCount++;
								continue;
							}

							long lSpecDataPoints = m_ipFMANSpecData->GetNumberOfDataPoints();
							if (dMRMThreshold>0) {
								// thresholding is active, check how many clear the threshold
								double dpiXVal, dpiYVal;
								for (long dpi=1; dpi<=lSpecDataPoints; dpi++) {
									m_ipFMANSpecData->GetDataPoint(dpi, &dpiXVal, &dpiYVal);
									if (dpiYVal<dMRMThreshold) {
									} else {
										lScansCount++;
									}
								}
							} else {
								double dpiXVal, dpiYVal;
								for (long dpi=1; dpi<=lSpecDataPoints; dpi++) {
									m_ipFMANSpecData->GetDataPoint(dpi, &dpiXVal, &dpiYVal);
									if (dpiYVal>0) {
										lScansCount++;
									}
								}
							}
						}
					} // lDataPointsIndex
					if (m_iVerbose>=VERBOSE_INFO) {
						std::cout << ", " << lScansCount << "/" << lNumberOfDataPoints;
						if (0!=experimentsUseMCAScans[lExperimentIndex]) {
							std::cout << "-(MCA)->1/1";
						}
						std::cout << std::endl;
					}

					totalNumScans_ += lScansCount;
				} // ExperimentIndex

				m_sampleScanMap.sampleCached(lSampleIndex, lPeriodIndex);
				m_sampleScanMap.reorderScanNumbers();

				// compute the merged scans for all experiments
				// in the same (SampleIndex, PeriodIndex)
				if (fToMergeScan && m_DPSettings.m_dGroupPrecursorMassTolerance>0) {
					if (m_iVerbose>=VERBOSE_INFO) {
						std::cout << "INFO:       Merged scan: searching for candidates...";
					}
					long lMergedScanCount=0, lTotalScansConsidered=0;
					MergedScanTable mergedScans;
					mergedScans.initNewSample(lSampleIndex, lPeriodIndex);
					mergeScans(lSampleIndex, lPeriodIndex, experimentsScanType, m_sampleScanMap, 
						lMergedScanCount, lTotalScansConsidered, mergedScans);
					if (lMergedScanCount>0) {
						m_sampleMergedScansCache[mergedScans.getKey()] = mergedScans;
					}
					if (m_iVerbose>=VERBOSE_INFO) {
						std::cout << " " << lMergedScanCount << "/" << lTotalScansConsidered << std::endl;
					}
					totalNumScans_ += lMergedScanCount;
				}

			} // PeriodIndex
		} // SampleIndex
		firstScanNumber_ = 1;
		lastScanNumber_ = totalNumScans_;

		if (m_iVerbose>=VERBOSE_INFO) {
			std::cout << "INFO: Allocated " << totalNumScans_ << " scan slot(s) via full sweep." << std::endl;
		}

		// perform a prefetch so that we can tell where we will get Precursor information
		getSamplePrecursorInfo (1, 0);
		if (m_iVerbose>=VERBOSE_INFO) {
			switch (m_sampleDDE.getType()) {
				case DDEData::TYPE_DDEEX:
					std::cout << "INFO: Using Precursor info from DDEEX data stream." << std::endl;
					break;
				case DDEData::TYPE_DDE:
					std::cout << "INFO: Using Precursor info from DDE data stream." << std::endl;
					break;
				case DDEData::TYPE_NONE:
				default:
					break;
			}
		}

		if (fToWarn) {
			std::cerr << "WARNING: Your installed library has issue accessing experiment information." << std::endl;
			std::cerr << "WARNING: Translation is unlikely to complete successfully." << std::endl;
			std::cerr << "WARNING: Please check that you have the right version of the Analyst software installed." << std::endl;
		}

		if (fMCAData) {
			std::cerr << "WARNING: Each spectra of MACs is reported separately." << std::endl;
			std::cerr << "WARNING: The summing of MACs is not supported currently." << std::endl;
		}
	}
}

bool AnalystQSInterface::getExperimentsInfo(long lSampleIndex, long lPeriodIndex, std::vector<MSScanType> &experimentsScanType, std::vector<long> &experimentsPolarity, std::vector<long> &experimentsUseMCAScans) {
	bool fToWarn = false;
	long lActualNumberOfExperiments = m_ipFMANWiffFile->GetNumberOfExperiments(lSampleIndex,lPeriodIndex);

	// keep the scan type for each experiment
	experimentsScanType.resize(lActualNumberOfExperiments, SCAN_UNDEF);
	experimentsPolarity.resize(lActualNumberOfExperiments, 0);
	experimentsUseMCAScans.resize(lActualNumberOfExperiments, 0);
	for (long lExperimentIndex=0; lExperimentIndex<lActualNumberOfExperiments; lExperimentIndex++) {
		try {
			IUnknownPtr ipUnknown(m_ipFMANWiffFile->GetExperimentObject(lSampleIndex, lPeriodIndex, lExperimentIndex));
			IExperimentPtr ipExperiment;
			ipUnknown->QueryInterface(&ipExperiment);
			experimentsScanType[lExperimentIndex] = getMSScanType(ipExperiment->ScanType);
			experimentsPolarity[lExperimentIndex] = ipExperiment->Polarity;
			experimentsUseMCAScans[lExperimentIndex] = ipExperiment->UseMCAScans;
		} catch (... /*_com_error &com_error*/) {
			fToWarn = true;
		}
	}

	return fToWarn;
}

void AnalystQSInterface::mergeScans (
	long lSampleIndex, long lPeriodIndex, std::vector<MSScanType> experimentsScanType, const ScanTable &scanTable, 
	long &lMergedScanCount, long &lTotalScansConsidered, MergedScanTable &mergedScans) {

	lMergedScanCount = lTotalScansConsidered = 0;

	std::vector<bool> experimentsMergable(experimentsScanType.size(), false);
	if (0==experimentsScanType.size()) return;
	{
		bool fContinue=false;
		for(std::vector<MSScanType>::size_type i=0; i<experimentsScanType.size(); i++) {
			if (isMSScanTypeMergable(experimentsScanType[i])) {
				experimentsMergable[i]=fContinue=true;
			}
		}
		if (!fContinue) return;
	}

	// we need precursor mz, and charge
	getSamplePrecursorInfo (lSampleIndex, lPeriodIndex);
	if (!m_sampleDDE.isAvailable(lSampleIndex, lPeriodIndex)) {
		// we cannot continue if we do not have at least the precursor mz
		return;
	}

	MergeCandidates sampleMergeCandidates;
	sampleMergeCandidates.resize(0);

	if (0==m_DPSettings.m_dGroupPrecursorMassTolerance) return;

	/*
	when m_DPSettings.m_iGroupMinCycles = 1, 
	we are not interested  to report spectra with a single scan [considered not merged]
	*/
	size_t effectiveMinTotalSpectras = (m_DPSettings.m_iGroupMinCycles>1) ? m_DPSettings.m_iGroupMinCycles : 2;

	double dPrecursorMZ;
	double dPrecursorIntensity;
	int nPrecursorCharge;
	long lScanNumber;
	long lNumberOfDataPoints=m_sampleDDE.getNumberOfDataPoints();
	long lNumberOfExperiments=m_sampleDDE.getNumberOfExperiments();
#if 0
	double dMascotPrecision=0.1;
#endif
	for (long lDataPointsIndex=1; lDataPointsIndex<=lNumberOfDataPoints; lDataPointsIndex++) {
		for (long lExperimentIndex=1; lExperimentIndex<lNumberOfExperiments; lExperimentIndex++) {
			// scan type is not mergable ?
			if (!experimentsMergable[lExperimentIndex]) continue;

			// precursor MZ is not available?
			m_sampleDDE.getPrecursorMZCharge(lExperimentIndex, lDataPointsIndex, dPrecursorMZ, dPrecursorIntensity, nPrecursorCharge);
			if (0==dPrecursorMZ) continue;
			if (0==dPrecursorIntensity) continue;

			// scan number not allocated?
			scanTable.getPrecursorScanNumber(lExperimentIndex, lDataPointsIndex, lScanNumber);
			if (0==lScanNumber) continue;

#if 0
			//TODO: on AnalystQS Mascot.dll uses 1 decimal place for precursor MZ
			//      if we do not follow, we will have different merged scans selection
			dPrecursorMZ = (int((dPrecursorMZ/dMascotPrecision)+0.5))*dMascotPrecision;
#endif

			// this scan has cleared the threshold, candidate for merge checking
			sampleMergeCandidates.push_back(MergeCandidate(lExperimentIndex, lDataPointsIndex, dPrecursorMZ, nPrecursorCharge, lScanNumber));
		}
	}

	// go thru' the candidate and merge any possible
	long lMergedScanNumber = scanTable.getLastScanNumberAssigned();
	MergeCandidates::size_type numMergeCandidates=sampleMergeCandidates.size();
	for(MergeCandidates::size_type i=0; i<numMergeCandidates; i++) {
		MergeCandidate &mergeCandidate = sampleMergeCandidates[i];

		if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
			std::cout << "\nProcessing seed ";
			mergeCandidate.dump();
		}

		// if a scan is already merged, it is excluded from future merging
		if (mergeCandidate.m_fMerged) continue;

		MergedScan mergedScan(mergeCandidate, i);
		long lLastDataPointIndex = mergeCandidate.m_lDataPointIndex;
		int nPrecursorCharge = mergeCandidate.m_nPrecursorCharge;
		for(MergeCandidates::size_type j=i+1; j<numMergeCandidates; j++) {
			MergeCandidate &candidateToMerge = sampleMergeCandidates[j];

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << "\n\tChecking ";
				candidateToMerge.dump();
			}

			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << "\t\t" << (candidateToMerge.m_fMerged ? "merged" : "avail");
			}
#endif

			// if a scan is already merged, it is excluded from future merging
			if (candidateToMerge.m_fMerged) continue;

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << " Cycle Span " << (candidateToMerge.m_lDataPointIndex - lLastDataPointIndex);
			}
#endif

			//if ((candidateToMerge.m_lDataPointIndex - mergeCandidate.m_lDataPointIndex) > m_DPSettings.m_iGroupMaxCyclesBetween) {
			if ((candidateToMerge.m_lDataPointIndex - lLastDataPointIndex) > m_DPSettings.m_iGroupMaxCyclesBetween) {
				// exceed the cycle span in group, no further potential candidate
				break;
			}

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << " Charge " << nPrecursorCharge << " " << candidateToMerge.m_nPrecursorCharge;
			}
#endif

			// TODO: merging should consider same charge, but not unknown charge?
			if (0 != nPrecursorCharge) {
				if (0!=candidateToMerge.m_nPrecursorCharge && candidateToMerge.m_nPrecursorCharge != nPrecursorCharge) {
					// the precursor at different charge are not combined, not a candidate
					continue;
				}
			}

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << " PrecursorMZ " << candidateToMerge.m_dPrecursorMZ << " " << (fabs(candidateToMerge.m_dPrecursorMZ - mergeCandidate.m_dPrecursorMZ));
			}
#endif

			if (fabs(candidateToMerge.m_dPrecursorMZ - mergeCandidate.m_dPrecursorMZ) > m_DPSettings.m_dGroupPrecursorMassTolerance) {
				// the precursor MZ is not within the tolerance, not a candidate
				continue;
			}

#ifndef NODEBUGDETAIL
			if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
				std::cout << " ADDED" << std::endl;
			}
#endif

			// pass the above criteria, so the scan can be merged
			mergedScan.mergeScan (candidateToMerge, j);
			lLastDataPointIndex = candidateToMerge.m_lDataPointIndex;
			if (0 == nPrecursorCharge) {
				nPrecursorCharge = candidateToMerge.m_nPrecursorCharge;
			}

		}

		if (mergedScan.getNumberOfMergedScan()>1) {
			lTotalScansConsidered++;
		}

		if (mergedScan.getNumberOfMergedScan()>=effectiveMinTotalSpectras) {
			// scans merged, considered them as "used for merge"
			// link these scan origins with the final merged scan number
			lMergedScanNumber++;
			for (int k=0; k < (int) mergedScan.getNumberOfMergedScan(); k++) {
				sampleMergeCandidates[mergedScan.getCandidateId(k)].m_fMerged = true;
				m_scanOriginToMergeTable.add (mergedScan.getMergedScanNumber(k), lMergedScanNumber);
			}

			mergedScans.cacheSampleMergedScan(mergedScan);
			lMergedScanCount++;
		}
	}

	//Sanity check
	if (m_iVerbose>=VERBOSE_DEBUG) {
		std::cout << "DEBUG: merged scans details" << std::endl;
		mergedScans.dump();
	}
}

ScanPtr AnalystQSInterface::instantiateMergedScans (long lSampleIndex, long lPeriodIndex, const MergedScan &mergedScan) {

	std::auto_ptr<Scan> apScan(new Scan());

	long lExperimentIndex=mergedScan.m_mergedExperimentIndex[0];
	long lDataPointIndex=mergedScan.m_mergedDataPointIndex[0];

	apScan->setNumScanOrigins((int)mergedScan.m_mergedScanNumber.size());
	apScan->scanOriginNums[0] = mergedScan.m_mergedScanNumber[0];
	apScan->scanOriginParentFileIDs[0] = sha1Report_;
	apScan->isMerged_ = true;
	apScan->precursorList_.push_back( PrecursorScanInfo(0, 0, 0) );

	HRESULT hr = m_ipFMANChromData->SetToTIC(lSampleIndex, lPeriodIndex, 0);
	if (FAILED(hr)) {
		std::cerr << "FATAL: Chromatogram.TIC("
			<<lSampleIndex<<", "
			<<lPeriodIndex<<", 0), retention time for merged scan.  COM error " 
			<< std::hex << hr << std::dec << std::endl;
		_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
	}
	m_dChromotographRTInSec = apScan->retentionTimeInSec_ = m_ipFMANChromData->GetDataPointXValue(lDataPointIndex) * 60;

	hr = m_ipFMANChromData->SetToTIC(lSampleIndex, lPeriodIndex, lExperimentIndex);
	if (FAILED(hr)) {
		std::cerr << "FATAL: Chromatogram.TIC("
			<<lSampleIndex<<", "
			<<lPeriodIndex<<", "
			<<lExperimentIndex<<") for merged scan, COM error " 
			<< std::hex << hr << std::dec << std::endl;
		_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
	}
	float fxValue = (float) m_ipFMANChromData->GetDataPointXValue(lDataPointIndex) * 60;
	hr = m_ipFMANSpecData->raw_SetSpectrum(lSampleIndex, lPeriodIndex, lExperimentIndex, fxValue, fxValue);
	if (FAILED(hr)) {
		std::cerr << "FATAL: Could not retrieve Spectrum("
			<< lSampleIndex 
			<< ", " << lPeriodIndex
			<< ", " << lExperimentIndex
			<< ", " << fxValue
			<< ", " << fxValue << ") info." << std::endl;
		_com_issue_errorex(hr, m_ipFMANSpecData.GetInterfacePtr (), m_ipFMANSpecData.GetIID());
	}

	//-----

	{
		IUnknownPtr ipUnknown(m_ipFMANWiffFile->GetExperimentObject(lSampleIndex, lPeriodIndex, lExperimentIndex));
		IExperimentPtr ipExperiment;
		ipUnknown->QueryInterface(&ipExperiment);
		apScan->polarity_ = (0==ipExperiment->Polarity?POSITIVE:NEGATIVE);
		long lScanType = ipExperiment->ScanType;
		apScan->scanType_ = getMSScanType(lScanType);
		apScan->msLevel_ = getMSLevel (lScanType);
	}

	m_sampleDDE.getPrecursorInfo(lExperimentIndex, lDataPointIndex, apScan->precursorList_.back().MZ_, apScan->precursorList_.back().intensity_);
	m_sampleDDE.getPrecursorInfoEx(lExperimentIndex, lDataPointIndex, apScan->precursorList_.back().charge_, apScan->precursorList_.back().collisionEnergy_);

	double startMZ, endMZ;
	if (SUCCEEDED(m_ipFMANSpecData->raw_GetStartMass(&startMZ)) &&
		SUCCEEDED(m_ipFMANSpecData->raw_GetStopMass(&endMZ))) {
			apScan->startMZ_ = (Scan::MzValueType) startMZ;
			apScan->endMZ_ = (Scan::MzValueType) endMZ;
	} else {
			apScan->startMZ_ = apScan->endMZ_ = 0;
	}

	//-----

  double dAddTolerance = 0.001; // TODO: should be a user-defined settings

	for (size_t i=1; i<mergedScan.getNumberOfMergedScan (); i++) {
		lExperimentIndex=mergedScan.m_mergedExperimentIndex[i];
		lDataPointIndex=mergedScan.m_mergedDataPointIndex[i];

		apScan->scanOriginNums[i] = mergedScan.m_mergedScanNumber[i];
		apScan->scanOriginParentFileIDs[i] = sha1Report_;

		HRESULT hr = m_ipFMANChromData->SetToTIC(lSampleIndex, lPeriodIndex, lExperimentIndex);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Chromatogram.TIC("
				<<lSampleIndex<<", "
				<<lPeriodIndex<<", "
				<<lExperimentIndex<<") for merging, COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}
		fxValue = (float) m_ipFMANChromData->GetDataPointXValue(lDataPointIndex) * 60;
		hr = m_ipFMANSpecDataToMerge->raw_SetSpectrum(lSampleIndex, lPeriodIndex, lExperimentIndex, fxValue, fxValue);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Could not retrieve Spectrum("
				<< lSampleIndex 
				<< ", " << lPeriodIndex
				<< ", " << lExperimentIndex
				<< ", " << fxValue
				<< ", " << fxValue << ") for merging." << std::endl;
			_com_issue_errorex(hr, m_ipFMANSpecDataToMerge.GetInterfacePtr (), m_ipFMANSpecDataToMerge.GetIID());
		}

		hr = m_ipFMANSpecData->Add(m_ipFMANSpecDataToMerge, dAddTolerance);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Could not add Spectrum("
				<< lSampleIndex 
				<< ", " << lPeriodIndex
				<< ", " << lExperimentIndex
				<< ", " << fxValue
				<< ", " << fxValue << ") for merging." << std::endl;
			_com_issue_errorex(hr, m_ipFMANSpecDataToMerge.GetInterfacePtr (), m_ipFMANSpecDataToMerge.GetIID());
		}
	}

	processMSSpecData(apScan.get());

	return apScan.release();
}

void AnalystQSInterface::getSamplePrecursorInfo (long lSampleIndex, long lPeriodIndex) {

	if (m_sampleDDE.isAvailable(lSampleIndex, lPeriodIndex)) {
		return;
	}

	getSamplePrecursorInfo (m_sampleDDE, lSampleIndex, lPeriodIndex);
#ifndef NODEBUGDETAIL
	// Sanity check!
	if (m_iVerbose>=VERBOSE_DEBUGDETAIL) {
		std::cout << "DEBUG: sample DDE details" << std::endl;
		m_sampleDDE.dump();
	}
#endif
}

void AnalystQSInterface::getSamplePrecursorInfo (DDEData &ddeData, long lSampleIndex, long lPeriodIndex) {

	if (ddeData.isAvailable(lSampleIndex, lPeriodIndex)) {
		return;
	}

	long lActualNumberOfExperiments = m_ipFMANWiffFile->GetNumberOfExperiments(lSampleIndex,lPeriodIndex);
	HRESULT hr = m_ipFMANChromData->raw_SetToTIC(lSampleIndex, lPeriodIndex, 0);
	if (FAILED(hr)) {
		std::cerr << "Error: Chromatogram.TIC("<<lSampleIndex<<", "<<lPeriodIndex<<", 0), COM error " 
			<< std::hex << hr << std::dec << std::endl;
		return;
	}
	long lNumberOfDataPoints = m_ipFMANChromData->GetNumberOfDataPoints ();
	ddeData.initNewSample (lSampleIndex, lPeriodIndex, lActualNumberOfExperiments, lNumberOfDataPoints);

	IUnknownPtr ipUnknownFileManagerObject(m_ipFMANWiffFile->GetFileManagerObject());
	IFMWIFFPtr ipIFMWIFF;
	hr = ipUnknownFileManagerObject->QueryInterface(&ipIFMWIFF);
	unsigned long dwFileHandle=0;
	hr = ipIFMWIFF->FMANGetWIFFDriverFileHandle(&dwFileHandle);

	IUnknownPtr ipUnknownWTS;
	hr = ipIFMWIFF->FMANWIFFGetWTS (&ipUnknownWTS);

	IWIFFTSvrDDERealTimeDataExPtr ipWIFFTSvrDDERealTimeDataEx(ipUnknownWTS);

	try {
		// TODO: WCH
		// DDERealTimeData has no associated period, but
		// we do not have sample data to test this behaviour
		long lStreamDataObject=0;
		hr = ipWIFFTSvrDDERealTimeDataEx->OpenArrStrCLS_DDERealTimeDataEx(dwFileHandle, &lStreamDataObject, lSampleIndex);

		if (lNumberOfDataPoints>0 && lActualNumberOfExperiments>1) {
			long lDDEExIndex=0;
			double dDDEMass=0; 
			long lDDETime=0; 
			short sDDECharge=0; 
			float fDDECollisionEnergy=0, fCollisionEnergyForMS3=0; 
			double dDDEIntensity=0; 
			short sDDEScanType=0; 
			long lDDEPeriodIndex=0, lDDEExperimIndex=0, lDDECycleIndex=0, lDDEParentMassIndex=0; 
			double dDDEDynamicFillTime=0, dDDEExReserved2=0; 
			long lDDEExReserved3=0, lDDEExReserved4=0;
			try {
				do {
					hr = ipWIFFTSvrDDERealTimeDataEx->ReadArrStrCLS_DDERealTimeDataEx(lStreamDataObject, 
						&lDDEExIndex, &dDDEMass, &lDDETime, &sDDECharge, 
						&fDDECollisionEnergy, &fCollisionEnergyForMS3, &dDDEIntensity,
						&sDDEScanType, &lDDEPeriodIndex, &lDDEExperimIndex, &lDDECycleIndex,
						&lDDEParentMassIndex, &dDDEDynamicFillTime, 
						&dDDEExReserved2, &lDDEExReserved3, &lDDEExReserved4);
					if (SUCCEEDED(hr)) {
						if (dDDEMass>0) {
							ddeData.cacheSampleInfo (lDDEExperimIndex, lDDECycleIndex+1, dDDEMass, dDDEIntensity);
							ddeData.cacheSampleInfoEx(lDDEExperimIndex, lDDECycleIndex+1, sDDECharge, fDDECollisionEnergy);
						}
						/*std::cout << "DDERealTimeDataEx("<<lDDEPeriodIndex<<","<<lDDEExperimIndex<<","<<lDDECycleIndex<<"):" << std::endl;
						std::cout << "  Index=" << lDDEExIndex << ",Mass=" << dDDEMass << ",Time=" << lDDETime << ",Charge=" << sDDECharge << std::endl;
						std::cout << "  CollisionEnergy=" << fDDECollisionEnergy << ",CollisionEnergyForMS3=" << fCollisionEnergyForMS3 << ",Intensity=" << dDDEIntensity << std::endl;
						std::cout << "  ScanType=" << sDDEScanType << ",ParentMassIndex=" << lDDEParentMassIndex << "DynamicFillTime=" << dDDEDynamicFillTime << std::endl;
						std::cout << "  ExReserved2=" << dDDEExReserved2 << ",ExReserved3=" << lDDEExReserved3 << ",ExReserved4=" << lDDEExReserved4 << std::endl;*/
					} else {
						std::cerr << "DDERealTimeDataEx(): hr="  << std::hex << hr << std::dec << std::endl;
						break;
					}
				} while (true);
			} catch (... /*_com_error &com_error*/) {
				// do nothing
				//std::cerr << "Error: com_error in DDEEx processing, hr=" << std::hex << hr << std::dec << std::endl;
			}
			if (lDDEExIndex>0) {
				ddeData.sampleCached(lSampleIndex, lPeriodIndex);
				ddeData.sampleExCached(lSampleIndex, lPeriodIndex);
			}
		}

		hr = ipWIFFTSvrDDERealTimeDataEx->CloseArrStrCLS_DDERealTimeDataEx(lStreamDataObject);
	} catch (_com_error &com_error) {
		// do nothing
		std::cerr << "Error: com_error in DDEEx processing, hr=" << std::hex << hr << std::dec << std::endl;
	}

	if (ddeData.isAvailable(lSampleIndex, lPeriodIndex)) {
		return;
	}

	IWIFFTSvrDDERealTimeDataPtr ipWIFFTSvrDDERealTimeData(ipUnknownWTS);

	try {
		// TODO: WCH
		// DDERealTimeData has no associated period, but
		// we do not have sample data to test this behaviour
		long lStreamDataObject=0;
		hr = ipWIFFTSvrDDERealTimeData->OpenArrStrCLS_DDERealTimeData(dwFileHandle, &lStreamDataObject, lSampleIndex);

		if (lNumberOfDataPoints>0 && lActualNumberOfExperiments>1) {
			double dDDERTPrecursorMass=0;
			long lDDERTIndex=0;
			long lDDERTCount=0;
			double dDDERTReserved1=0;
			double dDDERTReserved2=0;

			try {
				for (long lDataPointIndex=1; lDataPointIndex<=lNumberOfDataPoints; lDataPointIndex++) {
					for (long lExperimentIndex=1; lExperimentIndex<lActualNumberOfExperiments; lExperimentIndex++) {
						hr = ipWIFFTSvrDDERealTimeData->ReadArrStrCLS_DDERealTimeData(lStreamDataObject, 
							&dDDERTPrecursorMass, &lDDERTIndex, &lDDERTCount, &dDDERTReserved1, &dDDERTReserved2);
						if (dDDERTPrecursorMass>0) {
							ddeData.cacheSampleInfo (lExperimentIndex, lDataPointIndex, dDDERTPrecursorMass, dDDERTReserved1);
							/*std::cout << "DDERealTimeData("<<lExperimentIndex<<", "<<lDataPointIndex<<"):" << std::endl;
							std::cout << "  RTPrecursorMass=" << dDDERTPrecursorMass << ",RTIndex=" << lDDERTIndex << ",RTCount=" << lDDERTCount << std::endl;
							std::cout << "  RTReserved1=" << dDDERTReserved1 << ",RTReserved2=" << dDDERTReserved2 << std::endl;*/
						}
						/*if (SUCCEEDED(hr)) {
							std::cout << "DDERealTimeData():" << std::endl;
							std::cout << "  RTPrecursorMass=" << dDDERTPrecursorMass << ",RTIndex=" << lDDERTIndex << ",RTCount=" << lDDERTCount << std::endl;
							std::cout << "  RTReserved1=" << dDDERTReserved1 << ",RTReserved2=" << dDDERTReserved2 << std::endl;
						} else {
							std::cerr << "DDERealTimeData(): hr="  << std::hex << hr << std::dec << std::endl;
							break;
						}*/
					}
				}
				ddeData.sampleCached(lSampleIndex, lPeriodIndex);
			} catch (... /*_com_error &com_error*/) {
				// do nothing
				//std::cerr << "Error: com_error in DDE processing, hr=" << std::hex << hr << std::dec << std::endl;
			}
		}
		hr = ipWIFFTSvrDDERealTimeData->CloseArrStrCLS_DDERealTimeData(lStreamDataObject);
	} catch (... /*_com_error &com_error*/) {
		// do nothing
		//std::cerr << "Error: com_error in DDE processing, hr=" << std::hex << hr << std::dec << std::endl;
	}
}

void AnalystQSInterface::getSampleLog(void) {
	if (m_strSampleLog.empty()) {
		IUnknownPtr ipUnknownFileManagerObject(m_ipFMANWiffFile->GetFileManagerObject());

		IFMProcessingPtr ipIFMProcessing;
		HRESULT hr = ipUnknownFileManagerObject->QueryInterface(&ipIFMProcessing);

		{
			long lKeyTypeIndex = 0;	// value 10 is taken based on trace
			// TODO: how to integrate the sample index
			short sSampleId = 1;
			hr = ipIFMProcessing->FMProcEnumLogInit(sSampleId, lKeyTypeIndex);

			LPWSTR lpwstr=NULL;
			do
			{
				lpwstr=NULL;
				long lKeyType=0, lTimeOff=0;
				hr = ipIFMProcessing->FMProcEnumLogNext(sSampleId, &lpwstr, &lKeyType, &lTimeOff);

				if (NULL != lpwstr) {
					USES_CONVERSION;
					std::string strLogs = W2A(lpwstr);
					::SysFreeString(lpwstr);

					if (m_iVerbose>=VERBOSE_DEBUG) {
						std::cout << "DEBUG: Sample log" << std::endl;
						std::cout << "DEBUG: " << strLogs.c_str() << std::endl;
					}

					// ASSUMPTION: keep first log with "Mass Spectrometer:"
					if (m_strSampleLog.empty()) {
						const std::string prefix("Mass Spectrometer:");
						if (strLogs.npos != strLogs.find(prefix)) {
							m_strSampleLog = strLogs;
							if (m_iVerbose<VERBOSE_DEBUG) {
								break;
							}
						}
					}
				}
			} while (lpwstr!=NULL);
			hr = ipIFMProcessing->FMProcEnumLogReset(sSampleId);
		}
	}
}

inline void AnalystQSInterface::loadMergedScans (long lSampleIndex, long lPeriodIndex) {
	MergedScanTableCache::const_iterator iterCache= m_sampleMergedScansCache.find(SamplePeriodKey(lSampleIndex, lPeriodIndex));
	if (iterCache != m_sampleMergedScansCache.end()) {
		std::cout << "Computing merge scans";
		long lIndex=0;
		for(MergedScanTable::MergedScans::const_iterator iter=iterCache->second.m_experimentsMergedScans.begin();
			iter != iterCache->second.m_experimentsMergedScans.end();
			++iter) {
			lIndex++;
			if ((lIndex % 100) == 0) {
				std::cout << "M" << lIndex;
			}
			else if ((lIndex % 10) == 0) {
				std::cout << ".";
			}
			m_iteratorState.pushMergedScan(instantiateMergedScans (lSampleIndex, lPeriodIndex, *iter));
		}
		if (0!=(lIndex % 100)) {
			std::cout << "*M" << lIndex << "*";
		}
		if (m_iVerbose>=VERBOSE_DEBUG) {
			if (lIndex>0) std::cout << "DEBUG: merged scan peak list" << std::endl;
			lIndex=0;
			for(std::deque<ScanPtr>::const_iterator iter=m_iteratorState.m_queueMergedScans.begin();
				iter != m_iteratorState.m_queueMergedScans.end();
				++iter) {
				lIndex++;
				ScanPtr pScan = *iter;
				std::cout << "Merged Scan#" << lIndex << ": " << pScan->precursorList_.back().MZ_ 
					<< ", " << pScan->precursorList_.back().intensity_ 
					<< ", " << pScan->precursorList_.back().charge_ << std::endl;
				for (int i=0; i<pScan->getNumDataPoints(); i++) {
					std::cout << "pk#" << (i+1) 
						<< "\t" << pScan->mzArray_[i] 
						<< "\t" << pScan->intensityArray_[i] 
						<< std::endl;
				}
			}			
		}
	}
}

inline void AnalystQSInterface::initScanIteratorState () {

	curScanNumber_ = 0;

	m_iteratorState.init();

	getSamplePrecursorInfo (m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);

	m_iteratorState.initNewWiff(m_ipFMANWiffFile->GetActualNumberOfSamples(), 
		m_ipFMANWiffFile->GetActualNumberOfPeriods(m_iteratorState.m_lCurrentSampleIndex), 
		m_ipFMANWiffFile->GetNumberOfExperiments(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex));

	//starts with 1
	HRESULT hr = m_ipFMANChromData->raw_SetToTIC(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex, 0);
	if (FAILED(hr)) {
		std::cerr << "FATAL: Chromatogram.TIC("<<m_iteratorState.m_lCurrentSampleIndex<<", "<<m_iteratorState.m_lCurrentPeriodIndex<<", 0), COM error "
			<< std::hex << hr << std::dec << std::endl;
		_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
	}
	m_iteratorState.initNewDatapoints(m_ipFMANChromData->GetNumberOfDataPoints ());

	IUnknownPtr ipUnknownPF(m_ipPeakFinderFactory->CreatePeakFinder (m_bstrWiffFileName, m_iteratorState.m_lCurrentSampleIndex, FACTORY_SPECTRUM, &m_paramsString));
	ipUnknownPF->QueryInterface(&m_ipPeakFinder);

	determineRetentionTimeType(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);

	// set up the merged scan
	loadMergedScans (m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);
	// re-position data stream
	hr = m_ipFMANChromData->raw_SetToTIC(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex, 0);
	if (FAILED(hr)) {
		std::cerr << "FATAL: Chromatogram.TIC("<<m_iteratorState.m_lCurrentSampleIndex<<", "<<m_iteratorState.m_lCurrentPeriodIndex<<", 0), COM error "
			<< std::hex << hr << std::dec << std::endl;
		_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
	}
}

inline bool AnalystQSInterface::nextScanIteratorState () {

	ScanIteratorState::IteratorStateType state = m_iteratorState.next ();

	if (ScanIteratorState::STATE_NO_TRANSITION==state) {
		HRESULT hr = m_ipFMANChromData->SetToTIC(
			m_iteratorState.m_lCurrentSampleIndex, 
			m_iteratorState.m_lCurrentPeriodIndex, 
			m_iteratorState.m_lCurrentExperimentIndex);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Chromatogram.TIC("
				<<m_iteratorState.m_lCurrentSampleIndex<<", "
				<<m_iteratorState.m_lCurrentPeriodIndex<<", "
				<<m_iteratorState.m_lCurrentExperimentIndex<<"), COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}

		return true;

	} else if (ScanIteratorState::STATE_NEW_SAMPLE==state) {
		// we have moved to a new sample
		// number of periods, number of experiments, and number of data points can changed

		getSamplePrecursorInfo (m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);

		m_iteratorState.initNewSample (m_ipFMANWiffFile->GetActualNumberOfPeriods(m_iteratorState.m_lCurrentSampleIndex),
			m_ipFMANWiffFile->GetNumberOfExperiments(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex));

		HRESULT hr = m_ipFMANChromData->raw_SetToTIC(
			m_iteratorState.m_lCurrentSampleIndex, 
			m_iteratorState.m_lCurrentPeriodIndex, 0);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Chromatogram.TIC("
				<<m_iteratorState.m_lCurrentSampleIndex<<", "
				<<m_iteratorState.m_lCurrentPeriodIndex<<", "
				<<m_iteratorState.m_lCurrentExperimentIndex<<"), COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}
		m_iteratorState.initNewDatapoints(m_ipFMANChromData->GetNumberOfDataPoints ());

		IUnknownPtr ipUnknownPF(m_ipPeakFinderFactory->CreatePeakFinder (m_bstrWiffFileName, m_iteratorState.m_lCurrentSampleIndex, FACTORY_SPECTRUM, &m_paramsString));
		ipUnknownPF->QueryInterface(&m_ipPeakFinder);

		determineRetentionTimeType(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);

		// set up the merged scan
		loadMergedScans (m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);
		// reposition data stream
		hr = m_ipFMANChromData->raw_SetToTIC(
			m_iteratorState.m_lCurrentSampleIndex, 
			m_iteratorState.m_lCurrentPeriodIndex, 0);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Chromatogram.TIC("
				<<m_iteratorState.m_lCurrentSampleIndex<<", "
				<<m_iteratorState.m_lCurrentPeriodIndex<<", "
				<<m_iteratorState.m_lCurrentExperimentIndex<<"), COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}

		return true;

	} else if (ScanIteratorState::STATE_NEW_PERIOD==state) {
		// we have moved to a new period
		// number of experiments, and number of data points can changed

		getSamplePrecursorInfo (m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);

		m_iteratorState.initNewPeriod(m_ipFMANWiffFile->GetNumberOfExperiments(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex));

		HRESULT hr = m_ipFMANChromData->SetToTIC(
			m_iteratorState.m_lCurrentSampleIndex, 
			m_iteratorState.m_lCurrentPeriodIndex, 0);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Chromatogram.TIC("
				<<m_iteratorState.m_lCurrentSampleIndex<<", "
				<<m_iteratorState.m_lCurrentPeriodIndex<<", 0), COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}

		determineRetentionTimeType(m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);

		// set up the merged scan
		loadMergedScans (m_iteratorState.m_lCurrentSampleIndex, m_iteratorState.m_lCurrentPeriodIndex);
		// reposition data stream
		hr = m_ipFMANChromData->SetToTIC(
			m_iteratorState.m_lCurrentSampleIndex, 
			m_iteratorState.m_lCurrentPeriodIndex, 0);
		if (FAILED(hr)) {
			std::cerr << "FATAL: Chromatogram.TIC("
				<<m_iteratorState.m_lCurrentSampleIndex<<", "
				<<m_iteratorState.m_lCurrentPeriodIndex<<", 0), COM error " 
				<< std::hex << hr << std::dec << std::endl;
			_com_issue_errorex(hr, m_ipFMANChromData.GetInterfacePtr (), m_ipFMANChromData.GetIID());
		}

		m_iteratorState.initNewDatapoints(m_ipFMANChromData->GetNumberOfDataPoints ());

		return true;

	} else if (ScanIteratorState::STATE_MRM_CONTINUE==state) {
		// MRM scans are cached, so no computation needed
		return true;

	} else if (ScanIteratorState::STATE_MERGEDSCAN_CONTINUE==state) {
		// MRM scans are cached, so no computation needed
		return true;

	} else if (ScanIteratorState::STATE_ENDED==state) {
		return false;

	}

	return false;
}

void AnalystQSInterface::buildInstrumentInfo() {
	// TODO:
	// 1. actual implementation
	// 2. take value supply by user?

	std::string strManufacturer;
	getMSManufacturer(strManufacturer);
	if (!strManufacturer.empty()) {
		// default all manufacturer string to ABI_SCIEX
		// as we do not have a full list of corresponding manufacturer
		if (m_iVerbose>=VERBOSE_INFO) {
			std::cout << "INFO: Found Manufacturer tag \"" << strManufacturer.c_str() << "\" in file." << std::endl;
		}
	}

	long lModelId=0;
	std::string strModel;
	getMSModel(lModelId, strModel);
	instrumentInfo_.instrumentModel_ = modelIdToMSInstrumentModel(lModelId);
	if (!strModel.empty()) {
		if (m_iVerbose>=VERBOSE_INFO) {
			std::cout << "INFO: Found Model tag \"" << strModel.c_str() << "\" in file." << std::endl;
		}
	}

	getMSSerialNumber(instrumentInfo_.instrumentSerialNumber_);
	if (!instrumentInfo_.instrumentSerialNumber_.empty()) {
		if (m_iVerbose>=VERBOSE_INFO) {
			std::cout << "INFO: Found Serial Number tag \"" << instrumentInfo_.instrumentSerialNumber_.c_str() << "\" in file." << std::endl;
		}
	}

	getMSHardwareVersion(instrumentInfo_.instrumentHardwareVersion_);
	if (!instrumentInfo_.instrumentHardwareVersion_.empty()) {
		if (m_iVerbose>=VERBOSE_INFO) {
			std::cout << "INFO: Found Hardware Version tag \"" << instrumentInfo_.instrumentHardwareVersion_.c_str() << "\" in file." << std::endl;
		}
	}

	if (IONIZATION_UNDEF == instrumentInfo_.ionSource_) {
		std::string strIonSource;
		getMSDetector(strIonSource);
		if (strIonSource.empty()) {
			strIonSource = "ESI";
		} else {
			// TODO: translate internal text to MSIonizationType
			if (m_iVerbose>=VERBOSE_INFO) {
				std::cout << "INFO: Found Ionization tag \"" << strIonSource.c_str() << "\" in file." << std::endl;
			}
		}
		instrumentInfo_.ionSource_ = MSIonizationTypeFromString(strIonSource);
	}

	if (instrumentInfo_.analyzerList_.size()<1 
		|| ANALYZER_UNDEF == instrumentInfo_.analyzerList_[0]) {
		std::string strAnalyzer;
		getMSMassAnalyzer(strAnalyzer);
		if (strAnalyzer.empty()) {
			strAnalyzer = "TOFMS";
		} else {
			// TODO: translate internal text to MSIonizationType
			if (m_iVerbose>=VERBOSE_INFO) {
				std::cout << "INFO: Found Mass Analyzer tag \"" << strAnalyzer.c_str() << "\" in file." << std::endl;
			}
		}
		if (instrumentInfo_.analyzerList_.empty()) {
			instrumentInfo_.analyzerList_.push_back(MSAnalyzerTypeFromString(strAnalyzer));
		} else {
			instrumentInfo_.analyzerList_[0] = MSAnalyzerTypeFromString(strAnalyzer);
		}
	}

	/*
	std::string strDetector;
	getMSDetector(strDetector);
	*/
	instrumentInfo_.detector_ = DETECTOR_UNDEF;
}

void AnalystQSInterface::buildAcquisitionSoftwareInfo() {
	std::string strFileSignature;
	getCreator (m_strAcquisitionFileName, strFileSignature);

	std::string strSoftware;
	getSoftwareVersion (strFileSignature, strSoftware, 
		instrumentInfo_.acquisitionSoftwareVersion_);

	instrumentInfo_.acquisitionSoftware_ = softwareToMSAcquisitionSoftwareType(strSoftware);
}

//----------

void AnalystQSInterface::setVerbose(bool verbose) {
	verbose_ = verbose;
}
