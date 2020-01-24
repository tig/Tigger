/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Help Dialog (HELP.C)                                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module display the requested help in a simple dialog.      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 10/31/89 00052 Add context help to the menus and dialogs.  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "sess.h"
#include "global.h"
#include "prodata.h"
#include "dialogs.h"
#include "term.h"
#include "global.h"


  WORD wHelpTopic;
  WORD wHelpDialog;
  WORD wHelpMenu;
  BOOL bHelpActive = FALSE;
  BOOL bHelpTopic;

  BOOL DlgHelp1(HWND, unsigned, WORD, LONG);
  BOOL DlgHelp2(HWND, unsigned, WORD, LONG);

  static int	near ReadNextIndex(HANDLE, char *);
  static USHORT near ReadHelp2Topic(HWND);
  static void	near SetHelp2Buttons(HWND);
  static WORD	wMaxTopic = HELP_ITEM_MAX;

  extern char szHelpPath[];

void CmdHelpIndex()
  {
  FARPROC lpDlg;


  if (!bHelpActive)
    {
    bHelpActive = TRUE;
    bHelpTopic	= TRUE;

    SetPath(szCwd);

    lpDlg = MakeProcInstance((FARPROC)DlgHelp1, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_HELP1), hWnd, lpDlg);
    FreeProcInstance(lpDlg);

    SetFocus(hWnd);

    bHelpActive = FALSE;
    }
  }

//
// 00052 CmdHelpTopic
//
void CmdHelpTopic(WORD wTopic)
  {
  HWND	  hWnd;
  FARPROC lpDlg;


  if (!bHelpActive)
    {
    bHelpActive = TRUE;
    bHelpTopic	= FALSE;

    SetPath(szCwd);

    hWnd = GetFocus();

    wHelpTopic = wTopic;
    lpDlg = MakeProcInstance((FARPROC)DlgHelp2, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_HELP2), hWnd, lpDlg);
    FreeProcInstance(lpDlg);

    SetFocus(hWnd);

    bHelpActive = FALSE;
    }
  }


BOOL DlgHelp1(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  FARPROC  lpDlg;
  int      hHelpFile;
  OFSTRUCT OfStruct;
  char     szBuf[256];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
     if ((hHelpFile = OpenFile(szHelpPath, &OfStruct, OF_READ)) == -1)
       {
       ErrorMessage(IDS_ERRDLG_NOHELP);
       EndDialog(hDlg, FALSE);
       break;
       }

     wMaxTopic = 0;
     while (ReadNextIndex(hHelpFile, szBuf))
       {
       SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, 0, (DWORD)((LPSTR)szBuf));
       wMaxTopic++;
       }
     DosClose(hHelpFile);
     SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, wHelpTopic, 0L);
     break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L);
          if (i > 0)
            wHelpTopic = i;
          else
            wHelpTopic = 0;
          lpDlg = MakeProcInstance((FARPROC)DlgHelp2, hWndInst);
          i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_HELP2), hWnd, lpDlg);
          FreeProcInstance(lpDlg);
          if (i == FALSE)
            EndDialog(hDlg, FALSE);
          else
            {
            SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, wHelpTopic, 0L);
            SetFocus(hDlg);
            }
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_LISTBOX:
          if (HIWORD(lParam) == LBN_DBLCLK)
	    {
	    i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L);
            if (i >= 0)
              {
              wHelpTopic = i;
              lpDlg = MakeProcInstance((FARPROC)DlgHelp2, hWndInst);
              i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_HELP2), hWnd, lpDlg);
              FreeProcInstance(lpDlg);
              if (i == FALSE)
                EndDialog(hDlg, FALSE);
              else
                {
                SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, wHelpTopic, 0L);
                SetFocus(hDlg);
                }
	      }
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


