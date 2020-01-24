/************************************************************************
 *
 *     Project:  CONXLIB      
 *
 *      Module:  shipcust.c
 *
 *     Remarks:  customer info and shipping info dialogs.
 *
 *   Revisions:  
 *
 *    3/1/92   C.Kindel    Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "CONXLIBi.h"

/****************************************************************
 * CustomerInfoDialog
 ****************************************************************/
BOOL WINAPI CustomerInfoDialog( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINIFile, DWORD dwData2 )
{
   return DoSetupDialog( hwndParent, szHelpFile, DLG_CUSTOMERINFO, lpszINIFile, dwData2 ) ;
}  

/****************************************************************
 * ShippingInfoDialog
 ****************************************************************/
BOOL WINAPI ShippingInfoDialog( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINIFile, DWORD dwData2 )
{
   return DoSetupDialog( hwndParent, szHelpFile, DLG_SHIPPINGINFO, lpszINIFile, dwData2 ) ;
}


BOOL NEAR PASCAL InitCustomerInfo( HWND hDlg, LPSTR lpszINIFile ) ;
BOOL NEAR PASCAL SaveCustomerInfo( HWND hDlg, LPSTR lpszINIFile, LPSTR lspzSection ) ;
VOID NEAR PASCAL CustomerInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
/****************************************************************
 *  BOOL CALLBACK fnCustomerInfo( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Customer info dialog box.  Contains name, address, phone
 *    and more...
 *
 *    Contents are written to the [Customer Info] section of
 *    the ini file whose name is pointed to by the lpszINI member
 *    of DlgData.
 *
 *
 ****************************************************************/
BOOL CALLBACK fnCustomerInfo( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
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
         InitCustomerInfo( hDlg, lpDlgData->lpszINIFile ) ;
         CustomerInfoValidate( hDlg, wMsg, wParam, lParam ) ;

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), lpDlgData->hfontSmall, FALSE ) ;

         if (lpDlgData->dwData2 & 0x0000001)
         {
            ShowWindow( GetDlgItem( hDlg, DLG_BILLINGINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_SHIPPINGINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_COMMSETTINGS ), SW_HIDE ) ;
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
                  SaveCustomerInfo( hDlg, lpDlgData->lpszINIFile, rglpsz[IDS_INI_CUSTADDRESS] ) ;

               }

               lpDlgData->uiExitBtnID = idItem ;
               EndDialog( hDlg, idItem ) ;
            break ;

            /***********************************************************/
            /*  This stuff is common to Shipping and CustomerInfo      */
            /***********************************************************/
            /*
             * Edit controls that can be validated on the fly
             */
            case IDD_NAME:          /* Required */
            case IDD_STREET1:       /* Required */
            case IDD_CITY:          /* Required */
            case IDD_STATE:         /* Required if USA */
            case IDD_ZIP:           /* Required if USA or CANADA */
            case IDD_AREACODE:      /* Required if UAS or CANADA */
            case IDD_PHONENUMBER:   /* Required if USA or CANADA */
            case IDD_OTHEREDIT:     /* Required if OTHER */
               if (wNotifyCode == EN_CHANGE)
                  CustomerInfoValidate( hDlg, wMsg, wParam, lParam ) ;
            break ;

            /*
             * Misc controls that may, or maynot be validated
             */
            case IDD_COMPANYNAME:   /* Not Required */
            case IDD_STREET2:       /* Not Required */
            case IDD_EXTENSION:     /* Not Required */

            case IDD_USA:           /* Not Required */
            case IDD_CANADA:        /* Not Required */
            case IDD_OTHER:         /* Not Required */
               if (wNotifyCode == BN_CLICKED)
                  CustomerInfoValidate( hDlg, wMsg, wParam, lParam ) ;
            break ;
            /***********************************************************/

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpDlgData->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_CUSTOMER ) );
            break ;

            default :
               return FALSE ;
         }
         break ;
      }

      case WM_PAINT:
         UpdateIndicators( hDlg ) ;
      return FALSE ;

      case WM_INDICATORPAINT:
         lpDlgData = GET_DLGDATAPROP( hDlg ) ;
         lpDlgData->fIndicatorPaint = FALSE ;
         /*
          * Paint our "required data" indicators...
          */
      {
         HDC   hDC = GetDC( hDlg ) ;
         HFONT hfont ;

         hfont = SelectObject( hDC, (HFONT)SendMessage( hDlg, WM_GETFONT, 0, 0L ) ) ;
         SetTextColor( hDC, RGBLTRED ) ;
         SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;

         /*
          * Members always required
          */
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_NAME ) ) ;
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_CITY ) ) ;
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_STREET1 ) ) ;

         /*
          * Conditional members.
          */
         if (IsDlgButtonChecked( hDlg, IDD_USA ))
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_STATE ) ) ;
         else
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_STATE ) ) ;

         if (!IsDlgButtonChecked( hDlg, IDD_OTHER ))
         {
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_ZIP ) ) ;
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_AREACODE ) ) ;
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_PHONENUMBER ) ) ;

            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_OTHEREDIT ) ) ;
         }
         else
         {
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_ZIP ) ) ;
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_AREACODE ) ) ;
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_PHONENUMBER ) ) ;

            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_OTHEREDIT ) ) ;
         }

         SelectObject( hDC, hfont ) ;

         ReleaseDC( hDlg, hDC ) ;
      }
      break ;

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      return FALSE ;

      default:
         return FALSE ;

   }
   return TRUE ;

} /* fnCustomerInfo()  */

