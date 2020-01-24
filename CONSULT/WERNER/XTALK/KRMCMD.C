/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Command Processor (KRMCMD.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module processes the Kermit Commands.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <time.h>
#include <dos.h>	//dca00046 JDB
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "kermit.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "xfer.h"
#include "dialogs.h"
#include "global.h"
#include "variable.h"

  int  nKermitMode;
  int  nKermitStatus;

extern void AbortServer();

static int  near CommandBye();
static int  near CommandCancel(PSTR);
static int  near CommandDelete(PSTR);
static int  near CommandEcho(PSTR);
static int  near CommandFinish();
static int  near CommandGet(PSTR);
static int  near CommandHangup();
static int  near CommandHelp(PSTR);
static int  near CommandKermit(PSTR);
static int  near CommandReceive(PSTR);
static int  near CommandRemoteCopy(PSTR);
static int  near CommandRemoteCWD(PSTR);
static int  near CommandRemoteDelete(PSTR);
static int  near CommandRemoteDir(PSTR);
static int  near CommandRemoteHelp(PSTR);
static int  near CommandRemoteHost(PSTR);
static int  near CommandRemoteKermit(PSTR);
static int  near CommandRemoteLogin(PSTR);
static int  near CommandRemoteRename(PSTR);
static int  near CommandRemoteSend(PSTR);
static int  near CommandRemoteSet(PSTR);
static int  near CommandRemoteShow(PSTR);
static int  near CommandRemoteSpace(PSTR);
static int  near CommandRemoteType(PSTR);
static int  near CommandRemoteWho();
static int  near CommandSend(PSTR);
static int  near CommandServer();
static int  near CommandSet(PSTR);
static int  near CommandShow();
static int  near CommandStats();

static void near Show2Char(int, int, int);
static void near Show2Int(int, int, int);
static void near StartIfNull();
static int  near LookupKeyword(PSTR);
static PSTR near GetStringToken(PSTR, PSTR);
static PSTR near GetString(PSTR, PSTR);
static PSTR near GetNumeric(PSTR, WORD *);
static PSTR near GetStringArgument(PSTR, PSTR);
static int  near GetChar(PSTR);
static int  near GetInt(PSTR);


int KermitCommand(pCmd, nSource)
  PSTR pCmd;
  int  nSource;
{
  char sToken[132];
  register i;


  pCmd = GetStringToken(pCmd, sToken);
  switch (LookupKeyword(sToken))
    {
    case TKN_BYE:
      if (nKermitMode == KM_NULL)
        nKermitStatus = ERRKRM_NOKCP;
      else if (nKermitMode == KM_IDLE)
	nKermitStatus = CommandBye();
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_CANCEL:
      nKermitStatus = CommandCancel(pCmd);
      break;

    case TKN_COMMENT:
      nKermitStatus = 0;
      break;

    case TKN_CONNECT:
      if (!bConnected)
        nKermitStatus = KermitConnect();
      else
	nKermitStatus = ERRKRM_CONNECT;

      StartIfNull();
      break;

    case TKN_ECHO:
      nKermitStatus = CommandEcho(pCmd);
      break;

    case TKN_EXIT:
      if (nKermitMode == KM_IDLE)
	{
	TermKermit();
	nKermitStatus = 0;
	}
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_FINISH:
      if (nKermitMode == KM_IDLE)
	nKermitStatus = CommandFinish();
      else if (nKermitMode == KM_NULL)
        nKermitStatus = ERRKRM_NOKCP;
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_GET:
      StartIfNull();

      if (nKermitMode == KM_IDLE)
	nKermitStatus = CommandGet(pCmd);
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_HANGUP:
      if (nSource == KRMSRC_USER)
	{
	nKermitStatus = ERRKRM_CANTBEUSER;
	}
      else if (nKermitMode == KM_BUSY)
	{
	nKermitStatus = ERRKRM_BUSY;
	}
      else
	{
	if (nKermitMode == KM_IDLE)
	  TermKermit();

	if (bConnected)
	  KermitHangup();

	nKermitStatus = 0;
	}
      break;

    case TKN_HELP:
      nKermitStatus = CommandHelp(pCmd);
      break;

    case TKN_RECEIVE:
      StartIfNull();

      if (nKermitMode == KM_IDLE)
	nKermitStatus = CommandReceive(pCmd);
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_REMOTE:
      StartIfNull();

      if (nKermitMode == KM_IDLE)
	{
	pCmd = GetStringToken(pCmd, sToken);
	switch (LookupKeyword(sToken))
	  {
	  case TKN_COPY:
	    nKermitStatus = CommandRemoteCopy(pCmd);
	    break;

	  case TKN_CWD:
	    nKermitStatus = CommandRemoteCWD(pCmd);
	    break;

	  case TKN_DELETE:
	    nKermitStatus = CommandRemoteDelete(pCmd);
	    break;

	  case TKN_DIRECTORY:
	    nKermitStatus = CommandRemoteDir(pCmd);
	    break;

	  case TKN_HELP:
	    nKermitStatus = CommandRemoteHelp(pCmd);
	    break;

	  case TKN_HOST:
	    nKermitStatus = CommandRemoteHost(pCmd);
	    break;

	  case TKN_KERMIT:
	    nKermitStatus = CommandRemoteKermit(pCmd);
	    break;

	  case TKN_LOGIN:
	    nKermitStatus = CommandRemoteLogin(pCmd);
	    break;

	  case TKN_RENAME:
	    nKermitStatus = CommandRemoteRename(pCmd);
	    break;

	  case TKN_SEND:
	    nKermitStatus = CommandRemoteSend(pCmd);
	    break;

	  case TKN_SET:
	    nKermitStatus = CommandRemoteSet(pCmd);
	    break;

	  case TKN_SHOW:
	    nKermitStatus = CommandRemoteShow(pCmd);
	    break;

	  case TKN_DISK:
	  case TKN_SPACE:
	    nKermitStatus = CommandRemoteSpace(pCmd);
	    break;

	  case TKN_TYPE:
	    nKermitStatus = CommandRemoteType(pCmd);
	    break;

	  case TKN_WHO:
	    nKermitStatus = CommandRemoteWho();
	    break;

	  default:
	    nKermitStatus = ERRKRM_PARSE;
	  }
	}
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_SEND:
      StartIfNull();

      if (nKermitMode == KM_IDLE)
	nKermitStatus = CommandSend(pCmd);
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_SERVER:
      StartIfNull();

      if (nKermitMode == KM_IDLE)
	{
	nKermitMode = KM_SERVER;
	nKermitStatus = NULL;
	}
      else
	nKermitStatus = ERRKRM_BUSY;
      break;

    case TKN_SET:
      nKermitStatus = CommandSet(pCmd);
      break;

    case TKN_SHOW:
      if (nSource == KRMSRC_USER)
	nKermitStatus = CommandShow();
      else
	nKermitStatus = ERRKRM_MUSTBEUSER;
      break;

    case TKN_STATISTICS:
      nKermitStatus = CommandStats();
      break;

    default:
      nKermitStatus = ERRKRM_PARSE;
    }

  return (nKermitStatus);
}


static int near CommandBye()
{
  filexfer.kermit.fAbortKermit = TRUE;

  if (KermitRemoteMessage("L") == 0)
    return (0);
  else
    return (ERRKRM_INTERNAL);
}


static int near CommandCancel(pStr)
  PSTR pStr;
{
  char sToken[132];


  GetStringToken(pStr, sToken);
  switch (LookupKeyword(sToken))
    {
    case TKN_BATCH:
      if (filexfer.kermit.cMode == WAIT_FILE_DATA || filexfer.kermit.cMode == SEND_FILE_DATA)
	{
	filexfer.kermit.bAbortBatch = TRUE;
	return (0);
	}

      return (ERRKRM_NOTXFER);
      break;

    case TKN_FILE:
      if (filexfer.kermit.cMode == WAIT_FILE_DATA || filexfer.kermit.cMode == SEND_FILE_DATA)
	{
	filexfer.kermit.bAbortFile = TRUE;
	return (0);
	}

      return (ERRKRM_NOTXFER);
      break;

    case TKN_SERVER:
      AbortServer();
      break;
    }

  return (ERRKRM_PARSE);
}


static int near CommandEcho(pCmd)
  PSTR pCmd;
{
  if (*pCmd != NULL)
    LineToTerminal(pCmd);

  return (0);
}


static int near CommandFinish()
{
  filexfer.kermit.fAbortKermit = TRUE;

  if (KermitRemoteMessage("F") == 0)
    return (0);
  else
    return (ERRKRM_INTERNAL);
}


static int near CommandGet(pStr)
  PSTR pStr;
{
  char sRemoteFile[128];
  char sLocalFile[128];

  GetString(GetString(pStr, sRemoteFile), sLocalFile);

  if (sRemoteFile[0] == NULL)
    return (ERRKRM_PARSE);

  return (KermitGetFile(sRemoteFile, sLocalFile));
}


static int near CommandHelp(pCmd)
  PSTR pCmd;
{
  return (0);
}

static int near CommandReceive(pStr)
  PSTR pStr;
{
  char sLocalFile[128];

  GetString(pStr, sLocalFile);
  return (KermitReceive(sLocalFile));
}

static int near CommandSend(pStr)
  PSTR pStr;
{
  char sLocalFile[128];
  char sRemoteFile[128];


  GetString(GetString(pStr, sLocalFile), sRemoteFile);

  if (sLocalFile[0] == NULL)
    return (ERRKRM_PARSE);

  return (KermitSendFile(sLocalFile, sRemoteFile));
}


static int near CommandRemoteCopy(pStr)
  PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GK");
  pStr = GetStringArgument(sMessage, pStr);
  if (*pStr == NULL)
    return (ERRKRM_PARSE);
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteCWD(pStr)
  PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GC");
  GetStringArgument(sMessage, GetStringArgument(sMessage, pStr));

  return (KermitMessage(sMessage));
}

