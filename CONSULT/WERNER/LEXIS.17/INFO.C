/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Info Routine (INFO.C)                                            */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <memory.h>
#include <time.h>
#include "library.h"
#include "lexis.h"
#include "comm.h"
#include "data.h"
#include "term.h"
#include "dialogs.h"
#include "diary.h"
#include "dde.h"
#include "record.h"


  HMENU hInfoFileMenu;
  HMENU hInfoEditMenu;
  HMENU hInfoDispMenu;
  HMENU hInfoScanMenu;
  HMENU hInfoChngMenu;
  HMENU hInfoOptsMenu;
  HMENU hInfoCiteMenu;
  HMENU hInfoInfoMenu;
  HMENU hInfoHelpMenu;

  WORD wInfoMessage = 0;
  char sInfoMessage[32];
  char sInfoStatus[32];

  HWND hChildInfo = NULL;
  char fMenuMode;

  static WORD wMenuItem;
  static int  nKeysPos;
  static int  nTimePos;
  static RECT TextRect;

  static void near pascal InfoDisp(HWND, HDC);
  static void near GenerateStatus(HWND);


void InfoMessage(sMsg, pArg)
  SHORT sMsg;
  PSTR  pArg;
{
  wInfoMessage = sMsg;

  if (pArg)
    {
    if (strlen(pArg) < sizeof(sInfoMessage))
      strcpy(sInfoMessage, pArg);
    else
      {
      memcpy(sInfoMessage, pArg, sizeof(sInfoMessage)-4);
      sInfoMessage[sizeof(sInfoMessage)-4] = '.';
      sInfoMessage[sizeof(sInfoMessage)-3] = '.';
      sInfoMessage[sizeof(sInfoMessage)-2] = '.';
      sInfoMessage[sizeof(sInfoMessage)-1] = NULL;
      }
    }
  else
    sInfoMessage[0] = NULL;

  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}


