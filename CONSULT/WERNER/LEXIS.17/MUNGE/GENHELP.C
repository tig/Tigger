/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Generate Help File (GENHELP.C)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#include <stdio.h>

typedef unsigned int WORD;

main()
{
  FILE *inFile;
  FILE *otFile;
  long wHeader, wData, wOffset;
  char buf[256];
  register WORD i;


  /* Phase 1 */
  wHeader = 0;
  inFile = fopen("help.txt", "r");
  while (fgets(buf, sizeof(buf), inFile))
    {
    if (buf[0] == '#')
      continue;

    i = strlen(buf);
    if (i)
      buf[--i] = NULL;

    while (i && buf[i-1] == ' ')
      buf[--i] = '\0';

    if (i && buf[0] != ' ')
      wHeader += i + 5;
    }
  wHeader++;
  fclose(inFile);


  /* Phase 2 */
  wData = 0;
  inFile = fopen("help.txt", "r");
  otFile = fopen("help.dat", "wb");
  while (fgets(buf, sizeof(buf), inFile))
    {
    if (buf[0] == '#')
      continue;

    i = strlen(buf);
    if (i)
      buf[--i] = NULL;

    while (i && buf[i-1] == ' ')
      buf[--i] = '\0';

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
      //register j;
      //for (j = 0; buf[j] == ' '; j++);

      //wData += i + 1 - j;
        wData += i - 1;
        }
      }
    else
      wData++;
    }
  fclose(inFile);
  fputc(NULL, otFile);


  /* Phase 3 */
  inFile = fopen("help.txt", "r");
  while (fgets(buf, sizeof(buf), inFile))
    {
    if (buf[0] == '#')
      continue;

    i = strlen(buf);
    if (i)
      buf[--i] = NULL;

    while (i && buf[i-1] == ' ')
      buf[--i] = '\0';

    if (i && buf[0] == ' ')
      {
      register j;
      for (j = 0; buf[j] == ' '; j++);

      j = 2;
      fputs(&buf[j], otFile);
      fputc(NULL, otFile);
      printf(&buf[j]);
      printf("\n");
      }
    else if (i == 0)
      {
      fputc(NULL, otFile);
      printf("Null mark\n\n");
      }
    }
  fclose(inFile);
  fputc(NULL, otFile);
  fputc(NULL, otFile);
  fputc(NULL, otFile);
  fclose(otFile);
}
