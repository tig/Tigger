/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  shadow.c
 *
 *     Remarks:
 *
 *    This module provides routines for drawing 3D shadows and other
 *    effects in Windows 3.0.
 *
 *   Revisions:  
 *     00.00.000  3/26/91 cek   First Version.
 *
 *
 ************************************************************************/

#define WINDLL
//#define IN_A_DLL

#include <windows.h>
#include "sharereg.h"

#ifndef IN_A_DLL
extern HANDLE hgInst ;
#define bMonochrome FALSE
#else
extern BOOL bMonochrome ;
#endif


#define GETHINST(hwnd)     (GetWindowWord( hwnd, GWW_HINSTANCE ))

#define SHADOWWIDTH  1

#define WM_MYPAINT      (WM_USER + 0x700)


BOOL FAR PASCAL fnEnumChildren3D( HWND hWnd, DWORD lParam ) ;
BOOL FAR PASCAL fnEnumChildrenHide( HWND hWnd, DWORD lParam ) ;

/** void FAR PASCAL DrawFaceFrame( HDC hDC, LPRECT rc, WORD wWidth )
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
 *      WORD wWidth :   The width of the frame to draw.
 *
 *  RETURN (void FAR PASCAL):
 *      The frame will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL DrawFaceFrame( HDC hDC, LPRECT rc, WORD wWidth )
{
   RECT    rc1 ;
   DWORD   rgbOld ;

   /* don't go through a bunch of work if we don't have to */
   if ( !wWidth )
       return ;

   /* set up color to be button-face color--so it may not be gray */
   rgbOld = SetBkColor( hDC, GetSysColor( COLOR_BTNFACE ) ) ;

   /* perform CopyRect w/o bloody windows style overhead */
   rc1 = *rc ;

   /* top */
   rc1.top = rc->top ;
   rc1.left = rc->left ;
   rc1.bottom = rc->top + wWidth ;
   rc1.right = rc->right ;

   /* blast it out */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* right */
   rc1.left = rc->right - wWidth ;
   rc1.bottom = rc->bottom ;

   /* blast this part now */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* left */
   rc1.left = rc->left ;
   rc1.right = rc->left + wWidth ;

   /* and another part */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* bottom */
   rc1.right = rc->right ;
   rc1.top = rc->bottom - wWidth ;

   /* finish it off */
   ExtTextOut( hDC, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL ) ;

   /* restore the old bk color */
   SetBkColor( hDC, rgbOld ) ;
} /* DrawFaceFrame() */


/** void FAR PASCAL Draw3DRect( HDC, LPRECT, WORD, WORD )
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
 *      WORD wShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      WORD wFlags         :   The following flags may be passed to describe
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
 *  RETURN (void FAR PASCAL):
 *      The 3D looking rectangle will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL Draw3DRect( HDC hDC, LPRECT lpRect,
                               WORD wShadowWidth, WORD wFlags )
{
   /* sanity check--don't work if you don't have to! */
   if ( !wShadowWidth || !RectVisible( hDC, lpRect ) )
       return ;

   /* draw the top line */
   Draw3DLine( hDC, lpRect->left, lpRect->top,
                   lpRect->right - lpRect->left,
                   wShadowWidth, DRAW3D_TOPLINE | wFlags ) ;

   /* right line */
   Draw3DLine( hDC, lpRect->right, lpRect->top,
                   lpRect->bottom - lpRect->top,
                   wShadowWidth, DRAW3D_RIGHTLINE | wFlags ) ;

   /* bottom line */
   Draw3DLine( hDC, lpRect->left, lpRect->bottom,
                   lpRect->right - lpRect->left,
                   wShadowWidth, DRAW3D_BOTTOMLINE | wFlags ) ;

   /* left line */
   Draw3DLine( hDC, lpRect->left, lpRect->top,
                   lpRect->bottom - lpRect->top,
                   wShadowWidth, DRAW3D_LEFTLINE | wFlags ) ;
} /* Draw3DRect() */


