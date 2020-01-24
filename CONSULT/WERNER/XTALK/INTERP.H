/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Interpreter Include File (INTERP.H)                             *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#define FW_COMMA    1
#define FW_EOL	    2

extern WORD wAddr;
extern WORD wLine;

extern int  nStackAddr;
extern int  nStackHigh;

extern int  nStack;
extern WORD wStackType[64];
extern WORD wStackAddr[64];
extern WORD wStackLine[64];

extern int  nKeyBuffer[8];
extern BYTE bKeyIn, bKeyOut;

extern int  InterpreterStart(char *);
extern void InterpreterStop();
extern void InterpreterContinue();
extern int  InterpreterKeyboard(WORD, char);
extern void InterpreterError(int);
extern int  GotoLabel(char *);

extern int  EvalBoolean(char *);
extern int  EvalChannel(int *, char);
extern int  EvalInteger(long *);
extern int  EvalIntVariable(int *, char);
extern int  EvalRange(int *, int, int, char);
extern int  EvalString(BYTE *, BYTE *);
extern int  EvalStrVariable(int *, char);
extern int  EvalTime(long *);
extern int  EvalIntFunct(long *);
extern int  EvalStrFunct(BYTE *, BYTE *);
extern int  EvalIntIntIntInt(int *, int *, int *, int *);
extern int  EvalIntIntIntIntStr(int *, int *, int *, int *, char *);
extern int  EvalIntvar(int *);
extern int  EvalStrvar(int *);
extern int  EvalCtlFlags(WORD *);
extern int  EvalComma();
extern int  EvalEol();
extern int  IfEndOfLine();

extern SHORT DlgboxInterpret();
extern SHORT AlertInterpret();


extern void InterpEscapeKey();
extern void InterpDDEAdvise();
extern int  InterpKeyboard(int);
extern void InterpReset();


/* File I/O definitions and routines */

#define FILE_INPUT	0
#define FILE_OUTPUT	1
#define FILE_APPEND	2
#define FILE_RANDOM	3

//#define FILE_OPEN	  0x01
#define FILE_EOF	0x02
#define FILE_EOL	0x04

extern int  hFileHandles[8];
extern int  nFileModes[8];
extern char fFileFlags[8];

extern int FileOpen(int, char *, int);
extern int FileRead(int, WORD);
extern int FileReadLine(int, WORD);
extern int FileWrite(int, BYTE *, BYTE);
extern int FileWriteLine(int);
extern int FileGetRec(int, int, WORD);
extern int FilePutRec(int, BYTE *, BYTE);
extern int FileSeek(int, long);
extern int FileClose(int);
