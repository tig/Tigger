/************************************************************************
 *
 *     Project:  FontShopper
 *
 *      Module:  dlgs.c
 *
 *     Remarks:  Dialog box support routines for the following dialog
 *               boxes:
 *
 *          Customer Information - CUSTINFO.C
 *          Shipping Information - CUSTINFO.C
 *          Billing Information  - BILLING.C
 *          Comm Setup           - here.
 *
 *   Revisions:
 *
 *    3/1/92   C.Kindel    Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "CONXLIBi.h"

//#define SUPPORTSLOWBAUD
#define SUPPORT4800BAUD
#define SUPPORT9600BAUD
//#define SUPPORT14KBAUD
//#define SUPPORT19KBAUD
//#define SUPPORT38KBAUD
//#define SUPPORT56KBAUD
//#define SUPPORT128KBAUD
//#define SUPPORT256KBAUD

/****************************************************************
 * CommSettingsDialog
 ****************************************************************/
BOOL WINAPI CommSettingsDialog( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINIFile, DWORD dwData2 )
{
   return DoSetupDialog( hwndParent, szHelpFile, DLG_COMMSETTINGS, lpszINIFile, dwData2 ) ;
}  


/****************************************************************
 *  BOOL WINAPI DoSetupDialog( HWND hwndParent, UINT uiDlgID,
 *                             LPSTR lpszINIFile, DWORD dwData2 )
 *
 *  Description: 
 *
 *    Pops one of the setup Dialog boxes. uiDlgID can be:
 *
 *          DLG_CUSTOMERINFO
 *          DLG_SHIPPINGINFO
 *          DLG_BILLINGINFO
 *          DLG_COMMSETTINGS
 *
 * Each dialog box has a set of exit buttons.  There will always be
 * an OK and/or CANCEL plus one button for each of the other dialog
 * boxes.
 *
 * If any of the exit buttons except OK or CANCEL were hit we need
 * to pop up a different dialog box.  the uiExitBtnMember contains
 * the dlg ID of the exit button hit.  These IDs are the same as
 * the dialog template IDs.
 *
 * For all of these dialogs, if the first bit of dwData2 is set
 * the other dialog buttons will not be shown.
 *
 *  Comments:
 *
 *    Will only return false in the case of a serious error.
 *
 ****************************************************************/
BOOL WINAPI DoSetupDialog( HWND hwndParent, LPSTR szHelpFile, UINT uiDlgID, LPSTR lpszINIFile, DWORD dwData2 )
{
   DLGDATA    DlgData ;


   DP3( hWDB, "DoSetupDialog( %04X, %d, ... )", hwndParent, uiDlgID ) ;

   DlgData.uiExitBtnID = uiDlgID ;
   DlgData.lpszINIFile = lpszINIFile ;
   DlgData.dwData2 = dwData2 ;
   DlgData.fIndicatorPaint = FALSE ;
   DlgData.hfontSmall = ReallyCreateFontEx( NULL, "MS Sans Serif", "Regular", 8, 0 ) ;
   DlgData.lpszHelpFile = szHelpFile ;

DoADialog:

   switch (DlgData.uiExitBtnID)
   {
      case DLG_CUSTOMERINFO:
         DialogBoxParam( hmodDLL,
                         MAKEINTRESOURCE( DlgData.uiExitBtnID ),
                         hwndParent,
                         fnCustomerInfo,
                         (LPARAM)(LPDLGDATA)&DlgData ) ;
      break ;

      case DLG_SHIPPINGINFO:
         DialogBoxParam( hmodDLL,
                         MAKEINTRESOURCE( DlgData.uiExitBtnID ),
                         hwndParent,
                         fnShippingInfo,
                         (LPARAM)(LPDLGDATA)&DlgData ) ;
      break ;

      case DLG_BILLINGINFO:
         DialogBoxParam( hmodDLL,
                         MAKEINTRESOURCE( DlgData.uiExitBtnID ),
                         hwndParent,
                         fnBillingInfo,
                         (LPARAM)(LPDLGDATA)&DlgData ) ;
      break ;

      case DLG_COMMSETTINGS:
         DialogBoxParam( hmodDLL,
                         MAKEINTRESOURCE( DlgData.uiExitBtnID ),
                         hwndParent,
                         fnCommSetup,
                         (LPARAM)(LPDLGDATA)&DlgData ) ;
      break ;

      default:
      {
         DP1( hWDB, "Weird dialog ID = %d", DlgData.uiExitBtnID ) ;
         DeleteObject( DlgData.hfontSmall ) ;
         return FALSE ;
      }

   }

   if (DlgData.uiExitBtnID != IDOK && DlgData.uiExitBtnID != IDCANCEL)
      goto DoADialog ;

   DeleteObject( DlgData.hfontSmall ) ;

   return DlgData.uiExitBtnID != IDCANCEL ;

} /* DoSetupDlg()  */

