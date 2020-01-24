/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Help Routine (HELP.C)                                            */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include "lexis.h"
#include "library.h"
#include "data.h"
#include "dialogs.h"



  WORD wHelpTopic;

  BOOL FAR  PASCAL DlgHelp1(HWND, unsigned, WORD, LONG);
  BOOL FAR  PASCAL DlgHelp2(HWND, unsigned, WORD, LONG);

  static int  near pascal ReadNextIndex(HANDLE, char *);
  static void near pascal ReadHelp2Topic(HWND);
  static void near pascal SetHelp2Buttons(HWND);
  static WORD wMaxTopic;


void FAR PASCAL CmdCommands()
{
  FARPROC lpDlg;


  lpDlg = MakeProcInstance((FARPROC)DlgHelp1, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_HELP1), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  SetFocus(hWnd);
}


BOOL FAR PASCAL DlgHelp1(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  FARPROC  lpDlg;
  HANDLE   hResInfo;
  HANDLE   hResFile;
  char     szBuf[256];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
     hResInfo = FindResource(hWndInst, MAKEINTRESOURCE(IDU_HELP), MAKEINTRESOURCE(USER));
     hResFile = AccessResource(hWndInst, hResInfo);

     wMaxTopic = 0;
     while (ReadNextIndex(hResFile, szBuf))
       {
       SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, 0, (DWORD)((LPSTR)szBuf));
       wMaxTopic++;
       }
     close(hResFile);
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


BOOL FAR PASCAL DlgHelp2(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      ReadHelp2Topic(hDlg);
      SetHelp2Buttons(hDlg);
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


static int near pascal ReadNextIndex(hHelpFile, pBuf)
  HANDLE hHelpFile;
  PSTR   pBuf;
{
  long  lPos;
  char  buf[256];
  int   nBytes;


  lPos = tell(hHelpFile);
  _dos_read(hHelpFile, buf, sizeof(buf), &nBytes);
  if (buf[0] && nBytes)
    {
    register i;

    i = strlen(buf);
    memcpy(pBuf, buf, i+1);
    lseek(hHelpFile, lPos + i + 5, NULL);
    return (i);
    }
  else
    return (NULL);
}


static void near pascal ReadHelp2Topic(HWND hDlg)
  {
  HANDLE   hResInfo;
  HANDLE   hResFile;
  char     szBuf[256];
  WORD	   nIndex, wLineCnt;
  unsigned nBytes;
  long     wOffset;
  long     lPos, lPosHead;
  register WORD i;


  SendDlgItemMessage(hDlg, ID_LISTBOX, WM_SETREDRAW, 0, 0L);
  SendDlgItemMessage(hDlg, ID_LISTBOX, LB_RESETCONTENT, 0, 0L);

  hResInfo = FindResource(hWndInst, MAKEINTRESOURCE(IDU_HELP), MAKEINTRESOURCE(USER));
  hResFile = AccessResource(hWndInst, hResInfo);
  lPosHead = tell(hResFile);

  for (nIndex = 0; nIndex <= wHelpTopic; nIndex++)
    {
    lPos = tell(hResFile);
    _dos_read(hResFile, szBuf, sizeof(szBuf), &nBytes);
    i = strlen(szBuf);

    wOffset = *(long *)(&szBuf[i+1]);
    lseek(hResFile, lPos + i + 5, NULL);
    }

  SetDlgItemText(hDlg, ID_EDIT, szBuf);
  lseek(hResFile, lPosHead + wOffset, NULL);

  wLineCnt = 0;
  while (1)
    {
    lPos = tell(hResFile);
    memset(szBuf, 0, sizeof(szBuf));
    _dos_read(hResFile, szBuf, sizeof(szBuf), &nBytes);
    if (szBuf[0] && nBytes)
      {
      i = strlen(szBuf);

      if (szBuf[0] == '%')
        szBuf[0] = ' ';
      SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, 0, (DWORD)((LPSTR)szBuf));
      lseek(hResFile, lPos + i + 1, NULL);
      }
    else
      break;
    }

  SendDlgItemMessage(hDlg, ID_LISTBOX, WM_SETREDRAW, 1, 0L);
  InvalidateRect(GetDlgItem(hDlg, ID_LISTBOX), NULL, TRUE);

  close(hResFile);
  }


static void near pascal SetHelp2Buttons(HWND hDlg)
  {
  if (wHelpTopic == 0)
    {
    EnableWindow(GetDlgItem(hDlg, ID_HELP_NEXT), TRUE);
    SetFocus(GetDlgItem(hDlg, ID_HELP_TOPICS));
    EnableWindow(GetDlgItem(hDlg, ID_HELP_PREV), FALSE);
    }
  else if (wHelpTopic + 1 >= wMaxTopic)
    {
    EnableWindow(GetDlgItem(hDlg, ID_HELP_PREV), TRUE);
    SetFocus(GetDlgItem(hDlg, ID_HELP_TOPICS));
    EnableWindow(GetDlgItem(hDlg, ID_HELP_NEXT), FALSE);
    }
  else
    {
    EnableWindow(GetDlgItem(hDlg, ID_HELP_NEXT), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_HELP_PREV), TRUE);
    }
  }
