/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Terminal Dialog (TERMDLG.C)                                     *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contain the terminal emulator dialog to inputting   *
*             the emulation settngs.                                          *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 08/03/89 00005 Fix updating problem with terminal setup    *
*				    dialog.  Disable graphic line for 3101.   *
*				    Activate graphic option for VT52.	      *
*	      1.01 10/15/89 00045 Fix the problem of tabbing into the non-    *
*				    displayed area on the dialog.	      *
*	      1.01 10/31/89 00052 Add context help to the menus and dialogs.  *
*	      1.01 12/23/89 00069 GUI correction, #009. 		      *
*    1.1  03/01/90 dca00038 MKL changed setup.terminal.more.graphics text to  *
*			    "Enable graphics mode"			      *
*    1.1  03/01/90 dca00039 MKL set Autowrap to ON if Windowwrap is ON	      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "term.h"
#include "sess.h"
#include "font.h"
#include "prodata.h"
#include "variable.h"


  BOOL DlgTerminal(HWND, unsigned, WORD, LONG);
  static void near DisplayTerminal(HWND);
  static void near SetDialogTerminal(HWND, char);
  static void near ResetTerminal(HWND, char);
  static char fMore;


void CmdTerminal()
  {
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_TERMINAL;

  lpDlg = MakeProcInstance((FARPROC)DlgTerminal, hWndInst);
  if (DialogBox(hWndInst, MAKEINTRESOURCE(IDD_TERMINAL), hWnd, lpDlg) == -1)
    GeneralError(ERRSYS_NOMEMORY);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
  }


