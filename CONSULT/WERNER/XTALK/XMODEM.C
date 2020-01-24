/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   XMODEM File Transfer (XTALK.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 08/02/89 00006 Display timeout errors during uploading     *
*				    using XMODEM to a non-responsing host.    *
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
#include "prodata.h"
#include "term.h"
#include "xfer.h"
#include "dialogs.h"
#include "global.h"
#include "variable.h"


#define SND_DATA	0x01
#define SND_EOF 	0x02

#define RCV_DATA	0x03
#define RCV_EOF 	0x04

#define XFR_TERM	0x05

static int near RcvBuild();
static int near SndBuild();

static void near RcvData();
static void near RcvEOF();
static void near RcvAck();
static void near RcvNak();
static void near RcvCancel();
static void near RcvFileInfo();

static void near SndData();
static void near SndLast();
static void near SndCancel();

static BOOL near VerifyBlk(char *, int);
static void near ComputeSUM(char *, int);
static void near ComputeCRC(char *, int);
static void near SendAbortString(void);


int XferXModemSendName(protocol, filename)
  int  protocol;
  char *filename;
{
  int	   hFile;
  FILESTATUS fsts;
  OFSTRUCT OfStruct;
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendAbortString();
    return (i);
    }

  wXferTotalFiles  = 0;
  dwXferTotalBytes = 0L;

  if (filename == NULL || *filename == NULL)
    {
    if ((hFile = XferUploadFile("\\*.*", &OfStruct)) <= 0)
      return (ERRSYS_OPENFAULT);
    strcpy(szXferFilePath, StripPath(OfStruct.szPathName));
    }
  else
    {
    if ((hFile = OpenXferFile(filename)) == -1)
      return (ERRSYS_OPENFAULT);
    strcpy(szXferFilePath, filename);
    }

  memset(&filexfer.xmodem, 0, sizeof(filexfer.xmodem));

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

  fXferDirection	  = TRUE;
  filexfer.xmodem.cState  = SND_DATA;
  filexfer.xmodem.cSeqNo  = 1;
  filexfer.xmodem.file    = hFile;

  DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;

  switch (protocol)
    {
    case XFER_MODEM7:
	   // dca00088 PJL Fix PacketSize display problem.
      nXferBlockSize = Xfer.nPacketSize = 128;
      filexfer.xmodem.cCRC    = 0;
      break;

    case XFER_XMODEM:
	   // dca00088 PJL Fix PacketSize display problem.
      nXferBlockSize = Xfer.nPacketSize = 128;
      filexfer.xmodem.cCRC    = 1;
      break;

    case XFER_XMODEM1K:
	   // dca00088 PJL Fix PacketSize display problem.
      nXferBlockSize = Xfer.nPacketSize = 1024;
      filexfer.xmodem.cCRC    = 1;
      break;
    }

  lpXferDlg = MakeProcInstance((FARPROC)DlgXModem, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}


int XferXModemRecvName(protocol, filename)
  int  protocol;
  char *filename;
{
  int	   hFile;
  OFSTRUCT OfStruct;
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
    if ((hFile = XferDownloadFile("\\*.*", &OfStruct)) <= 0)
      {
      SendAbortString();
      return (ERRSYS_OPENFAULT);
      }
    strcpy(szXferFilePath, StripPath(OfStruct.szPathName));
    }
  else
    {
    if (nErrorCode = CreateXferFile(filename, &hFile))
      {
      SendAbortString();
      return(nErrorCode);
      }
    strcpy(szXferFilePath, filename);
    }

  memset(&filexfer.xmodem, 0, sizeof(filexfer.xmodem));

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
  fXferDirection    = FALSE;
  lXferFileSize     = 0L;
  filexfer.xmodem.cState  = RCV_DATA;
  filexfer.xmodem.cSeqNo  = 1;
  filexfer.xmodem.nChrCnt = 0;
  filexfer.xmodem.file    = hFile;

  switch (protocol)
    {
    case XFER_MODEM7:
	   // dca00088 PJL Fix PacketSize display problem.
      nXferBlockSize = Xfer.nPacketSize = 128;
      filexfer.xmodem.cCRC  = 0;
      break;

    case XFER_XMODEM:
	   // dca00088 PJL Fix PacketSize display problem.
      nXferBlockSize = Xfer.nPacketSize = 128;
      filexfer.xmodem.cCRC  = 1;
      break;

    case XFER_XMODEM1K:
	   // dca00088 PJL Fix PacketSize display problem.
      nXferBlockSize = Xfer.nPacketSize = 1024;
      filexfer.xmodem.cCRC  = 1;
      break;
    }

  lpXferDlg = MakeProcInstance((FARPROC)DlgXModem, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}

void XferXModemTerm()
{
  register i;


  i = nErrorCode;

  if ((wSystem & SS_XFER) == 0)
    return;

  ResetSystemStatus(SS_XFER);

  if (filexfer.xmodem.file != -1)
    DosClose(filexfer.xmodem.file);
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


BOOL DlgXModem(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char	 buf[32];
  char	 cType;
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      nErrorCode = 0;

      hXferDlg = hDlg;

      switch (Xfer.cTiming)
	{
	case XFER_TIMING_TIGHT:
	  filexfer.xmodem.nTimeOut = 4000;
	  break;

	case XFER_TIMING_NORMAL:
	  filexfer.xmodem.nTimeOut = 10000;
	  break;

	case XFER_TIMING_LOOSE:
	  filexfer.xmodem.nTimeOut = 15000;
	  break;

	default:
	  filexfer.xmodem.nTimeOut = 30000;
	  break;
	}

      InitXferIndicators();

      if (fXferDirection)
	{
	InfoDisplayMsg(MSGXFR_WAITUP);
	filexfer.xmodem.lTimeOut = GetCurrentTime() + filexfer.xmodem.nTimeOut;
	}
      else
	{
	InfoDisplayMsg(MSGXFR_WAITDOWN);
	filexfer.xmodem.lTimeOut = GetCurrentTime() + 500;
	}

      UpdateXferIndicators();

      CommFlushBuffers();
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_CANCEL:
	  nErrorCode = ERRXFR_LOCALCANCEL;
	  if (fXferDirection)
	    {
	    filexfer.xmodem.cWait = XFER_RCV_WAIT;
	    nXferRcvWait = 8;
	    }
	  else
	    {
	    if (filexfer.xmodem.cEOTCnt)
	      RcvEOF();
	    filexfer.xmodem.cWait = XFER_RCV_WAIT;
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
	XferXModemTerm();
	break;
	}

      if (filexfer.xmodem.cWait == XFER_SND_WAIT)
	{
	if (ComStat.cbOutQue == 0)
	  {
	  filexfer.xmodem.cWait = XFER_NUL_WAIT;
	  filexfer.xmodem.lTimeOut = GetCurrentTime() +
				     (long)filexfer.xmodem.nTimeOut;
	  }
	else
	  break;
	}
      else if (filexfer.xmodem.cWait == XFER_RCV_WAIT)
	{
	if (ComStat.cbInQue != 0)
	  RcvBuild();
	else
	  nXferRcvWait--;
	if (nXferRcvWait <= 0)
	  RcvCancel();
        break;
	}

      if (filexfer.xmodem.cState == XFR_TERM)
	{
	cType = 'Z';
	}
      else if (ComStat.cbInQue != NULL)
	{
	if (fXferDirection)
	  cType = (char)SndBuild();
	else
	  cType = (char)RcvBuild();

	filexfer.xmodem.lTimeOut = GetCurrentTime() + filexfer.xmodem.nTimeOut;
	}
      else if (GetCurrentTime() > filexfer.xmodem.lTimeOut)
	{
        cType = 'T';
	}
      else
	cType = -1;

      if (cType == 'D')
	{
	if (filexfer.xmodem.sData[1] == (BYTE)(filexfer.xmodem.cSeqNo - 1))
	  {
	  if (lXferFileBytes == 0L)
	    cType = 'S';
	  else
	    cType = 'R';
	  }
	else if (filexfer.xmodem.sData[1] != filexfer.xmodem.cSeqNo)
	  {
	  cType = 'C';
	  nErrorCode = ERRXFR_SEQERROR;
	  }
	}

      if (cType=='T' || cType=='Q' || cType=='N' || cType=='S' || cType=='R')
	{
        if (cType != 'N' || filexfer.xmodem.cEOTCnt == 0)  // 00016 Last Nak
          {
          if (cType != 'T' || filexfer.xmodem.cSeqNo != 1 || lXferFileBytes != 0L)
            {
            if (filexfer.xmodem.file != -1)
              wXferTotalErrors++;
            }
          if (filexfer.xmodem.fFirstNak)  // 00015 Prevent the first error
            {
            if (wXferConsecErrors++ > 5 && Xfer.cTiming == XFER_TIMING_TIGHT)
              filexfer.xmodem.nTimeOut = 10000;
            }
          else
            filexfer.xmodem.fFirstNak = TRUE;
          }

	switch (cType)
	  {
	  case 'T':
	    // 00006 Display XMODEM no-response timeout to users.
	    // 00006 if (!filexfer.xmodem.cEOTCnt && lXferFileSize != 0L)
	    if (!filexfer.xmodem.cEOTCnt)
              wXferLastError = XFER_ERROR_TIMEOUT;
            else
              wXferLastError = 0;
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

        UpdateXferIndicators();
	}
      else
	{
	if (cType != -1)
	  wXferConsecErrors = 0;
	}

      if (cType != -1)
	{
	switch (filexfer.xmodem.cState)
	  {
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

              case 'R':
                RcvAck();
                break;

              case 'S':
                RcvFileInfo();
                break;

              case 'T':
		if (filexfer.xmodem.cEOTCnt)
		  RcvEOF();
		else
		  RcvNak();
		break;

	      case 'C':
		if (filexfer.xmodem.cEOTCnt)
		  RcvEOF();
		else
		  RcvCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		RcvCancel();
		break;
	      }
	    break;

	  case SND_DATA:
	    switch (cType)
	      {
              case 'Y':
		SndData();
		break;

              case 'Q':
		RcvNak();
		break;

              case 'N':
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
              case 'Y':
		XferXModemTerm();
		break;

              case 'Q':
		RcvNak();
		break;

              case 'N':
              case 'T':
		SndLast();
		break;

	      case 'C':
		RcvCancel();
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		SndCancel();
		break;
	      }
	    break;

	  case XFR_TERM:
	    XferXModemTerm();
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

static int near RcvBuild()
{
  unsigned char buf[256];
  char *ptr;
  register i;


  if ((i = ReadComm(nCommID, buf, sizeof(buf))) < 0)
    {
    filexfer.xmodem.nChrCnt = 0;
    i = -i;
    }

  ptr = &buf[0];

  while (i--)
    {
    switch (filexfer.xmodem.nChrCnt)
      {
      case 0:
	switch (*ptr++)
	  {
	  case ASC_SOH:
	    filexfer.xmodem.sData[0] = ASC_SOH;
	   // dca00088 PJL Fix PacketSize display problem.
	    nXferBlockSize = Xfer.nPacketSize = 128;
	    filexfer.xmodem.nChrCnt++;
	    break;

	  case ASC_STX:
	    filexfer.xmodem.sData[0] = ASC_STX;
	   // dca00088 PJL Fix PacketSize display problem.
	    nXferBlockSize = Xfer.nPacketSize = 1024;
	    filexfer.xmodem.nChrCnt++;
	    break;

	  case ASC_CAN:
	    if (filexfer.xmodem.cCANCnt++ > 0)
	      {
	      nErrorCode = ERRXFR_HOSTCANCEL;
	      return 'C';
	      }
	    break;

	  case ASC_EOT:
            return 'Z';
	    break;

	  default:
	    filexfer.xmodem.cCANCnt = 0;
	    break;
	  }
	break;

      case 1:
	filexfer.xmodem.sData[1] = *ptr++;
	filexfer.xmodem.nChrCnt++;
	break;

      case 2:
	filexfer.xmodem.sData[2] = *ptr++;
	filexfer.xmodem.nChrCnt++;
	break;

      default:
	filexfer.xmodem.sData[filexfer.xmodem.nChrCnt++] = *ptr++;

	if (filexfer.xmodem.nChrCnt >= nXferBlockSize + 4 + filexfer.xmodem.cCRC)
          {
          if (i)
            {
            while (i--)
              {
              switch (*ptr++)
                {
                case ASC_CAN:
                  if (filexfer.xmodem.cCANCnt++ > 0)
                    {
                    nErrorCode = ERRXFR_HOSTCANCEL;
                    TermWriteData(ptr, i);
                    return 'C';
                    }
                  break;

                default:
                  filexfer.xmodem.cCANCnt = 0;
                  break;
                }
              }
            }

	  if (filexfer.xmodem.sData[1] != (char)((unsigned char)filexfer.xmodem.sData[2]^0xFF) ||
	      !VerifyBlk(&filexfer.xmodem.sData[3], nXferBlockSize))
            return 'Q';
	  else
            return 'D';
	  }
	break;
      }
    }

  return -1;
}

static void near RcvAck()
{
  char buf[1];


  buf[0] = ASC_ACK;
  CommDataOutput(buf, 1);

  filexfer.xmodem.nChrCnt = 0;
  filexfer.xmodem.cCANCnt = 0;
  filexfer.xmodem.cEOTCnt = 0;

  filexfer.xmodem.lTimeOut = GetCurrentTime() + filexfer.xmodem.nTimeOut;
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
  else if (wXferBlockCount == 0 && filexfer.xmodem.cCRC)
    {
    buf[0] = 'C';
    }
  else
    buf[0] = ASC_NAK;

  CommDataOutput(buf, 1);

  filexfer.xmodem.nChrCnt = 0;
  filexfer.xmodem.cCANCnt = 0;

  filexfer.xmodem.lTimeOut = GetCurrentTime() + filexfer.xmodem.nTimeOut;
}


static void near RcvData()
{
  USHORT usBytes, usResults;
  char buf[16];
  register len, i;


  if (filexfer.xmodem.file == -1)
    {
    nErrorCode = ERRXFR_NOFILEFOUND;
    return;
    }

  if (wXferBlockCount == 0)
    {
    if (nXferBlockSize > 256)
      InfoDisplayMsg(MSGXFR_MODEM1KRCV);
    else if (filexfer.xmodem.cCRC == 0)
      InfoDisplayMsg(MSGXFR_MODEM7RCV);
    else
      InfoDisplayMsg(MSGXFR_XMODEMRCV);
    }

  filexfer.xmodem.cSeqNo++;
  wXferBlockCount++;

  if (lXferFileBytes < lXferFileSize || lXferFileSize == 0L)
    {
    if (lXferFileSize == 0L)
      len = nXferBlockSize;
    else if ((long)nXferBlockSize <= lXferFileSize - lXferFileBytes)
      len = nXferBlockSize;
    else
      len = (int)(lXferFileSize - lXferFileBytes);

    usResults = DosWrite(filexfer.xmodem.file, &filexfer.xmodem.sData[3], len, &usBytes);
    if (usResults || len != usBytes)
      {
      if (usResults)
	nErrorCode = XlatError(usResults);
      else
	nErrorCode = ERRDOS_DISKFULL;
      RcvCancel();
      return;
      }

    lXferFileBytes += (long)len;
    }

  RcvAck();

  UpdateXferIndicators();
}


static void near RcvEOF()
{

  if (filexfer.xmodem.cEOTCnt++ > 0)
    {
    RcvAck();
    if (filexfer.xmodem.file != -1)
      DosClose(filexfer.xmodem.file);
    szXferFilePath[0] = NULL;
    filexfer.xmodem.file = -1;

    filexfer.xmodem.cState = XFR_TERM;
    filexfer.xmodem.cWait  = XFER_SND_WAIT;
    }
  else
    {
    RcvNak();
    }
}


static void near RcvFileInfo()
{
  register i;


  RcvAck();

  i = strlen(&filexfer.xmodem.sData[3]);
  lXferFileSize = atol(&filexfer.xmodem.sData[4+i]);

  lXferFileBytes = 0L;
  ResetXferIndicators(FALSE);
}

static void near RcvCancel()
{
  char buf[16];
  int  i;

  for (i = 0; i < 8; i++)
    {
    buf[i+0] = ASC_CAN;
    buf[i+8] = ASC_BS;
    }

  CommDataOutput(buf, sizeof(buf));

  if (filexfer.xmodem.file != -1)
    DosClose(filexfer.xmodem.file);
  filexfer.xmodem.file = -1;
  if (szXferFilePath[0] && fXferDirection == FALSE)
    DosDelete(szXferFilePath, 0L);

  filexfer.xmodem.cState = XFR_TERM;
  filexfer.xmodem.cWait  = XFER_SND_WAIT;
}


static int near SndBuild()
{
  char buf[64];
  register int i, j;


  if ((i = ReadComm(nCommID, buf, sizeof(buf))) <= 0)
    return (-1);

  for (j = 0; j < i; j++)
    {
    if (buf[j] != ASC_CAN && buf[j] != NULL)
      filexfer.xmodem.cCANCnt = 0;

    switch (buf[j])
      {
      case ASC_ACK:
	if (wXferBlockCount != 0)
          return 'Y';
	break;

      case ASC_NAK:
	if (wXferBlockCount == 0)
	  {
	  filexfer.xmodem.cCRC = 0;
          return 'Y';
	  }

        return 'N';
	break;

      case 'C':
	if (wXferBlockCount == 0)
	  {
	  filexfer.xmodem.cCRC = 1;
          return 'Y';
	  }
	break;

      case ASC_CAN:
	if (filexfer.xmodem.cCANCnt++ > 0)
	  {
	  nErrorCode = ERRXFR_HOSTCANCEL;
          return 'C';
	  }
	break;

      default:
	filexfer.xmodem.cCANCnt = 0;
	break;
      }
    }

  return -1;
}

static void near SndData()
{
  USHORT usBytes;


  if (wXferBlockCount == 0)
    {
    if (nXferBlockSize > 256)
      InfoDisplayMsg(MSGXFR_MODEM1KSND);
    else if (filexfer.xmodem.cCRC == 0)
      InfoDisplayMsg(MSGXFR_MODEM7SND);
    else
      InfoDisplayMsg(MSGXFR_XMODEMSND);
    }

  if (filexfer.xmodem.file == -1)
    {
    nErrorCode = ERRXFR_NOFILEFOUND;
    XferXModemTerm();
    return;
    }

  if (DosRead(filexfer.xmodem.file, &filexfer.xmodem.sData[3], nXferBlockSize, &usBytes))
    {
    nErrorCode = ERRSYS_READFAULT;
    SndCancel();
    return;
    }
  else if (usBytes == 0)
    {
    filexfer.xmodem.sData[0] = ASC_EOT;
    CommDataOutput(filexfer.xmodem.sData, 1);
    if (filexfer.xmodem.file != -1)
      DosClose(filexfer.xmodem.file);
    filexfer.xmodem.file = -1;
    filexfer.xmodem.cState = SND_EOF;
    return;
    }

  if (usBytes != nXferBlockSize)
    memset(&filexfer.xmodem.sData[3+usBytes], NULL, nXferBlockSize - usBytes);

  lXferFileBytes += usBytes;

  if (nXferBlockSize == 128)
    filexfer.xmodem.sData[0] = ASC_SOH;
  else
    filexfer.xmodem.sData[0] = ASC_STX;

  wXferBlockCount++;

  filexfer.xmodem.sData[1] = filexfer.xmodem.cSeqNo;
  filexfer.xmodem.sData[2] = filexfer.xmodem.cSeqNo ^ 0xFF;
  filexfer.xmodem.cSeqNo++;

  if (filexfer.xmodem.cCRC == 0)
    ComputeSUM(&filexfer.xmodem.sData[3], nXferBlockSize);
  else
    ComputeCRC(&filexfer.xmodem.sData[3], nXferBlockSize);

  CommDataOutput(filexfer.xmodem.sData, nXferBlockSize + 4 + filexfer.xmodem.cCRC);

  UpdateXferIndicators();

  filexfer.xmodem.cWait  = XFER_SND_WAIT;
}

static void near SndLast()
{
  if (filexfer.xmodem.sData[0] == ASC_EOT)
    {
    CommDataOutput(filexfer.xmodem.sData, 1);
    }
  else
    {
    if (filexfer.xmodem.cCRC == 0)
      ComputeSUM(&filexfer.xmodem.sData[3], nXferBlockSize);
    else
      ComputeCRC(&filexfer.xmodem.sData[3], nXferBlockSize);

    CommDataOutput(filexfer.xmodem.sData, nXferBlockSize + 4 + filexfer.xmodem.cCRC);
    }

  filexfer.xmodem.cWait  = XFER_SND_WAIT;
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

  filexfer.xmodem.cState = XFR_TERM;
  filexfer.xmodem.cWait  = XFER_SND_WAIT;
  }


static BOOL near VerifyBlk(azBuf, nCount)
  char *azBuf;
  int  nCount;
{
  int i, crc;
  unsigned int	j;
  unsigned char *ptr;

  ptr = (unsigned char *)azBuf;


  if (filexfer.xmodem.cCRC == 0)
    {
    j = 0;
    while (nCount-- > 0)
      j += *(ptr++);

    if (((int)(*ptr) & 0x00FF) == (j & 0x00FF))
      return TRUE;
    else
      return FALSE;
    }
  else
    {
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
    }
  return TRUE;
}


static void near ComputeSUM(pBuf, nCount)
  char *pBuf;
  int  nCount;
{
  register int i;

  i = 0;
  while (nCount-- > 0)
    i += *(pBuf++);

  *pBuf = (char)i;
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

  *(pBuf++) =  (char)((crc & 0xFF00) >> 8);
  *pBuf     =  (char)(crc & 0x00FF);
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
