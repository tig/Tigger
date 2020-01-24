/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   ZMODEM File Transfer (ZMODEM.C)                                 *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:																   *
* 1.00 07/17/89 Initial Version.                                  		   	   *
* 1.01 09/05/89 00023    ZFILE flag bits in the wrong order.         		   *
* 1.01 10/26/89 00050    Prevent buffer overflow to prevent lockups. 		   *
* 1.01 11/22/89 00061    Process ZSINIT message for GEnie.           		   *
* 1.01 01/12/90 00079    Lockup problem with error condition.        		   *
* 1.1  03/12/90 dca00045 PJL ZMODEM download file recovery added.        	   *
* 1.1  03/13/90 dca00047 JDB Make crctab a public array	             	   *
* 1.1  03/14/90 dca00050 PJL For file recovery, add date/time conversion	   *
*                        routines - C routines not reliable enough.		   *
* 1.1  03/26/90 dca00055 PJL Add code to check Transmit buffer limit.		   *
* 1.1  03/28/90 dca00058 PJL Fixed X4 <-> Windx transfer problems.		   	   *
* 1.1  04/06/90 dca00071 PJL Fixed error reporting on OpenFile calls.  	   *
* 1.1  04/11/90 dca00080 PJL More error reporting fixes.					   *
*																			   *
******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>
#include <string.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "properf.h"
#include "xfer.h"
#include "dialogs.h"
#include "global.h"
#include "variable.h"

#define ZPAD '*'        /* 052 Padding character begins frames */
#define ZDLE 030	/* Ctrl-X Zmodem escape - `ala BISYNC DLE */
#define ZDLEE (ZDLE^0100)	/* Escaped ZDLE as transmitted */
#define ZBIN 'A'        /* Binary frame indicator */
#define ZHEX 'B'        /* HEX frame indicator */
#define ZBIN32 'C'      /* Binary frame with 32 bit FCS */

/* Frame types (see array "frametypes" in zm.c) */
#define ZRQINIT 0	/* Request receive init */
#define ZRINIT	1	/* Receive init */
#define ZSINIT 2	/* Send init sequence (optional) */
#define ZACK 3		/* ACK to above */
#define ZFILE 4 	/* File name from sender */
#define ZSKIP 5 	/* To sender: skip this file */
#define ZNAK 6		/* Last packet was garbled */
#define ZABORT 7	/* Abort batch transfers */
#define ZFIN 8		/* Finish session */
#define ZRPOS 9 	/* Resume data trans at this position */
#define ZDATA 10	/* Data packet(s) follow */
#define ZEOF 11 	/* End of file */
#define ZFERR 12	/* Fatal Read or Write error Detected */
#define ZCRC 13 	/* Request for file CRC and response */
#define ZCHALLENGE 14   /* Receiver's Challenge */
#define ZCOMPL 15	/* Request is complete */
#define ZCAN 16 	/* Other end canned session with CAN*5 */
#define ZFREECNT 17	/* Request for free bytes on filesystem */
#define ZCOMMAND 18	/* Command from sending program */
#define ZSTDERR 19	/* Output to standard error, data follows */

/* ZDLE sequences */
#define ZCRCE 'h'       /* CRC next, frame ends, header packet follows */
#define ZCRCG 'i'       /* CRC next, frame continues nonstop */
#define ZCRCQ 'j'       /* CRC next, frame continues, ZACK expected */
#define ZCRCW 'k'       /* CRC next, ZACK expected, end of frame */
#define ZRUB0 'l'       /* Translate to rubout 0177 */
#define ZRUB1 'm'       /* Translate to rubout 0377 */

/* zdlread return values (internal) */
/* -1 is general error, -2 is timeout */
#define GOTOR 0400
#define GOTCRCE (ZCRCE|GOTOR)	/* ZDLE-ZCRCE received */
#define GOTCRCG (ZCRCG|GOTOR)	/* ZDLE-ZCRCG received */
#define GOTCRCQ (ZCRCQ|GOTOR)	/* ZDLE-ZCRCQ received */
#define GOTCRCW (ZCRCW|GOTOR)	/* ZDLE-ZCRCW received */
#define GOTCAN	(GOTOR|030)	/* CAN*5 seen */

/* Byte positions within header array */
#define ZF0	3	/* First flags byte */
#define ZF1	2
#define ZF2	1
#define ZF3	0
#define ZP0	0	/* Low order 8 bits of position */
#define ZP1	1
#define ZP2	2
#define ZP3	3	/* High order 8 bits of file position */

/* Bit Masks for ZRINIT flags byte ZF0 */
#define CANFDX	01	/* Rx can send and receive true FDX */
#define CANOVIO 02	/* Rx can receive data during disk I/O */
#define CANBRK	04	/* Rx can send a break signal */
#define CANCRY	010	/* Receiver can decrypt */
#define CANLZW	020	/* Receiver can uncompress */
#define CANFC32 040	/* Receiver can use 32 bit Frame Check */
#define ESCCTL 0100	/* Receiver expects ctl chars to be escaped */
#define ESC8   0200	/* Receiver expects 8th bit to be escaped */

/* Parameters for ZSINIT frame */
#define ZATTNLEN 32	/* Max length of attention string */
/* Bit Masks for ZSINIT flags byte ZF0 */
#define TESCCTL 0100	/* Transmitter expects ctl chars to be escaped */
#define TESC8	0200	/* Transmitter expects 8th bit to be escaped */

/* Parameters for ZFILE frame */
/* Conversion options one of these in ZF0 */
#define ZCBIN	1	/* Binary transfer - inhibit conversion */
#define ZCNL	2	/* Convert NL to local end of line convention */
#define ZCRESUM 3	/* Resume interrupted file transfer */
/* Management include options, one of these ored in ZF1 */
#define ZMSKNOLOC	0200	/* Skip file if not present at rx */
/* Management options, one of these ored in ZF1 */
#define ZMMASK	037	/* Mask for the choices below */
#define ZMNEWL	1	/* Transfer if source newer or longer */
#define ZMCRC	2	/* Transfer if different file CRC or length */
#define ZMAPND	3	/* Append contents to existing file (if any) */
#define ZMCLOB	4	/* Replace existing file */
#define ZMNEW	5	/* Transfer if source newer */
	/* Number 5 is alive ... */
#define ZMDIFF	6	/* Transfer if dates or lengths different */
#define ZMPROT	7	/* Protect destination file */
/* Transport options, one of these in ZF2 */
#define ZTLZW	1	/* Lempel-Ziv compression */
#define ZTCRYPT 2	/* Encryption */
#define ZTRLE	3	/* Run Length encoding */
/* Extended options for ZF3, bit encoded */
#define ZXSPARS 64	/* Encoding for sparse file operations */

/* Parameters for ZCOMMAND frame ZF0 (otherwise 0) */
#define ZCACK1	1	/* Acknowledge, then do command */


