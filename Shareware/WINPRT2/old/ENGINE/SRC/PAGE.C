/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  PAGE 
 *
 *      Module:  PAGE.C
 * 
 *      This module contians the main entry points for the Wi\nPrint 2.0
 *      engine Header/Footer stuff.  The Header/Footer setup dialog
 *      box is also handled here.
 *
 *   Revisions:  
 *       9/11/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"
#include "FTD.H"

BOOL INTERNAL SetPrinter( HWND hDlg, LPENGINECONFIG lpEC ) ;
BOOL INTERNAL SetOrient( HWND hDlg, LPENGINECONFIG lpEC ) ;
BOOL INTERNAL SetMultiplePagesUp( HWND hDlg, LPENGINECONFIG lpEC, BOOL fSetText ) ;
LRESULT INTERNAL CallFTDDlgProc( LPENGINECONFIG lpEC, HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL INTERNAL AddFTDControls( HWND hDlg, LPENGINECONFIG lpEC ) ;

#define HWNDPREVIEW             (HWND)GetProp(hDlg,MAKEINTATOM(1))
#define SETHWNDPREVIEW(hwnd)    (HWND)SetProp(hDlg,MAKEINTATOM(1),hwnd)
#define REMOVEHWNDPREVIEW       (HWND)RemoveProp(hDlg,MAKEINTATOM(1))

LRESULT CALLBACK fnPageConfigure( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPENGINECONFIG lpEC ;

    lpEC = (LPENGINECONFIG)GetWindowLong( hDlg, DWL_USER ) ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            HFONT hfont ;
            char  szBuf[MAX_CONFIGNAMELEN + 64] ;
            HCURSOR hcur ;
    
            hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

            hfont = ReallyCreateFontEx( NULL, "Helv", "Regular", 8, RCF_NORMAL ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_CURPRINTER ), hfont, FALSE ) ;
//            SetWindowFont( GetDlgItem( hDlg, IDD_FILETYPEDRIVER ), hfont, FALSE ) ;
    
            SetWindowLong( hDlg, DWL_USER, lParam ) ;
            lpEC = (LPENGINECONFIG)lParam ;

            SETHWNDPREVIEW(GetDlgItem( hDlg, IDD_SAMPLE )) ;
            engPreviewSetConfig( HWNDPREVIEW, lpEC ) ;

            /* Add controls for FTD.
             */
            AddFTDControls( hDlg, lpEC ) ;

            if (!SetPrinter( hDlg, lpEC ))
                break ;

            SetOrient( hDlg, lpEC ) ;

            SetMultiplePagesUp( hDlg, lpEC, TRUE ) ;    // set the edits
            SetMultiplePagesUp( hDlg, lpEC, FALSE ) ;   // set the radio buttons

            /* Associate muScrolls with their edits */
            engDlgSetMeasure( hDlg, IDD_LEFTMARGIN, lpEC->uiLeftMargin ) ;
            engDlgSetMeasure( hDlg, IDD_RIGHTMARGIN, lpEC->uiRightMargin ) ;
            engDlgSetMeasure( hDlg, IDD_TOPMARGIN, lpEC->uiTopMargin ) ;
            engDlgSetMeasure( hDlg, IDD_BOTTOMMARGIN, lpEC->uiBottomMargin ) ;

            engDlgSetMeasure( hDlg, IDD_HORZSEP, lpEC->uiHorzSep ) ;
            engDlgSetMeasure( hDlg, IDD_VERTSEP, lpEC->uiVertSep ) ;

            if (lpEC->dwShadeFlags)
                CheckDlgButton( hDlg, IDD_SHADEBORDERS, 1 ) ;

            if (lpEC->dwBorderFlags)
                CheckDlgButton( hDlg, IDD_BORDERLINES, 1 ) ;

            /* Set the window text to "Page Setup - ConfigName"
             */
            wsprintf( szBuf, GRCS(IDS_PAGESETUP), (LPSTR)lpEC->szConfigName ) ;
            SetWindowText( hDlg, szBuf ) ;
            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
            // return TRUE ;    // Focus will be set to first control

            SetFocus( GetDlgItem( hDlg, IDD_LEFTMARGIN ) ) ;

            SetCursor( hcur ) ;
            
            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;

            return FALSE ;   // We set focus
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

            switch (wID)
            {
                case IDD_LANDSCAPE:
                    if (wNotifyCode == BN_CLICKED && lpEC->dmOrientation != DMORIENT_LANDSCAPE)
                    {
                        lpEC->dmOrientation = DMORIENT_LANDSCAPE ;
                        SetOrient( hDlg, lpEC ) ;
                    }
                break ;

                case IDD_PORTRAIT:
                    if (wNotifyCode == BN_CLICKED && lpEC->dmOrientation != DMORIENT_PORTRAIT)
                    {
                        lpEC->dmOrientation = DMORIENT_PORTRAIT ;
                        SetOrient( hDlg, lpEC ) ;
                    }
                break ;

                case IDD_LEFTMARGIN:
                    if (wNotifyCode == EN_KILLFOCUS)
                    {
                        UINT ui = engDlgGetMeasure( hDlg, wID ) ;

                        if (ui != lpEC->uiLeftMargin)
                        {
                            lpEC->uiLeftMargin = ui ;
                            engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                        }
                        engDlgSetMeasure( hDlg, wID, ui ) ;
                    }
                break ;

                case IDD_RIGHTMARGIN:
                    if (wNotifyCode == EN_KILLFOCUS)
                    {
                        UINT ui = engDlgGetMeasure( hDlg, wID ) ;
                        if (ui != lpEC->uiRightMargin)
                        {
                            lpEC->uiRightMargin = ui ;
                            engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                        }
                        engDlgSetMeasure( hDlg, wID, ui ) ;
                    }
                break ;

                case IDD_TOPMARGIN:
                    if (wNotifyCode == EN_KILLFOCUS)
                    {
                        UINT ui = engDlgGetMeasure( hDlg, wID ) ;
                        if (ui != lpEC->uiTopMargin)
                        {
                            lpEC->uiTopMargin = ui ;
                            engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                        }
                        engDlgSetMeasure( hDlg, wID, ui ) ;
                    }
                break ;

                case IDD_BOTTOMMARGIN:
                    if (wNotifyCode == EN_KILLFOCUS)
                    {
                        UINT ui = engDlgGetMeasure( hDlg, wID ) ;
                        if (ui != lpEC->uiBottomMargin)
                        {
                            lpEC->uiBottomMargin = ui ;
                            engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                        }
                        engDlgSetMeasure( hDlg, wID, ui ) ;
                    }
                break ;

                case IDD_HORZSEP:
                    if (wNotifyCode == EN_KILLFOCUS)
                    {
                        UINT ui = engDlgGetMeasure( hDlg, wID ) ;
                        if (ui != lpEC->uiHorzSep)
                        {
                            lpEC->uiHorzSep = ui ;
                            engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                        }
                        engDlgSetMeasure( hDlg, wID, ui) ;
                    }
                break ;

                case IDD_VERTSEP:
                    if (wNotifyCode == EN_KILLFOCUS)
                    {
                        UINT ui = engDlgGetMeasure( hDlg, wID ) ;
                        if (ui != lpEC->uiVertSep)
                        {
                            lpEC->uiVertSep = ui ;
                            engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                        }
                        engDlgSetMeasure( hDlg, wID, ui ) ;
                    }
                break ;

                case IDD_ONEPAGEUP:
                    if (wNotifyCode == BN_CLICKED && 
                        (lpEC->uiPageRows != 1 || lpEC->uiPageColumns != 1))
                    {
                        lpEC->uiPageRows = 1 ;
                        lpEC->uiPageColumns = 1 ;
                        SetMultiplePagesUp( hDlg, lpEC, TRUE ) ;
                    }
                break ;

                case IDD_TWOPAGESUP:
                    if (wNotifyCode == BN_CLICKED &&
                        (lpEC->uiPageRows != 1 || lpEC->uiPageColumns != 2))
                    {
                        lpEC->uiPageRows = 1 ;
                        lpEC->uiPageColumns = 2 ;
                        SetMultiplePagesUp( hDlg, lpEC, TRUE ) ;
                    }
                break ;

                case IDD_FOURPAGESUP:
                    if (wNotifyCode == BN_CLICKED &&
                        (lpEC->uiPageRows != 2 || lpEC->uiPageColumns != 2))
                    {
                        lpEC->uiPageRows = 2 ;
                        lpEC->uiPageColumns = 2 ;
                        SetMultiplePagesUp( hDlg, lpEC, TRUE ) ;
                    }
                break ;

                case IDD_CUSTOM:
                    if (wNotifyCode == BN_CLICKED)
                    {
                        SetFocus( GetDlgItem( hDlg, IDD_ROWS ) ) ;
                    }
                break ;

                case IDD_ROWS:
                    if (wNotifyCode == EN_CHANGE)
                    {
                        lpEC->uiPageRows = GetDlgItemInt( hDlg, IDD_ROWS, NULL, FALSE ) ;
                        SetMultiplePagesUp( hDlg, lpEC, FALSE ) ;
                    }
                break ;

                case IDD_COLUMNS:
                    if (wNotifyCode == EN_CHANGE)
                    {
                        lpEC->uiPageColumns = GetDlgItemInt( hDlg, IDD_COLUMNS, NULL, FALSE ) ;
                        SetMultiplePagesUp( hDlg, lpEC, FALSE ) ;
                    }
                break ;

                case IDD_DLG_HF:
                    if (engHFConfigure( hDlg, lpEC ))
                        engPreviewReDraw( HWNDPREVIEW, FALSE, FALSE ) ;
                break ;

                case IDD_FTD:
                    if (ftdChoose( hDlg, lpEC ))
                    {
                        SetWindowRedraw( hDlg, FALSE ) ;
                        AddFTDControls( hDlg, lpEC ) ;
                        SetWindowRedraw( hDlg, TRUE ) ;
                        InvalidateRect( hDlg, NULL, TRUE ) ;
                        UpdateWindow( hDlg ) ;
                    }
                break ;

                case IDD_DLG_PRNSETUP:  // PrintDlg() :
                    if (engPrinterChoose( hDlg, lpEC ))
                    {
                        SetPrinter( hDlg, lpEC ) ;
                        SetOrient( hDlg, lpEC ) ;
                    }
                break ;

                case IDD_SHADEBORDERS:
                    if (wNotifyCode == BN_CLICKED)
                    {
                        if (IsDlgButtonChecked( hDlg, wID ))
                            lpEC->dwShadeFlags |= ENG_SHADE_LTGRAY ;
                        else
                            lpEC->dwShadeFlags &= ~ENG_SHADE_LTGRAY ;
                        engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                    }
                break ;

                case IDD_BORDERLINES:
                    if (wNotifyCode == BN_CLICKED)
                    {
                        if (IsDlgButtonChecked( hDlg, wID ))
                            lpEC->dwBorderFlags |= (ENG_TOPBORDER | 
                                                   ENG_LEFTBORDER | 
                                                   ENG_RIGHTBORDER | 
                                                   ENG_BOTTOMBORDER) ;
                        else
                            lpEC->dwBorderFlags &= ~(ENG_TOPBORDER | 
                                                     ENG_LEFTBORDER | 
                                                     ENG_RIGHTBORDER | 
                                                     ENG_BOTTOMBORDER) ;
                        engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
                    }
                break ;

                case IDOK:
                {
                    EndDialog (hDlg, TRUE) ;
                }
                break ;

                case IDCANCEL:
                    EndDialog (hDlg, FALSE) ;
                break ;

                case IDD_HELP:
                    DASSERTMSG( hWDB, 0, "Yo! No help yet." ) ;   
                break ;

                case IDD_SAMPLE:
                {
                    HWND hwndPreview = HWNDPREVIEW ;
        
                    DP1( hWDB, "IDD_SAMPLE" ) ;
         
                    if (!IsWindow( hwndPreview ))
                    {
                        DP1( hWDB, "No Preview window!" ) ;
                        break ;
                    }
        
                    if (hwndPreview == GetDlgItem( hDlg, IDD_SAMPLE))
                    {
                        engPreviewSetConfig( hwndPreview, NULL ) ;
                        InvalidateRect( hwndPreview, NULL, TRUE ) ;
        
                        hwndPreview = engPreviewCreate( GetParent( hDlg ), 
                                    WS_OVERLAPPED | 
                                    WS_CAPTION |
                                    WS_THICKFRAME |
                                    WS_MAXIMIZEBOX,
                                    10, 10, 500, 450, lpEC ) ;
                        SETHWNDPREVIEW(hwndPreview) ;
                        ShowWindow( hwndPreview, SW_SHOWNORMAL ) ;
                        SetActiveWindow( hDlg ) ;
                    }
                    else
                    {
                        engPreviewDestroy( hwndPreview ) ;
        
                        hwndPreview = GetDlgItem( hDlg, IDD_SAMPLE ) ;
                        engPreviewSetConfig( hwndPreview, lpEC ) ;
                        SETHWNDPREVIEW(hwndPreview) ;
                        InvalidateRect( hwndPreview, NULL, TRUE ) ;
                    }
                }
                break ;

                default :
                   return CallFTDDlgProc( lpEC, hDlg, uiMsg, wParam, lParam ) ;
            }
        }
        break ;


        case WM_VSCROLL:
        {
            #ifdef WIN32
            #else
            UINT wScrollCode = wParam ;      /* scroll bar code             */
            UINT nPos = LOWORD(lParam) ;     /* current scroll box position */
            HWND hwndMu = (HWND) HIWORD(lParam) ;  
            #endif
            UINT    wID ;
            int     n ;

            if (wScrollCode == SB_THUMBTRACK)
                break ;

            wID = GetDlgCtrlID( hwndMu ) ;

            switch (wID)
            {
                case IDD_ROWS_MU:
                    n = GetDlgItemInt( hDlg, IDD_ROWS, NULL, FALSE ) ;
                    if (wScrollCode == SB_LINEUP)
                    {
                        if (n < MAX_ROWSCOLUMNS)
                            n++ ;
                    }
                    else
                    {
                        if (n > 1)
                            n-- ;
                    }

                    SetDlgItemInt( hDlg, IDD_ROWS, n, FALSE ) ;
                    lpEC->uiPageRows = (UINT)n ;
                break ;

                case IDD_COLUMNS_MU:
                    n = GetDlgItemInt( hDlg, IDD_COLUMNS, NULL, FALSE ) ;
                    if (wScrollCode == SB_LINEUP)
                    {
                        if (n < MAX_ROWSCOLUMNS)
                            n++ ;
                    }
                    else
                    {
                        if (n > 1)
                            n-- ;
                    }

                    SetDlgItemInt( hDlg, IDD_COLUMNS, n, FALSE ) ;

                    lpEC->uiPageColumns = (UINT)n ;
                break ;

                default:
                    return CallFTDDlgProc( lpEC, hDlg, uiMsg, wParam, lParam ) ;
            }
        }
        break ;

        case WM_CTLCOLOR:
            return (BOOL)Ctl3dCtlColorEx( uiMsg, wParam, lParam ) ;
        break ;

        case WM_DESTROY:
        {
            HWND hwndPreview = HWNDPREVIEW ;

            REMOVEHWNDPREVIEW ;

            if (hwndPreview && hwndPreview != GetDlgItem( hDlg, IDD_SAMPLE ))
            {
                engPreviewDestroy( hwndPreview ) ;
            }

            DeleteObject( GetWindowFont( GetDlgItem( hDlg, IDD_CURPRINTER ) ) ) ;

            SetWindowFont( GetDlgItem( hDlg, IDD_CURPRINTER ), 
                            GetWindowFont( GetDlgItem( hDlg, IDD_FTD ) ), FALSE ) ;

            if (lpEC->lpfnFTD)
            {
                CallFTDDlgProc( lpEC, hDlg, uiMsg, wParam, lParam ) ;
                ftdFreeFTD( lpEC->lpfnFTD ) ;
                lpEC->lpfnFTD = NULL ;
            }
        }
        break ;

        default:
            return CallFTDDlgProc( lpEC, hDlg, uiMsg, wParam, lParam ) ;


   }
   return TRUE ;
}