/****************************************************************
 *  VOID WINAPI UpdateIndicators( HWND hDlg )
 *
 *  Description: 
 *
 *    Does the postmessage that causes WM_MYPAINT to happen.
 *    only allows one in the pipe at a time.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI UpdateIndicators( HWND hDlg )
{
   LPDLGDATA lpDlgData = GET_DLGDATAPROP( hDlg ) ;

   if (!lpDlgData)
   {
      DP1( hWDB, "UpdateIndicators has a NULL lpDlgData!" ) ;
      return ;
   }

   if (!lpDlgData->fIndicatorPaint)
   {
      lpDlgData->fIndicatorPaint = TRUE ;
      PostMessage( hDlg, WM_INDICATORPAINT, 0, 0L ) ;
   }

} /* UpdateIndicators()  */

/****************************************************************
 *  VOID WINAPI PaintIndicator( HDC hDC, HWND hwndCtl )
 *
 *  Description: 
 *
 *    Given a handle to a control, this function either paints
 *    a red * by the control, or wipes it clean based on whether
 *    that control has data in it or not.
 *
 *    The HDC is the DC of the parent (the dialog box).
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI PaintIndicator( HDC hDC, HWND hwndCtl )
{
   RECT        rcCtl ;
   TEXTMETRIC  tm ;
   int         nWidth ;
   HDC         hDC2 = NULL ;

   if (hDC == NULL)
      hDC = hDC2 = GetDC( GetParent( hwndCtl ) ) ;

   nWidth = GetTextMetricsCorrectly( hDC, &tm ) ;
   GetWindowRect( hwndCtl, &rcCtl ) ;

   ScreenToClient( GetParent( hwndCtl ), (LPPOINT)&rcCtl.left ) ;
   ScreenToClient( GetParent( hwndCtl ), (LPPOINT)&rcCtl.right ) ;

   if (!GetWindowTextLength( hwndCtl ))
      TextOut( hDC, rcCtl.left - nWidth - 1, rcCtl.top, "*", 1 ) ;
   else
   {
      rcCtl.right = rcCtl.left - 1 ;
      rcCtl.left -= nWidth + 1 ;

      ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rcCtl, NULL, 0, NULL ) ;
   }

   if (hDC == hDC2)
      ReleaseDC( GetParent( hwndCtl ), hDC ) ;

} /* PaintIndicator()  */

/****************************************************************
 *  VOID WINAPI ClearIndicator( HDC hDC, HWND hwndCtl )
 *
 *  Description: 
 *
 *    Always clears an indicator.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI ClearIndicator( HDC hDC, HWND hwndCtl )
{
   RECT        rcCtl ;
   TEXTMETRIC  tm ;
   int         nWidth ;

   HDC         hDC2 = NULL ;

   if (hDC == NULL)
      hDC = hDC2 = GetDC( GetParent( hwndCtl ) ) ;

   nWidth = GetTextMetricsCorrectly( hDC, &tm ) ;
   GetWindowRect( hwndCtl, &rcCtl ) ;

   ScreenToClient( GetParent( hwndCtl ), (LPPOINT)&rcCtl.left ) ;
   ScreenToClient( GetParent( hwndCtl ), (LPPOINT)&rcCtl.right ) ;

   rcCtl.right = rcCtl.left - 1 ;
   rcCtl.left -= nWidth + 1 ;

   ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rcCtl, NULL, 0, NULL ) ;

   if (hDC == hDC2)
      ReleaseDC( GetParent( hwndCtl ), hDC ) ;

} /* ClearIndicator()  */

BOOL NEAR PASCAL InitCommSettings( HWND hDlg, LPSTR lpszINIFile ) ;
BOOL NEAR PASCAL SaveCommSettings( HWND hDlg, LPSTR lpszINIFile ) ;
/****************************************************************
 *  BOOL CALLBACK fnCommSetup   ( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *
 ****************************************************************/
