/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   File Capture (CAPTURE.C)                                        *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module is used to capture incomming data to a capture      *
*             file.                                                           *
*                                                                             *
*  Revisions:																   *
*	1.00 07/17/89 Initial Version.											   *
*	1.01 11/06/89 00053 Upgrade capture logic for better file io.			   *
*	1.1  04/08/90 dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>
#include <time.h>
#include "xtalk.h"
#include "library.h"
#include "capture.h"
#include "comm.h"
#include "global.h"
#include "sess.h"
#include "term.h"
#include "prodata.h"
#include "variable.h"
#include "xfer.h"
#include "dialogs.h"


// Defined the size of the capture file buffer.
#define MAX_BUFFER_SIZE     512

// Defined handle to current capture file, if openned.
static HFILE hfCapFile = -1;

// Define structures for buffered io.
static BYTE  abCapBuffer[MAX_BUFFER_SIZE];
static short iCapBuffer;

// Define low level capture buffer routines.
static USHORT CaptureClose(void);
static USHORT CaptureWrite(char *pBuffer, short iBuffer);


  char szCaptureName[16];
  char fCapturePause;
  char szCaptureBuf[136];
  int  nCaptureInx;

int GetCaptureFile(HWND, int, char *, int);

int CmdCapture(szFile, fNew)
char *szFile;
int  fNew;
{
	char sBuf[96], szLog[64];

	if (bCaptureOn)
	{
		if (nCaptureInx)
			FlushCaptureBuf();

		CaptureClose();
		bCaptureOn = FALSE;
		szCaptureName[0] = NULL;
		CheckMenuItem(hWndMenu, CMD_CAPTURE, MF_UNCHECKED);
		ChangeMenuMsg(CMD_CAPTURE, MSGMNU_CAPTURE1);
		EnableMenuItem(hWndMenu, CMD_COPYCAPTURE, MF_GRAYED);
	}
	else
	{
		if (szFile == 0)
		{
			if (Sess.szLog[0] == NULL || Sess.szLog[0] == ' ')
			{
				char *ptr;
				register i;

				if (fUntitled)
				{
					szLog[0] = 'L';
					szLog[1] = 'O';
					szLog[2] = 'G';
					szLog[3] = NULL;
				}
				else
					strcpy(szLog, szDataCurFile);

				if (ptr = strchr(szLog, '.'))
					*ptr = NULL;

				i = strlen(szLog);
				_strdate(sBuf);
				szLog[i++] = '.';
				if (sBuf[0] == '0')
					szLog[i++] = sBuf[1];
				else
					szLog[i++] = (char)('A' + sBuf[1] - '0');
				szLog[i++] = sBuf[3];
				szLog[i++] = sBuf[4];
				szLog[i] = NULL;

				if (!IfSystemStatus(SS_SCRIPT))
					if (!GetCaptureFile(hWnd, MSG_LOG_FILE, szLog, sizeof(szLog)))
						return(0);
			}
			else
				LibEnstore(Sess.szLog, szLog, sizeof(szLog));
		}
		else
			strcpy(szLog, szFile);

		if (strchr(szLog, '.') == 0)
		{
			char *ptr;
			register i;

			if (ptr = strchr(szLog, '.'))
				*ptr = NULL;

			i = strlen(szLog);
			_strdate(sBuf);
			szLog[i++] = '.';
			if (sBuf[0] == '0')
				szLog[i++] = sBuf[1];
			else
				szLog[i++] = (char)('A' + sBuf[1] - '0');
			szLog[i++] = sBuf[3];
			szLog[i++] = sBuf[4];
			szLog[i] = NULL;
		}

		// dca00073 PJL Check for 'Invalid Drive' & 'Drive not Ready' errors.
		if (nErrorCode = SetFullPath(VAR_DIRFIL))
			return(nErrorCode);
		if (nErrorCode = SetPath(szLog))
			return(nErrorCode);

		strcpy(sBuf, StripPath(szLog));

		if (fNew)
			CreateXferFile(sBuf, (int *)&hfCapFile);
		else
		{
			USHORT usAction;

			if (!DosOpen(szLog, &hfCapFile, &usAction, 0L, FILE_NORMAL,
						FILE_OPEN | FILE_CREATE, OPEN_ACCESS_READWRITE, 0L))
				SeekToEndOfFile(hfCapFile);
		}

		if (hfCapFile == -1)
		{
			if (wSystemState == SS_PROGRAM)
				return(ERRSYS_OPENFAULT);
			else
				ErrorMessage(MSG_BADCAPTUREFILE);
			return(0);
		}

		iCapBuffer = 0;
		nCaptureInx = 0;
		bCaptureOn = TRUE;
		fCapturePause = FALSE;
		strcpy(szCaptureName, sBuf);
		ChangeMenuMsg1(CMD_CAPTURE, MSGMNU_CAPTURE2, szCaptureName);
		CheckMenuItem(hWndMenu, CMD_CAPTURE, MF_CHECKED);
		EnableMenuItem(hWndMenu, CMD_COPYCAPTURE, MF_ENABLED);
	}

	return(0);
}