static int near CommandRemoteDelete(pStr)
  PSTR pStr;
{
  char sMessage[128];


  if (*pStr == NULL)
    return (ERRKRM_PARSE);

  strcpy(sMessage, "GE");
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteDir(pStr)
 PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GD");
  GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteHelp(pStr)
 PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GH");
  GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteHost(pStr)
 PSTR pStr;
{
  char sMessage[128];

  if (strlen(pStr) > 80)
    *(pStr+80) = NULL;

  sMessage[0] = 'C';
  strcpy(&sMessage[1], pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteKermit(pStr)
 PSTR pStr;
{
  char sMessage[128];

  if (strlen(pStr) > 80)
    *(pStr+80) = NULL;

  sMessage[0] = 'K';
  strcpy(&sMessage[1], pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteLogin(pStr)
  PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GI");
  pStr = GetStringArgument(sMessage, pStr);
  pStr = GetStringArgument(sMessage, pStr);
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteRename(pStr)
  PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GR");
  pStr = GetStringArgument(sMessage, pStr);
  if (*pStr == NULL)
    return (ERRKRM_PARSE);
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteSend(pStr)
 PSTR pStr;
{
  char sMessage[128];

  if (strlen(pStr) > 80)
    *(pStr+80) = NULL;

  sMessage[0] = 'G';
  sMessage[1] = 'M';
  sMessage[2] = (char)(strlen(pStr) + 32);
  strcpy(&sMessage[3], pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteSet(pStr)
  PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "V!S");
  pStr = GetStringArgument(sMessage, pStr);
  if (*pStr == NULL)
    return (ERRKRM_PARSE);
  pStr = GetStringArgument(sMessage, pStr);
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteShow(pStr)
  PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "V!Q");
  pStr = GetStringArgument(sMessage, pStr);
  pStr = GetStringArgument(sMessage, pStr);
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteSpace(pStr)
 PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GU");
  GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteType(pStr)
  PSTR pStr;
{
  char sMessage[128];


  if (*pStr == NULL)
    return (ERRKRM_PARSE);

  strcpy(sMessage, "GT");
  pStr = GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}

static int near CommandRemoteWho(pStr)
 PSTR pStr;
{
  char sMessage[128];

  strcpy(sMessage, "GW");
  GetStringArgument(sMessage, pStr);

  return (KermitMessage(sMessage));
}


static int near CommandSet(pStr)
  PSTR pStr;
{
  DCB  sDCB;
  char sToken[132];
  int  i;


  pStr = GetStringToken(pStr, sToken);
  switch (LookupKeyword(sToken))
    {
    case TKN_BELL:
      GetStringToken(pStr, sToken);
      switch (LookupKeyword(sToken))
	{
	case TKN_ON:
	  filexfer.kermit.fBellOn = TRUE;
	  break;

	case TKN_OFF:
	  filexfer.kermit.fBellOn = FALSE;
	  break;

	default:
	  return (ERRKRM_PARSE);
	}

      return (0);
      break;

    case TKN_BLOCKCHECK:
      i = GetInt(pStr);
      if (i < 1|| i > 3)
      return(ERRKRM_BLOCKCHECK);

      filexfer.kermit.cBlockCheck = (char)i;
      return(0);
      break;

    case TKN_DELAY:
      i = GetInt(pStr);
      if (i < 0|| i > 60)
      return(ERRKRM_DELAY);

      filexfer.kermit.nDelay = i;
      return(0);
      break;

    case TKN_EOF:
      GetStringToken(pStr, sToken);
      if ((i = LookupKeyword(sToken)) == TKN_CTRLZ)
	filexfer.kermit.fEofCtrlZ = TRUE;
      else if (i == TKN_NOCTRLZ)
	filexfer.kermit.fEofCtrlZ = FALSE;
      else
	return (ERRKRM_PARSE);

      return (0);
      break;

    case TKN_FILE:
      pStr = GetStringToken(pStr, sToken);
      switch (LookupKeyword(sToken))
	{
	case TKN_NAMES:
	  GetStringToken(pStr, sToken);
	  switch (LookupKeyword(sToken))
	    {
	    case TKN_CONVERTED:
	      filexfer.kermit.fFileNamesConverted = TRUE;
	      break;

	    case TKN_LITERAL:
	      filexfer.kermit.fFileNamesConverted = FALSE;
	      break;

	    default:
	      return (ERRKRM_PARSE);
	    }
	  break;

	case TKN_SUPERSEDE:
	  GetStringToken(pStr, sToken);
	  switch (LookupKeyword(sToken))
	    {
	    case TKN_ON:
	      filexfer.kermit.fFileSupersede = TRUE;
	      break;

	    case TKN_OFF:
	      filexfer.kermit.fFileSupersede = FALSE;
	      break;

	    default:
	      return (ERRKRM_PARSE);
	    }
	  break;

	case TKN_WARNING:
	  GetStringToken(pStr, sToken);
	  switch (LookupKeyword(sToken))
	    {
	    case TKN_ON:
	      filexfer.kermit.fFileWarning = TRUE;
	      break;

	    case TKN_OFF:
	      filexfer.kermit.fFileWarning = FALSE;
	      break;

	    default:
	      return (ERRKRM_PARSE);
	    }
	  break;

	default:
	  return (ERRKRM_PARSE);
	}

      return (0);
      break;

    case TKN_INCOMPLETE:
      pStr = GetStringToken(pStr, sToken);
      switch (LookupKeyword(sToken))
	{
	case TKN_KEEP:
	  filexfer.kermit.fFileKeep = TRUE;
	  break;

	case TKN_DISCARD:
	  filexfer.kermit.fFileKeep = FALSE;
	  break;

	default:
	  return (ERRKRM_PARSE);
	}
      return (0);
      break;

    case TKN_PARITY:
      GetCommState(nCommID, (DCB FAR *)&sDCB);

      GetStringToken(pStr, sToken);
      switch (LookupKeyword(sToken))
	{
	case TKN_EVEN:
	  sDCB.ByteSize = 7;
	  sDCB.Parity = EVENPARITY;
	  break;

	case TKN_NONE:
	  sDCB.ByteSize = 8;
	  sDCB.Parity = NOPARITY;
	  break;

	case TKN_ODD:
	  sDCB.ByteSize = 7;
	  sDCB.Parity = ODDPARITY;
	  break;

	default:
	  return (ERRKRM_PARSE);
	  break;
	}

      SetCommState((DCB FAR *)&sDCB);
      return (0);
      break;

    case TKN_RECEIVE:
      pStr = GetStringToken(pStr, sToken);
      if (*pStr == NULL)
	return (ERRKRM_PARSE);

      switch (LookupKeyword(sToken))
	{
	case TKN_ENDOFPACKET:
	  filexfer.kermit.cRcvEOL = GetChar(pStr);
	  return(0);
	  break;

	case TKN_PACKETLENGTH:
	  i = GetInt(pStr);
	  if (i < 10 || i > KERMIT_MAXBUF - 64)
	    return(ERRKRM_PACKETLENGTH);

	  filexfer.kermit.nRcvPacketSize = i;
	  return(0);
	  break;

	case TKN_PADCHARACTER:
	  filexfer.kermit.cRcvPadChar = GetChar(pStr);
	  return(0);
	  break;

	case TKN_PADDING:
	  i = GetInt(pStr);
	  if (i < 0 || i > 32)
	    return(ERRKRM_PADDING);

	  filexfer.kermit.nRcvPadNum = i;
	  return(0);
	  break;

	case TKN_PAUSE:
	  i = GetInt(pStr);
	  if (i < 0 || i > 1000)
	    return(ERRKRM_PAUSE);

	  filexfer.kermit.nPause = i;
	  return(0);
	  break;

	case TKN_STARTOFPACKET:
	  filexfer.kermit.cRcvStartChar = GetChar(pStr);
	  return(0);
	  break;

	case TKN_TIMEOUT:
	  i = GetInt(pStr);
	  if (i < 0 || i > 600)
	    return(ERRKRM_TIMEOUT);

	  filexfer.kermit.nRcvTimeOut = i;
	  return(0);
	  break;
	}

    case TKN_RETRY:
      pStr = GetStringToken(pStr, sToken);
      switch (LookupKeyword(sToken))
	{
	case TKN_INITIAL:
	  i = GetInt(pStr);
	  if (i < 0 || i > 60)
	    return(ERRKRM_INITIAL);

	  filexfer.kermit.nInitialErrors = i;
	  break;

	case TKN_PACKETS:
	  i = GetInt(pStr);
	  if (i < 0 || i > 60)
	    return(ERRKRM_INITIAL);

	  filexfer.kermit.nPacketErrors = i;
	  break;

	default:
	  return (ERRKRM_PARSE);
	  break;
	}

      return(0);
      break;

    case TKN_SEND:
      pStr = GetStringToken(pStr, sToken);
      if (*pStr == NULL)
	return (ERRKRM_PARSE);

      switch (LookupKeyword(sToken))
	{
	case TKN_ENDOFPACKET:
	  filexfer.kermit.cSndEOL = GetChar(pStr);
	  return(0);
	  break;

	case TKN_PACKETLENGTH:
	  i = GetInt(pStr);
	  if (i < 10 || i > KERMIT_MAXBUF - 64)
	    return(ERRKRM_PACKETLENGTH);

	  filexfer.kermit.nSndPacketSize = i;
	  return(0);
	  break;

	case TKN_PADCHARACTER:
	  filexfer.kermit.cSndPadChar = GetChar(pStr);
	  return(0);
	  break;

	case TKN_PADDING:
	  i = GetInt(pStr);
	  if (i < 0 || i > 32)
	    return(ERRKRM_PADDING);

	  filexfer.kermit.nSndPadNum = i;
	  return(0);
	  break;

	case TKN_PAUSE:
	  i = GetInt(pStr);
	  if (i < 0 || i > 1000)
	    return(ERRKRM_PAUSE);

	  filexfer.kermit.nPause = i;
	  return(0);
	  break;

	case TKN_STARTOFPACKET:
	  filexfer.kermit.cSndStartChar = GetChar(pStr);
	  return(0);
	  break;

	case TKN_TIMEOUT:
	  i = GetInt(pStr);
	  if (i < 0 || i > 600)
	    return(ERRKRM_TIMEOUT);

	  filexfer.kermit.nSndTimeOut = i;
	  return(0);
	  break;
	}
      break;

    case TKN_TIMER:
      GetStringToken(pStr, sToken);
      switch (LookupKeyword(sToken))
	{
	case TKN_ON:
	  filexfer.kermit.fTimer = TRUE;
	  break;

	case TKN_OFF:
	  filexfer.kermit.fTimer = FALSE;
	  break;

	default:
	  return (ERRKRM_PARSE);
	}

      return (0);
      break;

    default:
      return (ERRKRM_PARSE);
    }

  return (ERRKRM_PARSE);
}


static int near CommandShow()
{
  char sBuf1[96], sBuf2[96], sStr1[8], sStr2[8];
  char sOn[3], sOff[4];
  int  nLen;


  LineToTerminal("");

  sOn[0] = 'O';
  sOn[1] = 'n';
  sOn[2] = NULL;

  sOff[0] = 'O';
  sOff[1] = 'f';
  sOff[2] = 'f';
  sOff[3] = NULL;


  LoadString(hWndInst, KRMSTR_SHOW01, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, (filexfer.kermit.fFileWarning) ? sOn : sOff);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW02, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, (filexfer.kermit.fFileKeep) ? sOff : sOn);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW03, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, (filexfer.kermit.fEofCtrlZ) ? sOn : sOff);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW04, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, (filexfer.kermit.fBellOn) ? sOn : sOff);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW06, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, (filexfer.kermit.fTimer) ? sOn : sOff);
  LineToTerminal(sBuf2);

  if (filexfer.kermit.cSndStartChar < ' ')
    {
    sStr1[0] = '^';
    sStr1[1] = filexfer.kermit.cSndStartChar + 64;
    sStr1[2] = NULL;
    }
  else
    {
    sStr1[0] = filexfer.kermit.cSndStartChar;
    sStr1[1] = NULL;
    }

  if (filexfer.kermit.cRcvStartChar < ' ')
    {
    sStr2[0] = '^';
    sStr2[1] = filexfer.kermit.cRcvStartChar + 64;
    sStr2[2] = NULL;
    }
  else
    {
    sStr2[0] = filexfer.kermit.cRcvStartChar;
    sStr2[1] = NULL;
    }

  LoadString(hWndInst, KRMSTR_SHOW09, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, sStr1, sStr2);
  LineToTerminal(sBuf2);

  if (filexfer.kermit.cSndEOL < ' ')
    {
    sStr1[0] = '^';
    sStr1[1] = filexfer.kermit.cSndEOL + 64;
    sStr1[2] = NULL;
    }
  else
    {
    sStr1[0] = filexfer.kermit.cSndEOL;
    sStr1[1] = NULL;
    }

  if (filexfer.kermit.cRcvEOL < ' ')
    {
    sStr2[0] = '^';
    sStr2[1] = filexfer.kermit.cRcvEOL + 64;
    sStr2[2] = NULL;
    }
  else
    {
    sStr2[0] = filexfer.kermit.cRcvEOL;
    sStr2[1] = NULL;
    }

  LoadString(hWndInst, KRMSTR_SHOW10, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, sStr1, sStr2);
  LineToTerminal(sBuf2);

  if (filexfer.kermit.cSndCtlq < ' ')
    {
    sStr1[0] = '^';
    sStr1[1] = filexfer.kermit.cSndCtlq + 64;
    sStr1[2] = NULL;
    }
  else
    {
    sStr1[0] = filexfer.kermit.cSndCtlq;
    sStr1[1] = NULL;
    }

  if (filexfer.kermit.cRcvCtlq < ' ')
    {
    sStr2[0] = '^';
    sStr2[1] = filexfer.kermit.cRcvCtlq + 64;
    sStr2[2] = NULL;
    }
  else
    {
    sStr2[0] = filexfer.kermit.cRcvCtlq;
    sStr2[1] = NULL;
    }

  LoadString(hWndInst, KRMSTR_SHOW11, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, sStr1, sStr2);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW12, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.nSndPacketSize, filexfer.kermit.nRcvPacketSize);
  LineToTerminal(sBuf2);

  if (filexfer.kermit.cSndPadChar < ' ')
    {
    sStr1[0] = '^';
    sStr1[1] = filexfer.kermit.cSndPadChar + 64;
    sStr1[2] = NULL;
    }
  else
    {
    sStr1[0] = filexfer.kermit.cSndPadChar;
    sStr1[1] = NULL;
    }

  if (filexfer.kermit.cRcvPadChar < ' ')
    {
    sStr2[0] = '^';
    sStr2[1] = filexfer.kermit.cRcvPadChar + 64;
    sStr2[2] = NULL;
    }
  else
    {
    sStr2[0] = filexfer.kermit.cRcvPadChar;
    sStr2[1] = NULL;
    }

  LoadString(hWndInst, KRMSTR_SHOW13, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, sStr1, sStr2);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW14, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.nSndPadNum, filexfer.kermit.nRcvPadNum);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW15, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.nSndTimeOut, filexfer.kermit.nRcvTimeOut);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW16, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.nPause);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW17, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.nInitialErrors, filexfer.kermit.nPacketErrors);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_SHOW18, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.cBlockCheck);
  LineToTerminal(sBuf2);

  return (0);
}


