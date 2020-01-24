/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Procedure Moduel (KERMIT1.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module control the Kermit file transfer and processes the  *
*             window messages.                                                *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/08/89 00007 Change Kermit logic to not use the CR char  *
*                                   as an event flag.  Removed code.          *
*             1.01 09/28/89 00040 Permit hiding of the xfer dialog window.    *
* 1.1  04/17/90 dca00086 MKL corrected Info Bar display problem after transfer*
*      is canceled.							      *
* 1.1  04/17/90 dca00091 MKL fixed Kermit not posting error on receiving file *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>	//dca00046 JDB
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "kermit.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "variable.h"
#include "xfer.h"

extern void near RcvYPar();

static void near ResetKermit(BOOL);

static void near InvalidPacket();

static void near SndStart();
static void near SndAbort();
static void near SndCancel();

static void near ErrorMsg();
static void near RcvAbort();
static void near RcvCancel();

       void AbortServer();
static long lResetDelay;
// dca00091 MKL
static int nTimeoutCount= 0;


BOOL DlgKermit(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  HANDLE hMEM, hMenu;
  int	 nDelay, i;
  char	 cType;
  char	 buf[81];

  switch (message)
    {
    case WM_INITDIALOG:
      hXferDlg = hDlg;

      ResetKermit(TRUE);
      nKermitMode = KM_IDLE;
      lResetDelay = 0L;
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_CANCEL:
          if (bKermitNonServer)
            {
	    TermMessage(MSGINF_KERMIT_ABORTED, ERRXFR_LOCALCANCEL);
	    // dca00086 MKL corrected Info Bar display problem after cancel
	    InfoMessage(MSGKRM_PROCESSING);

            if (filexfer.kermit.cMode == WAIT_TO_ABORT || ComStat.cbInQue == 0)
              KermitUserAbort();
            else
              filexfer.kermit.cMode = WAIT_TO_ABORT;
            }
          else
            {
	    // dca00086 MKL corrected Info Bar display problem after cancel
	    InfoMessage(MSGKRM_IDLE);

            filexfer.kermit.bAbortFile  = TRUE;
            filexfer.kermit.bAbortBatch = TRUE;
            nErrorCode = ERRXFR_LOCALCANCEL;
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

    case WM_CLOSE:
      KermitUserAbort();
      break;

    case WM_SETFOCUS:
      SetFocus(hChildInput);
      break;

    case WM_TIMER:
      if (!IsIconic(hWnd))
	{
	if (!IsWindowVisible(hDlg) && ((filexfer.kermit.hFile != -1) || bKermitNonServer))
          {
          if (!fHideXferWindow)
            ShowWindow(hDlg, SW_SHOW);
          }
	}

      if (nKermitMode == KM_NULL)
	return FALSE;

      if (!bConnected && nKermitMode >= KM_BUSY)
	{
	TermKermit();
	return FALSE;
	}

      if (lResetDelay != 0L && GetCurrentTime() > lResetDelay)
	{
	if (nKermitMode != KM_SERVER)
	  nKermitMode = KM_IDLE;
	lResetDelay = 0L;
	}

      if (ComStat.cbInQue + CommSize() != 0) {
	cType = (char)ReceivePacket();
	nTimeoutCount = 0;
      }
      else if (ComStat.cbInQue > KERMIT_MAXBUF)
        cType = 'Q';
      else if (GetCurrentTime() > filexfer.kermit.lTimeOut)
        {
        cType = (char)PacketTimeOut();
        if (cType == -1)
        {
          if (filexfer.kermit.bAbortFile && bKermitNonServer)
            TermKermit();
          else if (filexfer.kermit.cMode != WAIT_FOR_COMMAND) {
	    // dca00091 MKL
	    ++ nTimeoutCount;
            if (nTimeoutCount < filexfer.kermit.nPacketErrors) {
		ReSend();
	    }
            else {
		nTimeoutCount = 0;
		LineToTerminal("Kermit:Receive Timeout");
		TermKermit();
	    }
 	  }
          // dca00091 MKL
	  // break;
        }
	else nTimeoutCount = 0;

        UpdateXferIndicators();
        }
      else
	cType = -1;

      if (cType != -1 && filexfer.kermit.cMode == WAIT_TO_END)
        {
        TermKermit();
        break;
        }

      if (cType == filexfer.kermit.cSndType)	   /* Check for an echo */
	{
	  ReReceive();		/* Trash current input packet and   */
	  cType = -1;		/*   ... wait for another.	    */
	}

      if (cType == 'Q')
	FlushComm(nCommID, 1);

      if ((filexfer.kermit.cRcvSeq == filexfer.kermit.cPrvSeq || cType == 'T' ||
          cType == 'Q' || cType == 'N') && cType != -1 && cType != 'E')
	{
        ++ wXferTotalErrors;
	if (Xfer.cMaxErrors != 0 && ++wXferConsecErrors >= Xfer.cMaxErrors)
	  {
          cType = 'T';
	  }
	else if (cType == 'N' && filexfer.kermit.cRcvSeq == ((filexfer.kermit.cCurSeq+1) & 63))
	  {
          cType = 'Y';
	  }
	else
	  {
	  if (filexfer.kermit.cMode != WAIT_FOR_COMMAND)
	    {
	    ReSend();
	    if (cType == 'N')
	      i = ERRKRM_PACKET_N;
            else if (cType == 'Q')
	      i = ERRKRM_PACKET_Q;
	    else
	      i = ERRKRM_PACKET_U;
            SetDlgItemStr(hDlg, ID_XFER_MSG, i);

	    cType = -1;
	    }
	  }

        UpdateXferIndicators();
	}

      if (cType == 'Y')
        {
        if (filexfer.kermit.sRcvPacket[3] == 'X' && filexfer.kermit.nRcvLen>3)
          filexfer.kermit.bAbortFile = TRUE;
        if (filexfer.kermit.sRcvPacket[3] == 'Z' && filexfer.kermit.nRcvLen>3)
          {
          filexfer.kermit.bAbortFile  = TRUE;
          filexfer.kermit.bAbortBatch = TRUE;
          }
        }

      if (cType != -1)
        {
	switch (filexfer.kermit.cMode)
	  {
	  case WAIT_FOR_COMMAND:
	    switch (cType)
	      {
              case 'C':
              case 'G':
		RcvServerCommand();
		break;

              case 'I':
		RcvInit();
		break;

              case 'R':
		RequestFileCommand();
		break;

              case 'S':
		RcvStart();
		break;

              case 'F':
		RcvFile();
		break;

              case 'X':
		RcvText();
		break;

              case 'B':
		AckPacket();
                if (bKermitNonServer)
                  TermKermit();
                else
                  ResetKermit(FALSE);
		break;

              case 'D':
		break;

              case 'E':
		ErrorMsg();
		break;

              case 'T':
              case 'Q':
		wXferConsecErrors = 0;
		/* Add something here */
		break;

              case 'N':
		break;

	      default:
                sprintf(buf, "Unknown Type %c", cType);
		SetDlgItemText(hDlg, ID_XFER_MSG, buf);
		break;
	      }
	    break;

          case WAIT_TO_ABORT:
            SendPacket('E', 0, 0, NULL);
            nErrorCode = ERRXFR_LOCALCANCEL;
            TermKermit();
            break;

          case WAIT_TO_END:
            TermKermit();
            break;

	  case WAIT_COMMAND_SYNC:
	    switch (cType)
	      {
              case 'Y':
		SendStartAck();
		KermitRemoteMessage(szXferFilePath);
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		ResetKermit(FALSE);
		break;
	      }
	    break;


	  case WAIT_FOR_REPLY:
	    switch (cType)
	      {
              case 'Y':
		ErrorMsg();
		break;

              case 'E':
		ErrorMsg();
		break;

              case 'S':
		RcvStart();
		break;

              case 'F':
		RcvFile();
		break;

              case 'X':
		RcvText();
		break;

	      default:
		TermMessage(MSGINF_KERMIT_INVALID, NULL);
		ResetKermit(FALSE);
		break;
	      }
	    break;

	  case SEND_SEND_START:
	    switch (cType)
	      {
              case 'Y':
		SendStartAck();
		SendHeader();
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		SndAbort();
	      }
	    break;

	  case SEND_FILE_HEADER:
	    switch (cType)
	      {
              case 'Y':
		SendData();
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		SndAbort();
	      }
	    break;

	  case SEND_FILE_DATA:
	    switch (cType)
	      {
              case 'Y':
		SendData();
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		SndAbort();
	      }
	    break;

	  case SEND_END_FILE:
	    switch (cType)
	      {
              case 'Y':
		SendEob();
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		SndAbort();
	      }
	    break;

	  case SEND_END_BATCH:
	    switch (cType)
	      {
              case 'Y':
		ResetKermit(FALSE);
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		SndAbort();
	      }
	    break;

	  case SEND_BREAK:
	    switch (cType)
	      {
              case 'Y':
		ResetKermit(FALSE);
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		SndAbort();
	      }
	    break;

	  case WAIT_SEND_INIT:
	    switch (cType)
	      {
              case 'S':
		RcvStart();
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		RcvAbort();
	      }
	    break;

	  case WAIT_FILE_HEADER:
	    switch (cType)
	      {
              case 'F':
		RcvFile();
		break;

              case 'X':
		RcvText();
		break;

              case 'B':
               AckPacket();
                if (bKermitNonServer)
                  TermKermit();
                else
                  ResetKermit(FALSE);
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		RcvAbort();
	      }
	    break;

	  case WAIT_FILE_DATA:
	    switch (cType)
	      {
              case 'A':
		RcvAttr();
		break;

              case 'D':
		RcvData();
		break;

              case 'Z':
		RcvEOF();
		break;

              case 'E':
		ErrorMsg();
		break;

	      default:
		RcvAbort();
	      }
	    break;

	  case ABORT_SERVER:
	    ResetKermit(TRUE);
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

void KermitUserAbort()
{
  SendPacket('E', 0, 0, NULL);
  nErrorCode = ERRXFR_LOCALCANCEL;
  TermKermit();
}


/*****************************************************************************/
/* ResetKermit() Put XTALK in a Kermit (Client Mode) idle state.	     */
/*****************************************************************************/
static void near ResetKermit(fDisplay)
  BOOL fDisplay;
{
  char buf[8];


  buf[0] = NULL;
  if (nTermChrX != 1)
    LineToTerminal(buf);

  if (filexfer.kermit.fAbortKermit)
    {
    TermKermit();
    return;
    }

  if (!bKermitNonServer)
    {
    if (!fHideXferWindow)
      ShowWindow(hXferDlg, SW_HIDE);
    }

  filexfer.kermit.cMode = WAIT_FOR_COMMAND;
  filexfer.kermit.fExchangedParameters = FALSE;

  if (fDisplay)
    {
    if (bKermitNonServer)
      InfoMessage(MSGKRM_PROCESSING);
    else
      InfoMessage(MSGKRM_IDLE);
    }

  SetDlgItemText(hXferDlg, ID_XFER_MSG, "");

  filexfer.kermit.bAbortFile  = FALSE;
  filexfer.kermit.bAbortBatch = FALSE;
  filexfer.kermit.fTextFlag   = FALSE;

  filexfer.kermit.cPrvSeq = 63;
  filexfer.kermit.cCurSeq = 0;

  lXferFileBytes    = 0L;
  lXferFileSize     = 0L;
  wXferCurrFiles    = 0;
  wXferTotalErrors  = 0;
  wXferConsecErrors = 0;

  if (bKermitNonServer)
    ResetXferIndicators(FALSE);
  else
    ResetXferIndicators(TRUE);

  ReReceive();				/* Initialize port to receive a pkt */

  strcpy(buf, "KCP1");
  buf[3] += cCommPort;
  IconTextChanged(buf);

  if (IsIconic(hWnd) && !(wSystem & SS_SCRIPT))
    {
    MessageBeep(NULL);
    MessageBox(GetFocus(), "Ready for next Command!", szAppName, MB_OK);
    }
  else
    FlashWindow(hXferDlg, FALSE);

  lResetDelay = GetCurrentTime() + 1000L;
}


static void near InvalidPacket()
{
  char buf[96];

  wXferTotalErrors++;
  if (Xfer.cMaxErrors != 0 && ++wXferConsecErrors >= Xfer.cMaxErrors)
    {
    SendPacket('E', 0, 0, NULL);
    ResetKermit(TRUE);
    nErrorCode = ERRXFR_TOOMANYERRORS;
    }

  ReSend();

  SetDlgItemStr(hXferDlg, ID_XFER_MSG, IDS_ERRKRM + (ERRKRM_PACKET_Q & 0xFF));

  UpdateXferIndicators();
}



/*****************************************************************************/
/* AbortServer() Abort the server and go to aborted server state.	     */
/*****************************************************************************/
void AbortServer()
{
  SendPacket('E', 0, 0, NULL);
  ResetKermit(TRUE);
  nErrorCode = ERRXFR_LOCALCANCEL;
}

static void near SndAbort()
  {
  SendPacket('E', 0, 0, NULL);
  SndCancel();
  }

static void near SndCancel()
  {
  nErrorCode = ERRXFR_TOOMANYERRORS;
  if (bKermitNonServer)
    TermKermit();
  else
    ResetKermit(TRUE);
  }

static void near RcvAbort()
{
  RcvCancel();
  nErrorCode = ERRXFR_TOOMANYERRORS;
}


static void near RcvCancel()
  {
  KermitClose();

  if (bKermitNonServer)
    TermKermit();
  else
    ResetKermit(TRUE);
  }


static void near ErrorMsg()
{
  FlushComm(nCommID, 0);
  FlushComm(nCommID, 1);

  if (*(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset))
    LineToTerminal(filexfer.kermit.sRcvPacket+filexfer.kermit.nRcvDataOffset);
  if (bKermitNonServer)
    {
    filexfer.kermit.fAbortKermit = TRUE;
    TermMessage(MSGINF_KERMIT_ABORTED, NULL);
    }
  ResetKermit(FALSE);
}
