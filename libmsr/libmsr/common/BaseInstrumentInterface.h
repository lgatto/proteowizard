// -*- mode: c++ -*-


/*
    File: BaseInstrumentInterface.h
    Description: common interface to vendor-specific functions.
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


#pragma once

#include "common.h"
#include "Scan.h"
#include "Base64.h"
#include "InstrumentInfo.h"
#include "MSTypes.h"

class BaseInstrumentInterface {
public:

	string curSourceName_;

	long totalNumScans_;
	long firstScanNumber_;
	long lastScanNumber_;
	double startTimeInSec_;
	double endTimeInSec_;
	string timeStamp_;

	bool doCentroid_;
	bool doTandemCentroid_;
	bool doDeisotope_;
	bool verbose_;
	int accurateMasses_;
	int inaccurateMasses_;
	std::vector<int> chargeCounts_; 

	// used for MassLynx and MassHunter
	std::vector<string> inputFileNameList_;

	InstrumentInfo instrumentInfo_;

public:
	BaseInstrumentInterface()
		:	totalNumScans_(0),
			firstScanNumber_(0),
			lastScanNumber_(0),
			doCentroid_(false),
			doTandemCentroid_(false),
			doDeisotope_(false),
			verbose_(false),
			accurateMasses_(0),
			inaccurateMasses_(0),
			chargeCounts_(1000, 0),
			curScanNumber_(0)
	{}
	virtual ~BaseInstrumentInterface() {}

	virtual bool initInterface() = 0;
	virtual bool setInputFile(const string& fileName, const ProcessingOptions& options) = 0;
	virtual void setProcessingOptions(const ProcessingOptions& options) {};
	static ProcessingOptions getDefaultProcessingOptions() { return ProcessingOptions(); }
	virtual void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor) = 0;
	virtual void setDeisotoping(bool deisotope) = 0;
	virtual void setVerbose(bool verbose) = 0;

	virtual Scan* getScan() = 0; // returns next available scan (first, initally)
	virtual Scan* getScanHeader() = 0; // same as above, but without populating the peak arrays

	virtual Scan* getScan(long scanNumber) = 0; // returns requested scan by number, NULL if unavailable
	virtual Scan* getScanHeader(long scanNumber) = 0; // same as above, but without populating the peak arrays

	//virtual Scan* getScan(double retentionTime) = 0; // returns 

protected:
	long curScanNumber_;

	// centroid a scan with generic centroiding algorithm (thanks to Mike Hoopman)
	void doCentroidScan(Scan* scan);

};
