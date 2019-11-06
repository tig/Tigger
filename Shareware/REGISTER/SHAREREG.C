 /************************************************************************
 *
 *     Project:  Shareware Registration System
 *
 *      Module:  sharereg.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "reglib\\reglib.h"

#include "resource.h"
#include "sharereg.h"

//
// Define these to set how big the window is when it is created
//
#define NUMCHARSX 80
#define NUMCHARSY 25

#define APPDESC "Shareware Registration"

#define MAGIC_COOKIE 7431
/********************************************************************
 *   Global variables
 ********************************************************************/

HANDLE   hAppInst ;
char     szAppName[] = "ShareReg" ;

HWND     hwndTB ;      //Tool bar window

char     szCurApp[_MAX_FNAME+1] ;
char     szCurName[REG_MAX_NAME+1] ;
char     szCurRegNum[REG_MAX_NUMBER+1] ;
char     szCurRegString[REG_MAX_NAME + REG_MAX_NUMBER + 128] ;
char     szCurRegStringOEM[REG_MAX_NAME + REG_MAX_NUMBER + 128] ;
char     szRegFmt[] = "Registration Name:   \t%s\nRegistration Number: \t%s\n" ;
char     szRegFmtOEM[] = "Registration Name:   %s\r\nRegistration Number: %s\r\n" ;

/********************************************************************
 *   Local functions
 ********************************************************************/
LRESULT CALLBACK fnMainWnd( HWND hwnd, 
                            UINT uiMsg,  
                            WPARAM wParam,            
                            LPARAM lParam ) ;

