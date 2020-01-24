/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Communication Dialogs (COMMDLG.C)                               *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains all the communication and modem dialogs.   *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/23/89 00020 Make Windows 3.0 compatible.                *
*	      1.01 10/31/89 00052 Add context help to the menus and dialogs.	   *
*	1.1 03/13/90 dca00048 MKL added COM3, COM4 support.						   *
*	1.1b 05/18/90 dca00099 PJL Get rid of 'include "device.h"'.				   *
*	1.1b 05/18/90 dca00098 PJL Fix Modem string save-to-disk problem.		   *
*	1.1c 06/05/90 dca00103 PJL Setup.Modem not loading Modem params properly.  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "prodata.h"


  BOOL DlgCommunications(HWND, unsigned, WORD, LONG);
  BOOL DlgModem(HWND, unsigned, WORD, LONG);
  BOOL DlgSetting(HWND, unsigned, WORD, LONG);

  static void near CommDlgEnable(HWND hDlg);
  static char cCurrentPort;


void CmdCommunications()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_DEVICE;

  lpDlg = MakeProcInstance((FARPROC)DlgCommunications, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_COMMUNICATIONS), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}

void CmdModem()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_MODEM;

  lpDlg = MakeProcInstance((FARPROC)DlgModem, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_MODEM), hWnd, lpDlg))
    {
    FreeProcInstance(lpDlg);
    lpDlg = MakeProcInstance((FARPROC)DlgSetting, hWndInst);
    DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SETTING), hWnd, lpDlg);
    }
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


