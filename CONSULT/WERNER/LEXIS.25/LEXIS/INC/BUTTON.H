/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  button.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 ************************************************************************/


#define BU_GETHEIGHT WM_USER + 1

HWND FAR PASCAL CreateButtonWnd( HWND hwndParent ) ;
BOOL FAR PASCAL DestroyButtonWnd( HWND hwndButtonWnd ) ;
BOOL FAR PASCAL InitButtonWnd( HANDLE hInst ) ;
BOOL FAR PASCAL SetButtonPersonality( HWND hWnd, WORD wPer ) ;


/************************************************************************
 * End of File: button.h
 ************************************************************************/

