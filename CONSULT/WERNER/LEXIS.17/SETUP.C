/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Setup Dialog (SETUP.C)                                           */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <direct.h>
#include <string.h>
#include "lexis.h"
#include "data.h"
#include "session.h"
#include "dialogs.h"
#include "library.h"


  static char  fMaskHelp;
  static WORD  wLastMsg;
  static short sMaxSpeed;

  BOOL DlgSetup(HWND, unsigned, WORD, LONG);
  static void near pascal HelpMessage(HWND, WORD);
  static void near pascal ClearModemListbox(HWND);
  static void near pascal LoadModemListbox(HWND);


void CmdSetup()
  {
  FARPROC lpDlg;


  SetFilePath(szWorkCWD);

  lpDlg = MakeProcInstance((FARPROC)DlgSetup, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_SETUP), hWnd, lpDlg))
    ResetScreenDisplay();
  FreeProcInstance(lpDlg);
  }


BOOL DlgSetup(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
  {
  switch (message)
    {
    case WM_INITDIALOG:
      {
      char szList[512];
      register i;


      SetDlgItemText(hDlg, ID_SETUP_NETWORK1, szDataNetwork[0]);
      SetDlgItemText(hDlg, ID_SETUP_NETWORK2, szDataNetwork[1]);
      SetDlgItemText(hDlg, ID_SETUP_NETWORK3, szDataNetwork[2]);
      SetDlgItemText(hDlg, ID_SETUP_PREFIX1, szDataPrefix[0]);
      SetDlgItemText(hDlg, ID_SETUP_PREFIX2, szDataPrefix[1]);
      SetDlgItemText(hDlg, ID_SETUP_PREFIX3, szDataPrefix[2]);
      SetDlgItemText(hDlg, ID_SETUP_NUMBER1, szDataNumber[0]);
      SetDlgItemText(hDlg, ID_SETUP_NUMBER2, szDataNumber[1]);
      SetDlgItemText(hDlg, ID_SETUP_NUMBER3, szDataNumber[2]);
      SetDlgItemInt(hDlg, ID_SETUP_SPEED1, wDataSpeed[0], FALSE);
      SetDlgItemInt(hDlg, ID_SETUP_SPEED2, wDataSpeed[1], FALSE);
      SetDlgItemInt(hDlg, ID_SETUP_SPEED3, wDataSpeed[2], FALSE);
      SetDlgItemText(hDlg, ID_SETUP_NODE1, szDataNode[0]);
      SetDlgItemText(hDlg, ID_SETUP_NODE2, szDataNode[1]);
      SetDlgItemText(hDlg, ID_SETUP_NODE3, szDataNode[2]);

      SendDlgItemMessage(hDlg, ID_SETUP_ID,
                         EM_LIMITTEXT, sizeof(szDataLogon)-1, 0L);
      SendDlgItemMessage(hDlg, ID_SETUP_PREFIX,
                         EM_LIMITTEXT, sizeof(szDataPrefix)-1, 0L);
      SendDlgItemMessage(hDlg, ID_SETUP_NUMBER,
                         EM_LIMITTEXT, sizeof(szDataNumber)-1, 0L);
      SendDlgItemMessage(hDlg, ID_SETUP_NODE,
                         EM_LIMITTEXT, sizeof(szDataNode)-1, 0L);

      SetDlgItemText(hDlg, ID_SETUP_ID, szDataLogon);

      CheckDlgButton(hDlg, ID_SETUP_BELL, fDataBell);
      CheckDlgButton(hDlg, ID_SETUP_FASTPRINT, fDataFastPrint);
      CheckDlgButton(hDlg, ID_SETUP_STATUS, !fDataDisplayStatus);
      CheckDlgButton(hDlg, ID_SETUP_MACRO, !fDataDisplayMacro);
      CheckDlgButton(hDlg, ID_SETUP_INFO, !fDataDisplayInfo);

      if (wDataPort == DATA_COM1)
        i = ID_SETUP_COM1;
      else
        i = ID_SETUP_COM2;
      CheckDlgButton(hDlg, i, TRUE);

      CheckDlgButton(hDlg, ID_SETUP_DIRECT, fDataHardwire);
      if (fDataHardwire)
        ClearModemListbox(hDlg);
      else
        LoadModemListbox(hDlg);

      CheckDlgButton(hDlg, ID_SETUP_PBX, fDataPBX);

      if (wDataParity == DATA_EVEN)
        i = ID_SETUP_EVEN;
      else if (wDataParity == DATA_ODD)
        i = ID_SETUP_ODD;
      else
        i = ID_SETUP_NONE;
      CheckDlgButton(hDlg, i, TRUE);

      if (wDataDial == DATA_TONE)
        i = ID_SETUP_TONE;
      else if (wDataDial == DATA_PULSE)
        i = ID_SETUP_PULSE;
      else
        i = ID_SETUP_MANUAL;
      CheckDlgButton(hDlg, i, TRUE);

      DataQueryProfileKeyword(IDS_DATA_NETWORK, 0, szList, sizeof(szList));
      for (i = 0; szList[i];)
        {
        SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                           LB_ADDSTRING, 0, (DWORD)((LPSTR)&szList[i]));
        while (szList[i++]);
        }

      SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                         LB_SELECTSTRING, -1, (DWORD)(LPSTR)szDataNetwork[0]);

      CheckDlgButton(hDlg, ID_SETUP_PRIMARY, TRUE);

      SetDlgItemText(hDlg, ID_SETUP_PREFIX, szDataPrefix[0]);
      SetDlgItemText(hDlg, ID_SETUP_NUMBER, szDataNumber[0]);
      SetDlgItemInt(hDlg,  ID_SETUP_SPEED, wDataSpeed[0], FALSE);
      SetDlgItemText(hDlg, ID_SETUP_NODE, szDataNode[0]);

      SetDlgItemText(hDlg, ID_SETUP_RECORD, szDataDirRecord);
      SetDlgItemText(hDlg, ID_SETUP_NOTEPAD, szDataDirNotepad);
      SetDlgItemText(hDlg, ID_SETUP_MAPS, szDataDirMaps);
      SetDlgItemText(hDlg, ID_SETUP_DOC, szDataDirDoc);

      if (fSessOnline)
	{
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_COM1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_COM2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_DIRECT), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_PBX), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_MODEMS), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_EVEN), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_ODD), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_NONE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_TONE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_PULSE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_SETUP_MANUAL), FALSE);
	}

      fMaskHelp = TRUE;
      wLastMsg  = NULL;
      }
      break;

    case WM_CTLCOLOR:
      if (HIWORD(lParam) == CTLCOLOR_BTN)
        {
        register i;

        i = GetMenu(LOWORD(lParam));
        if (fMaskHelp)
          HelpMessage(hDlg, i);
        else if (i == ID_OK || i == ID_CANCEL)
          HelpMessage(hDlg, GetMenu(GetFocus()));
        }
      return (FALSE);
      break;

    case WM_USER:
      HelpMessage(hDlg, GetMenu(GetFocus()));
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          {
          char buf[64];
          int  j, fManual;
          register i;

          if (IsDlgButtonChecked(hDlg, ID_SETUP_MANUAL))
            {
            fManual = TRUE;
            sMaxSpeed = 9600;
            }
          else
            fManual = FALSE;

          fDataHardwire = IsDlgButtonChecked(hDlg, ID_SETUP_DIRECT);

          GetDlgItemText(hDlg, ID_SETUP_SPEED, buf, sizeof(buf));
          if (!ValidateSpeed(buf))
	    {
            if (sMaxSpeed <= 1200)
              ErrorMessage(IDS_MSG_SPEEDBAD1);
            else if (sMaxSpeed <= 2400)
              ErrorMessage(IDS_MSG_SPEEDBAD2);
            else
              ErrorMessage(IDS_MSG_SPEEDBAD3);

            SetFocus(GetDlgItem(hDlg, ID_SETUP_SPEED));
	    break;
	    }

          GetDlgItemText(hDlg, ID_SETUP_PREFIX, buf, sizeof(buf));
          if (!ValidateNumber(buf) && !fDataHardwire && !fManual)
	    {
            ErrorMessage(IDS_MSG_PREFIXBAD);
	    break;
	    }

          GetDlgItemText(hDlg, ID_SETUP_NUMBER, buf, sizeof(buf));
          if ((!ValidateNumber(buf) || buf[0] == '\0') && !fDataHardwire && !fManual)
	    {
            ErrorMessage(IDS_MSG_NUMBERBAD);
	    break;
	    }

          j = GetDlgItemInt(hDlg, ID_SETUP_SPEED, NULL, FALSE);
          i = IsDlgButtonChecked(hDlg, ID_SETUP_DIRECT);
          if (j > 9600 || (j > sMaxSpeed && sMaxSpeed != 0 && i == FALSE))
	    {
	    ErrorMessage(IDS_MSG_SPEEDHI);
	    break;
	    }
	  else if (j != 300 && j != 1200 && j != 2400 && j != 4800 && j != 9600)
	    {
            if (sMaxSpeed <= 1200)
              ErrorMessage(IDS_MSG_SPEEDBAD1);
            else if (sMaxSpeed <= 2400)
              ErrorMessage(IDS_MSG_SPEEDBAD2);
            else
              ErrorMessage(IDS_MSG_SPEEDBAD3);

            SetFocus(GetDlgItem(hDlg, ID_SETUP_SPEED));
	    break;
	    }

          GetDlgItemText(hDlg, ID_SETUP_ID,
                         szDataLogon, sizeof(szDataLogon));

          fDataBell = IsDlgButtonChecked(hDlg, ID_SETUP_BELL);
          fDataFastPrint = (char)IsDlgButtonChecked(hDlg, ID_SETUP_FASTPRINT);
          fDataDisplayStatus = (char)(!IsDlgButtonChecked(hDlg, ID_SETUP_STATUS));
          fDataDisplayMacro  = (char)(!IsDlgButtonChecked(hDlg, ID_SETUP_MACRO));
          fDataDisplayInfo   = (char)(!IsDlgButtonChecked(hDlg, ID_SETUP_INFO));

          if (IsDlgButtonChecked(hDlg, ID_SETUP_COM1))
            wDataPort = DATA_COM1;
          else
            wDataPort = DATA_COM2;

          fDataHardwire = IsDlgButtonChecked(hDlg, ID_SETUP_DIRECT);

          fDataPBX = IsDlgButtonChecked(hDlg, ID_SETUP_PBX);

          i = (int)SendDlgItemMessage(hDlg, ID_SETUP_MODEMS,
                                      LB_GETCURSEL, 0, 0L);
          if (!fDataHardwire)
            {
            if (i != LB_ERR)
              {
              SendDlgItemMessage(hDlg, ID_SETUP_MODEMS,
                                 LB_GETTEXT, i, (DWORD)(LPSTR)szDataModem);
              }
            else
              szDataModem[0] = NULL;
            }

          if (IsDlgButtonChecked(hDlg, ID_SETUP_EVEN))
            wDataParity = DATA_EVEN;
          else if (IsDlgButtonChecked(hDlg, ID_SETUP_ODD))
            wDataParity = DATA_ODD;
          else
            wDataParity = DATA_NONE;

          if (IsDlgButtonChecked(hDlg, ID_SETUP_TONE))
            wDataDial = DATA_TONE;
          else if (IsDlgButtonChecked(hDlg, ID_SETUP_PULSE))
            wDataDial = DATA_PULSE;
          else
            wDataDial = DATA_MANUAL;

          GetDlgItemText(hDlg, ID_SETUP_NETWORK1,
                         szDataNetwork[0], sizeof(szDataNetwork[0]));
          GetDlgItemText(hDlg, ID_SETUP_NETWORK2,
                         szDataNetwork[1], sizeof(szDataNetwork[0]));
          GetDlgItemText(hDlg, ID_SETUP_NETWORK3,
                         szDataNetwork[2], sizeof(szDataNetwork[0]));

          GetDlgItemText(hDlg, ID_SETUP_PREFIX1,
                         szDataPrefix[0], sizeof(szDataPrefix[0]));
          GetDlgItemText(hDlg, ID_SETUP_PREFIX2,
                         szDataPrefix[1], sizeof(szDataPrefix[0]));
          GetDlgItemText(hDlg, ID_SETUP_PREFIX3,
                         szDataPrefix[2], sizeof(szDataPrefix[0]));

          GetDlgItemText(hDlg, ID_SETUP_NUMBER1,
                         szDataNumber[0], sizeof(szDataNumber[0]));
          GetDlgItemText(hDlg, ID_SETUP_NUMBER2,
                         szDataNumber[1], sizeof(szDataNumber[0]));
          GetDlgItemText(hDlg, ID_SETUP_NUMBER3,
                         szDataNumber[2], sizeof(szDataNumber[0]));

          i = GetDlgItemInt(hDlg, ID_SETUP_SPEED1, NULL, FALSE);
          if (i == 300 || i == 1200 || i == 2400 || i == 4800 || i == 9600)
            wDataSpeed[0] = i;
          i = GetDlgItemInt(hDlg, ID_SETUP_SPEED2, NULL, FALSE);
          if (i == 300 || i == 1200 || i == 2400 || i == 4800 || i == 9600)
            wDataSpeed[1] = i;
          i = GetDlgItemInt(hDlg, ID_SETUP_SPEED3, NULL, FALSE);
          if (i == 300 || i == 1200 || i == 2400 || i == 4800 || i == 9600)
            wDataSpeed[2] = i;

          GetDlgItemText(hDlg, ID_SETUP_NODE1,
                         szDataNode[0], sizeof(szDataNode[0]));
          GetDlgItemText(hDlg, ID_SETUP_NODE2,
                         szDataNode[1], sizeof(szDataNode[0]));
          GetDlgItemText(hDlg, ID_SETUP_NODE3,
                         szDataNode[2], sizeof(szDataNode[0]));

          if (IsDlgButtonChecked(hDlg, ID_SETUP_PRIMARY))
            i = 0;
          else if (IsDlgButtonChecked(hDlg, ID_SETUP_1STALT))
            i = 1;
          else
            i = 2;

          j = (int)SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                                      LB_GETCURSEL, 0, 0L);
          if (j != LB_ERR)
            {
            SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                                LB_GETTEXT, j, (DWORD)(LPSTR)szDataNetwork[i]);
            }
          else
            szDataNetwork[i];

          GetDlgItemText(hDlg, ID_SETUP_PREFIX,
                         szDataPrefix[i], sizeof(szDataPrefix[0]));
          GetDlgItemText(hDlg, ID_SETUP_NUMBER,
                         szDataNumber[i], sizeof(szDataNumber[0]));
	  wDataSpeed[i] = GetDlgItemInt(hDlg, ID_SETUP_SPEED, NULL, FALSE);

          GetDlgItemText(hDlg, ID_SETUP_NODE,
                         szDataNode[i], sizeof(szDataNode[0]));

          GetDlgItemText(hDlg, ID_SETUP_RECORD,
                         szDataDirRecord, sizeof(szDataDirRecord));
          GetDlgItemText(hDlg, ID_SETUP_NOTEPAD,
                         szDataDirNotepad, sizeof(szDataDirNotepad));
          GetDlgItemText(hDlg, ID_SETUP_MAPS,
                         szDataDirMaps, sizeof(szDataDirMaps));
          GetDlgItemText(hDlg, ID_SETUP_DOC,
                         szDataDirDoc, sizeof(szDataDirDoc));

          DataUpdate();
          EndDialog(hDlg, TRUE);
          }
	  break;

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
          break;

	case ID_MORE:
	  {
	  RECT Rect;

	  GetWindowRect(hDlg, &Rect);
          Rect.right -= Rect.left;
          Rect.bottom = 28 * nSysFontHeight;
          if (Rect.top + Rect.bottom > nVertRes)
            Rect.top = (nVertRes - Rect.bottom) / 2;
          if (Rect.left + Rect.right > nHorzRes)
            Rect.left = (nHorzRes - Rect.right) / 2;

          MoveWindow(hDlg, Rect.left, Rect.top, Rect.right, Rect.bottom, TRUE);

	  SetFocus(GetDlgItem(hDlg, ID_OK));
          EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);

          EnableWindow(GetDlgItem(hDlg, ID_SETUP_RECORD), TRUE);
          EnableWindow(GetDlgItem(hDlg, ID_SETUP_NOTEPAD), TRUE);
          EnableWindow(GetDlgItem(hDlg, ID_SETUP_MAPS), TRUE);
          EnableWindow(GetDlgItem(hDlg, ID_SETUP_DOC), TRUE);

          SetFocus(GetDlgItem(hDlg, ID_SETUP_RECORD));
	  }
	  break;

        case ID_SETUP_COM1:
        case ID_SETUP_COM2:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            CheckRadioButton(hDlg, ID_SETUP_COM1, ID_SETUP_COM2, wParam);
            HelpMessage(hDlg, wParam);
            }
          else if (HIWORD(lParam) == NULL)
            HelpMessage(hDlg, wParam);
          break;

        case ID_SETUP_EVEN:
        case ID_SETUP_ODD:
        case ID_SETUP_NONE:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            CheckRadioButton(hDlg, ID_SETUP_EVEN, ID_SETUP_NONE, wParam);
            HelpMessage(hDlg, wParam);
            }
          else if (HIWORD(lParam) == NULL)
            HelpMessage(hDlg, wParam);
          break;

        case ID_SETUP_TONE:
        case ID_SETUP_PULSE:
        case ID_SETUP_MANUAL:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            CheckRadioButton(hDlg, ID_SETUP_TONE, ID_SETUP_MANUAL, wParam);
            HelpMessage(hDlg, wParam);
            }
          else if (HIWORD(lParam) == NULL)
            HelpMessage(hDlg, wParam);
          break;

        case ID_SETUP_DIRECT:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            if (IsDlgButtonChecked(hDlg, ID_SETUP_DIRECT))
              ClearModemListbox(hDlg);
            else
              LoadModemListbox(hDlg);
	    HelpMessage(hDlg, wParam);
            }
          else if (HIWORD(lParam) == NULL)
            HelpMessage(hDlg, wParam);
          break;

        case ID_SETUP_BELL:
        case ID_SETUP_FASTPRINT:
        case ID_SETUP_PBX:
        case ID_SETUP_STATUS:
        case ID_SETUP_MACRO:
        case ID_SETUP_INFO:
          if (HIWORD(lParam) == NULL)
            HelpMessage(hDlg, wParam);
          break;

        case ID_SETUP_PRIMARY:
        case ID_SETUP_1STALT:
        case ID_SETUP_2NDALT:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            char buf[32];
            register i, j;

            if (IsDlgButtonChecked(hDlg, ID_SETUP_PRIMARY))
              i = 0;
            else if (IsDlgButtonChecked(hDlg, ID_SETUP_1STALT))
              i = 1;
            else
              i = 2;

            j = (int)SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                                        LB_GETCURSEL, 0, 0L);
            if (j != LB_ERR)
              {
              SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                                  LB_GETTEXT, j, (DWORD)(LPSTR)buf);
              }
            else
              buf[0] = NULL;
            SetDlgItemText(hDlg, ID_SETUP_NETWORK1+i, buf);

            GetDlgItemText(hDlg, ID_SETUP_PREFIX, buf, sizeof(buf));
            SetDlgItemText(hDlg, ID_SETUP_PREFIX1+i, buf);

            GetDlgItemText(hDlg, ID_SETUP_NUMBER, buf, sizeof(buf));
            SetDlgItemText(hDlg, ID_SETUP_NUMBER1+i, buf);

            j = GetDlgItemInt(hDlg, ID_SETUP_SPEED, NULL, FALSE);
            SetDlgItemInt(hDlg, ID_SETUP_SPEED1+i, j, FALSE);

            GetDlgItemText(hDlg, ID_SETUP_NODE, buf, sizeof(buf));
            SetDlgItemText(hDlg, ID_SETUP_NODE1+i, buf);

            CheckRadioButton(hDlg, ID_SETUP_PRIMARY, ID_SETUP_2NDALT, wParam);

            if (wParam == ID_SETUP_PRIMARY)
              i = 0;
            else if (wParam == ID_SETUP_1STALT)
              i = 1;
            else
              i = 2;

            GetDlgItemText(hDlg, ID_SETUP_NETWORK1+i, buf, sizeof(buf));
            SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                         LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);

            GetDlgItemText(hDlg, ID_SETUP_PREFIX1+i, buf, sizeof(buf));
            SetDlgItemText(hDlg, ID_SETUP_PREFIX, buf);

            GetDlgItemText(hDlg, ID_SETUP_NUMBER1+i, buf, sizeof(buf));
            SetDlgItemText(hDlg, ID_SETUP_NUMBER, buf);

            j = GetDlgItemInt(hDlg, ID_SETUP_SPEED1+i, NULL, FALSE);
            SetDlgItemInt(hDlg, ID_SETUP_SPEED, j, FALSE);

            GetDlgItemText(hDlg, ID_SETUP_NODE1+i, buf, sizeof(buf));
            SetDlgItemText(hDlg, ID_SETUP_NODE, buf);

            HelpMessage(hDlg, wParam);
            }
          else if (HIWORD(lParam) == NULL)
            HelpMessage(hDlg, wParam);
          break;

        case ID_SETUP_MODEMS:
          if (HIWORD(lParam) == 0x0001)
            HelpMessage(hDlg, wParam);

	  if (HIWORD(lParam) == LBN_SELCHANGE)
            {
	    char modem[80], buf[80];
            register i, j;


	    j = (int)SendDlgItemMessage(hDlg, ID_SETUP_MODEMS,
                                      LB_GETCURSEL, 0, 0L);
            if (j != LB_ERR)
              {
	      SendDlgItemMessage(hDlg, ID_SETUP_MODEMS,
				LB_GETTEXT, j, (DWORD)(LPSTR)modem);

	      DataQueryProfileKeyword(IDS_DATA_MODEMS, modem, buf, sizeof(buf));
	      i = 0;
	      while (buf[i] != ',' && buf[i])
		i++;
	      if (buf[i] == ',')
		i++;
	      while (buf[i] != ',' && buf[i])
		i++;
	      if (buf[i] == ',')
		i++;

              if (buf[i])
		{
		sMaxSpeed = atoi(&buf[i]);
              //SetDlgItemText(hDlg, ID_SETUP_SPEED, &buf[i]);
		}
	      else
		sMaxSpeed = 0;
              }

            }
          break;

        case ID_SETUP_NETWORK:
          if (HIWORD(lParam) == 0x0001)
            HelpMessage(hDlg, wParam);

          if (HIWORD(lParam) == LBN_SELCHANGE)
            {
            char net[80], buf[80];
            register i, j;


            j = (int)SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                                      LB_GETCURSEL, 0, 0L);
            if (j != LB_ERR)
              {
              SendDlgItemMessage(hDlg, ID_SETUP_NETWORK,
                                LB_GETTEXT, j, (DWORD)(LPSTR)net);

              DataQueryProfileKeyword(IDS_DATA_NETWORK, net, buf, sizeof(buf));
              for (i = 0; buf[i]; i++)
                {
                if (buf[i] == ',')
                  {
                  i++;
                  break;
                  }
                }

	      SetDlgItemText(hDlg, ID_SETUP_NODE, &buf[i]);
              }

            }
          break;

        case ID_SETUP_ID:
        case ID_SETUP_PREFIX:
        case ID_SETUP_NUMBER:
        case ID_SETUP_SPEED:
        case ID_SETUP_NODE:
        case ID_SETUP_RECORD:
        case ID_SETUP_NOTEPAD:
        case ID_SETUP_MAPS:
        case ID_SETUP_DOC:
          if (HIWORD(lParam) == EN_SETFOCUS)
            HelpMessage(hDlg, wParam);
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