//
// Write Raw Data to the Current Capture File.
//
//
void WriteRawToCapture(char *buf, short sLen)
  {
  if (!bCaptureOn || !sLen || fCapturePause)
    return;

  if (CaptureWrite(buf, sLen))
    {
    CmdCapture(NULL, NULL);
    GeneralError(ERRDOS_DISKFULL);
    }
  }


//
// Write ASCIIZ String to the Current Capture File.
//
//
void WriteStrToCapture(char *psz)
  {
  USHORT usBytes;
  register i, sLen;


  if (!bCaptureOn || fCapturePause || *psz == '\0')
    return;

  // Trim the blanks from the end of the string.
  sLen = StrRightTrim(psz);

  // Mask out the hi-bit in the string.
  for (i = 0; i < sLen; i++)
    psz[i] &= 0x7F;

  if (CaptureWrite(psz, sLen))
    {
    CmdCapture(NULL, NULL);
    GeneralError(ERRDOS_DISKFULL);
    }
  else
    {
    char szBuf[2];

    szBuf[0] = '\r';
    szBuf[1] = '\n';

    if (CaptureWrite(szBuf, 2))
      {
      CmdCapture(NULL, NULL);
      GeneralError(ERRDOS_DISKFULL);
      }
    }
  }


//
// CaptureChars
//
// Returns the number of characters in the capture file/buffer.
//
ULONG CaptureChars()
  {
  FILESTATUS fsts;


  if (!bCaptureOn)
    return (0L);

  DosQFileInfo(hfCapFile, 1, &fsts, sizeof(fsts));

  return (fsts.cbFile + iCapBuffer);
  }


void FlushCaptureBuf()
  {
  register i;


  if (fCapturePause)
    {
    nCaptureInx = 0;
    return;
    }

  if (bCaptureOn && nCaptureInx)
    {
    char szBuf[2];

    while (nCaptureInx)
      {
      if (szCaptureBuf[nCaptureInx-1] == ' ')
	nCaptureInx--;
      else
	break;
      }

    if (CaptureWrite(szCaptureBuf, nCaptureInx))
      {
      CmdCapture(NULL, NULL);
      GeneralError(ERRDOS_DISKFULL);
      }

    szBuf[0] = '\r';
    szBuf[1] = '\n';

    if (CaptureWrite(szBuf, 2))
      {
      CmdCapture(NULL, NULL);
      GeneralError(ERRDOS_DISKFULL);
      }
    }

  nCaptureInx = 0;
  }


  char *szResponseBuf;
  int  nResponseLen;

  BOOL DlgCapture(HWND, unsigned, WORD, LONG);

