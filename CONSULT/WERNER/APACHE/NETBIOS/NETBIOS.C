/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  netbios.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  NetBIOS Communications DLL
 *
 *   Revisions:  
 *     01.00.000  3/ 6/91 baw   Initial version, initial build
 *
 ************************************************************************/

#define GLOBALDEFS  // for global declarations

#include "netbios.h"

/************************************************************************
 *  int FAR PASCAL GetConnectorUseCount( VOID )
 *
 *  Description: 
 *     Returns the usage count of this module.  This is the number
 *     of open connections using this module.
 *
 *  Comments:
 *      9/21/91  baw  Added this comment.
 *
 ************************************************************************/

int FAR PASCAL GetConnectorUseCount( VOID )
{
   return ( nUseCount ) ;

} /* end of GetConnectorUseCount() */

/************************************************************************
 *  HANDLE FAR PASCAL CreateCCB( HWND hWnd )
 *
 *  Description: 
 *     Creates and initializes a Connector Control Block (CCB).
 *     Returns NULL if unsuccessful.
 *
 *  Comments:
 *      9/21/91  baw  Wrote this comment.
 *
 ************************************************************************/

HANDLE FAR PASCAL CreateCCB( HWND hWnd )
{
   HANDLE  hCCB ;
   NPCCB   npCCB ;

   if (NULL == (hCCB =  LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                    sizeof( CCB ) )))
      return ( NULL ) ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
   {
      LocalFree( hCCB ) ;
      return ( NULL ) ;
   }

   // perform default initialization

   _fmemset( SERVER( npCCB ), NULL, MAXLEN_NCBNAMESIZE ) ;
   _fmemset( LOCAL( npCCB ), NULL, MAXLEN_NCBNAMESIZE ) ;

   BLANKPADDING( npCCB ) = FALSE ;
   SERVERMODE( npCCB ) = FALSE ;
   CONNECTED( npCCB ) = FALSE ;
   EXTENSION( npCCB ) = NETEXT_UNGERMANBASS ;
   TXTIMEOUT( npCCB ) = 30 ;
   RXTIMEOUT( npCCB ) = 30 ;
   READNCB( npCCB ) = NULL ;
   WRITENCB( npCCB ) = NULL ;
   PARENTWND( npCCB ) = hWnd ;

   // allocate transmit / receive buffers

   if (NULL ==
       (TXBLOCK( npCCB ) = GlobalAlloc( DATA_GMEMFLAGS, MAXLEN_XMTBLOCK )))
   {
      LocalUnlock( hCCB ) ;
      LocalFree( hCCB ) ;
      return ( NULL ) ;
   }

   if (NULL ==
       (RXBLOCK( npCCB ) = GlobalAlloc( DATA_GMEMFLAGS, MAXLEN_RECBLOCK )))
   {
      GlobalFree( TXBLOCK( npCCB ) ) ;
      LocalUnlock( hCCB ) ;
      LocalFree( hCCB ) ;
      return ( NULL ) ;
   }

   LocalUnlock( hCCB ) ;

   return ( hCCB ) ;

} /* end of CreateCCB() */

/************************************************************************
 *  BOOL FAR PASCAL DestroyCCB( HANDLE hCCB )
 *
 *  Description: 
 *     Destroys the CCB.
 *
 *  Comments:
 *      9/21/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL DestroyCCB( HANDLE hCCB )
{
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (TXBLOCK( npCCB ) != NULL)
      GlobalFree( TXBLOCK( npCCB ) ) ;
   if (RXBLOCK( npCCB ) != NULL)
      GlobalFree( RXBLOCK( npCCB ) ) ;

   LocalUnlock( hCCB ) ;

   return ( NULL == LocalFree( hCCB ) ) ;

} /* end of DestroyCCB() */

