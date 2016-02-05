//#include "psico.h"
#include "platform.h"
#include "common.h"
#include "InstrumentInterface.h"
#include "ProgramOptions.h"
#include "filesystem.h"

void usage(const string& exename)
{
	cerr << endl << exename << /* " version " << DATAXML_VERSION <<*/  endl << endl;
	//Console Text Alignment Ruler
	//Ruler:          1         2         3         4         5         6         7         8
	//Ruler:012345678901234567890123456789012345678901234567890123456789012345678901234567890
	cerr << "Usage: " << exename << " [options] <input filepath> [<scan range>|\"all\"] ...\n"
		 << "Note: each [scan range] can be a single scan # or a range with syntax: low-high\n"
		 << endl
		 << "Global options (pass on command-line):\n"
		 << "  --benchmark, -b      Run benchmark (parse entire file with no output) [off]\n"
		 << "  --summary, -s        Print basic summary: #MS1/MS2/tot scans, LC time [off]\n"
		 << "  --tsv <field list>   Export custom table of scan headers (see \"--tsv help\")\n"
		 << "  --help               View this usage message.\n"
		 << "  --help-<ext>         View format-specific options (e.g. \"--help-wiff\")\n"
		 << endl
		 << "Per scan options (pass on command-line or to interactive query):\n"
		 << "  --print-header, -ph  Print scan header (metadata) only [default]\n"
		 << "  --print-peaks, -pp   Print peak list only\n"
		 << "  --print-all, -pa     Print both header and peak list\n"
		 << "  --centroid, -c       Centroid tandem MS scans acquired in profile mode [off]\n"
		 << "  --centroid1, -c1     Centroid MS scans acquired in profile mode [off]\n"
		 << "  --vendor-centroid    Prefer vendor centroid routine when available [off]\n"
		 << "  --set <name> <value> Set format-specific option (see \"--help-<ext>\")\n"
		 << endl
		 << "Note: passing a per scan option on the command-line makes that option the default\n"
		 << endl
		 << "Example: show scan 2375 of input.RAW, centroiding tandem MS scans by default\n"
		 << "  " << exename << " -c C:\\test\\input.RAW 2375\n"
		 << endl
		 << "Authors: Josh Tasman, Chee Hong Wong, Matt Chambers, and Jimmy Eng\n"
		 << "Based on original work by Patrick Pedriolli, David Shteynburg, and Robert Hubley\n"
		 << endl;
}

void interactive_usage()
{
	cerr << endl;
	cerr << "Query syntax: [options] [<scan range>|\"all\"] ...\n"
		 << "Note: each [scan range] can be a single scan # or a range with syntax: low-high\n"
		 << endl
		 << "Interactive options:\n"
		 << "  --print-header, -ph  Print scan header (metadata) only [default]\n"
		 << "  --print-peaks, -pp   Print peak list only\n"
		 << "  --print-all, -pa     Print both header and peak list\n"
		 << "  --centroid, -c       Centroid tandem MS scans acquired in profile mode [off]\n"
		 << "  --centroid1, -c1     Centroid MS scans acquired in profile mode [off]\n"
		 << "  --vendor-centroid    Prefer vendor centroid routine when available [off]\n"
		 << "  --set <name> <value> Set format-specific option (see --help-<ext>)\n"
		 << endl;
}

bool setInputFile( const string& inputFilepathStr, InstrumentInterface* pInterface )
{
	path inputFilepath(inputFilepathStr, filesystem::native);
	const string& inputFilepathComplete = system_complete(inputFilepath).native_file_string();
	try
	{
		bool interfaceSetInput = pInterface->setInputFile(inputFilepathComplete);
		if(!interfaceSetInput)
		{
			cerr << "Error: unable to set input file " << inputFilepathComplete << endl;
			return false;
		}
	} catch( std::exception& e )
	{
		cerr << "Caught fatal exception setting input file: " << e.what() << endl;
		return false;
	}
/*#ifdef WIN32
	catch( _com_error &com_error )
	{
		cerr << "Caught COM exception setting input file to \"" << inputFilepathComplete << "\": " << com_error.ErrorMessage() << endl;
		return -1;
	}
#endif*/
	return true;
}

