/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows       *
*                                                                             *
*   Module:   Dos <--> OS/2 Conversion Module (DOSOS2.C)                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#undef min
#undef max
#include <stdlib.h>
#include <dos.h>
#include <fcntl.h>
#include <errno.h>
#include <io.h>
#include "dosos2.h"


USHORT DosChgFilePtr(HFILE hf, LONG lDist, USHORT usMethod, PULONG pulNewPtr)
  {
  union REGS inregs, outregs;


  inregs.h.ah = 0x42;
  inregs.x.cx = (WORD)(lDist >> 16);
  inregs.x.dx = (WORD)lDist;
  inregs.h.al = usMethod;
  inregs.x.bx = hf;
  intdos(&inregs, &outregs);

  if (outregs.x.cflag)
    return (outregs.x.ax);

  *pulNewPtr = (((ULONG)outregs.x.dx) << 16) + outregs.x.ax;

  return (0);
  }


USHORT DosQFileInfo(HFILE hf, USHORT usInfoLevel, PFILESTATUS pfstsInfo,
		    USHORT cbInfoBuf)
  {
  unsigned date, time;


  if (_dos_getftime(hf, &date, &time))
    return (errno);

  memcpy((PBYTE)&pfstsInfo->fdateCreation,   (PBYTE)&date, 2);
  memcpy((PBYTE)&pfstsInfo->ftimeCreation,   (PBYTE)&time, 2);
  memcpy((PBYTE)&pfstsInfo->fdateLastAccess, (PBYTE)&date, 2);
  memcpy((PBYTE)&pfstsInfo->ftimeLastAccess, (PBYTE)&time, 2);
  memcpy((PBYTE)&pfstsInfo->fdateLastWrite,  (PBYTE)&date, 2);
  memcpy((PBYTE)&pfstsInfo->ftimeLastWrite,  (PBYTE)&time, 2);
  pfstsInfo->cbFile	 = filelength(hf);
  pfstsInfo->cbFileAlloc = pfstsInfo->cbFile;
  pfstsInfo->attrFile	 = _A_NORMAL;

  return (0);
  }


USHORT DosClose(HFILE hf)
  {
  if (hf != -1)
    {
    if (_dos_close(hf))
      return (errno);
    }

  return (0);
  }


USHORT DosDelete(PSZ pszPath, ULONG ulReserve)
  {
  union REGS inregs, outregs;


  inregs.h.ah = 0x41;
  inregs.x.dx = (int)pszPath;
  intdos(&inregs, &outregs);

  if (outregs.x.cflag)
    return (outregs.x.ax);

  return (0);
  }


USHORT DosMove(PSZ pszOldName, PSZ pszNewName , ULONG ulReserved)
  {
  union REGS inregs, outregs;
  struct SREGS segregs;


  segread(&segregs);
  inregs.h.ah = 0x56;
  inregs.x.dx = FP_OFF(pszOldName);
  inregs.x.di = FP_OFF(pszNewName);
  segregs.es  = segregs.ds;
  intdosx(&inregs, &outregs, &segregs);

  if (outregs.x.cflag)
    return (outregs.x.ax);

  return (0);
  }


USHORT DosOpen(PSZ pszFileName, PHFILE phf, PUSHORT pusAction,
	       ULONG ulFileSize, USHORT usAttribute, USHORT fsOpenFlags,
	       USHORT fsOpenMode, ULONG ulReserved)
  {
  USHORT usMode;


  if ((fsOpenMode & 0x0F) == OPEN_ACCESS_READONLY)
    usMode = O_RDONLY;
  else if ((fsOpenMode & 0x0F) == OPEN_ACCESS_WRITEONLY)
    usMode = O_WRONLY;
  else
    usMode = O_RDWR;

  // usMode |= SH_COMPAT;

  *pusAction = 0;

  if (fsOpenFlags & FILE_CREATE)
    {
    if (_dos_creatnew(pszFileName, _A_NORMAL, (int *)phf) == 0)
      _dos_close(*phf);
    }

  if (_dos_open(pszFileName, usMode, (int *)phf))
    return (errno);

  return (0);
  }


USHORT DosRead(HFILE hf, PVOID pvBuf, USHORT cbBuf, PUSHORT pcbBytesRead)
  {

  if (_dos_read(hf, pvBuf, cbBuf, pcbBytesRead))
    return (errno);

  return (0);
  }


USHORT DosWrite(HFILE hf, PVOID pvBuf, USHORT cbBuf, PUSHORT pcbBytesWritten)
  {

  if (_dos_write(hf, pvBuf, cbBuf, pcbBytesWritten))
    return (errno);

  return (0);
  }


USHORT DosChDir(PSZ pszDirPath, ULONG ulReserved)
  {
  union REGS inregs, outregs;


  inregs.h.ah = 0x3B;
  inregs.x.dx = FP_OFF(pszDirPath);
  intdos(&inregs, &outregs);

  if (outregs.x.cflag)
    return (outregs.x.ax);

  return (0);
  }
