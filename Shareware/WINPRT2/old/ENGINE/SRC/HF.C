/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  HF 
 *
 *      Module:  HF.C
 * 
 *      This module contians the main entry points for the WinPrint 2.0
 *      engine Header/Footer stuff.  The Header/Footer setup dialog
 *      box is also handled here.
 *
 *   Revisions:  
 *       9/11/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"

#define EXTRA_HF_HEIGHT     4

UINT INTERNAL GetSectionHeight( LPENGINEJOB lpJob, LPENGINEHF_SECT lpEHFS ) ;
LPSTR INTERNAL HandleMeta( LPENGINEJOB lpJob, char c, LPSTR pD, LPSTR pS ) ;

LRESULT CALLBACK fnBtnSubClass( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

UINT INTERNAL myWNetGetUser( LPSTR szUser, UINT FAR *lpnBufferSize ) ;
UINT INTERNAL myWNetGetMachineName( LPSTR szMachine, UINT FAR *lpnBufferSize ) ;
static WNDPROC lpfnBtn = NULL ;

struct _stat gFileInfo ;

/* Returns the height of the header footer specified.
 *
 * We define a bit of extra space...
 */
UINT WINAPI engHFCalcHeight( HENGINEJOB lpJob, LPENGINEHF lpEHF )
{
    UINT    uiL = 0 ;
    UINT    uiC = 0 ;
    UINT    uiR = 0 ;

    VALIDATEPARAM( lpJob ) ;
    VALIDATEPARAM( lpEHF ) ;

    /* If the user has specified a height, then just return that */
    if (lpEHF->dwFlags & ENGHF_NOAUTOHEIGHT)
    {
        DP4( hWDB, "ENGHF_NOAUTOHEIGHT specified" ) ;
        return lpEHF->uiHeight ;
    }

    /* Each Header/Footer has three components: L, R, and C.  These
     * are called sections.  
     * The tallest of the sections determines the height of the entire
     * header/footer.
     */
    uiL = GetSectionHeight( lpJob, &lpEHF->Left ) ;
    uiC = GetSectionHeight( lpJob, &lpEHF->Center ) ;
    uiR = GetSectionHeight( lpJob, &lpEHF->Right ) ;

    return max( max( uiL, uiR ), uiC ) ;
}

UINT  INTERNAL GetSectionHeight( LPENGINEJOB lpJob, LPENGINEHF_SECT lpEHFS )
{
    HFONT   hfnt ;
    LPSTR   lpsz ;
    TEXTMETRIC tm ;
    UINT    nLines = 1 ;
    UINT    nHeight ;

    VALIDATEPARAM( lpJob ) ;
    VALIDATEPARAM( lpEHFS ) ;

    if (lpEHFS->szText[0] == '\0')
    {
        return 0L ;
    }

    /* Count the number of lines...
     */
    for (lpsz = lpEHFS->szText ; *lpsz ; lpsz++ )
    {
        if (*lpsz == '\n')
            nLines++ ;
    }


    hfnt = ReallyCreateFontEx( lpJob->hdc, lpEHFS->szFace, lpEHFS->szStyle, lpEHFS->wPoints, 0 ) ;
    hfnt = SelectObject( lpJob->hdc, hfnt ) ;
    GetTextMetrics( lpJob->hdc, &tm ) ;
    hfnt = SelectObject( lpJob->hdc, hfnt ) ;
    DeleteObject( hfnt ) ;
    
    nHeight = (tm.tmHeight + tm.tmExternalLeading) * nLines + EXTRA_HF_HEIGHT ;

    return (UINT )MulDiv( nHeight, 10000, lpJob->nPixelPer10CMY ) ;
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
LPSTR WINAPI engHFExpand( HENGINEJOB lpJob, LPSTR lpszIn )
{
    LPSTR   lpsz ;
    LPSTR   pS, pD ;
    char    c ;

    /* Assume the result will be at least as long as the input.
     */
    if (lpsz = GlobalAllocPtr( GHND, 4096 ))
    {
        for (pS = lpszIn, pD = lpsz ; *pS ; pS++ )
        {
            if (*pS == '&' && *(pS+1) != '\0')
                pD = HandleMeta( lpJob, c = *++pS, pD, pS) ;
            else
            {
                *pD = *pS ;
                *++pD = '\0' ;
            }
        }
        return lpsz ;
    }

    return NULL ;
}

LPSTR INTERNAL HandleMeta( LPENGINEJOB lpJob, char c, LPSTR pD, LPSTR pS )
{
    LPSTR lp ;

    switch (c)
    {
        case 'p':   // Page number, taken out of lpJob stucture
            wsprintf( pD, "%d", lpJob->nPhysicalPage ) ;
        break ;

        case 'P':   // Total number of pages
            #pragma message( "NEED!: Calculate number of pages" )
//            if (lpJob->nPhysicalPages == 0 && lpJob->lpfnFTD)
//                (*lpEC->lpfnFTD)( FTD_CALCNUMPAGES, (LPARAM)lpJob, (LPARAM)NULL, (LPARAM)NULL ) ;

            if (lpJob->nPhysicalPages == 0)
                wsprintf( pD, "(n/a)" ) ;
            else
                wsprintf( pD, "%d", lpJob->nPhysicalPages ) ;
        break ;

        case 'd':   // Current date (short format).
            TimeGetCurDate( pD, DATE_SHORTDATE ) ;
        break ;

        case 'D':   // Current date (long format).
            TimeGetCurDate( pD, 0 ) ;
        break ;

        case 't':   // Current time (no seconds).
            TimeGetCurTime( pD, TIME_NOSECONDS ) ;
        break ;

        case 'T':   // Current time (incl. seconds).
            TimeGetCurTime( pD, 0 ) ;
        break ;

        case 'r':   // Revised date (short format).
        case 'R':   // Revised date (long format).
            if (lpJob->hFile)
            {
                 _fstat( lpJob->hFile, &gFileInfo) ;
                 if (c == 'r')
                     TimeFormatDate( pD, localtime( &gFileInfo.st_mtime ), DATE_SHORTDATE ) ;
                 else
                     TimeFormatDate( pD, localtime( &gFileInfo.st_mtime ), 0 ) ;
            }
        break ;

        case 'v':   // Revised time (no seconds).
        case 'V':
            if (lpJob->hFile)
            {
                 _fstat( lpJob->hFile, &gFileInfo) ;
                 if (c == 'v')
                     TimeFormatTime( pD, localtime( &gFileInfo.st_mtime ), TIME_NOSECONDS ) ;
                 else
                     TimeFormatTime( pD, localtime( &gFileInfo.st_mtime ), 0 ) ;
            }
        break ;

        case 's':   // Size of file in Kbytes.
            if (lpJob->hFile)
                wsprintf( pD, "%lu", Div( lpJob->dwBytes, 1024 ) ) ;
        break ;

        case 'S':   // Size of file in bytes. 
            if (lpJob->hFile)
                wsprintf( pD, "%lu",  lpJob->dwBytes ) ;
        break ;

        case 'n':   // Device name of printer
            lstrcpy( pD, lpJob->lpEC->szDevice ) ;
        break ;

        case 'N':   // Name of output device 
        {
            char  szOutput[MAX_UNCSHARELEN] ;
            UINT  cb = MAX_UNCSHARELEN ;

            if (WN_SUCCESS == WNetGetConnection( lpJob->lpEC->szOutput, szOutput, &cb ))
                wsprintf( pD, "%s (%s)", (LPSTR)szOutput, (LPSTR)lpJob->lpEC->szOutput ) ;
            else
                lstrcpy( pD, lpJob->lpEC->szOutput ) ;
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
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<No file specified>" ) ;
            else
            {
                lstrcpy( pD, lpJob->lpszFile ) ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'b':   // Basename.  Everything but the extension.
        case 'B':
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, lpJob->lpszFile ) ;
                if (lp = _fstrrchr( pD, '.' ))
                    *lp = '\0' ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'i':   // Directory.  The directory component. 
        case 'I':
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, lpJob->lpszFile ) ;
                StripFilespec( pD ) ;
                AddBackslash( pD ) ;

                #pragma message( "NEED!: Strip drive component off" )

                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'e':   // Extension.
        case 'E':
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD+1, lpJob->lpszFile ) ;
                GetExtension( pD+1 ) ;
                *pD = '.' ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'h':   // Path.
        case 'H':
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, lpJob->lpszFile ) ;
                StripFilespec( pD ) ;
                AddBackslash( pD ) ;

                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'o':   // Root.  
        case 'O':
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, lpJob->lpszFile ) ;
                StripPath( pD ) ;
                if (lp = _fstrrchr( pD, '.' ))
                    *lp == '\0' ;
                OemToAnsi( pD, pD ) ;
                goto ChangeCase ;
            }
        break ;

        case 'l':   // Volume.  The drive. 
        case 'L':
            if (!lpJob->lpszFile)
                lstrcpy( pD, "<n/a>" ) ;
            else
            {
                lstrcpy( pD, lpJob->lpszFile ) ;
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
    if (!isupper(c))
        AnsiLower( pD ) ;
    else
        AnsiUpper( pD ) ;

    return pD += lstrlen( pD ) ;
}    

