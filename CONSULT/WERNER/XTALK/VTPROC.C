/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   VT102 Proc Module (VTPROC.C)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*    1.1 03/01/90 dca00035 MKL added cursor enable/disable capability	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "properf.h"
#include "comm.h"
#include "disp.h"
#include "interp.h"
#include "prodata.h"

static BOOL near RefreshLine(int, int, int);
static void near RefreshStr(int, int, PSTR, int, int);
static void near SetCurColor(char);
static void near CursorMovement(int, int);
       void near TermInput(char *, int);


long WndVT102Proc(hWnd, message, wParam, lParam )
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  PAINTSTRUCT ps;
  HBRUSH hBrush;
  PSTR pAdr;
  int  nXPos, nYPos, nOffX, nOffY, nExtX, nExtY, i, x, y;
  int  nX1, nX2, nY1, nY2, col, row;
  char szBuf[64];


  switch (message)
    {
    case UM_RESET:
      TermSetup();
      break;

    case UM_DATA:
      TermInput((PSTR)LOWORD(lParam), wParam);

      if (fMouseCursor && Disp.fSlowCur)
	{
	if (nTermChrX == nCursorToX && nTermChrY == nCursorToY)
	  fMouseCursor = FALSE;
	else if (nTermChrX != nCursorAtX || nTermChrY != nCursorAtY)
	  CursorMovement(nCursorToX, nCursorToY);
	}
      break;

    case WM_CREATE:
      SetScrollRange(hWnd, SB_HORZ, 0, 0, FALSE);
      SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);
      break;

    case WM_CHAR:

      if (IfSystemStatus(SS_SCRIPT))
	{
        if (InterpreterKeyboard(wParam, 1))
          break;
	}
      if (TermNameScan(wParam)) break;

      szBuf[0] = (char)wParam;
      i = 1;

      if (szBuf[0] == ASC_CR && Term.fNewLine)
	{
	szBuf[1] = ASC_LF;
	i = 2;
	}

      CommDataOutput(szBuf, i);
      if (bTermEcho)
	TermInput(szBuf, i);
      break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
      MouseSelectMsg(hWnd, message, wParam, lParam);
      break;

    case WM_LBUTTONDBLCLK:
      y = HIWORD(lParam) / nCurFontHeight + nTermOffY;

      if (HIBYTE(*pPriVidLine[y]) & VID_TYPE_DWID)
	x = (LOWORD(lParam) + nTermOffX * nCurFontWidth) / (nCurFontWidth * 2);
      else
	x = LOWORD(lParam) / nCurFontWidth + nTermOffX;
      x++;

      VidClickAndPick(x, y);
      break;

    case WM_RBUTTONDOWN:
      y = HIWORD(lParam) / nCurFontHeight + nTermOffY;

      if (HIBYTE(*pPriVidLine[y]) & VID_TYPE_DWID)
	x = (LOWORD(lParam) + nTermOffX * nCurFontWidth) / (nCurFontWidth * 2);
      else
	x = LOWORD(lParam) / nCurFontWidth + nTermOffX;

      x++;
      CursorMovement(x, y);
      break;

    case WM_SETFOCUS:
      fTermFocusActive = TRUE;
      VidCreateCaret(hWnd);
      VidSetCaretPos();
      ShowCaret(hWnd);
      fTermCaretActive = TRUE;
      break;

    case WM_KILLFOCUS:
      HideCaret(hWnd);
      fTermCaretActive = FALSE;
      VidDeleteCaret(hWnd);
      // dca00035 MKL added cursor enable/disable capability
      bCaretDisabled = FALSE;
      fTermFocusActive = FALSE;
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      if (bTermRevr)
	hBrush = CreateSolidBrush(ChrAttrColor[Disp.nForeground]);
      else
	hBrush = CreateSolidBrush(ChrAttrColor[Disp.nBackground]);
      FillRect(ps.hdc, (LPRECT)&ps.rcPaint, hBrush);
      DeleteObject(hBrush);

      /*------------------------------------------------------------------------*/
      /*  Compute the client screen area in characters and set limits.		*/
      /*------------------------------------------------------------------------*/
      nX1 = ps.rcPaint.left / nCurFontWidth + nTermOffX + 1;
      nX2 = (ps.rcPaint.right - 1) / nCurFontWidth + nTermOffX + 1;
      if (nX1 < 1) nX1 = 1;
      if (nX1 > TERM_MAXCOL) nX1 = TERM_MAXCOL;
      if (nX2 < 1) nX2 = 1;
      if (nX2 > TERM_MAXCOL) nX2 = TERM_MAXCOL;

      nY1 = ps.rcPaint.top / nCurFontHeight + nTermOffY;
      nY2 = (ps.rcPaint.bottom - 1) / nCurFontHeight + nTermOffY;

      for (row = nY1; row <= nY2; row++)
	{
	if (!RefreshLine(row, nX1, nX2))
	  break;
	}

      if (fSelectedArea)
	VidSelectArea(nSelectX1, nSelectY1, nSelectX2, nSelectY2);

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      break;

    case UM_NEWFONT:
      VT102UpdateFonts();
      break;

    case WM_HSCROLL:
    case WM_VSCROLL:
      VidScroll(message, wParam, lParam);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}


