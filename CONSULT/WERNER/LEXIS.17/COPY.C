/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Copy to Clipboard Module (COPY.C)                                */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <fcntl.h>
#include <time.h>
#include "lexis.h"
#include "library.h"
#include "record.h"
#include "term.h"
#include "comm.h"
#include "data.h"
#include "diary.h"
#include "dialogs.h"

static void near pascal GetSelectedLine(int, PSTR, int, int, int, int, char);


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
    nSelectX2 = TERM_MAXCOL;
    nSelectY1 = 0;
    nSelectY2 = TERM_MAXROW - 1;
    if (nSelectY2 >= TERM_MAXROW)
      nSelectY2 = TERM_MAXROW - 1;
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
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, FALSE);
    len += strlen(buf) + 4;
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

    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, FALSE);
    for (j = 0; buf[j]; j++)
      *lpStr++ = buf[j];

    if (i != nSelectY2)
      {
      if (*(lpStr-1) != ' ')
        *lpStr++ = ' ';
      }
    }

  *lpStr = '\0';

  GlobalUnlock(hMem);

  if (OpenClipboard(hWnd))
    {
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


void CmdCopyDiary()
{
  char buf[256];
  long lTime;
  register int i;


  time(&lTime);
  sprintf(buf, "[Entry at %.24s] - Copy\r\n", ctime(&lTime));
  DiaryWriteData(buf, strlen(buf));

  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = TERM_MAXCOL;
    nSelectY1 = 0;
    nSelectY2 = nTermWndY;
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
    DiaryWriteData(buf, strlen(buf));
    DiaryWriteNewline();
    }
}


void CmdCopyPrinter(BOOL fFlush)
  {
  char	 buf[256];
  register int i;


  if (!fSelectedArea)
    {
    nSelectX1 = 1;
    nSelectX2 = TERM_MAXCOL;
    nSelectY1 = 0;
    nSelectY2 = TERM_MAXROW-1;
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
    PrintLine(buf);
    }

  if (fFlush)
    FlushPrinter();
  }


/*
** Subroutine:
**   CmdCopyRecording()
**
** Description:
**   Copies the current screen image to the recording file, if active.
**
** Returns:
**   None.
*/
void CmdCopyRecording()
  {
  char	 buf[256];
  register i;


  /*
  ** If the session recorder is off then just return.
  */
  if (!bRecording)
    return;

  /*
  ** Prefix each page with a formfeed to make Q&A jump with joy.
  */
  RecordWriteData("\x0C\r\n", 3);

  /*
  ** Write out the current screen image.
  */
  for (i = 0; i < TERM_MAXROW; i++)
    {
    GetSelectedLine(i, buf, 1, 0, TERM_MAXCOL, TERM_MAXROW-1, TRUE);
    RecordWriteData(buf, strlen(buf));
    }
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

    CommWriteData(buf, j);
    }
}


/*****************************************************************************\
*  ClipQuerySelection
*
*  Looks in the clipboard for a possible cite selection.  If there is text in
*  the clipboard and it is under twenty characters long then return it in the
*  specified buffer and return the length, else return 0.
\*****************************************************************************/
short ClipQuerySelection(char *szBuffer)
  {
  HANDLE hText;
  LPSTR  lpText;


  if (OpenClipboard(hWnd) == FALSE)
    return (0);

  *szBuffer = '\0';

  if ((hText = GetClipboardData(CF_TEXT)) != NULL)
    {
    if ((lpText = GlobalLock(hText)) != 0L)
      {
      if (lstrlen(lpText) < 20)
        lstrcpy(szBuffer, lpText);

      GlobalUnlock(hText);
      }
    }

  CloseClipboard();

  return (strlen(szBuffer));
  }


int TermQuerySelection(char *szBuffer)
  {
  char     buf[90];
  int      nInx;
  register i, j;


  if (!fSelectedArea)
    {
    *szBuffer = NULL;
    return (0);
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

  nInx = 0;
  for (i = nSelectY1; i <= nSelectY2; i++)
    {
    GetSelectedLine(i, buf, nSelectX1, nSelectY1, nSelectX2, nSelectY2, FALSE);

    if (nInx && szBuffer[nInx-1] != ' ')
      szBuffer[nInx++] = ' ';

    if (i == nSelectY2)
      j = nSelectX2 - nSelectX1 + 1;
    else
      j = 80;
    buf[j] = '\0';

    for (j = 0; nInx < 500 && buf[j]; j++)
      {
      if (buf[j] != ' ')
        break;
      }

    for (; nInx < 500 && buf[j]; j++)
      {
      if (!(nInx && buf[j] == ' ' && szBuffer[nInx-1] == ' '))
        szBuffer[nInx++] = buf[j];
      }
    }

  szBuffer[nInx] = '\0';
  return (nInx);
  }

int TermQueryLine(int nLine, char *szBuffer)
  {
  int  *pInt;
  register i, j;


  pInt = pPriVidLine[nLine];
  for (i = 1, j = 0; i <= 80; i++)
    {
    if ((pInt[i] & 0x7F) != ' ')
      j = i;
    *szBuffer++ = (char)(pInt[i] & 0x7F);
    }
  *szBuffer = NULL;

  return (j);
  }


void TermQueryInput(char *szBuffer)
  {
  int  *pInt;
  register i;


  pInt = pPriVidLine[0];
  for (i = 1; i < nTermChrX; i++)
    *szBuffer++ = (char)(pInt[i] & 0x7F);
  *szBuffer = NULL;
  }


int TermQueryLength(int nLine)
  {
  int  *pInt;
  register i, j;


  pInt = pPriVidLine[nLine];
  for (i = 1, j = 0; i <= 80; i++)
    {
    if ((pInt[i] & 0x7F) != ' ')
      j = i;
    }

  return (j);
  }

static void near pascal GetSelectedLine(line, ptr, X1, Y1, X2, Y2, fCRLF)
  int  line;
  char *ptr;
  int  X1, Y1, X2, Y2;
  char fCRLF;
{
  int  *pInt, nMask;
  register i, j;


  if (Term.fEightBit)
    nMask = 0x00FF;
  else
    nMask = 0x007F;

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
      X2 = TERM_MAXCOL;
      }
    }
  else if (line == Y2)
    {
    X1 = 1;
    }
  else
    {
    X1 = 1;
    X2 = TERM_MAXCOL;
    }

  pInt = pPriVidLine[line];
  for (i = 0, j = X1; j <= X2; j++)
    ptr[i++] = (char)(pInt[j] & nMask);

  if (line != Y2)
    while (ptr[i-1] == ' ' && i > 0)
      i--;

  if (X2 == TERM_MAXCOL && fCRLF)
    {
    ptr[i++] = '\r';
    ptr[i++] = '\n';
    }

  ptr[i++] = NULL;
}
