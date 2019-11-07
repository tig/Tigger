// coltest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "coltest.h"

#include "mainfrm.h"
#include "coltedoc.h"
#include "coltevw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CColtestApp

BEGIN_MESSAGE_MAP(CColtestApp, CWinApp)
    //{{AFX_MSG_MAP(CColtestApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColtestApp construction

CColtestApp::CColtestApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CColtestApp object

CColtestApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CColtestApp initialization

BOOL CColtestApp::InitInstance()
{
    // wintime.c needs this.
	#ifndef WIN32
    TimeResetInternational( AfxGetInstanceHandle() ) ;
	#endif
    
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    SetDialogBkColor();        // set dialog background color to gray
    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    AddDocTemplate(new CMultiDocTemplate(IDR_CTTYPE,
            RUNTIME_CLASS(CColtestDoc),
            RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
            RUNTIME_CLASS(CColtestView)));

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();
    m_pMainWnd = pMainFrame;

    // enable file manager drag/drop and DDE Execute open
    m_pMainWnd->DragAcceptFiles();
    EnableShellOpen();
    RegisterShellFileTypes();

    // simple command line parsing
    if (m_lpCmdLine[0] == '\0')
    {
        // create a new (empty) document
        OpenDocumentFile( "*.*" );
    }
    else if ((m_lpCmdLine[0] == '-' || m_lpCmdLine[0] == '/') &&
        (m_lpCmdLine[1] == 'e' || m_lpCmdLine[1] == 'E'))
    {
        // program launched embedded - wait for DDE or OLE open
    }
    else
    {
        // open an existing document
        OpenDocumentFile(m_lpCmdLine);
    }

    return TRUE;
}

void CColtestApp::OnFileOpen()
{
    // prompt the user (with all document templates)
    OpenDocumentFile("*.*");
    // if returns NULL, the user has already been alerted
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
void CColtestApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CColtestApp commands
