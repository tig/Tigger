/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  mainwnd.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *   This module contains the main window procedure and support routines
 *   It is responsible for keeping the main window and all it's children
 *   up to date and organized.
 *
 *   Revisions:  
 *     00.00.000  1/20/91 cek   first version
 *
 *
 ************************************************************************/

#include "..\inc\undef.h"
#include <windows.h>

#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\timer.h"
#include "..\inc\menu.h"
#include "..\inc\topstat.h"
#include "..\inc\botstat.h"
#include "..\inc\button.h"
#include "..\inc\state.h"
#include "..\inc\userpref.h"
#include "..\inc\profile.h"
#include "..\inc\print.h"
#include "..\inc\filedlgs.h"
#include "..\inc\hardware.h"
#include "..\inc\help.h"
#include "..\inc\print.h"
#include "..\inc\term.h"

#include "..\inc\mainwnd.h"
#include "..\inc\direct.h"

#include "..\inc\global.h"
#include "..\inc\command.h"


/************************************************************************
 * Local Defines
 ************************************************************************/
/************************************************************************
 * Macros 
 ************************************************************************/
#define SYSERROR(a)

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
LONG NEAR PASCAL CreateMain( HWND hWnd ) ;
void NEAR PASCAL PaintMain( HWND hWnd ) ;
void NEAR PASCAL SizeMain( HWND hWnd, WORD wParam, LONG lParam ) ;
void NEAR PASCAL DestroyMain( HWND hWnd ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * Exported Fuctions
 ************************************************************************/
/*************************************************************************
 *  LONG FAR PASCAL MainWndProc( HWND     hWnd,
 *
 *  Description:
 *
 *  Main Window procedure.
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL fnMainWnd( HWND      hWnd,
                           unsigned  iMessage,  
                           WORD      wParam,			 
                           LONG      lParam )			  
{
   switch (iMessage)
   {
      case WM_CREATE:
         DP5( hWDB, "WM_CREATE: lexis" ) ;
         if (!CreateMain( hWnd ))
            return -1 ;
      break ;

      case WM_KEYDOWN :
      case WM_MOUSEMOVE:
         CheckKeyStates() ;
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      break ;

      case WM_SETFOCUS:
      {
         HWND hwndTerm = GETHWNDTERM( hWnd ) ;
         if (hwndTerm)
            SetFocus( hwndTerm ) ;
      }
      break ;

      case WM_INITMENU:
      case WM_MENUSELECT:
      case WM_INITMENUPOPUP:
         ProcessMenu( hWnd, iMessage, wParam, lParam ) ;
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      break ;

	   case WM_COMMAND:
         if (!ProcessMainCmd( hWnd, wParam, lParam ))
            return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      break ;

      case WM_WININICHANGE:
      {
         HWND hwndStat ;

         if (!lstrcmpi( "intl", (LPSTR)lParam ))
         {
            if (hwndStat = GETHWNDBOTSTAT( hWnd ))
               SendMessage( hwndStat, WM_WININICHANGE, 0, lParam ) ;
         
            #ifdef TIMEDISPLAY
            if (hwndStat = GETHWNDTOPSTAT( hWnd ))
               SendMessage( hwndStat, WM_WININICHANGE, 0, lParam ) ;
            #endif
         }
      }
      break ;

      case WM_PAINT:
         PaintMain( hWnd ) ;
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      break ;

      case WM_SIZE:
         SizeMain( hWnd, wParam, lParam ) ;
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
      break ;


	   case WM_DESTROY:
         Help( HELP_QUIT, NULL, 0 ) ;
         DestroyMain( hWnd ) ;
	   break;

	   default:		
	      return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
   }
   return 0L ;
}/* MainWndProc() */

/*************************************************************************
 *  void FAR PASCAL PlaceChildWindows( HWND hWnd, WORD wWidth, WORD wHeight )
 *
 *  Description:
 *    This function places and sizes the child windows within the main
 *    Window.  
 *
 *  Type/Parameter
 *          Description
 *    wWidth is the width of the parent, wHeight is the height...
 *
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL PlaceChildWindows( HWND hWnd, WORD wWidth, WORD wHeight )
{
   WORD     wHeightTop = 0;
   WORD     wHeightBot = 0;
   WORD     wHeightBut = 0;
   HWND     hwndChild ;
   RECT     rc ;
   HCURSOR  hCur = SetHourGlass() ;
   WORD     wTermTop ;

   if (hwndChild = GETHWNDTOPSTAT( hWnd ))
   {
      wHeightTop = (WORD)SendMessage( hwndChild, ST_GETHEIGHT, 0, 0L ) ;
      GetClientRect( hwndChild, &rc ) ;
      if ((WORD)(rc.right - rc.left) < wWidth)
      {
         MoveWindow( hwndChild,
                     0,
                     - 1,
                     wWidth,
                     wHeightTop,
                     TRUE ) ;
      }
      wTermTop = wHeightTop - 1;
   }
   else
      wTermTop = 0 ;

   if (hwndChild = GETHWNDBOTSTAT( hWnd ))
   {

      wHeightBot = (WORD)SendMessage( hwndChild, ST_GETHEIGHT, 0, 0L ) ;
      MoveWindow( hwndChild,
                  0,
                  (wHeight - wHeightBot) + 1,
                  wWidth,
                  wHeightBot,
                  TRUE ) ;
   }

   if (hwndChild = GETHWNDBUTTON( hWnd ))
   {
      wHeightBut = (WORD)SendMessage( hwndChild, BU_GETHEIGHT, 0, 0L ) ;
      MoveWindow( hwndChild,
                  0,
                  wHeight - (wHeightBot + wHeightBut) + 1 ,
                  wWidth,
                  wHeightBut,
                  TRUE ) ;
   }

   if (hwndChild = GETHWNDTERM( hWnd ))
   {
      MoveWindow( hwndChild,
                  0, wTermTop, wWidth,
                  wHeight - (wHeightBot + wHeightBut + wTermTop)+1, TRUE ) ;
   }
   else
   {
      DP5( hWDB, "No Term window to place!" ) ;
   }


   ResetHourGlass( hCur ) ;

}/* PlaceChildWindows() */


/************************************************************************
 * Functions local to this module
 ************************************************************************/
/*************************************************************************
 *  LONG NEAR PASCAL CreateMain( HWND hWnd ) ;
 *
 *  Description:
 *    This function handles the WM_CREATE case for the main window
 *    procedure.  Child windows are created here.
 *
 *  Return Value
 *          Returns one of the following values:
 * 
 *     TRUE        Create was sucessful.
 *     FALSE       There was some error.
 *
 *  Comments:
 *
 *************************************************************************/
LONG NEAR PASCAL CreateMain( HWND hWnd ) 
{
   HWND hwndTemp ;
   LPLEXISSTATE lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals ) ;

   if (!lpLS)
   {
      SYSERROR(1) ;
      return FALSE ;
   }

   if (lpLS->fTopStat)
   {
      if (hwndTemp = CreateTopStatus( hWnd ))
         SetWindowWord( hWnd, GWW_HWNDTOPSTAT, hwndTemp ) ;
      else
      {
         SYSERROR(1) ;
         GlobalUnlock( ghGlobals ) ;
         return FALSE ;
      }
   }
   else
      SetWindowWord( hWnd, GWW_HWNDTOPSTAT, 0 ) ;


   //
   // Set the current state
   //
   lpLS->wState = IDS_STATE_OFFLINE ;

   if (!LoadString( GETHINST( hWnd ), IDS_STATE_OFFLINE,
                     lpLS->szCurrentState, MAX_STRLEN ))
      {
         SYSERROR(1) ;
         GlobalUnlock( ghGlobals ) ;
         DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;
         return FALSE ;
      }

   if (lpLS->fBotStat)
   {
      if (hwndTemp = CreateBotStatus( hWnd ))
         SetWindowWord( hWnd, GWW_HWNDBOTSTAT, hwndTemp ) ;
      else
      {
         SYSERROR(1) ;
         GlobalUnlock( ghGlobals ) ;
         DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;
         return FALSE ;
      }

      SetWindowText( hwndTemp, lpLS->szCurrentState ) ;
      SetCDIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;

      SetRecordIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
      SetDiaryIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
      SetPrintIndicator( GETHWNDBOTSTAT( hWnd ), TRUE ) ;
   }
   else
      SetWindowWord( hWnd, GWW_HWNDBOTSTAT, 0 ) ;

   if (lpLS->fButtonWnd)
   {
      if (hwndTemp = CreateButtonWnd( hWnd ))
         SetWindowWord( hWnd, GWW_HWNDBUTTON, hwndTemp ) ;
      else
      {
         SYSERROR(1) ;
         GlobalUnlock( ghGlobals ) ;
         DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;
         DestroyBotStatus( GETHWNDBOTSTAT( hWnd ) ) ;
         DP1( hWDB, "Could not create botton window") ;
         return FALSE ;
      }
   }
   else
      SetWindowWord( hWnd, GWW_HWNDBUTTON, 0 ) ;

   //
   // Create the timer
   //
   if (!CreateTimer( hWnd ))
   {
      SYSERROR(1) ;
      GlobalUnlock( ghGlobals ) ;
      DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;
      DestroyBotStatus( GETHWNDBOTSTAT( hWnd ) ) ;
      DestroyButtonWnd( GETHWNDBUTTON( hWnd ) ) ;
      return FALSE ;
   }

   if (!SetPersonality( hWnd, lpLS->wPersonality ))
   {
      GlobalUnlock( ghGlobals ) ;
      DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;
      DestroyBotStatus( GETHWNDBOTSTAT( hWnd ) ) ;
      DestroyButtonWnd( GETHWNDBUTTON( hWnd ) ) ;
      return FALSE ;
   }

   GlobalUnlock( ghGlobals ) ;

   if (hwndTemp = CreateTerm( hWnd ))
      SetWindowWord( hWnd, GWW_HWNDTERM, hwndTemp ) ;
   else
   {
      SYSERROR(1) ;
      SetWindowWord( hWnd, GWW_HWNDTERM, 0 ) ;
      DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;
      DestroyBotStatus( GETHWNDBOTSTAT( hWnd ) ) ;
      DestroyButtonWnd( GETHWNDBUTTON( hWnd ) ) ;
      DP1( hWDB, "Could not create botton window") ;
      return FALSE ;
   }

   return TRUE ;

}/* CreateMain() */

