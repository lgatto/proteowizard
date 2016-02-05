#define LIBMSR_SOURCE
#include "BaseXMLInterface.h"

int BaseXMLInterface::getAttrIndex(const string& attrName, const char** atts, int attsCount)
{
	for(int i=0; i < attsCount; ++i)
	{
		if(!strcmp( atts[i], attrName.c_str()))
			return i;
	}
	return -1;
}

bool BaseXMLInterface::initInterface()
{
	// free previously initialized Expat parser
	freeInterface();

	// initialize new Expat parser
	m_pParser = XML_ParserCreate(NULL);

	// static member functions get the class instance handle from Expat's UserData
	XML_SetUserData(m_pParser, this);

	return true;
}

XML_Status BaseXMLInterface::ResumeParserFlush()
{
	XML_SetElementHandler(m_pParser, NULL, NULL);
	XML_SetCharacterDataHandler(m_pParser, NULL);
	return XML_ResumeParser(m_pParser);
}

void BaseXMLInterface::DocumentXmlDeclHandler( void* userData, const char* version, const char* encoding, int standalone )
{
	BaseXMLInterface* pInstance = static_cast<BaseXMLInterface*>(userData);

	const static char xmlDeclToken1[] = "<?xml version=\"";
	const static char xmlDeclToken2[] = "\" encoding=\"";
	const static char xmlDeclToken3[] = "\"?>";

	if(encoding)
		pInstance->m_xmlEncoding = to_lower_copy(string(encoding));
	else
		pInstance->m_xmlEncoding = "us-ascii";

	bool isUTF16 = pInstance->m_xmlEncoding == "utf-16";
	string& xmlDecl = pInstance->m_xmlDecl;
	xmlDecl.clear();
	BOOST_FOREACH(char c, xmlDeclToken1)
	{
		xmlDecl.push_back(c);
		if(isUTF16) xmlDecl.push_back('\0');
	}
	BOOST_FOREACH(char c, version)
	{
		xmlDecl.push_back(c);
		if(isUTF16) xmlDecl.push_back('\0');
	}
	BOOST_FOREACH(char c, xmlDeclToken2)
	{
		xmlDecl.push_back(c);
		if(isUTF16) xmlDecl.push_back('\0');
	}
	BOOST_FOREACH(char c, encoding)
	{
		xmlDecl.push_back(c);
		if(isUTF16) xmlDecl.push_back('\0');
	}
	BOOST_FOREACH(char c, xmlDeclToken3)
	{
		xmlDecl.push_back(c);
		if(isUTF16) xmlDecl.push_back('\0');
	}
}
