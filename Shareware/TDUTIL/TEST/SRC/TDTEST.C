/********************************************************************
 *
 *    Test program to demonstrate the use of the TDUTIL.DLL.
 *
 ********************************************************************/

#include <windows.h>
#include <stdlib.h>

#include <time.h>

#include <wdb.h>

//
// testd.h includes dialog box and other resource defines
//
#include "tdtestd.h"

#include "TDUTIL.H"


//
// Define these to set how big the window is when it is created
//
#define NUMCHARSX 80
#define NUMCHARSY 25

#define APPDESC "Demonstrates 3D Routines"

#define SHADOWWIDTH  1

/********************************************************************
 *   Global variables
 ********************************************************************/

char     szAppName [] = "Test" ;

HBRUSH   hbrBackground ;

HWND     hwndEC ;
HWND     hwndEC2 ;
HWND     hwndCB ;
HWND     hwndLB ;
HWND     hWnd ;

BOOL     bMonochrome ;

COLORREF rgbBackground ;

   typedef struct tagCOLORSTRUCT
   {
      char  *szColor ;
      DWORD rgb ;
   } COLORSTRUCT, *PCOLORSTRUCT, FAR *LPCOLORSTRUCT ;

   COLORSTRUCT rgcsColors[] =
   {
      "Red",            RGBRED,
      "Green",          RGBGREEN,
      "Blue",           RGBBLUE,
      "Brown",          RGBBROWN,
      "Magenta",        RGBMAGENTA,
      "Cyan",           RGBCYAN,
      "LightRed",       RGBLTRED,
      "LightGreen",     RGBLTGREEN,
      "LightBlue",      RGBLTBLUE,
      "Yellow",         RGBYELLOW,
      "LightMagenta",   RGBLTMAGENTA,
      "LightCyan",      RGBLTCYAN,
      "White",          RGBWHITE,
      "LightGray",      RGBLTGRAY,
      "Gray",           RGBGRAY,
      "Black",          RGBBLACK
   } ;

   #define _NUM_COLORS (sizeof( rgcsColors ) / sizeof( rgcsColors[0] ))



/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WPARAM, LPARAM);

