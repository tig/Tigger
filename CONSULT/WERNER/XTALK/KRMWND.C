/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit KCP Window Module (KRMWND.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module processes the Kermit KCP child window.              *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00040 Permit hiding of the xfer dialog window.    *
*	      1.01 11/07/89 00057 Make ESC key work in KCP.		      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "xfer.h"
#include "kermit.h"
#include "dialogs.h"

#define WND_EXTRA	10
#define UWW_FROG1	 0
#define UWW_EDIT	 2
#define UWW_ABORT	 4
#define UWW_FNADDR	 6


  long WndKermitProc(HWND, unsigned, WORD, long);
  long SubKermitProc(HWND, unsigned, WORD, long);


    HWND hKermitChild = 0;		/* Kermit Command Window	    */


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL CreateKermitChild(hParent)
  HWND	 hParent;
{
  static char fRegistered = FALSE;
  char	szClass[32];


  LoadString(hWndInst, IDS_WNDCLASS_KERMIT, szClass, sizeof(szClass));

  if (!fRegistered)
    {
    WNDCLASS WndClass;


    memset(&WndClass, NULL, sizeof(WNDCLASS));

    WndClass.style	       = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc       = WndKermitProc;
    WndClass.hInstance	       = hWndInst;
    WndClass.hbrBackground     = GetStockObject(WHITE_BRUSH);
    WndClass.lpszClassName     = (LPSTR)szClass;
    WndClass.cbWndExtra        = WND_EXTRA;

    RegisterClass(&WndClass);  /* We just ignore the error here */
    fRegistered = TRUE;
    }

  hKermitChild = CreateWindow(szClass, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
			      hParent, NULL, hWndInst, NULL);

  if (hKermitChild)
    {
    ResetScreenDisplay();
    return (TRUE);
    }
  else
    {
    return (FALSE);
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void DestroyKermitChild()
  {
  if (hKermitChild)
    {
    DestroyWindow(hKermitChild);
    hKermitChild = 0;
    ResetScreenDisplay();
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
long WndKermitProc(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  long lParam;
{

  switch (message)
    {
    case WM_COMMAND:
      if (LOWORD(lParam) == GetWindowWord(hWnd, UWW_ABORT))
	KermitUserAbort();
      break;

    case WM_CREATE:
      {
      HWND    hChild;
      HBITMAP hBitmap;
      FARPROC lpfnProc;
      char    szClass[32], szTitle[32];


      hKermitChild = hWnd;

      hBitmap = LoadBitmap(hWndInst, MAKEINTRESOURCE(IDB_FROG1));
      SetWindowWord(hWnd, UWW_FROG1, hBitmap);

      szClass[0] = 'b';
      szClass[1] = 'u';
      szClass[2] = 't';
      szClass[3] = 't';
      szClass[4] = 'o';
      szClass[5] = 'n';
      szClass[6] = NULL;

      szTitle[0] = 'C';
      szTitle[1] = 'a';
      szTitle[2] = 'n';
      szTitle[3] = 'c';
      szTitle[4] = 'e';
      szTitle[5] = 'l';
      szTitle[6] = NULL;

      hChild = CreateWindow(szClass, szTitle,
			    BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			    0, 0, 0, 0, hWnd, NULL, hWndInst, NULL);

      if (hChild == 0)
	{
	DestroyWindow(hWnd);
	break;
	}
      SetWindowWord(hWnd, UWW_ABORT, hChild);

      szClass[0] = 'e';
      szClass[1] = 'd';
      szClass[2] = 'i';
      szClass[3] = 't';
      szClass[4] = NULL;

      hChild = CreateWindow(szClass, NULL,
			    ES_LEFT | WS_BORDER | WS_CHILD | WS_VISIBLE,
			    0, 0, 0, 0, hWnd, NULL, hWndInst, NULL);

      if (hChild == 0)
	{
	DestroyWindow(hWnd);
	break;
	}
      SetWindowWord(hWnd, UWW_EDIT, hChild);


      SetWindowLong(hWnd, UWW_FNADDR, GetWindowLong(hChild, GWL_WNDPROC));
      lpfnProc = MakeProcInstance((FARPROC)SubKermitProc, hWndInst);
      SetWindowLong(hChild, GWL_WNDPROC, (LONG)lpfnProc);
      }
      break;

    case WM_DESTROY:
      {
      HWND    hChild;
      HBITMAP hBitmap;

      hBitmap = GetWindowWord(hWnd, UWW_FROG1);
      DeleteObject(hBitmap);

      if (hChild = GetWindowWord(hWnd, UWW_EDIT))
	{
	FreeProcInstance((FARPROC)GetWindowLong(hChild, GWL_WNDPROC));
	SetWindowLong(hChild, GWL_WNDPROC, GetWindowLong(hWnd, UWW_FNADDR));
	}
      }
      break;

    case WM_PAINT:
      {
      PAINTSTRUCT ps;
      RECT	  Rect;
      HBITMAP	  hOldBitmap;
      HDC	  hMemoryDC;


      GetClientRect(hWnd, &Rect);

      BeginPaint(hWnd, &ps);

      MoveTo(ps.hdc, ps.rcPaint.left, 0);
      LineTo(ps.hdc, ps.rcPaint.right+1, 0);

      hMemoryDC  = CreateCompatibleDC(ps.hdc);

      hOldBitmap = SelectObject(hMemoryDC, GetWindowWord(hWnd, UWW_FROG1));

      StretchBlt(ps.hdc, 2, 1, nSysFontWidth * 4, Rect.bottom - 5,
		 hMemoryDC, 0, 0, 24, 24, SRCCOPY);

      SelectObject(hMemoryDC, hOldBitmap);
      DeleteDC(hMemoryDC);

      SelectObject(ps.hdc, GetStockObject(BLACK_BRUSH));
      Rectangle(ps.hdc, nSysFontWidth * 5, 10,
		     Rect.right - nSysFontWidth * 10 - 1, Rect.bottom - 3);

      EndPaint(hWnd, &ps);
      }
      break;


    case WM_SIZE:
      {
      HWND hChild;

      hChild = GetWindowWord(hWnd, UWW_ABORT);
      MoveWindow(hChild, LOWORD(lParam) - nSysFontWidth * 10, 5,
		 nSysFontWidth * 10, HIWORD(lParam) - 9, TRUE);

      hChild = GetWindowWord(hWnd, UWW_EDIT);
      MoveWindow(hChild,
		 nSysFontWidth * 4 + nSysFontWidth / 2,
		 nSysFontHeight / 2 - 2,
		 LOWORD(lParam) - nSysFontWidth * 15,
		 HIWORD(lParam) - (nSysFontWidth + 2),
		 TRUE);
      }
      break;

    case WM_SETFOCUS:
      if (filexfer.kermit.hFile == -1 && hXferDlg)
        {
        if (!fHideXferWindow)
          ShowWindow(hXferDlg, SW_HIDE);
        }
      SetFocus(GetWindowWord(hWnd, UWW_EDIT));
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
long SubKermitProc(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  FARPROC lpfnProc;
  char buf[84];


  switch (message)
    {
    case WM_CHAR:
      if (wParam == '\r' || wParam == 26)
	{
	register i;

	SendMessage(hWnd, WM_GETTEXT, sizeof(buf), (LONG)((LPSTR)buf));

	/*
	i = strlen(buf);
	if (GetKeyState(VK_SHIFT) >= 0)
	  {
	  buf[i++] = (char)wParam;
	  buf[0]   = NULL;
	  }
	*/

	i = KermitCommand(buf, KRMSRC_USER);

	if (nKermitMode == KM_BUSY)
	  i = MSGKRM_PROCESSING;
	else if (i == 0)
	  i = MSGKRM_NOERROR;
	else
	  i = IDS_ERRKRM + (i & 0xFF);

        LoadString(hWndInst, i, buf, sizeof(buf));
        TermWriteData("Kermit: ", 8);
        LineToTerminal(buf);

	buf[0] = NULL;
	SendMessage(hWnd, WM_SETTEXT, 0, (LONG)((LPSTR)buf));

	return 0L;
	}
      else if (wParam == ASC_ESC)
	{
	// 00057 Make ESC key work in KCP.
	CmdKermit();
	return 0L;
	}
      break;
    }

  lpfnProc = (FARPROC)GetWindowLong(hKermitChild, UWW_FNADDR);
  return CallWindowProc(lpfnProc, hWnd, message, wParam, lParam);
}
