/************************************************************************
 *
 *     Project:  Font Connection 1.x
 *
 *      Module:  online.c
 *
 *     Remarks:  This module handles online transactions regarding
 *               purchasing.  It was pulled out of purchase.c because
 *               purchase.c was getting to big.  The functions in here
 *               are called from purchase.c
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include <lzexpand.h>
#include "global.h"
#include "purchase.h"
#include "newuser.h"
#include "bbs.h"

extern HLIST      hlstPurchased ;   // purchase.c
extern DWORD      gdwTotal ;     //purchase.c


static FSORDER    Order ;
static char       gszScratch[1024] ;
static char       gszTempPath[144] ;
static char       gszTTFFile[TTF_LENGTH] ;
static char       gszTT_File[TTF_LENGTH] ;

static UINT       fuEM ;
static OFSTRUCT   of ;

BOOL NEAR PASCAL SubmitSubOrder( HWND hDlg, LPDBNODE lpdn ) ;
BOOL NEAR PASCAL DownloadSubOrder( HWND hDlg, LPDBNODE lpdn ) ;
BOOL NEAR PASCAL CopyDownloadedFonts( HWND hDlg, LPDBNODE lpdn ) ;
BOOL NEAR PASCAL InstallDownloadedFonts( HWND hDlg, LPDBNODE lpdn ) ;
BOOL NEAR PASCAL GiveBBSTheOrder( HWND hDlg, LPDBNODE lpdn ) ;
HFILE NEAR PASCAL OpenLogfile( VOID ) ;
BOOL NEAR PASCAL DeleteDownloadedFonts( HWND hDlg, LPDBNODE lpdn ) ;


/****************************************************************
 *  BOOL WINAPI SubmitOrder( HWND hDlg )
 *
 *  Description: 
 *
 *
 *
 ****************************************************************/
