/************************************************************************
 *                
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASSOCIAT
 *
 *      Module:  ASSOCIAT.C
 * 
 *      This module contains the file type driver support routines.
 *      These routines are used by the file type drivers (ASSOCIATs) and
 *      file type driver dlls (ASSOCIATDs) to manage file types.
 *
 *   Revisions:  
 *       9/21/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include <shellapi.h>

#include "ENGINE.h"


#define MAX_COMMANDLEN      (MAX_PATHLEN + 128)
#define MAX_DDETOPIC        128

typedef struct tagACTION
{
    char    szCommand[MAX_COMMANDLEN] ;
    BOOL    fUseDDE ;
    char    szDDEMsg[MAX_COMMANDLEN] ;
    char    szDDEApp[MAX_DDETOPIC] ;
    char    szDDETopic[MAX_DDETOPIC] ;
    char    szDDENotRunning[MAX_COMMANDLEN] ;
} ACTION, FAR *LPACTION ;

typedef struct tagFT
{
    char    szExt[5] ;
    char    szIdent[64] ;
    char    szFileType[MAX_CONFIGNAMELEN] ;
    
    BOOL    fUseWinPrint ;
    ACTION  actPrint ;
    ACTION  actOpen ;    

} FT, FAR *LPFT ;

typedef struct tagASS
{
    char    szExt[5] ;
    char    szIdent[MAX_CONFIGNAMELEN] ;
} ASS, FAR *LPASS ;    

BOOL INTERNAL ReadAssociations( HWND hDlg ) ;
BOOL INTERNAL GetIdentFromExt( HKEY hkRoot, LPSTR lpszExt, LPSTR lpszIdent ) ;

BOOL INTERNAL GetFileTypeFromExt( HKEY hkRoot, LPSTR lpszExt, LPSTR lpszFileType ) ;

BOOL INTERNAL ReadIdent( HWND hwnd, HKEY hkRoot, LPFT lpft ) ;
BOOL INTERNAL WriteIdent( HWND hwnd, HKEY hkRoot, LPFT lpft ) ;
BOOL INTERNAL WriteIdent_ExtOnly( HWND hwnd, HKEY hkRoot, LPFT lpft ) ;

BOOL INTERNAL ReadAction( HWND hwnd, HKEY hk, LPACTION lpAct ) ;


static  FT gft ;

LRESULT CALLBACK fnAssociate( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPASS lpAss ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            HCURSOR hcur ;
            HWND    hwnd ;
            LPFT    lpft ;
            BOOL    f ;
            hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

            SetWindowLong( hDlg, DWL_USER, lParam ) ;
            lpAss = (LPASS)lParam ;
            DASSERT( hWDB, lpAss ) ;

            Edit_LimitText( GetDlgItem( hDlg, IDD_EXTENSION), 4 ) ;
            StripOffWhiteSpace( lpAss->szExt ) ;
            EnableWindow( GetDlgItem( hDlg, IDOK ), (BOOL)*lpAss->szExt ) ;

            Edit_SetText( GetDlgItem( hDlg, IDD_EXTENSION ), lpAss->szExt ) ;

            /* Fill file type list box and extension list box
             */
            ReadAssociations( hDlg )  ;

            lstrcpy( gft.szExt, lpAss->szExt ) ;
            hwnd = GetDlgItem( hDlg, IDD_FILETYPES ) ;
            if (GetFileTypeFromExt( HKEY_CLASSES_ROOT, gft.szExt, gft.szFileType ))
            {
                DP3( hWDB, "%s is Associated with %s", (LPSTR)gft.szExt, (LPSTR)gft.szFileType ) ;
                if (LB_ERR != ComboBox_SelectString( hwnd, 0, (LPSTR)&gft ))
                {
                    ComboBox_GetLBText( hwnd, ComboBox_GetCurSel( hwnd ), &lpft ) ;
                    ComboBox_SetText( hwnd, lpft->szFileType ) ;
                    f = TRUE ;
                }
                else
                {
                    ComboBox_SetCurSel( hwnd, -1 ) ;
                    ComboBox_SetText( hwnd, gft.szFileType ) ;
                    f = FALSE;
                }
            }
            else
            {
                ComboBox_SetCurSel( hwnd, 0 ) ;
                ComboBox_SetText( hwnd, GRCS(IDS_NONE) ) ;
                f = FALSE;
            }

            EnableWindow( GetDlgItem( hDlg, IDD_MODIFY ), f ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_COPY ), f ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_DELETE ), f ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
            SetCursor( hcur ) ;
            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;
            return TRUE ;
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

            if (!(lpAss = (LPASS)GetWindowLong( hDlg, DWL_USER )))
            {
                DASSERT( hWDB, lpAss ) ;
                EndDialog( hDlg, FALSE ) ;
            }

            switch (wID)
            {
                case IDD_EXTENSION:
                {
                    LPFT lpft ;
                    BOOL f ;

                    switch (wNotifyCode)
                    {
                        case EN_CHANGE:
                            Edit_GetText( hwndCtl, gft.szExt, MAX_CONFIGNAMELEN ) ;
                            StripOffWhiteSpace( gft.szExt ) ;

                            if (*gft.szExt == '\0')
                            {
                                EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;
                                return FALSE ;
                            }
                            EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;

                            hwndCtl = GetDlgItem( hDlg, IDD_FILETYPES );
                            if (GetFileTypeFromExt( HKEY_CLASSES_ROOT, gft.szExt, gft.szFileType ))
                            {
                                if (LB_ERR != ComboBox_SelectString( hwndCtl, 0, (LPSTR)&gft ))
                                {
                                    ComboBox_GetLBText( hwndCtl, ComboBox_GetCurSel( hwndCtl ), &lpft ) ;
                                    ComboBox_SetText( hwndCtl, lpft->szFileType ) ;
                                }
                                else
                                {
                                    ComboBox_SetCurSel( hwndCtl, -1 ) ;
                                    ComboBox_SetText( hwndCtl, gft.szFileType ) ;
                                }
                            }
                            else
                            {
                                ComboBox_SetCurSel( hwndCtl, 0 ) ;
                                ComboBox_SetText( hwndCtl, GRCS(IDS_NONE) ) ;
                            }

                            f = (ComboBox_GetCurSel( hwndCtl ) > 0) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_MODIFY ), f ) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_COPY ), f ) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_DELETE ), f ) ;

                        break ;

                        default:
                            return FALSE ;
                    }

                }
                break ;

                case IDD_FILETYPES:
                    switch (wNotifyCode)
                    {
                        case CBN_DBLCLK:
                            SendMessage( hDlg, WM_COMMAND, (WPARAM)IDOK, (LPARAM)(LPSTR)GetDlgItem( hDlg, IDOK ) ) ;
                        break ;

                        case CBN_SELCHANGE:
                        {
                            LPFT    lpft ;
                            UINT    ui ;
                            BOOL    f = FALSE ;

                            ui = ComboBox_GetCurSel( hwndCtl ) ;

                            if (ui != CB_ERR)
                            {
                                ComboBox_GetLBText( hwndCtl, ui, &lpft ) ;
                                ComboBox_SetText( hwndCtl, lpft->szFileType ) ;
                            }

                            f = (ui > 0) ;
                            
                            EnableWindow( GetDlgItem( hDlg, IDD_MODIFY ), f ) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_COPY ), f ) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_DELETE ), f ) ;
                        }
                        break ;

                        case CBN_EDITCHANGE:
                        {
                            BOOL f ;
    
                            ComboBox_GetText( hwndCtl, gft.szFileType, MAX_CONFIGNAMELEN ) ;
                            StripOffWhiteSpace( gft.szFileType ) ;

                            f = (LB_ERR != ComboBox_FindStringExact( hwndCtl, 0, (LPSTR)&gft )) ;

                            EnableWindow( GetDlgItem( hDlg, IDD_MODIFY ), f ) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_COPY ), f ) ;
                            EnableWindow( GetDlgItem( hDlg, IDD_DELETE ), f ) ;
                        }
                        break ;

                        case CBN_KILLFOCUS:
                        {
                            UINT    ui ;
    
                            ComboBox_GetText( hwndCtl, gft.szFileType, MAX_CONFIGNAMELEN ) ;
                            StripOffWhiteSpace( gft.szFileType ) ;
                            ui = ComboBox_FindStringExact( hwndCtl, 0, (LPSTR)&gft ) ;
                            if (ui != LB_ERR)
                                ComboBox_SetCurSel( hwndCtl, ui ) ;
                            
                        }
                        break ;
                    }
                break ;

                case IDD_NEW      :
                {
                    LPFT lpft ;
                    *gft.szIdent = '\0' ;
                    if (engFileTypeModify( hDlg, gft.szIdent ))
                    {
                        DWORD cb = MAX_CONFIGNAMELEN ;
                        HWND    hwndCtl = GetDlgItem( hDlg, IDD_FILETYPES ) ;
        
                        ComboBox_ResetContent( hwndCtl ) ;
                        ReadAssociations( hDlg )  ;

                        RegQueryValue( HKEY_CLASSES_ROOT, gft.szIdent, gft.szFileType, &cb ) ;

                        ComboBox_SelectString( GetDlgItem( hDlg, IDD_FILETYPES ), 0, (LPSTR)&gft ) ;
                        ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), 
                                        ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ), &lpft ) ;
                        ComboBox_SetText( GetDlgItem( hDlg, IDD_FILETYPES ), lpft->szFileType ) ;
                        SetDlgItemText( hDlg, IDCANCEL, GRCS( IDS_CLOSE ) ) ;
                    }
                }
                break ;

                case IDD_MODIFY   :
                {
                    UINT    uiItem = ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ) ;
                    LPFT    lpft ;
                    
                    if (uiItem != LB_ERR)
                    {
                        ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), uiItem, &lpft ) ;
                        lstrcpy( gft.szIdent, lpft->szIdent ) ;
                        if (engFileTypeModify( hDlg, gft.szIdent ))
                        {
                            DWORD cb = MAX_CONFIGNAMELEN ;

                            ComboBox_ResetContent( GetDlgItem( hDlg, IDD_FILETYPES ) ) ;
                            ReadAssociations( hDlg )  ;

                            RegQueryValue( HKEY_CLASSES_ROOT, gft.szIdent, gft.szFileType, &cb ) ;
                            ComboBox_SelectString( GetDlgItem( hDlg, IDD_FILETYPES ), 0, (LPSTR)&gft ) ;
                            ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), 
                                            ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ), &lpft ) ;
                            ComboBox_SetText( GetDlgItem( hDlg, IDD_FILETYPES ), lpft->szFileType ) ;
                            SetDlgItemText( hDlg, IDCANCEL, GRCS( IDS_CLOSE ) ) ;
                        }
                    }
                }
                break ;

                case IDD_COPY     :
                {
                    UINT    uiItem = ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ) ;
                    LPFT    lpft ;
                    
                    if (uiItem != LB_ERR)
                    {
                        char szNew[MAX_CONFIGNAMELEN] ;

                        *szNew = '\0' ;

                        ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), uiItem, &lpft ) ;

                        lstrcpy( gft.szIdent, lpft->szIdent ) ;
                        if (engFileTypeCopy( hDlg, gft.szIdent, szNew ))
                        {
                            DWORD cb = MAX_CONFIGNAMELEN ;

                            ComboBox_ResetContent( GetDlgItem( hDlg, IDD_FILETYPES ) ) ;
                            ReadAssociations( hDlg )  ;
                            lstrcpy( gft.szIdent, szNew ) ;
                            RegQueryValue( HKEY_CLASSES_ROOT, szNew, gft.szFileType, &cb ) ;
                            ComboBox_SelectString( GetDlgItem( hDlg, IDD_FILETYPES ), 0, (LPSTR)&gft ) ;
                            ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), 
                                            ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ), &lpft ) ;
                            ComboBox_SetText( GetDlgItem( hDlg, IDD_FILETYPES ), lpft->szFileType ) ;
                            SetDlgItemText( hDlg, IDCANCEL, GRCS( IDS_CLOSE ) ) ;
                        }
                    }

                }
                break ;

                case IDD_DELETE   :
                {
                    UINT    uiItem = ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ) ;
                    LPFT    lpft ;
                    
                    if (uiItem != LB_ERR)
                    {
                        ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), uiItem, &lpft ) ;

                        if (IDYES == ErrorResBox( hDlg, ghmodDLL, 
                                        MB_ICONEXCLAMATION | MB_YESNO, IDS_APPTITLE,
                                        IDS_ERR_DELETEFILETYPE, (LPSTR)lpft->szFileType ))
                        {
                            RegDeleteKey( HKEY_CLASSES_ROOT, (LPSTR)lpft->szIdent ) ;
                            ComboBox_ResetContent( GetDlgItem( hDlg, IDD_FILETYPES ) ) ;
                            ReadAssociations( hDlg )  ;
                            ComboBox_SetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ), uiItem ) ;
                            ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), 
                                            ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_FILETYPES ) ), &lpft ) ;
                            ComboBox_SetText( GetDlgItem( hDlg, IDD_FILETYPES ), lpft->szFileType ) ;
                            SetDlgItemText( hDlg, IDCANCEL, GRCS( IDS_CLOSE ) ) ;
                        }
                    }
                }
                break ;

                case IDOK:
                {
                    UINT    uiItem ;
                    LPFT    lpft ;
                    char    szExt[5] ;
                    
                    Edit_GetText( GetDlgItem( hDlg, IDD_EXTENSION ), lpAss->szExt, 5 ) ;
                    StripOffWhiteSpace( lpAss->szExt ) ;
                    if (*lpAss->szExt != '.')
                    {
                        *szExt = '.' ;
                        lstrcpy( szExt+1, lpAss->szExt ) ;
                    }
                    else
                        lstrcpy( szExt, lpAss->szExt ) ;

                    ComboBox_GetText( GetDlgItem( hDlg, IDD_FILETYPES ), gft.szFileType, MAX_CONFIGNAMELEN ) ;
                    StripOffWhiteSpace( gft.szFileType ) ;

                    if (*gft.szFileType == '\0' || (0 == lstrcmpi( gft.szFileType, GRCS(IDS_NONE) )))
                    {
                        DP1( hWDB, "No item selected!" ) ;

                        RegDeleteKey( HKEY_CLASSES_ROOT, szExt ) ; 
                        WriteProfileString( GRCS(IDS_EXTENSIONS), lpAss->szExt, NULL ) ;
                    }
                    else
                    {
                        uiItem = ComboBox_FindString( GetDlgItem( hDlg, IDD_FILETYPES ), 1, &gft ) ;
                        if (uiItem != LB_ERR)
                        {
                            ComboBox_GetLBText( GetDlgItem( hDlg, IDD_FILETYPES ), uiItem, &lpft ) ;
                            RegSetValue( HKEY_CLASSES_ROOT, szExt, REG_SZ, lpft->szIdent, 0 ) ;                        
                        }
                        else
                        {
                            #pragma message( "NEED: Verify extistence of file first!" )

                            /* It goes in the WIN.INI *and* in the registry
                             *
                             */
                            wsprintf( gft.actOpen.szCommand, "%s ^%s", (LPSTR)gft.szFileType, (LPSTR)szExt ) ;
                            WriteProfileString( GRCS(IDS_EXTENSIONS), lpAss->szExt, gft.actOpen.szCommand ) ;

                            wsprintf( gft.actOpen.szCommand, "%s %%1", (LPSTR)gft.szFileType ) ;
                            lstrcpy( gft.szExt, lpAss->szExt ) ;
                            lstrcpy( gft.szIdent, szExt ) ;
                            lstrcpy( gft.szFileType, szExt ) ;
                            WriteIdent_ExtOnly( hDlg, HKEY_CLASSES_ROOT, &gft ) ;
                        }
                    }

                    EndDialog (hDlg, wID) ;
                }
                break ;

                case IDCANCEL:
                    EndDialog (hDlg, wID) ;
                break ;

                default :
                   return FALSE ;
            }
        }
        break ;

        case WM_DRAWITEM:
        {
            LPFT    lpft ;
            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam ;
            HDC     hDC ;

            if (!(lpft = (LPFT)lpdis->itemData))
            {
                DP1( hWDB, "lbOwnerDraw(): itemData bad" ) ;
                return (LRESULT)-1 ;
            }
            
            hDC = lpdis->hDC ;

            if (lpdis->itemState & ODS_SELECTED)
            {
                SetTextColor( hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ; 
                SetBkColor( hDC, GetSysColor( COLOR_HIGHLIGHT ) ) ;
            }
            else
            {
                SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
                SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
            }
            
            if ((lpdis->itemAction & ODA_FOCUS) &&
               !(lpdis->itemState  & ODS_FOCUS))
                 DrawFocusRect( hDC, &lpdis->rcItem ) ;
            
            ExtTextOut( hDC, lpdis->rcItem.left+2, 
                             lpdis->rcItem.top, ETO_OPAQUE, &lpdis->rcItem, 
                             lpft->szFileType, lstrlen(lpft->szFileType), NULL) ;

            /* if we are gaining focus draw the focus rect after drawing
             * the text.
             */
            if ((lpdis->itemAction & ODA_FOCUS) &&
               (lpdis->itemState & ODS_FOCUS))
                 DrawFocusRect( hDC, &lpdis->rcItem ) ;
        
        } 
        break ;

        case WM_MEASUREITEM:
        {
            LPMEASUREITEMSTRUCT lpMIS = (LPMEASUREITEMSTRUCT)lParam ;
            HDC hdc ;
            TEXTMETRIC tm ;
            HFONT   hfont = ReallyCreateFontEx( NULL, "MS Sans Serif", "Regular", 8, 0 ) ;

            hdc = GetDC( hDlg ) ;
            hfont = SelectObject( hdc, hfont ) ;
            GetTextMetrics( hdc, &tm ) ;
            hfont = SelectObject( hdc, hfont ) ;
            DeleteObject( hfont ) ;
            
            ReleaseDC( hDlg, hdc ) ;
            lpMIS->itemHeight = tm.tmHeight+tm.tmExternalLeading ;
        }
        break ;

        case WM_COMPAREITEM:
        {
            LPCOMPAREITEMSTRUCT lpcis = (LPCOMPAREITEMSTRUCT)lParam ; 
            
#if 0
            if (0==lstrcmpi( ((LPFT)(lpcis->itemData1))->szFileType, GRCS(IDS_NONE) ))
                return (LRESULT)-1 ;

            if (0==lstrcmpi( ((LPFT)(lpcis->itemData2))->szFileType, GRCS(IDS_NONE) ))
                return (LRESULT)1 ;
#endif

            return (LRESULT)lstrcmpi( ((LPFT)(lpcis->itemData1))->szFileType, 
                                      ((LPFT)(lpcis->itemData2))->szFileType ) ;
        }
        break ;

#if 0 // for listboxes only
        case WM_CHARTOITEM:
        {
            char    c = (char)(WORD)wParam  ;
            UINT    wIndex = HIWORD( lParam ) ;
            HWND    hwndLB = (HWND)LOWORD( lParam ) ;
            UINT    w ;
            UINT    n ;
            LPFT    lpft ;
            
            switch(c)
            {
                case VK_RETURN:
                    /* Same as a double click
                     */
                    PostMessage( hDlg, WM_COMMAND, (WPARAM)IDD_FILETYPES,
                                 MAKELPARAM( hwndLB, LBN_DBLCLK ) ) ;
                    
                break ;
            
                default:
                DP( hWDB, "c = %c, wIndex = %d, hwndLB = %04X", c, wIndex, hwndLB ) ;
                if (ComboBox_GetTextLength( hwndLB ) == 1)
                {
                    n = ComboBox_GetCount( hwndLB ) ;
                    for (w = wIndex + 1 ; w < n ; w++ )
                    {
                        ComboBox_GetLBText( hwndLB, w, &lpft )  ;
                        if (toupper(*lpft->szFileType) == toupper(c))
                           return MAKELPARAM( w, 0 ) ;
                    }

                    for (w = 0 ; w < wIndex ; w++ )
                    {
                        ComboBox_GetLBText( hwndLB, w, &lpft )  ;
                        if (toupper(*lpft->szFileType) == toupper(c))
                           return MAKELPARAM( w, 0 ) ;
                    }
                }
                else
                    return (LRESULT)-2 ;

            }
        }
        return (LRESULT)-1 ;
#endif
        
        case WM_DELETEITEM:
            if (wParam == IDD_FILETYPES)
            {
                LPDELETEITEMSTRUCT lpdis = (LPDELETEITEMSTRUCT)lParam; 

                if (lpdis->itemData)
                    GlobalFreePtr( (LPSTR)lpdis->itemData ) ;
            }
        break ;

        default:
            return FALSE ;
   }
   return TRUE ;
}