BOOL NEAR PASCAL ReadCustomerInfo( HWND hDlg, LPSTR lpszINIFile, LPSTR lpszSection ) ;


/****************************************************************
 *
 *  BOOL NEAR PASCAL InitCustomerInfo( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL InitCustomerInfo( HWND hDlg, LPSTR lpszINIFile )
{
   /*
    * Read the INI file.  We use a function for this because
    * InitBillingInfo contains all the same controls.
    */
   ReadCustomerInfo( hDlg, lpszINIFile, rglpsz[IDS_INI_CUSTADDRESS] ) ;

   return TRUE ;
} /* InitCustomerInfo()  */

/****************************************************************
 *  BOOL NEAR PASCAL ReadCustomerInfo( HWND hDlg, LPSTR lpszINIFile, LPSTR lpszSection )
 *
 *  Description: 
 *
 *    Reads customer information from the lpszSection section
 *    of the lpszINIFile INI file.  Puts it all in the appropriate dlg
 *    items of hDlg.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL ReadCustomerInfo( HWND hDlg, LPSTR lpszINIFile, LPSTR lpszSection )
{
   char szBuf[128] ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_NAME], "", szBuf, MAX_NAME+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_NAME, EM_LIMITTEXT, MAX_NAME, 0L ) ;
   SetDlgItemText( hDlg, IDD_NAME, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_COMPANY], "", szBuf, MAX_NAME+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_COMPANYNAME, EM_LIMITTEXT, MAX_NAME, 0L ) ;
   SetDlgItemText( hDlg, IDD_COMPANYNAME, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_STREET1], "", szBuf, MAX_STREET+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_STREET1, EM_LIMITTEXT, MAX_STREET, 0L ) ;
   SetDlgItemText( hDlg, IDD_STREET1, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_STREET2], "", szBuf, MAX_STREET+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_STREET1, EM_LIMITTEXT, MAX_STREET, 0L ) ;
   SetDlgItemText( hDlg, IDD_STREET2, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_CITY], "", szBuf, MAX_CITY+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_CITY, EM_LIMITTEXT, MAX_CITY, 0L ) ;
   SetDlgItemText( hDlg, IDD_CITY, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_STATE], "", szBuf, MAX_STATE+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_STATE, EM_LIMITTEXT, MAX_STATE, 0L ) ;
   SetDlgItemText( hDlg, IDD_STATE, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_ZIP], "", szBuf, MAX_ZIP+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_ZIP, EM_LIMITTEXT, MAX_ZIP, 0L ) ;
   SetDlgItemText( hDlg, IDD_ZIP, szBuf ) ;

   switch (GetPrivateProfileInt( lpszSection, rglpsz[IDS_INI_COUNTRYCODE], COUNTRY_USA, lpszINIFile ))
   {
      case COUNTRY_USA:
         CheckRadioButton( hDlg, IDD_USA, IDD_OTHER, IDD_USA ) ;
      break ;

      case COUNTRY_CANADA:
         CheckRadioButton( hDlg, IDD_USA, IDD_OTHER, IDD_CANADA ) ;
      break ;

      case COUNTRY_OTHER:
         CheckRadioButton( hDlg, IDD_USA, IDD_OTHER, IDD_OTHER ) ;
      break ;
   }
   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_COUNTRY], "", szBuf, MAX_COUNTRY+1, lpszINIFile ) ;
   SetDlgItemText( hDlg, IDD_OTHEREDIT, szBuf ) ;
   SendDlgItemMessage( hDlg, IDD_OTHEREDIT, EM_LIMITTEXT, MAX_COUNTRY, 0L ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_AREACODE], "", szBuf, MAX_AREACODE+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_AREACODE, EM_LIMITTEXT, MAX_AREACODE, 0L ) ;
   SetDlgItemText( hDlg, IDD_AREACODE, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_NUMBER], "", szBuf, MAX_PHONE+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_PHONENUMBER, EM_LIMITTEXT, MAX_PHONE, 0L ) ;
   SetDlgItemText( hDlg, IDD_PHONENUMBER, szBuf ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_EXT], "", szBuf, MAX_EXT+1, lpszINIFile ) ;
   SendDlgItemMessage( hDlg, IDD_EXTENSION, EM_LIMITTEXT, MAX_EXT, 0L ) ;
   SetDlgItemText( hDlg, IDD_EXTENSION, szBuf ) ;

   return TRUE ;

} /* ReadCustomerInfo()  */