static int near CommandStats()
{
  char sBuf1[96], sBuf2[96];
  WORD wElapse, wBaud, wRate;


  LineToTerminal("");

  LoadString(hWndInst, KRMSTR_STAT01, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.lStatFileChars);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_STAT02, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.lStatCommIn);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_STAT03, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, filexfer.kermit.lStatCommOut);
  LineToTerminal(sBuf2);

  wElapse = (WORD)((filexfer.kermit.lStatStopTime - filexfer.kermit.lStatStartTime + 900L) / 1000L);
  if (wElapse)
    {
    wBaud = (WORD)((filexfer.kermit.lStatFileChars * 10L) / (long)wElapse);
    wRate = (wBaud * 100L + ComDCB.BaudRate / 2) / ComDCB.BaudRate;
    }
  else
    {
    wBaud = 0;
    wRate = 0;
    }

  LoadString(hWndInst, KRMSTR_STAT04, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, wElapse);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_STAT05, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, wBaud);
  LineToTerminal(sBuf2);

  LoadString(hWndInst, KRMSTR_STAT06, sBuf1, sizeof(sBuf1));
  sprintf(sBuf2, sBuf1, wRate);
  LineToTerminal(sBuf2);
}

static void near Show2Char(nFirst, nSecond, nChar)
  int  nFirst;
  int  nSecond;
  int  nChar;
{
  char sBuf[128], sChr[8];


  if (nChar < 32)
    {
    sChr[0] = '^';
    sChr[1] = nChar + 64;
    sChr[2] = NULL;
    }
  else
    {
    sChr[0] = nChar;
    sChr[1] = NULL;
    }

  sprintf(sBuf, "%s", sChr);
  LineToTerminal(sBuf);
}


