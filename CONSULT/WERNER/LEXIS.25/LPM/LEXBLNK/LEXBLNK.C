/************************************************************************
 *
 *     Project:  LEXIS Program Manager
 *
 *   Copyright (c) 1991 Werner Associates.  All Rights Reserved.
 *
 *      Module:  lexblnk.c
 *
 *     Remarks:
 *
 *    This is the main source file lexblnk, the LEXIS Program Manager
 *    Screen blanker
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include <wdb.h>

#include "resource.h"
#include "lexblnk.h"
#include "blnkdll.h"

#define DEFAULT_DELAYTIME  (60*2)
#define DEFAULT_UPDATETIME (3)

#define TIMER_DELAY     42
#define TIMER_UPDATE    43
#define TIMER_STAYONTOP 44

#define LPMINIFILENAME  "LPM.INI"

/********************************************************************
 *   Global variables
 ********************************************************************/

HANDLE   hAppInst ;
char     szAppName[] = "lexblnk" ;
HWND     hwndMain ;
BOOL     fBlanked = FALSE ;
BOOL     fOkToUnblank = FALSE ;

POINT    ptOld ;

int   nDelayTime = 0 ;
int   nUpdateTime = 0 ;

extern char    **__argv ;
extern char    __argc ;

HWND  hwndPrevActive ;

#define DEFAULT_MESSAGE1 "LEXIS 2000® Research Software" 
#define DEFAULT_MESSAGE2 "Press any key or move the mouse to unblank the screen"
#define MAX_MSGSIZE 128

char  szMsg1[MAX_MSGSIZE] ;
char  szMsg2[MAX_MSGSIZE]  ;
/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WORD, LONG);
WORD FAR PASCAL fnTimer( HWND hWnd, WORD wMsg, int nIDEvent, DWORD dwTime ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;

int FAR PASCAL AtoI(const LPSTR lpszBuf) ;

HCURSOR FAR PASCAL SetHourGlass( void );
HCURSOR FAR PASCAL ResetHourGlass( HCURSOR hCur );
#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008

HFONT FAR PASCAL
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags ) ;
/********************************************************************
 *   Useful macros
 ********************************************************************/
#define GETHINST(hwnd)     (GetWindowWord( hwnd, GWW_HINSTANCE ))

#define getrandom( min, max ) ((rand() % (int)(((max)+1) - (min)))+(min))

/********************************************************************
 *   WinMain function - Entry point to applcation
 ********************************************************************/