BOOL DlgTerminal(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  char buf[128];
  char fUpdate;
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      fMore = FALSE;

      if (Term.szEnter[0] == 0)
	{
	Term.szEnter[0] = 1;
	Term.szEnter[1] = ASC_CR;
	}
      SetDlgItemLength(hDlg, ID_TERM_ENTER, sizeof(Term.szEnter)-1);
      SetDlgItemLength(hDlg, ID_TERM_TABWIDTH, 2);

      SetDialogTerminal(hDlg, Term.cTerminal);
      DisplayTerminal(hDlg);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          i  = GetDlgItemInt(hDlg, ID_TERM_TABWIDTH, 0, 0);
          if (i < 1 || i > 16)
             {
             ErrorMessage(IDS_ERRDLG_TABWIDTH);
             break;
            }
	  if (IsDlgButtonChecked(hDlg, ID_TERM_VT102))
	    i = TERM_VT102;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_VT52))
	    i = TERM_VT52;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_IBMPC))
	    i = TERM_IBMPC;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_IBM3101))
	    i = TERM_IBM3101;
	  else
	    i = TERM_VIDTEX;
	  if (i != Term.cTerminal)
	    {
	    Term.cTerminal = (char)i;
	    fUpdate = TRUE;
	    }
	  else
	    fUpdate = FALSE;

	  if (IsDlgButtonChecked(hDlg, ID_TERM_80))
	    i = 80;
	  else
	    i = 132;
	   if (i != Term.nColumns)
	     {
	     Term.nColumns = i;
	     fUpdate = TRUE;
	     }

	  if (IsDlgButtonChecked(hDlg, ID_TERM_BAR))
	    Term.cCurShape = TERM_CURSOR_BAR;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_LINE))
	    Term.cCurShape = TERM_CURSOR_LINE;
	  else
	    Term.cCurShape = TERM_CURSOR_BLOCK;

	  if (IsDlgButtonChecked(hDlg, ID_TERM_OFF))
	    Term.fLfAuto = FALSE;
	  else
	    Term.fLfAuto = TRUE;

	  if (IsDlgButtonChecked(hDlg, ID_TERM_CR))
	    Term.fNewLine = FALSE;
	  else
	    Term.fNewLine = TRUE;

	  Term.fAutoWrap    = (char)IsDlgButtonChecked(hDlg, ID_TERM_AUTOWRAP);
	  Term.fEcho	    = (char)IsDlgButtonChecked(hDlg, ID_TERM_ECHO);
          Term.fNoColor     = (char)!IsDlgButtonChecked(hDlg, ID_TERM_NOCOLOR);
	  Term.fFormFeed    = (char)IsDlgButtonChecked(hDlg, ID_TERM_FORMFEED);
	  Term.fPrintExtent = (char)IsDlgButtonChecked(hDlg, ID_TERM_PRINTEXTENT);
	  Term.fSwapDel     = (char)IsDlgButtonChecked(hDlg, ID_TERM_SWAPDEL);
	  Term.fWindowWrap  = (char)IsDlgButtonChecked(hDlg, ID_TERM_WINDOWWRAP);
	  // dca00039 MKL set Autowrap to ON if Windowwrap is ON
	  if (Term.fWindowWrap) Term.fAutoWrap = TRUE;

	  Sess.fGraphics    = (char)IsDlgButtonChecked(hDlg, ID_TERM_GRAPHICS);

	  GetDlgItemData(hDlg, ID_TERM_ENTER, Term.szEnter, sizeof(Term.szEnter));
	  if (Term.szEnter[0] == 0)
	    {
	    Term.szEnter[0] = 1;
	    Term.szEnter[1] = ASC_CR;
	    }

	  Term.cTabWidth = (char)GetDlgItemInt(hDlg, ID_TERM_TABWIDTH, NULL, FALSE);
          if (Term.cTabWidth < 1 || Term.cTabWidth > 16)
	    Term.cTabWidth = 8;
          memset(bTermTabs, FALSE, sizeof(bTermTabs));
          for (i = Term.cTabWidth; i < sizeof(bTermTabs); i += Term.cTabWidth)
            bTermTabs[i] = TRUE;

	  fDataChanged = TRUE;

          bTermLFCR      = Term.fLfAuto;
	  bTermEcho	 = Term.fEcho;
	  bTermWrap	 = Term.fAutoWrap;
	  bTermModePrint = Term.fPrintExtent;

	  if (fUpdate)
	    UpdateTerminal();

	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	case ID_MORE:
	  {
	  RECT Rect;

	  GetWindowRect(hDlg, &Rect);
          Rect.right -= Rect.left;
          Rect.bottom = 24 * nSysFontHeight;
          if (Rect.top + Rect.bottom > nVertRes)
            Rect.top = (nVertRes - Rect.bottom) / 2;
          if (Rect.left + Rect.right > nHorzRes)
            Rect.left = (nHorzRes - Rect.right) / 2;

          MoveWindow(hDlg, Rect.left, Rect.top, Rect.right, Rect.bottom, TRUE);

	  UpdateWindow(hDlg);

	  // 00045 Enable tabbing to the now showable area of the dialog.
	  fMore = TRUE;
	  if (IsDlgButtonChecked(hDlg, ID_TERM_VT102))
	    i = TERM_VT102;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_VT52))
	    i = TERM_VT52;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_IBMPC))
	    i = TERM_IBMPC;
	  else if (IsDlgButtonChecked(hDlg, ID_TERM_IBM3101))
	    i = TERM_IBM3101;
	  else
	    i = TERM_VIDTEX;
	  SetDialogTerminal(hDlg, (char)i);

	  // 00069  GUI correction, #009.
	  EnableWindow(GetDlgItem(hDlg, ID_TERM_ECHO), TRUE);
	  SetFocus(GetDlgItem(hDlg, ID_TERM_ECHO));
          EnableWindow(GetDlgItem(hDlg, ID_MORE), FALSE);
	  }
	  break;

	case ID_TERM_VT102:
	  CheckRadioButton(hDlg, ID_TERM_VT102, ID_TERM_VIDTEX, wParam);
	  ResetTerminal(hDlg, TERM_VT102);
	  break;

	case ID_TERM_VT52:
	  CheckRadioButton(hDlg, ID_TERM_VT102, ID_TERM_VIDTEX, wParam);
	  ResetTerminal(hDlg, TERM_VT52);
	  break;

	case ID_TERM_IBMPC:
	  CheckRadioButton(hDlg, ID_TERM_VT102, ID_TERM_VIDTEX, wParam);
	  ResetTerminal(hDlg, TERM_IBMPC);
	  break;

	case ID_TERM_IBM3101:
	  CheckRadioButton(hDlg, ID_TERM_VT102, ID_TERM_VIDTEX, wParam);
	  ResetTerminal(hDlg, TERM_IBM3101);
	  break;

	case ID_TERM_VIDTEX:
	  CheckRadioButton(hDlg, ID_TERM_VT102, ID_TERM_VIDTEX, wParam);
	  ResetTerminal(hDlg, TERM_VIDTEX);
	  break;

	case ID_TERM_80:
	case ID_TERM_132:
	  CheckRadioButton(hDlg, ID_TERM_80, ID_TERM_132, wParam);
	  break;

	case ID_TERM_BAR:
	case ID_TERM_LINE:
	case ID_TERM_BLOCK:
	  CheckRadioButton(hDlg, ID_TERM_BAR, ID_TERM_BLOCK, wParam);
	  break;

	case ID_TERM_OFF:
	case ID_TERM_ADDLF:
	  CheckRadioButton(hDlg, ID_TERM_OFF, ID_TERM_ADDLF, wParam);
	  break;

	case ID_TERM_CR:
	case ID_TERM_CRLF:
	  CheckRadioButton(hDlg, ID_TERM_CR, ID_TERM_CRLF, wParam);
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

