/************************************************************************
 *
 *     Project:  TDUTIL
 *
 *      Module:  3dctrl.c
 *
 *     Remarks:  
 *
 *       If MULTICOLOR is defined in the makefile the subclassed
 *       buttons will work with background colors other than light gray.
 *       The primary disadvantage to turning MULTICOLOR on is that it
 *       over doubles the size of the DLL because of the size of the
 *       bitmap.
 *
 *       Other schemes could be devised for generating the bitmaps on
 *       the fly for other colors, but that would sacrifice speed.
 *
 *   Revisions:  
 *    1.00.000    2/14/92  cek   Wrote it.
 *    1.01.000    2/17/92  cek   Moved into tdutil
 *    1.02.000    3/25/92  cek   Fixed bug regarding focus from taskman
 *    2.00.000    5/20/92  cek   Incorporated wesc's bitmaps and some of
 *                               his techniques...thanks wes.
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "itdutil.h"

extern HMODULE    hInstDLL ;
extern BOOL       fMonochrome ;
extern HBITMAP    hbmBtn ;
extern LRESULT    (CALLBACK *lpfnDefButton)(HWND, UINT, WPARAM, LPARAM ) ;
extern UINT       dxBorder;
extern UINT       dyBorder;
extern HBRUSH   hbrBkGnd ;

#define CX_BMP        14
#define CY_BMP        13

/*****************************************************************
 * Stuff for subclassing the 'BUTTON' class
 *****************************************************************/

/* The state of a button (retreived by BM_GETSTATE) is a bitfield.
 * The following three states are documented.  There are more bits
 * used, but they are not documented (or needed here).
 */
#define BSTATE_CHECK         0x0003
#define BSTATE_STATE         0x0004
#define BSTATE_FOCUS         0x0008

/* These guys are flags that we use to keep track of what the current
 * user action is.
 */
#define ACTION_TEXT          0x0002
#define ACTION_CHECK         0x0004
#define ACTION_FOCUS         0x0008     
#define ACTION_BKGND         0x0010
#define ACTION_ERASETEXT     0x0020     // for groupboxes

VOID NEAR PASCAL PaintBtn( HWND hwnd, HDC hdc, UINT uiAction ) ;
UINT NEAR PASCAL WhichColor( HDC hdc, COLORREF rgb ) ;


/****************************************************************
 *  LRESULT CALLBACK fnNewButton( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *    This is the subclass window proc for the button class.  Look in
 *    shadow.c for where we subclass the buttons.
 *
 ****************************************************************/
