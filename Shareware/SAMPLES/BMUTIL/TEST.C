/********************************************************************
 *
 *    Test program to demonstrate the use of the bmutil.c module.
 *
 *    To use this app:
 *
 * Click the left mouse button and you get a bitmap of a 
 * New that is bitbltted 'Tranparently'.
 * 
 * Click the right mouse button and you get a bitmap of a
 * cat that is 'translated' so that the 'background' color
 * of the bitmap is the same as GetSysColor(COLOR_WINDOW)
 *
 *
 ********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include <wdb.h>
#include <rgb.h>
#include "bmutil.h"

//
// testd.h includes dialog box and other resource defines
//
#include "testd.h"

//
// Define these to set how big the window is when it is created
//
#define NUMCHARSX 80
#define NUMCHARSY 25

#define APPDESC "Demonstrates Bitmaps"


   COLORREF rgcsColors[] =
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

   #define _NUM_COLORS (sizeof( rgcsColors ) / sizeof( rgcsColors[0] ))


/********************************************************************
 *   Global variables
 ********************************************************************/

char     szAppName [] = "Test" ;
HANDLE   hinstApp ;

/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL PaintInfo( HWND hWnd ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;

/********************************************************************
 *   Useful macros
 ********************************************************************/
#define GETHINST(hwnd)     (GetWindowWord( hwnd, GWW_HINSTANCE ))

COLORREF WINAPI GetHighlightColor( COLORREF rgb )
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

} /* GetHighlightColor()  */


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

/********************************************************************
 *   WinMain function - Entry point to applcation
 ********************************************************************/
int PASCAL WinMain(  HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpszCmdLine,		  
                     int      nCmdShow )			  
{
   MSG msg;

   hinstApp = hInstance ;

   if (!hPrevInstance)	
	   if (!InitApplication( hInstance )) 
	       return FALSE ;	  


   if (!InitInstance( hInstance, nCmdShow ))
        return FALSE ;

   while (GetMessage( &msg, NULL, NULL, NULL ))	 
  	{
      TranslateMessage( &msg ) ;
     	DispatchMessage( &msg ) ;	  
   }
   return msg.wParam ;	  
}


/********************************************************************
 *   Main window procedure
 ********************************************************************/
