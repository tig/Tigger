/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  DEFFTD 
 *
 *      Module:  DEFFTD.C
 * 
 *      This is the default FTD module.  The RawText ftd code is here.
 *      This module sould serve as the basis for a creating a new 
 *      FTDD DLL.
 *
 *      For adding new FTDs see BITMAP.C which contains the bitmap/dib
 *      FTD which is also contained within ENGINE.DLL.
 *
 *   Revisions:  
 *       9/11/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"

#include "FTD.H"

/* The function prototype for each FTD within an FTDD should be listed here
 * and placed in the table below (FTDD).
 */
LRESULT CALLBACK fnFTD_RawText( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 ) ;
LRESULT CALLBACK fnFTD_Bitmap( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 ) ;

/* The FTDD structure defines which FTDs this FTDD exports.  The first member
 * of the FTDD is a LPSTR that identifies the name of the FTD, the second
 * member is the function pointer.  
 *
 * The LPSTR may be a resource string ID.  
 */
static FTDD    ftdd[] =
                {
                    { (LPSTR)IDS_DEFFTD_RAWTEXT,    fnFTD_RawText },
                    { (LPSTR)IDS_DEFFTD_BITMAP,     fnFTD_Bitmap }
                } ;

#define NUM_FTD     (sizeof(ftdd)/sizeof(ftdd[0]))

/*----------------------------------------------------------------------
 * Functions that must exist in each FTDD
 */
/* Function to initalize a FTDD
 *
 *  ORDINAL #2
 */
BOOL WINAPI FTDD_Init( VOID )
{
    return TRUE ;
}

/* Function to uninitalize a FTDD
 *
 *  ORDINAL #3
 */
BOOL WINAPI FTDD_UnInit( VOID )
{
    return TRUE ;
}
          
/* Functions to enumerate FTDs within an FTDD
 * 
 * These functions use the lpszFTD_Name member of the ftdd structure.
 *
 *  ORDINAL #4
 */
LPSTR WINAPI FTDD_FindFirstFTD( LPSTR lpszFTD_Name )
{
    if (HIWORD( ftdd[0].lpszFTD_Name ))
       lstrcpy( lpszFTD_Name, ftdd[0].lpszFTD_Name ) ;
    else
       lstrcpy( lpszFTD_Name, GRCS( LOWORD(ftdd[0].lpszFTD_Name )) ) ;
    return lpszFTD_Name ;
}

/*
 *  ORDINAL #5
 */
LPSTR WINAPI FTDD_FindNextFTD( LPSTR lpszPrevFTD, LPSTR lpszFTD_Name )
{
    UINT    n ;

    for (n = 0 ; n < (NUM_FTD-1) ; n++ )
    {
        if (HIWORD( ftdd[n].lpszFTD_Name ))
        {
            if (0 == lstrcmpi( ftdd[n].lpszFTD_Name, lpszFTD_Name ))
            {
                lstrcpy( lpszFTD_Name, ftdd[n+1].lpszFTD_Name ) ;
                return lpszFTD_Name ;            
            }
        }
        else
        {
            if (0 == lstrcmpi( GRCS( LOWORD( ftdd[n].lpszFTD_Name ) ), lpszFTD_Name ))
            {
                lstrcpy( lpszFTD_Name, GRCS( LOWORD( ftdd[n+1].lpszFTD_Name ) ) ) ;
                return lpszFTD_Name ;            
            }
        }
    }

    return NULL ;
}

/* Function to obtain the function address of an FTD.
 *
 * ORDIINAL #6
 */
FTDPROC WINAPI FTDD_GetProcAddress( LPSTR lpszFTD_Name ) 
{
    UINT    n ;

    for (n = 0 ; n < NUM_FTD ; n++ )
    {
        if (HIWORD( ftdd[n].lpszFTD_Name ))
        {
            if (0 == lstrcmpi( ftdd[n].lpszFTD_Name, lpszFTD_Name ))
                return ftdd[n].lpfnFTD ;
        }
        else
        {
            if (0 == lstrcmpi( GRCS( LOWORD( ftdd[n].lpszFTD_Name ) ), lpszFTD_Name ))
                return ftdd[n].lpfnFTD ;
        }
    }

    return NULL ;
}

