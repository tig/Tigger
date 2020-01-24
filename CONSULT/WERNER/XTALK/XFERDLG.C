/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   File Transfer Dialogs (XFERDLG.C)                               *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions:       														   *
*  1.00 07/17/89 Initial Version.                                             *
*  1.01 10/31/89 00052    Add context help to the menus and dialogs.          *
*  1.01 02/03/90 dca00017 PJL Incorrect help text for Setup.Protocol fixed.   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "xfer.h"
#include "prodata.h"


  BOOL DlgProtocol(HWND, unsigned, WORD, LONG);
  BOOL DlgFileRequest(HWND, unsigned, WORD, LONG);
  BOOL DlgProtocolAscii(HWND, unsigned, WORD, LONG);
  BOOL DlgProtocolCrosstalk(HWND, unsigned, WORD, LONG);
  BOOL DlgProtocolDart(HWND, unsigned, WORD, LONG);
  BOOL DlgProtocolXModem(HWND, unsigned, WORD, LONG);
  BOOL DlgProtocolCISB(HWND, unsigned, WORD, LONG);
  BOOL DlgProtocolZmodem(HWND, unsigned, WORD, LONG);


void CmdProtocol()
{
  FARPROC lpDlg;
  register int i;


  InfoMessage(MSGINF_HELP);
// 02/03/90 dca00017 PJL Incorrect help text for Setup.Protocol.
// wHelpDialog = HELP_ITEM_DEVICE;
   wHelpDialog = HELP_ITEM_PROTOCOL;

  lpDlg = MakeProcInstance((FARPROC)DlgProtocol, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  switch (i)
    {
    case XFER_XTALK:
      lpDlg = MakeProcInstance((FARPROC)DlgProtocolCrosstalk, hWndInst);
      if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL_CROSSTALK), hWnd, lpDlg))
	Xfer.cProtocol = (char)i;
      FreeProcInstance(lpDlg);
      break;

    case XFER_DART:
      lpDlg = MakeProcInstance((FARPROC)DlgProtocolDart, hWndInst);
      if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL_DART), hWnd, lpDlg))
	Xfer.cProtocol = (char)i;
      FreeProcInstance(lpDlg);
      break;

    case XFER_MODEM7:
    case XFER_XMODEM:
    case XFER_XMODEM1K:
    case XFER_YMODEM:
      lpDlg = MakeProcInstance((FARPROC)DlgProtocolXModem, hWndInst);
      if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL_XMODEM), hWnd, lpDlg))
	Xfer.cProtocol = (char)i;
      FreeProcInstance(lpDlg);
      break;

    case XFER_CSERVEB:
      lpDlg = MakeProcInstance((FARPROC)DlgProtocolCISB, hWndInst);
      if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL_CISB), hWnd, lpDlg))
	Xfer.cProtocol = (char)i;
      FreeProcInstance(lpDlg);
      break;

    case XFER_ZMODEM:
      lpDlg = MakeProcInstance((FARPROC)DlgProtocolZmodem, hWndInst);
      if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL_ZMODEM), hWnd, lpDlg))
	Xfer.cProtocol = (char)i;
      FreeProcInstance(lpDlg);
      break;
    }

  UpdateProtocol();

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


int XferFileRequest()
{
  FARPROC lpDlg;
  register int i;


  lpDlg = MakeProcInstance((FARPROC)DlgFileRequest, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_FILE_REQUEST), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  return (i);
}