int PASCAL WinMain(  HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpszCmdLine,		  
                     int      nCmdShow )			  
{
   MSG msg;
   BOOL  fBlankNow = FALSE ;
   BOOL  fQuit = FALSE ;
   HWND  hwndOldActive = GetActiveWindow() ;

   int i ;

   for (i = 1 ; i < __argc ; i++)
   {
      if (*__argv[i] == '/' || *__argv[i] == '-')
      {
         switch (*(__argv[i]+1))
         {
            //
            // Blank now.
            //
            case 's':
            case 'S':
               fBlankNow = TRUE ;
            break ;

            //
            // Delay time
            //
            case 'd':
            case 'D':
               nDelayTime = AtoI( __argv[i]+2 ) ;
            break ;

            //
            // Update speed
            //
            case 'u':
            case 'U':
               nUpdateTime = AtoI( __argv[i]+2 ) ;
            break ;

            //
            // Quit
            //
            case 'q':
            case 'Q':
               fQuit = TRUE ;
            break ;

            default:
               DP1( NULL, "LexBlnk: Bad command line switch: %s",
                   (LPSTR)__argv[i] ) ;

         }
      }
   }
                        
   hAppInst = hInstance ;

   if (hPrevInstance)
   {
      GetInstanceData( hPrevInstance, (PSTR)&hwndMain, sizeof( HWND ) ) ;

      //
      // There is already a copy running.  Activate it with the
      // parameters by passing messages.
      //
      if (nUpdateTime)
         SendMessage( hwndMain, WM_UPDATETIME, nUpdateTime, 0L ) ;

      if (nDelayTime)
         SendMessage( hwndMain, WM_DELAYTIME, nDelayTime, 0L ) ;

      if (fBlankNow)
         SendMessage( hwndMain, WM_BLANKNOW, TRUE, 0L ) ;

      if (fQuit)
         SendMessage( hwndMain, WM_CLOSE, 0, 0L ) ;

      return FALSE ;
   }
   else
   {
      if (fQuit)
         return FALSE ;

	   if (!InitApplication( hInstance ))
         return FALSE ;
   }

   //
   // Read profile string for default values
   //
   if (!nUpdateTime)
      nUpdateTime = GetPrivateProfileInt( szAppName, "UpdateTime",
                    DEFAULT_UPDATETIME, LPMINIFILENAME ) ;

   if (!nDelayTime)
      nDelayTime = GetPrivateProfileInt( szAppName, "DelayTime",
                    DEFAULT_DELAYTIME, LPMINIFILENAME ) ;

   GetPrivateProfileString( szAppName, "Message1", DEFAULT_MESSAGE1,
                            szMsg1, MAX_MSGSIZE, LPMINIFILENAME ) ;

   GetPrivateProfileString( szAppName, "Message2", DEFAULT_MESSAGE2,
                            szMsg2, MAX_MSGSIZE, LPMINIFILENAME ) ;

   if (!InitInstance( hInstance, nCmdShow ))
      return FALSE ;

   if (fBlankNow == TRUE)
      SendMessage( hwndMain, WM_BLANKNOW, TRUE, 0L ) ;
   else
      SetActiveWindow( hwndOldActive ) ;


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
   static FARPROC lpfnTimer ;

   switch (wMsg)
   {
      case WM_CREATE:
      {
         srand((unsigned) time( NULL ) ) ;

         hwndMain = hWnd ;
         D(hWDB=wdbOpenSession(hWnd, "lexblnk", "LPM.INI", WDB_LIBVERSION));
         DP1( hWDB, "WDB active!" ) ;

         //
         // Create delay timer
         //
         blnkResetTime() ;

         lpfnTimer = MakeProcInstance( fnTimer, hAppInst ) ;
         if (!SetTimer( hWnd, TIMER_DELAY, 500, lpfnTimer ))
         {
            DP1( hWDB, "SetTimer failed" ) ;
            FreeProcInstance( lpfnTimer ) ;
            return -1 ;
         }

         blnkSetHook() ;

#ifdef STAYONTOP
         if (!SetTimer( hWnd, TIMER_STAYONTOP, 500, lpfnTimer ))
         {
            DP1( hWDB, "SetTimer failed" ) ;
            KillTimer( hWnd, TIMER_DELAY ) ;
            KillTimer( hWnd, TIMER_UPDATE ) ;
            FreeProcInstance( lpfnTimer ) ;
            return -1 ;
         }
#endif

      }
      break ;

      case WM_BLANKNOW:
         blnkResetTime() ;
         if (!fBlanked)
         {
            char  szClassName[80] ;

            DP3( hWDB, "WM_BLANKNOW" ) ;

            //
            // Make sure we can blank.  If it's a dos app don't even try.
            //
            hwndPrevActive = GetActiveWindow( ) ;

            GetClassName( hwndPrevActive, szClassName, 80 ) ;

            DP3( hWDB, "szClassName = %s", (LPSTR)szClassName ) ;
            if (!lstrcmpi( szClassName, "tty" ))
               break ;

            //
            // Create update timer
            //
            if (!SetTimer( hWnd, TIMER_UPDATE, nUpdateTime * 1000, lpfnTimer ))
            {
               DP1( hWDB, "SetTimer failed" ) ;
               return -1 ;
            }

            fBlanked = TRUE ;
            ShowWindow( hWnd, SW_SHOWNORMAL ) ;

            SetActiveWindow( hWnd ) ;
            //
            // Hide the cursor
            //

            fOkToUnblank = FALSE ;

            ShowCursor( FALSE ) ;
            ShowCursor( FALSE ) ;
            ShowCursor( FALSE ) ;

            SetCapture( hWnd ) ;

         }

      break ;

      case WM_UPDATETIME:
         DP3( hWDB, "WM_UPDATETIME" ) ;
         nUpdateTime = wParam ;

         //
         // Reset timer
         // 
         if (fBlanked)
         {
            KillTimer( hWnd, TIMER_UPDATE ) ;
            if (!SetTimer( hWnd, TIMER_UPDATE, nUpdateTime * 1000, lpfnTimer ))
            {
               DP1( hWDB, "SetTimer failed" ) ;
               return -1 ;
            }
         }
      break ;

      case WM_DELAYTIME:
         DP3( hWDB, "WM_DELAYTIME" ) ;
         nDelayTime = wParam ;

         //
         // Reset timer
         // 
         blnkResetTime() ;
      break ;

      case WM_KEYDOWN:
      case WM_CHAR:
      case WM_MOUSEMOVE:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_LBUTTONDBLCLK:
      case WM_RBUTTONDBLCLK:
         if (fBlanked && fOkToUnblank)
         {
            DP3( hWDB, "Un-blanking" ) ;

            KillTimer( hWnd, TIMER_UPDATE ) ;

            blnkResetTime() ;

            ShowCursor( TRUE ) ;
            ShowCursor( TRUE ) ;
            ShowCursor( TRUE ) ;

            ReleaseCapture( ) ;

            ShowWindow( hWnd, SW_HIDE ) ;
            if (hwndPrevActive)
            {
               SetActiveWindow( hwndPrevActive ) ;
               SetFocus( hwndPrevActive ) ;
            }

            fBlanked = FALSE ;
         }

      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         HFONT       hFont ;
         short       x, y ;

         short cxMax, cyMax ;
         DWORD dw ;

         if (BeginPaint (hWnd, &ps))
         {
            SetTextColor( ps.hdc, RGBLTCYAN ) ;
            SetBkColor( ps.hdc, RGBBLACK ) ;

            hFont = ReallyCreateFont( ps.hdc, "TmsRmn", 14, RCF_BOLD ) ;

            hFont = SelectObject( ps.hdc, hFont ) ;

            dw = GetTextExtent( ps.hdc, szMsg1, lstrlen( szMsg1 ) ) ;

            cxMax = GetDeviceCaps( ps.hdc, HORZRES ) - LOWORD( dw ) ;
            cyMax = GetDeviceCaps( ps.hdc, VERTRES ) - HIWORD( dw ) ;

            x = getrandom( 0, cxMax ) ;
            y = getrandom( 0, cyMax ) ;

            DP3( hWDB, "x = %d, y = %d", x, y ) ;
            TextOut( ps.hdc, x, y, szMsg1, lstrlen( szMsg1 ) ) ;

            hFont = SelectObject( ps.hdc, hFont ) ;
            DeleteObject( hFont ) ;

            SetTextColor( ps.hdc, RGBBLACK ) ;
            SetBkColor( ps.hdc, RGBLTGREEN ) ;

            hFont = ReallyCreateFont( ps.hdc, "Helv", 10, RCF_NORMAL ) ;

            hFont = SelectObject( ps.hdc, hFont ) ;

            dw = GetTextExtent( ps.hdc, szMsg2, lstrlen( szMsg2 ) ) ;

            cxMax = GetDeviceCaps( ps.hdc, HORZRES ) - LOWORD( dw ) ;
            cyMax = GetDeviceCaps( ps.hdc, VERTRES ) - HIWORD( dw ) ;

            x = getrandom( 0, cxMax ) ;
            y = getrandom( 0, cyMax ) ;

            DP3( hWDB, "x = %d, y = %d", x, y ) ;
            TextOut( ps.hdc, x, y, szMsg2, lstrlen( szMsg2 ) ) ;

            hFont = SelectObject( ps.hdc, hFont ) ;
            DeleteObject( hFont ) ;

            EndPaint (hWnd, &ps) ;
         }
      }
      break ;

	   case WM_DESTROY:
         DP3( hWDB, "WM_DESTROY" ) ;
         //
         // Kill timers
         //
         KillTimer( hWnd, TIMER_DELAY ) ;
         KillTimer( hWnd, TIMER_UPDATE ) ;
#ifdef STAYONTOP
         KillTimer( hWnd, TIMER_STAYONTOP ) ;
#endif

         if (lpfnTimer)
            FreeProcInstance( lpfnTimer ) ;

         D(wdbCloseSession(hWDB));
	      PostQuitMessage( 0 ) ;

         blnkUnhookHook( ) ;

	      break;

	   default:		
	      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }
   return 0L ;
}

