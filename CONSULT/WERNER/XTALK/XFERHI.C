/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communication Associates Inc.                   *
*                All Rights Reserved.
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   File Transfer Common Module (XFERHI.C)                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the common logic used to keep the xfer     *
*             on the screen and looking pretty.                               *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.                                  		   *
*	1.01 08/02/89 00006    Display timeout errors during uploading     		   *
*		    			   using XMODEM to a non-responsing host.    		   *
*  1.01 08/09/89 00009    Even out the xfer rate displayed.           		   *
*  1.01 08/15/89 00017    Clear the old # of # from the xfer dialog.  		   *
*  1.01 09/28/89 00040    Permit hiding of the xfer dialog window.    		   *
*  1.01 01/31/90 dca00008 JDB Fix Kermit File xfer dialog coming   		   *
*                         back from hide                                      *
*  1.1 03/07/90 dca00042 PJL Fixed Block Size file transfer display.		   *
*  1.1 03/30/90 dca00062 PJL Fix XWP extension addition problem in File.Open. *
*  1.1 03/29/90 dca00063 PJL Fixed strange xfer error report messages.		   *
*	1.1 04/08/90 dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <direct.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"
#include "term.h"
#include "xfer.h"
#include "kermit.h"
#include "variable.h"


struct stXfer Xfer;

union FILEXFER filexfer;

// 00040 True if the xfer dialog is hidden and we are to use the info line.
  BOOL fHideXferWindow;


/*---------------------------------------------------------------------------*/
/*  Define variables used in handling the file currently being transfered.   */
/*---------------------------------------------------------------------------*/
  char	szXferFilePath[MAX_PATH];
  WORD	wXferTotalErrors;
  WORD	wXferConsecErrors;
  WORD	wXferLastError;

  WORD	 wXferTotalFiles;		/* Total number of files to xfer    */
  DWORD dwXferTotalBytes;		/* Total number of bytes to xfer    */
  WORD	 wXferCurrFiles;		/* Current file number being xfer   */
  DWORD dwXferCurrBytes;		/* Total bytes xfer so far	    */
  long	 lXferFileBytes;		/* Bytes xfer in active file xfer   */
  long	 lXferFileSize; 		/* File size of active file xfer    */
  int	 nXferBlockSize;		/* Size of last block xfer (info)   */
  WORD	 wXferBlockCount;		/* Number of blocks in active xfer  */
  char	 fXferDirection;		/* True for sending, else receiving */
  char	 fXferChangedComm;		/* The comm params changed for xfer */

  int	 nXferRcvWait;			/* Wait for idle on line then abort */


  void near CmdXfer(BOOL);
  BOOL DlgXfer(HWND, unsigned, WORD, LONG);


static long lXferTimeCheck;
       long lXferByteCheck;
static WORD wXferErrorCheck;
static WORD wXferFileCheck;
static WORD wXferConsecCheck;


// 00009 To support an even file transfer cps display remember the last four
// 00009   CPS ratings and average them together.
static USHORT ausCps[4];    // Storage for the last four CPS rates.
static USHORT iCps;         // Index for CPS, mostly use when we are starting.


void InitXferIndicators()
{
  char buf[256];
  register WORD i;


  //dca00008 JDB fHideXferWindow = FALSE;
  //dca00008 JDB CheckMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_CHECKED);

  if (hKermitChild)
    LoadString(hWndInst, IDS_PROTOCOL_KERMIT, buf, sizeof(buf));
  else
    LoadString(hWndInst, IDS_PROTOCOL_FIRST+Xfer.cProtocol, buf, sizeof(buf));
  i = strlen(buf);
  GetWindowText(hXferDlg, &buf[i], sizeof(buf) - i);
  if (buf[i] == ' ')
    SetWindowText(hXferDlg, buf);

  wXferCurrFiles    = 0;
  dwXferCurrBytes   = 0L;

  lXferFileBytes    = 0L;

  if (Xfer.cProtocol <= XFER_DART || Xfer.cProtocol > XFER_KERMIT)
    ResetXferIndicators(TRUE);
  else
    ResetXferIndicators(FALSE);
}


