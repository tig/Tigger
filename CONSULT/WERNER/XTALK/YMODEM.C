/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   YMODEM File Transfer (YMODEM.C)                                 *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/15/89 00015 Prevent the first error when downloading    *
*                                   with XModem or YModem.                    *
*             1.01 08/15/89 00016 Prevent the final (and normal) Nak from     *
*                                   displaying in the dialog box.             *
*             1.01 09/05/89 00024 Previous xfer info still in the dialog.     *
*             1.1  04/19/90 dca00088 PJL Fix PacketSize display problem.      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "term.h"
#include "prodata.h"
#include "xfer.h"
#include "dialogs.h"
#include "variable.h"

#define SND_FILE	0x01
#define SND_ACKFILE	0x02
#define SND_DATA	0x03
#define SND_EOF 	0x04
#define SND_IDLE	0x05

#define RCV_FILE	0x06
#define RCV_START	0x07
#define RCV_DATA	0x08
#define RCV_EOF 	0x09
#define RCV_IDLE	0x0B

#define XFR_TERM	0x0C


extern BOOL DlgYModem(HWND, unsigned, WORD, LONG);

static char near RcvBuild();
static char near SndBuild();

static void near RcvStart();
static void near RcvFile();
static void near RcvData();
static void near RcvEOF();
static void near RcvAck();
static void near RcvNak();
static void near RcvCancel();

static void near SndFile();
static void near SndData();
static void near SndEOF();
static void near SndLast();
static void near SndCancel();

static BOOL near VerifyBlk(char *, int);
static void near ComputeCRC(char *, int);
static void near SendAbortString(void);


int XferYModemSendName(filename)
  char *filename;
{
  int	   hFile;
  FILESTATUS fsts;
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.ym, 0, sizeof(filexfer.ym));

  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendAbortString();
    return (i);
    }

  if (filename == NULL || *filename == NULL)
    {
    if (XferUploadBatch("\\*.*", szXferFilePath))
      {
      SendAbortString();
      return (ERRSYS_OPENFAULT);
      }
    }
  else
    strcpy(szXferFilePath, filename);

  if (ComputeFilesToSend(szXferFilePath, &filexfer.ym.sendfile) == 0)
    {
    SendAbortString();
    return (ERRXFR_NOFILEFOUND);
    }

  if ((hFile = OpenXferFile(filexfer.ym.sendfile.name)) == -1)
    {
    SendAbortString();
    return (ERRSYS_OPENFAULT);
    }

  strcpy(szXferFilePath, filexfer.ym.sendfile.name);

  GetCommState(nCommID, (DCB FAR *)&sDCB);
  if (Comm.cDataBits < 8 || sDCB.fInX || sDCB.fOutX)
    {
    if (Comm.cDataBits < 8)
      {
      sDCB.ByteSize = 8;
      sDCB.Parity   = NOPARITY;
      sDCB.StopBits = ONESTOPBIT;
      }
    sDCB.fInX	    = FALSE;
    sDCB.fOutX	    = FALSE;
    SetCommState((DCB FAR *)&sDCB);
    fXferChangedComm = TRUE;
    }
  else
    fXferChangedComm = FALSE;

  fXferDirection	 = TRUE;
  filexfer.ym.cState = SND_FILE;
  filexfer.ym.cSeqNo = 0;
  filexfer.ym.file   = hFile;

  DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;

  // dca00088 PJL Fix PacketSize display problem.
  nXferBlockSize = Xfer.nPacketSize = 1024;

  lpXferDlg = MakeProcInstance((FARPROC)DlgYModem, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}


int XferYModemRecvName(filename)
  char *filename;
{
  int	   hFile;
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendAbortString();
    return (i);
    }

  if (filename == NULL || *filename == NULL)
    {
    hFile = -1;
    szXferFilePath[0] = NULL;
    }
  else
    {
    if (nErrorCode = CreateXferFile(filename, &hFile))
      {
      SendAbortString();
      return(nErrorCode);
      }
    strcpy(szXferFilePath, StripPath(filename));
    }

  memset(&filexfer.ym, 0, sizeof(filexfer.ym));

  GetCommState(nCommID, (DCB FAR *)&sDCB);
  if (Comm.cDataBits < 8 || sDCB.fInX || sDCB.fOutX)
    {
    if (Comm.cDataBits < 8)
      {
      sDCB.ByteSize = 8;
      sDCB.Parity   = NOPARITY;
      sDCB.StopBits = ONESTOPBIT;
      }
    sDCB.fInX	    = FALSE;
    sDCB.fOutX	    = FALSE;
    SetCommState((DCB FAR *)&sDCB);
    fXferChangedComm = TRUE;
    }
  else
    fXferChangedComm = FALSE;

  wXferTotalFiles  = 0;     // 00024 Clear the previous file count
  fXferDirection	  = FALSE;
  filexfer.ym.cState  = RCV_FILE;
  filexfer.ym.cSeqNo  = 0;
  filexfer.ym.nChrCnt = 0;
  filexfer.ym.file    = hFile;

  // dca00088 PJL Fix PacketSize display problem.
  nXferBlockSize = Xfer.nPacketSize = 1024;
  lXferFileSize  = 0L;

  lpXferDlg = MakeProcInstance((FARPROC)DlgYModem, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}

