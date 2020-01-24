/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Miscellaneous Routines (PROMISC.C)                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains various subroutines that don't fit any-    *
*             where else.                                                     *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 08/02/89 00003 Fix OK/CANCEL problem with warning dialogs. *
*             1.01 09/05/89 00025 Add optional to CR/LF to host paste.        *
*             1.01 09/08/89 00033 Make for a smoother startup.                *
*  1.1  03/30/90 dca00062 PJL Fix XWP extension addition problem in File.Open *
*  1.1  04/02/90 dca00059 MKL added Win3.0 Icon				      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "xfer.h"
#include "kermit.h"
#include "global.h"
#include "variable.h"


typedef struct
  {
  char	*name;
  int	value;
  } TOKEN;

static TOKEN krmcmds[] =
  {
  "ALL",		TKN_ALL,
  "BATCH",		TKN_BATCH,
  "BAUD",		TKN_BAUD,
  "BELL",		TKN_BELL,
  "BLOCK-CHECK",	TKN_BLOCKCHECK,
  "BYE",		TKN_BYE,
  "CANCEL",		TKN_CANCEL,
  "CLEAR",		TKN_CLEAR,
  "COMMENT",		TKN_COMMENT,
  "CONNECT",		TKN_CONNECT,
  "CONTROL",            TKN_CONTROL,
  "CONVERTED",		TKN_CONVERTED,
  "COPY",		TKN_COPY,
  "CTRL-Z",		TKN_CTRLZ,
  "CWD",		TKN_CWD,
  "DEBUG",		TKN_DEBUG,
  "DEFINE",		TKN_DEFINE,
  "DELAY",		TKN_DELAY,
  "DELETE",		TKN_DELETE,
  "DIAL",		TKN_DIAL,
  "DIRECTORY",		TKN_DIRECTORY,
  "DISCARD",		TKN_DISCARD,
  "DISK",		TKN_DISK,
  "ECHO",		TKN_ECHO,
  "END-OF-PACKET",	TKN_ENDOFPACKET,
  "EOF",		TKN_EOF,
  "ERASE",		TKN_ERASE,
  "ERROR",		TKN_ERROR,
  "EVEN",		TKN_EVEN,
  "EXIT",		TKN_EXIT,
  "FILE",		TKN_FILE,
  "FINISH",		TKN_FINISH,
  "GET",		TKN_GET,
  "HANGUP",		TKN_HANGUP,
  "HELP",		TKN_HELP,
  "HOST",		TKN_HOST,
  "INCOMPLETE", 	TKN_INCOMPLETE,
  "INPUT",		TKN_INPUT,
  "INITIAL",		TKN_INITIAL,
  "KEEP",		TKN_KEEP,
  "KERMIT",		TKN_KERMIT,
  "LITERAL",		TKN_LITERAL,
  "LOCAL",		TKN_LOCAL,
  "LOGIN",		TKN_LOGIN,
  "NAMES",		TKN_NAMES,
  "NOCTRL-Z",		TKN_NOCTRLZ,
  "NONE",		TKN_NONE,
  "ODD",		TKN_ODD,
  "OFF",		TKN_OFF,
  "ON", 		TKN_ON,
  "OUTPUT",		TKN_OUTPUT,
  "PACKET-LENGTH",	TKN_PACKETLENGTH,
  "PACKETS",		TKN_PACKETS,
  "PAD-CHARACTER",	TKN_PADCHARACTER,
  "PADDING",		TKN_PADDING,
  "PARITY",		TKN_PARITY,
  "PAUSE",		TKN_PAUSE,
  "PRINT",		TKN_PRINT,
  "PROGRAM",		TKN_PROGRAM,
  "PUSH",		TKN_PUSH,
  "QUIT",		TKN_QUIT,
  "QUOTE",              TKN_QUOTE,
  "RECEIVE",		TKN_RECEIVE,
  "REMOTE",		TKN_REMOTE,
  "RENAME",		TKN_RENAME,
  "RETRY",		TKN_RETRY,
  "RUN",		TKN_RUN,
  "SCRIPT",		TKN_SCRIPT,
  "SEND",		TKN_SEND,
  "SERVER",		TKN_SERVER,
  "SESSION",		TKN_SESSION,
  "SET",		TKN_SET,
  "SHOW",		TKN_SHOW,
  "SPACE",		TKN_SPACE,
  "START-OF-PACKET",	TKN_STARTOFPACKET,
  "STATISTICS", 	TKN_STATISTICS,
  "SUBMIT",		TKN_SUBMIT,
  "SUPERSEDE",		TKN_SUPERSEDE,
  "TIMEOUT",		TKN_TIMEOUT,
  "TIMER",		TKN_TIMER,
  "TRANSMIT",		TKN_TRANSMIT,
  "TRANSACTIONS",	TKN_TRANSACTIONS,
  "TYPE",		TKN_TYPE,
  "WARNING",		TKN_WARNING,
  "WHO",		TKN_WHO
  };


