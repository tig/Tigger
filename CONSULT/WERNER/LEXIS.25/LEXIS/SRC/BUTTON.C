/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  button.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module controls the button child window.  It is responsible
 *    for sizing, creating, destroying, drawing, and sending the
 *    appropriate messages to the parent.  It also has APIs for
 *    setting the status of the buttons, and for changing their text.
 *
 *
 *   Revisions:  
 *     00.00.000  1/26/91 cek   First Version
 *
 ************************************************************************/

#include "..\inc\undef.h"
#include <windows.h>

#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\button.h"
#include "..\inc\state.h"
#include "..\inc\term.h"
#include "..\inc\global.h"


/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/
//
// Define NOSUBCLASS if you do not want to subclass the bmpbtns for some
// reason.  They are subclassed so that we get keyboard messages and can
// move from button to button with the keyboard.
//
// #define NOSUBCLASS

#define BTNCLASSNAME "bmpbtn"


#define GWW_FOO   0
#define NUM_BUTTONWNDEXTRABYTES (1 * sizeof( WORD ) )
#define DEFSIZEX  100
#define DEFSIZEY  50

//
// button size defines
//

#define NUMBUTS   ((IDB_END - IDB_START) +1)
#define NUMBUTROWS   3
#define NUMBUTCOLS   (NUMBUTS / NUMBUTROWS)
#define EXTRAX    1
#define EXTRAY    1
#define HEIGHTEXTRA 6

#define DEFAULT_BTNHEIGHT  14    // this is incase a GetDC fails.

//
// Macros for getting the x and y "coordinates" of a given button
//
#define BTNX(wID) ((wID - IDB_START) % NUMBUTCOLS)
#define BTNY(wID) ((wID - IDB_START) / NUMBUTCOLS)

//
// the blue column of buttons (KWIC, Full, Cite) is #5
//
#define BLUECOLUMN 5 

#define FONTHEIGHT 8
#define FONTWEIGHT 400
#define FONTNAME   "Helv"

// This is the brush that is used to paint the background of
// the buttonw indow.
#define BUTTONWND_BACKGROUND GRAY_BRUSH

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
LONG FAR PASCAL
fnButtonWnd( HWND hWnd, WORD iMessge, WORD wParam, LONG lParam ) ;

