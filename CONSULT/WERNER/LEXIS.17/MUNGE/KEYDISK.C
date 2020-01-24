/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Key Disk for HyperRom (KEYDISK.C)                                */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: Use to read HyperRom password off key disk.                      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include <ctype.h>
#include "lexis.h"
#include "data.h"

extern cdecl far module(char *);


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL KeyDiskCheck();						      */
/*									      */
/*  Called in the application's initialize routine to check for a key disk.   */
/*  Return false if the user wants to cancel the whole session. 	      */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL KeyDiskCheck()
  {
  BYTE	 abBuf[256];
  register i;


  fWorkHyperRom     = FALSE;
  fWorkHyperRomMode = FALSE;
  memset(szWorkHyperRom, '\0', sizeof(szWorkHyperRom));
  memset(szWorkPassword, '\0', sizeof(szWorkPassword));

#ifdef HYPERROM
  // Call the key disk routine to load the HyperRom userid & password.
  memset(abBuf, '\0', sizeof(abBuf));
  strcpy(abBuf, "0123456789ABCDEF");
  i = module(abBuf);
  LineToTerminal(abBuf);


  if (i == 0)
    {
    register j;

    i = j = 0;
    while (isalnum(abBuf[i]))
      szWorkHyperRom[j++] = abBuf[i++];
    szWorkHyperRom[j] = '\0';
    i++;

    j = 0;
    while (isalnum(abBuf[i]))
      szWorkPassword[j++] = abBuf[i++];
    szWorkPassword[j] = '\0';

    fWorkHyperRom     = TRUE;
    fWorkHyperRomMode = TRUE;
    }

  if (fWorkHyperRom)
    {
    EnableMenuItem(hWndMenu, CMD_HYPERROM, MF_ENABLED);
    CheckMenuItem(hWndMenu, CMD_HYPERROM, MF_CHECKED);
    InvalidateRect(hChildStatus, NULL, TRUE);
    }
#endif

  return (TRUE);
  }
