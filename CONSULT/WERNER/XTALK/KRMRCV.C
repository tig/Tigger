/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Receive Module (KRMRCV.C)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module support the Kermit receive (download) routines.     *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/12/89 00035 Abort if disk is full on dumb download.     *
*	      1.01 11/07/89 00056 Don't display result mesage for text dl.    *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include <direct.h>
#include <dos.h>	//dca00046 JDB
#include "xtalk.h"
#include "library.h"
#include "kermit.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "xfer.h"
#include "global.h"
#include "variable.h"

#define PKT_WAIT_SOH	    1
#define PKT_SKIP_SOH	    2
#define PKT_WAIT_LEN	    3
#define PKT_WAIT_DATA	    4
#define PKT_WAIT_EOL	    5

#if defined(KENT)
static char tbuf[180];
extern HANDLE hDlogFile;
#endif

void near SndInit();
void near RcvYPar();
void near SndPar(char *);

void near RcvInit();
void near RcvStart();
void near RcvFile();
void near RcvText();
void near RcvAttr();
void near RcvData();
void near RcvEOF();
void near RcvPar(char *);

void near SendPacket(char, char, int, char *);
void near ReReceive();
void near ReSend();
void near AckPacket();
void near AckSPacket(char *);

int near PacketBuild();
int near PacketTimeOut();

static int near ComputeChk1(char *);
static int near ComputeChk2(char *);
static int near ComputeChk3(char *);
static HFILE near OpenRcvFile(PSTR);

void near SndInit()
{
  char buf[64];

  RcvPar(buf);
  SendPacket('I', 0, strlen(buf), buf);

  InfoMessage(MSGINF_KERMIT_IEXCH);
  /*
  filexfer.kermit.cMode = WAIT_FOR_I_ACK;
  */
}

void near RcvYPar()
{
  char buf[64];

  SndPar(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset);
  AckPacket();

  filexfer.kermit.cMode = SEND_FILE_HEADER;
}

void near RcvInit()
{
  char buf[64];

  SndPar(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset);
  RcvPar(buf);
  AckSPacket(buf);

  InfoMessage(MSGINF_KERMIT_IEXCH);
}

void near RcvStart()
{
  char buf[64];

  filexfer.kermit.lStatFileChars = 0L;
  filexfer.kermit.lStatCommIn	 = 0L;
  filexfer.kermit.lStatCommOut	 = 0L;
  filexfer.kermit.lStatStartTime = GetCurrentTime();
  filexfer.kermit.fStatLastXfer = TRUE;

  SndPar(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset);
  RcvPar(buf);

  AckSPacket(buf);

  filexfer.kermit.cMode = WAIT_FILE_HEADER;
  InfoMessage(MSGINF_KERMIT_SEXCH);
}

void near RcvFile()
{
  char buf[128];
  char *ptr;

  if (szXferFilePath[0] == NULL)
    DecodeBuffData(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset, szXferFilePath, 64);

  if (ptr = strchr(szXferFilePath, ':'))
    {
    strcpy(buf, ptr+1);
    strcpy(szXferFilePath, buf);
    }
  if (ptr = strchr(szXferFilePath, ']'))
    {
    strcpy(buf, ptr+1);
    strcpy(szXferFilePath, buf);
    }

  if (filexfer.kermit.fFileNamesConverted)
    strcpy(szXferFilePath, StripPath(szXferFilePath));

  AckPacket();

  filexfer.kermit.cMode = WAIT_FILE_DATA;
  InfoMessage(MSGINF_KERMIT_NAME);

  wXferCurrFiles++;

  fXferDirection = FALSE;
  InitXferIndicators();

  lXferFileSize = 0;
  filexfer.kermit.fTextFlag = FALSE;
}

void near RcvText()
{
  AckPacket();

  filexfer.kermit.cMode = WAIT_FILE_DATA;
  InfoMessage(MSGINF_KERMIT_RTEXT);

  lXferFileSize = 0;
  filexfer.kermit.fTextFlag = TRUE;
}


void near RcvAttr()
{
  char *ptr;
  char attr;
  int  len;
  char buf[32];


  InfoMessage(MSGINF_KERMIT_ATTR);

  ptr = filexfer.kermit.sRcvPacket + filexfer.kermit.nRcvDataOffset;

  while (*ptr)
    {
    attr = *ptr++;
    len  = unchar(*ptr++);

    switch (attr)
      {
      case '!':
	lXferFileSize = (long)atol(ptr) * 1024L;
	break;

      case '"':
        if (*ptr == 'A')
	  sprintf(buf, "%s, ASC", szXferFilePath);
        else if (*ptr == 'B')
	  sprintf(buf, "%s, BIN", szXferFilePath);
        else if (*ptr == 'I')
	  sprintf(buf, "%s, IMG", szXferFilePath);
	else
	  break;

	SetDlgItemText(hXferDlg, ID_XFER_MSG, buf);
	break;

      case '1':
	lXferFileSize = (long)atol(ptr);
	break;

      default:
	break;
      }

    ptr += len;
    }

  AckPacket();

  ResetXferIndicators(FALSE);
}


