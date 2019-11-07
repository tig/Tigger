// winprt20.h : main header file for WINPRT20.DLL

#ifndef _INC_WINDOWS
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinprt20App : See winprt20.cpp for implementation.

class CWinprt20App : public COleControlModule
{
public:
    BOOL InitInstance();
    int ExitInstance();
    
    CString m_strConfigFileName ;
};

extern const GUID CDECL BASED_CODE _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
extern CWinprt20App g_WinPrint;

#include "cprop.h"

HGLOBAL wpCreateDevNames( LPCSTR szPrinter, LPCSTR szPrinterDriver, LPCSTR szPrinterOutput ) ;
HGLOBAL wpCreateDevMode(LPCSTR szPrinter, LPCSTR szPrinterDriver, LPCSTR szPrinterOutput ) ;
