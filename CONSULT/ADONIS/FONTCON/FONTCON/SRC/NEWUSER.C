/************************************************************************
 *
 *     Project:  FontShopper
 *
 *      Module:  newuser.c
 *
 *     Remarks:  Handles all stuff dealing with a new user.
 *
 *   Revisions:  
 *    4/7/92      cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "global.h"
#include "newuser.h"

#include "purchase.h"   // macros for calculating price stuff

BOOL CALLBACK fnNewUserDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK fnGetUserID(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;
BOOL CALLBACK fnRenewDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;

/****************************************************************
 *  BOOL WINAPI SignUpNewUser( HWND hwndParent, LPCUSTINFO lpCI )
 *
 *  Description: 
 *
 *    This function does everything neccessary to get a new user
 *    signed up.  This includes logging on to generate a new UserID
 *    and getting the dude's money.
 *
 *    Returns: 
 *             0 - Cancel or abort.  Not connected.
 *             1 - Entered what appears to be a valid User ID
 *             2 - Bought a subscription.  Still connected.
 *             3 - Got a demo User ID. Still connected.
 *
 ****************************************************************/
static LPCUSTINFO lpCustInfo ;

UINT WINAPI SignUpNewUser( HWND hwndParent, LPCUSTINFO lpCI, BOOL fForce )
{
   FSORDER  Order ;
   BYTE     bResult ;
   char     szNumber[MAX_CARDNUM+1] ;
   char     szTempID[MAX_PASSWORD+1] ;
   UINT     uiSel ;
   DWORD    dwLibID ;

   lpCustInfo = lpCI ;

   if (fForce)
      uiSel = IDD_PURCHASE ;
   else
      /*
       * Have you already received a User ID and password from Adonis?
       */
      uiSel = GetUserID( hwndParent, lpCI ) ;

   switch( uiSel )
   {
      case IDCANCEL:
         return 0 ;

      case IDOK:
         return 1 ;
      break ;

      case IDD_PURCHASE:
      {
         /*
          * Bring up the NewUser dialog box...
          */

         DLGPROC  lpfnDlg ;
         UINT     uiRet ;
         BOOL     fHasUserID = (lpCustInfo->szUserID[0] != '\0') ;

         (FARPROC)lpfnDlg = MakeProcInstance( (FARPROC)fnNewUserDlg, ghInst ) ;

         uiRet = DialogBox( ghInst, MAKEINTRESOURCE( NEWUSER_DLG ), hwndParent, lpfnDlg );

         FreeProcInstance( (FARPROC)lpfnDlg );

         if (uiRet != IDOK)
            return 0 ;

         /*
          * Log on.
          */
         switch (BBSConnect( hwndParent ))
         {
            case 0:           // canceled or execuatbles updated
            return 0 ;

            case 1:           // a-okay
            break ;

            case 2:           // database was updated
               /* We can let him continue here because the local
                * database has nothing to do with subscriptions...
                */
            break ;
         }

         wsprintf( szTempID, "%08lX", gdwBundleID ) ;
         SendBBSMsg(  MSG_SET_USER_ID, szTempID, NULL ) ;

         /*
          * Process the order...
          */
         if (SendBBSMsg( MSG_BEGIN_SESSION, NULL, &bResult ))
         {
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_ACK:
            break ;

            case ASCII_EOT:
               /* Database Error: Database is DOWN.
                */
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_DATABASE_DOWN ) ;
            break ;

            case ASCII_NAK:
            default:
               /* Error:
                */
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
            break ;
         }

         /* If he's already got a User ID don't generate another, just
          * validate it.
          */
         if (!fHasUserID)
         {
            if (SendBBSMsg( MSG_SET_MEDIA_PREF, &(lpCustInfo->uiMediaType), NULL ))
            {
               BBSDisConnect() ;
               return 0 ;
            }

            dwLibID = FS_LIBID_NEW ;

            if (SendBBSMsg( MSG_NEW_GET_NEW_USER_ID, &dwLibID, gCustInfo.szUserID ))
            {
               BBSDisConnect() ;
               return 0 ;
            }

            DP1( hWDB, "New User's ID is '%s'", (LPSTR)gCustInfo.szUserID ) ;

            if (gCustInfo.szUserID[0] == '\0')
            {
               /* The bbs tells us that the new user ID request failed by
                * passing back a null string
                */
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONEXCLAMATION, IDS_APPNAME,
                            IDS_ERR_NEWUSER_NOTACCEPTED ) ;
               return 0 ;

            }
         }
         else
         {
            dwLibID = FS_LIBID_UPGRADE ;

            SendBBSMsg(  MSG_SET_USER_ID, gCustInfo.szUserID, NULL ) ;
         }

         #if 0 // done in BBSConnect()
         if (SendBBSMsg( MSG_SET_CUST_INFO, &gCustInfo.CustAddress, NULL ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }
         #endif

         /*
          * Build the order struct
          */
         memset( &Order,0,sizeof(FSORDER) ) ;

         Order.bProdType = FSHOP ;
         Order.dwVendorID = WS_VEND_ID ;

         Order.dwLibID = dwLibID ;

         Order.bCourierType = (BYTE)gCustInfo.uiShipMethod ;

         if (gCustInfo.uiWhichAddress == ADDRESS_CUSTOMER)
            Order.ShipAddress = gCustInfo.CustAddress ;
         else
            Order.ShipAddress = gCustInfo.ShipAddress ;

         lstrcpy( Order.szVersion, "EXE" );

         Order.dwQuantity = 1;

         if (gCustInfo.uiMediaType == 0)
            Order.bMediaType = FLOPPY12 ;
         else
            Order.bMediaType = DISK14 ;

         lstrcpy( Order.szCCName, gCustInfo.CreditCard.szNameOnCard ) ;

         StripSpaces( gCustInfo.CreditCard.szNumber, szNumber, sizeof(szNumber) ) ;
         lstrcpy( Order.szCCNum, szNumber ) ;

         wsprintf( Order.szCCExpDate, "%02d%02d",
                     gCustInfo.CreditCard.wExpMonth,
                     gCustInfo.CreditCard.wExpYear ) ;

         Order.wCCType = gCustInfo.uiWhichCard ;

         Order.dwPriceEach = gdwSubscriptPrice ;
         Order.dwTotal = Order.dwPriceEach ;

         Order.dwShipping = SHIPPING_PRICE ;
         Order.dwTotal += Order.dwShipping ;

         Order.dwTaxAmount = WA_TAXES( Order.dwTotal ) ;
         Order.dwTotal += Order.dwTaxAmount ;

         Order.dwSurcharge = AMEX_SURCHARGE( Order.dwTotal ) ;
         Order.dwTotal += Order.dwSurcharge ;

         if (SendBBSMsg( MSG_SET_ORDER, &Order, NULL ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         if (SendBBSMsg( MSG_QUERY_NEWUSER_OK, NULL, &bResult ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_ACK:
            break ;

            case ASCII_NAK:
               /*
                * IDS_ERR_CREDIT_NOT_ACCEPT
                */
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                               IDS_APPNAME, IDS_ERR_CREDIT_NOT_ACCEPT ) ;
               gCustInfo.szUserID[0] = '\0' ;
            return 0 ;

            case ASCII_EOT:
               /*
                * IDS_ERR_CONDITIONAL_ORDER
                *
                * ClipArt shopper allowed the user to submit the order
                * conditionally.
                */
               if (IDNO == ErrorResBox( hwndParent, NULL, MB_ICONQUESTION | MB_YESNO,
                                        IDS_APPNAME, IDS_ERR_CONDITIONAL_ORDER ))
               {
                  gCustInfo.szUserID[0] = '\0' ;
                  BBSDisConnect() ;
                  return 0 ;
               }
            break ;

            default:
               gCustInfo.szUserID[0] = '\0' ;
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
            return 0 ;

         }

         /*
          * All new users start with their password being the same
          * as their User ID
          */
         DP1( hWDB, "Setting New User's Password to '%s'", (LPSTR)gCustInfo.szUserID ) ;
         if (SendBBSMsg( MSG_SET_PASSWD, gCustInfo.szUserID, NULL ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         if (SendBBSMsg( MSG_COMMIT_SUBSCRIPTION, NULL, &bResult ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_ACK:
            break ;

            case ASCII_NAK:
               gCustInfo.szUserID[0] = '\0' ;
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                                  IDS_APPNAME, IDS_ERR_NEWUSER_NOTACCEPTED ) ;
            return 0 ;

            default:
               gCustInfo.szUserID[0] = '\0' ;
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
            return 0 ;
         }

         if (!fHasUserID)
         {
            gfDemoUser = FALSE ;
            WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                    GRCS( IDS_INI_STATUS ),
                                    42,
                                    GRCS( IDS_INIFILE ) ) ;

            SaveUserID( &gCustInfo, gCustInfo.szUserID, GRCS( IDS_CONFIG ) ) ;

            /* Save here, just incase next msgs fail.  Note that the
            * bbs doesn't really care what the password is...
            */
            SavePassword( &gCustInfo, gCustInfo.szUserID, GRCS( IDS_CONFIG ) ) ;

            if (SendBBSMsg( MSG_COMMIT_PASSWD, NULL, &bResult))
            {
               BBSDisConnect() ;
               return 0 ;
            }

            switch( bResult )
            {
               case ASCII_ACK:
                  gCustInfo.fValidPassword = TRUE ;
               break ;

               default:
                  BBSDisConnect() ;
                  ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                              IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
               return 0 ;
            }

            /*
             * Tell the guy to change his password...
             */
            MessageBeep( MB_ICONINFORMATION ) ;
            ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                        IDS_APPNAME, IDS_ERR_YOURPASSWORDIS,
                              (LPSTR)gCustInfo.szUserID ) ;
         }
         else
         {
            gfDemoUser = FALSE ;
            WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                    GRCS( IDS_INI_STATUS ),
                                    42,
                                    GRCS( IDS_INIFILE ) ) ;

            ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                        IDS_APPNAME, IDS_ERR_DEMOTOFULLOK,
                              (LPSTR)gCustInfo.szUserID ) ;
         }   

         return 2 ; // bought a subscription
      }
      break ;

      /* This is essentially the same as the case above, except that
       * we don't charge the guy anything and we use a different
       * Lib ID to "buy" this "free" user ID
       */
      case IDD_REQUESTDEMOID:
      {

         /* Tell him what it means to get a demo ID.
          */

         /*
          * Log on.
          */
         switch (BBSConnect( hwndParent ))
         {
            case 0:           // canceled or execuatbles updated
            return 0 ;

            case 1:           // a-okay
            break ;

            case 2:           // database was updated
               /* We can let him continue here because the local
                * database has nothing to do with subscriptions...
                */
            break ;
         }

         wsprintf( szTempID, "%08lX", gdwBundleID ) ;
         SendBBSMsg(  MSG_SET_USER_ID, szTempID, NULL ) ;

         /* Process the order...
          */
         if (SendBBSMsg( MSG_BEGIN_SESSION, NULL, &bResult ))
         {
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_ACK:
            break ;

            case ASCII_EOT:
               /* Database Error: Database is DOWN.
                */
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_DATABASE_DOWN ) ;
            break ;

            case ASCII_NAK:
            default:
               /* Error:
                */
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
            break ;
         }

         if (SendBBSMsg( MSG_GET_NEW_LIMITED_DEMO_USER_ID, NULL, gCustInfo.szUserID ))
         {
            BBSDisConnect() ;
            return 0 ;
         }

         DP1( hWDB, "Demo User's ID is '%s'", (LPSTR)gCustInfo.szUserID ) ;

         if (gCustInfo.szUserID[0] == '\0')
         {
            /* The bbs tells us that the new user ID request failed by
             * passing back a null string
             */
            BBSDisConnect() ;
            ErrorResBox( hwndParent, NULL, MB_ICONEXCLAMATION, IDS_APPNAME,
                         IDS_ERR_GENERAL_COMM ) ;
            return 0 ;

         }

         #if  0 // done in BBSConnect()
         if (SendBBSMsg( MSG_SET_CUST_INFO, &gCustInfo.CustAddress, NULL ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }
         #endif

         /*
          * Build the order struct
          */
         memset( &Order,0,sizeof(FSORDER) ) ;

         Order.bProdType = FSHOP ;
         Order.dwVendorID = WS_VEND_ID ;
         Order.dwLibID = FS_LIBID_LIMITED_DEMO ;
         Order.dwQuantity = 1;

         /* Leave the rest of the order struct NULL */

         if (SendBBSMsg( MSG_SET_ORDER, &Order, NULL ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         if (SendBBSMsg( MSG_QUERY_NEWUSER_OK, NULL, &bResult ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_NAK:
               /* IDS_ERR_CREDIT_NOT_ACCEPT (so what!?)
                */
               DP1( hWDB, "MSG_QUERY_NEWUSER_OK returned NAK!!!!!" ) ;

            break ;

            case ASCII_EOT:
               DP1( hWDB, "MSG_QUERY_NEWUSER_OK returned EOT!!!!!" ) ;
            break ;
         }

         /*
          * All new users start with their password being the same
          * as their User ID
          */
         DP1( hWDB, "Setting New User's Password to '%s'", (LPSTR)gCustInfo.szUserID ) ;
         if (SendBBSMsg( MSG_SET_PASSWD, gCustInfo.szUserID, NULL ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         if (SendBBSMsg( MSG_COMMIT_NEWUSER, NULL, &bResult ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_ACK:
            break ;

            case ASCII_NAK:
               gCustInfo.szUserID[0] = '\0' ;
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                                  IDS_APPNAME, IDS_ERR_NEWUSER_NOTACCEPTED ) ;
            return 0 ;

            default:
               gCustInfo.szUserID[0] = '\0' ;
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
            return 0 ;
         }

         gfDemoUser = TRUE ;
         WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                 GRCS( IDS_INI_STATUS ),
                                 0,
                                 GRCS( IDS_INIFILE ) ) ;
         SaveUserID( &gCustInfo, gCustInfo.szUserID, GRCS( IDS_CONFIG ) ) ;
         SavePassword( &gCustInfo, gCustInfo.szUserID, GRCS( IDS_CONFIG ) ) ;

         if (SendBBSMsg( MSG_COMMIT_PASSWD, NULL, &bResult))
         {
            BBSDisConnect() ;
            return 0 ;
         }

         switch( bResult )
         {
            case ASCII_ACK:
               gCustInfo.fValidPassword = TRUE ;
               SavePassword( &gCustInfo, gCustInfo.szUserID, GRCS( IDS_CONFIG ) ) ;
            break ;

            default:
            case ASCII_NAK:
               BBSDisConnect() ;
               ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
            break ;
         }

         /*
          * Tell the guy to change his password...
          */
         MessageBeep( MB_ICONINFORMATION ) ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                      IDS_APPNAME, IDS_ERR_YOURDEMOPASSWORDIS,
                            (LPSTR)gCustInfo.szUserID ) ;

         lstrcpy( gCustInfo.szPassword, gCustInfo.szUserID ) ;

         return 3 ;  // got a demo ID
      }
      break ;

   }

   return TRUE ;

} /* SignUpNewUser()  */


BOOL CALLBACK fnNewUserDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   switch (wMsg)
   {
      case WM_INITDIALOG:

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), ghfontSmall, FALSE ) ;

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         switch( idItem )
         {
            case IDD_SHIPPINGINFO:
               ShippingInfoDialog( hDlg, gszHelpFile,
                                    GRCS(IDS_INIFILE), NULL );
               GetCustInfo( &gCustInfo, GRCS(IDS_CONFIG) ) ;
               InvalidateRect( hDlg, NULL, TRUE ) ;
            break ;

            case IDD_BILLINGINFO:
               BillingInfoDialog( hDlg, gszHelpFile,
                                   GRCS(IDS_INIFILE), NULL );
               GetCustInfo( &gCustInfo, GRCS(IDS_CONFIG) ) ;
               InvalidateRect( hDlg, NULL, TRUE ) ;
            break ;

            case IDD_COMMSETUP:
               CommSettingsDialog( hDlg, gszHelpFile, 
                                   GRCS(IDS_INIFILE), NULL );
               GetCustInfo( &gCustInfo, GRCS(IDS_CONFIG) ) ;
               InvalidateRect( hDlg, NULL, TRUE ) ;
            break ;

            case IDOK:
            {
               if (!VerifyOrderInfoDlg( hDlg, gszHelpFile, GRCS(IDS_CONFIG), &gCustInfo, TRUE))
                  break ;
            }
            // !!! FALL THROUGH !!!

            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break ;

            case IDB_HELP:
                ViewFontshopHelp( IDS_HELP_NEWUSER );
            break ;
         }
      }
      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         LPSTR       lp;
         RECT        rc;
         int         nDP ;
         DWORD       dwPrice = gdwSubscriptPrice ;
         HFONT       hfntDlg = GetWindowFont( hDlg ) ;

         BeginPaint( hDlg, &ps );

         SetBkMode( ps.hdc, TRANSPARENT );
         SetBkColor( ps.hdc, GetSysColor(COLOR_WINDOW) ) ;
         SetTextColor( ps.hdc, GetSysColor(COLOR_WINDOWTEXT) ) ;

         GetClientRect( GetDlgItem(hDlg,IDS_SUBTOTAL), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_SUBTOTAL), hDlg, &rc);
                  
         //*** Location of the decimal point
         lp = GRCS( IDS_SAMPDOLLAR );
         nDP = rc.right + (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

         PaintDollarAmount( ps.hdc, nDP, rc.top, dwPrice );

         GetClientRect( GetDlgItem(hDlg,IDS_SHIPPING), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_SHIPPING), hDlg, &rc);

         PaintDollarAmount( ps.hdc, nDP, rc.top, 500 );
         dwPrice += 500 ;

         GetClientRect( GetDlgItem(hDlg,IDS_SURCHARGE), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_SURCHARGE), hDlg, &rc);

         if (gCustInfo.uiWhichCard==AMERICAN_EXPRESS)
         {
               PaintDollarAmount( ps.hdc, nDP, rc.top, 
                  AMEX_SURCHARGE(gdwSubscriptPrice));
               dwPrice += AMEX_SURCHARGE(gdwSubscriptPrice);
         }
         else
               PaintDollarAmount( ps.hdc, nDP, rc.top, 0L );

         GetClientRect( GetDlgItem(hDlg,IDS_TAX), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_TAX), hDlg, &rc);

         if (gCustInfo.uiWhichAddress == ADDRESS_CUSTOMER)
            lp = gCustInfo.CustAddress.szState ;
         else
            lp = gCustInfo.ShipAddress.szState ;

         if (!_fstricmp( lp, "WA" ))
         {
               PaintDollarAmount( ps.hdc, nDP, rc.top, 
                  WA_TAXES(dwPrice));
               dwPrice += WA_TAXES(dwPrice);
         }
         else
            PaintDollarAmount( ps.hdc, nDP, rc.top, 0L );

         GetClientRect( GetDlgItem(hDlg,IDS_TOTAL), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_TOTAL), hDlg, &rc);

         PaintDollarAmount( ps.hdc, nDP, rc.top, dwPrice );

         EndPaint( hDlg, &ps );
      }
      break ;

      default:
         return FALSE ;
   }

   return FALSE ;

} /* fnNewUserDlg()  */


