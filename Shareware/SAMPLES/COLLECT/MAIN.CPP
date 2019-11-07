/*************************************************************************
**
**    OLE Automation Collection sample
**
**    main.cpp
**
**    Procedures to create application's main window.
**    Procedures to load the appropriate type information depending on language.
**
**     Written by Microsoft Product Support Services, Windows Developer Support
**    (c) Copyright Microsoft Corp. 1993 All Rights Reserved
**
*************************************************************************/
#include <windows.h>     
#include <ole2.h>
#include <olenls.h>
#include <dispatch.h>     
#include <compobj.h>
#include <initguid.h>
#include "app.h"          

// Globals    
HINSTANCE   g_hinst;                   // Instance of application

CAppCF FAR*   g_pAppCF = NULL ;                    // App Class Factory

DWORD       g_dwRegisterCF;
ULONG       g_cObj = 0 ;
ULONG       g_cLock = 0 ;

// String resource buffers
char g_szTitle[STR_LEN];

#ifdef _APP_
/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application. Should register the app class
 *  if a previous instance has not done so and do any other one-time
 *  initializations.
 *
 */
int PASCAL WinMain (HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
   MSG msg;

   LoadString(hinst, IDS_PROGNAME, g_szTitle, STR_LEN);
   
   if (!hinstPrev)
      if (!InitApplication(hinst))
         return FALSE;
         
   if (!InitOle(lpCmdLine, &g_dwRegisterCF))
      return FALSE;
            
   if (!InitInstance(hinst, nCmdShow))  
   {
      UninitOle(g_dwRegisterCF);   
      return (FALSE);    
   }

   while (GetMessage(&msg, NULL, NULL, NULL))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   
   UninitOle(g_dwRegisterCF);   
   return (msg.wParam); 
}

/*
 * InitInstance
 *
 * Purpose:
 *  Creates and shows main window
 *
 * Parameters:
 *  hinst           hInstance of application
 *  nCmdShow        specifies how window is to be shown
 *
 * Return Value:
 *  TRUE if initialization succeeded, FALSE otherwise.
 */
BOOL InitInstance (HINSTANCE hinst, int nCmdShow)
{
  
   g_hinst = hinst;
   // Create Main Window
   g_hwnd = CreateWindow("MainWndClass", g_szTitle,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       350, 200,
                       NULL, NULL, hinst, NULL);
   if (!g_hwnd)
      return FALSE;
   
   ShowWindow(g_hwnd, nCmdShow);           
   UpdateWindow(g_hwnd);           
   
   return TRUE;          
}



/*
 * InitOle
 *
 * Purpose:
 *  Initialize OLE and registers class factory
 *
 * Parameters:
 *  lpCmdLine       Command line passed to application
 *  pdwRegisterCF   Returns id returned after class factory registration. Can be used to 
 *                  revoke class factory registration.
 *
 * Return Value:
 *    TRUE if OLE initialization succeeded, FALSE otherwise.
 *
 */
BOOL InitOle(LPSTR lpCmdLine, LPDWORD pdwRegisterCF )
{
   LPCLASSFACTORY pcf = NULL;
   HRESULT hrErr;
   
   *pdwRegisterCF = 0;
   if (NOERROR != OleInitialize(NULL))
       return FALSE;
   
   // Create new class factory and register it if the application is started up by
   //    OLE for Automation.        
   #ifdef _APP_
   if (_fstrstr(lpCmdLine, "-Automation") != NULL)
   #endif
   {
       pcf = new CAppCF;
       if (!pcf)
           goto error;             
       hrErr = CoRegisterClassObject(CLSID_App, pcf,
                                    #ifdef _APP
                                     CLSCTX_LOCAL_SERVER, REGCLS_SINGLEUSE,
                                    #else
                                     CLSCTX_INPROC_SERVER, REGCLS_MULTIPLEUSE,
                                    #endif
                                     pdwRegisterCF);
       if (hrErr != NOERROR)
           goto error;           
       pcf->Release();
   }
   return TRUE;           
           
error:
    OleUninitialize();
    if (!pcf)
        pcf->Release();
    return FALSE;
}

