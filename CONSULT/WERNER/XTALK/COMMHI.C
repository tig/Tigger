/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Communication Support (COMMHI.C)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the key communication routines that are    *
*             used most of the time (thus stays in core alot).                *
*                                                                             *
*  Revisions:																   *
*	1.00 07/17/89 Initial Version.											   *
*	1.01 08/02/89 00001 Fix HST speed change problem.						   *
*	1.01 08/08/89 00007 Change Kermit logic to not use the CR char			   *
*						as an event flag.  Removed code.					   *
*	1.01 08/09/89 00010 Don't display the disconnect message if				   *
*						we don't have the focus (just beep).				   *
*	1.01 09/08/89 00034 Improve error check for external routines.			   *
*	1.01 09/28/89 00039 Improve Scrollback buffer support.					   *
*	1.01 01/18/90 00085 Remove disconnect message.							   *
*	1.1  04/07/90 dca00072 PJL Fix REPLY '~', REPLY '`~' problem.              *                                                               *
*	1.1b 05/15/90 dca00096 PJL Fix PP PM2400SA modem problems.                 *
*	1.1b 05/16/90 dca00096 PJL Fix "Watch Dialing" display.					   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "capture.h"
#include "term.h"
#include "sess.h"
#include "properf.h"
#include "interp.h"
#include "prodata.h"
#include "dialogs.h"
#include "variable.h"


extern HWND hXferDlg;

  struct stComm  Comm;
  struct stModem Modem;

  char	  cCommPort;		/* Current Comm Port */
  int	  nCommID;		/* Communication ID, returned from OpenComm */
  int	  nCommTimerDelay;	/* Timer delay to call CommDataInput again  */
  DCB	  ComDCB;		/* Current DCB information on the Comm Port */
  COMSTAT ComStat;		/* Current status of the Communication Port */
  WORD far *lpCommEvent;
  int	  nCommEvent;
  int	  nCommCount;

  int	  nCommFramingErrors;
  int	  nCommOverrunErrors;
  int	  nCommParityErrors;

  long	  lTimeConnected;
  long    lTimeLastRecv = 0L;
  long	  lTimeInitSent;

  char	  fCommStsRLSD; 	/* The current status of the CD, True = Up  */
  int	  nCommStsTime;
  char	  fCommStsXON;

  int	  hDemoFile;
  char	  fDemoPause;
  char	  fDemoStep;


/*---------------------------------------------------------------------------*/
/* Define Communication File Name Variables				     */
/*---------------------------------------------------------------------------*/
  char sCommFileName[16];
  char sCommWorkName[16];
  int  nCommWorkInx;


/*---------------------------------------------------------------------------*/
/* Define Proteus's Communication States                                     */
/*---------------------------------------------------------------------------*/
  char fCommStateXoff;


  BOOL DlgNoPhone(HWND, unsigned, WORD, LONG);


  BYTE dCommBuffer[512];
  BYTE *pCommBufPtr;
  int  nCommBufCnt;
  char fCommBufErr;


void CmdConnect()
{
	FARPROC lpDlg;
	char buf[64];

	UpdateWindow(hChildStatus);
	UpdateWindow(hChildMacro);

	if (!bConnected)
	{
		if (!IfSystemStatus(SS_SCRIPT) && Sess.szScript[0])
		{
			register USHORT usResults;

			LibEnstore(Sess.szScript, buf, sizeof(buf));
			fCompileOnly = TRUE;
			if (usResults = ExecuteScript(buf))
			{
				GeneralError(usResults);
				return;
			}
		}

		if (!fManualDial)
			LibEnstore(Sess.szNumber, szManualNumber, sizeof(szManualNumber));

		if (szManualNumber[0] == NULL && !Comm.cLocal)
		{
			lpDlg = MakeProcInstance((FARPROC)DlgNoPhone, hWndInst);
			if (!DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NOPHONE), hWnd, lpDlg))
			{
				FreeProcInstance(lpDlg);
				return;
			}
			FreeProcInstance(lpDlg);
		}

		if (CommPortOpen() == 0)
		{
			if (szManualNumber[0] != 0 && !Comm.cLocal)
			{
// dca00096 PJL Take out ModemInit .. move to PRODIAL.C (where it belongs!)
//				Modem.szModemInit[Modem.szModemInit[0]+1] = NULL;
//				CommStrOutput(&Modem.szModemInit[1], TRUE);
				DialNumber();
			}
			else if (Sess.szScript[0] != NULL)
			{
				LibEnstore(Sess.szScript, buf, sizeof(buf));
				if (nErrorCode = ExecuteScript(buf))
					GeneralError(nErrorCode);
			}
		}
		else
		{
			if (!IfSystemStatus(SS_SCRIPT))
				GeneralError(nErrorCode);
		}
	}
	else
	{
		nCommStsTime = 0;
		CommPortClose();
		// dca00096 PJL Are we running a script?
		if (IfSystemStatus(SS_SCRIPT))
		{
			InterpreterStop();
			ExecuteCloseScript();
		}
	 }
}