BOOL WINAPI engAssociate( HWND hwnd, LPSTR lpszExt, LPSTR lpszIdent )
{
    ASS ass ;

    lstrcpy( ass.szExt, lpszExt ) ;
    ass.szIdent[0] = '\0' ;

    if (IDOK == DialogBoxParam( ghmodDLL, MAKEINTRESOURCE( DLG_ASSOCIATE ), 
                            hwnd, (DLGPROC)fnAssociate, (LPARAM)(LPSTR)&ass ))
    {
        lstrcpy( lpszExt, ass.szExt ) ;
        if (lpszIdent)
            lstrcpy( lpszIdent, ass.szIdent ) ;

        return TRUE ;    
    }

    return FALSE ;
}

VOID INTERNAL EnablePrintDDE( HWND hDlg, BOOL f ) ;
VOID INTERNAL EnableOpenDDE( HWND hDlg, BOOL f ) ;


LRESULT CALLBACK fnFileType( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPFT    lpFT ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            HCURSOR hcur ;
            HWND    hwnd ;
    
            hcur = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ;

            SetWindowLong( hDlg, DWL_USER, lParam ) ;
            lpFT = (LPFT)lParam ;
            DASSERT( hWDB, lpFT ) ;

            /* Ident.  If '\0' then it should be editable, otherwise not.
             */
            if (lpFT->szIdent[0] == '\0' || lpFT->szFileType[0] == '\0')
            {
                DestroyWindow( GetDlgItem( hDlg, IDD_IDENTIFIER_STATIC ) ) ;
                hwnd = GetDlgItem( hDlg, IDD_IDENTIFIER ) ;
                Edit_LimitText( hwnd, 63 ) ;
                Edit_SetText( hwnd, lpFT->szIdent ) ;

                if (lpFT->szFileType[0] != '\0')
                    SetWindowText( hDlg, GRCS( IDS_COPYFILETYPE ) ) ;
            }
            else
            {
                DestroyWindow( GetDlgItem( hDlg, IDD_IDENTIFIER ) ) ;
                SetDlgItemText( hDlg, IDD_IDENTIFIER_STATIC, lpFT->szIdent ) ;
                SetWindowText( hDlg, GRCS( IDS_MODIFYFILETYPE ) ) ;
            }

            /* File Type
             */
            hwnd = GetDlgItem( hDlg, IDD_FILETYPE ) ;
            Edit_LimitText( hwnd, MAX_CONFIGNAMELEN-1 ) ;
            Edit_SetText( hwnd, lpFT->szFileType ) ;

            /* Use WinPrint ? 
             */
            CheckDlgButton( hDlg, IDD_USE_WINPRINT, lpFT->fUseWinPrint ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_PRINT_COMMAND ), !lpFT->fUseWinPrint ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_PRINT_COMMAND_LBL ), !lpFT->fUseWinPrint ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_PRINT_USESDDE ), !lpFT->fUseWinPrint ) ;

            /* Print action
             */
            CheckDlgButton( hDlg, IDD_PRINT_USESDDE, lpFT->actPrint.fUseDDE ) ;
            EnablePrintDDE( hDlg, lpFT->actPrint.fUseDDE && !lpFT->fUseWinPrint ) ;

            hwnd = GetDlgItem( hDlg, IDD_PRINT_COMMAND ) ;
            Edit_LimitText( hwnd, MAX_COMMANDLEN-1 ) ;
            Edit_SetText( hwnd, lpFT->actPrint.szCommand ) ;

            hwnd = GetDlgItem( hDlg, IDD_PRINT_DDEMESSAGE ) ;
            Edit_LimitText( hwnd, MAX_COMMANDLEN-1 ) ;
            Edit_SetText( hwnd, lpFT->actPrint.szDDEMsg ) ;

            hwnd = GetDlgItem( hDlg, IDD_PRINT_APPLICATION ) ;
            Edit_LimitText( hwnd, MAX_DDETOPIC-1 ) ;
            Edit_SetText( hwnd, lpFT->actPrint.szDDEApp ) ;

            hwnd = GetDlgItem( hDlg, IDD_PRINT_NOTRUNNING ) ;
            Edit_LimitText( hwnd, MAX_COMMANDLEN-1 ) ;
            Edit_SetText( hwnd, lpFT->actPrint.szDDENotRunning ) ;

            hwnd = GetDlgItem( hDlg, IDD_PRINT_TOPIC ) ;
            Edit_LimitText( hwnd, MAX_DDETOPIC-1 ) ;
            Edit_SetText( hwnd, lpFT->actPrint.szDDETopic ) ;

            /* Open action
             */
            CheckDlgButton( hDlg, IDD_OPEN_USESDDE, lpFT->actOpen.fUseDDE ) ;
            EnableOpenDDE( hDlg, lpFT->actOpen.fUseDDE ) ;

            hwnd = GetDlgItem( hDlg, IDD_OPEN_COMMAND ) ;
            Edit_LimitText( hwnd, MAX_COMMANDLEN-1 ) ;
            Edit_SetText( hwnd, lpFT->actOpen.szCommand ) ;

            hwnd = GetDlgItem( hDlg, IDD_OPEN_DDEMESSAGE ) ;
            Edit_LimitText( hwnd, MAX_COMMANDLEN-1 ) ;
            Edit_SetText( hwnd, lpFT->actOpen.szDDEMsg ) ;

            hwnd = GetDlgItem( hDlg, IDD_OPEN_APPLICATION ) ;
            Edit_LimitText( hwnd, MAX_DDETOPIC-1 ) ;
            Edit_SetText( hwnd, lpFT->actOpen.szDDEApp ) ;

            hwnd = GetDlgItem( hDlg, IDD_OPEN_NOTRUNNING ) ;
            Edit_LimitText( hwnd, MAX_COMMANDLEN-1 ) ;
            Edit_SetText( hwnd, lpFT->actOpen.szDDENotRunning ) ;

            hwnd = GetDlgItem( hDlg, IDD_OPEN_TOPIC ) ;
            Edit_LimitText( hwnd, MAX_DDETOPIC-1 ) ;
            Edit_SetText( hwnd, lpFT->actOpen.szDDETopic ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

            SetCursor( hcur ) ;

            Ctl3dSubclassDlg( hDlg, CTL3D_ALL ) ;

            return TRUE ;
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

            if (!(lpFT = (LPFT)GetWindowLong( hDlg, DWL_USER )))
            {
                DASSERT( hWDB, lpFT ) ;
                EndDialog( hDlg, FALSE ) ;
            }

            switch (wID)
            {
                case IDD_PRINT_USESDDE       :
                {
                    BOOL f = IsDlgButtonChecked( hDlg, wID ) ;

                    lpFT->actPrint.fUseDDE = f ;
                    EnablePrintDDE( hDlg, f && !lpFT->fUseWinPrint ) ;
                }
                break ;

                case IDD_OPEN_USESDDE        :
                {
                    BOOL f = IsDlgButtonChecked( hDlg, wID ) ;

                    lpFT->actOpen.fUseDDE = f ;
                    EnableOpenDDE( hDlg, f ) ;
                }
                break ;

                case IDD_IDENTIFIER:
                    if (lpFT->fUseWinPrint && wNotifyCode == EN_CHANGE)
                    {
                        char szBuf[MAX_COMMANDLEN] ;
                        char szIdent[MAX_CONFIGNAMELEN] ;

                        Edit_GetText( hwndCtl, szIdent, MAX_CONFIGNAMELEN-1 ) ;
                        wsprintf( szBuf, "WINPRINT.EXE -C:\"%s\"", (LPSTR)szIdent ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_COMMAND ), szBuf ) ;
                    }
                break ;

                case IDD_USE_WINPRINT        :
                {
                    lpFT->fUseWinPrint = IsDlgButtonChecked( hDlg, wID ) ;
                    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_COMMAND ), !lpFT->fUseWinPrint ) ;
                    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_COMMAND_LBL ), !lpFT->fUseWinPrint ) ;
                    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_USESDDE ), !lpFT->fUseWinPrint ) ;
                    EnablePrintDDE( hDlg, lpFT->actPrint.fUseDDE && !lpFT->fUseWinPrint ) ;

                    if (lpFT->fUseWinPrint)
                    {
                        char szBuf[MAX_COMMANDLEN] ;
                        /* Put winprint command line in command */
                        wsprintf( szBuf, "WINPRINT.EXE -C:\"%s\"", (LPSTR)lpFT->szIdent ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_COMMAND ), szBuf ) ;
                        CheckDlgButton( hDlg, IDD_PRINT_USESDDE, FALSE ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_DDEMESSAGE ), "" ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_APPLICATION ), "" ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_NOTRUNNING ), "" ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_TOPIC ), "" ) ;
                    }
                    else
                    {
                        CheckDlgButton( hDlg, IDD_PRINT_USESDDE, lpFT->actPrint.fUseDDE ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_COMMAND ), lpFT->actPrint.szCommand ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_DDEMESSAGE ), lpFT->actPrint.szDDEMsg ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_APPLICATION ), lpFT->actPrint.szDDEApp ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_NOTRUNNING ), lpFT->actPrint.szDDENotRunning ) ;
                        Edit_SetText( GetDlgItem( hDlg, IDD_PRINT_TOPIC ), lpFT->actPrint.szDDETopic ) ;
                    }
                }
                break ;

                case IDD_BROWSE              :
                break ;

                case IDOK:
                    if (lpFT->szIdent[0] == '\0' || lpFT->szFileType[0] == '\0')
                        Edit_GetText( GetDlgItem( hDlg, IDD_IDENTIFIER ), 
                                      lpFT->szIdent, MAX_CONFIGNAMELEN ) ;
        
                    Edit_GetText( GetDlgItem( hDlg, IDD_FILETYPE ), 
                                  lpFT->szFileType, MAX_CONFIGNAMELEN-1 ) ;

                    StripOffWhiteSpace( lpFT->szFileType ) ;
                    StripOffWhiteSpace( lpFT->szIdent ) ;

                    if (*lpFT->szFileType == 0 || *lpFT->szIdent == 0)
                        break ;

                    if (lpFT->fUseWinPrint)
                    {
                        Edit_GetText( GetDlgItem( hDlg, IDD_PRINT_COMMAND ),
                                      lpFT->actPrint.szCommand, MAX_COMMANDLEN ) ;
                        StripOffWhiteSpace( lpFT->actPrint.szCommand ) ;
                        lpFT->actPrint.fUseDDE = FALSE ;
                        *lpFT->actPrint.szDDEMsg = '\0' ;
                        *lpFT->actPrint.szDDEApp = '\0' ;
                        *lpFT->actPrint.szDDENotRunning = '\0' ;
                        *lpFT->actPrint.szDDETopic = '\0' ;
                    }
                    else
                    {
                        Edit_GetText( GetDlgItem( hDlg, IDD_PRINT_COMMAND ),
                                      lpFT->actPrint.szCommand, MAX_COMMANDLEN ) ;
                        StripOffWhiteSpace( lpFT->actPrint.szCommand ) ;
                        if (lpFT->actPrint.fUseDDE)
                        {
                            Edit_GetText( GetDlgItem( hDlg, IDD_PRINT_DDEMESSAGE ), 
                                          lpFT->actPrint.szDDEMsg, MAX_COMMANDLEN ) ;
                            Edit_GetText( GetDlgItem( hDlg, IDD_PRINT_APPLICATION ), 
                                          lpFT->actPrint.szDDEApp, MAX_DDETOPIC ) ;
                            Edit_GetText( GetDlgItem( hDlg, IDD_PRINT_NOTRUNNING ), 
                                          lpFT->actPrint.szDDENotRunning, MAX_COMMANDLEN ) ;
                            Edit_GetText( GetDlgItem( hDlg, IDD_PRINT_TOPIC ), 
                                          lpFT->actPrint.szDDETopic, MAX_DDETOPIC ) ;

                            StripOffWhiteSpace( lpFT->actPrint.szDDEMsg ) ;
                            StripOffWhiteSpace( lpFT->actPrint.szDDEApp ) ;
                            StripOffWhiteSpace( lpFT->actPrint.szDDENotRunning ) ;
                            StripOffWhiteSpace( lpFT->actPrint.szDDETopic ) ;
                        }
                        else
                        {
                            *lpFT->actPrint.szDDEMsg = '\0' ;
                            *lpFT->actPrint.szDDEApp = '\0' ;
                            *lpFT->actPrint.szDDENotRunning = '\0' ;
                            *lpFT->actPrint.szDDETopic = '\0' ;
                        }
                    }

                    Edit_GetText( GetDlgItem( hDlg, IDD_OPEN_COMMAND ),
                                  lpFT->actOpen.szCommand, MAX_COMMANDLEN ) ;
                    StripOffWhiteSpace( lpFT->actOpen.szCommand ) ;
                    if (lpFT->actOpen.fUseDDE)
                    {
                        Edit_GetText( GetDlgItem( hDlg, IDD_OPEN_DDEMESSAGE ), 
                                      lpFT->actOpen.szDDEMsg, MAX_COMMANDLEN ) ;
                        Edit_GetText( GetDlgItem( hDlg, IDD_OPEN_APPLICATION ), 
                                      lpFT->actOpen.szDDEApp, MAX_DDETOPIC ) ;
                        Edit_GetText( GetDlgItem( hDlg, IDD_OPEN_NOTRUNNING ), 
                                      lpFT->actOpen.szDDENotRunning, MAX_COMMANDLEN ) ;
                        Edit_GetText( GetDlgItem( hDlg, IDD_OPEN_TOPIC ), 
                                      lpFT->actOpen.szDDETopic, MAX_DDETOPIC ) ;
                        StripOffWhiteSpace( lpFT->actOpen.szDDEMsg ) ;
                        StripOffWhiteSpace( lpFT->actOpen.szDDEApp ) ;
                        StripOffWhiteSpace( lpFT->actOpen.szDDENotRunning ) ;
                        StripOffWhiteSpace( lpFT->actOpen.szDDETopic ) ;
                    }
                    else
                    {
                        *lpFT->actOpen.szDDEMsg = '\0' ;
                        *lpFT->actOpen.szDDEApp = '\0' ;
                        *lpFT->actOpen.szDDENotRunning = '\0' ;
                        *lpFT->actOpen.szDDETopic = '\0' ;
                    }

                    // Fall through!

                case IDCANCEL:
                    EndDialog (hDlg, wID) ;
                break ;

                default :
                   return FALSE ;
            }
        }
        break ;

        default:
            return FALSE ;
   }
   return TRUE ;
}

