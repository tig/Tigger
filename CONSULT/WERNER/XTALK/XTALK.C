/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   The Main Driver File (XTALK.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This is where it all begins in the wonderful world of windows.  *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/23/89 00020 Make Windows 3.0 compatible.                *
*             1.01 09/28/89 00040 Permit hiding of the xfer dialog window.    *
*	      1.01 10/15/89 00043 Process WM_SPOOLERSTATUS message.	      *
*	      1.01 10/31/89 00052 Add context help to the menus and dialogs.  *
*	      1.01 11/06/89 00055 Modify disconnect message and put in RC.    *
* 1.1 04/02/90 dca00059 MKL added Win3.0 Help.				      *
* 1.1 04/02/90 dca00059 MKL added Win3.0 Icon.				      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>

#define menu_procedures		70
#define menu_commands		75
#define menu_mouse		80
#define menu_keyboard		85
#define dde_info		17

#include <dos.h>
#include <dde.h>
#include "xtalk.h"
#include "library.h"
#include "dialogs.h"
#include "comm.h"
#include "disp.h"
#include "sess.h"
#include "term.h"
#include "prodata.h"
#include "interp.h"
#include "properf.h"
#include "xfer.h"
#include "kermit.h"
#include "global.h"
#include "variable.h"


HANDLE hWndInst;		/* instance handle */
HWND   hWnd;
HMENU  hWndMenu;
HANDLE hAccelTable;
BOOL   fIsActive;

short  sVersion;  // 00020 Make Windows 3.0 compatible.

WORD wSystemState;
WORD wSystem;

/*----------------------------------------------------------------------------*/
/*  Define the global hardware configuration variables			      */
/*----------------------------------------------------------------------------*/
  char fMonoDisplay;
  char fLowResDisplay;
  int  nVertRes;
  int  nHorzRes;
  BYTE byScrollBarHeight;
  BYTE byScrollBarWidth;
  char fMaximized;


/*----------------------------------------------------------------------------*/
/*  Define the various conditions that can exist under XTALK/WND	      */
/*----------------------------------------------------------------------------*/
BOOL bConnected;		/* Connection is active, protect line	    */
BOOL bCaptureOn;		/* Copy all character input/output to disk  */
BOOL bPrinterOn;		/* Copy all character input/output to print */
BOOL bDisplayOn;		/* Echo characters to the screen	    */


char bDebugDDE;
char bDebugEmulation;
char bDebugKermit;
char bDebugScript;
char bDebugXfer;


/*---------------------------------------------------------------------------*/
/* Define multible modeless dialog(s) table for IsDialogMessage loop.	     */
/*---------------------------------------------------------------------------*/
#define MAX_DIALOGS 16		/* Maximum number of Modeless Dialogs	   */
HWND Dialogs[MAX_DIALOGS];
int  DialogCnt = 0;

BOOL near DispatchDialogMessage(LPMSG);

/*----------------------------------------------------------------------------*/
/*  Define the many string constants use in this communication program.       */
/*----------------------------------------------------------------------------*/
/*** string arrays for using stringtable ***/

char szAppName[16];		/* strings */
char szAppTitle[32];		/* strings */
char szIFN[MAX_STR_LEN];	/* illegal filename */
char szFNF[MAX_STR_LEN];	/* file not found	*/
char szREF[MAX_STR_LEN];	/* replace existing file */
char szSCC[MAX_STR_LEN];	/* save current changes */
char szEOF[MAX_STR_LEN];	/* error opening file */
char szECF[MAX_STR_LEN];	/* error creating file */
char szExt[MAX_STR_LEN];

char szXtalkExePath[50];	/* path of xtalk.exe, "xtalk.exe" not included*/
char szHelpPath[60];		/* path of Help file, file name included */

BYTE   cIconMode;
HANDLE hIconFont;
BYTE   cIconType;
long   lIconParm1;
long   lIconParm2;
int    nIconRate;
char   szIconText[8];
int    nIconTextX, nIconTextY;

FARPROC  lpXferDlg;
HWND	 hXferDlg;
BOOL	 bDirection;
char	 *pFTCB;
OFSTRUCT *pFS;


//
// 00052 Define items to support the help message filter.
//
short HelpMessageFilter(short sCode, WORD wParam, LPMSG lpMsg);
FARPROC lpHelpMessageFilter;


extern void XTalkPaint(HWND);
extern void XTalkSize(LONG);

void near _setargv() { };

   static void near TerminalProcessor();


/******************************************************************************/
/*									      */
/*  Routine: WinMain							      */
/*									      */
/*  Remarks: MS Windows calls 'WinMain' to start us up.                       */
/*									      */
/******************************************************************************/

int WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
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


  // 00052 Setup the help message filter.
  lpHelpMessageFilter = MakeProcInstance((FARPROC)HelpMessageFilter, hWndInst);
  SetWindowsHook(WH_MSGFILTER, lpHelpMessageFilter);


  /* Polling messages from event queue (main processing loop here) */

  while (1)
    {
    if (lTimeLastRecv + 2500L > GetCurrentTime() && fIsActive)
      {
      if (!PeekMessage((LPMSG)&msg, NULL, 0, 0, FALSE))
        {
        if (wSystemState == SS_TERMINAL)
          TerminalProcessor();
        else if (wSystemState == SS_PROGRAM)
          InterpreterContinue();
	continue;
	}
      }

    if (GetMessage((LPMSG)&msg, NULL, 0, 0))
      {
      if (!DispatchDialogMessage((LPMSG)&msg))
	{
	if (TranslateAccelerator(hWnd, hAccelTable, (LPMSG)&msg) == 0)
	  {
	  if (msg.message != WM_KEYDOWN || !TerminalVirtualKey((LPMSG)&msg))
	    {
	    TranslateMessage((LPMSG)&msg);
	    DispatchMessage((LPMSG)&msg);
	    }
	  }
	else
	  {
	  SendMessage(hChildInfo, WM_MENUSELECT, 0xFFFF, 0L);
	  }
	}
      }
    else
      break;
    }


  return (int)msg.wParam;
}

