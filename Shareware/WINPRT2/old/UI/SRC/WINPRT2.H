/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  WINPRT2.h
 *
 ***********************************************************************/

#ifndef _WINPRT2_H_
#define _WINPRT2_H_

#include "..\res\WINPRT2.str"
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
 * Global Variables, defined in WINPRT2.c
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
 * End of File: WINPRT2.h
 ***********************************************************************/

