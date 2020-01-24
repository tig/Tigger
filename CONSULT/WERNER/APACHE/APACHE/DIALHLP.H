//---------------------------------------------------------------------------
//
//  Module: dialhlp.h
//
//  Purpose:
//     Header file for dialhlp.c
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/ 2/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

// function prototypes

HSCB OpenPort( HWND, WORD ) ;
VOID ClosePort( HSCB ) ;
BOOL CreateConnector_New( HWND, HSCB, LPSTR ) ;
BOOL DestroyConnector_New( HSCB ) ;
BOOL CreateEmulator_New( HWND, HSCB, WORD ) ;

BOOL GetPhoneINIString( HINSTANCE, WORD, int, LPSTR, int ) ;
VOID ConvertModemString( LPSTR, LPSTR ) ;
int WriteModemString( HSCB, LPSTR ) ;
int CheckModemResult( HWND, WORD, NPSTR, PWORD ) ;

//---------------------------------------------------------------------------
//  End of File: dialhlp.h
//---------------------------------------------------------------------------

