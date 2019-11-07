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
 *     02.00.000  1/3/92  cek   Make compatible with Win32
 *
 ************************************************************************/

#ifdef STANDALONE
   #define _WINDLL
   #define STRICT

   #include <windows.h>
   #include <windowsx.h>

   #ifdef WIN32
      #include <strict.h>
      #include <malloc.h>
   #endif

   #include <wdb.h>
   #include "status.h"
   #include <3dutil.h>

#else
   #include "shoplibi.h"
   #define IN_A_DLL
#endif

/*************************************************************************
 * Local Defines                                      
 *************************************************************************/

#define MAX_STATLEN  128

//
// The handle that is passed to CreateMsgWindow is stored in the 
// cbWndExtra part of the child window.  This flag is used to 
// retrieve that data:
//
#define NUM_EXTRABYTES (sizeof(LPVOID) + sizeof(LONG) + sizeof(HFONT))

#define GWL_LPSTATUS   0 
#define GWL_FLAGS      (GWL_LPSTATUS + sizeof(LPVOID))
#define GWW_HFONT      (GWL_FLAGS + sizeof(LONG))

#define LINEWIDTH      ((int)1)

#define SW_ERR         ((LRESULT)(-1))

/*************************************************************************
 * Macros                                      
 *************************************************************************/
#define ISDIGIT(c)  ((c) >= '0' && (c) <= '9')

/*
 * These macros all act on window longs.  They will all work in both
 * Win16 and Win32.
 */
#define GETSTYLE(hWnd)        LOWORD( GetWindowLong( hWnd, GWL_STYLE ) )
#define GETFLAGS(hWnd)        GetWindowLong( hWnd, GWL_FLAGS )
#define SETFLAGS(hWnd,l)      SetWindowLong( hWnd, GWL_FLAGS, l )

#define GETLPSTATUS(hWnd)     (LPSTATUS)GetWindowLong( hWnd, GWL_LPSTATUS )
#define SETLPSTATUS(hWnd,lp)  (LPSTATUS)SetWindowLong( hWnd, GWL_LPSTATUS, (LONG)lp )

/*
 * Since the size of HFONT and HINSTANCE change between 16 and 32 we
 * need different macros
 */
#ifndef WIN32
   #define GETHINST(hwnd)     (HINSTANCE)GetWindowWord( hwnd, GWW_HINSTANCE )
   #define GETHFONT(hWnd)     (HFONT)GetWindowWord( hWnd, GWW_HFONT )
   #define SETHFONT(hWnd,hf)  (HFONT)SetWindowWord( hWnd, GWW_HFONT, (WORD)hf )

   #define GETID(hWnd)        (WORD)GetWindowWord( hWnd, GWW_ID ) 
   
   #define GALLOC(cb)         GlobalAllocPtr( GHND, cb )
   #define GFREE(lp)          GlobalFreePtr( lp )
#else
   #define GETHINST(hwnd)     (HINSTANCE)GetWindowLong( hwnd, GWL_HMODULE )
   #define GETHFONT(hWnd)     (HFONT)GetWindowLong( hWnd, GWW_HFONT )
   #define SETHFONT(hWnd,hf)  (HFONT)SetWindowLong( hWnd, GWW_HFONT, (LONG)hf )

   #define GETID(hWnd)        (WORD)GetWindowLong( hWnd, GWL_ID ) 

   #define GALLOC(cb)         malloc(cb)
   #define GFREE(lp)          free(lp)
#endif


/************************************************************************
 * Local data structures
 ************************************************************************/

//
// MSG/STATLINE data structures
//
// Structure for msg line
typedef struct tagSTATUS
{
/* 2  */ WORD   wNumStats ;    // Number of status boxes
/* 10 */ RECT     rect ;         // coordinates of MsgLineWnd
/* 14 */ COLORREF rgbColor ;     // Text color
/* 16 */ WORD   wJustify ;
/* 18 */ BOOL     fBorder ;
/* 20 */ BOOL     fTouched ;     // TRUE if CurMSg has changed since last
/* 22 */ BOOL     fNoMsg ;
/* 24 */ GLOBALHANDLE hszDefault ;
                                    // WM_PAINT
} STATUS, *PSTATUS, FAR *LPSTATUS ;

// structure for status boxes
typedef struct tagSTATBOX
{
/* 8  */ RECT     rect ;                  // coordinates of Stat Box
/* 12 */ COLORREF rgbColor ;              // Color of text
/* 14 */ LONG     wWidth ;                // Width of box in pixels
/* 142*/ char     szCurStat[MAX_STATLEN] ;             // Current status
/* 144*/ WORD   wJustify ;
/* 146*/ BOOL     fBorder  ;
/* 148*/ BOOL     fTouched ;          // TRUE if CurStat has changed since
                                    // last WM_PAINT
} STATBOX, *PSTATBOX, FAR *LPSTATBOX ;


