// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "winprint.h"

#include "mainfrm.h"
#include "queview.h"
#include "cfglstvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int CMainFrame::m_nDefCmdShow = SW_SHOWMAXIMIZED;
int CMainFrame::m_nDefCmdShowOld = SW_SHOWMAXIMIZED;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_DROPFILES()
    ON_WM_SIZE()
    ON_WM_INITMENUPOPUP()
    ON_WM_SYSCOMMAND()
    ON_COMMAND(ID_OPTIONS_PREFERENCES, OnOptionsPreferences)
    //}}AFX_MSG_MAP
    // Global help commands
    ON_COMMAND(ID_HELP_INDEX, CFrameWnd::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CFrameWnd::OnHelpUsing)
    ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpIndex)
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

static UINT BASED_CODE indicators[] =
{
    ID_SEPARATOR,           // status line indicator
//    ID_INDICATOR_CAPS,
//    ID_INDICATOR_NUM,
//    ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    m_pConfigListView = NULL ;
    m_pQueueView = NULL ;

    m_fSaveMinPos = FALSE ;
    m_fTopMost = FALSE ;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::ActivateFrame(int nCmdShow)
{
    if (nCmdShow == -1)
        nCmdShow = m_nDefCmdShow;   // use our default
    CFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::Initialize()
{
    CString szSection ;
    CString szShowCmd ;
    szSection.LoadString( IDS_INI_CONFIG ) ;
    szShowCmd.LoadString( IDS_INI_SHOWCMD ) ;

    m_nDefCmdShow = AfxGetApp()->GetProfileInt(szSection, szShowCmd, m_nDefCmdShow);
    m_nDefCmdShowOld = m_nDefCmdShow;
}

void CMainFrame::Terminate()
{
    CString szSection ;
    CString szShowCmd ;
    szSection.LoadString( IDS_INI_CONFIG ) ;
    szShowCmd.LoadString( IDS_INI_SHOWCMD ) ;

    if (m_nDefCmdShow != m_nDefCmdShowOld)
    {
        AfxGetApp()->WriteProfileInt(szSection, szShowCmd, m_nDefCmdShow);
        m_nDefCmdShowOld = m_nDefCmdShow;
    }
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
                CWnd* pParentWnd, CCreateContext* pContext)
{
    // Turn off auto update of title bar
    dwDefaultStyle &= ~((DWORD)FWS_ADDTOTITLE) ;
    BOOL f = CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle,
                pParentWnd, pContext);

    return f ;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.Create(this) ||
        !m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
        !m_wndToolBar.SetButtons(buttons,
          sizeof(buttons)/sizeof(UINT)))
    {
        TRACE("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE("Failed to create status bar\n");
        return -1;      // fail to create
    }

    RestorePosition() ;

    DragAcceptFiles( TRUE ) ;

    AddFrameWnd() ;
    
    return 0;
}

void CMainFrame::OnDestroy()
{   
    RemoveFrameWnd() ;
    CFrameWnd::OnDestroy();
    SavePosition() ;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
     CCreateContext* pContext)
{
    // create a splitter with 2 ros, 1 column
    if (!m_wndSplitter.CreateStatic(this, 2, 1))
    {
        TRACE(_T("Failed to CreateStaticSplitter\n"));
        return FALSE;
    }

    // add the first splitter pane - the default view in column 0
    if (!m_wndSplitter.CreateView(0, 0,
        pContext->m_pNewViewClass, CSize(0, 250), pContext))
    {
        TRACE(_T("Failed to create first pane\n"));
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(1, 0,
        RUNTIME_CLASS(CQueueView), CSize(0, 0), pContext))
    {
        TRACE(_T("Failed to create second pane\n"));
        return FALSE;
    }

    m_pConfigListView = (CConfigListView*)m_wndSplitter.GetPane(0, 0) ;
    m_pQueueView = (CQueueView*)m_wndSplitter.GetPane(1,0) ;

    // activate the input view
    SetActiveView(m_pConfigListView);

    // Modify the system menu
    //
    CMenu* pSysMenu = GetSystemMenu( FALSE ) ;
    ASSERT(pSysMenu) ;
    pSysMenu->AppendMenu( MF_SEPARATOR ) ;
    pSysMenu->AppendMenu( MF_STRING & MF_ENABLED & (m_fSaveMinPos ? MF_CHECKED : MF_UNCHECKED), 
                          ID_SYS_SAVEMINPOS, "S&ave Minimized Position" ) ;
    pSysMenu->AppendMenu( MF_STRING & MF_ENABLED & (m_fTopMost ? MF_CHECKED : MF_UNCHECKED), 
                          ID_SYS_ALWAYSONTOP, "Always on &Top" ) ;

    return TRUE;
}

    
BOOL CMainFrame::SavePosition()
{       
    CString szFile ;
    CString szSection ;
    CString szKey ;
    
    szSection.LoadString( IDS_INI_CONFIG ) ;
    szKey.LoadString( IDS_INI_WNDPOS ) ;

    WINDOWPLACEMENT wp;
    CString szValue ;

    wp.length = sizeof( WINDOWPLACEMENT );
    GetWindowPlacement( &wp );        
                                             
    int n ;                                             
    int nHeight ;
    m_wndSplitter.GetRowInfo( 0, nHeight, n ) ;
              
    LPTSTR p = szValue.GetBuffer( 255 ) ;              
    wsprintf( p, _T("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"),
        wp.showCmd, wp.ptMinPosition.x, wp.ptMinPosition.y,
        wp.ptMaxPosition.x, wp.ptMaxPosition.y,
        wp.rcNormalPosition.left, wp.rcNormalPosition.top,
        wp.rcNormalPosition.right, wp.rcNormalPosition.bottom,
        nHeight, m_fSaveMinPos, m_fTopMost );
        
    szValue.ReleaseBuffer() ;
    theApp.WriteProfileString( szSection, szKey, szValue );
    return TRUE ;
}

BOOL CMainFrame::RestorePosition()
{   
    CString sz ;
    CString szSection ;
    CString szKey ;
    
    szSection.LoadString( IDS_INI_CONFIG ) ;
    szKey.LoadString( IDS_INI_WNDPOS ) ;

    WINDOWPLACEMENT wp;
    int     nConv;

    wp.length = sizeof( WINDOWPLACEMENT );

    sz = theApp.GetProfileString(szSection, szKey, _T("") ) ;
    
    if (sz.IsEmpty())
        return FALSE;

    LPTSTR   lp = (LPTSTR)sz.GetBuffer( 255 );
    
    wp.showCmd = (WORD)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMinPosition.x = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMinPosition.y = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMaxPosition.x = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMaxPosition.y = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.left = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.top = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.right = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.bottom = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;
    
    int nHeight = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    m_fSaveMinPos = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    m_fTopMost = (int)ParseOffNumber( (LPTSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;
        
    m_wndSplitter.SetRowInfo( 0, nHeight, 0 ) ;

    if (m_fSaveMinPos == FALSE)
        wp.ptMinPosition.x = wp.ptMinPosition.y = -1 ;

    wp.showCmd = SW_HIDE ;

    SetWindowPos( m_fTopMost ? &wndTopMost : &wndNoTopMost, 0,0,0,0, 
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE ) ;
        
    return (BOOL)SetWindowPlacement( &wp ) ;
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
// CMainFrame message handlers
void CMainFrame::OnSysColorChange()
{
    Ctl3dColorChange() ;
    theApp.LoadBitmaps( TRUE ) ;
    CFrameWnd::OnSysColorChange();
}                 

void CMainFrame::OnDropFiles(HDROP hDropInfo)
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

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
    CFrameWnd::OnSize(nType, cx, cy);
    
    if (!IsWindowVisible())
        return;

    switch (nType)
    {
    case SIZE_MINIMIZED:
        m_nDefCmdShow = SW_SHOWMINIMIZED;
        break;
    case SIZE_MAXIMIZED:
        m_nDefCmdShow = SW_SHOWMAXIMIZED;
        break;
    case SIZE_RESTORED:
        m_nDefCmdShow = SW_SHOWNORMAL;
        break;
    }
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
    CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    if (bSysMenu)
    {
        pPopupMenu->CheckMenuItem( ID_SYS_SAVEMINPOS, MF_BYCOMMAND | (m_fSaveMinPos ? MF_CHECKED : MF_UNCHECKED) ) ;
        pPopupMenu->CheckMenuItem( ID_SYS_ALWAYSONTOP, MF_BYCOMMAND | (m_fTopMost ? MF_CHECKED : MF_UNCHECKED) ) ;
    }    
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
    switch(nID)
    {
        case ID_SYS_SAVEMINPOS:
            m_fSaveMinPos = !m_fSaveMinPos ;
        break ;
        
        case ID_SYS_ALWAYSONTOP:
            m_fTopMost = !m_fTopMost ;
            SetWindowPos( m_fTopMost ? &wndTopMost : &wndNoTopMost, 0,0,0,0, 
                SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE ) ;
        break ;
    }
    
    CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnOptionsPreferences() 
{
   // get current font description
   CFont* pFont = theApp->m_pFont ;
   LOGFONT lf;
   if (pFont != NULL)
       pFont->GetObject(sizeof(LOGFONT), &lf);
   else
       ::GetObject(GetStockObject(ANSI_VAR_FONT), sizeof(LOGFONT), &lf);

    CFontDialog dlg( &lf, CF_ENABLETEMPLATE | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT, NULL, this );
    dlg.m_cf.lpTemplateName = MAKEINTRESOURCE(IDD_PREFERENCES) ;
    dlg.m_cf.hInstance = AfxGetInstanceHandle() ;
    if (dlg.DoModal() == IDOK)
    {
        // switch to new font.
        m_font.DeleteObject();
        if (m_font.CreateFontIndirect(&lf))
        {
            CWaitCursor wait;
            SetFont(&m_font);
            m_lfDefFont = lf;

            if (GetPrinterFont() == NULL)
            {
                // notify container that content has changed
                GetDocument()->UpdateAllItems(NULL);
            }
        }
    }

    
}