#define SENT_INIT	1
#define SENT_FILE	2
#define SENT_DATA	3
#define SENT_ZFIN	4

#define RECV_INIT	5
#define RECV_FILE	6
#define RECV_DATA	7
#define RECV_ZFIN	8


#define PHASE_SYNC	1
#define PHASE_PAD	2
#define PHASE_TYPE	3
#define PHASE_BINARY	4
#define PHASE_HEX	5
#define PHASE_ZFIN	6

// dca00050 PJL For file recovery date/time conversion.
static int Month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


BOOL DlgZmodem(HWND, unsigned, WORD, LONG);

static int  near XferZmodemInit();
static void near XferZmodemTerm(int);
static void near XferZmodemDone(void);
static void near SendBinaryHeader(int, char *);
static void near SendHexHeader(int, char *);
static void near SendBinaryData(char *, int, int);
static BOOL near VerifyBlk(char *, int);
static void near ReadAnyHeader();
static int  near BuildAnyHeader();
static void near ReadBinaryData();
static int  near BuildBinaryData();
static void near SendRecvInitHeader();
static void near RecvSendSendInit();
static void near RecvSendInitData();
static void near RecvSendFileHeader();
static BOOL near RecvSendFileData();
static void near RecvSendDataHeader();
static BOOL near RecvSendDataData();
static void near RecvSendAck();
static void near RecvSendNak();
static void near RecvSendPos();
static BOOL near SendRecvNak();
static void near SendRecvFin();
static void near ProcessReceiverInit(void);
static void near SendSendInitHeader(void);
static BOOL near SendSendFileHeader(void);
static void near ProcessFilePos(void);
static BOOL near SendSendFileData(void);
static void near SendEofPacket(void);
static void near SendSendFin(void);
static void near SendAbortString(void);


// dca00045 PJL (Added this routine while I was in here)
void ZmodemCloseFile(void)
{

	if (filexfer.zmodem.file == -1)
		return;

	if (!fXferDirection)
		if (filexfer.zmodem.wFileDate && filexfer.zmodem.wFileTime)
			_dos_setftime(filexfer.zmodem.file, filexfer.zmodem.wFileDate,
							filexfer.zmodem.wFileTime);

	DosClose(filexfer.zmodem.file);
	filexfer.zmodem.file = -1;
}


// dca00055 PJL. Check transmit buffer before sending chars out the port.
void SendCommChars(char *buf, int len)
{

	GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	while (ComStat.cbOutQue+len > nCommPortMemory)
	{
		LibDelay(100);
		GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
	}
	CommDataOutput(buf, len);
	GetCommError(nCommID, (COMSTAT FAR *)&ComStat);
}


// dca00080 PJL Use Consec (not Max) error count.
BOOL UpdateErrorCount()
{

	wXferConsecErrors++;
	wXferTotalErrors++;
	UpdateXferIndicators();
	if (Xfer.cMaxErrors != 0 && wXferConsecErrors > Xfer.cMaxErrors)
	{
		XferZmodemTerm(ERRXFR_TOOMANYERRORS);
		return(FALSE);
	}
	return(TRUE);
}