LONG FAR PASCAL fnMainWnd( HWND hWnd, 
                             WORD wMsg,  
                             WORD wParam,			 
                             LONG lParam )			  
{
   char   szBuf[80] ;
   static HBITMAP  hbmSave ;

   switch (wMsg)
   {
      case WM_CREATE:
      {
         BITMAP   BM ;
         HDC      hdcMem ;
         UINT     i ;
         HDC      hdc ;
         HBITMAP hbmNew ;
         HDC      hdcSave ;


         hdc = GetDC( hWnd ) ;

         hbmNew = LoadBitmap( hinstApp, "BMP2" ) ;
         GetObject( hbmNew, sizeof( BITMAP ), &BM ) ;
         DeleteObject( hbmNew ) ;


         hdcSave = CreateCompatibleDC( hdc ) ;
         hbmSave = CreateCompatibleBitmap( hdc, BM.bmWidth * _NUM_COLORS, BM.bmHeight ) ;

         hbmSave = SelectObject( hdcSave, hbmSave ) ;

         hdcMem = CreateCompatibleDC( hdc ) ;

         for (i = 0 ; i < _NUM_COLORS ; i++)
         {
            if (i == 8 || i == 9)
            {
               hbmNew = LoadBitmap( hinstApp, "BMP2" ) ;
               bmColorTranslate( hbmNew, RGBLTGRAY,
                                       rgcsColors[i], 0 ) ;

               bmColorTranslate( hbmNew, RGBGRAY,
                                 GetShadowColor( rgcsColors[i] ), 0 ) ;

               bmColorTranslate( hbmNew, RGBWHITE,
                                 GetHighlightColor( rgcsColors[i] ), 0 ) ;


               bmColorTranslate( hbmNew, RGBLTCYAN,
                                 RGBBLACK, 0 ) ;

               bmColorTranslate( hbmNew, RGBCYAN,
                                 RGBLTGRAY, 0 ) ;
            }
            else
            {
               hbmNew = LoadBitmap( hinstApp, "BMP1" ) ;

               bmColorTranslate( hbmNew, RGBLTGRAY,
                                       rgcsColors[i], 0 ) ;
      
               bmColorTranslate( hbmNew, RGBGRAY,
                                 GetShadowColor( rgcsColors[i] ), 0 ) ;

               bmColorTranslate( hbmNew, RGBWHITE,
                                 GetHighlightColor( rgcsColors[i] ), 0 ) ;

               bmColorTranslate( hbmNew, RGBLTGREEN,
                                 RGBBLACK, 0 ) ;

               bmColorTranslate( hbmNew, RGBGREEN,
                                 RGBLTGRAY, 0 ) ;
            }


            hbmNew = SelectObject( hdcMem, hbmNew ) ;

            BitBlt( hdcSave, i*BM.bmWidth, 0,
                           BM.bmWidth, BM.bmHeight, hdcMem, 0, 0, SRCCOPY ) ;

            hbmNew = SelectObject( hdcMem, hbmNew ) ;

            DeleteObject( hbmNew ) ;
         }

         hbmSave = SelectObject( hdcSave, hbmSave ) ;
         DeleteDC( hdcSave ) ;

         DeleteDC( hdcMem ) ;

         ReleaseDC( hWnd, hdc ) ;
      }
      break ;

      #if 0
      case WM_ERASEBKGND:
      {
         HBRUSH hbr ;
         HBRUSH hbrOrig ;

         hbrOrig = SetClassWord( hWnd, GCW_HBRBACKGROUND, hbr = CreateSolidBrush( RGBCYAN ) ) ;

         DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

         SetClassWord( hWnd, GCW_HBRBACKGROUND, hbrOrig ) ;

         DeleteObject( hbr ) ;
      }
      break ;

      #endif

#if 0
      case WM_RBUTTONDOWN:
      {
         HDC      hDC ;
         HBITMAP  hbmOld ;
         BITMAP   BM ;

         HDC      hdcMem ;

         GetObject( hbmNew, sizeof( BITMAP ), &BM ) ;

         if (hDC = GetDC( hWnd ))
         {
            hdcMem = CreateCompatibleDC( hDC ) ;

            hbmOld = SelectObject( hdcMem, hbmNew ) ;

            BitBlt( hDC, LOWORD( lParam ), HIWORD( lParam ),
                        BM.bmWidth, BM.bmHeight, hdcMem, 0, 0, SRCCOPY ) ;

            SelectObject( hdcMem, hbmOld ) ;
            DeleteDC( hdcMem ) ;

            ReleaseDC( hWnd, hDC ) ;
         }

      }
      break ;

      case WM_LBUTTONDOWN:
      {
         HDC      hDC ;
         HBITMAP  hbmOld ;
         BITMAP   BM ;

         HDC      hdcMem ;

         HDC      hdcTemp ;
         HBITMAP  hbmTemp ;

         GetObject( hbmNew, sizeof( BITMAP ), &BM ) ;

         if (hDC = GetDC( hWnd ))
         {
            hdcTemp = CreateCompatibleDC( hDC );
            hbmTemp = CreateCompatibleBitmap( hDC, BM.bmWidth, BM.bmHeight ) ;
            hbmTemp = SelectObject( hdcTemp, hbmTemp ) ;

            BitBlt( hdcTemp, 0, 0, BM.bmWidth, BM.bmHeight, hDC,
                    LOWORD( lParam ), HIWORD( lParam ), SRCCOPY ) ;

            hdcMem = CreateCompatibleDC( hDC ) ;

            hbmOld = SelectObject( hdcMem, hbmNew ) ;

            SetBkColor( hdcTemp, RGBLTGRAY ) ; 

            bmTransBlt( hdcTemp, 0, 0, 
                        BM.bmWidth, BM.bmHeight, hdcMem, 0, 0 ) ;

            SelectObject( hdcMem, hbmOld ) ;
            DeleteDC( hdcMem ) ;

            BitBlt( hDC, LOWORD( lParam ), HIWORD( lParam ),
                        BM.bmWidth, BM.bmHeight, hdcTemp, 0, 0, SRCCOPY ) ;

            DeleteObject( SelectObject( hdcTemp, hbmTemp ) ) ;
            DeleteDC( hdcTemp ) ;

            ReleaseDC( hWnd, hDC ) ;
         }

      }
      break ;

#endif               

	   case WM_COMMAND:
         switch (wParam)
         {

            case IDM_ABOUT:
            {
               FARPROC lpfnAbout ;
               lpfnAbout = MakeProcInstance( fnAbout, GETHINST( hWnd ) ) ;

      		   DialogBox(  GETHINST( hWnd ),		
		                     "ABOUTBOX",	 
                           hWnd,			 
                           lpfnAbout ) ;

               FreeProcInstance( lpfnAbout ) ;
            }
            break;


            default:
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         }

         //
         // Force a repaint
         //
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         BITMAP   BM ;
         HDC      hdcMem ;
         UINT     i ;
         HDC      hdc ;
         HBITMAP hbmNew ;

         GetObject( hbmSave, sizeof( BITMAP ), &BM ) ;

         hdc = BeginPaint( hWnd, &ps ) ;

         hdcMem = CreateCompatibleDC( hdc ) ;

         hbmSave = SelectObject( hdcMem, hbmSave ) ;

         BitBlt( hdc, LOWORD( lParam ), HIWORD( lParam ),
                        BM.bmWidth, BM.bmHeight, hdcMem, 0, 0, SRCCOPY ) ;

         hbmSave = SelectObject( hdcMem, hbmSave ) ;

         DeleteDC( hdcMem ) ;

         EndPaint( hWnd, &ps ) ;
      }
      break ;

	   case WM_DESTROY:

         if (hbmSave)
            DeleteObject( hbmSave ) ;

	      PostQuitMessage( 0 ) ;
	      break;

	   default:		
	      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }
   return 0L ;
}

