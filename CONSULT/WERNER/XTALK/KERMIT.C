/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communication Associates Inc.                   *
*                     All Rights Reserved.
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Startup (KERMIT.C)                                       *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the Kermit startup and init logic.         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/08/89 00007 Change Kermit logic to not use the CR char  *
*                                   as an event flag.  Removed code.          *
*             1.01 01/31/90 dca00008 JDB Fix kermit file xfer dialog poping up*
* 1.1 04/09/90 dca00077 MKL changed default block check type to 1	      *
* 1.1 04/17/90 dca00085 MKL fixed that Windx popos into the foreground after  *
*     transfer is done in the background.				      *
* 1.1 04/17/90 dca00091 MKL fixed Kermit not posting error on receiving file  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "prodata.h"
#include "xfer.h"
#include "kermit.h"
#include "variable.h"
#include "dialogs.h"

  extern BOOL DlgKermit(HWND, unsigned, WORD, LONG);


  BOOL bKermitNonServer;


void CmdKermit()
  {
  register i;


  if (nKermitMode != KM_NULL)
    {
    TermKermit();
    return;
    }

  if (InitKermit(FALSE) == FALSE)
    {
    MessageBox(GetFocus(), "Unable to start Kermit!", szAppName, MB_OK);
    }

  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendPacketX('E', 0, 0, NULL);
    GeneralError(i);
    TermKermit();
    }
  }


int XferKermitSendName(filename)
  char *filename;
{
  char	   szCmd[96];
  register i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendPacketX('E', 0, 0, NULL);
    return (i);
    }

  if (filename == NULL || *filename == NULL)
    {
    if (XferUploadBatch("\\*.*", szXferFilePath))
      return (ERRSYS_OPENFAULT);
    }
  else
    strcpy(szXferFilePath, filename);

  if (ComputeFilesToSend(szXferFilePath, &filexfer.kermit.sendfile) == 0)
    return (ERRXFR_NOFILEFOUND);

  fXferDirection = TRUE;

  szCmd[0] = 'S';
  szCmd[1] = 'E';
  szCmd[2] = 'N';
  szCmd[3] = 'D';
  szCmd[4] = ' ';
  strcpy(&szCmd[5], szXferFilePath);

  i = KermitCommand(szCmd, KRMSRC_SCRIPT);
  bKermitNonServer = TRUE;
  if (i)
    return (i);
  return (0);
}


int XferKermitRecvName(filename)
  char *filename;
{
  register i;

  if (InitKermit(TRUE) == FALSE)
    return (TRUE);

  if (i = SetFullPath(VAR_DIRFIL))
    {
    fXferDirection = FALSE;
    SendPacketX('E', 0, 0, NULL);
    TermKermit();
    return (i);
    }

  fXferDirection = FALSE;

  if (i = KermitReceive(filename))
    TermKermit();
  bKermitNonServer = TRUE;

  return (i);
}


int XferKermitRequestName(filename)
  char *filename;
{
  register i;


  if (i = SetFullPath(VAR_DIRFIL))
    {
    SendPacketX('E', 0, 0, NULL);
    return (i);
    }

  if (InitKermit(TRUE) == FALSE)
    return (TRUE);

  fXferDirection = FALSE;

  if (i = KermitGetFile(filename, NULL))
    TermKermit();

  return (i);
}


