/************************************************************************
 *      Module:  comm.c
 *
 *     Remarks:  Low-level Comm functions.  Derived from the 3.1
 *               TTY sample.
 *
 *   Revisions:  
 *          3/4/92      cek     Wrote it.
 *          10/28/92    cek     Moved to sample.
 *
 ***********************************************************************/

/****************************************************************
 *  BOOL WINAPI ProcessCOMMNotification( UINT iCid, LONG lParam )
 *
 *  Description: 
 *
 *    Called by whatever window proc is currently recieving
 *    comm notifications.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ProcessCOMMNotification( UINT iCid, LONG lParam )
{
   #ifdef DEBUG
   static     int nZM = 0 ;
   #endif

   HCURSOR    hOldCursor, hWaitCursor ;
   BOOL       f ;
   int        nLength, n ;
   COMSTAT    ComStat ;
   MSG        msg ;
   char       szBuf[80] ;

   DASSERT( hWDB, lpReadBuf ) ;

   /* If it's a CN_EVENT update the status bar
    */

   if (((CN_EVENT & LOWORD( lParam )) == CN_EVENT) &&
       (*lpEvtWord & (EV_RLSD | EV_CTS | EV_DSR)))
   {
      DP3( hWDB, "CN_EVENT...setting stat bar" ) ;

      if (*lpMSRShadow & ACE_RLSD)
         StatusSetStatBox( hwndStat, IDSTAT_CD, rglpsz[IDS_CD] ) ;
      else
         StatusSetStatBox( hwndStat, IDSTAT_CD, "" ) ;

      if (*lpMSRShadow & ACE_DSR)
         StatusSetStatBox( hwndStat, IDSTAT_DSR, rglpsz[IDS_DSR] ) ;
      else
         StatusSetStatBox( hwndStat, IDSTAT_DSR, "" ) ;

      if (*lpMSRShadow & ACE_CTS)
         StatusSetStatBox( hwndStat, IDSTAT_CTS, rglpsz[IDS_CTS] ) ;
      else
         StatusSetStatBox( hwndStat, IDSTAT_CTS, "" ) ;

      /* Clear the event so we get more.
       */
      GetCommEventMask( iCid, EV_RLSD | EV_CTS | EV_DSR ) ;
   }

   if (fUseCN_RECEIVE == TRUE)
   {
      if ((CN_RECEIVE & LOWORD( lParam )) == CN_RECEIVE)
      {
         do
         {
            if (nLength = ReadCommBlock( (LPVOID) lpReadBuf, uiMaxBlock ))
            {
               /*
                * Now take the data that we've just read and
                * put it into our own little circular buffer (or if we've
                * got a Zmodem() transfer going on, just give it to
                * Zmodem.
                */
               if (hwndZmodem == NULL)
               {
                  WriteTTYBlock( hwndTTY, (LPVOID) lpReadBuf, nLength,
                                 READCOMM_COLOR ) ;
                  UpdateWindow( hwndTTY ) ;

                  for (n = 0 ; n < nLength ; n++)
                     if (!PutInBYTE( lpReadBuf[n] ))
                     {
                        DP1( hWDB, "Overrun condition calling PutInBYTE()" ) ;
                     }
               }
               else
               {
                  if (ZMS_OK != Zmodem( hwndZmodem, ZM_RXBLK,
                                       MAKELPARAM( nLength, 0 ),
                                       (LPARAM)(LPSTR)lpReadBuf ))
                  {
                     DP1( hWDB, "Zmodem( ZM_RXBLK ) failed in ProcessCommNotification!" ) ;
                  }
               }

               dwcbReceive += nLength ;
            }

            if (n = GetCommError( iCid, &ComStat )) 
            {
               DP1( hWDB, "CE-%s", (LPSTR)CommErrString( n ) ) ;

               wsprintf( szBuf, "[%s]", CommErrString( n ) ) ;
               f = SetTTYBinary( FALSE ) ;
               WriteTTYBlock( hwndTTY, szBuf, lstrlen( szBuf ),
                              COMMERROR_COLOR ) ;
               SetTTYBinary( f ) ;
            }


         }
         while ((!PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE )) ||
               (ComStat.cbInQue >= uiMaxBlock)) ;
      }
   }
   else // don't use CN_RECEIVE
   {
      /* This is really disgusting.  Most of this crap is needed
       * because the COMM driver in Win 3.1 is such a peice of shit.
       *
       * First of all, by default we are not using CN_RECEIVE notifcation
       * because of the bug in the driver mentioned above.  So we have
       * to use CN_EVENT/RXCHAR notifications.  That's what we're handling
       * here.
       *
       * On a fast machine (i.e. a 486/33) we can use CN_EVENT/RXCHAR
       * notifications without too much trouble.  We coalesce WM_COMMNOTIFY
       * messages in our PeekMessage() loop in BBS.C to get good smooth
       * multi-tasking.
       *
       * Unfortunately on a slower machines (i.e. even a 386/33) we tend
       * to get CE_OVERRUN errors while doing the Zmodem bit.
       *
       * By default this thing is setup to multi-task smoothly...we'll get
       * CE_OVERRUNS on slow hardware, but Zmodem catches these well and
       * recovers.  But for some people this will not be adequate, thus the
       * "NoYieldOnSlowMachines" flag.  If this flag is 1, then we may not
       * yield at high baud rates (we stay in the do...while loop for the
       * duration the the Zmodem transfer).
       */

      DPF5( hWDB, "0x%04X:", *lpEvtWord ) ;

      // verify that it is a COMM event specified by our mask

      if (((CN_EVENT & LOWORD( lParam )) == CN_EVENT) &&
          (*lpEvtWord & EV_RXCHAR))
      {
         /* Show the hourglass cursor if we won't be yeilding...
          */
         if (fNoYieldOnSlowMachines &&
             hwndZmodem &&
             uiBaudRate >= CBR_4800 )
         {
            hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
            hOldCursor = SetCursor( hWaitCursor ) ;
         }

         /* Clear the flag first.  Because if it is after, we might
          * clear an EV_RXCHAR that occured after we got 0 back from
          * ReadComm()
          */
         GetCommEventMask( iCid, EV_RXCHAR ) ;

         /* We loop here since it is highly likely that the buffer
          * can been filled while we are reading this block.  This
          * is especially true when operating at high baud rates
          * (e.g. >= 9600 baud).
          */
         do
         {
            if ((nLength = ReadCommBlock( (LPVOID) lpReadBuf, uiMaxBlock )) > 0)
            {
               if (hwndZmodem == NULL)
               {
                  WriteTTYBlock( hwndTTY, (LPVOID) lpReadBuf, nLength,
                                 READCOMM_COLOR ) ;
                  UpdateWindow( hwndTTY ) ;

                  for (n = 0 ; n < nLength ; n++)
                     if (!PutInBYTE( lpReadBuf[n] ))
                     {
                        DP1( hWDB, "Overrun condition calling PutInBYTE()" ) ;
                     }
               }
               else
               {
                  if (ZMS_OK != Zmodem( hwndZmodem, ZM_RXBLK,
                                       MAKELPARAM( nLength, 0 ),
                                       (LPARAM)(LPSTR)lpReadBuf ))
                  {
                     DP1( hWDB, "Zmodem( ZM_RXBLK ) failed in ProcessCommNotification!" ) ;
                  }

                  if (fNoYieldOnSlowMachines)
                  {
                     /* Update the download info */
                     UpdateOnlineDialog( hwndZmodem ) ;
                  }
               }

               DPF5( hWDB, "%d ", nLength ) ;

               dwcbReceive += nLength ;
            }

            /* Since we do a lot of processing between the call
             * to ReadCommBlock() above and here, we need to
             * GetCommError().  If there is a CommError and we don't
             * do this here, we may never get another WM_COMMNOTIFY!
             */
            while (n = GetCommError( iCommDevice, NULL ))
            {
               DP1( hWDB, "CE-%s", (LPSTR)CommErrString( n ) ) ;

               wsprintf( szBuf, "[%s]", CommErrString( n ) ) ;
               f = SetTTYBinary( FALSE ) ;
               WriteTTYBlock( hwndTTY, szBuf, lstrlen( szBuf ),
                              COMMERROR_COLOR ) ;
               SetTTYBinary( f ) ;
            }
         }
         while (fNoYieldOnSlowMachines == TRUE && (nLength > 0)) ;

         if (fNoYieldOnSlowMachines &&
             hwndZmodem &&
             uiBaudRate >= CBR_4800 )
            SetCursor( hOldCursor ) ;

      }
      DP5( hWDB, "" ) ;
   }

   wsprintf( szBuf, "%lu", dwcbReceive ) ;
   StatusSetStatBox( hwndStat, IDSTAT_RECEIVE, szBuf ) ;

   return ( TRUE ) ;

} /* ProcessCOMMNotification()  */