/************************************************************************
 * Internal APIs and data structures for MSG/STATLINES
 ************************************************************************/
static BOOL NEAR PASCAL IsRectInRect( LPRECT lpR1, LPRECT lpR2 ) ;

static BOOL NEAR PASCAL StatusCreate( HWND hWnd, LPCREATESTRUCT lp ) ;

/*
 * wID is always 16 bits since it must be packed in wParam or lParam as
 * a notification code.
 */
static void NEAR PASCAL StatusInvalidate( HWND hWnd, WORD wID ) ;

static void NEAR PASCAL StatusPaint( HWND hWnd ) ;

static BOOL NEAR PASCAL CheckClick( HWND, LPARAM, LPWORD ) ;

static int NEAR PASCAL StatGetWidth( HWND hWnd, LPSTR lpszText ) ;

LRESULT WINAPI
fnStatus( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

#if 0
/* misc. control flag defines */
#define DRAW3D_IN           0x0100
#define DRAW3D_OUT          0x0200

#define DRAW3D_TOPLINE      0x0400
#define DRAW3D_BOTTOMLINE   0x0800
#define DRAW3D_LEFTLINE     0x1000
#define DRAW3D_RIGHTLINE    0x2000

void WINAPI Draw3DLine( HDC hDC, int x, int y, int nLen,
                               int wShadowWidth, UINT wFlags ) ;
void WINAPI
Draw3DLine1( HDC hDC, int x, int y, int nLen, UINT wFlags ) ;

void WINAPI Draw3DRect( HDC hDC, LPRECT lpRect,
                               int wShadowWidth, UINT wFlags ) ;
COLORREF WINAPI GetHighlightColor( COLORREF rgb ) ;
COLORREF WINAPI GetShadowColor( COLORREF rgb ) ;
#endif

/*************************************************************************
 * Local Variables                                     
 *************************************************************************/
UINT  wExtraSpace = 1 ;

/*************************************************************************
 * window class names
 *************************************************************************/
char szStatusClassName[] = SZ_STATUSCLASSNAME ;


/*************************************************************************
 *  BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance )
 *
 *  Description: 
 *
 *    Registers the window class.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance )
{
   WNDCLASS wc ;

   if (!hPrev)
   {
      // 
      // Register the Child Windows class
      //
#ifdef IN_A_DLL
      wc.style = CS_DBLCLKS |
                 CS_VREDRAW |
                 CS_HREDRAW |
                 CS_GLOBALCLASS ;
#else
      wc.style = CS_DBLCLKS |
                 CS_VREDRAW |
                 CS_HREDRAW ;
#endif

      wc.lpfnWndProc = (WNDPROC)fnStatus ;

      wc.cbClsExtra = 0 ;
      //
      // cbWndExtra holds a handle to the STATUS structure
      //
      wc.cbWndExtra = NUM_EXTRABYTES ; 
                          

      wc.hInstance = hInstance ;
      wc.hIcon = NULL ;
      wc.hCursor = LoadCursor( NULL, IDC_ARROW ) ;
      wc.hbrBackground = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) ) ;
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
 *  LPARAM WINAPI
 *  fnStatus( HWND hWnd, int iMessage, WPARAM wParam, LPARAM lParam ) ;
 *
 *  Description:
 *    Default window proc for message line window.
 *
 *  Comments:
 *
 ************************************************************************/
LRESULT WINAPI fnStatus( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   LRESULT        lRetVal = 0L ;


   switch (wMsg)
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
            hFont = SelectObject( hDC, GETHFONT( hWnd ) ) ;

            GetTextMetrics( hDC, &tm ) ;

            SelectObject( hDC, hFont ) ;

            ReleaseDC (hWnd, hDC) ;

            return (LRESULT)(LONG)(tm.tmHeight + tm.tmExternalLeading +
                   (LINEWIDTH * 4) + wExtraSpace +
                   (GetSystemMetrics( SM_CYBORDER ) * 2)) ;
         }
         else
         {
            return 0L ;
         }
      }
      break ;

      case ST_GETSTATUSWIDTH:
         return MAKELRESULT( StatGetWidth( hWnd, (LPSTR)lParam ), 0 ) ;
      break ;

      case ST_SETSTATBOXSIZE:
         lpStatus = GETLPSTATUS( hWnd ) ;
         if (lpStatus)
         {
            lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

            if ((WORD)wParam < lpStatus->wNumStats)
            {
               lpStatBox[(WORD)wParam].wWidth = (int)lParam ;

               StatusInvalidate( hWnd, (WORD)0xFFFF ) ;
               InvalidateRect( hWnd, NULL, TRUE ) ;
            }
            else
               lRetVal = SW_ERR ;

         }
         else
            return SW_ERR ;
      break ;

      case ST_SETCOLOR:
         lpStatus = GETLPSTATUS( hWnd ) ;
         if (lpStatus)
         {
            if ((int)wParam == -1)
            {
               lpStatus->fTouched = TRUE ;
               lpStatus->rgbColor = (COLORREF)lParam ;
               StatusInvalidate( hWnd, 0 ) ;
            }
            else
            {
               if ((WORD)wParam < lpStatus->wNumStats)
               {
                  lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
                  lpStatBox[(WORD)wParam].fTouched = TRUE ;
                  lpStatBox[(WORD)wParam].rgbColor = (COLORREF)lParam  ;
                  StatusInvalidate( hWnd, (WORD)(LOWORD( wParam ) + 1) ) ;
               }
               else
                  lRetVal = SW_ERR ;
            }
         }
         else
            return SW_ERR ;
      break ;
         

      case WM_SETTEXT :
         lRetVal = DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         lpStatus = GETLPSTATUS( hWnd ) ;
         if (lpStatus)
         {

            if (!lpStatus->fNoMsg)
            {
               lpStatus->fTouched = TRUE ;

               StatusInvalidate( hWnd, 0 ) ;
            }
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETSTATBOX :
         lpStatus = GETLPSTATUS( hWnd ) ;
         if (lpStatus)
         {
            lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

            if ( (WORD)wParam < lpStatus->wNumStats &&
                 lstrlen( (LPSTR)lParam ) < MAX_STATLEN)
               {
                  lpStatBox[(WORD)wParam].fTouched = TRUE ;
                  lstrcpy( lpStatBox[(WORD)wParam].szCurStat, (LPSTR)lParam ) ;
               }
               else
                  lRetVal = SW_ERR ;

            StatusInvalidate( hWnd, (WORD)(LOWORD( wParam ) + 1) ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETBORDER:                  // wParam == -1 is message line
         lpStatus = GETLPSTATUS( hWnd ) ;
         if (lpStatus)
         {
            if ((int)wParam == -1)
               lpStatus->fBorder = (BOOL)LOWORD( lParam ) ;
            else
            {
               if ((WORD)wParam < lpStatus->wNumStats)
               {
                  lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
                  lpStatBox[(WORD)wParam].fBorder = (BOOL)LOWORD( (LONG)lParam ) ;
               }
               else
                  lRetVal = SW_ERR ;
            }
            StatusInvalidate( hWnd, (WORD)0xFFFF ) ;
            InvalidateRect( hWnd, NULL, TRUE ) ;
            UpdateWindow( hWnd ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETJUSTIFY:
         lpStatus = GETLPSTATUS( hWnd ) ;
         if (lpStatus)
         {
            if ((int)wParam == -1)
            {
               lpStatus->wJustify = LOWORD( lParam ) ;
               lpStatus->fTouched = TRUE ;
               StatusInvalidate( hWnd, 0 ) ;
            }
            else
            {
               if ((WORD)wParam < lpStatus->wNumStats)
               {
                  lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
                  lpStatBox[(WORD)wParam].wJustify = LOWORD( lParam ) ;
                  lpStatBox[(WORD)wParam].fTouched = TRUE ;
               }
               else
                  lRetVal = SW_ERR ;
               StatusInvalidate( hWnd, (WORD)(LOWORD( wParam ) + 1) ) ;
            }
            UpdateWindow( hWnd ) ;
         }
         else
            return SW_ERR ;
      break ;

      case ST_SETDEFAULTTEXT:
         /*
          * This message allows you to set the default text that
          * is used by the message bar.  Whenever the window text
          * is set to "" the default text will be displayed.
          */
         if (!lParam)
            return SW_ERR ;

         if (lpStatus = GETLPSTATUS( hWnd ))
         {
            if (lpStatus->hszDefault)
            {
               GlobalFree( lpStatus->hszDefault ) ;
            }

            if (lpStatus->hszDefault = GlobalAlloc( GHND,
                                          lstrlen( (LPSTR)lParam ) + 1 ))
            {
               LPSTR lpsz = GlobalLock( lpStatus->hszDefault ) ;

               lstrcpy( lpsz, (LPSTR)lParam ) ;

               GlobalUnlock( lpStatus->hszDefault ) ;
            }

            if (!lpStatus->fNoMsg)
            {
               lpStatus->fTouched = TRUE ;

               StatusInvalidate( hWnd, 0 ) ;
            }
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
            #ifndef WIN32
            PostMessage( GetParent( hWnd ), WM_COMMAND, (WPARAM)GETID( hWnd ),
                         (LPARAM)MAKELONG( hWnd, wID ) ) ;
            #else
            PostMessage( GetParent( hWnd ), WM_COMMAND,
                         (WPARAM)MAKELONG( GETID( hWnd ), wID ),
                         (LPARAM)hWnd ) ;
            #endif
         }
      }
      return 0L ;

      case WM_SIZE:
         StatusInvalidate( hWnd, (WORD)0xFFFF ) ;
      break ;
         
      case WM_PAINT :
         StatusPaint( hWnd ) ;
      break ;

      case WM_GETFONT:
         return MAKELRESULT( GETHFONT( hWnd ), 0 ) ;

      case WM_SETFONT:
      {
         HFONT hFontOld = GETHFONT( hWnd ) ;

         if (wParam)
            SETHFONT( hWnd, wParam ) ;

         if (lParam)
         {
            StatusInvalidate( hWnd, (WORD)0xFFFF ) ;
            InvalidateRect( hWnd, NULL, TRUE ) ;
         }

         return MAKELRESULT( hFontOld, 0 ) ;
      }
      break ;

      case WM_DESTROY :

         //
         // Get the handle from the Window extra bytes
         //
         lpStatus = GETLPSTATUS( hWnd ) ;

         // free the memory
         //
         if (lpStatus)
         {
            if (lpStatus->hszDefault)
               GlobalFree( lpStatus->hszDefault ) ;
            GFREE( lpStatus ) ;
         }
      break ;


      default :
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   } // switch (wMsg)

   return lRetVal ;

} /* fnStatus() */


/*************************************************************************
 *  WORD WINAPI StatGetWidth( HWND hWnd, LPSTR lpszText )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 *************************************************************************/
int NEAR PASCAL StatGetWidth( HWND hWnd, LPSTR lpszText )
{

   if (!hWnd)
   {
      return FALSE ;
   }

   if (lstrlen( lpszText ))
   {
      LPSTATUS     lpStatus ;
      HDC          hDC ;
      HANDLE       hFont ;
      int          nWidth ;

      // 
      // get the global handle from the window extra bytes
      //
      lpStatus = GETLPSTATUS( hWnd ) ;

      if (!lpStatus)
         return 0 ;

      if (hDC = GetDC (hWnd))
      {
         #ifdef WIN32
         SIZE size ;
         #endif

         hFont = SelectObject( hDC, GETHFONT( hWnd ) ) ;

         // Get Width
         #ifndef WIN32
         nWidth = LOWORD( GetTextExtent( hDC,
                              (LPSTR)lpszText, lstrlen( lpszText ) ) );
         #else
         GetTextExtentPoint( hDC, (LPSTR)lpszText, lstrlen( lpszText ), &size ) ;
         nWidth = size.cx ;

         #endif

         SelectObject( hDC, hFont ) ;

         ReleaseDC( hWnd, hDC ) ;
      }
      else
      {
         return LINEWIDTH * 4 ;
      }

      return nWidth + (int)(LINEWIDTH * 4) ;
   }
   return (int)LINEWIDTH * 4 ;

}/* StatGetWidth() */


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
static BOOL NEAR PASCAL StatusCreate( HWND hWnd, LPCREATESTRUCT lpCS )
{
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   UINT           i ;
   WORD         wNumStats ;

   if (lpCS->lpCreateParams)
      wNumStats = *(LPWORD)lpCS->lpCreateParams ;
   else
      wNumStats = 0 ;

   // 
   // Allocate the memory for the stat boxes and message line
   //
   if (!(lpStatus = GALLOC( sizeof( STATUS ) +
                            (sizeof( STATBOX ) * wNumStats) )))
   {
      return FALSE ;
   }

   //
   // Get structure and do some setting up
   //
   lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;

   lpStatus->wNumStats = wNumStats ;

   lpStatus->fTouched = TRUE ;
   lpStatus->fBorder = FALSE ;
   lpStatus->wJustify = DT_LEFT ;
   lpStatus->rgbColor = GetSysColor( COLOR_BTNTEXT )  ;
   if (lpCS->lpszName)
      lpStatus->fNoMsg = FALSE ;
   else
      lpStatus->fNoMsg = TRUE ;

   for (i = 0 ; i < lpStatus->wNumStats ; i++)
   {
      lpStatBox[i].fTouched = TRUE ;
      lpStatBox[i].fBorder = TRUE ;
      lpStatBox[i].wJustify = DT_CENTER ;
      lpStatBox[i].rgbColor = GetSysColor( COLOR_BTNTEXT ) ;
   }

   //
   // Store the handle in the window extra bytes at GWW_LPSTATUS
   //
   SETLPSTATUS( hWnd, lpStatus ) ;

   SETHFONT( hWnd, GetStockObject( SYSTEM_FONT ) ) ;

   SETFLAGS( hWnd, lpCS->style & 0xFF00 ) ;

   return TRUE ;
}
/* StatusCreate() */

/*************************************************************************
 *  static void StatusInvalidate( HWND hWnd, WORD wID )
 *
 *  Description:
 *    Invalidates the specified element of the status line.  The message
 *    part is 0, and the first stat box is 1.  Setting wID to (WORD)0xFFFF
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
 *          box, 2 = 2nd stat box, etc... (UINT)-1 = recalc and invalidate all.
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
static void NEAR PASCAL StatusInvalidate( HWND hWnd, WORD wID )
{
   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   WORD         i ;
   DWORD          wWndFlags ;
   RECT           rc ;

   if (hWnd == 0)
      return ;

   lpStatus= GETLPSTATUS( hWnd ) ;
   wWndFlags = GETFLAGS( hWnd ) ;

   if (lpStatus == 0)
   {
      DP1( hWDB, "StatusInvalidate : lpStatus == NULL!" ) ;
      return ;
   }

   if (lpStatus)
      lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
   else
   {
      DP1( hWDB, "StatusInvalidate : GlobalLock failed!" ) ;
      return ;
   }

   if (wID == (WORD)0xFFFF)
   {
      int         wHeight ;
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
         hFont = SelectObject( hDC, GETHFONT( hWnd ) ) ;
         GetTextMetrics( hDC, &tm ) ;
         SelectObject( hDC, hFont ) ;

         ReleaseDC (hWnd, hDC) ;
      }
      else
      {
         return ;
      }

      wHeight = tm.tmHeight +
                  tm.tmExternalLeading + (LINEWIDTH * 4)+wExtraSpace ;

      //
      // Set the rects for all boxes
      //
      // start with MsgLine
      //
      GetClientRect( hWnd, &rc ) ;

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
            lpStatBox[i].rect.right = rc.left + (int)lpStatBox[i].wWidth +
                                      (int)(2 * LINEWIDTH) ;

            rc.left = lpStatBox[i].rect.right + (int)(2 * LINEWIDTH) ;
            lpStatBox[i].fTouched = TRUE ;
         }
      }
      else
      {
         DP5( hWDB, "fNoMsg == FALSE" ) ;

         // Calculate from right to left.
         //

         for (i = lpStatus->wNumStats ; i > 0 ; i--)
         {
            lpStatBox[i-1].rect.right = rc.right ;

            //
            // left - 2*lw for border.
            //
            lpStatBox[i-1].rect.left = rc.right -
                                       (int)lpStatBox[i-1].wWidth -
                                       (int)(2 * LINEWIDTH) ; 

            lpStatBox[i-1].rect.top = rc.top ;


            lpStatBox[i-1].rect.bottom = rc.bottom ;

            rc.right = lpStatBox[i-1].rect.left - (int)(LINEWIDTH * 2) ;

            lpStatBox[i-1].fTouched = TRUE ;
         }

         if (lpStatus->wNumStats)
            lpStatus->rect.right = rc.right - (int)(LINEWIDTH * 9) ;
         else
            lpStatus->rect.right = rc.right - (int)(LINEWIDTH * 4) ;

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
      DP5( hWDB, "StatusInvalidate : ID Passed in" ) ;

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

}
/* StatusInvalidate() */

/*************************************************************************
 *  static void StatusPaint( HWND hWnd )
 *
 *  Description: 
 *
 *    Handles painting.  
 *
 *  Comments:
 *
 *************************************************************************/
static void NEAR PASCAL StatusPaint( HWND hWnd )
{

   LPSTATUS       lpStatus ;
   LPSTATBOX      lpStatBox ;
   int            i ;

   HANDLE         hFont ;
   RECT           r1 ;
   RECT           r2 ;
   DWORD          wFlags ;
   COLORREF       rgbBtnFace ;
   PAINTSTRUCT    ps ;
   TEXTMETRIC     tm ;

   GetClientRect( hWnd, &r1 ) ;

   lpStatus = GETLPSTATUS( hWnd ) ;
   if (lpStatus == 0)
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

   if (lpStatus)
      lpStatBox = (LPSTATBOX)((LPSTATUS)lpStatus+1) ;
   else
   {
      DP1( hWDB, "StatusPaint : lpStatus is NULL!" ) ;
      EndPaint( hWnd, &ps ) ;
      return ;
   }

   // Select font
   hFont = SelectObject( ps.hdc, GETHFONT( hWnd ) ) ;
   GetTextMetrics( ps.hdc, &tm ) ;
            
   SetBkColor( ps.hdc, GetNearestColor( ps.hdc, rgbBtnFace ) ) ;
   SetBkMode( ps.hdc, TRANSPARENT ) ;

   // 
   // See if the entire area needs to be painted
   //
   if (IsRectInRect( &r1, &ps.rcPaint ))
   {
      HPEN  hPen ;
      hPen = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_WINDOWFRAME ) ) ;

      DP5( hWDB, "Paint entire area" ) ;

      if (lpStatus->wNumStats)
      {
         int n ;

         //
         // Draw the main
         //
         if (lpStatus->fNoMsg)
         {
            // around stat boxes
            tdDraw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_OUT ) ;
         }
         else
         {
            n = lpStatus->rect.right + (int)(LINEWIDTH * 5) ;

            r2.top = r1.top ;
            r2.bottom = r1.bottom ;

            r2.left = n + 1 ;
            r2.right =  r1.right ;

            r1.right = n ;

            // msgline
            tdDraw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_OUT ) ;

            // around stat boxes
            tdDraw3DRect( ps.hdc, &r2, LINEWIDTH, DRAW3D_OUT ) ;
         }



         //
         // Message
         //
         if ((!lpStatus->fNoMsg) && lpStatus->fBorder)
         {
            CopyRect( &r2, &lpStatus->rect ) ;
            tdDraw3DRect( ps.hdc, &r2, LINEWIDTH, DRAW3D_IN ) ;
         }

         //
         // stat boxes
         //
         for (i = 0 ; i < (int)lpStatus->wNumStats ; i++)
         {
            if (lpStatBox[i].fBorder)
            {
               CopyRect( &r2, &lpStatBox[i].rect ) ;
               tdDraw3DRect( ps.hdc, &r2, LINEWIDTH, DRAW3D_IN ) ;
            }
         }

         if (!lpStatus->fNoMsg)
         {
            hPen = SelectObject( ps.hdc, hPen ) ;
            #ifndef WIN32
            MoveTo( ps.hdc, n, 0 ) ;
            #else
            MoveToEx( ps.hdc, n, 0, NULL ) ;
            #endif
            LineTo( ps.hdc, n, r1.bottom ) ;
            hPen = SelectObject( ps.hdc, hPen ) ;
         }

      }
      else
      {
         //
         // msgline only
         //
         tdDraw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_OUT ) ;

         if (lpStatus->fBorder)
         {
            CopyRect( &r1, &lpStatus->rect ) ;
            tdDraw3DRect( ps.hdc, &r1, LINEWIDTH, DRAW3D_IN ) ;
         }
      }

      DeleteObject( hPen ) ;
   }

   //
   // tdDraw text if needed
   //
   r1.top      = lpStatus->rect.top + (int)wExtraSpace - 1 ;
   r1.left     = lpStatus->rect.left   + (int)(LINEWIDTH * 3) ;
   r1.bottom   = lpStatus->rect.bottom - (int)LINEWIDTH ;
   r1.right    = lpStatus->rect.right  - (int)(LINEWIDTH * 3) ;

   if ((!lpStatus->fNoMsg) &&
       (lpStatus->fTouched || IsRectInRect( &r1, &ps.rcPaint )))
   {
      LPSTR lpszText = GlobalAllocPtr( GHND, GetWindowTextLength( hWnd )+1 ) ;
      
      if (lpszText)
      {   

         SetTextColor( ps.hdc, lpStatus->rgbColor ) ;

         DP5( hWDB, "About to draw text" ) ;

         // 
         // Clipping is on 
         //
         GetWindowText( hWnd, lpszText, GetWindowTextLength( hWnd )+1 ) ;

         if (lstrlen(lpszText) || !lpStatus->hszDefault)
            DrawText( ps.hdc, lpszText, -1, &r1,
               /*DT_WORDBREAK |*/ DT_NOPREFIX | lpStatus->wJustify ) ;
         else
         {
            LPSTR lpsz = GlobalLock( lpStatus->hszDefault) ;

            DP5( hWDB, "We have a default text string" ) ;
            DP5( hWDB, "    lpsz = %s", (LPSTR)lpsz ) ;

            DrawText( ps.hdc, lpsz, -1, &r1,
               /*DT_WORDBREAK |*/ DT_NOPREFIX | lpStatus->wJustify ) ;

            GlobalUnlock( lpStatus->hszDefault ) ;
         }

         lpStatus->fTouched = FALSE ;
         GlobalFreePtr( lpszText ) ;
      }
   }

   for (i = 0 ; i < (int)lpStatus->wNumStats ; i++)
   {
      r1.top    = lpStatBox[i].rect.top + (int)wExtraSpace - 1 ;
      r1.left   = lpStatBox[i].rect.left   + (int)(LINEWIDTH*3) ;
      r1.bottom = lpStatBox[i].rect.bottom - (int)LINEWIDTH ;
      r1.right  = lpStatBox[i].rect.right  - (int)(LINEWIDTH*3) ;

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

   SelectObject( ps.hdc, hFont ) ;
   EndPaint( hWnd, &ps ) ;

}/* StatusPaint() */

