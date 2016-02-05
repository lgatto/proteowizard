// -*- mode: c++ -*-


/*
    File: AnalystImpl.cpp
    Description: shared implementation across Analyst & AnalystQS interfaces
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
#include "AnalystImpl.h"

using namespace std;

int getLibraryVersion (enum LibraryTagType whichLibrary, std::string &strVersion) {
	strVersion.resize(0);

	std::string strKeyName("SOFTWARE\\PE Sciex\\Products\\");

	if (LIBRARY_Analyst==whichLibrary) {
		strKeyName.append ("Analyst3Q");
	} else if (LIBRARY_AnalystQS==whichLibrary) {
		strKeyName.append ("AnalystQS");
	} else {
		return -2;
	}

	USES_CONVERSION;

	HKEY hKeyProducts;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			strKeyName.c_str(),
			0, KEY_READ, &hKeyProducts) == ERROR_SUCCESS) {

		TCHAR szVersion[256] = {0};
		DWORD dwBufferSize = 256;
		if (RegQueryValueEx(hKeyProducts,TEXT("Version"), NULL, NULL,
			(LPBYTE)&szVersion, &dwBufferSize) == ERROR_SUCCESS) {
			USES_CONVERSION;
			strVersion = T2A(szVersion);
		}

		RegCloseKey(hKeyProducts);

		return 0;

	} else {
		return -1;
	}
}

const char *getLibraryName (enum LibraryTagType whichLibrary) {
	switch (whichLibrary) {
		case LIBRARY_Analyst:
			return "Analyst";
		case LIBRARY_AnalystQS:
			return "Analyst QS";
		default:
			return "Unknown";
	}
}

bool isCompatible (enum LibraryTagType whichLibrary, std::string &strSignature) {
	switch (whichLibrary) {
		case LIBRARY_Analyst:
			return ((strSignature.find(getLibraryName(LIBRARY_Analyst)) != strSignature.npos)
				&& (strSignature.find(getLibraryName(LIBRARY_AnalystQS)) == strSignature.npos));
		case LIBRARY_AnalystQS:
			return (strSignature.find(getLibraryName(LIBRARY_AnalystQS)) != strSignature.npos);
		default:
			return false;
	}
}

void getSoftwareVersion (const std::string &strSignature, std::string &strSoftware, std::string &strVersion) {
	std::string::size_type nPos = strSignature.find_last_of(" ");
	if (nPos == std::string::npos) {
		// no space found, assume it is just software name, no version
		strSoftware = strSignature;
		strVersion.resize(0);
	} else {
		// the RHS of space might be the version
		// assume that if we have digits, period mostly, it is a version
		std::string::size_type lVersionChars = 0;
		std::string::size_type nLen = strSignature.length();
		for (std::string::size_type i=nPos+1; i<nLen; i++) {
			if (strSignature[i] == '.' || (strSignature[i]>= '0' && strSignature[i]<='9')) {
				lVersionChars++;
			}
		}
		nLen -= (nPos+1);
		nLen -= lVersionChars;

		if (lVersionChars > nLen) {
			strSoftware = strSignature.substr(0, nPos);
			strVersion = strSignature.substr(nPos+1);
		} else {
			strSoftware = strSignature;
			strVersion.resize(0);
		}
	}
}

MSAcquisitionSoftwareType softwareToMSAcquisitionSoftwareType(std::string &strSoftware) {
	if (strSoftware=="Analyst QS") {
		return ANALYSTQS;
	} else if (strSoftware=="Analyst") {
		return ANALYST;
	} else {
		return ACQUISITIONSOFTWARE_UNDEF;
	}
}

int getModelById (long lId, std::string &strModel) {
	strModel.resize(0);

	char    buffer[11+1]; //-2147483648 to 2147483647
	_ltoa(lId, buffer, 10);
	std::string strKeyName("SOFTWARE\\PE Sciex\\Analyst\\DAME\\DeviceModelMap\\0\\");
	strKeyName.append (buffer);

	USES_CONVERSION;

	HKEY hKeyAnalyst;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		strKeyName.c_str(),
		0, KEY_READ, &hKeyAnalyst) == ERROR_SUCCESS) {

		int nRet = -1;
		TCHAR szVersion[1024] = {0};
		DWORD dwBufferSize = 1024;
		if (RegQueryValueEx(hKeyAnalyst,TEXT("UserFriendlyName"), NULL, NULL,
			(LPBYTE)&szVersion, &dwBufferSize) == ERROR_SUCCESS) {
			strModel = T2A(szVersion);

			// new system: "QStar XL"
			// old system: "MassSpecMethod QStar XL"
			// so, we remove it from older system for consistency
			const std::string prefix("MassSpecMethod ");
			if (0 == strModel.find(prefix)) {
				strModel.erase (0, prefix.length() );
			}

			nRet = 0;

		} else {

			strModel = std::string("Model Id: ").append(buffer);

			cerr << "WARNING: Device Model Map does not contain \"UserFriendlyName\", using model id rather than model description." << endl;
			cerr << "WARNING: Please check that you have installed your Analyst library correctly." << endl;

		}

		RegCloseKey(hKeyAnalyst);

		return nRet;

	} else {

			strModel = std::string("Model Id: ").append(buffer);

		cerr << "WARNING: Device Model Map not found, using model id rather than model description." << endl;
		cerr << "WARNING: Please check that you have installed your Analyst library correctly." << endl;

		return -1;
	}
}

MSInstrumentModelType modelIdToMSInstrumentModel(long lId) {
	switch (lId) {
		case 1:
			return API_100;
		case 2:
			return API_100_LC;
		case 3:
			return API_150_MCA;
		case 4:
			return API_150_EX;
		case 5:
			return API_165;
		case 6:
			return API_300;
		case 7:
			return API_350;
		case 8:
			return API_365;
		case 9:
			return API_2000;
		case 10:
			return API_3000;
		case 11:
			return API_4000;
		case 12:
			return GENERIC_SINGLE_QUAD;
		case 13:
			return QTRAP;
		case 14:
			return _4000_Q_TRAP;
		case 15:
			return API_3200;
		case 16:
			return _3200_Q_TRAP;
		case 17:
			return API_5000;
		case 1001:
			return ELAN_6000;
		case 2001:
			return QSTAR;
		case 2002:
			return API_QSTAR_PULSAR;
		case 2003:
			return API_QSTAR_PULSAR_I;
		case 2004:
			return QSTAR_XL_SYSTEM;
		case 10000:
			return AGILENT_TOF;
		default:
			return INSTRUMENTMODEL_UNDEF;
	}
}

void getSampleLogEntry(const std::string &strSampleLog, const std::string &strKey, std::string &strValue) {
	strValue.resize(0);

	if (strKey.empty () ) {
		return;
	}

	std::string::size_type nStartPos = strSampleLog.find(strKey);
	if (strSampleLog.npos == nStartPos) {
		return;
	}

	nStartPos += strKey.length() ;

	std::string::size_type nEndPos = strSampleLog.find(", ", nStartPos);
	if (strSampleLog.npos == nEndPos) {
		strValue = strSampleLog.substr (nStartPos);
	} else {
		nEndPos -= nStartPos;
		strValue = strSampleLog.substr (nStartPos, nEndPos);
	}
}

static const int scanToMSLevel[] = {
	1, 
	1, 2, 2, 2, 1, 
	2, 1, 1, 2, 1, 
	2, 1, 3, 1, 1, 
	1, 1};
static const int scanToMSLevelSize = sizeof(scanToMSLevel)/sizeof(int);
static const int scanToMSLevelLastIndex = scanToMSLevelSize-1;

static const char *scanToString[] = {
	"Q1 Scan",
	"Q1 MI", "Q3 Scan", "Q3 MI", "MRM", "Precursor Scan",
	"Product Ion Scan", "Neutral Loss Scan", "TOF MS1", "TOF MS2", "TOF Precursor Ion Scan",
	"EPI", "ER", "MS3", "TDF", "EMS",
	"EMC", "Unknown"};
static const int scanToStringSize = sizeof(scanToString)/sizeof(int);
static const int scanToStringLastIndex = scanToStringSize-1;

static const MSScanType scanToMSScanType[] = {
	Q1Scan,
	Q1MI, Q3Scan, Q3MI, MRM, PrecursorScan,
	ProductIonScan, NeutralLossScan, TOFMS1, TOFMS2, TOFPrecursorIonScan,
	EPI, ER, MS3, TDF, EMS,
	EMC, SCAN_UNDEF
};
static const int scanToMSScanTypeSize = sizeof(scanToMSScanType)/sizeof(int);
static const int scanToMSScanTypeLastIndex = scanToMSScanTypeSize-1;

long getMSLevel(long lScanType)
{
	return ((lScanType<scanToMSLevelSize) ? 
		scanToMSLevel[lScanType] : scanToMSLevel[scanToMSLevelLastIndex]);
}

const char *getPolarityString(long lPolarity)
{
	return (0==lPolarity?"+":"-");
}

const char *getScanTypeString(long lScanType)
{
	return ((lScanType<scanToStringSize) ? 
	scanToString[lScanType] : scanToString[scanToStringLastIndex]);
}

MSScanType getMSScanType(long lScanType)
{
	return ((lScanType<scanToMSScanTypeSize) ? 
		scanToMSScanType[lScanType] : scanToMSScanType[scanToMSScanTypeLastIndex]);
}
