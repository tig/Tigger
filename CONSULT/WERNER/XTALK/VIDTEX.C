/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communication Associates.  Inc.                 *
*                All Rights Reserved                                          *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   VIDTEX Terminal Emulation (VIDTEX.C)                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 12/25/89 00074 Change path to file directory.	      *
*             1.01 01/19/90 00090 Check for cTabWidth being equal to zero.    *
*             1.01 01/31/90 dca00009 JDB Fix cserv-b proto as per kents       *
*                           information today.                                *
*	      1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage  *
*			    routine to support VT102 screen alignment cap.    *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "capture.h"
#include "comm.h"
#include "disp.h"
#include "font.h"
#include "interp.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "properf.h"
#include "variable.h"
#include "xfer.h"
#include "dialogs.h"

#define STATE_TERMINAL	0
#define TERM_COL80  80

extern void InitCisGH(char *, int, char);

static void near TermReset();
static void near TermInput(char *, int);
static BOOL near RefreshLine(int, int, int);
static void near RefreshStr(int, int, PSTR, int, int);
static int  near TermVidtexEscape(char *, int);
static void near TermLinePrint(int);

static int  near VidtexDle(char *, int);


void VidtexInit()
{
  char szBuf[16];

  nTermChrX   = 0;
  nTermChrY   = 0;

  nSaveChrX   = 0;
  nSaveChrY   = 0;
  nSaveAttr   = 0;

  nTermMaxCol = 80;

  nTermOffX   = 0;
  nTermOffY   = 0;

  LoadString(hWndInst, IDS_EMULATOR_VIDTEX, szBuf, sizeof(szBuf));
  SendMessage(hChildStatus, UM_EMULATOR, (WORD)szBuf, 0L);

  SelectObject(hTerminalDC, hCurFont);

  TermReset();
}


void VidtexTerm()
{
}