BOOL KermitConnect()
{
	DCB sDCB;

	if (!bConnected)
		CommPortOpen();

	if (nCommID >= 0)
	{
		GetCommState(nCommID, (DCB FAR *)&sDCB);
		// if (Comm.cDataBits < 8)
		//   {
		//   sDCB.ByteSize = 8;
		//   sDCB.Parity   = NOPARITY;
		//   sDCB.StopBits = ONESTOPBIT;
		//   }
		SetCommState((DCB FAR *)&sDCB);
	}

	return (0);
}


BOOL KermitHangup()
{

	if (bConnected)
		CmdConnect();

	return (!bConnected);
}


void CmdPause()
{
}


void CmdBreak()
{

	CommPortBreak(Comm.nBreakLen);
}


void CommDataStatus()
{
	register i;

	if (!bConnected)
		return;

	i = GetCommEventMask(nCommID, nCommEvent);

	if (i & EV_RLSD)
	{
		if (lTimeInitSent == 0L || lTimeInitSent < GetCurrentTime())
		{
			if (fCommStsRLSD)
			{
				fCommStsRLSD = FALSE;
				if (bConnected && !Comm.cLocal)
					nCommStsTime = 2;
			}
			else
			{
				fCommStsRLSD = TRUE;
				PostMessage(hChildStatus, UM_CONNECT, 0, 0L);
				nCommStsTime = 0;
			}
		}
	}

	i = GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	if (i & (CE_FRAME | CE_OVERRUN | CE_RXOVER))
		fCommBufErr = TRUE;
	if (i & CE_FRAME)
		nCommFramingErrors++;
	if (i & CE_OVERRUN)
		nCommOverrunErrors++;
	if (i & CE_RXPARITY)
		nCommParityErrors++;

	if (ComStat.fXoffHold && (fCommStateXoff == 0))
	{
		fCommStateXoff = 1;
		SendMessage(hChildInfo, UM_SCRNMODE, INFO_XOFFHOLD, 1L);
	}
	else if (!ComStat.fXoffHold && fCommStateXoff == 1)
	{
		fCommStateXoff = 0;
		SendMessage(hChildInfo, UM_SCRNMODE, INFO_XOFFHOLD, 0L);
	}

	if (nCommStsTime)
		if (--nCommStsTime == 0)
			if (bConnected)
			{
				CommPortClose();
				if (IfSystemStatus(SS_SCRIPT))
					nErrorCode = ERRDIA_LOSTCONNECT;
				else
				{
					if (ExecuteCloseScript())
					{
						// 00010  If we don't have the focus then just beep at the user.
						// 00085  Remove the disconnect message.
						// if (fIsActive)
						//   GeneralError(ERRDIA_LOSTCONNECT);
						// else
						//   {
							MessageBeep(0);
							MessageBeep(0);
						//   }
					}
				}
			}
}