int XferZmodemSendName(filename)
  char *filename;
{
  FILESTATUS fsts;
  static short sInZmodemSendLogic = 0;
  HANDLE   file;
  register i;

  if (sInZmodemSendLogic)
    return (0);

  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  memset(&filexfer.zmodem, 0, sizeof(filexfer.zmodem));

  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendAbortString();
    return (i);
    }

  sInZmodemSendLogic++;
  if (filename == NULL || *filename == NULL)
    {
    if (XferUploadBatch("\\*.*", szXferFilePath))
      {
      sInZmodemSendLogic--;
      SendAbortString();
      return (ERRSYS_OPENFAULT);
      }
    }
  else
    strcpy(szXferFilePath, filename);

  if (ComputeFilesToSend(szXferFilePath, &filexfer.zmodem.sendfile) == 0)
    {
    sInZmodemSendLogic--;
    SendAbortString();
    return (ERRXFR_NOFILEFOUND);
    }

  if ((file = OpenXferFile(filexfer.zmodem.sendfile.name)) == -1)
    {
    sInZmodemSendLogic--;
    return (ERRSYS_OPENFAULT);
    }

  strcpy(szXferFilePath, filexfer.zmodem.sendfile.name);

  XferZmodemInit();

  fXferDirection         = TRUE;
  filexfer.zmodem.cState = SENT_INIT;
  filexfer.zmodem.file   = file;
  lXferFileBytes         = 0L;

  DosQFileInfo(file, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;

  nXferBlockSize = Xfer.nPacketSize;

  lpXferDlg = MakeProcInstance((FARPROC)DlgZmodem, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  sInZmodemSendLogic--;
  return (NULL);
}


int XferZmodemRecvName(filename)
char *filename;
{
  int	   file;
  char	   buf[128];
  register int i;

  if (wSystemState == SS_PROCESS)
  {
    SendAbortString();
    return (ERRXFR_BUSY);
  }

  memset(&filexfer.zmodem, 0, sizeof(filexfer.zmodem));

  if (i = SetFullPath(VAR_DIRFIL))
  {
    SendAbortString();
    return (i);
  }

  if (filename == NULL || *filename == NULL)
  {
    file = -1;
    szXferFilePath[0] = NULL;
  }
  else
  {
    if (nErrorCode = CreateXferFile(filename, &file))
    {
      SendAbortString();
      return(nErrorCode);
    }
    strcpy(szXferFilePath, StripPath(filename));
  }

  if ((i = XferZmodemInit()) != NULL)
  {
    if (file != -1)
    {
      DosClose(file);
      DosDelete(buf, 0L);
    }

    return (i);
  }

  fXferDirection    = FALSE;
  filexfer.zmodem.cState = RECV_INIT;
  filexfer.zmodem.file = file;
  wXferCurrFiles    = 0;
  lXferFileBytes    = 0L;
  lXferFileSize     = 0L;
  wXferTotalFiles   = 0;

  lpXferDlg = MakeProcInstance((FARPROC)DlgZmodem, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  if (!IsIconic(hWnd))
    ShowWindow(hXferDlg, SW_SHOW);

  return (NULL);
}


static int near XferZmodemInit()
{
	DCB	 sDCB;
	register i;

	// dca00080 PJL Use Consec (not Max) error count.
	wXferBlockCount = wXferTotalErrors = wXferConsecErrors = 0;

	GetCommState(nCommID, (DCB FAR *)&sDCB);
	if (Comm.cDataBits < 8)
	{
		sDCB.ByteSize = 8;
		sDCB.Parity   = NOPARITY;
		sDCB.StopBits = ONESTOPBIT;
	}

	sDCB.fInX  = TRUE;
	sDCB.fOutX = TRUE;

	SetCommState((DCB FAR *)&sDCB);

	FlushComm(nCommID, 0);
	FlushComm(nCommID, 1);

	for (i = 0; i < 12; i++)
	{
		filexfer.zmodem.szAttn[i]	 = ASC_CAN;
		filexfer.zmodem.szAttn[i+12] = ASC_BS;
	}
	filexfer.zmodem.szAttn[24] = '\0';

	return (NULL);
}


static void near XferZmodemTerm(results)
int results;
{
  char buf[24];
  register i, j;

  j = results;

  CommGet();
  CommGet();

  if (results)
  {
    FlushComm(nCommID, 0);
    FlushComm(nCommID, 1);

    // 00061 Process ZSINIT packet and Attn sequence
    SendCommChars(filexfer.zmodem.szAttn, strlen(filexfer.zmodem.szAttn));
    LibDelay(600);
  }
  else if (fXferDirection)
  {
    buf[0] = 'O';
    buf[1] = 'O';
    SendCommChars(buf, 2);
    LibDelay(200);
  }

  CommPortUpdate();

	ZmodemCloseFile();

  if (!fXferDirection && (j == ERRXFR_LOCALCANCEL || j == ERRXFR_TOOMANYERRORS ||
      j == ERRDOS_DISKFULL))
  {
    filexfer.zmodem.faborting = TRUE;
    filexfer.zmodem.errorcode = j;
    return;
  }

  ResetSystemStatus(SS_XFER);
  SystemUpdate();

  IconTextChanged(NULL);
  InfoDisplay(NULL);

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

  nErrorCode = j;

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


// 00061 Process ZSINIT packet and Attn sequence
static void near XferZmodemDone()
{

  ResetSystemStatus(SS_XFER);
  SystemUpdate();

  IconTextChanged(NULL);
  InfoDisplay(NULL);

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

  nErrorCode = filexfer.zmodem.errorcode;

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


BOOL DlgZmodem(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
	char buf[64];

	switch (message)
	{
	case WM_INITDIALOG:
		nErrorCode = 0;

		hXferDlg = hDlg;

		switch (Xfer.cTiming)
		{
		case XFER_TIMING_TIGHT:
			filexfer.zmodem.nTimeOut = 4000;
			break;

		case XFER_TIMING_NORMAL:
			filexfer.zmodem.nTimeOut = 10000;
			break;

		case XFER_TIMING_LOOSE:
			filexfer.zmodem.nTimeOut = 15000;
			break;

		default:
			filexfer.zmodem.nTimeOut = 30000;
			break;
		}

		Xfer.cProtocol = XFER_ZMODEM;
		InitXferIndicators();

		if (fXferDirection)
		{
			InfoDisplayMsg(MSGXFR_ZMODEMSND);
			filexfer.zmodem.lTimeOut = GetCurrentTime() + filexfer.zmodem.nTimeOut;
			SendSendInitHeader();
			ReadAnyHeader();
		}
		else
		{
			InfoDisplayMsg(MSGXFR_ZMODEMRCV);
			filexfer.zmodem.lTimeOut = GetCurrentTime() + filexfer.zmodem.nTimeOut;
			SendRecvInitHeader();
			ReadAnyHeader();
		}

		UpdateXferIndicators();
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_CANCEL:
			XferZmodemTerm(ERRXFR_LOCALCANCEL);
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
			XferZmodemTerm(ERRXFR_LOSTCONNECT);
			return (0);
		}

		if (filexfer.zmodem.faborting)
		{
			register i;

			if ((CommGet()) == -1)
			{
				if (filexfer.zmodem.delay++ > 10)
					XferZmodemDone();
			}
			else
			{
				filexfer.zmodem.delay = 0;
				while ((CommGet()) != -1);
			}
			break;
		}

		// if receiving
		if (!fXferDirection)
		{
			register i;

			if (filexfer.zmodem.fNextIsHeader)
				i = BuildAnyHeader();
			else
				i = BuildBinaryData();

			if (i != -1)
			{
				if (i == 'T')
					wXferLastError = XFER_ERROR_TIMEOUT;
				else
					wXferLastError = XFER_ERROR_GENERAL;

				switch (filexfer.zmodem.cState)
				{
				case RECV_INIT:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZRQINIT:
							SendRecvInitHeader();
							ReadAnyHeader();
							break;

						case ZSINIT:
							RecvSendSendInit();
							ReadBinaryData();
							break;

						case ZFILE:
							RecvSendFileHeader();
							ReadBinaryData();
							filexfer.zmodem.cState = RECV_FILE;
							break;

						case ZNAK:
							if (SendRecvNak())
								ReadAnyHeader();
							break;

						case ZFIN:
							SendRecvFin();
							ReadAnyHeader();
							filexfer.zmodem.cState = RECV_ZFIN;
							break;

						default:
							ReadAnyHeader();
							break;
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					case 'D':
						RecvSendInitData();
						ReadAnyHeader();
						break;

					default:
						if (UpdateErrorCount())
						{
							SendRecvInitHeader();
							ReadAnyHeader();
						}
						break;
					}
					break;

				case RECV_FILE:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZFILE:
							RecvSendFileHeader();
							ReadBinaryData();
							break;

						case ZDATA:
							RecvSendDataHeader();
							filexfer.zmodem.cState = RECV_DATA;
							break;

						case ZNAK:
							if (SendRecvNak())
								ReadAnyHeader();
							break;

						case ZEOF:
							ZmodemCloseFile();
							SendRecvInitHeader();
							ReadAnyHeader();
							filexfer.zmodem.cState = RECV_INIT;
							break;

						default:
							ReadAnyHeader();
						}
						break;

					case 'D':
						switch (filexfer.zmodem.sData[filexfer.zmodem.nLength-1])
						{
						case ZCRCE:
							if (RecvSendFileData())
								ReadAnyHeader();
							break;

						case ZCRCG:
							if (RecvSendFileData())
								ReadBinaryData();
							break;

						case ZCRCQ:
							if (RecvSendFileData())
								ReadBinaryData();
							break;

						case ZCRCW:
							if (RecvSendFileData())
								ReadAnyHeader();
							break;
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					default:
						if (UpdateErrorCount())
						{
							RecvSendNak();
							ReadAnyHeader();
						}
						break;
					}
					break;

				case RECV_DATA:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZDATA:
							RecvSendDataHeader();
							break;

						// dca00058 PJL Fix X4 <-> Windx transfer problems.		   	   *
						case ZEOF:
							ZmodemCloseFile();
							SendRecvInitHeader();
							ReadAnyHeader();
							filexfer.zmodem.cState = RECV_INIT;
							break;

						case ZNAK:
							if (SendRecvNak())
								ReadAnyHeader();
							break;

						default:
							ReadAnyHeader();
							break;
						}
						break;

					case 'D':
						switch (filexfer.zmodem.sData[filexfer.zmodem.nLength-1])
						{
						case ZCRCE:
							if (RecvSendDataData())
								ReadAnyHeader();
							break;

						case ZCRCG:
							if (RecvSendDataData())
								ReadBinaryData();
							break;

						case ZCRCQ:
							if (RecvSendDataData())
							{
								RecvSendAck();
								ReadBinaryData();
							}
							break;

						case ZCRCW:
							if (RecvSendDataData())
							{
								RecvSendAck();
								ReadAnyHeader();
							}
							break;

						default:
							RecvSendPos();
							ReadAnyHeader();
							break;
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					default:
						if (UpdateErrorCount())
						{
							RecvSendPos();
							ReadAnyHeader();
						}
						break;
					}
					break;

				case RECV_ZFIN:
					XferZmodemTerm(0);
					break;
				}
			}
		}
		// else if sending ...
		else
		{
			register i;

			if (filexfer.zmodem.fNextIsHeader)
				i = BuildAnyHeader();
			else
				i = BuildBinaryData();

			if (i == -1)
			{
				if (filexfer.zmodem.fStreaming)
					SendSendFileData();
			}
			else
			{
				if (i == 'T')
					wXferLastError = XFER_ERROR_TIMEOUT;
				else
					wXferLastError = XFER_ERROR_GENERAL;

				switch (filexfer.zmodem.cState)
				{
				case SENT_INIT:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZRINIT:
							ProcessReceiverInit();
							if (SendSendFileHeader())
								ReadAnyHeader();
							break;

						case ZNAK:
							if (SendRecvNak())
								ReadAnyHeader();
							break;

						default:
							ReadAnyHeader();
							break;
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					default:
						if (UpdateErrorCount())
						{
							SendSendInitHeader();
							ReadAnyHeader();
						}
						break;
					}
					break;

				case SENT_FILE:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZRPOS:
							ProcessFilePos();
							if (SendSendFileData())
								ReadAnyHeader();
							break;

						case ZACK:
							SendSendFileData();
							break;

						case ZNAK:
							if (SendRecvNak())
								ReadAnyHeader();
							break;

						// dca00058 PJL Fix X4 <-> Windx transfer problems.		   	   *
						case ZSKIP:
							lXferFileBytes = lXferFileSize;
							UpdateXferIndicators();
							SendEofPacket();
							ReadAnyHeader();
							break;

						default:
							ReadAnyHeader();
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					default:
						if (UpdateErrorCount())
						{
							RecvSendNak();
							ReadAnyHeader();
						}
						break;
					}
					break;

				case SENT_DATA:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZRPOS:
							ProcessFilePos();
							if (SendSendFileData())
								ReadAnyHeader();
							break;

						case ZACK:
							if (SendSendFileData())
								ReadAnyHeader();
							break;

						case ZNAK:
							ReadAnyHeader();
							break;

						default:
							ReadAnyHeader();
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					default:
						if (UpdateErrorCount())
						{
							RecvSendNak();
							ReadAnyHeader();
						}
						break;
					}
					break;

				case SENT_ZFIN:
					switch (i)
					{
					case 'H':
						switch (filexfer.zmodem.sData[0])
						{
						case ZFIN:
							XferZmodemTerm(0);
							break;

						case ZRINIT:
							ProcessReceiverInit();
							if (SendSendFileHeader())
								ReadAnyHeader();
							break;

						case ZACK:
							SendSendFin();
							ReadAnyHeader();
							break;

						default:
							ReadAnyHeader();
						}
						break;

					case 'C':
						XferZmodemTerm(ERRXFR_HOSTCANCEL);
						break;

					default:
						if (UpdateErrorCount())
						{
							RecvSendNak();
							ReadAnyHeader();
						}
						break;
					}
					break;
				}
			}
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}


