/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Dialing Module (PRODIAL.C)                                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module does all the work with dialing a profile and        *
*             display a dialog for select of a profile.                       *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*  1.01 08/03/89 00004 Auto open capture and printer.						   *
*  1.01 09/12/89 00036 Add dialing count down.								   *
*  1.01 10/31/89 00052 Add context help to the menus and dialogs.			   *
*  1.01 01/19/90 00089 Don't display user call error.						   *
*  1.1  04/02/90 dca00065 PJL Fix Invalid/Open disk drive error reporting.	   *
*	1.1  04/07/90 dca00072 PJL Change CommStrOutput decl. (REPLY '~'problem).  *
*  1.1  04/09/90 dca00075 MKL fixed action.dial focus not set problem.        *
*  1.1	04/18/90 dca00092 MKL fixed Modem error problem when ModemInit String *
*	is empty							      *
*	1.1b 05/15/90 dca00096 PJL Fix PP PM2400SA modem problems.                 *
*	1.2  05/23/90 dca00101 PJL Fix dialing timeout problem.         		   *
*	1.2  06/04/90 dca00102 PJL Fix duplicate modem string display problem.	   *
*                                                                             *
\*****************************************************************************/

#define NOSOUND 		/* Sound driver routines */

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "sess.h"
#include "term.h"
#include "xfer.h"
#include "prodata.h"
#include "interp.h"
#include "global.h"
#include "variable.h"


#define COMM_START	0
#define COMM_INIT	1
#define COMM_SCAN	2
#define COMM_WAIT	3
#define COMM_OK 	4
#define COMM_ERROR	5
#define COMM_CONNECT	6
#define COMM_SPEED	7
#define COMM_ARQ	8
#define COMM_RING       9
#define COMM_NOTONE    10
#define COMM_BUSY      11
#define COMM_NOCD      12
#define COMM_VOICE     13
#define COMM_DIAL      14
#define COMM_REDIAL    15
#define COMM_ABORT     16
#define COMM_NOTHING   17


static char near GetNextNumber(char *, char *, char);
// dca00096 PJL Fix for busy/no answer...disconnect...redial problem
extern void ModemReset(void);

static int  nState;
static long lTimer;
static FARPROC lpDlgDialing;
static char fWatchDialing = FALSE;
static char cRetries;

BOOL DlgDial(HWND, unsigned, WORD, LONG);


void CmdDial()
{
	FARPROC lpDlg;

	if (hXferDlg)
	{
		DialTerm(0);
		return;
	}

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
	if (!IsPathAvailable(VAR_DIRXWP))
		return;

	if (DataSaveFirst() == ID_CANCEL)
		return;

	InfoMessage(MSGINF_HELP);
	wHelpDialog = HELP_ITEM_DIAL;

	lpDlg = MakeProcInstance((FARPROC)DlgDial, hWndInst);
	if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_DIAL), hWnd, lpDlg))
	{
		InfoMessage(NULL);
		InitSystemNewData();
		CmdConnect();
		DataAutoOpen();  // 00004 Auto open the capture and/or printer.
	}
	else
		InfoMessage(NULL);

	FreeProcInstance(lpDlg);

	wHelpDialog = NULL;
}


