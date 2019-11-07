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
#include "PRECOMP.H"
#include "global.h"


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
      return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;

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
//            InvalidateRect( hwndClient, NULL, TRUE ) ;
         }
      break ;


      case IDM_OPT_STAT:
      {
         RECT  rc ;

         SetWaitCursor() ;

         gfStatLine = !(GetMenuState( GetMenu( hWnd ), (WORD)wParam,
                                   MF_BYCOMMAND ) & MF_CHECKED)  ;
         if (gfStatLine)
         {
            if (ghwndStat)
               SetWindowPos( ghwndStat, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW ) ;
            else
            {
               if (ghwndStat = CreateStatus( hWnd ))
                   SetWindowPos( ghwndStat, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW ) ;
               else
                  gfStatLine = FALSE ;
            }
         }
         else
            if (ghwndStat)
               ShowWindow( ghwndStat, SW_HIDE ) ;

         GetClientRect( ghwndApp, &rc ) ;
         SendMessage( ghwndApp, WM_SIZE, (WPARAM)SIZENORMAL,
            (LPARAM)MAKELONG( rc.right-rc.left, rc.bottom-rc.top ) ) ;

         CheckMenuItem( GetMenu( hWnd ), (WORD)wParam,
                        gfStatLine ? MF_CHECKED : MF_UNCHECKED ) ;

         /*
          * Write to INI file
          */

         UpdateInfo( ) ;

         SetNormalCursor() ;

      }
      break ;

      /* Window menu
       */
      case IDM_WINDOW_TILE:
         SendMessage( ghwndMDIClient, WM_MDITILE, 0, 0L ) ;
      break ;

      case IDM_WINDOW_CASCADE:
         SendMessage( ghwndMDIClient, WM_MDICASCADE, 0, 0L ) ;
      break ;

      case IDM_WINDOW_ICONS:
         SendMessage( ghwndMDIClient, WM_MDIICONARRANGE, 0, 0L ) ;
      break ;

      case IDM_WINDOW_CLOSEALL:
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
         return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;
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
      return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;

   switch( (WORD)wParam )
   {

      default:
         return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;
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

