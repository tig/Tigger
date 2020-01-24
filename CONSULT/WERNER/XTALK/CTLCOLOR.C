/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Color Bar Control Window (CTLCOLOR.C)			      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports a color bar control window that is used    *
*	      within the application.					      *
*                                                                             *
*  Revisions: 1.00 10/17/89 Initial Version.				      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "ctlcolor.h"
#include "dialogs.h"


//
//  Local defines, typedefs and macros.
//
#define DATA_EXTRA	 4
#define UW_SELECTION	 0
#define UW_CAPTURE	 2


//
//  Prototypes.
//
LONG CtlColorBarProc(HWND hWnd, unsigned message, WORD wParam, LONG lParam);
static void near DrawSelector(HWND hWnd, HDC hDC);
static void near DrawCaret(HWND hWnd);


//
//  Set the current color selection.
//
VOID SetColorBar(HWND hCtl, USHORT usSelect)
  {
  if (usSelect > 7)
    usSelect = 0;

  SendMessage(hCtl, WM_USER, usSelect, 0L);
  }


//
//  Get the current color selection.
//
VOID GetColorBar(HWND hCtl, PUSHORT pusSelect)
  {
  *pusSelect = (USHORT)GetWindowWord(hCtl, UW_SELECTION);
  }


//
//  Register the color bar control window so we can use it in dialogs.
//
BOOL RegisterColorBarControl(HANDLE hWndInst)
  {
  WNDCLASS WndClass;
  char szClassName[64];


  memset(&WndClass, 0, sizeof(WNDCLASS));
  LoadString(hWndInst, IDS_WNDCLASS_COLORBAR, szClassName, sizeof(szClassName));

  WndClass.style	     = CS_HREDRAW | CS_VREDRAW;
  WndClass.lpfnWndProc	     = CtlColorBarProc;
  WndClass.hInstance	     = hWndInst;
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1);
  WndClass.lpszClassName     = (LPSTR)szClassName;
  WndClass.cbWndExtra	     = DATA_EXTRA;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return (FALSE);

  return (TRUE);
  }


