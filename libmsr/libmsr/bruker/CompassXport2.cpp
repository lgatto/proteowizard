#include "CompassXport2.h"

HRESULT ICompassXport2::ConvertAndLog(BSTR inputFileName, BSTR outputFileName, long logLevel)
{
	HRESULT rv;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, &rv, parms, inputFileName, outputFileName, logLevel);
	return rv;
}

HRESULT ICompassXport2::ConvertMultipleAndLog(BSTR inputRootPathName, BSTR outputFileName, long logLevel)
{
	HRESULT rv;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, &rv, parms, inputRootPathName, outputFileName, logLevel);
	return rv;
}

HRESULT ICompassXport2::ConvertAndLog2(BSTR inputFileName, BSTR outputFileName, long logLevel, VARIANT_BOOL convertRaw)
{
	HRESULT rv;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BOOL;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, &rv, parms, inputFileName, outputFileName, logLevel, convertRaw);
	return rv;
}

HRESULT ICompassXport2::ConvertMultipleAndLog2(BSTR inputRootPathName, BSTR outputFileName, long logLevel, VARIANT_BOOL convertRaw)
{
	HRESULT rv;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BOOL;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, &rv, parms, inputRootPathName, outputFileName, logLevel, convertRaw);
	return rv;
}
