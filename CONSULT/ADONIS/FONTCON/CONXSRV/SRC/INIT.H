/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  init.h
 *
 ***********************************************************************/

#ifndef _INIT_H_
#define _INIT_H_

int WINAPI MyWinMain( HINSTANCE hinst, HINSTANCE hinstPrev,
                      LPCSTR lpszCmd, int nCmdShow ) ;
BOOL WINAPI InitClass( HINSTANCE hInstance ) ;
GLOBALHANDLE WINAPI GetStrings( HINSTANCE hInst ) ;
BOOL WINAPI DoCmdLine( LPSTR lpszResult ) ;
HWND WINAPI CreateMain( VOID ) ;

#endif


/************************************************************************
 * End of File: init.h
 ***********************************************************************/

