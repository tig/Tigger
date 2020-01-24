/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Library Routines (LIBRARY.C)                                     */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "lexis.h"
#include "library.h"
#include "data.h"


typedef struct
  {
  WORD   environment;
  LPSTR  commandline;
  LPSTR  FCB1;
  LPSTR  FCB2;
  } EXECBLOCK;
static EXECBLOCK exec;
static char szSpawnAppName[32];
static char szCommandLine[64];
static WORD wFCB1Contents[2];

  FILE WinFiles[WINOPEN];

extern int Int21Function4B(BYTE, LPSTR, LPSTR);
extern int PInt21Function4B(BYTE, LPSTR, LPSTR);


int SetFilePath(path)
  char *path;
{
  int drive;

  if (path[0] == 0)
    return (0);

  strupr(path);
  if (path[1] == ':')
    {
    _dos_getdrive(&drive);

    if (drive != path[0] - '@')
      _dos_setdrive(path[0] - '@', &drive);

    if (path[1] == ':' && path[2] == '\0')
      return (chdir("\\"));
    else
      return (chdir(&path[2]));
    }
  else
    return (chdir(path));
}

void SpawnApp(char *szAppName, char *szParams)
  {
  strcpy(szSpawnAppName, szAppName);
  strcpy(szCommandLine,  szParams);

  GlobalCompact(-1L);
  LockData(0);

  exec.environment = 0;
  exec.commandline = szCommandLine;

  wFCB1Contents[0] = 2;
  wFCB1Contents[1] = SW_SHOWNORMAL;
  exec.FCB1 = (LPSTR)wFCB1Contents;
  exec.FCB2 = (LPSTR)NULL;

  if (nWorkVersion < 3)
    Int21Function4B(0, (LPSTR)szSpawnAppName, (LPSTR)&exec);
  else
    PInt21Function4B(0, (LPSTR)szSpawnAppName, (LPSTR)&exec);

  UnlockData(0);
  }


void ChangeMenuMsg(wMenu, nMsgNo)
  WORD wMenu;
  int  nMsgNo;
{
  char buf[64];

  LoadString(hWndInst, nMsgNo, buf, sizeof(buf));
  ChangeMenuText(wMenu, buf);
}

void ChangeMenuText(wMenu, pBuf)
  WORD wMenu;
  PSTR pBuf;
{
  char tmp[64];
  register int i;


  GetMenuString(hWndMenu, wMenu, tmp, sizeof(tmp), MF_BYCOMMAND);

  for (i = 0; i < sizeof(tmp) && tmp[i] != '(' && tmp[i]; i++);
  if (tmp[i] == '(')
    {
    strcpy(&tmp[i+1], pBuf);
    i = strlen(tmp);
    while (tmp[i-1] == ' ')
      i--;
    tmp[i++] = ')';
    tmp[i++] = '.';
    tmp[i++] = '.';
    tmp[i++] = '.';
    tmp[i] = 0;
    }
  else
    {
    for (i = 0; i < sizeof(tmp) && tmp[i] != '/' && tmp[i]; i++);
    if (tmp[i] == '/')
      strcpy(&tmp[i+1], pBuf);
    else
      strcpy(tmp, pBuf);
    i = strlen(tmp);
    while (tmp[i-1] == ' ')
      i--;
    }

  ChangeMenu(hWndMenu, wMenu, tmp, wMenu, MF_BYCOMMAND | MF_CHANGE | MF_STRING);
}

void LibDestore(inpstr, outstr, outsiz)
  BYTE *inpstr;
  BYTE *outstr;
  int  outsiz;
{
  register int i;

  i = 1;
  while (*inpstr)
    {
    switch (*inpstr)
      {
      case '`':
	if (inpstr[1] == '^')
	  {
	  if (i < outsiz)
	    outstr[i++] = '^';
	  inpstr++;
	  }
	else
	  {
	  if (i < outsiz)
	    outstr[i++] = '`';
	  }
	break;

      case '^':
	if (inpstr[1] >= '@' && inpstr[1] <= '_')
	  {
	  if (i < outsiz)
	    outstr[i++] = inpstr[1] - '@';
	  inpstr++;
	  }
	else
	  {
	  if (i < outsiz)
	    outstr[i++] = '^';
	  }
	break;

      case '|':
	if (i < outsiz)
	  outstr[i++] = ASC_CR;
	break;

      default:
	if (i < outsiz)
	  outstr[i++] = *inpstr;
	break;
      }

    inpstr++;
    }

  outstr[0] = (BYTE)(i - 1);
}

