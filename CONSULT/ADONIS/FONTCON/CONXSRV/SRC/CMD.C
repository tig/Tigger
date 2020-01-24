/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  cmd.c
 *
 *     Remarks:  Command handler for the menu.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "connect.h"
#include "cmd.h"
#include "dde.h"
#include "config.h"
#include "menu.h"
#include "botstat.h"
#include "subdlgs.h"


/****************************************************************
 *  LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Handles all WM_COMMAND messages
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   BOOL fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;

   if (wMsg != WM_COMMAND)
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   switch( (WORD)wParam )
   {
      /*
       * File Menu
       */
      case IDM_FILE_EXIT:
         SendMessage (hWnd, WM_CLOSE, 0, 0L) ;
      break ;

      case IDM_CONNECT:
         DoConnect() ;
      break ;

      case IDM_DISCONNECT:
         DoDisConnect() ;
      break ;

      case IDM_RESETDDE:
         ddeUnInitDDEServer( ) ;
         if (!ddeInitDDEServer( ))
         {
            DP1( hWDB, "ddeInitDDEServer() failed!" ) ;

            ErrorResBox( hWnd, hinstApp,
                        MB_ICONHAND, IDS_APPTITLE, 
                        IDS_ERR_DDESERVERINIT ) ;
         }
      break ;
      
      /*
       * Options Menu
       */
      case IDM_OPT_FONT:
         if (DoChooseFont())
         {
            InvalidateRect( hwndTTY, NULL, TRUE ) ;
         }
      break ;

      /*
       * Help Menu
       */
      case IDM_HELP_CONTENTS:
         //WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_CONTENTS, 0L ) ;
      break ;

      case IDM_HELP_ONHELP:
         //WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_HELPONHELP, 0L ) ;
      break ;

      case IDCHILD_STAT:
      case IDM_HELP_ABOUT:
         DoAboutBox() ;
      break ;

      #ifdef DEBUG
      case IDM_WDB_CONFIG:
         wdbConfigDlg( hWDB ) ;
      break ;
      #endif

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* CmdHandler()  */

/****************************************************************
 *  LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Handles all WM_SYSCOMMAND messages
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   if (wMsg != WM_SYSCOMMAND)
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   switch( (WORD)wParam )
   {

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* SysCmdHandler()  */


/****************************************************************
 *  void NEAR PASCAL UpdateInfo( void ) ;
 *
 *  Description: 
 *
 *    Updates all status indicators to reflect the current state
 *    of the app.  It sets the window text of the stat bar to indicate
 *    the current filename, section, and key.
 *
 *  Comments:
 *
 ****************************************************************/
void WINAPI UpdateInfo( void )
{


} /* UpdateInfo()  */

/************************************************************************
 * End of File: cmd.c
 ***********************************************************************/

