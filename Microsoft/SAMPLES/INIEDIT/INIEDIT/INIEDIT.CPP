// iniedit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "iniedit.h"
                     
#include "mainfrm.h"
#include "inieddoc.h"
#include "iniedvw.h"

#include "flistdoc.h"
#include "flistvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CINIeditApp

BEGIN_MESSAGE_MAP(CINIeditApp, CWinApp)
    //{{AFX_MSG_MAP(CINIeditApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
    // Global help commands
    ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CINIeditApp construction

CINIeditApp::CINIeditApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CINIeditApp object

CINIeditApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CINIeditApp initialization

BOOL CINIeditApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    SetDialogBkColor();        // set dialog background color to gray
    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    // Register the inifile document template                                                  
    // !! NOTE !! Register this first
    AddDocTemplate(new CMultiDocTemplate(IDR_INITYPE,
            RUNTIME_CLASS(CINIeditDoc),
            RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
            RUNTIME_CLASS(CINIeditView)));

                                                  
    // Register the list of files document template                                                  
    AddDocTemplate(new CMultiDocTemplate(IDR_FILESTYPE,
            RUNTIME_CLASS(CFListDoc),
            RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
            RUNTIME_CLASS(CFListView)));

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();                                                           
    m_pMainWnd = pMainFrame;

    // create the list of files document
    CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetTail();
    pTemplate->OpenDocumentFile(NULL);

    if (m_lpCmdLine[0] != '\0')
    {
        // TODO: add command line processing here
    }

    return TRUE;
}

// Override OnFileNew
void CINIeditApp::OnFileNew()
{
    if (m_templateList.IsEmpty())
    {
        TRACE0("Error : no document templates registered with CWinApp\n");
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        return;
    }

    // The CINIeditDoc template is the first one in the list.
    //    
    CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead();

    ASSERT(pTemplate != NULL);
    ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

    pTemplate->OpenDocumentFile(NULL);
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
void CINIeditApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
