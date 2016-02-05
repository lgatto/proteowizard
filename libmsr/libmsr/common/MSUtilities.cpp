// -*- mode: c++ -*-


/*
    File: MSUtilities.cpp
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


#define LIBMSR_SOURCE
#include "MSUtilities.h"
#include <cctype> // for toupper, tolower
#include <algorithm>

using namespace std;

#ifdef WIN32
#include <tchar.h> // for _stprintf
string convertBstrToString(const BSTR& bstring)
{
	_bstr_t bTmp(bstring);
	return string((const char *)bTmp);
}
#endif

string convertToURI(const string& filename,
					const string& hostname) 
{	
	string output = "file://";

	// Change the first part of the path (the volume name) to the hostname of the localmachine
	if (filename.find(':') != string::npos) {
		// local path
		output += hostname;
		output += "/";
		output += filename.substr(0, filename.find(':')); // drive
		output += "/";
		output += filename.substr(filename.find(':') + 2); // path, skipping "drive:/"
	}
	else if (filename.substr(0,2) == "\\") {
		// network path
		output += filename.substr(2); // loose the "//"
	}
	else { 
		// if the path was relative, just keep it
		output = filename;
	}

	// TODO: URI should have space (i.e.' ') converted to "%20"
	// Since we are on Windows change all the "\" to  "/"
	string::size_type pos=output.find('\\');
	while (pos != string::npos) {
		output[pos] = '/';
		pos=output.find('\\');
	}

	return output;
}


// ex ID string: urn:lsid:unknown.org:OGDEN_c_thermotest_1min.mzML
string convertToIDString(const string& filename,
							  const string& hostname) {
	string output = "urn:lsid:unknown.org:";

	// Change the first part of the path (the volume name) to the hostname of the localmachine
	if (filename.find(':') != string::npos) {
		// local path
		output += hostname;
		output += "_";
		output += filename.substr(0, filename.find(':')); // drive
		output += "_";
		output += filename.substr(filename.find(':') + 2); // path, skipping "drive:/"
	}
	else if (filename.substr(0,2) == "\\") {
		// network path
		output += filename.substr(2); // loose the "//"
	}
	else { 
		// if the path was relative, just keep it
		output = filename;
	}

	// Since we are on Windows change all the "\" to  "_"
	string::size_type pos=output.find('\\');
	while (pos != string::npos) {
		output[pos] = '_';
		pos=output.find('\\');
	}

	return output;
}

#ifdef _WIN32
string toString(__int64 value) {
	ostringstream converter;
	converter << value;
	return converter.str();
}

string toString(__w64 unsigned int value) {
	ostringstream converter;
	converter << value;
	return converter.str();
}
#endif

string toString(long value) {
	ostringstream converter;
	converter << value;
	return converter.str();
}

string toString(int value) {
	ostringstream converter;
	converter << value;
	return converter.str();
}

string toString(double value) {
	ostringstream converter;
	converter << value;
	return converter.str();
}

string toString(float value) {
	ostringstream converter;
	converter << value;
	return converter.str();
}

double toDouble(const string& string) {
	istringstream converter;
	converter.str(string);
	double value;
	converter >> value;
	return value;
}



int toInt(const string& string) {
	istringstream converter;
	converter.str(string);
	int value;
	converter >> value;
	return value;
}
