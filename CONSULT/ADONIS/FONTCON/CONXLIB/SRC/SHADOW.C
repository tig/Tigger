/************************************************************************
 *
 *     Project:  3DUTIL.DLL
 *
 *      Module:  shadow.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *     00.00.000  3/26/91 cek   First Version.
 *     00.00.001  8/19/91 cek   3DUTIL
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "CONXLIBi.h"

extern BOOL bMonochrome ;

#define SHADOWWIDTH  1

#define WM_MYPAINT      (WM_USER + 0x700)


/** VOID WINAPI tdDraw3DRect( HDC, LPRECT, UINT, UINT )
 *
 *  DESCRIPTION: 
 *      Draws a 3D rectangle that is shaded.  wFlags can be used to
 *      control how the rectangle looks.
 *
 *  ARGUMENTS:
 *      HDC hDC             :   Handle to the device context that will be
 *                              used to display the rectangle.
 *
 *      RECT rect           :   A rectangle describing the dimensions of
 *                              the rectangle in device coordinates.
 *
 *      UINT wShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      UINT wFlags         :   The following flags may be passed to describe
 *                              the style of the rectangle:
 *
 *                              DRAW3D_IN   :   The shadow is drawn such that
 *                              the box appears to be sunk in to the screen.
 *                              This is default if 0 is passed.
 *
 *                              DRAW3D_OUT  :   The shadow is drawn such that
 *                              the box appears to be sticking out of the
 *                              screen.
 *
 *  RETURN (VOID WINAPI):
 *      The 3D looking rectangle will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

VOID WINAPI tdDraw3DRect( HDC hDC, LPRECT lpRect,
                               UINT wShadowWidth, UINT wFlags )
{
   /* sanity check--don't work if you don't have to! */
   if ( !wShadowWidth || !RectVisible( hDC, lpRect ) )
       return ;

   /* draw the top line */
   tdDraw3DLine( hDC, lpRect->left, lpRect->top,
                   lpRect->right - lpRect->left,
                   wShadowWidth, DRAW3D_TOPLINE | wFlags ) ;

   /* right line */
   tdDraw3DLine( hDC, lpRect->right, lpRect->top,
                   lpRect->bottom - lpRect->top,
                   wShadowWidth, DRAW3D_RIGHTLINE | wFlags ) ;

   /* bottom line */
   tdDraw3DLine( hDC, lpRect->left, lpRect->bottom,
                   lpRect->right - lpRect->left,
                   wShadowWidth, DRAW3D_BOTTOMLINE | wFlags ) ;

   /* left line */
   tdDraw3DLine( hDC, lpRect->left, lpRect->top,
                   lpRect->bottom - lpRect->top,
                   wShadowWidth, DRAW3D_LEFTLINE | wFlags ) ;
} /* Draw3DRect() */


