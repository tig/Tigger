// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "StgEdit.h"
#include "ContentsView.h"
#include "StreamView.h"
#include "StorageView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// Helpers for saving/restoring window state

static TCHAR BASED_CODE szSection[] = _T("Settings");
static TCHAR BASED_CODE szWindowPos[] = _T("WindowPos");
static TCHAR szFormat[] = _T("%u,%u,%d,%d,%d,%d,%d,%d,%d,%d");

static BOOL PASCAL NEAR ReadWindowPlacement(LPWINDOWPLACEMENT pwp)
{
	CString strBuffer = AfxGetApp()->GetProfileString(szSection, szWindowPos);
	if (strBuffer.IsEmpty())
		return FALSE;

	WINDOWPLACEMENT wp;
	int nRead = _stscanf(strBuffer, szFormat,
		&wp.flags, &wp.showCmd,
		&wp.ptMinPosition.x, &wp.ptMinPosition.y,
		&wp.ptMaxPosition.x, &wp.ptMaxPosition.y,
		&wp.rcNormalPosition.left, &wp.rcNormalPosition.top,
		&wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom);

	if (nRead != 10)
		return FALSE;

	wp.length = sizeof wp;
	*pwp = wp;
	return TRUE;
}

static void PASCAL NEAR WriteWindowPlacement(LPWINDOWPLACEMENT pwp)
	// write a window placement to settings section of app's ini file
{
	TCHAR szBuffer[sizeof("-32767")*8 + sizeof("65535")*2];

	wsprintf(szBuffer, szFormat,
		pwp->flags, pwp->showCmd,
		pwp->ptMinPosition.x, pwp->ptMinPosition.y,
		pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
		pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
		pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileString(szSection, szWindowPos, szBuffer);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
//	m_pStgEditView = NULL ;
//    m_pContentsView	= NULL ;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	WINDOWPLACEMENT wp;
	if (ReadWindowPlacement(&wp))
		SetWindowPlacement(&wp);

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	LoadBarState(_T("General"));


   	int nWidth = AfxGetApp()->GetProfileInt(szSection, _T("SplitterPos"), 0);
	if (nWidth)
		m_wndSplitter.SetColumnInfo( 0, nWidth, 0 ) ;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnClose() 
{
	// SaveBarState saves everything but the number of Columns in the Palette
	// we need to do that ourseleves.
	SaveBarState(_T("General"));

	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if (GetWindowPlacement(&wp))
	{
		wp.flags = 0;
		if (IsZoomed())
			wp.flags |= WPF_RESTORETOMAXIMIZED;
		// and write it to the .INI file
		WriteWindowPlacement(&wp);
	}

    int nWidth, n ;
    m_wndSplitter.GetColumnInfo( 0, nWidth, n ) ;
	AfxGetApp()->WriteProfileInt(szSection, _T("SplitterPos"), nWidth);

	CFrameWnd::OnClose();
}
// CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
    // create a splitter with 1 row, 2 columns
    if (!m_wndSplitter.CreateStatic(this, 1, 2))
    {
        TRACE(_T("Failed to CreateStaticSplitter\n"));
        return FALSE;
    }

    // add the first splitter pane - the default view in column 0
    if (!m_wndSplitter.CreateView(0, 0,
        pContext->m_pNewViewClass, CSize(240, 0), pContext))
    {
        TRACE(_T("Failed to create first pane\n"));
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(0, 1,
        RUNTIME_CLASS(CStorageView), CSize(0, 0), pContext))
    {
        TRACE(_T("Failed to create second pane\n"));
        return FALSE;
    }

//    m_pStgEditView = (CStgEditView*)m_wndSplitter.GetPane(0, 0) ;
//    m_pContentsView = (CContentsView*)m_wndSplitter.GetPane(0,1) ;

    // activate the input view
    SetActiveView((CView*)m_wndSplitter.GetPane(0, 0));
    m_wndSplitter.SetColumnInfo( 0, 240, 0 ) ;

    return TRUE;
}

void CMainFrame::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	((CContentsView*)m_wndSplitter.GetPane(0,1))->OnUpdateEditCopy(pCmdUI) ;
}

void CMainFrame::OnEditCopy() 
{
	((CContentsView*)m_wndSplitter.GetPane(0,1))->OnEditCopy() ;	
}
