// headerse.cpp : implementation file
//

#include "stdafx.h"
#include <ctype.h>
#include "winprt20.h"
#include "wintime.h"
#include "control.h"
#include "headerse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int MAX_SECTIONSTRLEN     = 4096;
const int MAX_UNCSHARELEN       = 256 ;

DWORD WINAPI Div( DWORD n, DWORD d )
{
    DWORD  l ;

    l = n / d ;
    if ( ((n % d) != 0) && ((n % d) >= (d / 2)))
        l++ ;

    return l ;
}    

UINT myWNetGetMachineName( LPSTR szMachine, UINT FAR *lpnBufferSize ) ;
UINT myWNetGetUser( LPSTR szUser, UINT FAR *lpnBufferSize ) ;
VOID WINAPI AnsiPathToOEM( LPSTR szPath );
BOOL WINAPI IsWild( LPSTR lpszPath );
VOID WINAPI AddBackslash( LPSTR lpszPath );
VOID WINAPI StripBackslash( LPSTR lpszPath );
VOID WINAPI StripFilespec( LPSTR lpszPath );
VOID WINAPI StripPath( LPSTR lpszPath );
LPSTR WINAPI GetExtension( LPSTR lpszFile );

/////////////////////////////////////////////////////////////////////////////
// CHeaderSection

IMPLEMENT_DYNCREATE(CHeaderSection, CControlProperty)

// This macro is defined in cprop.h
IMPLEMENT_CONTROLPROP(CHeaderSection,
    0x2a50D, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46)

static const FONTDESC g_fdDefault =
    { sizeof(FONTDESC), "Times New Roman", FONTSIZE(8), FW_NORMAL, ANSI_CHARSET, FALSE, FALSE, FALSE };

CHeaderSection::CHeaderSection( ) :
    m_Font( &m_xFontNotify ) 
{
    m_pHeader = NULL ;
    EnableAutomation();
    OnResetState() ;
    
    m_Font.InitializeFont( &g_fdDefault ) ;
    

    // REVIEW:  Use AfxLockApp?
}

CHeaderSection::~CHeaderSection()
{
    // REVIEW:  Use AfxUnLockApp?
}

CString& CHeaderSection::GetExpandedString() 
{
    LPSTR   lpsz = NULL ;
    LPSTR   pS, pD ;
    char    c ;

    lpsz = (LPSTR)GlobalAllocPtr( GHND, MAX_SECTIONSTRLEN ) ;
    if (lpsz)
    {
        for (pS = m_strText.GetBuffer(MAX_SECTIONSTRLEN), pD = lpsz ; *pS ; pS++ )
        {
            if (*pS == '&' && *(pS+1) != '\0')
                pD = ExpandMeta( c = *++pS, pD ) ;
            else
            {
                *pD = *pS ;
                *++pD = '\0' ;
            }
        }
        m_strText.ReleaseBuffer(-1) ;

        CString str(lpsz) ;
        GlobalFreePtr( lpsz ) ;
        return str ;
    }
    
    return CString("") ;
}


/* Expands the HF section text in lpszIn.
 *
 * Calls back into the filetype driver to obtain components where
 * needed.
 *
 * Section text specifiers can have the following macros 
 * in them:
 *
 *      &p -    Page number, taken out of lpJob stucture
 *      &P -    Total number of pages, taken out of lpJob structure but
 *              not calculated until first needed.
 *
 *      &d -    Current date (short format).
 *      &D -    Current date (long format).
 *
 *      &t -    Current time (no seconds).
 *      &T -    Current time (incl. seconds).
 *
 *      &r -    Revised date (short format).
 *      &R -    Revised date (long format).
 *
 *      &v -    Revised time (no seconds).
 *      &V -    Revised time (incl. seconds).
 *
 *      &s -    Size of file in Kbytes. 
 *      &S -    Size of file in bytes. 
 *
 *      &n -    Device name of printer job is being printed on ("HP LaserJet").
 *      &N -    Name of output device job is being printed on ("LPT1:" or 
 *              "\\SERVER\SHARE (LPT1)").
 *
 *      && -    Ampersand.
 *
 *      // for all of the following, an upper-case equivelent would produce
 *      // an upper case string.
 *
 *      &f -    Full path and filename.
 *
 *      &b -    Basename.  Everything but the extension. (i.e. "c:\foo\bar\hello")
 *
 *      &i -    Directory.  The directory component.  This does not include the
 *              following backslash (i.e. "\foo\bar").
 *
 *      &e -    Extension.  Begins with the '.'.  (i.e. ".c")
 *
 *      &h -    Path. The path to the file including drive and trailing
 *              backslash. ("c:\foo\bar\" or "\\server\share\foo\bar\")
 *
 *      &o -    Root.  The filename, less the extension. ("hello")
 *
 *      &l -    Volume.  The drive.  Typically a letter and a colon, but
 *              may also be a UNC style path (i.e. "\\server\share").
 * 
 *      &u -    User name.
 *
 *      &m -    Machine name
 */
