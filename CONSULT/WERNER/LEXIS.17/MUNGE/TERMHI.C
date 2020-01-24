/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Terminal Emulation, High Use (TERMHI.C)                          */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This terminal emulation module performs keyboard and other misc  */
/*           support.                                                         */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include "lexis.h"
#include "library.h"
#include "term.h"
#include "comm.h"
#include "data.h"
#include "session.h"
#include "dialogs.h"


  HDC	 hTerminalDC;
  char	 fTermFocusActive = FALSE;
  char	 fTermCaretActive = FALSE;

  int	 nTermChrX;		/* Current Character X cursor position	   */
  int	 nTermChrY;		/*		     Y cursor position	   */
  char	 cTermAttr;		/*		     Attributes 	   */
  int	 nTermCSet;		/* Current Screen Display Character Set    */
  BOOL	 bTermWrap;		/*			  Line Wrap Mode   */
  BOOL	 bTermOrigin;		/*			  Origin Setting   */
  int	 nTermErase;		/*			  Selective Erase  */

  int	 nSaveChrX;		/* Saved Character X cursor position	   */
  int	 nSaveChrY;		/*		   Y cursor position	   */
  int	 nSaveAttr;		/*		   Attributes		   */
  int	 nSaveCSet;		/* Saved Screen Display Character Set	   */
  BOOL	 bSaveWrap;		/*			Line Wrap Mode	   */
  BOOL	 bSaveOrigin;		/*			Origin Setting	   */
  int	 nSaveErase;		/*			Selective Erase    */

  int	 nTermMarginTop;
  int	 nTermMarginBot;

  BOOL	 bTermLFCR;
  BOOL	 bTermEcho;
  BOOL	 bTermQFlg;
  BOOL	 bTermPFlg;
  BOOL	 bTermRevr;
  BOOL	 bTermInsert;
  BOOL	 bTermAltKey;
  BOOL	 bTermAltCur;
  BOOL	 bTermLckKey;
  char	 cTermLEDs;
  char	 fTermVT52Mode;
  char	 fTermGraphicsOn;

  BOOL	 bTermAutoPrint;	/* Auto Print Switch			   */
  BOOL	 bTermCntlPrint;	/* Media Copy (Printer Controller On)	   */
  BOOL	 bTermModePrint;	/* Print full screen / print margin area   */
  BOOL	 bTermFeedPrint;	/* Add Form Feed to end of print screen    */


  int	 nTermCS[4];		/* Character sets designation	       */

  char   cTermFontAttr;         /* Last Character's Attributes         */
  char   cTermFontColor;        /* Last Character's Color              */

  int	 nTermOffX;		/* Screen scroll X character offset    */
  int	 nTermOffY;		/* Screen scroll Y character offset    */
  int	 nTermExtX;		/* Screen scroll X character extent    */
  int	 nTermExtY;		/* Screen scroll Y character extent    */
  int	 nTermWndX;
  int	 nTermWndY;
  char	 fTermPause = FALSE;


  char	 bTermTabs[TERM_MAXCOL];

  BOOL	 bTermEscape;		/* Escape processing mode */

  BYTE	 cTermBuf[16];
  int	 nTermInx;

  RECT	 TermClient;

  long	 ChrAttrColor[] =
	   {
	   RGB(  0,  0,  0),
	   RGB(255,  0,  0),
	   RGB(  0,255,  0),
	   RGB(255,255,  0),
	   RGB(  0,  0,255),
	   RGB(255,  0,255),
	   RGB(  0,255,255),
	   RGB(255,255,255)
	   };

/*---------------------------------------------------------------------------*/
/* Define Keyboard Lockout Variable and Routines			     */
/*---------------------------------------------------------------------------*/
  char	 fCapsLock;
  char	 fNumsLock;
  char	 fPauseOn;

  HWND hChildTerminal = NULL;


/*---------------------------------------------------------------------------*/
/* Define Mouse Character Select Varibles and Routines			     */
/*---------------------------------------------------------------------------*/
  char fMouseDown;
  char fSelectedArea;
  int  nSelectX1, nSelectY1;
  int  nSelectX2, nSelectY2;


/*---------------------------------------------------------------------------*/
/* Define Variables used for Keyboard identifier save logic		     */
/*---------------------------------------------------------------------------*/
  char sTermFileName[16];
  char sTermWorkName[16];
  int  nTermWorkInx;

  struct stTerm  Term;


static void near pascal GetSelectedLine(int, PSTR, int, int, int, int, char);


void CmdScreen()
{

}


