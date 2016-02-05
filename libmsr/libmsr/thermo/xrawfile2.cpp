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

// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "xrawfile2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IXRawfile properties

/////////////////////////////////////////////////////////////////////////////
// IXRawfile operations

long IXRawfile2::Open(LPCTSTR szFileName)
{
	long ret;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, &ret, parms,
		 szFileName);

	return ret;
}

void IXRawfile2::Close()
{
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IXRawfile2::GetFileName(BSTR* pbstrFileName)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrFileName);
}

void IXRawfile2::GetCreatorID(BSTR* pbstrCreatorID)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrCreatorID);
}

void IXRawfile2::GetVersionNumber(long* pnVersion)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnVersion);
}

void IXRawfile2::GetCreationDate(DATE* pCreationDate)
{
	static BYTE parms[] =
		VTS_PDATE;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pCreationDate);
}

void IXRawfile2::IsError(long* pbIsError)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbIsError);
}

void IXRawfile2::IsNewFile(long* pbIsNewFile)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbIsNewFile);
}

void IXRawfile2::GetErrorCode(long* pnErrorCode)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnErrorCode);
}

void IXRawfile2::GetErrorMessage(BSTR* pbstrErrorMessage)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrErrorMessage);
}

void IXRawfile2::GetWarningMessage(BSTR* pbstrWarningMessage)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrWarningMessage);
}

void IXRawfile2::GetSeqRowNumber(long* pnSeqRowNumber)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnSeqRowNumber);
}

void IXRawfile2::GetSeqRowSampleType(long* pnSampleType)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnSampleType);
}

void IXRawfile2::GetSeqRowDataPath(BSTR* pbstrDataPath)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrDataPath);
}

void IXRawfile2::GetSeqRowRawFileName(BSTR* pbstrRawFileName)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrRawFileName);
}

void IXRawfile2::GetSeqRowSampleName(BSTR* pbstrSampleName)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrSampleName);
}

void IXRawfile2::GetSeqRowSampleID(BSTR* pbstrSampleID)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrSampleID);
}

void IXRawfile2::GetSeqRowComment(BSTR* pbstrComment)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrComment);
}

void IXRawfile2::GetSeqRowLevelName(BSTR* pbstrLevelName)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrLevelName);
}

void IXRawfile2::GetSeqRowUserText(long nIndex, BSTR* pbstrUserText)
{
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nIndex, pbstrUserText);
}

void IXRawfile2::GetSeqRowInstrumentMethod(BSTR* pbstrInstrumentMethod)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstrumentMethod);
}

void IXRawfile2::GetSeqRowProcessingMethod(BSTR* pbstrProcessingMethod)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrProcessingMethod);
}

void IXRawfile2::GetSeqRowCalibrationFile(BSTR* pbstrCalibrationFile)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrCalibrationFile);
}

void IXRawfile2::GetSeqRowVial(BSTR* pbstrVial)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrVial);
}

void IXRawfile2::GetSeqRowInjectionVolume(double* pdInjVol)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdInjVol);
}

void IXRawfile2::GetSeqRowSampleWeight(double* pdSampleWt)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdSampleWt);
}

void IXRawfile2::GetSeqRowSampleVolume(double* pdSampleVolume)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdSampleVolume);
}

void IXRawfile2::GetSeqRowISTDAmount(double* pdISTDAmount)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdISTDAmount);
}

void IXRawfile2::GetSeqRowDilutionFactor(double* pdDilutionFactor)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdDilutionFactor);
}

void IXRawfile2::GetSeqRowUserLabel(long nIndex, BSTR* pbstrUserLabel)
{
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nIndex, pbstrUserLabel);
}

void IXRawfile2::InAcquisition(long* pbInAcquisition)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbInAcquisition);
}

void IXRawfile2::GetNumberOfControllers(long* pnNumControllers)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumControllers);
}

void IXRawfile2::GetControllerType(long nIndex, long* pnControllerType)
{
	static BYTE parms[] =
		VTS_I4 VTS_PI4;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nIndex, pnControllerType);
}

void IXRawfile2::SetCurrentController(long nControllerType, long nControllerNumber)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nControllerType, nControllerNumber);
}

void IXRawfile2::GetCurrentController(long* pnControllerType, long* pnControllerNumber)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnControllerType, pnControllerNumber);
}

