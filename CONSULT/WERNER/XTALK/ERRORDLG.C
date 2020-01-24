/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Compiler and Interpreter Error Dialogs                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include "xtalk.h"
#include "library.h"
#include "variable.h"
#include "interp.h"
#include "prodata.h"
#include "term.h"
#include "dialogs.h"


  BOOL DlgError(HWND, unsigned, WORD, LONG);
  void CompilerError();


void GeneralError(nError)
  int  nError;
{
  FARPROC lpDlg;


  nErrorCode = nError;

  if (wSystem & (SS_ABORT || SS_SCRIPT || SS_DDE))
    return;

  if (nError >= ERRCOM_FIRST && nError < ERRINT_FIRST)
    {
    CompilerError();
    }
  else
    {
    HWND hWndFocus;

    wLine = 0;
    lpDlg = MakeProcInstance((FARPROC)DlgError, hWndInst);
    hWndFocus = GetFocus();
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_GENERROR), hWnd, lpDlg);
    if (hWndFocus)
      SetFocus(hWndFocus);
    FreeProcInstance(lpDlg);
    }
}


void CompilerError()
{
  FARPROC lpDlg;
  register i;


  lpDlg = MakeProcInstance((FARPROC)DlgError, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_COMERROR), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  if (i)
    EditScript(szScriptName);
}

void InterpreterError(nError)
  int nError;
{
  HWND hWndFocus;
  FARPROC lpDlg;
  register i;


  InterpreterStop();

  nErrorCode = nError;

  lpDlg = MakeProcInstance((FARPROC)DlgError, hWndInst);
  hWndFocus = GetFocus();
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_INTERROR), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  if (i)
    EditScript(szScriptName);
  else if (hWndFocus)
    SetFocus(hWndFocus);
}