static BOOL near VerifyBlk(azBuf, nCount)
char *azBuf;
int  nCount;
{
	int i, crc;
	unsigned int	j;
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

	if ((USHORT)(*(azBuf++) & 0x00FF) != (USHORT)(((crc & 0xFF00) >> 8) & 0x00FF))
		return FALSE;

	if (((USHORT)(*azBuf) & 0x00FF) != (USHORT)(crc & 0x00FF))
		return FALSE;

	return TRUE;
}


/*---------------------------------------------------------------------------*/
/*  ZMODEM CRC Subroutine						     */
/*									     */
/*									     */
/*---------------------------------------------------------------------------*/

//dca00047 JDB Needed this array as public
//dca00047 JDB So I removed the static attribute
//dca00047 static unsigned short crctab[256] = {
unsigned short crctab[256] = {
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

#define updcrc(cp, crc) (crctab[((crc >> 8) & 255)] ^ (crc << 8) ^ cp)



/*---------------------------------------------------------------------------*/
/*  ZMODEM Receiving Subroutines					     */
/*									     */
/*									     */
/*---------------------------------------------------------------------------*/

static void near ReadAnyHeader()
{

	filexfer.zmodem.nChrCnt = 0;
	filexfer.zmodem.nCanCnt = 0;
	filexfer.zmodem.fEscSeq = FALSE;
	filexfer.zmodem.cPhase = PHASE_SYNC;
	filexfer.zmodem.fNextIsHeader = TRUE;

	filexfer.zmodem.lTimeOut = GetCurrentTime() + filexfer.zmodem.nTimeOut;
}


static int near BuildAnyHeader()
{
	register c;

	while ((c = CommGet()) != -1)
	{
		if (CommError())
			return ('E');

		switch (filexfer.zmodem.cPhase)
		{
		case PHASE_SYNC:
	switch (c)
	  {
          case 'O':
	    if (filexfer.zmodem.cState == RECV_ZFIN)
              return ('Z');
	    filexfer.zmodem.nCanCnt = 0;
	    break;

	  case ZPAD | 0200:
	  case ZPAD:
	    filexfer.zmodem.nCanCnt = 0;
	    filexfer.zmodem.cPhase = PHASE_PAD;
	    break;

	  case ZDLE:
            if (filexfer.zmodem.nCanCnt++ > 5)
              return ('C');
	    break;

	  case ZCRCW:
	    if (filexfer.zmodem.nCanCnt)
              {
              return ('E');
              }

	  default:
	    filexfer.zmodem.nCanCnt = 0;
	  }
	break;

      case PHASE_PAD:
	switch (c)
	  {
	  case ZPAD:
	    break;

	  case ZDLE:
	    filexfer.zmodem.cPhase = PHASE_TYPE;
	    break;

	  default:
	    filexfer.zmodem.nCanCnt = 0;
	    filexfer.zmodem.cPhase = PHASE_SYNC;
	  }
	break;

      case PHASE_TYPE:
	switch (c)
	  {
	  case ZBIN:
	    filexfer.zmodem.fEscSeq = FALSE;
	    filexfer.zmodem.cPhase = PHASE_BINARY;
	    break;

	  case ZHEX:
	    filexfer.zmodem.fEscSeq = FALSE;
	    filexfer.zmodem.cPhase = PHASE_HEX;
	    break;

	  case ZDLE:
	    filexfer.zmodem.nCanCnt = 1;
	    filexfer.zmodem.cPhase = PHASE_SYNC;
	    break;

	  default:
	    filexfer.zmodem.nCanCnt = 0;
	    filexfer.zmodem.cPhase = PHASE_SYNC;
	  }
	break;

      case PHASE_BINARY:
	if (!filexfer.zmodem.fEscSeq)
	  {
	  switch (c)
	    {
	    case 021:
	    case 0221:
	    case 023:
	    case 0223:
	      break;

	    case ZDLE:
	      filexfer.zmodem.nCanCnt = 1;
	      filexfer.zmodem.fEscSeq = TRUE;
	      break;

	    default:
	      if (!filexfer.zmodem.fCtlEsc || (c & 0140))
		{
		// 00050 Prevent buffer overflow and lockups.
		if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
		  filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = (char)c;
		}
	      break;
	    }
	  }
	else
	  {
	  switch (c)
	    {
	    case ZDLE:
	      if (filexfer.zmodem.nCanCnt++ >= 4)
                return ('C');
	      break;

	    case ZCRCE:
	    case ZCRCG:
	    case ZCRCQ:
	    case ZCRCW:
              return ('E');
	      break;

	    case ZRUB0:
	      // 00050 Prevent buffer overflow and lockups.
	      if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
		filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = 0177;
	      filexfer.zmodem.fEscSeq = FALSE;
	      break;

	    case ZRUB1:
	      // 00050 Prevent buffer overflow and lockups.
	      if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
		filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = 0377;
	      filexfer.zmodem.fEscSeq = FALSE;
	      break;

	    case 021:
	    case 0221:
	    case 023:
	    case 0223:
	      break;

	    default:
	      if (!filexfer.zmodem.fCtlEsc || (c & 0140))
		{
		if ((c & 0140) == 0100)
		  {
		  // 00050 Prevent buffer overflow and lockups.
		  if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
		    filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = (char)(c ^ 0100);
		  filexfer.zmodem.fEscSeq = FALSE;
		  }
                else
                  {
                  return ('E');
                  }
		}
	      break;
	    }
	  }
	break;

      case PHASE_HEX:
	c &= 0x7F;
	if (!filexfer.zmodem.fEscSeq)
	  {
	  switch (c)
	    {
	    case 021:
	    case 023:
	      break;

	    case ZDLE:
              return ('E');
	      break;

	    default:
              if (c >= ' ')
		{
                if (c >= '0' && c <= '9')
                  c = c - '0';
                else if (c >= 'a' && c <= 'f')
                  c = c - 'a' + 10;
                else
                  {
                  return ('E');
                  }

		filexfer.zmodem.sData[filexfer.zmodem.nChrCnt] = (char)(c << 4);
		filexfer.zmodem.fEscSeq = TRUE;
		}
	      break;
	    }
	  }
	else
	  {
	  switch (c)
	    {
	    case 021:
	    case 023:
	      break;

	    case ZDLE:
              return ('E');

	    default:
              if (c >= ' ')
		{
                if (c >= '0' && c <= '9')
                  c = c - '0';
                else if (c >= 'a' && c <= 'f')
                  c = c - 'a' + 10;
                else
                  {
                  return ('E');
                  }

		// 00050 Prevent buffer overflow and lockups.
		if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
		  filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] += (char)c;
		filexfer.zmodem.fEscSeq = FALSE;
		}
	      break;
	    }
	  }
	break;

      case PHASE_ZFIN:
	if (filexfer.zmodem.nChrCnt++ > 0)
          return ('Z');
	break;
      }

    if (filexfer.zmodem.nChrCnt >= 7)
      break;
    }

  if (filexfer.zmodem.nChrCnt >= 7)
    {
    if (!VerifyBlk(filexfer.zmodem.sData, 5))
      {
      return ('E');
      }

    filexfer.zmodem.lPos = filexfer.zmodem.sData[4] & 0x00FF;
    filexfer.zmodem.lPos = (filexfer.zmodem.lPos << 8) | (filexfer.zmodem.sData[3] & 0x00FF);
    filexfer.zmodem.lPos = (filexfer.zmodem.lPos << 8) | (filexfer.zmodem.sData[2] & 0x00FF);
    filexfer.zmodem.lPos = (filexfer.zmodem.lPos << 8) | (filexfer.zmodem.sData[1] & 0x00FF);

    return ('H');
    }

  if (GetCurrentTime() > filexfer.zmodem.lTimeOut)
    return ('T');

  return (-1);
}


