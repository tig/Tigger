/************************************************************************
 *
 *    Copyright (C) 1991, Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  toolbar.c
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
#include "toolbar.h"
#include "cmd.h"
#include "toolpal.h"

#define CX_SPACING   3
#define CY_SPACING   3

#define NUM_ITEMS    (sizeof(rgtpItem) / sizeof(rgtpItem[0]))

TOOLPALITEM rgtpItem[] =
{
   0L                                  ,//    dwSize ;          // sizeof (TOOLPALITEM)
   IDM_FILE_COMMIT                     ,//    wID ;
   NULL                                ,//    hModule ;
   (LPSTR)IDM_FILE_COMMIT              ,//    lpszUpBitmap  ;
   (LPSTR)IDM_FILE_COMMIT_DN           ,//    lpszDnBitmap  ;
   0                                   ,//    lpszDisBitmap ;
   0                                   ,//    wFlags ;
   0                                   ,//    cGangLoadItems ;
   ""                                  ,//    rgbReserved[18] ;
      
   0L                                  ,//    dwSize ;          // sizeof (TOOLPALITEM)
   IDM_EDIT_UNDO                       ,//    wID ;
   NULL                                ,//    hModule ;
   (LPSTR)IDM_EDIT_UNDO                ,//    lpszUpBitmap  ;
   (LPSTR)IDM_EDIT_UNDO_DN             ,//    lpszDnBitmap  ;
   0                                   ,//    lpszDisBitmap ;
   0                                   ,//    wFlags ;
   0                                   ,//    cGangLoadItems ;
   ""                                  ,//    rgbReserved[18] ;

   0L                                  ,//    dwSize ;          // sizeof (TOOLPALITEM)
   IDM_FILE_OPEN                       ,//    wID ;
   NULL                                ,//    hModule ;
   (LPSTR)IDM_FILE_OPEN                ,//    lpszUpBitmap  ;
   (LPSTR)IDM_FILE_OPEN_DN             ,//    lpszDnBitmap  ;
   0                                   ,//    lpszDisBitmap ;
   0                                   ,//    wFlags ;
   0                                   ,//    cGangLoadItems ;
   ""                                  ,//    rgbReserved[18] ;

   0L                                  ,//    dwSize ;          // sizeof (TOOLPALITEM)
   IDM_EDIT_ADD_FILE                   ,//    wID ;
   NULL                                ,//    hModule ;
   (LPSTR)IDM_EDIT_ADD_FILE            ,//    lpszUpBitmap  ;
   (LPSTR)IDM_EDIT_ADD_FILE_DN         ,//    lpszDnBitmap  ;
   0                                   ,//    lpszDisBitmap ;
   0                                   ,//    wFlags ;
   0                                   ,//    cGangLoadItems ;
   ""                                  ,//    rgbReserved[18] ;

   0L                                  ,//    dwSize ;          // sizeof (TOOLPALITEM)
   IDM_FILE_EDIT                       ,//    wID ;
   NULL                                ,//    hModule ;
   (LPSTR)IDM_FILE_EDIT                ,//    lpszUpBitmap  ;
   (LPSTR)IDM_FILE_EDIT_DN             ,//    lpszDnBitmap  ;
   0                                   ,//    lpszDisBitmap ;
   0                                   ,//    wFlags ;
   0                                   ,//    cGangLoadItems ;
   ""                                  ,//    rgbReserved[18] ;

   0L                                  ,//    dwSize ;          // sizeof (TOOLPALITEM)
   IDM_FILE_PRINT                      ,//    wID ;
   NULL                                ,//    hModule ;
   (LPSTR)IDM_FILE_PRINT               ,//    lpszUpBitmap  ;
   (LPSTR)IDM_FILE_PRINT_DN            ,//    lpszDnBitmap  ;
   0                                   ,//    lpszDisBitmap ;
   0                                   ,//    wFlags ;
   0                                   ,//    cGangLoadItems ;
   ""                                  ,//    rgbReserved[18] ;

} ;

static HWND hwndTP ;
static WORD wHeight ;
static HBRUSH hbr ;

#define ID_TOOLPAL   0

LRESULT CALLBACK fnToolBar( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

/*************************************************************************
 *  HWND WINAPI CreateToolBar( HWND hwndParent )
 *
 *  Description: 
 *
 *    Creates a toolbar.
 * 
 *  Comments:
 *
 *************************************************************************/
HWND WINAPI CreateToolBar( HWND hwndParent )
{
   HWND hwndTB ;
   RECT     rc ;

   GetClientRect( hwndParent, &rc ) ;

   if (hwndTB = CreateWindow(
         SZ_TOOLBARCLASSNAME,
         "",
         WS_CHILD | WS_BORDER | WS_CLIPCHILDREN,
         -1, -1, rc.right - rc.left+1, 24,
         hwndParent,
         (HMENU)IDCHILD_TOOLBAR,
         hinstApp,
         NULL ))
   {
      DP4( hWDB, "ToolBar created ok!" ) ;
      SendMessage( hwndTB, WM_SETFONT, (WPARAM)hfontSmall, (LPARAM)TRUE ) ;

      if (!ResetToolBar( hwndTB ))
         return NULL ;
   }
   else
   {
      DP1( hWDB, "Could not create ToolBar!" ) ;
   }


   return hwndTB ;

}/* CreateToolBar() */

