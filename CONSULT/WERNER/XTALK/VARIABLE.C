/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Variable Module (VARIABLE.C)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the dynamic variable logic routines.       *
*             Len, Level, Name, 0, Type, Data                                 *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 08/02/89 00001 Fix HST speed change problem. 	      *
*             1.01 09/08/89 00030 Change MISC from a global variable to a     *
*                                   session variable to match the book.       *
*             1.01 09/08/89 00034 Improve error check for external routines.  *
*	      1.01 10/26/89 00049 When a new terminal is selected from the    *
*				    NEW.FILE dialog or the script language    *
*				    reset the terminal params to defaults.    *
*	      1.01 12/25/89 00070 Improve NEXTLINE support.		      *
*             1.01 01/18/90 D0086 Update VARLEN in case ValidatePath routine  *
*                                   has changed the VARDATA string.           *
*             1.01 01/19/90 00090 Check for cTabWidth being equal to zero.    *
*  dca00066 JDB 04/03/90 Added DDE Advise                                     *
*		1.1  04/12/90 dca00082 PJL Couldn't set Port to COM3/COM4 from script. *
*                                                                             *
\*****************************************************************************/

#define NOATOM			/* Atom Manager routines */
#define NOBITMAP		/* typedef HBITMAP and associated routines */
#define NOBRUSH 		/* typedef HBRUSH and associated routines */
#define NOCLIPBOARD		/* clipboard routines */
#define NOCOLOR 		/* CTLCOLOR_*, COLOR_* */
#define NOCREATESTRUCT		/* typedef CREATESTRUCT */
#define NOCTLMGR		/* control and dialog routines */
#define NODRAWTEXT		/* DrawText() and DT_* */
#define NOFONT			/* typedef FONT and associated routines */
#define NOGDI			/* StretchBlt modes and gdi logical objects */
#define NOGDICAPMASKS		/* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOHDC			/* typedef HDC and associated routines */
#define NOICON			/* IDI_* */
#define NOKANJI 		/* Kanji support stuff. */
#define NOMENUS 		/* MF_*, HMENU and associated routines */
#define NOMETAFILE		/* typedef METAFILEPICT */
#define NOMINMAX		/* Macros min(a,b) and max(a,b) */
#define NONCMESSAGES		/* WM_NC* and HT* */
#define NOPEN			/* typedef HPEN and associated routines */
#define NORASTEROPS		/* binary and tertiary raster ops */
#define NOREGION		/* typedef HRGN and associated routines */
#define NORESOURCE		/* Predefined resource types:  RT_* */
#define NOSCROLL		/* SB_* and scrolling routines */
#define NOSOUND 		/* Sound driver routines */
#define NOSYSCOMMANDS		/* SC_* */
#define NOSYSMETRICS		/* SM_*, GetSystemMetrics */
#define NOTEXTMETRIC		/* typedef TEXTMETRIC and associated routines */
#define NOWH			/* SetWindowsHook and WH_* */
#define NOWINMESSAGES		/* WM_* */
#define NOWINOFFSETS		/* GWL_*, GCL_*, associated routines */
#define NOWINSTYLES		/* WS_*, CS_*, ES_*, LBS_* */
#define NOWNDCLASS		/* typedef WNDCLASS and associated routines */

#include <windows.h>
#include <dos.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "variable.h"
#include "comm.h"
#include "disp.h"
#include "sess.h"
#include "term.h"
#include "xfer.h"
#include "compile.h"
#include "macro.h"
#include "properf.h"
#include "dialogs.h"
#include "global.h"

#include "ddeadv.h"	/* dca00066 JDB DDE Advise */

extern int lstrcmp(LPSTR, LPSTR);

  extern char sInfoMessage[];

  char  fHostScript;
  char  fTimeout;
  char  fConnectARQ;
  int   nConnectSpeed;
  int   nErrorCode;
  int   nErrorFlag;
  int   nErrorSave;
  char  szScriptName[16];
  char  szMatchedString[33];
  char  cDefInputFile;
  char  cDefOutputFile;
  int   nWorkChoice;


  HANDLE   hHandleData;
  WORD far *lpHandleData;
  WORD	   wHandles;

  HANDLE   hVariableData;
  BYTE far *lpVariableData;
  WORD	   wFirstFree;

static WORD wLocked;
static char fNeedCompression;
static void near CompressData();
static int  near GetNewHandle();
static int  near MinimumData(WORD);
static BYTE * near comcpy(char *, int, BYTE *, BYTE *);
static BYTE near datcpy(char *, BYTE *,  BYTE);

int InitVariable()
{
  char szBuf[32];
  register WORD i;


  if (!(hHandleData = GlobalAlloc(GMEM_MOVEABLE, 1024L)))
    {
    ErrorMessage(MSG_LOCALMEMORY);
    return (FALSE);
    }

  if (!(hVariableData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, 2048L)))
    {
    GlobalFree(hHandleData);
    ErrorMessage(MSG_LOCALMEMORY);
    return (FALSE);
    }

  wHandles     = (WORD)(GlobalSize(hHandleData)) / 2;
  lpHandleData = (WORD far *)GlobalLock(hHandleData);
  for (i = 0; i < wHandles; i++)
    lpHandleData[i] = 0xFFFF;
  GlobalUnlock(hHandleData);

  wFirstFree = 0;

  wLocked = 0;
  fNeedCompression = FALSE;

  szBuf[0] = 'K';
  szBuf[1] = 'E';
  szBuf[2] = 'Y';
  szBuf[4] = NULL;
  szBuf[5] = NULL;
  for (i = 0; i < 24; i++)
    {
    if (i < 10)
      {
      szBuf[3] = (char)('0' + i);
      }
    else
      {
      szBuf[3] = (char)('0' + i / 10);
      szBuf[4] = (char)('0' + i % 10);
      }
    CreateVariableHandle(szBuf, 0);
    }

  szBuf[0] = 'D';
  szBuf[1] = 'I';
  szBuf[2] = 'R';
  szBuf[3] = 'X';
  szBuf[4] = 'W';
  szBuf[5] = 'P';
  szBuf[6] = NULL;
  CreateVariableHandle(szBuf, 0);
  szBuf[5] = 'S';
  CreateVariableHandle(szBuf, 0);
  szBuf[3] = 'F';
  szBuf[4] = 'I';
  szBuf[5] = 'L';
  CreateVariableHandle(szBuf, 0);

  szBuf[0] = '#';
  szBuf[1] = '1';
  szBuf[2] = NULL;
  CreateVariableHandle(szBuf, 0);

  return (TRUE);
}