/********************************************************************
 *  Utility function to paint the screen.
 ********************************************************************/

VOID NEAR PASCAL
PrintALine( HDC hDC, short y, LPSTR lpsz, short ptsize, WORD wFlags ) ;


BOOL NEAR PASCAL PaintInfo( HWND hWnd ) 
{
   short x, y ;
   short xChar, yChar ;
   char szBuffer[256] ;
   HFONT    hFont ;

   TEXTMETRIC  tm ;
   PAINTSTRUCT ps ;
   HANDLE      hMod ;

   if (!BeginPaint( hWnd, &ps ))
      return FALSE ;

   x = 0 ;
   y = 1 ;

   SetBkColor( ps.hdc, GetSysColor( COLOR_WINDOW ) ) ;

   return EndPaint( hWnd, &ps ) ;
}


/********************************************************************
 *  InitApplication - Initialize the Class and register it
 ********************************************************************/
BOOL NEAR PASCAL InitApplication( HANDLE hInstance )
{
    WNDCLASS  wc;

    wc.style         = NULL ;      
    wc.lpfnWndProc   = fnMainWnd;  
                                   
    wc.cbClsExtra    = 0;             
    wc.cbWndExtra    = 0;             
    wc.hInstance     = hInstance;      
    wc.hIcon         = LoadIcon( hInstance, szAppName );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = COLOR_HIGHLIGHT + 1 ;
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/********************************************************************
 *   Initialize this instance.
 ********************************************************************/
BOOL NEAR PASCAL InitInstance(   HANDLE hInstance,
                                 int    nCmdShow )
{
   HWND            hWnd;      

   short          x, y ;
   short          xChar, yChar ;
   HDC            hDC ;

   x = CW_USEDEFAULT ;
   y = CW_USEDEFAULT ;

   if (hDC = GetDC( NULL ))
   {
      TEXTMETRIC     tm ;

      SelectObject( hDC, GetStockObject( SYSTEM_FIXED_FONT ) ) ;
      GetTextMetrics( hDC, &tm ) ;
      xChar = (tm.tmAveCharWidth) ;
      yChar = (tm.tmHeight + tm.tmExternalLeading ) ;
      ReleaseDC( NULL, hDC ) ;
   }


   hWnd = CreateWindow( szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        x, y,
                        xChar * NUMCHARSX, yChar * NUMCHARSY,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;


   if (!hWnd)
       return FALSE ;

   hinstApp = hInstance ;

   ShowWindow( hWnd, nCmdShow ) ;

   UpdateWindow( hWnd ) ;

   return TRUE ;             
}


/****************************************************************
 *  BOOL FAR PASCAL
 *    fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    About box window procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
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
	      break ;
    }

   return FALSE ;
} /* fnAbout()  */