long WndVidtexProc(hWnd, message, wParam, lParam )
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
      TermReset();
      break;

    case UM_DATA:
      TermInput((PSTR)LOWORD(lParam), wParam);
      break;

    case WM_CHAR:
      if (IfSystemStatus(SS_SCRIPT))
	{
        if (InterpreterKeyboard(wParam, 1))
          break;
	}
      if (TermNameScan(wParam))
	break;
      szBuf[0] = (char)wParam;
      i = 1;

      CommDataOutput(szBuf, i);
      if (Term.fEcho)
	TermInput(szBuf, i);
      break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
      MouseSelectMsg(hWnd, message, wParam, lParam);
      break;

    case WM_LBUTTONDBLCLK:
      y = HIWORD(lParam) / nCurFontHeight + nTermOffY;
      x = LOWORD(lParam) / nCurFontWidth + nTermOffX + 1;
      VidClickAndPick(x, y);
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
      fTermFocusActive = FALSE;
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      hBrush = CreateSolidBrush(ChrAttrColor[Disp.nBackground]);
      FillRect(ps.hdc, &ps.rcPaint, hBrush);
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

    if (cCurAttr == (char)HIBYTE(pLine[nCol]) && !fBlk && nInx)
      {
      cCurBuff[nInx++] = LOBYTE(pLine[nCol]);
      }
    else
      {
      if (nInx)
	RefreshStr(nRow, nCol - nInx, cCurBuff, cCurAttr, nInx);

      nInx = 0;
      if (pLine[nCol] != 0x7020)
	{
        cCurAttr = (char)HIBYTE(pLine[nCol]);
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
  int  nXPos, nYPos;


  nXPos = (nCol - nTermOffX - 1) * nCurFontWidth;
  nYPos = (nRow - nTermOffY) * nCurFontHeight;

  TextOut(hTerminalDC, nXPos, nYPos, (LPSTR)pText, nLen);
}


void near VidtexCharOut(nChar)
  int  nChar;
{
  PINT	pLine;
  PINT	pChar;
  RECT	Rect;
  int   nXPos, nYPos;


  pLine = pPriVidLine[nTermChrY];
  pChar = pLine + nTermChrX;

  if ((WORD)nChar == LOBYTE(*pChar))
    return;

  *pChar = (0x70 << 8) | (nChar & 0x00FF);

  nXPos = (nTermChrX - nTermOffX - 1) * nCurFontWidth;
  nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;

  TextOut(hTerminalDC, nXPos, nYPos, (LPSTR)pChar, 1);
}


static void near TermInput(szBuf, nChrCnt)
  char *szBuf;
  int  nChrCnt;
{
  char buf[4];
  int  i;


  if (nChrCnt <= 0)
    return;

  if (fTermCaretActive)
    {
    HideCaret(hChildTerminal);
    fTermCaretActive = FALSE;
    }

  while (nChrCnt > 0)
    {
    if (terminal.vidtex.dle)
      {
      i = VidtexDle(szBuf, nChrCnt);

      szBuf   += i;
      nChrCnt -= i;
      continue;
      }

    if (bTermEscape)
      {
      i = TermVidtexEscape(szBuf, nChrCnt);

      szBuf   += i;
      nChrCnt -= i;
      continue;
      }

    switch (*szBuf & 0x7F)
      {
      case ASC_NUL:
	break;

      case ASC_BEL:
	MessageBeep(NULL);
	break;

      case ASC_ENQ:
        buf[0] = ASC_DLE;
        buf[1] = '+';
        buf[2] = ASC_DLE;
        buf[3] = '0';
        CommDataOutput(buf, 4);

	break;

      case ASC_DLE:
        terminal.vidtex.dle   = TRUE;
        filexfer.qb.iData = 0;
        filexfer.qb.esc   = FALSE;
        filexfer.qb.end   = 0;
        filexfer.qb.cm    = 0; //JDB dca00009 fix
	break;

      case ASC_SI:
      case ASC_SO:
	break;

      case ASC_BS:
	if (nTermChrX > 1)
	  {
	  nTermChrX--;
	  }
	else if (nTermChrY > 0)
	  {
	  nTermChrY--;
	  nTermChrX = TERM_COL80;
	  }
        VidtexCharOut(' ');

	if (nCaptureInx)
	  nCaptureInx--;
	break;

      case ASC_HT:
        // 00090 Check for zero TabWidth.
        if (Term.cTabWidth)
          {
          nTermChrX = ((nTermChrX / Term.cTabWidth) + 1) * Term.cTabWidth;
          if (nTermChrX > TERM_COL80)
            nTermChrX = TERM_COL80;
          }
	break;

      case ASC_LF:
	if (nTermChrY == TERM_MAXROW-1)
	  VidScrollUp(0, TERM_MAXROW-1);
	else if (nTermChrY < TERM_MAXROW - 1)
	  nTermChrY++;

	if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
          {
	  FlushCaptureBuf();
          }

	if (bPrinterOn || bTermAutoPrint)
	  PrintLine(NULL);
	break;

      case ASC_FF:
	nTermChrX = 1;
	nTermChrY = 1;
	// dca00034 MKL added one parameter to VidClearPage  *
	VidClearPage(0x7020);
	break;

      case ASC_CR:
	nTermChrX = 1;
	if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
          WriteCharToCapture(' ');
	break;

      case ASC_ESC:
	bTermEscape = TRUE;
	nTermInx    = 0;
	if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	  FlushCaptureBuf();
	break;

      default:
        if ((*szBuf & 0x7F) < ' ')
	  break;

	if ((nTermChrX > TERM_COL80) ||
            (Term.fWindowWrap && nTermChrX > nTermExtX))
	  {
	  nTermChrX = 1;
	  if (nTermChrY == TERM_MAXROW-1)
	    {
	    VidScrollUp(0, TERM_MAXROW-1);
	    }
	  else if (nTermChrY < TERM_MAXROW - 2)
	    nTermChrY++;

	  if (bPrinterOn || bTermAutoPrint)
	    PrintLine(NULL);
	  }

        VidtexCharOut(*szBuf & 0x7F);

	if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
          WriteCharToCapture(*szBuf & 0x7F);

	if (bPrinterOn || bTermAutoPrint)
          PrintChar(*szBuf & 0x7F, nTermChrX - 1);

	nTermChrX++;
	break;
      }

      szBuf++;
      nChrCnt--;
    }

  VidSeeMe(nTermChrY);

  if (fTermFocusActive)
    {
    VidSetCaretPos();
    ShowCaret(hChildTerminal);
    fTermCaretActive = TRUE;
    }
}


BOOL EmulatorVidtexKey(msg)
  LPMSG msg;
{
  char szBuf[64];
  char fLocal;
  int  i;

  szBuf[0] = ASC_ESC;
  fLocal   = FALSE;
  i = 0;

  switch (msg->wParam)
    {
    case VK_BACK:
      i = 1;
      if (Term.fSwapDel)
	{
        if (GetKeyState(VK_CONTROL) >= 0)
	  szBuf[0] = ASC_DEL;
	else
	  szBuf[0] = ASC_BS;
	}
      else
	{
        if (GetKeyState(VK_CONTROL) < 0)
	  szBuf[0] = ASC_DEL;
	else
	  szBuf[0] = ASC_BS;
        }

      TermNameScan(ASC_BS);
      break;

    case VK_UP:
      szBuf[1] = 'A';
      i = 2;
      break;

    case VK_DOWN:
      szBuf[1] = 'B';
      i = 2;
      break;

    case VK_LEFT:
      szBuf[1] = 'D';
      i = 2;
      break;

    case VK_RIGHT:
      szBuf[1] = 'C';
      i = 2;
      break;

    case VK_HOME:
      szBuf[1] = 'H';
      i = 2;
      break;

    case VK_END:
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'K';
	i = 2;

	fLocal = TRUE;
	}
      break;

    case VK_NEXT:
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'H';
	szBuf[2] = ASC_ESC;
        szBuf[3] = 'J';
	i = 4;

	fLocal = TRUE;
	}
      break;

    case VK_RETURN:
      if (GetKeyState(VK_CONTROL) < 0)
	{
	szBuf[0] = ASC_LF;
	i = 1;
	}
      break;

    default:
      return FALSE;
    }

  if (i == 0)
    return FALSE;

  if (fLocal)
    {
    TermInput(szBuf, i);
    return TRUE;
    }

  szBuf[i] = NULL;

  CommDataOutput(szBuf, i);
  if (Term.fEcho)
    TermInput(szBuf, i);

  return TRUE;
}

