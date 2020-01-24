//*************************************************************
//  File name: purchase.c
//
//  Description: 
//      Code for the purchase dialog
//
//  History:    Date       Author     Comment
//               3/29/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include <dos.h>

#include "purchase.h"
#include "newuser.h"
#include "viewtext.h"
#include "bbs.h"
#include "online.h"

/* Exported to ONLINE.C
 */
HLIST    hlstPurchased ;

DWORD    gdwPrice, gdwSize, gdwShipping;

// imported by online.c
DWORD    gdwTotal ;

DWORD    gdwMaxItemSize ;

BOOL NEAR PASCAL ShowPublisherCopyrights(HWND hLB);
BOOL NEAR PASCAL EnufDiskSpace( HWND hwnd, DWORD dwSize, DWORD dwMax ) ;

BOOL CALLBACK fnPostPurchase( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI IsStyleOnSystem( HWND hDlg, LPSTYLE lpStyle ) ;

//*************************************************************
//
//  PurchaseProc
//
//  Purpose:
//      Dialog proc for the purchase dialog
//
//
//  Parameters:
//      HWND hDlg
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL CALLBACK)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/29/92   MSM        Created
//
//*************************************************************

BOOL CALLBACK PurchaseProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int tabs[4];

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            char szBuf[128];
            char szTime[128];
            DWORD dwTime;
            UINT  uiS, uiM, uiH;
            COMMSETTINGS cs;
            UINT  uiItems ;

            tabs[0] = tabs[1] = tabs[2] = tabs[3] = 0;
            GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;

            SetWindowFont( GetDlgItem( hDlg, IDL_ITEMS ), ghfontSmall, FALSE );
            SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), ghfontSmall, FALSE );

            if (!(uiItems = FillPurchaseDialog( hDlg )))
            {
               EndDialog( hDlg, FALSE ) ;
               return TRUE ;
            }

            if (!ReadCommSettings( &cs, GRCS(IDS_INIFILE) ))
                cs.dwBaudRate = 1200;

            /* Calculate the time to dl based on 10 bits per
             * byte of data (one start bit and one stop bit).
             *
             * We also add 30 seconds per item to account for
             * the time Ingres takes to look things up.
             */
            dwTime = gdwSize/(cs.dwBaudRate/10) +
                     (uiItems * 30) +  // Ingres overhead
                     25 ;              // Connect time

            uiH = (UINT)(dwTime/3600);
            dwTime = dwTime%3600;

            uiM = (UINT)(dwTime/60);
            dwTime = dwTime%60;

            uiS = (UINT)dwTime;

            #if 0
            if (uiS<=30)
                uiS = 30;
            else
            {
                uiS = 0;
                uiM++;
                if (uiM>59)
                {
                    uiM=0;
                    uiH++;
                }
            }
            #endif

            GetDlgItemText( hDlg, IDS_DOWNLOAD, szBuf, 128 ) ;
            wsprintf( szTime, szBuf, uiH, uiM, uiS );
            SendDlgItemMessage( hDlg, IDS_DOWNLOAD, WM_SETTEXT, 0,
                (LONG)(LPSTR)szTime );

            SendDlgItemMessage( hDlg, IDL_ITEMS, WM_SETREDRAW, 0, 0L );
            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
            return TRUE;
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            LPSTR       lp;
            RECT        rc;
            RECT        rcLB;
            int         nDP, cxChar, cyChar ;
            DWORD       dwPrice = gdwPrice;
            int         htabs[4];
            HFONT       hFont;
            TEXTMETRIC  tm ;
            HFONT       hfntDlg = GetWindowFont( hDlg ) ;

            BeginPaint( hDlg, &ps );

            GetClientRect( GetDlgItem(hDlg,IDS_SUBTOTAL), &rc );
            MapWindowRect(GetDlgItem(hDlg,IDS_SUBTOTAL), hDlg, &rc);

            GetClientRect( GetDlgItem(hDlg,IDL_ITEMS), &rcLB );
            MapWindowRect(GetDlgItem(hDlg,IDL_ITEMS), hDlg, &rcLB);

            if (tabs[0] == 0)   //*** Need to calculate the tabs
            {
                LPSTR lp;

                lp = GRCS(IDS_COLLECTION);
                tabs[0] = (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

                lp = GRCS(IDS_SAMPPRICE);
                tabs[1] = tabs[0];
                tabs[1] += (int)(LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)))*8/9);

                tabs[2] = tabs[1];
                tabs[2] += (int)(LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp))));

                lp = (LPSTR)"Digital Typface Corporation, Inc.";

                tabs[3] = tabs[2];
                tabs[3] += (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

                //*** Convert to DlgUnits
                tabs[0] = (tabs[0]*4)/LOWORD(GetDialogBaseUnits());
                tabs[1] = (tabs[1]*4)/LOWORD(GetDialogBaseUnits());
                tabs[2] = (tabs[2]*4)/LOWORD(GetDialogBaseUnits());
                tabs[3] = (tabs[3]*4)/LOWORD(GetDialogBaseUnits());

                SendDlgItemMessage( hDlg, IDL_ITEMS, LB_SETTABSTOPS, 4,
                    (LONG)(LPINT)tabs );
                SendDlgItemMessage( hDlg, IDL_ITEMS, WM_SETREDRAW, 1, 0L );
                InvalidateRect( GetDlgItem(hDlg,IDL_ITEMS), NULL, TRUE );
            }

            SetBkMode( ps.hdc, TRANSPARENT );
            SetBkColor( ps.hdc, GetSysColor(COLOR_WINDOW) ) ;
            
            lp = (LPSTR)"Type\tPrice\tSize\tPublisher\tDescription";

            hFont = SelectObject( ps.hdc, ghfontSmall );
            cxChar = GetTextMetricsCorrectly( ps.hdc, &tm ) ;
            cyChar = tm.tmHeight + tm.tmExternalLeading ;
            SelectObject( ps.hdc, hFont );

            //*** Convert into 'new' dialog units
            for (nDP=0; nDP<4; nDP++)
                htabs[nDP] = MulDiv(tabs[nDP], cxChar, 4 );

            hfntDlg = SelectObject( ps.hdc, hfntDlg );
            TabbedTextOut( ps.hdc, rcLB.left, rcLB.top - (cyChar+4), lp, lstrlen(lp), 4, htabs, rcLB.left );
            hfntDlg = SelectObject( ps.hdc, hfntDlg );

            //*** Location of the decimal point
            lp = GRCS( IDS_SAMPDOLLAR );
            nDP = rc.right + (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

            PaintDollarAmount( ps.hdc, nDP, rc.top, dwPrice );

            GetClientRect( GetDlgItem(hDlg,IDS_SHIPPING), &rc );
            MapWindowRect(GetDlgItem(hDlg,IDS_SHIPPING), hDlg, &rc);

            PaintDollarAmount( ps.hdc, nDP, rc.top, gdwShipping );
            dwPrice += gdwShipping;

            GetClientRect( GetDlgItem(hDlg,IDS_SURCHARGE), &rc );
            MapWindowRect(GetDlgItem(hDlg,IDS_SURCHARGE), hDlg, &rc);

            if (gCustInfo.uiWhichCard==AMERICAN_EXPRESS)
            {
                PaintDollarAmount( ps.hdc, nDP, rc.top, 
                    AMEX_SURCHARGE(gdwPrice));
                dwPrice += AMEX_SURCHARGE(gdwPrice);
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
        break;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDCANCEL:
                    EndDialog(hDlg, FALSE );
                    return TRUE;
                break;

                case IDB_SHIPPING:
                    ShippingInfoDialog( hDlg, gszHelpFile,
                                        GRCS(IDS_INIFILE), 1 );
                    GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
                break;

                case IDB_BILLING: 
                    BillingInfoDialog( hDlg, gszHelpFile,
                                        GRCS(IDS_INIFILE), 1 );
                    GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
                    FillPurchaseDialog( hDlg );
                break;

                case IDB_COMM:
                    CommSettingsDialog( hDlg, gszHelpFile,
                                        GRCS(IDS_INIFILE), 1 );
                    GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
                break;

                case IDOK:
                {
                  DLGPROC lpfn ;
                  BOOL    f ;

                  if (!EnufDiskSpace( hDlg, gdwSize, gdwMaxItemSize ))
                     break ;

                  if (!GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ))
                  {
                     ErrorResBox( hDlg, ghInst, MB_ICONINFORMATION,
                              IDS_APPNAME, IDS_ERR_CUSTINFO ) ;
                     break ;
                  }

                  /* Get first purchased thing.  This will be a collection
                   * if there are any collections selected.
                   */
                  if (gfDemoUser == FALSE && !VerifyOrderInfoDlg( hDlg, gszHelpFile, GRCS( IDS_INIFILE ), &gCustInfo,
                           (NODETYPE(GetFirstPurchasedNode( glpDB ))==NT_COLLECTION)))
                     return TRUE ;

                  if (!ShowPublisherCopyrights(GetDlgItem(hDlg,IDL_ITEMS)))
                     return TRUE ;

                  f = SubmitOrder( hDlg ) ;

                  if (hlstPurchased != NULL)
                  {   
                     lpfn = (DLGPROC)MakeProcInstance( (FARPROC)fnPostPurchase, ghInst );
                     DialogBox( ghInst,
                             MAKEINTRESOURCE(POSTPURCHASE_DLG),
                             hDlg, lpfn ) ;
                     FreeProcInstance( (FARPROC)lpfn );
                  }

                  if (f)
                     EndDialog( hDlg, TRUE ) ;

                }
                return TRUE;

                case IDB_HELP:
                    return ViewFontshopHelp( IDS_HELP_PURCHASE );
                break;

                default:
                    return FALSE;
                break;    
            }
        break;    
    }
    return FALSE;

} //*** PurchaseProc


