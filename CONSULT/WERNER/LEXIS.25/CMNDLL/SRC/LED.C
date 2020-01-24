/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL.DLL, Common DLL for Werner Apps
 *
 *      Module:  led.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 * 
 *     Remarks:
 *
 *
 ************************************************************************/

#define WINDLL
#define NOCOMM
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "..\inc\cmndll.h"

/***************************************************************************
 * Local Defines                                      
 ***************************************************************************/

#define BEVELWIDTH   1
#define DEFAULT_SIZE 15
#define DEF_VERTX    10
#define DEF_VERTY    20
#define DEF_HORZX    20
#define DEF_HORZY    10


#define SQUARE_MULT  2

#define GWW_FLAGS   0
#define GWL_COLOR   2

#define WNDEXTRABYTES 1*sizeof(WORD)+1*sizeof(LONG)

/***************************************************************************
 * Macros                                      
 ***************************************************************************/

#define GETSTYLE(hWnd) LOWORD( GetWindowLong( hWnd, GWL_STYLE ) )
#define GETHINST(hWnd) GetWindowWord( hWnd, GWW_HINSTANCE )
#define GETFLAGS(hWnd) GetWindowWord( hWnd, GWW_FLAGS )
#define GETCOLOR(hWnd)  GetWindowLong( hWnd, GWL_COLOR ) 

#define lpCreate ((LPCREATESTRUCT)lParam)

/***************************************************************************
 *  Local data structures
 ***************************************************************************/

/**************************************************************************
 * Prototypes for local functions
 **************************************************************************/

static BOOL NEAR PASCAL
Create( HWND hWnd, LONG lParam ) ;

//
// Window proc for buttons, THIS FUNCTION MUST BE EXPORTED
//
LONG FAR PASCAL fnLED (HWND, unsigned, WORD, LONG) ;

static BOOL NEAR PASCAL Create( HWND hWnd, LONG lParam ) ;

static BOOL NEAR PASCAL
PaintLED( HWND hWnd, HDC hDC ) ;

static BOOL NEAR PASCAL
DrawRoundBezel( HDC hDC, short x, short y, short nDiameter ) ;

static
BOOL NEAR PASCAL DrawRoundLED( HDC hDC, short x, short y, short nDiameter ) ;

static BOOL NEAR PASCAL
DrawSquareLED( HDC hDC, short x, short y, short x1, short y1) ;

/****************************************************************************
 * Local Variables                                     
 ****************************************************************************/

DWORD    rgbDefaultColor = RGBGRAY ;

/****************************************************************************
 * window class names
 ****************************************************************************/
char szLED[] = "CMNDLLled" ;


/************************************************************************
 *  BOOL FAR PASCAL LocalLEDInit( HANDLE hPrev, HANDLE hInst )
 *
 *  Description:
 *    This function initializes the LED class.  It should be called
 *    from the LibMain or WinMain function.
 *
 *  Type/Parameter
 *          Description
 *    HANDLE   hPrev
 *          instance handle of previous instance
 *
 *    HANDLE   hInst
 *          instance handle of current instance
 * 
 *  Comments:
 *
 ************************************************************************/
BOOL FAR PASCAL
LocalLEDInit( HANDLE hPrev, HANDLE hInst ) 
{
   WNDCLASS    wc ;

   if (!hPrev)
   {
      wc.hCursor        = LoadCursor( NULL,IDC_ARROW ) ;
      wc.hIcon          = NULL ;
      wc.lpszMenuName   = NULL ;
      wc.lpszClassName  = (LPSTR)szLED ;
      wc.hbrBackground  = (HBRUSH)COLOR_WINDOW+1 ;
      wc.hInstance      = hInst ;
      wc.style          = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS ;
      wc.lpfnWndProc    = fnLED ;
      wc.cbClsExtra     = 0 ;
      wc.cbWndExtra     = WNDEXTRABYTES ;
      if (!RegisterClass( &wc ))
         return FALSE ;

   }

   return TRUE ;
}/* LEDInit() */



/*************************************************************************
 *  LONG FAR PASCAL fnLED( HWND hWnd, unsigned iMessage,
 *                            WORD wParam, LONG lParam )
 *
 *  Description:
 *    Window proc for the LED window class
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnLED( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_CREATE:
         Create( hWnd, lParam ) ;
      break ;

      case LED_SETCOLOR:
         InvalidateRect( hWnd, NULL, FALSE ) ;
         return SetWindowLong( hWnd, GWL_COLOR, lParam ) ;
      break ;

      case LED_GETCOLOR:
         return GETCOLOR( hWnd ) ;
      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;

         if (BeginPaint( hWnd, &ps ))
         {
            PaintLED( hWnd, ps.hdc ) ;
            EndPaint( hWnd, &ps ) ;
         }
      }
      return 0L ;

      case WM_DESTROY:
      break ;

   }
   return DefWindowProc(hWnd, wMsg, wParam, lParam) ;

}/* fnLED() */