//
//  The Color Bar Control Procedural Logic
//
LONG CtlColorBarProc(HWND hWnd, unsigned message, WORD wParam, LONG lParam)
  {
  switch (message)
    {
    case WM_CREATE:
      SetWindowWord(hWnd, UW_SELECTION, 0);
      SetWindowWord(hWnd, UW_CAPTURE, 0);
      break;

    case WM_GETDLGCODE:
      return (DLGC_WANTARROWS);
      break;

    case WM_KEYDOWN:
      {
      HDC hDC;
      USHORT usSelection;

      hDC = GetDC(hWnd);
      DrawSelector(hWnd, hDC);

      usSelection = GetWindowWord(hWnd, UW_SELECTION);

      switch (wParam)
	{
	case VK_HOME:
	  usSelection = 0;
	  break;

	case VK_END:
	  usSelection = 7;
	  break;

	case VK_LEFT:
	  usSelection = (usSelection == 0) ? 7 : usSelection - 1;
	  break;

	case VK_RIGHT:
	  usSelection = (usSelection == 7) ? 0 : usSelection + 1;
	  break;
	}

      SetWindowWord(hWnd, UW_SELECTION, usSelection);

      DrawSelector(hWnd, hDC);
      ReleaseDC(hWnd, hDC);

      DrawCaret(hWnd);
      }
      break;

    case WM_PAINT:
      {
      PAINTSTRUCT ps;
      RECT   rClient;
      HBRUSH hBrush, hOldBrush;
      USHORT usWidth;


      GetClientRect(hWnd, &rClient);
      usWidth = rClient.right / 8;

      BeginPaint(hWnd, &ps);

      hBrush	= CreateSolidBrush(RGB(0x00, 0x00, 0x00));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 0 + 4, 4, usWidth * 1 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0x00, 0x00, 0xFF));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 1 + 4, 4, usWidth * 2 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0x00, 0xFF, 0x00));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 2 + 4, 4, usWidth * 3 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0xFF, 0x00, 0x00));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 3 + 4, 4, usWidth * 4 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0x00, 0xFF, 0xFF));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 4 + 4, 4, usWidth * 5 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0xFF, 0xFF, 0x00));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 5 + 4, 4, usWidth * 6 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0xFF, 0x00, 0xFF));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 6 + 4, 4, usWidth * 7 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      hBrush	= CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
      hOldBrush = SelectObject(ps.hdc, hBrush);
      Rectangle(ps.hdc, usWidth * 7 + 4, 4, usWidth * 8 - 4, rClient.bottom - 4);
      SelectObject(ps.hdc, hOldBrush);
      DeleteObject(hBrush);

      DrawSelector(hWnd, ps.hdc);
      EndPaint(hWnd, &ps);
      }
      break;

    case WM_SETFOCUS:
      {
      RECT rClient;

      GetClientRect(hWnd, &rClient);
      CreateCaret(hWnd, NULL, rClient.right / 8 - 3, 2);
      DrawCaret(hWnd);
      ShowCaret(hWnd);
      }
      break;

    case WM_LBUTTONDOWN:
      {
      HDC  hDC;
      RECT rClient;

      hDC = GetDC(hWnd);
      GetClientRect(hWnd, &rClient);

      DrawSelector(hWnd, hDC);
      SetWindowWord(hWnd, UW_SELECTION, LOWORD(lParam) / (rClient.right / 8));
      DrawSelector(hWnd, hDC);

      ReleaseDC(hWnd, hDC);

      if (hWnd == SetFocus(hWnd))
	DrawCaret(hWnd);

      SetCapture(hWnd);
      SetWindowWord(hWnd, UW_CAPTURE, TRUE);
      }
      break;

    case WM_MOUSEMOVE:
      {
      HDC    hDC;
      RECT   rClient;
      SHORT sSelection;

      if (GetWindowWord(hWnd, UW_CAPTURE))
	{
	GetClientRect(hWnd, &rClient);

	sSelection = LOWORD(lParam) / (rClient.right / 8);
	if (sSelection < 0)
	  sSelection = 0;
	if (sSelection > 7)
	  sSelection = 7;

	if (GetWindowWord(hWnd, UW_SELECTION) != sSelection)
	  {
	  hDC = GetDC(hWnd);

	  DrawSelector(hWnd, hDC);
	  SetWindowWord(hWnd, UW_SELECTION, sSelection);
	  DrawSelector(hWnd, hDC);

	  ReleaseDC(hWnd, hDC);
	  }
	}
      }
      break;

    case WM_LBUTTONUP:
      DrawCaret(hWnd);
      if (GetWindowWord(hWnd, UW_CAPTURE))
	{
	SetWindowWord(hWnd, UW_CAPTURE, 0);
	ReleaseCapture();
	}
      break;

    case WM_KILLFOCUS:
      DestroyCaret();
      break;

    case WM_USER:
      SetWindowWord(hWnd, UW_SELECTION, wParam);
      InvalidateRect(hWnd, NULL, TRUE);
      break;

    default:
      return (DefWindowProc(hWnd, message, wParam, lParam));
      break;
    }

  return (0L);
  }


//
//
//
static void near DrawSelector(HWND hWnd, HDC hDC)
  {
  RECT	 rClient;
  HANDLE hOldPen, hOldBrush;
  USHORT usOldROP2, usOldBkMode;
  USHORT usHeight, usWidth;


  GetClientRect(hWnd, &rClient);
  usHeight = rClient.bottom;
  usWidth  = rClient.right / 8;

  usOldROP2   = SetROP2(hDC, R2_NOT);
  usOldBkMode = SetBkMode(hDC, TRANSPARENT);

  hOldPen   = SelectObject(hDC, CreatePen(0, 2, RGB(0, 0, 0)));
  hOldBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));

  Rectangle(hDC, GetWindowWord(hWnd, UW_SELECTION) * usWidth + 1,
	    1, (GetWindowWord(hWnd, UW_SELECTION) + 1) * usWidth, usHeight);

  SelectObject(hDC, hOldBrush);
  SetROP2(hDC, usOldROP2);
  SetBkMode(hDC, usOldBkMode);
  }


//
//  Draw the Caret around the currently selected color.
//
static void near DrawCaret(HWND hWnd)
  {
  RECT rClient;


  GetClientRect(hWnd, &rClient);

  SetCaretPos((rClient.right / 8) * GetWindowWord(hWnd, UW_SELECTION) + 3,
	      rClient.bottom - 4);
  }
