/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Global Include File (GLOBAL.H)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

extern char   szCwd[128];
extern char   szEditor[41];
extern char   szStartupScript[41];
extern char   szOutNumber[41];
extern char   szLdNumber[41];
extern char   szMisc[41];
extern char   fCodeSave;
extern char   fBackups;
extern char   fSecret;
extern char   fPasteCrLf;  // 00025 Add optional to CR/LF to host paste.

extern BOOL InitGlobal();
extern BOOL LoadGlobalData();
extern BOOL SaveGlobalData();