BOOL WINAPI SubmitOrder( HWND hDlg )
{
   BYTE     bResult ;
   LPDBNODE lpdn;
   char     szNumber[MAX_CARDNUM+1] ;

   if (!VerifyDestDir( hDlg, gszDestPath ))
      return FALSE ;

   lpdn = GetFirstPurchasedNode( glpDB );

   GetTempFileName(0, "fs", 0, gszTempPath ) ;
   fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
   OpenFile( gszTempPath, &of, OF_DELETE ) ;
   SetErrorMode( fuEM ) ;
   *(strrchr( gszTempPath, '\\' )+1) = '\0' ;

   /* Setup Order structure with defaults
    */
   memset( &Order,0,sizeof(FSORDER) ) ;
   GetCustInfo( &gCustInfo, GRCS( IDS_APPNAME ) ) ;

   Order.bProdType = FSHOP ;
   Order.dwVendorID = WS_VEND_ID ;

   Order.dwLibID = 0 ;

   if (gCustInfo.uiWhichAddress == ADDRESS_CUSTOMER)
      Order.ShipAddress = gCustInfo.CustAddress ;
   else
      Order.ShipAddress = gCustInfo.ShipAddress ;

   lstrcpy( Order.szVersion, "TTF" );

   lstrcpy( Order.szCCName, gCustInfo.CreditCard.szNameOnCard ) ;

   StripSpaces( gCustInfo.CreditCard.szNumber, szNumber, sizeof(szNumber) ) ;
   lstrcpy( Order.szCCNum, szNumber ) ;

   wsprintf( Order.szCCExpDate, "%02d%02d",
               gCustInfo.CreditCard.wExpMonth,
               gCustInfo.CreditCard.wExpYear ) ;

   Order.wCCType = gCustInfo.uiWhichCard ;

   if (gCustInfo.szUserID[0] == '\0')
   {
      /*
       * This guy has no UserID.  He's a new user...
       */
      if (!SignUpNewUser( hDlg, &gCustInfo, FALSE ))
         return FALSE ;

      /*
       * Sign up new user might have actually left us logged
       * on.  Notice that below we check if we're connected or not
       */
   }

   if (!IsBBSConnected() && (gCustInfo.fValidPassword == FALSE))
   {
      if (!GetUserIDPasswordDlg( hDlg, gszHelpFile, gCustInfo.szUserID, gCustInfo.szPassword ))
         return FALSE ;

      gCustInfo.fValidPassword = TRUE ;
   }

   /* Connect to bbs.
    */
   if (!IsBBSConnected())
   {
      switch (BBSConnect( hDlg ))
      {
         case 0:        // not connected
         return FALSE ;

         case 1:        // all's cool
         break ;

         case 2:        // database was updated
            BBSDisConnect() ;
            EndDialog( hDlg, FALSE ) ;
            return FALSE ;
         break ;
      }
   }

   /* Setup customer info structure and send it
    */
   #if 0 // this is done in BBSConnect()
   SendBBSMsg(  MSG_SET_CUST_INFO, &gCustInfo.CustAddress, NULL ) ;
   #endif

   SendBBSMsg(  MSG_SET_USER_ID, gCustInfo.szUserID, NULL ) ;

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
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_DATABASE_DOWN ) ;
      break ;

      case ASCII_NAK:
      default:
         /* Error:
            */
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      break ;
   }

   /* This function will return failure if the user's
    * account is expired and he does not try to renew. The
    * code there also logs off in this case.
    */
   if (-1 == CheckAccountStatus( hDlg ) )
      return FALSE ;

   SendBBSMsg( MSG_COMMIT_CUST_INFO, NULL, &bResult ) ;

   switch( bResult )
   {
      case ASCII_ACK:
      break ;
      
      case ASCII_NAK:
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                      IDS_APPNAME, IDS_ERR_CUSTINFO_REJECTED ) ;
      return FALSE ;

      default:
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      return FALSE ;
   }

   if (hlstPurchased)
   {
      fsmmListDestroy( hlstPurchased ) ;
      hlstPurchased = NULL ;
   }

   /* gdwTotal is used in the Post Purchase dialog box to tell
    * the user how much his credit card was charged.
    */
   gdwTotal = 0 ;

   /* Loop through each of the selected items.  Issue
    * a separate order for each.
    */
   while (lpdn)
   {
      /* If he's a demo user, reject any attempt to purchase
       * things that cost something.  Note that we should never
       * really have to worry about him getting this far, but
       * I'm just being careful.
       */
      if (gfDemoUser == TRUE && gfDemoDatabase == FALSE)
         switch (NODETYPE(lpdn))
         {
            case NT_COLLECTION:
               if (FONTPRICE( ((LPCOLL)lpdn) ))
               {
                  lpdn = GetNextPurchasedNode( glpDB, lpdn );
                  goto NextNode ;
               }
            break;

            case NT_FACE:
               if (FONTPRICE( ((LPFACE)lpdn) ))
               {
                  lpdn = GetNextPurchasedNode( glpDB, lpdn );
                  goto NextNode ;
               }
            break;

            case NT_STYLE:
               if (FONTPRICE( ((LPSTYLE)lpdn) ))
               {
                  lpdn = GetNextPurchasedNode( glpDB, lpdn );
                  goto NextNode ;
               }
            break;
         }

      if (!SubmitSubOrder( hDlg, lpdn ))
         return FALSE ;

      gdwTotal += Order.dwTotal ;

      /* Get the next purchase and remove this one.
       */
      switch (NODETYPE(lpdn))
      {
           case NT_STYLE:
               glpDB->dwTotal-= FONTPRICE(((LPSTYLE)lpdn));
               glpDB->dwSize -= FONTSIZE(((LPSTYLE)lpdn));
               glpDB->wFonts--;
           break;

           case NT_FACE:
           {
               LPSTYLE lpS = GetFirstStyle( (LPFACE)lpdn, NULL );

               glpDB->dwTotal -= FONTPRICE(((LPFACE)lpdn));
               glpDB->dwSize  -= FONTSIZE(((LPFACE)lpdn));
               glpDB->wFonts--;

               //*** When unchecking a FACE we need
               //*** to handle the children properly
               while (lpS)
               {
                   VI_UNCHECKNODE(lpS);
                   lpS = GetNextStyle( lpS, NULL );
               }
           }
           break;

           case NT_COLLECTION:
           {
               LPCOLL lpColl = (LPCOLL)lpdn;

               glpDB->dwTotal -= lpColl->Info.dwPrice;
               glpDB->wColls--;
           }
           break;
      }

      VI_UNCHECKNODE(lpdn);

NextNode:

      lpdn = GetNextPurchasedNode( glpDB, lpdn );
      FillPurchaseDialog( hDlg );
   }

   /*
    * Log off
    */
   BBSDisConnect() ;

   return TRUE ;

} /* SubmitOrder()  */

/****************************************************************
 *  BOOL NEAR PASCAL SubmitSubOrder( HWND hDlg, LPDBNODE lpdn )
 *
 *  Description: 
 *
 *
 *
 *
 ****************************************************************/
BOOL NEAR PASCAL SubmitSubOrder( HWND hDlg, LPDBNODE lpdn )
{
   if (GiveBBSTheOrder( hDlg, lpdn ))
   {
      /***********************************************************
       * From here on in the process the user has PAID for his
       * fonts.  We must be careful not to screw him.
       ***********************************************************/

      if (NODETYPE(lpdn) != NT_COLLECTION)
      {
         if (!CopyDownloadedFonts( hDlg, lpdn ))
            return TRUE ;

         /* Does the guy want his fonts installed for him?
          * If so do it here.  If we did it in DownloadSubOrder()
          * there would be the potential for someone to
          * get free font's by purchasing a 'face' of several
          * styles and disrupting the session after several of
          * the styles were already downloaded and installed.
          */
         if (gfInstallFonts == TRUE)
         {
            if (!InstallDownloadedFonts( hDlg, lpdn ))
               return FALSE ;
         }
      }
   }
   else
      return FALSE ;

   return TRUE ;

} /* SubmitSubOrder()  */