/*
 * UnitinitOle
 *
 *  Purpose:
 *   Unitialize OLE
 *
 * Parameters:
 *  pdwRegisterCF   Id returned after class factory registration. Used to 
 *                  revoke class factory registration.
 *
 */
void UninitOle(DWORD dwRegisterCF)
{
    if(g_dwRegisterCF != 0)
      CoRevokeClassObject(g_dwRegisterCF);

    OleUninitialize();
}


#else // _APP_

/*
 * InitApplication
 *
 * Purpose:
 *  Registers window class
 *
 * Parameters:
 *  hinst       hInstance of application
 *
 * Return Value:
 *  TRUE if initialization succeeded, FALSE otherwise.
 */
BOOL InitApplication (HINSTANCE hinst)
{
   WNDCLASS wc;

   wc.style = CS_DBLCLKS;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hinst;
   wc.hIcon = LoadIcon(hinst, "GenericIcon");
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = COLOR_WINDOW + 1;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = "MainWndClass";
     
   return RegisterClass(&wc);
 
}


/*
 * MainWndProc
 *
 * Purpose:
 *  Window procedure for main window
 *
 */
LRESULT CALLBACK MainWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   {           
      case WM_DESTROY:                
      #ifdef _APP_
         PostQuitMessage(0);
      #endif
         break;

      default:                         
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   
   return NULL;
}


extern "C"
HANDLE CALLBACK LibMain(HINSTANCE hInst, WORD wDataSeg, WORD cbHeapSize, LPSTR lpCmdLine)
{

    if (0!=cbHeapSize)
        UnlockData(0);

    g_hinst=hInst;

    LoadString(g_hinst, IDS_PROGNAME, g_szTitle, STR_LEN);
  
    if (!InitApplication(hInst))
        return 0 ;

    return hInst;
}

#pragma code_seg("WEP_SEG") 
extern "C"
void CALLBACK WEP(int bSystemExit)
{
//    UninitOle(g_dwRegisterCF);   

    return;
}







/*
 * DllGetClassObject
 *
 * Purpose:
 *  Provides an IClassFactory for a given CLSID that this DLL is
 *  registered to support.  This DLL is placed under the CLSID
 *  in the registration database as the InProcServer.
 *
 * Parameters:
 *  clsID           REFCLSID that identifies the class factory desired.
 *                  Since this parameter is passed this DLL can handle
 *                  any number of objects simply by returning different
 *                  class factories here for different CLSIDs.
 *
 *  riid            REFIID specifying the interface the caller wants
 *                  on the class object, usually IID_ClassFactory.
 *
 *  ppv             LPVOID FAR * in which to return the interface pointer.
 *
 * Return Value:
 *  HRESULT         NOERROR on success, otherwise contains an error SCODE.
 */

HRESULT __export FAR PASCAL DllGetClassObject(REFCLSID rclsid, REFIID riid
    , LPVOID FAR *ppv)
    {
    //Check that we can provide the interface
    if (!IsEqualIID(riid, IID_IUnknown) && !IsEqualIID(riid, IID_IClassFactory))
        return ResultFromScode(E_NOINTERFACE);

    *ppv=NULL;

    //Check each of the three CLSIDs we support.
    if (IsEqualCLSID(rclsid, CLSID_App))
        *ppv=(LPVOID)new CAppCF ;

    if (NULL==*ppv)
        return ResultFromScode(E_OUTOFMEMORY);

    //Don't forget to AddRef the object through any interface we return
    ((LPUNKNOWN)*ppv)->AddRef();

    return NOERROR;
    }





/*
 * DllCanUnloadNow
 *
 * Purpose:
 *  Answers if the DLL can be freed, that is, if there are no
 *  references to anything this DLL provides.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if nothing is using us, FALSE otherwise.
 */

