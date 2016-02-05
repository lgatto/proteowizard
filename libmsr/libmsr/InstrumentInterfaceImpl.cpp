#define LIBMSR_SOURCE
#include "InstrumentInterfaceImpl.h"
#include "filesystem.h"

void InstrumentInterfaceImpl::setInterface(const InterfaceType& type)
{
	freeInterface();

	string strLibraryVersion;

	switch(type)
	{
		case INTERFACE_XML_MZXML:
			m_pInterface = new mzXMLInterface();
			break;

		case INTERFACE_XML_MZDATA:
			m_pInterface = new mzDataInterface();
			break;

		/*case INTERFACE_XML_MZML:
			m_pInterface = new mzXMLInterface();
			break;*/

		case INTERFACE_TEXT_MGF:
			m_pInterface = new MGFInterface();
			break;

		case INTERFACE_TEXT_DTA:
			m_pInterface = new DTAInterface();
			break;

		case INTERFACE_ABI_ANALYST:
#ifdef LIBMSR_HAS_ABI_ANALYST
			// try to initialize Analyst or AnalystQS WIFF interface
			if(0 == getLibraryVersion(LIBRARY_Analyst, strLibraryVersion))
				m_pInterface = new AnalystInterface();
			else if(0 == getLibraryVersion(LIBRARY_AnalystQS, strLibraryVersion))
				m_pInterface = new AnalystQSInterface();
			break;
#endif

		case INTERFACE_THERMO_XCALIBUR:
#ifdef LIBMSR_HAS_THERMO_XCALIBUR
			// try to initialize Thermo RAW interface
			m_pInterface = new ThermoInterface();
			break;
#endif

		case INTERFACE_BRUKER_COMPASSXPORT:
#ifdef LIBMSR_HAS_BRUKER_COMPASSXPORT
			// try to initialize Bruker interface
			m_pInterface = new BrukerInterface();
			break;
#endif
		default:
			m_pInterface = NULL;
	}

	if(m_pInterface == NULL || !m_pInterface->initInterface())
		freeInterface();
}

bool InstrumentInterfaceImpl::initInterface()
{
	// free previously initialized interface
	freeInterface();

	return true;
}

string GetXMLFileRootElement(const string& filepath)
{
	const static char xmlDeclTag[] = "<?xml";
	const static char xmlDeclTagUTF16[] = {'<',0,'?',0,'x',0,'m',0,'l',0};
	const static string xmlDeclTagStr(xmlDeclTag);
	const static string xmlDeclTagUTF16Str(xmlDeclTagUTF16, xmlDeclTagUTF16 + 10);

	ifstream fileStream( filepath.c_str(), ios::binary );
	if( fileStream.is_open() )
	{
		char buf[256];
		fileStream.getline(buf, 255, '>' ); // should contain the XML declaration
		string line1(buf, buf + 255);

		// Is this an XML file? (allow for byte order mark before the declaration)
		if( line1.find( xmlDeclTagStr ) <= 5 || line1.find( xmlDeclTagUTF16Str ) <= 5 )
		{
			// Yes, so the first non-comment, non-processing instruction element gives the type
			std::getline( fileStream, line1, '>' );
			size_t rootElIdx;
			rootElIdx = line1.find( '<', 0 );
			while( rootElIdx == string::npos || line1[rootElIdx+1] == '?' || line1[rootElIdx+1] == '!' )
			{
				std::getline( fileStream, line1, '>' );
				rootElIdx = line1.find( '<' );
			}
			string rootElBytes = line1.substr( rootElIdx+1, line1.find_first_of( " \r\n", rootElIdx+2 )-rootElIdx-1 );
			if(rootElBytes.find('\0') != string::npos)
			{
				string rootEl;
				BOOST_FOREACH(char c, rootElBytes)
					if(c != 0)
						rootEl.push_back(c);
				return rootEl;
			} else
				return rootElBytes;
		}
	}
	return "unknown";
}

bool InstrumentInterfaceImpl::setInputFile(const string& fileName, const ProcessingOptions& options)
{
	path inputFilepath(fileName, filesystem::native);
	const string& inputFilename = system_complete(inputFilepath).native_file_string();
	string ext = to_lower_copy( GetFilenameExtension(inputFilename) );
	if(ext == "raw") {
#ifndef LIBMSR_HAS_THERMO_XCALIBUR
		cerr << "Error: Thermo RAW interface is not supported, RAW files cannot be read." << endl;
#endif
		setInterface(INTERFACE_THERMO_XCALIBUR);
		if(!m_pInterface) {
			return false;
		}
	} else if(ext == "wiff") {
#ifndef LIBMSR_HAS_ABI_ANALYST
		cerr << "Error: Analyst WIFF interface is not supported, WIFF files cannot be read." << endl;
#endif
		setInterface(INTERFACE_ABI_ANALYST);
		if(!m_pInterface) {
			return false;
		}
	} else if(ext == "yep" || ext == "baf" || inputFilepath.leaf() == "fid") {
#ifndef LIBMSR_HAS_BRUKER_COMPASSXPORT
		cerr << "Error: Bruker CompassXport interface is not supported, YEP/BAF/FID files cannot be read." << endl;
#endif
		setInterface(INTERFACE_BRUKER_COMPASSXPORT);
		if(!m_pInterface) {
			return false;
		}
	} else if(ext == "mgf") {
		setInterface(INTERFACE_TEXT_MGF);
	} else if(ext == "dta") {
		setInterface(INTERFACE_TEXT_DTA);
	} else {
		string type = to_lower_copy( GetXMLFileRootElement(inputFilename) );

		if(type == "mzdata")
			setInterface(INTERFACE_XML_MZDATA);
		else if(type == "mzxml")
			setInterface(INTERFACE_XML_MZXML);
		else if(type == "indexedmzml" || type == "mzml")
			setInterface(INTERFACE_XML_MZML);
		else
		{
			cerr << "Error: no interface supports file with extension '" << ext << "' and type '" << type << "'" << endl;
			return false;
		}
	}

	bool rv = m_pInterface->setInputFile(inputFilename, options);
	if(!rv)
		freeInterface();
	return rv;
}

ProcessingOptions InstrumentInterfaceImpl::getDefaultProcessingOptions(const string& formatExtension)
{
	if(formatExtension == "mgf") {
		return BaseInstrumentInterface::getDefaultProcessingOptions();
	} else if(formatExtension == "dta") {
		return BaseInstrumentInterface::getDefaultProcessingOptions();
	} else if(formatExtension == "mzdata") {
		return mzDataInterface::getDefaultProcessingOptions();
	} else if(formatExtension == "mzxml") {
		return mzXMLInterface::getDefaultProcessingOptions();
	//} else if(formatExtension == "mzml") {
	//	return mzMLInterface::getDefaultProcessingOptions();
	}
#ifdef LIBMSR_HAS_THERMO_XCALIBUR
	else if(formatExtension == "raw") {
		return ThermoInterface::getDefaultProcessingOptions();
	}
#endif
#ifdef LIBMSR_HAS_ABI_ANALYST
	else if(formatExtension == "wiff") {
		return AnalystInterface::getDefaultProcessingOptions();
	}
#endif
#ifdef LIBMSR_HAS_BRUKER_COMPASSXPORT
	else if(formatExtension == "yep" || formatExtension == "baf" || formatExtension == "fid") {
		return BrukerInterface::getDefaultProcessingOptions();
	}
#endif
	else {
		return BaseInstrumentInterface::getDefaultProcessingOptions();
	}
}