LRESULT INTERNAL CallFTDDlgProc( LPENGINECONFIG lpEC, HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    MSG msg ;

    if (lpEC == NULL)
    {
        return TRUE ;
    }

    msg.hwnd = hDlg ;
    msg.message = uiMsg ;
    msg.wParam = wParam ;
    msg.lParam = lParam ;

    if (lpEC->lpfnFTD)
        return (LRESULT)(*lpEC->lpfnFTD)( FTD_CONFIGDLGMSG, (LPARAM)(LPMSG)&msg, (LPARAM)NULL, (LPARAM)NULL ) ;
                                
    return (LRESULT)0L ;
}

BOOL INTERNAL AddFTDControls( HWND hDlg, LPENGINECONFIG lpEC )
{
    char szBuf[MAX_CONFIGNAMELEN] ;
    char szFTD[MAX_CONFIGNAMELEN] ;
    HWND hwnd ;
    HWND hwndCtl ;
    MSG  msg ;
    LPDLGBOXHEADER lpDlg ;

    if (*lpEC->szFTD)
        lstrcpy( szFTD, lpEC->szFTD ) ;
    else
        lstrcpy( szFTD, GRCS(IDS_DEFAULT_FTD) ) ;

    wsprintf( szBuf, GRCS(IDS_FILETYPEDRIVER), (LPSTR)szFTD ) ;
    SetDlgItemText( hDlg, IDD_FILETYPEDRIVER, szBuf ) ;

    msg.hwnd = hDlg ;
    msg.message = WM_DESTROY ;
    msg.wParam = NULL ;
    msg.lParam = NULL ;
    /* Tell previous FTD dialog code that we're nuking it.
     */
    if (lpEC->lpfnFTD)
        (*lpEC->lpfnFTD)( FTD_CONFIGDLGMSG, (LPARAM)(LPMSG)&msg, (LPARAM)NULL, (LPARAM)NULL ) ;

    /* Remove controls from previous FTD.
     * 
     * When we add controls, we put them in the zorder after
     * the IDD_FILETYPEDRIVER group box.  That way, when we
     * go to delete them, we just enumerate all controls
     * after IDD_FILETYPEDRIVER.
     */
    hwnd = GetDlgItem( hDlg, IDD_FILETYPEDRIVER ) ;
    
    while (hwndCtl = GetWindow( hwnd, GW_HWNDNEXT ))
    {
        GetWindowText( hwndCtl, szBuf, 80 ) ;
        DestroyWindow( hwndCtl ) ;
    }

    /* Unload previous FTD
     */
    if (lpEC->lpfnFTD)
    {
        ftdFreeFTD( lpEC->lpfnFTD ) ;
        lpEC->lpfnFTD = NULL ;  
    }
    
    /* Load new FTD 
     */
    if (!(lpEC->lpfnFTD = ftdLoadFTD( lpEC->szFTDD, lpEC->szFTD )))
    {
        DP1( hWDB, "Could not load FTD for %s, %s", (LPSTR)lpEC->szFTDD, lpEC->szFTD ) ;
        return FALSE ;
    }

    /* Read config, if any...
     */
//    (*lpfnFTD)( FTD_READCONFIG, (LPARAM)&lpEC->FTDInfo, (LPARAM)lpszSection, (LPARAM)lpszFile ) ;

    /* Add controls for FTD.  This involves first getting the
     * CONFIGDLG dialog box template from the FTD, then
     * going through that template creating controls as needed.
     */
    lpDlg = (LPDLGBOXHEADER)(*lpEC->lpfnFTD)( FTD_GETDLGTEMPLATE, 0L, 0L, 0L ) ;
    hwnd = GetDlgItem( hDlg, IDD_FILETYPEDRIVER ) ;

    if (lpDlg)
    {
        BYTE    n ;
        LPCTRLDATA      lpCtrl ;
        HWND hwndCtrl ;
        int x,y,cx,cy ;
        int cxChar, cyChar ;
        TEXTMETRIC tm ;
        HDC hdc ;
        RECT rcGroup ;
        HFONT hfont = GetWindowFont( hDlg ) ;
        
        if (hdc = GetDC( hDlg ))
        {
            hfont = SelectObject( hdc, hfont ) ;
            cxChar = GetTextMetricsCorrectly( hdc, &tm ) ;
            cyChar = tm.tmHeight ;
            hfont = SelectObject( hdc, hfont ) ;
            ReleaseDC( hDlg, hdc ) ;
        }

        GetWindowRect( hwnd, &rcGroup ) ;
        MapWindowRect( NULL, hDlg, &rcGroup ) ;

        for (n = 0 ; n < lpDlg->bNumberOfItems ; n++)
        {
            lpCtrl = (LPCTRLDATA)(((LPSTR)lpDlg)+ sizeof(DLGBOXHEADER) +
                                           (sizeof(CTRLDATA) * n)) ;

            if (lpCtrl->wID == -1 || lpCtrl->wID >= 5000)
            {
                x = MulDiv(lpCtrl->x, cxChar, 4) + rcGroup.left + (cxChar / 2) ;
                y = MulDiv(lpCtrl->y, cyChar, 8) + rcGroup.top + cyChar ;
                cx = MulDiv(lpCtrl->cx, cxChar, 4);
                cy = MulDiv(lpCtrl->cy, cyChar, 8);
    
                hwndCtrl =  CreateWindow( lpCtrl->szClass, 
                                          lpCtrl->szText, 
                                          lpCtrl->lStyle | WS_VISIBLE, 
                                          x, 
                                          y, 
                                          cx, 
                                          cy,
                                          hDlg,
                                          (HMENU)lpCtrl->wID,
                                          GetWindowInstance( hDlg ),
                                          NULL ) ;
                if (hwndCtrl)
                {
                    SetWindowFont( hwndCtrl, hfont, FALSE ) ;
                    Ctl3dSubclassCtl( hwndCtrl ) ;
                }   
            }
        }

        msg.message = WM_INITDIALOG ;
        msg.lParam = (LPARAM)lpEC ;
        (*lpEC->lpfnFTD)( FTD_CONFIGDLGMSG, (LPARAM)(LPMSG)&msg, (LPARAM)NULL, (LPARAM)NULL ) ;
    }
    else
    {
        DP1( hWDB, "No dialog template for this FTD" ) ;
    }
    return TRUE ;
}


