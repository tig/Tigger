// WOSA/XRT XRTFrame Sample Application Version 1.01
//
// Copyright (c) 1993-94 Microsoft Corporation, All Rights Reserved.
//
//      This is a part of the Microsoft Source Code Samples. 
//      This source code is only intended as a supplement to 
//      Microsoft Development Tools and/or WinHelp documentation.
//      See these sources for detailed information regarding the 
//      Microsoft samples programs.
//
// xrtframe.cpp : Defines the class behaviors for the application.
//
// CXRTFrameApp, CApplication, CAboutBox implementation  
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//
// The CApplication class implements an automation interface for the
// "Application" object.  The following
// methods and properties are defined on XRTFrame.Application.1
//
// (Note:  those marked with a '*' are not implemented yet).
// (Note:  '!' indicates "Value" property)
//
//  Items Object
//      Methods:
//          Activate
//          Quit
//          Quit( save )*
//          DataObjects*
//          DataObjects( name )*
//          DataObjects( index )*
//          DoAbout
//
//      Properties:
//          Application [out]
//          Visible [in/out]
//          ActiveDocument [out]*
//

#include "stdafx.h"
#include "xrtframe.h"

#include "mainfrm.h"
#include "ipframe.h"
#include "doc.h"
#include "view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXRTFrameApp

BEGIN_MESSAGE_MAP(CXRTFrameApp, CWinApp)
    //{{AFX_MSG_MAP(CXRTFrameApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CXRTFrameApp, CWinApp)
IMPLEMENT_DYNCREATE(CApplication, CCmdTarget)
#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CXRTFrameApp construction

CXRTFrameApp::CXRTFrameApp()
{
}

CXRTFrameApp::~CXRTFrameApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXRTFrameApp object

CXRTFrameApp NEAR theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
const CLSID BASED_CODE g_clsidDataObject =
{ 0x5EE917A0L, 0xD5DD, 0x101A, { 0x9A, 0x7C, 0x00, 0xAA, 0x00, 0x33, 0x97, 0x10 } };

/////////////////////////////////////////////////////////////////////////////
// CXRTFrameApp initialization

