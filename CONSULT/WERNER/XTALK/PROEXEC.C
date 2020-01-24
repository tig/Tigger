/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Script Execution (PROEXEC.C)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module is the front-end for the interpreter.  It displays  *
*             script to run dialog and setups the system to run a script.     *
*                                                                             *
*  Revisions:
*  1.00 07/17/89 Initial Version.                                  *
*  1.01 10/26/89 00051 Leave the execute dialog up for edit, new,  *
* 						  and compile.			      *
*	1.01 10/31/89 00052 Add context help to the menus and dialogs.  *
*  1.1  04/02/90 dca00065 PJL Fix Invalid/Open disk drive error reporting.	   *
*  1.1  04/09/90 dca00076 MKL fixed action.scripts focus not set after deletion*
*                                                                             *
\*****************************************************************************/

#define NOATOM			/* Atom Manager routines */
#define NOBITMAP		/* typedef HBITMAP and associated routines */
#define NOBRUSH 		/* typedef HBRUSH and associated routines */
#define NOSOUND 		/* Sound driver routines */

#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "properf.h"
#include "interp.h"
#include "variable.h"
#include "compile.h"
#include "global.h"
#include "variable.h"

  BOOL DlgExecute(HWND, unsigned, WORD, LONG);
  BOOL DlgExecRun(HWND, unsigned, WORD, LONG);

  char PclToEdit[16];


void CmdExecute()
{
  FARPROC lpDlg;
  register i;


  if (IfSystemStatus(SS_SCRIPT))
    {
    InterpreterStop();
    return;
    }

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
  if (!IsPathAvailable(VAR_DIRXWS))
    return;

  CommReset();

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_SCRIPT;

  lpDlg = MakeProcInstance((FARPROC)DlgExecute, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_EXECUTE), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


BOOL DlgExecute(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  HANDLE   hFile;
  USHORT   usBytes;
  OFSTRUCT sOfStruct;
  struct find_t buffer;
  HWND	 hCtl;
  PSTR	 pStr;
  char	 ent[84], buf[84], rec[184], szDir[MAX_PATH];
  int    i, j;


  switch (message)
    {
    case WM_INITDIALOG:
      fCompileOnly = FALSE;

      GetStrVariable(VAR_DIRXWS, szDir, sizeof(szDir));
      SetDlgItemText(hDlg, ID_PATH, szDir);
      SetFilePath(szDir);

      hCtl = GetDlgItem(hDlg, ID_LISTBOX);

      i = _dos_findfirst("*.XW*", _A_NORMAL, &buffer);
      while (i == 0)
        {
        if (*(char *)(strchr(buffer.name, '.')+3) == 'C')
          {
          strcpy(buf, buffer.name);
          *(strchr(buf, '.')) = NULL;
          SendMessage(hCtl, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
          if ((i = (int)SendMessage(hCtl, LB_GETCURSEL, 0, 0L)) != LB_ERR)
            {
            SendMessage(hCtl, LB_GETTEXT, (WORD)i, (DWORD)((LPSTR)rec));
            if (memicmp(rec, buf, strlen(buf)))
              {
              sprintf(ent, "%-8s - (Object Code Only)", buf);
              SendMessage(hCtl, LB_ADDSTRING, NULL, (DWORD)((LPSTR)ent));
              }
            }
          else
            {
            sprintf(ent, "%-8s - (Object Code Only)", buf);
            SendMessage(hCtl, LB_ADDSTRING, NULL, (DWORD)((LPSTR)ent));
            }
          }
        else if (*(char *)(strchr(buffer.name, '.')+3) == 'S')
          {
          if ((hFile = OpenFile(buffer.name, &sOfStruct, OF_READ)) != -1)
            {
	    DosRead(hFile, rec, sizeof(rec), &usBytes);
	    DosClose(hFile);
            rec[sizeof(rec)-1] = 0;

            pStr = rec;
            while (*pStr <= ' ')
              *pStr++;
            if (*pStr == '/')
              {
              while (!isalnum(*pStr) && *pStr)
                *pStr++;

              while (*pStr >= ' ')
                *pStr++;
              *pStr = NULL;

              pStr = rec;
              while (!isalnum(*pStr) && *pStr)
                *pStr++;

              strcpy(buf, buffer.name);
              *(strchr(buf, '.')) = NULL;
              sprintf(ent, "%-8s - %-40s", buf, pStr);
              }
            else
              {
              strcpy(buf, buffer.name);
              *(strchr(buf, '.')) = NULL;
              sprintf(ent, "%-8s -", buf);
              }
            }
          else
            {
            strcpy(buf, buffer.name);
            *(strchr(buf, '.')) = NULL;
            sprintf(ent, "%-8s -", buf);
            }
          SendMessage(hCtl, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);

          if ((i = (int)SendMessage(hCtl, LB_GETCURSEL, 0, 0L)) != LB_ERR)
            {
            SendMessage(hCtl, LB_GETTEXT, (WORD)i, (DWORD)((LPSTR)rec));
            strcat(buf, "       ");
            if (memicmp(rec, buf, 8) == 0)
              SendMessage(hCtl, LB_DELETESTRING, i, 0L);
            }
	  i = (int)SendMessage(hCtl, LB_ADDSTRING, NULL, (DWORD)((LPSTR)ent));
          }

	i = _dos_findnext(&buffer);
        }

      // dca00076 MKL
      // SendMessage(hCtl, LB_SETCURSEL, 0, 0L);
      i = (int) SendMessage(hCtl, LB_SETCURSEL, 0, 0L);
      if (i == LB_ERR) {
          EnableWindow(GetDlgItem(hDlg, ID_OK), FALSE);
          EnableWindow(GetDlgItem(hDlg, ID_OPT1), FALSE);
	  EnableWindow(GetDlgItem(hDlg, ID_DELETE), FALSE);
	  EnableWindow(GetDlgItem(hDlg, ID_EDIT), FALSE);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_LISTBOX:
	  /* dca00076 MKL
          EnableWindow(GetDlgItem(hDlg, ID_OPT1), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_DELETE), TRUE);
	  EnableWindow(GetDlgItem(hDlg, ID_EDIT), TRUE);
	  */

	  if (HIWORD(lParam) != LBN_DBLCLK)
            {
	  //if ((i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L)) == LB_ERR)
	  //  break;
	  //
	  //SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, (WORD)i,
	  //		     (DWORD)((LPSTR)buf));

	    break;
            }

        case ID_OPT1:
	  if (wParam == ID_OPT1)
	    {
            fCompileOnly = TRUE;
	    fAlwaysCompile = TRUE;
	    }

	case IDOK:
	  if ((i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L)) == LB_ERR)
	    break;

	  SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, (WORD)i,
			     (DWORD)((LPSTR)buf));

          *(char *)strchr(buf, ' ') = NULL;
	  i = strlen(buf);
	  buf[i++] = ' ';
	  GetDlgItemText(hDlg, ID_OPT2, &buf[i], 60);

	  // 00051 Keep the dialog up for bad compiles and compile onlys.
	  if (i = ExecuteScript(buf))
	    GeneralError(i);
          else if (wParam != ID_OPT1)
	    EndDialog(hDlg, FALSE);
	  break;

	case ID_ADD:
	  SetFullPath(VAR_DIRXWS);

	  PclToEdit[0] = ' ';
	  PclToEdit[1] = NULL;
	  EditScript(PclToEdit);
          EndDialog(hDlg, FALSE);
	  break;

	case ID_DELETE:
	  if (CancelMessage(MSG_SURETODELETE) != ID_OK)
	    break;

	  if ((i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L)) == LB_ERR)
	    break;

	  SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, (WORD)i,
			     (DWORD)((LPSTR)buf));
	  SendDlgItemMessage(hDlg, ID_LISTBOX, LB_DELETESTRING, i, 0L);

	  SetFullPath(VAR_DIRXWS);

          *(char *)strchr(buf, ' ') = NULL;
          strcat(buf, ".XWS");
	  DosDelete(buf, 0L);

          *(char *)strchr(buf, '.') = NULL;
          strcat(buf, ".XWC");
	  DosDelete(buf, 0L);

          // dca00076 MKL
          if ((i =(int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, 0, 0L)
	     )== LB_ERR)
	  {
              EnableWindow(GetDlgItem(hDlg, ID_OK), FALSE);
              EnableWindow(GetDlgItem(hDlg, ID_OPT1), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_DELETE), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_EDIT), FALSE);
	  }
	  break;

	case ID_EDIT:
	  if ((i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L)) == LB_ERR)
	    break;

	  SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, (WORD)i,
			     (DWORD)((LPSTR)buf));

          *(char *)strchr(buf, ' ') = NULL;
	  strcpy(PclToEdit, buf);
	  // 00051 Keep the dialog up for edits.
          if (PclToEdit[0] != NULL)
	    EditScript(PclToEdit);
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


