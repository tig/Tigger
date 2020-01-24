/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Extra CIS Features Module (PROCIS.C)                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports the CIS Hi-res and Med-res graphics.       *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/29/89 Add Invert command to CIS graphic window.         *
*             1.01 01/19/90 00093 Make sure the clipboard is already empty.   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "capture.h"
#include "dialogs.h"
#include "term.h"
#include "xfer.h"
#include "variable.h"

  extern int cdecl rand(void);

static BOOL near InputCisGH(HWND, char *, int);
static int nWidth;
static int nHeight;


BOOL InitCisGH(azBuf, nChrCnt, fHigh)
  char *azBuf;
  int  nChrCnt;
  char fHigh;
{
  HWND hChildWnd;
  char szCisWindow[32];
  RECT Rect;


  if (fHigh)
    {
    nWidth  = 256;
    nHeight = 192;
    }
  else
    {
    nWidth  = 128;
    nHeight = 96;
    }

  Rect.left   = 0;
  Rect.top    = 0;
  Rect.right  = nWidth + 1;
  Rect.bottom = nHeight + 1;

  AdjustWindowRect((LPRECT)&Rect, WS_POPUPWINDOW, TRUE);

  LoadString(hWndInst, IDS_WNDCLASS_CISG, szCisWindow, sizeof(szCisWindow));

  hChildWnd = CreateWindow(szCisWindow,
                      (LPSTR)"CIS Graphics",
		      WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION | WS_SIZEBOX,
		      32 + rand() % 32,
		      16 + rand() % 16,
		      nWidth + nWidth / 2,
		      nHeight + nHeight / 2,
		      (HWND)hWnd,	   /* use parent */
		      (HMENU)NULL,	   /* use class menu */
		      (HANDLE)hWndInst,    /* handle to window instance */
		      (LPSTR)NULL	   /* no params to pass on */
		      );

  /* Make window visible according to the way the app is activated */

  UpdateWindow(hChildWnd);

  if (InputCisGH(hChildWnd, azBuf, nChrCnt) == FALSE)
    ResetSystemStatus(SS_XFER);

  return TRUE;
}


long CisWndProc(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  PAINTSTRUCT ps;
  HDC	      hDC, hMemDC;
  HANDLE      hBitmap, hOldBitmap;
  HANDLE      hBuffer;
  LPSTR       lpBuffer;
  RECT	      ClientRect;
  long size;
  int  x, y;
  char azBuf[256];
  int  nChrCnt;
  register i;


  switch (message)
    {
    case WM_CREATE:
      hXferDlg = hWnd;

      SetSystemStatus(SS_XFER);
      bTermEscape   = FALSE;
      nTermInx	    = 0;
      nSaveChrX     = 0;
      nSaveChrY     = 0;

      hDC    = GetDC(hWnd);
      hMemDC = CreateCompatibleDC(hDC);
      ReleaseDC(hWnd, hDC);

      hBitmap	    = CreateBitmap(nWidth, nHeight, 1, 1, 0L);
      hOldBitmap    = SelectObject(hMemDC, hBitmap);

      ClientRect.left	= 0;
      ClientRect.top	= 0;
      ClientRect.right	= nWidth;
      ClientRect.bottom = nHeight;

      FillRect(hMemDC, (LPRECT)&ClientRect, GetStockObject(WHITE_BRUSH));

      SetWindowWord(hWnd, 0, hMemDC);
      SetWindowWord(hWnd, 2, hOldBitmap);
      SetWindowWord(hWnd, 4, hBitmap);
      SetWindowWord(hWnd, 6, TRUE);
      SetWindowWord(hWnd, 8, nWidth);
      SetWindowWord(hWnd,10, nHeight);
      break;

    case WM_SYSCOMMAND:
      switch (wParam)
	{
	default:
	  return DefWindowProc(hWnd, message, wParam, lParam);
	  break;
	}
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case CMD_CISG_COPY:
	  if (OpenClipboard(hWnd) == FALSE)
	    break;

          // 00093 Make sure the clipboard is already empty.
          EmptyClipboard();

	  x = GetWindowWord(hWnd, 8);
	  y = GetWindowWord(hWnd,10);
	  size = (x / 8) * y;

          if ((hBuffer = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_LOWER|GMEM_ZEROINIT, size)) == NULL)
	   {
	   CloseClipboard();
           GeneralError(ERRSYS_NOMEMORY);
	   break;
	   }

	  lpBuffer = GlobalLock(hBuffer);
	  GetBitmapBits(GetWindowWord(hWnd, 4), size, lpBuffer);
	  hBitmap = CreateBitmap(x, y, 1, 1, 0L);
	  SetBitmapBits(hBitmap, size, lpBuffer);

	  SetClipboardData(CF_BITMAP, hBitmap);
	  CloseClipboard();

	  GlobalUnlock(hBuffer);
	  GlobalFree(hBuffer);
	  break;

        case CMD_CISG_INVERT:
          hMemDC = GetWindowWord(hWnd, 0);
          x      = GetWindowWord(hWnd, 8);
          y      = GetWindowWord(hWnd,10);

          BitBlt(hMemDC, 0, 0, x, y, NULL, 0, 0, DSTINVERT);
          InvalidateRect(hWnd, NULL, FALSE);
          break;

	default:
	  return DefWindowProc(hWnd, message, wParam, lParam);
	}
      break;

    case WM_TIMER:
      if (hDemoFile != -1)
	{
	i = DosRead(hDemoFile, azBuf, 64, &nChrCnt);
	if (i || nChrCnt == 0)
	  {
	  DosClose(hDemoFile);
	  hDemoFile = -1;
	  }
	}
      else
	{
	GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	if ((nChrCnt = ReadComm(nCommID, (LPSTR)azBuf, sizeof(azBuf))) < 0)
	  nChrCnt = -nChrCnt;
	}

      if (nChrCnt > 0)
	{
	lTimeLastRecv = GetCurrentTime();

	WriteRawToCapture(azBuf, nChrCnt);

	if (InputCisGH(hWnd, azBuf, nChrCnt) == FALSE)
	  {
	  SetWindowWord(hWnd, 6, FALSE);
	  ResetSystemStatus(SS_XFER);
	  }
	}
      break;

    case WM_PAINT:
      BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
      hMemDC = GetWindowWord(hWnd, 0);
      x      = GetWindowWord(hWnd, 8);
      y      = GetWindowWord(hWnd,10);

      GetClientRect(hWnd,(LPRECT)&ClientRect);
      StretchBlt(ps.hdc, ClientRect.left, ClientRect.top,
			 ClientRect.right - ClientRect.left,
			 ClientRect.bottom - ClientRect.top,
			 hMemDC, 0, 0, x, y, SRCCOPY);

      EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
      break;

    case WM_DESTROY:
      if (GetWindowWord(hWnd, 6) != FALSE)
	{
	ResetSystemStatus(SS_XFER);
	}
      SetWindowWord(hWnd, 6, FALSE);
      hMemDC  = GetWindowWord(hWnd, 0);
      hBitmap = GetWindowWord(hWnd, 2);

      DeleteObject(SelectObject(hMemDC, hBitmap));
      DeleteDC(hMemDC);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}