void EditScript(szScript)
  char *szScript;
{
  static char szPath[MAX_PATH];

  if (szScript[0] == ' ')
    {
    szPath[0] = 0;
    szPath[1] = 0;
    }
  else
    {
    strcpy(&szPath[1], szScript);
    if (strchr(&szPath[1],  '.') == 0)
      strcat(&szPath[1], ".XWS");
    }

  szPath[0] = (char)(strlen(&szPath[1]+1));
  SetFullPath(VAR_DIRXWS);
  LockData(0);
  szEditor[szEditor[0]+1] = '\0';
  SpawnApp(&szEditor[1], szPath);
  UnlockData(0);
}


void SetDlgItemStr(hDlg, item, msg)
  HWND hDlg;
  int  item;
  int  msg;
{
  char buf[80];


  if (msg)
    LoadString(hWndInst, msg, buf, sizeof(buf));
  else
    buf[0] = NULL;

  SetDlgItemText(hDlg, item, buf);
}


int TranslateToken(group, token)
  int  group;
  char *token;
{
  TOKEN *tokens;
  int	entries;
  char	*ptr1;
  char	*ptr2;
  char	bFound;
  int	len, i;


  strupr(token);
  tokens  = krmcmds;
  entries = sizeof(krmcmds)/sizeof(TOKEN);

  for (i = 0; i < entries; i++)
    {
    ptr1 = token;
    ptr2 = tokens[i].name;

    bFound = TRUE;
    if (*ptr2 >= '0' && *ptr2 <= '9')
      {
      len = *ptr2 - '0';
      ptr2++;
      }
    else
      len = 0;

    while (*ptr1)
      {
      if (*ptr1++ != *ptr2++)
	{
	bFound = FALSE;
	break;
	}

      len--;
      }

    if (bFound && len <= 0)
      return (tokens[i].value);
    }

  return (NULL);
}


void SystemUpdate()
{
  SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
}


void UpdateTitle()
  {
  char szNewTitle[64], szCurTitle[64];


  strcpy(szNewTitle, szAppTitle);

  if (!fUntitled)
    {
    strcat(szNewTitle, " - ");
    strcat(szNewTitle, szDataCurFile);
    }

  GetWindowText(hWnd, szCurTitle, sizeof(szCurTitle));

  if (strcmp(szCurTitle, szNewTitle))  // 00033 Only update if different.
    SetWindowText(hWnd, szNewTitle);
  }


