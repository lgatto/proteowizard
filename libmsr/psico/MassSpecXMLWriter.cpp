// -*- mode: c++ -*-


/*
    File: MassSpecXMLWriter.cpp
    Description: instrument-independent common parent to mzXML and mzML writers.
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
#include "common.h"
#include "MassSpecXMLWriter.h"

using namespace std;

MassSpecXMLWriter::MassSpecXMLWriter(
									 const string& programName,
									 const string& programVersion,
									 BaseInstrumentInterface* BaseInstrumentInterface)
	:	foutFD_(0),
		scanNumberOffset_(0),
		mzValuesDoublePrecision_(false),
		intenValuesDoublePrecision_(false),
		minMsLevel_(1),
		maxMsLevel_(std::numeric_limits<int>::max()),
		doCentroiding_(false),
		doDeisotoping_(false),
		doCompression_(false),
		verbose_(false)
{
	programName_ = programName;
	programVersion_ = programVersion;
	instrumentInterface_ = BaseInstrumentInterface;
	sha1Report_[0] = 0;
	// get hostname for the URIs
	//hostName_ = getComputerName();
	//cout << "(got computer name: " << hostName_ << ")" << endl;
}

MassSpecXMLWriter::~MassSpecXMLWriter() {
	//(*pOut_).close();
	if (pOut_)
		delete pOut_;
	//if (foutFD_)
	//	::close(foutFD_);
}

void
MassSpecXMLWriter::setPrecision(bool mzDoublePrecision, bool intenDoublePrecision) {
	mzValuesDoublePrecision_ = mzDoublePrecision;
	intenValuesDoublePrecision_ = intenDoublePrecision;
}

void
MassSpecXMLWriter::setMinMsLevel(int msLevel) {
	minMsLevel_ = msLevel;
}

void
MassSpecXMLWriter::setMaxMsLevel(int msLevel) {
	maxMsLevel_ = msLevel;
}

void
MassSpecXMLWriter::setMinScanNum(int scanNum) {
	minScanNum_ = scanNum;
}

void
MassSpecXMLWriter::setMaxScanNum(int scanNum) {
	maxScanNum_ = scanNum;
}

void
MassSpecXMLWriter::setCentroiding(bool centroid, bool tandemCentroid) {
	doCentroiding_ = centroid && tandemCentroid;
}

void
MassSpecXMLWriter::setDeisotoping(bool deisotope) {
	doDeisotoping_ = deisotope;
}

void 
MassSpecXMLWriter::setCompression(bool compression) {
	doCompression_ = compression;
}

void
MassSpecXMLWriter::setVerbose(bool verbose) {
	verbose_ = verbose;
}

bool
MassSpecXMLWriter::setInputFile(const string& inputFileName) {
	inputFileName_ = inputFileName;

	// TODO: deal with MassHunter data files correctly
	if (instrumentInterface_->instrumentInfo_.manufacturer_ == AGILENT) {
		// hack for trapper debugging
		inputFileNameList_.push_back(inputFileName+"AcqData\\Devices.xml");
		inputFileSHA1List_.push_back("dummy");
		return true;
	}

	//cout << "(got file URI: " << convertToURI(inputFileName_, hostName_) << ")" << endl;

	// run sha1

	if (instrumentInterface_->instrumentInfo_.manufacturer_ != WATERS) {
		// process one input file
		// sha1 input file
		if (verbose_ ) {
			cout << "Calculating sha1-sum of " << inputFileName << endl;
		}
		sha1_.Reset();
		if ( !(sha1_.HashFile(inputFileName.c_str()))) {
			cerr << "Cannot open file " << inputFileName << " for sha-1 calculation\n";
			return false;// Cannot open file for sha1
		}
		sha1_.Final();
		sha1_.ReportHash(sha1Report_, SHA1::REPORT_HEX);
		if (verbose_) {
			cout << "--done (sha1):" << sha1Report_ << endl;
		}
	}
	else {
		// Waters
		// expect instrument interface has list of input filenames
		if (verbose_) {
			cout << "Processing SHA1 for " << inputFileNameList_.size() << " input files." << endl;
		}
		for (vector<string>::size_type f=0; f<instrumentInterface_->inputFileNameList_.size(); ++f) {
			string inputFile = instrumentInterface_->inputFileNameList_[f];
			if (verbose_) {		
				cout << "Calculating sha1-sum of " << inputFile << endl;
			}
			sha1Report_[0] = 0; // necessary-- fix SHA1 interface to be better
			sha1_.Reset();
			if ( !(sha1_.HashFile(inputFile.c_str()))) {
				cerr << "Cannot open file " << inputFile << " for sha-1 calculation\n";
				return false;// Cannot open file for sha1
			}
			sha1_.Final();
			sha1_.ReportHash(sha1Report_, SHA1::REPORT_HEX);
			if (verbose_) {
				cout << "--done (sha1):" << sha1Report_ << endl;
			}

			// save these
			inputFileNameList_.push_back(inputFile);
			inputFileSHA1List_.push_back(sha1Report_);
		}

	}
	return true;
}

bool
MassSpecXMLWriter::setOutputFile(const string& outputFileName) {
	outputFileName_ = outputFileName;

	// Open output mzML file or die
	ofstream* pfOut = new ofstream(outputFileName.c_str(), ios::binary);
	pOut_ = pfOut;
	if( !pOut_->good() ) {
		cerr << "Error opening output file " << outputFileName << endl;;
		return false;
	}
	else {
		// For the indexing we are going to need an fd stream
		// so that we have a 64-bit ftell function
		//if( !(foutFD_ = ::open( outputFileName.c_str(), O_RDONLY)) ) {
		//	cerr << "Error opening output file for indexing\n";
		//	return false;
		//}
	}
	return true;
}