/****************************************************************
 *
 *  BOOL NEAR PASCAL SaveCustomerInfo( HWND hDlg, LPSTR lpszINIFile, LPSTR lpszSection )
 *
 ****************************************************************/
BOOL NEAR PASCAL SaveCustomerInfo( HWND hDlg, LPSTR lpszINIFile, LPSTR lpszSection )
{
   char szBuf[128] ;

   GetDlgItemText( hDlg, IDD_NAME, szBuf, MAX_NAME+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_NAME], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_COMPANYNAME, szBuf, MAX_NAME+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_COMPANY], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_STREET1, szBuf, MAX_STREET+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_STREET1], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_STREET2, szBuf, MAX_STREET+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_STREET2], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_CITY, szBuf, MAX_CITY+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_CITY], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_STATE, szBuf, MAX_STATE+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_STATE], szBuf,  lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_ZIP, szBuf, MAX_ZIP+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_ZIP], szBuf, lpszINIFile ) ;

   switch (DlgWhichRadioButton( hDlg, IDD_USA, IDD_OTHER, IDD_USA ))
   {
      case IDD_USA:
         wsprintf( szBuf, "%d", COUNTRY_USA ) ;
         WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_COUNTRYCODE], szBuf, lpszINIFile ) ;
      break ;

      case IDD_CANADA:
         wsprintf( szBuf, "%d", COUNTRY_CANADA ) ;
         WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_COUNTRYCODE], szBuf, lpszINIFile ) ;
      break ;

      case IDD_OTHER:
         wsprintf( szBuf, "%d", COUNTRY_OTHER ) ;
         WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_COUNTRYCODE], szBuf, lpszINIFile ) ;
      break ;
   }

   GetDlgItemText( hDlg, IDD_OTHEREDIT, szBuf, MAX_COUNTRY+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_COUNTRY], szBuf, lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_AREACODE, szBuf, MAX_AREACODE+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_AREACODE], szBuf,lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_PHONENUMBER, szBuf, MAX_PHONE+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_NUMBER], szBuf,  lpszINIFile ) ;

   GetDlgItemText( hDlg, IDD_EXTENSION, szBuf, MAX_EXT+1 ) ;
   WritePrivateProfileString( lpszSection, rglpsz[IDS_INI_EXT], szBuf, lpszINIFile ) ;

   return TRUE ;
} /* SaveCustomerInfo()  */

/****************************************************************
 *  VOID NEAR PASCAL CustomerInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL CustomerInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   if (
         GetWindowTextLength( GetDlgItem( hDlg, IDD_NAME ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_STREET1 ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_CITY ) ) &&
         ((!IsDlgButtonChecked( hDlg, IDD_USA )) ||
               (MAX_STATE == GetWindowTextLength( GetDlgItem( hDlg, IDD_STATE ) ))) &&
         ((IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_ZIP ) )) &&
         ((IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_AREACODE ) )) &&
         ((IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_PHONENUMBER ) )) &&
         ((!IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_OTHEREDIT ) ))
         )
      EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
   else
      EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;

   if (IsDlgButtonChecked( hDlg, IDD_OTHER ))
      EnableWindow( GetDlgItem( hDlg, IDD_OTHEREDIT ), TRUE ) ;
   else
      EnableWindow( GetDlgItem( hDlg, IDD_OTHEREDIT ), FALSE ) ;

   UpdateIndicators( hDlg ) ;

} /* CustomerInfoValidate()  */