static void near ReadBinaryData()
{

  filexfer.zmodem.nChrCnt = 0;
  filexfer.zmodem.nCanCnt = 0;
  filexfer.zmodem.fEscSeq = FALSE;
  filexfer.zmodem.nLength = 0;
  filexfer.zmodem.fNextIsHeader = FALSE;

  filexfer.zmodem.lTimeOut = GetCurrentTime() + filexfer.zmodem.nTimeOut;
}


// Build a binary data packet.
static int near BuildBinaryData()
{
  register c;

  while ((c = CommGet()) != -1)
  {
    if (filexfer.zmodem.nChrCnt > 1080)
      return ('E');

    if (CommError())
      return ('E');

    if (!filexfer.zmodem.fEscSeq)
    {
      switch (c)
      {
	case 021:
	case 0221:
	case 023:
	case 0223:
	  break;

	case ZDLE:
	  filexfer.zmodem.nCanCnt = 1;
	  filexfer.zmodem.fEscSeq = TRUE;
	  break;

	default:
	  // 00050 Prevent buffer overflow and lockups.
	  if (!filexfer.zmodem.fCtlEsc || (c & 0140))
	    if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
	      filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = (char)c;
	  break;
      }
    }
    else
    {
      switch (c)
      {
	case ZDLE:
	  if (filexfer.zmodem.nCanCnt++ >= 4)
            return ('C');
	  break;

	case ZCRCE:
	case ZCRCG:
	case ZCRCQ:
	case ZCRCW:
	  if (filexfer.zmodem.nLength)
            return ('E');

	  // 00050 Prevent buffer overflow and lockups.
	  if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
	    filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = (char)c;
	  filexfer.zmodem.nLength = filexfer.zmodem.nChrCnt;
	  filexfer.zmodem.fEscSeq = FALSE;
	  break;

	case ZRUB0:
	  // 00050 Prevent buffer overflow and lockups.
	  if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
	    filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = 0177;
	  filexfer.zmodem.fEscSeq = FALSE;
	  break;

	case ZRUB1:
	  // 00050 Prevent buffer overflow and lockups.
	  if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
	    filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = 0377;
	  filexfer.zmodem.fEscSeq = FALSE;
	  break;

	case 021:
	case 0221:
	case 023:
	case 0223:
	  break;

	default:
	  if (!filexfer.zmodem.fCtlEsc || (c & 0140))
	    if ((c & 0140) == 0100)
	    {
	      // 00050 Prevent buffer overflow and lockups.
	      if (filexfer.zmodem.nChrCnt < sizeof(filexfer.zmodem.sData)-32)
		filexfer.zmodem.sData[filexfer.zmodem.nChrCnt++] = (char)(c ^ 0100);
	      filexfer.zmodem.fEscSeq = FALSE;
	    }
	    else
              return ('E');
	  break;
      }
    }

    if (filexfer.zmodem.nLength && (filexfer.zmodem.nChrCnt >= filexfer.zmodem.nLength + 2))
    {
      if (!VerifyBlk(filexfer.zmodem.sData, filexfer.zmodem.nLength))
        return ('E');
      return ('D');
    }
  }

  if (GetCurrentTime() > filexfer.zmodem.lTimeOut)
    return ('T');

  return (-1);
}


