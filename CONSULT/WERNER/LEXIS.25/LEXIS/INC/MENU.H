/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  menu.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 ************************************************************************/

BOOL FAR PASCAL SetLexisMenu( HWND hWnd, WORD wMenu ) ;
BOOL FAR PASCAL CleanupLexisMenu( HWND hWnd ) ;

BOOL FAR PASCAL
ProcessMenu( HWND hWnd, WORD iMsg, WORD wParam, LONG lParam ) ;

/************************************************************************
 * End of File: menu.h
 ************************************************************************/

