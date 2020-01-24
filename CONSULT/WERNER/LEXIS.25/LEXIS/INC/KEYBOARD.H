/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  keyboard.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *
 ************************************************************************/
BOOL FAR PASCAL KeyDownTerm( HWND hWnd, WORD wParam, LONG lParam ) ;
BOOL FAR PASCAL CharTerm( HWND hWnd, WORD wParam, LONG lParam ) ;
void FAR PASCAL AddChar( PTERMBLOCK pTB, char c ) ;
void FAR PASCAL InsertChar( PTERMBLOCK pTB, char c ) ;




/************************************************************************
 * End of File: keyboard.h
 ************************************************************************/


