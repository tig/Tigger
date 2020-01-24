/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   VT52 Terminal Emulation (VT52.C)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*  1.01 08/03/89 00005 Fix updating problem with terminal setup dialog,	   *
*						Disable graphics for 3101, activate graphics for VT52. *
*  1.01 01/19/90 00090 Check for cTabWidth being equal to zero.			   *
*  1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage			   *
*						routine to support VT102 screen alignment cap.		   *
*  1.1  04/01/90 dca00064 PJL Automatically set VT102 font.				   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "font.h"
#include "properf.h"
#include "capture.h"
#include "comm.h"
#include "disp.h"
#include "interp.h"
#include "sess.h"
#include "prodata.h"
#include "dialogs.h"

#define TERM_COL80  80

static BOOL near RefreshLine(int, int, int);
static void near RefreshStr(int, int, PSTR, int, int);
static void near SetCurColor(char);
static int  near TermVT52Escape(char *, int);
static int  near TermCharXlat(WORD);
static void near TermLinePrint(int);
static void near TermScreenPrint();
static void near TermReset();
static void near TermInput(char *, int);

static int  nTermCntlXPos;



void VT52Init()
{
	char szBuf[16];

	nTermChrX   = 1;
	nTermChrY   = 0;

	nSaveChrX   = 1;
	nSaveChrY   = 0;

	nTermOffX   = 0;
	nTermOffY   = 0;

	nTermMaxCol = TERM_COL80;

	// dca00064 PJL Automatically set VT102 font.
	ResetFont(fVT102Font);

	if (!(wSystem & SS_INIT))
		SendMessage(hChildTerminal, UM_RESET, 0, 0L);

	LoadString(hWndInst, IDS_EMULATOR_VT52, szBuf, sizeof(szBuf));
	SendMessage(hChildStatus, UM_EMULATOR, (WORD)szBuf, 0L);
}


void VT52Term()
{
}