BOOL INTERNAL SetPrinter( HWND hDlg, LPENGINECONFIG lpEC )
{
    char  szBuf[MAX_DEVICELEN+MAX_OUTPUTLEN+64] ;
    char  szOutput[MAX_UNCSHARELEN] ;
    UINT  cb ;

    cb = MAX_UNCSHARELEN ;

    if (lpEC->dwFlags & ENGCFG_USEDEFAULTPRN)
    {
        if (engPrinterGetDefault( hDlg, lpEC ))
        {
            if (WN_SUCCESS == WNetGetConnection( lpEC->szOutput, szOutput, &cb ))
                wsprintf( szBuf, "Default Printer (%s on %s (%s))", 
                                (LPSTR)lpEC->szDevice, (LPSTR)szOutput, 
                                (LPSTR)lpEC->szOutput ) ;
            else
                wsprintf( szBuf, "Default Printer (%s on %s)", 
                                (LPSTR)lpEC->szDevice, (LPSTR)lpEC->szOutput ) ;
        }
        else
        {
            EndDialog( hDlg, FALSE ) ;
            return FALSE ;
        }
    }
    else
    {
        if (WN_SUCCESS == WNetGetConnection( lpEC->szOutput, szOutput, &cb ))
            wsprintf( szBuf, "%s on %s (%s)", (LPSTR)lpEC->szDevice, 
                                              (LPSTR)szOutput, (LPSTR)lpEC->szOutput ) ;
        else
            wsprintf( szBuf, "%s on %s", (LPSTR)lpEC->szDevice, 
                                         (LPSTR)lpEC->szOutput ) ;
    }

    SetDlgItemText( hDlg, IDD_CURPRINTER, szBuf ) ;

    return TRUE ;
}