static BOOL near RefreshLine(nRow, nX1, nX2)
  int  nRow;
  int  nX1, nX2;
{
  PINT	pLine;
  char	cCurAttr, cCurBuff[VID_MAXCOL];
  int	nCol, nInx, fBlk, nBlk;


  if ((pLine = VssGetLine(nRow)) == NULL)
    return NULL;

  if (*pLine & VID_TYPE_DWID)
    {
    nX1 = (nX1 + 1) / 2;
    nX2 = (nX2 + 1) / 2;
    }

  for (nCol = nX1, nInx = 0; nCol <= nX2; nCol++)
    {
    if (' ' == LOBYTE(pLine[nCol]))
      {
      if (' ' == LOBYTE(pLine[nCol+1]) && nCol+1 <= nX2)
	{
        if (' ' == LOBYTE(pLine[nCol+2]) && nCol+2 <= nX2)
	  fBlk = TRUE;
	else
	  fBlk = FALSE;
	}
      else
	fBlk = FALSE;
      }
    else
      fBlk = FALSE;

    if (cCurAttr == (char)(HIBYTE(pLine[nCol]) && !fBlk && nInx))
      {
      cCurBuff[nInx++] = LOBYTE(pLine[nCol]);
      }
    else
      {
      if (nInx)
	RefreshStr(nRow, nCol - nInx, cCurBuff, cCurAttr, nInx);

      nInx = 0;
      if ((pLine[nCol] & ~(TERM_ATTR_BOLD << 8)) != ((TERM_ATTR_WHITE << 12) | ' '))
	{
	cCurAttr = (char)(HIBYTE(pLine[nCol]));
	cCurBuff[nInx++] = LOBYTE(pLine[nCol]);
	}
      }
    }

  if (nInx)
    RefreshStr(nRow, nCol - nInx, cCurBuff, cCurAttr, nInx);

  return TRUE;
}


static void near RefreshStr(nRow, nCol, pText, nAttr, nLen)
  int  nRow, nCol;
  PSTR pText;
  int  nAttr;
  int  nLen;
{
  RECT Rect;
  int  nXPos, nYPos;
  int  nDelta[VID_MAXCOL+1];
  int  nType, nWidth;
  register i;

  nType = *GetVidLine(nRow);

  i = 0;
  if (nAttr & TERM_ATTR_BOLD)
    i += 1;
  if (nAttr & TERM_ATTR_UNDERLINE)
    i += 2;

  if (nType & (VID_TYPE_DTOP | VID_TYPE_DBOT))
    i += 12;
  else if (nType & VID_TYPE_DWID)
    i += 8;

  if (i < 8)
    nWidth = nCurFontWidth;
  else
    nWidth = nCurFontWidth * 2;

  if (i != cTermFontAttr)
    {
    cTermFontAttr = (char)i;
    SelectObject(hTerminalDC, hVT102Fonts[cTermFontAttr]);
    }


  nXPos = (nCol - nTermOffX - 1) * nWidth;
  if (i >= 8)
    nXPos = ((nCol-1) * 2 - nTermOffX) * nCurFontWidth;
  nYPos = (nRow - nTermOffY) * nCurFontHeight;


  if (((i = nAttr) & 0x77) != cTermFontColor)
    SetCurColor((char)i);


  for (i = 0; i < nLen; i++)
    nDelta[i] = nWidth;

  Rect.top    = nYPos;
  Rect.left   = nXPos;
  Rect.bottom = nYPos + nCurFontHeight;
  Rect.right  = nXPos + nWidth * nLen;

  if (nType & VID_TYPE_DBOT)
    nYPos -=nCurFontHeight;

  ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, pText, nLen, (LPINT)nDelta);
}


