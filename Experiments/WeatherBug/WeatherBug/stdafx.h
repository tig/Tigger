// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

// TODO: reference additional headers your program requires here
#include <conio.h>
#include <WebServices.h>
#pragma comment(lib, "WebServices.lib")

#define ASSERT ATLASSERT
#define VERIFY ATLVERIFY

#define HR(expr) { hr = expr; if (FAILED(hr)) return hr; }

#include "api.wxbug.net.wsdl.h"
#include "wshelpers.h"