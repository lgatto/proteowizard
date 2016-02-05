#ifndef _INCLUDED_MZDATAINTERFACE_H_
#define _INCLUDED_MZDATAINTERFACE_H_

#include "BaseXMLInterface.h"

typedef map< long, XML_Index > mzDataIndex;

class mzDataInterface : public BaseXMLInterface
{
public:
	using BaseXMLInterface::getScan;

	mzDataInterface()
		:	m_pCurScan(NULL),
			m_curBinaryArrayType(NO_BINARY_ARRAY),
			m_curIsDoublePrecision(false)
	{
		m_curBinaryEndianType = GetHostEndianType();
	}

	void freeInterface()
	{
		BaseXMLInterface::freeInterface();
		for(CvErrorMap::iterator itr = m_unknownCvCounts.begin(); itr != m_unknownCvCounts.end(); ++itr)
			cerr << "Warning: ignored " << itr->second.second << " instances of the unknown CV term: " << itr->second.first << endl;
	}

	bool setInputFile(const string& fileName, const ProcessingOptions& options);

private:
	enum BinaryArrayType { NO_BINARY_ARRAY, MZ_BINARY_ARRAY, INTEN_BINARY_ARRAY };

	Scan* m_pCurScan;
	string m_curSourceFileName;
	string m_curSourcePath;
	string m_curInstrumentName;
	string m_curName;
	string m_curVersion;

	BinaryArrayType m_curBinaryArrayType;
	string m_curPeakData;
	bool m_curIsDoublePrecision;
	HostEndianType m_curBinaryEndianType;

	typedef map< CVAccession, pair<string, int> > CvErrorMap;
	CvErrorMap m_unknownCvCounts;

	mzDataIndex m_scanIndex; // maps scan number to index entry
	mzDataIndex::const_iterator m_curScanIndexItr;
	void readIndex();
	void createIndex();

	bool parseHeader();
	bool parseNextScan(const XML_Index& scanByteOffset);

	Scan* getScan(long scanNumber, bool getPeaks);

	static bool parseScan(mzDataInterface*& pInstance, const char **atts, int attsCount);
	static bool parseCvParam(mzDataInterface*& pInstance, const CVAccession& id, const string& name, const string& value);

	static void HandleSourceFileName(void *userData, const XML_Char *s, int len);
	static void HandleSourcePath(void *userData, const XML_Char *s, int len);
	static void HandleInstrumentName(void *userData, const XML_Char *s, int len);
	static void HandleName(void *userData, const XML_Char *s, int len);
	static void HandleVersion(void *userData, const XML_Char *s, int len);
	static void HandlePeakData(void *userData, const XML_Char *s, int len);

	static void CreateIndexStartElement(void *userData, const char *name, const char **atts);
	static void CreateIndexEndElement(void *userData, const char *name);

	static void HeaderStartElement(void *userData, const char *name, const char **atts);
	static void HeaderEndElement(void *userData, const char *name);

	static void BodyStartElement(void *userData, const char *name, const char **atts);
	static void BodyEndElement(void *userData, const char *name);
};

#endif
