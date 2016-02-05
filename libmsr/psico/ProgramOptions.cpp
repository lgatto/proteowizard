// -*- mode: c++ -*-


/*
    File: ProgramOptions.cpp
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

#include "common.h"
#include "ProgramOptions.h"
#include "InstrumentInterface.h"

const char* OutputFormatStrings[4] =
{
	"mzXML",
	"mzML",
	"mzData",
	"test"
};

ProgramOptions::ProgramOptions ()
:
	doCompression(false),
	doRulers(false),
	doCentroid(false),
	doTandemCentroid(false),
	preferVendorCentroid(false),
	doDeisotope(false),
	verboseMode(true),
	mzValuesDoublePrecision(false),
	intenValuesDoublePrecision(false),
	filterMinMsLevel(1),
	filterMaxMsLevel(std::numeric_limits<int>::max()),
	filterMinScanNum(1),
	filterMaxScanNum(std::numeric_limits<int>::max()),
	instrumentModel(""),
	instrumentIonisation(""),
	instrumentMSType(""),
	instrumentDetector("")
{
	/*formatSpecificOptions["IntensityPeakCutoff"] = "0.0";
	formatSpecificOptions["IntensityPeakCutoffPercentage"] = "0.1";
	formatSpecificOptions["PrecursorGroupMassTolerance"] = "0.0";
	formatSpecificOptions["MaxCyclesBetweenGroups"] = "10";
	formatSpecificOptions["MinCyclesPerGroup"] = "1";
	formatSpecificOptions["DeterminePrecursorCharge"] = "0";
	formatSpecificOptions["MinPeakCount"] = "10";*/
}

void
ProgramOptions::printArgs(void) {

	/*cerr << "Information settings:" << endl;
	cerr << "  instrument model: " << instrumentModel << endl;
	cerr << "  ionisation used: " << instrumentIonisation << endl;
	cerr << "  mass spectrometry type: " << instrumentMSType << endl;
	cerr << "  detector used: " << instrumentDetector << endl;
	cerr << "  use information recorded in wiff file: " << useWiffFileInformation << endl;
	cerr << endl;*/

	cerr << "Global options:" << endl;
	cerr << "  minimum MS level: " << filterMinMsLevel << endl;
	if( filterMaxMsLevel == std::numeric_limits<int>::max() )
		cerr << "  maximum MS level: none" << endl;
	else
		cerr << "  maximum MS level: " << filterMaxMsLevel << endl;
	cerr << "  minimum scan number: " << filterMinScanNum << endl;
	if( filterMaxScanNum == std::numeric_limits<int>::max() )
		cerr << "  maximum scan number: none" << endl;
	else
		cerr << "  maximum scan number: " << filterMaxScanNum << endl;
	cerr << "  centroid MS scans: " << (doCentroid ? "Yes" : "No") << endl;
	cerr << "  centroid tandem MS scans: " << (doTandemCentroid ? "Yes" : "No") << endl;
	cerr << "  prefer vendor centroid: " << (preferVendorCentroid ? "Yes" : "No") << endl;
	cerr << "  compression: " << (doCompression ? "Yes" : "No") << endl;
	cerr << "  rulers: " << (doRulers ? "Yes" : "No") << endl;
	cerr << "  verbose: " << (verboseMode ? "Yes" : "No") << endl;
	cerr << "  m/z value precision: " << (mzValuesDoublePrecision ? "64" : "32") << endl;
	cerr << "  intensity value precision: " << (intenValuesDoublePrecision ? "64" : "32") << endl;
	cerr << "  output formats:";
	for(set<OutputFormat>::iterator itr = outputFormats.begin(); itr != outputFormats.end(); ++itr)
		cerr << " " << OutputFormatStrings[*itr];
	cerr << endl << endl;

	if( !formatSpecificOptions.empty() )
	{
		cerr << "Format-specific options:" << endl;
		for( ProcessingOptions::const_iterator itr = formatSpecificOptions.begin();
			 itr != formatSpecificOptions.end();
			 ++itr )
			cerr << "  " << itr->first << ": " << itr->second.value << endl;
		cerr << endl;
	}
}

bool 
ProgramOptions::parseArgs(int argc, char* argv[]) {
	int curArg = 1;
	bool fInvalidOption=false;
	int nArgsLeft = argc;
	isArgOption.resize(argc, 0);
	for( curArg = 1 ; curArg < argc ; ++curArg ) {
		if (*argv[curArg] != '-')
			continue;
		--nArgsLeft;
		if (!setOption(argc, argv, curArg)) {
			if (isArgOption[0] != 2) // not --help or --help-<ext>
				cerr << "Error: Invalid option: '" << argv[curArg] << "'" << endl;
			fInvalidOption = true;
			break;
		} else
			isArgOption[curArg] = 1;
	}

	if (fInvalidOption) {
		return false;
	}

	if (outputFormats.empty())
		outputFormats.insert(getDefaultOutputFormat());

	if (nArgsLeft < 1) {
		cerr << "Error: Please specify an input file to convert" << endl;
		return false;
	} else {
		for( curArg = 1 ; curArg < argc; ++curArg )
			if (!isArgOption[curArg])
			{
				if (string(argv[curArg]).find(".dta") != string::npos)
					FindFilesByMask( argv[curArg], inputFileNames, 1 );
				else
					FindFilesByMask( argv[curArg], inputFileNames );
			}
	}

	return true;
}

