/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   IBM 3101 Terminal Emulator (IBM3101.C)                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 11/07/89 00059 Enhance IBM 3101 with ESC X and ESC Z.      *
*	      1.01 11/23/89 00062 Change emulation to scroll at end.	      *
*	      1.1  03/01/90 dca00032 MKL fixed keyboard lock unlock problem   *
*	      1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage  *
*			    routine to support VT102 screen alignment cap.    *
*	      1.1  04/09/90 dca00078 MKL fixed "print at" problem	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "properf.h"
#include "capture.h"
#include "comm.h"
#include "disp.h"
#include "interp.h"
#include "sess.h"
#include "prodata.h"
#include "dialogs.h"

#define TERM_COL    80
#define TERM_ROW    24

static BOOL near RefreshLine(int, int, int);
static void near RefreshStr(int, int, PSTR, int, int);
static void near SetCurColor(char);
static int  near TermIBM3101Escape(char *, int);
static int  near TermCharXlat(WORD);
static void near TermLinePrint(int);
static void near TermScreenPrint();
static void near TermInput(char *, int);

static char f3101MaskOut;

// 00059 Enhance IBM 3101 with ESC X and ESC Z
static short sPosX, sPosY;
static short fMoveCursor;


void IBM3101Init()
{
  char buf[16];


  nTermChrX   = 1;
  nTermChrY   = 0;

  // 00059 Enhance IBM 3101 with ESC X and ESC Z
  sPosX = nTermChrX;
  sPosY = nTermChrY;
  fMoveCursor = TRUE;

  nTermOffX   = 0;
  nTermOffY   = 0;

  nTermMaxCol = TERM_COL;

  f3101MaskOut = FALSE;

  cTermAttr = 0x70;
  nSaveAttr = cTermAttr;
  cTermFontAttr = 0;
  cTermFontColor = 0x70;
  SetTextColor(hTerminalDC, ChrAttrColor[Disp.nForeground]);
  SetBkColor(hTerminalDC, ChrAttrColor[Disp.nBackground]);

  SendMessage(hChildTerminal, UM_RESET, 0, 0L);

  LoadString(hWndInst, IDS_EMULATOR_3101, buf, sizeof(buf));
  SendMessage(hChildStatus, UM_EMULATOR, (WORD)buf, 0L);
}


void IBM3101Term()
{
}