void ResetXferIndicators(char fServer)
{
  char buf[256];
  register i;


  wXferBlockCount   = 0;
  wXferTotalErrors  = 0;
  wXferConsecErrors = 0;
  wXferLastError    = 0;
  lXferTimeCheck    = GetCurrentTime();
  iCps = 0;     // 00009 Initialize Cps rating index.

  lXferByteCheck    = 0L;
  wXferErrorCheck   = 0;
  wXferFileCheck    = 0;
  wXferConsecCheck  = 0;


  if (fServer)
    {
    buf[ 0] = 'W';
    buf[ 1] = 'a';
    buf[ 2] = 'i';
    buf[ 3] = 't';
    buf[ 4] = 'i';
    buf[ 5] = 'n';
    buf[ 6] = 'g';
    buf[ 7] = ' ';
    buf[ 8] = NULL;
    }
  else if (fXferDirection)
    {
    buf[ 0] = 'S';
    buf[ 1] = 'e';
    buf[ 2] = 'n';
    buf[ 3] = 'd';
    buf[ 4] = 'i';
    buf[ 5] = 'n';
    buf[ 6] = 'g';
    buf[ 7] = ' ';
    buf[ 8] = NULL;
    }
  else
    {
    buf[ 0] = 'R';
    buf[ 1] = 'e';
    buf[ 2] = 'c';
    buf[ 3] = 'e';
    buf[ 4] = 'i';
    buf[ 5] = 'v';
    buf[ 6] = 'i';
    buf[ 7] = 'n';
    buf[ 8] = 'g';
    buf[ 9] = ' ';
    buf[10] = NULL;
    }
  if (szXferFilePath[0])
    {
    register j;

    j = strlen(buf);
    for (i = 0; szXferFilePath[i]; i++)
      {
      if (szXferFilePath[i] == '&')
        buf[j++] = '&';
      buf[j++] = szXferFilePath[i];
      }
    buf[j] = NULL;
    }
  else
    {
    i = strlen(buf);
    buf[i++] = 'N';
    buf[i++] = '/';
    buf[i++] = 'A';
    buf[i  ] = NULL;
    }

  SetDlgItemText(hXferDlg, ID_XFER_TITLE, buf);

  if (fXferDirection)
    {
    buf[ 0] = 'B';
    buf[ 1] = 'y';
    buf[ 2] = 't';
    buf[ 3] = 'e';
    buf[ 4] = 's';
    buf[ 5] = ' ';
    buf[ 6] = 's';
    buf[ 7] = 'e';
    buf[ 8] = 'n';
    buf[ 9] = 't';
    buf[10] = ':';
    buf[11] = NULL;
    }
  else
    {
    buf[ 0] = 'B';
    buf[ 1] = 'y';
    buf[ 2] = 't';
    buf[ 3] = 'e';
    buf[ 4] = 's';
    buf[ 5] = ' ';
    buf[ 6] = 'r';
    buf[ 7] = 'e';
    buf[ 8] = 'c';
    buf[ 9] = 'v';
    buf[10] = 'd';
    buf[11] = ':';
    buf[12] = NULL;
    }
  SetDlgItemText(hXferDlg, ID_XFER_NAME, buf);

  SetDlgItemInt(hXferDlg, ID_XFER_TOTAL, wXferTotalErrors, FALSE);
  SetDlgItemInt(hXferDlg, ID_XFER_CONSEC, wXferConsecErrors, FALSE);
  buf[0] = NULL;
  SetDlgItemText(hXferDlg, ID_XFER_LAST, buf);
  SetDlgItemText(hXferDlg, ID_XFER_MSG, buf); // 00017 Clear old # of #.

  buf[0] = 'N';
  buf[1] = '/';
  buf[2] = 'A';
  buf[3] = NULL;
  SetDlgItemText(hXferDlg, ID_XFER_SIZE, buf);
  SetDlgItemText(hXferDlg, ID_XFER_EST,  buf);
  SetDlgItemText(hXferDlg, ID_XFER_CPS,  buf);

  SetDlgItemInt(hXferDlg, ID_XFER_BLOCK, 0, FALSE);
  SetDlgItemInt(hXferDlg, ID_XFER_BYTES, 0, FALSE);

  if (lXferFileSize != 0L)
    SetDlgItemLong(hXferDlg, ID_XFER_OFBYTES, lXferFileSize, FALSE);
  else
    SetDlgItemText(hXferDlg, ID_XFER_OFBYTES, buf);

  PaintXferGraphics(GetDlgItem(hXferDlg, ID_XFER_GRAPHICS));
}


