#ifndef _INCLUDED_MZXMLINTERFACE_H_
#define _INCLUDED_MZXMLINTERFACE_H_

#include "BaseXMLInterface.h"

typedef pair< XML_Index, size_t > mzXMLIndexEntry;
typedef map< long, mzXMLIndexEntry > mzXMLIndex;

class mzXMLInterface : public BaseXMLInterface
{
public:
	using BaseXMLInterface::getScan;

	mzXMLInterface()
		:	m_pCurScan(NULL), m_curBinaryCompressionType(BINARY_COMPRESSION_NONE),
			m_allScansCentroided(false), m_allScansDeisotoped(false)
	{}

	bool setInputFile(const string& fileName, const ProcessingOptions& options);

private:

	Scan* m_pCurScan;
	string m_curPrecursorMz;
	string m_curPeakData;
	BinaryCompressionType m_curBinaryCompressionType;
	bool m_curIsDoublePrecision;
	double m_curCollisionEnergy;
	bool m_allScansCentroided;
	bool m_allScansDeisotoped;
	string m_curInstrumentId;
	map<string, MSAnalyzerType> m_instrumentIdToAnalyzer;

	mzXMLIndex m_scanIndex; // maps scan number to index entry
	mzXMLIndex::iterator m_curScanIndexItr;
	void readIndex();
	void createIndex();

	bool parseHeader();
	bool parseCurScan();

	Scan* getScan(long scanNumber, bool getPeaks);

	static bool parseScan(mzXMLInterface*& pInstance, const char **atts, int attsCount);
	static void HandlePrecursorMz(void *userData, const XML_Char *s, int len);
	static void HandlePeakData(void *userData, const XML_Char *s, int len);

	static void CreateIndexStartElement(void *userData, const char *name, const char **atts);
	static void CreateIndexEndElement(void *userData, const char *name);

	static void HeaderStartElement(void *userData, const char *name, const char **atts);
	static void HeaderEndElement(void *userData, const char *name);

	static void BodyStartElement(void *userData, const char *name, const char **atts);
	static void BodyEndElement(void *userData, const char *name);
};

#endif