static void near DisplayTerminal(hDlg)
  HWND hDlg;
{
  char buf[128];
  register i;


  if (Term.cTerminal == TERM_VT102)
    i = ID_TERM_VT102;
  else if (Term.cTerminal == TERM_VT52)
    i = ID_TERM_VT52;
  else if (Term.cTerminal == TERM_IBMPC)
    i = ID_TERM_IBMPC;
  else if (Term.cTerminal == TERM_IBM3101)
    i = ID_TERM_IBM3101;
  else
    i = ID_TERM_VIDTEX;
  CheckDlgButton(hDlg, i, TRUE);

  if (Term.nColumns == 132)
    i = ID_TERM_132;
  else
    i = ID_TERM_80;
  CheckDlgButton(hDlg, i , TRUE);

  if (Term.cCurShape == TERM_CURSOR_BAR)
    i = ID_TERM_BAR;
  else if (Term.cCurShape == TERM_CURSOR_LINE)
    i = ID_TERM_LINE;
  else
    i = ID_TERM_BLOCK;
  CheckDlgButton(hDlg, i, TRUE);

  if (Term.fLfAuto)
    i = ID_TERM_ADDLF;
  else
    i = ID_TERM_OFF;
  CheckDlgButton(hDlg, i, TRUE);

  if (Term.fNewLine)
    i = ID_TERM_CRLF;
  else
    i = ID_TERM_CR;
  CheckDlgButton(hDlg, i, TRUE);

  CheckDlgButton(hDlg, ID_TERM_AUTOWRAP,    Term.fAutoWrap);
  CheckDlgButton(hDlg, ID_TERM_ECHO,        Term.fEcho);
  CheckDlgButton(hDlg, ID_TERM_FORMFEED,    Term.fFormFeed);
  CheckDlgButton(hDlg, ID_TERM_GRAPHICS,    Sess.fGraphics);
  CheckDlgButton(hDlg, ID_TERM_NOCOLOR,     !Term.fNoColor);
  CheckDlgButton(hDlg, ID_TERM_PRINTEXTENT, Term.fPrintExtent);
  CheckDlgButton(hDlg, ID_TERM_SWAPDEL,     Term.fSwapDel);
  CheckDlgButton(hDlg, ID_TERM_WINDOWWRAP,  Term.fWindowWrap);

  SetDlgItemData(hDlg, ID_TERM_ENTER, Term.szEnter);
  SetDlgItemInt(hDlg, ID_TERM_TABWIDTH, Term.cTabWidth, FALSE);
}

