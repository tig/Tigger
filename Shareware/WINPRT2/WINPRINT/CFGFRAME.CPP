// mainfrm.cpp : implementation of the CConfigFrame class
//

#include "stdafx.h"
#include "itsybits.h"

#include "winprint.h"
#include "winprdoc.h"
#include "cfgframe.h"
#include "cfgview.h"
#include "cntritem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigFrame

IMPLEMENT_DYNCREATE(CConfigFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CConfigFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CConfigFrame)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_DROPFILES()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_INITMENUPOPUP()
    //}}AFX_MSG_MAP
    // Global help commands
    ON_COMMAND(ID_HELP_INDEX, CFrameWnd::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CFrameWnd::OnHelpUsing)
    ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpIndex)
    ON_COMMAND(ID_SYS_SAVEMINPOS, OnSysSaveMinPos)
    ON_COMMAND(ID_SYS_ALWAYSONTOP, OnSysAlwaysOnTop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
    // same order as in the bitmap 'toolbar.bmp'
        ID_SEPARATOR,
    ID_APP_ABOUT,
    ID_CONTEXT_HELP,
};


/////////////////////////////////////////////////////////////////////////////
// CConfigFrame construction/destruction

CConfigFrame::CConfigFrame()
{
    m_fInitialUpdate = FALSE ;
}

CConfigFrame::~CConfigFrame()
{
}

void CConfigFrame::ActivateFrame(int nCmdShow)
    // nCmdShow is the normal show mode this frame should be in
{   
    if (m_fInitialUpdate == TRUE) return ;

    CFrameWnd::ActivateFrame(nCmdShow ) ;
}

BOOL CConfigFrame::Create(LPCSTR lpszClassName,
    LPCSTR lpszWindowName,
    DWORD dwStyle,
    const RECT& rect,
    CWnd* pParentWnd,
    LPCSTR lpszMenuName,
    DWORD dwExStyle,
    CCreateContext* pContext)
{
    m_strTitle = lpszWindowName;    // save title for later

    if (!CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle,
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, (LPSTR)pContext))
    {
        TRACE0("Warning: failed to create CFrameWnd\n");
        return FALSE;
    }

    return TRUE;
}

BOOL CConfigFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
    CWnd* pParentWnd, CCreateContext* pContext)
{
    // only do this once
    ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

    m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

    if (m_strTitle.IsEmpty())
    {
        CString strFullString;
        if (strFullString.LoadString(nIDResource))
            AfxExtractSubString(m_strTitle, strFullString, 0);    // first sub-string
    }
    
    dwDefaultStyle |= IBS_HORZCAPTION ;
    if (!Create(GetIconWndClass(dwDefaultStyle, nIDResource),
      m_strTitle, dwDefaultStyle, rectDefault,
      pParentWnd, NULL, 0L, pContext))
        return FALSE;   // will self destruct on failure normally

    return TRUE;
}

int CConfigFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

#if 0
    if (!m_wndToolBar.Create(this) ||
        !m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
        !m_wndToolBar.SetButtons(buttons,
          sizeof(buttons)/sizeof(UINT)))
    {
        TRACE("Failed to create toolbar\n");
        return -1;      // fail to create
    }
#endif
    // Modify the system menu
    //
    CMenu* pSysMenu = GetSystemMenu( FALSE ) ;
    ASSERT(pSysMenu) ;
    pSysMenu->AppendMenu( MF_SEPARATOR ) ;
    pSysMenu->AppendMenu( MF_STRING & MF_ENABLED & MF_UNCHECKED, 
                          ID_SYS_SAVEMINPOS, "S&ave Minimized Position" ) ;
    pSysMenu->AppendMenu( MF_STRING & MF_ENABLED & MF_UNCHECKED, 
                          ID_SYS_ALWAYSONTOP, "Always on &Top" ) ;

    DragAcceptFiles( TRUE ) ;

    AddFrameWnd() ;

    ibSetCaptionSize( this->m_hWnd, GetSystemMetrics( SM_CYCAPTION ) / 2 + 6 ) ;
    
    return 0;
}

void CConfigFrame::OnDestroy()
{   
    TRACE("Destroying CConfigFrame.\n") ;

    RemoveFrameWnd() ;
    
    CFrameWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CConfigFrame diagnostics

#ifdef _DEBUG
void CConfigFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CConfigFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigFrame message handlers
void CConfigFrame::OnSysColorChange()
{
    Ctl3dColorChange() ;
    theApp.LoadBitmaps( TRUE ) ;
    CFrameWnd::OnSysColorChange();
}                 

void CConfigFrame::OnDropFiles(HDROP hDropInfo)
{   
    UINT    cb ;
    if (cb = DragQueryFile( hDropInfo, 0, NULL, 0))
    {
        LPTSTR lpsz = new TCHAR[cb+2] ;
        ASSERT( lpsz ) ;
        
//        if (DragQueryFile( hDropInfo, 0, lpsz, cb+2 ))
//            m_pObjListView->DoBindToFile( lpsz ) ;
        
        delete lpsz ;
    }
    CFrameWnd::OnDropFiles(hDropInfo);
}

void CConfigFrame::OnClose() 
{
    DestroyWindow();
}

void CConfigFrame::OnSize(UINT nType, int cx, int cy) 
{
    CFrameWnd::OnSize(nType, cx, cy);
  
}

void CConfigFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
    CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

//    if (bSysMenu)
    {   
        CConfigView* pView = (CConfigView*)GetActiveView() ;
        if (pView && pView->m_pItem)
        {
            pPopupMenu->CheckMenuItem( ID_SYS_SAVEMINPOS, MF_BYCOMMAND | (pView->m_pItem->m_fSaveMinPos ? MF_CHECKED : MF_UNCHECKED) ) ;
            pPopupMenu->CheckMenuItem( ID_SYS_ALWAYSONTOP, MF_BYCOMMAND | (pView->m_pItem->m_fTopMost ? MF_CHECKED : MF_UNCHECKED) ) ;
        }            
    }    
}

void CConfigFrame::OnSysSaveMinPos()
{
    CConfigView* pView = (CConfigView*)GetActiveView() ;
    if (pView && pView->m_pItem)
        pView->m_pItem->m_fSaveMinPos = !pView->m_pItem->m_fSaveMinPos ;

}

void CConfigFrame::OnSysAlwaysOnTop()
{            
    CConfigView* pView = (CConfigView*)GetActiveView() ;
    if (pView && pView->m_pItem)
    {
        pView->m_pItem->m_fTopMost = !pView->m_pItem->m_fTopMost ;
        SetWindowPos( pView->m_pItem->m_fTopMost ? &wndTopMost : &wndNoTopMost, 0,0,0,0, 
            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE ) ;
    }
}

LRESULT CConfigFrame::WindowProc(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{   
    if (uiMsg == WM_COMMAND)
        DefWindowProc( uiMsg, wParam, lParam ) ;
        
    return CFrameWnd::WindowProc( uiMsg, wParam, lParam ) ;
}

LRESULT CConfigFrame::DefWindowProc(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    return ibDefWindowProc( this->m_hWnd, uiMsg, wParam, lParam ) ;
}