void IXRawfile2::GetNumSpectra(long* pnNumberOfSpectra)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x24, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumberOfSpectra);
}

void IXRawfile2::GetNumStatusLog(long* pnNumberOfStatusLogEntries)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x25, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumberOfStatusLogEntries);
}

void IXRawfile2::GetNumErrorLog(long* pnNumberOfErrorLogEntries)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x26, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumberOfErrorLogEntries);
}

void IXRawfile2::GetNumTuneData(long* pnNumTuneData)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x27, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumTuneData);
}

void IXRawfile2::GetMassResolution(double* pdMassResolution)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdMassResolution);
}

void IXRawfile2::GetExpectedRunTime(double* pdExpectedRunTime)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdExpectedRunTime);
}

void IXRawfile2::GetNumTrailerExtra(long* pnNumberOfTrailerExtraEntries)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumberOfTrailerExtraEntries);
}

void IXRawfile2::GetLowMass(double* pdLowMass)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdLowMass);
}

void IXRawfile2::GetHighMass(double* pdHighMass)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdHighMass);
}

void IXRawfile2::GetStartTime(double* pdStartTime)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x2d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdStartTime);
}

void IXRawfile2::GetEndTime(double* pdEndTime)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x2e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdEndTime);
}

void IXRawfile2::GetMaxIntegratedIntensity(double* pdMaxIntegIntensity)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x2f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdMaxIntegIntensity);
}

void IXRawfile2::GetMaxIntensity(long* pnMaxIntensity)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x30, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnMaxIntensity);
}

void IXRawfile2::GetFirstSpectrumNumber(long* pnFirstSpectrum)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x31, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnFirstSpectrum);
}

void IXRawfile2::GetLastSpectrumNumber(long* pnLastSpectrum)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x32, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnLastSpectrum);
}

void IXRawfile2::GetInstrumentID(long* pnInstrumentID)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x33, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnInstrumentID);
}

void IXRawfile2::GetInletID(long* pnInletID)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x34, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnInletID);
}

void IXRawfile2::GetErrorFlag(long* pnErrorFlag)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x35, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnErrorFlag);
}

void IXRawfile2::GetSampleVolume(double* pdSampleVolume)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x36, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdSampleVolume);
}

void IXRawfile2::GetSampleWeight(double* pdSampleWeight)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdSampleWeight);
}

void IXRawfile2::GetVialNumber(long* pnVialNumber)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x38, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnVialNumber);
}

void IXRawfile2::GetInjectionVolume(double* pdInjectionVolume)
{
	static BYTE parms[] =
		VTS_PR8;
	InvokeHelper(0x39, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdInjectionVolume);
}

void IXRawfile2::GetFlags(BSTR* pbstrFlags)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrFlags);
}

void IXRawfile2::GetAcquisitionFileName(BSTR* pbstrFileName)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrFileName);
}

void IXRawfile2::GetInstrumentDescription(BSTR* pbstrInstrumentDescription)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstrumentDescription);
}

void IXRawfile2::GetAcquisitionDate(BSTR* pbstrAcquisitionDate)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrAcquisitionDate);
}

void IXRawfile2::GetOperator(BSTR* pbstrOperator)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrOperator);
}

void IXRawfile2::GetComment1(BSTR* pbstrComment1)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x3f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrComment1);
}

void IXRawfile2::GetComment2(BSTR* pbstrComment2)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x40, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrComment2);
}

void IXRawfile2::GetSampleAmountUnits(BSTR* pbstrSampleAmountUnits)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x41, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrSampleAmountUnits);
}

void IXRawfile2::GetInjectionAmountUnits(BSTR* pbstrInjectionAmountUnits)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x42, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInjectionAmountUnits);
}

void IXRawfile2::GetSampleVolumeUnits(BSTR* pbstrSampleVolumeUnits)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrSampleVolumeUnits);
}

void IXRawfile2::GetFilters(VARIANT* pvarFilterArray, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pvarFilterArray, pnArraySize);
}

void IXRawfile2::ScanNumFromRT(double dRT, long* pnScanNumber)
{
	static BYTE parms[] =
		VTS_R8 VTS_PI4;
	InvokeHelper(0x45, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 dRT, pnScanNumber);
}

void IXRawfile2::RTFromScanNum(long nScanNumber, double* pdRT)
{
	static BYTE parms[] =
		VTS_I4 VTS_PR8;
	InvokeHelper(0x46, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pdRT);
}

