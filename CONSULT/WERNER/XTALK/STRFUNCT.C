/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   String Function Module (STRFUNCT.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the string functions for the interpreter.  *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*	      1.01 12/25/89 00075 PAD doesn't pad with a string.              *
*             1.01 01/17/89 00081 Implement ARG(0) (EvalStrFunct) PJL/JBD.    *
*		1.1 04/12/90 dca00083 PJL Update Windx Version number.				   *
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
#include <direct.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "compile.h"
#include "interp.h"
#include "variable.h"
#include "capture.h"
#include "comm.h"
#include "macro.h"
#include "term.h"
#include "prodata.h"
#include "properf.h"
#include "xfer.h"

// dca00083 PJL define Crosstalk for Windows Version Number
#define WINDX_VERSION_MAJOR 1
#define WINDX_VERSION_MINOR 1

#define OpCode(x)	(*((WORD far *)(&lpObjectCode[x])))
  extern WORD wAddr;

  extern unsigned char near cdecl _osmajor;
  extern unsigned char near cdecl _osminor;
  extern char * cdecl getenv(char *);

  static int near SkipComma();
  static int near SkipRPAR();
  static int near IsByteInString(BYTE, BYTE *, BYTE);
  static int near IsByteInStringClass(BYTE, BYTE *, BYTE, int);

  struct find_t filefind;


/*---------------------------------------------------------------------------*/
/*  Evaluate String Function									    */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalStrFunct(pBuf, pLen)
  BYTE *pBuf;
  BYTE *pLen;
{
  struct find_t buffer;
  char	 szPath[MAX_PATH];
  DWORD  dwBuf;
  long	 lBuf;
  int	 nWork;
  BYTE	 *pBuf1, bLen, *pBuf2, bLen2, *ptr, bWork;
  register i;


  if (nStackAddr + 768 >= nStackHigh)
    return (ERRINT_TOOCOMPLEX);
  pBuf1 = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;
  pBuf2 = &filexfer.stack.data[nStackAddr];
  nStackAddr += 512;

  bLen = *pLen;
  i = 0;
  wAddr += 2;
  switch (OpCode(wAddr - 2))
    {
    case TK_ARG:
      if (OpCode(wAddr) != TK_LPAR)
	{
	*pLen = (BYTE)GetStrVariable(VAR_ARGLINE, pBuf1, 255);
	}
      else
	{
	wAddr += 2;
	if ((i = EvalInteger(&lBuf)) == 0)
	  {
          // 00081 PJL & JBD - 0L ok also
	  // if (lBuf < 1L || lBuf > 9)
	  if (lBuf < 0L || lBuf > 9L)
	    {
	    i = ERRATH_OUTOFRANGE;
	    break;
	    }

          // 00081 PJL & JBD - implement ARG(0)
	  // GetStrVariable(VAR_ARGLINE, pBuf1, 255);
	  *pLen = (BYTE)GetStrVariable(VAR_ARGLINE, pBuf1, 255);
          if (lBuf == 0L)
            break;

	  while (lBuf-- != 1L && *pBuf1)
	    {
	    while (*pBuf1 != ' ' && *pBuf1)
	      pBuf1++;
	    while (*pBuf1 == ' ' && *pBuf1)
	      pBuf1++;
	    if (*pBuf1 == ',')
	      {
	      pBuf1++;
	      while (*pBuf1 == ' ' && *pBuf1)
		pBuf1++;
	      }
	    }

	  ptr = pBuf1;
	  while (*ptr != ' ' && *ptr != ',' && *ptr)
	    ptr++;
	  *ptr = NULL;

	  *pLen = (BYTE)strlen(pBuf1);
	  }
	}
      break;

    case TK_BINARY:
      wAddr += 2;
      if (i = EvalInteger(&dwBuf))
	break;
      memset(pBuf1, '0', 32);
      if (dwBuf < 256L)
	{
	dwBuf = dwBuf << 24;
	*pLen = 8;
	}
      else if (dwBuf < 65536L)
	{
	dwBuf = dwBuf << 16;
	*pLen = 16;
	}
      else
	{
	*pLen = 32;
	}
      for (i = 0, ptr = pBuf1; i < *pLen; i++, ptr++)
	{
	if (dwBuf & 0x80000000L)
	  *ptr = '1';
	dwBuf = dwBuf << 1;
	}

      i = 0;
      break;

    case TK_BITSTRIP:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	bWork = (BYTE)lBuf;
	}
      else
	{
	bWork = 0x7F;
	if (i = SkipRPAR())
	  break;
	}

      for (i = 0; i < *pLen; i++)
	pBuf1[i] &= bWork;

      i = 0;
      break;

    case TK_BOOKNAME:
      {
      wAddr += 2;
      if (i = EvalInteger(&lBuf))
	break;
      if (lBuf < 1L || lBuf > 4096L)
	{
	i = ERRATH_OUTOFRANGE;
	break;
	}

      *pBuf1 = NULL;
      CreatePath(szPath, VAR_DIRXWP, "*");

      i = _dos_findfirst(szPath, _A_NORMAL, &buffer);
      while (i == 0)
	{
        if (--lBuf == 0)
	  {
	  strcpy(pBuf1, buffer.name);
	  break;
	  }

	i = _dos_findnext(&buffer);
	}

      *strchr(pBuf1, '.') = NULL;
      *pLen = (BYTE)strlen(pBuf1);
      i = 0;
      }
      break;

    case TK_CAPFILE:
      strcpy(pBuf1, szCaptureName);
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_CHR:
      wAddr += 2;
      if (i = EvalInteger(&lBuf))
	break;

      *pBuf1 = (BYTE)lBuf;
      *pLen  = 1;
       i = 0;
      break;

    case TK_CURDIR:
      if (OpCode(wAddr) == TK_LPAR)
	{
	unsigned drive, max_drives;

	wAddr += 2;
        if (i = EvalString(pBuf1, pLen))
          break;
        *pBuf1 = toupper(*pBuf1);
        if (*pBuf1 < 'A' || *pBuf1 > 'Z')
	  {
	  i = ERRATH_OUTOFRANGE;
	  break;
	  }

	_dos_getdrive(&drive);
        _dos_setdrive(*pBuf1 - '@', &max_drives);
        if ((unsigned)(*pBuf1 - '@') <= max_drives)
	  DosQCurDir(0, pBuf1, MAX_PATH);
	else
	  *pBuf1 = NULL;
	_dos_setdrive(drive, &max_drives);
        i = SkipRPAR();
	}
      else
	{
	DosQCurDir(0, pBuf1, MAX_PATH);
	}
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_CURDRIVE:
      _dos_getdrive(&nWork);
      pBuf1[0] = (BYTE)('@' + nWork);
      pBuf1[1] = ':';
      *pLen = 2;
      break;

    case TK_DATE:
      {
      if (OpCode(wAddr) == TK_LPAR)
	{
	char fmt[16];
	int  nYear, nMonth, nDay, nFeb;

	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;

	nYear  = (int)(lBuf * 100L / 36525L);
	nDay   = (int)(lBuf - (long)(nYear * 365 + nYear / 4));
	if (nYear % 4 == 0)
	  nFeb = 29;
	else
	  nFeb = 28;

	nMonth = 1;
	if (nDay > 31 && nMonth ==  1) { nDay -= 31; nMonth++; }
	if (nDay > nFeb && nMonth ==  2) { nDay -= nFeb; nMonth++; }
	if (nDay > 31 && nMonth ==  3) { nDay -= 31; nMonth++; }
	if (nDay > 30 && nMonth ==  4) { nDay -= 30; nMonth++; }
	if (nDay > 31 && nMonth ==  5) { nDay -= 31; nMonth++; }
	if (nDay > 30 && nMonth ==  6) { nDay -= 30; nMonth++; }
	if (nDay > 31 && nMonth ==  7) { nDay -= 31; nMonth++; }
	if (nDay > 31 && nMonth ==  8) { nDay -= 31; nMonth++; }
	if (nDay > 30 && nMonth ==  9) { nDay -= 30; nMonth++; }
	if (nDay > 31 && nMonth == 10) { nDay -= 31; nMonth++; }
	if (nDay > 30 && nMonth == 11) { nDay -= 30; nMonth++; }

	fmt[ 0] = '%';
	fmt[ 1] = '0';
	fmt[ 2] = '2';
	fmt[ 3] = 'd';
	fmt[ 4] = '/';
	fmt[ 5] = '%';
	fmt[ 6] = '0';
	fmt[ 7] = '2';
	fmt[ 8] = 'd';
	fmt[ 9] = '/';
	fmt[10] = '%';
	fmt[11] = '0';
	fmt[12] = '2';
	fmt[13] = 'd';
	fmt[14] = NULL;
	sprintf(pBuf1, fmt, nMonth, nDay, nYear);
	i = 0;
	}
      else
	{
	_strdate(pBuf1);
	}
      *pLen = 8;
      }
      break;

    case TK_DEHEX:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      for (i = 0; i < *pLen; i++)
	{
	if (pBuf1[i*2+0] > '9')
	  bWork = (pBuf1[i*2+0] - 'A' + 10) * 16;
	else
	  bWork = (pBuf1[i*2+0] - '0') * 16;
	if (pBuf1[i*2+1] > '9')
	  bWork += pBuf1[i*2+1] - 'A' + 10;
	else
	  bWork += pBuf1[i*2+1] - '0';
	pBuf1[i] = bWork;
	}
      *pLen /= 2;
      i = SkipRPAR();
      break;

    case TK_DELETE:
      if (OpCode(wAddr) == TK_LPAR)
	{
	int nStart, nLen;

        nStart = 1;
        nLen   = 1;

	wAddr += 2;
	if (i = EvalString(pBuf1, pLen))
          break;
        if (OpCode(wAddr) == TK_COMMA)
          {
          wAddr += 2;
          if (i = EvalInteger(&lBuf))
            break;
          nStart = (int)lBuf;

          if (OpCode(wAddr) == TK_COMMA)
            {
            wAddr += 2;
            if (i = EvalInteger(&lBuf))
              break;
            nLen = (int)lBuf;
            }
          }
        else
          i = SkipRPAR();

	if (nStart > 0 && nStart > *pLen && nLen > 0)
	  {
	  i = ERRATH_OUTOFRANGE;
	  break;
	  }

	for (i = nStart-1; i < (int)(*pLen) - nLen; i++)
	  pBuf1[i] = pBuf1[i+nLen];
	pBuf1[i] = NULL;

	*pLen = (BYTE)strlen(pBuf1);
	i = 0;
	}
      else
	i = ERRINT_MISPARAM;
      break;

    case TK_DESTORE:
      wAddr += 2;
      if (i = EvalString(pBuf2, pLen))
	break;
      LibDestore(pBuf2, pBuf1, 255);
      *pLen = *pBuf1;
      pBuf1++;
      i = SkipRPAR();
      break;

    case TK_DETEXT:
      {
      register j;

      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      for (i = 0, j = 0; i < *pLen; i += 4)
	{
	pBuf1[j++] = pBuf1[i+0] - 32 + ((pBuf1[i+3] - 32) & 0x03) * 64;
	if (pBuf1[i+1] == 0x60)
	  break;
	else
	  pBuf1[j++] = pBuf1[i+1] - 32 + ((pBuf1[i+3] - 32) & 0x0C) * 16;
	if (pBuf1[i+2] == 0x60)
	  break;
	else
	  pBuf1[j++] = pBuf1[i+2] - 32 + ((pBuf1[i+3] - 32) & 0x30) * 4;
	}

      *pLen = (BYTE)j;
      i = SkipRPAR();
      }
      break;

    case TK_DOSVERSION:
      {
      char fmt[16];

      fmt[0] = '%';
      fmt[1] = 'd';
      fmt[2] = '.';
      fmt[3] = '%';
      fmt[4] = '0';
      fmt[5] = '2';
      fmt[6] = 'd';
      fmt[7] = NULL;

      sprintf(pBuf1, fmt, _osmajor, _osminor);
      *pLen = (BYTE)strlen(pBuf1);
      }
      break;

    case TK_ENHEX:
      wAddr += 2;
      if (i = EvalString(pBuf2, pLen))
	break;
      if (*pLen > 127)
	i = ERRINT_LONGSTRING;
      else
	{
	for (i = 0; i < *pLen; i++)
	  {
	  bWork = pBuf2[i] >> 4;
	  if (bWork > 9)
	    pBuf1[i*2+0] = bWork - 10 + 'A';
	  else
	    pBuf1[i*2+0] = bWork + '0';
	  bWork = pBuf2[i] & 0x0F;
	  if (bWork > 9)
	    pBuf1[i*2+1] = bWork - 10 + 'A';
	  else
	    pBuf1[i*2+1] = bWork + '0';
	  }

	*pLen *= 2;
	i = SkipRPAR();
	}
      break;

    case TK_ENSTORE:
      wAddr += 2;
      if (i = EvalString(pBuf2+1, pLen))
	break;
      pBuf2[0] = *pLen;
      LibEnstore(pBuf2, pBuf1, 256);
      *pLen = (BYTE)strlen(pBuf1);
      i = SkipRPAR();
      break;

    case TK_ENTEXT:
      {
      register j;

      wAddr += 2;
      if (i = EvalString(pBuf2, pLen))
	break;

      if (*pLen > 189)
	{
	i = ERRINT_LONGSTRING;
	break;
	}

      for (i = 0, j = 0; i < *pLen;)
	{
	bWork  = (pBuf2[i] & 0xC0) >> 6;
	pBuf1[j++] = (BYTE)(pBuf2[i++] & 0x3F) + 32;
	if (i < *pLen)
	  {
	  bWork |= (pBuf2[i] & 0xC0) >> 4;
	  pBuf1[j++] = (BYTE)(pBuf2[i++] & 0x3F) + 32;
	  }
	else
	  pBuf1[j++] = 0x60;
	if (i < *pLen)
	  {
	  bWork |= (pBuf2[i] & 0xC0) >> 2;
	  pBuf1[j++] = (BYTE)(pBuf2[i++] & 0x3F) + 32;
	  }
	else
	  pBuf1[j++] = 0x60;
	pBuf1[j++] = (BYTE)(bWork + 32);
	}

      *pLen = (BYTE)j;
      i = SkipRPAR();
      }
      break;

    case TK_ENVIRON:
      wAddr += 2;
      if (i = EvalString(pBuf2, pLen))
        break;
      strupr(pBuf2);
      ptr = getenv(pBuf2);
      if (ptr)
	strcpy(pBuf1, ptr);
      else
	*pBuf1 = NULL;
      *pLen = (BYTE)strlen(pBuf1);
      i = SkipRPAR();
      break;

    case TK_EXTRACT:
      {
      WORD wSaveAddr;
      int  nClass, nWhere;

      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      nClass = -1;
      nWhere = 1;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	wSaveAddr = wAddr;
	if (i = EvalString(pBuf2, &bLen2))
	  {
	  if (i != ERRINT_MIXTYPES)
	    break;
	  wAddr = wSaveAddr;
	  if (i = EvalRange(&nClass, 0, 255, 0))
	    break;
          if (OpCode(wAddr - 2) == TK_RPAR)
            wAddr -= 2;
	  }
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalRange(&nWhere, 0, 3, 0))
	    break;
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
	  }
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	}

      if (nWhere == 0)
	{
	register j;
	for (i = j = 0; i < *pLen; i++)
	  {
	  if (IsByteInStringClass(pBuf1[i], pBuf2, bLen2, nClass))
	    pBuf1[j++] = pBuf1[i];
	  }
	*pLen = (BYTE)j;
	}
      if (nWhere == 1 || nWhere == 3)
	{
	for (i = *pLen; i > 0; i--)
	  {
	  if (!IsByteInStringClass(pBuf1[i-1], pBuf2, bLen2, nClass))
	    break;
	  }
	pBuf1  = &pBuf1[i];
	*pLen -= (BYTE)i;
	}
      if (nWhere == 2 || nWhere == 3)
	{
	for (i = 0; i < *pLen; i++)
	  {
	  if (!IsByteInStringClass(pBuf1[i], pBuf2, bLen2, nClass))
	    break;
	  }
	*pLen = (BYTE)i;
	}

      i = 0;
      }
      break;

    case TK_FILEFIND:
      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalString(pBuf1, pLen))
	  break;
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalInteger(&lBuf))
	    break;
	  nWork = (int)lBuf;
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
	  nWork = 0;
	  }
	if (i = _dos_findfirst(pBuf1, nWork, &filefind))
	  {
	  *pBuf1 = NULL;
	  }
	else
	  {
	  strcpy(pBuf1, filefind.name);
	  }
	}
      else
	{
	if (_dos_findnext(&filefind))
	  *pBuf1 = NULL;
	else
	  strcpy(pBuf1, filefind.name);
	}

      i = 0;
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_FKEY:
      wAddr += 2;
      if (i = EvalInteger(&lBuf))
	break;
      if (lBuf < 1L || lBuf > 48L)
	{
	i = ERRATH_OUTOFRANGE;
	break;
	}
      strcpy(pBuf1, Macro.szMacro[((int)lBuf)-1]);
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_FNSTRIP:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalInteger(&lBuf))
	break;
      bWork = (BYTE)lBuf;

      if (bWork & 0x01)
	{
	if (pBuf1[1] == ':')
	 pBuf1 += 2;
	}

      if (bWork & 0x02)
	{
	ptr = 0;
	for (i = 0; pBuf1[i]; i++)
	  {
	  if (pBuf1[i] == '\\')
	    ptr = &pBuf1[i];
	  }
	if (ptr)
	  {
	  if (pBuf1[1] == ':')
	    strcpy(&pBuf1[2], ptr+1);
	  else
	    strcpy(pBuf1, ptr+1);
	  }
	}

      if (bWork & 0x04)
	{
	if (ptr = strchr(pBuf1, '.'))
	  *ptr = NULL;
	}

      if (bWork & 0x08)
	strupr(pBuf1);
      else if (bWork & 0x10)
	strlwr(pBuf1);
      i = 0;
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_HEX:
      wAddr += 2;
      if (i = EvalInteger(&dwBuf))
	break;
      if (dwBuf < 65536L)
	{
	dwBuf = dwBuf << 16;
	*pLen = 4;
	}
      else
	{
	*pLen = 8;
	}
      for (i = 0; i < *pLen; i++)
	{
	bWork = (BYTE)(dwBuf >> 28) & 0x0F;
	if (bWork > 9)
	  pBuf1[i] = (BYTE)('A' - 10 + bWork);
	else
	  pBuf1[i] = (BYTE)('0' + bWork);

	dwBuf = dwBuf << 4;
	}
      i = 0;
      break;

    case TK_HMS:
      {
      int nHours, nMinutes, nSeconds, nTenths;

      wAddr += 2;
      if (i = EvalInteger(&lBuf))
	break;
      nTenths	= (int)(lBuf % 10L);
      nSeconds	= (int)((lBuf / 10L) % 60L);
      nMinutes	= (int)((lBuf / 600L) % 60L);
      nHours	= (int)(lBuf / 36000L);

      if (OpCode(wAddr-2) != TK_RPAR && OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	nWork = (int)lBuf;
	}
      else
	{
	nWork = 0;
	}

      i = 0;
      if (!(nHours == 0 && (nWork & 0x01)))
	{
	itoa(nHours, &pBuf1[i], 10);
	i = strlen(pBuf1);
	if (nWork & 0x04)
	  pBuf1[i++] = 'h';
	else
	  pBuf1[i++] = ':';
	}

      if (!(nHours == 0 && nMinutes == 0 && (nWork & 0x01)))
	{
	if (nMinutes < 10 && !(nWork & 0x04))
	  pBuf1[i++] = '0';

	itoa(nMinutes, &pBuf1[i], 10);
	i = strlen(pBuf1);
	if (nWork & 0x04)
	  pBuf1[i++] = 'm';
	else
	  pBuf1[i++] = ':';
	}

      if (nSeconds < 10 && !(nWork & 0x04))
	pBuf1[i++] = '0';

      itoa(nSeconds, &pBuf1[i], 10);
      i = strlen(pBuf1);
      if ((nWork & 0x02) == 0)
	{
	pBuf1[i++] = '.';
	pBuf1[i++] = (BYTE)('0' + nTenths);
	}

      if (nWork & 0x04)
	pBuf1[i++] = 's';

      *pLen = (BYTE)i;
      i = 0;
      }
      break;

    case TK_INJECT:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalString(pBuf2, &bLen2))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	nWork = (int)lBuf;
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	nWork = 1;
	}

      if (nWork < 1 || nWork > *pLen)
	{
	i = ERRATH_OUTOFRANGE;
	break;
	}

      memcpy(pBuf1+nWork-1, pBuf2, bLen2);
      break;

    case TK_INSERT:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalString(pBuf2, &bLen2))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	nWork = (int)lBuf;
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	nWork = 1;
	}

      if (nWork < 1 || nWork > *pLen)
	{
	i = ERRATH_OUTOFRANGE;
	break;
	}

      if (*pLen + bLen2 > 255)
	{
	i = ERRINT_LONGSTRING;
	break;
	}

      nWork--;
      for (i = *pLen-1; i >= nWork; i--)
	pBuf1[i + bLen2] = pBuf1[i];
      memcpy(&pBuf1[nWork], pBuf2, bLen2);
      i = 0;
      *pLen += bLen2;
      break;

    case TK_LEFT:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;

      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	nWork = (int)lBuf;
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	nWork = 1;
	}

      if (nWork < *pLen)
	*pLen = (BYTE)nWork;
      break;

    case TK_LOWCASE:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      for (i = 0; i < *pLen; i++)
	pBuf1[i] = tolower(pBuf1[i]);
      i = SkipRPAR();
      break;

    case TK_MATCH:
      strcpy(pBuf1, szMatchedString);
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_MID:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalInteger(&lBuf))
	break;
      nWork = (int)lBuf;
      if (OpCode(wAddr-2) != TK_RPAR && OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	bWork = (BYTE)lBuf;
	}
      else
	{
	bWork = 255;
	}

      if (nWork > *pLen)
	{
	*pLen = 0;
	}
      else
	{
	pBuf1 += (nWork-1);
	*pLen -= (BYTE)(nWork-1);
	if (*pLen > bWork)
	  *pLen = bWork;
	}
      break;

    case TK_MKSTR:
      {
      wAddr += 2;
      if (i = EvalInteger((long *)pBuf1))
	break;
      *pLen = 4;
      }
      break;

    case TK_NAME:
      {
      char *ptr;

      if (fUntitled)
        {
        *pLen = 0;
        }
      else
        {
        strcpy(pBuf1, szDataCurFile);

        if (ptr = strchr(pBuf1, '.'))
          *ptr = NULL;

        *pLen = (BYTE)strlen(pBuf1);
        }
      }
      break;

    case TK_NEXTCHAR:
      if ((i = CommGet()) == -1)
	*pLen = 0;
      else
	{
	*pBuf1 = (BYTE)i;
	*pLen  = 1;
	}
      i = 0;
      break;

    case TK_OCTAL:
      wAddr += 2;
      if (i = EvalInteger(&dwBuf))
	break;
      if (dwBuf < 65536L)
	{
	*pLen = 6;
	}
      else
	{
	*pLen = 11;
	}
      for (i = 1; i <= *pLen; i++)
	{
	bWork = (BYTE)(dwBuf & 0x07);
	pBuf1[*pLen-i] = (BYTE)('0' + bWork);
	dwBuf = dwBuf >> 3;
	}
      i = 0;
      break;

    case TK_PACK:
      {
      WORD wSaveAddr, wLen;

      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;

      pBuf2[0] = ' ';
      bLen2 = 1;
      bWork = 1;

      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	wSaveAddr = wAddr;
	if (i = EvalString(pBuf2, &bLen2))
	  {
	  if (i != ERRINT_MIXTYPES)
	    break;
	  wAddr = wSaveAddr;
	  if (i = EvalInteger(&lBuf))
	    break;
	  }
	else
	  {
	  if (OpCode(wAddr) == TK_COMMA)
	    {
	    wAddr += 2;
	    if (i = EvalInteger(&lBuf))
	      break;
	    bWork = (BYTE)lBuf;
	    }
	  else
	    {
	    if (i = SkipRPAR())
	      break;
	    }
	  }
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	}

      wLen  = *pLen;
      *pLen = 1;
      for (i = 1, ptr = pBuf1; i < wLen; i++)
	{
	if (bWork)
	  {
	  if (IsByteInString(pBuf1[i], pBuf2, bLen2))
	    {
	    if (pBuf1[i] != *ptr)
	      {
	      ptr++;
	      *ptr = pBuf1[i];
	      (*pLen)++;
	      }
	    }
	  else
	    {
	    ptr++;
	    *ptr = pBuf1[i];
	    (*pLen)++;
	    }
	  }
	else
	  {
	  if (IsByteInString(pBuf1[i], pBuf2, bLen2))
	    {
	    if (!IsByteInString(*ptr, pBuf2, bLen2))
	      {
	      ptr++;
	      *ptr = pBuf1[i];
	      (*pLen)++;
	      }
	    }
	  else
	    {
	    ptr++;
	    *ptr = pBuf1[i];
	    (*pLen)++;
	    }
	  }
	}

      i = 0;
      }
      break;

    case TK_PAD:
      {
      WORD wSaveAddr;
      int  nLen;
      char *pPadding;	// 00075 Pad with a string.
      int  sPadding;	// 00075

      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalRange(&nLen, 0, 255, 0))
	break;

      if (nLen < (int)(*pLen))
        {
        *pLen = (BYTE)nLen;
        pBuf1[nLen] = NULL;
        }

      pBuf2[0] = ' ';
      bLen2 = 1;
      bWork = 1;

      pPadding = pBuf2; // 00075 Pad with string.
      sPadding = 0;

      if (OpCode(wAddr-2) != TK_RPAR && OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	wSaveAddr = wAddr;
	if (i = EvalString(pBuf2, &bLen2))
	  {
	  if (i != ERRINT_MIXTYPES)
	    break;
	  wAddr = wSaveAddr;
	  if (i = EvalInteger(&lBuf))
	    break;
	  bWork = (BYTE)lBuf;
	  }
	else
	  {
	  if (OpCode(wAddr) == TK_COMMA)
	    {
	    wAddr += 2;
	    if (i = EvalInteger(&lBuf))
	      break;
	    bWork = (BYTE)lBuf;
	    }
	  else
	    {
	    if (i = SkipRPAR())
	      break;
	    }
	  }
	}

      if (nLen > *pLen)
	{
	if (bWork == 1)
	  {
	  for (i = *pLen; i < nLen; i++)
	    {
	    pBuf1[i] = pBuf2[sPadding++]; // 00075
	    if (sPadding >= bLen2)
	      sPadding = 0;
	    }
	  }
	else if (bWork == 2)
	  {
	  for (i = 0; i < *pLen; i++)
	    pBuf1[nLen-1-i] = pBuf1[*pLen-1-i];
	  for (i = 0; i < nLen - *pLen; i++)
	    {
	    pBuf1[i] = pBuf2[sPadding++]; // 00075
	    if (sPadding >= bLen2)
	      sPadding = 0;
	    }
	  }
	else
	  {
	  register j;
	  j = nLen - (nLen - *pLen) / 2;

	  for (i = *pLen; i < nLen; i++)
	    {
	    pBuf1[i] = pBuf2[sPadding++]; // 00075
	    if (sPadding >= bLen2)
	      sPadding = 0;
	    }
	  for (i = 0; i < *pLen; i++)
	    pBuf1[j-1-i] = pBuf1[*pLen-1-i];
	  for (i = 0; i < j - *pLen; i++)
	    {
	    pBuf1[i] = pBuf2[sPadding++]; // 00075
	    if (sPadding >= bLen2)
	      sPadding = 0;
	    }
	  }

	*pLen = (BYTE)nLen;
	}

      i = 0;
      }
      break;

    case TK_QUOTE:
      wAddr += 2;
      if (i = EvalString(pBuf1+1, pLen))
	break;

      bWork = 0;
      for (i = 1; i <= *pLen; i++)
	{
        if (pBuf1[i] == ',' || pBuf1[i] == '\'')
	  {
	  bWork = 1;
	  break;
	  }
	}

     if (bWork)
       {
       *pBuf1 = '"';
       pBuf1[*pLen+1] = '"';
       *pLen += 2;
       }
     else
       {
       pBuf1++;
       }
      i = SkipRPAR();
      break;

    case TK_RIGHT:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;
	nWork = (int)lBuf;
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	nWork = 1;
	}

      if (nWork < 1 || nWork > 255)
	{
	i = ERRATH_OUTOFRANGE;
	break;
	}

      if (nWork < *pLen)
	{
	pBuf1 += *pLen - (BYTE)nWork;
	*pLen  = (BYTE)nWork;
	}
      break;

    case TK_SLICE:
      {
      WORD wSaveAddr;
      int  nInx, nBeg;

      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalRange(&nInx, 1, 4096, 0))
	break;

      pBuf2[0] = ' ';
      bLen2 = 1;
      nBeg  = 0;

      if (OpCode(wAddr-2) != TK_RPAR && OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	wSaveAddr = wAddr;
	if (i = EvalString(pBuf2, &bLen2))
	  {
	  pBuf2[0] = ' ';
	  bLen2 = 1;
	  if (i != ERRINT_MIXTYPES)
	    break;
	  wAddr = wSaveAddr;
	  if (i = EvalRange(&nBeg, 1, *pLen, 0))
	    break;
	  nBeg--;
	  }
	else
	  {
	  if (OpCode(wAddr) == TK_COMMA)
	    {
	    wAddr += 2;
	    if (i = EvalRange(&nBeg, 1, *pLen, 0))
	      break;
	    nBeg--;
	    }
	  else
	    {
	    if (i = SkipRPAR())
	      break;
	    }
	  }
	}

      while (nInx-- > 1 && nBeg < *pLen)
	{
	while (!IsByteInString(pBuf1[nBeg], pBuf2, bLen2) && nBeg < *pLen)
	  nBeg++;
	nBeg++;
	}

      if (nBeg < *pLen)
	{
	pBuf1 += nBeg;
	*pLen -= (BYTE)nBeg;
	nBeg = 0;

	while (!IsByteInString(pBuf1[nBeg], pBuf2, bLen2) && nBeg < *pLen)
	  nBeg++;

	*pLen = (BYTE)nBeg;
	}
      else
	{
	*pLen = 0;
	}

      i = 0;
      }
      break;

    case TK_STR:
      wAddr += 2;
      if (i = EvalInteger(&lBuf))
	break;
      ltoa(lBuf, pBuf1, 10);
      *pLen = (BYTE)strlen(pBuf1);
      break;

    case TK_STRIP:
      {
      WORD wSaveAddr;
      int  nClass, nWhere;

      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      nClass = -1;
      nWhere = 1;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	wSaveAddr = wAddr;
	if (i = EvalString(pBuf2, &bLen2))
	  {
	  if (i != ERRINT_MIXTYPES)
	    break;
	  wAddr = wSaveAddr;
	  if (i = EvalRange(&nClass, 0, 255, 0))
	    break;
	  }
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalRange(&nWhere, 0, 3, 0))
	    break;
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
	  }
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	}

      if (nWhere == 0)
	{
	register j;
	for (i = j = 0; i < *pLen; i++)
	  {
	  if (!IsByteInStringClass(pBuf1[i], pBuf2, bLen2, nClass))
	    pBuf1[j++] = pBuf1[i];
	  }
	*pLen = (BYTE)j;
	}
      if (nWhere == 1 || nWhere == 3)
	{
	for (i = *pLen; i > 0; i--)
	  {
	  if (!IsByteInStringClass(pBuf1[i-1], pBuf2, bLen2, nClass))
	    break;
	  }
	*pLen = (BYTE)i;
	}
      if (nWhere == 2 || nWhere == 3)
	{
	for (i = 0; i < *pLen; i++, pBuf1++)
	  {
	  if (!IsByteInStringClass(*pBuf1, pBuf2, bLen2, nClass))
	    break;
	  }
	*pLen -= (BYTE)i;
	}

      i = 0;
      }
      break;

    case TK_SUBST:
      {
      BYTE *pOld, bOld, *pNew, bNew;

      pOld = pBuf1 + 256;
      pNew = pBuf1 + 512;
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalString(pOld, &bOld))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalString(pNew, &bNew))
	break;

      for (i = 0; i < *pLen; i++)
	{
	register j;

	for (j = 0; j < bOld; j++)
	  {
	  if (pBuf1[i] == pOld[j])
	    {
	    if (j < bNew)
	      pBuf1[i] = pNew[j];
	    break;
	    }
	  }
	}

      i = SkipRPAR();
      }
      break;

    case TK_TIME:
      {
      if (OpCode(wAddr) == TK_LPAR)
	{
	char fmt[16];
	int  nHours, nMinutes, nSeconds;

	wAddr += 2;
	if (i = EvalInteger(&lBuf))
	  break;

	nHours	 = (int)(lBuf / 3600L);
	nMinutes = (int)((lBuf / 60L) % 60L);
	nSeconds = (int)(lBuf % 60L);

	fmt[ 0] = '%';
	fmt[ 1] = '0';
	fmt[ 2] = '2';
	fmt[ 3] = 'd';
	fmt[ 4] = ':';
	fmt[ 5] = '%';
	fmt[ 6] = '0';
	fmt[ 7] = '2';
	fmt[ 8] = 'd';
	fmt[ 9] = ':';
	fmt[10] = '%';
	fmt[11] = '0';
	fmt[12] = '2';
	fmt[13] = 'd';
	fmt[14] = NULL;
	sprintf(pBuf1, fmt, nHours, nMinutes, nSeconds);
	i = 0;
	}
      else
	{
	_strtime(pBuf1);
	}
      *pLen = 8;
      }
      break;

    case TK_UPCASE:
      wAddr += 2;
      if (i = EvalString(pBuf1, pLen))
	break;
      for (i = 0; i < *pLen; i++)
	pBuf1[i] = toupper(pBuf1[i]);
      i = SkipRPAR();
      break;

    case TK_VERSION:
		// dca00083 PJL define Crosstalk for Windows Version Number
		sprintf(pBuf1, "%d.%d", WINDX_VERSION_MAJOR, WINDX_VERSION_MINOR);
      *pLen = strlen(pBuf1);
      break;

    case TK_WINSTRING:
      {
      int nRow, nCol, nLen, *pInt;
      register j;

      wAddr += 2;
      if (i = EvalInteger(&lBuf))
	break;
      nRow = (int)lBuf;
      if (i = SkipComma())
	break;
      if (i = EvalInteger(&lBuf))
	break;
      nCol = (int)lBuf;
      if (i = SkipComma())
	break;
      if (i = EvalInteger(&lBuf))
	break;
      nLen = (int)lBuf;

      if (nRow < 1 || nRow > 24 || nCol < 1 || nCol > 132 || nLen < 1)
	{
	i = ERRATH_OUTOFRANGE;
	break;
	}

      nRow--;

      if (nCol + nLen - 1 > 132)
	nLen = 133 - nCol;

      pInt = VssGetLine(nRow);
      for (i = 0, j = nCol; j < nCol+nLen; j++)
	pBuf1[i++] = (BYTE)(pInt[j] & 0x007F);

      i = 0;
      *pLen = (BYTE)nLen;
      }
      break;

    case TK_WINVERSION:
      {
      char fmt[32];

      fmt[0] = '%';
      fmt[1] = 'd';
      fmt[2] = '.';
      fmt[3] = '%';
      fmt[4] = '0';
      fmt[5] = '2';
      fmt[6] = 'd';
      fmt[7] = NULL;

      i = GetVersion();
      sprintf(pBuf1, fmt, LOBYTE(i), HIBYTE(i));
      *pLen = (BYTE)strlen(pBuf1);
      i = 0;
      }
      break;

    default:
      i = ERRINT_INTERNAL;
      break;
    }

 if (i == 0)
   {
   if ((int)*pLen + (int)bLen > 255)
     i = ERRINT_LONGSTRING;
   else
     {
     memcpy(pBuf, pBuf1, *pLen);
     }
   }

  nStackAddr -= 768;
  return (i);
}

