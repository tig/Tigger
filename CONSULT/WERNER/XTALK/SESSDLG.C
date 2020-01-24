/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Session Dialog (SESSDLG.C)                                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports the session dialog logic.                  *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 10/15/89 00045 Fix the problem of tabbing into the non-    *
*				    displayed area on the dialog.	      *
*	      1.01 10/31/89 00052 Add context help to the menus and dialogs.  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include "xtalk.h"
#include "library.h"
#include "sess.h"
#include "term.h"
#include "global.h"
#include "prodata.h"
#include "variable.h"
#include "dialogs.h"


  BOOL DlgSession(HWND, unsigned, WORD, LONG);


void CmdSession()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_SESSION;

  lpDlg = MakeProcInstance((FARPROC)DlgSession, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SESSION), hWnd, lpDlg) == -1)
    GeneralError(ERRSYS_NOMEMORY);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


BOOL DlgSession(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char szPassword[sizeof(Sess.szPassword)];
  register int i;


  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemLength(hDlg, ID_SESSION_DESCRIPTION, sizeof(Sess.szDescription)-1);
      SetDlgItemLength(hDlg, ID_SESSION_NUMBER, sizeof(Sess.szNumber)-1);
      SetDlgItemLength(hDlg, ID_SESSION_MODIFIER, sizeof(Sess.szModifier)-1);
      SetDlgItemLength(hDlg, ID_SESSION_USERID, sizeof(Sess.szUserid)-1);
      SetDlgItemLength(hDlg, ID_SESSION_PASSWORD, sizeof(Sess.szPassword)-1);
      SetDlgItemLength(hDlg, ID_SESSION_SCRIPT, sizeof(Sess.szScript)-1);
      SetDlgItemLength(hDlg, ID_SESSION_NETID, sizeof(Sess.szNetID)-1);
      SetDlgItemLength(hDlg, ID_SESSION_LOG, sizeof(Sess.szLog)-1);
      SetDlgItemLength(hDlg, ID_SESSION_ANSWERBACK, sizeof(Term.szAnswerback)-1);

      SetDlgItemLength(hDlg, ID_SESSION_PATIENCE, 3);
      SetDlgItemLength(hDlg, ID_SESSION_REDIALCOUNT, 2);
      SetDlgItemLength(hDlg, ID_SESSION_REDIALWAIT, 3);

      SetDlgItemData(hDlg, ID_SESSION_DESCRIPTION, Sess.szDescription);
      SetDlgItemData(hDlg, ID_SESSION_NUMBER, Sess.szNumber);
      SetDlgItemData(hDlg, ID_SESSION_MODIFIER, Sess.szModifier);
      SetDlgItemData(hDlg, ID_SESSION_USERID, Sess.szUserid);
      SetDlgItemData(hDlg, ID_SESSION_ANSWERBACK, Term.szAnswerback);

      if (!fSecret)
	SetDlgItemData(hDlg, ID_SESSION_PASSWORD, Sess.szPassword);
      else
	{
	szPassword[0] = '(';
	szPassword[1] = 'S';
	szPassword[2] = 'e';
	szPassword[3] = 'c';
	szPassword[4] = 'r';
	szPassword[5] = 'e';
	szPassword[6] = 't';
	szPassword[7] = ')';
	szPassword[8] = NULL;
	SetDlgItemText(hDlg, ID_SESSION_PASSWORD, szPassword);
	}

      SetDlgItemData(hDlg, ID_SESSION_SCRIPT, Sess.szScript);
      SetDlgItemData(hDlg, ID_SESSION_NETID, Sess.szNetID);
      SetDlgItemData(hDlg, ID_SESSION_LOG, Sess.szLog);

      SetDlgItemInt(hDlg, ID_SESSION_PATIENCE, Sess.nPatience, 0);
      SetDlgItemInt(hDlg, ID_SESSION_REDIALCOUNT, Sess.nRedialCount, 0);
      SetDlgItemInt(hDlg, ID_SESSION_REDIALWAIT, Sess.nRedialWait, 0);

      if (Sess.cLogOption == SESSION_NORMAL)
	i = ID_SESSION_NORMAL;
      else if (Sess.cLogOption == SESSION_RAW)
	i = ID_SESSION_RAW;
      else if (Sess.cLogOption == SESSION_VISUAL)
	i = ID_SESSION_VISUAL;
      else
	i = ID_SESSION_MANUAL;
      CheckDlgButton(hDlg, i, TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i  = GetDlgItemInt(hDlg, ID_SESSION_PATIENCE, 0, TRUE);
          if (i < 10 || i > 999)
	    {
	    ErrorMessage(IDS_ERRDLG_PATIENCE);
	    break;
	    }
          i  = GetDlgItemInt(hDlg, ID_SESSION_REDIALCOUNT, 0, TRUE);
          if (i < 0 || i > 99)
	    {
	    ErrorMessage(IDS_ERRDLG_REDIALCOUNT);
	    break;
	    }
          i  = GetDlgItemInt(hDlg, ID_SESSION_REDIALWAIT, 0, TRUE);
          if (i < 0 || i > 99)
	    {
	    ErrorMessage(IDS_ERRDLG_REDIALWAIT);
	    break;
	    }

	  GetDlgItemData(hDlg, ID_SESSION_DESCRIPTION, Sess.szDescription,
			 sizeof(Sess.szDescription));
	  GetDlgItemData(hDlg, ID_SESSION_NUMBER, Sess.szNumber,
			 sizeof(Sess.szNumber));
	  GetDlgItemData(hDlg, ID_SESSION_MODIFIER, Sess.szModifier,
			 sizeof(Sess.szModifier));
	  GetDlgItemData(hDlg, ID_SESSION_USERID, Sess.szUserid,
			 sizeof(Sess.szUserid));

	  GetDlgItemText(hDlg, ID_SESSION_PASSWORD, szPassword,
			 sizeof(szPassword));
	  if (!fSecret || szPassword[0] != '(')
	    {
            GetDlgItemData(hDlg, ID_SESSION_PASSWORD, Sess.szPassword,
                           sizeof(Sess.szPassword));
	    }

	  GetDlgItemData(hDlg, ID_SESSION_SCRIPT, Sess.szScript,
			 sizeof(Sess.szScript));
	  GetDlgItemData(hDlg, ID_SESSION_NETID, Sess.szNetID,
			 sizeof(Sess.szNetID));
	  GetDlgItemData(hDlg, ID_SESSION_LOG, Sess.szLog,
			 sizeof(Sess.szLog));

	  Sess.nPatience    = GetDlgItemInt(hDlg, ID_SESSION_PATIENCE, 0, 0);
	  Sess.nRedialCount = GetDlgItemInt(hDlg, ID_SESSION_REDIALCOUNT, 0, 0);
	  Sess.nRedialWait  = GetDlgItemInt(hDlg, ID_SESSION_REDIALWAIT, 0, 0);

	  if (IsDlgButtonChecked(hDlg, ID_SESSION_NORMAL))
	    Sess.cLogOption = SESSION_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_SESSION_RAW))
	    Sess.cLogOption = SESSION_RAW;
	  else if (IsDlgButtonChecked(hDlg, ID_SESSION_VISUAL))
	    Sess.cLogOption = SESSION_VISUAL;
	  else
	    Sess.cLogOption = SESSION_MANUAL;

          GetDlgItemData(hDlg, ID_SESSION_ANSWERBACK, Term.szAnswerback,
			 sizeof(Term.szAnswerback));

	  fDataChanged = TRUE;

	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_MORE:
	  {
	  RECT Rect;

	  GetWindowRect(hDlg, &Rect);
          Rect.right -= Rect.left;
          Rect.bottom = 25 * nSysFontHeight;
          if (Rect.top + Rect.bottom > nVertRes)
            Rect.top = (nVertRes - Rect.bottom) / 2;
          if (Rect.left + Rect.right > nHorzRes)
            Rect.left = (nHorzRes - Rect.right) / 2;

          MoveWindow(hDlg, Rect.left, Rect.top, Rect.right, Rect.bottom, TRUE);

	  // 00045 Enable tabbing to the now showable area of the dialog.
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_PATIENCE), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_REDIALCOUNT), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_REDIALWAIT), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_LOG), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_NORMAL), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_ANSWERBACK), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_SESSION_MODIFIER), TRUE);

          SetFocus(GetDlgItem(hDlg, ID_SESSION_PATIENCE));
          EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
	  }
	  break;

	case ID_SESSION_NORMAL:
	case ID_SESSION_RAW:
	case ID_SESSION_VISUAL:
	case ID_SESSION_MANUAL:
	  CheckRadioButton(hDlg, ID_SESSION_NORMAL, ID_SESSION_MANUAL, wParam);
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