/****************************************************************
 *  BOOL WINAPI OpenConnection( )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI OpenConnection( )
{
   BOOL       fRetVal ;
   HCURSOR    hOldCursor, hWaitCursor ;

   // show the hourglass cursor
   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   uiMaxBlock = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                          rglpsz[IDS_INI_MAXBLOCK],
                                          MAXBLOCK,
                                          rglpsz[IDS_INI_FILENAME] ) ;

   fUseCN_RECEIVE = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                          rglpsz[IDS_INI_USE_CN_RECEIVE],
                                          USE_CN_RECEIVE,
                                          rglpsz[IDS_INI_FILENAME] ) ;

   fNoYieldOnSlowMachines = GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                          rglpsz[IDS_INI_NOYIELDONSLOWMACHINES],
                                          NO_YIELD_ON_SLOW_MACHINES,
                                          rglpsz[IDS_INI_FILENAME] ) ;

   if (fUseCN_RECEIVE == TRUE)
      uiEventMask = EVENT_MASK_CN_RECEIVE ;
   else
      uiEventMask = EVENT_MASK_RXCHAR ;

   uiRXQueue = ((UINT)1024 * (UINT)GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                     rglpsz[IDS_INI_RXQUEUE],
                                     RXQUEUE,
                                     rglpsz[IDS_INI_FILENAME] )) ;

   uiTXQueue = ((UINT)1024 * (UINT)GetPrivateProfileInt( rglpsz[IDS_INI_MAIN],
                                     rglpsz[IDS_INI_TXQUEUE],
                                     TXQUEUE,
                                     rglpsz[IDS_INI_FILENAME] )) ;

   /* BUG!!!! in Windows 3.1 COMM.DRV:
    *
    * RX/TX queue size must be less than 32K.  OpenComm will NOT
    * fail if it is greater than or equal to 32K but nothing will
    * work.
    *
    * Force uiRX/TXQueue to be at most one less than 32K...
    */
   uiRXQueue = min( (UINT)((UINT)32 * 1024) - 1, uiRXQueue ) ;
   uiTXQueue = min( (UINT)((UINT)32 * 1024) - 1, uiTXQueue ) ;


   DP1( hWDB, "   RXQueueSize = %u, TXQueueSize = %u, uiMaxBlock = %u",
                       uiRXQueue, uiTXQueue, uiMaxBlock ) ;

   #ifdef DEBUG
   if (fUseCN_RECEIVE)
      DP1( hWDB, "            Using CN_RECEIVE notifications.  Could CRASH Win 3.10!!!" ) ;
   else
      DP1( hWDB, "            Using CN_EVENT (RXCHAR) notifications." ) ;
   #endif

   #ifdef DEBUG
   if (fNoYieldOnSlowMachines)
      DP1( hWDB, "            NoYieldOnSlowMachines = 1 : Might not yield on slow hw." ) ;
   else
      DP1( hWDB, "            NoYieldOnSlowMachines = 0 : Might drop chars on slow hw." ) ;
   #endif

   // Allocate our read buffer
   if (!(lpReadBuf = GlobalAllocPtr( GHND, uiMaxBlock + 1)))
   {
      DP1( hWDB, "GlobalAllocPtr() of input buffer failed!!!" ) ;
      return FALSE ;
   }

   // open COMM device
   if ((iCommDevice = OpenComm( szPort, uiRXQueue, uiTXQueue )) < 0)
   {
      DP1( hWDB, "OpenComm( %s, ... ) failed!", (LPSTR)szPort ) ;
      iCommDevice = IE_BADID ;
      return FALSE ;
   }

   fRetVal = SetupConnection( FALSE ) ;

   if (fRetVal)
   {
      fCommConnected = TRUE ;

      lpEvtWord = ((LPWORD)SetCommEventMask( iCommDevice, uiEventMask )) ;

      DASSERT( hWDB, lpEvtWord ) ;

      lpMSRShadow = (LPBYTE)lpEvtWord + DCB_MSRSHADOW ;

      if (*lpMSRShadow & ACE_RLSD)
         StatusSetStatBox( hwndStat, IDSTAT_CD, rglpsz[IDS_CD] ) ;
      else
         StatusSetStatBox( hwndStat, IDSTAT_CD, "" ) ;

      if (*lpMSRShadow & ACE_DSR)
         StatusSetStatBox( hwndStat, IDSTAT_DSR, rglpsz[IDS_DSR] ) ;
      else
         StatusSetStatBox( hwndStat, IDSTAT_DSR, "" ) ;

      if (*lpMSRShadow & ACE_CTS)
         StatusSetStatBox( hwndStat, IDSTAT_CTS, rglpsz[IDS_CTS] ) ;
      else
         StatusSetStatBox( hwndStat, IDSTAT_CTS, "" ) ;

      SetCommNotificationWnd( hwndTTY ) ;

      // assert DTR

      EscapeCommFunction( iCommDevice, SETDTR ) ;

      SetTTYFocus( hwndTTY ) ;

   }
   else
   {
      DP1( hWDB, "SetupConnection() failed" ) ;
      fCommConnected = FALSE ;
      CloseComm( iCommDevice ) ;
      iCommDevice = IE_BADID ;
   }

   // restore cursor
   SetCursor( hOldCursor ) ;

   dwcbSend = 0 ;
   StatusSetStatBox( hwndStat, IDSTAT_SEND, "0" ) ;

   dwcbReceive = 0 ;
   StatusSetStatBox( hwndStat, IDSTAT_RECEIVE, "0" ) ;
   
   StatusSetStatBox( hwndStat, IDSTAT_PORT, szPort ) ;
   StatusSetStatBox( hwndStat, IDSTAT_BAUD, szBaudBuf ) ;
   StatusSetStatBox( hwndStat, IDSTAT_CONNECT,  rglpsz[IDS_CONNECT] ) ;
   return fRetVal ;

} // end of OpenConnection()


