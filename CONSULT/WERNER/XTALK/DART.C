/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1989, 1990 Digital Communications Associates, Inc.           *
*  All rights reserved.                                                       *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   DART File Transfer Protocol (DART.C)                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the Crosstalk Communication's DART file    *
*             transfer protocol.  I use a data monitor, source to DART from   *
*             Mk.4, a 11 page document of the DART protocol and comments from *
*             from Crosstalk people to develop this code.                     *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/05/89 00021    Added DART compression fix from Paula.   *
*             1.01 09/05/89 00024    Previous xfer info still in the dialog.  *
*             1.01 02/01/90 dca00011 PJL Fixed DART Cancelling problems.      *
*             1.1  02/16/90 dca00025 PJL DART wasn't handling the Target path *
*                                    in INIT packets (on Receives).           *
*             1.1  04/11/90 dca00080 PJL More error reporting fixes.          *
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
#include "capture.h"
#include "dialogs.h"
#include "variable.h"

#define ST_SERVER	0x01
#define ST_RECV 	0x02
#define ST_SEND 	0x03
#define ST_INIT 	0x04
#define ST_HEADER	0x05
#define ST_DATA 	0x06
#define ST_EOF		0x07
#define ST_BREAK	0x08
#define ST_FINISH	0x09
#define ST_TERM    0x0A


extern BOOL DlgDart(HWND, unsigned, WORD, LONG);

static void near ProcessInitPacket();
static void near ProcessHeaderPacket();
static void near ProcessDataPacket();
static void near ProcessEofPacket();
static void near ProcessBreakPacket();
static void near ProcessCancelPacket();

static void near GenerateRecvInitPacket();
static void near GenerateSendInitPacket();
static void near ProcessInitAckPacket();
static void near GenerateHeaderPacket();
static void near ProcessHeaderAckPacket();
static void near GenerateDataPacket(BOOL fRestart);
static void near GenerateEofPacket();
static void near GenerateBreakPacket();

static char near ReceivePacket();
static void near SendInfoPacket(char, char);
static void near SendDataPacket(char, char, BYTE *, int);
static void near SendLastPacket();
static void near SendCancelPacket(int);
static int  near DecompressNonData(BYTE *, int);
static int  near CompressNonData(BYTE *, int);

static void near CancelTransfer(int);

static BOOL near VerifyCRC(BYTE *, int);
static void near ComputeCRC(BYTE *, int);
static void near ComputeDataCKSUM(BYTE *, int);


int XferDartSendName(filename)
  char *filename;
{
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.dart, 0, sizeof(filexfer.dart));

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);
  if (filename == NULL || *filename == NULL)
    {
    if (XferUploadBatch("\\*.*", szXferFilePath))
      return (ERRSYS_OPENFAULT);
    }
  else
    strcpy(szXferFilePath, filename);

  if (ComputeFilesToSend(szXferFilePath, &filexfer.dart.sendfile) == 0)
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

  fXferDirection       = TRUE;
  lXferFileBytes       = 0L;
  lXferFileSize        = 0L;
  filexfer.dart.cState = ST_SEND;
  filexfer.dart.cSeqNo = 0;
  filexfer.dart.file   = -1;

  if (Xfer.fHostMode == 0)
    {
    char szBuf[16];

    szBuf[0] = ASC_ETX;
    szBuf[1] = 'R';
    szBuf[2] = 'E';
    szBuf[3] = 'C';
    szBuf[4] = 'E';
    szBuf[5] = 'I';
    szBuf[6] = 'V';
    szBuf[7] = 'E';
    szBuf[8] = ASC_CR;
    CommDataOutput(szBuf, 9);
    filexfer.dart.fDisplay = TRUE;
    }

  nXferBlockSize = 0;

  lpXferDlg = MakeProcInstance((FARPROC)DlgDart, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}


int XferDartRecvName(filename)
  char *filename;
{
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.dart, 0, sizeof(filexfer.dart));

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

  wXferTotalFiles  = 0; 	    // 00024 Clear the previous file count
  fXferDirection	 = FALSE;
  nXferBlockSize	 = 0;
  lXferFileSize 	 = 0L;
  filexfer.dart.cSeqNo	 = 0;
  filexfer.dart.nChrCnt  = 0;
  filexfer.dart.file	 = -1;

  if (szXferFilePath[0] == NULL)
    filexfer.dart.cState  = ST_SERVER;
  else
    {
    if (Xfer.fHostMode == 0)
      {
      char szBuf[1];

      szBuf[0] = ASC_ETX;
      szBuf[1] = 'S';
      szBuf[2] = 'E';
      szBuf[3] = 'N';
      szBuf[4] = 'D';
      szBuf[5] = ASC_CR;
      CommDataOutput(szBuf, 6);
      filexfer.dart.fDisplay = TRUE;
      }

    filexfer.dart.cState  = ST_RECV;
    }

  lpXferDlg = MakeProcInstance((FARPROC)DlgDart, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}

int XferDartServer(char fDirection)
  {
  DCB	   sDCB;
  register int i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.dart, 0, sizeof(filexfer.dart));

  if (i = SetFullPath(VAR_DIRFIL))
    return (i);

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
  fXferDirection        = fDirection;
  nXferBlockSize        = 0;
  lXferFileBytes        = 0L;
  lXferFileSize         = 0L;
  filexfer.dart.cSeqNo	= 0;
  filexfer.dart.nChrCnt = 0;
  filexfer.dart.file    = -1;

  szXferFilePath[0]    = NULL;
  filexfer.dart.cState = ST_SERVER;
 
  lpXferDlg = MakeProcInstance((FARPROC)DlgDart, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}

