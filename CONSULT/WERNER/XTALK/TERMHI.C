/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Terminal Support Module (TERMHI.C)                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contain support routines that connects us to the    *
*             rest of the program.                                            *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00039 Improve Scrollback buffer support.          *
*    1.1  03/01/90 dca00032 MKL fixed keyboard lock unlock problem            *
*    1.1  03/01/90 dca00033 MKL added Auto keyborad repeat ON/OFF capability  *
*    1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage	      *
*			    routine to support VT102 screen alignment cap.    *
*    1.1  03/01/90 dca00035 MKL added VT102 cursor enable/disable capability  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "comm.h"
#include "disp.h"
#include "prodata.h"
#include "properf.h"
#include "dialogs.h"
#include "interp.h"
#include "global.h"
#include "variable.h"
#include "kermit.h"
#include "macro.h"

extern void ToggleNotes();


  HDC	 hTerminalDC;
  char	 fTermFocusActive = FALSE;
  char	 fTermCaretActive = FALSE;

  WORD	 wCaretBlinkTime;

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

  // dca00033 MKL added Auto keyborad repeat ON/OFF capability
  BOOL   bTermNoAutoRepeat;	/* Keyboard Auto Repeat Switch */
  // dca00035 MKL added VT102 cursor enable/disable capability
  BOOL	 bCaretDisabled;

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

  int	 nTermMaxCol;		/* Width of logical screen, 80 or 132  */

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
  int  nCurEmulator;

/*---------------------------------------------------------------------------*/
/* Define Mouse Character Select Varibles and Routines			     */
/*---------------------------------------------------------------------------*/
  char fMouseDown;
  char fSelectedArea;
  int  nSelectX1, nSelectY1;
  int  nSelectX2, nSelectY2;
  int  nMouseAtX, nMouseAtY;

  char fMouseCursor;
  int  nCursorToX, nCursorToY;
  int  nCursorAtX, nCursorAtY;

/*---------------------------------------------------------------------------*/
/* Define Variables used for Keyboard identifier save logic		     */
/*---------------------------------------------------------------------------*/
  char sTermFileName[16];
  char sTermWorkName[16];
  int  nTermWorkInx;

/*---------------------------------------------------------------------------*/
/* Define Variable to support Zmodem					     */
/*---------------------------------------------------------------------------*/
  char cZmodemSeq;

  union TERMINAL terminal;
  struct stTerm  Term;


static void near GetSelectedLine(int, PSTR, int, int, int, int, char);
static void near AppendToNotes(char *, int);


void CmdScreen()
{

}

void TermWriteData(char *pBuf, int cBuf)
  {
  SendMessage(hChildTerminal, UM_DATA, cBuf, (DWORD)((LPSTR)pBuf));
  }

void TermWriteByte(int sChar)
  {
  BYTE buf[1];

  buf[0] = (BYTE)sChar;
  SendMessage(hChildTerminal, UM_DATA, 1, (DWORD)((LPSTR)buf));
  }

void TermWriteNewLine()
  {
  char buf[2];
  register i;

  i = 0;
  buf[i++] = ASC_CR;
  if (!Term.fNewLine)
    buf[i++] = ASC_LF;

  SendMessage(hChildTerminal, UM_DATA, i, (DWORD)((LPSTR)buf));
  }


void TermWriteClear()
  {
  nTermChrX = 1;
  nTermChrY = 0;
  VidSetCaretPos();
  // dca00034 MKL added one parameter to VidClearPage  *
  VidClearPage(0x7020);
  }