BOOL NEAR PASCAL InitShippingInfo( HWND hDlg, LPSTR lpszINIFile ) ;
BOOL NEAR PASCAL SaveShippingInfo( HWND hDlg, LPSTR lpszINIFile ) ;
VOID NEAR PASCAL ShippingInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
/****************************************************************
 *  BOOL CALLBACK fnShippingInfo( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *
 ****************************************************************/
BOOL CALLBACK fnShippingInfo( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   LPDLGDATA    lpDlgData ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         DP1( hWDB, "WM_INITDIALOG" ) ;
         lpDlgData = (LPDLGDATA)lParam ;

         SET_DLGDATAPROP( hDlg, lpDlgData ) ;

         ComboBox_AddString( GetDlgItem( hDlg, IDD_MEDIA ), GRCS( IDS_MEDIA_12 ) ) ;
         ComboBox_AddString( GetDlgItem( hDlg, IDD_MEDIA ), GRCS( IDS_MEDIA_144 ) ) ;

         /*
          * Read data from INI file
          */
         InitShippingInfo( hDlg, lpDlgData->lpszINIFile ) ;
         ShippingInfoValidate( hDlg, wMsg, wParam, lParam ) ;

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), lpDlgData->hfontSmall, FALSE ) ;

         if (lpDlgData->dwData2 & 0x0000001)
         {
            ShowWindow( GetDlgItem( hDlg, DLG_BILLINGINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_CUSTOMERINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_COMMSETTINGS ), SW_HIDE ) ;
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
                  SaveShippingInfo( hDlg, lpDlgData->lpszINIFile ) ;
               }

               lpDlgData->uiExitBtnID = idItem ;
               EndDialog( hDlg, idItem ) ;
            break ;

            case IDD_USECUSTADDRESS:         
               if (wNotifyCode == BN_CLICKED)
               {
                  char szBuf[MAX_NAME+1] ;

                  /*
                   * If we are supposed to use the customer address OR
                   * there is no current shipping address, read the
                   * customer address
                   */
                  if (IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ) ||
                      !GetPrivateProfileString( rglpsz[IDS_INI_SHIPINFO], rglpsz[IDS_INI_NAME], "", szBuf, MAX_NAME+1, lpDlgData->lpszINIFile ))
                     ReadCustomerInfo( hDlg, lpDlgData->lpszINIFile, rglpsz[IDS_INI_CUSTADDRESS] ) ;
                  else                                                                        
                     ReadCustomerInfo( hDlg, lpDlgData->lpszINIFile, rglpsz[IDS_INI_SHIPINFO] ) ;

                  /*
                   * Hack!
                   */
                  ShippingInfoValidate( hDlg, WM_INITDIALOG, wParam, lParam ) ;
               }
            break ;


            /***********************************************************/
            /*  This stuff is common to Shipping and CustomerInfo      */
            /***********************************************************/
            /*
             * Edit controls that can be validated on the fly
             */
            case IDD_NAME:          /* Required */
            case IDD_STREET1:       /* Required */
            case IDD_CITY:          /* Required */
            case IDD_STATE:         /* Required if USA */
            case IDD_ZIP:           /* Required if USA or CANADA */
            case IDD_AREACODE:      /* Required if UAS or CANADA */
            case IDD_PHONENUMBER:   /* Required if USA or CANADA */
            case IDD_OTHEREDIT:     /* Required if OTHER */
               if (wNotifyCode == EN_CHANGE)
                  ShippingInfoValidate( hDlg, wMsg, wParam, lParam ) ;
            break ;

            /*
             * Misc controls that may, or maynot be validated
             */
            case IDD_COMPANYNAME:   /* Not Required */
            case IDD_STREET2:       /* Not Required */
            case IDD_EXTENSION:     /* Not Required */

            case IDD_USA:           /* Not Required */
            case IDD_CANADA:        /* Not Required */
            case IDD_OTHER:         /* Not Required */
               if (wNotifyCode == BN_CLICKED)
                  ShippingInfoValidate( hDlg, wMsg, wParam, lParam ) ;
            break ;
            /***********************************************************/

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpDlgData->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_SHIPPING ));
            break ;

            default :
               return FALSE ;
         }
      }
      break ;

      case WM_PAINT:
         UpdateIndicators( hDlg ) ;
      return FALSE ;

      case WM_INDICATORPAINT:
         lpDlgData = GET_DLGDATAPROP( hDlg ) ;
         lpDlgData->fIndicatorPaint = FALSE ;
      /*
       * Paint our "required data" indicators...
       */
      {
         HDC hDC = GetDC( hDlg ) ;
         HFONT hfont ;

         hfont = SelectObject( hDC, (HFONT)SendMessage( hDlg, WM_GETFONT, 0, 0L ) ) ;
         SetTextColor( hDC, RGBLTRED ) ;
         SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;

         /*
          * Members always required
          */
         if (!IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
         {
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_NAME ) ) ;
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_CITY ) ) ;
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_STREET1 ) ) ;
         }
         else
         {
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_NAME ) ) ;
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_CITY ) ) ;
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_STREET1 ) ) ;
         }

         /*
          * Conditional members.
          */
         if (IsDlgButtonChecked( hDlg, IDD_USA ) &&
             !IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_STATE ) ) ;
         else
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_STATE ) ) ;

         if (!IsDlgButtonChecked( hDlg, IDD_OTHER ) &&
             !IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
         {
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_ZIP ) ) ;
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_AREACODE ) ) ;
            PaintIndicator( hDC, GetDlgItem( hDlg, IDD_PHONENUMBER ) ) ;

            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_OTHEREDIT ) ) ;
         }
         else
         {
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_ZIP ) ) ;
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_AREACODE ) ) ;
            ClearIndicator( hDC, GetDlgItem( hDlg, IDD_PHONENUMBER ) ) ;

            if (!IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
               PaintIndicator( hDC, GetDlgItem( hDlg, IDD_OTHEREDIT ) ) ;
            else
               ClearIndicator( hDC, GetDlgItem( hDlg, IDD_OTHEREDIT ) ) ;
         }

         SelectObject( hDC, hfont ) ;

         ReleaseDC( hDlg, hDC ) ;
      }
      break ;

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      return FALSE ;

      default:
         return FALSE ;

   }
   return TRUE ;

} /* fnShippingInfo()  */
/****************************************************************
 *
 *  BOOL NEAR PASCAL InitShippingInfo( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL InitShippingInfo( HWND hDlg, LPSTR lpszINIFile )
{
   /*
    * Read from the [Shipping Info] section
    */
   switch (GetPrivateProfileInt( rglpsz[IDS_INI_SHIPINFO], rglpsz[IDS_INI_SHIPMETHOD], SHIPMETHOD_1TO2DAY, lpszINIFile ))
   {
      case SHIPMETHOD_1TO2DAY:
         CheckRadioButton( hDlg, IDD_1TO2DAY, IDD_UPS, IDD_1TO2DAY ) ;
      break ;

      case SHIPMETHOD_2TO3DAY:
         CheckRadioButton( hDlg, IDD_1TO2DAY, IDD_UPS, IDD_2TO3DAY ) ;
      break ;

      case SHIPMETHOD_UPS:
         CheckRadioButton( hDlg,  IDD_1TO2DAY, IDD_UPS, IDD_UPS ) ;
      break ;
   }

   ComboBox_SetCurSel( GetDlgItem( hDlg, IDD_MEDIA ), 
      GetPrivateProfileInt( rglpsz[IDS_INI_SHIPINFO],
                            rglpsz[IDS_INI_MEDIA],
                            IDS_MEDIA_12 - IDS_MEDIA_12, lpszINIFile ) );

   if (GetPrivateProfileInt( rglpsz[IDS_INI_SHIPINFO],
                             rglpsz[IDS_INI_WHICHADDRESS],
                             ADDRESS_CUSTOMER, lpszINIFile ))
      CheckDlgButton( hDlg, IDD_USECUSTADDRESS, FALSE ) ;
   else
      CheckDlgButton( hDlg, IDD_USECUSTADDRESS, TRUE ) ;


   /*
    * If the USE CUSTOMER ADDRESS check box is lit then
    * fill the address items with the stuff from [Customer Information]
    */
   if (IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
      ReadCustomerInfo( hDlg, lpszINIFile, rglpsz[IDS_INI_CUSTADDRESS] ) ;
   else
      ReadCustomerInfo( hDlg, lpszINIFile, rglpsz[IDS_INI_SHIPINFO] ) ;

   return TRUE ;


} /* InitShippingInfo()  */