LRESULT CALLBACK fnNewButton( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   LRESULT     lRet;
   UINT        uiAction ;

   switch(uiMsg)
   {
      case BM_SETSTATE:
      case BM_SETCHECK:
         uiAction = ACTION_CHECK;
      goto DoPaint;

      case WM_KILLFOCUS:     
         uiAction = 0;
      goto DoPaint;

      case WM_ENABLE:
         uiAction = ACTION_TEXT | ACTION_CHECK ;
      goto DoPaint;

      case WM_SETFOCUS:
      {
         DWORD       dwStyle;
         HDC         hdc;
         UINT        uiState;
         UINT        uiStateNew;

         if (wParam == NULL)
            uiAction = ACTION_CHECK | ACTION_TEXT | ACTION_BKGND ;
         else
            uiAction = ACTION_CHECK | ACTION_TEXT ;

DoPaint:

         /* Ask the button for it's state */
         uiState = Button_GetState( hwnd ) ;

         if((dwStyle = GetWindowLong( hwnd, GWL_STYLE )) & WS_VISIBLE)
         {

            /* Allow the default window proc to set it's internal state
             * info.  Fool it into thinking the window is not visible.
             */
            SetWindowLong( hwnd, GWL_STYLE, dwStyle & ~(WS_VISIBLE) ) ;

            lRet = CallWindowProc( lpfnDefButton,
                                   hwnd, uiMsg, wParam, lParam ) ;

            SetWindowLong( hwnd, GWL_STYLE,
                           GetWindowLong( hwnd, GWL_STYLE) | WS_VISIBLE );

            uiStateNew = Button_GetState( hwnd ) ;

            if((uiMsg   != BM_SETSTATE &&
                uiMsg   != BM_SETCHECK) ||
               uiState != uiStateNew)
            {
               if ((hdc = GetDC(hwnd)) != NULL)
               {
                  /* Always redraw if the state changed since we hid the
                   * window from the default proc above.
                   */
                  if ((uiState & BSTATE_CHECK) != (uiStateNew & BSTATE_CHECK))
                     uiAction |= ACTION_CHECK;

                  ExcludeUpdateRgn( hdc, hwnd ) ;

                  PaintBtn( hwnd, hdc,
                            uiAction | ((uiState ^ uiStateNew) & BSTATE_FOCUS)) ;

                  ReleaseDC( hwnd, hdc ) ;
               }
            }
            return lRet;
         }
      }
      break;

      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         UINT        uiState = Button_GetState( hwnd ) ;

         BeginPaint( hwnd, &ps ) ;

         if (GetWindowLong( hwnd, GWL_STYLE ) & WS_VISIBLE)
            PaintBtn( hwnd, ps.hdc,
                      ACTION_TEXT | ACTION_CHECK | (uiState & BSTATE_FOCUS) ) ;

         EndPaint( hwnd, &ps ) ;
      }
      return 0L;

      case WM_SETTEXT:
      {
         WORD wStyle = (WORD)(GetWindowLong( hwnd, GWL_STYLE ) ) & 0x1F;

        /* In case the new group box name is longer than the old
         * name, this paints over the old name before repainting 
         * the group box with the new name
         */      
        if (wStyle == BS_GROUPBOX)
        {
            HDC hdc = GetDC( hwnd ) ;
            LRESULT lr ;

            PaintBtn( hwnd, hdc, ACTION_ERASETEXT | ACTION_TEXT ) ;

            lr = DefWindowProc( hwnd, uiMsg, wParam, lParam ) ;

            PaintBtn( hwnd, hdc, ACTION_TEXT ) ;
            ReleaseDC( hwnd, hdc ) ;
            return lr ;

        }
      }
      break ;
   }

   return CallWindowProc( lpfnDefButton, hwnd, uiMsg, wParam, lParam ) ;

} /* fnNewButton()  */

