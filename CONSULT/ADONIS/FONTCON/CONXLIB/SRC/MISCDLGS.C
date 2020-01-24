/************************************************************************
 *
 *     Project:  Font Shopper
 *
 *      Module:  miscdlgs.c
 *
 *     Remarks:  Misc. dialog boxes.
 *
 *   Revisions:
 *    4/5/92   cek   Wrote it.
 *    6/5/92   cek   Moved to CONXLIB
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "CONXLIBi.h"

typedef struct tagUSERIDDLGDATA
{
   LPSTR lpszUserID ;
   LPSTR lpszOldPassword ;
   LPSTR lpszNewPassword ;
   LPSTR lpszHelpFile ;

   HFONT hfontSmall ;

} USERIDDLGDATA, *PUSERIDDLGDATA, FAR *LPUSERIDDLGDATA ;

BOOL CALLBACK fnUserIDNewPasswordDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK fnUserIDPasswordDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK fnVerifyOrderInfoDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;

/****************************************************************
 * BOOL WINAPI GetUserIDNewPasswordDlg( )
 *
 *  Description: 
 *
 *    Get the user's id and a new password (for changing the password).
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI GetUserIDNewPasswordDlg( HWND hwndParent, LPSTR szHelpFile,
            LPSTR szUserID, LPSTR szOldPassword, LPSTR szNewPassword )
{
   USERIDDLGDATA dlg ;
   UINT  ui ;

   dlg.lpszUserID = szUserID ;
   dlg.lpszOldPassword = szOldPassword ;
   dlg.lpszNewPassword = szNewPassword ;
   dlg.lpszHelpFile = szHelpFile ;
   dlg.hfontSmall = ReallyCreateFontEx( NULL, "MS Sans Serif", "Regular", 8, 0 ) ;

   ui = DialogBoxParam( hmodDLL,
               MAKEINTRESOURCE( DLG_NEWPASSWORD ),
               hwndParent,
               fnUserIDNewPasswordDlg, (DWORD)(LPSTR)&dlg  );

   DeleteObject( dlg.hfontSmall ) ;

   return ui == IDOK ;
} /* GetNewPasswordDlg()  */

