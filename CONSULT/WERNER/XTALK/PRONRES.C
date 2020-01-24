/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Main Window Support (PRONRES.C)                                 *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains support routines for the main window.      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00040 Permit hiding of the xfer dialog window.    *
*	      1.01 12/29/89 00040 Prevent InputLine and KCP conflict.	      *
* 1.1 04/02/90 dca00060 MKL added Win3.0 Icon				      *
* 1.1 04/09/90 dca00074 MKL added new icon for Win3.0			      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dde.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "disp.h"
#include "macro.h"
#include "term.h"
#include "interp.h"
#include "prodata.h"
#include "properf.h"
#include "kermit.h"

// dca00074 MKL added new icon for Win3.0			      *
#define IDI_ICON2	31

static void near UpdateMenus();


void SetSystemStatus(stat)
  WORD stat;
{
  if (stat == NULL)
    wSystem = NULL;
  else
    wSystem |= stat;

  if (wSystem & SS_DIAL)
    wSystemState = SS_DIALING;
  else if (wSystem & (SS_XFER | SS_DIRECT))
    wSystemState = SS_PROCESS;
  else if (wSystem & (SS_SCRIPT))
    wSystemState = SS_PROGRAM;
  else if (wSystem & (SS_ABORT | SS_INIT | SS_SCROLL))
    wSystemState = SS_NULL;
  else
    wSystemState = SS_TERMINAL;

  if (stat == SS_SCRIPT)
    {
    ChangeMenuMsg(CMD_EXECUTE, MSGMNU_SCRIPT2);
    CheckMenuItem(hWndMenu, CMD_EXECUTE, MF_CHECKED);
    EnableMenuItem(hWndMenu, CMD_SCROLL, MF_GRAYED);
    }

  UpdateMenus();

  SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);
  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}

void ResetSystemStatus(stat)
  WORD stat;
{
  wSystem &= ~stat;

  if (wSystem & SS_DIAL)
    wSystemState = SS_DIALING;
  else if (wSystem & (SS_XFER | SS_DIRECT))
    wSystemState = SS_PROCESS;
  else if (wSystem & (SS_SCRIPT))
    wSystemState = SS_PROGRAM;
  else if (wSystem & (SS_ABORT | SS_INIT | SS_SCROLL))
    wSystemState = SS_NULL;
  else
    wSystemState = SS_TERMINAL;

  if (stat == SS_SCRIPT)
    {
    ChangeMenuMsg(CMD_EXECUTE, MSGMNU_SCRIPT1);
    CheckMenuItem(hWndMenu, CMD_EXECUTE, MF_UNCHECKED);
    EnableMenuItem(hWndMenu, CMD_SCROLL, MF_ENABLED);
    }

  UpdateMenus();

  SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);
  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}

static void near UpdateMenus()
{
  if (wSystemState != SS_TERMINAL)
    {
    EnableMenuItem(hWndMenu, CMD_DIAL, MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_UPLOAD, MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_SEND, MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_RECEIVE, MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_PASTEUPLOAD, MF_GRAYED);
    if (IfSystemStatus(SS_SCRIPT))
      EnableMenuItem(hWndMenu, CMD_EXECUTE, MF_ENABLED);
    else
      EnableMenuItem(hWndMenu, CMD_EXECUTE, MF_GRAYED);
    if (nKermitMode == KM_NULL)
      EnableMenuItem(hWndMenu, CMD_KERMIT, MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_PLAYBACK, MF_GRAYED);
    if (IfSystemStatus(SS_XFER))
      {
      EnableMenuItem(hWndMenu, CMD_COMMUNICATIONS, MF_GRAYED);
      EnableMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_ENABLED);  // 00040
      }
    else
      {
      EnableMenuItem(hWndMenu, CMD_COMMUNICATIONS, MF_ENABLED);
      CheckMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_UNCHECKED); // 00040
      EnableMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_GRAYED);   // 00040
      }
    }

  if (wSystemState == SS_TERMINAL)
    {
    if (bConnected)
      {
      EnableMenuItem(hWndMenu, CMD_DIAL, MF_GRAYED);
      EnableMenuItem(hWndMenu, CMD_UPLOAD, MF_ENABLED);
      EnableMenuItem(hWndMenu, CMD_SEND, MF_ENABLED);
      EnableMenuItem(hWndMenu, CMD_RECEIVE, MF_ENABLED);
      EnableMenuItem(hWndMenu, CMD_PASTEUPLOAD, MF_ENABLED);
      }
    else
      {
      EnableMenuItem(hWndMenu, CMD_DIAL, MF_ENABLED);
      EnableMenuItem(hWndMenu, CMD_UPLOAD, MF_GRAYED);
      EnableMenuItem(hWndMenu, CMD_SEND, MF_GRAYED);
      EnableMenuItem(hWndMenu, CMD_RECEIVE, MF_GRAYED);
      EnableMenuItem(hWndMenu, CMD_PASTEUPLOAD, MF_GRAYED);
      }
    EnableMenuItem(hWndMenu, CMD_EXECUTE, MF_ENABLED);
    EnableMenuItem(hWndMenu, CMD_KERMIT, MF_ENABLED);
    EnableMenuItem(hWndMenu, CMD_PLAYBACK, MF_ENABLED);
    EnableMenuItem(hWndMenu, CMD_COMMUNICATIONS, MF_ENABLED);
    CheckMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_UNCHECKED); // 00040
    EnableMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_GRAYED);   // 00040
    }
}


