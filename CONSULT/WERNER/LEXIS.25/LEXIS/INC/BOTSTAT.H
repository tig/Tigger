/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  botstat.h
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


HWND FAR PASCAL CreateBotStatus( HWND hwndParent )  ;
BOOL FAR PASCAL DestroyBotStatus( HWND hwndStatus ) ;
void FAR PASCAL SetCDIndicator( HWND hwndStat, BOOL fOn ) ;
void FAR PASCAL SetRecordIndicator( HWND hwndStat, BOOL fOn ) ;
void FAR PASCAL SetDiaryIndicator( HWND hwndStat, BOOL fOn ) ;
void FAR PASCAL SetPrintIndicator( HWND hwndStat, BOOL fOn ) ;
void FAR PASCAL CheckKeyStates( void ) ;

 
// 
// Function in EXPORTS!
//
LONG FAR PASCAL
fnBotStat( HWND hwndStat, WORD iMessage, WORD wParam, LONG lParam ) ;


/************************************************************************
 * End of File: botstat.h
 ************************************************************************/


