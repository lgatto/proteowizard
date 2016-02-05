#ifndef _INCLUDED_XMLINTERFACE_H_
#define _INCLUDED_XMLINTERFACE_H_

#define XML_LARGE_SIZE // support large files (>2^31)
#include <expat.h>

#ifndef HAS_ATTR
#define HAS_ATTR(name) (getAttrIndex(name,atts,attsCount) > -1)
#endif

#ifndef GET_ATTR
#define GET_ATTR(name) atts[getAttrIndex(name,atts,attsCount)+1]
#endif

#include "common.h"
#include "BaseInstrumentInterface.h"

class BaseXMLInterface : public BaseInstrumentInterface
{
protected:
	XML_Parser	m_pParser;
	bool		m_curGetPeaks; // determines whether to read and decode base64
	bool		m_isSuspended;
	XML_Bool	m_isDone;
	string		m_xmlDecl;
	string		m_xmlEncoding;

	string		m_inputFilename;
	ifstream	m_inputFile;

	static int	getAttrIndex(const string& attrName, const char** atts, int attsCount);
	XML_Status	ResumeParserFlush();
	static void DocumentXmlDeclHandler(void* userData, const char* version, const char* encoding, int standalone);

	virtual Scan* getScan(long scanNumber, bool getPeaks) = 0;

public:
	BaseXMLInterface() : m_pParser(NULL)
	{
	}

	virtual ~BaseXMLInterface()
	{
		freeInterface();
	}

	virtual bool initInterface();
	virtual bool setInputFile(const string& fileName, const ProcessingOptions& options) = 0;

	virtual void freeInterface()
	{
		if(m_pParser)
		{
			XML_ParserFree(m_pParser);
			m_pParser = NULL;
		}
	}

	virtual void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor) {}
	virtual void setDeisotoping(bool deisotope) {}

	virtual void setVerbose(bool verbose)
	{
		verbose_ = verbose;
	}

	Scan* getScan()							{ return getScan(0, true); }
	Scan* getScanHeader()					{ return getScan(0, false); }

	Scan* getScan(long scanNumber)			{ return getScan(scanNumber, true); }
	Scan* getScanHeader(long scanNumber)	{ return getScan(scanNumber, false); }

	virtual void setDefaultScanInfo(Scan* scan)
	{
		if(scan->ionization_ == IONIZATION_UNDEF && instrumentInfo_.ionSource_ != IONIZATION_UNDEF)
			scan->ionization_ = instrumentInfo_.ionSource_;
		if(scan->analyzer_ == ANALYZER_UNDEF && !instrumentInfo_.analyzerList_.empty())
			scan->analyzer_ = instrumentInfo_.analyzerList_.back();
		if(scan->polarity_ == POLARITY_UNDEF)
			scan->polarity_ = ANY;
		if(scan->scanType_ == SCAN_UNDEF)
			scan->scanType_ = FULL;
		if(scan->msLevel_ > 1 && scan->activation_ == ACTIVATION_UNDEF)
			scan->activation_ = CID;
	}
};

enum BinaryCompressionType
{
	BINARY_COMPRESSION_UNDEF = 0,
	BINARY_COMPRESSION_ZLIB,
	BINARY_COMPRESSION_NONE,
	NUM_BINARY_COMPRESSION_TYPES
};

inline BinaryCompressionType toBinaryCompressionType(const string& value)
{
	string tmp = value;
	to_lower(tmp);
	if(tmp == "zlib")
		return BINARY_COMPRESSION_ZLIB;
	else if(tmp == "none")
		return BINARY_COMPRESSION_NONE;
	else
		return BINARY_COMPRESSION_UNDEF;
}

inline bool toBool(const string& value)
{
		string tmp = value;
		to_lower(tmp);
		if(tmp == "true" || tmp == "1")
			return true;
		else if(tmp == "false" || tmp == "0")
			return false;
		else
			throw std::invalid_argument("cannot parse XML boolean from value '" + value + "'");
}

class XMLIsDoublePrecision
{
	bool m_value;

public:
	XMLIsDoublePrecision(const string& value)
	{
		if(value == "32")
			m_value = false;
		else if(value == "64")
			m_value = true;
		else
			throw std::invalid_argument("cannot parse XML precision from value '" + value + "'");
	}

	operator bool() { return m_value; }
};

class XMLEndianType
{
	HostEndianType m_value;

public:
	XMLEndianType(const string& value)
	{
		string tmp = value;
		to_lower(tmp);
		if(tmp == "network" || tmp == "big")
			m_value = COMMON_BIG_ENDIAN;
		else if(tmp == "little")
			m_value = COMMON_LITTLE_ENDIAN;
		else
			throw std::invalid_argument("cannot parse XML endian type from value '" + value + "'");
	}

	operator HostEndianType() { return m_value; }
};

#endif
