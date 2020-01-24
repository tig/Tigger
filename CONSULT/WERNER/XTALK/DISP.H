/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Display Include File (DISPLAY.H)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

struct stDisp
  {
  char	fDisplayNotes;
  char	fOneChar;
  char	fSlowCur;
  char	fShowStatusBar;
  char	fShowMacroBar;
  char	fShowInfoBar;
  char	fShowVScroll;
  char	fShowHScroll;
  char	fShowInput;
  char	fMacron;
  char	fNoBlink;
  int	nForeground;
  int	nBackground;
  int	nBolding;
  int	nBlinking;
};

extern struct stDisp Disp;

extern BOOL InitDisplay();
extern void NewDisplayData();