/*---------------------------------------------------------------------------*/
/*  ZMODEM Sending Subroutines						     */
/*									     */
/*									     */
/*---------------------------------------------------------------------------*/

static void near BufferReset()
{

	filexfer.zmodem.nReSendCnt = 0;
}


// Add a char to the buffer.
static void near BufferChar(int ch)
{

	filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = (char)ch;
}


// Add a DLE-encoded char to the buffer.
static void near BufferData(int ch)
{

	switch (ch &= 255)
	{
	case ZDLE:
		filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = ZDLE;
		filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = (char)(filexfer.zmodem.nLastCh = (ch ^= 0100));
		break;

	case 015:
	case 0215:
		if (!filexfer.zmodem.fCtlEsc && (filexfer.zmodem.nLastCh & 0177) != '@')
		{
			filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = (char)(filexfer.zmodem.nLastCh = ch);
			break;
		}

	case 020:
	case 021:
	case 023:
	case 0220:
	case 0221:
	case 0223:
		filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = ZDLE;
		filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = (char)(filexfer.zmodem.nLastCh = (ch ^= 0100));
		break;

	default:
		if (filexfer.zmodem.fCtlEsc && !(ch & 0140))
		{
			filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = ZDLE;
			ch ^= 0100;
		}
		filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = (char)(filexfer.zmodem.nLastCh = ch);
	}
}


static void near BufferHexC(int ch)
{
	static char digits[] = "0123456789abcdef";

	filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = digits[(ch >> 4) & 0x0F];
	filexfer.zmodem.sReSendData[filexfer.zmodem.nReSendCnt++] = digits[ch & 0x0F];
}


static void near BufferSend()
{

	SendCommChars(filexfer.zmodem.sReSendData, filexfer.zmodem.nReSendCnt);
	filexfer.zmodem.lTimeOut = GetCurrentTime() + filexfer.zmodem.nTimeOut;
}


// Send a binary header.
static void near SendBinaryHeader(int type, char *hdr)
{
	unsigned short crc;
	register i;

	BufferReset();

	if (type == ZDATA)
		for (i = filexfer.zmodem.nZnulls; i > 0; i--)
			BufferData(0);

	BufferChar(ZPAD);
	BufferChar(ZDLE);
	BufferChar(ZBIN);

	BufferData(type);
	crc = updcrc(type, 0);

	for (i = 0; i < 4; i++, hdr++)
	{
		BufferData(*hdr);
		crc = updcrc(*hdr & 255, crc);
	}

	crc = updcrc(0,updcrc(0,crc));
	BufferData(crc>>8);
	BufferData(crc);

	BufferSend();
}


// Send a hex header.
static void near SendHexHeader(int type, char *hdr)
{
	unsigned short crc;
	register i;

	BufferReset();

	BufferChar(ZPAD);
	BufferChar(ZPAD);
	BufferChar(ZDLE);
	BufferChar(ZHEX);

	BufferHexC(type);
	crc = updcrc(type, 0);

	for (i = 0; i < 4; i++, hdr++)
	{
		BufferHexC(*hdr);
		crc = updcrc(*hdr & 255, crc);
	}

	crc = updcrc(0,updcrc(0,crc));
	BufferHexC(crc>>8);
	BufferHexC(crc);

	BufferChar(ASC_CR);
	BufferChar(ASC_LF);

	if (type != ZFIN && type != ZACK)
		BufferChar(ASC_XON);

	BufferSend();
}


// Send a binary packet.
static void near SendBinaryData(char *buf, int length, int frameend)
{
	unsigned short crc;

	BufferReset();

	crc = 0;
	for (;--length >= 0; ++buf)
	{
		BufferData(*buf);
		crc = updcrc((*buf & 255), crc);
	}

	BufferChar(ZDLE);
	BufferChar(frameend);
	crc = updcrc(frameend, crc);

	crc = updcrc(0,updcrc(0,crc));
	BufferData(crc>>8);
	BufferData(crc);

	if (frameend == ZCRCW)
		BufferChar(ASC_XON);

	BufferSend();
}


// Take a long int (i.e., filesize), and stuff it in a char buffer.
static void near LongToHdr(char *hdr, long lPos)
{

	hdr[0] = (char)(lPos);
	hdr[1] = (char)(lPos >> 8);
	hdr[2] = (char)(lPos >> 16);
	hdr[3] = (char)(lPos >> 24);
}


// Send out a ZRINIT packet.
static void near SendRecvInitHeader()
{
	char buf[8];

	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = CANFDX | CANOVIO;
	SendHexHeader(ZRINIT, buf);
}


// Process the header of a ZSINIT packet.
static void near RecvSendSendInit()
{
/*
	Don't have to do anything here.  Sender sends capability
	flags, but currently, they are all zero - none defined
	(yet) in the protocol.
*/
}


// Process an incoming ZSINIT packet.
static void near RecvSendInitData()
{

	RecvSendAck();

	// 00061 Process ZSINIT packet and Attn sequence
	strncpy(filexfer.zmodem.szAttn, &filexfer.zmodem.sData[4], 32);

	ResetXferIndicators(FALSE);
}


// Process the header of a ZFILE packet.
static void near RecvSendFileHeader()
{
/*
	Right now, we don't process the header.  We assume the following:
	ZF0:	(Conversion Options)	-	Binary transfer and File Recovery On
	ZF1:	(Management Options)	-	None (all zero's)
	ZF2:	(Transport Options)		-	None (all zero's)
*/
}


// dca00050 PJL - convert seconds to mon, day, year, hour, min, sec
void long_to_datetime(unsigned long zfsecs)
{
	unsigned long total_secs, total_days;
	int	mon, day, year, hour, min, sec, i;
	char pzbuf[50];

	total_secs = zfsecs;

	// first, convert seconds to days (# secs in a day = 86400)
	total_days = total_secs / 86400;

	// divide by # days in a year
	year = total_days / 365;

	// get left over days
	day = total_days % 365;

	// subtract a day for each leap year
	day -= (year / 4);

	// adjust year & day if needed
	if (day < 0)
	{
		day += 365;
		--year;
	}

	// break down month & day
	for (i = 0, mon = 1; i < 12; i++)
	{
		if ((day - Month[i]) <= 0)
			break;
		day -= Month[i];
		++mon;
	}

	// now, convert left over seconds to hour, min, sec

	// # secs per day = 86400
	total_secs %= 86400;

	// # secs per hour = 3600
	hour = total_secs / 3600;
	total_secs %= 3600;

	// # secs per minute = 60
	min = total_secs / 60;

	// grab left over secs
	sec = total_secs % 60;

	// save file date & time in DOS (1980) format
	filexfer.zmodem.wFileDate = ((year - 10) << 9) + (mon << 5) + day;
	filexfer.zmodem.wFileTime = (hour << 11) + (min << 5) + sec/2;
}