int CommDataRead(ptr, len)
char *ptr;
int  len;
{
	register i;

	if (!bConnected)
		return (0);

	i = GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	if (i & (CE_FRAME | CE_OVERRUN | CE_RXOVER))
		fCommBufErr = TRUE;

	if (ComStat.cbInQue == 0)
		return (0);

	if (ComStat.cbInQue < len)
		len = ComStat.cbInQue;

	if ((len = ReadComm(nCommID, ptr, len)) < 0)
		len = -len;

	if (len)
	{
		lTimeLastRecv = GetCurrentTime();

		if (bDisplayOn)
			SendMessage(hChildTerminal, UM_DATA, len, (DWORD)((LPSTR)ptr));

		if (bCaptureOn && Sess.cLogOption == SESSION_RAW)
			WriteRawToCapture(ptr, len);
	}

	return (len);
}


/******************************************************************************/
/*  All data received from the communication port is read here and only here! */
/*  The 'CommDataInput' routine is called either in the main window loop or   */
/*  using WM_TIMER messages, defined by 'bInTimerMode' flag.                  */
/*----------------------------------------------------------------------------*/
/*
void CommDataInput()
{
	char azBuf[196];
	int  nChrCnt;
	int  i;

	if (!bConnected)
		return;

	i = GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	if (i & (CE_FRAME | CE_OVERRUN | CE_RXOVER))
		fCommBufErr = TRUE;

	if (wSystem & SS_SCROLL)
		return;

	if (ComStat.cbInQue > sizeof(azBuf))
		nChrCnt = sizeof(azBuf);
	else
		nChrCnt = ComStat.cbInQue;

	nChrCnt = ReadComm(nCommID, azBuf, nChrCnt);

	if (nChrCnt < 0)
		nChrCnt = -nChrCnt;

	if (nChrCnt)
	{
		if (bCaptureOn && Sess.cLogOption == SESSION_RAW)
			WriteRawToCapture(azBuf, nChrCnt);

		lTimeLastRecv = GetCurrentTime();

		nCommCount += nChrCnt;
		SendMessage(hChildTerminal, UM_DATA, nChrCnt, (DWORD)((LPSTR)azBuf));
	}
}
*/


/*
**  CommDataOutput
**
**  All data sent to the communication port is written here and only here!
*/
void CommDataOutput(azBuf, nChrCnt)
char *azBuf;
int  nChrCnt;
{

	if (bConnected)
	{
		WriteComm(nCommID, azBuf, nChrCnt);
		GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	}

	// 00039 If the user typed an character then turn off scroll mode.
	if (IfSystemStatus(SS_SCROLL) && wSystemState == SS_TERMINAL)
		VssScrollToggle();
}


void CommCharOutput(chBuf)
char chBuf;
{
	char buf[2];

	buf[0] = chBuf;
	buf[1] = 0;
	CommDataOutput(buf, 1);
}


// dca00072 PJL Fix REPLY '~', REPLY '`~' problem.
// dca00097 PJL Fix 'watch dial' display problem.
void CommStrOutput(char *azBuf, BOOL modem_string)
{
	char buf[3];
	char fWatchDialing;

	if (!bConnected)
		return;

	fWatchDialing = (hXferDlg > 0) & GetProfileInt((LPSTR)szAppName, (LPSTR)"WatchDial", 0);

	buf[1] = buf[2] = 0;
	// dca00098 PJL
	while (*azBuf != 0)
	{
		if (modem_string)
			if (*azBuf == '~')
			{
				LONG timeout;
				// delay for half a second
				timeout = GetCurrentTime() + 500;
				while (timeout > GetCurrentTime());
			}
			else
			{
				buf[0] = *azBuf;
				CommDataOutput(buf, 1);
				if (fWatchDialing)
					if ((*azBuf == 13) && (*(azBuf+1) != 10))
					{
						buf[1] = 10;
						SendMessage(hChildTerminal, UM_DATA, 2, (LONG)((LPSTR)buf));
						buf[1] = 0;
					}
					else
						SendMessage(hChildTerminal, UM_DATA, 1, (LONG)((LPSTR)buf));
			}
		else
		{
			buf[0] = *azBuf;
			CommDataOutput(buf, 1);
			if (Term.fEcho && wSystemState == SS_TERMINAL)
				SendMessage(hChildTerminal, UM_DATA, 1, (LONG)((LPSTR)buf));
		}
		++azBuf;
    }
}