void near VT102CharOut(nChar)
  int  nChar;
{
  PINT	pLine;
  PINT	pChar;
  RECT	Rect;
  int	nXPos, nYPos, nWidth;
  int	i;


  if (bTermInsert)
    VidInsertChars(1);

  pLine = pPriVidLine[nTermChrY];
  pChar = pLine + nTermChrX;

  if ((*pChar == nChar) ||
      (LOBYTE(nChar) == ' ' && (*pChar | 0x0800) == (nChar | 0x0800)))
    return;

  *pChar = nChar;

  i = 0;
  if (HIBYTE(nChar) & TERM_ATTR_BOLD)
    i += 1;
  if (HIBYTE(nChar) & TERM_ATTR_UNDERLINE)
    i += 2;

  if (*pLine & (VID_TYPE_DTOP | VID_TYPE_DBOT))
    i += 12;
  else if (*pLine & VID_TYPE_DWID)
    i += 8;

  if (i < 8)
    nWidth = nCurFontWidth;
  else
    nWidth = nCurFontWidth * 2;


  if (i != cTermFontAttr)
    {
    cTermFontAttr = (char)i;
    SelectObject(hTerminalDC, hVT102Fonts[cTermFontAttr]);
    }


  if ((i = HIBYTE(nChar) & 0x77) != cTermFontColor)
    SetCurColor((char)i);


  nXPos = (nTermChrX - nTermOffX - 1) * nWidth;
  nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;

  if ((*pLine & 0x07) == 0)
    {
    if ((cTermFontAttr & 1) == 0)
      TextOut(hTerminalDC, nXPos, nYPos, (LPSTR)pChar, 1);
    else
      {
      Rect.top	  = nYPos;
      Rect.left   = nXPos;
      Rect.bottom = nYPos + nCurFontHeight;
      Rect.right  = nXPos + nCurFontWidth;
      ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, (LPSTR)pChar, 1, NULL);
      }
    }
  else
    {
    if (*pLine & VID_TYPE_DTOP)
      {
      Rect.top	  = nYPos;
      Rect.left   = nXPos;
      Rect.bottom = nYPos + nCurFontHeight;
      Rect.right  = nXPos + nCurFontWidth * 2;

      ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, (LPSTR)pChar, 1, NULL);
      }
    else if (*pLine & VID_TYPE_DBOT)
      {
      Rect.top	  = nYPos;
      Rect.left   = nXPos;
      Rect.bottom = nYPos + nCurFontHeight;
      Rect.right  = nXPos + nCurFontWidth * 2;

      nYPos -=nCurFontHeight;

      ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, (LPSTR)pChar, 1, NULL);
      }
    else if (*pLine & VID_TYPE_DWID)
      {
      Rect.top	  = nYPos;
      Rect.left   = nXPos;
      Rect.bottom = nYPos + nCurFontHeight;
      Rect.right  = nXPos + nCurFontWidth * 2;

      ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, (LPSTR)pChar, 1, NULL);
      }
    }
}

void near VT102CharDbl(nAttr)
  int nAttr;
{
  RECT	Rect;
  PSTR	pLine;


  *pPriVidLine[nTermChrY] &= 0xF8;

  if (nAttr == 3)
    *pPriVidLine[nTermChrY] |= VID_TYPE_DWID | VID_TYPE_DTOP;
  else if (nAttr == 4)
    *pPriVidLine[nTermChrY] |= VID_TYPE_DWID | VID_TYPE_DBOT;
  else if (nAttr == 6)
    *pPriVidLine[nTermChrY] |= VID_TYPE_DWID;


  GetClientRect(hChildTerminal, (LPRECT)&Rect);

  Rect.top    = (nTermChrY - nTermOffY) * nCurFontHeight;
  Rect.bottom = (nTermChrY - nTermOffY + 1) * nCurFontHeight;
  InvalidateRect(hChildTerminal, (LPRECT)&Rect, TRUE);
  UpdateWindow(hChildTerminal);
}


