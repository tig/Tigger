/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  online.c
 *
 *     Remarks:  Handles the Online Status modeless dialog box and
 *               the online Finite State Machine (FSM).
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "CONXSRV.h"
#include "connect.h"
#include "bbs.h"
#include "online.h"
#include "..\..\inc\CONXPRO.h"

//#define TEST_FAILURE
#ifdef TEST_FAILURE
#pragma message( "!!!! TEST_FAILURE DEFINED !!!!!" ) 
#endif

//#define TEST

#ifdef TEST
#pragma message( "!!!! TEST !!!!!" ) 
#endif

/* Exported
 */
HWND     hdlgOnline ;


/*
 * Function failure handling stuff.
 */
#define RIP_CITY( ui )  {uiIDerr=ui;goto Failure;}
static UINT    uiIDerr ;

static DLGPROC  lpfnOnline ;

static ZMODEMSENDPROC lpfnZmodemSend ;
UINT CALLBACK fnZmodemSend( HWND hwnd, LPBYTE lp, UINT cb ) ;


static DWORD dwPercent = 0 ;

/* Timing/Error recovery/Resync variables
 */
static UINT    uiOnlineDriverSpeed ;
static UINT    uiMaxReSyncAttempts ;
static UINT    uiMaxReSendAttempts ;
static UINT    uiReSyncWaitAckSecs ;
static UINT    uiNumErrors ;

static UINT    uiWaitAckSecs ;
static UINT    uiSayHiWaitAckSecs ;
static UINT    uiWaitReceiveSecs ;

static LPSTR   lpszLastError ;

BOOL CALLBACK fnOnlineDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID NEAR PASCAL SendData( LPONLINEFSM lpFSM ) ;
VOID WINAPI EnableOnlineDownloadStuff( HWND hDlg, BOOL f ) ;


/****************************************************************
 *  BOOL WINAPI StartOnlineDialog( VOID )
 *
 *  Description: 
 *
 *    Pops up the online status modeless dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI StartOnlineDialog( VOID )
{
   UINT  uiExitCode ;
#ifdef TEST
   DWORD dwSend ;
#endif

   (FARPROC)lpfnOnline = MakeProcInstance( (FARPROC)fnOnlineDlg, hinstApp ) ;

   hdlgOnline = CreateDialog( hinstApp, MAKEINTRESOURCE( DLG_ONLINE ),
                              hwndClientApp, lpfnOnline ) ;


   if (hdlgOnline)
   {
      /* Initialize the Online Finiate State Machine (BBS.C).
       */
      if (OnlineFSM( hdlgOnline, FF_ONLINE_INITFSM ) ==
          EXITCODE_OK)
      {
         char szBuf[128] ;

         wsprintf( szBuf, rglpsz[IDS_LOGGINGON], GetBaudRate() ) ;
         SetOnlineStatusMsg( szBuf ) ;

         wsprintf( szBuf, rglpsz[IDS_ONLINECAPTION], GetBaudRate() ) ;
         SetWindowText( hdlgOnline, szBuf ) ;

         /* We always have to say hi.
          */
         uiExitCode = SendBBSMsg(  MSG_SAY_HI, NULL, NULL ) ;

#ifdef TEST
         dwSend = EXE_FONTSHOP_INTERNAL  ;
         SendBBSMsg( MSG_SEND_EXETYPE, (LPVOID)&dwSend, 0L ) ;

         SendBBSMsg( MSG_SET_CUR_FILENAME, "NEWSHOP.EXE", 0L ) ;

         uiExitCode = SendBBSMsg(  MSG_RECEIVE_FILE, NULL, NULL ) ;
#endif
         if (EXITCODE_OK == uiExitCode)
            return TRUE ;
         else
            return FALSE ;

      }
      else
      {
         return FALSE ;
      }
   }

   return (BOOL)hdlgOnline ;

} /* StartOnlineDialog()  */

/****************************************************************
 *  BOOL WINAPI EndOnlineDialog( VOID )
 *
 *  Description: 
 *
 *    Closes down the online dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI EndOnlineDialog( VOID )
{
   DP4( hWDB, "EndOnlineDialog()..." ) ;

   if (hdlgOnline && IsWindow( hdlgOnline ))
   {
      DP4( hWDB, "Destroying online dialog box window..." ) ;

      DestroyWindow( hdlgOnline ) ;
   }
   
   hdlgOnline = NULL ;

   if (lpfnOnline)
      FreeProcInstance( (FARPROC)lpfnOnline ) ;
   lpfnOnline = NULL ;

   return TRUE ;

} /* EndOnlineDialog()  */

/****************************************************************
 *  BOOL CALLBACK fnOnlineDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 ****************************************************************/
BOOL CALLBACK fnOnlineDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         char szTime[64] ;
         /*
          * Setup all the controls
          */

         TimeGetCurTime( szTime, TIME_12HOUR ) ;
         SetDlgItemText( hDlg, IDD_STARTTIME, szTime ) ;
         SetDlgItemText( hDlg, IDD_CURRENTTIME, szTime ) ;

         if (IsWindow( hwndClientApp ))
            DlgCenter( hDlg, hwndClientApp, 0 ) ;

         ShowOnlineDownloadStuff( hDlg, FALSE ) ;
         EnableOnlineDownloadStuff( hDlg, FALSE ) ;

      }
      break ;

      case WM_COMMAND:
         switch ((UINT)wParam)
         {
            case IDCANCEL:
               if (IsWindowEnabled( (HWND)LOWORD( lParam )))
               {
                  OnlineFSM( hDlg, FF_ONLINE_USERABORT ) ;
               }
            break ;

            default :
               return FALSE ;
         }
         break ;

      case WM_DRAWITEM:
      {
         LPDRAWITEMSTRUCT lpdis ;
         char     szBuf[64] ;
         RECT     rc ;
         RECT     rcGauge ;
         int      cxText, cyText ;
         SIZE     Size ;
         int      cxDone ;
         HBRUSH   hbr ;


         lpdis = (LPDRAWITEMSTRUCT)lParam ;

         rc = lpdis->rcItem ;

         hbr = SelectObject( lpdis->hDC, GetStockObject( NULL_BRUSH ) ) ;
         Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom ) ;
         InflateRect( &rc, -1, -1 ) ;

         rcGauge = rc ;
         

         if (dwPercent != (DWORD)-1)
         {
            wsprintf( szBuf, "%d%%", (UINT)(dwPercent / (DWORD)100)) ;

            GetTextExtentPoint( lpdis->hDC, szBuf, lstrlen( szBuf ), &Size ) ;

            cxText = rcGauge.left + ((rcGauge.right - rcGauge.left) / 2) - (Size.cx / 2) ;

            cyText = rcGauge.top + ((rcGauge.bottom - rcGauge.top) / 2) - (Size.cy / 2) ;

            cxDone = rcGauge.left +
               (UINT)(((((long)rcGauge.right - (long)rcGauge.left) * (dwPercent / 100)) + 50) / 100 ) ;


            /*
             * Draw the done color
             */
            SetBkColor( lpdis->hDC, RGBLTRED ) ;
            SetTextColor( lpdis->hDC, RGBWHITE ) ;

            rcGauge.right = cxDone ;
            ExtTextOut( lpdis->hDC, cxText, cyText,
                  ETO_OPAQUE | ETO_CLIPPED,
                  &rcGauge, szBuf, lstrlen(szBuf), NULL ) ;

            /*
            * Draw the not-done color
            */
            SetBkColor( lpdis->hDC, RGBLTGREEN ) ;
            SetTextColor( lpdis->hDC, RGBBLACK ) ;

            rcGauge = rc ;

            rcGauge.left = cxDone ;

            ExtTextOut( lpdis->hDC, cxText, cyText,
               ETO_OPAQUE | ETO_CLIPPED,
               &rcGauge, szBuf, lstrlen(szBuf), NULL ) ;
         }
         else
         {
            /* Make it look disabled.
             */
            SetBkColor( lpdis->hDC, RGBLTGRAY ) ;

            ExtTextOut( lpdis->hDC, 0, 0, ETO_OPAQUE | ETO_CLIPPED,
               &rcGauge, NULL, 0, NULL ) ;
         }

         SelectObject( lpdis->hDC, hbr ) ;
      }
      break ;

      case WM_TIMER:
      {
         switch( wParam )
         {
            case IDTIMER_ONLINE_FSM:
               /* !!!! IMPORTANT !!!!!
               *
               * This is, perhaps, the most important case statement in
               * all of CONXSRV.  This sucker drives the OnlineFSM
               * finite state machine (See BBS.C).  The SetTimer() call that
               * initiates this timer can be found in there too.
               */
               if (EXITCODE_OK != OnlineFSM( hDlg, FF_ONLINE_PROCESS ))
               {
                  OnlineFSM( hDlg, FF_ONLINE_ABORT ) ;
               }
            break ;
         }
      }
      break ;

      case WM_DESTROY:
         /* Hack!  ShowOnlineDownloadStuff has a static var that
          * tells it whether it is open or not.  We reset it to
          * it's default state before exiting so that next time it
          * will be set correctly in WM_INITDIALOG above.
          */
         DP1( hWDB, "fnOnlineDlg: WM_DESTROY" ) ;
         ShowOnlineDownloadStuff( hDlg, TRUE ) ;
      break ;

   }
   return FALSE ;

} /* fnOnlineDlg()  */


