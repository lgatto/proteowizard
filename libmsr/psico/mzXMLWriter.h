// -*- mode: c++ -*-


/*
    File: mzXMLWriter.h
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



#pragma once

#include "common.h"
#include "MassSpecXMLWriter.h"

class mzXMLWriter : public MassSpecXMLWriter {
public:
	mzXMLWriter(
		const string& programName,
		const string& programVersion,
		BaseInstrumentInterface* BaseInstrumentInterface);
	~mzXMLWriter() {}
	virtual void writeDocument(void);
};