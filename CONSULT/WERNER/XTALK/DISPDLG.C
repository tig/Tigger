/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Display Dialog (DISPDLG.C)                                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This modula contains the display dialog logic.                  *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.                                             *
*	1.01 10/15/89 00045    Fix the problem of tabbing into the non-    		   *
*	                       displayed area on the dialog.	      			   *
*	1.01 10/31/89 00052    Add context help to the menus and dialogs.  		   *
*  1.01 02/03/90 dca00017 PJL Fixed incorrect help text for Setup.Display.    *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "disp.h"
#include "term.h"
#include "prodata.h"


  BOOL DlgDisplay(HWND, unsigned, WORD, LONG);
  static void near UserButtonPaint(HWND, HWND, int, int);
  static void near Normalize(void);
  static void near Restore(void);


void CmdDisplay()
  {
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
// 02/03/90 dca00017 PJL Incorrect help text for Setup.Display.
// wHelpDialog = HELP_ITEM_DEVICE;
  wHelpDialog = HELP_ITEM_DISPLAY;

  lpDlg = MakeProcInstance((FARPROC)DlgDisplay, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_DISPLAY), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
  }


BOOL DlgDisplay(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static char fShowStatusBar;
  static char fShowMacroBar;
  static char fShowInfoBar;
  static char fShowVScrollBar;
  static char fShowHScrollBar;
  static char fShowInput;
  char buf[32];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      fShowStatusBar  = Disp.fShowStatusBar;
      fShowMacroBar   = Disp.fShowMacroBar;
      fShowInfoBar    = Disp.fShowInfoBar;
      fShowVScrollBar = Disp.fShowVScroll;
      fShowHScrollBar = Disp.fShowHScroll;
      fShowInput      = Disp.fShowInput;

      CheckDlgButton(hDlg, ID_DISPLAY_STATUS,  Disp.fShowStatusBar);
      CheckDlgButton(hDlg, ID_DISPLAY_MACRO,   Disp.fShowMacroBar);
      CheckDlgButton(hDlg, ID_DISPLAY_INFO,    Disp.fShowInfoBar);
      CheckDlgButton(hDlg, ID_DISPLAY_VERT,    Disp.fShowVScroll);
      CheckDlgButton(hDlg, ID_DISPLAY_HORZ,    Disp.fShowHScroll);
      CheckDlgButton(hDlg, ID_DISPLAY_INPUT,   Disp.fShowInput);
      CheckDlgButton(hDlg, ID_DISPLAY_NOBLINK, Disp.fNoBlink);
      CheckDlgButton(hDlg, ID_DISPLAY_ONECHAR, Disp.fOneChar);
      CheckDlgButton(hDlg, ID_DISPLAY_SLOWCUR, Disp.fSlowCur);

      for (i = IDS_CHRATTR_BLACK; i <= IDS_CHRATTR_UNDERLINED; i++)
        {
        if (fMonoDisplay && i >= IDS_CHRATTR_RED && i <= IDS_CHRATTR_CYAN)
          continue;

	LoadString(hWndInst, i, buf, sizeof(buf));
	if (i < IDS_CHRATTR_BOLDED)
	  {
	  SendDlgItemMessage(hDlg, ID_DISPLAY_FOREGROUND, LB_ADDSTRING, 0,
			     (DWORD)((LPSTR)buf));
	  SendDlgItemMessage(hDlg, ID_DISPLAY_BACKGROUND, LB_ADDSTRING, 0,
			     (DWORD)((LPSTR)buf));
	  }
	SendDlgItemMessage(hDlg, ID_DISPLAY_BOLDING, LB_ADDSTRING, 0,
			   (DWORD)((LPSTR)buf));
	SendDlgItemMessage(hDlg, ID_DISPLAY_BLINKING, LB_ADDSTRING, 0,
			   (DWORD)((LPSTR)buf));
        }

      Normalize();
      SendDlgItemMessage(hDlg, ID_DISPLAY_FOREGROUND, LB_SETCURSEL,
                         Disp.nForeground, 0L);
      SendDlgItemMessage(hDlg, ID_DISPLAY_BACKGROUND, LB_SETCURSEL,
                         Disp.nBackground, 0L);
      SendDlgItemMessage(hDlg, ID_DISPLAY_BOLDING, LB_SETCURSEL,
                         Disp.nBolding, 0L);
      SendDlgItemMessage(hDlg, ID_DISPLAY_BLINKING, LB_SETCURSEL,
                         Disp.nBlinking, 0L);
      Restore();
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	  Disp.fShowStatusBar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_STATUS);
	  Disp.fShowMacroBar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_MACRO);
	  Disp.fShowInfoBar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_INFO);
	  Disp.fShowVScroll = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_VERT);
	  Disp.fShowHScroll = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_HORZ);
	  Disp.fShowInput = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_INPUT);
	  Disp.fOneChar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_ONECHAR);
	  Disp.fSlowCur = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_SLOWCUR);
	  Disp.fNoBlink = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_NOBLINK);

	  Disp.nForeground = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_FOREGROUND,
					   LB_GETCURSEL, 0, 0L);
	  Disp.nBackground = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_BACKGROUND,
					   LB_GETCURSEL, 0, 0L);
	  Disp.nBolding  = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_BOLDING,
					 LB_GETCURSEL, 0, 0L);
	  Disp.nBlinking = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_BLINKING,
					 LB_GETCURSEL, 0, 0L);
          Restore();

	  fDataChanged = TRUE;
	  SetTextColor(hTerminalDC, ChrAttrColor[Disp.nForeground]);
	  SetBkColor(hTerminalDC, ChrAttrColor[Disp.nBackground]);
	  ResetScreenDisplay();

	  EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
	  Disp.fShowStatusBar = fShowStatusBar;
	  Disp.fShowMacroBar  = fShowMacroBar;
	  Disp.fShowInfoBar   = fShowInfoBar;
	  Disp.fShowVScroll   = fShowVScrollBar;
	  Disp.fShowHScroll   = fShowHScrollBar;
	  Disp.fShowInput     = fShowInput;
	  ResetScreenDisplay();

	  EndDialog(hDlg, FALSE);
	  break;

	case ID_MORE:
          {
	  RECT Rect;

	  GetWindowRect(hDlg, &Rect);
          Rect.right -= Rect.left;
          Rect.bottom = 23 * nSysFontHeight;
          if (Rect.top + Rect.bottom > nVertRes)
            Rect.top = (nVertRes - Rect.bottom) / 2;
          if (Rect.left + Rect.right > nHorzRes)
            Rect.left = (nHorzRes - Rect.right) / 2;

          MoveWindow(hDlg, Rect.left, Rect.top, Rect.right, Rect.bottom, TRUE);

	  // 00045 Enable tabbing to the now showable area of the dialog.
	  EnableWindow(GetDlgItem(hDlg, ID_DISPLAY_FOREGROUND), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_DISPLAY_BACKGROUND), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_DISPLAY_BOLDING), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_DISPLAY_BLINKING), TRUE);

	  SetFocus(GetDlgItem(hDlg, ID_DISPLAY_FOREGROUND));
          EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
	  }
	  break;

	case ID_DISPLAY_STATUS:
	case ID_DISPLAY_MACRO:
	case ID_DISPLAY_INFO:
	case ID_DISPLAY_VERT:
	case ID_DISPLAY_HORZ:
	case ID_DISPLAY_INPUT:
	  if (HIWORD(lParam) == BN_CLICKED)
	    {
	    Disp.fShowStatusBar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_STATUS);
	    Disp.fShowMacroBar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_MACRO);
	    Disp.fShowInfoBar = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_INFO);
	    Disp.fShowVScroll = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_VERT);
	    Disp.fShowHScroll = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_HORZ);
	    Disp.fShowInput = (char)IsDlgButtonChecked(hDlg, ID_DISPLAY_INPUT);
	    ResetScreenDisplay();
	    }
	  break;

	case ID_DISPLAY_FOREGROUND:
	  if (HIWORD(lParam) == LBN_SELCHANGE)
	    InvalidateRect(GetDlgItem(hDlg, ID_DISPLAY_TEXT), NULL, FALSE);
	  else
	    return FALSE;

	case ID_DISPLAY_BACKGROUND:
	  if (HIWORD(lParam) == LBN_SELCHANGE)
	    {
	    InvalidateRect(GetDlgItem(hDlg, ID_DISPLAY_TEXT), NULL, FALSE);
	    InvalidateRect(GetDlgItem(hDlg, ID_DISPLAY_BOLD), NULL, FALSE);
	    InvalidateRect(GetDlgItem(hDlg, ID_DISPLAY_BLINK), NULL, FALSE);
	    }
	  else
	    return FALSE;
	  break;

	case ID_DISPLAY_BOLDING:
	  if (HIWORD(lParam) == LBN_SELCHANGE)
	    InvalidateRect(GetDlgItem(hDlg, ID_DISPLAY_BOLD), NULL, FALSE);
	  else
	    return FALSE;
	  break;

	case ID_DISPLAY_BLINKING:
	  if (HIWORD(lParam) == LBN_SELCHANGE)
	    InvalidateRect(GetDlgItem(hDlg, ID_DISPLAY_BLINK), NULL, FALSE);
	  else
	    return FALSE;
	  break;

	case ID_DISPLAY_TEXT:
	  if (HIWORD(lParam) == BN_PAINT)
	    {
	    i = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_FOREGROUND, LB_GETCURSEL, 0, 0L);
	    UserButtonPaint(hDlg, (HWND)LOWORD(lParam), 0, i);
	    }
	  break;

	case ID_DISPLAY_BOLD:
	  if (HIWORD(lParam) == BN_PAINT)
	    {
	    i = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_BOLDING, LB_GETCURSEL, 0, 0L);
	    UserButtonPaint(hDlg, (HWND)LOWORD(lParam), 1, i);
	    }
	  break;

	case ID_DISPLAY_BLINK:
	  if (HIWORD(lParam) == BN_PAINT)
	    {
	    i = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_BLINKING, LB_GETCURSEL, 0, 0L);
	    UserButtonPaint(hDlg, (HWND)LOWORD(lParam), 2, i);
	    }
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}