/****************************************************************************
 *
 * Raw Text File Type Driver
 *
 ****************************************************************************/

/* FTDCFG structure 
 *
 * Most FTDs need to store data about their state and configuration.  The
 * following structure is internal to each ftd.  The ENGINECONFIG structure
 * has a member, FTDInfo.lpCustData, that points to this structure.
 *
 * This data is typically referenced by a variable named 'lpFTD'.
 *
 * The FTDCFG
 */
typedef struct tagRAWTEXTCFG
{
    char    szFace[LF_FACESIZE] ;
    char    szStyle[LF_FACESIZE] ;
    UINT    uiPointSize ;

    UINT    uiSpaces ;

    BOOL    fLineNumbers    : 1 ;
    BOOL    fExpandTabs     : 1 ;
    BOOL    fWordWrap       : 1 ;

} RAWTEXTCFG, * PRAWTEXTCFG, FAR * LPRAWTEXTCFG ;

/* FTDINFO structure 
 *
 * Most FTDs need to store 'runtime' state information.  This could include
 * buffers used for formatting lines, file handles, and so forth.  This
 * runtime data is typically refrerenced by the lpCustData member of the 
 * ENGINEJOB structure.
 */
typedef struct tagRAWTEXTINFO
{
    HFONT   hfont ;
    UINT    cyChar ;
    UINT    cxChar ;

    UINT    nLine ;

} RAWTEXTINFO, * PRAWTEXTINFO, FAR * LPRAWTEXTINFO ;    


/* The dlg box procedure that is used for controlling the controls that are
 * added to the page setup dialog box...
 */
BOOL CALLBACK fnFTD_RawTextDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

BOOL INTERNAL RawTextOutputPage( LPENGINEJOB lpJob, DWORD dwFlags, LPARAM lp ) ;


