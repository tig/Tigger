/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Screen Routines (PROSCRN.C)                                      */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <memory.h>
#include "lexis.h"
#include "library.h"
#include "record.h"
#include "comm.h"
#include "data.h"
#include "term.h"
#include "session.h"


typedef int * PINT;


/*----------------------------------------------------------------------------*/
/*  Define variables used in the primary video page			      */
/*----------------------------------------------------------------------------*/
  PINT pPriVidLine[VID_MAXROW];
  int  nPriVidPage[VID_MAXROW][VID_MAXCOL+1];


/*----------------------------------------------------------------------------*/
/*  Define variables used in screen paging logic			      */
/*----------------------------------------------------------------------------*/
  FILE *pVidLines;
  int  nVidLines[949];
  int  nCurVidLine;
  int  nCurVidPage;
  int  nMaxVidLines;
  int  nNumVidLines;
  int  nNumVidPages;

  char fVidScreenMode;

/*----------------------------------------------------------------------------*/
/*  Define Client Window variables					      */
/*----------------------------------------------------------------------------*/
  RECT VidClient;


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

  nCurVidLine  = 0;
  nCurVidPage  = 0;
  nNumVidLines = 0;
  nNumVidPages = 2;

  return (NULL);
}


int TermVideo()
{
  if (pVidLines)
    {
    fclose(pVidLines);
    pVidLines = NULL;
    }

  return (NULL);
}


PINT GetVidLine(nLine)
  int  nLine;
{
  return (pPriVidLine[nLine]);
}



void VidPageUp()
{
}

void VidPageDown()
{
}

void VidLineUp()
{
}

void VidLineDown()
{
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
    hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_BLACK]);
  else
    hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_WHITE]);
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
    hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_BLACK]);
  else
    hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_WHITE]);
  FillRect(hTerminalDC, &Rect, hBrush);
  DeleteObject(hBrush);

  ValidateRect(hChildTerminal, (LPRECT)&Rect);
}


void VidInsertChars(int nChars)
  {
  RECT	Rect;
  PINT	pInt;
  int	col, nWidth;
  short sLine, sPosX, chr, chr1, chr2;


  UpdateWindow(hChildTerminal);


  sLine = nTermChrY;
  sPosX = nTermChrX;
  chr   = 0x7020;
  while (1)
    {
    pInt = pPriVidLine[sLine];
    chr1 = pInt[TERM_MAXCOL];
    chr2 = pInt[TERM_MAXCOL-1];
    for (col = TERM_MAXCOL; col >= sPosX + nChars; col--)
      pInt[col] = pInt[col-nChars];

    pInt[sPosX] = chr;
    chr = chr1;

    if (sLine == nTermChrY)
      for (col = nTermChrX; col < nTermChrX + nChars; col++)
        pInt[col] = 0x7020;

    nWidth = nCurFontWidth;

    Rect.top    = sLine * nCurFontHeight;
    Rect.bottom = (sLine + 1) * nCurFontHeight;
    Rect.left   = (sPosX - 1) * nWidth;
    Rect.right  = 100 * nWidth;

    ScrollWindow(hChildTerminal, nWidth * nChars, 0, (LPRECT)&Rect, NULL);

    if (sLine > 20 || (chr == 0x7020 && chr2 == 0x7020))
      break;

    sLine++;
    sPosX = 1;
    nChars = 1;
    }
  }


void VidDeleteChars()
  {
  RECT	Rect;
  PINT	pInt;
  int	col, nWidth;
  short sLine, sPosX, chr;


  sLine = nTermChrY;
  sPosX = nTermChrX;
  while (1)
    {
    UpdateWindow(hChildTerminal);

    pInt = pPriVidLine[sLine];
    for (col = sPosX; col <= TERM_MAXCOL - 1; col++)
      pInt[col] = pInt[col+1];

    for (col = TERM_MAXCOL; col <= TERM_MAXCOL; col++)
      pInt[col] = 0x7020;

    nWidth = nCurFontWidth;

    Rect.top    = sLine * nCurFontHeight;
    Rect.bottom = (sLine + 1) * nCurFontHeight;
    Rect.left   = (sPosX) * nWidth;
    Rect.right  = 100 * nWidth;

    ScrollWindow(hChildTerminal, -nWidth, 0, (LPRECT)&Rect, NULL);

    sPosX = 1;

    /*
    ** If the new line to scan is off the page then break.
    */
    if (++sLine >= TERM_MAXROW)
      break;

    /*
    ** If the new line to delete a character from is all blanks then break.
    */
    pInt = pPriVidLine[sLine];
    for (col = 1; col <= TERM_MAXCOL; col++)
      {
      if (pInt[col] != 0x7020)
        break;
      }
    if (col > TERM_MAXCOL)
      break;

    /*
    ** Move the deleted character to the previous row.
    */
    chr = pInt[1];
    pInt = pPriVidLine[sLine-1];
    pInt[TERM_MAXCOL] = chr;
    }

  UpdateWindow(hChildTerminal);
  }


