// -*- mode: c++ -*-


/*
    File: Scan.cpp
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


#define LIBMSR_SOURCE
#include "Scan.h"

void Scan::setNumDataPoints(int numDataPoints, bool resizeArrays) {
	numDataPoints_ = numDataPoints;
	if(resizeArrays) {
		mzArray_.resize(numDataPoints, MzValueType());
		intensityArray_.resize(numDataPoints, IntenValueType());
	}
}

void Scan::addDataPoint(MzValueType mz, IntenValueType intensity) {
	mzArray_.push_back(mz);
	intensityArray_.push_back(intensity);
	numDataPoints_ = (int) mzArray_.size();
}

void Scan::setNumScanOrigins(int numScanOrigins) {
	numScanOrigins_ = numScanOrigins;
	scanOriginNums.resize(numScanOrigins, 0);
	scanOriginParentFileIDs.resize(numScanOrigins, "");
}


Scan::Scan() {
	num_ = 0;
	msLevel_ = 0;
	startMZ_ = -1;
	endMZ_ = -1;
	minObservedMZ_ = -1;
	maxObservedMZ_ = -1;
	basePeakMZ_ = -1;
	basePeakIntensity_ = -1;
	totalIonCurrent_ = -1;
	retentionTimeInSec_ = -1;
	polarity_ = POLARITY_UNDEF;
	analyzer_ = ANALYZER_UNDEF;
	ionization_ = IONIZATION_UNDEF;
	scanType_ = SCAN_UNDEF;
	activation_ = ACTIVATION_UNDEF;

	isCentroided_ = false;
	isDeisotoped_ = false;

	// thermo scans only:
	isThermo_ = false;
	thermoFilterLine_ = "";

	// MassLynx scans only
	isMassLynx_ = false;
	isCalibrated_ = false;

	isMerged_ = false;
	mergedScanNum_ = -1;

	numDataPoints_ = 0;

	numScanOrigins_ = 0;
}


Scan::~Scan() {
}