void IXRawfile2::GetFilterForScanNum(long nScanNumber, BSTR* pbstrFilter)
{
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pbstrFilter);
}

void IXRawfile2::GetFilterForScanRT(double dRT, BSTR* pbstrFilter)
{
	static BYTE parms[] =
		VTS_R8 VTS_PBSTR;
	InvokeHelper(0x48, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 dRT, pbstrFilter);
}

void IXRawfile2::GetMassListFromScanNum(long* pnScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, 
		VARIANT* pvarPeakFlags, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PI4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x49, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnScanNumber, bstrFilter, nIntensityCutoffType, nIntensityCutoffValue, nMaxNumberOfPeaks, bCentroidResult, pdCentroidPeakWidth, pvarMassList, pvarPeakFlags, pnArraySize);
}

void IXRawfile2::GetMassListFromRT(double* pdRT, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, VARIANT* pvarPeakFlags, 
		long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PR8 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x4a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, bstrFilter, nIntensityCutoffType, nIntensityCutoffValue, nMaxNumberOfPeaks, bCentroidResult, pdCentroidPeakWidth, pvarMassList, pvarPeakFlags, pnArraySize);
}

void IXRawfile2::GetNextMassListFromScanNum(long* pnScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, 
		VARIANT* pvarPeakFlags, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PI4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x4b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnScanNumber, bstrFilter, nIntensityCutoffType, nIntensityCutoffValue, nMaxNumberOfPeaks, bCentroidResult, pdCentroidPeakWidth, pvarMassList, pvarPeakFlags, pnArraySize);
}

void IXRawfile2::GetPrevMassListFromScanNum(long* pnScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, 
		VARIANT* pvarPeakFlags, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PI4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x4c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnScanNumber, bstrFilter, nIntensityCutoffType, nIntensityCutoffValue, nMaxNumberOfPeaks, bCentroidResult, pdCentroidPeakWidth, pvarMassList, pvarPeakFlags, pnArraySize);
}

void IXRawfile2::IsProfileScanForScanNum(long nScanNumber, long* pbIsProfileScan)
{
	static BYTE parms[] =
		VTS_I4 VTS_PI4;
	InvokeHelper(0x4d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pbIsProfileScan);
}

void IXRawfile2::IsCentroidScanForScanNum(long nScanNumber, long* pbIsCentroidScan)
{
	static BYTE parms[] =
		VTS_I4 VTS_PI4;
	InvokeHelper(0x4e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pbIsCentroidScan);
}

void IXRawfile2::GetScanHeaderInfoForScanNum(long nScanNumber, long* pnNumPackets, double* pdStartTime, double* pdLowMass, double* pdHighMass, double* pdTIC, double* pdBasePeakMass, double* pdBasePeakIntensity, long* pnNumChannels, long* pbUniformTime, 
		double* pdFrequency)
{
	static BYTE parms[] =
		VTS_I4 VTS_PI4 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4 VTS_PR8;
	InvokeHelper(0x4f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pnNumPackets, pdStartTime, pdLowMass, pdHighMass, pdTIC, pdBasePeakMass, pdBasePeakIntensity, pnNumChannels, pbUniformTime, pdFrequency);
}

void IXRawfile2::GetStatusLogForScanNum(long nScanNumber, double* pdStatusLogRT, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x50, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pdStatusLogRT, pvarLabels, pvarValues, pnArraySize);
}

void IXRawfile2::GetStatusLogForRT(double* pdRT, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x51, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, pvarLabels, pvarValues, pnArraySize);
}

void IXRawfile2::GetStatusLogLabelsForScanNum(long nScanNumber, double* pdStatusLogRT, VARIANT* pvarLabels, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x52, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pdStatusLogRT, pvarLabels, pnArraySize);
}

void IXRawfile2::GetStatusLogLabelsForRT(double* pdRT, VARIANT* pvarLabels, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PR8 VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x53, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, pvarLabels, pnArraySize);
}

void IXRawfile2::GetStatusLogValueForScanNum(long nScanNumber, LPCTSTR bstrLabel, double* pdStatusLogRT, VARIANT* pvarValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_PR8 VTS_PVARIANT;
	InvokeHelper(0x54, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, bstrLabel, pdStatusLogRT, pvarValue);
}

