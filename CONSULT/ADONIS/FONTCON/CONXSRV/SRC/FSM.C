/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  fsm.c
 *
 *     Remarks:  Finite State Machine == FSM
 *
 *    This included all functions that are common accross
 *    the finite state machine implementation used througout
 *    CONXSRV.  The functions here are used by modules such as
 *    connect.c and bbs.c.
 *
 *    We use TOOHELP.DLL for our timer routines because it is more
 *    accurate when a full screen dos app is active in enhanced mode.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "fsm.h"

VOID WINAPI StartPause( LPPAUSE lpPause, DWORD msecs )
{
   TIMERINFO  ti ;

   ti.dwSize = sizeof(ti) ;
   TimerCount( &ti ) ;

   lpPause->dwPause = msecs ;
   lpPause->dwStart = ti.dwmsSinceStart ;
}

BOOL WINAPI IsPauseDone( LPPAUSE lpPause ) 
{
   TIMERINFO   ti ;

   ti.dwSize = sizeof(ti) ;
   TimerCount( &ti ) ;

   if (ti.dwmsSinceStart >= (lpPause->dwStart + lpPause->dwPause))
   {
      ResetPause( lpPause ) ;
      return TRUE ;
   }

   return FALSE ;
}

VOID WINAPI ResetPause( LPPAUSE lpPause )
{
   lpPause->dwPause = 0 ;
   lpPause->dwStart = 0 ;
}


VOID WINAPI StartTimeout( LPTIMEOUT lpTimeout, UINT secs )
{
   TIMERINFO   ti ;

   ti.dwSize = sizeof(ti) ;
   TimerCount( &ti ) ;

   lpTimeout->uiTimeout = secs ;
   lpTimeout->uiStart = LOWORD(ti.dwmsSinceStart / 1000L) ;

}

BOOL WINAPI IsTimeoutOver( LPTIMEOUT lpTimeout )
{
   TIMERINFO   ti ;

   ti.dwSize = sizeof(ti) ;
   TimerCount( &ti ) ;

   if (LOWORD(ti.dwmsSinceStart / 1000L) >= (lpTimeout->uiStart + lpTimeout->uiTimeout))
   {
      ResetTimeout( lpTimeout ) ;
      return TRUE ;
   }

   return FALSE ;
}

VOID WINAPI ResetTimeout( LPTIMEOUT lpTimeout )
{
   lpTimeout->uiTimeout = 0 ;
   lpTimeout->uiStart = 0 ;
}


/************************************************************************
 * End of File: fsm.c
 ***********************************************************************/

