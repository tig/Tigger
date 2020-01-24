/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Data Include File (PRODATA.H)                                   *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*     	       1.01 08/03/89 00004    Auto open capture and printer.	       *
*             1.01 01/31/90 dca00012 PJL Define XWP Version Number limits.    *
*                                                                             *
\*****************************************************************************/

// dca00012 PJL Define the XWP Version Number limits.
// The version number is a 2-byte value - major & minor version numbers
#define MIN_XWP_VER 0x0100				   	// original (1st) version
#define MAX_XWP_VER 0x0101				   	// latest (shipping) version

extern char   fUntitled;
extern char   fDataChanged;
extern char   fNoteChanged;
extern char   szDataCurFile[];

extern char   cNumberLoop;

extern char   fCompileOnly;
extern char   fAlwaysCompile;
extern char   fManualDial;
extern char   szManualNumber[64];

extern int  DataNew();
extern int  DataOpen(char *);
extern int  DataSave();
extern int  DataSaveAs(char *);

extern void   DataChanged();
extern BOOL   DataSaveFirst();
extern BOOL   DataParamRead(PSTR, BOOL);
extern USHORT DataParamLoad(int);
extern void   DataParamSave(int);
extern void   InitSystemNewData();
extern BOOL   DataModemLoad(int);
extern BOOL   DataModemSave(int);
extern BOOL   InitData();
extern BOOL   DataTerm();
extern void   DataDialLoad(HWND, WORD);
extern HWND   hClientDDE;

extern void DataAutoOpen();  // 00004 Auto capture and printer open