/*************************************************************************
 *  BOOL FAR PASCAL SetPersonality( hWnd, WORD wPer )
 *
 *  Description:
 *    This function sets the personality, changing the menu and button
 *    bar as appropriate...
 *
 *  Type/Parameter
 *          Description
 *
 *    WORD wPer ... 0 = LEGAL, 1 = CORPORTATE, 2 = COMBINED
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL SetPersonality( HWND hWnd, WORD wPer )
{
   char szBuf[MAX_STRLEN] ;

   HANDLE hInst = GETHINST( hWnd ) ;


   if (wPer > COMBINED)
      return FALSE ;

   //
   // First we must load and display the menu!
   //
   if (!SetLexisMenu( hWnd, wPer+1 ))
   {
      SYSERROR(1) ;
      DP1( hWDB, "Could not create the menu" ) ;
      return FALSE ;
   }

   //
   // Change the button window
   //
   SetButtonPersonality( hWnd, wPer ) ;

   //
   // Change the window text
   //
   if (LoadString( hInst, IDS_APPTITLE_LEGAL + wPer, szBuf, MAX_INILEN ))
      SetWindowText( hWnd, szBuf ) ;

   return TRUE ;

} /* SetPersonality() */


/*************************************************************************
 *  void NEAR PASCAL PaintMain( HWND hWnd ) ;
 *
 *  Description:
 *   This function handles the painting of the main window (i.e. the
 *   WM_PAINT message.
 *
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL PaintMain( HWND hWnd ) 
{
}/* PaintMain() */


