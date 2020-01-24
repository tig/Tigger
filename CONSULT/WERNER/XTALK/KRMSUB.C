/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Kermit Subroutine Module (KRMSUB.C)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains subroutines used by Kermit.                *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "kermit.h"
#include "term.h"
#include "prodata.h"
#include "variable.h"


void KermitFileName(pName)
  PSTR pName;
{
  char szDir[MAX_PATH];
  int  i;


  GetStrVariable(VAR_DIRFIL, szDir, sizeof(szDir));

  if (strchr(pName, '/') || strchr(pName, '\\') || strchr(pName, ':'))
    return;

  i = strlen(szDir);
  if (szDir[i-1] != '/' && szDir[i-1] != '\\')
    {
    szDir[i]   = '\\';
    szDir[i+1] = NULL;
    }

  strcat(szDir, pName);
  strcpy(pName, szDir);
}


void KermitConvert(pFileName)
  PSTR pFileName;
{
  PSTR pStr1;
  PSTR pStr2;


  pStr1 = pStr2 = pFileName;

  while (*pStr1)
    {
    if (*pStr1 == ':' || *pStr1 == '/' || *pStr1 == '\\')
      pStr2 = pStr1 + 1;

    pStr1++;
    }

  pStr1 = pFileName;

  while (*pStr2)
    *pStr1++ = *pStr2++;

  *pStr1 = NULL;
}