BOOL CALLBACK fnAbout(  HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

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
                        
   hAppInst = hInstance ;

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
LRESULT CALLBACK fnMainWnd( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )            
{
    static RECT    rcMain ;
    
    switch (uiMsg)
    {
        case WM_CREATE:
            szCurName[0] = '\0' ;
            szCurRegNum[0] = '\0' ;
            wsprintf( szCurRegString, szRegFmt, (LPSTR)szCurName, (LPSTR)szCurRegNum ) ;
            wsprintf( szCurRegStringOEM, szRegFmtOEM, (LPSTR)szCurName, (LPSTR)szCurRegNum ) ;
            
            D(hWDB=wdbOpenSession(hwnd, "ShareReg", "SHAREREG.INI", WDB_LIBVERSION));
            DP1( hWDB, "WDB active!" ) ;
                
            if (hwndTB = tbCreate( hwnd ))
            {
                char  szBuf[8 + _MAX_FNAME + 1] ;
                WORD  w ;
 
                // Fill combo box with app names
                //
                FillAppNameCB() ;
                ShowWindow( hwndTB, SW_NORMAL ) ;
                    
                w = (WORD)SendMessage( rghwndChild[IDD_APPCB], CB_GETCURSEL, 0, 0L ) ;
                SendMessage( rghwndChild[IDD_APPCB], CB_GETLBTEXT, 0, (LONG)(LPSTR)szCurApp ) ;
                    
                wsprintf( szBuf, "%s [%s]", (LPSTR)szAppName, (LPSTR)szCurApp ) ;
                SetWindowText( hwnd, szBuf ) ;
                    
            }
            else                       
                return -1 ;
                    
            GetClientRect( hwnd, &rcMain ) ;
        break ;
                  
        case WM_SETFOCUS:
           if (hwndTB)
                SetFocus( hwndTB ) ;
        break ;
            
        case WM_SIZE:
        {
            WORD     wHeight = 0 ;
            RECT     rc ;
                
            if (hwndTB)
            {
                wHeight = HIWORD( SendMessage( hwndTB, TB_HOWBIG, 0, 0L ) ) ;
                    
                rc.left = 0 ;
                rc.right = (short)LOWORD( lParam ) ;
                rc.top = wHeight ;
                rc.bottom = HIWORD( lParam ) ;
                
                if (rcMain.right != (short)LOWORD( lParam ))
                {
                    MoveWindow( hwndTB, 0, 0, LOWORD( lParam ), wHeight, TRUE ) ;
                    rcMain.right = (short)LOWORD( lParam ) ;
                }
                    
                if (rcMain.bottom != (short)HIWORD( lParam ))
                {
                    rcMain.bottom = HIWORD( lParam ) ;
                }
                    
                InvalidateRect( hwnd, &rc, TRUE ) ;
            }
        }
        break ;
            
        //
        // The tool bar dictates the minimum size of the window.
        //
        case WM_GETMINMAXINFO:
        {
            DWORD    dw ;
            short    nX, nY ;
            LPPOINT  lppt ;
            
            if (hwndTB)
            {
                dw = SendMessage( hwndTB, TB_HOWBIG, 0, 0L ) ;
                    
                lppt = (LPPOINT)lParam ;
                    
                nX = GetSystemMetrics( SM_CXFRAME ) * 2 ;
                nY = GetSystemMetrics( SM_CYFRAME ) * 2 +
                GetSystemMetrics( SM_CYCAPTION ) +
                GetSystemMetrics( SM_CYMENU ) ;
                    
                lppt[3].x = LOWORD( dw ) + nX ;
                lppt[3].y = HIWORD( dw ) - 1 + nY ;
                    
                lppt[4].x = LOWORD( dw ) + nX ;
            }
            else
                return DefWindowProc( hwnd, uiMsg, wParam, lParam ) ;
        }
        break ;
            
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_COPY:
                case IDD_DOITBTN:
                {
                    GetWindowText( rghwndChild[IDD_NAMEEDIT], szCurName,
                          REG_MAX_NAME ) ;
                    rlGetRegNumber( szCurApp, szCurName, szCurRegNum ) ;
                    wsprintf( szCurRegString, szRegFmt, (LPSTR)szCurName, (LPSTR)szCurRegNum ) ;
                    wsprintf( szCurRegStringOEM, szRegFmtOEM, (LPSTR)szCurName, (LPSTR)szCurRegNum ) ;
                   
                    // Paste to clip
                    //
                    if (OpenClipboard( hwnd ))
                    {
                        HGLOBAL hmem ;
                        LPSTR lp ;
                        hmem = GlobalAlloc( GHND, lstrlen( szCurRegStringOEM ) + 1 ) ;
                        lp = (LPSTR)GlobalLock( hmem ) ;
                        lstrcpy( lp, szCurRegString ) ;
                        GlobalUnlock( hmem ) ;
                        SetClipboardData( CF_TEXT, hmem ) ;

                        //hmem = GlobalAlloc( GHND, lstrlen( szCurRegStringOEM ) + 1 ) ;
                        //lp = (LPSTR)GlobalLock( hmem ) ;
                        //lstrcpy( lp, szCurRegStringOEM ) ;
                        //GlobalUnlock( hmem ) ;
                        //SetClipboardData( CF_OEMTEXT, hmem ) ;
                        CloseClipboard() ;
                    }
                    InvalidateRect( hwnd, NULL, TRUE ) ;
                    UpdateWindow( hwnd ) ;
                }
                break ;
                
                case IDD_APPCB:
                    if (HIWORD( lParam ) == CBN_SELCHANGE)
                    {
                        char  szBuf[8 + _MAX_FNAME + 1] ;
                        WORD  w ;
                    
                        DP5( hWDB, "CBN_SELCHANGE" ) ;
                    
                        w = (WORD)SendMessage( rghwndChild[IDD_APPCB],
                                 CB_GETCURSEL, 0, 0L ) ;
                        SendMessage( rghwndChild[IDD_APPCB], CB_GETLBTEXT, w,
                                    (LONG)(LPSTR)szCurApp ) ;
                    
                        wsprintf( szBuf, "%s [%s]", (LPSTR)szAppName,
                                      (LPSTR)szCurApp ) ;
                        SetWindowText( hwnd, szBuf ) ;
                    }
                break ;
                
                case IDM_ABOUT:
                {
                    FARPROC lpfnAbout ;
                    lpfnAbout = MakeProcInstance( fnAbout, GETHINST( hwnd ) ) ;
                    DialogBox(  GETHINST( hwnd ), "ABOUTBOX",hwnd, lpfnAbout ) ;
                    
                    FreeProcInstance( lpfnAbout ) ;
                }
                break;
                    
                case IDM_EXIT:
                    SendMessage( hwnd, WM_CLOSE, 0, 0L ) ;
                break ;
                                
                case IDM_PASTE:
                    // Paste contents of clipboard into name edit
                    //
                    if (OpenClipboard( hwnd ))
                    {
                        HGLOBAL hmem ;
                        LPSTR lp ;
                        if (hmem = GetClipboardData( CF_TEXT ))
                        {
                            lp = (LPSTR)GlobalLock( hmem ) ;
                            lstrcpy( szCurName, lp ) ;
                            GlobalUnlock( hmem ) ;
                            SetWindowText( rghwndChild[IDD_NAMEEDIT], szCurName ) ;
                        }
                        CloseClipboard() ;
                    }
                break ;
                    
                default:
                    return DefWindowProc( hwnd, uiMsg, wParam, lParam ) ;
            }
        break ;
                
        case WM_PAINT:
        {
            PAINTSTRUCT ps ;
            TEXTMETRIC  tm ;
            int         xChar, yChar ;
            RECT        rc ;
                
            if (BeginPaint (hwnd, &ps))
            {
                SelectObject( ps.hdc, GetStockObject( ANSI_FIXED_FONT ) ) ;
                GetTextMetrics( ps.hdc, &tm ) ;
                    
                GetClientRect( hwnd, &rc ) ;
                rc.top += HIWORD( SendMessage( hwndTB, TB_HOWBIG, 0, 0L ) ) + 1 ;
                    
                SetBkColor( ps.hdc, GetSysColor( COLOR_BTNFACE ) ) ;
                Draw3DRect( ps.hdc, &rc, 1, DRAW3D_OUT ) ;
                    
                xChar = tm.tmAveCharWidth ;
                yChar = tm.tmHeight + tm.tmExternalLeading ;

                rc.top += 2 ; 
                rc.left += 2 ;
                DrawText( ps.hdc, szCurRegString, lstrlen(szCurRegString), &rc,
                    DT_LEFT | DT_EXPANDTABS ) ;
                    
                EndPaint (hwnd, &ps) ;
            }
        }
        break ;
            
        case WM_DESTROY:
            if (hwndTB)
                tbDestroy( hwndTB ) ;
            
            SaveWindowPos( hwnd ) ;
            D(wdbCloseSession(hWDB));
            PostQuitMessage( 0 ) ;
        break;
            
        default:     
            return DefWindowProc( hwnd, uiMsg, wParam, lParam ) ;
    }
    return 0L ;
}

