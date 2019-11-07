/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  PRINTER 
 *
 *      Module:  PRINTER.C
 * 
 *
 *   Revisions:  
 *       9/27/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "ENGINE.h"

HDC WINAPI engPrinterCreateDC( HWND hwndParent, LPENGINECONFIG lpEC )
{
    LPDEVMODE lpdm ;
    HDC  hdc ;
    HCURSOR hcur ;

    VALIDATEPARAM( lpEC ) ;

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    if (!(*lpEC->szDriver && *lpEC->szDevice && *lpEC->szOutput) || 
        lpEC->dwFlags & ENGCFG_USEDEFAULTPRN)
    {
        /* Get the default printer
         */
        if (!engPrinterGetDefault( hwndParent, lpEC ))
        {
            SetCursor( hcur ) ;
            return NULL ;
        }
    }

    if (!(lpdm = engPrinterGetDevMode( hwndParent, lpEC )))
    {
        SetCursor( hcur ) ;
        DP1( hWDB, "engPrinterGetDevMode failed!!" ) ;
        return NULL ;
    }

//    lpdm->dmFields           = lpEC->dmFields        ;

    if (lpdm->dmFields & DM_ORIENTATION)
        lpdm->dmOrientation      = lpEC->dmOrientation   ;

    if (lpdm->dmFields & DM_PAPERSIZE)
        lpdm->dmPaperSize        = lpEC->dmPaperSize     ;

    if (lpdm->dmFields & DM_PAPERLENGTH)
        lpdm->dmPaperLength      = lpEC->dmPaperLength   ;

    if (lpdm->dmFields & DM_PAPERWIDTH)
        lpdm->dmPaperWidth       = lpEC->dmPaperWidth    ;

    if (lpdm->dmFields & DM_SCALE)
    {
        if (lpEC->dmScale == 0)
            lpEC->dmScale = 100 ;

        lpdm->dmScale            = lpEC->dmScale         ;
    }

    if (lpdm->dmFields & DM_COPIES)
        lpdm->dmCopies           = lpEC->dmCopies        ;

    if (lpdm->dmFields & DM_DEFAULTSOURCE)
        lpdm->dmDefaultSource    = lpEC->dmDefaultSource ;

    if (lpdm->dmFields & DM_PRINTQUALITY)
        lpdm->dmPrintQuality     = lpEC->dmPrintQuality  ;

    if (lpdm->dmFields & DM_COLOR)
        lpdm->dmColor            = lpEC->dmColor         ;

    if (lpdm->dmFields & DM_DUPLEX)
        lpdm->dmDuplex           = lpEC->dmDuplex        ;

    hdc = CreateDC( lpEC->szDriver, lpEC->szDevice, lpEC->szOutput, lpdm ) ;

    GlobalFreePtr( lpdm ) ;

    SetCursor( hcur ) ;

    return hdc ;
}    

