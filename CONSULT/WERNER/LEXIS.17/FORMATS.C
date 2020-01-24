/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Format Support Module (FORMATS.C)                                */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include "lexis.h"
#include "data.h"
#include "session.h"
#include "dialogs.h"


  BOOL DlgVarKwic(HWND, unsigned, WORD, LONG);


void CmdCite()
{
}


void CmdVarKwic()
{
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgVarKwic, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_VARKWIC), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


BOOL DlgVarKwic(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, ID_EDIT, nWorkLastVarKwic, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          nWorkLastVarKwic = GetDlgItemInt(hDlg, ID_EDIT, 0L, FALSE);
          if (nWorkLastVarKwic < 1) nWorkLastVarKwic = 1;
          if (nWorkLastVarKwic > 999) nWorkLastVarKwic = 999;
          SessIntCommand(COMMAND_VARKWIC, nWorkLastVarKwic);

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
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