/*---------------------------------------------------------------------------*/
/*									     */
/*---------------------------------------------------------------------------*/
void UpdateXferIndicators()
{
	char buf[96], fmt[64];
	WORD wCPS, wSec, wMin, i;

	if (lXferFileBytes != lXferByteCheck && GetCurrentTime() != lXferTimeCheck)
	{
		// 00040 If the file transfer window is hidden then use the info bar.
		if (fHideXferWindow)
		{
			if (fXferDirection)
				sprintf(buf, "Uploaded %ld bytes...", lXferFileBytes);
			else
				sprintf(buf, "Downloaded %ld bytes...", lXferFileBytes);

			InfoDisplay(buf);
		}

		if (lXferFileBytes)
		{
			register i;

			SetDlgItemInt(hXferDlg, ID_XFER_BLOCK, wXferBlockCount, FALSE);
			// dca00042 PJL Display the proper Block (Packet) Size.
			SetDlgItemInt(hXferDlg, ID_XFER_SIZE, Xfer.nPacketSize, FALSE);
			SetDlgItemLong(hXferDlg, ID_XFER_BYTES, lXferFileBytes, FALSE);

			if (lXferByteCheck)
			{
				wCPS = (WORD)(((lXferFileBytes - lXferByteCheck) * 1000L) /
						(GetCurrentTime() - lXferTimeCheck));

				// 00009  Lets compute the average CPS rate from the last four.
				for (i = sizeof(ausCps)/sizeof(USHORT) - 1; i > 0; i--)
					ausCps[i] = ausCps[i-1];
				ausCps[0] = wCPS;
				if (iCps++ == 4)
					iCps = 4;
				wCPS = 0;
				for (i = 0; i < iCps; i++)
					wCPS += ausCps[i];
				wCPS /= iCps;
			}
			else
				wCPS = 0;
		}
		else
			wCPS = 0;

		lXferTimeCheck = GetCurrentTime();

		if (wCPS)
		{
			fmt[0] = '%';
			fmt[1] = 'd';
			fmt[2] = ' ';
			fmt[3] = 'c';
			fmt[4] = 'p';
			fmt[5] = 's';
			fmt[6] = NULL;
			sprintf(buf, fmt, wCPS);
			SetDlgItemText(hXferDlg, ID_XFER_CPS, buf);
		}

		if (lXferFileSize != 0L)
		{
			if (wCPS)
				wSec = (WORD)((lXferFileSize - lXferFileBytes) / (long)wCPS);
			else
				wSec = 0;

			fmt[ 0] = '%';
			fmt[ 1] = '0';
			fmt[ 2] = 'd';
			fmt[ 3] = ':';
			fmt[ 4] = '%';
			fmt[ 5] = '0';
			fmt[ 6] = '2';
			fmt[ 7] = 'd';
			fmt[ 8] = ':';
			fmt[ 9] = '%';
			fmt[10] = '0';
			fmt[11] = '2';
			fmt[12] = 'd';
			fmt[13] = NULL;
			sprintf(buf, fmt, wSec / 3600, (wSec / 60) % 60, wSec % 60);
			SetDlgItemText(hXferDlg, ID_XFER_EST, buf);

			PaintXferGraphics(GetDlgItem(hXferDlg, ID_XFER_GRAPHICS));

			if (lXferFileBytes >= lXferFileSize)
			{
				buf[0] = '1';
				buf[1] = '0';
				buf[2] = '0';
			}
			else
			{
				i = (int)((lXferFileBytes * 100) / lXferFileSize);
				buf[0] = ' ';
				if (i < 10)
					buf[1] = ' ';
				else
					buf[1] = (char)('0' + i / 10);
				buf[2] = (char)('0' + i % 10);
			}

			buf[3] = '%';
			buf[4] = NULL;
		}
		else
		{
			fmt[0] = '%';
			fmt[1] = '0';
			fmt[2] = '4';
			fmt[3] = 'd';
			fmt[4] = NULL;
			sprintf(buf, fmt, wXferBlockCount);
		}

		IconTextChanged(buf);

		lXferByteCheck = lXferFileBytes;
		//lXferTimeCheck = GetCurrentTime();
	}

	// 00006 Display XMODEM no-response timeout to users.
	// 00006 if (wXferTotalErrors != wXferErrorCheck || (wXferConsecErrors != wXferConsecCheck) && lXferFileSize != 0L)
	if (wXferTotalErrors != wXferErrorCheck || (wXferConsecErrors != wXferConsecCheck))
	{
		SetDlgItemInt(hXferDlg, ID_XFER_TOTAL, wXferTotalErrors, FALSE);
		SetDlgItemInt(hXferDlg, ID_XFER_CONSEC, wXferConsecErrors, FALSE);

		// dca00063 PJL Fixed xfer error report messages.
		switch (wXferLastError)
		{
		case XFER_ERROR_TIMEOUT:
			buf[0] = 'T';
			buf[1] = 'i';
			buf[2] = 'm';
			buf[3] = 'e';
			buf[4] = 'o';
			buf[5] = 'u';
			buf[6] = 't';
			buf[7] = NULL;
			break;

		case XFER_ERROR_SEQ:
			buf[0] = 'S';
			buf[1] = 'e';
			buf[2] = 'q';
			buf[3] = 'u';
			buf[4] = 'e';
			buf[5] = 'n';
			buf[6] = 'c';
			buf[7] = 'e';
			buf[8] = NULL;
			break;

		case XFER_ERROR_NAK:
			buf[0] = 'N';
			buf[1] = 'A';
			buf[2] = 'K';
			buf[3] = NULL;
			break;

		case XFER_ERROR_DUP:
			buf[0] = 'D';
			buf[1] = 'U';
			buf[2] = 'P';
			buf[3] = NULL;
			break;

		default:
			buf[0] = 'B';
			buf[1] = 'a';
			buf[2] = 'd';
			buf[3] = ' ';
			buf[4] = 'P';
			buf[5] = 'a';
			buf[6] = 'c';
			buf[7] = 'k';
			buf[8] = 'e';
			buf[9] = 't';
			buf[10] = NULL;
			break;
		}
		SetDlgItemText(hXferDlg, ID_XFER_LAST, buf);

		fmt[0] = 'E';
		fmt[1] = '%';
		fmt[2] = '0';
		fmt[3] = '3';
		fmt[4] = 'd';
		fmt[5] = NULL;
		sprintf(buf, fmt, wXferConsecErrors);
		IconTextChanged(buf);

		wXferErrorCheck = wXferTotalErrors;
		wXferConsecCheck = wXferConsecErrors;
	}

	if (wXferFileCheck != wXferCurrFiles && wXferTotalFiles == 0)
	{
		LoadString(hWndInst, MSGXFR_FILENO, fmt, sizeof(fmt));
		sprintf(buf, fmt, wXferCurrFiles);
		SetDlgItemText(hXferDlg, ID_XFER_MSG, buf);
		wXferFileCheck = wXferCurrFiles;
	}
	else if (wXferFileCheck != wXferCurrFiles && wXferTotalFiles)
	{
		if (wXferCurrFiles > wXferTotalFiles)
			wXferTotalFiles = wXferCurrFiles;
		LoadString(hWndInst, MSGXFR_FILEOF, fmt, sizeof(fmt));
		sprintf(buf, fmt, wXferCurrFiles, wXferTotalFiles);
		SetDlgItemText(hXferDlg, ID_XFER_MSG, buf);
		wXferFileCheck = wXferCurrFiles;
	}
}