void IconTextChanged(pText)
  PSTR pText;
{
  HDC  hDC;
  char szBuf[8];
  int  i, nXPos;

  // dca00059 MKL added Win3.0 Icon
  char szNewTitle[64];

  if (pText == NULL)
    {
    if (wSystem & SS_DDE)
      {
      szBuf[0] = 'D';
      szBuf[1] = 'D';
      szBuf[2] = 'E';
      }
    else if (wSystem & SS_SCRIPT)
      {
      szBuf[0] = 'E';
      szBuf[1] = 'X';
      szBuf[2] = 'E';
      }
    else
      {
      szBuf[0] = 'C';
      szBuf[1] = 'O';
      szBuf[2] = 'M';
      }

    if (Comm.cPort != -1)
      cCommPort == Comm.cPort;

    szBuf[3] = (char)'1' + cCommPort;
    szBuf[4] = NULL;
    pText = szBuf;
    }

  // dca00060 MKL added Win3.0 Icon
  if (IsIconic(hWnd) && sVersion < 3)
    {
    hDC = GetDC(hWnd);

    nXPos = nIconTextX;
    for (i = 0; i < 4; i++)
      {
      if (szIconText[i] != *pText) {
	szIconText[i] = *pText;
	TextOut(hDC, nXPos, nIconTextY, pText, 1);
      }

      pText++;
      nXPos += nSysFontWidth;
      }

    ReleaseDC(hWnd, hDC);
    }
  else if (IsIconic(hWnd)) {
     strcpy(szNewTitle, szAppName);
     strcat(szNewTitle, " - ");
     strcat(szNewTitle, szDataCurFile);
     strcat(szNewTitle, " ");
     strcat(szNewTitle, pText);
     SetWindowText(hWnd, szNewTitle);
  }
  else
    strcpy(szIconText, pText);
}

void ResetScreenDisplay()
{
  RECT Rect;

  InvalidateRect(hChildTerminal, NULL, TRUE);

  GetClientRect(hWnd, (LPRECT)&Rect);
  PostMessage(hWnd, WM_SIZE, fMaximized ? 2 : 0,
	      MAKELONG(Rect.right - Rect.left, Rect.bottom - Rect.top));
}


/*---------------------------------------------------------------------------*/
/*  AskUserMessageStr()                                                      */
/*                                                                           */
/*  Description:                                                             */
/*    Formats the message from the message no and a string parameter and     */
/*    ask the user the question.                                             */
/*                                                                           */
/*  Return:                                                                  */
/*    ID_OK or ID_CANCEL from the MessageBox() subroutine.                   */
/*---------------------------------------------------------------------------*/
short AskUserMessageStr(short sMsgNo, char *szParam)
  {
  char szBuf[128], szFmt[64];

  LoadString(hWndInst, sMsgNo, szFmt, sizeof(szFmt));
  sprintf(szBuf, szFmt, szParam);

  return (MessageBox(hWnd , szBuf, szAppName,
                     MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL));
  }


/*---------------------------------------------------------------------------*/
/*  TellUserMessageStr()						     */
/*                                                                           */
/*  Description:                                                             */
/*    Formats the message from the message no and a string parameter and     */
/*    tell the user about the situation.				     */
/*                                                                           */
/*  Return:                                                                  */
/*    ID_OK or ID_CANCEL from the MessageBox() subroutine.                   */
/*---------------------------------------------------------------------------*/
short TellUserMessageStr(short sMsgNo, char *szParam)
  {
  char szBuf[128], szFmt[64];

  LoadString(hWndInst, sMsgNo, szFmt, sizeof(szFmt));
  sprintf(szBuf, szFmt, szParam);

  return (MessageBox(hWnd , szBuf, szAppName,
		     MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL));
  }


void ErrorMsg(msg, errno)
  char *msg;
  int  errno;
{
  char buf[256];

  sprintf(buf,"%s, %d", msg, errno);
  MessageBeep(NULL);
  MessageBox(hWnd ,(LPSTR)buf, szAppName, MB_OK | MB_SYSTEMMODAL);
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
  return (MessageBox(hWnd, buf, szAppName,
//	  MB_OKCANCEL|MB_ICONEXCLAMATION | MB_SYSTEMMODAL)); // 0003 old code
	  MB_OK|MB_ICONEXCLAMATION | MB_SYSTEMMODAL)); // 00003 Ok/Cancel fix
}

int CancelMessage(int msgno)
  {
  char buf[80];

  LoadString(hWndInst, msgno, buf, sizeof(buf));

  MessageBeep(NULL);
  return (MessageBox(hWnd, buf, szAppName,
	  MB_OKCANCEL|MB_ICONEXCLAMATION | MB_SYSTEMMODAL));
  }

void InfoMessage(msgno)
  int  msgno;
{
  char buf[80];

  LoadString(hWndInst, msgno, buf, sizeof(buf));
  InfoDisplay(buf);
}

