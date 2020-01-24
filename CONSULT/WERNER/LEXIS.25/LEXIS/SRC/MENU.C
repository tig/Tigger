/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  menu.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module is responsible for the menus.  It handles such
 *    tasks as telling the message line that the user is on such
 *    and such menu item and so forth.  It also has APIs for chang-
 *    ing the menu for the other personalities of lexis.  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/
#include <windows.h>
#include <cmndll.h>        // is this needed?

#include "..\inc\lexis.h"
#include "..\inc\menu.h"


/************************************************************************
 * Local variables
 ************************************************************************/
//
// This is the handle to the current menu.
//
HMENU hCurrentMenu = 0 ;

/************************************************************************
 * Exported/External Functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL SetLexisMenu( HWND hWnd, wMenu )
 *
 *  Description:
 *    Creates and displays one of the three menu's.  If a previous menu
 *    was set, this will destroy it and then create the new one.
 *
 *  Type/Parameter
 *          Description
 *    WORD  wMenu
 *          One of LEGALMENU, CORPMENU, COMBINEDMENU
 *
 * 
 *  Return Value
 *     TRUE if ok, FALSE if not.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL SetLexisMenu( HWND hWnd, WORD wMenu )
{
   DP5( hWDB, "SetLexisMenu" ) ;
   if (hCurrentMenu)
   {
      SetMenu( hWnd, NULL ) ;
      DestroyMenu( hCurrentMenu ) ;
   }
 
   hCurrentMenu = LoadMenu( GETHINST( hWnd ), MAKEINTRESOURCE( wMenu ) ) ;
   if (!hCurrentMenu)
   {
      DP1( hWDB, "Could not load menu %d", wMenu ) ;
      return FALSE ;
   }

   SetMenu( hWnd, hCurrentMenu ) ;

   return TRUE ;

}/* CreateLexisMenu() */

/*************************************************************************
 *  BOOL FAR PASCAL CleanupLexisMenu( HWND hWnd )
 *
 *  Description:
 *    Destroys the current menu.  Must be called if SetLexisMenu is
 *    ever called.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL CleanupLexisMenu( HWND hWnd )
{
   DP5( hWDB,"CleanupLexisMenu") ;
   if (hCurrentMenu)
   {
      SetMenu( hWnd, NULL ) ;
      DestroyMenu( hCurrentMenu ) ;
   }
   else
      return FALSE ;

   return TRUE ;
}/* CleanupMenu() */


/*************************************************************************
 *  BOOL FAR PASCAL
 *    ProcessMenu( HWND hWnd, WORD iMsg, WORD wParam, LONG lParam )
 *
 *  Description:
 *    This function processes WM_INITMENU and WM_MENUSELECT messages
 *    for the purpose of writing messages to the status bar (bottom)
 *    indiacting which menu item is selected.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ProcessMenu( HWND hWnd, WORD iMsg, WORD wParam, LONG lParam )
{
   char  szText[MAX_STRLEN + 1] ;
   HWND  hwndBotStat = GETHWNDBOTSTAT( hWnd ) ;
   WORD  w ;
   static HMENU hMenu ;

   switch ( iMsg )
   {
      case WM_INITMENU:
         // 
         // Get the handle to the window here, this way we store
         // it a way and dont have to waste time getting it later
         //
         hMenu = wParam ;
      break ;

      case WM_MENUSELECT:
         if (!hwndBotStat)
         {
            DP5( hWDB,"!hwndBotStatus" ) ;
            break ;
         }

         if (LOWORD(lParam) == 0xFFFF || HIWORD(lParam) == 0)
         {
            //
            // If nothing is selected anymore then set the status line
            // to "".  The subclassing of the WM_SETTEXT message for the
            // status line will trap this and put the current status
            // there.
            //
            SetWindowText( hwndBotStat, "" ) ;
            break ;
         }

         w = 0 ;
         //
         // Whenever a menu is hilited.
         // We will store the IDS of the appropriate string in the w 
         // variable, only doing the loadstring once at the end.  
         //
         if (LOWORD(lParam) & MF_HILITE)
         {   

            if ((LOWORD(lParam) & MF_POPUP))
            {
               WORD  pos ;

               //
               // If it's a popup window, first assume it's the SYSMENU.
               // Go through all possible popups to see if it's that
               // popup.
               // 
               w = IDS_SYSMENU ;

               for ( pos = IDM_TL_FILE ; pos <= IDM_TL_HELP ; pos++)
               {
                  if (GetSubMenu( hMenu, pos - IDM_TL_FILE ) == wParam)
                  {
                     w = pos ;
                     break ;
                  }
               }
            }
            else
               if (LOWORD(lParam & MF_SYSMENU))
               {
                  // 
                  // It is a system menu item.
                  //
                  switch (wParam)
                  {
                     case SC_SIZE:
                        w = IDS_SYSMENU_SIZE ;
                     break ;

                     case SC_MOVE:
                        w = IDS_SYSMENU_MOVE ;
                     break ;

                     case SC_MINIMIZE:
                        w = IDS_SYSMENU_MINIMIZE ;
                     break ;

                     case SC_MAXIMIZE:
                        w = IDS_SYSMENU_MAXIMIZE ;
                     break ;

                     case SC_CLOSE:
                        w = IDS_SYSMENU_CLOSE ;
                     break ;

                     case SC_RESTORE:
                        w = IDS_SYSMENU_RESTORE ;
                     break ;

                     case SC_TASKLIST:
                        w = IDS_SYSMENU_TASKLIST ;
                     break ;
                  }
               }
               else
                  //
                  // It's a normal menu item, wParam has the IDM
                  //
                  w = wParam ;

         }
         if (w)
         {
            if (LoadString( GETHINST( hWnd ), w, szText, MAX_STRLEN ))
               SetWindowText( hwndBotStat, szText ) ;
         }
         else
            SetWindowText( hwndBotStat, "" ) ;
      break ;

   }
   return TRUE ;
}/* ProcessMenu() */

/************************************************************************
 * End of File: menu.c
 ************************************************************************/

