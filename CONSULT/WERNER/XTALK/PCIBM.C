/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   IBM PC Terminal Emulation (PCIBM.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*  1.01 09/06/89 00027 Auto start for sending.								   *
*  1.01 12/31/89 00078 Problem with ESC [ C.								   *
*  1.01 01/19/90 00090 Check for cTabWidth being equal to zero.			   *
*  1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage			   *
*					routine to support VT102 screen alignment cap.			   *
*  1.1  03/01/90 dca00036 MKL fixed display attribute problem.				   *
*  1.1  04/01/90 dca00064 PJL Automatically set OEM (Terminal) font.		   *
*  1.1  04/25/90 dca00095 MKL changed IBMPC terminal emulation back to the way*
*	it was: ^[[0m sets background and foreground to user's setup color,   *
*	not white on black						      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <dos.h>
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
#include "xfer.h"
#include "prodata.h"
#include "dialogs.h"


#define PCIBM_MAXCOL	80
#define PCIBM_MAXROW	24

static void near CreateFonts();
static void near DeleteFonts();
static void near UpdateFonts();
static void near CharInput(unsigned char *, int);
static BOOL near CharOut(int);
static int  near EscapeSeq(unsigned char *, int);

static BOOL near RefreshLine(int, int, int);
static void near RefreshStr(int, int, PSTR, int, int);
static void near SetCurColor(char);

HFONT hPCIBMFonts[4];

void PCIBMInit()
{
	char szBuf[16];

	nTermChrX   = 1;
	nTermChrY   = 0;
	cTermAttr   = 0;

	nSaveChrX   = 1;
	nSaveChrY   = 0;
	nSaveAttr   = 0;

	nTermOffX   = 0;
	nTermOffY   = 0;

	/* MKL */
	bTermEscape = FALSE;

	// dca00064 Automatically set OEM (Terminal) font.
	ResetFont(FALSE);
	CreateFonts();

	SendMessage(hChildTerminal, UM_RESET, 0, 0L);

	LoadString(hWndInst, IDS_EMULATOR_PCIBM, szBuf, sizeof(szBuf));
	SendMessage(hChildStatus, UM_EMULATOR, (WORD)szBuf, 0L);
}


void PCIBMTerm()
{

	DeleteFonts();
}


static void near CreateFonts()
{
	int i;

	for (i = 0; i < sizeof(hPCIBMFonts) / sizeof(HFONT); i++)
		hPCIBMFonts[i] = NULL;

	UpdateFonts();
}


static void near DeleteFonts()
{
	int i;

	SelectObject(hTerminalDC, hSysFont);
	cTermFontAttr = -1;

	for (i = 0; i < sizeof(hPCIBMFonts) / sizeof(HFONT); i++)
	{
		if (hPCIBMFonts[i] != NULL)
		{
			DeleteObject(hPCIBMFonts[i]);
			hPCIBMFonts[i] = NULL;
		}
	}
}


static void near UpdateFonts()
{
	LOGFONT LogFont;
	char buf[64];

	DeleteFonts();

	GetObject(hCurFont, sizeof(LOGFONT), (LPSTR)&LogFont);

	LogFont.lfHeight    = nCurFontHeight;
	LogFont.lfWidth     = nCurFontWidth;
	LogFont.lfCharSet   = bCurFontCharSet;

	LogFont.lfWeight    = 400;
	LogFont.lfUnderline = FALSE;
	hPCIBMFonts[0] = CreateFontIndirect((LPLOGFONT)&LogFont);

	LogFont.lfWeight    = 700;
	hPCIBMFonts[1] = CreateFontIndirect((LPLOGFONT)&LogFont);

	LogFont.lfWeight    = 400;
	LogFont.lfUnderline = TRUE;
	hPCIBMFonts[2] = CreateFontIndirect((LPLOGFONT)&LogFont);

	LogFont.lfWeight    = 700;
	hPCIBMFonts[3] = CreateFontIndirect((LPLOGFONT)&LogFont);
}


long WndPCIBMProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	int	 i, x, y;
	int	 nX1, nX2, nY1, nY2;
	char	 szBuf[1];

	switch (message)
	{
	case UM_RESET:
		nTermChrX = 1;
		nTermChrY = 0;
		nSaveChrX = 1;
		nSaveChrY = 0;
		bTermEscape = FALSE;

		SelectObject(hTerminalDC, hPCIBMFonts[0]);
		// dca00036 MKL fixed display attribute problem
		cTermAttr = 0x70;
		// cTermAttr = (Disp.nForeground&0x0007)<<4 | (Disp.nBackground&0x0007);
		nSaveAttr = cTermAttr;
		cTermFontAttr = -1;
		cTermFontColor = -1;
		bTermRevr = FALSE;

		// dca00034 MKL added one parameter to VidClearPage  *
		VidClearPage(nSaveAttr<<8|0x0020);
		VidSetCaretPos();

		cZmodemSeq = 0;
		break;

	case UM_NEWFONT:
		UpdateFonts();
		break;

	case UM_DATA:
		CharInput((PSTR)LOWORD(lParam), wParam);
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
			szBuf[i++] = ASC_LF;
		CommDataOutput(szBuf, i);
		if (Term.fEcho)
			CharInput(szBuf, i);
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
		if (nX1 > PCIBM_MAXCOL) nX1 = PCIBM_MAXCOL;
		if (nX2 < 1) nX2 = 1;
		if (nX2 > PCIBM_MAXCOL) nX2 = PCIBM_MAXCOL;

		nY1 = ps.rcPaint.top / nCurFontHeight + nTermOffY;
		if (ps.rcPaint.top == ps.rcPaint.bottom)
			nY2 = nY1;
		else
			nY2 = (ps.rcPaint.bottom - 1) / nCurFontHeight + nTermOffY;

		for (i = nY1; i <= nY2; i++)
		{
			if (!RefreshLine(i, nX1, nX2))
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
	char	cCurAttr, cCurBuff[VID_MAXCOL+1];
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
			if ((pLine[nCol] & ~(TERM_ATTR_BOLD << 8)) != ((TERM_ATTR_WHITE << 12) | ' '))
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
	RECT Rect;
	int  nXPos, nYPos;
	int  nDelta[VID_MAXCOL+1];
	int  i;

	/*
	nType = *GetVidLine(nRow);
	*/

	i = 0;
	if (nAttr & TERM_ATTR_BOLD)
		i += 1;
	if (nAttr & TERM_ATTR_UNDERLINE)
		i += 2;

	if (i != cTermFontAttr)
	{
		cTermFontAttr = (char)i;
		if (SelectObject(hTerminalDC, hPCIBMFonts[cTermFontAttr]) == NULL)
			SelectObject(hTerminalDC, hCurFont);
	}

	if (((i = nAttr) & 0x77) != cTermFontColor)
		SetCurColor((char)i);

	nXPos = (nCol - nTermOffX - 1) * nCurFontWidth;
	nYPos = (nRow - nTermOffY) * nCurFontHeight;

	for (i = 0; i < nLen; i++)
		nDelta[i] = nCurFontWidth;

	Rect.top    = nYPos;
	Rect.left   = nXPos;
	Rect.bottom = nYPos + nCurFontHeight;
	Rect.right  = nXPos + nCurFontWidth * nLen;

	ExtTextOut(hTerminalDC, nXPos, nYPos, 0x04, (LPRECT)&Rect, pText, nLen, (LPINT)nDelta);
}


/* 
	CharOut()
	the low byte of the parameter nChar is for ascii value,
	the high byte of the parameter nChar is for attribute as follows:

	--------------+------------+------+-------------
	bit   7       |  6   5   4 |   3  |   2  1  0
	--------------+------------+------+-------------
            underline | foreground | bold | background
	--------------+------------+------+-------------
*/

static BOOL near CharOut(nChar)
int  nChar;
{
	PINT	pLine;
	PINT	pChar;
	RECT	Rect;
	int	nXPos, nYPos, i;

	if (nChar == NULL)
	{
		if ((pLine = VssGetLine(nTermChrY)) == NULL)
			return FALSE;
	}
	else pLine = pPriVidLine[nTermChrY];

	pChar = pLine + nTermChrX;

	if (nChar != NULL)
	{
		nChar = (nChar & 0x00FF) | cTermAttr << 8;

		if (*pChar == nChar) return TRUE;

		*pChar = nChar;
	}
	// dca00036 MKL fixed display attribute problem
	else if (*pChar == 0x7020) return TRUE;
	// else nChar = *pChar;

	i = 0;
	if (HIBYTE(nChar) & TERM_ATTR_BOLD) i += 1;
	if (HIBYTE(nChar) & TERM_ATTR_UNDERLINE) i += 2;
	/* i=0 normal, i=1 bold, i=2 underline, i=3 bold underline */
	if (i != cTermFontAttr)
	{
		cTermFontAttr = (char)i;
		SelectObject(hTerminalDC, hPCIBMFonts[cTermFontAttr]);
	}

	if ((i = (HIBYTE(nChar) & 0x77)) != cTermFontColor) SetCurColor((char)i);

	nXPos = (nTermChrX - nTermOffX - 1) * nCurFontWidth;
	nYPos = (nTermChrY - nTermOffY) * nCurFontHeight;

	TextOut(hTerminalDC, nXPos, nYPos, (LPSTR)pChar, 1);

	return TRUE;
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
		// dca00036 MKL fixed display attribute problem
		dwForeground = ChrAttrColor[Disp.nBackground];
		// dwForeground = RGB(0, 0, 0);
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
		// dca00036 MKL fixed display attribute problem
		dwForeground = ChrAttrColor[Disp.nForeground];
		// dwForeground = RGB(255, 255, 255);
		break;
	}

	switch (cColor & 0x07)
	{
	case TERM_ATTR_BLACK:
		// dca00036 MKL fixed display attribute problem
		dwBackground = ChrAttrColor[Disp.nBackground];
		// dwBackground = RGB(0, 0, 0);
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
		// dca00036 MKL fixed display attribute problem
		dwBackground = ChrAttrColor[Disp.nForeground];
		// dwBackground = RGB(255, 255, 255);
		break;
	}

	if (!bTermRevr)
	{
		SetTextColor(hTerminalDC, dwForeground);
		SetBkColor(hTerminalDC, dwBackground);
	}
	else
	{
		SetTextColor(hTerminalDC, dwBackground);
		SetBkColor(hTerminalDC, dwForeground);
	}
}


static void near CharInput(szBuf, nChrCnt)
unsigned char *szBuf;
int  nChrCnt;
{
	int  i;

	if (nChrCnt <= 0)
		return;

	if (fTermCaretActive)
		HideCaret(hChildTerminal);

	while (nChrCnt > 0)
	{
		if (bTermEscape)
		{
			i = EscapeSeq(szBuf, nChrCnt);

			szBuf   += i;
			nChrCnt -= i;
			continue;
		}

		switch (*szBuf)
		{
		case ASC_NUL:
		case ASC_STX:
		case ASC_ETX:
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
			CharOut(' ');
			if (nCaptureInx)
				nCaptureInx--;
			break;

		case ASC_HT:
			// 00090 Check for zero TabWidth.
			if (Term.cTabWidth)
			{
				nTermChrX = (((nTermChrX - 1) / Term.cTabWidth) + 1) * Term.cTabWidth + 1;
				if (nTermChrX > PCIBM_MAXCOL)
					nTermChrX = PCIBM_MAXCOL;
			}
			break;

		case ASC_CR:
			nTermChrX = 1;
			if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
			{
				WriteCharToCapture(' ');
				FlushCaptureBuf();
			}
			if (!Term.fLfAuto)
				break;

		case ASC_LF:
			if (nTermChrY == PCIBM_MAXROW - 1)
				VidScrollUp(0, PCIBM_MAXROW - 1);
			else
				nTermChrY++;

			if (Term.fLfAuto)
				nTermChrX = 1;

			if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
				FlushCaptureBuf();

			if (bPrinterOn)
				PrintLine(NULL);
			break;

		case ASC_FF:
			nTermChrX = 1;
			nTermChrY = 0;
			// dca00034 MKL added one parameter to VidClearPage
			VidClearPage(0x7020);
			break;

		case ASC_ESC:
			bTermEscape = TRUE;
			nTermInx    = 0;

			if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
				FlushCaptureBuf();
			break;

		default:
			if (*szBuf == ASC_CAN)
				cZmodemSeq = 1;
			else
			{
				switch(cZmodemSeq)
				{
				case 3:
					if (*szBuf == '0')
						cZmodemSeq = 4;
					else if (*szBuf == '1')  // 00027 Set to 5 for zmodem send request.
						cZmodemSeq = 5;
					else
						cZmodemSeq = 0;
					break;

				case 2:
					if (*szBuf == '0')
						cZmodemSeq++;
					else
						cZmodemSeq = 0;
					break;

				case 1:
					if (*szBuf == 'B')
						cZmodemSeq++;
					else
						cZmodemSeq = 0;
					break;
				}
			}

			if (!Sess.fGraphics)
				*szBuf &= 0x7F;

			if ((nTermChrX > PCIBM_MAXCOL) ||
				(Term.fWindowWrap && nTermChrX > nTermExtX))
			{
				nTermChrX = 1;
				if (nTermChrY == PCIBM_MAXROW - 1)
					VidScrollUp(0, PCIBM_MAXROW - 1);
				else
					nTermChrY++;

				if (bPrinterOn)
					PrintLine(NULL);
			}

			CharOut(*szBuf);

			if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
				WriteCharToCapture(*szBuf);

			if (bPrinterOn)
				PrintChar(*szBuf, nTermChrX - 1);

			nTermChrX++;
			break;
		}

		szBuf++;
		nChrCnt--;

		if (cZmodemSeq > 3)
			break;
	}

	if (fTermCaretActive)
	{
		VidSetCaretPos();
		ShowCaret(hChildTerminal);
	}

	if (cZmodemSeq > 3)
	{
		register i;

		nTermChrX = 1;
		VidClearArea(nTermChrX, nTermChrY, 8, nTermChrY);
		if (cZmodemSeq == 4)
			i = XferZmodemRecvName(NULL);
		else
			i = XferZmodemSendName(NULL);  // 00027 Auto start zmodem for send packet.

		if (i)
			GeneralError(i);

		cZmodemSeq = 0;
	}

	VidSeeMe(nTermChrY);
}


BOOL EmulatorPCIBMKey(msg)
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
      szBuf[1] = '[';
      szBuf[2] = 'A';
      i = 3;
      break;

    case VK_DOWN:
      szBuf[1] = '[';
      szBuf[2] = 'B';
      i = 3;
      break;

    case VK_LEFT:
      szBuf[1] = '[';
      szBuf[2] = 'D';
      i = 3;
      break;

    case VK_RIGHT:
      szBuf[1] = '[';
      szBuf[2] = 'C';
      i = 3;
      break;

    case VK_HOME:
      szBuf[1] = '[';
      szBuf[2] = 'H';
      i = 3;
      break;

    case VK_END:
      szBuf[1] = '[';
      szBuf[2] = 'K';
      i = 3;
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
    CharInput(szBuf, i);
    return TRUE;
    }

  szBuf[i] = NULL;

  CommDataOutput(szBuf, i);
  if (Term.fEcho)
    CharInput(szBuf, i);

  return TRUE;
}

static int near EscapeSeq(szBuf, nChrCnt)
  unsigned char *szBuf;
  int  nChrCnt;
{
  char buf[16];
  int  nCount;
  int  i, x, y;
  int  ch;


  nCount = 0;
  while (nChrCnt-- > 0 && bTermEscape)
    {
    nCount++;
    ch = *szBuf++;

    if (ch == ASC_CAN || ch == ASC_SUB)
      {
      cTermBuf[0] = ASC_CAN;
      }
    else if (nTermInx == 0 || ch == ASC_ESC)
      {
      cTermBuf[0] = (char)ch;
      nTermInx = 1;
      memset(&cTermBuf[1], 0, sizeof(cTermBuf) - 1);
      if (ch == '[')
	continue;
      }

    switch (cTermBuf[0])
      {
      case ASC_CAN:
	bTermEscape = FALSE;
	break;

      case '[':
	switch (ch)
	  {
          case ';':
	    nTermInx++;
	    if (nTermInx >= sizeof(cTermBuf))
	      bTermEscape = FALSE;
	    break;

          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            cTermBuf[nTermInx] = cTermBuf[nTermInx] * 10 + (ch - '0');
	    break;

          case 'A':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    nTermChrY -= i;
	    if (nTermChrY < 0 || nTermChrY >= PCIBM_MAXROW)
	      nTermChrY = 0;

	    bTermEscape = FALSE;
	    break;

          case 'B':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    nTermChrY += i;
	    if (nTermChrY < 0 || nTermChrY >= PCIBM_MAXROW)
	      nTermChrY = PCIBM_MAXROW - 1;

	    bTermEscape = FALSE;
	    break;

          case 'C':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    // 00078 Problem with ESC [ C.
	    nTermChrX += i;
	    if (nTermChrX < 1)
	      {
	      nTermChrX = 1;
	      }
	    else if (nTermChrX > PCIBM_MAXCOL)
	      {
	      nTermChrX -= PCIBM_MAXCOL;
	      nTermChrY++;
	      }

	    bTermEscape = FALSE;
	    break;

          case 'D':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    nTermChrX -= i;
	    if (nTermChrX < 1 || nTermChrX > PCIBM_MAXCOL)
	      nTermChrX = 1;

	    bTermEscape = FALSE;
	    break;

          case 'J':
	    switch (cTermBuf[1])
	      {
	      case 2:
		nTermChrX = 1;
		nTermChrY = 0;
		// dca00034 MKL added one parameter to VidClearPage
		VidClearPage(0x7020);
		break;
	      }

	    bTermEscape = FALSE;
	    break;

          case 'K':
	    switch (cTermBuf[1])
	      {
	      case 0:
		VidClearArea(nTermChrX, nTermChrY, PCIBM_MAXCOL, nTermChrY);
		break;
	       }

	    bTermEscape = FALSE;
	    break;

          case 'H':
          case 'f':
	    if ((nTermChrY = cTermBuf[1] - 1) < 0)
	      nTermChrY = 0;
	    else if (nTermChrY > PCIBM_MAXROW - 1)
	      nTermChrY = PCIBM_MAXROW - 1;


	    if ((nTermChrX = cTermBuf[2]) < 1)
	      nTermChrX = 1;
	    else if (nTermChrX > PCIBM_MAXCOL)
	      nTermChrX = PCIBM_MAXCOL;

	    bTermEscape = FALSE;
	    break;

          case 'm':
	    for (i = 1; i <= nTermInx; i++)
	      {
	      switch (cTermBuf[i])
		{
		case 0:
		  cTermAttr  = 0x70;
		  break;

		case 1:
		  if (Disp.nBolding <= TERM_ATTR_WHITE)
		    cTermAttr = (cTermAttr & 0x8F) | (Disp.nBolding << 4);
		  else if (Disp.nBolding == TERM_ATTR_WHITE+1)
		    cTermAttr |= TERM_ATTR_BOLD;
		  else
		    cTermAttr |= TERM_ATTR_UNDERLINE;
		  break;

		case 4:
		  cTermAttr |= TERM_ATTR_UNDERLINE;
		  break;

		case 5:
		  if (Disp.nBlinking <= TERM_ATTR_WHITE)
		    cTermAttr = (cTermAttr & 0x8F) | (Disp.nBlinking << 4);
		  else if (Disp.nBlinking == TERM_ATTR_WHITE+1)
		    cTermAttr |= TERM_ATTR_BOLD;
		  else
		    cTermAttr |= TERM_ATTR_UNDERLINE;
		  break;

		case 7:
		  if ((cTermAttr & 0x70) == (TERM_ATTR_WHITE << 4))
		    cTermAttr = (char)(0x07 | (cTermAttr & 0x80));
		  else
		    cTermAttr = (char)(0x07 | (cTermAttr & 0xF0));
		  break;

		case 8:		/* ^[[8m invisible attribute */
		  /* cTermAttr high nibble is for foreground 
				low nibble is for background */
		  // dca00036 MKL fixed display attribute problem
		  // cTermAttr = 0x00;
		  cTermAttr = (char)(0x07 & cTermAttr)|((0x07 & cTermAttr)<<4);
		  break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		  if (Term.fNoColor || fMonoDisplay)
		    cTermAttr = (cTermAttr & 0x88) | 0x70;
		  else
		    cTermAttr = (cTermAttr & 0x8F) + (cTermBuf[i] - 30 << 4);
		  break;

		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		  if (Term.fNoColor || fMonoDisplay)
		    cTermAttr = (cTermAttr & 0x88) | 0x70;
		  else
		    cTermAttr = (cTermAttr & 0xF8) + cTermBuf[i] - 40;
		  break;
		}
	      }

	    bTermEscape = FALSE;
	    break;

          case 'n':
	    switch (cTermBuf[1])
	      {
	      case 6:
                sprintf(buf, "\033[%d;%dR", nTermChrY + 1, nTermChrX);
		CommDataOutput(buf, strlen(buf));
		break;
	      }

	    bTermEscape = FALSE;
	    break;

          case 's':
	    nSaveChrX = nTermChrX;
	    nSaveChrY = nTermChrY;
	    bTermEscape = FALSE;
	    break;

          case 'u':
	    nTermChrX = nSaveChrX;
	    nTermChrY = nSaveChrY;
	    bTermEscape = FALSE;
	    break;

	  default:
	    bTermEscape = FALSE;
	  }
	break;

      default:
	bTermEscape = FALSE;
      }
    }

  return (nCount);
}