/*************************************************************************
 *  static BOOL NEAR PASCAL Create( HWND hWnd, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
static BOOL NEAR PASCAL Create( HWND hWnd, LONG lParam )
{
   SetWindowWord( hWnd, GWW_FLAGS, (WORD)lpCreate->style & 0x00FF ) ;

   SetWindowLong( hWnd, GWL_COLOR, rgbDefaultColor ) ;

   SetWindowLong( hWnd, GWL_STYLE, lpCreate->style & 0xFFFFFF00 ) ;

   if (lpCreate->style & LED_ROUND)
      SetWindowPos( hWnd, NULL, 0, 0, DEFAULT_SIZE, DEFAULT_SIZE,
                 SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE ) ;

   return TRUE ;

}/* Create() */


/************************************************************************
 *  static BOOL NEAR PASCAL PaintLED( HWND hWnd, HDC hDC )
 *
 *  Description: 
 *
 *    This function paints the LED.
 *
 *  Comments:
 *
 ************************************************************************/
static BOOL NEAR PASCAL
PaintLED( HWND hWnd, HDC hDC )
{
   RECT     rc ;
   HBRUSH   hbrColor, hbrOld ;
   HPEN     hPen, hOldPen ;
   DWORD    rgbNorm ;
   DWORD    rgbHigh ;

   GetClientRect( hWnd, &rc ) ;

   rgbNorm = GETCOLOR( hWnd ) ;

   hbrColor = CreateSolidBrush( rgbNorm ) ;

   hbrOld = SelectObject( hDC, hbrColor ) ;

   rgbHigh = GetHighlightColor( rgbNorm ) ;

   hPen = CreatePen( PS_SOLID, 1, rgbHigh ) ;

   hOldPen = SelectObject( hDC, hPen ) ;

   if (GETSTYLE( hWnd ) & LED_ROUND)
      DrawRoundLED( hDC, rc.left, rc.top, rc.right - rc.left ) ;
   else
      if (GETSTYLE( hWnd ) & LED_HORZ)
         DrawSquareLED( hDC, rc.left, rc.top,
                             rc.left + DEF_HORZX, rc.top + DEF_HORZY ) ;
      else
         if (GETSTYLE( hWnd ) & LED_VERT)
            DrawSquareLED( hDC, rc.left, rc.top,
                              rc.left + DEF_VERTX, rc.top + DEF_VERTY ) ;
         else
            DrawSquareLED( hDC, rc.left, rc.top, rc.right, rc.bottom ) ;

   SelectObject( hDC, hbrOld ) ;
   DeleteObject( hbrColor ) ;

   SelectObject( hDC, hOldPen ) ;
   DeleteObject( hPen ) ;

   return TRUE ;

}/* PaintLED() */

/****************************************************************
 *  static BOOL NEAR PASCAL DrawRoundLED( HDC, short x, short y, short nDiameter )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL DrawRoundLED( HDC hDC, short x, short y, short nDiameter )
{
   short nShadeX, nShadeY ;
   HPEN     hOldPen ;
   LOGPEN   lp ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Ellipse( hDC, x, y, x + nDiameter, y + nDiameter ) ;

   //
   // Calculate where the shading goes...
   // 
   nShadeX = x + nDiameter / 3 ;
   nShadeY = y + nDiameter / 3 ;

   GetObject( hOldPen, sizeof( LOGPEN ), (LPSTR)&lp ) ;

   SetPixel( hDC, nShadeX - 1 , nShadeY + 1  , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX     , nShadeY + 1  , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX     , nShadeY      , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX + 1 , nShadeY      , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX + 1 , nShadeY - 1  , lp.lopnColor ) ;

   return TRUE ;

} /* DrawRoundLED()  */

/****************************************************************
 *  static BOOL NEAR PASCAL
 *    DrawSquareLED( HDC, short x, short y, short nSizeX, short nSizeY )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL
DrawSquareLED( HDC hDC, short x, short y, short x1, short y1)
{
   HPEN     hOldPen ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Rectangle( hDC, x, y, x1, y1 ) ;

   SelectObject( hDC, hOldPen ) ;

   MoveTo( hDC, x + 2, y1 - 3 ) ;
   LineTo( hDC, x + 2, y + 2 ) ;
   LineTo( hDC, x1 - 2, y + 2 ) ;

   return TRUE ;

} /* DrawSquareLED()  */


/************************************************************************
 * End of File: led.c
 ************************************************************************/