/****************************************************************
 *
 *  BOOL NEAR PASCAL SaveShippingInfo( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL SaveShippingInfo( HWND hDlg, LPSTR lpszINIFile )
{
   char szBuf[128] ;

   if (!IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
   {
      /*
       * Save address
       */
      SaveCustomerInfo( hDlg, lpszINIFile, rglpsz[IDS_INI_SHIPINFO] ) ;

      WritePrivateProfileString( rglpsz[IDS_INI_SHIPINFO], rglpsz[IDS_INI_WHICHADDRESS], "1", lpszINIFile ) ;
   }
   else
      WritePrivateProfileString( rglpsz[IDS_INI_SHIPINFO], rglpsz[IDS_INI_WHICHADDRESS], "0", lpszINIFile );

   switch (DlgWhichRadioButton( hDlg, IDD_1TO2DAY, IDD_UPS, IDD_1TO2DAY ))
   {
      case IDD_1TO2DAY:
         wsprintf( szBuf, "%d", SHIPMETHOD_1TO2DAY ) ;
         WritePrivateProfileString( rglpsz[IDS_INI_SHIPINFO],
                  rglpsz[IDS_INI_SHIPMETHOD], szBuf, lpszINIFile );
      break ;

      case IDD_2TO3DAY:
         wsprintf( szBuf, "%d", SHIPMETHOD_2TO3DAY ) ;
         WritePrivateProfileString( rglpsz[IDS_INI_SHIPINFO],
                  rglpsz[IDS_INI_SHIPMETHOD], szBuf, lpszINIFile );
      break ;

      case IDD_UPS:
         wsprintf( szBuf, "%d", SHIPMETHOD_UPS ) ;
         WritePrivateProfileString( rglpsz[IDS_INI_SHIPINFO],
                  rglpsz[IDS_INI_SHIPMETHOD], szBuf, lpszINIFile );
      break ;
   }

   wsprintf( szBuf, "%d",
             ComboBox_GetCurSel( GetDlgItem( hDlg, IDD_MEDIA ) ) );
   WritePrivateProfileString( rglpsz[IDS_INI_SHIPINFO],
                              rglpsz[IDS_INI_MEDIA],
                              szBuf, lpszINIFile );
   return TRUE ;

} /* SaveShippingInfo()  */