/****************************************************************
 *  BOOL NEAR PASCAL EnufDiskSpace( HWND hwnd, DWORD dwSize, DWORD dwMax )
 *
 *  Description: 
 *
 *    Checks for enuf disk space.  Returns FALSE if there is
 *    not enuf.
 *
 *    There are two places there must be enough disk space:
 *
 *       gszDestPath - Sizeof expanded ttf files + fudge
 *       gszTempPath - Sizeof largest compressed ttf files + fudge
 *
 *    DWORD dwSize is the size of the entire order (compressed).
 *                 we need dwSize + 40% on the dest disk.
 *
 *    DWORD dwMax  is the size of the largest single file we'll
 *                 download.  we need dwMax + fudge on the
 *                 temp disk.
 *
 ****************************************************************/
BOOL NEAR PASCAL EnufDiskSpace( HWND hwnd, DWORD dwSize, DWORD dwMax )
{
   struct   _diskfree_t drvinfo ;
   DWORD    dwFree ;
   DWORD    dwNeed ;
   char       szTempPath[144] ;
   UINT       fuEM ;
   OFSTRUCT   of ;
   char     szFreeK[32] ;
   char     szSizeK[32] ;
   char     szNeedK[32] ;

   GetTempFileName(0, "fs", 0, szTempPath ) ;
   fuEM = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
   OpenFile( szTempPath, &of, OF_DELETE ) ;
   SetErrorMode( fuEM ) ;
   *(strrchr( szTempPath, '\\' )+1) = '\0' ;

   dwMax += (dwMax / 4) ;

   /* Get Free disk space on szTempPath */
   DP2( hWDB, "Order requires %lu bytes of disk space on the TEMP drive.", (DWORD)dwMax ) ;

   wsprintf( szSizeK, "%lu.%02luK", 
                      dwMax/1024,
                      ((dwMax%1024)*100)/1024 ) ;

   _dos_getdiskfree( (toupper(szTempPath[0]) - 'A') + 1, &drvinfo ) ;
   dwFree =  (DWORD)drvinfo.avail_clusters *
             drvinfo.sectors_per_cluster *
             drvinfo.bytes_per_sector ;

   wsprintf( szFreeK, "%lu.%02luK", 
                      dwFree/1024,
                      ((dwFree%1024)*100)/1024 ) ;

   DP2( hWDB, "   Temp (%s) has %lu bytes free.", (LPSTR)szTempPath, dwFree ) ;

   if (dwFree <= dwMax)
   {
      char szDrive[3] ;

      wsprintf( szDrive, "%c:", toupper(szTempPath[0]) ) ;

      dwNeed = (dwMax - dwFree) + 128 ;
      wsprintf( szNeedK, "%lu.%02luK", 
                      dwNeed/1024,
                      ((dwNeed%1024)*100)/1024 ) ;

      ErrorResBox( hwnd, ghInst, MB_ICONINFORMATION,
                   IDS_APPNAME, IDS_ERR_TEMP_DISKSPACE,
                   (LPSTR)szSizeK,
                   (LPSTR)szDrive,
                   (LPSTR)szDrive,
                   (LPSTR)szFreeK,
                   (LPSTR)szNeedK,
                   (LPSTR)szDrive ) ;

      return FALSE ;
   }

   /* Remember, the size of the files in the database is the compressed
    * size, so we need to increase it by 40%.
    */
   dwSize = (dwSize * 10) / 6 ;

   /* Get Free disk space on gszDestPath */
   if (toupper(gszDestPath[0]) == toupper(szTempPath[0]))
      dwSize += dwMax ;

   wsprintf( szSizeK, "%lu.%02luK", 
                      dwSize/1024,
                      ((dwSize%1024)*100)/1024 ) ;

   DP2( hWDB, "Order requires %lu bytes of disk space on the dest. drive.",
               (DWORD)dwSize ) ;

   _dos_getdiskfree( (toupper(gszDestPath[0]) - 'A') + 1, &drvinfo ) ;
   dwFree =  (DWORD)drvinfo.avail_clusters *
             drvinfo.sectors_per_cluster *
             drvinfo.bytes_per_sector ;

   wsprintf( szFreeK, "%lu.%02luK", 
                      dwFree/1024,
                      ((dwFree%1024)*100)/1024 ) ;


   DP2( hWDB, "   Dest (%s) has %lu bytes free.", (LPSTR)gszDestPath, dwFree ) ;

   if (dwFree <= dwSize)
   {
      char szDrive[3] ;

      wsprintf( szDrive, "%c:", toupper(gszDestPath[0]) ) ;

      dwNeed = (dwSize - dwFree) + 128 ;
      wsprintf( szNeedK, "%lu.%02luK", 
                      dwNeed/1024,
                      ((dwNeed%1024)*100)/1024 ) ;

      ErrorResBox( hwnd, ghInst, MB_ICONINFORMATION,
                   IDS_APPNAME, IDS_ERR_DEST_DISKSPACE,
                   (LPSTR)szSizeK,
                   (LPSTR)szDrive,
                   (LPSTR)szDrive,
                   (LPSTR)szFreeK,
                   (LPSTR)szNeedK,
                   (LPSTR)szDrive ) ;

      return FALSE ;
   }


   return TRUE ;

} /* EnufDiskSpace()  */