void VidClearPage()
{
  HBRUSH hBrush;
  RECT	 Rect;
  char	 szBuf[TERM_MAXCOL+1];
  PINT	 pInt;
  int	 col, row, i;


  /*  Clear primary video page	*/
  for (row = 0; row < VID_MAXROW; row++)
    {
    pInt = pPriVidLine[row];
    *pInt = NULL;
    for (col = 1; col <= VID_MAXCOL; col++)
      pInt[col] = 0x7020;
    }

  if (fTermCaretActive)
    HideCaret(hChildTerminal);

  GetClientRect(hChildTerminal, &Rect);
  if (bTermRevr)
    hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_BLACK]);
  else
    hBrush = CreateSolidBrush(ChrAttrColor[TERM_ATTR_WHITE]);
  FillRect(hTerminalDC, &Rect, hBrush);
  DeleteObject(hBrush);

  ValidateRect(hChildTerminal, NULL);

  if (fTermCaretActive)
    ShowCaret(hChildTerminal);
}


void VidClearArea(int nX1, int nY1, int nX2, int nY2)
  {
  RECT	Rect;
  PINT	pInt;
  int	col, row;


  if (nY1 == nY2)
    {
    pInt = pPriVidLine[nY1];
    for (col = nX1; col <= nX2; col++)
      pInt[col] = 0x7020;
    }
  else
    {
    pInt = pPriVidLine[nY1];
    for (col = nX1; col <= TERM_MAXCOL; col++)
      pInt[col] = 0x7020;

    pInt = pPriVidLine[nY2];
    for (col = 1; col <= nX2; col++)
      pInt[col] = 0x7020;

    nX1 = 1;
    nX2 = TERM_MAXCOL;

    for (row = nY1 + 1; row < nY2; row++)
      {
      pInt = pPriVidLine[row];
      for (col = 1; col <= TERM_MAXCOL; col++)
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


void VidUpdateSize()
{
  static char fVertVisible = FALSE;
  RECT Rect;
  int  nMinPos, nMaxPos;
  int  nVertMinPos, nVertMaxPos;
  int  nHorzMinPos, nHorzMaxPos;
  register int i;


  GetClientRect(hChildTerminal, (LPRECT)&VidClient);

  GetClientRect(hChildTerminal, (LPRECT)&Rect);
  GetScrollRange(hChildTerminal, SB_VERT, &nVertMinPos, &nVertMaxPos);
  GetScrollRange(hChildTerminal, SB_HORZ, &nHorzMinPos, &nHorzMaxPos);

  if (!fVertVisible)
    nVertMinPos = nVertMaxPos = 0;

  if (nVertMinPos != nVertMaxPos)
    Rect.right += byScrollBarWidth;
  if (nHorzMinPos != nHorzMaxPos)
    Rect.bottom += byScrollBarHeight;

  if (1)//Disp.fShowVScroll)
    {
    nTermExtY = nTermWndY = (Rect.bottom - Rect.top) / nCurFontHeight;
    if (nTermExtY > TERM_MAXROW)
      nTermExtY = TERM_MAXROW;
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

  if (1)//Disp.fShowHScroll)
    {
    i = Rect.right - Rect.left;
    if (nVertMinPos != nVertMaxPos)
      i -= byScrollBarWidth;
    nTermExtX = nTermWndX = i / nCurFontWidth;
    if (nTermExtX > TERM_MAXCOL)
      nTermExtX = TERM_MAXCOL;
    nHorzMinPos = 0;
    if (nTermExtX < TERM_MAXCOL)
      nHorzMaxPos = TERM_MAXCOL - nTermExtX;
    else
      nHorzMaxPos = 0;
    }
  else
    {
    SetScrollRange(hChildTerminal, SB_HORZ, 0, 0, TRUE);
    nHorzMinPos = 0;
    nHorzMaxPos = 0;
    }

  if (1)//Disp.fShowVScroll)
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

  if (1)//Disp.fShowHScroll)
    {
    i = Rect.right - Rect.left;
    if (nVertMinPos != nVertMaxPos)
      i -= byScrollBarWidth;
    nTermExtX = nTermWndX = i / nCurFontWidth;
    if (nTermExtX > TERM_MAXCOL)
      nTermExtX = TERM_MAXCOL;
    nHorzMinPos = 0;
    if (nTermExtX < TERM_MAXCOL)
      nHorzMaxPos = TERM_MAXCOL - nTermExtX;
    else
      nHorzMaxPos = 0;
    }
  else
    {
    SetScrollRange(hChildTerminal, SB_HORZ, 0, 0, TRUE);
    nHorzMinPos = 0;
    nHorzMaxPos = 0;
    }

  if (nTermOffX > TERM_MAXCOL - nTermExtX)
    {
    nTermOffX = TERM_MAXCOL - nTermExtX;
    SetScrollPos(hChildTerminal, SB_HORZ, nTermOffX, TRUE);
    InvalidateRect(hChildTerminal, NULL, TRUE);
    }

  if (1)//Disp.fShowHScroll)
    {
    GetScrollRange(hChildTerminal, SB_HORZ, &nMinPos, &nMaxPos);
    if (nHorzMinPos != nMinPos || nHorzMaxPos != nMaxPos)
      SetScrollRange(hChildTerminal, SB_HORZ, 0, TERM_MAXCOL-nTermExtX, TRUE);
    }
  else
    SetScrollRange(hChildTerminal, SB_HORZ, 0, 0, TRUE);

  if (nTermOffY > TERM_MAXROW - nTermExtY)
    {
    nTermOffY = TERM_MAXROW - nTermExtY;
    SetScrollPos(hChildTerminal, SB_VERT, nTermOffY, TRUE);
    InvalidateRect(hChildTerminal, NULL, TRUE);
    }

  if (1)//Disp.fShowVScroll)
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
        if (nTermOffX < TERM_MAXCOL - nTermExtX)
	  nPos = nTermOffX + 1;
	break;

      case SB_TOP:
      case SB_PAGEUP:
	nPos = 0;
	break;

      case SB_BOTTOM:
      case SB_PAGEDOWN:
        nPos = TERM_MAXCOL - nTermExtX;
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


void VidClickAndPick(int x1, int y1, WORD message)
  {
  long lTimeOut;
  char buf[64];
  register i, x;


  if (wSystemState != SS_TERMINAL)
    return;

  if (((char)pPriVidLine[y1][x1]) == ' ')
    return;

  while (x1 > 1 && isalnum((char)pPriVidLine[y1][x1-1]))
    x1--;

  i  = 0;
  x = x1;
  while ((isalnum((char)pPriVidLine[y1][x]) || (char)pPriVidLine[y1][x] == '-') && i < 40)
    buf[i++] = (char)pPriVidLine[y1][x++];

  buf[i] = NULL;

  VidSelectArea(x1, y1, x-1, y1);
  if (message)
    {
    lTimeOut = GetCurrentTime() + 100;
    while (lTimeOut > GetCurrentTime());
    if (i >= 4 && i <= 8)
      {
      if (memicmp(buf, "cite", 4) == 0)
        SessCanCommand(COMMAND_CITE);
      else if (memicmp(buf, "full", 4) == 0)
        SessCanCommand(COMMAND_FULL);
      else if (memicmp(buf, "kwic", 4) == 0)
        SessCanCommand(COMMAND_KWIC);
      else if (memicmp(buf, "segmts", 6) == 0)
        SessCanCommand(COMMAND_SEGMENTS);
      else
        SessTellCommand(buf, i);
      }
    else
      SessTellCommand(buf, i);

    VidSelectArea(x1, y1, x-1, y1);
    }
  else
    {
    nSelectX1 = x1;
    nSelectY1 = y1;
    nSelectX2 = x - 1;
    nSelectY2 = y1;
    fSelectedArea = TRUE;
    }
  }


void VidSelectArea(nX1, nY1, nX2, nY2)
  int  nX1, nY1;
  int  nX2, nY2;
{
  RECT	Rect;


  UpdateWindow(hChildTerminal);
  SelectObject(hTerminalDC, GetStockObject(BLACK_BRUSH));


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
    Rect.right = (TERM_MAXCOL - nX1 + 1) * nCurFontWidth;
    Rect.top = (nY1 - nTermOffY) * nCurFontHeight;
    Rect.bottom = nCurFontHeight;
    PatBlt(hTerminalDC, Rect.left, Rect.top, Rect.right, Rect.bottom, DSTINVERT);

    if (nY2 - nY1 > 1)
      {
      Rect.left = 0;
      Rect.right = (TERM_MAXCOL - nTermOffX) * nCurFontWidth;
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

  nXPos = ((nTermChrX - 1) - nTermOffX) * nCurFontWidth;

  nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;
  if (Term.cCurShape == TERM_CURSOR_LINE)
    nYPos += nCurFontHeight - 1;

  SetCaretPos(nXPos, nYPos);
}

void VidCreateCaret(hWnd)
  HWND hWnd;
{
  if (fTermInsert)
    CreateCaret(hWnd, NULL, NULL, nCurFontHeight);
  else
    CreateCaret(hWnd, 1, nCurFontWidth, nCurFontHeight);

  if (fTermCaretActive)
    ShowCaret(hChildTerminal);
}

void VidDeleteCaret(hWnd)
  HWND hWnd;
{
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
