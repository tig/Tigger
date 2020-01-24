/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   VT102 Character Support Module (VTCHAR.C)                       *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:								      *
*    1.00 07/17/89 Initial Version.                                  *
*    1.01 09/08/89 Add ESC [ n G support (goto to column).           *
*    1.01 01/15/90 Only return on ^[[0c is argument is 0.            *
*    1.01 01/19/90 00090 Check for cTabWidth being equal to zero.    *
*    1.01 02/01/90 dca00013 MKL Allow ^[[?1h to switch user into     *
*                           application mode                     *
*    1.01 02/01/90 dca00014 MKL change scolling region limit from    *
*  		   	    2 lines to 1			      *
*    1.01 02/02/90 dca00015 MKL If keyboard is already in applica-   *
*                           tion cursor mode, ignore ^[=.        *
*    1.01 02/09/90 dca00022 PJL/MKL Fix Vt100 cursor/keypad em.		      *
*    1.1  03/01/90 dca00030 MKL added VT102 device attribute capability	      *
*    1.1  03/01/90 dca00031 MKL added VT102 Load LED cap, added "L1" indicator*
*			    to infomation bar				      *
*    1.1  03/01/90 dca00032 MKL fixed keyboard lock unlock problem	      *
*    1.1  03/01/90 dca00033 MKL added Auto keyborad repeat ON/OFF capability  *
*    1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage routine to*
*			    support VT102 screen alignment capability	      *
*    1.1  03/01/90 dca00035 MKL added cursor enable/disable capability	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "capture.h"
#include "comm.h"
#include "disp.h"
#include "font.h"
#include "sess.h"
#include "prodata.h"

static int  near TermVT52Escape(char *, int);
static int  near TermVT102Escape(char *, int);
static int  near TermCharXlat(WORD);
static void near TermLinePrint(int);
static void near TermScreenPrint();

static int  nTermCntlXPos;

void near TermInput(szBuf, nChrCnt)
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
      if (fTermVT52Mode)
	i = TermVT52Escape(szBuf, nChrCnt);
      else
	{
	if (*szBuf == ASC_ESC)
	  {
	  szBuf++;
	  nChrCnt--;
	  }
	if (nChrCnt)
	  i = TermVT102Escape(szBuf, nChrCnt);
	else
	  i = 0;
	}

      szBuf   += i;
      nChrCnt -= i;
      continue;
      }


    if (bTermCntlPrint)
      {
      switch (*szBuf)
	{
	case ASC_BS:
	  if (nTermCntlXPos)
	    nTermCntlXPos--;
	  break;

	case ASC_HT:
          for (i = nTermCntlXPos+1; i <= nTermMaxCol; i++)
	    if (bTermTabs[i-1] != FALSE)
	      break;

	  if (i > nTermMaxCol)
            nTermCntlXPos = nTermMaxCol;
	  else
            nTermCntlXPos = i;
	  break;

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

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    FlushCaptureBuf();
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
	  fMouseCursor = FALSE;
	  MessageBeep(NULL);
	  break;

	case ASC_BS:
	  if (nTermChrX > 1)
	    nTermChrX--;

	  if (nCaptureInx)
	    nCaptureInx--;
	  break;

	case ASC_HT:
	  for (i = nTermChrX+1; i <= nTermMaxCol; i++)
	    if (bTermTabs[i-1] != FALSE)
	      break;

	  if (i > nTermMaxCol)
	    nTermChrX = nTermMaxCol;
	  else
	    nTermChrX = i;
	  break;

	case ASC_CR:
	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
            {
            WriteCharToCapture(' ');
	    FlushCaptureBuf();
            }
	  nTermChrX = 1;
          if (!bTermLFCR)
            break;

	case ASC_LF:
	case ASC_VT:
	case ASC_FF:
	  if (nTermChrY == nTermMarginBot)
	    VidScrollUp(nTermMarginTop, nTermMarginBot);
	  else if (nTermChrY < TERM_MAXROW - 1)
	    nTermChrY++;

	  if (bTermLFCR)
	    nTermChrX = 1;

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    FlushCaptureBuf();

	  if (bPrinterOn || bTermAutoPrint)
	    PrintLine(NULL);
	  break;

	case ASC_SO:
	  if (Sess.fGraphics)
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
   //     if (nTermChrX > 1)
   //       nTermChrX--;
   //     VT102CharOut(' ');
   //     if (nCaptureInx)
   //       nCaptureInx--;
   //     if (bPrinterOn || bTermAutoPrint)
   //       PrintChar(' ', nTermChrX-1);
          break;

	default:
          if (*szBuf < ' ')
	    break;

	  fMouseCursor = FALSE;

	  if ((nTermChrX > nTermMaxCol) ||
	      (Term.fWindowWrap && nTermChrX > nTermExtX))
	    {
	    if (bTermWrap)
	      {
	      nTermChrX = 1;
	      if (nTermChrY == nTermMarginBot)
		{
		VidScrollUp(nTermMarginTop, nTermMarginBot);
		}
	      else if (nTermChrY < TERM_MAXROW - 1)
		nTermChrY++;

	      if (bPrinterOn || bTermAutoPrint)
		PrintLine(NULL);
	      }
	    else
	      nTermChrX = nTermMaxCol;
	    }

	  VT102CharOut(TermCharXlat(*szBuf));

	  if (bCaptureOn && Sess.cLogOption == SESSION_NORMAL)
	    WriteCharToCapture(*szBuf);

	  if (bPrinterOn || bTermAutoPrint)
	    PrintChar(*szBuf, nTermChrX - 1);

	  if (bTermWrap || nTermChrX != nTermMaxCol)
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


BOOL EmulatorVT102Key(msg)
  LPMSG msg;
{
  char szBuf[64];
  char fLocal;
  int  i;


  // dca00032 MKL fixed keyboard lock unlock problem
  // if (bTermLckKey) return (TRUE);

  szBuf[0] = ASC_ESC;
  fLocal   = FALSE;
  i = 0;

  switch (msg->wParam)
    {
    case VK_F1:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'P';
	i = 2;
	}
      else
	{
        szBuf[1] = 'O';
        szBuf[2] = 'P';
	i = 3;
	}
      break;

    case VK_F2:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'Q';
	i = 2;
	}
      else
	{
        szBuf[1] = 'O';
        szBuf[2] = 'Q';
	i = 3;
	}
      break;

    case VK_F3:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'R';
	i = 2;
	}
      else
	{
        szBuf[1] = 'O';
        szBuf[2] = 'R';
	i = 3;
	}
      break;

    case VK_F4:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'S';
	i = 2;
	}
      else
	{
        szBuf[1] = 'O';
        szBuf[2] = 'S';
	i = 3;
	}
      break;

    case VK_F5:
      if (bTermAltKey)
        szBuf[1] = '>';
      else
        szBuf[1] = '=';
      i = 2;
      fLocal = TRUE;
      break;

    case VK_F6:
      if (fTermVT52Mode)
	{
        szBuf[1] = '<';
	i = 2;
	}
      else
	{
        szBuf[1] = '[';
        szBuf[2] = '?';
        szBuf[3] = '2';
        szBuf[4] = 'l';
	i = 5;
	}
      fLocal = TRUE;
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
	if (fTermVT52Mode)
          szBuf[1] = '?';
	else
          szBuf[1] = 'O';

	szBuf[2] = (char)('p' + msg->wParam - VK_NUMPAD0);
	i = 3;
	}
      break;

    case VK_DECIMAL:
      if (GetKeyState(VK_NUMLOCK) & 0x01 != 0)
	{
	if (bTermAltKey)
	  {
	  if (fTermVT52Mode)
            szBuf[1] = '?';
	  else
            szBuf[1] = 'O';

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
      if (bTermAltKey && (GetKeyState(VK_NUMLOCK) & 0x01 != 0))
        {
        szBuf[1] = 'O';
        szBuf[2] = 'M';
        i = 3;
        }
      else
        {
        szBuf[0] = ASC_CR;
        i = 1;
        }
      break;

    case VK_SUBTRACT:
      if (GetKeyState(VK_NUMLOCK) & 0x01 != 0)
	{
	if (bTermAltKey)
	  {
	  if (fTermVT52Mode)
            szBuf[1] = '?';
	  else
            szBuf[1] = 'O';

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
	  if (fTermVT52Mode)
            szBuf[1] = '?';
	  else
            szBuf[1] = 'O';

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
      if (fTermVT52Mode)
	{
        szBuf[1] = 'A';
	i = 2;
	}
      else
        {
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//        szBuf[1] = '[';
//      if (bTermAltKey && bTermAltCur)
//        szBuf[1] = 'O';
        if (bTermAltCur) szBuf[1] = 'O';
		 else szBuf[1] = '[';
        szBuf[2] = 'A';
        i = 3;
	}
      break;

    case VK_DOWN:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'B';
	i = 2;
	}
      else
	{
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//        szBuf[1] = '[';
//        if (bTermAltKey && bTermAltCur)
//          szBuf[1] = 'O';
        if (bTermAltCur) szBuf[1] = 'O';
	     else szBuf[1] = '[';
        szBuf[2] = 'B';
        i = 3;
	}
      break;

    case VK_LEFT:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'D';
	i = 2;
	}
      else
	{
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//        szBuf[1] = '[';
//        if (bTermAltKey && bTermAltCur)
//          szBuf[1] = 'O';
        if (bTermAltCur) szBuf[1] = 'O';
	     else szBuf[1] = '[';
        szBuf[2] = 'D';
        i = 3;
	}
      break;

    case VK_RIGHT:
      if (fTermVT52Mode)
	{
        szBuf[1] = 'C';
	i = 2;
	}
      else
	{
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//        szBuf[1] = '[';
//        if (bTermAltKey && bTermAltCur)
//          szBuf[1] = 'O';
        if (bTermAltCur) szBuf[1] = 'O';
	     else szBuf[1] = '[';
        szBuf[2] = 'C';
        i = 3;
	}
      break;

    case VK_HOME:
      if (bTermAltKey)
        {
        szBuf[1] = 'O';
        szBuf[2] = 'w';
        i = 3;
        }
      break;

    case VK_INSERT:
      if (bTermAltKey)
        {
        if (fTermVT52Mode)
          szBuf[1] = '?';
        else
          szBuf[1] = 'O';

        szBuf[2] = 'p';
        i = 3;
        }
      break;

    case VK_DELETE:
      if (bTermAltKey)
        {
        if (fTermVT52Mode)
          szBuf[1] = '?';
        else
          szBuf[1] = 'O';

        szBuf[2] = 'n';
        i = 3;
        }
      break;

    case VK_END:
      if (bTermAltKey)
        {
        szBuf[1] = 'O';
        szBuf[2] = 'q';
        i = 3;
        }
      break;

    case VK_NEXT:
      if (bTermAltKey)
        {
        szBuf[1] = 'O';
        szBuf[2] = 's';
        i = 3;
        }
      break;

    case VK_PRIOR:
      if (bTermAltKey)
        {
        szBuf[1] = 'O';
        szBuf[2] = 'y';
        i = 3;
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
  if (bTermEcho)
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

    if (*szBuf)
      cTermBuf[nTermInx++] = *szBuf++;
    else
      {
      szBuf++;
      continue;
      }

    bTermEscape = FALSE;
    switch (cTermBuf[0])
      {
      case '<':
	fTermVT52Mode = FALSE;
	SendMessage(hChildStatus, UM_EMULATOR, 0, 1L);
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
            nTermChrY = TERM_MAXROW-1;

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
	SendMessage(hChildStatus, UM_KEYPAD, 1, (long)nTermMaxCol);
	break;

      case '>':
	bTermAltKey = FALSE;
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//	bTermAltCur = FALSE;
	SendMessage(hChildStatus, UM_KEYPAD, 0, (long)nTermMaxCol);
	break;

      case 'F':
	fTermGraphicsOn = TRUE;
	break;

      case 'G':
	fTermGraphicsOn = FALSE;
	break;

      case 'K':
	VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL-1, nTermChrY);
	break;

      case 'J':
	// dca00034 MKL added one parameter to VidClearPage  *
	if (nTermChrX == 1 && nTermChrY == 0)
	  VidClearPage(0x7020);
	else
          VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL, TERM_MAXROW-1);
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


static int near TermVT102Escape(szBuf, nChrCnt)
  char *szBuf;
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

    if (ch == NULL)
      {
      continue;
      }
    else if (ch == ASC_CAN || ch == ASC_SUB)
      {
      cTermBuf[0] = ASC_CAN;
      }
    else if (nTermInx == 0)
      {
      cTermBuf[0] = (char)ch;
      nTermInx = 1;
      memset(&cTermBuf[1], 0, sizeof(cTermBuf) - 1);
      bTermQFlg = FALSE;
      if (strchr("()[#*+", ch) != NULL)
	continue;
      }

    switch (cTermBuf[0])
      {
      case ASC_CAN:
	bTermEscape = FALSE;
	break;

      case 'D':
	if (nTermChrY == nTermMarginBot)
	  VidScrollUp(nTermMarginTop, nTermMarginBot);
	else
	  nTermChrY++;

	bTermEscape = FALSE;
	break;

      case 'E':
	if (nTermChrY == nTermMarginBot)
	  VidScrollUp(nTermMarginTop, nTermMarginBot);
	else
	  nTermChrY++;

	nTermChrX = 1;

	bTermEscape = FALSE;
	break;

      case 'H':
	bTermTabs[nTermChrX-1] = TRUE;
	bTermEscape = FALSE;
	break;

      case 'M':
	if (nTermChrY == nTermMarginTop)
	  VidScrollDown(nTermMarginTop, nTermMarginBot);
	else
	  nTermChrY--;

	bTermEscape = FALSE;
	break;

      case 'Z':
	// dca00030 MKL added VT102 device attribute capability
	buf[0] = ASC_ESC;
        buf[1] = '[';
        buf[2] = '?';
        buf[3] = '6';
        buf[4] = 'c';
	CommDataOutput(buf, 5);

	bTermEscape = FALSE;
	break;

      case 'c':
	TermReset();
	bTermEscape = FALSE;
	break;

      case '7':
	if (bTermCntlPrint)
	  {
	  bTermCntlPrint = FALSE;
	  SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 0L);
	  ClosePrinter();
	  }
	nSaveChrX   = nTermChrX;
	nSaveChrY   = nTermChrY;
	nSaveAttr   = cTermAttr;
	bSaveOrigin = bTermOrigin;
	bTermEscape = FALSE;
	break;

      case '8':
	nTermChrX   = nSaveChrX;
	nTermChrY   = nSaveChrY;
	cTermAttr   = (char)nSaveAttr;
	bTermOrigin = bSaveOrigin;
	bTermEscape = FALSE;
	break;

      case '(':
      case ')':
      case '*':
      case '+':
	switch (cTermBuf[0])
	  {
          case ')':
	    i = 1;
	    break;

          case '*':
	    i = 2;
	    break;

          case '+':
	    i = 3;
	    break;

	  default:
	    i = 0;
	    break;
	  }

	switch (ch)
	  {
          case 'A':
	    nTermCS[i] = TERM_CS_BRITISH;
	    break;

          case '0':
	    nTermCS[i] = TERM_CS_DECGPH;
	    break;

	  default:
	    nTermCS[i] = TERM_CS_ASCII;
	    break;
	  }

	bTermEscape = FALSE;
	break;

      case '#':
	switch (ch)
	  {
          case '3':
          case '4':
          case '5':
          case '6':
            VT102CharDbl(ch - '0');
	    break;
	  // dca00034 MKL added one parameter to VidClearPage routine
          case '8':	/* ^[#8 Screen Alignment Display */
	    VidClearPage(0x7045);
	    break;
	  }

	bTermEscape = FALSE;
	break;

      case '=':
	bTermAltKey = TRUE;
//	dca00015  MKL  If keyboard is already in app cursor mode, ignore ^[=.
//      bTermAltCur = FALSE;
//	SendMessage(hChildStatus, UM_KEYPAD, 1, (long)nTermMaxCol);

	if (bTermAltCur)
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//	  SendMessage(hChildStatus, UM_KEYPAD, 3, (long)nTermMaxCol);
	  SendMessage(hChildStatus, UM_KEYPAD, 4, (long)nTermMaxCol);
	else
	  SendMessage(hChildStatus, UM_KEYPAD, 1, (long)nTermMaxCol);
	bTermEscape = FALSE;
	break;

      /* 0=Num, 1=Appl, 2=Cur, 3=Lock, 4=ACur */
      case '>':
	bTermAltKey = FALSE;
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//	bTermAltCur = FALSE;
//	SendMessage(hChildStatus, UM_KEYPAD, 0, (long)nTermMaxCol);
   if (bTermAltCur)
	   SendMessage(hChildStatus, UM_KEYPAD, 2, (long)nTermMaxCol);
	else
      SendMessage(hChildStatus, UM_KEYPAD, 0, (long)nTermMaxCol);
	bTermEscape = FALSE;
	break;

      case '<':
      case '\\':
	bTermEscape = FALSE;
	break;

      case '[':
	switch (ch)
	  {
          case '?':
	    bTermQFlg = TRUE;
	    break;

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

	    if (nTermChrY < nTermMarginTop)
	      nTermChrY = nTermMarginTop;
	    bTermEscape = FALSE;
	    break;

          case 'B':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    nTermChrY += i;

	    if (nTermChrY > nTermMarginBot)
	      nTermChrY = nTermMarginBot;
	    bTermEscape = FALSE;
	    break;

          case 'C':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    nTermChrX += i;

	    if (nTermChrX > nTermMaxCol)
	      nTermChrX = nTermMaxCol;
	    bTermEscape = FALSE;
	    break;

          case 'D':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

	    nTermChrX -= i;

	    if (nTermChrX < 1)
	      nTermChrX = 1;
	    bTermEscape = FALSE;
	    break;

          // 00032 Add support for ESC [ n G to move cursor to column n.
          case 'G':
	    if ((i = cTermBuf[1]) == 0)
	      i = 1;

            nTermChrX = i;

	    if (nTermChrX < 1)
	      nTermChrX = 1;
	    else if (nTermChrX > TERM_MAXCOL)
	      nTermChrX = TERM_MAXCOL;

	    bTermEscape = FALSE;
	    break;

          case 'J':
	    switch (cTermBuf[1])
	      {
	      case 0:
		// dca00034 MKL added one parameter to VidClearPage
		if (nTermChrX == 1 && nTermChrY == 0)
		  VidClearPage(0x7020);
		else
		  VidClearArea(nTermChrX, nTermChrY,
                               nTermMaxCol, TERM_MAXROW-1);
		break;

	      case 1:
		VidClearArea(0, 0, nTermChrX, nTermChrY);
		break;

	      case 2:
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
		VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL, nTermChrY);
		break;

	      case 1:
		VidClearArea(0, nTermChrY, nTermChrX, nTermChrY);
		break;

	      case 2:
		VidClearArea(0, nTermChrY, nTermMaxCol, nTermChrY);
		break;
	     }

	    bTermEscape = FALSE;
	    break;

          case 'L':
	    if (nTermChrY >= nTermMarginTop && nTermChrY <= nTermMarginBot)
	      {
	      if (cTermBuf[1] == 0)
		cTermBuf[1] = 1;
	      else if (cTermBuf[1] > TERM_MAXROW)
                cTermBuf[1] = TERM_MAXROW-1;

	      while (cTermBuf[1]-- > 0)
		VidScrollDown(nTermChrY, nTermMarginBot);
	      }

	    bTermEscape = FALSE;
	    break;

          case 'M':
	    if (nTermChrY >= nTermMarginTop && nTermChrY <= nTermMarginBot)
	      {
	      if (cTermBuf[1] == 0)
		cTermBuf[1] = 1;
              else if (cTermBuf[1] >= TERM_MAXROW)
                cTermBuf[1] = TERM_MAXROW-1;

	      while (cTermBuf[1]-- > 0)
		VidScrollUp(nTermChrY, nTermMarginBot);
	      }

	    bTermEscape = FALSE;
	    break;

          case 'H':
          case 'f':
	    if (bTermCntlPrint)
	      {
	      bTermCntlPrint = FALSE;
	      SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 0L);
	      ClosePrinter();
	      }
	    if (bTermOrigin)
	      {
	      nTermChrY = cTermBuf[1] - 1 + nTermMarginTop;

	      if (nTermChrY < nTermMarginTop)
		nTermChrY = nTermMarginTop;

	      if (nTermChrY > nTermMarginBot)
		nTermChrY = nTermMarginBot;
	      }
	    else
	      {
	      if ((nTermChrY = cTermBuf[1] - 1) < 0)
		nTermChrY = 0;
              else if (nTermChrY >= TERM_MAXROW)
                nTermChrY = TERM_MAXROW-1;
	      }

	    if ((nTermChrX = cTermBuf[2]) < 1)
	      nTermChrX = 1;
	    else if (nTermChrX > TERM_MAXCOL)
	      nTermChrX = TERM_MAXCOL;

	    bTermEscape = FALSE;
	    break;

          case 'c':
            // 00080 Only return a value is argument is 0.
	    // dca00030 MKL added VT102 device attribute capability
            if (cTermBuf[1] == 0)
              {
              buf[0] = ASC_ESC;
              buf[1] = '[';
              buf[2] = '?';
              buf[3] = '6';
              buf[4] = 'c';
              CommDataOutput(buf, 5);
              }

	    bTermEscape = FALSE;
	    break;

          case 'g':
	    switch (cTermBuf[1])
	      {
	      case 0:
		bTermTabs[nTermChrX-1] = FALSE;
		break;

	      case 3:
		memset(bTermTabs, FALSE, sizeof(bTermTabs));
                // 00090 Check for zero TabWidth.
                if (Term.cTabWidth)
                  {
                  for (i = Term.cTabWidth; i < sizeof(bTermTabs); i += Term.cTabWidth)
                    bTermTabs[i] = TRUE;
                  }
		break;
	       }

	    bTermEscape = FALSE;
	    break;

          case 'h':
	    if (bTermQFlg)
	      {
	      switch (cTermBuf[1])
		{
		// dca00013 MKL Allow ^[[?1h to switch user into application mode
		case 1:
//		  if (bTermAltKey)
//		    {
//		    bTermAltCur = TRUE;
//		    SendMessage(hChildStatus, UM_KEYPAD, 2, (long)nTermMaxCol);
//		    }

// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//		  bTermAltKey = TRUE;
		  bTermAltCur = TRUE;
		  if (bTermAltKey)
			 SendMessage(hChildStatus, UM_KEYPAD, 4, (long)nTermMaxCol);
		  else
			 SendMessage(hChildStatus, UM_KEYPAD, 2, (long)nTermMaxCol);
		  break;

		case 3:
		  nTermMaxCol = 132;
		  SendMessage(hChildStatus, UM_KEYPAD, -1, (long)nTermMaxCol);
		  VidUpdateSize();
		  // dca00034 MKL added one parameter to VidClearPage
		  VidClearPage(0x7020);
		  break;

		case 5:
		  bTermRevr = TRUE;
		  InvalidateRect(hChildTerminal, NULL, TRUE);
		  UpdateWindow(hChildTerminal);
		  break;

		case 6:
		  nTermChrX = 1;
		  nTermChrY = nTermMarginTop;
		  bTermOrigin = TRUE;
		  break;

		case 7:
		  bTermWrap = TRUE;
		  break;

		// dca00033 MKL added Auto keyborad repeat ON/OFF capability
		case 8:		/* ^[[?8h Auto Repeat On */
		  bTermNoAutoRepeat = FALSE;
		  break;

		case 18:
		  bTermFeedPrint = TRUE;
		  break;

		case 19:
		  bTermModePrint = TRUE;
		  break;

		// dca00035 MKL added cursor enable/disable capability
		case 25:	/* ^[[?25h Cursor(Caret) Enable */
		  if (bCaretDisabled) {
		     ShowCaret(hChildTerminal);
		     bCaretDisabled = FALSE;
		  }
		  break;
		}
	      }
	    else
	      {
	      switch (cTermBuf[1])
		{
		case 2:	/* ^[[2h lock keyboard */
		  bTermLckKey = TRUE;
		  SendMessage(hChildStatus, UM_KEYPAD, 3, (long)nTermMaxCol);
		  break;

		case 4:
		  bTermInsert = TRUE;
		  break;

		case 12:
		  bTermEcho = FALSE;
		  break;

		case 20:	/* ^[[20h  Set LineFeed Mode */
		  bTermLFCR = TRUE;
		  break;
		}
	      }

	    bTermEscape = FALSE;
	    break;

          case 'l':
	    if (bTermQFlg)
	      {
	      switch (cTermBuf[1])
		{
		case 1:
		  bTermAltCur = FALSE;
// dca00022 PJL/MKL Fix Vt100 cursor/keypad emulation.
//       SendMessage(hChildStatus, UM_KEYPAD, 1, (long)nTermMaxCol);
		  if (bTermAltKey)
             SendMessage(hChildStatus, UM_KEYPAD, 1, (long)nTermMaxCol);
         else
		      SendMessage(hChildStatus, UM_KEYPAD, 0, (long)nTermMaxCol);
		  break;

		case 2:
		  fTermVT52Mode = TRUE;
		  SendMessage(hChildStatus, UM_EMULATOR, 0, 2L);
		  break;

		case 3:
		  nTermMaxCol = 80;
		  SendMessage(hChildStatus, UM_KEYPAD, -1, (long)nTermMaxCol);
		  VidUpdateSize();
		  // dca00034 MKL added one parameter to VidClearPage
		  VidClearPage(0x7020);
		  break;

		case 5:
		  bTermRevr = FALSE;
		  InvalidateRect(hChildTerminal, NULL, TRUE);
		  UpdateWindow(hChildTerminal);
		  break;

		case 6:
		  nTermChrX = 1;
		  nTermChrY = 0;
		  bTermOrigin = FALSE;
		  break;

		case 7:
		  bTermWrap = FALSE;
		  break;

		// dca00033 MKL added Auto keyborad repeat ON/OFF capability
		case 8:		/* ^[[?8l Auto Repeat Off */
		  bTermNoAutoRepeat = TRUE;
		  break;

		case 18:
		  bTermFeedPrint = FALSE;
		  break;

		case 19:
		  bTermModePrint = FALSE;
		  break;
		// dca00035 MKL added cursor enable/disable capability
		case 25:	/* ^[[?25l Cursor(Caret) Disable */
		  if (!bCaretDisabled) {
		     HideCaret(hChildTerminal);
		     bCaretDisabled = TRUE;
		  }
		  break;
		}
	      }
	    else
	      {
	      switch (cTermBuf[1])
		{
		// dca00032 MKL fixed keyboard lock unlock problem
		case 2:	/* ^[[2l unlock keyboard */
		  bTermLckKey = FALSE;
		  //if (!bTermAltKey)
		  //  i = 0;
		  //else if (bTermAltKey && !bTermAltCur)
		  //  i = 1;
		  //else
		  //  i = 2;

		  /* 0=Num, 1=Appl, 2=Cur, 3=Lock, 4=ACur */
		  if (bTermAltKey && bTermAltCur) i = 4;
		  else if (bTermAltKey) i = 1;
		  else if (bTermAltCur) i = 2;
		  else i = 0;
		  SendMessage(hChildStatus, UM_KEYPAD, i, (long)nTermMaxCol);
		  break;

		case 4:
		  bTermInsert = FALSE;
		  break;

		case 12:
		  bTermEcho = TRUE;
		  break;

		case 20:
		  bTermLFCR = FALSE;
		  break;
		}
	      }
	    bTermEscape = FALSE;
	    break;

          case 'i':
	    if (bTermQFlg)
	      {
	      switch (cTermBuf[1])
		{
		case 1:
		  TermLinePrint(nTermChrY);
		  break;

		case 4:
		  if (bTermAutoPrint)
		    {
		    bTermAutoPrint = FALSE;
		    SendMessage(hChildInfo, UM_SCRNMODE, INFO_AUTO, 0L);
		    ClosePrinter();
		    }
		  break;

		case 5:
		  if (!bTermAutoPrint)
		    {
		    bTermAutoPrint = TRUE;
		    SendMessage(hChildInfo, UM_SCRNMODE, INFO_AUTO, 1L);
		    OpenPrinter();
		    }
		  break;
		}
	      }
	    else
	      {
	      switch (cTermBuf[1])
		{
		case 0:		/* ^[[0i */
		  TermScreenPrint();
		  break;

		case 4:
		  if (bTermCntlPrint)
		    {
		    bTermCntlPrint = FALSE;
		    SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 0L);
		    ClosePrinter();
		    }
		  break;

		case 5:
		  if (!bTermCntlPrint)
		    {
		    bTermCntlPrint = TRUE;
		    SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 1L);
		    OpenPrinter();
		    }
		  break;
		}
	      }

	    bTermEscape = FALSE;
	    break;

	  // 00067 Add support for ESC [ 27 m
	  case 'm':
	    for (i = 1; i <= nTermInx; i++)
	      {
	      switch (cTermBuf[i])
		{
		case 0:
		  cTermAttr  = TERM_ATTR_WHITE << 4;
		  // MKL added ^[[0m clear ^[#6 attribute function
		  VT102CharDbl(5);
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

		case 27:
		  cTermAttr = (cTermAttr & 0x88) | (TERM_ATTR_WHITE << 4);
		  break;

		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		  if (!(Term.fNoColor || fMonoDisplay))
		    cTermAttr = (cTermAttr & 0x8F) + (cTermBuf[i] - 30 << 4);
		  else
		    cTermAttr = (cTermAttr & 0x88) | (TERM_ATTR_WHITE << 4);
		  break;

		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		  if (!(Term.fNoColor || fMonoDisplay))
		    cTermAttr = (cTermAttr & 0xF8) + cTermBuf[i] - 40;
		  else
		    cTermAttr = (cTermAttr & 0x88) | TERM_ATTR_WHITE;
		  break;
		}
	      }

	    bTermEscape = FALSE;
	    break;

          case 'n':
	    if (bTermQFlg)
	      {
	      switch (cTermBuf[1])
		{
		case 15:
		  buf[0] = ASC_ESC;
                  buf[1] = '[';
                  buf[2] = '1';
                  buf[3] = '0';
                  buf[4] = 'n';
		  CommDataOutput(buf, 5);
		  break;
		}
	      }
	    else
	      {
	      switch (cTermBuf[1])
		{
		case 5:
		  buf[0] = ASC_ESC;
                  buf[1] = '[';
                  buf[2] = '0';
                  buf[3] = 'n';
		  CommDataOutput(buf, 4);
		  break;

		case 6:
                  sprintf(buf,"\033[%d;%dR", nTermChrY + 1, nTermChrX);
		  CommDataOutput(buf, strlen(buf));
		  break;
		}
	      }

	    bTermEscape = FALSE;
	    break;

          case 'q':
	    switch (cTermBuf[1])
	      {
	      case 0: /* ^[[0q */
		cTermLEDs = 0;
		// dca00031 MKL added VT102 Load LED cap, added "L1" indicator
		//	  to infomation bar
		SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);
		break;

	      case 1: /* ^[[1q */
		cTermLEDs |= 0x01;
		// dca00031 MKL
		SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);
		break;

	      case 2:
		cTermLEDs |= 0x02;
		break;

	      case 3:
		cTermLEDs |= 0x04;
		break;

	      case 4:
		cTermLEDs |= 0x08;
		break;
	      }

	    bTermEscape = FALSE;
	    break;

          case 'r':
	    if (cTermBuf[1] == 0)
	      nTermMarginTop = 0;
	    else
	      nTermMarginTop = cTermBuf[1] - 1;

	    if (cTermBuf[2] == 0)
	      nTermMarginBot = TERM_MAXROW - 1;
	    else
	      nTermMarginBot = cTermBuf[2] - 1;

	    if (nTermMarginTop < 0)
	      nTermMarginTop = 0;

	    if (nTermMarginBot > TERM_MAXROW - 1)
	      nTermMarginBot = TERM_MAXROW - 1;

//	dca00014   MKL  change scolling region limit from 2 lines to 1
//	    if (nTermMarginTop + 2 > nTermMarginBot)
	    if (nTermMarginTop + 1 > nTermMarginBot)
	      {
	      nTermMarginTop = 0;
	      nTermMarginBot = TERM_MAXROW - 1;
	      }

            nTermChrX = 1;
            if (bTermOrigin)
              nTermChrY = nTermMarginTop;
            else
              nTermChrY = 0;

	    bTermEscape = FALSE;
	    break;

          case 'y':
	    bTermEscape = FALSE;
	    break;

          case 'P':
	    if (cTermBuf[1] <= 0)
	      cTermBuf[1] = 1;
	    else if (cTermBuf[1] > TERM_MAXROW - 1)
	      cTermBuf[1] = TERM_MAXROW - 1;

	    VidDeleteChars(cTermBuf[1]);
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


static int near TermCharXlat(ch)
  WORD ch;
{
  ch &= 0xFF;

  switch (nTermCS[nTermCSet])
    {
    case TERM_CS_ASCII:
      break;

    case TERM_CS_DECGPH:
      if (!Sess.fGraphics)
	break;
      if (fVT102Font && bCurFontCharSet != OEM_CHARSET)
	{
	if (fTermVT52Mode)
	  {
	  if (ch > 95)
	    ch += 96;
	  }
	else
	  {
	  if (ch > 95)
	    ch += 128;
	  }
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


void near TermReset()
{
  int i;


  nTermChrX = 1;
  nTermChrY = 0;
  bTermEscape = FALSE;

  bTermEcho = Term.fEcho;


  /***** FONT CODE *****/
  SelectObject(hTerminalDC, hVT102Fonts[0]);
  cTermAttr   = TERM_ATTR_WHITE << 4;
  nSaveAttr   = cTermAttr;
  cTermFontAttr  = -1;
  cTermFontColor = -1;
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

  bTermLFCR = Term.fLfAuto;

  nTermMarginTop = 0;
  nTermMarginBot = TERM_MAXROW - 1;

  memset(bTermTabs, FALSE, sizeof(bTermTabs));

  // 00090 Check for zero TabWidth.
  if (Term.cTabWidth)
    {
    for (i = Term.cTabWidth; i < TERM_MAXCOL; i += Term.cTabWidth)
      bTermTabs[i] = TRUE;
    }

  nTermMaxCol = Term.nColumns;

  bTermInsert	 = FALSE;
  bTermAltKey	 = FALSE;
  bTermAltCur	 = FALSE;
  bTermLckKey	 = FALSE;
  // dca00033 MKL added Auto keyborad repeat ON/OFF capability
  bTermNoAutoRepeat = FALSE;

  bCaretDisabled = FALSE;

  SendMessage(hChildStatus, UM_KEYPAD, 0, (long)nTermMaxCol);
  cTermLEDs	 = 0;
  SendMessage(hChildStatus, UM_LIGHTS, 0, 0L);

  fTermVT52Mode   = FALSE;
  fTermGraphicsOn = FALSE;
  SendMessage(hChildStatus, UM_EMULATOR, 0, 1L);

  if (bTermAutoPrint)
    {
    bTermAutoPrint = FALSE;
    SendMessage(hChildInfo, UM_SCRNMODE, INFO_AUTO, 0L);
    ClosePrinter();
    }
  if (bTermCntlPrint)
    {
    bTermCntlPrint = FALSE;
    SendMessage(hChildInfo, UM_SCRNMODE, INFO_CONTROL, 0L);
    ClosePrinter();
    }

  bTermModePrint = Term.fPrintExtent;
  bTermFeedPrint = FALSE;

  VidUpdateSize();
  // dca00034 MKL added one parameter to VidClearPage
  VidClearPage(0x7020);
}

void TermSetup()
{
  TermReset();
}


static void near TermLinePrint(nLine)
  int  nLine;
{
  PINT	pInt;
  char	buf[133];
  int	i;


  pInt =  pPriVidLine[nLine] + 1;

  for (i = 0; i < nTermMaxCol; i++)
    buf[i] = (char)(*pInt++);
  buf[nTermMaxCol] = NULL;

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
    nBot = TERM_MAXROW - 1;
    }

  for (i = nTop; i <= nBot; i++)
    TermLinePrint(i);

  if (bTermFeedPrint)
    FlushPrinter();
}
