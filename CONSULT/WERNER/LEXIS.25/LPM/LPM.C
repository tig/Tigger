/************************************************************************
 *
 *     Project:  LEXIS Program Manager
 *
 *   Copyright (c) 1991 Werner Associates.  All Rights Reserved.
 *
 *      Module:  lpm.c
 *
 *     Remarks:
 *
 *    This is the main source file ofr LPM, the LEXIS Program Manager.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include <windows.h>
#include <stdlib.h>
#include <wdb.h>

#include "resource.h"
#include "lpm.h"
#include "bmpbtn.h"

#define NUM_SECONDS	5
#define MIN_CXBOTTEXT 80
#define MIN_CYBOTTEXT 64

/********************************************************************
 *   Global variables
 ********************************************************************/

HANDLE   hAppInst ;
char     szAppName[] = "LPM" ;

HBRUSH   ghbrBackground ;     // background for windclass

char    szInstructions[] = "Welcome to the LEXIS® 2000 for Windows Program \
Manager!\n\nTo start a program, double click on its symbol with the \
left mouse button or tap the TAB key until the symbol is highlighted \
and press the ENTER key." ;

char    szInfo[] = "Copyright © 1991 Werner Associates.\n\
All Rights Reserved.\n\n\
LEXIS® 2000 Research Software.  Mead Data Central, Inc.\n\nWindows is a trademark of Microsoft Corporation." ;

//
// The following are used to place the stuff in the window
//
RECT  rcClient ;           // "Client" area.
RECT  rcBottom ;           // "Botom box"

short cyTopText ;          // top text box (instructions) height

short cxButton ;           // height of one button
short cyButton ;           // width of one button

short cyBotText ;          // bottom text box (clock and (c)) height
				
//
// Buttons
//
HWND  rghwndBtn[NUMBUTTONS+2] ;
char  *rgszBtnText[] =
#ifdef TEXTONLY
{
   "LEXIS for Windows",
   "LEXIS for DOS",
   "Home Access",
   "CheckCiteII",
   "LEXFORM",
   "Utilities",
   "F1 = Help!",
   "Blank Screen"

} ;
#else
{
   "~BTN0||LEXIS\nfor\nWindows",
   "~BTN1||LEXIS\nfor\nDOS",
   "~BTN2||Home\nAccess",
   "~BTN3||CheckCite\nII",
   "~BTN4||LEXFORM",
   "~BTN5||Utilities",
   "~HELP||F1 = Help!",
   "~BLANK||Blank\nScreen"
} ;
#endif

FARPROC  rglpfnBtnOld[NUMBUTTONS+2] ;
FARPROC  lpfnSubClass ;
WORD     wCurFocus = IDD_LFW ;

BOOL		fStopInput ;
HCURSOR	hHourGlass ;
BOOL		fHourGlass ;

short		nDispColors ; 
short		nDispSizeY ; 

/***************************
   The programs that are launched are fixed, but their
   locations on the drive might not be.  For this reason
   the path's for each file is read from the lpm.ini file
****************************/

//
// INI file section
//
#define  INI_SECTION    "Command Lines"
//
// Keynames
//
char    *rgszApp[] =
{
   "WinLexis",
   "DOSLexis",
   "HOMEAccess",
   "CheckCite",
   "LEXFORM",
   "Utilities"
} ;

/********************************************************************
 *   Local functions
 ********************************************************************/
