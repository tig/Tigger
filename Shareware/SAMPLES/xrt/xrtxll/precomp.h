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

#define lstrlenA lstrlen

#include <windows.h>

#endif

#include <windowsx.h>
#include <stdlib.h>

#ifdef WIN32
#include <g:\xl4nt\xlcall\xlcall.h>
#else
#include <xlcall.h>
#endif

#pragma pack(push,id,8)
#include <ole2.h>
#include <compobj.h>
#pragma pack(pop,id,8)

#include <ctype.h>
#include <ddeml.h>
