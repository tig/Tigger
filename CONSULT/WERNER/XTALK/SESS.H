/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Session Include File (SESS.H)                                   *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#define SESSION_NORMAL	0
#define SESSION_RAW	1
#define SESSION_VISUAL	2
#define SESSION_MANUAL	3

struct stSess
  {
  char szDescription[41];
  char szNumber[81];        /* 41 */
  char szModifier[17];
  char szUserid[41];
  char szPassword[41];
  char szScript[41];
  char filler1[14];
  char fPrinterOn;
  char fCaptureOn;
  char szNetID[41];         /* 17 */
  char szLog[17];
  char cLogOption;
  int  nPatience;
  int  nRedialCount;
  int  nRedialWait;
  char fGraphics;
  char filler2;
  char szHeader[64];
  char szFooter[64];
  char szMisc[41];          // 00030 Add MISC to profile.
  char szFiller[22];	    // 00030 Add MISC to profile.
  char fLabelOn;
  };


extern struct stSess Sess;

extern BOOL InitSession();
extern void NewSessionData();
