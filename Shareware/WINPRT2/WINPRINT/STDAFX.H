// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef STRICT
#define STRICT
#endif

// Disable "unrefereced formal parameter"
#pragma warning( disable : 4100 )

// Disable "Note: Creating new precompiled header" 
#pragma warning( disable : 4699 )

// Disable "4706: assignment within conditional expression"
#pragma warning( disable : 4706 )

#if _MSC_VER == 800
// Disable "C4702" "unreachable code" (only happens with 
// C8.
#pragma warning( disable : 4702 )
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions (including VB)
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes

#include <ctl3d.h>

#ifndef WIN32
// For Win16 there is no Unicode.  These defines make
// everthing ANSI
//
#define PTSTR   PSTR
#define LPTSTR  LPSTR
#define LPCTSTR LPCSTR
#define _T(a) a
#define _T(a) a

#endif
