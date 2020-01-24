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
#include <dos.h>
#include <dde.h>
#include "lexis.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "connect.h"
#include "session.h"
#include "term.h"
#include "data.h"
#include "record.h"
#include "robot.h"


HANDLE hWndInst;		/* instance handle */
HWND   hWnd;
HMENU  hWndMenu;
HANDLE hAccelTable;
HANDLE hProcessDlg = 0;

WORD   wSystem;
WORD   wSystemState;

/*----------------------------------------------------------------------------*/
/*  Define the global hardware configuration variables			      */
/*----------------------------------------------------------------------------*/
  char fMonoDisplay;
  char fLowResDisplay;
  int  nVertRes;
  int  nHorzRes;
  BYTE byScrollBarHeight;
  BYTE byScrollBarWidth;


/*----------------------------------------------------------------------------*/
/*  Define the various conditions that can exist under WinLexis/WND              */
/*----------------------------------------------------------------------------*/
BOOL bPrinterOn;		/* Copy all character input/output to print */
int  nProcessing;               /* Copy all character input/output to print */


/*----------------------------------------------------------------------------*/
/*  Define the many string constants use in this communication program.       */
/*----------------------------------------------------------------------------*/
/*** string arrays for using stringtable ***/

char szAppName[16];		/* strings */
char szAppTitle[40];            /* strings */
char szIFN[MAX_STR_LEN];	/* illegal filename */
char szFNF[MAX_STR_LEN];	/* file not found	*/
char szREF[MAX_STR_LEN];	/* replace existing file */
char szSCC[MAX_STR_LEN];	/* save current changes */
char szEOF[MAX_STR_LEN];	/* error opening file */
char szECF[MAX_STR_LEN];	/* error creating file */
char szExt[MAX_STR_LEN];



extern void WinLexisPaint(HWND);
extern void WinLexisSize(LONG);

void near _setargv() { };

   static void near pascal TerminalProcessor();


/******************************************************************************/
/*									      */
/*  Routine: WinMain							      */
/*									      */
/*  Remarks: MS Windows calls 'WinMain' to start us up.                       */
/*									      */
/******************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
  HANDLE hInstance;
  HANDLE hPrevInstance;
  LPSTR  lpszCmdLine;
  int	 cmdShow;
{
  MSG  msg;


  /* Perform one of two initialization procedures depending if this is the
     first copy or not.  Faster init for the second (and so on) copies	   */

  if (hPrevInstance == FALSE)
    {
    if (FirstCopyInit(hInstance) == FALSE)
      return FALSE;
    }
  else
    {
    if (AnotherCopyInit(hInstance,hPrevInstance) == FALSE)
      return FALSE;
    }


  /* Perform common initialization procedure */

  if (InstanceInit(hInstance, lpszCmdLine, cmdShow) == FALSE)
    return FALSE;


  /* Polling messages from event queue (main processing loop here) */

  nProcessing = 0;
  while (1)
    {
    if (nProcessing == 0)
      {
      if (!fSessProcess && ulCommLastRecv + 2500L > GetCurrentTime())
        {
        if (!PeekMessage((LPMSG)&msg, NULL, 0, 0, FALSE))
          {
          BYTE abBuffer[64];
          register i;

          nProcessing++;
          if (i = CommReadData(abBuffer, sizeof(abBuffer)))
            TermWriteData(abBuffer, i, FALSE);
          nProcessing--;
          continue;
          }
        }
      }

    if (GetMessage(&msg, NULL, 0, 0))
      {
      if (!(hProcessDlg != NULL && IsDialogMessage(hProcessDlg, &msg)))
	{
	if (TranslateAccelerator(hWnd, hAccelTable, (LPMSG)&msg) == 0)
	  {
          if (msg.message != WM_KEYDOWN || !TerminalVirtualKey((LPMSG)&msg))
	    {
	    TranslateMessage((LPMSG)&msg);
	    DispatchMessage((LPMSG)&msg);
	    }
	  }
	}
      }
    else
      break;
    }

  return (int)msg.wParam;
}