VOID INTERNAL EnableOpenDDE( HWND hDlg, BOOL f ) 
{
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_DDEMESSAGE ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_APPLICATION ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_NOTRUNNING ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_TOPIC ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_DDEMESSAGE_LBL ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_APPLICATION_LBL ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_NOTRUNNING_LBL ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_OPEN_TOPIC_LBL ), f ) ;
}

VOID INTERNAL EnablePrintDDE( HWND hDlg, BOOL f ) 
{
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_DDEMESSAGE ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_APPLICATION ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_NOTRUNNING ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_TOPIC ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_DDEMESSAGE_LBL ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_APPLICATION_LBL ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_NOTRUNNING_LBL ), f ) ;
    EnableWindow( GetDlgItem( hDlg, IDD_PRINT_TOPIC_LBL ), f ) ;
}

BOOL WINAPI engFileTypeCopy( HWND hwnd, LPSTR lpszIdentOld, LPSTR  lpszIdentNew )
{
    LPFT  lpft ;
    
    if (!(lpft = (LPFT)GlobalAllocPtr( GHND, sizeof( FT ) )))
    {
        DP1( hWDB, "GALLOC failed engFileType" ) ;
        return FALSE ;
    }

    lstrcpy( lpft->szIdent, lpszIdentOld ) ;
    ReadIdent( hwnd, HKEY_CLASSES_ROOT, lpft ) ;
    lpft->szIdent[0] = '\0' ;

    if (IDOK == DialogBoxParam( ghmodDLL, MAKEINTRESOURCE( DLG_FILETYPE ), 
                            hwnd, (DLGPROC)fnFileType, (LPARAM)(LPSTR)lpft ))
    {
        lstrcpy( lpszIdentNew, lpft->szIdent ) ;

        WriteIdent( hwnd, HKEY_CLASSES_ROOT, lpft ) ;

        GlobalFreePtr( lpft ) ;
        return TRUE ;    
    }

    GlobalFreePtr( lpft ) ;
    return FALSE ;
}