BOOL TerminalVirtualKey(LPMSG msg)
  {
  char szBuf[64];


  if (msg->message != WM_KEYDOWN)
    return (FALSE);

  if (sActiveMacro)
    {
    switch (msg->wParam)
      {
      case VK_UP:
        if (!fSessOnline)
          return (TRUE);
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        sActiveButton -= 7;
        if (sActiveButton < 0)
          sActiveButton += 21;
        SendMessage(hChildMacro, UM_BUTTON, TRUE, 0L);
        break;

      case VK_DOWN:
        if (!fSessOnline)
          return (TRUE);
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        sActiveButton += 7;
        if (sActiveButton > 20)
          sActiveButton -= 21;
        SendMessage(hChildMacro, UM_BUTTON, TRUE, 0L);
        break;

      case VK_LEFT:
        if (!fSessOnline)
          return (TRUE);
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        if (sActiveButton-- == 0)
          sActiveButton = 20;
        SendMessage(hChildMacro, UM_BUTTON, TRUE, 0L);
        break;

      case VK_RIGHT:
        if (!fSessOnline)
          return (TRUE);
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        if (sActiveButton++ == 20)
          sActiveButton = 0;
        SendMessage(hChildMacro, UM_BUTTON, TRUE, 0L);
        break;

      case VK_ADD:
      case VK_TAB:
        sActiveMacro = FALSE;
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        break;

      case VK_RETURN:
        SendMessage(hChildMacro, UM_EXECUTE, 0, 0L);
        sActiveMacro = FALSE;
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        break;

      default:
        return (FALSE);
      }

    return (TRUE);
    }

  switch (msg->wParam)
    {
    case VK_UP:
      if (GetKeyState(VK_SHIFT) < 0)
        {
        MouseSelectMsg(hChildTerminal, WM_KEYDOWN, msg->wParam, 0L);
        return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_VSCROLL, SB_LINEUP, 0L);
	return (TRUE);
        }
      else if (fSelectedArea)
        {
      //nTermChrX = nSelectX2;
      //nTermChrY = nSelectY2;
        MouseSelectClr();
        }
      break;

    case VK_RIGHT:
      if (GetKeyState(VK_SHIFT) < 0)
        {
        MouseSelectMsg(hChildTerminal, WM_KEYDOWN, msg->wParam, 0L);
        return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_HSCROLL, SB_LINEDOWN, 0L);
	return (TRUE);
	}
      else if (fSelectedArea)
        {
      //nTermChrX = nSelectX2;
      //nTermChrY = nSelectY2;
        MouseSelectClr();
        }
      break;

    case VK_LEFT:
      if (GetKeyState(VK_SHIFT) < 0)
        {
        MouseSelectMsg(hChildTerminal, WM_KEYDOWN, msg->wParam, 0L);
        return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_HSCROLL, SB_LINEUP, 0L);
	return (TRUE);
	}
      else if (fSelectedArea)
        {
      //nTermChrX = nSelectX2;
      //nTermChrY = nSelectY2;
        MouseSelectClr();
        }
      break;

    case VK_DOWN:
      if (GetKeyState(VK_SHIFT) < 0)
        {
        MouseSelectMsg(hChildTerminal, WM_KEYDOWN, msg->wParam, 0L);
        return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_VSCROLL, SB_LINEDOWN, 0L);
	return (TRUE);
	}
      else if (fSelectedArea)
        {
      //nTermChrX = nSelectX2;
      //nTermChrY = nSelectY2;
        MouseSelectClr();
        }
      break;

    case 187:	 /* = */
      if (GetKeyState(VK_CONTROL) < 0)
	{
	SendMessage(hChildTerminal, UM_RESET, 0, 0L);
	return TRUE;
	}
      break;

    case VK_ADD:
    case VK_TAB:
      if (!fDataDisplayMacro)
        {
        sActiveMacro = TRUE;
        if (!fSessOnline)
          sActiveButton = 15;
        SendMessage(hChildMacro, UM_BUTTON, TRUE, 0L);
        return (TRUE);
        }
      break;

    case VK_NUMLOCK:
    case VK_CAPITAL:
      SendMessage(hChildInfo, WM_TIMER, 0, 0L);
      return FALSE;
      break;
    }

  TermWriteKeyboard(msg->message, msg->wParam);
  return (FALSE);
  }


