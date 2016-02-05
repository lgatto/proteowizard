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

#include "ProgramOptions.h"
#include "InstrumentInterface.h"
#include "regex.h"

ProgramOptions::ProgramOptions ()
:
	doBenchmark(false),
	doSummary(false),
	printMetaData(true),
	printPeakData(false),
	doCentroid(false),
	doTandemCentroid(false),
	preferVendorCentroid(false)
	//doDeisotope(false),
{
}

void ProgramOptions::printArgs()
{
	cerr << "Global options:" << endl;
	cerr << "  benchmark mode: " << (doCentroid ? "Yes" : "No") << endl;
	cerr << "  summary mode: " << (doCentroid ? "Yes" : "No") << endl;
	cerr << endl;
	cerr << "Per scan options:" << endl;
	cerr << "  centroid MS scans: " << (doCentroid ? "Yes" : "No") << endl;
	cerr << "  centroid MS scans: " << (doCentroid ? "Yes" : "No") << endl;
	cerr << "  centroid MS scans: " << (doCentroid ? "Yes" : "No") << endl;
	cerr << "  centroid tandem MS scans: " << (doTandemCentroid ? "Yes" : "No") << endl;
	cerr << "  prefer vendor centroid: " << (preferVendorCentroid ? "Yes" : "No") << endl;
	cerr << endl;

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

bool ProgramOptions::parseArgs(const vector<string>& args, bool isGlobal)
{
	size_t curArg = 1;
	bool fInvalidOption = false;
	int nArgsLeft = args.size();
	isArgOption.resize(args.size(), 0);
	for( curArg = 1 ; curArg < args.size() ; ++curArg ) {
		if (args[curArg][0] != '-')
			continue;
		--nArgsLeft;
		if (!setOption(args, curArg, isGlobal)) {
			if (isArgOption[0] != 2) // not --help or --help-<ext>
				cerr << "Error: Invalid option: '" << args[curArg] << "'" << endl;
			fInvalidOption = true;
			break;
		} else
			isArgOption[curArg] = 1;
	}

	if (fInvalidOption) {
		return false;
	}

	if (nArgsLeft < 1) {
		cerr << "Error: Please specify an input file to convert" << endl;
		return false;
	} else {
		// look for input filepath when parsing from command-line
		for( curArg = 1 ; isGlobal && curArg < args.size(); ++curArg )
			if (!isArgOption[curArg])
			{
				inputFilepath = args[curArg];
				++curArg;
				break;
			}

		// treat the rest of the arguments as one or more input scan ranges
		for( ; curArg < args.size(); ++curArg )
		{
			if(isArgOption[curArg])
				continue;

			static regex scanRangeRegex( "(\\d+)-(\\d+)" );
			static regex scanNumberRegex( "(\\d+)" );
			smatch scanRangeMatch;
			string scanRangeStr = args[curArg];
			isArgOption[curArg] = 1;
			if( scanRangeStr == "all" )
			{
				inputScanNumbers.insert(0); // indicates "all scans"
			} else if( regex_match( scanRangeStr, scanRangeMatch, scanRangeRegex ) )
			{
				int low = lexical_cast<int>( scanRangeMatch[1] );
				int high = lexical_cast<int>( scanRangeMatch[2] );
				if( low > high )
				{
					cerr << "Warning: low number in range \"" << scanRangeStr << "\" is greater than high number" << endl;
					std::swap(low, high);
				}
				low = max(1, low);
				high = max(1, high);
				for( int num = low; num <= high; ++num )
					inputScanNumbers.insert(num);
			} else if( regex_match( scanRangeStr, scanRangeMatch, scanNumberRegex ) )
			{
				inputScanNumbers.insert( lexical_cast<int>( scanRangeMatch[1] ) );
			} else
			{
				isArgOption[curArg] = 0;
				cerr << "Warning: argument \"" << scanRangeStr << "\" could not be parsed as a scan range" << endl;
			}
		}
	}

	return true;
}

bool ProgramOptions::setOption(const vector<string>& args, size_t& i, bool isGlobal)
{
	string str(args[i]);

	if( str == "--print-header" || str == "-ph" )
	{
		printMetaData = true;
		printPeakData = false;
	} else if( str == "--print-peaks" || str == "-pp" )
	{
		printMetaData = false;
		printPeakData = true;
	} else if( str == "--print-all" || str == "-pa" )
	{
		printMetaData = true;
		printPeakData = true;
	} else if( str == "--centroid1" || str == "-c1" )
	{
		doCentroid = true;
	} else if( str == "--centroid" || str == "-c" )
	{
		doTandemCentroid = true;
	} else if( str == "--vendor-centroid" )
	{
		preferVendorCentroid = true;
	} else if( str == "--set" )
	{
		if( i+2 >= args.size() )
		{
			cerr << "Name or value for format-specific option is missing." << endl;
			return false;
		}
		formatSpecificOptions[args[i+1]].value = args[i+2];
		isArgOption[i] = 1;
		++i;
		isArgOption[i] = 1;
		++i;
	} else if(isGlobal)
	{
		if( str == "--help" )
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
			cerr << endl << "Usage: " << args[0] << " --set <option> <value>" << endl;
			exit(-1);
			return false;
		} else if( str == "--benchmark" || str == "-b" )
		{
			doBenchmark = true;
		} else if( str == "--summary" || str == "-s" )
		{
			doSummary = true;
		} else if( str == "--tsv" )
		{
			if( i+1 >= args.size() )
			{
				cerr << "Error: --tsv must have a field list to export." << endl;
			}

			if( i+1 >= args.size() || args[i+1] == "help" )
			{
				cerr << "The --tsv option allows export of a customized table of scan metadata.\n"
					 << "Usage: readms <input filepath> --tsv <field list>\n"
					 << "where \"<field list>\" is a comma-separated list of field names. The\n"
					 << "following field names are recognized:\n"
					 << "Field           Description\n"
					 << "================================================================================\n"
					 << "num             Scan number\n"
					 << "mslevel         MS level\n"
					 << "type            Scan type (using mzXML lexicon)\n"
					 << "time            Retention time in seconds\n"
					 << "peaks           Peak count\n"
					 << "ionsource       Ionization method\n"
					 << "polarity        Ionization polarity\n"
					 << "analyzer        Mass analysis method\n"
					 << "activation      Activation method (for tandem MS)\n"
					 << "tic             Total ion current\n"
					 << "centroided      1 if peak list is centroided, 0 if not\n"
					 << "deisotoped      1 if peak list is deisotoped, 0 if not\n"
					 << "parentnum       Precursor scan number (for tandem MS)\n"
					 << "parentmz        Precursor m/z (for tandem MS)\n"
					 << "parentcharge    Precursor charge (for tandem MS)\n"
					 << "parentmslevel   Precursor MS level (for tandem MS)\n"
					 << "stdout          The export table will be written to standard output\n"
					 << endl
					 << "Note: The parent* fields will return a comma delimited list of values when\n"
					 << "      the tandem MS has multiple associated precursors.\n"
					 << endl << endl;
				exit(-1);
			} else
			{
				const string& fieldList = args[i+1];
				vector<string> fieldNames;
				boost::split( fieldNames, fieldList, boost::is_any_of(",") );
				for( size_t j=0; j < fieldNames.size(); ++j )
				{
					const string& name = fieldNames[j];
					if( name == "num" || name == "mslevel" || name == "type" || name == "time" || name == "peaks" ||
						name == "ionsource" || name == "polarity" || name == "analyzer" || name == "activation" ||
						name == "tic" || name == "centroided" || name == "deisotoped" ||
						name == "parentnum" || name == "parentmz" || name == "parentcharge" || name == "parentmslevel" ||
						name == "stdout" )
						exportFieldList.push_back(name);
					else
						cerr << "Warning: \"" << name << "\" is not a valid field name (try \"--tsv help\")" << endl;
				}
				isArgOption[i] = 1;
				++i;
			}
		} else // parameter not valid
			return false;
	}
	else // parameter not valid
		return false;

	return true;
}


