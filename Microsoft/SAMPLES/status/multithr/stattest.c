/********************************************************************
 *
 *    Test program to demonstrate the use of the status module.
 *
 *    For documentation on the functions in the status module
 *    see the source code comments in  status.C.
 *
 *   3/18/93  cek   Made Win32 version multithreaded for fun.
 *
 ********************************************************************/

#define STRICT

#include <windows.h>

#include <wdb.h>

#include "resource.h"
#include "status.h" 

/* By defining these you can turn on or off the status lines in this
 * test app
 */
//#define NOBOTSTAT
//#define NOTOPSTAT


HMODULE   hinstApp;

char     szAppName [] = "StatTest" ;
char     szDescription [] = "Test application to exercise the Status Window Class" ;

BOOL FAR PASCAL fnAbout(  HWND hDlg, UINT wMsg, UINT wParam, LONG lParam ) ;

#ifndef WIN32
#define GETHINST(hwnd)    (HINSTANCE)GetWindowWord( hwnd, GWW_HINSTANCE )
#else
#define GETHINST(hwnd)    (HMODULE)GetWindowLong( hwnd, GWL_HINSTANCE )
#endif

/*
 * In this demo application, the bottom status bar has "stat boxes" for
 * the capslock, numlock, and scroll lock status'.  The left side
 * of the window is the message window.  The text of the message window
 * can be set by calling SetWindowText() on the hWnd of the status window
 */
#ifndef NOBOTSTAT

   BOOL FAR PASCAL
      GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock, BOOL *bScrollLock ) ;

   HWND  hwndBottom ;
   #define INDEX_CAPSLOCK     0     // each stat box within the status
   #define INDEX_NUMLOCK      1     // window has an index value
   #define INDEX_SCROLLLOCK   2     // (from left to right).
   #define INDEX_LED1         3
   #define INDEX_LED2         4

   #define NUM_BOTBOXES 5

   /*
    * rgszBot (bottom stat box items)
    */
   struct
   {
      char *szStat[2] ;
      COLORREF rgb ;
   }
   rgszBot [] =
   {
      "", "CAPS", RGBBLUE,
      "", "NUM", RGBRED,
      "", "SCRL", RGBBLACK,
      "", "XX", RGBBLACK,
      "", "XX", RGBBLACK,
   } ;

   /*
    * the following are sample strings that we will SetWindowText()
    * to the 'message' part of the bottom status window
    */
   #define NUM_MSGS 5

   UINT wCurMsg = 0 ;

   char *rgszMsg[] =
   {
      "This is Msg #1...",
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
      "1234567890!@#%$^&*()-+}{[]",
      "abcdefghijklmnopqrstuvwxyz",
      "This is a test of the emergency broadcast system of America..."
   } ;
   BOOL  bCapsLock ;
   BOOL  bNumLock ;
   BOOL  bScrollLock ;
#endif

/*
 * In this demo app.  The top status window has five 'stat boxes':
 *    0 - A label that reads "Project:"
 *    1 - A field that might indicate the current project
 *    2 - A dummy stat box that provides "blank space"
 *    3 - A label that reads "Filename:"
 *    4 - A field that might indicate the current filename
 */
#ifndef NOTOPSTAT
   HWND  hwndTop ;

   #define INDEX_NAME_LBL      0
   #define INDEX_NAME          1
   #define INDEX_DUMMY1        2    // you can put space between items
   #define INDEX_GAUGE_LBL      3
   #define INDEX_GAUGE          4

   #define NUM_TOPBOXES 5

   /*
    * The "XXX"s below are a hack.  They make it easy to
    * specify the width of the stat box.
    */
   struct
   {
      char     *szTop[2] ;
      COLORREF rgb ;
   }
   rgszTop [] =
   {
      "","Project:", RGBRED,
      "XXXXXXXXXXXXXXXXXXXX", "Status Window Demo", RGBBLACK,
      "XX", "",   RGBBLACK,
      "","Gas Gauge:", RGBBLUE,
      "XXXXXXXXXXXXXXXXXXXXXXX", "C:\\FOO\\BAR\\STATUS.C", RGBBLACK
   } ;

