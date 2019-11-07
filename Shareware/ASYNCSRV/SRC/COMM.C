/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASYNCSRV
 *
 *      Module:  comm.c
 *
 *     Remarks:  Low-level Comm functions.  Derived from the 3.1
 *               TTY sample.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "ASYNCSRV.h"
#include "botstat.h"
#include "bbs.h"
#include "online.h"
#include "..\..\inc\CONXPRO.h"

/* HACK ALERT!!! There is a bug in the Windows 3.10 COMM driver when
 * CN_RECEIVE notifications are used.
 *
 * Essentially what happens is at high speeds, with a lot of data
 * coming in (i.e. a 9600 baud ZMODEM transfer) it is possible
 * for the COMM.DRV to royally fuck up.  In fact, it fucks up so
 * badly that the machine typically reboots!  This by itself is not
 * too bad (ha!).  Unfortunately, while we're downloading we
 * have a file open which we're writing to.  SmartDrv 4.0 does write
 * caching.  Thus the machine reboots with data still in
 * SmartDrv's write cache...and that's an easy way to trash someone's
 * disk in an ugly way (I know...it did it to mine several times).
 *
 * ASYNCSRV will work much better with the following #define enabled.
 * But until we can be sure that the user has the fixed
 * 3.10 COMM.DRV on his system we have to disable it.
 *
 * If USE_CN_RECEIVE is #defined as 1 the code here will use CN_RECEIVE
 * notifications.  Otherwise it will simply use CN_EVENT notifications
 * with the EV_RXCHAR event.
 *
 * The value of this define is the default value for the fUseCN_RECEIVE
 * flag.  fUseCN_RECEIVE can be changed through the CONXION.INI flag
 * "UseCN_RECEIVE".  UseCN_RECEIVE=1 means use CN_RECEIVE notifications,
 * UseCN_RECEIVE=0 means don't
 *
 * IMPORTANT (cek 7/17/92):  After this hack was implemented, it was
 * realized that NOT using CN_RECEIVES actually improves performance
 * and response.  So, by default USE_CN_RECEIVE is FALSE.
 */
BOOL        fUseCN_RECEIVE = USE_CN_RECEIVE ;


/* On slow hardware (25MHz 386 and below) at high baud rates
 * (2400 baud and above), setting the [Comm Settings] entry
 * 'NoYield' to 1 may improve zmodem performance
 * and will eliminate the chance of comm overruns.  The downfall is that
 * we never yield, thus no indication is given that things are working and
 * the user cannot multitask.
 *
 * By default this setting is 0.  This setting has no effect when
 * CN_RECEIVE notifications are used (UseCN_RECEIVE = 1).
 */
BOOL        fNoYieldOnSlowMachines = NO_YIELD_ON_SLOW_MACHINES ;

BOOL        gfIgnoreCarrierDetect = FALSE ; // set in modem.ini

BOOL        fCommConnected ; // used in tty.c

static DCB         dcb ;
static char        szPort[] = "COM1:" ;
static char        szBaudBuf[] = "384000 Baud" ;
static UINT        uiBaudRate ;
static LPBYTE      lpMSRShadow ;
static LPWORD      lpEvtWord ;

static DWORD       dwcbSend ;          // total bytes sent
static DWORD       dwcbReceive ;       // total bytes rec'd

static HWND        hwndZmodem = NULL ; // if not NULL there's a zmodem
                                       // transfer in effect

static UINT        uiRXQueue ;         // RX queue size (see ASYNCSRV.H)
static UINT        uiTXQueue ;         // TX queue size (see ASYNCSRV.H)
static UINT        uiMaxBlock = MAXBLOCK ;  // duh

static LPBYTE      lpReadBuf = NULL ;  // GlobalAlloc'd block

/*
 * The EVENT_MASK is used by Set/GetCommEventMask()
 */
static UINT       uiEventMask ;

