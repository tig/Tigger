/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Input Line (INPUT.C)                                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the child window that processes the input  *
*             line during input mode.                                         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "interp.h"
#include "term.h"
#include "xfer.h"
#include "dialogs.h"

  HWND hChildInput  = NULL;
  long InputSubClass(HWND, unsigned, WORD, LONG);

static int nWidth;
static FARPROC lpfnOldInput;


BOOL CreateInputChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  FARPROC lpfnInput;
  FARPROC lpfnPrompt;
  char szCntl[8], buf[1];


  szCntl[0] = 'e';
  szCntl[1] = 'd';
  szCntl[2] = 'i';
  szCntl[3] = 't';
  szCntl[4] = NULL;

  hWnd = CreateWindow(szCntl,
		      NULL,
		      WS_CHILD | WS_VISIBLE | WS_BORDER,
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
    hChildInput = hWnd;
    SendMessage(hWnd, EM_LIMITTEXT, 80, 0L);
    buf[0] = NULL;
    SendMessage(hWnd, WM_SETTEXT, 0, (LONG)((LPSTR)buf));

    lpfnInput = MakeProcInstance((FARPROC)InputSubClass, hWndInst);
    lpfnOldInput = (FARPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)lpfnInput);
    }
  else
    {
    hChildInput = NULL;
    return FALSE;
    }
}


BOOL DestroyInputChild()
{
  if (hChildInput)
    DestroyWindow(hChildInput);

  hChildInput  = NULL;

  return TRUE;
}


long InputSubClass(hWnd, message, wParam, lParam)
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char buf[84];

  switch (message)
    {
    case WM_CHAR:
      if (wParam == '\r' || wParam == 26)
	{
	register i;

        if (IfSystemStatus(SS_SCRIPT))
          {
          register j;

	  SendMessage(hWnd, WM_GETTEXT, sizeof(buf), (LONG)((LPSTR)buf));
	  i = strlen(buf);
          buf[i] = (char)wParam;
          i++;

          for (j = 0; j < i; j++)
            InterpreterKeyboard(buf[j], TRUE);

	  buf[0] = NULL;
	  SendMessage(hWnd, WM_SETTEXT, 0, (LONG)((LPSTR)buf));
          }
        else  if (bConnected)
	  {
	  SendMessage(hWnd, WM_GETTEXT, sizeof(buf), (LONG)((LPSTR)buf));
	  i = strlen(buf);
	  if (GetKeyState(VK_SHIFT) >= 0)
	    {
	    buf[i] = (char)wParam;
	    i++;
	    }
	  CommDataOutput(buf, i);
	  buf[0] = NULL;
	  SendMessage(hWnd, WM_SETTEXT, 0, (LONG)((LPSTR)buf));
	  }
        else
          {
	  buf[0] = NULL;
	  SendMessage(hWnd, WM_SETTEXT, 0, (LONG)((LPSTR)buf));
	  MessageBeep(NULL);
          }

	return 0L;
	}
      break;
    }

  return CallWindowProc(lpfnOldInput, hWnd, message, wParam, lParam);
}