BOOL INTERNAL SetOrient( HWND hDlg, LPENGINECONFIG lpEC )
{
    WORD wID ;
    HCURSOR hcur ;
    hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

    if (lpEC->dmFields & DM_ORIENTATION)
    {
        if (lpEC->dmOrientation == DMORIENT_LANDSCAPE)
            wID = IDD_LANDSCAPE ;
        else
            wID = IDD_PORTRAIT ;

        EnableWindow( GetDlgItem( hDlg, IDD_PORTRAIT ), TRUE ) ;
        EnableWindow( GetDlgItem( hDlg, IDD_LANDSCAPE ), TRUE ) ;

        CheckRadioButton( hDlg, IDD_PORTRAIT, IDD_LANDSCAPE, wID ) ;
    }
    else
    {
        wID = IDD_NOORIENT ;
        EnableWindow( GetDlgItem( hDlg, IDD_PORTRAIT ), FALSE ) ;
        EnableWindow( GetDlgItem( hDlg, IDD_LANDSCAPE ), FALSE ) ;
    }

    DestroyIcon( (HICON)SendDlgItemMessage( hDlg, IDD_ORIENT, 
                     STM_SETICON, (WPARAM)LoadIcon( ghmodDLL, 
                                       MAKEINTRESOURCE( wID ) ), 0L ) ) ;
    engPreviewReDraw( HWNDPREVIEW, TRUE, TRUE ) ;

    SetCursor( hcur ) ;

    return TRUE ;                                
}