#define EVENT_MASK_CN_RECEIVE   EV_RLSD | EV_CTS | EV_DSR 
#define EVENT_MASK_RXCHAR       EV_RLSD | EV_CTS | EV_DSR | EV_RXCHAR

BOOL NEAR PASCAL SetupConnection( BOOL fXonXoff ) ;
LPSTR NEAR PASCAL CommErrString( int n ) ;

BYTE    grgInputBuffer[INPUTBUFFER_SIZE] ;
PBYTE   gpInputBufferFirst, gpInputBufferNext ;

/****************************************************************
 *  BOOL WINAPI SetCommNotificationWnd( hwnd )
 *
 *  Description: 
 *
 *    Enables comm notifications for a specific window.
 *
 ****************************************************************/
BOOL WINAPI SetCommNotificationWnd( HWND hwnd )
{
   BOOL f ;

   ResetInputBuffer() ;

   if (iCommDevice < 0)
      return FALSE ;

   if (hwnd)
   {
      lpEvtWord = SetCommEventMask( iCommDevice, uiEventMask ) ;

      if (fUseCN_RECEIVE == TRUE)
         f = EnableCommNotification( iCommDevice, hwnd, uiMaxBlock, -1 ) ;
      else
         f = EnableCommNotification( iCommDevice, hwnd, -1, -1 ) ;
   }
   else
   {
      lpEvtWord = SetCommEventMask( iCommDevice, uiEventMask ) ;

      f = EnableCommNotification( iCommDevice, NULL, -1, -1 ) ;
   }

   return f ;

} /* SetCommNotificationWnd()  */

/****************************************************************
 *  BOOL WINAPI TellCommToDoZmodem( HWND hwnd )
 *
 *  Description: 
 *
 *    This function tells the comm stuff that it should stop
 *    putting data in the input buffer and instead send it on
 *    to Zmodem() via the ZM_RXBLK command.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI TellCommToDoZmodem( HWND hwnd )
{
   if (hwndZmodem = hwnd)
   {
      DWORD cb = PeekInBYTE( NULL ) ;
      LPBYTE lp ;

      SetupConnection( TRUE ) ;

      SetTTYBinary( TRUE ) ;

      /* If there's anything currently in the input buffer,
       * give it to Zmodem().
       */
      if (cb && (lp = GlobalAllocPtr( GHND, cb )))
      {
         DP1( hWDB, "TellCommToDoZmodem() : Data in input buffer (%d bytes).  Doing ZM_RXBLK!", cb );
         GetInBuffer( lp, cb ) ;
         Zmodem( hwnd, ZM_RXBLK, (LPARAM)cb, (LPARAM)lp ) ;
         GlobalFreePtr( lp ) ;
      }
   }
   else
   {
      FlushInComm(  ) ;
      SetupConnection( FALSE ) ;
   }


   return TRUE ;

} /* TellCommToDoZmodem()  */

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
 *  BOOL WINAPI FlushInComm()
 *
 *  Description: 
 *
 *    Flushes the input queue
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI FlushInComm( VOID )
{
   ResetInputBuffer() ;

   return FlushComm( iCommDevice, 1 ) ;

} /* FlushInComm()  */

/****************************************************************
 *  FlushOutComm()
 *
 *  Description: 
 *
 *    Flushes the output queue.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI FlushOutComm( VOID )
{
   return FlushComm( iCommDevice, 0 ) ;

} /* FlushOutComm()  */