/****************************************************************
 *  VOID NEAR PASCAL PaintBtn( HWND hwnd, HDC hdc, UINT uiAction )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL PaintBtn( HWND hwnd, HDC hdc, UINT uiAction )
{
   RECT        rc;
   RECT        rcClient;
   HFONT       hfont;
   LOGBRUSH    lbr ;
   WORD        wStyle;
   UINT        uiState;
   HBRUSH      hbrDlgBG;
   HBRUSH      hbrBG;
   HWND        hwndParent;
   int         cxBmp;
   int         cyBmp;
   int         cbWndText = 0 ;
   LPSTR       lpszWndText = NULL ;


   wStyle = (WORD)(GetWindowLong( hwnd, GWL_STYLE ) ) & 0x1F;

   hwndParent = GetParent( hwnd ) ;

   GetClientRect( hwnd, (LPRECT)&rcClient ) ;
   rc = rcClient ;

   if((hfont = GetWindowFont( hwnd )) != NULL)
      hfont = SelectObject( hdc, hfont ) ;

   #ifdef WIN32
   hbrDlgBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLORDLG, 
                                (WPARAM)hdc,
                                (LPARAM)hwnd ) ;
   #else
   hbrDlgBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLOR,
                                (WPARAM)hdc,
                                MAKELPARAM( hwnd, CTLCOLOR_DLG ) ) ;
   #endif

   GetObject( hbrDlgBG, sizeof(LOGBRUSH), &lbr ) ;

   #ifdef WIN32
   hbrBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLORBTN,
                                (WPARAM)hdc,
                                (LPARAM)hwnd ) ;
   #else
   hbrBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLOR,
                                (WPARAM)hdc,
                                MAKELPARAM( hwnd, CTLCOLOR_BTN ) ) ;
   #endif

   hbrBG = SelectObject( hdc, hbrBG ) ;

   IntersectClipRect(hdc, rc.left, rc.top, rc.right, rc.bottom);

   uiState = Button_GetState( hwnd ) ;

   if(uiAction & ACTION_BKGND)
      PatBlt( hdc, rc.left, rc.top,
            rc.right-rc.left, rc.bottom-rc.top, PATCOPY ) ;

   cyBmp = 0;

#ifdef MUTLICOLOR
   cxBmp = (WhichColor( hdc, lbr.lbColor ) * CX_BMP * 4) +
            (((uiState & BSTATE_CHECK) != 0) |
            ((uiState & BSTATE_STATE) >> 1)   ) * CX_BMP ;
#else
   cxBmp = (((uiState & BSTATE_CHECK) != 0) |
            ((uiState & BSTATE_STATE) >> 1)   ) * CX_BMP ;
#endif

   if(uiAction & (ACTION_TEXT | ACTION_FOCUS))
   {
      cbWndText = GetWindowTextLength( hwnd ) ;
      lpszWndText = GlobalAllocPtr( GHND, cbWndText + 1 ) ;
      GetWindowText( hwnd, lpszWndText, cbWndText + 1 ) ;
   }

   switch( wStyle )
   {
      case BS_RADIOBUTTON:
      case BS_AUTORADIOBUTTON:
         cyBmp = CY_BMP ;
      goto DrawBtn;

      case BS_3STATE:
      case BS_AUTO3STATE:
         if((uiState & BSTATE_CHECK) == 2)
            cyBmp = CY_BMP * 2 ;

         // fall through!!!

      case BS_CHECKBOX:
      case BS_AUTOCHECKBOX:
DrawBtn:
      {
         HDC   hdcMem;


         if(uiAction & ACTION_CHECK)
         {
            if (hdcMem = CreateCompatibleDC(hdc))
            {
               hbmBtn = SelectObject( hdcMem, hbmBtn ) ;
               BitBlt( hdc,
                       rc.left,
                       rc.top + (rc.bottom - rc.top - CY_BMP) / 2, 
                       CX_BMP,
                       CY_BMP,
                       hdcMem,
                       cxBmp,
                       cyBmp, SRCCOPY ) ;
               hbmBtn = SelectObject( hdcMem, hbmBtn ) ;
               DeleteDC( hdcMem ) ;
             }
         }

         SetBkMode( hdc, TRANSPARENT ) ;

         if(uiAction & ACTION_TEXT)
         {
            rc.left = rcClient.left + CX_BMP + 4 ;

            if (hbrDlgBG)
            {
               hbrDlgBG = SelectObject( hdc, hbrDlgBG ) ;

               PatBlt( hdc, rc.left, rc.top,
                       rc.right-rc.left, rc.bottom-rc.top, PATCOPY ) ;
               hbrDlgBG = SelectObject( hdc, hbrDlgBG ) ;
            }

            if(!IsWindowEnabled( hwnd ))
               SetTextColor( hdc, RGBGRAY ) ;

            DP( hWDB, "DrawText" ) ;
            DrawText( hdc, lpszWndText, cbWndText,
                      (LPRECT)&rc, DT_VCENTER | DT_LEFT | DT_SINGLELINE ) ;
         }

         if(uiAction & ACTION_FOCUS)
         {
            int dy;

            rc.left = rcClient.left + CX_BMP + 4 ;
            dy = DrawText( hdc, lpszWndText, cbWndText,
                           (LPRECT)&rc,
                           DT_CALCRECT |
                           DT_VCENTER |
                           DT_LEFT | DT_SINGLELINE ) ;

            rc.top = max( 0, (rcClient.bottom - dy) / 2) ;
            rc.bottom = min( rcClient.bottom, rc.top + dy ) ;
            rc.left = max( 0, rc.left - dxBorder * 2 );
            rc.right = min( rcClient.right, rc.right + (int)dxBorder * 4 ) ;

            DrawFocusRect( hdc, (LPRECT)&rc ) ;
         }
      }
      break;

      case BS_GROUPBOX:
         if((uiAction & ACTION_TEXT) || (uiAction & ACTION_CHECK))
         {
            int      dy;
#ifndef TD
            HBRUSH hbrShadow, hbrHilite ;
#endif

            SetBkMode( hdc, TRANSPARENT ) ;

            if (uiAction & ACTION_ERASETEXT)
                SetTextColor( hdc, lbr.lbColor ) ;

            dy = DrawText( hdc, lpszWndText, cbWndText,
                           (LPRECT)&rc, DT_CALCRECT| DT_LEFT | DT_SINGLELINE ) ;
            

            rcClient.top += dy / 2;
            rcClient.right--;
            rcClient.bottom--;

            lbr.lbColor = SetBkColor( hdc, lbr.lbColor ) ;

#ifdef TD
            tdDraw3DRect( hdc, &rcClient, 1, DRAW3D_CREASE | DRAW3D_IN ) ;
#else
            hbrShadow = CreateSolidBrush( tdGetShadowColor( lbr.lbColor ) ) ;
            hbrHilite = CreateSolidBrush( tdGetHighlightColor( lbr.lbColor ) ) ;
            
            FrameRect(hdc, (LPRECT) &rcClient, hbrShadow);
            OffsetRect(&rcClient, 1, 1);
            FrameRect(hdc, (LPRECT) &rcClient, hbrHilite);
            DeleteObject( hbrShadow ) ;
            DeleteObject( hbrHilite ) ;
#endif


            rc.left += 4;
            rc.right += 4;
            rc.bottom = rc.top + dy;

            if (hbrDlgBG)
            {
               hbrDlgBG = SelectObject( hdc, hbrDlgBG ) ;

               PatBlt( hdc, rc.left, rc.top,
                       rc.right-rc.left, rc.bottom-rc.top, PATCOPY ) ;
               hbrDlgBG = SelectObject( hdc, hbrDlgBG ) ;
            }

            if(!IsWindowEnabled( hwnd ))
                SetTextColor( hdc, RGBGRAY ) ;

            lbr.lbColor = SetBkColor( hdc, lbr.lbColor ) ;
            SetBkMode( hdc, OPAQUE ) ;

            DrawText( hdc, lpszWndText, cbWndText,
                      (LPRECT)&rc, DT_LEFT | DT_SINGLELINE ) ;

         }
      break ;

   }  // switch (wStyle)

   SelectObject(hdc, hbrBG);

   if(hfont != NULL)
      SelectObject(hdc, hfont);

   if (lpszWndText != NULL)
      GlobalFreePtr( lpszWndText ) ;

} /* PaintBtn()  */

