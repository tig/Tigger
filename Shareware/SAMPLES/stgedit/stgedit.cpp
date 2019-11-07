// StgEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "StgEdit.h"

#include "MainFrm.h"
#include "StgEditDoc.h"
#include "StgEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStgEditApp

BEGIN_MESSAGE_MAP(CStgEditApp, CWinApp)
	//{{AFX_MSG_MAP(CStgEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStgEditApp construction

CStgEditApp::CStgEditApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStgEditApp object

CStgEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CStgEditApp initialization

BOOL CStgEditApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CStgEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CStgEditView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

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

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
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
void CStgEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CStgEditApp commands

void FormatHexEditLine( ULONG ulAddress, ULONG cb, BYTE* rgb, PTSTR pszLine ) 
{
	ASSERT(cb<=BYTESPERLINE);
	ULONG cch ;

	wsprintf( pszLine, TEXT("%08X: "), ulAddress ) ;
	for (cch = BYTESPERLINE * 6 + 12; (unsigned) lstrlen(TEXT("12345678:")) < cch--; )
	{
		pszLine[cch] = TEXT(' ');
	}

	for (cch = 0; cch < cb; cch++)
	{
		TCHAR szTemp[3];
		wsprintf(szTemp, TEXT("%02X"), rgb[cch]);
		pszLine[cch * 3 + cch / DIVISIONPOINTS + 10] = szTemp[0];
		pszLine[cch * 3 + cch / DIVISIONPOINTS + 11] = szTemp[1];
		char ch = rgb[cch];
		if (ch < ' ')
		{
			ch = '.';
		}
		// this is a little trick to get the character in the
		// correct type of string (UNICODE or not)
		wsprintf(szTemp, TEXT("%c"), ch);
		pszLine[cch + 10 + cch / DIVISIONPOINTS + 3 * BYTESPERLINE + BYTESPERLINE / DIVISIONPOINTS] = szTemp[0];
	}
	pszLine[cch + 10 + cch / DIVISIONPOINTS + 3 * BYTESPERLINE + BYTESPERLINE / DIVISIONPOINTS] = 0;
}