void near RcvData()
{
  char *ptr;
  BYTE ch;
  int  nRptCnt, nBytes, i;
  BOOL bSet8Bit;

  char buf[80];
  int  inx;


  if (filexfer.kermit.hFile == -1 && !filexfer.kermit.fTextFlag)
    {
    SetFullPath(VAR_DIRFIL);

    if ((filexfer.kermit.hFile = OpenRcvFile(szXferFilePath)) == -1)
      {
      if (bKermitNonServer)
	{
	nErrorCode = XlatError(nErrorCode);
	if (nErrorCode == ERRDOS_NOFILE)
	  nErrorCode = ERRDOS_WRITEPROTECTED;
	TermMessage(MSGINF_KERMIT_ABORTED, nErrorCode);
	KermitUserAbort();
	}
      else
	AckSPacket("X");
      return;
      }

    InfoMessage(MSGINF_KERMIT_RDATA);

    SetDlgItemStr(hXferDlg, ID_XFER_MSG, 0);
    }

  /***/

  ptr = filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset;
  nBytes = 0;
  inx = 0;


  while (ch = *ptr++)
    {
    nRptCnt  = 1;
    bSet8Bit = FALSE;

    if (filexfer.kermit.bSndRepeatPrefixFlag)
      {
      if (ch == filexfer.kermit.cSndRepeatPrefix)
	{
	nRptCnt = unchar(*ptr++);
	ch = *ptr++;
	}
      }

    if (filexfer.kermit.cEightBitPrefix != 'N')
      {
      if (ch == filexfer.kermit.cEightBitPrefix)
	{
	bSet8Bit = TRUE;
	ch = *ptr++;
	}
      }

    if (ch == filexfer.kermit.cSndCtlq)
      {
      ch = *ptr++;
      if (ch != filexfer.kermit.cSndCtlq)
	{
	i = ch & 0x7F;
        if ((i >= 0100 && i <= 0137) || i == '?')
	  ch = ch ^ 64;
	}
      }

    if (bSet8Bit)
      ch |= 0x80;

    nBytes += nRptCnt;

    if (!filexfer.kermit.fTextFlag)
      {
      while (nRptCnt--)
	{
	if (EOF == KermitPutChar(ch))
	  {
	  nErrorCode = ERRDOS_DISKFULL;
	  filexfer.kermit.bAbortBatch = TRUE;

	  // 00035  Abort if disk is full on dumb download.
	  if (bKermitNonServer)
	    {
	    TermMessage(MSGINF_KERMIT_ABORTED, ERRXFR_LOCALDISKFULL);
	    KermitUserAbort();
	    }
	  else
	    AckSPacket("Z");

	  return;
	  }
	}
      }
    else
      {
      while (nRptCnt--)
	{
	buf[inx++] = ch;
	if (inx == sizeof(buf))
	  {
	  SendMessage(hChildTerminal, UM_DATA, inx, (DWORD)((LPSTR)buf));
	  inx = 0;
	  }
	}
      }
    }

  if (inx)
    SendMessage(hChildTerminal, UM_DATA, inx, (DWORD)((LPSTR)buf));

  /***/

  if (filexfer.kermit.bAbortBatch)
    {
    // 00035  Abort if disk is full on dumb download.
    if (bKermitNonServer)
      {
      TermMessage(MSGINF_KERMIT_ABORTED, ERRXFR_LOCALDISKFULL);
      KermitUserAbort();
      }
    else
      AckSPacket("Z");

    filexfer.kermit.bAbortFile = FALSE;
    filexfer.kermit.fStatLastXfer = FALSE;
    }
  else if (filexfer.kermit.bAbortFile)
    {
    AckSPacket("X");
    filexfer.kermit.bAbortBatch = FALSE;
    filexfer.kermit.fStatLastXfer = FALSE;
    }
  else
    {
    AckPacket();
    }

  lXferFileBytes      += nBytes;
  filexfer.kermit.lStatFileChars += nBytes;
  wXferBlockCount++;

  UpdateXferIndicators();
}