/****************************************************************
 *  VOID WINAPI UpdateOnlineDialog( HWND hwnd )
 *
 *  Description: 
 *
 *    Updates the dialog box (download stuff).
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI UpdateOnlineDialog( HWND hwnd )
{
   static DWORD dwTimeStarted ;
   static DWORD dwFileSize ;

   static DWORD dwSumCPS = 0 ;
   static DWORD dwIterations = 1 ;
   static DWORD dwStartBytes = 0 ;

   char szBuf1[256] ;
   char szBuf2[256] ;
   PSTR pszFileName ;
   DWORD dw ;
   DWORD dwTime ;
   DWORD dwTimeNow ;
   DWORD dwTimeLeft ;
   DWORD dwBytes ;
   UINT  uiCPS ;
   UINT  uiSecs ;
   UINT  uiMins ;
   UINT  uiHrs ;

   lstrcpy( szBuf1, (LPSTR)Zmodem( hwnd, ZM_GET_FILENAME, 0L, 0L ) ) ;

   /* Strip path off of filename to hide from user since
    * it's usually a temp filename...
    */
   GetDlgItemText( hwnd, IDD_FILENAME, szBuf2, 255 ) ;

   pszFileName = strrchr( szBuf1, '\\' ) ;
   if (pszFileName)
      pszFileName++ ;
   else
      pszFileName = szBuf1  ;

   if (lstrcmpi( szBuf2, pszFileName ))
   {
      SetDlgItemText( hwnd, IDD_FILENAME, pszFileName) ;

      dwFileSize = Zmodem( hwnd, ZM_GET_FILESIZE, 0L, 0L ) ;
      wsprintf( szBuf1, "%lu bytes", (DWORD)dwFileSize ) ;
      GetDlgItemText( hwnd, IDD_FILESIZE, szBuf2, 255 ) ;
      if (lstrcmpi( szBuf1, szBuf2 ))
         SetDlgItemText( hwnd, IDD_FILESIZE, szBuf1 ) ;

      dwTimeStarted = Zmodem( hwnd, ZM_GET_TIMESTARTED, 0L, 0L) ;

      dwStartBytes = (DWORD)Zmodem( hwnd, ZM_GET_FILEBYTES, 0L, 0L ) ;

      dwSumCPS = 0 ;
      dwIterations = 1 ;

   }

   /* Bytes received
    */
   dwBytes = (DWORD)Zmodem( hwnd, ZM_GET_FILEBYTES, 0L, 0L ) ;
   wsprintf( szBuf1, "%lu bytes", (DWORD)dwBytes ) ;
   GetDlgItemText( hwnd, IDD_BYTECOUNT, szBuf2, 255 ) ;
   if (lstrcmpi( szBuf1, szBuf2 ))
      SetDlgItemText( hwnd, IDD_BYTECOUNT, szBuf1 ) ;

   /*
    * Percent
    */
   if (dwFileSize && dwBytes)
      dw = (((DWORD)10000 * dwBytes) / dwFileSize) ;
   else
      dw = 0 ;

   if (dw != dwPercent)
   {
      dwPercent = dw ;
      InvalidateRect( GetDlgItem( hwnd, IDD_GASGAUGE ), NULL, FALSE ) ;
      UpdateWindow( GetDlgItem( hwnd, IDD_GASGAUGE ) ) ;
   }

   dwBytes -= dwStartBytes ;

   /*
    * c.p.s.
    */
   time( &dwTimeNow ) ;

   dwTime = (dwTimeNow - dwTimeStarted) ;

   if (dwBytes)
   {
      if (dwTime)
      {
         uiCPS = (UINT)(dwBytes / dwTime) ;
         if (uiCPS == 0)
            uiCPS = (UINT)(GetBaudRate() / 8) ;
      }
      else
         uiCPS = (UINT)(GetBaudRate() / 8) ;

      dwSumCPS += uiCPS ;
      dwIterations++ ;

      uiCPS = (UINT)((DWORD)dwSumCPS / (DWORD)dwIterations) ;
   }
   else
      uiCPS = 0 ;

   wsprintf( szBuf1, "%d cps", uiCPS ) ;

   GetDlgItemText( hwnd, IDD_THROUGHPUT, szBuf2, 255 ) ;
   if (lstrcmpi( szBuf1, szBuf2 ))
      SetDlgItemText( hwnd, IDD_THROUGHPUT, szBuf1 ) ;


   /* Est. Time is based on current cps.
    */
   if (uiCPS != 0)
      dwTimeLeft = (dwFileSize - dwBytes) / (DWORD)uiCPS ;
   else
      dwTimeLeft = (dwFileSize - dwBytes) / (DWORD)(GetBaudRate() / 8) ;

   dwTimeLeft %= (DWORD)86400 ;
   uiHrs = (UINT)(dwTimeLeft / (DWORD)3600) ;

   dwTimeLeft %= (DWORD)3600 ;
   uiMins = (UINT)(dwTimeLeft / (DWORD)60) ;

   uiSecs = (UINT)(dwTimeLeft % (DWORD)60) ;

   wsprintf( szBuf1, "%02d:%02d:%02d", uiHrs, uiMins, uiSecs ) ;
   GetDlgItemText( hwnd, IDD_ESTTIME, szBuf2, 255 ) ;
   if (lstrcmpi( szBuf1, szBuf2 ))
      SetDlgItemText( hwnd, IDD_ESTTIME, szBuf1 ) ;

   if (uiNumErrors = (UINT)Zmodem( hwnd, ZM_GET_TOTALERRORS, 0L, 0L ))
   {
      switch( Zmodem( hwnd, ZM_GET_LASTERROR, 0L, 0L ) )
      {
         case ZME_LOCALCANCEL:
            lpszLastError = "Transfer canceled by user" ;
         break ;

         case ZME_TOOMANYERRORS:
            lpszLastError = "Too many errors" ;
         break ;

         case ZME_TIMEOUT:
            lpszLastError = "Timeout" ;
         break ;

         case ZME_GENERAL:
            lpszLastError = "General file transfer error" ;
         break ;

         case ZME_HOSTCANCEL:
            lpszLastError = "Transfer canceled by host" ;
         break ;

         case ZME_DISKERROR:
            lpszLastError = "File I/O Error" ;
         break ;

         case ZME_DISKFULL:
            lpszLastError = "Out of disk space" ;
         break ;

         case ZME_NAK:
            lpszLastError = "NAK Error" ;
         break ;

         case ZME_RECV_INIT:
         case ZME_RECV_FILE:
         case ZME_RECV_DATA:
         case ZME_SENT_INIT:
         case ZME_SENT_FILE:
         case ZME_SENT_DATA:
         case ZME_SENT_ZFIN:
            lpszLastError = "CRC Error" ;
         break ;

      }
   }


} /* UpdateOnlineDialog()  */