/*************************************************************************
 *  static BOOL CheckClick( HWND, LONG, LPUINT ) ;
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
static BOOL NEAR PASCAL CheckClick( HWND hWnd, LPARAM lParam, LPWORD lpwID )
{
   LPSTATUS     lpStatus ;
   LPSTATBOX    lpStatBox ;
   short        n ;
   int          x, y ;

   if (!hWnd)
   {
      return FALSE ;
   }

   // 
   // get the global handle from the window extra bytes
   //
   lpStatus = GETLPSTATUS( hWnd ) ;

   if (lpStatus)
   {
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
      *lpwID = (WORD)0xFFFF ;
      return TRUE ;
   }

   for (n = 0 ; n < (short)lpStatus->wNumStats ; n++)
      if (x >= lpStatBox[n].rect.left &&
          x <= lpStatBox[n].rect.right &&
          y >= lpStatBox[n].rect.top &&
          y <= lpStatBox[n].rect.bottom)
      {
         *lpwID = n ;
         return TRUE ;
      }

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
static BOOL NEAR PASCAL IsRectInRect( LPRECT lpR1, LPRECT lpR2 ) 
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


#if 0
/*
 * the following is a useful api for drawing 3d effects.
 */
/** void WINAPI Draw3DRect( HDC, LPRECT, UINT, UINT )
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
 *      int wShadowWidth   :   Width of the shadow in device coordinates.
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
 *  RETURN (void WINAPI):
 *      The 3D looking rectangle will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void WINAPI Draw3DRect( HDC hDC, LPRECT lpRect,
                               int wShadowWidth, UINT wFlags )
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


/** void WINAPI Draw3DLine( HDC hDC, int x, int y, int nLen,
 *
 *  DESCRIPTION: 
 *      Draws a 3D line that can be used to make a 3D box.
 *
 *  ARGUMENTS:
 *      HDC hDC             :   Handle to the device context that will be
 *                              used to display the 3D line.
 *
 *      int x, y           :   Coordinates of the beginning of the line.
 *                              These coordinates are in device units and
 *                              represent the _outside_ most point. Horiz-
 *                              ontal lines are drawn from left to right and
 *                              vertical lines are drawn from top to bottom.
 *
 *      int wShadowWidth   :   Width of the shadow in device coordinates.
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
 *  RETURN (void WINAPI):
 *      The line will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void WINAPI Draw3DLine( HDC hDC, int x, int y, int nLen,
                               int wShadowWidth, UINT wFlags ) 
{
   HBRUSH  hOldBrush ;
   HBRUSH  hBrush ;
   HPEN    hOldPen ;
   BOOL    fDark ;
   POINT   Point[ 4 ] ;         /* define a polgon with 4 points    */
   COLORREF rgb;

   /* if width is zero, don't do nothin'! */
   if ( !wShadowWidth )
       return ;

   /* if width is 1 use lines instead of polygons */
   if (wShadowWidth == 1)
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

   /* select the appropriate color for the fill */

   if ( fDark )
      rgb = GetSysColor( COLOR_BTNSHADOW ) ;
   else
   {

      /*
      * Windows 3.1 supports the COLOR_BTNHIGHTLIGHT color.
      * If we are running a version less than 3.1 then
      * simulate the highlight color.
      */
      if (LOWORD( GetVersion() ) >= 0x0A03)
         rgb = GetSysColor( COLOR_BTNHIGHLIGHT ) ;
      else
         rgb = GetHighlightColor( GetSysColor( COLOR_BTNFACE ) ) ;
   }

   hBrush = CreateSolidBrush( GetNearestColor( hDC, rgb ) ) ;

   hOldBrush = SelectObject( hDC, hBrush ) ;

   /* finally, draw the dern thing */
   Polygon( hDC, (LPPOINT)&Point, 4 ) ;

   /* restore what we killed */
   SelectObject( hDC, hOldBrush ) ;

   DeleteObject( hBrush ) ;

   SelectObject( hDC, hOldPen ) ;

} /* Draw3DLine() */

