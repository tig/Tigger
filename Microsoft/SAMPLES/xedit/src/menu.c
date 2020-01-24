/************************************************************************
 *
 *     Project:  XEDIT 2.0
 *
 *      Module:  menu.c
 *
 *     Remarks:  Handles all extra menu processing (help messages).
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "..\inc\XEDIT.h"
#include "..\inc\cmd.h"
#include "..\inc\menu.h"


/****************************************************************
 *  LRESULT FAR PASCAL MenuHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT FAR PASCAL MenuHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   static HMENU   hMenu ;

   if (wMsg != WM_INITMENU &&
       wMsg != WM_MENUSELECT &&
       wMsg != WM_INITMENUPOPUP)
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   switch ( wMsg )
   {
      case WM_INITMENU:
         // 
         // Get the handle to the window here, this way we store
         // it a way and dont have to waste time getting it later
         //
         hMenu = (HMENU)wParam ;
         DPF5( hWDB, "WM_INITMENU:      %04X, %08lX", wParam, lParam ) ;
      break ;

      case WM_INITMENUPOPUP:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         DPF5( hWDB, "WM_INITMENUPOPUP: %04X, %08lX", wParam, lParam ) ;
      break ;

      case WM_MENUSELECT:
         if (!hwndStat)
         {
            DP5( hWDB,"!hwndStat" ) ;
            break ;
         }

         DPF5( hWDB, "WM_MENUSELECT:   %04X, %08lX", wParam, lParam ) ;

         if (LOWORD(lParam) == 0xFFFF ||
             HIWORD(lParam) == 0 ||
             (WORD)wParam == 0)
         {
            DPF5( hWDB, "  Noting selected" ) ;
            //
            // If nothing is selected anymore then set the status line
            // to "".  The subclassing of the WM_SETTEXT message for the
            // status line will trap this and put the current status
            // there.
            //
            SetWindowText( hwndStat, "" ) ;
            break ;
         }

         if (LOWORD(lParam) & MF_HILITE)
         {   

            if ((LOWORD(lParam) & MF_POPUP))
            {
               DPF5( hWDB, "  MF_POPUP" ) ;

               DPF5( hWDB, ", SubMenu = %04X", GetSubMenu( hMenu, 0 ) ) ;
               if (GetSubMenu( hMenu, 0 ) == (HMENU)wParam)
               {
                  SetWindowText( hwndStat, rglpsz[IDS_MNU_FILE] ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, 1 ) == (HMENU)wParam)
               {
                  SetWindowText( hwndStat, rglpsz[IDS_MNU_EDIT] ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, 2 ) == (HMENU)wParam)
               {
                  SetWindowText( hwndStat, rglpsz[IDS_MNU_SEARCH] ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, 3 ) == (HMENU)wParam)
               {
                  SetWindowText( hwndStat, rglpsz[IDS_MNU_OPT] ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, 4 ) == (HMENU)wParam)
               {
                  SetWindowText( hwndStat, rglpsz[IDS_MNU_HELP] ) ;
                  break ;
               }

               /*
                * If we get here it is either the sys menu icon or
                * one of our popup sub-menus.
                *
                * Go through each of our popups and see if it's it.
                * We do this by getting 
                */

               SetWindowText( hwndStat, " " ) ;

            }
            else /* if MF_POPUP */
               if (LOWORD((DWORD)lParam & MF_SYSMENU))
               {
                  DPF5( hWDB, "  MF_SYSMENU" ) ;
                  // 
                  // It is a system menu item.
                  //
                  switch ((WORD)wParam)
                  {
                     case SC_SIZE:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_SIZE] ) ;
                     break ;

                     case SC_MOVE:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_MOVE] ) ;
                     break ;

                     case SC_MINIMIZE:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_MINIMIZE] ) ;
                     break ;

                     case SC_MAXIMIZE:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_MAXIMIZE] ) ;
                     break ;

                     case SC_CLOSE:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_CLOSE] ) ;
                     break ;

                     case SC_RESTORE:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_RESTORE] ) ;
                     break ;

                     case SC_TASKLIST:
                        SetWindowText( hwndStat, rglpsz[IDS_SMNU_TASKLIST] ) ;
                     break ;
                  }
               } /* MF_SYSMENU */
               else
               {
                  DPF5( hWDB, "  Normal menu item" ) ;
                  //
                  // It's a normal menu item, wParam has the IDM
                  //
                  if (((WORD)wParam + IDS_MNU_FILE) <= LAST_IDS)
                     SetWindowText( hwndStat,
                        rglpsz[(WORD)wParam + IDS_MNU_FILE] ) ;
                  else
                     SetWindowText( hwndStat, " " ) ;

               }
         } /* MF_HILITE */

      break ;
   }
   DP5( hWDB, " ." ) ;

   return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

} /* MenuHandler()  */

/****************************************************************
 *  void WINAPI EnableFindReplace( BOOL fEnable )
 *
 *  Description: 
 *
 *    Disables the find/replace menu items.
 *
 *  Comments:
 *
 ****************************************************************/
void WINAPI EnableFindReplace( BOOL fEnable )
{
   HMENU hMenu = GetMenu( hwndApp ) ;

   EnableMenuItem( hMenu, IDM_SEARCH_FIND,
         MF_BYCOMMAND | (fEnable ? MF_ENABLED : MF_GRAYED) ) ;

   EnableMenuItem( hMenu, IDM_SEARCH_FINDNEXT,
         MF_BYCOMMAND | (fEnable ? MF_ENABLED : MF_GRAYED) ) ;

   EnableMenuItem( hMenu, IDM_SEARCH_REPLACE,
         MF_BYCOMMAND | (fEnable ? MF_ENABLED : MF_GRAYED) ) ;


} /* DisableFindReplace()  */

/************************************************************************
 * End of File: menu.c
 ***********************************************************************/

