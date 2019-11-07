/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  BITMAP 
 *
 *      Module:  BITMAP.C
 * 
 *      This is the bitmap/dib FTD module.  
 *
 *   Revisions:  
 *       12/22/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "ENGINE.h"

#include "FTD.H"

/* Most FTDs need to store data about their state and configuration.  The
 * following structure is internal to each ftd.  The ENGINECONFIG structure
 * has a member, FTDInfo.lCustData, that points to this structure.
 */
typedef struct tagBITMAPCFG
{
    /* Stuff that is saved as configuration info is at the top.
     */
    BOOL    fDraftQuality ;

    BOOL    fPrinterRes ;

    UINT    uiScaleFlags ;

    UINT    uiScaleX ;
    UINT    uiScaleY ;


} BITMAPCFG, * PBITMAPCFG, FAR * LPBITMAPCFG ;

#define SCALE_XANDY             0x0000
#define SCALE_BESTFIT           0x0001
#define SCALE_STRETCHTOPAGE     0x0002

LRESULT CALLBACK fnFTD_Bitmap( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 ) ;
BOOL CALLBACK fnFTD_BitmapDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

/* Bitmap/Dib FTD proc
 *
 */
LRESULT CALLBACK fnFTD_Bitmap( UINT uiCmd, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3 )
{
    LPBITMAPCFG     lpFTD ;
//    LPENGINECONFIG  lpEC ;
//    LPENGINEJOB     lpJob ;

    switch (uiCmd)
    {
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
                DP1( hWDB, "Bitmap: uiStructSize == 0, allocating..." ) ;
                lpFTDInfo->lpData = (LPARAM)GlobalAllocPtr( GHND, sizeof( BITMAPCFG ) ) ;
                lpFTDInfo->uiStructSize = sizeof( BITMAPCFG ) ;
            }
            else
            {
                /* Make sure it's the right size
                 */
                if (lpFTDInfo->uiStructSize != sizeof( BITMAPCFG ))
                {
                    DP1( hWDB, "Bitmap: uiStructSize != BITMAPCFG size" ) ;
                    return 0L ;
                }
            }

            if (lpFTD = (LPBITMAPCFG)lpFTDInfo->lpData)
            {
                char szBuf[128] ;
                LPSTR lpsz ;
                /* Read like mad!
                 */    
                if (GetPrivateProfileString( (LPSTR)lParam2, GRCS(IDS_KEY_BITMAPOPTS),
                                           "", szBuf, 128, (LPSTR)lParam3 ))
                {
                    lpsz = szBuf ;
                    DP3( hWDB, "Bitmap: Got opts-> '%s'", (LPSTR)lpsz ) ;
    
                    lpFTD->fDraftQuality = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->fPrinterRes = (BOOL)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->uiScaleFlags = (BOOL)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->uiScaleX = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                    lpFTD->uiScaleY = (UINT)ParseOffNumber( (LPSTR FAR *)&lpsz, NULL ) ;
                }
                else
                    fnFTD_Bitmap( FTD_INITCONFIG, (LPARAM)lpFTDInfo, 0L, 0L ) ;
            }
            else
            {
                DP1( hWDB, "Bitmap: lpData bad!" ) ;
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

            if (lpFTDInfo->uiStructSize != sizeof( BITMAPCFG ))
            {
                DP1( hWDB, "Bitmap: uiStructSize bad!" ) ;
                return 0L ;
            }

            if (lpFTD = (LPBITMAPCFG)lpFTDInfo->lpData)
            {
                char szBuf[128] ;

                /* Write like mad!
                 */    
                wsprintf( szBuf, "%d,%d,%d,%d,%d", lpFTD->fDraftQuality,
                                                      lpFTD->fPrinterRes,
                                                      lpFTD->uiScaleFlags,
                                                      lpFTD->uiScaleX,
                                                      lpFTD->uiScaleY ) ;
                WritePrivateProfileString( (LPSTR)lParam2, GRCS(IDS_KEY_BITMAPOPTS),
                                           szBuf, (LPSTR)lParam3 ) ;
            }
            else
            {
                DP1( hWDB, "Bitmap: lpData bad!" ) ;
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
            return (LRESULT)GetDlgBoxRes( ghmodDLL, (LPSTR)MAKEINTRESOURCE( DLG_FTD_BITMAP ) ) ;
        break ;

        case FTD_CONFIGDLGMSG:
        {
            /* lParam1 -> LPMSG
             * lParam2 -> NULL
             * lParam3 -> NULL
             */
            LPMSG   lpMsg = (LPMSG)lParam1 ;

            return (LRESULT)fnFTD_BitmapDlg( lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam ) ;
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

            DP1( hWDB, "Bitmap: uiStructSize == 0, allocating..." ) ;
            lpFTDInfo->lpData = (LPARAM)GlobalAllocPtr( GHND, sizeof( BITMAPCFG ) ) ;
            lpFTDInfo->uiStructSize = sizeof( BITMAPCFG ) ;

            if (lpFTD = (LPBITMAPCFG)lpFTDInfo->lpData)
            {
                lpFTD->fDraftQuality = FALSE ;
                lpFTD->fPrinterRes = FALSE ;
                lpFTD->uiScaleFlags = SCALE_BESTFIT ;
                lpFTD->uiScaleX = 100 ;
                lpFTD->uiScaleY = 100 ;
            }
        }
        return MAKELRESULT( TRUE, 0 ) ;

        default:
            /* Call the default FTD proc (in ENGINE.DLL) for any
             * commands that are not specifically processed.
             */
            return ftdDefaultProc( uiCmd, lParam1, lParam2, lParam3 ) ;
        break ;
    }
    
    return 0L ;
}

BOOL CALLBACK fnFTD_BitmapDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPENGINECONFIG  lpEC ;
    LPBITMAPCFG     lpFTD ;

    switch( uiMsg )
    {
        case WM_INITDIALOG:
        {
            /* lParam points to a LPENGINECONFIG structure
             */
            lpEC = (LPENGINECONFIG)lParam  ;

            if (lpEC && lpEC->FTDInfo.lpData)
            {
                lpFTD = (LPBITMAPCFG)lpEC->FTDInfo.lpData ;

                switch( lpFTD->uiScaleFlags )
                {
                    case SCALE_STRETCHTOPAGE:
                        CheckRadioButton( hDlg, IDD_BESTFIT, IDD_SCALEXANDY, IDD_STRETCHTOPAGE ) ;
                    break ;

                    case SCALE_XANDY:
                        CheckRadioButton( hDlg, IDD_BESTFIT, IDD_SCALEXANDY, IDD_SCALEXANDY ) ;
                    break ;

                    case SCALE_BESTFIT:
                    default:
                        CheckRadioButton( hDlg, IDD_BESTFIT, IDD_SCALEXANDY, IDD_BESTFIT ) ;
                    break ;
                }

                SetDlgItemInt( hDlg, IDD_XSCALE, lpFTD->uiScaleX, FALSE ) ;
                SetDlgItemInt( hDlg, IDD_YSCALE, lpFTD->uiScaleY, FALSE ) ;

                if (lpFTD->uiScaleFlags != SCALE_XANDY)
                {
                    EnableWindow( GetDlgItem( hDlg, IDD_XSCALE ), FALSE ) ;
                    EnableWindow( GetDlgItem( hDlg, IDD_YSCALE ), FALSE ) ;
                }

                CheckDlgButton( hDlg, IDD_USEPRINTERRES, lpFTD->fPrinterRes ) ;
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

            if (lpEC && !(lpFTD = (LPBITMAPCFG)lpEC->FTDInfo.lpData))
            {
                DP1( hWDB, "FTDInfo.lpData == NULL!!" ) ;
            }

            switch (wID)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog( hDlg, wID ) ;
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



/************************************************************************
 * End of BITMAP.H
 ***********************************************************************/