/****************************************************************
 *  BOOL NEAR PASCAL SetupConnection( BOOL fXonXoff )
 *
 *  Description: 
 *
 *    Changes the connection settings.  if fXonXoff is TRUE
 *    XON XOFF is enabled, otherwise it's disabled.  It needs to
 *    be on for Zmodem, off otherwise.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL SetupConnection( BOOL fXonXoff )
{
   BOOL       fRetVal ;
   DCB        dcb ;

   GetCommState( iCommDevice, &dcb ) ;

   dcb.BaudRate = uiBaudRate ;
   dcb.ByteSize = 8 ;
   dcb.Parity = NOPARITY ;
   dcb.StopBits = ONESTOPBIT ;

   dcb.fChEvt = FALSE ;

   // setup hardware flow control

   dcb.fOutxDsrFlow = dcb.fDtrflow = FALSE ;
   dcb.DsrTimeout = 0 ;

   dcb.fOutxCtsFlow = dcb.fRtsflow = TRUE ;
   dcb.CtsTimeout = 30 ;

   // setup software flow control

   dcb.fInX = dcb.fOutX = fXonXoff ;
   dcb.XonChar = ASCII_XON ;
   dcb.XoffChar = ASCII_XOFF ;
   dcb.XonLim = 100 ;
   dcb.XoffLim = 100 ;

   // other various settings

   dcb.fBinary = TRUE ;
   dcb.fParity = TRUE ;
   dcb.fRtsDisable = FALSE ;
   dcb.fDtrDisable = FALSE ;

   fRetVal = !(SetCommState( &dcb ) < 0) ;

   return ( fRetVal ) ;

} /* SetupConnection()  */