LPSTR CHeaderSection::ExpandMeta( char c, LPSTR pD )
{
    LPSTR lp ;
    FILETIME fileTime ;

    switch (c)
    {
        case 'p':   // Page number, taken out of lpJob stucture
            wsprintf( pD, "%d", m_pHeader->m_pControl->m_Page.GetPhysicalPageNum() ) ;
        break ;

        case 'P':   // Total number of pages
            wsprintf( pD, "%d", m_pHeader->m_pControl->m_Page.GetNumPhysicalPages() ) ;
        break ;

        case 'd':   // Current date (short format).
        case 'D':   // Current date (long format).
            TimeGetCurDate( pD, c == 'd' ? DATE_SHORTDATE : DATE_DEFAULT ) ;
        break ;

        case 't':   // Current time (no seconds).
        case 'T':   // Current time (incl. seconds).
            TimeGetCurTime( pD, c == 't' ? TIME_NOSECONDS : 0 ) ;

        case 'r':   // Revised date (short format).
        case 'R':   // Revised date (long format).
            m_pHeader->m_pControl->m_Input.GetFileTime( &fileTime ) ;
            TimeFormatDateEx( pD, &fileTime, c == 'r' ? DATE_SHORTDATE : 0 ) ;            
        break ;

        case 'v':   // Revised time (no seconds).
        case 'V':
            m_pHeader->m_pControl->m_Input.GetFileTime( &fileTime ) ;
            TimeFormatTimeEx( pD, &fileTime, c == 'r' ? DATE_SHORTDATE : 0 ) ;            
        break ;

        case 's':   // Size of file in Kbytes.
            wsprintf( pD, "%lu", Div(m_pHeader->m_pControl->m_Input.GetFileSize(),1024L ) );
        break ;

        case 'S':   // Size of file in bytes. 
            wsprintf( pD, "%lu", m_pHeader->m_pControl->m_Input.GetFileSize() );
        break ;

        case 'n':   // Device name of printer
            lstrcpy( pD, m_pHeader->m_pControl->m_strPrinter ) ;
        break ;

        case 'N':   // Name of output device 
        {
            char  szOutput[MAX_UNCSHARELEN] ;
            UINT  cb = MAX_UNCSHARELEN ;

            if (WN_SUCCESS == WNetGetConnection( (LPSTR)(LPCSTR)m_pHeader->m_pControl->m_strPrinterOutput, szOutput, &cb ))
                wsprintf( pD, "%s (%s)", (LPSTR)szOutput, (LPCSTR)m_pHeader->m_pControl->m_strPrinterOutput ) ;
            else
                lstrcpy( pD, m_pHeader->m_pControl->m_strPrinterOutput ) ;
        }
        break ;

        case 'u':
        case 'U':
        {
            UINT cb = 128 ;

            cb = myWNetGetUser( pD, &cb ) ;
            goto ChangeCase ;
        }
        break ;

        case 'm':
        case 'M':
        {
            UINT cb = 128 ;

            cb = myWNetGetMachineName( pD, &cb ) ;
            goto ChangeCase ;
        }
        break ;

        case 'f':   // Full path and filename.
        case 'F':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<No file specified>" ) ;
            else
            {
                lstrcpy( pD, m_pHeader->m_pControl->m_Input.GetFileName() ) ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'b':   // Basename.  Everything but the extension.
        case 'B':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, m_pHeader->m_pControl->m_Input.GetFileName() ) ;
                lp = _fstrrchr( pD, '.' ) ;
                if (lp)
                    *lp = '\0' ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'i':   // Directory.  The directory component. 
        case 'I':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, m_pHeader->m_pControl->m_Input.GetFileName() ) ;
                StripFilespec( pD ) ;
                AddBackslash( pD ) ;

                #pragma message( "NEED!: Strip drive component off" )

                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'e':   // Extension.
        case 'E':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD+1, m_pHeader->m_pControl->m_Input.GetFileName() ) ;
                GetExtension( pD+1 ) ;
                *pD = '.' ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'h':   // Path.
        case 'H':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, m_pHeader->m_pControl->m_Input.GetFileName() ) ;
                StripFilespec( pD ) ;
                AddBackslash( pD ) ;

                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'o':   // Root.  
        case 'O':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, m_pHeader->m_pControl->m_Input.GetFileName() ) ;
                StripPath( pD ) ;
                lp = _fstrrchr( pD, '.' ) ;
                if (lp)
                    *lp == '\0' ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'l':   // Volume.  The drive. 
        case 'L':
            if (m_pHeader->m_pControl->m_Input.GetFileName().GetLength() == 0)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, m_pHeader->m_pControl->m_Input.GetFileName()) ;
                StripFilespec( pD ) ;

                #pragma message( "NEED!: Strip to get drive component" )

                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case '&':   // Ampersand.
            *pD = '&' ;
            *++pD = '\0' ;
        return pD ;

        default:    // ignore it!
        return pD ;
    } 

    return pD += lstrlen( pD ) ;