/****************************************************************
 *  VOID WINAPI ShowOnlineDownloadStuff( HWND, BOOL f )
 *
 *  Description: 
 *
 *    If f is TRUE then extend the dialog box to include the
 *    file download shit.
 *
 *
 ****************************************************************/
VOID WINAPI ShowOnlineDownloadStuff( HWND hDlg, BOOL f )
{
   static BOOL fCur = TRUE ;
   RECT  rcItem ;
   RECT  rcDlg ;

   EnableOnlineDownloadStuff( hDlg, f ) ;

   if (f == fCur)
      return ;

   fCur = f ;

   GetWindowRect( hDlg, &rcDlg ) ;
   GetWindowRect( GetDlgItem( hDlg, IDD_FILEDOWNLOAD_GRP ), &rcItem ) ;
   MapWindowRect( GetDlgItem( hDlg, IDD_FILEDOWNLOAD_GRP ), NULL, &rcItem ) ;

   /* Make sure SWP_NOACTIVATE is used to avoid activating the
    * window (since it might be in a WM_DESTROY)
    */
   if (f == TRUE)
      SetWindowPos( hDlg, NULL, 0, 0, rcDlg.right - rcDlg.left,
                     (rcDlg.bottom - rcDlg.top) +
                     (rcItem.bottom - rcItem.top) + 6, 
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE ) ;
   else
      SetWindowPos( hDlg, NULL, 0, 0, rcDlg.right - rcDlg.left,
                     (rcDlg.bottom - rcDlg.top) -
                     (rcItem.bottom - rcItem.top) - 6, 
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE ) ;

} /* ShowOnlineDownloadStuff()  */


VOID WINAPI EnableOnlineDownloadStuff( HWND hDlg, BOOL f )
{
   EnableWindow( GetDlgItem( hDlg, IDD_FILEDOWNLOAD_GRP ), f ) ;

   EnableWindow( GetDlgItem( hDlg, IDD_FILENAME ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_FILESIZE ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_BYTECOUNT ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_THROUGHPUT ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_ESTTIME ), f ) ;

   EnableWindow( GetDlgItem( hDlg, IDD_FILENAME_LBL ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_FILESIZE_LBL ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_BYTECOUNT_LBL ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_THROUGHPUT_LBL ), f ) ;
   EnableWindow( GetDlgItem( hDlg, IDD_ESTTIME_LBL ), f ) ;

   dwPercent = (DWORD)-1 ;
   InvalidateRect( GetDlgItem( hDlg, IDD_GASGAUGE ), NULL, FALSE ) ;
}

/****************************************************************
 *  VOID WINAPI SetOnlineStatusMsg( LPSTR lpsz )
 *
 *  Description: 
 *
 *    For whatever dialog box is up there will always be a
 *    IDD_STATUS text control. 
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SetOnlineStatusMsg( LPSTR lpsz )
{
   if (IsWindow( hdlgOnline ))
   {
      char  szBuf[256] ;
      char  szCurTime[64] ;

      if (lpsz)
      {
         GetDlgItemText( hdlgOnline, IDD_STATUS, szBuf, 255 ) ;
         if (0 != lstrcmpi( szBuf, lpsz ))
            SetDlgItemText( hdlgOnline, IDD_STATUS, lpsz ) ;

         #if 0
         if (IsWindow( hwndStat ))
         {
            GetWindowText( hwndStat, szBuf, 255 ) ;
            if (0 != lstrcmpi( szBuf, lpsz ))
               SetWindowText( hwndStat, lpsz ) ;
         }
         #endif

      }

      GetDlgItemText( hdlgOnline, IDD_CURRENTTIME, szBuf, 255 ) ;
      TimeGetCurTime( szCurTime, TIME_12HOUR ) ;
      if (0 != lstrcmpi( szBuf, szCurTime ))
         SetDlgItemText( hdlgOnline, IDD_CURRENTTIME, szCurTime ) ;


      /*
       * Error stuff
       */
      GetDlgItemText( hdlgOnline, IDD_LASTERROR, szBuf, 255 ) ;
      if (lpszLastError && (0 != lstrcmpi( szBuf, lpszLastError )))
         SetDlgItemText( hdlgOnline, IDD_LASTERROR, lpszLastError ) ;

      if (GetDlgItemInt( hdlgOnline, IDD_NUMERRORS, NULL, FALSE ) != uiNumErrors)
         SetDlgItemInt( hdlgOnline, IDD_NUMERRORS, uiNumErrors, FALSE ) ;

   }

} /* SetOnlineStatusMsg()  */



/****************************************************************
 *  UINT WINAPI OnlineFSM( HWND hwnd, UINT uiFunc )
 *
 *  Description: 
 *
 *    This is the finite state machine function that is running
 *    whenever we are connected.  The functions DoConnectDlg() and
 *    DoDisConnectDlg() are responsible for starting and
 *    stoping this FSM.
 *
 *    This function is the core of CONXSRV's interaction with
 *    the bbs.  All messages that are sent or recieved are
 *    processed through this FSM.  The beauty of the FSM
 *    is that all timeouts and pauses are handled easily and
 *    in a 'cooperative' manner.  It is also easily extensible.
 *
 *    The timer (see the main window procedure) that drives this FSM
 *    is set to ONLINE_DRIVE_SEC seconds.
 *
 *    OnlineFSM() returns one of the "EXITCODE_"s defined in
 *    bbs.h.  EXITCODE_OK indicates that the call was
 *    successful and is deined as 0x0000.  All other codes indicate
 *    some kind of error and are defined greater than 0.
 *
 ****************************************************************/
