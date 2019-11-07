/********************************************************************
 *
 *    Test program to demonstrate the use of the NEWEXE module.
 *
 *    For documentation on the functions in the NEWEXE module
 *    see the source code comments in  NEWEXE.C.
 *
 ********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

//
// testd.h includes dialog box and other resource defines
//
#include "testd.h"

#include "newexe.h" 

#include "fileopen.h"

/********************************************************************
 *   Global variables
 ********************************************************************/

char szAppName [] = "Test" ;
char szFName[144] ;
char szFileSpec[] = "*.EXE" ;

WORD  wNumSegs ;
WORD  wOpSys ;
char  *rgszOpSys[] = { "Unknown", "OS/2", "Windows", "MS-DOS 4.x", "Win 386" } ;

WORD  wFlags ;

char  szDesc[512] ;  // module description


/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL PaintInfo( HDC hDC, int xChar, int yChar ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;



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

   switch (wMsg)
   {

	   case WM_COMMAND:
         switch (wParam)
         {
            case IDM_FILE:
            {
               int fh ;
               NEWEXEHDR NewExe ;
               OLDEXEHDR OldExe ;

               FileOpenDialog( hWnd,
                             "Open a File",
                              szFileSpec,
                              DLGOPEN_MUSTEXIST |
                              DLGOPEN_OPEN,
                              szFName,
                              143 ) ;

               if (fh = newexeOpenNewExe( szFName, &OldExe, &NewExe ))
               {
                  LONG     lNonResOffset ;
                  BYTE     cb ;


                  wNumSegs = newexeNUMSEGS( NewExe ) ;
                  wFlags   = newexeOTHERFLAGS( NewExe ) ;
                  wOpSys   = newexeOPSYS( NewExe ) ;

                  // Get module description
                  // cbNonResTable = newexeCBNONRES( NewExe ) ;
                  lNonResOffset = newexeNONRESOFFSET( NewExe ) ;

                  lseek( fh, lNonResOffset, SEEK_SET ) ;

                  //
                  // Get length of desc
                  //

                  if (_lread( fh, (LPSTR)&cb, sizeof(BYTE) ) == sizeof(BYTE))
                  {
                     _lread( fh, szDesc, cb ) ;
                     szDesc[cb] = '\0' ;
                  }

                  _lclose( fh ) ;
               }
               else
               {
                  char szErr[256] ;

                  wNumSegs = 0 ;
                  if (errno == INVALIDEXE)
                     wsprintf( szErr,
                               "%s does not have the new executable format",
                               (LPSTR)szFName ) ;
                  else
                     wsprintf( szErr, "There was an error reading %s",
                               (LPSTR) szFName ) ;
                  MessageBox( hWnd, szErr, "New EXE Demo", MB_ICONEXCLAMATION ) ;
               }
            }
            break ;


            case IDM_ABOUT:
            {
               FARPROC lpfnAbout ;
               char szBuf[80] ;

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
BOOL NEAR PASCAL PaintInfo( HDC hDC, int xChar, int yChar )
{
   short x, y ;
   char szBuffer[256] ;

   x = y = 1 ;

   SelectObject( hDC, GetStockObject( SYSTEM_FIXED_FONT ) ) ;
   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"      File Name: %s",
                      (LPSTR)szFName ) ) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"   Module Desc.: %s",
                      (LPSTR)szDesc ) ) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"   Target OpSys: %s",
                      (LPSTR)rgszOpSys[wOpSys] ) ) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"       Font Bit: %s",
                      (LPSTR)(wFlags & 0x0002 ? "On" : "Off") ) ) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"     Memory Bit: %s",
                      (LPSTR)(wFlags & 0x0004 ? "On" : "Off") ) ) ;
   y++ ;

   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Windows 3.0 Bit: %s",
                      (LPSTR)(wFlags & 0x0008 ? "On" : "Off") ) ) ;
   y++ ;


   TextOut( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"   Num Segments: %d",
                      wNumSegs ) ) ;
   y++ ;

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

   int             x, y ;

   x = GetSystemMetrics( SM_CXSCREEN ) ;
   y = GetSystemMetrics( SM_CYSCREEN ) ;

   hWnd = CreateWindow( szAppName,
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
