/********************************************************************
 *
 *    Test program to demonstrate the use of the SELPRN module.
 *
 *    For documentation on the functions in the SELPRN module
 *    see the source code comments in  SELPRN.C.
 *
 ********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include "test.h"	  
#include "testd.h"
#include "selprn.h" 

HANDLE   hInst;
char     szAppName [] = "Test" ;

/********************************************************************
 *   Global variables to hold the printer information
 ********************************************************************/
char szDevice[32] ;        // Printer drivers use 32 characters max.
                           // for the device name (PCL/HP LaserJet)

char szDriver[_MAX_PATH] ; // Name of the driver file (HPPCL.DRV)
                           // Could possibly have a full path.

char szPort[_MAX_PATH] ;   // The port can be a printer/com port or
                           // a file name.

BOOL fAllowSetup = TRUE ;  // Remeber if we want the Setup... button

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
         lstrcpy (szDevice, "" ) ;
         lstrcpy( szDriver, "" ) ;
         lstrcpy( szPort, "" ) ;

         //
         // DoSelPrn is just a helper function in this program.  It
         // calls PrinterSelPrnDialog
         //
         DoSelPrn( hWnd, SELPRN_GETDEFAULT ) ;

      break ;

	   case WM_COMMAND:
         switch (wParam)
         {
            case IDM_GETDEFAULT:
               //
               // When called with SELPRN_GETDEFAULT, PrinterSelPrnDialog()
               // gets the current printer from the WIN.INI and does NOT
               // display the dialog box
               //
               DoSelPrn( hWnd, SELPRN_GETDEFAULT ) ;
               break ;

            case IDM_USEDEFAULT:
               //
               // If you want to force the dialog box to come up with 
               // the default printer (in the WIN.INI file) use the
               // SELPRN_USEDEFAULT flag
               //
               if (fAllowSetup)
                  DoSelPrn( hWnd, SELPRN_STANDARD |
                                  SELPRN_USEDEFAULT |
                                  SELPRN_ALLOWSETUP) ;
               else
                  DoSelPrn( hWnd, SELPRN_STANDARD |
                                  SELPRN_USEDEFAULT ) ;
               break ;

            case IDM_STANDARD:
               //
               // SELPRN_STANDARD gives the "defaults"
               //
               if (fAllowSetup)
                  DoSelPrn( hWnd, SELPRN_STANDARD | SELPRN_ALLOWSETUP) ;
               else
                  DoSelPrn( hWnd, SELPRN_STANDARD ) ;
               break ;

            case IDM_ALLOWSETUP:
               //
               // Change the menu to reflect the whether the Setup... 
               // button is enabled or not.
               //
               if (fAllowSetup)
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_ALLOWSETUP,
                              MF_BYCOMMAND | MF_UNCHECKED ) ;
               }
               else
               {
                  CheckMenuItem( GetMenu( hWnd ),
                              IDM_ALLOWSETUP,
                              MF_BYCOMMAND | MF_CHECKED ) ;
               }
               fAllowSetup = !fAllowSetup ;
               return 0L ;
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
         HDC         hDC ;
         PAINTSTRUCT ps ;
         TEXTMETRIC  tm ;
         int         xChar, yChar ;

         hDC = BeginPaint (hWnd, &ps) ;

         GetTextMetrics( hDC, &tm ) ;

         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;

         SetBkMode( hDC, OPAQUE ) ;
         SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
         PaintInfo( hDC, xChar, yChar) ;

         EndPaint (hWnd, &ps) ;
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
 *  Utility function to call the PrintSelPrnDialog() function.
 ********************************************************************/
void DoSelPrn( HWND hWnd, WORD wFlags )
{
   //
   // Call PrintSelPrnDialog() with the appropriate arguments
   // and test the return value
   //
   switch (PrintSelPrnDialog( hWnd,
                              (LPSTR)szAppName,
                              (LPSTR)szDriver,
                              (LPSTR)szDevice,
                              (LPSTR)szPort,
                              wFlags ))
   {
      case DLG_CANCEL:
         //
         // User hit the cancel button
         //
         break ;

      case DLG_OK:
         //
         // User hit OK in either the SelPrn dialog box, or in
         // the printer driver's DevMode dialog box.
         //
         break ;

      case DLG_ERROR:
         //
         // Something went wrong...
         //
         MessageBox( hWnd, "Select Printer Failed.",
                           "Error", MB_ICONEXCLAMATION ) ;
         break ;
   }
}

/********************************************************************
 *  Utility function to paint the screen.
 ********************************************************************/
BOOL PaintInfo( HDC hDC, int xChar, int yChar )
{
   char szBuffer[256] ;
   int x, y ;

   x = 1 ;
   y = 1 ;  

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Current Printer: %s (%s) on %s",
                      (LPSTR)szDevice,
                      (LPSTR)szDriver,
                      (LPSTR)szPort ) ) ;
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