void LibEnstore(inpstr, outstr, outsiz)
  BYTE *inpstr;
  BYTE *outstr;
  int  outsiz;
{
  int  inpsiz;
  register int i;


  i = 1;
  inpsiz = (int)(*inpstr++);
  while (inpsiz--)
    {
    if (*inpstr == '\r')
      {
      if (i < outsiz)
	{
	*outstr++ = '|';
	i++;
	}
      }
    else if (*inpstr == '^')
      {
      if (i < outsiz-1)
	{
	*outstr++ = '`';
	i++;
	*outstr++ = '^';
	i++;
	}
      }
    else if (*inpstr < ' ')
      {
      if (i < outsiz-1)
	{
	*outstr++ = '^';
	i++;
	*outstr++ = *inpstr + '@';
	i++;
	}
      }
    else
      {
      if (i < outsiz)
	{
	*outstr++ = *inpstr;
	i++;
	}
      }

    inpstr++;
    }

  *outstr = NULL;
}

int WhichRadioButton(hDlg, wFirstID, wLastID, wDefault)
  HWND hDlg;
  WORD wFirstID;
  WORD wLastID;
  WORD wDefault;
{
  register int i;


  for (i = wFirstID; i <= wLastID; i++)
    {
    if (IsDlgButtonChecked(hDlg, i))
      return (i);
    }

  return (wDefault);
}

void SetDlgItemLength(hDlg, wID, nLength)
  HWND hDlg;
  WORD wID;
  int  nLength;
{
  SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, nLength, 0L);
}

int lmemcmp(out, in, len)
  LPSTR out;
  LPSTR in;
  WORD	len;
{
  while (len--)
    {
    if (*out++ != *in++)
      return (1);
    }

  return (0);
}

void lmemcpy(out, in, len)
  LPSTR out;
  LPSTR in;
  WORD	len;
{
  while (len--)
    *out++ = *in++;
}

void lmemset(out, value, len)
  LPSTR out;
  BYTE	value;
  WORD	len;
{
  while (len--)
    *out++ = value;
}

void LibBinToAsc(pAsc, pBin, nBin)
  BYTE *pAsc;
  BYTE *pBin;
  int  nBin;
{
  register i;


  while (nBin-- > 0)
    {
    i = (*pBin >> 4) & 0x0F;
    if (i < 10)
      *pAsc++ = (char)('0' + i);
    else
      *pAsc++ = (char)('A' + i - 10);

    i = *pBin & 0x0F;
    if (i < 10)
      *pAsc++ = (char)('0' + i);
    else
      *pAsc++ = (char)('A' + i - 10);

    pBin++;
    }

  *pAsc = NULL;
}

void LibAscToBin(pBin, pAsc)
  BYTE *pBin;
  BYTE *pAsc;
{
  register i;

  while (*pAsc)
    {
    if (*pAsc < 'A')
      i = (*pAsc - '0') * 16;
    else
      i = (*pAsc - 'A' + 10) * 16;
    pAsc++;

    if (*pAsc < 'A')
      i += *pAsc - '0';
    else
      i += *pAsc - 'A' + 10;
    pAsc++;

    *pBin++ = (char)i;
    }
}

