/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: NRes Routines (PRONRES.C)                                        */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dde.h>
#include "lexis.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "term.h"
#include "data.h"


static void near pascal UpdateMenus();


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
  else if (wSystem & (SS_ABORT | SS_INIT))
    wSystemState = SS_NULL;
  else
    wSystemState = SS_TERMINAL;

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
  else if (wSystem & (SS_ABORT | SS_INIT))
    wSystemState = SS_NULL;
  else
    wSystemState = SS_TERMINAL;

  UpdateMenus();

  SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);
  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}

static void near pascal UpdateMenus()
{
  if (wSystemState != SS_TERMINAL)
    {
    }
}


void WinLexisSize(lParam)
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
    if (!fDataDisplayStatus)
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
    if (!fDataDisplayInfo)
      {
      MoveWindow(hChildInfo, 0, nBot - x, XPos, x, TRUE);
      if (nCurHeight < nOldHeight)
	InvalidateRect(hChildInfo, NULL, TRUE);
      nBot -= x;
      }
    else
      MoveWindow(hChildInfo, 0, 0, 0, 0, FALSE);
    }

  if (hChildMacro != NULL)
    {
    if (!fDataDisplayMacro)
      {
      x = nSysFontHeight * 5;
      MoveWindow(hChildMacro, 0, nBot - x, XPos, x, TRUE);
      nBot -= x;
      }
    else
      MoveWindow(hChildMacro, 0, 1000, 8, 8, FALSE);
    }

  if (hChildTerminal)
    {
    MoveWindow(hChildTerminal, 0, nTop, XPos, nBot - nTop, TRUE);
    VidUpdateSize();
    UpdateWindow(hChildTerminal);
    }

  nOldHeight = nCurHeight;
}


void WinLexisPaint(hWnd)
  HWND hWnd;
{
  PAINTSTRUCT ps;
  LOGFONT     LogFont;
  TEXTMETRIC  TM;
  BITMAP Bitmap;
  HBRUSH hBrush;
  HANDLE hBitmap, hOldBitmap;
  HANDLE hResData, hMF;
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
    }
  else if (wSystem == SS_INIT)
    {
    hBitmap = LoadBitmap(hWndInst, MAKEINTRESOURCE(IDB_TITLE));
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