BOOL WINAPI engPrinterGetDefault( HWND hwndParent, LPENGINECONFIG lpEC )
{
    PRINTDLG        pd ;
    DEVNAMES FAR *  lpdn ;
    DEVMODE  FAR *  lpdm ;
    
    _fmemset( &pd, '\0', sizeof( PRINTDLG ) ) ;
    pd.lStructSize = sizeof( PRINTDLG ) ;
    pd.hwndOwner = hwndParent ;
    pd.hDevMode = NULL ;
    pd.hDevNames = NULL ;
    pd.Flags = PD_RETURNDEFAULT ;
    
    if (!PrintDlg( &pd ))
    {
        ErrorResBox( hwndParent, ghmodDLL, MB_ICONEXCLAMATION, 
                         IDS_APPTITLE, IDS_PDERR_NODEFAULTPRN ) ;
        dwLastError = ENGERR_NODEFAULTPRN ;
        return FALSE ;
    }

    if (lpdn = (DEVNAMES FAR *)GlobalLock( pd.hDevNames ))
    {
        lstrcpy( lpEC->szDriver, (LPSTR)lpdn+(lpdn->wDriverOffset) ) ;
        lstrcpy( lpEC->szDevice, (LPSTR)lpdn+(lpdn->wDeviceOffset) ) ;
        lstrcpy( lpEC->szOutput, (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;

        GlobalUnlock( pd.hDevNames ) ;
        GlobalFree( pd.hDevNames ) ;
    }
    else
    {
        if (pd.hDevMode)
            GlobalFree( pd.hDevMode ) ;

        dwLastError = ENGERR_GLOBALALLOC ;
        return FALSE ;
    }

    if (lpdm = (DEVMODE FAR *)GlobalLock( pd.hDevMode ))
    {
        lpEC->dmFields = lpdm->dmFields ;

        GlobalUnlock( pd.hDevMode ) ;
        GlobalFree( pd.hDevMode ) ;
    }
    else
    {
        dwLastError = ENGERR_GLOBALALLOC ;
        return FALSE ;
    }


    return TRUE ;    
}

LPDEVMODE WINAPI engPrinterGetDevMode( HWND hwnd, LPENGINECONFIG lpEC )
{
    LPFNDEVMODE     lpfnExtDeviceMode ;
    HMODULE         hDriver ;
    char            szDriver[144] ;
    LPDEVMODE       lpDevMode = NULL ;

    wsprintf( szDriver, "%s.DRV", (LPSTR)lpEC->szDriver ) ;
   
    if (HINSTANCE_ERROR >= (hDriver = LoadLibrary( szDriver )))
    {
        DP1( hWDB, "GetDevMode : Could not load driver : %s", (LPSTR)szDriver ) ;
        return NULL ;
    }

    lpfnExtDeviceMode = (LPFNDEVMODE)GetProcAddress( hDriver, PROC_EXTDEVICEMODE ) ;

    if (!lpfnExtDeviceMode)
    {
        DP3( hWDB, "GetDevMode : Driver does not support ExtDeviceMode") ;
        FreeLibrary( hDriver ) ;
        return NULL ;
    }
    else
    {
        short n ;
        /*
         * Call ExtDeviceMode to get size of DEVMODE structure
         */
        n = (*lpfnExtDeviceMode)( hwnd, hDriver,
                                   (LPDEVMODE)NULL,
                                   (LPSTR)lpEC->szDevice,
                                   (LPSTR)lpEC->szOutput,
                                   (LPDEVMODE)NULL,
                                   (LPSTR)NULL,
                                   0 ) ;

        if ((n <= 0) || !(lpDevMode = GlobalAllocPtr( GHND, n )))
        {
            DP1( hWDB, "GetDevMode : First call to ExtDeviceMode bad! n = %d", n ) ;
            FreeLibrary( hDriver ) ;
            return NULL ;
        }
        
        n = (*lpfnExtDeviceMode)( hwnd, hDriver,
                                   (LPDEVMODE)lpDevMode,
                                   (LPSTR)lpEC->szDevice,
                                   (LPSTR)lpEC->szOutput,
                                   (LPDEVMODE)lpDevMode,
                                   (LPSTR)NULL,
                                   DM_COPY ) ;

        FreeLibrary( hDriver ) ;

        if (n != IDOK)
        {
            DP1( hWDB, "GetDevMode : Second call to ExtDeviceMode bad! n = %d", n ) ;
            GlobalFreePtr( lpDevMode ) ;
            return NULL ;
        }
   }

   return lpDevMode ;

} /* engPrintGetDevMode()  */


LRESULT CALLBACK fnPrintSetup( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

BOOL WINAPI engPrinterChoose( HWND hwndParent, LPENGINECONFIG lpECin )
{
    PRINTDLG        pd ;
    DEVNAMES FAR *  lpdn = NULL ;
    DEVMODE FAR *   lpdm = NULL ;
    LPENGINECONFIG  lpEC ;
    HCURSOR         hcur ; ;

    /* Copy the ENGINECONFIG structure */
    if (!(lpEC = engConfigCopy( NULL, lpECin )))
        return FALSE ;

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    _fmemset( &pd, '\0', sizeof( PRINTDLG ) ) ;
    pd.lStructSize = sizeof( PRINTDLG ) ;
    pd.hwndOwner = GetActiveWindow() ;
    pd.hInstance = ghmodDLL ;
    pd.lpfnSetupHook = (UINT (CALLBACK*)(HWND,UINT,WPARAM,LPARAM))fnPrintSetup ;

DoPrintSetup:
    /* Default Printer? */
    if (lpEC->dwFlags & ENGCFG_USEDEFAULTPRN)
    {
        pd.Flags = PD_RETURNDEFAULT ;
        if (!PrintDlg( &pd ))
        {
            DP1( hWDB, "PrintDlg (PD_RETURNDEFAULT) failed! : %08lu", CommDlgExtendedError() ) ;
            ErrorResBox( hwndParent, ghmodDLL, MB_ICONEXCLAMATION, 
                         IDS_APPTITLE, IDS_PDERR_NODEFAULTPRN ) ;
    
            FAIL(ENGERR_NODEFAULTPRN) ;
        }

        hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

        if (lpdn = (DEVNAMES FAR *)GlobalLock( pd.hDevNames ))
        {
            lpdn->wDefault = DN_DEFAULTPRN ;
        }
    }
    else
    {
        UINT w1 = lstrlen( lpEC->szDriver ) + 1;
        UINT w2 = lstrlen( lpEC->szDevice ) + 1;
        UINT w3 = lstrlen( lpEC->szOutput ) + 1;

        if (!(pd.hDevNames = GlobalAlloc( GHND, sizeof(DEVNAMES) + w1 + w2 + w3 )))
        {
            FAIL( ENGERR_GLOBALALLOC ) ;            
        }
        
        if (lpdn = (DEVNAMES FAR *)GlobalLock( pd.hDevNames ))
        {
            lpdn->wDriverOffset = sizeof( DEVNAMES ) ;
            _fmemcpy( (LPSTR)lpdn + lpdn->wDriverOffset, lpEC->szDriver, w1 ) ;
            
            lpdn->wDeviceOffset = sizeof( DEVNAMES ) + w1 ;
            _fmemcpy( (LPSTR)lpdn + lpdn->wDeviceOffset, lpEC->szDevice, w2 ) ;
            
            lpdn->wOutputOffset = sizeof( DEVNAMES ) + w1 + w2 ;
            _fmemcpy( (LPSTR)lpdn + lpdn->wOutputOffset, lpEC->szOutput, w3 ) ;
          
            lpdn->wDefault = 0 ;
              
            GlobalUnlock( pd.hDevNames ) ;
            lpdn = NULL ;
        }
    }

    /* only allocate a devmode if needed
     */
    if (pd.hDevMode == NULL && !(pd.hDevMode = GlobalAlloc( GHND, sizeof(DEVMODE) )))
    {
        DP1( hWDB, "Could not allocate hDevMode" ) ;
        FAIL( ENGERR_GLOBALALLOC ) ;
    }

    lpdm = (DEVMODE FAR *)GlobalLock( pd.hDevMode ) ;
    _fmemset( lpdm, '\0', sizeof(DEVMODE) ) ;
    lpdm->dmFields           = lpEC->dmFields        ;
    lpdm->dmOrientation      = lpEC->dmOrientation   ;
    lpdm->dmPaperSize        = lpEC->dmPaperSize     ;
    lpdm->dmPaperLength      = lpEC->dmPaperLength   ;
    lpdm->dmPaperWidth       = lpEC->dmPaperWidth    ;
    lpdm->dmScale            = lpEC->dmScale         ;
    lpdm->dmCopies           = lpEC->dmCopies        ;
    lpdm->dmDefaultSource    = lpEC->dmDefaultSource ;
    lpdm->dmPrintQuality     = lpEC->dmPrintQuality  ;
    lpdm->dmColor            = lpEC->dmColor         ;
    lpdm->dmDuplex           = lpEC->dmDuplex        ;
    GlobalUnlock( pd.hDevMode ) ;
    lpdm = NULL ;
    
    pd.Flags = PD_PRINTSETUP | PD_SHOWHELP | PD_USEDEVMODECOPIES | PD_ENABLESETUPHOOK ;
    pd.lCustData = (LPARAM)lpEC ;

    if (PrintDlg( &pd ) == 0)
    {
        DWORD dwErr ;
        UINT  nErr ;

        // Hit cancel, or an error
        switch( dwErr = CommDlgExtendedError() )
        {
            case PDERR_PRINTERNOTFOUND:
                ErrorResBox( hwndParent, ghmodDLL, MB_ICONEXCLAMATION, IDS_APPTITLE,
                             IDS_PDERR_PRINTERNOTFOUND, 
                             (LPSTR)lpEC->szDevice, 
                             (LPSTR)lpEC->szOutput ) ;

                if (lpdn)           
                {
                    GlobalUnlock( pd.hDevNames ) ;
                    lpdn = NULL ;
                }

                if (pd.hDevNames)
                {
                    GlobalFree( pd.hDevNames ) ;
                    pd.hDevNames = NULL ;
                }

                if (lpdm)
                {
                    GlobalUnlock( pd.hDevMode ) ;
                    lpdm = NULL ;
                }

                if (pd.hDevMode)    
                {
                    GlobalFree( pd.hDevMode ) ;
                    pd.hDevMode = NULL ;
                }

                lpEC->dwFlags |= ENGCFG_USEDEFAULTPRN ;
            goto DoPrintSetup ;

            case 0:                     // The user hit CANCEL
            case PDERR_NODEFAULTPRN:    // This is not an error in this case
                FAIL( ENGERR_ZERO ) ;
            break ;

            case PDERR_PRINTERCODES:   nErr = IDS_PDERR_PRINTERCODES ;    break ;
            case PDERR_SETUPFAILURE:   nErr = IDS_PDERR_SETUPFAILURE ;    break ;
            case PDERR_PARSEFAILURE:   nErr = IDS_PDERR_PARSEFAILURE ;    break ;
            case PDERR_RETDEFFAILURE:  nErr = IDS_PDERR_RETDEFFAILURE ;   break ;
            case PDERR_LOADDRVFAILURE: nErr = IDS_PDERR_LOADDRVFAILURE ;  break ;
            case PDERR_GETDEVMODEFAIL: nErr = IDS_PDERR_GETDEVMODEFAIL ;  break ;
            case PDERR_INITFAILURE:    nErr = IDS_PDERR_INITFAILURE ;     break ;
            case PDERR_NODEVICES:      nErr = IDS_PDERR_NODEVICES ;       break ;
            case PDERR_DNDMMISMATCH:   nErr = IDS_PDERR_DNDMMISMATCH ;    break ;
            case PDERR_CREATEICFAILURE:nErr = IDS_PDERR_CREATEICFAILURE ; break ;
            default: 
                nErr = IDS_CDERR_GENERALCODES ; 
            break ;
        }

        ErrorResBox( hwndParent, ghmodDLL, MB_ICONEXCLAMATION, IDS_APPTITLE, nErr ) ;

        FAIL( ENGERR_PRINTERSETUP ) ;
    }

    /* If we get here, then the user hit OK
     */

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    if (!(lpdn = (DEVNAMES FAR *)GlobalLock( pd.hDevNames )))
        FAIL( ENGERR_GLOBALALLOC ) ;

    lstrcpy( lpECin->szDriver, (LPSTR)lpdn+lpdn->wDriverOffset ) ;
    lstrcpy( lpECin->szDevice, (LPSTR)lpdn+lpdn->wDeviceOffset ) ;
    lstrcpy( lpECin->szOutput, (LPSTR)lpdn+lpdn->wOutputOffset ) ;

    if (lpdn->wDefault & DN_DEFAULTPRN)
        lpECin->dwFlags |= ENGCFG_USEDEFAULTPRN ;
    else
        lpECin->dwFlags &= ~ENGCFG_USEDEFAULTPRN ;

    GlobalUnlock( pd.hDevNames ) ;
    GlobalFree( pd.hDevNames ) ;

    if (!(lpdm = (DEVMODE FAR *)GlobalLock( pd.hDevMode )))
        FAIL( ENGERR_GLOBALALLOC ) ;

    lpECin->dmFields           = lpdm->dmFields        ;
    lpECin->dmOrientation      = lpdm->dmOrientation   ;
    lpECin->dmPaperSize        = lpdm->dmPaperSize     ;
    lpECin->dmPaperLength      = lpdm->dmPaperLength   ;
    lpECin->dmPaperWidth       = lpdm->dmPaperWidth    ;
    lpECin->dmScale            = lpdm->dmScale         ;
    lpECin->dmCopies           = lpdm->dmCopies        ;
    lpECin->dmDefaultSource    = lpdm->dmDefaultSource ;
    lpECin->dmPrintQuality     = lpdm->dmPrintQuality  ;
    lpECin->dmColor            = lpdm->dmColor         ;
    lpECin->dmDuplex           = lpdm->dmDuplex        ;

    GlobalUnlock( pd.hDevMode ) ;
    GlobalFree( pd.hDevMode ) ;

    if (lpEC)
        engConfigFree( lpEC ) ;

    SetCursor( hcur ) ;
    return TRUE ;    
  
Fail:
    
    if (lpdn)
        GlobalUnlock( pd.hDevNames ) ;

    if (pd.hDevNames)
        GlobalFree( pd.hDevNames ) ;

    if (lpdm)
        GlobalUnlock( pd.hDevMode ) ;

    if (pd.hDevMode)
        GlobalFree( pd.hDevMode ) ;

    if (lpEC)
        engConfigFree( lpEC ) ;

    SetCursor( hcur ) ;

    return FALSE ;  
}

LRESULT CALLBACK fnPrintSetup( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            LPENGINECONFIG lpEC = (LPENGINECONFIG)((LPPRINTDLG)lParam)->lCustData ;

            if (lpEC && lpEC->szConfigName)
            {
                char szBuf[MAX_CONFIGNAMELEN + 32] ;
                wsprintf( szBuf, GRCS(IDS_PRINTERSETUP), (LPSTR)lpEC->szConfigName ) ;
                SetWindowText( hDlg, szBuf ) ;
            }
            
            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;
        }
        return TRUE ;

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
    
            switch (wID)
            {
                case pshHelp:
                    DASSERTMSG( hWDB, 0, "No help yet" ) ;
                break ;

                default:
                    return FALSE ;

            }
        }
        break ;
    }

    return 0L ;
}