BOOL WINAPI CloseConnection( VOID )
{
   if (fCommConnected == FALSE)
      return TRUE ;
   // kill the focus

   KillTTYFocus( hwndTTY ) ;

   EnableCommNotification( iCommDevice, NULL, -1, -1 ) ;


   EscapeCommFunction( iCommDevice, SETXON ) ;  // if XOFF we might hang

   /* Flush just to make sure */
   DP5( hWDB, "Flushing transmit queue..." ) ;
   FlushComm( iCommDevice, 0 ) ;
   DP5( hWDB, "Flushing receive queue..." ) ;
   FlushComm( iCommDevice, 1 ) ;

   // drop DTR
   EscapeCommFunction( iCommDevice, CLRDTR ) ;

   /* HACK!  There is (another) bug in the 3.10 COMM driver where
    * it is possible to hang on CloseComm().  This has something
    * to do with doing a CLRDTR (above) and the way the COMM driver
    * does some funky loop back stuff.  I don't fully understand it
    * (Bryan Woodruff of MS Developer Support does) but I do know
    * that the work around is to wait a half second or so after
    * clearing the DTR
    */
#pragma message("NEED : Silly timing loop here" ) 

   /* Flush just to make sure */
   DP5( hWDB, "Flushing transmit queue..." ) ;
   FlushComm( iCommDevice, 0 ) ;
   DP5( hWDB, "Flushing receive queue..." ) ;
   FlushComm( iCommDevice, 1 ) ;

   // close comm connection

   DP5( hWDB, "Closing connection (CloseComm())..." ) ;
   CloseComm( iCommDevice ) ;
   DP5( hWDB, "   Back from CloseComm()" ) ;

   iCommDevice = IE_BADID ;
   fCommConnected = FALSE ;

   StatusSetStatBox( hwndStat, IDSTAT_PORT, "" ) ;
   StatusSetStatBox( hwndStat, IDSTAT_BAUD, "" ) ;
   StatusSetStatBox( hwndStat, IDSTAT_CD, "" ) ;
   StatusSetStatBox( hwndStat, IDSTAT_DSR, "" ) ;
   StatusSetStatBox( hwndStat, IDSTAT_CTS, "" ) ;
   StatusSetStatBox( hwndStat, IDSTAT_CONNECT,rglpsz[IDS_NOTCONNECT] ) ;

   if (lpReadBuf != NULL)
   {
      GlobalFreePtr( lpReadBuf ) ;
      lpReadBuf = NULL ;
   }

   return ( TRUE ) ;

} // end of CloseConnection()