/****************************************************************
 *  BOOL CALLBACK fnUserIDNewPasswordDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnUserIDNewPasswordDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   LPUSERIDDLGDATA lpDD ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         lpDD = (LPUSERIDDLGDATA)lParam ;
         SET_DLGDATAPROP( hDlg, lpDD ) ;

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), lpDD->hfontSmall, FALSE ) ;

         Edit_LimitText( GetDlgItem( hDlg, IDD_USERID ), MAX_PASSWORD ) ;
         Edit_LimitText( GetDlgItem( hDlg, IDD_CURPASSWORD ), MAX_PASSWORD ) ;
         Edit_LimitText( GetDlgItem( hDlg, IDD_NEWPASSWORD1 ), MAX_PASSWORD ) ;
         Edit_LimitText( GetDlgItem( hDlg, IDD_NEWPASSWORD2 ), MAX_PASSWORD ) ;

         if (lpDD->lpszUserID[0] != '\0')
         {
            Edit_SetText( GetDlgItem( hDlg, IDD_USERID ), lpDD->lpszUserID ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_USERID ), FALSE ) ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         lpDD = (LPUSERIDDLGDATA)GET_DLGDATAPROP( hDlg ) ;

         switch( idItem )
         {
            case IDD_USERID:
            case IDD_CURPASSWORD:
            case IDD_NEWPASSWORD1:
            case IDD_NEWPASSWORD2:
               if ( wNotifyCode == EN_CHANGE)
                  EnableWindow( GetDlgItem( hDlg, IDOK ),
                     GetWindowTextLength( GetDlgItem( hDlg, IDD_USERID ) ) &&
                     GetWindowTextLength( GetDlgItem( hDlg, IDD_CURPASSWORD ) ) &&
                     GetWindowTextLength( GetDlgItem( hDlg, IDD_NEWPASSWORD1 ) ) &&
                     GetWindowTextLength( GetDlgItem( hDlg, IDD_NEWPASSWORD2 ) ) ) ;
            break ;

            case IDOK:
            {
               char szUserID[MAX_PASSWORD+1] ;
               char szCur[MAX_PASSWORD+1] ;
               char szNew1[MAX_PASSWORD+1] ;
               char szNew2[MAX_PASSWORD+1] ;

               if (lpDD->lpszUserID[0] == '\0')
               {
                  Edit_GetText( GetDlgItem( hDlg, IDD_USERID ), szUserID, MAX_PASSWORD+1 ) ;

                  if (lstrlen( szUserID ) < MAX_PASSWORD)
                  {
                     ErrorResBox( hDlg, NULL, MB_ICONINFORMATION, IDS_APPNAME,
                                 IDS_ERR_USERIDTOOSHORT ) ;
                     break ;
                  }
               }

               Edit_GetText( GetDlgItem( hDlg, IDD_CURPASSWORD ), szCur, MAX_PASSWORD+1 ) ;

               if (0 != lstrcmpi( szCur, lpDD->lpszOldPassword ))
               {
                  ErrorResBox( hDlg, NULL, MB_ICONINFORMATION, IDS_APPNAME,
                              IDS_ERR_INVALIDPASSWORD ) ;
                  break ;
               }

               Edit_GetText( GetDlgItem( hDlg, IDD_NEWPASSWORD1 ), szNew1, MAX_PASSWORD+1 ) ;
               Edit_GetText( GetDlgItem( hDlg, IDD_NEWPASSWORD2 ), szNew2, MAX_PASSWORD+1 ) ;

               if (0 != lstrcmpi( szNew1, szNew2 ))
               {
                  ErrorResBox( hDlg, NULL, MB_ICONINFORMATION, IDS_APPNAME,
                              IDS_ERR_NEWPASSWORDSDIFFER ) ;
                  break ;
               }

               if (lpDD->lpszUserID[0] == '\0')
                  lstrcpy( lpDD->lpszUserID, szUserID ) ;

               lstrcpy( lpDD->lpszNewPassword, szNew1 ) ;
            }
            // !!! FALL THROUGH !!!

            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break;

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpDD->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_CHANGEPASSWORD ) );
            break;
         }
      }
      break ;

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      break ;

      default:
         return FALSE ;
   }

   return TRUE ;

} /* fnUserIDNewPasswordDlg()  */

/****************************************************************
 * BOOL WINAPI GetUserIDPasswordDlg( )
 *
 *  Description: 
 *
 *    Get the user's ID and verifies his password.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI GetUserIDPasswordDlg( HWND hwndParent, LPSTR szHelpFile,
                                  LPSTR szUserID, LPSTR szOldPassword )
{
   USERIDDLGDATA dlg ;

   dlg.lpszUserID = szUserID ;
   dlg.lpszOldPassword = szOldPassword ;
   dlg.lpszHelpFile = szHelpFile ;

   return (IDOK == DialogBoxParam( hmodDLL, MAKEINTRESOURCE( DLG_USERID ),
                          hwndParent, fnUserIDPasswordDlg,
                          (LPARAM)((LPVOID)&dlg) )) ;

} /* GetNewPasswordDlg()  */