static void near ResetTerminal(hDlg, cTerm)
  HWND hDlg;
  char cTerm;
{
  char buf[8];


  SetDialogTerminal(hDlg, cTerm);

  CheckDlgButton(hDlg, ID_TERM_80, TRUE);
  CheckDlgButton(hDlg, ID_TERM_132, FALSE);
  CheckDlgButton(hDlg, ID_TERM_OFF, TRUE);
  CheckDlgButton(hDlg, ID_TERM_ADDLF, FALSE);
  CheckDlgButton(hDlg, ID_TERM_CR, TRUE);
  CheckDlgButton(hDlg, ID_TERM_CRLF, FALSE);
  CheckDlgButton(hDlg, ID_TERM_NOCOLOR, FALSE);
  CheckDlgButton(hDlg, ID_TERM_FORMFEED, FALSE);
  CheckDlgButton(hDlg, ID_TERM_PRINTEXTENT, FALSE);
  CheckDlgButton(hDlg, ID_TERM_WINDOWWRAP, FALSE);

  buf[0] = '^';
  buf[1] = 'M';
  buf[2] = NULL;
  SetDlgItemText(hDlg, ID_TERM_ENTER, buf);
  SetDlgItemInt(hDlg, ID_TERM_TABWIDTH, 8, FALSE);

  switch (cTerm)
    {
    case TERM_VT102:
      CheckDlgButton(hDlg, ID_TERM_ECHO,     FALSE);
      CheckDlgButton(hDlg, ID_TERM_GRAPHICS, TRUE);   // 00005
      CheckDlgButton(hDlg, ID_TERM_SWAPDEL,  TRUE);
      CheckDlgButton(hDlg, ID_TERM_AUTOWRAP, FALSE);
      break;

    case TERM_VT52:
      CheckDlgButton(hDlg, ID_TERM_ECHO,     FALSE);
      CheckDlgButton(hDlg, ID_TERM_GRAPHICS, TRUE);   // 00005
      CheckDlgButton(hDlg, ID_TERM_SWAPDEL,  TRUE);
      CheckDlgButton(hDlg, ID_TERM_AUTOWRAP, FALSE);
      break;

    case TERM_IBMPC:
      CheckDlgButton(hDlg, ID_TERM_ECHO,     FALSE);
      CheckDlgButton(hDlg, ID_TERM_GRAPHICS, TRUE);   // 00005
      CheckDlgButton(hDlg, ID_TERM_SWAPDEL,  FALSE);
      CheckDlgButton(hDlg, ID_TERM_AUTOWRAP, FALSE);
      break;

    case TERM_IBM3101:
      CheckDlgButton(hDlg, ID_TERM_ECHO,     FALSE);
      CheckDlgButton(hDlg, ID_TERM_SWAPDEL,  FALSE);
      CheckDlgButton(hDlg, ID_TERM_AUTOWRAP, TRUE);
      break;

    case TERM_VIDTEX:
      CheckDlgButton(hDlg, ID_TERM_ECHO,     FALSE);
      CheckDlgButton(hDlg, ID_TERM_GRAPHICS, FALSE);   // 00005
      CheckDlgButton(hDlg, ID_TERM_SWAPDEL,  FALSE);
      CheckDlgButton(hDlg, ID_TERM_AUTOWRAP, FALSE);
      break;
    }
}