BOOL DlgError(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  HFILE hfSrcFile;
  char fmt[128], buf[128];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      GetDlgItemText(hDlg, ID_ERROR_ERROR, fmt, sizeof(fmt));
      sprintf(buf, fmt, nErrorCode/256, nErrorCode & 0x00FF, szScriptName);
      SetDlgItemText(hDlg, ID_ERROR_ERROR, buf);

      if (wLine)
	{
	GetDlgItemText(hDlg, ID_ERROR_LINENO, fmt, sizeof(fmt));
	sprintf(buf, fmt, wLine);
	SetDlgItemText(hDlg, ID_ERROR_LINENO, buf);
	}

      switch (nErrorCode >> 8)
	{
	case ERRCOM_FIRST >> 8:
	  i = IDS_ERRCOM;
	  break;

	case ERRINT_FIRST >> 8:
	  i = IDS_ERRINT;
	  break;

	case ERRIOE_FIRST >> 8:
	  i = IDS_ERRIOE;
	  break;

	case ERRATH_FIRST >> 8:
	  i = IDS_ERRATH;
	  break;

	case ERRSYS_FIRST >> 8:
	  i = IDS_ERRSYS;
	  break;

	case ERRDOS_FIRST >> 8:
	  i = IDS_ERRDOS;
	  break;

	case ERRDIA_FIRST >> 8:
	  i = IDS_ERRDIA;
	  break;

	case ERRMIS_FIRST >> 8:
	  i = IDS_ERRMIS;
	  break;

	case ERRDDE_FIRST >> 8:
	  i = IDS_ERRDDE;
	  break;

	case ERRDEV_FIRST >> 8:
	  i = IDS_ERRDEV;
	  break;

        case ERRTRM_FIRST >> 8:
          i = IDS_ERRTRM;
	  break;

	case ERRXFR_FIRST >> 8:
	  i = IDS_ERRXFR;
	  break;

	case ERRKRM_FIRST >> 8:
	  i = IDS_ERRKRM;
	  break;

	default:
	  i = 0;
	}

      if (i)
	SetDlgItemStr(hDlg, ID_ERROR_TEXT, i + (nErrorCode & 0x00FF));

      if (wLine)
	{
	BYTE	 abBuf[MAX_BUFFER];
	USHORT	  iBuf;
	USHORT	 usAction, usBytes;
	register fFound;


	fFound = FALSE;
        CreatePath(buf, VAR_DIRXWS, szScriptName);
	if (DosOpen(buf, &hfSrcFile, &usAction, 0L, FILE_NORMAL,
		     FILE_OPEN, OPEN_ACCESS_READWRITE, 0L) == 0)
	  {
	  iBuf = usBytes = 0;

	  while (1)
	    {
	    if (LibGetLine(buf, 128, hfSrcFile, &iBuf, &usBytes, abBuf,
			   MAX_BUFFER))
	      break;

	    if (wLine-- == 1)
	      {
	      fFound = TRUE;
	      break;
	      }
	    }
	  }

	if (!fFound)
	  {
	  EnableWindow(GetDlgItem(hDlg, ID_EDIT), FALSE);
	  buf[ 0] = '<';
	  buf[ 1] = 'N';
	  buf[ 2] = 'o';
	  buf[ 3] = ' ';
	  buf[ 4] = 'S';
	  buf[ 5] = 'o';
	  buf[ 6] = 'u';
	  buf[ 7] = 'r';
	  buf[ 8] = 'c';
	  buf[ 9] = 'e';
	  buf[10] = ' ';
	  buf[11] = 'a';
	  buf[12] = 'v';
	  buf[13] = 'a';
	  buf[14] = 'i';
	  buf[15] = 'l';
	  buf[16] = 'a';
	  buf[17] = 'b';
	  buf[18] = 'l';
	  buf[19] = 'e';
	  buf[20] = '>';
	  buf[21] = NULL;
	  }

	SetDlgItemText(hDlg, ID_ERROR_LINE, buf);
	}
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_EDIT:
	  EndDialog(hDlg, TRUE);
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_CTLCOLOR:
      if ((LOWORD(lParam) == GetDlgItem(hDlg, ID_ERROR_TEXT)) && !fMonoDisplay)
	{
	SetTextColor(wParam, RGB(255, 0, 0));
	return (GetStockObject(WHITE_BRUSH));
	}
      if ((LOWORD(lParam) == GetDlgItem(hDlg, ID_ERROR_LINE)) && !fMonoDisplay)
	{
	SetTextColor(wParam, RGB(0, 0, 255));
	return (GetStockObject(WHITE_BRUSH));
	}
      return FALSE;
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


USHORT ErrorToMsg(USHORT usError)
  {
  register i;

  switch (usError >> 8)
    {
    case ERRCOM_FIRST >> 8:
      i = IDS_ERRCOM;
      break;

    case ERRINT_FIRST >> 8:
      i = IDS_ERRINT;
      break;

    case ERRIOE_FIRST >> 8:
      i = IDS_ERRIOE;
      break;

    case ERRATH_FIRST >> 8:
      i = IDS_ERRATH;
      break;

    case ERRSYS_FIRST >> 8:
      i = IDS_ERRSYS;
      break;

    case ERRDOS_FIRST >> 8:
      i = IDS_ERRDOS;
      break;

    case ERRDIA_FIRST >> 8:
      i = IDS_ERRDIA;
      break;

    case ERRMIS_FIRST >> 8:
      i = IDS_ERRMIS;
      break;

    case ERRDDE_FIRST >> 8:
      i = IDS_ERRDDE;
      break;

    case ERRDEV_FIRST >> 8:
      i = IDS_ERRDEV;
      break;

    case ERRTRM_FIRST >> 8:
      i = IDS_ERRTRM;
      break;

    case ERRXFR_FIRST >> 8:
      i = IDS_ERRXFR;
      break;

    case ERRKRM_FIRST >> 8:
      i = IDS_ERRKRM;
      break;

    default:
      return (0);
    }

  return (i + (usError & 0x00FF));
  }
