/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Common Transfer Logic (KRMCOM.C)                         *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains various logic to support Kermit xfers.     *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/08/89 00007 Change Kermit logic to not use the CR char  *
*                                   as an event flag.  Redesigned Receive     *
*                                   Packet routine.                           *
* 1.1  04/17/90 dca00087 MKL fixed consec error for each block sent problem   *
* 1.1  04/24/90 dca00094 MKL assign packetsize to Xfer.nPacketSize 	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "kermit.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "xfer.h"
#include "capture.h"
#include "global.h"
#include "variable.h"

#define PKT_WAIT_SOH	    1
#define PKT_SKIP_SOH	    2
#define PKT_WAIT_LEN	    3
#define PKT_WAIT_DATA	    4
#define PKT_WAIT_EOL	    5

static int near ComputeChk1(char *);
static int near ComputeChk2(char *);
static int near ComputeChk3(char *);


int KermitMessage(pMsg)
  PSTR pMsg;
{
  char buf[64];


  strcpy(szXferFilePath, pMsg+1);

  filexfer.kermit.lStatFileChars = 0;
  filexfer.kermit.lStatCommIn	 = 0;
  filexfer.kermit.lStatCommOut	 = 0;
  filexfer.kermit.lStatStartTime = GetCurrentTime();

  GetOurParams(buf);
  SendPacket('I', 0, strlen(buf), buf);

  filexfer.kermit.cMode = WAIT_COMMAND_SYNC;

  if (nKermitMode != KM_SERVER)
    nKermitMode = KM_BUSY;
  return (0);
}

USHORT KermitRemoteMessage(pMsg)
  PSTR pMsg;
{
  SendPacket('G', 0, strlen(pMsg), pMsg);
  filexfer.kermit.cMode = WAIT_FOR_REPLY;

  if (nKermitMode != KM_SERVER)
    nKermitMode = KM_BUSY;

  return (0);
}

int KermitGetFile(pRemote, pLocal)
  PSTR pRemote;
  PSTR pLocal;
{
  if (pLocal && *pLocal != NULL)
    strcpy(szXferFilePath, pLocal);
  else
    szXferFilePath[0] = NULL;

  SendPacket('R', 0, strlen(pRemote), pRemote);
  filexfer.kermit.cMode = WAIT_SEND_INIT;

  if (nKermitMode != KM_SERVER)
    nKermitMode = KM_BUSY;

  return (0);
}


int KermitReceive(pLocal)
  PSTR pLocal;
{
  if (pLocal && *pLocal)
    strcpy(szXferFilePath, pLocal);
  else
    szXferFilePath[0] = NULL;

  CommCharOutput(ASC_XON);

  filexfer.kermit.cMode = WAIT_SEND_INIT;

  if (nKermitMode != KM_SERVER)
    nKermitMode = KM_BUSY;

  InfoMessage(MSGKRM_PROCESSING);

  return (0);
}


int KermitSendFile(pLocal, pRemote)
  PSTR pLocal;
  PSTR pRemote;
{
  char sFileName[96], sAccess[4];


  SetPath(pLocal);
  if (strchr(pLocal, '?') || strchr(pLocal, '*'))
    {
    strcpy(sFileName, StripPath(pLocal));

    filexfer.kermit.fWildCardSend = TRUE;

    if (_dos_findfirst(sFileName, _A_NORMAL | _A_RDONLY, &filexfer.kermit.sendfile) != 0)
      return (ERRKRM_NOWILDFILE);

    if (pRemote[0] != NULL)
      {
      while (stricmp(pRemote, filexfer.kermit.sendfile.name) != 0)
	{
        if (_dos_findnext(&filexfer.kermit.sendfile) != 0)
	  return (ERRKRM_NOWILDSTART);
	}
      }

    strcpy(szXferFilePath, filexfer.kermit.sendfile.name);

    if (KermitOpen(szXferFilePath, FALSE))
      return (ERRKRM_FILEREAD);
    }
  else
    {
    filexfer.kermit.fWildCardSend = FALSE;

    strcpy(szXferFilePath, pLocal);
  //KermitFileName(szXferFilePath);
    if (KermitOpen(szXferFilePath, FALSE))
      return (ERRKRM_FILEREAD);

    if (*pRemote != NULL && *pRemote !=  ' ')
      strcpy(szXferFilePath, pRemote);
    else
      strcpy(szXferFilePath, StripPath(pLocal));
    }

  SendStart();

  if (nKermitMode != KM_SERVER)
    nKermitMode = KM_BUSY;

  return FALSE;
}