void PaintXferGraphics(HWND hCtl)
  {
  HBRUSH hBrush;
  RECT	 Rect;
  HDC	 hDC;
  register i;


  GetClientRect(hCtl, &Rect);
  hDC = GetDC(hCtl);

  if (lXferFileSize == 0L)
    {
    FillRect(hDC, &Rect, GetStockObject(LTGRAY_BRUSH));
    ReleaseDC(hCtl, hDC);
    return;
    }

  if (fMonoDisplay)
    hBrush = GetStockObject(LTGRAY_BRUSH);
  else
    hBrush = CreateSolidBrush(RGB(0, 255, 0));

  i = Rect.right;
  Rect.right = (int)(((long)Rect.right * lXferFileBytes) / lXferFileSize);
  FillRect(hDC, &Rect, hBrush);
  if (!fMonoDisplay)
    DeleteObject(hBrush);

  Rect.left  = Rect.right;
  Rect.right = i;
  if (fMonoDisplay)
    hBrush = CreateSolidBrush(RGB(0, 0, 0));
  else
    hBrush = CreateSolidBrush(RGB(255, 0, 0));
  FillRect(hDC, &Rect, hBrush);
  DeleteObject(hBrush);

  SelectObject(hDC, GetStockObject(BLACK_PEN));

  for (i = 1; i < 4; i++)
    {
    register j;

    j = (Rect.right / 4) * i;
    MoveTo(hDC, j, Rect.bottom / 2);
    LineTo(hDC, j, Rect.bottom);
    }

  for (i = 1; i < 10; i++)
    {
    register j;

    if (i != 5)
      {
      j = (Rect.right / 10) * i;
      MoveTo(hDC, j, Rect.bottom - (Rect.bottom / 4));
      LineTo(hDC, j, Rect.bottom);
      }
    }

  ReleaseDC(hCtl, hDC);
  }