BOOL CALLBACK fnFTD_RawTextDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPENGINECONFIG  lpEC ;
    LPRAWTEXTCFG     lpFTD ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            char szBuf[LF_FACESIZE*3] ;

            /* lParam points to a LPENGINECONFIG structure
             */
            lpEC = (LPENGINECONFIG)lParam  ;

            if (lpEC && lpEC->FTDInfo.lpData)
            {
                lpFTD = (LPRAWTEXTCFG)lpEC->FTDInfo.lpData ;

                wsprintf( szBuf, "%s %s, %d pt.", 
                            (LPSTR)lpFTD->szFace, (LPSTR)lpFTD->szStyle,
                            lpFTD->uiPointSize ) ;
                SetDlgItemText( hDlg, IDD_FONT_LBL, szBuf ) ;

                CheckDlgButton( hDlg, IDD_LINENUMBERS, lpFTD->fLineNumbers ) ;

                SetDlgItemInt( hDlg, IDD_SPACES, lpFTD->uiSpaces, FALSE ) ;
                EnableWindow( GetDlgItem( hDlg, IDD_SPACES ), lpFTD->fExpandTabs ) ;
                CheckDlgButton( hDlg, IDD_EXPANDTABS, lpFTD->fExpandTabs ) ;
            }

        }
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
        
            lpEC = (LPENGINECONFIG)GetWindowLong( hDlg, DWL_USER ) ;

            if (lpEC && !(lpFTD = (LPRAWTEXTCFG)lpEC->FTDInfo.lpData))
            {
                DP1( hWDB, "FTDInfo.lpData == NULL!!" ) ;
            }

            switch (wID)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog( hDlg, wID ) ;
                break ;

                case IDD_EXPANDTABS:
                    if (wNotifyCode == BN_CLICKED)
                        lpFTD->fExpandTabs = IsDlgButtonChecked( hDlg, IDD_EXPANDTABS ) ;

                    EnableWindow( GetDlgItem( hDlg, IDD_SPACES ), lpFTD->fExpandTabs ) ;
                break ;

                case IDD_SPACES:
                    if (wNotifyCode == EN_CHANGE)
                    {
                        lpFTD->uiSpaces = GetDlgItemInt( hDlg, IDD_SPACES, NULL, FALSE ) ;
                    }
                break ;            

                case IDD_LINENUMBERS:
                    if (wNotifyCode == BN_CLICKED)
                        lpFTD->fLineNumbers = IsDlgButtonChecked( hDlg, IDD_LINENUMBERS ) ;
                break ;

                case IDD_FONT:
                {
                    LOGFONT lf;
                    CHOOSEFONT chf;
                    HDC     hdc ;

                    if (!lpEC || !lpFTD)
                        break ;
                
                    chf.hDC = engPrinterCreateDC( hDlg, lpEC ) ;

                    _fmemset( &lf, '\0', sizeof( LOGFONT ) ) ;
                    lstrcpy( lf.lfFaceName, lpFTD->szFace ) ;
                    lf.lfWidth = 0 ;
                    if (hdc = GetDC( hDlg ))
                    {
                        lf.lfHeight = -MulDiv( lpFTD->uiPointSize, 
                                               GetDeviceCaps( hdc, LOGPIXELSY),
                                               72 )  ;
                        ReleaseDC( hDlg, hdc ) ;
                    }

                    chf.lStructSize = sizeof( CHOOSEFONT ) ;
                    chf.hwndOwner = hDlg ;
                    chf.lpLogFont = &lf ;
                    chf.Flags = CF_USESTYLE | CF_INITTOLOGFONTSTRUCT | CF_SHOWHELP | CF_PRINTERFONTS | CF_FORCEFONTEXIST ;
                    chf.rgbColors = 0L ;
                    chf.lCustData = 0L ;
                    chf.lpfnHook = (UINT (CALLBACK*)(HWND, UINT, WPARAM, LPARAM))NULL ;
                    chf.lpTemplateName = (LPSTR) NULL ;
                    chf.hInstance = (HANDLE) NULL ;
                    chf.iPointSize = 0 ;
                    chf.lpszStyle = lpFTD->szStyle ;
                    chf.nFontType = SCREEN_FONTTYPE ;
                    chf.nSizeMin = 0 ;
                    chf.nSizeMax = 0 ;
                
                    if (ChooseFont(&chf))
                    {
                        char szBuf[LF_FACESIZE * 3] ;
                        lpFTD->uiPointSize = chf.iPointSize / 10 ;
                        lstrcpy( lpFTD->szFace, lf.lfFaceName ) ;

                        wsprintf( szBuf, "%s %s, %d pt.", 
                                (LPSTR)lpFTD->szFace, (LPSTR)lpFTD->szStyle,
                                lpFTD->uiPointSize ) ;
                        SetDlgItemText( hDlg, IDD_FONT_LBL, szBuf ) ;
                    }
                                        
                    DeleteDC( chf.hDC ) ;
                }
                break ;

                default :
                   return FALSE ;
            }
        }
        break ;

        case WM_DESTROY:
        break ;

        default:
            return FALSE ;
   }

   return TRUE ;
}

/* Raw Text FTD proc
 *
 */