BOOL FAR PASCAL fnAbout(  HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

BOOL FAR PASCAL fnTest(  HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam );

BOOL NEAR PASCAL PaintInfo( HDC hDC, int xChar, int yChar ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;

static HFONT hSmallFont ;

/********************************************************************
 *   Useful macros
 ********************************************************************/
#ifdef WIN32
#define GETHINST(hwnd)     (HINSTANCE)(GetWindowLong( hwnd, GWL_HINSTANCE ))
#else
#define GETHINST(hwnd)     (HINSTANCE)(GetWindowLong( hwnd, GWW_HINSTANCE ))
#endif

/********************************************************************
 *   WinMain function - Entry point to applcation
 ********************************************************************/
int PASCAL WinMain(  HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpszCmdLine,		  
                     int      nCmdShow )			  
{
   MSG msg;

   /*
    * Check the spec version to make sure we're using an up to date
    * version of the DLL...
    */
   if (!tdCheckVersion( _TDUTIL_SPEC_VER_ ))
   {
      char szMsg[256] ;
      wsprintf( szMsg, "The version of TDUTIL.DLL on this system is old.\n"\
                       "Please upgrade to version %d.00 or higher.",
                       _TDUTIL_SPEC_VER_ / 100 ) ; 

      MessageBox( NULL, szMsg, "Test App",  MB_ICONSTOP ) ;
      return FALSE ;
   }


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

//   DP( hWDB, "Normal exit" ) ;
//   D( hWDB = wdbCloseSession( hWDB ) ) ;

   return msg.wParam ;	  
}


/********************************************************************
 *   Main window procedure
 ********************************************************************/
LONG FAR PASCAL fnMainWnd( HWND hWnd, 
                             UINT wMsg,  
                             WPARAM wParam,			 
                             LPARAM lParam )			  
{

   int n ;

   switch (wMsg)
   {
      case WM_CREATE:
      {
         TEXTMETRIC  tm ;
         HDC         hDC ;
         int         xChar, yChar ;
         short       i ;
         char        szBuf[80] ;

//         D( if (hWDB = wdbOpenSession( hWnd,
//                                       szAppName, "test.ini", WDB_LIBVERSION ))
//            {
//               DPS( hWDB, "WDB is active!" ) ;
//            }
//         ) ;

         hSmallFont = CreateFont(   5,
                                    0,
                                    0,
                                    0,
                                    400,
                                    0,
                                    0,
                                    0,
                                    ANSI_CHARSET,
                                    0,
                                    0,
                                    0,
                                    VARIABLE_PITCH,
                                    "Helv" ) ;

         hDC = GetDC( hWnd ) ;

         if (GetDeviceCaps( hDC, NUMCOLORS ) <= 4)
            bMonochrome = TRUE ;
         else
            bMonochrome = FALSE ;

         SelectObject( hDC, hSmallFont ) ;
         GetTextMetrics( hDC, &tm ) ;

         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;

         hwndLB = CreateWindow( "listbox", "list box",
                                 LBS_NOTIFY |
                                 LBS_HASSTRINGS |
                                 WS_VISIBLE |
                                 WS_VSCROLL |
                                 WS_CHILD,
                                 xChar * 2, yChar * 4,
                                 xChar * 15, yChar * 6,
                                 hWnd, (HMENU)42,
                                 GETHINST( hWnd ), NULL ) ;

         SendMessage( hwndLB, WM_SETFONT, (WPARAM)hSmallFont, TRUE ) ;

         hwndEC = CreateWindow( "edit", "no border",
                                 WS_VISIBLE | WS_CHILD,
                                 xChar * 22, yChar * 4,
                                 xChar * 15, yChar + (SHADOWWIDTH * 2),
                                 hWnd, (HMENU)NULL,
                                 GETHINST( hWnd ), NULL ) ;

         SendMessage( hwndEC, WM_SETFONT, (WPARAM)hSmallFont, TRUE ) ;

         hwndEC2 = CreateWindow( "edit", "border",
                                 ES_AUTOHSCROLL |
                                 WS_BORDER |
                                 WS_VISIBLE | WS_CHILD,
                                 xChar * 38, yChar * 4,
                                 xChar * 15, yChar + //(SHADOWWIDTH * 2) +
                                    yChar / 2,
                                 hWnd, (HMENU)NULL,
                                 GETHINST( hWnd ), NULL ) ;

         SendMessage( hwndEC2, WM_SETFONT, (WPARAM)hSmallFont, TRUE ) ;

         hwndCB = CreateWindow( "combobox", "combo box",
                                 CBS_DROPDOWN |
                                 WS_VISIBLE |
                                 WS_CHILD,
                                 xChar * 60, yChar * 4,
                                 xChar * 15, yChar * 10,
                                 hWnd, (HMENU)NULL,
                                 GETHINST( hWnd ), NULL ) ;

         SendMessage( hwndCB, WM_SETFONT, (WPARAM)hSmallFont, TRUE ) ;

         for (i = 0 ; i < _NUM_COLORS ; i++)
         {
            wsprintf( szBuf, "%04X Blah!", i ) ;
            SendMessage( hwndCB, CB_ADDSTRING, 0, (LONG)(LPSTR)szBuf ) ;

            SendMessage( hwndLB, LB_ADDSTRING, 0,
               (LONG)(LPSTR)rgcsColors[i].szColor ) ;
         }

         ReleaseDC( hWnd, hDC ) ;

         rgbBackground = RGBLTGRAY ;
      }

      //
      // Fall through!
      //

      case WM_SYSCOLORCHANGE:
      {
         if (hbrBackground)
         {
            DP1( hWDB, "Deleting hbrBackground: %04X", hbrBackground ) ;
            #ifdef WIN32
            SetClassLong( hWnd, GCL_HBRBACKGROUND, COLOR_BTNFACE+1 ) ;
            #else
            SetClassLong( hWnd, GCW_HBRBACKGROUND, COLOR_BTNFACE+1 ) ;
            #endif
            DeleteObject( hbrBackground ) ;
            hbrBackground = 0 ;
         }

         hbrBackground = CreateSolidBrush( rgbBackground ) ;
         DP1( hWDB, "hbrBackground = %04X", hbrBackground ) ;
         #ifdef WIN32
         SetClassLong( hWnd, GCL_HBRBACKGROUND, (LONG)hbrBackground ) ;
         #else
         SetClassWord( hWnd, GCW_HBRBACKGROUND, (WORD)hbrBackground ) ;
         #endif
         InvalidateRect( hWnd, NULL, TRUE ) ;
      }
      break ;

      #if 0
      case WM_ERASEBKGND:
      {
         DWORD  dw ;
         #ifdef WIN32
         HBRUSH hbr = SetClassLong( hWnd, GCL_HBRBACKGROUND, hbrBackground ) ;
         #else
         HBRUSH hbr = SetClassWord( hWnd, GCW_HBRBACKGROUND, hbrBackground ) ;
         #endif

         dw = DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

         #ifdef WIN32
         SetClassLong( hWnd, GCL_HBRBACKGROUND, hbr ) ;
         #else
         SetClassWord( hWnd, GCW_HBRBACKGROUND, hbr ) ;
         #endif

         return dw ;
      }
      break ;
      #endif

     case WM_COMMAND:
     {
      #ifdef WIN32
        WORD wNotifyCode = HIWORD(wParam); /* notification code                */
        WORD wID = LOWORD(wParam);         /* item, control, or accelerator ID */
        HWND hwndCtl = (HWND) lParam;      /* handle of control                */
      #else
        WORD wNotifyCode = HIWORD(lParam) ;
        WORD wID = wParam ;
        HWND hwndCtl = (HWND)LOWORD(lParam) ;
      #endif
        
        switch (wID)
         {
            case 42:    // the list box

               switch (wNotifyCode)
               {
                  case LBN_SELCHANGE:
                     n = (int)SendMessage( hwndLB, LB_GETCURSEL, 0, 0L ) ;
                     rgbBackground = rgcsColors[n].rgb ;
                     if (hbrBackground)
                     {
                        DP1( hWDB, "Deleting hbrBackground: %04X", hbrBackground ) ;
                        #ifdef WIN32
                        SetClassLong( hWnd, GCL_HBRBACKGROUND, COLOR_BTNFACE+1 ) ;
                        #else
                        SetClassWord( hWnd, GCW_HBRBACKGROUND, COLOR_BTNFACE+1 ) ;
                        #endif

                        DeleteObject( hbrBackground ) ;
                        hbrBackground = 0 ;
                     }

                     hbrBackground = CreateSolidBrush( rgbBackground ) ;
                     #ifdef WIN32
                     SetClassLong( hWnd, GCL_HBRBACKGROUND, (LONG)hbrBackground ) ;
                     #else
                     SetClassWord( hWnd, GCW_HBRBACKGROUND, (WORD)hbrBackground ) ;
                     #endif
                     InvalidateRect( hWnd, NULL, TRUE ) ;
                  break ;
               }
               
            break ;


            case IDM_TEST:
            {
               FARPROC lpfnTest ;
               lpfnTest = MakeProcInstance( fnTest, GETHINST( hWnd ) ) ;

      		   DialogBoxParam(  GETHINST( hWnd ),		
		                          "TESTBOX",	 
                                hWnd,			 
                                lpfnTest,
                                GetSysColor( COLOR_BTNFACE ) ) ;

               FreeProcInstance( lpfnTest ) ;
            }
            break;

            default:
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         }

         //
         // Force a repaint
         //
         //InvalidateRect( hWnd, NULL, TRUE ) ;
       }
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
         if (hbrBackground)
         {
            DP1( hWDB, "Deleting hbrBackground: %04X", hbrBackground ) ;
            #ifdef WIN32
            SetClassLong( hWnd, GCL_HBRBACKGROUND, COLOR_WINDOW+1) ;
            #else
            SetClassWord( hWnd, GCW_HBRBACKGROUND, COLOR_WINDOW+1 ) ;
            #endif
            DeleteObject( hbrBackground ) ;
            hbrBackground = 0 ;
         }

         if (hSmallFont)
            DeleteObject( hSmallFont ) ;

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
   int x, y ;
   char  szBuffer[256] ;
   char  szVer[32] ;
   RECT  rc ;
   int   dx, dy ;
   TEXTMETRIC tm ;

#if 0
   tdDrawChild3DRect( hwndEC, SHADOWWIDTH, DRAW3D_IN ) ;
   tdDrawChild3DRect( hwndEC2, SHADOWWIDTH, DRAW3D_IN ) ;
   tdDrawChild3DRect( hwndCB, SHADOWWIDTH, DRAW3D_IN ) ;
   tdDrawChild3DRect( hwndLB, SHADOWWIDTH, DRAW3D_IN ) ;
#endif
   SetBkColor( hDC, rgbBackground ) ; 
   tdDrawChildren3D( hDC, hWnd ) ;

   x = 5 ;
   y = 1 ;

   yChar+= SHADOWWIDTH * 2 + 2 ;

   hSmallFont = SelectObject( hDC, hSmallFont ) ;
   
   GetTextMetrics( hDC, &tm ) ;
   dx = tm.tmAveCharWidth ;
   dy = tm.tmHeight + tm.tmExternalLeading + (SHADOWWIDTH * 2 + 2) ;

   rc.top = y * dy ;
   rc.left = x * dx ;
   rc.bottom = rc.top + (1 * dy) ;
   rc.right = rc.left + (55 * dx ) ;


   tdGetVersion( szVer ) ;

   tdExtTextOut3D( hDC, dx * x, dy * y, DRAW3D_IN | ETO_OPAQUE,
                 &rc,
                 szBuffer,
                 wsprintf( szBuffer,
                           (LPSTR)"TDUTIL.DLL Version %s    hInstance = 0x%04X",
                           (LPSTR)szVer,
                           tdGethInst()),
                 NULL,
                 SHADOWWIDTH ) ;
   hSmallFont = SelectObject( hDC, hSmallFont ) ;
   x = 20 ;
   y = 5 ;

   SelectObject( hDC, GetStockObject( SYSTEM_FIXED_FONT ) ) ;

   tdTextOut3D( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Using tdTextOut3D" ), SHADOWWIDTH, DRAW3D_OUT ) ;
   y+=1 ;

   tdTextOut3D( hDC, xChar * x, yChar * y, szBuffer,
            wsprintf( szBuffer,
                      (LPSTR)"Using tdTextOut3D (in)" ), SHADOWWIDTH, DRAW3D_IN ) ;
   y+=1 ;

   tdExtTextOut3D( hDC, xChar * x, yChar * y, DRAW3D_IN,
                 NULL,
                 szBuffer,
                 wsprintf( szBuffer,
                           (LPSTR)"tdTextOut3D( DRAW3D_IN )" ),
                 NULL,
                 1 ) ;
   y+=1 ;

   rc.top = y * yChar ;
   rc.left = x * xChar ;
   rc.bottom = rc.top + (3 * yChar) ;
   rc.right = rc.left + (50 * xChar ) ;

   tdExtTextOut3D( hDC, xChar * x, yChar * y, DRAW3D_IN | ETO_OPAQUE,
                 &rc,
                 szBuffer,
                 wsprintf( szBuffer,
                           (LPSTR)"DRAW3D_IN (tdExtTextOut())"),
                 NULL,
                 SHADOWWIDTH ) ;
   y+=4 ;


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
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1 );
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
   int          x, y ;
   int          xChar, yChar ;
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

//   DASSERT( hWDB, hWnd ) ;

//   DP( hWDB, "Begin execution" ) ;

   ShowWindow( hWnd, nCmdShow ) ;

   UpdateWindow( hWnd ) ;

   return TRUE ;             
}



/****************************************************************
 *  BOOL FAR PASCAL
 *    fnTest(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    About box window procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnTest(  HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   int n, i ;

   switch (wMsg)
   {
      case WM_COMMAND:
      {
         #ifdef WIN32
         WORD wNotifyCode = HIWORD(wParam); /* notification code                */
         WORD wID = LOWORD(wParam);         /* item, control, or accelerator ID */
         HWND hwndCtl = (HWND) lParam;      /* handle of control                */
         #else
        WORD wNotifyCode = HIWORD(lParam) ;
        WORD wID = wParam ;
        HWND hwndCtl = (HWND)LOWORD(lParam) ;
         #endif

         switch (wID)
         {
            case IDOK :
            case IDCANCEL :
   	   	   EndDialog( hDlg, TRUE ) ;
   	   	   return TRUE ;

            case IDD_LISTBOX:
               switch (wNotifyCode)
               {
                  case LBN_SELCHANGE:
                     n = (int)SendMessage( hwndCtl,
                                           LB_GETCURSEL, 0, 0L ) ;

                     //
                     // HACK!  Use WM_INITDIALOG to change the color
                     //
                     tdShadowControl( hDlg, WM_INITDIALOG,
                                    NULL, rgcsColors[n].rgb ) ;
                     InvalidateRect( hDlg, NULL, TRUE ) ;
                  break ;
               }
               

            break ;
	      }
      }
	   break ;

      case WM_INITDIALOG:
         SendMessage( GetDlgItem( hDlg, IDD_LISTBOX ),
                      WM_SETFONT, (WPARAM)hSmallFont, TRUE ) ;

         for (i = 0 ; i < _NUM_COLORS ; i++)
         {
            SendMessage( GetDlgItem( hDlg, IDD_LISTBOX),
                         LB_ADDSTRING, 0,
                         (LONG)(LPSTR)rgcsColors[i].szColor ) ;
         }


         SetDlgItemText( hDlg, IDD_DESC, APPDESC ) ;
//         DP( hWDB, "lParam = %08lX", lParam ) ;
         return tdShadowControl( hDlg, wMsg, wParam, lParam ) ;

      case WM_DESTROY:
         return tdShadowControl( hDlg, wMsg, wParam, lParam ) ;

      default:
         return tdShadowControl( hDlg, wMsg, wParam, lParam ) ;
   }

   return FALSE ;
} /* fnTest()  */




