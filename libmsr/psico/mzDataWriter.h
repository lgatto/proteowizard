// -*- mode: c++ -*-


/*
    File: mzDataWriter.h
    Description: instrument-independent mzData writer.
    Date: October 12, 2007

    Copyright (C) 2007 Matt Chambers, Vanderbilt MSRC


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

#ifndef _INCLUDED_MZDATAWRITER_H_
#define _INCLUDED_MZDATAWRITER_H_

#include "common.h"
#include "MassSpecXMLWriter.h"

class mzDataWriter : public MassSpecXMLWriter {
public:
	mzDataWriter(
		const string& programName,
		const string& programVersion,
		BaseInstrumentInterface* BaseInstrumentInterface);
	~mzDataWriter() {}
	virtual void writeDocument(void);

	bool setInputFile(const string& inputFileName);

private:

	template<class ValueT>
	void cvParam(
			const string& cvLabel,
			const string& accession,
			const string& name,
			const ValueT& value,
			const string& unitAccession = "",
			const string& unitName = "")
	{
		open("cvParam");
		attr("cvLabel", cvLabel);
		attr("accession", accession);
		attr("name", name);
		attr("value", value);
		if (unitAccession != "") {
			attr("unitAccession", unitAccession);
			if (unitName != "") {
				attr("unitName", unitName);
			}
		}
		close(); // cvParam
	}
};

#endif
