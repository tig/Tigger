/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  mainwnd.h
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

BOOL FAR PASCAL SetPersonality( HWND hWnd, WORD wPer ) ;
void FAR PASCAL PlaceChildWindows( HWND hWnd, WORD wWidth, WORD wHeight ) ;

LONG FAR PASCAL fnMainWnd( HWND      hWnd,
                           unsigned  iMessage,  
                           WORD      wParam,			 
                           LONG      lParam ) ;


/************************************************************************
 * End of File: mainwnd.h
 ************************************************************************/

