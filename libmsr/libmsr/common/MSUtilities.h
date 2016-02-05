// -*- mode: c++ -*-


/*
    File: MSUtilities.h
    Description: shared utilities for mzXML-related projects.
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

#ifndef _INCLUDED_MSUTILITIES_H_
#define _INCLUDED_MSUTILITIES_H_

#include "common.h"

string convertToURI(const string& filename,
						 const string& hostname);


 
// ex ID string: urn:lsid:unknown.org:OGDEN_c_thermotest_1min.mzML
string convertToIDString(const string& filename,
							  const string& hostname);

#ifdef _WIN32
#include "Comdef.h" // for BSTR
string convertBstrToString(const BSTR& bstring);
string toString(__int64 value);
string toString(__w64 unsigned int value);
#endif
string toString(long value);
string toString(int value);
string toString(double value);
string toString(float value);

double toDouble(const string& string);
int toInt(const string& string);

#endif
