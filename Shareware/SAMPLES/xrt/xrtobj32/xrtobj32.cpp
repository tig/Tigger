// xrtobj32.cpp
//
// This is the WinMain module for a WOSA/XRT compliant Data Object.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// October 29, 1993
//
// Internet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  October 29, 1993 cek Created
//
// -------------------------------------------------------------------------
//
// Notes:
// 
// This application implements a very simple COM object that supports
// the basic interfaces required for Uniform Data Transfer (i.e.
// IDataObject, IEnumFORMATETC).   
// 
// If this application is started with /Embedding on the command line 
// then the application will exit once all refcounts on interfaces go
// to zero.  If the application is started without /Embedding, user action
// is required to shut the program down (i.e. ALT-F4).
// 
// The structure of this program is as thus:
// 
// There is a main window ("fnMainWnd") which contains several child windows:
// 
//     A listbox showing the current data set
//     A box that shows status information such as the refcounts on the
//         various interfaces
//     A menu with File.Exit, Options.Update Speed..., 
//         Options.Market Simulation, and Help.About...
//
//  TODO:  Dialog boxes not implemented yet.
// 
// When the main window is created a timer is started (WM_TIMER style) using
// an update rate as specified in XRTOBJ32.INI.   This update rate can
// be changed using the Options.Update Speed... menu command (which pops up 
// a dialog and saves the chages to XRTOBJ32.INI).
// 
// At startup, a list of instruments (Ticker Symbol, Last Price, and 
// Last Volume) is read from XRTOBJ32.INI into an array of XRTSTOCK 
// structures.
// 
// On each WM_TIMER message each XRTSTOCK element is randomly changed.
// The 'randomizer' code is only semi-random.  Variables specify 
// whether the stock prices should increase or decrease (allowing
// simulation of an increasing or decreasing market).
// 
// The XRTSTOCK structure contains a member that allows code to determine
// whether informaiton in the structure has changed since the last look.
// 
// After the WM_TIMER code has finished updating the list of 
// stocks, it notifies the IDataObject implementation 
// essentially saying "Hey!  Data has changed!".   The IDataObject
// implementation then attempts to call the OnDataChange member
// of an IAdviseSink pointer it should have gotten via IDataObject::
// DAdvise.   The IDataObject code fills a memory buffer with
// data and calls IAdviseSink::OnDataChange() with that data.
// Depending on which clipboard format was specified the 
// data either represents a snapshot of the entire data set
// (CF_TEXT) or the set of data points that changed since
// the last OnDataChange was sent (CF_XRTMARKETDATA).
// 
// The IDataObject code resets the member in the XRTSTOCK array
// that indicates changed data before it returns.
// 
// So in effect what this application does is provide a simulation
// of a stock market data feed.   The receiving end (IAdviseSink) 
// can look at the data that comes across and treat each stock item
// as an individual 'trade'.
// 
// 
#include "precomp.h"
#include "resource.h"
#include "clsid.h"
#include "xrtobj32.h"
#include "object.h"
#include "simobj.h"

#include "debug.h"

// Forward function prototypes
BOOL InitApplication( HINSTANCE hInst ) ;
BOOL WINAPI DoAboutBox( VOID ) ;

// Used by the Assert macros in DEBUG.H and the code in
// the debug OLE2.DLL
ASSERTDATA

// Standard app globals
HINSTANCE   g_hInst = NULL ;
HWND        g_hwndMain = NULL ;
LPTSTR      g_pszCmdLine = NULL ;

// Class factory related globals
LPCLASSFACTORY  g_pIClassFactory = NULL ;
DWORD           g_cLock = 0L ;              // Lock count
DWORD           g_cObj = 0L ;               // Number of alive objects
DWORD           g_dwRegCO = 0L ;

// Simulation globals
CSimulation*    g_pSimObj = NULL ;
#define MARKET_TIMEOUT      250
#define UPDATE_TIMEOUT      500
UINT            g_uiTimeoutMarket = MARKET_TIMEOUT ;
UINT            g_uiTimeoutUpdate = UPDATE_TIMEOUT ;

// Simple minded way of keeping track of the objects we've
// created.  The following is an array of object pointers that
// is initallialy set to all NULLs.  Each time we create an
// object we store it's pointer in this array; when it's deleted
// we search for it's pointer and NULL it out
//
// Note in future versions this will be replaced with a listbox
// that will show visually the objects that have been created
//
#define MAX_OBJECTS     16
CStocksObject*  g_rgpObj[MAX_OBJECTS] ;