long WndIBM3101Proc(hWnd, message, wParam, lParam )
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
      nTermChrX = 1;
      nTermChrY = 0;
      bTermEscape = FALSE;

      // 00059 Enhance IBM 3101 with ESC X and ESC Z
      sPosX = nTermChrX;
      sPosY = nTermChrY;
      fMoveCursor = TRUE;

      memset(bTermTabs, FALSE, sizeof(bTermTabs));

      // dca00034 MKL added one parameter to VidClearPage
      VidClearPage(0x7020);
      VidSetCaretPos();
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

      szBuf[0] = (char)wParam;
      i = 1;

      if (szBuf[0] == ASC_CR && bTermLFCR)
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

      // 00059 Enhance IBM 3101 with ESC X and ESC Z
      if (!fMoveCursor)
	{
	nTermChrX = sPosX;
	nTermChrY = sPosY;
	fMoveCursor = TRUE;
	}

      x++;
      if (x < 1) x = 1;
      if (x > TERM_COL) x = TERM_COL;
      if (y < 0) y = 0;
      if (y >= TERM_ROW) y = TERM_ROW - 1;

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
      if (nX1 > TERM_COL) nX1 = TERM_COL;
      if (nX2 < 1) nX2 = 1;
      if (nX2 > TERM_COL) nX2 = TERM_COL;

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
  int	nCol, nInx;


  if ((pLine = VssGetLine(nRow)) == NULL)
    return NULL;

  for (nCol = nX1, nInx = 0; nCol <= nX2; nCol++)
    {
    if (' ' != LOBYTE(pLine[nCol]) && nInx)
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


void near IBM3101CharOut(nChar)
  int  nChar;
{
  PINT	pLine;
  PINT	pChar;
  RECT	Rect;
  int	nXPos, nYPos, nWidth;
  int	i;


  pLine = pPriVidLine[sPosY];
  pChar = pLine + sPosX;

  if (*pChar == nChar)
    return;

  *pChar = nChar;

  nXPos = (sPosX - nTermOffX - 1) * nCurFontWidth;
  nYPos = (sPosY - nTermOffY) * nCurFontHeight;

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

  // dca00078 MKL fixed "print at" problem
  sPosX = nTermChrX;
  sPosY = nTermChrY;

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
      i = TermIBM3101Escape(szBuf, nChrCnt);

      szBuf   += i;
      nChrCnt -= i;
      continue;
      }

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
	  if (sPosX > 1)
	    sPosX--;

	  if (nCaptureInx)
	    nCaptureInx--;
	  break;

	case ASC_HT:
	  for (i = sPosX; i <= TERM_COL; i++)
	    if (bTermTabs[i-1] != FALSE)
	      break;

	  if (i > TERM_COL)
	    sPosX = TERM_COL;
	  else
	    sPosX = i;
	  break;

	case ASC_CR:
	  sPosX = 1;

	  if (!bTermLFCR)
	    break;
	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
            WriteCharToCapture(' ');

	case ASC_LF:
	  if (sPosY == TERM_ROW - 1)
	    VidScrollUp(0, TERM_ROW - 1);
	  else
	    sPosY++;

	  if (bTermLFCR)
	    sPosX = 1;

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    FlushCaptureBuf();

	  if (bPrinterOn)
	    PrintLine(NULL);
	  break;

	case ASC_FF:
	  sPosX = 1;
	  sPosY = 0;
	  // dca00034 MKL added one parameter to VidClearPage
	  VidClearPage(0x7020);
	  if (bPrinterOn)
	    PrintLine(NULL);
	  break;

	case ASC_CAN:
	  f3101MaskOut = TRUE;
	  break;

	case ASC_ESC:
	  bTermEscape = TRUE;
	  nTermInx  = 0;
	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    FlushCaptureBuf();
	  break;

	case ASC_DEL:
	  if (sPosX > 1)
	    sPosX--;
          IBM3101CharOut(' ');
	  if (nCaptureInx)
	    nCaptureInx--;
	  if (bPrinterOn)
	    PrintChar(' ', sPosX-1);
	  break;

	default:
          if (*szBuf < ' ')
	    break;

	  if ((sPosX > TERM_COL) ||
	      (Term.fWindowWrap && sPosX > nTermExtX))
	    {
	    if (Term.fAutoWrap)
	      {
	      sPosX = 1;
	      if (sPosY == TERM_ROW - 1)
		{
		VidScrollUp(0, TERM_ROW - 1);
		}
	      else if (sPosY < TERM_ROW - 1)
		sPosY++;

	      if (bPrinterOn)
		PrintLine(NULL);
	      }
	    else
	      sPosX = TERM_COL;
	    }

	  IBM3101CharOut(*szBuf);

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    WriteCharToCapture(*szBuf);

	  if (bPrinterOn)
	    PrintChar(*szBuf, sPosX - 1);

	  sPosX++;

	  // 00062 Change emulation to scroll at end.
	  if ((sPosX > TERM_COL) ||
	      (Term.fWindowWrap && sPosX > nTermExtX))
	    {
	    if (Term.fAutoWrap)
	      {
	      sPosX = 1;
	      if (sPosY == TERM_ROW - 1)
		{
		VidScrollUp(0, TERM_ROW - 1);
		}
	      else if (sPosY < TERM_ROW - 1)
		sPosY++;

	      if (bPrinterOn)
		PrintLine(NULL);
	      }
	    else
	      sPosX = TERM_COL;
	    }

	  break;
	}

      szBuf++;
      nChrCnt--;
    }

  // 00059 Enhance IBM 3101 with ESC X and ESC Z
  if (fMoveCursor)
    {
    nTermChrX = sPosX;
    nTermChrY = sPosY;
    }

  VidSeeMe(sPosY);

  if (fTermFocusActive)
    {
    VidSetCaretPos();
    ShowCaret(hChildTerminal);
    fTermCaretActive = TRUE;
    }

  // dca00078 MKL fixed "print at" problem
  nTermChrX = sPosX;
  nTermChrY = sPosY;
}

