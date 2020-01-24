
/************************************************************************
 *
 *     Project:  Windows Sample Applications
 *
 *      Module:  status.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (charlkin)
 *
 *     Remarks:
 *       This module contains all functions used to display a 3d looking
 *       status line (window) within a parent window. 
 *
 *   Revisions:  
 *     01.00.000  9/25/91 cek   Made into a sample
 *
 ************************************************************************/

#define WINDLL
#include <windows.h>
/*#include <wdb.h>*/
/*#include "status.h" */
#include "cmndll.h"

/*************************************************************************
 * Imported variables                                      
 *************************************************************************/
extern HANDLE hInst ;

/*************************************************************************
 * Local Defines                                      
 *************************************************************************/

#ifndef WIN31
#define COLOR_BTNHIGHLIGHT        20      // windows 3.1 supports this
                                          // new color.
#endif

#define MAX_STATLEN  128

//
// The handle that is passed to CreateMsgWindow is stored in the 
// cbWndExtra part of the child window.  This flag is used to 
// retrieve that data:
//
#define NUM_EXTRABYTES 3*sizeof(WORD)

#define GWW_GHSTATUS   0 
#define GWW_FLAGS      2
#define GWW_HFONT      4

#define LINEWIDTH      1

#define SW_ERR         (LONG)-1

/*************************************************************************
 * Macros                                      
 *************************************************************************/
#define ISDIGIT(c)  ((c) >= '0' && (c) <= '9')

#define GETHINST(hwnd)    GetWindowWord( hwnd, GWW_HINSTANCE )

#define GETSTYLE(hWnd)    LOWORD( GetWindowLong( hWnd, GWL_STYLE ) )
#define GETSTATE(hWnd)    GetWindowWord( hWnd, GWW_STATE )
#define GETFLAGS(hWnd)    GetWindowWord( hWnd, GWW_FLAGS )
#define GETGHSTATUS(hWnd) GetWindowWord( hWnd, GWW_GHSTATUS )
#define GETHFONT(hWnd)    GetWindowWord( hWnd, GWW_HFONT ) 
#define GETID(hWnd)       GetWindowWord( hWnd, GWW_ID ) 

/************************************************************************
 * Local data structures
 ************************************************************************/

//
// MSG/STATLINE data structures
//
// Structure for msg line
typedef struct tagSTATUS
{
   WORD     wNumStats ;             // Number of status boxes
   RECT     rect ;                  // coordinates of MsgLineWnd
   DWORD    rgbColor ;              // Text color
   WORD     wJustify ;
   BOOL     fBorder ;
   BOOL     fTouched ;              // TRUE if CurMSg has changed since last
   BOOL     fNoMsg ;
                                    // WM_PAINT
} STATUS, *PSTATUS, FAR *LPSTATUS ;

// structure for status boxes
typedef struct tagSTATBOX
{
   RECT     rect ;                  // coordinates of Stat Box
   DWORD    rgbColor ;              // Color of text
   WORD     wWidth ;                // Width of box in pixels
   char     szCurStat[MAX_STATLEN] ;             // Current status
   WORD     wJustify ;
   BOOL     fBorder  ;
   BOOL     fTouched ;          // TRUE if CurStat has changed since
                                    // last WM_PAINT
} STATBOX, *PSTATBOX, FAR *LPSTATBOX ;


/************************************************************************
 * Internal APIs and data structures for MSG/STATLINES
 ************************************************************************/
static BOOL IsRectInRect( LPRECT lpR1, LPRECT lpR2 ) ;

static BOOL StatusCreate( HWND hWnd, LPCREATESTRUCT lp ) ;

static void StatusInvalidate( HWND hWnd, WORD wID ) ;

static void StatusPaint( HWND hWnd ) ;

static BOOL CheckClick( HWND, LONG, LPWORD ) ;

LONG FAR PASCAL
fnStatus( HWND hWnd, int iMessage, WORD wParam, LONG lParam ) ;

/*************************************************************************
 * Local Variables                                     
 *************************************************************************/
WORD  wExtraSpace = 1 ;

