/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Display Init (DISPLO.C)                                         *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the display data init code.                *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include "xtalk.h"
#include "disp.h"
#include "term.h"


BOOL InitDisplay()
{
  NewDisplayData();

  return (TRUE);
}

void NewDisplayData()
{
  Disp.fDisplayNotes  = FALSE;
  Disp.fOneChar       = FALSE;
  Disp.fSlowCur       = FALSE;
  Disp.fShowStatusBar = TRUE;
  Disp.fShowMacroBar  = TRUE;
  Disp.fShowInfoBar   = TRUE;
  Disp.fShowVScroll   = TRUE;
  Disp.fShowHScroll   = TRUE;
  Disp.fShowInput     = FALSE;
  Disp.fMacron	      = FALSE;
  Disp.fNoBlink       = FALSE;
  Disp.nForeground    = TERM_ATTR_BLACK;
  Disp.nBackground    = TERM_ATTR_WHITE;
  Disp.nBolding       = TERM_ATTR_BLUE;
  Disp.nBlinking      = TERM_ATTR_RED;
}
