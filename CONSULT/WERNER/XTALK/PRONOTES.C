/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Note Processor Module (PRONOTES.C)                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the code to process the notes dialogs and  *
*             supports it's features.                                         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00040 Permit hiding of the xfer dialog window.    *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include "xtalk.h"
#include "prodata.h"
#include "dialogs.h"
#include "disp.h"


  FARPROC lpNoteDlg;
  HWND	  hNoteDlg = NULL;
  char	  szNotes[1024];


  BOOL DlgNotes(HWND, unsigned, WORD, LONG);

void CmdNotes()
  {
  ToggleNotes();
  }

void ToggleNotes()
{
  if (hNoteDlg)
    {
    DestroyWindow(hNoteDlg);
    UnLinkDialog(hNoteDlg);
    hNoteDlg = NULL;
    CheckMenuItem(hWndMenu, CMD_WINDOW_NOTES, MF_UNCHECKED);  // 00040
    }
  else
    {
    lpNoteDlg = MakeProcInstance((FARPROC)DlgNotes, hWndInst);
    hNoteDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_NOTES), hWnd, lpNoteDlg);
    LinkDialog(hNoteDlg);
 // SetFocus(hWnd);
    CheckMenuItem(hWndMenu, CMD_WINDOW_NOTES, MF_CHECKED);  // 00040
    }
}


BOOL DlgNotes(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  HWND hCtl;
  char buf[64];


  switch (message)
    {
    case WM_INITDIALOG:
      hCtl = GetDlgItem(hDlg, ID_EDIT);
      SendMessage(hCtl, EM_LIMITTEXT, sizeof(szNotes), 0L);
      SendMessage(hCtl, WM_SETTEXT, 0, (LONG)((LPSTR)szNotes));

      GetWindowText(hDlg, buf, sizeof(buf));
      strcat(buf, szDataCurFile);
      SetWindowText(hDlg, buf);

      CheckDlgButton(hDlg, ID_RECALL, Disp.fDisplayNotes);

      SetFocus(GetDlgItem(hDlg, ID_CANCEL));
      return FALSE;
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
	  hCtl = GetDlgItem(hDlg, ID_EDIT);
          SendMessage(hCtl, WM_CHAR, '\r', 0L);
	  break;

	case ID_SAVE:
	  hCtl = GetDlgItem(hDlg, ID_EDIT);
	  SendMessage(hCtl, WM_GETTEXT, sizeof(szNotes), (LONG)((LPSTR)szNotes));
	  Disp.fDisplayNotes = (char)IsDlgButtonChecked(hDlg, ID_RECALL);

	  DestroyWindow(hNoteDlg);
	  UnLinkDialog(hNoteDlg);
	  hNoteDlg = NULL;
	  break;

	case ID_COPY:
	  SendMessage(GetDlgItem(hDlg, ID_EDIT), WM_COPY, 0, 0L);
	  break;

	case ID_CUT:
	  SendMessage(GetDlgItem(hDlg, ID_EDIT), WM_CUT, 0, 0L);
	  break;

	case ID_PASTE:
	  SendMessage(GetDlgItem(hDlg, ID_EDIT), WM_PASTE, 0, 0L);
	  break;

	case IDCANCEL:
          ToggleNotes();
	  break;

        case ID_EDIT:
          if (HIWORD(lParam) == EN_CHANGE)
            fDataChanged = TRUE;
          break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_USER:
      hCtl = GetDlgItem(hDlg, ID_EDIT);
      if (wParam)
	{
	SendMessage(hCtl, WM_SETTEXT, 0, (LONG)((LPSTR)szNotes));
	GetWindowText(hDlg, buf, sizeof(buf));
        *(PSTR)(strchr(buf, '-') + 2) = NULL;
	strcat(buf, szDataCurFile);
	SetWindowText(hDlg, buf);
	}
      else
	SendMessage(hCtl, WM_GETTEXT, sizeof(szNotes), (LONG)((LPSTR)szNotes));
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}
