/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Digital Communications, Inc.  All rights reserved.      *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Crosstalk File Transfer Protocol (CROSS.C)                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   Here is my view of what the Crosstalk File Transfer Protocol    *
*             should do.  I used a data monitor, assembler source to the      *
*             Crosstalk code in Mk.4 and comments from Crosstalk people to    *
*             develop this code.  Since XTALK does not have a command mode    *
*             the Crosstalk protocol had to be coded to simulate command      *
*             mode for multible file transfers.                               *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 10/15/89 00044 Add a delay between the ^C and 'RC' to      *
*				    support R2HOST.			      *
*	      1.01 10/15/89 00054 Add a delay for R2HOST.		      *
*                                                                             *
\*****************************************************************************/

#define NOATOM                  /* atom routines */
#define NOBITMAP		/* typedef HBITMAP and associated routines */
#define NOBRUSH 		/* typedef HBRUSH and associated routines */
#define NOCLIPBOARD		/* clipboard routines */
#define NOCOLOR 		/* CTLCOLOR_*, COLOR_* */
#define NOCREATESTRUCT		/* typedef CREATESTRUCT */
#define NODRAWTEXT		/* DrawText() and DT_* */
#define NOFONT			/* typedef FONT and associated routines */
#define NOGDI			/* StretchBlt modes and gdi logical objects */
#define NOGDICAPMASKS		/* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOHDC			/* typedef HDC and associated routines */
#define NOICON			/* IDI_* */
#define NOKANJI 		/* Kanji support stuff. */
#define NOKEYSTATE		/* MK_* */
#define NOMETAFILE		/* typedef METAFILEPICT */
#define NOMINMAX		/* Macros min(a,b) and max(a,b) */
#define NONCMESSAGES		/* WM_NC* and HT* */
#define NOPEN			/* typedef HPEN and associated routines */
#define NORASTEROPS		/* binary and tertiary raster ops */
#define NOREGION		/* typedef HRGN and associated routines */
#define NORESOURCE		/* Predefined resource types:  RT_* */
#define NOSOUND 		/* Sound driver routines */
#define NOSYSMETRICS		/* SM_*, GetSystemMetrics */
#define NOTEXTMETRIC		/* typedef TEXTMETRIC and associated routines */
#define NOVIRTUALKEYCODES	/* VK_* */
#define NOWH			/* SetWindowsHook and WH_* */
#define NOWINOFFSETS		/* GWL_*, GCL_*, associated routines */
#define NOWINSTYLES		/* WS_*, CS_*, ES_*, LBS_* */
#define NOWNDCLASS		/* typedef WNDCLASS and associated routines */

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB was memory.h now string.h
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "xfer.h"
#include "capture.h"
#include "dialogs.h"
#include "variable.h"

#define SND_FILE	0x01
#define SND_SIZE	0x02
#define SND_DATA	0x03
#define SND_EOF 	0x04
#define SND_TERM        0x05

#define RCV_FILE        0x06
#define RCV_TERM        0x07


extern BOOL DlgCross(HWND, unsigned, WORD, LONG);

static char near PacketBuild();
static char near SndBuild();

static void near RcvRequest();
static void near RcvFile();
static void near RcvSize();
static void near RcvData();
static void near RcvEOF();
static void near RcvAck();
static void near RcvCan();
static void near RcvDle();
static void near RcvEot();
static void near RcvNak();
static void near RcvSO();
static void near RcvCancel();

static void near SndFile(char *);
static void near SndSize();
static void near SndData();
static void near SndLast();
static void near SndCancel();

static BOOL near VerifyBlk(char *, int);
static void near ComputeCRC(char *, int);

int XferCrossSendName(filename)
  char *filename;
{
  OFSTRUCT OfStruct;
  DCB	   sDCB;
  char	   buf[8];
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.cross, 0, sizeof(filexfer.cross));

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);
  if (filename == NULL || *filename == NULL)
    {
    if (XferUploadBatch("\\*.*", szXferFilePath))
      return (ERRSYS_OPENFAULT);
    }
  else
    strcpy(szXferFilePath, filename);

  if (ComputeFilesToSend(szXferFilePath, &filexfer.cross.sendfile) == 0)
    return (ERRXFR_NOFILEFOUND);

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

  fXferDirection	= TRUE;
  filexfer.cross.cState = SND_FILE;
  filexfer.cross.cSeqNo = 0;
  filexfer.cross.file   = -1;
  wXferCurrFiles  = 0;

  if (Xfer.nPacketSize < 256)
    nXferBlockSize = 256;
  else if (Xfer.nPacketSize > 1024)
    nXferBlockSize = 1024;
  else
    nXferBlockSize = Xfer.nPacketSize;

  lpXferDlg = MakeProcInstance((FARPROC)DlgCross, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}