/************************************************************************
 *  BOOL FAR PASCAL OpenConnectorInit( HANDLE hCCB )
 *
 *  Description: 
 *     This begins the open connector function.  This function
 *     posts a notification message when the connection has been
 *     established.  OpenConnectorComplete() should be called in
 *     response to a successful notification.
 *
 *     If this function returns TRUE, a notification will be sent.
 *
 *  Comments:
 *      9/21/91  baw  Broke OpenConnector() into two stages
 *
 ************************************************************************/

BOOL FAR PASCAL OpenConnectorInit( HANDLE hCCB )
{
   HNCB   hConnectNCB ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (CONNECTED( npCCB ) || !NBCheckAvail())
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // setup an NCB to establish session

   if (NULL == (hConnectNCB = NBCreateNCB( PARENTWND( npCCB ),
                                           LAN_ADAPTER_PRIMARY,
                                           TRUE, (LPVOID) PostNetBIOS )))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // store the connection NCB

   CONNECTNCB( npCCB ) = hConnectNCB ;

   // establish the connection

   // this function will simply hang out a call or listen and will
   // post a notification to the window when the connection has been
   // established

   if (0 != NBCreateSession( hConnectNCB, (LPSTR) SERVER( npCCB ),
                             (LPSTR) LOCAL( npCCB ),
                             EXTENSION( npCCB ), RXTIMEOUT( npCCB ),
                             TXTIMEOUT( npCCB ), SERVERMODE( npCCB ) ))
   {
      NBDestroyNCB( hConnectNCB ) ;
      LocalUnlock( hCCB ) ;

      DP1( hWDB, "NBCreateSession() failed." ) ;

      return ( FALSE ) ;
   }

   DP5( hWDB, "OpenConnectorInit() returning TRUE." ) ;

   LocalUnlock( hCCB ) ;

   // bump use count

   nUseCount++ ;

   return ( TRUE ) ;

} /* end of OpenConnectorInit() */

/************************************************************************
 *  BOOL FAR PASCAL OpenConnectorComplete( HANDLE hCCB )
 *
 *  Description:
 *     This function should be called when the CE_CONNECT notification
 *     is receive by the parent window.
 *
 *  Comments:
 *      9/28/91  baw  Broke OpenConnector() into two stages
 *
 ************************************************************************/

