// -*- mode: c++ -*-


/*
    File: ProgramOptions.h
    Description: program options parsing for mzWiff
    Date: July 31, 2007

    Copyright (C) 2007 Chee Hong WONG, Bioinformatics Institute


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
#include "MSTypes.h"

class ProgramOptions
{
public:

	bool			doBenchmark;
	bool			doSummary;

	bool			printMetaData;
	bool			printPeakData;

	bool			doCentroid;
	bool			doTandemCentroid;
	bool			preferVendorCentroid;
	bool			doDeisotope;

	string			inputFilepath;
	set<int>		inputScanNumbers;

	vector<string>	exportFieldList;

	ProcessingOptions	formatSpecificOptions;

	ProgramOptions();
	void printArgs();
	bool parseArgs(const vector<string>& args, bool isGlobal = true);

private:

	bool setOption(const vector<string>& args, size_t& i, bool isGlobal);
	vector<int> isArgOption;

	template< typename T >
	bool setValueOption(const vector<string>& args, size_t i, T& var)
	{
		if( i+1 < args.size() ) {
			try {
				T tmp = lexical_cast<T>( args[i+1] );
				var = tmp;
			} catch(...) {
				cerr << "Value for argument " << i << " ('" << args[i] << "') is invalid." << endl;
				return false;
			}
			return true;
		} else
		{
			cerr << "Value for argument " << i << " ('" << args[i] << "') is missing." << endl;
			return false;
		}
	}
};
