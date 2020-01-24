/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Printer Support (PRINT.C)                                        */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "lexis.h"
#include "comm.h"
#include "data.h"
#include "diary.h"
#include "session.h"
#include "term.h"
#include "library.h"
#include "dialogs.h"

  HANDLE GetPrinterDC();
  BOOL FAR PASCAL AbortProc(HDC, int);
  BOOL FAR PASCAL AbortDlg(HWND, unsigned, WORD, LONG);
  FARPROC lpAbortDlg, lpAbortProc;

  static void near pascal PrintHeader();
  static void near pascal PrintFooter();
  static void near pascal BuildFormatLine(char *, BYTE *);

  BOOL bAbort;                  /* FALSE if user cancels printing */
  int  IOStatus;
  HWND hAbortDlgWnd;
  BOOL fNoPrinter;

  HDC  hPrintDC;
  int  CharPrintWidth;
  int  CharPrintHeight;
  int  ExtPrintLeading;
  int  nPrintX;
  int  nPrintY;
  int  nPrintLineNo;
  int  nPrintPageNo;

  char cPrintBuf[256];
  char szPrintDate[16];
  char szPrintTime[16];

static int nGatePrinter;

  BOOL DlgPrint(HWND, unsigned, WORD, LONG);
  BOOL DlgPageSetup(HWND, unsigned, WORD, LONG);
  BOOL DlgPrintSetup(HWND, unsigned, WORD, LONG);
  BOOL DlgPrintDoc(HWND, unsigned, WORD, LONG);


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPageSetup()
{
  FARPROC lpDlg;

  lpDlg = MakeProcInstance((FARPROC)DlgPageSetup, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PAGESETUP), hWnd, lpDlg);
  FreeProcInstance(lpDlg);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPrint()
{
  FARPROC lpDlg;
  register i;


  lpDlg = MakeProcInstance((FARPROC)DlgPrint, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PRINT), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  if (i)
    {
    OFSTRUCT OfStruct;
    int extfile;
    char szFileNameIn[256], szExt[16];

    strcpy(szFileNameIn, "");
    if (nWorkLastPrint == 0)
      {
      strcpy(szExt, "\\*.REC");
      SetFilePath(szDataDirRecord);
      }
    else if (nWorkLastPrint == 1)
      {
      strcpy(szExt, "\\*.HST");
      SetFilePath(szDataDirMaps);
      }
    else if (nWorkLastPrint == 2)
      {
      strcpy(szExt, "\\*.NTS");
      SetFilePath(szDataDirNotepad);
      }
    else
      {
      strcpy(szExt, "\\*.DOC");
      SetFilePath(szDataDirDoc);
      }


    if (DlgOpen(hWndInst, hWnd, IDD_OPEN_PRINT, &OfStruct, &extfile,
            szFileNameIn, szExt, szAppName) == NOOPEN)
      {
      extfile = -1;
      return;
      }
    else
      {
      HDC hPr;                      /* handle for printer device context     */
      int LineSpace;                /* spacing between lines                 */
      int LinesPerPage;             /* lines per page                        */
      int CurrentLine;              /* current line                          */
      char aBuf[256];
      int  cBuf;
      int  iBuf;
      char aLine[80];               /* buffer to store lines before printing */
      int  cLine;
      TEXTMETRIC TextMetric;        /* information about character size      */
      POINT PhysPageSize;           /* information about the page            */
      FARPROC lpAbortDlg, lpAbortProc;


      PrinterOff();
      hPr = GetPrinterDC();
      if (!hPr)
        {
        sprintf(aBuf, "Cannot print %s", szFileNameIn);
        MessageBox(hWnd, aBuf, NULL, MB_OK | MB_ICONQUESTION);
        close(extfile);
        return;
        }

      strcpy(cPrintBuf, szFileNameIn);
      lpAbortDlg =  MakeProcInstance(AbortDlg, hWndInst);
      lpAbortProc = MakeProcInstance(AbortProc, hWndInst);

      /* Define the abort function */

      Escape(hPr, SETABORTPROC, NULL,
          (LPSTR) (long) lpAbortProc,
          (LPSTR) NULL);

      if (Escape(hPr, STARTDOC, 4, "Lexis", (LPSTR) NULL) < 0)
        {
        MessageBox(hWnd, "Unable to start print job",
                   NULL, MB_OK | MB_ICONQUESTION);
        FreeProcInstance(AbortDlg);
        FreeProcInstance(AbortProc);
        DeleteDC(hPr);
        close(extfile);
        return;
        }

      bAbort = FALSE; /* Clears the abort flag  */

      /* Create the Abort dialog box (modeless) */

      hAbortDlgWnd = CreateDialog(hWndInst, "AbortDlg",
          hWnd, lpAbortDlg);

      /* Disable the main window to avoid reentrancy problems */

      EnableWindow(hWnd, FALSE);

      _strdate(szPrintDate);
      _strtime(szPrintTime);

      /* Since you may have more than one line, you need to
       * compute the spacing between lines.  You can do that by
       * retrieving the height of the characters you are printing
       * and advancing their height plus the recommended external
       * leading height.
       */

      GetTextMetrics(hPr, &TextMetric);
      LineSpace = TextMetric.tmHeight +
          TextMetric.tmExternalLeading;

      /* Since you may have more lines than can fit on one
       * page, you need to compute the number of lines you can
       * print per page.  You can do that by retrieving the
       * physical dimensions of the page and dividing the height
       * by the line spacing.
       */

      Escape(hPr, GETPHYSPAGESIZE, NULL, (LPSTR) NULL,
          (LPSTR) &PhysPageSize);
      LinesPerPage = PhysPageSize.y / LineSpace;
      LinesPerPage = 61;


      /* Keep track of the current line on the current page */

      nPrintPageNo = 1;
      CurrentLine  = 1;

      /* One way to output one line at a time is to retrieve
       * one line at a time from the edit control and write it
       * using the TextOut function.  For each line you need to
       * advance one line space.  Also, you need to check for the
       * end of the page and start a new page if necessary.
       */

      IOStatus = 0;
      cBuf  = 0;
      iBuf  = 0;
      cLine = 0;
      while (!IOStatus && extfile != -1)
        {
        if (CurrentLine == 1)
          {
          char buf[256];

          BuildFormatLine(buf, szDataHeader);

          if (buf[0])
            {
            TextOut(hPr, 0, CurrentLine*LineSpace, buf, strlen(buf));
            CurrentLine += 2;
            }
          }

        // Build a line to print.
        while (1)
          {
          if (iBuf == cBuf)
            {
            if ((cBuf = read(extfile, aBuf, sizeof(aBuf))) <= 0)
              {
              close(extfile);
              extfile = -1;
              break;
              }
            else
              iBuf = 0;
            }

          if (aBuf[iBuf] == ASC_FF)
            break;

          if (aBuf[iBuf] == ASC_LF || aBuf[iBuf] == 26)
            {
            iBuf++;
            break;
            }

          if (aBuf[iBuf] >= ' ')
            {
            if (cLine < sizeof(aLine))
              aLine[cLine++] = aBuf[iBuf];
            }
          iBuf++;
          }

        TextOut(hPr, 0, CurrentLine*LineSpace, (LPSTR)aLine, cLine);
        cLine = 0;

        if (aBuf[iBuf] == ASC_FF)
          {
          iBuf++;
          CurrentLine = LinesPerPage - 4;
          }

        if (++CurrentLine >= LinesPerPage - 3)
          {
          char buf[256];

          BuildFormatLine(buf, szDataFooter);

          if (buf[0])
            TextOut(hPr, 0, 59*LineSpace, buf, strlen(buf));

          CurrentLine += 8;
          }

        if (CurrentLine > LinesPerPage )
          {
          Escape(hPr, NEWFRAME, 0, 0L, 0L);
          nPrintPageNo++;
          CurrentLine = 1;
          if (IOStatus<0 || bAbort)
            break;
          }
        }

      if (CurrentLine < LinesPerPage - 3)
        {
        char buf[256];

        BuildFormatLine(buf, szDataFooter);
        if (buf[0])
          TextOut(hPr, 0, 59*LineSpace, buf, strlen(buf));
        }

      Escape(hPr, NEWFRAME, 0, 0L, 0L);
      Escape(hPr, ENDDOC, 0, 0L, 0L);

      EnableWindow(hWnd, TRUE);

      /* Destroy the Abort dialog box */

      DestroyWindow(hAbortDlgWnd);
      FreeProcInstance(AbortDlg);
      FreeProcInstance(AbortProc);
      DeleteDC(hPr);
      if (extfile != -1)
        close(extfile);
      }
    }

  return;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPrintSetup()
  {
  FARPROC lpDlg;
  register i;


  lpDlg = MakeProcInstance((FARPROC)DlgPrintSetup, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PRINTSETUP), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  if (i == FALSE)
    ErrorMessage(IDS_MSG_NOPRINTERS);
  }


BOOL PrintDocument()
  {
  FARPROC lpDlg;
  register i;

  SetFilePath(szDataDirDoc);
  lpDlg = MakeProcInstance((FARPROC)DlgPrintDoc, hWndInst);
  i = DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PRINTDOC), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  return (i);
  }



