/*****************************************************************************\
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Init Module (PROINIT.C)                                         *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module initializes the system and get us running.          *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 08/03/89 00004 Auto open capture and printer.	      *
*             1.01 08/23/89 00020 Make Windows 3.0 compatible.                *
*             1.01 09/28/89 00038 Correctly process 'WIN :XTALK' command.     *
*	      1.01 10/20/89 00048 Prevent windows displaying disk errors.     *
*	      1.01 11/07/89 00058 Reset info font for :XTALK startup.	      *
*	1.1 04/02/90 dca00059  MKL added Windows 3.0 Help		      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <fcntl.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "disp.h"
#include "font.h"
#include "macro.h"
#include "term.h"
#include "sess.h"
#include "xfer.h"
#include "interp.h"
#include "prodata.h"
#include "properf.h"
#include "dialogs.h"
#include "global.h"
#include "variable.h"
#include "ctlcolor.h"

// dca00074 MKL added new icon
#define IDI_ICON2	31

/* message box strings loaded in sample.c from the stringtable */
extern char szIFN[], szFNF[], szREF[], szSCC[], szEOF[], szECF[], szExt[];
extern OFSTRUCT DataFileStruct;
extern char szXtalkExePath[];
extern char szHelpPath[];


/******************************************************************************/
/*									      */
/*  Routine: FirstCopyInit						      */
/*									      */
/*  Remarks: Initialize code for the first copy of XTALK in memory.	      */
/*									      */
/******************************************************************************/

BOOL FirstCopyInit(hInstance)
  HANDLE hInstance;
{
  WNDCLASS WndClass;
  char	   szClassName[32], buf[32];

  // Get the major version of Windows.
  sVersion = LOBYTE(GetVersion());

  /* Load strings from resource */

  LoadString(hInstance, IDS_APPNAME,  szAppName, 16);
  LoadString(hInstance, IDS_APPTITLE, szAppTitle, 32);

  LoadString(hInstance, IDS_IFN, szIFN, MAX_STR_LEN); /* illegal filename */
  LoadString(hInstance, IDS_FNF, szFNF, MAX_STR_LEN); /* file not found     */
  LoadString(hInstance, IDS_REF, szREF, MAX_STR_LEN); /* replace existing file */
  LoadString(hInstance, IDS_SCC, szSCC, MAX_STR_LEN); /* save current changes */
  LoadString(hInstance, IDS_EOF, szEOF, MAX_STR_LEN); /* error opening file */
  LoadString(hInstance, IDS_ECF, szECF, MAX_STR_LEN); /* error creating file */
  LoadString(hInstance, IDS_EXT, szExt, MAX_STR_LEN); /* default file ext. */

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_ACCEL));


/*----------------------------------------------------------------------------*/
/*  Register XTALK's Main Window Class.                                       */
/*    Processes MENU commands.						      */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_MAIN, szClassName, sizeof(szClassName));

  WndClass.lpfnWndProc	     = WndMainProc;
  WndClass.hInstance	     = hInstance;
  // dca00074 MKL added new icon for Win3.0
  // WndClass.hIcon	     = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
  if (sVersion < 3) WndClass.hIcon	     = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
  else  WndClass.hIcon	     = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON2));
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.lpszMenuName      = (LPSTR)IDM_MENU;
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register XTALK's Child Status Window Class.                               */
/*    Maintains the Status area between the Menu and Terminal display area.   */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_STATUS, szClassName, sizeof(szClassName));

  WndClass.style	     = CS_HREDRAW | CS_DBLCLKS;
  WndClass.lpfnWndProc       = WndStatProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor           = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_HAND));
  WndClass.hbrBackground     = GetStockObject(WHITE_BRUSH);
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register XTALK's Info Status Window Class.                                */
/*    Maintains the Status area between the Menu and Terminal display area.   */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_INFO, szClassName, sizeof(szClassName));

  WndClass.style	     = CS_HREDRAW;
  WndClass.lpfnWndProc	     = WndInfoProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.hbrBackground     = GetStockObject(WHITE_BRUSH);
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register XTALK's Child Macro Window Class.                                */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_MACRO, szClassName, sizeof(szClassName));

  WndClass.style	     = CS_HREDRAW;
  WndClass.lpfnWndProc	     = WndMacroProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor           = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_HAND));
  WndClass.hbrBackground     = COLOR_MENU + 1;
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register XTALK's Child Terminal Window Class.                             */
/*    Terminal emulation window while in VT102/VT52 modes.		      */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_TERM, szClassName, sizeof(szClassName));

  /*
  WndClass.style	     = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
  */
  WndClass.style	     = CS_OWNDC | CS_DBLCLKS;
  WndClass.lpfnWndProc	     = WndVT102Proc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;

