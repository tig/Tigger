// Ole2View.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <winver.h>

#include <initguid.h>
DEFINE_GUID(CATID_Ole2ViewInterfaceViewers, 0x64454f82, 0xf827, 0x11ce, 0x90, 0x59, 0x8, 0x0, 0x36, 0xf1, 0x25, 0x2);
DEFINE_GUID(IID_IInterfaceViewer,0xfc37e5ba,0x4a8e,0x11ce, 0x87,0x0b,0x08,0x00,0x36,0x8d,0x23,0x02);
#include "iviewers\\iview.h"

#include "Ole2View.h"

#include "mainfrm.h"
#include "doc.h"
#include "obj_vw.h"
#include "regview.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// Bump this to force registration of iviewers.dll
#define VER_BUILD 30

/////////////////////////////////////////////////////////////////////////////
// COle2ViewApp

BEGIN_MESSAGE_MAP(COle2ViewApp, CWinApp)
    //{{AFX_MSG_MAP(COle2ViewApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
END_MESSAGE_MAP()

CImageList* g_pImages ;

/////////////////////////////////////////////////////////////////////////////
// COle2ViewApp construction

COle2ViewApp::COle2ViewApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COle2ViewApp object

COle2ViewApp theApp;
#if _MFC_VER >= 0x0300
OSVERSIONINFO  g_osvi ;
#endif

/////////////////////////////////////////////////////////////////////////////
// COle2ViewApp initialization

BOOL COle2ViewApp::InitInstance()
{
	USES_CONVERSION;
    g_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
    GetVersionEx(&g_osvi);

    HRESULT hr ;
    /*
    if (g_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && LOWORD(g_osvi.dwBuildNumber) >= 1200)
    {
        // We're running on SUR, so enable Network OLE
        //
        // COM initialization flags; passed to CoInitialize.
        //     typedef enum tagCOINIT
        //     {
        //       COINIT_MULTITHREADED = 0,     // OLE calls objects on any thread.
        //       COINIT_SINGLETHREADED = 1,    // OLE calls objects on single thread.
        //       COINIT_APARTMENTTHREADED  = 2	// Apartment model
        //     } COINIT;
        // 
        //     WINOLEAPI  CoInitializeEx(LPVOID pvReserved, DWORD);
        HINSTANCE hmod = LoadLibrary(_T("OLE32.DLL"));
        if (hmod)
        {
            HRESULT (STDAPICALLTYPE* lpCoInitEx)(void*, DWORD);
            (FARPROC&)lpCoInitEx = GetProcAddress(hmod,_T("CoInitializeEx"));
            if (lpCoInitEx)
            {
                if (FAILED(hr = (*lpCoInitEx)(NULL, 1)))
                {
                    ErrorMessage( _T("CoInitializeEx failed."), hr ) ;
                    return FALSE;
                }
                else
                {
                    AfxMessageBox(_T("Network OLE enabled."));
                }
            }
            else
            {
                FreeLibrary(hmod);
                AfxMessageBox( _T("Could not find CoInitializeEx in OLE32.DLL") ) ;
                return FALSE;
            }

            FreeLibrary(hmod);
        }
        else
        {
            AfxMessageBox( _T("Could not load OLE32.DLL") ) ;
            return FALSE;
        }
    }
    */

    if (FAILED(hr = AfxOleInit())) //OleInitialize(NULL)))
    {
        ErrorMessage( _T("OleInitialize failed.  Could not initialized OLE; Ole2View cannot run."), hr ) ;
        return FALSE;
    }

    SetRegistryKey( IDS_REGISTRYKEY );

    if (g_osvi.dwMajorVersion >= 4)
    {
        AfxEnableWin40Compatibility() ;
    }
    else
    	Enable3dControls();

    g_pImages = new CImageList;
    if (0 == g_pImages->Create( IDB_IMAGELIST, 16, 100, RGB(0,255,0) /*RGBLTGREEN*/ ))
        AfxMessageBox("Could not load bitmaps" ) ;

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.
    AddDocTemplate(new CSingleDocTemplate(IDR_MAINFRAME,
            RUNTIME_CLASS(COle2ViewDoc),
            RUNTIME_CLASS(CMainFrame),     // main SDI frame window
            RUNTIME_CLASS(CObjTreeView)));

    int nCmdShow = m_nCmdShow ;
#ifndef _MAC
    m_nCmdShow = SW_HIDE ;
    OnFileNew() ;

    // AutoRegister iviewers if not already registered
    //
    RegisterIViewersDLL(AfxGetMainWnd(), FALSE) ;

    ((CMainFrame*)GetMainWnd())->RestorePosition(nCmdShow) ;
    ProcessCmdLine() ;
    ((CMainFrame*)GetMainWnd())->m_pObjTreeView->SetFocus();
#endif // !_MAC

    return TRUE;
}

