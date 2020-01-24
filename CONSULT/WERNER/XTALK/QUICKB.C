/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   QUICKB File Transfer (QUICKB.C)                                 *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains supports QUICKB with some QUICKB+ options. *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/11/89 00012 QuickB uses only 512 byte blocks with VT102 *
*                                   emulation.                                *
*             1.01 08/15/89 00014 Permit user to type in path to CIS prompt.  *
*             1.01 09/05/89 00024 Previous xfer info still in the dialog.     *
*	      1.01 10/26/89 00050 Prevent buffer overflow to prevent lockups. *
*	      1.01 12/18/89 00068 Correct problem with different block sizes. *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00027 JDB
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "term.h"
#include "prodata.h"
#include "xfer.h"
#include "dialogs.h"
#include "variable.h"


#define STATE_IDLE      0x00
#define STATE_DOWNLOAD  0x01
#define STATE_UPLOAD    0x02
#define STATE_DONE      0x03


extern BOOL DlgQuickB(HWND, unsigned, WORD, LONG);


static char near RcvBuild();

static void near ProcessTransportPacket();
static void near ProcessApplicationPacket();
static void near ProcessFailurePacket();
static void near ProcessTransferPacket();
static void near ProcessDataPacket();

static void near GenerateDataPacket();

static void near SendTransportPacket();
static void near SendQuickPacket(BYTE, BYTE *, int);
static void near SendLastPacket();
static void near SendQuotedPacket(BYTE *, int);

static void near SendAck(BYTE);
static void near SendPlusAck(BYTE);
static void near SendNak(void);
static void near SendFailurePacket(BYTE);
static WORD near ComputePacketCRC(BYTE * pBuf, int cBuf);
static WORD near ComputePacketChecksum(BYTE * pBuf, int cBuf);
static void near SetDataQuoting(int);

static unsigned CSBFileCRC(LPSTR,LONG *);	//dca00027 JDB
extern unsigned short crctab[256];		//dca00027 JDB
USHORT ResumeXferFile(char *, int *);		//dca00027 JDB
extern int FAR PASCAL _lopen(LPSTR,int);	//dca00027 JDB
extern int FAR PASCAL _llseek(int,LONG,int);	//dca00027 JDB
//extern int FAR PASCAL _lclose(int);		//dca00027 JDB
extern int FAR PASCAL _lread(int,LPSTR,int);	//dca00027 JDB

int XferQuickBSendName(char *szName)
  {
  int	   hFile;
  OFSTRUCT OfStruct;
  register i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);

  wXferTotalFiles  = 0;
  dwXferTotalBytes = 0L;

  if (szName == NULL || *szName == NULL)
    {
    if ((hFile = XferUploadFile("\\*.*", &OfStruct)) <= 0)
      return (ERRSYS_OPENFAULT);
    strcpy(szXferFilePath, StripPath(OfStruct.szPathName));
    }
  else
    {
    if ((hFile = OpenXferFile(szName)) == -1)
      return (ERRSYS_OPENFAULT);
    strcpy(szXferFilePath, szName);
    }

  return (XferQuickBStart(hFile));
  }


int XferQuickBRecvName(char *szName)
  {
  register i;

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);

  return (XferQuickBStart(-1));
  }


int XferQuickBStart(int hFile)
{
  DCB sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.qb, 0, sizeof(filexfer.qb));

  filexfer.qb.cSeqNo = '0';
  filexfer.qb.ws    = 0;
  filexfer.qb.wr    = 0;
  filexfer.qb.bs    = 4;
  filexfer.qb.cm    = 0;
  filexfer.qb.dq    = 1;

  if (hFile == -1)
    szXferFilePath[0] = NULL;

  GetCommState(nCommID, (DCB FAR *)&sDCB);
  if (Comm.cDataBits < 8)
    {
    if (Comm.cDataBits < 8)
      {
      sDCB.ByteSize = 8;
      sDCB.Parity   = NOPARITY;
      sDCB.StopBits = ONESTOPBIT;
      }
    SetCommState((DCB FAR *)&sDCB);
    fXferChangedComm = TRUE;
    }
  else
    fXferChangedComm = FALSE;

  wXferTotalFiles  = 0;     // 00024 Clear the previous file count

  if (hFile == -1)
    fXferDirection = FALSE;
  else
    fXferDirection = TRUE;

  filexfer.qb.cState  = STATE_IDLE;
  filexfer.qb.cSeqNo  = 1;
  filexfer.qb.iData   = 0;
  filexfer.qb.file    = hFile;

  nXferBlockSize = 1024;
  lXferFileSize  = 0L;

  lpXferDlg = MakeProcInstance((FARPROC)DlgQuickB, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}

