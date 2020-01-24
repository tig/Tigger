/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Term Support Module (TERMLO.C)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include "lexis.h"
#include "library.h"
#include "term.h"
#include "comm.h"
#include "font.h"
#include "data.h"
#include "dialogs.h"


BOOL CreateTerminalChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  char szClassName[32];


  fMouseDown = FALSE;
  fSelectedArea = FALSE;
  nSelectX1 = nSelectX2 = 0;
  nSelectY1 = nSelectY2 = 0;


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

  bTermAutoPrint = FALSE;
  bTermCntlPrint = FALSE;

  if (hWnd)
    {
    hChildTerminal = hWnd;
    hTerminalDC    = GetDC(hWnd);
    TermInit();
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
    TermTerm();

    DestroyWindow(hChildTerminal);
    hChildTerminal = NULL;
    hTerminalDC    = NULL;
    }

  return TRUE;
}

extern char fPauseOn;


void UpdateTerminalFont()
{
  SendMessage(hChildTerminal, UM_NEWFONT, 0, 0L);
  InvalidateRect(hChildTerminal, NULL, TRUE);
  VidUpdateSize();
  if (fTermFocusActive)
    VidSetCaretPos();
}


BOOL InitTerminal()
{
  NewTerminalData();

  return (TRUE);
}

void NewTerminalData()
{
  Term.nColumns        = 80;
  Term.cCurShape       = TERM_CURSOR_BLOCK;
  Term.fLfAuto	       = FALSE;
  Term.fNewLine        = FALSE;
  Term.fAutoWrap       = FALSE;
  Term.fEcho	       = FALSE;
  Term.fEightBit       = FALSE;
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