long InfoWndProc(hWnd, message, wParam, lParam )
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  PAINTSTRUCT ps;
  HMENU hMenu;
  char	szBuf[32];
  long	lTime;
  int	i, nXPos;


  switch (message)
    {
    case UM_INFOMSG:
      {
      HDC hDC;

      hDC = GetDC(hWnd);
      InfoDisp(hWnd, hDC);
      ReleaseDC(hWnd, hDC);
      }
      break;

    case WM_CREATE:
      fMenuMode = FALSE;
      wMenuItem = NULL;
      sInfoMessage[0] = NULL;
      memset(sInfoStatus, ' ', sizeof(sInfoStatus));

      hMenu	    = GetMenu(GetParent(hWnd));
      hInfoFileMenu = GetSubMenu(hMenu, 0);
      hInfoEditMenu = GetSubMenu(hMenu, 1);
      hInfoDispMenu = GetSubMenu(hMenu, 2);
      hInfoScanMenu = GetSubMenu(hMenu, 3);
      hInfoChngMenu = GetSubMenu(hMenu, 4);
      hInfoOptsMenu = GetSubMenu(hMenu, 5);
      hInfoCiteMenu = GetSubMenu(hMenu, 6);
      hInfoInfoMenu = GetSubMenu(hMenu, 7);
      hInfoHelpMenu = GetSubMenu(hMenu, 8);
      break;

    case WM_MENUSELECT:
    //{
    //char buf[80];
    //sprintf(buf, "%x %x %x", wParam, HIWORD(lParam), LOWORD(lParam));
    //LineToTerminal(buf);
    //}
      if (LOWORD(lParam) == 16)
        break;
      if ((wParam & 0xF000) || LOWORD(lParam) == 0xFFFF)
	fMenuMode = FALSE;
      else
	fMenuMode = TRUE;
      if (wMenuItem != wParam)
	{
        HDC hDC;

	if ((LOWORD(lParam) & (MF_SYSMENU|MF_POPUP)) == (MF_SYSMENU|MF_POPUP))
	  wMenuItem = -1;
	else
	  wMenuItem = wParam;
	hDC = GetDC(hWnd);
        InfoDisp(hWnd, hDC);
	ReleaseDC(hWnd, hDC);
	}
      break;

    case WM_SIZE:
      {
      HDC hDC;

      hDC = GetDC(hWnd);
      nTimePos        = LOWORD(lParam) - LOWORD(GetTextExtent(hDC, "00:00:000", 9));
    //nTimePos        = LOWORD(lParam) - 9 * nSysFontWidth;
      nKeysPos        = nTimePos - LOWORD(GetTextExtent(hDC, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 26));
    //nKeysPos        = nTimePos - 26 * nSysFontWidth;
      TextRect.top    = 1;
      TextRect.bottom = HIWORD(lParam);
      TextRect.left   = 0;
      TextRect.right  = nKeysPos - 2 * nSysFontWidth;
      ReleaseDC(hWnd, hDC);
      }
      break;

    case WM_TIMER:
      GenerateStatus(hWnd);
      break;

    case WM_PAINT:
      {
      RECT  Rect;
      DWORD dwColor;

      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);

      MoveTo(ps.hdc, ps.rcPaint.left, 0);
      LineTo(ps.hdc, ps.rcPaint.right, 0);

      InfoDisp(hWnd, ps.hdc);

      MoveTo(ps.hdc, nKeysPos - nSysFontWidth, ps.rcPaint.top);
      LineTo(ps.hdc, nKeysPos - nSysFontWidth, ps.rcPaint.bottom);

      GetClientRect(hWnd, &Rect);
      Rect.left  = nKeysPos;
      Rect.right = nTimePos;
      dwColor = SetTextColor(ps.hdc, RGB(255, 0, 0));
      DrawText(ps.hdc, sInfoStatus, 24, &Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      SetTextColor(ps.hdc, dwColor);

      MoveTo(ps.hdc, nTimePos - nSysFontWidth, ps.rcPaint.top);
      LineTo(ps.hdc, nTimePos - nSysFontWidth, ps.rcPaint.bottom);

      _strdate(szBuf);
      GetClientRect(hWnd, &Rect);
      Rect.left  = nTimePos;
      DrawText(ps.hdc, szBuf, 8, &Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      }
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}

static void near pascal InfoDisp(HWND hWnd, HDC hDC)
{
  RECT Rect;
  char szBuf[64];
  int  i;


  GetClientRect(hWnd, &Rect);
  Rect.top   = 1;
  Rect.left  = nSysFontWidth / 2;
  Rect.right = nKeysPos - nSysFontWidth * 2;

  FillRect(hDC, &Rect, GetStockObject(COLOR_WINDOW+1));

  i = 0;
  if (fMenuMode)
    {
    if (wMenuItem == -1)
      i = INFO_SYSBOX;
    else if (wMenuItem == hInfoFileMenu)
      i = INFO_MFILE;
    else if (wMenuItem == hInfoEditMenu)
      i = INFO_EDIT;
    else if (wMenuItem == hInfoDispMenu)
      i = INFO_SEARCH;
    else if (wMenuItem == hInfoScanMenu)
      i = INFO_VIEW;
    else if (wMenuItem == hInfoChngMenu)
      i = INFO_FORMATS;
    else if (wMenuItem == hInfoOptsMenu)
      i = INFO_CITATORS;
    else if (wMenuItem == hInfoCiteMenu)
      i = INFO_DELIVERY;
    else if (wMenuItem == hInfoInfoMenu)
      i = INFO_INFO;
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
          if (wMenuItem >= CMD_NEW && wMenuItem <= CMD_ABOUT)
            i = wMenuItem - CMD_NEW + INFO_NEW;
          else
            i = 0;
          break;
	}
      }

    if (i)
      {
      LoadString(hWndInst, i, szBuf, sizeof(szBuf));
      DrawText(hDC, szBuf, strlen(szBuf), &Rect, DT_SINGLELINE | DT_VCENTER);
      }
    else
      goto here;
    }
  else
    {
here:
    if (wSystem & SS_DDE)
      {
      strcpy(szBuf, "DDE Mode...");
      DrawText(hDC, szBuf, strlen(szBuf), &Rect, DT_SINGLELINE | DT_VCENTER);
      }
    else
      {
      char buf[96];

      if (wInfoMessage)
        {
        if (sInfoMessage[0])
          {
          char fmt[96];

          LoadString(hWndInst, wInfoMessage, fmt, sizeof(buf));
          sprintf(buf, fmt, sInfoMessage);
          }
        else
          {
          LoadString(hWndInst, wInfoMessage, buf, sizeof(buf));
          }
        }
      else
        LoadString(hWndInst, IDS_INFO_OFFLINE, buf, sizeof(buf));

      DrawText(hDC, buf, strlen(buf), &Rect, DT_SINGLELINE | DT_VCENTER);
      }
    }
}

static void near GenerateStatus(HWND hWnd)
  {
  char  szBuf[32];


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
  else if (wSystem & SS_SCRIPT)
    {
    szBuf[ 8] = 'E';
    szBuf[ 9] = 'X';
    szBuf[10] = 'E';
    szBuf[11] = 'C';
    }

  if (bPrinterOn)
    {
    szBuf[13] = 'P';
    szBuf[14] = 'R';
    }

  if (bRecording)
    {
    szBuf[16] = 'R';
    szBuf[17] = 'C';
    }

  if (bDiary)
    {
    szBuf[19] = 'D';
    szBuf[20] = 'Y';
    }

  if (fCommCarrier)
    {
    szBuf[22] = 'C';
    szBuf[23] = 'D';
    }

  if (memcmp(sInfoStatus, szBuf, 24))
    {
    HDC  hDC;
    RECT Rect;


    hDC = GetDC(hWnd);
    SetTextColor(hDC, RGB(255, 0, 0));
    GetClientRect(hWnd, &Rect);
    Rect.left  = nKeysPos;
    Rect.right = nTimePos;
    DrawText(hDC, szBuf, 24, &Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    ReleaseDC(hWnd, hDC);

    memcpy(sInfoStatus, szBuf, 24);
    }
}

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