int LibStrToInt(szBuf, lInteger)
  char *szBuf;
  long *lInteger;
{
  register len;


  *lInteger = 0L;
  switch (szBuf[(len = strlen(szBuf))-1])
    {
    case 'H':
    case 'h':
      while (len-- > 1)
	{
	if (!isxdigit(*szBuf))
	  return (TRUE);
	if (*szBuf < 'A')
	  *lInteger = *lInteger * 16L + (*szBuf - '0');
	else
	  *lInteger = *lInteger * 16L + (toupper(*szBuf) - 'A' + 10);
	szBuf++;
	}
      break;

    case 'O':
    case 'o':
    case 'Q':
    case 'q':
      while (len-- > 1)
	{
	if (!isdigit(*szBuf) || *szBuf > '7')
	  return (TRUE);
	*lInteger = *lInteger * 8L + (*szBuf - '0');
	szBuf++;
	}
      break;

    case 'B':
    case 'b':
      while (len-- > 1)
	{
	if (*szBuf != '0' && *szBuf != '1')
	  return (TRUE);
	*lInteger = *lInteger * 2L + (*szBuf - '0');
	szBuf++;
	}
      break;

    case 'K':
    case 'k':
      while (len-- > 1)
	{
	if (!isdigit(*szBuf))
	  return (TRUE);
	*lInteger = *lInteger * 10L + (*szBuf - '0');
	szBuf++;
	}
      *lInteger *= 1024L;
      break;

    default:
      while (len-- > 0)
	{
	if (!isdigit(*szBuf))
	  return (TRUE);
	*lInteger = *lInteger * 10L + (*szBuf - '0');
	szBuf++;
	}
      break;
    }

  return (FALSE);
}

void GetProfileData(szApp, szKey, vDefault, vString, nSize)
  char *szApp;
  char *szKey;
  char *vDefault;
  char *vString;
  int  nSize;
{
  char szDefault[128], szString[128];

  LibEnstore(vDefault, szDefault, sizeof(szDefault));
  GetProfileString(szApp, szKey, szDefault, szString, sizeof(szString));
  LibDestore(szString, vString, nSize);
}

void WriteProfileData(szApp, szKey, vString)
  char *szApp;
  char *szKey;
  char *vString;
{
  char szNull[1], szString[128], szBuf[128];

  szNull[0] = 0;
  LibEnstore(vString, szString, sizeof(szString));
  GetProfileString(szApp, szKey, szNull, szBuf, sizeof(szBuf));
  if (strcmp(szString, szBuf))
    WriteProfileString(szApp, szKey, szString);
}

int IsEnoughDiskSpace(nDrive, lSpace)
  int  nDrive;
  long lSpace;
{
  struct diskfree_t drive;
  long	lTotal;

  _dos_getdiskfree(nDrive, &drive);

  lTotal = (long)drive.avail_clusters *
	   (long)drive.sectors_per_cluster *
	   (long)drive.bytes_per_sector ;

  if (lTotal >= lSpace)
    return (TRUE);
  else
    return (FALSE);
}

char * StripPath(path)
  char *path;
{
  char *ptr;

  for (ptr = path; *path; path++)
    {
    if (*path == '/' || *path == '\\' || *path == ':')
      ptr = path + 1;
    }

  return (ptr);
}

FILE * fopen(path, pMode)
  char *path;
  char *pMode;
{
  FILE *pFile;
  int  i;


  pFile = 0;
  for (i = 0; i < WINOPEN; i++)
    {
    if (WinFiles[i].flag == 0)
      {
      pFile = &WinFiles[i];
      break;
      }
    }

  if (pFile == 0)
    return (0);

  if ((pFile->hmem = LocalAlloc(LMEM_MOVEABLE, BUFSIZ)) == 0)
    return (0);

  pFile->flag = 0;

  while (*pMode)
    {
    switch (*pMode)
      {
      case 'r':
	pFile->flag |= _IOREAD;
	break;

      case 'w':
	pFile->flag |= _IOWRT;
	break;

      case 'a':
	pFile->flag |= _IORW;
	pFile->flag |= _IOAPPEND;
	break;

      case '+':
	pFile->flag |= _IORW;
	break;

      default:
	break;
      }
    pMode++;
    }

  if (pFile->flag & _IOAPPEND)
    {
    if ((i = _dos_creatnew(path, _A_NORMAL, &pFile->file)) != 0)
      i = _dos_open(path, O_RDWR, &pFile->file);

    if (i == 0)
      fseek(pFile, 0L, SEEK_END);

    pFile->cnt = BUFSIZ;
    }
  else if (pFile->flag & _IOWRT)
    {
    i = _dos_creat(path, _A_NORMAL, &pFile->file);
    pFile->cnt = BUFSIZ;
    }
  else
    {
    i = _dos_open(path, O_RDONLY, &pFile->file);
    pFile->cnt = 0;
    }

  if (i)
    {
    LocalFree(pFile->hmem);
    pFile->flag = 0;
    return (0);
    }

  pFile->base = LocalLock(pFile->hmem);
  pFile->ptr  = pFile->base;

  return (pFile);
}