/****************************************************************
 *  BOOL CALLBACK fnDefaultAbortProc( HDC hdcPrn, short nCode )
 *
 *
 ****************************************************************/
BOOL CALLBACK fnDefaultAbortProc( HDC hdcPrn, int nCode )
{
    MSG         msg ;
    LPENGINEJOB lpJob ;

    if (nCode != 0 && nCode != SP_OUTOFDISK)
    {
        DP1( hWDB, "fnDefaultAbortProc(): nCode = %04X", nCode ) ;
        return FALSE ;
    }

    if (nCode == SP_OUTOFDISK)
    {
        DP1( hWDB, "fnDefaultAbortProc(): nCode is SP_OUTOFDISK, PeekMessage time..." ) ;
    }

    if (lpJob = GetJobFromSleazyTable( hdcPrn ))
    {
        while (!lpJob->fUserAborted && 
               PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (!lpJob->hdlgStatus || 
                !IsDialogMessage( lpJob->hdlgStatus, &msg ))
            {
                TranslateMessage( &msg ) ;
                DispatchMessage( &msg ) ;
            }
        }

        return !lpJob->fUserAborted ;
    }
    else
    {
        DP1( hWDB, "This is bad.  Could not get lpJob in fnDefaultAbortProc" ) ;
        #pragma message( "Need some smart error reporting here" )
        return FALSE ;
    }

} /* fnAbortProc()  */