int GetCaptureFile(hWnd, nMsgNo, szBuf, nLen)
  HWND hWnd;
  int  nMsgNo;
  char *szBuf;
  int  nLen;
{
  FARPROC lpDlg;
  register i;


  szResponseBuf  = szBuf;
  nResponseLen	 = nLen;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_CAPTURE;

  lpDlg = MakeProcInstance((FARPROC)DlgCapture, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_CAPTURE), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);

  return (i);
}


BOOL DlgCapture(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
	char szBuf[MAX_PATH];
	register int i;

	switch (message)
	{
	case WM_INITDIALOG:
		// dca00073 PJL Check for 'Invalid Drive/Drive not Ready' errors.
		if (SetFullPath(VAR_DIRFIL))
		{
			EndDialog(hDlg, FALSE);
			return(FALSE);
		}

		szBuf[0] = NULL;
		DlgDirList(hDlg, szBuf, NULL, ID_PATH, NULL);
		SetDlgItemLength(hDlg, ID_EDIT, nResponseLen-1);
		SetDlgItemText(hDlg, ID_EDIT, szResponseBuf);

		if (Sess.cLogOption == SESSION_NORMAL)
			i = ID_SESSION_NORMAL;
		else if (Sess.cLogOption == SESSION_RAW)
			i = ID_SESSION_RAW;
		else if (Sess.cLogOption == SESSION_VISUAL)
			i = ID_SESSION_VISUAL;
		else
			i = ID_SESSION_MANUAL;
		CheckDlgButton(hDlg, i, TRUE);

		return(TRUE);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hDlg, ID_EDIT, szResponseBuf, nResponseLen);

			if (IsDlgButtonChecked(hDlg, ID_SESSION_NORMAL))
				Sess.cLogOption = SESSION_NORMAL;
			else if (IsDlgButtonChecked(hDlg, ID_SESSION_RAW))
				Sess.cLogOption = SESSION_RAW;
			else if (IsDlgButtonChecked(hDlg, ID_SESSION_VISUAL))
				Sess.cLogOption = SESSION_VISUAL;
			else
				Sess.cLogOption = SESSION_MANUAL;

			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;

		case ID_SESSION_NORMAL:
		case ID_SESSION_RAW:
		case ID_SESSION_VISUAL:
		case ID_SESSION_MANUAL:
			CheckRadioButton(hDlg, ID_SESSION_NORMAL, ID_SESSION_MANUAL, wParam);
			break;

		default:
			return(FALSE);
			break;
		}
		break;

	default:
		return(FALSE);
		break;
	}

	return(TRUE);
}


//
// CaptureClose
//
// Flush the capture buffer and close the capture file.
//
static USHORT CaptureClose(void)
  {
  USHORT usBytes;
  register USHORT usResults;


  if (iCapBuffer)
    {
    usResults = DosWrite(hfCapFile, abCapBuffer, iCapBuffer, &usBytes);
    DosClose(hfCapFile);
    hfCapFile = -1;
    if (usResults)
      return (XlatError(usResults));
    if (usBytes != iCapBuffer)
      return (ERRDOS_DISKFULL);
    }
  else
    {
    usResults = DosClose(hfCapFile);
    hfCapFile = -1;
    }

  return (usResults);
  }


//
// CaptureWrite
//
// Write to the capture file buffer and auto-flush if full.
//
static USHORT CaptureWrite(char *pBuffer, short iBuffer)
  {
  USHORT usBytes;
  register USHORT usResults;
  register i;


  usResults = 0;
  for (i = 0; i < iBuffer; i++)
    {
    abCapBuffer[iCapBuffer++] = *pBuffer++;
    if (iCapBuffer >= MAX_BUFFER_SIZE)
      {
      usResults = DosWrite(hfCapFile, abCapBuffer, iCapBuffer, &usBytes);
      iCapBuffer = 0;

      if (usResults)
	return (XlatError(usResults));
      else if (usBytes == 0)
	return (ERRDOS_DISKFULL);
      }
    }

  return (0);
  }