UINT INTERNAL myWNetGetUser( LPSTR szUser, UINT FAR *lpnBufferSize )
{
    typedef UINT (WINAPI *FNWNETGETUSER)( LPSTR, UINT FAR* ) ;
    FNWNETGETUSER lpfn ;
    char    szNetDrv[16] ;      

    if (!GetPrivateProfileString( "Boot", "network.drv", "", szNetDrv, 16, "SYSTEM.INI" ))
    {
        DP3( hWDB, "No network installed" ) ;
        return NULL ;
    }
    
    if (lpfn = (FNWNETGETUSER)GetProcAddress( GetModuleHandle( szNetDrv ), (LPSTR)MAKEINTRESOURCE( 16 )))
    {
        (*lpfn)( szUser, lpnBufferSize ) ;

        if (szUser[0] == '\0')
        {
            /* Try a different tact...
             */
        }
    }
    else
    {
        DP3( hWDB, "'%s, WNetGetUser' could not be found.  Not supported?", (LPSTR)szNetDrv ) ;
        return 0 ;
    }
}

UINT INTERNAL myWNetGetMachineName( LPSTR szMachine, UINT FAR *lpnBufferSize )
{
    char    szNetDrv[16] ;      

    if (!GetPrivateProfileString( "Boot", "network.drv", "", szNetDrv, 16, "SYSTEM.INI" ))
    {
        DP3( hWDB, "No network installed" ) ;
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

    StripOffWhiteSpace( szMachine ) ;

    if (0) return 0 ;
}


BOOL INTERNAL FontInfoFromWindow( HDC hdc, HFONT hfont, LPENGINEHF_SECT lpEHFS ) ;

LRESULT CALLBACK fnHFConfigure( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPENGINECONFIG lpEC ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            HFONT hfont ;
            HDC   hdc ;
            char  szBuf[MAX_CONFIGNAMELEN + 32] ;

            SetWindowLong( hDlg, DWL_USER, lParam ) ;
            lpEC = (LPENGINECONFIG)lParam ;

            hfont = ReallyCreateFontEx( NULL, "MS Sans Serif", NULL, 8, RCF_NORMAL ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_TEXT1 ), hfont, FALSE ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_TEXT3 ), hfont, FALSE ) ;

            if (hdc = engPrinterCreateDC( hDlg, lpEC ))
            {
                hfont = GetMatchingFont( NULL, hdc, lpEC->Header.Left.szFace, lpEC->Header.Left.szStyle, lpEC->Header.Left.wPoints, 0 ) ;
                SetWindowFont( GetDlgItem( hDlg, IDD_LEFT_H ), hfont, FALSE ) ;

                hfont = GetMatchingFont( NULL, hdc, lpEC->Header.Center.szFace, lpEC->Header.Center.szStyle, lpEC->Header.Center.wPoints, 0 ) ;
                SetWindowFont( GetDlgItem( hDlg, IDD_CENTER_H ), hfont, FALSE ) ;

                hfont = GetMatchingFont( NULL, hdc, lpEC->Header.Right.szFace, lpEC->Header.Right.szStyle, lpEC->Header.Right.wPoints, 0 ) ;
                SetWindowFont( GetDlgItem( hDlg, IDD_RIGHT_H ), hfont, FALSE ) ;

                hfont = GetMatchingFont( NULL, hdc, lpEC->Footer.Left.szFace, lpEC->Footer.Left.szStyle, lpEC->Footer.Left.wPoints, 0 ) ;
                SetWindowFont( GetDlgItem( hDlg, IDD_LEFT_F ), hfont, FALSE ) ;

                hfont = GetMatchingFont( NULL, hdc, lpEC->Footer.Center.szFace, lpEC->Footer.Center.szStyle, lpEC->Footer.Center.wPoints, 0 ) ;
                SetWindowFont( GetDlgItem( hDlg, IDD_CENTER_F ), hfont, FALSE ) ;

                hfont = GetMatchingFont( NULL, hdc, lpEC->Footer.Right.szFace, lpEC->Footer.Right.szStyle, lpEC->Footer.Right.wPoints, 0 ) ;
                SetWindowFont( GetDlgItem( hDlg, IDD_RIGHT_F ), hfont, FALSE ) ;

                DeleteDC( hdc ) ;
            }                        

            SetDlgItemText( hDlg, IDD_LEFT_H, lpEC->Header.Left.szText ) ;
            Edit_LimitText( GetDlgItem( hDlg, IDD_LEFT_H ), MAX_HFTEXTLEN-1 ) ;
            SetDlgItemText( hDlg, IDD_CENTER_H, lpEC->Header.Center.szText ) ;
            Edit_LimitText( GetDlgItem( hDlg, IDD_CENTER_H ), MAX_HFTEXTLEN-1 ) ;
            SetDlgItemText( hDlg, IDD_RIGHT_H, lpEC->Header.Right.szText ) ;
            Edit_LimitText( GetDlgItem( hDlg, IDD_RIGHT_H ), MAX_HFTEXTLEN-1 ) ;

            SetDlgItemText( hDlg, IDD_LEFT_F, lpEC->Footer.Left.szText ) ;
            Edit_LimitText( GetDlgItem( hDlg, IDD_LEFT_F ), MAX_HFTEXTLEN-1 ) ;
            SetDlgItemText( hDlg, IDD_CENTER_F, lpEC->Footer.Center.szText ) ;
            Edit_LimitText( GetDlgItem( hDlg, IDD_CENTER_F ), MAX_HFTEXTLEN-1 ) ;
            SetDlgItemText( hDlg, IDD_RIGHT_F, lpEC->Footer.Right.szText ) ;
            Edit_LimitText( GetDlgItem( hDlg, IDD_RIGHT_F ), MAX_HFTEXTLEN-1 ) ;

            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;

            /* Subclass the buttons so that they don't get focus
             */
            lpfnBtn = SubclassWindow( GetDlgItem( hDlg, IDD_LEFT_H ), fnBtnSubClass ) ;
            SubclassWindow( GetDlgItem( hDlg, IDD_CENTER_H ), fnBtnSubClass ) ;
            SubclassWindow( GetDlgItem( hDlg, IDD_RIGHT_H ), fnBtnSubClass ) ;
            SubclassWindow( GetDlgItem( hDlg, IDD_LEFT_F ), fnBtnSubClass ) ;
            SubclassWindow( GetDlgItem( hDlg, IDD_CENTER_F ), fnBtnSubClass ) ;
            SubclassWindow( GetDlgItem( hDlg, IDD_RIGHT_F ), fnBtnSubClass ) ;

            wsprintf( szBuf, GRCS(IDS_HFCONFIG), (LPSTR)lpEC->szConfigName ) ;
            SetWindowText( hDlg, szBuf ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

            // return TRUE ;    // Focus will be set to first control
            // return FALSE ;   // We set focus
        }
        break ;

        case WM_CTLCOLOR:
            return (BOOL)Ctl3dCtlColorEx( uiMsg, wParam, lParam ) ;
        break ;

        case WM_COMMAND:
        {
            #ifdef WIN32
            WORD wNotifyCode = HIWORD(wParam);  
            WORD wID = LOWORD(wParam);
            HWND hwndCtl = (HWND)lParam;     
            #else
            WORD wNotifyCode = HIWORD(lParam) ;
            WORD wID = wParam ;
            HWND hwndCtl = (HWND)LOWORD(lParam) ;
            #endif

            HWND hwndEdit = (HWND)GetProp( hDlg, MAKEINTATOM( 1 ) ) ;

            if (!(lpEC = (LPENGINECONFIG)GetWindowLong( hDlg, DWL_USER )))
            {
                DASSERT( hWDB, lpEC ) ;
                EndDialog( hDlg, FALSE ) ;
            }
                
            switch (wID)
            {
                case IDD_FONT:
                    if (hwndEdit)
                    {
                        LOGFONT lf;
                        CHOOSEFONT chf;
                    
                        chf.hDC = engPrinterCreateDC( hDlg, lpEC ) ;

                        GetObject( GetWindowFont( hwndEdit ), sizeof( LOGFONT ), (LPSTR)&lf ) ;
                    
                        chf.lStructSize = sizeof( CHOOSEFONT ) ;
                        chf.hwndOwner = hDlg ;
                        chf.lpLogFont = &lf ;
                        chf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SHOWHELP | CF_PRINTERFONTS | CF_FORCEFONTEXIST ;
                        chf.rgbColors = 0L ;
                        chf.lCustData = 0L ;
                        chf.lpfnHook = (UINT (CALLBACK*)(HWND, UINT, WPARAM, LPARAM))NULL ;
                        chf.lpTemplateName = (LPSTR) NULL ;
                        chf.hInstance = (HANDLE) NULL ;
                        chf.iPointSize = 0 ;
                        chf.lpszStyle = (LPSTR)NULL ;
                        chf.nFontType = SCREEN_FONTTYPE ;
                        chf.nSizeMin = 0 ;
                        chf.nSizeMax = 0 ;
                    
                        if (ChooseFont(&chf))
                        {
                            DeleteObject( GetWindowFont( hwndEdit ) ) ;

                            SetWindowFont( hwndEdit, CreateFontIndirect( &lf ), TRUE ) ;
                        }
                                            
                        DeleteDC( chf.hDC ) ;
                    }
                break ;

                case IDD_PAGENUMBER:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&p" ) ;
                break ;

                case IDD_NUMBEROFPAGES:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&n" ) ;
                break ;

                case IDD_CURRENTDATE:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&d" ) ;
                break ;

                case IDD_CURRENTTIME:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&t" ) ;
                break ;

                case IDD_REVISEDDATE:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&r" ) ;
                break ;

                case IDD_REVISEDTIME:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&v" ) ;
                break ;

                case IDD_FULLPATHNAME:
                    if (hwndEdit)
                        Edit_ReplaceSel( hwndEdit, "&f" ) ;
                break ;
                
                case IDOK:
                {
                    HDC hdc ;

                    if (hdc = engPrinterCreateDC( hDlg, lpEC ))
                    {
                        FontInfoFromWindow( hdc, GetWindowFont( GetDlgItem( hDlg, IDD_LEFT_H ) ), &lpEC->Header.Left ) ;
                        FontInfoFromWindow( hdc, GetWindowFont( GetDlgItem( hDlg, IDD_CENTER_H ) ), &lpEC->Header.Center ) ;
                        FontInfoFromWindow( hdc, GetWindowFont( GetDlgItem( hDlg, IDD_RIGHT_H ) ), &lpEC->Header.Right ) ;
        
                        FontInfoFromWindow( hdc, GetWindowFont( GetDlgItem( hDlg, IDD_LEFT_F ) ), &lpEC->Footer.Left ) ;
                        FontInfoFromWindow( hdc, GetWindowFont( GetDlgItem( hDlg, IDD_CENTER_F ) ), &lpEC->Footer.Center ) ;
                        FontInfoFromWindow( hdc, GetWindowFont( GetDlgItem( hDlg, IDD_RIGHT_F ) ), &lpEC->Footer.Right ) ;
        
                        DeleteDC( hdc ) ;
                    }
        
                    GetDlgItemText( hDlg, IDD_LEFT_H, lpEC->Header.Left.szText, MAX_HFTEXTLEN ) ;
                    GetDlgItemText( hDlg, IDD_CENTER_H, lpEC->Header.Center.szText, MAX_HFTEXTLEN ) ;
                    GetDlgItemText( hDlg, IDD_RIGHT_H, lpEC->Header.Right.szText, MAX_HFTEXTLEN ) ;
        
                    GetDlgItemText( hDlg, IDD_LEFT_F, lpEC->Footer.Left.szText, MAX_HFTEXTLEN ) ;
                    GetDlgItemText( hDlg, IDD_CENTER_F, lpEC->Footer.Center.szText, MAX_HFTEXTLEN ) ;
                    GetDlgItemText( hDlg, IDD_RIGHT_F, lpEC->Footer.Right.szText, MAX_HFTEXTLEN ) ;

                    EndDialog (hDlg, TRUE) ;
                }
                break ;

                case IDCANCEL:
                    EndDialog (hDlg, FALSE) ;
                break ;

                case IDD_HELP:
                    DASSERTMSG( hWDB, 0, "Yo! No help yet." ) ;
                break ;


                default :
                   return FALSE ;
            }
        }
        break ;

        case WM_DESTROY:
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_LEFT_H ) ) ) ;
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_RIGHT_H ) ) ) ;
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_CENTER_H ) ) );

            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_LEFT_F ) ) ) ;
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_RIGHT_F ) ) ) ;
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_CENTER_F ) ) ) ;
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_TEXT1 ) ) ) ;

            RemoveProp( hDlg, MAKEINTATOM( 1 ) ) ;

        break ;

        default:
            return FALSE ;

   }
   return TRUE ;
}

