/********************************************************************
 *
 *    minitask
 *
 *    This application demonstrates activating another app.  It uses
 *    the same technique that TASKMAN.EXE does.
 *
 *    8/26/91 cek
 *
 ********************************************************************/
#include <windows.h>
#include <stdlib.h>

#include <wdb.h>

#include "resource.h"

#define IDD_TASKLISTBOX 42

//
// Define these to set how big the window is when it is created
//
#define NUMCHARSX 25
#define NUMCHARSY 15

#define APPDESC "Task Switcher Demo"

/********************************************************************
 *   Global variables
 ********************************************************************/

char     szAppName [] = "MiniTask" ;
HANDLE   hAppInstance ;
short          xChar, yChar ; // system font size

/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL PaintInfo( HDC hDC, int xChar, int yChar ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;

int FAR PASCAL
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;

/********************************************************************
 *   Useful macros
 ********************************************************************/
#define GETHINST(hwnd)     (GetWindowWord( hwnd, GWW_HINSTANCE ))


/****************************************************************
 *  HWND NEAR PASCAL GetTopLevelCreatorWnd( HWND hwnd )
 *
 *  Description: 
 *
 *    This function returns the topmost owner of the given window.
 *
 *  Comments:
 *
 ****************************************************************/
HWND NEAR PASCAL GetTopLevelCreatorWnd( HWND hwnd )
{
   while (GetWindow( hwnd, GW_OWNER ))
     hwnd = GetWindow( hwnd, GW_OWNER ) ;

   return hwnd ;
} /* GetTopLevelCreatorWnd()  */


/****************************************************************
 *  VOID NEAR PASCAL ActivateTheSucker( HWND hwndGo )
 *
 *  Description: 
 *
 *    This function activates a given window.  The code for this
 *    function was derived from the Windows 3.0 TaskMan source,
 *    but has been re-written to use only documented functions
 *    and structures, thus it is ok for distribution...
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL ActivateTheSucker( HWND hwndGo )
{
   HWND  hwndLast ;
   DWORD lTemp ;

   if (!IsWindow( hwndGo ))
      goto Beep ;


   /* Switch to that task. */

   //
   // Activate the toplevel window of the task
   //
   hwndLast = GetLastActivePopup( hwndGo ) ;

   DP( hWDB, "hwndLast = %04X", hwndLast ) ;

   if (!IsWindow( hwndLast ))
      goto Beep ;

   /* But only if it isn't disabled. */
   lTemp = GetWindowLong( hwndLast, GWL_STYLE ) ;

   if (!(lTemp & WS_DISABLED))
   {
      HANDLE   hDWP ;
      HWND     hwndTop;
      BOOL     fMany;

      DP( hWDB, "   Not Disabled" ) ;

      fMany = (hwndTop = GetTopLevelCreatorWnd( hwndLast )) != hwndLast ;

      DP( hWDB, "   fMany = %d, hwndTop = %04X", fMany, hwndTop ) ;

      hDWP = BeginDeferWindowPos( 2 ) ;

      if (fMany)
      {
         hDWP = DeferWindowPos( hDWP, hwndLast, (HWND)HWND_TOP, 0, 0, 0, 0,
                                 SWP_NOSIZE | SWP_NOMOVE ) ;

         hDWP = DeferWindowPos( hDWP, hwndTop, (HWND)HWND_TOP, 0, 0, 0, 0,
                                SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE ) ;
      }
      else
      {
         hDWP = DeferWindowPos( hDWP, hwndTop, (HWND)HWND_TOP, 0, 0, 0, 0,
                                /*SWP_NOACTIVATE | */SWP_NOSIZE | SWP_NOMOVE ) ;
      }

      EndDeferWindowPos( hDWP ) ;

      if (IsIconic( hwndLast ))
      {
         DP1( hWDB, "   IsIconic" ) ;
         SendMessage( hwndLast, WM_SYSCOMMAND, SC_RESTORE, 0L ) ;
      }
      else
      {
         DP1( hWDB, "   !IsIconic" ) ;
         SetFocus( hwndLast ) ;
      }
   }
   else
   {
      DP( hWDB, "   Disabled" ) ;
Beep:
      MessageBeep(0);
   }


} /* ActivateTheSucker()  */

/********************************************************************
 *   WinMain function - Entry point to applcation
 ********************************************************************/