// Applicaiton WinMain
//
// Standard WinMain stuff.  InitApplication, register classes and with
// OLE, create window, message loop...
//
int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR p, int nCmdShow)
{
    g_hInst = hInst ;
#ifdef WIN32
    g_pszCmdLine = GetCommandLine() ;
#else
    g_pszCmdLine = pszCmdLine ;
#endif

    // Gotta do this for OLE
    SetMessageQueue( 96 ) ;

    // Initialize Application (register wnd classes etc...)
    if (!InitApplication( hInst ))
    {
        AssertSz( 0, L"InitApplication failed\r\n" ) ;
        return 0 ;
    }
    
    if (FAILED(OleInitialize(NULL)))
    {
        AssertSz(0, L"OleInitialize failed\r\n" ) ;
        return 0  ;
    }

    // Read our settings from the INI file
    //
    g_uiTimeoutMarket = GetPrivateProfileInt( L"Simulation", L"MarketTimeout", MARKET_TIMEOUT, L"XRTOBJ32.INI" ) ;
    g_uiTimeoutUpdate = GetPrivateProfileInt( L"Simulation", L"UpdateTimeout", UPDATE_TIMEOUT, L"XRTOBJ32.INI" ) ;

    // We save the window position on shutdown.  Read it here.
    //
    int    x = GetPrivateProfileInt( L"Config", L"x", CW_USEDEFAULT, L"XRTOBJ32.INI" ) ;
    int    y = GetPrivateProfileInt( L"Config", L"y", CW_USEDEFAULT, L"XRTOBJ32.INI" ) ;
    int    cx = GetPrivateProfileInt( L"Config", L"cx", CW_USEDEFAULT, L"XRTOBJ32.INI" ) ;
    int    cy = GetPrivateProfileInt( L"Config", L"cy", CW_USEDEFAULT, L"XRTOBJ32.INI" ) ;
    BOOL    fMin = GetPrivateProfileInt( L"Config", L"Min", FALSE, L"XRTOBJ32.INI" ) ;
    
    // Create our window
#ifdef WIN32
#define SZ_TITLE L"Simple Stocks-R-Us XRT Data Object (32 bit)"
#else
#define SZ_TITLE L"Simple Stocks-R-Us XRT Data Object (16 bit)"
#endif
    g_hwndMain = CreateWindow ( L"XRTOBJ32.MainWnd", SZ_TITLE,
                                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,  
                                x,y,cx,cy, NULL, NULL, g_hInst, NULL ) ;

    // Create our class factory interface
    g_pIClassFactory = new CImpIClassFactory ;
    if (g_pIClassFactory == NULL)
    {
        AssertSz( 0, L"new CXRTClassFactory failed\r\n" ) ;
        goto BailOut ;
    }
    g_pIClassFactory->AddRef() ;

    // Register our class object
    //
    HRESULT hr ;
    hr = CoRegisterClassObject( CLSID_StocksRUsStockQuotes, (LPUNKNOWN)g_pIClassFactory,
                                CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_dwRegCO ) ;
    if (FAILED(hr))
    {
        AssertSz( 0, L"CoRegisterClassObject failed\r\n" ) ;
        goto BailOut ;
    }

    if (lstrcmpi( g_pszCmdLine, L"/Embedding" ) == 0)
        nCmdShow = SW_MINIMIZE ;

    if (fMin == TRUE)  
        nCmdShow = SW_MINIMIZE ;

    ShowWindow( g_hwndMain, nCmdShow ) ;
    UpdateWindow( g_hwndMain ) ;

    // message loop
    //
    MSG            msg ;
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage( &msg ) ;
        DispatchMessage( &msg ) ;
    }

    if (g_dwRegCO != 0)
        CoRevokeClassObject( g_dwRegCO ) ;

    if (g_pIClassFactory)
        g_pIClassFactory->Release() ;

BailOut:
    OleUninitialize() ;
    return 0 ;
}