/****************************************************************
 *  UINT WINAPI GetInBYTE( LPBYTE lpb )
 *
 *    Get's the next input character from our input buffer.
 *    Increments the current pointer to the next character in
 *    the buffer.
 *
 *    Buffer is circular so this function will wrap if
 *    neccessary.
 *
 *    The return value is the number of bytes in the input
 *    buffer at the time of the call.  Thus if there
 *    is nothing in the input buffer this function
 *    returns 0.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD WINAPI GetInBYTE( LPBYTE lpb )
{
   DWORD cb ;

   if (gpInputBufferFirst == gpInputBufferNext)
      return 0 ;

   /*
    * Check to see if we've wrapped around
    */
   if (gpInputBufferFirst > gpInputBufferNext)
      cb = INPUTBUFFER_SIZE - (gpInputBufferFirst - gpInputBufferNext) ;
   else
      cb = gpInputBufferNext - gpInputBufferFirst ;

   if (lpb)
      *lpb = *gpInputBufferFirst ;

   /*
    * Nuke the one we just read.
    */
   *gpInputBufferFirst == '\0' ;

   gpInputBufferFirst++ ;

   if (gpInputBufferFirst >= (grgInputBuffer + INPUTBUFFER_SIZE))
      gpInputBufferFirst = grgInputBuffer ;

   return cb ;

} /* GetInBYTE()  */

/****************************************************************
 *  DWORD WINAPI GetInBuffer( LPVOID lpBuf, DWORD cb )
 *
 *  Description: 
 *
 *    Gets cb bytes from the input buffer and places them into
 *    lpBuf.
 *
 *    returns the number of bytes actually copied.
 *
 ****************************************************************/
DWORD WINAPI GetInBuffer( LPVOID lpBuf, DWORD cb )
{
   DWORD  n ;
   DWORD  cbRead = 0;

   for (n = 0 ; n < cb; n++)
      cbRead += GetInBYTE( (LPBYTE)lpBuf + n ) ;

   return cbRead ;

} /* GetInBuffer()  */

/****************************************************************
 *  DWORD WINAPI PeekInBYTE( LPBYTE lpb )
 *
 *    Returns in lpb the next input character from our input buffer.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD WINAPI PeekInBYTE( LPBYTE lpb )
{
   DWORD  cb ;

   if (gpInputBufferFirst == gpInputBufferNext)
      return 0 ;

   /*
    * Check to see if we've wrapped around
    */
   if (gpInputBufferFirst > gpInputBufferNext)
      cb = INPUTBUFFER_SIZE - (gpInputBufferFirst - gpInputBufferNext) ;
   else
      cb = gpInputBufferNext - gpInputBufferFirst ;

   if (lpb)
      *lpb = *gpInputBufferFirst ;

   return cb ;

} /* PeekInBYTE()  */

/****************************************************************
 *  BOOL WINAPI PutInBYTE( BYTE b )
 *
 *  Description: 
 *
 *    Puts a byte into our input buffer.  This function is
 *    called primarliy by the ProcessCOMMnotifcation() function
 *    in COMM.C.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI PutInBYTE( BYTE b )
{
   /*
    * Check to see if this byte will cause an overrun.
    * if so bail.
    */
   if (gpInputBufferNext == grgInputBuffer + INPUTBUFFER_SIZE)
   {
      if (gpInputBufferFirst == grgInputBuffer)
         return FALSE ;
   }
   else
      if (gpInputBufferNext+1 == gpInputBufferFirst)
         return FALSE ;

   *gpInputBufferNext = b ;

   if (gpInputBufferNext == grgInputBuffer + INPUTBUFFER_SIZE)
      gpInputBufferNext == grgInputBuffer ;
   else
      gpInputBufferNext++ ;

   return TRUE ;

} /* PutInBYTE()  */

/****************************************************************
 *  VOID WINAPI ResetInputBuffer( VOID )
 *
 *  Description: 
 *
 *    Resets the input buffer to empty.    
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI ResetInputBuffer( VOID )
{
   UINT i ;

   gpInputBufferNext = gpInputBufferFirst = grgInputBuffer ;

   for (i = 0 ; i < INPUTBUFFER_SIZE ; i++)
      grgInputBuffer[i] = '\0' ;

} /* ResetInputBuffer()  */

