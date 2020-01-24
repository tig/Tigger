/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Macro Dialog Module (MACRODLG.C)                                *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the dialog logic for the macros.           *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	1.1  04/12/90 dca00081 MKL fixed function keys not allow leading      *
*		spaces problem.						      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "macro.h"
#include "disp.h"
#include "sess.h"
#include "term.h"
#include "xfer.h"
#include "dialogs.h"
#include "prodata.h"
#include "ctlcolor.h"


  BOOL DlgMacros(HWND, unsigned, WORD, LONG);
  static void near DisplayGroup(HWND, char);
  static void near SaveGroup(HWND, char);
  static void near SetColorBars(HWND hDlg, BYTE bColor);
  static BYTE near GetColorBars(HWND hDlg);

  static SHORT	sLastMacro;

void CmdMacros()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_FUNCTIONS;

  lpDlg = MakeProcInstance((FARPROC)DlgMacros, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_MACROS), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}

BOOL DlgMacros(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static char	cGroup = 0;
  static HANDLE hSaveMem;
  LPSTR  lptr;
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      if ((hSaveMem = GlobalAlloc(GMEM_FIXED, (DWORD)sizeof(Macro))) == NULL)
	{
	EndDialog(hDlg, FALSE);
	return (TRUE);
	}
      lptr = GlobalLock(hSaveMem);
      lmemcpy(lptr, (LPSTR)&Macro, sizeof(Macro));
      GlobalUnlock(hSaveMem);

      for (i = 0; i < 6; i++)
	SetDlgItemLength(hDlg, ID_MACRO_MACRO1+i, sizeof(Macro.szMacro[0])-1);

      DisplayGroup(hDlg, cGroup);

      CheckDlgButton(hDlg, ID_MACRO_MACRON, Disp.fMacron);
    //CheckDlgButton(hDlg, ID_MACRO_LABEL,  Sess.fLabelOn);
      CheckDlgButton(hDlg, ID_MACRO_GROUP1 + cGroup, TRUE);

      sLastMacro = cGroup * 6;
      SetColorBars(hDlg, Macro.bColor[sLastMacro]);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	  SaveGroup(hDlg, cGroup);
	  Disp.fMacron	= (char)IsDlgButtonChecked(hDlg, ID_MACRO_MACRON);
	//Sess.fLabelOn = (char)IsDlgButtonChecked(hDlg, ID_MACRO_LABEL);
	  Macro.bColor[sLastMacro] = GetColorBars(hDlg);

	  GlobalFree(hSaveMem);

	  UpdateMacro();
	  fDataChanged = TRUE;
	  EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
	  lptr = GlobalLock(hSaveMem);
	  lmemcpy((LPSTR)&Macro, lptr, sizeof(Macro));
	  GlobalUnlock(hSaveMem);
	  GlobalFree(hSaveMem);

	  EndDialog(hDlg, FALSE);
	  break;

	case ID_MACRO_MACRO1:
	case ID_MACRO_MACRO2:
	case ID_MACRO_MACRO3:
	case ID_MACRO_MACRO4:
	case ID_MACRO_MACRO5:
	case ID_MACRO_MACRO6:
	  if (HIWORD(lParam) == EN_SETFOCUS)
	    {
	    Macro.bColor[sLastMacro] = GetColorBars(hDlg);
	    sLastMacro = cGroup * 6 + (wParam - ID_MACRO_MACRO1);
	    SetColorBars(hDlg, Macro.bColor[sLastMacro]);
	    }
	  break;

	case ID_MACRO_LABEL1:
	case ID_MACRO_LABEL2:
	case ID_MACRO_LABEL3:
	case ID_MACRO_LABEL4:
	case ID_MACRO_LABEL5:
	case ID_MACRO_LABEL6:
	  if (HIWORD(lParam) == EN_SETFOCUS)
	    {
	    Macro.bColor[sLastMacro] = GetColorBars(hDlg);
	    sLastMacro = cGroup * 6 + (wParam - ID_MACRO_LABEL1);
	    SetColorBars(hDlg, Macro.bColor[sLastMacro]);
	    }
	  break;

	case ID_MACRO_GROUP1:
	  if (cGroup != 0)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 0);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, 0);
            }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

	case ID_MACRO_GROUP2:
	  if (cGroup != 1)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 1);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, MSGDLG_KEYS_CONTROL1);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

	case ID_MACRO_GROUP3:
	  if (cGroup != 2)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 2);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, 0);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

        case ID_MACRO_GROUP4:
	  if (cGroup != 3)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 3);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, MSGDLG_KEYS_CONTROL1);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

        case ID_MACRO_GROUP5:
	  if (cGroup != 4)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 4);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, 0);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

        case ID_MACRO_GROUP6:
	  if (cGroup != 5)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 5);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, 0);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

        case ID_MACRO_GROUP7:
	  if (cGroup != 6)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 6);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, 0);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
	  break;

        case ID_MACRO_GROUP8:
	  if (cGroup != 7)
	    {
	    SaveGroup(hDlg, cGroup);
	    DisplayGroup(hDlg, cGroup = 7);
	    SetDlgItemStr(hDlg, ID_MACRO_NOTE, 0);
	    }
	  CheckRadioButton(hDlg, ID_MACRO_GROUP1, ID_MACRO_GROUP8, wParam);
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

