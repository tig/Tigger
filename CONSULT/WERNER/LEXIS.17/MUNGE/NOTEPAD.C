/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Notepad Support (NOTEPAD.C)                                      */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <direct.h>
#include <dos.h>
#include <errno.h>
#include <string.h>
#include "lexis.h"
#include "notepad.h"
#include "data.h"
#include "library.h"
#include "dialogs.h"


  static void near NotepadOpen(void);

void CmdNotepad()
  {
  SpawnApp("NOTEPAD.EXE", "");
  }


  BOOL DlgNotepad(HWND, unsigned, WORD, LONG);

void NotepadNewFile()
  {
  FARPROC lpDlg;
  char    szCwd[128];

  getcwd(szCwd, sizeof(szCwd));
  SetFilePath(szDataDirNotepad);

  lpDlg = MakeProcInstance((FARPROC)DlgNotepad, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEW_NOTEPAD), hWnd, lpDlg))
    {
    int hFile;

    if (_dos_creatnew(szWorkNotepad, _A_NORMAL, &hFile) == 0)
      _dos_close(hFile);

    NotepadOpen();
    }

  FreeProcInstance(lpDlg);
  SetFilePath(szCwd);
  }


void NotepadOpenFile()
  {
  OFSTRUCT OfStruct;
  int extfile;
  char szFileNameIn[128];


  SetFilePath(szDataDirNotepad);

  strcpy(szFileNameIn, "");

  if (DlgOpen(hWndInst, hWnd, IDD_OPEN_NOTEPAD, &OfStruct, &extfile,
          szFileNameIn, "\\*.NTS", szAppName) == NOOPEN)
    {
    extfile = -1;
    }
  else
    {
    _dos_close(extfile);
    strcpy(szWorkNotepad, StripPath(OfStruct.szPathName));
    NotepadOpen();
    }
  }


BOOL DlgNotepad(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  OFSTRUCT ofStruct;


  switch (message)
    {
    case WM_INITDIALOG:
      DlgDirList(hDlg, NULL, NULL, ID_PATH, NULL);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case IDOK:
          GetDlgItemText(hDlg, ID_EDIT, szWorkNotepad, sizeof(szWorkNotepad));
          if (OpenFile(szWorkNotepad, &ofStruct, OF_PARSE) ||
              DlgSearchSpec(szWorkNotepad))
            {
            MessageDisplay(IDS_MSG_BADNAME, MB_ICONHAND);
            break;
            }

          if (strchr(szWorkNotepad, '.') == NULL)
            {
            register i;

            i = strlen(szWorkNotepad);
            szWorkNotepad[i++] = '.';
            szWorkNotepad[i++] = 'N';
            szWorkNotepad[i++] = 'T';
            szWorkNotepad[i++] = 'S';
            szWorkNotepad[i]   = NULL;
            }

	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
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

static void near NotepadOpen()
  {
  char szArg[64];

  szArg[0] = (char)strlen(szWorkNotepad);
  strcpy(&szArg[1], szWorkNotepad);
  SpawnApp("NOTEPAD.EXE", szArg);
  }
