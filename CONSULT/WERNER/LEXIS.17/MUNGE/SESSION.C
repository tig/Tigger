/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Communication Module (SESSION.C)                                 */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*  Remarks: This communication module was design to meet the communications  */
/*           needs of the WinLexis project, to be modular, and efficient. Any */
/*	     feature not needed to support the project was not included.  I   */
/*	     did not want or need to write a general purpose communication    */
/*	     module.  But I needed to put all communication relative logic in */
/*	     one place for future ports to other environments.	This is all   */
/*	     well and good, except that I added some of the AI filter logic   */
/*	     here.   Given the code is well documented, adding the AI filter  */
/*	     with the communication code does improve speed, and does not     */
/*	     make the module harder to maintain.			      */
/*									      */
/*	     The AI filter routines are rather simple.	All data going thru   */
/*	     the communication port is broken down into lines.	Lines are     */
/*	     terminated with CR and 'juck' is removed.	Then the completed    */
/*	     lines are scanned and flags are set if conditions are met.  If   */
/*	     filters detect something useful, the AI (other module) routines  */
/*	     are called.  Thus most of the 'grunt' work is done here which    */
/*	     make us efficient, and the real AI work is done else where which */
/*	     make us modular.  You can have your cake and eat it too.	      */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <dos.h>
#include <memory.h>
#include <string.h>
#include "lexis.h"
#include "session.h"
#include "comm.h"
#include "connect.h"
#include "data.h"
#include "diary.h"
#include "aiscan.h"
#include "record.h"
#include "report.h"
#include "robot.h"
#include "term.h"
#include "dialogs.h"
#include "library.h"


/******************************************************************************/
/*									      */
/*  Input Global Variables                                                    */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Output Global Variables                                                   */
/*									      */
/******************************************************************************/

  BOOL   fSessOnline;               /* True if user selected online         */
  BOOL   fSessProcess;              /* True if session has the data stream  */
  BOOL   fSessRobot;                /* Robot automatic process has control  */
  WORD   wSessLineState;            /* The current state of the comm line   */
  WORD   wSessKeyboard;             /* The status of the keyboard data      */


/******************************************************************************/
/*									      */
/*  Local Constants							      */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Local Variables							      */
/*									      */
/******************************************************************************/

  static BOOL   fAutoConnection;    /* Status of automatic connection       */


