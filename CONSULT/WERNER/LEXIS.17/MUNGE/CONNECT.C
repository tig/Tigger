/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Connect to the Lexis/Nexis system (CONNECT.C)                    */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This module is responsible for connecting to the Lexis/Nexis     */
/*           system.  Due to many options available to the user, this code    */
/*           is alittle on the complex side.  It must try up to three         */
/*           different networks and give the user manual if specified in the  */
/*           setup file.                                                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <direct.h>
#include <memory.h>
#include <string.h>
#include "lexis.h"
#include "connect.h"
#include "comm.h"
#include "data.h"
#include "session.h"
#include "tokens.h"
#include "dialogs.h"


#define CONNECT_INIT        0
#define CONNECT_DIAL        1
#define CONNECT_MEAD        2

#define DEFAULT_TIMEOUT     30000L

#define OpCode(x) (*(int far *)(&lpTokens[x]))

  FARPROC lpConnectDlg;
  FARPROC lpDisconnectDlg;

  static HANDLE hTokens = NULL;
  static LPSTR  lpTokens;
  static WORD   iTokens;
  static long   lTimeout;
  static long   lSleepUntil;
  static int    nState;

  static char achStringArg[256];
  static int  cStringArg;
  static int  iStringArg;
  static int  nIntegerArg;

  static BOOL near ConnectNext(HWND);

  static int  near Interpret(HWND);
  static int  near GetToken(int);
  static int  near GetStringArg();
  static int  near GetIntegerArg();
  static int  near GotoLabel();


  BOOL DlgConnect(HWND, unsigned, WORD, LONG);
  BOOL DlgDisconnect(HWND, unsigned, WORD, LONG);


void ConnectStart()
  {
  chdir(szWorkCWD);

  lpConnectDlg = MakeProcInstance((FARPROC)DlgConnect, hWndInst);
  hProcessDlg =  CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_CONNECT),
                              hWnd, lpConnectDlg);
  }

void ConnectEnd()
  {
  if (hProcessDlg == 0)
    return;

  DestroyWindow(hProcessDlg);
  hProcessDlg = 0;
  FreeProcInstance(lpConnectDlg);

  if (!fSessOnline)
    MessageDisplay(IDS_MSG_CALLFAILED, MB_ICONHAND);

  nWorkCDLostCount = 0;
  }

void DisconnectStart()
  {
  if (hProcessDlg || fDataHardwire)
    return;

  lpConnectDlg = MakeProcInstance((FARPROC)DlgDisconnect, hWndInst);
  hProcessDlg =  CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_DISCONNECT),
                              hWnd, lpConnectDlg);
  }

void DisconnectEnd()
  {
  if (hProcessDlg == 0)
    return;

  DestroyWindow(hProcessDlg);
  hProcessDlg = 0;
  FreeProcInstance(lpConnectDlg);
  }

static BOOL near ConnectNext(HWND hDlg)
  {
  register i;


  iTokens  = 0;
  lTimeout = GetCurrentTime() + DEFAULT_TIMEOUT;

  if (fDataHardwire || wDataDial == DATA_MANUAL)
    return (FALSE);

  while (1)
    {
    nWorkNetwork++;
    if (nWorkNetwork == 0)
      i = ID_CONNECT_PRI;
    else if (nWorkNetwork == 1)
      i = ID_CONNECT_1ST;
    else if (nWorkNetwork == 2)
      i = ID_CONNECT_2ND;
    else
      return (FALSE);

    if (szDataNumber[nWorkNetwork][0])
      break;
    }
  CheckRadioButton(hDlg, ID_CONNECT_PRI, ID_CONNECT_2ND, i);

  sCommSpeed = wDataSpeed[nWorkNetwork];
  CommUpdate();

  nState = CONNECT_INIT;
  CheckDlgButton(hDlg, ID_CONNECT_INIT, TRUE);
  CheckDlgButton(hDlg, ID_CONNECT_DIAL, FALSE);

  if (!fSessOnline)
    {
    if (CmdSignon())
      {
      MessageDisplay(IDS_MSG_BADCOMM, MB_ICONHAND);
      return (FALSE);
      }
    }

  return (TRUE);
  }


BOOL DlgConnect(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static BOOL fAbort;
  static BOOL fFirstLight;
  static long lChangeLight;
  static BOOL fEscape;


  switch (message)
    {
    case WM_INITDIALOG:
      {
      char buf[512];
      register i;

      /*
      ** Reset abort flag.
      */
      fAbort = FALSE;

      if (fWorkHyperRomMode)
        InfoMessage(IDS_INFO_SIGNONP, NULL);
      else
        InfoMessage(IDS_INFO_SIGNON, NULL);

      if (fDataHardwire || wDataDial == DATA_MANUAL)
	{
	DataQueryProfileKeyword(IDS_DATA_NETWORK, szDataNetwork[0], buf, sizeof(buf));
        for (i = 0; buf[i]; i++)
          {
          if (buf[i] == ',')
            {
            buf[i] = NULL;
            break;
            }
          }

	if (DataLoadProfileScript(buf, &hTokens))
	  {
	  ConnectEnd();
	  SessTellConnection(FALSE);
	  break;
	  }
	lpTokens = GlobalLock(hTokens);
	iTokens  = 0;

	lTimeout = GetCurrentTime() + DEFAULT_TIMEOUT;

	nState = CONNECT_DIAL;
	CheckDlgButton(hDlg, ID_CONNECT_INIT, FALSE);
	CheckDlgButton(hDlg, ID_CONNECT_DIAL, TRUE);
	}
      else
	{
	DataQueryProfileKeyword(IDS_DATA_MODEMS, szDataModem, buf, sizeof(buf));
	for (i = 0; buf[i]; i++)
	  {
	  if (buf[i] == ',')
	    {
            buf[i] = '\0';
	    break;
	    }
	  }

	if (DataLoadProfileScript(buf, &hTokens))
	  {
	  hTokens = NULL;
	  ConnectEnd();
	  SessTellConnection(FALSE);
	  break;
	  }
	lpTokens = GlobalLock(hTokens);

	nWorkNetwork = -1;
	if (!ConnectNext(hDlg))
	  {
	  ConnectEnd();
	  SessTellConnection(FALSE);
	  break;
	  }
	}

      fFirstLight  = TRUE;
      lChangeLight = GetCurrentTime() + 1500L;
      CheckDlgButton(hDlg, ID_CONNECT_LIGHT1, TRUE);
      }
      break;

    case WM_DESTROY:
      if (hTokens)
        {
        GlobalUnlock(hTokens);
        GlobalFree(hTokens);
        }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	case ID_CANCEL:
          fAbort = TRUE;
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_TIMER:
      {
      register i;

      if (fAbort)
        {
        ConnectEnd();
        SessTellConnection(FALSE);
        break;
        }

      if (GetCurrentTime() > lTimeout)
        {
        SessTellConnection(FALSE);
        if (!ConnectNext(hDlg))
          {
          ConnectEnd();
          SessTellConnection(FALSE);
          }
        break;
        }

      if (GetCurrentTime() > lChangeLight)
        {
        fFirstLight = !fFirstLight;
        CheckDlgButton(hDlg, ID_CONNECT_LIGHT1, fFirstLight);
        CheckDlgButton(hDlg, ID_CONNECT_LIGHT2, !fFirstLight);
        lChangeLight = GetCurrentTime() + 1500L;
        }

      if (nState != CONNECT_MEAD)
        {
        i = Interpret(hDlg);

        if (i == -1)
          break;
	else if (i == 10)
	  {
	  MessageBox(hDlg, "Check your Modem", NULL, MB_OK);
	  ConnectEnd();
	  SessTellConnection(FALSE);
	  break;
	  }
        else if (i)
          {
          if (!ConnectNext(hDlg))
            {
            ConnectEnd();
            SessTellConnection(FALSE);
            }
          break;
          }
        }
      else
        {
        char buf[1];

        buf[0] = '\0';
        while (CommReadData(buf, 1))
          {
          if (CommQueryCarrierDetect() == 0)
            {
            if (!ConnectNext(hDlg))
              {
              ConnectEnd();
              SessTellConnection(FALSE);
	      break;
              }
            break;
            }

          if (buf[0] == ASC_ETX)
            {
            ConnectEnd();
            SessTellConnection(TRUE);
            break;
            }

          if (buf[0] == ASC_ESC)
            fEscape = TRUE;
          else
            {
            if (fEscape)
              {
              if (buf[0] == 'D')
                {
                if (!ConnectNext(hDlg))
                  {
                  ConnectEnd();
                  SessTellConnection(FALSE);
                  }
                break;
                }
              fEscape = FALSE;
              }
            }

          buf[0] = '\0';
          }

        return (TRUE);
        }

      switch (nState)
        {
        case CONNECT_INIT:
          {
          char buf[80];
          register i;

          GlobalUnlock(hTokens);
          GlobalFree(hTokens);

          DataQueryProfileKeyword(IDS_DATA_NETWORK, szDataNetwork[nWorkNetwork], buf, sizeof(buf));
          for (i = 0; buf[i]; i++)
            {
            if (buf[i] == ',')
              {
              buf[i] = NULL;
              break;
              }
            }

          if (stricmp(buf, "Manual") == 0)
            {
            ConnectEnd();
            fSessProcess  = FALSE;
            wSessKeyboard = KEYBD_DIRECT;
            if (fWorkHyperRomMode)
              InfoMessage(IDS_INFO_MANUALP, NULL);
            else
              InfoMessage(IDS_INFO_MANUAL, NULL);
            break;
            }
          else if (DataLoadProfileScript(buf, &hTokens))
            {
            if (!ConnectNext(hDlg))
              {
              ConnectEnd();
              SessTellConnection(FALSE);
              }
            break;
            }
          lpTokens = GlobalLock(hTokens);
          iTokens  = 0;

          lTimeout = GetCurrentTime() + DEFAULT_TIMEOUT;

          nState = CONNECT_DIAL;
          CheckDlgButton(hDlg, ID_CONNECT_INIT, FALSE);
          CheckDlgButton(hDlg, ID_CONNECT_DIAL, TRUE);
          }
          break;

	case CONNECT_DIAL:
          if (fWorkHyperRomMode)
	    InfoMessage(IDS_INFO_SIGNONP, NULL);
	  else
	    InfoMessage(IDS_INFO_SIGNON, NULL);
          GlobalUnlock(hTokens);
          GlobalFree(hTokens);
          hTokens = NULL;
          fEscape = FALSE;
          nState = CONNECT_MEAD;
          break;
        }
      }
      break;

    default:
      return (FALSE);
    }

  return (TRUE);
}


