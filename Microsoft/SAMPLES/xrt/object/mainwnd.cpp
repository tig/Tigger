// mainwnd.cpp : implementation file
//

#include "dataobj.h"
#include "xrt_obj.h"
#include "mainwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
                   
LONG WINAPI ParseOffNumber( LPSTR FAR *lplp, LPINT lpConv ) ;

/////////////////////////////////////////////////////////////////////////////
// CMainWnd

IMPLEMENT_DYNCREATE(CMainWnd, CFrameWnd)

CMainWnd::CMainWnd()
{   
    MessageBeep( 1 ) ;                
    CString sz ;
    sz.LoadString( AFX_IDS_APP_TITLE ) ;
    Create( NULL, sz, WS_OVERLAPPEDWINDOW, rectDefault ) ;
    RestorePosition() ;
}

CMainWnd::~CMainWnd()
{
}

BOOL CMainWnd::SavePosition()
{       
    CString szFile ;
    CString szSection ;
    CString szKey ;
    
    szFile.LoadString( IDS_INIFILE ) ;
    szSection.LoadString( IDS_INI_CONFIG ) ;
    szKey.LoadString( IDS_INI_WNDPOS ) ;

    WINDOWPLACEMENT wp;
    CString szValue ;

    wp.length = sizeof( WINDOWPLACEMENT );
    GetWindowPlacement( &wp );
              
    LPSTR p = szValue.GetBuffer( 255 ) ;              
    wsprintf( p, "%d, %d, %d, %d, %d, %d, %d, %d, %d",
        wp.showCmd, wp.ptMinPosition.x, wp.ptMinPosition.y,
        wp.ptMaxPosition.x, wp.ptMaxPosition.y,
        wp.rcNormalPosition.left, wp.rcNormalPosition.top,
        wp.rcNormalPosition.right, wp.rcNormalPosition.bottom );
        
    szValue.ReleaseBuffer() ;
    WritePrivateProfileString( szSection, szKey, szValue, szFile );
    return TRUE ;
}

BOOL CMainWnd::RestorePosition()
{   
    CString szFile ;
    CString szSection ;
    CString szKey ;
    
    szFile.LoadString( IDS_INIFILE ) ;
    szSection.LoadString( IDS_INI_CONFIG ) ;
    szKey.LoadString( IDS_INI_WNDPOS ) ;

    WINDOWPLACEMENT wp;
    char    szBuffer[ 255 ];
    LPSTR   lp = (LPSTR)szBuffer;
    int     nConv;

    wp.length = sizeof( WINDOWPLACEMENT );

    if (!GetPrivateProfileString(szSection, szKey, "", lp, 255, szFile))
        return FALSE;

    wp.showCmd = (WORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
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

    // Always strip off minimize. 
    //
    if (wp.showCmd == SW_SHOWMINIMIZED)
        wp.showCmd = SW_SHOWNORMAL ;
        
    return (BOOL)SetWindowPlacement( &wp ) ;
}

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
    //{{AFX_MSG_MAP(CMainWnd)
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers


void CMainWnd::OnDestroy()
{
    CFrameWnd::OnDestroy();
    
    // TODO: Add your message handler code here
    SavePosition() ;
}


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