void doExportFields( ostream& o, Scan* scan, const vector<string>& fields )
{
	for( size_t i=0; i < fields.size(); ++i )
	{
		if( i > 0 )
			o << '\t';

		const string& name = fields[i];
		if( name == "stdout" )
			continue;
		else if( name == "num" )
			o << scan->num_;
		else if( name == "mslevel" )
			o << scan->msLevel_;
		else if( name == "type" )
			o << scan->scanType_;
		else if( name == "time" )
			o << scan->retentionTimeInSec_;
		else if( name == "peaks" )
			o << scan->getNumDataPoints();
		else if( name == "ionsource" )
			o << toString(scan->ionization_);
		else if( name == "polarity" )
			o << scan->polarity_;
		else if( name == "analyzer" )
			o << toString(scan->analyzer_);
		else if( name == "tic" )
			o << scan->totalIonCurrent_;
		else if( name == "centroided" )
			o << scan->isCentroided_;
		else if( name == "deisotoped" )
			o << scan->isDeisotoped_;
		else if( scan->msLevel_ > 1 && !scan->precursorList_.empty() )
		{
			if( name == "activation" )
				o << toString(scan->activation_);
			else if( name == "parentnum" )
			{
				o << scan->precursorList_[0].num_;
				for( size_t j=1; j < scan->precursorList_.size(); ++j )
					o << "," << scan->precursorList_[j].num_;
			} else if( name == "parentmz" )
			{
				o << scan->precursorList_[0].MZ_;
				for( size_t j=1; j < scan->precursorList_.size(); ++j )
					o << "," << scan->precursorList_[j].MZ_;
			} else if( name == "parentcharge" )
			{
				o << scan->precursorList_[0].charge_;
				for( size_t j=1; j < scan->precursorList_.size(); ++j )
					o << "," << scan->precursorList_[j].charge_;
			} else if( name == "parentmslevel" )
			{
				o << scan->precursorList_[0].msLevel_;
				for( size_t j=1; j < scan->precursorList_.size(); ++j )
					o << "," << scan->precursorList_[j].msLevel_;
			}
			// should never get here!
		} else
			o << "n/a";
	}
	o << '\n';
}

ostream* tsvExportStream;
void doScanOutput( Scan* curScan, const ProgramOptions& options, size_t& totalPeakCount,
				   map<int, size_t>& msLevelCount, map<string, size_t>& chargeStateCount )
{
	if( !options.exportFieldList.empty() )
	{
		doExportFields( *tsvExportStream, curScan, options.exportFieldList );
		return;
	}

	if( options.doSummary )
	{
		totalPeakCount += curScan->getNumDataPoints();
		++ msLevelCount[curScan->msLevel_];
		if( curScan->msLevel_ > 1 )
		{
			stringstream chargeStrStream;
			chargeStrStream << curScan->precursorList_[0].charge_;
			for( size_t i=1; i < curScan->precursorList_.size(); ++i )
				chargeStrStream << "/" << curScan->precursorList_[i].charge_;
			++ chargeStateCount[chargeStrStream.str()];
		}
		return;
	}

	if( options.printMetaData )
	{
		cout.fill(' ');
		const int max_field_length = 22;
		cout.width(max_field_length);
		cout << right << "Scan #: "					<< curScan->num_ << "\n";
		cout.width(max_field_length);
		cout << right << "MS level: "				<< curScan->msLevel_ << "\n";
		cout.width(max_field_length);
		cout << right << "Scan type: "				<< toString(curScan->scanType_) << "\n";
		cout.width(max_field_length);
		cout << right << "Retention time: "			<< curScan->retentionTimeInSec_ << " seconds\n";
		cout.width(max_field_length);
		cout << right << "Peak count: "				<< curScan->getNumDataPoints() << "\n";
		cout.width(max_field_length);
		cout << right << "Ionization: "				<< toString(curScan->ionization_) << "\n";
		cout.width(max_field_length);
		cout << right << "Polarity: "				<< curScan->polarity_ << "\n";
		cout.width(max_field_length);
		cout << right << "Analyzer: "				<< toString(curScan->analyzer_) << "\n";
		cout.width(max_field_length);
		cout << right << "Is centroided: "			<< curScan->isCentroided_ << "\n";
		cout.width(max_field_length);
		cout << right << "Is deisotoped: "			<< curScan->isDeisotoped_ << "\n";
		cout.width(max_field_length);
		cout << right << "Total ion current: "		<< curScan->totalIonCurrent_ << "\n";
		cout.width(max_field_length);
		cout << right << "Min. observed m/z: "		<< curScan->minObservedMZ_ << "\n";
		cout.width(max_field_length);
		cout << right << "Max. observed m/z: "		<< curScan->maxObservedMZ_ << "\n";
		cout.width(max_field_length);
		cout << right << "Min. scanned m/z: "		<< curScan->startMZ_ << "\n";
		cout.width(max_field_length);
		cout << right << "Max. scanned m/z: "		<< curScan->endMZ_ << "\n";
		cout.width(max_field_length);
		cout << right << "Base peak intensity: "	<< curScan->basePeakIntensity_ << "\n";
		cout.width(max_field_length);
		cout << right << "Base peak m/z: "			<< curScan->basePeakMZ_ << "\n";

		if( curScan->msLevel_ > 1 )
		{
			cout.width(max_field_length);
			cout << right << "Activation: "			<< toString(curScan->activation_) << "\n";
			cout.width(max_field_length);
			cout << right << "Precursor list: ";
			for(size_t i=0; i < curScan->precursorList_.size(); ++i)
			{
				PrecursorScanInfo& p = curScan->precursorList_[i];
				cout <<	"(" << p.num_ << "," << p.charge_ << "," << p.MZ_ << "," << p.msLevel_ << "," <<
						p.intensity_ << "," << p.collisionEnergy_ << /*"," << p.accurateMZ_ <<*/ ") ";
			}
			cout << "\n";
		}
		cout << endl;
	}

	if( options.printPeakData )
	{
		int numDataPoints = curScan->getNumDataPoints();

		cout << "Peak m/z list:";
		for(int i=0; i < numDataPoints; ++i)
			cout << " " << curScan->mzArray_[i];
		cout << "\n";

		cout << "Peak intensity list:";
		for(int i=0; i < numDataPoints; ++i)
			cout << " " << curScan->intensityArray_[i];
		cout << "\n";
	}

	cout << endl;
}

