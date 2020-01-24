/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Session Include File (SESSION.H)                                 */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Global Definitions							      */
/*									      */
/******************************************************************************/

#define SESSION_OFFLINE     0       /* Offline                              */
#define SESSION_CONNECT     1       /* Automatic modem init and dial        */
#define SESSION_MANUAL      2       /* User dialing modem manually          */
#define SESSION_LEXIS       3       /* Lexis                                */
#define SESSION_PROCESSING  4       /* Processing                           */
#define SESSION_DOCUMENT    5       /* Processing                           */
#define SESSION_DISCONNECT  6       /* Disconnect                           */
#define SESSION_RECYCLE     7       /* Recycle                              */
#define SESSION_NEXT        8       /* Next                                 */

#define KEYBD_SESSION       0       /* Send keyboard input to session logic */
#define KEYBD_DIRECT        1       /* Send keyboard input to comm port     */
#define KEYBD_LOCAL         2       /* Local processing of keyboard input   */
#define KEYBD_LOCKED        3       /* Keyboard is locked                   */


/******************************************************************************/
/*									      */
/*  Output Global Variables                                                   */
/*									      */
/******************************************************************************/

  extern BOOL   fSessOnline;        /* True if user selected online         */
  extern BOOL   fSessProcess;       /* True if session has the data stream  */
  extern BOOL   fSessRobot;         /* Robot automatic process has control  */
  extern WORD   wSessLineState;     /* The current state of the comm line   */
  extern WORD   wSessKeyboard;      /* The status of the keyboard data      */


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern BOOL   SessInit(void);
  extern void   SessTerm(void);
  extern void   SessTaskProcess(void);
  extern void   SessTellCarrierHigh(void);
  extern void   SessTellOnLexis(void);
  extern void   SessTellConnection(BOOL);
  extern void   SessTellDisconnect(BOOL bIcon);
  extern void   SessTellStartDocument(void);
  extern void   SessTellEndDocument(void);
  extern void   SessTellCommand(PBYTE, WORD);
  extern void   SessTellGeneric(PBYTE, WORD, PBYTE);
  extern void   SessTellKeyboard(WORD, WORD);
  extern void   SessTellKeyboardLocked(void);
  extern void   SessTellKeyboardUnlocked();
  extern void   SessCanCommand(int);
  extern void	SessStrCommand(int);
  extern void   SessIntCommand(int, int);
  extern void   SessArgCommand(int, char *, int);