static void near pascal HelpMessage(hDlg, wID)
  HWND hDlg;
  WORD wID;
{
  char buf[96];
  register i;


  if (fMaskHelp)
    {
    if (wID == ID_CANCEL)
      {
      fMaskHelp = FALSE;
      i = IDH_SETUP_FIRST;
      }
    else
      i = -1;
    }
  else
    {
    if (wID == ID_OK)
      i = IDH_OK;
    else if (wID == ID_CANCEL)
      i = IDH_CANCEL;
    else if (wID >= ID_SETUP_FIRST && wID <= ID_SETUP_LAST)
      i = IDH_SETUP_FIRST + wID - ID_SETUP_FIRST;
    else
      i = 0;
    }

  if (i != -1)
    {
    if (i != wLastMsg)
      {
      if (i)
        LoadString(hWndInst, i, buf, sizeof(buf));
      else
        buf[0] = NULL;

      SetDlgItemText(hDlg, ID_SETUP_MESSAGE, buf);
      wLastMsg = i;
      }
    }
}


static void near pascal ClearModemListbox(hDlg)
  HWND hDlg;
{
  SendDlgItemMessage(hDlg, ID_SETUP_MODEMS, LB_RESETCONTENT, 0, 0L);
  EnableWindow(GetDlgItem(hDlg, ID_SETUP_MODEMS), FALSE);
}


