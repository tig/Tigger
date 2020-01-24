/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  bbs.c
 *
 *     Remarks:  Highlevel communications functions for interacting
 *               with the bbs.
 *
 *   Revisions:  
 *    3/18/92  cek   Basic design...started implementation.
 *    3/23/92  cek   I'm on a plane from NY to Seattle and I'm feeling
 *                   pretty good.  If some of the comments in this code
 *                   sound smart-ass, that's why.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "bbs.h"
#include "connect.h"
#include "online.h"

#define SENDBBS_TIMER_ID  43

/*
 * Function failure handling stuff.
 */
#define RIP_CITY( ui )  {uiIDerr=ui;goto Failure;}
static UINT    uiIDerr ;

extern BOOL        fUseCN_RECEIVE ;

#define fCoalesce TRUE

/****************************************************************
 *  UINT ProcessSendBBSMsg( LPGENERICMSG lpMsg )
 *
 *  Description: 
 *
 *    The BBS is set up to recieve messages from us.  These messages
 *    are composed of a stream of data where the first four bytes
 *    are the MessageID (actually it's the two byte message ID send twice)
 *    and the following bytes contain the
 *    data assocaited with the message (if any) which, in turn,
 *    is followed by a a CRC value.
 *
 *    If the ProcessSendBBSMsg() call returns EXITCODE_OK (0) the message
 *    was processed succssfully by the BBS.
 *
 *    Otherwise something went terribly wrong and we had to hang up.
 *
 *    From this, you can see that ProcessSendBBSMsg is a synchronous call,
 *    and will not return to the caller until the entire message
 *    has been carried out.  If it returns FALSE we are no
 *    longer connected and the user will have already been
 *    informed of what went  wrong.
 *
 *    The HWND hwnd parameter to ProcessSendBBSMsg() is the session
 *    window.  This window is used to store state machine
 *    data as well as to act as the parent to any pop-ups
 *    that may be needed.
 *
 *    FontShopper contains a similar function, but the font shopper
 *    implementation uses DDE to talk to CONXSRV's DDE Server which
 *    in turn calls this function.
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI ProcessSendBBSMsg( LPGENERICMSG lpMsg )
{
   LPONLINEFSM    lpFSM ;
   MSG            msg ; // for peekmessage loop
   BOOL           fDispatchNotify = FALSE ;
   MSG            msgNotify ; // for coelescing
   LPARAM         lTemp ;

   if (!IsWindow( hdlgOnline ))
      return EXITCODE_GENERALFAILURE ;

   if (NULL == (lpFSM = (LPONLINEFSM)GET_FSM( hdlgOnline )))
   {
      DP3( hWDB, "GET_FSM failed in ProcessSendBBSMsg!!!!  Returning..." ) ;
      return EXITCODE_GENERALFAILURE ;
   }

   if (lpFSM->uiState != STATE_ONLINE_IDLE)
   {
      DP1( hWDB, "ProcessSendBBSMsg attempt while not idle!" ) ;
      /*
       * Oh, dear.... what to do here...
       *
       * This case will be encountered if we try to do
       * something like MSG_HANGING_UP while in the
       * middle of someother action.
       */
   }

   /********************************************************
    *
    * This is where each of the messages is handled
    *
    * First initialize the FSM structure so we don't have
    * to in each case...
    * 
    *******************************************************/

   DP2( hWDB, "<<<< ProcessSendBBSMsg( '%s' == 0x%02X ) >>>>", (LPSTR)rglpsz[IDS_MSG_NUL + (UINT)lpMsg->SendMsg], (UINT)lpMsg->SendMsg ) ;

   DP4( hWDB, "lpMsg->SendMsg    = %02X", (UINT)lpMsg->SendMsg ) ;
   DP4( hWDB, "lpMsg->wFlags     = %04X", (UINT)lpMsg->wFlags ) ;
   DP4( hWDB, "lpMsg->lpSend     = %08lX", (LPVOID)lpMsg->lpSend ) ;
   DP4( hWDB, "lpMsg->cbSend     = %d", lpMsg->cbSend ) ;
   DP4( hWDB, "lpMsg->lpReceive  = %08lX", (LPVOID)lpMsg->lpReceive ) ;
   DP4( hWDB, "lpMsg->cbReceive  = %d", lpMsg->cbReceive ) ;


   lpFSM->lpReceive     = lpMsg->lpReceive ;
   lpFSM->cbReceive     = lpMsg->cbReceive ;
   lpFSM->fReceiveCRC   = lpMsg->wFlags & MSGFLAG_RECEIVECRC ;
   lpFSM->fReceivePad   = lpMsg->wFlags & MSGFLAG_RECEIVEPAD ;

   lpFSM->lpSend        = lpMsg->lpSend ;
   lpFSM->cbSend        = lpMsg->cbSend ;   
   lpFSM->fSendCRC      = lpMsg->wFlags & MSGFLAG_SENDCRC ;
   lpFSM->fSendPad      = lpMsg->wFlags & MSGFLAG_SENDPAD ;

   lpFSM->bSendMsg      = lpMsg->SendMsg ;

   switch( lpMsg->wFlags & 0xFF00 )
   {
      case MSGCLASS_GOIDLE:
         lpFSM->uiNextState = STATE_ONLINE_IDLE ;
      break ;

      case MSGCLASS_WAITACK:
         lpFSM->uiNextState = STATE_ONLINE_WAITACK ;
      break ;

      case MSGCLASS_WAITRECEIVE:
         lpFSM->uiNextState = STATE_ONLINE_WAITRECEIVE ;
      break ;

      case MSGCLASS_SENDDATA_WAITACK:
         lpFSM->uiNextState = STATE_ONLINE_SENDDATA_WAITACK ;
      break ;

      case MSGCLASS_SENDDATA_WAITRECEIVE:
         lpFSM->uiNextState = STATE_ONLINE_SENDDATA_WAITRECEIVE ;
      break ;

      case MSGCLASS_CONXPRO_RECEIVE:
         lpFSM->uiNextState = STATE_ONLINE_CONXPRO_RECEIVE ;
      break ;

      default:
         DP1( hWDB, "Invalid Message class in ProcessSendBBSMsg()! (%d)", lpMsg->wFlags & 0xFF00 ) ;
         lpFSM->uiNextState = STATE_ONLINE_IDLE ;
      break ;
   }

   lpFSM->uiState = STATE_ONLINE_SENDMSG ;

   if (lpFSM->bSendMsg != MSG_SAY_HI)
      SetOnlineStatusMsg( rglpsz[IDS_SENDINGMSG] ) ;

   /*
    * error recovery stuff
    */
   lpFSM->uiReSendAttempts = 0 ;
   lpFSM->uiReSyncAttempts = 0 ;
   lpFSM->uiSavedNextState = lpFSM->uiNextState ;

   DP4( hWDB, "Entering peek message loop" ) ;
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

         /*
          * This is where we detect if the FSM has been shut down or
          * not.  The most likely place for a FF_ONLINE_ABORT to happen
          * is during a WM_TIMER messag (i.e. a call to OnlineFSM with
          * FF_ONLINE_PROCESS).  By trapping this after the DispatchMsg()
          * call we get the fact that it aborted immediately.
          */ 
         if (hdlgOnline && IsWindow( hdlgOnline ) && (lpFSM = (LPONLINEFSM)GET_FSM( hdlgOnline )))
         {
            if (lpFSM->uiState == STATE_ONLINE_IDLE)
            {
               if ((!fUseCN_RECEIVE && fCoalesce) && fDispatchNotify)
                  DispatchMessage( &msgNotify ) ;

               return lpFSM->uiExitCode ;
            }

            if (lpFSM->uiState == STATE_ONLINE_FATALABORT)
            {
               if ((!fUseCN_RECEIVE && fCoalesce) && fDispatchNotify)
                  DispatchMessage( &msgNotify ) ;

               DP2( hWDB, "   uiState == STATE_ONLINE_FATALABORT... doing EXIT" ) ;
               return OnlineFSM( hdlgOnline, FF_ONLINE_ABORT ) ;
            }

         }
         else
         {
            if ((!fUseCN_RECEIVE && fCoalesce) && fDispatchNotify)
               DispatchMessage( &msgNotify ) ;

            DP2( hWDB, "   Bailing with GENERALFAILURE" ) ;
            return EXITCODE_GENERALFAILURE ;
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


/****************************************************************
 *  UINT FAR WINAPI SendBBSMsg( MSGTYPE bbsmsg, LPVOID lpSend, LPVOID lpReceive )
 *
 *    SendBBSMsg is a wrapper for ProcessSendBBSMsg() that
 *    makes invoking a specific message simple.  Only the message ID
 *    (bbsmsg), send buffer (lpSend), and receive buffer (lpReceive)
 *    are needed.
 *
 ****************************************************************/
UINT FAR WINAPI SendBBSMsg( MSGTYPE bbsmsg, LPVOID lpSend, LPVOID lpReceive )
{
   GENERICMSG     Msg ;

   DP2( hWDB, "<<<< SendBBSMsg( '%s' == 0x%02X ) >>>>", (LPSTR)rglpsz[IDS_MSG_NUL + (UINT)bbsmsg], (UINT)bbsmsg ) ;

   Msg.lpReceive     = lpReceive ;     // Pointer to receive buffer
   Msg.cbReceive     = 0 ;             // Number of bytes in receive buffer

   Msg.lpSend        = lpSend ;        // Pointer to send buffer
   Msg.cbSend        = 0 ;             // number of bytes to be sent.

   Msg.wFlags        = 0x0000 ;

   Msg.SendMsg       = (BYTE)bbsmsg ;  // Current message

   switch( bbsmsg )
   {
      /*
       * These messages have no send or receive buffers.  They simply
       * tell the bbs to do something with data it already has.  If
       * the bbs got the message it will ACK.
       */
      case MSG_SAY_HI:
         Msg.wFlags |= MSGCLASS_WAITACK ;
      break ;

      case MSG_HANGING_UP:
         /*
          * The host will respond with '+++'.  Just
          * ignore it.
          */
         Msg.wFlags |= MSGCLASS_GOIDLE ;
      break ;

      default:
         DP1( hWDB, "Invalid SendBBSMsg! bbsmsg = %04X", bbsmsg ) ;
      break ;

   }

   return ProcessSendBBSMsg( &Msg ) ;

} /* SendBBSMsg()  */


/************************************************************************
 * End of File: bbs.c
 ***********************************************************************/