BOOL InitKermit(BOOL bServer)
{
  DCB	 sDCB;


  if (nKermitMode != KM_NULL)
    return FALSE;

  if (wSystem & SS_XFER)
    return (ERRXFR_BUSY);

  bKermitNonServer = bServer;

  if (!CreateKermitChild(hWnd))
    return FALSE;

  memset(&filexfer.kermit, 0, sizeof(filexfer.kermit));

  //dca00008 JDB Fix kermit file xfer dialog coming back from hide
  fHideXferWindow = FALSE;	//dca00008
  CheckMenuItem(hWndMenu,CMD_WINDOW_XFER, MF_CHECKED);  //dca00008

  nKermitMode = KM_IDLE;

  lXferFileBytes    = 0L;
  lXferFileSize     = 0L;
  wXferConsecErrors = 0;
  wXferBlockCount   = 0;
  szXferFilePath[0] = '\0';

  filexfer.kermit.bAbortFile	  = FALSE;
  filexfer.kermit.bAbortBatch	  = FALSE;
  filexfer.kermit.fAbortKermit	  = FALSE;

  filexfer.kermit.fBellOn	      = TRUE;
  // dca00077 MKL
  // filexfer.kermit.cBlockCheck	      = 3;
  filexfer.kermit.cBlockCheck	      = 1;
  filexfer.kermit.nPause	      = 0;
  filexfer.kermit.fEofCtrlZ	      = FALSE;
  filexfer.kermit.fFileNamesConverted = TRUE;
  filexfer.kermit.fFileSupersede      = FALSE;
  filexfer.kermit.fFileWarning	      = TRUE;
  filexfer.kermit.fFileKeep	      = FALSE;
  filexfer.kermit.nInitialErrors      = 2;
  // dca00091 MKL
  // filexfer.kermit.nPacketErrors       = 16;
  filexfer.kermit.nPacketErrors       = 4;
  filexfer.kermit.fTakeEcho	      = FALSE;
  filexfer.kermit.fTakeError	      = FALSE;
  filexfer.kermit.fTimer	      = TRUE;

  filexfer.kermit.nSndPacketSize  = PACKET_SIZE;
  filexfer.kermit.nSndDataOffset  = 3;
  filexfer.kermit.nSndTimeOut	  = LOCAL_TIMEOUT;
  filexfer.kermit.cSndStartChar   = START_CHAR;
  filexfer.kermit.cSndCtlq	  = CONTROL_QUOTE;
  filexfer.kermit.nSndPadNum	  = PADDING_NUM;
  filexfer.kermit.cSndPadChar	  = PADDING_CHAR;
  filexfer.kermit.cSndEOL	  = EOL_CHAR;
  filexfer.kermit.bSndRepeatPrefixFlag = TRUE;
  filexfer.kermit.cSndRepeatPrefix     = REPEAT_PREFIX;
  filexfer.kermit.bSndPacketSizeOverride = FALSE;
  filexfer.kermit.bSndTimeOutOverride	 = FALSE;

  filexfer.kermit.nRcvPacketSize  = PACKET_SIZE;
  filexfer.kermit.nRcvDataOffset  = 3;
  // dca00091 MKL
  // filexfer.kermit.nRcvTimeOut	  = REMOTE_TIMEOUT;
  filexfer.kermit.nRcvTimeOut	  = 6;
  filexfer.kermit.cRcvStartChar   = START_CHAR;
  filexfer.kermit.cRcvCtlq	  = CONTROL_QUOTE;
  filexfer.kermit.nRcvPadNum	  = PADDING_NUM;
  filexfer.kermit.cRcvPadChar	  = PADDING_CHAR;
  filexfer.kermit.cRcvEOL	  = EOL_CHAR;
  filexfer.kermit.bRcvRepeatPrefixFlag = TRUE;
  filexfer.kermit.cRcvRepeatPrefix     = REPEAT_PREFIX;

  filexfer.kermit.cEightBitPrefix = EIGHT_BIT_PREFIX;
  filexfer.kermit.hFile = -1;

  filexfer.kermit.nCycleDelay = 200;

  filexfer.kermit.lStatFileChars = 0L;
  filexfer.kermit.lStatCommIn	 = 0L;
  filexfer.kermit.lStatCommOut	 = 0L;
  filexfer.kermit.lStatStartTime = 0L;
  filexfer.kermit.lStatStopTime  = 0L;
  filexfer.kermit.fStatLastXfer  = TRUE;

  lpXferDlg = MakeProcInstance((FARPROC)DlgKermit, hWndInst);

  hXferDlg = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_XFER), hWnd, lpXferDlg);

  if (hXferDlg == 0)
    return FALSE;
  else
    LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  CheckMenuItem(hWndMenu, CMD_KERMIT, MF_CHECKED);

  InitXferIndicators();

  SetFocus(hKermitChild);

  return TRUE;
}


BOOL TermKermit()
{
  HANDLE hMEM;

  if (nKermitMode == KM_NULL)
    return FALSE;


  DestroyKermitChild();

  if (hXferDlg)
    {
    DestroyWindow(hXferDlg);
    UnLinkDialog(hXferDlg);
    hXferDlg = NULL;
    }

  // Close the kermit file transfer file, if openned.
  KermitClose();

  if (!fXferDirection && szXferFilePath[0] && !filexfer.kermit.fFileKeep)
      DosDelete(szXferFilePath, 0L);

  FreeProcInstance(lpXferDlg);
  lpXferDlg = 0L;

  IconTextChanged(NULL);
  nKermitMode = KM_NULL;

  ResetSystemStatus(SS_XFER);
  InfoDisplay(NULL);

  CheckMenuItem(hWndMenu, CMD_KERMIT, MF_UNCHECKED);

  ResetScreenDisplay();
  // dca00085 MKL fixed Focus problem so that Windx won't pops into the fore-
  //   ground after tansfer is done in the background
  // SetFocus(hWnd);
  if (hWnd == GetActiveWindow()) SetFocus(hWnd);

  return TRUE;
}
