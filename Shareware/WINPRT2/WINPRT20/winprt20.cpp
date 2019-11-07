// winprt20.cpp : Implementation of CWinprt20App and DLL registration.
//
// WinPrint 2.0 has been assigned the following GUIDs by Microsoft
//
// Available range: 0002A5xx-0000-0000-C000-000000000046
//
// Component                            GUID
//  CWinPrintControl                    0002A501-0000-0000-C000-000000000046
//  _tlid (Typelibrary)                 0002A502-0000-0000-C000-000000000046
//  IID_IWinPrintControl                0002A503-0000-0000-C000-000000000046
//  IID_IWinPrintControlEvents          0002A504-0000-0000-C000-000000000046
//  CControlPropPage                    0002A505-0000-0000-C000-000000000046
//  CWinPrintEngine                     0002A506-0000-0000-C000-000000000046
//  IID_IWinPrintEngine                 0002A507-0000-0000-C000-000000000046
//  IID_IWinPrintEngineEvents           0002A508-0000-0000-C000-000000000046
//  CEnginePropPage                     0002A509-0000-0000-C000-000000000046
//  IWinPrintHeader                     0002A50A-0000-0000-C000-000000000046
//  CHeader                             0002A50B-0000-0000-C000-000000000046
//  IWinPrintHeaderSection              0002A50C-0000-0000-C000-000000000046
//  CHeaderSection                      0002A50D-0000-0000-C000-000000000046
//
//  WinPrint 2.0 Primary UI             0002A5F0-0000-0000-C000-000000000046
#include "stdafx.h"
#include "winprt20.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CWinprt20App g_WinPrint;

const GUID CDECL BASED_CODE _tlid =
        { 0x0002A502, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x46 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CWinprt20App::InitInstance - DLL initialization

BOOL CWinprt20App::InitInstance()
{
    BOOL bInit = COleControlModule::InitInstance();
    
    if (bInit)
    {
        m_strConfigFileName.LoadString(IDS_CONFIGFILENAME) ;
    }

    return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CWinprt20App::ExitInstance - DLL termination

int CWinprt20App::ExitInstance()
{
    // TODO: Add your own module termination code here.
    
    return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    AfxOleUnregisterTypeLib(_tlid);
    AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid,
        _wVerMajor, _wVerMinor, "WINPRT20");
    COleObjectFactoryEx::UpdateRegistryAll(TRUE);
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    AfxOleUnregisterTypeLib(_tlid);
    COleObjectFactoryEx::UpdateRegistryAll(FALSE);
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// WinPrint 2.0 Helper APIs
//


/////////////////////////////////////////////////////////////////////////////
// wpCreateDevNames retuns an HGLOBAL containing a DEVNAMES structure
// correctly filled out.
//
HGLOBAL wpCreateDevNames( LPCSTR szPrinter, LPCSTR szPrinterDriver, LPCSTR szPrinterOutput )
{
    ASSERT(szPrinter) ;
    ASSERT(*szPrinter) ;
    ASSERT(szPrinterDriver) ;
    ASSERT(*szPrinterDriver) ;
    ASSERT(szPrinterOutput) ;
    ASSERT(*szPrinterOutput) ;
    
    UINT w1 = lstrlen(szPrinterDriver) + 1;
    UINT w2 = lstrlen(szPrinter) + 1 ;
    UINT w3 = lstrlen(szPrinterOutput) + 1;

    HGLOBAL hDevNames = GlobalAlloc( GHND, sizeof(DEVNAMES) + w1 + w2 + w3 ) ;
    if (hDevNames != NULL)
    {
        DEVNAMES FAR* lpdn = (DEVNAMES FAR*)GlobalLock( hDevNames ) ;
        if (lpdn != NULL )
        {
            lpdn->wDriverOffset = sizeof( DEVNAMES ) ;
            _fmemcpy( (LPSTR)lpdn + lpdn->wDriverOffset, szPrinterDriver, w1 ) ;
                
            lpdn->wDeviceOffset = sizeof( DEVNAMES ) + w1 ;
            _fmemcpy( (LPSTR)lpdn + lpdn->wDeviceOffset, szPrinter, w2 ) ;
                
            lpdn->wOutputOffset = sizeof( DEVNAMES ) + w1 + w2 ;
            _fmemcpy( (LPSTR)lpdn + lpdn->wOutputOffset, szPrinterOutput, w3 ) ;
              
            lpdn->wDefault = 0 ;
                  
            GlobalUnlock( hDevNames ) ;
            lpdn = NULL ;
        }
        else
        {
            ASSERT(lpdn) ;
            GlobalFree( hDevNames ) ;
            hDevNames = NULL ;
        }
    }

    ASSERT(hDevNames) ;
    return hDevNames ;
}

HGLOBAL wpCreateDevMode(LPCSTR szPrinter, LPCSTR szPrinterDriver, LPCSTR szPrinterOutput )
{
    LPFNDEVMODE     lpfnExtDeviceMode ;
    HMODULE         hDriver ;
    char            szDriver[_MAX_PATH] ;
    LPDEVMODE       lpDevMode = NULL ;

    wsprintf( szDriver, "%s.DRV", (LPCSTR)szPrinterDriver ) ;
   
    if (HINSTANCE_ERROR >= (hDriver = LoadLibrary( szDriver )))
    {
        TRACE( "wpCreateDevMode : Could not load driver : %s\n", (LPCSTR)szDriver ) ;
        return NULL ;
    }

    lpfnExtDeviceMode = (LPFNDEVMODE)GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;

    if (!lpfnExtDeviceMode)
    {
        TRACE( "wpCreateDevMode : Driver does not support ExtDeviceMode\n") ;
        FreeLibrary( hDriver ) ;
        return NULL ;
    }
    else
    {
        int n ;
        /*
         * Call ExtDeviceMode to get size of DEVMODE structure
         */
        n = (*lpfnExtDeviceMode)( NULL, hDriver,
                                   (LPDEVMODE)NULL,
                                   (LPSTR)szPrinter,
                                   (LPSTR)szPrinterOutput,
                                   (LPDEVMODE)NULL,
                                   (LPSTR)NULL,
                                   0 ) ;
        
        if (n <= 0)
        {
            TRACE( "wpCreateDevMode : First call to ExtDeviceMode bad! n = %d\n", n ) ;
            FreeLibrary( hDriver ) ;
            return NULL ;
        }

        lpDevMode = (LPDEVMODE)GlobalAllocPtr( GHND, n ) ;
        ASSERT(lpDevMode) ;
        
        n = (*lpfnExtDeviceMode)( NULL, hDriver,
                                   (LPDEVMODE)lpDevMode,
                                   (LPSTR)szPrinter,
                                   (LPSTR)szPrinterOutput,
                                   (LPDEVMODE)lpDevMode,
                                   (LPSTR)NULL,
                                   DM_COPY ) ;

        FreeLibrary( hDriver ) ;

        if (n != IDOK)
        {
            TRACE( "wpCreateDevMode : Second call to ExtDeviceMode bad! n = %d\n", n ) ;
            GlobalFreePtr( lpDevMode ) ;
            return NULL ;
        }
   }

   return GlobalPtrHandle(lpDevMode) ;

} /* engPrintwpCreateDevMode()  */
