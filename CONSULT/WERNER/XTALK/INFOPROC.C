/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Info Procedure (INFOPROC.C)                                     *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the window procedure code for the info     *
*             window.                                                         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#define NOATOM                  /* atom routines */
#define NOBITMAP		/* typedef HBITMAP and associated routines */
#define NOBRUSH 		/* typedef HBRUSH and associated routines */
#define NOCLIPBOARD		/* clipboard routines */
#define NOCOLOR 		/* CTLCOLOR_*, COLOR_* */
#define NOCREATESTRUCT		/* typedef CREATESTRUCT */
#define NOCTLMGR		/* control and dialog routines */
#define NODRAWTEXT		/* DrawText() and DT_* */
#define NOFONT			/* typedef FONT and associated routines */
#define NOGDICAPMASKS		/* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOICON			/* IDI_* */
#define NOKANJI 		/* Kanji support stuff. */
#define NOKEYSTATE		/* MK_* */
#define NOMB			/* MB_* and MessageBox() */
#define NOMETAFILE		/* typedef METAFILEPICT */
#define NOMINMAX		/* Macros min(a,b) and max(a,b) */
#define NONCMESSAGES		/* WM_NC* and HT* */
#define NOPEN			/* typedef HPEN and associated routines */
#define NORASTEROPS		/* binary and tertiary raster ops */
#define NOREGION		/* typedef HRGN and associated routines */
#define NORESOURCE		/* Predefined resource types:  RT_* */
#define NOSCROLL		/* SB_* and scrolling routines */
#define NOSHOWWINDOW		/* SHOW_* and HIDE_* */
#define NOSOUND 		/* Sound driver routines */
#define NOSYSCOMMANDS		/* SC_* */
#define NOSYSMETRICS		/* SM_*, GetSystemMetrics */
#define NOWH			/* SetWindowsHook and WH_* */
#define NOWINSTYLES		/* WS_*, CS_*, ES_*, LBS_* */
#define NOWNDCLASS		/* typedef WNDCLASS and associated routines */

#include <windows.h>
#include <time.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "properf.h"
#include "dialogs.h"
#include "dde.h"

#define  TIM_CLOCK	1

  HMENU hInfoFileMenu;
  HMENU hInfoEditMenu;
  HMENU hInfoSessMenu;
  HMENU hInfoConfMenu;
  HMENU hInfoUserMenu;
  HMENU hInfoHelpMenu;

  char sInfoMessage[64];


  static WORD wMenuItem;
  static WORD wInfoMode;
  static int  nKeysPos;
  static char cScrnMode;
  static int  nClockPos;
  static char szClock[26];
  static long lLastTime;
  static RECT TextRect;

  static void near pascal InfoDisp(HDC);
  static void near pascal InfoKeyState(HDC);


void far pascal InfoDisplay(msg)
  char *msg;
{
  if (msg == NULL)
    sInfoMessage[0] = NULL;
  else
    {
    register int i;

    for (i = 0; i < sizeof(sInfoMessage) - 1 && *msg >= ' '; i++)
      sInfoMessage[i] = *msg++;
    sInfoMessage[i] = NULL;
    }

  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}


void far pascal InfoDisplayMsg(msg)
  int msg;
{
  char buf[64];

  if (msg == NULL)
    sInfoMessage[0] = NULL;
  else
    {
    register int i;

    LoadString(hWndInst, msg, buf, sizeof(buf));

    for (i = 0; i < sizeof(sInfoMessage) - 1 && buf[i] >= ' '; i++)
      sInfoMessage[i] = buf[i];
    sInfoMessage[i] = NULL;
    }

  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}