void FatalError(errno)
  int  errno;
{
  char buf[80];

  sprintf(buf, "Fatal Error <%04u>\r\n", errno);

  //write(3, buf, strlen(buf));

  MessageBox(hWnd, buf, szAppName, MB_OK | MB_SYSTEMMODAL);

  DestroyWindow(hWnd);
}


void CmdPlayback()
{
  int extfile;
  char szFileNameIn[128], szExtIn[50], pszAppnameIn[60];
  OFSTRUCT FileInfo;


  if (hDemoFile != -1)
    {
    InfoDisplay(NULL);
    CheckMenuItem(hWndMenu, CMD_PLAYBACK, MF_UNCHECKED);
    DosClose(hDemoFile);
    hDemoFile = -1;
    FlushComm(nCommID, 0);
    FlushComm(nCommID, 1);
    return;
    }

  SetScrollLock(hWnd, FALSE);

  strcpy(szFileNameIn, "");
  strcpy(szExtIn, "\\*.*");
  strcpy(pszAppnameIn, "Playback File");

  SetFullPath(VAR_DIRFIL);

  // dca00062 PJL Fix XWP extension addition problem in File.Open.
  if (DlgOpen(hWndInst, hWnd, IDD_OPENDEMO, (OFSTRUCT *)&FileInfo, &extfile,
          szFileNameIn, szExtIn, pszAppnameIn, FALSE, FALSE) == NOOPEN)
    return;

  if (extfile > 0)
    {
    UpdateTerminal();
    UpdateWindow(hChildTerminal);

    hDemoFile = extfile;
    CheckMenuItem(hWndMenu, CMD_PLAYBACK, MF_CHECKED);
    InfoMessage(MSGINF_PLAYBACK);
    fDemoPause = FALSE;
    }
  else
    {
    GeneralError(ERRDOS_NOFILE);
    }
}

BOOL TermMacroCheck(message, wParam, macro)
  unsigned message;
  WORD	   wParam;
  char	   *macro;
{
  return FALSE;
}

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
        if (Term.fNewLine || *lpText != '\n')
          {
          azBuf[0] = *lpText++;
          CommDataOutput(azBuf, 1);
          if (Term.fEcho)
            SendMessage(hChildTerminal, UM_DATA, 1, (DWORD)((LPSTR)azBuf));
          }
        else
          lpText++;
	}

      GlobalUnlock(hText);

      // 00025 Add optional to CR/LF to host paste.
      if (fPasteCrLf)
        {
        CommDataOutput(&Xfer.szLineDelim[1], (int)(Xfer.szLineDelim[0]));
        if (Term.fEcho)
          SendMessage(hChildTerminal, UM_DATA, (int)(Xfer.szLineDelim[0]),
                      (DWORD)((LPSTR)(&Xfer.szLineDelim[1])));
        }
      }
    }

  CloseClipboard();
}


void CmdPasteUpload()
{
  HANDLE hText;
  LPSTR  lpText;
  HFILE  hFile;
  USHORT usBytes, usAction;
  char   name[128];
  char	 buf[256];


  if (OpenClipboard(hWnd) == FALSE)
    return;

  if ((hText = GetClipboardData(CF_TEXT)) != NULL)
    {
    if ((lpText = GlobalLock(hText)) != 0L)
      {
      GetTempFileName(0, "PAS", 0, name);
      if (!DosOpen(name, &hFile, &usAction, 0L, FILE_NORMAL,
		   FILE_OPEN | FILE_CREATE, OPEN_ACCESS_READWRITE, 0L))
	{
	while (*lpText)
	  {
	  register i;

	  for (i = 0; i < sizeof(buf) && *lpText; i++)
	    buf[i] = *lpText++;

	  if (i)
	    DosWrite(hFile, buf, i, &usBytes);
	  }

	DosClose(hFile);
	XferASCIIUploadName(name);
	}
      else
	DosDelete(name, 0L);

      GlobalUnlock(hText);
      }
    }

  CloseClipboard();
}