BOOL FAR PASCAL OpenConnectorComplete( HANDLE hCCB )
{
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   // store LSN and set connection flag

   LSN( npCCB ) = NBGetLSN( CONNECTNCB( npCCB ) ) ;
   CONNECTED( npCCB ) = TRUE ;

   // we can destroy the CONNECT NCB now

   NBDestroyNCB( CONNECTNCB( npCCB ) ) ;
   CONNECTNCB( npCCB ) = NULL ;

   // create Read / Write NCBs

   // Asynchronous and posts a message

   if (NULL == (READNCB( npCCB ) = NBCreateNCB( PARENTWND( npCCB ),
                                                LAN_ADAPTER_PRIMARY,
                                                TRUE, (LPVOID) PostNetBIOS )))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // Asynchronous, but does not post a message.
   // This means wait for the response w/ PeekMessage loop!

   if (NULL == (WRITENCB( npCCB ) = NBCreateNCB( PARENTWND( npCCB ),
                                                 LAN_ADAPTER_PRIMARY,
                                                 TRUE, NULL )))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // post receive request

   NBRequestReceive( READNCB( npCCB ), LSN( npCCB ), RXBLOCK( npCCB ),
                     EXTENSION( npCCB ) ) ;

   // store NCB handle

   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of OpenConnectorComplete() */

/************************************************************************
 *  BOOL FAR PASCAL CloseConnector( HANDLE hCCB )
 *
 *  Description: 
 *     Closes the connection.
 *
 *  Comments:
 *      9/28/91  baw  Added this comment.
 *
 ************************************************************************/

BOOL FAR PASCAL CloseConnector( HANDLE hCCB )
{
   NPCCB  npCCB ;

   // bump use count

   nUseCount-- ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (!CONNECTED( npCCB ))
   {
      // already disconnected

      LocalUnlock( hCCB ) ;
      return( TRUE ) ;
   }

   // create a temporary NCB

   if (NULL == (CONNECTNCB( npCCB ) = NBCreateNCB( hWnd, LAN_ADAPTER_PRIMARY,
                                                   TRUE, NULL )))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // destroy connection

   if (0 != NBDestroySession( CONNECTNCB( npCCB ), LOCAL( npCCB ),
                              LSN( npCCB ) ))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // destroy NCBs

   NBDestroyNCB( READNCB( npCCB ) ) ;
   NBDestroyNCB( WRITENCB( npCCB ) ) ;
   NBDestroyNCB( CONNECTNCB( npCCB ) ) ;

   LocalUnlock( hCCB ) ;
   return ( TRUE ) ;

} /* end of CloseConnector() */

/************************************************************************
 *  BOOL SetupConnector( HANDLE hCCB )
 *
 *  Description: 
 *     Sets the connection parameters as defined in the CCB.
 *     Returns TRUE if no error.
 *
 *  Comments:
 *      9/28/91  baw  Added this comment.
 *
 ************************************************************************/

BOOL SetupConnector( HANDLE hCCB )
{
   return ( TRUE ) ;

} /* end of SetupConnector() */

/************************************************************************
 *  int FAR PASCAL ReadConnectorBlock( HANDLE hCCB, HWND hWnd,
 *                                     int nMaxBytes, LPSTR lpBlock )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

int FAR PASCAL ReadConnectorBlock( HANDLE hCCB, HWND hWnd,
                                   int nMaxBytes, LPSTR lpBlock )
{
   WORD   wLength ;
   HNCB   hReadNCB ;
   NPCCB  npCCB ;
   LPNCB  lpReadNCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( -1 ) ;

   if (!CONNECTED( npCCB ))
   {
      LocalUnlock( hCCB ) ;
      return ( -1 ) ;
   }

   hReadNCB = READNCB( npCCB ) ;
   if (NULL == (lpReadNCB = (LPNCB) GlobalLock( hReadNCB )))
   {
      LocalUnlock( hCCB ) ;
      return ( -1 ) ;
   }

   switch (lpReadNCB -> bReturn)
   {
      case NBE_NO_ERROR:
         break ;

      case NBE_PENDING:
         LocalUnlock( hCCB ) ;
         return ( -1 ) ;

      case NBE_SESSION_CLOSED:

         // post WM_EVENT_DISCONNECT to parent

         LocalUnlock( hCCB ) ;
         return ( -1 ) ;

      default:
         // an error occurred - post another receive request
         // and return a failure

         GlobalUnlock( hReadNCB ) ;

         // free the receive block (locked by NBRequestReceive)

         GlobalUnlock( RXBLOCK( npCCB ) ) ;

         // post another receive request

         NBRequestReceive( hReadNCB, LSN( npCCB ), RXBLOCK( npCCB ),
                           EXTENSION( npCCB ) ) ;
         LocalUnlock( hCCB ) ;
         return ( -1 ) ;
   }

   wLength = lpReadNCB -> wLength ;

   if (wLength > (WORD) nMaxBytes)
   {
      _fmemcpy( (LPSTR) lpBlock, (LPSTR) (lpReadNCB -> lpBuffer), nMaxBytes ) ;
      _fmemmove( (LPSTR) (lpReadNCB -> lpBuffer),
                 ((LPSTR) (lpReadNCB -> lpBuffer) + nMaxBytes),
                 (int) wLength - nMaxBytes ) ;
      lpReadNCB -> wLength = wLength - (WORD) nMaxBytes ;
      GlobalUnlock( hReadNCB ) ;
      LocalUnlock( hCCB ) ;
      return ( nMaxBytes ) ;
   }
   else
   {
      _fmemcpy( (LPSTR) lpBlock, (LPSTR) (lpReadNCB -> lpBuffer),
                (int) wLength ) ;
      GlobalUnlock( hReadNCB ) ;

      // free the receive block (locked by NBRequestReceive)

      GlobalUnlock( RXBLOCK( npCCB ) ) ;

      // post another receive request

      NBRequestReceive( hReadNCB, LSN( npCCB ), RXBLOCK( npCCB ),
                        EXTENSION( npCCB ) ) ;

      LocalUnlock( hCCB ) ;
      return ( (int) wLength ) ;
   }

} /* end of ReadConnectorBlock() */

/************************************************************************
 *  BOOL FAR PASCAL WriteConnectorByte( HANDLE hCCB, BYTE bSend )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL WriteConnectorByte( HANDLE hCCB, BYTE bSend )
{
   return ( WriteConnectorBlock( hCCB, (LPSTR) &bSend, 1 ) ) ;

} /* end of WriteConnectorByte() */

/************************************************************************
 *  BOOL FAR PASCAL WriteConnectorBlock( HANDLE hCCB, LPSTR lpBlock,
 *                                       int nBlockLen )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL WriteConnectorBlock( HANDLE hCCB, LPSTR lpBlock,
                                     int nBlockLen )
{
   BYTE          bLSN ;
   GLOBALHANDLE  hXmtBlock ;
   HNCB          hWriteNCB ;
   LPSTR         lpLocation ;
   NPCCB         npCCB ;
   WORD          wLength, wRemaining, wRetVal, wExtension ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (!CONNECTED( npCCB ))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   hWriteNCB = WRITENCB( npCCB ) ;
   bLSN = LSN( npCCB ) ;
   hXmtBlock = TXBLOCK( npCCB ) ;
   wExtension = EXTENSION( npCCB ) ;

   // loop until all of the block is transmitted

   lpLocation = lpBlock ;
   wRemaining = (WORD) nBlockLen ;
   do
   {
      wLength = (wRemaining > MAXLEN_XMTBLOCK) ? MAXLEN_XMTBLOCK : wRemaining ;
      wRetVal = NBPostSend( hWriteNCB, bLSN, hXmtBlock, lpLocation, wLength,
                            wExtension ) ;
      wRemaining = wRemaining - wLength ;

      DPF5( hWDB, "NBPostSend() returned %04x\r\n", wRetVal ) ;

      if (0 != wRetVal)
      {
         LocalUnlock( hCCB ) ;
         return ( FALSE ) ;
      }

      // this could break if over selector (segment) limits

      lpLocation = lpLocation + wLength ;
   }
   while (wRemaining > 0) ;

   LocalUnlock( hCCB ) ;
   return ( TRUE ) ;

} /* end of WriteConnectorBlock() */

/************************************************************************
 *  BOOL FAR _cdecl WriteConnectorFormat( HANDLE hCCB,
 *                                        LPSTR lpszFmt, ... )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR _cdecl WriteConnectorFormat( HANDLE hCCB, LPSTR lpszFmt,
                                      LPSTR lpArgList )
{
   BYTE      abBlock[MAXLEN_FMTSTR] ;
   int       nLen ;

   nLen = wvsprintf( (LPSTR) abBlock, (LPSTR) lpszFmt, (LPSTR) &lpArgList ) ;
   return ( WriteConnectorBlock( hCCB, abBlock, nLen ) ) ;

} /* end of WriteConnectorFormat() */

/************************************************************************
 *  BOOL FAR PASCAL IsConnected( HANDLE hCCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL IsConnected( HANDLE hCCB )
{
   BOOL   fRetVal ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   fRetVal = CONNECTED( npCCB ) ;
   LocalUnlock( hCCB ) ;

   return ( fRetVal ) ;

} /* end of IsConnected() */

/************************************************************************
 *  HNCB NBCreateNCB( HWND hWnd, BYTE bLana, BOOL fAsync,
 *                    LPVOID lpPostFunc )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

HNCB NBCreateNCB( HWND hWnd, BYTE bLana, BOOL fAsync, LPVOID lpPostFunc )
{
   HNCB    hNCB ;
   LPXNCB  lpXNCB ;

   if (NULL != ( hNCB = GlobalAlloc( NCB_GMEMFLAGS, sizeof( XNCB ))))
   {
      if (NULL != (lpXNCB = (LPXNCB) GlobalLock( hNCB )))
      {
         lpXNCB -> lpPostFunc = lpPostFunc ;
         lpXNCB -> hWnd = hWnd ;
         lpXNCB -> hNCB = hNCB ;
         lpXNCB -> bLana = bLana ;
         lpXNCB -> bAsync = (BYTE) (fAsync ? NBC_ASYNC : 0) ;
         GlobalUnlock( hNCB ) ;
      }
   }

   return ( hNCB ) ;

} /* end of NBCreateNCB() */

/************************************************************************
 *  BOOL NBDestroyNCB( HNCB hNCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL NBDestroyNCB( HNCB hNCB )
{
   return ( NULL == GlobalFree( hNCB ) ) ;

} /* end of NBDestroyNCB() */

/************************************************************************
 *  BOOL NBCheckAvail( VOID )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL NBCheckAvail( VOID )
{
   BOOL  fAvail ;

   fAvail = FALSE ;
   if (_dos_getvect( 0x5C ))
   {
      HNCB   hNCB ;
      LPNCB  lpNCB ;

      // grab a temporary NCB

      if (NULL != (hNCB = NBCreateNCB( NULL, 0, FALSE, NULL )))
      {
         if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
         {
            lpNCB -> bCommand = NBC_INVALID_COMMAND ;

            if (InvokeNetBIOS( lpNCB ) == NBE_INVALID_COMMAND)
               fAvail = TRUE ;

            GlobalUnlock( hNCB ) ;
         }
         NBDestroyNCB( hNCB ) ;
      }
   }

   DPF1( hWDB, "NBCheckAvail() returning %s\r\n",
         (LPSTR) (fAvail ? "TRUE" : "FALSE") ) ;

   return ( fAvail ) ;

} /* end of NBCheckAvail() */

/************************************************************************
 *  WORD NBPerformCommand( LPNCB lpNCB, BYTE bCmd, BOOL fWait )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD NBPerformCommand( LPNCB lpNCB, BYTE bCmd, BOOL fWait )
{
   MSG         msg ;
   WORD        wRetVal ;
   DWORD       dwTime ;

   // prepare command

   lpNCB -> bCommand = (BYTE) (bCmd | ((LPXNCB) lpNCB) -> bAsync) ;

   if (lpNCB -> bCommand & NBC_ASYNC)
      lpNCB -> lpPostFunc = ((LPXNCB) lpNCB) -> lpPostFunc ;

   wRetVal = InvokeNetBIOS( lpNCB ) ;

   // check for error

   if ((wRetVal) && (wRetVal != 0xFF))
      return ( wRetVal ) ;

   // check if app requested a software wait

   if (fWait && ((LPXNCB) lpNCB) -> bAsync)
   {
      dwTime = GetCurrentTime() ;
      if (lpNCB -> lpPostFunc != NULL)
      {
         // PostFunction is being used, therefore do a PeekMessage loop

         DP3( hWDB, "NBPerformCommand, entering PeekMessage loop w/ post" ) ;

         while (GetCurrentTime() - dwTime < NETBIOS_TIMEOUT )
            if (PeekMessage( &msg, ((LPXNCB) lpNCB) -> hWnd,
                             WM_EVENT_RXCHAR, WM_EVENT_RXCHAR,
                             PM_REMOVE ))
               break ;

         // completed if CmdCmplt != 0xFF

         return ( ((LPNCB) lpNCB) -> bReturn ) ;

      }
      else
      {
         // Wait for command to complete by
         // checking NCB command complete flag

         DP3( hWDB, "NBPerformCommand, entering PeekMessage loop blocked" ) ;

         while ( GetCurrentTime() - dwTime < NETBIOS_TIMEOUT )
            if (lpNCB -> bCmdCmplt != 0xFF)
               break ;
            else
               PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ;

         // completed if bCmdCmplt != 0xFF

         return ( ((LPNCB) lpNCB) -> bReturn ) ;
      }
   }

   return ( wRetVal ) ;

} /* end of NBPerformCommand() */

/************************************************************************
 *  LPSTR NBCreatePaddedName( LPSTR lpDst, LPSTR lpSrc )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

LPSTR NBCreatePaddedName( LPSTR lpDst, LPSTR lpSrc )
{
   int  nCount ;

   for (nCount = 0; nCount < (MAXLEN_NCBNAMESIZE - 1); nCount++)
   {
      if (!(lpDst[ nCount ] = lpSrc[ nCount ]))
      {
         while (nCount < (MAXLEN_NCBNAMESIZE - 1))
            lpDst[ nCount++ ] = ' ' ;
      }
   }
   lpDst[ MAXLEN_NCBNAMESIZE - 1 ] = NULL ;

   return ( lpDst ) ;

} /* end of NBCreatePaddedName() */

/************************************************************************
 *  BOOL NBClearNCB( HNCB hNCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL NBClearNCB( HNCB hNCB )
{
   LPNCB  lpNCB ;
   BOOL   bFlag ;

   bFlag = TRUE ;
   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
      // clear memory
      _fmemset( (LPSTR) lpNCB, 0, sizeof( NCB ) ) ;
      lpNCB -> bLanaNum = ((LPXNCB) lpNCB) -> bLana ;
      GlobalUnlock( hNCB ) ;
   }
   else
      bFlag = FALSE ;

   return ( bFlag ) ;

} /* end of NBClearNCB() */

/************************************************************************
 *  WORD NBAddLocalName( HNCB hNCB, LPSTR lpLocalName )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD NBAddLocalName( HNCB hNCB, LPSTR lpLocalName )
{
   LPNCB  lpNCB ;
   WORD   wReturn ;

   wReturn = -1 ;
   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
      // prepare NCB

      if (NBClearNCB( hNCB ))
      {
         // put LocalName in NCB

         NBCreatePaddedName( lpNCB -> abName, lpLocalName ) ;

         // fire off command and wait (nicely) for response

         wReturn = NBPerformCommand( lpNCB, NBC_ADD_NAME, PERFORM_WAIT ) ;
      }
      GlobalUnlock( hNCB ) ;
   }

   return ( wReturn ) ;

} /* end of NBAddLocalName() */

/************************************************************************
 *  WORD NBDeleteLocalName( HNCB hNCB, LPSTR lpLocalName )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD NBDeleteLocalName( HNCB hNCB, LPSTR lpLocalName )
{
   LPNCB  lpNCB ;
   WORD   wReturn ;

   wReturn = -1 ;
   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
      // prepare NCB

      if (NBClearNCB( hNCB ))
      {
         // put LocalName in NCB

         NBCreatePaddedName( lpNCB -> abName, lpLocalName ) ;

         // fire off command and wait (nicely) for response

         wReturn = NBPerformCommand( lpNCB, NBC_DELETE_NAME, PERFORM_WAIT ) ;
      }
      GlobalUnlock( hNCB ) ;
   }

   return ( wReturn ) ;

} /* end of NBDeleteLocalName() */

/************************************************************************
 *  WORD NBCreateSession( HNCB hNCB, LPSTR lpServerName, LPSTR lpLocalName,
 *                        WORD wExtension, WORD wRxTimeOut, WORD wTxTimeOut,
 *                        BOOL fServerMode )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD NBCreateSession( HNCB hNCB, LPSTR lpServerName, LPSTR lpLocalName,
                      WORD wExtension, WORD wRxTimeOut, WORD wTxTimeOut,
                      BOOL fServerMode )
{
   BYTE   bCommand ;
   LPNCB  lpNCB ;
   WORD   wReturn ;

   wReturn = -1 ;

   // add local name (if not UB)

   if (wExtension != NETEXT_UNGERMANBASS)
   {
      if (0 != NBAddLocalName( hNCB, lpLocalName ))
      {
         DP5( hWDB, "NBAddLocalName() failed." ) ;
         return ( wReturn ) ;
      }
   }

   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
      // prepare NCB

      if (NBClearNCB( hNCB ))
      {
         // put LocalName and ServerName in NCB

         if (wExtension == NETEXT_UNGERMANBASS)
         {
            _fmemset( lpNCB -> abName, ' ', 16 ) ;
            lpNCB -> abCallName[0] = NBGetNameLen( lpServerName ) ;
            lstrcpy( &(lpNCB -> abCallName[1]), lpServerName ) ;
         }
         else
         {
            NBCreatePaddedName( lpNCB -> abName, lpLocalName ) ;
            NBCreatePaddedName( lpNCB -> abCallName, lpServerName ) ;
         }

         lpNCB -> bRTO = LOBYTE( wRxTimeOut ) ;
         lpNCB -> bSTO = LOBYTE( wTxTimeOut ) ;

         // prepare command

         if (fServerMode)
         {
            switch (wExtension)
            {
               case NETEXT_UNGERMANBASS:
               case NETEXT_STANDARD:
               default:
                  bCommand = NBC_LISTEN ;
                  break;
            }
         }
         else
         {
            switch (wExtension)
            {
               case NETEXT_UNGERMANBASS:
                  bCommand = NBC_CALL_NIU ;
                  lpNCB -> bNumber = 0xFF ;
                  break ;

               case NETEXT_STANDARD:
               default:
                  bCommand = NBC_CALL ;
            }
         }
         // fire off command and wait (nicely) for response

         wReturn = NBPerformCommand( lpNCB, bCommand, PERFORM_WAIT ) ;
      }

      GlobalUnlock( hNCB ) ;
   }
   if ((0 != wReturn) && (wExtension != NETEXT_UNGERMANBASS))
      NBDeleteLocalName( hNCB, lpLocalName ) ;

   return ( wReturn ) ;

} /* end of NBCreateSession() */

/************************************************************************
 *  WORD NBDestroySession( HNCB hNCB, LPSTR lpLocalName, BYTE bLSN )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD NBDestroySession( HNCB hNCB, LPSTR lpLocalName, BYTE bLSN )
{
   LPNCB  lpNCB ;
   WORD   wReturn ;

   wReturn = -1 ;

   NBDeleteLocalName( hNCB, lpLocalName ) ;
   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
       // prepare NCB

      NBClearNCB( hNCB ) ;
      lpNCB -> bLSN = bLSN ;

      wReturn = NBPerformCommand( lpNCB, NBC_HANG_UP, PERFORM_WAIT ) ;
      GlobalUnlock( hNCB ) ;
   }
   return ( wReturn ) ;

} /* end of NBDestroySession() */

/************************************************************************
 *  BYTE NBGetLSN( HNCB hNCB )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BYTE NBGetLSN( HNCB hNCB )
{
   BYTE   bRetVal ;
   LPNCB  lpNCB ;

   bRetVal = -1 ;

   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
      bRetVal = lpNCB -> bLSN ;
      GlobalUnlock( hNCB ) ;
   }

   return ( bRetVal ) ;

} /* end of NBGetLSN() */

/************************************************************************
 *  BYTE NBGetNameLen( LPSTR lpName )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BYTE NBGetNameLen( LPSTR lpName )
{
   BYTE  bRetVal ;

   bRetVal = 0 ;
   while (lpName[ bRetVal ] != ' ' && lpName[ bRetVal ] != NULL)
      bRetVal++ ;

   return ( bRetVal ) ;

} /* end of NBGetNameLen() */

/************************************************************************
 *  WORD NBRequestReceive( HNCB hNCB, BYTE bLSN, GLOBALHANDLE hRecBlock,
 *                         WORD wExtension )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

WORD NBRequestReceive( HNCB hNCB, BYTE bLSN, GLOBALHANDLE hRecBlock,
                       WORD wExtension )
{
   BYTE    bCommand ;
   LPNCB   lpNCB ;
   LPVOID  lpRecBlock ;
   WORD    wRetVal ;

   // pessimistic view of return

   wRetVal = NBE_UNSPECIFIED_ERROR ;

   if (NULL != (lpNCB = (LPNCB) GlobalLock( hNCB )))
   {
      // clear memory & prepare for receive request
      _fmemset( (LPSTR) lpNCB, 0, sizeof( NCB ) ) ;
      lpNCB -> bLSN = bLSN ;

      // lock down receive buffer

      if (NULL == (lpRecBlock = (LPVOID) GlobalLock( hRecBlock )))
      {
         GlobalUnlock( hNCB ) ;
         return ( NBE_UNSPECIFIED_ERROR ) ;
      }

      lpNCB -> lpBuffer = lpRecBlock ;
      lpNCB -> wLength = MAXLEN_RECBLOCK ;

      switch (wExtension)
      {
         case NETEXT_UNGERMANBASS:
            bCommand = NBC_RECEIVE_PKTU ;
            break ;

         case NETEXT_STANDARD:
         default:
            bCommand = NBC_RECEIVE ;
      }

      wRetVal = NBPerformCommand( lpNCB, bCommand, PERFORM_NOWAIT ) ;
      GlobalUnlock( hNCB ) ;
   }
   return ( wRetVal ) ;

} /* end of NBRequestReceive() */

/************************************************************************
 *  WORD NBPostSend( HNCB hNCB, BYTE bLSN, GLOBALHANDLE hXmtBlock,
 *                   LPSTR lpPacket, WORD wLength, WORD wExtension )
 *
 *  Description:
 *
 *     Posts a NetBIOS write command... This implementation waits
 *     (nicely) for the response.  I designed it this way to
 *     eliminate as much message traffic as possible.
 *
 *  Comments:
 *     4/ 6/91 baw   Wrote It
 *
 ************************************************************************/

WORD NBPostSend( HNCB hNCB, BYTE bLSN, GLOBALHANDLE hXmtBlock,
                 LPSTR lpPacket, WORD wLength, WORD wExtension )
{
   BYTE    bCommand ;
   LPNCB   lpNCB ;
   LPVOID  lpXmtBlock ;
   WORD    wRetVal ;

   wRetVal = NBE_UNSPECIFIED_ERROR ;

   if (NULL == (lpNCB = (LPNCB) GlobalLock( hNCB )))
      return ( wRetVal ) ;

   // lock down transmit buffer

   if (NULL == (lpXmtBlock = (LPVOID) GlobalLock( hXmtBlock )))
   {
      GlobalUnlock( hNCB ) ;
      return ( wRetVal ) ;
   }

   // clear NCB

   NBClearNCB( hNCB ) ;

   // prepare transmit buffer and NCB

   _fmemcpy( (LPSTR) lpXmtBlock, lpPacket, wLength ) ;
   lpNCB -> lpBuffer = lpXmtBlock ;
   lpNCB -> wLength = wLength ;
   lpNCB -> bLSN = bLSN ;

   switch (wExtension)
   {
      case NETEXT_UNGERMANBASS:
         bCommand = NBC_SEND_PKTU ;
         break ;

      case NETEXT_STANDARD:
      default:
         bCommand = NBC_SEND ;
   }

   wRetVal = NBPerformCommand( lpNCB, bCommand, PERFORM_WAIT ) ;

   GlobalUnlock( hXmtBlock ) ;
   GlobalUnlock( hNCB ) ;
   
   return ( wRetVal ) ;

} /* end of NBPostSend() */

/************************************************************************
 * End of File: netbios.c
 ************************************************************************/