/*************************************************************************
 * window class names
 *************************************************************************/
#define SZ_STATUSCLASSNAME "STATUS"
char szStatusClassName[] = SZ_STATUSCLASSNAME ;


/*************************************************************************
 *  BOOL FAR PASCAL StatusInit( HANDLE hPrev, HANDLE hInstance )
 *
 *  Description: 
 *
 *    Registers the window class.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL StatusInit( HANDLE hPrev, HANDLE hInstance )
{
   WNDCLASS wc ;

   if (!hPrev)
   {
      // 
      // Register the Child Windows class
      //
      wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW | CS_GLOBALCLASS ;

      wc.lpfnWndProc = fnStatus ;


      wc.cbClsExtra = 0 ;
      //
      // cbWndExtra holds a handle to the STATUS structure
      //
      wc.cbWndExtra = NUM_EXTRABYTES ; 
                          

      wc.hInstance = hInstance ;
      wc.hIcon = NULL ;
      wc.hCursor = LoadCursor( NULL, IDC_ARROW ) ;
      wc.hbrBackground = COLOR_BTNFACE + 1 ;
      wc.lpszMenuName = NULL ;

      wc.lpszClassName = szStatusClassName ;

      if (RegisterClass( &wc ))
      /*
      * always return true.  Register class will return failure if
      * the class was already registerd, but that's no big deal because
      * we have used a very unique class name (haven't we?).
      */
         return TRUE ;
   }
   return TRUE ;
}/* StatusInit() */

/************************************************************************
 *  LONG FAR PASCAL
 *  fnStatus( HWND hWnd, int iMessage, WORD wParam, LONG lParam ) ;
 *
 *  Description:
 *    Default window proc for message line window.
 *
 *  Comments:
 *
 ************************************************************************/