/****************************************************************
 *  BOOL NEAR PASCAL GiveBBSTheOrder( HWND hDlg, LPDBNODE lpdn )
 *
 *  Description: 
 *
 *    Handles setting up the order structure and passing it
 *    to the bbs.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL GiveBBSTheOrder( HWND hDlg, LPDBNODE lpdn )
{
   BYTE     bResult ;
   PURCHASEDNODE PN ;
   DWORD    dwOrderID = (DWORD)-1 ;
   HFILE    hfileLog ;
   PSTR     pszType ;
   char     szProduct[63] ;
   LPSTR    lpszPub ;
   LPSTR    lpszFilename ;
   BOOL     fFailed = FALSE ;

   switch (NODETYPE(lpdn))
   {
      case NT_COLLECTION:
      {
         DWORD  dwPrice = FONTPRICE( ((LPCOLL)lpdn) ) ;
         int    i = (int)((LPCOLL)lpdn)->wShipping;

         if (i == -1)
            i = (BYTE)gCustInfo.uiShipMethod ;

         Order.dwQuantity = 1;

         Order.bCourierType = (BYTE)i ;
         Order.dwShipping = ((LPCOLL)lpdn)->Info.rgShippingRates[i] ;
         Order.dwPriceEach = Order.dwTotal = dwPrice + Order.dwShipping ;

         if (gCustInfo.uiMediaType == 0)
            Order.bMediaType = FLOPPY12 ;
         else
            Order.bMediaType = DISK14 ;

         Order.bProdType = FONT_COLLECTION ;
         Order.dwID = 0 ;
         Order.dwLibID = NODEID( ((LPCOLL)lpdn) ) ;

         Order.dwPubID = PARENT( LPPUB, lpdn )->fiFSPub ;

         DP1( hWDB, "*** Purchasing Font Collection ID 0x%08lX (PubID = 0x%08lX)",
              Order.dwID, Order.dwPubID ) ;

         pszType = "Col" ;
         lpszPub = PARENT( LPPUB, lpdn )->lpszPublisher ;
         lpszFilename = "n/a" ;
         lstrcpy( szProduct, NODENAME( ((LPCOLL)lpdn) ) ) ;
      }
      break;

      case NT_FACE:
      {
         DWORD  dwPrice = FONTPRICE( ((LPFACE)lpdn) ) ;

         Order.bMediaType = 0 ;
         Order.dwQuantity = 1;
         Order.bCourierType = 0 ;
         Order.dwShipping = 0 ;
         Order.dwPriceEach = Order.dwTotal = dwPrice ;

         Order.bProdType = FONT_FACE ;
         Order.dwID = NODEID( ((LPFACE)lpdn) ) ;
         Order.dwLibID = 0 ;
         Order.dwPubID = PARENT( LPPUB, PARENT( LPFAMILY, lpdn ) )->fiFSPub ;

         DP1( hWDB, "*** Purchasing Font Face ID 0x%08lX (PubID = 0x%08lX)",
              Order.dwID, Order.dwPubID ) ;

         pszType = "Fac" ;
         lpszPub = PARENT( LPPUB, PARENT( LPFAMILY, lpdn ) )->lpszPublisher ;
         lpszFilename = "n/a" ;
         lstrcpy( szProduct, NODENAME( ((LPFACE)lpdn) ) ) ;
      }
      break;

      case NT_STYLE:
      {
         DWORD  dwPrice = FONTPRICE( ((LPSTYLE)lpdn) ) ;

         Order.bMediaType = 0 ;
         Order.dwQuantity = 1;
         Order.bCourierType = 0 ;
         Order.dwShipping = 0 ;
         Order.dwPriceEach = Order.dwTotal = dwPrice ;

         Order.bProdType = FONT_STYLE ;
         Order.dwID = NODEID( ((LPSTYLE)lpdn) ) ;
         Order.dwLibID = 0 ;
         Order.dwPubID = PARENT( LPPUB, PARENT( LPFAMILY, PARENT( LPSTYLE, lpdn ) ) )->fiFSPub ;

         DP1( hWDB, "*** Purchasing Font Style ID 0x%08lX (PubID = 0x%08lX)",
              Order.dwID, Order.dwPubID ) ;

         pszType = "Sty" ;
         lpszPub = PARENT( LPPUB, PARENT( LPFAMILY, PARENT( LPSTYLE, lpdn ) ) )->lpszPublisher ;
         lpszFilename = ((LPSTYLE)lpdn)->Info.szTTFFile ;
         wsprintf( szProduct, "%s %s",
                     (LPSTR)NODENAME( PARENT( LPFACE, lpdn ) ),
                     (LPSTR)NODENAME( ((LPSTYLE)lpdn) ) ) ;
      }
      break;
   }

   if (gCustInfo.uiWhichCard==AMERICAN_EXPRESS)
   {
       Order.dwSurcharge = AMEX_SURCHARGE(Order.dwTotal) ;
       Order.dwTotal += AMEX_SURCHARGE(Order.dwTotal);
   }

   if (!lstrcmpi(Order.ShipAddress.szState, "WA"))
   {
      Order.dwTaxAmount = WA_TAXES( Order.dwTotal ) ;
      Order.dwTotal += WA_TAXES( Order.dwTotal ) ;
   }

   /* Send order to bbs.
    */
   if (SendBBSMsg( MSG_SET_ORDER, &Order, NULL ))
   {
      BBSDisConnect() ;
      return FALSE ;
   }

   /* Query to see if it's a valid order
    */
   if (NODETYPE(lpdn) == NT_COLLECTION)
   {
      if (SendBBSMsg( MSG_QUERY_LIB_ORDER_OK, NULL, &bResult ))
      {
         BBSDisConnect() ;
         return FALSE ;
      }
   }
   else
   {
      if (SendBBSMsg( MSG_QUERY_CLIP_DLD_OK, NULL, &bResult ))
      {
         BBSDisConnect() ;
         return FALSE ;
      }
   }

   switch( bResult )
   {
      case ASCII_ACK:
      break ;

#if 0
      case ASCII_EOT:
         /*
         * IDS_ERR_CONDITIONAL_ORDER
         *
         * ClipArt shopper allowed the user to submit the order
         * conditionally.
         */
         if (IDNO == ErrorResBox( hDlg, NULL, MB_ICONQUESTION | MB_YESNO,
                                 IDS_APPNAME, IDS_ERR_CONDITIONAL_ORDER ))
         {
            BBSDisConnect() ;
            return FALSE ;
         }
      break ;
#endif

      case ASCII_EOT:
      case ASCII_NAK:
         /*
         * IDS_ERR_CREDIT_NOT_ACCEPT
         */
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                        IDS_APPNAME, IDS_ERR_CREDIT_NOT_ACCEPT ) ;
      return FALSE ;

      default:
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      return FALSE ;

   }

   if (NODETYPE(lpdn) == NT_COLLECTION)
   {
      if (SendBBSMsg( MSG_COMMIT_LIB_ORDER, NULL, &bResult ))
      {
         DP1( hWDB, "COMMIT_LIB_ORDER failed!" ) ;
         BBSDisConnect() ;
         return FALSE ;
      }
   }
   else
   {
      /* Begin download for this sub order.  We download
       * to the TEMP directory.
       *
       */
      if (!DownloadSubOrder( hDlg, lpdn ))
      {
         BBSDisConnect() ;
         return FALSE ;
      }

      /* Commit the order (i.e. charge the dude's
       * credit card).
       */
      if (SendBBSMsg( MSG_COMMIT_CLIP_DLD, NULL, &bResult ))
      {
         DeleteDownloadedFonts( hDlg, lpdn ) ;
         BBSDisConnect() ;
         return FALSE ;
      }

   }

   switch( bResult )
   {
      case ASCII_ACK:
         /* get the order ID.  Tell the user what the order
          * id is somehow
          */
         fFailed = SendBBSMsg( MSG_QUERY_ORDERID, NULL, &dwOrderID ) ;
      break ;

      case ASCII_NAK:
         /* order was rejected */
         switch (NODETYPE(lpdn))
         {
            case NT_FACE:
               DeleteDownloadedFonts( hDlg, lpdn ) ;
               BBSDisConnect() ;
               ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ORDER_REJECTED,
                     (LPSTR)NODENAME( ((LPFACE)lpdn) ) ) ;
            break;

            case NT_STYLE:
               DeleteDownloadedFonts( hDlg, lpdn ) ;
               BBSDisConnect() ;
               ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ORDER_REJECTED,
                     (LPSTR)NODENAME( ((LPSTYLE)lpdn) ) ) ;
            break;

            case NT_COLLECTION:
               BBSDisConnect() ;
               ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_ORDER_REJECTED,
                     (LPSTR)NODENAME( ((LPCOLL)lpdn) ) ) ;
            break;
         }
      return FALSE ;

      default:
         DeleteDownloadedFonts( hDlg, lpdn ) ;
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
      return FALSE ;
   }

   /* Write to the log file
    */
   if (HFILE_ERROR != (hfileLog = OpenLogfile()))
   {
      LPSTYLE lpdnStyle ;
      char    szDate[16] ;
      char    szTime[16] ;

      wsprintf( gszScratch, GRCS( IDS_LOG_FMT2 ),
                  (DWORD)dwOrderID,

                  (LPSTR)TimeGetCurDate( szDate, DATE_SHORTDATE ),
                  (LPSTR)TimeGetCurTime( szTime, TIME_24HOUR | TIME_NOSECONDS ),

                  (LPSTR)pszType,

                  (DWORD)Order.dwTotal/100L,   // price
                  (DWORD)Order.dwTotal%100L,

                  (DWORD)Order.dwID,  // id

                  (LPSTR)lpszFilename,

                  (LPSTR)szProduct,
                  (LPSTR)lpszPub,

                  (DWORD)Order.dwPriceEach/100L,   // price
                  (DWORD)Order.dwPriceEach%100L,

                  (DWORD)Order.dwShipping/100L,    // ship
                  (DWORD)Order.dwShipping%100L,
                  
                  (DWORD)Order.dwSurcharge/100L,   // amex
                  (DWORD)Order.dwSurcharge%100L,   
                  
                  (DWORD)Order.dwTaxAmount/100L,   // tax
                  (DWORD)Order.dwTaxAmount%100L    // tax
                  ) ;

      _lwrite( hfileLog, gszScratch, lstrlen( gszScratch ) ) ;

      if (NODETYPE( lpdn ) == NT_FACE)
      {
         lpdnStyle = GetFirstStyle( (LPFACE)lpdn, NULL ) ;

         while( lpdnStyle && NODETYPE(lpdnStyle) == NT_STYLE )
         {
            wsprintf( szProduct, "%s %s",
                        (LPSTR)NODENAME( ((LPFACE)lpdn) ),
                        (LPSTR)NODENAME( lpdnStyle ) ) ;

            wsprintf( gszScratch, GRCS( IDS_LOG_FMT3 ),
                        (LPSTR)lpdnStyle->Info.szTTFFile,
                        (LPSTR)szProduct,
                        (LPSTR)lpszPub ) ;

            _lwrite( hfileLog, gszScratch, lstrlen( gszScratch ) ) ;

            lpdnStyle = GetNextStyle( lpdnStyle, NULL ) ;
         }
      }
      _lclose( hfileLog ) ;
   }

   /* Add the pointer to the node to the purchased list
    */
   if (hlstPurchased == NULL)
      hlstPurchased = fsmmListCreate( NULL ) ;

   PN.lpdn = lpdn ;
   PN.dwOrderID = dwOrderID ;
   fsmmListAllocInsertNode( hlstPurchased, (LPSTR)&PN, sizeof( PN ), LIST_INSERTTAIL ) ;

   return !fFailed ; // fFailed will be TRUE if QUERY_ORDER_ID failed.

} /* GiveBBSTheOrder()  */


