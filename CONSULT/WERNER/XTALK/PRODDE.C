/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   The DDE Processor (PRODDE.C)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports the Microsoft/Lotus Dyanmic Data Exchange  *
*             of better known as DDE.  We support full DDE except for advise  *
*             which is rather dependent of the type of data.  Since advising  *
*             is not really needed in a communication application and that it *
*             uses lots of resources and would make us slower for the other   *
*             99.5% users, thus better to be fast...                          *
*                                                                             *
*             The famous FISH.EXE demos has a number of bugs in it, one is    *
*             that it must use different windows for the 'fish' and 'sub'     *
*             topics.  So you may see some weird logic that uses the input    *
*             window to simulate the second DDE connect and to make fishy     *
*             work.                                                           *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*  dca00066 JDB 04/03/90 Added DDE Advise                                     *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "interp.h"
#include "kermit.h"
#include "variable.h"
#include "sess.h"
#include "ddeadv.h"	/* dca00066 JDB DDE Advise */


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
  ATOM	 aAdviseItem;
  ATOM	 aAdviseItems[26];
  HANDLE hAdviseMem;
  ATOM	 aUnadviseItem;


BOOL SendDDEData(HWND, WORD, char *, BOOL);
static int near  ExecuteCommands(char *);
static void near GlobalReAddAtom(ATOM);