ChangeCase:
// REVIEW:  What if it's an LFN and has mixed case?  Idea: only AnsiUpper.
    if (!IsCharUpper(c))
        AnsiLower( pD ) ;
    else
        AnsiUpper( pD ) ;

    return pD += lstrlen( pD ) ;
}    

UINT myWNetGetUser( LPSTR szUser, UINT FAR *lpnBufferSize )
{
    typedef UINT (WINAPI *FNWNETGETUSER)( LPSTR, UINT FAR* ) ;
    FNWNETGETUSER lpfn ;
    char    szNetDrv[16] ;      

    if (!::GetPrivateProfileString( "Boot", "network.drv", "", szNetDrv, 16, "SYSTEM.INI" ))
    {
        TRACE( "No network installed\n" ) ;
        return NULL ;
    }
    
    lpfn = (FNWNETGETUSER)GetProcAddress( GetModuleHandle( szNetDrv ), (LPSTR)MAKEINTRESOURCE( 16 )) ;
    if (lpfn)
    {
        (*lpfn)( szUser, lpnBufferSize ) ;

        if (szUser[0] == '\0')
        {
            /* Try a different tact...
             */
        }
        return 1 ;
    }
    else
    {
        TRACE("'%s, WNetGetUser' could not be found.  Not supported?\n", (LPSTR)szNetDrv ) ;
        return 0 ;
    }
}

UINT myWNetGetMachineName( LPSTR szMachine, UINT FAR *lpnBufferSize )
{
    char    szNetDrv[16] ;      
    UNUSED lpnBufferSize ;

    if (!::GetPrivateProfileString( "Boot", "network.drv", "", szNetDrv, 16, "SYSTEM.INI" ))
    {
        TRACE("No network installed\n" ) ;
        return NULL ;
    }

    _asm
    {
        push    ds
        lds     dx, szMachine    ; ds:dx -> lpBuffer

        mov     ax, 5E00h       ; get machine name
        int     21h
        jnc     gmnExit         ; just return cx on success
        xor     cx, cx          ; return failure

gmnExit:
        mov     ax, cx          ; return CX
        pop     ds
    }

    //StripOffWhiteSpace( szMachine ) ;

    return 0 ;
}

/* Filename manipulation functions
 */
/* Given an ANSI path, convert it to upper-case, then to OEM so DOS
 * is happy.
 */
VOID WINAPI AnsiPathToOEM( LPSTR szPath )
{
    AnsiUpper( szPath ) ;
    AnsiToOem( szPath, szPath ) ;
}


BOOL WINAPI IsWild( LPSTR lpszPath )
{
    while (*lpszPath)
    {
        if (*lpszPath == '?' || *lpszPath == '*')
            return TRUE ;
        lpszPath++ ;
    }
    
    return FALSE ;
}

/* Ensures that a path ends with a backslash.
 */
VOID WINAPI AddBackslash( LPSTR lpszPath )
{
    if (lpszPath[lstrlen( lpszPath ) - 1] == '\\')
        return ;
    lstrcat( lpszPath, "\\" ) ;
}

/* Removes a trailing backslash from a proper directory name UNLESS it is
 * the root directory.  Assumes a fully qualified directory path.
 */
VOID WINAPI StripBackslash( LPSTR lpszPath )
{
    register UINT len ;

    len = lstrlen( lpszPath ) - 1 ;

    if ((len == 2) || (lpszPath[len] != '\\'))
        return ;

    lpszPath[len] = NULL ;
}