/****************************************************************
 *  BOOL NEAR PASCAL DownloadSubOrder( HWND hDlg, LPDBNODE lpdn )
 *
 * Download a sub order.  We download to the TEMP directory.
 *
 * If it's a font face that's being ordered then
 * there may be more than one file to download.
 * get them all.
 *
 * Returns TRUE if entire sub-order was downloaded correctly,
 * false otherwise.
 *
 * Upon a successful return all downloaded fonts will be
 * in the temp directory, compressed (szTT_File).  If FALSE
 * is returned then everything has been deleted.
 *
 ****************************************************************/
BOOL NEAR PASCAL DownloadSubOrder( HWND hDlg, LPDBNODE lpdn )
{
   LPSTYLE  lpdnStyle ;
   char     szFile[144] ;
   HFILE    hfile ;
   OFSTRUCT of ;

   if (NODETYPE( lpdn ) == NT_FACE)
      lpdnStyle = GetFirstStyle( (LPFACE)lpdn, NULL ) ;
   else
      lpdnStyle = (LPSTYLE)lpdn ;

   while( lpdnStyle && NODETYPE(lpdnStyle) == NT_STYLE )
   {
      lstrcpy( gszTT_File, lpdnStyle->Info.szTTFFile ) ;
      *(strchr( gszTT_File, '.' ) + 1) = '\0' ;
      strcat( gszTT_File, "TT_" ) ;

      DP1( hWDB, "   Downloading %s...", (LPSTR)gszTT_File ) ;

      SendBBSMsg( MSG_SET_CUR_FILENAME, gszTT_File, NULL ) ;

      if (SendBBSMsg( MSG_RECEIVE_FILE, NULL, gszTempPath ))
      {
         DP1( hWDB, "MSG_RECEIVE_FILE Failed!!! Cleaning up..." ) ;

         DeleteDownloadedFonts( hDlg, lpdn ) ;

         BBSDisConnect() ;

         return FALSE ;
      }

      /* Verify that the file has a non-zero file size */
      wsprintf( szFile, "%s%s", (LPSTR)gszTempPath, (LPSTR)gszTT_File ) ;
      if (HFILE_ERROR == (hfile = OpenFile( szFile, &of, OF_READ )))
      {
         DP1( hWDB, "Attempt to verify file exists failed!..." ) ;
         DeleteDownloadedFonts( hDlg, lpdn ) ;
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_FONTNOTONBBS, (LPSTR)gszTT_File ) ;

         return FALSE ;
      }

      if (0 == _filelength( hfile ))
      {
         DP1( hWDB, "File has zero size!!..." ) ;
         DeleteDownloadedFonts( hDlg, lpdn ) ;
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
               IDS_APPNAME, IDS_ERR_FONTNOTONBBS, (LPSTR)gszTT_File ) ;
         return FALSE ;
      }

      lclose( hfile ) ;

      if (NODETYPE(lpdn) == NT_FACE)
         lpdnStyle = GetNextStyle( lpdnStyle, NULL ) ;
      else
         lpdnStyle = NULL ;
   }

   return TRUE ;

} /* DownloadSubOrder()  */