#ifdef MUTLICOLOR

/***************************************************************
 * This stuff is used by the multicolor check/radio support.
 ***************************************************************/

COLORREF rgcrColors[] =
{
   RGBWHITE,
   RGBBLACK,
   RGBLTGRAY,
   RGBGRAY,
   RGBLTRED,
   RGBRED,
   RGBYELLOW,
   RGBBROWN,
   RGBLTGREEN,
   RGBGREEN,
   RGBLTCYAN,
   RGBCYAN,
   RGBLTBLUE,
   RGBBLUE,
   RGBLTMAGENTA,
   RGBMAGENTA
} ;

#define _NUM_COLORS (sizeof( rgcrColors ) / sizeof( rgcrColors[0] ))

VOID WINAPI SetupColorTable( HDC hdc )
{
   register UINT i ;

   for (i = 0 ; i < _NUM_COLORS ; i++)
      rgcrColors[i] = GetNearestColor( hdc, rgcrColors[i] ) ;
}


UINT NEAR PASCAL WhichColor( HDC hdc, COLORREF rgb )
{
   register UINT i ;

   rgb = GetNearestColor( hdc, rgb ) ;
    
   for (i = 0 ; i < _NUM_COLORS ; i++)
   {
      if (rgb == rgcrColors[i])
         return i ;
   }

   return 2 ; //RGBLTGRAY
}