/****************************************************************
 *  BOOL CALLBACK fnPostPurchase( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *    Post-Purchase summary dialog.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnPostPurchase( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   static int tabs[4];

   switch (uiMsg)
   {
      case WM_INITDIALOG:
      {
         char              szFmt[256];
         char              szText[256];
         LPDBNODE          lpdn ;
         LPPUB             lpPub ;
         LPPURCHASEDNODE   lpPN ;
         HWND              hwndLB = GetDlgItem( hDlg, IDL_ITEMS ) ;

         if (hlstPurchased == NULL)
         {
            EndDialog( hDlg, FALSE ) ;
            return TRUE ;
         }

         tabs[0] = tabs[1] = tabs[2] = tabs[3] = 0;

         SetWindowFont( hwndLB, ghfontSmall, FALSE );
         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT2 ), ghfontSmall, FALSE );

         GetDlgItemText( hDlg, IDD_TEXT, szFmt, 255 ) ;
         if (gfInstallFonts)
            wsprintf( szText, szFmt, (LPSTR)gszDestPath, (LPSTR)GRCS( IDS_AND_INSTALLED ) ) ;
         else
            wsprintf( szText, szFmt, (LPSTR)gszDestPath, (LPSTR)"" ) ;
         SetDlgItemText( hDlg, IDD_TEXT, szText ) ;

         GetDlgItemText( hDlg, IDD_TEXT3, szFmt, 255 ) ;
         wsprintf( szText, szFmt,
                  (LPSTR)GRCS( gCustInfo.uiWhichCard + IDS_MASTERCARD ),
                  (DWORD)gdwTotal/100L, (DWORD)gdwTotal%100L ) ;

         SetDlgItemText( hDlg, IDD_TEXT3, szText ) ;

         lpPN = (LPPURCHASEDNODE)fsmmListFirstNode( hlstPurchased ) ;
         while( lpPN )
         {

            lpdn = lpPN->lpdn ;

            lpPub = (LPPUB)lpdn;

            while (NODETYPE(lpPub)!=NT_PUB)
               lpPub = PARENT(LPPUB,lpPub);

            switch (NODETYPE(lpdn))
            {
               case NT_COLLECTION:
               {
                  LPCOLL lpColl = (LPCOLL)lpdn ;
                  DWORD  dwPrice = FONTPRICE( lpColl ) ;

                  wsprintf( szText, "%08lX\t$%ld.%02ld\t%s (%s)",
                      (DWORD)lpPN->dwOrderID,
                      (DWORD)dwPrice/100L,
                      (DWORD)dwPrice%100L,
                      (LPSTR)(lpColl->Info.szName),
                      (LPSTR)lpPub->lpszPublisher ) ;

                  ListBox_AddString( hwndLB, szText ) ;
               }
               break;

               case NT_FACE:
               {
                  LPFACE lpFace = (LPFACE)lpdn;
                  DWORD  dwPrice = FONTPRICE(lpFace);
                  LPSTYLE lpStyle ;

                  wsprintf( szText, "%08lX\t$%ld.%02ld\t%s (%s)",
                      (DWORD)lpPN->dwOrderID,
                      (DWORD)dwPrice/100L,
                      (DWORD)dwPrice%100L,
                      (LPSTR)NODENAME(lpFace),
                      (LPSTR)lpPub->lpszPublisher ) ;

                  ListBox_AddString( hwndLB, szText ) ;

                  /* Get each style within the face */

                  lpStyle = GetFirstStyle( lpFace, NULL ) ;
                  while( lpStyle )
                  {
                     wsprintf( szText, "\t\t%s %s\t%s",
                         (LPSTR)NODENAME(lpFace),
                         (LPSTR)NODENAME(lpStyle),
                         (LPSTR)(lpStyle->Info.szTTFFile) ) ;

                     ListBox_AddString( hwndLB, szText ) ;

                     lpStyle = GetNextStyle( lpStyle, NULL ) ;
                  }

               }
               break;

               case NT_STYLE:
               {
                  LPSTYLE lpStyle = (LPSTYLE)lpdn;
                  DWORD   dwPrice = FONTPRICE(lpStyle);
                  LPFACE  lpFace = PARENT( LPFACE, lpStyle ) ;

                  wsprintf( szText, "%08lX\t$%ld.%02ld\t%s %s (%s)\t%s",
                      (DWORD)lpPN->dwOrderID,
                      (DWORD)dwPrice/100L,
                      (DWORD)dwPrice%100L,
                      (LPSTR)NODENAME(lpFace),
                      (LPSTR)NODENAME(lpStyle),
                      (LPSTR)lpPub->lpszPublisher,
                      (LPSTR)(lpStyle->Info.szTTFFile) ) ;

                  ListBox_AddString( hwndLB, szText ) ;

               }
               break;

            }

            lpPN = (LPPURCHASEDNODE)fsmmListNextNode( hlstPurchased, lpPN ) ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
         return TRUE;
      }
      break;

      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         LPSTR       lp;
         RECT        rcLB;
         int         nDP, cxChar, cyChar ;
         int         htabs[4];
         TEXTMETRIC  tm ;
         HFONT       hfntDlg ;

         BeginPaint( hDlg, &ps );

         GetClientRect( GetDlgItem(hDlg,IDL_ITEMS), &rcLB );
         MapWindowRect(GetDlgItem(hDlg,IDL_ITEMS), hDlg, &rcLB);

         if (tabs[0] == 0)   //*** Need to calculate the tabs
         {
               LPSTR lp;

               lp = "Order ID0000" ;
               tabs[0] = (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

               lp = GRCS( IDS_SAMPPRICE ) ;
               tabs[1] = tabs[0];
               tabs[1] += (int)(LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)))*8/9);

               lp = "123456789012345678901234567890123456789012345678012345"  ;
               tabs[2] = tabs[1];
               tabs[2] += (int)(LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp))));

               lp = (LPSTR)"WWWWWWWW.TTF";

               tabs[3] = tabs[2];
               tabs[3] += (int)LOWORD(GetTextExtent(ps.hdc,lp,lstrlen(lp)));

               //*** Convert to DlgUnits
               tabs[0] = (tabs[0]*4)/LOWORD(GetDialogBaseUnits());
               tabs[1] = (tabs[1]*4)/LOWORD(GetDialogBaseUnits());
               tabs[2] = (tabs[2]*4)/LOWORD(GetDialogBaseUnits());
               tabs[3] = (tabs[3]*4)/LOWORD(GetDialogBaseUnits());

               SendDlgItemMessage( hDlg, IDL_ITEMS, LB_SETTABSTOPS, 4,
                  (LONG)(LPINT)tabs );
               SendDlgItemMessage( hDlg, IDL_ITEMS, WM_SETREDRAW, 1, 0L );
               InvalidateRect( GetDlgItem(hDlg,IDL_ITEMS), NULL, TRUE );
         }

         SetBkMode( ps.hdc, TRANSPARENT );
         SetBkColor( ps.hdc, GetSysColor(COLOR_WINDOW) ) ;
      
         lp = (LPSTR)"Order ID\tPrice\tDescription\tFilename";

         ghfontSmall = SelectObject( ps.hdc, ghfontSmall );
         cxChar = GetTextMetricsCorrectly( ps.hdc, &tm ) ;
         ghfontSmall = SelectObject( ps.hdc, ghfontSmall );
         cyChar = tm.tmHeight + tm.tmExternalLeading ;

         //*** Convert into 'new' dialog units
         for (nDP=0; nDP<4; nDP++)
            htabs[nDP] = MulDiv(tabs[nDP], cxChar, 4 );

         hfntDlg = GetWindowFont( hDlg ) ;
         hfntDlg = SelectObject( ps.hdc, hfntDlg );
         TabbedTextOut( ps.hdc, rcLB.left, rcLB.top - (cyChar+4), lp, lstrlen(lp), 4, htabs, rcLB.left );
         hfntDlg = SelectObject( ps.hdc, hfntDlg );

         EndPaint( hDlg, &ps );
      }
      break;

      case WM_COMMAND:
         switch (wParam)
         {
               case IDCANCEL:
               case IDOK:
                  EndDialog(hDlg, TRUE );
                  return TRUE;
               break;

               case IDB_HELP:
                  return ViewFontshopHelp( IDS_HELP_POSTPURCHASE );
               break;

               default:
                  return FALSE;
               break;    
         }
      break;

      case WM_DESTROY:

         if (hlstPurchased)
         {
            fsmmListDestroy( hlstPurchased ) ;
            hlstPurchased = NULL ;
         }

      break ;
   }
   return FALSE;

} /* fnPostPurchase()  */