BOOL WINAPI engFileTypeModify( HWND hwnd, LPSTR lpszIdent )
{
    LPFT lpft ;

    if (!(lpft = (LPFT)GlobalAllocPtr( GHND, sizeof( FT ) )))
    {
        DP1( hWDB, "GALLOC failed engFileType" ) ;
        return FALSE ;
    }

    lstrcpy( lpft->szIdent, lpszIdent ) ;
    ReadIdent( hwnd, HKEY_CLASSES_ROOT, lpft ) ;

    if (IDOK == DialogBoxParam( ghmodDLL, MAKEINTRESOURCE( DLG_FILETYPE ), 
                            hwnd, (DLGPROC)fnFileType, (LPARAM)(LPSTR)lpft ))
    {
        WriteIdent( hwnd, HKEY_CLASSES_ROOT, lpft ) ;

        GlobalFreePtr( lpft ) ;
        return TRUE ;    
    }

    GlobalFreePtr( lpft ) ;
    return FALSE ;
}


BOOL INTERNAL ReadAssociations( HWND hDlg )
{
    HKEY    hkRoot ;
    HWND    hwndCB = GetDlgItem( hDlg, IDD_EXTENSION ) ;
    HWND    hwndLB = GetDlgItem( hDlg, IDD_FILETYPES ) ;
    
    if (RegOpenKey( HKEY_CLASSES_ROOT, NULL, &hkRoot ) == ERROR_SUCCESS) 
    {
        char    szRoot[MAX_CONFIGNAMELEN] ;
        DWORD   dwIndex ;
        LPFT    lpft ;

        SetWindowRedraw( hwndLB, FALSE ) ;

        for (dwIndex = 0; 
             RegEnumKey( hkRoot, dwIndex, szRoot, sizeof(szRoot)) == ERROR_SUCCESS; 
             ++dwIndex) 
        {
            if (*szRoot == '.')
            {
//                AnsiUpper( szRoot+1 ) ;
//                if (CB_ERR == ComboBox_FindString( hwndCB, 0, szRoot+1 ) )
//                    ComboBox_AddString( hwndCB, szRoot+1 ) ;
            }
            else
            {
                if (lpft = GlobalAllocPtr( GHND, sizeof(FT) ))
                {
                    lstrcpy( lpft->szIdent, szRoot ) ;
                    ReadIdent( hDlg, hkRoot, lpft ) ;
                    ComboBox_AddString( hwndLB, (LPSTR)lpft )  ;
                }
                else
                {
                    DP1( hWDB, "Galloc failed reading registry!" ) ;
                }
            }
        }

        if (lpft = GlobalAllocPtr( GHND, sizeof(FT) ))
        {
            lstrcpy( lpft->szIdent, GRCS(IDS_NONE) ) ;
            lstrcpy( lpft->szFileType, GRCS(IDS_NONE) ) ;
            ComboBox_AddString( hwndLB, (LPSTR)lpft )  ;
        }
        else
        {
            DP1( hWDB, "Galloc failed reading registry!" ) ;
        }

        RegCloseKey( hkRoot ) ;

        SetWindowRedraw( hwndLB, TRUE ) ;
        InvalidateRect( hwndLB, NULL, TRUE ) ;
        UpdateWindow( hwndLB ) ;

    }
    else
    {
        DP1( hWDB, "Registry corrupt?" ) ;
        return TRUE ;
    }

}