BOOL CALLBACK fnCommSetup   ( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   LPDLGDATA    lpDlgData ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         DP1( hWDB, "WM_INITDIALOG" ) ;
         lpDlgData = (LPDLGDATA)lParam ;

         SET_DLGDATAPROP( hDlg, lpDlgData ) ;

         /*
          * Read data from INI file
          */
         InitCommSettings( hDlg, lpDlgData->lpszINIFile ) ;

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), lpDlgData->hfontSmall, FALSE ) ;
         SetWindowFont( GetDlgItem( hDlg, IDD_DIALSTRING ), lpDlgData->hfontSmall, FALSE ) ;

         if (lpDlgData->dwData2 & 0x0000001)
         {
            ShowWindow( GetDlgItem( hDlg, DLG_CUSTOMERINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_SHIPPINGINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_BILLINGINFO ), SW_HIDE ) ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), 0 ) ;
      return TRUE ;

      case WM_COMMAND:
      {
         UINT     idItem   = (UINT)LOWORD( wParam ) ;
         UINT     wNotifyCode = (UINT)HIWORD( lParam ) ;

         lpDlgData = GET_DLGDATAPROP( hDlg ) ;

         switch (idItem)
         {
            case IDOK:
            case IDCANCEL:
            case DLG_CUSTOMERINFO:
            case DLG_SHIPPINGINFO:
            case DLG_BILLINGINFO:
            case DLG_COMMSETTINGS:
               /*
                * These all mean "Exit", and all except IDCANCEL imply
                * "Save changes"...
                *
                * If it's not CANCEL and if IDOK is enabled then we must
                * have valid data.
                * Write that data...
                */
               if (idItem != IDCANCEL &&
                   IsWindowEnabled( GetDlgItem( hDlg, IDOK ) ))
               {
                  SaveCommSettings( hDlg, lpDlgData->lpszINIFile ) ;

               }

               lpDlgData->uiExitBtnID = idItem ;
               EndDialog( hDlg, idItem ) ;
            break ;

            case IDD_DIALPREFIX:
            case IDD_DIALSUFFIX:
            case IDD_206:
            {
               char szBuf[MAX_SPECIALPREFIX*2 + MAX_PHONENUMBER+2] ;
               char  szPre[MAX_SPECIALPREFIX+1] ;
               char  szSuf[MAX_SPECIALPREFIX+1] ;

               GetDlgItemText( hDlg, IDD_DIALPREFIX, szPre, MAX_SPECIALPREFIX+1 ) ;
               GetDlgItemText( hDlg, IDD_DIALSUFFIX, szSuf, MAX_SPECIALPREFIX+1 ) ;

               wsprintf( szBuf, "%s%s555-5555%s",
                        (LPSTR)szPre,
                        (LPSTR)(!IsDlgButtonChecked( hDlg, IDD_206 ) ? "(206)-" : ""),
                        (LPSTR)szSuf ) ;

               SetDlgItemText( hDlg, IDD_DIALSTRING, szBuf ) ;
            }
            break ;

            case IDD_MODEM:
               if (wNotifyCode == LBN_SELCHANGE)
               {
                  /*
                   * He changed the modem, so we need to update
                   * the IDD_DIALSTRING
                   */
               }
            break ;

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpDlgData->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_COMM ) );
            break ;

            default :
               return FALSE ;
         }
         break ;
      }

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      return FALSE ;

      default:
         return FALSE ;

   }
   return TRUE ;

} /* fnCommSetup()  */

BOOL NEAR PASCAL GetModems( HWND hwndLB ) ;
BOOL NEAR PASCAL GetBaudRates( HWND hwndCB ) ;
BOOL NEAR PASCAL GetPorts( HWND hwndCB ) ;