/****************************************************************
 *  BOOL WINAPI RenewUser( VOID )
 *
 *  Description: 
 *
 *    Presents the RenewUser dialog box which is a variant of
 *    the SignUpNewUser dialog.
 *
 *    If the guy renews we return TRUE (and either stay logged on
 *    or log on.
 *
 *    Otherwise we return FALSE and are logged off.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI RenewUser( HWND hwndParent, LPCUSTINFO lpCI )
{
   FSORDER  Order ;
   BYTE     bResult ;
   char     szNumber[MAX_CARDNUM+1] ;
   DLGPROC  lpfnDlg ;
   UINT     uiRet ;

   lpCustInfo = lpCI ;

   /* Bring up the Renew dialog box...
    */
   MessageBeep( MB_ICONEXCLAMATION ) ;
   (FARPROC)lpfnDlg = MakeProcInstance( (FARPROC)fnRenewDlg, ghInst ) ;
   uiRet = DialogBox( ghInst, MAKEINTRESOURCE( RENEWUSER_DLG ),
                     hwndParent, lpfnDlg );
   FreeProcInstance( (FARPROC)lpfnDlg );

   if (uiRet != IDOK)
      return 0 ;

   if (!IsBBSConnected( ))
   {
      /*
      * Log on.
      */
      switch (BBSConnect( hwndParent ))
      {
         case 0:           // canceled or execuatbles updated
         return 0 ;

         case 1:           // a-okay
         break ;

         case 2:           // database was updated
            /* We can let him continue here because the local
            * database has nothing to do with subscriptions...
            */
         break ;
      }
   }

   /*
    * Process the order...
    */
   if (SendBBSMsg( MSG_BEGIN_SESSION, NULL, &bResult ))
   {
      BBSDisConnect() ;
      return 0 ;
   }

   switch( bResult )
   {
      case ASCII_ACK:
      break ;

      case ASCII_EOT:
         /* Database Error: Database is DOWN.
            */
         BBSDisConnect() ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_DATABASE_DOWN ) ;
      break ;

      case ASCII_NAK:
      default:
         /* Error:
            */
         BBSDisConnect() ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      break ;
   }

   SendBBSMsg(  MSG_SET_USER_ID, gCustInfo.szUserID, NULL ) ;

   /*
    * Build the order struct
    */
   memset( &Order,0,sizeof(FSORDER) ) ;

   Order.bProdType = FSHOP ;
   Order.dwVendorID = WS_VEND_ID ;

   Order.dwLibID = FS_LIBID_RENEWAL ;

   Order.bCourierType = (BYTE)gCustInfo.uiShipMethod ;

   if (gCustInfo.uiWhichAddress == ADDRESS_CUSTOMER)
      Order.ShipAddress = gCustInfo.CustAddress ;
   else
      Order.ShipAddress = gCustInfo.ShipAddress ;

   lstrcpy( Order.szVersion, "EXE" );

   Order.dwQuantity = 1;

   if (gCustInfo.uiMediaType == 0)
      Order.bMediaType = FLOPPY12 ;
   else
      Order.bMediaType = DISK14 ;

   lstrcpy( Order.szCCName, gCustInfo.CreditCard.szNameOnCard ) ;

   StripSpaces( gCustInfo.CreditCard.szNumber, szNumber, sizeof(szNumber) ) ;
   lstrcpy( Order.szCCNum, szNumber ) ;

   wsprintf( Order.szCCExpDate, "%02d%02d",
               gCustInfo.CreditCard.wExpMonth,
               gCustInfo.CreditCard.wExpYear ) ;

   Order.wCCType = gCustInfo.uiWhichCard ;

   Order.dwPriceEach = gdwRenewalPrice ;
   Order.dwTotal = Order.dwPriceEach ;

   Order.dwTaxAmount = WA_TAXES( Order.dwTotal ) ;
   Order.dwTotal += Order.dwTaxAmount ;

   Order.dwSurcharge = AMEX_SURCHARGE( Order.dwTotal ) ;
   Order.dwTotal += Order.dwSurcharge ;

   if (SendBBSMsg( MSG_SET_ORDER, &Order, NULL ))
   {
      gCustInfo.szUserID[0] = '\0' ;
      BBSDisConnect() ;
      return 0 ;
   }

   if (SendBBSMsg( MSG_QUERY_RENEWAL_OK, NULL, &bResult ))
   {
      gCustInfo.szUserID[0] = '\0' ;
      BBSDisConnect() ;
      return 0 ;
   }

   switch( bResult )
   {
      case ASCII_ACK:
      break ;

      case ASCII_NAK:
         /*
          * IDS_ERR_CREDIT_NOT_ACCEPT
          */
         BBSDisConnect() ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                           IDS_APPNAME, IDS_ERR_CREDIT_NOT_ACCEPT ) ;
         gCustInfo.szUserID[0] = '\0' ;
      return 0 ;

      case ASCII_EOT:
         /*
          * IDS_ERR_CONDITIONAL_ORDER
          *
          * ClipArt shopper allowed the user to submit the order
          * conditionally.
          */
         if (IDNO == ErrorResBox( hwndParent, NULL, MB_ICONQUESTION | MB_YESNO,
                                    IDS_APPNAME, IDS_ERR_CONDITIONAL_ORDER ))
         {
            gCustInfo.szUserID[0] = '\0' ;
            BBSDisConnect() ;
            return 0 ;
         }
      break ;

      default:
         gCustInfo.szUserID[0] = '\0' ;
         BBSDisConnect() ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      return 0 ;

   }

   if (SendBBSMsg( MSG_COMMIT_SUBSCRIPTION, NULL, &bResult ))
   {
      gCustInfo.szUserID[0] = '\0' ;
      BBSDisConnect() ;
      return 0 ;
   }

   switch( bResult )
   {
      case ASCII_ACK:
      break ;

      case ASCII_NAK:
         gCustInfo.szUserID[0] = '\0' ;
         BBSDisConnect() ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                              IDS_APPNAME, IDS_ERR_RENEWAL_NOTACCEPTED ) ;
      return 0 ;

      default:
         gCustInfo.szUserID[0] = '\0' ;
         BBSDisConnect() ;
         ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      return 0 ;
   }


   gfExpiredUser = FALSE ;
   WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                           GRCS( IDS_INI_STATUS ),
                           42,
                           GRCS( IDS_INIFILE ) ) ;

   ErrorResBox( hwndParent, NULL, MB_ICONINFORMATION,
                        IDS_APPNAME, IDS_ERR_RENEWALOK ) ;

   return TRUE ;

   
} /* RenewUser()  */