//
// Subclass function for bmpbtns...
//
LONG FAR PASCAL
fnBmpBtn( HWND hwndBmpBtn, WORD iMessage, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL Create( HWND hWnd ) ;
BOOL NEAR PASCAL Size( HWND hWnd, LONG lParam ) ;
BOOL NEAR PASCAL Paint( HWND hWnd ) ;
BOOL NEAR PASCAL SetColors( HWND hwnd, int x, int y ) ;
BOOL NEAR PASCAL KeyDown( WORD wParam ) ;
BOOL NEAR PASCAL SetBtnText( WORD wID, WORD wTextID ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

//
// Keep handles to the buttons in an array
//
HWND     rghwndBtn[NUMBUTCOLS][NUMBUTROWS] ;

#ifndef NOSUBCLASS
FARPROC  rglpfnOldBmpBtn[NUMBUTCOLS][NUMBUTROWS] ;

FARPROC  lpfnBmpBtn ;
#endif

WORD     wButtonHeight ;
HFONT    hFont ;

/************************************************************************
 * External/Exported Functions
 ************************************************************************/
/*************************************************************************
 *  HWND FAR PASCAL CreateButtonWnd( HWND hwndParent ) ;
 *
 *  Description:
 *    This function creates the button window.
 *    The button window is responsible for creating all of it's buttons
 *    (which are "bmpbtns").
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hwndParent
 *          The parent window.
 * 
 *  Return Value
 *    Returns the handle to the button window, 0 if it fails to
 *    create the window.
 *
 *  Comments:
 *
 *************************************************************************/
HWND FAR PASCAL CreateButtonWnd( HWND hwndParent ) 
{
   HWND  hwndBtnWnd ;
   LOGFONT lf ;
   TEXTMETRIC tm ;
   HDC hDC ;


   FontInitLogFont( &lf ) ;

   if (hDC = GetDC( hwndParent ))
   {
      SelectObject( hDC,  GetStockObject( SYSTEM_FIXED_FONT ) ) ;
      GetTextMetrics( hDC, &tm ) ;

      lstrcpy( lf.lfFaceName, FONTNAME ) ;

      lf.lfHeight = tm.tmHeight ;

      lf.lfWeight = FONTWEIGHT ;
      if (!FontSelectIndirect( hDC, &lf ))
      {
         DP1( hWDB,"FontSelectIndirect failed!!") ;
      }
      ReleaseDC( hwndParent, hDC ) ;
   }

   FontSelectIndirect( NULL, &lf ) ;

   hFont = CreateFontIndirect( &lf ) ;

   if (!(hwndBtnWnd = CreateWindow(
                                    "buttonwnd",
                                    "",
                                    WS_CHILD,
                                    0,
                                    0,
                                    DEFSIZEX,
                                    DEFSIZEY,
                                    hwndParent,
                                    0,
                                    GETHINST( hwndParent ),
                                    NULL )))
   {
      DP1( hWDB, "ButtonWnd create failed!" ) ;
      return 0 ;
   }

   return hwndBtnWnd ;

}/* CreateButtonWnd() */


/*************************************************************************
 *  BOOL FAR PASCAL DestroyButtonWnd( HWND hwndBtnWnd ) ;
 *
 *  Description:
 *    This function destroys the top status window.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hwndBtnWnd
 *          Handle to the status window obtained from CreateButtonWnd...
 * 
 *  Return Value
 *    TRUE if it succeeded, FALSE otherwise.
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DestroyButtonWnd( HWND hwndBtnWnd ) 
{
   if (hwndBtnWnd)
   {
      BOOL b ;

      if (hFont)
         DeleteObject( hFont ) ;

      b = DestroyWindow( hwndBtnWnd ) ;
      return b ;
   }
   else
      return FALSE ;

}/* DestroyButtonWnd() */

/*************************************************************************
 *  BOOL FAR PASCAL InitButtonWnd( HANDLE hInst )
 *
 *  Description:
 *    This function initializes the buttonwin window class used by
 *    the button window.
 *
 *  Type/Parameter
 *          Description
 *    HINST    hInst
 *    Applciation's instance.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL InitButtonWnd( HANDLE hInst )
{
   WNDCLASS    wc ;
   //
   // Register the window class
   //
   wc.style         = CS_VREDRAW | CS_HREDRAW ;

   wc.lpfnWndProc   = fnButtonWnd;  
                                  
   wc.cbClsExtra    = 0 ;             
   wc.cbWndExtra    = NUM_BUTTONWNDEXTRABYTES ;             
   wc.hInstance     = hInst ;      
   wc.hIcon         = NULL ;
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = GetStockObject( BUTTONWND_BACKGROUND ) ; 
   wc.lpszMenuName  = NULL ;
   wc.lpszClassName = "buttonwnd" ;

   /* Register the window class and return success/failure code. */

   return RegisterClass( &wc ) ;

}/* InitButtonWnd() */

/*************************************************************************
 *  LONG FAR PASCAL
 *    fnButtonWnd( HWND hWnd, WORD iMessge, WORD wParam, LONG lParam ) ;
 *
 *  Description:
 *    Window proc for the button widnow
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnButtonWnd( HWND hWnd, WORD iMessage, WORD wParam, LONG lParam ) 
{

   switch( iMessage )
   {

      case WM_CREATE:
         Create( hWnd ) ;
      break ;

      case WM_COMMAND:
         if (wParam >= IDB_START && wParam <= IDB_END)
         {
            PostMessage( GetParent( hWnd ), WM_COMMAND, wParam, 0L ) ;
            return 0L ;
         }
      break ;

      case WM_SETFOCUS:
         if (rghwndBtn[0][0])
            SetFocus( rghwndBtn[0][0] ) ;
      break ;

//      case WM_KEYDOWN:
//         KeyDown( wParam ) ;
//         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
//      break ;

      case BU_GETHEIGHT:
         return ((wButtonHeight + EXTRAY) * NUMBUTROWS) + (2*EXTRAY) ;
      break ;

      case WM_MOVE:
         InvalidateRect( hWnd, NULL, TRUE ) ;
      break ;

      case WM_SIZE:
         Size( hWnd, lParam ) ;
      break ;

      case WM_PAINT:
         Paint( hWnd ) ;
      break ;

      case WM_CTLCOLOR:
         return GetStockObject( BUTTONWND_BACKGROUND ) ;
         break ;

      case WM_DESTROY:
      {
         int x, y ;

         for (x = 0 ; x < NUMBUTCOLS ; x++)
            for (y = 0 ; y <NUMBUTROWS ; y++)
               if (rghwndBtn[x][y])
               {
                  DestroyWindow( rghwndBtn[x][y] ) ;
                  rghwndBtn[x][y] = NULL ;
               }

         #ifndef NOSUBCLASS
         if (lpfnBmpBtn)
            FreeProcInstance( lpfnBmpBtn ) ;
         #endif
      }
      break ;

      default:
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
   }
   return 0L ;
}/* fnButtonWnd() */

/*************************************************************************
 *  BOOL FAR PASCAL SetButtonPersonality( HWND hWnd, WORD wPer )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL SetButtonPersonality( HWND hWnd, WORD wPer )
{
   switch( wPer )
   {
      case LEGAL:
      case COMBINED:
         SetBtnText( IDB_QUOTES, IDB_QUOTES );
         SetBtnText( IDB_FOCUS, IDB_FOCUS ) ;
         SetBtnText( IDB_LEXDOC, IDB_LEXDOC ) ;
         SetBtnText( IDB_LEXSTAT, IDB_LEXSTAT ) ;
         SetBtnText( IDB_LEXSEE, IDB_LEXSEE ) ;
      break ;

      case CORP:
         SetBtnText( IDB_QUOTES, IDB_SAVE );
         SetBtnText( IDB_FOCUS, IDB_MODIFY ) ;
         SetBtnText( IDB_LEXDOC, IDB_SEGMENTS ) ;
         SetBtnText( IDB_LEXSTAT, IDB_RECALL ) ;
         SetBtnText( IDB_LEXSEE, IDB_QUOTES ) ;
      break ;
   }

   return TRUE ;
}/* SetButtonPersonality() */

/************************************************************************
 * Internal functions
 ************************************************************************/
/*************************************************************************
 *  BOOL NEAR PASCAL Create( HWND hWnd )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL Create( HWND hWnd )
{
   WORD        wID ;
   WORD        wButtonWidth ;
   char        szBuf[64] ;
   TEXTMETRIC  tm ;
   HDC         hDC ;
   WORD        xPos, yPos ;

   if (hDC = GetDC( hWnd ))
   {
      GetTextMetrics( hDC, &tm ) ;
      ReleaseDC( hWnd, hDC ) ;
      wButtonHeight = tm.tmHeight + tm.tmExternalLeading + HEIGHTEXTRA ;
      //
      // This is really a bogus value, this thing's going to be re-calculated
      // at the first paint message anyway...
      //
      wButtonWidth  = tm.tmAveCharWidth * 10 ;
   }
   else
   {
      wButtonHeight = DEFAULT_BTNHEIGHT ;
      wButtonWidth = 10 ;
   }

   #ifndef NOSUBCLASS
   lpfnBmpBtn = MakeProcInstance( (FARPROC)fnBmpBtn,
                                  GETHINST( GetParent( hWnd ) ) ) ;
   #endif

   for ( wID = IDB_START ; wID <= IDB_END ; wID++)
   {
      int x, y ;

      x = BTNX( wID ) ;
      y = BTNY( wID ) ;

      xPos = (x * wButtonWidth) + EXTRAX ;
      yPos = (y * wButtonHeight) + EXTRAY ;

      //
      // BmpBtn's take a window text of "#<number>" to specify
      // the string in the string table with id of <number>
      //
      wsprintf( szBuf, "#%d", wID ) ;


      if (x == 0 && y == 0)
         rghwndBtn[x][y] = CreateWindow(  BTNCLASSNAME,
                                             szBuf,
                                             WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE,
                                             xPos, yPos,
                                             wButtonWidth, wButtonHeight,
                                             hWnd,
                                             wID,
                                             GETHINST( GetParent( hWnd ) ),
                                             NULL ) ;
      else
         rghwndBtn[x][y] = CreateWindow(  BTNCLASSNAME,
                                             szBuf,
                                             WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                             xPos, yPos,
                                             wButtonWidth, wButtonHeight,
                                             hWnd,
                                             wID,
                                             GETHINST( GetParent( hWnd ) ),
                                             NULL ) ;

      if (rghwndBtn[x][y])
      {
         SendMessage( rghwndBtn[x][y], WM_SETFONT, hFont, FALSE ) ;

         SetColors( rghwndBtn[x][y], x, y ) ;

         #ifndef NOSUBCLASS
         //
         // Now Subclass the sucker
         //
         if (lpfnBmpBtn)
         {   
            rglpfnOldBmpBtn[x][y] = (FARPROC)GetWindowLong( rghwndBtn[x][y],
                                                      GWL_WNDPROC ) ;
            SetWindowLong( rghwndBtn[x][y], GWL_WNDPROC, (LONG)lpfnBmpBtn ) ;
         }
         #endif
      }
   }


   return TRUE ;
}/* Create() */

/*************************************************************************
 *  BOOL NEAR PASCAL Size( HWND hWnd, LONG lParam )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL Size( HWND hWnd, LONG lParam )
{
   WORD        wID ;
   WORD        xPos, yPos ;
   WORD        x, y ;
   WORD        wButtonWidth ;
   WORD        wExtra ;

   wExtra = LOWORD(lParam - (2*EXTRAX)) % NUMBUTCOLS ;

   wButtonWidth  = ((LOWORD( lParam ) - (2*EXTRAX)) / NUMBUTCOLS) - EXTRAX ;

   for ( wID = IDB_START ; wID <= IDB_END ; wID++)
   {
      x = BTNX( wID ) ;
      y = BTNY( wID ) ;

      xPos = (x * (wButtonWidth + EXTRAX ) ) + EXTRAX + (wExtra / 2);
      yPos = (y * (wButtonHeight + EXTRAY) ) + (2*EXTRAY) ;

      if (rghwndBtn[x][y])
         MoveWindow( rghwndBtn[x][y], xPos, yPos,
                     wButtonWidth, wButtonHeight, TRUE ) ;
   }

   return TRUE ;

}/* Size() */


/*************************************************************************
 *  BOOL NEAR PASCAL Paint( HWND hWnd )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL Paint( HWND hWnd )
{
   PAINTSTRUCT ps ;
   RECT        rc ;

   if (BeginPaint( hWnd, &ps ))
   {
      GetClientRect( hWnd, &rc ) ;

      //
      // Draw the border
      // 
      SelectObject( ps.hdc, GetStockObject( BLACK_PEN )) ;
      MoveTo( ps.hdc, rc.left, rc.top ) ;
      LineTo( ps.hdc, rc.right, rc.top ) ;

      EndPaint( hWnd, &ps ) ;
   }

   return TRUE ;

}/* Paint() */


/*************************************************************************
 *  BOOL NEAR PASCAL SetColors( HWND hwnd )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL SetColors( HWND hwnd, int x, int y )
{
   LPLEXISSTATE lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals ) ;

   if (!lpLS)
      return FALSE ;

   if (lpLS->wNumColors > 8)
   {
      SendMessage( hwnd, BB_SETTEXTCOLOR, 0, GetSysColor(COLOR_BTNTEXT) ) ;
      if (x==0 && y == 0)
      {
         SendMessage( hwnd, BB_SETTEXTCOLOR, 0, (LONG)RGBWHITE ) ;
         SendMessage( hwnd, BB_SETFACECOLOR, 0, RGBGREEN ) ;
         SendMessage( hwnd, BB_SETSHADOWCOLOR, 0, GetSysColor(COLOR_BTNSHADOW )) ;
         GlobalUnlock( ghGlobals ) ;
         return TRUE ;
      }

      if (x < BLUECOLUMN)
      {
         SendMessage( hwnd, BB_SETTEXTCOLOR, 0, GetSysColor(COLOR_BTNTEXT) ) ;
         SendMessage( hwnd, BB_SETFACECOLOR, 0, GetSysColor(COLOR_BTNFACE) ) ;
         SendMessage( hwnd, BB_SETSHADOWCOLOR, 0, GetSysColor(COLOR_BTNSHADOW) ) ;
      }

      if (x == BLUECOLUMN)
      {
         SendMessage( hwnd, BB_SETTEXTCOLOR, 0, (LONG)RGBWHITE ) ;
         SendMessage( hwnd, BB_SETFACECOLOR, 0, (LONG) RGBLTBLUE ) ;
         SendMessage( hwnd, BB_SETSHADOWCOLOR, 0, (LONG) RGBBLUE ) ;
      }
      if (x > BLUECOLUMN)
      {
         SendMessage( hwnd, BB_SETTEXTCOLOR, 0, (LONG)RGBBLACK ) ;
         SendMessage( hwnd, BB_SETFACECOLOR, 0, (LONG)RGBYELLOW ) ;
         SendMessage( hwnd, BB_SETSHADOWCOLOR, 0, (LONG)RGBBROWN ) ;
      }
   }

   GlobalUnlock( ghGlobals ) ;
   return TRUE ;
}/* SetColors() */

/*************************************************************************
 *  BOOL NEAR PASCAL SetBtnText( WORD wID, WORD wTextID )
 *
 *  Description: 
 *
 * Sets the button identified by wID to the text string #wTextID, which in
 * turn loads the appropriate string for the button from the resource
 * file...
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL SetBtnText( WORD wID, WORD wTextID )
{
   char szText[MAX_STRLEN] ;

   if (rghwndBtn[BTNX( wID )][BTNY( wID )])
   {
      wsprintf( szText, "#%d", wTextID ) ;
      SetWindowText( rghwndBtn[BTNX( wID )][BTNY( wID )], szText ) ;
   }
   else
      return FALSE ;

   return TRUE ;
}/* SetBtnText() */


#ifndef NOSUBCLASS
/*************************************************************************
 *  BOOL NEAR PASCAL KeyDown( wParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL KeyDown( WORD wParam )
{
   WORD wID = GetWindowWord( GetFocus(), GWW_ID ) ;
   int  x, y ;

   
   if (wID)
   {
      x = BTNX( wID ) ;
      y = BTNY( wID ) ;
   }
   else
   {
      x = y = 0 ;
      if (rghwndBtn[x][y])
         SetFocus( rghwndBtn[x][y] ) ;
      return TRUE ;
   }

   switch (wParam)
   {
      case VK_RETURN:
      {
         HWND hwndTemp = GetFocus() ;

         if (!rghwndBtn[0][0])
            return FALSE ;

         if (hwndTemp == rghwndBtn[0][0])
            return TRUE ;
         else
         {

            SetFocus( rghwndBtn[0][0] ) ;
            SendMessage( rghwndBtn[0][0], WM_KEYDOWN, VK_RETURN, 0L ) ;
            SendMessage( rghwndBtn[0][0], WM_KEYUP, VK_RETURN, 0L ) ;

            SetFocus( hwndTemp ) ;
         }
         return TRUE ;
      }
      break ;

      case VK_HOME:
         x = y = 0 ;
         break ;

      case VK_END:
         x = NUMBUTCOLS - 1 ;
         y = NUMBUTROWS - 1 ;
         break ;

      case VK_UP:
         if (y == 0)
            y = NUMBUTROWS - 1 ;
         else
            y-- ;
      break ;

      case VK_DOWN:
         if (y == NUMBUTROWS - 1)
            y = 0 ;
         else
            y++ ;
      break ;

      case VK_PRIOR:
         y = 0 ;
         if (x == 0)
            x = NUMBUTCOLS - 1 ;
         else
            x-- ;
      break ;

      case VK_NEXT:
         y = NUMBUTROWS - 1 ;
         if (x == NUMBUTCOLS - 1)
            x = 0 ;
         else
            x++ ;
      break ;

      case VK_LEFT:
         if (x == 0)
            x = NUMBUTCOLS - 1 ;
         else
            x-- ;
      break ;

      case VK_RIGHT:
         if (x == NUMBUTCOLS - 1)
            x = 0 ;
         else
            x++ ;
      break ;

      case VK_TAB:
         if (GetKeyState( VK_SHIFT ) < 0)
         {
            if (y == 0 && x == 0)
            {
               SetFocus( GetParent( GetParent( GetFocus() ) ) )  ;
               return TRUE ;
            }

            if (y == 0)
            {
               y = NUMBUTROWS - 1 ;
               
               if (x == 0)
                  x = NUMBUTCOLS - 1 ;
               else
                  x-- ;
            }
            else
                y-- ;
         }
         else           // shift key is NOT down
         {
            if (y == NUMBUTROWS-1 && x == NUMBUTCOLS-1)
            {
               SetFocus( GetParent( GetParent( GetFocus() ) ) )  ;
               return TRUE ;
            }

            if (y == NUMBUTROWS-1)
            {
               y = 0 ;
               
               if (x == NUMBUTCOLS-1)
                  x = 0 ;
               else
                  x++ ;
            }
            else
                y++ ;
         }
      break ;
   }

   if (rghwndBtn[x][y])
      SetFocus( rghwndBtn[x][y] ) ;

   return TRUE ;
}/* KeyDown() */

/*************************************************************************
 *  LONG FAR PASCAL fnBmpBtn( HWND hwndBmpBtn, WORD iMessage, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL fnBmpBtn( HWND hwndBmpBtn, WORD iMessage, WORD wParam, LONG lParam )
{
   WORD wID = GetWindowWord( hwndBmpBtn, GWW_ID ) ;
   int x, y ;
   HWND     hwndMain = GetParent( GetParent( hwndBmpBtn ) ) ;
   HWND     hwndBotStat = GETHWNDBOTSTAT( hwndMain ) ;

   switch (iMessage)
   {
      case WM_KEYDOWN :
      {
         KeyDown( wParam ) ;
         if (hwndBotStat)
            SendMessage( hwndBotStat, iMessage, wParam, lParam ) ;
      }
      break ;

      case WM_SETFOCUS:
         if (HeyTerminalDoYouWantFocus( GETHWNDTERM( hwndMain ) ))
            SetFocus( hwndMain ) ;
      break ;

   }

   x = BTNX( wID ) ;
   y = BTNY( wID ) ;

   if (rglpfnOldBmpBtn[x][y])
      return CallWindowProc( rglpfnOldBmpBtn[x][y], hwndBmpBtn,
                             iMessage, wParam, lParam ) ;
   else
      return 0L ;

}/* fnBmpBtn() */

#else

LONG FAR PASCAL fnBmpBtn( HWND hwndBmpBtn, WORD iMessage, WORD wParam, LONG lParam )
{
   return 0L ;
}
#endif
/************************************************************************
 * End of File: button.c
 ************************************************************************/