BOOL DlgProtocol(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  register int i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cProtocol == XFER_XTALK)
	i = ID_PROTOCOL_XTALK;
      else if (Xfer.cProtocol == XFER_DART)
	i = ID_PROTOCOL_DART;
      else if (Xfer.cProtocol == XFER_MODEM7)
	i = ID_PROTOCOL_MODEM7;
      else if (Xfer.cProtocol == XFER_XMODEM)
	i = ID_PROTOCOL_XMODEM;
      else if (Xfer.cProtocol == XFER_XMODEM1K)
	i = ID_PROTOCOL_MODEM1K;
      else if (Xfer.cProtocol == XFER_YMODEM)
	i = ID_PROTOCOL_YMODEM;
      else if (Xfer.cProtocol == XFER_ZMODEM)
        i = ID_PROTOCOL_ZMODEM;
      else if (Xfer.cProtocol == XFER_CSERVEB)
        i = ID_PROTOCOL_CSERVEB;
      else
        {
        i = ID_PROTOCOL_KERMIT;
        EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
        }
      CheckDlgButton(hDlg, i, TRUE);

      return (ID_OK);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_XTALK))
	    Xfer.cProtocol = XFER_XTALK;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_DART))
	    Xfer.cProtocol = XFER_DART;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_MODEM7))
	    Xfer.cProtocol = XFER_MODEM7;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_MODEM1K))
	    Xfer.cProtocol = XFER_XMODEM1K;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_YMODEM))
	    Xfer.cProtocol = XFER_YMODEM;
     //   else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_MODEM1G))
     //     Xfer.cProtocol = XFER_MODEM1G;
     //   else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_YMODEMG))
     //     Xfer.cProtocol = XFER_YMODEMG;
          else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_KERMIT))
            Xfer.cProtocol = XFER_KERMIT;
          else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_ZMODEM))
            Xfer.cProtocol = XFER_ZMODEM;
          else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_CSERVEB))
            Xfer.cProtocol = XFER_CSERVEB;
	  else
	    Xfer.cProtocol = XFER_XMODEM;

          fDataChanged = TRUE;
	  EndDialog(hDlg, -1);
	  break;

	case ID_MORE:
	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_XTALK))
	    i = XFER_XTALK;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_DART))
	    i = XFER_DART;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_MODEM7))
	    i = XFER_MODEM7;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_MODEM1K))
	    i = XFER_XMODEM1K;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_YMODEM))
	    i = XFER_YMODEM;
     //   else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_MODEM1G))
     //     i = XFER_MODEM1G;
          else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_ZMODEM))
            i = XFER_ZMODEM;
          else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_CSERVEB))
            i = XFER_CSERVEB;
	  else
	    i = XFER_XMODEM;

	  EndDialog(hDlg, i);
	  break;

	case ID_CANCEL:
	  EndDialog(hDlg, -1);
	  break;

	case ID_PROTOCOL_XTALK:
	case ID_PROTOCOL_DART:
	case ID_PROTOCOL_MODEM7:
	case ID_PROTOCOL_XMODEM:
	case ID_PROTOCOL_MODEM1K:
	case ID_PROTOCOL_YMODEM:
        case ID_PROTOCOL_ZMODEM:
        case ID_PROTOCOL_KERMIT:
        case ID_PROTOCOL_CSERVEB:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            CheckRadioButton(hDlg, ID_PROTOCOL_XTALK, ID_PROTOCOL_CSERVEB, wParam);
            if (wParam == ID_PROTOCOL_KERMIT)
              EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
            else
              EnableWindow(GetDlgItem(hDlg, ID_MORE), TRUE);
            }
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

