/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Scrollback Logic (PROPERF2.C)                                   *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module does all the real work with the scroll back buffer. *
*             We try to reduce the overhead to Windows by allocating our      *
*             scrollback buffer in nice 4k chucks and by compressing the      *
*             lines of text.  So we really never know how much we can fit in  *
*             scroll buffer.                                                  *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00039 Improve Scrollback buffer support.          *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include "xtalk.h"
#include "term.h"
#include "properf.h"
#include "library.h"

extern int hDemoFile;

void VssScrollMode()
{
  if (wSystem & SS_SCROLL)
    {
    if ((wSystem & SS_SCRIPT) == 0 && (GetKeyState(0x91) & 0x01))
      return;

    MouseSelectClr();

    ResetSystemStatus(SS_SCROLL);
    SystemUpdate();

    SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
    CheckMenuItem(hWndMenu, CMD_SCROLL, MF_UNCHECKED);

    if (nVssCurLinePos || nTermOffX)
      {
      sVssOldLinePos = nVssCurLinePos;
      nVssCurLinePos = 0;
      nTermOffX = 0;
      InvalidateRect(hChildTerminal, NULL, TRUE);
      }

    VidUpdateSize();

    VidSetCaretPos();
    ShowCaret(hChildTerminal);
    }
  else
    {
    if (GetKeyState(0x91) & 0x01 == 0)
      return;

    if (wSystem & SS_SCRIPT)
      {
      SetScrollLock(hWnd, FALSE);
      return;
      }

    SetSystemStatus(SS_SCROLL);
    SystemUpdate();

    SendMessage(hChildInfo, UM_INFOMSG, 0, 0L);
    CheckMenuItem(hWndMenu, CMD_SCROLL, MF_CHECKED);

    HideCaret(hChildTerminal);

    if (-sVssOldLinePos < nVssCurLineCnt - nTermExtY)
      {
      nVssCurLinePos = sVssOldLinePos;
      InvalidateRect(hChildTerminal, NULL, TRUE);
      }
    else if (sVssOldLinePos)
      {
      nVssCurLinePos = -nVssCurLineCnt;
      InvalidateRect(hChildTerminal, NULL, TRUE);
      }

    VidUpdateSize();
    }
}


void VssScrollToggle()
{
  if ((GetKeyState(0x91) & 0x01) == 0)
    {
    if (!(wSystem & SS_SCRIPT) && hDemoFile == -1)
      SetScrollLock(hWnd, TRUE);
    }
  else
    SetScrollLock(hWnd, FALSE);
}