/** VOID WINAPI tdDraw3DLine( HDC hDC, UINT x, UINT y, UINT nLen,
 *
 *  DESCRIPTION: 
 *      Draws a 3D line that can be used to make a 3D box.
 *
 *  ARGUMENTS:
 *      HDC hDC             :   Handle to the device context that will be
 *                              used to display the 3D line.
 *
 *      UINT x, y           :   Coordinates of the beginning of the line.
 *                              These coordinates are in device UINTs and
 *                              represent the _outside_ most point. Horiz-
 *                              ontal lines are drawn from left to right and
 *                              vertical lines are drawn from top to bottom.
 *
 *      UINT wShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      UINT  wFlags        :   The following flags may be passed to
 *                              describe the style of the 3D line:
 *
 *                              DRAW3D_IN   :   The shadow is drawn such that
 *                              the box appears to be sunk in to the screen.
 *                              This is default if 0 is passed.
 *
 *                              DRAW3D_OUT  :   The shadow is drawn such that
 *                              the box appears to be sticking out of the
 *                              screen.
 *
 *                              DRAW3D_TOPLINE, _BOTTOMLINE, _LEFTLINE, and
 *                              _RIGHTLINE  :   Specifies that a "top",
 *                              "Bottom", "Left", or"Right" line is to be
 *                              drawn.
 *
 *  RETURN (VOID WINAPI):
 *      The line will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

VOID WINAPI tdDraw3DLine( HDC hDC, UINT x, UINT y, UINT nLen,
                               UINT wShadowWidth, UINT wFlags ) 
{
   HBRUSH  hOldBrush ;
   HBRUSH  hBrush ;
   HPEN    hOldPen ;
   BOOL    fDark ;
   POINT   Point[ 4 ] ;         /* define a polgon with 4 points    */

   /* if width is zero, don't do nothin'! */
   if ( !wShadowWidth )
       return ;

   /* if width is 1 use lines instead of polygons */
   if (wShadowWidth == 1 || bMonochrome)
   {
     tdDraw3DLine1( hDC, x, y, nLen, wFlags ) ;
      return ;
   }

   /* define shape of polygon--origin is always the same */
   Point[0].x = x ;
   Point[0].y = y ;

   /*  To do this we'll simply draw a polygon with four sides, using 
    *  the appropriate brush.  I dare you to ask me why this isn't a
    *  switch/case!
    */
   if ( wFlags & DRAW3D_TOPLINE )
   {
       /* across to right */
       Point[1].x = x + nLen - (wShadowWidth == 1 ? 1 : 0) ;
       Point[1].y = y ;

       /* down/left */
       Point[2].x = x + nLen - wShadowWidth ;
       Point[2].y = y + wShadowWidth ;

       /* accross to left */
       Point[3].x = x + wShadowWidth ;
       Point[3].y = y + wShadowWidth ;

       /* select 'dark' brush if 'in'--'light' for 'out' */
       fDark = ( wFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }

   /* possibly the bottom? */
   else if ( wFlags & DRAW3D_BOTTOMLINE )
   {
       /* across to right */
       Point[1].x = x + nLen ;
       Point[1].y = y ;

       /* up/left */
       Point[2].x = x + nLen - wShadowWidth ;
       Point[2].y = y - wShadowWidth ;

       /* accross to left */
       Point[3].x = x + wShadowWidth ;
       Point[3].y = y - wShadowWidth ;

       /* select 'light' brush if 'in' */
       fDark = ( wFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* ok, it's gotta be left? */
   else if ( wFlags & DRAW3D_LEFTLINE )
   {
       /* down */
       Point[1].x = x ;
       Point[1].y = y + nLen - (wShadowWidth == 1 ? 1 : 0) ;

       /* up/right */
       Point[2].x = x + wShadowWidth ;
       Point[2].y = y + nLen - wShadowWidth ;

       /* down */
       Point[3].x = x + wShadowWidth ;
       Point[3].y = y + wShadowWidth ;

       /* select 'dark' brush if 'in'--'light' for 'out' */
       fDark = ( wFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }
   
   /* well maybe it's for the right side? */
   else if ( wFlags & DRAW3D_RIGHTLINE )
   {
       /* down */
       Point[1].x = x ;
       Point[1].y = y + nLen ;

       /* up/left */
       Point[2].x = x - wShadowWidth ;
       Point[2].y = y + nLen - wShadowWidth ;

       /* up */
       Point[3].x = x - wShadowWidth ;
       Point[3].y = y + wShadowWidth ;

       /* select 'light' brush if 'in' */
       fDark = ( wFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* bad drugs? */
   else return ;

   /* select NULL_PEN for no borders */
   hOldPen = SelectObject( hDC, GetStockObject( NULL_PEN ) ) ;

   /* select the appropriate color for the fill */
   if ( fDark )
       hBrush = CreateSolidBrush( GetNearestColor( hDC,
                                  tdGetShadowColor( GetBkColor( hDC ) ) ) ) ;
   else
       hBrush = CreateSolidBrush( GetNearestColor( hDC,
                                  tdGetHighlightColor( GetBkColor( hDC ) ) ) ) ;

   hOldBrush = SelectObject( hDC, hBrush ) ;

   /* finally, draw the dern thing */
   Polygon( hDC, (LPPOINT)&Point, 4 ) ;

   /* restore what we killed */
   SelectObject( hDC, hOldBrush ) ;

   DeleteObject( hBrush ) ;

   SelectObject( hDC, hOldPen ) ;
} /*tdDraw3DLine() */

/****************************************************************
 *  VOID WINAPI
 *   tdDraw3DLine1( HDC hDC, UINT x, UINT y, UINT nLen, UINT wFlags )
 *
 *  Description: 
 *
 *    Does the same thing astdDraw3DLine but for single width lines.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI
   tdDraw3DLine1( HDC hDC, UINT x, UINT y, UINT nLen, UINT wFlags )
{
   BOOL    fDark ;
   RECT    rc ;
   COLORREF   rgbOld ;

   if ( wFlags & DRAW3D_TOPLINE )
   {
      rc.left = x ;
      rc.right = rc.left + nLen - 1 ;
      rc.top = y ;
      rc.bottom = y + 1 ;

      fDark = ( wFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }

   /* possibly the bottom? */
   else if ( wFlags & DRAW3D_BOTTOMLINE )
   {
      rc.left = x ;
      rc.right = rc.left + nLen - 1 ;
      rc.top = y - 1 ;
      rc.bottom = y ;

      fDark = ( wFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* ok, it's gotta be left? */
   else if ( wFlags & DRAW3D_LEFTLINE )
   {
      rc.left = x ;
      rc.right = x + 1 ;
      rc.top = y ;
      rc.bottom = y + nLen ;

      fDark = ( wFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }
   
   /* well maybe it's for the right side? */
   else if ( wFlags & DRAW3D_RIGHTLINE )
   {
      rc.left = x - 1 ;
      rc.right = x ;
      rc.top = y ;
      rc.bottom = y + nLen ;

      fDark = ( wFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* bad drugs? */
   else return ;

   /* select the appropriate color for the fill */
   if ( fDark || bMonochrome )
      rgbOld = SetBkColor( hDC,
               GetNearestColor( hDC, tdGetShadowColor( GetBkColor( hDC ) ) ) ) ;
   else
      rgbOld = SetBkColor( hDC,
               GetNearestColor( hDC, tdGetHighlightColor( GetBkColor( hDC ) ) ) ) ;

   /* finally, draw the dern thing */
   
   ExtTextOut( hDC, x, y, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

   SetBkColor( hDC, rgbOld ) ;

} /* tdDraw3DLine1()  */

/****************************************************************
 *  VOID WINAPI tdGetChildRect( HWND hwndChild, LPRECT lprc )
 *
 *  Description: 
 *
 *    Gets the rectangle of a child window in parent client coords.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI tdGetChildRect( HWND hwndChild, LPRECT lprc )
{
   GetWindowRect( hwndChild, lprc ) ;
   ScreenToClient( GetParent( hwndChild ), (LPPOINT)&lprc->left ) ;
   ScreenToClient( GetParent( hwndChild ), (LPPOINT)&lprc->right ) ;

} /* GetChildRect()  */

/****************************************************************
 *  BOOL WINAPI
 *    tdDrawDropShadow( HDC hDC, HWND hWnd, short ShadowWidth )
 *
 *  Description: 
 *
 *  This function draws shadows along the bottom and right sides
 *  of a window.
 *
 *  Type/Parameter
 *          Description
 *  hDC              HDC   Device context previously obtained.
 *  hWnd             HWND  Handle to the window or dialog control
 *                         that is to be shadowed.
 *  ShadowWidth      short The width, in device UINTs, of the shadow.
 *
 *
 *   Example:
 *
 *       // in a window function 
 *          .
 *          .
 *          .
 *
 *       case WM_PAINT:
 *          hDC = BeginPaint (hDlg, &ps) ;                  // Get hDC
 *          GetTextMetrics (hDC, &tm) ;                     // tm is of type TEXTMETRIC
 *          tdDrawDropShadow (hDC,                                
 *                      GetDlgItem (hDlg, IDD_GROUPBOX1),   // Use GetDlgItem to get the handle of the edit
 *                                                          // control "IDD_GROUPBOX1"
 *                      tm.tmAveCharWidth / 2) ;            // 1/2 the system character width is nice looking.
 *          EndPaint (hDlg, &ps) ;
 *          return DefWindowProc (hDlg, iMessage, wParam, lParam) ;  // DO NOT "return 0L"
 *          .
 *          .
 *          .
 *
 *  Comments:
 *
 *    This function should draw a shadow around any window, whether
 *    it is a dialog control or not.
 *
 ****************************************************************/
BOOL WINAPI
   tdDrawDropShadow( HDC hDC, HWND hWnd, short ShadowWidth )
{
   RECT     Rect ;
   POINT    Points[7] ;
   HBRUSH   hbrOld ;
   HBRUSH   hbr ;

   if (!SelectObject (hDC, GetStockObject (NULL_PEN)))
   {
      DP1( hWDB, "DrawDropShadow SelectPen failed" ) ;
      return FALSE ;
   }

   tdGetChildRect( hWnd, &Rect ) ;

   // Draw white on bottom and right
   Points [0].x = Rect.left + ShadowWidth ;
   Points [0].y = Rect.bottom ;

   Points [1].x = Rect.left + ShadowWidth  ;
   Points [1].y = Rect.bottom + ShadowWidth ;

   Points [2].x = Rect.right + ShadowWidth ;
   Points [2].y = Rect.bottom + ShadowWidth ;

   Points [3].x = Rect.right + ShadowWidth ;
   Points [3].y = Rect.top + ShadowWidth ;

   Points [4].x = Rect.right ;
   Points [4].y = Rect.top + ShadowWidth ;

   Points [5].x = Rect.right ;
   Points [5].y = Rect.bottom ;

   Points [6].x = Rect.left ;
   Points [6].y = Rect.bottom ;

   hbr = CreateSolidBrush( RGBGRAY ) ;
   if (!(hbrOld = SelectObject( hDC, hbr ) )) 
   {
      DP1( hWDB, "DrawDropShadow select brush failed, hbr = %d, hdc = %d", hbr, hDC ) ;
      DeleteObject( hbr ) ;
      return FALSE ;
   }

   if (!Polygon (hDC, (LPPOINT) &Points, 7))
   {
      DeleteObject( hbr ) ;
      return FALSE ;
   }

   SelectObject( hDC, hbrOld ) ;
   DeleteObject( hbr ) ;

   return TRUE ;
} /* tdDrawDropShadow()  */

/****************************************************************
 *  COLORREF WINAPI tdGetHighLightColor( COLORREF rgb )
 *
 *  Description: 
 *
 *    This function returns the highlight color that corresponds
 *    to the given rgb value.  If there is no "high intensity"
 *    color that matches, white is used (or yellow if the color
 *    is white).
 *
 *  Comments:
 *
 ****************************************************************/
COLORREF WINAPI tdGetHighlightColor( COLORREF rgb )
{
   BYTE  cRed, cGreen, cBlue ;

   if (rgb == RGBLTRED     ||
       rgb == RGBLTGREEN   ||
       rgb == RGBLTBLUE    ||
       rgb == RGBLTMAGENTA ||
       rgb == RGBLTCYAN    ||
       rgb == RGBLTGRAY    ||
       rgb == RGBYELLOW)
      return RGBWHITE ;

   if (rgb == RGBWHITE)
      return RGBLTGRAY ;

   if (rgb == RGBBLACK || rgb == RGBGRAY)
      return RGBLTGRAY ;

   cRed = (BYTE)(rgb & 0x000000FF) ;

   cGreen = (BYTE)((rgb & 0x0000FF00) >> 8) ;

   cBlue = (BYTE)((rgb & 0x00FF0000) >> 16) ;

   if (cRed == 128)
      cRed += 64 ;

   if (cGreen == 128)
      cGreen += 64 ;

   if (cBlue == 128)
      cBlue += 64 ;

   return RGB( cRed, cGreen, cBlue ) ;

} /* tdGetHighlightColor()  */


/****************************************************************
 *  COLORREF WINAPI tdGetShadowColor( COLORREF rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate shadow color for the given rgb.
 *
 *  Comments:
 *
 ****************************************************************/
COLORREF WINAPI tdGetShadowColor( COLORREF rgb )
{
   BYTE  cRed, cGreen, cBlue ;

   if (rgb == RGBBLACK)
      return RGBGRAY ;

   if (rgb == RGBRED     ||
       rgb == RGBGREEN   ||
       rgb == RGBBLUE    ||
       rgb == RGBBROWN   ||
       rgb == RGBMAGENTA ||
       rgb == RGBCYAN    ||
       rgb == RGBWHITE   ||
       rgb == RGBGRAY)
      return RGBBLACK ;

   if (rgb == RGBLTGRAY)
      return RGBGRAY ;

   cRed = (BYTE)(rgb & 0x000000FF) ;

   cGreen = (BYTE)((rgb & 0x0000FF00) >> 8) ;

   cBlue = (BYTE)((rgb & 0x00FF0000) >> 16) ;

   if (cRed > 128)
      cRed -= 64 ;

   if (cGreen > 128)
      cGreen -= 64 ;

   if (cBlue > 128)
      cBlue -= 64 ;

   return RGB( cRed, cGreen, cBlue ) ;
} /* tdGetShadowColor()  */


/************************************************************************
 * End of File: shadow.c
 ***********************************************************************/