WORD FAR PASCAL fnTimer( HWND hWnd, WORD wMsg, int nIDEvent, DWORD dwTime )
{
   POINT  pt ;
   //
   // What timer event is it?  Delay or Update?
   //
   switch (nIDEvent)
   {
      case TIMER_DELAY:
         if (fBlanked)
         {
            fOkToUnblank = TRUE ;
            blnkResetTime() ;
         }
         else
         {
            GetCursorPos( &pt ) ;

            if (pt.x != ptOld.x && pt.y != ptOld.y)
               blnkResetTime() ;

            ptOld = pt ;

            DPF3( hWDB, "TIMER_DELAY: " ) ;

            blnkIncTime( 1 ) ;

            DP3( hWDB, "  current time = %d", blnkIncTime(0) ) ;

            if (blnkIncTime(0) == (nDelayTime * 2))
               PostMessage( hWnd, WM_BLANKNOW, 0, 0L ) ;
         }
      break ;

      case TIMER_UPDATE:
         DP3( hWDB, "TIMER_UPDATE" ) ;
         if (fBlanked)
         {
            InvalidateRect( hWnd, NULL, TRUE ) ;
            UpdateWindow( hWnd ) ;
         }
      break ;

#ifdef STAYONTOP
      case TIMER_STAYONTOP:
         if (fBlanked && (GetActiveWindow() != hwndMain))
            SetActiveWindow( hwndMain ) ;
      break ;
#endif

      default:
         DP1( hWDB, "Unknown timer" ) ;
   }

   return TRUE ;
}