void near RcvEOF()
{
  if (!filexfer.kermit.fTextFlag)
    {
    KermitClose();
    }
  else
    {
    char buf[1];

    buf[0] = NULL;
    if (nTermChrX != 1)
      LineToTerminal(buf);
    }

  if (filexfer.kermit.sRcvPacket[3] == 'D')
    {
    if (!filexfer.kermit.fFileKeep)
      DosDelete(szXferFilePath, 0L);
    TermMessage(MSGINF_KERMIT_DBAD, NULL);
    filexfer.kermit.fStatLastXfer = FALSE;
    }
  else
    {
    // 00056 Don't display result message for text dl.
    if (!filexfer.kermit.fTextFlag)
      TermMessage(MSGINF_KERMIT_DGOOD, NULL);
    filexfer.kermit.fStatLastXfer = TRUE;
    }

  szXferFilePath[0] = '\0';

  AckPacket();

  if (filexfer.kermit.fBellOn)
    MessageBeep(NULL);

  filexfer.kermit.cMode = WAIT_FILE_HEADER;
  lXferFileSize  = 0L;
  lXferFileBytes = 0L;

  ResetXferIndicators(FALSE);
  InfoMessage(MSGKRM_IDLE);

  filexfer.kermit.lStatStopTime = GetCurrentTime();
}


void near SndPar(ptr)
  char *ptr;
{
  int x;

  filexfer.kermit.fExchangedParameters = TRUE;

  /* Packet Size to Send */
  x = (*ptr != NULL) ? unchar(*ptr++) : PACKET_SIZE;
  if (filexfer.kermit.bSndPacketSizeOverride == FALSE)
    filexfer.kermit.nSndPacketSize = (x < 10) ? PACKET_SIZE : x;

  /* Timeout */
  x = (*ptr != NULL) ? unchar(*ptr++) : LOCAL_TIMEOUT;
  if (filexfer.kermit.bSndTimeOutOverride == FALSE)
    filexfer.kermit.nSndTimeOut = (x < 0) ? LOCAL_TIMEOUT : x;

  /* Padding Number */
  filexfer.kermit.nSndPadNum = (*ptr != NULL) ? unchar(*ptr++) : 0;

  /* Padding Character */
  filexfer.kermit.cSndPadChar = (*ptr != NULL) ? *ptr++ : 0;

  /* Terminator */
  filexfer.kermit.cSndEOL = (*ptr != NULL) ? unchar(*ptr++) : ASC_CR;
  if (filexfer.kermit.cSndEOL < 2 || filexfer.kermit.cSndEOL > 037)
    filexfer.kermit.cSndEOL = ASC_CR;

  /* Control Prefix */
  filexfer.kermit.cSndCtlq = (*ptr != NULL) ? *ptr++ : CONTROL_QUOTE;

  /* Eight Bit Prefix */
  if (*ptr != NULL)
    {
    filexfer.kermit.cEightBitPrefix = *ptr++;
    if (filexfer.kermit.cEightBitPrefix == 'Y' && ComDCB.Parity == NOPARITY)
      filexfer.kermit.cEightBitPrefix = 'N';
    else if (filexfer.kermit.cEightBitPrefix == 'Y' && ComDCB.Parity != NOPARITY)
      filexfer.kermit.cEightBitPrefix = '&';
    else if ((filexfer.kermit.cEightBitPrefix <= 0040 || filexfer.kermit.cEightBitPrefix >= 0100) &&
	(filexfer.kermit.cEightBitPrefix <= 0140 || filexfer.kermit.cEightBitPrefix >= 0177))
      filexfer.kermit.cEightBitPrefix = 'N';
    else
      filexfer.kermit.cEightBitPrefix = '&';
    }
  else
    filexfer.kermit.cEightBitPrefix = 'N';

  /* Block Check Type */
  filexfer.kermit.cBlockCheck = (*ptr != NULL) ? *ptr++ - '0' : 1;
  if (filexfer.kermit.cBlockCheck < 1 || filexfer.kermit.cBlockCheck > 3)
    filexfer.kermit.cBlockCheck = 1;

  /* Repeat Prefix */
  if (*ptr != NULL)
    {
    filexfer.kermit.cSndRepeatPrefix = *ptr++;
    filexfer.kermit.bSndRepeatPrefixFlag = TRUE;
    }
  else
    filexfer.kermit.bSndRepeatPrefixFlag = FALSE;

  /* Capas */
  if (*ptr != NULL)
    {
    if (*ptr & 0x02)
      {
      filexfer.kermit.bExtendedHeaderFlag = TRUE;

      ptr += 2;
      x    = unchar(*ptr++) * 95;
      x   += unchar(*ptr++);

      if (x > filexfer.kermit.nSndPacketSize)
	filexfer.kermit.nSndPacketSize = x;

      if (filexfer.kermit.nSndPacketSize > KERMIT_MAXBUF - 64)
	filexfer.kermit.nSndPacketSize = KERMIT_MAXBUF - 64;
      }
    else
      filexfer.kermit.bExtendedHeaderFlag = FALSE;
    }
  else
    {
    filexfer.kermit.bExtendedHeaderFlag = FALSE;
    }
}

