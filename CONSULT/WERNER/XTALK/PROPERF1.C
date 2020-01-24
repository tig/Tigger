/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Scrollback Module (PROPERF1.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains support code for performance related       *
*             options in the system dialog (memory usage).                    *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00039 Improve Scrollback buffer support.          *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "properf.h"
#include "dialogs.h"

  int  nScrollMemory;
  int  nScrollType;
  int  nCommPortMemory;

  HANDLE  hVssPages[VSS_MAXPAGES];
  LPSTR  lpVssPages[VSS_MAXPAGES];
  int	  nVssPagesUsed;

  WORD	  nVssFirstLineInx;
  WORD	  nVssLastLineInx;
  int	  nVssLastLineLen;
  int	  nVssCurLineCnt;
  int	  nVssCurLinePos;
  short   sVssOldLinePos;
  int	  sVssCurLineBuf[VID_MAXCOL+1];


void VssCreate()
{
  char sBuf[1024];
  int  i;


  memset(hVssPages, 0, sizeof(hVssPages));
  memset(lpVssPages, 0, sizeof(lpVssPages));
  nVssCurLineCnt = 0;
  nVssCurLinePos = 0;
  sVssOldLinePos = 0;

  if (nScrollMemory == 0)
    {
    nVssPagesUsed = 0;
    return;
    }

  /*  Temp fix for development of Scroll Logic	*/
  nScrollType = VSS_REALMEM;

  if (nScrollType == VSS_REALMEM)
    {
    for (i = 0; i < nScrollMemory / 4; i++)
      {
      if ((hVssPages[i] = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, (long)VSS_PAGESIZE)) == NULL)
	break;

      lpVssPages[i] = GlobalLock(hVssPages[i]);
      }

    if (i < 2)
      {
      WarningMessage(MSGVSS_NOREALMEM);
      VssDelete();
      return;
      }

    if (i < nScrollMemory / 4)
      WarningMessage(MSGVSS_PTREALMEM);

    nVssPagesUsed = i;
    }
  else
    {
    /* EMS MEMORY CODE */
    }

  nVssFirstLineInx = 0;
  nVssLastLineInx  = 0;
  nVssLastLineLen  = 0;
  nVssCurLineCnt   = 0;
  nVssCurLinePos   = 0;
}


void VssDelete()
{
  char sBuf[64];
  int  i;


  if (nScrollType == VSS_REALMEM)
    {
    for (i = 0; i < VSS_MAXPAGES; i++)
      {
      if (lpVssPages[i] != 0L)
        GlobalUnlock(hVssPages[i]);

      if (hVssPages[i] != 0)
	GlobalFree(hVssPages[i]);
      }
    }
  else
    {
    /* EMS MEMORY CODE */
    }

  nVssPagesUsed = 0;
}


BOOL InitPerformance()
{
  VssCreate();

  return (TRUE);
}


void TermPerformance()
{
  if (wSystem & SS_SCROLL)
    {
    MouseSelectClr();

    wSystem &= ~SS_SCROLL;
    SystemUpdate();

    nVssCurLinePos = 0;
    nTermOffX = 0;

    ShowCaret(hChildTerminal);
    }

  VssDelete();
}
