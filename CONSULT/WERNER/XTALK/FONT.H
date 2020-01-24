/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Font Include File (FONT.H)                                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

struct stFont
  {
  char szFaceName[16];
  char cPitchAndFamily;
  char cCharSet;
  char cHeight;
  char cWidth;
  char cFill1;
  char cFill2;
  };

extern struct stFont Font;

extern HANDLE hSysFont;
extern short  nSysFontWidth;
extern short  nSysFontHeight;

extern HANDLE hCurFont;
extern short  nCurFontWidth;
extern short  nCurFontHeight;
extern BYTE   bCurFontCharSet;

extern HANDLE hInfoFont;
extern HANDLE hMacroFont;

extern char fVT102Font;

extern BOOL InitFont();
extern void TermFont();
extern void NewFontData();
extern void UpdateFont();