#endif // MULTICOLOR

/*****************************************************************
 * Window procedure for the TDUTIL.STATIC custom control
 *****************************************************************/

VOID NEAR PASCAL PaintStatic( HWND hwnd, HDC hdc, DWORD dwStyle ) ;

typedef struct tagSTATICFLAGS
{
   WORD  nAlignment     : 4 ;
   WORD  f3DFrame       : 1 ;
   WORD  fCrease        : 1 ;
   WORD  fDraw3DOut     : 1 ;
   WORD  fTextEdge      : 1 ;
   WORD  fTextShadow    : 1 ;
   WORD  nFrameDepth    : 3 ;
   WORD  nTextDepth     : 3 ;
   WORD  fReserved      : 1 ;
} SFLAGS, *PSFLAGS, FAR *LPSFLAGS ;

/****************************************************************
 *  LRESULT CALLBACK fn3DStatic( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
 *
 *  Description: 
 *
 *
 ****************************************************************/
LRESULT CALLBACK fn3DStatic( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   LRESULT     lRet;
   DWORD       dwStyle ;
   HDC         hdc ;

   switch(uiMsg)
   {
      case WM_CREATE:
         #ifdef DEBUG
         if (sizeof(SFLAGS) != sizeof(WORD))
            DP1( hWDB, "STATICFLAGS structure is not 16 bits!!!!" ) ;
         #endif
      break;

      case WM_ENABLE:
         dwStyle = GetWindowLong( hwnd, GWL_STYLE ) ;
         if(dwStyle & WS_VISIBLE)
         {
            if ((hdc = GetDC(hwnd)) != NULL)
            {
               RECT  rcClient ;

               GetClientRect( hwnd, &rcClient ) ;
               IntersectClipRect( hdc, rcClient.left, rcClient.top,
		                                 rcClient.right, rcClient.bottom);

               PaintStatic( hwnd, hdc, dwStyle ) ;

               ReleaseDC( hwnd, hdc ) ;
            }
            return lRet;
         }
      break;

      case WM_ERASEBKGND:
        /* The control will be erased in StaticPaint(). */
      return MAKELRESULT( TRUE, 0 ) ;

#if 0
      case WM_SETFONT:
      {
         HFONT hFont ;

         hFont = SETFONT( hwnd, (HFONT)wParam ) ;
         if (lParam)
         {
            InvalidateRect( hwnd, NULL, TRUE ) ;
            UpdateWindow( hwnd ) ;
         }

         return MAKELRESULT( hFont, 0 ) ;
      }
      break ;

      case WM_GETFONT:
      return MAKELRESULT( GETFONT( hwnd ), 0 ) ;
#endif
      case WM_PAINT:
      {
         PAINTSTRUCT ps;

         dwStyle = GetWindowLong( hwnd, GWL_STYLE ) ;

         BeginPaint( hwnd, &ps ) ;

         PaintStatic( hwnd, ps.hdc, dwStyle ) ;

         EndPaint( hwnd, &ps ) ;
      }
      return 0L;

   }

   return DefWindowProc( hwnd, uiMsg, wParam, lParam ) ;

} /* fn3DStatic()  */


