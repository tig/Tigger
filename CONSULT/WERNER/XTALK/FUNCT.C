/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   File Functions (FUNCT.C)                                        *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   File I/O support for the interpreter, since the standard C      *
*             waste's valuable memory I wrote my own.                         *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 08/09/89 00011 A read should read "1,2,3" as one item and  *
*                                   not parse of the comma.                   *
*             1.01 09/07/89 00028 Read still does not properly handled quoted *
*                                   strings.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "interp.h"
#include "comm.h"
#include "term.h"
#include "properf.h"
#include "variable.h"


#define MAX_FILES  8

int  hFileHandles[MAX_FILES];
int  nFileModes[MAX_FILES];
char fFileFlags[MAX_FILES];


int FileOpen(nMode, szFile, nChan)
  int  nMode;
  char *szFile;
  int  nChan;
{
  ULONG ulPos;
  OFSTRUCT OfStruct;
  register i;


  nErrorCode = 0;

  if (fFileFlags[nChan] & FILE_OPEN)
    return (ERRIOE_CHANNELOPEN);

  OfStruct.cBytes = 0;
  if (nMode == FILE_OUTPUT)
    {
    if ((i = OpenFile(szFile, &OfStruct, OF_WRITE|OF_CREATE)) == -1)
      nErrorCode = ERRSYS_OPENFAULT;
    }
  else if (nMode == FILE_APPEND || nMode == FILE_RANDOM)
    {
    if ((i = OpenFile(szFile, &OfStruct, OF_READWRITE)) == -1)
      {
      if ((i = OpenFile(szFile, &OfStruct, OF_WRITE|OF_CREATE)) == -1)
        nErrorCode = ERRSYS_OPENFAULT;
      }
    else if (nMode == FILE_APPEND)
      DosChgFilePtr(i, 0L, FILE_END, &ulPos);
    }
  else
    {
    if ((i = OpenFile(szFile, &OfStruct, OF_READ)) == -1)
      nErrorCode = ERRIOE_NOCANFINDFILE;
    }

  if (nErrorCode == 0)
    {
    hFileHandles[nChan] = i;
    fFileFlags[nChan]	= FILE_OPEN;
    nFileModes[nChan]	= nMode;
    }

  return (0);
}

int FileRead(nChan, wVarNo)
  int  nChan;
  WORD wVarNo;
{
  FILESTATUS fsts;
  char buf[256];
  int  nLen, nOff;  // 00028  Skip over blanks
  char chTermScan;  // 00011  Define end of item
  ULONG ulPos;
  long fpos;
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_INPUT)
    {
    if (nFileModes[nChan] != FILE_RANDOM)
      return (ERRIOE_NOCANREAD);
    else
      return (ERRIOE_NODORANDOM);
    }

  DosChgFilePtr(hFileHandles[nChan], 0L, FILE_CURRENT, &fpos);
  if (DosRead(hFileHandles[nChan], buf, sizeof(buf), &nLen))
    {
    nErrorCode = ERRSYS_READFAULT;
    return (0);
    }

  fFileFlags[nChan] = FILE_OPEN;

  if (nLen == 0)
    fFileFlags[nChan] |= FILE_EOF | FILE_EOL;

  // 00028 Skip over blanks
  for (nOff = 0; nOff < nLen; nOff++)
    {
    if (buf[nOff] != ' ')
      break;
    }

  // 00011  Set the scan termination character based the first character.
  if (nOff < nLen && buf[nOff] == '"')
    chTermScan = '"';
  else
    chTermScan = ',';

  for (i = nOff; i < nLen && i < 255; i++)
    {
    if (i == nOff && buf[i] == '"')
      {
      }
    else if (buf[i] == ASC_CR)
      {
      DosChgFilePtr(hFileHandles[nChan], fpos+i+2, FILE_BEGIN, &ulPos);
      fFileFlags[nChan] |= FILE_EOL;
      break;
      }
    else if (buf[i] == chTermScan)   // 00011  Term read item on , or "
      {
      register j;

      // 00028 Skip over following comma for quoted strings.
      j = i;
      if (chTermScan == '"')
        {
        while (j < nLen && buf[j] != ',' && buf[j] != ASC_CR)
          j++;
        }
      if (buf[j] == ASC_CR)
        j++;

      DosChgFilePtr(hFileHandles[nChan], fpos+j+1, FILE_BEGIN, &ulPos);
      break;
      }
    }

  DosChgFilePtr(hFileHandles[nChan], 0L, FILE_CURRENT, &ulPos);
  DosQFileInfo(hFileHandles[nChan], 1, &fsts, sizeof(fsts));
  if (ulPos >= fsts.cbFile)
    fFileFlags[nChan] |= FILE_EOF | FILE_EOL;

  // 00011  If we started with a double quote then don't write it out.
  if (buf[nOff] == '"')
    SetStrVariable(wVarNo, &buf[nOff+1], i-1-nOff);
  else
    SetStrVariable(wVarNo, &buf[nOff], i-nOff);

  return (0);
}