int near ReceivePacket()
{
  char *ptr;
  int  crc;
  int  chktyp;
  register int i,j;


  //  00007 Start of change (Look down).
  //  If the 'iInputPacket' index is still 0 then we are looking for the
  //  mark character ^A
  if (filexfer.kermit.iInputPacket == 0)
    {
    while ((i = CommGet()) != -1)
      {
      if (i == filexfer.kermit.cRcvStartChar)
        break;
      }
    if (i == -1)
      return (-1);  // Didn't find the start of packet character yet.
    else
      filexfer.kermit.sInputPacket[filexfer.kermit.iInputPacket++] = i;
    }

  //  At this point we found the start of packet character, lets start looking
  //    for the end of packet character (CR) and fill in the buffer.
  while ((i = CommGet()) != -1)
    {
    if (filexfer.kermit.iInputPacket < KERMIT_MAXBUF - 3)
      filexfer.kermit.sInputPacket[filexfer.kermit.iInputPacket++] = i;

    if (i == ASC_CR)
      {
      filexfer.kermit.sInputPacket[filexfer.kermit.iInputPacket++] = ASC_CR;
      filexfer.kermit.sInputPacket[filexfer.kermit.iInputPacket++] = '\0';
      ptr = filexfer.kermit.sInputPacket;
      filexfer.kermit.iInputPacket = 0;
      break;
      }
    }

  if (i == -1)
    return (-1);  // Not a complete packet yet, go back and wait for more.

  //  We now have everything between the start of packet character and the
  //    end of packet character.  Lets process it and see what happens.
  //  00007 End of change (look up).

  if (nKermitMode == KM_BUSY)
    filexfer.kermit.lStatCommIn += strlen(ptr);

  /* Locate the start of packet character */
nextch:
  while (*ptr != filexfer.kermit.cRcvStartChar && *ptr)
    ptr++;

  if (strchr(ptr+1, filexfer.kermit.cRcvStartChar))
    {
    ptr++;
    goto nextch;
    }

  /* Skip multible start characters */
  while (*ptr == filexfer.kermit.cRcvStartChar && *ptr)
    ptr++;

  strcpy(filexfer.kermit.sRcvPacket, ptr);

  /* Load & Verify the Length Byte */
  filexfer.kermit.nRcvLen = unchar(*ptr++);

  if (filexfer.kermit.nRcvLen == 0)
    {
    i = 32 + *ptr;

    /* Load & Verify the Sequence Byte */
    filexfer.kermit.cRcvSeq = unchar(*ptr++);
    if (filexfer.kermit.cRcvSeq < 0 || filexfer.kermit.cRcvSeq > 63)
      return ('Q');

    i += *ptr;

    /* Load & Verify the Type Byte */
    filexfer.kermit.cRcvType = *ptr++;
    if (filexfer.kermit.cRcvType < 'A' || filexfer.kermit.cRcvType > 'Z')
      return ('Q');

    i += *ptr;

    j  = unchar(*ptr++) * 95;

    i += *ptr;

    j += unchar(*ptr++);

    if (*ptr++ != tochar((char)((i + ((i & 192) / 64)) & 63)))
      return ('Q');

    filexfer.kermit.nRcvLen = j;

    filexfer.kermit.nRcvDataOffset = 6;
    }
  else
    {
    if (filexfer.kermit.nRcvLen < 3 || filexfer.kermit.nRcvLen > 95)
      return ('Q');
    filexfer.kermit.nRcvLen -= 2;

    /* Load & Verify the Sequence Byte */
    filexfer.kermit.cRcvSeq = unchar(*ptr++);

    /* Load & Verify the Type Byte */
    filexfer.kermit.cRcvType = *ptr++;
    if (filexfer.kermit.cRcvType < 'A' || filexfer.kermit.cRcvType > 'Z')
      return ('Q');

    if (filexfer.kermit.cRcvType != 'E')
      if (filexfer.kermit.cRcvSeq < 0 || filexfer.kermit.cRcvSeq > 63)
        return ('Q');

    filexfer.kermit.nRcvDataOffset = 3;
    }

  // dca00094 MKL
  // nXferBlockSize = filexfer.kermit.nRcvLen;
  Xfer.nPacketSize = nXferBlockSize = filexfer.kermit.nRcvLen;

  if (filexfer.kermit.cRcvType == 'Y' && (filexfer.kermit.nRcvLen>4 || filexfer.kermit.cMode==WAIT_FOR_REPLY))
    chktyp = 1;
  else if (filexfer.kermit.cRcvType == 'S' || filexfer.kermit.cRcvType == 'I' || filexfer.kermit.cRcvType == 'E')
    chktyp = 1;
  else if (filexfer.kermit.cRcvType == 'N')
    chktyp = filexfer.kermit.nRcvLen;
  else
    chktyp = filexfer.kermit.cBlockCheck;

  ptr = filexfer.kermit.sRcvPacket + filexfer.kermit.nRcvDataOffset + filexfer.kermit.nRcvLen - chktyp;
  filexfer.kermit.cRcvChk[0] = *ptr;
  *ptr++ = NULL;
  filexfer.kermit.cRcvChk[1] = *ptr++;
  filexfer.kermit.cRcvChk[2] = *ptr++;


  /* Verify checksum or CRC */

  switch (chktyp)
    {
    case 1:				    /* Type 1 - 6 bit checksum	*/
      i = ComputeChk1(filexfer.kermit.sRcvPacket);
      if (filexfer.kermit.cRcvChk[0] != tochar((char)(i & 077)))
	return (filexfer.kermit.cRcvType = 'Q');
      break;

    case 2:				    /* Type 2 - 12 bit checksum */
      i = ComputeChk2(filexfer.kermit.sRcvPacket);
      if ((filexfer.kermit.cRcvChk[0] != tochar((char)((i & 07700) >> 6))) ||
	  (filexfer.kermit.cRcvChk[1] != tochar((char)(i & 077))))
	return (filexfer.kermit.cRcvType = 'Q');
      break;

    case 3:				    /* Type 3 - 16 bit CRC-CCITT */
      i = ComputeChk3(filexfer.kermit.sRcvPacket);
      if ((filexfer.kermit.cRcvChk[0] != tochar(((unsigned)(i & 0170000)) >> 12)) ||
	  (filexfer.kermit.cRcvChk[1] != tochar((i & 07700) >> 6)) ||
	  (filexfer.kermit.cRcvChk[2] != tochar(i & 077)))
	return (filexfer.kermit.cRcvType = 'Q');
      break;
    }

  if (filexfer.kermit.cRcvType == 'N')
    filexfer.kermit.cBlockCheck = chktyp;

  return (filexfer.kermit.cRcvType);
}