static void near TerminalProcessor()
  {
  static short sInside = 0;
  USHORT usBytes;
  char	 buf[256];
  int	 i;


  if (IfSystemStatus(SS_SCROLL) || sInside)
    return;

  sInside++;

  if (hDemoFile != -1 && !fDemoPause && !(wSystem & SS_XFER))
    {
    if (Sess.szNetID[2] == '*')
      {
      if (DosRead(hDemoFile, buf, 1, &usBytes))
	i = 0;
      else if (usBytes == 0)
	i = 0;
      i = usBytes;
      if (i == 0)
	CmdPlayback();

      fDemoPause = TRUE;
      }
    else if (DosRead(hDemoFile, buf, 64, &usBytes))
      {
      CmdPlayback();
      i = 0;
      }
    else if (usBytes == 0)
      {
      CmdPlayback();
      i = 0;
      }
    else
      {
      lTimeLastRecv = GetCurrentTime();
      i = usBytes;
      }
    }
  else
    {
    for (i = 0; i < sizeof(buf); i++)
      {
      register int j;
      if ((j = (int)(CommGet())) == -1)
	break;
      buf[i] = (char)j;
      }
    }

  if (i)
    {
    SendMessage(hChildTerminal, UM_DATA, i,(DWORD)((LPSTR)buf));
    nCommCount += i;
    }

  if (Sess.szNetID[1] == '*')
    DosWrite(3, buf, i, &usBytes);

  sInside--;
}