/** void FAR PASCAL Draw3DLine( HDC hDC, WORD x, WORD y, WORD nLen,
 *
 *  DESCRIPTION: 
 *      Draws a 3D line that can be used to make a 3D box.
 *
 *  ARGUMENTS:
 *      HDC hDC             :   Handle to the device context that will be
 *                              used to display the 3D line.
 *
 *      WORD x, y           :   Coordinates of the beginning of the line.
 *                              These coordinates are in device units and
 *                              represent the _outside_ most point. Horiz-
 *                              ontal lines are drawn from left to right and
 *                              vertical lines are drawn from top to bottom.
 *
 *      WORD wShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      WORD  wFlags        :   The following flags may be passed to
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
 *  RETURN (void FAR PASCAL):
 *      The line will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL Draw3DLine( HDC hDC, WORD x, WORD y, WORD nLen,
                               WORD wShadowWidth, WORD wFlags ) 
{
   HPEN    hOldPen ;
   BOOL    fDark ;
   POINT   Point[ 4 ] ;         /* define a polgon with 4 points    */

   /* if width is zero, don't do nothin'! */
   if ( !wShadowWidth )
       return ;

   /* if width is 1 use lines instead of polygons */
   if (wShadowWidth == 1 || bMonochrome)
   {
      Draw3DLine1( hDC, x, y, nLen, wFlags ) ;
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

    {
        HBRUSH  hOldBrush ;
        HBRUSH  hBrush ;

        /* select the appropriate color for the fill */
        if ( fDark == TRUE )
        {
           hBrush = CreateSolidBrush( GetNearestColor( hDC,
                                      GetShadowColor( GetBkColor( hDC ) ) ) ) ;
        }                                 
        else
        {
           hBrush = CreateSolidBrush( GetNearestColor( hDC,
                                      GetHighlightColor( GetBkColor( hDC ) ) ) ) ;
        }
           
        if (hOldBrush = SelectObject( hDC, hBrush ))
        {
            /* finally, draw the dern thing */
            Polygon( hDC, (LPPOINT)&Point, 4 ) ;

            /* restore what we killed */
            SelectObject( hDC, hOldBrush ) ;
        }

        DeleteObject( hBrush ) ;
   }
   
   SelectObject( hDC, hOldPen ) ;
} /* Draw3DLine() */

/****************************************************************
 *  void FAR PASCAL
 *    Draw3DLine1( HDC hDC, WORD x, WORD y, WORD nLen, WORD wFlags )
 *
 *  Description: 
 *
 *    Does the same thing as Draw3DLine but for single width lines.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL
Draw3DLine1( HDC hDC, WORD x, WORD y, WORD nLen, WORD wFlags )
{
   BOOL    fDark ;
   RECT    rc ;
   DWORD   rgbOld ;

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
               GetNearestColor( hDC, GetShadowColor( GetBkColor( hDC ) ) ) ) ;
   else
      rgbOld = SetBkColor( hDC,
               GetNearestColor( hDC, GetHighlightColor( GetBkColor( hDC ) ) ) ) ;

   /* finally, draw the dern thing */
   
   ExtTextOut( hDC, x, y, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

   SetBkColor( hDC, rgbOld ) ;

} /* Draw3DLine1()  */

/****************************************************************
 *  BOOL FAR PASCAL
 *    ExtTextOut3D( HDC hDC, short x, short y, WORD wOptions,
 *                  LPRECT lpRect, LPSTR lpString,
 *                  short nChars, LPINT lpDx, WORD wWidth)
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
BOOL FAR PASCAL
ExtTextOut3D( HDC hDC, short x, short y, WORD wOptions,
              LPRECT lpRect, LPSTR lpString,
              short nChars, LPINT lpDx, WORD wWidth) 
{
   RECT rc ;

   if (lpRect)
      rc = *lpRect ;
   else
   {
      DWORD  dw ;

      rc.left = x ;
      rc.top = y ;
      rc.right = rc.left +
         LOWORD( dw = GetTextExtent( hDC, lpString, nChars ) ) + (2*wWidth) + 2 ;
      rc.bottom = rc.top + HIWORD( dw ) + (2*wWidth) + 1 ;
   }

   // draw the shadow first, then the text

   if (wOptions & DRAW3D_OUT)
      Draw3DRect( hDC, &rc, wWidth, DRAW3D_OUT ) ;
   else
      Draw3DRect( hDC, &rc, wWidth, DRAW3D_IN ) ;

   //
   // Move the rect in
   //

   InflateRect( &rc, -((2*(int)wWidth) + 2), -((2*(int)wWidth) + 1) ) ;

   wOptions &= (0x000F) ;

   return ExtTextOut( hDC, x + wWidth + 1, y + wWidth,
               wOptions, &rc, lpString, nChars, lpDx ) ;

} /* ExtTextOut3D()  */