//*************************************************************
//
//  ShowPublisherCopyrights
//
//  Purpose:
//      Shows the publisher copyrights, one by one
//
//
//  Parameters:
//      HWND hLB - listbox to check publishers from
//      
//
//  Return: (BOOL NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/15/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL ShowPublisherCopyrights(HWND hLB) 
{
    LPPUB lpPub = GetFirstPublisher( glpDB, NULL );
    int   i;
    WORD  wRet = TRUE;
    char  szTemp[80] ;

    while (lpPub)
    {
        FreePublisherLCopyright( lpPub );
        lpPub = GetNextPublisher( lpPub, NULL );
    }

    for (i=0; i<ListBox_GetCount(hLB); i++)
    {
        LPPUB lpPub = (LPPUB)ListBox_GetItemData(hLB,i);
        LPSTR lp;
        WORD  w;

        while (lpPub && NODETYPE(lpPub)!=NT_PUB)
            lpPub = PARENT( LPPUB, lpPub );

        if (!lpPub || lpPub->lpszLCopyright)  // Already done it!
            continue;

        lp = LoadPublisherLCopyright( lpPub );
// BUGFIX #0008 5/23/92
        wsprintf( szTemp, "%s License Agreement", lpPub->lpszPublisher );
        w = ViewText( GetParent(hLB),lp,szTemp,VT_AGREE_DISAGREE|VT_NOHELP);
// BUGFIX #0008 5/23/92

        if (w == IDDISAGREE)
        {
            wRet = FALSE;
            break;
        }
    }

    while (lpPub)
    {
        FreePublisherLCopyright( lpPub );
        lpPub = GetNextPublisher( lpPub, NULL );
    }

    return wRet;

} //*** ShowPublisherCopyrights