/****************************************************************
 *  BOOL WINAPI SetCommPort( LPSTR lpszPort )
 *
 *  Description: 
 *
 *    Sets the port to the passed value.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI SetCommPort( LPSTR lpszPort )
{
   lstrcpy( szPort, lpszPort ) ;

   return TRUE ;

} /* SetCommPort()  */

/****************************************************************
 *  BOOL     WINAPI SetBaudRate( DWORD dwBaud )
 *
 *  Description:
 *
 *    If we're connected this will change the baud rate for
 *    the connection.  Otherwise it just sets it up for the next
 *    time we OpenComm().
 *
 ****************************************************************/
BOOL     WINAPI SetBaudRate( DWORD dwBaud )
{
   wsprintf( szBaudBuf, "%ld Baud", dwBaud ) ;

   switch (dwBaud)
   {
      case 110:
         uiBaudRate = CBR_110 ;
      break ;

      case 300:
         uiBaudRate = CBR_300 ;
      break ;

      case 600:
         uiBaudRate = CBR_600 ;
      break ;

      case 1200:
         uiBaudRate = CBR_1200;
      break ;

      case 2400:
         uiBaudRate = CBR_2400;
      break ;

      case 4800:
         uiBaudRate = CBR_4800;
      break ;

      case 9600:
         uiBaudRate = CBR_9600;
      break ;

      case 14400:
         uiBaudRate = CBR_14400;
      break ;

      case 19200:
         uiBaudRate = CBR_19200;
      break ;

      case 38400:
         uiBaudRate = CBR_38400;
      break ;

      case 56000:
         uiBaudRate = CBR_56000;
      break ;

      case 128000:
         uiBaudRate = CBR_128000 ;
      break ;

      case 256000:
         uiBaudRate = CBR_256000 ;
      break ;

   }

   if (iCommDevice > 0)
   {
      DCB        dcb ;

      GetCommState( iCommDevice, &dcb ) ;

      dcb.BaudRate = uiBaudRate ;

      StatusSetStatBox( hwndStat, IDSTAT_BAUD, szBaudBuf ) ;

      return !(SetCommState( &dcb ) < 0) ;
   }

   return TRUE ;


} /* SetBaudRate()  */


/****************************************************************
 *  DWORD   WINAPI GetBaudRate( VOID )
 *
 *  Description: 
 *
 ****************************************************************/
DWORD WINAPI GetBaudRate( VOID )
{
   DWORD dwBaud ;

   switch (uiBaudRate)
   {
      case CBR_110:
         dwBaud = 110 ;
      break ;

      case CBR_300:
         dwBaud = 300 ;
      break ;

      case CBR_600:
         dwBaud = 600 ;
      break ;

      case CBR_1200:
         dwBaud = 1200;
      break ;

      case CBR_2400:
         dwBaud = 2400;
      break ;

      case CBR_4800:
         dwBaud = 4800;
      break ;

      case CBR_9600:
         dwBaud = 9600;
      break ;

      case CBR_14400:
         dwBaud = 14400;
      break ;

      case CBR_19200:
         dwBaud = 19200;
      break ;

      case CBR_38400:
         dwBaud = 38400;
      break ;

      case CBR_56000:
         dwBaud = 56000;
      break ;

      case CBR_128000:
         dwBaud = 128000 ;
      break ;

      case CBR_256000:
         dwBaud = 256000 ;
      break ;

      default:
         dwBaud = 2400 ;
      break ;

   }

   return dwBaud ;

} /* SetBaudRate()  */

/****************************************************************
 *  int WINAPI GetCommID(VOID)
 *
 *  Comments:
 *
 ****************************************************************/
int WINAPI GetCommID(VOID)
{
   return iCommDevice ;

} /* GetCommID()  */

