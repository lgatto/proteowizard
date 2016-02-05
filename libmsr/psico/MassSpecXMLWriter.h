// -*- mode: c++ -*-


/*
    File: MassSpecXMLWriter.h
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

#ifndef _INCLUDED_MASSSPECXMLWRITER_H_
#define _INCLUDED_MASSSPECXMLWRITER_H_

#include "common.h"
#include "SimpleXMLWriter.h"

#include "SHA1.h"
#include "MSUtilities.h"
#include "ScanConverter.h"
#include "SimpleXMLWriter.h"
#include "BaseInstrumentInterface.h"

class MassSpecXMLWriter : public SimpleXMLWriter {
protected:
	int foutFD_; // filedescriptor, for indexing
	std::vector<long> scanOffsets_;
	BaseInstrumentInterface* instrumentInterface_;
	SHA1 sha1_; // md5sum calculator
	char sha1Report_[1024]; // sha1 hash
	string inputFileName_;
	// only used for MassLynx, currently:
	std::vector<string> inputFileNameList_;
	std::vector<string> inputFileSHA1List_;

	Base64 base64Transformer_;

	string outputFileName_;
	string programName_;
	string programVersion_;
	string hostName_;

	int scanNumberOffset_;

	bool mzValuesDoublePrecision_;
	bool intenValuesDoublePrecision_;

	int minMsLevel_;
	int maxMsLevel_;

	int minScanNum_;
	int maxScanNum_;

	bool doCentroiding_;
	bool doDeisotoping_;
	bool doCompression_;
	bool verbose_;
public:
	MassSpecXMLWriter(
		const string& programName,
		const string& programVersion,
		BaseInstrumentInterface* BaseInstrumentInterface);

	virtual ~MassSpecXMLWriter();

	void setInstrumentInterface(BaseInstrumentInterface* instrumentInterface)
	{
		instrumentInterface_ = instrumentInterface;
	}

	virtual void setPrecision(bool mzDoublePrecision, bool intenDoublePrecision);
	virtual void setMinMsLevel(int msLevel);
	virtual void setMaxMsLevel(int msLevel);
	virtual void setMinScanNum(int scanNum);
	virtual void setMaxScanNum(int scanNum);
	virtual void setCentroiding(bool centroid, bool tandemCentroid);
	virtual void setDeisotoping(bool deisotope);
	virtual void setCompression(bool compression);
	virtual void setVerbose(bool verbose);
	virtual bool setInputFile(const string& inputFileName);
	virtual bool setOutputFile(const string& outputFileName);

	virtual void writeDocument(void) = 0;
};

#endif