int CommPortOpen()
{
	long lTimer;
	char buf[64];
	char tmp[64];

	nErrorCode = 0;

	nCommFramingErrors = 0;
	nCommOverrunErrors = 0;
	nCommParityErrors  = 0;

	fConnectARQ	= FALSE;
	nConnectSpeed = 0;

	lTimeInitSent = 0L;

	if (nCommID >= 0)
		CloseComm(nCommID);

	buf[0] = 'C';
	buf[1] = 'O';
	buf[2] = 'M';
	buf[3] = (char)'1' + cCommPort;
	buf[4] = 0;

	nCommID = OpenComm(buf, nCommPortMemory+64, nCommPortMemory+64);

	if (nCommID < 0)
	{
		nErrorCode = 0x2A00 + -nCommID;
		return (nErrorCode);
	}

	EscapeCommFunction(nCommID, SETDTR);
	EscapeCommFunction(nCommID, SETRTS);
	lTimer = GetCurrentTime() + 50;
	while (lTimer > GetCurrentTime());
	EscapeCommFunction(nCommID, CLRDTR);
	EscapeCommFunction(nCommID, CLRRTS);
	lTimer = GetCurrentTime() + 50;
	while (lTimer > GetCurrentTime());
	EscapeCommFunction(nCommID, SETDTR);
	EscapeCommFunction(nCommID, SETRTS);

	GetCommError(nCommID, (COMSTAT FAR *)&ComStat);

	nCommEvent  = EV_RLSD;
	(WORD far *)lpCommEvent = SetCommEventMask(nCommID, nCommEvent);

	nCommCount = 0;
	fCommStsRLSD = FALSE;
	nCommStsTime = 0;
	fCommStsXON	= FALSE;
	fCommStateXoff = 0;

	SendMessage(hChildInfo, UM_SCRNMODE, INFO_XOFFHOLD, 0L);

	if (szIconText[0] == 'C' && szIconText[1] == 'O' && szIconText[1] == 'M')
		IconTextChanged(NULL);

	bConnected = TRUE;
	nCommBufCnt = 0;

	if (CommPortUpdate())
	{
		CommPortClose();
		return (nErrorCode);
	}

	EnableMenuItem(hWndMenu, CMD_NEW, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_OPEN, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_DIAL, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_BREAK, MF_ENABLED);
	EnableMenuItem(hWndMenu, CMD_SEND, MF_ENABLED);
	EnableMenuItem(hWndMenu, CMD_RECEIVE, MF_ENABLED);
	EnableMenuItem(hWndMenu, CMD_PASTE, MF_ENABLED);
	EnableMenuItem(hWndMenu, CMD_PASTEUPLOAD, MF_ENABLED);

	ChangeMenuMsg(CMD_CONNECT, MSGMNU_DISCONNECT);
	CheckMenuItem(hWndMenu, CMD_CONNECT, MF_CHECKED);

	ResetSystemStatus(0);
	SendMessage(hChildStatus, UM_CONNECT, 0, 0L);

	lTimeInitSent = GetCurrentTime() + 5000L;

	GetCommEventMask(nCommID, nCommEvent);
	return (0);
}