FILE * fdopen(file, pMode)
  int  file;
  char *pMode;
{
  FILE *pFile;
  int  i;


  pFile = 0;
  for (i = 0; i < WINOPEN; i++)
    {
    if (WinFiles[i].flag == 0)
      {
      pFile = &WinFiles[i];
      break;
      }
    }

  if (pFile == 0)
    return (0);

  if ((pFile->hmem = LocalAlloc(LMEM_MOVEABLE, BUFSIZ)) == 0)
    return (0);

  pFile->flag = 0;

  while (*pMode)
    {
    switch (*pMode)
      {
      case 'r':
	pFile->flag |= _IOREAD;
	break;

      case 'w':
	pFile->flag |= _IOWRT;
	break;

      case 'a':
	pFile->flag |= _IORW;
	pFile->flag |= _IOAPPEND;
	break;

      case '+':
	pFile->flag |= _IORW;
	break;

      default:
	break;
      }
    pMode++;
    }


  pFile->file = file;

  if (pFile->flag & _IOAPPEND)
    {
    fseek(pFile, 0L, SEEK_END);

    pFile->cnt = BUFSIZ;
    }
  else if (pFile->flag & _IOWRT)
    {
    pFile->cnt = BUFSIZ;
    }
  else
    {
    pFile->cnt = 0;
    }

  pFile->base = LocalLock(pFile->hmem);
  pFile->ptr  = pFile->base;

  return (pFile);
}


int fclose(pFile)
  FILE *pFile;
{
  int  bytes;


  if (pFile->flag & (_IOWRT | _IORW))
    {
    if (pFile->cnt != BUFSIZ)
      {
      _dos_write(pFile->file, pFile->base, BUFSIZ - pFile->cnt, &bytes);
      }
    }

  LocalUnlock(pFile->hmem);
  LocalFree(pFile->hmem);
  _dos_close(pFile->file);
  pFile->flag = 0;

  return (0);
}


char * fgets(buf, len, file)
  char *buf;
  int  len;
  FILE *file;
{
  register char *ptr;
  register int	i;


  if (feof(file))
    return 0;

  ptr = buf;
  while ((i = getc(file)) != '\n')
    {
    if (i == -1)
      break;

    if (len > 1)
      {
      *ptr++ = (char)i;
      len--;
      }
    }

  *ptr = NULL;

  return (buf);
}


int ungetc(ch, file)
  int  ch;
  FILE *file;
{
  file->cnt++;
  file->ptr--;

  return (*file->ptr = (char)ch);
}

void rewind(file)
  FILE *file;
{
  fseek(file, 0L, SEEK_SET);
}


int fseek(file, addr, seek)
  FILE *file;
  long addr;
  int  seek;
{
  union REGS inregs, outregs;


  inregs.h.ah = 0x42;
  inregs.x.cx = FP_SEG(addr);
  inregs.x.dx = FP_OFF(addr);
  inregs.h.al = (unsigned char)seek;
  inregs.x.bx = file->file;
  intdos(&inregs, &outregs);

  if (outregs.x.cflag)
    return (-1);

  file->cnt = 0;

  return (0);
}

