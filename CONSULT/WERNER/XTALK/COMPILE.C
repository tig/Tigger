/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Compiler (COMPILE.C)                                            *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module is more of an advance tokenizer than a true compiler*
*             but we can detect various errors at compile time, reduce the    *
*             size of the script in memory and speed up execution over a true *
*             interpreter.                                                    *
*                                                                             *
*             The compiler (super-tokernizer) get tokens from the TOKEN module*
*             and stores them in global memory array.  Thus all keywords are  *
*             converted to a single token, label are identified, strings are  *
*             encoded, etc, etc.  All the stuff that a normal tokenizer would *
*             do from the text book of writing a compiler.  The hard part is  *
*             that we pretend that we are a real compiler by keeping track of *
*             all the condition and loop statements and insert internal       *
*             pointers.  I use a simple push-down pop-up stack for storing    *
*             the 'THEN' of if statements and 'WHILE' of while loops.  So when*
*             I get to the end of the structure I can look at the stack to    *
*             find the beginning.  Of course if we end the script and still   *
*             have stuff on the stack then we got an error.                   *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/08/89 00031 Permit the backquote to work.               *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "compile.h"
#include "interp.h"
#include "prodata.h"
#include "variable.h"
#include "dialogs.h"
#include "term.h"

  HANDLE hObjectCode  = 0;
  BYTE far *lpObjectCode;
  FARPROC lpCompDlg;
  HWND    hCompDlg;


  BOOL DlgCompile(HWND, unsigned, WORD, LONG);

  static int near PopLocation(WORD);
  static int near PushLocation(WORD);
  static int near CheckLabel(char *);


BOOL NeedCompile(szScript)
  char *szScript;
{
  char szFile[32], szPath[MAX_PATH];
  unsigned uScriptDate, uScriptTime, uObjectDate, uObjectTime;
  int  file;

  register char *ptr;


  strcpy(szFile, szScript);
  if (ptr = strchr(szFile, '.'))
    *ptr = NULL;

  CreatePath(szPath, VAR_DIRXWS, szScript);
  if (_dos_open(szPath, O_RDONLY, &file))
    return (FALSE);
  _dos_getftime(file, &uScriptDate, &uScriptTime);
  DosClose(file);

  strcat(szFile, ".XWC");
  CreatePath(szPath, VAR_DIRXWS, szFile);
  if (_dos_open(szPath, O_RDONLY, &file))
    return (TRUE);
  _dos_getftime(file, &uObjectDate, &uObjectTime);
  DosClose(file);

  if (uScriptDate < uObjectDate)
    return (FALSE);
  if (uScriptDate > uObjectDate)
    return (TRUE);
  if (uScriptTime < uObjectTime)
    return (FALSE);

  return (TRUE);
}

BOOL LoadScriptObject(szScript)
  char *szScript;
{
  FILESTATUS fsts;
  char szFile[32], szPath[MAX_PATH];
  int  file;
  WORD wLen, wCopied;
  register char *ptr;


  strcpy(szFile, szScript);
  if (ptr = strchr(szFile, '.'))
    *ptr = NULL;

  if (hObjectCode)
    {
    GlobalUnlock(hObjectCode);
    GlobalFree(hObjectCode);
    hObjectCode = 0;
    }

  strcat(szFile, ".XWC");
  CreatePath(szPath, VAR_DIRXWS, szFile);
  if (_dos_open(szPath, O_RDONLY, &file))
    return (ERRINT_OBJECT);

  DosQFileInfo(file, 1, &fsts, sizeof(fsts));
  wLen = (WORD)fsts.cbFile;

  if ((hObjectCode = GlobalAlloc(GMEM_MOVEABLE, (DWORD)wLen)) == 0)
    {
    DosClose(file);
    return (ERRINT_MEMORY);
    }

  lpObjectCode = GlobalLock(hObjectCode);
  DosRead(file, (void far *)lpObjectCode, wLen, (unsigned *)&wCopied);
  DosClose(file);

  if (wLen != wCopied)
    return (ERRSYS_READFAULT);

  return (0);
}

