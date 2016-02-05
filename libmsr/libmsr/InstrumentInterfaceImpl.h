#ifndef _INCLUDED_INSTRUMENTINTERFACEIMPL_H_
#define _INCLUDED_INSTRUMENTINTERFACEIMPL_H_

#include "common.h"

#include "xml/mzXMLInterface.h"
#include "xml/mzDataInterface.h"
//#include "xml/mzMLInterface.h"

#include "text/MGFInterface.h"
#include "text/DTAInterface.h"

#ifdef LIBMSR_HAS_MFC

#ifdef LIBMSR_HAS_THERMO_XCALIBUR
#include "thermo/ThermoInterface.h"
#endif

#ifdef LIBMSR_HAS_ABI_ANALYST
#undef LIBMSR_HAS_ABI_ANALYST
#endif

#ifdef LIBMSR_HAS_ABI_ANALYST
#include "abi/AnalystImpl.h"
#include "abi/AnalystInterface.h"
#include "abi/AnalystQSInterface.h"
#endif

#ifdef LIBMSR_HAS_BRUKER_COMPASSXPORT
#include "bruker/BrukerInterface.h"
#endif

#endif

enum InterfaceType
{
	INTERFACE_XML_MZXML,
	INTERFACE_XML_MZDATA,
	INTERFACE_XML_MZML,
	INTERFACE_TEXT_MGF,
	INTERFACE_TEXT_DTA,
	INTERFACE_ABI_ANALYST,
	INTERFACE_THERMO_XCALIBUR,
	INTERFACE_BRUKER_COMPASSXPORT
};

class InstrumentInterfaceImpl
{
	void testInterface()
	{
		if( m_pInterface == NULL )
			throw runtime_error( "interface not initialized" );
	}

	void setInterface(const InterfaceType& type);

public:
	InstrumentInterfaceImpl()
		:	m_pInterface(NULL)
	{}

	~InstrumentInterfaceImpl()
	{
		freeInterface();
	}

	BaseInstrumentInterface* getBaseInterface() const
	{
		return m_pInterface;
	}

	bool initInterface();
	bool setInputFile(const string& fileName, const ProcessingOptions& options);

	void freeInterface()
	{
		if(m_pInterface) delete m_pInterface;
		m_pInterface = NULL;
	}

	void setProcessingOptions(const ProcessingOptions& options)
	{
		testInterface();
		m_pInterface->setProcessingOptions(options);
	}

	static ProcessingOptions getDefaultProcessingOptions(const string& formatExtension);

	void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor)
	{
		testInterface();
		m_pInterface->setCentroiding(centroid, tandemCentroid, preferVendor);
	}

	void setDeisotoping(bool deisotope)
	{
		testInterface();
		m_pInterface->setDeisotoping(deisotope);
	}

	void setVerbose(bool verbose)
	{
		testInterface();
		m_pInterface->setVerbose(verbose);
	}

	long getFirstScanNumber()
	{
		testInterface();
		return m_pInterface->firstScanNumber_;
	}

	long getLastScanNumber()
	{
		testInterface();
		return m_pInterface->lastScanNumber_;
	}

	long getTotalScanCount()
	{
		testInterface();
		return m_pInterface->totalNumScans_;
	}

	Scan* getScan()
	{
		testInterface();
		return m_pInterface->getScan();
	}

	Scan* getScanHeader()
	{
		testInterface();
		return m_pInterface->getScanHeader();
	}

	Scan* getScan(long scanNumber)
	{
		testInterface();
		return m_pInterface->getScan(scanNumber);
	}

	Scan* getScanHeader(long scanNumber)
	{
		testInterface();
		return m_pInterface->getScanHeader(scanNumber);
	}

private:
	friend class InstrumentInterface;

	// pointer to current interface
	BaseInstrumentInterface* m_pInterface;
};

#endif