void TermVariable()
{
  if (wLocked)
    {
    GlobalUnlock(hHandleData);
    GlobalUnlock(hVariableData);
    }

  GlobalFree(hHandleData);
  GlobalFree(hVariableData);
}


int CreateVariableHandle(name, level)
  char *name;
  int  level;
{
  WORD nLen;
  register i;


  LockVariable();

  if (fNeedCompression)
    CompressData();

  nLen = strlen(name);

  for (i = 0; i < wFirstFree; i++)
    {
    if (nLen == lpVariableData[i])
      {
      if (lpVariableData[i+1] == (BYTE)level)
	{
	if (lstrcmp(name, &lpVariableData[i+2]) == 0)
	  break;
	}
      }

    i += nLen + 3;
    if (lpVariableData[i] == 0)
      i += lpVariableData[i+1] + 1;   /* Skip over String Variable */
    else
      i += 4;			      /* Skip over Integer Variable */
    }

  if (i < wFirstFree)
    {
    register j;

    for (j = 0; j < wHandles; j++)
      {
      if (lpHandleData[j] == i)
	{
	i = j;
	break;
	}
      }
    }
  else
    {
    if (MinimumData(nLen+32))
      return (FALSE);

    if ((i = GetNewHandle()) == -1)
      return (FALSE);

    lpHandleData[i] = wFirstFree;

    lpVariableData[wFirstFree] = (BYTE)nLen;
    lpVariableData[wFirstFree+1] = (BYTE)level;
    lstrcpy(&lpVariableData[wFirstFree+2], name);
    lpVariableData[wFirstFree+3+nLen] = 0;   /* Set Variable to String */
    lpVariableData[wFirstFree+4+nLen] = 0;   /* Set string length to 0 */
    wFirstFree += 5+nLen;
    }

  UnlockVariable();

  return (i);
}

void DeleteVariableHandle(varno)
  int  varno;
{
  WORD nLen;
  register i;


  LockVariable();

  i = lpHandleData[varno];
  nLen = lpVariableData[i] + 3;
  if (lpVariableData[i+nLen] == 0)
    nLen += lpVariableData[i+nLen+1] + 2;
  else
    nLen += 5;

  lmemset(&lpVariableData[i], 0, nLen);
  lpHandleData[varno] = 0xFFFF;

  fNeedCompression = TRUE;

  UnlockVariable();
}

int GetVariableHandle(name)
  char *name;
{
  int  nLen, nVar, nLev;
  register i;


  LockVariable();

  if (fNeedCompression)
    CompressData();

  nLen = strlen(name);

  nVar = -1;
  nLev = -1;
  for (i = 0; i < wFirstFree;)
    {
    if (nLen == lpVariableData[i] && nLev < (int)(lpVariableData[i+1]))
      {
      if (lstrcmp(name, &lpVariableData[i+2]) == 0)
	{
	register j;

	for (j = 0; j < wHandles; j++)
	  {
	  if (lpHandleData[j] == i)
	    {
	    nVar = j;
	    break;
	    }
	  }
	}
      }

    if (lpVariableData[i])
      {
      i += lpVariableData[i] + 3;
      if (lpVariableData[i] == 0)
	i += lpVariableData[i+1] + 2;	/* Skip over String Variable */
      else
	i += 5; 			/* Skip over Integer Variable */
      }
    else
      i++;
    }

  UnlockVariable();

  return (nVar);
}


short SetIntVariable(varno, varint)
  int  varno;
  long varint;
{
  WORD wVarAddr;
  char buf[64];
  WORD nHeadLen;
  register WORD i, j;
  HANDLE hAdvItem;				// dca00066 JDB DDE Advise
  long var_val;

  /* If Advised on then send advise message */	// dca00066 JDB DDE Advise
  /* Get the current value */			// dca00066 JDB DDE Advise
  GetIntVariable(varno,&var_val);		// dca00066 JDB DDE Advise
  if(var_val != varint)				// dca00066 JDB DDE Advise
  {						// dca00066 JDB DDE Advise
	hAdvItem = advised_on(varno,NULL);	// dca00066 JDB DDE Advise
	if(hAdvItem != NULL)			// dca00066 JDB DDE Advise
		Advise(hAdvItem,varint,NULL);	// dca00066 JDB DDE Advise
  }						// dca00066 JDB DDE Advise

  LockVariable();

  wVarAddr = lpHandleData[varno];
  nHeadLen = lpVariableData[wVarAddr] + 3;
  if (lpVariableData[wVarAddr+nHeadLen] == 0)
    j = lpVariableData[wVarAddr+nHeadLen+1] + 1;
  else
    j = sizeof(long);

  lmemset(&lpVariableData[wVarAddr+nHeadLen], 0, j+1);

  if (j >= sizeof(long))
    {
    lpVariableData[wVarAddr+nHeadLen] = 1;
    *((long far *)(&lpVariableData[wVarAddr+nHeadLen+1])) = varint;
    if (j != sizeof(long))
      fNeedCompression = TRUE;
    }
  else
    {
    lmemcpy(buf, &lpVariableData[wVarAddr], nHeadLen);
    lmemset(&lpVariableData[wVarAddr], 0, nHeadLen);

    fNeedCompression = TRUE;
    if (MinimumData(nHeadLen+32))
      return (ERRSYS_NOMEMORY);

    lpHandleData[varno] = wFirstFree;
    lmemcpy(&lpVariableData[wFirstFree], buf, nHeadLen);
    wFirstFree += nHeadLen;
    lpVariableData[wFirstFree] = 1;	    /* Set type as Integer */
    wFirstFree++;
    *((long far *)(&lpVariableData[wFirstFree])) = varint;
    wFirstFree += sizeof(long);
    }

  UnlockVariable();
  return (NULL);
}