LRESULT CALLBACK fnFTD_RawText( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 )
{
    LPRAWTEXTCFG    lpFTD ;
    LPRAWTEXTINFO   lpRaw ;

    LPENGINEJOB     lpJob ;

    switch (uiCmd)
    {
/******************* Setup/Dlg box time functions ***************************/

        case FTD_GETMODULEHANDLE:
            /* lParam1 -> NULL
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
            return MAKELRESULT( ghmodDLL, 0L ) ;
        break ;

        case FTD_READCONFIG:
        {
            /* lParam1 -> LPFTDINFO
             * lParam2 -> LPSTR lpszSection
             * lParam3 -> LPSTR lpszINIFile
             */
            LPFTDINFO       lpFTDInfo = (LPFTDINFO)lParam1 ;
            /* If uiStructSize is 0, then allocate lpData to our liking.
             */
            if (lpFTDInfo->uiStructSize == 0)  
            {
                DP1( hWDB, "RawText: uiStructSize == 0, allocating..." ) ;
                lpFTDInfo->lpData = (LPARAM)GlobalAllocPtr( GHND, sizeof( RAWTEXTCFG ) ) ;
                lpFTDInfo->uiStructSize = sizeof( RAWTEXTCFG ) ;
            }
            else
            {
                /* Make sure it's the right size
                 */
                if (lpFTDInfo->uiStructSize != sizeof( RAWTEXTCFG ))
                {
                    DP1( hWDB, "RawText: uiStructSize != RAWTEXTCFG size" ) ;
                    return 0L ;
                }
            }

            if (lpFTD = (LPRAWTEXTCFG)lpFTDInfo->lpData)
            {
                char szBuf[128] ;
                LPSTR lpsz ;
                /* Read like mad!
                 */    
                if (GetPrivateProfileString( (LPSTR)lParam2, GRCS(IDS_KEY_RAWTEXTOPTS),
                                           "", szBuf, 128, (LPSTR)lParam3 ))
                {
                    lpsz = szBuf ;
                    DP3( hWDB, "RawText: Got opts-> '%s'", (LPSTR)lpsz ) ;
    
                    ParseOffString( (LPSTR FAR *)&lpsz, lpFTD->szFace ) ;
                    ParseOffString( (LPSTR FAR *)&lpsz, lpFTD->szStyle ) ;
                    lpFTD->uiPointSize = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->fLineNumbers = (BOOL)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->fExpandTabs = (BOOL)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->uiSpaces = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                }
                else
                    fnFTD_RawText( FTD_INITCONFIG, (LPARAM)lpFTDInfo, 0L, 0L ) ;
            }
            else
            {
                DP1( hWDB, "RawText: lpData bad!" ) ;
                return 0L ;
            }

            return MAKELRESULT( TRUE, 0 ) ;
        }
        break ;

        case FTD_WRITECONFIG:
        {
            /* lParam1 -> LPFTDINFO
             * lParam2 -> LPSTR lpszSection
             * lParam3 -> LPSTR lpszINIFile
             */
            LPFTDINFO       lpFTDInfo = (LPFTDINFO)lParam1 ;

            if (lpFTDInfo->uiStructSize != sizeof( RAWTEXTCFG ))
            {
                DP1( hWDB, "RawText: uiStructSize bad!" ) ;
                return 0L ;
            }

            if (lpFTD = (LPRAWTEXTCFG)lpFTDInfo->lpData)
            {
                char szBuf[128] ;

                /* Write like mad!
                 */    
                wsprintf( szBuf, "%s,%s,%d,%d,%d,%d", (LPSTR)lpFTD->szFace,
                                                      (LPSTR)lpFTD->szStyle,
                                                      lpFTD->uiPointSize,
                                                      lpFTD->fLineNumbers,
                                                      lpFTD->fExpandTabs,
                                                      lpFTD->uiSpaces ) ;
                WritePrivateProfileString( (LPSTR)lParam2, GRCS(IDS_KEY_RAWTEXTOPTS),
                                           szBuf, (LPSTR)lParam3 ) ;
            }
            else
            {
                DP1( hWDB, "RawText: lpData bad!" ) ;
                return 0L ;
            }

            return MAKELRESULT( TRUE, 0 ) ;
        }    
        break ;

        case FTD_GETDLGTEMPLATE:
            /* lParam1 -> NULL
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
            /* Return a LPDLGBOXHEADER to the caller with our
             * dialog box template
             */
            return (LRESULT)GetDlgBoxRes( ghmodDLL, (LPSTR)MAKEINTRESOURCE( DLG_FTD_RAWTEXT ) ) ;
        break ;

        case FTD_CONFIGDLGMSG:
        {
            /* lParam1 -> LPMSG
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
            LPMSG   lpMsg = (LPMSG)lParam1 ;

            return (LRESULT)fnFTD_RawTextDlg( lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam ) ;
        }
        break ;

        case FTD_INITCONFIG:
        {
            /* lParam1 -> LPFTDINFO
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
            LPFTDINFO       lpFTDInfo = (LPFTDINFO)lParam1 ;
            /* If uiStructSize is 0, then allocate lpData to our liking.
             */
            if (lpFTDInfo->uiStructSize)  
                GlobalFreePtr( lpFTDInfo->lpData ) ;

            DP1( hWDB, "RawText: uiStructSize == 0, allocating..." ) ;
            lpFTDInfo->lpData = (LPARAM)GlobalAllocPtr( GHND, sizeof( RAWTEXTCFG ) ) ;
            lpFTDInfo->uiStructSize = sizeof( RAWTEXTCFG ) ;

            if (lpFTD = (LPRAWTEXTCFG)lpFTDInfo->lpData)
            {
                lstrcpy( lpFTD->szStyle, GRCS( IDS_RAWTEXT_DEFAULT_STYLE ) ) ;
                lstrcpy( lpFTD->szFace, GRCS( IDS_RAWTEXT_DEFAULT_FACE ) ) ;
                lpFTD->uiPointSize = 10 ;
                lpFTD->fLineNumbers = FALSE ;
                lpFTD->fExpandTabs = TRUE ;
                lpFTD->uiSpaces = 8 ;
            }
        }
        return MAKELRESULT( TRUE, 0 ) ;

/******************* Print time functions *********************************/

        case FTD_STARTDOC:
            lpJob = (LPENGINEJOB)lParam1 ;

            /* Call the default to open the file.
             */
            if (!ftdDefaultProc( uiCmd, lParam1, lParam2, lParam3 ))
                return NULL ;

            if (lpJob->lpCustData = GlobalAllocPtr( GHND, sizeof(RAWTEXTINFO) ))
            {
                TEXTMETRIC tm ;

                lpFTD = (LPRAWTEXTCFG)lpJob->lpEC->FTDInfo.lpData ;
                lpRaw = (LPRAWTEXTINFO)lpJob->lpCustData ;

                DP3( hWDB, "Using %s %s, %d point font...", (LPSTR)lpFTD->szFace, (LPSTR)lpFTD->szStyle, lpFTD->uiPointSize ) ;
                lpRaw->hfont = ReallyCreateFontEx( lpJob->hdc, lpFTD->szFace, lpFTD->szStyle, lpFTD->uiPointSize, NULL ) ;
                DASSERTMSG( hWDB, lpRaw->hfont, "Could not create font!" ) ;

                lpRaw->hfont = SelectObject( lpJob->hdc, lpRaw->hfont ) ;
                lpRaw->cxChar = GetTextMetricsCorrectly( lpJob->hdc, &tm ) ;

                lpRaw->cyChar = tm.tmHeight + tm.tmExternalLeading - 1 ;

                lpRaw->hfont = SelectObject( lpJob->hdc, lpRaw->hfont ) ;

                return TRUE ;
            }
            else
            {
                DP1( hWDB, "GlobalAllocPtr( RAWTEXTINFO ) failed!" ) ;
                return NULL ;
            }
        break ;

        case FTD_ENDDOC:
            /* lParam1 -> LPJOB
             *
             * If lpJob->hFile is non-zero, close the file.
             */
            lpJob = (LPENGINEJOB)lParam1 ;
            if (lpJob->lpCustData)
            {
                lpRaw = (LPRAWTEXTINFO)lpJob->lpCustData ;
                if (lpRaw->hfont)
                    DeleteObject( lpRaw->hfont ) ;

                GlobalFreePtr( lpJob->lpCustData ) ;
                lpJob->lpCustData = NULL ;
            }

            /* Call the default to close the file.
             */
            return ftdDefaultProc( uiCmd, lParam1, lParam2, lParam3 ) ;
        break ;

        case FTD_OUTPUTPAGE:
            /* lParam1 -> LPJOB
             *
             * Returns TRUE if successful, FALSE if error.
             */
            return RawTextOutputPage( (LPENGINEJOB)lParam1, (DWORD)lParam2, lParam3 ) ;
        break ;

        default:
            /* Call the default FTD proc (in ENGINE.DLL) for any
             * commands that are not specifically processed.
             */
            return ftdDefaultProc( uiCmd, lParam1, lParam2, lParam3 ) ;
        break ;
    }
    
    return 0L ;
}

