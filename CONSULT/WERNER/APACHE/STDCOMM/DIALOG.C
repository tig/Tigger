//---------------------------------------------------------------------------
//
//  Module: dialog.c
//
//  Purpose:
//     Dialog box handling routines for STDCOMM.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              2/22/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"     // precompiled stuff

#include "stdcomm.h"

//************************************************************************
//  LPWORD NEAR GetResourceTable( HINSTANCE hInstance, int nIDResTable )
//
//  Description:
//    Finds and loads the binary information from the resource
//    file.
//
//  Parameters:
//     HINSTANCE hInstance
//        instance for resource table
//
//     int nIDResTable
//        resource table ID
//
//  History:   Date       Author      Comment
//              2/ 5/92   BryanW      Wrote it.
//
//************************************************************************

LPWORD NEAR GetResourceTable( HINSTANCE hInstance, int nIDResTable )
{
   HRSRC    hresTable ;
   HGLOBAL  hTable ;

   hresTable =
      FindResource( hInstance, MAKEINTRESOURCE( nIDResTable ),
                    RT_RCDATA ) ;
   hTable = LoadResource( hInstance, hresTable ) ;
   return( (LPWORD) LockResource( hTable ) ) ;

} // end of GetResourceTable()

//************************************************************************
//  BOOL NEAR ReleaseResourceTable( LPWORD lpResTable )
//
//  Description:
//     Releases the resource table.
//
//  Parameters:
//     LPWORD lpResTable
//        pointer to resource table
//
//  History:   Date       Author      Comment
//              2/ 5/92   BryanW      Wrote it.
//
//************************************************************************

BOOL NEAR ReleaseResourceTable( LPWORD lpResTable )
{
   HGLOBAL  hTable ;

   hTable = (HGLOBAL) GlobalHandle( (UINT) HIWORD( lpResTable ) ) ;
   UnlockResource( hTable ) ;
   return( FreeResource( hTable ) ) ;

} // end of ReleaseResourceTable()

//---------------------------------------------------------------------------
//  BOOL NEAR FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd,
//                          int nIDString, in nIDResTable,
//                          WORD wCurrentSetting )
//
//  Description:
//     Fills the given combo box with strings from the resource
//     table starting at nIDString.  Associated items are
//     added from given table.  The combo box is notified of
//     the current setting.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to application instance
//
//     HWND hCtrlWnd
//        handle to combo box control
//
//     int nIDString
//        first resource string id
//
//     int nIDResTable
//        id value for resource table of associated values
//
//     WORD wCurrentSetting
//        current setting (for combo box selection)
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Pulled from the init procedure.
//
//---------------------------------------------------------------------------

BOOL NEAR FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
                        int nIDResTable, WORD wCurrentSetting )
{
   char   szBuffer[ MAXLEN_TEMPSTR ] ;
   WORD   wCount, wPosition ;
   WORD   wTableLen ;
   LPWORD lpResTable ;

   if (NULL == (lpResTable = GetResourceTable( hInstance, nIDResTable )))
   {
      DP1( hWDB, "FillComboBox: GetResourceTable() failed!" ) ;
      return ( FALSE ) ;
   }

   wTableLen = *lpResTable ;

   DPF3( hWDB, "FillComboBox: wTableLen = %d\r\n", wTableLen ) ;

   for (wCount = 0; wCount < wTableLen; wCount++)
   {
      // load the string from the string resources and
      // add it to the combo box

      LoadString( hInstance, nIDString + wCount, szBuffer, sizeof( szBuffer ) ) ;
      wPosition = LOWORD( SendMessage( hCtrlWnd, CB_ADDSTRING, NULL,
                                       (LPARAM) (LPSTR) szBuffer ) ) ;

      // use item data to store the actual table value

      SendMessage( hCtrlWnd, CB_SETITEMDATA, (WPARAM) wPosition,
                   (LPARAM) (LONG) *(lpResTable + wCount + 1) ) ;

      // if this is our current setting, select it

      if (*(lpResTable + wCount + 1) == wCurrentSetting)
         SendMessage( hCtrlWnd, CB_SETCURSEL, (WPARAM) wPosition, NULL ) ;
   }
   ReleaseResourceTable( lpResTable ) ;
   return ( TRUE ) ;

} // end of FillComboBox()

