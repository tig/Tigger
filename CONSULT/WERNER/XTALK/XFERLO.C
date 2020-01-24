/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   File Transfer Init Module (XFERLO.C)                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains common file transfer initialization logic. *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00040 Permit hiding of the xfer dialog window.    *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "xfer.h"
#include "term.h"
#include "kermit.h"
#include "prodata.h"
#include "global.h"
#include "variable.h"
#include "dialogs.h"

WORD crc_table[] =
  {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
  0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
  0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
  0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
  0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
  0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
  0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
  0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
  0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
  0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
  0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
  0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
  0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
  };


void InitFileTransfer()
  {
  }


/*
**  HideFileTransfer (00040)
**
**  Called from the menu bar to toggle the display state of the file transfer
**    dialog.
*/
extern long lXferByteCheck;
void HideFileTransfer()
  {
  win_assert(hXferDlg);
  win_assert(IfSystemStatus(SS_XFER));

  if (nKermitMode == KM_IDLE)
    return;

  if (fHideXferWindow)
    {
    fHideXferWindow = FALSE;
    ShowWindow(hXferDlg, SW_SHOW);
    InfoDisplay(NULL);
    lXferByteCheck = 1L;
    UpdateXferIndicators();
    CheckMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_CHECKED);
    }
  else
    {
    fHideXferWindow = TRUE;
    ShowWindow(hXferDlg, SW_HIDE);
    lXferByteCheck = 1L;
    UpdateXferIndicators();
    CheckMenuItem(hWndMenu, CMD_WINDOW_XFER, MF_UNCHECKED);
    }
  }


void TermFileTransfer()
  {
  }



BOOL InitProtocol()
{
  NewProtocolData();

  return (TRUE);
}

void NewProtocolData()
{
  Xfer.cProtocol	= XFER_XMODEM;
  Xfer.cLWait		= XFER_WAIT_NONE;
  Xfer.szLWaitPrompt[0] = 1;
  Xfer.szLWaitPrompt[1] = ':';
  Xfer.cLWaitCount	= 2;
  Xfer.nLWaitDelay	= 2000;
  Xfer.cCWait		= XFER_WAIT_NONE;
  Xfer.nCWaitDelay	= 250;
  Xfer.szLineDelim[0]	= 1;
  Xfer.szLineDelim[1]   = ASC_CR;
  Xfer.szBlankEx[0]	= 1;
  Xfer.szBlankEx[1]	= '.';
  Xfer.cWordWrap	= 0;
  Xfer.fTabEx           = 1;
  Xfer.cLineTime	= 10;
  Xfer.nPacketSize      = 256;
  Xfer.fRecovery	= 1;
  Xfer.fDateMode	= 0;
  Xfer.fHostMode        = 0;
  Xfer.cMaxErrors       = 20;
  Xfer.fPmode		= 1;
  Xfer.cTiming		= XFER_TIMING_NORMAL;
  Xfer.cFileType	= 'B';
  Xfer.cCheckType	= 3;
  Xfer.cHeader		= ASC_SOH;
  Xfer.cControlQuote	= '#';
  Xfer.cBinaryQuote	= '&';
  Xfer.cRecvMode	= XFER_RECV_NORMAL;

  UpdateProtocol();
}

void UpdateProtocol()
{
  char szBuf[32];

  LoadString(hWndInst, IDS_PROTOCOL_FIRST+Xfer.cProtocol, szBuf, sizeof(szBuf));
  ChangeMenuMsg1(CMD_SEND, MSGMNU_SEND, szBuf);
  ChangeMenuMsg1(CMD_RECEIVE, MSGMNU_RECEIVE, szBuf);
  ChangeMenuText(CMD_PROTOCOL, szBuf);

  if (!IfSystemStatus(SS_INIT))
    ResetSystemStatus(NULL);
}

WORD ComputeFilesToSend(char *szPath, struct find_t *FindBuffer)
  {
  register i;


  wXferTotalFiles  = 0;
  dwXferTotalBytes = 0L;


  if (SetPathFile(szPath))
    return (0);


  GlobalCompact(-1L);
  LockData(0);

  strcpy(szPath, StripPath(szPath));

  i = _dos_findfirst(szPath, _A_NORMAL, FindBuffer);
  while (i == 0)
    {
    wXferTotalFiles++;
    dwXferTotalBytes += FindBuffer->size;
    i = _dos_findnext(FindBuffer);
    }

  if (wXferTotalFiles)
    _dos_findfirst(szPath, _A_NORMAL, FindBuffer);

  UnlockData(0);

  return (wXferTotalFiles);
  }


/*---------------------------------------------------------------------------*/
/*  This subroutine is used by the file transfer code to create/open a file  */
/*  to down to.  It sets the correct path (DIRFIL) and and the BACKUP option */
/*  is set and the file already exist then it will rename it with the .BAK   */
/*  suffix.								     */
/*---------------------------------------------------------------------------*/
USHORT CreateXferFile(char *szName, int *pfh)
  {
  OFSTRUCT OfStruct;
  char	   szPath[MAX_PATH];
  char	   szBackupName[MAX_PATH];
  char	   *ptr;
  register i;


  if (fBackups)
    {
    if (OpenFile(szName, &OfStruct, OF_EXIST) != -1)
      {
      strcpy(szBackupName, szName);
      if ((ptr = strchr(szBackupName, '.')) == NULL)
	ptr = &szBackupName[strlen(szBackupName)];
      *ptr++ = '.';
      *ptr++ = 'B';
      *ptr++ = 'A';
      *ptr++ = 'K';
      *ptr   = NULL;

      DosDelete(szBackupName, 0L);
      DosMove(szName, szBackupName, 0L);
      }
    }

  if ((*pfh = OpenFile(szName, &OfStruct, OF_READWRITE | OF_CREATE)) == -1)
    return (XlatError(OfStruct.nErrCode));
  else
    return (0);
  }


/*---------------------------------------------------------------------------*/
/*  This subroutine is used by the file transfer code to open a file to send */
/*  to the other guy.  It will generate the correct path and returns the     */
/*  status from the OpenFile function.	Not much here but saves codes else-  */
/*  where.								     */
/*---------------------------------------------------------------------------*/
int OpenXferFile(szName)
  char *szName;
{
  OFSTRUCT OfStruct;


  return (OpenFile(szName, &OfStruct, OF_READ));
}