/****************************************************************
 *  BOOL NEAR PASCAL DeleteDownloadedFonts( HWND hDlg, LPDBNODE lpdn )
 *
 *  Description: 
 *
 *    Deletes all files associated with a download, if the
 *    COMMIT fails.
 *
 *
 ****************************************************************/
BOOL NEAR PASCAL DeleteDownloadedFonts( HWND hDlg, LPDBNODE lpdn )
{
   LPSTYLE lpdnStyle ;

   fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;

   /* If any were already downloaded without committing the
    * order delete them!
    */
   if (NODETYPE( lpdn ) == NT_FACE)
      lpdnStyle = GetFirstStyle( (LPFACE)lpdn, NULL ) ;
   else
      lpdnStyle = (LPSTYLE)lpdn ;

   while( lpdnStyle && NODETYPE(lpdnStyle) == NT_STYLE )
   {
      wsprintf( gszScratch, "%s%s",
            (LPSTR)gszTempPath, (LPSTR)lpdnStyle->Info.szTTFFile ) ;

      DP1( hWDB, "      Deleting %s", (LPSTR)gszScratch ) ;
      OpenFile( gszScratch, &of, OF_DELETE ) ;

      if (NODETYPE(lpdn) == NT_FACE)
         lpdnStyle = GetNextStyle( lpdnStyle, NULL ) ;
      else
         lpdnStyle = NULL ;
   }

   SetErrorMode( fuEM ) ;

   return TRUE ;

} /* DeleteDownloadedFonts()  */