// Respond to a ZFILE packet.
static BOOL near RecvSendFileData()
{
	char buf[132];

	// dca00045 PJL File recovery - check file size, date, time
	if (filexfer.zmodem.file == -1)
	{
		OFSTRUCT OfStruct;
		unsigned long fmodtime;
		char szPath[MAX_PATH];

		lXferFileBytes = fmodtime = 0L;
		filexfer.zmodem.wFileDate = filexfer.zmodem.wFileTime = 0;

		// parse filesize, file modification date/time
		sscanf(&filexfer.zmodem.sData[strlen(filexfer.zmodem.sData)+1],
				"%U%O", &lXferFileSize, &fmodtime);

		// convert file modtime to date and time values
		if (fmodtime > 0) long_to_datetime(fmodtime);

		// build the pathname
		strcpy(szXferFilePath, filexfer.zmodem.sData);
		CreatePath(szPath, VAR_DIRFIL, szXferFilePath);

		// open the file, check for possible file recovery
		if ((filexfer.zmodem.file = OpenFile(szPath, &OfStruct, OF_READWRITE)) == -1)
		{
			if ((filexfer.zmodem.file = OpenFile(szPath, &OfStruct, OF_CREATE | OF_READWRITE)) == -1)
			{
				// dca00071	PJL	Fix file transfer error reporting.
				XferZmodemTerm(XlatError(OfStruct.nErrCode));
				return(FALSE);
			}
		}
		else
		{
			FILESTATUS fsts;

			// get file stats, then close the file
			DosQFileInfo(filexfer.zmodem.file, 1, &fsts, sizeof(fsts));
			DosClose(filexfer.zmodem.file);

			// create a backup copy?
			if (fBackups)
			{
				char szBack[MAX_PATH], *dot;
				int i;
				memset(szBack, 0, MAX_PATH);
				strcpy(szBack, szPath);
				if (dot = strrchr(szBack, '.')) *dot = 0;
				strcat(szBack, ".bak");
				if (i = DosCopy(szPath, szBack)) return(i);
			}

			// check filedate, filetime, filesize
			if ((fsts.fdateLastWrite == filexfer.zmodem.wFileDate) &&
				(fsts.ftimeLastWrite == filexfer.zmodem.wFileTime) &&
				(fsts.cbFile <= lXferFileSize))
			{
				// re-open for file recovery
				if ((filexfer.zmodem.file = OpenFile(szPath, &OfStruct, OF_READWRITE)) == -1)
				{
					// dca00071	PJL	Fix file transfer error reporting.
					XferZmodemTerm(XlatError(OfStruct.nErrCode));
					return(FALSE);
				}
				DosChgFilePtr(filexfer.zmodem.file, fsts.cbFile, FILE_BEGIN, &lXferFileBytes);
			}
			// re-open the file, truncating to length zero
			else if ((filexfer.zmodem.file = OpenFile(szPath, &OfStruct, OF_CREATE | OF_READWRITE)) == -1)
			{
				// dca00071	PJL	Fix file transfer error reporting.
				XferZmodemTerm(XlatError(OfStruct.nErrCode));
				return(FALSE);
			}
		}

		wXferCurrFiles++;
		wXferBlockCount = 0;
		ResetXferIndicators(FALSE);
	}

	LongToHdr(buf, lXferFileBytes);
	SendHexHeader(ZRPOS, buf);

	return(TRUE);
}


// Process the header of a ZDATA packet (file offset).
static void near RecvSendDataHeader()
{
	ULONG ulPos;
	char buf[8];

	if (filexfer.zmodem.lPos > lXferFileBytes)
	{
		LongToHdr(buf, lXferFileBytes);
		SendHexHeader(ZRPOS, buf);
		ReadAnyHeader();
	}
	else
	{
		if (filexfer.zmodem.file != -1)
			DosChgFilePtr(filexfer.zmodem.file, filexfer.zmodem.lPos, FILE_BEGIN, &ulPos);
		lXferFileBytes = filexfer.zmodem.lPos;
		ReadBinaryData();
	}
}


// Write data from a ZDATA packet to the file.
static BOOL near RecvSendDataData()
{
	USHORT usBytes;
	register USHORT usResults;

	lXferFileBytes += filexfer.zmodem.nLength - 1;
	wXferBlockCount++;

	if (filexfer.zmodem.file != -1)
		if (usResults = DosWrite(filexfer.zmodem.file, filexfer.zmodem.sData,
								filexfer.zmodem.nLength - 1, &usBytes))
			XferZmodemTerm(XlatError(usResults));
		else if (filexfer.zmodem.nLength - 1 != usBytes)
			XferZmodemTerm(ERRDOS_DISKFULL);
		else
		{
			// dca00080 PJL Use Consec (not Max) error count.
			wXferConsecErrors = 0;
			UpdateXferIndicators();
			return(TRUE);
		}

  return(FALSE);
}


// Send out a ZACK packet.
static void near RecvSendAck()
{
	char buf[8];

	LongToHdr(buf, lXferFileBytes);
	SendHexHeader(ZACK, buf);
	// dca00080 PJL Use Consec (not Max) error count.
	wXferConsecErrors = 0;
}


// Send out a ZNAK packet.
static void near RecvSendNak()
{
	char buf[8];

	LongToHdr(buf, lXferFileBytes);
	SendHexHeader(ZNAK, buf);
	UpdateErrorCount();
}


// Send out a ZRPOS packet.
static void near RecvSendPos()
{
	char buf[8];

	LongToHdr(buf, lXferFileBytes);
	SendHexHeader(ZRPOS, buf);
}


// Received a ZNAK packet - update ErrorCount.
static BOOL near SendRecvNak()
{

	if (!UpdateErrorCount)
		return(FALSE);

	SendCommChars(filexfer.zmodem.sReSendData, filexfer.zmodem.nReSendCnt);
	filexfer.zmodem.nChrCnt = 0;

	wXferLastError = XFER_ERROR_NAK;

	filexfer.zmodem.lTimeOut = GetCurrentTime() + filexfer.zmodem.nTimeOut;

	return(TRUE);
}


// Send out a Hex ZFIN packet.
static void near SendRecvFin()
{
	char buf[8];

	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	SendHexHeader(ZFIN, buf);
}