BOOL INTERNAL GetIdentFromExt( HKEY hkRoot, LPSTR lpszExt, LPSTR lpszIdent ) 
{
    DWORD cb ;
    char szExt[5] ;

    if (*lpszExt != '.')
    {
        *szExt = '.' ;
        lstrcpy( szExt+1, lpszExt ) ;
    }    
    else
        lstrcpy( szExt, lpszExt ) ;

    cb = MAX_CONFIGNAMELEN ;
    if (RegQueryValue( hkRoot, szExt, lpszIdent, &cb ) == ERROR_SUCCESS)
        return TRUE ;

    return FALSE ;
}


BOOL INTERNAL GetFileTypeFromExt( HKEY hkRoot, LPSTR lpszExt, LPSTR lpszFileType ) 
{
    char    szIdent[MAX_CONFIGNAMELEN] ;
    DWORD   cb ;
    HKEY    hkExt, hk, hkShell ;
    ACTION  actOpen ;

    if (GetIdentFromExt( hkRoot, lpszExt, szIdent ))
    {
        char szExt[5] ;
    
        cb = MAX_CONFIGNAMELEN ;
        if ((RegQueryValue( hkRoot, szIdent, lpszFileType, &cb ) == ERROR_SUCCESS) &&
            *lpszFileType != '\0')
            return TRUE ;

        /* It might be a conversion from WIN.INI, thus it won't have an Ident */
        DP3( hWDB, "%s has no value, checking 'open' sub-tree.", (LPSTR)lpszExt ) ;
        if (*lpszExt != '.')
        {
            *szExt = '.' ;
            lstrcpy( szExt+1, lpszExt ) ;
        }    
        else
            lstrcpy( szExt, lpszExt ) ;

        if (RegOpenKey( hkRoot, szExt, &hkExt ) != ERROR_SUCCESS) 
        {
            DP3( hWDB, "    Could not open '%s'", (LPSTR)szExt ) ;
            return FALSE ;
        }

        if (RegOpenKey( hkExt, "shell", &hkShell ) != ERROR_SUCCESS) 
        {
            DP3( hWDB, "    Could not open shell" ) ;
            RegCloseKey( hkExt ) ;
            return FALSE ;
        }
            
        if (RegOpenKey( hkShell, "open", &hk ) != ERROR_SUCCESS) 
        {
            DP3( hWDB, "    Could not open open" ) ;
            RegCloseKey( hkShell ) ;
            RegCloseKey( hkExt ) ;
            return FALSE ;
        }

        ReadAction( NULL, hk, &actOpen ) ;

        lstrcpy( lpszFileType, actOpen.szCommand ) ;

        RegCloseKey( hk ) ;
        RegCloseKey( hkShell ) ;
        RegCloseKey( hkExt ) ;
    }
    else
    {
        /* Go to [Extensions] section of WIN.INI and check there.
         */
        DP3( hWDB, "%s not found in registry, trying WIN.INI", (LPSTR)lpszExt ) ;
        GetProfileString( GRCS(IDS_EXTENSIONS), lpszExt, "", 
                          lpszFileType, MAX_CONFIGNAMELEN ) ;
    }

    if (lstrlen( lpszFileType ))
    {
        LPSTR lp ;
    
        lp = _fstrrchr( lpszFileType, '^' ) ;
        if (!lp)
            lp = _fstrrchr( lpszFileType, '%' ) ;

        if (lp)
        {
            *(lp-1) = '\0' ;
            return TRUE ;
        }
    }

    return FALSE ;
}