int near PacketTimeOut()
  {
  FlushComm(nCommID, 1);

  wXferTotalErrors ++;

  if (Xfer.cMaxErrors != 0 && wXferConsecErrors++ > Xfer.cMaxErrors)
    return ('E');

  ReReceive();

  return (-1);
  }


void SendPacketX(char type, char seq, int len, char *buf)
  {
  SendPacket(type, seq, len, buf);
  }

void near SendPacket(type, seq, len, buf)
  char type;
  char seq;
  int  len;
  char *buf;
{
  char *ptr, *datptr;
  int  chktyp;
  int i;


  filexfer.kermit.cCurSeq  = seq;
  filexfer.kermit.cPrvSeq  = (seq-1) & 63;

  filexfer.kermit.nSndLen  = len;
  filexfer.kermit.cSndSeq  = seq;
  filexfer.kermit.cSndType = type;

  ptr = filexfer.kermit.sSndPacket;

  /*
   Replies to a 'S' packet is always Check type 1
  */
  if ((type == 'Y' && len > 1) || type == 'S' || type == 'I')
    chktyp = 1;
  else if (seq == 0 && type != 'D' && type != 'A' && type != 'Z' &&
           type != 'B' && type != 'Y')
    chktyp = 1;
  else
    chktyp = filexfer.kermit.cBlockCheck;

  /*
   Do any requested padding
  */
  for (i = 0; i < filexfer.kermit.nSndPadNum; *ptr++ = filexfer.kermit.cSndPadChar);

  *ptr++ = filexfer.kermit.cSndStartChar;	   /* Start Packet with the start char */

  datptr = ptr; 			/* Mark position to start Block Chk */

  /* Put in Length */
  if (len <= 90)
    {
    *ptr++ = tochar(len + chktyp + 2);		    /* Put in the length  */
    *ptr++ = tochar(filexfer.kermit.cSndSeq = seq); /* The packet number  */
    *ptr++ = filexfer.kermit.cSndType = type;	    /* The packet type  */
    }
  else
    {
    i  = (*ptr++ = tochar(0));
    i += (*ptr++ = tochar(filexfer.kermit.cSndSeq = seq));     /* The packet number    */
    i += (*ptr++ = filexfer.kermit.cSndType = type);	       /* The packet type      */
    i += (*ptr++ = tochar((len + chktyp) / 95));
    i += (*ptr++ = tochar((len + chktyp) % 95));

    *ptr++ = tochar((i + ((i & 192) / 64)) & 63);
    }

  for (i = 0; i < len; i++)			    /* Data		    */
    *ptr++ = *buf++;
  *ptr = NULL;				/* Mark end of packet for block chk */

  switch (chktyp)
    {
    case 1:				    /* Type 1 - 6 bit checksum	*/
      *ptr++ = tochar(ComputeChk1(datptr));
      break;

    case 2:				    /* Type 2 - 12 bit checksum */
      i = ComputeChk2(datptr);
      *ptr++ = tochar((i & 07700) >> 6);
      *ptr++ = tochar(i & 077);
      break;

    case 3:				    /* Type 3 - 16 bit CRC-CCITT */
      i = ComputeChk3(datptr);
      *ptr++ = tochar(( (unsigned)(i & 0170000)) >> 12);
      *ptr++ = tochar((i & 07700) >> 6);
      *ptr++ = tochar(i & 077);
      break;
    }

  *ptr++ = filexfer.kermit.cSndEOL;		   /* EOL character */
  *ptr++ = NULL;			/* End of the packet */

  ReReceive();

  // dca00087 MKL fixed consec error for each block sent problem
  if (type == 'S') FlushComm(nCommID, 1);

  CommDataOutput(filexfer.kermit.sSndPacket, strlen(filexfer.kermit.sSndPacket));

  if (nKermitMode == KM_BUSY)
    filexfer.kermit.lStatCommOut += strlen(filexfer.kermit.sSndPacket);

}

