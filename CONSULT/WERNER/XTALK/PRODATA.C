/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Data Module (PRODATA.C)                                         *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains logic to process the profile and data      *
*             structure of.                                                   *
*                                                                             *
*  Revisions:								      *
*  1.00 07/17/89 Initial Version.                              		      *
*	1.01 08/03/89 00004    Auto open capture and printer.   	      *
*  1.01 09/05/89 00026    Add Zmodem to New File dialog.        	      *
*	1.01 10/26/89 00049    When a new terminal is selected from the       *
*	                       NEW.FILE dialog or the script language         *
*	                       reset the terminal params to defaults.         *
*	1.01 10/31/89 00052    Add context help to the menus and dialogs.     *
*  1.01 01/19/90 00088    Check version number.                       	      *
*  1.01 01/22/90 00095    If XWP is null then simulate a load.        	      *
*  1.01 01/23/90 00096    Add error return for FILE.SAVE.             	      *
*  1.01 01/30/90 dca00012 PJL Stop hard-coding XWP Version Number. 	      *
*  1.01 02/03/90 dca00017 PJL Fixed incorrect help text for File.Open.	      *
*  1.1  03/13/90 dca00048 MKL added COM3, COM4 support			      *
*  1.1  03/19/90 dca00053 MKL added Windows 3.0 proportional font support     *
*  1.1  03/19/90 dca00054 PJL Disallow File.New from File.Open.               *
*  1.1  03/30/90 dca00062 PJL Fix XWP extension addition problem in File.Open *
*  1.1  04/02/90 dca00065 PJL Fix Invalid/Open disk drive error reporting.	   *
*																			   *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "comm.h"
#include "disp.h"
#include "font.h"
#include "prodata.h"
#include "sess.h"
#include "term.h"
#include "xfer.h"
#include "macro.h"
#include "dialogs.h"
#include "global.h"
#include "variable.h"

struct HEADER
  {
  int  nVersion;
  char szDescription[32];
  };

  char	   fUntitled;
  char	   fDataChanged;
  char	   szDataCurFile[16];
  OFSTRUCT DataFileStruct;

  char	   cNumberLoop;

  char	   fCompileOnly;
  char	   fAlwaysCompile = FALSE;
  char	   fManualDial;
  char	   szManualNumber[64];

extern HWND hNoteDlg;
extern char szNotes[1024];

static void near SetEncoding(BYTE);
static void near EncodedRead(int, char *, int);
static void near EncodedWrite(int, char *, int);
static void near Convert(char *);
static char fConnectNow;

BOOL DlgNewFile(HWND, unsigned, WORD, LONG);


/*---------------------------------------------------------------------------*/
/*  CMDNEW() - Initialize Proteus to a new (default) Parameters.	     */
/*									     */
/*---------------------------------------------------------------------------*/
void CmdNew()
  {
  FARPROC lpDlg;

  if (DataSaveFirst() == ID_CANCEL)
    return;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_NEW;

  DataNew();

  lpDlg = MakeProcInstance((FARPROC)DlgNewFile, hWndInst);
  DialogBox(hWndInst, MAKEINTRESOURCE(IDD_NEWFILE), hWnd, lpDlg);
  FreeProcInstance(lpDlg);

  InitSystemNewData();
  UpdateTitle();

  if (fConnectNow && !bConnected)
    CmdConnect();

  wHelpDialog = NULL;
  InfoMessage(NULL);
  }