/****************************************************************
 *  BOOL CALLBACK fnUserIDPasswordDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnUserIDPasswordDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   LPUSERIDDLGDATA lpDD ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         lpDD = (LPUSERIDDLGDATA)lParam ;
         SET_DLGDATAPROP( hDlg, lpDD ) ;

         Edit_LimitText( GetDlgItem( hDlg, IDD_USERID ), MAX_PASSWORD ) ;
         Edit_LimitText( GetDlgItem( hDlg, IDD_CURPASSWORD ), MAX_PASSWORD ) ;

         if (lpDD->lpszUserID[0] != '\0')
         {
            Edit_SetText( GetDlgItem( hDlg, IDD_USERID ), lpDD->lpszUserID ) ;
            EnableWindow( GetDlgItem( hDlg, IDD_USERID ), FALSE ) ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         lpDD = (LPUSERIDDLGDATA)GET_DLGDATAPROP( hDlg ) ;

         switch( idItem )
         {
            case IDOK:
            {
               char szUserID[MAX_PASSWORD+1] ;
               char szPassword[MAX_PASSWORD+1] ;

               if (lpDD->lpszUserID[0] == '\0')
               {
                  Edit_GetText( GetDlgItem( hDlg, IDD_USERID ), szUserID, MAX_PASSWORD+1 ) ;

                  if (lstrlen( szUserID ) < MAX_PASSWORD)
                  {
                     ErrorResBox( hDlg, NULL, MB_ICONINFORMATION, IDS_APPNAME,
                                 IDS_ERR_USERIDTOOSHORT ) ;
                     break ;
                  }
               }

               Edit_GetText( GetDlgItem( hDlg, IDD_CURPASSWORD ), szPassword, MAX_PASSWORD+1 ) ;

               if (lpDD->lpszOldPassword[0] == '\0' ||
                   (0 != lstrcmpi( szPassword, lpDD->lpszOldPassword )))
               {
                  ErrorResBox( hDlg, NULL, MB_ICONINFORMATION, IDS_APPNAME,
                              IDS_ERR_INVALIDPASSWORD ) ;
                  break ;
               }

               if (lpDD->lpszUserID[0] == '\0')
                  lstrcpy( lpDD->lpszUserID, szUserID ) ;
            }
            // !!! FALL THROUGH !!!

            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break ;

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpDD->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_USERID ) );
            break;
         }
      }
      break ;

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      break ;

      default:
         return FALSE ;
   }

   return TRUE ;

} /* fnUserIDPasswordDlg()  */

typedef struct tagVERIFYDLGDATA
{
   LPCUSTINFO lpCI ;
   BOOL       fVerifyShipping ;
   LPSTR      lpszHelpFile ;
   LPSTR      lpszAppName ;

} VERIFYDLGDATA, *PVERIFYDLGDATA, FAR *LPVERIFYDLGDATA ;


/****************************************************************
 * BOOL WINAPI VerifyOrderInfoDlg( )
 *
 *  Description:
 *
 *    Pops up a dialog box that askes the question "Here's the
 *    current billing and shipping information, is this correct?"
 *
 *    The dialog box can come up in one of two ways.  if the
 *    fVerifyShipping parameter is TRUE then the dialog box comes
 *    up with the lower half containting shipping info.  Otherwise
 *    the lower half is hidden and only billing info is shown.
 *
 ****************************************************************/
BOOL WINAPI VerifyOrderInfoDlg( HWND hwndParent, LPSTR szHelpFile, LPSTR szAppName, LPCUSTINFO lpCustInfo, BOOL fVerifyShipping )
{
   VERIFYDLGDATA vdd ;

   vdd.lpCI = lpCustInfo ;
   vdd.fVerifyShipping = fVerifyShipping ;
   vdd.lpszHelpFile = szHelpFile ;
   vdd.lpszAppName = szAppName ;

   return (IDOK == DialogBoxParam( hmodDLL, MAKEINTRESOURCE( DLG_ORDERINFO ),
                          hwndParent, fnVerifyOrderInfoDlg,
                          (LPARAM)(LPSTR)&vdd ));

} /* VerifyOrderInfoDlg()  */

static VOID NEAR PASCAL SetupBillngMethod( HWND hwndItem, UINT uiWhichCard ) ;
static VOID NEAR PASCAL SetupShippingMethod( HWND hwndCtl, UINT uiShipMethod ) ;
static VOID NEAR PASCAL SetupAddress( HWND hDlg, LPADDRESS lpAddr ) ;