static void near DisplayGroup(hDlg, cGroup)
  HWND hDlg;
  char cGroup;
{
  char buf[16];
  register int i, j;


  for (j = cGroup * 6; j < (cGroup+1) * 6; j++)
    {
    if (j < 12)
      {
      buf[0] = ' ';
      buf[1] = 'S';
      buf[2] = 'h';
      buf[3] = 'i';
      buf[4] = 'f';
      buf[5] = 't';
      buf[6] = '-';
      buf[7] = 'F';
      i = j;
      }
    else if (j < 24)
      {
      buf[0] = ' ';
      buf[1] = ' ';
      buf[2] = 'C';
      buf[3] = 't';
      buf[4] = 'r';
      buf[5] = 'l';
      buf[6] = '-';
      buf[7] = 'F';
      i = j - 12;
      }
    else if (j < 36)
      {
      buf[0] = ' ';
      buf[1] = ' ';
      buf[2] = 'C';
      buf[3] = 't';
      buf[4] = 'r';
      buf[5] = 'l';
      buf[6] = '-';
      buf[7] = (char)('A' + j - 24);
      buf[8] = NULL;
      i = 24;
      }
    else
      {
      buf[0] = ' ';
      buf[1] = ' ';
      buf[2] = 'C';
      buf[3] = 't';
      buf[4] = 'r';
      buf[5] = 'l';
      buf[6] = '-';
      if (j < 40)
        buf[7] = (char)('A' + j - 24);
      else if (j == 40)
        buf[7] = 'R';
      else
        buf[7] = (char)('A' + j - 22);

      buf[8] = NULL;
      i = 36;
      }

    if (i == 9)
      {
      if (buf[2] == ' ')
        buf[2] = '*';
      else if (buf[1] == ' ')
        buf[1] = '*';
      else if (buf[0] == ' ')
        buf[0] = '*';
      }

    if (i < 9)
      {
      buf[8] = (char)('1' + i);
      buf[9] = NULL;
      }
    else if (i < 24)
      {
      buf[8] = (char)('0' + (i+1) / 10);
      buf[9] = (char)('0' + (i+1) % 10);
      buf[10] = NULL;
      }

    i = j % 6;
    SetDlgItemText(hDlg, ID_MACRO_TITLE1 + i, buf);

    SetDlgItemText(hDlg, ID_MACRO_MACRO1 + i, Macro.szMacro[j]);
    SetDlgItemText(hDlg, ID_MACRO_LABEL1 + i, Macro.szLabel[j]);
    CheckDlgButton(hDlg, ID_MACRO_MFLAG1 + i, Macro.fMenu[j]);
    }

  sLastMacro = cGroup * 6;
  SetColorBars(hDlg, Macro.bColor[sLastMacro]);
}


static void near SaveGroup(hDlg, cGroup)
  HWND hDlg;
  char cGroup;
{
  register int i, j;

  for (i = 0, j = cGroup * 6; i < 6; i++, j++)
    {
    memset(Macro.szMacro[j], NULL, sizeof(Macro.szMacro[0]));
    memset(Macro.szLabel[j], NULL, sizeof(Macro.szLabel[0]));

    GetDlgItemText(hDlg, ID_MACRO_MACRO1 + i, Macro.szMacro[j],
		   sizeof(Macro.szMacro[0]));

    GetDlgItemText(hDlg, ID_MACRO_LABEL1 + i, Macro.szLabel[j],
		   sizeof(Macro.szLabel[0]));
    /* dca00081 MKL fixed function key label leading spaces problem
    if (Macro.szLabel[j][0] == ' ')
      Macro.szLabel[j][0] = NULL;
    */

    Macro.fMenu[j] = (char)IsDlgButtonChecked(hDlg, ID_MACRO_MFLAG1 + i);
    }
}


static void near SetColorBars(HWND hDlg, BYTE bColor)
  {
  USHORT wColor;

  if (bColor == 0)
    bColor = 0x07;


  GetColorBar(GetDlgItem(hDlg, ID_MACRO_COLORBAR1), &wColor);
  if ((BYTE)wColor != bColor / 16)
    SetColorBar(GetDlgItem(hDlg, ID_MACRO_COLORBAR1), bColor / 16);

  GetColorBar(GetDlgItem(hDlg, ID_MACRO_COLORBAR2), &wColor);
  if ((BYTE)wColor != bColor & 0x0F)
    SetColorBar(GetDlgItem(hDlg, ID_MACRO_COLORBAR2), bColor & 0x0F);
  }


static BYTE near GetColorBars(HWND hDlg)
  {
  USHORT wColorF, wColorB;

  GetColorBar(GetDlgItem(hDlg, ID_MACRO_COLORBAR1), &wColorF);
  GetColorBar(GetDlgItem(hDlg, ID_MACRO_COLORBAR2), &wColorB);

  return ((BYTE)(wColorF * 16 + wColorB));
  }