LRESULT CALLBACK fnBtnSubClass( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case WM_SETFOCUS:
            SetProp( GetParent( hWnd ), MAKEINTATOM( 1 ), (HANDLE)hWnd ) ;
        break ;
    }

    return CallWindowProc( lpfnBtn, hWnd, uiMsg, wParam, lParam ) ;
}

BOOL INTERNAL FontInfoFromWindow( HDC hdc, HFONT hfont, LPENGINEHF_SECT lpEHFS )
{
    hfont = SelectObject( hdc, hfont ) ;
    GetTextFace( hdc, LF_FACESIZE, lpEHFS->szFace ) ;
    GetTextStyle( hdc, LF_FACESIZE, lpEHFS->szStyle ) ;
    hfont = SelectObject( hdc, hfont ) ;

    hdc = GetDC(NULL) ;
    hfont = SelectObject( hdc, hfont ) ;
    lpEHFS->wPoints = GetTextPointSize( hdc ) ;
    hfont = SelectObject( hdc, hfont ) ;
    ReleaseDC( NULL, hdc ) ;

    return TRUE ; ;
}

BOOL WINAPI engHFConfigure( HWND hwndParent, LPENGINECONFIG lpEC )
{
    LPENGINECONFIG    lpec ;

    /* Copy the ENGINECONFIG structure */
    if (lpec = engConfigCopy( NULL, lpEC ))
    {
        if (DialogBoxParam( ghmodDLL, MAKEINTRESOURCE( DLG_HFCONFIGURE ), 
                            hwndParent, (DLGPROC)fnHFConfigure, (LPARAM)lpec ))
        {
            /* Something was changed, so copy into lpEC
             */
            engConfigCopy( lpEC, lpec ) ;
            engConfigFree( lpec ) ;
            return TRUE ;
        }
        else
        {
            engConfigFree( lpec ) ;
            return FALSE ;
        }
    }
    return FALSE ;
}

/************************************************************************
 * End of File: HF.C
 ***********************************************************************/


                                  
