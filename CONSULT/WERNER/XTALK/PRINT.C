/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Print Module (PRINT.C)                                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/11/89 00013 On printer error the print option in Windx  *
*                                   will be turned off.                       *
*             1.01 09/12/89 00037 Corrected problem with spooler=off.         *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "sess.h"
#include "dialogs.h"


  // 00037  Corrected problem with spooler=off.
  BOOL AbortProc(HDC, int);
  BOOL AbortDlg(HWND, unsigned, WORD, LONG);
  FARPROC lpAbortDlg, lpAbortProc;


  static void near PrintHeader();
  static void near PrintFooter();
  static void near BuildFormatLine(char *, BYTE *);

  // 00037  Corrected problem with spooler=off.
  BOOL bAbort;                  /* FALSE if user cancels printing */
  BOOL bPrintError;		/* TRUE if we had a printer error */
  int  IOStatus;
  HWND hAbortDlgWnd;

  short sPrintGate = 0;
  short sPrintTerm = 0;

  HDC   hPrintDC;
  int   CharPrintWidth;
  int   CharPrintHeight;
  int   ExtPrintLeading;
  int   nPrintX;
  int   nPrintY;
  int   nPrintLineNo;
  int   nPrintPageNo;

  char cPrintBuf[133];
  char szPrintDate[10];
  char szPrintTime[10];

