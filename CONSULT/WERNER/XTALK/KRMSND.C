/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Send Module (KRMSND.C)                                   *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module supports the Kermit send (upload) routines.         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*  1.1  4/24/90 dca00094 MKL assigned packetsize to Xfer.nPacketSize          *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
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

extern void near SndPar(PSTR);

static int  near EncodeFileData(PSTR, int);

void near SendStart()
{
  char buf[64];


  filexfer.kermit.lStatFileChars = 0;
  filexfer.kermit.lStatCommIn	 = 0;
  filexfer.kermit.lStatCommOut	 = 0;
  filexfer.kermit.lStatStartTime = GetCurrentTime();

  fXferDirection = TRUE;

  GetOurParams(buf);
  SendPacket('S', 0, strlen(buf), buf);

  InfoMessage(MSGINF_KERMIT_SEXCH);
  ResetXferIndicators(FALSE);
  filexfer.kermit.cMode = SEND_SEND_START;
}

void near SendAnother()
{
  char buf[64];

  SendHeader();
  ResetXferIndicators(FALSE);
}


void near SendStartAck()
{
  char buf[64];

  SndPar(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset);
}


void near SendHeader()
{
  FILESTATUS fsts;
  char buf[64];
  char *ptr;

  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;
  wXferConsecErrors = 0;

//if (filexfer.kermit.fFileNamesConverted)
//  ptr = StripPath(szXferFilePath);
//else
    ptr = szXferFilePath;

  SendPacket('F', filexfer.kermit.cCurSeq, strlen(ptr), ptr);

  InfoMessage(MSGINF_KERMIT_SFILE);

  DosQFileInfo(filexfer.kermit.hFile, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;

  lXferFileBytes = 0;
  wXferCurrFiles++;

  fXferDirection = TRUE;
  InitXferIndicators();

  filexfer.kermit.cMode = SEND_FILE_DATA;
}


void near SendAttr()
{
  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;
  wXferConsecErrors = 0;

}


void near SendData()
{
  char	buf[KERMIT_MAXBUF];
  int	i;


  if (filexfer.kermit.fEof || filexfer.kermit.bAbortFile)
    {
    SendEof();
    return;
    }

  if (lXferFileBytes == 0L)
    {
    InfoMessage(MSGINF_KERMIT_SDATA);
    }


  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;
  wXferConsecErrors = 0;

  lXferFileBytes += EncodeFileData(buf, filexfer.kermit.nSndPacketSize);

  SendPacket('D', filexfer.kermit.cCurSeq, strlen(buf), buf);

  wXferBlockCount++;
  UpdateXferIndicators();
}


void near SendEof()
{

  if (filexfer.kermit.hFile != -1)
    {
    KermitClose();
    szXferFilePath[0] = '\0';
    filexfer.kermit.lStatFileChars += lXferFileBytes;
    }

  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;
  wXferConsecErrors = 0;

  if (!filexfer.kermit.bAbortFile)
    {
    SendPacket('Z', filexfer.kermit.cCurSeq, 0, 0);
    TermMessage(MSGINF_KERMIT_UGOOD, NULL);
    filexfer.kermit.fStatLastXfer = TRUE;
    }
  else
    {
    SendPacket('Z', filexfer.kermit.cCurSeq, 1, "D");
    TermMessage(MSGINF_KERMIT_UBAD, NULL);
    filexfer.kermit.bAbortFile = FALSE;
    filexfer.kermit.fStatLastXfer = FALSE;
    }

  if (filexfer.kermit.fBellOn)
    MessageBeep(NULL);

  filexfer.kermit.cMode = SEND_END_FILE;
}


void near SendEob()
  {
  USHORT usAction;


  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;
  wXferConsecErrors = 0;

  if (filexfer.kermit.fWildCardSend && !filexfer.kermit.bAbortBatch)
    {
    if (_dos_findnext(&filexfer.kermit.sendfile) == 0)
      {
      strcpy(StripPath(szXferFilePath), filexfer.kermit.sendfile.name);

      if (KermitOpen(szXferFilePath, FALSE) == 0)
	{
	SendAnother();
	return;
	}
      }
    }
  else
    {
    filexfer.kermit.fStatLastXfer = FALSE;
    }

  if (bKermitNonServer)
    filexfer.kermit.fAbortKermit = TRUE;

  SendPacket('B', filexfer.kermit.cCurSeq, 0, 0);
  filexfer.kermit.cMode = SEND_END_BATCH;

  filexfer.kermit.lStatStopTime = GetCurrentTime();
}


static int near EncodeFileData(PSTR pStr, int nLen)
  {
  WORD ch, lastch;
  int  nBytes, nChars, nCount;


  nBytes = 0;
  lastch = (WORD)KermitGetChar();
  nChars = 1;
  nCount = 1;

  // dca00094 MKL
  // nXferBlockSize = nLen;
  Xfer.nPacketSize = nXferBlockSize = nLen;

  while (nBytes < nLen - 10 && !filexfer.kermit.fEof)
    {
    ch = (WORD)KermitGetChar();
    if (filexfer.kermit.fEofCtrlZ && ch == 26)
      filexfer.kermit.fEof = TRUE;

    nChars++;

    if (ch == lastch && filexfer.kermit.bSndRepeatPrefixFlag)
      {
      nCount++;
      if (nCount == 95)
	{
	*pStr++ = filexfer.kermit.cSndRepeatPrefix;
	*pStr++ = tochar(94);
	nBytes += 2;
	nCount	= 0;
	}
      else
	continue;
      }

    if (nCount > 1)
      {
      *pStr++ = filexfer.kermit.cSndRepeatPrefix;
      *pStr++ = tochar((char)nCount);
      nBytes += 2;
      }

    if (filexfer.kermit.cEightBitPrefix != 'N')
      {
      if (lastch & 0x80)
	{
	*pStr++ = filexfer.kermit.cEightBitPrefix;
	nBytes++;
	lastch &= 0x7F;
	}
      }

    if ((lastch & 0x7F) < 32 || (lastch & 0x7F) == 127)
      {
      *pStr++ = filexfer.kermit.cSndCtlq;
      nBytes++;
      lastch ^= 0x40;
      }

    if (lastch == filexfer.kermit.cSndCtlq)
      {
      *pStr++ = filexfer.kermit.cSndCtlq;
      nBytes++;
      }
    else if (lastch == filexfer.kermit.cEightBitPrefix && filexfer.kermit.cEightBitPrefix != 'N')
      {
      *pStr++ = filexfer.kermit.cSndCtlq;
      nBytes++;
      }
    else if (lastch == filexfer.kermit.cSndRepeatPrefix && filexfer.kermit.bSndRepeatPrefixFlag)
      {
      *pStr++ = filexfer.kermit.cSndCtlq;
      nBytes++;
      }

    *pStr++ = (char)lastch;
    nBytes++;

    lastch = ch;
    nCount = 1;
    }

  *pStr = NULL;

  if (!filexfer.kermit.fEof)
    KermitUnget(ch);

  return (nChars - 1);
}


void GetOurParams(ptr)
  char *ptr;
{
  char ch;

  *ptr++ = tochar((char)filexfer.kermit.nRcvPacketSize);
  *ptr++ = tochar((char)filexfer.kermit.nRcvTimeOut);
  *ptr++ = tochar((char)filexfer.kermit.nRcvPadNum);
  *ptr++ = filexfer.kermit.cRcvPadChar;
  *ptr++ = tochar(filexfer.kermit.cRcvEOL);
  *ptr++ = filexfer.kermit.cRcvCtlq;

  if (ComDCB.ByteSize == 8)
    *ptr++ = 'Y';
  else
    *ptr++ = '&';

  *ptr++ = filexfer.kermit.cBlockCheck + '0';
  *ptr++ = filexfer.kermit.cRcvRepeatPrefix;

  *ptr++ = tochar(2);	     /* 8 = ATTR */

  *ptr++ = tochar(0);
  *ptr++ = tochar((KERMIT_MAXBUF - 64) / 95);
  *ptr++ = tochar((KERMIT_MAXBUF - 64) % 95);

  *ptr++ = NULL;
}