static void near SetCurColor(cColor)
  char cColor;
{
  DWORD dwForeground;
  DWORD dwBackground;


  cTermFontColor = cColor;

  switch ((cColor >> 4) & 0x07)
    {
    case TERM_ATTR_BLACK:
      dwForeground = ChrAttrColor[Disp.nBackground];
      break;

    case TERM_ATTR_RED:
      dwForeground = RGB(255,0,0);
      break;

    case TERM_ATTR_GREEN:
      dwForeground = RGB(0,255,0);
      break;

    case TERM_ATTR_YELLOW:
      dwForeground = RGB(255,255,0);
      break;

    case TERM_ATTR_BLUE:
      dwForeground = RGB(0,0,255);
      break;

    case TERM_ATTR_MAGENTA:
      dwForeground = RGB(255,0,255);
      break;

    case TERM_ATTR_CYAN:
      dwForeground = RGB(0,255,255);
      break;

    case TERM_ATTR_WHITE:
      dwForeground = ChrAttrColor[Disp.nForeground];
      break;
    }

  switch (cColor & 0x07)
    {
    case TERM_ATTR_BLACK:
      dwBackground = ChrAttrColor[Disp.nBackground];
      break;

    case TERM_ATTR_RED:
      dwBackground = RGB(255,0,0);
      break;

    case TERM_ATTR_GREEN:
      dwBackground = RGB(0,255,0);
      break;

    case TERM_ATTR_YELLOW:
      dwBackground = RGB(255,255,0);
      break;

    case TERM_ATTR_BLUE:
      dwBackground = RGB(0,0,255);
      break;

    case TERM_ATTR_MAGENTA:
      dwBackground = RGB(255,0,255);
      break;

    case TERM_ATTR_CYAN:
      dwBackground = RGB(0,255,255);
      break;

    case TERM_ATTR_WHITE:
      dwBackground = ChrAttrColor[Disp.nForeground];
      break;
    }

  if (dwBackground == RGB(0,0,0) && dwForeground == RGB(0,0,255))
    dwForeground = RGB(255,255,0);

  if (bTermRevr)
    {
    SetTextColor(hTerminalDC, dwBackground);
    SetBkColor(hTerminalDC, dwForeground);
    }
  else
    {
    SetTextColor(hTerminalDC, dwForeground);
    SetBkColor(hTerminalDC, dwBackground);
    }
}

static void near CursorMovement(x, y)
  int  x, y;
{
  char szBuf[8];
  register i;


  if (x == nTermChrX && y == nTermChrY)
    {
    fMouseCursor = FALSE;
    return;
    }

  if (Disp.fSlowCur)
    {
    fMouseCursor = TRUE;
    nCursorToX = x;
    nCursorToY = y;
    nCursorAtX = nTermChrX;
    nCursorAtY = nTermChrY;
    }

  if (x < 1) x = 1;
  if (x > nTermMaxCol) x = nTermMaxCol;
  if (y < 0) y = 0;
  if (y >= TERM_MAXROW) y = TERM_MAXROW - 1;

  i = 0;
  szBuf[i++] = ASC_ESC;
  if (!fTermVT52Mode)
    {
    if (bTermAltCur)
      szBuf[i++] = 'O';
    else
      szBuf[i++] = '[';
    }

  if (x >= nTermChrX)
    {
    if (y != nTermChrY)
      {
      if (y > nTermChrY)
	{
	y = y - nTermChrY;
        szBuf[i] = 'B';
	}
      else
	{
	y = nTermChrY - y;
        szBuf[i] = 'A';
	}

      if (Disp.fSlowCur)
	y = 1;
      while (y-- > 0)
	CommDataOutput(szBuf, i + 1);
      }

    if (x != nTermChrX)
      {
      if (x > nTermChrX)
	{
	x = x - nTermChrX;
        szBuf[i] = 'C';
	}
      else
	{
	x = nTermChrX - x;
        szBuf[i] = 'D';
	}

      if (Disp.fSlowCur)
	x = 1;
      while (x-- > 0)
	CommDataOutput(szBuf, i + 1);
      if (Disp.fSlowCur)
	return;
      }
    }
  else
    {
    if (x != nTermChrX)
      {
      if (x > nTermChrX)
	{
	x = x - nTermChrX;
        szBuf[i] = 'C';
	}
      else
	{
	x = nTermChrX - x;
        szBuf[i] = 'D';
	}

      if (Disp.fSlowCur)
	x = 1;
      while (x-- > 0)
	CommDataOutput(szBuf, i + 1);
      if (Disp.fSlowCur)
	return;
      }

    if (y != nTermChrY)
      {
      if (y > nTermChrY)
	{
	y = y - nTermChrY;
        szBuf[i] = 'B';
	}
      else
	{
	y = nTermChrY - y;
        szBuf[i] = 'A';
	}

      if (Disp.fSlowCur)
	y = 1;
      while (y-- > 0)
	CommDataOutput(szBuf, i + 1);
      }
    }
}