BOOL TerminalVirtualKey(msg)
  LPMSG msg;
{
  char szBuf[64];
  // unused variable .........MKL
  // int  i;

  //if (msg->message != WM_KEYDOWN) return FALSE; 

  /* Any key cancels Mouse Cursor mode for Slow Converters */
  fMouseCursor = FALSE;

  if (IfSystemStatus(SS_SCRIPT) && !Disp.fShowInput)
    if (InterpreterKeyboard(msg->wParam, 0))
      return (TRUE);

  if ((msg->wParam >= 'A' && msg->wParam <= 'Z') ||
      msg->wParam == ' ' || msg->wParam == VK_RETURN)
    {
    if (IfSystemStatus(SS_SCROLL))
      VssScrollToggle();
    }

  // unused variable .........MKL
  //i = 0;
  switch (msg->wParam)
    {
    case VK_PRIOR:
      if (wSystem & SS_SCROLL)
        {
	VssScrollDown(0);
	return (TRUE);
        }
      break;

    case VK_NEXT:
      if (wSystem & SS_SCROLL)
        {
	VssScrollUp(0);
	return (TRUE);
        }
      break;

    case VK_UP:
      if (wSystem & SS_SCROLL)
        {
	VssScrollDown(1);
	return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_VSCROLL, SB_LINEUP, 0L);
	return (TRUE);
	}
      break;

    case VK_RIGHT:
      if (wSystem & SS_SCROLL)
        {
	VidScroll(WM_HSCROLL, SB_LINEDOWN, 0L);
	return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_HSCROLL, SB_LINEDOWN, 0L);
	return (TRUE);
	}
      break;

    case VK_LEFT:
      if (wSystem & SS_SCROLL)
        {
	VidScroll(WM_HSCROLL, SB_LINEUP, 0L);
	return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_HSCROLL, SB_LINEUP, 0L);
	return (TRUE);
	}
      break;

    case VK_DOWN:
      if (wSystem & SS_SCROLL)
        {
	VssScrollUp(1);
	return (TRUE);
        }
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	VidScroll(WM_VSCROLL, SB_LINEDOWN, 0L);
	return (TRUE);
	}
      break;

    case VK_HOME:
      if (wSystem & SS_SCROLL)
        {
	VssGotoTop();
	return (TRUE);
        }
      break;

    case VK_END:
      if (wSystem & SS_SCROLL)
        {
	VssGotoBot();
	return (TRUE);
        }
      break;

    case VK_ESCAPE:
      if (wSystem & SS_DIAL)
	{
	DialTerm();
	return TRUE;
	}
      if (ComStat.fXoffHold && !(wSystem & SS_SCROLL))
	{
        EscapeCommFunction(nCommID, SETXON);
	return TRUE;
	}
      break;

    case 187:	 /* = */
      if (GetKeyState(VK_CONTROL) < 0 && !(wSystem & SS_SCROLL))
	{
	SendMessage(hChildTerminal, UM_RESET, 0, 0L);
	return TRUE;
	}
      break;

    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
      if (GetKeyState(VK_CONTROL) < 0 && Macro.szMacro[msg->wParam - 'A' + 24][0] != '\0')
        {
        ActivateMacro(msg->wParam - 'A' + 24);
        return (TRUE);
        }
      //dca00032 MKL fixed keyboard lock unlock problem
      //return (FALSE);
      break;

    case 'R':
      if (GetKeyState(VK_CONTROL) < 0 && Macro.szMacro[40][0] != '\0')
        {
        ActivateMacro(40);
        return (TRUE);
        }
      //dca00032 MKL fixed keyboard lock unlock problem
      //return (FALSE);
      break;

    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      if (GetKeyState(VK_CONTROL) < 0 && Macro.szMacro[msg->wParam - 'T' + 41][0] != '\0')
        {
        ActivateMacro(msg->wParam - 'T' + 41);
        return (TRUE);
        }
      //dca00032 MKL fixed keyboard lock unlock problem
      //return (FALSE);
      break;

    case 'S':
      if (GetKeyState(VK_CONTROL) < 0 && hDemoFile != -1)
	{
	fDemoPause = TRUE;
	return TRUE;
	}
      break;

    case 'Q':
      if (GetKeyState(VK_CONTROL) < 0 && ComStat.fXoffHold &&
	  !(wSystem & SS_SCROLL))
	{
        EscapeCommFunction(nCommID, SETXON);
	return TRUE;
	}

      if (GetKeyState(VK_CONTROL) < 0 && hDemoFile != -1)
	{
	fDemoPause = FALSE;
	return TRUE;
	}
      break;

    case VK_PAUSE:
      if (fPauseOn)
	{
	fPauseOn = FALSE;
	szBuf[0] = ASC_XOFF;
	}
      else
	{
	fPauseOn = TRUE;
	szBuf[0] = ASC_XON;
	}

      CommDataOutput(szBuf, 1);
      break;

    case VK_F1:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(0);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(12);
	return TRUE;
	}
      break;

    case VK_F2:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(1);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(13);
	return TRUE;
	}
      break;

    case VK_F3:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(2);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(14);
	return TRUE;
	}
      break;

    case VK_F4:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(3);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(15);
	return TRUE;
	}
      break;

    case VK_F5:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(4);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(16);
	return TRUE;
	}
      break;

    case VK_F6:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(5);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(17);
	return TRUE;
	}
      break;

    case VK_F7:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(6);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(18);
	return TRUE;
	}
      else
	{
	ToggleNotes();
	return TRUE;
	}
      break;

    case VK_F8:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(7);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(19);
	return TRUE;
	}
      break;

    case VK_F9:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(8);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(20);
	return TRUE;
	}
      else
	{
	if (Disp.fShowInput)
	  Disp.fShowInput = FALSE;
	else
	  Disp.fShowInput = TRUE;
	if (Disp.fShowInput)
	  SetFocus(hChildInput);
	else
	  SetFocus(hChildTerminal);
	ResetScreenDisplay();
	return TRUE;
	}
      break;

    case VK_F10:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(9);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(21);
	return TRUE;
	}
      break;

    case VK_F11:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(10);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(22);
	return TRUE;
	}
      break;

    case VK_F12:
      if (GetKeyState(VK_SHIFT) < 0)
	{
	ActivateMacro(11);
	return TRUE;
	}
      else if (GetKeyState(VK_CONTROL) < 0)
	{
	ActivateMacro(23);
	return TRUE;
	}
      break;

    case VK_CANCEL:
      if (GetKeyState(VK_CONTROL) < 0 && !(wSystem & SS_SCROLL))
	{
	CmdBreak();
	return TRUE;
	}
      break;

    case 0x91:
      if (!IfSystemStatus(SS_SCRIPT))
        VssScrollMode();

    case VK_NUMLOCK:
    case VK_CAPITAL:
      SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);
      return FALSE;
      break;
    }

  // if (IfSystemStatus(SS_SCROLL))
  //   VssScrollToggle();

  if (Disp.fShowInput || nKermitMode != KM_NULL)
    return FALSE;

  if (!bConnected && IfSystemStatus(SS_SCRIPT))
    return FALSE;

  if ((wSystemState!=SS_TERMINAL && wSystemState!=SS_PROGRAM) || !bConnected)
    return TRUE;

  // unused code .......... MKL  
  //if (i)
  //  {
  //  CommStrOutput(szBuf);
  //  return TRUE;
  //  }

  switch (nCurEmulator)
    {
    case TERM_VT102:
      //dca00032 MKL fixed keyboard lock unlock problem
      // return (EmulatorVT102Key(msg));
      if (bTermLckKey) return (TRUE);
      else if (bTermNoAutoRepeat) {
	 if ((HIWORD(msg->lParam)&0x4000)!=0) return(TRUE);
      }
      return(EmulatorVT102Key(msg));
      break;

    case TERM_VT52:
      //dca00032 MKL fixed keyboard lock unlock problem
      // return (EmulatorVT52Key(msg));
      if (bTermLckKey) return (TRUE);
      else if (bTermNoAutoRepeat) {
	 if ((HIWORD(msg->lParam)&0x4000)!=0) return(TRUE);
      }
      return(EmulatorVT52Key(msg));
      break;

    case TERM_IBM3101:
      //dca00032 MKL fixed keyboard lock unlock problem
      // return (EmulatorIBM3101Key(msg));
      if (bTermLckKey) return (TRUE);
      else return (EmulatorIBM3101Key(msg));
      break;

    case TERM_IBMPC:
      return (EmulatorPCIBMKey(msg));
      break;

    case TERM_VIDTEX:
      return (EmulatorVidtexKey(msg));
      break;

    default:
      return FALSE;
    }
}


