// IViewers.cpp : Defines the initialization routines for the DLL.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1993 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "IViewers.h"

#include "typelib.h"
#include "iview.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR SZ_DEFAULTSVRDESC[] = _T("Ole2View Default Interface Viewer");
static TCHAR SZ_CATEGORYDESC[]   = _T("Ole2View Interface Viewers");

/////////////////////////////////////////////////////////////////////////////
// CIViewersApp

BEGIN_MESSAGE_MAP(CIViewersApp, CWinApp)
	//{{AFX_MSG_MAP(CIViewersApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIViewersApp construction

CIViewersApp::CIViewersApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIViewersApp object

CIViewersApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIViewersApp initialization

BOOL CIViewersApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
    //AfxEnableControlContainer();
    SetRegistryKey( IDS_REGISTRYKEY );
	COleObjectFactory::RegisterAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

/*
void RegOldCatInfo(ICatRegister* pcr, REFCATID rcatid, LCID lcid, OLECHAR *szdesc, char* szOldKey)
{
    CATEGORYINFO catinfo;
    catinfo.catid = rcatid;
    catinfo.lcid = lcid;
    catinfo.szDescription = szdesc;
    if (FAILED(pcr->RegisterCategories(1, &catinfo)))
    {
        return;
    }

    OLECHAR wszguid[40];
    StringFromGUID2(catinfo.catid, wszguid, 40) ;
    TCHAR szKey[_MAX_PATH];
    wsprintf( szKey, _T("Component Categories\\%S\\OldKey"), wszguid);
	RegSetValue(HKEY_CLASSES_ROOT, szKey, REG_SZ, szOldKey, lstrlen(szOldKey)+1);
}
*/

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	USES_CONVERSION;
	OLECHAR wszIID[64];
	// Register \Interface\{IID_IInterfaceViewer} = IInterfaceViewer
    // Set = {clsid}
    StringFromGUID2(IID_IInterfaceViewer, wszIID, 64) ;
	LPTSTR lpszIID = OLE2T(wszIID);
	TCHAR szKey[128] ;
	wsprintf(szKey, _T("Interface\\%s"), lpszIID);
	TCHAR szValue[64] ;
	lstrcpy(szValue,_T("IInterfaceViewer"));
    HRESULT hr ;
	if ((hr = ::RegSetValue(HKEY_CLASSES_ROOT, szKey, REG_SZ, szValue, lstrlen(szValue)+1)) != ERROR_SUCCESS)
        return hr ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
    ICatRegister* pcr = NULL ;
#endif

    try
    {

#ifdef __ICatInformation_INTERFACE_DEFINED__
        ICatRegister* pcr = NULL ;
        HRESULT hr ;
        hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_SERVER, IID_ICatRegister, (void**)&pcr);
        if (SUCCEEDED(hr))
        {
            /*
            RegOldCatInfo(pcr, CATID_Insertable  , 0x409, L"OLE Embeddable Objects", "Insertable");
            RegOldCatInfo(pcr, CATID_Control     , 0x409, L"OLE Controls",           "Control");
            RegOldCatInfo(pcr, CATID_Programmable, 0x409, L"OLE Automation Objects", "Programmable");
            RegOldCatInfo(pcr, CATID_DocObject   , 0x409, L"OLE Document Objects",   "DocObject");
            RegOldCatInfo(pcr, CATID_IsShortcut  , 0x409, L"_IsShortcut",            "IsShortcut");
            RegOldCatInfo(pcr, CATID_NeverShowExt, 0x409, L"_NeverShowExt",          "NeverShowExt");
            RegOldCatInfo(pcr, CATID_Printable   , 0x409, L"_Printable",             "Printable");
            */
            // Performance hack
            // we release pcr below
            // End performance hack
        }