void near AckPacket()
{
  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  SendPacket('Y', filexfer.kermit.cPrvSeq, 0, "");
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;

  wXferConsecErrors = 0;
}


void near AckSPacket(char *dat)
  {
  filexfer.kermit.cPrvSeq = filexfer.kermit.cRcvSeq;
  SendPacket('Y', filexfer.kermit.cPrvSeq, strlen(dat), dat);
  filexfer.kermit.cCurSeq = filexfer.kermit.cPrvSeq + 1 & 63;

  wXferConsecErrors = 0;
  }


void near ReSend()
  {
  ReReceive();

  CommDataOutput(filexfer.kermit.sSndPacket, strlen(filexfer.kermit.sSndPacket));

  if (nKermitMode == KM_BUSY)
    filexfer.kermit.lStatCommOut += strlen(filexfer.kermit.sSndPacket);
  }

void near ReReceive()
{
  filexfer.kermit.nPktCnt     = 0;
  filexfer.kermit.nPktState  = PKT_WAIT_SOH;

  filexfer.kermit.cRcvSeq    = 0;
  filexfer.kermit.cRcvType   = 0;
  memset(filexfer.kermit.sRcvPacket, 0, sizeof(filexfer.kermit.sRcvPacket));
  filexfer.kermit.cRcvChk[0] = 0;
  filexfer.kermit.cRcvChk[1] = 0;
  filexfer.kermit.cRcvChk[2] = 0;

  filexfer.kermit.lTimeOut = GetCurrentTime() + filexfer.kermit.nRcvTimeOut * 1000L;
}


