/************************************************************************
 *
 *     Project:  TDUTIL.DLL
 *
 *      Module:  shadow.c
 *
 *     Remarks:
 *
 *       Originally TDUTIL just did 'background' effects.  It did this
 *       by painting on the client area of the dialog box after
 *       the WM_PAINT was processed.  It did the appropriate shadow
 *       effect for each control.
 *
 *       Now it still does 'background' effects for some controls, but
 *       also subclasses buttons to get really cool looks.
 *
 *   Revisions:  
 *     00.00.000  3/26/91 cek   First Version.
 *     00.00.001  8/19/91 cek   TDUTIL
 *     00.00.001  5/19/92 cek   More cool stuff
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "ITDUTIL.h"

extern HMODULE    hInstDLL ;
extern BOOL       fMonochrome ;
extern HBITMAP    hbmBtn ;
extern HBRUSH     hbrBkGnd ;
extern LRESULT    (CALLBACK *lpfnDefButton)(HWND, UINT, WPARAM, LPARAM ) ;
extern UINT       dxBorder;
extern UINT       dyBorder;

#define SHADOWWIDTH  1

#define WM_MYPAINT      (WM_USER + 0x700)

/****************************************************************
 *  BOOL WINAPI
 *  tdShadowControl( HWND hDlg, UINT uiMsg,
 *                 WPARAM wParam, LONG lParam )
 *
 *  Description: 
 *
 *    This function is called kind of like a DefWindowProc for
 *    dialog boxes.  It takes care of all "special" painting.
 *
 *    tdShadowControl lets you use TDUTIL in "auto-pilot" mode for
 *    any given dialog/window.  Just call tdShadowControl on your
 *    WM_INITDIALOG, WM_CTLCOLOR, WM_DESTROY, WM_PAINT, and 'default'
 *    cases in your dialog box.  
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI
   tdShadowControl( HWND hDlg, UINT uiMsg, WPARAM wParam, LONG lParam ) 
{
   static UINT msgTDUTIL ;
   COLORREF    rgbBG ;
   HBRUSH      hbrBG ;

   switch (uiMsg)
   {
      case WM_INITDIALOG:
         DP5( hWDB, "tdShadowControl : WM_INITDIALOG" ) ;

         /* Register a message that we'll use to fake out the
          * dialog painting.
          */
         if (msgTDUTIL == 0)
         {
            msgTDUTIL = RegisterWindowMessage( "TDUTIL.DLL" ) ;
         }

         //
         // Enumerate all children looking for WHITE or GRAY FRAMES
         // and hide them
         //
         HackButtonsAndFrames( hDlg ) ;

         if (lParam)
            rgbBG = lParam ;
         else
            rgbBG = GetSysColor( COLOR_BTNFACE ) ;

         //
         // This will allow us to pass in WM_INITDIALOGs to change
         // the color.
         //
         hbrBG = GET_HBRBG( hDlg ) ;

         if (hbrBG)
         {
            DP( hWDB, "WM_INITDIALOG: Deleting brush : 0x%04X", hbrBG ) ;
            DeleteObject( hbrBG ) ;
         }

         hbrBG = CreateSolidBrush( rgbBG ) ;
         DP( hWDB, "WM_INITDIALOG: Created brush : 0x%04X", hbrBG ) ;

         SET_RGBBG( hDlg, rgbBG ) ;
         SET_HBRBG( hDlg, hbrBG ) ;
      return TRUE ;

#ifdef WIN32
      case WM_CTLCOLORBTN:
      case WM_CTLCOLORMSGBOX:
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLORDLG:
         rgbBG = GET_RGBBG( hDlg ) ;
         hbrBG = GET_HBRBG( hDlg ) ;

         SetBkColor( (HDC)wParam, rgbBG ) ;
         SetTextColor( (HDC)wParam, tdMapTextColor( rgbBG ) ) ;

         #if 0
         if (hbrBkGnd && uiMsg == WM_CTLCOLORDLG)
            return (BOOL)hbrBkGnd ;
         #endif

      return (BOOL)hbrBG ;

      case WM_CTLCOLORLISTBOX:
      case WM_CTLCOLORSCROLLBAR:
      return FALSE ;