/****************************************************************
 *  BOOL FAR PASCAL
 *    TextOut3D( HDC hDC, short xPos, short yPos, LPSTR lpsz,
 *               short nCount, WORD wWidth, WORD wStyle )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL
TextOut3D( HDC hDC, short xPos, short yPos, LPSTR lpsz,
           short nCount, WORD wWidth, WORD wStyle )
{
   if (!nCount)
      nCount = lstrlen( lpsz ) ;

   return ExtTextOut3D( hDC, xPos, yPos, wStyle,
                        (LPRECT)0, lpsz, nCount, (LPINT)0, wWidth ) ;
} /* TextOut3D()  */

/****************************************************************
 *  void FAR PASCAL GetChildRect( HWND hwndChild, LPRECT lprc )
 *
 *  Description: 
 *
 *    Gets the rectangle of a child window in parent client coords.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL GetChildRect( HWND hwndChild, LPRECT lprc )
{
   GetWindowRect( hwndChild, lprc ) ;
   ScreenToClient( GetParent( hwndChild ), (LPPOINT)&lprc->left ) ;
   ScreenToClient( GetParent( hwndChild ), (LPPOINT)&lprc->right ) ;

} /* GetChildRect()  */

/****************************************************************
 *  void FAR PASCAL
 *    DrawChild3DRect( HDC hDC, HWND hwndChild, WORD wWidth, WORD wFlags )
 *
 *  Description: 
 *
 *    Draws a 3d border around a child control.
 *
 *  Comments:
 *
 ****************************************************************/
