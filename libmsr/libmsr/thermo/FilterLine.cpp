// -*- mode: c++ -*-


/*
    File: FilterLine.cpp
    Description: parsing for Thermo/Xcalibur "filter line".
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
#include "FilterLine.h"
#include "MSUtilities.h"

#include <sstream>
#include <stack>
#include <iostream>
#include <cctype> // for toupper
#include <algorithm>


using namespace std;


/*

FilterLine dictionary
--From Thermo


Analyzer:

ITMS		Ion Trap
TQMS		Triple Quad
SQMS		Single Quad
TOFMS		TOF
FTMS		ICR
Sector		Sector

Segment Scan Event   (Sectors only)

Polarity
-		Negative
+		Positive


Scan Data
c		centroid
p		profile


Ionization Mode
EI		Electron Impact
CI		Chemical Ionization
FAB		Fast Atom Bombardment
ESI		Electrospray
APCI		Atmospheric Pressure Chemical Ionization
NSI		Nanospray
TSP		Thermospray
FD		Field Desorption
MALDI	Matrix Assisted Laser Desorption Ionization
GD		Glow Discharge

Corona
corona			corona on
!corona		corona off

PhotoIoniziation
pi			photo ionization on
!pi			photo ionization off

Source CID
sid			source cid on
!sid			source cid off
sid=<x>     source cid on at <x> energy


Detector set
det			detector set
!det			detector not set


TurboScan
t			turbo scan on
!t			turob scan off

Enhanced			(Sectors only)
E			enhanced on
!E			enhanced off

Dependent Type
d			data dependent active
!d			data dependent not-active

Wideband
w			wideband activation on
!w			wideband activation off

Accurate Mass
!AM			accurate mass not active
AM			accurate mass active 
AMI			accurate mass with internal calibration
AME			accurate mass with external calibration

Ultra
u			ultra on
!u			ultra off

Scan Type:
full			full scan
SIM			single ion monitor
SRM			single reaction monitor
CRM
z			zoom scan
Q1MS			q1 mass spec scan
Q3MS			q3 mass spec scan 

Sector Scan			(Sectors only)
BSCAN		b scan
ESCAN		e scan



MSorder

MS2			MSn order
MS3
…
MS15

Activation Type
cid			collision induced dissociation
mpd
ecd			electron capture dissociation
pqd			pulsed q dissociation
etd			electron transfer dissociation
hcd			high energy collision dissociation
sa			supplemental cid
ptr			proton transfer reaction

Free Region			(Sectors only)
ffr1			field free region 1
ffr2			field free region 2

Mass range
[low mass – high mass]

*/

MSAnalyzerType 
FilterLine::parseAnalyzer(const string& word) {
	if (word == "ITMS") {
		return ITMS;
	}
	else if (word == "TQMS") {
		return TQMS;
	}
	else if (word == "SQMS") {
		return SQMS;
	}
	else if (word == "TOFMS") {
		return TOFMS;
	}
	else if (word == "FTMS") {
		return FTMS;
	}
	else if (word == "SECTOR") {
		return SECTOR;
	}
	else {
		return ANALYZER_UNDEF;
	}

}


MSPolarityType 
FilterLine::parsePolarity(const string& word) {
	if (word == "+") {
		return POSITIVE;
	}
	else if (word == "-") {
		return NEGATIVE;
	}
	// does this really exist?
	else if (word == "a") {
		return ANY;
	}
	else {
		return POLARITY_UNDEF;
	}
}


MSScanDataType 
FilterLine::parseScanData(const string& word) {
	if (word == "C") {
		return CENTROID;
	}
	else if (word == "P") {
		return PROFILE;
	}
	else {
		return SCANDATA_UNDEF;
	}
}




MSIonizationType 
FilterLine::parseIonizationMode(const string & word) {
	if (word == "EI" ){
		return EI;
	}
	else if (word == "CI") {
		return CI;
	}
	else if (word == "FAB") {
		return FAB;
	}
	else if (word == "ESI") {
		return ESI;
	}
	else if (word == "APCI") {
		return APCI;
	}
	else if (word == "NSI") {
		return NSI;
	}
	else if (word == "TSP") {
		return TSP;
	}
	else if (word == "FD") {
		return FD;
	}
	else if (word == "MALDI") {
		return MALDI;
	}
	else if (word == "GD") {
		return GD;
	}
	else {
		return IONIZATION_UNDEF;
	}
}



FilterLine::AccurateMassType 
FilterLine::parseAccurateMass(const string& word) {
	if (word == "!AM") {
		return NO_AM;
	}
	else if (word == "AM") {
		return AM;
	}
	else if (word == "AMI") {
		return AMI;
	}
	else if (word == "AME") {
		return AME;
	}
	else {
		return ACCURATEMASS_UNDEF;
	}
}