/****************************************************************
 *  BOOL NEAR PASCAL CopyDownloadedFonts( HWND hDlg, LPDBNODE lpdn )
 *
 *  Description: 
 *
 *  Installs the *.TTF files which were previously copied to the
 *  gszDestPath sub directory associated with the 
 *  sub order (identified by lpdn).
 *
 *  
 *
 *  When this function is called, the guy has already paid for
 *  the fonts so we must be very careful...
 *
 ****************************************************************/
BOOL NEAR PASCAL CopyDownloadedFonts( HWND hDlg, LPDBNODE lpdn )
{
   LPSTYLE  lpdnStyle ;
   OFSTRUCT ofSrc ;
   OFSTRUCT ofDst ;
   HFILE    hfSrcFile ;
   HFILE    hfDstFile ;
   BOOL     fSuccess = TRUE ;

   if (NODETYPE( lpdn ) == NT_FACE)
      lpdnStyle = GetFirstStyle( (LPFACE)lpdn, NULL ) ;
   else
      lpdnStyle = (LPSTYLE)lpdn ;

   while( lpdnStyle && NODETYPE(lpdnStyle) == NT_STYLE )
   {
      DP1( hWDB, "Copying Downloaded Fonts..." ) ;
      lstrcpy( gszTTFFile, lpdnStyle->Info.szTTFFile ) ;
      lstrcpy( gszTT_File, gszTTFFile ) ;
      *(strchr( gszTT_File, '.' ) + 1) = '\0' ;
      strcat( gszTT_File, "TT_" ) ;
   
      /* The file is compressed and has the extention
       * .TT_.  LZexpand() it to .TTF and delete the
       * .TT_.
       */
      wsprintf( gszScratch, "%s%s", (LPSTR)gszTempPath, (LPSTR)gszTT_File ) ;
      DP1( hWDB, "   Opening %s as source...", (LPSTR)gszScratch ) ;

      hfSrcFile = LZOpenFile( gszScratch, &ofSrc, OF_READ ) ;
      if (hfSrcFile != HFILE_ERROR)
      {
         wsprintf( gszScratch, "%s%s", (LPSTR)gszDestPath, (LPSTR)gszTTFFile ) ;
         DP1( hWDB, "   Opening %s as destination...", (LPSTR)gszScratch ) ;

         hfDstFile = LZOpenFile( gszScratch, &ofDst, OF_CREATE ) ;
         if (hfDstFile != HFILE_ERROR)
         {
            wsprintf( gszScratch, "%s%s",
                      (LPSTR)gszTempPath, (LPSTR)gszTT_File ) ;

            switch( LZCopy(hfSrcFile, hfDstFile) )
            {
               case LZERROR_BADINHANDLE:  // The handle identifying the source file was not valid.
               case LZERROR_BADOUTHANDLE: // The handle identifying the destination file was not valid.
               case LZERROR_GLOBALLOC:    // There is insufficient memory for the required buffers.
               case LZERROR_GLOBLOCK:     // The handle identifying the internal data structures is invalid.
               case LZERROR_READ:         // The source file format was not valid.
               case LZERROR_UNKNOWNALG:   // The source file was compressed with an unrecognized compression algorithm.
                  fSuccess = FALSE ;
                  BBSDisConnect() ;
                  ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                               IDS_APPNAME, IDS_ERR_LZERROR_GENERAL, (LPSTR)gszScratch ) ;
               break ;

               case LZERROR_WRITE:        // There is insufficient space for the output file.
                  fSuccess = FALSE ;
                  BBSDisConnect() ;
                  ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                               IDS_APPNAME, IDS_ERR_LZERROR_WRITE, (LPSTR)gszScratch ) ;
               break ;

               default: // success
                  /* Delete TT_ from the temp only on success.
                   */
                  fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
                  OpenFile( gszScratch, &of, OF_DELETE ) ;
                  SetErrorMode( fuEM ) ;
               break ;
            }

            LZClose( hfDstFile ) ;
         }

         LZClose( hfSrcFile ) ;
      }
      else
      {
         fSuccess = FALSE ;
         wsprintf( gszScratch, "%s%s",
                      (LPSTR)gszTempPath, (LPSTR)gszTT_File ) ;
         BBSDisConnect() ;
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                  IDS_APPNAME, IDS_ERR_LZERROR_GENERAL, (LPSTR)gszScratch ) ;
      }

      if (NODETYPE(lpdn) == NT_FACE)
         lpdnStyle = GetNextStyle( lpdnStyle, NULL ) ;
      else
         lpdnStyle = NULL ;
   }

   return fSuccess ;

} /* CopyDownloadedFonts() */