#else
      case WM_CTLCOLOR:
         DP5( hWDB, "tdShadowControl : WM_CTLCOLOR" ) ;
         switch (HIWORD( lParam ))
         {
            case CTLCOLOR_BTN:
            case CTLCOLOR_MSGBOX:
            case CTLCOLOR_STATIC:
            case CTLCOLOR_DLG:
               rgbBG = GET_RGBBG( hDlg ) ;
               hbrBG = GET_HBRBG( hDlg ) ;

               SetBkColor( (HDC)wParam, rgbBG ) ;
               SetTextColor( (HDC)wParam, tdMapTextColor( rgbBG ) ) ;

               #if 0
               if (hbrBkGnd && (HIWORD(lParam) == CTLCOLOR_DLG))
                  return (BOOL)hbrBkGnd ;
               #endif

               return (BOOL)hbrBG ;

            case CTLCOLOR_EDIT:
            case CTLCOLOR_LISTBOX:
            case CTLCOLOR_SCROLLBAR:
               return FALSE ;

            default:
               return FALSE ;

         }
      break ;
#endif

      case WM_PAINT:
      {
         if (msgTDUTIL != 0)
            PostMessage ( hDlg, msgTDUTIL, 0, 0L ) ;

         return FALSE ;
      }

      case WM_DESTROY:
         REMOVE_RGBBG( hDlg ) ;
         hbrBG = REMOVE_HBRBG( hDlg ) ;
         if (hbrBG)
         {
            DP( hWDB, "WM_DESTROY: Deleting brush : 0x%04X", hbrBG ) ;
            DeleteObject( hbrBG ) ;
         }
      break ;

      default:
         if (uiMsg == msgTDUTIL)
         {
            HDC hDC ;

            if (hDC = GetDC( hDlg ))
            {
               rgbBG = GET_RGBBG( hDlg ) ;

               SetBkColor( hDC, rgbBG ) ;
               SetTextColor( hDC, tdMapTextColor( rgbBG ) ) ;
               tdDrawChildren3D( hDC, hDlg ) ;
               ReleaseDC( hDlg, hDC ) ;
            }

            return TRUE ;
         }

         return FALSE ;
   }

   return TRUE ;
} /* tdShadowControl()  */

/****************************************************************
 *  BOOL NEAR PASCAL HackButtonsAndFrames( HWND hWnd )
 *
 *  Description: 
 *
 *    Subclasses checkboxes, radiobuttons, and group boxes.  Hides
 *    grey and white frames.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL HackButtonsAndFrames( HWND hWnd )
{
   HWND  hwndChild ;
   WORD  wStyle ;

   for (hwndChild = GetWindow( hWnd, GW_CHILD ) ;
        hwndChild != NULL && IsChild( hWnd, hwndChild ) ;
        hwndChild = GetWindow( hwndChild, GW_HWNDNEXT ))
   {
      /*
       * If the bitmap was loaded then we have 3d effects enabled
       */
      if (hbmBtn && GetWindowLong( hwndChild, GWL_WNDPROC ) == (LONG)lpfnDefButton)
      {
         /* it's a button so subclass it */
         wStyle = LOWORD( GetWindowLong( hwndChild, GWL_STYLE ) ) ;

         if (wStyle >= BS_CHECKBOX && wStyle <= BS_AUTORADIOBUTTON)
         {
            DP3( hWDB, "Subclassing button!" ) ;

            SetWindowLong( hwndChild, GWL_WNDPROC, (LONG)fnNewButton ) ;

            if (GetWindowLong( hwndChild, GWL_WNDPROC ) != (LONG)lpfnDefButton )
            {
               DP1( hWDB, "Did not subclass!" );
            }
         }

      }

      switch(ChildKind( hwndChild ))
      {
         case CHILD_GRAYFRAME    :
         case CHILD_WHITEFRAME   :
            ShowWindow( hwndChild, SW_HIDE ) ;
      }
   }

   return TRUE ;

} /* HackButtonsAndFrames()  */