static int nGatePrinter;

  BOOL DlgPrint(HWND, unsigned, WORD, LONG);
  BOOL DlgPageSetup(HWND, unsigned, WORD, LONG);
  BOOL DlgPrintSetup(HWND, unsigned, WORD, LONG);


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPageSetup()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_PAGESETUP;

  lpDlg = MakeProcInstance((FARPROC)DlgPageSetup, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PAGESETUP), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPrint()
{
  TogglePrinter();
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
void CmdPrintSetup()
{
  FARPROC lpDlg;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_PRINTERSETUP;

  lpDlg = MakeProcInstance((FARPROC)DlgPrintSetup, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_PRINTSETUP), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  wHelpDialog = NULL;
  InfoMessage(NULL);
}

void TogglePrinter()
{
  if (!bPrinterOn)
    {
    if (OpenPrinter())
      {
      bPrinterOn = TRUE;
      CheckMenuItem(hWndMenu, CMD_PRINT, MF_CHECKED);
      }
    }
  else
    {
    if (sPrintGate)
      {
      sPrintTerm = TRUE;
      }
    else
      {
      bPrinterOn = FALSE;
      CheckMenuItem(hWndMenu, CMD_PRINT, MF_UNCHECKED);
      ClosePrinter();
      }
    }
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
  register i;

  if (hPrintDC == NULL)
    return;

  if (nPrintLineNo >= 58)
    {
    if (FlushPrinter() == FALSE)
      return;
    }

  if (nPrintLineNo == 0)
    PrintHeader();

  if (pStr == NULL)
    {
    i = TextOut(hPrintDC, 0, nPrintY, cPrintBuf, nPrintX + 1);
    nPrintX = 0;
    memset(cPrintBuf, ' ', sizeof(cPrintBuf));
    }
  else
    {
    i = TextOut(hPrintDC, 0, nPrintY, pStr, strlen(pStr));
    }

  nPrintLineNo++;
  nPrintY += CharPrintHeight;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL FlushPrinter()
{
  register i;


  if (hPrintDC == NULL || nPrintLineNo == 0)
    return FALSE;

  sPrintGate++;

  PrintFooter();

  nPrintY = 0;
  nPrintLineNo = 0;

  if ((i = Escape(hPrintDC, NEWFRAME, NULL, NULL, NULL)) < 0)
    {
    ErrorMessage(MSGPRT_PRINTERR);
    sPrintGate--;
    bPrintError = TRUE;
    TogglePrinter();    // 00013  Toggle off the printer on an error.
    return (FALSE);
    }

  nPrintPageNo++;

  sPrintGate--;

  if (sPrintTerm)
    {
    if (bPrinterOn)
      TogglePrinter();
    return (FALSE);
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

  // 00037  Corrected problem with spooler=off.
  lpAbortDlg =  MakeProcInstance(AbortDlg, hWndInst);
  lpAbortProc = MakeProcInstance(AbortProc, hWndInst);

  // 00037  Corrected problem with spooler=off.
  bAbort = FALSE;
  Escape(hPrintDC, SETABORTPROC, NULL,
      (LPSTR) (long) lpAbortProc,
      (LPSTR) NULL);

  if (Escape(hPrintDC, STARTDOC, 5, "XTalk", NULL) < 0 )
    {
    goto CantPrint;
    }

  nPrintX = 0;
  nPrintY = 0;
  nPrintLineNo = 0;
  nPrintPageNo = 1;

  sPrintTerm  = 0;
  bPrintError = FALSE;

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

  if (nPrintLineNo && !bPrintError)
    {
    PrintFooter();
    Escape(hPrintDC, NEWFRAME, NULL, NULL, NULL);
    }

  Escape(hPrintDC, ENDDOC, NULL, NULL, NULL);

  DeleteDC(hPrintDC);
  hPrintDC = NULL;

  // 00037  Corrected problem with spooler=off.
  FreeProcInstance(AbortDlg);
  FreeProcInstance(AbortProc);

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
static void near PrintHeader()
{
  char buf[192];


  BuildFormatLine(buf, Sess.szHeader);

  if (buf[0])
    {
    TextOut(hPrintDC, 0, nPrintY, buf, strlen(buf));

    nPrintLineNo += 3;
    nPrintY += CharPrintHeight * 3;
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
static void near PrintFooter()
{
  char buf[192];


  BuildFormatLine(buf, Sess.szFooter);
  if (buf[0])
    {
    nPrintLineNo = 58;
    nPrintY = CharPrintHeight * 58;

    TextOut(hPrintDC, 0, nPrintY, buf, strlen(buf));
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
static void near BuildFormatLine(szLine, szFormat)
  char *szLine;
  BYTE *szFormat;
{
  char buf[256], *ptr;
  register i;


  if (szFormat[0] == 0)
    {
    *szLine = NULL;
    return;
    }

  ptr = buf;
  for (i = 0; i < szFormat[0]; i++)
    {
    if (szFormat[i+1] == '&')
      {
      i++;
      switch (toupper(szFormat[i+1]))
	{
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

	case 'F':
	  memcpy(ptr, &Sess.szDescription[1], Sess.szDescription[0]);
	  ptr += Sess.szDescription[0];
	  break;

	case '&':
	  *ptr++ = '&';
	  break;

	case NULL:
	  break;

	default:
	  *ptr++ = '&';
	  *ptr++ = szFormat[i+1];
	  break;
	}
      }
    else
      {
      *ptr++ = szFormat[i+1];
      }
    }
  *ptr = NULL;

  for (i = 0; i < (80 - strlen(buf)) / 2; i++)
    szLine[i] = ' ';
  strcpy(&szLine[i], buf);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/
BOOL InitPrinter()
{
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
      SetDlgItemLength(hDlg, ID_PRINT_HEADER, sizeof(Sess.szHeader)-1);
      SetDlgItemLength(hDlg, ID_PRINT_FOOTER, sizeof(Sess.szFooter)-1);

      SetDlgItemData(hDlg, ID_PRINT_HEADER, Sess.szHeader);
      SetDlgItemData(hDlg, ID_PRINT_FOOTER, Sess.szFooter);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
	  GetDlgItemData(hDlg, ID_PRINT_HEADER, Sess.szHeader,
			 sizeof(Sess.szHeader));
	  GetDlgItemData(hDlg, ID_PRINT_FOOTER, Sess.szFooter,
			 sizeof(Sess.szFooter));
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
BOOL DlgPrintSetup(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  static int hLibrary = 0;
  FARPROC lpfnPrinter;
  char szName[16], szList[256], szEntry[96], szOutput[64], szNull[1], buf[96];
  register i;


  switch (message)
    {
    case WM_INITDIALOG:
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

      for (i = 0; i < sizeof(szList);)
	{
	if (szList[i] == NULL)
	  break;
	GetProfileString(szName, &szList[i], szNull, buf, sizeof(buf));

	if (buf[0])
	  {
	  register j;

	  strcpy(szEntry, &szList[i]);
	  j = strlen(szEntry);
	  szEntry[j++] = ' ';
	  szEntry[j++] = 'o';
	  szEntry[j++] = 'n';
	  szEntry[j++] = ' ';
	  szEntry[j++] = NULL;

	  for (j = 0; buf[j] && buf[j] != ','; j++);
          while (buf[j] == ',')
            {
            char *ptr;

            j++;
            strcpy(szOutput, szEntry);
            strcat(szOutput, &buf[j]);
            if (ptr = strchr(szOutput, ','))
              *ptr = '\0';
            SendDlgItemMessage(hDlg, ID_LISTBOX, LB_ADDSTRING, 0, (long)(LPSTR)szOutput);
            while (buf[j] && buf[j] != ',')
              j++;
            }
	  }
	while(szList[i++]);
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
      for (i = 0; buf[i] && buf[i] != ','; i++);
      buf[i] = NULL;

      SendDlgItemMessage(hDlg, ID_LISTBOX, LB_SELECTSTRING, -1, (DWORD)(LPSTR)buf);
      break;

    case WM_COMMAND:
      switch (wParam)
	{
	case IDOK:
	  i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETCURSEL, 0, 0L);
	  if (i != LB_ERR)
	    {
	    SendDlgItemMessage(hDlg, ID_LISTBOX, LB_GETTEXT, i, (DWORD)(LPSTR)buf);
	    i = strlen(buf);
	    while (buf[--i] != ' ');
	    while (buf[--i] != ' ');
	    buf[i] = NULL;

	    szName[0] = 'd';
	    szName[1] = 'e';
	    szName[2] = 'v';
	    szName[3] = 'i';
	    szName[4] = 'c';
	    szName[5] = 'e';
	    szName[6] = 's';
	    szName[7] = NULL;
	    szNull[0] = NULL;
	    GetProfileString(szName, buf, szNull, szEntry, sizeof(szEntry));
	    i = strlen(buf);
	    buf[i++] = ',';
	    strcpy(&buf[i], szEntry);

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
	  EndDialog(hDlg, FALSE);
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

  // 00037  Corrected problem with spooler=off.
/*
**
**  FUNCTION: AbortProc()
**
**  PURPOSE:  Processes messages for the Abort Dialog box
**
*/
BOOL AbortProc(HDC hPr, int Code)
  {
  MSG msg;

  while (PeekMessage(&msg, NULL, NULL, NULL, TRUE))
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }

  return (1);
  }

/*
**
**  FUNCTION: AbortDlg(HWND, unsigned, WORD, LONG)
**
**  PURPOSE:  Processes messages for printer abort dialog box
**
**  MESSAGES:
**
**      WM_INITDIALOG - initialize dialog box
**      WM_COMMAND    - Input received
**
**  COMMENTS
**
**      This dialog box is created while the program is printing, and allows
**      the user to cancel the printing process.
**
*/

BOOL AbortDlg(hDlg, msg, wParam, lParam)
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