BOOL FAR PASCAL DlgCommunications(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char buf1[8], buf2[128], buf3[128];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      // dca00048 MKL added COM3, COM4 support
      if (sVersion < 3) {
	/* hide COM3, COM4 buttons if not running Windows 3.0 or later */
	EnableWindow(GetDlgItem(hDlg, ID_COMM_COM3), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_COMM_COM4), FALSE);
      }
      if (Comm.cPort == 0)
	CheckDlgButton(hDlg, ID_COMM_COM1, TRUE);
      else if (Comm.cPort == 1)
	CheckDlgButton(hDlg, ID_COMM_COM2, TRUE);
      // dca00048 MKL added COM3, COM4 support
      else if (Comm.cPort == 2) {
	CheckDlgButton(hDlg, ID_COMM_COM3, TRUE);
      }
      else if (Comm.cPort == 3) {
	CheckDlgButton(hDlg, ID_COMM_COM4, TRUE);
      }
      else
	CheckDlgButton(hDlg, ID_COMM_CURRENT, TRUE);

      if (bConnected)
	{
        // dca00048 MKL added COM3, COM4 support
	// if (cCommPort == 0)
	//   EnableWindow(GetDlgItem(hDlg, ID_COMM_COM2), FALSE);
	// else 
	//   EnableWindow(GetDlgItem(hDlg, ID_COMM_COM1), FALSE);
	   switch (cCommPort) {
	   case 0:
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM2), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM3), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM4), FALSE);
	      break;
	   case 1:
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM1), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM3), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM4), FALSE);
	      break;
	   case 2:
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM1), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM2), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM4), FALSE);
	      break;
	   case 3:
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM1), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM2), FALSE);
	      EnableWindow(GetDlgItem(hDlg, ID_COMM_COM3), FALSE);
	      break;
	   }
	}

      if (Comm.nSpeed == 110)
	i = ID_COMM_110;
      else if (Comm.nSpeed == 300)
	i = ID_COMM_300;
      else if (Comm.nSpeed == 600)
	i = ID_COMM_600;
      else if (Comm.nSpeed == 1200)
	i = ID_COMM_1200;
      else if (Comm.nSpeed == 2400)
	i = ID_COMM_2400;
      else if (Comm.nSpeed == 4800)
	i = ID_COMM_4800;
      else if (Comm.nSpeed == 9600)
	i = ID_COMM_9600;
      else
	i = ID_COMM_19200;
      CheckDlgButton(hDlg, i, TRUE);

      if (Comm.cDataBits == 7)
	i = ID_COMM_WORD7;
      else
	i = ID_COMM_WORD8;
      CheckDlgButton(hDlg, i, TRUE);

      if (Comm.cParity == NOPARITY)
	i = ID_COMM_NOPAR;
      else if (Comm.cParity == ODDPARITY)
	i = ID_COMM_ODD;
      else if (Comm.cParity == EVENPARITY)
	i = ID_COMM_EVEN;
      else if (Comm.cParity == MARKPARITY)
	i = ID_COMM_MARK;
      else
	i = ID_COMM_SPACE;
      CheckDlgButton(hDlg, i, TRUE);

      if (Comm.cStopBits == ONESTOPBIT)
	i = ID_COMM_STOP1;
      else
	i = ID_COMM_STOP2;
      CheckDlgButton(hDlg, i, TRUE);

      if (Comm.nBreakLen <= 10)
	i = ID_COMM_B10;
      else if (Comm.nBreakLen <= 150)
	i = ID_COMM_B150;
      else if (Comm.nBreakLen <= 200)
	i = ID_COMM_B200;
      else if (Comm.nBreakLen <= 250)
	i = ID_COMM_B250;
      else if (Comm.nBreakLen <= 300)
	i = ID_COMM_B300;
      else if (Comm.nBreakLen <= 500)
	i = ID_COMM_B500;
      else if (Comm.nBreakLen <= 1000)
	i = ID_COMM_B1000;
      else
	i = ID_COMM_B2000;
      CheckDlgButton(hDlg, i, TRUE);

      if (Comm.cLocal)
	i = ID_COMM_ON;
      else
	i = ID_COMM_OFF;
      CheckDlgButton(hDlg, i, TRUE);

      if (Comm.cFlow == MODEM_NONE)
	i = ID_COMM_NONE;
      else if (Comm.cFlow == MODEM_XON)
	i = ID_COMM_XON;
      else if (Comm.cFlow == MODEM_RTS)
	i = ID_COMM_RTS;
      else
	i = ID_COMM_BOTH;
      CheckDlgButton(hDlg, i, TRUE);

      return (TRUE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	  if (IsDlgButtonChecked(hDlg, ID_COMM_COM1))
	    Comm.cPort = 0;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_COM2))
	    Comm.cPort = 1;
          // dca00048 MKL added COM3, COM4 support
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_COM3))
	    Comm.cPort = 2;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_COM4))
	    Comm.cPort = 3;
	  else
	    Comm.cPort = -1;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_110))
	    Comm.nSpeed = 110;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_300))
	    Comm.nSpeed = 300;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_600))
	    Comm.nSpeed = 600;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_1200))
	    Comm.nSpeed = 1200;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_2400))
	    Comm.nSpeed = 2400;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_4800))
	    Comm.nSpeed = 4800;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_9600))
	    Comm.nSpeed = 9600;
	  else
	    Comm.nSpeed = 19200;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_WORD7))
	    Comm.cDataBits = 7;
	  else
	    Comm.cDataBits = 8;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_EVEN))
	    Comm.cParity = EVENPARITY;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_ODD))
	    Comm.cParity = ODDPARITY;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_NOPAR))
	    Comm.cParity = NOPARITY;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_MARK))
	    Comm.cParity = MARKPARITY;
	  else
	    Comm.cParity = SPACEPARITY;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_STOP1))
	    Comm.cStopBits = ONESTOPBIT;
	  else
	    Comm.cStopBits = TWOSTOPBITS;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_B10))
	    Comm.nBreakLen = 10;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_B150))
	    Comm.nBreakLen = 150;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_B200))
	    Comm.nBreakLen = 200;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_B250))
	    Comm.nBreakLen = 250;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_B300))
	    Comm.nBreakLen = 300;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_B500))
	    Comm.nBreakLen = 500;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_B1000))
	    Comm.nBreakLen = 1000;
	  else
	    Comm.nBreakLen = 2000;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_ON))
	    Comm.cLocal = TRUE;
	  else
	    Comm.cLocal = FALSE;

	  if (IsDlgButtonChecked(hDlg, ID_COMM_NONE))
	    Comm.cFlow = MODEM_NONE;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_XON))
	    Comm.cFlow = MODEM_XON;
	  else if (IsDlgButtonChecked(hDlg, ID_COMM_RTS))
	    Comm.cFlow = MODEM_RTS;
	  else
	    Comm.cFlow = MODEM_BOTH;

	  CommPortUpdate();

          fDataChanged = TRUE;

	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_COMM_COM1:
	case ID_COMM_COM2:
	case ID_COMM_COM3:
	case ID_COMM_COM4:
	case ID_COMM_CURRENT:
	  CheckRadioButton(hDlg, ID_COMM_COM1, ID_COMM_CURRENT, wParam);
	  break;

	case ID_COMM_110:
	case ID_COMM_300:
	case ID_COMM_600:
	case ID_COMM_1200:
	case ID_COMM_2400:
	case ID_COMM_4800:
	case ID_COMM_9600:
	case ID_COMM_19200:
	  CheckRadioButton(hDlg, ID_COMM_110, ID_COMM_19200, wParam);
	  break;

	case ID_COMM_WORD7:
	case ID_COMM_WORD8:
	  CheckRadioButton(hDlg, ID_COMM_WORD7, ID_COMM_WORD8, wParam);
	  break;

	case ID_COMM_EVEN:
	case ID_COMM_ODD:
	case ID_COMM_NOPAR:
	case ID_COMM_MARK:
	case ID_COMM_SPACE:
	  CheckRadioButton(hDlg, ID_COMM_EVEN, ID_COMM_SPACE, wParam);
	  break;

	case ID_COMM_B10:
	case ID_COMM_B150:
	case ID_COMM_B200:
	case ID_COMM_B250:
	case ID_COMM_B300:
	case ID_COMM_B500:
	case ID_COMM_B1000:
	case ID_COMM_B2000:
	  CheckRadioButton(hDlg, ID_COMM_B10, ID_COMM_B2000, wParam);
	  break;

	case ID_COMM_STOP1:
	case ID_COMM_STOP2:
	  CheckRadioButton(hDlg, ID_COMM_STOP1, ID_COMM_STOP2, wParam);
	  break;

	case ID_COMM_ON:
	case ID_COMM_OFF:
	  CheckRadioButton(hDlg, ID_COMM_ON, ID_COMM_OFF, wParam);
	  break;

	case ID_COMM_NONE:
	case ID_COMM_XON:
	case ID_COMM_RTS:
	case ID_COMM_BOTH:
	  CheckRadioButton(hDlg, ID_COMM_NONE, ID_COMM_BOTH, wParam);
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return (FALSE);
      break;
    }

  return (TRUE);
}