/****************************************************************
 *
 *  BOOL NEAR PASCAL InitCommSettings( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL InitCommSettings( HWND hDlg, LPSTR lpszINIFile )
{
   char  szBuf[128] ;
   char  szPre[MAX_SPECIALPREFIX+1] ;
   char  szSuf[MAX_SPECIALPREFIX+1] ;
   BOOL  f206 ;

   /*
    * Do the EM_LIMITTEXT thang on the edit controls.
    */
   SendDlgItemMessage( hDlg, IDD_DIALPREFIX, EM_LIMITTEXT, MAX_SPECIALPREFIX, 0L ) ;
   SendDlgItemMessage( hDlg, IDD_DIALSUFFIX, EM_LIMITTEXT, MAX_SPECIALPREFIX, 0L ) ;

   /*
    * Is the '206' flag set?
    */
   CheckDlgButton( hDlg, IDD_206, 
      f206 = GetPrivateProfileInt( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_LOCATION], FALSE, lpszINIFile )) ;

   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_PREFIX], "1-", szPre, MAX_SPECIALPREFIX+1, lpszINIFile ) ;
   SetDlgItemText( hDlg, IDD_DIALPREFIX, szPre ) ;

   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_SUFFIX], "", szSuf, MAX_SPECIALPREFIX+1, lpszINIFile ) ;
   SetDlgItemText( hDlg, IDD_DIALSUFFIX, szSuf ) ;

   wsprintf( szBuf, "%s%s555-5555%s",
            (LPSTR)szPre,
            (LPSTR)(!f206 ? "(206)-" : ""),
            (LPSTR)szSuf ) ;

   SetDlgItemText( hDlg, IDD_DIALSTRING, szBuf ) ;

   /*
    * Fill the modem list box and select one
    */
   GetModems( GetDlgItem( hDlg, IDD_MODEM ) ) ;
   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_MODEM], "Hayes", szBuf, MAX_MODEMTYPE, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_MODEM, CB_SELECTSTRING, 0, (LPARAM)(LPSTR)szBuf ) ;

   GetPorts( GetDlgItem( hDlg, IDD_PORT ) ) ;
   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_PORT], "Com1:", szBuf, MAX_COMMDEVICE, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_PORT, CB_SELECTSTRING, 0, (LPARAM)(LPSTR)szBuf ) ;

   GetBaudRates( GetDlgItem( hDlg, IDD_BAUDRATE ) ) ;
   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_BAUD], "2400", szBuf, MAX_BAUDRATE, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_BAUDRATE, CB_SELECTSTRING, 0, (LPARAM)(LPSTR)szBuf ) ;

   switch (GetPrivateProfileInt( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_DIALTYPE], DIALTYPE_TONE, lpszINIFile ))
   {
      case DIALTYPE_TONE:
         CheckRadioButton( hDlg, IDD_TONE, IDD_PULSE, IDD_TONE ) ;
      break ;

      case DIALTYPE_PULSE:
         CheckRadioButton( hDlg, IDD_TONE, IDD_PULSE, IDD_PULSE ) ;
      break ;
   }

   CheckDlgButton( hDlg, IDD_NOYIELD,
      GetPrivateProfileInt( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_NOYIELD], FALSE, lpszINIFile ) ) ;

   return TRUE ;

} /* InitCommSettings()  */


/****************************************************************
 *
 *  BOOL NEAR PASCAL SaveCommSettings( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL SaveCommSettings( HWND hDlg, LPSTR lpszINIFile )
{
   char  szBuf[128] ;
   int   n ;

   /*
    * Is the '206' flag set?
    */
   wsprintf( szBuf, "%d", IsDlgButtonChecked( hDlg, IDD_206 ) ) ;
   WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_LOCATION], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_DIALPREFIX, szBuf, MAX_SPECIALPREFIX+1 ) ;
   WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_PREFIX], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_DIALSUFFIX, szBuf, MAX_SPECIALPREFIX+1 ) ;
   WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_SUFFIX], szBuf, lpszINIFile ) ;

   n = (int)SendDlgItemMessage( hDlg, IDD_MODEM, CB_GETCURSEL, 0, 0L ) ;
   if (n != -1)
   {
      SendDlgItemMessage( hDlg, IDD_MODEM, CB_GETLBTEXT, n, (LPARAM)(LPSTR)szBuf ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_MODEM], szBuf, lpszINIFile ) ;
   }

   n = (int)SendDlgItemMessage( hDlg, IDD_PORT, CB_GETCURSEL, 0, 0L ) ;
   if (n != -1)
   {
      SendDlgItemMessage( hDlg, IDD_PORT, CB_GETLBTEXT, n, (LPARAM)(LPSTR)szBuf ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_PORT], szBuf, lpszINIFile ) ;
   }

   n = (int)SendDlgItemMessage( hDlg, IDD_BAUDRATE, CB_GETCURSEL, 0, 0L ) ;
   if (n != -1)
   {
      SendDlgItemMessage( hDlg, IDD_BAUDRATE, CB_GETLBTEXT, n, (LPARAM)(LPSTR)szBuf ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_BAUD], szBuf, lpszINIFile ) ;
   }

   switch (DlgWhichRadioButton( hDlg, IDD_TONE, IDD_PULSE, IDD_TONE ))
   {
      case IDD_TONE:
         wsprintf( szBuf, "%d", DIALTYPE_TONE ) ;
      break ;

      case IDD_PULSE:
         wsprintf( szBuf, "%d", DIALTYPE_PULSE ) ;
      break ;

   }
   WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_DIALTYPE], szBuf, lpszINIFile ) ;

   wsprintf( szBuf, "%d", IsDlgButtonChecked( hDlg, IDD_NOYIELD ) ) ;
   WritePrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS], rglpsz[IDS_INI_NOYIELD], szBuf, lpszINIFile ) ;

   return TRUE ;

} /* SaveCommSettings()  */