/****************************************************************
 *  BOOL NEAR PASCAL InstallDownloadedFonts( HWND hDlg, LPDBNODE lpdn )
 *
 *  Description: 
 *
 *  Installs the *.TTF files which were previously copied to the
 *  gszDestPath sub directory associated with the 
 *  sub order (identified by lpdn).
 *
 *  We will not be called unless the TTF file was successfully
 *  copied (but we do error checking anyway).
 *
 *  If the destpath is the system directory, the FOT files
 *  that are created will not have any absolute path information
 *  in them and thus can be used on any machine.  But if the
 *  destpath is NOT the system dir the FOT files will refer to
 *  the TTF files in the destpath.
 *
 *  When this function is called, the guy has already paid for
 *  the fonts so we must be very careful...
 *
 ****************************************************************/
BOOL NEAR PASCAL InstallDownloadedFonts( HWND hDlg, LPDBNODE lpdn )
{                                              
   LPSTYLE  lpdnStyle ;
   char szSysDir[120] ;
   char szFontResource[144] ;
   
   GetSystemDirectory( szSysDir, 120 ) ;
   lstrcat( szSysDir, "\\" ) ;

   if (NODETYPE( lpdn ) == NT_FACE)
      lpdnStyle = GetFirstStyle( (LPFACE)lpdn, NULL ) ;
   else
      lpdnStyle = (LPSTYLE)lpdn ;

   while( lpdnStyle && NODETYPE(lpdnStyle) == NT_STYLE )
   {
      lstrcpy( gszTTFFile, lpdnStyle->Info.szTTFFile ) ;
   
      wsprintf( szFontResource, "%s%s",
         (LPSTR)szSysDir, (LPSTR)gszTTFFile ) ;

      *(strchr( szFontResource, '.' ) + 1) = '\0' ;
      lstrcat( szFontResource, "FOT" ) ;

      wsprintf( gszScratch, "%s%s", (LPSTR)gszDestPath, (LPSTR)gszTTFFile ) ;

      DP1( hWDB, "   Installing %s and %s",
         (LPSTR)szFontResource, (LPSTR)gszScratch ) ;

      /* Verify the TTF file exists first.
       */
      if (0 == _access( gszScratch, 0 ))
      {
         if (lstrcmpi( szSysDir, gszDestPath ) == 0)
         {
            if (!CreateScalableFontResource( 0,
                                    szFontResource,
                                    gszTTFFile,
                                    gszDestPath ))
            {
               BBSDisConnect() ;
               ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_FONTINSTALL, (LPSTR)gszTTFFile ) ;

               return FALSE ;
            }
         }
         else
         {
            wsprintf( gszScratch, "%s%s",
               (LPSTR)gszDestPath, (LPSTR)gszTTFFile ) ;
         
            if (!CreateScalableFontResource( 0,
                                    szFontResource,
                                    gszScratch,
                                    NULL ))
            {
               BBSDisConnect() ;
               ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_FONTINSTALL, (LPSTR)gszTTFFile ) ;

               return FALSE ;
            }
         }

         if (!AddFontResource( szFontResource ))
         {
            BBSDisConnect() ;
            ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                     IDS_APPNAME, IDS_ERR_FONTINSTALL2, (LPSTR)gszTTFFile ) ;

            return FALSE ;
         }

         /* Add to the WIN.INI file [fonts] section an entry of the
          * form:
          *    Foobar Bright Italic (TrueType)=FOOBARI.FOT
          *
          * Never write "Regular"... it's implied.
          */
         if (lstrcmpi( "Regular", NODENAME( lpdnStyle ) ) == 0)
            wsprintf( gszScratch, "%s (TrueType)",
                  (LPSTR)((LPFACE)NODENAME( PARENT( LPFACE, lpdnStyle ) ) ) ) ;
         else
            wsprintf( gszScratch, "%s %s (TrueType)",
                  (LPSTR)((LPFACE)NODENAME( PARENT( LPFACE, lpdnStyle ) ) ),
                  (LPSTR)((LPSTYLE)NODENAME( lpdnStyle ) )) ;

         *(strchr( gszTTFFile , '.' ) + 1) = '\0' ;
         lstrcat( gszTTFFile, "FOT" ) ;
         DP1( hWDB, "   Creating WIN.INI entry '%s=%s'",
            (LPSTR)gszScratch, (LPSTR)gszTTFFile ) ;
         WriteProfileString( "fonts", gszScratch, gszTTFFile ) ;

         SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0L ) ;
      }

      if (NODETYPE(lpdn) == NT_FACE)
         lpdnStyle = GetNextStyle( lpdnStyle, NULL ) ;
      else
         lpdnStyle = NULL ;

   }

   return TRUE ;

} /* InstallDownloadedFonts()  */