// Send out a ZRQINIT packet.
static void near SendSendInitHeader()
{
	char buf[8];

	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	SendHexHeader(ZRQINIT, buf);
}


// Process an incoming ZRINIT packet.
static void near ProcessReceiverInit()
{

	if (filexfer.zmodem.sData[4] & CANFDX)
		filexfer.zmodem.fStream = TRUE;
	else
		filexfer.zmodem.fStream = FALSE;

	filexfer.zmodem.nRcvBuf = filexfer.zmodem.sData[1] * 256 + filexfer.zmodem.sData[2];

	if (filexfer.zmodem.nRcvBuf < 32 || filexfer.zmodem.nRcvBuf > 1024)
		filexfer.zmodem.nRcvBuf = nXferBlockSize;
	else if (filexfer.zmodem.nRcvBuf && filexfer.zmodem.nRcvBuf < nXferBlockSize)
		nXferBlockSize = filexfer.zmodem.nRcvBuf;

	filexfer.zmodem.fStream = (char)(filexfer.zmodem.fStream && filexfer.zmodem.nRcvBuf == 0);
}


// dca00050 PJL - convert file modtime to seconds, then to octal string
void long_to_octal(char *fname, char *zfbuf)
{
	unsigned long total_days, total_secs;
	unsigned time_secs;
	struct stat fst;
	struct tm *dt;
	int i;

	// grab file modtime
	stat(fname, &fst);
	dt = localtime(&fst.st_mtime);

	// get days since Jan. 1, 1970

	// first, convert years to days
	dt->tm_year -= 70;
	total_days = (dt->tm_year * 365) + (dt->tm_year / 4);

	// add in months
	for (i = 0; i < dt->tm_mon; i++)
		total_days += Month[i];

	// add in days
	total_days += dt->tm_mday;

	// convert days to seconds (secs per day = 86400)
	total_secs = total_days * 86400;
	
	// convert time to seconds & add it in
	time_secs = (dt->tm_hour * 3600) + (dt->tm_min * 60) + dt->tm_sec;
	total_secs += time_secs;

	sprintf(zfbuf, "%lu %lo", fst.st_size, total_secs);
}


// Send a ZFILE packet.
static BOOL near SendSendFileHeader()
{
	FILESTATUS fsts;
	char buf[256];

	if (filexfer.zmodem.file == -1)
	{
		if (_dos_findnext(&filexfer.zmodem.sendfile) == 0)
		{
			if ((filexfer.zmodem.file = OpenXferFile(filexfer.zmodem.sendfile.name)) == -1)
			{
				// dca00071	PJL	Fix file transfer error reporting.
				XferZmodemTerm(ERRSYS_READFAULT);
				return(FALSE);
			}

			strcpy(szXferFilePath, filexfer.zmodem.sendfile.name);
			DosQFileInfo(filexfer.zmodem.file, 1, &fsts, sizeof(fsts));
			lXferFileSize = fsts.cbFile;
		}
	}

	wXferBlockCount = 0;
	lXferFileBytes  = 0;

	if (filexfer.zmodem.file != -1)
	{
		int inx;

		// 00023 Reorder the flags bits to the correct location.
		buf[ZF0] = ZCBIN | ZCRESUM;
		buf[ZF1] = 0;
		buf[ZF2] = 0;
		buf[ZF3] = 0;
		SendBinaryHeader(ZFILE, buf);

		// dca00045 PJL File recovery - send file size, date, time
		memset(buf, 0, 256);
		strcpy(buf, StripPath(szXferFilePath));
		inx = strlen(buf) + 1;
		long_to_octal(szXferFilePath, buf+inx);
		inx += strlen(buf+inx);

		wXferCurrFiles++;
		SendBinaryData(buf, inx, ZCRCW);
		filexfer.zmodem.cState = SENT_FILE;
		ResetXferIndicators(FALSE);
	}
	else
	{
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;

		SendHexHeader(ZFIN, buf);
		filexfer.zmodem.cState = SENT_ZFIN;
	}

	return(TRUE);
}


// Do an FSEEK.
static void near ProcessFilePos()
{
	ULONG ulPos;

	lXferFileBytes = filexfer.zmodem.lPos;
	DosChgFilePtr(filexfer.zmodem.file, filexfer.zmodem.lPos, FILE_BEGIN, &ulPos);
	filexfer.zmodem.fHeader = TRUE;
}


// Send out a ZDATA packet.
static BOOL near SendSendFileData()
{
	USHORT usBytes;
	BYTE pBuf[1100];
	int  i;

	// If we are in streaming output mode then check the comm buffer for room.
	if (filexfer.zmodem.fStreaming)
	{
		// We don't want to put too much in the communication buffer.
		if ((i = nCommPortMemory) > 4096)
			i = 4096;

		// get current transmit buffer size
		GetCommError(nCommID, (COMSTAT FAR *)&ComStat);

		if (ComStat.cbOutQue + 2048 > i)
			return (TRUE);
	}

	if (DosRead(filexfer.zmodem.file, pBuf, nXferBlockSize, &usBytes))
	{
		XferZmodemTerm(ERRSYS_READFAULT);
		return (FALSE);
	}

	i = usBytes;

	if (i)
	{
		if (filexfer.zmodem.fHeader)
		{
			BYTE pHdr[8];

			FlushComm(nCommID, 0);
			LongToHdr(pHdr, lXferFileBytes);
			SendBinaryHeader(ZDATA, pHdr);
			filexfer.zmodem.fHeader = FALSE;
		}
		if (filexfer.zmodem.fStream == 0 && lXferFileBytes + i < lXferFileSize)
		{
			SendBinaryData(pBuf, i, ZCRCG);
			filexfer.zmodem.fStreaming = TRUE;
		}
		else
		{
			SendBinaryData(pBuf, i, ZCRCW);
			filexfer.zmodem.fStreaming = FALSE;
		}

		lXferFileBytes += (long)i;
		wXferBlockCount++;
		filexfer.zmodem.cState = SENT_DATA;
	}
	else
		SendEofPacket();

	UpdateXferIndicators();

	return (TRUE);
}


// Send a ZEOF packet - with ending filesize.
static void near SendEofPacket()
{
	BYTE buf[8];

	// close the file
	ZmodemCloseFile();

	// send ZEOF
	LongToHdr(buf, lXferFileBytes);
	SendHexHeader(ZEOF, buf);

	// reset state indicator
	filexfer.zmodem.cState = SENT_ZFIN;
}


// Send a ZFIN packet - with ending filesize.
static void near SendSendFin()
{
	BYTE buf[8];

	LongToHdr(buf, lXferFileBytes);
	SendHexHeader(ZFIN, buf);
}
	

// Cancel a transfer (Send out Atten sequence).
static void near SendAbortString()
{
	char buf[32];
	register i;

	for (i = 0; i < 12; i++)
	{
		buf[i]    = ASC_CAN;
		buf[i+12] = ASC_BS;
	}

	SendCommChars(buf, 24);
}