/*************************************************************************
 *  void NEAR PASCAL SizeMain( hWnd, wParam, lParam ) ;
 *
 *  Description:
 *    This function handles the sizing of the main window.
 *    It is responsible for sizing all of the children.
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL SizeMain( HWND hWnd, WORD wParam, LONG lParam ) 
{

   if (wParam == SIZEICONIC)
      return ;

   PlaceChildWindows( hWnd, LOWORD( lParam ), HIWORD( lParam ) ) ;
}/* SizeMain() */



/*************************************************************************
 *  void NEAR PASCAL DestroyMain( hWnd ) ;
 *
 *  Description:
 *    This function is responsible for destroying all child window upon
 *    the WM_DESTROY message.
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL DestroyMain( HWND hWnd ) 
{
   if (!CleanupLexisMenu( hWnd ))
   {
      DP1( hWDB, "CleanupLexisMenu returned FALSE" ) ;
   }

   DP5( hWDB,"DestroyTimer.." ) ;
   DestroyTimer( hWnd ) ;

   DestroyTerm( GETHWNDTERM( hWnd ) ) ;

   DP5( hWDB,"DestroyTopStatus.." ) ;
   DestroyTopStatus( GETHWNDTOPSTAT( hWnd ) ) ;

   DP5( hWDB,"DestroyBotStatus.." ) ;
   DestroyBotStatus( GETHWNDBOTSTAT( hWnd ) ) ;

   DP5( hWDB,"DestroyButtonWnd.." ) ;
   DestroyButtonWnd( GETHWNDBUTTON( hWnd ) ) ;
   
   SaveScreenPosition( hWnd ) ;

   PostQuitMessage( 0 ) ;
}/* DestroyMain() */


/************************************************************************
 * End of File: mainwnd.c
 ************************************************************************/
