/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  init.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *       This module handles all initialization of the applcation and
 *       window classes.  The idea here is have a module that is used
 *       at startup, does it's job, then is discarded...
 *
 *   Revisions:  
 *     00.00.000  1/20/91 cek   First version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"

#include <windows.h>
#include <cmndll.h>
#include <stdlib.h>

#include "..\inc\lexis.h"
#include "..\inc\mainwnd.h"
#include "..\inc\init.h"
#include "..\inc\state.h"
#include "..\inc\button.h"
#include "..\inc\filedlgs.h"
#include "..\inc\print.h"
#include "..\inc\term.h"

#include "..\inc\global.h"

/************************************************************************
 * Local Defines
 ************************************************************************/
//
// Define MAXIMIZEBYDEFAULT if the main window is to come up Maximized
// the very first time lexis is run on a system.  
// Do not define it if the main window is to come up "normal" the
// first time it's run.
//
// In both cases the window will come up according to how it was when
// it was last closed thereafter...
//
#define MAXIMIZEBYDEFAULT

/************************************************************************
 * Macros 
 ************************************************************************/
#define SYSERROR(a)

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
BOOL NEAR PASCAL InitWndClass( HANDLE hPrevInst, HANDLE hInstance ) ;
void NEAR PASCAL GetPrevInstanceData( HANDLE hPrevInstance ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External Functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL InitInstance( HANDLE hPrevInstance, HANDLE hInstance )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL InitInstance( HANDLE hPrevInstance, HANDLE hInstance )
{
   LPLEXISSTATE lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
   {
      SYSERROR(0) ;
      return FALSE ;
   }

   if (!LoadString( hInstance, IDS_APPNAME, lpLS->szAppName, APPNAME_LEN - 1 ))
   {
      SYSERROR(0) ;
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   //
   // global.c exports this function.  
   //
   GetVersionNumber( lpLS->szVersion ) ;

   //
   // Initialize the window classes (this call also intializes "bmpbtn"
   // and "status"
   //
	if (!InitWndClass( hPrevInstance, hInstance ))
   {
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }


   GlobalUnlock( ghGlobals ) ;

   return TRUE ;

}/* InitInstance() */


/*************************************************************************
 *  HWND FAR PASCAL CreateMainWnd( HANDLE hInstance, int nCmdShow ) ;
 *
 *  Description:
 *    This function handles the creation of the main window.  It is called
 *    from WndMain.  It creates, moves, and shows the window as appropriate.
 *
 *  Type/Parameter
 *          Description
 *    HANDLE hInstance
 *          Handle to the current instance.
 *
 *    int   nCmdShow
 *          How is the window supposed to be shown?
 *          
 * 
 *  Return Value
 *    HWND        Handle to the main window.  0 if it failed.
 *
 *  Comments:
 *
 *************************************************************************/
HWND FAR PASCAL CreateMainWnd( HANDLE hInstance, int nCmdShow ) 
{
   HWND  hWnd, hwndStat ;
   HDC   hDC ;

   int   xSize, xPos,
         ySize, yPos ;

   LPLEXISSTATE lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
   {
      SYSERROR(0) ;
      return FALSE ;
   }

   // 
   //  Get stored window size...
   //
   xPos = ReadScreenX( hInstance ) ;
   yPos = ReadScreenY( hInstance ) ;

   xSize = ReadScreenXsize( hInstance ) ;
   ySize = ReadScreenYsize( hInstance ) ;

   //
   // Get the device caps for the display.  The main thing we get here
   // is the number of colors
   //
   if (hDC = CreateDC( "DISPLAY", NULL, NULL, NULL ))
   {
      lpLS->wNumColors = GetDeviceCaps( hDC, NUMCOLORS ) ;
   
      DeleteDC( hDC ) ;
   }
   else
   {
      SYSERROR(0) ;
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   // 
   //  Get some personality
   //
   lpLS->wPersonality = ReadPersonality( hInstance ) ;

   lpLS->fTopStat = ReadTopStat( hInstance ) ;

   lpLS->fBotStat = ReadBotStat( hInstance ) ;

   lpLS->fButtonWnd = ReadButtonWnd( hInstance ) ;

   hWnd = CreateWindow( lpLS->szAppName,
                        NULL,   
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                        xPos, yPos,
                        xSize, ySize,
                        NULL,                 
                        NULL,                   // use class menu
                        hInstance,            
                        NULL                  
                     ) ;

   if (!hWnd)
   {
      DP1( hWDB, "CreateWindow failed! (MainWindow)" ) ;
      SYSERROR(0) ;
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   //
   // Make the window and it's children visible
   // Use the INI settings if they exist. 
   //
   #ifdef MAXIMIZEBYDEFAULT
   ShowWindow( hWnd, ReadScreenShow( hInstance, SW_SHOWMAXIMIZED) ) ;
   #else
   ShowWindow( hWnd, ReadScreenShow( hInstance, nCmdShow ) ) ;
   #endif

   lpLS->fFastPrint = ReadFastPrint( hInstance ) ;

   //
   // Get directories
   // and initilize file dialog boxes (this way they will "track"
   // the users dir changes throughout a session without affecting the
   // "saved/default" directories.
   //
   ReadDirectories( hInstance, "",
                           lpLS->szDiary, lpLS->szNotepad,
                           lpLS->szRecord, lpLS->szDocument ) ;

   //
   // Initialize the directories that are local to filedlgs.c
   // These dirs are kept so the dlg boxes can remember where
   // the user was last...
   //
   InitDirectories( lpLS->szDiary, lpLS->szNotepad,
                    lpLS->szRecord, lpLS->szDocument ) ;

   //
   // Setup the "Default" fonts...
   //
   lstrcpy( lpLS->lfNormal.lfFaceName, "Courier" ) ;
   lpLS->lfNormal.lfFGcolor = RGBBLACK ;
   lpLS->lfNormal.lfBGcolor = RGBWHITE ;

   lstrcpy( lpLS->lfHighlight.lfFaceName, "Courier" ) ;
   lpLS->lfHighlight.lfFGcolor = RGBBLACK ;
   lpLS->lfHighlight.lfBGcolor = RGBWHITE ;
   lpLS->lfHighlight.lfUnderline = TRUE ;
   ReadFonts( hInstance, &(lpLS->lfNormal), &(lpLS->lfHighlight) ) ;
   FontSelectIndirect( NULL, (LPLOGFONT)&(lpLS->lfNormal) ) ;
   FontSelectIndirect( NULL, (LPLOGFONT)&(lpLS->lfHighlight) ) ;

   if (hwndStat = GETHWNDTOPSTAT( hWnd ))
   {
      ShowWindow( hwndStat, SW_NORMAL ) ;
      UpdateWindow( hwndStat ) ;
   }

   if (hwndStat = GETHWNDBOTSTAT( hWnd ))
   {
      ShowWindow( hwndStat, SW_NORMAL ) ;
      UpdateWindow( hwndStat ) ;
   }

   if (hwndStat = GETHWNDBUTTON( hWnd ))
   {
      ShowWindow( hwndStat, SW_NORMAL ) ;
      UpdateWindow( hwndStat ) ;
   }

   UpdateWindow( hWnd ) ;

   GlobalUnlock( ghGlobals ) ;

   // 
   // Initialize the printer strings that are local to print.c
   //
   InitPrinter( hWnd ) ;

   if (hwndStat = GETHWNDTERM( hWnd ))
   {
      ShowWindow( hwndStat, SW_NORMAL ) ;
      ResetTerm( hwndStat) ;
      ClearTerm( hwndStat) ;
      LockTermKeyboard( hwndStat, FALSE ) ;
      UpdateWindow( hwndStat ) ;
      SetFocus( hwndStat ) ;
   }


   return hWnd ;             

}/* CreateMainWnd() */


/************************************************************************
 * Internal APIs
 ************************************************************************/
/*************************************************************************
 *  BOOL NEAR PASCAL InitWndClass( HANDLE hPrevInst, HANDLE hInstance ) ;
 *
 *  Description:
 *    This function intializes all window classes used by Lexis.
 *
 *  Type/Parameter
 *          Description
 *    HANDLE   hPrevInst
 *          Instance handle to the previous instance of lexis.
 *
 *    HANDLE   hInstance
 *          Instance handle to the current instance.
 * 
 *  Return Value
 *          Returns one of the following values:
 *    TRUE  Initialized sucessfully.
 *    FALSE Initizlization failed.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL InitWndClass( HANDLE hPrevInst, HANDLE hInstance ) 
{
   WNDCLASS  wc;

   LPLEXISSTATE lpLS ;

   // 
   // If there's already a copy running just return
   //
   if (hPrevInst)
      return TRUE ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
   {
      SYSERROR(0) ;
      return FALSE ;
   }

   //
   // Register the main window class
   //
   wc.style         = 0 ;

   wc.lpfnWndProc   = fnMainWnd;  
                                  
   wc.cbClsExtra    = 0 ;             
   wc.cbWndExtra    = NUM_WNDEXTRABYTES ;             
   wc.hInstance     = hInstance;      
   wc.hIcon         = LoadIcon( hInstance, lpLS->szAppName );
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = COLOR_APPWORKSPACE + 1 ; 
   wc.lpszMenuName  = NULL ;                    // no class menu!
   wc.lpszClassName = lpLS->szAppName;

   GlobalUnlock( ghGlobals ) ;
   /* Register the window class and return success/failure code. */

   if (!RegisterClass( &wc ))
   {
      SYSERROR(0) ;
      return FALSE ;
   }

   if (!InitTerm( hInstance ))
   {
      SYSERROR(0) ;
      return FALSE ;
   }


   if (!InitButtonWnd( hInstance ))
   {
      SYSERROR(0) ;
      return FALSE ;
   }

   return TRUE ;

}/* InitWndClass() */


/************************************************************************
 * End of File: init.c
 ************************************************************************/