/******************************************************************************/
/*  CommPortUpdate							      */
/*									      */
/*  This routine updates the comm port using the current defaults from the    */
/*  user's profile.                                                           */
/*----------------------------------------------------------------------------*/
int CommPortUpdate()
{
	char buf[8];
	register i;

	nErrorCode = 0;

	if (Comm.cPort != -1)
		if (Comm.cPort != cCommPort)
		{
			cCommPort = Comm.cPort;
			ReadModemParameters(IDS_MODEM_HAYES1200);
			LoadModemParameters();
		}

	buf[0] = 'C';
	buf[1] = 'O';
	buf[2] = 'M';
	buf[3] = (char)'1' + cCommPort;
	buf[4] = 0;
	ChangeMenuText(CMD_COMMUNICATIONS, buf);

	IconTextChanged(NULL);

	if (hChildStatus)
		SendMessage(hChildStatus, UM_CONFIG, 0, 0L);

	if (!bConnected)
		return (0);

	i = GetCommState(nCommID, (DCB FAR *)&ComDCB);
	if (i < 0)
	{
		nErrorCode = 0x2A00 + -i;
		return (nErrorCode);
	}

	if (Modem.cChange && nConnectSpeed)	// 00001 Fix HST speed problem.
		ComDCB.BaudRate = nConnectSpeed;
	else
		ComDCB.BaudRate	= Comm.nSpeed;
	ComDCB.ByteSize		= Comm.cDataBits;
	ComDCB.StopBits		= Comm.cStopBits;
	ComDCB.Parity		= Comm.cParity;
	ComDCB.fBinary		= TRUE;
	ComDCB.fRtsDisable	= FALSE;
// dca00096 PJL Need to check COMM.DRV code ... do we need fParity?
//              (Kent didn't set it...)
//	ComDCB.fParity		= (Comm.cParity != NOPARITY);
// dca00096 PJL Fix port init...don't monitor DSR on RTS/CTS handshaking
	ComDCB.fOutxDsrFlow	= FALSE;
	ComDCB.fDtrDisable	= FALSE;
	ComDCB.fPeChar		= FALSE;
	ComDCB.fNull		= FALSE;
	ComDCB.fChEvt		= FALSE;
	ComDCB.XonLim		= nCommPortMemory / 4;
	ComDCB.XoffLim		= nCommPortMemory / 4;
	if (Comm.cXonChar)
		ComDCB.XonChar	= Comm.cXonChar;
	else
		ComDCB.XonChar	= ASC_XON;
	if (Comm.cXoffChar)
		ComDCB.XoffChar = Comm.cXoffChar;
	else
		ComDCB.XoffChar = ASC_XOFF;

	if (Comm.cFlow == MODEM_XON || Comm.cFlow == MODEM_BOTH)
	{
		ComDCB.fOutX	= TRUE;
		ComDCB.fInX		= TRUE;
		fCommStsXON		= TRUE;
	}
	else
	{
		ComDCB.fOutX	= FALSE;
		ComDCB.fInX		= FALSE;
		fCommStsXON		= FALSE;
	}

	// dca00096 PJL Fix port init...don't monitor DSR on RTS/CTS handshaking
	if (Comm.cFlow == MODEM_RTS || Comm.cFlow == MODEM_BOTH)
	{
		ComDCB.fOutxCtsFlow	= TRUE;
		ComDCB.fRtsflow		= TRUE;
	}
	else
	{
		ComDCB.fOutxCtsFlow	= FALSE;
		ComDCB.fRtsflow		= FALSE;
	}

	SetCommState((DCB FAR *)&ComDCB);
	if (i < 0)
	{
		nErrorCode = 0x2A00 + -i;
		return (nErrorCode);
	}
	return (0);
}


short CommFlushBuffers()
{
	LONG timeout;

	if (nCommID >= 0)
	{
		// flush transmit & receive queues
		FlushComm(nCommID, 0);
		FlushComm(nCommID, 1);

		// dca00096 PJL Check for XOFF, CTS hold
		GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
		if (ComStat.fXoffHold)
		{
			EscapeCommFunction(nCommID, SETXON);
			fCommStateXoff = 0;
			SendMessage(hChildInfo, UM_SCRNMODE, INFO_XOFFHOLD, 0L);
			timeout = GetCurrentTime() + 1000;
			while (timeout > GetCurrentTime());
		}
		if (ComStat.fCtsHold)
		{
			EscapeCommFunction(nCommID, SETRTS);
			timeout = GetCurrentTime() + 1000;
			while (timeout > GetCurrentTime());
		}
	}

	// reset Comm port error counts
	CommReset();

	return (0);
}


/*****************************************************************************\
*  CommPortDump
*
*  If the comm port is openned, read all the byte in the input buffer and
*  display on the terminal emulation window.  Normally used before an update
*  to keep from losing data.
\*****************************************************************************/
void CommPortDump()
{
	BYTE abBuf[64];
	register i, ch;

	i = 0;
	while ((ch = CommGet()) != -1)
	{
		abBuf[i++] = (BYTE)ch;
		if (i >= sizeof(abBuf))
		{
			TermWriteData(abBuf, i);
			i = 0;
		}
	}

	TermWriteData(abBuf, i);
}


