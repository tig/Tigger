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

#define USE_COGETCLASSOBJECT

HINSTANCE   g_hInst ;
int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int nCmdShow)
{
    HRESULT hr ;
    LPCLASSFACTORY  pClassFactory = NULL ;
    LPUNKNOWN       pUnknown = NULL ;
    LPDATAOBJECT    pDataObject = NULL ; 
    
    g_hInst = hInst ;

    SetMessageQueue( 96 ) ;
    
    if (FAILED(OleInitialize(NULL)))
    {
        MessageBox( HWND_DESKTOP, TEXT( "OleInitialize() failed." ), TEXT( "Aborting" ), MB_OK ) ;
        return 0  ;
    }

    TCHAR szObjName[128] ;    
    if (pszCmdLine && *pszCmdLine)
    {
        #ifdef WIN32
        LPTSTR p = GetCommandLine() ;
        while (*p && (*p != TEXT(' ')))
            p++ ;

        while (*p == TEXT(' '))
            p++ ;

        lstrcpy( szObjName, p ) ;
        #else
        lstrcpy( szObjName, pszCmdLine ) ;
        #endif

    }
    else
    #ifdef USE_COGETCLASSOBJECT
        lstrcpy( szObjName, TEXT( "StocksRUs.Stocks.1" ) ) ;
    #else
        lstrcpy( szObjName, TEXT( "E:\\SOURCE\\XRT\\XRTOBJ.XRT" ) ) ;        // use a filename moniker
    #endif

    // Try to get CLSID from string                     
    // This looks in the registry.
    //
    CLSID   clsid ;
    if (SUCCEEDED(hr = CLSIDFromProgID( szObjName, &clsid )))
    {
        // Looks like we were given a ProgID so let's use CoGetClassObject
        //
        hr = CoGetClassObject( clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory,
                               (LPVOID FAR *)&pClassFactory ) ;
                 
        if (SUCCEEDED(hr))         
        { 
            // Create an instance of the object (Instance) and store
            // it in m_pIUnknown
            //
            hr = pClassFactory->CreateInstance( NULL, IID_IUnknown, 
                                (LPVOID FAR *)&pUnknown ) ;
            if (FAILED(hr))         
            {
                TCHAR szBuf[256] ; 
                wsprintf( szBuf, TEXT( "pClassFactory->CreateInstance() failed!  %s\r\n" ), 
                                 (LPTSTR)HRtoString(hr) ) ;
                AssertSz(0, szBuf ) ;
                MessageBox( HWND_DESKTOP, szBuf, TEXT( "Aborting" ), MB_OK ) ;
                pClassFactory->Release() ;
                OleUninitialize() ;
                return 0 ;
            }                              

            hr = pUnknown->QueryInterface( IID_IDataObject, 
                                           (LPVOID FAR*)&pDataObject ) ;
            pUnknown->Release() ;     
            pClassFactory->Release() ;
        }
        else
        {
            AssertSz(0, TEXT( "GetClassObject failed" )) ;
            MessageBox( HWND_DESKTOP, TEXT( "GetClassObject failed." ), TEXT( "Aborting" ), MB_OK ) ;
        }
        
    }
    else
    {
        // GetCLSIDFromString failed, so we should have a filename.
        // Try using a file moniker
        LPMONIKER   lpmk = NULL ;

        hr = CreateFileMoniker( szObjName, &lpmk ) ;
        if (FAILED(hr))
        {
            TCHAR szBuf[256] ; 
            wsprintf( szBuf, TEXT( "CreateFileMoniker( %s ) failed!  %s\r\n" ), 
                                (LPTSTR)szObjName, (LPTSTR)HRtoString(hr) ) ;
            AssertSz(0, szBuf ) ;

            wsprintf( szBuf, TEXT( "The class object '%s' is not registered properly.  TestApp cannot continue." ), 
                            (LPTSTR)szObjName ) ;
            MessageBox( HWND_DESKTOP, szBuf, TEXT( "Aborting" ), MB_OK ) ;

            OleUninitialize() ;
            return 0 ;
        }

        hr = BindMoniker( lpmk, 0, IID_IDataObject, (LPVOID FAR*)&pDataObject ) ;
        if (FAILED(hr))
        {
            TCHAR szBuf[256] ; 
            wsprintf( szBuf, TEXT( "BindMoniker failed!  %s\r\n" ), 
                                (LPTSTR)HRtoString(hr) ) ;
            AssertSz(0, szBuf ) ;
            MessageBox( HWND_DESKTOP, szBuf, TEXT( "Aborting" ), MB_OK ) ;

            OleUninitialize() ;
            return 0 ;
        }

    }

    if (SUCCEEDED(hr) && pDataObject != NULL)
    {
        DisplayIDataObject( HWND_DESKTOP, pDataObject, 
                            (LPIID)&IID_IDataObject, 
                            TEXT( "WOSA/XRT Data Object" ) ) ;
        pDataObject->Release() ;
    }

    OleUninitialize() ;
    
    return 0 ;
}

