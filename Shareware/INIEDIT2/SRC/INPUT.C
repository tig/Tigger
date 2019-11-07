/************************************************************************
 *
 *    Copyright (C) 1991, Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  input.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 16 API
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/
#include "iniedit.h"
#include <3dutil.h>
#include "status.h"
#include "input.h"
#include "cmd.h"

LRESULT FAR PASCAL fnInput( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

/*************************************************************************
 *  HWND FAR PASCAL CreateInput( HWND hwndParent )
 *
 *  Description: 
 *
 *    Creates a input.
 * 
 *  Comments:
 *
 *************************************************************************/
HWND FAR PASCAL CreateInput( HWND hwndParent )
{
   HWND hwndTB ;
   WNDCLASS wc ;
   RECT     rc ;

   wc.style             = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC ;
   wc.lpfnWndProc       = fnInput ;
   wc.cbClsExtra        = 0 ;
   wc.cbWndExtra        = 0 ;
   wc.hInstance         = hinstApp ;
   wc.hIcon             = NULL ;
   wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
   (WORD)wc.hbrBackground     = COLOR_BTNFACE + 1 ;
   wc.lpszMenuName      = NULL ;
   wc.lpszClassName     = SZ_INPUTCLASSNAME ;

   RegisterClass (&wc) ;

   GetClientRect( hwndParent, &rc ) ;

   if (hwndTB = CreateWindow(
         SZ_INPUTCLASSNAME,
         "",
         WS_CHILD | WS_BORDER,
         -1, -1, rc.right - rc.left+1, 50,
         hwndParent,
         (HMENU)IDCHILD_INPUT,
         hinstApp,
         NULL ))
   {
      DP4( hWDB, "Input created ok!" ) ;

      SendMessage( hwndTB, WM_SETFONT, (WPARAM)hfontSmall, (LPARAM)TRUE ) ;
   }
   else
   {
      DP1( hWDB, "Could not create Input!" ) ;
   }


   return hwndTB ;

}/* CreateInput() */

/****************************************************************
 *  LRESULT FAR PASCAL
 *       fnInput( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Input window procedure.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT FAR PASCAL
   fnInput( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   switch(wMsg)
   {
      case WM_CREATE:
      break ;

      case WM_DESTROY:
      break ;

      case ST_GETHEIGHT:
         return (LRESULT)40 ;
      break ;

      #if 0
      case WM_PAINT:
      {
         RECT        rc ;
         PAINTSTRUCT ps ;

         BeginPaint( hWnd, &ps ) ;



         EndPaint( hWnd, &ps ) ;
      }
      #endif
      break ;

      case WM_NCCALCSIZE:
      {
         LRESULT dwRet ;
         /* 
          * This is sent when the window manager wants to find out
          * how big our client area is to be.  
          */
         dwRet = DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         InflateRect(((LPRECT)lParam), -1, -1 ) ;
         return dwRet ;
      }
      break ;

      case WM_NCPAINT:
      {
         /*
          * Paint the non-client area here.  We will call DefWindowProc
          * after we are done so it can paint the boarders and so
          * forth...
          */
         RECT  rc ;
         HDC hDC = GetWindowDC( hWnd ) ;

         GetWindowRect( hWnd, &rc ) ;
         rc.right -= rc.left ;
         rc.left = 0 ;
         rc.bottom -= rc.top ;
         rc.top = 0 ;

         if ((LONG)GetWindowLong( hWnd,GWL_STYLE ) & WS_BORDER)
            InflateRect( &rc, -1, -1 ) ;

         SetBkColor( hDC, GetSysColor( COLOR_BTNFACE ) ) ;
         tdDraw3DRect( hDC, &rc, 1, DRAW3D_OUT );

         ReleaseDC( hWnd, hDC ) ;
      }
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   }

   return 0L ;

} /* fnInput()  */


/************************************************************************
 * End of File: input.c
 ************************************************************************/