STDAPI DllCanUnloadNow(void)
    {
    SCODE   sc;

    //Our answer is whether there are any object or locks
    sc=(0L==g_cObj && 0==g_cLock) ? S_OK : S_FALSE;
    return ResultFromScode(sc);
    }


#endif // _APP_
/*
 * CreateDispatchInterface
 *
 * Purpose:
 *   Calls CreateStdDispatch to create the standard IDispatch implemenatation
 *   for an object.
 *
 * Parameters:
 *  punkController  Points to object's IUnknown implementation.
 *  pvProgInterface Points to the object's implemenatation of exposed properties and methods. 
 *  ppunkStdDispatch    Returns standard IDispatch implemenatation.
 *  clsid           Interface id of object in type library. 
 *
 * Return Value:
 *  HRESULT
 *
 */

HRESULT CreateDispatchInterface(LPUNKNOWN   punkController,
                                LPVOID      pvProgInterface,
                                IUnknown FAR* FAR* ppunkStdDispatch,
                                REFCLSID clsid)
{                          
    HRESULT hr;
    LPTYPELIB ptlib = NULL;
    LPTYPEINFO ptinfo = NULL;
    LPUNKNOWN punkStdDispatch = NULL;
    
    *ppunkStdDispatch = NULL;  
    
    // Note: OLE Automation functions do not set the out pointer parameters 
    // to NULL on an error return as required by the OLE specifications (See
    // OLE 2.01 specs Section 4.5). This is a problem that will be fixed in a
    // future version.  In the meantime, do not depend on the out pointer 
    // parameters being set to NULL when the function fails.
    
    // Load Type Library. If required, notify user on failure.
    hr = LoadRegTypeLib(LIBID_App, 1, 0, 0x0409, &ptlib);
    if (FAILED(hr))
        return hr; 
        
    // Get type information for interface of the object.    
    hr = ptlib->GetTypeInfoOfGuid(clsid, &ptinfo);
    if (FAILED(hr))  
    { 
        ptlib->Release();
        return hr;
    }   
        
    // Create a standard implementation of the IDispatch for the object.
    hr = CreateStdDispatch(punkController, pvProgInterface, ptinfo, &punkStdDispatch);
    if (FAILED(hr))
    {
        ptinfo->Release();
        ptlib->Release();
        return hr;
    }
    
    ptinfo->Release();
    ptlib->Release();
    *ppunkStdDispatch = punkStdDispatch; 
    return NOERROR;
} 

/*
 * LoadTypeInfo
 *
 *  Purpose:
 *   Gets type information of an object's interface from type library.
 *
 * Parameters:
 *  ppunkStdDispatch    Returns type information.
 *  clsid               Interface id of object in type library. 
 *
 * Return Value:
 *  HRESULT
 *
 */
HRESULT LoadTypeInfo(ITypeInfo FAR* FAR* pptinfo, REFCLSID clsid)
{                          
    HRESULT hr;
    LPTYPELIB ptlib = NULL;
    LPTYPEINFO ptinfo = NULL;

    *pptinfo = NULL; 
    
    // Note: OLE Automation functions do not set the out pointer parameters 
    // to NULL on an error return as required by the OLE specifications (See
    // OLE 2.01 specs Section 4.5). This is a problem that will be fixed in a
    // future version.  In the meantime, do not depend on the out pointer 
    // parameters being set to NULL when the function fails.
    
    // Load Type Library. If required, notify user on failure.
    hr = LoadRegTypeLib(LIBID_App, 1, 0, 0x0409, &ptlib);
    if (FAILED(hr))
        return hr; 
    
    // Get type information for interface of the object.      
    hr = ptlib->GetTypeInfoOfGuid(clsid, &ptinfo);
    if (FAILED(hr))  
    { 
        ptlib->Release();
        return hr;
    }   

    ptlib->Release();
    *pptinfo = ptinfo;
    return NOERROR;
} 
       
    
    