void GetIntVariable(varno, varint)
  int  varno;
  long *varint;
{
  register WORD i;


  LockVariable();

  i  = lpHandleData[varno];
  i += lpVariableData[i] + 3;
  if (lpVariableData[i] == 0)
    {
    *varint = 0L;
    }
  else
    {
    *varint = *((long far *)(&lpVariableData[i + 1]));
    }

  UnlockVariable();
}

short SetStrVariable(varno, vardata, varlen)
  int  varno;
  BYTE *vardata;
  WORD varlen;
{
  WORD wVarAddr;
  //dca00066 JDB DDE Advise BYTE buf[64];
  BYTE buf[256];	//dca00066 JDB DDE Advise
  WORD nHeadLen;
  register WORD i, j;
  HANDLE hAdvItem;	//dca00066 JDB DDE Advise

  if (varno == -1)
    return (NULL);

  /* Get the string variable and check for not equal */
  GetStrVariable(varno,buf,sizeof(buf));	//dca00066 JDB DDE Advise
  if(lstrcmp(buf,vardata) != 0)			//dca00066 JDB DDE Advise
  {						//dca00066 JDB DDE Advise
	/* They are different */		//dca00066 JDB DDE Advise
	hAdvItem = advised_on(varno,NULL);	//dca00066 JDB DDE Advise
	if(hAdvItem != NULL)			//dca00066 JDB DDE Advise
		Advise(hAdvItem,0L,vardata);	//dca00066 JDB DDE Advise
  }						//dca00066 JDB DDE Advise


  if (varlen > 255)
    varlen = strlen(vardata);
  if (varlen > 255)
    varlen = 255;

  switch (varno)
    {
    case VAR_DIRXWP:
    case VAR_DIRXWS:
    case VAR_DIRFIL:
      if (!ValidatePath(vardata))
        return (ERRDOS_NOPATH);
      // D0086 PJL have to update VARLEN in case ValidatePath
      //         routine has changed the VARDATA string.
      varlen = strlen(vardata);
      break;
    }

  LockVariable();

  wVarAddr = lpHandleData[varno];
  nHeadLen = lpVariableData[wVarAddr] + 3;
  if (lpVariableData[wVarAddr+nHeadLen] == 0)
    j = lpVariableData[wVarAddr+nHeadLen+1] + 1;
  else
    j = 4;

  lmemset(&lpVariableData[wVarAddr+nHeadLen], 0, j+1);

  if (varlen+1 <= j)
    {
    lpVariableData[wVarAddr+nHeadLen+1] = (BYTE)varlen;
    lmemcpy(&lpVariableData[wVarAddr+nHeadLen+2], vardata, varlen);
    if (j != varlen+1)
      fNeedCompression = TRUE;
    }
  else
    {
    lmemcpy(buf, &lpVariableData[wVarAddr], nHeadLen);
    lmemset(&lpVariableData[wVarAddr], 0, nHeadLen);

    fNeedCompression = TRUE;
    if (MinimumData(nHeadLen+j+32))
      return (ERRSYS_NOMEMORY);

    lpHandleData[varno] = wFirstFree;
    lmemcpy(&lpVariableData[wFirstFree], buf, nHeadLen);
    wFirstFree += nHeadLen;
    lpVariableData[wFirstFree] = 0;	    /* Set type as String */
    wFirstFree++;
    lpVariableData[wFirstFree] = (BYTE)varlen;
    wFirstFree++;
    lmemcpy(&lpVariableData[wFirstFree], vardata, varlen);
    wFirstFree += varlen;
    }

  UnlockVariable();

  return (0);
}



BYTE GetStrVariable(varno, vardata, maxlen)
  int  varno;
  BYTE *vardata;
  WORD maxlen;
{
  WORD wVarAddr, wVarLen;
  BYTE szBuf[64];
  register WORD i;


  LockVariable();

  wVarAddr = lpHandleData[varno];
  i = lpVariableData[wVarAddr] + 3;
  if (lpVariableData[wVarAddr + i++] == 0)
    {
    if ((wVarLen = (WORD)lpVariableData[wVarAddr + i]) < maxlen)
      maxlen = lpVariableData[wVarAddr + i];
    else if (wVarLen > maxlen)
      wVarLen = maxlen;

    while (maxlen--)
      *vardata++ = lpVariableData[wVarAddr + ++i];
    if (maxlen)
      *vardata = NULL;
    }
  else
    {
    ltoa(*((long far *)(&lpVariableData[wVarAddr + i])), szBuf, 10);
    if ((i = strlen(szBuf)) < maxlen)
      maxlen = i;
    else if (i > maxlen)
      wVarLen = maxlen;

    i = 0;
    while (maxlen--)
      *vardata++ = szBuf[i++];
    if (maxlen)
      *vardata = NULL;
    }

  UnlockVariable();

  return ((BYTE)wVarLen);
}

BYTE GetStrLength(varno)
  int  varno;
{
  WORD wVarAddr;
  BYTE bVarLen;
  register WORD i;


  LockVariable();

  wVarAddr = lpHandleData[varno];
  i = lpVariableData[wVarAddr] + 3;
  if (lpVariableData[wVarAddr + i] == 0)
    bVarLen = lpVariableData[wVarAddr + i + 1];
  else
    bVarLen = 0;

  UnlockVariable();

  return (bVarLen);
}

BYTE GetVariableLevel(varno)
  int  varno;
{
  register WORD i;


  LockVariable();

  i = lpHandleData[varno];
  i = lpVariableData[i+1];

  UnlockVariable();

  return ((BYTE)i);
}

void LockVariable()
{
  if (wLocked == 0)
    {
    lpHandleData = (WORD far *)GlobalLock(hHandleData);
    lpVariableData = GlobalLock(hVariableData);
    }
  wLocked++;
}

void UnlockVariable()
{
  if (wLocked == 1)
    {
    GlobalUnlock(hHandleData);
    GlobalUnlock(hVariableData);
    }
  wLocked--;
}