BOOL DlgDisconnect(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static BOOL fAbort;
  static BOOL fFirstLight;
  static long lChangeLight;
  static BOOL fEscape;


  switch (message)
    {
    case WM_INITDIALOG:
      {
      char buf[512], *ptr;
      register i;

      /*
      ** Reset abort flag.
      */
      fAbort = FALSE;

      DataQueryProfileKeyword(IDS_DATA_MODEMS, szDataModem, buf, sizeof(buf));

      for (i = 0; buf[i]; i++)
        {
        if (buf[i] == ',')
          {
	  i++;
	  ptr = &buf[i];
          break;
          }
	}
      while (buf[i] != ',')
	i++;
      buf[i] = '\0';

      if (DataLoadProfileScript(ptr, &hTokens))
        {
        hTokens = NULL;
        DisconnectEnd();
        break;
        }
      lpTokens = GlobalLock(hTokens);

      iTokens  = 0;
      lTimeout = GetCurrentTime() + DEFAULT_TIMEOUT;

      fFirstLight  = TRUE;
      lChangeLight = GetCurrentTime() + 1500L;
      CheckDlgButton(hDlg, ID_CONNECT_LIGHT1, TRUE);
      SetFocus(GetDlgItem(hDlg, ID_CANCEL));
      return (FALSE);
      }
      break;

    case WM_DESTROY:
      if (hTokens)
        {
        GlobalUnlock(hTokens);
        GlobalFree(hTokens);
        }
      return (FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	case ID_CANCEL:
          fAbort = TRUE;
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_TIMER:
      {
      register i;

      if (fAbort)
        {
        DisconnectEnd();
        break;
        }

      if (GetCurrentTime() > lTimeout)
        {
        DisconnectEnd();
        break;
        }

      if (GetCurrentTime() > lChangeLight)
        {
        fFirstLight = !fFirstLight;
        CheckDlgButton(hDlg, ID_CONNECT_LIGHT1, fFirstLight);
        CheckDlgButton(hDlg, ID_CONNECT_LIGHT2, !fFirstLight);
        lChangeLight = GetCurrentTime() + 1500L;
	//SetFocus(GetDlgItem(hDlg, ID_CANCEL));
        }

      i = Interpret(hDlg);

      if (i == 10)
	{
	MessageBox(NULL, "Check your Modem", NULL, MB_OK);
	DisconnectEnd();
	}
      else if (i != -1)
        DisconnectEnd();
      }
      break;

    default:
      return (FALSE);
    }

  return (TRUE);
}


static int near Interpret(HWND hDlg)
  {
  static long lModemTimeout;
  static BOOL fMatchFlag;
  register i;


  if (iTokens == 0)
    {
    iTokens += 2;
    fMatchFlag	  = FALSE;
    lSleepUntil   = 0L;
    lModemTimeout = 0L;
    }

  if (lSleepUntil != 0L)
    {
    if (lSleepUntil > GetCurrentTime())
      return (-1);
    lSleepUntil = 0L;
    }

  if (lModemTimeout != 0L)
    {
    if (lModemTimeout <= GetCurrentTime())
      {
      lModemTimeout = 0L;
      return (10);
      }
    }

  if (fMatchFlag)
    {
    char buf[1];

    while (CommReadData(buf, 1))
      {
      if (toupper((buf[0] & 0x7F)) == toupper(achStringArg[iStringArg]))
        {
        iStringArg++;
        if (iStringArg >= cStringArg)
          {
          fMatchFlag = FALSE;
	  lModemTimeout = 0L;
          break;
          }
        }
      else
        iStringArg = 0;
      }

    return (-1);
    }

  switch (*(int far *)(&lpTokens[iTokens]))
    {
    case TKN_BREAK:
      iTokens += 2;
      if (i = GetIntegerArg())
        return (i);
      CmdBreak(nIntegerArg);
      break;

    case TKN_DTR:
      iTokens += 2;
      if (i = GetIntegerArg())
        return (i);
      CommUpdateDtr(nIntegerArg);
      break;

    case TKN_END:
      return (0);
      break;

    case TKN_GOTO:
      iTokens += 2;
      if (i = GetStringArg())
        return(i);
      if (i = GotoLabel())
        return(i);
      break;

    case TKN_IF:
      {
      int nInteger1;

      iTokens += 2;
      if (i = GetIntegerArg())
        return (i);
      if (i = GetToken(TKN_EQUAL))
        return (i);
      nInteger1 = nIntegerArg;
      if (i = GetIntegerArg())
        return (i);
      if (nInteger1 != nIntegerArg)
        break;
      if (i = GetToken(TKN_THEN))
        return (i);
      if (i = GetStringArg())
        return (i);
      if (i = GotoLabel())
        return(i);
      }
      break;

    case TKN_INPUT:
      iTokens += 2;
      if (i = GetStringArg())
        return (i);
      if (!GetToken(TKN_COMMA))
	{
	if (i = GetIntegerArg())
	  return (i);

	lModemTimeout = GetCurrentTime() + nIntegerArg;
	}
      fMatchFlag = TRUE;
      iStringArg = 0;
      break;

    case TKN_LABEL:
      iTokens += 2;
      while (lpTokens[iTokens++]);
      break;

    case TKN_PRINT:
      iTokens += 2;
      if (i = GetStringArg())
        return (i);
      if (OpCode(iTokens) == TKN_SEMI)
        iTokens += 2;
      else
        achStringArg[cStringArg++] = '\r';
      CommWriteData(achStringArg, cStringArg);
      break;

    case TKN_SLEEP:
      iTokens += 2;
      if (i = GetIntegerArg())
        return (i);
      lSleepUntil = GetCurrentTime() + (long)(unsigned)nIntegerArg;
      break;

    case TKN_TIMEOUT:
      iTokens += 2;
      if (i = GetIntegerArg())
        return (i);
      lTimeout = GetCurrentTime() + (long)nIntegerArg * 1000L;
      break;
    }

  while (OpCode(iTokens) != TKN_EOL)
    {
    switch (OpCode(iTokens))
      {
      case TKN_IDENT:
      case TKN_LABEL:
      case TKN_STRCON:
        iTokens += 2;
        while (lpTokens[iTokens++]);
        break;

      default:
        iTokens += 2;
      }
    }
  iTokens += 4;

  return (-1);
  }


static int near GetToken(int nToken)
  {
  if (OpCode(iTokens) == nToken)
    {
    iTokens += 2;
    return (0);
    }
  else
    return (1);
  }

static int near GetStringArg()
  {
  int  i;


  cStringArg = 0;

  while (1)
    {
    switch (OpCode(iTokens))
      {
      case TKN_IDENT:
      case TKN_STRCON:
        iTokens += 2;
        while (lpTokens[iTokens])
          achStringArg[cStringArg++] = lpTokens[iTokens++];
        iTokens++;
        break;

      case TKN_PLUS:
        iTokens += 2;
        break;

      case TKN_DIAL:
        iTokens += 2;

        achStringArg[cStringArg++] = 'D';
        if (wDataDial == DATA_TONE)
          achStringArg[cStringArg++] = 'T';
        else
          achStringArg[cStringArg++] = 'P';
        break;

      case TKN_PREFIX:
        iTokens += 2;

        for (i = 0; szDataPrefix[nWorkNetwork][i]; i++)
          achStringArg[cStringArg++] = szDataPrefix[nWorkNetwork][i];
        break;

      case TKN_NUMBER:
        iTokens += 2;

        for (i = 0; szDataNumber[nWorkNetwork][i]; i++)
          achStringArg[cStringArg++] = szDataNumber[nWorkNetwork][i];
        break;

      case TKN_NODE:
        iTokens += 2;

        for (i = 0; szDataNode[nWorkNetwork][i]; i++)
          achStringArg[cStringArg++] = szDataNode[nWorkNetwork][i];
        break;

      default:
        return (0);
        break;
      }
    }
  }

static int near GetIntegerArg()
  {
  if (OpCode(iTokens) == TKN_NUMCON)
    {
    iTokens += 2;
    nIntegerArg = OpCode(iTokens);
    iTokens += 2;

    return (0);
    }
  else if (OpCode(iTokens) == TKN_SPEED)
    {
    iTokens += 2;
    nIntegerArg = (int)wDataSpeed[nWorkNetwork];

    return (0);
    }

  return (1);
  }

static int near GotoLabel()
  {
  register i;


  i = 0;
  while (1)
    {
    if (OpCode(i+2) == TKN_END)
      return (1);
    else if (OpCode(i+2) == TKN_LABEL)
      {
      register j;

      for (j = 0; j < cStringArg; j++)
        {
        if (lpTokens[i+4+j] != toupper(achStringArg[j]))
          break;
        }
      if (j >= cStringArg)
        {
        iTokens = i;
        return (0);
        }
      }

    i += OpCode(i);
    }
  }