#endif 


/*
 * The following code is a mish-mash used in this sample to setup
 * the stat boxes.  It is not necessarily the best way to do it,
 * but it worked for this sample.
 */
int  MyMax( int w1, int w2)     
{
   return ((w1 > w2) ? w1 : w2) ;
}


void SetUpStatBox( HWND hwnd, UINT nID, int nWidth )
{
   SendMessage( hwnd, ST_SETSTATBOXSIZE, nID, (LONG)nWidth ) ;
}

void SetStatus( HWND hwnd, UINT nID, LPSTR lpszBuf )
{
   SendMessage( hwnd, ST_SETSTATBOX, nID, (LONG)lpszBuf ) ;
}

#ifndef NOBOTSTAT
   void BuildBottomBoxes( HWND hwnd )
   {
      UINT i ;

      for (i = 0 ; i < NUM_BOTBOXES ; i++)
      {
         /*
          * Get the width of the widest peice of text you might
          * put into a stat box and use that to set the stat
          * box width.  This way you are sure that the text
          * will fit.
          */
         SetUpStatBox( hwnd, i,
            MyMax
            (
               StatusGetWidth( hwnd, 
                                 rgszBot[i].szStat[0] ),
               StatusGetWidth( hwnd,
                                 rgszBot[i].szStat[1] )
            )) ;

         SetStatus( hwnd, i, rgszBot[i].szStat[0] ) ;

         /*
          * set the color of this guy
          */
         SendMessage( hwnd, ST_SETCOLOR, (WPARAM)i, (LPARAM)rgszBot[i].rgb ) ;

      }
   }
#endif   

#ifndef NOTOPSTAT
   void BuildTopBoxes( HWND hwnd )
   {
      UINT i ;

      /*
      * Get the width of the widest peice of text you might
      * put into a stat box and use that to set the stat
      * box width.  This way you are sure that the text
      * will fit.
      */
      for (i = 0 ; i < NUM_TOPBOXES ; i++)
      {
         SetUpStatBox( hwnd, i,
            MyMax
            (
               StatusGetWidth( hwnd,
                                 rgszTop[i].szTop[0] ),
               StatusGetWidth( hwnd,
                                 rgszTop[i].szTop[1] )
            )) ;

         SetStatus( hwnd, i, rgszTop[i].szTop[1] ) ;

         /* set the color of this guy
          */
         if (i != 0 && i != 3)
             SendMessage( hwnd, ST_SETCOLOR, (WPARAM)i, (LPARAM)rgszTop[i].rgb ) ;
      }
   }
#endif

/****************************************************************
 *  static BOOL NEAR PASCAL DrawRoundLED( HDC, int x, int y, int nDiameter )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL DrawRoundLED( HDC hDC, int x, int y, int nDiameter )
{
   int nShadeX, nShadeY ;
   HPEN     hOldPen ;
   LOGPEN   lp ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Ellipse( hDC, x, y, x + nDiameter, y + nDiameter ) ;

   //
   // Calculate where the shading goes...
   // 
   nShadeX = x + nDiameter / 3 ;
   nShadeY = y + nDiameter / 3 ;

   GetObject( hOldPen, sizeof( LOGPEN ), (LPSTR)&lp ) ;

   SetPixel( hDC, nShadeX - 1 , nShadeY + 1  , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX     , nShadeY + 1  , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX     , nShadeY      , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX + 1 , nShadeY      , lp.lopnColor ) ;
   SetPixel( hDC, nShadeX + 1 , nShadeY - 1  , lp.lopnColor ) ;

   hOldPen = SelectObject( hDC, hOldPen ) ;

   return TRUE ;

} /* DrawRoundLED()  */