/********************************************************************
 *  InitApplication - Initialize the Class and register it
 ********************************************************************/
BOOL NEAR PASCAL InitApplication( HANDLE hInstance )
{
    WNDCLASS  wc;

   // register the toolbar
   if (!tbRegister())
       return FALSE ;

   wc.style         = 0 ; //CS_HREDRAW | CS_VREDRAW ;      
   wc.lpfnWndProc   = fnMainWnd;  
                                  
   wc.cbClsExtra    = 0;             
   wc.cbWndExtra    = 0;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, szAppName );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = COLOR_BTNFACE + 1 ;
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
   HWND            hwnd;      

   short          x, y ;
   short          xChar, yChar ;
   HDC            hDC ;

   x = CW_USEDEFAULT ;
   y = CW_USEDEFAULT ;

   if (hDC = GetDC( NULL ))
   {
      TEXTMETRIC     tm ;

      SelectObject( hDC, GetStockObject( ANSI_VAR_FONT ) ) ;
      GetTextMetrics( hDC, &tm ) ;
      xChar = (tm.tmAveCharWidth) ;
      yChar = (tm.tmHeight + tm.tmExternalLeading ) ;
      ReleaseDC( NULL, hDC ) ;
   }


   hwnd = CreateWindow( szAppName,
                        szAppName,   
                        WS_OVERLAPPEDWINDOW,  
                        x, y,
                        xChar * NUMCHARSX, yChar * NUMCHARSY,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;


   if (!hwnd)
       return FALSE ;

#if 0
   //
   // Place the window in it's default position
   //
   if (hwndTB)
   {
      DWORD       dwTBsize ;

      dwTBsize = SendMessage( hwndTB, TB_HOWBIG, 0, 0L ) ;
      SetWindowPos( hwnd, NULL, x, y, 
                                LOWORD( dwTBsize ),
                                yChar * NUMCHARSY,
                                SWP_NOZORDER ) ;
   }

   //
   // use INI settings to place the window (if they're there)
   //
#endif

   PlaceWindow( hwnd ) ;
   ShowWindow( hwnd, nCmdShow ) ;

   UpdateWindow( hwnd ) ;

   return TRUE ;             
}


/****************************************************************
 *  BOOL FAR PASCAL
 *    fnAbout(  hwnd hDlg, WORD uiMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    About box window procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL CALLBACK fnAbout(  HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uiMsg)
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

/************************************************************************
 * End of File: sharereg.c
 ***********************************************************************/

