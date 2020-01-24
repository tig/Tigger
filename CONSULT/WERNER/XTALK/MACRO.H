/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Macro Include File (MACRO.H)                                    *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#define MAX_MACRO_ROW    4
#define MAX_MACRO_COL   12

struct stMacro
  {
  char szMacro[48][41];
  char szLabel[48][13];
  char fMenu[48];
  BYTE bColor[48];
  };

extern struct stMacro Macro;
extern char cMacroRow[4];
extern char cMacroRows;

extern BOOL InitMacro();
extern void NewMacroData();
extern void UpdateMacro();
extern void FlashMacro(short);