void XTalkSize(lParam)
  LONG lParam;
{
  static int nOldHeight = 0;
  int  nTop, nBot, XPos, nCurHeight;
  register x;


  nTop = 0;
  nBot = HIWORD(lParam);
  XPos = LOWORD(lParam);

  if (nBot == 0)
    return;

  nCurHeight = nBot;

  x = nSysFontHeight + 2;

  if (hChildStatus != NULL)
    {
    if (Disp.fShowStatusBar)
      {
      MoveWindow(hChildStatus, 0, 0, XPos, x+3, TRUE);
      if (nCurHeight < nOldHeight)
	InvalidateRect(hChildStatus, NULL, TRUE);
      nTop += (x+3);
      }
    else
      MoveWindow(hChildStatus, 0, 0, 0, 0, FALSE);
    }

  if (hChildInfo != NULL)
    {
    if (Disp.fShowInfoBar)
      {
      MoveWindow(hChildInfo, 0, nBot - (x+2), XPos, x+2, TRUE);
      if (nCurHeight < nOldHeight)
	InvalidateRect(hChildInfo, NULL, TRUE);
      nBot -= (x+2);
      }
    else
      MoveWindow(hChildInfo, 0, 0, 0, 0, FALSE);
    }

  if (hChildMacro != NULL)
    {
    if (Disp.fShowMacroBar)
      {
      if (cMacroRows == 1)
	x = (nSysFontHeight * 2) + 2;
      else if (cMacroRows == 2)
	x = (nSysFontHeight * 4) + 3;
      else if (cMacroRows == 3)
        x = (nSysFontHeight * 6) + 4;
      else if (cMacroRows == 4)
        x = (nSysFontHeight * 8) + 5;
      else
	x = 0;

      if (x > 0)
	{
	MoveWindow(hChildMacro, 0, nBot - x, XPos, x, TRUE);
	if (nCurHeight < nOldHeight)
	  InvalidateRect(hChildMacro, NULL, TRUE);
	nBot -= x;
	}
      else
	MoveWindow(hChildMacro, 0, 0, 0, 0, FALSE);
      x = nSysFontHeight + 2;
      }
    else
      MoveWindow(hChildMacro, 0, 0, 0, 0, FALSE);
    }

    if (hKermitChild && bKermitNonServer == FALSE)
      {
      // 00077 Prevent InputLine and KCP conflict.
      if (hChildInput)
	MoveWindow(hChildInput,  0, 0, 0, 0, FALSE);

      MoveWindow(hKermitChild, 0, nBot - (x * 2) - 1, XPos, (x * 2) + 1, TRUE);
      UpdateWindow(hChildMacro);
      InvalidateRect(hKermitChild, NULL, TRUE);
      nBot -= (x * 2) + 1;
      }
    else if (hChildInput != NULL)
      {
      if (Disp.fShowInput)
	{
	// MKL fixed Input line height problem
	// MoveWindow(hChildInput, 0, nBot - x - 1, XPos, x + 2, TRUE);
	MoveWindow(hChildInput, 0, nBot - (x+5) , XPos, (x+6), TRUE);
	InvalidateRect(hChildInput, NULL, TRUE);
	// MKL fixed Input line height problem
	// nBot -= x + 1;
	nBot -= x + 5;
	}
      else
	{
	MoveWindow(hChildInput,  0, 0, 0, 0, FALSE);
	}
      }

    if (hChildTerminal)
      {
      MoveWindow(hChildTerminal, 0, nTop, XPos, nBot - nTop, TRUE);
   /* InvalidateRect(hChildTerminal, NULL, TRUE); */
      VidUpdateSize();
   /* UpdateWindow(hChildTerminal); */
      }

  nOldHeight = nCurHeight;
}