/****************************************************************
 *  VOID WINAPI tdDraw3DCrease( HDC hDC, LPRECT lprc, UINT uiFlags )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI tdDraw3DCrease( HDC hDC, LPRECT lpRect, UINT uiFlags )
{
   RECT       rc ;
   COLORREF   rgbOld ;

   if (fMonochrome)
   {
      DP1( hWDB, "fMonochrome!" ) ;
      rgbOld = SetBkColor( hDC, 
               GetNearestColor( hDC, tdGetShadowColor( GetBkColor( hDC ) ) ) ) ;

      tdDrawFaceFrame( hDC, lpRect, 1 ) ;

      SetBkColor( hDC, rgbOld ) ;

      return ;
   }

   rc = *lpRect ;

   if (uiFlags & DRAW3D_OUT)
      OffsetRect( &rc, 1, 1 ) ;

   rgbOld = SetBkColor( hDC, tdGetShadowColor( GetBkColor( hDC ) ) ) ;

   DrawFrame( hDC, &rc ) ;

   if (uiFlags & DRAW3D_OUT)
      OffsetRect( &rc, -1, -1 ) ;
   else
      OffsetRect( &rc, 1, 1 ) ;


   SetBkColor( hDC, tdGetHighlightColor( rgbOld ) ) ;

   DrawFrame( hDC, &rc ) ;

   SetBkColor( hDC, rgbOld ) ;

} /* tdDraw3DCrease()  */

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
                               UINT uiShadowWidth, UINT uiFlags )
{
   /* sanity check--don't work if you don't have to! */
   if ( !uiShadowWidth || !RectVisible( hDC, lpRect ) )
       return ;

   if (uiFlags & DRAW3D_CREASE)
   {
      tdDraw3DCrease( hDC, lpRect, uiFlags ) ;
      return ;
   }

   /* if width is 1 use lines instead of polygons */
   if (uiShadowWidth == 1 || fMonochrome)
   {
      /* draw the top line */
      Draw3DLine1( hDC, lpRect->left, lpRect->top,
                     lpRect->right - lpRect->left,
                     DRAW3D_TOPLINE | uiFlags ) ;

      /* right line */
      Draw3DLine1( hDC, lpRect->right, lpRect->top,
                     lpRect->bottom - lpRect->top,
                     DRAW3D_RIGHTLINE | uiFlags ) ;

      /* bottom line */
      Draw3DLine1( hDC, lpRect->left, lpRect->bottom,
                     lpRect->right - lpRect->left,
                     DRAW3D_BOTTOMLINE | uiFlags ) ;

      /* left line */
      Draw3DLine1( hDC, lpRect->left, lpRect->top,
                     lpRect->bottom - lpRect->top,
                     DRAW3D_LEFTLINE | uiFlags ) ;
   }
   else
   {
      /* draw the top line */
      tdDraw3DLine( hDC, lpRect->left, lpRect->top,
                     lpRect->right - lpRect->left,
                     uiShadowWidth, DRAW3D_TOPLINE | uiFlags ) ;

      /* right line */
      tdDraw3DLine( hDC, lpRect->right, lpRect->top,
                     lpRect->bottom - lpRect->top,
                     uiShadowWidth, DRAW3D_RIGHTLINE | uiFlags ) ;

      /* bottom line */
      tdDraw3DLine( hDC, lpRect->left, lpRect->bottom,
                     lpRect->right - lpRect->left,
                     uiShadowWidth, DRAW3D_BOTTOMLINE | uiFlags ) ;

      /* left line */
      tdDraw3DLine( hDC, lpRect->left, lpRect->top,
                     lpRect->bottom - lpRect->top,
                     uiShadowWidth, DRAW3D_LEFTLINE | uiFlags ) ;
   }
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
 *      UINT uiShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      UINT  uiFlags        :   The following flags may be passed to
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

VOID WINAPI tdDraw3DLine( HDC hDC, UINT x, UINT y, UINT uiLen,
                               UINT uiShadowWidth, UINT uiFlags ) 
{
   HBRUSH  hOldBrush ;
   HBRUSH  hBrush ;
   HPEN    hOldPen ;
   BOOL    fDark ;
   POINT   Point[ 4 ] ;         /* define a polgon with 4 points    */

   /* if width is zero, don't do nothin'! */
   if ( !uiShadowWidth )
       return ;

   /* if width is 1 use lines instead of polygons */
   if (uiShadowWidth == 1 || fMonochrome)
   {
      Draw3DLine1( hDC, x, y, uiLen, uiFlags ) ;
      return ;
   }

   /* define shape of polygon--origin is always the same */
   Point[0].x = x ;
   Point[0].y = y ;

   /*  To do this we'll simply draw a polygon with four sides, using 
    *  the appropriate brush.  I dare you to ask me why this isn't a
    *  switch/case!
    */
   if ( uiFlags & DRAW3D_TOPLINE )
   {
       /* across to right */
       Point[1].x = x + uiLen - (uiShadowWidth == 1 ? 1 : 0) ;
       Point[1].y = y ;

       /* down/left */
       Point[2].x = x + uiLen - uiShadowWidth ;
       Point[2].y = y + uiShadowWidth ;

       /* accross to left */
       Point[3].x = x + uiShadowWidth ;
       Point[3].y = y + uiShadowWidth ;

       /* select 'dark' brush if 'in'--'light' for 'out' */
       fDark = ( uiFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }

   /* possibly the bottom? */
   else if ( uiFlags & DRAW3D_BOTTOMLINE )
   {
       /* across to right */
       Point[1].x = x + uiLen ;
       Point[1].y = y ;

       /* up/left */
       Point[2].x = x + uiLen - uiShadowWidth ;
       Point[2].y = y - uiShadowWidth ;

       /* accross to left */
       Point[3].x = x + uiShadowWidth ;
       Point[3].y = y - uiShadowWidth ;

       /* select 'light' brush if 'in' */
       fDark = ( uiFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* ok, it's gotta be left? */
   else if ( uiFlags & DRAW3D_LEFTLINE )
   {
       /* down */
       Point[1].x = x ;
       Point[1].y = y + uiLen - (uiShadowWidth == 1 ? 1 : 0) ;

       /* up/right */
       Point[2].x = x + uiShadowWidth ;
       Point[2].y = y + uiLen - uiShadowWidth ;

       /* down */
       Point[3].x = x + uiShadowWidth ;
       Point[3].y = y + uiShadowWidth ;

       /* select 'dark' brush if 'in'--'light' for 'out' */
       fDark = ( uiFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }
   
   /* well maybe it's for the right side? */
   else if ( uiFlags & DRAW3D_RIGHTLINE )
   {
       /* down */
       Point[1].x = x ;
       Point[1].y = y + uiLen ;

       /* up/left */
       Point[2].x = x - uiShadowWidth ;
       Point[2].y = y + uiLen - uiShadowWidth ;

       /* up */
       Point[3].x = x - uiShadowWidth ;
       Point[3].y = y + uiShadowWidth ;

       /* select 'light' brush if 'in' */
       fDark = ( uiFlags & DRAW3D_IN ) ? FALSE : TRUE ;
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
 *  VOID WINAPI
 *    tdDrawChild3DRect( HDC     hDC,
 *                       HWND    hwndChild,
 *                       UINT    uiWidth,
 *                       UINT    uiFlags )
 *
 *  Description: 
 *
 *    Draws a 3d border around a child control.
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI tdDrawChild3DRect( HDC hDC, HWND hwndChild,
                                   UINT uiWidth, UINT uiFlags )
{
   RECT     rc ;
   UINT     w = ChildKind( hwndChild ) ;

   // deterimine what kind of child it is:
   switch (w)
   {
      case CHILD_UNKNOWN      :
//         DP5( hWDB, "UNKNOWN" ) ;
      break ;

      case CHILD_BUTTON       :
      break ;

      case CHILD_ICON         :
      break ;

      case CHILD_TEXT         :
//         goto Normal ;
      break ;

      case CHILD_BLACKFRAME   :
      {
         TEXTMETRIC  tm ;
         GetTextMetrics( hDC, &tm ) ;
         tdDrawDropShadow( hDC, hwndChild,
                         tm.tmAveCharWidth / 2 ) ;
      }
      break ;

      case CHILD_BLACKRECT     :
         goto Normal ;
      break ;

      case CHILD_COMBO        :
#ifdef MUCKWITHCOMBOBOXES
         goto Normal ;
#endif
      break ;

      case CHILD_DROPDOWNCOMBO:
#ifdef MUCKWITHCOMBOBOXES
         goto Normal ;
#endif
      break ;

      case CHILD_GRAYFRAME    :
         tdGetChildRect( hwndChild, &rc ) ;
         if (fMonochrome)
         {
            TEXTMETRIC  tm ;
            GetTextMetrics( hDC, &tm ) ;
            tdDrawDropShadow( hDC, hwndChild,
                           tm.tmAveCharWidth / 2 ) ;
         }
         else
            InflateRect( &rc, uiWidth *2, uiWidth *2) ;

       tdDraw3DRect( hDC, &rc, uiWidth *2, DRAW3D_IN ) ;
      break ;

      case CHILD_GRAYRECT     :
         goto Normal ;
      break ;

      case CHILD_WHITEFRAME   :

         if (!fMonochrome)
         {
            tdGetChildRect( hwndChild, &rc ) ;
            InflateRect( &rc, uiWidth * 2, uiWidth *2) ;
            tdDraw3DRect( hDC, &rc, uiWidth * 2, DRAW3D_OUT ) ;
         }
      break ;

      case CHILD_WHITERECT    :
         goto Normal ;
      break ;

      case CHILD_GROUPBOX:
      break ;

      case CHILD_CHECKBOX:
      case CHILD_RADIOBUTTON:
      break ;

      default:

Normal:
         if (IsWindowVisible( hwndChild ))
         {
            if (fMonochrome)
            {
               TEXTMETRIC  tm ;
               GetTextMetrics( hDC, &tm ) ;
               tdDrawDropShadow( hDC, hwndChild,
                              tm.tmAveCharWidth / 2 ) ;
            }
            else
            {
               tdGetChildRect( hwndChild, &rc ) ;
               InflateRect( &rc, uiWidth, uiWidth) ;
               tdDraw3DRect( hDC, &rc, uiWidth, uiFlags ) ;
            }
         }

      break ;
   }

} /* DrawChild3DRect()  */

/****************************************************************
 *  BOOL WINAPI tdDrawChildren3D( HDC hDC, HWND hDlg )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI tdDrawChildren3D( HDC hDC, HWND hDlg )
{
   HWND  hwndChild ;

   for (hwndChild = GetWindow( hDlg, GW_CHILD ) ;
        hwndChild != NULL && IsChild( hDlg, hwndChild ) ;
        hwndChild = GetWindow( hwndChild, GW_HWNDNEXT ))
   {
      tdDrawChild3DRect( hDC, hwndChild, SHADOWWIDTH, DRAW3D_IN ) ;
   }

   return TRUE ;

} /* tdDrawChildren3D()  */

/****************************************************************
 *  BOOL WINAPI
 *    tdDrawDropShadow( HDC hDC, HWND hWnd, UINT ShadouiWidth )
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
 *  ShadouiWidth      int The width, in device UINTs, of the shadow.
 *
 *    This function should draw a shadow around any window, whether
 *    it is a dialog control or not.
 *
 ****************************************************************/
BOOL WINAPI
   tdDrawDropShadow( HDC hDC, HWND hWnd, UINT uiWidth )
{
   RECT     rc ;
   POINT    Points[7] ;
   HBRUSH   hbrOld ;
   HBRUSH   hbr ;

   if (!SelectObject (hDC, GetStockObject (NULL_PEN)))
   {
      DP1( hWDB, "DrawDropShadow SelectPen failed" ) ;
      return FALSE ;
   }

   tdGetChildRect( hWnd, &rc ) ;

   // Draw white on bottom and right
   Points [0].x = rc.left + uiWidth ;
   Points [0].y = rc.bottom ;

   Points [1].x = rc.left + uiWidth  ;
   Points [1].y = rc.bottom + uiWidth ;

   Points [2].x = rc.right + uiWidth ;
   Points [2].y = rc.bottom + uiWidth ;

   Points [3].x = rc.right + uiWidth ;
   Points [3].y = rc.top + uiWidth ;

   Points [4].x = rc.right ;
   Points [4].y = rc.top + uiWidth ;

   Points [5].x = rc.right ;
   Points [5].y = rc.bottom ;

   Points [6].x = rc.left ;
   Points [6].y = rc.bottom ;

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

/****************************************************************
 *  COLORREF WINAPI tdMapTextColor( COLORREF rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate text color for the given bg.
 *
 *  Comments:
 *
 ****************************************************************/
COLORREF WINAPI tdMapTextColor( COLORREF rgb )
{
   return rgb = GetSysColor( COLOR_WINDOWTEXT ) ;

   if (rgb == RGBRED     ||
       rgb == RGBGREEN   ||
       rgb == RGBBLUE    ||
       rgb == RGBBROWN   ||
       rgb == RGBMAGENTA ||
       rgb == RGBGRAY    ||
       rgb == RGBBLACK)
      return RGBWHITE ;

   return RGBBLACK ;

} /* tdMapTextColor()  */


/****************************************************************
 *  BOOL WINAPI
 *    tdExtTextOut3D( HDC hDC, int x, int y, UINT wOptions,
 *                  LPRECT lpRect, LPSTR lpString,
 *                  int nChars, LPINT lpDx, UINT uiWidth)
 *
 *  Description: 
 *
 *    Draws a string within a 3D frame.  Works just like ExtTextOut
 *    except the extra param and the wOptions can also have
 *    DRAW3D_IN or DRAW3D_OUT in it (default is IN).
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI
tdExtTextOut3D( HDC hDC, int x, int y, UINT wOptions,
              LPRECT lpRect, LPSTR lpString,
              int nChars, LPINT lpDx, UINT uiWidth) 
{
   RECT rc ;

   if (lpRect)
      rc = *lpRect ;
   else
   {
      SIZE   size ;

      rc.left = x ;
      rc.top = y ;
      GetTextExtentPoint( hDC, lpString, nChars, &size ) ;
      rc.right = rc.left + size.cx + (2*uiWidth) + 2 ;
      rc.bottom = rc.top + size.cy + (2*uiWidth) + 1 ;
   }

   // draw the shadow first, then the text
   tdDraw3DRect( hDC, &rc, uiWidth,
                 wOptions & (DRAW3D_IN | DRAW3D_OUT | DRAW3D_CREASE) ) ;

   //
   // Move the rect in
   //

   InflateRect( &rc, -(int)((2*uiWidth) + 2), -(int)((2*uiWidth) + 1) ) ;

   wOptions &= (0x000F) ;

   return ExtTextOut( hDC, x + uiWidth + 1, y + uiWidth + 1,
               wOptions, &rc, lpString, nChars, lpDx ) ;

} /* tdExtTextOut3D()  */

/****************************************************************
 *  BOOL WINAPI
 *    tdTextOut3D( HDC hDC, int xPos, int yPos, LPSTR lpsz,
 *               int nCount, UINT uiWidth, UINT uiStyle )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI
   tdTextOut3D( HDC hDC, int xPos, int yPos, LPSTR lpsz,
                int nCount, UINT uiWidth, UINT uiStyle )
{
   if (!nCount)
      nCount = lstrlen( lpsz ) ;

   return tdExtTextOut3D( hDC, xPos, yPos, uiStyle,
                        (LPRECT)0, lpsz, nCount, (LPINT)0, uiWidth ) ;
} /* tdTextOut3D()  */


/*
 *====================================================================
 *  Internal functions
 *====================================================================
 */

/****************************************************************
 *  VOID NEAR PASCAL
 *   Draw3DLine1( HDC hDC, UINT x, UINT y, UINT uiLen, UINT uiFlags )
 *
 *  Description: 
 *
 *    Does the same thing astdDraw3DLine but for single width lines.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL
   Draw3DLine1( HDC hDC, UINT x, UINT y, UINT uiLen, UINT uiFlags )
{
   BOOL    fDark ;
   RECT    rc ;
   COLORREF   rgbOld ;

   if ( uiFlags & DRAW3D_TOPLINE )
   {
      rc.left = x ;
      rc.right = rc.left + uiLen - 1 ;
      rc.top = y ;
      rc.bottom = y + 1 ;

      fDark = ( uiFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }

   /* possibly the bottom? */
   else if ( uiFlags & DRAW3D_BOTTOMLINE )
   {
      rc.left = x ;
      rc.right = rc.left + uiLen - 1 ;
      rc.top = y - 1 ;
      rc.bottom = y ;

      fDark = ( uiFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* ok, it's gotta be left? */
   else if ( uiFlags & DRAW3D_LEFTLINE )
   {
      rc.left = x ;
      rc.right = x + 1 ;
      rc.top = y ;
      rc.bottom = y + uiLen ;

      fDark = ( uiFlags & DRAW3D_IN ) ? TRUE : FALSE ;
   }
   
   /* well maybe it's for the right side? */
   else if ( uiFlags & DRAW3D_RIGHTLINE )
   {
      rc.left = x - 1 ;
      rc.right = x ;
      rc.top = y ;
      rc.bottom = y + uiLen ;

      fDark = ( uiFlags & DRAW3D_IN ) ? FALSE : TRUE ;
   }

   /* bad drugs? */
   else return ;

   /* select the appropriate color for the fill */
   if ( fDark || fMonochrome )
      rgbOld = SetBkColor( hDC,
               GetNearestColor( hDC, tdGetShadowColor( GetBkColor( hDC ) ) ) ) ;
   else
      rgbOld = SetBkColor( hDC,
               GetNearestColor( hDC, tdGetHighlightColor( GetBkColor( hDC ) ) ) ) ;

   /* finally, draw the dern thing */
   
   ExtTextOut( hDC, x, y, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

   SetBkColor( hDC, rgbOld ) ;

} /* Draw3DLine1()  */

VOID NEAR PASCAL DrawFrame( HDC hDC, LPRECT lprc )
{
   RECT    rc1 ;

   /* perform CopyRect w/o bloody windows style overhead */
   rc1 = *lprc ;

   /* top */
   rc1.top = lprc->top ;
   rc1.left = lprc->left ;
   rc1.bottom = lprc->top + 1 ;
   rc1.right = lprc->right ;

   /* blast it out */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* right */
   rc1.left = lprc->right - 1 ;
   rc1.bottom = lprc->bottom ;

   /* blast this part now */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* left */
   rc1.left = lprc->left ;
   rc1.right = lprc->left + 1 ;

   /* and another part */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* bottom */
   rc1.right = lprc->right ;
   rc1.top = lprc->bottom - 1 ;

   /* finish it off */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

} /* DrawFaceFrame() */

/****************************************************************
 *  UINT NEAR PASCAL ChildKind( hwndChild )
 *
 *  Description: 
 *
 *    This function figures out what kind of control hwndChild is.
 *
 *    The following are reported:
 *
 *     CHILD_BUTTON   - button, bmpbtn, radio button, check box
 *     CHILD_COMBO    - combo box
 *     CHILD_DROPDOWNCOMBO - combo box that can drop down
 *     CHILD_EDIT     - edit control
 *     CHILD_LISTBOX  - list box
 *     CHILD_SCROLLBAR - Scroll bar
 *     CHILD_TEXT     - static text control
 *     CHILD_BLACKFRAME
 *     CHILD_GRAYFRAME
 *     CHILD_GRAYRECT
 *     CHILD_ICON
 *     CHILD_WHITEFRAME
 *     CHILD_WHITERECT
 *     CHILD_GROUPBOX
 *     CHILD_UNKNOWN
 *
 *  Comments:
 *
 ****************************************************************/
UINT NEAR PASCAL ChildKind( HWND hwndChild )
{
   #define MAX_CLASSNAME 64 

   char szName[MAX_CLASSNAME] ;

   UINT uiStyle ;

   uiStyle = LOWORD( GetWindowLong( hwndChild, GWL_STYLE ) ) ;

   //
   // First figure out the class:
   //
   if (!GetClassName( hwndChild, szName, MAX_CLASSNAME ))
      return CHILD_UNKNOWN ;

   if (!lstrcmpi( szName, "BUTTON" ))
   {
      switch( uiStyle )
      {
         case BS_GROUPBOX:
            return CHILD_GROUPBOX ;

         case BS_USERBUTTON: 
         case BS_AUTORADIOBUTTON:
         case BS_OWNERDRAW: 
         case BS_LEFTTEXT:    
         case BS_PUSHBUTTON:    
         case BS_DEFPUSHBUTTON:
            return CHILD_BUTTON ;

         case BS_CHECKBOX:    
         case BS_AUTOCHECKBOX:
         case BS_3STATE:    
         case BS_AUTO3STATE: 
            return CHILD_CHECKBOX ;

         case BS_RADIOBUTTON:
            return CHILD_RADIOBUTTON ;
      }

      return CHILD_BUTTON ;
   }

   if (!lstrcmpi( szName, "EDIT" ))
      return CHILD_EDIT ;

   if (!lstrcmpi( szName, "LISTBOX" ))
      return CHILD_LISTBOX ;

   if (!lstrcmpi( szName, "SCROLLBAR" ))
      return CHILD_SCROLLBAR ;

   if (!lstrcmpi( szName, "STATIC" ))
   {
      switch (uiStyle)
      {
         case SS_ICON:
            return CHILD_ICON ;

         case  SS_BLACKFRAME:
            return CHILD_BLACKFRAME ;

         case  SS_BLACKRECT:
            return CHILD_BLACKRECT ;

         case  SS_GRAYFRAME:
            return CHILD_GRAYFRAME ;

         case  SS_GRAYRECT:
            return CHILD_GRAYRECT ;

         case  SS_WHITEFRAME:
            return CHILD_WHITEFRAME ;

         case  SS_WHITERECT:
            return CHILD_WHITERECT ;

      }

      return CHILD_TEXT ;
   }

   if (!lstrcmpi( szName, "COMBOBOX" ))
   {
      if (uiStyle == CBS_DROPDOWN || uiStyle == CBS_DROPDOWNLIST)
         return CHILD_DROPDOWNCOMBO ;
      else
         return CHILD_COMBO ;
   }

   return CHILD_UNKNOWN ;

} /* ChildKind()  */

/*
 *====================================================================
 *  Obsolete functions
 *====================================================================
 */

/** VOID WINAPI tdDrawFaceFrame( HDC hDC, LPRECT rc, UINT uiWidth )
 *
 *  DESCRIPTION: 
 *      This function draws a flat frame with the current button-face
 *      color.
 *
 *  ARGUMENTS:
 *      HDC hDC     :   The DC to draw into.
 *
 *      LPRECT rc   :   The containing rect for the new frame.
 *
 *      UINT uiWidth :   The width of the frame to draw.
 *
 *  RETURN (VOID WINAPI):
 *      The frame will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

VOID WINAPI tdDrawFaceFrame( HDC hDC, LPRECT rc, UINT uiWidth )
{
   RECT    rc1 ;

   /* don't go through a bunch of work if we don't have to */
   if ( !uiWidth )
       return ;

   /* perform CopyRect w/o bloody windows style overhead */
   rc1 = *rc ;

   /* top */
   rc1.top = rc->top ;
   rc1.left = rc->left ;
   rc1.bottom = rc->top + uiWidth ;
   rc1.right = rc->right ;

   /* blast it out */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* right */
   rc1.left = rc->right - uiWidth ;
   rc1.bottom = rc->bottom ;

   /* blast this part now */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* left */
   rc1.left = rc->left ;
   rc1.right = rc->left + uiWidth ;

   /* and another part */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* bottom */
   rc1.right = rc->right ;
   rc1.top = rc->bottom - uiWidth ;

   /* finish it off */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

} /* DrawFaceFrame() */


/****************************************************************
 *  BOOL WINAPI fnEnumChildren3D( HWND hWnd, LRESULT lParam )
 *
 *  Description: 
 *
 *    OBSOLETE: We now use GetWindow().  Exported for
 *    compatibility with older versions.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnEnumChildren3D( HWND hWnd, LRESULT lParam )
{
   return 0 ;
} /* fnEnumChildren()  */

/****************************************************************
 *  BOOL WINAPI fnEnumChildrenHide( HWND hWnd, LRESULT lParam )
 *
 *  Description: 
 *
 *    Function used by the EnumChildWindows Function for WM_INITDIALOG
 *    to hide child windows.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnEnumChildrenHide( HWND hWnd, LRESULT lParam )
{
   return 0 ;

} /* fnEnumChildren()  */

VOID WINAPI tdDraw3DLine1( HDC hDC, UINT x, UINT y, UINT uiLen, UINT uiFlags )
{
}

BOOL WINAPI tdHideSomeChildren( HWND hWnd )
{
   return 0 ;
}

UINT WINAPI tdChildKind( HWND hwndChild )
{
   return 0 ;
}


/************************************************************************
 * End of File: shadow.c
 ***********************************************************************/