BOOL DlgFileRequest(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      EnableWindow(GetDlgItem(hDlg, ID_OK), FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
	  GetDlgItemText(hDlg, ID_EDIT, szXferFilePath, MAX_PATH);
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

        case ID_EDIT:
          if (HIWORD(lParam) == EN_CHANGE)
            {
            EnableWindow(GetDlgItem(hDlg, IDOK),
              (int)SendMessage(GetDlgItem(hDlg, ID_EDIT), WM_GETTEXTLENGTH, 0, 0L));
            }
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


//
//  CmdAscii()
//
//  The Command ASCII routine is called when the user wants to edit the ASCII
//  upload parameters.
//
//
void CmdAscii()
  {
  FARPROC lpDlg;


  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_ASCIISET;

  lpDlg = MakeProcInstance((FARPROC)DlgProtocolAscii, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PROTOCOL_ASCII), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
  }


//
//  DlgProtocolAscii()
//
//  The Dialog Protocol ASCII dialog procedure supports the CmdAscii() function
//  to permit the user to modify the current ASCII upload parameters.
//
//
BOOL DlgProtocolAscii(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
  {
  char szNull[1];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cLWait == XFER_WAIT_NONE)
        {
        ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_HIDE);
	SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "");
	i = ID_ASCII_LWAIT_NONE;
        }
      else if (Xfer.cLWait == XFER_WAIT_ECHO)
        {
        ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_HIDE);
	SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "");
	i = ID_ASCII_LWAIT_ECHO;
        }
      else if (Xfer.cLWait == XFER_WAIT_PROMPT)
        {
        SetDlgItemLength(hDlg, ID_ASCII_LWAIT_TEXT, sizeof(Xfer.szLWaitPrompt)-1);
        SetDlgItemData(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.szLWaitPrompt);
	SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "");
	i = ID_ASCII_LWAIT_PROMPT;
        }
      else if (Xfer.cLWait == XFER_WAIT_COUNT)
        {
        SetDlgItemLength(hDlg, ID_ASCII_LWAIT_TEXT, 3);
	SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "chars");
        SetDlgItemInt(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.cLWaitCount, FALSE);
	i = ID_ASCII_LWAIT_COUNT;
        }
      else
        {
        SetDlgItemLength(hDlg, ID_ASCII_LWAIT_TEXT, 2);
        SetDlgItemInt(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.nLWaitDelay/1000, FALSE);
	SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "secs");
	i = ID_ASCII_LWAIT_DELAY;
        }
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.cCWait == XFER_WAIT_NONE)
	i = ID_ASCII_CWAIT_NONE;
      else if (Xfer.cCWait == XFER_WAIT_ECHO)
	i = ID_ASCII_CWAIT_ECHO;
      else
	i = ID_ASCII_CWAIT_DELAY;
      CheckDlgButton(hDlg, i, TRUE);

      SetDlgItemInt(hDlg, ID_ASCII_CWAIT_TEXT, Xfer.nCWaitDelay, FALSE);

      if (i != ID_ASCII_CWAIT_DELAY)
	EnableWindow(GetDlgItem(hDlg, ID_ASCII_CWAIT_TEXT), FALSE);

      SetDlgItemData(hDlg, ID_ASCII_LINEDELIM, Xfer.szLineDelim);
      SetDlgItemData(hDlg, ID_ASCII_BLANKEX, Xfer.szBlankEx);
      if (Xfer.fTabEx)
	i = ID_ASCII_TABEX_YES;
      else
	i = ID_ASCII_TABEX_NO;
      CheckDlgButton(hDlg, i, TRUE);

      SetDlgItemInt(hDlg, ID_ASCII_LINETIME, Xfer.cLineTime, FALSE);
      SetDlgItemInt(hDlg, ID_ASCII_WORDWRAP, Xfer.cWordWrap, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i  = GetDlgItemInt(hDlg, ID_ASCII_WORDWRAP, 0, TRUE);
          if (i != 0 && (i < 20 || i > 127))
            {
            ErrorMessage(IDS_ERRDLG_WORDWRAP);
            break;
            }

          i  = GetDlgItemInt(hDlg, ID_ASCII_LINETIME, 0, TRUE);
          if (i < 0 || i > 127)
            {
            ErrorMessage(IDS_ERRDLG_LINETIME);
            break;
            }

	  if (IsDlgButtonChecked(hDlg, ID_ASCII_LWAIT_NONE))
	    Xfer.cLWait = XFER_WAIT_NONE;
	  else if (IsDlgButtonChecked(hDlg, ID_ASCII_LWAIT_ECHO))
	    Xfer.cLWait = XFER_WAIT_ECHO;
	  else if (IsDlgButtonChecked(hDlg, ID_ASCII_LWAIT_PROMPT))
            {
            GetDlgItemData(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.szLWaitPrompt,
                           sizeof(Xfer.szLWaitPrompt));
	    Xfer.cLWait = XFER_WAIT_PROMPT;
            }
	  else if (IsDlgButtonChecked(hDlg, ID_ASCII_LWAIT_COUNT))
            {
            i  = GetDlgItemInt(hDlg, ID_ASCII_LWAIT_TEXT, 0, TRUE);
            if (i < 0 || i > 255)
              {
              ErrorMessage(IDS_ERRDLG_LWAITCOUNT);
              break;
              }
            Xfer.cLWaitCount = (BYTE)i;
	    Xfer.cLWait = XFER_WAIT_COUNT;
            }
	  else
            {
            i = GetDlgItemInt(hDlg, ID_ASCII_LWAIT_TEXT, 0, TRUE);
            if (i < 0 || i > 25)
              {
              ErrorMessage(IDS_ERRDLG_LWAITDELAY);
              break;
              }

            Xfer.nLWaitDelay = i * 1000;
	    Xfer.cLWait = XFER_WAIT_DELAY;
            }

	  if (IsDlgButtonChecked(hDlg, ID_ASCII_CWAIT_NONE))
	    Xfer.cCWait = XFER_WAIT_NONE;
	  else if (IsDlgButtonChecked(hDlg, ID_ASCII_CWAIT_ECHO))
	    Xfer.cCWait = XFER_WAIT_ECHO;
          else
            {
	    Xfer.cCWait = XFER_WAIT_DELAY;
            i  = GetDlgItemInt(hDlg, ID_ASCII_CWAIT_TEXT, 0, TRUE);
            if (i < 0 || i > 9999)
               {
               ErrorMessage(IDS_ERRDLG_CWAITDELAY);
               break;
              }

            Xfer.nCWaitDelay = i;
            }

	  GetDlgItemData(hDlg, ID_ASCII_LINEDELIM, Xfer.szLineDelim,
			 sizeof(Xfer.szLineDelim));
	  GetDlgItemData(hDlg, ID_ASCII_BLANKEX, Xfer.szBlankEx,
			 sizeof(Xfer.szBlankEx));
	  Xfer.fTabEx = (char)IsDlgButtonChecked(hDlg, ID_ASCII_TABEX_YES);

          Xfer.cLineTime = (char)GetDlgItemInt(hDlg, ID_ASCII_LINETIME, 0, TRUE);

          Xfer.cWordWrap = (char)GetDlgItemInt(hDlg, ID_ASCII_WORDWRAP, 0, TRUE);

	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_ASCII_CWAIT_NONE:
	case ID_ASCII_CWAIT_ECHO:
	  CheckRadioButton(hDlg, ID_ASCII_CWAIT_NONE, ID_ASCII_CWAIT_DELAY, wParam);
	  EnableWindow(GetDlgItem(hDlg, ID_ASCII_CWAIT_TEXT), FALSE);
	  break;

	case ID_ASCII_CWAIT_DELAY:
	  CheckRadioButton(hDlg, ID_ASCII_CWAIT_NONE, ID_ASCII_CWAIT_DELAY, wParam);
	  EnableWindow(GetDlgItem(hDlg, ID_ASCII_CWAIT_TEXT), TRUE);
	  break;

	case ID_ASCII_TABEX_YES:
	case ID_ASCII_TABEX_NO:
	  CheckRadioButton(hDlg, ID_ASCII_TABEX_YES, ID_ASCII_TABEX_NO, wParam);
	  break;

        case ID_ASCII_LWAIT_NONE:
	  CheckRadioButton(hDlg, ID_ASCII_LWAIT_NONE, ID_ASCII_LWAIT_DELAY, wParam);
	  ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_HIDE);
	  SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "");
          break;

        case ID_ASCII_LWAIT_ECHO:
	  CheckRadioButton(hDlg, ID_ASCII_LWAIT_NONE, ID_ASCII_LWAIT_DELAY, wParam);
	  ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_HIDE);
	  SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "");
          break;

        case ID_ASCII_LWAIT_PROMPT:
	  CheckRadioButton(hDlg, ID_ASCII_LWAIT_NONE, ID_ASCII_LWAIT_DELAY, wParam);
	  SetDlgItemLength(hDlg, ID_ASCII_LWAIT_TEXT, sizeof(Xfer.szLWaitPrompt)-1);
	  SetDlgItemData(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.szLWaitPrompt);
	  ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_SHOW);
	  SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "");
          break;

        case ID_ASCII_LWAIT_COUNT:
	  CheckRadioButton(hDlg, ID_ASCII_LWAIT_NONE, ID_ASCII_LWAIT_DELAY, wParam);
	  SetDlgItemLength(hDlg, ID_ASCII_LWAIT_TEXT, 3);
	  SetDlgItemInt(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.cLWaitCount, FALSE);
	  ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_SHOW);
	  SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "chars");
          break;

        case ID_ASCII_LWAIT_DELAY:
	  CheckRadioButton(hDlg, ID_ASCII_LWAIT_NONE, ID_ASCII_LWAIT_DELAY, wParam);
	  SetDlgItemLength(hDlg, ID_ASCII_LWAIT_TEXT, 2);
	  SetDlgItemInt(hDlg, ID_ASCII_LWAIT_TEXT, Xfer.nLWaitDelay/1000, FALSE);
	  ShowWindow(GetDlgItem(hDlg, ID_ASCII_LWAIT_TEXT), SW_SHOW);
	  SetDlgItemText(hDlg, ID_ASCII_LTYPE_TEXT, "secs");
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


