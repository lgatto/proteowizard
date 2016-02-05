#define LIBMSR_SOURCE

#include "InstrumentInterface.h"
#include "InstrumentInterfaceImpl.h"

InstrumentInterface::InstrumentInterface()
{
	pimpl_ = new InstrumentInterfaceImpl();
}

InstrumentInterface::~InstrumentInterface()
{
	delete pimpl_;
}

BaseInstrumentInterface* InstrumentInterface::getBaseInterface() const
{
	return pimpl_->getBaseInterface();
}

bool InstrumentInterface::initInterface()
{
	return pimpl_->initInterface();
}

bool InstrumentInterface::setInputFile(const string& fileName, const ProcessingOptions& options)
{
	return pimpl_->setInputFile(fileName, options);
}

void InstrumentInterface::freeInterface()
{
	pimpl_->freeInterface();
}

ProcessingOptions InstrumentInterface::getDefaultProcessingOptions(const string& formatExtension)
{
	return InstrumentInterfaceImpl::getDefaultProcessingOptions(to_lower_copy(formatExtension));
}

void InstrumentInterface::setProcessingOptions(const ProcessingOptions& options)
{
	pimpl_->setProcessingOptions(options);
}

void InstrumentInterface::setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor)
{
	pimpl_->setCentroiding(centroid, tandemCentroid, preferVendor);
}

void InstrumentInterface::setDeisotoping(bool deisotope)
{
	pimpl_->setDeisotoping(deisotope);
}

void InstrumentInterface::setVerbose(bool verbose)
{
	pimpl_->setVerbose(verbose);
}

long InstrumentInterface::getFirstScanNumber()
{
	return pimpl_->getFirstScanNumber();
}

long InstrumentInterface::getLastScanNumber()
{
	return pimpl_->getLastScanNumber();
}

long InstrumentInterface::getTotalScanCount()
{
	return pimpl_->getTotalScanCount();
}

Scan* InstrumentInterface::getScan()
{
	return pimpl_->getScan();
}

Scan* InstrumentInterface::getScanHeader()
{
	return pimpl_->getScanHeader();
}

Scan* InstrumentInterface::getScan(long scanNumber)
{
	return pimpl_->getScan(scanNumber);
}

Scan* InstrumentInterface::getScanHeader(long scanNumber)
{
	return pimpl_->getScanHeader(scanNumber);
}