void IXRawfile2::GetStatusLogValueForRT(double* pdRT, LPCTSTR bstrLabel, VARIANT* pvarValue)
{
	static BYTE parms[] =
		VTS_PR8 VTS_BSTR VTS_PVARIANT;
	InvokeHelper(0x55, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, bstrLabel, pvarValue);
}

void IXRawfile2::GetTrailerExtraForScanNum(long nScanNumber, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x56, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nScanNumber, pvarLabels, pvarValues, pnArraySize);
}

void IXRawfile2::GetTrailerExtraForRT(double* pdRT, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x57, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, pvarLabels, pvarValues, pnArraySize);
}

long IXRawfile2::GetTrailerExtraLabelsForScanNum(long nScanNumber, VARIANT* pvarLabels, long* pnArraySize)
{
	long ret;

	static BYTE parms[] =
		VTS_I4 VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x58, DISPATCH_METHOD, VT_EMPTY, &ret, parms,
		 nScanNumber, pvarLabels, pnArraySize);

	return ret;
}

void IXRawfile2::GetTrailerExtraLabelsForRT(double* pdRT, VARIANT* pvarLabels, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PR8 VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x59, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, pvarLabels, pnArraySize);
}

long IXRawfile2::GetTrailerExtraValueForScanNum(long nScanNumber, LPCTSTR bstrLabel, VARIANT* pvarValue)
{
	long ret;

	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_PVARIANT;
	InvokeHelper(0x5a, DISPATCH_METHOD, VT_EMPTY, &ret, parms,
		 nScanNumber, bstrLabel, pvarValue);

	return ret;
}

void IXRawfile2::GetTrailerExtraValueForRT(double* pdRT, LPCTSTR bstrLabel, VARIANT* pvarValue)
{
	static BYTE parms[] =
		VTS_PR8 VTS_BSTR VTS_PVARIANT;
	InvokeHelper(0x5b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRT, bstrLabel, pvarValue);
}

void IXRawfile2::GetErrorLogItem(long nItemNumber, double* pdRT, BSTR* pbstrErrorMessage)
{
	static BYTE parms[] =
		VTS_I4 VTS_PR8 VTS_PBSTR;
	InvokeHelper(0x5c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nItemNumber, pdRT, pbstrErrorMessage);
}

void IXRawfile2::GetTuneData(long nSegmentNumber, VARIANT* pvarLabels, VARIANT* pvarValues, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x5d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nSegmentNumber, pvarLabels, pvarValues, pnArraySize);
}

void IXRawfile2::GetNumInstMethods(long* pnNumInstMethods)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x5e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumInstMethods);
}

void IXRawfile2::GetInstMethod(long nInstMethodItem, BSTR* pbstrInstMethod)
{
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x5f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nInstMethodItem, pbstrInstMethod);
}

void IXRawfile2::GetChroData(long nChroType1, long nChroOperator, long nChroType2, LPCTSTR bstrFilter, LPCTSTR bstrMassRanges1, LPCTSTR bstrMassRanges2, double dDelay, double* pdStartTime, double* pdEndTime, long nSmoothingType, long nSmoothingValue, 
		VARIANT* pvarChroData, VARIANT* pvarPeakFlags, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_R8 VTS_PR8 VTS_PR8 VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x60, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nChroType1, nChroOperator, nChroType2, bstrFilter, bstrMassRanges1, bstrMassRanges2, dDelay, pdStartTime, pdEndTime, nSmoothingType, nSmoothingValue, pvarChroData, pvarPeakFlags, pnArraySize);
}

void IXRawfile2::RefreshViewOfFile()
{
	InvokeHelper(0x61, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IXRawfile2::GetTuneDataValue(long nSegmentNumber, LPCTSTR bstrLabel, VARIANT* pvarValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_PVARIANT;
	InvokeHelper(0x62, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nSegmentNumber, bstrLabel, pvarValue);
}

void IXRawfile2::GetTuneDataLabels(long nSegmentNumber, VARIANT* pvarLabels, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nSegmentNumber, pvarLabels, pnArraySize);
}

void IXRawfile2::GetInstName(BSTR* pbstrInstName)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x64, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstName);
}

void IXRawfile2::GetInstModel(BSTR* pbstrInstModel)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstModel);
}

void IXRawfile2::GetInstSerialNumber(BSTR* pbstrInstSerialNumber)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x66, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstSerialNumber);
}