int PASCAL WinMain(  HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpszCmdLine,		  
                     int      nCmdShow )			  
{
   MSG msg;			  
                        
   hAppInstance = hInstance ;

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

   switch (wMsg)
   {
      case WM_CREATE :

         hwndLB = CreateWindow(  "listbox", "MiniTaskLB",
                                 WS_CHILD |
                                 WS_VISIBLE |
                                 WS_VSCROLL |
                                 LBS_HASSTRINGS |
                                 LBS_NOINTEGRALHEIGHT |
                                 LBS_NOTIFY,
                                 0, 0, 0, 0,    // size it later...
                                 hWnd,
                                 IDD_TASKLISTBOX,
                                 GetWindowWord( hWnd, GWW_HINSTANCE ),
                                 NULL ) ;
                                
      break ;

      case WM_ACTIVATEAPP:

         //!! FALL THROUGH !!

      case WM_SETFOCUS:
      {
         char szTemp[40] ;
         HWND hwndNext ;
         short nIndex ;

         SendMessage( hwndLB, WM_SETREDRAW, FALSE, 0L ) ;
         SendMessage( hwndLB, LB_RESETCONTENT, 0, 0L ) ;

         hwndNext = GetWindow( hWnd, GW_HWNDFIRST ) ;
         while (hwndNext)
         {
            if ((hwndNext != hWnd) &&
                (IsWindowVisible( hwndNext )) &&
                (!GetWindow( hwndNext, GW_OWNER )))
            {
               if (GetWindowText( hwndNext, (LPSTR)szTemp, sizeof(szTemp) ))
               {
                  nIndex = (short)SendMessage(hwndLB, LB_ADDSTRING, 0, (DWORD)(LPSTR)szTemp ) ;
                  SendMessage( hwndLB, LB_SETITEMDATA, nIndex, (DWORD)hwndNext) ;
               }
            }
            hwndNext = GetWindow( hwndNext, GW_HWNDNEXT ) ;
         }
         SendMessage( hwndLB, WM_SETREDRAW, TRUE, 0L ) ;
      }
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

	   case WM_COMMAND:
      {
         short nIndex ;
         HWND  hwndGo ;

         switch (wParam)
         {
   	      case IDD_TASKLISTBOX:
	      	   switch (HIWORD(lParam))
		         {
      		      case LBN_DBLCLK:
                     nIndex = (short)SendMessage(hwndLB, LB_GETCURSEL, 0, 0L ) ;
                     hwndGo = (HWND)SendMessage(hwndLB, LB_GETITEMDATA, nIndex, 0L ) ;

      		      	ActivateTheSucker( hwndGo ) ;
                  break;

                  default:
			            return(FALSE);
		         }
		      break;


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
      }
      break ;

      case WM_SIZE:
         SetWindowPos( hwndLB, NULL, 0, 0,
                          LOWORD( lParam ), HIWORD( lParam ),
                          SWP_NOACTIVATE |
                          SWP_NOZORDER ) ;
      break ;

	   case WM_DESTROY:

         if (IsWindow( hwndLB ))
            DestroyWindow( hwndLB ) ;
	      PostQuitMessage( 0 ) ;
	      break;

	   default:		
	      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }
   return 0L ;
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
   HDC            hDC ;

   x = CW_USEDEFAULT ;
   y = CW_USEDEFAULT ;

   if (hDC = GetDC( NULL ))
   {
      TEXTMETRIC     tm ;

      SelectObject( hDC, GetStockObject( SYSTEM_FONT ) ) ;
      xChar = GetTextMetricsCorrectly( hDC, &tm ) ;
      yChar = (tm.tmHeight + tm.tmExternalLeading ) ;
      ReleaseDC( NULL, hDC ) ;
   }


   hWnd = CreateWindow( szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        x, y,
                        xChar * NUMCHARSX + (GetSystemMetrics( SM_CXFRAME ) * 2),
                        yChar * NUMCHARSY + (GetSystemMetrics( SM_CYFRAME ) * 2 +
                                             GetSystemMetrics( SM_CYCAPTION) +
                                             GetSystemMetrics( SM_CYMENU )),
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

/****************************************************************
 *  int FAR PASCAL
 *    GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTextMetrics )
 *
 *  Description: 
 *    
 *    This function gets the textmetrics of the font currently
 *    selected into the hDC.  It returns the average char width as
 *    the return value.
 *
 *    This function computes the average character width correctly
 *    by using GetTextExtent() on the string "abc...xzyABC...XYZ"
 *    which works out much better for proportional fonts.
 *
 *    Note that this function returns the same TEXTMETRIC
 *    values that GetTextMetrics() does, it simply has a different
 *    return value.
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM )
{
   short  nAveWidth ;
   char   rgchAlphabet[52] ;
   short  i ;

   //
   // Get the TM of the current font.  Note that GetTextMetrics
   // gets the incorrect AveCharWidth value for proportional fonts.
   // This is the whole poshort in this exercise.
   //
   GetTextMetrics(hDC, lpTM);

   //
   // If it's not a variable pitch font GetTextMetrics was correct
   // so just return.
   //
   if (!(lpTM->tmPitchAndFamily & FIXED_PITCH))  
      return lpTM->tmAveCharWidth ;
   else
   {
      for ( i = 0 ; i <= 25 ; i++)
         rgchAlphabet[i] = (char)(i+(short)'a') ;

      for ( i = 0 ; i<=25 ; i++)
         rgchAlphabet[i+25] = (char)(i+(short)'A') ;

      nAveWidth = LOWORD( GetTextExtent( hDC,
                             (LPSTR)rgchAlphabet, 52 ) ) / 52 ;
   }

   //
   // Return the calculated average char width
   //
   return nAveWidth ;

} /* GetTextMetricsCorrectly()  */