// dca00096 PJL Fix busy/no answer...disconnect...redial problem
void ModemReset(void)
{
	LONG timeout;
	char CRbuf[2];

	// send a CR to the modem
	CRbuf[0] = 13;
	CRbuf[1] = 0;
	CommDataOutput(CRbuf, 1);
	timeout = GetCurrentTime() + 500;
	while (timeout > GetCurrentTime());
}


int CommPortClose()
{
	DCB sDCB;
	char cMode;
	register i;

	nErrorCode = 0;

	if (nCommID < 0)
	{
		bConnected = FALSE;
		return (0);
	}

	if (hChildInfo)
		SendMessage(hChildInfo, WM_TIMER, 0, 0L);

	if (bCaptureOn)
		CmdCapture(0, 0);

	// dca00096 PJL Clear COM buffers & disconnect modem
	CommFlushBuffers();
	if (!Comm.cLocal && (Modem.szModemHangup[0] > 0))
	{
		LONG timeout;

		InfoMessage(MSGINF_DISCONNECTING);

		// send Hangup string to the modem
		CommStrOutput(Modem.szModemHangup+1, TRUE);

		// delay, look for 'OK'
		timeout = GetCurrentTime() + 1000;
		while (timeout > GetCurrentTime())
			i = CommGet();

		InfoDisplay(NULL);
	}

	// dca00096 PJL Clear DTR, RTS
	EscapeCommFunction(nCommID, CLRDTR);
	EscapeCommFunction(nCommID, CLRRTS);

	// close COM port
	i = CloseComm(nCommID);
	nCommID = -1;

	bConnected	  = FALSE;
	fCommStsRLSD  = FALSE;
	fCommStsXON	  = FALSE;
	lTimeLastRecv = 0L;

	EnableMenuItem(hWndMenu, CMD_BREAK, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_SEND, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_RECEIVE, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_PASTE, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_PASTEUPLOAD, MF_GRAYED);
	EnableMenuItem(hWndMenu, CMD_DIAL, MF_ENABLED);
	EnableMenuItem(hWndMenu, CMD_OPEN, MF_ENABLED);
	EnableMenuItem(hWndMenu, CMD_NEW, MF_ENABLED);

	ChangeMenuMsg(CMD_CONNECT, MSGMNU_CONNECT);
	CheckMenuItem(hWndMenu, CMD_CONNECT, MF_UNCHECKED);

	if (fConnectARQ)
	{
		fConnectARQ = FALSE;
		SendMessage(hChildStatus, UM_CONFIG, 0, 0L);
	}

	ResetSystemStatus(0);

	if (i < 0)
	{
		nErrorCode = 0x2A00 + -i;
		return (nErrorCode);
	}

	return (0);
}


void CommPortBreak(int nTime)
{

	SetCommBreak(nCommID);
	LibDelay(nTime);
	ClearCommBreak(nCommID);
}


int CommBufFil()
{
	register i;

	nCommBufCnt = 0;

	if (!bConnected)
		return (-1);

	i = GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	if (i & (CE_FRAME | CE_OVERRUN | CE_RXOVER))
		fCommBufErr = TRUE;

	if (ComStat.cbInQue == 0)
		return (-1);

	if (ComStat.cbInQue < sizeof(dCommBuffer))
		nCommBufCnt = ComStat.cbInQue;
	else
		nCommBufCnt = sizeof(dCommBuffer);

	if ((nCommBufCnt = ReadComm(nCommID, dCommBuffer, nCommBufCnt)) < 0)
		nCommBufCnt = -nCommBufCnt;

	if (nCommBufCnt == 0)
		return (-1);

	lTimeLastRecv = GetCurrentTime();

	if (bCaptureOn && Sess.cLogOption == SESSION_RAW && !IfSystemStatus(SS_XFER))
		WriteRawToCapture(dCommBuffer, nCommBufCnt);

	pCommBufPtr = dCommBuffer;

	nCommBufCnt--;

	return (*pCommBufPtr++);
}