/****************************************************************
 *  BOOL NEAR PASCAL GetModems( HWND hwndLB )
 *
 *  Description: 
 *
 *    Fills the modem listbox with modems from MODEMS.INI
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL GetModems( HWND hwndLB )
{
   LPSTR lpModemBuf ;
   LPSTR lpszModem ;
   char  szModemINI[144] ;

   GetModuleFileName( hmodDLL, szModemINI, 144 ) ;
   if (lpszModem = _fstrrchr( szModemINI, '\\' ))
      lstrcpy( lpszModem+1, "MODEM.INI" ) ;
   else
      lstrcpy( szModemINI, "MODEM.INI" ) ;

   SendMessage( hwndLB, CB_RESETCONTENT, 0, 0L ) ;
   /*
    * Allocate a chunk that *should* be big enough.
    */
   if (!(lpModemBuf = GlobalAllocPtr( GHND, (UINT)(1024*16) +1 )))
   {
      DP1( hWDB, "Could not allocate %u bytes for modem buffer", 1024*16 + 1) ;
      return FALSE ;
   }

   if (GetPrivateProfileSections( lpModemBuf,
                                  1024*16,
                                  szModemINI ))
   {
      for (lpszModem = lpModemBuf ;
         *lpszModem != '\0' ;
         lpszModem += lstrlen( lpszModem ) + 1)
      {
         /*
          * lpszModem now points to a Modem name 
          */
         SendMessage( hwndLB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)lpszModem ) ;
      }
   }
   else
   {
      /*
       * It doesn't look like this is a INI file (or the buffer
       * wasn't big enough (I doubt it).
       */
      DP1( hWDB, "GetPrivateprofileString (Modems) failed" ) ;

      GlobalFreePtr( lpModemBuf ) ;
      return FALSE ;
   }

   GlobalFreePtr( lpModemBuf ) ;

   return TRUE  ;

} /* GetModems()  */


/****************************************************************
 *  BOOL NEAR PASCAL GetBaudRates( hwndCB )
 *
 *  Description: 
 *
 *    Fills combo box with applicable baud rates.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL GetBaudRates( HWND hwndCB )
{
   SendMessage( hwndCB, CB_RESETCONTENT, 0, 0L ) ;

#ifdef SUPPORTSLOWBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"110" ) ;
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"300" ) ;
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"600" ) ;
#endif
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"1200" ) ;
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"2400" ) ;

#ifdef SUPPORT4800BAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"4800" ) ;
#endif

#ifdef SUPPORT9600BAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"9600" ) ;
#endif

#ifdef SUPPORT14KBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"14400" ) ;
#endif

#ifdef SUPPORT19KBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"19200" ) ;
#endif

#ifdef SUPPORT38KBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"38400" ) ;
#endif

#ifdef SUPPORT56KBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"56000" ) ;
#endif

#ifdef SUPPORT128KBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"128000" ) ;
#endif

#ifdef SUPPORT256KBAUD
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"256000" ) ;
#endif

   return TRUE ;

} /* GetBaudRates()  */

/****************************************************************
 *  BOOL NEAR PASCAL GetPorts(  HWND hwndCB )
 *
 *  Description: 
 *
 *    Fills combo box with applicable comm ports
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL GetPorts( HWND hwndCB )
{
   SendMessage( hwndCB, CB_RESETCONTENT, 0, 0L ) ;

   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Com1:" ) ;
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Com2:" ) ;
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Com3:" ) ;
   SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)(LPSTR)"Com4:" ) ;

   return TRUE ;

} /* GetPorts()  */

/************************************************************************
 * End of File: dlgs.c
 ***********************************************************************/