/* Since there may be more than one print job going at once
 * that uses this code and the AbortProc only gets an hdc and
 * nCode as parameters, we have a sleazy table that we can
 * use to key off of the DC to find the appropriate hPrtJob.
 *
 * HACK!!! Someday this thing should be dynamically allocated.
 */
typedef struct tagSleazyTable
{
    HDC         hdc ;
    LPENGINEJOB lpJob ;
} SLEAZYTABLE ;

#define MAX_CONCURRENTJOBS      64

static SLEAZYTABLE rgSleaze[MAX_CONCURRENTJOBS] ;

LPENGINEJOB WINAPI GetJobFromSleazyTable( HDC hdc )
{
   short i ;

   for (i = 0 ; i < MAX_CONCURRENTJOBS ; i++)
      if ( rgSleaze[i].hdc == hdc )
         return rgSleaze[i].lpJob ;

   return NULL ;

} /* GethPrtJobSleazyTable()  */

VOID WINAPI PutJobInSleazyTable( LPENGINEJOB lpJob ) 
{
    short i ;

    for (i = 0 ; i < MAX_CONCURRENTJOBS ; i++)
    {
        if (rgSleaze[i].hdc == NULL)
        {
            rgSleaze[i].hdc = lpJob->hdc ;
            rgSleaze[i].lpJob = lpJob ;
            return ;
        }
    }

    DASSERTMSG( hWDB, 0, "PutJobInSleazyTable" ) ;
}

