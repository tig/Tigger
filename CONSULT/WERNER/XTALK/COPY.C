/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Copy Data (COPY.C)                                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   Copy to the clipboard, to a file, to the printer and to other   *
*             wonderful places.  If we are not in scroll mode or nothing has  *
*             selected we just copy the entire screen.                        *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 10/15/89 00046 Flush time printer after a copy to printer. *
*	      1.01 10/26/89 00052 Remove trailing ^Z from copy to file.       *
*             1.01 01/19/90 00093 Make sure the clipboard is already empty.   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include <string.h>	//dca00046 JDB Added for external decls
#include "xtalk.h"
#include "library.h"
#include "capture.h"
#include "term.h"
#include "sess.h"
#include "comm.h"
#include "disp.h"
#include "prodata.h"
#include "properf.h"
#include "dialogs.h"
#include "interp.h"
#include "global.h"
#include "variable.h"

extern void ToggleNotes();

static void near GetSelectedLine(int, PSTR, int, int, int, int, char);
static void near AppendToNotes(char *, int);


void CmdCopy()
{
  HANDLE hMem;
  LPSTR  lpStr;
  char	 buf[256];
  int	 len;
  register int i;


  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = nTermMaxCol;
    nSelectY1 = nVssCurLinePos;
    nSelectY2 = nTermWndY + nVssCurLinePos;
    if (nSelectY2 > TERM_MAXROW)
      nSelectY2 = TERM_MAXROW;
    }

  if (nSelectY1 > nSelectY2)
    {
    i = nSelectY1;
    nSelectY1 = nSelectY2;
    nSelectY2 = i;

    i = nSelectX1;
    nSelectX1 = nSelectX2;
    nSelectX2 = i;
    }

  for (i = nSelectY1, len = 1; i <= nSelectY2; i++)
    {
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, TRUE);
    len += strlen(buf);
    }

  hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE|GMEM_LOWER|GMEM_ZEROINIT, (DWORD)len);
  if (hMem == 0)
    {
    return;
    }

  lpStr = (LPSTR)GlobalLock(hMem);

  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    register j;

    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, TRUE);
    for (j = 0; buf[j]; j++)
      *lpStr++ = buf[j];
    }
  *lpStr = NULL;

  GlobalUnlock(hMem);

  if (OpenClipboard(hWnd))
    {
    // 00093 Make sure the clipboard is already empty.
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    }
  else
    {
    GlobalFree(hMem);
    MessageBeep(NULL);
    }

}


void CmdCopyCapture()
{
  char	 buf[256];
  register int i;


  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = nTermMaxCol;
    nSelectY1 = nVssCurLinePos;
    nSelectY2 = nTermWndY + nVssCurLinePos;
    if (nSelectY2 > TERM_MAXROW)
      nSelectY2 = TERM_MAXROW;
    }

  if (nSelectY1 > nSelectY2)
    {
    i = nSelectY1;
    nSelectY1 = nSelectY2;
    nSelectY2 = i;

    i = nSelectX1;
    nSelectX1 = nSelectX2;
    nSelectX2 = i;
    }

  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, FALSE);
    WriteStrToCapture(buf);
    }
}


void CmdCopyFile()
{
  USHORT usBytes;
  // 00052 Save the previous file name.
  static char FileName[64] = "";
  OFSTRUCT OfStruct;
  int  file;
  char buf[256], tmp[32];
  register int i;


  memset(&OfStruct, 0, sizeof(OFSTRUCT));

  SetFullPath(VAR_DIRFIL);
  tmp[0] = '\\';
  tmp[1] = '*';
  tmp[2] = '.';
  tmp[3] = 'T';
  tmp[4] = 'X';
  tmp[5] = 'T';
  tmp[6] = NULL;

  i = DlgSaveAs(hWndInst, hWnd, IDD_SAVEAS, &OfStruct, &file,
		FileName, tmp, szAppName, TRUE, TRUE);

  if (i == NOSAVE || file == -1)
    return;

  // 00052 Remove trailing control-Z's.
  SeekToEndOfFile(file);

  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = nTermMaxCol;
    nSelectY1 = nVssCurLinePos;
    nSelectY2 = nTermWndY + nVssCurLinePos;
    if (nSelectY2 > TERM_MAXROW)
      nSelectY2 = TERM_MAXROW;
    }

  if (nSelectY1 > nSelectY2)
    {
    i = nSelectY1;
    nSelectY1 = nSelectY2;
    nSelectY2 = i;

    i = nSelectX1;
    nSelectX1 = nSelectX2;
    nSelectX2 = i;
    }


  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, TRUE);
    DosWrite(file, buf, strlen(buf), &usBytes);
    }

  i = strlen(buf);
  if (buf[i-1] != '\n')
    {
    buf[0] = '\r';
    buf[1] = '\n';
    DosWrite(file, buf, 2, &usBytes);
    }

  DosClose(file);
}


