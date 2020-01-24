/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Diary Support Module (DIARY.C)                                   */
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
#include <time.h>
#include "lexis.h"
#include "diary.h"
#include "data.h"
#include "dialogs.h"
#include "library.h"


  int  hDiaryFile = NULL;
  BOOL bDiary     = FALSE;

extern BOOL DlgDiary(HWND, unsigned, WORD, LONG);

static BOOL near DiaryOpenNew();
static BOOL near DiaryClose();


void CmdDiary()
  {
  if (bDiary)
    DiaryOff();
  else
    DiaryOn();
  }


void DiaryOff()
  {
  char buf[256];
  long lTime;


  time(&lTime);
  sprintf(buf, "[Entry at %.24s] - Diary Stopped\r\n", ctime(&lTime));
  DiaryWriteData(buf, strlen(buf));

  if (bDiary)
    {
    bDiary = FALSE;
    CheckMenuItem(hWndMenu, CMD_DIARY, MF_UNCHECKED);
    EnableMenuItem(hWndMenu, CMD_COPYDIARY, MF_GRAYED);
    }
  }


void DiaryOn()
  {
  char buf[256];
  long lTime;


  if (!bDiary)
    {
    bDiary = TRUE;
    CheckMenuItem(hWndMenu, CMD_DIARY, MF_CHECKED);
    EnableMenuItem(hWndMenu, CMD_COPYDIARY, MF_ENABLED);
    }

  time(&lTime);
  sprintf(buf, "[Entry at %.24s] - Diary Started\r\n", ctime(&lTime));
  DiaryWriteData(buf, strlen(buf));
  }


void DiaryUpdate()
  {
  char buf[256];
  long lTime;


  time(&lTime);
  sprintf(buf, "[Entry at %.24s] - %s; %s\r\n", ctime(&lTime), szWorkClient, szWorkLibrary);
  DiaryWriteData(buf, strlen(buf));
  }


void DiaryDownload(char *szDocFile)
  {
  char buf[256];
  long lTime;


  time(&lTime);
  sprintf(buf, "[Entry at %.24s] - Document download to %s\r\n", ctime(&lTime), szDocFile);
  DiaryWriteData(buf, strlen(buf));
  }


void DiaryTerm()
  {
  DiaryClose();
  }


WORD DiaryWriteData(BYTE *byBuf, unsigned cBuf)
  {
  if (bDiary && hDiaryFile)
    {
    unsigned iBuf;

    if (_dos_write(hDiaryFile, byBuf, cBuf, &iBuf))
      {
      MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
      DiaryOff();
      }

    if (cBuf != iBuf)
      {
      MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
      DiaryOff();
      }

    return (iBuf);
    }
  else
    return (0);
  }


WORD DiaryWriteNewline()
  {
  if (bDiary && hDiaryFile)
    {
    char aTmp[2];
    unsigned iTmp;

    aTmp[0] = '\r';
    aTmp[1] = '\n';

    if (_dos_write(hDiaryFile, aTmp, 2, &iTmp))
      {
      MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
      DiaryOff();
      }

    if (2 != iTmp)
      {
      MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
      DiaryOff();
      }

    return (iTmp);
    }
  else
    return (0);
  }


void DiaryNewFile()
  {
  FARPROC lpDlg;

  SetFilePath(szDataDirMaps);
  lpDlg = MakeProcInstance((FARPROC)DlgDiary, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEW_DIARY), hWnd, lpDlg))
    {
    if (DiaryOpenNew())
      MessageDisplay(IDS_MSG_BADOPEN, MB_ICONHAND);
    }
  FreeProcInstance(lpDlg);
  }


void DiaryOpenFile()
  {
  OFSTRUCT OfStruct;
  int extfile;
  char szFileNameIn[128];


  SetFilePath(szDataDirMaps);
  strcpy(szFileNameIn, "");

  OfStruct.cBytes = 0;
  if (DlgOpen(hWndInst, hWnd, IDD_OPEN_DIARY, &OfStruct, &extfile,
          szFileNameIn, "\\*.HST", szAppName) == NOOPEN)
    {
    extfile = -1;
    }
  else
    {
    if (hDiaryFile)
      DiaryClose();

    lseek(extfile, 0L, SEEK_END);

    hDiaryFile = extfile;
    EnableMenuItem(hWndMenu, CMD_DIARY, MF_ENABLED);
    DiaryOn();
    }
  }


BOOL DlgDiary(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
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
          GetDlgItemText(hDlg, ID_EDIT, szWorkDiary, sizeof(szWorkDiary));
          if (OpenFile(szWorkDiary, &ofStruct, OF_PARSE) ||
              DlgSearchSpec(szWorkDiary))
            {
            MessageDisplay(IDS_MSG_BADNAME, MB_ICONHAND);
            break;
            }

          if (strchr(szWorkDiary, '.') == NULL)
            {
            register i;

            i = strlen(szWorkDiary);
            szWorkDiary[i++] = '.';
            szWorkDiary[i++] = 'H';
            szWorkDiary[i++] = 'S';
            szWorkDiary[i++] = 'T';
            szWorkDiary[i]   = NULL;
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


static BOOL near DiaryOpenNew()
  {
  register i;


  if (hDiaryFile)
    DiaryClose();

  if (i = _dos_creatnew(szWorkDiary, _A_NORMAL, &hDiaryFile))
    {
    if (i == EEXIST)
      {
      if (MessageDisplay(IDS_MSG_FILEXIST, MB_ICONQUESTION) == ID_OK)
        {
        if (_dos_creat(szWorkDiary, _A_NORMAL, &hDiaryFile))
          return (TRUE);
        }
      else
        return (TRUE);
      }
    else
      return (TRUE);
    }

  EnableMenuItem(hWndMenu, CMD_DIARY, MF_ENABLED);
  DiaryOn();

  return (FALSE);
  }


static BOOL near DiaryClose()
  {
  if (bDiary)
    DiaryOff();

  if (hDiaryFile)
    {
    _dos_close(hDiaryFile);
    hDiaryFile = NULL;
    EnableMenuItem(hWndMenu, CMD_DIARY, MF_GRAYED);
    bDiary = FALSE;
    return (FALSE);
    }
  return (TRUE);
  }
