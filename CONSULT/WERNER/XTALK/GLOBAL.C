/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Global Data (GLOBAL.C)                                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module controls the global data stored in the WIN.INI.     *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/05/89 00025 Add optional to CR/LF to host paste.        *
*             1.01 09/08/89 00030 Change MISC from a global variable to a     *
*                                   session variable to match the book.       *
*	      1.01 10/15/89 00045 Fix the problem of tabbing into the non-    *
*				    displayed area on the dialog.	      *
*	      1.01 12/25/89 00071 Corrections to Setup.System dialog.	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <direct.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "global.h"
#include "comm.h"
#include "term.h"
#include "properf.h"
#include "dialogs.h"
#include "variable.h"


  char szCwd[128];
  char szEditor[41];
  char szStartupScript[41];
  char szOutNumber[41];
  char szLdNumber[41];
  // char szMisc[41];  // 00030 Change MISC from global to profile variable.
  char fCodesave = TRUE;
  char fSecret;
  char fBackups;
  char fPasteCrLf;  // 00025 Add optional to CR/LF to host paste.


  BOOL DlgSystem(HWND, unsigned, WORD, LONG);


BOOL InitGlobal()
{
  DosQCurDir(0, szCwd, sizeof(szCwd));
  strcat(szCwd, "\\");

  LoadGlobalData();

  return (TRUE);
}

BOOL LoadGlobalData()
{
  char buf[32], szCwd[64], szDir[64], szNull[1];
  int  i;


  szNull[0] = NULL;

  DosQCurDir(0, szCwd, sizeof(szCwd));
  strcpy(buf, "DirXwp");
  GetProfileString(szAppName, buf, szCwd, szDir, sizeof(szDir));
  SetStrVariable(VAR_DIRXWP, szDir, -1);

  strcpy(buf, "DirXws");
  GetProfileString(szAppName, buf, szCwd, szDir, sizeof(szDir));
  SetStrVariable(VAR_DIRXWS, szDir, -1);

  strcpy(buf, "DirFil");
  GetProfileString(szAppName, buf, szCwd, szDir, sizeof(szDir));
  SetStrVariable(VAR_DIRFIL, szDir, -1);

  strcpy(buf, "Editor");
  GetProfileData(szAppName, buf, "\013NOTEPAD.EXE", szEditor, sizeof(szEditor));

  strcpy(buf, "Startup");
  GetProfileData(szAppName, buf, szNull, szStartupScript,
		   sizeof(szStartupScript));

  strcpy(buf, "OutNumber");
  GetProfileData(szAppName, buf, szNull, szOutNumber, sizeof(szOutNumber));

  strcpy(buf, "LdNumber");
  GetProfileData(szAppName, buf, szNull, szLdNumber, sizeof(szLdNumber));

  // 00030 Change MISC from global to profile variable.
  // strcpy(buf, "Misc");
  // GetProfileData(szAppName, buf, szNull, szMisc, sizeof(szMisc));

  strcpy(buf, "Backups");
  fBackups  = (char)GetProfileInt(szAppName, buf, 0);

  strcpy(buf, "Secret");
  fSecret   = (char)GetProfileInt(szAppName, buf, 0);

  // 00025 Add optional to CR/LF to host paste.
  strcpy(buf, "Paste");
  fPasteCrLf = (char)GetProfileInt(szAppName, buf, 1);

  strcpy(buf, "Scroll");
  nScrollMemory = GetProfileInt(szAppName, buf, 16);
  if (nScrollMemory != 0 && nScrollMemory != 32 && nScrollMemory != 48 && nScrollMemory != 64)
    nScrollMemory = 16;

  strcpy(buf, "Comm");
  nCommPortMemory = GetProfileInt(szAppName, buf, 2048);
  if (nCommPortMemory < 2048 || nCommPortMemory > 16384)
    nCommPortMemory = 2048;

  return TRUE;
}