int XferCrossRecvName(filename)
  char *filename;
{
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.cross, 0, sizeof(filexfer.cross));

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);
  if (filename == NULL)
    {
    if (!XferFileRequest())
      return (ERRXFR_LOCALCANCEL);
    }
  else
    {
    strcpy(szXferFilePath, filename);
    }

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

  fXferDirection	 = FALSE;
  nXferBlockSize	 = 256;
  lXferFileSize 	 = 0L;
  filexfer.cross.cSeqNo  = 0;
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.file    = -1;
  filexfer.cross.cState  = RCV_TERM;

  wXferCurrFiles  = 0;
  wXferTotalFiles = 0;
  dwXferTotalBytes = 0L;

  lpXferDlg = MakeProcInstance((FARPROC)DlgCross, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  if (szXferFilePath[0] && Xfer.fHostMode == 0)
    {
    char buf[8];

    buf[0] = ASC_ETX;
    buf[1] = 'X';
    buf[2] = 'M';
    buf[3] = ' ';
    CommDataOutput(buf, 4);
    CommDataOutput(szXferFilePath, strlen(szXferFilePath));
    buf[0] = ASC_CR;
    CommDataOutput(buf, 1);
    filexfer.cross.fDisplay = TRUE;
    }

  return (NULL);
}

int XferCrossServer()
{
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.cross, 0, sizeof(filexfer.cross));
  filexfer.cross.fServer = TRUE;

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);

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

  fXferDirection	 = FALSE;
  nXferBlockSize	 = 256;
  lXferFileSize 	 = 0L;
  filexfer.cross.cSeqNo  = 0;
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.file    = -1;
  filexfer.cross.cState  = RCV_FILE;

  wXferCurrFiles  = 0;
  wXferTotalFiles = 0;
  dwXferTotalBytes = 0L;

  lpXferDlg = MakeProcInstance((FARPROC)DlgCross, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  if (szXferFilePath[0] && Xfer.fHostMode == 0)
    {
    char buf[8];

    buf[0] = ASC_ETX;
    buf[1] = 'X';
    buf[2] = 'M';
    buf[3] = ' ';
    CommDataOutput(buf, 4);
    CommDataOutput(szXferFilePath, strlen(szXferFilePath));
    buf[0] = ASC_CR;
    CommDataOutput(buf, 1);
    filexfer.cross.fDisplay = TRUE;
    }

  return (NULL);
}