int IsStrVariable(varno)
  int  varno;
{
  register i;

  LockVariable();

  i = lpHandleData[varno];
  i += lpVariableData[i] + 3;
  if (lpVariableData[i] == 0)
    i = TRUE;
  else
    i = FALSE;

  UnlockVariable();

  return (i);
}


void PurgeVariableLevel(nLevel)
  WORD nLevel;
{
  WORD nLen;
  register i;


  LockVariable();

  if (fNeedCompression)
    CompressData();

  for (i = 0; i < wFirstFree;)
    {
    nLen = lpVariableData[i] + 3;
    if (lpVariableData[i+nLen] == 0)
      nLen += (BYTE)lpVariableData[i+nLen+1] + 2;   /* Skip over Str Var */
    else
      nLen += 5;				    /* Skip over Int Var */

    if (lpVariableData[i+1] == (BYTE)nLevel)
      {
      register j;

      lmemset(&lpVariableData[i], 0, nLen);
      for (j = 0; j < wHandles; j++)
	{
	if (lpHandleData[j] == i)
	  {
	  lpHandleData[j] = 0xFFFF;
	  break;
	  }
	}
       }

    i += nLen;
    }

  UnlockVariable();

  fNeedCompression = TRUE;
}


static void near CompressData()
{
  WORD nLen;
  register WORD i, j;


  for (i = 0, j = 0; i < wFirstFree;)
    {
    if (lpVariableData[i] != 0)
      {
      nLen = (WORD)lpVariableData[i] + 3;
      if (lpVariableData[i+nLen] == 0)
	nLen += (WORD)lpVariableData[i+nLen+1] + 2;
      else
	nLen += 5;

      if (i == j)
	{
	i += nLen;
	j += nLen;
	}
      else
	{
	register k;

	for (k = 0; k < wHandles; k++)
	  {
	  if (lpHandleData[k] == i)
	    lpHandleData[k] = j;
	  }

	while (nLen--)
	  lpVariableData[j++] = lpVariableData[i++];
	}
      }
    else
      i++;
    }

  wFirstFree = j;
  fNeedCompression = FALSE;
}

static int near MinimumData(wExtra)
  WORD wExtra;
{
  DWORD dwSize;
  WORD  wFreeBytes;


  if (fNeedCompression)
    CompressData();

  dwSize = GlobalSize(hVariableData);
  wFreeBytes = (WORD)dwSize - wFirstFree;
  if (wExtra > wFreeBytes)
    {
    if (dwSize >= 60*1024L)
      return (ERRSYS_NOMEMORY);

    GlobalUnlock(hVariableData);
    if (!(hVariableData = GlobalReAlloc(hVariableData, dwSize + 1024L, 0)))
      return (ERRSYS_NOMEMORY);

    lpVariableData = (BYTE far *)GlobalLock(hVariableData);
    }

  return (NULL);
}

static int near GetNewHandle()
{
  register WORD i, j;


  for (i = 0; i < wHandles; i++)
    {
    if (lpHandleData[i] == -1)
      break;
    }

  if (i >= wHandles)
    {
    GlobalUnlock(hHandleData);
    if ((hHandleData = GlobalReAlloc(hHandleData, (DWORD)wHandles * 2L + 16L, 0)) == 0)
      {
      return (-1);
      }
    lpHandleData = (WORD far *)GlobalLock(hHandleData);

    j = (WORD)(GlobalSize(hHandleData) / 2L);
    for (i = wHandles; i < j; i++)
      lpHandleData[i] = 0xFFFF;

    i = wHandles;
    wHandles = j;
    }

  return (i);
}