BOOL INTERNAL ReadIdent( HWND hwnd, HKEY hkRoot, LPFT lpft )
{
    char    szPath[MAX_COMMANDLEN] ;
    LPSTR   lp ;
    DWORD   cb ;
    HKEY    hkShell ;
    HKEY    hkIdent ;
    HKEY    hk ;

    lpft->szFileType[0] = '\0' ;
    lpft->fUseWinPrint = FALSE ;
    _fmemset( &lpft->actPrint, '\0', sizeof( ACTION ) ) ;
    _fmemset( &lpft->actOpen, '\0', sizeof( ACTION ) ) ;

    /* On input, lplpft->szIdent contains the name to read */
    if (lpft->szIdent[0] == '\0')
        return FALSE ;

    cb = MAX_CONFIGNAMELEN ;
    if (RegQueryValue( hkRoot, lpft->szIdent, lpft->szFileType, &cb ) == ERROR_SUCCESS)
    {
        
        if (RegOpenKey( hkRoot, lpft->szIdent, &hkIdent ) != ERROR_SUCCESS) 
            return FALSE ;

        /* Get the 'shell' sub key.  This contains our two actions, 'print' and
         * 'open'
         */
        if (RegOpenKey( hkIdent, "shell", &hkShell ) == ERROR_SUCCESS) 
        {
            if (RegOpenKey( hkShell, "print", &hk ) == ERROR_SUCCESS) 
            {
                ReadAction( hwnd, hk, &lpft->actPrint ) ;

                /* See if Print action is WinPrint
                 */
                lstrcpy( szPath, lpft->actPrint.szCommand ) ;
                StripPath( szPath ) ;
                if (lp = _fstrchr( szPath, '.' ))
                    *lp = '\0' ;

                if (lstrcmpi( szPath, "winprint" ) == 0)
                {
                    DP5( hWDB, "Use WinPrint!" ) ;
                    lpft->fUseWinPrint = TRUE ;
                }
                else
                    lpft->fUseWinPrint = FALSE ;

                RegCloseKey( hk ) ;
            }

            if (RegOpenKey( hkShell, "open", &hk ) == ERROR_SUCCESS) 
            {
                ReadAction( hwnd, hk, &lpft->actOpen ) ;
                RegCloseKey( hk ) ;
            }

            RegCloseKey( hkShell ) ;

        }
        else
        {
            DP5( hWDB, "shell\\ does not exist" ) ;
        }

        RegCloseKey( hkIdent ) ;
    }
    else
    {
        DP1( hWDB, "Key name (%s) not in registry!", (LPSTR)lpft->szIdent ) ;
        return FALSE ;
    }
    
    return TRUE ;
}

