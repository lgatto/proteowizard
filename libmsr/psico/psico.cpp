// -*- mode: c++ -*-

/*
	Program: ReAdW ("read double-u")
	Description: read Thermo/Xcalibur native RAW mass-spec data files,
	and produce XML (mzXML, mzML) output.  Please note, the program 
	requires the XRawfile library from ThermoFinnigan to run.

	Date: July 2007
	Author: Joshua Tasman, ISB Seattle, 2007, based on original work by 
	Pedrioli Patrick, ISB, Proteomics (original author), and Brian
	Pratt, InSilicos

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

#include "psico.h"
#include "common.h"
#include "platform.h"
#include "InstrumentInterface.h"
#include "mzXMLWriter.h"
#include "mzMLWriter.h"
#include "mzDataWriter.h"
#include "TestWriter.h"
#include "ProgramOptions.h"
#include "filesystem.h"

void usage(const string& exename) {
	cerr << endl << exename << /* " version " << DATAXML_VERSION <<*/  endl << endl;
	//Console Text Alignment Ruler
	//Ruler:          1         2         3         4         5         6         7         8
	//Ruler:012345678901234567890123456789012345678901234567890123456789012345678901234567890
	cerr << "Usage: " << exename << " [options] <input filemask> [another input filemask] ...\n"
		 << "Options common to all input formats:\n"
		 << "  --mzXML              Write mzXML output format [default]\n"
		 << "  --mzML               Write mzML output format\n"
		 << "  --mzData             Write mzData output format\n"
		 //<< "  --merge              Merge all input files into a single output file\n"
		 << "  --mz32               Encode m/z values in 32-bit precision [default]\n"
		 << "  --mz64               Encode m/z values in 64-bit precision\n"
		 << "  --inten32            Encode intensity values in 32-bit precision [default]\n"
		 << "  --inten64            Encode intensity values in 64-bit precision\n"
		 << "  --min-ms-level <n>   Min. MS level to include scan in output [1]\n"
		 << "  --max-ms-level <n>   Max. MS level to include scan in output [none]\n"
		 << "  --min-scan-num <n>   Min. scan number to include in output [1]\n"
		 << "  --max-scan-num <n>   Max. scan number to include in output [none]\n"
		 << "  --centroid, -c       Centroid tandem MS scans acquired in profile mode [off]\n"
		 << "  --centroid1, -c1     Centroid MS scans acquired in profile mode [off]\n"
		 << "  --vendor-centroid    Prefer vendor centroid routine when available [off]\n"
		 << "  --compress, -z       Compress peaks with zlib [off]\n"
		 << "  --rulers, -r         Compress peaks with m/z rulers [off]\n"
		 << "  --verbose, -v        Give verbose progress information [on]\n"
		 << "  --outdir, -o         Set the directory to which output files are written [.]\n"
		 << "  --help               View this usage message.\n"
		 << "  --help-<ext>         View format-specific options (e.g. \"--help-wiff\")\n"
		 << "  --set <name> <value> Set format-specific option (see \"--help-<ext>\")\n"
		 << endl
		 << "Example: convert input.RAW to input.mzXML, centroiding tandem MS scans\n"
		 << "  " << exename << " --mzXML -c C:\\test\\input.RAW\n"
		 << endl
		 << "Authors: Josh Tasman, Chee Hong Wong, and Matt Chambers\n"
		 << "Based on original work by Patrick Pedriolli, David Shteynburg, and Robert Hubley\n"
		 << endl;
}

