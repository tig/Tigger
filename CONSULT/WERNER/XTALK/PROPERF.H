/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Scrollback Include File (PROPERF.H)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 09/28/89 00039 Improve Scrollback buffer support.          *
*                                                                             *
\*****************************************************************************/

#define VSS_MAXPAGES	16
#define VSS_PAGESIZE	4096
#define VSS_REALMEM	0
#define VSS_EMSMEM	1


extern int  nScrollMemory;
extern int  nScrollType;
extern int  nCommPortMemory;

extern HANDLE hVssPages[VSS_MAXPAGES];
extern LPSTR  lpVssPages[VSS_MAXPAGES];
extern int    nVssPagesUsed;

extern HANDLE hVssVirtFile;
extern WORD   wVssVirtTemp;

extern WORD   nVssFirstLineInx;
extern WORD   nVssLastLineInx;
extern int    nVssLastLineLen;

extern int    nVssCurLineCnt;
extern int    nVssCurLinePos;
extern short  sVssOldLinePos;
extern int    sVssCurLineBuf[VID_MAXCOL+1];

extern void VssAddLine(int *);
extern PINT VssGetLine(int);
extern void VssScrollMode();
extern void VssScrollToggle();
extern BOOL VssScrollUp(int);
extern BOOL VssScrollDown(int);
extern BOOL VssScrollRight(int);
extern BOOL VssScrollLeft(int);
extern BOOL VssGotoTop();
extern BOOL VssGotoBot();

extern BOOL InitPerformance();
extern void TermPerformance();

extern void VssCreate();
extern void VssDelete();
