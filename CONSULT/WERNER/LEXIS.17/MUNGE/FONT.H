/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Font Include File (FONT.H)                                       */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

extern HANDLE hSysFont;
extern short  nSysFontWidth;
extern short  nSysFontHeight;

extern HANDLE hCurFont;
extern short  nCurFontWidth;
extern short  nCurFontHeight;
extern BYTE   bCurFontCharSet;

extern BOOL InitFont();
extern void TermFont();
extern void NewFontData();
extern void UpdateFont();
