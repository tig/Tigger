/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  global.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module contains all global data.  Any global variables defined
 *    for LEXIS 2000 are declared here.  It would be optimal if they
 *    could be only referenced through function calls, but that would
 *    impose too much of a speed penalty.  Thus we'll just name them
 *    very carefully, and make sure that they are commented where they
 *    are used.
 *
 *    The GLOBAL.H file contains all "extern <global var>" declarations
 *    and a module that needs global data should include it.
 *
 *    NOTE! NOTE! NOTE!
 *
 *       All global variable names are preceded by a "g_" to help
 *       identify them as global variables!!!!
 *
 *
 *   Revisions:  
 *     00.00.000  1/24/91 cek   first version
 *
 ************************************************************************/

//
// Make this module compile FAST!!
//
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSYTLES
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define NOOEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOMB
//#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSOUND
//#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS

#include <windows.h>
#include <cmndll.h>

#include "..\inc\lexis.h"

#define GLOBAL
#include "..\inc\global.h"

#include "..\lexis.ver"

//
// This is the ONLY global variable used in Lexis 2.5 besides
// "local globals" local only to a particular module...
//
GLOBALHANDLE ghGlobals ;

/************************************************************************
 * External functions
 ************************************************************************/

/*************************************************************************
 *  GLOBALHANDLE FAR PASCAL AllocGlobals( void )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
GLOBALHANDLE FAR PASCAL AllocGlobals( void )
{
   return GlobalAlloc( GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof( LEXISSTATE ) ) ;
}/* AllocGlobals() */

/*************************************************************************
 *  GLOBALHANDLE FAR PASCAL FreeGlobals( GLOBALHANDLE ghGlobals )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
GLOBALHANDLE FAR PASCAL FreeGlobals( GLOBALHANDLE ghGlobals )
{
   return GlobalFree( ghGlobals ) ;
}/* FreeGlobals() */

/*************************************************************************
 *  LPSTR GetVersionNumber( LPSTR szVersion )
 *
 *  Description:
 *    This function simply builds the version number using the version
 *    defines (VER_*) included from the "lexis.ver" file (which is
 *    updated in the makefile each build by BUMPVER.EXE).
 *
 *  Comments:
 *    This function is here because this module will compile very quickly
 *    because it has very little overhead.
 *
 *************************************************************************/
LPSTR FAR PASCAL GetVersionNumber( LPSTR szVersion )
{
   return VerMakeVersionNumber(  szVersion,
                                 VER_MAJOR,
                                 VER_MINOR,
                                 VER_BUILD ) ;
}/* GetVersionNumber() */


/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * End of File: global.c
 ************************************************************************/