/****************************************************************
 *  VOID WINAPI ResetToolBar()
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HWND WINAPI ResetToolBar( HWND hWnd )
{
   short i ;
   BOOL     fVisible = IsWindowVisible( hWnd ) ;

   if (!hwndTP)
   {
      rgtpItem[0].cGangLoadItems = NUM_ITEMS ;
      for (i = 0 ; i < NUM_ITEMS ; i++)
      {
         rgtpItem[i].dwSize = sizeof( TOOLPALITEM ) ;
         rgtpItem[i].hModule = hinstApp ;

         if (fToolBarLabels != TRUE)
         {
            (DWORD)rgtpItem[i].lpszUpBitmap += 200 ;
            (DWORD)rgtpItem[i].lpszDnBitmap += 200 ;
            //(DWORD)rgtpItem[i].lpszDisBitmap += 200 ;
         }
      }
   }
   else
   {
      if (fVisible)
         ShowWindow( hWnd, SW_HIDE ) ;

      DestroyWindow( hwndTP ) ;

      hwndTP = NULL ;

      for (i = 0 ; i < NUM_ITEMS ; i++)
      {
         if (fToolBarLabels != TRUE)
         {
            (DWORD)rgtpItem[i].lpszUpBitmap += 200 ;
            (DWORD)rgtpItem[i].lpszDnBitmap += 200 ;
            //(DWORD)rgtpItem[i].lpszDisBitmap += 200 ;
         }
         else
         {
            (DWORD)rgtpItem[i].lpszUpBitmap -= 200 ;
            (DWORD)rgtpItem[i].lpszDnBitmap -= 200 ;
            //(DWORD)rgtpItem[i].lpszDisBitmap -= 200 ;
         }
      }
   }

   DP5( hWDB, "Creating Toolpal..." ) ;
   if (hwndTP = CreateWindow(
         TP_SZCLASSNAME,
         "",
         WS_CHILD | WS_VISIBLE |  TPS_ROUNDLOOK |
         TPS_BORDERS | TPS_TOOLBAR | TPS_ROWORIENT,
         CX_SPACING, CY_SPACING, 0, 0,
         hWnd,
         (HMENU)ID_TOOLPAL,
         hinstApp,
         (rgtpItem[0].cGangLoadItems > 0) ?
         (VOID FAR*)rgtpItem : NULL )) 
   {
      RECT rc ;

      DP4( hWDB, "Toolpal created ok!" ) ;
      SendMessage( hwndTP, TP_SETNOTIFYMSG, (WPARAM)(WMU_TOOLPALNOTIFY), 0L ) ;
      SendMessage( hwndTP, TP_SETNUMCOLS, (WPARAM)1, 0L ) ;
//            SendMessage( hwndTP, TP_SETITEMSIZE, 0,
//               MAKELPARAM( 34, 34 ) ) ;
//            SendMessage( hwndTP, TP_ENABLEITEM, 3, 0L ) ;
      GetWindowRect( hwndTP, &rc ) ;
      wHeight = rc.bottom - rc.top ;

      if (fVisible)
         ShowWindow( hWnd, SW_SHOW ) ;

      return hwndTP ;
   }
   else
   {
      if (fVisible)
         ShowWindow( hWnd, SW_SHOW ) ;

      DP1( hWDB, "Could not create Toolpal!" ) ;
      return NULL ;
   }

} /* ResetToolBar()  */


/****************************************************************
 *  LRESULT CALLBACK
 *       fnToolBar( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    ToolBar window procedure.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT CALLBACK
   fnToolBar( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{

   switch(wMsg)
   {
      case WM_SYSCOLORCHANGE:
         if (hbr)
            DeleteObject( hbr ) ;

         hbr = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );

         if (hwndTP)
            SendMessage( hwndTP, wMsg, wParam, lParam ) ;
      break ;

      case WMU_TOOLPALNOTIFY:
         SendMessage( hwndApp, WM_COMMAND, wParam, 0L ) ;

      break ;

      #if 0
      case WM_SIZE:
      {
         RECT  rc ;
         short n = 1 ;

         GetWindowRect( hwndTP, &rc ) ;
         ShowWindow( hwndTP, SW_HIDE ) ;
         SendMessage( hwndTP, TP_SETNUMCOLS, (WPARAM)n, 0L ) ;
         while (LOWORD( (DWORD)lParam ) < (rc.right - rc.left))
         {
            n++ ;
            SendMessage( hwndTP, TP_SETNUMCOLS, (WPARAM)n, 0L ) ;
            GetWindowRect( hwndTP, &rc ) ;
         }
         ShowWindow( hwndTP, SW_SHOW ) ;

      }
      break ;
      #endif

      case WM_CTLCOLOR:
         if (HIWORD(lParam) == CTLCOLOR_BTN)
         {
            SetBkColor( (HDC)wParam, GetSysColor( COLOR_ACTIVEBORDER ) );

            return MAKELRESULT(hbr,0) ;
         }
      return NULL ;

      case WM_DESTROY:
         if (hbr)
            DeleteObject( hbr ) ;
         hwndTP = NULL ;
      break ;

      case ST_GETHEIGHT:
         return MAKELRESULT( wHeight + CY_SPACING*2 + 4, 0 ) ;
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

} /* fnToolBar()  */


/************************************************************************
 * End of File: toolbar.c
 ************************************************************************/