#endif
        if (FAILED(hr = DllRegisterDefaultInterfaceViewer( CLSID_IUnknownViewer, IID_IUnknown )))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_IPersistViewer, IID_IPersist )))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_IPersistViewer, IID_IPersistStream )))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_IPersistViewer, IID_IPersistStreamInit )))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_IPersistViewer, IID_IPersistMemory )))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_ITypeLibViewer, IID_ITypeLib)))
            AfxThrowOleException(hr);

        // Register the ITypeLib viewer for ITypeInfo also
        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_ITypeLibViewer, IID_ITypeInfo)))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_IDispatchViewer, IID_IDispatch )))
            AfxThrowOleException(hr);

        if (FAILED(hr = DllRegisterInterfaceViewer( CLSID_IDataObjectViewer, IID_IDataObject )))
            AfxThrowOleException(hr);
    }
    catch(CException* pException)
    {
        if (pException->IsKindOf(RUNTIME_CLASS(COleException)))
        {
            hr = ((COleException*)pException)->m_sc;
            ErrorMessage("DllRegisterServer failure.", hr);
        }
        else
            pException->ReportError();
        pException->Delete();
    }

    // Begin performance hack
    if (pcr)
        pcr->Release();
    pcr=NULL;
    // End performance hack

    if (hr != S_OK) hr = E_FAIL;
    return hr ;
}

STDAPI DllRegisterInterfaceViewer( REFCLSID rclsid, REFIID riid )
{
    HRESULT hr = S_OK ;
    HKEY hk = NULL ;

    OLECHAR szOleIID[64];
	OLECHAR szOleCLSID[64];

    try
    {
		USES_CONVERSION;
        TCHAR szKey[128];

        // \Interface\{iid} exists?
        StringFromGUID2(riid, szOleIID, 64) ;

		LPTSTR lpszIID = OLE2T(szOleIID);
#ifdef __DONT_MUNGE_NON_INSTALLED_INTERFACES_
        LONG cb ;
        TCHAR szValue[128];
        wsprintf( szKey, _T("Interface\\%s"), lpszIID );
        cb = sizeof(szValue);
        if ((hr = ::RegQueryValue(HKEY_CLASSES_ROOT, szKey, szValue, &cb)) != ERROR_SUCCESS)
        {
            ErrorMessage("RegQueryValue failed for \\Interface\\{xxx}.", hr);
            AfxThrowOleException(hr) ;
        }
#endif

        // Create \Interface\{iid}\Ole2ViewIViewerCLSID
        wsprintf( szKey, _T("Interface\\%s\\Ole2ViewIViewerCLSID"), lpszIID);
        if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
        {
            ErrorMessage("RegCreateKey failed for \\Interface\\Ole2ViewIViewerCLSID.", hr);
            AfxThrowOleException(hr) ;
        }

        // Set = {clsid}
        StringFromGUID2(rclsid, szOleCLSID, 64) ;
	    LPTSTR lpszCLSID = OLE2T(szOleCLSID);
        if ((hr = ::RegSetValue(hk, NULL, REG_SZ, lpszCLSID, lstrlen(lpszCLSID)+1)) != ERROR_SUCCESS)
        {
            ErrorMessage("RegSetValue failed for \\Interface\\Ole2ViewIViewerCLSID = {clsid}.", hr);
            AfxThrowOleException(hr) ;
        }
		RegCloseKey(hk) ;


        // Create \Component Categories\{component category id} = (default) - ""
		// and    \Component Categories\{component category id} = 409 - "Ole2View Interface Viewers"
#ifdef __ICatInformation_INTERFACE_DEFINED__
        ICatRegister* pcr = NULL ;
        HRESULT hr ;
        hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_SERVER, IID_ICatRegister, (void**)&pcr);
        if (SUCCEEDED(hr))
        {
            CATEGORYINFO catinfo;
            catinfo.catid = TYPEID_Ole2ViewInterfaceViewers;
            catinfo.lcid = 0x0409 ; // english
            wcscpy(catinfo.szDescription,T2OLE(SZ_CATEGORYDESC));
            if (FAILED(hr = pcr->RegisterCategories(1, &catinfo)))
            {
                pcr->Release();
                TRACE("ICatRegister::RegisterCategories failed. %s\n", HRtoString(hr) );
                ErrorMessage("ICatRegister::RegisterCategories failed.", hr);
                AfxThrowOleException(hr) ;
            }
            CATID rgcatid[1] ;
            rgcatid[0] = TYPEID_Ole2ViewInterfaceViewers;
            if (FAILED(hr = pcr->RegisterClassImplCategories(rclsid, 1, rgcatid)))
            {
                pcr->Release();
                TRACE("ICatRegister::RegisterClassImplCategories failed. %s\n", HRtoString(hr) );
                ErrorMessage("ICatRegister::RegisterClassImplCategories failed.",hr);
                AfxThrowOleException(hr) ;
            }
            pcr->Release();
        }
        else
#endif
        {
		    StringFromGUID2(TYPEID_Ole2ViewInterfaceViewers, szOleIID, 64);
		    lpszIID = OLE2T(szOleIID) ;
		    wsprintf( szKey, _T("Component Categories\\%s"), lpszIID ) ;
            if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
                AfxThrowOleException(hr) ;

            if ((hr = ::RegSetValueEx(hk, _T("409"), NULL, REG_SZ, 
			    (BYTE*)SZ_CATEGORYDESC, lstrlen(SZ_CATEGORYDESC)+1)) != ERROR_SUCCESS)
                AfxThrowOleException(hr) ;
		    RegCloseKey(hk) ;

            hk = NULL;
		    // Create \CLSID\{clsid}\Implemented Categories\{component category id} = ""
		    wsprintf( szKey, _T("CLSID\\%s\\Implemented Categories\\%s"), lpszCLSID, lpszIID) ;
            if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
                AfxThrowOleException(hr) ;
        }

		// Create \CLSID\{clsid}\ToolboxBitmap = "xxx"
        TCHAR szMod[_MAX_PATH+16];
        if (GetModuleFileName(AfxGetInstanceHandle(), szMod, sizeof(szMod)/sizeof(TCHAR)))
        {
  	        wsprintf( szKey, _T("CLSID\\%s\\ToolBoxBitmap32"), lpszCLSID) ;
            hk = NULL ;
            if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
                return hr ;
            lstrcat(szMod,",137");
            hr = ::RegSetValue(hk, NULL, REG_SZ, szMod, sizeof(szMod)/sizeof(TCHAR)) ;
    	    RegCloseKey(hk) ;
        }

//        if ((hr = ::RegSetValueEx(hk, lpszIID, NULL, REG_SZ, (BYTE*)_T(""), 2*sizeof(TCHAR))) != ERROR_SUCCESS)
//            AfxThrowMemoryException() ;
//		RegCloseKey(hk) ;

	}
    catch(CException* pException)
    {
		if (hk)
			RegCloseKey( hk ) ;
        pException;//->ReportError();
        throw;
    }

    return hr ;
}