/****************************************************************
 *  VOID NEAR PASCAL ShippingInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL ShippingInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   int i ;

   if (  IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ) ||
         (
         GetWindowTextLength( GetDlgItem( hDlg, IDD_NAME ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_STREET1 ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_CITY ) ) &&
         ((!IsDlgButtonChecked( hDlg, IDD_USA )) ||
               (MAX_STATE == GetWindowTextLength( GetDlgItem( hDlg, IDD_STATE ) ))) &&
         ((IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_ZIP ) )) &&
         ((IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_AREACODE ) )) &&
         ((IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_PHONENUMBER ) )) &&
         ((!IsDlgButtonChecked( hDlg, IDD_OTHER )) ||
               GetWindowTextLength( GetDlgItem( hDlg, IDD_OTHEREDIT ) ))
         )
      )
      EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
   else
      EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;

   /*
    * If the USE CUSTOMER ADDRESS check box is lit then
    * disable all the address guys
    */
   if (IsDlgButtonChecked( hDlg, IDD_USECUSTADDRESS ))
      for ( i = IDD_NAME_LBL ; i <= IDD_EXTENSION ; i++)
         EnableWindow( GetDlgItem( hDlg, i ), FALSE ) ;
   else
      for ( i = IDD_NAME_LBL ; i <= IDD_EXTENSION ; i++)
         EnableWindow( GetDlgItem( hDlg, i ), TRUE ) ;

   if (IsDlgButtonChecked( hDlg, IDD_OTHER ))
      EnableWindow( GetDlgItem( hDlg, IDD_OTHEREDIT ), TRUE ) ;
   else
      EnableWindow( GetDlgItem( hDlg, IDD_OTHEREDIT ), FALSE ) ;

   UpdateIndicators( hDlg ) ;

} /* ShippingInfoValidate()  */


/************************************************************************
 * End of File: shipcust.c
 ***********************************************************************/