/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPrinter()
{
  TogglePrinter();
}


void PrinterOn()
  {
  if (!bPrinterOn)
    {
    if (OpenPrinter())
      {
      bPrinterOn = TRUE;
      CheckMenuItem(hWndMenu, CMD_PRINTER, MF_CHECKED);
      }
    }
  }


void PrinterOff()
  {
  if (bPrinterOn)
    {
    bPrinterOn = FALSE;
    CheckMenuItem(hWndMenu, CMD_PRINTER, MF_UNCHECKED);
    ClosePrinter();
    }
  }


void TogglePrinter()
{
  if (!bPrinterOn)
    {
    if (OpenPrinter())
      {
      bPrinterOn = TRUE;
      CheckMenuItem(hWndMenu, CMD_PRINTER, MF_CHECKED);
      }
    }
  else
    {
    bPrinterOn = FALSE;
    CheckMenuItem(hWndMenu, CMD_PRINTER, MF_UNCHECKED);
    ClosePrinter();
    }
}

void PrintWriteData(char *pBuffer, int cBuffer)
  {
  if (hPrintDC == NULL)
    return;

  if (nPrintX + cBuffer > sizeof(cPrintBuf))
    cBuffer = sizeof(cPrintBuf) - nPrintX;

  memcpy(&cPrintBuf[nPrintX], pBuffer, cBuffer);
  nPrintX += cBuffer;
  }