int IdentifyVariable(szVariable)
  char *szVariable;
{
  HANDLE hResInfo, hResData;
  LPSTR  lpTokens, lpKey;
  register i;


  strupr(szVariable);
  if ((i = GetVariableHandle(szVariable)) != -1)
    return (i);

  hResInfo = FindResource(hWndInst, MAKEINTRESOURCE(IDU_TOKENS), MAKEINTRESOURCE(USER));
  hResData = LoadResource(hWndInst, hResInfo);
  lpTokens = LockResource(hResData);
  if (hResInfo == NULL || hResData == NULL)
    return (ERRSYS_NOMEMORY);

  i = strlen(szVariable);

  lpKey = lpTokens;
  while (*lpKey)
    {
    if (lmemcmp(lpKey, szVariable, i+1) == 0)
      {
      i = *((int far *)(lpKey+i+1));

      if ((i & CL_SYVAR) == 0)
	i = -1;
      else
	i |= 0x8000;
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
    i = -1;
  GlobalUnlock(hResData);
  FreeResource(hResData);

  return (i);
}

int GetSysVariableAny(wToken, byBuf, byLen, lInteger)
  WORD wToken;
  BYTE *byBuf;
  BYTE *byLen;
  long *lInteger;
{
  char buf[64];
  register BYTE *ptr;

  ptr = 0;
  switch (wToken & 0x7FFF)
    {
    case TK_ANSWERBACK:
      ptr = Term.szAnswerback;
      break;

    case TK_ANSWERSETUP:
      ptr = Modem.szAnswerSetup;
      break;

    case TK_AUTOWRAP:
      *lInteger = (long)Term.fAutoWrap;
      break;

    case TK_BACKUPS:
      *lInteger = (long)fBackups;
      break;

    case TK_BLANKEX:
      ptr = Xfer.szBlankEx;
      break;

    case TK_BREAKLEN:
      *lInteger = (long)Comm.nBreakLen;
      break;

    case TK_CHOICE:
      *lInteger = (long)nWorkChoice;
      break;

    case TK_COLUMNS:
      *lInteger = (long)Term.nColumns;
      break;

    case TK_CURSHAPE:
      *lInteger = (long)Term.cCurShape - 1;
      break;

    case TK_DATABITS:
      *lInteger = (long)Comm.cDataBits;
      break;

    case TK_DATEMODE:
      *lInteger = (long)Xfer.fDateMode;
      break;

    case TK_DEFINPUT:
      *lInteger = (long)cDefInputFile;
      break;

    case TK_DEFOUTPUT:
      *lInteger = (long)cDefOutputFile;
      break;

    case TK_DESCRIPTION:
      ptr = Sess.szDescription;
      break;

    case TK_DIALPREFIX:
      ptr = Modem.szDialPrefix;
      break;

    case TK_DIALSUFFIX:
      ptr = Modem.szDialSuffix;
      break;

    case TK_DISPLAY:
      *lInteger = (long)bDisplayOn;
      break;

    case TK_ECHO:
      *lInteger = (long)Term.fEcho;
      break;

    case TK_EDITOR:
      ptr = szEditor;
      break;

    case TK_ENTER:
      ptr = Term.szEnter;
      break;

    case TK_FLOW:
      *lInteger = (long)Comm.cFlow;
      break;

    case TK_FOOTER:
      ptr = Sess.szFooter;
      break;

    case TK_FORMFEED:
      *lInteger = (long)Term.fFormFeed;
      break;

    case TK_FRAMINGERRORS:
      *lInteger = (long)nCommFramingErrors;
      break;

    case TK_GRAPHICS:
      *lInteger = (long)Sess.fGraphics;
      break;

    case TK_HEADER:
      ptr = Sess.szHeader;
      break;

    case TK_HOSTSCRIPT:
      *lInteger = (long)fHostScript;
      break;

    case TK_HOSTMODE:
      *lInteger = (long)Xfer.fHostMode;
      break;

    case TK_LDNUMBER:
      ptr = szLdNumber;
      break;

    case TK_LFAUTO:
      *lInteger = (long)Term.fLfAuto;
      break;

    case TK_LINEDELIM:
      ptr = Xfer.szLineDelim;
      break;

    case TK_LINETIME:
      *lInteger = (long)Xfer.cLineTime;
      break;

    case TK_LOCAL:
      *lInteger = (long)Comm.cLocal;
      break;

    case TK_MAXERRORS:
      *lInteger = (long)Xfer.cMaxErrors;
      break;

    case TK_MESSAGE:
      strcpy(byBuf, sInfoMessage);
      *byLen = (BYTE)strlen(byBuf);
      return (0);
      break;

    case TK_MISC:
      ptr = Sess.szMisc;
      break;

    case TK_MODEMCHANGE:
      *lInteger = (char)Modem.cChange;
      break;

    case TK_MODEMHANGUP:
      ptr = Modem.szModemHangup;
      break;

    case TK_MODEMINIT:
      ptr = Modem.szModemInit;
      break;

    case TK_MODEMNAME:
      ptr = Modem.szName;
      break;

    case TK_MODEMRESULTS:
      {
      register i;

      i = datcpy(byBuf, Modem.szCommandOk, 0);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen   = (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szCommandError, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szConnect, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szConnectSpeed, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szConnectARQ, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szDialTone, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szRingDetect, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szNoDialTone, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szBusy, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      i = datcpy(byBuf, Modem.szNoCarrier, *byLen);
      byBuf[i] = ',';
      byBuf   += i + 1;
      *byLen  += (BYTE)(i + 1);
      return (0);
      }
      break;

    case TK_MODIFIER:
      ptr = Sess.szModifier;
      break;

    case TK_NETID:
      ptr = Sess.szNetID;
      break;

    case TK_NEWLINE:
      *lInteger = (long)Term.fNewLine;
      break;

    case TK_NOCOLOR:
      *lInteger = (long)Term.fNoColor;
      break;

    case TK_NUMBER:
      ptr = Sess.szNumber;
      break;

    case TK_ONTIME:
      *lInteger = lTimeConnected;
      break;

    case TK_OUTNUMBER:
      ptr = szOutNumber;
      break;

    case TK_OVERRUNERRORS:
      *lInteger = (long)nCommOverrunErrors;
      break;

    case TK_PACKETSIZE:
      *lInteger = (long)Xfer.nPacketSize;
      break;

    case TK_PARITY:
      *lInteger = (long)Comm.cParity;
      break;

    case TK_PARITYERRORS:
      *lInteger = (long)nCommParityErrors;
      break;

    case TK_PASSWORD:
      ptr = Sess.szPassword;
      break;

    case TK_PATIENCE:
      *lInteger = (long)Sess.nPatience;
      break;

    case TK_PORT:
      *lInteger = (long)cCommPort + 1;
      break;

    case TK_PRINTER:
      *lInteger = (long)bPrinterOn;
      break;

    case TK_PRINTEXTENT:
      *lInteger = (long)Term.fPrintExtent;
      break;

    case TK_PROTOCOL:
      LoadString(hWndInst, IDS_PROTOCOL_FIRST+Xfer.cProtocol, byBuf, 255);
      *byLen = (BYTE)strlen(byBuf);
      return (0);
      break;

    case TK_RECOVERY:
      *lInteger = (long)Xfer.fRecovery;
      break;

    case TK_RECVMODE:
      *lInteger = (long)Xfer.cRecvMode;
      break;

    case TK_REDIALCOUNT:
      *lInteger = (long)Sess.nRedialCount;
      break;

    case TK_REDIALWAIT:
      *lInteger = (long)Sess.nRedialWait;
      break;

    case TK_REVIEW:
      *lInteger = (long)nScrollMemory * 1024L;
      break;

    case TK_SCRIPT:
      ptr = Sess.szScript;
      break;

    case TK_SECRET:
      *lInteger = (long)fSecret;
      break;

    case TK_SHOWACTIVE:
      *lInteger = (long)Disp.fMacron;
      break;

    case TK_SHOWINFOBAR:
      *lInteger = (long)Disp.fShowInfoBar;
      break;

    case TK_SHOWINPUT:
      *lInteger = (long)Disp.fShowInput;
      break;

    case TK_SHOWMACROBAR:
      *lInteger = (long)Disp.fShowMacroBar;
      break;

    case TK_SHOWSTATUSBAR:
      *lInteger = (long)Disp.fShowStatusBar;
      break;

    case TK_SHOWHSCROLL:
      *lInteger = (long)Disp.fShowHScroll;
      break;

    case TK_SHOWVSCROLL:
      *lInteger = (long)Disp.fShowVScroll;
      break;

    case TK_SPEED:
      *lInteger = (long)Comm.nSpeed;
      break;

    case TK_STARTUP:
      ptr = szStartupScript;
      break;

    case TK_STOPBITS:
      if (Comm.cStopBits)
        *lInteger = 2L;
      else
        *lInteger = 1L;
      break;

    case TK_SWAPDEL:
      *lInteger = (long)Term.fSwapDel;
      break;

    case TK_SYSTIME:
      *lInteger = GetCurrentTime() / 100L;
      break;

    case TK_TABEX:
      *lInteger = (long)Xfer.fTabEx;
      break;

    case TK_TABWIDTH:
      *lInteger = (long)Term.cTabWidth;
      break;

    case TK_TERMINAL:
      LoadString(hWndInst, IDS_EMULATOR_FIRST+Term.cTerminal, byBuf, 255);
      *byLen = (BYTE)strlen(byBuf);
      while (*byLen && byBuf[*byLen-1] == ' ')
        byBuf[--(*byLen)] = NULL;
      return (0);
      break;

    case TK_TIMING:
      *lInteger = (long)Xfer.cTiming;
      break;

    case TK_USERID:
      ptr = Sess.szUserid;
      break;

    case TK_XOFFCHAR:
      *lInteger = (long)Comm.cXoffChar;
      break;

    case TK_XONCHAR:
      *lInteger = (long)Comm.cXonChar;
      break;

    case TK_WINDOWWRAP:
      *lInteger = (long)Term.fWindowWrap;
      break;

    default:
      return (2);
      break;
    }

  if (ptr)
    {
    *byLen = *(ptr++);
    memcpy(byBuf, ptr, *byLen);
    return (0);
    }
  else
    {
    return (1);
    }
}

int PutSysVariableAny(wToken, byBuf, byLen, lInteger)
  WORD wToken;
  BYTE *byBuf;
  BYTE byLen;
  long lInteger;
{
  char buf[32];
  int  i, nInteger;
  register int	len;
  register BYTE *ptr;


  if (byBuf)
    lInteger = atol(byBuf);
  else
    byLen = 0;

  nInteger = (int)lInteger;

  ptr = 0;
  switch (wToken & 0x7FFF)
    {
    case TK_ANSWERBACK:
      len = sizeof(Term.szAnswerback);
      ptr = Term.szAnswerback;
      break;

    case TK_ANSWERSETUP:
      len = sizeof(Modem.szAnswerSetup);
      ptr = Modem.szAnswerSetup;
      break;

    case TK_AUTOWRAP:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fAutoWrap = (char)nInteger;
      break;

    case TK_BACKUPS:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      fBackups = (char)nInteger;
      break;

    case TK_BLANKEX:
      len = sizeof(Xfer.szBlankEx);
      ptr = Xfer.szBlankEx;
      break;

    case TK_BREAKLEN:
      if (nInteger < 10 || nInteger > 5000)
        return (ERRDEV_BADPARAM);
      Comm.nBreakLen = nInteger;
      return (CommPortUpdate());
      break;

    case TK_CHOICE:
      nWorkChoice = nInteger;
      break;

    case TK_COLUMNS:
      if (lInteger != 80 && lInteger != 132)
	return (ERRTRM_BADPARAM);
      if (Term.cTerminal != TERM_VT102)
	return (ERRTRM_BADPARAM);
      Term.nColumns = (int)lInteger;
      UpdateTerminal();
      break;

    case TK_CURSHAPE:
      if (nInteger <  0 || nInteger > 2)
	return (ERRTRM_BADPARAM);
      Term.cCurShape = (char)(nInteger + 1);
      if (fTermCaretActive)
        {
        VidDeleteCaret(hChildTerminal);
        VidCreateCaret(hChildTerminal);
        VidSetCaretPos();
        ShowCaret(hChildTerminal);
        }
      break;

    case TK_DATABITS:
      if (nInteger < 7 || nInteger > 8)
        return (ERRDEV_BYTESIZE);
      Comm.cDataBits = (char)nInteger;
      return (CommPortUpdate());
      break;

    case TK_DATEMODE:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Xfer.fDateMode = (char)nInteger;
      break;

    case TK_DEFINPUT:
      cDefInputFile = (char)lInteger;
      break;

    case TK_DEFOUTPUT:
      cDefOutputFile = (char)lInteger;
      break;

    case TK_DESCRIPTION:
      len = sizeof(Sess.szDescription);
      ptr = Sess.szDescription;
      break;

    case TK_DIALPREFIX:
      len = sizeof(Modem.szDialPrefix);
      ptr = Modem.szDialPrefix;
      break;

    case TK_DIALSUFFIX:
      len = sizeof(Modem.szDialSuffix);
      ptr = Modem.szDialSuffix;
      break;

    case TK_DISPLAY:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      bDisplayOn = (char)nInteger;
      break;

    case TK_ECHO:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fEcho = (char)nInteger;
      bTermEcho  = (char)nInteger;
      break;

    case TK_EDITOR:
      len = sizeof(szEditor);
      ptr = szEditor;
      break;

    case TK_ENTER:
      len = sizeof(Term.szEnter);
      ptr = Term.szEnter;
      break;

    case TK_FLOW:
      Comm.cFlow = (char)nInteger;
      if (nInteger < 0 || nInteger > 3)
        return (ERRDEV_BADPARAM);
      return (CommPortUpdate());
      break;

    case TK_FOOTER:
      len = sizeof(Sess.szFooter);
      ptr = Sess.szFooter;
      break;

    case TK_FORMFEED:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fFormFeed = (char)nInteger;
      break;

    case TK_FRAMINGERRORS:
      nCommFramingErrors = (int)lInteger;
      break;

    case TK_GRAPHICS:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Sess.fGraphics = (char)nInteger;
      break;

    case TK_HEADER:
      len = sizeof(Sess.szHeader);
      ptr = Sess.szHeader;
      break;

    case TK_HOSTMODE:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Xfer.fHostMode = (char)nInteger;
      break;

    case TK_HOSTSCRIPT:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      fHostScript = (char)nInteger;
      break;

    case TK_LOCAL:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Comm.cLocal = (char)nInteger;
      return (CommPortUpdate());
      break;

    case TK_LDNUMBER:
      len = sizeof(szLdNumber);
      ptr = szLdNumber;
      break;

    case TK_LFAUTO:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fLfAuto = (char)nInteger;
      break;

    case TK_LINEDELIM:
      len = sizeof(Xfer.szLineDelim);
      ptr = Xfer.szLineDelim;
      break;

    case TK_LINETIME:
      if (nInteger < 0 || nInteger > 127)
	return (ERRATH_OUTOFRANGE);
      Xfer.cLineTime = (char)nInteger;
      break;

    case TK_MAXERRORS:
      if (nInteger < 0 || nInteger > 20)
        return (ERRXFR_BADPARAM);
      Xfer.cMaxErrors = (char)nInteger;
      break;

    case TK_MODEMCHANGE:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Modem.cChange = (char)nInteger;
      break;

    case TK_MODEMHANGUP:
      len = sizeof(Modem.szModemHangup);
      ptr = Modem.szModemHangup;
      break;

    case TK_MODEMINIT:
      len = sizeof(Modem.szModemInit);
      ptr = Modem.szModemInit;
      break;

    case TK_MODEMNAME:
      {
      register i;
      char buf[32];

      for (i = IDS_MODEM_FIRST; i <= IDS_MODEM_LAST; i++)
        {
        LoadString(hWndInst, i, buf, sizeof(buf));
        *strchr(buf, ',') = NULL;
        if (memicmp(byBuf, buf, byLen) == 0)
          {
          ReadModemParameters(i);
          return (0);
          }
        }

      return (ERRDEV_BADMODEM);
      }
      break;

    case TK_MODEMRESULTS:
      byBuf = comcpy(Modem.szCommandOk,    sizeof(Modem.szCommandOk),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szCommandError, sizeof(Modem.szCommandError), byBuf, &byLen);
      byBuf = comcpy(Modem.szConnect,	   sizeof(Modem.szConnect),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szConnectSpeed, sizeof(Modem.szConnectSpeed), byBuf, &byLen);
      byBuf = comcpy(Modem.szConnectARQ,   sizeof(Modem.szConnectARQ),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szDialTone,	   sizeof(Modem.szDialTone),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szRingDetect,   sizeof(Modem.szRingDetect),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szNoDialTone,   sizeof(Modem.szNoDialTone),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szBusy,	   sizeof(Modem.szBusy),	 byBuf, &byLen);
      byBuf = comcpy(Modem.szNoCarrier,    sizeof(Modem.szNoCarrier),	 byBuf, &byLen);
      return (0);
      break;

    case TK_MISC:
      len = sizeof(Sess.szMisc);
      ptr = Sess.szMisc;
      break;

    case TK_MODIFIER:
      len = sizeof(Sess.szModifier);
      ptr = Sess.szModifier;
      break;

    case TK_NETID:
      len = sizeof(Sess.szNetID);
      ptr = Sess.szNetID;
      break;

    case TK_NEWLINE:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fNewLine = (char)nInteger;
      break;

    case TK_NOCOLOR:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fNoColor = (char)nInteger;
      break;

    case TK_NUMBER:
      len = sizeof(Sess.szNumber);
      ptr = Sess.szNumber;
      break;

    case TK_ONTIME:
      lTimeConnected = lInteger;
      break;

    case TK_OUTNUMBER:
      len = sizeof(szOutNumber);
      ptr = szOutNumber;
      break;

    case TK_OVERRUNERRORS:
      nCommOverrunErrors = (int)lInteger;
      break;

    case TK_PACKETSIZE:
      if (nInteger < 128 || nInteger > 1024)
        return (ERRXFR_BADPARAM);
      Xfer.nPacketSize = nInteger;
      break;

    case TK_PARITY:
      if (nInteger < 0 || nInteger > 4)
        return (ERRDEV_BADPARAM);
      Comm.cParity = (char)nInteger;
      return (CommPortUpdate());
      break;

    case TK_PARITYERRORS:
      nCommParityErrors = (int)lInteger;
      break;

    case TK_PASSWORD:
      len = sizeof(Sess.szPassword);
      ptr = Sess.szPassword;
      break;

    case TK_PATIENCE:
      if (lInteger < 10 || lInteger > 999)
	return (ERRATH_OUTOFRANGE);
      Sess.nPatience = (int)lInteger;
      break;

    case TK_PORT:
	// dca00082 PJL Use MAX_PORT constant for highest port #.
      if (nInteger < 1 || nInteger > MAX_PORT)
	return (ERRDEV_BADID);
      if (bConnected)
	return (ERRDEV_OPEN);
      Comm.cPort = (char)(nInteger - 1);
      CommPortUpdate();
      break;

    case TK_PRINTER:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      if ((nInteger && !bPrinterOn) || (nInteger == 0L && bPrinterOn))
	TogglePrinter();
      break;

    case TK_PRINTEXTENT:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fPrintExtent = (char)nInteger;
      break;

    case TK_PROTOCOL:
      {
      LibFilterString(byBuf, byLen);
      if (memicmp("XTALK", byBuf, byLen) == 0)
        i = XFER_XTALK;
      else if (memicmp("CSERVEB", byBuf, byLen) == 0)
        i = XFER_CSERVEB;
      else
        {
        register j;

        j = 0;
        for (i = IDS_PROTOCOL_FIRST; i <= IDS_PROTOCOL_LAST; i++)
          {
          LoadString(hWndInst, i, buf, sizeof(buf));
	  LibFilterString(buf, (BYTE)strlen(buf));
          if (memicmp(buf, byBuf, byLen) == 0)
            {
            Xfer.cProtocol = (char)(j);
            UpdateProtocol();
            return (0);
            }
          else
            j++;
          }
        return (ERRXFR_BADPROTOCOL);
        }

      Xfer.cProtocol = (char)i;
      UpdateProtocol();
      return (0);
      }
      break;

    case TK_RECOVERY:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Xfer.fRecovery = (char)nInteger;
      break;

    case TK_RECVMODE:
      if (nInteger < 0 || nInteger > 2)
        return (ERRATH_OUTOFRANGE);
      Xfer.cRecvMode = (char)nInteger;
      break;

    case TK_REDIALCOUNT:
      if (lInteger < 0 || lInteger > 99)
	return (ERRATH_OUTOFRANGE);
      Sess.nRedialCount = (int)lInteger;
      break;

    case TK_REDIALWAIT:
      if (lInteger < 0 || lInteger > 99)
	return (ERRATH_OUTOFRANGE);
      Sess.nRedialWait = (int)lInteger;
      break;

    case TK_REVIEW:
      if (lInteger >= 65536)
	nScrollMemory = 64;
      else if (lInteger >= 32768)
	nScrollMemory = 32;
      else if (lInteger >= 16384)
	nScrollMemory = 16;
      else
	nScrollMemory = 0;
      VssDelete();
      VssCreate();
      break;

    case TK_SCRIPT:
      len = sizeof(Sess.szScript);
      ptr = Sess.szScript;
      break;

    case TK_SECRET:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      fSecret = (char)nInteger;
      break;

    case TK_SHOWACTIVE:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fMacron = (char)nInteger;
      UpdateMacro();
      break;

    case TK_SHOWINFOBAR:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fShowInfoBar = (char)nInteger;
      ResetScreenDisplay();
      break;

    case TK_SHOWINPUT:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fShowInput = (char)nInteger;
      ResetScreenDisplay();
      break;

    case TK_SHOWMACROBAR:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fShowMacroBar = (char)nInteger;
      ResetScreenDisplay();
      break;

    case TK_SHOWSTATUSBAR:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fShowStatusBar = (char)nInteger;
      ResetScreenDisplay();
      break;

    case TK_SHOWHSCROLL:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fShowHScroll = (char)nInteger;
      ResetScreenDisplay();
      break;

    case TK_SHOWVSCROLL:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Disp.fShowVScroll = (char)nInteger;
      ResetScreenDisplay();
      break;

    case TK_SPEED:
      if (nInteger != 110 && nInteger != 300 && nInteger != 600 &&
          nInteger != 1200 && nInteger != 2400 && nInteger != 4800 &&
          nInteger != 9600 && nInteger != 19200)
        return (ERRDEV_BAUDRATE);
      Comm.nSpeed = (int)nInteger;
      return (CommPortUpdate());
      break;

    case TK_STARTUP:
      len = sizeof(szStartupScript);
      ptr = szStartupScript;
      break;

    case TK_STOPBITS:
      if (nInteger < 1 || nInteger > 2)
        return (ERRDEV_BADPARAM);
      if (nInteger == 1)
        Comm.cStopBits = 0;
      else
        Comm.cStopBits = 2;
      return (CommPortUpdate());
      break;

    case TK_SWAPDEL:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fSwapDel = (char)nInteger;
      break;

    case TK_TABEX:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Xfer.fTabEx = (char)nInteger;
      break;

    case TK_TABWIDTH:
      if (nInteger < 0 || nInteger > 16)
        return (ERRTRM_BADPARAM);
      Term.cTabWidth = (char)nInteger;
      memset(bTermTabs, FALSE, sizeof(bTermTabs));
      // 00090 Check for zero tab width
      if (Term.cTabWidth)
        {
        for (i = Term.cTabWidth; i < sizeof(bTermTabs); i += Term.cTabWidth)
          bTermTabs[i] = TRUE;
        }
      break;

    case TK_TERMINAL:
      {
      register j;

      LibFilterString(byBuf, byLen);

      j= 0;
      for (i = IDS_EMULATOR_FIRST; i <= IDS_EMULATOR_LAST; i++)
        {
        LoadString(hWndInst, i, buf, sizeof(buf));
	LibFilterString(buf, (BYTE)strlen(buf));
        if (memicmp(buf, byBuf, byLen) == 0)
          {
          Term.cTerminal = (char)(j);
	  // 00049 Reset terminal to defaults.
	  UpdateNewTerminal();
          return (0);
          }
        else
          j++;
        }
      return (ERRTRM_BADTERMINAL);
      }
      break;

    case TK_TIMING:
      if (nInteger < 0 || nInteger > 3)
        return (ERRXFR_BADPARAM);
      Xfer.cTiming = (char)nInteger;
      break;

    case TK_USERID:
      len = sizeof(Sess.szUserid);
      ptr = Sess.szUserid;
      break;

    case TK_XOFFCHAR:
      Comm.cXoffChar = (char)nInteger;
      return (CommPortUpdate());
      break;

    case TK_XONCHAR:
      Comm.cXonChar = (char)nInteger;
      return (CommPortUpdate());
      break;

    case TK_WINDOWWRAP:
      if (nInteger != 0 && nInteger != 1)
        return (ERRATH_NOTONOROFF);
      Term.fWindowWrap = (char)nInteger;
      break;

    default:
      return (TRUE);
      break;
    }

  if (ptr)
    {
    if (byLen + 1 > (BYTE)len)
      byLen = (BYTE)(len - 1);
    *(ptr++) = byLen;
    memcpy(ptr, byBuf, byLen);
    }

  return (FALSE);
}

static BYTE near datcpy(pOut, byBuf,  byLen)
  char *pOut;
  BYTE *byBuf;
  BYTE byLen;
{
  int  nLen;
  register i;

  nLen = byBuf[0];
  if (nLen > 254-byLen)
    nLen = 254-byLen;

  for (i = 0; i < byBuf[0]; i++)
    *(pOut++) = byBuf[i+1];
  return (byBuf[0]);
}

static BYTE * near comcpy(pOut, nLen, byBuf, byLen)
  char *pOut;
  int  nLen;
  BYTE *byBuf;
  BYTE *byLen;
{
  register i;

  i = 1;
  while (1)
    {
    if (*byBuf == ',' || i >= nLen || *byLen == 0)
      break;
    pOut[i] = *(byBuf++);
    (*byLen)--;
    i++;
    }
  pOut[0] = (char)(i - 1);
  while (*byBuf != ',' && *byLen)
    {
    byBuf++;
    (*byLen)--;
    }
  if (*byLen)
    {
    byBuf++;
    (*byLen)--;
    }
  return (byBuf);
}
