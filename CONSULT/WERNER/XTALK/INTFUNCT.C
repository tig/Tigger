/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Integer Functions (INTFUNCT.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains the various integer functions used in the  *
*             script language.                                                *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/19/90 00082 Fix LOC problem.                            *
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
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "xtalk.h"
#include "library.h"
#include "capture.h"
#include "compile.h"
#include "interp.h"
#include "variable.h"
#include "comm.h"
#include "term.h"
#include "properf.h"
#include "xfer.h"


#define OpCode(x)	(*((WORD far *)(&lpObjectCode[x])))
  extern WORD wAddr;
  extern struct find_t filefind;

//extern long cdecl atol(char *);
//extern long cdecl tell(int);
  static int near SkipComma();
  static int near SkipRPAR();



/*---------------------------------------------------------------------------*/
/*  Evaluate Integer Function						     */
/*									     */
/*---------------------------------------------------------------------------*/
int EvalIntFunct(lBuf)
  long *lBuf;
{
  BYTE *pBuf, bLen, *pBuf2, bLen2;
  register i;


  if (nStackAddr + 768 > nStackHigh)
    return (ERRINT_TOOCOMPLEX);
  pBuf = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;
  pBuf2 = &filexfer.stack.data[nStackAddr];
  nStackAddr += 256;

  *lBuf = 0L;

  i = 0;
  wAddr += 2;
  switch (OpCode(wAddr-2))
    {
    case TK_ABS:
      wAddr += 2;
      if (i = EvalInteger(lBuf))
	break;

      if (*lBuf < 0)
	*lBuf = - *lBuf;
      break;

    case TK_ACTIVE:
      if (GetFocus() == hChildTerminal)
        *lBuf = 1L;
      break;

    case TK_ASC:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      *lBuf = *pBuf;
      break;

    case TK_CAPCHARS:
      *lBuf = CaptureChars();
      break;

    case TK_CKSUM:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      i = 0;
      while (bLen-- > 0)
	i += *(pBuf++);

      *lBuf = (WORD)i;
      i = 0;
      break;

    case TK_CLASS:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      if (*pBuf == ' ' || *pBuf == '\t' || *pBuf == '\r' || *pBuf == '\n' ||
	  *pBuf == 12  || *pBuf == '\b' || *pBuf == 0)
	*lBuf |= 0x01;
      if (isupper(*pBuf))
	*lBuf |= 0x02;
      if (islower(*pBuf))
	*lBuf |= 0x04;
      if (*pBuf == '$' || *pBuf == '%' || *pBuf == '_')
	*lBuf |= 0x08;
      if (isdigit(*pBuf))
	*lBuf |= 0x10;
      else if (isxdigit(*pBuf))
	*lBuf |= 0x20;
      if (*pBuf == ' ' || *pBuf == ','	|| *pBuf == '.' || *pBuf == '(' ||
	  *pBuf == '/' || *pBuf == '\\' || *pBuf == ':' || *pBuf == ';' ||
	  *pBuf == '<' || *pBuf == '='	|| *pBuf == '>' || *pBuf == '!' ||
	  *pBuf == ')')
	*lBuf |= 0x40;
      if (ispunct(*pBuf))
	*lBuf |= 0x80;
      break;

    case TK_COLORSCREEN:
      if (!fMonoDisplay)
        *lBuf = 1L;
      break;

    case TK_CONNECTARQ:
      if (fConnectARQ)
        *lBuf = 1;
      break;

    case TK_CONNECTED:
      if (bConnected)
        *lBuf = 1;
      break;

    case TK_CONNECTSPEED:
      *lBuf = nConnectSpeed;
      break;

    case TK_COUNT:
      {
      BYTE *pBuf2, bLen2;

      wAddr += 2;
      pBuf2  = pBuf + 256;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalString(pBuf2, &bLen2))
	break;
      if (i = SkipRPAR())
	break;

      for (i = 0; i < bLen; i++)
	{
	register j;

	for (j = 0; j < bLen2; j++)
	  {
	  if (pBuf[i] == pBuf2[j])
	    {
	    (*lBuf)++;
	    break;
	    }
	  }
	}
      i = 0;
      }
      break;

    case TK_CRC:
      {
      register int crc;

      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(lBuf))
	  break;
        crc = (int)(*lBuf);
	}
      else
	{
	if (i = SkipRPAR())
	  break;
        crc = 0;
	}

      while (bLen-- > 0)
	{
	crc = crc ^ (int)(*(pBuf++)) << 8;
	for (i = 0; i < 8; ++i)
	  {
	  if (crc & 0x8000)
	    crc = crc << 1 ^ 0x1021;
	  else
	    crc = crc << 1;
	  }
	}

      *lBuf = (long)((WORD)crc);
      i = 0;
      }
      break;

    case TK_CURDAY:
      {
      struct tm *newtime;
      time_t long_time;

      time(&long_time);
      newtime = localtime(&long_time);

      *lBuf = (long)newtime->tm_mday;
      }
      break;

    case TK_CURHOUR:
      {
      struct tm *newtime;
      time_t long_time;

      time(&long_time);
      newtime = localtime(&long_time);

      *lBuf = (long)newtime->tm_hour;
      }
      break;

    case TK_CURMINUTE:
      {
      struct tm *newtime;
      time_t long_time;

      time(&long_time);
      newtime = localtime(&long_time);

      *lBuf = (long)newtime->tm_min;
      }
      break;

    case TK_CURMONTH:
      {
      struct tm *newtime;
      time_t long_time;

      time(&long_time);
      newtime = localtime(&long_time);

      *lBuf = (long)(newtime->tm_mon + 1);
      }
      break;

    case TK_CURSECOND:
      {
      struct tm *newtime;
      time_t long_time;

      time(&long_time);
      newtime = localtime(&long_time);

      *lBuf = (long)newtime->tm_sec;
      }
      break;

    case TK_CURYEAR:
      {
      struct tm *newtime;
      time_t long_time;

      time(&long_time);
      newtime = localtime(&long_time);

      *lBuf = (long)(newtime->tm_year + 1900);
      }
      break;

    case TK_DDESTATUS:
      wAddr += 2;
      if (i = EvalInteger(lBuf))
	break;

      if (StatusDDE((int)(*lBuf)))
        *lBuf = 1L;
      else
        *lBuf = 0L;
      break;

    case TK_EOF:
      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalInteger(lBuf))
	  break;
        if (*lBuf < 1 || *lBuf > 8)
	  {
	  i = ERRIOE_MISCHANNEL;
	  break;
	  }
	i = (int)(*lBuf);
	}
      else
	i = (int)cDefInputFile;

      if (!(fFileFlags[i-1] & FILE_OPEN))
	{
	i = ERRIOE_CHANNELNOTOPEN;
	break;
	}

      if (fFileFlags[i-1] & FILE_EOF)
        *lBuf = 1L;
      else
	*lBuf = 0L;

      i = 0;
      break;

    case TK_EOL:
      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalInteger(lBuf))
	  break;
        if (*lBuf < 1 || *lBuf > 8)
	  {
	  i = ERRIOE_MISCHANNEL;
	  break;
	  }
	i = (int)(*lBuf);
	}
      else
	i = (int)cDefInputFile;

      if (!(fFileFlags[i-1] & FILE_OPEN))
	{
	i = ERRIOE_CHANNELNOTOPEN;
	break;
	}

      if (fFileFlags[i-1] & FILE_EOL)
        *lBuf = 1L;
      else
	*lBuf = 0L;

      i = 0;
      break;

    case TK_ERRCLASS:
      *lBuf = (long)(nErrorSave / 256);
      break;

    case TK_ERRNO:
      *lBuf = (long)(nErrorSave & 0x00FF);
      break;

    case TK_ERROR:
      if (nErrorFlag)
        *lBuf = 1L;
      nErrorFlag = 0;
      break;

    case TK_EXISTS:
      {
      struct find_t buffer;
      int  nAttr;

      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalInteger(lBuf))
	  break;
	nAttr = (int)(*lBuf);
	}
      else
	{
	if (i = SkipRPAR())
	  break;
        nAttr = 0x10;
	}
      if (_dos_findfirst(pBuf, nAttr, &buffer))
	*lBuf = 0L;
      else
        *lBuf = 1L;
      }
      break;

    case TK_FALSE:
      break;

    case TK_FILEATTR:
      {
      int nAttr;

      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalString(pBuf, &bLen))
	  break;
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalInteger(lBuf))
	    break;
	  nAttr = (int)(*lBuf);
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
          nAttr = -1;
	  }
	if (i = _dos_findfirst(pBuf, nAttr, &filefind))
	  {
	  *lBuf = 0L;
	  i = 0;
	  break;
	  }
	}

      *lBuf = (long)((BYTE)filefind.attrib);
      i = 0;
      }
      break;

    case TK_FILEDATE:
      {
      int nAttr, nYear, nMonth, nDay, nLeap;

      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalString(pBuf, &bLen))
	  break;
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalInteger(lBuf))
	    break;
	  nAttr = (int)(*lBuf);
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
	  nAttr = 0;
	  }
	if (i = _dos_findfirst(pBuf, nAttr, &filefind))
	  {
	  *lBuf = 0L;
	  i = 0;
	  break;
	  }
	}

      nYear  = 80 + (filefind.wr_date >> 9);
      nMonth = (filefind.wr_date >> 5) & 0x000F;
      nDay   = filefind.wr_date & 0x001F;
      nLeap  = nYear % 4 == 0;

      *lBuf = (long)nYear * 365L + (long)(nYear/4);
      if (nMonth > 1)
	nDay += 31;
      if (nMonth > 2)
	{
	nDay += 28;
	if (nLeap)
	  nDay++;
	}
      if (nMonth > 3)
	nDay += 31;
      if (nMonth > 4)
	nDay += 30;
      if (nMonth > 5)
	nDay += 31;
      if (nMonth > 6)
	nDay += 30;
      if (nMonth > 7)
	nDay += 31;
      if (nMonth > 8)
	nDay += 31;
      if (nMonth > 9)
	nDay += 30;
      if (nMonth > 10)
	nDay += 31;
      if (nMonth > 11)
	nDay += 30;
      *lBuf += (long)nDay;
      i = 0;
      }
      break;

    case TK_FILESIZE:
      {
      int nAttr;

      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalString(pBuf, &bLen))
	  break;
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalInteger(lBuf))
	    break;
	  nAttr = (int)(*lBuf);
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
	  nAttr = 0;
	  }
	if (i = _dos_findfirst(pBuf, nAttr, &filefind))
	  {
	  *lBuf = 0L;
	  i = 0;
	  break;
	  }
	}

      *lBuf = filefind.size;
      i = 0;
      }
      break;

    case TK_FILETIME:
      {
      int nAttr, nHour, nMinute, nSecond;

      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalString(pBuf, &bLen))
	  break;
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalInteger(lBuf))
	    break;
	  nAttr = (int)(*lBuf);
	  }
	else
	  {
	  if (i = SkipRPAR())
	    break;
	  nAttr = 0;
	  }
	if (i = _dos_findfirst(pBuf, nAttr, &filefind))
	  {
	  *lBuf = 0L;
	  i = 0;
	  break;
	  }
	}

      nHour   = filefind.wr_time >> 11;
      nMinute = (filefind.wr_time >> 5) & 0x003F;
      nSecond = filefind.wr_time & 0x001F;

      *lBuf = (long)nSecond + (long)nMinute * 60L + (long)nHour * 3600L;
      i = 0;
      }
      break;

    case TK_FNCHECK:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      if (strchr(pBuf, ':'))
	*lBuf |= 1;
      if (strchr(pBuf, '\\'))
	*lBuf |= 2;
      if (strchr(pBuf, '.'))
	*lBuf |= 4;
      if (strchr(pBuf, '?'))
	*lBuf |= 8;
      if (strchr(pBuf, '*'))
	*lBuf |= 16;
      if (*pBuf == '@')
	*lBuf |= 32;
      break;

    case TK_FREEFILE:
      for (i = 0; i < 8; i++)
	{
        if (!(fFileFlags[i] & FILE_OPEN))
	  {
	  *lBuf = i + 1;
	  i = 0;
	  break;
	  }
	}
      i = 0;
      break;

    case TK_FREEMEM:
      *lBuf = (long)GlobalCompact(0L);
      break;

    case TK_INBOOK:
      {
      OFSTRUCT OfStruct;

      if (OpCode(wAddr) == TK_LPAR)
	{
        wAddr += 2;

        bLen = 255;
        if (i = EvalString(pBuf2, &bLen))
          break;

        CreatePath(pBuf, VAR_DIRXWP, pBuf2);

	if (i = SkipRPAR())
	  break;
	}
      else
	{
        CreatePath(pBuf, VAR_DIRXWP, "*");
	}

      if (i = _dos_findfirst(pBuf, 0, (struct find_t*)pBuf+128))
	return (0);

      *lBuf += 1L;
      while (_dos_findnext((struct find_t *)pBuf+128) == 0)
	*lBuf += 1L;

      i = 0;
      }
      break;

    case TK_INKEY:
      if (bKeyIn != bKeyOut)
	{
	*lBuf = (long)nKeyBuffer[bKeyOut++];
	if (bKeyOut >= sizeof(nKeyBuffer)/sizeof(int))
	  bKeyOut = 0;
	}
      break;

    case TK_INSCRIPT:
      {
      WORD wSaveAddr, wSaveLine;

      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;

      wSaveAddr = wAddr;
      wSaveLine = wLine;

      strupr(pBuf);
      if (!GotoLabel(pBuf))
        *lBuf = 1L;

      wAddr = wSaveAddr;
      wLine = wSaveLine;
      }
      break;

    case TK_INSTR:
      {
      int nStart;

      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipComma())
	break;
      if (i = EvalString(pBuf2, &bLen2))
	break;
      if (OpCode(wAddr) == TK_COMMA)
	{
	wAddr += 2;
	if (i = EvalRange(&nStart, 1, 255, 0))
	  break;
	}
      else
	{
	if (i = SkipRPAR())
	  break;
	nStart = 1;
	}

      for (i = nStart-1; i < bLen; i++)
	{
	if (memcmp(&pBuf[i], pBuf2, bLen2) == 0)
	  {
	  *lBuf = (long)(i+1);
	  break;
	  }
	}
      i = 0;
      }
      break;

    case TK_LENGTH:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;

      if (OpCode(wAddr) != TK_RPAR)
	i = ERRINT_BADEXPRESSION;
      else
	{
	wAddr += 2;
	*lBuf = (long)bLen;
	}
      break;

    case TK_LOC:
      {
      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalInteger(lBuf))
	  break;
        if (*lBuf < 1 || *lBuf > 8)
	  {
	  i = ERRIOE_MISCHANNEL;
	  break;
	  }
	i = (int)(*lBuf);
	}
      else
	i = (int)cDefInputFile;

      if (!(fFileFlags[i-1] & FILE_OPEN))
	{
	i = ERRIOE_CHANNELNOTOPEN;
	break;
	}

      // 00092 Fix LOC problem.
      DosChgFilePtr(hFileHandles[i-1], 0L, FILE_CURRENT, lBuf);
      i = 0;
      }
      break;

    case TK_MKINT:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      *lBuf = *(long *)pBuf;
      break;

    case TK_NULL:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      if (bLen == 0)
        *lBuf = 1L;
      break;

    case TK_OFF:
      break;

    case TK_ON:
      *lBuf = 1L;
      break;

    case TK_ONLINE:
      if (bConnected && (fCommStsRLSD || Comm.cLocal))
        *lBuf = 1L;
      break;

    case TK_SECNO:
      {
      int nHour, nMinute, nSecond;

      if (OpCode(wAddr) == TK_LPAR)
	{
	if (i = EvalRange(&nHour, 0, 23, FW_COMMA))
	  break;
	if (i = EvalRange(&nMinute, 0, 59, FW_COMMA))
	  break;
	if (i = EvalRange(&nSecond, 0, 59, 0))
	  break;
	}
      else
	{
	struct dostime_t time;
	_dos_gettime(&time);
	nHour	= time.hour;
	nMinute = time.minute;
	nSecond = time.second;
	}
      *lBuf = (long)nSecond + (long)nMinute * 60L + (long)nHour * 3600L;
      }
      break;

    case TK_TIMEOUT:
      if (fTimeout)
        *lBuf = 1L;
      break;

    case TK_TRUE:
      *lBuf = 1L;
      break;

    case TK_INTVAL:
      wAddr += 2;
      if (i = EvalString(pBuf, &bLen))
	break;
      if (i = SkipRPAR())
	break;
      LibStrToInt(pBuf, lBuf);
      break;

    case TK_WEEKDAY:
      if (OpCode(wAddr) == TK_LPAR)
	{
	wAddr += 2;
	if (i = EvalInteger(lBuf))
	  break;

	*lBuf %= 7;
	}
      else
	{
	struct dosdate_t date;

	_dos_getdate(&date);
	*lBuf = date.dayofweek;
	}
      break;

    case TK_WINCHAR:
      {
      int nRow, nCol, *pInt;
      register j;

      wAddr += 2;
      if (i = EvalRange(&nRow, 1, 24, FW_COMMA))
	break;
      if (i = EvalRange(&nCol, 1, 132, 0))
	break;

      pInt = VssGetLine(nRow-1);
      *lBuf = (long)(pInt[nCol] & 0x00FF);
      }
      break;

    case TK_WINSIZEX:
      *lBuf = (long)nTermExtX;
      break;

    case TK_WINSIZEY:
      *lBuf = (long)nTermExtY;
      break;

    case TK_XPOS:
      *lBuf = (long)nTermChrX;
      break;

    case TK_YPOS:
      *lBuf = (long)nTermChrY + 1;
      break;

    default:
      i = ERRINT_INTERNAL;
      break;
    }

  nStackAddr -= 512;
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