long far pascal WndInfoProc(hWnd, message, wParam, lParam )
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  PAINTSTRUCT ps;
  HMENU hMenu;
  HDC	hDC;
  char	szBuf[32];
  long	lTime;
  int	i, nXPos;


  switch (message)
    {
    case UM_SCRNMODE:
      if (lParam != 0L)
	cScrnMode |= wParam;
      else
	cScrnMode &= ~wParam;

    case UM_SCROLPOS:
    case UM_KEYSTATE:
      hDC = GetDC(hWnd);
      InfoKeyState(hDC);
      ReleaseDC(hWnd, hDC);
      break;

    case UM_INFOMSG:
      hDC = GetDC(hWnd);
      InfoDisp(hDC);
      ReleaseDC(hWnd, hDC);
      break;

    case WM_CREATE:
      fMenuMode = FALSE;
      wMenuItem = NULL;
      wInfoMode = NULL;
      cScrnMode = 0;
      time(&lLastTime);
      sInfoMessage[0] = NULL;

      hMenu	    = GetMenu(GetParent(hWnd));
      hInfoFileMenu = GetSubMenu(hMenu, 0);
      hInfoEditMenu = GetSubMenu(hMenu, 1);
      hInfoSessMenu = GetSubMenu(hMenu, 2);
      hInfoConfMenu = GetSubMenu(hMenu, 3);
      hInfoUserMenu = GetSubMenu(hMenu, 4);
      hInfoHelpMenu = GetSubMenu(hMenu, 5);
      break;

    case WM_MENUSELECT:
      if (wParam & 0xF000)
	fMenuMode = FALSE;
      else
	fMenuMode = TRUE;
      if (wMenuItem != wParam)
	{
	if ((LOWORD(lParam) & (MF_SYSMENU|MF_POPUP)) == (MF_SYSMENU|MF_POPUP))
	  wMenuItem = -1;
	else
	  wMenuItem = wParam;
	hDC = GetDC(hWnd);
	InfoDisp(hDC);
	ReleaseDC(hWnd, hDC);
	}
      break;

    case WM_TIMER:
      if (bConnected)
	{
	if (Sess.szDescription[0] == NULL)
	  i = 1;
	else
	  i = 2;
	}
      else
	{
	i = 0;
	}

      if (i != wInfoMode)
	{
	wInfoMode = i;
        hDC = GetDC(hWnd);
	InfoDisp(hDC);
        ReleaseDC(hWnd, hDC);
	}

      time(&lTime);
      if (lTime == lLastTime)
	break;

      if ((i = (int)(lTime % 60)) != 00 && lTime == lLastTime+1 && szClock[0] != NULL)
	{
	strcpy(szBuf, szClock);
	szBuf[13] = (char)('0' + i / 10);
	szBuf[14] = (char)('0' + i % 10);
	}
      else
	{
	strcpy(szBuf, ctime(&lTime) + 4);
	}

      if (bConnected)
        SendMessage(hChildStatus, UM_TIMER, (WORD)(lTime - lLastTime), 0L);
      lLastTime = lTime;

      /*
      **  Display only the characters that have changed (makes for a faster
      **  update on the screen).
      */

      hDC = GetDC(hWnd);

      nXPos = nClockPos;
      for (i = 0; i < 20; i++)
	{
	if (szClock[i] != szBuf[i])
	  {
	  szClock[i] = szBuf[i];
	  TextOut(hDC, nXPos, 1, &szBuf[i], 1);
	  }

	nXPos += nSysFontWidth;
	}

      if (bConnected)
	{
	if (Sess.szDescription[0] == NULL)
	  i = 1;
	else
	  i = 2;
	}
      else
	{
	i = 0;
	}

      if (i != wInfoMode)
	{
	wInfoMode = i;
	InfoDisp(hDC);
	}

      ReleaseDC(hWnd, hDC);
      break;

    case WM_SIZE:
      nClockPos       = LOWORD(lParam) - 16 * nSysFontWidth;
      nKeysPos	      = nClockPos - 14 * nSysFontWidth;
      TextRect.top    = 1;
      TextRect.bottom = HIWORD(lParam);
      TextRect.left   = 0;
      TextRect.right  = nKeysPos - 2 * nSysFontWidth;
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      MoveTo(ps.hdc, ps.rcPaint.left, 0);
      LineTo(ps.hdc, ps.rcPaint.right, 0);

      InfoDisp(ps.hdc);

      MoveTo(ps.hdc, nClockPos - nSysFontWidth, ps.rcPaint.top);
      LineTo(ps.hdc, nClockPos - nSysFontWidth, ps.rcPaint.bottom);

      fCapsLock = FALSE;
      fNumsLock = FALSE;
      InfoKeyState(ps.hdc);

      MoveTo(ps.hdc, nKeysPos - nSysFontWidth, ps.rcPaint.top);
      LineTo(ps.hdc, nKeysPos - nSysFontWidth, ps.rcPaint.bottom);

      for (i = 0; i < sizeof(szClock); i++)
	szClock[i] = NULL;

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      break;

    case WM_DDE_INITIATE:
    case WM_DDE_TERMINATE:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_REQUEST:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
      ProcessDDE(hWnd, message, wParam, lParam);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}

