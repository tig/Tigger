/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: CiteLink Support Module (CITELINK.C)                             */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <string.h>
#include "lexis.h"
#include "data.h"
#include "session.h"
#include "term.h"
#include "library.h"
#include "dialogs.h"


  BOOL DlgAutoCite(HWND, unsigned, WORD, LONG);
  BOOL DlgShepard(HWND, unsigned, WORD, LONG);
  BOOL DlgLexstat(HWND, unsigned, WORD, LONG);
  BOOL DlgLexsee(HWND, unsigned, WORD, LONG);
  BOOL DlgResume(HWND, unsigned, WORD, LONG);

  static void FilterCiteText(char *buf);


void CmdAutoCite()
{
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgAutoCite, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_AUTOCITE), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


void CmdShepard()
{
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgShepard, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SHEPARD), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


void CmdLexstat()
{
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgLexstat, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_LEXSTAT), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


void CmdLexsee()
{
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgLexsee, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_LEXSEE), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


void Browse()
{
}


void CmdResume()
{
  FARPROC lpDlg;


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  lpDlg = MakeProcInstance((FARPROC)DlgResume, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_RESUME), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


BOOL DlgAutoCite(hDlg, message, wParam, lParam)
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

      if (TermQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else if (ClipQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else
        SetDlgItemText(hDlg, ID_EDIT, szWorkCite);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          {
          char buf[256];

          GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));
          SessArgCommand(COMMAND_AUTOCITE, buf, 0);
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


BOOL DlgShepard(hDlg, message, wParam, lParam)
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

      if (TermQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else if (ClipQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else
        SetDlgItemText(hDlg, ID_EDIT, szWorkCite);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          {
          char buf[256];

          GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));
          SessArgCommand(COMMAND_SHEPARD, buf, 0);
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


BOOL DlgLexstat(hDlg, message, wParam, lParam)
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

      if (TermQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else if (ClipQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else
        SetDlgItemText(hDlg, ID_EDIT, szWorkCite);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          {
          char buf[256];

          GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));
          SessArgCommand(COMMAND_LXSTAT, buf, 0);
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


BOOL DlgLexsee(hDlg, message, wParam, lParam)
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

      if (TermQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else if (ClipQuerySelection(buf))
        {
        FilterCiteText(buf);
        SetDlgItemText(hDlg, ID_EDIT, buf);
        }
      else
        SetDlgItemText(hDlg, ID_EDIT, szWorkCite);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          {
          char buf[256];

          GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));
          SessArgCommand(COMMAND_LEXSEE, buf, 0);
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


BOOL DlgResume(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      CheckDlgButton(hDlg, ID_RESUME_PRIOR, TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          {
          register i;

          if (IsDlgButtonChecked(hDlg, ID_RESUME_PRIOR))
            i = COMMAND_RESUME;
          else if (IsDlgButtonChecked(hDlg, ID_RESUME_LEXIS))
            i = COMMAND_RESUMELEXIS;
          else if (IsDlgButtonChecked(hDlg, ID_RESUME_AUTOCITE))
            i = COMMAND_RESUMEAUTOCITE;
          else if (IsDlgButtonChecked(hDlg, ID_RESUME_SHEPARD))
            i = COMMAND_RESUMESHEPARD;
          else if (IsDlgButtonChecked(hDlg, ID_RESUME_LEXSEE))
            i = COMMAND_RESUMELEXSEE;
          else
            i = COMMAND_RESUMELXSTAT;

          SessCanCommand(i);
          }

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_RESUME_PRIOR:
        case ID_RESUME_LEXIS:
        case ID_RESUME_AUTOCITE:
        case ID_RESUME_SHEPARD:
        case ID_RESUME_LEXSEE:
        case ID_RESUME_LEXSTAT:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_RESUME_PRIOR, ID_RESUME_LEXSTAT, wParam);
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


static void FilterCiteText(char *buf)
  {
  register i;

  i = strlen(buf);

  while (i)
    {
    if (buf[i-1] == ' ' || buf[i-1] == ',' || buf[i-1] == ';')
      {
      buf[--i] = '\0';
      }
    else
      break;
    }

  strncpy(szWorkCite, buf, sizeof(szWorkCite)-1);
  }
