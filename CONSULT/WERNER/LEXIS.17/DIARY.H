/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Diary Include File (DIARY.H)                                     */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

extern BOOL bDiary;
extern void DiaryOff();
extern void DiaryOn();
extern void DiaryUpdate();
extern void DiaryDownload(char *szDocFile);
extern void DiaryNewFile();
extern void DiaryOpenFile();
extern WORD DiaryWriteData(BYTE *, unsigned);
extern WORD DiaryWriteNewline();
extern void DiaryTerm();
