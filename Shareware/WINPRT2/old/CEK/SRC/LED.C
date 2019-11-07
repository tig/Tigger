/************************************************************************
 *      Module:  led.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 * 
 *     Remarks:
 *
 ************************************************************************/

   #include "PRECOMP.H"
   #include "ICEK.h"

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

#define GWL_COLOR   0

#define WNDEXTRABYTES sizeof(LONG)+sizeof(UINT)

/***************************************************************************
 * Macros                                      
 ***************************************************************************/

#define GETSTYLE(hWnd) LOWORD( GetWindowStyle( hWnd ) )
#define GETCOLOR(hWnd)  (COLORREF)GetWindowLong( hWnd, GWL_COLOR ) 
#define SETCOLOR(hWnd, rgb) (COLORREF)SetWindowLong( hWnd, GWL_COLOR, rgb)

/***************************************************************************
 *  Local data structures
 ***************************************************************************/

/**************************************************************************
 * Prototypes for local functions
 **************************************************************************/

static BOOL NEAR PASCAL Create( HWND hWnd, LPARAM lParam ) ;

//
// Window proc for buttons, THIS FUNCTION MUST BE EXPORTED
//
LRESULT CALLBACK fnLED (HWND, UINT, WPARAM, LPARAM ) ;

static BOOL NEAR PASCAL Create( HWND hWnd, LPARAM lParam ) ;

static BOOL NEAR PASCAL PaintLED( HWND hWnd, HDC hDC ) ;

static BOOL NEAR PASCAL
DrawRoundBezel( HDC hDC, int x, int y, int nDiameter ) ;

static
BOOL NEAR PASCAL DrawRoundLED( HDC hDC, int x, int y, int nDiameter ) ;

static BOOL NEAR PASCAL
DrawSquareLED( HDC hDC, int x, int y, int x1, int y1) ;

/****************************************************************************
 * Local Variables                                     
 ****************************************************************************/

COLORREF    rgbDefaultColor = RGBGRAY ;

/****************************************************************************
 * window class names
 ****************************************************************************/

/************************************************************************
 *  BOOL WINAPI LEDInit( HANDLE hPrev, HANDLE hInst, LPSTR lpszClassName )
 *
 *  Description:
 *    This function initializes the LED class.  It should be called
 *    from the LibMain or WinMain function.
 *
 ************************************************************************/
BOOL WINAPI LEDInit( HANDLE hPrev, HANDLE hInst, LPSTR lpsz ) 
{
   WNDCLASS    wc ;

   DP3( hWDB, "LocalLEDInit()..." ) ;

   if (!hPrev)
   {
      wc.hCursor        = LoadCursor( NULL,IDC_ARROW ) ;
      wc.hIcon          = NULL ;
      wc.lpszMenuName   = NULL ;
      if (lpsz)
         wc.lpszClassName = lpsz ;
      else
         wc.lpszClassName  = (LPSTR)SZ_LEDCLASSNAME ;
      wc.hbrBackground  = (HBRUSH)GetStockObject( NULL_BRUSH ) ;
      wc.hInstance      = hInst ;
      wc.style          = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS ;
      wc.lpfnWndProc    = fnLED ;
      wc.cbClsExtra     = 0 ;
      wc.cbWndExtra     = WNDEXTRABYTES ;
      if (!RegisterClass( &wc ))
      {
         DP1( hWDB, "LED Window class failed to register!" ) ;
         return FALSE ;
      }
   }

   return TRUE ;
}/* LEDInit() */



/*************************************************************************
 *  LONG CALLBACK fnLED( HWND hWnd, unsigned iMessage,
 *                            WPARAM wParam, LPARAM lParam )
 *
 *  Description:
 *    Window proc for the LED window class
 *
 *  Comments:
 *
 *************************************************************************/
LRESULT CALLBACK fnLED( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch (wMsg)
   {
      case WM_CREATE:
         return (LRESULT)Create( hWnd, lParam ) ;
      break ;

      case LED_SETCOLOR:
         InvalidateRect( hWnd, NULL, FALSE ) ;
         return (LRESULT)SetWindowLong( hWnd, GWL_COLOR, lParam ) ;
      break ;

      case LED_GETCOLOR:
         return (LRESULT)GETCOLOR( hWnd ) ;
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

   }
   return DefWindowProc(hWnd, wMsg, wParam, lParam) ;

}/* fnLED() */

/*************************************************************************
 *  static BOOL NEAR PASCAL Create( HWND hWnd, LPARAM lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
static BOOL NEAR PASCAL Create( HWND hWnd, LPARAM lParam )
{
   #define lpCreate ((LPCREATESTRUCT)lParam)

   SETCOLOR( hWnd, rgbDefaultColor ) ;

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
static BOOL NEAR PASCAL PaintLED( HWND hWnd, HDC hDC )
{
   RECT     rc ;
   HBRUSH   hbrColor ;
   HPEN     hPen ;
   COLORREF    rgbNorm ;
   COLORREF    rgbHigh ;

   GetClientRect( hWnd, &rc ) ;

   rgbNorm = GETCOLOR( hWnd ) ;

   hbrColor = CreateSolidBrush( rgbNorm ) ;

   hbrColor = SelectObject( hDC, hbrColor ) ;

   rgbHigh = tdGetHighlightColor( rgbNorm ) ;

   hPen = CreatePen( PS_SOLID, 1, rgbHigh ) ;

   hPen = SelectObject( hDC, hPen ) ;

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

   hbrColor = SelectObject( hDC, hbrColor ) ;
   DeleteObject( hbrColor ) ;

   hPen = SelectObject( hDC, hPen ) ;
   DeleteObject( hPen ) ;

   return TRUE ;

}/* PaintLED() */

/****************************************************************
 *  static BOOL NEAR PASCAL DrawRoundLED( HDC, int x, int y, int nDiameter )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL DrawRoundLED( HDC hDC, int x, int y, int nDiameter )
{
   int    nShadeX, nShadeY ;
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

   SelectObject( hDC, hOldPen ) ;

   return TRUE ;

} /* DrawRoundLED()  */

/****************************************************************
 *  static BOOL NEAR PASCAL
 *    DrawSquareLED( HDC, int x, int y, int nSizeX, int nSizeY )
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
DrawSquareLED( HDC hDC, int x, int y, int x1, int y1)
{
   HPEN     hOldPen ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Rectangle( hDC, x, y, x1, y1 ) ;

   SelectObject( hDC, hOldPen ) ;

   MoveToEx( hDC, x + 2, y1 - 3, NULL ) ;
   LineTo( hDC, x + 2, y + 2 ) ;
   LineTo( hDC, x1 - 2, y + 2 ) ;

   return TRUE ;

} /* DrawSquareLED()  */


/************************************************************************
 * End of File: led.c
 ************************************************************************/

