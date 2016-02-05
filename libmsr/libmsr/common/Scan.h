// -*- mode: c++ -*-


/*
    File: Scan.h
    Description: instrument-independent scan representation.
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

#ifndef _INCLUDED_SCAN_H_
#define _INCLUDED_SCAN_H_
#pragma once

#include "common.h"
#include "MSTypes.h"

class LIBMSR_DECL NativeScanRef {
public:
	typedef std::pair<ScanCoordinateType,string> CoordinateNameValue;
public:
	MSManufacturerType coordinateType_;
	std::vector<CoordinateNameValue> coordinates_;

public:
	inline std::vector<CoordinateNameValue>::size_type getNumCoordinates(void) const 
	{
		return coordinates_.size();
	}
	inline void addCoordinate(ScanCoordinateType name, const string &value)
	{
		coordinates_.push_back(CoordinateNameValue(name, value));
	}
	inline void getCoordinate(std::vector<CoordinateNameValue>::size_type index, ScanCoordinateType &name, string &value)
	{
		if (index>=0 && index<coordinates_.size()) {
			const CoordinateNameValue &coordinate = coordinates_[index];
			name = coordinate.first;
			value = coordinate.second;
		} else {
			name = SCAN_COORDINATE_UNDEF;
			value.resize(0);
		}
	}
	inline void setCoordinateType (MSManufacturerType coordinateType)
	{
		coordinateType_ = coordinateType;
	}
	inline MSManufacturerType getCoordinateType (void)
	{
		return coordinateType_;
	}

public:
	NativeScanRef() {}
	NativeScanRef(MSManufacturerType coordinateType) {setCoordinateType(coordinateType);}
	~NativeScanRef() {}
};

struct LIBMSR_DECL PrecursorScanInfo {
	PrecursorScanInfo(	long num, int msLevel, double MZ,
						int charge = 0, double intensity = 0, double collisionEnergy = 0,
						bool accurateMZ = false )
		:	num_(num), msLevel_(msLevel),
			charge_(charge), MZ_(MZ), intensity_(intensity), collisionEnergy_(collisionEnergy),
			accurateMZ_(accurateMZ)
	{}

	long num_;
	int msLevel_;
	int charge_;
	double MZ_;
	double intensity_;
	double collisionEnergy_;  // for MSn, this refers to the collision which produced the nth level fragment
	bool accurateMZ_;
};

class LIBMSR_DECL Scan {
public:
	typedef double MzValueType;
	typedef double IntenValueType;

	string source_;
	long num_;
	int msLevel_;

	// last scan range
	MzValueType startMZ_; // min scanned
	MzValueType endMZ_; // max scanned

	// all scan ranges
	typedef pair<MzValueType, MzValueType> MzRange;
	vector<MzRange> mzRanges_;

	MzValueType minObservedMZ_; // min observed
	MzValueType maxObservedMZ_; // max observed

	// base peak: peak with greatest intesity
	MzValueType basePeakMZ_;
	IntenValueType basePeakIntensity_;

	IntenValueType totalIonCurrent_;
	double retentionTimeInSec_;

	MSPolarityType polarity_;
	MSAnalyzerType analyzer_; // keep this per-scan, for example for FT which has IT or FT
	MSIonizationType ionization_;
	MSScanType scanType_; // full, zoom, srm, etc
	MSActivationType activation_;

	bool isCentroided_;
	bool isDeisotoped_;

	// for MSn >= 2
	vector<PrecursorScanInfo> precursorList_;

	// for thermo scans only
	bool isThermo_;
	string thermoFilterLine_;

	// for MassLynx_ scans only
	bool isMassLynx_;
	bool isCalibrated_;

	// for MS2 averaging or merging
	bool isMerged_;
	long mergedScanNum_;

	NativeScanRef nativeScanRef_;

private:
	int numDataPoints_;

public:
	int getNumDataPoints(void) const {return numDataPoints_;}
	void setNumDataPoints(int numDataPoints, bool resizeArrays = true); // (re)allocates arrays
	void addDataPoint(MzValueType mz, IntenValueType intensity);

	vector<MzValueType> mzArray_;
	vector<IntenValueType> intensityArray_;


//protected:
	// MCC: make public and use to indicate count of acquisitions in mzData
	int numScanOrigins_;

public:
	void setNumScanOrigins(int numScanOrigins);
	int getNumScanOrigins(void) {return numScanOrigins_;}
	int isScanMergedResult() { return (scanOriginNums.size()>0 ? 1 : 0); }


	std::vector<long> scanOriginNums;
	std::vector<string> scanOriginParentFileIDs;

public:
	Scan();
	~Scan();
};

#endif // _INCLUDED_SCAN_H_
