/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*  Program:   Generate Help File (GENHELP.C)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This program reads the XTHELP.TXT file and generates a          *
*             XTHELP.DAT file.                                                *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>

typedef unsigned int WORD;

main()
{
  FILE	*inFile;
  FILE	*otFile;
  short sItems;
  long	wHeader, wData, wOffset;
  char	buf[256];
  register WORD i;


  /* Phase 1 */
  sItems  = 0;
  wHeader = 0;
  inFile = fopen("xthelp.txt", "r");
  while (fgets(buf, sizeof(buf), inFile))
    {
    if (buf[0] == '#')
      continue;

    i = strlen(buf);
    if (i)
      buf[--i] = NULL;

    if (i && buf[0] != ' ')
      {
      wHeader += i + 5;
      printf("[%d] %s\n", sItems, buf);
      sItems++;
      }
    }
  wHeader++;
  fclose(inFile);


  /* Phase 2 */
  wData = 0;
  inFile = fopen("xthelp.txt", "r");
  otFile = fopen("xtalk.hlp", "wb");
  while (fgets(buf, sizeof(buf), inFile))
    {
    if (buf[0] == '#')
      continue;

    i = strlen(buf);
    if (i)
      buf[--i] = NULL;

    if (i)
      {
      if (buf[0] != ' ')
        {
        fputs(buf, otFile);
        fputc(NULL, otFile);
        wOffset = wHeader+wData;
        fputc(((char *)(&wOffset))[0], otFile);
        fputc(((char *)(&wOffset))[1], otFile);
        fputc(((char *)(&wOffset))[2], otFile);
        fputc(((char *)(&wOffset))[3], otFile);
        }
      else
        {
        register j;
        for (j = 0; buf[j] == ' '; j++);

        wData += i + 1 - j;
        }
      }
    else
      wData++;
    }
  fclose(inFile);
  fputc(NULL, otFile);


  /* Phase 3 */
  inFile = fopen("xthelp.txt", "r");
  while (fgets(buf, sizeof(buf), inFile))
    {
    if (buf[0] == '#')
      continue;

    i = strlen(buf);
    if (i)
      buf[--i] = NULL;

    if (i && buf[0] == ' ')
      {
      register j;
      for (j = 0; buf[j] == ' '; j++);

      fputs(&buf[j], otFile);
      fputc(NULL, otFile);
      }
    else if (i == 0)
      fputc(NULL, otFile);

    }
  fclose(inFile);
  fputc(NULL, otFile);
  fclose(otFile);
}
