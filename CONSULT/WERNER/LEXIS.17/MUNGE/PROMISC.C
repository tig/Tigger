/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Misc Routines (PROMISC.C)                                        */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include "lexis.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "data.h"
#include "term.h"
#include "session.h"


void SetDlgItemStr(hDlg, item, msg)
  HWND hDlg;
  int  item;
  int  msg;
{
  char buf[80];

  LoadString(hWndInst, msg, buf, sizeof(buf));
  SetDlgItemText(hDlg, item, buf);
}


void SystemUpdate()
{

  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);

  UpdateTitle();
}


void UpdateTitle()
{
  char buf[64];


  strcpy(buf, szAppTitle);

  SetWindowText(hWnd, buf);
}


void ResetScreenDisplay()
{
  RECT Rect;

  InvalidateRect(hChildTerminal, NULL, TRUE);

  GetClientRect(hWnd, (LPRECT)&Rect);
  PostMessage(hWnd, WM_SIZE, 0,
	      MAKELONG(Rect.right - Rect.left, Rect.bottom - Rect.top));
}


void ErrorMsg(msg, errno)
  char *msg;
  int  errno;
{
  char buf[256];

  sprintf(buf,"%s, %d", msg, errno);
  MessageBeep(NULL);
  MessageBox(hWnd ,(LPSTR)buf, NULL, MB_OK | MB_SYSTEMMODAL);
}


WORD MessageDisplay(int nMsgNo, int nIcon)
  {
  char buf[80];

  LoadString(hWndInst, nMsgNo, buf, sizeof(buf));

  return (MessageBox(hWnd, buf, szAppName, MB_OK  | MB_SYSTEMMODAL | nIcon));
  }


void ErrorMessage(msgno)
  int  msgno;
{
  char buf[80];

  LoadString(hWndInst, msgno, buf, sizeof(buf));

  MessageBeep(NULL);
  MessageBox(hWnd, buf, szAppName, MB_OK | MB_SYSTEMMODAL);
}

int WarningMessage(msgno)
  int  msgno;
{
  char buf[80];

  LoadString(hWndInst, msgno, buf, sizeof(buf));

  MessageBeep(NULL);
  return (MessageBox(hWnd, buf, szAppName, MB_OKCANCEL|MB_ICONEXCLAMATION | MB_SYSTEMMODAL));
}

void FatalError(errno)
  int  errno;
{
  char buf[80];

  sprintf(buf, "Fatal Error <%04u>\r\n", errno);

  write(3, buf, strlen(buf));

  MessageBox(hWnd, buf, szAppName, MB_OK | MB_SYSTEMMODAL);

  DestroyWindow(hWnd);
}


BOOL TermMacroCheck(message, wParam, macro)
  unsigned message;
  WORD	   wParam;
  char	   *macro;
{
  return FALSE;
}




/*****************************************************************************\
*
*
\*****************************************************************************/
void CmdPaste()
{
  HANDLE hText;
  LPSTR  lpText;
  char	 azBuf[1];


  if (OpenClipboard(hWnd) == FALSE)
    return;

  if ((hText = GetClipboardData(CF_TEXT)) != NULL)
    {
    if ((lpText = GlobalLock(hText)) != 0L)
      {
      while (*lpText)
        {
	azBuf[0] = *lpText++;
        TermWriteData(azBuf, 1, TRUE);
	}

      GlobalUnlock(hText);
      }
    }

  CloseClipboard();
}


/*****************************************************************************\
*
*
\*****************************************************************************/
void MenuPopupInit(WORD wIndex)
  {
  extern int hRecordFile;
  extern int hDiaryFile;

  switch (wIndex)
    {
    case 0:
      if (hRecordFile == 0 && hDiaryFile == 0)
        EnableMenuItem(hWndMenu, CMD_CLOSE, MF_GRAYED);
      else
        EnableMenuItem(hWndMenu, CMD_CLOSE, MF_ENABLED);
      break;

    case 1:
      if (fSelectedArea)
        EnableMenuItem(hWndMenu, CMD_COPY, MF_ENABLED);
      else
        EnableMenuItem(hWndMenu, CMD_COPY, MF_GRAYED);

      EnableMenuItem(hWndMenu, CMD_PASTE, MF_GRAYED);
      if (fSessOnline)
        {
        if (OpenClipboard(hWnd))
          {
          if (GetClipboardData(CF_TEXT))
            EnableMenuItem(hWndMenu, CMD_PASTE, MF_ENABLED);

          CloseClipboard();
          }
        }
      break;
    }
  }