// Window procedure for the main application window
//
LRESULT CALLBACK fnMainWnd( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
#define TIMER_MARKETRATE    1
#define TIMER_UPDATERATE    2

    static UINT uiTimer1 = 0 ;
    static UINT uiTimer2 = 0 ;

    switch( uiMsg )
    {
        case WM_CREATE:
        {
            for (int i = 0 ; i < MAX_OBJECTS ; i++)
                g_rgpObj[i] = NULL ;

            // Create an instance of our market simulation object.   This object
            // is a self contained simple market simulation.  It is driven by
            // it's ::OnTick method which is called in the WM_TIMER case below
            //
            g_pSimObj = new CSimulation ;

            if (g_pSimObj == NULL)
                return -1 ;

            // Create two timers:
            //  One for the market update rate (ie how fast the market
            //  simulation runs).
            //  And one for the upadate rate (ie how often IAdviseSink::OnDataChange
            //  notifications are generated.
            //
            uiTimer1 = SetTimer( hwnd, TIMER_MARKETRATE, g_uiTimeoutMarket, NULL ) ;
            uiTimer2 = SetTimer( hwnd, TIMER_UPDATERATE, g_uiTimeoutUpdate, NULL ) ;
        }
        break ;

        case WM_TIMER:
        {
            UINT    wTimerID = wParam ;

            switch (wTimerID)
            {
                case TIMER_MARKETRATE:
                    // Make the simulation generate data
                    //
                    g_pSimObj->OnTick() ;
                break ;

                case TIMER_UPDATERATE:
                    // Notify all instances of CStocksObject
                    //
                    // TODO:  Re-implement code to manage list of instanced objects
                    //      There's a listbox where the Itemdata is a pointer to
                    //      the object.
                    //
                    for (int i = 0 ; i < MAX_OBJECTS ; i++)
                    {
                        if (g_rgpObj[i] != NULL)
                            g_rgpObj[i]->OnUpdateTick() ;
                    }
                break ;
            }
        }
        break ;

        case WM_COMMAND:
        {
            #ifdef WIN32
            WORD wNotifyCode = HIWORD(wParam);  
            WORD wID = LOWORD(wParam);         
            HWND hwndCtl = (HWND) lParam;      
            #else
            WORD wNotifyCode = HIWORD(lParam) ;
            WORD wID = wParam ;
            HWND hwndCtl = (HWND)LOWORD(lParam) ;
            #endif

            switch (wID)
            {
                case ID_OPTIONS_UPDATESPEED:
                    // Pop up dialog that allows the update speed to
                    // be changed
                    //
                    // TODO: Implement dialog
                    //


                    if (uiTimer1) 
                        KillTimer( hwnd, TIMER_UPDATERATE ) ;
                    uiTimer1 = SetTimer( hwnd, TIMER_UPDATERATE, g_uiTimeoutUpdate, NULL ) ;
                break ;

                case ID_OPTIONS_MARKETSIMULATION:
                    // Popup dialog that allows simulation parameters to
                    // be changed
                    //
                    // TODO: Implement dialog
                    //


                    if (uiTimer2) 
                        KillTimer( hwnd, TIMER_MARKETRATE ) ;
                    uiTimer2 = SetTimer( hwnd, TIMER_MARKETRATE, g_uiTimeoutMarket, NULL ) ;
                break ;
                
                case ID_HELP_ABOUT:
                    DoAboutBox() ;
                break ;
    
                case ID_FILE_EXIT:
                    SendMessage( hwnd, WM_CLOSE, 0, 0L ) ;
                break ;
            }
        }
        break ;

        case WM_PAINT:
        {
            TEXTMETRIC  tm ;
            PAINTSTRUCT ps ;

            if (!BeginPaint( hwnd, &ps ))
                return FALSE ;

            GetTextMetrics( ps.hdc, &tm ) ;

            // Real simple code to display the latest simulation
            // data.
            //
            if (g_pSimObj) 
            {
                PXRTSTOCKS  pData = g_pSimObj->GetData() ;
                int cy = 4 ;
                RECT rc ;
                TCHAR  szBuf[1024*16] ;
                TCHAR  szItem[64] ;
                PXRTSTOCK   pQuote ;

                GetClientRect( hwnd, &rc ) ;
                szBuf[0] = '\0' ;
                for (UINT i = 0 ; i < pData->cStocks ; i++)
                {
                    pQuote = &pData->rgStocks[i] ;
                    if (pQuote->uiMembers)
                    {
                        wsprintf( szItem, L"%s\t%lu.%02lu\t%lu.%02lu\t%lu.%02lu\t%lu\n",     
                                    (LPTSTR)pQuote->szSymbol,
                                    pQuote->ulHigh/1000L, pQuote->ulHigh%1000L,
                                    pQuote->ulLow/1000L, pQuote->ulLow%1000L,
                                    pQuote->ulLast/1000L, pQuote->ulLast%1000L,
                                    pQuote->ulVolume
                                     ) ;
                        lstrcat( szBuf, szItem ) ;
                    }
                }
                DrawText( ps.hdc, szBuf, -1, &rc, DT_EXPANDTABS ) ;
            }

            EndPaint( hwnd, &ps ) ;
        }
        break ;

        case WM_DESTROY:
            if (g_pSimObj)
                 delete g_pSimObj ;

            if (!IsIconic( hwnd ))
            {
                RECT    rc ;
                TCHAR    szBuf[32] ;
                GetWindowRect( hwnd, &rc ) ;

                wsprintf( szBuf, L"%d", rc.left ) ;
                WritePrivateProfileString( L"Config", L"x", szBuf, L"XRTOBJ32.INI" ) ;
                wsprintf( szBuf, L"%d", rc.top ) ;
                WritePrivateProfileString( L"Config", L"y", szBuf, L"XRTOBJ32.INI" ) ;
                wsprintf( szBuf, L"%d", rc.right - rc.left ) ;
                WritePrivateProfileString( L"Config", L"cx", szBuf, L"XRTOBJ32.INI" ) ;
                wsprintf( szBuf, L"%d", rc.bottom - rc.top) ;
                WritePrivateProfileString( L"Config", L"cy", szBuf, L"XRTOBJ32.INI" ) ;
                WritePrivateProfileString( L"Config", L"Min", L"0", L"XRTOBJ32.INI" ) ;
            }
            else
            {
                WritePrivateProfileString( L"Config", L"Min", L"1", L"XRTOBJ32.INI" ) ;
            }

            if (uiTimer1)   KillTimer( hwnd, TIMER_MARKETRATE ) ;
            if (uiTimer2)   KillTimer( hwnd, TIMER_UPDATERATE ) ;
            PostQuitMessage( 0 ) ;
        break ;

        default:
            return DefWindowProc( hwnd, uiMsg, wParam, lParam ) ;
    }

    return 0L ;

} // fnMainWnd() 



