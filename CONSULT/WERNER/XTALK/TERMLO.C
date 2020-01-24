/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Terminal Change Module (TERMLO.C)                               *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the logic to initialize the terminal       *
*             emulation and to change the emulation once we have started.     *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 10/26/89 00049 When a new terminal is selected from the    *
*				    NEW.FILE dialog or the script language    *
*				    reset the terminal params to defaults.    *
*             1.01 01/19/90 00091 Update screen display.                      *
* 1.1  4/24/90 dca00093 MKL changed xtalk to display VT102 VT52 without dash  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include "xtalk.h"
#include "library.h"
#include "sess.h"
#include "term.h"
#include "comm.h"
#include "font.h"
#include "prodata.h"
#include "properf.h"
#include "dialogs.h"
#include "interp.h"


BOOL CreateTerminalChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  char szClassName[32];


  sTermFileName[0] = NULL;
  sTermWorkName[0] = NULL;
  nTermWorkInx = NULL;

  fMouseDown = FALSE;
  fSelectedArea = FALSE;
  nSelectX1 = nSelectX2 = 0;
  nSelectY1 = nSelectY2 = 0;

  fMouseCursor = FALSE;


  InitVideo();

  LoadString(hWndInst, IDS_WNDCLASS_TERM, szClassName, sizeof(szClassName));

  hWnd = CreateWindow(szClassName,
		      NULL,
		      WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
		      0,
		      0,
		      0,
		      0,
		      hParent,
		      (HMENU)NULL,
		      (HANDLE)hWndInst,
		      NULL
		      );

  nCurEmulator	 = TERM_VT102;
  bTermAutoPrint = FALSE;
  bTermCntlPrint = FALSE;

  if (hWnd)
    {
    hChildTerminal = hWnd;
    hTerminalDC    = GetDC(hWnd);
    VT102Init();
    UpdateFont();
    return TRUE;
    }
  else
    {
    hChildTerminal = NULL;
    hTerminalDC    = NULL;
    return FALSE;
    }
}


BOOL DestroyTerminalChild()
{
  if (hChildTerminal)
    {
    switch (nCurEmulator)
      {
      case TERM_VT102:
	VT102Term();
	break;

      case TERM_VT52:
	VT52Term();
	break;

      case TERM_IBM3101:
	IBM3101Term();
	break;

      case TERM_IBMPC:
	PCIBMTerm();
	break;

      case TERM_VIDTEX:
	VidtexTerm();
	break;
      }

    DestroyWindow(hChildTerminal);
    hChildTerminal = NULL;
    hTerminalDC    = NULL;
    }

  return TRUE;
}

extern char fPauseOn;

void UpdateTerminal()
{
  char buf[32];


  fCapsLock = FALSE;
  fNumsLock = FALSE;
  fPauseOn  = FALSE;
  SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);


  if (Term.cTerminal == (char)nCurEmulator)
    {
    SendMessage(hChildTerminal, UM_RESET, 0, 0L);
    return;
    }

  switch (nCurEmulator)
    {
    case TERM_VT102:
      VT102Term();
      break;

    case TERM_VT52:
      VT52Term();
      break;

    case TERM_IBM3101:
      IBM3101Term();
      break;

    case TERM_IBMPC:
      PCIBMTerm();
      break;

    case TERM_VIDTEX:
      VidtexTerm();
      break;
    }


  nCurEmulator = Term.cTerminal;

  switch (Term.cTerminal)
    {
    case TERM_VT102:
      SetWindowLong(hChildTerminal, GWL_WNDPROC, (long)WndVT102Proc);
      VT102Init();
      break;

    case TERM_VT52:
      SetWindowLong(hChildTerminal, GWL_WNDPROC, (long)WndVT52Proc);
      VT52Init();
      break;

    case TERM_IBM3101:
      SetWindowLong(hChildTerminal, GWL_WNDPROC, (long)WndIBM3101Proc);
      IBM3101Init();
      break;

    case TERM_IBMPC:
      SetWindowLong(hChildTerminal, GWL_WNDPROC, (long)WndPCIBMProc);
      PCIBMInit();
      break;

    case TERM_VIDTEX:
      SetWindowLong(hChildTerminal, GWL_WNDPROC, (long)WndVidtexProc);
      VidtexInit();
      break;
    }

  InvalidateRect(hChildTerminal, NULL, TRUE);
  UpdateWindow(hChildTerminal);

  LoadString(hWndInst, IDS_EMULATOR_FIRST+Term.cTerminal, buf, sizeof(buf));
  // dca00093 MKL
  if (strcmp(buf, "VT-102 ")==0) strcpy(buf, "VT102  ");
  else if (strcmp(buf, "VT-52  ")==0) strcpy(buf, " VT52  ");
  ChangeMenuText(CMD_TERMINAL, buf);
}

