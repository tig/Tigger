/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  lexis.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This is the main module for Lexis2000 2.5.  This module contains
 *    the message loop only.  This module will be
 *    PRELOAD MOVABLE.  The idea here is to make this module small and
 *    tight.
 *
 *
 *   Revisions:  
 *     02.50.000  1/20/91 cek   First version.
 *
 *
 ************************************************************************/

#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\init.h"
#include "..\inc\lexdlgd.h"

//
// lexis.ver contains VER_MAJOR, VER_MINOR, and VER_BUILD (managed by
// BUMPVER.EXE)
//
#include "..\lexis.ver"

//
// Global data
//
#include "..\inc\global.h"


/************************************************************************
 * Local Defines
 ************************************************************************/

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/


/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * Functions
 ************************************************************************/
/*************************************************************************
 *  int PASCAL WinMain(  )
 *
 *  Description:
 *    Main message loop for Lexis2000
 *
 *
 *  Comments:
 *
 *************************************************************************/
int FAR PASCAL WinMain(  HANDLE   hInstance,
                     HANDLE   hPrevInstance,
                     LPSTR    lpCmdLine,		  
                     int      nCmdShow )			  
{
   MSG msg;
   LPLEXISSTATE lpLS ;
   HWND hWnd ;
   HANDLE hAccTable ;

   D(
      if (!(hWDB=wdbOpenSession( hWnd,
                                 "Lexis2000",
                                 "WDB.INI",
                                 WDB_LIBVERSION )))
      {
         ODS( "wdb OpenSession Failed!\n\r" ) ;
      }
   ) ;

   //
   // Allocate memory for global variables
   // All global vars are kept in a LEXISTATE structure that is globally
   // allocated...
   //
   if (!(ghGlobals = AllocGlobals( )))
   {
      DP1( hWDB,  "Major Malfunction!  Could not global alloc global vars!") ;
      return FALSE ;
   }

   //
   // Initialize all global variables.  If we are the first instance
   // then this function will "generate" the data.  If this is not
   // the first instance then all stuff will be copied from the 
   // previous instance with the GetInstanceData() function.
   //
   if (!InitInstance( hPrevInstance, hInstance ))
   {
      DP1( hWDB, "Major malfunction!  Could not initialize instance!") ;
      FreeGlobals( ghGlobals ) ;
      return FALSE ;
   }

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
   {
      DP1( hWDB, "Major Malfunction! Could not lock global memory!") ;
      FreeGlobals( ghGlobals ) ;
      return FALSE ;
   }

   #ifdef DEBUG
   {
      char szDLLVer[10] ;

      VerGetCMNDLLVersion( szDLLVer ) ;

      DP1( hWDB,  "" ) ;
      DP1( hWDB,  "*******************************************************" ) ;
      DP1( hWDB,  "- %s Version %s (CMNDLL Version %s) ",
            (LPSTR)lpLS->szAppName, (LPSTR)lpLS->szVersion, (LPSTR)szDLLVer ) ;
      DP1( hWDB,  "*******************************************************" ) ;

   }
   #endif

   //
   // Create the main window
   //
   if (!( lpLS->hwndMainWnd = CreateMainWnd( hInstance, nCmdShow )))
   {
      DP1( hWDB, "Major malfunction! Could not create main window!") ;
      GlobalUnlock( ghGlobals ) ;
      FreeGlobals( ghGlobals ) ;
      return FALSE ;
   }
   hWnd = lpLS->hwndMainWnd ;

   hAccTable = LoadAccelerators( hInstance, lpLS->szAppName ) ;

   GlobalUnlock( ghGlobals ) ;

   //
   // Reset time and date formats to those in the WIN.INI
   //
   TimeResetInternational() ;


   //
   // Main message loop
   //
   while (GetMessage( &msg, NULL, NULL, NULL ))	 
  	{
      if (!TranslateAccelerator( hWnd, hAccTable, &msg ))
      {
         TranslateMessage( &msg ) ;
     	   DispatchMessage( &msg ) ;
      }
   }

   FreeGlobals( ghGlobals ) ;

   DP1( hWDB,  "*******************************************************" ) ;
   DP1( hWDB,  "- LEXIS 2000 Normal Exit" ) ;
   DP1( hWDB,  "*******************************************************" ) ;

   return msg.wParam ;	  
}/* WinMain() */


#if 0
HWND hwndCurF1Wnd = NULL ; // this is the current dialog box that is
                           // trapping the F1 key.

BOOL FAR PASCAL TrapF1Key( HWND hWnd )
{
   SetProp( hWnd, "PREVWND", hwndCurF1Wnd ) ;
   hwndCurF1Wnd = hWnd ;
   return TRUE ;
}

BOOL FAR PASCAL TranslateF1Key( LPMSG lpMsg )
{
   if (!hwndCurF1Wnd)
      return TRUE ;

   if (lpMsg->message == WM_KEYDOWN)
   {
      DP5( hWDB, "TranslateF1Key (WM_KEYDOWN)") ;
   }
   if (hwndCurF1Wnd == lpMsg->hwnd &&
       lpMsg->message == WM_KEYDOWN &&
       lpMsg->wParam == VK_F1)
      PostMessage( hwndCurF1Wnd, WM_COMMAND, IDD_HELP, 0L ) ;
   else
      return TRUE ;

   return FALSE ;
}

BOOL FAR PASCAL UnTrapF1Key( HWND hWnd )
{
   hwndCurF1Wnd = RemoveProp( hWnd, "PREVWND" ) ;
   return TRUE ;
}
#endif



/************************************************************************
 * End of File: lexis.c
 ************************************************************************/