BOOL SaveGlobalData()
{
  char buf[32], tmp[64], szDir[64], szNull[1], fmt[2];


  szNull[1] = NULL;

  strcpy(buf, "DirXwp");
  GetProfileString(szAppName, buf, szNull, tmp, sizeof(tmp));
  GetStrVariable(VAR_DIRXWP, szDir, sizeof(szDir));
  if (strcmp(szDir, tmp))
    WriteProfileString(szAppName, buf, szDir);

  strcpy(buf, "DirXws");
  GetProfileString(szAppName, buf, szNull, tmp, sizeof(tmp));
  GetStrVariable(VAR_DIRXWS, szDir, sizeof(szDir));
  if (strcmp(szDir, tmp))
    WriteProfileString(szAppName, buf, szDir);

  strcpy(buf, "DirFil");
  GetProfileString(szAppName, buf, szNull, tmp, sizeof(tmp));
  GetStrVariable(VAR_DIRFIL, szDir, sizeof(szDir));
  if (strcmp(szDir, tmp))
    WriteProfileString(szAppName, buf, szDir);

  strcpy(buf, "Editor");
  WriteProfileData(szAppName, buf, szEditor);

  strcpy(buf, "Startup");
  WriteProfileData(szAppName, buf, szStartupScript);

  strcpy(buf, "OutNumber");
  WriteProfileData(szAppName, buf, szOutNumber);

  strcpy(buf, "LdNumber");
  WriteProfileData(szAppName, buf, szLdNumber);

  // 00030 Change MISC from global to profile variable.
  // strcpy(buf, "Misc");
  // WriteProfileData(szAppName, buf, szMisc);

  strcpy(buf, "Backups");
  GetProfileString(szAppName, buf, szNull, tmp, sizeof(tmp));
  if (fBackups)
    fmt[0] = '1';
  else
    fmt[0] = '0';
  fmt[1] = NULL;
  if (strcmp(fmt, tmp))
    WriteProfileString(szAppName, buf, fmt);

  strcpy(buf, "Secret");
  GetProfileString(szAppName, buf, szNull, tmp, sizeof(tmp));
  if (fSecret)
    fmt[0] = '1';
  else
    fmt[0] = '0';
  fmt[1] = NULL;
  if (strcmp(fmt, tmp))
    WriteProfileString(szAppName, buf, fmt);

  // 00025 Add optional to CR/LF to host paste.
  strcpy(buf, "Paste");
  GetProfileString(szAppName, buf, szNull, tmp, sizeof(tmp));
  if (fPasteCrLf)
    fmt[0] = '1';
  else
    fmt[0] = '0';
  fmt[1] = NULL;
  if (strcmp(fmt, tmp))
    WriteProfileString(szAppName, buf, fmt);

  strcpy(buf, "Scroll");
  if (nScrollMemory != GetProfileInt(szAppName, buf, 16))
    {
    sprintf(tmp, "%d", nScrollMemory);
    WriteProfileString(szAppName, buf, tmp);
    }

  strcpy(buf, "Comm");
  if (nCommPortMemory != GetProfileInt(szAppName, buf, 2048))
    {
    sprintf(tmp, "%d", nCommPortMemory);
    WriteProfileString(szAppName, buf, tmp);
    }

  return TRUE;
}


void CmdSystem()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_SYSTEM;

  lpDlg = MakeProcInstance((FARPROC)DlgSystem, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SYSTEM), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