/****************************************************************
 *  VOID NEAR PASCAL PaintStatic( HWND hwnd, HDC hdc, DWORD )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL PaintStatic( HWND hwnd, HDC hdc, DWORD dwStyle )
{
   RECT        rc;
   RECT        rcClient;
   HFONT       hfont;
   LOGBRUSH    lbr ;
   HBRUSH      hbrDlgBG;
   HBRUSH      hbrBG;
   HWND        hwndParent;
   SFLAGS      sfStyle ;

   *(LPWORD)(&sfStyle) = LOWORD( dwStyle ) ;

   DP1( hWDB, "PaintStatic: %04X", LOWORD( dwStyle ) ) ;

   hwndParent = GetParent( hwnd ) ;

   GetClientRect( hwnd, (LPRECT)&rcClient ) ;
   rc = rcClient ;

   if((hfont = GetWindowFont( hwnd )) != NULL)
      hfont = SelectObject( hdc, hfont ) ;

   #ifdef WIN32
   hbrDlgBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLORDLG,
                                (WPARAM)hdc,
                                (LPARAM)hwnd ) ;
   #else
   hbrBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLOR,
                                (WPARAM)hdc,
                                MAKELPARAM( hwnd, CTLCOLOR_DLG ) ) ;
   #endif

   GetObject( hbrDlgBG, sizeof(LOGBRUSH), &lbr ) ;

   #ifdef WIN32
   hbrBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLORSTATIC,
                                (WPARAM)hdc,
                                (LPARAM)hwnd ) ;
   #else
   hbrBG = (HBRUSH)SendMessage( hwndParent, WM_CTLCOLOR,
                                (WPARAM)hdc,
                                MAKELPARAM( hwnd, CTLCOLOR_STATIC ) ) ;
   #endif

   hbrBG = SelectObject( hdc, hbrBG ) ;


   IntersectClipRect(hdc, rc.left, rc.top, rc.right, rc.bottom);

   switch( sfStyle.nAlignment )
   {
      case TDSS_LEFT:
      case TDSS_CENTER:
      case TDSS_RIGHT:
      case TDSS_LEFTNOWORDWRAP:
      {
         UINT  uiEto = ETO_OPAQUE ;
         UINT  cb ;

         tdExtTextOut3D( hdc, 0, 0, ETO_OPAQUE,
              &rc, NULL, 0, NULL, 1 ) ;

         InflateRect( &rc, -1, -1 ) ;

         UnrealizeObject( hbrBG ) ;
         hbrBG = SelectObject( hdc, hbrBG ) ;

         if ((cb = GetWindowTextLength( hwnd )) != 0)
         {
            LPSTR lpszWndText ;

            if (lpszWndText = GlobalAllocPtr( GHND, cb + 1 ))
            {
               UINT     uiDT ;
               COLORREF rgbText; 

               GetWindowText( hwnd, lpszWndText, cb+1 ) ;

               #if 0
               if ((wStyle & ~TDSS_NOPREFIX) == TDSS_LEFTNOWORDWRAP)
                  uiDT = DT_NOCLIP | DT_EXPANDTABS;
               else
               {
                  uiDT = DT_NOCLIP | DT_WORDBREAK | DT_EXPANDTABS;
                  uiDT |= ((wStyle & ~TDSS_NOPREFIX) - TDSS_LEFT);
               }

               if (wStyle & TDSS_NOPREFIX)
                  uiDT |= DT_NOPREFIX;

               #endif

               if ((!IsWindowEnabled( hwnd ) && GetSysColor( COLOR_GRAYTEXT )))
                  rgbText = SetTextColor(hdc, GetSysColor( COLOR_GRAYTEXT ) ) ;

               DrawText( hdc, lpszWndText, -1, &rc, uiDT ) ;

               if ((!IsWindowEnabled( hwnd ) && GetSysColor( COLOR_GRAYTEXT )))
                  SetTextColor( hdc, rgbText ) ;

               GlobalFreePtr( lpszWndText ) ;
            }
         }

         hbrBG = SelectObject( hdc, hbrBG ) ;
      }
      break;

   }

   SelectObject(hdc, hbrBG);

   if(hfont != NULL)
      SelectObject(hdc, hfont);

} /* PaintStatic()  */

/************************************************************************
 * End of File: 3dctrl.c
 ***********************************************************************/