static int near SkipComma()
{
  if (OpCode(wAddr) != TK_COMMA)
    return (ERRINT_MISPARAM);

  wAddr += 2;
  return (0);
}

static int near SkipRPAR()
{
  if (OpCode(wAddr) != TK_RPAR)
    return (ERRINT_BADEXPRESSION);

  wAddr += 2;
  return (0);
}

static int near IsByteInString(byte, str, len)
  BYTE byte;
  BYTE *str;
  BYTE len;
{
  register i;

  for (i = 0; i < len; i++, str++)
    if (byte == *str)
      return (TRUE);

  return (FALSE);
}

static int near IsByteInStringClass(byte, str, len, class)
  BYTE byte;
  BYTE *str;
  BYTE len;
  int  class;
{
  int  bMask;
  register i;


  if (class == -1)
    {
    for (i = 0; i < len; i++, str++)
      if (byte == *str)
	return (TRUE);
    }
  else
    {
    bMask = 0;
    if (byte == ' ' || byte == '\t' || byte == '\r' || byte == '\n' ||
	byte == 12  || byte == '\b' || byte == 0)
      bMask |= 0x01;
    if (isupper(byte))
      bMask |= 0x02;
    if (islower(byte))
      bMask |= 0x04;
    if (byte == '$' || byte == '%' || byte == '_')
      bMask |= 0x08;
    if (isdigit(byte))
      bMask |= 0x10;
    if (isxdigit(byte))
      bMask |= 0x20;
    if (byte == ' ' || byte == ','  || byte == '.' || byte == '(' ||
	byte == '/' || byte == '\\' || byte == ':' || byte == ';' ||
	byte == '<' || byte == '='  || byte == '>' || byte == '!' ||
	byte == ')')
      bMask |= 0x40;
    if (ispunct(byte))
      bMask |= 0x80;

    if (bMask & class)
      return (TRUE);
    }


  return (FALSE);
}