/*----------------------------------------------------------------------------*/
/*  Register XTALK's Child Terminal Window Class.                             */
/*    Terminal emulation window while in VT102/VT52 modes.		      */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_CISG, szClassName, sizeof(szClassName));

  WndClass.hCursor	     = LoadCursor(NULL, IDC_CROSS);
  WndClass.hIcon	     = NULL;
  WndClass.lpszMenuName      = (LPSTR)IDM_CISG;
  WndClass.lpszClassName     = szClassName;
/*WndClass.hbrBackground     = (HBRUSH)GetStockObject(WHITE_BRUSH);*/
  WndClass.hInstance	     = hInstance;
  WndClass.style	     = CS_HREDRAW | CS_VREDRAW;
  WndClass.lpfnWndProc	     = CisWndProc;
  WndClass.cbWndExtra	     = 12;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;

  if (!RegisterColorBarControl(hInstance))
    return (FALSE);

  return TRUE;		/* Initialization succeeded */
}


/******************************************************************************/
/*									      */
/*  Routine: AnotherCopyInit						      */
/*									      */
/*  Remarks: Initialize code for the second (or more) copy of XTALK.	      */
/*									      */
/******************************************************************************/

BOOL AnotherCopyInit(hInstance, hPrevInstance)
  HANDLE hInstance;
  HANDLE hPrevInstance;
{
  GetInstanceData(hPrevInstance, szAppName, 16);
  GetInstanceData(hPrevInstance, szAppTitle, 32);

  GetInstanceData(hPrevInstance, szIFN, MAX_STR_LEN);
  GetInstanceData(hPrevInstance, szFNF, MAX_STR_LEN);
  GetInstanceData(hPrevInstance, szREF, MAX_STR_LEN);
  GetInstanceData(hPrevInstance, szSCC, MAX_STR_LEN);
  GetInstanceData(hPrevInstance, szEOF, MAX_STR_LEN);
  GetInstanceData(hPrevInstance, szECF, MAX_STR_LEN);
  GetInstanceData(hPrevInstance, szExt, MAX_STR_LEN);

  GetInstanceData(hPrevInstance, (PSTR)&hAccelTable, sizeof(hAccelTable));

  return TRUE;
}


/******************************************************************************/
/*									      */
/*  Routine: InstanceInit						      */
/*									      */
/*  Remarks: Common Init routines for First and Second copies of XTALK.       */
/*									      */
/******************************************************************************/