int ExecuteScript(szScript)
  PSTR szScript;
{
  char szFile[255], szLabel[32], szNull[1], *ptr;
  register i;


//if (IfSystemStatus(SS_SCRIPT))
//  {
//  InterpreterStop();
//  return (NULL);
//  }

  strcpy(szFile, szScript);
  if (ptr = strchr(szFile, ';'))
    *ptr = NULL;

  szNull[0] = NULL;

  ptr = szFile;
  while (*ptr)
    {
    if (*ptr == ' ' || *ptr == ')')
      {
      *ptr++ = NULL;
      break;
      }
    ptr++;
    }

  while (*ptr && *ptr == ' ')
    ptr++;
  SetStrVariable(VAR_ARGLINE, ptr, -1);

  if ((ptr = strchr(szFile, '(')) != NULL)
    {
    *ptr++ = NULL;
    i = strlen(ptr);
    if (i > 31)
     ptr[31] = NULL;
    strcpy(szLabel, ptr);
    strupr(szLabel);
    }
  else
    szLabel[0] = NULL;

  strupr(szFile);
  strcpy(szScriptName, szFile);
  if (fAlwaysCompile || NeedCompile(szFile))
    {
    fAlwaysCompile = FALSE;

    if (i = CompileScript(szFile))
      return (i);

    if (i = SaveScriptObject(szFile))
      return (i);
    }

  if (!fCompileOnly)
    {
    if (i = LoadScriptObject(szFile))
      return (i);
    }

  if (!fCompileOnly || IfSystemStatus(SS_SCRIPT))
    return (InterpreterStart(szLabel));

  fCompileOnly = FALSE;

  return (NULL);
}


int ExecuteCloseScript()
  {
  char buf[96];
  register char *ptr;
  register i;


  LibEnstore(Sess.szScript, buf, sizeof(buf));
  if (ptr = strchr(buf, ';'))
    {
    if (i = ExecuteScript(ptr+1))
      GeneralError(i);
    return (FALSE);
    }

  LibEnstore(szStartupScript, buf, sizeof(buf));
  if (ptr = strchr(buf, ';'))
    {
    if (i = ExecuteScript(ptr+1))
      GeneralError(i);
    return (FALSE);
    }
  else
    return (TRUE);
  }
