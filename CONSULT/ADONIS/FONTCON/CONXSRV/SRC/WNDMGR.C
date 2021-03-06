/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV
 *
 *      Module:  wndmgr.c
 *
 *     Remarks:  Handles moving and sizing all of the windows.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"
#include "wndmgr.h"

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
   short    dyTTY = 0 ;
   short    dyTool = 1 ;

   if (wMsg == WM_SIZE)
   {
      HDWP hdwp ;

      if (!(hdwp = BeginDeferWindowPos( 1 )))
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


      if (hwndTTY && IsWindowVisible( hwndTTY ))
      {
         hdwp = DeferWindowPos( hdwp, hwndTTY, NULL,
                     0, dyTTY-1,
                     dx,
                     dy - (dyStat) + 2,
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