UINT WINAPI OnlineFSM( HWND hwnd, UINT uiFunction )
{
   LPONLINEFSM   lpFSM = NULL ;

   if (uiFunction != FF_ONLINE_INITFSM)
   {

      /*
       * This condition could be caused during an error.  The
       * FF_ONLINE_FSM function nukes the memory and then pops up the
       * error dialog.  When this dialog box is up WM_TIMER messages
       * are still comming into the main window, which is translating
       * them into calls here.  This little test allows us to back
       * out gracefully in this case.
       *
       * The other scenario where we'll get called after lpFSM has
       * been freed is during a sequence of OnlineBBS() calls where
       * the return value is not being checked.  By bolting out of here
       * gracefully in this case not checking the return value is cool.
       */
      if (IsWindow( hwnd ))
         lpFSM = (LPONLINEFSM)GET_FSM( hwnd ) ;

      if (!lpFSM)
      {
         DP4( hWDB, "OnlineFSM(): lpFSM is NULL and we're not INITFSM!" ) ;
         return EXITCODE_OK ;
      }

   }

   /* Handle each forced function.  The 'default' case (also FF_ONLINE_PROCESS)
    * in this
    * switch statement is where the actual states are handled.
    */
   switch (uiFunction)
   {
      case FF_ONLINE_INITFSM:
         DP3( hWDB, "OnlineFSM( FF_ONLINE_INITFSM )" ) ;
         SetOnlineStatusMsg( rglpsz[IDS_INITIALIZING] ) ;
         
         /* Attempt to read conxtion.ini for timing options
          */
         uiOnlineDriverSpeed = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_ONLINEDRIVERSPEED],
                                               ONLINE_DRIVE_MSEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         uiMaxReSyncAttempts = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                                     rglpsz[IDS_INI_RESYNCATTEMPTS],
                                                     DEFAULT_RESYNCATTEMPTS,
                                                     rglpsz[IDS_INI_FILENAME] ) ;
         uiMaxReSendAttempts = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                                     rglpsz[IDS_INI_RESENDATTEMPTS],
                                                     DEFAULT_RESENDATTEMPTS,
                                                     rglpsz[IDS_INI_FILENAME] ) ;

         uiReSyncWaitAckSecs = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                                     rglpsz[IDS_INI_RESYNCWAITACK],
                                                     ONLINE_RESYNC_WAITACK_SEC,
                                                     rglpsz[IDS_INI_FILENAME] ) ;

         uiWaitAckSecs = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_WAITACK],
                                               ONLINE_WAITACK_SEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         uiSayHiWaitAckSecs = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_SAYHIWAITACK],
                                               ONLINE_SAYHIWAITACK_SEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;


         uiWaitReceiveSecs = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                               rglpsz[IDS_INI_WAITRECEIVE],
                                               ONLINE_WAITRECEIVE_SEC,
                                               rglpsz[IDS_INI_FILENAME] ) ;

         DP3( hWDB, "uiSayHiWaitAckSecs       = %d", uiSayHiWaitAckSecs ) ;
         DP3( hWDB, "uiWaitAckSecs       = %d", uiWaitAckSecs ) ;
         DP3( hWDB, "uiWaitReceiveSecs   = %d", uiWaitReceiveSecs ) ;
         DP3( hWDB, "uiMaxReSendAttempts = %d", uiMaxReSendAttempts ) ;
         DP3( hWDB, "uiMaxReSyncAttempts = %d", uiMaxReSyncAttempts ) ;
         DP3( hWDB, "uiReSyncWaitAckSecs = %d", uiReSyncWaitAckSecs ) ;
         
         /*
          * Turn on our driving light
          */
         if (!SetTimer( hwnd, IDTIMER_ONLINE_FSM, uiOnlineDriverSpeed, NULL ))
         {
            DP1( hWDB, "Could not create IDTIMER_ONLINE_FSM" ) ;
            RIP_CITY( IDS_ERR_TIMER ) ;
         }

         /*
          * Now I could have been really lazy and just used a global for
          * the FSM data structures.  But I'm not lazy, and by taking the
          * time to use instanced data, there is the distinct possiblity
          * that this code could be used somewhere else somday...so there.
          */
         if (lpFSM = GlobalAllocPtr( GHND, sizeof( ONLINEFSM )))
         {
            /*
             * Put our far pointer into the window props of the dlg.
             */
            CREATE_FSM( hwnd, lpFSM ) ;

            /* Initialize the FSM structure
             *
             */
            lpFSM->uiNextState = STATE_ONLINE_IDLE ;
            lpFSM->uiExitCode = EXITCODE_OK ;
            ResetPause( &lpFSM->Pause ) ;
            ResetTimeout( &lpFSM->Timeout ) ;
         }
         else
         {
            DP1( hWDB, "Could not allocate FSM structure" ) ;

            ErrorResBox( hwndClientApp, NULL, MB_OK | MB_ICONSTOP,
                         IDS_APPTITLE, IDS_ERR_MEMORYALLOC,
                         (LPSTR)__FILE__,
                         (LPSTR)__LINE__ ) ;

            RIP_CITY( 0 ) ;
         }

         /*
          * Setup our callback for the Zmodem() protocol.
          */
         lpfnZmodemSend =
            (ZMODEMSENDPROC)MakeProcInstance( (FARPROC)fnZmodemSend,
                                              hinstApp ) ;

         /*
          * Set inital state
          */
         lpFSM->uiState = STATE_ONLINE_IDLE ;

         uiNumErrors = 0 ;
         lpszLastError = "" ;

      break ;

      /*
       * This forced function can never be called from within
       * the online dialog box.  Otherwise we'll crash a horrible
       * death when we destroy the window, and free the procedure
       * instance in EndOnlineDilaog()
       */
      case FF_ONLINE_EXITFSM:
      {
         UINT uiExitCode = lpFSM->uiExitCode ;
         DP3( hWDB, "OnlineFSM( FF_ONLINE_EXITFSM )" ) ;

         /* The timer may have already been killed, but there is
          * nothing wrong with calling it again (or so the docs say)
          */
         KillTimer( hwnd, IDTIMER_ONLINE_FSM ) ;

         GlobalFreePtr( lpFSM ) ;
         REMOVE_FSM( hwnd ) ;

         if (lpfnZmodemSend)
         {
            Zmodem( hwnd, ZM_KILL, 0L, 0L ) ;
            TellCommToDoZmodem( NULL ) ;

            FreeProcInstance( (FARPROC)lpfnZmodemSend ) ;
            lpfnZmodemSend = NULL ;
         }

         EndOnlineDialog() ;

         return uiExitCode ;
      }
      break ;

      case FF_ONLINE_ABORT:
      {
         UINT uiExitCode = lpFSM->uiExitCode ;
         DP3( hWDB, "OnlineFSM( FF_ONLINE_ABORT )" ) ;

         SetOnlineStatusMsg( rglpsz[IDS_ABORTING] ) ;

         if (uiExitCode != EXITCODE_OK)
         {
            ErrorResBox( hwnd, hinstApp, MB_OK | MB_ICONSTOP,
                   IDS_ERR_COMMUNICATIONS, uiExitCode + IDS_ERR_EC_OK ) ;

         }

         PostMessage( hwndApp, WM_COMMAND, IDM_DISCONNECT, 0L ) ;

         return uiExitCode ;
      }
      break ;

      case FF_ONLINE_USERABORT:
         DP3( hWDB, "OnlineFSM( FF_ONLINE_USERABORT )" ) ;
      {
         UINT  uiExitCode = lpFSM->uiExitCode ;

         SetOnlineStatusMsg( rglpsz[IDS_ABORTING] ) ;

         /*
          * Tell Zmodem to go to hell
          */
         if (lpFSM->uiState == STATE_ONLINE_CONXPRO_RECEIVE)
         {
            DP1( hWDB, "Aborting CONXPRO!" ) ;

            Zmodem( hwnd, ZM_LOCALCANCEL, 0L, 0L ) ;

            break ;
         }

         /*
          * This forced function is executed whenever the user
          * says 'abort'
          *
          * We post a WM_COMMAND to the main window here because
          * if we tried to destroy the Online dialog box here
          * we would be destroying the window procedure that
          * called us.  In debug 3.1 you get a nice rip
          * when this happens.  By posting the IDM_DISCONNECT
          * message we accomplish everything nicely.
          * 
          */
         PostMessage( hwndApp, WM_COMMAND, IDM_DISCONNECT, 0L ) ;

         return uiExitCode ;
      }
      break ;

      case FF_ONLINE_PROCESS:
         DP4( hWDB, "OnlineFSM( FF_ONLINE_PROCESS )" ) ;
      {
         if (IsOnline() == FALSE)
         {
            SetOnlineStatusMsg( NULL ) ;

            if (lpFSM->uiState != STATE_ONLINE_FATALABORT)
            {
               DP3( hWDB, "IsOnline failed in FF_ONLINE_PROCESS.  Aborting!" ) ;

               lpFSM->uiState = STATE_ONLINE_FATALABORT ;
               lpFSM->uiExitCode = EXITCODE_LOSTCONNECTION ;
               return lpFSM->uiExitCode ;
            }
            else
               return EXITCODE_OK ;
         }

         switch( lpFSM->uiState )
         {
            case STATE_ONLINE_IDLE:

               DP4( hWDB, "STATE_ONLINE_IDLE" ) ;

               SetOnlineStatusMsg( rglpsz[IDS_IDLE] ) ;
            break ;

            case STATE_ONLINE_PAUSE:
               DP4( hWDB, "STATE_ONLINE_PAUSE" ) ;

               SetOnlineStatusMsg( NULL ) ;

               if (IsPauseDone( &lpFSM->Pause ))
               {
                  /* Pause is complete.  Party on dude.
                   */
                  lpFSM->uiState = lpFSM->uiNextState ;
                  ResetPause( &lpFSM->Pause ) ;
               }
            break ;

            case STATE_ONLINE_SENDMSG:
               DP3( hWDB, "STATE_ONLINE_SENDMSG" ) ;
               SetOnlineStatusMsg( NULL ) ;  // set elsewhere

               /* Just for the heck of it, clear the input
                * queue and theoutput queue...
                */
               //FlushInComm() ;
               //FlushOutComm() ;

               /*
                * Send the two bytes making up the message twice for
                * redundancy.  (Trust me, the bbs expects this).
                */
               SetTTYBinary( TRUE ) ;
               WriteCommBlock( &lpFSM->bSendMsg, sizeof( lpFSM->bSendMsg ) ) ;
               WriteCommBlock( &lpFSM->bSendMsg, sizeof( lpFSM->bSendMsg ) ) ;
               SetTTYBinary( FALSE ) ;

               /*
                * this state can transition to either SENDDATA,
                * RECEIVEDATA, SENDRECEIVEDATA, or IDLE.  The
                * state it transistions to is determined by
                * the previous state, which should have set
                * uiNextState.
                */
               lpFSM->uiLastState = lpFSM->uiState ;
               lpFSM->uiState = lpFSM->uiNextState ;

               /* Special HACK!  For Zmodem */
               if (lpFSM->uiState == STATE_ONLINE_CONXPRO_RECEIVE)
               {
                  TellCommToDoZmodem( hwnd ) ;
               }

            break ;

            case STATE_ONLINE_SENDDATA_WAITACK:
            {
               LPBYTE   lpSend = lpFSM->lpSend ;

               SetOnlineStatusMsg( rglpsz[IDS_WAITINGFORRESPONSE] ) ;

               /*
                * Send the data pointed to by lpFSM->lpSend and
                * then wait for the host to respond with an
                * ack.
                */

               SendData( lpFSM ) ;

               lpFSM->uiLastState = lpFSM->uiState ;
               lpFSM->uiState = STATE_ONLINE_WAITACK ;
            }
            break ;

            case STATE_ONLINE_SENDDATA_WAITRECEIVE:
               SetOnlineStatusMsg( NULL ) ;
               /*
                * Send the data, but do not wait for an ACK.  If
                * there is data to be received then wait for it.
                */

               SendData( lpFSM ) ;

               lpFSM->uiLastState = lpFSM->uiState ;

               if (lpFSM->cbReceive)
                  lpFSM->uiState = STATE_ONLINE_WAITRECEIVE ;
               else
                  lpFSM->uiState = STATE_ONLINE_IDLE ;
            break ;

            case STATE_ONLINE_WAITACK:
            DP4( hWDB, "STATE_ONLINE_WAITACK" ) ;
            {
               BYTE  bInChar ;

               if (lpFSM->bSendMsg != MSG_SAY_HI)
               {
                  SetOnlineStatusMsg( rglpsz[IDS_WAITINGFORRESPONSE] ) ;

                  /*
                   * If we ever enter this state without a timeout
                   * set it means that we are entering it for the
                   * first time.  So set the timout timer...
                   */

                  if (!InTimeout( lpFSM->Timeout ))
                  {
                     StartTimeout( &lpFSM->Timeout, uiWaitAckSecs ) ;
                     break ;
                  }

               }
               else
               {
                  SetOnlineStatusMsg( NULL ) ;

                  /* Special case MSG_SAY_HI.  The BBS may take a while
                   * to respond to this one (longer than the default
                   * 20 second timeout).
                   */
                  if (!InTimeout( lpFSM->Timeout ))
                  {
                     StartTimeout( &lpFSM->Timeout, uiSayHiWaitAckSecs ) ;
                     break ;
                  }
               }

               /*
                * We're waiting for the bbs to ACKnowledge the last
                * command that was sent.  
                */
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  /* We've timed out!  Bummer.
                   *
                   */                    
                  DP1( hWDB, "Timeout in STATE_ONLINE_WAITACK!!!" ) ;

                  lpFSM->uiLastState = lpFSM->uiState ;
                  lpFSM->uiExitCode = EXITCODE_ACK_TIMEOUT ;
                  ResetTimeout( &lpFSM->Timeout ) ;

                  /* For MSG_SAY_HI we don't try to re-sync.  If
                   * we didn't get a response to the host within
                   * uiSayHiWaitAckSecs then the host aint there...period.
                   */
                  if (lpFSM->bSendMsg == MSG_SAY_HI)
                     lpFSM->uiState = STATE_ONLINE_FATALABORT ;
                  else
                     lpFSM->uiState = STATE_ONLINE_ERROR ;

                  break ;
               }

               /*
                * Check the input buffer for our ack.
                */
               if (GetInBYTE( &bInChar ))
               {

#ifdef TEST_FAILURE
/*** Take this out ****/
#pragma message( "HACK ALERT!!! Calling IsRButtonDown()" )
if (IsRButtonDown()) bInChar = ASCII_NAK ;
#endif

                  switch( bInChar )
                  {

                     case ASCII_ACK:
                        lpFSM->uiLastState = lpFSM->uiState ;
                        lpFSM->uiState = STATE_ONLINE_IDLE ;
                        ResetTimeout( &lpFSM->Timeout ) ;

                        DP3( hWDB, "Got ACK!!!!! Yay!" ) ;
                     
                     break ;

                     // BUGFIX 0021 5/30/92 CEK 
                     case ASCII_NAK:
                        /* If we get a NAK it means abort
                         * immediately, no need to try to recover.
                         */

                        lpFSM->uiExitCode = EXITCODE_NAK ;
                        lpFSM->uiLastState = lpFSM->uiState ;
                        ResetTimeout( &lpFSM->Timeout ) ;
                        lpFSM->uiState = STATE_ONLINE_FATALABORT ;

                     break ;
                     // BUGFIX 0021 5/30/92 CEK 

                     default:
                        DP3( hWDB, "Got something other than an ACK!" ) ;

                        lpFSM->uiExitCode = EXITCODE_ACK_BAD ;
                        lpFSM->uiLastState = lpFSM->uiState ;
                        ResetTimeout( &lpFSM->Timeout ) ;
                        lpFSM->uiState = STATE_ONLINE_ERROR ;

                        /*
                         * trust me on this one.  We don't want
                         * timer messages to stop comming in
                         */
                        return EXITCODE_OK ;
                     break ;
                  }
               }
            }
            break ;

            case STATE_ONLINE_WAITRECEIVE:
            DP4( hWDB, "STATE_ONLINE_WAITRECEIVE" ) ;
            {
               BYTE  bInChar ;
               DWORD  cbIn ;
               DWORD  cbRequired ;

               SetOnlineStatusMsg( rglpsz[IDS_RECEIVINGDATA] ) ; 

               /*
                * If we ever enter this state without a timeout
                * set it means that we are entering it for the
                * first time.  So set the timout timer...
                */
               if (!InTimeout( lpFSM->Timeout ))
               {
                  StartTimeout( &lpFSM->Timeout, uiWaitReceiveSecs ) ;
                  break ;
               }

               /*
                * We're waiting for the bbs to send us a buffer that
                * is lpFSM->cbReceive bytes long.
                */
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  /* We've timed out!  Bummer.
                   *
                   */                    
                  DP1( hWDB, "Timeout in STATE_ONLINE_WAITRECEIVE!!!" ) ;

                  lpFSM->uiExitCode = EXITCODE_RECEIVE_TIMEOUT ;
                  lpFSM->uiLastState = lpFSM->uiState ;
                  ResetTimeout( &lpFSM->Timeout ) ;
                  lpFSM->uiState = STATE_ONLINE_ERROR ;
                  /*
                   * trust me on this one.  We don't want
                   * timer messages to stop comming in
                   */
                  return EXITCODE_OK ;
               }

               /*
                * Check the input buffer to see if we've gotten all
                * the bytes we're waiting for.  If this block is
                * supposed to be CRC checked then add the size of
                * the CRC word to the amount of data.
                */
               cbIn = PeekInBYTE( &bInChar ) ;
               if (lpFSM->fReceiveCRC)
                  cbRequired = lpFSM->cbReceive + sizeof(CRC) ;
               else
                  cbRequired = lpFSM->cbReceive ;

               if (cbIn >= cbRequired)
               {
                  GetInBuffer( lpFSM->lpReceive, lpFSM->cbReceive ) ;

                  if (lpFSM->fReceiveCRC)
                  {
                     CRC   crcIn ;

                     GetInBuffer( &crcIn, sizeof(CRC) ) ;

#ifdef TEST_FAILURE
/*** Take this out ****/
#pragma message( "HACK ALERT!!! Calling IsRButtonDown()" )
                     if (IsRButtonDown() || (crcIn != CalcCRC( lpFSM->lpReceive, lpFSM->cbReceive )))
#else
                     if (crcIn != CalcCRC( lpFSM->lpReceive, lpFSM->cbReceive ))
#endif
                     {
                        DP1( hWDB, "CRC check failed on receiving buffer" ) ;
                        lpFSM->uiLastState = lpFSM->uiState ;
                        lpFSM->uiExitCode = EXITCODE_RECEIVE_CRC ;
                        ResetTimeout( &lpFSM->Timeout ) ;
                        lpFSM->uiState = STATE_ONLINE_ERROR ;
                        
                        /*
                         * trust me on this one.  We don't want
                         * timer messages to stop comming in
                         */
                        return EXITCODE_OK ;
                     }

                     DP3( hWDB, "Got buffer (CRC ok!)." ) ;
                  }

                  /*
                   * Un pad if neccessary  PADCHAR == '~'
                   */
                  if (lpFSM->fReceivePad)
                  {
                     LPBYTE lp ;

                     lpFSM->lpReceive[lpFSM->cbReceive] = '\0';
                     if (lp = _fstrchr( lpFSM->lpReceive, PADCHAR ))
                         *lp = '\0';
                  }

                  lpFSM->uiLastState = lpFSM->uiState ;
                  lpFSM->uiState = STATE_ONLINE_IDLE ;
                  ResetTimeout( &lpFSM->Timeout ) ;
               }
            }
            break ;

            case STATE_ONLINE_CONXPRO_RECEIVE:
            {
               LPARAM   lParam1 = NULL ;
               LPARAM   lParam2 = NULL ;
               LRESULT  lResult = NULL ;

               SetOnlineStatusMsg( NULL ) ;

               /*
                *  Cause Zmodem() do go to the next state by
                * calling it with the ZM_TIMER command.  Note that
                * the first time we come through here we are
                * will cause ZM_TIMER to return failure because
                * ZM_INIT has not happened yet.  Zmodem() is
                * robust, however, so this is cool
                */
               Zmodem( hwnd, ZM_TIMER, 0L, 0L ) ;

               switch(lResult = Zmodem( hwnd, ZM_QUERYSTATUS, lParam1, lParam2 ))
               {
                  case ZMS_IDLE:

                     DP2( hWDB, "ZMS_IDLE" ) ;

                     SetOnlineStatusMsg( rglpsz[IDS_CONXPRORECEIVE] ) ;

                     /*
                      * This means there is no CONXPRO transfer
                      * in progress, and thus we should initiate
                      * the one specified...
                      *
                      * lpFSM->lpReceive points to the list of files we
                      * are receiveing.  This MUST be a doubly-null
                      * terminated list!  It can be a destination dir too.
                      *
                      * lpFSM->cbReceive is the length of the
                      * filename buffer.
                      *
                      * if cbSend is non-zero then lpFSM->lpSend points
                      * to a WORD that specifies the id of the file to
                      * download.
                      *
                      * We assume here that the other side has
                      * initated the CONXPRO send.
                      */
                     if (lpFSM->cbSend != 0)
                     {
                        /*
                         * Send the Exe ID (a WORD), then
                         * go onto normal Zmodem receive
                         */
                        SetTTYBinary( TRUE ) ;
                        WriteCommBlock( lpFSM->lpSend, lpFSM->cbSend ) ;
                        SetTTYBinary( FALSE ) ;
                     }

                     /*
                      * Next, tell our comm routines to stop putting data
                      * into our InputBuffer and to call the Zmodem( ZM_RXBLK )
                      * command whenever it gets new comm data.
                      */
//                     TellCommToDoZmodem( hwnd ) ;

                     /*
                      * Initialize the Zmodem() session by passing
                      * our send callback address and by specifying
                      * no timer (lparam1 == 0).
                      */
//                     Zmodem( hwnd, ZM_SHOWSTATUS, 0L, 0L ) ;

                     if (ZMS_OK != Zmodem( hwnd, ZM_INIT, (LPARAM)lpfnZmodemSend, 0L ))
                     {
                        DP1( hWDB, "ZM_INIT failed!" ) ;

                        lpFSM->uiLastState = lpFSM->uiState ;
                        lpFSM->uiExitCode = EXITCODE_RECEIVE_FILE ;
                        lpFSM->uiState = STATE_ONLINE_ERROR ;
                        break ;
                     }

                     if (lpFSM->lpReceive)
                     {
                        DP3( hWDB, "Calling ZM_RECEIVE with a dest of %s", (LPSTR)lpFSM->lpReceive ) ;
                     }
                     else
                     {
                        DP3( hWDB, "Calling ZM_RECEIVE with NO dest" ) ;
                     }

                     if (ZMS_OK != Zmodem( hwnd, ZM_RECEIVE, (LPARAM)lpFSM->lpReceive, 0L ))
                     {
                        DP1( hWDB, "ZM_RECEIVE failed" ) ;

                        Zmodem( hwnd, ZM_KILL, 0L, 0L ) ;
                        TellCommToDoZmodem( NULL ) ;

                        lpFSM->uiLastState = lpFSM->uiState ;
                        lpFSM->uiExitCode = EXITCODE_RECEIVE_FILE ;
                        lpFSM->uiState = STATE_ONLINE_ERROR ;
                        break ;
                     }

                     ShowOnlineDownloadStuff( hwnd, TRUE ) ;
                  break ;

                  case ZMS_SENDING:
                     /*
                      * This means we are in the middle of a send transfer.
                      * This should never happen here 'cause we're receive!
                      */
                     DP1( hWDB, "ZMS_SENDING in ONLINE_RECEIVE!!!" ) ;


                  case ZMS_OK:
                  case ZMS_RECEIVING:
                     /*
                      * After we've started a CONXPRO receive we'll get this
                      * status message back until the receive is complete...
                      */

                     /*
                      * Update dialog box  here
                      */
                     UpdateOnlineDialog( hwnd ) ;

                  break ;

                  case ZMS_FILECOMPLETED:

                     DP2( hWDB, "ZMS_FILECOMPLETED" ) ;
                     UpdateOnlineDialog( hwnd ) ;

                     /*
                      * This indicates that one of the files we requested has
                      * completed successfully.  This status is a lot like
                      * ZMS_RECIEVING in that a transfer is still in progress.
                      *
                      * When we get this status, lParam point to te name of
                      * the file that was received.  We then tell the protocol
                      * that we understand, and thank you, by using the
                      * ZM_ACK_FILECOMPLETED command.
                      */
                     Zmodem( hwnd, ZM_ACK_FILECOMPLETED, lParam1, 0L ) ;

                     SetDlgItemText( hwnd, IDD_ESTTIME, "00:00:00" ) ;

                     dwPercent = 10000 ;
                     InvalidateRect( GetDlgItem( hwnd, IDD_GASGAUGE ), NULL, FALSE ) ;
                     UpdateWindow( GetDlgItem( hwnd, IDD_GASGAUGE ) ) ;

                  break ;

                  case ZMS_COMPLETED:
                     DP2( hWDB, "ZMS_COMPLETED" ) ;

                     /*
                      * This status indicates that all of the files we requested
                      * have been received a-ok!
                      */

                     Zmodem( hwnd, ZM_ACK_COMPLETED, 0L, 0L ) ;

                     Zmodem( hwnd, ZM_KILL, 0L, 0L ) ;
                     TellCommToDoZmodem( NULL ) ;

                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiExitCode = EXITCODE_OK ;
                     lpFSM->uiState = STATE_ONLINE_IDLE ;

                     SetDlgItemText( hwnd, IDD_ESTTIME, "00:00:00" ) ;

                     dwPercent = 10000 ;
                     InvalidateRect( GetDlgItem( hwnd, IDD_GASGAUGE ), NULL, FALSE ) ;

                     EnableOnlineDownloadStuff( hwnd, FALSE ) ;
                  break ;

                  case ZMS_ABORTING:
                     SetOnlineStatusMsg( rglpsz[IDS_CONXPROABORTING] ) ;
                  break ;

                  case ZMS_ABORTED:
                  case ZMS_ERROR:
                  {
                     UINT uiLastErr = (UINT)Zmodem( hwnd, ZM_GET_LASTERROR,
                                                    0L, 0L ) ;

                     /*
                      * This means the transfer was aborted (i.e. the
                      * protocol detected an abort situation).
                      */

                     DPF1( hWDB, "Zmodem Error or Aborted! uiLastErr = %04X : ",
                                 uiLastErr ) ;

                     Zmodem( hwnd, ZM_KILL, 0L, 0L ) ;
                     TellCommToDoZmodem( NULL ) ;

                     EnableOnlineDownloadStuff( hwnd, FALSE ) ;

                     lpFSM->uiLastState = lpFSM->uiState ;

                     switch( uiLastErr )
                     {
                        case ZME_DISKERROR:
                        case ZME_DISKFULL:
                           DP1( hWDB, "DISKERROR or DISKFULL" ) ;
                           lpFSM->uiExitCode = EXITCODE_DISKSPACE ;
                           lpFSM->uiState = STATE_ONLINE_ERROR ;
                        break ;

                        case ZME_HOSTCANCEL:
                           DP1( hWDB, "HOSTCANCELed" ) ;
                           lpFSM->uiExitCode = EXITCODE_HOSTCANCEL ;
                           lpFSM->uiState = STATE_ONLINE_ERROR ;
                        break ;

                        case ZME_LOCALCANCEL:
                           DP1( hWDB, "LOCALCANCEL" ) ;
                           lpFSM->uiExitCode = EXITCODE_RECEIVE_FILE ;
                           lpFSM->uiState = STATE_ONLINE_IDLE ;
                        break ;

                        default:
                           DP1( hWDB, "GENERAL" ) ;
                           lpFSM->uiExitCode = EXITCODE_RECEIVE_FILE ;
                           lpFSM->uiState = STATE_ONLINE_ERROR ;
                        break ;
                     }
                  }
                  break ;

                  default:
                     DP1( hWDB, "Unknown ZMS_ value (0x%08lX) returned by ZM_QUERYSTATUS!",
                              (DWORD)lResult ) ;
                     Zmodem( hwnd, ZM_KILL, 0L, 0L ) ;
                     TellCommToDoZmodem( NULL ) ;

                     lpFSM->uiLastState = lpFSM->uiState ;
                     lpFSM->uiExitCode = EXITCODE_RECEIVE_FILE ;
                     lpFSM->uiState = STATE_ONLINE_ERROR ;

 //                     ShowOnlineDownloadStuff( hwnd, TRUE ) ;
                  break ;
               }
            }
            break ;

            case STATE_ONLINE_ERROR:
               /*
                * This state will be entered when an error in
                * communication occurs.
                *
                * Before this state is entered, the previous
                * state saved the current message information off
                * into the fsmErrorRecovery member of the ONLINEFSM
                * structure.  We'll use this information to
                * resend the message.
                */
               DP3( hWDB, "STATE_ONLINE_ERROR: Resends = %d, uiLastState = %04X",
                  lpFSM->uiReSendAttempts, lpFSM->uiLastState ) ;

               switch( lpFSM->uiLastState )
               {
                  case STATE_ONLINE_WAITACK:
                     SetOnlineStatusMsg( rglpsz[IDS_COMMERROR] ) ; 
                     /*
                      * !!! NEED !!!
                      *
                      * Look at uiExitCode to see what kind of send
                      * error we have.  For instance we could have a
                      * timeout or we could have gotten junk
                      */
                     lpszLastError = rglpsz[IDS_INVALIDSEND] ;
                  break ;

                  case STATE_ONLINE_WAITRECEIVE:
                     SetOnlineStatusMsg( rglpsz[IDS_COMMERROR] ) ; 
                     /*
                      * !!! NEED !!!
                      *
                      * Look at uiExitCode to see what kind of send
                      * error we have.  For instance we could have a
                      * timeout or we could have had a bad CRC.
                      */
                     lpszLastError = rglpsz[IDS_INVALIDRECEIVE] ;
                  break ;

                  case STATE_ONLINE_CONXPRO_RECEIVE:
                     lpFSM->uiReSendAttempts = uiMaxReSendAttempts ;
                     lpszLastError = rglpsz[IDS_FILETRANSFERFAILED] ;
                  break ;

                  default:
                     SetOnlineStatusMsg( rglpsz[IDS_COMMERROR] ) ; 
                     lpszLastError = "General Error (huh?)" ;
                  break ;
               }

               uiNumErrors++ ;

               if (lpFSM->uiReSendAttempts < uiMaxReSendAttempts)
               {
                  lpFSM->uiReSyncAttempts = 0 ;

                  lpFSM->uiState = STATE_ONLINE_RESYNC ;
               }
               else
               {
                  /*
                   * No more allowed.  Sorry fella.
                   */
                  DP3( hWDB, "   Surpassed %d re-send attempts", uiMaxReSendAttempts ) ;

                  lpFSM->uiState = STATE_ONLINE_FATALABORT ;
               }
            break ;

            case STATE_ONLINE_RESYNC:
               DP3( hWDB, "STATE_ONLINE_RESYNC: ReSyncs = %d", lpFSM->uiReSyncAttempts ) ;

               SetOnlineStatusMsg( rglpsz[IDS_RESYNCING] ) ; 

               /*
                * This state is entered by the STATE_ONLINE_ERROR
                * state.  Its purpose is to resynchronize CONXSRV
                * with the bbs.  Once we've synched back up we'll
                * go try to send the message again...
                */
               if (lpFSM->uiReSyncAttempts < uiMaxReSyncAttempts)
               {
                  BYTE b = MSG_SAY_HI ;

                  SetTTYBinary( TRUE ) ;

                  /*
                   * Reset comm
                   */
                  FlushInComm() ;
                  FlushOutComm() ;

                  WriteCommBlock( &b, sizeof( b ) ) ;
                  SetTTYBinary( FALSE ) ;

                  StartTimeout( &lpFSM->Timeout, uiReSyncWaitAckSecs ) ;
                  lpFSM->uiState = STATE_ONLINE_RESYNC_WAITACK ;
               }
               else
               {
                  DP3( hWDB, "   Surpassed %d re-sync attempts", uiMaxReSyncAttempts ) ;
                  /*
                   * No more allowed.  Sorry fella.
                   */
                  lpFSM->uiState = STATE_ONLINE_FATALABORT ;
               }
            break ;

            case STATE_ONLINE_RESYNC_WAITACK:
            DP3( hWDB, "STATE_ONLINE_RESYNC_WAITACK" ) ;
            {
               BYTE  bInChar ;
               SetOnlineStatusMsg( NULL ) ;

               /*
                * We're waiting for the bbs to ACKnowledge the last
                * command that was sent.  
                */
               if (IsTimeoutOver( &lpFSM->Timeout ))
               {
                  DP3( hWDB, "Timeout in STATE_ONLINE_RESYNC_WAITACK!!!" ) ;

                  lpFSM->uiExitCode = EXITCODE_ACK_TIMEOUT ;
                  ResetTimeout( &lpFSM->Timeout ) ;
                  lpFSM->uiState = STATE_ONLINE_RESYNC ;
                  lpFSM->uiReSyncAttempts++ ;
                  break ;
               }

               /*
                * Check the input buffer for our ack.
                */
               if (GetInBYTE( &bInChar ))
               {
                  switch( bInChar )
                  {

                     case ASCII_ACK:
                        ResetTimeout( &lpFSM->Timeout ) ;

                        /*
                         * COOL!  We re-synched!  Send the message
                         * again!
                         */
                        DP2( hWDB, "Re-Sync Successful!!!  Re-Sending msg..." ) ;

                        SetOnlineStatusMsg( rglpsz[IDS_RESENDING] ) ; 
                        lpFSM->uiState = STATE_ONLINE_SENDMSG ;
                        lpFSM->uiExitCode = EXITCODE_OK ;
                        lpFSM->uiNextState = lpFSM->uiSavedNextState ;
                        lpFSM->uiReSendAttempts++ ;
                     
                     break ;

                     // BUGFIX 0021 5/30/92 CEK 
                     case ASCII_NAK:
                        /* If we get a NAK it means abort
                         * immediately, no need to try to recover.
                         */
                        lpFSM->uiExitCode = EXITCODE_NAK ;
                        lpFSM->uiLastState = lpFSM->uiState ;
                        ResetTimeout( &lpFSM->Timeout ) ;
                        lpFSM->uiState = STATE_ONLINE_FATALABORT ;

                     break ;
                     // BUGFIX 0021 5/30/92 CEK 

                     default:
                        DP3( hWDB, "Got something other than an ACK!" ) ;

                        lpFSM->uiExitCode = EXITCODE_ACK_BAD ;
                        ResetTimeout( &lpFSM->Timeout ) ;
                        lpFSM->uiState = STATE_ONLINE_RESYNC ;
                        lpFSM->uiReSyncAttempts++ ;
                     break ;
                  }
               }
            }
            break ;

            case STATE_ONLINE_FATALABORT:
               /*
                * This state is entered by the ONLINE_ERROR state
                * if error recovery could not happen.
                *
                * The peek message loop is looking for this
                * state, and exits when it sees it.
                */
            break ;

            default:
               DP1( hWDB, "Yikes!  Undefined state in OnlineFSM" ) ;
               DASSERTMSG( hWDB, 0, "Yikes!  Undefined state in OnlineFSM" ) ;
               RIP_CITY( 0 ) ;
            break ;
         }

      }
      break ;

   }

   return EXITCODE_OK ;