bool 
ProgramOptions::setOption(int argc, char *argv[], int& i) {
	//int    iTmp = 0;
	//double dTmp = 0.0;

	string str(argv[i]);

	// Common options
	if( str == "--mzXML" )
	{
		outputFormats.insert(MZXML_FORMAT);
	} else if( str == "--mzML" )
	{
		outputFormats.insert(MZML_FORMAT);
	} else if( str == "--mzData" )
	{
		outputFormats.insert(MZDATA_FORMAT);
	} else if( str == "--test" )
	{
		outputFormats.insert(TEST_FORMAT);
	} else if( str == "--mz32" )
	{
		mzValuesDoublePrecision = false;
	} else if( str == "--mz64" )
	{
		mzValuesDoublePrecision = true;
	} else if( str == "--inten32" )
	{
		intenValuesDoublePrecision = false;
	} else if( str == "--inten64" )
	{
		intenValuesDoublePrecision = true;
	} else if( str == "--min-ms-level" )
	{
		if( !setValueOption<int>( argc, argv, i, filterMinMsLevel ) )
			return false;
		filterMinMsLevel = max( 1, filterMinMsLevel );
		isArgOption[i] = 1;
		++i;
	} else if( str == "--max-ms-level" )
	{
		if( !setValueOption<int>( argc, argv, i, filterMaxMsLevel ) )
			return false;
		isArgOption[i] = 1;
		++i;
	} else if( str == "--min-scan-num" )
	{
		if( !setValueOption<int>( argc, argv, i, filterMinScanNum ) )
			return false;
		filterMinScanNum = max( 1, filterMinScanNum );
		isArgOption[i] = 1;
		++i;
	} else if( str == "--max-scan-num" )
	{
		if( !setValueOption<int>( argc, argv, i, filterMaxScanNum ) )
			return false;
		isArgOption[i] = 1;
		++i;
	} else if( str == "--centroid1" || str == "-c1" )
	{
		doCentroid = true;
	} else if( str == "--centroid" || str == "-c" )
	{
		doTandemCentroid = true;
	} else if( str == "--vendor-centroid" )
	{
		preferVendorCentroid = true;
	} else if( str == "--compress" || str == "-z" )
	{
		doCompression = true;
	} else if( str == "--rulers" || str == "-r" )
	{
		doRulers = true;
	} else if( str == "--verbose" || str == "-v" )
	{
		verboseMode = true;
	} else if( str == "--outdir" || str == "-o" )
	{
		if( !setValueOption<string>( argc, argv, i, outputPath ) )
			return false;
		isArgOption[i] = 1;
		++i;
	} else if( str == "--help" )
	{
		// output usage statement
		isArgOption[0] = 2;
		return false;
	} else if( str.find("--help-") == 0 )
	{
		string ext = str.substr(7);
		cerr << "Help on format-specific options for \"" << ext << "\":" << endl << endl;
		//       0         1         2         3         4         5         6         7         8
		//       012345678901234567890123456789012345678901234567890123456789012345678901234567890
		cerr << "Option    Type      Range     Default   Description" << endl;
		cerr << "================================================================================" << endl;
		ProcessingOptions defaultOptions = InstrumentInterface::getDefaultProcessingOptions(ext);
		for( ProcessingOptions::const_iterator itr = defaultOptions.begin(); itr != defaultOptions.end(); ++itr )
		{
			cerr.fill(' ');
			cerr.width(10);
			cerr << left << itr->first;
			cerr.width(10);
			cerr << left << itr->second.type;
			cerr.width(10);
			cerr << left << itr->second.range;
			cerr.width(10);
			cerr << left << itr->second.value;
			cerr.width(40);
			cerr << left << itr->second.description;
			cerr << endl;
		}
		cerr << endl << "Usage: " << argv[0] << " --set <option> <value>" << endl;
		exit(-1);
		return false;
	} else if( str == "--set" )
	{
		if( i+2 >= argc )
		{
			cerr << "Name or value for format-specific option is missing." << endl;
			return false;
		}
		formatSpecificOptions[argv[i+1]].value = argv[i+2];
		isArgOption[i] = 1;
		++i;
		isArgOption[i] = 1;
		++i;
	}
	else // parameter not valid
		return false;

	return true;
}