static int near TermVidtexEscape(szBuf, nChrCnt)
  char *szBuf;
  int  nChrCnt;
{
  char buf[64];
  int  nCount;
  int  i, x, y;
  int  ch;

  nCount = 0;
  while (nChrCnt-- > 0 && bTermEscape)
    {
    nCount++;
    cTermBuf[nTermInx++] = *szBuf++ & 0x7F;

    bTermEscape = FALSE;
    switch (cTermBuf[0])
      {
      case 'A':
	if (nTermChrY != 0)
	  nTermChrY--;
	break;

      case 'B':
	if (nTermChrY != TERM_MAXROW-1)
	  nTermChrY++;
	break;

      case 'C':
	if (nTermChrX < nTermMaxCol)
	  nTermChrX++;
	break;

      case 'D':
	if (nTermChrX > 1)
	  nTermChrX--;
	break;

      case 'H':
	nTermChrX = 1;
	nTermChrY = 0;
	break;

      case 'J':
	// dca00034 MKL added one parameter to VidClearPage  *
	if (nTermChrX == 1 && nTermChrY == 0)
	  VidClearPage(0x7020);
	else
	  VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL - 1, TERM_MAXROW - 1);
	break;

      case 'K':
	VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL-1, nTermChrY);
	break;

      case 'G':
	if (nTermInx > 1)
	  {
          if (cTermBuf[1] == 'H')
	    {
	    InitCisGH(szBuf, nChrCnt, TRUE);
	    nCount += nChrCnt;
	    }
          else if (cTermBuf[1] == 'M')
	    {
	    InitCisGH(szBuf, nChrCnt, FALSE);
	    nCount += nChrCnt;
	    }
	  }
	else
	  bTermEscape = TRUE;
	break;

      case 'j':
	nTermChrX = 1;
	nTermChrY = 0;
	// dca00034 MKL added one parameter to VidClearPage  *
	VidClearPage(0x7020);
	break;

      case 'k':
        bTermEscape = TRUE;
	break;

      case 'Y':
	if (nTermInx > 2)
	  {
	  nTermChrY = cTermBuf[1] - 32;
	  nTermChrX = cTermBuf[2] - 31;

	  if (nTermChrX < 1)
	    nTermChrX = 1;
	  else if (nTermChrX > TERM_MAXCOL)
	    nTermChrX = TERM_MAXCOL;

	  if (nTermChrY < 0)
	    nTermChrY = 0;
	  else if (nTermChrY >= TERM_MAXROW)
	    nTermChrY = TERM_MAXROW - 1;
	  }
	else
	  bTermEscape = TRUE;
	break;

      case 'I':
      case ':':
        sprintf(buf, "#IB1,CC,GH,GM,SS7o,+1243\r");
	CommDataOutput(buf, strlen(buf));
	break;

      default:
	break;
      }
    }

  return (nCount);
}

