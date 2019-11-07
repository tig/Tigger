#ifdef WIN32

#define _UNICODE
#define UNICODE
#include <windows.h>
#include <tchar.h>
#include <wchar.h>
 
#define EXPORT 
 
#else

// For Win16 there is no Unicode.  These defines make
// everthing ANSI
//
#define LPTSTR  LPSTR
#define LPCTSTR LPCSTR
#define TEXT(a) a

#define EXPORT __export

#include <windows.h>

#endif

#include <windowsx.h> 
#include <memory.h>
#include <compobj.h>
#include <ole2.h>

#ifndef WIN32

#include <ver.h>
#include <olenls.h>   
#include <variant.h>

#else

#include <winver.h>

#endif