BOOL DlgProtocolCrosstalk(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
  {
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cTiming == XFER_TIMING_TIGHT)
	i = ID_PROTOCOL_TIGHT;
      else if (Xfer.cTiming == XFER_TIMING_NORMAL)
	i = ID_PROTOCOL_NORMAL;
      else if (Xfer.cTiming == XFER_TIMING_LOOSE)
	i = ID_PROTOCOL_LOOSE;
      else
	i = ID_PROTOCOL_SLOPPY;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.nPacketSize <= 256)
	i = ID_PROTOCOL_256;
      else if (Xfer.nPacketSize <= 512)
	i = ID_PROTOCOL_512;
      else if (Xfer.nPacketSize <= 768)
        i = ID_PROTOCOL_768;
      else
	i = ID_PROTOCOL_1024;
      CheckDlgButton(hDlg, i, TRUE);

      SetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, Xfer.cMaxErrors, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i = GetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, 0, TRUE);
	  if (i < 0 || i > 20)
            {
            ErrorMessage(IDS_ERRDLG_MAXERRORS);
            break;
            }
          Xfer.cMaxErrors = (char)i;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_256))
	    Xfer.nPacketSize = 256;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_512))
	    Xfer.nPacketSize = 512;
          else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_768))
            Xfer.nPacketSize = 768;
	  else
            Xfer.nPacketSize = 1024;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_TIGHT))
	    Xfer.cTiming = XFER_TIMING_TIGHT;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_NORMAL))
	    Xfer.cTiming = XFER_TIMING_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_LOOSE))
	    Xfer.cTiming = XFER_TIMING_LOOSE;
	  else
	    Xfer.cTiming = XFER_TIMING_SLOPPY;

	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case ID_PROTOCOL_256:
	case ID_PROTOCOL_512:
	case ID_PROTOCOL_768:
	case ID_PROTOCOL_1024:
	  CheckDlgButton(hDlg, ID_PROTOCOL_256, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_512, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_768, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_1024, FALSE);
	  CheckDlgButton(hDlg, wParam, TRUE);
	  break;

	case ID_PROTOCOL_TIGHT:
	case ID_PROTOCOL_NORMAL:
	case ID_PROTOCOL_LOOSE:
	case ID_PROTOCOL_SLOPPY:
	  CheckRadioButton(hDlg, ID_PROTOCOL_TIGHT, ID_PROTOCOL_SLOPPY, wParam);
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