/*
 * Failure handler.  If we come down here it's because of some sort
 * of fatal error such as out of memory or unable to read
 * config.
 */
Failure:

   DP1( hWDB, "RIP_CITY! uiIDerr = %d", uiIDerr ) ;

   /*
    * I hate goto's too, but they are great for failure handling
    */
   if (lpFSM)
   {
      GlobalFreePtr( lpFSM ) ;

      REMOVE_FSM( hwnd ) ;

   }

   EndOnlineDialog() ;

   /*
    * Just in case
    */
   TellCommToDoZmodem( NULL ) ;

   if (lpfnZmodemSend)
   {
      FreeProcInstance( (FARPROC)lpfnZmodemSend ) ;
      lpfnZmodemSend = NULL ;
   }

   /*
    * Put up a nasty error message.
    */
   if (uiIDerr)
   {
      /*
       * Use IDS_ERRXXXX format
       */
      ErrorResBox( hwndClientApp, NULL, MB_OK | MB_ICONSTOP,
                   IDS_ERR_COMMUNICATIONS, uiIDerr ) ;
   }
   
   return EXITCODE_GENERALFAILURE ;

} /* OnlineFSM()  */


/****************************************************************
 *  UINT CALLBACK fnZmodemSend( HWND hwnd, LPBYTE lp, UINT cb )
 *
 *  Description: 
 *
 *    This is the Zmodem() send callback.  It is called by the
 *    CONXPRO protocol when it wants to send data.  See
 *    CONXPRO.DOC for more info.
 *
 *  Comments:
 *
 ****************************************************************/