int DataNew()
{
  int  hFile;
  char szPath[MAX_PATH];
  register USHORT usResult;


  if (bCaptureOn)
    CmdCapture(0, 0);

  CreatePath(szPath, VAR_DIRXWP, "NORMAL");
  if ((hFile = OpenFile(szPath, &DataFileStruct, OF_READ)) > 0)
    {
    fUntitled    = TRUE;
    fDataChanged = FALSE;
    UpdateTitle();
    LoadString(hWndInst, IDS_UNTITLED, szDataCurFile, sizeof(szDataCurFile));
    if (usResult = DataParamLoad(hFile))
      return (usResult);
    DataFileStruct.cBytes = 0;
    InitSystemNewData();
    return (0);
    }

  fUntitled    = TRUE;
  fDataChanged = FALSE;
  UpdateTitle();

  LoadString(hWndInst, IDS_UNTITLED, szDataCurFile, sizeof(szDataCurFile));

  NewFontData();
  NewDisplayData();
  NewMacroData();
  NewProtocolData();
  NewSessionData();
  NewTerminalData();

  UpdateMacro();

  memset(szNotes, 0, sizeof(szNotes));

  return 0;
}


void CmdOpen()
  {
  int  hFile;
  char szFileName[17];
  register i;

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
  if (!IsPathAvailable(VAR_DIRXWP))
    return;

  if (DataSaveFirst() == ID_CANCEL)
    return;

  InfoMessage(MSGINF_HELP);
// dca00017 02/03/90 Wrong help text for File.Open.
  wHelpDialog = HELP_ITEM_OPEN;

  if (bCaptureOn)
    CmdCapture(0, 0);

  SetFullPath(VAR_DIRXWP);
  DataFileStruct.cBytes = 0;
  // dca00054 PJL Disallow File.New (create new file) from File.Open.
  // dca00062 PJL Fix XWP extension addition problem in File.Open.
  i = DlgOpen(hWndInst, hWnd, IDD_OPEN, &DataFileStruct, &hFile, szFileName,
              "\\*.XWP", szAppName, FALSE, TRUE);

  if (i != NOOPEN)
    {
    register USHORT usResult;

    strcpy(szDataCurFile, szFileName);

    fUntitled	 = FALSE;
    fDataChanged = FALSE;
    UpdateTitle();

    if (usResult = DataParamLoad(hFile))
      {
      GeneralError(usResult);
      return;
      }

    InitSystemNewData();

    if (MessageBox(hWnd, "Do you wish to Connect?", szAppName,
                   MB_OKCANCEL | MB_ICONQUESTION | MB_APPLMODAL) == IDOK)
      CmdConnect();
    DataAutoOpen();  // 00004 Auto Capture and Printer open.
    }

  wHelpDialog = NULL;
  InfoMessage(NULL);
  }


int DataOpen(szFileName)
  char *szFileName;
{
  int  hFile;
  char szPath[MAX_PATH], *ptr;
  register i;


  strupr(szFileName);
  CreatePath(szPath, VAR_DIRXWP, szFileName);

  if ((hFile = OpenFile(szPath, &DataFileStruct, OF_READ)) < 0)
    return (nErrorCode = ERRMIS_SETUPFILE);

  strcpy(szDataCurFile, StripPath(szPath));

  fUntitled    = FALSE;
  fDataChanged = FALSE;
  UpdateTitle();

  if (nErrorCode = DataParamLoad(hFile))
    return (nErrorCode);

  InitSystemNewData();

  DataAutoOpen();  // 00004 Auto Capture and Printer open.

  return (nErrorCode = 0);
}