void XferYModemTerm()
{
  register i;

  if ((wSystem & SS_XFER) == 0)
    return;

  i = nErrorCode;
  ResetSystemStatus(SS_XFER);

  if (filexfer.ym.file != -1)
    DosClose(filexfer.ym.file);
  if (nErrorCode && szXferFilePath[0] && !fXferDirection)
    DosDelete(szXferFilePath, 0L);

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
    nErrorCode = i;
    if (nErrorCode)
      GeneralError(nErrorCode);
    else
      {
      if (IsIconic(hWnd))
	InfoMessage(MSGXFR_ALLDONE);
      }
    }
}


BOOL DlgYModem(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char	cType;


  switch (message)
    {
    case WM_INITDIALOG:
      nErrorCode = 0;

      hXferDlg = hDlg;

      switch (Xfer.cTiming)
	{
	case XFER_TIMING_TIGHT:
          filexfer.ym.nTimeOut = 4000;
	  break;

	case XFER_TIMING_NORMAL:
          filexfer.ym.nTimeOut = 10000;
	  break;

	case XFER_TIMING_LOOSE:
          filexfer.ym.nTimeOut = 15000;
	  break;

	default:
          filexfer.ym.nTimeOut = 30000;
	  break;
	}

      InitXferIndicators();

      if (fXferDirection)
	{
	InfoDisplayMsg(MSGXFR_YMODEMSND);
        filexfer.ym.lTimeOut = GetCurrentTime() + filexfer.ym.nTimeOut;
	}
      else
	{
	InfoDisplayMsg(MSGXFR_YMODEMRCV);
        filexfer.ym.lTimeOut = GetCurrentTime() + 500;
	}

      UpdateXferIndicators();

      CommFlushBuffers();
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_CANCEL:
	  nErrorCode = ERRXFR_LOCALCANCEL;
	  if (filexfer.ym.cWait == XFER_RCV_WAIT)
	    {
	    RcvCancel();
	    XferYModemTerm();
	    }
	  else if (fXferDirection)
	    {
            filexfer.ym.cWait = XFER_RCV_WAIT;
	    nXferRcvWait = 8;
	    }
	  else
	    {
            if (filexfer.ym.cEOTCnt)
	      RcvEOF();
            filexfer.ym.cWait = XFER_RCV_WAIT;
	    nXferRcvWait = 8;
	    }
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
	nErrorCode = ERRXFR_LOSTCONNECT;
	XferYModemTerm();
	break;
	}

      if (filexfer.ym.cWait == XFER_SND_WAIT)
	{
	if (ComStat.cbOutQue == 0)
	  {
          filexfer.ym.cWait = XFER_NUL_WAIT;
	  filexfer.ym.lTimeOut = GetCurrentTime() +
				     (long)filexfer.ym.nTimeOut;
	  }
	else
	  break;
	}
      else if (filexfer.ym.cWait == XFER_RCV_WAIT)
	{
        if (ComStat.cbInQue + CommSize() != 0)
	  RcvBuild();
	else
	  nXferRcvWait--;
	if (nXferRcvWait <= 0)
	  {
	  RcvCancel();
	  break;
          }
        break;
	}

      if (filexfer.ym.cState == XFR_TERM)
	{
	cType = 'Z';
	}
      else if (ComStat.cbInQue + CommSize() != NULL)
	{
	if (fXferDirection)
	  cType = SndBuild();
	else
	  cType = RcvBuild();

        filexfer.ym.lTimeOut = GetCurrentTime() + filexfer.ym.nTimeOut;
	}
      else if (GetCurrentTime() >= filexfer.ym.lTimeOut)
	{
        cType = 'T';
	}
      else
	cType = -1;

      if (cType == 'D')
	{
        if (filexfer.ym.sData[1] == filexfer.ym.cSeqNo - 1)
	  {
	  if (lXferFileBytes == 0L)
	    cType = 'K';
	  else
	    {
	    cType = 'R';
	    }
	  }
        else if (filexfer.ym.sData[1] != filexfer.ym.cSeqNo)
	  {
	  cType = 'C';
	  nErrorCode = ERRXFR_SEQERROR;
	  }
	}

      if (cType=='T' || cType=='Q' || cType=='N' || cType=='S' || cType=='R')
	{
        if (cType != 'T' || filexfer.ym.cSeqNo != 0 || lXferFileBytes != 0L)
	  wXferTotalErrors++;

        if (cType != 'N' || filexfer.xmodem.cEOTCnt == 0)  // 00016 Last Nak
          {
          if (filexfer.xmodem.fFirstNak)  // 00015 Prevent the first error
            {
            if (wXferConsecErrors++ > 5 && Xfer.cTiming == XFER_TIMING_TIGHT)
              filexfer.ym.nTimeOut = 10000;
            }
          else
            filexfer.xmodem.fFirstNak = TRUE;
          }

	switch (cType)
	  {
	  case 'T':
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
        switch (filexfer.ym.cState)
	  {
	  case RCV_FILE:
	    switch (cType)
	      {
              case 'D':
		RcvFile();
		break;

              case 'T':
		RcvStart();
		break;

	      case 'Q':
		RcvNak();
		break;

	      case 'K':
              case 'R':
                RcvAck();
                break;

	      case 'C':
		RcvCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		RcvCancel();
		break;
	      }
	    break;

	  case RCV_START:
	    switch (cType)
	      {
              case 'D':
		RcvData();
                filexfer.ym.cState = RCV_DATA;
		break;

              case 'T':
		RcvStart();
		break;

	      case 'K':
	      case 'R':
		RcvAck();
		break;

              case 'Q':
		RcvNak();
		break;

	      case 'C':
		RcvCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		RcvCancel();
		break;
	      }
	    break;

	  case RCV_DATA:
	    switch (cType)
	      {
              case 'D':
		RcvData();
		break;

              case 'Z':
		RcvEOF();
		break;

              case 'Q':
		RcvNak();
		break;

	      case 'K':
	      case 'R':
		RcvAck();
		break;

              case 'T':
		RcvNak();
		break;

	      case 'C':
		RcvCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		RcvCancel();
		break;
	      }
	    break;

	  case SND_FILE:
	    switch (cType)
	      {
	      case 'K':
              case 'T':
		SndFile();
		break;

              case 'N':
              case 'Q':
		SndLast();
		break;

	      case 'C':
		SndCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		SndCancel();
		break;
	      }
	    break;

	  case SND_ACKFILE:
	    switch (cType)
	      {
	      case 'Y':
                filexfer.ym.cState = SND_DATA;
		break;

              case 'N':
		if (wXferConsecErrors > 1)
		  {
		  SndData();
                  filexfer.ym.cState = SND_DATA;
		  break;
		  }

	      case 'K':
              case 'Q':
              case 'T':
		SndLast();
		break;

	      case 'C':
		SndCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		SndCancel();
		break;
	      }
	    break;

	  case SND_DATA:
	    switch (cType)
	      {
	      case 'K':
              case 'Y':
		SndData();
		break;

              case 'N':
              case 'Q':
              case 'T':
		SndLast();
		break;

	      case 'C':
		SndCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		SndCancel();
		break;
	      }
	    break;

	  case SND_EOF:
	    switch (cType)
	      {
	      case 'K':
              case 'Y':
		SndEOF();
		break;

              case 'N':
              case 'Q':
              case 'T':
		SndLast();
		break;

	      case 'C':
		SndCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		SndCancel();
		break;
	      }
	    break;

	  case XFR_TERM:
	    XferYModemTerm();
	    break;
	  }
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}

static char near RcvBuild()
{
  int  i;


  while ((i = CommGet()) != -1)
    {
    if (filexfer.ym.nChrCnt == 0)
      {
      switch (i)
	{
	case ASC_SOH:
          filexfer.ym.sData[0] = ASC_SOH;
	  // dca00088 PJL Fix PacketSize display problem.
	  nXferBlockSize = Xfer.nPacketSize = 128;
          filexfer.ym.nChrCnt++;
	  break;

	case ASC_STX:
          filexfer.ym.sData[0] = ASC_STX;
	  // dca00088 PJL Fix PacketSize display problem.
	  nXferBlockSize = Xfer.nPacketSize = 1024;
          filexfer.ym.nChrCnt++;
	  break;

	case ASC_CAN:
          if (filexfer.ym.cCANCnt++ > 0)
	    {
	    nErrorCode = ERRXFR_HOSTCANCEL;
            return 'C';
	    }
	  break;

	case ASC_EOT:
          return 'Z';
	  break;

	default:
          filexfer.ym.cCANCnt = 0;
	  break;
	}
      }
    else
      {
      if (filexfer.ym.nChrCnt == 1)
	{
        filexfer.ym.sData[1] = (BYTE)i;
        filexfer.ym.nChrCnt++;
	}
      else if (filexfer.ym.nChrCnt == 2)
	{
        filexfer.ym.sData[2] = (BYTE)i;
        filexfer.ym.nChrCnt++;
	}
      else
	{
        filexfer.ym.sData[filexfer.ym.nChrCnt++] = (BYTE)i;

        if (filexfer.ym.nChrCnt >= nXferBlockSize + 5)
	  {
          if (filexfer.ym.sData[1] != (char)((unsigned char)filexfer.ym.sData[2]^0xFF) ||
              !VerifyBlk(&filexfer.ym.sData[3], nXferBlockSize))
	    {
            return 'Q';
	    }
	  else
	    {
            return 'D';
	    }
	  }
	}
      }
    }

  return -1;
}

static void near RcvStart()
{
  char buf[1];

  if (wXferConsecErrors < 10)
    {
    buf[0] = 'C';
    CommDataOutput(buf, 1);
    filexfer.ym.lTimeOut = GetCurrentTime() + filexfer.ym.nTimeOut;
    }
  else
    {
    nErrorCode = ERRXFR_HOSTNORESP;
    XferYModemTerm();
    }
}

static void near RcvFile()
{
  register i;


  if (filexfer.ym.sData[3] == NULL)
    {
    RcvAck();
    filexfer.ym.cSeqNo++;
    filexfer.ym.cState = XFR_TERM;
    filexfer.ym.cWait  = XFER_SND_WAIT;
    if (wXferCurrFiles == 0)
      nErrorCode = ERRXFR_NOFILEFOUND;
    return;
    }

  strcpy(szXferFilePath, StripPath(&filexfer.ym.sData[3]));
  if (nErrorCode = CreateXferFile(szXferFilePath, &filexfer.ym.file))
    {
    RcvCancel();
    return;
    }

  filexfer.ym.cSeqNo++;
  i = strlen(&filexfer.ym.sData[3]);
  lXferFileSize = atol(&filexfer.ym.sData[4+i]);

  RcvAck();

  if (lXferFileSize < 0)
    {
    nErrorCode = ERRXFR_NOFILEFOUND;
    RcvCancel();
    return;
    }
  else
    {
    CommDataOutput("C", 1);
    }

  wXferCurrFiles++;

  lXferFileBytes = 0L;
  ResetXferIndicators(FALSE);
  filexfer.ym.cState = RCV_START;
  filexfer.ym.lTimeOut = GetCurrentTime() + filexfer.ym.nTimeOut;
}


static void near RcvAck()
{
  char buf[1];


  buf[0] = ASC_ACK;
  CommDataOutput(buf, 1);

  filexfer.ym.nChrCnt = 0;
  filexfer.ym.cCANCnt = 0;
  filexfer.ym.cEOTCnt = 0;

  filexfer.ym.lTimeOut = GetCurrentTime() + filexfer.ym.nTimeOut;
}


static void near RcvNak()
{
  char buf[1];


  if (lXferFileBytes == 0L && wXferConsecErrors > 10)
    {
    nErrorCode = ERRXFR_TOOMANYERRORS;
    RcvCancel();
    return;
    }
  else if (lXferFileBytes == 0L)
    {
    buf[0] = 'C';
    }
  else
    {
    buf[0] = ASC_NAK;
    }

  CommDataOutput(buf, 1);

  filexfer.ym.nChrCnt = 0;
  filexfer.ym.cCANCnt = 0;

  filexfer.ym.lTimeOut = GetCurrentTime() + filexfer.ym.nTimeOut;
}


static void near RcvData()
  {
  USHORT usBytes, usResults;
  register len, i;


  RcvAck();
  filexfer.ym.cSeqNo++;

  if (lXferFileBytes < lXferFileSize || lXferFileSize == 0L)
    {
    if (lXferFileSize == 0L)
      len = nXferBlockSize;
    else if ((long)nXferBlockSize <= lXferFileSize - lXferFileBytes)
      len = nXferBlockSize;
    else
      len = (int)(lXferFileSize - lXferFileBytes);

    usResults = DosWrite(filexfer.ym.file, &filexfer.ym.sData[3], len, &usBytes);
    if (usResults || len != usBytes)
      {
      if (usResults)
	nErrorCode = XlatError(usResults);
      else
	nErrorCode = ERRDOS_DISKFULL;
      RcvCancel();
      return;
      }
    }

  lXferFileBytes += (long)len;
  wXferBlockCount++;
  UpdateXferIndicators();
  }


static void near RcvEOF()
{
  char buf[1];

  if (filexfer.ym.cEOTCnt++ > 0)
    {
    RcvAck();
    buf[0] = 'C';
    CommDataOutput(buf, 1);

    DosClose(filexfer.ym.file);
    filexfer.ym.file = -1;
    szXferFilePath[0]	 = NULL;

    filexfer.ym.cState = RCV_FILE;
    filexfer.ym.cSeqNo = 0;
    lXferFileSize	   = 0L;
    lXferFileBytes	   = 0L;
    ResetXferIndicators(FALSE);
    }
  else
    {
    RcvNak();
    }
}


static void near RcvCancel()
{
  char buf[16];
  register i;

  for (i = 0; i < 8; i++)
    {
    buf[i+0] = ASC_CAN;
    buf[i+8] = ASC_BS;
    }

  CommDataOutput(buf, sizeof(buf));

  if (filexfer.ym.file != -1)
    DosClose(filexfer.ym.file);
  filexfer.ym.file = -1;
  if (szXferFilePath[0] == NULL && fXferDirection == FALSE)
    DosDelete(szXferFilePath, 0L);

  filexfer.ym.cState = XFR_TERM;
  filexfer.ym.cWait  = XFER_SND_WAIT;
}

static char near SndBuild()
{
  int  i;

  while ((i = CommGet()) != -1)
    {
    switch (i)
      {
      case ASC_ACK:
        filexfer.ym.cCANCnt = 0;
        return 'Y';
	break;

      case ASC_NAK:
        filexfer.ym.cCANCnt = 0;
        return 'N';
	break;

      case 'C':
        filexfer.ym.cCANCnt = 0;
        if (lXferFileBytes == 0L)
	  {
	  nXferBlockSize = Xfer.nPacketSize;
	  return 'K';
	  }
	break;

      case ASC_CAN:
        if (filexfer.ym.cCANCnt++ > 0)
	  {
	  nErrorCode = ERRXFR_HOSTCANCEL;
          return 'C';
	  }
	break;

      default:
        filexfer.ym.cCANCnt = 0;
	break;
      }
    }

  return -1;
}

static void near SndFile()
  {
  FILESTATUS fsts;
  char buf[64];


  memset(filexfer.ym.sData, 0, 133);

  filexfer.ym.sData[0] = ASC_SOH;
  filexfer.ym.sData[1] = 0;
  filexfer.ym.sData[2] = 255;

  if (filexfer.ym.file == -1)
    {
    if (_dos_findnext(&filexfer.ym.sendfile) == 0)
      {
      if ((filexfer.ym.file = OpenXferFile(filexfer.ym.sendfile.name)) == -1)
        {
        nErrorCode = ERRSYS_OPENFAULT;
        SndCancel();
        return;
        }

      strcpy(szXferFilePath, filexfer.ym.sendfile.name);

      filexfer.ym.cSeqNo = 0;

      DosQFileInfo(filexfer.ym.file, 1, &fsts, sizeof(fsts));
      lXferFileSize = fsts.cbFile;

      nXferBlockSize = Xfer.nPacketSize;
      }
    }

  if (filexfer.ym.file != -1)
    {
    register i;

    strcpy(buf, StripPath(szXferFilePath));
    sprintf(&filexfer.ym.sData[3], "%s^%ld 0 0 0 0", buf, lXferFileSize);
    for (i = 3; i < 128; i++)
      {
      if (filexfer.ym.sData[i] == '^')
	filexfer.ym.sData[i] = '\0';
      }
    wXferCurrFiles++;
    }

  ComputeCRC(&filexfer.ym.sData[3], 128);

  filexfer.ym.cSeqNo = 1;

  CommDataOutput(filexfer.ym.sData, 133);

  if (filexfer.ym.file != -1)
    filexfer.ym.cState = SND_ACKFILE;
  else
    filexfer.ym.cState = SND_EOF;

  wXferBlockCount = 0;

  ResetXferIndicators(FALSE);
  filexfer.ym.cWait = XFER_SND_WAIT;
}

static void near SndData()
{
  USHORT usBytes;


  if (lXferFileBytes >= lXferFileSize)
    {
    filexfer.ym.sData[0] = ASC_EOT;
    filexfer.ym.cState   = SND_EOF;
    CommDataOutput(filexfer.ym.sData, 1);
    }
  else
    {
    memset(filexfer.ym.sData, 0, nXferBlockSize+5);

    if (nXferBlockSize == 128)
      filexfer.ym.sData[0] = ASC_SOH;
    else
      filexfer.ym.sData[0] = ASC_STX;

    filexfer.ym.sData[1] = filexfer.ym.cSeqNo;
    filexfer.ym.sData[2] = filexfer.ym.cSeqNo ^ 0xFF;

    if (DosRead(filexfer.ym.file, &filexfer.ym.sData[3], nXferBlockSize, &usBytes))
      {
      nErrorCode = ERRSYS_READFAULT;
      SndCancel();
      return;
      }

    if (usBytes != nXferBlockSize)
      memset(&filexfer.ym.sData[3+usBytes], NULL, nXferBlockSize - usBytes);

    ComputeCRC(&filexfer.ym.sData[3], nXferBlockSize);

    filexfer.ym.cSeqNo++;
    lXferFileBytes += (long)nXferBlockSize;

    CommDataOutput(filexfer.ym.sData, nXferBlockSize+5);
    wXferBlockCount++;
    }

  filexfer.ym.cCANCnt = 0;

  UpdateXferIndicators();

  filexfer.ym.cWait = XFER_SND_WAIT;
}

static void near SndEOF()
{
  if (filexfer.ym.file != -1)
    {
    DosClose(filexfer.ym.file);
    filexfer.ym.file   = -1;
    filexfer.ym.cState = SND_FILE;
    filexfer.ym.cSeqNo = 0;
    lXferFileBytes = 0L;
    }
  else
    {
    XferYModemTerm();
    }
}

static void near SndLast()
{
  register len;


  if (filexfer.ym.sData[0] == ASC_SOH)
    len = 128 + 5;
  else if (filexfer.ym.sData[0] == ASC_STX)
    len = 1024 + 5;
  else
    len = 1;

  CommDataOutput(filexfer.ym.sData, len);

  filexfer.ym.cCANCnt = 0;
  filexfer.ym.cWait   = XFER_SND_WAIT;
}

static void near SndCancel()
{
  char buf[16];
  register i;

  for (i = 0; i < 8; i++)
    {
    buf[i+0] = ASC_CAN;
    buf[i+8] = ASC_BS;
    }

  CommDataOutput(buf, sizeof(buf));

  filexfer.ym.cState = XFR_TERM;
  filexfer.ym.cWait  = XFER_SND_WAIT;
}

static BOOL near VerifyBlk(azBuf, nCount)
  char *azBuf;
  int  nCount;
{
  register int i, crc;
  unsigned char *ptr;


  ptr = (unsigned char *)azBuf;

  crc = 0;
  while (nCount-- > 0)
    {
    crc = crc ^ (int)(*(azBuf++)) << 8;
    for (i = 0; i < 8; ++i)
      {
      if (crc & 0x8000)
	crc = crc << 1 ^ 0x1021;
      else
	crc = crc << 1;
      }
    }

  if (((int)(*(azBuf++)) & 0x00FF) != (((crc & 0xFF00) >> 8) & 0x00FF))
    return FALSE;

  if (((unsigned int)(*azBuf) & 0x00FF) != (crc & 0x00FF))
    return FALSE;

  return TRUE;
}

static void near ComputeCRC(pBuf, nCount)
  char *pBuf;
  int  nCount;
{
  register int crc, i;

  crc = 0;
  while (nCount-- > 0)
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

  *(pBuf++) = (char)((crc & 0xFF00) >> 8);
  *pBuf     = (char)(crc & 0x00FF);
}


static void near SendAbortString()
  {
  char buf[6];
  int  i;

  buf[0] = ASC_CAN;
  buf[1] = ASC_CAN;
  buf[2] = ASC_CAN;
  buf[3] = ASC_BS;
  buf[4] = ASC_BS;
  buf[5] = ASC_BS;

  CommDataOutput(buf, sizeof(buf));
  }
