/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  global.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    The GLOBAL.H file contains all "extern <global var>" declarations
 *    and a module that needs global data should include it.
 *
 *    The global variables are all declared in the "global.c" module.
 *
 *   Revisions:  
 *     00.00.000  1/24/91 cek   first version
 *
 ************************************************************************/

#ifndef _MAX_PATH
#include <stdlib.h>
#endif

#ifndef MAX_STRLEN
#include "lexis.h"
#endif

/************************************************************************
 * Variable sizes
 ************************************************************************/
#define APPNAME_LEN  9
#define VERSION_LEN  10
#define CURRENTSTATE_LEN MAX_STRLEN

/************************************************************************
 * Data structures
 ************************************************************************/
typedef struct tagLEXISSTATE
{
   //
   // Appname.  This is the appname used in the window class and the one
   // that resides in the DEF file.
   //
   char szAppName[APPNAME_LEN] ;

   //
   // Holds the version number returned from VerMakeVersionNum
   //
   char szVersion[VERSION_LEN] ;
   
   //
   // Handle to the main window
   //
   HWND hwndMainWnd ;

   //
   // The number of colors supported on the display
   //
   WORD wNumColors ;

   //
   // Current state.  The value that this variable holds is really a
   // string table identifier for the state string.
   //
   WORD wState ;

   char szCurrentState[CURRENTSTATE_LEN] ;

   BOOL fBotStat      ;
   BOOL fTopStat      ;
   BOOL fButtonWnd    ;
   BOOL fFastPrint    ;

   BOOL fL2000Keyboard ;
   BOOL fBell ;

   WORD wPersonality ;        // CORPORATE, LEGAL, or COMBINED

   EXTLOGFONT  lfNormal ;
   EXTLOGFONT  lfHighlight ;

   //
   // Directories...
   //
   char szDiary   [_MAX_PATH] ;
   char szRecord  [_MAX_PATH] ;
   char szNotepad [_MAX_PATH] ;
   char szDocument[_MAX_PATH] ;

} LEXISSTATE, *PLEXISSTATE, FAR *LPLEXISSTATE ;

#ifndef GLOBAL
extern GLOBALHANDLE ghGlobals ;
#endif

/************************************************************************
 * Macros for accessing global data
 ************************************************************************/

/************************************************************************
 * Exported functions 
 ************************************************************************/

GLOBALHANDLE FAR PASCAL AllocGlobals( void ) ;
GLOBALHANDLE FAR PASCAL FreeGlobals( GLOBALHANDLE ghGlobals ) ;
LPSTR FAR PASCAL GetVersionNumber( LPSTR szVersion ) ;

/************************************************************************
 * End of File: global.h
 ************************************************************************/


