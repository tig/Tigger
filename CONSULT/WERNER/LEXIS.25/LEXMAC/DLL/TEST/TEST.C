// test 
//

#include "windows.h"	  
#include <stdlib.h>
#include "test.h"	  
#include "testd.h"
#include "..\lexmac.h" 

HANDLE   hInst;
char     szAppName [] = "Test" ;
char szClient[64] ;
char szPages[64] ;
char szTimeOut[64] ;
char szMacro[64] ;
char szSelection [64] ;

BOOL PaintInfo( HDC hDC, int xChar, int yChar ) ;

int PASCAL WinMain(  HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR lpCmdLine,		  
                     int nCmdShow )			  
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
    wc.hbrBackground = GetStockObject( LTGRAY_BRUSH ); 
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
   HWND            hWnd;               /* Main window handle.                */

   int             x, y ;

   hInst = hInstance;

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

long FAR PASCAL MainWndProc( HWND     hWnd, 
                             unsigned iMessage,  
                             WORD wParam,			 
                             LONG lParam )			  
{
   HDC      hDC ;
   HANDLE   hFont ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;
static   int xChar, yChar ;
   static BOOL fAllowSetup = TRUE ;

   switch (iMessage)
   {
      case WM_CREATE:
      {
         lstrcpy( szClient, "<none>" ) ;
         lstrcpy( szMacro, "<none>" ) ;
         lstrcpy( szSelection, "<none>" ) ;
         lstrcpy( szPages, "5" ) ;
         lstrcpy( szTimeOut, "15" ) ;

         hDC = GetDC (hWnd) ;
         hFont = CreateFont(  6, 6, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                              OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS,
                              PROOF_QUALITY,
                              FIXED_PITCH,
                              "Courier" ) ;

         SelectObject( hDC, hFont ) ;

         GetTextMetrics( hDC, &tm ) ;
         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;
         DeleteObject (SelectObject (hDC, hFont)) ;
         ReleaseDC (hWnd, hDC) ;

         break ;
      }

	   case WM_COMMAND:
         switch (wParam)
         {
            case IDM_ABOUT:
               AboutDialog() ;
               break;

            case IDM_MAIN:
            {
               if (!SelectMacroDialog( szMacro,
                                             szSelection,
                                             szClient,
                                             szPages,
                                             szTimeOut ))
                  MessageBox( hWnd, "SelectMacroDialog returned false", szAppName, MB_ICONINFORMATION ) ;
            }
               break;

            default:
               return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
         }
         InvalidateRect( hWnd, NULL, TRUE ) ;
         break ;

      case WM_PAINT:
            hDC = BeginPaint (hWnd, &ps) ;

            hFont = CreateFont(  6, 6, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                                 OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS,
                                 PROOF_QUALITY,
                                 FIXED_PITCH,
                                 "Courier" ) ;

            SelectObject( hDC, hFont ) ;

            SetBkMode( hDC, OPAQUE ) ;
            SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
            PaintInfo( hDC, xChar, yChar) ;

            DeleteObject (SelectObject (hDC, hFont)) ;
            EndPaint (hWnd, &ps) ;
         break ;

	   case WM_DESTROY:
	      PostQuitMessage( 0 ) ;
	      break;

	   default:		
	      return DefWindowProc( hWnd, iMessage, wParam, lParam ) ;
   }
   return 0L ;
}


BOOL PaintInfo( HDC hDC, int xChar, int yChar )
{
   char szBuffer[256] ;

   int x, y ;

   x = 1 ;
   y = 1 ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Macro: '%s'",
                      (LPSTR)szMacro ) ) ;

   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Client: '%s'",
                      (LPSTR)szClient ) ) ;

   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Selection: '%s'",
                      (LPSTR)szSelection ) ) ;

   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Pages: %s",
                      (LPSTR)szPages ) ) ;

   TextOut( hDC, xChar * x, yChar * ++y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current TimeOut: %s",
                      (LPSTR)szTimeOut ) ) ;
   return TRUE ;
}