static int near VidtexDle(szBuf, nChrCnt)
  char *szBuf;
  int  nChrCnt;
{
  int  nCount;


  if (filexfer.qb.iData == 0)
    {
    if (*szBuf == 'B')
      {
      }
    else if (*szBuf >= '0' && *szBuf <= '9')
      {
      terminal.vidtex.dle = FALSE;
      return (1);
      }
    else
      {
      terminal.vidtex.dle = FALSE;
      return (0);
      }
    }

  nCount = 0;
  while (nChrCnt-- > 0 && terminal.vidtex.dle)
    {
    nCount++;

    if (*szBuf == ASC_DLE)
      {
      filexfer.qb.esc = TRUE;
      }
    else
      {
      if (filexfer.qb.iData < sizeof(filexfer.qb.sData))
        {
        if (filexfer.qb.esc)
          {
          filexfer.qb.sData[filexfer.qb.iData++] = *szBuf & 0x1F;
          filexfer.qb.esc = FALSE;
          }
        else
          filexfer.qb.sData[filexfer.qb.iData++] = *szBuf;
        }
      }

    if (filexfer.qb.end != 0)
      {
      if (--filexfer.qb.end == 0)
        terminal.vidtex.dle = FALSE;
      }
    else
      {
      if (*szBuf == 0x03)
        {
        filexfer.qb.end = filexfer.qb.cm + 1;
        }
      }

    szBuf++;
    }

  if (terminal.vidtex.dle == FALSE)
    {
    register i;

    // 00074 Change path to file directory.
    if (i = SetFullPath(VAR_DIRFIL))
      GeneralError(i);
    else
      XferQuickBStart(-1);
    }

  return (nCount);
}

static void near TermReset()
{
  int i;


  terminal.vidtex.dle = FALSE;

  nTermChrX = 1;
  nTermChrY = 0;
  bTermEscape = FALSE;

  SetTextColor(hTerminalDC, ChrAttrColor[Disp.nForeground]);
  SetBkColor(hTerminalDC, ChrAttrColor[Disp.nBackground]);

  nTermMaxCol = TERM_COL80;

  // dca00034 MKL added one parameter to VidClearPage  *
  VidClearPage(0x7020);
  VidSetCaretPos();
}
