// -*- mode: c++ -*-


/*
	File: MSTypes.h
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

#pragma once

#ifndef _INCLUDED_MSTYPES_H_
#define _INCLUDED_MSTYPES_H_

#include "common.h"

#define DEFAULT_CV_LABEL "MS"

struct LIBMSR_DECL ProcessingOption
{
	ProcessingOption(	const string& name = "",
						const string& type = "",
						const string& value = "",
						const string& description = "",
						const string& range = "" ) :
		name(name),
		type(type),
		value(value),
		description(description),
		range(range)
	{}

	string name;
	string type;
	string value;
	string description;
	string range; // []=exclusive, ()=inclusive, [min_value,max_value]
};

struct LIBMSR_DECL ProcessingOptions : public map<string, ProcessingOption>
{
	template<class T>
	T get(const string& name) const
	{
		const_iterator itr = find(name);
		if(itr != end())
			return lexical_cast<T>(itr->second.value);
		return T();
	}

	template<class T>
	T set(const string& name, const T& value)
	{
		T preValue;
		const_iterator itr = find(name);
		if(itr != end())
			preValue = lexical_cast<T>(itr->second.value);
		else
			preValue = T();
		itr->second.value = value;
		return preValue;
	}
};

typedef enum LIBMSR_DECL {
	MANUFACTURER_UNDEF = 0,
	THERMO, // specific incarnation unknown
	THERMO_SCIENTIFIC,
	THERMO_FINNIGAN,
	WATERS,
	ABI_SCIEX,
	AGILENT,

	NUM_MANUFACTURER_TYPES
} MSManufacturerType;
string toString(MSManufacturerType manufacturer);
string toOBO(MSManufacturerType manufacturer, const string& cvLabel = DEFAULT_CV_LABEL);
MSManufacturerType MSManufacturerTypeFromString(const string &manufacturer);


typedef enum LIBMSR_DECL {
	INSTRUMENTMODEL_UNDEF = 0,
	// thermo scientific
	LTQ,
	LTQ_FT,
	LTQ_FT_ULTRA,
	LTQ_ORBITRAP,
	LTQ_ORBITRAP_DISCOVERY,
	LTQ_ORBITRAP_XL,
	LXQ,

	// thermo finnigan
	LCQ_ADVANTAGE,
	LCQ_CLASSIC,
	LCQ_DECA,
	LCQ_DECA_XP_PLUS,
	LCQ_FLEET,
	TSQ_QUANTUM,

	// waters
	Q_TOF_MICRO,
	Q_TOF_ULTIMA,

	// ABI_SCIEX
	API_100,
	API_100_LC,
	API_150_MCA,
	API_150_EX,
	API_165,
	API_300,
	API_350,
	API_365,
	API_2000,
	API_3000,
	API_4000,
	GENERIC_SINGLE_QUAD,
	QTRAP,
	_4000_Q_TRAP,
	API_3200,
	_3200_Q_TRAP,
	API_5000,
	ELAN_6000,
	AGILENT_TOF,
	QSTAR,
	API_QSTAR_PULSAR,
	API_QSTAR_PULSAR_I,
	QSTAR_XL_SYSTEM,

	NUM_INSTRUMENTMODEL_TYPES
} MSInstrumentModelType;
string toString(MSInstrumentModelType instrumentModel);
string toOBO(MSInstrumentModelType instrumentModel, const string& cvLabel = DEFAULT_CV_LABEL);
//MSManufacturerType toMSManufacturerType(MSInstrumentModelType instrumentModel);
MSInstrumentModelType MSInstrumentModelTypeFromString(const string &instrumentModel);


typedef enum {
	ACQUISITIONSOFTWARE_UNDEF = 0,
	XCALIBUR,
	MASSLYNX,
	ANALYST,
	ANALYSTQS,
	MASSHUNTER,

	NUM_ACQUISITIONSOFTWARE_TYPES
} MSAcquisitionSoftwareType;
string toString(MSAcquisitionSoftwareType acquisitionSoftware);
string toOBO(MSAcquisitionSoftwareType acquisitionSoftware, const string& cvLabel = DEFAULT_CV_LABEL);
MSAcquisitionSoftwareType MSAcquisitionSoftwareTypeFromString(const string &acquisitionSoftware);


typedef enum {
	ANALYZER_UNDEF = 0,

	// originally defined from Thermo types
	ITMS,		// Ion Trap
	TQMS,		// Triple Quad
	SQMS,		// Single Quad
	TOFMS,		// TOF
	FTMS,		// ICR
	SECTOR,		// Sector

	// adding types for Waters

	// adding types for Analyst

	// adding types for Agilent MassHunter
	QTOF,		// quadrupole time of flight-- different than thermo's TOFMS?

	NUM_ANALYZER_TYPES
} MSAnalyzerType;
string toString(MSAnalyzerType analyzer);
string toOBO(MSAnalyzerType analyzer, const string& cvLabel = DEFAULT_CV_LABEL);
string toOBOText(MSAnalyzerType analyzer);
MSAnalyzerType MSAnalyzerTypeFromOBOText(const string &analyzer);
MSAnalyzerType MSAnalyzerTypeFromString(const string &analyzer);



typedef enum {
	DETECTOR_UNDEF = 0,

	NUM_DETECTOR_TYPES
} MSDetectorType;
string toString(MSDetectorType detector);
string toOBO(MSDetectorType detector, const string& cvLabel = DEFAULT_CV_LABEL);
string toOBOText(MSDetectorType detector);
MSDetectorType MSDetectorTypeFromOBOText(const string &detector);
MSDetectorType MSDetectorTypeFromString(const string &detector);

typedef enum {
	POLARITY_UNDEF = 0,
	POSITIVE,
	NEGATIVE,
	ANY, // does this really exist? from original ReAdW

	NUM_POLARITY_TYPES
} MSPolarityType;
MSPolarityType MSPolarityTypeFromOBOText(const string &polarity);
MSPolarityType MSPolarityTypeFromString(const string &polarity);


// rename?
typedef enum {
	SCANDATA_UNDEF = 0,
	CENTROID,
	PROFILE,

	NUM_SCANDATA_TYPES
} MSScanDataType;



typedef enum {
	IONIZATION_UNDEF = 0,

	// originally added for Thermo
	EI,		// Electron Impact
	CI,		// Chemical Ionization
	FAB,	// Fast Atom Bombardment
	ESI,	// Electrospray
	APCI,	// Atmospheric Pressure Chemical Ionization
	NSI,	// Nanospray
	TSP,	// Thermospray
	FD,		// Field Desorption
	MALDI,	// Matrix Assisted Laser Desorption Ionization
	GD,		// Glow Discharge

	// added for Waters

	// added for Analyst

	// added for Agilent MassHunter
	// assuming NSI is equivilent to nanospray ESI
	MS_CHIP,

	NUM_IONIZATION_TYPES
} MSIonizationType;
string toString(MSIonizationType ionization);
string toOBO(MSIonizationType ionization, const string& cvLabel = DEFAULT_CV_LABEL);
string toOBOText(MSIonizationType ionization);
MSIonizationType MSIonizationTypeFromOBOText(const string& ionization);
MSIonizationType MSIonizationTypeFromString(const string &ionization);



typedef enum {
	SCAN_UNDEF = 0,

	// originally added for Thermo
	FULL,		// full scan
	SIM,       	// single ion monitor
	SRM,       	// single reaction monitor
	CRM,       	// 
	Z,			// zoom scan
	Q1MS,		// q1 (quadrupole 1) mass spec scan
	Q3MS,		// q3 (quadrupole 3) mass spec scan

	// added for Waters

	// added for Analyst
	Q1Scan,
	Q1MI, 
	Q3Scan, 
	Q3MI, 
	MRM, 
	PrecursorScan,
	ProductIonScan, 
	NeutralLossScan, 
	TOFMS1, 
	TOFMS2, 
	TOFPrecursorIonScan,
	EPI, 
	ER, 
	MS3, 
	TDF, 
	EMS,
	EMC,

	// added for Agilent MassHunter

	NUM_SCAN_TYPES
} MSScanType;
string toString(MSScanType scanType);
string toOBO(MSScanType scanType, const string& cvLabel = DEFAULT_CV_LABEL);
string toOBOText(MSScanType scanType);
MSScanType MSScanTypeFromString(const string &scanType);

typedef enum {
	ACTIVATION_UNDEF = 0,

	// originally added for Thermo
	CID,		// collision induced dissociation
	MPD,		// 
	ECD,		// electron capture dissociation
	PQD,		// pulsed q dissociation
	ETD,		// electron transfer dissociation
	HCD,		// high energy collision dissociation
	SA,			// supplemental cid
	PTR,		// proton transfer reaction

	// added for Waters

	// added for Analyst

	// added for Agilent MassHunter
} MSActivationType;
string toString(MSActivationType activation);
string toOBO(MSActivationType activation, const string& cvLabel = DEFAULT_CV_LABEL);
string toOBOText(MSActivationType activation);
MSActivationType MSActivationTypeFromOBOText(const string &activation);
MSActivationType MSActivationTypeFromString(const string &activation);


typedef enum {
	SCAN_COORDINATE_UNDEF = 0,

	// added for Thermo

	// added for Waters

	// added for Analyst
	ABI_COORDINATE_SAMPLE,
	ABI_COORDINATE_PERIOD,
	ABI_COORDINATE_EXPERIMENT,
	ABI_COORDINATE_CYCLE,

	// added for Agilent MassHunter

	NUM_SCANCOORDINATE_TYPES
} ScanCoordinateType;
string toString(ScanCoordinateType scanCoordinateType);

struct CVAccession
{
	CVAccession( const string& labelWithNumber, char delimiter = ':' )
	{
		size_t delimIdx = labelWithNumber.find_first_of(delimiter);
		label_ = labelWithNumber.substr( 0, delimIdx );
		to_lower(label_);
		number_ = lexical_cast<int>( labelWithNumber.substr( delimIdx+1 ) );
	}

	CVAccession( const string& label, int number ) : label_(label), number_(number) { to_lower(label_); }
	CVAccession( int number = 0 ) : label_(defaultLabel()), number_(number) { to_lower(label_); }

	string label_;
	int number_;

	operator string() const
	{
		stringstream accessionString;
		accessionString << label_ << ':';
		accessionString.width(7);
		accessionString.fill(0);
		accessionString << right << number_;
		return accessionString.str();
	}

	static string defaultLabel() { return DEFAULT_CV_LABEL; }

	bool operator< (const CVAccession& rhs) const
	{
		if( label_ == rhs.label_ )
			return number_ < rhs.number_;
		else
			return label_ < rhs.label_;
	}
};

struct CVTerm
{
	CVTerm(
		const string& cvLabel = "",
		int accession = 0,
		const string& name = "",
		const string& value = "",
		int unitAccession = 0,
		const string& unitName = "")
	{
	}

	CVAccession accession;
	string name;
	string value;
	CVAccession unitAccession;
	string unitName;
};

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
struct CV : public map<CVAccession, CVTerm>
{
	bool readFromURL(const string& url);
};

#endif