STDAPI DllRegisterDefaultInterfaceViewer(REFCLSID rclsid, REFIID riid)
{
    USES_CONVERSION;
    HRESULT hr = DllRegisterInterfaceViewer(rclsid, riid);
    if (FAILED(hr))
        return hr ;

    // Create \CLSID\{component category id}\TreatAs = {rclsid}
    OLECHAR wcs[40] ;
	StringFromGUID2(TYPEID_Ole2ViewInterfaceViewers, wcs, 40);
	TCHAR* psz = OLE2T(wcs) ;
    TCHAR szKey[256] ;
	wsprintf( szKey, _T("CLSID\\%s"), psz) ;
    HKEY hk = NULL ;
    if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
        return hr ;
    hr = ::RegSetValue(hk, NULL, REG_SZ, SZ_DEFAULTSVRDESC, lstrlen(SZ_DEFAULTSVRDESC)+1) ;
	RegCloseKey(hk) ;

    TCHAR szMod[_MAX_PATH+16];
    if (GetModuleFileName(AfxGetInstanceHandle(), szMod, sizeof(szMod)/sizeof(TCHAR)))
    {
  	    wsprintf( szKey, _T("CLSID\\%s\\ToolBoxBitmap32"), psz) ;
        hk = NULL ;
        if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
            return hr ;
        lstrcat(szMod,",137");
        hr = ::RegSetValue(hk, NULL, REG_SZ, szMod, sizeof(szMod)/sizeof(TCHAR)) ;
    	RegCloseKey(hk) ;
    }

   	wsprintf( szKey, _T("CLSID\\%s\\TreatAs"), psz) ;
    hk = NULL ;
    if ((hr = ::RegCreateKey(HKEY_CLASSES_ROOT, szKey, &hk)) != ERROR_SUCCESS)
        return hr ;
	StringFromGUID2(rclsid, wcs, 40);
	psz = OLE2T(wcs) ;
    hr = ::RegSetValue(hk, NULL, REG_SZ, psz, lstrlen(psz)+1) ;
	RegCloseKey(hk) ;

    return hr ;
}