void XferCrossTerm()
{
  register i;

  if ((wSystem & SS_XFER) == 0)
    return;

  i = nErrorCode;
  ResetSystemStatus(SS_XFER);

  if (filexfer.cross.file != -1)
    DosClose(filexfer.cross.file);
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


BOOL DlgCross(hDlg, message, wParam, lParam)
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
	  filexfer.cross.nTimeOut = 4000;
	  break;

	case XFER_TIMING_NORMAL:
	  filexfer.cross.nTimeOut = 10000;
	  break;

	case XFER_TIMING_LOOSE:
	  filexfer.cross.nTimeOut = 15000;
	  break;

	default:
	  filexfer.cross.nTimeOut = 30000;
	  break;
	}

      InitXferIndicators();
      if (filexfer.cross.fServer)
        ResetXferIndicators(TRUE);

      CommFlushBuffers();

      if (fXferDirection)
	{
	InfoDisplayMsg(MSGXFR_CROSSSND);
	filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
        SndFile(filexfer.cross.sendfile.name);
	}
      else
	{
	InfoDisplayMsg(MSGXFR_CROSSRCV);
	filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
	}
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_CANCEL:
	  nErrorCode = ERRXFR_LOCALCANCEL;
	  if (fXferDirection)
            {
            if (filexfer.cross.cState == SND_TERM)
              {
              XferCrossTerm();
              }
            else
              {
              RcvCan();
              filexfer.cross.cState = SND_TERM;
              }
	    }
	  else
	    {
            if (filexfer.cross.cState == RCV_FILE)
              filexfer.cross.fCancel = TRUE;
            else
              {
              filexfer.cross.cWait = XFER_RCV_WAIT;
              nXferRcvWait = 8;
              }
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
	XferCrossTerm();
	break;
	}

      if (filexfer.cross.cWait == XFER_SND_WAIT)
	{
	if (ComStat.cbOutQue == 0)
          {
	  filexfer.cross.cWait = XFER_NUL_WAIT;
	  filexfer.cross.lTimeOut = GetCurrentTime() +
				     (long)filexfer.cross.nTimeOut;

          if (filexfer.cross.cState == SND_TERM)
            {
            XferCrossTerm();
            break;
            }
	  }
	else
	  break;
	}
      else if (filexfer.cross.cWait == XFER_RCV_WAIT)
	{
        if (ComStat.cbInQue + CommSize() != 0)
	  PacketBuild();
	else
	  nXferRcvWait--;
	if (nXferRcvWait <= 0)
	  {
          RcvCancel();
	  break;
          }
        break;
	}

      if (ComStat.cbInQue + CommSize() != NULL)
	{
	cType = PacketBuild();
	filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
	}
      else if (GetCurrentTime() >= filexfer.cross.lTimeOut)
	{
        cType = 'T';
	}
      else
	cType = -1;

      if (cType == ASC_NUL)
	{
	if (filexfer.cross.sData[4] == ((BYTE)(filexfer.cross.cSeqNo-1)|0x80))
	  cType = 'R';
	else if (filexfer.cross.sData[4] != (filexfer.cross.cSeqNo | 0x80))
	  {
	  nErrorCode = ERRXFR_SEQERROR;
	  cType = 'S';
	  }
	}

      if (cType=='T' || cType=='Q' || cType==ASC_NAK || cType=='S' || cType=='R' || cType == 'L')
	{
        wXferTotalErrors++;
	wXferConsecErrors++;
	switch (cType)
	  {
	  case 'T':
	    wXferLastError = XFER_ERROR_TIMEOUT;
	    break;

	  case 'Q':
	    wXferLastError = XFER_ERROR_CRC;
	    break;

	  case ASC_NAK:
	    wXferLastError = XFER_ERROR_NAK;
	    break;

	  case 'R':
	    wXferLastError = XFER_ERROR_DUP;
	    break;

          case 'L':
            wXferLastError = XFER_ERROR_LONG;
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
        if (filexfer.cross.fCancel)
          {
          nXferRcvWait = 4;
          filexfer.cross.cWait = XFER_RCV_WAIT;
          break;
          }

	switch (filexfer.cross.cState)
          {
	  case RCV_FILE:
	    switch (cType)
	      {
	      case ASC_ETX:
		RcvFile();
		break;

	      case ASC_SYN:
		RcvSize();
		break;

	      case ASC_NUL:
		RcvData();
		break;

	      case ASC_EOT:
		RcvEOF();
		break;

	      case ASC_SO:
		RcvAck();
		break;

	      case ASC_CAN:
		RcvAck();
		nErrorCode = ERRXFR_HOSTCANCEL;
		RcvCancel();
		break;

	      case 'T':
                if (lXferFileSize != 0L)
		  RcvNak();
                else
                  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
		break;

	      case 'Q':
		RcvNak();
		break;

	      case 'S':
	      case 'C':
		RcvCancel();
		break;

	      case 'R':
		RcvAck();
		break;

              case 'L':
                filexfer.cross.cWait = XFER_RCV_WAIT;
                nXferRcvWait = 8;
                nErrorCode = ERRXFR_TOOLONG;
		break;

	      default:
		nErrorCode = ERRXFR_HOSTBAD;
		RcvCancel();
		break;
	      }
	    break;

          case RCV_TERM:
	    switch (cType)
	      {
	      case ASC_ETX:
		RcvFile();
		break;

	      case ASC_SYN:
		RcvSize();
                filexfer.cross.cState = RCV_FILE;
		break;

              case 'T':
                XferCrossTerm();
		break;

              default:
                filexfer.cross.nChrCnt = 0;
                filexfer.cross.cCANCnt = 0;
                break;
	      }
	    break;

	  case SND_FILE:
	    switch (cType)
	      {
	      case ASC_ETX:
		SndFile(szXferFilePath);
		break;

	      case ASC_ACK:
		SndSize();
		break;

              case 'T':
		SndSize();
		break;

	      case ASC_CAN:
		RcvAck();
		nErrorCode = ERRXFR_HOSTCANCEL;
                XferCrossTerm();
		break;

	      case ASC_DLE:
		RcvAck();
		nErrorCode = ERRXFR_HOSTDISKFULL;
                XferCrossTerm();
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

	  case SND_SIZE:
	    switch (cType)
	      {
	      case ASC_ETX:
		SndFile(szXferFilePath);
		break;

	      case ASC_NAK:
                SndLast();
		break;

	      case ASC_ACK:
	      case 'T':
		SndData();
		break;

	      case ASC_CAN:
		RcvAck();
		nErrorCode = ERRXFR_HOSTCANCEL;
                XferCrossTerm();
		break;

	      case ASC_DLE:
		RcvAck();
		nErrorCode = ERRXFR_HOSTDISKFULL;
                XferCrossTerm();
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
	      case ASC_ACK:
		SndData();
		break;

	      case ASC_EOT:
		SndFile(0);
		break;

	      case ASC_NAK:
              case 'Q':
              case 'T':
		SndLast();
		break;

	      case ASC_CAN:
		RcvAck();
		nErrorCode = ERRXFR_HOSTCANCEL;
                XferCrossTerm();
		break;

	      case ASC_DLE:
		RcvAck();
		nErrorCode = ERRXFR_HOSTDISKFULL;
                XferCrossTerm();
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

          case SND_TERM:
	    switch (cType)
	      {
	      case ASC_ACK:
                XferCrossTerm();
		break;

              case 'C':
                RcvCancel();
                break;

	      default:
                RcvCancel();
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

  return TRUE;
}

static char near PacketBuild()
{
  int  i;


  while ((i = CommGet()) != -1)
    {
    if (filexfer.cross.fDisplay && i != ASC_SOH && i != ASC_ACK && i != ASC_ETX)
      {
      TermWriteByte(i);
      }
    else if (filexfer.cross.nChrCnt == 0)
      {
      filexfer.cross.fDisplay = FALSE;
      switch (i)
	{
	case ASC_SOH:
	  filexfer.cross.sData[0] = ASC_SOH;
	  filexfer.cross.nChrCnt++;
	  break;

	case ASC_ETX:
	  filexfer.cross.sData[0] = ASC_ETX;
	  filexfer.cross.nPacketSize = 80;
	  filexfer.cross.nChrCnt++;
	  break;

        case ASC_ACK:
	  filexfer.cross.cCANCnt = 0;
          return (ASC_ACK);
	  break;

	case ASC_CAN:
	  if (filexfer.cross.cCANCnt++ > 0)
	    {
            return 'C';
	    nErrorCode = ERRXFR_HOSTCANCEL;
	    }
	  break;

	default:
	  filexfer.cross.cCANCnt = 0;
	  break;
	}
      }
    else if (filexfer.cross.nChrCnt == 1)
      {
      switch (i)
	{
	case ASC_ACK:
	case ASC_NAK:
	case ASC_EOT:
	case ASC_CAN:
	case ASC_DLE:
	case ASC_SO:
          filexfer.cross.sData[1] = (BYTE)i;
	  filexfer.cross.nPacketSize = 4;
	  filexfer.cross.nChrCnt++;
	  break;

	case ASC_SYN:
          filexfer.cross.sData[1] = (BYTE)i;
	  filexfer.cross.nPacketSize = 8;
	  filexfer.cross.nChrCnt++;
	  break;

	case ASC_NUL:
          filexfer.cross.sData[1] = (BYTE)i;
	  filexfer.cross.nChrCnt++;
	  break;

	default:
	  if (filexfer.cross.sData[0] == ASC_ETX)
	    {
	    filexfer.cross.sData[1] = (char)i;
	    filexfer.cross.nChrCnt++;
	    }
	  else
	    {
	    filexfer.cross.nChrCnt = 0;
	    filexfer.cross.cCANCnt = 0;
	    }
	  break;
	}
      }
    else if (filexfer.cross.nChrCnt < 4)
      {
      filexfer.cross.sData[filexfer.cross.nChrCnt++] = (char)i;
      if (filexfer.cross.nChrCnt == 4)
	{
	if (filexfer.cross.sData[1] == ASC_NUL)
	  {
	  nXferBlockSize = filexfer.cross.sData[2]*256+filexfer.cross.sData[3];
	  filexfer.cross.nPacketSize = nXferBlockSize + 9;
          if (nXferBlockSize > sizeof(filexfer.cross.sData)-32)
            {
            filexfer.cross.nChrCnt = 0;
            filexfer.cross.cCANCnt = 0;
            return ('L');
            }
	  }
	else if (filexfer.cross.nPacketSize == 4)
	  {
	  if (!VerifyBlk(&filexfer.cross.sData[1], 1))
            return 'Q';
	  else
	    return (filexfer.cross.sData[1]);
	  }
	}
      }
    else
      {
      if (filexfer.cross.nChrCnt > sizeof(filexfer.cross.sData)-32)
       {
        filexfer.cross.nChrCnt = 0;
        filexfer.cross.cCANCnt = 0;
        return ('L');
        }

      if (filexfer.cross.sData[0] == ASC_ETX)
	{
        if (i == ASC_CR)
          {
          filexfer.cross.nPacketSize = filexfer.cross.nChrCnt;
          filexfer.cross.sData[filexfer.cross.nChrCnt] = NULL;
	  filexfer.cross.nChrCnt = 0;
	  return (ASC_ETX);
	  }
	else
	  filexfer.cross.sData[filexfer.cross.nChrCnt++] = (char)i;
	if (filexfer.cross.nChrCnt >= filexfer.cross.nPacketSize)
	  {
	  filexfer.cross.nChrCnt = 0;
	  filexfer.cross.cCANCnt = 0;
	  }
	}
      else
	{
	filexfer.cross.sData[filexfer.cross.nChrCnt++] = (char)i;
	if (filexfer.cross.nChrCnt >= filexfer.cross.nPacketSize)
	  {
	  filexfer.cross.nChrCnt = 0;
	  if (!VerifyBlk(&filexfer.cross.sData[1],filexfer.cross.nPacketSize-3))
	    return 'Q';
	  else
	    return (filexfer.cross.sData[1]);
	  }
	}
      }
    }

  return -1;
}


static void near RcvFile()
  {
  if (filexfer.cross.file == -1)
    {
    if (memicmp(&filexfer.cross.sData[1], "No more", 7) == 0)
      {
      if (filexfer.cross.nPacketSize > 15)
        {
        TermWriteData(&filexfer.cross.sData[14], filexfer.cross.nPacketSize-15);
        TermWriteNewLine();
        }
      if (wXferCurrFiles == 0)
        nErrorCode = ERRXFR_NOFILEFOUND;
      XferCrossTerm();
      return;
      }

    filexfer.cross.sData[strlen(filexfer.cross.sData)] = NULL;
    strcpy(szXferFilePath, StripPath(&filexfer.cross.sData[4]));

    /*
    if (nErrorCode = CreateXferFile(szXferFilePath, &filexfer.cross.file))
      {
      RcvCancel();
      return;
      }
    else
      {
      wXferCurrFiles++;
      ResetXferIndicators(FALSE);
      filexfer.cross.cState = RCV_FILE;
      }
    */
    }

  RcvAck();
  }


static void near RcvAck()
{
  char buf[4];

  buf[0] = ASC_SOH;
  buf[1] = ASC_ACK;
  buf[2] = 0x00;
  buf[3] = 0x14;
  CommDataOutput(buf, 4);
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cCANCnt = 0;

  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
}


static void near RcvCan()
{
  char buf[4];


  buf[0] = ASC_SOH;
  buf[1] = ASC_CAN;
  ComputeCRC(&buf[1], 1);
  CommDataOutput(buf, 4);
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cCANCnt = 0;

  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
}


static void near RcvDle()
{
  char buf[4];


  buf[0] = ASC_SOH;
  buf[1] = ASC_DLE;
  ComputeCRC(&buf[1], 1);
  CommDataOutput(buf, 4);
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cCANCnt = 0;

  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
}


static void near RcvEot()
{
  char buf[4];


  buf[0] = ASC_SOH;
  buf[1] = ASC_EOT;
  buf[2] = 0x80;
  buf[3] = 0x1B;
  CommDataOutput(buf, 4);

  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cCANCnt = 0;

  filexfer.cross.lTimeOut = GetCurrentTime() + 4000;
}


static void near RcvNak()
{
  char buf[4];


  if (lXferFileBytes == 0L && wXferConsecErrors > 10)
    {
    nErrorCode = ERRXFR_TOOMANYERRORS;
    RcvCancel();
    return;
    }

  buf[0] = ASC_SOH;
  buf[1] = ASC_NAK;
  ComputeCRC(&buf[1], 1);
  CommDataOutput(buf, 4);

  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cCANCnt = 0;

  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
}


static void near RcvSize()
{
  RcvAck();

  lXferFileSize  = *((long *)&filexfer.cross.sData[2]);
  ResetXferIndicators(FALSE);
}


static void near RcvData()
{
  USHORT usBytes;
  char buf[16];
  int  len, i;


  filexfer.cross.cSeqNo++;

  if (filexfer.cross.file == -1)
    {
    if (nErrorCode = CreateXferFile(szXferFilePath, &filexfer.cross.file))
      {
      RcvCancel();
      return;
      }
    else
      {
      wXferCurrFiles++;
      ResetXferIndicators(FALSE);
      filexfer.cross.cState = RCV_FILE;
      }
    }

  if (lXferFileBytes < lXferFileSize || lXferFileSize == 0L)
    {
    if (lXferFileSize == 0L)
      len = nXferBlockSize;
    else if ((long)nXferBlockSize <= lXferFileSize - lXferFileBytes)
      len = nXferBlockSize;
    else
      len = (int)(lXferFileSize - lXferFileBytes);

    i = DosWrite(filexfer.cross.file, &filexfer.cross.sData[6], len, &usBytes);
    if (i || usBytes != len)
      {
      if (i)
	nErrorCode = XlatError(i);
      else
	nErrorCode = ERRDOS_DISKFULL;
    //RcvDle();
      RcvCancel();
      return;
      }
    }

  RcvAck();

  lXferFileBytes += (long)len;
  wXferBlockCount++;
  UpdateXferIndicators();
}


static void near RcvEOF()
{
  char buf[1];


  RcvEot();

  if (filexfer.cross.file != -1)
    {
    DosClose(filexfer.cross.file);
    filexfer.cross.file = -1;
    }
  filexfer.cross.cSeqNo = 0;
  szXferFilePath[0] = NULL;
  lXferFileBytes = 0L;
  lXferFileSize  = 0L;
  ResetXferIndicators(FALSE);

  filexfer.cross.cState = RCV_TERM;
  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
}


static void near RcvSO()
{
  char buf[4];


  buf[0] = ASC_SOH;
  buf[1] = ASC_SO;
  ComputeCRC(&buf[1], 1);
  CommDataOutput(buf, 4);
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cCANCnt = 0;

  filexfer.cross.lTimeOut = GetCurrentTime() + filexfer.cross.nTimeOut;
}


static void near RcvCancel()
{
  if (szXferFilePath[0] && !fXferDirection)
    {
    if (filexfer.cross.file != -1)
      DosClose(filexfer.cross.file);
    filexfer.cross.file = -1;
    DosDelete(szXferFilePath, 0L);
    szXferFilePath[0] = '\0';
    }

  RcvCan();

 filexfer.cross.cWait  = XFER_SND_WAIT;
 filexfer.cross.cState = SND_TERM;
}

static void near SndFile(szFile)
  char *szFile;
{
  FILESTATUS fsts;


  if (filexfer.cross.sData[0] == ASC_ETX)
    {
    TermWriteData(&filexfer.cross.sData[1], filexfer.cross.nPacketSize-2);
    TermWriteNewLine();
    }

  if (szFile == 0)
    {
    if (_dos_findnext(&filexfer.cross.sendfile) == 0)
      {
      szFile = filexfer.cross.sendfile.name;
      }
    else
      {
      if (filexfer.cross.fServer)
        CommDataOutput("\003No more files\r", 15);
      XferCrossTerm();
      return;
      }
    }

  if ((filexfer.cross.file = OpenXferFile(szFile)) <= 0)
    {
    if (filexfer.cross.fServer)
      CommDataOutput("\003No more files\r", 15);
    XferCrossTerm();
    return;
    }

  DosQFileInfo(filexfer.cross.file, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;
  strcpy(szXferFilePath, szFile);

  // 00044 Add a delay between the ETX and the 'RC' so R2HOST will work.
  filexfer.cross.sData[0] = ASC_ETX;
  CommDataOutput(filexfer.cross.sData, 1);

  // 00044 If we only had a real OS to work with...
  LibDelay(1000);

  filexfer.cross.sData[0] = 'R';
  filexfer.cross.sData[1] = 'C';
  filexfer.cross.sData[2] = ' ';
  CommDataOutput(filexfer.cross.sData, 3);
  CommDataOutput(StripPath(szFile), strlen(StripPath(szFile)));
  filexfer.cross.sData[0] = ASC_CR;
  CommDataOutput(filexfer.cross.sData, 1);

  filexfer.cross.cSeqNo = 0;
  wXferCurrFiles++;
  ResetXferIndicators(FALSE);

  filexfer.cross.nChrCnt  = 0;
  filexfer.cross.cState   = SND_FILE;
  filexfer.cross.cWait    = XFER_SND_WAIT;
  filexfer.cross.fDisplay = TRUE;
}

static void near SndSize()
{
  char buf[8];


  buf[0] = ASC_SOH;
  buf[1] = ASC_SYN;
  *((long *)(&buf[2])) = lXferFileSize;
  ComputeCRC(&buf[1], 5);
  CommDataOutput(buf, 8);

  memcpy(filexfer.cross.sData, buf, 8);
  filexfer.cross.nPacketSize = 8;

  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cState = SND_SIZE;
  filexfer.cross.cWait	= XFER_SND_WAIT;
}

static void near SndData()
{
  USHORT usBytes;


  if (lXferFileBytes >= lXferFileSize)
    {
    RcvEot();
    if (filexfer.cross.file)
      {
      DosClose(filexfer.cross.file);
      filexfer.cross.file = -1;
      }
    filexfer.cross.cSeqNo = 0;
    lXferFileBytes = 0L;

    // 00054 Wait for 2.5 seconds after a file upload for R2HOST.
    LibDelay(2500);
    }
  else if (filexfer.cross.file != -1)
    {
    filexfer.cross.sData[0] = ASC_SOH;
    filexfer.cross.sData[1] = NULL;
    filexfer.cross.sData[4] = filexfer.cross.cSeqNo | 0x80;
    filexfer.cross.sData[5] = ASC_STX;

    if (DosRead(filexfer.cross.file, &filexfer.cross.sData[6], nXferBlockSize, &usBytes))
      {
      nErrorCode = ERRSYS_READFAULT;
      SndCancel();
      return;
      }

    filexfer.cross.sData[2] = (BYTE)(usBytes / 256);
    filexfer.cross.sData[3] = (BYTE)(usBytes % 256);
    filexfer.cross.sData[usBytes+6] = ASC_ETX;
    ComputeCRC(&filexfer.cross.sData[1], usBytes+6);

    filexfer.cross.cSeqNo++;
    lXferFileBytes += (long)usBytes;

    CommDataOutput(filexfer.cross.sData, usBytes+9);
    filexfer.cross.nPacketSize = usBytes+9;
    filexfer.cross.nSendSize = filexfer.cross.nPacketSize;
    memcpy(filexfer.cross.sSend,filexfer.cross.sData,filexfer.cross.nSendSize);
    wXferBlockCount++;
    }

  filexfer.cross.cCANCnt = 0;

  UpdateXferIndicators();

  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cState  = SND_DATA;
  filexfer.cross.cWait	 = XFER_SND_WAIT;
}

static void near SndLast()
{
  CommDataOutput(filexfer.cross.sSend, filexfer.cross.nSendSize);

  filexfer.cross.cCANCnt = 0;
  filexfer.cross.nChrCnt = 0;
  filexfer.cross.cWait	 = XFER_SND_WAIT;
}

static void near SndCancel()
{
  RcvCan();

  filexfer.cross.cState = SND_TERM;
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
	crc = crc << 1 ^ 0x8005;
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
	crc = crc << 1 ^ 0x8005;
      else
	crc = crc << 1;
      }
    }

  *(pBuf++) = (char)((crc & 0xFF00) >> 8);
  *pBuf     = (char)(crc & 0x00FF);
}