static void near pascal LoadModemListbox(hDlg)
  HWND hDlg;
{
  char szList[512], szBuf[128];
  register i;


  EnableWindow(GetDlgItem(hDlg, ID_SETUP_MODEMS), TRUE);

  DataQueryProfileKeyword(IDS_DATA_MODEMS, 0, szList, sizeof(szList));

  for (i = 0; szList[i];)
    {
    SendDlgItemMessage(hDlg, ID_SETUP_MODEMS,
                       LB_ADDSTRING, 0, (DWORD)((LPSTR)&szList[i]));
    while (szList[i++]);
    }

  SendDlgItemMessage(hDlg, ID_SETUP_MODEMS,
                     LB_SELECTSTRING, -1, (DWORD)(LPSTR)szDataModem);

  DataQueryProfileKeyword(IDS_DATA_MODEMS, szDataModem, szBuf, sizeof(szBuf));

  i = 0;
  while (szBuf[i] != ',' && szBuf[i])
    i++;
  if (szBuf[i] == ',')
    i++;
  while (szBuf[i] != ',' && szBuf[i])
    i++;
  if (szBuf[i] == ',')
    i++;

  if (szBuf[i])
    sMaxSpeed = atoi(&szBuf[i]);
  else
    sMaxSpeed = 0;
  }