static void near CommDlgEnable(HWND hDlg)
  {
  if (IsDlgButtonChecked(hDlg, ID_COMM_COMBIOS))
    {
    CheckDlgButton(hDlg, ID_COMM_NONE, TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_XON), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_RTS), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_BOTH), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_19200), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_MARK), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_SPACE), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B10), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B150), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B200), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B250), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B300), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B500), FALSE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B1000), FALSE);
    }
  else
    {
    EnableWindow(GetDlgItem(hDlg, ID_COMM_XON), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_RTS), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_BOTH), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_19200), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_MARK), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_SPACE), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B10),  TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B150), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B200), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B250), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B300), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B500), TRUE);
    EnableWindow(GetDlgItem(hDlg, ID_COMM_B1000), TRUE);
    }
  }


BOOL FAR PASCAL DlgModem(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
	char buf[32];
	register int i;

	switch (message)
	{
	case WM_INITDIALOG:
		// dca00048 MKL added COM3, COM4 support
		if (sVersion < 3)
		{
			/* hide COM3, COM4 buttons if not running Windows 3.0 or later */
			EnableWindow(GetDlgItem(hDlg, ID_MODEM_COM3), FALSE);
			EnableWindow(GetDlgItem(hDlg, ID_MODEM_COM4), FALSE);
		}
		cCurrentPort = cCommPort;

		if (cCommPort == 0)
			CheckDlgButton(hDlg, ID_MODEM_COM1, TRUE);
		else if (cCommPort == 1)
			CheckDlgButton(hDlg, ID_MODEM_COM2, TRUE);
		// dca00048 MKL added COM3, COM4 support
		else if (cCommPort == 2)
			CheckDlgButton(hDlg, ID_MODEM_COM3, TRUE);
		else
			CheckDlgButton(hDlg, ID_MODEM_COM4, TRUE);

		for (i = IDS_MODEM_FIRST; i <= IDS_MODEM_LAST; i++)
		{
			register j;

			LoadString(hWndInst, i, buf, sizeof(buf));
			for (j = 0; j < sizeof(buf) - 1 && buf[j] != ','; j++);
			buf[j] = 0;
			SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, NULL, (DWORD)((LPSTR)buf));
		}

		// dca00103 PJL Setup.Modem not loading Modem params properly.
		LoadModemParameters();
		if (Comm.cLocal)
			LoadString(hWndInst, IDS_MODEM_DIRECT, buf, sizeof(buf));
		else
			LibEnstore(Modem.szName, buf, sizeof(buf));
		(void)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SELECTSTRING, -1, (DWORD)((LPSTR)buf));
		if (memcmp(buf, "Direct", 6) == 0)
			EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_OK:
			// dca00098 PJL
			SaveModemParameters();
			EndDialog(hDlg, FALSE);
			break;

		case ID_CANCEL:
			cCommPort = cCurrentPort;
			ReadModemParameters(IDS_MODEM_HAYES1200);
			LoadModemParameters();
			EndDialog(hDlg, FALSE);
			break;

		case ID_MORE:
			EndDialog(hDlg, TRUE);
			break;

		case ID_LISTBOX:
			if (HIWORD(lParam) == LBN_SELCHANGE)
			{
				i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L);

				if (i >= 0 && i <= IDS_MODEM_LAST - IDS_MODEM_FIRST)
					ReadModemParameters(IDS_MODEM_FIRST + i);

				if (i == IDS_MODEM_CUSTOM - IDS_MODEM_FIRST)
					EndDialog(hDlg, TRUE);
				else if (i == IDS_MODEM_DIRECT - IDS_MODEM_FIRST)
					EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
				else
					EnableWindow(GetDlgItem(hDlg, ID_MORE), TRUE);
			}
			break;

		case ID_MODEM_COM1:
		case ID_MODEM_COM2:
		case ID_MODEM_COM3:
		case ID_MODEM_COM4:
			// dca00048 MKL added COM3, COM4 support
			// CheckRadioButton(hDlg, ID_MODEM_COM1, ID_MODEM_COM2, wParam);
			CheckRadioButton(hDlg, ID_MODEM_COM1, ID_MODEM_COM4, wParam);
			// if (cCommPort != 0)
			if (cCommPort != (char)(wParam - ID_MODEM_COM1))
			{
				// cCommPort = 0;
				cCommPort = (char) (wParam - ID_MODEM_COM1);
				ReadModemParameters(IDS_MODEM_HAYES1200);
				LoadModemParameters();
				if (Comm.cLocal && cCommPort == cCurrentPort)
					LoadString(hWndInst, IDS_MODEM_DIRECT, buf, sizeof(buf));
				else
					LibEnstore(Modem.szName, buf, sizeof(buf));
				(void)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SELECTSTRING, -1, (DWORD)((LPSTR)buf));
			}
			break;

		/* MKL commented this out
		case ID_MODEM_COM2:
			CheckRadioButton(hDlg, ID_MODEM_COM1, ID_MODEM_COM4, wParam);
			if (cCommPort != 1)
			{
				cCommPort = 1;
				ReadModemParameters(IDS_MODEM_HAYES1200);
				LoadModemParameters();
				if (Comm.cLocal && cCommPort == cCurrentPort)
					LoadString(hWndInst, IDS_MODEM_DIRECT, buf, sizeof(buf));
				else
					LibEnstore(Modem.szName, buf, sizeof(buf));
				(void)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SELECTSTRING, -1, (DWORD)((LPSTR)buf));
			}
			break;
		*/

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