BOOL DlgProtocolDart(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cRecvMode == XFER_RECV_NORMAL)
	i = ID_PROTOCOL_RECVNORM;
      else if (Xfer.cRecvMode == XFER_RECV_ALL)
	i = ID_PROTOCOL_RECVALL;
      else
	i = ID_PROTOCOL_RECVNEWER;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.cTiming == XFER_TIMING_TIGHT)
	i = ID_PROTOCOL_TIGHT;
      else if (Xfer.cTiming == XFER_TIMING_NORMAL)
	i = ID_PROTOCOL_NORMAL;
      else if (Xfer.cTiming == XFER_TIMING_LOOSE)
	i = ID_PROTOCOL_LOOSE;
      else
	i = ID_PROTOCOL_SLOPPY;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.fDateMode)
	i = ID_PROTOCOL_CREATE;
      else
	i = ID_PROTOCOL_ORIGINAL;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.fRecovery)
	i = ID_PROTOCOL_RECON;
      else
	i = ID_PROTOCOL_RECOFF;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.nPacketSize == 128)
	i = ID_PROTOCOL_128;
      else if (Xfer.nPacketSize == 256)
	i = ID_PROTOCOL_256;
      else if (Xfer.nPacketSize == 512)
	i = ID_PROTOCOL_512;
      else
	i = ID_PROTOCOL_1024;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.fHostMode)
	i = ID_PROTOCOL_HOSTOFF;
      else
	i = ID_PROTOCOL_HOSTON;
      CheckDlgButton(hDlg, i, TRUE);

      SetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, Xfer.cMaxErrors, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i = GetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, 0, TRUE);
	  if (i < 0 || i > 20)
            {
            ErrorMessage(IDS_ERRDLG_MAXERRORS);
            break;
            }
          Xfer.cMaxErrors = (char)i;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_RECVNORM))
	    Xfer.cRecvMode = XFER_RECV_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_RECVALL))
	    Xfer.cRecvMode = XFER_RECV_ALL;
	  else
	    Xfer.cRecvMode = XFER_RECV_NEWER;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_TIGHT))
	    Xfer.cTiming = XFER_TIMING_TIGHT;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_NORMAL))
	    Xfer.cTiming = XFER_TIMING_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_LOOSE))
	    Xfer.cTiming = XFER_TIMING_LOOSE;
	  else
	    Xfer.cTiming = XFER_TIMING_SLOPPY;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_128))
	    Xfer.nPacketSize = 128;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_256))
	    Xfer.nPacketSize = 256;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_512))
	    Xfer.nPacketSize = 512;
	  else
	    Xfer.nPacketSize = 1024;

	  Xfer.fDateMode = (char)IsDlgButtonChecked(hDlg, ID_PROTOCOL_CREATE);
	  Xfer.fRecovery = (char)IsDlgButtonChecked(hDlg, ID_PROTOCOL_RECON);
          Xfer.fHostMode = (char)IsDlgButtonChecked(hDlg, ID_PROTOCOL_HOSTOFF);

	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_PROTOCOL_CREATE:
	case ID_PROTOCOL_ORIGINAL:
	  CheckRadioButton(hDlg, ID_PROTOCOL_CREATE, ID_PROTOCOL_ORIGINAL, wParam);
	  break;

	case ID_PROTOCOL_RECON:
	case ID_PROTOCOL_RECOFF:
	  CheckRadioButton(hDlg, ID_PROTOCOL_RECON, ID_PROTOCOL_RECOFF, wParam);
	  break;

	case ID_PROTOCOL_HOSTON:
	case ID_PROTOCOL_HOSTOFF:
	  CheckRadioButton(hDlg, ID_PROTOCOL_HOSTON, ID_PROTOCOL_HOSTOFF, wParam);
	  break;

	case ID_PROTOCOL_128:
	case ID_PROTOCOL_256:
	case ID_PROTOCOL_512:
	case ID_PROTOCOL_1024:
	  CheckDlgButton(hDlg, ID_PROTOCOL_128, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_256, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_512, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_1024, FALSE);
	  CheckDlgButton(hDlg, wParam, TRUE);
	  break;

	case ID_PROTOCOL_TIGHT:
	case ID_PROTOCOL_NORMAL:
	case ID_PROTOCOL_LOOSE:
	case ID_PROTOCOL_SLOPPY:
	  CheckRadioButton(hDlg, ID_PROTOCOL_TIGHT, ID_PROTOCOL_SLOPPY, wParam);
	  break;

	case ID_PROTOCOL_RECVNORM:
	case ID_PROTOCOL_RECVALL:
	case ID_PROTOCOL_RECVNEWER:
	  CheckRadioButton(hDlg, ID_PROTOCOL_RECVNORM, ID_PROTOCOL_RECVNEWER, wParam);
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

