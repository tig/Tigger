/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communication Associates Inc.                   *
*                All Rights Reserved
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Dynamic Message Generator Module (DLGMSG.C)                     *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module is responsible for generating and display user      *
*             defined messages through use of the script language.            *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 01/26/90 dca00005 Fixed & problem when user adds & 2 alert *
*                                                                             *
\*****************************************************************************/
#include <windows.h>
#include <string.h>
#include "xtalk.h"
#include "compile.h"
#include "interp.h"
#include "variable.h"
#include "dialogs.h"

/******************************************************************************/
/*									      */
/*  Local Constants & Macros                                                  */
/*									      */
/******************************************************************************/

#define OpCode(x)	(*((WORD far *)(&lpObjectCode[x])))


/******************************************************************************/
/*									      */
/*  Local Variables							      */
/*									      */
/******************************************************************************/

  static int nVarNo;


/******************************************************************************/
/*									      */
/*  Dialog Prototypes                                                         */
/*									      */
/******************************************************************************/

  BOOL DlgAlert(HWND, unsigned, WORD, LONG);


/******************************************************************************/
/*									      */
/*  Local Procedures Prototypes 					      */
/*									      */
/******************************************************************************/

  static SHORT near GenerateDialog();
  static SHORT near DisplayDialog();
  static SHORT near GenerateHeader(int, int, int, int);
  static SHORT near GenerateControl(int, int, int, int, int, PSTR, WORD, int);
  static SHORT near GenerateTrailer(int);


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT AlertInterpret()                                                    */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT AlertInterpret()
{
  FARPROC lpDlg;
  register i;

  lpDlg = MakeProcInstance((FARPROC)DlgAlert, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_ALERT), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  return (i);
}


/******************************************************************************/
/*									      */
/*  Dialog Procedures                                                         */
/*									      */
/******************************************************************************/

BOOL DlgAlert(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static nOk, nCancel;
  HWND   hCtl;


  switch (message)
    {
    case WM_INITDIALOG:
      {
      BYTE szMsg[256], bLen, szButton[4][32];
      RECT RectDlg, RectCtl;
      int  nButton, cxDlg, cxCtl, xPos, xDelta;
      register i;


      nOk = 1;
      nCancel = 2;
      if (i = EvalString(szMsg, &bLen))
        {
        EndDialog(hDlg, i);
        break;
        }

      if (bLen > 64);
        bLen = 64;
      szMsg[bLen] = NULL;

      if (i = EvalComma())
        {
        EndDialog(hDlg, i);
        break;
        }

      nVarNo  = 0;
      nButton = 0;
      while (OpCode(wAddr) != TK_EOPL && OpCode(wAddr) != TK_EOLL &&
             OpCode(wAddr) != TK_EOVL)
        {
        if (OpCode(wAddr) == TK_OK)
          {
          strcpy(szButton[nButton++], "Ok");
          nOk = nButton;
          wAddr += 2;
          }
        else if (OpCode(wAddr) == TK_CANCEL)
          {
          strcpy(szButton[nButton++], "Cancel");
          nCancel = nButton;
          wAddr += 2;
          }
        else if (OpCode(wAddr) == TK_VARIABLE)
          break;
        else
          {
          BYTE buf[256], len;

          	if (i = EvalString(buf, &len))
            	{
            		EndDialog(hDlg, i);
            		return (TRUE);
            	}

		// d0083
		// JDB Fixed the Length problem when user supplies &
	  	if(strchr(buf,'&') == NULL)
	  	{
          		buf[10] = NULL;
          		szButton[nButton][0] = '&';
          		strcpy(&szButton[nButton++][1], buf);
	  	}
	  	else
	  	{
          		buf[11] = NULL;
          		strcpy(&szButton[nButton++][0], buf);
	  	}
          }

        if (OpCode(wAddr) == TK_COMMA)
          wAddr += 2;

        if (nButton > 3)
          break;
        }

      if (OpCode(wAddr) == TK_VARIABLE)
        {
        if (i = EvalStrVariable(&nVarNo, NULL))
          {
          EndDialog(hDlg, i);
          break;
          }
        }

      for (i = 0; i < 4; i++)
        {
        if (i < nButton)
          SetDlgItemText(hDlg, ID_ALERT_BUTTON1+i, szButton[i]);
        else
          ShowWindow(GetDlgItem(hDlg, ID_ALERT_BUTTON1+i), SW_HIDE);
        }

      if (nVarNo)
        {
        SetDlgItemText(hDlg, ID_ALERT_MSG1, szMsg);
        ShowWindow(GetDlgItem(hDlg, ID_ALERT_MSG2), SW_HIDE);
        }
      else
        {
        SetDlgItemText(hDlg, ID_ALERT_MSG2, szMsg);
        ShowWindow(GetDlgItem(hDlg, ID_ALERT_MSG1), SW_HIDE);
        ShowWindow(GetDlgItem(hDlg, ID_ALERT_EDIT), SW_HIDE);
        }

      GetWindowRect(hDlg, &RectDlg);
      GetWindowRect(GetDlgItem(hDlg, ID_ALERT_BUTTON1), &RectCtl);
      cxDlg  = RectDlg.right - RectDlg.left;
      cxCtl  = RectCtl.right - RectCtl.left;
      xPos   = (cxDlg - nButton * cxCtl) / (nButton + 1);
      xDelta = xPos + cxCtl;

      for (i = 0; i < nButton; i++)
        {
        MoveWindow(GetDlgItem(hDlg, ID_ALERT_BUTTON1+i),
                   xPos, RectCtl.top-RectDlg.top,
                   cxCtl, RectCtl.bottom-RectCtl.top,
                   TRUE);
        xPos += xDelta;
        }
      }
      if (!IfEndOfLine())
        EndDialog(hDlg, ERRINT_EXTRATEXT);

      if (nVarNo)
        SetFocus(GetDlgItem(hDlg, ID_ALERT_EDIT));
      else if (hCtl = GetDlgItem(hDlg, nOk+2))
        SetFocus(hCtl);
      else
        return (TRUE);

      return (FALSE);
      break;

    case WM_COMMAND:
      if (wParam == ID_OK)
        {
        nWorkChoice = nOk;

        if (nVarNo)
          {
          char buf[64];

          GetDlgItemText(hDlg, ID_ALERT_EDIT, buf, sizeof(buf));
          SetStrVariable(nVarNo, buf, strlen(buf));
          }

        EndDialog(hDlg, FALSE);
        }
      else if (wParam == ID_CANCEL)
        {
        nWorkChoice = nCancel;
        EndDialog(hDlg, FALSE);

        }
      else if (wParam >= ID_ALERT_BUTTON1 && wParam <= ID_ALERT_BUTTON4)
        {
        nWorkChoice = wParam - 2;

        if (nVarNo && nCancel != wParam - 2)
          {
          char buf[64];

          GetDlgItemText(hDlg, ID_ALERT_EDIT, buf, sizeof(buf));
          SetStrVariable(nVarNo, buf, strlen(buf));
          }

        EndDialog(hDlg, FALSE);
        }
      break;

    default:
      return (FALSE);
      break;
    }

  return (TRUE);
}