BOOL CXRTFrameApp::InitInstance()
{
    char szProfile[_MAX_PATH+1] ;
    lstrcpy( szProfile, "XRTFRAME.INI" ) ;
    m_pszProfileName = strdup( szProfile ) ;

    // Initialize OLE 2.0 libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    #if _MFC_VER >= 0x0300
	Enable3dControls();
    #else
    SetDialogBkColor();        // Set dialog background color to gray
    #endif
    
    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(
        IDR_XRTFRAMETYPE,
        RUNTIME_CLASS(CDataObject),
        RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
        RUNTIME_CLASS(CDataObjectView));
    pDocTemplate->SetServerInfo(
        IDR_XRTFRMTYPE_SRVR_EMB, IDR_XRTFRMTYPE_SRVR_IP,
        RUNTIME_CLASS(CInPlaceFrame));
    AddDocTemplate(pDocTemplate);


    // Connect the COleTemplateServer to the document template.
    //  The COleTemplateServer creates new documents on behalf
    //  of requesting OLE containers by using information
    //  specified in the document template.
    m_server.ConnectTemplate(g_clsidDataObject, pDocTemplate, FALSE);

    // Register all OLE server factories as running.  This enables the
    //  OLE 2.0 libraries to create objects from other applications.
    COleTemplateServer::RegisterAll();
        // Note: MDI applications register all server objects without regard
        //  to the /Embedding or /Automation on the command line.

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    m_pMainWnd = pMainFrame;

    // Parse the command line to see if launched as OLE server
    if (RunEmbedded() || RunAutomated())
    {
        // Application was run with /Embedding or /Automation.  Don't show the
        //  main window in this case.
        return TRUE;
    }

    // When a server application is launched stand-alone, it is a good idea
    //  to update the system registry in case it has been damaged.
    m_server.UpdateRegistry(OAT_INPLACE_SERVER);
    COleObjectFactory::UpdateRegistryAll();

    USES_CONVERSION;
    OLECHAR wszCLSID[40] ;
    StringFromGUID2(g_clsidDataObject, wszCLSID, 40);
    CString str;
    str.Format("CLSID\\%S\\ActivateAtStorage", wszCLSID);
    RegSetValue(HKEY_CLASSES_ROOT, str, REG_SZ, _T("Y"), 1);

    EnableShellOpen();
    RegisterShellFileTypes();

    // create a new (empty) document
    OnFileNew();

    if (m_lpCmdLine[0] != '\0')
    {
        // TODO: add command line processing here
    }

    // The main window has been initialized, so show and update it.
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //{{AFX_MSG(CAboutDlg)
        // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CXRTFrameApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CApplication is needed because CWinApp cannot be an OLECREATEable
// 
BEGIN_MESSAGE_MAP(CApplication, CCmdTarget)
    //{{AFX_MSG_MAP(CApplication)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CApplication, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CApplication)
    DISP_PROPERTY_EX(CApplication, "Visible", GetVisible, SetVisible, VT_BOOL)
    DISP_PROPERTY_EX(CApplication, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
    DISP_PROPERTY_EX(CApplication, "FullName", GetFullName, SetNotSupported, VT_BSTR)
    DISP_PROPERTY_EX(CApplication, "Name", GetName, SetNotSupported, VT_BSTR)
    DISP_FUNCTION(CApplication, "Quit", Quit, VT_EMPTY, VTS_VARIANT)
    DISP_FUNCTION(CApplication, "DoAbout", DoAbout, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION(CApplication, "Activate", Activate, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
    // BUGFIX: 9/29/94 cek 
    // Changed .DataObjects from DISP_FUNCTION to DISP_PROPERTY_PARAM
    // so that app.DataObjects("FOO.XRF") works.
    //
    //DISP_FUNCTION(CApplication, "DataObjects", DataObjects, VT_DISPATCH, VTS_VARIANT)
    DISP_PROPERTY_PARAM(CApplication, "DataObjects", DataObjects, SetNotSupported, VT_VARIANT, VTS_VARIANT)
    DISP_PROPERTY_EX(CApplication, "Parent", GetApplication, SetNotSupported, VT_DISPATCH)
END_DISPATCH_MAP()

IMPLEMENT_OLECREATE(CApplication, "XRTFrame.Application.1", 0x5EE917A1L, 0xD5DD, 0x101A, 0x9A, 0x7C, 0x00, 0xAA, 0x00, 0x33, 0x97, 0x10)
#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CApplication

CApplication::CApplication()
{
    EnableAutomation();
    
    // To keep the application running as long as an OLE automation 
    //  object is active, the constructor calls AfxOleLockApp.
    
    AfxOleLockApp();
}

CApplication::~CApplication()
{
    AfxOleUnlockApp();
}

void CApplication::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.

    delete this;
}


/////////////////////////////////////////////////////////////////////////////
// CApplication message handlers

void CApplication::Quit(const VARIANT FAR& save)
{
    save ;
    AfxGetApp()->m_pMainWnd->SendMessage( WM_CLOSE ) ;
}

void CApplication::DoAbout()
{
    ((CXRTFrameApp*)AfxGetApp())->OnAppAbout() ;
}

BOOL CApplication::GetVisible()
{
    return AfxGetApp()->m_pMainWnd->IsWindowVisible() ;
}

void CApplication::SetVisible(BOOL fVisible)
{
    if (fVisible)
        AfxGetApp()->m_pMainWnd->ShowWindow( SW_SHOW ) ;
    else
        AfxGetApp()->m_pMainWnd->ShowWindow( SW_HIDE ) ;
    AfxOleSetUserCtrl( fVisible ) ;
}

void CApplication::Activate()
{
    ((CXRTFrameApp*)AfxGetApp())->m_pMainWnd->BringWindowToTop() ;
}

VARIANT CApplication::DataObjects(const VARIANT FAR& item)
{
    // BUGFIX: 9/29/94 cek 
    // Changed .DataObjects from DISP_FUNCTION to DISP_PROPERTY_PARAM
    // so that app.DataObjects("FOO.XRF") works.
    //
    VARIANT va;
    VariantInit(&va) ;

    LPDISPATCH lpDisp = NULL ;
    CDataObjects* pItems = new CDataObjects ;

    ASSERT(pItems) ;
    if (pItems == NULL || !pItems->IsKindOf(RUNTIME_CLASS(CDataObjects)))
    {
        // TODO:  raise exception
        va.vt = VT_DISPATCH ;
        va.pdispVal = NULL ;
        return va ;
    }    

    if (item.vt == VT_ERROR ||item.vt == VT_EMPTY ) // no index
    {
        va.vt = VT_DISPATCH ;
        va.pdispVal = pItems->GetIDispatch(FALSE) ;
        return va;
    }
    else
    {
        va.vt = VT_DISPATCH ;
        va.pdispVal = pItems->GetItem( item ) ;
        delete pItems ;
        return va ;
    }
}

LPDISPATCH CApplication::GetApplication()
{
    LPDISPATCH  lpDisp = NULL ;
    lpDisp = GetIDispatch(TRUE) ;
    
    return lpDisp ;
}

BSTR CApplication::GetFullName()
{
    char szFileName[260] ;
    GetModuleFileName( AfxGetApp()->m_hInstance, szFileName, 260 ) ;
    USES_CONVERSION;
    BSTR bstr ;
    bstr = SysAllocStringLen( T2OLE(szFileName), lstrlen(szFileName) +1) ;
    return bstr ;
}

BSTR CApplication::GetName()
{
    CString s;
    
    s = AfxGetAppName() ;
    return s.AllocSysString();
}

