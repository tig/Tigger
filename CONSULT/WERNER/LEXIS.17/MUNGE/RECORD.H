/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Record Include File (RECORD.H)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

extern BOOL bRecording;
extern void RecordOff();
extern void RecordOn();
extern void RecordNewFile();
extern void RecordOpenFile();
extern WORD RecordWriteData(BYTE *, unsigned);
extern WORD RecordWriteNewline();
extern void RecordTerm();
