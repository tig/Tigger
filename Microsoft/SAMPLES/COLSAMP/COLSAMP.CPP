// colsamp.cpp : Defines the class behaviors for the application.
//
// CColSampApp, CApplication, CAboutBox implementation  
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//
// The CApplication class implements an automation interface for the
// "Application" object.  The following
// methods and properties are defined on ColSamp.Application.1
//
// (Note:  those marked with a '*' are not implemented yet).
// (Note:  '!' indicates "Value" property)
//
//  Items Object
//      Methods:
//          Activate
//          Quit
//          Quit( save )*
//          Documents*
//          Documents( name )*
//          Documents( index )*
//          DoAbout
//
//      Properties:
//          Application [out]*
//          Visible [in/out]
//          ActiveDocument [out]*
//

#include "stdafx.h"
#include "colsamp.h"

#include "mainfrm.h"
#include "ipframe.h"
#include "doc.h"
#include "view.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColSampApp

BEGIN_MESSAGE_MAP(CColSampApp, CWinApp)
    //{{AFX_MSG_MAP(CColSampApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


IMPLEMENT_DYNCREATE(CColSampApp, CWinApp)

/////////////////////////////////////////////////////////////////////////////
// CColSampApp construction

CColSampApp::CColSampApp()
{
}

CColSampApp::~CColSampApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CColSampApp object

CColSampApp NEAR theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
static const CLSID BASED_CODE clsid =
{ 0x4e89d562, 0xc0d5, 0x101a, { 0x9a, 0x7c, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10 } };


/////////////////////////////////////////////////////////////////////////////
// CColSampApp initialization

BOOL CColSampApp::InitInstance()
{
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

    SetDialogBkColor();        // Set dialog background color to gray
    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(
        IDR_COLSAMPTYPE,
        RUNTIME_CLASS(CColSampDoc),
        RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
        RUNTIME_CLASS(CColSampView));
    pDocTemplate->SetServerInfo(
        IDR_COLSAMPTYPE_SRVR_EMB, IDR_COLSAMPTYPE_SRVR_IP,
        RUNTIME_CLASS(CInPlaceFrame));
    AddDocTemplate(pDocTemplate);

    // Connect the COleTemplateServer to the document template.
    //  The COleTemplateServer creates new documents on behalf
    //  of requesting OLE containers by using information
    //  specified in the document template.
    m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

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
void CColSampApp::OnAppAbout()
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
    //{{AFX_DISPATCH_MAP(CColSampApp)
    DISP_PROPERTY_EX(CApplication, "Visible", GetVisible, SetVisible, VT_BOOL)
    DISP_FUNCTION(CApplication, "Quit", Quit, VT_EMPTY, VTS_VARIANT)
    DISP_FUNCTION(CApplication, "DoAbout", DoAbout, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION(CApplication, "Activate", Activate, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION(CApplication, "Documents", Documents, VT_DISPATCH, VTS_VARIANT)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

IMPLEMENT_OLECREATE(CApplication, "ColSamp.Application.1", 0x44104580, 0xce0e, 0x101a, 0x9a, 0x7c, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10)
/////////////////////////////////////////////////////////////////////////////
// CApplication message handlers

void CApplication::Quit(const VARIANT FAR& save)
{
   AfxGetApp()->m_pMainWnd->SendMessage( WM_CLOSE ) ;
}

void CApplication::DoAbout()
{
    ((CColSampApp*)AfxGetApp())->OnAppAbout() ;
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
}

void CApplication::Activate()
{
    ((CColSampApp*)AfxGetApp())->m_pMainWnd->BringWindowToTop() ;
}

LPDISPATCH CApplication::Documents(const VARIANT FAR& index)
{
    LPDISPATCH lpDisp = NULL ;

    CDocuments* pItems = new CDocuments ;
    
    if (pItems)
    {
        lpDisp  = pItems->GetIDispatch(FALSE) ;    
    }
    else
    {
        TRACE( "new CDocuments failed\r\n") ;
    }

    if (index.vt != VT_ERROR)
    {
        LPDISPATCH lpDispItem = NULL ;
        
        if (lpDispItem = pItems->GetItem( index ))
        {
            lpDisp->Release() ;
            lpDisp = lpDispItem ;
        }
    }
        
    return lpDisp ;
}

/////////////////////////////////////////////////////////////////////////////
// CApplication

IMPLEMENT_DYNCREATE(CApplication, CCmdTarget)

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

