/************************************************************************
 *
 *    Copyright (C) 1991 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  dialog.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Dialog box routines
 *
 *   Revisions:  
 *     01.00.001  1/ 7/91 baw   Initial Version, first build
 *
 ************************************************************************/

#include "netbios.h"

/************************************************************************
 *  int GetRadioButton( HWND hDlg, int nIDFirstButton, int nIDLastButton )
 *
 *  Description:
 *     Returns the value of the currently selected radio button.
 *
 *  Comments:
 *      7/ 3/91  baw  Added this comment.
 *
 ************************************************************************/

int GetRadioButton( HWND hDlg, int nIDFirstButton, int nIDLastButton )
{
   int  i ;

   for (i = nIDFirstButton; i <= nIDLastButton; i++) {
      if (IsDlgButtonChecked( hDlg, i ))
         return( i ) ;
   }
   return( 0 ) ;

} /* end of GetRadioButton() */

/************************************************************************
 *  BOOL SettingsDlgInit( HWND hDlg, LOCALHANDLE hCCB )
 *
 *  Description:
 *     Init stuff for dialog box.
 *
 *  Comments:
 *     6/23/91  baw  No comment.
 *
 ************************************************************************/

BOOL SettingsDlgInit( HWND hDlg, LOCALHANDLE hCCB )
{
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   // set up local name

   SetDlgItemText( hDlg, IDD_LOCALNAME, npCCB -> achLocal ) ;
   SendDlgItemMessage( hDlg, IDD_LOCALNAME, EM_LIMITTEXT, MAXLEN_NCBNAMESIZE,
                       NULL ) ;

   // set up remote name and limit length

   SetDlgItemText( hDlg, IDD_REMOTENAME, npCCB -> achServer ) ;
   SendDlgItemMessage( hDlg, IDD_REMOTENAME, EM_LIMITTEXT, MAXLEN_NCBNAMESIZE,
                       NULL ) ;

   // server mode setting

   CheckDlgButton( hDlg, IDD_SERVERMODE, npCCB -> fServerMode ) ;

   // blank padding setting

   CheckDlgButton( hDlg, IDD_BLANKPAD, npCCB -> fBlankPadding ) ;

   // receive timeout setting, limit text length

   SetDlgItemInt( hDlg, IDD_RXTIMEOUT, npCCB -> wRxTimeout, FALSE ) ;
   SendDlgItemMessage( hDlg, IDD_RXTIMEOUT, EM_LIMITTEXT, MAXLEN_TIMEOUT,
                       NULL ) ;

   // transmit timeout setting, limit text length

   SetDlgItemInt( hDlg, IDD_TXTIMEOUT, npCCB -> wTxTimeout, FALSE ) ;
   SendDlgItemMessage( hDlg, IDD_RXTIMEOUT, EM_LIMITTEXT, MAXLEN_TIMEOUT,
                       NULL ) ;

   // set NetBIOS extension

   CheckRadioButton( hDlg, IDD_STANDARD, IDD_UB,
                     npCCB -> wExtension + IDD_STANDARD ) ;

   LocalUnlock( hCCB ) ;
   return( TRUE ) ;

} /* end of SettingsDlgInit() */

/************************************************************************
 *  BOOL SettingsDlgTerm( HWND hDlg, LOCALHANDLE hCCB )
 *
 *  Description:
 *     SettingsDlg termination routine.  Saves information to the CCB
 *     structure.
 *
 *  Comments:
 *      6/24/91  baw  Wrote this comment.
 *
 ************************************************************************/

BOOL SettingsDlgTerm( HWND hDlg, LOCALHANDLE hCCB )
{
   char   szTempName[ MAXLEN_NCBNAMESIZE ] ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   // get remote and local names - may need to remove blanks

   GetDlgItemText( hDlg, IDD_REMOTENAME, szTempName, MAXLEN_NCBNAMESIZE ) ;

   if (NULL == *szTempName)
   {
      MessageBox( hDlg, "You must specify a remote name.", "NetBIOS",
                  MB_ICONEXCLAMATION | MB_OK ) ;
      return ( FALSE ) ;
   }
   lstrcpy( npCCB -> achServer, szTempName ) ;

   GetDlgItemText( hDlg, IDD_LOCALNAME, (LPSTR) npCCB -> achLocal,
                   MAXLEN_NCBNAMESIZE ) ;

   // get server mode setting

   npCCB -> fServerMode = IsDlgButtonChecked( hDlg, IDD_SERVERMODE ) ;

   // get blank padding setting

   npCCB -> fBlankPadding = IsDlgButtonChecked( hDlg, IDD_BLANKPAD ) ;

   // get receive timeout setting

   npCCB -> wRxTimeout = GetDlgItemInt( hDlg, IDD_RXTIMEOUT, NULL, FALSE ) ;

   // get transmit timeout setting

   npCCB -> wTxTimeout = GetDlgItemInt( hDlg, IDD_TXTIMEOUT, NULL, FALSE ) ;

   // get NetBIOS extension

   npCCB -> wExtension = GetRadioButton( hDlg, IDD_STANDARD, IDD_UB ) -
                            IDD_STANDARD ;

   LocalUnlock( hCCB ) ;
   return ( TRUE ) ;

} /* end of SettingsDlgTerm() */

/************************************************************************
 *  BOOL FAR PASCAL SettingsDlgProc( HWND hDlg, WORD wMsg,
 *                                   WORD wParam, LONG lParam )
 *
 *  Description
 *     This is the settings dialog handler.
 *
 *  Comments:
 *      6/24/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL FAR PASCAL SettingsDlgProc( HWND hDlg, WORD wMsg,
                                 WORD wParam, LONG lParam )
{
   static LOCALHANDLE  hCCB ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         hCCB = (LOCALHANDLE) LOWORD( lParam ) ;
         if (!SettingsDlgInit( hDlg, hCCB ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
         return ShadowControl( hDlg, wMsg, wParam, RGBCYAN ) ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_OK:
               if (SettingsDlgTerm( hDlg, hCCB ))
               {
                  EndDialog( hDlg, TRUE ) ;
                  return ( TRUE ) ;
               }
               break ;

            case IDD_CANCEL:
               EndDialog( hDlg, FALSE ) ;
               return ( TRUE ) ;

         }
         break ;

      case WM_DESTROY:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         HDC         hDC ;

         hDC = BeginPaint(hDlg, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         EndPaint( hDlg, &ps ) ;

      }
      //
      // Fall through
      //
      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

   }
   return ( FALSE ) ;

} /* end of SettingsDlgProc() */

/************************************************************************
 * End of File: dialog.c
 ************************************************************************/

