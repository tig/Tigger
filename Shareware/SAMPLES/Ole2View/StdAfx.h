// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

//#define VC_EXTRALEAN

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions (including VB)
#include <afxole.h>      
#include <afxcmn.h>
#include <afxcview.h>
#include <afxpriv.h>
#include <afxmt.h>          // MFC Multi-Threading support
#include <ole2ver.h>
#include <winreg.h>

// COM Component Categoories Manager
#include <comcat.h>

#ifdef _MAC
#ifdef RegOpenKeyEx
#undef RegOpenKeyEx
#endif
#define RegOpenKeyEx(hKey,lpSubKey,ulOptions,samDesired,phkResult) RegOpenKey(hKey,lpSubKey,phkResult)
#endif

#pragma warning(disable : 4100)

