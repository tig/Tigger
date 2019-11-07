/************************************************************************
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  wndmgr.c
 *
 *     Remarks:  Handles moving and sizing all of the windows.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"
#include "wndmgr.h"
#include "status.h"

/****************************************************************
 *  LRESULT FAR PASCAL
 *    SizeHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Hanldes the WM_SIZE message, moving children as appropriate.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT FAR PASCAL SizeHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   short    dyStat = 0;
   short    dy = HIWORD( lParam ) ;
   short    dx = LOWORD( lParam ) ;
   short    dyLB = 0 ;
   short    dyInput = 0 ;
   short    dyTool = 1 ;

   if (wMsg == WM_SIZE)
   {
      HDWP hdwp ;

      if (!(hdwp = BeginDeferWindowPos( 1 )))
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      if (hwndTool && IsWindowVisible( hwndTool ))
      {
         dyTool = (short)(DWORD)SendMessage( hwndTool, ST_GETHEIGHT, 0, (LPARAM)0L ) ;

         hdwp = DeferWindowPos( hdwp, hwndTool, NULL,
                        -1,
                        -1,
                        dx+2,
                        dyTool,
                        SWP_NOZORDER ) ;

         dyLB = dyTool - 1 ;
      }

      if (!hdwp)
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      if (hwndInput && IsWindowVisible( hwndInput ))
      {
         dyInput = (short)(DWORD)SendMessage( hwndInput, ST_GETHEIGHT, 0, (LPARAM)0L ) ;

         hdwp = DeferWindowPos( hdwp, hwndInput, NULL,
                     -1,
                     dyTool-2,
                     dx+2,
                     dyInput,
                     SWP_NOZORDER ) ;

         dyLB += dyInput - 1 ;
      }

      if (!hdwp)
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      if (hwndStat && IsWindowVisible( hwndStat ))
      {
         dyStat = (short)(DWORD)SendMessage( hwndStat, ST_GETHEIGHT, 0, (LPARAM)0L ) ;

         hdwp = DeferWindowPos( hdwp, hwndStat, NULL,
                     -1,
                     (dy - dyStat) + 1,
                     dx+2,
                     dyStat,
                     SWP_NOZORDER ) ;

         dyStat-- ;
      }

      if (!hdwp)
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      if (hwndLB && IsWindowVisible( hwndLB ))
      {
         hdwp = DeferWindowPos( hdwp, hwndLB, NULL,
                     -1, dyLB-1,
                     dx+2,
                     dy - (dyTool + dyInput + dyStat) + 3,
                     SWP_NOZORDER ) ;

      }

      EndDeferWindowPos( hdwp ) ;

      return (LRESULT)0L ;
   }

   if ((wMsg == WM_GETMINMAXINFO) && IsWindowVisible( hWnd ))
   {
      LPPOINT lppt = (LPPOINT)lParam ;
      short dy ;

      dy = 2 * GetSystemMetrics( SM_CYFRAME ) +
           GetSystemMetrics( SM_CYMENU ) +
           GetSystemMetrics( SM_CYCAPTION ) ;

#ifdef USE_LB_FOR_MINMAX         
      dy += 2* GetSystemMetrics( SM_CYVSCROLL ) +
            GetSystemMetrics( SM_CYVTHUMB ) ;
#endif 
                                 
      if (hwndTool && IsWindowVisible( hwndTool ))
         dy += (short)(DWORD)SendMessage( hwndTool, ST_GETHEIGHT, 0, (LPARAM)0L ) - 1 ;

      if (hwndInput && IsWindowVisible( hwndInput ))
         dy += (short)(DWORD)SendMessage( hwndInput, ST_GETHEIGHT, 0, (LPARAM)0L ) - 1 ;

      if (hwndStat && IsWindowVisible( hwndStat ))
         dy += (short)(DWORD)SendMessage( hwndStat, ST_GETHEIGHT, 0, (LPARAM)0L ) - 1 ;

      lppt[3].y = dy - 1 ;

      return (LRESULT)0L ;
   }

   return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

} /* SizeHandler()  */


/************************************************************************
 * End of File: wndmgr.c
 ***********************************************************************/