LONG FAR PASCAL fnMainWnd(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL fnAbout(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL InitApplication( HANDLE ) ;

BOOL NEAR PASCAL InitInstance( HANDLE, int ) ;

LONG FAR PASCAL fnSubClass( HWND hwndBmpBtn, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL AreWeShell( VOID ) ;

BOOL NEAR PASCAL ExecApp( WORD wAppID ) ;

BOOL FAR PASCAL fnExit(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam ) ;
 
HCURSOR FAR PASCAL SetHourGlass( void );
HCURSOR FAR PASCAL ResetHourGlass( HCURSOR hCur );
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

   ghbrBackground = CreateSolidBrush( RGBLTGRAY ) ;

   if (!hPrevInstance)
   {
	   if (!InitApplication( hInstance ))
      {
          DeleteObject( ghbrBackground ) ;
	       return FALSE ;
      }
   }
   else
      return FALSE ;

   if (!BmpBtnInit( hPrevInstance, hInstance ))
   {
      DP1( hWDB, "BmpBtn failed to init!" ) ;
      DeleteObject( ghbrBackground ) ;
      return FALSE ;
   }

   if (!InitInstance( hInstance, nCmdShow ))
   {
        DeleteObject( ghbrBackground ) ;
        return FALSE ;
   }


   while (GetMessage( &msg, NULL, NULL, NULL ))	 
  	{
      TranslateMessage( &msg ) ;
     	DispatchMessage( &msg ) ;	  
   }

   DeleteObject( ghbrBackground ) ;

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
   static RECT    rcMain ;
   static HFONT   hFont ;
	static HCURSOR hCursor ;
	static HCURSOR hHourGlass ;

   switch (wMsg)
   {

      case WM_CREATE:
      {
			short i ;
			HDC	hDC ;

         D(hWDB=wdbOpenSession(hWnd, "LPM", "LPM.INI", WDB_LIBVERSION));

         DP1( hWDB, "WDB active!" ) ;

   		hHourGlass = LoadCursor( NULL, IDC_WAIT ) ;

         SetWindowText( hWnd, "LEXIS® 2000 Program Manager" ) ;

         if (!(lpfnSubClass = MakeProcInstance( fnSubClass, hAppInst )))
         {
            DP1( hWDB, "MakeProcInstance failed!" ) ;
            return -1 ;
         }

			hDC = GetDC( hWnd ) ;

			nDispSizeY = GetDeviceCaps( hDC, VERTRES ) ;
			if (nDispSizeY == 350)	//ega
				nDispColors = 8 ;
			else
				nDispColors = GetDeviceCaps( hDC, NUMCOLORS ) ;
			DP1( hWDB, "nDispColors = %d", nDispColors ) ;
			DP1( hWDB, "PLANES      = %d", GetDeviceCaps( hDC, PLANES )) ;
			DP1( hWDB, "BITSPIXEL   = %d", GetDeviceCaps( hDC, BITSPIXEL )) ;
			DP1( hWDB, "NUMCOLORS   = %d",GetDeviceCaps( hDC, NUMCOLORS )) ;
			DP1( hWDB, "COLORRES    = %d",GetDeviceCaps( hDC, COLORRES )) ;
	
         DP5( hWDB, "Creating font..." ) ;
			switch( nDispSizeY )
			{
				case 350:		// EGA
		         hFont = ReallyCreateFont( NULL, "Helv", 8, RCF_NORMAL ) ;
				break ;

				case 480:		// VGA
		         hFont = ReallyCreateFont( NULL, "Helv", 10, RCF_NORMAL ) ;
				break ;

				case 600:
		         hFont = ReallyCreateFont( NULL, "Helv", 12, RCF_NORMAL ) ;
				break ;

				case 768:
		         hFont = ReallyCreateFont( NULL, "Helv", 14, RCF_NORMAL ) ;
				break ;

				default:
		         hFont = ReallyCreateFont( NULL, "Helv", 8, RCF_NORMAL ) ;
				break ;
			}
         DP5( hWDB, "hFont = %d", hFont ) ;
			ReleaseDC( hWnd, hDC ) ;

         for (i = IDD_LFW ; i <= IDD_UTIL ; i++)
         {
            if (!(rghwndBtn[i] = CreateWindow("LEXISbmpbtn",
                        rgszBtnText[i],
                        BS_PUSHBUTTON |
                        WS_CHILD |
                        WS_VISIBLE,
                        0,0,0,0,
                        hWnd,
                        i,
                        hAppInst,
                        NULL )))
            {
               DP1( hWDB, "Could not create button %d", i ) ;
               return -1 ;
            }
            SendMessage( rghwndBtn[i], WM_SETFONT, hFont, 0L ) ;

            //
            // Subclass it
            //
            rglpfnBtnOld[i] = (FARPROC)SetWindowLong( rghwndBtn[i],
                                                      GWL_WNDPROC,
                                                      (LONG)lpfnSubClass ) ;
         }

         i = IDD_HELP ;
         if (!(rghwndBtn[i] = CreateWindow("LEXISbmpbtn",
                     rgszBtnText[i],
                     BS_PUSHBUTTON |
                     WS_CHILD |
                     WS_VISIBLE,
							0,0,0,0,
                     hWnd,
                     i,
                     hAppInst,
                     NULL )))
         {
            DP1( hWDB, "Could not create button %d", i ) ;
            return -1 ;
         }
         SendMessage( rghwndBtn[i], WM_SETFONT, hFont, 0L ) ;
         //
         // Subclass it
         //
         rglpfnBtnOld[i] = (FARPROC)SetWindowLong( rghwndBtn[i],
                                                   GWL_WNDPROC,
                                                   (LONG)lpfnSubClass ) ;

         i = IDD_BLANK ;
         if (!(rghwndBtn[i] = CreateWindow("LEXISbmpbtn",
                     rgszBtnText[i],
                     BS_PUSHBUTTON |
                     WS_CHILD |
                     WS_VISIBLE,
							0,0,0,0,
                     hWnd,
                     i,
                     hAppInst,
                     NULL )))
         {
            DP1( hWDB, "Could not create button %d", i ) ;
            return -1 ;
         }
         SendMessage( rghwndBtn[i], WM_SETFONT, hFont, 0L ) ;
         //
         // Subclass it
         //
         rglpfnBtnOld[i] = (FARPROC)SetWindowLong( rghwndBtn[i],
                                                   GWL_WNDPROC,
                                                   (LONG)lpfnSubClass ) ;

         WinExec( "lexblnk.exe", SW_SHOWNORMAL ) ;
      }
      break ;

      case WM_SETFOCUS:
         if (rghwndBtn[wCurFocus])
            SetFocus( rghwndBtn[wCurFocus] ) ;
         else
   	      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

		//
		// When we win exec something we set a timer for about 5 seconds.
		// While this timer is running, we SetCapture() and put up the
		// hour glass.  This will *hopefully* prevent the user from
		// double-clicking twice...
		//
		case WM_TIMER:

			DP3( hWDB, "WM_TIMER" ) ;

			if (fStopInput)
				fStopInput = FALSE ;

			if (fHourGlass)
			{
				ResetHourGlass( hCursor ) ;
				fHourGlass = FALSE ;
			}

			KillTimer( hWnd, 42 ) ;

		break ;

	   case WM_COMMAND:
			if (fStopInput)
				break ;

         switch (wParam)
         {
            case IDD_LFW:
            case IDD_LFD:
            case IDD_HOME:
            case IDD_CHECK:
            case IDD_LEXFRM:
            case IDD_UTIL:
            {
					hCursor = SetHourGlass( ) ;
					fHourGlass = TRUE ;

					fStopInput = TRUE ;

               if (!ExecApp( wParam ))
               {
                  char szBuf[80] ;
                  wsprintf( szBuf, "Could not start the %s application.\nContact your system administrator.",
                                   (LPSTR)rgszApp[wParam] ) ;
           
						ResetHourGlass( hCursor ) ;
						fHourGlass = FALSE ;

                  MessageBox( hWnd, szBuf, "LEXIS® Program Manager",
                              MB_ICONEXCLAMATION ) ;

						fStopInput = FALSE ;
               }
					else
					{
						SetTimer( hWnd, 42, NUM_SECONDS * 1000, NULL ) ;
					}
            }
            break ;

            case IDD_HELP:
               WinHelp( hWnd, "LPM.HLP", HELP_INDEX, NULL ) ;
            break ;

            case IDD_BLANK:
				{
            	FARPROC lpfnAbout ;
            	lpfnAbout = MakeProcInstance( fnAbout, hAppInst ) ;
            	DialogBox( hAppInst, "ABOUTBOX", hWnd, lpfnAbout ) ;
            	FreeProcInstance( lpfnAbout ) ;
				}
            break ;

            default:
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         }

      break ;

      case WM_LBUTTONDBLCLK:
      {
         POINT pt ;
         pt.x = LOWORD( lParam ) ;
         pt.y = HIWORD( lParam ) ;
         if (PtInRect( &rcBottom, pt ))
         {
            FARPROC lpfnAbout ;
            lpfnAbout = MakeProcInstance( fnAbout, hAppInst ) ;
            DialogBox( hAppInst, "ABOUTBOX1", hWnd, lpfnAbout ) ;
            FreeProcInstance( lpfnAbout ) ;
         }
      }
      break ;

      case WM_PAINT:
      {
         PAINTSTRUCT ps ;
         RECT        rc ;
         RECT        rc2 ;
         RECT        rc3 ;
			HFONT			hOldFont ;

         if (BeginPaint (hWnd, &ps))
         {
            hOldFont = SelectObject( ps.hdc, hFont ) ;

            GetClientRect( hWnd, &rc ) ;

				if (nDispColors < 16)
					SetBkColor( ps.hdc, RGBWHITE ) ;
				else
	            SetBkColor( ps.hdc, RGBLTGRAY ) ; //GetSysColor( COLOR_BTNFACE ) ) ;

            Draw3DRect( ps.hdc, &rc, BORDERBEZEL, DRAW3D_OUT ) ;

            InflateRect( &rc, BORDERBEZEL-BORDERWIDTH,
                              BORDERBEZEL-BORDERWIDTH ) ;

            Draw3DRect( ps.hdc, &rc, BORDERBEZEL, DRAW3D_IN ) ;

            InflateRect( &rc, -(SPACING * 2), -(SPACING * 2) ) ;

            rc2 = rc ;

            // top box
            rc2.bottom = rc2.top + cyTopText ;
            rc3 = rc2 ;
            Draw3DRect( ps.hdc, &rc2, 1, DRAW3D_IN ) ;

            InflateRect( &rc3, -SPACING, -SPACING ) ;

				if (nDispColors < 16)
					ExtTextOut( ps.hdc, 0, 0, ETO_OPAQUE, &rc3, NULL, 0, NULL ) ;

            DrawText( ps.hdc, szInstructions, -1, &rc3,
                      DT_CENTER | DT_WORDBREAK ) ;

            // button space
            rc2.top = rc2.bottom + SPACING ;
            rc2.bottom = rc2.top + ((SPACING + cyButton) * 2) + SPACING * 2 ;
//            Draw3DRect( ps.hdc, &rc2, 1, DRAW3D_IN ) ;
            
            // bottom box
            //
				#if 0
            rc2.top = rc2.bottom + SPACING ;
            rc2.bottom = rcClient.bottom ;
            rc2.left += MIN_CXBOTTEXT + (4*SPACING) ;
            rc2.right -= MIN_CXBOTTEXT + (4*SPACING) ;
				#endif

				rc2.bottom = rcClient.bottom - SPACING ;
				rc2.top = rc2.bottom - cyBotText ;
            rc2.left += MIN_CXBOTTEXT + (4*SPACING) ;
            rc2.right -= MIN_CXBOTTEXT + (4*SPACING) ;
				//
				// This is for the hidden about box
				//
				rcBottom = rc2 ;

            Draw3DRect( ps.hdc, &rc2, 1, DRAW3D_IN ) ;

            InflateRect( &rc2, -SPACING, -SPACING ) ;

				if (nDispColors < 16)
					ExtTextOut( ps.hdc, 0, 0, ETO_OPAQUE, &rc2, NULL, 0, NULL ) ;

            DrawText( ps.hdc, szInfo, -1, &rc2,
                      DT_CENTER | DT_WORDBREAK ) ;


            SelectObject( ps.hdc, hOldFont ) ;

            EndPaint (hWnd, &ps) ;
         }
      }
      break ;

		case WM_SIZE:
		{
         HDC   hDC ;
         int   i;
         short x, y ;
			HFONT	hOldFont ;

         GetClientRect( hWnd, &rcMain ) ;

         rcClient = rcMain ;
         InflateRect( &rcClient, -(2 * BORDERWIDTH), -(2 * BORDERWIDTH)) ;

         if (hDC = GetDC( hWnd ))
         {
            RECT  rc ;

				hOldFont = SelectObject( hDC, hFont ) ;

            rc = rcMain ;
            InflateRect( &rc, -(SPACING * 3), -(SPACING * 3) ) ;

            //
            // Select the font in.
            //
            cyTopText = (2*SPACING)+4+
                        DrawText( hDC, szInstructions, -1, &rc,
                                 DT_CENTER |
                                 DT_CALCRECT |
                                 DT_WORDBREAK ) ;
            DP5( hWDB, "cyTopText = %d", cyTopText ) ;

            rc = rcMain ;
            InflateRect( &rc, -(MIN_CXBOTTEXT), -(SPACING * 3) ) ;

				cyBotText =    4+SPACING+
					            DrawText( hDC, szInfo, -1, &rc,
               		      DT_CENTER |
									DT_CALCRECT |
									DT_WORDBREAK ) ;
				cyBotText = max( cyBotText, MIN_CYBOTTEXT ) ;
				DP5( hWDB, "cyBotText= %d", cyBotText ) ;

            //
            // Unselect it
            //
            SelectObject( hDC, hOldFont ) ;
            ReleaseDC( hWnd, hDC ) ;
         }

         cxButton = ((rcClient.right - rcClient.left) -
                     SPACING * (1+NUMCOLS)) / NUMCOLS ;

			cyButton = ((rcClient.bottom - rcClient.top) -
							(2 * SPACING * (1+NUMROWS) + cyTopText + cyBotText)) /
							NUMROWS ;

         x = rcClient.left + (SPACING) ;
         y = rcClient.top + cyTopText + (2*SPACING) ;

         for (i = IDD_LFW ; i <= IDD_UTIL ; i++)
         {
				if (rghwndBtn[i])
					MoveWindow( rghwndBtn[i], x, y, cxButton, cyButton, FALSE ) ;

            if (i == (NUMCOLS-1))
            {
               x = rcClient.left + SPACING ;
               y += cyButton + SPACING ;
            }
            else
               x += cxButton + SPACING ;
         }

         x = rcClient.left + SPACING ;
         y = rcClient.bottom - (cyBotText + SPACING) ;

         i = IDD_HELP ;
         if (rghwndBtn[i])
				MoveWindow( rghwndBtn[i], x, y, MIN_CXBOTTEXT, cyBotText, FALSE ) ;

         x = rcClient.right - (MIN_CXBOTTEXT + SPACING ) ;
         i = IDD_BLANK ;
         if (rghwndBtn[i])
				MoveWindow( rghwndBtn[i], x, y, MIN_CXBOTTEXT, cyBotText, FALSE ) ;
		}
		break ;

      case WM_CLOSE:
      {
         FARPROC lpfnExit ;
         WORD    n ;

         DP5( hWDB, "WM_CLOSE" ) ;

         lpfnExit = MakeProcInstance( fnExit, hAppInst ) ;
         n = DialogBox( hAppInst, "EXITBOX", hWnd, lpfnExit ) ;
         FreeProcInstance( lpfnExit ) ;

         if (n)
         {
            //
            // Check to see if we are installed as the shell.  If so then
            // we want to do an ExitWindows
            //
            if (!AreWeShell())
               DestroyWindow( hWnd ) ;
            else
            {
               //
               // If ExitWindows fails (or even returns) some other app
               // is running and refuses to close
               //
               ExitWindows( NULL, 0 ) ;
            }

            WinExec( "lexblnk.exe /Q", SW_SHOWNORMAL ) ;
         }
      }
      break ;


	   case WM_DESTROY:

         if (hFont)
            DeleteObject( hFont ) ;

         D(wdbCloseSession(hWDB));
	      PostQuitMessage( 0 ) ;
	      break;

	   default:		
	      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }
   return 0L ;
}

/****************************************************************
 *  BOOL NEAR PASCAL AreWeShell( VOID )
 *
 *  Description:
 *
 *    Determines if we are the shell by looking in the system.ini
 *    file.  Know of a better way?
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL AreWeShell( VOID )
{
   char szShell[_MAX_PATH] ;

   GetPrivateProfileString( "boot",
                            "shell",
                            "progman.exe",
                            szShell,
                            _MAX_PATH,
                            "system.ini" ) ;
   if (lstrcmpi( szShell, "lpm.exe" ))
      return FALSE ;
   else
      return TRUE ;

} /* AreWeShell()  */

/********************************************************************
 *  InitApplication - Initialize the Class and register it
 ********************************************************************/
BOOL NEAR PASCAL InitApplication( HANDLE hInstance )
{
    WNDCLASS  wc;

   wc.style         = CS_DBLCLKS ; //CS_HREDRAW | CS_VREDRAW ;      
   wc.lpfnWndProc   = fnMainWnd;  
                                  
   wc.cbClsExtra    = 0;             
   wc.cbWndExtra    = 0;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, szAppName );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = ghbrBackground ; //COLOR_BTNFACE + 1 ;
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
   HDC            hDC ;

   x = GetSystemMetrics( SM_CXSCREEN ) ;
   y = GetSystemMetrics( SM_CYSCREEN ) ;

	x = min(x, 800) ;
	y = min(y, 600) ;

   hWnd = CreateWindow( szAppName,
                        szAppName,   
                        WS_POPUP | WS_CAPTION | WS_SYSMENU,
                        0, 0,
                        x, y,
                        NULL,                 
                        NULL,                 
                        hInstance,            
                        NULL                  
                     ) ;


   if (!hWnd)
       return FALSE ;


   DlgCenter( hWnd, NULL, TRUE ) ;

   ShowWindow( hWnd, nCmdShow ) ;

   UpdateWindow( hWnd ) ;

   return TRUE ;             
}

LONG FAR PASCAL fnSubClass( HWND hwndBmpBtn, WORD wMsg, WORD wParam, LONG lParam )
{
   WORD     wID = GetWindowWord( hwndBmpBtn, GWW_ID ) ;
   BOOL     fEnterDown = FALSE ;

   switch (wMsg)
   {
      case WM_KEYDOWN :
      {
         #if 0
         if (wParam == VK_RETURN)
         {
            fEnterDown = TRUE ;
            SendMessage( hwndBmpBtn, BM_SETSTATE, TRUE, 0L ) ;
         }
         else
         #endif
         {
            WORD i = wID ;

            switch (wParam)
            {
               case VK_UP:
                  if (i == IDD_LFW)
                  {
                     i = IDD_HELP ;
                     break ;
                  }
                  if (i == IDD_HELP)
                  {
                     i = IDD_CHECK ;
                     break ;
                  }

                  if (i == IDD_HOME)
                  {
                     i = IDD_BLANK ;
                     break ;
                  }
                  if (i == IDD_BLANK)
                  {
                     i = IDD_UTIL ;
                     break ;
                  }

                  if (i < NUMBUTTONS)
                  {
                     if (i < IDD_CHECK)
                        i += IDD_CHECK ;
                     else
                        i -= IDD_CHECK ;
                  }
               break ;

               case VK_DOWN:
                  if (i == IDD_CHECK)
                  {
                     i = IDD_HELP ;
                     break ;
                  }
                  if (i == IDD_HELP)
                  {
                     i = IDD_LFW ;
                     break ;
                  }

                  if (i == IDD_UTIL)
                  {
                     i = IDD_BLANK ;
                     break ;
                  }
                  if (i == IDD_BLANK)
                  {
                     i = IDD_HOME ;
                     break ;
                  }

                  if (i < NUMBUTTONS)
                  {
                     if (i < IDD_CHECK)
                        i += IDD_CHECK ;
                     else
                        i -= IDD_CHECK ;
                  }
               break ;

               case VK_LEFT:
                  if (i < NUMBUTTONS)
                  {
                     if (i == IDD_LFW)
                        i = IDD_HOME ;
                     else
                        if (i == IDD_CHECK)
                           i = IDD_UTIL ;
                        else
                           i-- ;
                  }
                  else
                     if (i == IDD_HELP)
                        i = IDD_BLANK ;
                     else
                        i = IDD_HELP ;
               break ;

               case VK_RIGHT:
                  if (i < NUMBUTTONS)
                  {
                     if (i == IDD_HOME)
                        i = IDD_LFW ;
                     else
                        if (i == IDD_UTIL)
                           i = IDD_CHECK ;
                        else
                           i++ ;
                  }
                  else
                     if (i == IDD_HELP)
                        i = IDD_BLANK ;
                     else
                        i = IDD_HELP ;
               break ;

               case VK_TAB:
                  if (GetKeyState( VK_SHIFT ) < 0)
                  {
                     if (i == IDD_LFW)
                        i = IDD_BLANK ;
                     else
                        i-- ;
                  }
                  else
                  {
                     if (i == IDD_BLANK)
                        i = IDD_LFW ;
                     else
                        i++ ;
                  }
               break ;

               case VK_F1:
                  SendMessage( GetParent( hwndBmpBtn ), WM_COMMAND,
                               IDD_HELP, 0L ) ;
               break ;
            }
            if (rghwndBtn[i])
               SetFocus( rghwndBtn[i] ) ;
         }

      }
      break ;

      #if 0
      case WM_KEYUP :
      {
         DP5( hWDB, "KeyUp" ) ;
         if (wParam == VK_RETURN)
         {
            fEnterDown = FALSE ;
            SendMessage( hwndBmpBtn, BM_SETSTATE, FALSE, 0L ) ;
         }
      }
      break ;
      #endif

      case WM_SETFOCUS:
         if (wID < NUMBUTTONS)
            wCurFocus = wID ;
         SendMessage( hwndBmpBtn, BM_SETSTYLE, BS_DEFPUSHBUTTON, 1L ) ;
      break ;

      case WM_KILLFOCUS:
         SendMessage( hwndBmpBtn, BM_SETSTYLE, BS_PUSHBUTTON, 1L ) ;
      break ;

   }

   if (rglpfnBtnOld[wID])
      return CallWindowProc( rglpfnBtnOld[wID], hwndBmpBtn,
                             wMsg, wParam, lParam ) ;
   else
      return 0L ;

}/* fnSubClass() */


/****************************************************************
 *  BOOL FAR PASCAL ExecApp( WORD wAppID )
 *
 *  Description: 
 *
 *    Execs the given app using settings in INI file
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL ExecApp( WORD wAppID )
{
   char  szCmdLine[_MAX_PATH + 128] ;
   char  szINI[_MAX_FNAME+_MAX_EXT] ;
   WORD  n ;

   wsprintf( szINI, "%s.INI", (LPSTR)szAppName ) ;

   if (!GetPrivateProfileString( INI_SECTION,
                                 rgszApp[wAppID],
                                 "",
                                 szCmdLine,
                                 _MAX_PATH+128,
                                 szINI ))
   {
      DP1( hWDB, "ExecApp: GetProfile failed!" ) ;
      return FALSE ;
   }

   if ((n = WinExec( szCmdLine, SW_NORMAL )) <= 32)
   {
      DP1( hWDB, "ExecApp: WinExec failed = %d", n ) ;
      return FALSE ;
   }
   DP5( hWDB, "ExecApp: WinExec OK = %d", n ) ;

   return TRUE ;
} /* ExecApp()  */


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
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         ShowWindow( GetDlgItem( hDlg, IDD_BOX0 ), SW_HIDE ) ;
         return ShadowControl( hDlg, wMsg, wParam, RGBGRAY ) ;
      break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            case IDCANCEL :
   	   	   EndDialog( hDlg, TRUE ) ;
   	   	   return TRUE ;
	      }
	      break ;

      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

    }

   return FALSE ;
} /* fnAbout()  */


