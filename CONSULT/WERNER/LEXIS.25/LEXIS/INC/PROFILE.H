/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  profile.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 ************************************************************************/

// Maximum number of characters allowed in an ini string
#define MAX_INILEN 256

LPSTR FAR PASCAL ProfileRead( LPSTR lpszSection,
                              LPSTR lpszKey,
                              LPSTR lpszDefault,
                              LPSTR lpszValue ) ;

BOOL FAR PASCAL ProfileReadTrueFalse( LPSTR lpszSection,
                                      LPSTR lpszKey,
                                      BOOL  fDefault ) ;

int FAR PASCAL ProfileReadInt( LPSTR lpszSection,
                               LPSTR lpszKey,
                               int   nDefault ) ;

DWORD FAR PASCAL ProfileReadDW( LPSTR  lpszSection,
                                LPSTR  lpszKey,
                                DWORD dwDefault ) ;

int FAR PASCAL ProfileWrite( LPSTR lpszSection,
                             LPSTR lpszKey,
                             LPSTR lpszValue ) ;

int FAR PASCAL ProfileWriteInt( LPSTR lpszSection,
                                LPSTR lpszKey,
                                int   nValue ) ;
DWORD FAR PASCAL ProfileWriteDW( LPSTR lpszSection,
                                 LPSTR lpszKey,
                                 DWORD dwValue ) ;

/************************************************************************
 * End of File: profile.h
 ************************************************************************/