BOOL DlgHelp2(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      if (ReadHelp2Topic(hDlg))
	{
	ErrorMessage(IDS_ERRDLG_NOHELP);
	EndDialog(hDlg, FALSE);
	break;
	}
      SetHelp2Buttons(hDlg);
      if (!bHelpTopic)
	{
	SetFocus(GetDlgItem(hDlg, ID_CANCEL));
	EnableWindow(GetDlgItem(hDlg, ID_HELP_TOPICS), FALSE);
	return (FALSE);
	}
      return (TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
        case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_HELP_TOPICS:
          EndDialog(hDlg, TRUE);
          break;

        case ID_HELP_NEXT:
          wHelpTopic++;
          ReadHelp2Topic(hDlg);
          SetHelp2Buttons(hDlg);
          break;

        case ID_HELP_PREV:
          wHelpTopic--;
          ReadHelp2Topic(hDlg);
          SetHelp2Buttons(hDlg);
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


static int near ReadNextIndex(hHelpFile, pBuf)
  HANDLE hHelpFile;
  PSTR   pBuf;
{
  ULONG ulPos;
  ULONG lPos;
  char  buf[256];
  int   nBytes;


  DosChgFilePtr(hHelpFile, 0L, FILE_CURRENT, &lPos);
  DosRead(hHelpFile, buf, sizeof(buf), &nBytes);
  if (buf[0] && nBytes)
    {
    register i;

    i = strlen(buf);
    memcpy(pBuf, buf, i+1);
    DosChgFilePtr(hHelpFile, lPos + i + 5, FILE_BEGIN, &ulPos);
    return (i);
    }
  else
    return (NULL);
}


static USHORT near ReadHelp2Topic(HWND hDlg)
  {
  ULONG    ulPos;
  HFILE    hHelpFile;
  OFSTRUCT OfStruct;
  char     szBuf[256];
  WORD	   nIndex, wLineCnt;
  USHORT   nBytes;
  long     wOffset;
  long     lPos;
  register WORD i;


  SendDlgItemMessage(hDlg, ID_LISTBOX, WM_SETREDRAW, 0, 0L);
  SendDlgItemMessage(hDlg, ID_LISTBOX, LB_RESETCONTENT, 0, 0L);

  if ((hHelpFile = OpenFile(szHelpPath, &OfStruct, OF_READ)) == -1)
    return (-1);

  for (nIndex = 0; nIndex <= wHelpTopic; nIndex++)
    {
    DosChgFilePtr(hHelpFile, 0L, FILE_CURRENT, &lPos);
    DosRead(hHelpFile, szBuf, sizeof(szBuf), &nBytes);
    i = strlen(szBuf);

    wOffset = *(long *)(&szBuf[i+1]);
    DosChgFilePtr(hHelpFile, lPos + i + 5, FILE_BEGIN, &ulPos);
    }

  DosChgFilePtr(hHelpFile, wOffset, FILE_BEGIN, &ulPos);

  wLineCnt = 0;
  while (1)
    {
    DosChgFilePtr(hHelpFile, 0L, FILE_CURRENT, &lPos);
    DosRead(hHelpFile, szBuf, sizeof(szBuf), &nBytes);
    if (szBuf[0] && nBytes)
      {
      i = strlen(szBuf);

      if (szBuf[0] == '%')
        szBuf[0] = ' ';
      if (szBuf[i+1] == '\0' || wLineCnt++ == 8)
        SendDlgItemMessage(hDlg, ID_LISTBOX, WM_SETREDRAW, 1, 0L);
      SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, 0, (DWORD)((LPSTR)szBuf));
      DosChgFilePtr(hHelpFile, lPos + i + 1, FILE_BEGIN, &ulPos);
      }
    else
      break;
    }

  DosClose(hHelpFile);

  return (0);
  }


static void near SetHelp2Buttons(hDlg)
  HWND hDlg;
{
  if (wHelpTopic == 0)
    {
    EnableWindow(GetDlgItem(hDlg, ID_HELP_NEXT), TRUE);
    if (GetFocus() == GetDlgItem(hDlg, ID_HELP_PREV))
      SetFocus(GetDlgItem(hDlg, ID_HELP_NEXT));
    EnableWindow(GetDlgItem(hDlg, ID_HELP_PREV), FALSE);
    }
  else if (wHelpTopic + 1 >= wMaxTopic)
    {
    EnableWindow(GetDlgItem(hDlg, ID_HELP_PREV), TRUE);
    if (GetFocus() == GetDlgItem(hDlg, ID_HELP_NEXT))
      SetFocus(GetDlgItem(hDlg, ID_HELP_PREV));
    EnableWindow(GetDlgItem(hDlg, ID_HELP_NEXT), FALSE);
    }
  else
    {
    EnableWindow(GetDlgItem(hDlg, ID_HELP_NEXT), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_HELP_PREV), TRUE);
    }
}