int near DecodeBuffData(sInBuf, sOtBuf, nOtLen)
  PSTR sInBuf;
  PSTR sOtBuf;
  int  nOtLen;
{
  char ch;
  int  nRptCnt, nBytes, i;
  BOOL bSet8Bit;
  nBytes = 0;

  while (ch = *sInBuf++)
    {
    nRptCnt = 1;
    bSet8Bit = FALSE;

    if (filexfer.kermit.bSndRepeatPrefixFlag)
      {
      if (ch == filexfer.kermit.cSndRepeatPrefix)
	{
	nRptCnt = unchar(*sInBuf++);
	ch = *sInBuf++;
	}
      }

    if (filexfer.kermit.cEightBitPrefix != 'N')
      {
      if (ch == filexfer.kermit.cEightBitPrefix)
	{
	bSet8Bit = TRUE;
	ch = *sInBuf++;
	}
      }

    if (ch == filexfer.kermit.cSndCtlq)
      {
      ch = *sInBuf++;
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

    if (nBytes > nOtLen)
      {
      nBytes -= nRptCnt;
      break;
      }

    while (nRptCnt--)
      *sOtBuf++ = ch;
    }

  *sOtBuf = NULL;

  return (nBytes);
}


/*  C H K 1  --  Compute a type-1 Kermit 6-bit checksum.  */

static int near ComputeChk1(pkt)
  char *pkt;
{
  unsigned int i;

  i = ComputeChk2(pkt);
  i = (((i & 0300) >> 6) + i) & 077;

  return(i);
}

/*  C H K 2  --  Compute the numeric sum of all the bytes in the packet.  */

static int near ComputeChk2(pkt)
  char *pkt;
{
  unsigned int i;

  for (i = 0; *pkt != NULL; i += *pkt++);

  return(i);
}


/*  C H K 3  --  Compute a type-3 Kermit block check.  */
/*
 Calculate the 16-bit CRC of a null-terminated string using a byte-oriented
 tableless algorithm invented by Andy Lowry (Columbia University).  The
 magic number 010201 is derived from the CRC-CCITT polynomial x^16+x^12+x^5+1.
 Note - this function could be adapted for strings containing imbedded 0's
 by including a length argument.
*/
static int near ComputeChk3(s)
  char *s;
{
  register unsigned int i, crc;

  /* Compute CRC */

  for (crc = 0; *s; s++)
    {
    i = (crc ^ *s) & 0x0F;
    crc = (crc >> 4) ^ (i * 010201);
    i = (crc ^ (*s >> 4)) & 0x0F;
    crc = (crc >> 4) ^ (i * 010201);
    }

  return ((int)crc);
}


USHORT KermitOpen(char *pName, BOOL fWrite)
  {
  USHORT usAction;
  register USHORT usResults;


  if (pName)
    {
    USHORT fsOpenFlags;

    if (fWrite)
      fsOpenFlags = FILE_OPEN | FILE_CREATE;
    else
      fsOpenFlags = FILE_OPEN;

    if (usResults = DosOpen(pName, &filexfer.kermit.hFile, &usAction, 0L,
			  FILE_NORMAL, fsOpenFlags, OPEN_ACCESS_READWRITE, 0L))
      return (XlatError(usResults));
    }

  filexfer.kermit.fWrite = fWrite;
  filexfer.kermit.fEof	 = FALSE;
  filexfer.kermit.iBuf = 0;

  return (0);
  }


USHORT KermitClose(void)
  {
  USHORT usBytes;
  register USHORT usResults;


  if (filexfer.kermit.hFile == -1)
    return (0);

  if (filexfer.kermit.fWrite)
    {
    usResults = DosWrite(filexfer.kermit.hFile, filexfer.kermit.abBuf,
			     MAX_BUFFER - filexfer.kermit.iBuf, &usBytes);
    if (usResults || MAX_BUFFER - filexfer.kermit.iBuf != usBytes)
      {
      DosClose(filexfer.kermit.hFile);
      filexfer.kermit.hFile = -1;
      filexfer.kermit.fEof  = TRUE;

      if (usResults)
	return(XlatError(usResults));
      else
	return(ERRDOS_DISKFULL);
      }
    }

  usResults = DosClose(filexfer.kermit.hFile);
  filexfer.kermit.hFile = -1;

  return (usResults);
  }


SHORT KermitFlush(short sChar)
  {
  USHORT usBytes;
  register USHORT usResults;


  if (filexfer.kermit.hFile == -1)
    return (-1);

  if (usResults = DosWrite(filexfer.kermit.hFile, filexfer.kermit.abBuf,
			   MAX_BUFFER, &usBytes))
    return (-1);
  else if (MAX_BUFFER != usBytes)
    return (-1);

  filexfer.kermit.iBuf = MAX_BUFFER - 1;
  filexfer.kermit.abBuf[0] = (BYTE)sChar;
  filexfer.kermit.pBuf = &filexfer.kermit.abBuf[1];

  return (0);
  }


SHORT KermitFill(void)
  {
  USHORT usBytes;
  register USHORT usResults;


  if (filexfer.kermit.hFile == -1)
    return (-1);

  if (usResults = DosRead(filexfer.kermit.hFile, filexfer.kermit.abBuf,
			  MAX_BUFFER, &usBytes))
    {
    filexfer.kermit.fEof = TRUE;
    return (-1);
    }

  if (usBytes == 0)
    {
    filexfer.kermit.fEof = TRUE;
    return (-1);
    }

  filexfer.kermit.iBuf = usBytes - 1;
  filexfer.kermit.pBuf = &filexfer.kermit.abBuf[1];

  return ((SHORT)filexfer.kermit.abBuf[0]);
  }


SHORT KermitUnget(short sChar)
  {
  filexfer.kermit.iBuf++;
  filexfer.kermit.pBuf--;

  return (*filexfer.kermit.pBuf = (char)sChar);
  }
