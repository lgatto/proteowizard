// -*- mode: c++ -*-


/*
    File: mzXMLWriter.cpp
    Description: instrument-independent mzXML writer.
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
#include "mzXMLWriter.h"

#include <vector>
#include <iostream>
#include <string>

using namespace std;



mzXMLWriter::mzXMLWriter(const string& programName,
						 const string& programVersion,
						 BaseInstrumentInterface* BaseInstrumentInterface) : 
MassSpecXMLWriter(programName,
				  programVersion,
				  BaseInstrumentInterface)
{
}




void mzXMLWriter::writeDocument(void)	  
{
	vector<fstream::pos_type> scanOffsetVec;
	scanOffsetVec.reserve(instrumentInterface_->totalNumScans_);

	startDocument();

	//xml header and namespace info 
	open("mzXML");
	attr("xmlns", "http://sashimi.sourceforge.net/schema_revision/mzXML_3.0");
	attr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	attr("xsi:schemaLocation", "http://sashimi.sourceforge.net/schema_revision/mzXML_3.0 http://sashimi.sourceforge.net/schema_revision/mzXML_3.0/mzXML_idx_3.0.xsd");

	condenseAttr_ = true;
	open("msRun");
	attr("scanCount", instrumentInterface_->totalNumScans_);
	string startTime("PT");
	startTime += toString(instrumentInterface_->startTimeInSec_);
	startTime += "S";
	attr("startTime", startTime);
	string endTime("PT");
	endTime += toString(instrumentInterface_->endTimeInSec_);
	endTime += "S";
	attr("endTime", endTime);

	if (instrumentInterface_->instrumentInfo_.manufacturer_ != WATERS) {
		open("parentFile");
		attr("fileName", convertToURI(inputFileName_, hostName_));
		attr("fileType", "RAWData");
		attr("fileSha1", sha1Report_);
		close(); // parentFile
	} else {
		for (std::vector<string>::size_type f=0; f<inputFileNameList_.size(); ++f) {
			open("parentFile");
			attr("fileName", convertToURI(inputFileNameList_[f], hostName_));
			attr("fileType", "RAWData");
			attr("fileSha1", inputFileSHA1List_[f]);
			close(); // parentFile
		}
	}

	// map analyzer type to unique instrument id
	vector<size_t> analyzerToInstrumentID(NUM_ANALYZER_TYPES, 0);

	// write a separate instrument for each analyzer
	size_t numInstruments = instrumentInterface_->instrumentInfo_.analyzerList_.size();
	for(size_t i=0; i < numInstruments; ++i) {
		open("msInstrument");

		// if multiple "instruments", give each one a unique id
		if (numInstruments > 1) {
			attr("msInstrumentID", (i+1));
			analyzerToInstrumentID[ instrumentInterface_->instrumentInfo_.analyzerList_[i] ] = i+1;
		}

		open("msManufacturer");
		attr("category", "msManufacturer");
		attr("value", toString(instrumentInterface_->instrumentInfo_.manufacturer_));
		close(); // msManufacturer

		open("msModel");
		attr("category", "msModel");
		attr("value", toString(instrumentInterface_->instrumentInfo_.instrumentModel_));
		close(); // msModel

		open("msIonisation");
		attr("category", "msIonisation");
		attr("value", toString(instrumentInterface_->instrumentInfo_.ionSource_));
		close(); // msIonisation

		open("msMassAnalyzer");
		attr("category", "msMassAnalyzer");
		attr("value", toString(instrumentInterface_->instrumentInfo_.analyzerList_[i]));
		close(); // msMassAnalyzer

		open("msDetector");
		attr("category", "msDetector");
		attr("value", toString(instrumentInterface_->instrumentInfo_.detector_)); 
		close(); // msDetector

		open("software");
		attr("type", "acquisition");
		attr("name", toString(instrumentInterface_->instrumentInfo_.acquisitionSoftware_));
		attr("version", instrumentInterface_->instrumentInfo_.acquisitionSoftwareVersion_);
		close(); // softwareType

		close(); // msInstrument
	}

	open("dataProcessing");
	if (instrumentInterface_->doCentroid_) attr("centroided", "1");
	if (instrumentInterface_->doDeisotope_) attr("deisotoped", "1");

	open("software");
	attr("type", "conversion"); // fix
	attr("name", programName_); // fix
	attr("version", programVersion_); // fix
	close(); // software
	close(); // dataProcessing

	stack<int> lastMSLevel;

	size_t curScanCount = 0;
	size_t curPeakCount = 0;
	while(true) {
		Scan* curScan = instrumentInterface_->getScan();

		if (verbose_) {
			if ((curScanCount % 10) == 0 || curScan == NULL) {
				cout << curScanCount << " of " << instrumentInterface_->totalNumScans_ << " scans converted; " <<
						curPeakCount << " peaks written.\t\r";
			}
		}

		if (curScan == NULL) {
			if (verbose_ && curScanCount >= 10)
				cout << endl;
			break;
		}

		++curScanCount;

		if( curScan->msLevel_ < minMsLevel_ || curScan->msLevel_ > maxMsLevel_ ||
			curScan->num_ < minScanNum_ || curScan->num_ > maxScanNum_ )
		{
			delete curScan;
			continue;
		}

		// do we need to close the previous scan?
		// (yes, if the stack is not empty,
		// and this level is greater than the last)
		// WCH: merged (resultant) scan is standalone
		//      (i.e. no single associated MS1 scan)
		while (!lastMSLevel.empty() && 
			(curScan->msLevel_ <= lastMSLevel.top() 
			|| 0!=curScan->isScanMergedResult())) {
				close(); // scan
				lastMSLevel.pop();
		}

		lastMSLevel.push(curScan->msLevel_);

		// save index
		scanOffsetVec.push_back(curFileLength_);
		condenseAttr_ = true;
		open("scan");
		attr("num", curScan->num_);
		//condenseAttr_ = false;
		attr("msLevel", curScan->msLevel_);
		attr("peaksCount", curScan->getNumDataPoints());

		string polarity;
		if (curScan->polarity_ == NEGATIVE) {
			polarity = "-";
		}
		else if (curScan->polarity_ == POSITIVE) {
			polarity = "+";
		}
		else if (curScan->polarity_ == POLARITY_UNDEF) {
			polarity = "any";
		}
		else {
			cerr << "unknown polarity" << endl;
			exit(-1);
		}
		attr("polarity", polarity);

		if (curScan->isCentroided_ && !instrumentInterface_->doCentroid_)
			attr("centroided", 1);
		if (curScan->isDeisotoped_ && !instrumentInterface_->doDeisotope_)
			attr("deisotoped", 1);

		attr("scanType", toString(curScan->scanType_));
		attr("retentionTime", string("PT") + toString(curScan->retentionTimeInSec_) + string("S")); // in seconds
		attr("lowMz", curScan->minObservedMZ_);
		attr("highMz", curScan->maxObservedMZ_);
		attr("startMz", curScan->startMZ_);
		attr("endMz", curScan->endMZ_);
		attr("basePeakMz", curScan->basePeakMZ_);
		attr("basePeakIntensity", curScan->basePeakIntensity_);
		attr("totIonCurrent", curScan->totalIonCurrent_);

		if (numInstruments > 1)
			attr("msInstrumentID", analyzerToInstrumentID[ curScan->analyzer_ ]);

		// MassLynx only
		if (curScan->isMassLynx_) {
			if (curScan->isCalibrated_) {
				attr("scanType", "calibration");
			}
		}

		// <scan ... merged="1" ...>
		if (curScan->isMerged_ ) {
			attr("merged", "1");
			if (-1 != curScan->mergedScanNum_) {
				attr("mergedScanNum", curScan->mergedScanNum_);
			}
		}

		// <nativeScanRef ...>
		if (curScan->nativeScanRef_.getNumCoordinates()>0) {
			NativeScanRef &nativeScanRef = curScan->nativeScanRef_;
			open("nativeScanRef");
			attr("coordinateType", toString(nativeScanRef.getCoordinateType()));
			ScanCoordinateType name;
			string value;
			for (std::vector<NativeScanRef::CoordinateNameValue>::size_type i=0; 
				i<nativeScanRef.getNumCoordinates(); i++) {
				open("coordinate");
				nativeScanRef.getCoordinate(i, name, value);
				attr("name", toString(name));
				attr("value", value);
				close(); // coordinate
			}
			close(); // nativeScanRef
		}

		// <scanOrigin ...>
		if (curScan->getNumScanOrigins()>1) {
			for (long i=0; i<curScan->getNumScanOrigins(); i++) {
				open("scanOrigin");
				attr("parentFileID", curScan->scanOriginParentFileIDs[i]);
				attr("num", curScan->scanOriginNums[i]);
				close(); // scanOrigin
			}
		}

		if (curScan->msLevel_ >= 2) {
			// precursor info
			if (curScan->precursorList_.back().collisionEnergy_ != 0) {
				attr("collisionEnergy", curScan->precursorList_.back().collisionEnergy_);
			}
			condenseAttr_ = true;
			open("precursorMz");
			if (curScan->precursorList_.back().num_ > 0) {
				attr("precursorScanNum", curScan->precursorList_.back().num_);
			}
			if (curScan->precursorList_.back().intensity_ > 0) {
				attr("precursorIntensity", curScan->precursorList_.back().intensity_);
			}
			else {
				// precursorIntensity is required
				attr("precursorIntensity", 0);
			}
			if (curScan->precursorList_.back().charge_ > 0) {
				attr("precursorCharge", curScan->precursorList_.back().charge_);
			}
			data(toString(curScan->precursorList_.back().MZ_));
			close(); // precursorMz
			condenseAttr_ = false;
		} // precursor


		{
			// paired values must have the same precision, so use most precision for both
			bool useDoublePrecision = mzValuesDoublePrecision_ || intenValuesDoublePrecision_;
			ScanConverter scanConverter(curScan, doCompression_, useDoublePrecision, useDoublePrecision); // will free mem on descope
			condenseAttr_ = true;
			open("peaks");
			attr("precision", (useDoublePrecision ? "64" : "32"));
			condenseAttr_ = false;
			attr("byteOrder", "network");
			attr("pairOrder", "m/z-int");
			if (scanConverter.isMZIntCompressed_) {
				attr("compressionType", "zlib");
				attr("compressedLen", (long)scanConverter.mzIntArrayCompressedByteSize_);
			}
			data(scanConverter.mzIntB64String_);
			close(); // peaks

			curPeakCount += curScan->getNumDataPoints();
		}

		// scan is closed at top of loop

		delete curScan;
	}

	while (!lastMSLevel.empty()) {
		// close the remaining scans
		close(); //scan
		lastMSLevel.pop();
	}

	close(); //msRun

	// index

	// Save the offset for the indexOffset element
	fstream::pos_type indexOffset = curFileLength_; // pOut_->tellp(); //_telli64(foutFD_);

	condenseAttr_ = true;
	open("index");
	attr("name", "scan");

	for (size_t scanNum = 0; scanNum < scanOffsetVec.size(); scanNum++ ) {
		open("offset");
		attr("id", (scanNum+1)); // scans start at 1, not 0
		data(lexical_cast<string>(scanOffsetVec[scanNum]));
		close(); // offset
	}

	close(); //index

	open("indexOffset");
	data(lexical_cast<string>(indexOffset));
	close(); // indexOffset

	open("sha1");
	noattr();
	pOut_->flush();
	if (verbose_) cout << endl;
	cout << "Calculating sha1-sum of mzXML" << endl;
	sha1Report_[0] = 0;
	sha1_.Reset();
	if( !(sha1_.HashFile(outputFileName_.c_str()))) {
		cerr << "Cannot open file " << outputFileName_ << " for sha-1 calculation\n";
		exit(-1);// Cannot open file for sha1
	}
	sha1_.Final();
	sha1_.ReportHash(sha1Report_, SHA1::REPORT_HEX);
	cout << "--done (mzXML sha1):" << sha1Report_ << endl;
	data(sha1Report_);
	close(); //sha1

	close(); //mzXML
}