void ProcessDDE(hWnd, message, wParam, lParam)
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
  char      szAdviseName[256];	//dca00066 JDB DDE Advise max size of a variable
  DDE_ADV_DATA dde_adv_data;	//dca00066 JDB DDE Advise
  int varno;			//dca00066 JDB DDE Advise variable number


  for (nChannel = 0; nChannel < MAX_CHANNELS; nChannel++)
    if (wParam == hWndDDEs[nChannel])
      break;

  wLow	    = LOWORD(lParam);
  wHigh     = HIWORD(lParam);
  wResponse = REJECTED;


  switch (message)
    {
    case WM_DDE_INITIATE:
      if (aApp != wLow || hWndDDEs[0] || bConnected || wParam == hWnd)
	return;

      hWndDDEs[0] = wParam;
      GlobalReAddAtom(aApp);
      GlobalReAddAtom(aTopic);
      SendMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(aApp, aTopic));
      nDDESessions++;

      SetSystemStatus(SS_DDE);
      break;

	/* dca00066 WM_DDE_ADVISE and WM_DDE_UNADVISE Completely NEW. -JDB */
    case WM_DDE_ADVISE:		//dca00066 JDB DDE Advise
      GlobalGetAtomName(wHigh,szAdviseName,sizeof(szAdviseName));
      hAdviseMem = wLow;
      dde_adv_data = *(DDE_ADV_DATA_PTR)GlobalLock(hAdviseMem);
      GlobalUnlock(hAdviseMem);
      
      /* Check for bad format */
      if(dde_adv_data.cfFormat != CF_TEXT)
      {
	/* We only support CF_TEXT */
	      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
	      break;
      }

      /* check for non-existant variable */
      upcase(szAdviseName);
      varno = GetVariableHandle(szAdviseName);
      if(varno == -1)
      {
	      /* This is not an available variable */
	      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(REJECTED, wHigh));
	      break;
      }

      /* we are OK so release the global memory item and ACK */
      GlobalFree(hAdviseMem);
      add_advise(szAdviseName,varno,wParam,dde_adv_data);
      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(ACCEPTED, wHigh));
      break;

    case WM_DDE_UNADVISE:	//dca00066 JDB DDE Advise
      if(wHigh == NULL)
      {
	remove_advise(-1,wParam);	/* Dump all advise for client */
	PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(ACCEPTED, wHigh));
	break;
      }

      /* This must be a specific variable */
      GlobalGetAtomName(wHigh,szAdviseName,sizeof(szAdviseName));
      upcase(szAdviseName);
      varno = GetVariableHandle(szAdviseName);
      if(varno == -1)
      {
	      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(REJECTED, wHigh));
	      break;
      }
  
      if(advised_on(varno,wParam) != NULL)
      {
	      remove_advise(varno,wParam);
	      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(ACCEPTED, wHigh));
      }
      else
      {
	      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(REJECTED, wHigh));
      }
      break;


    case WM_DDE_TERMINATE:
      if (hWndDDEs[nChannel] && nChannel < MAX_CHANNELS)
	TerminateDDE(nChannel);

      if (nDDESessions)
	nDDESessions--;

      if ((wSystem & SS_ABORT) && nDDESessions == 0)
	DestroyWindow(hWnd);
        remove_advise(-1,wParam);  //dca00066 JDB Kill connections for client
      break;

    case WM_DDE_REQUEST:
      if (wLow == CF_TEXT)
	{
	if (wHigh == aSysItems)
	  {
          strcpy(buf, "SysItems\tTopics\tStatus\tFormats");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
	else if (wHigh == aTopics)
	  {
          strcpy(buf, "System");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
	else if (wHigh == aStatus)
	  {
	  if (wSystem & SS_SCRIPT)
            strcpy(buf, "Script");
          else if (wSystem & SS_XFER)
            strcpy(buf, "Transfer");
          else if (nKermitMode == KM_IDLE)
            strcpy(buf, "Kermit");
	  else
            strcpy(buf, "Ready");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }
	else if (wHigh == aFormats)
	  {
          strcpy(buf, "Text");
	  if (SendDDEData(wParam, wHigh, buf, TRUE))
	    break;
	  }

	i = GlobalGetAtomName(wHigh, buf, sizeof(buf));
	if ((i = IdentifyVariable(buf)) != -1)
	  {
	  if (i & 0x8000)
	    {
	    if ((i = GetSysVariableAny(i, buf, &bLen, &lInteger)) == 1)
	      {
	      ltoa(lInteger, buf, 10);
	      i = 0;
              }
            else
              buf[bLen] = NULL;
	    if (i == 0)
	      {
	      if (SendDDEData(wParam, wHigh, buf, TRUE))
		break;
	      }
	    }
	  else
	    {
	    GetStrVariable(i, buf, 255);
	    if (SendDDEData(wParam, wHigh, buf, TRUE))
	      break;
	    }
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
	  register int j;

	  i = GlobalGetAtomName(wHigh, buf, sizeof(buf));

	  if ((i = IdentifyVariable(buf)) == -1)
	    i = CreateVariableHandle(buf, LEVEL_GLOBAL);

	  lstrcpy(buf, lpData->info);
	  j = strlen(buf);
	  if (buf[j-2] == ASC_CR && buf[j-1] == ASC_LF)
	    buf[j-2] = NULL;
	  if (i & 0x8000)
	    {
	    if (buf[0] != '"')
	      PutSysVariableAny(i, buf, (BYTE)strlen(buf), 0L);
	    else
	      PutSysVariableAny(i, &buf[1], (BYTE)strlen(buf)-2, 0L);
	    }
	  else
	    {
	    if (IsStrVariable(i))
	      {
	      if (buf[0] != '"')
		SetStrVariable(i, buf, strlen(buf));
	      else
		SetStrVariable(i, &buf[1], strlen(buf)-2);
	      }
	    else
	      {
	      if (buf[0] != '"')
		lInteger = atol(buf);
	      else
		lInteger = atol(&buf[1]);
	      SetIntVariable(i, lInteger);
	      }
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
      nErrorCode = 0;
      fRelease = FALSE;

      if ((lpData = (LPDDEDATA)GlobalLock(wLow)) != NULL)
	{
	if (lpData->cfFormat == CF_TEXT)
	  {
          i = nRequestVar;
          aRequestItem = NULL;

	  if (i && i != -1)
	    {
	    register j;

	    lstrcpy(buf, lpData->info);
	    j = strlen(buf);
	    if (buf[j-2] == ASC_CR && buf[j-1] == ASC_LF)
	      buf[j-2] = NULL;
	    SetStrVariable(i, buf, strlen(buf));
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
      else
	nErrorCode = ERRDDE_JUNK;
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
	if (wLow & ACCEPTED)
	  nErrorCode = 0;
	else if (wLow & BUSY)
	  nErrorCode = ERRDDE_BUSY;
	else
	  nErrorCode = ERRDDE_REJECTED;

	GlobalDeleteAtom(aPokeItem);
	aPokeItem = NULL;
	GlobalFree(hPokeMem);
	}
      else if (wHigh == aRequestItem && aRequestItem != NULL)
	{
	if (wLow & BUSY)
	  nErrorCode = ERRDDE_BUSY;
	else
	  {
	  nErrorCode = ERRDDE_REJECTED;
	  }

	GlobalDeleteAtom(aRequestItem);
	aRequestItem = NULL;
	}
      else if (wHigh == aAdviseItem && aAdviseItem != NULL)
	{
	if (wLow & ACCEPTED)
	  nErrorCode = 0;
	else if (wLow & BUSY)
	  nErrorCode = ERRDDE_BUSY;
	else
	  nErrorCode = ERRDDE_REJECTED;

	/* dca00066 JDB DDE Advise Removed this block */
	//dca00066 JDB if (nErrorCode)
	//dca00066 JDB {
	//dca00066 JDB 	GlobalDeleteAtom(aAdviseItem);
	//dca00066 JDB 	for (i = 0; i < 26; i++)
	//dca00066 JDB 	if (aAdviseItems[i] == aAdviseItem)
	//dca00066 JDB 	aAdviseItems[i] = NULL;
	//dca00066 JDB }

	GlobalFree(hAdviseMem);

	//dca00066 JDB MessageBox(hWnd,"Deleting Atom","DDE ACK Message",MB_OK);
	GlobalDeleteAtom(aAdviseItem); //dca00066 JDB DDE Advise
	aAdviseItem = NULL;
	}
      else if (wHigh == aUnadviseItem && aUnadviseItem != NULL)
	{
	if (wLow & ACCEPTED)
	  nErrorCode = 0;
	else if (wLow & BUSY)
	  nErrorCode = ERRDDE_BUSY;
	else
	  nErrorCode = ERRDDE_REJECTED;

	GlobalDeleteAtom(aUnadviseItem);
	  for (i = 0; i < 26; i++)
	    if (aAdviseItems[i] == aUnadviseItem)
	      aAdviseItems[i] = NULL;

	aUnadviseItem = NULL;
	}
      else if (wHigh == hExecuteCmd && hExecuteCmd != NULL)
	{
	if (wLow & ACCEPTED)
	  nErrorCode = 0;
	else if (wLow & BUSY)
	  nErrorCode = ERRDDE_BUSY;
	else
	  nErrorCode = ERRDDE_REJECTED;

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

	ExecuteCommands(buf);

	wResponse = ACCEPTED;
        GlobalUnlock(wHigh);
	}

      PostMessage(wParam, WM_DDE_ACK, hWnd, MAKELONG(wResponse, wHigh));
      break;
    }
}


BOOL InitDDE()
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

  aAdviseItem	= NULL;
  memset(aAdviseItems, 0, sizeof(aAdviseItems));
  aUnadviseItem = NULL;

  return (TRUE);
}

void TermDDE()
{
  register i;

  for (i = 0; i < MAX_CHANNELS; i++)
    TerminateDDE(i);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  int Initiate(app, topic)                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
int InitiateDDE(app, topic)
  char *app;
  char *topic;
{
  register i;


  nErrorCode = 0;

  for (i = 1; i < MAX_CHANNELS; i++)
    if (hWndDDEs[i] == NULL)
      break;

  if (i >= MAX_CHANNELS)
    {
    nErrorCode = ERRDDE_NOCHANNEL;
    return (0);
    }

  aInitApp   = GlobalAddAtom(app);
  aInitTopic = GlobalAddAtom(topic);

  nInitChannel = i;

  if (i != 2)
    SendMessage(-1, WM_DDE_INITIATE, hWnd, MAKELONG(aInitApp, aInitTopic));
  else
    SendMessage(-1, WM_DDE_INITIATE, hChildInfo, MAKELONG(aInitApp, aInitTopic));

  if (hWndDDEs[i] == NULL)
    {
    i = -1;
    nErrorCode = ERRDDE_NOINIT;
    }

  GlobalDeleteAtom(aInitApp);
  GlobalDeleteAtom(aInitTopic);

  aInitApp   = NULL;
  aInitTopic = NULL;

  return (i);
}

int PokeDDE(channel, item, data)
  int  channel;
  char *item;
  char *data;
{
  LPDDEDATA lpData;


  if (channel < 0 || channel >= MAX_CHANNELS || hWndDDEs[channel] == 0)
    return (ERRDDE_NOCHANNEL);

  if ((aPokeItem = GlobalAddAtom(item)) == NULL)
    return (ERRINT_MEMORY);

  if (hPokeMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD)(4 + strlen(data) + 1)))
    {
    lpData = (LPDDEDATA)GlobalLock(hPokeMem);
    *(LPSTR)lpData = NULL;

    lpData->fRelease = FALSE;
    lpData->cfFormat = CF_TEXT;

    lstrcpy(lpData->info, (LPSTR)data);

    GlobalUnlock(hPokeMem);

    if (channel != 2)
      {
      if (PostMessage(hWndDDEs[channel],WM_DDE_POKE,hWnd,MAKELONG(hPokeMem,aPokeItem)))
	return (0);
      else
	return (ERRDDE_POST);
      }
    else
      {
      if (PostMessage(hWndDDEs[channel],WM_DDE_POKE,hChildInfo,MAKELONG(hPokeMem,aPokeItem)))
	return (0);
      else
	return (ERRDDE_POST);
      }
    }
  else
    return (ERRINT_MEMORY);
}

int RequestDDE(channel, item, var)
  int  channel;
  char *item;
  int  var;
{
  nErrorCode = 0;

  if (channel < 0 || channel >= MAX_CHANNELS || hWndDDEs[channel] == 0)
    {
    nErrorCode = ERRDDE_NOCHANNEL;
    return (0);
    }

  aRequestItem = GlobalAddAtom(item);
  nRequestVar  = var;

  if (channel != 2)
    {
    if (PostMessage(hWndDDEs[channel],WM_DDE_REQUEST,hWnd,MAKELONG(CF_TEXT,aRequestItem)))
      return (0);
    else
      {
      GlobalDeleteAtom(aRequestItem);
      aRequestItem = NULL;
      return (ERRDDE_POST);
      }
    }
  else
    {
    if (PostMessage(hWndDDEs[channel],WM_DDE_REQUEST,hChildInfo,MAKELONG(CF_TEXT,aRequestItem)))
      return (0);
    else
      {
      GlobalDeleteAtom(aRequestItem);
      aRequestItem = NULL;
      return (ERRDDE_POST);
      }
    }
}

int ExecuteDDE(channel, buf)
  int  channel;
  char *buf;
{
  LPSTR lpStr;

  nErrorCode = 0;

  if (channel < 0 || channel >= MAX_CHANNELS || hWndDDEs[channel] == 0)
    {
    nErrorCode = ERRDDE_NOCHANNEL;
    return (0);
    }

  if (hExecuteCmd = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD)(strlen(buf) + 1)))
    {
    lpStr = (LPSTR)GlobalLock(hExecuteCmd);

    lstrcpy(lpStr, (LPSTR)buf);

    GlobalUnlock(hExecuteCmd);

    if (channel != 2)
      {
      if (PostMessage(hWndDDEs[channel],WM_DDE_EXECUTE,hWnd,MAKELONG(0,hExecuteCmd)))
	return (0);
      else
	return (ERRDDE_POST);
      }
    else
      {
      if (PostMessage(hWndDDEs[channel],WM_DDE_EXECUTE,hChildInfo,MAKELONG(0,hExecuteCmd)))
	return (0);
      else
	return (ERRDDE_POST);
      }
    }
  else
    return (ERRINT_MEMORY);
}

int AdviseDDE(channel, item, var)
  int  channel;
  char *item;
  int  var;
{
  LPDDEDATA lpData;
  ATOM	    aItem;
  register  i;


  nErrorCode = 0;

  if (channel < 0 || channel >= MAX_CHANNELS || hWndDDEs[channel] == 0)
    {
    nErrorCode = ERRDDE_NOCHANNEL;
    return (0);
    }

  aItem = GlobalAddAtom(item);

  if (aAdviseItems[var-1])
    {
    nErrorCode = ERRDDE_VARBUSY;
    return (0);
    }

  if (hAdviseMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 4L))
    {
    lpData = (LPDDEDATA)GlobalLock(hAdviseMem);
    lpData->cfFormat = CF_TEXT;

    GlobalUnlock(hAdviseMem);

    if (PostMessage(hWndDDEs[channel],WM_DDE_ADVISE,hWnd,MAKELONG(hAdviseMem,aItem)))
      {
      aAdviseItems[var-1] = aItem;
      return (0);
      }
    else
      return (ERRDDE_POST);
    }
  else
    return (ERRINT_MEMORY);
}

