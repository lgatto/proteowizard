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

#include "psico.h"
#include "MSTypes.h"

enum InputFormat
{
	THERMO_RAW_FORMAT,
	ABI_WIFF_FORMAT,
	ABI_WIFFQS_FORMAT,
	AGILENT_RAW_FORMAT,
	AGILENT_MASSHUNTER_FORMAT
};

typedef pair< string, InputFormat >	InputFile;
typedef set< InputFile >			InputFileList;
typedef set< string >				InputFileNames;

enum OutputFormat
{
	MZXML_FORMAT,
	MZML_FORMAT,
	MZDATA_FORMAT,
	TEST_FORMAT
};

extern const char* OutputFormatStrings[4];

class ProgramOptions
{
public:

	// Common options
	bool			doCompression;
	bool			doRulers;
	bool			doCentroid;
	bool			doTandemCentroid;
	bool			preferVendorCentroid;
	bool			doDeisotope;
	bool			verboseMode;

	bool			mzValuesDoublePrecision;
	bool			intenValuesDoublePrecision;

	int				filterMinMsLevel;
	int				filterMaxMsLevel;

	int				filterMinScanNum;
	int				filterMaxScanNum;

	InputFileList	inputFileList;
	InputFileNames	inputFileNames;

	string				outputPath;
	string				outputFileName;
	set<OutputFormat>	outputFormats;

	string			instrumentModel;
	string			instrumentIonisation;
	string			instrumentMSType;
	string			instrumentDetector;

	ProcessingOptions	formatSpecificOptions;

	// RAW only options
	// WIFF only options

	ProgramOptions ();
	void printArgs(void);
	bool parseArgs(int argc, char* argv[]);

private:

	OutputFormat getDefaultOutputFormat() { return MZXML_FORMAT; }

	bool setOption(int argc, char *argv[], int& i);
	vector<int> isArgOption;

	template< typename T >
	bool setValueOption(int argc, char *argv[], int i, T& var)
	{
		if( i+1 < argc ) {
			try {
				T tmp = lexical_cast<T>( argv[i+1] );
				var = tmp;
			} catch(...) {
				cerr << "Value for argument " << i << " ('" << argv[i] << "') is invalid." << endl;
				return false;
			}
			return true;
		} else
		{
			cerr << "Value for argument " << i << " ('" << argv[i] << "') is missing." << endl;
			return false;
		}
	}
};
