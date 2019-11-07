/************************************************************************
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  botstat.c
 *
 *
 *     Remarks:
 *    This module is responsible for the bottom status window.  It uses the
 *    "status" window class exported from CMNDLL.
 *
 *
 ************************************************************************/
#include "iniedit.h"
#include "botstat.h"
#include "cmd.h"
#include "status.h"

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
   HWND  hwndStat ;
   LONG  lNumStats = NUM_STATBOXES ;

   DP5( hWDB, "CreateStatus" ) ;

   StatusInit( 0, hinstApp ) ;
   

   if (!(hwndStat = CreateWindow(
                                 SZ_STATUSCLASSNAME,
                                 "",
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

   SetDirtyIndicator( hwndStat, fDirty ) ;
   SetHelpAvailIndicator( hwndStat, FALSE ) ;

   return hwndStat ;

}/* CreateStatus() */

/*************************************************************************
 *  void FAR PASCAL SetDirtyIndicator( HWND hwndStat, BOOL fOn )
 *
 *  Description:
 *    This function sets the Diary status indicator on and off.  
 *
 *  Type/Parameter
 *          Description
 *  BOOL    fOn
 *          Indicates whether the status should be on or off.  If this
 *          value is TRUE then the status is turned on.
 * 
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL SetDirtyIndicator( HWND hwndStat, BOOL fOn )
{
   if (hwndStat)
      StatusSetStatBox( hwndStat, IDSTAT_DIRTY,
         (LPARAM)(fOn ? rglpsz[IDS_DIRTY] : rglpsz[IDS_CLEAN]) ) ;

}/* SetDiaryIndicator() */

/*************************************************************************
 *  void FAR PASCAL SetHelpAvailIndicator( HWND hwndStat, BOOL fOn )
 *
 *  Description:
 *    This function sets the Diary status indicator on and off.  
 *
 *  Type/Parameter
 *          Description
 *  BOOL    fOn
 *          Indicates whether the status should be on or off.  If this
 *          value is TRUE then the status is turned on.
 * 
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL SetHelpAvailIndicator( HWND hwndStat, BOOL fOn )
{
   if (hwndStat)
      StatusSetStatBox( hwndStat, IDSTAT_HELP, (fOn ? rglpsz[IDS_HELP] : rglpsz[IDS_NOHELP]) ) ;

}/* SetDiaryIndicator() */

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
   short n1, n2 ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_DIRTY] ) ;
   n2 = StatusGetWidth( hwndStat, rglpsz[IDS_CLEAN] ) ;

   StatusSetStatBoxSize( hwndStat, IDSTAT_DIRTY, max(n1,n2) ) ;

   n1 = StatusGetWidth( hwndStat, rglpsz[IDS_HELP] ) ;
   n2 = StatusGetWidth( hwndStat, rglpsz[IDS_NOHELP] ) ;

   StatusSetStatBoxSize( hwndStat, IDSTAT_HELP, max(n1,n2) ) ;

}/* BuildTopBoxes() */

/************************************************************************
 * End of File: botstat.c
 ***********************************************************************/