void XTalkPaint(hWnd)
  HWND hWnd;
{
  PAINTSTRUCT ps;
  LOGFONT     LogFont;
  TEXTMETRIC  TM;
  BITMAP Bitmap;
  HBRUSH hBrush;
  HANDLE hBitmap, hOldBitmap;
  HDC	 hMemoryDC;
  DWORD  rgbOld;
  RECT	 Rect;
//RECT   ClientRect, TextRect;
  HANDLE hIcon;
  HANDLE hLogFont, hLogFontOld;
  int	 xClient, yClient, XPos;
  int	 x, y;

  register i;


  BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

  if (IsIconic(hWnd))
    {
    hBrush = CreateSolidBrush(GetSysColor(COLOR_BACKGROUND));
    FillRect(ps.hdc, (LPRECT)&ps.rcPaint, hBrush);
    DeleteObject(hBrush);

    x = GetSystemMetrics(SM_CXICON);
    y = GetSystemMetrics(SM_CYICON);

    GetClientRect(hWnd, (LPRECT)&Rect);
    XPos = ((Rect.right - Rect.left) - x) / 2;
    yClient = ((Rect.bottom - Rect.top) - y) / 2;

    // dca00074 MKL added a new icon for win3.0
    // hIcon = LoadIcon(hWndInst, MAKEINTRESOURCE(IDI_ICON));
    if (sVersion < 3) hIcon = LoadIcon(hWndInst, MAKEINTRESOURCE(IDI_ICON));
    else hIcon = LoadIcon(hWndInst, MAKEINTRESOURCE(IDI_ICON2));
    DrawIcon(ps.hdc, XPos, yClient, hIcon);

    // dca00060 MKL added Win3.0 Icon
    if (sVersion < 3) {
    nIconTextX = XPos;
    nIconTextY = yClient + y / 8 - 1;
    TextOut(ps.hdc, nIconTextX, nIconTextY, szIconText, 4);

    MoveTo(ps.hdc, XPos - 1, yClient + (y / 16) * 1 - 1);
    LineTo(ps.hdc, XPos - 1, yClient + (y / 16) * 9 + 1);

    MoveTo(ps.hdc, XPos + x, yClient + (y / 16) * 1 - 1);
    LineTo(ps.hdc, XPos + x, yClient + (y / 16) * 9 + 1);
    }
    }
  else if (wSystem == SS_INIT)
    {
    hBitmap = LoadBitmap(hWndInst, MAKEINTRESOURCE(IDB_TITLE1));
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    hMemoryDC = CreateCompatibleDC(ps.hdc);
    hOldBitmap = SelectObject(hMemoryDC, hBitmap);

    FillRect(ps.hdc, (LPRECT)&ps.rcPaint, GetStockObject(GRAY_BRUSH));
    GetClientRect(hWnd, &Rect);

    ps.rcPaint.left   = (Rect.right - Bitmap.bmWidth) / 2 - 4;
    ps.rcPaint.right  = ps.rcPaint.left + Bitmap.bmWidth + 8;
    ps.rcPaint.top    = 41;
    ps.rcPaint.bottom = ps.rcPaint.top + Bitmap.bmHeight + 6;
    FillRect(ps.hdc, &ps.rcPaint, GetStockObject(BLACK_BRUSH));

    BitBlt(ps.hdc, (Rect.right - Bitmap.bmWidth) / 2,
                   44,
                   Bitmap.bmWidth,
                   Bitmap.bmHeight,
                   hMemoryDC, 0, 0, SRCCOPY);
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    }
  else
    {
    FillRect(ps.hdc, (LPRECT)&ps.rcPaint, GetStockObject(WHITE_BRUSH));
    }

  EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
}


#define MAX_DIALOGS 16
extern HWND Dialogs[MAX_DIALOGS];
extern int  DialogCnt;

void LinkDialog(hWnd)
  HWND hWnd;
{
  if (DialogCnt != MAX_DIALOGS)
    Dialogs[DialogCnt++] = hWnd;
}

void UnLinkDialog(hWnd)
  HWND hWnd;
{
  register i;

  for (i = 0; i < DialogCnt; i++)
    {
    if (Dialogs[i] == hWnd)
      {
      Dialogs[i] == NULL;
      DialogCnt--;
      break;
      }
    }

  for (i = 0; i < MAX_DIALOGS - 1; i++)
    {
    if (Dialogs[i] == NULL)
      {
      Dialogs[i] == Dialogs[i+1];
      Dialogs[i+1] == NULL;
      }
    }

}
