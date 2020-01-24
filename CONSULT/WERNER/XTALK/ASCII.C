/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   ASCII File Tansfer (ASCII.C)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module perform ascii text upload to various type os remote *
*             computers.                                                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 08/02/89 00002 Fix word wrapping of words longer than the  *
*				  column wrap position. 		      *
*             1.01 08/09/89 00008 Speed up ASCII uploads.                     *
*	      1.01 12/25/89 00072 Slow down ASCII uploads alittle.	      *
*	      1.01 12/25/89 00073 Correct Tabwidth and LWAIT Count options.   *
*             1.01 01/19/90 00087 Reset error code to remove past errors.     *
*             1.01 01/19/90 00090 Check for cTabWidth being equal to zero.    *
*             1.01 01/22/90 00094 Clear old error code.                       *
*             1.01 02/20/90 dca00026 JDB Added SendMessage to speed up the    *
*                           ascii upload with char or line delay              *
*	1.1 04/08/90 dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "xfer.h"
#include "comm.h"
#include "term.h"
#include "properf.h"
#include "variable.h"
#include "prodata.h"
#include "dialogs.h"

#define SEND_NEXT_LINE	    1
#define SEND_NEXT_CHAR	    2
#define LINE_WAIT_ECHO	    3
#define LINE_WAIT_PROMPT    4
#define LINE_WAIT_COUNT     5
#define LINE_WAIT_DELAY     6
#define CHAR_WAIT_ECHO	    7
#define CHAR_WAIT_DELAY     8
#define XFER_TERM           9


BOOL DlgASCII(HWND, unsigned, WORD, LONG);
static void near GetNextLine();
static void near UpdateIndicators();


//
//  XferASCIIUploadName()
//
//
int XferASCIIUploadName(char *szName)
  {
  int	   hFile;
  FILESTATUS fsts;
  OFSTRUCT OfStruct;
  char	   szPath[MAX_PATH];
  register i;


  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

	// dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
  if (i = SetFullPath(VAR_DIRFIL))
    return (0);

  if (szName == NULL || *szName == NULL)
    {
    if ((hFile = XferUploadFile("\\*.TXT", &OfStruct)) <= 0)
      return (ERRSYS_OPENFAULT);
    }
  else
    {
    if (strchr(szName, ':') || strchr(szName, '\\'))
      strcpy(szPath, szName);
    else
      CreatePath(szPath, VAR_DIRFIL, szName);
    if ((hFile = OpenFile(szPath, (LPOFSTRUCT)&OfStruct, OF_READ)) <= 0)
      return(ERRSYS_OPENFAULT);
    }

  memset(&filexfer.ascii, 0, sizeof(filexfer.ascii));
  filexfer.ascii.hFile = hFile;
  strcpy(szXferFilePath, OfStruct.szPathName);
  lXferFileBytes = 0;

  DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;

  if (lXferFileSize == 0L)
    return (ERRSYS_OPENFAULT);

  lpXferDlg = MakeProcInstance((FARPROC)DlgASCII, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_ASCII), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  // 00094 Clear old error code.
  return (nErrorCode = 0);
  }


//
//  XferASCIIUploadFile()
//
int XferASCIIUploadFile(int hFile)
  {
  FILESTATUS fsts;

  if (wSystemState == SS_PROCESS)
    return (ERRXFR_BUSY);

  //  00087 Reset error code to remove past errors.
  nErrorCode = 0;

  memset(&filexfer.ascii, 0, sizeof(filexfer.ascii));
  filexfer.ascii.hFile = hFile;
  lXferFileBytes = 0;

  DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
  lXferFileSize = fsts.cbFile;

  if (lXferFileSize == 0L)
    return (ERRSYS_OPENFAULT);

  lpXferDlg = MakeProcInstance((FARPROC)DlgASCII, hWndInst);
  hXferDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_ASCII), hWnd, lpXferDlg);
  LinkDialog(hXferDlg);

  SetSystemStatus(SS_XFER);

  // 00094 Clear old error code.
  return (nErrorCode = 0);
  }