/****************************************************************
 *  static BOOL NEAR PASCAL
 *    DrawSquareLED( HDC, int x, int y, int nSizeX, int nSizeY )
 *
 *  Description: 
 *
 *    This function draws a 3D looking LED light using the
 *    current brush.
 *
 *  Comments:
 *
 ****************************************************************/
static BOOL NEAR PASCAL
DrawSquareLED( HDC hDC, int x, int y, int x1, int y1)
{
   HPEN     hOldPen ;

   hOldPen = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;

   Rectangle( hDC, x, y, x1, y1 ) ;

   SelectObject( hDC, hOldPen ) ;

   MoveToEx( hDC, x + 2, y1 - 3, NULL ) ;
   LineTo( hDC, x + 2, y + 2 ) ;
   LineTo( hDC, x1 - 2, y + 2 ) ;

   return TRUE ;

} /* DrawSquareLED()  */



/****************************************************************
 *  DWORD FAR PASCAL fnMainWnd( HWND hwnd, UINT wMsg, WPARAM wParam, LONG lParam )
 *
 *  Description: 
 *
 *    Main window proc for the status window sample.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD FAR PASCAL fnMainWnd( HWND hwnd, UINT wMsg, UINT wParam, LONG lParam )
{
   static   HFONT hStatFont ;
   static   long  nPercent ;

   switch (wMsg)
   {
      case WM_CREATE:
      {
         WORD lNumStats ;
         HDC    hdc ;

         SetTimer( hwnd, 42, 500, NULL ) ;


         #ifdef _WDB_H_
         D( if (hWDB = wdbOpenSession( hwnd,
                                       szAppName, "wdb.ini", 0 ))
            {
               DP( hWDB, "WDB is active!" ) ;
            }
         ) ;
         #endif
         hdc = GetDC( hwnd ) ;

         // Create a 10 point helv font
         hStatFont = CreateFont( -MulDiv( 10, GetDeviceCaps( hdc, LOGPIXELSY ), 72 ), 
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Helv" ) ;
         ReleaseDC( hwnd, hdc ) ;

      #ifndef NOTOPSTAT
         // create status window
         //
         lNumStats = NUM_TOPBOXES ;
         if (!(hwndTop = CreateWindow(
               SZ_STATUSCLASSNAME,
               NULL,
               WS_CHILD | WS_BORDER,
               0,          // just create it small for now.
               0,          // the WM_SIZE case will resize it.
               1,
               1,
               hwnd,
               (HMENU)IDD_TOP,
               hinstApp,
               (LPVOID)&lNumStats
            )))
         {
            #ifdef _WDB_H_
            DP( hWDB,  "MsgWnd create failed!" ) ;
            #endif 
            return TRUE ;
         }


         /*
          * set the font (if you dont do this the the system font is used).
          */
         SendMessage( hwndTop, WM_SETFONT, (WPARAM)hStatFont, 0L ) ;

         /*
          * using our helper functions (above) set the widths and
          * default text for each stat box (uses the currently set font).
          */
         BuildTopBoxes( hwndTop ) ;

         /*
          * Set the border and justificaton of the labels
          */
         SendMessage( hwndTop, ST_SETBORDER, INDEX_NAME_LBL, (LONG)FALSE ) ;
         SendMessage( hwndTop, ST_SETJUSTIFY, INDEX_NAME_LBL, (LONG)DT_RIGHT) ;
         SendMessage( hwndTop, ST_SETBORDER, INDEX_GAUGE_LBL, (LONG)FALSE ) ;
         SendMessage( hwndTop, ST_SETJUSTIFY, INDEX_GAUGE_LBL, (LONG)DT_RIGHT ) ;

         /*
          * Set the justification of the fields.
          */
         SendMessage( hwndTop, ST_SETJUSTIFY, INDEX_NAME, (LONG)DT_LEFT) ;

         SendMessage( hwndTop, ST_SETOWNERDRAW, INDEX_GAUGE, MAKELPARAM( TRUE, 0 ) ) ;