/****************************************************************
 *  HFILE NEAR PASCAL OpenLogfile( VOID )
 *
 *  Description: 
 *
 *
 *
 ****************************************************************/
HFILE NEAR PASCAL OpenLogfile( VOID )
{
   HFILE    hfileLog ;
   char     szFile[144] ;

   wsprintf( szFile, "%s%s", (LPSTR)gszExePath, (LPSTR)"ORDER.LOG" ) ;

   if (HFILE_ERROR == OpenFile( szFile, &of, OF_EXIST ))
   {
      int n ;

      /* If the log file does not exist, create it, and write the
       * header.
       */

      DP2( hWDB, "Log file does not exist, creating it..." ) ;

      if (HFILE_ERROR != (hfileLog = OpenFile( szFile, &of, OF_CREATE |
                                                 OF_WRITE |
                                                 OF_SHARE_DENY_WRITE  )))
      {
         lstrcpy( gszScratch, GRCS( IDS_LOG_HEADER ) ) ;
         _lwrite( hfileLog, gszScratch, lstrlen( gszScratch ) ) ;

         wsprintf( gszScratch, GRCS( IDS_LOG_FMT1 ),
                     GRCS( IDS_LOG_LBL_ORDERID ),
                     GRCS( IDS_LOG_LBL_DATE ),
                     GRCS( IDS_LOG_LBL_TIME ),
                     GRCS( IDS_LOG_LBL_TYPE ),
                     GRCS( IDS_LOG_LBL_TOTAL ),
                     GRCS( IDS_LOG_LBL_PRODID ),
                     GRCS( IDS_LOG_LBL_FILENAME ),
                     GRCS( IDS_LOG_LBL_DESCRIPTION ),
                     GRCS( IDS_LOG_LBL_PUBLISHER ),
                     GRCS( IDS_LOG_LBL_PRICE ),
                     GRCS( IDS_LOG_LBL_SHIPPING ),
                     GRCS( IDS_LOG_LBL_SURCHARGE ),
                     GRCS( IDS_LOG_LBL_SALESTAX ) ) ;

         _lwrite( hfileLog, gszScratch, n = lstrlen( gszScratch ) ) ;

         memset( gszScratch, '-', n ) ;

         _lwrite( hfileLog, gszScratch, n ) ;
         _lwrite( hfileLog, "\r\n", 2 ) ;

         _lclose( hfileLog ) ;

         DP2( hWDB, "Log file created!" ) ;
      }
      else
      {
         DP1( hWDB, "Error, could not create log file!" ) ;
      }
   }

   if (HFILE_ERROR != (hfileLog = OpenFile( szFile, &of, OF_READWRITE |
                                                 OF_SHARE_DENY_WRITE  )))
   {
      _lseek( hfileLog, 0L, 2 ) ;
   }
   else
   {
      DP1( hWDB, "Error, could not re-open log file!" ) ;
   }

   return hfileLog ;

} /* OpenLogfile()  */


/************************************************************************
 * End of File: online.c
 ***********************************************************************/