/********************************************************************
 *  InitApplication - Initialize the Class and register it
 ********************************************************************/
BOOL NEAR PASCAL InitApplication( HANDLE hInstance )
{
    WNDCLASS  wc;

   wc.style         = 0 ;
   wc.lpfnWndProc   = fnMainWnd;  
                                  
   wc.cbClsExtra    = 0;             
   wc.cbWndExtra    = 0;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, szAppName );
   wc.hCursor       = LoadCursor( hInstance, szAppName );
   wc.hbrBackground = GetStockObject( BLACK_BRUSH );
   wc.lpszMenuName  = NULL ; //szAppName;
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

   x = GetSystemMetrics( SM_CXSCREEN ) ;
   y = GetSystemMetrics( SM_CYSCREEN ) ;

   hWnd = CreateWindow( szAppName,
                        szAppName,   
                        WS_POPUP,
                        0, 0,
                        x, y,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;


   if (!hWnd)
       return FALSE ;

   return TRUE ;             
}

#if 0
/*************************************************************************
 *  BOOL FAR PASCAL SetHourGlass( BOOL f )
 *
 *  Description: 
 *
 *    This function turns on the Hourglass cursor.
 *
 *  Comments:
 *
 *************************************************************************/
HCURSOR FAR PASCAL SetHourGlass( void )
{
   return SetCursor( hHourGlass ) ;
}/* SetHourGlass() */

HCURSOR FAR PASCAL ResetHourGlass( HCURSOR hCur )
{
   return SetCursor( hCur ) ;
}
#endif
/*************************************************************************
 *  int FAR PASCAL AtoI(const LPSTR lpszBuf)
 *
 *  Description:
 *    Converts a zero terminated character string to an integer value.
 *    The function behaves exactly like the C runtime function AtoI,
 *    the only difference being the far pointer.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszBuf
 *
 *          String to be converted
 * 
 *  Return Value
 *    Returns an int value produced by interpreting the input chars as
 *    a number.
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL AtoI(const LPSTR lpszBuf)
{
   int n = 0;
   LPSTR lpszCur = lpszBuf ;
   BOOL  fNeg ;

   while (isspace(*lpszCur))
      lpszCur++ ;

   if (!(isdigit(*lpszCur) || *lpszCur == '-' || *lpszCur == '+'))
      return 0 ;

   fNeg = FALSE ;

   if (*lpszCur == '-')
   {
      lpszCur++ ;
      fNeg = TRUE ;
   }

   if (*lpszCur == '+')
      lpszCur++ ;

   while (isdigit(*lpszCur))
   {
       n *= 10 ;
       n += *lpszCur - '0' ;
       lpszCur++ ;
   }
   return n * (fNeg ? -1 : 1) ;
}/* AtoI() */

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/************************************************************************
 *
 *  REALLY SELECT A FONT STUFF
 *
 *  The following code selects a font based on a point setting (not
 *  pixels).  It is cool since it helps avoid the wrath of the font
 *  randomizer in GDI.
 *
 **********************************************************************/