void MouseSelectMsg(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  RECT Rect;
  PINT pLine;
  int  nHigh, nLow;
  int  x,y;


  nHigh = (int)HIWORD(lParam);
  nLow	= (int)LOWORD(lParam);

  switch (message)
    {
    case WM_MOUSEMOVE:
      if (!fMouseDown)
	return;

      GetClientRect(hWnd, (LPRECT)&Rect);

      y = nHigh / nCurFontHeight;
      if (y < 0)
        {
        x = 1;
        y = 0;
        }
      y += nTermOffY;

      if (nHigh < Rect.top)
        x = 1;
      else if (nHigh > Rect.bottom)
        x = TERM_MAXCOL;
      else
        {
        x = nLow / nCurFontWidth + nTermOffX + 1;
        if (x < 1) x = 1;
        if (x > TERM_MAXCOL) x = TERM_MAXCOL;
        }

      if (y >= TERM_MAXROW)
        {
        x = TERM_MAXCOL;
        y = TERM_MAXROW - 1;
        }

      if (nLow < Rect.left)
        VidScroll(WM_HSCROLL, SB_LINEUP, 0L);
      else if (nLow > Rect.right)
        VidScroll(WM_HSCROLL, SB_LINEDOWN, 0L);

      if (nHigh < Rect.top)
        VidScroll(WM_VSCROLL, SB_LINEUP, 0L);
      else if (nHigh > Rect.bottom)
        VidScroll(WM_VSCROLL, SB_LINEDOWN, 0L);
      break;

    case WM_KEYDOWN:
      if (!fSelectedArea)
        {
        nSelectX1 = nSelectX2 = nTermChrX;
        nSelectY1 = nSelectY2 = nTermChrY;
        }
      switch (wParam)
        {
        case VK_UP:
          x = nSelectX2;
          if (y = nSelectY2)
            y--;
          break;

        case VK_DOWN:
          x = nSelectX2;
          if ((y = nSelectY2) < 23)
            y++;
          break;

        case VK_RIGHT:
          if ((x = nSelectX2) < 80)
            x++;
          y = nSelectY2;
          break;

        case VK_LEFT:
          if ((x = nSelectX2) > 1)
            x--;
          y = nSelectY2;
          break;
        }
      break;

    case WM_LBUTTONDOWN:
      if (fMouseDown)
	return;

      MouseSelectClr();

      y = nHigh / nCurFontHeight + nTermOffY;
      x = nLow / nCurFontWidth + nTermOffX + 1;

      if (x < 1) x = 1;
      if (x > TERM_MAXCOL) x = TERM_MAXCOL;
      if (y >= TERM_MAXROW - 1) y = TERM_MAXROW - 1;

      nSelectX2 = nSelectX1 = x;
      nSelectY2 = nSelectY1 = y;

      fMouseDown = TRUE;
      SetCapture(hWnd);

      VidClickAndPick(x, y, FALSE);
      if (nSelectX1 != nSelectX2 || nSelectY1 != nSelectY2)
        fSelectedArea = TRUE;
      return;
      break;

    case WM_LBUTTONUP:
      if (!fMouseDown)
	return;
      fMouseDown = FALSE;
      ReleaseCapture();
      return;

    default:
      return;
    }

  if (!fSelectedArea)
    {
    fSelectedArea = TRUE;
    VidSelectArea(nSelectX1, nSelectY1, x, y);
    }
  else
    {
    int n1, n2, n3;

    if (nSelectX2 == x && nSelectY2 == y)
      return;

    n1 = nSelectY1 * TERM_MAXCOL + nSelectX1;
    n2 = nSelectY2 * TERM_MAXCOL + nSelectX2;
    n3 = y * TERM_MAXCOL + x;

    if (n2 == n1)
      {
      if (n3 < n2)
	VidSelectArea(nSelectX2-1, nSelectY2, x, y);
      else
	VidSelectArea(nSelectX2+1, nSelectY2, x, y);
      }
    else if (n2 < n1)
      {
      if (n3 > n1)
	{
	VidSelectArea(nSelectX2, nSelectY2, nSelectX1, nSelectY1);
	VidSelectArea(nSelectX1, nSelectY1, x, y);
	}
      else if (n3 < n2)
	VidSelectArea(nSelectX2-1, nSelectY2, x, y);
      else
	VidSelectArea(nSelectX2, nSelectY2, x-1, y);
      }
    else
      {
      if (n3 < n1)
	{
	VidSelectArea(nSelectX2, nSelectY2, nSelectX1, nSelectY1);
	VidSelectArea(nSelectX1, nSelectY1, x, y);
	}
      else if (n3 > n2)
	VidSelectArea(nSelectX2+1, nSelectY2, x, y);
      else
	VidSelectArea(nSelectX2, nSelectY2, x+1, y);
      }
    }

  nTermChrX = nSelectX2 = x;
  nTermChrY = nSelectY2 = y;
  VidSetCaretPos();
  }


void MouseSelectClr()
  {
  if (fSelectedArea)
    {
    VidSelectArea(nSelectX1, nSelectY1, nSelectX2, nSelectY2);
    fSelectedArea = FALSE;
    }
  }


void LineToTerminal(line)
  PSTR line;
{
  char buf[4];
  register int i;


  TermWriteData(line, strlen(line), FALSE);

  i = 0;
  buf[i++] = ASC_CR;
  if (!bTermLFCR)
    buf[i++] = ASC_LF;

  TermWriteData(buf, i, FALSE);
}

void CharToTerminal(key)
  int key;
{
  char buf[4];

  buf[0] = (char)key;
  TermWriteData(buf, 1, FALSE);
}
