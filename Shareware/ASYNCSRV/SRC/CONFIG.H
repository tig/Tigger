/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  config.h
 *
 ***********************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

VOID WINAPI     SaveOptions( VOID ) ;
VOID FAR PASCAL RestoreDefaults( VOID ) ;

VOID FAR PASCAL RestoreWndPos( HWND hWnd ) ;
VOID FAR PASCAL SaveWndPos( HWND hWnd ) ;

#endif

/************************************************************************
 * End of File: config.h
 ***********************************************************************/