/************************************************************************
 *  BOOL NEAR SettingsDlgInit( HWND hDlg, LOCALHANDLE hCCB )
 *
 *  Description:
 *     Init stuff for dialog box.
 *
 *  Comments:
 *     6/23/91  baw  No comment.
 *
 ************************************************************************/

BOOL NEAR SettingsDlgInit( HWND hDlg, LOCALHANDLE hCCB )
{
   char   szBuffer[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ] ;
   WORD   wCount, wMaxCOM, wPosition ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   wMaxCOM = LOWORD( EscapeCommFunction( NULL, GETMAXCOM ) ) + 1 ;

   // load the COM prefix from resources

   LoadString( hLibInst, IDS_COMPREFIX, szTemp, sizeof( szTemp ) ) ;

   // fill port combo box and make initial selection

   for (wCount = 0; wCount < wMaxCOM; wCount++)
   {
      wsprintf( szBuffer, "%s%d", (LPSTR) szTemp, wCount + 1 ) ;
      SendDlgItemMessage( hDlg, IDD_PORTCB, CB_ADDSTRING, NULL,
                          (LPARAM) (LPSTR) szBuffer ) ;
   }
   SendDlgItemMessage( hDlg, IDD_PORTCB, CB_SETCURSEL,
                       (WPARAM) (SETTINGS( npCCB ).bPort - 1), NULL ) ;

   // disable COM port combo box if connection
   // has already been established

   EnableWindow( GetDlgItem( hDlg, IDD_PORTCB ), !(npCCB -> fConnected) ) ;

   // fill baud rate combo box and make initial selection

   FillComboBox( hLibInst, GetDlgItem( hDlg, IDD_BAUDRATECB ),
                 IDS_BAUD110, RESTABLE_BAUDRATES,
                 SETTINGS( npCCB ).wBaudRate ) ;

   // fill data bits combo box and make initial selection

   for (wCount = 5; wCount < 9; wCount++)
   {
      wsprintf( szBuffer, "%d", wCount ) ;
      wPosition = LOWORD( SendDlgItemMessage( hDlg, IDD_DATABITSCB,
                                              CB_ADDSTRING, NULL,
                                              (LPARAM) (LPSTR) szBuffer ) ) ;

      // if current selection, tell the combo box

      if (wCount == SETTINGS( npCCB ).bByteSize)
         SendDlgItemMessage( hDlg, IDD_DATABITSCB, CB_SETCURSEL,
                             (WPARAM) wPosition, NULL ) ;
   }

   // fill parity combo box and make initial selection

   FillComboBox( hLibInst, GetDlgItem( hDlg, IDD_PARITYCB ),
                 IDS_PARITYNONE, RESTABLE_PARITY,
                 SETTINGS( npCCB ).bParity ) ;

   // fill stop bits combo box and make initial selection

   FillComboBox( hLibInst, GetDlgItem( hDlg, IDD_STOPBITSCB ),
                 IDS_ONESTOPBIT, RESTABLE_STOPBITS,
                 SETTINGS( npCCB ).bStopBits ) ;

   // fill break length combo box and make initial selection

   FillComboBox( hLibInst, GetDlgItem( hDlg, IDD_BREAKLENGTHCB ),
                 IDS_BREAK100, RESTABLE_BREAKLENGTHS,
                 SETTINGS( npCCB ).wBreakLength ) ;

   // initalized the flow control settings

   CheckDlgButton( hDlg, IDD_DTRDSR,
                   (SETTINGS( npCCB ).bFlowControl & FC_DTRDSR) ) ;
   CheckDlgButton( hDlg, IDD_RTSCTS,
                   (SETTINGS( npCCB ).bFlowControl & FC_RTSCTS) ) ;
   CheckDlgButton( hDlg, IDD_XONXOFF,
                   (SETTINGS( npCCB ).bFlowControl & FC_XONXOFF) ) ;

   LocalUnlock( hCCB ) ;
   return( TRUE ) ;

} /* end of SettingsDlgInit() */