void XferASCIITerm()
{
  CommPortDump();
  CommFlushBuffers();

  if (filexfer.ascii.hFile != -1)
    {
    DosClose(filexfer.ascii.hFile);
    filexfer.ascii.hFile = -1;
    }

  if (szXferFilePath[0] != NULL)
    {
    if (*StripPath(szXferFilePath) == '~')
      DosDelete(szXferFilePath, 0L);
    }

  if (hXferDlg)
    {
    DestroyWindow(hXferDlg);
    UnLinkDialog(hXferDlg);
    hXferDlg = NULL;
    }

  if (lpXferDlg)
    {
    FreeProcInstance(lpXferDlg);
    lpXferDlg = NULL;
    }

  ResetSystemStatus(SS_XFER);
  InfoDisplay(NULL);
  IconTextChanged(NULL);

  if (!IfSystemStatus(SS_SCRIPT))
    {
    MessageBeep(0);
    if (nErrorCode)
      GeneralError(nErrorCode);
    else
      {
      if (IsIconic(hWnd))
	InfoMessage(MSGXFR_ALLDONE);
      }
    }
}


BOOL DlgASCII(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  HBRUSH hBrush;
  RECT	 Rect;
  HDC	 hDC;
  char	 buf[256];
  int	 i, j;


  switch (message)
    {
    case WM_INITDIALOG:
      hXferDlg = hDlg;

      if (*StripPath(szXferFilePath) == '~')
	SetDlgItemStr(hDlg, ID_XFER_NAME, MSGXFR_ASCIITEMP);
      else
	SetDlgItemText(hDlg, ID_XFER_NAME, szXferFilePath);

      InfoDisplayMsg(MSGXFR_ASCIISND);
      filexfer.ascii.lTime = GetCurrentTime();
      SetDlgItemLong(hDlg, ID_XFER_SIZE, lXferFileSize, FALSE);

      UpdateIndicators();

      filexfer.ascii.nState = SEND_NEXT_LINE;
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_CANCEL:
          nErrorCode = ERRXFR_LOCALCANCEL;
	  XferASCIITerm();
	  break;

	case ID_XFER_GRAPHICS:
	  if (HIWORD(lParam) == BN_PAINT)
	    {
	    GetClientRect(LOWORD(lParam), &Rect);
	    hBrush = CreateSolidBrush(RGB(255, 0, 0));
	    hDC = GetDC(LOWORD(lParam));

	    i = Rect.right;
	    Rect.right = (int)(((long)Rect.right * lXferFileBytes) / lXferFileSize);
	    FillRect(hDC, &Rect, hBrush);
	    DeleteObject(hBrush);

	    Rect.left  = Rect.right;
	    Rect.right = i;
	    FillRect(hDC, &Rect, GetStockObject(WHITE_BRUSH));

	    ReleaseDC(LOWORD(lParam), hDC);
	    }
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    case WM_TIMER:
      {
      short iCycles;	// 00072 Lets not take too much time.


      iCycles = 8;
      if (!bConnected)
	{
	nErrorCode = ERRXFR_LOSTCONNECT;
	XferASCIITerm();
	break;
	}

      switch (filexfer.ascii.nState)
	{
        case SEND_NEXT_LINE:
DoAnotherLine:
	  if (filexfer.ascii.fEOF)
            {
            filexfer.ascii.nState = XFER_TERM;
            filexfer.ascii.lTimeOut = GetCurrentTime() + 1000L;
	    break;
	    }
	  else if (ComStat.cbOutQue > nCommPortMemory / 2)
	    {
	    SetDlgItemStr(hDlg, ID_XFER_MSG, MSGXFR_ASCIIBUFF);
	    break;
	    }
	  else
	    {
	    GetNextLine();
	    if (filexfer.ascii.length)
	      {
	      if (Xfer.cCWait == XFER_WAIT_NONE)
		{
		CommDataOutput(filexfer.ascii.line, filexfer.ascii.length);
		if (Term.fEcho)
		  SendMessage(hChildTerminal, UM_DATA,
			      filexfer.ascii.length,
			      (LONG)((LPSTR)filexfer.ascii.line));
		lXferFileBytes += filexfer.ascii.length;
		switch (Xfer.cLWait)
		  {
		  case XFER_WAIT_NONE:
		    CommDataRead(buf, sizeof(buf));
		    break;

		  case XFER_WAIT_ECHO:
		    filexfer.ascii.pindex = 0;
		    filexfer.ascii.plength = (int)Xfer.szLineDelim[0];
		    memcpy(filexfer.ascii.prompt, &Xfer.szLineDelim[1],
			   (int)Xfer.szLineDelim[0]);

		    filexfer.ascii.lTimeOut = GetCurrentTime() +
					      (long)Xfer.cLineTime * 1000L;
		    filexfer.ascii.nState = LINE_WAIT_ECHO;
		    break;

		  case XFER_WAIT_PROMPT:
		    filexfer.ascii.pindex = 0;
		    filexfer.ascii.plength = (int)Xfer.szLWaitPrompt[0];
		    memcpy(filexfer.ascii.prompt, &Xfer.szLWaitPrompt[1],
			   (int)Xfer.szLWaitPrompt[0]);

		    filexfer.ascii.lTimeOut = GetCurrentTime() +
					      (long)Xfer.cLineTime * 1000L;
		    filexfer.ascii.nState = LINE_WAIT_PROMPT;
		    break;

		  case XFER_WAIT_COUNT:
		    filexfer.ascii.count = Xfer.cLWaitCount;
		    filexfer.ascii.lTimeOut = GetCurrentTime() +
					      (long)Xfer.cLineTime * 1000L;
		    filexfer.ascii.nState = LINE_WAIT_COUNT;

		    filexfer.ascii.fSkip  = FALSE;  // 00073
		  // 00073 filexfer.ascii.fSkip  = TRUE;
		    break;

		  case XFER_WAIT_DELAY:
		    filexfer.ascii.lTimeOut = GetCurrentTime()+Xfer.nLWaitDelay;
		    filexfer.ascii.nState = LINE_WAIT_DELAY;
		    break;
		  }

		SetDlgItemStr(hDlg, ID_XFER_MSG, MSGXFR_ASCIISEND);
		}
	      else
		{
		filexfer.ascii.index  = 0;
		filexfer.ascii.nState = SEND_NEXT_CHAR;
		}
	      UpdateIndicators();
              }
            // 00008 If we are still in line mode send up another line.
            if (filexfer.ascii.nState == SEND_NEXT_LINE)
	      {
	      // 00072 Lets not hog the whole system up.
	      if (iCycles-- > 0)
		goto DoAnotherLine;
	      }
	    }
	  break;

	case SEND_NEXT_CHAR:
	  if (filexfer.ascii.index >= filexfer.ascii.length)
	    {
	    filexfer.ascii.nState = SEND_NEXT_LINE;
	    }
	  else
	    {
            filexfer.ascii.nEchoCh = filexfer.ascii.line[filexfer.ascii.index];
	    CommDataOutput(&filexfer.ascii.line[filexfer.ascii.index++], 1);
	    if (Term.fEcho)
	      SendMessage(hChildTerminal, UM_DATA, 1,
			  (LONG)((LPSTR)&filexfer.ascii.line[filexfer.ascii.index-1]));
	    lXferFileBytes++;
	    switch (Xfer.cCWait)
	      {
	      case XFER_WAIT_NONE:
		CommDataRead(buf, sizeof(buf));
		break;

	      case XFER_WAIT_ECHO:
                filexfer.ascii.lTimeOut = GetCurrentTime() + 500L;
                filexfer.ascii.nState  = CHAR_WAIT_ECHO;
		break;

	      case XFER_WAIT_DELAY:
		filexfer.ascii.lTimeOut = GetCurrentTime() + Xfer.nCWaitDelay;
		filexfer.ascii.nState = CHAR_WAIT_DELAY;
		break;
	      }

	    SetDlgItemStr(hDlg, ID_XFER_MSG, MSGXFR_ASCIISEND);
	    UpdateIndicators();
	    }
	  break;

	case LINE_WAIT_ECHO:
	case LINE_WAIT_PROMPT:
	  if (GetCurrentTime() > filexfer.ascii.lTimeOut)
	    filexfer.ascii.nState = SEND_NEXT_LINE;
	  i = CommDataRead(buf, sizeof(buf));
	  for (j = 0; j < i; j++)
	    {
	    if (buf[j] == filexfer.ascii.prompt[filexfer.ascii.pindex++])
	      {
	      if (filexfer.ascii.pindex == filexfer.ascii.plength)
		filexfer.ascii.nState = SEND_NEXT_LINE;
	      }
	    else
	      {
	      filexfer.ascii.pindex = 0;
	      }
	    }
	  break;

	case LINE_WAIT_COUNT:
	  if (GetCurrentTime() > filexfer.ascii.lTimeOut)
	    filexfer.ascii.nState = SEND_NEXT_LINE;
	  if ((i = CommDataRead(buf, sizeof(buf))) > 0)
	    {
	    if (filexfer.ascii.fSkip)
	      {
	      for (j = 0; j < i; j++)
		{
		if (buf[j] == ASC_CR || buf[j] == ASC_LF)
		  {
		  filexfer.ascii.fSkip = FALSE;
		  j++;
		  break;
		  }
		}
	      i -= j;
	      }
	    filexfer.ascii.count -= i;
	    if (filexfer.ascii.count <= 0)
	      filexfer.ascii.nState = SEND_NEXT_LINE;
	    }
	  break;

	case LINE_WAIT_DELAY:
	  CommDataRead(buf, sizeof(buf));
	  if (GetCurrentTime() > filexfer.ascii.lTimeOut)
	  { // dca00026 JDB
	    filexfer.ascii.nState = SEND_NEXT_LINE;
     	    SendMessage(hDlg,WM_TIMER,0,0L); // dca00026 JDB
	  } // dca00026 JDB
	  break;

	case CHAR_WAIT_ECHO:
          {
          int nLen, i;

	  if (GetCurrentTime() > filexfer.ascii.lTimeOut)
	    filexfer.ascii.nState = SEND_NEXT_CHAR;
          nLen = CommDataRead(buf, sizeof(buf));
          for (i = 0; i < nLen; i++)
            {
            if (buf[i] == (char)filexfer.ascii.nEchoCh)
              {
              filexfer.ascii.nState = SEND_NEXT_CHAR;
              break;
              }
            }
          }
	  break;

	case CHAR_WAIT_DELAY:
	  CommDataRead(buf, sizeof(buf));
	  if (GetCurrentTime() > filexfer.ascii.lTimeOut)
	  {	 //dca00026  JDB
	    filexfer.ascii.nState = SEND_NEXT_CHAR;
     	    SendMessage(hDlg,WM_TIMER,0,0L); //dca00026 JDB
	  }	 // dca00026 JDB
	  break;

        case XFER_TERM:
          if (ComStat.cbOutQue == 0)
            {
            if (GetCurrentTime() > filexfer.ascii.lTimeOut)
              XferASCIITerm();
            }
          else
            filexfer.ascii.lTimeOut = GetCurrentTime() + 1000L;
          break;
	}
      }
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


/*---------------------------------------------------------------------------*/
/* GetNextLine - Get the next line of input from the input file.  Expand the */
/*		 tabs and use blankex option, if required.  This routine may */
/*		 look sort-of junky, but because speed and code size is not  */
/*		 problem here I rather save memory (overall) and have more   */
/*		 control over the get line logic (vs using the C library).   */
/*---------------------------------------------------------------------------*/
static void near GetNextLine()
{
  ULONG ulPos;
  FILESTATUS fsts;
  USHORT usBytes;
  char buf[256], *ptr;
  long lPos;
  int  word;
  register i, j;


  DosChgFilePtr(filexfer.ascii.hFile, 0L, FILE_CURRENT, &lPos);
  if (DosRead(filexfer.ascii.hFile, buf, sizeof(buf)-1, &usBytes))
    usBytes = 0;

  if (usBytes == 0)
    {
    filexfer.ascii.length = 0;
    filexfer.ascii.fEOF = TRUE;
    return;
    }
  buf[usBytes] = 26;			    /* Add a software EOF */

  for (i = 0, j = 0; i < usBytes; i++)
    {
    if (buf[i] == 26)
      {
      break;
      }
    else if (buf[i] == '\r')
      {
      if (j == 0 && Xfer.szBlankEx[0])
	{
	j = (int)(Xfer.szBlankEx[0]);
	memcpy(filexfer.ascii.line, &Xfer.szBlankEx[1], j);
	}

      i++;
      if (i < usBytes && buf[i] == '\n')
	i++;
      if (i <= usBytes && buf[1] == 26)
	filexfer.ascii.fEOF = TRUE;
      break;
      }
    else if (buf[i] == '\t')
      {
      if (Xfer.fTabEx && Term.cTabWidth) // 00090 Check for zero TabWidth.
	{
	memset(&filexfer.ascii.line[j], ' ', Term.cTabWidth);
	j = (j / Term.cTabWidth + 1) * Term.cTabWidth; // 00073
	}
      else
	{
	filexfer.ascii.line[j++] = '\t';
	}

      if (Xfer.cWordWrap != 0 && j >= Xfer.cWordWrap)
	{
	i++;
	break;
	}
      }
    else if (iscntrl(buf[i]))
      {
      /* We don't want control characters */
      }
    else
      {
      if (Xfer.cWordWrap != 0)
	{
	if (buf[i] == ' ' && j >= Xfer.cWordWrap)
	  {
	  i++;
	  break;
	  }
	else if (buf[i] == ' ')
	  {
	  filexfer.ascii.line[j++] = ' ';
	  }
	else
	  {
	  // Compute the end of the next word, thus (WORD - I) == the length.
	  for (word = i; !isspace(buf[word]) && !iscntrl(buf[word]); word++);
	  // 00002 Fix wordwrap of too big of a word.
	  // 00002 if (j + (word - i) > Xfer.cWordWrap)
	  if ((j + (word - i) > Xfer.cWordWrap) && j)
	    {
	    break;  // Since the next word would exceed wordwrap lets flush
		    //	 the current line buffer (since j != 0).
	    }
	  else
	    {
	    if (word < sizeof(filexfer.ascii.line) - 16)
	      {
	      // 00002 Fix wordwrap of too big of a word.
	      // 00002 while (!isspace(buf[i]) && !iscntrl(buf[i]))
	      while (!isspace(buf[i]) && !iscntrl(buf[i]) && (j < Xfer.cWordWrap))
		filexfer.ascii.line[j++] = buf[i++];
	      i--;
	      }
	    else
	      {
	      filexfer.ascii.line[j++] = buf[i];
	      }
	    }
	  }
	}
      else
	{
	filexfer.ascii.line[j++] = buf[i];
	}
      }

    if (j > sizeof(filexfer.ascii.line) - 16)
      {
      i++;
      break;
      }
    }

  while (j > 2 && filexfer.ascii.line[j-1] == ' ')
    j--;

  memcpy(&filexfer.ascii.line[j], &Xfer.szLineDelim[1], (int)(Xfer.szLineDelim[0]));
  j += (int)(Xfer.szLineDelim[0]);

  if (buf[i] != 26)
    DosChgFilePtr(filexfer.ascii.hFile, lPos + i, FILE_BEGIN, &ulPos);

  filexfer.ascii.length = j;

  filexfer.ascii.lines++;
}

static void near UpdateIndicators()
{
  SetDlgItemLong(hXferDlg, ID_XFER_BYTES, lXferFileBytes, FALSE);
  SetDlgItemInt(hXferDlg, ID_XFER_LINES, filexfer.ascii.lines, FALSE);
  InvalidateRect(GetDlgItem(hXferDlg, ID_XFER_GRAPHICS), NULL, TRUE);
  UpdateWindow(GetDlgItem(hXferDlg, ID_XFER_GRAPHICS));
}
