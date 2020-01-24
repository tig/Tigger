/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Initialization (PROINIT.C)                                       */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include <string.h>
#include "lexis.h"
#include "library.h"
#include "comm.h"
#include "font.h"
#include "term.h"
#include "session.h"
#include "record.h"
#include "diary.h"
#include "data.h"
#include "dialogs.h"
#include "btnlib.h"
#include "robot.h"

/* message box strings loaded in sample.c from the stringtable */
extern char szIFN[], szFNF[], szREF[], szSCC[], szEOF[], szECF[], szExt[];
extern OFSTRUCT DataFileStruct;


/******************************************************************************/
/*									      */
/*  Routine: FirstCopyInit						      */
/*									      */
/*  Remarks: Initialize code for the first copy of WinLexis in memory.        */
/*									      */
/******************************************************************************/

BOOL FirstCopyInit(hInstance)
  HANDLE hInstance;
{
  WNDCLASS WndClass;
  char	   szClassName[32], buf[32];
  int	   i;


  //SetErrorMode(1);

  /* Load strings from resource */

  LoadString(hInstance, IDS_APPNAME,  szAppName, 16);
  LoadString(hInstance, IDS_APPTITLE, szAppTitle, sizeof(szAppTitle));

  LoadString(hInstance, IDS_IFN, szIFN, MAX_STR_LEN); /* illegal filename */
  LoadString(hInstance, IDS_FNF, szFNF, MAX_STR_LEN); /* file not found     */
  LoadString(hInstance, IDS_REF, szREF, MAX_STR_LEN); /* replace existing file */
  LoadString(hInstance, IDS_SCC, szSCC, MAX_STR_LEN); /* save current changes */
  LoadString(hInstance, IDS_EOF, szEOF, MAX_STR_LEN); /* error opening file */
  LoadString(hInstance, IDS_ECF, szECF, MAX_STR_LEN); /* error creating file */
  LoadString(hInstance, IDS_EXT, szExt, MAX_STR_LEN); /* default file ext. */

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_ACCEL));


/*----------------------------------------------------------------------------*/
/*  Register WinLexis's Main Window Class.                                    */
/*    Processes MENU commands.						      */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_MAIN, szClassName, sizeof(szClassName));

  WndClass.lpfnWndProc       = MainWndProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hIcon             = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.lpszMenuName      = (LPSTR)IDM_MENU;
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register WinLexis's Child Status Window Class.                            */
/*    Maintains the Status area between the Menu and Terminal display area.   */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_STATUS, szClassName, sizeof(szClassName));

  WndClass.style	     = CS_HREDRAW | CS_DBLCLKS;
  WndClass.lpfnWndProc       = StatusWndProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.hbrBackground     = GetStockObject(WHITE_BRUSH);
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register WinLexis's Info Status Window Class.                             */
/*    Maintains the Status area between the Menu and Terminal display area.   */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_INFO, szClassName, sizeof(szClassName));

  WndClass.style	     = CS_HREDRAW;
  WndClass.lpfnWndProc       = InfoWndProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor           = LoadCursor(NULL, IDC_ARROW);
  WndClass.hbrBackground     = GetStockObject(WHITE_BRUSH);
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;


/*----------------------------------------------------------------------------*/
/*  Register WinLexis's Child Terminal Window Class.                          */
/*    Terminal emulation window while in VT102/VT52 modes.		      */
/*									      */
/*----------------------------------------------------------------------------*/
  memset(&WndClass, NULL, sizeof(WNDCLASS));
  LoadString(hInstance, IDS_WNDCLASS_TERM, szClassName, sizeof(szClassName));

  /*
  WndClass.style	     = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
  */
  WndClass.style	     = CS_OWNDC | CS_DBLCLKS;
  WndClass.lpfnWndProc       = TermWndProc;
  WndClass.hInstance	     = hInstance;
  WndClass.hCursor	     = LoadCursor(NULL, IDC_ARROW);
  WndClass.lpszClassName     = (LPSTR)szClassName;

  if (!RegisterClass((LPWNDCLASS)&WndClass))
    return FALSE;

  if (!RegisterButtons(hInstance))
    return (FALSE);

  return TRUE;		/* Initialization succeeded */
}


/******************************************************************************/
/*									      */
/*  Routine: AnotherCopyInit						      */
/*									      */
/*  Remarks: Initialize code for the second (or more) copy of WinLexis.       */
/*									      */
/******************************************************************************/

BOOL AnotherCopyInit(hInstance, hPrevInstance)
  HANDLE hInstance;
  HANDLE hPrevInstance;
{
  GetInstanceData(hPrevInstance, szAppName, 16);
  GetInstanceData(hPrevInstance, szAppTitle, sizeof(szAppTitle));

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
/*  Remarks: Common Init routines for First and Second copies of WinLexis.    */
/*									      */
/******************************************************************************/

BOOL InstanceInit(hInstance, lpszCmdLine, cmdShow )
  HANDLE hInstance;
  LPSTR  lpszCmdLine;
  int	 cmdShow;
{
  HMENU   hMenu;
  HDC     hDC;
  HANDLE  hFile;
  long    lWaitTime;
  char    szClassName[32];
  char    buf[132];
  register i;


  nWorkVersion = LOBYTE(GetVersion());

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


  bPrinterOn	= FALSE;


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

  hWndMenu = GetMenu(hWnd);

  if (cmdShow == SW_SHOW)
    cmdShow = SW_MAXIMIZE;

  ShowWindow(hWnd, cmdShow);
  UpdateWindow(hWnd);

  lWaitTime = GetCurrentTime() + 5000;

  if (!InitFont())
    return FALSE;
  if (!InitTerminal())
    return FALSE;

  InitPrinter();
  InitDDE();
  RobotReset();

  CreateStatusChild(hWnd);
  CreateInfoChild(hWnd);
  CreateMacroChild(hWnd);
  CreateTerminalChild(hWnd);

  if (!CommInit())
    return FALSE;
  if (!DataInit())
    return FALSE;
  UpdateFont();
  if (!SessInit())
    return FALSE;

  ResetSystemStatus(SS_INIT);

  while (lWaitTime > GetCurrentTime());

  SetTimer(hWnd, COMM_TIMER, 250, NULL);

  ResetScreenDisplay();

  SetFocus(hChildTerminal);

  return TRUE;
}

void ProgramAbort()
{
  if (IfSystemStatus(SS_ABORT))
    nDDESessions = 0;

  if (nDDESessions)
    TermDDE();

  SetSystemStatus(SS_ABORT);
}

void ProgramWrapup()
{
  KillTimer(hWnd, COMM_TIMER);

  RobotTerm();

  RecordTerm();
  DiaryTerm();
  PrinterOff();

  TermDDE();

  SessTerm();
  DataTerm();
  CommTerm();

  TermPrinter();

  DestroyStatusChild();
  DestroyInfoChild();
  DestroyMacroChild();
  DestroyTerminalChild();

  TermFont();
  TermVideo();
}


void CmdAbout()
{
  FARPROC lpDlgAbout;

  lpDlgAbout = MakeProcInstance((FARPROC)DlgAbout, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, lpDlgAbout);
  FreeProcInstance(lpDlgAbout);
}


BOOL DlgAbout(hDlg, message, wParam, lParam)
  HWND	   hDlg;
  unsigned message;
  WORD	   wParam;
  LONG	   lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case ID_OK:
	case ID_CANCEL:
	  EndDialog(hDlg, TRUE);
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return(TRUE);
}