BOOL SaveScriptObject(szScript)
  char *szScript;
{
  char szFile[32], szPath[MAX_PATH];
  int  file;
  WORD wLen, wCopied;
  register char *ptr;


  strcpy(szFile, szScript);
  if (ptr = strchr(szFile, '.'))
    *ptr = NULL;

  if (hObjectCode == 0)
    return (ERRSYS_GENERALFAILURE);

  strcat(szFile, ".XWC");
  CreatePath(szPath, VAR_DIRXWS, szFile);
  if (_dos_creat(szPath, _A_NORMAL, &file))
    return (ERRSYS_OPENFAULT);

  wLen = (WORD)GlobalSize(hObjectCode);
  DosWrite(file, (void far *)lpObjectCode, wLen, (unsigned *)&wCopied);
  DosClose(file);

  if (wLen != wCopied)
    return (ERRDOS_DISKFULL);

  return (0);
}

int CompileScript(szScript)
  char *szScript;
{
  HANDLE hResInfo, hResData;
  LPSTR  lpTokens, lpKey, lpObj;
  MSG    Msg;
  WORD	 wLen;
  WORD	 wSaveAddr, wSaveLine;
  HFILE  hfSrcFile;
  BYTE	 abBuf[MAX_BUFFER];
  USHORT iBuf, usAction, usBytes;
  char   szSource[256], *pSource, szNextLine[256];
  DWORD  dwInteger;
  char   szBuf[256];
  BYTE	 byData[256];
  char	 cComments;
  char	 fInString;
  char   fInWatch;
  char   fElseFlag;
  int	 lastToken;
  int	 nParenCnt;
  int    nBraceCnt[32], iBraceCnt;
  char   *ptr;
  register i;


  if (hObjectCode)
    {
    GlobalUnlock(hObjectCode);
    GlobalFree(hObjectCode);
    hObjectCode = 0;
    }

  CreatePath(szBuf, VAR_DIRXWS, szScript);

  if (DosOpen(szBuf, &hfSrcFile, &usAction, 0L, FILE_NORMAL, FILE_OPEN,
	      OPEN_ACCESS_READWRITE, 0L))
    return (nErrorCode = ERRCOM_FILEOPEN);
  iBuf = usBytes = 0;

  if ((hObjectCode = GlobalAlloc(GMEM_MOVEABLE, 32000L)) == 0)
    {
    if ((hObjectCode = GlobalAlloc(GMEM_MOVEABLE, 16000L)) == 0)
      {
      if ((hObjectCode = GlobalAlloc(GMEM_MOVEABLE, 8000L)) == 0)
	{
	if ((hObjectCode = GlobalAlloc(GMEM_MOVEABLE, 4000L)) == 0)
	  {
	  if ((hObjectCode = GlobalAlloc(GMEM_MOVEABLE, 2000L)) == 0)
	    {
	    hObjectCode = GlobalAlloc(GMEM_MOVEABLE, 1000L);
	    }
	  }
	}
      }
    }
  if (hObjectCode == 0)
    {
    DosClose(hfSrcFile);
    return (nErrorCode = ERRCOM_MEMORY);
    }

  lpCompDlg = MakeProcInstance((FARPROC)DlgCompile, hWndInst);
  hCompDlg  = CreateDialog(hWndInst, MAKEINTRESOURCE(IDD_COMPILE), hWnd, lpCompDlg);
  if (hCompDlg != -1)
    LinkDialog(hCompDlg);
  else
    {
    DosClose(hfSrcFile);
    GlobalUnlock(hObjectCode);
    GlobalFree(hObjectCode);
    return (nErrorCode = ERRCOM_MEMORY);
    }

  if (!IsIconic(hWnd))
    ShowWindow(hCompDlg, SW_SHOW);
  UpdateWindow(hCompDlg);

  if (ptr = strchr(szBuf, '.'))
    *ptr = NULL;
  SetDlgItemText(hCompDlg, ID_COMPILE_SCRIPT, StripPath(szBuf));

  InfoMessage(MSGINF_COMPILING);

  lpObj = lpObjectCode = GlobalLock(hObjectCode);
  wAddr = 0;
  wLen = (WORD)GlobalSize(hObjectCode);

  hResInfo = FindResource(hWndInst, MAKEINTRESOURCE(IDU_TOKENS), MAKEINTRESOURCE(USER));
  hResData = LoadResource(hWndInst, hResInfo);
  lpTokens = LockResource(hResData);
  if (hResInfo == 0 || hResData == 0 || lpTokens == 0L)
    {
    DestroyWindow(hCompDlg);
    UnLinkDialog(hCompDlg);
    DosClose(hfSrcFile);
    GlobalUnlock(hObjectCode);
    GlobalFree(hObjectCode);
    return (nErrorCode = ERRCOM_MEMORY);
    }

  pSource   = szSource;
  *pSource  = NULL;
  wLine     = 0;
  nStack    = 0;
  fInWatch  = FALSE;
  cComments = 0;
  lastToken = 0;
  nParenCnt = 0;
  iBraceCnt = 0;
  nBraceCnt[0] = 0;
  nErrorCode = 0;


  if (LibGetLine(szNextLine, sizeof(szNextLine), hfSrcFile, &iBuf, &usBytes, abBuf, MAX_BUFFER))
    {
    szNextLine[0] = 26;
    i = 1;
    }
  else
    i = strlen(szNextLine);

  szNextLine[i+0] = '\r';
  szNextLine[i+1] = '\0';

  while (nErrorCode == 0 && (wAddr + 256 < wLen))
    {
    MSG msg;

    while (PeekMessage(&msg, hCompDlg, 0, 0, PM_REMOVE))
      IsDialogMessage(hCompDlg, &msg);

    if (*pSource == NULL)
      {
      if (nParenCnt != 0)
	{
	nErrorCode = ERRCOM_MISLPAR;
	break;
	}

      SetDlgItemInt(hCompDlg, ID_COMPILE_LINE, wLine+1, FALSE);
      memcpy(szSource, szNextLine, sizeof(szSource)-1);
      szSource[255] = '\r';
      if (szNextLine[0] != 26)
	{
        fElseFlag = FALSE;
	if (LibGetLine(szNextLine, sizeof(szNextLine), hfSrcFile, &iBuf, &usBytes, abBuf, MAX_BUFFER))
	  {
          szNextLine[0] = 26;
	  szNextLine[1] = '\r';
	  szNextLine[2] = '\0';
	  }
        else
          {
          char *ptr;

	  i = strlen(szNextLine);
	  szNextLine[i+0] = '\r';
	  szNextLine[i+1] = '\0';

          ptr = szNextLine;
          while (isspace(*ptr))
            ptr++;
          if (memicmp(ptr, "ELSE", 4) == 0)
            fElseFlag = TRUE;
          }
        }

      if (szSource[0] == 26)
	{
	while (nStack && (wStackType[nStack-1] == TK_THEN ||
			  wStackType[nStack-1] == TK_ELSE))
	  {
	  nStack--;
	  i = wStackAddr[nStack];
	  *(int far *)(&lpObjectCode[i+4]) = wAddr;
	  *(int far *)(&lpObjectCode[i+6]) = wLine;
	  }

        DestroyWindow(hCompDlg);
        UnLinkDialog(hCompDlg);

	GlobalUnlock(hResData);
	FreeResource(hResData);
	DosClose(hfSrcFile);
	*((int far *)lpObj)++ = TK_EOFM;
	wAddr += 2;
	GlobalUnlock(hObjectCode);
	if ((hObjectCode = GlobalReAlloc(hObjectCode, (DWORD)wAddr, 0)) == 0)
	  return (nErrorCode = ERRCOM_MEMORY);
	lpObjectCode = GlobalLock(hObjectCode);

	InfoMessage(NULL);

	if (nStack)
	  {
	  GlobalUnlock(hObjectCode);
	  GlobalFree(hObjectCode);
	  hObjectCode = NULL;

	  return (nErrorCode = ERRCOM_EOF);
	  }
	return (0);
	}

      pSource = szSource;
      wLine++;
      }

    if (cComments)
      {
      if (fInString)
	{
	if (*pSource == '"')
	  fInString = 0;
	}
      else
	{
	if (*pSource == '"')
	  fInString = 1;
	else if (*pSource == '/')
	  {
	  if (pSource[1] == '*')
	    {
	    cComments++;
	    pSource++;
	    }
	  }
	else if (*pSource == '*')
	  {
	  if (pSource[1] == '/')
	    {
	    cComments--;
	    pSource++;
	    }
	  }
	}

      if (*pSource == '\r')
	{
	*((int far *)lpObj)++ = lastToken = TK_EOPL;
	wAddr += 2;
	}

      if (*pSource)
	pSource++;
      continue;
      }

    switch (toupper(*pSource))
      {
      case '\r':
        if (lastToken == TK_ELSE || lastToken == TK_THEN || fElseFlag ||
            (lastToken == TK_EOLL && !fInWatch))
          break;

        while (nBraceCnt[iBraceCnt] < nStack &&
               (wStackType[nStack-1] == TK_THEN || wStackType[nStack-1] == TK_ELSE))
	  {
	  nStack--;
	  i = wStackAddr[nStack];
	  *(int far *)(&lpObjectCode[i+4]) = wAddr;
	  *(int far *)(&lpObjectCode[i+6]) = wLine;
	  }

        for (i = nBraceCnt[iBraceCnt]; i < nStack; i++)
	  {
	  if (wStackType[i] == TK_IF)
	    {
	    nErrorCode = ERRCOM_MISTHEN;
	    break;
	    }
	  if (wStackType[i] == TK_THEN)
	    {
	    nErrorCode = ERRCOM_STKOVR;
	    break;
	    }
	  }

        if (iBraceCnt)
	  {
	  *((int far *)lpObj)++ = lastToken = TK_EOVL;
	  wAddr += 2;
	  *pSource = NULL;
	  break;
	  }

	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERAND;

	*((int far *)lpObj)++ = lastToken = TK_EOPL;
	wAddr += 2;
	*pSource = NULL;
	break;

      case '\f':
      case '\n':
      case '\t':
      case ' ':
      case 0:
	break;

      case '"':
	i = 0;
	pSource++;
	while (*pSource != '"' && *pSource != '\r' && *pSource != 0)
          {
          if (*pSource == '`' && *(pSource+1) == '"')  // 00031 Fix quoted str
            pSource++;                                 // 00031 Fix quoted str

	  szBuf[i++] = *pSource++;
          }
	szBuf[i] = NULL;

	if (*pSource != '"')
	  {
	  nErrorCode = ERRCOM_MISQUOTE;
	  break;
	  }

	*((int far *)lpObj)++ = lastToken = TK_STRING;
	LibDestore(szBuf, byData, sizeof(byData));
	lmemcpy(lpObj, byData, byData[0] + 1);
	lpObj += (int)byData[0] + 1;
	wAddr += (int)byData[0] + 3;

	if (*pSource == '\r')
	  pSource--;
	break;

      case '#':
	*((int far *)lpObj)++ = lastToken = TK_POUND;
	wAddr += 2;
	break;

      case '.':
	if (*(pSource+1) == '.' && *(pSource+2) == '.')
	  {
	  *(pSource+1) = 0;
	  *((int far *)lpObj)++ = TK_EOVL;
	  wAddr += 2;
	  }
	else
	  nErrorCode = ERRCOM_BADCONSTANT;
	break;

      case '&':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	*((int far *)lpObj)++ = lastToken = TK_BITAND;
	wAddr += 2;
	break;

      case '\'':
	i = 0;
	pSource++;
	while (*pSource != '\'' && *pSource != '\r' && *pSource != 0)
          {
          if (*pSource == '`' && *(pSource+1) == '\'') // 00031 Fix quoted str
            pSource++;                                 // 00031 Fix quoted str

	  szBuf[i++] = *pSource++;
          }
	szBuf[i] = NULL;

	if (*pSource != '\'')
	  {
	  nErrorCode = ERRCOM_MISQUOTE;
	  break;
	  }

	*((int far *)lpObj)++ = lastToken = TK_STRING;
	LibDestore(szBuf, byData, sizeof(byData));
	lmemcpy(lpObj, byData, byData[0] + 1);
	lpObj += (int)byData[0] + 1;
	wAddr += (int)byData[0] + 3;

	if (*pSource == '\r')
	  pSource--;
	break;

      case '(':
	if (lastToken == TK_VARIABLE)
	  nErrorCode = ERRCOM_UNKNOWN;
	nParenCnt++;
	*((int far *)lpObj)++ = lastToken = TK_LPAR;
	wAddr += 2;
	break;

      case ')':
	if (nParenCnt-- == 0)
	  nErrorCode = ERRCOM_MISRPAR;
	*((int far *)lpObj)++ = lastToken = TK_RPAR;
	wAddr += 2;
	break;

      case '*':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	*((int far *)lpObj)++ = lastToken = TK_MUL;
	wAddr += 2;
	break;

      case '+':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	*((int far *)lpObj)++ = lastToken = TK_PLUS;
	wAddr += 2;
	break;

      case ',':
	*((int far *)lpObj)++ = lastToken = TK_COMMA;
	wAddr += 2;
	break;

      case '-':
	if (pSource[1] != '-')
	  {
	  if (lastToken & CL_OPERN)
	    *((int far *)lpObj)++ = lastToken = TK_NEG;
	  else
	    *((int far *)lpObj)++ = lastToken = TK_SUB;
	  }
	else
	  {
          if (iBraceCnt)
	    *((int far *)lpObj)++ = lastToken = TK_EOVL;
	  else
	    *((int far *)lpObj)++ = lastToken = TK_EOPL;
	  *pSource = NULL;
	  }
	wAddr += 2;
	break;

      case '/':
	if (pSource[1] != '*')
	  {
	  *((int far *)lpObj)++ = lastToken = TK_DIV;
	  wAddr += 2;
	  }
	else
	  {
	  cComments++;
	  fInString = 0;
	  pSource++;
	  }
	break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	i = 0;
	while (isalnum(*pSource))
	  szBuf[i++] = *pSource++;
	szBuf[i] = 0;

	if (*pSource == '.')
	  {
	  nErrorCode = ERRCOM_NOREAL;
	  break;
	  }

	if (LibStrToInt(szBuf, &dwInteger))
	  {
          nErrorCode = ERRCOM_BADCONSTANT;
	  break;
          }

        if (dwInteger & 0x80000000L)
	  {
          nErrorCode = ERRCOM_OUTOFRANGE;
	  break;
          }

	if (lastToken == TK_NEG)
	  {
	  dwInteger = -(long)dwInteger;
	  lpObj -= 2;
	  wAddr -= 2;
	  }

	*((int far *)lpObj)++ = lastToken = TK_INTEGER;
	*((DWORD far *)lpObj)++ = dwInteger;
	wAddr += 6;

	pSource--;
	break;

      case ':':
	if (nParenCnt != 0)
	  {
	  nErrorCode = ERRCOM_MISLPAR;
	  break;
	  }

	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERAND;

	*((int far *)lpObj)++ = lastToken = TK_EOLL;
	wAddr += 2;
	break;

      case ';':
	if (lastToken == 0 || lastToken == TK_EOLL || lastToken == TK_EOPL)
	  {
	  *((int far *)lpObj)++ = lastToken = TK_EOPL;
	  *pSource = NULL;
	  }
	else
	  {
	  *((int far *)lpObj)++ = lastToken = TK_SEMI;
	  }
	wAddr += 2;
	break;

      case '<':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	if (pSource[1] == '=')
	  {
	  *((int far *)lpObj)++ = lastToken = TK_LE;
	  pSource++;
	  }
	else if (pSource[1] == '>')
	  {
	  *((int far *)lpObj)++ = lastToken = TK_INEQ;
	  pSource++;
	  }
	else
	  {
	  *((int far *)lpObj)++ = lastToken = TK_LT;
	  }
	wAddr += 2;
	break;

      case '=':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	*((int far *)lpObj)++ = lastToken = TK_EQ;
	wAddr += 2;
	break;

      case '>':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	if (pSource[1] != '=')
	  {
	  *((int far *)lpObj)++ = lastToken = TK_GT;
	  }
	else
	  {
	  *((int far *)lpObj)++ = lastToken = TK_GE;
	  pSource++;
	  }
	wAddr += 2;
	break;

      case '?':
	*((int far *)lpObj)++ = lastToken = TK_PRINT;
	wAddr += 2;
	break;

      case '@':
	*((int far *)lpObj)++ = lastToken = TK_AMP;
	wAddr += 2;
	break;

      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '%':
      case '$':
	i = 0;
	while (isalnum(*pSource) || *pSource=='_' || *pSource=='%' || *pSource=='$')
	  {
	  if (i < 31)
	    szBuf[i++] = toupper(*pSource);
	  *pSource++;
	  }
	szBuf[i] = 0;
	pSource--;

        if (lastToken != TK_LABEL && lastToken != TK_GOTO && lastToken != TK_GOSUB)
	  {
	  lpKey = lpTokens;
	  while (*lpKey)
	    {
	    if (lmemcmp(lpKey, szBuf, i+1) == 0)
	      {
	      *((int far *)lpObj)++ = lastToken = *((int far *)(lpKey+i+1));
	      wAddr += 2;

	      if (lastToken == 0)
		{
		nErrorCode = ERRCOM_MK4KEYWORD;
		break;
		}

	      if ((lastToken & CL_PAREN) && pSource[1] != '(')
		{
		nErrorCode = ERRCOM_MISLPARFUNCT;
		break;
		}

	      switch (lastToken)
		{
		case TK_IF:
		  if (nErrorCode = PushLocation(TK_IF))
		    break;
		  *((int far *)lpObj)++ = 0;
		  *((int far *)lpObj)++ = 0;
		  *((int far *)lpObj)++ = 0;
		  *((int far *)lpObj)++ = 0;
		  wAddr += 8;
		  break;

		case TK_THEN:
		  if (nStack && wStackType[nStack-1] == TK_IF)
		    {
		    i = wStackAddr[nStack-1];
		    *(int far *)(&lpObjectCode[i])   = wAddr;
		    *(int far *)(&lpObjectCode[i+2]) = wLine;

		    wStackType[nStack-1] = TK_THEN;
		    }
		   else
		    nErrorCode = ERRCOM_MISIF;
		  break;

		case TK_ELSE:
		  while (nStack && wStackType[nStack-1] == TK_ELSE)
		    {
		    nStack--;
		    i = wStackAddr[nStack];
		    *(int far *)(&lpObjectCode[i+4]) = wAddr - 2;
		    *(int far *)(&lpObjectCode[i+6]) = wLine;
		    }
		  if (nStack && wStackType[nStack-1] == TK_THEN)
		    {
		    *((int far *)lpObj)++ = 0;
		    *((int far *)lpObj)++ = 0;
		    wAddr += 4;

		    wSaveAddr = wStackAddr[nStack-1];
		    *(int far *)(&lpObjectCode[wSaveAddr+4]) = wAddr;
		    *(int far *)(&lpObjectCode[wSaveAddr+6]) = wLine;

		    wStackType[nStack-1] = TK_ELSE;
		    wStackAddr[nStack-1] = wAddr - 8;
		    wStackLine[nStack-1] = wLine;
		    }
		  else
		    nErrorCode = ERRCOM_MISELSE;
		  break;

		case TK_REPEAT:
		  if (nErrorCode = PushLocation(TK_REPEAT))
		    break;
		  break;

		case TK_UNTIL:
		  wSaveAddr = wAddr;
		  wSaveLine = wLine;
		  if (PopLocation(TK_REPEAT))
		    {
		    nErrorCode = ERRCOM_MISREPEAT;
		    break;
		    }

		  *((int far *)lpObj)++ = wAddr - 2;
		  *((int far *)lpObj)++ = wLine;

		  wAddr = wSaveAddr + 4;
		  wLine = wSaveLine;
		  break;

		case TK_WHILE:
		  if (nErrorCode = PushLocation(TK_WHILE))
		    break;
		  *((int far *)lpObj)++ = 0;
		  *((int far *)lpObj)++ = 0;
		  wAddr += 4;
		  break;

		case TK_WEND:
		  wSaveAddr = wAddr;
		  wSaveLine = wLine;
		  if (PopLocation(TK_WHILE))
		    {
		    nErrorCode = ERRCOM_MISWHILE;
		    break;
		    }

		  *(int far *)(&lpObjectCode[wAddr])   = wSaveAddr-2;
		  *(int far *)(&lpObjectCode[wAddr+2]) = wSaveLine;

		  *((int far *)lpObj)++ = wAddr - 2;
		  *((int far *)lpObj)++ = wLine;

		  wAddr = wSaveAddr + 4;
		  wLine = wSaveLine;
		  break;

                case TK_WATCH:
                  fInWatch = TRUE;
		  break;

                case TK_ENDWATCH:
                  fInWatch = FALSE;
		  break;
		}
	      break;
	      }
	     else
	      {
	      while (*lpKey != 0)
		lpKey++;
	      lpKey += 3;
	      }
	    }

	  if (*lpKey == 0)
	    {
	    *((int far *)lpObj)++ = lastToken = TK_VARIABLE;
	    lmemcpy(lpObj, szBuf, i+1);
	    lpObj += i+1;
	    wAddr  += i+3;
	    }
	  }
	else
          {
          if (lastToken == TK_LABEL)
            {
            if (CheckLabel(szBuf))
              {
              nErrorCode = ERRCOM_DUPLABEL;
              break;
              }
            }

	  *((int far *)lpObj)++ = lastToken = TK_VARIABLE;
	  lmemcpy(lpObj, szBuf, i+1);
	  lpObj += i+1;
	  wAddr  += i+3;
	  }
	break;

      case '[':
	nErrorCode = ERRCOM_ARRAY;
	break;

      case '\\':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	*((int far *)lpObj)++ = lastToken = TK_IDIV;
	wAddr += 2;
	break;

      case ']':
	nErrorCode = ERRCOM_ARRAY;
	break;

      case '^':
	if (lastToken & CL_OPERN)
	  nErrorCode = ERRCOM_OPERATOR;

	*((int far *)lpObj)++ = lastToken = TK_BITXOR;
	wAddr += 2;
	break;

      case '{':
        if (iBraceCnt >= sizeof(nBraceCnt) / sizeof(int))
          nErrorCode = ERRCOM_STKOVR;
        nBraceCnt[++iBraceCnt] = nStack;
	break;

      case '|':
	*((int far *)lpObj)++ = lastToken = TK_BITOR;
	wAddr += 2;
	break;

      case '}':
        if (iBraceCnt-- == 0)
	  nErrorCode = ERRCOM_MISRPAR;
	break;

      case '~':
	*((int far *)lpObj)++ = lastToken = TK_BITNOT;
	wAddr += 2;
	break;

      default:
	nErrorCode = ERRCOM_UNKNOWN;
	break;
      }

    if (*pSource)
      pSource++;
    }

  DestroyWindow(hCompDlg);
  UnLinkDialog(hCompDlg);

  GlobalUnlock(hResData);
  FreeResource(hResData);
  DosClose(hfSrcFile);
  GlobalUnlock(hObjectCode);
  GlobalFree(hObjectCode);
  hObjectCode = 0;

  InfoMessage(NULL);

  if (nErrorCode)
    {
    return (nErrorCode);
    }
  else
    return (0);
}