long WndMainProc(hWnd, message, wParam, lParam )
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
  HANDLE hIcon;
  LONG result;


  switch (message)
    {
    case WM_SYSCOMMAND:
      switch (wParam)
	{
        case CMD_RUNAPP:
          CmdRunApp();
	  break;

	// dca00060 MKL added Win3.0 Icon
	case SC_MINIMIZE:
	  if (sVersion >=3) {
             result = DefWindowProc(hWnd, message, wParam, lParam);
	     IconTextChanged(NULL);
	     return (result);
	  }
          else return DefWindowProc(hWnd, message, wParam, lParam);
	  break;
	case SC_MAXIMIZE:
	case SC_RESTORE:
	case 0XF122:	/* get this if double-click on text protion of icon */
	  if (sVersion >=3) UpdateTitle();
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
          break;
        }
      break;

    case WM_ACTIVATE:
      fIsActive = wParam;
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;

    case WM_COMMAND:
      if (wParam >= CMD_MENUMACRO1 && wParam <= CMD_MENUMACRO48)
        {
        ActivateMacro(wParam - CMD_MENUMACRO1);
        break;
        }

      switch (wParam)
	{
	case CMD_NEW:
          TermWriteClear();
	  CmdNew();
	  break;

	case CMD_OPEN:
          TermWriteClear();
	  CmdOpen();
	  break;

	case CMD_SAVE:
	  CmdSave();
	  break;

	case CMD_SAVEAS:
	  CmdSaveAs();
	  break;

	case CMD_CAPTURE:
          CmdCapture(0, 0);
	  break;

	case CMD_UPLOAD:
	  i = XferASCIIUploadName(NULL);

	  if (i && i != ERRXFR_LOCALCANCEL && i != ERRSYS_OPENFAULT)
	    GeneralError(i);
	  break;

	case CMD_PAGESETUP:
	  CmdPageSetup();
	  break;

	case CMD_PRINT:
	  TogglePrinter();
	  break;

	case CMD_PRINTSETUP:
	  CmdPrintSetup();
	  break;

	case CMD_SEND:
	  CmdSend();
	  break;

	case CMD_RECEIVE:
	  CmdReceive();
	  break;

	case CMD_EXIT:
          if (bConnected)
            {
	    // 00055 Change disconnect message.
	    if (TellUserMessageStr(IDS_ERRDLG_DISCONNECT1, NULL) != IDYES)
              return FALSE;
            }
	  CmdExit();
	  break;

	case CMD_COPY:
          CmdCopy();
	  break;

	case CMD_COPYFILE:
	  CmdCopyFile();
	  break;

	case CMD_COPYCAPTURE:
	  CmdCopyCapture();
	  break;

	case CMD_COPYNOTES:
	  CmdCopyNotes();
	  break;

	case CMD_COPYPRINTER:
	  CmdCopyPrinter();
	  break;

	case CMD_COPYSEND:
	  CmdCopySend();
	  break;

	case CMD_PASTE:
	  CmdPaste();
	  break;

	case CMD_PASTEUPLOAD:
	  CmdPasteUpload();
	  break;

	case CMD_SCROLL:
	  VssScrollToggle();
	  break;

	case CMD_CONNECT:
          if (bConnected)
	    {
	    // 00055 Change disconnect message.
	    if (AskUserMessageStr(IDS_ERRDLG_DISCONNECT2, NULL) != IDYES)
              break;
            if (!bConnected)
              break;
            }
          else
            TermWriteClear();
	  CmdConnect();
	  break;

	case CMD_DIAL:
          TermWriteClear();
	  CmdDial();
	  break;

	case CMD_EXECUTE:
	  CmdExecute();
	  break;

	case CMD_NOTES:
          CmdNotes();
	  break;

	case CMD_KERMIT:
	  CmdKermit();
	  break;

	case CMD_PLAYBACK:
	  CmdPlayback();
	  break;

	case CMD_BREAK:
	  CmdBreak();
	  break;

        case CMD_RESET:
          SendMessage(hChildTerminal, UM_RESET, 0, 0L);
          break;

	case CMD_SESSION:
	  CmdSession();
	  break;

	case CMD_COMMUNICATIONS:
	  CmdCommunications();
	  break;

	case CMD_MODEM:
	  CmdModem();
	  break;

	case CMD_TERMINAL:
	  CmdTerminal();
	  break;

	case CMD_PROTOCOL:
	  CmdProtocol();
	  break;

	case CMD_ASCII:
	  CmdAscii();
	  break;

	case CMD_MACROS:
	  CmdMacros();
	  break;

	case CMD_DISPLAY:
	  CmdDisplay();
	  break;

	case CMD_FONTS:
	  CmdFonts();
	  break;

        case CMD_WINDOW_XFER:
          HideFileTransfer();
          break;

        case CMD_WINDOW_NOTES:
          ToggleNotes();
          break;

	case CMD_SYSTEM:
	  CmdSystem();
	  break;

	case CMD_ABOUT:
	  CmdAbout();
	  break;

	// dca00059 MKL added Win3.0 Help
        case CMD_INDEX:
	  if (sVersion < 3) CmdHelpIndex();
	  else WinHelp(hWnd, szHelpPath, HELP_INDEX, 0L);
          break;
	case CMD_HELP_HELP:
	  WinHelp(hWnd, NULL, HELP_HELPONHELP, 0L);
	  break;

	case CMD_HELP_PROC:
	  WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) menu_procedures);
	  break;

	case CMD_HELP_CMMD:
	  WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) menu_commands);
	  break;

	case CMD_HELP_MOUSE:
	  WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) menu_mouse);
	  break;

	case CMD_HELP_KBD:
	  WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) menu_keyboard);
	  break;

	case CMD_HELP_DDE:
	  WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) dde_info);
	  break;

	default:
	  return DefWindowProc(hWnd, message, wParam, lParam);
	}
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

    case WM_SHOWWINDOW:
      if (wParam)
	{
	if (hXferDlg)
	  ShowWindow(hXferDlg, SW_SHOW);
	}
      else
	{
	if (hXferDlg)
	  ShowWindow(hXferDlg, SW_HIDE);
	}
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;

    // 00043 Process WM_SPOOLERSTATUS message.
    case WM_SPOOLERSTATUS:
      if (lParam == 0)
	{
	if (bPrinterOn)
	  CmdPrint();
	}
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

      CommDataStatus();
      if (hChildInfo)
	SendMessage(hChildInfo, WM_TIMER, 0, 0L);
      if (hChildStatus)
	SendMessage(hChildStatus, WM_TIMER, 0, 0L);

      i = 200;
      switch (wSystemState)
	{
	case SS_NULL:
	  break;

	case SS_TERMINAL:
	  TerminalProcessor();
	  break;

	case SS_PROGRAM:
	  InterpreterContinue();
	  i = 100;
	  break;

	case SS_PROCESS:
          if (hXferDlg)
            SendMessage(hXferDlg, WM_TIMER, 0, 0L);
	  if (nKermitMode == KM_IDLE && IfSystemStatus(SS_SCRIPT))
	    InterpreterContinue();
	  break;

	case SS_DIALING:
	  if (hXferDlg)
	    SendMessage(hXferDlg, WM_TIMER, 0, 0L);
	  break;
	}

      if (fMouseDown && (nMouseAtX != 0 || nMouseAtY != 0))
	PostMessage(hChildTerminal, WM_MOUSEMOVE, 0, MAKELONG(nMouseAtX,nMouseAtY));

      SetTimer(hWnd, COMM_TIMER, i, NULL);
      break;

    case WM_SETFOCUS:
      {
      register i;


      i = GetKeyState(0x91) & 0x01;
      if (i && !IfSystemStatus(SS_SCROLL))
        SetScrollLock(hWnd, FALSE);
      if (!i && IfSystemStatus(SS_SCROLL))
        SetScrollLock(hWnd, TRUE);

      if (hChildInfo)
	SendMessage(hChildInfo, UM_KEYSTATE, 0, 0L);

      if (hKermitChild)
	SetFocus(hKermitChild);
      else if (Disp.fShowInput && hChildInput)
	SetFocus(hChildInput);
      else if (hChildTerminal)
	SetFocus(hChildTerminal);
      }
      break;

    case WM_SIZE:
      if (wParam == SIZEFULLSCREEN)
        fMaximized = 1;
      else if (wParam == SIZENORMAL)
        fMaximized = 0;

      XTalkSize(lParam);
      break;

    case WM_PAINT:
      XTalkPaint(hWnd);
      /* MKL
      if (sVersion < 3) XTalkPaint(hWnd);
      else {
	if (wSystem == SS_INIT) XTalkPaint(hWnd);
        else {
	   hIcon = LoadIcon(hWndInst, MAKEINTRESOURCE(IDI_ICON));
	   return DefWindowProc(hWnd, message, wParam, lParam);
	}
      }
      */
      break;

    case WM_MENUSELECT:
      SendMessage(hChildInfo, message, wParam, lParam);
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;

    case WM_CLOSE:
      if (bConnected)
	{
	// 00055 Change disconnect message.
	if (TellUserMessageStr(IDS_ERRDLG_DISCONNECT1, NULL) != IDYES)
	  return FALSE;
	}

      if (DataSaveFirst() == IDCANCEL)
	return FALSE;

      ProgramAbort();
      break;

    case WM_DESTROY:
      // dca00059 MKL added Win3.0 Help
      if (sVersion >= 3) WinHelp(hWnd, szHelpPath, HELP_QUIT, 0L);
      ProgramWrapup();
      PostQuitMessage(0);
      break;

    case WM_QUERYENDSESSION:
      if (DataSaveFirst() == IDCANCEL)
	return FALSE;

      if (bConnected)
	{
	// 00055 Change disconnect message.
	if (TellUserMessageStr(IDS_ERRDLG_DISCONNECT1, NULL) != IDYES)
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

BOOL near DispatchDialogMessage(msg)
  LPMSG msg;
{
  register i;

  if (DialogCnt != 0)
    {
    for (i = 0; i < DialogCnt; i++)
      {
      if (IsDialogMessage(Dialogs[i], msg))
	return TRUE;
      }
    }

  return FALSE;
}


//
// 00052 HelpMessageFilter
//
//   Define a message filter to look for the F1 key so we can display the help
// dialog (if not already displayed).
//
short HelpMessageFilter(short sCode, WORD wParam, LPMSG lpMsg)
{
  switch (sCode) {
    case MSGF_DIALOGBOX:
      if (lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F1) {
	if (wHelpDialog) {
	   // dca00059 MKL added Win3.0 Help
	   if (sVersion < 3) CmdHelpTopic(wHelpDialog);
	   else WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) wHelpDialog);
	}
      }
      break;

    case MSGF_MENU:
      if (lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F1) {
	if (wHelpMenu) {
	   // dca00059 MKL added Win3.0 Help
	   if (sVersion < 3) CmdHelpTopic(wHelpMenu);
	   else {
	      if (wHelpMenu==HELP_ITEM_SCREENFILE)
		 wHelpMenu=HELP_ITEM_COPYFILE;
	      else if (wHelpMenu==HELP_ITEM_SCREENNOTES)
		 wHelpMenu=HELP_ITEM_COPYNOTES;
	      else if (wHelpMenu==HELP_ITEM_SCREENCAPTURE)
		 wHelpMenu=HELP_ITEM_COPYCAPTURE;
	      else if (wHelpMenu==HELP_ITEM_SCREENPRINTER)
		 wHelpMenu=HELP_ITEM_COPYPRINTER;
	      WinHelp(hWnd, szHelpPath, HELP_CONTEXT, (DWORD) wHelpMenu);
	   }
	}
      }
      break;
  }

  return (0);
}