BOOL DlgSystem(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char szDir[MAX_PATH];
  register i;

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemLength(hDlg, ID_GLOBAL_STARTUP,   sizeof(szStartupScript)-1);
      SetDlgItemLength(hDlg, ID_GLOBAL_OUTNUMBER, sizeof(szOutNumber)-1);
      SetDlgItemLength(hDlg, ID_GLOBAL_LDNUMBER,  sizeof(szLdNumber)-1);

      SetDlgItemData(hDlg, ID_GLOBAL_STARTUP,	szStartupScript);
      SetDlgItemData(hDlg, ID_GLOBAL_OUTNUMBER, szOutNumber);
      SetDlgItemData(hDlg, ID_GLOBAL_LDNUMBER,	szLdNumber);

      SetDlgItemLength(hDlg, ID_GLOBAL_XWP, sizeof(szDir)-1);
      SetDlgItemLength(hDlg, ID_GLOBAL_XWS, sizeof(szDir)-1);
      SetDlgItemLength(hDlg, ID_GLOBAL_FIL, sizeof(szDir)-1);
      SetDlgItemLength(hDlg, ID_GLOBAL_EDIT, sizeof(szEditor)-1);

      GetStrVariable(VAR_DIRXWP, szDir, sizeof(szDir));
      SetDlgItemPath(hDlg, ID_GLOBAL_XWP, szDir);

      GetStrVariable(VAR_DIRXWS, szDir, sizeof(szDir));
      SetDlgItemPath(hDlg, ID_GLOBAL_XWS, szDir);

      GetStrVariable(VAR_DIRFIL, szDir, sizeof(szDir));
      SetDlgItemPath(hDlg, ID_GLOBAL_FIL, szDir);

      SetDlgItemData(hDlg, ID_GLOBAL_EDIT, szEditor);

      if (nScrollMemory == 0)
	i = ID_GLOBAL_NONE;
      else if (nScrollMemory <= 16)
	i = ID_GLOBAL_16K;
      else if (nScrollMemory <= 32)
	i = ID_GLOBAL_32K;
      else
	i = ID_GLOBAL_64K;
      CheckDlgButton(hDlg, i, TRUE);

      if (bConnected)
	{
	EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP2K), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP4K), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP8K), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP12K), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP16K), FALSE);
	}
      if (nCommPortMemory == 2048)
	i = ID_GLOBAL_CP2K;
      else if (nCommPortMemory == 4096)
	i = ID_GLOBAL_CP4K;
      else if (nCommPortMemory == 8192)
	i = ID_GLOBAL_CP8K;
      else if (nCommPortMemory == 12288)
	i = ID_GLOBAL_CP12K;
      else
	i = ID_GLOBAL_CP16K;
      CheckDlgButton(hDlg, i, TRUE);

      if (fBackups)
        CheckDlgButton(hDlg, ID_GLOBAL_BACKUPS, TRUE);
      if (!fSecret)
        CheckDlgButton(hDlg, ID_GLOBAL_SECRET, TRUE);
      if (fPasteCrLf)  // 00025 Add optional to CR/LF to host paste.
	CheckDlgButton(hDlg, ID_GLOBAL_PASTE, TRUE);  // 00071
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
          if (!GetDlgItemPath(hDlg, ID_GLOBAL_XWP, szDir, sizeof(szDir)))
            break;
          if (!GetDlgItemPath(hDlg, ID_GLOBAL_XWS, szDir, sizeof(szDir)))
            break;
          if (!GetDlgItemPath(hDlg, ID_GLOBAL_FIL, szDir, sizeof(szDir)))
            break;

          GetDlgItemData(hDlg, ID_GLOBAL_EDIT, szEditor,
                           sizeof(szEditor));
	  GetDlgItemData(hDlg, ID_GLOBAL_STARTUP, szStartupScript,
			   sizeof(szStartupScript));
	  GetDlgItemData(hDlg, ID_GLOBAL_OUTNUMBER, szOutNumber,
			   sizeof(szOutNumber));
	  GetDlgItemData(hDlg, ID_GLOBAL_LDNUMBER, szLdNumber,
			   sizeof(szLdNumber));

          fBackups   = (char)IsDlgButtonChecked(hDlg, ID_GLOBAL_BACKUPS);
          fSecret    = (char)(!IsDlgButtonChecked(hDlg, ID_GLOBAL_SECRET));
          // 00025 Add optional to CR/LF to host paste.
          fPasteCrLf = (char)IsDlgButtonChecked(hDlg, ID_GLOBAL_PASTE);

          if (GetDlgItemPath(hDlg, ID_GLOBAL_XWP, szDir, sizeof(szDir)))
            SetStrVariable(VAR_DIRXWP, szDir, -1);

          if (GetDlgItemPath(hDlg, ID_GLOBAL_XWS, szDir, sizeof(szDir)))
            SetStrVariable(VAR_DIRXWS, szDir, -1);

          if (GetDlgItemPath(hDlg, ID_GLOBAL_FIL, szDir, sizeof(szDir)))
            SetStrVariable(VAR_DIRFIL, szDir, -1);

	  if (IsDlgButtonChecked(hDlg, ID_GLOBAL_NONE))
	    i = 0;
	  else if (IsDlgButtonChecked(hDlg, ID_GLOBAL_16K))
	    i = 16;
	  else if (IsDlgButtonChecked(hDlg, ID_GLOBAL_32K))
	    i = 32;
	  else
	    i = 64;
	  if (i != nScrollMemory)
	    {
	    nScrollMemory = i;
	    VssDelete();
	    VssCreate();
	    }

	  if (!bConnected)
	    {
	    if (IsDlgButtonChecked(hDlg, ID_GLOBAL_CP2K))
	      nCommPortMemory = 2048;
	    else if (IsDlgButtonChecked(hDlg, ID_GLOBAL_CP4K))
	      nCommPortMemory = 4096;
	    else if (IsDlgButtonChecked(hDlg, ID_GLOBAL_CP8K))
	      nCommPortMemory = 8192;
	    else if (IsDlgButtonChecked(hDlg, ID_GLOBAL_CP12K))
	      nCommPortMemory = 12288;
	    else
	      nCommPortMemory = 16384;
	    }

	  SaveGlobalData();
	  EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
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
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_XWP), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_XWS), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_FIL), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_EDIT), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_NONE), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_16K), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_32K), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_64K), TRUE);

	  if (!bConnected)
	    {
	    EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP2K), TRUE);
	    EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP4K), TRUE);
	    EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP8K), TRUE);
	    EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP12K), TRUE);
	    EnableWindow(GetDlgItem(hDlg, ID_GLOBAL_CP16K), TRUE);
	    }

          SetFocus(GetDlgItem(hDlg, ID_GLOBAL_XWP));
          EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
	  }
	  break;

	case ID_GLOBAL_NONE:
	case ID_GLOBAL_16K:
	case ID_GLOBAL_32K:
	case ID_GLOBAL_64K:
	  CheckRadioButton(hDlg, ID_GLOBAL_NONE, ID_GLOBAL_64K, wParam);
	  break;

	case ID_GLOBAL_CP2K:
	case ID_GLOBAL_CP4K:
	case ID_GLOBAL_CP8K:
	case ID_GLOBAL_CP12K:
	case ID_GLOBAL_CP16K:
	  CheckRadioButton(hDlg, ID_GLOBAL_CP2K, ID_GLOBAL_CP16K, wParam);
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