static void near pascal InfoDisp(hDC)
  HDC hDC;
{
  char szBuf[64];
  int  i;

  i = 0;
  if (fMenuMode)
    {
    if (wMenuItem == -1)
      i = INFO_SYSBOX;
    else if (wMenuItem == hInfoFileMenu)
      i = INFO_FILE;
    else if (wMenuItem == hInfoEditMenu)
      i = INFO_EDIT;
    else if (wMenuItem == hInfoSessMenu)
      i = INFO_MSESSION;
    else if (wMenuItem == hInfoConfMenu)
      i = INFO_SETUP;
    else if (wMenuItem == hInfoUserMenu)
      i = INFO_USER;
    else if (wMenuItem == hInfoHelpMenu)
      i = INFO_HELP;
    else
      {
      switch (wMenuItem)
	{
	case 0xF120:
	  i = INFO_RESTORE;
	  break;

	case 0xF010:
	  i = INFO_MOVE;
	  break;

	case 0xF000:
	  i = INFO_SIZE;
	  break;

	case 0xF020:
	  i = INFO_REDUCE;
	  break;

	case 0xF030:
	  i = INFO_ENLARGE;
	  break;

	case 0xF060:
	  i = INFO_CLOSE;
	  break;

	default:
	  if (wMenuItem >= CMD_NEW && wMenuItem <= CMD_MENUNULL)
            i = wMenuItem - CMD_NEW + INFO_NEW;
          else if (wMenuItem >= CMD_INDEX && wMenuItem <= CMD_ABOUT)
            i = wMenuItem - CMD_NEW + INFO_NEW;
	  else
	    i = INFO_MENUMACRO;
	}
      }

    if (i)
      {
      LoadString(hWndInst, i, szBuf, sizeof(szBuf));
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    }
  else
    {
    if (wSystem & SS_SCROLL)
      {
      strcpy(szBuf, "'Edit.Scroll' to return to terminal mode");
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    else if (sInfoMessage[0] != NULL)
      {
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, sInfoMessage, strlen(sInfoMessage), NULL);
      }
    else if (wSystem & SS_DDE)
      {
      strcpy(szBuf, "DDE Mode...");
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    else if (wInfoMode == 0)
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, "Disconnected", 12, NULL);
    else if (wInfoMode == 1 && Comm.cLocal)
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, "Connected to Computer", 21, NULL);
    else if (wInfoMode == 1)
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, "Connected to Modem", 18, NULL);
    else
      {
      strcpy(szBuf, "Connected to ");
      LibEnstore(Sess.szDescription, &szBuf[13], sizeof(szBuf)-13);
      ExtTextOut(hDC, 0, 1, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    }
}

static void near pascal InfoKeyState(hDC)
  HDC hDC;
{
  DWORD dwColor;
  char	szBuf[16];


  memset(szBuf, ' ', sizeof(szBuf));


  if (GetKeyState(VK_CAPITAL) & 0x01)
    {
    szBuf[0] = 'C';
    szBuf[1] = 'A';
    szBuf[2] = 'P';
    }

  if (GetKeyState(VK_NUMLOCK) & 0x01)
    {
    szBuf[4] = 'N';
    szBuf[5] = 'U';
    szBuf[6] = 'M';
    }

  if (wSystem & SS_DIAL)
    {
    szBuf[ 8] = 'D';
    szBuf[ 9] = 'I';
    szBuf[10] = 'A';
    szBuf[11] = 'L';
    }
  else if (wSystem & SS_XFER)
    {
    szBuf[ 8] = 'X';
    szBuf[ 9] = 'F';
    szBuf[10] = 'E';
    szBuf[11] = 'R';
    }
  else if (wSystem & SS_SCRIPT)
    {
    szBuf[ 8] = 'E';
    szBuf[ 9] = 'X';
    szBuf[10] = 'E';
    szBuf[11] = 'C';
    }
  else if (GetKeyState(0x91) & 0x01)
    {
    szBuf[ 8] = 'S';
    szBuf[ 9] = 'C';
    szBuf[10] = 'R';
    szBuf[11] = 'L';
    }
  else if ((cScrnMode & INFO_XOFFHOLD) && bConnected)
    {
    szBuf[ 8] = 'X';
    szBuf[ 9] = 'O';
    szBuf[10] = 'F';
    szBuf[11] = 'F';
    }
  else if (cScrnMode & INFO_CONTROL)
    {
    szBuf[ 8] = 'C';
    szBuf[ 9] = 'N';
    szBuf[10] = 'T';
    szBuf[11] = 'L';
    }
  else if (cScrnMode & INFO_AUTO)
    {
    szBuf[ 8] = 'A';
    szBuf[ 9] = 'U';
    szBuf[10] = 'T';
    szBuf[11] = 'O';
    }

  dwColor = SetTextColor(hDC, RGB(255, 0, 0));
  TextOut(hDC, nKeysPos, 1, szBuf, 12);
  SetTextColor(hDC, dwColor);
}