void PrintWriteNewline()
  {
  register i;


  if (hPrintDC == NULL)
    return;

  if (nPrintLineNo >= 60)
    FlushPrinter();
  else
    {
    for (i = 0; i < 71; i++)
      {
      if (cPrintBuf[i] != ' ')
        break;
      }
    if (i == 71)
      {
      if (memcmp("PAGE ", &cPrintBuf[71], 5) == 0)
        FlushPrinter();
      }
    }

  if (nPrintLineNo == 0)
    PrintHeader();

  TextOut(hPrintDC, 0, nPrintY, cPrintBuf, nPrintX + 1);

  nPrintX = 0;
  memset(cPrintBuf, ' ', sizeof(cPrintBuf));
  nPrintLineNo++;
  nPrintY += CharPrintHeight;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void PrintChar(cChar, nXPos)
  char cChar;
  int  nXPos;
{

  if (hPrintDC == NULL || nXPos >= sizeof(cPrintBuf) - 1)
    return;

  if (nXPos > nPrintX) nPrintX = nXPos;

  cPrintBuf[nXPos] = cChar;
}


/*---------------------------------------------------------------------------*/
/*									     */
/*---------------------------------------------------------------------------*/
void PrintLine(pStr)
  PSTR pStr;
{

  if (hPrintDC == NULL)
    return;

  if (nPrintLineNo >= 60)
    FlushPrinter();

  if (nPrintLineNo == 0)
    PrintHeader();

  if (pStr == NULL)
    {
    TextOut(hPrintDC, 0, nPrintY, cPrintBuf, nPrintX + 1);

    nPrintX = 0;
    memset(cPrintBuf, ' ', sizeof(cPrintBuf));
    }
  else
    {
    TextOut(hPrintDC, 0, nPrintY, pStr, strlen(pStr));
    }

  nPrintLineNo++;
  nPrintY += CharPrintHeight;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL FlushPrinter()
{
  short     iError;

  if (hPrintDC == NULL || nPrintLineNo == 0)
    return FALSE;

  PrintFooter();

  nPrintY = CharPrintHeight;
  nPrintLineNo = 0;

  iError = Escape(hPrintDC, NEWFRAME, NULL, NULL, NULL);
  nPrintPageNo++;

  if (iError < 0)
    {
    if (bWorkPrintDoc && !fTermCancel)
       fTermCancel = TRUE;

    PrinterOff();

    /* PrintError(iError); */
    return FALSE;
    }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL OpenPrinter()
{
  char buf[40];
  char *pch;
  char *pchFile;
  char *pchPort;
  char fileandport[128];
  TEXTMETRIC Metrics;
  short sDraftMode;
  POINT ptPhyPage;


  if (nGatePrinter != 0)
    {
    nGatePrinter++;
    return nGatePrinter;
    }

  GetProfileString("Windows", "Device", "", fileandport, 40);

  for( pch = fileandport; *pch && *pch != ','; ++pch ) { }

  if (*pch) *pch++=0;
    while( *pch  &&  *pch <= ' ' )  pch++;
    pchFile = pch;
    while( *pch  &&  *pch != ','  &&  *pch > ' ' )  pch++;
    if( *pch )	*pch++ = 0;
    while( *pch  &&  (*pch <= ' ' || *pch == ',') )  pch++;
    pchPort = pch;
    while( *pch  &&  *pch > ' ' )  pch++;
    *pch = 0;

  if (!(hPrintDC = CreateDC(pchFile, fileandport, pchPort, NULL)))
    {
CantPrint:
    if (hPrintDC)
      DeleteDC(hPrintDC);

    ErrorMessage(MSGPRT_NOPRINTER);

    hPrintDC = NULL;
    return FALSE;
    }

  GetTextMetrics(hPrintDC, (LPTEXTMETRIC)&Metrics);
  CharPrintHeight = Metrics.tmHeight + Metrics.tmExternalLeading;
  ExtPrintLeading = Metrics.tmExternalLeading;
  CharPrintWidth  = Metrics.tmAveCharWidth;   /* the average width */

  Escape(hPrintDC, GETPHYSPAGESIZE, NULL, (LPSTR)NULL, (LPSTR)&ptPhyPage);

  lpAbortDlg =  MakeProcInstance(AbortDlg, hWndInst);
  lpAbortProc = MakeProcInstance(AbortProc, hWndInst);

  bAbort = FALSE;
  Escape(hPrintDC, SETABORTPROC, NULL,
      (LPSTR) (long) lpAbortProc,
      (LPSTR) NULL);

  if (Escape(hPrintDC, STARTDOC, 5, "Lexis", NULL) < 0 )
    {
    goto CantPrint;
    }

  nPrintX = 0;
  nPrintY = CharPrintHeight;
  nPrintLineNo = 0;
  nPrintPageNo = 1;

  _strdate(szPrintDate);
  _strtime(szPrintTime);

  memset(cPrintBuf, ' ', sizeof(cPrintBuf));
  nGatePrinter++;

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL ClosePrinter()
{
  if (hPrintDC == NULL || nGatePrinter == 0)
    return FALSE;

  if (nGatePrinter > 1)
    {
    nGatePrinter--;
    return nGatePrinter;
    }

  nGatePrinter--;

  if (nPrintX)
    {
    cPrintBuf[nPrintX+1] = NULL;
    PrintLine(cPrintBuf);
    }

  if (nPrintLineNo)
    {
    PrintFooter();
    Escape(hPrintDC, NEWFRAME, NULL, NULL, NULL);
    }

  Escape(hPrintDC, ENDDOC, NULL, NULL, NULL);

  DeleteDC(hPrintDC);
  hPrintDC = NULL;

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
static void near pascal PrintHeader()
  {
  char buf[256];


  if (bWorkPrintDoc)
    {
    nPrintLineNo = 0;
    nPrintY = 0;
    return;
    }

  BuildFormatLine(buf, szDataHeader);

  if (buf[0])
    {
    TextOut(hPrintDC, 0, 0, buf, strlen(buf));
    nPrintLineNo += 3;
    nPrintY = CharPrintHeight * 3;
    }
  else
    {
    nPrintLineNo = 0;
    nPrintY = 0;
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
static void near pascal PrintFooter()
{
  char buf[256];


  if (bWorkPrintDoc)
    return;

  BuildFormatLine(buf, szDataFooter);
  if (buf[0])
    {
    nPrintLineNo = 59;
    nPrintY = CharPrintHeight * 59;

    TextOut(hPrintDC, 0, nPrintY, buf, strlen(buf));
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
static void near pascal BuildFormatLine(char *szLine, BYTE *szFormat)
  {
  char buf[256], *ptr;
  short cFormat;
  register i;


  if ((cFormat = strlen(szFormat)) == 0)
    {
    *szLine = NULL;
    return;
    }

  ptr = buf;
  for (i = 0; i < cFormat; i++)
    {
    if (szFormat[i] == '&')
      {
      i++;
      switch (toupper(szFormat[i]))
	{
        case 'C':
          strcpy(ptr, szWorkClient);
          ptr += strlen(szWorkClient);
	  break;

	case 'D':
	  strcpy(ptr, szPrintDate);
	  ptr += 8;
	  break;

	case 'T':
	  strcpy(ptr, szPrintTime);
	  ptr += 8;
	  break;

	case 'P':
	  {
	  char tmp[16];

	  itoa(nPrintPageNo, tmp, 10);
	  strcpy(ptr, tmp);
	  ptr += strlen(tmp);
	  }
	  break;

	case '&':
	  *ptr++ = '&';
	  break;

	case NULL:
	  break;

	default:
	  *ptr++ = '&';
          *ptr++ = szFormat[i];
	  break;
	}
      }
    else
      {
      *ptr++ = szFormat[i];
      }
    }
  *ptr = NULL;

  if (strlen(buf) >= 80)
    {
    i = 0;
    buf[80] = '\0';
    }
  else
    {
    for (i = 0; i < (80 - strlen(buf)) / 2; i++)
      szLine[i] = ' ';
    }
  strcpy(&szLine[i], buf);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL InitPrinter()
  {
  hPrintDC = GetPrinterDC();
  if (hPrintDC)
    {
    DeleteDC(hPrintDC);
    fNoPrinter = FALSE;
    }
  else
    {
    fNoPrinter = TRUE;
    EnableMenuItem(hWndMenu, CMD_PRINT,      MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_PRINTER,    MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_PAGESETUP,  MF_GRAYED);
    EnableMenuItem(hWndMenu, CMD_PRINTSETUP, MF_GRAYED);
    }

  hPrintDC     = NULL;
  nGatePrinter = 0;

  return TRUE;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL TermPrinter()
{
  if (nGatePrinter)
    ClosePrinter();

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL DlgPageSetup(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  register i;

  switch (message)
    {
    case WM_INITDIALOG:
      SetDlgItemLength(hDlg, ID_PRINT_HEADER, sizeof(szDataHeader)-1);
      SetDlgItemLength(hDlg, ID_PRINT_FOOTER, sizeof(szDataFooter)-1);

      SetDlgItemText(hDlg, ID_PRINT_HEADER, szDataHeader);
      SetDlgItemText(hDlg, ID_PRINT_FOOTER, szDataFooter);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          GetDlgItemText(hDlg, ID_PRINT_HEADER, szDataHeader,
			 sizeof(szDataHeader));
          GetDlgItemText(hDlg, ID_PRINT_FOOTER, szDataFooter,
			 sizeof(szDataFooter));
          DataUpdate();
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  EndDialog(hDlg, FALSE);
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL DlgPrint(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{

  switch (message)
    {
    case WM_INITDIALOG:
      {
      register i;

      if (nWorkLastPrint == 0)
        i = ID_PRINT_RECORD;
      else if (nWorkLastPrint == 1)
        i = ID_PRINT_DIARY;
      else if (nWorkLastPrint == 2)
        i = ID_PRINT_NOTEPAD;
      else
        i = ID_PRINT_DOCUMENT;
      CheckDlgButton(hDlg, i, TRUE);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          if (IsDlgButtonChecked(hDlg, ID_PRINT_RECORD))
            nWorkLastPrint = 0;
          else if (IsDlgButtonChecked(hDlg, ID_PRINT_DIARY))
            nWorkLastPrint = 1;
          else if (IsDlgButtonChecked(hDlg, ID_PRINT_NOTEPAD))
            nWorkLastPrint = 2;
          else
            nWorkLastPrint = 3;

          EndDialog(hDlg, TRUE);
	  break;

	case ID_CANCEL:
	  EndDialog(hDlg, FALSE);
          break;

        case ID_PRINT_RECORD:
        case ID_PRINT_DIARY:
        case ID_PRINT_NOTEPAD:
        case ID_PRINT_DOCUMENT:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_PRINT_RECORD, ID_PRINT_DOCUMENT, wParam);
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL DlgPrintSetup(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static int hLibrary = 0;
  FARPROC lpfnPrinter;
  char szName[16], szList[256], szEntry[96], szOutput[64], szNull[1], buf[96];
  short sIndex;
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
      {
      short iPrinters;

      szName[0] = 'd';
      szName[1] = 'e';
      szName[2] = 'v';
      szName[3] = 'i';
      szName[4] = 'c';
      szName[5] = 'e';
      szName[6] = 's';
      szName[7] = NULL;
      szNull[0] = NULL;
      GetProfileString(szName, NULL, szNull, szList, sizeof(szList));

      iPrinters = 0;
      for (i = 0; i < sizeof(szList);)
	{
	if (szList[i] == NULL)
	  break;
	GetProfileString(szName, &szList[i], szNull, buf, sizeof(buf));
	if (buf[0])
	  {
          register j, k;

	  strcpy(szEntry, &szList[i]);
	  j = strlen(szEntry);
	  szEntry[j++] = ' ';
	  szEntry[j++] = 'o';
	  szEntry[j++] = 'n';
	  szEntry[j++] = ' ';

          for (k = 0; buf[k] && buf[k] != ','; k++);
          while (buf[k] == ',')
            {
            register m;

            k++;
            strcpy(&szEntry[j], &buf[k]);
            for (m = 0; szEntry[m]; m++)
              {
              if (szEntry[m] == ',')
                {
                szEntry[m] = '\0';
                break;
                }
              }

            SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, 0, (long)(LPSTR)szEntry);
            iPrinters++;

            for (; buf[k] && buf[k] != ','; k++);
            }
          }

	while(szList[i++]);
	}

      if (iPrinters == 0)
        {
        EndDialog(hDlg, FALSE);
        break;
        }

      szName[0] = 'w';
      szName[1] = 'i';
      szName[2] = 'n';
      szName[3] = 'd';
      szName[4] = 'o';
      szName[5] = 'w';
      szName[6] = 's';
      szName[7] = NULL;

      szEntry[0] = 'd';
      szEntry[1] = 'e';
      szEntry[2] = 'v';
      szEntry[3] = 'i';
      szEntry[4] = 'c';
      szEntry[5] = 'e';
      szEntry[6] = NULL;

      GetProfileString(szName, szEntry, szNull, buf, sizeof(buf));
      strcpy(szEntry, buf);
      for (i = 0; buf[i] && buf[i] != ','; i++);
      buf[i++] = ' ';
      buf[i++] = 'o';
      buf[i++] = 'n';
      buf[i++] = ' ';
      buf[i++] = '\0';
      i = strlen(szEntry);
      while (i && szEntry[i-1] != ',')
        i--;
      strcat(buf, &szEntry[i]);

      SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
          sIndex = (short)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L);
          if (sIndex != LB_ERR)
	    {
            SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, sIndex, (DWORD)(LPSTR)buf);
	    i = strlen(buf);
	    while (buf[--i] != ' ');
	    while (buf[--i] != ' ');
            buf[i] = '\0';

	    szName[0] = 'd';
	    szName[1] = 'e';
	    szName[2] = 'v';
	    szName[3] = 'i';
	    szName[4] = 'c';
	    szName[5] = 'e';
	    szName[6] = 's';
            szName[7] = '\0';
            szNull[0] = '\0';
	    GetProfileString(szName, buf, szNull, szEntry, sizeof(szEntry));
            for (i = 0; szEntry[i]; i++)
              {
              if (szEntry[i] == ',')
                {
                szEntry[i] = '\0';
                break;
                }
              }

	    i = strlen(buf);
	    buf[i++] = ',';
	    strcpy(&buf[i], szEntry);

            SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, sIndex, (DWORD)(LPSTR)szEntry);
            i = strlen(szEntry);
            while (i >= 0 && szEntry[i-1] != ' ')
              i--;
            strcat(buf, ",");
            strcat(buf, &szEntry[i]);

	    szName[0] = 'w';
	    szName[1] = 'i';
	    szName[2] = 'n';
	    szName[3] = 'd';
	    szName[4] = 'o';
	    szName[5] = 'w';
	    szName[6] = 's';
	    szName[7] = NULL;

	    szEntry[0] = 'd';
	    szEntry[1] = 'e';
	    szEntry[2] = 'v';
	    szEntry[3] = 'i';
	    szEntry[4] = 'c';
	    szEntry[5] = 'e';
	    szEntry[6] = NULL;

	    WriteProfileString(szName, szEntry, buf);
	    }

	  if (hLibrary >= 32)
	    {
	    FreeLibrary(hLibrary);
	    hLibrary = 0;
	    }
	  EndDialog(hDlg, TRUE);
	  break;

	case IDCANCEL:
	  if (hLibrary >= 32)
	    {
	    FreeLibrary(hLibrary);
	    hLibrary = 0;
	    }
          EndDialog(hDlg, TRUE);
	  break;

	case ID_LISTBOX:
	  break;

	case ID_MORE:
	  i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L);
	  if (i != LB_ERR)
	    {
	    SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, i, (DWORD)(LPSTR)szEntry);
	    i = strlen(szEntry);
	    while (szEntry[--i] != ' ');
	    while (szEntry[--i] != ' ');
	    szEntry[i] = NULL;

	    szName[0] = 'd';
	    szName[1] = 'e';
	    szName[2] = 'v';
	    szName[3] = 'i';
	    szName[4] = 'c';
	    szName[5] = 'e';
	    szName[6] = 's';
	    szName[7] = NULL;
	    szNull[0] = NULL;
	    GetProfileString(szName, szEntry, szNull, buf, sizeof(buf));

	    for (i = 0; buf[i] && buf[i] != ',' && buf[i] != ' '; i++);
	    for (; buf[i] && (buf[i] == ',' || buf[i] == ' '); i++);
	    strcpy(szOutput, &buf[i]);

	    for (i = 0; buf[i] && buf[i] != ',' && buf[i] != ' '; i++);
	    buf[i++] = '.';
	    buf[i++] = 'D';
	    buf[i++] = 'R';
	    buf[i++] = 'V';
	    buf[i]   = NULL;

	    if (hLibrary >= 32)
	      FreeLibrary(hLibrary);
	    if ((hLibrary = LoadLibrary(buf)) >= 32)
	      {
	      lpfnPrinter = GetProcAddress(hLibrary, "DEVICEMODE");
	      if (lpfnPrinter != NULL)
		{
		(*lpfnPrinter)((HWND)hDlg, (HANDLE)hLibrary, (LPSTR)szEntry, (LPSTR)szOutput);
		}
	      }
	    }
	  break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL DlgPrintDoc(HWND hDlg, unsigned msg, WORD wParam, LONG lParam)
  {
  switch (msg)
    {
    case WM_INITDIALOG:
      {
      long lTime;
      char szBuf[32], szDoc[16];

      time(&lTime);
      strcpy(szBuf, ctime(&lTime));
      szDoc[ 0] = szBuf[4];
      szDoc[ 1] = szBuf[5];
      szDoc[ 2] = szBuf[6];
      szDoc[ 3] = szBuf[8];
      szDoc[ 4] = szBuf[9];
      szDoc[ 5] = szBuf[0];
      szDoc[ 6] = szBuf[1];
      szDoc[ 7] = szBuf[2];
      szDoc[ 8] = '.';
      szDoc[ 9] = 'D';
      szDoc[10] = 'O';
      szDoc[11] = 'C';
      szDoc[12] = '\0';

      strupr(szDoc);
      SetDlgItemText(hDlg, ID_PRINT_DOCUMENT, szDoc);
      CheckDlgButton(hDlg, ID_PRINT_FILE, TRUE);
      CheckDlgButton(hDlg, ID_PRINT_APPEND, TRUE);

      if (fNoPrinter)
        EnableWindow(GetDlgItem(hDlg, ID_PRINT_PRINTER), FALSE);
      }
      break;

    case WM_COMMAND:
      switch (wParam)
	{
        case ID_OK:
          if (!fCommCarrier)
            {
            bWorkStoreDoc = FALSE;
            bWorkPrintDoc = FALSE;
            EndDialog(hDlg, FALSE);
            SessTellDisconnect(FALSE);
            break;
            }

          if (IsDlgButtonChecked(hDlg, ID_PRINT_FILE))
            {
            char szPath[64];
            register i;

            GetDlgItemText(hDlg, ID_PRINT_DOCUMENT, szPath, sizeof(szPath));
            if (strchr(szPath, '.') == NULL)
              strcat(szPath, ".DOC");

            if (!ValidateFilename(szPath))
              {
              MessageDisplay(IDS_MSG_BADNAME, MB_ICONHAND);
              break;
              }

            if (IsDlgButtonChecked(hDlg, ID_PRINT_APPEND))
              {
              i = _dos_creatnew(szPath, _A_NORMAL, &hWorkDocument);
              if (i)
                {
                i = _dos_open(szPath, O_RDWR, &hWorkDocument);
                lseek(hWorkDocument, 0L, 2);
                }
              }
            else
              {
              if (i = _dos_creatnew(szPath, _A_NORMAL, &hWorkDocument))
                {
                if (i)
                  {
                  if (!IsDlgButtonChecked(hDlg, ID_PRINT_ERASE))
                    if (MessageDisplay(IDS_MSG_FILEXIST, MB_OKCANCEL | MB_ICONQUESTION) != ID_OK)
                      break;

                  i = _dos_creat(szPath, _A_NORMAL, &hWorkDocument);
                  }
                }
              }

            if (i)
              {
              ErrorMessage(IDS_MSG_BADDOCFILE);
              break;
              }
            else
              strcpy(szWorkDocument, szPath);

            DiaryDownload(szWorkDocument);

            bWorkStoreDoc = TRUE;
            }
          else
            bWorkStoreDoc = FALSE;

          if (IsDlgButtonChecked(hDlg, ID_PRINT_PRINTER))
            {
            bWorkPrintDoc = TRUE;
            PrinterOn();
	    if (!bPrinterOn)
	      {
	      bWorkStoreDoc = FALSE;
	      bWorkPrintDoc = FALSE;
	      DosClose(hWorkDocument);
	      break;
	      }
            DiaryDownload("Printer");
            }
          else
            bWorkPrintDoc = FALSE;

          EndDialog(hDlg, TRUE);
	  break;

        case ID_CANCEL:
        case ID_PRINT_STOP:
          bWorkStoreDoc = FALSE;
          bWorkPrintDoc = FALSE;
	  EndDialog(hDlg, FALSE);
          break;

        case ID_PRINT_FILE:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            if (IsDlgButtonChecked(hDlg, ID_PRINT_FILE))
              {
              EnableWindow(GetDlgItem(hDlg, ID_PRINT_DOCUMENT), TRUE);
              EnableWindow(GetDlgItem(hDlg, ID_PRINT_APPEND), TRUE);
              EnableWindow(GetDlgItem(hDlg, ID_PRINT_ERASE), TRUE);
              }
            else
              {
              EnableWindow(GetDlgItem(hDlg, ID_PRINT_DOCUMENT), FALSE);
              EnableWindow(GetDlgItem(hDlg, ID_PRINT_APPEND), FALSE);
              EnableWindow(GetDlgItem(hDlg, ID_PRINT_ERASE), FALSE);
              }

            if (IsDlgButtonChecked(hDlg, ID_PRINT_PRINTER) ||
                IsDlgButtonChecked(hDlg, ID_PRINT_FILE))
              EnableWindow(GetDlgItem(hDlg, ID_OK), TRUE);
            else
              EnableWindow(GetDlgItem(hDlg, ID_OK), FALSE);
            }
          break;

        case ID_PRINT_PRINTER:
          if (HIWORD(lParam) == BN_CLICKED)
            {
            if (IsDlgButtonChecked(hDlg, ID_PRINT_PRINTER) ||
                IsDlgButtonChecked(hDlg, ID_PRINT_FILE))
              EnableWindow(GetDlgItem(hDlg, ID_OK), TRUE);
            else
              EnableWindow(GetDlgItem(hDlg, ID_OK), FALSE);
            }
          break;

        case ID_PRINT_APPEND:
        case ID_PRINT_ERASE:
          if (HIWORD(lParam) == BN_CLICKED)
            CheckRadioButton(hDlg, ID_PRINT_APPEND, ID_PRINT_ERASE, wParam);
          break;

	default:
	  return FALSE;
	  break;
	}
      break;

    default:
      return FALSE;
      break;
    }

  return TRUE;
}


/****************************************************************************

    FUNCTION: GetPrinterDC()

    PURPOSE:  Get hDc for current device on current output port according to
	      info in WIN.INI.

    COMMENTS:

	Searches WIN.INI for information about what printer is connected, and
	if found, creates a DC for the printer.

	returns
	    hDC > 0 if success
	    hDC = 0 if failure

****************************************************************************/

HANDLE GetPrinterDC()
{
    char pPrintInfo[80];
    LPSTR lpTemp;
    LPSTR lpPrintType;
    LPSTR lpPrintDriver;
    LPSTR lpPrintPort;

    if (!GetProfileString("windows", "Device", (LPSTR)"", pPrintInfo, 80))
	return (NULL);
    lpTemp = lpPrintType = pPrintInfo;
    lpPrintDriver = lpPrintPort = 0;
    while (*lpTemp) {
	if (*lpTemp == ',') {
	    *lpTemp++ = 0;
	    while (*lpTemp == ' ')
		lpTemp = AnsiNext(lpTemp);
	    if (!lpPrintDriver)
		lpPrintDriver = lpTemp;
	    else {
		lpPrintPort = lpTemp;
		break;
	    }
	}
	else
	    lpTemp = AnsiNext(lpTemp);
    }

    return (CreateDC(lpPrintDriver, lpPrintType, lpPrintPort, (LPSTR) NULL));
}

/****************************************************************************

    FUNCTION: AbortProc()

    PURPOSE:  Processes messages for the Abort Dialog box

****************************************************************************/

int AbortProc(hPr, Code)
HDC hPr;			    /* for multiple printer display contexts */
int Code;			    /* printing status			     */
{
    MSG msg;

    /* Process messages intended for the abort dialog box */

    while (!bAbort && PeekMessage(&msg, NULL, NULL, NULL, TRUE))
	if (!IsDialogMessage(hAbortDlgWnd, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}

    /* bAbort is TRUE (return is FALSE) if the user has aborted */

    return (!bAbort);
}

/****************************************************************************

    FUNCTION: AbortDlg(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for printer abort dialog box

    MESSAGES:

	WM_INITDIALOG - initialize dialog box
	WM_COMMAND    - Input received

    COMMENTS

	This dialog box is created while the program is printing, and allows
	the user to cancel the printing process.

****************************************************************************/

int AbortDlg(hDlg, msg, wParam, lParam)
HWND hDlg;
unsigned msg;
WORD wParam;
LONG lParam;
{
    switch(msg) {

	/* Watch for Cancel button, RETURN key, ESCAPE key, or SPACE BAR */

	case WM_COMMAND:
	    return (bAbort = TRUE);

	case WM_INITDIALOG:

	    /* Set the focus to the Cancel box of the dialog */

	    SetFocus(GetDlgItem(hDlg, IDCANCEL));
            SetDlgItemText(hDlg, ID_EDIT, cPrintBuf);
	    return (TRUE);
	}
    return (FALSE);
}