UINT WINAPI ProcessSendBBSMsg( LPGENERICMSG lpMsg )
{
   MSG            msg ; // for peekmessage loop
   BOOL           fDispatchNotify = FALSE ;
   MSG            msgNotify ; // for coelescing
   LPARAM         lTemp ;

   for (;;)
   {
      if (!fUseCN_RECEIVE && fCoalesce)
         memset( &msgNotify, '\0', sizeof( msg ) ) ;

      while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE))
      {
         if ((!fUseCN_RECEIVE && fCoalesce) && (msg.message == WM_COMMNOTIFY))
         {
            lTemp = msgNotify.lParam ;
            msgNotify = msg ;
            msgNotify.lParam |= lTemp ;

            fDispatchNotify = TRUE ;
         }
         else
         {
   //         TranslateMessage( &msg ) ;
            DispatchMessage( &msg ) ;
         }

         /* Do processing, and look for your 'exit' case
          */


         if ( EXIT CASE is TRUE )
         {
            if ((!fUseCN_RECEIVE && fCoalesce) && fDispatchNotify)
                  DispatchMessage( &msgNotify ) ;

            return TRUE ;
         }

      }

      if ((!fUseCN_RECEIVE && fCoalesce) && fDispatchNotify)
      {
         DispatchMessage( &msgNotify ) ;
         fDispatchNotify = FALSE ;
      }

      /*
       * All well behaved peekmessage loops should really be
       * waitmessage loops, but most folks don't realize that.
       * I do.
       */
      WaitMessage() ;
   }

} /* ProcessSendBBSMsg()  */


/************************************************************************
 * End of File: comm.c
 ***********************************************************************/

