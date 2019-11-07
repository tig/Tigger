/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASYNCSRV
 *
 *      Module:  botstat.c
 *
 *     Remarks:
 *    This module is responsible for the bottom status window.  It uses the
 *    "status" window class.
 *
 *    Revsions:
 *       3/4/92   cek   Wrote it.
 *
 ************************************************************************/
#include "PRECOMP.H"
#include "ASYNCSRV.h"
#include "botstat.h"
#include "cmd.h"

/************************************************************************
 * Local Defines
 ************************************************************************/

#define MSGCOLOR        RGBBLACK

#define DUMMYWIDTH      0

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

static void NEAR PASCAL BuildBotBoxes( HWND hwndStat ) ;

/*************************************************************************
 *  HWND FAR PASCAL CreateStatus( HWND hwndParent ) ;
 *
 *  Description:
 *    This function creates, and displays the the bottom status window using
 *    the "status" window class.
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hwndParent
 *          The parent window.
 * 
 *  Return Value
 *    Returns the handle to the bottom status window, 0 if it fails to
 *    create the window.
 *
 *  Comments:
 *
 *  The bottom status bar has two "stat boxes" and one "message box".
 *  The two stat boxes, in order of appearance are:
 *
 *     IDB_DIRTY     - Indicates that something is dirty and can be saved.
 *     IDB_HELPAVAIL - Indicates the current item has help avail.
 *
 *  The syntax of the "status" window class makes the message box have
 *  the identifier 0xFFFF.
 *
 *  These boxes act very much like buttons, thus the IDB identifiers.
 *
 *************************************************************************/
HWND FAR PASCAL CreateStatus( HWND hwndParent ) 
{
   HWND   hwndStat ;
   DWORD  lNumStats = NUM_STATBOXES ;

   DP5( hWDB, "CreateStatus" ) ;

   if (!(hwndStat = CreateWindow(
                                 SZ_STATUSCLASSNAME,
                                 NULL,//"",
                                 WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
                                 0,
                                 0,
                                 1,
                                 1,
                                 hwndParent,
                                 (HMENU)IDCHILD_STAT,
                                 hinstApp,
                                 (LPSTR)&lNumStats )))
   {
      DP1( hWDB, "Stat create failed!" ) ;
      return 0 ;
   }

   SendMessage( hwndStat, WM_SETFONT, (WPARAM)hfontSmall, (LPARAM)TRUE ) ;

   BuildBotBoxes( hwndStat ) ;

   #if 0
   SendMessage( hwndStat, ST_SETDEFAULTTEXT, 0, (LPARAM)rglpsz[IDS_STAT_READY] ) ;
   #endif

   return hwndStat ;

}/* CreateStatus() */

/*************************************************************************
 *  BOOL FAR PASCAL DestroyStatus( HWND hwndStatus ) ;
 *
 *  Description:
 *    This function destroys the top status window.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DestroyStatus( HWND hwndStatus ) 
{
   if (hwndStatus)
   {
      if (DestroyWindow( hwndStatus ))
      {
         hwndStat = 0 ;
         return TRUE ;
      }
   }

   return FALSE ;

}/* DestroyStatus() */


/************************************************************************
 * Local/Private functions
 ************************************************************************/

/*************************************************************************
 *  void NEAR PASCAL BuildBotBoxes( hwndStat )
 *
 *  Description:
 *    This function figures out the top status boxes and set's them up.
 *
 *  Comments:
 *
 *************************************************************************/
static void NEAR PASCAL BuildBotBoxes( HWND hwndStat ) 
{
   UINT n1, n2 ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_RECEIVE] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_RECEIVE_LBL, n1 ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDSTAT_RECEIVE_LBL, MAKELPARAM( DT_RIGHT, 0 ) ) ;
   SendMessage( hwndStat, ST_SETBORDER, IDSTAT_RECEIVE_LBL, (LPARAM)FALSE ) ;
   StatusSetStatBox( hwndStat, IDSTAT_RECEIVE_LBL, rglpsz[IDS_RECEIVE] ) ;

   n1 = StatusGetWidth( hwndStat, "1,000,000" ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_RECEIVE, n1 ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDSTAT_RECEIVE, MAKELPARAM( DT_LEFT, 0 ) ) ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_SEND] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_SEND_LBL, n1 ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDSTAT_SEND_LBL, MAKELPARAM( DT_RIGHT, 0 ) ) ;
   SendMessage( hwndStat, ST_SETBORDER, IDSTAT_SEND_LBL, (LPARAM)FALSE ) ;
   StatusSetStatBox( hwndStat, IDSTAT_SEND_LBL, rglpsz[IDS_SEND] ) ;

   n1 = StatusGetWidth( hwndStat, "1,000,000" ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_SEND, n1 ) ;
   SendMessage( hwndStat, ST_SETJUSTIFY, IDSTAT_SEND, MAKELPARAM( DT_LEFT, 0 ) ) ;

   n1 = StatusGetWidth( hwndStat, "COM4:" ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_PORT, n1 ) ;

   n1 = StatusGetWidth( hwndStat, "38400 Baud" ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_BAUD, n1 ) ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_CD] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_CD, n1 ) ;
   SendMessage( hwndStat, ST_SETCOLOR, IDSTAT_CD, (LPARAM)RGBLTRED ) ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_DSR] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_DSR, n1 ) ;
   SendMessage( hwndStat, ST_SETCOLOR, IDSTAT_DSR, (LPARAM)RGBLTRED ) ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_CTS] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_CTS, n1 ) ;
   SendMessage( hwndStat, ST_SETCOLOR, IDSTAT_CTS, (LPARAM)RGBLTRED ) ;
                                                                   
   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_DDE_CONNECT] ) ;
   n2 = StatusGetWidth( hwndStat, rglpsz[IDS_DDE_NOTCONNECT] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_DDE, max(n1,n2) ) ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_CONNECT] ) ;
   n2 = StatusGetWidth( hwndStat, rglpsz[IDS_NOTCONNECT] ) ;
   StatusSetStatBoxSize( hwndStat, IDSTAT_CONNECT, max(n1,n2) ) ;


}/* BuildTopBoxes() */

/************************************************************************
 * End of File: botstat.c
 ***********************************************************************/