void VssAddLine(pLine)
  PINT pLine;
{
  WORD	wAdr, wPag, wOff;
  WORD	wRng1, wRng2;
  WORD	wFirst;
  LPINT lpPage;

  int  sBuf[VID_MAXCOL+8];
  int  ch, rpt;
  int  nLen;
  register i;


  if (nVssPagesUsed == 0)
    return;

  ch  = *pLine++;
  rpt = 1;
  for (i = 1, nLen = 0; i <= VID_MAXCOL; i++)
    {
    if (*pLine == ch && i != VID_MAXCOL)
      {
      rpt++;
      }
    else
      {
      if (rpt < 4 && ch != 0)
	{
	while (rpt--)
	  sBuf[nLen++] = ch;
	}
      else
	{
	sBuf[nLen++] = 0;
	sBuf[nLen++] = rpt;
	sBuf[nLen++] = ch;
	}

      ch  = *pLine;
      rpt = 1;
      }

    pLine++;
    }

  /*------------------------------------------------------------------------*/
  /*  Load line/offset table, always first page.			    */
  /*------------------------------------------------------------------------*/
  lpPage = (LPINT)lpVssPages[0];


  /*------------------------------------------------------------------------*/
  /*  Find the address for the new line and adjust for overwrites.  If the  */
  /*  current line count is zero, then initial values are ok to start with. */
  /*------------------------------------------------------------------------*/
  if (nVssCurLineCnt)
    {
    wAdr = lpPage[nVssLastLineInx] + nVssLastLineLen;
    wRng1 = wAdr;
    wPag  = wAdr / VSS_PAGESIZE;
    wOff  = wAdr % VSS_PAGESIZE;

    if (wOff + nLen >= (VSS_PAGESIZE / sizeof(int)))
      {
      wPag++;
      wOff = 0;

      if (wPag == nVssPagesUsed)
	wPag = 1;

      wAdr = wPag * VSS_PAGESIZE;
      }

    wRng2 = wAdr + nLen;

    /*--------------------------------------------------------------------*/
    /*	Search index for entries that will be overwritten and delete.	  */
    /*--------------------------------------------------------------------*/

    /*
    fprintf(stdaux, "wAdr %u, wPag %u, wOff %u, Rng1 %u, Rng2 %u\r\n",
	  wAdr, wPag, wOff, wRng1, wRng2);
    */

    while (1)
      {
      wFirst = lpPage[nVssFirstLineInx];
      if ((wRng1 < wRng2 && wFirst >= wRng1 && wFirst <= wRng2) ||
	  (wRng1 > wRng2 && (wFirst >= wRng1 || wFirst <= wRng2)))
	{
	if (++nVssFirstLineInx == VSS_PAGESIZE / sizeof(int))
	  nVssFirstLineInx = 0;
	nVssCurLineCnt--;
	}
      else
	break;
      }

    /*--------------------------------------------------------------------*/
    /*	Bump last line index by one, if overwrap, adjust pointers.	  */
    /*--------------------------------------------------------------------*/
    if (++nVssLastLineInx == VSS_PAGESIZE / sizeof(int))
      nVssLastLineInx = 0;

    lpPage[nVssLastLineInx] = wAdr;
    nVssLastLineLen	    = nLen;

    if (nVssLastLineInx == nVssFirstLineInx)
      {
      if (++nVssFirstLineInx == VSS_PAGESIZE / sizeof(int))
	nVssFirstLineInx = 0;
      nVssCurLineCnt--;
      }
    }
  else
    {
    wAdr = VSS_PAGESIZE;
    wPag = 1;
    wOff = 0;

    *lpPage         = wAdr;
    nVssLastLineLen = nLen;
    }

  /*------------------------------------------------------------------------*/
  /*  Copy new compress line to memory and update counters.		    */
  /*------------------------------------------------------------------------*/
  lpPage = (LPINT)lpVssPages[wPag];

  /*
  fprintf(stdaux, "Memcpy Page = %lX, Offset = %u\r\n", lpPage, wOff);
  */

  for (i = 0, lpPage += wOff; i < nLen; i++)
    *lpPage++ = sBuf[i];

  nVssCurLineCnt++;

  /*------------------------------------------------------------------------*/
  /*  Release the data page if in virtual memory mode.			    */
  /*------------------------------------------------------------------------*/
}


PINT VssGetLine(nLine)
  int  nLine;
{
  WORD	wAdr, wPag, wOff;
  LPINT lpPage;
  int	ch, rpt;
  int	i;


  /*
  fprintf(stdaux, "nLine %d, Pos %d, Cnt %d\r\n", nLine, nVssCurLinePos, nVssCurLineCnt);
  */

  nLine += nVssCurLinePos;


  if (nLine >= VID_MAXROW || nLine < (-nVssCurLineCnt))
    return (NULL);

  if (nLine >= 0)
    return (pPriVidLine[nLine]);

  if ((nLine += nVssLastLineInx + 1) < 0)
    nLine += VSS_PAGESIZE / sizeof(int);


  /*------------------------------------------------------------------------*/
  /*  Load line/offset table, always first page.			    */
  /*------------------------------------------------------------------------*/
  lpPage = (LPINT)lpVssPages[0];


  /*------------------------------------------------------------------------*/
  /*  Find the address of the request line.				    */
  /*------------------------------------------------------------------------*/
  wAdr = lpPage[nLine];
  wPag = wAdr / VSS_PAGESIZE;
  wOff = wAdr % VSS_PAGESIZE;


  /*------------------------------------------------------------------------*/
  /*  Copy & decompress line to local memory buffer.			    */
  /*------------------------------------------------------------------------*/
  lpPage = (LPINT)lpVssPages[wPag];

  /*
  fprintf(stdaux, "Memcpy Page = %lX, Offset = %u\r\n", lpPage, wOff);
  */

  lpPage += wOff;
  for (i = 0; i < VID_MAXCOL;)
    {
    if ((ch = *lpPage++) == 0)
      {
      rpt = *lpPage++;
      ch  = *lpPage++;
      while (rpt--)
	sVssCurLineBuf[i++] = ch;
      }
    else
      {
      sVssCurLineBuf[i++] = ch;
      }
    }

  return (sVssCurLineBuf);
}


