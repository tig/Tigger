/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: DDE Support (PRODDE.C)                                           */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "lexis.h"
#include "session.h"
#include "library.h"
#include "comm.h"
#include "data.h"
#include "term.h"
#include "robot.h"


/*****************************************************************************\
*                                                                             *
*  Communication Link Data Structure Definitions (DDE).                       *
*                                                                             *
\*****************************************************************************/
#define ACCEPTED	    0x8000
#define REJECTED	    0x0000
#define BUSY		    0x4000

#define WM_DDE_FIRST	    0x03E0
#define WM_DDE_INITIATE     0x03E0
#define WM_DDE_TERMINATE    0x03E1
#define WM_DDE_ADVISE	    0x03E2
#define WM_DDE_UNADVISE     0x03E3
#define WM_DDE_ACK	    0x03E4
#define WM_DDE_DATA	    0x03E5
#define WM_DDE_REQUEST	    0x03E6
#define WM_DDE_POKE	    0x03E7
#define WM_DDE_EXECUTE	    0x03E8
#define WM_DDE_LAST	    0x03E8

typedef struct
  {
  WORD	fEmpty:12;
  WORD	fResponse:1;
  WORD	fRelease:1;
  WORD	fNoData:1;
  WORD	fAck:1;
  WORD	cfFormat;
  BYTE	info[30];
  } DDEDATA;

typedef DDEDATA *PDDEDATA;
typedef DDEDATA FAR *LPDDEDATA;


typedef struct
  {
  BYTE	fAdvised:1;
  BYTE	fAckFlow:1;
  BYTE	fUpdated:1;
  BYTE	fNullMem:1;
  } DDEVAR;


/*****************************************************************************\
*                                                                             *
*  Global variables and defines.                                              *
*                                                                             *
\*****************************************************************************/
#define MAX_CHANNELS 9

#define ERROR_PARAM 1
#define ERROR_CMD   2

  ATOM	 aApp;
  ATOM	 aTopic;
  ATOM	 aSysItems;
  ATOM	 aTopics;
  ATOM	 aStatus;
  ATOM	 aFormats;

  int	 nDDESessions;

  int	 nChannel;
  HWND	 hWndDDEs[MAX_CHANNELS];
  ATOM	 aInitApp;
  ATOM	 aInitTopic;
  int	 nInitChannel;
  ATOM	 aPokeItem;
  HANDLE hPokeMem;
  ATOM	 aRequestItem;
  int	 nRequestVar;
  HANDLE hExecuteCmd;

  char   ScrBuf[2048];


BOOL far pascal SendDDEData(HWND, WORD, char *, BOOL);
static int near pascal	ExecuteCommands(char *);
static void near pascal GlobalReAddAtom(ATOM);