static void near SetDialogTerminal(hDlg, cTerm)
  HWND hDlg;
  char cTerm;
{
  switch (cTerm)
    {
    case TERM_VT102:
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CR), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CRLF), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_ENTER), SW_HIDE);
      // dca00038 MKL changed setup.terminal.more.graphics text to
      //	      "Enable graphics mode"
      SetDlgItemText(hDlg, ID_TERM_GRAPHICS, "Enable line drawing characters");

      EnableWindow(GetDlgItem(hDlg, ID_TERM_132), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_PRINTEXTENT), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_ADDLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_CRLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_GRAPHICS), TRUE);  // 00005
      EnableWindow(GetDlgItem(hDlg, ID_TERM_NOCOLOR), TRUE);
      if (fMore)
	EnableWindow(GetDlgItem(hDlg, ID_TERM_AUTOWRAP), TRUE);
      break;

    case TERM_VT52:
      CheckDlgButton(hDlg, ID_TERM_NOCOLOR,  FALSE);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CR), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CRLF), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_ENTER), SW_HIDE);
      // dca00038 MKL
      SetDlgItemText(hDlg, ID_TERM_GRAPHICS, "Enable line drawing characters");

      EnableWindow(GetDlgItem(hDlg, ID_TERM_132), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_PRINTEXTENT), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_ADDLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_CRLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_GRAPHICS), TRUE);  // 00005
      EnableWindow(GetDlgItem(hDlg, ID_TERM_NOCOLOR), FALSE);
      if (fMore)
	EnableWindow(GetDlgItem(hDlg, ID_TERM_AUTOWRAP), TRUE);
      break;

    case TERM_IBMPC:
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CR), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CRLF), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_ENTER), SW_HIDE);
      // dca00038 MKL
      SetDlgItemText(hDlg, ID_TERM_GRAPHICS, "Enable IBM PC graphic characters");

      EnableWindow(GetDlgItem(hDlg, ID_TERM_132), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_PRINTEXTENT), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_ADDLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_CRLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_GRAPHICS), TRUE);  // 00005
      EnableWindow(GetDlgItem(hDlg, ID_TERM_NOCOLOR), TRUE);
      if (fMore)
	EnableWindow(GetDlgItem(hDlg, ID_TERM_AUTOWRAP), FALSE);
      break;

    case TERM_IBM3101:
      CheckDlgButton(hDlg, ID_TERM_NOCOLOR,  FALSE);
      CheckDlgButton(hDlg, ID_TERM_GRAPHICS, FALSE);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CR), SW_HIDE);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CRLF), SW_HIDE);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_ENTER), SW_SHOW);
      // dca00038 MKL
      SetDlgItemText(hDlg, ID_TERM_GRAPHICS, "Enable graphic characters");

      EnableWindow(GetDlgItem(hDlg, ID_TERM_132), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_PRINTEXTENT), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_ADDLF), TRUE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_GRAPHICS), FALSE);	// 00005
      EnableWindow(GetDlgItem(hDlg, ID_TERM_NOCOLOR), FALSE);
      if (fMore)
	EnableWindow(GetDlgItem(hDlg, ID_TERM_AUTOWRAP), TRUE);
      break;

    case TERM_VIDTEX:
      CheckDlgButton(hDlg, ID_TERM_CR, TRUE);
      CheckDlgButton(hDlg, ID_TERM_CRLF, FALSE);
      CheckDlgButton(hDlg, ID_TERM_NOCOLOR,  FALSE);
      // dca00038 MKL
      SetDlgItemText(hDlg, ID_TERM_GRAPHICS, "Enable graphic characters");

      ShowWindow(GetDlgItem(hDlg, ID_TERM_CR), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_CRLF), SW_SHOW);
      ShowWindow(GetDlgItem(hDlg, ID_TERM_ENTER), SW_HIDE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_132), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_PRINTEXTENT), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_ADDLF), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_CRLF), FALSE);
      EnableWindow(GetDlgItem(hDlg, ID_TERM_GRAPHICS), TRUE);  // 00005
      EnableWindow(GetDlgItem(hDlg, ID_TERM_NOCOLOR), FALSE);
      if (fMore)
	EnableWindow(GetDlgItem(hDlg, ID_TERM_AUTOWRAP), FALSE);
      break;
    }
}