BOOL DlgSetting(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char buf[32];
  register int i;

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemLength(hDlg, ID_MODEM_PREFIX,	sizeof(Modem.szDialPrefix)-1);
      SetDlgItemLength(hDlg, ID_MODEM_SUFFIX,	sizeof(Modem.szDialSuffix)-1);
      SetDlgItemLength(hDlg, ID_MODEM_ANSWER,	sizeof(Modem.szAnswerSetup)-1);
      SetDlgItemLength(hDlg, ID_MODEM_INIT,	sizeof(Modem.szModemInit)-1);
      SetDlgItemLength(hDlg, ID_MODEM_HANGUP,	sizeof(Modem.szModemHangup)-1);
      SetDlgItemLength(hDlg, ID_MODEM_OK,	sizeof(Modem.szCommandOk)-1);
      SetDlgItemLength(hDlg, ID_MODEM_ERROR,	sizeof(Modem.szCommandError)-1);
      SetDlgItemLength(hDlg, ID_MODEM_CONNECT,	sizeof(Modem.szConnect)-1);
      SetDlgItemLength(hDlg, ID_MODEM_SPEED,	sizeof(Modem.szConnectSpeed)-1);
      SetDlgItemLength(hDlg, ID_MODEM_ARQ,	sizeof(Modem.szConnectARQ)-1);
      SetDlgItemLength(hDlg, ID_MODEM_DIALTONE, sizeof(Modem.szDialTone)-1);
      SetDlgItemLength(hDlg, ID_MODEM_RING,	sizeof(Modem.szRingDetect)-1);
      SetDlgItemLength(hDlg, ID_MODEM_NOTONE,	sizeof(Modem.szNoDialTone)-1);
      SetDlgItemLength(hDlg, ID_MODEM_BUSY,	sizeof(Modem.szBusy)-1);
      SetDlgItemLength(hDlg, ID_MODEM_CARRIER,	sizeof(Modem.szNoCarrier)-1);

      SetDlgItemData(hDlg, ID_MODEM_PREFIX,   Modem.szDialPrefix);
      SetDlgItemData(hDlg, ID_MODEM_SUFFIX,   Modem.szDialSuffix);
      SetDlgItemData(hDlg, ID_MODEM_ANSWER,   Modem.szAnswerSetup);
      SetDlgItemData(hDlg, ID_MODEM_INIT,     Modem.szModemInit);
      SetDlgItemData(hDlg, ID_MODEM_HANGUP,   Modem.szModemHangup);
      SetDlgItemData(hDlg, ID_MODEM_OK,       Modem.szCommandOk);
      SetDlgItemData(hDlg, ID_MODEM_ERROR,    Modem.szCommandError);
      SetDlgItemData(hDlg, ID_MODEM_CONNECT,  Modem.szConnect);
      SetDlgItemData(hDlg, ID_MODEM_SPEED,    Modem.szConnectSpeed);
      SetDlgItemData(hDlg, ID_MODEM_ARQ,      Modem.szConnectARQ);
      SetDlgItemData(hDlg, ID_MODEM_DIALTONE, Modem.szDialTone);
      SetDlgItemData(hDlg, ID_MODEM_RING,     Modem.szRingDetect);
      SetDlgItemData(hDlg, ID_MODEM_NOTONE,   Modem.szNoDialTone);
      SetDlgItemData(hDlg, ID_MODEM_BUSY,     Modem.szBusy);
      SetDlgItemData(hDlg, ID_MODEM_CARRIER,  Modem.szNoCarrier);

      CheckDlgButton(hDlg, ID_MODEM_CHANGE, Modem.cChange);

      LibEnstore(Modem.szName, buf, sizeof(buf));
      SetDlgItemText(hDlg, ID_EDIT, buf);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	  GetDlgItemData(hDlg, ID_MODEM_PREFIX,   Modem.szDialPrefix, sizeof(Modem.szDialPrefix));
	  GetDlgItemData(hDlg, ID_MODEM_SUFFIX,   Modem.szDialSuffix, sizeof(Modem.szDialSuffix));
	  GetDlgItemData(hDlg, ID_MODEM_ANSWER,   Modem.szAnswerSetup, sizeof(Modem.szAnswerSetup));
	  GetDlgItemData(hDlg, ID_MODEM_INIT,   Modem.szModemInit, sizeof(Modem.szModemInit));
	  GetDlgItemData(hDlg, ID_MODEM_HANGUP,   Modem.szModemHangup, sizeof(Modem.szModemHangup));
	  GetDlgItemData(hDlg, ID_MODEM_OK,	  Modem.szCommandOk, sizeof(Modem.szCommandOk));
	  GetDlgItemData(hDlg, ID_MODEM_ERROR,	  Modem.szCommandError, sizeof(Modem.szCommandError));
	  GetDlgItemData(hDlg, ID_MODEM_CONNECT,  Modem.szConnect, sizeof(Modem.szConnect));
	  GetDlgItemData(hDlg, ID_MODEM_SPEED,	  Modem.szConnectSpeed, sizeof(Modem.szConnectSpeed));
	  GetDlgItemData(hDlg, ID_MODEM_ARQ,	  Modem.szConnectARQ, sizeof(Modem.szConnectARQ));
	  GetDlgItemData(hDlg, ID_MODEM_DIALTONE, Modem.szDialTone, sizeof(Modem.szDialTone));
	  GetDlgItemData(hDlg, ID_MODEM_RING,	  Modem.szRingDetect, sizeof(Modem.szRingDetect));
	  GetDlgItemData(hDlg, ID_MODEM_NOTONE,   Modem.szNoDialTone, sizeof(Modem.szNoDialTone));
	  GetDlgItemData(hDlg, ID_MODEM_BUSY,	  Modem.szBusy, sizeof(Modem.szBusy));
	  GetDlgItemData(hDlg, ID_MODEM_CARRIER,  Modem.szNoCarrier, sizeof(Modem.szNoCarrier));

	  Modem.cChange = (char)IsDlgButtonChecked(hDlg, ID_MODEM_CHANGE);

	  SaveModemParameters();
	  EndDialog(hDlg, TRUE);
	  break;

        case ID_CANCEL:
	  ReadModemParameters(IDS_MODEM_HAYES1200);
	  LoadModemParameters();
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