//*************************************************************
//
//  PaintDollarAmount
//
//  Purpose:
//      Paints a dollar amount aligned at the decimal point
//
//
//  Parameters:
//      HDC hDC - DC to paint into
//      int nDP - x coord of the decimal point
//      int nY  - y coord of the line
//      DWORD dwPrice
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/31/92   MSM        Created
//
//*************************************************************

VOID WINAPI PaintDollarAmount( HDC hDC, int nDP, int nY, DWORD dwPrice )
{
    char szTemp[40];
    LPSTR lp = (LPSTR)szTemp;
    RECT  rc;
    DWORD dwTE;

    //*** Output the dollar amount
    wsprintf( lp, "%ld", dwPrice/100L );

    dwTE = GetTextExtent(hDC, lp, lstrlen(lp));
    rc.top  = nY;
    rc.bottom = nY + (int)HIWORD(dwTE);

    rc.right = nDP;
    rc.left = rc.right - (int)LOWORD(dwTE);

    ExtTextOut( hDC, rc.left, rc.top, ETO_OPAQUE, &rc, lp, lstrlen(lp),NULL);

    //*** Output the cents
    wsprintf( lp, ".%02ld", dwPrice%100L );

    rc.left = rc.right;
    rc.right = rc.left + (int)LOWORD(GetTextExtent(hDC, lp, lstrlen(lp)));

    ExtTextOut( hDC, rc.left, rc.top, ETO_OPAQUE, &rc, lp, lstrlen(lp),NULL);

} //*** PaintDollarAmount