void XferQuickBTerm(int nError)
{
  if ((wSystem & SS_XFER) == 0)
    return;

  nErrorCode = nError;

  ResetSystemStatus(SS_XFER);

  /* if (filexfer.qb.file != -1) dca00069 JDB */
    DosClose(filexfer.qb.file);
  /* if (nErrorCode && szXferFilePath[0] && !fXferDirection)
    DosDelete(szXferFilePath, 0L); dca00069 JDB */

  if (hXferDlg)
    {
    DestroyWindow(hXferDlg);
    UnLinkDialog(hXferDlg);
    hXferDlg = NULL;
    }

  if (lpXferDlg)
    {
    FreeProcInstance(lpXferDlg);
    lpXferDlg = NULL;
    }

  if (fXferChangedComm)
    CommPortUpdate();

  IconTextChanged(NULL);
  InfoDisplay(NULL);

  if (!IfSystemStatus(SS_SCRIPT))
    {
    MessageBeep(0);
    if (nErrorCode)
      GeneralError(nErrorCode);
    else
      {
      if (IsIconic(hWnd))
	InfoMessage(MSGXFR_ALLDONE);
      }
    }
}


BOOL DlgQuickB(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char	buf[32];
  char	cType;


  switch (message)
    {
    case WM_INITDIALOG:
      nErrorCode = 0;

      hXferDlg = hDlg;

      switch (Xfer.cTiming)
	{
	case XFER_TIMING_TIGHT:
          filexfer.qb.nTimeOut = 7000;
	  break;

	case XFER_TIMING_NORMAL:
          filexfer.qb.nTimeOut = 10000;
	  break;

	case XFER_TIMING_LOOSE:
          filexfer.qb.nTimeOut = 15000;
	  break;

	default:
          filexfer.qb.nTimeOut = 30000;
	  break;
	}

      SetDataQuoting(0);

      if (Term.cTerminal == TERM_VIDTEX)
        {
        SendAck(1);
        SendTransportPacket();
        }
      else
        {
        SendPlusAck(0);  // 00012  Make sure we send QuickB start packet
        }

      Xfer.cProtocol = XFER_CSERVEB;
      InitXferIndicators();
      InfoDisplayMsg(MSGXFR_QUICKBACT);
      UpdateXferIndicators();
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_CANCEL:
	  nErrorCode = ERRXFR_LOCALCANCEL;
          SendFailurePacket('A');
          filexfer.qb.cState = STATE_DONE;
	  break;

	case ID_HIDE:
	  HideFileTransfer();
	  break;

	case ID_XFER_GRAPHICS:
	  if (HIWORD(lParam) == BN_PAINT)
	    PaintXferGraphics(LOWORD(lParam));
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_TIMER:
      if (!bConnected)
	{
        XferQuickBTerm(ERRXFR_LOSTCONNECT);
	break;
	}

      if (filexfer.qb.bWait)
	{
	if (ComStat.cbOutQue == 0)
	  {
          filexfer.qb.bWait = FALSE;
          filexfer.qb.lTimeOut = GetCurrentTime() +
                                     (long)filexfer.qb.nTimeOut;
	  }
	else
	  break;
	}

      if (filexfer.qb.cState == STATE_DONE)
        {
        if (ComStat.cbInQue + CommSize() != NULL)
          {
          RcvBuild();
          break;
          }

        XferQuickBTerm(nErrorCode);
        break;
        }

      if (ComStat.cbInQue + CommSize() != NULL)
	{
        cType = RcvBuild();

        filexfer.qb.lTimeOut = GetCurrentTime() + filexfer.qb.nTimeOut;
	}
      else if (GetCurrentTime() >= filexfer.qb.lTimeOut)
	{
        cType = 'X';
	}
      else
	cType = -1;

      if (cType=='X' || cType=='Q' || cType=='S' || cType=='R')
	{
        if (cType != 'X' || lXferFileBytes != 0L)
	  wXferTotalErrors++;
	wXferConsecErrors++;
	switch (cType)
	  {
          case 'X':
	    wXferLastError = XFER_ERROR_TIMEOUT;
	    break;

	  case 'Q':
	    wXferLastError = XFER_ERROR_CRC;
	    break;

	  case 'N':
	    wXferLastError = XFER_ERROR_NAK;
	    break;

	  case 'R':
	    wXferLastError = XFER_ERROR_DUP;
	    break;

	  default:
	    wXferLastError = XFER_ERROR_SEQ;
	    break;
	  }

	if (Xfer.cMaxErrors != 0 && wXferTotalErrors >= (WORD)Xfer.cMaxErrors)
	  {
	  nErrorCode = ERRXFR_TOOMANYERRORS;
	  cType = 'C';
          }

	if (Xfer.cMaxErrors != 0 && wXferConsecErrors >= (WORD)Xfer.cMaxErrors)
	  {
	  nErrorCode = ERRXFR_TOOMANYERRORS;
	  cType = 'C';
	  }
	}
      else
	{
	if (cType != -1)
	  wXferConsecErrors = 0;
	}

      UpdateXferIndicators();

      if (cType != -1)
        {
        switch (filexfer.qb.cState)
          {
          case STATE_IDLE:
            switch (cType)
              {
              case '+':
                ProcessTransportPacket();
                SendTransportPacket();
                break;

              case '?':
                ProcessApplicationPacket();
                break;

              case 'F':
                ProcessFailurePacket();
                break;

              case 'T':
                ProcessTransferPacket();
                break;

              case 'X':
                SendNak();
                break;
              }
            break;

          case STATE_DOWNLOAD:
            switch (cType)
              {
              case 'N':
                ProcessDataPacket();
                break;

              case 'T':
                ProcessTransferPacket();
                break;

              case 'F':
                ProcessFailurePacket();
                break;

              case 'X':
                SendNak();
                break;
              }
            break;

          case STATE_UPLOAD:
            switch (cType)
              {
              case 'F':
                ProcessFailurePacket();
                break;

              case ASC_ACK:
                GenerateDataPacket();
                break;

              case ASC_NAK:
              case 'X':
                SendLastPacket();
                break;
              }
            break;
          }
	}
      break;

    default:
      return FALSE;
      break;
    }

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void ProcessTransportPacket()                                             */
/*									      */
/*----------------------------------------------------------------------------*/
static void near ProcessTransportPacket()
{
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void ProcessTransferPacket()                                              */
/*									      */
/*----------------------------------------------------------------------------*/
static void near ProcessTransferPacket()
  {
  FILESTATUS fsts;
  OFSTRUCT OfStruct;	//dca00027 JDB Needed for recovery from Tf packet

  switch (filexfer.qb.sData[4])
    {
    case 'D':
      {
      register i;

      for (i = 0; i < sizeof(szXferFilePath)-1; i++)    // 00014 Full Path
        {
        if (filexfer.qb.sData[6+i] != ASC_ETX)
          szXferFilePath[i] = filexfer.qb.sData[6+i];
        else
          break;
        }
      szXferFilePath[i] = NULL;

      //dca00027 JDB Next Line Superseeded by Resume function called below
      //if (nErrorCode = CreateXferFile(szXferFilePath, &filexfer.qb.file))

      //dca00027 JDB Moved this line up from below so resume could set the
      //dca00027 JDB lXferFileButes to the Recovered Length
      lXferFileBytes = 0L; //dca00027 JDB

	//dca00027 JDB This is the resume function call
       if (nErrorCode = ResumeXferFile(szXferFilePath, &filexfer.qb.file))
        {
        SendFailurePacket('C');
        filexfer.qb.cState = STATE_DONE;
        break;
        }

      //dca00027 JDB lXferFileBytes = 0L;
      fXferDirection = FALSE;
      ResetXferIndicators(FALSE);
      filexfer.qb.cState = STATE_DOWNLOAD;
      }
      break;

    case 'U':
      {
      register i;

      if (filexfer.qb.file == -1)
        {
        for (i = 0; i < sizeof(szXferFilePath)-1; i++)  // 00014 Full Path
          {
          if (filexfer.qb.sData[6+i] != ASC_ETX)
            szXferFilePath[i] = filexfer.qb.sData[6+i];
          else
            break;
          }
        szXferFilePath[i] = NULL;

        if ((i = OpenXferFile(szXferFilePath)) <= 0)
          {
          SendFailurePacket('C');
          nErrorCode = ERRSYS_OPENFAULT;
           filexfer.qb.cState = STATE_DONE;
          break;
           }
        filexfer.qb.file = i;
        }

      DosQFileInfo(filexfer.qb.file, 1, &fsts, sizeof(fsts));
      lXferFileSize = fsts.cbFile;

      nXferBlockSize = 1024;

      fXferDirection = TRUE;
      ResetXferIndicators(FALSE);
      GenerateDataPacket();
      filexfer.qb.cState = STATE_UPLOAD;
      }
      break;

    case 'C':
      /* if (filexfer.qb.file != -1) dca00069 JDB */
	DosClose(filexfer.qb.file);
      szXferFilePath[0]    = NULL;
      filexfer.qb.file = -1;

      lXferFileBytes         = 0L;
      ResetXferIndicators(FALSE);

      filexfer.qb.cState = STATE_DONE;
      break;

    case 'I':
      lXferFileSize = atol(&filexfer.qb.sData[7]);
      ResetXferIndicators(FALSE);
      break;

    case 'f':	//dca00027 JDB Failed CRC for upload/download resume
	_lclose(filexfer.qb.file);
	lXferFileBytes=0L;
	 filexfer.qb.file = 
		OpenFile(szXferFilePath, &OfStruct, OF_READWRITE | OF_CREATE);
	break;
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void ProcessApplicationPacket()                                           */
/*									      */
/*----------------------------------------------------------------------------*/
static void near ProcessApplicationPacket()
  {
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void ProcessFailurePacket()                                               */
/*									      */
/*----------------------------------------------------------------------------*/
static void near ProcessFailurePacket()
  {
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void ProcessDataPacket()                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
static void near ProcessDataPacket()
  {
  USHORT usBytes;
  register USHORT usResults;


  nXferBlockSize = filexfer.qb.cData - filexfer.qb.cm - 6;

  usResults = DosWrite(filexfer.qb.file, &filexfer.qb.sData[4], nXferBlockSize,
		       &usBytes);
  if (usResults || nXferBlockSize != usBytes)
    {
    if (usResults)
      nErrorCode = XlatError(usResults);
    else
      nErrorCode = ERRDOS_DISKFULL;
    SendFailurePacket('C');
    filexfer.qb.cState = STATE_DONE;
    }

  lXferFileBytes += (long)nXferBlockSize;
  wXferBlockCount++;
  UpdateXferIndicators();
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  char GenerateDataPacket()                                                 */
/*									      */
/*----------------------------------------------------------------------------*/
static void near GenerateDataPacket()
{

  if (filexfer.qb.file == -1)
    {
    filexfer.qb.cState = STATE_DONE;
    }
  else if (lXferFileBytes >= lXferFileSize)
    {
    char buf[8];

    buf[0] = 'C';
    SendQuickPacket('T', buf, 1);
    DosClose(filexfer.qb.file);
    filexfer.qb.file = -1;
    }
  else
    {
    USHORT usBytes;
    BYTE byBuf[1024];

    // 00068 Correct problem with different block sizes.
    nXferBlockSize = Xfer.nPacketSize;
    if (nXferBlockSize > 1024)
     nXferBlockSize = 1024;
    else if (nXferBlockSize < 128)
     nXferBlockSize = 128;

    if (DosRead(filexfer.qb.file, byBuf, nXferBlockSize, &usBytes))
      {
      nErrorCode = ERRSYS_READFAULT;
      SendFailurePacket('E');
      filexfer.qb.cState = STATE_DONE;
      return;
      }

    SendQuickPacket('N', byBuf, usBytes);
    lXferFileBytes += (long)usBytes;
    wXferBlockCount++;
    }

  UpdateXferIndicators();
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  char RcvBuild()                                                           */
/*									      */
/*----------------------------------------------------------------------------*/
static char near RcvBuild()
  {
  static BOOL bNextIsQuoted;
  static int  nCheckSkipover;
  register i;


  while ((i = CommGet()) != -1)
    {
    switch (i)
      {
      case ASC_ENQ:
        SendAck(filexfer.qb.byLastAck);
        filexfer.qb.iData = 0;
        break;

      case ASC_DLE:
        if (filexfer.qb.iData)
          {
          bNextIsQuoted = TRUE;
          }
        else
          {
          filexfer.qb.sData[0] = ASC_DLE;
          filexfer.qb.cData = 0;
          filexfer.qb.iData = 1;
          bNextIsQuoted  = FALSE;
          nCheckSkipover = 0;
          }
        break;

      case ASC_NAK:
        filexfer.qb.iData = 0;
        return (ASC_NAK);
        break;

      case ASC_ETX:
	// 00050 Prevent buffer overflow and lockups.
	if (filexfer.qb.iData < sizeof(filexfer.qb.sData)-32)
	  filexfer.qb.sData[filexfer.qb.iData++] = (char)i;
	else
	  {
	  SendNak();
	  return ('Q');
	  }
        nCheckSkipover = filexfer.qb.cm + 1;
        break;

      default:
        if (bNextIsQuoted)
          {
	  // 00050 Prevent buffer overflow and lockups.
	  if (filexfer.qb.iData < sizeof(filexfer.qb.sData)-32)
	    filexfer.qb.sData[filexfer.qb.iData++] =
		filexfer.qb.aQuoteToText[i];
	  else
	    {
	    SendNak();
	    return ('Q');
	    }
          bNextIsQuoted = FALSE;
          }
	else
	  {
	  // 00050 Prevent buffer overflow and lockups.
	  if (filexfer.qb.iData < sizeof(filexfer.qb.sData)-32)
	    filexfer.qb.sData[filexfer.qb.iData++] = (char)i;
	  else
	    {
	    SendNak();
	    return ('Q');
	    }
	  }

        if (nCheckSkipover)
          {
          if (nCheckSkipover-- == 1)
            {
            filexfer.qb.cData   = filexfer.qb.iData;
            filexfer.qb.iData = 0;

            if (filexfer.qb.sData[3] == '+' || filexfer.qb.cm == 0)
              {
              WORD wChecksum;
              char buf[96];

              wChecksum = ComputePacketChecksum(&filexfer.qb.sData[2],
                                                filexfer.qb.cData - 3);

              if (filexfer.qb.sData[filexfer.qb.cData-1] != wChecksum)
                {
                SendNak();
                return ('Q');
                }
              }
            else
              {
              WORD wCRC;
              char buf[96];

              wCRC = ComputePacketCRC(&filexfer.qb.sData[2],
                                      filexfer.qb.cData - 4);

              if (filexfer.qb.sData[filexfer.qb.cData-2] != HIBYTE(wCRC) ||
                  filexfer.qb.sData[filexfer.qb.cData-2] != HIBYTE(wCRC))
                {
                SendNak();
                return ('Q');
                }
              }

            if (filexfer.qb.sData[2] - '0' != filexfer.qb.cSeqNo)
              {
              if ((filexfer.qb.sData[2] - '0' + 1) % 10 == filexfer.qb.cSeqNo)
                {
                SendAck(filexfer.qb.sData[2] - '0');
                return ('R');
                }
              else
                return ('S');
              }

	//dca00027 JDB This is damn presumptious.  Not EVERY packed is acked.  The
	//dca00027 JDB TD packet is not acked if Tr is to follow.
	//dca00027 JDB Without a re-write, this is the hacked in fix
            if(filexfer.qb.sData[3] != 'T' || filexfer.qb.sData[4] != 'D')
		    SendAck(filexfer.qb.cSeqNo);

            return (filexfer.qb.sData[3]);
            }
          }
        else if (filexfer.qb.iData == 2)
          {
          if (i == '+')
            {
            filexfer.qb.iData = 0;
            return (ASC_CAN);
            }
          else if (i >= '0' && i <= '9')
            {
            filexfer.qb.iData = 0;
            filexfer.qb.cSeqNo = (i - '0' + 1) % 10;
            return (ASC_ACK);
            }
          }
        break;
      }
    }

  return -1;
  }


static void near SendTransportPacket()
{
  BYTE byBuf[32];


  byBuf[0] = 0;
  byBuf[1] = 1;
  if (Xfer.nPacketSize == 128)
    byBuf[2] = 1;
  else if (Xfer.nPacketSize == 512)
    byBuf[2] = 4;
  else if (Xfer.nPacketSize == 1024)
    byBuf[2] = 8;
  else
    byBuf[2] = 2;

  byBuf[ 3] = 1;
  byBuf[ 4] = 0;
  byBuf[ 5] = 0;
  byBuf[ 6] = 0x14;
  byBuf[ 7] = 0x00;
  byBuf[ 8] = 0xd4;
  byBuf[ 9] = 0x00;
  byBuf[10] = 0;
  byBuf[11] = 0;
  byBuf[12] = 0;
  byBuf[13] = 0;
  //dca00027 JDB byBuf[14] = 0;
  byBuf[14] = 0x02;	//dca00027 JDB Support Tr and Tf packets
  byBuf[15] = 0;
  byBuf[16] = 1;

  SendQuickPacket('+', byBuf, 17);

  filexfer.qb.cm = 1;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SendAck(BYTE bySeq)                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SendAck(BYTE bySeq)
  {
  filexfer.qb.byLastAck = bySeq;

  filexfer.qb.sData[0] = ASC_DLE;
  filexfer.qb.sData[1] = '0' + bySeq;
  CommDataOutput(filexfer.qb.sData, 2);
  filexfer.qb.cSeqNo = (bySeq + 1) % 10;
  filexfer.qb.bWait  = TRUE;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SendPlusAck(BYTE bySeq)                                              */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SendPlusAck(BYTE bySeq)
  {
  filexfer.qb.byLastAck = bySeq;

  filexfer.qb.sData[0] = ASC_DLE;
  filexfer.qb.sData[1] = '+';
  filexfer.qb.sData[2] = ASC_DLE;
  filexfer.qb.sData[3] = '0' + bySeq;
  CommDataOutput(filexfer.qb.sData, 4);
  filexfer.qb.cSeqNo = (bySeq + 1) % 10;
  filexfer.qb.bWait  = TRUE;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SendNak(void)                                                        */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SendNak(void)
  {
  filexfer.qb.sData[0] = ASC_NAK;
  CommDataOutput(filexfer.qb.sData, 1);
  filexfer.qb.bWait = TRUE;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SendFailurePacket(BYTE byCode)                                       */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SendFailurePacket(BYTE byCode)
  {
  BYTE byBuf[8];

  byBuf[0] = byCode;
  SendQuickPacket('F', byBuf, 1);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SendQuickPacket(BYTE byTyp, BYTE *pBuf, int cBuf)                    */
/*									      */
/*  Send a standard Quick B packet, nothing fancy here.                       */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SendQuickPacket(BYTE byTyp, BYTE *pBuf, int cBuf)
  {
  register i;


  filexfer.qb.sData[0] = ASC_DLE;
  filexfer.qb.sData[1] = 'B';

  filexfer.qb.sData[2] = '0' + filexfer.qb.cSeqNo;
  filexfer.qb.sData[3] = byTyp;

  for (i = 4; cBuf; cBuf--)
    filexfer.qb.sData[i++] = *pBuf++;

  filexfer.qb.sData[i++] = ASC_ETX;
  if (filexfer.qb.cm)
    {
    WORD wCrc;

    wCrc = ComputePacketCRC(&filexfer.qb.sData[2], i-2);
    filexfer.qb.sData[i++] = HIBYTE(wCrc);
    filexfer.qb.sData[i++] = LOBYTE(wCrc);
    }
  else
    {
    WORD wChecksum;
    wChecksum = ComputePacketChecksum(&filexfer.qb.sData[2], i-2);
    filexfer.qb.sData[i++] = LOBYTE(wChecksum);
    }

  SendQuotedPacket(filexfer.qb.sData, filexfer.qb.cData = i);
  filexfer.qb.lTimeOut = GetCurrentTime() + filexfer.qb.nTimeOut;
  }

static void near SendLastPacket()
  {
  SendQuotedPacket(filexfer.qb.sData, filexfer.qb.cData);
  }

static void near SendQuotedPacket(BYTE *pBuf, int cBuf)
  {
  BYTE buf[256];
  int  nEtx;
  register i, j;


  if (filexfer.qb.cm)
    nEtx = cBuf - 3;
  else
    nEtx = cBuf - 2;

  for (i = 0, j = 0; j < cBuf; j++)
    {
    if (i > sizeof(buf)-4)
      {
      CommDataOutput(buf, i);
      i = 0;
      }

    if (j > 1 && j != nEtx && filexfer.qb.aTextToQuote[*pBuf])
      {
      buf[i++] = ASC_DLE;
      buf[i++] = filexfer.qb.aTextToQuote[*pBuf];
      }
    else
      {
      buf[i++] = *pBuf;
      }

    pBuf++;
    }

  if (i)
    CommDataOutput(buf, i);
  filexfer.qb.bWait = TRUE;
  }

/*----------------------------------------------------------------------------*/
/*									      */
/*  void SetDataQuoting(int nLevel)                                           */
/*									      */
/*  Due to the really weird patterns that Quick B uses to quote characters    */
/*  and to get ready for Quick + B support, we use a simple table of nulls    */
/*  or the quoted values.  Thus is the table entry for a character is null    */
/*  then we don't quote it, else we use 'DLE' followed by the entry in the    */
/*  table.  Since we use temporary space for the table, this routine will     */
/*  generate the corrected table based on the specified level.                */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SetDataQuoting(int nLevel)
  {
  register i;

  memset(filexfer.qb.aTextToQuote, NULL,
         sizeof(filexfer.qb.aTextToQuote));
  memset(filexfer.qb.aQuoteToText, NULL,
         sizeof(filexfer.qb.aQuoteToText));

  switch (nLevel)
    {
    case 0:
      filexfer.qb.aTextToQuote[ASC_NUL] = 0x40 + ASC_NUL;
      filexfer.qb.aTextToQuote[ASC_ETX] = 0x40 + ASC_ETX;
      filexfer.qb.aTextToQuote[ASC_ENQ] = 0x40 + ASC_ENQ;
      filexfer.qb.aTextToQuote[ASC_DLE] = 0x40 + ASC_DLE;
      filexfer.qb.aTextToQuote[ASC_DC1] = 0x40 + ASC_DC1;
      filexfer.qb.aTextToQuote[ASC_DC3] = 0x40 + ASC_DC3;
      filexfer.qb.aTextToQuote[ASC_NAK] = 0x40 + ASC_NAK;
      break;

    case 1:
      filexfer.qb.aTextToQuote[ASC_ETX] = 0x40 + ASC_ETX;
      filexfer.qb.aTextToQuote[ASC_ENQ] = 0x40 + ASC_ENQ;
      filexfer.qb.aTextToQuote[ASC_DLE] = 0x40 + ASC_DLE;
      filexfer.qb.aTextToQuote[ASC_DC1] = 0x40 + ASC_DC1;
      filexfer.qb.aTextToQuote[ASC_DC3] = 0x40 + ASC_DC3;
      filexfer.qb.aTextToQuote[ASC_NAK] = 0x40 + ASC_NAK;
      break;

    case 2:
      filexfer.qb.aTextToQuote[ASC_ETX] = 0x40 + ASC_ETX;
      filexfer.qb.aTextToQuote[ASC_ENQ] = 0x40 + ASC_ENQ;
      filexfer.qb.aTextToQuote[ASC_DLE] = 0x40 + ASC_DLE;
      filexfer.qb.aTextToQuote[ASC_DC1] = 0x40 + ASC_DC1;
      filexfer.qb.aTextToQuote[ASC_DC3] = 0x40 + ASC_DC3;
      filexfer.qb.aTextToQuote[ASC_NAK] = 0x40 + ASC_NAK;
      filexfer.qb.aTextToQuote[0x80+ASC_ETX] = 0x60 + ASC_ETX;
      filexfer.qb.aTextToQuote[0x80+ASC_ENQ] = 0x60 + ASC_ENQ;
      filexfer.qb.aTextToQuote[0x80+ASC_DLE] = 0x60 + ASC_DLE;
      filexfer.qb.aTextToQuote[0x80+ASC_DC1] = 0x60 + ASC_DC1;
      filexfer.qb.aTextToQuote[0x80+ASC_DC3] = 0x60 + ASC_DC3;
      filexfer.qb.aTextToQuote[0x80+ASC_NAK] = 0x60 + ASC_NAK;
      filexfer.qb.aTextToQuote[0x7F] = 0x2F;
      filexfer.qb.aTextToQuote[0xFF] = 0x2E;
      break;

    case 3:
      for (i = 0; i < 0x20; i++)
        {
        filexfer.qb.aTextToQuote[i] = 0x40 + i;
        filexfer.qb.aTextToQuote[0x80+i] = 0x60 + i;
        }
      filexfer.qb.aTextToQuote[0x7F] = 0x2F;
      filexfer.qb.aTextToQuote[0xFF] = 0x2E;
      filexfer.qb.aTextToQuote[0x1F] = 0x2D;
      break;
    }

  for (i = 0; i < 0x20; i++)
    {
    filexfer.qb.aQuoteToText[i+0x40] = i;
    filexfer.qb.aQuoteToText[i+0x60] = 0x80 + i;
    }
  filexfer.qb.aQuoteToText[0x2D] = 0x1F;
  filexfer.qb.aQuoteToText[0x2E] = 0xFF;
  filexfer.qb.aQuoteToText[0x2F] = 0x7F;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  WORD ComputePacketChecksum(BYTE * pBuf, int cBuf)                         */
/*									      */
/*  Compute a Checksum.  Quick B must have the strangest checksum that I have */
/*  ever seen.   Looks like it assume that it would always be in assembler,   */
/*  we only code in the magic of C.                                           */
/*									      */
/*----------------------------------------------------------------------------*/
static WORD near ComputePacketChecksum(BYTE * pBuf, int cBuf)
{
  WORD checksum;

  checksum = 0;
  while (cBuf--)
    {
    checksum = checksum << 1;
    if (checksum > 0xff)
      checksum = (checksum & 0xff) + 1;
    checksum += *pBuf++ & 0xff;
    if (checksum > 0xff)
      checksum = (checksum & 0xff) + 1;
    }

  return (checksum);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  WORD ComputePacketCRC(BYTE * pBuf, int cBuf)                              */
/*									      */
/*  Compute a CRC.  Quick B is a little different from the rest as it starts  */
/*  the CRC total at -1.                                                      */
/*									      */
/*----------------------------------------------------------------------------*/
static WORD near ComputePacketCRC(BYTE * pBuf, int cBuf)
{
  unsigned crc;
  register i;

  crc = -1;
  while (cBuf--)
    {
    crc = crc ^ (int)(*(pBuf++)) << 8;
    for (i = 0; i < 8; ++i)
      {
      if (crc & 0x8000)
	crc = crc << 1 ^ 0x1021;
      else
	crc = crc << 1;
      }
    }

  return (crc);
}

//dca00027 JDB From here on ...
/*---------------------------------------------------------------------------*/
/* This function was copied from the CreateXferFile function in xferlo and   */
/* modified to perform the compuserv b+ download file recovery               */
/* JDB */
/*---------------------------------------------------------------------------*/
extern BOOL fBackups;

USHORT ResumeXferFile(char *szName, int *pfh)
{
OFSTRUCT OfStruct;
char	   szPath[MAX_PATH];
char	   szBackupName[MAX_PATH];
char	   *ptr;
register i;
BOOL FileExists;
unsigned crc;
long length;
char name[80];
BYTE packet[32];
int plen;

	/* Check for existance of file */
	if(OpenFile(szName, &OfStruct, OF_EXIST) != -1)
		FileExists = TRUE;
	else
		FileExists = FALSE;

	if(FileExists == TRUE)
	{
		/* Make The Backup of needed */
		if (fBackups)
		{
			strcpy(szBackupName, szName);
			if ((ptr = strchr(szBackupName, '.')) == NULL)
			ptr = &szBackupName[strlen(szBackupName)];
			*ptr++ = '.';
			*ptr++ = 'B';
			*ptr++ = 'A';
			*ptr++ = 'K';
			*ptr   = NULL;
			DosCopy(szName, szBackupName);
		}

		/* Adjust the packet number */
		filexfer.qb.cSeqNo = (filexfer.qb.cSeqNo + 1) % 10;

		/* The file exists so try a recovery */
		crc=CSBFileCRC(szName,&length);

		/* Construct The Packet For Recovery */
		plen = sprintf(packet,"r%ld %u ",length,crc);

		/* Open the file */
		if ((*pfh = OpenFile(szName, &OfStruct,
				OF_READWRITE)) == -1)
			return (XlatError(OfStruct.nErrCode));

		/* The file is open so seek to end */
		_llseek(*pfh,0,2);	/* End of file */

		/* and set the transfer indicator to match */
		lXferFileBytes=length;

		/* Send the resume packet */
		SendQuickPacket('T',packet,plen);
		return(0);

	}
	else
	{
		/* It is a download of a file that does not exist */
                SendAck(filexfer.qb.cSeqNo);

		/* if it did not exist then create it */
		if ((*pfh = OpenFile(szName, &OfStruct,
				OF_READWRITE | OF_CREATE)) == -1)
			return (XlatError(OfStruct.nErrCode));
		else
			return (0);
	}
}

/* dca00027 JDB crc table for quickb crc calculation */
extern unsigned short crctab[256];
/* 
dca00027 JDB Modified Zmodem.c to make this table (already in zmodem) public
static unsigned short crctab[256] =
{
    0x0000,  0x1021,  0x2042,  0x3063,	0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,	0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x0210,  0x3273,  0x2252,	0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,	0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x0420,  0x1401,	0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,	0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x0630,	0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,	0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,	0x0840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,	0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,	0x1a71,  0x0a50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,	0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,	0x2c22,  0x3c03,  0x0c60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,	0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,	0x3e13,  0x2e32,  0x1e51,  0x0e70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,	0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,	0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0x00a1,  0x30c2,  0x20e3,	0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,	0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x02b1,  0x1290,  0x22f3,  0x32d2,	0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,	0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x0481,	0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,	0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x0691,  0x16b0,	0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,	0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,	0x18c0,  0x08e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,	0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,	0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,	0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,	0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,	0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,	0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};
*/

unsigned CSBFileCRC(lpszFileName,pLength)
LPSTR lpszFileName;
long *pLength;
{
int file_handle;
char lpszDataBuffer[512];	// Optimum for read/write
long length;
unsigned crc;
unsigned int data_byte;
int i,nr;

	/* start out */
	length = 0;
	crc = 0xffff;

	/* Open the file */
	file_handle = _lopen(lpszFileName,0);	// Read Only

	do
	{
		nr = _lread(file_handle,lpszDataBuffer,sizeof(lpszDataBuffer));

		/* update the length */
		length+=(long)nr;

		/* do the crc */
		for(i=0; i<nr; ++i)
		{
			data_byte = lpszDataBuffer[i];
			crc = crctab[(crc>>8 ^ data_byte)&0xff]^(crc<<8);
		}
	}
	while(nr>0);

	/* close the file and exit */
	_lclose(file_handle);

	/* and leave */
	*pLength = length;

	return(crc);
}
