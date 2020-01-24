//***************************************************************************
//
//  Module: stdcomm.c
//
//  Purpose:
//     Provides a generic interface for a RS-232 communications
//     device.
//
//  Description of functions:
//     Contained within function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              1/ 7/91   BryanW      Initial Version, first build
//              1/12/92   BryanW      Added this new comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"     // precompiled stuff

#include "stdcomm.h"

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
 *  HCCB FAR PASCAL CreateCCB( HWND hWnd )
 *
 *  Description:
 *     Creates and initializes a Connector Control Block (CCB).
 *     Returns NULL if unsuccessful.
 *
 *  Comments:
 *      9/21/91  baw  Wrote this comment.
 *
 ************************************************************************/

HCCB FAR PASCAL CreateCCB( HWND hWnd )
{
   HCCB   hCCB ;
   NPCCB  npCCB ;

   if (NULL == (hCCB =  LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                    sizeof( CCB ) )))
      return ( NULL ) ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
   {
      LocalFree( hCCB ) ;
      return ( NULL ) ;
   }

   // this should read from defaults (.INI file)

   SETTINGS( npCCB ).bPort = 1 ;
   SETTINGS( npCCB ).wBaudRate = CBR_9600 ;
   SETTINGS( npCCB ).bByteSize = 8 ;
   SETTINGS( npCCB ).bParity = NOPARITY ;
   SETTINGS( npCCB ).bStopBits = ONESTOPBIT ;
   SETTINGS( npCCB ).bFlowControl = FC_RTSCTS ;
   SETTINGS( npCCB ).wBreakLength = 500 ;
   CONNECTED( npCCB ) = FALSE ;
   CID( npCCB ) = NULL ;
   PARENTWND( npCCB ) = hWnd ;

   LocalUnlock( hCCB ) ;

   return ( hCCB ) ;

} /* end of CreateCCB() */

/************************************************************************
 *  BOOL FAR PASCAL DestroyCCB( HCCB hCCB )
 *
 *  Description:
 *     Destroys the CCB.
 *
 *  Comments:
 *      9/21/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL DestroyCCB( HCCB hCCB )
{
   return ( NULL == LocalFree( hCCB ) ) ;

} /* end of DestroyCCB() */

//************************************************************************
//  BOOL FAR PASCAL OpenConnector( HCCB hCCB )
//
//  Description:
//     This function opens the connection device.  This interface
//     provides for multi-state functions.  This will allow the
//     application to continue to call this function until the
//     process completes or fails.
//
//     Returns ERROR_IO_PENDING if successive calls should be made.
//     Returns FALSE if unsucessful, otherwise returns TRUE.
//
//  Parameters:
//     HCCB hCCB
//        handle to connector control block.
//
//  History:   Date       Author      Comment
//              1/12/92   BryanW      Put back together in a single call.
//
//************************************************************************

BOOL FAR PASCAL OpenConnector( HCCB hCCB )
{
   int    nResult ;
   char   szPort[ 10 ], szTemp[ 10 ] ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (CONNECTED( npCCB ))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   // load the COM prefix string and append port number

   LoadString( hLibInst, IDS_COMPREFIX, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szPort, "%s%d", (LPSTR) szTemp, SETTINGS( npCCB ).bPort ) ;

   nResult = OpenComm( szPort, RXBUFSIZE, TXBUFSIZE ) ;
   if (nResult < 0)
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }
   
   CID( npCCB ) = nResult ;

   // bump use count

   nUseCount++ ;

   // set connected status

   CONNECTED( npCCB ) = TRUE ;

   if (!SetupConnector( hCCB ))
   {
      LocalUnlock( hCCB ) ;
      CloseConnector( hCCB ) ;
      return ( FALSE ) ;
   }

   if (!SetEvent( hCCB, CN_RXBLK ))
   {
      LocalUnlock( hCCB ) ;
      CloseConnector( hCCB ) ;
      return ( FALSE ) ;
   }

   // NEED: check DTR usage

   // assert DTR

   EscapeCommFunction( CID( npCCB ), SETDTR ) ;

   LocalUnlock( hCCB ) ;
   return ( TRUE ) ;

} // end of OpenConnector()

