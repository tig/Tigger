/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Info Init (INFOINIT.C)                                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the initialization code the info child     *
*             window.                                                         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#define NOATOM                  /* atom routines */
#define NOBITMAP		/* typedef HBITMAP and associated routines */
#define NOBRUSH 		/* typedef HBRUSH and associated routines */
#define NOCLIPBOARD		/* clipboard routines */
#define NOCOLOR 		/* CTLCOLOR_*, COLOR_* */
#define NOCOMM			/* COMM driver routines */
#define NOCREATESTRUCT		/* typedef CREATESTRUCT */
#define NOCTLMGR		/* control and dialog routines */
#define NODRAWTEXT		/* DrawText() and DT_* */
#define NOFONT			/* typedef FONT and associated routines */
#define NOGDI			/* StretchBlt modes and gdi logical objects */
#define NOGDICAPMASKS		/* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOICON			/* IDI_* */
#define NOKANJI 		/* Kanji support stuff. */
#define NOKEYSTATE		/* MK_* */
#define NOMB			/* MB_* and MessageBox() */
#define NOMENUS 		/* MF_*, HMENU and associated routines */
#define NOMETAFILE		/* typedef METAFILEPICT */
#define NOMINMAX		/* Macros min(a,b) and max(a,b) */
#define NONCMESSAGES		/* WM_NC* and HT* */
#define NOPEN			/* typedef HPEN and associated routines */
#define NORASTEROPS		/* binary and tertiary raster ops */
#define NOREGION		/* typedef HRGN and associated routines */
#define NORESOURCE		/* Predefined resource types:  RT_* */
#define NOSCROLL		/* SB_* and scrolling routines */
#define NOSHOWWINDOW		/* SHOW_* and HIDE_* */
#define NOSOUND 		/* Sound driver routines */
#define NOSYSCOMMANDS		/* SC_* */
#define NOSYSMETRICS		/* SM_*, GetSystemMetrics */
#define NOVIRTUALKEYCODES	/* VK_* */
#define NOWH			/* SetWindowsHook and WH_* */
#define NOWNDCLASS		/* typedef WNDCLASS and associated routines */

#include <windows.h>
#include <stdio.h>
#include "xtalk.h"
#include "dialogs.h"


  HWND hChildInfo = NULL;
  char fMenuMode;

BOOL far pascal CreateInfoChild(hParent)
  HWND hParent;
{
  HWND hWnd;
  char szClassName[64];


  fMenuMode = FALSE;

  LoadString(hWndInst, IDS_WNDCLASS_INFO, szClassName, sizeof(szClassName));

  hWnd = CreateWindow(szClassName,
		      NULL,
		      WS_CHILD | WS_VISIBLE,
		      0,
		      0,
		      0,
		      0,
		      hParent,
		      (HMENU)NULL,
		      (HANDLE)hWndInst,
		      NULL
		      );

  if (hWnd)
    {
    hChildInfo = hWnd;
    return TRUE;
    }
  else
    {
    hChildInfo = NULL;
    return FALSE;
    }
}


BOOL far pascal DestroyInfoChild()
{
  if (hChildInfo)
    DestroyWindow(hChildInfo);

  hChildInfo = NULL;

  return TRUE;
}