/****************************************************************
 *  int FAR PASCAL
 *    fnEnumReally( LPLOGFONT lpLogFont,
 *                  LPTEXTMETRIC lpTextMetrics,
 *                  short nFontType, LPLOGFONT lpLF )
 *
 *  Description: 
 *
 *    Call back for EnumFonts and ReallySelectFont().
 *
 *    DO NOT FORGET TO EXPORT!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL
fnEnumReally( LPLOGFONT lpLogFont,
              LPTEXTMETRIC lpTextMetrics, short nFontType, LPLOGFONT lpLF )
{
   DP5( hWDB, "In fnEnumReally!" ) ;

   *lpLF = *lpLogFont ;

   //
   // Only need to be called once!
   //
   return FALSE ;

}/* fnEnumReally() */
/****************************************************************
 *  HFONT FAR PASCAL
 *    ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize,
 *                      short nWeight, WORD wFlags )
 *
 *  Description: 
 *
 *    Creates a font just like CreateFont, but attempts to avoid the
 *    font randomizer.
 *
 *    wFlags can be RCF_ITALIC, RCF_UNDERLINE,
 *    RCF_BOLD and/or RCF_STRIKEOUT.
 *
 *  Comments:
 *
 ****************************************************************/
HFONT FAR PASCAL
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags )
{
   LOGFONT     lf ;
   FARPROC     lpfnEnumReally ;
   HDC         hCurDC ;

   DP5( hWDB, "In ReallyCreateFont!" ) ;

   //
   // First use font enumeration get the facename specified.
   // If that fails use a font based off of ANSI_VAR_FONT
   //

   if (!(lpfnEnumReally = MakeProcInstance( fnEnumReally, hAppInst )))
   {
      DP1( hWDB, "MakeProcInstance failed" ) ;
      return FALSE ;
   }

   //
   // if the DC wasn't specified then use the display...
   //
   if (!hDC)
   {
      if (!(hCurDC = GetDC( NULL )))
      {
         DP1( hWDB,"Could not GetDC" ) ;
         FreeProcInstance( lpfnEnumReally ) ;
         return FALSE ;
      }
   }
   else
      hCurDC = hDC ;

   if (hCurDC)
   {

      //
      // Enumerate.  If lfFaceName is not lpszFace then the font was
      // not found.
      //
      if (lpszFace)
         EnumFonts( hCurDC, lpszFace, lpfnEnumReally, (LPSTR)&lf ) ;
      else
         DP1( hWDB, "lpszFace is NULL!" ) ;

      if (!lpszFace || lstrcmpi( lf.lfFaceName, lpszFace))
         GetObject( GetStockObject( ANSI_VAR_FONT ),
                    sizeof( LOGFONT ), (LPSTR)&lf ) ;

      lf.lfWidth = 0 ;
      lf.lfHeight = -MulDiv( nPointSize,
                             GetDeviceCaps( hCurDC, LOGPIXELSY),
                             72 )  ;

      lf.lfWeight    = (wFlags & RCF_BOLD) ? FW_BOLD : FW_NORMAL ;
      lf.lfItalic    = (BYTE)((wFlags & RCF_ITALIC) == RCF_ITALIC) ;
      lf.lfStrikeOut = (BYTE)((wFlags & RCF_STRIKEOUT) == RCF_STRIKEOUT) ;
      lf.lfUnderline = (BYTE)((wFlags & RCF_UNDERLINE) == RCF_UNDERLINE) ;
   }

   FreeProcInstance( lpfnEnumReally ) ;

   if (hCurDC != hDC)
      ReleaseDC( NULL, hCurDC ) ;

   DP5( hWDB, "About to return from ReallyCreateFont" ) ;
   return CreateFontIndirect( &lf ) ;

} /* ReallyCreateFont()  */

/************************************************************************
 *
 *  end of REALLY SELECT A FONT STUFF
 *
 ************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/************************************************************************
 * End of File: lexblnk.c
 ***********************************************************************/