/************************************************************************
 *  BOOL FAR PASCAL CloseConnector( HCCB hCCB )
 *
 *  Description:
 *     Closes the connection.
 *
 *  Comments:
 *      9/21/91  baw  Added this comment.
 *
 ************************************************************************/

BOOL FAR PASCAL CloseConnector( HCCB hCCB )
{
   NPCCB  npCCB ;
   int    nResult ;

   DP3( hWDB, "CloseConnector()" ) ;

   // bump use count
   nUseCount-- ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (!CONNECTED( npCCB ))
   {
      // already disconnected

      LocalUnlock( hCCB ) ;
      return ( TRUE ) ;
   }

   // flush I/O buffers

   FlushComm( CID( npCCB ), 0 ) ;
   FlushComm( CID( npCCB ), 1 ) ;

   // NEED: check DTR usage

   // drop DTR

   EscapeCommFunction( CID( npCCB ), CLRDTR ) ;

   // kill the pending events

   KillEvent( hCCB ) ;

   nResult = CloseComm( CID( npCCB ) ) ;
   if (nResult < 0)
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   CID( npCCB ) = 0 ;
   CONNECTED( npCCB ) = FALSE ;

   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of CloseConnector() */

/************************************************************************
 *  BOOL SetupConnector( HCCB hCCB )
 *
 *  Description:
 *     Sets the communication parameters as defined in CCB.csSettings.
 *     Returns TRUE if no error.
 *
 *  Comments:
 *       6/19/91  baw  Wrote this comment.
 *       6/25/91  baw  Updated for FC_HARDWARE and FC_SOFTWARE.
 *
 ************************************************************************/

//------------------------------------------------------------------------
//  BOOL SetupConnector( HCCB hCCB )
//
//  Description:
//     Sets the communication parameters as defined in CCB.csSettings.
//     Returns TRUE if no error, FALSE otherwise.
//
//  Parameters:
//     HCCB hCCB
//        handle to CCB
//
//  History:   Date       Author      Comment
//              6/19/91   BryanW      Wrote this comment.
//
//              6/25/91   BryanW      Updated for FC_HARDWARE and
//                                    FC_SOFTWARE.
//
//              2/22/92   BryanW      Changed to FC_XONXOFF, FC_RTSCTS,
//                                    and FC_DTRDSR.
//
//------------------------------------------------------------------------

BOOL SetupConnector( HCCB hCCB )
{
   BYTE   bSet ;
   DCB    dcb ;
   NPCCB  npCCB ;
   int    nResult ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (!CONNECTED( npCCB ))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   GetCommState( CID( npCCB ), &dcb ) ;

   dcb.BaudRate = SETTINGS( npCCB ).wBaudRate ;
   dcb.ByteSize = SETTINGS( npCCB ).bByteSize ;
   dcb.Parity = SETTINGS( npCCB ).bParity ;
   dcb.StopBits = SETTINGS( npCCB ).bStopBits ;

   // setup RTS/CTS hardware flow control

   bSet = (BYTE) ((SETTINGS( npCCB ).bFlowControl & FC_RTSCTS) != 0) ;
   dcb.fRtsflow = dcb.fOutxCtsFlow = bSet ;
   dcb.CtsTimeout = (bSet) ? 30 : 0 ;

   // setup DTR/DSR hardware flow control

   bSet = (BYTE) ((SETTINGS( npCCB ).bFlowControl & FC_DTRDSR) != 0) ;
   dcb.fDtrflow = dcb.fOutxDsrFlow = bSet ;
   dcb.DsrTimeout = (bSet) ? 30 : 0 ;

   // setup XON/XOFF software flow control

   bSet = (BYTE) ((SETTINGS( npCCB ).bFlowControl & FC_XONXOFF) != 0) ;
   dcb.fInX = dcb.fOutX = bSet ;
   dcb.XonChar = ASCII_XON ;
   dcb.XoffChar = ASCII_XOFF ;

   // NEED: compute the baud rate, determine optimal setting in
   // milliseconds and compute the ranges for the XON/XOFF window

   dcb.XonLim = 10 ;
   dcb.XoffLim = 10 ;

   // other various settings

   dcb.fBinary = TRUE ;
   dcb.fParity = TRUE ;
   dcb.fDtrDisable = FALSE ;
   dcb.fRtsDisable = FALSE ;

   nResult = SetCommState( &dcb ) ;

   DPF5( hWDB, "SetCommState = %d\r\n", nResult ) ;

   LocalUnlock( hCCB ) ;

   return ( !(nResult < 0) ) ;

} // end of SetupConnector()

/************************************************************************
 *  int FAR PASCAL ReadConnectorBlock( HCCB hCCB, int nMaxBytes,
 *                                     LPSTR lpBlock )
 *
 *  Description:
 *     This function should be called in response to a CN_RXBLK
 *     notification.
 *
 *  Comments:
 *      9/21/91  baw  Added this comment.
 *
 ************************************************************************/

int FAR PASCAL ReadConnectorBlock( HCCB hCCB, int nMaxBytes,
                                   LPSTR lpBlock )
{
   int    nCid, nError, nLength ;
   NPCCB  npCCB ;

#ifdef NOT_USED
   int    i ;
   char   szErrorStr[ MAXLEN_TEMPSTR ] ;
#endif

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( -1 ) ;

   if (!CONNECTED( npCCB ))
   {
      LocalUnlock( hCCB ) ;
      return ( -1 ) ;
   }

   // Clear event status 

   ClearEvent( hCCB, CN_RXBLK ) ;

   nCid = CID( npCCB ) ;

   LocalUnlock( hCCB ) ;

   if ((nLength = ReadComm( nCid, lpBlock, nMaxBytes )) < 0)
   {
      while (nError = GetCommError( nCid, NULL ))
      {
#ifdef NOT_USED
         for (i = 0; i < 14; i++)
         {
            if ((nError >> i) & 0x01)
            {
               LoadString( hLibInst, IDS_CE_RXOVER + i,
                           szErrorStr, MAXLEN_TEMPSTR ) ;
               DPF3( hWDB, "%s\r\n", (LPSTR) szErrorStr ) ;
            }
         }
#endif
      
         // lpfnErrorProc = npCCB -> lpfnErrorProc ;
         // wParam = npCCB -> wParam ;

         // error reporting function to terminal
         // if (lpfnErrorProc)
         //    (*lpfnErrorProc)( hCCB, wParam, nError ) ;
      }
      nLength *= -1 ;
   }

   LocalUnlock( hCCB ) ;
   return ( nLength ) ;

} /* end of ReadConnectorBlock() */
             
/************************************************************************
 *  BOOL FAR PASCAL WriteConnectorByte( HCCB hCCB, BYTE bSend )
 *
 *  Description:
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL WriteConnectorByte( HCCB hCCB, BYTE bSend )
{
   int      nCid, nError ;
   NPCCB    npCCB ;
   COMSTAT  csStatus ;
#ifdef NOT_USED
   int      i ;
   char     szErrorStr[ MAXLEN_TEMPSTR ] ;
#endif

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (!CONNECTED( npCCB ))
   {
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

   nCid = CID( npCCB ) ;
   do
   {
      // nError = GetCommError( nCid, &csStatus ) ;
      while (nError = GetCommError( nCid, &csStatus ))
      {
#ifdef NOT_USED
         for (i = 0; i < 14; i++)
         {
            if ((nError >> i) & 0x01)
            {
               LoadString( hLibInst, IDS_CE_RXOVER + i,
                           szErrorStr, MAXLEN_TEMPSTR ) ;
               DPF3( hWDB, "%s\r\n", (LPSTR) szErrorStr ) ;
            }
         }
#endif
      }
   }
   while (csStatus.cbOutQue >= (WORD) (TXBUFSIZE - 1)) ;
   WriteComm( nCid, &bSend, 1 ) ;

   LocalUnlock( hCCB ) ;
   return ( TRUE ) ;

} /* end of WriteConnectorByte() */

/************************************************************************
 *  BOOL FAR PASCAL WriteConnectorBlock( HCCB hCCB, LPSTR lpBlock,
 *                                       int nBlockLen )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR PASCAL WriteConnectorBlock( HCCB hCCB, LPSTR lpBlock,
                                     int nBlockLen )
{
   int      nCid, nError ;
   NPCCB    npCCB ;
   COMSTAT  csStatus ;
#ifdef NOT_USED
   int      i ;
   char     szErrorStr[ MAXLEN_TEMPSTR ] ;
#endif

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   nCid = CID( npCCB ) ;
   do
   {
      // nError = GetCommError( nCid, &csStatus ) ;
      while (nError = GetCommError( nCid, &csStatus ))
      {
#ifdef NOT_USED
         for (i = 0; i < 14; i++)
         {
            if ((nError >> i) & 0x01)
            {
               LoadString( hLibInst, IDS_CE_RXOVER + i,
                           szErrorStr, MAXLEN_TEMPSTR ) ;
               DPF3( hWDB, "%s\r\n", (LPSTR) szErrorStr ) ;
            }
         }
#endif
      }
   }
   while (csStatus.cbOutQue >= (WORD) (TXBUFSIZE - nBlockLen)) ;
   WriteComm( nCid, lpBlock, nBlockLen ) ;

   LocalUnlock( hCCB ) ;
   return ( TRUE ) ;

} /* end of WriteConnectorBlock() */

/************************************************************************
 *  BOOL FAR _cdecl WriteConnectorFormat( HCCB hCCB,
 *                                        LPSTR lpszFmt, ... )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

BOOL FAR _cdecl WriteConnectorFormat( HCCB hCCB, LPSTR lpszFmt,
                                      LPSTR lpArgList )
{
   BYTE      abBlock[MAXLEN_FMTSTR] ;
   int       nLen ;

   nLen = wvsprintf( (LPSTR) abBlock, (LPSTR) lpszFmt, (LPSTR) &lpArgList ) ;
   return ( WriteConnectorBlock( hCCB, abBlock, nLen ) ) ;

} /* end of WriteConnectorFormat() */

/************************************************************************
 *  LONG FAR PASCAL EscapeConnectorFunction( HCCB hCCB, WORD wParam,
 *                                           LONG lParam1, LONG lParam2 )
 *
 *  Description:
 *     Device entry point for extensible functionality.
 *
 *  Comments:
 *      9/21/91  baw  Wrote it.
 *
 ************************************************************************/

LONG FAR PASCAL EscapeConnectorFunction( HCCB hCCB, WORD wParam,
                                         LONG lParam1, LONG lParam2 )
{
   LONG   lRetVal ;
   NPCCB  npCCB ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( ECFERR_INVALIDCCB ) ;

   switch (wParam)
   {
      case ECF_CONNECTSTATUS:
         lRetVal = (LONG) CONNECTED( npCCB ) ;
         break ;

      case ECF_SETNOTIFYWND:
         ClearEvent( hCCB, CN_RXBLK ) ;
         KillEvent( hCCB ) ;
         PARENTWND( npCCB ) = LOWORD( lParam1 ) ;
         lRetVal = (LONG) SetEvent( hCCB, CN_RXBLK ) ;
         break;

      case ECF_GETNOTIFYWND:
         lRetVal = MAKELONG( PARENTWND( npCCB ), NULL ) ;
         break ;

      default:
         lRetVal = ECFERR_UNDEFFUNC ;
   }
   LocalUnlock( hCCB ) ;
   return ( lRetVal ) ;

} /* end of EscapeConnectorFunction() */

//***************************************************************************
//  End of File: stdcomm.c
//***************************************************************************

