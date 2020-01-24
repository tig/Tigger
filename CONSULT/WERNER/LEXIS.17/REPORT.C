/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Report Support Module (REPORT.C)                                 */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <dos.h>
#include <memory.h>
#include <string.h>
#include "lexis.h"
#include "library.h"
#include "report.h"
#include "comm.h"
#include "data.h"
#include "record.h"
#include "session.h"
#include "term.h"
#include "dialogs.h"

/*
** Define Global Variables.
*/
  BOOL fToggleRecordOn;


  FARPROC lpReportDlg;

  static long   lTimeout;
  static long   lSleepUntil;


  BOOL DlgReport(HWND, unsigned, WORD, LONG);


void ReportStart()
  {
  fToggleRecordOn = bRecording;
  if (bRecording)
    RecordOff();

  lpReportDlg = MakeProcInstance((FARPROC)DlgReport, hWndInst);
  hProcessDlg =  CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_REPORT),
                              hWnd, lpReportDlg);
  }

void ReportEnd()
{
  if (hProcessDlg == 0)
    return;

  if (bWorkStoreDoc)
    {
    _dos_close(hWorkDocument);
    bWorkStoreDoc = FALSE;
    }

  if (bWorkPrintDoc)
    {
    PrinterOff();
    bWorkPrintDoc = FALSE;
    }

  DestroyWindow(hProcessDlg);
  hProcessDlg = 0;
  FreeProcInstance(lpReportDlg);

  fTermPrinting = FALSE;

  if (fToggleRecordOn)
    {
    RecordOn();
    fToggleRecordOn = FALSE;
    }
  }


void ReportError()
  {
  if (!fTermCancel)
    {
    fTermCancel = TRUE;
    MessageDisplay(IDS_MSG_DISKFULL, MB_ICONHAND);
    }
  }


BOOL DlgReport(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static BOOL fFirstLight;
  static long lChangeLight;


  switch (message)
    {
    case WM_INITDIALOG:
      {
      char szBuf[64], szFmt[96];
      register i, j;

      strupr(szWorkDocument);
      GetDlgItemText(hDlg, ID_REPORT_FILE, szFmt, sizeof(szFmt));
      sprintf(szBuf, szFmt, szWorkDocument);

      for (i = j = 0; szBuf[i]; i++, j++)
	{
	szFmt[j] = szBuf[i];
	if (szFmt[j] == '&')
          szFmt[++j] = '&';
	}
      szFmt[j] = '\0';

      SetDlgItemText(hDlg, ID_REPORT_FILE, szFmt);

      CheckDlgButton(hDlg, ID_REPORT_FILE, bWorkStoreDoc);
      CheckDlgButton(hDlg, ID_REPORT_PRINT, bWorkPrintDoc);

      fFirstLight  = TRUE;
      lChangeLight = GetCurrentTime() + 1500L;
      CheckDlgButton(hDlg, ID_REPORT_LIGHT1, TRUE);
      fTermPrinting = TRUE;
      fTermCancel   = FALSE;
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	  break;

	case ID_CANCEL:
          fTermCancel = TRUE;
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_TIMER:
      {
      BYTE abBuffer[1024];
      register i;

      if (!fCommCarrier)
        {
        ReportEnd();
        SessTellDisconnect(FALSE);
        break;
        }

      if (GetCurrentTime() > lChangeLight)
        {
        fFirstLight = !fFirstLight;
        CheckDlgButton(hDlg, ID_REPORT_LIGHT1, fFirstLight);
        CheckDlgButton(hDlg, ID_REPORT_LIGHT2, !fFirstLight);
        lChangeLight = GetCurrentTime() + 1500L;
        }

      if (i = CommReadData(abBuffer, sizeof(abBuffer)))
        TermWriteData(abBuffer, i, FALSE);
      }
      break;

    default:
      return (FALSE);
    }

  return (TRUE);
}
