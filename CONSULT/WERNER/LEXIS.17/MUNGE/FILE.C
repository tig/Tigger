/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: File Support Module (FILE.C)                                     */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include "lexis.h"
#include "data.h"
#include "record.h"
#include "diary.h"
#include "notepad.h"
#include "dialogs.h"


  BOOL DlgFileNew(HWND, unsigned, WORD, LONG);
  BOOL DlgFileOpen(HWND, unsigned, WORD, LONG);
  BOOL DlgFileClose(HWND, unsigned, WORD, LONG);


void CmdNew()
{
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgFileNew, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEW_FILE), hWnd, lpDlg))
    {
    if (nWorkLastFileType == 0)
      RecordNewFile();
    else if (nWorkLastFileType == 1)
      DiaryNewFile();
    else
      NotepadNewFile();
    }
  FreeProcInstance(lpDlg);
}


void CmdOpen()
{
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgFileOpen, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_OPEN_FILE), hWnd, lpDlg))
    {
    if (nWorkLastFileType == 0)
      RecordOpenFile();
    else if (nWorkLastFileType == 1)
      DiaryOpenFile();
    else
      NotepadOpenFile();
    }
  FreeProcInstance(lpDlg);
}


void CmdClose()
{
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgFileClose, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_CLOSE_FILE), hWnd, lpDlg))
    {
    if (nWorkLastFileType == 0)
      RecordTerm();
    else
      DiaryTerm();
    }
  FreeProcInstance(lpDlg);
}


void CmdSave()
{
}


void CmdSaveAs()
{
}


BOOL DlgFileNew(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      {
      register i;

      if (nWorkLastFileType == 0)
        i = ID_NEW_RECORD;
      else if (nWorkLastFileType == 1)
        i = ID_NEW_DIARY;
      else
        i = ID_NEW_NOTEPAD;
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (IsDlgButtonChecked(hDlg, ID_NEW_RECORD))
            nWorkLastFileType = 0;
          else if (IsDlgButtonChecked(hDlg, ID_NEW_DIARY))
            nWorkLastFileType = 1;
          else
            nWorkLastFileType = 2;

          EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
          break;

        case ID_NEW_RECORD:
        case ID_NEW_DIARY:
        case ID_NEW_NOTEPAD:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_NEW_RECORD, ID_NEW_NOTEPAD, wParam);
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


BOOL DlgFileOpen(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      {
      register i;

      if (nWorkLastFileType == 0)
        i = ID_OPEN_RECORD;
      else if (nWorkLastFileType == 1)
        i = ID_OPEN_DIARY;
      else
        i = ID_OPEN_NOTEPAD;
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (IsDlgButtonChecked(hDlg, ID_OPEN_RECORD))
            nWorkLastFileType = 0;
          else if (IsDlgButtonChecked(hDlg, ID_OPEN_DIARY))
            nWorkLastFileType = 1;
          else
            nWorkLastFileType = 2;

          EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
          break;

        case ID_OPEN_RECORD:
        case ID_OPEN_DIARY:
        case ID_OPEN_NOTEPAD:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_OPEN_RECORD, ID_OPEN_NOTEPAD, wParam);
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


BOOL DlgFileClose(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      {
      extern int hRecordFile;
      extern int hDiaryFile;

      if (hRecordFile == 0)
        EnableWindow(GetDlgItem(hDlg, ID_CLOSE_RECORD), FALSE);
      if (hDiaryFile == 0)
        EnableWindow(GetDlgItem(hDlg, ID_CLOSE_DIARY), FALSE);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (IsDlgButtonChecked(hDlg, ID_CLOSE_RECORD))
            nWorkLastFileType = 0;
          else if (IsDlgButtonChecked(hDlg, ID_CLOSE_DIARY))
            nWorkLastFileType = 1;
          else
            EndDialog(hDlg, FALSE);

          EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
          break;

        case ID_CLOSE_RECORD:
        case ID_CLOSE_DIARY:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_CLOSE_RECORD, ID_CLOSE_DIARY, wParam);
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