BOOL DlgDial(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
	char szPath[MAX_PATH], buf[64], *ptr;
	int  i;

	switch (message)
	{
	case WM_INITDIALOG:
		// dca00075 MKL
		// EnableWindow(GetDlgItem(hDlg, ID_DELETE), FALSE);
		SetDlgItemLength(hDlg, ID_DIAL_NUMBER, sizeof(szManualNumber)-1);

		GetStrVariable(VAR_DIRXWP, szPath, sizeof(szPath));
		SetDlgItemPath(hDlg, ID_PATH, szPath);

		DataDialLoad(hDlg, ID_LISTBOX);

		// dca00075 MKL
		if ((i = (int) SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, 0, 0L)
			)== LB_ERR) EnableWindow(GetDlgItem(hDlg, ID_DELETE), FALSE);

		if (fManualDial)
		{
			CheckDlgButton(hDlg, ID_DIAL_MANUAL, fManualDial);
			SetDlgItemText(hDlg, ID_DIAL_NUMBER, szManualNumber);
		}
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_LISTBOX:
			if (HIWORD(lParam) == LBN_SELCHANGE)
			{
				EnableWindow(GetDlgItem(hDlg, ID_DELETE), TRUE);
				CheckDlgButton(hDlg, ID_DIAL_MANUAL, FALSE);
				SetDlgItemStr(hDlg, ID_DIAL_NUMBER, 0);
			}

			if (HIWORD(lParam) != LBN_DBLCLK)
				break;

		case IDOK:
			if (fManualDial = (char)IsDlgButtonChecked(hDlg, ID_DIAL_MANUAL))
			{
				GetDlgItemText(hDlg, ID_DIAL_NUMBER, szManualNumber,
					sizeof(szManualNumber));
				EndDialog(hDlg, TRUE);
				break;
			}

			if ((i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L)) == LB_ERR)
				break;

			SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, (WORD)i,
				(DWORD)((LPSTR)buf));

			if (ptr = strchr(buf, ' '))
				*ptr = NULL;

			if (DataOpen(buf) == 0)
				EndDialog(hDlg, TRUE);
			else
				EndDialog(hDlg, FALSE);
			break;

		case ID_DELETE:
			if (CancelMessage(MSG_SURETODELETE) != ID_OK)
				break;

			if ((i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L)) == LB_ERR)
				break;

			SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, (WORD)i,
					(DWORD)((LPSTR)buf));
			SendDlgItemMessage(hDlg, ID_LISTBOX, LB_DELETESTRING, i, 0L);

			ptr = strchr(buf, ' ');
			*ptr = NULL;
			CreatePath(szPath, VAR_DIRXWP, buf);
			DosDelete(szPath, 0L);
			// dca00075 MKL
			if ((i =(int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, 0, 0L)
				)== LB_ERR) EnableWindow(GetDlgItem(hDlg, ID_DELETE), FALSE);
			else EnableWindow(GetDlgItem(hDlg, ID_DELETE), TRUE);
			break;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;

		case ID_DIAL_MANUAL:
			if (HIWORD(lParam) == BN_CLICKED)
				SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SETCURSEL, -1, 0L);
			break;

		default:
			return FALSE;
			break;
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}


void DialNumber()
{
	char szBuf[128];

	lpDlgDialing = MakeProcInstance((FARPROC)DlgDialing, hWndInst);
	hXferDlg = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_DIALING), hWnd, lpDlgDialing);

	if (hXferDlg == 0)
		return;

	LinkDialog(hXferDlg);

	szBuf[0] = 'D';
	szBuf[1] = 'i';
	szBuf[2] = 'a';
	szBuf[3] = 'l';
	szBuf[4] = 'i';
	szBuf[5] = 'n';
	szBuf[6] = 'g';
	szBuf[7] = ' ';
	LibEnstore(Sess.szDescription, &szBuf[8], sizeof(szBuf)-8);
	InfoDisplay(szBuf);

	SetSystemStatus(SS_DIAL);
	nState = COMM_START;
	CommReset();
}


void DialTerm(int sError)
{
	char buf[64];
	char fScript;

	if (hXferDlg == 0)
		return;

	nErrorCode = sError;

	if (GetProfileInt(szAppName, "WatchDial", 0) != fWatchDialing)
	{
		char buf[2];

		buf[0] = (char)('0' + fWatchDialing);
		buf[1] = 0;
		WriteProfileString(szAppName, "WatchDial", buf);
	}

	nState = COMM_NOTHING;

	if (IfSystemStatus(SS_SCRIPT))
		fScript = TRUE;
	else
		fScript = FALSE;

	DestroyWindow(hXferDlg);
	UnLinkDialog(hXferDlg);
	hXferDlg = 0;
	FreeProcInstance(lpDlgDialing);
	ResetSystemStatus(SS_DIAL);
	InfoDisplay(NULL);

	if (IfSystemStatus(SS_ABORT))
		return;

	if (sError && !fScript)
	{
		// 00089 Don't display user call cancelled.
		if (sError != ERRDIA_CALLCANCEL)
			GeneralError(sError);
	}

	if (nErrorCode == 0)
	{
		if (Sess.szScript[0])
		{
			register i;

			if (IfSystemStatus(SS_SCRIPT))
				InterpreterStop();

			i = fCompileOnly;
			fCompileOnly = 0;
			LibEnstore(Sess.szScript, buf, sizeof(buf));
			if (i = ExecuteScript(buf))
				GeneralError(i);
			fCompileOnly = (char)i;
		}
	}
}