BOOL InstanceInit(hInstance, lpszCmdLine, cmdShow )
  HANDLE hInstance;
  LPSTR  lpszCmdLine;
  int	 cmdShow;
{
  HMENU  hMenu;
  HDC	 hDC;
  HANDLE hFile;
  long	 lWaitTime;
  char	 szClassName[32];
  char	 buf[132];
  int	 i;


  // Get the major version of Windows.
  sVersion = LOBYTE(GetVersion());

  // MKL added code for getting full path of xtalk.exe
  GetModuleFileName(GetModuleHandle("XTALK"), (LPSTR)szXtalkExePath, 49);
  for (i=strlen(szXtalkExePath)-1; i>=0; -- i)
  {
      if (szXtalkExePath[i] != '\\')  szXtalkExePath[i] = '\0';
      else break;
  }
  strcpy(szHelpPath, szXtalkExePath);
  if (sVersion < 3) strcat(szHelpPath, "xtalk.hlp");
  else strcat(szHelpPath, "xthelp.hlp");

  // 00048 Prevent windows from displaying disk errors on screen.
  SetErrorMode(1);

  wSystem = SS_INIT;

  /*-------------------------------------------------------------------------*/
  /* Load various useful global variables about this computer.		     */
  /*-------------------------------------------------------------------------*/
  buf[0] = 'D';
  buf[1] = 'I';
  buf[2] = 'S';
  buf[3] = 'P';
  buf[4] = 'L';
  buf[5] = 'A';
  buf[6] = 'Y';
  buf[7] = NULL;
  hDC = CreateIC(buf, &buf[7], &buf[7], &buf[7]);

  if (GetDeviceCaps(hDC, BITSPIXEL) == 1 && GetDeviceCaps(hDC, PLANES) == 1)
    fMonoDisplay = TRUE;
  else
    fMonoDisplay = FALSE;

  nVertRes = GetDeviceCaps(hDC, VERTRES);
  nHorzRes = GetDeviceCaps(hDC, HORZRES);

  if (GetDeviceCaps(hDC, VERTRES) <= 200)
    fLowResDisplay = TRUE;
  else
    fLowResDisplay = FALSE;

  DeleteDC(hDC);

  byScrollBarHeight = (BYTE)GetSystemMetrics(SM_CYHSCROLL);
  byScrollBarWidth  = (BYTE)GetSystemMetrics(SM_CXVSCROLL);

  /*-------------------------------------------------------------------------*/
  /*									     */
  /*-------------------------------------------------------------------------*/
  hWndInst = hInstance;


  bCaptureOn	= FALSE;
  bPrinterOn	= FALSE;
  bDisplayOn	= TRUE;

  hDemoFile  = -1;

  hIconFont = GetStockObject(OEM_FIXED_FONT);


  /* Create a window instance using the application as the class (image) */

  LoadString(hInstance, IDS_WNDCLASS_MAIN, szClassName, sizeof(szClassName));

  hWnd = CreateWindow((LPSTR)szClassName,
		      (LPSTR)szAppTitle,
		      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      0,
		      (HWND)NULL,	   /* no parent */
		      (HMENU)NULL,	   /* use class menu */
		      (HANDLE)hInstance,   /* handle to window instance */
		      (LPSTR)NULL	   /* no params to pass on */
		      );

  hWndMenu = GetSystemMenu(hWnd, FALSE);
  ChangeMenu(hWndMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
  ChangeMenu(hWndMenu, 0, "R&un...", CMD_RUNAPP, MF_APPEND | MF_STRING);

  hWndMenu = GetMenu(hWnd);

  // dca00059 MKL added Windows 3.0 Help
  if (sVersion >= 3) {
     hMenu = GetSubMenu(hWndMenu, 6);
     ChangeMenu(hMenu, 0, "&Index", CMD_INDEX,MF_CHANGE|MF_BYPOSITION|MF_STRING);
     ChangeMenu(hMenu, 1, "&Using Help", CMD_HELP_HELP, MF_BYPOSITION|MF_STRING);
     ChangeMenu(hMenu, 1, NULL, NULL, MF_BYPOSITION|MF_SEPARATOR);
     /*
     ChangeMenu(hMenu,1, "Last Help", CMD_HELP_LAST, MF_BYPOSITION|MF_STRING);
     */
     ChangeMenu(hMenu, 1, "&DDE", CMD_HELP_DDE, MF_BYPOSITION|MF_STRING);
     ChangeMenu(hMenu, 1, "&Keyboard", CMD_HELP_KBD, MF_BYPOSITION|MF_STRING);
     ChangeMenu(hMenu, 1, "&Mouse", CMD_HELP_MOUSE, MF_BYPOSITION|MF_STRING);
     ChangeMenu(hMenu, 1, "&Commands", CMD_HELP_CMMD, MF_BYPOSITION|MF_STRING);
     ChangeMenu(hMenu, 1, "&Procedures", CMD_HELP_PROC, MF_BYPOSITION|MF_STRING);
  }

  SetScrollLock(hWnd, FALSE);

  if (cmdShow == SW_SHOW && GetProfileInt(szAppName, "Maximized", 1))
    cmdShow = SW_MAXIMIZE;

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  lWaitTime = GetCurrentTime() + 3000;

  if (!InitVariable())
    return FALSE;
  if (!InitData())
    return FALSE;
  if (!InitFont())
    return FALSE;
  if (!InitGlobal())
    return FALSE;
  if (!InitMacro())
    return FALSE;
  if (!InitSession())
    return FALSE;
  if (!InitTerminal())
    return FALSE;
  if (!InitProtocol())
    return FALSE;
  if (!InitDisplay())
    return FALSE;

  InitPerformance();
  InitPrinter();
  InitDDE();

  IconTextChanged(NULL);

  // 00058 Reset info line font when started with :XTALK format.
  if (*lpszCmdLine)
    {
    while (*lpszCmdLine == ' ')
      lpszCmdLine++;
    if (*lpszCmdLine != ':')
      {
      hInfoFont = hSysFont;
      hMacroFont = hSysFont;
      }
    }
  else
    {
    hInfoFont = hSysFont;
    hMacroFont = hSysFont;
    }

  CreateStatusChild(hWnd);
  CreateInfoChild(hWnd);
  CreateMacroChild(hWnd);
  CreateInputChild(hWnd);
  CreateTerminalChild(hWnd);

  ShowWindow(hChildStatus, SW_HIDE);
  ShowWindow(hChildInfo, SW_HIDE);
  ShowWindow(hChildMacro, SW_HIDE);
  ShowWindow(hChildTerminal, SW_HIDE);

  if (!InitCommunications())
    return FALSE;

  DataNew();
  InitSystemNewData();

  SetTimer(hWnd, COMM_TIMER, 250, NULL);

  ResetSystemStatus(SS_INIT);

  while (lWaitTime > GetCurrentTime());

  ResetScreenDisplay();

  ShowWindow(hChildStatus, SW_SHOW);
  UpdateWindow(hChildStatus);
  ShowWindow(hChildInfo, SW_SHOW);
  UpdateWindow(hChildInfo);
  ShowWindow(hChildMacro, SW_SHOW);
  UpdateWindow(hChildMacro);
  ShowWindow(hChildTerminal, SW_SHOW);
  UpdateWindow(hChildTerminal);

  if (*lpszCmdLine)
    {
    // 00038 If the first command line argument starts with a colon then the
    // 00038   has perform a non MS-DOS exec startup from DOS.  Skip over it.
    while (*lpszCmdLine == ' ')
      lpszCmdLine++;
    if (*lpszCmdLine == ':')
      {
      while (*lpszCmdLine != ' ' && *lpszCmdLine)  // Skip over the ':' word.
        lpszCmdLine++;
      while (*lpszCmdLine == ' ')
        lpszCmdLine++;
      }
    // 00038 *** End of Fix ***

    lstrcpy(buf, lpszCmdLine);

    if (memicmp(buf, "system", 6) == 0)
      *lpszCmdLine = NULL;
    }

  if (*lpszCmdLine != NULL)
    {
    if (szStartupScript[0])
      {
      strcpy(buf, &szStartupScript[1]);
      i = (int)(szStartupScript[0]);
      buf[i++] = ' ';
      lstrcpy(&buf[i], lpszCmdLine);
      strupr(buf);
      if (i = ExecuteScript(buf))
	GeneralError(i);
      }
    else
      {
      lstrcpy(buf, lpszCmdLine);
      strupr(buf);

      if (i = DataOpen(buf))
        GeneralError(i);
      else
	{
        CmdConnect();
	DataAutoOpen();  // 00004 Auto open the capture and/or printer.
	}
      }
    }
  else
    {
    if (Comm.cLocal)
      {
      if (CommPortOpen())
	{
	Comm.cPort = 1;
	CommPortUpdate();
	PostMessage(hChildStatus, UM_CONFIG, 0, 0L);
	}
      }

    if (szStartupScript[0])
      {
      if (i = ExecuteScript(&szStartupScript[1]))
	GeneralError(i);
      }
    }

  if (hXferDlg)
    SetFocus(hXferDlg);
  else
    SetFocus(hChildTerminal);

  return TRUE;
}

void CmdExit()
{
  if (DataSaveFirst() == IDCANCEL)
    return;

  ProgramAbort();
}

void ProgramAbort()
{
  if (IfSystemStatus(SS_XFER))
    {
    ErrorMessage(MSGXFR_MUSTCLOSE);
    return;
    }

  if (IfSystemStatus(SS_ABORT))
    nDDESessions = 0;

  if (nDDESessions)
    TermDDE();

  SetSystemStatus(SS_ABORT);
}

void ProgramWrapup()
{
  KillTimer(hWnd, COMM_TIMER);

  if (IfSystemStatus(SS_DIAL))
    DialTerm();

  if (IfSystemStatus(SS_SCRIPT))
    InterpreterStop();

  if (bCaptureOn)
    CmdCapture(0, 0);

  TermDDE();

  CommPortClose();
  TermCommunications();

  TermPrinter();

  DestroyStatusChild();
  DestroyInfoChild();
  DestroyMacroChild();
  DestroyInputChild();
  DestroyTerminalChild();

  TermFont();
  TermPerformance();
  DeleteObject(hIconFont);
  TermVariable();

  if (GetProfileInt(szAppName, "Maximized", 1) != fMaximized)
    {
    char buf[2];

    buf[0] = (char)('0' + fMaximized);
    buf[1] = NULL;
    WriteProfileString(szAppName, "Maximized", buf);
    }
}


void CmdAbout()
{
  FARPROC lpDlgAbout;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_ABOUT;

  lpDlgAbout = MakeProcInstance((FARPROC)DlgAbout, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, lpDlgAbout);
  FreeProcInstance(lpDlgAbout);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


BOOL DlgAbout(hDlg, message, wParam, lParam)
  HWND	   hDlg;
  unsigned message;
  WORD	   wParam;
  LONG	   lParam;
{
  union REGS regs;
  WORD wFree, wUsed;
  int  hFile;
  char buf[128], fmt[128];


  switch (message)
    {
    case WM_INITDIALOG:
      wFree = (WORD)(GlobalCompact(0L) / 1024L);
      wUsed = nScrollMemory;
      if (bConnected)
        wUsed += nCommPortMemory / 512;

      GetDlgItemText(hDlg, ID_ABOUT_CMEM, fmt, sizeof(fmt));
      sprintf(buf, fmt, wFree, wFree+wUsed);
      SetDlgItemText(hDlg, ID_ABOUT_CMEM, buf);

      SetFocus(GetDlgItem(hDlg, ID_OK));
      return (FALSE);
      break;

    case WM_COMMAND:
      EndDialog(hDlg, TRUE);
      break;

    default:
      return FALSE;
      break;
    }

  return(TRUE);
}


void CmdRunApp()
{
  FARPROC lpDlg;
  static  char szNull[2];
  register i;

  lpDlg = MakeProcInstance((FARPROC)DlgRunApp, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_RUNAPP), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  szNull[0] = 1;
  szNull[1] = 0;

  LockData(0);
  switch (i)
    {
    case 1:
      SpawnApp("CLIPBRD.EXE", szNull);
      break;

    case 2:
      SpawnApp("CONTROL.EXE", szNull);
      break;

    case 3:
      SpawnApp("NOTEPAD.EXE", szNull);
      break;
    }
  UnlockData(0);
}


BOOL DlgRunApp(hDlg, message, wParam, lParam)
  HWND	   hDlg;
  unsigned message;
  WORD	   wParam;
  LONG	   lParam;
{
  register i;

  switch (message)
    {
    case WM_INITDIALOG:
      EnableWindow(GetDlgItem(hDlg, ID_OK), FALSE);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (IsDlgButtonChecked(hDlg, ID_RUNAPP_CLIPBOARD))
            i = 1;
          else if (IsDlgButtonChecked(hDlg, ID_RUNAPP_CONTROL))
            i = 2;
          else
            i = 3;
          EndDialog(hDlg, i);
          break;

	case ID_CANCEL:
          EndDialog(hDlg, FALSE);
          break;

        case ID_RUNAPP_CLIPBOARD:
        case ID_RUNAPP_CONTROL:
        case ID_RUNAPP_NOTEPAD:
	  if (HIWORD(lParam) == BN_CLICKED)
            {
            EnableWindow(GetDlgItem(hDlg, ID_OK), TRUE);
            CheckRadioButton(hDlg, ID_RUNAPP_CLIPBOARD, ID_RUNAPP_NOTEPAD,
              wParam);
            }
	}
      break;

    default:
      return FALSE;
      break;
    }

  return(TRUE);
}