BOOL VssScrollDown(nLines)
  int nLines;
{
  if (nVssCurLinePos == -nVssCurLineCnt)
    {
    if (!fMouseDown)
      MessageBeep(NULL);
    return FALSE;
    }

  if (nLines == 0)
    {
    if (nTermExtY < VID_MAXROW)
      nLines = nTermExtY;
    else
      nLines = VID_MAXROW;
    }

  nVssCurLinePos -= nLines;

  if (nVssCurLinePos < -nVssCurLineCnt)
    nVssCurLinePos = -nVssCurLineCnt;

  if (nLines == 1)
    {
    UpdateWindow(hChildTerminal);
    ScrollWindow(hChildTerminal, 0, nCurFontHeight, NULL, NULL);
    }
  else
    {
    InvalidateRect(hChildTerminal, NULL, TRUE);
    }

  SetScrollPos(hChildTerminal, SB_VERT, nVssCurLineCnt + nVssCurLinePos, TRUE);
  UpdateWindow(hChildTerminal);

  return TRUE;
}

BOOL VssScrollUp(nLines)
  int nLines;
{
  if (nVssCurLinePos == VID_MAXROW - nTermExtY)
    {
    if (!fMouseDown)
      MessageBeep(NULL);
    return FALSE;
    }

  if (nLines == 0)
    {
    if (nTermExtY < VID_MAXROW)
      nLines = nTermExtY;
    else
      nLines = VID_MAXROW;
    }

  nVssCurLinePos += nLines;

  if (nVssCurLinePos > VID_MAXROW - nTermExtY)
    nVssCurLinePos = VID_MAXROW - nTermExtY;

  if (nLines == 1)
    {
    UpdateWindow(hChildTerminal);
    ScrollWindow(hChildTerminal, 0, -nCurFontHeight, NULL, NULL);
    }
  else
    {
    InvalidateRect(hChildTerminal, NULL, TRUE);
    }
  UpdateWindow(hChildTerminal);

  SetScrollPos(hChildTerminal, SB_VERT, nVssCurLineCnt + nVssCurLinePos, TRUE);
  return TRUE;
}

BOOL VssGotoTop()
{
  if (nVssCurLinePos == -nVssCurLineCnt)
    {
    MessageBeep(NULL);
    return FALSE;
    }

  nVssCurLinePos = -nVssCurLineCnt;

  InvalidateRect(hChildTerminal, NULL, TRUE);
  SetScrollPos(hChildTerminal, SB_VERT, nVssCurLineCnt + nVssCurLinePos, TRUE);
  UpdateWindow(hChildTerminal);

  return TRUE;
}

BOOL VssGotoBot()
{
  if (nVssCurLinePos == 0)
    {
    MessageBeep(NULL);
    return FALSE;
    }

  nVssCurLinePos = 0;

  InvalidateRect(hChildTerminal, NULL, TRUE);
  SetScrollPos(hChildTerminal, SB_VERT, nVssCurLineCnt + nVssCurLinePos, TRUE);
  UpdateWindow(hChildTerminal);

  return TRUE;
}