int UnadviseDDE(channel, item)
  int  channel;
  char *item;
{
  ATOM	    aItem;
  register  i;


  nErrorCode = 0;

  if (channel < 0 || channel >= MAX_CHANNELS || hWndDDEs[channel] == 0)
    {
    nErrorCode = ERRDDE_NOCHANNEL;
    return (0);
    }

  if ((aItem = GlobalFindAtom(item)) == NULL)
    return (ERRDDE_NOTADVISED);

  for (i = 0; i < 26; i++)
    if (aAdviseItems[i] == aItem)
      break;

  if (aAdviseItems[i] != aItem || i >= 26)
    return (ERRDDE_NOTADVISED);

  aUnadviseItem = aItem;

  if (PostMessage(hWndDDEs[channel], WM_DDE_UNADVISE, hWnd, MAKELONG(0,aItem)))
    return (0);
  else
    return (ERRDDE_POST);
}

int StatusDDE(channel)
  int channel;
{
  if (channel < 0 || channel >= MAX_CHANNELS || hWndDDEs[channel] == 0)
    return (0);
  else
    return (1);
}

void ScriptEndDDE()
{
  register i;

  nRequestVar = -1;

  for (i = 1; i < MAX_CHANNELS; i++)
    TerminateDDE(i);

  nErrorCode = 0;
}