extern HWND hNoteDlg;

void CmdCopyNotes()
{
  char	 buf[256];
  register int i;


  if (hNoteDlg)
    SendMessage(hNoteDlg, WM_USER, FALSE, 0L);

  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = nTermMaxCol;
    nSelectY1 = nVssCurLinePos;
    nSelectY2 = nTermWndY + nVssCurLinePos;
    if (nSelectY2 > TERM_MAXROW)
      nSelectY2 = TERM_MAXROW;
    }

  if (nSelectY1 > nSelectY2)
    {
    i = nSelectY1;
    nSelectY1 = nSelectY2;
    nSelectY2 = i;

    i = nSelectX1;
    nSelectX1 = nSelectX2;
    nSelectX2 = i;
    }

  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, TRUE);
    AppendToNotes(buf, strlen(buf));
    }

  i = strlen(buf);
  if (buf[i-1] != '\n')
    {
    buf[0] = '\r';
    buf[1] = '\n';
    AppendToNotes(buf, 2);
    }

  if (hNoteDlg)
    SendMessage(hNoteDlg, WM_USER, TRUE, 0L);

  DataChanged();
}

void CmdCopyPrinter()
{
  char	 buf[256];
  register int i;


  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = nTermMaxCol;
    nSelectY1 = nVssCurLinePos;
    nSelectY2 = nTermWndY + nVssCurLinePos;
    if (nSelectY2 > TERM_MAXROW)
      nSelectY2 = TERM_MAXROW;
    }

  if (nSelectY1 > nSelectY2)
    {
    i = nSelectY1;
    nSelectY1 = nSelectY2;
    nSelectY2 = i;

    i = nSelectX1;
    nSelectX1 = nSelectX2;
    nSelectX2 = i;
    }

  OpenPrinter();

  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, FALSE);
    PrintLine(buf);
    }

  ClosePrinter();

  // 00046 If the printer is still open then flush it.
  if (bPrinterOn)
    FlushPrinter();
}

void CmdCopySend()
{
  char	 buf[256];
  register int i;


  if (nSelectY1 > nSelectY2)
    {
    i = nSelectY1;
    nSelectY1 = nSelectY2;
    nSelectY2 = i;

    i = nSelectX1;
    nSelectX1 = nSelectX2;
    nSelectX2 = i;
    }

  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    register j;

    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, FALSE);
    j = strlen(buf);

    if (i != nSelectY2)
      {
      buf[j] = ' ';
      j++;
      buf[j] = NULL;
      }

    CommDataOutput(buf, j);
    }
}


static void near GetSelectedLine(line, ptr, X1, Y1, X2, Y2, fCRLF)
  int  line;
  char *ptr;
  int  X1, Y1, X2, Y2;
  char fCRLF;
{
  int  *pInt, nMask;
  register i, j;


  if (Sess.fGraphics)
    nMask = 0x00FF;
  else
    nMask = 0x007F;

  line -= nVssCurLinePos;
  Y1   -= nVssCurLinePos;
  Y2   -= nVssCurLinePos;

  if (line == Y1)
    {
    if (line == Y2)
      {
      if (X1 > X2)
	{
	i  = X1;
	X1 = X2;
	X2 = i;
	}
      }
    else
      {
      X2 = nTermMaxCol;
      }
    }
  else if (line == Y2)
    {
    X1 = 1;
    }
  else
    {
    X1 = 1;
    X2 = nTermMaxCol;
    }

  pInt = VssGetLine(line);
  for (i = 0, j = X1; j <= X2; j++)
    ptr[i++] = (char)(pInt[j] & nMask);

  if (line != Y2)
    while (ptr[i-1] == ' ' && i > 0)
      i--;

  if (X2 == nTermMaxCol && fCRLF)
    {
    ptr[i++] = '\r';
    ptr[i++] = '\n';
    }

  ptr[i++] = NULL;
}

extern HWND hNoteDlg;
extern char szNotes[1024];

static void near AppendToNotes(buf, len)
  char *buf;
  int  len;
{
  register int i;


  if (hNoteDlg)
    SendMessage(hNoteDlg, WM_USER, FALSE, 0L);

  i = strlen(szNotes);

  while (len-- && i < sizeof(szNotes)-1)
    szNotes[i++] = *buf++;

  szNotes[i] = NULL;

  if (hNoteDlg)
    SendMessage(hNoteDlg, WM_USER, TRUE, 0L);

  fDataChanged = TRUE;
}