MSScanType 
FilterLine::parseScanType(const string& word) {
	if (word == "FULL") {
		return FULL;
	}
	else if (word == "SIM") {
		return SIM;
	}
	else if (word == "SRM") {
		return SRM;
	}
	else if (word == "CRM") {
		return CRM;
	}
	else if (word == "Z") {
		return Z;
	}
	else if (word == "Q1MS") {
		return Q1MS;
	}
	else if (word == "Q3MS") {
		return Q3MS;
	}
	else {
		return SCAN_UNDEF;
	}
}



MSActivationType 
FilterLine::parseActivation(const string& word) {
	if (word == "CID") {
		return CID;
	}
	else if (word == "MPD") {
		return MPD;
	}
	else if (word == "ECD") {
		return ECD;
	}
	else if (word == "PQD") {
		return PQD;
	}
	else if (word == "ETD") {
		return ETD;
	}
	else if (word == "HCD") {
		return HCD;
	}
	else if (word == "SA") {
		return SA;
	}
	else if (word == "PTR") {
		return PTR;
	}
	else {
		return ACTIVATION_UNDEF;
	}
}


FilterLine::FilterLine() : 
analyzer_(ANALYZER_UNDEF),
polarity_(POLARITY_UNDEF),
scanData_(SCANDATA_UNDEF),
ionizationMode_(IONIZATION_UNDEF),
coronaOn_(BOOL_UNDEF),
photoIonizationOn_(BOOL_UNDEF),
sourceCIDOn_(BOOL_UNDEF),
detectorSet_(BOOL_UNDEF),
turboScanOn_(BOOL_UNDEF),
dependentActive_(BOOL_UNDEF),
widebandOn_(BOOL_UNDEF),
accurateMass_(ACCURATEMASS_UNDEF),
scanType_(SCAN_UNDEF),
msLevel_(0),
activationMethod_(ACTIVATION_UNDEF)
{
	cidParentMass_.clear();
	cidEnergy_.clear();
	scanRangeMin_.clear();
	scanRangeMax_.clear();
};


FilterLine::~FilterLine() {
}


void 
FilterLine::print() {
	if (analyzer_) {
		cout << "analyzer " << analyzer_ << endl;
	}

	if (polarity_) {
		cout << "polarity " << polarity_ << endl;
	}

	if (scanData_) {
		cout << "scan data " << scanData_ << endl;
	}

	if (ionizationMode_) {
		cout << "ionization mode " << ionizationMode_ << endl;
	}

	if (coronaOn_ != BOOL_UNDEF) {
		cout << "corona: " << coronaOn_ << endl;
	}

	if (photoIonizationOn_ != BOOL_UNDEF) {
		cout << "photoionization: " << photoIonizationOn_ << endl;
	}

	if (sourceCIDOn_ != BOOL_UNDEF) {
		cout << "source CID: " << sourceCIDOn_ << endl;
	}

	if (detectorSet_ != BOOL_UNDEF) {
		cout << "detector set: " << detectorSet_ << endl;
	}

	if (turboScanOn_ != BOOL_UNDEF) {
		cout << "turboscan: " << turboScanOn_ << endl;
	}

	if (dependentActive_ != BOOL_UNDEF) {
		cout << "data dependent: " << dependentActive_ << endl;
	}

	if (widebandOn_ != BOOL_UNDEF) {
		cout << "wideband: " << widebandOn_ << endl;
	}

	if (accurateMass_) {
		cout << "accurate mass: " << accurateMass_ << endl;
	}

	if (scanType_) {
		cout << "scan type: " << scanType_ << endl;
	}

	if (msLevel_ > 0 ) {
		cout << "MS level: " << msLevel_ << endl;
	}

	if (activationMethod_) {
		// cout << "activation type: " << activationMethod_ << endl;
	}


	cout << endl << endl << endl;

}