int readInputScanNumbers( const set<int>& scanNumbers,
						  const ProgramOptions& options,
						  InstrumentInterface* pInterface )
{
	pInterface->setProcessingOptions(options.formatSpecificOptions);
	pInterface->setCentroiding(options.doCentroid, options.doTandemCentroid, options.preferVendorCentroid);
	pInterface->setDeisotoping(options.doDeisotope);

	if( scanNumbers.empty() ) // iterate entire file silently
	{
		Scan* curScan;
		if( options.printPeakData )
			curScan = pInterface->getScan();
		else
			curScan = pInterface->getScanHeader();
		while( curScan != NULL )
		{
			delete curScan;
			if( options.printPeakData )
				curScan = pInterface->getScan();
			else
				curScan = pInterface->getScanHeader();
		}
	} else
	{
		size_t totalScanCount = 0;
		if( *scanNumbers.begin() == 0 )
			totalScanCount = pInterface->getTotalScanCount();
		else
			totalScanCount = min( (size_t) pInterface->getTotalScanCount(), scanNumbers.size() );

		size_t totalPeakCount = 0;
		map<int, size_t> msLevelCount;
		map<string, size_t> chargeStateCount; // charge states could be ambiguous

		if( !options.exportFieldList.empty() )
		{
			*tsvExportStream << totalScanCount << endl;
			*tsvExportStream << options.exportFieldList[0];
			for( size_t i=1; i < options.exportFieldList.size(); ++i )
				if( options.exportFieldList[i] != "stdout" )
					*tsvExportStream << "\t" << options.exportFieldList[i];
			*tsvExportStream << endl;
		}

		if( *scanNumbers.begin() > 0 )
		{
			// get subset of scans by random access
			for( set<int>::const_iterator itr = scanNumbers.begin(); itr != scanNumbers.end(); ++itr )
			{
				Scan* curScan;
				if( options.printPeakData )
					curScan = pInterface->getScan(*itr);
				else
					curScan = pInterface->getScanHeader(*itr);

				if( curScan == NULL )
					continue;

				doScanOutput( curScan, options, totalPeakCount, msLevelCount, chargeStateCount );
				delete curScan;
			}
		} else
		{
			// get all scans by sequential access
			Scan* curScan;
			while(true)
			{
				if( options.printPeakData )
					curScan = pInterface->getScan();
				else
					curScan = pInterface->getScanHeader();

				if( curScan == NULL )
					break;

				doScanOutput( curScan, options, totalPeakCount, msLevelCount, chargeStateCount );
				delete curScan;
			}
		}

		if( options.doSummary )
		{
			if( !totalScanCount )
				cout << "No scans found." << endl;
			else
			{
				cout << "Total scan count: " << totalScanCount << endl;

				if( options.printPeakData )
					cout << "Total peak count: " << totalPeakCount << endl;

				cout << "MS level counts:";
				for( map<int, size_t>::iterator itr = msLevelCount.begin(); itr != msLevelCount.end(); ++itr )
					cout << " " << itr->first << ":" << itr->second;
				cout << endl;

				if( !chargeStateCount.empty() )
				{
					cout << "Charge state counts:";
					for( map<string, size_t>::iterator itr = chargeStateCount.begin(); itr != chargeStateCount.end(); ++itr )
						cout << " " << itr->first << ":" << itr->second;
					cout << endl;
				}
			}
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	ProgramOptions programOptions;

	const char *execName = "ReadMS";

	filesystem::path::default_name_check( filesystem::native );

	if( argc < 2 )
	{
		// min args: at least one input filemask
		usage(execName);
		return -1;
	}

	try
	{
		if( !programOptions.parseArgs(vector<string>(argv, argv+argc)) )
		{
			usage(execName);
			return -1;
		}

		//if( programOptions.verboseMode )
		//	programOptions.printArgs();

		InstrumentInterface* pInterface = new InstrumentInterface();
		pInterface->initInterface();
		//std::cin.get();
		// open file initially to determine total scan count, and first and last scan number
		if( !setInputFile( programOptions.inputFilepath, pInterface ) )
			throw runtime_error( "could not open input file \"" + programOptions.inputFilepath + "\"" );

		long totalScanCount = pInterface->getTotalScanCount();
		long firstScanNumber = pInterface->getFirstScanNumber();
		long lastScanNumber = pInterface->getLastScanNumber();

		if( programOptions.doBenchmark )
		{
			// silently benchmark all scans
			if( !programOptions.inputScanNumbers.empty() )
				cerr << "Warning: scan range arguments are ignored in benchmark mode" << endl;

			set<int> emptySet; // read all scans silently
			cout << "Benchmark started." << endl;
			time_t then, now;
			time(&then);
			int rv = readInputScanNumbers( emptySet, programOptions, pInterface );
			if( rv < 0 )
				return rv;
			time(&now);
			cout << "Benchmark finished; " << now-then << " seconds elapsed." << endl;

		} else if( programOptions.inputScanNumbers.empty() )
		{
			// interactive mode
			if( !programOptions.exportFieldList.empty() )
			{
				cerr << "Warning: --tsv option requires some scan range arguments on the command-line" << endl;
				programOptions.exportFieldList.clear(); // no export from interactive mode
			}

			while(true)
			{
				cout << totalScanCount << " total scans detected; first scan: " << firstScanNumber << "; last scan: " << lastScanNumber << endl;
				cout << "Enter a query ('?' for help, 'q' to quit): ";
				string querySuffix;
				std::getline( std::cin, querySuffix );
				boost::trim(querySuffix);
				if( querySuffix == "?" )
					interactive_usage();
				else if( querySuffix == "q" || querySuffix == "quit" )
				{
					cout << endl;
					break;
				} else if( querySuffix.empty() )
					cout << endl;
				else
				{
					string queryStr = "interactive " + querySuffix; // args[0] is not part of query
					ProgramOptions queryOptions( programOptions ); // query inherits global defaults
					vector<string> queryArgs;
					boost::split( queryArgs, queryStr, boost::is_space() );
					if( !queryOptions.parseArgs(queryArgs, false) || queryOptions.inputScanNumbers.empty() )
						interactive_usage();
					else
					{
						int rv = readInputScanNumbers( queryOptions.inputScanNumbers, queryOptions, pInterface );
						if( rv < 0 )
							return rv;
					}
				}
			}
		} else
		{
			vector<string>::iterator stdoutItr = find( programOptions.exportFieldList.begin(), programOptions.exportFieldList.end(), "stdout" );
			if( !programOptions.exportFieldList.empty() )
			{
				if( stdoutItr != programOptions.exportFieldList.end() )
				{
					// stdout field should always be at the end
					programOptions.exportFieldList.erase(stdoutItr);
					programOptions.exportFieldList.push_back("stdout");
					stdoutItr = programOptions.exportFieldList.end()-1;
					tsvExportStream = &std::cout;
				} else
					tsvExportStream = new ofstream( change_extension( programOptions.inputFilepath, ".tsv" ).native_file_string().c_str(), ios::binary );
			}

			// use scan ranges from command-line
			int rv = readInputScanNumbers( programOptions.inputScanNumbers, programOptions, pInterface );

			if( stdoutItr == programOptions.exportFieldList.end() )
				delete tsvExportStream;

			if( rv < 0 )
				return rv;
		}

		pInterface->freeInterface();

	} catch( std::exception& e )
	{
		cerr << "Caught fatal exception: " << e.what() << endl;
		return 1;
	}

	return 0;
}