//*************************************************************
//
//  FillPurchaseDialog
//
//  Purpose:
//      Fill the purchase dialog
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: UINT - Number of items in the list box.
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/30/92   MSM        Created
//               6/2/92    CEK        Made return number.
//               6/5/92    CEk        Verify font is not already installed
//
//*************************************************************

UINT WINAPI FillPurchaseDialog( HWND hDlg )
{
    HWND hLB = GetDlgItem( hDlg, IDL_ITEMS );
    LPDBNODE lpdn = GetFirstPurchasedNode( glpDB );
    char     szTemp[254];
    UINT     nItems = 0 ;
    static   BOOL fDemoUserUI ;

    SetWindowRedraw( hLB, FALSE );
    ListBox_ResetContent( hLB );

    fDemoUserUI = FALSE ; // haven't presented it yet.

    gdwPrice = gdwSize = gdwMaxItemSize = gdwShipping = 0L;

    while (lpdn)
    {
        LPPUB lpPub = (LPPUB)lpdn;

        while (NODETYPE(lpPub)!=NT_PUB)
            lpPub = PARENT(LPPUB,lpPub);

        switch (NODETYPE(lpdn))
        {
            case NT_COLLECTION:
            {
                LPCOLL lpColl = (LPCOLL)lpdn;
                DWORD  dwPrice = lpColl->Info.dwPrice;
                int    i = (int)lpColl->wShipping;

               if (dwPrice && (gfDemoUser && !gfDemoDatabase))
               {
                  szTemp[0] = '\0' ; // don't add it to the listbox
                  
                  /* Nuke him from the list
                   */
                  glpDB->dwTotal-= dwPrice ;
                  glpDB->wColls--;
                  VI_UNCHECKNODE(lpdn);

                  if (!fDemoUserUI)
                  {
                     fDemoUserUI = TRUE ;
                     ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                         IDS_APPNAME, IDS_ERR_DEMOUSER ) ;
                  }
               }
               else
               {
                  gdwPrice += dwPrice;

                  if (i==-1)
                  {
                     i= (int)gCustInfo.uiShipMethod;
                     if (lpColl->Info.rgShippingRates[i]==(DWORD)-1)
                     {
                           //*** Pick new default
                           for (i=0; i<3; i++)
                           {
                              if (lpColl->Info.rgShippingRates[i]!=(DWORD)-1)
                              {
                                 lpColl->wShipping = (WORD)i;
                                 break;
                              }
                           }
                     }
                  }
                  if (lpColl->Info.rgShippingRates[i] != (DWORD)-1L )
                     gdwShipping += lpColl->Info.rgShippingRates[i];

                  wsprintf( szTemp, GRCS(IDS_PURCOLL),dwPrice/100,dwPrice%100,
                     lpPub->lpszPublisher, (LPSTR)(lpColl->Info.szName) );
               }
            }
            break;

            case NT_FACE:
            {
               LPFACE lpFace = (LPFACE)lpdn;
               LPSTYLE lpdnStyle ;
               DWORD  dwPrice = FONTPRICE(lpFace);
               DWORD  dwSize  = FONTSIZE(lpFace);
               BOOL     fOk = TRUE ;

               if (dwPrice && (gfDemoUser && !gfDemoDatabase))
               {
                  if (!fDemoUserUI)
                  {
                     fDemoUserUI = TRUE ;
                     ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                         IDS_APPNAME, IDS_ERR_DEMOUSER ) ;
                  }

                  goto NukeFace ;
               }

               /* Go through all styles in this face to see if any of them
                * are already on the system.  If they are then do
                * not attempt to purchase them.
                */
               lpdnStyle = GetFirstStyle( lpFace, NULL ) ;
               while(lpdnStyle && NODETYPE(lpdnStyle) == NT_STYLE)
               {
                  if (IsStyleOnSystem( hDlg, lpdnStyle ))
                  {
NukeFace:
                     fOk = FALSE ;
                     glpDB->dwTotal -= dwPrice ;
                     glpDB->dwSize  -= dwSize ;
                     glpDB->wFonts--;

                     lpdnStyle = GetFirstStyle( (LPFACE)lpdn, NULL );
                     //*** When unchecking a FACE we need
                     //*** to handle the children properly
                     while (lpdnStyle)
                     {
                        VI_UNCHECKNODE(lpdnStyle);
                        lpdnStyle = GetNextStyle( lpdnStyle, NULL );
                     }
                     VI_UNCHECKNODE(lpdn);

                     szTemp[0] = '\0' ; // don't add it to the listbox

                     break ;
                  }
                  lpdnStyle = GetNextStyle( lpdnStyle, NULL ) ;
               }

               if (fOk)
               {
                   gdwPrice += dwPrice;
                   gdwSize  += dwSize;
                   
                   gdwMaxItemSize = max( dwSize, gdwMaxItemSize ) ;

                   wsprintf( szTemp, GRCS(IDS_PURFACE),dwPrice/100,dwPrice%100,
                       dwSize/1024, ((dwSize%1024)*100)/1024, 
                    lpPub->lpszPublisher, (LPSTR)NODENAME(lpFace));
               }
            }
            break;

            case NT_STYLE:
            {
                LPSTYLE lpStyle = (LPSTYLE)lpdn;
                LPFACE lpFace = PARENT(LPFACE,lpStyle);
                DWORD  dwPrice = FONTPRICE(lpStyle);
                DWORD  dwSize  = FONTSIZE(lpStyle);

               if ((dwPrice && (gfDemoUser && !gfDemoDatabase))  ||
                   IsStyleOnSystem( hDlg, lpStyle ))
               {
                  szTemp[0] = '\0' ; // don't add it to the listbox
                  
                  /* Nuke him from the list
                   */
                  glpDB->dwTotal-= FONTPRICE(((LPSTYLE)lpdn));
                  glpDB->dwSize -= FONTSIZE(((LPSTYLE)lpdn));
                  glpDB->wFonts--;
                  VI_UNCHECKNODE(lpdn);

                  if ((dwPrice && (gfDemoUser && !gfDemoDatabase)) && !fDemoUserUI)
                  {
                     fDemoUserUI = TRUE ;
                     ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                         IDS_APPNAME, IDS_ERR_DEMOUSER ) ;
                  }
               }
               else
               {

                gdwPrice += dwPrice;
                gdwSize  += dwSize;

                gdwMaxItemSize = max( dwSize, gdwMaxItemSize ) ;

                wsprintf( szTemp, GRCS(IDS_PURSTYLE),dwPrice/100,dwPrice%100,
                    dwSize/1024, ((dwSize%1024)*100)/1024, 
                    lpPub->lpszPublisher,
                    (LPSTR)NODENAME(lpFace), (LPSTR)NODENAME(lpStyle) );
               }
            }
            break;

            default:
                szTemp[0] = '\0';
            break;
        }

        if (szTemp[0]!='\0')
        {
            int i = (int)ListBox_AddString( hLB, (LPSTR)szTemp );

            if (i>=0)
                ListBox_SetItemData( hLB, i, (LONG)lpdn );

            nItems++ ;
        }

        lpdn = GetNextPurchasedNode( glpDB, lpdn );
    }
    SetWindowRedraw( hLB, TRUE );
    InvalidateRect( hLB, NULL, TRUE );
    InvalidateRect( hDlg, NULL, TRUE );

    return nItems ;

} //*** FillPurchaseDialog