/****************************************************************
 *  BOOL WINAPI IsOnline()
 *
 *  Description: 
 *
 *    Fun little function that uses the msrshadow to determine
 *    whether we are actually online with some remote system
 *    or not.
 *
 *    !!! Question !!!
 *
 *    Are there any modems that do not give us the right
 *    indication?  If so we're screwed unless we add something
 *    to the MODEM.INI file that would make this
 *    function alwasy return TRUE.
 *
 ****************************************************************/
BOOL WINAPI IsOnline()
{
   if (iCommDevice < 0)
      return FALSE ;

   if (gfIgnoreCarrierDetect || (lpMSRShadow && (*lpMSRShadow & ACE_RLSD)))
      return TRUE ;

   return FALSE ;

} /* IsOnline()  */


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

/****************************************************************
 *  CRC WINAPI CalcCRC( LPVOID lpBuf, DWORD  cb )
 *
 *  Description: 
 *
 *  Calculate the cyclic redundancy check on an array of unsigned chars
 *  code ripped off from Galacticom XMODEM-CRC Protocol pg 183 'The Major BBS'
 *
 *  Comments:
 *
 ****************************************************************/
CRC WINAPI CalcCRC( LPVOID lpBuf, DWORD  cb )
{
   CRC   crc = 0 ;
   UINT  i  ;    

   while(cb--)
   {
      crc = crc ^ (CRC)*((LPBYTE)lpBuf)++ << 8 ;

      for(i = 8 ; i-- ; )
         if(crc & 0x08000) 
            crc = (crc << 1)^0x1021 ;
         else
            crc = crc << 1 ;
   }

   return crc ;

} /* CalcCRC()  */


//---------------------------------------------------------------------------
//  int WINAPI ReadCommBlock( LPVOID lpBlock, int nMaxLength )
//
//  Description:
//     Reads a block from the COM port and stuffs it into
//     the provided block.
//
//  Parameters:
//     HWND hwndTTY
//        handle to TTY window
//
//     LPVOID lpBlock
//        block used for storage
//
//     int nMaxLength
//        max length of block to read
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

int WINAPI ReadCommBlock( LPVOID lpBlock, int nMaxLength )
{
   char       szError[ 32 ] ;
   int        nLength, nError ;
   BOOL       f ;

   if (iCommDevice < 0)
      return FALSE ;

   nLength = ReadComm( iCommDevice, lpBlock, nMaxLength ) ;

   if (nLength < 0)
   {
      nLength *= -1 ;
      while (nError = GetCommError( iCommDevice, NULL ))
      {
         DP1( hWDB, "CE-%s", CommErrString( nError ) ) ;

         wsprintf( szError, "[%s]", CommErrString( nError ) ) ;
         f = SetTTYBinary( FALSE ) ;
         WriteTTYBlock( hwndTTY, szError, lstrlen( szError ),
                        COMMERROR_COLOR ) ;
         SetTTYBinary( f ) ;
      }
   }

   return ( nLength ) ;

} // end of ReadCommBlock()


/****************************************************************
 *  BOOL WINAPI WriteCommByte( BYTE bByte )
 *
 *  Description: 
 *
 *    Writes a byte to the comm port without echoing it to
 *    the tty window.  Should only be used by the
 *    ProcessTTYCaracter() fn in tty.c
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI WriteCommByte( BYTE bByte )
{
   char szBuf[16] ;
   COMSTAT ComStat ;

   if (iCommDevice < 0)
      return FALSE ;

   GetCommError( iCommDevice, &ComStat ) ;
   while( uiTXQueue - ComStat.cbOutQue < 1 )
   {
      DP1( hWDB, "TXQueue full!" ) ;
      GetCommError( iCommDevice, &ComStat ) ;
   }

   WriteComm( iCommDevice, (LPSTR) &bByte, 1 ) ;

   wsprintf( szBuf, "%lu", ++dwcbSend ) ;
   StatusSetStatBox( hwndStat, IDSTAT_SEND, szBuf ) ;

   return ( TRUE ) ;
} /* WriteCommByte()  */


