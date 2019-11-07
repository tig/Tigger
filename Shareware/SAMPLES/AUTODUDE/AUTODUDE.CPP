// autodude.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "autodude.h"

#include "mainfrm.h"
#include "autoddoc.h"
#include "autodvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeApp

BEGIN_MESSAGE_MAP(CAutoDudeApp, CWinApp)
    //{{AFX_MSG_MAP(CAutoDudeApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeApp construction

CAutoDudeApp::CAutoDudeApp()
{                                         
    m_pFileMgr = NULL ;
    m_dwCFileMgrCF = 0 ;
    m_dwRegisterCFileMgr = 0 ;
    
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoDudeApp object

CAutoDudeApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeApp initialization

BOOL CAutoDudeApp::InitInstance()
{                 
    TRACE( "CAutoDudeApp::InitInstance()\n" ) ;
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    SetDialogBkColor();        // set dialog background color to gray
    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    if(InitOle() != NOERROR)
      return FALSE;
                                               
    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    AddDocTemplate(new CSingleDocTemplate(IDR_MAINFRAME,
            RUNTIME_CLASS(CAutoDudeDoc),
            RUNTIME_CLASS(CMainFrame),     // main SDI frame window
            RUNTIME_CLASS(CAutoDudeView)));                                             

    // create a new (empty) document
    OnFileNew();

    if (m_lpCmdLine[0] != '\0')
    {
        // TODO: add command line processing here 
        ((CAutoDudeDoc*)((CFrameWnd*)m_pMainWnd)->GetActiveDocument())->SetText( m_lpCmdLine ) ;
    }

    return TRUE;
}             

int CAutoDudeApp::ExitInstance()
{                
    TRACE( "CAutoDudeApp::ExitInstance()\n" ) ;
    
    // Must un-init OLE 2.0
    UninitOle() ;          
                
    TRACE( "Back from UninitOLE()\n" ) ;
    
    return CWinApp::ExitInstance() ;
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
void CAutoDudeApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CAutoDudeApp commands


/***
*HRESULT InitOle(void)
*Purpose:
*  Initialize Ole, and register our class factories.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
HRESULT CAutoDudeApp::InitOle()
{
    HRESULT hresult;
    IClassFactory FAR* pcf;

    if((hresult = OleInitialize(NULL)) != NOERROR)
      goto LError0;

    // create the single global instance of CFileMgr
    if((m_pFileMgr  = CFileMgr::Create()) == NULL){
      hresult = ResultFromScode(E_OUTOFMEMORY);
      goto LError0;
    }

    if((pcf = CFileMgrCF::Create()) == NULL)
      goto LError1;

    hresult = CoRegisterClassObject(
      CLSID_CFileMgr,
      pcf,
      CLSCTX_LOCAL_SERVER,
      REGCLS_MULTIPLEUSE,
      &m_dwCFileMgrCF);
      
    if(hresult != NOERROR)
      goto LError2;

    hresult = RegisterActiveObject(
      m_pFileMgr , CLSID_CFileMgr , NULL, &m_dwRegisterCFileMgr );
    if(hresult != NOERROR)
      goto LError2;

    pcf->Release();

    return NOERROR;

LError2:;
    pcf->Release();

LError1:;
    UninitOle();

LError0:;
    return hresult;
}


void CAutoDudeApp::UninitOle()
{
    if(m_dwRegisterCFileMgr  != 0)
      RevokeActiveObject(m_dwRegisterCFileMgr, NULL);

    if(m_dwCFileMgrCF != 0)
      CoRevokeClassObject(m_dwCFileMgrCF);

    OleUninitialize();
}