BOOL DlgProtocolXModem(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cTiming == XFER_TIMING_TIGHT)
	i = ID_PROTOCOL_TIGHT;
      else if (Xfer.cTiming == XFER_TIMING_NORMAL)
	i = ID_PROTOCOL_NORMAL;
      else if (Xfer.cTiming == XFER_TIMING_LOOSE)
	i = ID_PROTOCOL_LOOSE;
      else
	i = ID_PROTOCOL_SLOPPY;
      CheckDlgButton(hDlg, i, TRUE);

      SetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, Xfer.cMaxErrors, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i = GetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, 0, TRUE);
	  if (i < 0 || i > 20)
            {
            ErrorMessage(IDS_ERRDLG_MAXERRORS);
            break;
            }
          Xfer.cMaxErrors = (char)i;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_TIGHT))
	    Xfer.cTiming = XFER_TIMING_TIGHT;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_NORMAL))
	    Xfer.cTiming = XFER_TIMING_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_LOOSE))
	    Xfer.cTiming = XFER_TIMING_LOOSE;
	  else
	    Xfer.cTiming = XFER_TIMING_SLOPPY;

	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_PROTOCOL_TIGHT:
	case ID_PROTOCOL_NORMAL:
	case ID_PROTOCOL_LOOSE:
	case ID_PROTOCOL_SLOPPY:
	  CheckRadioButton(hDlg, ID_PROTOCOL_TIGHT, ID_PROTOCOL_SLOPPY, wParam);
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

