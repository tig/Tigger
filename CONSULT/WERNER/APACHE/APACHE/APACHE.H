/************************************************************************
 *
 *    Copyright (c) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  Apache for Windows
 *
 *      Module:  apache.h
 *
 *      Author:  Bryan A. Woodruff (baw)
 *
 *
 *     Remarks:  header file
 *
 *   Revisions:  
 *     01.00.000  1/ 7/91 baw   Initial Version
 *
 ************************************************************************/

#include "facelift.h"   // FACELIFT.DLL exported information
#include "taskmgr.h"    // TASKMGR.DLL exported information

#include "resource.h"   // resource information

/************************************************************************
 *
 * Module definitions
 *    apache.c
 *    
 ************************************************************************/

// constant definitions

#define INIT_MENU_POS       0
#define TERMINAL_MENU_POS   3

#define FONTWEIGHT          10
#define FONTFACE            "Helv"

#define SEEK_SET            0
#define SEEK_CUR            1
#define SEEK_END            2

#define MAXLEN_VERSION      11
#define MAXLEN_PATH         164
#define MAXLEN_FILENAME     32
#define MAXLEN_MODULENAME   12
#define MAXLEN_MENUITEM     65

#define MAXLEN_MODELESSDLGS 16

#define GWW_SCB             0
#define GWW_TERMEXTRABYTES  2

// definitions for time/date update timer

#define TIMERID_UPDATE      0x1000
#define TIMERFREQ_UPDATE    1000

#define TIMERID_INIT        0x1234
#define TIMERFREQ_INIT      2000

#define NOENTRY             "(none)"

// atoms

#define ATOM_INITBMP               0x0100

#define ATOM_SESSIONDIRPROCH       0x0101
#define ATOM_SESSIONDIRPROCL       0x0102

// data structures

typedef struct tagSTATMSGS
{
   char  *szStat[2] ;

} STATMSGS ;

typedef VOID FAR PASCAL FNTRANSLATEANDDISPATCH( LPMSG ) ;
typedef FNTRANSLATEANDDISPATCH FAR *LPFNTRANSLATEANDDISPATCH ;

// macros

#define GETHINST( hWnd )    ((HINSTANCE) GetWindowWord( hWnd, GWW_HINSTANCE ))
#define GETSCB( hWnd )      ((HSCB) GetWindowWord( hWnd, GWW_SCB ))

// Initial bitmap display macros

#define SET_BMP_PROP( x, y ) SetProp( x, MAKEINTATOM( ATOM_INITBMP ), y )
#define GET_BMP_PROP( x )    GetProp( x, MAKEINTATOM( ATOM_INITBMP ) )
#define REMOVE_BMP_PROP( x ) RemoveProp( x, MAKEINTATOM( ATOM_INITBMP ) )

// global definitions

#ifdef GLOBALDEFS
   char      szAppName[] = "Apache" ;
   char      szFrameClass[] = "Apache::Frame" ;
   char      szTerminalClass[] = "Apache::Terminal" ;
   char      szEmulatorClass[] = "EmulatorClass" ;
   char      szSplashClass[] = "Apache::SplashClass" ;
   char      szTitle[] = "Apache For Windows" ;
   char      szVersion[MAXLEN_VERSION] ;
   BOOL      fCapsLock, fScrollLock, fNumLock ;
   HANDLE    hAppInst, hAccel, hTaskList ;
   HFONT     hStatFont ;
   HMENU     hMenuInit, hMenuTerminal ;
   HMENU     hMenuInitWindow, hMenuTerminalWindow ;
   HWND      hFrameWnd, hClientWnd, hStatusWnd ;
   HWND      ahModelessDlgs[ MAXLEN_MODELESSDLGS ] ;
   WNDPROC   lpMDIClientProc ;
#else
   extern char      szAppName[] ;
   extern char      szFrameClass[] ;
   extern char      szTerminalClass[] ;
   extern char      szEmulatorClass[] ;
   extern char      szSplashClass[] ;
   extern char      szTitle[] ;
   extern char      szVersion[] ;
   extern BOOL      fCapsLock, fScrollLock, fNumLock ;
   extern HANDLE    hAppInst, hAccel, hTaskList ;
   extern HFONT     hStatFont ;
   extern HMENU     hMenuInit, hMenuTerminal ;
   extern HMENU     hMenuInitWindow, hMenuTerminalWindow ;
   extern HWND      hFrameWnd, hClientWnd, hStatusWnd ;
   extern HWND      ahModelessDlgs[ MAXLEN_MODELESSDLGS ] ;
   extern WNDPROC   lpMDIClientProc ;
#endif

// function prototypes (public)

LRESULT FAR PASCAL FrameWndProc( HWND, UINT, WPARAM, LPARAM ) ;
VOID NotifyMenuState( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL CloseEnumProc( HWND, LPARAM ) ;
LRESULT FAR PASCAL TerminalWndProc( HWND, UINT, WPARAM, LPARAM ) ;
LRESULT FAR PASCAL SplashWndProc( HWND, UINT, WPARAM, LPARAM ) ;

// function prototypes (imported)

BOOL FAR PASCAL SetTaskTimer( HWND ) ;
BOOL FAR PASCAL KillTaskTimer( HWND ) ;
BOOL FAR PASCAL ClearTaskPostFlag( HWND ) ;

/************************************************************************
 * End of File: apache.h
 ************************************************************************/

