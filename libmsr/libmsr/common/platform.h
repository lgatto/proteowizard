#ifndef _INCLUDED_PLATFORM_H_
#define _INCLUDED_PLATFORM_H_

#ifdef WIN32

// use std min/max instead of Win32 macros
#define NOMINMAX

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define SYS_PATH_DELIMITER "\\/"		// Windows pretty much works with both.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#ifndef _WINDOWS_ // don't include windows.h twice

#include <tchar.h>
#if defined(_AFXDLL) || defined(_ATL_STATIC_REGISTRY)
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif

#if defined(_ATL_DLL) || defined(_ATL_STATIC_REGISTRY)
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>
#endif

#else // No AFX
#include <windows.h>
#endif

#endif // windows.h already included

#else // Not Win32

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#ifndef __CYGWIN__
#include <sys/sysinfo.h>
#endif
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <glob.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#ifndef MAX_PATH
#define MAX_PATH 255
#endif
#define SYS_PATH_DELIMITER "/"

#endif

#endif
