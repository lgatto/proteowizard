// -*- mode: c++ -*-


/*
    File: AnalystInterface.h
    Description: Implementation of instrument interface for data acquired 
                 with Analyst.  The program depends on the general framework 
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


#pragma once

#include <string>
#include "SHA1.h"
#include "BaseInstrumentInterface.h"
#include "AnalystImpl.h"
#include "wifflib/analysttlbs.h"

class AnalystInterface : public BaseInstrumentInterface {
private:
	long firstScanNumber_;
	long lastScanNumber_;
	bool firstTime_;
	bool preferVendorCentroid_;

	LibraryTagType m_enumLibraryType;

	bool m_fInitialized;
	bool m_fCOMInitialized;

	std::string m_strAcquisitionFileName;
	_bstr_t     m_bstrWiffFileName;

	Analyst::IFMANChromDataPtr     m_ipFMANChromData;
	Analyst::IFMANSpecDataPtr      m_ipFMANSpecData;
	Analyst::IFMANWiffFilePtr      m_ipFMANWiffFile;
	Analyst::IPeakFinderFactoryPtr m_ipPeakFinderFactory;
	Analyst::IPeakFinder2Ptr       m_ipPeakFinder;
	BSTR                           m_paramsString;
	Analyst::IFMANSpecDataPtr      m_ipFMANSpecDataToMerge;

	DDEData                m_sampleDDE;
	ScanTable              m_sampleScanMap;
	MergedScanTableCache   m_sampleMergedScansCache;
	ScanOriginToMergeTable m_scanOriginToMergeTable;
	double                 m_dChromotographRTInSec;
	bool                   m_fUseExperiment0RT;

	//TODO:temporarily, have to change the interface and logics
	Scan *m_pScan;

	int m_iVerbose;

	SHA1 sha1_; // md5sum calculator
	char sha1Report_[1024]; // sha1 hash

private:
	void initScanIteratorState (void);
	bool nextScanIteratorState (void);
	ScanIteratorState m_iteratorState;

private:
	void termInterface(void);

	Scan* getFirstScan(void);
	Scan* getNextScan(void);
	bool isValidScan(void);

	void determineRetentionTimeType(long lSampleIndex, long lPeriodIndex);
	void processSpectrum(Scan *pScan);
	void processMSSpectrum(Scan *pScan);
	void processMRMSpectrum(Scan *pScan);
	void processMSSpecData(Scan *pScan);
	void centroidSpecData(bool doDeisotope, double &maxIntensity);

	void getMSRunTime (void);
	void computeScansCount (void);
	void getSamplePrecursorInfo (long lSampleIndex, long lPeriodIndex);
	void getSamplePrecursorInfo (DDEData &ddeData, long lSampleIndex, long lPeriodIndex);
	bool getExperimentsInfo(long lSampleIndex, long lPeriodIndex, std::vector<MSScanType> &experimentsScanType, std::vector<long> &experimentsPolarity, std::vector<long> &experimentsUseMCAScans);
	void mergeScans (long lSampleIndex, long lPeriodIndex, std::vector<MSScanType> experimentsScanType, const ScanTable &scanTable, long &lMergedScanCount, long &lTotalScansConsidered, MergedScanTable &mergedScans);
	ScanPtr instantiateMergedScans (long lSampleIndex, long lPeriodIndex, const MergedScan &mergedScan);
	void loadMergedScans (long lSampleIndex, long lPeriodIndex);

	void buildInstrumentInfo(void);
	void buildAcquisitionSoftwareInfo(void);

	int getCreator(const std::string &strFilename, std::string &strCreator);
	void getMSManufacturer(std::string &strManufacturer);
	void getMSModel(long &lModelId, std::string &strMode);
	void getMSSerialNumber(std::string &strSerialNumber);
	void getMSHardwareVersion(std::string &strHardwareVersion);
	void getMSIonisation(std::string &strIonisation);
	void getMSMassAnalyzer(std::string &strMassAnalyzer);
	void getMSDetector(std::string &strDetector);

	void getSampleLog(void);
	std::string m_strSampleLog;

public:
	long curScanNumber_;
	MSDataProcessing m_DPSettings;

public:
	AnalystInterface(void);
	~AnalystInterface(void);
	void setVerbose(int iVerbose);

	virtual bool initInterface(void);
	virtual bool setInputFile(const std::string& fileName, const ProcessingOptions& options);

	virtual void setProcessingOptions(const ProcessingOptions& options)
	{
		m_DPSettings.init(options);
	}

	static ProcessingOptions getDefaultProcessingOptions()
	{
		return MSDataProcessing::getDefaultProcessingOptions();
	}

	virtual void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor);
	virtual void setDeisotoping(bool deisotope);
	virtual void setVerbose(bool verbose);
	virtual Scan* getScan(void);
};