BOOL DlgCompile(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  switch (message)
    {
    case WM_INITDIALOG:
      break;

    case WM_COMMAND:
      nErrorCode = ERRCOM_USERABORT;
      break;

    default:
      return (FALSE);
    }

  return (TRUE);
}


static int near PopLocation(wToken)
  WORD wToken;
{
  if (nStack && wStackType[nStack-1] == wToken)
    {
    nStack--;
    wAddr = wStackAddr[nStack];
    wLine = wStackLine[nStack];
    return (FALSE);
    }

  return (ERRCOM_EXPRESSION);
}

static int near PushLocation(wToken)
  WORD wToken;
{
  if (nStack < sizeof(wStackType) / sizeof(WORD))
    {
    wStackType[nStack] = wToken;
    wStackAddr[nStack] = wAddr;
    wStackLine[nStack] = wLine;
    nStack++;
    return (0);
    }

  return (ERRCOM_STKOVR);
}

#define OpCode(x)	(*(WORD far *)(&lpObjectCode[x]))

static int near CheckLabel(szLabel)
  char *szLabel;
{
  WORD wAdr;
  int  nLen;
  register i;


  nLen = strlen(szLabel);
  wAdr = 0;

  while (wAdr + 5 < wAddr)
    {
    if (OpCode(wAdr) == TK_LABEL)
      {
      if (OpCode(wAdr+2) == TK_VARIABLE)
	{
        if (lmemcmp(&lpObjectCode[wAdr+4], szLabel, nLen + 1) == 0)
          return (TRUE);
	}
      }

    i = 1;
    while (i)
      {
      switch (OpCode(wAdr))
	{
	case TK_EOLL:
          wAdr += 2;
	  i = 0;
	  break;

	case TK_EOVL:
	case TK_EOPL:
          wAdr += 2;
	  i = 0;
	  break;

	case TK_INTEGER:
          wAdr += 6;
	  break;

	case TK_STRING:
          wAdr += (WORD)lpObjectCode[wAdr+2] + 3;
	  break;

	case TK_VARIABLE:
          wAdr += 2;
          while (lpObjectCode[wAdr])
            wAdr++;
          wAdr++;
	  break;

	case TK_ELSE:
	case TK_UNTIL:
	case TK_WHILE:
	case TK_WEND:
          wAdr += 6;
	  break;

	case TK_IF:
          wAdr += 10;
	  break;

	default:
          wAdr += 2;
	  break;
	}
      }
    }

  return (FALSE);
}
