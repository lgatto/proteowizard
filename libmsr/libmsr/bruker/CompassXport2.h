#if !defined(_INCLUDED_COMPASSXPORT2_H_)
#define _INCLUDED_COMPASSXPORT2_H_

#include "common.h"

// ICompassXport2 wrapper class
class ICompassXport2 : public COleDispatchDriver
{
public:
	ICompassXport2() {}		// Calls COleDispatchDriver default constructor
	ICompassXport2(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ICompassXport2(const ICompassXport2& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	HRESULT ConvertAndLog(BSTR inputFileName, BSTR outputFileName, long logLevel);
	HRESULT ConvertMultipleAndLog(BSTR inputRootPathName, BSTR outputFileName, long logLevel);
	HRESULT ConvertAndLog2(BSTR inputFileName, BSTR outputFileName, long logLevel, VARIANT_BOOL convertRaw);
	HRESULT ConvertMultipleAndLog2(BSTR inputRootPathName, BSTR outputFileName, long logLevel, VARIANT_BOOL convertRaw);
};

#endif // _INCLUDED_COMPASSXPORT2_H_