long filelength(file)
  const int file;
{
  union REGS inregs, outregs;
  long length;
  int  posseg, posoff;


  inregs.h.ah = 0x42;
  inregs.x.cx = 0;
  inregs.x.dx = 0;
  inregs.h.al = 1;
  inregs.x.bx = file;
  intdos(&inregs, &outregs);

  posseg = outregs.x.dx;
  posoff = outregs.x.ax;

  inregs.h.ah = 0x42;
  inregs.x.cx = 0;
  inregs.x.dx = 0;
  inregs.h.al = 2;
  inregs.x.bx = file;

  intdos(&inregs, &outregs);

  length = ((long)outregs.x.dx << 16) | outregs.x.ax;

  inregs.h.ah = 0x42;
  inregs.x.cx = posseg;
  inregs.x.dx = posoff;
  inregs.h.al = 0;
  inregs.x.bx = file;

  intdos(&inregs, &outregs);

  return (length);
}


int unlink(path)
  const char *path;
{
  union REGS inregs, outregs;


  inregs.h.ah = 0x41;
  inregs.x.dx = (int)path;
  intdos(&inregs, &outregs);

  if (outregs.x.cflag)
    return (-1);

  return (0);
}


int wfilbuf(pFile)
  FILE *pFile;
{
  int  bytes;


  if (pFile->flag & (_IOERR | _IOEOF))
    {
    pFile->cnt = 0;
    return EOF;
    }

  if (pFile->base == 0)
    {
    /* reallocate buffer */
    }

  if (_dos_read(pFile->file, pFile->base, BUFSIZ, &bytes) != 0)
    {
    pFile->flag |= _IOERR;
    pFile->cnt = 0;
    return EOF;
    }

  if (bytes == 0)
    {
    pFile->flag |= _IOEOF;
    pFile->cnt = 0;
    return EOF;
    }

  pFile->ptr = pFile->base;
  pFile->cnt = bytes - 1;

  return (*pFile->ptr++);
}


int wflsbuf(ch, pFile)
  int	ch;
  FILE *pFile;
{
  int  bytes;


  if (pFile->flag & _IOERR)
    {
    pFile->cnt = 0;
    return EOF;
    }

  if (pFile->base == 0)
    {
    /* reallocate buffer */
    }

  if (_dos_write(pFile->file, pFile->base, BUFSIZ, &bytes) != 0)
    {
    pFile->flag |= _IOERR;
    pFile->cnt = 0;
    return EOF;
    }

  if (bytes != BUFSIZ)
    {
    pFile->flag |= _IOERR;
    pFile->cnt = 0;
    return EOF;
    }

  pFile->ptr = pFile->base;
  pFile->cnt = BUFSIZ - 1;

  return (*pFile->ptr++ = (char)ch);
}

int fwrite(data, size, count, file)
  char *data;
  int  size;
  int  count;
  FILE *file;
{
  int  len;

  len = size * count;
  while (len--)
    putc(*data++, file);

  return (count);
}


//
//
//
BOOL ValidateSpeed(char *ptr)
  {
  register i;

  i = strlen(ptr);
  while (i)
    {
    if (ptr[i-1] != ' ')
      break;
    ptr[--i] = '\0';
    }

  if (*ptr == '\0')
    return (FALSE);

  while (*ptr)
    {
    if (*ptr < '0' || *ptr > '9')
      return (FALSE);
    ptr++;
    }

  return (TRUE);
  }


//
//
//
BOOL ValidateNumber(char *ptr)
  {
  register i;

  i = strlen(ptr);
  while (i)
    {
    if (ptr[i-1] != ' ')
      break;
    ptr[--i] = '\0';
    }

  while (*ptr)
    {
    if ((*ptr < '0' || *ptr > '9') && *ptr != ',' && *ptr != '-' && *ptr != '#' && *ptr != '*')
      return (FALSE);
    ptr++;
    }

  return (TRUE);
  }


//
//
//
BOOL ValidatePath(char *ptr)
  {
  OFSTRUCT ofStruct;


  if (OpenFile(ptr, &ofStruct, OF_PARSE) || DlgSearchSpec(ptr))
    return (FALSE);
  else
    return (TRUE);
  }


//
//
//
BOOL ValidateFilename(char *ptr)
  {
  while (*ptr)
    {
    if (*ptr == ' ' || *ptr == '*' || *ptr == '?')
      return (FALSE);

    ptr++;
    }

  return (TRUE); // Good
  }