void CmdSave()
{
  char szPath[MAX_PATH];
  int  hFile;


  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_SAVE;

  if (!fUntitled)
    {
    // 00096  Add error return for FILE.SAVE.
    CreatePath(szPath, VAR_DIRXWP, szDataCurFile);
    if ((hFile = OpenFile(szPath, &DataFileStruct, OF_CREATE | OF_WRITE)) != -1)
      DataParamSave(hFile);
    else
      GeneralError(ERRSYS_OPENFAULT);
    }
  else
    CmdSaveAs();

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


int DataSave()
{
  int  hFile;
  char szPath[MAX_PATH];


  if (fUntitled)
    {
    szDataCurFile[0] = 'T';
    szDataCurFile[1] = 'E';
    szDataCurFile[2] = 'M';
    szDataCurFile[3] = 'P';
    szDataCurFile[4] = '.';
    szDataCurFile[5] = 'X';
    szDataCurFile[6] = 'W';
    szDataCurFile[7] = 'P';
    szDataCurFile[8] = NULL;
    CreatePath(szPath, VAR_DIRXWP, szDataCurFile);

    if ((hFile = OpenFile(szPath, &DataFileStruct, OF_CREATE|OF_WRITE)) < 0)
      return (ERRSYS_OPENFAULT);

    DataParamSave(hFile);
    fUntitled	 = FALSE;
    fDataChanged = FALSE;
    UpdateTitle();
    }
  else
    {
    CreatePath(szPath, VAR_DIRXWP, szDataCurFile);
    if ((hFile = OpenFile(szPath, &DataFileStruct, OF_WRITE)) < 0)
      return (ERRSYS_OPENFAULT);

    DataParamSave(hFile);
    fDataChanged = FALSE;
    }

  return 0;
}


void CmdSaveAs()
{
  int  hFile;
  char szFileName[17];
  char buf[64];
  int  i;

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
  if (!IsPathAvailable(VAR_DIRXWP))
    return;

  InfoMessage(MSGINF_HELP);
  wHelpDialog = HELP_ITEM_SAVEAS;

  if (fUntitled)
    DataFileStruct.cBytes = 0;

  SetFullPath(VAR_DIRXWP);

  szFileName[0] = NULL;

  i = DlgSaveAs(hWndInst, hWnd, IDD_SAVEAS, &DataFileStruct, &hFile,
		szFileName, "\\*.XWP", szAppName, FALSE, FALSE);

  if (i != NOSAVE)
    {
    strcpy(szDataCurFile, szFileName);

    fUntitled	 = FALSE;

    if (hFile != -1)
      DataParamSave(hFile);

    fDataChanged = FALSE;
    UpdateTitle();
    }

  wHelpDialog = NULL;
  InfoMessage(NULL);
}


int DataSaveAs(name)
char *name;
{
	int  hFile;
	char *ptr;
	register i;

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
	if (!IsPathAvailable(VAR_DIRXWP))
		return(-1);

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
	if (SetFullPath(VAR_DIRXWP) || SetPath(name))
		return(-1);

	ptr = StripPath(name);
	if (strchr(ptr, '.'))
		ptr[12] = NULL;
	else
		ptr[8] = NULL;

	strcpy(szDataCurFile, StripPath(name));
	strupr(szDataCurFile);
	if (strchr(szDataCurFile, '.') == NULL)
	{
		i = strlen(szDataCurFile);
		szDataCurFile[i++] = '.';
		szDataCurFile[i++] = 'X';
		szDataCurFile[i++] = 'W';
		szDataCurFile[i++] = 'P';
		szDataCurFile[i++] = NULL;
	}

	if ((hFile = OpenFile(szDataCurFile, &DataFileStruct, OF_WRITE|OF_CREATE)) < 0)
	{
		szDataCurFile[0] = NULL;
		ErrorMessage(ERRSYS_OPENFAULT);
		return(-1);
	}

	DataParamSave(hFile);
	fUntitled    = FALSE;
	fDataChanged = FALSE;
	UpdateTitle();
	return 0;
}


void DataChanged()
{
  fDataChanged = TRUE;
}


void DataDialLoad(HWND hDlg, WORD nItem)
  {
  struct HEADER Header;
  struct find_t buffer;
  OFSTRUCT OfStruct;
  FILESTATUS fsts;
  int  hFile;
  char szExt[8], szPath[128];
  char *pStr;
  register i;


  CreatePath(szPath, VAR_DIRXWP, "*");

  i = _dos_findfirst(szPath, _A_NORMAL, &buffer);
  while (i == 0)
    {
    CreatePath(szPath, VAR_DIRXWP, buffer.name);
    if ((hFile = OpenFile(szPath, &OfStruct, OF_READ)) != -1)
      {
      // 00095 If the XWP is null then use default values.
      DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
      if (fsts.cbFile)
        {
        SetEncoding('k');
        EncodedRead(hFile, (PSTR)&Header, sizeof(Header));
        }
      else
        memset(&Header, 0, sizeof(Header));

      DosClose(hFile);

      if ((pStr = (char *)strchr(buffer.name, '.')) != NULL)
	*pStr = NULL;

      sprintf(szPath, "%-8s - %s", buffer.name, Header.szDescription);

      if (pStr)
        *pStr = '.';

      SendDlgItemMessage(hDlg, nItem, LB_ADDSTRING, 0, (DWORD)((LPSTR)szPath));
      }

    i = _dos_findnext(&buffer);
    }
  }


BOOL DataSaveFirst()
{
  int  i;
  char szFileName[16];
  char szBuf[128];


  if (fDataChanged == TRUE)
    {
    DlgMergeStrings(szSCC, szDataCurFile, szBuf);
    i = MessageBox(hWnd, szBuf, szAppName,
                   MB_YESNOCANCEL | MB_ICONQUESTION | MB_APPLMODAL);

    if (i == IDYES)
      {
      if (!fUntitled)
	CmdSave();
      else
	CmdSaveAs();
      }
    else if (i == IDNO)
      {
      fDataChanged = FALSE;
      }

    return (i);
    }
  else
    return (ID_OK);
}


//BOOL DataParamRead(PSTR pFile, BOOL fCat)
//  {
//  int  hFile;
//  char cat[64];
//
//  if (fCat)
//    CreatePath(cat, VAR_DIRXWP, pFile);
//  else
//    strcpy(cat, pFile);
//
//  if ((hFile = OpenFile(cat, &DataFileStruct, OF_READ)) != -1)
//    {
//   if (DataParamLoad(hFile))
//      return (FALSE);
//    return TRUE;
//    }
//
//  return FALSE;
//  }


//
//  DataParamLoad()
//
//  This routine loads a parameter file (via a file handle) into memory.  The
//  version number of the parameter file and used to effect the loading of the
//  data or return an error if not able to determine the format of the file.
//
//  To keep people from looking at the internals of the .XWP, a simple encoding
//  method is used to mask out the data.
//
USHORT DataParamLoad(int hFile)
  {
  FILESTATUS fsts;
  struct     HEADER Header;
  WORD       wNoteSize;


  // 00095 If the XWP is null then use default values.
  DosQFileInfo(hFile, 1, &fsts, sizeof(fsts));
  if (fsts.cbFile == 0)
    {
    DataNew();
    return (0);
    }

  SetEncoding('k');
  EncodedRead(hFile, (PSTR)&Header, sizeof(Header));

  // 00088 Check version number.
  // if (Header.nVersion != 256 && Header.nVersion != 257)
  // dca00012 PJL Stop hard-coding the XWP Version Number.  Also, use an
  //          XWP file version # rather than and EXE file version #.
  //          MIN_XWP_VER = 1st shipping, MAX_XWP_VER = latest shipping.
  if (Header.nVersion < MIN_XWP_VER || Header.nVersion > MAX_XWP_VER)
    return (ERRMIS_BADXWP);

  // dca00012 PJL Stop hard-coding the XWP Version Number.
  // if (Header.nVersion == 1 * 256 + 0)
  if (Header.nVersion == MIN_XWP_VER)
    {
    memset(&Comm, '\0', sizeof(Comm));
    EncodedRead(hFile, (PSTR)&Comm,  sizeof(struct stCommOld));
    }
  else
    EncodedRead(hFile, (PSTR)&Comm,  sizeof(Comm));

  EncodedRead(hFile, (PSTR)&Disp,  sizeof(Disp));
  EncodedRead(hFile, (PSTR)&Font,  sizeof(Font));

  // dca00012 PJL Stop hard-coding the XWP Version Number.
  // if (Header.nVersion == 1 * 256 + 0)
  if (Header.nVersion == MIN_XWP_VER)
    {
    memset(&Macro, '\0', sizeof(Macro));
    EncodedRead(hFile, (PSTR)&Macro,  sizeof(Macro) - 48);
    }
  else
    EncodedRead(hFile, (PSTR)&Macro,  sizeof(Macro));

  EncodedRead(hFile, (PSTR)&Sess,  sizeof(Sess));
  EncodedRead(hFile, (PSTR)&Term,  sizeof(Term));
  EncodedRead(hFile, (PSTR)&Xfer,  sizeof(Xfer));
  memset(szNotes, 0, sizeof(szNotes));
  EncodedRead(hFile, (PSTR)&wNoteSize, sizeof(wNoteSize));
  if (wNoteSize > sizeof(szNotes) - 1)
    wNoteSize = sizeof(szNotes) - 1;
  if (wNoteSize)
    EncodedRead(hFile, szNotes, wNoteSize);
  if (hNoteDlg)
    SendMessage(hNoteDlg, WM_USER, TRUE, 0L);

  DosClose(hFile);

  fDataChanged = FALSE;

  return (0);
  }


/*---------------------------------------------------------------------------*/
/*  Save the users parameters to the Profile (.XWP)			     */
/*---------------------------------------------------------------------------*/
void DataParamSave(hFile)
  int hFile;
{
  struct HEADER Header;
  WORD wNoteSize;


  // dca00012 PJL Stop hard-coding the XWP Version Number.
  // Header.nVersion = 1 * 256 + 1;
  Header.nVersion = MAX_XWP_VER;
  LibEnstore(Sess.szDescription, Header.szDescription,
	     sizeof(Header.szDescription));

  Sess.fPrinterOn = (char)bPrinterOn;
  Sess.fCaptureOn = (char)bCaptureOn;

  SetEncoding('k');
  EncodedWrite(hFile, (PSTR)&Header, sizeof(Header));
  EncodedWrite(hFile, (PSTR)&Comm,  sizeof(Comm));
  EncodedWrite(hFile, (PSTR)&Disp,  sizeof(Disp));
  EncodedWrite(hFile, (PSTR)&Font,  sizeof(Font));
  EncodedWrite(hFile, (PSTR)&Macro, sizeof(Macro));
  EncodedWrite(hFile, (PSTR)&Sess,  sizeof(Sess));
  EncodedWrite(hFile, (PSTR)&Term,  sizeof(Term));
  EncodedWrite(hFile, (PSTR)&Xfer,  sizeof(Xfer));

  if (wNoteSize = strlen(szNotes))
    {
    if (wNoteSize > sizeof(szNotes) - 1)
      wNoteSize = sizeof(szNotes) - 1;

    EncodedWrite(hFile, (PSTR)&wNoteSize, sizeof(wNoteSize));
    EncodedWrite(hFile, szNotes, wNoteSize);
    }
  else
    {
    wNoteSize = 0;
    EncodedWrite(hFile, (PSTR)&wNoteSize, sizeof(wNoteSize));
    }

  DosClose(hFile);

  fDataChanged = FALSE;
}

void InitSystemNewData()
{
  RECT Rect;


  if (Comm.cPort != -1)
    cCommPort = Comm.cPort;

  LoadModemParameters();

  GetClientRect(hWnd, (LPRECT)&Rect);
  SendMessage(hWnd, WM_SIZE, fMaximized ? 2 : 0,
	      MAKELONG(Rect.right - Rect.left, Rect.bottom - Rect.top));
  SendMessage(hChildTerminal, WM_SIZE, 0, 0L);

  UpdateFont();
  UpdateTerminal();
  CommPortUpdate();

  SendMessage(hChildStatus, UM_CONFIG, 0, 0L);

  UpdateMacro();

  if (hNoteDlg)
    PostMessage(hNoteDlg, WM_USER, TRUE, 0L);
  else if (Disp.fDisplayNotes && (wSystem == 0))
    CmdNotes();

  UpdateProtocol();

  IconTextChanged(NULL);
}

// 00004 This function was added for auto capture printer open fix.
/*****************************************************************************\
*  DataAutoOpen()
*
*  After a profile is loaded this routine is called to automatically open the
*    capture file and/or printer, if they were active when the profile was
*    last saved.
\*****************************************************************************/
void DataAutoOpen()
  {
  if (Sess.fPrinterOn != (char)bPrinterOn)
    TogglePrinter();

  if (Sess.fCaptureOn != (char)bCaptureOn)
    CmdCapture(0, FALSE);
  }


BOOL InitData()
{
  char szBuf[64];
  int  i;


  fUntitled    = TRUE;
  fDataChanged = FALSE;

  fManualDial  = FALSE;
  szManualNumber[0] = NULL;


  GetProfileString("extensions", "xwp", "", szBuf, sizeof(szBuf));
  if (szBuf[0] == NULL)
    {
    WriteProfileString("extensions", "xwp", "xtalk.exe ^.xwp");
    WriteProfileString("extensions", "xws", "notepad.exe ^.xws");
    }

  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*  Use a simple encode/decode system for the user's profile (.XWP)          */
/*---------------------------------------------------------------------------*/
static BYTE byEncodeByte;
static char fAbortEncoding;

static void near SetEncoding(byStart)
  BYTE byStart;
{
  byEncodeByte	 = byStart;
  fAbortEncoding = FALSE;
}


static void near EncodedRead(int hFile, char *pBuf, int nLen)
  {
  USHORT usBytes;
  BYTE byBuf[256];


  if (fAbortEncoding)
    return;

  while (nLen)
    {
    register i, j;

    if (nLen > 256)
      i = 256;
    else
      i = nLen;
    nLen -= i;

    if (DosRead(hFile, byBuf, i, &usBytes))
      {
      ErrorMessage(IDS_READ_ERROR);
      fAbortEncoding = TRUE;
      return;
      }

    for (j = 0; j < i; j++)
      {
      byBuf[j] ^= byEncodeByte;
      byEncodeByte += (char)(13 + (byEncodeByte << 2));
      }

    memcpy(pBuf, byBuf, i);
    pBuf += i;
    }
  }


static void near EncodedWrite(hFile, pBuf, nLen)
  int  hFile;
  char *pBuf;
  int  nLen;
{
  USHORT usBytes, usResults;
  BYTE byBuf[256];


  if (fAbortEncoding)
    return;

  while (nLen)
    {
    register i, j;

    if (nLen > 256)
      i = 256;
    else
      i = nLen;
    nLen -= i;

    memcpy(byBuf, pBuf, i);
    pBuf += i;

    for (j = 0; j < i; j++)
      {
      byBuf[j] ^= byEncodeByte;
      byEncodeByte += (char)(13 + (byEncodeByte << 2));
      }

    usResults = DosWrite(hFile, byBuf, i, &usBytes);
    if (usResults || i != usBytes)
      {
      ErrorMessage(IDS_WRITE_ERROR);
      fAbortEncoding = TRUE;
      return;
      }
    }
}

static void near Convert(ptr)
  char *ptr;
{
  char buf[256];
  register i, j;


  strcpy(buf, ptr);
  i = strlen(buf);
  *(ptr++) = (char)i;

  for (j = 0; j < i; j++)
    *(ptr)++ = buf[j];
}


BOOL DlgNewFile(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
	char buf[32];
	register int i;

	switch (message)
	{
	case WM_INITDIALOG:
		// dca00048 MKL added COM3,COM4 support
		if (sVersion < 3)
		{
			/* hide COM3, COM4 buttons if not running Windows 3.0 or later */
			EnableWindow(GetDlgItem(hDlg, ID_MODEM_COM3), FALSE);
			EnableWindow(GetDlgItem(hDlg, ID_MODEM_COM4), FALSE);
		}
		if (Comm.cPort >= 0)
			i = ID_MODEM_COM1 + Comm.cPort;
		else
			i = ID_MODEM_COM1 + cCommPort;
		CheckDlgButton(hDlg, i, TRUE);

		// MKL added Windows 3.0 proportional font support
		// SetDlgItemLength(hDlg, ID_EDIT, 8);
		SetDlgItemLength(hDlg, ID_EDIT, sizeof(Sess.szNumber)-1);
		SetDlgItemLength(hDlg, ID_SESSION_DESCRIPTION, sizeof(Sess.szDescription)-1);
		SetDlgItemLength(hDlg, ID_SESSION_NUMBER, sizeof(Sess.szNumber)-1);

		if (Comm.nSpeed == 110)
			i = ID_COMM_110;
		else if (Comm.nSpeed == 300)
			i = ID_COMM_300;
		else if (Comm.nSpeed == 1200)
			i = ID_COMM_1200;
		else if (Comm.nSpeed == 2400)
			i = ID_COMM_2400;
		else if (Comm.nSpeed == 9600)
			i = ID_COMM_9600;
		else
			i = ID_COMM_19200;
		CheckDlgButton(hDlg, i, TRUE);

		if (Term.cTerminal == TERM_VT102)
			i = ID_TERM_VT102;
		else if (Term.cTerminal == TERM_VT52)
			i = ID_TERM_VT52;
		else if (Term.cTerminal == TERM_IBMPC)
			i = ID_TERM_IBMPC;
		else if (Term.cTerminal == TERM_IBM3101)
			i = ID_TERM_IBM3101;
		else
			i = ID_TERM_VIDTEX;
		CheckDlgButton(hDlg, i, TRUE);

		if (Xfer.cProtocol == XFER_XTALK)
			i = ID_PROTOCOL_XTALK;
		else if (Xfer.cProtocol == XFER_DART)
			i = ID_PROTOCOL_DART;
		else if (Xfer.cProtocol == XFER_XMODEM)
			i = ID_PROTOCOL_XMODEM;
		else if (Xfer.cProtocol == XFER_CSERVEB)
			i = ID_PROTOCOL_CSERVEB;
		else if (Xfer.cProtocol == XFER_ZMODEM)  // 00026 Add Zmodem to dialog.
			i = ID_PROTOCOL_ZMODEM;
		else
			i = ID_PROTOCOL_KERMIT;
		CheckDlgButton(hDlg, i, TRUE);

		CheckDlgButton(hDlg, ID_NO, TRUE);
		fConnectNow = FALSE;
		return (TRUE);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (IsDlgButtonChecked(hDlg, ID_MODEM_COM1))
				i = 0;
			// dca00048 MKL added COM3,COM4 support
			else if (IsDlgButtonChecked(hDlg, ID_MODEM_COM2))
				i = 1;
			else if (IsDlgButtonChecked(hDlg, ID_MODEM_COM3))
				i = 2;
			else
				i = 3;
			Comm.cPort = (char)i;

			GetDlgItemData(hDlg, ID_SESSION_DESCRIPTION, Sess.szDescription,
							sizeof(Sess.szDescription));
			GetDlgItemData(hDlg, ID_SESSION_NUMBER, Sess.szNumber,
							sizeof(Sess.szNumber));

			if (IsDlgButtonChecked(hDlg, ID_COMM_110))
				Comm.nSpeed = 110;
			else if (IsDlgButtonChecked(hDlg, ID_COMM_300))
				Comm.nSpeed = 300;
			else if (IsDlgButtonChecked(hDlg, ID_COMM_1200))
				Comm.nSpeed = 1200;
			else if (IsDlgButtonChecked(hDlg, ID_COMM_2400))
				Comm.nSpeed = 2400;
			else if (IsDlgButtonChecked(hDlg, ID_COMM_9600))
				Comm.nSpeed = 9600;
			else
				Comm.nSpeed = 19200;

			if (IsDlgButtonChecked(hDlg, ID_TERM_VT102))
				i = TERM_VT102;
			else if (IsDlgButtonChecked(hDlg, ID_TERM_VT52))
				i = TERM_VT52;
			else if (IsDlgButtonChecked(hDlg, ID_TERM_IBMPC))
				i = TERM_IBMPC;
			else if (IsDlgButtonChecked(hDlg, ID_TERM_IBM3101))
				i = TERM_IBM3101;
			else
				i = TERM_VIDTEX;
			if (i != Term.cTerminal)
			{
				Term.cTerminal = (char)i;
				// 00049 Reset terminal to default parameters.
				UpdateNewTerminal();
			}

			if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_XTALK))
				Xfer.cProtocol = XFER_XTALK;
			else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_DART))
				Xfer.cProtocol = XFER_DART;
			else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_KERMIT))
				Xfer.cProtocol = XFER_KERMIT;
			else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_CSERVEB))
				Xfer.cProtocol = XFER_CSERVEB;
			else if (IsDlgButtonChecked(hDlg, ID_PROTOCOL_ZMODEM))  // 00026
				Xfer.cProtocol = XFER_ZMODEM;
			else
				Xfer.cProtocol = XFER_XMODEM;

			GetDlgItemData(hDlg, ID_SESSION_SCRIPT, Sess.szScript,
							sizeof(Sess.szScript));

			GetDlgItemText(hDlg, ID_EDIT, buf, sizeof(buf));

			// dca00065 PJL Fix Invalid/Open disk drive error reporting.
			i = 0;
			if (buf[0] > ' ')
				i = DataSaveAs(buf);
			else
				fDataChanged = TRUE;

			if (i == 0)
			{
				if (Sess.szNumber[0])
					Comm.cLocal = FALSE;

				Comm.cDataBits = 8;
				Comm.cParity   = 0;
				Comm.cStopBits = 0;

				if (IsDlgButtonChecked(hDlg, ID_YES))
					fConnectNow = TRUE;

				EndDialog(hDlg, TRUE);
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;

		case ID_MODEM_COM1:
		case ID_MODEM_COM2:
		// dca00048 MKL added COM3,COM4 support
		case ID_MODEM_COM3:
		case ID_MODEM_COM4:
			CheckRadioButton(hDlg, ID_MODEM_COM1, ID_MODEM_COM4, wParam);
			break;

		case ID_COMM_110:
		case ID_COMM_300:
		case ID_COMM_600:
		case ID_COMM_1200:
		case ID_COMM_2400:
		case ID_COMM_4800:
		case ID_COMM_9600:
		case ID_COMM_19200:
			CheckRadioButton(hDlg, ID_COMM_110, ID_COMM_19200, wParam);
			break;

		case ID_TERM_VT102:
		case ID_TERM_VT52:
		case ID_TERM_IBMPC:
		case ID_TERM_IBM3101:
		case ID_TERM_VIDTEX:
			CheckRadioButton(hDlg, ID_TERM_VT102, ID_TERM_VIDTEX, wParam);
			break;

		case ID_PROTOCOL_XTALK:
		case ID_PROTOCOL_DART:
		case ID_PROTOCOL_XMODEM:
		case ID_PROTOCOL_KERMIT:
		case ID_PROTOCOL_ZMODEM:
		case ID_PROTOCOL_CSERVEB:
			CheckRadioButton(hDlg, ID_PROTOCOL_XTALK, ID_PROTOCOL_CSERVEB, wParam);
			break;

		case ID_YES:
		case ID_NO:
			CheckRadioButton(hDlg, ID_YES, ID_NO, wParam);
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