VOID WINAPI RemoveJobFromSleazyTable( LPENGINEJOB lpJob ) 
{
    short i ;

    for (i = 0 ; i < MAX_CONCURRENTJOBS ; i++)
    {
        if (rgSleaze[i].hdc == lpJob->hdc)
        {
            rgSleaze[i].hdc = NULL ;
            rgSleaze[i].lpJob = NULL ;
            return ;
        }
    }

    DASSERTMSG( hWDB, 0, "RemoveJobInSleazyTable" ) ;
}


BOOL CALLBACK fnDefaultStatusDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPENGINEJOB     lpJob ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            char szBuf[256] ;
            char szOutput[MAX_UNCSHARELEN] ;
            UINT cb ;
            HFONT hfont ;

            hfont = ReallyCreateFontEx( NULL, "MS Sans Serif", "Regular", 8, RCF_NORMAL ) ;

            SetWindowLong( hDlg, DWL_USER, (LONG)lParam ) ;
            lpJob = (LPENGINEJOB)lParam ;

            wsprintf( szBuf, GRCS( IDS_STATUSDLG_LINE1 ), (LPSTR)lpJob->lpszDocName ) ;
            SetDlgItemText( hDlg, IDD_LINE1, szBuf ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_LINE1 ), hfont, FALSE ) ;

            wsprintf( szBuf, GRCS( IDS_STATUSDLG_LINE2 ), (LPSTR)lpJob->lpEC->szDevice ) ;
            SetDlgItemText( hDlg, IDD_LINE2, szBuf ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_LINE2 ), hfont, FALSE ) ;
            
            cb = MAX_UNCSHARELEN ;
            if (WN_SUCCESS == WNetGetConnection( lpJob->lpEC->szOutput, szOutput, &cb ))
                wsprintf( szBuf, GRCS( IDS_STATUSDLG_LINE3 ), (LPSTR)szOutput ) ;
            else
                wsprintf( szBuf, GRCS( IDS_STATUSDLG_LINE3 ), (LPSTR)lpJob->lpEC->szOutput ) ;
            SetDlgItemText( hDlg, IDD_LINE3, szBuf ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_LINE3 ), hfont, FALSE ) ;

            SetWindowFont( GetDlgItem( hDlg, IDD_LINE4 ), hfont, FALSE ) ;
            SendMessage( hDlg, WM_USER, 0, lParam ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;
            return TRUE ;    // Focus will be set to first control
        }
        break ;

        case WM_USER:
        {
            char szBuf[80] ;

            lpJob = (LPENGINEJOB)lParam ;
            if (lpJob->nPhysicalPages)
                wsprintf( szBuf, GRCS( IDS_STATUSDLG_LINE4 ), lpJob->nPhysicalPage, lpJob->nPhysicalPages ) ;
            else
                wsprintf( szBuf, GRCS( IDS_STATUSDLG_LINE5 ), lpJob->nPhysicalPage ) ;
            SetDlgItemText( hDlg, IDD_LINE4, szBuf ) ;
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

            lpJob = (LPENGINEJOB)GetWindowLong( hDlg, DWL_USER ) ;

            switch (wID)
            {
                case IDOK:
                case IDCANCEL:
                    lpJob->fUserAborted = TRUE ;
                break ;

                default :
                   return FALSE ;
            }
        }
        break ;

        case WM_DESTROY:
            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_LINE1 ) ) ) ;
        break ;

        default:
            return FALSE ;
        break ;
   }

   return TRUE ;
}


