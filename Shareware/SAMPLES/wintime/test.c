/********************************************************************
 *
 *    Test program to demonstrate the use of the WINTIME module.
 *
 *    For documentation on the functions in the WINTIME module
 *    see the source code comments in  WINTIME.C.
 *
 ********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include "test.h"	  
#include "testd.h"
#include "wintime.h" 

//
// hInst is imported by the WINTIME.C module!
//
HANDLE   hinstApp ;

char     szAppName [] = "Test" ;

/********************************************************************
 *   Global variables
 ********************************************************************/

   BOOL fShortDate = FALSE ;
   BOOL fNoDayofWeek = FALSE ;

   BOOL fForce24Hour = FALSE ;
   BOOL fNoSeconds = FALSE ;

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
LONG FAR PASCAL MainWndProc( HWND hWnd, 
                             WORD wMsg,  
                             WORD wParam,			 
                             LONG lParam )			  
{

   switch (wMsg)
   {
      case WM_CREATE:
         //
         // It is important that you initiialize the WinTime module
         // by calling this function.
         //
         TimeResetInternational() ;
      break ;

      case WM_WININICHANGE:
         //
         // If the user changes the International settings in control
         // panel, WM_WININICHANGE will be sent.  In this case you
         // need to reset the WinTime.C module.
         //
         if (!lstrcmpi( "intl", (LPSTR)lParam ))
         {
            TimeResetInternational() ;
            InvalidateRect( hWnd, NULL, TRUE ) ;
         }
      break ;


	   case WM_COMMAND:
         switch (wParam)
         {
            case IDM_SHORTDATE:
               if (fShortDate)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_SHORTDATE,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_SHORTDATE,
                              MF_BYCOMMAND | MF_CHECKED ) ;
               }
               fShortDate = !fShortDate ;
               break ;

            case IDM_NODAYOFWEEK:
               if (fNoDayofWeek)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_NODAYOFWEEK,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_NODAYOFWEEK,
                              MF_BYCOMMAND | MF_CHECKED ) ;
               }
               fNoDayofWeek = !fNoDayofWeek ;
               break ;


            case IDM_FORCE24HOUR:
               if (fForce24Hour)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_FORCE24HOUR,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_FORCE24HOUR,
                              MF_BYCOMMAND | MF_CHECKED ) ;
               }
               fForce24Hour = !fForce24Hour ;
               break ;

            case IDM_NOSECONDS:
               if (fNoSeconds)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_NOSECONDS,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_NOSECONDS,
                              MF_BYCOMMAND | MF_CHECKED ) ;
               }
               fNoSeconds = !fNoSeconds ;
               break ;

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

	   case WM_DESTROY:
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
BOOL PaintInfo( HDC hDC, int xChar, int yChar )
{
   char szBuffer[256] ;
   char szTime[80] ;
   char szDate[80] ;
   WORD w ;
   int x, y ;

   x = 1 ;
   y = 1 ;

   w = 0 ;

   if (fForce24Hour)
      w = TIME_24HOUR ;

   if (fNoSeconds)
      w |= TIME_NOSECONDS ;

   TimeGetCurTime( szTime, w ) ;

   w = 0 ;

   if (fNoDayofWeek)
      w = DATE_NODAYOFWEEK ;

   if (fShortDate)
      w |= DATE_SHORTDATE ;

   TimeGetCurDate( szDate, w ) ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Time: %s",
                      (LPSTR)szTime ) ) ;
   y++ ;


   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Date: %s",
                      (LPSTR)szDate ) ) ;
   y++ ;

   return TRUE ;
}

/********************************************************************
 *  InitApplication - Initialize the Class and register it
 ********************************************************************/
BOOL InitApplication( HANDLE hInstance )
{
    WNDCLASS  wc;

    wc.style         = NULL ;      
    wc.lpfnWndProc   = MainWndProc;  
                                   
    wc.cbClsExtra    = 0;             
    wc.cbWndExtra    = 0;             
    wc.hInstance     = hInstance;      
    wc.hIcon         = LoadIcon( hInstance, szAppName );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( GRAY_BRUSH ); 
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/********************************************************************
 *   Initialize this instance.
 ********************************************************************/
BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
   HWND            hWnd;               /* Main window handle.                */

   int             x, y ;

   hinstApp = hInstance;

   x = GetSystemMetrics( SM_CXSCREEN ) ;
   y = GetSystemMetrics( SM_CYSCREEN ) ;

   hWnd = CreateWindow(   szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, y / 3,
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