/****************************************************************
 *  BOOL FAR PASCAL
 *    fnExit(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *    About box window procedure
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL fnExit(  HWND hDlg, WORD wMsg, WORD wParam, LONG lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
         SetFocus( GetDlgItem( hDlg, IDD_PWD ) ) ;
         DlgCenter( hDlg, GetParent( hDlg ), TRUE ) ;
         ShowWindow( GetDlgItem( hDlg, IDD_BOX0 ), SW_HIDE ) ;
         return ShadowControl( hDlg, wMsg, wParam, RGBCYAN ) ;
      break ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK :
            {
               char szPassword[64] ;
               char szBuf[64] ;

               GetDlgItemText( hDlg, IDD_PWD, szBuf, 64 ) ;
               GetPrivateProfileString( "Admin", "Password",
                                        "", szPassword,
                                        64, "lpm.ini" ) ;

               if (!lstrcmpi( szPassword, szBuf ))
      	   	   EndDialog( hDlg, TRUE ) ;
   	   	   return TRUE ;
            }

            case IDCANCEL :
   	   	   EndDialog( hDlg, FALSE ) ;
   	   	   return TRUE ;
	      }
	      break ;

      default:
         return ShadowControl( hDlg, wMsg, wParam, lParam ) ;

    }

   return FALSE ;
} /* fnExit()  */

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



/************************************************************************
 * End of File: lpm.c
 ***********************************************************************/