BOOL DlgProtocolCISB(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cTiming == XFER_TIMING_TIGHT)
	i = ID_PROTOCOL_TIGHT;
      else if (Xfer.cTiming == XFER_TIMING_NORMAL)
	i = ID_PROTOCOL_NORMAL;
      else if (Xfer.cTiming == XFER_TIMING_LOOSE)
	i = ID_PROTOCOL_LOOSE;
      else
	i = ID_PROTOCOL_SLOPPY;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.nPacketSize == 128)
	i = ID_PROTOCOL_128;
      else if (Xfer.nPacketSize == 256)
	i = ID_PROTOCOL_256;
      else if (Xfer.nPacketSize == 512)
	i = ID_PROTOCOL_512;
      else
	i = ID_PROTOCOL_1024;
      CheckDlgButton(hDlg, i, TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_TIGHT))
	    Xfer.cTiming = XFER_TIMING_TIGHT;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_NORMAL))
	    Xfer.cTiming = XFER_TIMING_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_LOOSE))
	    Xfer.cTiming = XFER_TIMING_LOOSE;
	  else
	    Xfer.cTiming = XFER_TIMING_SLOPPY;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_128))
	    Xfer.nPacketSize = 128;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_256))
	    Xfer.nPacketSize = 256;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_512))
	    Xfer.nPacketSize = 512;
	  else
	    Xfer.nPacketSize = 1024;

	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_PROTOCOL_128:
	case ID_PROTOCOL_256:
	case ID_PROTOCOL_512:
	case ID_PROTOCOL_1024:
	  CheckDlgButton(hDlg, ID_PROTOCOL_128, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_256, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_512, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_1024, FALSE);
	  CheckDlgButton(hDlg, wParam, TRUE);
	  break;

	case ID_PROTOCOL_TIGHT:
	case ID_PROTOCOL_NORMAL:
	case ID_PROTOCOL_LOOSE:
	case ID_PROTOCOL_SLOPPY:
	  CheckRadioButton(hDlg, ID_PROTOCOL_TIGHT, ID_PROTOCOL_SLOPPY, wParam);
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

BOOL DlgProtocolZmodem(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if (Xfer.cTiming == XFER_TIMING_TIGHT)
	i = ID_PROTOCOL_TIGHT;
      else if (Xfer.cTiming == XFER_TIMING_NORMAL)
	i = ID_PROTOCOL_NORMAL;
      else if (Xfer.cTiming == XFER_TIMING_LOOSE)
	i = ID_PROTOCOL_LOOSE;
      else
	i = ID_PROTOCOL_SLOPPY;
      CheckDlgButton(hDlg, i, TRUE);

      if (Xfer.nPacketSize <= 128)
        i = ID_PROTOCOL_128;
      else if (Xfer.nPacketSize <= 256)
        i = ID_PROTOCOL_256;
      else if (Xfer.nPacketSize <= 512)
	i = ID_PROTOCOL_512;
      else
	i = ID_PROTOCOL_1024;
      CheckDlgButton(hDlg, i, TRUE);

      SetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, Xfer.cMaxErrors, FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i = GetDlgItemInt(hDlg, ID_PROTOCOL_MAXERRORS, 0, TRUE);
	  if (i < 0 || i > 20)
            {
            ErrorMessage(IDS_ERRDLG_MAXERRORS);
            break;
            }
          Xfer.cMaxErrors = (char)i;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_128))
            Xfer.nPacketSize = 128;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_256))
            Xfer.nPacketSize = 256;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_512))
	    Xfer.nPacketSize = 512;
	  else
            Xfer.nPacketSize = 1024;

	  if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_TIGHT))
	    Xfer.cTiming = XFER_TIMING_TIGHT;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_NORMAL))
	    Xfer.cTiming = XFER_TIMING_NORMAL;
	  else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_LOOSE))
	    Xfer.cTiming = XFER_TIMING_LOOSE;
	  else
	    Xfer.cTiming = XFER_TIMING_SLOPPY;

	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_PROTOCOL_128:
	case ID_PROTOCOL_256:
	case ID_PROTOCOL_512:
	case ID_PROTOCOL_1024:
	  CheckDlgButton(hDlg, ID_PROTOCOL_128, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_256, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_512, FALSE);
	  CheckDlgButton(hDlg, ID_PROTOCOL_1024, FALSE);
	  CheckDlgButton(hDlg, wParam, TRUE);
	  break;

	case ID_PROTOCOL_TIGHT:
	case ID_PROTOCOL_NORMAL:
	case ID_PROTOCOL_LOOSE:
	case ID_PROTOCOL_SLOPPY:
	  CheckRadioButton(hDlg, ID_PROTOCOL_TIGHT, ID_PROTOCOL_SLOPPY, wParam);
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