//
//  UpdateNewTerminal
//
//    This function is called when a new terminal is select by the user.  It
//  will set all the terminal parameters to default values.
//
void UpdateNewTerminal()
  {
  Term.nColumns        = 80;
  Term.cCurShape       = TERM_CURSOR_BLOCK;
  Term.fLfAuto	       = FALSE;
  Term.fNewLine        = FALSE;
  Term.fAutoWrap       = FALSE;
  Term.fEcho	       = FALSE;
  Term.fNoColor        = FALSE;
  Term.fFormFeed       = FALSE;
  Term.fPrintExtent    = FALSE;
  Term.fSwapDel        = TRUE;
  Term.fWindowWrap     = FALSE;
  Term.szEnter[0]      = 1;
  Term.szEnter[1]      = ASC_CR;
  Term.cTabWidth       = 8;
  Term.szAnswerback[0] = 0;
  Sess.fGraphics       = FALSE;

  switch (Term.cTerminal)
    {
    case TERM_VT102:
      Sess.fGraphics = TRUE;
      break;

    case TERM_VT52:
      Sess.fGraphics = TRUE;
      break;

    case TERM_IBMPC:
      Term.fSwapDel  = FALSE;
      Term.fNoColor  = TRUE;
      Sess.fGraphics = TRUE;
      break;

    case TERM_IBM3101:
      Term.fSwapDel = FALSE;
      Term.fAutoWrap = TRUE;
      break;

    case TERM_VIDTEX:
      Term.fSwapDel = FALSE;
      break;
    }

  // 00091 Update screen display.
  UpdateTerminal();
  }


void UpdateTerminalFont()
{
  SendMessage(hChildTerminal, UM_NEWFONT, 0, 0L);
  InvalidateRect(hChildTerminal, NULL, TRUE);
  VidUpdateSize();
  if (fTermFocusActive)
    VidSetCaretPos();
}

void SelectTerminal(cTerm)
  char cTerm;
{
  Term.cTerminal = cTerm;

  switch (cTerm)
    {
    case TERM_VT102:
      Term.fEcho	= FALSE;
      Term.fSwapDel	= TRUE;
      Term.fAutoWrap    = TRUE;
      break;

    case TERM_VT52:
      Term.fEcho	= FALSE;
      Term.fSwapDel	= TRUE;
      Term.fAutoWrap    = TRUE;
      break;

    case TERM_IBMPC:
      Term.fEcho	= FALSE;
      Term.fSwapDel	= FALSE;
      Term.fAutoWrap    = FALSE;
      break;

    case TERM_IBM3101:
      Term.fEcho	= FALSE;
      Term.fSwapDel	= FALSE;
      Term.fAutoWrap    = TRUE;
      break;

    case TERM_VIDTEX:
      Term.fEcho        = FALSE;
      Term.fSwapDel	= FALSE;
      Term.fAutoWrap    = FALSE;
      break;
    }

  Term.nColumns     = 80;
  Term.cCurShape    = TERM_CURSOR_BAR;
  Term.fLfAuto	    = FALSE;
  Term.fNewLine     = FALSE;
  Term.fFormFeed    = FALSE;
  Term.fPrintExtent = FALSE;
  Term.fWindowWrap  = FALSE;
  Term.szEnter[0]   = 1;
  Term.szEnter[1]   = ASC_CR;
  Term.cTabWidth    = 8;

  UpdateTerminal();
}


BOOL InitTerminal()
{
  NewTerminalData();

  return (TRUE);
}

void NewTerminalData()
{
  Term.cTerminal       = TERM_VT102;
  Term.nColumns        = 80;
  Term.cCurShape       = TERM_CURSOR_BLOCK;
  Term.fLfAuto	       = FALSE;
  Term.fNewLine        = FALSE;
  Term.fAutoWrap       = FALSE;
  Term.fEcho	       = FALSE;
  Term.fNoColor        = FALSE;
  Term.fFormFeed       = FALSE;
  Term.fPrintExtent    = FALSE;
  Term.fSwapDel        = TRUE;
  Term.fWindowWrap     = FALSE;
  Term.szEnter[0]      = 1;
  Term.szEnter[1]      = ASC_CR;
  Term.cTabWidth       = 8;
  Term.szAnswerback[0] = 0;
}
