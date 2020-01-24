/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Dos <--> OS/2 Conversion Module (DOSOS2.C)					   *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 10/20/89 Initial Version.				      *
*             1.01 01/17/89 D0082 Changed DosMove's INTDOS to INTDOSX.        *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>	//dca00046 JDB Added for external decls
#undef min
#undef max
#include <stdio.h>
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
    if (_dos_close(hf))
      return (errno);
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
  // intdos(&inregs, &outregs);
  // D0082 PJL - Need to use SEGREGS here, so call INTDOSX, not INTDOS:
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
    if (_dos_creatnew(pszFileName, _A_NORMAL, (int *)phf) == 0)
      _dos_close(*phf);

  if (_dos_open(pszFileName, usMode, (int *)phf))
    return (errno);

  return (0);
}


USHORT DosRead(HFILE hf, PVOID pvBuf, USHORT cbBuf, PUSHORT pcbBytesRead)
{

  if (_dos_read(hf, pvBuf, cbBuf, pcbBytesRead))
  {
//  char buf[80];
//  sprintf(buf, "DosRead error <%d>", errno);
//  LineToTerminal(buf);

    return (errno);
  }

  return (0);
}


USHORT DosWrite(HFILE hf, PVOID pvBuf, USHORT cbBuf, PUSHORT pcbBytesWritten)
{

  if (_dos_write(hf, pvBuf, cbBuf, pcbBytesWritten))
  {
//  char buf[80];
//  sprintf(buf, "DosWrite error <%d>", errno);
//  LineToTerminal(buf);
    return (errno);
  }

  return (0);
}


USHORT DosCopy(PSZ src_file, PSZ dest_file)
{
	int infile, outfile, inbytes, outbytes, in_err, out_err;
	unsigned fdate, ftime;
	char filebuf[BUFSIZ];

	// open source file
	if (_dos_open(src_file, O_RDONLY, &infile) != 0)
		return(errno);

	// open destination file
	if (_dos_creat(dest_file, _A_NORMAL, &outfile) != 0)
		return(errno);

	// copy the file
	for (in_err = out_err = 0; in_err == 0 && out_err == 0;)
	{
		in_err = DosRead(infile, (void far *)filebuf, BUFSIZ, &inbytes);
		if (inbytes == 0) break;
		out_err = DosWrite(outfile, (void far *)filebuf, inbytes, &outbytes);
		if (outbytes < inbytes) break;
	}

	// preserve file date & time
	_dos_getftime(infile, &fdate, &ftime);
	_dos_setftime(outfile, fdate, ftime);

	// close the files
	_dos_close(infile);
	_dos_close(outfile);

	// return any error code
	if (in_err) return(in_err);
	if (out_err)
	{
		unlink(dest_file);
		return(out_err);
	}
	return(0);
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


USHORT DosQCurDir(USHORT usDrive, PBYTE pPath, USHORT cPath)
{
  union REGS regs;

  *pPath = '\\';

  regs.h.ah = 0x47;
  regs.x.si = FP_OFF(pPath) + 1;
  regs.h.dl = (BYTE)usDrive;
  intdos(&regs, &regs);

  if (regs.x.cflag)
    return (regs.x.ax);

  return (0);
}