void FAR PASCAL DrawChild3DRect( HDC hDC, HWND hwndChild, WORD wWidth, WORD wFlags )
{
   RECT     rc ;
   WORD     w = ChildKind( hwndChild ) ;

   // deterimine what kind of child it is:
   switch (w)
   {
      case CHILD_UNKNOWN      :
         DP5( hWDB, "UNKNOWN" ) ;
      break ;

      case CHILD_BUTTON       :
         DP5( hWDB, "BUTTON" ) ;
      break ;

      case CHILD_ICON         :
         DP5( hWDB, "ICON" ) ;
      break ;

      case CHILD_TEXT         :
         DP5( hWDB, "TEXT" ) ;
         goto Normal ;
      break ;

      case CHILD_BLACKFRAME   :
      {
         TEXTMETRIC  tm ;
         DP5( hWDB, "BLACKFRAME" ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawDropShadow( hDC, hwndChild,
                         tm.tmAveCharWidth / 2 ) ;
      }
      break ;

      case CHILD_BLACKRECT     :
         DP5( hWDB, "BLACKRECT" ) ;
         goto Normal ;
      break ;

      case CHILD_COMBO        :
         DP5( hWDB, "COMBO" ) ;
         goto Normal ;
      break ;

      case CHILD_DROPDOWNCOMBO:
         DP5( hWDB, "DROPDOWNCOMBO" ) ;
         goto Normal ;
      break ;

      case CHILD_GRAYFRAME    :
         DP5( hWDB, "GRAYFRAME" ) ;
         GetChildRect( hwndChild, &rc ) ;
         if (bMonochrome)
         {
            TEXTMETRIC  tm ;
            GetTextMetrics( hDC, &tm ) ;
            DrawDropShadow( hDC, hwndChild,
                           tm.tmAveCharWidth / 2 ) ;
         }
         else
            InflateRect( &rc, wWidth *2, wWidth *2) ;

         Draw3DRect( hDC, &rc, wWidth *2, DRAW3D_IN ) ;
      break ;

      case CHILD_GRAYRECT     :
         DP5( hWDB, "GRAYRECT" ) ;
         goto Normal ;
      break ;

      case CHILD_WHITEFRAME   :
         DP5( hWDB, "WHITEFRAME" ) ;

         if (!bMonochrome)
         {
            GetChildRect( hwndChild, &rc ) ;
            InflateRect( &rc, wWidth * 2, wWidth *2) ;
            Draw3DRect( hDC, &rc, wWidth * 2, DRAW3D_OUT ) ;
         }
      break ;

      case CHILD_WHITERECT    :
         DP5( hWDB, "WHITERECT" ) ;
         goto Normal ;
      break ;

      case CHILD_GROUPBOX:
         DP5( hWDB, "GROUPBOX" ) ;
      break ;

      default:
         DP5( hWDB, "default: %d", w ) ;

Normal:
         if (IsWindowVisible( hwndChild ))
         {
            if (bMonochrome)
            {
               TEXTMETRIC  tm ;
               GetTextMetrics( hDC, &tm ) ;
               DrawDropShadow( hDC, hwndChild,
                              tm.tmAveCharWidth / 2 ) ;
            }
            else
            {
               GetChildRect( hwndChild, &rc ) ;
               InflateRect( &rc, wWidth, wWidth) ;
               Draw3DRect( hDC, &rc, wWidth, wFlags ) ;
            }
         }

      break ;
   }

} /* DrawChild3DRect()  */

/****************************************************************
 *  WORD FAR PASCAL ChildKind( hwndChild )
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
WORD FAR PASCAL ChildKind( HWND hwndChild )
{
   #define MAX_CLASSNAME 64 

   char szName[MAX_CLASSNAME] ;

   WORD wStyle ;

   wStyle = LOWORD( GetWindowLong( hwndChild, GWL_STYLE ) ) ;

   //
   // First figure out the class:
   //
   if (!GetClassName( hwndChild, szName, MAX_CLASSNAME ))
      return CHILD_UNKNOWN ;

   if (!lstrcmpi( szName, "BUTTON" ) ||
       !lstrcmpi( szName, "BMPBTN" ) )
   {
      if (wStyle == BS_GROUPBOX)
         return CHILD_GROUPBOX ;

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
      switch (wStyle)
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
      if (wStyle == CBS_DROPDOWN || wStyle == CBS_DROPDOWNLIST)
         return CHILD_DROPDOWNCOMBO ;
      else
         return CHILD_COMBO ;
   }

   return CHILD_UNKNOWN ;

} /* ChildKind()  */

/****************************************************************
 *  BOOL FAR PASCAL DrawChildren3D( HDC hDC, HWND hWnd )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL DrawChildren3D( HDC hDC, HWND hWnd )
{
   FARPROC lpfnEnum ;
   #ifndef IN_A_DLL
   HANDLE  hInst = GETHINST( hWnd ) ;
   #endif

   if (lpfnEnum = MakeProcInstance( fnEnumChildren3D, hInst ))
   {
      EnumChildWindows( hWnd, lpfnEnum, MAKELONG(hDC,hWnd) ) ;

      FreeProcInstance( lpfnEnum ) ;
   }
   else
   {
      DP1( hWDB, "MakeProcInstance() failed" ) ;
      return FALSE ;
   }

   return TRUE ;

} /* DrawChildren3D()  */

/****************************************************************
 *  BOOL FAR PASCAL fnEnumChildren3D( HWND hWnd, DWORD lParam )
 *
 *  Description: 
 *
 *    Function used by the EnumChildWindows Function for DrawChildren3D
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnEnumChildren3D( HWND hWnd, DWORD lParam )
{
   //
   // Only enumerate the children.  Not the grand children!
   //
   if (GetParent( hWnd ) == HIWORD( lParam ))
   {
      DrawChild3DRect( LOWORD( lParam ), hWnd, SHADOWWIDTH, DRAW3D_IN ) ;
   }

   return TRUE ;

} /* fnEnumChildren()  */

/****************************************************************
 *  BOOL FAR PASCAL
 *    DrawDropShadow( HDC hDC, HWND hWnd, short ShadowWidth )
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
 *  hBrush           HBRUSH Handle to the brush that is to be used to
 *                          draw the shadow.
 *  ShadowWidth      short The width, in device units, of the shadow.
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
 *          DrawDropShadow (hDC,                                
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
BOOL FAR PASCAL
   DrawDropShadow( HDC hDC, HWND hWnd, short ShadowWidth )
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

   GetChildRect( hWnd, &Rect ) ;

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
} /* DrawDropShadow()  */

/****************************************************************
 *  BOOL FAR PASCAL
 *  ShadowControl( HWND hDlg, WORD wMsg,
 *                 WORD wParam, LONG lParam, HBRUSH hbrBG )
 *
 *  Description: 
 *
 *    This function is called kind of like a DefWindowProc for
 *    dialog boxes.  It takes care of all "special" painting.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL
ShadowControl( HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) 
{
   HBRUSH hbrBG ;
   DWORD  rgbBG ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         //
         // Enumerate all children looking for WHITE or GRAY FRAMES
         // and hide them
         //
//         HideSomeChildren( hDlg ) ;

         if (lParam)
            rgbBG = lParam ;
         else
            rgbBG = GetSysColor( COLOR_BTNFACE ) ;

         hbrBG = CreateSolidBrush( rgbBG ) ;

         SetProp( hDlg, "rgbBG_L", LOWORD( rgbBG ) ) ;
         SetProp( hDlg, "rgbBG_H", HIWORD( rgbBG ) ) ;
         SetProp( hDlg, "hbrBG", hbrBG ) ;
         
         return TRUE ;

      case WM_CTLCOLOR:

         switch (HIWORD( lParam ))
         {
            case CTLCOLOR_BTN:
            case CTLCOLOR_MSGBOX:
            case CTLCOLOR_EDIT:
            case CTLCOLOR_LISTBOX:
            case CTLCOLOR_STATIC:
            case CTLCOLOR_DLG:
               rgbBG = MAKELONG( GetProp( hDlg, "rgbBG_L" ),
                                 GetProp( hDlg, "rgbBG_H" ) ) ;
               SetBkColor( (HDC)wParam, rgbBG ) ;
               SetTextColor( (HDC)wParam, MapTextColor( rgbBG ) ) ;
               hbrBG = GetProp( hDlg, "hbrBG" ) ;
               return hbrBG ;

            case CTLCOLOR_SCROLLBAR:
               return FALSE ;

            default:
               return FALSE ;

         }
      break ;

      case WM_PAINT:
      {
         PostMessage ( hDlg, WM_MYPAINT, 0, 0L ) ;
         return FALSE ;
      }

      case WM_MYPAINT:
      {
         HDC hDC ;

         if (hDC = GetDC( hDlg ))
         {
            rgbBG = MAKELONG( GetProp( hDlg, "rgbBG_L" ),
                              GetProp( hDlg, "rgbBG_H" ) ) ;

            SetBkColor( hDC, rgbBG ) ;
            SetTextColor( hDC, MapTextColor( rgbBG ) ) ;
            DrawChildren3D( hDC, hDlg ) ;
            ReleaseDC( hDlg, hDC ) ;
         }
      }
      break ;

      case WM_DESTROY:
         RemoveProp( hDlg, "rgbBG_L" ) ;
         RemoveProp( hDlg, "rgbBG_H" ) ;
         hbrBG = RemoveProp( hDlg, "hbrBG" ) ;
         DeleteObject( hbrBG ) ;
      break ;

      default:
         return FALSE ;
   }

   return TRUE ;
} /* ShadowControl()  */

/****************************************************************
 *  BOOL FAR PASCAL HideSomeChildren( HWND hWnd )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL HideSomeChildren( HWND hWnd )
{
   FARPROC lpfnEnum ;
   #ifndef IN_A_DLL
   HANDLE  hInst = GETHINST( hWnd ) ;
   #endif

   if (lpfnEnum = MakeProcInstance( fnEnumChildrenHide, hInst ))
   {
      EnumChildWindows( hWnd, lpfnEnum, MAKELONG(0, hWnd) ) ;

      FreeProcInstance( lpfnEnum ) ;
   }
   else
   {
      DP1( hWDB, "MakeProcInstance() failed" ) ;
      return FALSE ;
   }

   return TRUE ;

} /* HideSomeChildren()  */
/****************************************************************
 *  BOOL FAR PASCAL fnEnumChildrenHide( HWND hWnd, DWORD lParam )
 *
 *  Description: 
 *
 *    Function used by the EnumChildWindows Function for WM_INITDIALOG
 *    to hide child windows.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnEnumChildrenHide( HWND hWnd, DWORD lParam )
{
   //
   // Only enumerate the children.  Not the grand children!
   //
   if (GetParent( hWnd ) == HIWORD( lParam ))
   {
      switch(ChildKind( hWnd ))
      {
         case CHILD_GRAYFRAME    :
         case CHILD_WHITEFRAME   :
            ShowWindow( hWnd, SW_HIDE ) ;

      }
   }

   return TRUE ;

} /* fnEnumChildren()  */



/****************************************************************
 *  DWORD FAR PASCAL GetHighLightColor( DWORD rgb )
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
DWORD FAR PASCAL GetHighlightColor( DWORD rgb )
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

} /* GetHighLightColor()  */


/****************************************************************
 *  DWORD FAR PASCAL GetShadowColor( DWORD rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate shadow color for the given rgb.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL GetShadowColor( DWORD rgb )
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
} /* GetShadowColor()  */

/****************************************************************
 *  DWORD FAR PASCAL MapTextColor( DWORD rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate text color for the given bg.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL MapTextColor( DWORD rgb )
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

} /* MapTextColor()  */



/*************************************************************************
 * End of File: shadow.c
 ************************************************************************/