// InitApplicaiton
//
// Initialize our application stuff including
// Window classes
//
BOOL InitApplication( HINSTANCE hInst )
{
    WNDCLASS    wc ;

    wc.style             = 0L ;
    wc.lpfnWndProc       = fnMainWnd ;
    wc.cbClsExtra        = 0 ;
    wc.cbWndExtra        = 0 ;
    wc.hInstance         = hInst ;
    wc.hIcon             = LoadIcon( hInst,
                                MAKEINTRESOURCE( IDI_XRTOBJ ) ) ;
    wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
    wc.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1) ;
    wc.lpszMenuName      = MAKEINTRESOURCE( ID_MENU ) ;
    wc.lpszClassName     = L"XRTOBJ32.MainWnd" ;

    if (!RegisterClass (&wc))
        return FALSE ;

    return TRUE ;

}

// Implement About Box
//
BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;
BOOL WINAPI DoAboutBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnAbout, g_hInst ) ;
   DialogBox( g_hInst,
              MAKEINTRESOURCE( IDD_ABOUT ),
              g_hwndMain,
              lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) 
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
        // TODO:  Dynamicaly set version number in about box
        //
      break ;

      case WM_COMMAND:
      {
         #ifdef WIN32
         WORD wNotifyCode = HIWORD(wParam);  
         WORD wID = LOWORD(wParam);         
         HWND hwndCtl = (HWND) lParam;      
         #else
         WORD wNotifyCode = HIWORD(lParam) ;
         WORD wID = wParam ;
         HWND hwndCtl = (HWND)LOWORD(lParam) ;
         #endif

         switch (wID)
         {
            case IDOK:
            case IDCANCEL:
               EndDialog (hDlg, wID) ;
            break ;

            default :
               return FALSE ;
         }
         break ;
      }
      break ;

   }
   return FALSE ;
}


// IClassFactory Implementation
// CImpIClassFactory::CImpIClassFactory
// CImpIClassFactory::~CImpIClassFactory
//
//
CImpIClassFactory::CImpIClassFactory(void)
{
    m_cRef=0L;
    return;
}