BOOL CALLBACK fnRenewDlg(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), ghfontSmall, FALSE ) ;

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         switch( idItem )
         {
            case IDD_SHIPPINGINFO:
               ShippingInfoDialog( hDlg, gszHelpFile,
                                    GRCS(IDS_INIFILE), NULL );
               GetCustInfo( &gCustInfo, GRCS(IDS_CONFIG) ) ;
               InvalidateRect( hDlg, NULL, TRUE ) ;
            break ;

            case IDD_BILLINGINFO:
               BillingInfoDialog( hDlg, gszHelpFile,
                                   GRCS(IDS_INIFILE), NULL );
               GetCustInfo( &gCustInfo, GRCS(IDS_CONFIG) ) ;
               InvalidateRect( hDlg, NULL, TRUE ) ;
            break ;

            case IDOK:
            // !!! FALL THROUGH !!!

            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break ;

            case IDB_HELP:
                ViewFontshopHelp( IDS_HELP_RENEWAL );
            break ;
         }
      }
      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         LPSTR       lp;
         RECT        rc;
         int         nDP ;
         DWORD       dwPrice = gdwRenewalPrice ;
         HFONT       hfntDlg = GetWindowFont( hDlg ) ;

         BeginPaint( hDlg, &ps );

         SetBkMode( ps.hdc, TRANSPARENT );
         SetBkColor( ps.hdc, GetSysColor(COLOR_WINDOW) ) ;
         SetTextColor( ps.hdc, GetSysColor(COLOR_WINDOWTEXT) ) ;

         GetClientRect( GetDlgItem(hDlg,IDS_SUBTOTAL), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_SUBTOTAL), hDlg, &rc);
                  
         //*** Location of the decimal point
         lp = GRCS( IDS_SAMPDOLLAR );
         nDP = rc.right + (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

         PaintDollarAmount( ps.hdc, nDP, rc.top, dwPrice );

         GetClientRect( GetDlgItem(hDlg,IDS_SURCHARGE), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_SURCHARGE), hDlg, &rc);

         if (gCustInfo.uiWhichCard==AMERICAN_EXPRESS)
         {
               PaintDollarAmount( ps.hdc, nDP, rc.top, 
                  AMEX_SURCHARGE(gdwRenewalPrice));
               dwPrice += AMEX_SURCHARGE(gdwRenewalPrice);
         }
         else
               PaintDollarAmount( ps.hdc, nDP, rc.top, 0L );

         GetClientRect( GetDlgItem(hDlg,IDS_TAX), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_TAX), hDlg, &rc);

         if (gCustInfo.uiWhichAddress == ADDRESS_CUSTOMER)
            lp = gCustInfo.CustAddress.szState ;
         else
            lp = gCustInfo.ShipAddress.szState ;

         if (!_fstricmp( lp, "WA" ))
         {
               PaintDollarAmount( ps.hdc, nDP, rc.top, 
                  WA_TAXES(dwPrice));
               dwPrice += WA_TAXES(dwPrice);
         }
         else
            PaintDollarAmount( ps.hdc, nDP, rc.top, 0L );

         GetClientRect( GetDlgItem(hDlg,IDS_TOTAL), &rc );
         MapWindowRect(GetDlgItem(hDlg,IDS_TOTAL), hDlg, &rc);

         PaintDollarAmount( ps.hdc, nDP, rc.top, dwPrice );

         EndPaint( hDlg, &ps );
      }
      break ;

      default:
         return FALSE ;
   }

   return FALSE ;

} /* fnRenewDlg()  */