UINT CALLBACK fnZmodemSend( HWND hwnd, LPBYTE lp, UINT cb )
{
   DP3( hWDB, "fnZmodemSend( %d )", cb ) ;
   return WriteCommBlockNoEcho( lp, cb ) ;

} /* fnZmodemSend()  */


/****************************************************************
 *  VOID NEAR PASCAL SendData( LPONLINEFSM lpFSM )
 *
 *  Description: 
 *
 *    Called from one of the SENDATA states.  Sends the lpSend
 *    buffer, doing CRC stuff and padding as appropriate.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL SendData( LPONLINEFSM lpFSM )
{
   if (lpFSM->cbSend)
   {
      LPBYTE   lpSend = lpFSM->lpSend ;

      /*
       * We know there isn't supposed to be anything in the
       * input queue...so just make sure
       */
      FlushInComm() ;

      if (lpSend == NULL)
      {
         DP1( hWDB, "NULL send buffer pointer in SendData!!!!" ) ;
         return ;
      }

      /* Pad if needed...
       */
      if (lpFSM->fSendPad)
      {
         UINT len ;

         if (lpSend = GlobalAllocPtr( GHND, lpFSM->cbSend + 2 ))
         {
            _fmemcpy( lpSend, lpFSM->lpSend, LOWORD(lpFSM->cbSend) ) ;
            len = lstrlen( lpSend ) ;

            _fmemset( lpSend + len, PADCHAR, LOWORD(lpFSM->cbSend) - len ) ;
         }
         else
         {
            DP1( hWDB, "GlobalAlloc failed in SendData!!!" ) ;
            return ;
         }
      }

      /* CRC check if needed...
       */
      if (lpFSM->fSendCRC)
      {
         CRC   crc ;
         crc = CalcCRC( lpSend, lpFSM->cbSend ) ;
         WriteCommBlock( lpSend, lpFSM->cbSend ) ;

#ifdef TEST_FAILURE
#pragma message( "HACK ALERT!!! Calling IsRButtonDown()" )
if (IsRButtonDown())
   crc++ ;
#endif

         WriteCommBlock( &crc, sizeof( crc ) ) ;
      }
      else
         WriteCommBlock( lpSend, lpFSM->cbSend ) ;

      if (lpSend != lpFSM->lpSend)
         GlobalFreePtr( lpSend ) ;

   }

} /* SendData()  */

/************************************************************************
 * End of File: online.c
 ***********************************************************************/

