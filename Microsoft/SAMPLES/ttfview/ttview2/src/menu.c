/************************************************************************
 *
 *     Project:  TTVIEW
 *
 *      Module:  menu.c
 *
 *     Remarks:  Handles all extra menu processing (help messages).
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "global.h"


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
       return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;

   switch ( wMsg )
   {
      case WM_INITMENU:
         // 
         // Get the handle to the window here, this way we store
         // it a way and dont have to waste time getting it later
         //
         hMenu = (HMENU)wParam ;
         DP5( hWDB, "WM_INITMENU:      %04X, %08lX", wParam, lParam ) ;
      break ;

      case WM_INITMENUPOPUP:
         return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;
         DP5( hWDB, "WM_INITMENUPOPUP: %04X, %08lX", wParam, lParam ) ;
      break ;

      case WM_MENUSELECT:
         if (!ghwndStat)
         {
            DP5( hWDB,"!ghwndStat" ) ;
            break ;
         }

         DPF5( hWDB, "WM_MENUSELECT:   %04X, %08lX", wParam, lParam ) ;

         if (LOWORD(lParam) == 0xFFFF ||
             HIWORD(lParam) == 0 ||
             (WORD)wParam == 0)
         {
            DP5( hWDB, "  Noting selected" ) ;
            //
            // If nothing is selected anymore then set the status line
            // to "".  The subclassing of the WM_SETTEXT message for the
            // status line will trap this and put the current status
            // there.
            //
            SetWindowText( ghwndStat, rglpsz[IDS_STAT_READY] ) ;
            break ;
         }

         if (LOWORD(lParam) & MF_HILITE)
         {   

            if ((LOWORD(lParam) & MF_POPUP))
            {
               UINT  uiSysMenu = 0 ;

               DP5( hWDB, "  MF_POPUP" ) ;
               DP5( hWDB, ", SubMenu = %04X", GetSubMenu( hMenu, 0 ) ) ;

               if (ghwndActiveMDIChild &&
                   IsWindow( ghwndActiveMDIChild ) &&
                   IsZoomed( ghwndActiveMDIChild ))
                  uiSysMenu = 1 ;

               if (GetSubMenu( hMenu, uiSysMenu + 0 ) == (HMENU)wParam)
               {
                  SetWindowText( ghwndStat, GRCS( IDM_FILE ) ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, uiSysMenu + 1 ) == (HMENU)wParam)
               {
                  SetWindowText( ghwndStat, GRCS( IDM_OPT ) ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, uiSysMenu + 2 ) == (HMENU)wParam)
               {
                  SetWindowText( ghwndStat, GRCS( IDM_WINDOW ) ) ;
                  break ;
               }

               if (GetSubMenu( hMenu, uiSysMenu + 3 ) == (HMENU)wParam)
               {
                  SetWindowText( ghwndStat, GRCS( IDM_HELP ) ) ;
                  break ;
               }

               /*
                * If we get here it is either the sys menu icon or
                * one of our popup sub-menus.
                *
                * Go through each of our popups and see if it's it.
                * We do this by getting 
                */

               SetWindowText( ghwndStat, " " ) ;

            }
            else /* if MF_POPUP */
               if (LOWORD((DWORD)lParam & MF_SYSMENU))
               {
                  DP5( hWDB, "  MF_SYSMENU" ) ;
                  // 
                  // It is a system menu item.
                  //
                  switch ((WORD)wParam)
                  {
                     case SC_SIZE:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_SIZE ) ) ;
                     break ;

                     case SC_MOVE:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_MOVE ) ) ;
                     break ;

                     case SC_MINIMIZE:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_MINIMIZE ) ) ;
                     break ;

                     case SC_MAXIMIZE:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_MAXIMIZE ) ) ;
                     break ;

                     case SC_CLOSE:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_CLOSE ) ) ;
                     break ;

                     case SC_RESTORE:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_RESTORE ) ) ;
                     break ;

                     case SC_TASKLIST:
                        SetWindowText( ghwndStat, GRCS( IDS_SMNU_TASKLIST ) ) ;
                     break ;
                  }
               } /* MF_SYSMENU */
               else
               {
                  DP5( hWDB, "  Normal menu item" ) ;
                  //
                  // It's a normal menu item, wParam has the IDM
                  //

                  if ((WORD)wParam <= IDM_WINDOW_CHILD)
                     SetWindowText( ghwndStat, GRCS( (WORD)wParam ) ) ;
                  else
                     SetWindowText( ghwndStat, " " ) ;

               }
         } /* MF_HILITE */

      break ;
   }

   return DefFrameProc( hWnd, ghwndMDIClient, wMsg, wParam, lParam ) ;

} /* MenuHandler()  */


/************************************************************************
 * End of File: menu.c
 ***********************************************************************/