/******************************************************************************/
/*									      */
/*  Local Procedures							      */
/*									      */
/******************************************************************************/
  static void near SessionOffline(void);
  static void near SessionOnline(void);


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL SessInit()                                                           */
/*									      */
/*  Called in the application's initialize routine so we can get everything   */
/*  in order.  Returns true is successful (for future possiblities).          */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL SessInit()
  {
  /*
  **  Initialize all the Output Global Variables to their proper values.
  */
  SessionOffline();
  fSessProcess   = TRUE;
  wSessLineState = SESSION_OFFLINE;
  wSessKeyboard  = KEYBD_LOCAL;

  sActiveMacro  = FALSE;

  bWorkLogonSent = FALSE;
  bWorkFastPrint = FALSE;

  InfoMessage(IDS_INFO_OFFLINE, NULL);
  SendMessage(hChildMacro, UM_ONLINE, fSessOnline, 0L);
  SendMessage(hChildTerminal, UM_RESET, 1, 0L);
  return (TRUE);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTerm()                                                           */
/*									      */
/*  Called during the application's wrapup routine so we can make sure that   */
/*  everything is ok.                                                         */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTerm()
  {
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CmdHyperRom()                                                        */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void CmdHyperRom()
  {
  if (fWorkHyperRomMode)
    {
    InfoMessage(IDS_INFO_SWITCHING, NULL);
    CheckMenuItem(hWndMenu, CMD_HYPERROM, MF_UNCHECKED);
    fWorkHyperRomMode = FALSE;
    fWorkHyperRomSwitch = TRUE;
    InvalidateRect(hChildStatus, NULL, TRUE);
    SendMessage(hChildStatus, UM_CLIENT, 0, 0L);
    SendMessage(hChildStatus, UM_LIB, 0, 0L);
    }
  else
    {
    InfoMessage(IDS_INFO_SWITCHINGP, NULL);
    CheckMenuItem(hWndMenu, CMD_HYPERROM, MF_CHECKED);
    fWorkHyperRomMode = TRUE;
    fWorkHyperRomSwitch = TRUE;
    InvalidateRect(hChildStatus, NULL, TRUE);
    SendMessage(hChildStatus, UM_CLIENT, 0, 0L);
    SendMessage(hChildStatus, UM_LIB, 0, 0L);
    }

  AIscanReset();
  CommWriteData("\x02.so;.so\r", -1);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CmdSignon()                                                          */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
USHORT CmdSignon()
  {
  if (fSelectedArea)
    MouseSelectClr();

  if (fSessOnline)
    {
    if (wSessKeyboard == KEYBD_DIRECT && fSessProcess == FALSE)
      SessTellDisconnect(FALSE);
    else if (wSessKeyboard != KEYBD_LOCAL)
      MessageBeep(NULL);
    else if (fWorkHyperRomMode)
      SessCanCommand(COMMAND_SIGNOFFP);
    else
      SessCanCommand(COMMAND_SIGNOFF);
    }
  else
    {
    /*
    ** If the comm port is still valid then we are still disconnecting, return.
    */
    if (sCommID >= 0)
      return(0);

    /*
    **  Initialize all the Input Global Variables to their defaults.
    */
    sCommPort       = wDataPort;
    sCommSpeed      = wDataSpeed[0];
    sCommParity     = wDataParity;
    fCommHardware   = TRUE;
    fCommLocal      = fDataHardwire;

#ifdef HYPERROM
    KeyDiskCheck();
#endif

    SendMessage(hChildTerminal, UM_RESET, 0, 0L);

    if (CommOpen() == FALSE)
      {
      MessageDisplay(IDS_MSG_BADCOMM, MB_ICONHAND);
      return (-1);
      }

    SessionOnline();
    nWorkNetwork  = 0;
    AIscanReset();

    CheckMenuItem(hWndMenu, CMD_SIGNON, MF_CHECKED);
    }

  return (0);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CmdExit()                                                            */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void CmdExit()
  {
  if (fSessOnline)
    {
    register i;

    i = MessageBox(hWnd, "Are you sure, Sign Off is normally used",
                   szAppName, MB_OKCANCEL | MB_ICONEXCLAMATION);
    if (i != 1)
      return;
    }

  SendMessage(hChildMacro, UM_ONLINE, FALSE, 0L);

  RecordTerm();
  DiaryTerm();
  PrinterOff();

  if (fSessOnline && wDataDial != DATA_MANUAL)
    {
    fSessProcess   = TRUE;
    wSessLineState = SESSION_DISCONNECT;
    DisconnectStart();
    }
  else
    {
    CommClose();
    ProgramAbort();
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTaskProcess()                                                    */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTaskProcess()
  {
  switch (wSessLineState)
    {
    case SESSION_OFFLINE:
      if (fSessOnline)
        {
        if (wDataDial == DATA_MANUAL)
          {
          fSessProcess  = FALSE;
          wSessKeyboard = KEYBD_DIRECT;
          if (fWorkHyperRomMode)
	    InfoMessage(IDS_INFO_MANUALP, NULL);
	  else
            InfoMessage(IDS_INFO_MANUAL, NULL);

          SendMessage(hChildMacro, UM_MANUAL, 1, 0L);
          break;
          }

        ConnectStart();
        wSessLineState = SESSION_CONNECT;
        }
      break;

    case SESSION_CONNECT:
      if (fAutoConnection)
	{
//#ifdef HYPERROM
//	  CommWriteData(".PSM\r", 5);
//#else
        CommWriteData(".WIN160\r", 8);
//#endif
        fTermNoDisplay = FALSE;
        fSessProcess   = FALSE;
        SessTellOnLexis();
        }
      else
        {
        SessTellDisconnect(FALSE);
        }
      break;

    case SESSION_MANUAL:
      break;

    case SESSION_DOCUMENT:
    //SessTellDisconnect(FALSE);  // Was True
      fTermNoDisplay = FALSE;
      wSessKeyboard  = KEYBD_LOCKED;
      fSessProcess   = FALSE;
      break;

    case SESSION_DISCONNECT:
      CommClose();
      ProgramAbort();
      break;

    case SESSION_RECYCLE:
      CommClose();
      SessInit();
      break;
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellCarrierHigh()                                                */
/*									      */
/*  Called by the communication module to tell the session module that the    */
/*  modem carrier is up.  Used for manual dialing.                            */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellCarrierHigh()
  {
  if (wDataDial == DATA_MANUAL && stricmp("Manual", szDataNetwork[0]))
    {
    fSessProcess   = TRUE;
    wSessKeyboard  = KEYBD_SESSION;
    ConnectStart();
    wSessLineState = SESSION_CONNECT;
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellOnLexis()                                                    */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellOnLexis()
  {
  wSessKeyboard = KEYBD_LOCKED;

  SendMessage(hChildMacro, UM_ONLINE, fSessOnline, 0L);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellConnection(fGood)                                            */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellConnection(BOOL fGood)
  {
  fAutoConnection = fGood;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellDisconnect()                                                 */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellDisconnect(BOOL bAbort)
  {
  SendMessage(hChildMacro, UM_ONLINE, FALSE, 0L);

  if (bAbort)
    ProgramAbort();

  RobotTerm();

  if (bWorkStoreDoc)
    {
    _dos_close(hWorkDocument);
    bWorkStoreDoc = FALSE;
    }

  SessionOffline();
  fSessProcess   = TRUE;
  wSessLineState = SESSION_OFFLINE;
  wSessKeyboard  = KEYBD_SESSION;

  RecordTerm();
  //DiaryTerm();

  SendMessage(hChildStatus, UM_DISCONNECT, 0, 0L);
  CheckMenuItem(hWndMenu, CMD_SIGNON, MF_UNCHECKED);

  if (wDataDial == DATA_MANUAL)
    {
    CommClose();
    SessInit();
    }
  else
    {
    fSessProcess   = TRUE;
    wSessLineState = SESSION_RECYCLE;
    SendMessage(hChildTerminal, UM_RESET, 1, 0L);
    DisconnectStart();
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellStartDocument()                                              */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellStartDocument()
  {
  long lTime;


  PrinterOff();
  //RecordTerm();

  if (fTermNoDisplay)
    return;

  if (PrintDocument())
    {
    fSessProcess   = TRUE;
    wSessKeyboard  = KEYBD_LOCKED;
    wSessLineState = SESSION_DOCUMENT;
    if (fWorkHyperRomMode)
      InfoMessage(IDS_INFO_PRINTINGP, NULL);
    else
      InfoMessage(IDS_INFO_PRINTING, NULL);
    ReportStart();
    }
  else
    {
    wSessKeyboard  = KEYBD_LOCKED;
    fTermNoDisplay = TRUE;

    lTime = GetCurrentTime() + 2000L;
    while (lTime > GetCurrentTime());
    SessCanCommand(COMMAND_STOP);
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellEndDocument()                                                */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellEndDocument()
{
  ReportEnd();
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellKeyboard()                                                   */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellCommand(BYTE *pBuffer, WORD wLength)
  {
  CommWriteChar('\x02');
  CommWriteData(pBuffer, wLength);
  CommWriteChar('\r');

  if (*pBuffer)
    {
    register i;

    for (i = COMMAND_FIRST; i <= COMMAND_LAST; i++)
      {
      if (wLength == strlen(Commands[i].szCommand))
        if (memicmp(pBuffer, Commands[i].szCommand, wLength) == 0)
          break;
      }

    if (i <= COMMAND_LAST)
      InfoMessage(IDS_INFO_PROCESS, Commands[i].szName);
    else
      {
      if (wLength == strlen(szDataLogon))
        {
        if (memicmp(pBuffer, szDataLogon, wLength) == 0)
          InfoMessage(IDS_INFO_PROCESS, Commands[COMMAND_PASSWORD].szName);
        else
          InfoMessage(IDS_INFO_PROCESS, pBuffer);
        }
      else
        InfoMessage(IDS_INFO_PROCESS, pBuffer);
      }
    }
  else
    {
    BYTE bBuf[16];

    bBuf[ 0] = 'T';
    bBuf[ 1] = 'r';
    bBuf[ 2] = 'a';
    bBuf[ 3] = 'n';
    bBuf[ 4] = 's';
    bBuf[ 5] = 'm';
    bBuf[ 6] = 'i';
    bBuf[ 7] = 't';
    bBuf[ 8] = ' ';
    bBuf[ 9] = 'K';
    bBuf[10] = 'e';
    bBuf[11] = 'y';
    bBuf[12] = NULL;
    InfoMessage(IDS_INFO_PROCESS, bBuf);
    }
  wSessKeyboard = KEYBD_LOCKED;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellGeneric()                                                    */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellGeneric(BYTE *pBuffer, WORD wLength, BYTE *pText)
  {
  CommWriteChar('\x02');
  CommWriteData(pBuffer, wLength);
  CommWriteChar('\r');

  InfoMessage(IDS_INFO_PROCESS, pText);
  wSessKeyboard = KEYBD_LOCKED;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellKeyboard()                                                   */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellKeyboard(wMsg, wKey)
  WORD wMsg;
  WORD wKey;
{


}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellKeyboardLocked()                                             */
/*									      */
/*  This routine is called by the terminal logic when we receive a keyboard   */
/*  unlock from LEXIS/NEXIS.                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellKeyboardLocked()
  {
  wSessKeyboard = KEYBD_LOCKED;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessTellKeyboardUnlocked()                                           */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessTellKeyboardUnlocked()
{
  if (fTermPrinting)
    {
    CommWriteData("\002\033+\r", 4);
    }
  else
    {
    if (fRobotWorking)
      {
      if (RobotKeyboardUnlock())
        return;
      }

    fTermNoDisplay = FALSE;
    wSessKeyboard  = KEYBD_LOCAL;

    if (fDataBell)
      MessageBeep(NULL);

    InfoMessage(IDS_INFO_READY, NULL);
    }
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessCanCommand(int nCommand)                                         */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessCanCommand(int nCommand)
{
  if (!fSessOnline || hProcessDlg ||
      (wSessKeyboard != KEYBD_LOCAL && nCommand != COMMAND_STOP))
    {
    MessageBeep(NULL);
    return;
    }

  if (sAiState > STATE_NULL)
    sAiState = STATE_NULL;

  CommWriteChar('\x02');
  CommWriteData(Commands[nCommand].szCommand, -1);
  CommWriteChar('\r');

  InfoMessage(IDS_INFO_PROCESS, Commands[nCommand].szName);
  wSessKeyboard = KEYBD_LOCKED;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessStrCommand(int nCommand)					      */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessStrCommand(int nCommand)
  {
  char szBuf[256];


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  CommWriteChar('\x02');
  CommWriteData(Commands[nCommand].szCommand, -1);
  CommWriteChar(' ');

  TermQueryInput(szBuf);
  CommWriteData(szBuf, -1);

  CommWriteChar('\r');

  InfoMessage(IDS_INFO_PROCESS, Commands[nCommand].szName);

  wSessKeyboard = KEYBD_LOCKED;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessIntCommand(int nCommand, int nArgument)                          */
/*									      */
/*  Called the application's initialize routine so we can get everything in   */
/*  order.  Returns true is successful (for future possiblities).             */
/*									      */
/*----------------------------------------------------------------------------*/
void SessIntCommand(int nCommand, int nArgument)
{
  char szNum[16];


  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  CommWriteChar('\x02');
  CommWriteData(Commands[nCommand].szCommand, -1);

  if (Commands[nCommand].fNumberParam)
    {
    if (nArgument == -1)
      nArgument = TermQueryArgument();
    if (nArgument >= 1)
      {
      if (nCommand == COMMAND_DISPDIF && nArgument > 255)
        nArgument = 255;
      if (nArgument > 999)
        nArgument = 999;

      itoa(nArgument, szNum, 10);
      CommWriteData(szNum, -1);
      }
    }

  CommWriteChar('\r');

  if (nArgument > 0)
    {
    char buf[64];
    register i;

    strcpy(buf, Commands[nCommand].szName);
    i = strlen(buf);
    buf[i++] = ' ';
    strcpy(&buf[i], szNum);
    InfoMessage(IDS_INFO_PROCESS, buf);
    }
  else
    InfoMessage(IDS_INFO_PROCESS, Commands[nCommand].szName);
  wSessKeyboard = KEYBD_LOCKED;
}

void SessArgCommand(int nCommand, char *szOption, int nNumber)
{
  if (!fSessOnline || wSessKeyboard != KEYBD_LOCAL)
    {
    MessageBeep(NULL);
    return;
    }

  CommWriteChar('\x02');
  CommWriteData(Commands[nCommand].szCommand, -1);

  if (*szOption)
    {
    if (nCommand != COMMAND_AUTOCITE && nCommand != COMMAND_SHEPARD &&
        nCommand != COMMAND_LXSTAT && nCommand != COMMAND_LEXSEE)
      CommWriteChar(';');
    else
      CommWriteChar(' ');

    CommWriteData(szOption, -1);
    }

  if (nNumber > 1)
    {
    char buf[32];

    itoa(nNumber, buf, 10);
    CommWriteData(buf, -1);
    }

  CommWriteChar('\r');

  InfoMessage(IDS_INFO_PROCESS, Commands[nCommand].szName);
  wSessKeyboard = KEYBD_LOCKED;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessOffline()                                                        */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SessionOffline()
  {
  fSessOnline = FALSE;
  EnableMenuItem(hWndMenu, CMD_NEWSEARCH,     MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SEARCHMODIFY,  MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SEARCHLEVEL,   MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_LIBRARY,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_FILE,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_CLIENT,        MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_ECLIPSE,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_ECLIPSERECALL, MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SEARCHDISPLAY, MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_STOP,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_NEXTPAGE,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_PREVPAGE,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_NEXTDOC,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_PREVDOC,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_GOTOPAGE,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_GOTODOC,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_FIRSTPAGE,     MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_FIRSTDOC,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_KWIC,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_FULL,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_FULL,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_CITE,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_VARKWIC,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SEGMENTS,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SEQUENCE,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_ENTERSTAR,     MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_EXITSTAR,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_AUTOCITE,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SHEPARD,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_LEXSTAT,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_LEXSEE,        MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_BROWSE,        MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_RESUME,        MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_SELECTSERVICE, MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_EXITSERVICE,   MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_PRINTSCREEN,   MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_PRINTDOC,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_PRINTALL,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_TIME,          MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_REQUEST,       MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_PAGES,         MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_DEBUT,         MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_TUTORIAL,      MF_GRAYED);
  EnableMenuItem(hWndMenu, CMD_LEXISHELP,     MF_GRAYED);

  CheckMenuItem(hWndMenu, CMD_HYPERROM, MF_UNCHECKED);
  EnableMenuItem(hWndMenu, CMD_HYPERROM,      MF_GRAYED);

  AIscanReset();
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void SessOnline()                                                         */
/*									      */
/*----------------------------------------------------------------------------*/
static void near SessionOnline()
  {
  fSessOnline = TRUE;
  EnableMenuItem(hWndMenu, CMD_NEWSEARCH,     MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SEARCHMODIFY,  MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SEARCHLEVEL,   MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_LIBRARY,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_FILE,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_CLIENT,        MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_ECLIPSE,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_ECLIPSERECALL, MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SEARCHDISPLAY, MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_STOP,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_NEXTPAGE,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_PREVPAGE,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_NEXTDOC,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_PREVDOC,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_GOTOPAGE,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_GOTODOC,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_FIRSTPAGE,     MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_FIRSTDOC,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_KWIC,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_FULL,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_FULL,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_CITE,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_VARKWIC,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SEGMENTS,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SEQUENCE,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_ENTERSTAR,     MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_EXITSTAR,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_AUTOCITE,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SHEPARD,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_LEXSTAT,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_LEXSEE,        MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_BROWSE,        MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_RESUME,        MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_SELECTSERVICE, MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_EXITSERVICE,   MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_PRINTSCREEN,   MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_PRINTDOC,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_PRINTALL,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_TIME,          MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_REQUEST,       MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_PAGES,         MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_DEBUT,         MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_TUTORIAL,      MF_ENABLED);
  EnableMenuItem(hWndMenu, CMD_LEXISHELP,     MF_ENABLED);
  }
