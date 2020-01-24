/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Buttons Support Module (BUTTONS.C)                               */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include "lexis.h"
#include "session.h"
#include "data.h"
#include "term.h"
#include "library.h"
#include "dialogs.h"


  BOOL DlgNextPage(HWND, unsigned, WORD, LONG);
  BOOL DlgNextDoc(HWND, unsigned, WORD, LONG);
  BOOL DlgCitator(HWND, unsigned, WORD, LONG);
  BOOL DlgPrevPage(HWND, unsigned, WORD, LONG);
  BOOL DlgPrevDoc(HWND, unsigned, WORD, LONG);
  BOOL DlgGotoPage(HWND, unsigned, WORD, LONG);
  BOOL DlgGotoDoc(HWND, unsigned, WORD, LONG);
  BOOL DlgPrintAll(HWND, unsigned, WORD, LONG);



void CmdNextPage(BOOL fAskUser)
{
  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  if (fAskUser)
    {
    FARPROC lpDlg;

    lpDlg = MakeProcInstance((FARPROC)DlgNextPage, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEXTPAGE), hWnd, lpDlg);
    FreeProcInstance(lpDlg);
    }
  else
    SessIntCommand(COMMAND_NEXTPAGE, -1);
}


void CmdNextDoc(BOOL fAskUser)
{
  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  if (fAskUser)
    {
    FARPROC lpDlg;

    lpDlg = MakeProcInstance((FARPROC)DlgNextDoc, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEXTDOC), hWnd, lpDlg);
    FreeProcInstance(lpDlg);
    }
  else
    SessIntCommand(COMMAND_NEXTDOC, -1);
}


void CmdCitator()
  {
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgCitator, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_CITATOR), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
  }


void CmdPrevPage(BOOL fAskUser)
  {
  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  if (fAskUser)
    {
    FARPROC lpDlg;

    lpDlg = MakeProcInstance((FARPROC)DlgPrevPage, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PREVPAGE), hWnd, lpDlg);
    FreeProcInstance(lpDlg);
    }
  else
    SessIntCommand(COMMAND_PREVPAGE, -1);
  }


void CmdPrevDoc(BOOL fAskUser)
  {
  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  if (fAskUser)
    {
    FARPROC lpDlg;

    lpDlg = MakeProcInstance((FARPROC)DlgPrevDoc, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PREVDOC), hWnd, lpDlg);
    FreeProcInstance(lpDlg);
    }
  else
    SessIntCommand(COMMAND_PREVDOC, -1);
  }


void CmdGotoPage()
  {
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgGotoPage, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_GOTO_PAGE), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
  }


void CmdGotoDoc()
  {
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgGotoDoc, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_GOTO_DOC), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
  }


void CmdPrintAll()
  {
  FARPROC lpDlg;


  if (!fSessOnline || hProcessDlg || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgPrintAll, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PRINTALL), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
  }