BOOL INTERNAL RawTextOutputPage( LPENGINEJOB lpJob, DWORD dwFlags, LPARAM lp )
{
    LPRAWTEXTINFO   lpRaw ;
    LPRAWTEXTCFG    lpFTD ;
    DWORD           dwBytesRead ;
    SIZE            size ;
    UINT            nLine = 0 ;
    int             xPos = 0, yPos = 0 ;
    LPSTR           lpszOut ;
    UINT            nChar = 0 ;
    int             nTabPos = 0 ;

    if (!(lpszOut = GlobalAllocPtr( GHND, 2048 )))
    {
        DASSERTMSG( hWDB, 0, "GlobalAllocPtr( 2048 )  failed!" ) ;
        return FALSE ;
    }

    lpRaw = (LPRAWTEXTINFO)lpJob->lpCustData ;
    lpFTD = (LPRAWTEXTCFG)lpJob->lpEC->FTDInfo.lpData ;

    lpRaw->hfont = SelectObject( lpJob->hdc, lpRaw->hfont ) ;

    if (lpFTD->fExpandTabs)
        nTabPos = lpRaw->cxChar * lpFTD->uiSpaces ;

    while (yPos + lpRaw->cyChar <= lpJob->uiSubPageY)
    {
        if (!(dwBytesRead = _lread( lpJob->hFile, lpszOut + nChar, 1 )))
        {
            TextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar ) ;
            nLine++ ;
            yPos = lpJob->uiSubPageY ;
        }

        switch( *(lpszOut + nChar) )
        {
            case '\r':  // cr
                if (lpFTD->fExpandTabs)
                    TabbedTextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar, 1, &nTabPos, 0 ) ;
                else
                    TextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar ) ;
                nChar = 0 ;
                _fmemset( lpszOut, '\0', 2048 ) ;

                lpJob->dwBytesRemaining -= dwBytesRead ;
            break ;

            case '\n':  // newline
                if (lpFTD->fExpandTabs)
                    TabbedTextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar, 1, &nTabPos, 0 ) ;
                else
                    TextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar ) ;
                nChar = 0 ;
                _fmemset( lpszOut, '\0', 2048 ) ;
                nLine++ ;

                yPos = (lpRaw->cyChar * nLine ) ;
                lpJob->dwBytesRemaining -= dwBytesRead ;
            break ;

            case '\f':  // formfeed
                if (lpFTD->fExpandTabs)
                    TabbedTextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar, 1, &nTabPos, 0 ) ;
                else
                    TextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar ) ;
                nChar = 0 ;
                _fmemset( lpszOut, '\0', 2048 ) ;
                nLine++ ;

                yPos = lpJob->uiSubPageY ;
                lpJob->dwBytesRemaining -= dwBytesRead ;
            break ;

            case '\x1A':// eof
                /* If CTRL-Z is not really end of page 
                 * then we need to do something different here.
                 */
                if (lpFTD->fExpandTabs)
                    TabbedTextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar, 1, &nTabPos, 0 ) ;
                else
                    TextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar ) ;
                nChar = 0 ;
                _fmemset( lpszOut, '\0', 2048 ) ;
                nLine++ ;

                yPos = lpJob->uiSubPageY ;
                lpJob->dwBytesRemaining = 0 ;
            break ;

            default:
                if (lpFTD->fExpandTabs)
                {
                    DWORD   dw ;
                    dw = GetTabbedTextExtent( lpJob->hdc, lpszOut, nChar + 1, 1, &nTabPos ) ;
                    size.cx = LOWORD( dw ) ;
                    size.cy = HIWORD( dw ) ;
                }
                else
                    GetTextExtentPoint( lpJob->hdc, lpszOut, nChar + 1, &size ) ;


                if (xPos + (UINT)size.cx > lpJob->uiSubPageX + 1)
                {
                    /* Went too far.  Backup one char and go to next line.
                     */
                    _llseek( lpJob->hFile, -1, 1 ) ;
                    *(lpszOut + nChar) = '\0' ;

                    if (lpFTD->fExpandTabs)
                        TabbedTextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar, 1, &nTabPos, 0 ) ;
                    else
                        TextOut( lpJob->hdc, xPos, yPos, lpszOut, nChar ) ;
                    nChar = 0 ;
                    _fmemset( lpszOut, '\0', 2048 ) ;
                    nLine++ ;

                    yPos = (lpRaw->cyChar * nLine ) ;
                }
                else
                {
                    lpJob->dwBytesRemaining -= dwBytesRead ;
                    nChar++ ;
                }
            break ;
        }
    }

    lpRaw->hfont = SelectObject( lpJob->hdc, lpRaw->hfont ) ;
    GlobalFreePtr( lpszOut ) ;

    return TRUE ;
}




/************************************************************************
 * End of DEFFTD.H
 ***********************************************************************/