// This function attempts to register the
// interface viewers in IVIEWERS.DLL
//
BOOL RegisterIViewersDLL(CWnd* pParent,BOOL fForce /*=FALSE*/)
{
    CString szSection;
    szSection.LoadString( IDS_INI_CONFIG ) ;
    if (fForce != TRUE)
    {
        USES_CONVERSION;

        TCHAR szBuf[128] ; LONG cb = sizeof(szBuf) ;
	    TCHAR szKey[128] ;
	    OLECHAR wszCATID[40];
	    StringFromGUID2(CATID_Ole2ViewInterfaceViewers, wszCATID, 64) ;
	    wsprintf(szKey, _T("Component Categories\\%s"), (TCHAR*)OLE2T(wszCATID)) ;
        if (RegQueryValue(HKEY_CLASSES_ROOT, szKey, szBuf, &cb) == ERROR_SUCCESS)
        {
            int n;
            n = theApp.GetProfileInt(szSection, _T("Version"), 0) ;
            if (n >= VER_BUILD)
                return TRUE ;
        }
    }

//	CFileDialog

    CString str ;
    CString str2 ;
	CString strViewerDLL = _T("IViewers.DLL") ;
    HINSTANCE hinst ;
	BOOL	fRet = FALSE ;

TryToLoadIViewers:	
	hinst = LoadLibrary( strViewerDLL ) ;
    if (hinst > (HINSTANCE)HINSTANCE_ERROR)
    {
        // Get DllRegisterServer function
        HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
        (FARPROC&)lpDllEntryPoint = GetProcAddress(hinst, "DllRegisterServer");
        if (lpDllEntryPoint)
        {
            HRESULT hr ;
            if (FAILED(hr = (*lpDllEntryPoint)()))
            {
                str.LoadString( IDS_AUTOREGFAILED ) ;
                str2.LoadString( IDS_IVIEWERSNOTFOUND2 ) ; 
                str += str2 ;
                AfxMessageBox( str ) ;
            }
			else
				fRet = TRUE ;
        }
        else
        {
            str.LoadString( IDS_IVIEWERSNOTFOUND3 ) ;
            str2.LoadString( IDS_IVIEWERSNOTFOUND2 ) ; 
            str += str2 ;
            AfxMessageBox( str ) ;
        }

        FreeLibrary( hinst ) ;
    }
    else
    {
        str.LoadString( IDS_IVIEWERSNOTFOUND1 ) ;
        str2.LoadString( IDS_IVIEWERSNOTFOUND2 ) ; 
        str += str2 ;
        str2.LoadString( IDS_IVIEWERSNOTFOUND4 ) ; 
        str += str2 ;
        if (AfxMessageBox( str, MB_YESNO ) == IDYES)
		{
			static TCHAR szFilter[] = _T("DLL Files (*.dll)|*.dll|AllFiles(*.*)|*.*|") ;

			CFileDialog dlg(TRUE, _T("iviewer.dll"), NULL,
							OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
							szFilter, pParent);
			if (IDOK == dlg.DoModal())
			{
				strViewerDLL = dlg.GetPathName() ;
				goto TryToLoadIViewers ;
			}
		}
    }

    if (fRet == TRUE)
        theApp.WriteProfileInt(szSection, _T("Version"), VER_BUILD) ;

	return fRet ;
}


int COle2ViewApp::ExitInstance()
{
    if (g_pImages)
        delete g_pImages;
    return CWinApp::ExitInstance() ;
}

void COle2ViewApp::OnAppAbout() 
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

BOOL COle2ViewApp::ProcessCmdLine()
{
    CMainFrame* pfrm = (CMainFrame*)GetMainWnd() ;
    // If there's something on the command line, try to
    // figure out if it's a 
	//  - CLSID or a ProgID: CoCreateInstance the thing
	//  - A filename: Bind to the file, via a FileMoniker
	// Then enum the interfaces as normal.
    //
    // 
    //
    if (m_lpCmdLine != NULL && *m_lpCmdLine != '\0')
    {
        ASSERT(pfrm->m_pObjTreeView) ;
        IUnknown* punk = NULL ;
        if (SUCCEEDED(pfrm->m_pObjTreeView->BindToFile(m_lpCmdLine, &punk)))
        {
            pfrm->m_pObjTreeView->AddObjectInstance(punk, m_lpCmdLine) ;
            punk->Release() ;
        }
        else 
        {
            // Maybe it's a ProgID
            //
            USES_CONVERSION;
            CLSID clsid ;
            HRESULT hr ;
            if (SUCCEEDED(CLSIDFromProgID(T2OLE(m_lpCmdLine), &clsid)))
            {
                hr = CoCreateInstance(clsid, NULL, 
                                ((COle2ViewDoc*)pfrm->GetActiveDocument())->m_dwClsCtx, 
                                IID_IUnknown, (void**)&punk) ;
                if (SUCCEEDED(hr))
                {
                    pfrm->m_pObjTreeView->AddObjectInstance(punk, m_lpCmdLine) ;
                    punk->Release() ;
                }
                else
                {
                    CString strError;
                    strError.Format("CoCreateInstance failed using the CLSID for '%s'", m_lpCmdLine) ;
                    ErrorMessage(strError, hr);
                    return FALSE ;
                }
            }
            else
            {
                // Maybe it's a TLB file
                LPTYPELIB lpTypeLib;
	            hr = ::LoadTypeLib(T2COLE(m_lpCmdLine), &lpTypeLib);
                if (SUCCEEDED(hr))
                {
	                // call the interface wiewer
	                ASSERT(lpTypeLib != NULL);
                    ViewInterface( pfrm->GetSafeHwnd(), IID_ITypeLib, (IUnknown*)lpTypeLib);
                    VERIFY(0 == lpTypeLib->Release()) ;
                }
                else
                {       
		            CString szErrorMsg;
		            szErrorMsg.Format(_T("The command line (%s) does not contain a valid persistent OLE object, ProgID, or Type Library file."),(LPCTSTR)m_lpCmdLine);
                    ErrorMessage( szErrorMsg, hr );
                    return FALSE ;
                }
            }
        }
    }
    else
        return FALSE ;

    return TRUE;
}


#ifdef _MAC
BOOL COle2ViewApp::CreateInitialDocument()
{
    OnFileNew() ;

    ((CMainFrame*)GetMainWnd())->RestorePosition(m_nCmdShow) ;
    ((CMainFrame*)GetMainWnd())->m_pObjTreeView->SetFocus();

    return TRUE ;//CWinApp::CreateInitialDocument();
}
#endif