void MouseSelectMsg(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static short xMouse = 0;
  static short yMouse = 0;
  RECT Rect;
  PINT pLine;
  int  nHigh, nLow;
  int  x,y;


  nHigh = (int)HIWORD(lParam);
  nLow	= (int)LOWORD(lParam);

  if (IfSystemStatus(SS_SCROLL))
    {
    switch (message)
      {
      case WM_MOUSEMOVE:
        if (!fMouseDown)
          return;
        break;

      case WM_LBUTTONDOWN:
        if (fMouseDown)
          return;

        nMouseAtX = 0;
        nMouseAtY = 0;

        MouseSelectClr();

        y = nHigh / nCurFontHeight + (nTermOffY + nVssCurLinePos);
        x = nLow / nCurFontWidth + nTermOffX + 1;

        if (x < 1) x = 1;
        if (x > nTermMaxCol) x = nTermMaxCol;
        if (y >= TERM_MAXROW - 1) y = TERM_MAXROW - 2;

        nSelectX2 = nSelectX1 = x;
        nSelectY2 = nSelectY1 = y;

        fMouseDown = TRUE;
        SetCapture(hWnd);
        return;
        break;

      case WM_LBUTTONUP:
        yMouse = 0;
        xMouse = 0;
        if (!fMouseDown)
          return;
        fMouseDown = FALSE;
        ReleaseCapture();
        return;

      default:
        return;
      }

    GetClientRect(hWnd, (LPRECT)&Rect);

    y = nHigh / nCurFontHeight;
    if (y < 0)
      {
      x = 1;
      y = 0;
      }
    y += (nTermOffY + nVssCurLinePos);

    if (nHigh < Rect.top)
      x = 1;
    else if (nHigh > Rect.bottom)
      x = nTermMaxCol;
    else
      {
      x = nLow / nCurFontWidth + nTermOffX + 1;
      if (x < 1) x = 1;
      if (x > nTermMaxCol) x = nTermMaxCol;
      }

    if (y >= TERM_MAXROW)
      {
      x = nTermMaxCol;
      y = TERM_MAXROW - 1;
      }

    if (nLow < Rect.left)
      VidScroll(WM_HSCROLL, SB_LINEUP, 0L);
    else if (nLow > Rect.right)
      VidScroll(WM_HSCROLL, SB_LINEDOWN, 0L);
    else if (nHigh < Rect.top)
      VssScrollDown(1);
    else if (nHigh > Rect.bottom)
      VssScrollUp(1);

    if (nLow < Rect.left || nLow > Rect.right ||
        nHigh < Rect.top || nHigh > Rect.bottom)
      {
      nMouseAtX = nLow;
      nMouseAtY = nHigh;
      }
    else
      {
      nMouseAtX = 0;
      nMouseAtY = 0;
      }

    if (!fSelectedArea)
      {
      fSelectedArea = TRUE;
      VidSelectArea(nSelectX1, nSelectY1, x, y);
      if (bConnected)
        EnableMenuItem(hWndMenu, CMD_COPYSEND, MF_ENABLED);
      ChangeMenuMsg(CMD_COPYFILE,    MSGMNU_CPY_FILE);
      ChangeMenuMsg(CMD_COPYCAPTURE, MSGMNU_CPY_LOG);
      if (!bCaptureOn)
        EnableMenuItem(hWndMenu, CMD_COPYCAPTURE, MF_GRAYED);
      ChangeMenuMsg(CMD_COPYNOTES,   MSGMNU_CPY_NOTES);
      ChangeMenuMsg(CMD_COPYPRINTER, MSGMNU_CPY_PRINTER);
      }
    else
      {
      int n1, n2, n3;

      if (nSelectX2 == x && nSelectY2 == y)
        return;

      n1 = nSelectY1 * nTermMaxCol + nSelectX1;
      n2 = nSelectY2 * nTermMaxCol + nSelectX2;
      n3 = y * nTermMaxCol + x;

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

    nSelectX2 = x;
    nSelectY2 = y;
    }
}

void MouseSelectClr()
{
  if (fSelectedArea)
    {
    VidSelectArea(nSelectX1, nSelectY1, nSelectX2, nSelectY2);
    fSelectedArea = FALSE;
    EnableMenuItem(hWndMenu, CMD_COPYSEND, MF_GRAYED);
    ChangeMenuMsg(CMD_COPYFILE,    MSGMNU_SCR_FILE);
    ChangeMenuMsg(CMD_COPYCAPTURE, MSGMNU_SCR_LOG);
    if (!bCaptureOn)
      EnableMenuItem(hWndMenu, CMD_COPYCAPTURE, MF_GRAYED);
    ChangeMenuMsg(CMD_COPYNOTES,   MSGMNU_SCR_NOTES);
    ChangeMenuMsg(CMD_COPYPRINTER, MSGMNU_SCR_PRINTER);
    }
}


int TermNameScan(wChar)
  WORD wChar;
{
  if ((wChar == ASC_DEL || wChar == ASC_BS) && nTermWorkInx)
    {
    nTermWorkInx--;
    nCommCount = 0;
    }
  else if (wChar > ' ' && nCommCount < 8)
    {
    if (nTermWorkInx < sizeof(sTermWorkName) - 2)
      {
      sTermWorkName[nTermWorkInx++] = (char)wChar;
      }

    nCommCount = 0;
    }
  else
    {
    if (nTermWorkInx > 2)
      {
      sTermWorkName[nTermWorkInx] = NULL;
      strcpy(sTermFileName, sTermWorkName);
      }
    sTermWorkName[0] = NULL;
    nTermWorkInx = 0;
    nCommCount = 0;
    if (wChar > ' ')
      sTermWorkName[nTermWorkInx++] = (char)wChar;
    }

  return (0);
}


void TermMessage(short sMsgNo1, short sErrNo)
  {
  char buf[256];


  LoadString(hWndInst, sMsgNo1, buf, sizeof(buf));
  if (sErrNo)
    {
    register i;

    i = strlen(buf);
    buf[i++] = ',';
    buf[i++] = ' ';

    LoadString(hWndInst, ErrorToMsg(sErrNo), &buf[i], sizeof(buf)-i);
    }

  LineToTerminal(buf);
  }


void LineToTerminal(line)
  PSTR line;
{
  char buf[4];
  register int i;


  SendMessage(hChildTerminal, UM_DATA, strlen(line), (DWORD)((LPSTR)line));

  i = 0;
  buf[i++] = ASC_CR;
  if (!Term.fNewLine)
    buf[i++] = ASC_LF;

  SendMessage(hChildTerminal, UM_DATA, i, (DWORD)((LPSTR)buf));
}

void CharToTerminal(key)
  int key;
{
  char buf[4];

  buf[0] = (char)key;
  SendMessage(hChildTerminal, UM_DATA, 1, (DWORD)((LPSTR)buf));
}