/************************************************************************
 *  BOOL NEAR SettingsDlgTerm( HWND hDlg, LOCALHANDLE hCCB )
 *
 *  Description:
 *     SettingsDlg termination routine.  Saves information to the CCB
 *     structure.
 *
 *  Comments:
 *      6/24/91  baw  Wrote this comment.
 *
 ************************************************************************/

BOOL NEAR SettingsDlgTerm( HWND hDlg, LOCALHANDLE hCCB )
{
   NPCCB  npCCB ;
   WORD   wSelection ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   // get port selection

   SETTINGS( npCCB ).bPort =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_PORTCB,
                                          CB_GETCURSEL,
                                          NULL, NULL ) ) + 1 ) ;
   // get baud rate selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BAUDRATECB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   SETTINGS( npCCB ).wBaudRate =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BAUDRATECB, CB_GETITEMDATA,
                                  (WPARAM) wSelection, NULL ) ) ;

   // get data bits selection

   SETTINGS( npCCB ).bByteSize =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_DATABITSCB,
                                          CB_GETCURSEL,
                                          NULL, NULL ) ) + 5 ) ;

   // get parity selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_PARITYCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   SETTINGS( npCCB ).bParity =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_PARITYCB,
                                          CB_GETITEMDATA,
                                          (WPARAM) wSelection,
                                           NULL ) ) ) ;

   // get stop bits selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_STOPBITSCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   SETTINGS( npCCB ).bStopBits =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_STOPBITSCB,
                                          CB_GETITEMDATA,
                                          (WPARAM) wSelection, NULL ) ) ) ;

   // get break length setting

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BREAKLENGTHCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   SETTINGS( npCCB ).wBreakLength =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BREAKLENGTHCB,
                                  CB_GETITEMDATA,
                                  (WPARAM) wSelection, NULL ) ) ;

   // get flow control settings

   SETTINGS( npCCB ).bFlowControl = 0 ;
   if (IsDlgButtonChecked( hDlg, IDD_DTRDSR ))
      SETTINGS( npCCB ).bFlowControl |= FC_DTRDSR ;
   if (IsDlgButtonChecked( hDlg, IDD_RTSCTS ))
      SETTINGS( npCCB ).bFlowControl |= FC_RTSCTS ;
   if (IsDlgButtonChecked( hDlg, IDD_XONXOFF ))
      SETTINGS( npCCB ).bFlowControl |= FC_XONXOFF ;

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
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         HCCB hCCB ;

         hCCB = (HCCB) LOWORD( lParam ) ;
         SETHCCB( hDlg, hCCB ) ;
         if (!SettingsDlgInit( hDlg, hCCB ))
            return ( FALSE ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         return ShadowControl( hDlg, wMsg, wParam, RGBLTGRAY ) ;
      }

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
               if (SettingsDlgTerm( hDlg, GETHCCB( hDlg ) ))
               {
                  EndDialog( hDlg, TRUE ) ;
                  return ( TRUE ) ;
               }
               break ;

            case IDCANCEL:
               EndDialog( hDlg, FALSE ) ;
               return ( TRUE ) ;

         }
         break ;

      case WM_DESTROY:
         REMOVEHCCB( hDlg ) ;

      // ... Fall through ,,,

      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

   }
   return ( FALSE ) ;

} /* end of SettingsDlgProc() */

//---------------------------------------------------------------------------
//  End of File: dialog.c
//---------------------------------------------------------------------------