int FileReadLine(nChan, wVarNo)
  int  nChan;
  WORD wVarNo;
{
  FILESTATUS fsts;
  ULONG ulPos;
  char buf[256];
  int  nLen;
  long fpos;
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_INPUT)
    {
    if (nFileModes[nChan] != FILE_RANDOM)
      return (ERRIOE_NOCANREAD);
    else
      return (ERRIOE_NODORANDOM);
    }

  DosChgFilePtr(hFileHandles[nChan], 0L, FILE_CURRENT, &fpos);
  if (DosRead(hFileHandles[nChan], buf, sizeof(buf), &nLen))
    {
    nErrorCode = ERRSYS_READFAULT;
    return (0);
    }

  fFileFlags[nChan] = FILE_OPEN;

  for (i = 0; i < nLen && i < 255; i++)
    {
    if (buf[i] == ASC_CR)
      {
      DosChgFilePtr(hFileHandles[nChan], fpos+i+2, FILE_BEGIN, &ulPos);
      fFileFlags[nChan] |= FILE_EOL;
      break;
      }
    }

  SetStrVariable(wVarNo, buf, i);

  if (nLen == 0 || (nLen != sizeof(buf) && nLen == i+2))
    fFileFlags[nChan] |= FILE_EOF | FILE_EOL;

  return (0);
}

int FileWrite(nChan, pBuf, bLen)
  int  nChan;
  BYTE *pBuf;
  BYTE bLen;
{
  USHORT usBytes;
  char buf[8];
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_OUTPUT && nFileModes[nChan] != FILE_APPEND)
    {
    if (nFileModes[nChan] != FILE_RANDOM)
      return (ERRIOE_NOCANWRITE);
    else
      return (ERRIOE_NODORANDOM);
    }


  if (i = DosWrite(hFileHandles[nChan], pBuf, bLen, &usBytes))
    {
    nErrorCode = XlatError(i);
    return (0);
    }
  else if (usBytes != (int)bLen)
    {
    nErrorCode = ERRDOS_DISKFULL;
    return (0);
    }

  return (0);
}

int FileWriteLine(nChan)
  int  nChan;
{
  USHORT usBytes;
  char buf[2];
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_OUTPUT && nFileModes[nChan] != FILE_APPEND)
    {
    if (nFileModes[nChan] != FILE_RANDOM)
      return (ERRIOE_NOCANWRITE);
    else
      return (ERRIOE_NODORANDOM);
    }

  buf[0] = ASC_CR;
  buf[1] = ASC_LF;

  if (i = DosWrite(hFileHandles[nChan], buf, 2, &usBytes))
    {
    nErrorCode = XlatError(i);
    return (0);
    }
  if (usBytes != 2)
    {
    nErrorCode = ERRDOS_DISKFULL;
    return (0);
    }

  return (0);
}

int FileGetRec(nChan, nLength, wVarNo)
  int  nChan;
  int  nLength;
  WORD wVarNo;
{
  USHORT usBytes;
  char buf[256];
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_RANDOM)
    return (ERRIOE_NODOTEXT);

  if (DosRead(hFileHandles[nChan], buf, nLength, &usBytes))
    nErrorCode = ERRSYS_READFAULT;
  else
    {
    if (usBytes < nLength)
      fFileFlags[nChan] = FILE_OPEN | FILE_EOF;

    if (usBytes < nLength)
      nLength = usBytes;

    SetStrVariable(wVarNo, buf, nLength);
    }

  return (0);
}

int FilePutRec(nChan, pBuf, bLen)
  int  nChan;
  BYTE *pBuf;
  BYTE bLen;
{
  USHORT usBytes;
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_RANDOM)
    return (ERRIOE_NODOTEXT);

  if (i = DosWrite(hFileHandles[nChan], pBuf, (int)bLen, &usBytes))
    nErrorCode = XlatError(i);
  else if (usBytes != (int)bLen)
    nErrorCode = ERRDOS_DISKFULL;

  return (0);
}

int FileSeek(nChan, lPos)
  int  nChan;
  long lPos;
{
  ULONG ulPos;
  register i;


  nErrorCode = 0;

  if (!(fFileFlags[nChan] & FILE_OPEN))
    return (ERRIOE_CHANNELNOTOPEN);

  if (nFileModes[nChan] != FILE_RANDOM)
    return (ERRIOE_NODOTEXT);

  DosChgFilePtr(hFileHandles[nChan], lPos, FILE_BEGIN, &ulPos);
  if (ulPos != lPos)
    nErrorCode = ERRSYS_SEEKERROR;

  return (0);
}

int FileClose(nChan)
  int  nChan;
{
  register i;


  if (nChan == -1)
    {
    for (i = 0; i < sizeof(hFileHandles)/sizeof(int); i++)
      {
      if (fFileFlags[i] & FILE_OPEN)
	{
	DosClose(hFileHandles[i]);
	fFileFlags[i] = 0;
	}
      }
    }
  else
    {
    if (!(fFileFlags[nChan] & FILE_OPEN))
      return (ERRIOE_CHANNELNOTOPEN);
    DosClose(hFileHandles[nChan]);
    fFileFlags[nChan] = 0;
    }

  return (0);
}
