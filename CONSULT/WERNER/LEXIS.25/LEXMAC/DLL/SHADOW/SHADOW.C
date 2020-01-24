// shadow.c
// 
// Shadow drawing routine for CEKLIB.DLL
//
// Copyright (C) 1990 Charles E. Kindel, Jr.
//

#include <windows.h>

/**************************************************************************
 *
 *  BOOL FARPASCAL
 *  DrawShadow (HDC hDC, HWND hWnd, HBRUSH hBrush, short ShadowWidth)
 *
 *  This function draws shadows along the bottom and right sides
 *  of a window.
 *
 *  Paramter         Type/Description
 *  ---------------------------------------------------------------
 *  hDC              HDC   Device context previously obtained.
 *  hWnd             HWND  Handle to the window or dialog control
 *                         that is to be shadowed.
 *  hBrush           HBRUSH Handle to the brush that is to be used to
 *                          draw the shadow.
 *  ShadowWidth      short The width, in device units, of the shadow.
 *
 *  RETURN VALUE
 *    Returns TRUE if successful, FALSE if not.
 *
 *  EXAMPLE
 *     
 *
 *       // in a window function 
 *          .
 *          .
 *          .
 *
 *       case WM_PAINT:
 *          hDC = BeginPaint (hDlg, &ps) ;                  // Get hDC
 *          GetTextMetrics (hDC, &tm) ;                     // tm is of type TEXTMETRIC
 *          DrawShadow (hDC,                                
 *                      GetDlgItem (hDlg, IDD_GROUPBOX1),   // Use GetDlgItem to get the handle of the edit
 *                                                          // control "IDD_GROUPBOX1"
 *                      GetStockObject (GRAY_BRUSH),        // Use GetStockObject to get a handle to a brush
 *                      tm.tmAveCharWidth / 2) ;            // 1/2 the system character width is nice looking.
 *          EndPaint (hDlg, &ps) ;
 *          return DefWindowProc (hDlg, iMessage, wParam, lParam) ;  // DO NOT "return 0L"
 *          .
 *          .
 *          .
 *
 *  NOTES
 *    This function should draw a shadow around any window, whether
 *    it is a dialog control or not.
 *
 *  AUTHOR
 *    Charles E. Kindel, Jr.
 *
 *  REVSIONS
 *    5/17/90     Initial version  (CEK)
 *
 ****************************************************************************/
BOOL FAR PASCAL
     DrawShadow ( HDC      hDC,           // handle to the Device Context
                  HWND     hWnd,          // handle to the window to be shadowed
                  HBRUSH   hBrush,        // brush to draw shadow with 
                  short    ShadowWidth )  // Width (in pixels) of the shadow
{
   LONG     Origin ;
   RECT     Rect ;
   POINT    Points[7] ;

   GetWindowRect (hWnd, (LPRECT) &Rect) ; 
   Origin = GetDCOrg (hDC) ;

   Rect.left   -= LOWORD(Origin) ;
   Rect.top    -= HIWORD(Origin) ;
   Rect.right  -= LOWORD(Origin) ;
   Rect.bottom -= HIWORD(Origin) ;

   Points [0].x = Rect.left + ShadowWidth ;
   Points [0].y = Rect.bottom ;

   Points [1].x = Rect.left + ShadowWidth ;
   Points [1].y = Rect.bottom + ShadowWidth ;

   Points [2].x = Rect.right + ShadowWidth ;
   Points [2].y = Rect.bottom + ShadowWidth ;

   Points [3].x = Rect.right + ShadowWidth ;
   Points [3].y = Rect.top + ShadowWidth ;

   Points [4].x = Rect.right ;
   Points [4].y = Rect.top + ShadowWidth ;

   Points [5].x = Rect.right ;
   Points [5].y = Rect.bottom ;

   Points [6].x = Rect.left + ShadowWidth ;
   Points [6].y = Rect.bottom ;

   if (!SelectObject (hDC, hBrush))
      return FALSE ;

   if (!SelectObject (hDC, GetStockObject (NULL_PEN)))
      return FALSE ;

   if (!Polygon (hDC, (LPPOINT) &Points, 7))
      return FALSE ;

   return TRUE ;
} /* end of DrawShadow */