void XferDartTerm()
{
  register i;

  if ((wSystem & SS_XFER) == 0)
    return;

  i = nErrorCode;

  if (filexfer.dart.file != -1)
    DosClose(filexfer.dart.file);
  if (nErrorCode && szXferFilePath[0] && !fXferDirection && !Xfer.fRecovery)
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
    {
    CommPortDump();
    CommPortUpdate();
    }

  ResetSystemStatus(SS_XFER);

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


BOOL DlgDart(hDlg, message, wParam, lParam)
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
			  filexfer.dart.nTimeOut = 4000;
			  break;

		  case XFER_TIMING_NORMAL:
			  filexfer.dart.nTimeOut = 10000;
			  break;

		  case XFER_TIMING_LOOSE:
			  filexfer.dart.nTimeOut = 15000;
			  break;

		  default:
			  filexfer.dart.nTimeOut = 30000;
			  break;
	   }

      InitXferIndicators();

      if (fXferDirection)
	   {
			InfoDisplayMsg(MSGXFR_DARTSND);
			filexfer.dart.lTimeOut = GetCurrentTime();
	   }
      else
	   {
			if (szXferFilePath[0])
				  GenerateRecvInitPacket();
			InfoDisplayMsg(MSGXFR_DARTRCV);
			filexfer.dart.lTimeOut = GetCurrentTime() + filexfer.dart.nTimeOut;
	   }

      UpdateXferIndicators();
      break;

    case WM_COMMAND:
      switch (wParam)
	   {
		  case ID_CANCEL:
            if (filexfer.dart.cState == ST_FINISH)
               XferDartTerm();
            else
            {
  		          SendCancelPacket(ERRXFR_HOSTCANCEL);
		          CancelTransfer(ERRXFR_LOCALCANCEL);
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
			XferDartTerm();
			break;
	   }

		// Get current COM buffer stats
		GetCommError(nCommID, (COMSTAT FAR *)&ComStat);

		if (filexfer.dart.cWait == XFER_SND_WAIT)
	   {
		   if (ComStat.cbOutQue == 0)
          {
          		if (filexfer.dart.cState == ST_TERM)
            	{
            		XferDartTerm();
		            break;
            	}
				filexfer.dart.cWait = XFER_NUL_WAIT;
			    filexfer.dart.lTimeOut = GetCurrentTime() +
						     (long)filexfer.dart.nTimeOut;
	  	   }
		   else
	  			break;
	   }

      if (ComStat.cbInQue + CommSize() != 0)
	   {
			cType = ReceivePacket();
			if (cType == 'Q')
			   cType = -1;
	        if (!filexfer.dart.fDisplay)
		       filexfer.dart.lTimeOut = GetCurrentTime() + filexfer.dart.nTimeOut;
	   }
      else if (GetCurrentTime() >= filexfer.dart.lTimeOut)
	   {
           cType = 'T';
	   }
      else
			cType = -1;

      if (cType == 'H')
      {
      }
      else if (cType == 'D' || cType == 'Z' || cType == 'Y')
	   {
			if (filexfer.dart.sData[3]-0x20 == ((BYTE)(filexfer.dart.cSeqNo-1)&0x1F))
	        {
	  			if (cType != 'Y')
				    cType = 'R';
				else
	    		cType = -1;
	  		}
			else if (filexfer.dart.sData[3] - 0x20 != filexfer.dart.cSeqNo)
	  		{
	  			nErrorCode = ERRXFR_SEQERROR;
	  			cType = 'S';
	  		}
	   }

      if (cType=='T' || cType=='W' || cType=='N' || cType=='S' || cType=='R')
	   {
			if (cType != 'T' || filexfer.dart.cSeqNo != 0 || lXferFileBytes != 0L)
	  			wXferTotalErrors++;
			wXferConsecErrors++;
			switch (cType)
	  		{
	  			case 'T':
	    			wXferLastError = XFER_ERROR_TIMEOUT;
	    			break;

	  			case 'W':
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
       	   cType = 'K';

			if (Xfer.cMaxErrors != 0 && wXferConsecErrors >= (WORD)Xfer.cMaxErrors)
	          cType = 'K';
		}
       else
	    {
			if (cType != -1)
	  			wXferConsecErrors = 0;
		}

      UpdateXferIndicators();

      if (cType != -1)
	   {
        switch (filexfer.dart.cState)
	     {
	      case ST_SERVER:
	    	switch (cType)
	      	{
	      		case 'I':
					ProcessInitPacket();
					break;

	      		case 'B':
					ProcessBreakPacket();
					break;

	      		case 'C':
					ProcessCancelPacket();
					break;

	      		case 'N':
					SendLastPacket();
					break;

	      		case 'W':
					SendInfoPacket('N', filexfer.dart.cSeqNo);
					break;

	      		case 'T':
                   filexfer.dart.lTimeOut = GetCurrentTime() + filexfer.dart.nTimeOut;
					break;

              	case 'K':
                	CancelTransfer(ERRXFR_TOOMANYERRORS);
					break;

	      		default:
					CancelTransfer(ERRXFR_HOSTBAD);
					break;
	      	}
	    	break;

	  case ST_RECV:
	    switch (cType)
	      {
	      case 'I':
		ProcessInitPacket();
		break;

	      case 'Y':
		ProcessInitAckPacket();
		break;

	      case 'H':
		ProcessHeaderPacket();
		break;

	      case 'D':
		ProcessDataPacket();
		break;

	      case 'Z':
		ProcessEofPacket();
		break;

	      case 'B':
		ProcessBreakPacket();
		break;

	      case 'C':
                if (wXferCurrFiles == 0)
                  nErrorCode = ERRXFR_NOFILEFOUND;
		ProcessCancelPacket();
		break;

	      case 'R':
		SendInfoPacket('Y', filexfer.dart.cSeqNo);
		break;

	      case 'T':
	      case 'N':
		SendLastPacket();
		break;

	      case 'W':
		SendInfoPacket('N', filexfer.dart.cSeqNo);
		break;

	      case 'S':
		SendCancelPacket(ERRXFR_SEQERROR);
		CancelTransfer(ERRXFR_SEQERROR);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		SendCancelPacket(ERRXFR_HOSTBAD);
		CancelTransfer(ERRXFR_HOSTBAD);
		break;
	      }
	    break;

	  case ST_SEND:
	    switch (cType)
	      {
	      case 'C':
		CancelTransfer(ERRXFR_HOSTCANCEL);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		GenerateSendInitPacket();
		break;
	      }
	    break;

	  case ST_INIT:
	    switch (cType)
	      {
	      case 'Y':
		ProcessInitAckPacket();
                GenerateHeaderPacket();
		break;

	      case 'T':
	      case 'N':
	      case 'W':
		SendLastPacket();
		break;

	      case 'C':
		CancelTransfer(ERRXFR_HOSTCANCEL);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		CancelTransfer(ERRXFR_HOSTBAD);
		break;
	      }
	    break;

	  case ST_HEADER:
	    switch (cType)
	      {
	      case 'Y':
		ProcessHeaderAckPacket();
		break;

	      case 'N':
	      case 'T':
	      case 'W':
		SendLastPacket();
		break;

	      case 'C':
		CancelTransfer(ERRXFR_HOSTCANCEL);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		CancelTransfer(ERRXFR_HOSTBAD);
		break;
	      }
	    break;

	  case ST_DATA:
	    switch (cType)
	      {
	      case 'Y':
		GenerateDataPacket(FALSE);
		break;

	      case 'N':
              case 'T':
	      case 'W':
		SendLastPacket();
		break;

	      case 'C':
		CancelTransfer(ERRXFR_HOSTCANCEL);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		CancelTransfer(ERRXFR_HOSTBAD);
		break;
	      }
	    break;

	  case ST_EOF:
	    switch (cType)
	      {
	      case 'Y':
	      case 'Z':
                if (wXferCurrFiles < wXferTotalFiles)
		  GenerateHeaderPacket();
		else
		  GenerateBreakPacket();
		break;

	      case 'N':
              case 'T':
	      case 'W':
		SendLastPacket();
		break;

	      case 'C':
		CancelTransfer(ERRXFR_HOSTCANCEL);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		CancelTransfer(ERRXFR_HOSTBAD);
		break;
	      }
	    break;

	  case ST_BREAK:
	    switch (cType)
	      {
	      case 'Y':
		XferDartTerm();
		break;

	      case 'N':
              case 'T':
	      case 'W':
		SendLastPacket();
		break;

	      case 'C':
		CancelTransfer(ERRXFR_HOSTCANCEL);
		break;

              case 'K':
                CancelTransfer(ERRXFR_TOOMANYERRORS);
		break;

	      default:
		CancelTransfer(ERRXFR_HOSTBAD);
		break;
	      }
	    break;

	  case ST_FINISH:
	    XferDartTerm();
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

/*---------------------------------------------------------------------------*/
/*  Contruct a packet from incomming data from the comm port.  Since all     */
/*  packets snd and rcv are the same, one routine does it all.	Remember,    */
/*  being a Windows App, we must remember our state to give control back to  */
/*  Windows and come back and continue processing.  (Nice App). 	     */
/*---------------------------------------------------------------------------*/
static char near ReceivePacket()
{
  register i;

  while ((i = CommGet()) != -1)
    {
    if (filexfer.dart.nChrCnt == 0)
      {
      if (i == ASC_SOH)
	filexfer.dart.sData[filexfer.dart.nChrCnt++] = (BYTE)i;
      else if (i == ASC_ACK)
        filexfer.dart.fDisplay = FALSE;
      else if (filexfer.dart.fDisplay)
        TermWriteByte(i);
      }
    else if (filexfer.dart.nChrCnt < 8)
      {
      filexfer.dart.fDisplay = FALSE;
      filexfer.dart.sData[filexfer.dart.nChrCnt++] = (BYTE)i;
      if (filexfer.dart.nChrCnt == 8)
	{
	if (!VerifyCRC(&filexfer.dart.sData[1], 4))
	  {
	  filexfer.dart.nChrCnt = 0;
	  return 'W';
	  }
	nXferBlockSize = (filexfer.dart.sData[1] - 0x20 << 5) +
			 (filexfer.dart.sData[2] - 0x20);

	if (nXferBlockSize)
	  {
	  filexfer.dart.nPacketSize = nXferBlockSize + 11;
	  }
	else
	  {
	  filexfer.dart.nChrCnt = 0;
	  return (filexfer.dart.sData[4]);
	 }
	}
      }
    else
      {
      filexfer.dart.sData[filexfer.dart.nChrCnt++] = (BYTE)i;
      if (filexfer.dart.nChrCnt >= filexfer.dart.nPacketSize)
	{
	filexfer.dart.nChrCnt = 0;
	if (!VerifyCRC(&filexfer.dart.sData[8], nXferBlockSize))
	  return 'W';
	else
	  return (filexfer.dart.sData[4]);
	}
      }
    }

  return -1;
}

/*---------------------------------------------------------------------------*/
/*  We just received an INIT packet, lets do something with it. 	     */
/*---------------------------------------------------------------------------*/
static void near ProcessInitPacket()
{
  BYTE buf[32];
  register len;

  len = DecompressNonData(&filexfer.dart.sData[8], nXferBlockSize);

  /* Version level */
  buf[0] = 0x00;

  /* Window Size */
  buf[1] = 0x01;

  /* Packet Size */
	switch(Xfer.nPacketSize)
	{
		case 128:	buf[2] = 0x00;
					break;

		case 256:	buf[2] = 0x01;
					break;

		case 512:	buf[2] = 0x02;
					break;

		default:	buf[2] = 0x03;
					break;
	}
	switch(filexfer.dart.sData[10])
	{
		case 0:		filexfer.dart.nSendPacketSize = 128;
					break;

		case 1:		filexfer.dart.nSendPacketSize = 256;
					break;

		case 2:		filexfer.dart.nSendPacketSize = 512;
					break;

		default:	filexfer.dart.nSendPacketSize = 1024;
					break;
	}

  /* Action */
  switch (filexfer.dart.sData[11])
  {
    case 'S':
      ComputeFilesToSend(&filexfer.dart.sData[12], &filexfer.dart.sendfile);
	   if (wXferTotalFiles)
	   {
			fXferDirection = TRUE;
			InitXferIndicators();

			*(WORD	*)(&buf[3]) = wXferTotalFiles;
			*(DWORD *)(&buf[5]) = dwXferTotalBytes;
			len = CompressNonData(buf, 9);
			SendDataPacket('Y', 0, buf, len);

			GenerateHeaderPacket();
	        ResetXferIndicators(FALSE);
	   }
	   else
	   {
			SendCancelPacket(ERRXFR_NOFILEFOUND);
	   		// dca00011 PJL Fixed DART Cancelling problems ...
	   		//              File Not Found, Drive Not Ready, etc.
			CancelTransfer(ERRXFR_NOFILEFOUND);
      }
      break;

    case 'R':
       wXferTotalFiles	= *(WORD *)(&filexfer.dart.sData[12]);
       dwXferTotalBytes = *(DWORD *)(&filexfer.dart.sData[14]);
		// dca00025 PJL look for target (destination) path spec.
		memset(filexfer.dart.szRequest, 0, MAX_PATH);
		if (len > 12)
		{
			if (SetPath(&filexfer.dart.sData[18]) == 0)
				strcpy(filexfer.dart.szRequest, &filexfer.dart.sData[18]);
 		}
       if (IsEnoughDiskSpace(0, dwXferTotalBytes))
	    {
			fXferDirection = FALSE;
			InitXferIndicators();
			len = CompressNonData(buf, 3);
			SendDataPacket('Y', 0, buf, len);
			filexfer.dart.cSeqNo = 1;
			filexfer.dart.cState = ST_RECV;
	    }
       else
	    {
         	SendCancelPacket(ERRXFR_LOCALDISKFULL);
	        // dca00011 PJL Fixed DART Cancelling problems ... Local Disk Full
           // filexfer.dart.cState = ST_FINISH;
			CancelTransfer(ERRXFR_LOCALDISKFULL);
	 	}
       break;

    case 'F':
       SendInfoPacket('Y', 0);
       filexfer.dart.cState = ST_FINISH;
       break;
  }
}


/*---------------------------------------------------------------------------*/
/*  We just received an HEADER packet, lets open a file to start a xfer.     */
/*  Lots of things to do and check here.  Check for auto-xfer restart and    */
/*  all those receive modes for which to download and which to skip.	     */
/*---------------------------------------------------------------------------*/
static void near ProcessHeaderPacket()
{
  ULONG  ulPos;
  FILESTATUS fsts;
  USHORT usBytes, usResults;
  OFSTRUCT OfStruct;
  int  hFile;
  char szPath[MAX_PATH];
  unsigned wDate, wTime;
  long lPos;
  BYTE buf[32];
  int  len;

  len = DecompressNonData(&filexfer.dart.sData[8], nXferBlockSize);

  filexfer.dart.wFileDate = (filexfer.dart.sData[8] << 8) +
			    filexfer.dart.sData[9];
  filexfer.dart.wFileTime = (filexfer.dart.sData[10] << 8) +
			    filexfer.dart.sData[11];
  lXferFileSize  = (filexfer.dart.sData[14] << 8) + filexfer.dart.sData[15];
  lXferFileSize *= 65536L;
  lXferFileSize += (filexfer.dart.sData[12] << 8) + filexfer.dart.sData[13];
  filexfer.dart.fFileMode = filexfer.dart.sData[16];

  buf[0] = 0;	/* Send the file flag to the other DART */

  lXferFileBytes = 0L;
  // dca00025 PJL look for target (destination) path spec.
  // CreatePath(szPath, VAR_DIRFIL, &filexfer.dart.sData[17]);
  if (filexfer.dart.szRequest[0] != 0)
  {
		int i;
		strcpy(szPath, filexfer.dart.szRequest);
		i = strlen(szPath);
		if (szPath[i-1] != '\\') szPath[i++] = '\\';
		strcpy(szPath+i, &filexfer.dart.sData[17]);
  }
  else CreatePath(szPath, VAR_DIRFIL, &filexfer.dart.sData[17]);
  if ((hFile = OpenFile(szPath, &OfStruct, OF_READWRITE)) < 0)
  {
    if (usResults = CreateXferFile(szPath, &hFile))
    {
	   // dca00080 PJL
	   // SendCancelPacket(ERRSYS_OPENFAULT);
      SendCancelPacket(ERRXFR_HOSTCANCEL);
      CancelTransfer(usResults);
      return;
    }
  }
  else
  {
    /* We have a existing file, lets check for a partial xfer from before */
    DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
    lPos = fsts.cbFile;
    if (lPos >= 16L)
    {
      lPos -= 16;
      DosChgFilePtr(hFile, lPos, FILE_BEGIN, &ulPos);
      DosRead(hFile, buf, 16, &usBytes);
      len = usBytes;
      if (buf[0] == '*' && buf[1] == ' ' && buf[2] == 'P' && buf[3] == 'A' &&
	  buf[4] == 'R' && buf[5] == 'T' && buf[6] == 'I' && buf[7] == 'A' &&
	  buf[8] == 'L' && buf[9] == ' ' && buf[10] == '*' &&
	  buf[11] == (BYTE)(0x20 + ((filexfer.dart.wFileDate >> 5) & 0xF)) &&
	  buf[12] == (BYTE)(0x20 + (filexfer.dart.wFileDate & 0x1F)) &&
	  buf[13] == (BYTE)(0x20 + (filexfer.dart.wFileDate >> 9)) &&
	  buf[14] == (BYTE)(0x20 + (filexfer.dart.wFileTime >> 11)) &&
	  buf[15] == (BYTE)(0x20 + ((filexfer.dart.wFileTime >> 5) & 0x3F)))
	{
	buf[0] = 2;
	*(long *)(&buf[1]) = lPos;
	lXferFileBytes	   = lPos;
	DosChgFilePtr(hFile, lPos, FILE_BEGIN, &ulPos);
	}
      else
	buf[0] = 0;
      }

    if (buf[0] == 0)
      {
      DosChgFilePtr(hFile, 0L, FILE_BEGIN, &ulPos);
      _dos_getftime(hFile, &wDate, &wTime);

      switch (Xfer.cRecvMode)
	{
	case XFER_RECV_NORMAL:
	  if (filexfer.dart.wFileDate == wDate && filexfer.dart.wFileTime == wTime)
	    if (fsts.cbFile == lXferFileSize)
	      buf[0] = 1;
	  break;

	case XFER_RECV_NEWER:
	  if (filexfer.dart.wFileDate < wDate)
	    buf[0] = 1;
	  else if (filexfer.dart.wFileDate > wDate)
	    buf[0] = 0;
	  else if (filexfer.dart.wFileTime <= wTime)
	    buf[0] = 1;
	  else
	    buf[0] = 0;
	  break;

	case XFER_RECV_ALL:
	  buf[0] = 0;
	  break;
	}
      }

    if (buf[0] == 0)
      {
      DosClose(hFile);
      if (usResults = CreateXferFile(szPath, &hFile))
	{
	// dca00080 PJL
   // SendCancelPacket(ERRSYS_OPENFAULT);
   SendCancelPacket(ERRXFR_HOSTCANCEL);
	CancelTransfer(usResults);
	return;
	}
      }
    else if (buf[0] == 1)
      {
      DosClose(hFile);
      hFile = -1;
      }
    }

  if (filexfer.dart.file != -1)
    DosClose(filexfer.dart.file);
  filexfer.dart.file = hFile;

  if (buf[0] == 2)
    len = 5;
  else
    len = 1;
  len = CompressNonData(buf, len);
  SendDataPacket('Y', -1, buf, len);
  if (filexfer.dart.cSeqNo++ == 31)
    filexfer.dart.cSeqNo = 0;

  strcpy(szXferFilePath, OfStruct.szPathName);
  ResetXferIndicators(FALSE);
}


/*---------------------------------------------------------------------------*/
/*  We just received an DATA packet, lets store the data.		     */
/*---------------------------------------------------------------------------*/
static void near ProcessDataPacket()
{
  USHORT usBytes, usResults;
  int  i, j;
  int  nCount, nChar;
  BYTE buf[256];
  register BYTE *ptr;


  ptr = &filexfer.dart.sData[8];
  for (i = j = 0; i < nXferBlockSize;)
    {
    if (j > sizeof(buf) - 16)
      {
      usResults = DosWrite(filexfer.dart.file, buf, j, &usBytes);
      if (usResults || j != usBytes)
	{
        SendCancelPacket(ERRXFR_LOCALDISKFULL);
        CancelTransfer(ERRXFR_LOCALDISKFULL);
        return;
	}
      lXferFileBytes += j;
      dwXferCurrBytes += j;
      j = 0;
      }

    if (*ptr == ASC_DLE)
      {
      ptr++;
      i++;
      if ((*ptr & 0x7F) == 'A' || (*ptr & 0x7F) == 'P' ||
	  (*ptr & 0x7F) == 'Q' || (*ptr & 0x7F) == 'S')
	{
	buf[j++] = (BYTE)(*ptr++ - '@');
	i++;
	}
      else if (*ptr == 'm')
	{
	ptr++;
	i++;
	buf[j++] = ASC_CR;
	buf[j++] = ASC_LF;
	}
      else if (*ptr == 'r')
	{
        nCount = ((ptr[1] - 0x20) << 6) + (ptr[2] - 0x20);
	ptr += 3;
	i   += 3;

	nChar = *ptr++;
	i++;
	if (nChar == ASC_DLE)
	  {
          if ((*ptr & 0x7F) == 'A' || (*ptr & 0x7F) == 'P' ||
              (*ptr & 0x7F) == 'Q' || (*ptr & 0x7F) == 'S')
            {
	    nChar = (BYTE)(*ptr++ - '@');
	    i++;
	    }
	  else
	    {
	    ptr++;
	    i++;
	    nChar = -1;
	    }
	  }

	while (nCount--)
	  {
	  if (nChar != -1)
	    buf[j++] = (BYTE)nChar;
	  else
	    {
	    buf[j++] = ASC_CR;
	    buf[j++] = ASC_LF;
	    }
	  if (j > sizeof(buf) - 16)
	    {
	    usResults = DosWrite(filexfer.dart.file, buf, j, &usBytes);
	    if (usResults || j != usBytes)
	      {
              SendCancelPacket(ERRXFR_LOCALDISKFULL);
              CancelTransfer(ERRXFR_LOCALDISKFULL);
              return;
	      }
	    lXferFileBytes += j;
            dwXferCurrBytes += j;
	    j = 0;
	    }
	  }
	}
      }
    else
      {
      buf[j++] = *ptr++;
      i++;
      }
    }

  if (j)
    {
    usResults = DosWrite(filexfer.dart.file, buf, j, &usBytes);
    if (usResults || j != usBytes)
      {
      SendCancelPacket(ERRXFR_LOCALDISKFULL);
      CancelTransfer(ERRXFR_LOCALDISKFULL);
      }
    lXferFileBytes += j;
    dwXferCurrBytes += j;
    j = 0;
    }

  SendInfoPacket('Y', -1);
  if (filexfer.dart.cSeqNo++ == 31)
    filexfer.dart.cSeqNo = 0;

  wXferBlockCount++;
  UpdateXferIndicators();
}


/*---------------------------------------------------------------------------*/
/*  We just received an EOF packet, lets close the file and idle	     */
/*---------------------------------------------------------------------------*/
static void near ProcessEofPacket()
{
  if (filexfer.dart.file != -1)
    {
    if (!Xfer.fDateMode)
      _dos_setftime(filexfer.dart.file,
		    filexfer.dart.wFileDate, filexfer.dart.wFileTime);
    DosClose(filexfer.dart.file);
    filexfer.dart.file = -1;
    }

  SendInfoPacket('Y', -1);
  if (filexfer.dart.cSeqNo++ == 31)
    filexfer.dart.cSeqNo = 0;

  szXferFilePath[0] = NULL;
  lXferFileSize     = 0L;
  lXferFileBytes    = 0L;
  ResetXferIndicators(FALSE);
}


/*---------------------------------------------------------------------------*/
/*  We just received an BREAK packet, no more files, lets wrap it up.	     */
/*---------------------------------------------------------------------------*/
static void near ProcessBreakPacket()
{
  SendInfoPacket('Y', -1);
  filexfer.dart.cWait == XFER_SND_WAIT;
  filexfer.dart.cState = ST_TERM;
}


/*****************************************************************************\
*  We just received an CANCEL packet, something be wrong here, term.
\*****************************************************************************/
static void near ProcessCancelPacket()
{
  CancelTransfer(filexfer.dart.sData[8]*256+filexfer.dart.sData[9]);
  SendInfoPacket('Y', 0);
  filexfer.dart.cWait == XFER_SND_WAIT;
  filexfer.dart.cState = ST_TERM;
}


/*---------------------------------------------------------------------------*/
/*  Generate the first INIT packet and send it. 			     */
/*---------------------------------------------------------------------------*/
static void near GenerateRecvInitPacket()
{
  BYTE buf[256];
  register len;


  buf[0] = 0;
  buf[1] = 1;
  if (Xfer.nPacketSize == 128)
    buf[2] = 0x00;
  else if (Xfer.nPacketSize == 256)
    buf[2] = 0x01;
  else if (Xfer.nPacketSize == 512)
    buf[2] = 0x02;
  else
    buf[2] = 0x03;
  buf[3] = 'S';
  strcpy(&buf[4], szXferFilePath);

  len = CompressNonData(buf, 5 + strlen(&buf[4]));
  SendDataPacket('I', 0, buf, len);
}


/*---------------------------------------------------------------------------*/
/*  Generate the first INIT packet and send it. 			     */
/*---------------------------------------------------------------------------*/
static void near GenerateSendInitPacket()
{
  BYTE buf[256];
  register len;

  fXferDirection = TRUE;
  InitXferIndicators();

  buf[0] = 0;
  buf[1] = 1;
  if (Xfer.nPacketSize == 128)
    buf[2] = 0x00;
  else if (Xfer.nPacketSize == 256)
    buf[2] = 0x01;
  else if (Xfer.nPacketSize == 512)
    buf[2] = 0x02;
  else
    buf[2] = 0x03;
  buf[3] = 'R';

  *(WORD  *)(&buf[4]) = wXferTotalFiles;
  *(DWORD *)(&buf[6]) = dwXferTotalBytes;

  buf[10] = NULL;

  len = CompressNonData(buf, 11);
  SendDataPacket('I', 0, buf, len);

  strcpy(szXferFilePath, filexfer.dart.sendfile.name);

//GenerateHeaderPacket();
  filexfer.dart.cState = ST_INIT;
}


/*---------------------------------------------------------------------------*/
/*  Go something with the ACK to our INIT packet.			     */
/*---------------------------------------------------------------------------*/
static void near ProcessInitAckPacket()
{
  register len;

  len = DecompressNonData(&filexfer.dart.sData[8], nXferBlockSize);

  if (filexfer.dart.sData[10] == 0)
    filexfer.dart.nSendPacketSize = 128;
  else if (filexfer.dart.sData[10] == 1)
    filexfer.dart.nSendPacketSize = 256;
  else if (filexfer.dart.sData[10] == 2)
    filexfer.dart.nSendPacketSize = 512;
  else
    filexfer.dart.nSendPacketSize = 1024;
}


/*---------------------------------------------------------------------------*/
/*  Tell the other guy about the first/next file to send.		     */
/*---------------------------------------------------------------------------*/
static void near GenerateHeaderPacket()
{
  ULONG  ulPos;
  USHORT usBytes;
  OFSTRUCT OfStruct;
  BYTE buf[256];
  int  len;


  filexfer.dart.wFileDate = filexfer.dart.sendfile.wr_date;
  filexfer.dart.wFileTime = filexfer.dart.sendfile.wr_time;
  lXferFileSize = filexfer.dart.sendfile.size;
  strcpy(szXferFilePath, filexfer.dart.sendfile.name);

  if (filexfer.dart.file == -1)
  {
    if ((filexfer.dart.file = OpenXferFile(szXferFilePath)) <= 0)
    {
	   // dca00080 PJL
	   // SendCancelPacket(ERRSYS_OPENFAULT);
      SendCancelPacket(ERRXFR_HOSTCANCEL);
      CancelTransfer(ERRSYS_OPENFAULT);
      return;
    }
  }

  filexfer.dart.fFileMode = 1;
  if (DosRead(filexfer.dart.file, buf, sizeof(buf), &usBytes) == 0)
    {
    register i;

    for (i = 0; i < usBytes; i++)
      {
      if (buf[i] >= 0x20 && buf[i] <= 0x7F)
        continue;
      if (buf[i] == 0x09 || buf[i] == 0x0D || buf[i] == 0x0A || buf[i] == 0x1A)
        continue;
      filexfer.dart.fFileMode = 0;
      break;
      }
    }
  DosChgFilePtr(filexfer.dart.file, 0L, FILE_BEGIN, &ulPos);

  filexfer.dart.filechk = 0;
  lXferFileBytes = 0L;

  buf[0] = (BYTE)((filexfer.dart.wFileDate >> 8) & 0xFF);
  buf[1] = (BYTE)(filexfer.dart.wFileDate & 0xFF);
  buf[2] = (BYTE)((filexfer.dart.wFileTime >> 8) & 0xFF);
  buf[3] = (BYTE)(filexfer.dart.wFileTime & 0xFF);
  buf[4] = (BYTE)((lXferFileSize >> 8) & 0xFF);
  buf[5] = (BYTE)(lXferFileSize & 0xFF);
  buf[6] = (BYTE)((lXferFileSize >> 24) & 0xFF);
  buf[7] = (BYTE)((lXferFileSize >> 16) & 0xFF);
  buf[8] = (BYTE)filexfer.dart.fFileMode;
  strcpy(&buf[9], StripPath(szXferFilePath));

  if (filexfer.dart.cSeqNo++ == 31)
    filexfer.dart.cSeqNo = 0;

  len = CompressNonData(buf, strlen(&buf[9])+10);
  SendDataPacket('H', filexfer.dart.cSeqNo, buf, len);
  wXferCurrFiles++;

  ResetXferIndicators(FALSE);
  filexfer.dart.cState = ST_HEADER;
}


/*---------------------------------------------------------------------------*/
/*  Response to Header ACK or what we do with the file. 		     */
/*---------------------------------------------------------------------------*/
static void near ProcessHeaderAckPacket()
{
  ULONG ulPos;

  DecompressNonData(&filexfer.dart.sData[8], nXferBlockSize);

  if (filexfer.dart.sData[8] == 0)
    {
    GenerateDataPacket(FALSE);
    }
  else if (filexfer.dart.sData[8] == 1)
    {
    if (wXferCurrFiles < wXferTotalFiles)
      {
      if (filexfer.dart.file != -1)
	DosClose(filexfer.dart.file);
      filexfer.dart.file   = -1;
      lXferFileBytes = 0L;
      if (_dos_findnext(&filexfer.dart.sendfile))
	{
	wXferTotalFiles = 0;
	lXferFileSize  = 0L;
        GenerateBreakPacket();
	}
      else
	{
        filexfer.dart.wFileDate = filexfer.dart.sendfile.wr_date;
        filexfer.dart.wFileTime = filexfer.dart.sendfile.wr_time;
        lXferFileSize = filexfer.dart.sendfile.size;
        strcpy(szXferFilePath, filexfer.dart.sendfile.name);
        GenerateHeaderPacket();
	}
      }
    else
      GenerateBreakPacket();
    }
  else
    {
    lXferFileBytes  = *(long *)(&filexfer.dart.sData[9]);
    DosChgFilePtr(filexfer.dart.file, lXferFileBytes, FILE_BEGIN, &ulPos);
    GenerateDataPacket(TRUE);
    }
}

//
//  GenerateDataPacket
//
//    -- 00021 DART outgoing compression added (coding by Paula)
//
static void near GenerateDataPacket(BOOL fRestart)
{
  ULONG ulPos;
  USHORT usBytes;
  BYTE buf[256];

  //Kent: next change - vars
  int nRptCnt, nCurCh, i, j;


  if (lXferFileBytes >= lXferFileSize)
    {
    GenerateEofPacket();
    if (filexfer.dart.file != -1)
      {
      DosClose(filexfer.dart.file);
      filexfer.dart.file   = -1;
      }
    lXferFileBytes = 0L;

    if (_dos_findnext(&filexfer.dart.sendfile))
      {
      wXferTotalFiles = 0;
      lXferFileSize  = 0L;
      }
    else
      {
      filexfer.dart.wFileDate = filexfer.dart.sendfile.wr_date;
      filexfer.dart.wFileTime = filexfer.dart.sendfile.wr_time;
      lXferFileSize = filexfer.dart.sendfile.size;
      strcpy(szXferFilePath, filexfer.dart.sendfile.name);
      }
    ResetXferIndicators(FALSE);
    return;
    }
  else
    {
    //Kent: I changed this whole ELSE around while adding code:

    usBytes = i = j = 0;
    nRptCnt = 1;
    while (i < (filexfer.dart.nSendPacketSize - 5))
      {
      // read from file
      if (j == usBytes)
        {
	if (DosRead(filexfer.dart.file, buf, sizeof(buf), &usBytes))
          {
          SendCancelPacket(ERRXFR_HOSTCANCEL);
          CancelTransfer(ERRSYS_READFAULT);
          return;
          }

        j = 0;
        }

      if (j < usBytes)
        {
        // grab next char and update checksum
        nCurCh = buf[j++];
        filexfer.dart.filechk += nCurCh;
        lXferFileBytes++;
        dwXferCurrBytes++;

	if (lXferFileBytes == 1 || fRestart)
	  filexfer.dart.nLastCh = nCurCh;

        // check for text mode CR-LFs
        if (filexfer.dart.fFileMode && (nCurCh == 10))
	  {
	  fRestart = FALSE;
	  continue;
	  }

	if (lXferFileBytes == 1 || fRestart)
	  {
	  fRestart = FALSE;
	  continue;
	  }

        // check for repeat chars
	if (((nCurCh & 0x00FF) == filexfer.dart.nLastCh) && (++nRptCnt < 4095))
                        continue;
        }

      // add nLastCh to buffer
      if (nRptCnt > 2)
        {
        // repeat char sequence of 3 or more
        filexfer.dart.sData[i++] = ASC_DLE;
        filexfer.dart.sData[i++] = 'r';
	filexfer.dart.sData[i++] = (char)(((nRptCnt >> 6) & 0x3f) + 0x20);
	filexfer.dart.sData[i++] = (char)((nRptCnt & 0x3f) + 0x20);
        nRptCnt = 1;
        }

      while (nRptCnt-- > 0)
        {
        if ((filexfer.dart.nLastCh & 0x7F) == ASC_SOH ||
            (filexfer.dart.nLastCh & 0x7F) == ASC_DLE ||
            (filexfer.dart.nLastCh & 0x7F) == ASC_XON ||
            (filexfer.dart.nLastCh & 0x7F) == ASC_XOFF)
          {
          filexfer.dart.sData[i++] = ASC_DLE;
	  filexfer.dart.sData[i++] = (char)(filexfer.dart.nLastCh + '@');
          }
        else if ((filexfer.dart.nLastCh == 13) && filexfer.dart.fFileMode)
          {
          filexfer.dart.sData[i++] = ASC_DLE;
          filexfer.dart.sData[i++] = 'm';
          }
        else
	  filexfer.dart.sData[i++] = (char)filexfer.dart.nLastCh;
	};

      // break out at EOF
      if (usBytes == 0) break;

      nRptCnt = 1;
      filexfer.dart.nLastCh = nCurCh;
      }

    // reset file pointer
    if (i >= (filexfer.dart.nSendPacketSize - 5) && usBytes)
      DosChgFilePtr(filexfer.dart.file, lXferFileBytes, FILE_BEGIN, &ulPos);

    //Kent: end of my changes. Paula.

    if (filexfer.dart.cSeqNo++ == 31)
      filexfer.dart.cSeqNo = 0;

    SendDataPacket('D', filexfer.dart.cSeqNo, filexfer.dart.sData, i);
    if (lXferFileBytes >= lXferFileSize)
      SendInfoPacket('Q', filexfer.dart.cSeqNo);

    nXferBlockSize = i;
    wXferBlockCount++;
    }

  filexfer.dart.cState = ST_DATA;
  UpdateXferIndicators();
  }


/*---------------------------------------------------------------------------*/
/*  We just received an INIT packet, lets do something with it. 	     */
/*---------------------------------------------------------------------------*/
static void near GenerateEofPacket()
{
  BYTE buf[32];
  register len;


  buf[0] = (BYTE)(filexfer.dart.filechk >> 8);
  buf[1] = (BYTE)(filexfer.dart.filechk & 0xFF);

  if (filexfer.dart.cSeqNo++ == 31)
    filexfer.dart.cSeqNo = 0;

  len = CompressNonData(buf, 2);
  SendDataPacket('Z', filexfer.dart.cSeqNo, buf, len);

  filexfer.dart.cState = ST_EOF;
}


/*---------------------------------------------------------------------------*/
/*  We just received an INIT packet, lets do something with it. 	     */
/*---------------------------------------------------------------------------*/
static void near GenerateBreakPacket()
{
  if (filexfer.dart.cSeqNo++ == 31)
    filexfer.dart.cSeqNo = 0;
  SendInfoPacket('B', filexfer.dart.cSeqNo);

  filexfer.dart.cState = ST_BREAK;
}


/*---------------------------------------------------------------------------*/
/*  Contruct a small information packet (no data field) and send it out.     */
/*---------------------------------------------------------------------------*/
static void near SendInfoPacket(cType, cSeqNo)
  char cType;
  char cSeqNo;
{
  filexfer.dart.sSend[0] = ASC_SOH;
  filexfer.dart.sSend[1] = 0x20;	    /* No data */
  filexfer.dart.sSend[2] = 0x20;	    /* No data */

  if (cSeqNo != -1)
    filexfer.dart.sSend[3] = cSeqNo + 0x20;
  else
    {
    filexfer.dart.sSend[3] = filexfer.dart.sData[3];
    filexfer.dart.cSeqNo   = filexfer.dart.sData[3] - 0x20;
    }

  filexfer.dart.sSend[4] = cType;
  ComputeCRC(&filexfer.dart.sSend[1], 4);

  filexfer.dart.nSendLen = 8;
  CommDataOutput(filexfer.dart.sSend, filexfer.dart.nSendLen);

  filexfer.dart.cWait = XFER_SND_WAIT;	    /* Wait for data to leave */
}


/*---------------------------------------------------------------------------*/
/*  Contruct a packet containing data and send it out.			     */
/*---------------------------------------------------------------------------*/
static void near SendDataPacket(cType, cSeqNo, pData, nLen)
  char cType;
  char cSeqNo;
  BYTE *pData;
  int  nLen;
{
  filexfer.dart.sSend[0] = ASC_SOH;
  filexfer.dart.sSend[1] = (BYTE)(((nLen >> 5) & 0x1F) + 0x20);
  filexfer.dart.sSend[2] = (BYTE)((nLen & 0x1F) + 0x20);

  if (cSeqNo != -1)
    filexfer.dart.sSend[3] = cSeqNo + 0x20;
  else
    {
    filexfer.dart.sSend[3] = filexfer.dart.sData[3];
    filexfer.dart.cSeqNo   = filexfer.dart.sData[3] - 0x20;
    }

  filexfer.dart.sSend[4] = cType;
  ComputeCRC(&filexfer.dart.sSend[1], 4);

  memcpy(&filexfer.dart.sSend[8], pData, nLen);
  ComputeCRC(&filexfer.dart.sSend[8], nLen);

  filexfer.dart.nSendLen = nLen + 11;
  CommDataOutput(filexfer.dart.sSend, filexfer.dart.nSendLen);

  filexfer.dart.cWait = XFER_SND_WAIT;	    /* Wait for data to leave */
}


/*---------------------------------------------------------------------------*/
/*  Contruct a small information packet (no data field) and send it out.     */
/*---------------------------------------------------------------------------*/
static void near SendCancelPacket(nError)
  int  nError;
{

  filexfer.dart.sSend[ 0] = ASC_SOH;
  filexfer.dart.sSend[ 1] = 0x20;
  filexfer.dart.sSend[ 2] = 0x22;
  filexfer.dart.sSend[ 3] = 0x20;
  filexfer.dart.sSend[ 4] = 'C';
  ComputeCRC(&filexfer.dart.sSend[1], 4);
  filexfer.dart.sSend[ 8] = (BYTE)(nError >> 8);
  filexfer.dart.sSend[ 9] = (BYTE)(nError & 0xFF);
  ComputeCRC(&filexfer.dart.sSend[8], 2);

  filexfer.dart.nSendLen = 14;
  CommDataOutput(filexfer.dart.sSend, filexfer.dart.nSendLen);
}


/*---------------------------------------------------------------------------*/
/*  Resend the last packet transmitted, now you know why I wasted my time in */
/*  the SendInfoPacket and SendDataPacket routines building a nice packet.   */
/*---------------------------------------------------------------------------*/
static void near SendLastPacket()
{
  CommDataOutput(filexfer.dart.sSend, filexfer.dart.nSendLen);

  filexfer.dart.cWait = XFER_SND_WAIT;	    /* Wait for data to leave */
}


/*---------------------------------------------------------------------------*/
/*  Decompress a non-data packet.  Which means the DLE stuff only.	     */
/*---------------------------------------------------------------------------*/
static int near DecompressNonData(pData, nLen)
BYTE *pData;
int nLen;
{
  register i, j;

  for (i = j = 0; i < nLen; i++)
  {
    if (pData[i] == ASC_DLE)
    {
      i++;
      pData[j++] = (BYTE)(pData[i] - 0x040);
    }
    else
      pData[j++] = pData[i];
  }

  return(j);
}


/*---------------------------------------------------------------------------*/
/*  Compress a non-data packet.  Just look for the four control characters   */
/*  and add the DLE.  Since non-data packet are small, will assume that our  */
/*  1k+ buffer has enough room for the growth of adding DLE <grin>.	     */
/*---------------------------------------------------------------------------*/
static int near CompressNonData(pData, nLen)
  BYTE *pData;
  int  nLen;
{
  int  nMore, nNewLen;
  register i;


  for (i = 0, nMore = 0; i < nLen; i++)
    {
    if ((pData[i] & 0x7F) == ASC_SOH || (pData[i] & 0x7F) == ASC_DLE ||
	(pData[i] & 0x7F) == ASC_XON || (pData[i] & 0x7F) == ASC_XOFF)
      nMore++;
    }

  if (nMore == 0)
    return (nLen);
  else
    nNewLen = nLen + nMore;

  for (i = nLen-1; i >= 0; i--)
    {
    if ((pData[i] & 0x7F) == ASC_SOH || (pData[i] & 0x7F) == ASC_DLE ||
	(pData[i] & 0x7F) == ASC_XON || (pData[i] & 0x7F) == ASC_XOFF)
      {
      pData[i+nMore] = (BYTE)(pData[i] + '@');
      nMore--;
      pData[i+nMore] = ASC_DLE;
      if (nMore == 0)
	return (nNewLen);
      }
    else
      {
      pData[i+nMore] = pData[i];
      }
    }
}


/*---------------------------------------------------------------------------*/
/*  Cancel an on-going DART file transfer.                                   */
/*---------------------------------------------------------------------------*/
static void near CancelTransfer(nError)
  int  nError;
{
  USHORT usBytes;
  BYTE buf[16];

  nErrorCode = nError;

  filexfer.dart.cState = ST_FINISH;
  filexfer.dart.cWait  = XFER_SND_WAIT;

  if (!fXferDirection && filexfer.dart.file != -1)
    {
    if (Xfer.fRecovery)
      {
      buf[ 0] = '*';
      buf[ 1] = ' ';
      buf[ 2] = 'P';
      buf[ 3] = 'A';
      buf[ 4] = 'R';
      buf[ 5] = 'T';
      buf[ 6] = 'I';
      buf[ 7] = 'A';
      buf[ 8] = 'L';
      buf[ 9] = ' ';
      buf[10] = '*';
      buf[11] = (BYTE)(0x20 + ((filexfer.dart.wFileDate >> 5) & 0xF));
      buf[12] = (BYTE)(0x20 + (filexfer.dart.wFileDate & 0x1F));
      buf[13] = (BYTE)(0x20 + (filexfer.dart.wFileDate >> 9));
      buf[14] = (BYTE)(0x20 + (filexfer.dart.wFileTime >> 11));
      buf[15] = (BYTE)(0x20 + ((filexfer.dart.wFileTime >> 5) & 0x3F));

      DosWrite(filexfer.dart.file, buf, 16, &usBytes);
      DosClose(filexfer.dart.file);
      filexfer.dart.file = -1;
      if (usBytes != 16)
	DosDelete(szXferFilePath, 0L);
      }
    else
      {
      if (filexfer.dart.file != -1)
	DosClose(filexfer.dart.file);
      filexfer.dart.file = -1;
      if (szXferFilePath[0] && !fXferDirection && !Xfer.fRecovery)
	DosDelete(szXferFilePath, 0L);
      }
    }
}


/*---------------------------------------------------------------------------*/
/*  Compute the CRC for the given string and checks the results with the CRC */
/*  that follows.  This routine will process the DART three-byte sequence    */
/*  without any prior changes.						     */
/*---------------------------------------------------------------------------*/
static BOOL near VerifyCRC(pData, nCount)
  BYTE *pData;
  int  nCount;
{
  register int i, crc;


  crc = 0;
  while (nCount-- > 0)
    {
    crc = crc ^ (int)(*(pData++)) << 8;
    for (i = 0; i < 8; ++i)
      {
      if (crc & 0x8000)
	crc = crc << 1 ^ 0x1021;
      else
	crc = crc << 1;
      }
    }

  if ((BYTE)(*pData++ - 0x20 & 0x1F) != (BYTE)((crc >> 11) & 0x1F))
    return (FALSE);
  if ((BYTE)(*pData++ - 0x20 & 0x3F) != (BYTE)((crc >> 5) & 0x3F))
    return (FALSE);
  if ((BYTE)(*pData - 0x20 & 0x1F) != ((BYTE)crc & 0x1F))
    return (FALSE);

  return (TRUE);
}


/*---------------------------------------------------------------------------*/
/*  Compute the DART three-byte CRC sequence.  The CRC is automatically      */
/*  added to the end of the computed data and in the proper format for	     */
/*  transmission.							     */
/*---------------------------------------------------------------------------*/
static void near ComputeCRC(pData, nCount)
  BYTE *pData;
  int  nCount;
{
  register int crc, i;

  crc = 0;
  while (nCount-- > 0)
    {
    crc = crc ^ (int)(*(pData++)) << 8;
    for (i = 0; i < 8; ++i)
      {
      if (crc & 0x8000)
	crc = crc << 1 ^ 0x1021;
      else
	crc = crc << 1;
      }
    }

  *pData++ = (BYTE)(((crc >> 11) & 0x1F) + 0x20);
  *pData++ = (BYTE)(((crc >> 5) & 0x3F) + 0x20);
  *pData   = (BYTE)((crc & 0x1F) + 0x20);
}


/*---------------------------------------------------------------------------*/
/*  Compute the DART three-byte CRC sequence.  The CRC is automatically      */
/*  added to the end of the computed data and in the proper format for	     */
/*  transmission.							     */
/*---------------------------------------------------------------------------*/
static void near ComputeDataCKSUM(pData, nCount)
  BYTE *pData;
  int  nCount;
{
  while (nCount-- > 0)
    filexfer.dart.filechk += *(pData++);
}