int main(int argc, char* argv[])
{
	ProgramOptions programOptions;

	// force the program name to ReAdW,
	// regardless of what it was called on the command line
	const char *execName = "PSIco"; // argv[0];
	string version = "mzML Prototype";

	filesystem::path::default_name_check(filesystem::native);

	//const char *execName = argv[0];
	//// remove preceeding path info from execName
	//const char* slashPos=strrchr(execName,'\\');
	//if ( slashPos ) {
	//	execName = slashPos+1;
	//}

	if (argc < 2) {
		// min args: at least one input filemask
		usage(execName);
		return -1;
	}

	try
	{
		if (!programOptions.parseArgs(argc, argv)) {
			usage(execName);
			return -1;
		}
		if (programOptions.verboseMode) {
			programOptions.printArgs();
		}

		InstrumentInterface* pInterface = new InstrumentInterface();
		pInterface->initInterface();

		MassSpecXMLWriter* msWriter;

		//CV cv;
		//cv.readFromURL("http://psidev.cvs.sourceforge.net/*checkout*/psidev/psi/psi-ms/mzML/controlledVocabulary/psi-ms.obo");

		for( InputFileNames::const_iterator itr = programOptions.inputFileNames.begin();
			 itr != programOptions.inputFileNames.end();
			 ++itr )
		{
			path inputFilepath(*itr, filesystem::native);
			const string& inputFilename = system_complete(inputFilepath).native_file_string();

			for( set<OutputFormat>::iterator itr2 = programOptions.outputFormats.begin();
				 itr2 != programOptions.outputFormats.end();
				 ++itr2 )
			{
				switch(*itr2)
				{
					default:
					case MZML_FORMAT:
						msWriter = new mzMLWriter(execName, version, pInterface->getBaseInterface());
						break;
					case MZXML_FORMAT:
						msWriter = new mzXMLWriter(execName, version, pInterface->getBaseInterface());
						break;
					case MZDATA_FORMAT:
						msWriter = new mzDataWriter(execName, version, pInterface->getBaseInterface());
						break;
					case TEST_FORMAT:
						msWriter = new TestWriter(pInterface->getBaseInterface());
						break;
				}

				if (programOptions.verboseMode) {
					msWriter->setVerbose(true);
				}

				try
				{
					bool interfaceSetInput = pInterface->setInputFile(inputFilename);
					msWriter->setInstrumentInterface(pInterface->getBaseInterface());
					if (!interfaceSetInput || !msWriter->setInputFile(inputFilename)) {
						cerr << "Error: unable to set input file " << inputFilename << endl;
						delete msWriter;
						continue;
					}
				} catch( std::exception& e )
				{
					cerr << "Caught fatal exception setting input file: " << e.what() << endl;
					cerr << "Skipping file \"" << inputFilename << "\"." << endl;
					delete msWriter;
					continue;
				}
#ifdef WIN32
				catch( _com_error &com_error )
				{
					cerr << "Caught COM exception setting input file to \"" << inputFilename << "\": " << com_error.ErrorMessage() << endl;
					cerr << "Skipping file \"" << inputFilename << "\"." << endl;
					delete msWriter;
					continue;
				}
#endif

				pInterface->setProcessingOptions(programOptions.formatSpecificOptions);
				pInterface->setCentroiding(programOptions.doCentroid, programOptions.doTandemCentroid, programOptions.preferVendorCentroid);
				pInterface->setDeisotoping(programOptions.doDeisotope);

				programOptions.outputFileName = change_extension( MAKE_PATH_FOR_BOOST( inputFilename ), string(".") + OutputFormatStrings[*itr2] ).string();
				if (!programOptions.outputPath.empty())
					programOptions.outputFileName = path(programOptions.outputPath).string() + "/" + path(programOptions.outputFileName).leaf();
				if (system_complete(path(programOptions.outputFileName, filesystem::native)).native_file_string() == inputFilename)
				{
					cerr << "Error: cannot read from and write to the same file at one time (reading and writing the same XML format?)" << endl;
					delete msWriter;
					continue;
				}

				if (!msWriter->setOutputFile(programOptions.outputFileName)) {
					cerr << "Error: unable to open " << programOptions.outputFileName << endl;
					delete msWriter;
					continue;
				}

				msWriter->setPrecision(programOptions.mzValuesDoublePrecision, programOptions.intenValuesDoublePrecision);
				msWriter->setMinMsLevel(programOptions.filterMinMsLevel);
				msWriter->setMaxMsLevel(programOptions.filterMaxMsLevel);
				msWriter->setMinScanNum(programOptions.filterMinScanNum);
				msWriter->setMaxScanNum(programOptions.filterMaxScanNum);
				msWriter->setCentroiding(programOptions.doCentroid, programOptions.doTandemCentroid);
				msWriter->setDeisotoping(programOptions.doDeisotope);
				msWriter->setCompression(programOptions.doCompression);

				try
				{
					msWriter->writeDocument();
				} catch( std::exception& e )
				{
					cerr << "Caught fatal exception during conversion: " << e.what() << endl;
					delete msWriter;
					continue;
				}

				delete msWriter;
			} // output formats loop

		} // input files loop

		SAFE_DELETE(pInterface);

	} catch( std::exception& e )
	{
		cerr << "Caught fatal exception: " << e.what() << endl;
		return 1;
	}

	return 0;
}