BOOL INTERNAL ReadAction( HWND hwnd, HKEY hk, LPACTION lpAct )
{
    DWORD   cb ;
    HKEY    hkDDE ;

    /* Get Command
     */
    cb = MAX_COMMANDLEN ;
    RegQueryValue( hk, "command", lpAct->szCommand, &cb ) ;

    /* Get DDE shit
     */
    if (RegOpenKey( hk, "ddeexec", &hkDDE ) == ERROR_SUCCESS) 
    {
        cb = MAX_COMMANDLEN ;
        RegQueryValue( hk, "ddeexec", lpAct->szDDEMsg, &cb ) ;
        cb = MAX_COMMANDLEN ;
        RegQueryValue( hkDDE, "ifexec", lpAct->szDDENotRunning, &cb ) ;

        cb = MAX_DDETOPIC ;
        RegQueryValue( hkDDE, "application", lpAct->szDDEApp, &cb ) ;
        if (cb == 0 || lpAct->szDDEApp[0] == '\0')
        {
            char    szBuf[MAX_COMMANDLEN] ;
            LPSTR    p ;

            lstrcpy( szBuf, lpAct->szCommand ) ;
            StripPath( szBuf ) ;
            if (p = _fstrchr( szBuf, '.' ))
                *p = '\0' ;
            szBuf[0] = (char)(DWORD)AnsiUpper( MAKELP( 0, szBuf[0] ) ) ;
            lstrcpy( lpAct->szDDEApp, szBuf ) ;
        }

        cb = MAX_DDETOPIC ;
        RegQueryValue( hkDDE, "topic", lpAct->szDDETopic, &cb ) ;
        if (cb == 0 || lpAct->szDDETopic[0] == '\0')
            lstrcpy( lpAct->szDDETopic, "System" ) ;

        lpAct->fUseDDE = TRUE ;
        RegCloseKey( hkDDE ) ;
    }
    else
        lpAct->fUseDDE = FALSE ;
    
    return TRUE ;
}