long WndVT52Proc(hWnd, message, wParam, lParam )
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

      if (szBuf[0] == ASC_CR && Term.fNewLine)
	{
	szBuf[1] = ASC_LF;
	i = 2;
	}

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

    case WM_RBUTTONDOWN:
      y = HIWORD(lParam) / nCurFontHeight + nTermOffY;
      x = LOWORD(lParam) / nCurFontWidth + nTermOffX;

      x++;
      if (x < 1) x = 1;
      if (x > TERM_COL80) x = TERM_COL80;
      if (y < 0) y = 0;
      if (y >= TERM_MAXROW - 1) y = TERM_MAXROW - 2;

      i = 0;
      szBuf[i++] = ASC_ESC;

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

	  while (x-- > 0)
	    CommDataOutput(szBuf, i + 1);
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

	  while (x-- > 0)
	    CommDataOutput(szBuf, i + 1);
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

	  while (y-- > 0)
	    CommDataOutput(szBuf, i + 1);
	  }
	}
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
      if (nX1 > TERM_COL80) nX1 = TERM_COL80;
      if (nX2 < 1) nX2 = 1;
      if (nX2 > TERM_COL80) nX2 = TERM_COL80;

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

    if (!fBlk && nInx)
      {
      cCurBuff[nInx++] = LOBYTE(pLine[nCol]);
      }
    else
      {
      if (nInx)
	RefreshStr(nRow, nCol - nInx, cCurBuff, cCurAttr, nInx);

      nInx = 0;
      if (' ' != LOBYTE(pLine[nCol]))
	{
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

  TextOut(hTerminalDC, nXPos, nYPos, pText, nLen);
}


void near VT52CharOut(nChar)
  int  nChar;
{
  PINT	pLine;
  PINT	pChar;
  RECT	Rect;
  int	nXPos, nYPos, nWidth;
  int	i;


  pLine = pPriVidLine[nTermChrY];
  pChar = pLine + nTermChrX;

  if (*pChar == nChar)
    return;

  *pChar = nChar;

  nXPos = (nTermChrX - nTermOffX - 1) * nCurFontWidth;
  nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;

  TextOut(hTerminalDC, nXPos, nYPos, (LPSTR)pChar, 1);
}

static void near TermInput(szBuf, nChrCnt)
  char *szBuf;
  int  nChrCnt;
{
  PSTR pAdr;
  int  nBuf[1];
  int  i;

  if (nChrCnt <= 0)
    return;

  for (i = 0; i < nChrCnt; i++)
    *(szBuf + i) &= 0x7F;

  if (fTermCaretActive)
    {
    HideCaret(hChildTerminal);
    fTermCaretActive = FALSE;
    }

  while (nChrCnt > 0)
    {
    if (bTermEscape)
      {
      i = TermVT52Escape(szBuf, nChrCnt);

      szBuf   += i;
      nChrCnt -= i;
      continue;
      }


    if (bTermCntlPrint)
      {
      switch (*szBuf)
	{
	case ASC_CR:
	  nTermCntlXPos = 0;
	  break;

	case ASC_LF:
	  PrintLine(NULL);
	  nTermCntlXPos = 0;
	  break;

	case ASC_ESC:
	  bTermEscape = TRUE;
	  nTermInx  = 0;
	  break;

	default:
	  PrintChar(*szBuf, nTermCntlXPos++);
	  break;
	}
      }
    else
      {
      switch (*szBuf)
	{
	case ASC_NUL:
	  break;

	case ASC_ENQ:
	  if (Term.szAnswerback[0] != NULL)
	    CommDataOutput(&Term.szAnswerback[1], (int)Term.szAnswerback[0]);
	  break;

	case ASC_BEL:
	  MessageBeep(NULL);
	  break;

	case ASC_BS:
	  if (nTermChrX > 1)
	    nTermChrX--;

	  if (nCaptureInx)
	    nCaptureInx--;
	  break;

	case ASC_HT:
          // 00090 Check for zero TabWidth.
          if (Term.cTabWidth)
            {
            for (i = nTermChrX + 1; i <= TERM_COL80; i++)
              if (bTermTabs[i-1] != FALSE)
                break;

            if (i > TERM_COL80)
              nTermChrX = TERM_COL80;
            else
              nTermChrX = i;
            }
	  break;

	case ASC_CR:
	  nTermChrX = 1;

          if (!Term.fLfAuto)
	    break;
	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
            WriteCharToCapture(' ');

	case ASC_LF:
	case ASC_VT:
	case ASC_FF:
	  if (nTermChrY == nTermMarginBot)
	    VidScrollUp(nTermMarginTop, nTermMarginBot);
	  else if (nTermChrY < TERM_MAXROW - 2)
	    nTermChrY++;

          if (Term.fLfAuto)
	    nTermChrX = 1;

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    FlushCaptureBuf();

	  if (bPrinterOn || bTermAutoPrint)
	    PrintLine(NULL);
	  break;

	case ASC_SO:
	  nTermCSet = 1;
	  break;

	case ASC_SI:
	  nTermCSet = 0;
	  break;

	case ASC_CAN:
	case ASC_SUB:
	  break;

	case ASC_ESC:
	  bTermEscape = TRUE;
	  nTermInx  = 0;
	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    FlushCaptureBuf();
	  break;

	case ASC_DEL:
	  if (nTermChrX > 1)
	    nTermChrX--;
          VT52CharOut(' ');
	  if (bPrinterOn || bTermAutoPrint)
            PrintChar(' ', nTermChrX-1);
	  if (nCaptureInx)
	    nCaptureInx--;
	  break;

	default:
          if (*szBuf < ' ')
	    break;

	  if ((nTermChrX > TERM_COL80) ||
	      (Term.fWindowWrap && nTermChrX > nTermExtX))
	    {
	    if (bTermWrap)
	      {
	      nTermChrX = 1;
	      if (nTermChrY == nTermMarginBot)
		{
		VidScrollUp(nTermMarginTop, nTermMarginBot);
		}
	      else if (nTermChrY < TERM_MAXROW - 2)
		nTermChrY++;

	      if (bPrinterOn || bTermAutoPrint)
		PrintLine(NULL);
	      }
	    else
	      nTermChrX = TERM_COL80;
	    }

	  VT52CharOut(TermCharXlat(*szBuf));

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    WriteCharToCapture(*szBuf);

	  if (bPrinterOn || bTermAutoPrint)
	    PrintChar(*szBuf, nTermChrX - 1);

	  nTermChrX++;
	  break;
	}
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


BOOL EmulatorVT52Key(msg)
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
    case VK_F1:
      szBuf[1] = 'P';
      i = 2;
      break;

    case VK_F2:
      szBuf[1] = 'Q';
      i = 2;
      break;

    case VK_F3:
      szBuf[1] = 'R';
      i = 2;
      break;

    case VK_F4:
      szBuf[1] = 'S';
      i = 2;
      break;

    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
      if (bTermAltKey)
	{
        szBuf[1] = '?';
	szBuf[2] = (char)('p' + msg->wParam - VK_NUMPAD0);
	i = 3;
	}
      break;

    case VK_DECIMAL:
      if (GetKeyState(VK_NUMLOCK) & 0x01 != 0)
	{
	if (bTermAltKey)
	  {
          szBuf[1] = '?';
          szBuf[2] = 'n';
	  i = 3;
	  }
	else
	  {
          szBuf[0] = '.';
	  i = 1;
	  }
	}
      else
	{
	szBuf[1] = ASC_DEL;
	i = 1;
	}
      break;

    case VK_ADD:
      if (GetKeyState(VK_NUMLOCK) & 0x01 != 0)
	{
	if (bTermAltKey)
	  {
          szBuf[1] = '?';
          szBuf[2] = 'M';
	  i = 3;
	  }
	else
	  {
	  szBuf[0] = ASC_CR;
	  i = 1;
	  }
	}
      break;

    case VK_SUBTRACT:
      if (GetKeyState(VK_NUMLOCK) & 0x01 != 0)
	{
	if (bTermAltKey)
	  {
          szBuf[1] = '?';
          szBuf[2] = 'm';
	  i = 3;
	  }
	else
	  {
          szBuf[0] = '-';
	  i = 1;
	  }
	}
      break;

    case VK_MULTIPLY:
      if (GetKeyState(VK_NUMLOCK) & 0x01 != 0)
	{
	if (bTermAltKey)
	  {
          szBuf[1] = '?';
          szBuf[2] = 'l';
	  i = 3;
	  }
	else
	  {
          szBuf[0] = ',';
	  i = 1;
	  }
	}
      break;

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

static int near TermVT52Escape(szBuf, nChrCnt)
  char *szBuf;
  int  nChrCnt;
{
  int  nCount;
  int  i, x, y;
  int  ch;

  nCount = 0;
  while (nChrCnt-- > 0 && bTermEscape)
    {
    nCount++;
    cTermBuf[nTermInx++] = *szBuf++;

    bTermEscape = FALSE;
    switch (cTermBuf[0])
      {
      case '<':
	fTermVT52Mode = FALSE;
	SendMessage(hChildStatus, UM_EMULATOR, 0, 0L);
	break;

      case 'A':
	if (nTermChrY != nTermMarginTop)
	  nTermChrY--;
	break;

      case 'B':
	if (nTermChrY != nTermMarginBot)
	  nTermChrY++;
	break;

      case 'C':
	if (nTermChrX < TERM_COL80)
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

      case 'Y':
	if (nTermInx > 2)
	  {
	  nTermChrY = cTermBuf[1] - 32;
	  nTermChrX = cTermBuf[2] - 31;

	  if (nTermChrX < 1)
	    nTermChrX = 1;
	  else if (nTermChrX > TERM_COL80)
	    nTermChrX = TERM_COL80;

	  if (nTermChrY < 0)
	    nTermChrY = 0;
	  else if (nTermChrY >= TERM_MAXROW)
	    nTermChrY = TERM_MAXROW - 1;

	  }
	else
	  bTermEscape = TRUE;
	break;

      case 'I':
	if (nTermChrY == nTermMarginTop)
	  VidScrollDown(nTermMarginTop, nTermMarginBot);
	else
	  nTermChrY--;
	break;

      case '=':
	bTermAltKey = TRUE;
	SendMessage(hChildStatus, UM_KEYPAD, 1, (long)TERM_COL80);
	break;

      case '>':
	bTermAltKey = FALSE;
	bTermAltCur = FALSE;
	SendMessage(hChildStatus, UM_KEYPAD, 0, (long)TERM_COL80);
	break;

      case 'F':
	// 00005 Added check for graphic option disable.
	if (Sess.fGraphics)
	  fTermGraphicsOn = TRUE;
	break;

      case 'G':
	fTermGraphicsOn = FALSE;
	break;

      case 'K':
	VidClearArea(nTermChrX, nTermChrY, TERM_COL80-1, nTermChrY);
	break;

      case 'J':
	// dca00034 MKL added one parameter to VidClearPage
	if (nTermChrX == 1 && nTermChrY == 0)
	  VidClearPage(0x7020);
	else
	  VidClearArea(nTermChrX, nTermChrY, TERM_COL80 - 1, TERM_MAXROW - 1);
	break;

      case '^':
	bTermAutoPrint = TRUE;
	SendMessage(hChildInfo, UM_SCRNMODE, INFO_AUTO, 1L);
	OpenPrinter();
	break;

      case '_':
	bTermAutoPrint = FALSE;
	SendMessage(hChildInfo, UM_SCRNMODE, INFO_AUTO, 0L);
	ClosePrinter();
	break;

      case 'W':
	bTermCntlPrint = TRUE;
	nTermCntlXPos  = 0;
	SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 1L);
	OpenPrinter();
	break;

      case 'X':
	bTermCntlPrint = FALSE;
	SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 0L);
	ClosePrinter();
	break;

      case 'V':
	TermLinePrint(nTermChrY);
	break;

      case ']':
	TermScreenPrint();
	break;

      case 'Z':
	cTermBuf[0] = ASC_ESC;
        cTermBuf[1] = '/';
        cTermBuf[2] = 'Z';
	CommDataOutput(cTermBuf, 3);
	break;
      }
    }

  return (nCount);
}


static int near TermCharXlat(ch)
  WORD ch;
{
  ch &= 0xFF;

  switch (nTermCS[nTermCSet])
    {
    case TERM_CS_ASCII:
      break;

    case TERM_CS_DECGPH:
      if (fVT102Font && bCurFontCharSet != OEM_CHARSET)
	{
	if (ch > 95)
	  ch += 96;
	}
      else
	{
	switch (ch)
	  {
	  case 0x5F:
	    ch = 32;
	    break;

	  case 0x60:
	    ch = 4;
	    break;

	  case 0x61:
	    ch = 177;
	    break;

	  case 0x62:
	    ch = 26;
	    break;

	  case 0x63:
	    ch = 23;
	    break;

	  case 0x64:
	    ch = 27;
	    break;

	  case 0x65:
	    ch = 25;
	    break;

	  case 0x66:
	    ch = 248;
	    break;

	  case 0x67:
	    ch = 241;
	    break;

	  case 0x68:
	    ch = 21;
	    break;

	  case 0x69:
	    ch = 18;
	    break;

	  case 0x6A:
	    ch = 217;
	    break;

	  case 0x6B:
	    ch = 191;
	    break;

	  case 0x6C:
	    ch = 218;
	    break;

	  case 0x6D:
	    ch = 192;
	    break;

	  case 0x6E:
	    ch = 197;
	    break;

	  case 0x6F:
	  case 0x70:
	  case 0x71:
	  case 0x72:
	  case 0x73:
	    ch = 196;
	    break;

	  case 0x74:
	    ch = 195;
	    break;

	  case 0x75:
	    ch = 180;
	    break;

	  case 0x76:
	    ch = 193;
	    break;

	  case 0x77:
	    ch = 194;
	    break;

	  case 0x78:
	    ch = 179;
	    break;

	  case 0x79:
	    ch = 243;
	    break;

	  case 0x7A:
	    ch = 242;
	    break;

	  case 0x7B:
	    ch = 227;
	    break;

	  case 0x7C:
	    ch = 157;
	    break;

	  case 0x7D:
	    ch = 156;
	    break;

	  case 0x7E:
	    ch = 250;
	    break;
	  }
	}
      break;

    case TERM_CS_BRITISH:
      break;
    }

  return (ch + cTermAttr * 256);
}


static void near TermReset()
{
  int i;


  nTermChrX = 1;
  nTermChrY = 0;
  bTermEscape = FALSE;

  bTermRevr   = FALSE;

  if (Term.fAutoWrap)
    bTermWrap = TRUE;
  else
    bTermWrap = FALSE;

  bSaveWrap = bTermWrap;

  bTermOrigin = FALSE;
  bSaveOrigin = FALSE;


  nTermCSet = 0;
  nSaveCSet = 0;
  memset(nTermCS, 0, sizeof(nTermCS));
  nTermCS[1] = TERM_CS_DECGPH;

  SetTextColor(hTerminalDC, ChrAttrColor[Disp.nForeground]);
  SetBkColor(hTerminalDC, ChrAttrColor[Disp.nBackground]);

  nTermMarginTop = 0;
  nTermMarginBot = TERM_MAXROW - 2;

  memset(bTermTabs, FALSE, sizeof(bTermTabs));

  // 00090 Check for zero TabWidth.
  if (Term.cTabWidth)
    {
    for (i = Term.cTabWidth; i < TERM_COL80; i += Term.cTabWidth)
      bTermTabs[i] = TRUE;
    }

  bTermInsert	 = FALSE;
  bTermAltKey	 = FALSE;
  bTermAltCur	 = FALSE;
  bTermLckKey	 = FALSE;
  SendMessage(hChildStatus, UM_KEYPAD, 0, (long)TERM_COL80);
  cTermLEDs	 = 0;
  SendMessage(hChildStatus, UM_LIGHTS, 0, 0L);

  fTermGraphicsOn = FALSE;
  SendMessage(hChildStatus, UM_EMULATOR, 0, 0L);

  if (bTermAutoPrint)
    ClosePrinter();
  if (bTermCntlPrint)
    ClosePrinter();

  bTermAutoPrint = FALSE;
  bTermCntlPrint = FALSE;
  bTermModePrint = FALSE;
  bTermFeedPrint = FALSE;

  // dca00034 MKL added one parameter to VidClearPage
  VidClearPage(0x7020);
  VidSetCaretPos();
}

static void near TermLinePrint(nLine)
  int  nLine;
{
  PINT	pInt;
  char	buf[133];
  int	i;


  pInt =  pPriVidLine[nLine] + 1;

  for (i = 0; i < TERM_COL80; i++)
    buf[i] = (char)(*pInt++);
  buf[TERM_COL80] = NULL;

  PrintLine(buf);
}

static void near TermScreenPrint()
{
  int  nTop, nBot;
  int  i;


  if (bTermModePrint)
    {
    nTop = nTermMarginTop;
    nBot = nTermMarginBot;
    }
  else
    {
    nTop = 0;
    nBot = TERM_MAXROW - 2;
    }

  for (i = nTop; i <= nBot; i++)
    TermLinePrint(i);

  if (bTermFeedPrint)
    FlushPrinter();
}
