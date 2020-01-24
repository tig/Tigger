// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "colsamp.h"

#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)
#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
    // Global help commands
    ON_COMMAND(ID_HELP_INDEX, CMDIFrameWnd::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CMDIFrameWnd::OnHelpUsing)
    ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
    // same order as in the bitmap 'toolbar.bmp'
    ID_FILE_NEW,
    ID_FILE_OPEN,
    ID_FILE_SAVE,
        ID_SEPARATOR,
    ID_EDIT_ADDITEM,
    ID_EDIT_CHANGEITEM,
    ID_EDIT_REMOVEITEM,
        ID_SEPARATOR,
    ID_APP_ABOUT,
    ID_CONTEXT_HELP,
};

static UINT BASED_CODE indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

#include <ctype.h>
// Very useful API that parses a LONG out of a string, updating
// the string pointer before it returns.
// 
LONG WINAPI ParseOffNumber( LPSTR FAR *lplp, LPINT lpConv )
{
    LPSTR lp = *lplp;
    LONG  lValue=0;
    int   sign=1;

    while( isspace(*lp) )
        lp++;
    if( *lp=='-' )
    {               
        sign = -1;
        lp++;
    }
    if (*lp=='\0')
    {
        *lpConv = FALSE;
        return 0L;
    }

    // Is it a decimal number
    if( _fstrnicmp( lp, (LPSTR)"0x",2) )
    {
        
        while( isdigit(*lp) )
        {
            lValue *= 10;
            lValue += (*lp - '0');
            lp++;
        }
    }    
    else
    {
        lp+=2;

        while( isxdigit(*lp) )
        {
            lValue *= 16;
            if( isdigit(*lp) )
                lValue += (*lp - '0');
            else
                lValue += (toupper(*lp) - 'A' + 10);
            lp++;
        }
    }
    while( isspace(*lp) )
        lp++;

    lValue *= (long)sign;

    if (*lp==',')
    {
        lp++;
        while( isspace(*lp) )
            lp++;
        *lplp = lp;
        if (lpConv)
            *lpConv = (int)TRUE;
    }
    else
    {
        *lplp = lp;
        if (lpConv)
            *lpConv = (int)(*lp=='\0');
    }
    return lValue;

} //*** ParseOffNumber

BOOL SavePosition( CWnd* pWnd, CString szKey  )
{   
    CString szFile ;
    CString szSection ;
    
    szSection.LoadString( IDS_INI_CONFIG ) ;

    WINDOWPLACEMENT wp;
    CString szValue ;

    wp.length = sizeof( WINDOWPLACEMENT );
    pWnd->GetWindowPlacement( &wp );        
                                             
    LPSTR p = szValue.GetBuffer( 255 ) ;              
    wsprintf( p, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
        wp.flags, wp.showCmd, wp.ptMinPosition.x, wp.ptMinPosition.y,
        wp.ptMaxPosition.x, wp.ptMaxPosition.y,
        wp.rcNormalPosition.left, wp.rcNormalPosition.top,
        wp.rcNormalPosition.right, wp.rcNormalPosition.bottom );
        
    szValue.ReleaseBuffer() ;
    AfxGetApp()->WriteProfileString( szSection, szKey, szValue );
    return TRUE ;
}

BOOL RestorePosition( CWnd* pWnd, CString szKey )
{   
    CString sz ;
    CString szSection ;
    
    szSection.LoadString( IDS_INI_CONFIG ) ;

    WINDOWPLACEMENT wp;
    int     nConv;

    wp.length = sizeof( WINDOWPLACEMENT );

    sz = AfxGetApp()->GetProfileString(szSection, szKey, "" ) ;
    
    if (sz.IsEmpty())
        return FALSE;

    LPSTR   lp = (LPSTR)sz.GetBuffer( 255 );

    wp.flags = (WORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;
    
    wp.showCmd = (WORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!pWnd->IsWindowVisible())
        wp.showCmd = SW_HIDE ;
        
    if (!nConv)
        return FALSE;

    wp.ptMinPosition.x = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMinPosition.y = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMaxPosition.x = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.ptMaxPosition.y = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.left = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.top = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.right = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    wp.rcNormalPosition.bottom = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return FALSE;

    pWnd->SetWindowPlacement( &wp ) ;

    return TRUE ;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
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

    CString szKey ;
    szKey.LoadString( IDS_INI_MAINWNDPOS ) ;
    RestorePosition( this, szKey ) ;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnDestroy()
{
    CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnClose()
{
    if (IsWindowVisible())
    {
        CString szKey ;
        szKey.LoadString( IDS_INI_MAINWNDPOS ) ;
        SavePosition( this, szKey) ;            
    }
    CMDIFrameWnd::OnClose();
}