static void near UserButtonPaint(hDlg, hCtl, nType, nAttr)
  HWND hDlg;
  HWND hCtl;
  int  nType;
  int  nAttr;
{
  HBRUSH hBrush;
  HFONT  hFont, hOldFont;
  RECT	 Rect;
  HDC	 hDC;
  char	 buf[32], szFace[32];
  register i;


  GetClientRect(hCtl, &Rect);
  hDC = GetDC(hCtl);

  if (nType == 0)
    LoadString(hWndInst, IDS_CHRATTR_SAMPLE, buf, sizeof(buf));
  else if (nType == 1)
    LoadString(hWndInst, IDS_CHRATTR_BOLDED, buf, sizeof(buf));
  else
    LoadString(hWndInst, IDS_CHRATTR_BLINKING, buf, sizeof(buf));

  i = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_BACKGROUND, LB_GETCURSEL, 0, 0L);
  if (fMonoDisplay && i)
    i += 6;

  SetBkColor(hDC, ChrAttrColor[i]);
  hBrush = CreateSolidBrush(ChrAttrColor[i]);
  FillRect(hDC, &Rect, hBrush);
  DeleteObject(hBrush);

  if (fMonoDisplay && nAttr)
    nAttr += 6;

  if (nAttr <= TERM_ATTR_WHITE)
    {
    SetTextColor(hDC, ChrAttrColor[nAttr]);
    DrawText(hDC, buf, -1, &Rect, DT_CENTER | DT_NOCLIP | DT_VCENTER);
    }
  else
    {
    GetTextFace(hDC, sizeof(szFace), szFace);
    hFont = CreateFont(0, 0, 0, 0, ((nAttr != 8) ? 400 : 700), 0, (BYTE)(nAttr == 9),
		       0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		       DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, szFace);
    hOldFont = SelectObject(hDC, hFont);

    i = (int)SendDlgItemMessage(hDlg, ID_DISPLAY_FOREGROUND, LB_GETCURSEL, 0, 0L);
    SetTextColor(hDC, ChrAttrColor[i]);
    DrawText(hDC, buf, -1, &Rect, DT_CENTER | DT_NOCLIP | DT_VCENTER);

    SelectObject(hDC, hOldFont);
    DeleteObject(hFont);
    }

  ReleaseDC(hCtl, hDC);
}

static void near Normalize()
  {
  if (fMonoDisplay)
    {
    if (Disp.nForeground > 0)
      Disp.nForeground -= 6;
    if (Disp.nForeground < 0)
      Disp.nForeground = 0;

    if (Disp.nBackground > 0)
      Disp.nBackground -= 6;
    if (Disp.nBackground < 0)
      Disp.nBackground = 0;

    if (Disp.nBolding > 0)
      Disp.nBolding -= 6;
    if (Disp.nBolding < 0)
      Disp.nBolding = 0;

    if (Disp.nBlinking > 0)
      Disp.nBlinking -= 6;
    if (Disp.nBlinking < 0)
      Disp.nBlinking = 0;
    }
  }

static void near Restore()
  {
  if (fMonoDisplay)
    {
    if (Disp.nForeground > 0)
      Disp.nForeground += 6;
    if (Disp.nBackground > 0)
      Disp.nBackground += 6;
    if (Disp.nBolding > 0)
      Disp.nBolding += 6;
    if (Disp.nBlinking > 0)
      Disp.nBlinking += 6;
    }
  }