void far pascal ProcessDDE(hWnd, message, wParam, lParam)
  HWND	   hWnd;
  unsigned message;
  WORD	   wParam;
  LONG	   lParam;
{
  HANDLE    hMem;
  LPDDEDATA lpData;
  LPSTR     lpStr;
  WORD	    wLow, wHigh, wResponse;
  BYTE	    buf[256], bLen;
  long	    lInteger;
  int	    num, len;
  char      fRelease;
  register  int i;


  for (nChannel = 0; nChannel < MAX_CHANNELS; nChannel++)
    if (wParam == hWndDDEs[nChannel])
      break;

  wLow	    = LOWORD(lParam);
  wHigh     = HIWORD(lParam);
  wResponse = REJECTED;


  switch (message)
    {
    case WM_DDE_INITIATE:
      if (aApp != wLow || hWndDDEs[0] || wParam == hWnd)
	return;

      hWndDDEs[0] = wParam;
      GlobalReAddAtom(aApp);
      GlobalReAddAtom(aTopic);
      SendMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(aApp, aTopic));
      nDDESessions++;

      SetSystemStatus(SS_DDE);
      break;

    case WM_DDE_ADVISE:
      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
      break;

    case WM_DDE_UNADVISE:
      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
      break;

    case WM_DDE_TERMINATE:
      if (hWndDDEs[nChannel] && nChannel < MAX_CHANNELS)
	TerminateDDE(nChannel);

      if (nDDESessions)
	nDDESessions--;

      if ((wSystem & SS_ABORT) && nDDESessions == 0)
	DestroyWindow(hWnd);
      break;

    case WM_DDE_REQUEST:
      if (wLow == CF_TEXT)
	{
        GlobalGetAtomName(wHigh, buf, sizeof(buf));
        if (stricmp("sysitems", buf) == 0)
	  {
          strcpy(buf, "SysItems\tTopics\tStatus\tFormats");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
        else if (stricmp("topics", buf) == 0)
	  {
          strcpy(buf, "System");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
        else if (stricmp("status", buf) == 0)
	  {
          if (!fSessOnline)
            strcpy(buf, "Offline");
          else if (wSessKeyboard == KEYBD_LOCAL && bWorkLogonSent)
            strcpy(buf, "Ready");
	  else
            strcpy(buf, "Busy");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
        else if (stricmp("formats", buf) == 0)
	  {
          strcpy(buf, "Text");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
        else if (stricmp("robot", buf) == 0)
          {
          strcpy(buf, szRobotStatus);
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
        else if (stricmp("L2KIXE", buf) == 0)
          {
          strcpy(buf, szRobotStatus);
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
        else if (stricmp("timeout", buf) == 0)
          {
          itoa(usWorkTimeout, buf, 10);
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }

	i = GlobalGetAtomName(wHigh, buf, sizeof(buf));
        if (buf[0] == '"')
          {
          for (i = 1; buf[i]; i++)
            buf[i-1] = buf[i];
          buf[i-1] = '\0';
          }

        if (stricmp(buf, "client") == 0)
	  {
          if (SendDDEData(wParam, wHigh, szWorkClient, TRUE))
            break;
          }
        else if (strnicmp(buf, "line", 4) == 0)
          {
          char *ptr;

          i = atoi(&buf[4]);
          if (i >= 1 && i <= 24)
            {
            ptr = ScrBuf;
            TermQueryLine(i-1, ptr);
            while (*ptr)
              ptr++;
            *ptr++ = ASC_CR;
            *ptr++ = ASC_LF;
            *ptr++ = '\0';
            if (SendDDEData(wParam, wHigh, ScrBuf, TRUE))
              break;
            }
          }
        else if (stricmp(buf, "screen") == 0)
          {
          char *ptr;

          ptr = ScrBuf;
          for (i = 0; i < 24; i++)
            {
            TermQueryLine(i, ptr);
            while (*ptr)
              ptr++;
            *ptr++ = ASC_CR;
            *ptr++ = ASC_LF;
            }

          if (SendDDEData(wParam, wHigh, ScrBuf, TRUE))
            break;
          }
	}

      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
      break;

    case WM_DDE_POKE:
      if (nChannel >= MAX_CHANNELS)
	{
	PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
	break;
	}

      fRelease = FALSE;
      if ((lpData = (LPDDEDATA)GlobalLock(wLow)) != NULL)
	{
	if (lpData->cfFormat == CF_TEXT)
	  {
          GlobalGetAtomName(wHigh, buf, sizeof(buf));
          if (stricmp("timeout", buf) == 0)
            {
            lstrcpy(buf, lpData->info);
            i = atoi(buf);
            if (i >= 5 && i <= 30)
              usWorkTimeout = i;
            }

	  wResponse = ACCEPTED;
	  }

        if (wResponse == ACCEPTED && lpData->fRelease)
          fRelease = TRUE;

        GlobalUnlock(wLow);
	}

      if (fRelease)
        GlobalFree(wLow);

      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
      break;

    case WM_DDE_DATA:
      fRelease = FALSE;

      if ((lpData = (LPDDEDATA)GlobalLock(wLow)) != NULL)
	{
	if (lpData->cfFormat == CF_TEXT)
	  {
	  if (i && i != -1)
	    {
	    register j;

	    lstrcpy(buf, lpData->info);
	    j = strlen(buf);
	    if (buf[j-2] == ASC_CR && buf[j-1] == ASC_LF)
	      buf[j-2] = NULL;
	    }

	  wResponse = ACCEPTED;
	  }

        if (lpData->fAck)
          {
	  PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
          if (wResponse == ACCEPTED && lpData->fRelease)
            fRelease = TRUE;
          }
	else
	  {
          fRelease = TRUE;
	  GlobalDeleteAtom(wHigh);
	  }

        GlobalUnlock(wLow);
        if (fRelease)
	  GlobalFree(wLow);
	}
      break;

    case WM_DDE_ACK:
      if (wLow == aInitApp && wHigh == aInitTopic && wLow && wHigh)
	{
	nDDESessions++;

	GlobalDeleteAtom(wLow);
	GlobalDeleteAtom(wHigh);

	if (hWndDDEs[nInitChannel])
	  PostMessage(wParam, WM_DDE_TERMINATE, hWnd, MAKELONG(0, 0));
	else
	  hWndDDEs[nInitChannel] = wParam;

	break;
	}

      if (wHigh == aPokeItem && aPokeItem != NULL)
	{
	GlobalDeleteAtom(aPokeItem);
	aPokeItem = NULL;
	GlobalFree(hPokeMem);
	}
      else if (wHigh == aRequestItem && aRequestItem != NULL)
	{
	GlobalDeleteAtom(aRequestItem);
	aRequestItem = NULL;
	}
      else if (wHigh == hExecuteCmd && hExecuteCmd != NULL)
	{
	GlobalFree(hExecuteCmd);
	hExecuteCmd = NULL;
	}
      break;

    case WM_DDE_EXECUTE:
      if (nChannel >= MAX_CHANNELS)
	{
	PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
	break;
	}

      if (wSystem & SS_SCRIPT)
	wResponse = BUSY;
      else if (lpStr = (LPSTR)GlobalLock(wHigh))
	{
	for (i = 0; lpStr[i] && i < 255; i++)
	  buf[i] = lpStr[i];
	buf[i] = NULL;

        if (ExecuteCommands(buf) == 0)
          wResponse = ACCEPTED;
        else
          wResponse = BUSY;

        GlobalUnlock(wHigh);
	}

      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
      break;
    }
}


BOOL far pascal InitDDE()
{
  aApp	    = GlobalAddAtom(szAppName);
  aTopic    = GlobalAddAtom("System");
  aSysItems = GlobalAddAtom("SysItems");
  aTopics   = GlobalAddAtom("Topics");
  aStatus   = GlobalAddAtom("Status");
  aFormats  = GlobalAddAtom("Formats");

  nDDESessions = 0;

  memset(hWndDDEs, 0, sizeof(hWndDDEs));
  aInitApp	= NULL;
  aInitTopic	= NULL;
  aPokeItem	= NULL;
  aRequestItem	= NULL;
  hExecuteCmd	= NULL;

  return (TRUE);
}

void far pascal TermDDE()
{
  register i;

  for (i = 0; i < MAX_CHANNELS; i++)
    TerminateDDE(i);
}


BOOL far pascal SendDDEData(hWndDDE, wAtom, buf, bRequest)
  HWND hWndDDE;
  WORD wAtom;
  char *buf;
  BOOL bRequest;
{
  HANDLE    hMem;
  DWORD     dwSize;
  LPDDEDATA lpData;
  BOOL	    bResult;
  int	    len;


  bResult = FALSE;

  len = strlen(buf) + 1;
  dwSize = 4 + len;
  hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dwSize);

  if (hMem)
    {
    if (lpData = (LPDDEDATA)GlobalLock(hMem))
      {
      register int i;

      lpData->fRelease	= TRUE;
      lpData->fResponse = bRequest;
      lpData->cfFormat	= CF_TEXT;

      for (i = 0; i < len; i++)
	lpData->info[i] = buf[i];

      GlobalUnlock(hMem);

      bResult = PostMessage(hWndDDE,WM_DDE_DATA,hWnd,MAKELONG(hMem,wAtom));
      }
    else
      GlobalFree(hMem);
    }

  return (bResult);
}

static int near pascal ExecuteCommands(cmd)
  char *cmd;
{
  char command[64], parameter[256];

  while (*cmd)
    {
    register int i;

    /* Search for '[' for beginning of command */
    while (*cmd != '[' && *cmd != NULL)
      cmd++;

    if (*cmd == NULL)
      break;

    cmd++;

    /* Copy command to buffer until '(' or ']' */
    i = 0;
    while (*cmd != ']' && *cmd != '(' && *cmd != NULL)
      {
      if (i < 15)
	command[i++] = toupper(*cmd);
      cmd++;
      }
    command[i] = NULL;

    if (*cmd == '(')
      {
      cmd++;
      i = 0;
      while (*cmd != ']' && *cmd != ')' && *cmd != NULL)
	{
	if (i < sizeof(parameter) - 1)
	  parameter[i++] = toupper(*cmd);
	cmd++;
	}
      parameter[i] = NULL;
      }

    switch (command[0])
      {
      case 'E':
        if (wSessKeyboard != KEYBD_LOCAL)
          return (BUSY);

        SessTellGeneric(parameter, strlen(parameter), "Remote Command");
        break;

      case 'R':
      case 'L':
        if (!RobotDdeCommand(parameter))
          return (BUSY);
        break;

      case 'O':
        if (!fSessOnline)
          CmdSignon();
        break;

      case 'C':
        switch (command[1])
          {
          case 'L':
            if (fSessOnline)
              {
              SessTellDisconnect(FALSE);
              VidClearPage();
              }
            break;

          case 'O':
            CmdCopy();
            break;
          }
        break;
      }
    }

  return (0);
}

int far pascal TerminateDDE(channel)
  int channel;
{

  if (channel >= 0 && channel < MAX_CHANNELS && hWndDDEs[channel])
    {
    if (channel != 2)
      PostMessage(hWndDDEs[channel], WM_DDE_TERMINATE, hWnd, 0L);
    else
      PostMessage(hWndDDEs[channel], WM_DDE_TERMINATE, hChildInfo, 0L);

    hWndDDEs[channel] = NULL;

    if (channel == 0)
      ResetSystemStatus(SS_DDE);

    return (0);
    }

  return (0);
}

static void near pascal GlobalReAddAtom(atom)
  ATOM atom;
{
  char buf[64];

  GlobalGetAtomName(atom, buf, sizeof(buf));
  GlobalAddAtom(buf);
}
