/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Search Support Module (SEARCH.C)                                 */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <string.h>
#include "lexis.h"
#include "data.h"
#include "session.h"
#include "term.h"
#include "dialogs.h"


/******************************************************************************/
/*									      */
/*  Local Constants							      */
/*									      */
/******************************************************************************/

#define MAX_BUFFER      512     /* Size of file buffer for our search i/o  */


/******************************************************************************/
/*									      */
/*  Local Procedures Prototypes 					      */
/*									      */
/******************************************************************************/

  BOOL DlgSearchLevel(HWND, unsigned, WORD, LONG);
  BOOL DlgSearchModify(HWND, unsigned, WORD, LONG);



void CmdSearchLevel()
  {
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgSearchLevel, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SEARCH_LEVEL), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
  }


void CmdSearchModify()
  {
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgSearchModify, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SEARCH_MODIFY), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
  }


BOOL DlgSearchLevel(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (IsDlgButtonChecked(hDlg, ID_SEARCH_SUMMARY))
            SessCanCommand(COMMAND_SEARCHDISPLAY);
          else
            {
            char buf[16];
            register i;

            GetDlgItemText(hDlg, ID_SEARCH_LEVEL, buf, sizeof(buf));
            i = atoi(buf);
            if (i > 255)
              i = 255;
            SessIntCommand(COMMAND_DISPDIF, i);
            }

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_SEARCH_LEVEL:
          if (HIWORD(lParam) == EN_CHANGE)
            CheckDlgButton(hDlg, ID_SEARCH_SUMMARY, FALSE);
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


BOOL DlgSearchModify(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      CheckDlgButton(hDlg, ID_SEARCH_MODIFY, TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (IsDlgButtonChecked(hDlg, ID_SEARCH_MODIFY))
            SessCanCommand(COMMAND_MODIFY);
          else
            {
            register i;

            i = GetDlgItemInt(hDlg, ID_SEARCH_LEVEL, 0, 0);
            if (i > 255)
              i = 255;
            SessIntCommand(COMMAND_MODIFY, i);
            }

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_SEARCH_LEVEL:
          if (HIWORD(lParam) == EN_CHANGE)
            CheckDlgButton(hDlg, ID_SEARCH_MODIFY, FALSE);
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