/* Remove the filespec portion from a path (including the backslash).
 */
VOID WINAPI StripFilespec( LPSTR lpszPath )
{
    LPSTR     p;

    p = lpszPath + lstrlen( lpszPath ) ;
    while ((*p != '\\') && (*p != ':') && (p != lpszPath))
        p-- ;

    if (*p == ':')
        p++ ;

    /* Don't strip backslash from root directory entry. */
    if (p != lpszPath)
    {
        if ((*p == '\\') && (*(p-1) == ':'))
            p++ ;
    }

    *p = NULL ;
}

/* Extract only the filespec portion from a path. 
 */
VOID WINAPI StripPath( LPSTR lpszPath )
{
    LPSTR     p ;

    // make sure there's no whitespace! (helps get rid of command line args)
    for (p = lpszPath ; *p != '\0' ; p++)
        if (isspace(*p))
        {
            *p = '\0' ;
            goto outahere;
        }
outahere:

    p = lpszPath + lstrlen(lpszPath);
    while ((*p != '\\') && (*p != ':') && (p != lpszPath))
        p-- ;

    if (p != lpszPath)
        p++ ;

    if (p != lpszPath)
        lstrcpy( lpszPath, p ) ;
}

/* Returns the extension part of a filename. This does not
 * include the period.
 */
LPSTR WINAPI GetExtension( LPSTR lpszFile )
{
    LPSTR p, pSave = NULL ;

    p = lpszFile ;
    while (*p)
    {
        if (*p == '.')
            pSave = p ;
        p++ ;
    }

    if (!pSave)
        return p ;

    return pSave + 1 ;
}

BEGIN_MESSAGE_MAP(CHeaderSection, CControlProperty)
    //{{AFX_MSG_MAP(CHeaderSection)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CHeaderSection, CControlProperty)
    //{{AFX_DISPATCH_MAP(CHeaderSection)
    DISP_PROPERTY_NOTIFY(CHeaderSection, "Text", m_strText, OnTextChanged, VT_BSTR)
    DISP_DEFVALUE(CHeaderSection, "Text")
    DISP_PROPERTY_EX_ID(CHeaderSection, "Font", DISPID_FONT, GetFont, SetFont, VT_FONT)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

void CHeaderSection::DoPropExchange(CPropExchange* pPX)
{
    PX_Font( pPX, "Font", m_Font, &g_fdDefault ) ;
    PX_String( pPX, "Text", m_strText, "" ) ;
}

void CHeaderSection::OnResetState()
{

}

STDMETHODIMP_(ULONG) CHeaderSection::XFontNotify::AddRef()
{
    METHOD_PROLOGUE(CHeaderSection, FontNotify)
    return  (ULONG)pThis->ExternalAddRef() ;
}

STDMETHODIMP_(ULONG) CHeaderSection::XFontNotify::Release()
{
    METHOD_PROLOGUE(CHeaderSection, FontNotify)
    return  (ULONG)pThis->ExternalRelease() ;
}         

STDMETHODIMP CHeaderSection::XFontNotify::QueryInterface( REFIID riid, 
                LPVOID FAR *ppvObj)
{
    METHOD_PROLOGUE(CHeaderSection, FontNotify)
    if (IsEqualIID( riid, IID_IUnknown ) || IsEqualIID( riid, IID_IPropertyNotifySink ))
    {
        *ppvObj = this ;
        AddRef() ;
        return NOERROR ;
    }
    return ResultFromScode( E_NOINTERFACE ) ;
}         

STDMETHODIMP CHeaderSection::XFontNotify::OnChanged(DISPID dispid)
{
    METHOD_PROLOGUE( CHeaderSection, FontNotify ) ;
    UNUSED dispid ;
    pThis->OnFontChanged() ;
    return NOERROR ;
}

STDMETHODIMP CHeaderSection::XFontNotify::OnRequestEdit(DISPID dispid)
{
    METHOD_PROLOGUE( CHeaderSection, FontNotify ) ;
    UNUSED dispid ;
    return NOERROR ;
}

/////////////////////////////////////////////////////////////////////////////
// CHeaderSection message handlers
void CHeaderSection::OnTextChanged() 
{
    // TODO: Add notification handler code

}

void CHeaderSection::OnFontChanged()
{

}

LPFONTDISP CHeaderSection::GetFont() 
{
    return m_Font.GetFontDispatch();
}

void CHeaderSection::SetFont(LPFONTDISP newValue) 
{
    m_Font.InitializeFont( &g_fdDefault, newValue ) ;
}


