/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  topstat.h
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

HWND FAR PASCAL CreateTopStatus( HWND hwndParent )  ;
BOOL FAR PASCAL DestroyTopStatus( HWND hwndStatus ) ;

//
// EXPORTS!!!
//
LONG FAR PASCAL
fnTopStat( HWND hwndStat, WORD iMessage, WORD wParam, LONG lParam ) ;


/************************************************************************
 * End of File: topstat.h
 ************************************************************************/

