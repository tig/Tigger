/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
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
*             1.01 08/28/89 00020    Modified to for Windows 2/3 support.     *
*	      1.01 10/31/89 00052    Add context help to menus & dialogs.     *
*	      1.01 11/07/89 00058    Reset info font for :XTALK startup.      *
*             1.01 01/18/90 00084    Add help to CopyToHost and PasteToHost.  *
*             1.01 01/31/90 dca00010 Added help (F1 support) for Window Menu. *
*    1.1  03/01/90 dca00031 MKL added "L1" indicator to info bar for VT102    *
*    1.1  03/19/90 dca00053 MKL added proportional font support		      *
*    1.1  03/26/90 dca00057 MKL changed InfoDisp() to use system font	      *
*    1.1  04/04/90 dca00059 MKL added Win3.0 Help			      *
*    1.1  04/05/90 dca00070 MKL fixed Win3.0 WM_MENUSELECT parameter changes  *
*	problem (info window not updated problem)			      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <time.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "font.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "properf.h"
#include "dialogs.h"
#include "dde.h"


  HWND hChildInfo = NULL;
  char fMenuMode;

  HMENU hInfoFileMenu;
  HMENU hInfoEditMenu;
  HMENU hInfoSessMenu;
  HMENU hInfoConfMenu;
  HMENU hInfoUserMenu;
  HMENU hInfoWindMenu;
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

  static void near InfoDisp(HWND hWnd, HDC hDC);
  static void near InfoKeyState(HWND hWnd, HDC hDC);



BOOL CreateInfoChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  char szClassName[64];


  fMenuMode = FALSE;

  LoadString(hWndInst, IDS_WNDCLASS_INFO, szClassName, sizeof(szClassName));

  hWnd = CreateWindow(szClassName,
		      NULL,
		      WS_CHILD | WS_VISIBLE,
		      0,
		      0,
		      0,
		      0,
		      hParent,
		      (HMENU)NULL,
		      (HANDLE)hWndInst,
		      NULL
		      );

  if (hWnd)
    {
    hChildInfo = hWnd;
    return TRUE;
    }
  else
    {
    hChildInfo = NULL;
    return FALSE;
    }
}


BOOL DestroyInfoChild()
{
  if (hChildInfo)
    DestroyWindow(hChildInfo);

  hChildInfo = NULL;

  return TRUE;
}


void InfoDisplay(msg)
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


void InfoDisplayMsg(msg)
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