void near RcvPar(ptr)
  char *ptr;
{
  char ch;

  *ptr++ = tochar(94);
  *ptr++ = tochar((char)filexfer.kermit.nRcvTimeOut);
  *ptr++ = tochar(filexfer.kermit.nRcvPadNum);
  *ptr++ = filexfer.kermit.cRcvPadChar;
  *ptr++ = tochar(filexfer.kermit.cRcvEOL);
  *ptr++ = filexfer.kermit.cRcvCtlq;

  if (ComDCB.ByteSize == 8)
    *ptr++ = filexfer.kermit.cEightBitPrefix;
  else
    *ptr++ = '&';

  *ptr++ = filexfer.kermit.cBlockCheck + '0';
  *ptr++ = filexfer.kermit.cRcvRepeatPrefix;

  *ptr++ = tochar(10);

  *ptr++ = tochar(0);
  *ptr++ = tochar((KERMIT_MAXBUF - 64) / 95);
  *ptr++ = tochar((KERMIT_MAXBUF - 64) % 95);

  *ptr++ = NULL;
}

void near RcvServerCommand()
{
  SendPacket('E', 0, 38, "Remote command not supported!\r");
}

void near RequestFileCommand()
{
  DecodeBuffData(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset, szXferFilePath, 64);

  if (filexfer.kermit.fFileNamesConverted)
    strcpy(szXferFilePath, StripPath(szXferFilePath));

  KermitSendFile(szXferFilePath, szXferFilePath);
}

static HFILE near OpenRcvFile(PSTR sFileName)
  {
  HFILE  hfRcvFile;
  USHORT usAction;
  char	 szPath[260], buf[16], tmp[64];
  int  i;


  if (filexfer.kermit.fFileNamesConverted)
    {

    }

  filexfer.kermit.fWrite = TRUE;
  filexfer.kermit.fEof	 = FALSE;
  filexfer.kermit.iBuf	 = MAX_BUFFER;
  filexfer.kermit.pBuf	 = filexfer.kermit.abBuf;

  CreatePath(szPath, VAR_DIRFIL, sFileName);

  if (nErrorCode = DosOpen(szPath, &hfRcvFile, &usAction, 0L, FILE_NORMAL,
	       FILE_OPEN, OPEN_ACCESS_READWRITE, 0L))
    {
    if (nErrorCode = DosOpen(szPath, &hfRcvFile, &usAction, 0L, FILE_NORMAL,
	       FILE_OPEN | FILE_CREATE, OPEN_ACCESS_READWRITE, 0L))
      return (-1);
    else
      return (hfRcvFile);
    }

  if (filexfer.kermit.fFileSupersede)
    {
    DosClose(hfRcvFile);
    return (-1);
    }

  if (!filexfer.kermit.fFileWarning)
    return (hfRcvFile);

  DosClose(hfRcvFile);

  buf[ 0] = 'X';
  buf[ 1] = '0';
  buf[ 2] = '0';
  buf[ 3] = '0';
  buf[ 4] = '0';
  buf[ 5] = '0';
  buf[ 6] = '0';
  buf[ 7] = '0';
  buf[ 8] = '.';
  buf[ 9] = ' ';
  buf[10] = ' ';
  buf[11] = ' ';
  buf[12] = NULL;

  for (i = 0; i < 7; i++)
    {
    if (*sFileName == '.' || *sFileName == NULL)
      break;

    buf[i] = *sFileName++;
    }

  while (*sFileName != '.' && *sFileName != NULL)
    sFileName++;

  if (*sFileName == '.')
    {
    sFileName++;
    for (i = 9; i < 12; i++)
      {
      if (*sFileName == NULL)
	break;

      buf[i] = *sFileName++;
      }
    }

  for (i = 0; i < 9; i++)
    {
    CreatePath(szPath, VAR_DIRFIL, buf);

    if (nErrorCode = DosOpen(szPath, &hfRcvFile, &usAction, 0L, FILE_NORMAL,
	       FILE_OPEN, OPEN_ACCESS_READWRITE, 0L))
      {
      sprintf(tmp, "Kermit: Warning, renamed file to '%s'.", buf);
      LineToTerminal(tmp);

      if (nErrorCode = DosOpen(szPath, &hfRcvFile, &usAction, 0L, FILE_NORMAL,
		 FILE_OPEN | FILE_CREATE, OPEN_ACCESS_READWRITE, 0L))
	return (-1);
      else
	return (hfRcvFile);
      }
    else
      DosClose(hfRcvFile);

    buf[7] = (char)('0' + i);
    }

  return (-1);
  }