BOOL INTERNAL WriteAction( HWND hwnd, HKEY hk, LPACTION lpAct ) ;
BOOL INTERNAL WriteIdent( HWND hwnd, HKEY hkRoot, LPFT lpft )
{
    HKEY    hkShell ;
    HKEY    hkIdent ;
    HKEY    hk ;

    if (RegOpenKey( hkRoot, lpft->szIdent, &hkIdent ) != ERROR_SUCCESS)
        RegCreateKey( hkRoot, lpft->szIdent, &hkIdent ) ;

    if (hkIdent == NULL)
    {
        DP1( hWDB, "Could not create key: %s", (LPSTR)lpft->szIdent ) ;
        return FALSE ;
    }

    RegSetValue( hkIdent, NULL, REG_SZ, lpft->szFileType, lstrlen( lpft->szFileType ) ) ;

    if ((RegOpenKey( hkIdent, "shell", &hkShell ) == ERROR_SUCCESS) ||
        (RegCreateKey( hkIdent, "shell", &hkShell ) == ERROR_SUCCESS))
    {
        if ((RegOpenKey( hkShell, "print", &hk ) == ERROR_SUCCESS) || 
            (RegCreateKey( hkShell, "print", &hk ) == ERROR_SUCCESS))
        {
            WriteAction( hwnd, hk, &lpft->actPrint ) ;
            RegCloseKey( hk ) ;
        }
    
        if ((RegOpenKey( hkShell, "open", &hk ) == ERROR_SUCCESS) || 
            (RegCreateKey( hkShell, "open", &hk ) == ERROR_SUCCESS))
        {
            WriteAction( hwnd, hk, &lpft->actOpen ) ;
            RegCloseKey( hk ) ;
        }
    
        RegCloseKey( hkShell ) ;
    }
    else
    {
        DP1( hWDB, "Could not create shell key" ) ;
        RegCloseKey( hkIdent ) ;
        return FALSE ;
    }

    RegCloseKey( hkIdent ) ;

    return TRUE ;
}

BOOL INTERNAL WriteIdent_ExtOnly( HWND hwnd, HKEY hkRoot, LPFT lpft )
{
    HKEY    hkShell ;
    HKEY    hkIdent ;
    HKEY    hk ;

    if (RegOpenKey( hkRoot, lpft->szIdent, &hkIdent ) != ERROR_SUCCESS)
        RegCreateKey( hkRoot, lpft->szIdent, &hkIdent ) ;

    if (hkIdent == NULL)
    {
        DP1( hWDB, "Could not create key: %s", (LPSTR)lpft->szIdent ) ;
        return FALSE ;
    }

    /* Nuke the value.
     */
    RegSetValue( hkIdent, NULL, REG_SZ, "", 0 ) ;

    if ((RegOpenKey( hkIdent, "shell", &hkShell ) == ERROR_SUCCESS) ||
        (RegCreateKey( hkIdent, "shell", &hkShell ) == ERROR_SUCCESS))
    {
        if ((RegOpenKey( hkShell, "open", &hk ) == ERROR_SUCCESS) || 
            (RegCreateKey( hkShell, "open", &hk ) == ERROR_SUCCESS))
        {
            DP3( hWDB, "Creating %s\\shell\\open\\command = %s", (LPSTR)lpft->szIdent, (LPSTR)lpft->actOpen.szCommand ) ;
            if (*lpft->actOpen.szCommand)
                RegSetValue( hk, "command", REG_SZ, lpft->actOpen.szCommand, lstrlen( lpft->actOpen.szCommand ) ) ;
            else
                RegDeleteKey( hk, "command" ) ;

            RegCloseKey( hk ) ;
        }
    
        RegCloseKey( hkShell ) ;
    }
    else
    {
        DP1( hWDB, "Could not create shell key" ) ;
        RegCloseKey( hkIdent ) ;
        return FALSE ;
    }

    RegCloseKey( hkIdent ) ;

    return TRUE ;
}

BOOL INTERNAL WriteAction( HWND hwnd, HKEY hk, LPACTION lpAct )
{
    HKEY    hkDDE = NULL ;

    /* Command
     */
    if (*lpAct->szCommand)
        RegSetValue( hk, "command", REG_SZ, lpAct->szCommand, lstrlen( lpAct->szCommand ) ) ;
    else
        RegDeleteKey( hk, "command" ) ;

    /* DDE shit
     */
    if (lpAct->fUseDDE) 
    {
        if (RegOpenKey( hk, "ddeexec", &hkDDE ) != ERROR_SUCCESS)
            RegCreateKey( hk, "ddeexec", &hkDDE ) ;

        if (hkDDE == NULL)
            return FALSE ;

        if (*lpAct->szDDEMsg)
            RegSetValue( hkDDE, NULL, REG_SZ, lpAct->szDDEMsg, lstrlen( lpAct->szDDEMsg ) ) ;

        if (*lpAct->szDDENotRunning)
            RegSetValue( hkDDE, "ifexec", REG_SZ, lpAct->szDDENotRunning, lstrlen( lpAct->szDDENotRunning ) ) ;
        else
            RegDeleteKey( hkDDE, "ifexec" ) ;

        if (*lpAct->szDDEApp)
            RegSetValue( hkDDE, "application", REG_SZ, lpAct->szDDEApp, lstrlen( lpAct->szDDEApp ) ) ;
        else
            RegDeleteKey( hkDDE, "application" ) ;

        if (*lpAct->szDDETopic)
            RegSetValue( hkDDE, "topic", REG_SZ, lpAct->szDDETopic, lstrlen( lpAct->szDDETopic ) ) ;
        else
            RegDeleteKey( hkDDE, "topic" ) ;
            
        RegCloseKey( hkDDE ) ;
    }
    else
    {
        RegDeleteKey( hk, "ddeexec" ) ;
    }
    
    return TRUE ;
}

/************************************************************************
 * End of File: ASSOCIAT.C
 ***********************************************************************/

