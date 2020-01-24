//************************************************************************
//  LRESULT FAR PASCAL SplashWndProc( HWND hWnd, WORD wMsg,
//                                    WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the window procedure for the "splash" window.
//
//  Parameters:
//     Same as standard window procedures.
//
//  History:   Date       Author      Comment
//              1/19/92   BryanW      Wrote it.
//
//************************************************************************

LRESULT FAR PASCAL SplashWndProc( HWND hWnd, UINT uMsg,
                                  WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_CREATE:
      {
         int      cxSize, cySize ;
         HBITMAP  hbmCredits ;
         BITMAP   bmCredits ;

         DP3( hWDB, "SplashWndProc:WM_CREATE" ) ;

         hbmCredits = LoadBitmap( GETHINST( hWnd ),
                                  MAKEINTRESOURCE( CREDITSBMP ) ) ;
         if (NULL == hbmCredits)
         {
            DP1( hWDB, "SplashWndProc: Failed to load bitmap!" ) ;
            return ( -1 ) ;
         }

         SETHBITMAP( hWnd, hbmCredits ) ;

         // set a timer for window destruction 

         SetTimer( hWnd, TIMERID_INIT, 5000, NULL ) ;

         GetObject( hbmCredits, sizeof( BITMAP ), (LPSTR) &bmCredits ) ;
         cxSize = GetSystemMetrics( SM_CXSCREEN ) ;
         cySize = GetSystemMetrics( SM_CYSCREEN ) ;
         MoveWindow( hWnd, (cxSize - bmCredits.bmWidth) / 2,
                     (cySize - bmCredits.bmHeight) / 2,
                     bmCredits.bmWidth, bmCredits.bmHeight, FALSE ) ;
      }
      break ;

      case WM_TIMER:
         KillTimer( hWnd, TIMERID_INIT ) ;
         DestroyWindow( hWnd ) ;
         break ;

      case WM_PAINT:
      {
         HBITMAP      hbmOld ;
         HDC          hDC, hMemDC ;
         PAINTSTRUCT  ps ;

         hDC = BeginPaint( hWnd, &ps ) ;
         hMemDC = CreateCompatibleDC( hDC ) ;
         hbmOld = SelectObject( hMemDC, GETHBITMAP( hWnd ) ) ;
         BitBlt( hDC, ps.rcPaint.left, ps.rcPaint.top,
                 ps.rcPaint.right - ps.rcPaint.left,
                 ps.rcPaint.bottom - ps.rcPaint.top,
                 hMemDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY ) ;
         SelectObject( hMemDC, hbmOld ) ;
         DeleteDC( hMemDC ) ;
         EndPaint( hWnd, &ps ) ;
      }
      break ;

      case WM_DESTROY:
         DeleteObject( GETHBITMAP( hWnd ) ) ;
         break ;

      default:
         return( DefWindowProc( hWnd, uMsg, wParam, lParam ) ) ;
   }
   return ( NULL ) ;

} // end of SplashWndProc()