void IXRawfile2::GetInstSoftwareVersion(BSTR* pbstrInstSoftwareVersion)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstSoftwareVersion);
}

void IXRawfile2::GetInstHardwareVersion(BSTR* pbstrInstHardwareVersion)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstHardwareVersion);
}

void IXRawfile2::GetInstFlags(BSTR* pbstrInstFlags)
{
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x69, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbstrInstFlags);
}

void IXRawfile2::GetInstNumChannelLabels(long* pnInstNumChannelLabels)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x6a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnInstNumChannelLabels);
}

void IXRawfile2::GetInstChannelLabel(long nChannelLabelNumber, BSTR* pbstrInstChannelLabel)
{
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x6b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nChannelLabelNumber, pbstrInstChannelLabel);
}

void IXRawfile2::GetNumberOfControllersOfType(long nControllerType, long* pnNumControllersOfType)
{
	static BYTE parms[] =
		VTS_I4 VTS_PI4;
	InvokeHelper(0x6c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nControllerType, pnNumControllersOfType);
}

void IXRawfile2::GetAverageMassList(long* pnFirstAvgScanNumber, long* pnLastAvgScanNumber, long* pnFirstBkg1ScanNumber, long* pnLastBkg1ScanNumber, long* pnFirstBkg2ScanNumber, long* pnLastBkg2ScanNumber, LPCTSTR bstrFilter, long nIntensityCutoffType, 
		long nIntensityCutoffValue, long nMaxNumberOfPeaks, long bCentroidResult, double* pdCentroidPeakWidth, VARIANT* pvarMassList, VARIANT* pvarPeakFlags, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x6d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnFirstAvgScanNumber, pnLastAvgScanNumber, pnFirstBkg1ScanNumber, pnLastBkg1ScanNumber, pnFirstBkg2ScanNumber, pnLastBkg2ScanNumber, bstrFilter, nIntensityCutoffType, nIntensityCutoffValue, nMaxNumberOfPeaks, bCentroidResult, 
		pdCentroidPeakWidth, pvarMassList, pvarPeakFlags, pnArraySize);
}

void IXRawfile2::IsThereMSData(long* pbMSData)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x6e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbMSData);
}

void IXRawfile2::HasExpMethod(long* pbMethod)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x6f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pbMethod);
}

void IXRawfile2::GetFilterMassPrecision(long* pnFilterMassPrecision)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x70, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnFilterMassPrecision);
}

void IXRawfile2::GetStatusLogForPos(long nPos, VARIANT* pvarRT, VARIANT* pvarValue, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x71, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nPos, pvarRT, pvarValue, pnArraySize);
}

void IXRawfile2::GetStatusLogPlottableIndex(VARIANT* pvarIndex, VARIANT* pvarValues, long* pnArraySize)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_PVARIANT VTS_PI4;
	InvokeHelper(0x72, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pvarIndex, pvarValues, pnArraySize);
}

void IXRawfile2::GetInstMethodNames(long* pnNumInstMethods, VARIANT* pvarNames)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PVARIANT;
	InvokeHelper(0x73, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pnNumInstMethods, pvarNames);
}

void IXRawfile2::SetMassTolerance(long bUseUserDefined, double dMassTolerance, long nUnits)
{
	static BYTE parms[] =
		VTS_I4 VTS_R8 VTS_I4;
	InvokeHelper(0x74, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bUseUserDefined, dMassTolerance, nUnits);
}

void IXRawfile2::GetChros(long nNumChros, double* pdStartTime, double* pdEndTime, VARIANT* pvarChroParamsArray, VARIANT* pvarChroDataSizeArray, VARIANT* pvarChroDataArray, VARIANT* pvarPeakFlagsArray)
{
	static BYTE parms[] =
		VTS_I4 VTS_PR8 VTS_PR8 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nNumChros, pdStartTime, pdEndTime, pvarChroParamsArray, pvarChroDataSizeArray, pvarChroDataArray, pvarPeakFlagsArray);
}

void IXRawfile2::GetLabelData(VARIANT* pvarLabels, VARIANT* pvarFlags, long* pnScanNumber)
{
	static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT VTS_I4;
	InvokeHelper(0x76, DISPATCH_METHOD, VT_EMPTY, NULL, parms, pvarLabels, pvarFlags, *pnScanNumber);
}