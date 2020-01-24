/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Common Screen Routines (PROSCRN.C)                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains all the fun stuff that is common to all    *
*             terminal emulators.                                             *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
* 1.1 dca00029 MKL 03/01/90 fixed cursor not appear underneath the last column*
* 1.1 dca00034 MKL 03/01/90 added one parameter to VidClearPage routine, so   *
*		 that the screen can be filled with any character as VT102    *
*		 screen alignment (^[#8) required.			      *
* 1.1 dca00040 MKL 03/01/90 fixed WindowWrap problem when iconized.	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include "xtalk.h"
#include "library.h"
#include "capture.h"
#include "comm.h"
#include "disp.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "properf.h"


#define VID_TYPE_DWIDTH     0x01
#define VID_TYPE_DHEIGHT    0x02

typedef int * PINT;


/*----------------------------------------------------------------------------*/
/*  Define variables used in the primary video page			      */
/*----------------------------------------------------------------------------*/
  PINT pPriVidLine[VID_MAXROW];
  int  nPriVidPage[VID_MAXROW][VID_MAXCOL+1];


/*----------------------------------------------------------------------------*/
/*  Define variables used in screen paging logic			      */
/*----------------------------------------------------------------------------*/
//  int  nVidLines[949];
//  int  nCurVidLine;
//  int  nCurVidPage;
//  int  nMaxVidLines;
//  int  nNumVidLines;
//  int  nNumVidPages;

//  char fVidScreenMode;

/*----------------------------------------------------------------------------*/
/*  Define Client Window variables					      */
/*----------------------------------------------------------------------------*/
  RECT VidClient;
  int  nVidCaretWidth;
  int  wVidCaretBlinkTime;


/*----------------------------------------------------------------------------*/
/*  Initialize the Virtual Video Screen logic.				      */
/*----------------------------------------------------------------------------*/
int InitVideo()
{
  PINT pInt;
  char sBuf[64];
  int  col, row;


  /*  Initialize primary video page */
  for (row = 0; row < VID_MAXROW; row++)
    {
    pPriVidLine[row] = &nPriVidPage[row][0];

    pInt = pPriVidLine[row];
    *pInt = NULL;
    for (col = 1; col <= VID_MAXCOL; col++)
      pInt[col] = 0x7020;
    }

//  nCurVidLine  = 0;
//  nCurVidPage  = 0;
//  nNumVidLines = 0;
//  nNumVidPages = 2;


  nVidCaretWidth     = nCurFontWidth;
  wVidCaretBlinkTime = GetCaretBlinkTime();

  return (NULL);
}


PINT GetVidLine(nLine)
  int  nLine;
{
  return (VssGetLine(nLine));
}


void VidScrollUp(nTop, nBot)
  int  nTop, nBot;
{
  HBRUSH hBrush;
  RECT	 Rect;
  PINT	 pInt;
  char	 szBuf[TERM_MAXCOL+1];
  int	 col, row;


  GetClientRect(hChildTerminal, &VidClient);

  if (nTop == 0 && nScrollMemory)
    VssAddLine(pPriVidLine[0]);

  if (bCaptureOn && Sess.cLogOption == SESSION_VISUAL)
    {
    register i;

    for (i = 1; i <= nTermMaxCol; i++)
      szBuf[i-1] = (char)(*(pPriVidLine[nTop]+i));
    szBuf[nTermMaxCol] = NULL;

    WriteStrToCapture(szBuf);
    }

  UpdateWindow(hChildTerminal);

  pInt = pPriVidLine[nTop];
  for (row = nTop; row < nBot; row++)
    pPriVidLine[row] = pPriVidLine[row+1];
  pPriVidLine[nBot] = pInt;

  *pInt = NULL;
  for (col = 1; col <= VID_MAXCOL; col++)
    pInt[col] = 0x7020;

  Rect.top = (nTop - nTermOffY + 1) * nCurFontHeight;
  if (Rect.top - nCurFontHeight < VidClient.top)
    Rect.top = VidClient.top;

  Rect.bottom = (nBot - nTermOffY + 1) * nCurFontHeight;
  if (Rect.bottom > VidClient.bottom)
    Rect.bottom = VidClient.bottom;

  if (Rect.top == VidClient.top && Rect.bottom == VidClient.bottom)
    {
    ScrollWindow(hChildTerminal, 0, -nCurFontHeight, NULL, NULL);
    }
  else
    {
    Rect.left  = VidClient.left;
    Rect.right = VidClient.right;
    ScrollWindow(hChildTerminal, 0, -nCurFontHeight, (LPRECT)&Rect, NULL);
    }

  Rect.top = (nBot - nTermOffY) * nCurFontHeight;


  if (bTermRevr)
    hBrush = CreateSolidBrush(ChrAttrColor[Disp.nForeground]);
  else
    hBrush = CreateSolidBrush(ChrAttrColor[Disp.nBackground]);
  FillRect(hTerminalDC, &Rect, hBrush);
  DeleteObject(hBrush);

  ValidateRect(hChildTerminal, &Rect);
}


void VidScrollDown(nTop, nBot)
  int  nTop, nBot;
{
  HBRUSH hBrush;
  RECT	 Rect;
  PINT	 pInt;
  int	 col, row;


  UpdateWindow(hChildTerminal);

  pInt = pPriVidLine[nBot];
  for (row = nBot; row > nTop; row--)
    pPriVidLine[row] = pPriVidLine[row-1];
  pPriVidLine[nTop] = pInt;

  *pInt = NULL;
  for (col = 1; col <= VID_MAXCOL; col++)
    pInt[col] = 0x7020;

  Rect.top = (nTop - nTermOffY) * nCurFontHeight;
  if (Rect.top < VidClient.top)
    Rect.top = VidClient.top;

  Rect.bottom = (nBot - nTermOffY) * nCurFontHeight;
  if (Rect.bottom + nCurFontHeight > VidClient.bottom)
    Rect.bottom = VidClient.bottom;

  if (Rect.top == VidClient.top && Rect.bottom == VidClient.bottom)
    {
    ScrollWindow(hChildTerminal, 0, nCurFontHeight, NULL, NULL);
    }
  else
    {
    Rect.left  = VidClient.left;
    Rect.right = VidClient.right;
    ScrollWindow(hChildTerminal, 0, nCurFontHeight, (LPRECT)&Rect, NULL);
    }

  Rect.bottom = Rect.top + nCurFontHeight;

  if (bTermRevr)
    hBrush = CreateSolidBrush(ChrAttrColor[Disp.nForeground]);
  else
    hBrush = CreateSolidBrush(ChrAttrColor[Disp.nBackground]);
  FillRect(hTerminalDC, &Rect, hBrush);
  DeleteObject(hBrush);

  ValidateRect(hChildTerminal, (LPRECT)&Rect);
}


void VidInsertChars(nChars)
  int  nChars;
{
  RECT	Rect;
  PINT	pInt;
  int	col, nWidth;


  UpdateWindow(hChildTerminal);

  pInt = pPriVidLine[nTermChrY];
  for (col = nTermMaxCol; col >= nTermChrX + nChars; col--)
    pInt[col] = pInt[col-nChars];

  for (col = nTermChrX; col < nTermChrX + nChars; col++)
    pInt[col] = 0x7020;

  if (*pInt & VID_TYPE_DWIDTH)
    nWidth = nCurFontWidth * 2;
  else
    nWidth = nCurFontWidth;

  Rect.top    = nTermChrY * nCurFontHeight;
  Rect.bottom = (nTermChrY + 1) * nCurFontHeight;
  Rect.left   = (nTermChrX - 1) * nWidth;
  Rect.right  = VidClient.right;

  ScrollWindow(hChildTerminal, nWidth * nChars, 0, (LPRECT)&Rect, NULL);
}


void VidDeleteChars(nChars)
  int  nChars;
{
  RECT	Rect;
  PINT	pInt;
  int	col, nWidth;


  UpdateWindow(hChildTerminal);

  pInt = pPriVidLine[nTermChrY];
  for (col = nTermChrX; col <= nTermMaxCol - nChars; col++)
    pInt[col] = pInt[col+nChars];

  for (col = nTermMaxCol - (nChars - 1); col <= nTermMaxCol; col++)
    pInt[col] = 0x7020;

  if (*pInt & VID_TYPE_DWIDTH)
    nWidth = nCurFontWidth * 2;
  else
    nWidth = nCurFontWidth;

  Rect.top    = nTermChrY * nCurFontHeight;
  Rect.bottom = (nTermChrY + 1) * nCurFontHeight;
  Rect.left   = nTermChrX * nWidth;     /* FIXED */
  Rect.right  = VidClient.right;

  ScrollWindow(hChildTerminal, -(nWidth * nChars), 0, (LPRECT)&Rect, NULL);
}


// dca00034 MKL added one parameter to VidClearPage routine, so that the 
//	screen can be filled with any character
void VidClearPage(wPattern)
int wPattern;  /* one character pattern to fill the page */
{
  HBRUSH hBrush;
  RECT	 Rect;
  char	 szBuf[TERM_MAXCOL+1];
  PINT	 pInt;
  int	 col, row, i;


  /*  Copy screen to scroll area if scroll is enabled  */
  if (nScrollMemory)
    for (row = 0; row < VID_MAXROW; row++)
      VssAddLine(pPriVidLine[row]);

  /*  Write to Capture File if ON and in Visual mode  */
  if (bCaptureOn && Sess.cLogOption == SESSION_VISUAL)
    {
    for (row = 0; row < VID_MAXROW; row++)
      {
      register i;

      for (i = 1; i <= nTermMaxCol; i++)
	szBuf[i-1] = (char)(*(pPriVidLine[row]+i));
      szBuf[nTermMaxCol] = NULL;

      WriteStrToCapture(szBuf);
      }
    }

  /*  Clear primary video page	*/
  for (row = 0; row < VID_MAXROW; row++)
    {
    pInt = pPriVidLine[row];
    *pInt = NULL;
    for (col = 1; col <= VID_MAXCOL; col++)
      pInt[col] = wPattern;
    }

  if (fTermCaretActive)
    HideCaret(hChildTerminal);

  GetClientRect(hChildTerminal, &Rect);
  if (bTermRevr)
    hBrush = CreateSolidBrush(ChrAttrColor[Disp.nForeground]);
  else
    hBrush = CreateSolidBrush(ChrAttrColor[Disp.nBackground]);
  FillRect(hTerminalDC, &Rect, hBrush);
  DeleteObject(hBrush);

  // dca00034 MKL
  // ValidateRect(hChildTerminal, NULL);
  if (wPattern != 0x7020) {
     InvalidateRect(hChildTerminal, NULL, FALSE);
     UpdateWindow(hChildTerminal);
     nTermChrX=1;
     nTermChrY=0;
     VidSetCaretPos();
  }
  else ValidateRect(hChildTerminal, NULL);

  if (fTermCaretActive)
    ShowCaret(hChildTerminal);
}


void VidClearArea(nX1, nY1, nX2, nY2)
  int  nX1, nY1;
  int  nX2, nY2;
{
  RECT	Rect;
  PINT	pInt;
  int	col, row;


  if (nY1 == nY2)
    {
    pInt = pPriVidLine[nY1];
    for (col = nX1; col <= nX2; col++)
      pInt[col] = 0x7020;

    if (*pInt & VID_TYPE_DWIDTH)
      {
      nX1 = 1;
      nX2 = nTermMaxCol;
      }
    }
  else
    {
    pInt = pPriVidLine[nY1];
    for (col = nX1; col <= nTermMaxCol; col++)
      pInt[col] = 0x7020;

    pInt = pPriVidLine[nY2];
    for (col = 1; col <= nX2; col++)
      pInt[col] = 0x7020;

    nX1 = 1;
    nX2 = nTermMaxCol;

    for (row = nY1 + 1; row < nY2; row++)
      {
      pInt = pPriVidLine[row];
      for (col = 1; col <= nTermMaxCol; col++)
	pInt[col] = 0x7020;
      }
    }

  if ((Rect.left = (nX1 - nTermOffX - 1) * nCurFontWidth) < 0)
    Rect.left = 0;

  Rect.right = (nX2 - nTermOffX) * nCurFontWidth;

  if ((Rect.top = (nY1 - nTermOffY) * nCurFontHeight) < 0)
    Rect.top = 0;

  Rect.bottom = (nY2 - nTermOffY + 1) * nCurFontHeight;

  InvalidateRect(hChildTerminal, (LPRECT)&Rect, TRUE);
  UpdateWindow(hChildTerminal);
}

/*
*
*
*
*
*/
void VidUpdateSize()
{
  static char fHorzVisible = FALSE;
  static char fVertVisible = FALSE;
  RECT Rect;
  int  nMinPos, nMaxPos;
  int  nVertMinPos, nVertMaxPos;
  int  nHorzMinPos, nHorzMaxPos;
  register int i;


  GetClientRect(hChildTerminal, (LPRECT)&VidClient);
  GetClientRect(hChildTerminal, (LPRECT)&Rect);

  if (VidClient.right == VidClient.left)
    return;

  if (fHorzVisible)
    GetScrollRange(hChildTerminal, SB_HORZ, &nHorzMinPos, &nHorzMaxPos);
  else
    nHorzMinPos = nHorzMaxPos = 0;

  if (fVertVisible)
    GetScrollRange(hChildTerminal, SB_VERT, &nVertMinPos, &nVertMaxPos);
  else
    nVertMinPos = nVertMaxPos = 0;

  if (nVertMinPos != nVertMaxPos)
    Rect.right += byScrollBarWidth;
  if (nHorzMinPos != nHorzMaxPos)
    Rect.bottom += byScrollBarHeight;

  nTermExtY = nTermWndY = (Rect.bottom - Rect.top) / nCurFontHeight;
  if (nTermExtY > TERM_MAXROW)
    nTermExtY = TERM_MAXROW;
  if (Disp.fShowVScroll)
    {
    nVertMinPos = 0;
    if (nTermExtY < TERM_MAXROW)
      nVertMaxPos = TERM_MAXROW - nTermExtY;
    else
      nVertMaxPos = 0;
    }
  else
    {
    nVertMinPos = 0;
    nVertMaxPos = 0;
    }

  i = Rect.right - Rect.left;
  if (nVertMinPos != nVertMaxPos || IfSystemStatus(SS_SCROLL))
    i -= byScrollBarWidth;
  nTermExtX = nTermWndX = i / nCurFontWidth;
  // dca00040 MKL fixed WindowWrap problem when iconized.
  // if (nTermExtX > nTermMaxCol)
  if (nTermExtX > nTermMaxCol || IsIconic(hWnd))
    nTermExtX = nTermMaxCol;

  if (Disp.fShowHScroll)
    {
    /* dca00040 MKL Comment out duplicated code
    i = Rect.right - Rect.left;
    if (nVertMinPos != nVertMaxPos || IfSystemStatus(SS_SCROLL))
      i -= byScrollBarWidth;
    nTermExtX = nTermWndX = i / nCurFontWidth;
    if (nTermExtX > nTermMaxCol)
      nTermExtX = nTermMaxCol;
    */
    nHorzMinPos = 0;
    if (nTermExtX < nTermMaxCol)
      nHorzMaxPos = nTermMaxCol - nTermExtX;
    else
      nHorzMaxPos = 0;
    }
  else
    {
    SetScrollRange(hChildTerminal, SB_HORZ, 0, 0, TRUE);
    fHorzVisible = FALSE;
    nHorzMinPos = 0;
    nHorzMaxPos = 0;
    }

  if (Disp.fShowVScroll)
    {
    if (nHorzMinPos != nHorzMaxPos)
      {
      nTermWndY = (Rect.bottom - Rect.top - byScrollBarHeight) / nCurFontHeight;
      nTermExtY = nTermWndY;
      if (nTermExtY > TERM_MAXROW)
	nTermExtY = TERM_MAXROW;
      if (nTermExtY < TERM_MAXROW)
	nVertMaxPos = TERM_MAXROW - nTermExtY;
      }
    }

  /* dca00040 MKL Comment out duplicated code
  if (Disp.fShowHScroll)
    {
    i = Rect.right - Rect.left;
    if (nVertMinPos != nVertMaxPos || IfSystemStatus(SS_SCROLL))
      i -= byScrollBarWidth;
    nTermExtX = nTermWndX = i / nCurFontWidth;
    if (nTermExtX > nTermMaxCol)
      nTermExtX = nTermMaxCol;
    nHorzMinPos = 0;
    if (nTermExtX < nTermMaxCol)
      nHorzMaxPos = nTermMaxCol - nTermExtX;
    else
      nHorzMaxPos = 0;
    }
  else
    {
    SetScrollRange(hChildTerminal, SB_HORZ, 0, 0, TRUE);
    fHorzVisible = FALSE;
    nHorzMinPos = 0;
    nHorzMaxPos = 0;
    }
  */
  if (nTermOffX > nTermMaxCol - nTermExtX)
    {
    nTermOffX = nTermMaxCol - nTermExtX;
    SetScrollPos(hChildTerminal, SB_HORZ, nTermOffX, TRUE);
    InvalidateRect(hChildTerminal, NULL, TRUE);
    }

  if (Disp.fShowHScroll)
    {
    if (fHorzVisible)
      GetScrollRange(hChildTerminal, SB_HORZ, &nMinPos, &nMaxPos);
    else
       nMinPos = nMaxPos = 0;
    if (nHorzMinPos != nMinPos || nHorzMaxPos != nMaxPos)
      {
      SetScrollRange(hChildTerminal, SB_HORZ, 0, nTermMaxCol-nTermExtX, TRUE);
      fHorzVisible = TRUE;
      }
    }
  else
    {
    SetScrollRange(hChildTerminal, SB_HORZ, 0, 0, TRUE);
    fHorzVisible = FALSE;
    }

  if (IfSystemStatus(SS_SCROLL))
    {
    SetScrollRange(hChildTerminal, SB_VERT, 0, nVssCurLineCnt + (VID_MAXROW - nTermExtY), FALSE);
    SetScrollPos(hChildTerminal, SB_VERT, nVssCurLineCnt + nVssCurLinePos, TRUE);
    fVertVisible = TRUE;
    }
  else
    {
    if (nTermOffY > TERM_MAXROW - nTermExtY)
      {
      nTermOffY = TERM_MAXROW - nTermExtY;
      SetScrollPos(hChildTerminal, SB_VERT, nTermOffY, TRUE);
      InvalidateRect(hChildTerminal, NULL, TRUE);
      }

    if (Disp.fShowVScroll)
      {
      SetScrollRange(hChildTerminal, SB_VERT, 0, nVertMaxPos, TRUE);
      if (nVertMaxPos)
	fVertVisible = TRUE;
      else
	fVertVisible = FALSE;
      }
    else
      {
      SetScrollRange(hChildTerminal, SB_VERT, 0, 0, TRUE);
      fVertVisible = FALSE;
      }
    }

  VidSetCaretPos();
}


void VidSeeMe(nRow)
  int nRow;
{
  register i;


  if (fLowResDisplay)
    return;

  i = 0;
  while (nRow < nTermOffY && i++ < 50)
    VidScroll(WM_VSCROLL, SB_LINEUP, 0L);

  i = 0;
  while (nRow >= nTermOffY + nTermExtY && i++ < 50)
    VidScroll(WM_VSCROLL, SB_LINEDOWN, 0L);
}


void VidScroll(message, wParam, lParam)
  unsigned message;
  WORD	   wParam;
  LONG	   lParam;
{
  int	   nType;
  int	   nPos;


  if ((wSystem & SS_SCROLL) && message == WM_VSCROLL)
    {
    switch (wParam)
      {
      case SB_LINEUP:
	VssScrollDown(1);
	break;

      case SB_LINEDOWN:
	VssScrollUp(1);
	break;

      case SB_TOP:
	VssGotoBot();
	break;

      case SB_PAGEUP:
	VssScrollDown(0);
	break;

      case SB_BOTTOM:
	VssGotoTop();
	break;

      case SB_PAGEDOWN:
	VssScrollUp(0);
	break;

      case SB_THUMBPOSITION:
	nVssCurLinePos = -(nVssCurLineCnt + (VID_MAXROW - nTermExtY)-((int)LOWORD(lParam)));
	SetScrollPos(hChildTerminal, SB_VERT, nVssCurLineCnt + nVssCurLinePos, TRUE);
	InvalidateRect(hChildTerminal, NULL, TRUE);
	UpdateWindow(hChildTerminal);
	break;
      }

    return;
    }

  if (message == WM_HSCROLL)
    {
    nPos = nTermOffX;

    switch (wParam)
      {
      case SB_LINEUP:
	if (nTermOffX > 0)
	  nPos = nTermOffX - 1;
	break;

      case SB_LINEDOWN:
	if (nTermOffX < nTermMaxCol - nTermExtX)
	  nPos = nTermOffX + 1;
	break;

      case SB_TOP:
      case SB_PAGEUP:
	nPos = 0;
	break;

      case SB_BOTTOM:
      case SB_PAGEDOWN:
	nPos = nTermMaxCol - nTermExtX;
	break;

      case SB_THUMBPOSITION:
	nPos = LOWORD(lParam);
	break;
      }

    if (nPos != nTermOffX)
      {
      if (!IsIconic(hWnd))
	{
	register i;

	i = (nTermOffX - nPos) * nCurFontWidth;
	nTermOffX = nPos;

	ScrollWindow(hChildTerminal, i, 0, NULL, NULL);
	SetScrollPos(hChildTerminal, SB_HORZ, nPos, TRUE);
	}
      else
	nTermOffX = nPos;
      }
    }
  else
    {
    nPos = nTermOffY;

    switch (wParam)
      {
      case SB_LINEUP:
	if (nTermOffY > 0)
	  nPos = nTermOffY - 1;
	break;

      case SB_LINEDOWN:
	if (nTermOffY < TERM_MAXROW - nTermExtY)
	  nPos = nTermOffY + 1;
	break;

      case SB_TOP:
      case SB_PAGEUP:
	nPos = 0;
	break;

      case SB_BOTTOM:
      case SB_PAGEDOWN:
	nPos = TERM_MAXROW - nTermExtY;
	break;

      case SB_THUMBPOSITION:
	nPos = LOWORD(lParam);
	break;
      }

    if (nPos != nTermOffY)
      {
      if (!IsIconic(hWnd))
	{
	register i;

	i = (nTermOffY - nPos) * nCurFontHeight;
	nTermOffY = nPos;

	ScrollWindow(hChildTerminal, 0, i, NULL, NULL);
	SetScrollPos(hChildTerminal, SB_VERT, nPos, TRUE);
	}
      else
	nTermOffY = nPos;
      }
    }

  UpdateWindow(hChildTerminal);
}

void VidClickAndPick(x1, y1)
  int x1, y1;
{
  long lTimeOut;
  char buf[64];
  register i, x;


  if (wSystemState != SS_TERMINAL)
    return;

  i  = 0;

  if (y1 < TERM_MAXROW)
    {
    x = x1;
    while (isalnum((char)pPriVidLine[y1][x]) && i < 40)
      buf[i++] = (char)pPriVidLine[y1][x++];

    if (Disp.fOneChar && i > 0)
      i = 1;
    }
  buf[i++] = '\r';

  VidSelectArea(x1, y1, x-1, y1);
  lTimeOut = GetCurrentTime() + 100;
  while (lTimeOut > GetCurrentTime());
  CommDataOutput(buf, i);
  VidSelectArea(x1, y1, x-1, y1);
}

void VidSelectArea(nX1, nY1, nX2, nY2)
  int  nX1, nY1;
  int  nX2, nY2;
{
  RECT	Rect;


  UpdateWindow(hChildTerminal);
  SelectObject(hTerminalDC, GetStockObject(BLACK_BRUSH));

  nY1 -= nVssCurLinePos;
  nY2 -= nVssCurLinePos;

  if (nY1 > nY2)
    {
    register int i;
    i	= nY1;
    nY1 = nY2;
    nY2 = i;
    i	= nX1;
    nX1 = nX2;
    nX2 = i;
    }

  if (nY1 == nY2)
    {
    if (nX1 > nX2)
      {
      register int i;
      i   = nX1;
      nX1 = nX2;
      nX2 = i;
      }

    Rect.left = (nX1 - nTermOffX - 1) * nCurFontWidth;
    Rect.right = (nX2 - nX1 + 1) * nCurFontWidth;
    Rect.top = (nY1 - nTermOffY) * nCurFontHeight;
    Rect.bottom = nCurFontHeight;
    PatBlt(hTerminalDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);
    }
  else
    {
    Rect.left = (nX1 - nTermOffX - 1) * nCurFontWidth;
    Rect.right = (nTermMaxCol - nX1 + 1) * nCurFontWidth;
    Rect.top = (nY1 - nTermOffY) * nCurFontHeight;
    Rect.bottom = nCurFontHeight;
    PatBlt(hTerminalDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);

    if (nY2 - nY1 > 1)
      {
      Rect.left = 0;
      Rect.right = (nTermMaxCol - nTermOffX) * nCurFontWidth;
      Rect.top = (nY1 - nTermOffY + 1) * nCurFontHeight;
      Rect.bottom = nCurFontHeight * (nY2 - nY1 - 1);
      PatBlt(hTerminalDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);
      }

    Rect.left = 0;
    Rect.right = (nX2 - nTermOffX) * nCurFontWidth;
    Rect.top = (nY2 - nTermOffY) * nCurFontHeight;
    Rect.bottom = nCurFontHeight;
    PatBlt(hTerminalDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);
    }
}


void VidSetCaretPos()
{
  int  nXPos, nYPos;


  if (!fTermFocusActive)
    return;

  if (*pPriVidLine[nTermChrY] & VID_TYPE_DWIDTH)
    { /* handle double width character mode */
    nXPos = ((nTermChrX - 1) * 2 - nTermOffX) * nCurFontWidth;

    if (nVidCaretWidth != nCurFontWidth * 2)
      {
      VidDeleteCaret(hChildTerminal);
      VidCreateCaret(hChildTerminal);
      }
    }
  else
    {
    // dca00029 MKL fixed cursor position at the last column
    // nXPos = ((nTermChrX - 1) - nTermOffX) * nCurFontWidth;
    if (nTermChrX > nTermMaxCol) 
	nXPos = ((nTermChrX - 2) - nTermOffX) * nCurFontWidth;
    else
	nXPos = ((nTermChrX - 1) - nTermOffX) * nCurFontWidth;

    if (nVidCaretWidth != nCurFontWidth)
      {
      VidDeleteCaret(hChildTerminal);
      VidCreateCaret(hChildTerminal);
      }
    }

  nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;
  if (Term.cCurShape == TERM_CURSOR_LINE)
    nYPos += nCurFontHeight - 1;

  SetCaretPos(nXPos, nYPos);
}

void VidCreateCaret(hWnd)
  HWND hWnd;
{
  wVidCaretBlinkTime = GetCaretBlinkTime();


  if (*pPriVidLine[nTermChrY] & VID_TYPE_DWIDTH)
    nVidCaretWidth = nCurFontWidth * 2;
  else
    nVidCaretWidth = nCurFontWidth;

  if (Term.cCurShape == TERM_CURSOR_BAR)
    CreateCaret(hWnd, NULL, NULL, nCurFontHeight);
  else if (Term.cCurShape == TERM_CURSOR_LINE)
    CreateCaret(hWnd, NULL, nVidCaretWidth, NULL);
  else
    CreateCaret(hWnd, 1, nCurFontWidth, nCurFontHeight);

  if (Disp.fNoBlink)
    SetCaretBlinkTime(-1);

  if (fTermCaretActive)
    ShowCaret(hChildTerminal);
}

void VidDeleteCaret(hWnd)
  HWND hWnd;
{
  SetCaretBlinkTime(wVidCaretBlinkTime);
  DestroyCaret();
}


/*----------------------------------------------------------------------------*/
/*  Scan user entered keys for possible file name.			      */
/*									      */
/*----------------------------------------------------------------------------*/
void VidNameScan(wChar)
  WORD wChar;
{





}
