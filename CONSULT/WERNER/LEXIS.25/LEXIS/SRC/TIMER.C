/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  timer.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module is responsible for handling all timers and dispatching
 *    all timer events to the appropriate windows.
 *
 *    It contains a timer procedure that is called as fast as windows
 *    can get it WM_TIMER message. WM_TIMER messages are then sent to
 *    all windows that need them, in the frequency they require.
 *
 *
 *   Revisions:  
 *     00.00.000  1/22/91 cek   First Version
 *
 *
 ************************************************************************/

#include "..\inc\undef.h"
#include <windows.h>
#include <string.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\timer.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/
#define BOTSTAT_TIME    18*60  // minute
#define TOPSTAT_TIME    18     // second
#define TERM_TIME       0     // how often to the term?

#define ID_TIMER        42    // just a random number (unique?)

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Local Variables
 ************************************************************************/
WORD wTopStat ;
WORD wBotStat ;
WORD wTerm ;

FARPROC lpfnTimer ;

/************************************************************************
 * Exported/External Functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL CreateTimer( HWND hWnd )
 *
 *  Description:
 *    This function is called to set up the timers.  
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND hWnd
 *          Handle to the main window.
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *          FALSE       There were not enough windows timers to fill
 *                      the request.
 *
 *          TRUE        Ok.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL CreateTimer( HWND hWnd )
{
   //
   // Initialize local counters
   //
   wTopStat = 0 ;
   wBotStat = 0 ;
   wTerm = 0 ;

   //
   // Create the timer event
   //
   if (lpfnTimer = MakeProcInstance( fnTimer, GETHINST( hWnd ) ))
   {
      if (!SetTimer( hWnd, ID_TIMER, 1, lpfnTimer ))
      {
         FreeProcInstance( lpfnTimer ) ;
         lpfnTimer = NULL ;
         return FALSE ;
      }
      DP5( hWDB, "Timer created!" ) ;
      return TRUE ;
   }
   else
      return FALSE ;

}/* CreateTimer() */


/*************************************************************************
 *  BOOL FAR PASCAL DestroyTimer( HWND hWnd )
 *
 *  Description:
 *
 *    This function kills the timer created by CreateTimer.
 *
 *  Return Value
 *
 *       TRUE  Success
 *       FALSE Failure
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DestroyTimer( HWND hWnd )
{
   if (lpfnTimer)
   {
      if (KillTimer( hWnd, ID_TIMER ))
      {
         FreeProcInstance( lpfnTimer ) ;
         DP5( hWDB, "Timer Destroyed!" ) ;
         return TRUE ;
      }
      else
         return FALSE ;
   }
   return TRUE ;

}/* DestroyTimer() */

/*************************************************************************
 *  WORD FAR PASCAL fnTimer( HWND hWnd,
 *                           WORD iMessage, WORD wParam, LONG lParam )
 *
 *  Description:
 *    This is the timer proc that is called by Windows.  This function
 *    MUST be in the applciations DEF file in the EXPORTS section!!!
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL
fnTimer( HWND hWnd, WORD iMessage, WORD wParam, LONG lParam )
{
   HWND hwndTopStat = GETHWNDTOPSTAT( hWnd ) ;
   HWND hwndBotStat = GETHWNDBOTSTAT( hWnd ) ;
   HWND hwndTerm =    GETHWNDTERM( hWnd ) ;

   if (hwndBotStat && ++wBotStat == BOTSTAT_TIME)
   {
      wBotStat = 0 ;
      SendMessage( hwndBotStat, WM_TIMER, wParam, 0L ) ;
   }

   #ifdef TIMEDISPLAY
   if (hwndTopStat && ++wTopStat == TOPSTAT_TIME)
   {
      wTopStat = 0 ;
      SendMessage( hwndTopStat, WM_TIMER, wParam, 0L ) ;
   }
   #endif
   return 0 ;

}/* fnTimer() */

/************************************************************************
 * Local Functions
 ************************************************************************/

/************************************************************************
 * End of File: timer.c
 ************************************************************************/