//         SendMessage( hwndTop, ST_SETBORDER, INDEX_GAUGE, (LONG)FALSE ) ;


         /*
          * The item at INDEX_DUMMY1 is used as a separator (i.e. blank
          * space.  Set it's border to 'off'
          */
         SendMessage( hwndTop, ST_SETBORDER, INDEX_DUMMY1, (LONG)FALSE ) ;

         ShowWindow( hwndTop, SW_NORMAL ) ;

         UpdateWindow( hwndTop ) ;

      #endif


      #ifndef NOBOTSTAT
         lNumStats = NUM_BOTBOXES ;
         if (!(hwndBottom = CreateWindow(
               SZ_STATUSCLASSNAME,
               " ",
               WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
               0,
               0,
               1,
               1,
               hwnd,
               (HMENU)IDD_BOTTOM,
               hinstApp,
               (LPVOID)&lNumStats
            )))
         {
            #ifdef _WDB_H_
            DP( hWDB,  "MsgWnd create failed!" ) ;
            #endif
            return TRUE ;
         }

         SendMessage( hwndBottom, WM_SETFONT, (WPARAM)hStatFont, 0L ) ;

         BuildBottomBoxes( hwndBottom ) ;
         ShowWindow( hwndBottom, SW_NORMAL ) ;

         UpdateWindow( hwndBottom ) ;
         /*
          * use the GetKeyStates() function (see below) to get our
          * current key states.  Use the SetStatus() macro to
          * set the stat boxes to the approriate state.
          */
         GetKeyStates( &bCapsLock, &bNumLock, &bScrollLock ) ;
         SetStatus( hwndBottom, INDEX_CAPSLOCK,
                    rgszBot[INDEX_CAPSLOCK].szStat[bCapsLock] ) ;
         SetStatus( hwndBottom, INDEX_NUMLOCK,
                    rgszBot[INDEX_NUMLOCK].szStat[bNumLock] ) ;
         SetStatus( hwndBottom, INDEX_SCROLLLOCK,
                    rgszBot[INDEX_SCROLLLOCK].szStat[bScrollLock] ) ;

         SendMessage( hwndBottom, ST_SETBORDER, INDEX_LED1, (LONG)FALSE ) ;
         SendMessage( hwndBottom, ST_SETOWNERDRAW, INDEX_LED1, (LONG)TRUE ) ;

         SendMessage( hwndBottom, ST_SETBORDER, INDEX_LED2, (LONG)FALSE ) ;
         SendMessage( hwndBottom, ST_SETOWNERDRAW, INDEX_LED2, (LONG)TRUE ) ;

         /*
          * Set the default text..
          */
         SendMessage( hwndBottom, ST_SETDEFAULTTEXT, 0,
                      (LONG)(LPSTR)"This is the default text" ) ;
      #endif
      }
      break ;

      #ifndef NOBOTSTAT
      /*
       * We use the following messages to detect changes in the key states.
       */
      case WM_MOUSEMOVE:
      case WM_SETFOCUS:
      case WM_KEYDOWN :
      {
         BOOL  bCaps = bCapsLock,      //globals
               bNum  = bNumLock,
               bScroll = bScrollLock ;
         //
         // if any changed
         //
         if (GetKeyStates( &bCapsLock, &bNumLock, &bScrollLock ))
         {
            if (bCapsLock != bCaps)
               SetStatus( hwndBottom, INDEX_CAPSLOCK,
                           rgszBot[INDEX_CAPSLOCK].szStat[bCapsLock] ) ;

            if (bNumLock != bNum)
               SetStatus( hwndBottom, INDEX_NUMLOCK,
                           rgszBot[INDEX_NUMLOCK].szStat[bNumLock] ) ;

            if (bScrollLock != bScroll)
               SetStatus( hwndBottom, INDEX_SCROLLLOCK,
                           rgszBot[INDEX_SCROLLLOCK].szStat[bScrollLock] ) ;
         }
         return DefWindowProc( hwnd, wMsg, wParam, lParam ) ;
      }
      break ;
      #endif

      case WM_COMMAND:
         switch ( LOWORD( wParam ))
         {
            case IDM_FILEEXIT:
               SendMessage( hwnd, WM_CLOSE, 0, 0L ) ;
               break ;

            #ifndef NOBOTSTAT
            case IDM_NEXTMSG:
               wCurMsg++ ;
               if (wCurMsg == NUM_MSGS)
                  wCurMsg = 0 ;

               SetWindowText( hwndBottom, rgszMsg[wCurMsg] ) ;
            break ;

            /*
             * Since status windows are child controls we can get
             * WM_COMMAND messages from them when the user does something.
             * this can be very handy.  For example run WinWord and
             * double click on the bottom status window in WinWord!
             *
             * Our Status window class will notifiy us when the user
             * doubleclicks on the status window.  The HIWORD
             * of lParam is the notification code, which in our case
             * is 0xFFFF for the message part, and 0 through 4 for the
             * stat boxes.
             *
             */
            case IDD_BOTTOM:
            {
               char szBuf[80] ;

               /*
                * The user double clicked on the message part, pop up
                * the about box.
                */
               #ifndef WIN32
               if (HIWORD( lParam ) == 0xFFFF)
               #else
               if (HIWORD( wParam ) == 0xFFFF)
               #endif
               {
                  FARPROC lpfnAbout ;
                  lpfnAbout = MakeProcInstance( (FARPROC)fnAbout, GETHINST( hwnd ) ) ;

                  DialogBox(  GETHINST( hwnd ),        
                              "ABOUTBOX",     
                              hwnd,             
                              (DLGPROC)lpfnAbout ) ;

                  FreeProcInstance( lpfnAbout ) ;
               }

               /* for kicks, we will set the message line to indicate
                * what the user did (i.e. where he clicked).
                */
               #ifndef WIN32
               wsprintf( szBuf, "Status Box Index = %d ", HIWORD( lParam ) ) ;
               #else
               wsprintf( szBuf, "Status Box Index = %d ", HIWORD( wParam ) ) ;
               #endif

               SetWindowText( hwndBottom, szBuf ) ;
            }
            break ;
            #endif

            case IDM_ABOUT:
            {
               FARPROC lpfnAbout ;
               lpfnAbout = MakeProcInstance( (FARPROC)fnAbout, GETHINST( hwnd ) ) ;

                 DialogBox(  GETHINST( hwnd ),        
                             "ABOUTBOX",     
                           hwnd,             
                           (DLGPROC)lpfnAbout ) ;

               FreeProcInstance( lpfnAbout ) ;

               #ifndef NOBOTSTAT
               SetWindowText( hwndBottom, "" ) ;
               #endif
            }
            break;

            default:
               return DefWindowProc( hwnd, wMsg, wParam, lParam ) ;
         }
         break ;

      case WM_TIMER:
      {
         RECT rc ;

         if (nPercent >= 1000)
            nPercent = 0 ;
         else
            nPercent+= 1 ;

         SendMessage( hwndTop, ST_GETSTATBOXRECT, INDEX_GAUGE, (LPARAM)(LPRECT)&rc ) ;
         InvalidateRect( hwndTop, &rc, FALSE ) ;
         UpdateWindow( hwndTop ) ;

         SendMessage( hwndBottom, ST_GETSTATBOXRECT, INDEX_LED1, (LPARAM)(LPRECT)&rc ) ;
         InvalidateRect( hwndBottom, &rc, FALSE ) ;
         UpdateWindow( hwndTop ) ;

         SendMessage( hwndBottom, ST_GETSTATBOXRECT, INDEX_LED2, (LPARAM)(LPRECT)&rc ) ;
         InvalidateRect( hwndBottom, &rc, FALSE ) ;
         UpdateWindow( hwndTop ) ;
      }  
      break ;

        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpdis ;
            
            lpdis = (LPDRAWITEMSTRUCT)lParam ;
            
            if (lpdis->hwndItem == hwndTop)
            {
                switch( lpdis->itemID )
                {
                    case INDEX_GAUGE:
                    {
                        char     szBuf[64] ;
                        RECT     rc ;
                        RECT     rcGauge ;
                        int      cxText, cyText ;
                        SIZE     Size ;
                        int      cxDone ;
                        HBRUSH   hbr ;
                        
                        wsprintf( szBuf, "%d%%", (UINT)(nPercent / 10L) ) ;
                        
                        hbr = SelectObject( lpdis->hDC, GetStockObject( NULL_BRUSH ) ) ;
                        hStatFont = SelectObject( lpdis->hDC, hStatFont ) ;
                        GetTextExtentPoint( lpdis->hDC, szBuf, lstrlen( szBuf ), &Size ) ;
                        
                        rc = lpdis->rcItem ;
                        Rectangle( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom ) ;
                        InflateRect( &rc, -1, -1 ) ;
                        
                        rcGauge = rc ;
                        cxText = rcGauge.left + ((rcGauge.right - rcGauge.left) / 2) - (Size.cx / 2) ;
                        cyText = rcGauge.top + ((rcGauge.bottom - rcGauge.top) /2 ) - (Size.cy / 2) ;
                        cxDone = rcGauge.left +
                            (((((long)rcGauge.right - (long)rcGauge.left) * nPercent) + 50) / 100 ) ;
                        
                        // Draw the done color
                        //
                        SetBkColor( lpdis->hDC, RGBLTRED ) ;
                        SetTextColor( lpdis->hDC, RGBWHITE ) ;
                        
                        rcGauge.right = cxDone ;
                        
                        ExtTextOut( lpdis->hDC, cxText, cyText, ETO_OPAQUE | ETO_CLIPPED, &rcGauge, szBuf, lstrlen(szBuf), NULL ) ;
                        
                        // Draw the not-done color
                        //
                        SetBkColor( lpdis->hDC, RGBLTGREEN ) ;
                        SetTextColor( lpdis->hDC, RGBBLACK ) ;
                        
                        rcGauge = rc ;
                        rcGauge.left = cxDone ;
                        
                        ExtTextOut( lpdis->hDC, cxText, cyText, ETO_OPAQUE | ETO_CLIPPED, &rcGauge, szBuf, lstrlen(szBuf), NULL ) ;
                        
                        hbr = SelectObject( lpdis->hDC, hbr ) ;
                        hStatFont = SelectObject( lpdis->hDC, hStatFont ) ;
                    }
                    break ;
                }
            }
            else
            {
                // hwndItem == hwndBottom
                switch( lpdis->itemID )
                {
                    case INDEX_LED1:
                    {
                        RECT     rc ;
                        HBRUSH   hbrColor ;
                        HPEN     hPen ;
                        static   fOn = FALSE ;
                        int      cx ;
                        
                        rc = lpdis->rcItem ;
                        InflateRect( &rc, -1, -1 ) ;
                        
                        if (fOn)
                        {
                            hbrColor = CreateSolidBrush( RGBLTRED ) ;
                            hPen = CreatePen( PS_SOLID, 1, RGBWHITE ) ;
                        }
                        else
                        {   
                            hbrColor = CreateSolidBrush( RGBRED ) ;
                            hPen = CreatePen( PS_SOLID, 1, RGBLTRED) ;
                        }
                        
                        fOn = !fOn ;
                        
                        hbrColor = SelectObject( lpdis->hDC, hbrColor ) ;
                        hPen = SelectObject( lpdis->hDC, hPen ) ;
                       
                        cx = rc.left + ((rc.right - rc.left) / 2) - ((rc.bottom - rc.top) / 2) ;
                        DrawRoundLED( lpdis->hDC, cx, rc.top, rc.bottom - rc.top) ;
                        
                        hbrColor = SelectObject( lpdis->hDC, hbrColor ) ;
                        DeleteObject( hbrColor ) ;
                        hPen = SelectObject( lpdis->hDC, hPen ) ;
                        DeleteObject( hPen ) ;
                    }
                    break ;

                    case INDEX_LED2:
                    {
                        RECT     rc ;
                        HBRUSH   hbrColor ;
                        HPEN     hPen ;
                        static   fOn = FALSE ;
                        int     cy  ;
                        
                        rc = lpdis->rcItem ;
                        InflateRect( &rc, -1, -1 ) ;
                        
                        if (fOn)
                        {
                            hbrColor = CreateSolidBrush( RGBLTGREEN ) ;
                            hPen = CreatePen( PS_SOLID, 1, RGBWHITE ) ;
                        }
                        else
                        {   
                            hbrColor = CreateSolidBrush( RGBGREEN ) ;
                            hPen = CreatePen( PS_SOLID, 1, RGBLTGREEN ) ;
                        }
                        
                        fOn = !fOn ;
                        
                        hbrColor = SelectObject( lpdis->hDC, hbrColor ) ;
                        hPen = SelectObject( lpdis->hDC, hPen ) ;
                        DrawSquareLED( lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom ) ;
                       
                        hbrColor = SelectObject( lpdis->hDC, hbrColor ) ;
                        DeleteObject( hbrColor ) ;
                        hPen = SelectObject( lpdis->hDC, hPen ) ;
                        DeleteObject( hPen ) ;
                    }
                    break ;
                }
            }  // if hwndItem
        }
        break ;

      case WM_SIZE:
      {
         int     wStatusHeight ;
         RECT     rc ;

         #ifndef NOTOPSTAT
         /*
          * Size the top status bar first.  Ask it how high it wants
          * to be and use MoveWindow() on it.
          */
         GetClientRect( hwndTop, &rc ) ;
         if (hwndTop) 
         {
            wStatusHeight = (int)SendMessage( hwndTop, ST_GETHEIGHT, 0, 0L ) ;
            MoveWindow( hwndTop,
                        -1,
                        -1,
                        LOWORD(lParam)+2,
                        wStatusHeight,
                        TRUE ) ;
         }
         #endif


         #ifndef NOBOTSTAT
         /*
          * Size the bottom status bar.
          */
         wStatusHeight = (int)SendMessage( hwndBottom, ST_GETHEIGHT, 0, 0L ) ;
         if (hwndBottom)
         {
            MoveWindow( hwndBottom,
                           -1,
                           (HIWORD(lParam) - wStatusHeight) + 1,
                           LOWORD(lParam)+2,
                           wStatusHeight,
                           TRUE ) ;
         }
         #endif

      }
      break ;

      case WM_DESTROY:
      {
         /*
          * child windows should get destroyed automatically, but
          * we do it here just in case (there's nothing wrong with
          * being too careful.
          */
         #ifndef NOTOPSTAT
         if (hwndTop)
            DestroyWindow( hwndTop ) ;
         #endif

         #ifndef NOBOTSTAT
         if (hwndBottom)
            DestroyWindow( hwndBottom ) ;
         #endif

         KillTimer( hwnd, 42 ) ;

         DeleteObject( hStatFont ) ;

         #ifdef _WDB_H_
         D( hWDB = wdbCloseSession( hWDB ) ) ;
         #endif

         PostQuitMessage( 0 ) ;
      }
      break;

      default:          
         return DefWindowProc( hwnd, wMsg, wParam, lParam ) ;
   }
   return 0L ;
} /* fnMainWnd()  */