/****************************************************************
 *  BOOL CALLBACK fnVerifyOrderInfoDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnVerifyOrderInfoDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   static char szExpDateFmt[128] ;
   char szBuf[256] ;
   LPVERIFYDLGDATA lpVDD ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         lpVDD = (LPVERIFYDLGDATA)lParam ;
         SET_DLGDATAPROP( hDlg, lpVDD ) ;

         SetupBillngMethod( GetDlgItem( hDlg, IDD_BILLINGMETHOD ), lpVDD->lpCI->uiWhichCard ) ;
         Static_SetText( GetDlgItem( hDlg, IDD_ACCOUNTNUMBER ), lpVDD->lpCI->CreditCard.szNumber ) ;

         Static_GetText( GetDlgItem( hDlg, IDD_EXPDATE ), szExpDateFmt, 128 ) ;
         wsprintf( szBuf, szExpDateFmt, lpVDD->lpCI->CreditCard.wExpMonth, lpVDD->lpCI->CreditCard.wExpYear ) ;
         Static_SetText( GetDlgItem( hDlg, IDD_EXPDATE ), szBuf ) ;

         /*
          * If fVerifyShipping == FALSE then shrink the dialog box
          */
         if (lpVDD->fVerifyShipping == FALSE)
         {
            RECT  rcItem ;
            RECT  rcDlg ;

            GetWindowRect( hDlg, &rcDlg ) ;

            GetWindowRect( GetDlgItem( hDlg, IDD_SHIPPING_GROUP ), &rcItem ) ;
            MapWindowRect( GetDlgItem( hDlg, IDD_SHIPPING_GROUP ), NULL, &rcItem ) ;

            SetWindowPos( hDlg, NULL, 0, 0, rcDlg.right - rcDlg.left,
                          (rcDlg.bottom - rcDlg.top) -
                          (rcItem.bottom - rcItem.top) - 6, 
                          SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER ) ;

            EnableWindow( GetDlgItem( hDlg, IDD_CUSTADDRESS ), FALSE ) ;

         }
         else
         {
            if (lpVDD->lpCI->uiWhichAddress == ADDRESS_CUSTOMER)
               SetupAddress( hDlg, &lpVDD->lpCI->CustAddress ) ;
            else
               SetupAddress( hDlg, &lpVDD->lpCI->ShipAddress ) ;

            SetupShippingMethod( GetDlgItem( hDlg, IDD_SHIPPINGMETHOD ), lpVDD->lpCI->uiShipMethod ) ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

         SetFocus( GetDlgItem( hDlg, IDOK ) ) ;
         return FALSE ;
      }
      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         lpVDD = (LPVERIFYDLGDATA)GET_DLGDATAPROP( hDlg ) ;

         switch( idItem )
         {
            case IDD_CUSTADDRESS:
               if (ShippingInfoDialog( hDlg, lpVDD->lpszHelpFile, GRCS(IDS_INIFILE), 1 ))
               {
                  GetCustInfo( lpVDD->lpCI, lpVDD->lpszAppName ) ;

                  if (lpVDD->lpCI->uiWhichAddress == ADDRESS_CUSTOMER)
                     SetupAddress( hDlg, &lpVDD->lpCI->CustAddress ) ;
                  else
                     SetupAddress( hDlg, &lpVDD->lpCI->ShipAddress ) ;

                  SetupShippingMethod( GetDlgItem( hDlg, IDD_SHIPPINGMETHOD ), lpVDD->lpCI->uiShipMethod ) ;
               }
            break ;

            case IDD_BILLINGINFO:
               if (BillingInfoDialog( hDlg, lpVDD->lpszHelpFile, GRCS(IDS_INIFILE), 1 ))
               {
                  GetCustInfo( lpVDD->lpCI, lpVDD->lpszAppName ) ;

                  SetupBillngMethod( GetDlgItem( hDlg, IDD_BILLINGMETHOD ), lpVDD->lpCI->uiWhichCard ) ;
                  Static_SetText( GetDlgItem( hDlg, IDD_ACCOUNTNUMBER ), lpVDD->lpCI->CreditCard.szNumber ) ;
         
                  wsprintf( szBuf, szExpDateFmt, lpVDD->lpCI->CreditCard.wExpMonth, lpVDD->lpCI->CreditCard.wExpYear ) ;
                  Static_SetText( GetDlgItem( hDlg, IDD_EXPDATE ), szBuf ) ;
               }
            break ;

            case IDOK:
            {
            }
            // !!! FALL THROUGH !!!

            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break ;

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpVDD->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_VERIFYORDER ) );
            break;
         }
      }
      break ;

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      break ;

      default:
         return FALSE ;
   }

   return TRUE ;

} /* fnVerifyOrderInfoDlg()  */

