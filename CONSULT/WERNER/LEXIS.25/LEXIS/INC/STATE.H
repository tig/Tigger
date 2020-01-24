/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  state.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 ************************************************************************/


#define LEGAL     0
#define CORP      1
#define COMBINED  2

BOOL FAR PASCAL SaveScreenPosition( HWND hWnd ) ;

WORD FAR PASCAL ReadScreenX( HANDLE hInst ) ;
WORD FAR PASCAL ReadScreenY( HANDLE hInst ) ;

WORD FAR PASCAL ReadScreenXsize( HANDLE hInst ) ;
WORD FAR PASCAL ReadScreenYsize( HANDLE hInst ) ;

int  FAR PASCAL ReadScreenShow( HANDLE hInst, int nCmdShow ) ;


WORD FAR PASCAL ReadPersonality( HANDLE hInst ) ;
BOOL FAR PASCAL WritePersonality( HANDLE hInst, WORD wPer ) ;

BOOL FAR PASCAL ReadBotStat( HANDLE hInst ) ;
BOOL FAR PASCAL ReadTopStat( HANDLE hInst ) ;
BOOL FAR PASCAL ReadButtonWnd( HANDLE hInst ) ;
BOOL FAR PASCAL ReadFastPrint( HANDLE hInst ) ;

BOOL FAR PASCAL WriteTopStat( HANDLE hInst, BOOL fStat ) ;
BOOL FAR PASCAL WriteBotStat( HANDLE hInst, BOOL fStat ) ;
BOOL FAR PASCAL WriteButtonWnd( HANDLE hInst, BOOL fStat ) ;
BOOL FAR PASCAL WriteFastPrint( HANDLE hInst, BOOL fStat ) ;

BOOL FAR PASCAL
WriteDirectories( HANDLE hInst,
                  LPSTR lpszDiary, LPSTR lpszNotePad,
                  LPSTR lpszRecord, LPSTR lpszDoc ) ;
BOOL FAR PASCAL
ReadDirectories( HANDLE hInst, LPSTR lpszDefault,
                  LPSTR lpszDiary, LPSTR lpszNotePad,
                  LPSTR lpszRecord, LPSTR lpszDoc ) ;

BOOL FAR PASCAL
ReadHeaderFooter( HANDLE hInst, LPSTR lpszHeader, LPSTR lpszFooter ) ;
BOOL FAR PASCAL
WriteHeaderFooter( HANDLE hInst, LPSTR lpszHeader, LPSTR lpszFooter ) ;

BOOL FAR PASCAL
WriteFonts( HANDLE hInst, LPEXTLOGFONT lplfNorm,LPEXTLOGFONT lplfHL ) ;
BOOL FAR PASCAL
ReadFonts( HANDLE hInst, LPEXTLOGFONT lplfNorm,LPEXTLOGFONT lplfHL ) ;

/************************************************************************
 * End of File: state.h
 ************************************************************************/

