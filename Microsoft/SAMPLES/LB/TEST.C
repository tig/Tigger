/********************************************************************
 *
 *    Test program to demonstrate the use of the LB module.
 *
 *    For documentation on the functions in the LB module
 *    see the source code comments in  LB.C.
 *
 ********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include <time.h>
//#include <wdb.h>

#include "lb.h"

//
// testd.h includes dialog box and other resource defines
//
#include "testd.h"

//
// Define these to set how big the window is when it is created
//
#define NUMCHARSX 80
#define NUMCHARSY 25

#define APPDESC "Demonstrates ListBox Routines"

/********************************************************************
 *   Global variables
 ********************************************************************/

char     szAppName [] = "LBTest" ;
HANDLE   hAppInstance ;

#define IDD_ODLB     42

/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL PaintInfo( HDC hDC, int xChar, int yChar ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;

int nLogPixelsY ;


/********************************************************************
 *   Useful macros
 ********************************************************************/
#define GETHINST(hwnd)     (GetWindowWord( hwnd, GWW_HINSTANCE ))

/********************************************************************
 *   WinMain function - Entry point to applcation
 ********************************************************************/
int PASCAL WinMain(  HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpszCmdLine,		  
                     int      nCmdShow )			  
{
   MSG msg;			  
                        
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
   static HWND hwndLB ;
   static HFONT hFont ;

   switch (wMsg)
   {
      case WM_CREATE:

         #if 0
         D( if (hWDB = wdbOpenSession( hWnd,
                                       szAppName, "lb.ini", WDB_LIBVERSION ))
            {
               //DPS( hWDB, "WDB is active!" ) ;
            }
            else
            {
              // DPS( hWDB, "WDB failed!" ) ;
            }
         ) ;
         #endif

         hwndLB = CreateWindow( "LISTBOX",
                                 "OwnerDraw...",
                                 WS_CHILD | WS_BORDER |
                                 WS_VSCROLL | 
                                 0x1000L | //LBS_DISABLENOSCROLL
                                 LBS_NOTIFY | LBS_MULTIPLESEL |
                                 LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS,
                                 0,
                                 0,
                                 2,
                                 3,
                                 hWnd,
                                 IDD_ODLB,
                                 GetWindowWord( hWnd, GWW_HINSTANCE ),
                                 NULL ) ;

         hFont = CreateFont(
                             -MulDiv( 8, nLogPixelsY, 72 ),
                             0,
                             0,
                             0,
                             FW_NORMAL,
                             FALSE,
                             FALSE,
                             FALSE,
                             ANSI_CHARSET,
                             OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY,
                             0,
                             "Helv"

                           ) ;

         SendMessage( hwndLB, WM_SETFONT, hFont, 0L ) ;

         lbReset( hwndLB ) ;

         lbFill( hwndLB, "LB.INI", 0 ) ;

         ShowWindow( hwndLB, SW_NORMAL ) ;
      break ;

      case WM_SETFOCUS:
         SetFocus( hwndLB ) ;
      break ;

      case WM_DRAWITEM:
      case WM_MEASUREITEM:
         return lbOwnerDraw( hWnd, wMsg, wParam, lParam ) ;
      break ;

	   case WM_COMMAND:
         switch (wParam)
         {
            case IDD_ODLB:
               if (lbProcessCmd( LOWORD( lParam ), HIWORD( lParam ) ))
                  return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
            break ;

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
         TEXTMETRIC  tm ;
         int         xChar, yChar ;

         if (BeginPaint (hWnd, &ps))
         {

            GetTextMetrics( ps.hdc, &tm ) ;

            xChar = tm.tmAveCharWidth ;
            yChar = tm.tmHeight + tm.tmExternalLeading ;

            PaintInfo( ps.hdc, xChar, yChar) ;

            EndPaint (hWnd, &ps) ;
         }
      }
      break ;

      case WM_SIZE:
         if (wParam == SIZENORMAL || wParam == SIZEZOOMSHOW)
            MoveWindow( hwndLB, 0, 0,
                        LOWORD( lParam ), HIWORD( lParam ), TRUE ) ;
      break ;
         

	   case WM_DESTROY:
         lbReset( hwndLB ) ;
         //D( hWDB = wdbCloseSession( hWDB ) ) ;
         DeleteObject( hFont ) ;
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
BOOL NEAR PASCAL PaintInfo( HDC hDC, int xChar, int yChar )
{
   short x, y ;
   char szBuffer[256] ;

   x = y = 1 ;

   SelectObject( hDC, GetStockObject( SYSTEM_FIXED_FONT ) ) ;
   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;

   #if 0
   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"      File Name: %s",
                      (LPSTR)szFName ) ) ;
   y++ ;
   #endif

   return TRUE ;
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
    wc.hbrBackground = COLOR_WINDOW + 1 ;
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

   hAppInstance = hInstance ;

   x = CW_USEDEFAULT ;
   y = CW_USEDEFAULT ;

   if (hDC = GetDC( NULL ))
   {
      TEXTMETRIC     tm ;

      nLogPixelsY = GetDeviceCaps( hDC, LOGPIXELSY ) ;

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
      case WM_INITDIALOG:
         SetDlgItemText( hDlg, 101, APPDESC ) ;
         return TRUE ;

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