static VOID NEAR PASCAL SetupBillngMethod( HWND hwndItem, UINT uiWhichCard )
{
   switch( uiWhichCard )
   {
      case MASTERCARD:
         Static_SetText( hwndItem, "Master Card" ) ;
      break ;

      case VISA:
         Static_SetText( hwndItem, "VISA" ) ;
      break ;

      case DISCOVER:
         Static_SetText( hwndItem, "Discover" ) ;
      break ;

      case AMERICAN_EXPRESS:
         Static_SetText( hwndItem, "American Express" ) ;
      break ;

      case CORPORATE_CARD:
         Static_SetText( hwndItem, "Corporate Account" ) ;
      break ;
   }
}

static VOID NEAR PASCAL SetupAddress( HWND hDlg, LPADDRESS lpAddr )
{
   char  szBuf[128] ;

   Static_SetText( GetDlgItem( hDlg, IDD_NAME ), lpAddr->szName ) ;
   Static_SetText( GetDlgItem( hDlg, IDD_COMPANYNAME ), lpAddr->szCompany ) ;
   Static_SetText( GetDlgItem( hDlg, IDD_STREET1 ), lpAddr->szStreet1 ) ;
   Static_SetText( GetDlgItem( hDlg, IDD_STREET2 ), lpAddr->szStreet2 ) ;
   Static_SetText( GetDlgItem( hDlg, IDD_CITY ), lpAddr->szCity ) ;
   Static_SetText( GetDlgItem( hDlg, IDD_STATE ), lpAddr->szState ) ;
   Static_SetText( GetDlgItem( hDlg, IDD_ZIP ), lpAddr->szPostalCode ) ;

   if (lpAddr->wCountry == COUNTRY_USA)
      Static_SetText( GetDlgItem( hDlg, IDD_COUNTRY ), "United States" ) ;
   else if (lpAddr->wCountry == COUNTRY_CANADA)
      Static_SetText( GetDlgItem( hDlg, IDD_COUNTRY ), "Canada" ) ;
   else
      Static_SetText( GetDlgItem( hDlg, IDD_COUNTRY ), lpAddr->szCountry ) ;

   if (lpAddr->Phone.szExt[0] != '\0')
      wsprintf( szBuf, "(%s) %s    Ext: %s", (LPSTR)lpAddr->Phone.szAreaCode,
                                          (LPSTR)lpAddr->Phone.szNumber,
                                          (LPSTR)lpAddr->Phone.szExt ) ;
   else
      wsprintf( szBuf, "(%s) %s", (LPSTR)lpAddr->Phone.szAreaCode,
                                          (LPSTR)lpAddr->Phone.szNumber ) ;

   Static_SetText( GetDlgItem( hDlg, IDD_PHONE ), szBuf ) ;

}

static VOID NEAR PASCAL SetupShippingMethod( HWND hwndCtl, UINT uiShipMethod )
{
   switch( uiShipMethod )
   {
      case SHIPMETHOD_1TO2DAY:
         Static_SetText( hwndCtl, "One to Two Day" ) ;
         break ;

      case SHIPMETHOD_2TO3DAY:
         Static_SetText( hwndCtl, "Two to Three Day" ) ;
         break ;

      case SHIPMETHOD_UPS    :
         Static_SetText( hwndCtl, "UPS" ) ;
         break ;
   }


}


/************************************************************************
 * End of File: miscdlgs.c
 ***********************************************************************/

