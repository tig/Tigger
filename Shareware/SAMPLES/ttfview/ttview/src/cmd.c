/************************************************************************
 *
 *     Project:  TTVIEW
 *
 *      Module:  cmd.c
 *
 *     Remarks:  Command handler
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "..\inc\TTVIEW.h"
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include "..\inc\cmd.h"
#include "..\inc\config.h"
#include "..\inc\menu.h"
#include "..\inc\botstat.h"
#include "..\inc\subdlgs.h"
#include "..\inc\dlghelp.h"


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

      case IDM_FILE_PRINTSETUP:
         DoPrinterSetup() ;
      break ;

      case IDM_FILE_PRINT:
      break ;

      case IDM_FILE_EXIT:
         SendMessage (hWnd, WM_CLOSE, 0, 0L) ;
      break ;


      
      /*
       * Options Menu
       */
      case IDM_OPT_FONT:
         if (DoChooseFont())
         {
            InvalidateRect( hwndClient, NULL, TRUE ) ;
         }
      break ;


      case IDM_OPT_STAT:
      {
         RECT  rc ;

         SetWaitCursor() ;

         fStatLine = !(GetMenuState( GetMenu( hWnd ), (WORD)wParam,
                                   MF_BYCOMMAND ) & MF_CHECKED)  ;
         if (fStatLine)
         {
            if (hwndStat)
               SetWindowPos( hwndStat, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW ) ;
            else
            {
               if (hwndStat = CreateStatus( hWnd ))
                   SetWindowPos( hwndStat, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW ) ;
               else
                  fStatLine = FALSE ;
            }
         }
         else
            if (hwndStat)
               ShowWindow( hwndStat, SW_HIDE ) ;

         GetClientRect( hwndApp, &rc ) ;
         SendMessage( hwndApp, WM_SIZE, (WPARAM)SIZENORMAL,
            (LPARAM)MAKELONG( rc.right-rc.left, rc.bottom-rc.top ) ) ;

         CheckMenuItem( GetMenu( hWnd ), (WORD)wParam,
                        fStatLine ? MF_CHECKED : MF_UNCHECKED ) ;

         /*
          * Write to INI file
          */
         SaveOptions( ) ;
         UpdateInfo( ) ;

         SetNormalCursor() ;

      }
      break ;


      /*
       * Help Menu
       */
      case IDM_HELP_CONTENTS:
         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_CONTENTS, 0L ) ;
      break ;

      case IDM_HELP_ONHELP:
         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_HELPONHELP, 0L ) ;
      break ;

      case IDCHILD_STAT:
      case IDM_HELP_ABOUT:
         DoAboutBox() ;
      break ;

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