/****************************************************************
 *  void WINAPI
 *    Draw3DLine1( HDC hDC, int x, int y, int nLen, UINT wFlags )
 *
 *  Description: 
 *
 *    Does the same thing as Draw3DLine but for single width lines.
 *
 *  Comments:
 *
 ****************************************************************/
void WINAPI
Draw3DLine1( HDC hDC, int x, int y, int nLen, UINT wFlags )
{
   BOOL    fDark ;
   RECT    rc ;
   COLORREF   rgbOld ;
   COLORREF    rgb ;

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
   if ( fDark )
      rgb = GetSysColor( COLOR_BTNSHADOW ) ;
   else
   {
      /*
      * Windows 3.1 supports the COLOR_BTNHIGHTLIGHT color.
      * If we are running a version less than 3.1 then
      * simulate the highlight color.
      */
      if (LOWORD( GetVersion() ) >= 0x0A03)
         rgb = GetSysColor( COLOR_BTNHIGHLIGHT ) ;
      else
         rgb = GetHighlightColor( GetSysColor( COLOR_BTNFACE ) ) ;
   }

   rgbOld = SetBkColor( hDC, rgb ) ;

   /* finally, draw the dern thing */
   
   ExtTextOut( hDC, x, y, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

   SetBkColor( hDC, rgbOld ) ;

} /* Draw3DLine1()  */


typedef struct tagCOLORSTUFF
{
   COLORREF rgbMapFrom ;
   COLORREF rgbMapTo ;
} COLORSTUFF, *PCOLORSTUFF, FAR *LPCOLORSTUFF ;

COLORSTUFF  rgMapHighlight[] =
{
   RGBBLACK    , RGBLTGRAY,
   RGBWHITE    , RGBLTGRAY,
   RGBGRAY     , RGBLTGRAY,
   RGBLTGRAY   , RGBWHITE,
   
   RGBBLUE     , RGBLTBLUE,
   RGBLTBLUE   , RGBWHITE,
   RGBGREEN    , RGBLTGREEN,
   
   RGBCYAN     , RGBLTCYAN,
   RGBLTGREEN  , RGBWHITE,
   RGBLTCYAN   , RGBWHITE,
   
   RGBRED      , RGBLTRED,
   RGBMAGENTA  , RGBLTMAGENTA,
   RGBBROWN    , RGBYELLOW,

   RGBLTRED    , RGBWHITE,
   RGBLTMAGENTA, RGBWHITE,
   RGBYELLOW   , RGBWHITE
} ;

#define HL_TABLE_SIZE (sizeof(rgMapHighlight) / 2*sizeof(rgMapHighlight[0]))


COLORSTUFF  rgMapShadow[] =
{
   RGBBLACK    , RGBGRAY,
   RGBWHITE    , RGBBLACK,
   RGBGRAY     , RGBBLACK,
   RGBLTGRAY   , RGBGRAY,
   
   RGBBLUE     , RGBBLACK,   // gray looks silly 
   RGBLTBLUE   , RGBBLUE,     
   RGBGREEN    , RGBBLACK,   
   
   RGBCYAN     , RGBBLACK,
   RGBLTGREEN  , RGBGREEN,
   RGBLTCYAN   , RGBCYAN,
   
   RGBRED      , RGBBLACK,
   RGBMAGENTA  , RGBBLACK,
   RGBBROWN    , RGBBLACK,

   RGBLTRED    , RGBRED,
   RGBLTMAGENTA, RGBMAGENTA,
   RGBYELLOW   , RGBBROWN
} ;

#define SH_TABLE_SIZE (sizeof(rgMapShadow) / sizeof(rgMapShadow[0]))


/****************************************************************
 *  COLORREF WINAPI GetHighLightColor( COLORREF rgb )
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
 *    It's debatable whether the table method is faster than
 *    a big case statement.  I think it is probably easier to
 *    tune (i.e. if this color doesn't look right just add or
 *    modify a table entry).  But the optimizing C compiler will
 *    simply turn the case (if) statement into a table anyway.
 *
 ****************************************************************/
COLORREF WINAPI GetHighlightColor( COLORREF rgb )
{
   short i ;
   BYTE  cRed, cGreen, cBlue ;

   //
   // Zap the high byte 'cause we don't want it
   //
   rgb &= 0x00FFFFFFL ;

#ifdef ORIGINAL

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

#else

   // Use the table

   for (i = 0 ; i < HL_TABLE_SIZE ; i++)
      if (rgb == rgMapHighlight[i].rgbMapFrom)
         return rgMapHighlight[i].rgbMapTo ;

#endif
   //
   // If that didn't work then the color isn't one of the 16 "standard"
   // colors.  Do a algorithmic mapping instead...  This could
   // could probably be simplified down to some unreadable bitwise
   // operation but I can't think right now.
   // 

   cRed = GetRValue( rgb ) ;

   cGreen = GetGValue( rgb ) ;

   cBlue = GetBValue( rgb ) ;

   if (cRed == 128)
      cRed += 64 ;

   if (cGreen == 128)
      cGreen += 64 ;

   if (cBlue == 128)
      cBlue += 64 ;

   return RGB( cRed, cGreen, cBlue ) ;

} /* GetHighLightColor()  */


/****************************************************************
 *  COLORREF WINAPI GetShadowColor( COLORREF rgb )
 *
 *  Description: 
 *
 *    Returns an appropriate shadow color for the given rgb.
 *
 *  Comments:
 *
 ****************************************************************/
COLORREF WINAPI GetShadowColor( COLORREF rgb )
{
   short i ;
   BYTE  cRed, cGreen, cBlue ;

   //
   // Zap the high byte 'cause we don't want it
   //
   rgb &= 0x00FFFFFFL ;

#ifdef ORIGINAL
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
#else

   // Use the table
                
   for (i = 0 ; i < SH_TABLE_SIZE ; i++)
      if (rgb == rgMapShadow[i].rgbMapFrom)
         return rgMapShadow[i].rgbMapTo ;

#endif
   //
   // If that didn't work then the color isn't one of the 16 "standard"
   // colors.  Do a algorithmic mapping instead...  This could
   // could probably be simplified down to some unreadable bitwise
   // operation but I can't think right now.
   // 

   cRed = GetRValue( rgb ) ;

   cGreen = GetGValue( rgb ) ;

   cBlue = GetBValue( rgb ) ;

   if (cRed > 128)
      cRed -= 64 ;

   if (cGreen > 128)
      cGreen -= 64 ;

   if (cBlue > 128)
      cBlue -= 64 ;

   return RGB( cRed, cGreen, cBlue ) ;
} /* GetShadowColor()  */

#endif

/************************************************************************
 * End of File: status.c
 ************************************************************************/