/****************************************************************
 *  BOOL WINAPI WriteCommBlock( LPVOID lpBlock, DWORD  cb )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI WriteCommBlock( LPVOID lpBlock, DWORD  cb )
{
   char szBuf[16] ;
   COMSTAT ComStat ;

   if (iCommDevice < 0)
   {
      DP1( hWDB, "WriteCommBlock: Invalid iCommDevice!" ) ;
      return FALSE ;
   }

   GetCommError( iCommDevice, &ComStat ) ;
   while( (uiTXQueue - ComStat.cbOutQue) < cb )
   {
      DP1( hWDB, "TXQueue full!" ) ;
      GetCommError( iCommDevice, &ComStat ) ;
   }

   WriteComm( iCommDevice, lpBlock, LOWORD(cb) ) ;

   WriteTTYBlock( hwndTTY, lpBlock, LOWORD(cb), WRITECOMM_COLOR ) ;

   dwcbSend += cb ;
   wsprintf( szBuf, "%lu", dwcbSend ) ;
   StatusSetStatBox( hwndStat, IDSTAT_SEND, szBuf ) ;

   return TRUE ;

} /* WriteCommBlock()  */


/****************************************************************
 *  BOOL WINAPI WriteCommBlockNoEcho( LPVOID lpBlock, DWORD  cb )
 *
 *  Description: 
 *
 *
 *
 ****************************************************************/
BOOL WINAPI WriteCommBlockNoEcho( LPVOID lpBlock, DWORD  cb )
{
   char szBuf[16] ;
//   COMSTAT ComStat ;

   if (iCommDevice < 0)
   {
      DP1( hWDB, "WriteCommBlockNoEcho: Invalid iCommDevice!" ) ;
      return FALSE ;
   }

   #if 0
   GetCommError( iCommDevice, &ComStat ) ;
   while( (uiTXQueue - ComStat.cbOutQue) < cb )
   {
      DP1( hWDB, "TXQueue full!" ) ;
      GetCommError( iCommDevice, &ComStat ) ;
   }
   #endif

   WriteComm( iCommDevice, lpBlock, LOWORD(cb) ) ;

   dwcbSend += cb ;
   wsprintf( szBuf, "%lu", dwcbSend ) ;
   StatusSetStatBox( hwndStat, IDSTAT_SEND, szBuf ) ;

   return TRUE ;

} /* WriteCommBlockNoEcho()  */

LPSTR NEAR PASCAL CommErrString( int n )
{
   static char szErr[16] ;

   switch( n )
   {
      case CE_RXOVER:
         return rglpsz[IDS_CE_RXOVER] ;

      case CE_OVERRUN:
         return rglpsz[IDS_CE_OVERRUN] ;

      case CE_RXPARITY:
         return rglpsz[IDS_CE_RXPARITY] ;

      case CE_FRAME:
         return rglpsz[IDS_CE_FRAME] ;

      case CE_BREAK:
         return rglpsz[IDS_CE_BREAK] ;

      case CE_CTSTO:
         return rglpsz[IDS_CE_CTSTO] ;

      case CE_DSRTO:
         return rglpsz[IDS_CE_DSRTO] ;

      case CE_RLSDTO:
         return rglpsz[IDS_CE_RLSDTO] ;

      case CE_TXFULL:
         return rglpsz[IDS_CE_TXFULL] ;

      case CE_PTO:
         return rglpsz[IDS_CE_PTO] ;

      case CE_IOE:
         return rglpsz[IDS_CE_IOE] ;

      case CE_DNS:
         return rglpsz[IDS_CE_DNS] ;

      case CE_OOP:
         return rglpsz[IDS_CE_OOP] ;

      case CE_MODE:
         return rglpsz[IDS_CE_MODE] ;

      default:
         wsprintf( szErr, "%04X", n ) ;
         return szErr ;
   }
}


/************************************************************************
 * End of File: comm.c
 ***********************************************************************/