BOOL DlgNextPage(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, ID_EDIT, nWorkLastNextPage, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          nWorkLastNextPage = GetDlgItemInt(hDlg, ID_EDIT, 0L, FALSE);
          if (nWorkLastNextPage < 1) nWorkLastNextPage = 1;
          if (nWorkLastNextPage > 999) nWorkLastNextPage = 999;
          SessIntCommand(COMMAND_NEXTPAGE, nWorkLastNextPage);

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


BOOL DlgNextDoc(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, ID_EDIT, nWorkLastNextDoc, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          nWorkLastNextDoc = GetDlgItemInt(hDlg, ID_EDIT, 0L, FALSE);
          if (nWorkLastNextDoc < 1) nWorkLastNextDoc = 1;
          if (nWorkLastNextDoc > 999) nWorkLastNextDoc = 999;
          SessIntCommand(COMMAND_NEXTDOC, nWorkLastNextDoc);

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


BOOL DlgCitator(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
  {
  switch (message)
    {
    case WM_INITDIALOG:
      {
      char buf[512];

      SetDlgItemLength(hDlg, ID_EDIT, 200);

      if (nWorkLastAutoCite)
        CheckDlgButton(hDlg, ID_CITE_AUTOCITE, TRUE);
      else
        CheckDlgButton(hDlg, ID_CITE_SHEPARD, TRUE);

      if (TermQuerySelection(buf))
        {
        SetDlgItemText(hDlg, ID_EDIT, buf);
        CheckDlgButton(hDlg, ID_CITE_USER, TRUE);
        }
      else if (ClipQuerySelection(buf))
        {
        SetDlgItemText(hDlg, ID_EDIT, buf);
        CheckDlgButton(hDlg, ID_CITE_USER, TRUE);
        }
      else
        CheckDlgButton(hDlg, ID_CITE_CURRENT, TRUE);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          {
          char buf[256];
          register i;

          if (nWorkLastAutoCite = IsDlgButtonChecked(hDlg, ID_CITE_AUTOCITE))
            i = COMMAND_AUTOCITE;
          else
            i = COMMAND_SHEPARD;

          GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));
          SessArgCommand(i, buf, 0);
          }

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_CITE_AUTOCITE:
        case ID_CITE_SHEPARD:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_CITE_AUTOCITE, ID_CITE_SHEPARD, wParam);
          break;

        case ID_EDIT:
          if (HIWORD(lParam) == EN_CHANGE)
            {
            char buf[256];
            register i;

            i = GetWindowText(LOWORD(lParam), buf, sizeof(buf));
            CheckDlgButton(hDlg, ID_CITE_CURRENT, i == 0);
            CheckDlgButton(hDlg, ID_CITE_USER, i != 0);
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


BOOL DlgPrevPage(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, ID_EDIT, nWorkLastPrevPage, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          nWorkLastPrevPage = GetDlgItemInt(hDlg, ID_EDIT, 0L, FALSE);
          if (nWorkLastPrevPage < 1) nWorkLastPrevPage = 1;
          if (nWorkLastPrevPage > 999) nWorkLastPrevPage = 999;
          SessIntCommand(COMMAND_PREVPAGE, nWorkLastPrevPage);

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


BOOL DlgPrevDoc(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, ID_EDIT, nWorkLastPrevDoc, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          nWorkLastPrevDoc = GetDlgItemInt(hDlg, ID_EDIT, 0L, FALSE);
          if (nWorkLastPrevDoc < 1) nWorkLastPrevDoc = 1;
          if (nWorkLastPrevDoc > 999) nWorkLastPrevDoc = 999;
          SessIntCommand(COMMAND_PREVDOC, nWorkLastPrevDoc);

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


BOOL DlgGotoPage(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      SetFocus(GetDlgItem(hDlg, ID_EDIT));
      SendDlgItemMessage(hDlg, ID_EDIT, EM_SETSEL, 0, MAKELONG(1, 1));
      return (FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          {
          char buf[16];
          register i;

          GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));
          if (buf[0] == '*')
            i = atoi(&buf[1]);
          else
            i = atoi(buf);
          SessIntCommand(COMMAND_GOTOSTARPAGE, i);
          }

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


BOOL DlgGotoDoc(hDlg, message, wParam, lParam)
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
          {
          register i;

          i = GetDlgItemInt(hDlg, ID_EDIT, 0L, FALSE);
          SessIntCommand(COMMAND_GOTODOC, i);
          }

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


BOOL DlgPrintAll(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, ID_PRINT_NUMBER, nWorkLastVarKwic, FALSE);
      ShowWindow(GetDlgItem(hDlg, ID_PRINT_NUMBER), SW_HIDE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          {
          register i;

          if (IsDlgButtonChecked(hDlg, ID_PRINT_FULL))
            i = COMMAND_FULL;
          else if (IsDlgButtonChecked(hDlg, ID_PRINT_KWIC))
            i = COMMAND_KWIC;
          else if (IsDlgButtonChecked(hDlg, ID_PRINT_VKWIC))
            i = COMMAND_VARKWIC;
          else if (IsDlgButtonChecked(hDlg, ID_PRINT_CITE))
            i = COMMAND_CITE;
          else if (IsDlgButtonChecked(hDlg, ID_PRINT_SEGMENTS))
            i = COMMAND_SEGMENTS;
          else
            i = 0;

          if (i == COMMAND_VARKWIC)
            {
            nWorkLastVarKwic = GetDlgItemInt(hDlg, ID_PRINT_NUMBER, NULL, 0);
            SessArgCommand(COMMAND_PRINTALL, Commands[i].szCommand,
                           nWorkLastVarKwic);
            }
          else if (i)
            SessArgCommand(COMMAND_PRINTALL, Commands[i].szCommand, 0);
          else
            SessCanCommand(COMMAND_PRINTALL);
          }

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_PRINT_FULL:
        case ID_PRINT_KWIC:
        case ID_PRINT_VKWIC:
        case ID_PRINT_CITE:
        case ID_PRINT_SEGMENTS:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            if (wParam == ID_PRINT_VKWIC)
              ShowWindow(GetDlgItem(hDlg, ID_PRINT_NUMBER), SW_SHOW);
            else
              ShowWindow(GetDlgItem(hDlg, ID_PRINT_NUMBER), SW_HIDE);

            if (IsDlgButtonChecked(hDlg, wParam))
              CheckDlgButton(hDlg, wParam, FALSE);
            else
              CheckRadioButton(hDlg, ID_PRINT_FULL, ID_PRINT_SEGMENTS, wParam);
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