BOOL DlgDialing(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
	static char cCommandOk;
	static char cCommandError;
	static char cConnect;
	static char cConnectSpeed;
	static char cConnectARQ;
	static char cRingDetect;
	static char cNoDialTone;
	static char cBusy;
	static char cNoCarrier;
	static char cVoice;
	static int  nSpeed;
	static char fLastWasDigit;
	static char fGotSomething;
	static int  nDialError;
	static char cNumber;
	static short sLastTime;   // 00036
	char szBuf[64], szFmt[64], szVoice[16];
	register i, j;

	switch (message)
	{
	case WM_INITDIALOG:
		fWatchDialing = (char)GetProfileInt(szAppName, "WatchDial", 0);
		CheckDlgButton(hDlg, ID_DIAL_WATCH, fWatchDialing);

		GetWindowText(hDlg, szBuf, sizeof(szBuf));
		LibEnstore(Sess.szDescription, szFmt, sizeof(szFmt));
		strcat(szBuf, szFmt);
		SetWindowText(hDlg, szBuf);

		fLastWasDigit = FALSE;
		fGotSomething = FALSE;
		fConnectARQ	  = FALSE;
		nConnectSpeed = 0;
		nErrorCode	  = 0;
		nDialError	  = 0;
		cNumber		  = 0;
		szMatchedString[0] = 0;

		szBuf[0] = 'D';
		szBuf[1] = 'i';
		szBuf[2] = 'a';
		szBuf[3] = 'l';
		szBuf[4] = 'i';
		szBuf[5] = 'n';
		szBuf[6] = 'g';
		szBuf[7] = ' ';
		szBuf[8] = NULL;

		i = toupper(szManualNumber[0]);
		if (i == 'L')
			LibEnstore(szLdNumber, szFmt, sizeof(szFmt));
		else if (i != 'X')
			LibEnstore(szOutNumber, szFmt, sizeof(szFmt));
		strcat(szBuf, szFmt);

		if (i == 'L' || i == 'X')
			strcat(szBuf, szManualNumber+1);
		else
			strcat(szBuf, szManualNumber);

		i = strlen(szBuf);
		szBuf[i++] = ' ';
		szBuf[i++] = '@';
		szBuf[i++] = ' ';
		_strtime(&szBuf[i]);

		SetFocus(GetDlgItem(hDlg, ID_OK));
		return (FALSE);
		break;

	case WM_USER+50:
		DestroyWindow(hDlg);
		UnLinkDialog(hDlg);
		return (FALSE);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_OK:
			if (nState == COMM_SCAN)
				// dca00096 PJL
				CommStrOutput(Modem.szDialSuffix+1, TRUE);
			else
				lTimer = GetCurrentTime() + 1L;
			break;

		case ID_CANCEL:
			nState = COMM_NOTHING;
			if (bConnected)
				CommPortClose();
			if (nErrorCode == 0)
				DialTerm(ERRDIA_CALLCANCEL);
			else
				DialTerm(0);
			break;

		case ID_DIAL_WATCH:
			fWatchDialing = (char)IsDlgButtonChecked(hDlg, ID_DIAL_WATCH);
			break;

		default:
			return FALSE;
			break;
		}
		break;

	case WM_CTLCOLOR:
		if ((LOWORD(lParam) == GetDlgItem(hDlg, ID_DIAL_STATUS)) && !fMonoDisplay)
		{
			SetTextColor(wParam, RGB(255, 0, 0));
			return (GetStockObject(WHITE_BRUSH));
		}
		if ((LOWORD(lParam) == GetDlgItem(hDlg, ID_DIAL_NUMBER)) && !fMonoDisplay)
		{
			SetTextColor(wParam, RGB(0, 0, 255));
			return (GetStockObject(WHITE_BRUSH));
		}
		return FALSE;
		break;

	case WM_TIMER:
		if (nState == COMM_NOTHING)
			break;

		if (!bConnected)
		{
			nDialError = ERRDIA_LOSTCONNECT;
			nState = COMM_ABORT;
			break;
		}

		if (nState == COMM_WAIT)
		{
			int ch;
			ch = 0;
			while ((i = CommGet()) != -1)
			{
				++ch;
				fGotSomething = TRUE;
				if (fWatchDialing)
				{
					if ((char)i == Modem.szModemInit[ch])
						break;
					if ((char)i == Modem.szDialPrefix[ch])
						break;
					if ((char)i == Modem.szDialSuffix[ch])
						break;
					if ((char)i == Modem.szModemHangup[ch])
						break;
					if ((char)i == Modem.szAnswerSetup[ch])
						break;
					CharToTerminal(i);
				}

				if (isdigit(i))
				{
					if (!fLastWasDigit)
					{
						nSpeed = 0;
						fLastWasDigit = TRUE;
					}
					nSpeed = nSpeed * 10 + (i - '0');
				}
				else
					fLastWasDigit = FALSE;

				if (Modem.szCommandOk[cCommandOk] == (char)i)
				{
					if (cCommandOk >= Modem.szCommandOk[0])
					{
						nState = COMM_OK;
						break;
					}
					cCommandOk++;
				}
				else
					cCommandOk = 1;

				if (Modem.szCommandError[cCommandError] == (char)i)
				{
					if (cCommandError >= Modem.szCommandError[0])
					{
						nState = COMM_ERROR;
						break;
					}
					cCommandError++;
				}
				else
					cCommandError = 1;

				if (Modem.szConnect[cConnect] == (char)i)
				{
					if (cConnect >= Modem.szConnect[0])
					{
						nState = COMM_CONNECT;
						break;
					}
					cConnect++;
				}
				else
					cConnect = 1;

				if (Modem.szConnectSpeed[cConnectSpeed] == '%')
				{
					if (!isdigit(i))
						cConnectSpeed++;
				}
				if (Modem.szConnectSpeed[cConnectSpeed] == (char)i)
				{
					if (cConnectSpeed >= Modem.szConnectSpeed[0])
					{
						nState = COMM_SPEED;
						break;
					}
					cConnectSpeed++;
				}
				else if (Modem.szConnectSpeed[cConnectSpeed] != '%')
					cConnectSpeed = 1;

				if (Modem.szConnectARQ[cConnectARQ] == '%')
				{
					if (!isdigit(i))
						cConnectARQ++;
				}
				if (Modem.szConnectARQ[cConnectARQ] == (char)i)
				{
					if (cConnectARQ >= Modem.szConnectARQ[0])
					{
						nState = COMM_ARQ;
						break;
					}
					cConnectARQ++;
				}
				else if (Modem.szConnectARQ[cConnectARQ] != '%')
					cConnectARQ = 1;

				if (Modem.szRingDetect[cRingDetect] == (char)i)
				{
					if (cRingDetect == Modem.szRingDetect[0])
					{
						nState = COMM_RING;
						break;
					}
					cRingDetect++;
				}
				else
					cRingDetect = 1;

				if (Modem.szNoDialTone[cNoDialTone] == (char)i)
				{
					if (cNoDialTone >= Modem.szNoDialTone[0])
					{
						LibEnstore(Modem.szNoDialTone, szMatchedString, 24);
						nState = COMM_NOTONE;
						break;
					}
					cNoDialTone++;
				}
				else
					cNoDialTone = 1;

				if (Modem.szBusy[cBusy] == (char)i)
				{
					if (cBusy >= Modem.szBusy[0])
					{
						LibEnstore(Modem.szBusy, szMatchedString, 24);
						nState = COMM_BUSY;
						break;
					}
					cBusy++;
				}
				else
					cBusy = 1;

				if (Modem.szNoCarrier[cNoCarrier] == (char)i)
				{
					if (cNoCarrier == Modem.szNoCarrier[0])
					{
						LibEnstore(Modem.szNoCarrier, szMatchedString, 24);
						nState = COMM_NOCD;
						break;
					}
					cNoCarrier++;
				}
				else
					cNoCarrier = 1;

				szVoice[0] = 'V';
				szVoice[1] = 'O';
				szVoice[2] = 'I';
				szVoice[3] = 'C';
				szVoice[4] = 'E';
				szVoice[5] = ASC_CR;
				szVoice[6] = ASC_LF;
				szVoice[7] = 0;
				if (szVoice[cVoice] == (char)i)
				{
					cVoice++;
					if (szVoice[cVoice] == NULL)
					{
						nState = COMM_VOICE;
						break;
					}
				}
				else
					cVoice = 0;
			}
		}
		else if (nState == COMM_DIAL)
			while ((i = CommGet()) != -1)
			{
				fGotSomething = TRUE;
				if (fWatchDialing)
					CharToTerminal(i);
			}

		switch (nState)
		{
		case COMM_START:
			cRetries = 1;
			// dca00092 MKL fixed empty ModemInit string problem
			// nState = COMM_INIT;
			// dca00096 PJL Moved ModemInit from COMMHI.C
			if (Modem.szModemInit[0] > 0)
			{
				SetDlgItemStr(hDlg, ID_DIAL_STATUS, MSGDIA_INIT);
				// dca00096 PJL
				CommStrOutput(Modem.szModemInit+1, TRUE);
				nState = COMM_INIT;
			}
			else
				nState = COMM_OK;
			lTimer = GetCurrentTime() + ONESEC * 2;
			break;

		case COMM_INIT:
			if ((i = CommGet()) == -1)
			{
				// dca00101 PJL This comparison is backwards!
                //if (lTimer > GetCurrentTime())
				if (lTimer <= GetCurrentTime())
				{
					nDialError = ERRDIA_MODEMERROR;
					nState = COMM_ABORT;
				}
				break;
			}

			CommUnGet();
			nState = COMM_SCAN;
			break;

		case COMM_SCAN:
			if (lTimer > GetCurrentTime())
				break;

			cCommandOk	  = 1;
			cCommandError = 1;
			cConnect	  = 1;
			cConnectSpeed = 1;
			cConnectARQ	  = 1;
			cRingDetect	  = 1;
			cNoDialTone	  = 1;
			cBusy		  = 1;
			cNoCarrier	  = 1;
			cVoice		  = 0;

			nState = COMM_WAIT;
			lTimer = GetCurrentTime() + Sess.nPatience * 1000L;
			break;

		case COMM_WAIT:
			if (lTimer > GetCurrentTime())
				break;

			if (fGotSomething)
			{
				// dca00096 PJL
				ModemReset();
				nState = COMM_REDIAL;
			}
			else
			{
				nDialError = ERRDIA_MODEMBAD;
				nState     = COMM_ABORT;
			}
			break;

		case COMM_OK:
			lTimer = GetCurrentTime() + ONESEC / 2;
			nState = COMM_DIAL;
			break;

		case COMM_ERROR:
			// dca00096 PJL
			ModemReset();
			nDialError = ERRDIA_MODEMERROR;
			nState = COMM_ABORT;
			break;

		case COMM_CONNECT:
			DialTerm(0);
			break;

		case COMM_SPEED:
			nConnectSpeed = nSpeed;
			if (Modem.cChange && nSpeed)
			{
				GetCommState(nCommID, (DCB far *)&ComDCB);
				if (ComDCB.BaudRate != nSpeed)
				{
					ComDCB.BaudRate = nSpeed;
					SetCommState((DCB far *)&ComDCB);
				}
			}
			DialTerm(0);
			break;

		case COMM_ARQ:
			fConnectARQ	= TRUE;
			nConnectSpeed = nSpeed;
			SendMessage(hChildStatus, UM_CONFIG, 0, 0L);
			DialTerm(0);
			break;

		case COMM_RING:
			nState = COMM_SCAN;
			break;

		case COMM_NOTONE:
			if (cRetries-1 > (char)Sess.nRedialCount && cNumber == 0)
			{
				nDialError = ERRDIA_NODIALTONE;
				nState = COMM_ABORT;
			}
			else
				nState = COMM_REDIAL;
			break;

		case COMM_BUSY:
			if (cRetries-1 > (char)Sess.nRedialCount && cNumber == 0)
			{
				nDialError = ERRDIA_NUMBERBUSY;
				nState = COMM_ABORT;
			}
			else
				nState = COMM_REDIAL;
			break;

		case COMM_NOCD:
			if (cRetries-1 > (char)Sess.nRedialCount && cNumber == 0)
			{
				nDialError = ERRDIA_NOANSWER;
				nState = COMM_ABORT;
			}
			else
				nState = COMM_REDIAL;
			break;

		case COMM_VOICE:
			nDialError = ERRDIA_VOICE;
			nState = COMM_ABORT;
			break;

		case COMM_DIAL:
			if (lTimer > GetCurrentTime())
			{
				// 00036 Add dialing count down.
				i = (short)((lTimer - GetCurrentTime()) / 1000L);
				if (sLastTime != i)
				{
					sLastTime = i;
					LoadString(hWndInst, MSGDIA_WAIT, szFmt, sizeof(szFmt));
					sprintf(szBuf, szFmt, i);
					SetDlgItemText(hDlg, ID_DIAL_STATUS, szBuf);
				}
				break;
			}

			if (strlen(szMatchedString) > 2)
				szMatchedString[strlen(szMatchedString)-3] = 0;
			SetDlgItemText(hDlg, ID_DIAL_MSG, szMatchedString);

			szMatchedString[0] = 0;

			LoadString(hWndInst, MSGDIA_DIAL, szFmt, sizeof(szFmt));
			sprintf(szBuf, szFmt, cRetries);
			SetDlgItemText(hDlg, ID_DIAL_STATUS, szBuf);

			// dca00096 PJL
			if (Modem.szDialPrefix[1] == 'A' && Modem.szDialPrefix[2] == 'T')
			{
				char mbuf[60];
				memset(mbuf, 0, 60);
				mbuf[0] = Modem.szDialPrefix[1];
				mbuf[1] = Modem.szDialPrefix[2];
				strcpy(mbuf+2, Sess.szModifier+1);
				strcat(mbuf, Modem.szDialPrefix+3);
				CommStrOutput(mbuf, TRUE);
			}
			else
				CommStrOutput(Modem.szDialPrefix+1, TRUE);

			cNumber = GetNextNumber(szBuf, szManualNumber, cNumber);

			i = toupper(szBuf[0]);
			if (i == 'L')
			{
				// dca00096 PJL
				CommStrOutput(szLdNumber+1, TRUE);
				j = (int)szLdNumber[0];
				memcpy(szFmt, szLdNumber+1, j);
			}
			else if (i != 'X')
			{
				// dca00096 PJL
				CommStrOutput(szOutNumber+1, TRUE);
				j = (int)szOutNumber[0];
				memcpy(szFmt, szOutNumber+1, j);
			}
			else
				j = 0;

			if (i == 'L' || i == 'X')
			{
				strcpy(szFmt+j, szBuf+1);
				// dca00072 PJL Change CommStrOutput decl. (REPLY '~'problem).
				CommStrOutput(szBuf+1, TRUE);
			}
			else
			{
				strcpy(szFmt+j, szBuf);
				// dca00072 PJL Change CommStrOutput decl. (REPLY '~'problem).
				CommStrOutput(szBuf, TRUE);
			}

			SetDlgItemText(hDlg, ID_DIAL_NUMBER, szFmt);

			// dca00096 PJL
			CommStrOutput(Modem.szDialSuffix+1, TRUE);
			nState = COMM_SCAN;
			break;

		case COMM_REDIAL:
			if (cRetries-1 > (char)Sess.nRedialCount && cNumber == 0)
			{
				nDialError = ERRDIA_NOANSWER;
				nState = COMM_ABORT;
			}
			else
			{
				nState = COMM_DIAL;
				lTimer = GetCurrentTime() + Sess.nRedialWait * 1000L;
				// 00036 Add dialing count down.
				sLastTime = Sess.nRedialWait;

				LoadString(hWndInst, MSGDIA_WAIT, szFmt, sizeof(szFmt));
				if (strlen(szMatchedString) > 3)
					szMatchedString[strlen(szMatchedString)-4] = NULL;
				SetDlgItemText(hDlg, ID_DIAL_MSG, szMatchedString);
				sprintf(szBuf, szFmt, Sess.nRedialWait);
				SetDlgItemText(hDlg, ID_DIAL_STATUS, szBuf);
				szMatchedString[0] = 0;
			}
			break;

		case COMM_ABORT:
			if (bConnected)
				CommPortClose();
			InfoDisplay(NULL);
			DialTerm(nDialError);
			break;
		}
		break;

	default:
		return (FALSE);
	}

	return (TRUE);
}

/*---------------------------------------------------------------------------*/
/* Get the next number from the list delimited by the sem ';'.  This permits */
/* dialing of multible numbers by parsing the user NUMBER session parameter. */
/* The INX is set to zero when the dialog (above) is started and this sub-   */
/* routine controls it after that.					     */
/*---------------------------------------------------------------------------*/
static char near GetNextNumber(szBuf, szNumber, nInx)
  char *szBuf;
  char *szNumber;
  char nInx;
{
  char *ptr;
  register i;

  ptr = szNumber;
  for (i = 0; i < nInx; i++)
    {
    if (ptr = strchr(ptr, ';'))
      {
      ptr++;
      }
    else
      {
      ptr = szNumber;
      nInx = 0;
      break;
      }
    }
  nInx++;

  if (strchr(ptr, ';') == NULL)
    {
    cRetries++;
    nInx = 0;
    }

  while (*ptr != NULL && *ptr != ';')
    *szBuf++ = *ptr++;
  *szBuf = NULL;

  return (nInx);
}

BOOL DlgNoPhone(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}