CImpIClassFactory::~CImpIClassFactory(void)
{
    return ;
}

// CImpIClassFactory::QueryInterface
// CImpIClassFactory::AddRef
// CImpIClassFactory::Release
//
// Purpose:
//  IUnknown members for CImpIClassFactory object.
//
STDMETHODIMP CImpIClassFactory::QueryInterface( REFIID riid, LPVOID FAR *ppv )
{
    *ppv = NULL ;    
    
    //Any interface on this object is the object pointer.
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
        *ppv = (LPVOID)this;
   
    // If we actually assign an interface to ppv we need to AddRef it
    // since we're returning a new pointer.
    //
    if (NULL != *ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CImpIClassFactory::AddRef(void)
{
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) CImpIClassFactory::Release(void)
{
    ULONG           cRefT;

    cRefT=--m_cRef;

    //Free the object if the reference and lock counts are zero.
    //
    if (0L == m_cRef && 0L == g_cLock)
        delete this;

    return cRefT;
}


// CImpIClassFactory::CreateInstance
//
// Purpose:
//  Instantiates an object supported by this class factory.  That
//  object must at least support IUnknown.
//
//  Derived classes should override this function.
//
// Parameters:
//  punkOuter       LPUNKNOWN to the controlling IUnknown if we are
//                  being used in an aggregation.
//  riid            REFIID identifying the interface the caller desires
//                  to have for the new object.
//  ppvObj          LPVOID FAR * in which to store the desired interface
//                  pointer for the new object.
//
// Return Value:
//  HRESULT         NOERROR if successful, otherwise contains E_NOINTERFACE
//                  if we cannot support the requested interface.
//
STDMETHODIMP CImpIClassFactory::CreateInstance(LPUNKNOWN punkOuter,
                                    REFIID riid, LPVOID FAR *ppvObj)
{
    HRESULT         hr ;
    
    *ppvObj = NULL ;
    
    hr = ResultFromScode(E_OUTOFMEMORY) ;
    
    // Verify that if there is a controlling unknown it's asking for
    // IUnknown
    if (NULL != punkOuter && !IsEqualIID( riid, IID_IUnknown ))
        return ResultFromScode( E_NOINTERFACE ) ;

    // Create an instance of the object.  Pass it the controlling
    // unknown and a pointer to the function it should call when
    // the object has been created/destroyed
    //
    CStocksObject*  pObj ;
    pObj = new CStocksObject( punkOuter, ObjectCreated ) ;

    if (pObj == NULL)
        return hr ;

    if (pObj->Init())
        hr = pObj->QueryInterface( riid, ppvObj ) ;

    if (FAILED(hr))
        delete pObj ;
     
    return hr ;
}

// This function is called by the constructor and destructor of
// the CStocksObject class.  
//
void WINAPI ObjectCreated( CStocksObject* pObj, BOOL fCreated ) 
{
    if (fCreated)
    {
        for (int i = 0 ; i < MAX_OBJECTS ; i++)
        {
            if (g_rgpObj[i] == NULL)
            {
                g_rgpObj[i] = pObj ;
                break ;
            }
        }
        
        g_cObj++ ;
    }
    else
    {
        for (int i = 0 ; i < MAX_OBJECTS ; i++)
        {
            if (g_rgpObj[i] == pObj)
            {
                g_rgpObj[i] = NULL ;
                break ;
            }
        }

        g_cObj-- ;
    }

    if (g_cObj == 0 && g_cLock == 0 && IsWindow( g_hwndMain ))
    {
        PostMessage( g_hwndMain, WM_CLOSE, 0, 0 ) ;
    }

    return ;
}    

// CImpIClassFactory::LockServer
//
// Purpose:
//  Increments or decrements the lock count of the serving IClassFactory
//  object.  When the lock count is zero and the reference count is zero
//  we get rid of this object.
//
//  DLL objects should override this to affect their DLL ref count.
//
// Parameters:
//  fLock           BOOL specifying whether to increment or decrement the
//                  lock count.
//
// Return Value:
//  HRESULT         NOERROR always.
//
STDMETHODIMP CImpIClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
        g_cLock++ ;
    else
        g_cLock-- ;

    if (m_cRef == 0 && g_cLock == 0 && IsWindow( g_hwndMain ))
    {
        PostMessage( g_hwndMain, WM_CLOSE, 0, 0 ) ;
    }
    
    return NOERROR;
}
