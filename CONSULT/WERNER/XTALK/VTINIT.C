/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   VT102 Init Module (VTINIT.C)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*	1.1  04/01/90 dca00064 PJL Set VT102 font when loading terminal.		   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "term.h"
#include "dialogs.h"


  HFONT hVT102Fonts[16];

void VT102Init()
{
	char szBuf[16];

	nTermChrX   = 1;
	nTermChrY   = 0;
	cTermAttr   = 0;
	nTermCSet   = 0;
	bTermWrap   = FALSE;
	nTermErase  = 0;

	nSaveChrX   = 1;
	nSaveChrY   = 0;
	nSaveAttr   = 0;
	nSaveCSet   = 0;
	bSaveWrap   = FALSE;
	nSaveErase  = 0;

	nTermMaxCol = 80;

	nTermOffX   = 0;
	nTermOffY   = 0;

	// dca00064 PJL Automatically set VT102 font.
	ResetFont(TRUE);
	VT102CreateFonts();

	SendMessage(hChildTerminal, UM_RESET, 0, 0L);

	LoadString(hWndInst, IDS_EMULATOR_VT102, szBuf, sizeof(szBuf));
	SendMessage(hChildStatus, UM_EMULATOR, (WORD)szBuf, 0L);
}


void VT102Term()
{
  if (nTermMaxCol == 132)
    {
    nTermMaxCol = 80;
    VidUpdateSize();
    }

  VT102DeleteFonts();
}


void VT102CreateFonts()
{
  int i;

  for (i = 0; i < sizeof(hVT102Fonts) / sizeof(HFONT); i++)
    hVT102Fonts[i] = NULL;

  VT102UpdateFonts();
}

void VT102DeleteFonts()
{
  int i;

  SelectObject(hTerminalDC, hCurFont);
  cTermFontAttr = -1;

  for (i = 0; i < sizeof(hVT102Fonts) / sizeof(HFONT); i++)
    {
    if (hVT102Fonts[i] != NULL)
      {
      DeleteObject(hVT102Fonts[i]);
      hVT102Fonts[i] = NULL;
      }
    }
}

void VT102UpdateFonts()
{
  LOGFONT LogFont;


  VT102DeleteFonts();

  memset(&LogFont, 0, sizeof(LOGFONT));
  GetObject(hCurFont, sizeof(LOGFONT), (LPSTR)&LogFont);

  LogFont.lfHeight    = nCurFontHeight;
  LogFont.lfWidth     = nCurFontWidth;
  LogFont.lfCharSet   = bCurFontCharSet;

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = FALSE;
  hVT102Fonts[0] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[1] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = TRUE;
  hVT102Fonts[2] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[3] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfHeight    = nCurFontHeight * 2;
  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = FALSE;
  hVT102Fonts[4] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[5] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = TRUE;
  hVT102Fonts[6] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[7] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfHeight    = nCurFontHeight;
  LogFont.lfWidth     = nCurFontWidth * 2;
  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = FALSE;
  hVT102Fonts[8] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[9] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = TRUE;
  hVT102Fonts[10] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[11] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfHeight    = nCurFontHeight * 2;
  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = FALSE;
  hVT102Fonts[12] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[13] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 400;
  LogFont.lfUnderline = TRUE;
  hVT102Fonts[14] = CreateFontIndirect((LPLOGFONT)&LogFont);

  LogFont.lfWeight    = 700;
  hVT102Fonts[15] = CreateFontIndirect((LPLOGFONT)&LogFont);
}