void CmdSend()
{

	CmdXfer(TRUE);
}


void CmdReceive()
{

	CmdXfer(FALSE);
}


void near CmdXfer(bDir)
BOOL bDir;
{
	register i;
	char pbuf[30];

	if (wSystem & SS_XFER)
		return;

  if (!bConnected)
  {
    ErrorMessage(MSG_BECONNECTED);
    return;
  }

  fXferDirection = (char)bDir;

  switch (Xfer.cProtocol)
  {
    case XFER_DART:
      if (fXferDirection)
        i = XferDartSendName(NULL);
      else
        i = XferDartRecvName(NULL);
      break;

    case XFER_XTALK:
      if (fXferDirection)
        i = XferCrossSendName(NULL);
      else
        i = XferCrossRecvName(NULL);
      break;

    case XFER_ZMODEM:
      if (fXferDirection)
        i = XferZmodemSendName(NULL);
      else
        i = XferZmodemRecvName(NULL);
      break;

    case XFER_MODEM7:
    case XFER_XMODEM:
    case XFER_XMODEM1K:
      if (fXferDirection)
        i = XferXModemSendName(Xfer.cProtocol, NULL);
      else
        i = XferXModemRecvName(Xfer.cProtocol, NULL);
      break;

    case XFER_YMODEM:
      if (fXferDirection)
        i = XferYModemSendName(NULL);
      else
        i = XferYModemRecvName(NULL);
      break;

    case XFER_CSERVEB:
      if (fXferDirection)
        i = XferQuickBSendName(NULL);
      else
        i = XferQuickBRecvName(NULL);
      break;

    case XFER_KERMIT:
      if (fXferDirection)
        i = XferKermitSendName(NULL);
      else
        i = XferKermitRecvName(NULL);
      break;
    }

	switch(i)
	{
	case 0:
	case ERRXFR_LOCALCANCEL:
	case ERRSYS_OPENFAULT:
	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
	case ERRSYS_BADDRIVE:
	case ERRSYS_DRIVENOTREADY:
		break;

	default:	
		GeneralError(i);
		break;
	}
}


int XferUploadBatch(char *mask, char *szFileNameIn)
{
	OFSTRUCT FS;
	int extfile;
	char szExtIn[128];

	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
	if (SetFullPath(VAR_DIRFIL))
		return(-1);

	strcpy(szFileNameIn, "");
	strcpy(szExtIn, mask);

	FS.cBytes = 0;
	if (DlgSend(hWndInst, hWnd, IDD_BATCH, &FS, &extfile,
			szFileNameIn, szExtIn, szAppName) == NOOPEN)
		return(-1);

	if (extfile != -1)
		DosClose(extfile);

	return(NULL);
}


int XferUploadFile(char *mask, OFSTRUCT *pFS)
{
	int extfile;
	char szFileNameIn[128], szExtIn[128];

	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
	if (SetFullPath(VAR_DIRFIL))
		return(-1);

	strcpy(szFileNameIn, "");
	strcpy(szExtIn, mask);

	pFS->cBytes = 0;
	// dca00062 PJL Fix XWP extension addition problem in File.Open.
	if (DlgOpen(hWndInst, hWnd, IDD_UPLOAD, pFS, &extfile,
				szFileNameIn, szExtIn, szAppName, FALSE, FALSE) == NOOPEN)
		extfile = -1;

	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
	if (SetPath(pFS->szPathName))
		return(-1);

	return(extfile);
}


int XferDownloadFile(char *mask, OFSTRUCT *pFS)
{
	int extfile;
	char szFileNameIn[128], szExtIn[50];

	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
	if (SetFullPath(VAR_DIRFIL))
		return(-1);

	if (sTermFileName[1] == ';')
		strcpy(szFileNameIn, &sTermFileName[2]);
	else
		strcpy(szFileNameIn, sTermFileName);

	strcpy(szExtIn, mask);

	pFS->cBytes = 0;
	if (DlgSaveAs(hWndInst, hWnd, IDD_DOWNLOAD, pFS, &extfile,
			szFileNameIn, szExtIn, szAppName, TRUE, FALSE) == NOSAVE)
		extfile = -1;

	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
	if (SetPath(pFS->szPathName))
		return(-1);

	return(extfile);
}
