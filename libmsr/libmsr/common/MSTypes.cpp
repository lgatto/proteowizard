// -*- mode: c++ -*-


/*
	File: MSTypes.cpp
	Description: Shared enum types for instrument and scan information
	Date: July 30, 2007

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

#include "MSTypes.h"

using namespace std;

string toString(MSManufacturerType manufacturer) {
	string str;
	switch (manufacturer) {
		case THERMO:
			str = "Thermo";
			break;
		case THERMO_SCIENTIFIC:
			str = "Thermo Scientific";
			break;
		case THERMO_FINNIGAN:
			str = "Thermo Finnigan";
			break;
		case WATERS:
			str = "Waters";
			break;
		case ABI_SCIEX:
			str = "ABI / SCIEX";
			break;
		case AGILENT:
			str = "Agilent";
			break;
		case MANUFACTURER_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSManufacturerType manufacturer, const string& cvLabel) {
	string obo = cvLabel;
	switch (manufacturer) {
		case THERMO:
			obo += ":1000483";
			break;
		case THERMO_SCIENTIFIC:
			obo += ":1000494";
			break;
		case THERMO_FINNIGAN:
			obo += ":1000125";
			break;
		case WATERS:
			obo += ":1000126";
			break;
		case ABI_SCIEX:
			obo += ":1000121";
			break;
		case AGILENT:
			obo += ":1000490";
			break;
		case MANUFACTURER_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

MSManufacturerType MSManufacturerTypeFromString(const string &manufacturer) {
	if (manufacturer == "Thermo")				return THERMO;
	if (manufacturer == "Thermo Scientific")	return THERMO_SCIENTIFIC;
	if (manufacturer == "Thermo Finnigan")		return THERMO_FINNIGAN;
	if (manufacturer == "Waters")				return WATERS;
	if (manufacturer == "ABI / SCIEX")			return ABI_SCIEX;
	if (manufacturer == "Agilent")				return AGILENT;
	return MANUFACTURER_UNDEF;
}

string toString(MSInstrumentModelType instrumentModel) {
	string str;
	switch (instrumentModel) {
		case LTQ:
			str = "LTQ";
			break;
		case LTQ_FT:
			str = "LTQ FT";
			break;
		case LTQ_FT_ULTRA:
			str = "LTQ FT Ultra";
			break;
		case LTQ_ORBITRAP:
			str = "LTQ Orbitrap";
			break;
		case LTQ_ORBITRAP_DISCOVERY:
			str = "LTQ Orbitrap Discovery";
			break;
		case LTQ_ORBITRAP_XL:
			str = "LTQ Orbitrap XL";
			break;
		case LXQ:
			str = "LXQ";
			break;
		case LCQ_ADVANTAGE:
			str = "LCQ Advantage";
			break;	
		case LCQ_CLASSIC:
			str = "LCQ Classic";
			break;
		case LCQ_DECA:
			str = "LCQ Deca";
			break;
		case LCQ_DECA_XP_PLUS:
			str = "LCQ Deca XP Plus";
			break;
		case LCQ_FLEET:
			str = "LCQ Fleet";
			break;
		case TSQ_QUANTUM:
			str = "TSQ Quantum";
			break;
		case Q_TOF_MICRO:
			str = "q-tof micro";
			break;
		case Q_TOF_ULTIMA:
			str = "Q-Tof Ultima";
			break;

		case API_100:
			str = "API 100";
			break;
		case API_100_LC:
			str = "API 100";
			break;
		case API_150_MCA:
			str = "API 150 MCA";
			break;
		case API_150_EX:
			str = "API 150 EX";
			break;
		case API_165:
			str = "API 165";
			break;
		case API_300:
			str = "API 300";
			break;
		case API_350:
			str = "API 350";
			break;
		case API_365:
			str = "API 365";
			break;
		case API_2000:
			str = "API 2000";
			break;
		case API_3000:
			str = "API 3000";
			break;
		case API_4000:
			str = "API 4000";
			break;
		case GENERIC_SINGLE_QUAD:
			str = "Generic Single Quad";
			break;
		case QTRAP:
			str = "QTrap";
			break;
		case _4000_Q_TRAP:
			str = "4000 Q Trap";
			break;
		case API_3200:
			str = "API 3200";
			break;
		case _3200_Q_TRAP:
			str = "3200 Q Trap";
			break;
		case API_5000:
			str = "API 5000";
			break;
		case ELAN_6000:
			str = "ELAN 6000";
			break;
		case AGILENT_TOF:
			str = "AGILENT TOF";
			break;
		case QSTAR:
			str = "QStar";
			break;
		case API_QSTAR_PULSAR:
			str = "API QStar Pulsar";
			break;
		case API_QSTAR_PULSAR_I:
			str = "API QStar Pulsar i";
			break;
		case QSTAR_XL_SYSTEM:
			str = "QSTAR XL System";
			break;

		case INSTRUMENTMODEL_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSInstrumentModelType instrumentModel, const string& cvLabel) {
	string obo = cvLabel;
	switch (instrumentModel) {
		case LTQ:
			obo += ":1000447";
			break;
		case LTQ_FT:
			obo += ":1000448";
			break;
		case LTQ_FT_ULTRA:
			obo += ":1000557";
			break;
		case LTQ_ORBITRAP:
			obo += ":1000449";
			break;
		case LTQ_ORBITRAP_DISCOVERY:
			obo += ":1000555";
			break;
		case LTQ_ORBITRAP_XL:
			obo += ":1000556";
			break;
		case LXQ:
			obo += ":1000450";
			break;
		case LCQ_ADVANTAGE:
			obo += ":1000167";
			break;	
		case LCQ_CLASSIC:
			obo += ":1000168";
			break;
		case LCQ_DECA:
			obo += ":1000554";
			break;
		case LCQ_DECA_XP_PLUS:
			obo += ":1000169";
			break;
		case LCQ_FLEET:
			obo += ":1000578";
			break;
		case TSQ_QUANTUM:
			obo += ":1000199";
			break;
		case Q_TOF_MICRO:
			obo += ":1000188";
			break;
		case Q_TOF_ULTIMA:
			obo += ":1000189";
			break;

		case API_100:
			obo += ":9999999"; // OBO: add
			break;
		case API_100_LC:
			obo += ":9999999"; // OBO: add
			break;
		case API_150_MCA:
			obo += ":9999999"; // OBO: add
			break;
		case API_150_EX:
			obo += ":9999999"; // OBO: add
			break;
		case API_165:
			obo += ":9999999"; // OBO: add
			break;
		case API_300:
			obo += ":9999999"; // OBO: add
			break;
		case API_350:
			obo += ":9999999"; // OBO: add
			break;
		case API_365:
			obo += ":9999999"; // OBO: add
			break;
		case API_2000:
			obo += ":9999999"; // OBO: add
			break;
		case API_3000:
			obo += ":9999999"; // OBO: add
			break;
		case API_4000:
			obo += ":9999999"; // OBO: add
			break;
		case GENERIC_SINGLE_QUAD:
			obo += ":9999999"; // OBO: add
			break;
		case QTRAP:
			obo += ":9999999"; // OBO: add
			break;
		case _4000_Q_TRAP:
			obo += ":9999999"; // OBO: add
			break;
		case API_3200:
			obo += ":9999999"; // OBO: add
			break;
		case _3200_Q_TRAP:
			obo += ":9999999"; // OBO: add
			break;
		case API_5000:
			obo += ":9999999"; // OBO: add
			break;
		case ELAN_6000:
			obo += ":9999999"; // OBO: add
			break;
		case AGILENT_TOF:
			obo += ":9999999"; // OBO: add
			break;
		case QSTAR:
			obo += ":9999999"; // OBO: add
			break;
		case API_QSTAR_PULSAR:
			obo += ":9999999"; // OBO: add
			break;
		case API_QSTAR_PULSAR_I:
			obo += ":9999999"; // OBO: add
			break;
		case QSTAR_XL_SYSTEM:
			obo += ":9999999"; // OBO: add
			break;

		case INSTRUMENTMODEL_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

MSInstrumentModelType MSInstrumentModelTypeFromString(const string &instrumentModel)
{
		if (instrumentModel == "LTQ")						return LTQ;
		if (instrumentModel == "LTQ FT")					return LTQ_FT;
		if (instrumentModel == "LTQ FT Ultra")				return LTQ_FT_ULTRA;
		if (instrumentModel == "LTQ Orbitrap")				return LTQ_ORBITRAP;
		if (instrumentModel == "LTQ Orbitrap Discovery")	return LTQ_ORBITRAP_DISCOVERY;
		if (instrumentModel == "LTQ Orbitrap XL")			return LTQ_ORBITRAP_XL;
		if (instrumentModel == "LXQ")						return LXQ;
		if (instrumentModel == "LCQ Advantage")				return LCQ_ADVANTAGE;	
		if (instrumentModel == "LCQ Classic")				return LCQ_CLASSIC;
		if (instrumentModel == "LCQ Deca")					return LCQ_DECA;
		if (instrumentModel == "LCQ Deca XP Plus")			return LCQ_DECA_XP_PLUS;
		if (instrumentModel == "LCQ Fleet")					return LCQ_FLEET;
		if (instrumentModel == "TSQ Quantum")				return TSQ_QUANTUM;
		if (instrumentModel == "q-tof micro")				return Q_TOF_MICRO;
		if (instrumentModel == "Q-Tof Ultima")				return Q_TOF_ULTIMA;
		if (instrumentModel == "API 100")					return API_100;
		if (instrumentModel == "API 100")					return API_100_LC;
		if (instrumentModel == "API 150 MCA")				return API_150_MCA;
		if (instrumentModel == "API 150 EX")				return API_150_EX;
		if (instrumentModel == "API 165")					return API_165;
		if (instrumentModel == "API 300")					return API_300;
		if (instrumentModel == "API 350")					return API_350;
		if (instrumentModel == "API 365")					return API_365;
		if (instrumentModel == "API 2000")					return API_2000;
		if (instrumentModel == "API 3000")					return API_3000;
		if (instrumentModel == "API 4000")					return API_4000;
		if (instrumentModel == "Generic Single Quad")		return GENERIC_SINGLE_QUAD;
		if (instrumentModel == "QTrap")						return QTRAP;
		if (instrumentModel == "4000 Q Trap")				return _4000_Q_TRAP;
		if (instrumentModel == "API 3200")					return API_3200;
		if (instrumentModel == "3200 Q Trap")				return _3200_Q_TRAP;
		if (instrumentModel == "API 5000")					return API_5000;
		if (instrumentModel == "ELAN 6000")					return ELAN_6000;
		if (instrumentModel == "AGILENT TOF")				return AGILENT_TOF;
		if (instrumentModel == "QStar")						return QSTAR;
		if (instrumentModel == "API QStar Pulsar")			return API_QSTAR_PULSAR;
		if (instrumentModel == "API QStar Pulsar i")		return API_QSTAR_PULSAR_I;
		if (instrumentModel == "QSTAR XL System")			return QSTAR_XL_SYSTEM;
		return INSTRUMENTMODEL_UNDEF;
}


string toString(MSAcquisitionSoftwareType acquisitionSoftware) {
	string str;
	switch (acquisitionSoftware) {
		case XCALIBUR:
			str = "Xcalibur";
			break;
		case MASSLYNX:
			str = "MassLynx";
			break;
		case ANALYST:
			str = "Analyst";
			break;
		case ANALYSTQS:
			str = "AnalystQS";
			break;
		case MASSHUNTER:
			str = "MassHunter";
			break;
		case ACQUISITIONSOFTWARE_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}


string toOBO(MSAcquisitionSoftwareType acquisitionSoftware, const string& cvLabel) {
	string obo = cvLabel;
	switch (acquisitionSoftware) {
		case XCALIBUR:
			obo += ":1000532";
			break;
		case MASSLYNX:
			obo += ":1000534";
			break;
		case ANALYST:
			obo += ":1000551";
			break;
		case ANALYSTQS:
			obo += ":9999999"; // OBO: add
			break;
		case MASSHUNTER:
			obo += ":9999999"; // OBO: add
			break;
		case ACQUISITIONSOFTWARE_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

MSAcquisitionSoftwareType MSAcquisitionSoftwareTypeFromString(const string &acquisitionSoftware)
{
	if (acquisitionSoftware == "Xcalibur")		return XCALIBUR;
	if (acquisitionSoftware == "MassLynx")		return MASSLYNX;
	if (acquisitionSoftware == "Analyst")		return ANALYST;
	if (acquisitionSoftware == "AnalystQS")		return ANALYSTQS;
	if (acquisitionSoftware == "MassHunter")	return MASSHUNTER;
	return ACQUISITIONSOFTWARE_UNDEF;
}



string toString(MSAnalyzerType analyzer) {
	string str;
	switch (analyzer) {
		case ITMS:
			str = "ITMS";
			break;
		case TQMS:
			str = "TQMS";
			break;
		case SQMS:
			str = "SQMS";
			break;
		case TOFMS:
			str = "TOFMS";
			break;
		case FTMS:
			str = "FTMS";
			break;
		case SECTOR:
			str = "SECTOR";
			break;
		case QTOF:
			str = "QTOF";
			break;
		case ANALYZER_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBOText(MSAnalyzerType analyzer) {
	string str;
	switch (analyzer) {
		case ITMS:
			str = "ion trap";
			break;
		case TQMS:
			str = "TQMS";
			break;
		case SQMS:
			str = "SQMS";
			break;
		case TOFMS:
			str = "time-of-flight";
			break;
		case FTMS:
			str = "fourier transform ion cyclotron resonance mass spectrometer";
			break;
		case SECTOR:
			str = "magnetic sector";
			break;
		case QTOF:
			str = "QTOF";
			break;
		case ANALYZER_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSAnalyzerType analyzer, const string& cvLabel) {
	string obo = cvLabel;
	switch (analyzer) {
		case ITMS:
			obo += ":1000264";
			break;
		case TQMS:
			obo += ":9999999"; // obo fix
			break;
		case SQMS:
			obo += ":9999999"; // obo fix
			break;
		case TOFMS:
			obo += ":1000084";
			break;
		case FTMS:
			obo += ":1000079";
			break;
		case SECTOR:
			obo += ":1000080";
			break;
		case QTOF:
			obo += ":9999999"; // obo fix
			break;
		case ANALYZER_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

MSAnalyzerType MSAnalyzerTypeFromOBOText(const string &analyzer) {
	if (analyzer == "ion trap")														return ITMS;
	if (analyzer == "TQMS")															return TQMS;
	if (analyzer == "SQMS")															return SQMS;
	if (analyzer == "time-of-flight")												return TOFMS;
	if (analyzer == "fourier transform ion cyclotron resonance mass spectrometer")	return FTMS;
	if (analyzer == "magnetic sector")												return SECTOR;
	if (analyzer == "QTOF")															return QTOF;

	return ANALYZER_UNDEF;
}

MSAnalyzerType MSAnalyzerTypeFromString(const string &analyzer) {

	if (analyzer == "ITMS")		return ITMS;
	if (analyzer == "TQMS")		return TQMS;
	if (analyzer == "SQMS")		return SQMS;
	if (analyzer == "TOFMS")	return TOFMS;
	if (analyzer == "FTMS")		return FTMS;
	if (analyzer == "SECTOR")	return SECTOR;
	if (analyzer == "QTOF")		return QTOF;

	return ANALYZER_UNDEF;
}


string toString(MSDetectorType detector) {
	string str;
	switch (detector) {
		case DETECTOR_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSDetectorType detector, const string& cvLabel) {
	string obo;
	switch (detector) {
		case DETECTOR_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

string toOBOText(MSDetectorType detector) {
	return "unknown";
}

MSDetectorType MSDetectorTypeFromOBOText(const string &detector) {
	return DETECTOR_UNDEF;
}

MSDetectorType MSDetectorTypeFromString(const string &detector) {
	return DETECTOR_UNDEF;
}

MSPolarityType MSPolarityTypeFromOBOText(const string &polarity) {
	if (polarity == "positive")	return POSITIVE;
	if (polarity == "negative")	return NEGATIVE;
	if (polarity == "any")		return ANY;
	return POLARITY_UNDEF;
}

MSPolarityType MSPolarityTypeFromString(const string &polarity) {
	if (polarity == "+")	return POSITIVE;
	if (polarity == "-")	return NEGATIVE;
	if (polarity == "any")	return ANY;
	return POLARITY_UNDEF;
}

string toString(MSIonizationType ionization) {
	string str;
	switch (ionization) {
		case EI:
			str = "EI";
			break;
		case CI:
			str = "CI";
			break;
		case FAB:
			str = "FAB";
			break;
		case ESI:
			str = "ESI";
			break;
		case APCI:
			str = "APCI";
			break;
		case NSI:
			str = "NSI";
			break;
		case TSP:
			str = "TSP";
		case FD:
			str = "FD";
			break;
		case MALDI:
			str = "MALDI";
			break;
		case GD:
			str = "GD";
			break;
		case MS_CHIP:
			str = "MS_CHIP";
			break;
		case IONIZATION_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSIonizationType ionization, const string& cvLabel) {
	string obo = cvLabel;
	switch (ionization) {
		case EI:
			obo += ":1000389";
			break;
		case CI:
			obo += ":1000386";
			break;
		case FAB:
			obo += ":1000074";
			break;
		case ESI:
			obo += ":1000073";
			break;
		case APCI:
			obo += ":1000070";
			break;
		case NSI:
			obo += ":1000398";
			break;
		case TSP:
			obo += ":9999999"; // obo fix
			break;
		case FD:
			obo += ":1000257";
			break;
		case MALDI:
			obo += ":1000075";
			break;
		case GD:
			obo += ":1000259";
			break;
		case MS_CHIP:
			obo += ":9999999"; // obo fix
			break;
		case IONIZATION_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

string toOBOText(MSIonizationType ionization) {
	string str;
	switch (ionization) {
		case EI:
			str = "electron ionization";
			break;
		case CI:
			str = "chemi-ionization";
			break;
		case FAB:
			str = "fast atom bombardment ionization";
			break;
		case ESI:
			str = "electrospray ionization";
			break;
		case APCI:
			str = "atmospheric pressure chemical ionization";
			break;
		case NSI:
			str = "nanoelectrospray";
			break;
		case TSP:
			str = "TSP";
		case FD:
			str = "field desorption";
			break;
		case MALDI:
			str = "matrix-assisted laser desorption ionization";
			break;
		case GD:
			str = "glow discharge ionization";
			break;
		case MS_CHIP:
			str = "MS_CHIP";
			break;
		case IONIZATION_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

MSIonizationType MSIonizationTypeFromOBOText(const string& ionization) {
	if (ionization == "electron ionization")							return EI;
	if (ionization == "chemi-ionization")								return CI;
	if (ionization == "fast atom bombardment ionization")				return FAB;
	if (ionization == "electrospray ionization")						return ESI;
	if (ionization == "atmospheric pressure chemical ionization")		return APCI;
	if (ionization == "nanoelectrospray")								return NSI;
	if (ionization == "TSP")											return TSP;
	if (ionization == "field desorption")								return FD;
	if (ionization == "matrix-assisted laser desorption ionization")	return MALDI;
	if (ionization == "glow discharge ionization")						return GD;
	if (ionization == "MS_CHIP")										return MS_CHIP;

	return IONIZATION_UNDEF;
}

MSIonizationType MSIonizationTypeFromString(const string &ionization) {
	if (ionization == "EI")			return EI;
	if (ionization == "CI")			return CI;
	if (ionization == "FAB")		return FAB;
	if (ionization == "ESI")		return ESI;
	if (ionization == "APCI")		return APCI;
	if (ionization == "NSI")		return NSI;
	if (ionization == "TSP")		return TSP;
	if (ionization == "FD")			return FD;
	if (ionization == "MALDI")		return MALDI;
	if (ionization == "GD")			return GD;
	if (ionization == "MS_CHIP")	return MS_CHIP;

	return IONIZATION_UNDEF;
}

string toString(MSScanType scanType) {
	string str;
	switch (scanType) {
		case FULL:
			str = "FULL";
			break;
		case SIM:
			str = "SIM";
			break;
		case SRM:
			str = "SRM";
			break;
		case CRM:
			str = "CRM";
			break;
		case Z:
			str = "Z";
			break;
		case Q1MS:
			str = "Q1MS";
			break;
		case Q3MS:
			str = "Q3MS";
			break;

		case Q1Scan:
			str = "Q1 Scan";
			break;
		case Q1MI:
			str = "Q1 MI";
			break;
		case Q3Scan:
			str = "Q3 Scan";
			break;
		case Q3MI:
			str = "Q3 MI";
			break;
		case MRM:
			str = "MRM";
			break;
		case PrecursorScan:
			str = "Precursor Scan";
			break;
		case ProductIonScan:
			str = "Product IonS can";
			break;
		case NeutralLossScan:
			str = "Neutral Loss Scan";
			break;
		case TOFMS1:
			str = "TOF MS1";
			break;
		case TOFMS2:
			str = "TOF MS2";
			break;
		case TOFPrecursorIonScan:
			str = "TOF Precursor Ion Scan";
			break;
		case EPI: // Enhanced Product Ion
			str = "EPI";
			break;
		case ER: // Enhanced Resolution
			str = "ER";
			break;
		case MS3:
			str = "MS3";
			break;
		case TDF: // Time Delayed Fragmentation
			str = "TDF";
			break;
		case EMS: // Enhanced MS
			str = "EMS";
			break;
		case EMC: // Enhanced Multi-Charge
			str = "EMC";
			break;

		case SCAN_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSScanType scanType, const string& cvLabel) {
	string obo = cvLabel;
	switch (scanType) {
		case FULL:
			obo += ":1000498";
			break;
		case SIM:
			obo += ":1000205";
			break;
		case SRM:
			obo += ":1000206";
			break;
		case CRM:
			obo += ":1000244";
			break;
		case Z:
			obo += ":1000497";
			break;
		case Q1MS:
			obo += ":9999999"; // obo fix
			break;
		case Q3MS:
			obo += ":9999999"; // obo fix
			break;

		case Q1Scan:
			obo += ":9999999"; // obo fix
			break;
		case Q1MI:
			obo += ":9999999"; // obo fix
			break;
		case Q3Scan:
			obo += ":9999999"; // obo fix
			break;
		case Q3MI:
			obo += ":9999999"; // obo fix
			break;
		case MRM:
			obo += ":9999999"; // obo fix
			break;
		case PrecursorScan:
			obo += ":9999999"; // obo fix
			break;
		case ProductIonScan:
			obo += ":9999999"; // obo fix
			break;
		case NeutralLossScan:
			obo += ":9999999"; // obo fix
			break;
		case TOFMS1:
			obo += ":9999999"; // obo fix
			break;
		case TOFMS2:
			obo += ":9999999"; // obo fix
			break;
		case TOFPrecursorIonScan:
			obo += ":9999999"; // obo fix
			break;
		case EPI:
			obo += ":9999999"; // obo fix
			break;
		case ER:
			obo += ":9999999"; // obo fix
			break;
		case MS3:
			obo += ":9999999"; // obo fix
			break;
		case TDF:
			obo += ":9999999"; // obo fix
			break;
		case EMS:
			obo += ":9999999"; // obo fix
			break;
		case EMC:
			obo += ":9999999"; // obo fix
			break;

		case SCAN_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

string toOBOText(MSScanType scanType) {
	string str;
	switch (scanType) {
		case FULL:
			str = "full scan";
			break;
		case SIM:
			str = "selected ion monitoring";
			break;
		case SRM:
			str = "selected reaction monitoring";
			break;
		case CRM:
			str = "consecutive reaction monitoring";
			break;
		case Z:
			str = "zoom scan";
			break;
		case Q1MS:
			str = "Q1MS";
			break;
		case Q3MS:
			str = "Q3MS";
			break;

		case Q1Scan:
			str = "Q1 Scan";
			break;
		case Q1MI:
			str = "Q1 MI";
			break;
		case Q3Scan:
			str = "Q3 Scan";
			break;
		case Q3MI:
			str = "Q3 MI";
			break;
		case MRM:
			str = "MRM";
			break;
		case PrecursorScan:
			str = "Precursor Scan";
			break;
		case ProductIonScan:
			str = "Product Ion Scan";
			break;
		case NeutralLossScan:
			str = "Neutral Loss Scan";
			break;
		case TOFMS1:
			str = "TOF MS1";
			break;
		case TOFMS2:
			str = "TOF MS2";
			break;
		case TOFPrecursorIonScan:
			str = "TOF Precursor Ion Scan";
			break;
		case EPI: // Enhanced Product Ion
			str = "EPI";
			break;
		case ER: // Enhanced Resolution
			str = "ER";
			break;
		case MS3:
			str = "MS3";
			break;
		case TDF: // Time Delayed Fragmentation
			str = "TDF";
			break;
		case EMS: // Enhanced MS
			str = "EMS";
			break;
		case EMC: // Enhanced Multi-Charge
			str = "EMC";
			break;

		case SCAN_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

MSScanType MSScanTypeFromString(const string &scanType) {
	if (scanType == "FULL")						return FULL;
	if (scanType == "SIM")						return SIM;
	if (scanType == "SRM")						return SRM;
	if (scanType == "CRM")						return CRM;
	if (scanType == "Z")						return Z;
	if (scanType == "Q1MS")						return Q1MS;
	if (scanType == "Q3MS")						return Q3MS;
	if (scanType == "Q1 Scan")					return Q1Scan;
	if (scanType == "Q1 MI")					return Q1MI;
	if (scanType == "Q3 Scan")					return Q3Scan;
	if (scanType == "Q3 MI")					return Q3MI;
	if (scanType == "MRM")						return MRM;
	if (scanType == "Precursor Scan")			return PrecursorScan;
	if (scanType == "Product Ion Scan")			return ProductIonScan;
	if (scanType == "Neutral Loss Scan")		return NeutralLossScan;
	if (scanType == "TOF MS1")					return TOFMS1;
	if (scanType == "TOF MS2")					return TOFMS2;
	if (scanType == "TOF Precursor Ion Scan")	return TOFPrecursorIonScan;
	if (scanType == "EPI")						return EPI;
	if (scanType == "ER")						return ER;
	if (scanType == "MS3")						return MS3;
	if (scanType == "TDF")						return TDF;
	if (scanType == "EMS")						return EMS;
	if (scanType == "EMC")						return EMC;
	return SCAN_UNDEF;
}


string toString(MSActivationType activation) {
	string str;
	switch (activation) {
		case CID:
			str = "CID";
			break;
		case MPD:
			str = "MPD";
			break;
		case ECD:
			str = "ECD";
			break;
		case PQD:
			str = "PQD";
			break;
		case ETD:
			str = "ETD";
			break;
		case HCD:
			str = "HCD";
			break;
		case SA:
			str = "SA";
			break;
		case PTR:
			str = "PTR";
			break;
		case ACTIVATION_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

string toOBO(MSActivationType activation, const string& cvLabel) {
	string obo;
	switch (activation) {
		case CID:
			obo += ":1000133";
			break;
		case MPD:
			obo += ":9999999"; // obo fix
			break;
		case ECD:
			obo += ":1000250";
			break;
		case PQD:
			obo += ":9999999"; // obo fix
			break;
		case ETD:
			obo += ":9999999"; // obo fix
			break;
		case HCD:
			obo += ":1000422";
			break;
		case SA:
			obo += ":9999999"; // obo fix
			break;
		case PTR:
			obo += ":9999999"; // obo fix
			break;
		case ACTIVATION_UNDEF:
		default:
			obo += ":9999999";
			break;
	}
	return obo;
}

string toOBOText(MSActivationType activation) {
	string str;
	switch (activation) {
		case CID:
			str = "collision-induced dissociation";
			break;
		case MPD:
			str = "MPD";
			break;
		case ECD:
			str = "electron capture dissociation";
			break;
		case PQD:
			str = "PQD";
			break;
		case ETD:
			str = "ETD";
			break;
		case HCD:
			str = "high-energy collision-induced dissociation";
			break;
		case SA:
			str = "SA";
			break;
		case PTR:
			str = "PTR";
			break;
		case ACTIVATION_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

MSActivationType MSActivationTypeFromOBOText(const string &activation) {
	if (activation == "collision-induced dissociation")				return CID;
	if (activation == "MPD")										return MPD;
	if (activation == "electron capture dissociation")				return ECD;
	if (activation == "PQD")										return PQD;
	if (activation == "ETD")										return ETD;
	if (activation == "high-energy collision-induced dissociation")	return HCD;
	if (activation == "SA")											return SA;
	if (activation == "PTR")										return PTR;

	return ACTIVATION_UNDEF;
}

MSActivationType MSActivationTypeFromString(const string &activation) {

	if (activation == "CID")	return CID;
	if (activation == "MPD")	return MPD;
	if (activation == "ECD")	return ECD;
	if (activation == "PQD")	return PQD;
	if (activation == "ETD")	return ETD;
	if (activation == "HCD")	return HCD;
	if (activation == "SA")		return SA;
	if (activation == "PTR")	return PTR;

	return ACTIVATION_UNDEF;
}


string toString(ScanCoordinateType scanCoordinateType) {
	string str;
	switch (scanCoordinateType) {
		case ABI_COORDINATE_SAMPLE:
			str = "sample";
			break;
		case ABI_COORDINATE_PERIOD:
			str = "period";
			break;
		case ABI_COORDINATE_EXPERIMENT:
			str = "experiment";
			break;
		case ABI_COORDINATE_CYCLE:
			str = "cycle";
			break;
		case SCAN_COORDINATE_UNDEF:
		default:
			str = "unknown";
			break;
	}
	return str;
}

/*bool CV::readFromURL(const string &url)
{
	exec_stream_t es("curl", string("-s ") + url);
	size_t oboLineCount = 0;
	while(!es.out().eof())
	{
		string oboLine;
		std::getline(es.out(), oboLine);
		++oboLineCount;
		CVTerm cvTerm;
		if(oboLine == "[Term]"
		{
			vector<string> oboTokens;

			std::getline(es.out(), oboLine);
			++oboLineCount;
			boost::algorithm::split(oboTokens, oboLine, boost::is_any_of(":"));
			if(oboTokens[0] != "id")
			{
				cerr << "Warning: cannot find id of CV term at line #: " << oboLineCount << endl;
				continue;
			}
			cvTerm.accession = CVAccession(boost::algorithm::trim_copy(oboTokens[1]+':'+oboTokens[2]));

			std::getline(es.out(), oboLine);
			++oboLineCount;
			boost::algorithm::split(oboTokens, oboLine, boost::is_any_of(":"));
			if(oboTokens[0] != "name")
			{
				cerr << "Warning: cannot find name of CV term at line #: " << oboLineCount << endl;
				continue;
			}
			cvTerm.name = boost::algorithm::trim_copy(oboTokens[1]);
			//cout << (string) cvTerm.accession << ": " << cvTerm.name << endl;
			insert(value_type(cvTerm.accession, cvTerm));
		}
	}
	es.close();
	if( es.exit_code() == 0 )
		return true;
	return false;
}*/