long WndInfoProc(hWnd, message, wParam, lParam )
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
      if (hInfoFont)
	SelectObject(hDC, hInfoFont);
      InfoKeyState(hWnd, hDC);
      ReleaseDC(hWnd, hDC);
      break;

    case UM_INFOMSG:
      hDC = GetDC(hWnd);
      InfoDisp(hWnd, hDC);
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
      hInfoWindMenu = GetSubMenu(hMenu, 5);
      hInfoHelpMenu = GetSubMenu(hMenu, 6);
      break;

    case WM_MENUSELECT:
      wHelpMenu = 0;  // 00052 Reset to no active menu item.
      // dca00070 MKL fixed Win3.0 WM_MENUSELECT parameter changes problem
      // if ((wParam & 0xF000) || LOWORD(lParam) == 0xFFFF)
      if ((wParam & 0xE000) || LOWORD(lParam) == 0xFFFF)
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
        InfoDisp(hWnd, hDC);
	ReleaseDC(hWnd, hDC);
	}
      break;

    case WM_TIMER:
      {
      RECT  Rect;
      short sOffset;

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
        InfoDisp(hWnd, hDC);
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

      if (hInfoFont)
	SelectObject(hDC, hInfoFont);

      GetClientRect(hWnd, &Rect);
      sOffset = (Rect.bottom - HIWORD(GetTextExtent(hDC, szClock, 1))) / 2;

      nXPos = nClockPos;
      for (i = 0; i < 20; i++)
	{
	if (szClock[i] != szBuf[i])
	  {
	  szClock[i] = szBuf[i];
          TextOut(hDC, nXPos, sOffset, &szBuf[i], 1);
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

      ReleaseDC(hWnd, hDC);

      if (i != wInfoMode)
	{
        hDC = GetDC(hWnd);
	wInfoMode = i;
        InfoDisp(hWnd, hDC);
        ReleaseDC(hWnd, hDC);
	}
      }
      break;

    case WM_SIZE:
      // dca00031 MKL added "L1" indicator to info bar for VT102
      // nClockPos       = LOWORD(lParam) - 16 * nSysFontWidth;
      // nKeysPos        = nClockPos - 15 * nSysFontWidth;
      // dca00053 MKL added proportional font support
      nClockPos       = LOWORD(lParam) - 16 * nSysFontWidth;
      nKeysPos        = nClockPos - 18 * nSysFontWidth;
      TextRect.top    = 1;
      TextRect.bottom = HIWORD(lParam);
      TextRect.left   = 0;
      // dca00031 MKL added "L1" indicator to info bar for VT102
      // TextRect.right  = nKeysPos - 2 * nSysFontWidth;
      TextRect.right  = nKeysPos - 1 * nSysFontWidth;
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      MoveTo(ps.hdc, ps.rcPaint.left, 0);
      LineTo(ps.hdc, ps.rcPaint.right, 0);

      InfoDisp(hWnd, ps.hdc);

      MoveTo(ps.hdc, nClockPos - nSysFontWidth, ps.rcPaint.top);
      LineTo(ps.hdc, nClockPos - nSysFontWidth, ps.rcPaint.bottom);

      if (hInfoFont)
	SelectObject(ps.hdc, hInfoFont);

      fCapsLock = FALSE;
      fNumsLock = FALSE;
      InfoKeyState(hWnd, ps.hdc);

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

static void near InfoDisp(HWND hWnd, HDC hDC)
  {
  RECT  Rect;
  short sOffset;
  char  szBuf[64];
  int   i;


  // dca00057 MKL changed message box of info bar to use system (proportional)
  //	font
  // if (hInfoFont)
  //   SelectObject(hDC, hInfoFont);
  SelectObject(hDC, GetStockObject(SYSTEM_FONT));

  GetClientRect(hWnd, &Rect);
  szBuf[0] = 'X';
  sOffset = (Rect.bottom - HIWORD(GetTextExtent(hDC, szBuf, 1))) / 2;

  i = 0;
  if (fMenuMode)
    {
    if (wMenuItem == -1)
      i = INFO_SYSBOX;
    else if (wMenuItem == hInfoFileMenu)
      {
      i = INFO_FILE;
      wHelpMenu = HELP_ITEM_MFILE;
      }
    else if (wMenuItem == hInfoEditMenu)
      {
      i = INFO_EDIT;
      wHelpMenu = HELP_ITEM_MEDIT;
      }
    else if (wMenuItem == hInfoSessMenu)
      {
      i = INFO_MSESSION;
      wHelpMenu = HELP_ITEM_MACTION;
      }
    else if (wMenuItem == hInfoConfMenu)
      {
      i = INFO_SETUP;
      wHelpMenu = HELP_ITEM_MSETUP;
      }
    else if (wMenuItem == hInfoUserMenu)
      {
      i = INFO_USER;
      wHelpMenu = HELP_ITEM_MUSER;
      }
    else if (wMenuItem == hInfoWindMenu)
      {
      i = INFO_WINDOW;
 // dca00010 PJL Added Help for Window Menu.
 //   wHelpMenu = 0;
      wHelpMenu = HELP_ITEM_WINDOW;
      }
    else if (wMenuItem == hInfoHelpMenu)
      {
      i = INFO_HELP;
      // dca00059 MKL added Win3.0 Help
      if (sVersion < 3) wHelpMenu = HELP_ITEM_MHELP;
      else wHelpMenu = 0;
      }
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
	  // 00052 Select a help topic for each menu item (if possible).
	  switch (wMenuItem)
	    {
	    case CMD_NEW:
	      wHelpMenu = HELP_ITEM_NEW;
	      break;

	    case CMD_OPEN:
	      wHelpMenu = HELP_ITEM_OPEN;
	      break;

	    case CMD_SAVE:
	      wHelpMenu = HELP_ITEM_SAVE;
	      break;

	    case CMD_SAVEAS:
	      wHelpMenu = HELP_ITEM_SAVEAS;
	      break;

	    case CMD_PAGESETUP:
	      wHelpMenu = HELP_ITEM_PAGESETUP;
	      break;

	    case CMD_PRINT:
	      wHelpMenu = HELP_ITEM_PRINT;
	      break;

	    case CMD_PRINTSETUP:
	      wHelpMenu = HELP_ITEM_PRINTERSETUP;
	      break;

	    case CMD_CAPTURE:
	      wHelpMenu = HELP_ITEM_CAPTURE;
	      break;

	    case CMD_UPLOAD:
	      wHelpMenu = HELP_ITEM_UPLOADTEXT;
	      break;

	    case CMD_SEND:
	      wHelpMenu = HELP_ITEM_SEND;
	      break;

	    case CMD_RECEIVE:
	      wHelpMenu = HELP_ITEM_RECEIVE;
	      break;

	    case CMD_EXIT:
	      wHelpMenu = HELP_ITEM_EXIT;
	      break;

	    case CMD_COPY:
	      wHelpMenu = HELP_ITEM_COPY;
	      break;

	    case CMD_PASTE:
	      wHelpMenu = HELP_ITEM_PASTE;
	      break;

	    case CMD_COPYFILE:
	      if (fSelectedArea)
		wHelpMenu = HELP_ITEM_COPYFILE;
	      else
		wHelpMenu = HELP_ITEM_SCREENFILE;
	      break;

	    case CMD_COPYCAPTURE:
	      if (fSelectedArea)
		wHelpMenu = HELP_ITEM_COPYCAPTURE;
	      else
		wHelpMenu = HELP_ITEM_SCREENCAPTURE;
	      break;

	    case CMD_COPYNOTES:
	      if (fSelectedArea)
		wHelpMenu = HELP_ITEM_COPYNOTES;
	      else
		wHelpMenu = HELP_ITEM_SCREENNOTES;
	      break;

	    case CMD_COPYPRINTER:
	      if (fSelectedArea)
		wHelpMenu = HELP_ITEM_COPYPRINTER;
	      else
		wHelpMenu = HELP_ITEM_SCREENPRINTER;
	      break;

	    case CMD_COPYSEND:
              wHelpMenu = HELP_ITEM_COPYHOST;   //00084 Add help
	      break;

	    case CMD_PASTEUPLOAD:
              wHelpMenu = HELP_ITEM_PASTEHOST;  //00084 Add help.
	      break;

	    case CMD_SCROLL:
	      wHelpMenu = HELP_ITEM_SCROLL;
	      break;

	    case CMD_CONNECT:
	      wHelpMenu = HELP_ITEM_CONNECT;
	      break;

	    case CMD_DIAL:
	      wHelpMenu = HELP_ITEM_DIAL;
	      break;

	    case CMD_EXECUTE:
	      wHelpMenu = HELP_ITEM_SCRIPT;
	      break;

	    case CMD_NOTES:
	      wHelpMenu = HELP_ITEM_NOTES;
	      break;

	    case CMD_KERMIT:
	      wHelpMenu = HELP_ITEM_KCP;
	      break;

	    case CMD_PLAYBACK:
	      wHelpMenu = HELP_ITEM_PLAYBACK;
	      break;

	    case CMD_BREAK:
	      wHelpMenu = HELP_ITEM_BREAK;
	      break;

	    case CMD_RESET:
	      wHelpMenu = HELP_ITEM_RESET;
	      break;

	    case CMD_SESSION:
	      wHelpMenu = HELP_ITEM_SESSION;
	      break;

	    case CMD_COMMUNICATIONS:
	      wHelpMenu = HELP_ITEM_DEVICE;
	      break;

	    case CMD_TERMINAL:
	      wHelpMenu = HELP_ITEM_TERMINAL;
	      break;

	    case CMD_PROTOCOL:
	      wHelpMenu = HELP_ITEM_PROTOCOL;
	      break;

	    case CMD_ASCII:
	      wHelpMenu = HELP_ITEM_ASCIISET;
	      break;

	    case CMD_MACROS:
	      wHelpMenu = HELP_ITEM_FUNCTIONS;
	      break;

	    case CMD_DISPLAY:
	      wHelpMenu = HELP_ITEM_DISPLAY;
	      break;

	    case CMD_FONTS:
	      wHelpMenu = HELP_ITEM_FONTS;
	      break;

	    case CMD_MODEM:
	      wHelpMenu = HELP_ITEM_MODEM;
	      break;

	    case CMD_SYSTEM:
	      wHelpMenu = HELP_ITEM_SYSTEM;
	      break;

	    // dca00010 PJL Added Help for Window.FileXfer & Window.Notes.
	    case CMD_WINDOW_XFER:
	      // dca00059 MKL
	      if (sVersion >=3) wHelpMenu = HELP_ITEM_TRANSFER;
	      else wHelpMenu = HELP_ITEM_WINDOW;
	      break;
	    case CMD_WINDOW_NOTES:
	      // dca00059 MKL
	      if (sVersion >=3) wHelpMenu = HELP_ITEM_NOTES;
	      else wHelpMenu = HELP_ITEM_WINDOW;
	      break;
	    case CMD_INDEX:
	      // dca00059 MKL added Win3.0 Help
	      if (sVersion < 3) wHelpMenu = HELP_ITEM_TOPICS;
	      else wHelpMenu = 0;
	      break;
	    case CMD_ABOUT:
	      wHelpMenu = HELP_ITEM_ABOUT;
	      break;
	    case CMD_HELP_PROC:
	    case CMD_HELP_CMMD:
	    case CMD_HELP_MOUSE:
	    case CMD_HELP_KBD:
	    case CMD_HELP_DDE:
	    case CMD_HELP_HELP:
	      wHelpMenu = 0; /* no context sentitive help */
	      break;
	    }
	  if (wMenuItem == CMD_INDEX && sVersion >= 3) i = INFO_INDEX;
	  else if (wMenuItem >= CMD_NEW && wMenuItem <= CMD_MENUNULL)
            i = wMenuItem - CMD_NEW + INFO_NEW;
          else if (wMenuItem >= CMD_WINDOW_XFER && wMenuItem <= CMD_HELP_LAST)
            i = wMenuItem - CMD_NEW + INFO_NEW;
	  else
	    i = INFO_MENUMACRO;
	}
      }

    if (i && wHelpDialog == NULL)
      {
      LoadString(hWndInst, i, szBuf, sizeof(szBuf));
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    }
  else
    {
    if (wSystem & SS_SCROLL)
      {
      strcpy(szBuf, "'Edit.Scroll' to return to terminal mode");
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    else if (sInfoMessage[0] != NULL)
      {
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, sInfoMessage, strlen(sInfoMessage), NULL);
      }
    else if (wSystem & SS_DDE)
      {
      strcpy(szBuf, "DDE Mode...");
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    else if (wInfoMode == 0)
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, "Disconnected", 12, NULL);
    // MKL change the third param form 1 to sOffset
    else if (wInfoMode == 1 && Comm.cLocal)
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, "Connected to Computer", 21, NULL);
    else if (wInfoMode == 1)
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, "Connected to Modem", 18, NULL);
    else
      {
      strcpy(szBuf, "Connected to ");
      LibEnstore(Sess.szDescription, &szBuf[13], sizeof(szBuf)-13);
      ExtTextOut(hDC, 4, sOffset, -1, (LPRECT)&TextRect, szBuf, strlen(szBuf), NULL);
      }
    }
  }

static void near InfoKeyState(HWND hWnd, HDC hDC)
  {
  RECT  Rect;
  short sOffset;
  DWORD dwColor;
  char	szBuf[16];


  memset(szBuf, ' ', sizeof(szBuf));

  GetClientRect(hWnd, &Rect);
  sOffset = (Rect.bottom - HIWORD(GetTextExtent(hDC, szBuf, 1))) / 2;

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

  // dca00031 MKL added "L1" indicator to info bar for VT102
  if (cTermLEDs & 0x01)
    {
    szBuf[13] = 'L';
    szBuf[14] = '1';
    }

  dwColor = SetTextColor(hDC, RGB(255, 0, 0));
  // dca00031 MKL added "L1" indicator to info bar for VT102
  // dca00053 MKL added proportional font support
  //TextOut(hDC, nKeysPos, sOffset, szBuf, 12);
  Rect.top = TextRect.top;
  Rect.bottom = TextRect.bottom;
  Rect.left = nKeysPos-2;
  Rect.right = nClockPos - nSysFontWidth - 1;
  ExtTextOut(hDC, nKeysPos-2, sOffset, -1, (LPRECT)&Rect, szBuf, 15, NULL);
  SetTextColor(hDC, dwColor);
  }
