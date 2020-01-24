// testapp.cpp
//
// This is the WinMain module for the WOSA/XRT test app (Data User).
// The source code that makes up most of this app (i.e. the dataobject viewer)
// is shared between testapp.exe and xrttest.dll which is an Ole2View 
// Interface Viewer DLL.
//
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// August 7, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  August 30, 1993  cek     
//  October 29, 1993 cek Ported to Win32
//
#include "precomp.h"
//#include <initguid.h>
#include "xrttest.h"
#include "idataobj.h"
#include "debug.h"
ASSERTDATA

HINSTANCE   g_hInst ;
int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int nCmdShow)
{
    HRESULT hr ;
    LPCLASSFACTORY  pClassFactory ;
    LPUNKNOWN       pUnknown ;
    LPDATAOBJECT    pDataObject ; 
    
    g_hInst = hInst ;

    SetMessageQueue( 96 ) ;
    
    if (FAILED(OleInitialize(NULL)))
    {
		MessageBox( HWND_DESKTOP, "OleInitialize() failed.", "Aborting", MB_OK ) ;
        return 0  ;
    }

    char szObjName[128] ;    
    if (pszCmdLine && *pszCmdLine)
    {
        lstrcpy( szObjName, pszCmdLine ) ;
    }
    else
        lstrcpy( szObjName, "StocksRUs.Stocks.1" ) ;

    // Get CLSID from string                     
    CLSID   clsid ;
    if (FAILED(hr = CLSIDFromString( szObjName, &clsid )))
    {
        char szBuf[256] ; wsprintf( szBuf, "CLISDFromString( %s ) failed!\r\n", (LPSTR)szObjName ) ;
        AssertSz(0, szBuf ) ;

		wsprintf( szBuf, "The class object '%s' is not registered properly.  TestApp cannot continue.", (LPSTR)szObjName ) ;
		MessageBox( HWND_DESKTOP, szBuf, "Aborting", MB_OK ) ;

        OleUninitialize() ;
        return 0 ;
    }

    hr = CoGetClassObject( clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory,
                           (LPVOID FAR *)&pClassFactory ) ;
                 
    if (SUCCEEDED(hr))         
    { 
        // Create an instance of the object (Instance) and store
        // it in m_pIUnknown
        //
        hr = pClassFactory->CreateInstance( NULL, IID_IUnknown, 
                            (LPVOID FAR *)&pUnknown ) ;
        if (SUCCEEDED(hr))         
        {
            hr = pUnknown->QueryInterface( IID_IDataObject, (LPVOID FAR*)&pDataObject ) ;
            pUnknown->Release() ;     
            
            if (SUCCEEDED(hr))
            {
                DisplayIDataObject( HWND_DESKTOP, pDataObject, (LPIID)&IID_IDataObject, "WOSA/XRT Data Object" ) ;
                pDataObject->Release() ;
            }
            else
            {
                AssertSz(0, "QueryInterface failed") ;
				MessageBox( HWND_DESKTOP, "QueryInterface on the class factory object failed.", "Aborting", MB_OK ) ;
            }

        }
        else
        {
            AssertSz(0, "CreateInstance") ;
            MessageBox( HWND_DESKTOP, "CreateInstance failed.", "Aborting", MB_OK ) ;
        }                              
        
        pClassFactory->Release() ;
    }
    else
    {
        AssertSz(0, "GetClassObject failed") ;
        MessageBox( HWND_DESKTOP, "GetClassObject failed.", "Aborting", MB_OK ) ;
    }
    
    OleUninitialize() ;
    
    return 0 ;
}