LONG FAR PASCAL
fnStatus( HWND hWnd, int iMessage, WORD wParam, LONG lParam )
{
   GLOBALHANDLE   ghStatus ;
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   LONG           lRetVal = 0L ;


   switch (iMessage)
   {
      case WM_CREATE :
         if (!StatusCreate( hWnd, (LPCREATESTRUCT)lParam ))
            return SW_ERR ;
      break ;
           
      case ST_GETHEIGHT:
      {
         HDC hDC ;
         HFONT hFont ;
         TEXTMETRIC tm ;

         if (hDC = GetDC (hWnd))
         {
            hFont = GETHFONT( hWnd ) ;

            SelectObject( hDC, hFont ) ;

            GetTextMetrics( hDC, &tm ) ;

            ReleaseDC (hWnd, hDC) ;
            return tm.tmHeight + tm.tmExternalLeading +
                   (LINEWIDTH * 6) + wExtraSpace ;
         }
         else
         {
            return 0L ;
         }
      }
      break ;

      case ST_GETSTATUSWIDTH:
         return StatusGetWidth( hWnd, (LPSTR)lParam ) ;
      break ;

      case ST_SETSTATBOXSIZE:
         ghStatus = GETGHSTATUS( hWnd ) ;
         if (ghStatus)
         {
            lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
            lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

            if (wParam < lpStatus->wNumStats)
            {
               lpStatBox[wParam].wWidth = LOWORD( lParam ) ;

               StatusInvalidate( hWnd, 0xFFFF ) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
            }
            else
               lRetVal = SW_ERR ;

            GlobalUnlock( ghStatus ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETCOLOR:
         ghStatus = GETGHSTATUS( hWnd ) ;
         if (ghStatus)
         {
            lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
            if ((int)wParam == -1)
            {
               lpStatus->fTouched = TRUE ;
               lpStatus->rgbColor = (COLORREF)lParam ;
               StatusInvalidate( hWnd, 0 ) ;
            }
            else
            {
               if (wParam < lpStatus->wNumStats)
               {
                  lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
                  lpStatBox[wParam].fTouched = TRUE ;
                  lpStatBox[wParam].rgbColor = (COLORREF)lParam  ;
                  StatusInvalidate( hWnd, wParam + 1 ) ;
               }
               else
                  lRetVal = SW_ERR ;
            }
            GlobalUnlock( ghStatus ) ;
         }
         else
            return SW_ERR ;
      break ;
         

      case WM_SETTEXT :
         lRetVal = DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
         ghStatus = GETGHSTATUS( hWnd ) ;
         if (ghStatus)
         {
            lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;

            if (!lpStatus->fNoMsg)
            {
               lpStatus->fTouched = TRUE ;

               StatusInvalidate( hWnd, 0 ) ;
            }
            GlobalUnlock( ghStatus ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETSTATBOX :
         ghStatus = GETGHSTATUS( hWnd ) ;
         if (ghStatus)
         {
            lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
            lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

            if ( wParam < lpStatus->wNumStats &&
                 lstrlen( (LPSTR)lParam ) < MAX_STATLEN)
               {
                  lpStatBox[wParam].fTouched = TRUE ;
                  lstrcpy( lpStatBox[wParam].szCurStat, (LPSTR)lParam ) ;
               }
               else
                  lRetVal = SW_ERR ;

            GlobalUnlock( ghStatus ) ;
            StatusInvalidate( hWnd, wParam + 1 ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETBORDER:                  // wParam == -1 is message line
         ghStatus = GETGHSTATUS( hWnd ) ;
         if (ghStatus)
         {
            lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
            if ((int)wParam == -1)
               lpStatus->fBorder = (BOOL)LOWORD( lParam ) ;
            else
            {
               if (wParam < lpStatus->wNumStats)
               {
                  lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
                  lpStatBox[wParam].fBorder = (BOOL)LOWORD( lParam ) ;
               }
               else
                  lRetVal = SW_ERR ;
            }
            GlobalUnlock( ghStatus ) ;
            StatusInvalidate( hWnd, 0xFFFF ) ;
            InvalidateRect( hWnd, NULL, TRUE ) ;
            UpdateWindow( hWnd ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETJUSTIFY:
         ghStatus = GETGHSTATUS( hWnd ) ;
         if (ghStatus)
         {
            lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
            if ((int)wParam == -1)
            {
               lpStatus->wJustify = LOWORD( lParam ) ;
               lpStatus->fTouched = TRUE ;
               StatusInvalidate( hWnd, 0 ) ;
            }
            else
            {
               if (wParam < lpStatus->wNumStats)
               {
                  lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
                  lpStatBox[wParam].wJustify = LOWORD( lParam ) ;
                  lpStatBox[wParam].fTouched = TRUE ;
               }
               else
                  lRetVal = SW_ERR ;
               StatusInvalidate( hWnd, wParam + 1 ) ;
            }
            GlobalUnlock( ghStatus ) ;
            UpdateWindow( hWnd ) ;
         }
         else
            return SW_ERR ;
      break ;

      case WM_MBUTTONDBLCLK:
      case WM_RBUTTONDBLCLK:
      case WM_LBUTTONDBLCLK:
      {
         WORD  wID ;

         if (!IsWindowEnabled( hWnd ))
            return 0L ;

         if (CheckClick( hWnd, lParam, &wID ))
         {
            //
            // Post a message to the parent.  We used to send
            // wParam as the LOWORD of lParam instead of hWnd.  but
            // as of Win 3.1 we discovered that this is frowned upon
            // and 3.1 debug tells us so.  Since this is a sample,
            // and we really shouldn't do anything illegal we will comply.
            // 
            // Thus you want to find out the key states on the double
            // click, add a new ST_ message like ST_GETLASTCOMMANDINFO
            // or some such.
            //
            PostMessage( GetParent( hWnd ), WM_COMMAND, GETID( hWnd ),
                         MAKELONG( hWnd, wID ) ) ;
         }
      }
      return 0L ;
         
      case WM_ENABLE:
         InvalidateRect( hWnd, NULL, TRUE ) ;
      break ;

      case WM_MOVE:
         InvalidateRect( hWnd, NULL, TRUE ) ;
      break ;

      case WM_SIZE:
         StatusInvalidate( hWnd, 0xFFFF ) ;
      break ;

      case WM_PAINT :
         StatusPaint( hWnd ) ;
      break ;

      case WM_GETFONT:
         return GETHFONT( hWnd ) ;

      case WM_SETFONT:
      {
         HFONT hFontOld = GETHFONT( hWnd ) ;

         if (wParam)
            SetWindowWord( hWnd, GWW_HFONT, wParam ) ;
         else
            SetWindowWord( hWnd, GWW_HFONT, GetStockObject( SYSTEM_FONT ) ) ;

         if (lParam)
         {
            StatusInvalidate( hWnd, 0xFFFF ) ;
            InvalidateRect( hWnd, NULL, TRUE ) ;
         }

         return hFontOld ;
      }
      break ;

      case WM_DESTROY :

         //
         // Get the handle from the Window extra bytes
         //
         ghStatus = (GLOBALHANDLE)GETGHSTATUS( hWnd ) ;

         // free the memory
         //
         if (ghStatus)
            GlobalFree( ghStatus ) ;
      break ;


      default :
         return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;

   } // switch (iMessage)

   return lRetVal ;
}
/* fnStatus() */
/*************************************************************************
 *  WORD FAR PASCAL StatusGetWidth( HWND hMsgWnd, LPSTR lpszText )
 *
 *  Description: 
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
WORD FAR PASCAL StatusGetWidth( HWND hMsgWnd, LPSTR lpszText )
{

   if (!hMsgWnd)
   {
      return FALSE ;
   }

   if (lstrlen( lpszText ))
   {
      GLOBALHANDLE ghStatus ;
      LPSTATUS    lpStatus ;
      HDC          hDC ;
      HANDLE       hFont ;
      RECT         rect ;
      // 
      // get the global handle from the window extra bytes
      //
      ghStatus = (GLOBALHANDLE)GETGHSTATUS( hMsgWnd ) ;

      if (ghStatus)
         lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
      else
      {
         return 0 ;
      }

      if (hDC = GetDC (hMsgWnd))
      {
         hFont = GETHFONT( hMsgWnd ) ;

         SelectObject( hDC, hFont ) ;

         // Get Width
         rect.right = 0 ;
         rect.bottom = 0 ;
         DrawText( hDC, lpszText, -1, &rect,
                        DT_CALCRECT |
                        DT_NOPREFIX |
                        DT_SINGLELINE |
                        DT_TOP ) ;

         ReleaseDC (hMsgWnd, hDC) ;
      }
      else
      {
         GlobalUnlock( ghStatus ) ;
         return LINEWIDTH * 4 ;
      }

      GlobalUnlock( ghStatus ) ;
      return (rect.right - rect.left) + (LINEWIDTH * 4) ;
   }
   return LINEWIDTH * 4 ;
}/* StatusGetWidth() */


/************************************************************************
 * Internal APIs and data structures for MSG/STATLINES
 ************************************************************************/

/*************************************************************************
 *  static BOOL StatusCreate( HWND hWnd, LPCREATESTRUCT lpCS )
 *
 *  Description:
 *    Handles the WM_CREATE message.
 *
 *  Returns FALSE if something went wrong.
 *
 *  Comments:
 *
 *************************************************************************/
static BOOL StatusCreate( HWND hWnd, LPCREATESTRUCT lpCS )
{
   GLOBALHANDLE   ghStatus ;
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   WORD           i ;
   WORD           wNumStats ;

   if (lpCS->lpCreateParams)
      wNumStats = *(LPWORD)lpCS->lpCreateParams ;
   else
      wNumStats = 0 ;

   // 
   // Allocate the memory for the stat boxes and message line
   //
   if (!(ghStatus = GlobalAlloc( GHND,
                                    sizeof( STATUS ) +
                                    (sizeof( STATBOX ) * wNumStats) )))
   {
      return FALSE ;
   }

   //
   // Get structure and do some setting up
   //
   lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
   lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

   lpStatus->wNumStats = wNumStats ;

   lpStatus->fTouched = TRUE ;
   lpStatus->fBorder = FALSE ;
   lpStatus->wJustify = DT_LEFT ;
   lpStatus->rgbColor = RGB(0,0,0) ;
   if (lpCS->lpszName)
      lpStatus->fNoMsg = FALSE ;
   else
      lpStatus->fNoMsg = TRUE ;


   for (i = 0 ; i < lpStatus->wNumStats ; i++)
   {
      lpStatBox[i].fTouched = TRUE ;
      lpStatBox[i].fBorder = TRUE ;
      lpStatBox[i].wJustify = DT_CENTER ;
      lpStatBox[i].rgbColor = RGB(0,0,0) ;
   }

   GlobalUnlock( ghStatus ) ;

   //
   // Store the handle in the window extra bytes at GWW_GHSTATUS
   //
   SetWindowWord( hWnd, GWW_GHSTATUS, (WORD)ghStatus ) ;

   SetWindowWord( hWnd, GWW_HFONT, GetStockObject( SYSTEM_FONT ) ) ;

   SetWindowWord( hWnd, GWW_FLAGS, (WORD)lpCS->style & 0xFF00 ) ;

   return TRUE ;
}
/* StatusCreate() */

/*************************************************************************
 *  static void StatusInvalidate( HWND hWnd, WORD wID )
 *
 *  Description:
 *    Invalidates the specified element of the status line.  The message
 *    part is 0, and the first stat box is 1.  Setting wID to 0xffff
 *    invalidates the entire status line, forcing a complete redraw and
 *    and re-calculation.
 *
 *  Type/Parameter
 *          Description
 *    HWND hWNd
 *          Window handle.
 *
 *    WORD  wID
 *          ID of the element to be invalidated, 0 = message, 1 = 1st stat
 *          box, 2 = 2nd stat box, etc... 0xFFFF = recalc and invalidate all.
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
static void StatusInvalidate( HWND hWnd, WORD wID )
{
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   WORD           i ;
   GLOBALHANDLE   ghStatus ;
   WORD           wWndFlags ;
   RECT           rc ;

   if (hWnd == 0)
      return ;

   ghStatus= GETGHSTATUS( hWnd ) ;
   wWndFlags = GETFLAGS( hWnd ) ;

   if (ghStatus == 0)
      return ;

   lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
   lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

   if (wID == 0xFFFF)
   {
      WORD        wHeight ;
      RECT        rc ;
      HDC         hDC ;
      HANDLE      hFont ;
      TEXTMETRIC  tm ;

      //
      // Figure out where the window should be
      //
      // Calculate all rects and store them
      if (hDC = GetDC (hWnd))
      {
         hFont = GETHFONT( hWnd ) ;

         SelectObject( hDC, hFont ) ;

         GetTextMetrics( hDC, &tm ) ;

         ReleaseDC (hWnd, hDC) ;
      }
      else
      {
         GlobalUnlock( ghStatus ) ;
         return ;
      }

      wHeight = tm.tmHeight + tm.tmExternalLeading + (LINEWIDTH * 6)+wExtraSpace ;

      //
      // Set the rects for all boxes
      //
      // start with MsgLine
      //
      GetClientRect( hWnd, &rc ) ;

      //
      // Adjust for black lines
      //
      rc.top++ ;
      rc.bottom-- ;

      // Move in from the edges
      rc.left     += LINEWIDTH * 6 ;
      rc.top      += LINEWIDTH * 2 ;
      rc.bottom   -= LINEWIDTH * 2 ;
      rc.right    -= LINEWIDTH * 6 ;

      //
      // Depending on whether there is a message, we handle things a bit
      // differently.  If there is a message then we make the stat boxes
      // flush right, calculating from right to left.
      // If, however, there is going to be no message, then we must make
      // the statboxes flush left, calculating from left to right...
      //

      if (lpStatus->fNoMsg)
      {
         // Calculate from left to right
         //
         for (i = 0 ; i < lpStatus->wNumStats ; i++)
         {
            lpStatBox[i].rect.left = rc.left ;
            lpStatBox[i].rect.top = rc.top ;
            lpStatBox[i].rect.bottom = rc.bottom ;
            lpStatBox[i].rect.right = rc.left + lpStatBox[i].wWidth +
                                      2 * LINEWIDTH ;

            rc.left = lpStatBox[i].rect.right + 2 * LINEWIDTH ;
            lpStatBox[i].fTouched = TRUE ;
         }
      }
      else
      {
         // Calculate from right to left.
         //

         for (i = lpStatus->wNumStats ; i > 0 ; i--)
         {
            lpStatBox[i-1].rect.right = rc.right ;

            //
            // left - 2*lw for border.
            //
            lpStatBox[i-1].rect.left = rc.right -
                                       lpStatBox[i-1].wWidth -
                                       2 * LINEWIDTH ; 

            lpStatBox[i-1].rect.top = rc.top ;


            lpStatBox[i-1].rect.bottom = rc.bottom ;

            rc.right = lpStatBox[i-1].rect.left - (LINEWIDTH * 2) ;

            lpStatBox[i-1].fTouched = TRUE ;
         }

         if (lpStatus->wNumStats)
            lpStatus->rect.right = rc.right - (LINEWIDTH * 9) ;
         else
            lpStatus->rect.right = rc.right - (LINEWIDTH * 4) ;

         lpStatus->rect.top = rc.top ;

         lpStatus->rect.bottom = rc.bottom ;

         lpStatus->rect.left = rc.left ;

         lpStatus->fTouched = TRUE ;
      }

      //
      // Invalidate the whole window
      //
      // InvalidateRect( hWnd, NULL, TRUE ) ;

   } // if INVALIDATEALL
   else
   {
      //
      // Invalidate the ID passed in
      // 
      if ((!lpStatus->fNoMsg) && wID == 0 && lpStatus->fTouched)
      {
         rc.top    = lpStatus->rect.top    + LINEWIDTH ;
         rc.left   = lpStatus->rect.left   + LINEWIDTH ;
         rc.bottom = lpStatus->rect.bottom - LINEWIDTH ;
         rc.right  = lpStatus->rect.right  - LINEWIDTH ;

         InvalidateRect( hWnd, (LPRECT)&rc, TRUE ) ;
      }

      if (lpStatus->wNumStats && wID > 0 && lpStatBox[wID-1].fTouched)
      {
         rc.top    = lpStatBox[wID-1].rect.top     + LINEWIDTH ;
         rc.left   = lpStatBox[wID-1].rect.left    + LINEWIDTH ;
         rc.bottom = lpStatBox[wID-1].rect.bottom  - LINEWIDTH ;
         rc.right  = lpStatBox[wID-1].rect.right   - LINEWIDTH ;

         InvalidateRect( hWnd, (LPRECT)&rc, TRUE ) ;
      }
   }

   // Unlock the data
   GlobalUnlock( ghStatus ) ;
}
/* StatusInvalidate() */

/*************************************************************************
 *  static void StatusPaint( HWND hWnd )
 *
 *  Description: 
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
static void StatusPaint( HWND hWnd )
{
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   WORD           i ;

   PAINTSTRUCT    ps ;
   TEXTMETRIC     tm ;
   HANDLE         hFont ;
   GLOBALHANDLE   ghStatus  ;
   RECT           r1 ;
   RECT           r2 ;
   WORD           wFlags ;
   COLORREF rgbBtnFace ;

   GetClientRect( hWnd, &r1 ) ;

   ghStatus = GETGHSTATUS( hWnd ) ;
   if (ghStatus == 0)
      return  ;

   wFlags = GETFLAGS( hWnd ) ;

   if (!BeginPaint( hWnd, &ps ))
      return ;

   if (!RectVisible( ps.hdc, &r1 ))
   {
      EndPaint( hWnd, &ps ) ;
      return ;
   }

   rgbBtnFace = GetSysColor( COLOR_BTNFACE ) ;

   lpStatus  = (LPSTATUS)GlobalLock( ghStatus ) ;
   lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

   // Select font
   hFont = GETHFONT( hWnd ) ;

   SelectObject( ps.hdc, hFont ) ;
   SetBkColor( ps.hdc, GetNearestColor( ps.hdc, rgbBtnFace ) ) ;
   SetBkMode( ps.hdc, TRANSPARENT ) ;

   GetTextMetrics( ps.hdc, &tm ) ;
            
   // 
   // See if the entire area needs to be painted
   //
   if (IsRectInRect( &r1, &ps.rcPaint ))
   {
      HPEN  hPen ;
      hPen = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_WINDOWFRAME ) ) ;

      if (lpStatus->wNumStats)
      {
         int n ;

         //
         // Draw the border
         // 

         hPen = SelectObject( ps.hdc, hPen ) ;
         MoveTo( ps.hdc, r1.left, r1.top ) ;
         LineTo( ps.hdc, r1.right, r1.top ) ;
         MoveTo( ps.hdc, r1.left, r1.bottom-1 ) ;
         LineTo( ps.hdc, r1.right, r1.bottom-1 ) ;

         hPen = SelectObject( ps.hdc, hPen ) ;

         r1.top++ ;
         r1.bottom-- ;

         //
         // Draw the main
         //
         if (lpStatus->fNoMsg)
         {
            // around stat boxes
            Draw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_OUT ) ;
         }
         else
         {
            n = lpStatus->rect.right + (LINEWIDTH * 5) ;

            r2.top = r1.top ;
            r2.bottom = r1.bottom ;

            r2.left = n + 1 ;
            r2.right =  r1.right ;

            r1.right = n ;

            // msgline
            Draw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_OUT ) ;

            // around stat boxes
            Draw3DRect( ps.hdc, &r2, LINEWIDTH, DRAW3D_OUT ) ;
         }



         //
         // Message
         //
         if ((!lpStatus->fNoMsg) && lpStatus->fBorder)
         {
            CopyRect( &r2, &lpStatus->rect ) ;
            Draw3DRect( ps.hdc, &r2, LINEWIDTH, DRAW3D_IN ) ;
         }

         //
         // stat boxes
         //
         for (i = 0 ; i < lpStatus->wNumStats ; i++)
         {
            if (lpStatBox[i].fBorder)
            {
               CopyRect( &r2, &lpStatBox[i].rect ) ;
               Draw3DRect( ps.hdc, &r2, LINEWIDTH, DRAW3D_IN ) ;
            }
         }

         if (!lpStatus->fNoMsg)
         {
            hPen = SelectObject( ps.hdc, hPen ) ;
            MoveTo( ps.hdc, n, 0 ) ;
            LineTo( ps.hdc, n, r1.bottom+1 ) ;
            hPen = SelectObject( ps.hdc, hPen ) ;
         }

      }
      else
      {
         hPen = SelectObject( ps.hdc, hPen ) ;
         MoveTo( ps.hdc, r1.left, r1.top ) ;
         LineTo( ps.hdc, r1.right, r1.top ) ;
         MoveTo( ps.hdc, r1.left, r1.bottom-1 ) ;
         LineTo( ps.hdc, r1.right, r1.bottom-1 ) ;
         hPen = SelectObject( ps.hdc, hPen ) ;

         r1.top++ ;
         r1.bottom-- ;

         //
         // msgline only
         //
         Draw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_OUT ) ;

         if (lpStatus->fBorder)
         {
            CopyRect( &r1, &lpStatus->rect ) ;
            Draw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_IN ) ;
         }
      }

      DeleteObject( hPen ) ;
   }

   //
   // Draw text if needed
   //
   r1.top      = lpStatus->rect.top + wExtraSpace - 1 ;
   r1.left     = lpStatus->rect.left   + (LINEWIDTH * 3) ;
   r1.bottom   = lpStatus->rect.bottom - LINEWIDTH ;
   r1.right    = lpStatus->rect.right  - (LINEWIDTH * 3) ;

   if ((!lpStatus->fNoMsg) &&
       (lpStatus->fTouched || IsRectInRect( &r1, &ps.rcPaint )))
   {
      char szText [MAX_STATLEN+1] ;
      SetTextColor( ps.hdc, lpStatus->rgbColor ) ;

      // 
      // Clipping is on 
      //
      GetWindowText( hWnd, szText, MAX_STATLEN ) ;

      DrawText( ps.hdc, szText, -1, &r1,
         DT_WORDBREAK | DT_NOPREFIX | lpStatus->wJustify ) ;

//         DT_SINGLELINE  | DT_TOP | DT_NOPREFIX | lpStatus->wJustify ) ;

      lpStatus->fTouched = FALSE ;
   }

   for (i = 0 ; i < lpStatus->wNumStats ; i++)
   {
      r1.top    = lpStatBox[i].rect.top + wExtraSpace - 1 ;
      r1.left   = lpStatBox[i].rect.left   + (LINEWIDTH*3) ;
      r1.bottom = lpStatBox[i].rect.bottom - LINEWIDTH ;
      r1.right  = lpStatBox[i].rect.right  - (LINEWIDTH*3) ;

      if (lpStatBox[i].fTouched ||
            IsRectInRect( &r1, &ps.rcPaint ))
      {
         SetTextColor( ps.hdc, lpStatBox[i].rgbColor ) ;

         //
         // Draw each stat box 
         //
         DrawText( ps.hdc, lpStatBox[i].szCurStat, -1, &r1,
            DT_SINGLELINE | DT_NOCLIP |
            DT_TOP | DT_NOPREFIX | lpStatBox[i].wJustify ) ;

         lpStatBox[i].fTouched = FALSE ;
      }
   }

   GlobalUnlock( ghStatus ) ;

   EndPaint( hWnd, &ps ) ;

}/* StatusPaint() */

/*************************************************************************
 *  static BOOL CheckClick( HWND, LONG, LPWORD ) ;
 *
 *  Description: 
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
static BOOL CheckClick( HWND hWnd, LONG lParam, LPWORD lpwID )
{
   GLOBALHANDLE ghStatus ;
   LPSTATUS     lpStatus ;
   LPSTATBOX    lpStatBox ;
   WORD         n ;
   int          x, y ;

   if (!hWnd)
   {
      return FALSE ;
   }

   // 
   // get the global handle from the window extra bytes
   //
   ghStatus = (GLOBALHANDLE)GETGHSTATUS( hWnd ) ;

   if (ghStatus)
   {
      lpStatus = (LPSTATUS)GlobalLock( ghStatus ) ;
      lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
   }
   else
      return FALSE ;

   x = LOWORD( lParam ) ;
   y = HIWORD( lParam ) ;

   if (x >= lpStatus->rect.left &&
       x <= lpStatus->rect.right &&
       y >= lpStatus->rect.top &&
       y <= lpStatus->rect.bottom)
   {
      GlobalUnlock( ghStatus ) ;
      *lpwID = 0xFFFF ;
      return TRUE ;
   }

   for (n = 0 ; n < lpStatus->wNumStats ; n++)
      if (x >= lpStatBox[n].rect.left &&
          x <= lpStatBox[n].rect.right &&
          y >= lpStatBox[n].rect.top &&
          y <= lpStatBox[n].rect.bottom)
      {
         GlobalUnlock( ghStatus ) ;
         *lpwID = n ;
         return TRUE ;
      }

   GlobalUnlock( ghStatus ) ;
   return FALSE ;
   
}/* CheckClick() */



/************************************************************************
 *  static BOOL IsRectInRect( LPRECT lpR1, LPRECT lpR2 ) ;
 *
 *  Description:
 *    Tests if lpR2 is in lpR1.
 *
 *
 *  Comments:
 *
 ************************************************************************/
static BOOL IsRectInRect( LPRECT lpR1, LPRECT lpR2 ) 
{
   HRGN hrgn ;
   BOOL bInRect ;

   if (hrgn = CreateRectRgn( lpR1->left, lpR1->top, lpR1->right, lpR1->bottom ))
   {
      bInRect = RectInRegion( hrgn, lpR2 ) ;

      DeleteObject( hrgn ) ;
   }
   else
      bInRect = TRUE ;

   return bInRect ;

}/* IsRectInRect() */

/************************************************************************
 * End of File: status.c
 ************************************************************************/

