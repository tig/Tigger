/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  lexis.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *       This is the main include file for LEXIS.  It includes data types,
 *       macros, and constants that are used globally across the applcation.
 *
 *   Revisions:  
 *     00.00.000  1/20/91 cek   First version.
 *
 ************************************************************************/


/************************************************************************
 * Defines
 ************************************************************************/

// 
// Include resource id defines
//
#include "..\inc\lexisd.h"

// define TIMEDISPLAY if you want the time displayed on the top 
// status bar.
// #define TIMEDISPLAY        

#define MAX_STRLEN   80    // Maximum string resource string length

/************************************************************************
 * Window Word constants
 ************************************************************************/
// 
// The window handles to each of the "main" child windows (top status,
// bottom status, and the terminal window) are stored in the main
// window's "Window Extra Bytes" and are accessed through GetWindowWord
//
#define NUM_WNDEXTRABYTES  4 * sizeof(WORD)  // for the main window class

#define GWW_HWNDTOPSTAT    0
#define GWW_HWNDBOTSTAT    2
#define GWW_HWNDTERM       4
#define GWW_HWNDBUTTON     6

/************************************************************************
 * Macros 
 ************************************************************************/
#define GETHINST(hwnd)        GetWindowWord( hwnd, GWW_HINSTANCE )
#define GETHWNDTOPSTAT(hwnd)  GetWindowWord( hwnd, GWW_HWNDTOPSTAT ) 
#define GETHWNDBOTSTAT(hwnd)  GetWindowWord( hwnd, GWW_HWNDBOTSTAT ) 
#define GETHWNDTERM(hwnd)     GetWindowWord( hwnd, GWW_HWNDTERM ) 
#define GETHWNDBUTTON(hwnd)   GetWindowWord( hwnd, GWW_HWNDBUTTON ) 

/************************************************************************
 * Data structures
 ************************************************************************/

/************************************************************************
 * Functions
 ************************************************************************/
#if 0
BOOL FAR PASCAL UnTrapF1Key( HWND hWnd );
BOOL FAR PASCAL TrapF1Key( HWND hWnd ) ;
BOOL FAR PASCAL TranslateF1Key( LPMSG lpMsg ) ;
#endif
/************************************************************************
 * End of File: lexis.h
 ************************************************************************/

