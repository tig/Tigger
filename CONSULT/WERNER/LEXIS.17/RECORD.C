/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Record Routines (RECORD.C)                                       */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include <direct.h>
#include <errno.h>
#include <string.h>
#include "lexis.h"
#include "record.h"
#include "data.h"
#include "dialogs.h"
#include "library.h"


  int  hRecordFile = 0;
  BOOL bRecording = FALSE;

static BOOL near RecordOpenNew();
static BOOL near RecordClose();


void CmdRecord()
  {
  if (hRecordFile == 0)
    {
    MessageBeep(NULL);
    return;
    }

  if (bRecording)
    {
    bRecording = FALSE;
    CheckMenuItem(hWndMenu, CMD_RECORDING, MF_UNCHECKED);
    }
  else
    {
    bRecording = TRUE;
    CheckMenuItem(hWndMenu, CMD_RECORDING, MF_CHECKED);
    }
  }


void RecordOff()
  {
  if (bRecording)
    CmdRecord();
  }


void RecordOn()
  {
  if (!bRecording)
    CmdRecord();
  }


void RecordTerm()
  {
  RecordClose();
  }


WORD RecordWriteData(BYTE *byBuf, unsigned cBuf)
  {
  if (bRecording)
    {
    unsigned iBuf;

    if (_dos_write(hRecordFile, byBuf, cBuf, &iBuf))
      {
      MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
      CmdRecord();
      }

    if (cBuf != iBuf)
      {
      MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
      CmdRecord();
      }

    return (iBuf);
    }
  else
    return (0);
  }


WORD RecordWriteNewline()
  {
  if (bRecording)
    {
    char aTmp[2];
    unsigned iTmp;

    aTmp[0] = '\r';
    aTmp[1] = '\n';
    _dos_write(hRecordFile, aTmp, 2, &iTmp);

    return (iTmp);
    }
  else
    return (0);
  }


  BOOL DlgRecord(HWND, unsigned, WORD, LONG);

void RecordNewFile()
  {
  FARPROC lpDlg;


  SetFilePath(szDataDirRecord);

  lpDlg = MakeProcInstance((FARPROC)DlgRecord, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEW_RECORD), hWnd, lpDlg))
    {
    if (RecordOpenNew())
      MessageDisplay(IDS_MSG_BADOPEN, MB_ICONHAND);
    }
  FreeProcInstance(lpDlg);
  }


void RecordOpenFile()
  {
  OFSTRUCT OfStruct;
  int extfile;
  char szFileNameIn[128];


  SetFilePath(szDataDirRecord);

  strcpy(szFileNameIn, "");

  OfStruct.cBytes = 0;
  if (DlgOpen(hWndInst, hWnd, IDD_OPEN_RECORD, &OfStruct, &extfile,
          szFileNameIn, "\\*.REC", szAppName) == NOOPEN)
    {
    extfile = -1;
    }
  else
    {
    if (hRecordFile)
      RecordClose();

    lseek(extfile, 0L, SEEK_END);

    hRecordFile = extfile;
    EnableMenuItem(hWndMenu, CMD_RECORDING, MF_ENABLED);
    CheckMenuItem(hWndMenu, CMD_RECORDING, MF_CHECKED);
    bRecording = TRUE;
    }
  }


BOOL DlgRecord(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
  {
  OFSTRUCT ofStruct;


  switch (message)
    {
    case WM_INITDIALOG:
      DlgDirList(hDlg, NULL, NULL, ID_PATH, NULL);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case IDOK:
          GetDlgItemText(hDlg, ID_EDIT, szWorkRecord, sizeof(szWorkRecord));
          if (!ValidateFilename(szWorkRecord))
            {
            MessageDisplay(IDS_MSG_BADNAME, MB_ICONHAND);
            break;
            }
          if (strchr(szWorkRecord, '.') == NULL)
            {
            register i;

            i = strlen(szWorkRecord);
            szWorkRecord[i++] = '.';
            szWorkRecord[i++] = 'R';
            szWorkRecord[i++] = 'E';
            szWorkRecord[i++] = 'C';
            szWorkRecord[i]   = NULL;
            }

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


static BOOL near RecordOpenNew()
  {
  register i;


  if (hRecordFile)
    RecordClose();

  if (i = _dos_creatnew(szWorkRecord, _A_NORMAL, &hRecordFile))
    {
    if (i == EEXIST)
      {
      if (MessageDisplay(IDS_MSG_FILEXIST, MB_OKCANCEL | MB_ICONQUESTION) == ID_OK)
        {
        if (_dos_creat(szWorkRecord, _A_NORMAL, &hRecordFile))
          return (TRUE);
        }
      else
        return (TRUE);
      }
    else
      return (TRUE);
    }

  EnableMenuItem(hWndMenu, CMD_RECORDING, MF_ENABLED);
  CheckMenuItem(hWndMenu, CMD_RECORDING, MF_CHECKED);
  bRecording = TRUE;

  return (FALSE);
  }

static BOOL near RecordClose()
  {
  if (hRecordFile)
    {
    _dos_close(hRecordFile);
    hRecordFile = NULL;
    CheckMenuItem(hWndMenu, CMD_RECORDING, MF_UNCHECKED);
    EnableMenuItem(hWndMenu, CMD_RECORDING, MF_GRAYED);
    bRecording = FALSE;
    return (FALSE);
    }
  return (TRUE);
  }