static void near Show2Int(nFirst, nSecond, nInt)
  int  nFirst;
  int  nSecond;
  int  nInt;
{
  char sBuf[128], sChr[8];


  sprintf(sBuf, "is %d", nInt);
  LineToTerminal(sBuf);
}


static void near StartIfNull()
{
  if (nKermitMode == KM_NULL && bConnected && !(wSystem & SS_XFER))
    InitKermit(FALSE);
}

static int near LookupKeyword(pStr)
  PSTR pStr;
{
  return (TranslateToken(0, pStr));
}

static PSTR near GetStringToken(pStr, pToken)
  PSTR pStr;
  PSTR pToken;
{
  int ch, i;


  while (isspace(*pStr))
    pStr++;

  i = 0;
  while (*pStr && !isspace(*pStr))
    {
    ch = *pStr++;
    *pToken++ = ch;

    if (i++ >= 80)
      break;
    }

  *pToken = NULL;

  return (pStr);
}


static PSTR near GetString(pStr, pToken)
  PSTR pStr;
  PSTR pToken;
{
  int ch, i;


  while (isspace(*pStr))
    pStr++;

  while (*pStr != NULL && !isspace(*pStr))
    {
    ch = *pStr++;
    *pToken++ = ch;
    }

  *pToken = NULL;

  return (pStr);
}


