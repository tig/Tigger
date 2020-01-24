/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  filedlgs.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *
 ************************************************************************/
#define OPEN_RECORD     0
#define OPEN_NOTEPAD    1
#define OPEN_DIARY      2

#define NEW_RECORD     0
#define NEW_NOTEPAD    1
#define NEW_DIARY      2

#define PRINT_RECORD     0
#define PRINT_NOTEPAD    1
#define PRINT_DIARY      2
#define PRINT_DOCUMENT   3

WORD FAR PASCAL
OpenFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType ) ;

WORD FAR PASCAL
NewFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType ) ;

WORD FAR PASCAL
PrintFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType ) ;

void FAR PASCAL
InitDirectories( LPSTR lpszDiary, LPSTR lpszNotepad,
              LPSTR lpszRecord, LPSTR lpszDoc ) ;

/************************************************************************
 * End of File: filedlgs.h
 ************************************************************************/

