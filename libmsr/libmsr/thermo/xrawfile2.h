// -*- mode: c++ -*-

/***************************************************************************
                             ReAdW

This program converts Xcalibur native data into mzXML format. The program 
requires the XRawfile library from ThermoFinnigan to run.

                             -------------------
    begin				: Sat Mar 27
    copyright			: (C) 2004 by Pedrioli Patrick, ISB, Proteomics
    email				: ppatrick@student.ethz.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#if !defined(__XRAWFILE2)
#define __XRAWFILE2

#include "common.h"

// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IXRawfile wrapper class

class IXRawfile2 : public COleDispatchDriver
{
public:
	IXRawfile2() {}		// Calls COleDispatchDriver default constructor
	IXRawfile2(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IXRawfile2(const IXRawfile2& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long Open(LPCTSTR szFileName);
	void Close();
	void GetFileName(BSTR* pbstrFileName);
	void GetCreatorID(BSTR* pbstrCreatorID);
	void GetVersionNumber(long* pnVersion);
	void GetCreationDate(DATE* pCreationDate);
	void IsError(long* pbIsError);
	void IsNewFile(long* pbIsNewFile);
	void GetErrorCode(long* pnErrorCode);
	void GetErrorMessage(BSTR* pbstrErrorMessage);
	void GetWarningMessage(BSTR* pbstrWarningMessage);
	void GetSeqRowNumber(long* pnSeqRowNumber);
	void GetSeqRowSampleType(long* pnSampleType);
	void GetSeqRowDataPath(BSTR* pbstrDataPath);
	void GetSeqRowRawFileName(BSTR* pbstrRawFileName);
	void GetSeqRowSampleName(BSTR* pbstrSampleName);
	void GetSeqRowSampleID(BSTR* pbstrSampleID);
	void GetSeqRowComment(BSTR* pbstrComment);
	void GetSeqRowLevelName(BSTR* pbstrLevelName);
	void GetSeqRowUserText(long nIndex, BSTR* pbstrUserText);
	void GetSeqRowInstrumentMethod(BSTR* pbstrInstrumentMethod);
	void GetSeqRowProcessingMethod(BSTR* pbstrProcessingMethod);
	void GetSeqRowCalibrationFile(BSTR* pbstrCalibrationFile);
	void GetSeqRowVial(BSTR* pbstrVial);
	void GetSeqRowInjectionVolume(double* pdInjVol);
	void GetSeqRowSampleWeight(double* pdSampleWt);
	void GetSeqRowSampleVolume(double* pdSampleVolume);
	void GetSeqRowISTDAmount(double* pdISTDAmount);
	void GetSeqRowDilutionFactor(double* pdDilutionFactor);
	void GetSeqRowUserLabel(long nIndex, BSTR* pbstrUserLabel);
	void InAcquisition(long* pbInAcquisition);
	void GetNumberOfControllers(long* pnNumControllers);
	void GetControllerType(long nIndex, long* pnControllerType);
	void SetCurrentController(long nControllerType, long nControllerNumber);
	void GetCurrentController(long* pnControllerType, long* pnControllerNumber);
	void GetNumSpectra(long* pnNumberOfSpectra);
	void GetNumStatusLog(long* pnNumberOfStatusLogEntries);
	void GetNumErrorLog(long* pnNumberOfErrorLogEntries);
	void GetNumTuneData(long* pnNumTuneData);
	void GetMassResolution(double* pdMassResolution);
	void GetExpectedRunTime(double* pdExpectedRunTime);
	void GetNumTrailerExtra(long* pnNumberOfTrailerExtraEntries);
	void GetLowMass(double* pdLowMass);
	void GetHighMass(double* pdHighMass);
	void GetStartTime(double* pdStartTime);
	void GetEndTime(double* pdEndTime);
	void GetMaxIntegratedIntensity(double* pdMaxIntegIntensity);
	void GetMaxIntensity(long* pnMaxIntensity);
	void GetFirstSpectrumNumber(long* pnFirstSpectrum);
	void GetLastSpectrumNumber(long* pnLastSpectrum);
	void GetInstrumentID(long* pnInstrumentID);
	void GetInletID(long* pnInletID);
	void GetErrorFlag(long* pnErrorFlag);
	void GetSampleVolume(double* pdSampleVolume);
	void GetSampleWeight(double* pdSampleWeight);
	void GetVialNumber(long* pnVialNumber);
	void GetInjectionVolume(double* pdInjectionVolume);
	void GetFlags(BSTR* pbstrFlags);
	void GetAcquisitionFileName(BSTR* pbstrFileName);
	void GetInstrumentDescription(BSTR* pbstrInstrumentDescription);
	void GetAcquisitionDate(BSTR* pbstrAcquisitionDate);
	void GetOperator(BSTR* pbstrOperator);
	void GetComment1(BSTR* pbstrComment1);
	void GetComment2(BSTR* pbstrComment2);
	void GetSampleAmountUnits(BSTR* pbstrSampleAmountUnits);
	void GetInjectionAmountUnits(BSTR* pbstrInjectionAmountUnits);
	void GetSampleVolumeUnits(BSTR* pbstrSampleVolumeUnits);
	void GetFilters(VARIANT* pvarFilterArray, long* pnArraySize);
	void ScanNumFromRT(double dRT, long* pnScanNumber);
	void RTFromScanNum(long nScanNumber, double* pdRT);
	void GetFilterForScanNum(long nScanNumber, BSTR* pbstrFilter);
	void GetFilterForScanRT(double dRT, BSTR* pbstrFilter);
	void GetMassListFromScanNum(long* pnScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, 
		VARIANT* pvarPeakFlags, long* pnArraySize);
	void GetMassListFromRT(double* pdRT, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, VARIANT* pvarPeakFlags, 
		long* pnArraySize);
	void GetNextMassListFromScanNum(long* pnScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, 
		VARIANT* pvarPeakFlags, long* pnArraySize);
	void GetPrevMassListFromScanNum(long* pnScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, 
		VARIANT* pvarPeakFlags, long* pnArraySize);
	void IsProfileScanForScanNum(long nScanNumber, long* pbIsProfileScan);
	void IsCentroidScanForScanNum(long nScanNumber, long* pbIsCentroidScan);
	void GetScanHeaderInfoForScanNum(long nScanNumber, long* pnNumPackets, double* pdStartTime, double* pdLowMass, double* pdHighMass, double* pdTIC, double* pdBasePeakMass, double* pdBasePeakIntensity, long* pnNumChannels, long* pbUniformTime, 
		double* pdFrequency);
	void GetStatusLogForScanNum(long nScanNumber, double* pdStatusLogRT, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize);
	void GetStatusLogForRT(double* pdRT, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize);
	void GetStatusLogLabelsForScanNum(long nScanNumber, double* pdStatusLogRT, VARIANT* pvarLabels, long* pnArraySize);
	void GetStatusLogLabelsForRT(double* pdRT, VARIANT* pvarLabels, long* pnArraySize);
	void GetStatusLogValueForScanNum(long nScanNumber, LPCTSTR bstrLabel, double* pdStatusLogRT, VARIANT* pvarValue);
	void GetStatusLogValueForRT(double* pdRT, LPCTSTR bstrLabel, VARIANT* pvarValue);
	void GetTrailerExtraForScanNum(long nScanNumber, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize);
	void GetTrailerExtraForRT(double* pdRT, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize);
	long GetTrailerExtraLabelsForScanNum(long nScanNumber, VARIANT* pvarLabels, long* pnArraySize);
	void GetTrailerExtraLabelsForRT(double* pdRT, VARIANT* pvarLabels, long* pnArraySize);
	long GetTrailerExtraValueForScanNum(long nScanNumber, LPCTSTR bstrLabel, VARIANT* pvarValue);
	void GetTrailerExtraValueForRT(double* pdRT, LPCTSTR bstrLabel, VARIANT* pvarValue);
	void GetErrorLogItem(long nItemNumber, double* pdRT, BSTR* pbstrErrorMessage);
	void GetTuneData(long nSegmentNumber, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize);
	void GetNumInstMethods(long* pnNumInstMethods);
	void GetInstMethod(long nInstMethodItem, BSTR* pbstrInstMethod);
	void GetChroData(long nChroType1, long nChroOperator, long nChroType2, LPCTSTR bstrFilter, LPCTSTR bstrMassRanges1, LPCTSTR bstrMassRanges2, double dDelay, double* pdStartTime, double* pdEndTime, long nSmoothingType, long nSmoothingValue, 
		VARIANT* pvarChroData, VARIANT* pvarPeakFlags, long* pnArraySize);
	void RefreshViewOfFile();
	void GetTuneDataValue(long nSegmentNumber, LPCTSTR bstrLabel, VARIANT* pvarValue);
	void GetTuneDataLabels(long nSegmentNumber, VARIANT* pvarLabels, long* pnArraySize);
	void GetInstName(BSTR* pbstrInstName);
	void GetInstModel(BSTR* pbstrInstModel);
	void GetInstSerialNumber(BSTR* pbstrInstSerialNumber);
	void GetInstSoftwareVersion(BSTR* pbstrInstSoftwareVersion);
	void GetInstHardwareVersion(BSTR* pbstrInstHardwareVersion);
	void GetInstFlags(BSTR* pbstrInstFlags);
	void GetInstNumChannelLabels(long* pnInstNumChannelLabels);
	void GetInstChannelLabel(long nChannelLabelNumber, BSTR* pbstrInstChannelLabel);
	void GetNumberOfControllersOfType(long nControllerType, long* pnNumControllersOfType);
	void GetAverageMassList(long* pnFirstAvgScanNumber, long* pnLastAvgScanNumber, long* pnFirstBkg1ScanNumber, long* pnLastBkg1ScanNumber, long* pnFirstBkg2ScanNumber, long* pnLastBkg2ScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, 
		long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, VARIANT* pvarPeakFlags, long* pnArraySize);
	void IsThereMSData(long* pbMSData);
	void HasExpMethod(long* pbMethod);
	void GetFilterMassPrecision(long* pnFilterMassPrecision);
	void GetStatusLogForPos(long nPos, VARIANT* pvarRT, VARIANT* pvarValue, long* pnArraySize);
	void GetStatusLogPlottableIndex(VARIANT* pvarIndex, VARIANT* pvarValues, long* pnArraySize);
	void GetInstMethodNames(long* pnNumInstMethods, VARIANT* pvarNames);
	void SetMassTolerance(long bUseUserDefined, double dMassTolerance, long nUnits);
	void GetChros(long nNumChros, double* pdStartTime, double* pdEndTime, VARIANT* pvarChroParamsArray, VARIANT* pvarChroDataSizeArray, VARIANT* pvarChroDataArray, VARIANT* pvarPeakFlagsArray);
	void GetLabelData(VARIANT* pvarLabels, VARIANT* pvarFlags, long* pnScanNumber);
};

#endif //__XRAWFILE2