/****************************************************************
 *  UINT WINAPI GetUserID( HWND hwnd, LPCUSTINFO lpCI )
 *
 *  Description: 
 *
 *    This dialog box is where the user enters his User ID for
 *    the first time.  Once entered, it never needs to be entered
 *    again 'cause it's writen to the ini file.
 *
 ****************************************************************/
UINT WINAPI GetUserID( HWND hwnd, LPCUSTINFO lpCI )
{
   DLGPROC  lpfnDlg ;
   UINT     uiRet ;

   (FARPROC)lpfnDlg = MakeProcInstance( (FARPROC)fnGetUserID, ghInst ) ;
   uiRet = DialogBoxParam( ghInst, MAKEINTRESOURCE( NOUSERID_DLG ),
                     hwnd, lpfnDlg, (LPARAM)lpCI );
   FreeProcInstance( (FARPROC)lpfnDlg );

   return uiRet ;

} /* GetUserID()  */

/****************************************************************
 *  BOOL CALLBACK fnGetUserID(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnGetUserID(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   static LPCUSTINFO lpCI ;      // HACK!!! Only one instance!

   switch (wMsg)
   {
      case WM_INITDIALOG:

         lpCI = (LPCUSTINFO)lParam ;

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), ghfontSmall, FALSE ) ;
         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT2 ), ghfontSmall, FALSE ) ;

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

         Edit_LimitText( GetDlgItem( hDlg, IDD_USERID ), MAX_PASSWORD ) ;
         EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;
      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         switch( idItem )
         {
            case IDD_USERID:
               if (wNotifyCode == EN_CHANGE)
               {
                  char szBuf[MAX_PASSWORD+1] ;
                  PSTR p ;

                  if (GetWindowTextLength( hwndCtl ) == MAX_PASSWORD)
                  {
                     EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;

                     /* Assume it's ok, then make sure it's in hex
                      */
                     GetWindowText( hwndCtl, szBuf, MAX_PASSWORD+1 ) ;
                     for (p = szBuf ; *p ; p++)
                        if (!isxdigit( *p ))
                           EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;
                  }
                  else
                     EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;
               }
            break ;

            case IDOK:
            {
               char szBuf[MAX_PASSWORD+1] ;

               GetDlgItemText( hDlg, IDD_USERID, szBuf, MAX_PASSWORD+1 ) ;
               SaveUserID( lpCI, szBuf, GRCS( IDS_CONFIG ) ) ;
               SavePassword( lpCI, szBuf, GRCS( IDS_CONFIG ) ) ;

               /* Assume he's not a demo user.  Demo user's will never
                * enter a user id...
                */
               WritePrivateProfileWORD( GRCS( IDS_CONFIG ),
                                        GRCS( IDS_INI_STATUS ),
                                        42,
                                        GRCS( IDS_INIFILE ) ) ;
               gfDemoUser = FALSE ;
            }
            // !!! FALL THROUGH !!!

            case IDD_PURCHASE:
            case IDD_REQUESTDEMOID:
            case IDCANCEL:
               EndDialog( hDlg, wParam ) ;
            break ;

            case IDB_HELP:
                ViewFontshopHelp( IDS_HELP_NOUSERID );
            break ;
         }
      }
      break ;

      default:
         return FALSE ;
   }

   return FALSE ;

} /* fnGetUserID()  */


/************************************************************************
 * End of File: newuser.c
 ***********************************************************************/