BOOL SendDDEData(hWndDDE, wAtom, buf, bRequest)
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

int TerminateDDE(channel)
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
      {
      ResetSystemStatus(SS_DDE);
      if (!IfSystemStatus(SS_SCRIPT))
	PurgeVariableLevel(LEVEL_GLOBAL);
      IconTextChanged(NULL);
      }

    return (0);
    }

  nErrorCode = ERRDDE_NOCHANNEL;
  return (0);
}

static int near ExecuteCommands(cmd)
  char *cmd;
{
  char command[16], parameter[64];


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
      case 'B':
        if (bConnected)
          CmdConnect();
        break;

      case 'C':
	switch (command[1])
	  {
          case 'A':
	    if (IfSystemStatus(SS_SCRIPT))
	      InterpreterStop();
	    break;

          case 'L':
	    ProgramAbort();
	    break;
	  }
	break;

      case 'D':
        if (command[1] == 'I')
	  {
	  switch (command[2])
	    {
            case 'A':
	      if (!bConnected)
		{
		DataOpen(parameter);
		CmdConnect();
		}
	      break;
	    }
	  }
	break;

      case 'E':
	if ((wSystem & SS_SCRIPT) == 0)
	  ExecuteScript(parameter);
	break;

      case 'G':
        CommPortOpen();
        break;

      case 'K':
	KermitCommand(parameter, KRMSRC_DDE);
	break;

      case 'L':
	DataOpen(parameter);
	break;

      case 'N':
        if (command[1] == 'E' && command[2] == 'W' && command[3] == NULL)
	  DataNew();
	break;

      case 'S':
        if (command[1] == 'A' && command[2] == 'V' && command[3] == 'E')
	  {
	  switch (command[4])
	    {
	    case NULL:
	      DataSave();
	      break;

            case 'A':
	      DataSaveAs(parameter);
	      break;
	    }
	  }
	break;
      }
    }

  return (0);
}

static void near GlobalReAddAtom(atom)
  ATOM atom;
{
  char buf[64];

  GlobalGetAtomName(atom, buf, sizeof(buf));
  GlobalAddAtom(buf);
}