bool 
FilterLine::parse(string filterLine) {
	/**
	almost all of the fields are optional
	*/
	boost::to_upper(filterLine);
	stringstream s(filterLine);
	string w;


	if (s.eof()) {
		return 1; // ok, empty line
	}
	s >> w;

	analyzer_ = parseAnalyzer(w);
	if (analyzer_) {
		// "analyzer" field was present
		if (s.eof()) {
			return 1;
		}
		s >> w;
	}

	polarity_ = parsePolarity(w);
	if (polarity_) {
		// "polarity" field was present
		if (s.eof()) {
			return 1;
		}
		s >> w;
	}


	scanData_ = parseScanData(w);
	if (scanData_) {
		// "scan data type" field present
		if (s.eof()) {
			return 1;
		}
		s >> w;
	}


	ionizationMode_ = parseIonizationMode(w);
	if (ionizationMode_) {
		// "ionization mode" field present
		if (s.eof()) {
			return 1;
		}
		s >> w;
	}



	bool advance = false;



	// corona
	if (w == "!CORONA") {
		coronaOn_ = BOOL_FALSE;
		advance = true;
	}
	else if (w == "CORONA") {
		coronaOn_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}

	// photoIonization
	if (w == "!PI") {
		photoIonizationOn_ = BOOL_FALSE;
		advance = true;
	}
	else if (w == "PI") {
		photoIonizationOn_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}

	// source CID
	if (w == "!SID") {
		sourceCIDOn_ = BOOL_FALSE;
		advance = true;
	}
	else if (w.find("SID") == 0) { // handle cases where SID energy is explicit
		sourceCIDOn_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}


	// detector
	if (w == "!DET") {
		detectorSet_ = BOOL_FALSE;
		advance = true;
	}
	else if (w == "DET") {
		detectorSet_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}


	// turboscan
	if (w == "!T") {
		turboScanOn_ = BOOL_FALSE;
		advance = true;
	}
	else if (w == "T") {
		turboScanOn_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}


	// dependent type
	if (w == "!D") {
		dependentActive_ = BOOL_FALSE;
		advance = true;
	}
	else if (w == "D") {
		dependentActive_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}


	// wideband
	if (w == "!W") {
		widebandOn_ = BOOL_FALSE;
		advance = true;
	}
	else if (w == "W") {
		widebandOn_ = BOOL_TRUE;
		advance = true;
	}
	if (advance) {
		if (s.eof()) {
			return 1;
		}
		s >> w;
		advance = false;
	}


	accurateMass_ = parseAccurateMass(w);
	if (accurateMass_) {
		// "accurate mass" field present
		if (s.eof()) {
			return 1;
		}
		s >> w;
	}

	scanType_ = parseScanType(w);
	if (scanType_) {
		if (scanType_ == Q1MS || scanType_ == Q3MS)
			msLevel_ = 1;

		// "scan type" field present
		if (s.eof()) {
			return 1;
		}
		s >> w;
	}

	// MS order
	if ( (w.substr(0,2) == "MS") && (w.length() >= 2) ) {
		if (w.length() == 2) {
			msLevel_ = 1; // just "MS"
		} else {
			// MSn: extract int n
			//cout << "len: " << w.length() << endl;
			msLevel_ = toInt(w.substr(2)); // take number after "ms"
		}
		if (s.eof()) {
			return 1;
		}
		s >> w;      
	}


	// CID info
	// TODO: MSn for n > 2: comma-separated
	// if msLevel >=2 there should be mass@energy pairs for each level >= 2
	if (msLevel_ > 1) {
		int expectedPairs = msLevel_ - 1;
		for (int i=0; i< expectedPairs; ++i) {
			char c=w[0];
			size_t markerPos = w.find('@',0);
			// make sure this word starts with a numeric char, and the word contains "@"
			if( markerPos == string::npos || ! ( (c >= '0') && (c <= '9') ) )
				return false;
			size_t energyPos = markerPos+1;
			c = w[energyPos];
			if ( ! ( (c >= '0') && (c <= '9') ) ) {
				energyPos = w.find_first_of("1234567890-+", energyPos); // find first numeric character after the "@"
				if (energyPos != string::npos) {
					activationMethod_ = parseActivation(w.substr(markerPos+1, energyPos-markerPos-1));
					if (activationMethod_ == 0)
						return false;
				} else
					return false;
			}

			string mass = w.substr(0, markerPos);
			string energy = w.substr(energyPos);
			// cout << "got mass " << mass << " at " << energy << " energy using activation " << (int) activationMethod_ << " (from " << w << ")" << endl;
			cidParentMass_.push_back(lexical_cast<double>(mass));
			cidEnergy_.push_back(lexical_cast<double>(energy));			

			// prematurely done?
			if (s.eof()) {
				return false;
			}
			s >> w;

		}
	}

	// try to get activation type if not already set
	if (activationMethod_ == 0) {
		activationMethod_ = parseActivation(w);
		if (activationMethod_) {
			// "activation type" field present
			if (s.eof()) {
				return 1;
			}
			s >> w;
		}
	}


	// product masses or mass ranges
	// TODO: parse single values, for SIM, SRM, CRM
	// some test based on ms level?

	string w2;
	std::getline(s, w2); // get all tokens until closing bracket
	w.append(w2);
	boost::trim_if(w, boost::is_any_of("[ ]")); // trim flanking brackets and whitespace
	vector<string> massRangeStrs;
	boost::split(massRangeStrs, w, boost::is_any_of(","));
	for(size_t i=0; i < massRangeStrs.size(); ++i)
	{
		string& massRangeStr = massRangeStrs[i]; // "<rangeMin>-<rangeMax>"
		boost::trim(massRangeStr); // trim flanking whitespace
		vector<string> rangeMinMaxStrs;
		boost::split(rangeMinMaxStrs, massRangeStr, boost::is_any_of("-"));
		scanRangeMin_.push_back(lexical_cast<double>(rangeMinMaxStrs[0]));
		scanRangeMax_.push_back(lexical_cast<double>(rangeMinMaxStrs[1]));
	}

	if (s.eof()) {
		// cout << "done parsing" << endl;
		return true;
	}
	else {
		do {
			cout << "unparsed filter line element: " << w << endl;
		} while (s >> w);
		return false;
	}
	//     while (!s.eof()) {
	//       string w;
	//       s >> w;
	//       cout << "word: " << w << endl;
	//     }
}