/****************************************************************
 *  BOOL WINAPI IsStyleOnSystem( HWND hDlg, LPSTYLE lpStyle )
 *
 *  Description: 
 *
 *    Returns TRUE if the style passed exists on the system.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI IsStyleOnSystem( HWND hDlg, LPSTYLE lpStyle )
{
   BOOL     fRegular = FALSE ;
   char     szFontEntry[LF_FACESIZE*2+16] ;
   char     szTTFFile[TTF_LENGTH] ;
   char     szPath[144] ;
   PSTR     psz ;
   OFSTRUCT of ;

   /* First check the most obvious: The [fonts] section of the WIN.INI
    */
   wsprintf( szFontEntry, "%s %s (TrueType)",
      (LPSTR)NODENAME( PARENT( LPFACE, lpStyle ) ),
      (LPSTR)NODENAME( lpStyle ) ) ;

TryAgain:

   DP3( hWDB, "Looking in WIN.INI for '%s'", (LPSTR)szFontEntry ) ;
   if (GetProfileString( "fonts", szFontEntry, "", szPath, 144 ))
   {
      DP3( hWDB, "  Found!" ) ;
      /* It's there!
       */
      if (psz = strrchr( szPath, '\\' ))
         strcpy( szTTFFile, psz + 1 ) ;
      else
         strcpy( szTTFFile, szPath ) ;

      /* Stip off the .FOT */
      if (psz = strchr( szTTFFile, '.' ))
         *(psz) = '\0' ;

      /* Append a .TTF */
      strcat( szTTFFile, ".TTF" ) ;

      if (lstrcmpi( szTTFFile, lpStyle->Info.szTTFFile ) == 0)
      {
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                   IDS_APPNAME, IDS_ERR_TTFALREADYINSTALLED,
            (LPSTR)szFontEntry,
            (LPSTR)szTTFFile ) ;
      }
      else
      {
         ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                   IDS_APPNAME, IDS_ERR_NAMEALREADYINSTALLED,
            (LPSTR)szFontEntry ) ;
      }

      return TRUE ;
   }
   else
   {
      /* If the style is "Regular" try again */
      if (fRegular == FALSE && lstrcmpi( "Regular", NODENAME( lpStyle ) ) == 0)
      {
         wsprintf( szFontEntry, "%s (TrueType)",
               (LPSTR)NODENAME( PARENT( LPFACE, lpStyle ) )) ;
         fRegular = TRUE ;
         goto TryAgain ;
      }
   }

   /* It doesn't seem to be in the WIN.INI.  We still need to make
    * sure it's not in the destination directory.
    */
   wsprintf( szPath, "%s%s", (LPSTR)gszDestPath,
             (LPSTR)lpStyle->Info.szTTFFile ) ;

   DP3( hWDB, "Looking for %s...", (LPSTR)szPath ) ;
   if (HFILE_ERROR != OpenFile( szPath, &of, OF_EXIST ))
   {
      DP3( hWDB, "   Found!" ) ;
      ErrorResBox( hDlg, NULL, MB_ICONINFORMATION,
                   IDS_APPNAME, IDS_ERR_TTFEXISTS,
                  (LPSTR)szFontEntry,
                  (LPSTR)szPath ) ;

      return TRUE ;
   }

   /* Now let OpenFile() do it's thing.  It will attempt to search
    * the current dir, the path, the windows dir, etc... for the
    * file.  Here we give the user an option...
    */

   DP3( hWDB, "Looking for %s...", (LPSTR)lpStyle->Info.szTTFFile ) ;
   if (HFILE_ERROR != OpenFile( lpStyle->Info.szTTFFile, &of, OF_EXIST ))
   {
      DP3( hWDB, "   Found!" ) ;

      if (IDNO == ErrorResBox( hDlg, NULL, MB_ICONQUESTION | MB_YESNO,
                   IDS_APPNAME, IDS_ERR_TTFEXISTSSOMEWHERE,
                  (LPSTR)szFontEntry,
                  (LPSTR)of.szPathName ))
      {
         return TRUE ;
      }

      /* if he says so...
       */
      return FALSE ;
   }

   return FALSE ;

} /* IsStyleOnSystem()  */

//*** EOF: purchase.c