BOOL INTERNAL SetMultiplePagesUp( HWND hDlg, LPENGINECONFIG lpEC, BOOL fSetText )
{
    UINT wID ;

    if (lpEC->uiPageRows == 0 || lpEC->uiPageColumns == 0 || 
        (lpEC->uiPageRows == 1 && lpEC->uiPageColumns == 1))
    {
        lpEC->uiPageRows = lpEC->uiPageColumns = 1 ;
        wID = IDD_ONEPAGEUP ;
    }
    else if (lpEC->uiPageRows == 1 && lpEC->uiPageColumns == 2)
        wID = IDD_TWOPAGESUP ;
    else if (lpEC->uiPageRows == 2 && lpEC->uiPageColumns == 2)
        wID = IDD_FOURPAGESUP ;
    else
        wID = IDD_CUSTOM ;

    if (fSetText)        
    {
        if (GetDlgItemInt( hDlg, IDD_ROWS, NULL, FALSE ) != lpEC->uiPageRows)
            SetDlgItemInt( hDlg, IDD_ROWS, lpEC->uiPageRows, FALSE ) ;
        if (GetDlgItemInt( hDlg, IDD_COLUMNS, NULL, FALSE ) != lpEC->uiPageColumns)
            SetDlgItemInt( hDlg, IDD_COLUMNS, lpEC->uiPageColumns, FALSE ) ;
    }
    else
    {
        CheckRadioButton( hDlg, IDD_ONEPAGEUP, IDD_CUSTOM, wID ) ;
        engPreviewReDraw( HWNDPREVIEW, FALSE, TRUE ) ;
    }

    return TRUE ;

}    

BOOL WINAPI engPageConfigure( HWND hwndParent, LPENGINECONFIG lpEC )
{
    LPENGINECONFIG    lpec ;

    /* Copy the ENGINECONFIG structure */
    if (lpec = engConfigCopy( NULL, lpEC ))
    {
    
        if (DialogBoxParam( ghmodDLL, MAKEINTRESOURCE( DLG_PAGESETUP ), 
                            hwndParent, (DLGPROC)fnPageConfigure, (LPARAM)lpec ))
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
 * End of File: PAGE.C
 ***********************************************************************/

