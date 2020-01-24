/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  timer.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

BOOL FAR PASCAL CreateTimer( HWND hWnd ) ;
BOOL FAR PASCAL DestroyTimer( HWND hWnd ) ;

//
// Function exported in EXPORTS!
//
WORD FAR PASCAL
fnTimer( HWND hWnd, WORD iMessage, WORD wParam, LONG lParam ) ;

/************************************************************************
 * End of File: timer.h
 ************************************************************************/