BOOL WINAPI engPrint( HWND hWnd, LPSTR lpszFile, LPSTR lpszConfig )
{
#define FAILURE(dwErr) {dwLastError = dwErr;fResult=FALSE;goto Exit;}  
    BOOL        fResult = TRUE ;

    LPENGINEJOB lpJob ;
    LPENGINECONFIG lpEC ;
    DOCINFO     docinfo ;
    short       n ;
    HCURSOR     hcur ;
    char        szCfg[MAX_CONFIGNAMELEN] ;
    char        szAnsiFile[MAX_PATHLEN] ;
    RECT        rcPage ;
    UINT uiX, uiY ;
    UINT uiHorzSep, uiVertSep ;

    OemToAnsi( lpszFile, szAnsiFile ) ;

    if (lpszConfig == NULL)
    {
        if (!ftdConfigGetName( szCfg, szAnsiFile ))
        {
            /* Pop up a msg box saying that there is no configuration
             * associated with this file extension.  Tell him to use
             * "File.Associate" to set this up.
             *
             * Or pop up a list of file types to use with this file.
             *
             */
             return FALSE ;
        }

        lpszConfig = szCfg ;
    }

    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    if (!(lpJob = engJobCreate( hWnd, (LPARAM)lpszConfig, CREATEJOB_BYCONFIGNAME )))
    {
        DP1( hWDB, "engJobCreate Failed" ) ;
        SetCursor( hcur ) ;
        return FALSE ;
    }

    if (!(lpEC = lpJob->lpEC))
    {
        DP1( hWDB, "lpJob->lpEC == NULL!!!" ) ;
        DASSERT( hWDB, lpJob->lpEC ) ;
        engJobDestroy( lpJob ) ;
        SetCursor( hcur ) ;
        return FALSE ;
    }

    /* Load the FTD.  Make sure we free it later...
     */
    if (!(lpEC->lpfnFTD = ftdLoadFTD( lpEC->szFTDD, lpEC->szFTD )))
    {
        DP1( hWDB, "Could not load FTD for %s, %s", (LPSTR)lpEC->szFTDD, lpEC->szFTD ) ;
        engJobDestroy( lpJob ) ;
        SetCursor( hcur ) ;
        return FALSE ;
    }

    lpJob->lpszDocName = szAnsiFile;
    lpJob->lpszFile = lpszFile ;

    /* Setup abort proc.
     */
    lpJob->fUserAborted = FALSE ;
    lpJob->lpfnAbort = (ABORTPROC)fnDefaultAbortProc ;
    SetAbortProc( lpJob->hdc, lpJob->lpfnAbort ) ;

    /* Setup status dialog
     */
    if (!(lpJob->dwFlags & ENGJOB_NOSTATUSDLG))
    {
        lpJob->lpfnStatus = fnDefaultStatusDlg ;
        lpJob->hdlgStatus = CreateDialogParam( ghmodDLL, 
                                               MAKEINTRESOURCE( DLG_PRINTSTATUS ),
                                               lpJob->hwndParent, 
                                               lpJob->lpfnStatus,
                                               (LPARAM)lpJob ) ;
    }

    /* Do a StartDoc to setup print manager
     */
    docinfo.lpszDocName = lpJob->lpszDocName ;
    docinfo.lpszOutput = NULL ; // no printing to file here
    docinfo.cbSize = sizeof( docinfo ) + lstrlen( lpJob->lpszDocName ) ;

    if (SP_ERROR == StartDoc( lpJob->hdc, &docinfo ))
    {
        DP1( hWDB, "StartDoc Failed!" ) ;
        FAILURE( lpJob->dwResult = ENGERR_STARTDOCFAILED ) ;
    }

    /* Tell FTD we're starting a document.
     *
     * This gives it the file name and allows it to initialize itself.
     * it may allocate memory and will probably store a file handle
     * in lpEC->hFile.  Thus once this is done, FTD_ENDDOC, must
     * be called at a later point to nuke the memory and close the
     * file.
     */
    if (!(*lpEC->lpfnFTD)( FTD_STARTDOC, (LPARAM)lpJob, (LPARAM)NULL, (LPARAM)NULL ))
    {
        DP1( hWDB, "FTD_STARTDOC failed!" ) ;
        FAILURE( lpJob->dwResult = ENGERR_STARTDOCFAILED ) ;
    }

    rcPage = lpJob->rcPhysWorkspace ;
    
    uiX = lpJob->uiSubPageX ;
    uiY = lpJob->uiSubPageY ;
    uiVertSep = MulDiv( (UINT)lpEC->uiVertSep, lpJob->nPixelPer10CMX, 10000 ) ;
    uiHorzSep = MulDiv( (UINT)lpEC->uiHorzSep, lpJob->nPixelPer10CMY, 10000 ) ;

NewPage:
    
    DP3( hWDB, "Starting a page..." ) ;
    /* Start a page
     */
    n = StartPage( lpJob->hdc ) ;

    SetCursor( hcur ) ;

    if (n > 0)
    {
        UINT    row, col ;
        UINT    x, y ;

        lpJob->nPhysicalPage++ ;

        /* Update the page number in the status dlg...
         */
        SendMessage( lpJob->hdlgStatus, WM_USER, 0, (LPARAM)lpJob ) ;

        /* Paint to our little hearts content.
         */
        DP4( hWDB, "Entering engPaintPage..." ) ;
        engPaintPage( lpJob, NULL ) ;
        DP4( hWDB, "Done with engPaintPage..." ) ;

        /* Now for each sub-page have the filetype driver
         * paint.
         */

        for (row = 0 ; row < lpEC->uiPageRows ; row++ )
        {
            for (col = 0 ; col < lpEC->uiPageColumns ; col++ )
            {
                // if FTD says it's done then we're done.
                if (!(*lpEC->lpfnFTD)( FTD_QUERYDATALEFT, (LPARAM)lpJob, 0L, 0L ))
                {
                    DP3( hWDB, "FTD_QUERYDATALEFT returned 0" ) ;
                    row = lpEC->uiPageRows ;
                    col = lpEC->uiPageColumns ;
                    continue ;
                }

                // Get dimensions of logical page
                x = rcPage.left + COL_POS(col, lpJob) + (col * uiVertSep) ;
                y = rcPage.top + ROW_POS(row, lpJob) + (row * uiHorzSep) ;
   
                // Set window extents so that the ftd sees (0,0)
                SetViewportOrg( lpJob->hdc, x, y ) ;
                
                // Pass hdc to ftd
                if (!(*lpEC->lpfnFTD)( FTD_OUTPUTPAGE, (LPARAM)lpJob, (LPARAM)uiX, (LPARAM)uiY ))
                {
                    DP1( hWDB, "FTD_OUTPUTPAGE returned an error!" ) ;
                    lpJob->dwResult = ENGERR_FTDOUTPUTFAILED ;
                    goto Exit ;
                }

                // Set origin back...
                SetViewportOrg( lpJob->hdc, 0, 0 ) ;
            }
        }

        if (lpJob->fUserAborted)
        {
            DP1( hWDB, "fUserAborted == TRUE after endPaintPage!" ) ;
            lpJob->dwResult = ENGERR_USERABORTED ;
            goto Exit ;
        }

        /* End the page
         */
        if ((n = EndPage( lpJob->hdc )) < 0)
        {
            switch (n)
            {
                case SP_APPABORT:
                    // AbortProc returned zero
                    DP1( hWDB, "AbortProc returned zero" ) ;
                break ;
    
                case SP_USERABORT:
                    // Job was canceled from the print manager
                    DP1( hWDB, "Job was canceled from the print manager" ) ;
                break ;
    
                case SP_OUTOFDISK:
                    // out of disk space, and none's gonna become available
                    DP1( hWDB, "out of disk space, and none's gonna become available" ) ;
                break ;
    
                case SP_OUTOFMEMORY:
                    // Not enough memory.
                    DP1( hWDB, "Not enough memory." ) ;
                break ;
    
                case SP_ERROR:
                default:
                    // General error
                    DP1( hWDB, "General error (EndPage == %04X)", n ) ;
                break ;
            }

            lpJob->fUserAborted = TRUE ;
            lpJob->dwResult = MAKELONG( ENGERR_ENDPAGEFAILED, n ) ;
            FAILURE( ENGERR_ENDPAGEFAILED ) ;
        }

        /* Do we need more physical pages?
         *
         * Ask the filetype driver.
         */
        if ((*lpEC->lpfnFTD)( FTD_QUERYDATALEFT, (LPARAM)lpJob, 0L, 0L ))
            goto NewPage ;

    }
    else
    {
        DP1( hWDB, "StartPage failed! (%04X)", n ) ;
        lpJob->fUserAborted = TRUE ;
        lpJob->dwResult = MAKELONG( ENGERR_STARTPAGEFAILED, n ) ;
        FAILURE( ENGERR_STARTPAGEFAILED ) ;
    }
  
Exit:

    DP3( hWDB, "Exit:" );

    if (lpEC && lpEC->lpfnFTD)
    {
        (*lpEC->lpfnFTD)( FTD_ENDDOC, (LPARAM)lpJob, (LPARAM)NULL, (LPARAM)NULL ) ;
        ftdFreeFTD( lpEC->lpfnFTD ) ;
        lpEC->lpfnFTD = NULL ;  
    }

    if (lpJob->fUserAborted)
        AbortDoc( lpJob->hdc ) ;
    else
        EndDoc( lpJob->hdc ) ;

    if (lpJob->hdlgStatus)
        DestroyWindow( lpJob->hdlgStatus ) ;

    engJobDestroy( lpJob ) ;

    SetCursor( hcur ) ;

    return fResult ;

#undef FAILURE
}


/************************************************************************
 * End of File: PRINTER.c
 ***********************************************************************/