/************************************************************************
 *  BOOL FAR PASCAL
 *  GetKeyStates( BOOL *bCapsLock , BOOL *bNumLock, BOOL *bScrollLock ) ;
 *
 *  Description:
 *    Gets the states of the toggle keys.
 *
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *             TRUE
 *                      The states changed.
 *
 *             FALSE
 *                      The states did not change
 *
 *  Comments:
 *
 ************************************************************************/
BOOL FAR PASCAL
GetKeyStates( BOOL *bCapsLock, BOOL *bNumLock, BOOL *bScrollLock )
{
   BOOL bChanged = FALSE ;

   if (*bCapsLock != (GetKeyState( VK_CAPITAL ) & 0x01 ))
   {
      *bCapsLock = !*bCapsLock ;
      bChanged = TRUE ;
   }
   if (*bNumLock != (GetKeyState( VK_NUMLOCK ) & 0x01 ))
   {
      *bNumLock = !*bNumLock ;
      bChanged = TRUE ;
   }

   if (*bScrollLock != (GetKeyState( 0x91 ) & 0x01 ))
   {
      *bScrollLock = !*bScrollLock ;
      bChanged = TRUE ;
   }
   return bChanged ;
}/* GetKeyStates() */


/****************************************************************
 *  BOOL FAR PASCAL
 *    fnAbout(  HWND hDlg, UINT wMsg, UINT wParam, LONG lParam )
 *
 *  Description: 
 *
 *    About box window procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnAbout(  HWND hDlg, UINT wMsg, UINT wParam, LONG lParam )
{
   #ifdef WIN32
   UNREFERENCED_PARAMETER( lParam ) ;
   #endif

   switch (wMsg)
   {
      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
                 EndDialog( hDlg, TRUE ) ;
                 return TRUE ;
          }
         return TRUE ;

      case WM_INITDIALOG:
         SetDlgItemText( hDlg, IDD_DESC, szDescription ) ;
         return TRUE ;
   }

   return FALSE ;
} /* fnAbout()  */