static PSTR near GetNumeric(pStr, pToken)
  PSTR pStr;
  WORD *pToken;
{
  *pToken = 0;

  while (isspace(*pStr))
    pStr++;

  if (!isdigit(*pStr))
    return (pStr);

  if (*pStr == '0')
    {
    pStr++;
    if (*pStr == 'x' || *pStr == 'X')
      {
      pStr++;
      while (1)
	{
	if (isdigit(*pStr))
          *pToken = *pToken * 16 + *pStr - '0';
	else if (isxdigit(*pStr))
          *pToken = *pToken * 16 + toupper(*pStr) - 'A' + 10;
	else
	  break;

	pStr++;
	}
      }
    else
      {
      while (*pStr >= '0' && *pStr <= '7')
        *pToken = *pToken * 8 + *pStr++ - '0';
      }
    }
  else
    {
    while (isdigit(*pStr))
      *pToken = *pToken * 10 + *pStr++ - '0';
    }

  return (pStr);
}


static int near GetChar(pStr)
  PSTR pStr;
{
  int nChar;


  while (isspace(*pStr))
    pStr++;

  if (*pStr == '^')
    nChar = *(pStr+1) - 64;
  else
    nChar = *pStr;


  return (nChar);
}


static int near GetInt(pStr)
  PSTR pStr;
{
  int nInt;


  nInt = 0;

  while (isspace(*pStr))
    pStr++;

  if (!isdigit(*pStr))
    return (-1);

  if (*pStr == '0')
    {
    pStr++;
    if (*pStr == 'x' || *pStr == 'X')
      {
      pStr++;
      while (1)
	{
	if (isdigit(*pStr))
          nInt = nInt * 16 + *pStr - '0';
	else if (isxdigit(*pStr))
          nInt = nInt * 16 + toupper(*pStr) - 'A' + 10;
	else
	  break;

	pStr++;
	}
      }
    else
      {
      while (*pStr >= '0' && *pStr <= '7')
        nInt = nInt * 8 + *pStr++ - '0';
      }
    }
  else
    {
    while (isdigit(*pStr))
      nInt = nInt * 10 + *pStr++ - '0';
    }

  return (nInt);
}


static PSTR near GetStringArgument(sMessage, pStr)
  PSTR sMessage;
  PSTR pStr;
{
  char sArgument[96];
  register i, j;


  if (*pStr == NULL)
    return (pStr);

  pStr = GetString(pStr, sArgument);

  if (j = strlen(sArgument))
    {
    i = strlen(sMessage);
    *(sMessage + i) = j + 32;
    strcpy(sMessage + i + 1, sArgument);
    }

  return (pStr);
}
