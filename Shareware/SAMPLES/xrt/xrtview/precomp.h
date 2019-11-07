// WOSA/XRT XRTView Sample Application Version 1.00.003
//
// Copyright (c) 1994 Microsoft Corp.  All Rights Reserved.
//
// Unicode support macros
//
//
#define STRICT
#ifdef _UNICODE
#include <tchar.h>
#else 
#define LPTSTR      LPSTR
#define LPTCSTR     LPCSTR
#define TEXT(a_)    a_
#endif

#ifdef _UNICODE
 #define TBSTR    WBSTR
#else
 #define  TBSTR   BSTR
#endif

#ifdef _UNICODE
 #define TSTRVAL wbstrVal
#else
 #define TSTRVAL bstrVal
#endif


#include <windows.h>
#include <windowsx.h>
#include <ole2.h>

#ifndef WIN32
#define EXPORT __export
#include <olenls.h>
#include <ver.h>
#include <compobj.h>
#endif


#include <variant.h>
//#include <ctl3d.h>
//#include <olestd.h>
