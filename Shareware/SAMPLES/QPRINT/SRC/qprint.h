/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  QPRINT.h
 *
 ***********************************************************************/

#ifndef _QPRINT_H_
#define _QPRINT_H_

#include "..\res\QPRINT.str"
#include "res.h"
#include "dlgs.h"

/**************************************************************************
 *************************************************************************/

/*
 * Macros
 */
#define GALLOC(n) GlobalAllocPtr(GHND, n)
#define GFREE(lp) GlobalFreePtr(lp)

/*
 * Global Variables, defined in QPRINT.c
 */
extern HWND        hwndApp ;

extern HINSTANCE   hinstApp ;

extern char        szVerNum[] ;

extern LPSTR       rglpsz[] ;

LRESULT FAR PASCAL fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;


BOOL WINAPI PrintFile( LPSTR lpszFile ) ;

#endif

/************************************************************************
 * End of File: QPRINT.h
 ***********************************************************************/

