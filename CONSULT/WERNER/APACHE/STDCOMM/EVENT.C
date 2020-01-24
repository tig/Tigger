/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  event.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  System Timer Event Manager
 *               Provides timing using the system timer. (18.2 / second)
 *
 *   Revisions:  
 *     01.00.000  9/21/91 baw   Private events are encoded to EV_*.
 *
 ************************************************************************/

#include "precomp.h"     // precompiled stuff

#include "stdcomm.h"

/************************************************************************
 *  BOOL SetEvent( HANDLE hCCB, WORD wEvent )
 *
 *  Description:
 *     Sets a connector event.  This should also implement
 *     EnableCommNotification for Windows 3.1.
 *
 *  Comments:
 *      9/21/91  baw  Added this comment.
 *      9/21/91  baw  Removed the HWND parameter - now in CCB.
 *
 ************************************************************************/

BOOL SetEvent( HANDLE hCCB, WORD wEvent )
{
   int    i ;
   NPCCB  npCCB ;
   WORD   wEventMask ;

   DP3( hWDB, "SetEvent()" ) ;

   // translate to EV_*

   switch (wEvent)
   {
      case CN_RXBLK:
         wEventMask = EV_RXCHAR ;
         break ;

      case CN_TXEMPTY:
         wEventMask = EV_TXEMPTY ;
         break ;

      default:
         return ( FALSE ) ;
   }

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (LOWORD( GetVersion() ) >= 0x30A)
   {
      // use new 3.1 COMM API

//      EnableCommNotification( CID( npCCB ), PARENTWND( npCCB ), 250, -1 ) ;
//      SetCommEventMask( CID( npCCB ), NULL ) ;
      EnableCommNotification( CID( npCCB ), PARENTWND( npCCB ), -1, -1 ) ;
      SetCommEventMask( CID( npCCB ), wEventMask ) ;
      LocalUnlock( hCCB ) ;
      return ( TRUE ) ;
   }
   else
   {
      if (!nIdTimer)
      {
         DP5( hWDB, "Creating System Timer" ) ;
         nIdTimer = CreateSystemTimer( 55, (FARPROC) TimerHandler ) ;
         if (!nIdTimer)
         {
            LocalUnlock( hCCB ) ;
            return ( FALSE ) ;
         }
      }
   
      // if event exists, update it
   
      for (i = 0; i < MAXEVENTS; i++)
      {
         if (Events[i].hWnd == PARENTWND( npCCB ))
         {
            Events[i].hWnd =       NULL ;
            Events[i].nCid =       CID( npCCB ) ;
            Events[i].wEvent =     wEvent ;
            Events[i].wEventMask = wEventMask ;
            Events[i].fPostFlag  = FALSE ;
            Events[i].lpEventPtr = SetCommEventMask( CID( npCCB ),
                                                      wEventMask ) ;
            Events[i].hWnd       = PARENTWND( npCCB ) ;
            LocalUnlock( hCCB ) ;
            return ( TRUE ) ;
         }
      }

      // find place in event list
   
      for (i = 0; i < MAXEVENTS; i++)
      {
         if (Events[i].hWnd == NULL)
         {
            Events[i].nCid =       CID( npCCB ) ;
            Events[i].wEvent =     wEvent ;
            Events[i].wEventMask = wEventMask ;
            Events[i].lpEventPtr = SetCommEventMask( CID( npCCB ),
                                                      wEventMask ) ;
            Events[i].hWnd =       PARENTWND( npCCB ) ;
            LocalUnlock( hCCB ) ;
            return ( TRUE ) ;
         }
      }
      LocalUnlock( hCCB ) ;
      return ( FALSE ) ;
   }

} /* end of SetEvent() */

/************************************************************************
 *  BOOL KillEvent( HANDLE hCCB )
 *
 *  Description:
 *     Kills pending connector events.
 *
 *  Comments:
 *      9/21/91  baw  Added this comment.
 *
 ************************************************************************/

BOOL KillEvent( HANDLE hCCB )
{
   int    i ;
   NPCCB  npCCB ;

   DP3( hWDB, "KillEvent()" ) ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (LOWORD( GetVersion() ) >= 0x30A)
   {
      // use new 3.1 COMM API

      EnableCommNotification( CID( npCCB ), NULL, -1, -1 ) ;
   }
   else
   {
      for (i = 0; i < MAXEVENTS; i++)
      {
         if (Events[i].hWnd == PARENTWND( npCCB ))
         {
            Events[i].hWnd =       NULL ;
            Events[i].nCid =       0 ;
            Events[i].fPostFlag =  FALSE ;
            Events[i].wEvent =     0 ;
            Events[i].wEventMask = 0 ;
            Events[i].lpEventPtr = NULL ;
         }
      }

      // if no events, kill timer

      for (i = 0; i < MAXEVENTS; i++)
      {
         if (Events[i].hWnd != NULL)
            return ( TRUE ) ;
      }

      KillSystemTimer( nIdTimer ) ;
      nIdTimer = 0 ;
   }
   
   LocalUnlock( hCCB ) ;

   return ( TRUE ) ;

} /* end of KillEvent() */

/************************************************************************
 *  BOOL ClearEvent( HANDLE hCCB, WORD wEvent )
 *
 *  Description:
 *     Clears a posted event flag.
 *
 *  Comments:
 *      9/21/91  baw  Added this comment.
 *
 ************************************************************************/

BOOL ClearEvent( HANDLE hCCB, WORD wEvent )
{
   int    i ;
   NPCCB  npCCB ;
   WORD   wEventWord ;

   if (NULL == (npCCB = (NPCCB) LocalLock( hCCB )))
      return ( FALSE ) ;

   if (LOWORD( GetVersion() ) >= 0x30A)
   {
      WORD  wEventMask ;

      switch (wEvent)
      {
         case CN_RXBLK:
            wEventMask = EV_RXCHAR ;
            break ;
   
         case CN_TXEMPTY:
            wEventMask = EV_TXEMPTY ;
            break ;
   
         default:
            LocalUnlock( hCCB ) ;
            return ( FALSE ) ;
      }

      wEventWord = GetCommEventMask( CID( npCCB ), EV_RLSDS | EV_RXCHAR ) ;
//      DPF3( hWDB, "Events: %04x\r\n", wEventWord ) ;
      LocalUnlock( hCCB ) ;
      return ( TRUE ) ;
   }
   else
   {
      for (i = 0; i < MAXEVENTS; i++)
      {
         if (Events[i].hWnd == PARENTWND( npCCB ) &&
             Events[i].wEvent == wEvent)
         {
            Events[i].fPostFlag = FALSE ;
            GetCommEventMask( Events[i].nCid, Events[i].wEventMask ) ;
            LocalUnlock( hCCB ) ;
            return ( TRUE ) ;
         }
      }
   }

   LocalUnlock( hCCB ) ;
   return ( FALSE ) ;

} /* end of ClearEvent() */

/************************************************************************
 * End of File: event.c
 ************************************************************************/