BOOL EmulatorIBM3101Key(msg)
  LPMSG msg;
{
  char szBuf[32];
  int  i;


  if (msg->message != WM_KEYDOWN)
    return FALSE;

  szBuf[0] = ASC_ESC;
  i = 0;

  switch (msg->wParam)
    {
    case '1':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'a';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '2':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'b';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '3':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'c';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '4':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'd';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '5':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'e';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '6':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'f';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '7':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'g';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '8':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'h';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '9':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'i';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case '0':
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'j';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case 189:
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'k';
	szBuf[2] = Term.szEnter[1];
	i = 3;
	}
      break;

    case 187:
      if (GetKeyState(VK_CONTROL) < 0)
	{
        szBuf[1] = 'l';
	szBuf[2] = Term.szEnter[1];
	i = 3;
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
        szBuf[1] = 'J';
	i = 2;
	}
      else
	{
        szBuf[1] = 'I';
	i = 2;
	}
      break;

    case VK_NEXT:
      szBuf[1] = 'L';
      i = 2;
      break;

    case VK_RETURN:
      f3101MaskOut = FALSE;
      if (GetKeyState(VK_CONTROL) < 0)
	szBuf[0] = ASC_LF;
      else
	szBuf[0] = Term.szEnter[1];

      i = 1;
      break;

    default:
      return FALSE;
    }

  if (i == 0)
    return FALSE;

  szBuf[i] = NULL;

  CommDataOutput(szBuf, i);
  if (Term.fEcho)
    TermInput(szBuf, i);

  return TRUE;
}

static int near TermIBM3101Escape(szBuf, nChrCnt)
  char *szBuf;
  int  nChrCnt;
{
  int  nCount;


  nCount = 0;
  while (nChrCnt-- > 0 && bTermEscape)
    {
    nCount++;
    cTermBuf[nTermInx++] = *szBuf++;

    bTermEscape = FALSE;
    switch (cTermBuf[0])
      {
      case '0':
	bTermTabs[sPosX-1] = TRUE;
	break;

      case '1':
	bTermTabs[sPosX-1] = FALSE;
	break;

      // dca00032 MKL fixed keyboard lock unlock problem
      case ':':
	bTermLckKey = TRUE;
	SendMessage(hChildStatus, UM_KEYPAD, 3, (long)nTermMaxCol);
	break;

      // dca00032 MKL fixed keyboard lock unlock problem
      case ';':
	bTermLckKey = FALSE;
	SendMessage(hChildStatus, UM_KEYPAD, 0, (long)nTermMaxCol);
	break;

      case 'A':
	if (sPosY != 0)
	  sPosY--;
	break;

      case 'B':
	if (sPosY < TERM_ROW - 1)
	  sPosY++;
	break;

      case 'C':
	if (sPosX < TERM_COL)
	  sPosX++;
	break;

      case 'D':
	if (sPosX > 1)
	  sPosX--;
	break;

      case 'H':
	sPosX = 1;
	sPosY = 0;
	break;

      case 'I':
	VidClearArea(sPosX, sPosY, TERM_COL, sPosY);
	break;

      case 'X':
      case 'Y':
	if (nTermInx > 2)
	  {
	  // 00059
	  if (cTermBuf[0] == 'X')
	    fMoveCursor = FALSE;
	  else
	    fMoveCursor = TRUE;

	  sPosY = cTermBuf[1] - 32;
	  sPosX = cTermBuf[2] - 31;

	  if (sPosX < 1)
	    sPosX = 1;
	  else if (sPosX > TERM_COL)
	    sPosX = TERM_COL;

	  if (sPosY < 0)
	    sPosY = 0;
	  else if (sPosY >= TERM_ROW)
	    sPosY = TERM_ROW - 1;
	  }
	else
	  bTermEscape = TRUE;
	break;

      case 'Z':
	// 00059
	nTermChrX = sPosX;
	nTermChrY = sPosY;
	fMoveCursor = TRUE;
	break;

      case 'K':
      case 'L':
	sPosX = 1;
	sPosY = 0;

      case 'J':
	VidClearArea(sPosX, sPosY, TERM_COL, TERM_ROW - 1);
	break;
      }
    }

  return (nCount);
}
