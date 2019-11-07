/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  GENERIC.h
 *
 ***********************************************************************/

#ifndef _GENERIC_H_
#define _GENERIC_H_

#include "..\res\GENERIC.str"
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
 * Global Variables, defined in GENERIC.c
 */
extern HWND        hwndApp ;

extern HINSTANCE   hinstApp ;

extern char        szVerNum[] ;

extern LPSTR       rglpsz[] ;

LRESULT FAR PASCAL fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;


#endif

/************************************************************************
 * End of File: GENERIC.h
 ***********************************************************************/