long MainWndProc(hWnd, message, wParam, lParam )
  HWND hWnd;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  FARPROC lpDlg;
  PAINTSTRUCT ps;
  RECT Rect;
  int  xClient, yClient, nXPos, nYPos;
  int  nTop, nBot;
  int  i, x, y;
  char tmp[8];


  switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
	{
	case CMD_NEW:
	  CmdNew();
	  break;

	case CMD_OPEN:
          CmdOpen();
	  break;

        case CMD_CLOSE:
          CmdClose();
	  break;

	case CMD_PRINT:
          CmdPrint();
	  break;

        case CMD_RECORDING:
          CmdRecord();
          break;

        case CMD_DIARY:
          CmdDiary();
          break;

        case CMD_PRINTER:
          CmdPrinter();
          break;

        case CMD_SETUP:
          CmdSetup();
	  break;

	case CMD_PAGESETUP:
	  CmdPageSetup();
	  break;

	case CMD_PRINTSETUP:
	  CmdPrintSetup();
	  break;

        case CMD_HYPERROM:
          CmdHyperRom();
	  break;

        case CMD_SIGNON:
          CmdSignon();
          break;

	case CMD_EXIT:
	  CmdExit();
	  break;

	case CMD_COPY:
	  CmdCopy();
	  break;

        case CMD_COPYDIARY:
          CmdCopyDiary();
	  break;

	case CMD_PASTE:
	  CmdPaste();
	  break;

        case CMD_NEWSEARCH:
          SessCanCommand(COMMAND_NEWSEARCH);
          break;

        case CMD_SEARCHMODIFY:
          CmdSearchModify();
          break;

        case CMD_SEARCHLEVEL:
          CmdSearchLevel();
          break;

        case CMD_LIBRARY:
          SessCanCommand(COMMAND_CHANGELIBRARY);
          break;

        case CMD_FILE:
          SessCanCommand(COMMAND_CHANGEFILE);
          break;

        case CMD_CLIENT:
	  SendMessage(hChildStatus, UM_CLIENT, 0, 0L);
          SessCanCommand(COMMAND_NEWCLIENT);
          break;

        case CMD_ECLIPSE:
          SessCanCommand(COMMAND_ECLIPSE);
          break;

        case CMD_ECLIPSERECALL:
          SessCanCommand(COMMAND_ECLIPSERECALL);
          break;

        case CMD_SEARCHDISPLAY:
          SessCanCommand(COMMAND_SEARCHDISPLAY);
          break;

        case CMD_STOP:
          SessCanCommand(COMMAND_STOP);
          break;

        case CMD_NEXTPAGE:
          CmdNextPage(TRUE);
          break;

        case CMD_PREVPAGE:
          CmdPrevPage(TRUE);
          break;

        case CMD_NEXTDOC:
          CmdNextDoc(TRUE);
          break;

        case CMD_PREVDOC:
          CmdPrevDoc(TRUE);
          break;

        case CMD_GOTOPAGE:
          CmdGotoPage();
          break;

        case CMD_GOTODOC:
          CmdGotoDoc();
          break;

        case CMD_FIRSTPAGE:
          SessCanCommand(COMMAND_FIRSTPAGE);
          break;

        case CMD_FIRSTDOC:
          SessCanCommand(COMMAND_FIRSTDOC);
          break;

        case CMD_KWIC:
          SessCanCommand(COMMAND_KWIC);
          break;

        case CMD_FULL:
          SessCanCommand(COMMAND_FULL);
          break;

        case CMD_CITE:
          SessCanCommand(COMMAND_CITE);
          break;

        case CMD_VARKWIC:
          CmdVarKwic();
          break;

        case CMD_SEGMENTS:
          SessCanCommand(COMMAND_SEGMENTS);
          break;

        case CMD_SEQUENCE:
          SessCanCommand(COMMAND_SEQUENCE);
          break;

        case CMD_ENTERSTAR:
	  SessStrCommand(COMMAND_STAR);
          break;

        case CMD_EXITSTAR:
	  SessCanCommand(COMMAND_XSTAR);
          break;

	case CMD_FONTS:
	  CmdFonts();
	  break;

        case CMD_AUTOCITE:
          CmdAutoCite();
          break;

        case CMD_SHEPARD:
          CmdShepard();
          break;

        case CMD_LEXSTAT:
          CmdLexstat();
          break;

        case CMD_LEXSEE:
          CmdLexsee();
          break;

        case CMD_BROWSE:
          SessCanCommand(COMMAND_BROWSE);
          break;

        case CMD_RESUME:
          CmdResume();
          break;

        case CMD_SELECTSERVICE:
          SessCanCommand(COMMAND_SELECTSERVICE);
          break;

        case CMD_EXITSERVICE:
          SessCanCommand(COMMAND_EXITSERVICE);
          break;

        case CMD_PRINTSCREEN:
          SessCanCommand(COMMAND_PRINTSCREEN);
          break;

        case CMD_PRINTDOC:
          if (fDataFastPrint)
            {
            if (bWorkFastPrint)
              SessCanCommand(COMMAND_FASTPRINT2);
            else
              SessCanCommand(COMMAND_FASTPRINT1);
            bWorkFastPrint = TRUE;
            }
          else
            SessCanCommand(COMMAND_PRINTDOC);
          break;

        case CMD_PRINTALL:
          CmdPrintAll();
          break;

        case CMD_TIME:
          SessCanCommand(COMMAND_TIME);
          break;

        case CMD_REQUEST:
          SessCanCommand(COMMAND_REQUEST);
          break;

        case CMD_PAGES:
          SessCanCommand(COMMAND_PAGES);
          break;

        case CMD_DEBUT:
          SessCanCommand(COMMAND_DEBUT);
          break;

        case CMD_GENERAL:
          CmdCommands();
          break;

        case CMD_TUTORIAL:
          SessCanCommand(COMMAND_TUTOR);
          break;

        case CMD_LEXISHELP:
          SessCanCommand(COMMAND_HELP);
          break;

	case CMD_ABOUT:
	  CmdAbout();
	  break;

	default:
	  return DefWindowProc(hWnd, message, wParam, lParam);
	}
      break;

    case WM_INITMENU:
      if (sActiveMacro)
        {
        sActiveMacro = FALSE;
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        }
      break;

    case WM_INITMENUPOPUP:
      if (sActiveMacro)
        {
        sActiveMacro = FALSE;
        SendMessage(hChildMacro, UM_BUTTON, FALSE, 0L);
        }
      if (HIWORD(lParam) == 0)
        MenuPopupInit(LOWORD(lParam));
      break;

    case WM_DDE_INITIATE:
    case WM_DDE_TERMINATE:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_REQUEST:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
      ProcessDDE(hWnd, message, wParam, lParam);
      break;

    case WM_TIMER:
      KillTimer(hWnd, wParam);

      if (wSystem & SS_ABORT)
	{
	if (nDDESessions == 0)
	  {
	  DestroyWindow(hWnd);
	  break;
	  }
	}

      SendMessage(hChildInfo, WM_TIMER, wParam, lParam);

      if (nProcessing == 0)
        {
        nProcessing++;

        if (hProcessDlg)
          SendMessage(hProcessDlg, WM_TIMER, wParam, lParam);
        else if (fSessProcess)
          SessTaskProcess();
        else
          {
          BYTE abBuffer[64];
          register i;

          if (i = CommReadData(abBuffer, sizeof(abBuffer)))
            TermWriteData(abBuffer, i, FALSE);
          }

        nProcessing--;
        }

      SetTimer(hWnd, COMM_TIMER, 250, NULL);
      break;

    case WM_SETFOCUS:
      if (hChildInfo)
	SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);

      SetFocus(hChildTerminal);
      break;

    case WM_SIZE:
      WinLexisSize(lParam);
      break;

    case WM_PAINT:
      WinLexisPaint(hWnd);
      break;

    case WM_MENUSELECT:
      SendMessage(hChildInfo, message, wParam, lParam);
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;

    case WM_CLOSE:
      CmdExit();
      break;

    case WM_DESTROY:
      ProgramWrapup();
      PostQuitMessage(0);
      break;

    case WM_QUERYENDSESSION:
      if (fSessOnline)
	{
        if (MessageBox(hWnd, "Sure you want to disconnect!", szAppName,
		     MB_YESNO | MB_ICONQUESTION) != IDYES)
	  return FALSE;
	}

      ProgramWrapup();
      return TRUE;
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }

  return(0L);
}