static BOOL near InputCisGH(hWnd, azBuf, nChrCnt)
  HWND hWnd;
  char *azBuf;
  int  nChrCnt;
{
  HDC  hMemDC;
  char buf[8];
  int  x, y;
  int  i;


  hMemDC = GetWindowWord(hWnd, 0);
  x	 = GetWindowWord(hWnd, 8);
  y	 = GetWindowWord(hWnd,10);

  while (nChrCnt-- > 0)
    {
    if (bTermEscape)
      {
      cTermBuf[nTermInx++] = *azBuf;
      switch (cTermBuf[0])
	{
        case 'G':
	  if (nTermInx > 1)
	    {
	    bTermEscape = FALSE;
	    nTermInx	= 0;
	    InvalidateRect(hWnd, NULL, FALSE);
	    if (nChrCnt > 0)
	      SendMessage(hWnd, UM_DATA, nChrCnt, MAKELONG(azBuf+1, 0));
	    return FALSE;
	    }
	  break;

        case 'Y':
	  if (nTermInx > 2)
	    {
	    nSaveChrY	= cTermBuf[1] - 31;
	    nSaveChrX	= cTermBuf[2] - 31;
	    bTermEscape = FALSE;
	    nTermInx	= 0;
	    }
	  break;

	default:
	  bTermEscape = FALSE;
	  nTermInx    = 0;
	  break;
	}
      }
    else
      {
      switch (*azBuf)
	{
	case ASC_BEL:
          buf[0] = '\r';
	  CommDataOutput(buf, 1);
	  break;

	case ASC_ESC:
	  bTermEscape = TRUE;
	  nTermInx    = 0;
	  break;

	default:
	  if (nTermInx == 1)
	    {
	    nSaveChrX += *azBuf - 32;
	    if (nSaveChrX >= x)
	      {
	      nSaveChrX %= x;
	      nSaveChrY++;
	      }
	    nTermInx = 0;
	    }
	  else
	    {
	    i = *azBuf - 32;
	    while (i-- > 0)
	      {
	      MoveTo(hMemDC, nSaveChrX, nSaveChrY);

	      if (nSaveChrX + i < x)
		{
		nSaveChrX += i;
		i = 0;
		}

	      nSaveChrX++;
	      LineTo(hMemDC, nSaveChrX, nSaveChrY);

	      if (nSaveChrX >= x)
		{
		nSaveChrX %= x;
		nSaveChrY++;
		}
	      }
	    nTermInx = 1;
	    }
	  break;
	}
      }

    azBuf++;
    }

  InvalidateRect(hWnd, NULL, FALSE);
  return TRUE;
}