/****************************************************************
 *  BOOL InitApplication( HANDLE hInstance )
 *
 *  Description: 
 *
 *    Registers the main window class.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL InitApplication( HANDLE hInstance )
{
   WNDCLASS  wc;
   BOOL      b ;

   wc.style         = 0 ;      
   wc.lpfnWndProc   = (WNDPROC)fnMainWnd;  
                                  
   wc.cbClsExtra    = 0;             
   wc.cbWndExtra    = 0;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, szAppName );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = GetStockObject( WHITE_BRUSH ); 
   wc.lpszMenuName  = szAppName;
   wc.lpszClassName = szAppName;

   /* Register the window class and return success/failure code. */

   b = RegisterClass( &wc ) ;

   if (!b)
      return FALSE ;

   return TRUE ;
} /* InitApplication()  */

/****************************************************************
 *  int PASCAL
 *    WinMain(  HANDLE hInstance, HANDLE hPrevInstance,
 *              LPSTR lpCmdLine, int nCmdShow )
 *
 *  Description: 
 *
 *    Entry point.
 *
 *  Comments:
 *
 ****************************************************************/
int PASCAL WinMain(  HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow )
{
   HWND            hwnd;               /* Main window handle.                */
   MSG msg;
 
   if (!hPrevInstance)     
      if (!InitApplication( hInstance ))
         return FALSE ;

   /* Register the status window class.
    * Note that if you put the status.c module into a DLL you
    * will probably call this function from your LibMain()
    *
    */
   StatusInit( hPrevInstance, hInstance, SZ_STATUSCLASSNAME ) ;

   hinstApp = hInstance;

#ifndef WIN32
   hwnd = CreateWindow(   
                        szAppName,   
                        "Status Control Test (16 Bit)", 
                        WS_OVERLAPPEDWINDOW,  
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;
#else
   hwnd = CreateWindow(   
                        szAppName,   
                        "Status Control Test (32 Bit)", 
                        WS_OVERLAPPEDWINDOW,  
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;
#endif

   if (!hwnd)
       return FALSE ;

   ShowWindow( hwnd, nCmdShow ) ;
   UpdateWindow( hwnd ) ;


   while (GetMessage( &msg, NULL, NULL, NULL ))      
   {
      TranslateMessage( &msg ) ;
      DispatchMessage( &msg ) ;       
   }

   return msg.wParam ;       
} /* WinMain()  */


/************************************************************************
 * End of stattest.c
 ************************************************************************/
