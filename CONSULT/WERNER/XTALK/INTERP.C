/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (c) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Interpreter (INTERP.C)                                          *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module is the key for running the scripts.                 *
*                                                                             *
*  Revisions:																   *
*	1.00 07/17/89 Initial Version.											   *
*	1.01 08/15/89 00018 Flash the macro keys while running a script.		   *
*	1.01 08/15/89 00019 Only compile script once from CHAIN.				   *
*	1.01 09/08/89 00034 Improve error check for external routines.			   *
*	1.01 09/28/89 00041 Request command requires an argument.				   *
*	1.01 11/23/89 00063 Add disconnect error checking to NEXTLINE.			   *
*	1.01 11/23/89 00064 Input all characters in NEXTLINE.					   *
*	1.01 11/25/89 00070 Improve NEXTLINE support.							   *
*	1.01 01/19/90 00090 Check for cTabWidth being equal to zero.			   *
*	1.01 01/26/90 dca00006 Check for max characters in Nextline.			   *
*	1.01 02/05/90 dca00020 wait "time" does not need to be online,			   *
*                         but wait time "for" does ...JDB.					   *
*	1.1  03/01/90 dca00034 MKL added one parameter to VidClearPage			   *
*						   routine to support VT102 screen alignment cap.	   *
*	1.1  04/03/90 dca00065 PJL Change "UNKNOWNUNIT" to "BADDRIVE".			   *
*		 04/03/90 dca00066 JDB Added DDE Advise.							   *
*	1.1  04/07/90 dca00072 PJL Fixed REPLY '~'/REPLY '`~' problem.			   *
*	1.1  04/08/90 dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.  *
*                                                                             *
\*****************************************************************************/

#define NOATOM			/* Atom Manager routines */
#define NOBITMAP		/* typedef HBITMAP and associated routines */
#define NOBRUSH 		/* typedef HBRUSH and associated routines */
#define NOCLIPBOARD		/* clipboard routines */
#define NOCOLOR 		/* CTLCOLOR_*, COLOR_* */
#define NOCREATESTRUCT		/* typedef CREATESTRUCT */
#define NOCTLMGR		/* control and dialog routines */
#define NODRAWTEXT		/* DrawText() and DT_* */
#define NOFONT			/* typedef FONT and associated routines */
#define NOGDI			/* StretchBlt modes and gdi logical objects */
#define NOGDICAPMASKS		/* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOHDC			/* typedef HDC and associated routines */
#define NOICON			/* IDI_* */
#define NOKANJI 		/* Kanji support stuff. */
#define NOMENUS 		/* MF_*, HMENU and associated routines */
#define NOMETAFILE		/* typedef METAFILEPICT */
#define NOMINMAX		/* Macros min(a,b) and max(a,b) */
#define NONCMESSAGES		/* WM_NC* and HT* */
#define NOPEN			/* typedef HPEN and associated routines */
#define NORASTEROPS		/* binary and tertiary raster ops */
#define NOREGION		/* typedef HRGN and associated routines */
#define NORESOURCE		/* Predefined resource types:  RT_* */
#define NOSCROLL		/* SB_* and scrolling routines */
#define NOSOUND 		/* Sound driver routines */
#define NOSYSCOMMANDS		/* SC_* */
#define NOSYSMETRICS		/* SM_*, GetSystemMetrics */
#define NOTEXTMETRIC		/* typedef TEXTMETRIC and associated routines */
#define NOWH			/* SetWindowsHook and WH_* */
#define NOWINMESSAGES		/* WM_* */
#define NOWINOFFSETS		/* GWL_*, GCL_*, associated routines */
#define NOWINSTYLES		/* WS_*, CS_*, ES_*, LBS_* */
#define NOWNDCLASS		/* typedef WNDCLASS and associated routines */

#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "interp.h"
#include "compile.h"
#include "variable.h"
#include "capture.h"
#include "macro.h"
#include "comm.h"
#include "global.h"
#include "sess.h"
#include "term.h"
#include "xfer.h"
#include "kermit.h"
#include "prodata.h"
#include "properf.h"

  extern int  nPrintX;

#define ST_NORMAL	0x01
#define ST_WATCH	0x02
#define ST_WAIT 	0x03
#define ST_INPUT	0x04
#define ST_NEXT 	0x05

#define OpCode(x)	(*(WORD far *)(&lpObjectCode[x]))

  static int  nState;
	 WORD wAddr;
	 WORD wLine;
  static char fTrap;
  static char fTrace;

  static WORD wEolAddr;
  static WORD wEolLine;

	 int  nStack;
         WORD wStackType[64];
         WORD wStackAddr[64];
         WORD wStackLine[64];
	 int  nGosubInx;
	 WORD wGosubAddr[8];
	 WORD wGosubLine[8];

	 int  nStackAddr;
	 int  nStackHigh;

  static int  nInputVar;
  static int  nInputInx;
  static int  nInputStr;
  static long lNextTimeout;
  static int  nNextMax;

	 int  nKeyBuffer[8];
	 BYTE bKeyIn, bKeyOut;


  static char fWatchWait;
  static int  nWatchStack;
  static char fWatchComm;
  static long lWatchQuiet;
  static WORD wWatchQuietAddr;
  static WORD wWatchQuietLine;
  static WORD wWatchKey;
  static WORD wWatchKeyAddr;
  static WORD wWatchKeyLine;
  static long lWatchCount;
  static WORD wWatchCountAddr;
  static WORD wWatchCountLine;
  static long lWatchTime;
  static int  fWaitOL;	//JDB dca00020 Wait Online Flag.


  static void near StateNormal();
  static void near StateWatch();
  static void near WatchPrep();
  static void near WaitPrep();
  static void near GotoEndOfStatement();
  static void near GotoNextEOLL();
  static void near GotoNextEOPL();
  static int  near GotoNextWend();
  static int  near GotoCleanEOL();
  static int  near PopLocation(WORD);
  static int  near PushLocation(WORD);
  static void near SkipVariable();
  static int  near SkipComma();
  static WORD near XlatKey(WORD);
  static void near TraceLine();

  void dumpdata(BYTE *, int);
  void dprint(BYTE *);

extern void remove_all_advise();	/* dca00066 JDB DDE Advise */

int InterpreterStart(szLabel)
  char *szLabel;
{
  char szExec[5];
  register int i;

  if (hObjectCode == 0)
    return (TRUE);

  // dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.
  if (SetFullPath(VAR_DIRFIL))
  	 return(FALSE);

  fHostScript = FALSE;
  fHostScript = FALSE;
  bDisplayOn  = TRUE;
  nWorkChoice = 0;

  nState = ST_NORMAL;
  wAddr  = 0;
  wLine  = 1;
  fTrap  = 0;
  fTrace = 0;

  wEolLine = 0;

  nStack    = 0;
  nGosubInx = 0;

  nStackAddr = 0;
  nStackHigh = sizeof(filexfer.stack.data)-1;

  bKeyIn  = 0;
  bKeyOut = 0;

  if (szLabel && *szLabel)
    {
    if (GotoLabel(szLabel))
      return (ERRINT_NOLABEL);
    }

  SetSystemStatus(SS_SCRIPT);

  SetScrollLock(hWnd, FALSE);

  szExec[0] = 'E';
  szExec[1] = 'X';
  szExec[2] = 'E';
  szExec[3] = 'C';
  szExec[4] = NULL;
  IconTextChanged(szExec);

  for (i = 0; i < 8; i++)
    fFileFlags[i] = 0;
  cDefInputFile  = 1;
  cDefOutputFile = 1;

  nErrorCode = 0;
  return (FALSE);
}

void InterpreterStop()
  {
  FileClose(-1);

  if (hObjectCode)
    {
    GlobalUnlock(hObjectCode);
    GlobalFree(hObjectCode);
    hObjectCode = 0;
    }

  ScriptEndDDE();
  PurgeVariableLevel(LEVEL_LOCAL);
  if (!IfSystemStatus(SS_DDE))
    PurgeVariableLevel(LEVEL_GLOBAL);
  ResetSystemStatus(SS_SCRIPT);
  IconTextChanged(NULL);
  InfoMessage(NULL);

  LoadGlobalData();
  LoadModemParameters();

  remove_all_advise();		/* dca00066 JDB DDE Advise */
  }

void InterpreterContinue()
{
  char buf[8];
  static char fInside;
  register i;

  if (hObjectCode == 0)
    return;

  if (fInside)
    return;
  fInside = TRUE;

  switch (nState)
    {
    case ST_WATCH:
      StateWatch();
      break;

    case ST_NORMAL:
      StateNormal();
      break;

    case ST_WAIT:
      if (nErrorCode != -1)
	nState = ST_NORMAL;
      break;

    case ST_NEXT:
      // 00063 Add disconnect error checking to NEXTLINE.
      if (nErrorCode)
	{
	nState = ST_NORMAL;
	fTimeout = TRUE;
	break;
	}
      else if (lNextTimeout != 0L)
	{
	if (GetCurrentTime() >= lNextTimeout)
	  {
          nErrorCode = SetStrVariable(nInputVar, filexfer.stack.data, nInputInx);
	  nState = ST_NORMAL;
	  fTimeout = TRUE;
	  break;
	  }
	}
      // JDB dca00006 while ((i = CommGet()) != -1)
      // Fixed the max characters in nextline
      while ( (nInputInx < nNextMax) && ((i = CommGet()) != -1) )
	{
	if ((i & 0x7F) == ASC_CR)
	  {
          nErrorCode = SetStrVariable(nInputVar, filexfer.stack.data, nInputInx);
	  nState = ST_NORMAL;
	  fTimeout = FALSE;
          if (fHostScript)
	    {
	    buf[0] = ASC_CR;
	    buf[1] = ASC_LF;
	    CommDataOutput(buf, 2);
            }
          if (bDisplayOn)
            TermWriteNewLine();
	  break;
	  }
	else if (((i & 0x7F) == ASC_BS || (i & 0x7F) == ASC_DEL) && fHostScript)
	  {
	  if (nInputInx == 0)
	    {
            if (fHostScript)
	      {
	      buf[0] = ASC_BEL;
	      CommDataOutput(buf, 1);
	      }
            if (bDisplayOn)
              TermWriteByte(ASC_BEL);
	    }
	  else
	    {
	    buf[0] = (char)(i & 0x7F);
            buf[1] = ' ';
	    buf[2] = (char)(i & 0x7F);
            if (fHostScript)
	      CommDataOutput(buf, 3);
            if (bDisplayOn)
              TermWriteData(buf, 3);
	    nInputInx--;
	    }
          }
	else if ((i & 0x7F) != ASC_LF)
	  {
	  // 00064 input all characters.
	  filexfer.stack.data[nInputInx++] = (BYTE)i;

	  // Only echo printable characters while in HostScript mode.
	  if (fHostScript && i >= ' ')
	    {
	    buf[0] = (BYTE)i;
	    CommDataOutput(buf, 1);
	    }
	  // Only echo printable characters while the Display variable is true.
	  if (bDisplayOn && i >= ' ')
	    TermWriteByte(i);
	  if (nInputInx >= nNextMax)
	    {
	    nErrorCode = SetStrVariable(nInputVar, filexfer.stack.data, nInputInx);
	    nState = ST_NORMAL;
	    fTimeout = FALSE;
	    }
	  }
	}
      break;
    }

  fInside = FALSE;
}


/******************************************************************************/
/*									      */
/*  InterpreterKeyboard(WORD wKey, char fSrc);                                */
/*									      */
/*  This routine is called from two main places, the virtual keyboard logic   */
/*  for the terminal emulator (fSrc == FALSE), and within each terminal       */
/*  emulator (fSrc == TRUE).  If (FSrc == FALSE), then wKey is a VIRTUAL      */
/*  keystroke, else it is the ascii letter.  We return true or false if we    */
/*  process the key (true we did).  Thus any key that we do not use will be   */
/*  forwarded to the terminal emulation logic (scroll-lock for example).      */
/*									      */
/******************************************************************************/
int InterpreterKeyboard(wKey, fSrc)
  WORD wKey;
  char fSrc;
{
  register i;


  /*
  ** If the key is virtual, then lets uses some of them (change wKey) and
  ** dump the others (return false).
  */
  if (fSrc == 0)
    {
    if (GetKeyState(VK_CONTROL) < 0)
      {
      if (wKey >= 'A' && wKey <= 'P')
        FlashMacro(wKey - 'A' + 24);
      else if (wKey == 'R')
        FlashMacro(40);
      else if (wKey >= 'T' && wKey <= 'Z')
        FlashMacro(wKey - 'T' + 41);
      }

    switch (wKey)
      {
      case VK_BACK:
        wKey = ASC_BS;
        break;

      case VK_RETURN:
        wKey = ASC_CR;
        break;

      case VK_UP:
        wKey = 0x0501;
        break;

      case VK_DOWN:
        wKey = 0x0502;
        break;

      case VK_LEFT:
        wKey = 0x0503;
        break;

      case VK_RIGHT:
        wKey = 0x0504;
        break;

      case VK_HOME:
        wKey = 0x0505;
        break;

      case VK_END:
        wKey = 0x0506;
        break;

      case VK_PRIOR:
        wKey = 0x0507;
        break;

      case VK_NEXT:
        wKey = 0x0508;
        break;

      case VK_INSERT:
        wKey = 0x0511;
        break;

      case VK_DELETE:
        wKey = 0x0512;
        break;

      case VK_F1:
      case VK_F2:
      case VK_F3:
      case VK_F4:
      case VK_F5:
      case VK_F6:
      case VK_F7:
      case VK_F8:
      case VK_F9:
      case VK_F10:
      case VK_F11:
      case VK_F12:
      case VK_F13:
      case VK_F14:
      case VK_F15:
      case VK_F16:
        wKey = 0x0401 + (wKey - VK_F1);
        if (GetKeyState(VK_SHIFT) < 0)
          {
          FlashMacro(wKey - 0x401); // 00018 Flash the macro key, if shown.
          wKey += 16;
          }
        if (GetKeyState(VK_CONTROL) < 0)
          {
          FlashMacro(wKey - 0x401 + 12); // 00018 Flash the macro key.
          wKey += 32;
          }
        break;

      default:
        return (FALSE);
        break;
      }
    }


  if (nState == ST_WATCH)
    {
    if (wWatchKey == 0 || wWatchKey == wKey)
      {
      if (fWatchWait)
        {
        wAddr = wWatchKeyAddr;
        wLine = wWatchKeyLine;
        }
      nState = ST_NORMAL;
      return (TRUE);
      }
    return (FALSE);
    }
  else if (nState == ST_INPUT)
    {
    if (wKey == ASC_CR)
      {
      if (nInputStr)
        {
        nErrorCode = SetStrVariable(nInputVar, filexfer.stack.data, nInputInx);
        }
      else
        {
        long lVariable;

        filexfer.stack.data[nInputInx] = NULL;
        lVariable = atol(filexfer.stack.data);
        nErrorCode = SetIntVariable(nInputVar, lVariable);
        }

      nState = ST_NORMAL;
      if (bDisplayOn)
	{
	CharToTerminal(ASC_CR);
	if (!Term.fNewLine)
	  CharToTerminal(ASC_LF);
	}
      }
    else if (wKey == ASC_BS)
      {
      if (nInputInx == 0)
	MessageBeep(0);
      else
        {
        if (bDisplayOn)
          {
          CharToTerminal(ASC_BS);
          CharToTerminal(' ');
          CharToTerminal(ASC_BS);
          }
	nInputInx--;
	}
      }
    else
      {
      if (nTermChrX >= Term.nColumns || nInputInx >= 132)
	MessageBeep(0);
      else
        {
        if (nInputStr || (wKey >= '0' && wKey <= '9'))
          {
          filexfer.stack.data[nInputInx++] = (BYTE)wKey;
          if (bDisplayOn)
            CharToTerminal((BYTE)wKey);
          }
        else
          MessageBeep(NULL);
	}
      }
    }
  else
    {
    i = bKeyIn + 1;
    if (i >= 8)
      i = 0;

    if (i != bKeyOut)
      {
      nKeyBuffer[bKeyIn] = wKey;
      bKeyIn = (BYTE)i;
      }
    else
      MessageBeep(NULL);
    }

  return (TRUE);
}

static void near StateNormal()
{
  WORD wToken, wSaveAddr;
  long lInteger;
  BYTE *ptr;
  BYTE byBuf[256], byLen;
  BYTE byBuf2[256], byLen2;
  char fFlag;
  int  nLoop, nInteger;
  register i;


  // 00034  Improve error check for external routines.
  if (fTrap == 0 && nErrorCode && (nErrorCode != -1))
    {
    InterpreterError(nErrorCode);
    return;
    }
  else
    {
    if (nErrorCode && nErrorCode != -1)
      {
      nErrorFlag = TRUE;
      nErrorSave = nErrorCode;
      nErrorCode = 0;
      }
    }

  nLoop = 32;
  while (nLoop > 0 && nState == ST_NORMAL)
    {
    if (nStackAddr + 512 >= nStackHigh)
      {
      InterpreterError(ERRINT_INTERNAL);
      return;
      }

    wToken = OpCode(wAddr);
    wAddr += 2;
    i = 0;
    switch (wToken)
      {
      case TK_ADD:
	while (!IfEndOfLine())
	  {
	  wSaveAddr = wAddr;
	  if (i = EvalString(byBuf, &byLen))
	    {
	    if (i != ERRINT_MIXTYPES || byLen != 0)
	      break;
	    wAddr = wSaveAddr;
	    if (i = EvalInteger(&lInteger))
	      break;
	    ltoa(lInteger, byBuf, 10);
	    byLen = (BYTE)strlen(byBuf);
	    }
          if (byLen)
            WriteRawToCapture(byBuf, byLen);

	  if (OpCode(wAddr) == TK_SEMI)
	    {
	    wAddr += 2;
	    }
	  else if (OpCode(wAddr) == TK_COMMA)
            {
	    wAddr += 2;
	    }
	  else
            {
            byBuf[0] = '\r';
            byBuf[1] = '\n';
            WriteRawToCapture(byBuf, 2);
	    break;
	    }
	  }
	break;

      case TK_ALARM:
	MessageBeep(0);
        nLoop = 0;
	break;

      case TK_ACTIVATE:
	ShowWindow(hWnd, SW_SHOW);
	SetFocus(hWnd);
        nLoop = 0;
	break;

      case TK_ALERT:
        i = AlertInterpret();
        nLoop = 0;
        break;

      case TK_BYE:
	CommPortClose();
        nLoop = 0;
	break;

      case TK_CALL:
        if (bConnected)
          CommPortClose();

	if (IfEndOfLine())
	  CmdDial();
	else
	  {
	  if (i = EvalString(byBuf, &byLen))
	    break;
	  DataOpen(byBuf);
	  if (nErrorCode == 0)
	    CmdConnect();
          }
        nLoop = 0;
	break;

      case TK_CAPTURE:
	if (OpCode(wAddr) == TK_ON)
	  {
	  if (fCapturePause)
	    fCapturePause = FALSE;
	  else
            CmdCapture(0, 0);
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_PAUSE)
	  {
	  fCapturePause = TRUE;
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_OFF)
	  {
	  if (bCaptureOn)
            CmdCapture(0, 0);
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_TOGGLE)
	  {
	  if (fCapturePause)
	    fCapturePause = FALSE;
	  else if (bCaptureOn)
	    fCapturePause = TRUE;
	  else
            CmdCapture(0, 0);
	  wAddr += 2;
	  }
	else
	  {
	  char fNew;

	  if (OpCode(wAddr) == TK_NEW)
	    {
	    fNew = TRUE;
	    wAddr += 2;
	    }
	  else
	    fNew = FALSE;

          if (EvalString(byBuf, &byLen))
            {
            i = ERRMIS_NOFILE;
            break;
            }

	  if (bCaptureOn)
            CmdCapture(NULL, fNew);
          i = CmdCapture(byBuf, fNew);
	  }
	break;

      case TK_CHAIN:
        {
        char szFile[256], *ptr;

        if (i = EvalString(byBuf, &byLen))
          break;

        strcpy(szFile, byBuf);
        if (ptr = strchr(szFile, '('))
          *ptr = NULL;
        if (ptr = strchr(szFile, ' '))
          *ptr = NULL;

        if (NeedCompile(szFile))
          {
          HANDLE hSaveObject;
          LPSTR  lpSaveObject;
          WORD wSaveAddr, wSaveLine;

          wSaveAddr = wAddr;
          wSaveLine = wLine;
          hSaveObject  = hObjectCode;
          lpSaveObject = lpObjectCode;
          hObjectCode = NULL;

          if ((i = CompileScript(szFile)) == 0) // 00019 Only compile once.
            i = SaveScriptObject(szFile);

          wAddr = wSaveAddr;
          wLine = wSaveLine;
          hObjectCode  = hSaveObject;
          lpObjectCode = lpSaveObject;
          }

        PurgeVariableLevel(LEVEL_LOCAL);
        if (i = ExecuteScript(byBuf))
          break;

        wToken = TK_EOLL;
        nLoop = 0;
        }
        break;

      case TK_CHDIR:
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (chdir(byBuf))
	  i = ERRDOS_NOPATH;
	break;

      case TK_CHMOD:
	if (i = EvalString(byBuf, &byLen))
          break;
        if (OpCode(wAddr) != TK_COMMA)
          {
          lInteger = 0;
          }
        else
          {
          wAddr += 2;
          if (i = EvalInteger(&lInteger))
            break;
          }
        if (_dos_setfileattr(byBuf, (unsigned)lInteger))
	  i = ERRDOS_NOFILE;
	break;

      case TK_CLEAR:
	if (OpCode(wAddr) == TK_WINDOW)
	  {
	  // dca00034 MKL added one parameter to VidClearPage
	  VidClearPage(0X7020);
	  wAddr += 2;
	  break;
	  }
	else if (OpCode(wAddr) == TK_LINE)
	  {
	  VidClearArea(0, nTermChrY, TERM_MAXCOL, nTermChrY);
	  wAddr += 2;
	  break;
	  }
	else if (OpCode(wAddr) == TK_EOW)
	  {
	  VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL, TERM_MAXROW);
	  wAddr += 2;
	  break;
	  }
	else if (OpCode(wAddr) == TK_BOW)
	  {
	  VidClearArea(1, 0, nTermChrX, nTermChrY);
	  wAddr += 2;
	  break;
	  }
	else if (OpCode(wAddr) == TK_EOL)
	  {
	  VidClearArea(nTermChrX, nTermChrY, TERM_MAXCOL, nTermChrY);
	  wAddr += 2;
	  break;
	  }
	else if (OpCode(wAddr) == TK_BOL)
	  {
	  VidClearArea(1, nTermChrY, nTermChrX, nTermChrY);
	  wAddr += 2;
	  break;
	  }
	else if (!IfEndOfLine())
	  {
	  i = ERRINT_BADKEYWORD;
	  }

      case TK_CLS:
        TermWriteClear();
	break;

      case TK_CLOSE:
	{
	int  nChan;

	if (IfEndOfLine())
	  {
	  nChan = -1;
	  }
	else
	  {
	  if (i = EvalChannel(&nChan, 0))
	    break;
	  }
	FileClose(nChan);
	}
	break;

      case TK_CMODE:
	if (OpCode(wAddr) == TK_NORMAL)
	  {
	  Sess.cLogOption = SESSION_NORMAL;
	  }
	else if (OpCode(wAddr) == TK_RAW)
	  {
	  Sess.cLogOption = SESSION_RAW;
	  }
	else if (OpCode(wAddr) == TK_VISUAL)
	  {
	  Sess.cLogOption = SESSION_VISUAL;
	  }
	else if (OpCode(wAddr) == TK_MANUAL)
	  {
	  Sess.cLogOption = SESSION_MANUAL;
	  }
	else
	  {
	  i = ERRINT_BADKEYWORD;
	  }
	wAddr += 2;
	break;

      case TK_COMPILE:
         {
         HANDLE hSaveObject;
         LPSTR  lpSaveObject;
         WORD wSaveAddr, wSaveLine;

	 if (i = EvalString(byBuf, &byLen))
           break;
         wSaveAddr = wAddr;
         wSaveLine = wLine;
         hSaveObject  = hObjectCode;
         lpSaveObject = lpObjectCode;
         hObjectCode = NULL;

        if (i = CompileScript(byBuf))
          i = SaveScriptObject(byBuf);

         wAddr = wSaveAddr;
         wLine = wSaveLine;
         hObjectCode  = hSaveObject;
         lpObjectCode = lpSaveObject;

         wToken = TK_EOLL;
         }
	 break;

      case TK_CWAIT:
	if (OpCode(wAddr) == TK_NONE)
	  {
	  Xfer.cCWait = XFER_WAIT_NONE;
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_ECHO)
	  {
	  Xfer.cCWait = XFER_WAIT_ECHO;
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_DELAY)
	  {
	  wAddr += 2;
          if (i = EvalRange(&nInteger, 0, 9999, NULL))
            break;
          if (OpCode(wAddr) == TK_MSEC)
            {
            wAddr += 2;
            if (nInteger <= 32)
              nInteger *= 1000;
            else
              nInteger = -1;
            }
          if (nInteger < 0 || nInteger > 9999)
            {
            i = ERRATH_OUTOFRANGE;
            break;
            }
	  Xfer.cCWait = XFER_WAIT_DELAY;
          Xfer.nCWaitDelay = (int)nInteger;
	  }
	else
	  {
	  i = ERRINT_BADKEYWORD;
	  }
	break;

      case TK_DDEEXECUTE:
	if (i = EvalInteger(&lInteger))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalString(byBuf, &byLen))
	  break;
	if ((i = ExecuteDDE((int)lInteger, byBuf)) == 0 && nErrorCode == 0)
	  {
	  nState = ST_WAIT;
	  nErrorCode = -1;
	  }
	break;

      case TK_DDEINITIATE:
	{
	int  nChannel, nVarNo;

	if (i = EvalIntVariable(&nVarNo, FW_COMMA))
	  break;
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalString(byBuf2, &byLen2))
	  break;
	nChannel = InitiateDDE(byBuf, byBuf2);
	SetIntVariable(nVarNo, (long)nChannel);
	}
	break;

      case TK_DDEPOKE:
	if (i = EvalInteger(&lInteger))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalString(byBuf2, &byLen2))
	  break;
        if ((i = PokeDDE((int)lInteger, byBuf, byBuf2)) == 0)
	  {
	  nState = ST_WAIT;
	  nErrorCode = -1;
	  }
	break;

      case TK_DDEREQUEST:
	{
	int  nVarNo;

	if (i = EvalInteger(&lInteger))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalStrVariable(&nVarNo, 0))
	  break;
	if ((i = RequestDDE((int)lInteger, byBuf, nVarNo)) == 0 && nErrorCode == 0)
	  {
	  nState = ST_WAIT;
	  nErrorCode = -1;
	  }
	}
	break;

      case TK_DDETERMINATE:
	if (i = EvalInteger(&lInteger))
	  break;
	i = TerminateDDE((int)lInteger);
	break;

      case TK_DEFINPUT:
      case TK_DEFOUTPUT:
	if (OpCode(wAddr) == TK_EQ)
	  {
	  wAddr += 2;
	  if (i = EvalInteger(&lInteger))
	    break;

	  PutSysVariableAny(wToken, 0, 0, lInteger);
	  }
	else
	  {
	  i = ERRINT_MISEQ;
	  }
	break;

      case TK_DELETE:
	 if (i = EvalString(byBuf, &byLen))
	   break;
	 if (DosDelete(byBuf, 0L))
	   i = ERRIOE_NOCANFINDFILE;
	 break;

      case TK_DIAL:
	if (IfEndOfLine())
          {
          if (Sess.szNumber[0] == 0)
            {
            nErrorCode = ERRDIA_NONUMBER;
            break;
            }
          strcpy(szManualNumber, &Sess.szNumber[1]);
          }
        else
          {
          if (i = EvalString(byBuf, &byLen))
            break;
          strcpy(szManualNumber, byBuf);
          }
        if (!bConnected)
          if (nErrorCode = CommPortOpen())
            break;
        DialNumber();
        nLoop = 0;
        break;

      case TK_DIALOGBOX:
        i = DlgboxInterpret();
        break;

      case TK_GROUP:
      case TK_TABSTOP:
      case TK_LTEXT:
      case TK_RTEXT:
      case TK_CTEXT:
      case TK_CHECKBOX:
      case TK_PUSHBUTTON:
      case TK_LISTBOX:
      case TK_GROUPBOX:
      case TK_DEFPUSHBUTTON:
      case TK_EDITTEXT:
      case TK_ENDDIALOG:
        i = ERRINT_NOTINDIALOG;
        break;

      case TK_DRIVE:
	{
	int  nDrive, nMaxDrive;

	if (i = EvalString(byBuf, &byLen))
          break;
        *byBuf = toupper(*byBuf);
	_dos_setdrive(byBuf[0] - '@', &nMaxDrive);
	_dos_getdrive(&nDrive);
	if (nDrive != byBuf[0] - '@')
	  i = ERRSYS_BADDRIVE;
	}
	break;

      case TK_END:
      case TK_EOFM:
      case TK_HALT:
	InterpreterStop();
	break;

      case TK_ENDWATCH:
	i = ERRINT_INTERNAL;
	break;

      case TK_ELSE:
	wAddr = OpCode(wAddr+0);
	wLine = OpCode(wAddr+2);

	if (wAddr == 0 || wLine == 0)
	  i = ERRINT_INTERNAL;
	wToken = TK_EOLL;
	break;

      case TK_EOLL:
	nLoop--;
	break;

      case TK_EOVL:
	wLine++;
	nLoop--;
	if (fTrace)
	  TraceLine();
	break;

      case TK_EOPL:
	if (wEolLine)
	  {
	  wAddr = wEolAddr;
	  wLine = wEolLine;
	  wEolLine = 0;
	  }
	else
	  {
	  wLine++;
	  }
	nLoop--;
	if (fTrace)
	  TraceLine();
	break;

      case TK_FKEY:
	{
	int nKey;

        if (i = EvalRange(&nKey, 1, 48, FW_COMMA))
	  break;
	nKey--;
        if (i = EvalString(&byBuf[1], &byLen))
	  break;
	if (byLen >= sizeof(Macro.szMacro[0]))
	  {
	  i = ERRINT_LONGSTRING;
	  break;
          }
        byBuf[0] = byLen;
        LibEnstore(byBuf, Macro.szMacro[nKey], sizeof(Macro.szMacro[0]));
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalString(byBuf, &byLen))
	    break;
	  if (byLen >= sizeof(Macro.szLabel[0]))
	    {
	    i = ERRINT_LONGSTRING;
	    break;
	    }
	  strcpy(Macro.szLabel[nKey], byBuf);
	  if (OpCode(wAddr) == TK_COMMA)
	    {
	    wAddr += 2;
	    if (i = EvalBoolean(&fFlag))
	      break;
	    Macro.fMenu[nKey] = fFlag;
	    }
          }

        UpdateMacro();
	}
	break;

      case TK_GET:
	{
	int nChannel, nLength, nVarNo;
        WORD wSaveAddr, wSaveLine;

        wSaveAddr = wAddr;
        wSaveLine = wLine;

        if (EvalRange(&nLength, 1, 250, FW_COMMA) == 0)
          {
          if (EvalStrVariable(&nVarNo, FW_EOL) == 0)
            {
            i = FileGetRec((int)cDefInputFile-1, nLength, nVarNo);
            break;
            }
          }

        wAddr = wSaveAddr;
        wLine = wSaveLine;

	if (i = EvalChannel(&nChannel, FW_COMMA))
	  break;
	if (i = EvalRange(&nLength, 1, 250, FW_COMMA))
	  break;
	if (i = EvalStrVariable(&nVarNo, FW_EOL))
	  break;
	i = FileGetRec(nChannel, nLength, nVarNo);
	}
	break;

      case TK_GO:
        nErrorCode = CommPortOpen();
        nLoop = 0;
	break;

      case TK_GOSUB:
	if (nGosubInx >= sizeof(wGosubAddr)/sizeof(WORD))
	  {
	  i = ERRINT_TOOMANYGOSUBS;
	  break;
	  }

	if (OpCode(wAddr) == TK_AMP)
	  {
	  wAddr += 2;
	  if (i = EvalString(byBuf, &byLen))
	    break;
	  }
	else if (OpCode(wAddr) != TK_VARIABLE)
	  {
	  i = ERRINT_BADLABEL;
	  break;
	  }
	else
	  {
	  lstrcpy(byBuf, &lpObjectCode[wAddr+2]);
	  SkipVariable();
	  }
        GotoEndOfStatement();
	wGosubAddr[nGosubInx] = wAddr;
	wGosubLine[nGosubInx] = wLine;
	nGosubInx++;
	if (GotoLabel(byBuf))
	  {
	  i = ERRINT_NOLABEL;
	  break;
	  }
	wEolLine = 0;
	wToken = TK_EOLL;
	break;

      case TK_GOTO:
	if (OpCode(wAddr) == TK_AMP)
	  {
	  wAddr += 2;
	  if (i = EvalString(byBuf, &byLen))
            break;
          strupr(byBuf);
	  }
	else if (OpCode(wAddr) != TK_VARIABLE)
	  {
	  i = ERRINT_BADLABEL;
	  break;
	  }
	else
	  {
	  lstrcpy(byBuf, &lpObjectCode[wAddr+2]);
	  SkipVariable();
	  }
	if (GotoLabel(byBuf))
	  {
	  i = ERRINT_NOLABEL;
	  break;
	  }
	wEolLine = 0;
	wToken = TK_EOLL;
	break;

      case TK_GRAB:
        if (bCaptureOn)
	  CmdCopyCapture();
	break;

      case TK_HIDE:
	ShowWindow(hWnd, SW_HIDE);
	break;

      case TK_IF:
	wSaveAddr = wAddr;
	wAddr += 8;
	if (i = EvalBoolean(&fFlag))
	  break;

	if (OpCode(wAddr) != TK_THEN)
	  {
	  i = ERRINT_BADEXPRESSION;
	  break;
	  }

	if (fFlag)
	  {
	  wAddr = OpCode(wSaveAddr+0);
	  wLine = OpCode(wSaveAddr+2);
	  }
	else
	  {
	  wAddr = OpCode(wSaveAddr+4);
	  wLine = OpCode(wSaveAddr+6);
	  }

	if (wAddr == 0 || wLine == 0)
	  i = ERRINT_INTERNAL;
	wToken = TK_EOLL;
	break;

      case TK_INPUT:
        {
        WORD wSaveAddr, wSaveLine;

        wSaveAddr = wAddr;
        wSaveLine = wLine;
	if (i = EvalStrVariable(&nInputVar, FW_EOL))
          {
          wAddr = wSaveAddr;
          wLine = wSaveLine;
          if (i = EvalIntVariable(&nInputVar, FW_EOL))
            break;
          nInputStr = FALSE;
          }
        else
          nInputStr = TRUE;

	nInputInx = 0;
	nState = ST_INPUT;
        }
	break;

      case TK_KCLEAR:
	memset(&Macro, 0, sizeof(Macro));
	UpdateMacro();
	break;

      case TK_KERMIT:
	if (i = EvalString(byBuf, &byLen))
	  break;
	i = KermitCommand(byBuf, KRMSRC_SCRIPT);
        nLoop = 0;
	break;

      case TK_LABEL:
	if (OpCode(wAddr) == TK_VARIABLE)
	  SkipVariable();
	break;

      case TK_LOAD:
	if (i = EvalString(byBuf, &byLen))
	  break;
	DataOpen(byBuf);
	break;

      case TK_LPRINT:
        {
        BOOL bToggled;

	if (!bPrinterOn)
          {
          TogglePrinter();
          bToggled = TRUE;
          }
        else
          bToggled = FALSE;

	if (IfEndOfLine())
	  {
	  PrintLine(NULL);
	  TogglePrinter();
	  break;
	  }
	while (!IfEndOfLine())
	  {
	  wSaveAddr = wAddr;
	  if (i = EvalString(byBuf, &byLen))
	    {
	    if (i != ERRINT_MIXTYPES || byLen != 0)
	      break;
	    wAddr = wSaveAddr;
	    if (i = EvalInteger(&lInteger))
	      break;
	    ltoa(lInteger, byBuf, 10);
	    byLen = (BYTE)strlen(byBuf);
	    }
	  for (i = 0; i < byLen; i++)
	    {
	    PrintChar(byBuf[i], nPrintX);
	    nPrintX++;
	    }

	  if (OpCode(wAddr) == TK_SEMI)
	    {
	    wAddr += 2;
	    if (IfEndOfLine())
	      {
	      i = 0;
	      break;
	      }
	    }
	  else if (OpCode(wAddr) == TK_COMMA)
	    {
	    wAddr += 2;
            // 00090 Check for zero TabWidth.
            if (Term.cTabWidth)
              nPrintX = ((nPrintX / (int)Term.cTabWidth) + 1) * (int)Term.cTabWidth;
	    }
	  else
	    {
	    PrintLine(NULL);
	    i = 0;
	    break;
	    }
	  }

        if (bToggled)
          TogglePrinter();
        }
	break;

      case TK_LWAIT:
	if (OpCode(wAddr) == TK_NONE)
	  {
	  Xfer.cLWait = XFER_WAIT_NONE;
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_ECHO)
	  {
	  Xfer.cLWait = XFER_WAIT_ECHO;
	  wAddr += 2;
	  }
	else if (OpCode(wAddr) == TK_PROMPT)
	  {
	  wAddr += 2;
	  if (i = EvalString(byBuf, &byLen))
	    break;
	  if (byLen + 1 > sizeof(Xfer.szLWaitPrompt))
	    {
	    i = ERRINT_LONGSTRING;
	    break;
	    }
	  Xfer.cLWait = XFER_WAIT_PROMPT;
	  memcpy(&Xfer.szLWaitPrompt[1], byBuf, byLen);
	  Xfer.szLWaitPrompt[0] = byLen;
	  }
	else if (OpCode(wAddr) == TK_DELAY)
	  {
	  wAddr += 2;
	  if (i = EvalTime(&lInteger))
            break;
          if (lInteger < 0 || lInteger > 25000)
            {
            i = ERRATH_OUTOFRANGE;
            break;
            }
	  Xfer.cLWait = XFER_WAIT_DELAY;
          Xfer.nLWaitDelay = (int)lInteger;
	  }
	else if (OpCode(wAddr) == TK_COUNT)
	  {
	  wAddr += 2;
          if (i = EvalRange(&nInteger, 0, 255, NULL))
            break;
	  Xfer.cLWait = XFER_WAIT_COUNT;
          Xfer.cLWaitCount = (BYTE)nInteger;
	  }
	else
	  {
	  i = ERRINT_BADKEYWORD;
	  }
	break;

      case TK_MAXIMIZE:
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	break;

      case TK_MESSAGE:
	if (i = EvalString(byBuf, &byLen))
	  break;

	InfoDisplay(byBuf);
	break;

      case TK_MINIMIZE:
	ShowWindow(hWnd, SW_SHOWMINIMIZED);
	break;

      case TK_MKDIR:
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (mkdir(byBuf))
          i = ERRDOS_ACCESSDENIED;
	break;

      case TK_MOVE:
	{
	RECT Rect;
	int x, x1, y1, x2, y2;

        GetWindowRect(hWnd, &Rect);

	x1 = Rect.left;
	y1 = Rect.top;
	x2 = Rect.right-Rect.left;
	y2 = Rect.bottom-Rect.top;

	if (EvalRange(&x, 0, 4096, 0) == 0)
	  {
	  x1 = x;
	  if (SkipComma() == 0)
	    {
	    if (EvalRange(&x, 0, 4096, 0) == 0)
	      {
	      y1 = x;
	      if (SkipComma() == 0)
		{
		if (EvalRange(&x, 0, 4096, 0) == 0)
		  {
		  x2 = x;
		  if (SkipComma() == 0)
		    {
		    if (EvalRange(&x, 0, 4096, 0) == 0)
		      {
		      y2 = x;
		      }
		    }
		  }
		}
	      }
	    }
	  }

	MoveWindow(hWnd, x1, y1, x2, y2, 1);
	}
	break;

      case TK_NEXTLINE:
	lNextTimeout = 0L;
	nNextMax = 255;
	if (i = EvalStrVariable(&nInputVar, 0))
	  break;
	if (OpCode(wAddr) == TK_COMMA)
	  {
	  wAddr += 2;
	  if (i = EvalTime(&lInteger))
	    break;
	  if (lInteger)
	    lNextTimeout = GetCurrentTime() + lInteger;
	  if (OpCode(wAddr) == TK_COMMA)
	    {
	    wAddr += 2;
	    if (i = EvalRange(&nNextMax, 1, 250, FW_EOL))
	      break;
	    }
	  }
	nInputInx = 0;
	nState = ST_NEXT;
	break;

      case TK_NEW:
	DataNew();
	break;

      case TK_OFF:
	CommPortClose();
	break;

      case TK_OPEN:
	{
	int nMode, nChannel;

	if (OpCode(wAddr) == TK_RANDOM)
	  nMode = FILE_RANDOM;
	else if (OpCode(wAddr) == TK_INPUT)
	  nMode = FILE_INPUT;
	else if (OpCode(wAddr) == TK_OUTPUT)
	  nMode = FILE_OUTPUT;
	else if (OpCode(wAddr) == TK_APPEND)
	  nMode = FILE_APPEND;
	else
	  {
	  i = ERRINT_MISKEYWORD;
	  break;
	  }
	wAddr += 2;
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (OpCode(wAddr) != TK_AS)
	  {
	  i = ERRINT_MISKEYWORD;
	  break;
	  }
	wAddr += 2;
	if (i = EvalChannel(&nChannel, FW_EOL))
	  break;
	i = FileOpen(nMode, byBuf, nChannel);
	}
	break;

      case TK_PMODE:
        if (OpCode(wAddr) == TK_DOS)
          Xfer.fPmode = 1;
        else if (OpCode(wAddr) == TK_CPM)
          Xfer.fPmode = 0;
	else
	  i = ERRINT_BADKEYWORD;
	wAddr += 2;
	break;

      case TK_REPLY:
        if (!bConnected)
          {
          i = ERRINT_NOTONLINE;
          break;
          }
        else
	  {
	  CommDataStatus();

	  if (ComStat.cbOutQue + 786 > nCommPortMemory)
	    {
	    wAddr -= 2;
	    return;
	    }
	  }

	while (!IfEndOfLine())
	  {
          if (!bConnected)
            {
            i = ERRINT_NOTONLINE;
            break;
            }

	  wSaveAddr = wAddr;
	  if (i = EvalString(byBuf, &byLen))
	    {
	    if (i != ERRINT_MIXTYPES || byLen != 0)
	      break;
	    wAddr = wSaveAddr;
	    if (i = EvalInteger(&lInteger))
	      break;
	    ltoa(lInteger, byBuf, 10);
	    byLen = (BYTE)strlen(byBuf);
	    }
          if (byLen)
		   	{
				// dca00072 PJL Fixed REPLY '~'/REPLY '`~' problem.
				char outbuf[256];
				byBuf[byLen] = 0;
				memset(outbuf, 0, 256);
				LibDestore(byBuf, outbuf, 255);
				memset(byBuf, 0, 256);
				strcpy(byBuf, outbuf+1);
				CommStrOutput(byBuf, FALSE);
			}

	  if (OpCode(wAddr) == TK_SEMI)
	    {
	    wAddr += 2;
	    }
	  else if (OpCode(wAddr) == TK_COMMA)
            {
	    wAddr += 2;
	    }
	  else
            {
            break;
	    }
	  }

        if (OpCode(wAddr-2) != TK_SEMI)
          CommDataOutput(&Xfer.szLineDelim[1], (int)Xfer.szLineDelim[0]);
	break;

      case TK_POP:
	if (nGosubInx == 0)
	  {
	  i = ERRINT_GOSUBUNDER;
	  break;
	  }
	nGosubInx--;
	break;

      case TK_PRINT:
	if (OpCode(wAddr) == TK_AT)
	  {
	  int nRow, nCol;


	  wAddr += 2;
          if (i = EvalRange(&nRow, 0, 24, FW_COMMA))
	    break;
          if (i = EvalRange(&nCol, 0, 132, 0))
            break;
          if (nRow == 0 && nCol == 0)
            nRow = nCol = 1;
          if (nRow == 0 || nCol == 0)
            {
            i = ERRATH_OUTOFRANGE;
            break;
            }

	  if (OpCode(wAddr) == TK_COMMA)
            wAddr += 2;
          if (bDisplayOn)
            {
            nTermChrX = nCol;
            nTermChrY = nRow - 1;
            }
	  }
	if (IfEndOfLine())
          {
          if (bDisplayOn)
            {
            byBuf[0] = '\r';
            byBuf[1] = '\n';
            if (Term.fLfAuto)
              i = 1;
            else
              i = 2;
            TermWriteData(byBuf, i);
            }
	  i = 0;
	  break;
	  }
	while (!IfEndOfLine())
          {
          if (OpCode(wAddr) == TK_COMMA)
            wAddr += 2;

	  wSaveAddr = wAddr;
	  if (i = EvalString(byBuf, &byLen))
	    {
	    if (i != ERRINT_MIXTYPES || byLen != 0)
	      break;
	    wAddr = wSaveAddr;
	    if (i = EvalInteger(&lInteger))
	      break;
	    ltoa(lInteger, byBuf, 10);
	    byLen = (BYTE)strlen(byBuf);
	    }
          if (byLen && bDisplayOn)
            TermWriteData(byBuf, byLen);

	  if (OpCode(wAddr) == TK_SEMI)
	    {
	    wAddr += 2;
	    if (IfEndOfLine())
	      {
	      i = 0;
	      break;
	      }
	    }
	  else if (OpCode(wAddr) == TK_COMMA)
            {
	    wAddr += 2;
            if (bDisplayOn)
              TermWriteByte('\t');
	    }
	  else
            {
            if (bDisplayOn)
              {
              byBuf[0] = '\r';
              byBuf[1] = '\n';
              if (Term.fLfAuto)
                i = 1;
              else
                i = 2;
              TermWriteData(byBuf, i);
              }
	    i = 0;
	    break;
	    }
	  }
	break;

      case TK_PUT:
	{
	int  nChannel;
        WORD wSaveAddr, wSaveLine;

        wSaveAddr = wAddr;
        wSaveLine = wLine;

        if (EvalString(byBuf, &byLen) == 0)
          {
          if (IfEndOfLine())
            {
            i = FilePutRec((int)cDefOutputFile-1, byBuf, byLen);
            break;
            }
          }

        wAddr = wSaveAddr;
        wLine = wSaveLine;

	if (i = EvalChannel(&nChannel, FW_COMMA))
	  break;
	if (i = EvalString(byBuf, &byLen))
	  break;
        i = FilePutRec(nChannel, byBuf, byLen);
        }
	break;

      case TK_READ:
	{
	int  nChannel, nVarNo;

	if (OpCode(wAddr) == TK_LINE)
	  {
	  wAddr += 2;
	  if (OpCode(wAddr) != TK_VARIABLE)
	    {
	    if (i = EvalChannel(&nChannel, FW_COMMA))
	      break;
	    }
	  else
	    {
	    nChannel = (char)cDefInputFile-1;
	    }
	  if (i = EvalStrVariable(&nVarNo, 0))
	    break;

	  if (i = FileReadLine(nChannel, nVarNo))
	    break;
	  }
	else
	  {
	  if (OpCode(wAddr) != TK_VARIABLE)
	    {
	    if (i = EvalChannel(&nChannel, FW_COMMA))
	      break;
	    }
	  else
	    {
	    nChannel = (char)cDefInputFile-1;
	    }

	  while (1)
	    {
	    if (i = EvalStrVariable(&nVarNo, 0))
	      break;

	    if (i = FileRead(nChannel, nVarNo))
	      break;

	    if (OpCode(wAddr) != TK_COMMA)
	      break;

	    wAddr+= 2;
	    }
	  }
	}
	break;

      case TK_RECEIVE:
        if (!bConnected)
          {
          i = ERRINT_NOTONLINE;
          break;
          }
	if (i = EvalString(byBuf, &byLen))
	  byBuf[0] = NULL;
	switch (Xfer.cProtocol)
	  {
	  case XFER_XTALK:
	    if (byBuf[0])
              nErrorCode = XferCrossRecvName(byBuf);
	    else
              nErrorCode = XferCrossServer();
	    break;

	  case XFER_DART:
	    if (byBuf[0])
              nErrorCode = XferDartRecvName(byBuf);
	    else
              nErrorCode = XferDartServer(FALSE);
	    break;

	  case XFER_MODEM7:
	  case XFER_XMODEM:
	  case XFER_XMODEM1K:
	    nErrorCode = XferXModemRecvName(Xfer.cProtocol, byBuf);
	    break;

	  case XFER_YMODEM:
	    nErrorCode = XferYModemRecvName(byBuf);
	    break;

          case XFER_KERMIT:
            nErrorCode = XferKermitRecvName(byBuf);
	    break;

          case XFER_ZMODEM:
            nErrorCode = XferZmodemRecvName(byBuf);
	    break;
	  }
	if (nErrorCode == 0)
	  return;
	break;

      case TK_RENAME:
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (i = SkipComma())
	  break;
	if (i = EvalString(byBuf2, &byLen2))
	  break;
	if (DosMove(byBuf, byBuf2, 0L))
          nErrorCode = ERRDOS_NOFILE;
	break;

      case TK_REPEAT:
	break;

      case TK_REQUEST:
        if (!bConnected)
          {
          i = ERRINT_NOTONLINE;
          break;
          }
        // 00041 Request command requires an argument.
        if (i = EvalString(byBuf, &byLen))
	  {
	  i = ERRMIS_NOFILE;
          break;
	  }
	switch (Xfer.cProtocol)
	  {
	  case XFER_XTALK:
            nErrorCode = XferCrossRecvName(byBuf);
	    break;

	  case XFER_DART:
            nErrorCode = XferDartRecvName(byBuf);
	    break;

	  case XFER_MODEM7:
	  case XFER_XMODEM:
	  case XFER_XMODEM1K:
	    nErrorCode = XferXModemRecvName(Xfer.cProtocol, byBuf);
	    break;

	  case XFER_YMODEM:
	    nErrorCode = XferYModemRecvName(byBuf);
	    break;

          case XFER_KERMIT:
            nErrorCode = XferKermitRequestName(byBuf);
            break;

          case XFER_ZMODEM:
            nErrorCode = XferZmodemRecvName(byBuf);
	    break;
	  }
	if (nErrorCode == 0)
	  return;
	break;

      case TK_RESTORE:
	ShowWindow(hWnd, SW_SHOWNORMAL);
	break;

      case TK_RETURN:
	if (nGosubInx == 0)
	  {
	  i = ERRINT_GOSUBUNDER;
	  break;
	  }
	nGosubInx--;
	wAddr = wGosubAddr[nGosubInx];
	wLine = wGosubLine[nGosubInx];
	wToken = TK_EOLL;
	break;

      case TK_RMDIR:
	if (i = EvalString(byBuf, &byLen))
	  break;
	if (rmdir(byBuf))
          i = ERRDOS_NOPATH;
	break;

      case TK_RUN:
	if (i = EvalString(byBuf, &byLen))
	  break;
	if ((ptr = strchr(byBuf, ' ')) == NULL)
	  {
	  byBuf2[0] = 0;
	  byBuf2[1] = 0;
	  }
	else
	  {
	  *ptr = NULL;
	  strcpy(&byBuf2[1], ptr+1);
	  byBuf2[0] = (BYTE)strlen(&byBuf[1]);
	  }
	SpawnApp(byBuf, byBuf2);
	break;

      case TK_SAVE:
	{
        WORD wSaveAddr, wSaveLine;

        wSaveAddr = wAddr;
        wSaveLine = wLine;

        if (i = EvalString(byBuf, &byLen))
          byBuf[0] = '\0';

        if (byBuf[0] == '\0')
          {
          nErrorCode = DataSave();
          wAddr = wSaveAddr;
          wLine = wSaveLine;
          }
        else
			nErrorCode = DataSaveAs(byBuf);

	i = 0;
        }
	break;

      case TK_SEEK:
	{
	int  nChannel;
        WORD wSaveAddr, wSaveLine;

        wSaveAddr = wAddr;
        wSaveLine = wLine;

        if (EvalInteger(&lInteger) == 0)
          {
          if (IfEndOfLine())
            {
            i = FileSeek((int)cDefInputFile-1, lInteger);
            break;
            }
          }

        wAddr = wSaveAddr;
        wLine = wSaveLine;

	if (i = EvalChannel(&nChannel, FW_COMMA))
	  break;
	if (i = EvalInteger(&lInteger))
	  break;

	i = FileSeek(nChannel, lInteger);
	}
	break;

      case TK_SEND:
        if (!bConnected)
          {
          i = ERRINT_NOTONLINE;
          break;
          }
	if (i = EvalString(byBuf, &byLen))
	  byBuf[0] = NULL;
	switch (Xfer.cProtocol)
	  {
	  case XFER_XTALK:
	    if (byBuf[0])
              nErrorCode = XferCrossSendName(byBuf);
	    else
              nErrorCode = XferCrossServer();
	    break;

	  case XFER_DART:
	    if (byBuf[0])
	      nErrorCode = XferDartSendName(byBuf);
	    else
              nErrorCode = XferDartServer(TRUE);
	    break;

	  case XFER_MODEM7:
	  case XFER_XMODEM:
	  case XFER_XMODEM1K:
	    nErrorCode = XferXModemSendName(Xfer.cProtocol, byBuf);
	    break;

	  case XFER_YMODEM:
	    nErrorCode = XferYModemSendName(byBuf);
	    break;

          case XFER_ZMODEM:
	    if (byBuf[0])
              nErrorCode = XferZmodemSendName(byBuf);
	    else
              nErrorCode = ERRXFR_NEEDFILENAME;
	    break;

          case XFER_KERMIT:
	    if (byBuf[0])
              nErrorCode = XferKermitSendName(byBuf);
	    else
              nErrorCode = ERRXFR_NEEDFILENAME;
	    break;
	  }
	if (nErrorCode == 0)
	  return;
	break;

      case TK_SENDBREAK:
	CmdBreak();
	break;

      case TK_SHOW:
	ShowWindow(hWnd, SW_SHOW);
	break;

      case TK_SIZE:
	{
	RECT Rect;
	int  x, y;

	if (i = EvalRange(&x, 0, 4096, FW_COMMA))
	  break;
	if (i = EvalRange(&y, 0, 4096, 0))
	  break;

	GetWindowRect(hWnd, (LPRECT)&Rect);
	MoveWindow(hWnd, Rect.left, Rect.top, x, y, 1);
	}
	break;

      case TK_STRING:
	i = ERRINT_SYMCOMMAND;
	break;

      case TK_QUIT:
      case TK_TERMINATE:
	InterpreterStop();
	ProgramAbort();
	break;

      case TK_TRACE:
	if (i = EvalBoolean(&fFlag))
	  break;
	fTrace = fFlag;
	break;

      case TK_TRAP:
	nErrorCode = 0;
	if (OpCode(wAddr) == TK_ON)
	  {
	  fTrap = 1;
	  }
	else if (OpCode(wAddr) == TK_OFF)
	  {
	  fTrap = 0;
	  }
	else
	  {
	  i = ERRINT_MISONOFF;
	  }
	wAddr += 2;
	break;

      case TK_UNTIL:
	wSaveAddr = wAddr;
	wAddr += 4;
	if (i = EvalBoolean(&fFlag))
	  break;

	if (!fFlag)
	  {
	  wAddr = OpCode(wSaveAddr+0);
	  wLine = OpCode(wSaveAddr+2);
	  wToken = TK_EOLL;
	  }
	break;

      case TK_UPLOAD:
        if (!bConnected)
          {
          i = ERRINT_NOTONLINE;
          break;
          }
	if (i = EvalString(byBuf, &byLen))
	  break;
        nErrorCode = XferASCIIUploadName(byBuf);
        nLoop = 0;
	break;

      case TK_GLOBAL:
	if (OpCode(wAddr) == TK_LOAD)
	  {
	  wAddr += 2;
	  LoadGlobalData();
          LoadModemParameters();
	  break;
	  }
	else if (OpCode(wAddr) == TK_SAVE)
	  {
	  wAddr += 2;
	  SaveGlobalData();
          SaveModemParameters();
	  break;
	  }
      case TK_STRINGKEY:
      case TK_INTEGERKEY:
	{
        int  nLevel, nType;

	if (wToken == TK_GLOBAL)
	  {
	  nLevel = LEVEL_GLOBAL;
	  wToken = OpCode(wAddr);
	  wAddr += 2;
	  }
	else
	  {
	  nLevel = LEVEL_LOCAL;
	  }

	if (wToken == TK_STRINGKEY)
	  {
	  nType = TYPE_STRING;
	  }
	else if (wToken == TK_INTEGERKEY)
	  {
	  nType = TYPE_INTEGER;
	  }
	else
	  {
	  i = ERRINT_BADVARTYPE;
	  break;
	  }

	while (1)
          {
	  if (OpCode(wAddr) == TK_VARIABLE)
	    {
	    lstrcpy(byBuf, &lpObjectCode[wAddr+2]);
	    SkipVariable();
	    if ((i = CreateVariableHandle(byBuf, nLevel)) == 0)
	      {
	      i = ERRINT_VARIABLESPACE;
	      break;
	      }
            if (nType == TYPE_INTEGER)
	      SetIntVariable(i, 0L);
	    else
              nErrorCode = SetStrVariable(i, byBuf, 0);
	    }
	  else
	    {
            i = ERRINT_NORESERVE;
	    break;
	    }

	  if (IfEndOfLine())
	    {
            i = 0;
	    break;
	    }
	  else if (OpCode(wAddr) != TK_COMMA)
	    {
	    break;
	    }

	  wAddr += 2;
	  }
	}
	break;

      case TK_VARIABLE:
        {
        WORD wSaveAddr, wSaveLine;
	int nVar;

	wAddr -= 2;
	lstrcpy(byBuf, &lpObjectCode[wAddr+2]);
	SkipVariable();
	if (OpCode(wAddr) == TK_EQ)
          {
	  wAddr += 2;
          }
	else
          {
          i = ERRINT_BADCOMMAND;
	  break;
	  }

	if ((nVar = GetVariableHandle(byBuf)) == -1)
	  {
	  if ((nVar = CreateVariableHandle(byBuf, LEVEL_LOCAL)) == 0)
	    {
	    i = ERRINT_VARIABLESPACE;
	    break;
	    }
	  }

	wSaveAddr = wAddr;
        wSaveLine = wLine;
	if (i = EvalString(byBuf, &byLen))
	  {
	  if (i != ERRINT_MIXTYPES)
	    break;
	  wAddr = wSaveAddr;
          wLine = wSaveLine;
	  if (i = EvalInteger(&lInteger))
	    break;
	  SetIntVariable(nVar, lInteger);
	  }
	else
	  {
          if (i = SetStrVariable(nVar, byBuf, byLen))
            {
            i = 0;
            nErrorCode = i;
            }
	  }
	}
	break;

      case TK_WAIT:
	WaitPrep();
	wToken = TK_EOLL;
	break;

      case TK_WATCH:
	WatchPrep();
	wToken = TK_EOLL;
	break;

      case TK_WEND:
	wLine = OpCode(wAddr+2);
	wAddr = OpCode(wAddr+0);
	wToken = TK_EOLL;
	break;

      case TK_WHILE:
	wSaveAddr = wAddr;
	wAddr += 4;
	if (i = EvalBoolean(&fFlag))
	  break;

	if (!fFlag)
	  {
	  wAddr = OpCode(wSaveAddr+0) + 6;
	  wLine = OpCode(wSaveAddr+2);
	  wToken = TK_EOLL;
	  }
	break;

      case TK_WRITE:
	{
	int  nChannel, nVarNo;

	if (OpCode(wAddr) == TK_LINE)
	  {
	  wAddr += 2;

	  wSaveAddr = wAddr;
	  if (i = EvalChannel(&nChannel, FW_COMMA))
	    {
	    wAddr = wSaveAddr;
	    nChannel = (int)cDefOutputFile-1;
	    if (OpCode(wAddr) == TK_COMMA)
	      wAddr += 2;
	    }

	  if (i = EvalString(byBuf, &byLen))
	    break;

	  if (i = FileWrite(nChannel, byBuf, byLen))
	    break;

	  if (OpCode(wAddr) != TK_SEMI)
	    {
	    if (i = FileWriteLine(nChannel))
	      break;
	    }
	  else
	    wAddr += 2;
	  }
	else
	  {
	  wSaveAddr = wAddr;
	  if (i = EvalChannel(&nChannel, FW_COMMA))
	    {
	    wAddr = wSaveAddr;
	    nChannel = (int)cDefOutputFile-1;
	    if (OpCode(wAddr) == TK_COMMA)
	      wAddr += 2;
	    }

	  while (1)
	    {
	    if (i = EvalString(byBuf, &byLen))
	      break;

	    if (i = FileWrite(nChannel, byBuf, byLen))
	      break;

	    if (OpCode(wAddr) != TK_COMMA)
	      break;

	    byBuf[0] = ',';
	    if (i = FileWrite(nChannel, byBuf, 1))
	      break;

	    wAddr+= 2;
	    }

	  if (OpCode(wAddr) != TK_SEMI)
	    {
	    if (i = FileWriteLine(nChannel))
	      break;
	    }
	  else
	    wAddr += 2;
	  }
	}
	break;

      case TK_ANSWERBACK:
      case TK_ANSWERSETUP:
      case TK_BLANKEX:
      case TK_DESCRIPTION:
      case TK_DIALPREFIX:
      case TK_DIALSUFFIX:
      case TK_EDITOR:
      case TK_ENTER:
      case TK_FOOTER:
      case TK_HEADER:
      case TK_LDNUMBER:
      case TK_LINEDELIM:
      case TK_MISC:
      case TK_MODEMHANGUP:
      case TK_MODEMINIT:
      case TK_MODEMNAME:
      case TK_MODEMRESULTS:
      case TK_MODIFIER:
      case TK_NETID:
      case TK_NUMBER:
      case TK_OUTNUMBER:
      case TK_PASSWORD:
      case TK_PROTOCOL:
      case TK_SCRIPT:
      case TK_STARTUP:
      case TK_TERMINAL:
      case TK_USERID:
	if (OpCode(wAddr) == TK_EQ)
	  wAddr += 2;
        if (OpCode(wAddr) == TK_NEXTLINE)
           {
           i =  ERRINT_BADEXPRESSION;
           break;
           }
	if (i = EvalString(byBuf, &byLen))
	  break;

        nErrorCode = PutSysVariableAny(wToken, byBuf, byLen, 0L);
	break;

      case TK_AUTOWRAP:
      case TK_BACKUPS:
      case TK_BREAKLEN:
      case TK_COLUMNS:
      case TK_CURSHAPE:
      case TK_DATABITS:
      case TK_DATEMODE:
      case TK_DISPLAY:
      case TK_ECHO:
      case TK_FLOW:
      case TK_FORMFEED:
      case TK_FRAMINGERRORS:
      case TK_GRAPHICS:
      case TK_HOSTSCRIPT:
      case TK_HOSTMODE:
      case TK_LFAUTO:
      case TK_LINETIME:
      case TK_LOCAL:
      case TK_MAXERRORS:
      case TK_MODEMCHANGE:
      case TK_NEWLINE:
      case TK_NOCOLOR:
      case TK_OVERRUNERRORS:
      case TK_PACKETSIZE:
      case TK_PARITY:
      case TK_PARITYERRORS:
      case TK_PATIENCE:
      case TK_PORT:
      case TK_PRINTER:
      case TK_PRINTEXTENT:
      case TK_RECOVERY:
      case TK_RECVMODE:
      case TK_REDIALCOUNT:
      case TK_REDIALWAIT:
      case TK_REVIEW:
      case TK_SECRET:
      case TK_SHOWACTIVE:
      case TK_SHOWINFOBAR:
      case TK_SHOWINPUT:
      case TK_SHOWMACROBAR:
      case TK_SHOWSTATUSBAR:
      case TK_SHOWHSCROLL:
      case TK_SHOWVSCROLL:
      case TK_SPEED:
      case TK_STOPBITS:
      case TK_SWAPDEL:
      case TK_TABEX:
      case TK_TABWIDTH:
      case TK_TIMING:
      case TK_WINDOWWRAP:
      case TK_XOFFCHAR:
      case TK_XONCHAR:
	if (OpCode(wAddr) == TK_EQ)
	  wAddr += 2;
	if (i = EvalInteger(&lInteger))
	  break;
        nErrorCode = PutSysVariableAny(wToken, NULL, NULL, lInteger);
	break;

      case TK_COLORSCREEN:
        i = ERRINT_READONLY;
        break;

      default:
	i = ERRINT_BADCOMMAND;
	break;
      }

    if (hObjectCode == 0)
      return;

    if (i)
      {
    //if (IfEndOfLine())
    //  wLine--;
      if (i == ERRINT_EXTRATEXT && (wToken & 0xFFE0) == CL_KEYWD)
        i = ERRINT_NORESERVE;
      InterpreterError(i);
      return;
      }

    if (fTrap == 0 && nErrorCode && (nErrorCode != -1))
      {
    //if (IfEndOfLine())
    //  wLine--;
      InterpreterError(nErrorCode);
      return;
      }
    else
      {
      if (nErrorCode && nErrorCode != -1)
        {
        nErrorFlag = TRUE;
        nErrorSave = nErrorCode;
        nErrorCode = 0;
        }
      }

    if (wToken!=TK_EOLL && wToken!=TK_EOPL && wToken!=TK_EOVL && !IfEndOfLine())
      {
      if (IfEndOfLine())
        wLine--;
      if ((wToken & 0xFFE0) == CL_KEYWD)
        InterpreterError(ERRINT_BADEXPRESSION);
      else
        InterpreterError(ERRINT_EXTRATEXT);
      }
    }
}

void near WatchPrep()
{
  WORD wSaveAddr, wSaveLine;
  BYTE byBuf[256], byLen;
  long lLong;
  BYTE byFlag;
  int  nParams;


  fWatchComm = FALSE;
  lWatchTime = 0L;
  nParams = 0;
  fWaitOL = TRUE;	// JDB dca00020 Watch must always be ONLINE

  if (OpCode(wAddr) == TK_FOR)
    {
    wAddr += 2;
    }
  else if (!(OpCode(wAddr) == TK_EOPL))
    {
    if (EvalTime(&lLong))
      {
      InterpreterError(ERRINT_BADWATCH);
      return;
      }
    else
      {
      lWatchTime = GetCurrentTime() + lLong;
      nParams = 1;

      if (OpCode(wAddr) == TK_FOR)
	wAddr += 2;
      }
    }

  if (GotoCleanEOL())
    {
    InterpreterError(ERRINT_EXTRATEXT);
    return;
    }

  fWatchWait   = 1;
  nWatchStack  = nStackAddr;
  lWatchQuiet  = 0L;
  wWatchKey    = -1;
  lWatchCount  = 0L;
  szMatchedString[0] = NULL;

  fTimeout = FALSE;

  while (1)
    {
    while (OpCode(wAddr) == TK_EOPL)
      wAddr += 2;

    if (OpCode(wAddr) == TK_ENDWATCH)
      {
      wAddr += 2;
      filexfer.stack.data[nStackAddr++] = 0;
      wEolAddr = wAddr;
      wEolLine = wLine;

      if (nParams == 0)
	InterpreterError(ERRINT_NOWATCHPARM);
      else
	nState = ST_WATCH;
      break;
      }

    if (PushLocation(0))
      {
      InterpreterError(ERRINT_TOOCOMPLEX);
      return;
      }

    GotoNextEOLL();
    while (OpCode(wAddr-4) == TK_COMMA && OpCode(wAddr-2) == TK_EOPL)
      GotoNextEOLL();

    if (OpCode(wAddr-2) != TK_EOLL)
      {
      InterpreterError(ERRINT_BADWATCHPARM);
      return;
      }
    wSaveAddr = wAddr;
    wSaveLine = wLine;
    PopLocation(0);

    if (OpCode(wAddr) == TK_QUIET)
      {
      wAddr += 2;
      fWatchComm = TRUE;
      if (lWatchQuiet != 0L)
	{
	InterpreterError(ERRINT_DUPKEYWORD);
	return;
	}
      if (EvalTime(&lWatchQuiet))
	{
	InterpreterError(ERRINT_BADTIME);
	return;
	}
      if (lWatchQuiet <= 0L)
	{
	InterpreterError(ERRINT_BADQUIET);
	return;
	}
      wWatchQuietAddr = wSaveAddr;
      wWatchQuietLine = wSaveLine;
      GotoNextEOPL();
      lTimeLastRecv = GetCurrentTime();
      }
    else if (OpCode(wAddr) == TK_KEY)
      {
      wAddr += 2;
      if (wWatchKey != -1)
	{
	InterpreterError(ERRINT_DUPKEYWORD);
	return;
	}
      if (EvalInteger(&lLong))
	{
	InterpreterError(ERRINT_WANTINTEGER);
	return;
	}
      wWatchKey = (WORD)lLong;
      if (wWatchKey == -1)
	{
	InterpreterError(ERRINT_BADKEY);
	return;
	}
      wWatchKeyAddr = wSaveAddr;
      wWatchKeyLine = wSaveLine;
      GotoNextEOPL();
      }
    else if (OpCode(wAddr) == TK_COUNT)
      {
      wAddr += 2;
      fWatchComm = TRUE;
      if (lWatchCount != 0L)
	{
	InterpreterError(ERRINT_DUPKEYWORD);
	return;
	}
      if (EvalInteger(&lWatchCount))
	{
	InterpreterError(ERRINT_WANTINTEGER);
	return;
	}
      if (lWatchCount <= 0L)
	{
	InterpreterError(ERRINT_BADCOUNT);
	return;
	}
      wWatchCountAddr = wSaveAddr;
      wWatchCountLine = wSaveLine;
      GotoNextEOPL();
      }
    else if (OpCode(wAddr) != TK_CASE && OpCode(wAddr) != TK_SPACE && (OpCode(wAddr) & CL_KEYWD))
      {
      InterpreterError(ERRINT_BADWATCHPARM);
      return;
      }
    else
      {
      fWatchComm = TRUE;
      while (1)
        {
        if (OpCode(wAddr) == TK_CASE)
          {
          byFlag = 1;
          wAddr += 2;
          if (OpCode(wAddr) == TK_SPACE)
            {
            byFlag = 3;
            wAddr += 2;
            }
          }
        else if (OpCode(wAddr) == TK_SPACE)
          {
          byFlag = 2;
          wAddr += 2;
          if (OpCode(wAddr) == TK_CASE)
            {
            byFlag = 3;
            wAddr += 2;
            }
          }
        else
          byFlag = 0;

	if (EvalString(byBuf, &byLen))
	  {
	  InterpreterError(ERRINT_BADWATCHSTR);
	  return;
	  }

        if (byLen > 32)
          byLen = 32;

	if (nStackAddr + byLen + 32 >= nStackHigh)
	  {
	  InterpreterError(ERRINT_TOOCOMPLEX);
	  return;
	  }

	filexfer.stack.data[nStackAddr++] = (BYTE)(wSaveAddr % 256);
	filexfer.stack.data[nStackAddr++] = (BYTE)(wSaveAddr / 256);
	filexfer.stack.data[nStackAddr++] = (BYTE)(wSaveLine % 256);
	filexfer.stack.data[nStackAddr++] = (BYTE)(wSaveLine / 256);
	filexfer.stack.data[nStackAddr++] = byFlag;
	filexfer.stack.data[nStackAddr++] = 0;
	filexfer.stack.data[nStackAddr++] = byLen;
	memcpy(&filexfer.stack.data[nStackAddr], byBuf, (WORD)byLen);
	nStackAddr += byLen;

	if (OpCode(wAddr) == TK_COMMA)
          {
          wAddr += 2;
          if (IfEndOfLine())
            GotoNextEOLL();
          }
	else
	  break;
	}

      GotoNextEOPL();
      }

    nParams++;
    }
}

void near WaitPrep()
{
  BYTE byBuf[256], byLen;
  long lLong;
  BYTE byFlag;
  int  nParams;


  fWatchComm = FALSE;
  if (PushLocation(0))
    {
    InterpreterError(ERRINT_TOOCOMPLEX);
    return;
    }

  fWaitOL = TRUE;	// dca00020 JDB Start out assumeing wait must be online

  if (EvalTime(&lLong))
    {
    PopLocation(0);
    lWatchTime = 0L;
    }
  else
    {
    nStack--;
    lWatchTime = GetCurrentTime() + lLong;
    }

  fWatchWait   = 0;
  nWatchStack  = nStackAddr;
  lWatchQuiet  = 0L;
  wWatchKey    = -1;
  lWatchCount  = 0L;
  szMatchedString[0] = NULL;

  fTimeout = FALSE;

  byFlag = 0;

  if (OpCode(wAddr) == TK_FOR)
    wAddr += 2;
  else
     fWaitOL = FALSE;	// JDB dca00020  If no FOR then wait does not have
			// to be online.

  while (1)
    {
    if (IfEndOfLine())
      {
      break;
      }
    else if (OpCode(wAddr) == TK_CASE)
      {
      byFlag |= 1;
      wAddr  += 2;
      }
    else if (OpCode(wAddr) == TK_SPACE)
      {
      byFlag |= 2;
      wAddr  += 2;
      }
    else if (OpCode(wAddr) == TK_QUIET)
      {
      wAddr += 2;
      fWatchComm = TRUE;
      if (lWatchQuiet != 0L)
	{
	InterpreterError(ERRINT_DUPKEYWORD);
	return;
	}
      if (EvalTime(&lWatchQuiet))
	{
	InterpreterError(ERRINT_BADTIME);
	return;
	}
      if (lWatchQuiet <= 0L)
	{
	InterpreterError(ERRINT_BADQUIET);
	return;
        }
      lTimeLastRecv = GetCurrentTime();
      }
    else if (OpCode(wAddr) == TK_KEY)
      {
      wAddr += 2;
      if (wWatchKey != -1)
	{
	InterpreterError(ERRINT_DUPKEYWORD);
	return;
	}
      if (EvalInteger(&lLong))
	{
	InterpreterError(ERRINT_WANTINTEGER);
	return;
	}
      wWatchKey = (WORD)lLong;
      if (wWatchKey == -1)
	{
	InterpreterError(ERRINT_BADKEY);
	return;
	}
      }
    else if (OpCode(wAddr) == TK_COUNT)
      {
      wAddr += 2;
      fWatchComm = TRUE;
      if (lWatchCount != 0L)
	{
	InterpreterError(ERRINT_DUPKEYWORD);
	return;
	}
      if (EvalInteger(&lWatchCount))
	{
	InterpreterError(ERRINT_WANTINTEGER);
	return;
	}
      if (lWatchCount <= 0L)
	{
	InterpreterError(ERRINT_BADCOUNT);
	return;
	}
      }
    else if ((OpCode(wAddr) & 0xFF00) == CL_KEYWD)
      {
      InterpreterError(ERRINT_BADWATCHPARM);
      return;
      }
    else
      {
      fWatchComm = TRUE;

      if (EvalString(byBuf, &byLen))
	{
	InterpreterError(ERRINT_BADWATCHSTR);
	return;
	}

      if (byLen > 32)
        byLen = 32;

      if (nStackAddr + byLen + 32 >= nStackHigh)
        {
	InterpreterError(ERRINT_TOOCOMPLEX);
	return;
	}

      filexfer.stack.data[nStackAddr++] = 1;
      filexfer.stack.data[nStackAddr++] = 1;
      filexfer.stack.data[nStackAddr++] = 1;
      filexfer.stack.data[nStackAddr++] = 1;
      filexfer.stack.data[nStackAddr++] = byFlag;
      filexfer.stack.data[nStackAddr++] = 0;
      filexfer.stack.data[nStackAddr++] = byLen;
      memcpy(&filexfer.stack.data[nStackAddr], byBuf, (WORD)byLen);
      nStackAddr += byLen;
      }

    if (OpCode(wAddr) == TK_COMMA)
      wAddr += 2;
    }

  filexfer.stack.data[nStackAddr++] = 0;
  nState = ST_WATCH;
}

static void near StateWatch()
{
  WORD wInx, wOff;
  BYTE byBuf[64], byLen;
  register i;


  //if (!bConnected) 	//dca00020
  if (!bConnected && (fWaitOL == TRUE)) // JDB dca00020 Check online status
					// only if needed
    {
    nErrorCode = ERRDIA_LOSTCONNECT;
    nStackAddr = nWatchStack;
    nState     = ST_NORMAL;
    return;
    }

  if (lWatchTime != 0L)
    {
    if (GetCurrentTime() > lWatchTime)
      {
      fTimeout   = TRUE;
      nStackAddr = nWatchStack;
      nState     = ST_NORMAL;
      return;
      }
    }

  if (fWatchComm == FALSE)
    {
    byLen = 0;
    while ((i = CommGet()) != -1)
      {
      if (byLen >= sizeof(byBuf))
        {
        if (bDisplayOn)
          TermWriteData(byBuf, byLen);
        if (fHostScript)
          CommDataOutput(byBuf, byLen);
        byLen = 0;
        }
      byBuf[byLen++] = (BYTE)i;
      }
    if (bDisplayOn)
      TermWriteData(byBuf, byLen);
    if (fHostScript)
      CommDataOutput(byBuf, byLen);
    return;
    }

  byLen = 0;
  while ((i = CommGet()) != -1)
    {
    if (byLen >= sizeof(byBuf))
      {
      if (bDisplayOn)
        TermWriteData(byBuf, byLen);
      if (fHostScript)
        CommDataOutput(byBuf, byLen);
      byLen = 0;
      }
    byBuf[byLen++] = (BYTE)i;

    i &= 0x7F;

    if (lWatchCount != 0L)
      {
      if (lWatchCount-- <= 1)
	{
        nStackAddr = nWatchStack;
        if (fWatchWait)
          {
          wAddr = wWatchCountAddr;
          wLine = wWatchCountLine;
          }

        if (byLen && bDisplayOn)
          TermWriteData(byBuf, byLen);
        if (fHostScript)
          CommDataOutput(byBuf, byLen);

        nState = ST_NORMAL;
        return;
	}
      }

    wInx = nWatchStack;
    while (filexfer.stack.data[wInx])
      {
      WORD wFirstOff;

      wOff = (WORD)filexfer.stack.data[wInx+5];

FirstChar:
      wFirstOff = wOff;
      if ((filexfer.stack.data[wInx+4] & 0x02) && filexfer.stack.data[wInx+wOff+7] != ' ' && isspace(i))
	{
	}
      else if (filexfer.stack.data[wInx+wOff+7] == '~')
	{
	wOff++;
	switch (filexfer.stack.data[wInx+wOff+7])
	  {
	  case '_':
	    if (isspace(i))
	      wOff++;
	    else
	      wOff = 0;
	    break;

	  case 'A':
	    if (isupper(i))
	      wOff++;
	    else
	      wOff = 0;
	    break;

	  case 'a':
	    if (islower(i))
	      wOff++;
	    else
	      wOff = 0;
	    break;

	  case '#':
	    if (isdigit(i))
	      wOff++;
	    else
	      wOff = 0;
	    break;

	  case 'X':
	    if (isalnum(i))
	      wOff++;
	    else
	      wOff = 0;
	    break;

	  case '?':
	    wOff++;
	    break;

	  default:
	    if (i == '~')
	      wOff++;
	    else
	      wOff = 0;
	    break;
	  }
	}
      else
	{
	if (filexfer.stack.data[wInx+4] & 0x01)
          {
	  if (filexfer.stack.data[wInx+wOff+7] == (BYTE)i)
	    wOff++;
	  else
	    wOff = 0;
	  }
	else
	  {
	  register j;

	  j = (int)(filexfer.stack.data[wInx+wOff+7]);
	  if (toupper(j) == toupper(i))
	    wOff++;
	  else
	    wOff = 0;
	  }
	}

      if (wOff == 0 && wFirstOff)
        goto FirstChar;

      if (wOff == (WORD)filexfer.stack.data[wInx+6])
	{
	if (fWatchWait)
	  {
	  wAddr = filexfer.stack.data[wInx+0] + filexfer.stack.data[wInx+1] * 256;
	  wLine = filexfer.stack.data[wInx+2] + filexfer.stack.data[wInx+3] * 256;
	  }

        if (wOff > 32)
          wOff = 32;
	memcpy(szMatchedString, &filexfer.stack.data[wInx+7], wOff);
	szMatchedString[wOff] = NULL;

	nStackAddr = nWatchStack;
	nState	   = ST_NORMAL;
	break;
	}
      else
	filexfer.stack.data[wInx+5] = (BYTE)wOff;

      wInx += filexfer.stack.data[wInx+6] + 7;
      }
    }

  if (byLen && bDisplayOn)
    TermWriteData(byBuf, byLen);
  if (fHostScript)
    CommDataOutput(byBuf, byLen);

  if (lWatchQuiet != 0L)
    {
    if (lTimeLastRecv + lWatchQuiet < GetCurrentTime())
      {
      if (nState != ST_NORMAL)
	{
	nStackAddr = nWatchStack;
        if (fWatchWait)
          {
          wAddr = wWatchQuietAddr;
          wLine = wWatchQuietLine;
          }
        nState = ST_NORMAL;
	}
      }
    }
}

int GotoLabel(szLabel)
  char *szLabel;
{
  WORD wSaveAddr, wSaveLine;
  int  nLen;
  register i;


  wSaveAddr = wAddr;
  wSaveLine = wLine;
  if ((nLen = strlen(szLabel)) == 0)
    return (TRUE);

  wAddr = 0;
  wLine = 1;

  while (1)
    {
    if (OpCode(wAddr) == TK_LABEL)
      {
      if (OpCode(wAddr+2) == TK_VARIABLE)
	{
	if (lmemcmp(&lpObjectCode[wAddr+4], szLabel, nLen + 1) == 0)
	  return (FALSE);
	}
      }

    i = 1;
    while (i)
      {
      switch (OpCode(wAddr))
	{
	case TK_EOLL:
	  wAddr += 2;
	  i = 0;
	  break;

	case TK_EOVL:
	case TK_EOPL:
	  wAddr += 2;
	  wLine++;
	  i = 0;
	  break;

	case TK_INTEGER:
	  wAddr += 6;
	  break;

	case TK_STRING:
	  wAddr += (WORD)lpObjectCode[wAddr+2] + 3;
	  break;

	case TK_VARIABLE:
	  SkipVariable();
	  break;

	case TK_EOFM:
	  i = 0;
	  break;

	case TK_ELSE:
	case TK_UNTIL:
	case TK_WHILE:
	case TK_WEND:
	  wAddr += 6;
	  break;

	case TK_IF:
	  wAddr += 10;
	  break;

	default:
	  wAddr += 2;
	  break;
	}
      }

    if (OpCode(wAddr) == TK_EOFM)
      {
      wAddr = wSaveAddr;
      wLine = wSaveLine;
      return (TRUE);
      }
    }

  return (FALSE);
}

static void near GotoEndOfStatement()
{
  while (1)
    {
    switch (OpCode(wAddr))
      {
      case TK_EOLL:
	wAddr += 2;
	return;
	break;

      case TK_EOVL:
	wAddr += 2;
	wLine++;
	if (fTrace)
	  TraceLine();
	return;
	break;

      case TK_EOPL:
	if (wEolLine)
	  {
	  wAddr = wEolAddr;
	  wLine = wEolLine;
	  wEolLine = 0;
	  }
	else
	  {
	  wAddr += 2;
	  wLine++;
	  }
	if (fTrace)
	  TraceLine();
	return;
	break;

      case TK_INTEGER:
	wAddr += 6;
	break;

      case TK_STRING:
	wAddr += (WORD)lpObjectCode[wAddr+2] + 3;
	break;

      case TK_VARIABLE:
	SkipVariable();
	break;

      case TK_EOFM:
	return;
	break;

      case TK_ELSE:
        return;

      case TK_UNTIL:
      case TK_WHILE:
      case TK_WEND:
	wAddr += 6;
	break;

      case TK_IF:
	wAddr += 10;
	break;

      default:
	wAddr += 2;
	break;
      }
    }
}

static void near GotoNextEOLL()
{
  while (1)
    {
    switch (OpCode(wAddr))
      {
      case TK_EOLL:
	wAddr += 2;
	return;
	break;

      case TK_EOVL:
	wAddr += 2;
	wLine++;
	if (fTrace)
	  TraceLine();
	return;
	break;

      case TK_EOPL:
	if (wEolLine)
	  {
	  wAddr = wEolAddr;
	  wLine = wEolLine;
	  wEolLine = 0;
	  }
	else
	  {
	  wAddr += 2;
	  wLine++;
	  }
	if (fTrace)
	  TraceLine();
	return;
	break;

      case TK_INTEGER:
	wAddr += 6;
	break;

      case TK_STRING:
	wAddr += (WORD)lpObjectCode[wAddr+2] + 3;
	break;

      case TK_VARIABLE:
	SkipVariable();
	break;

      case TK_EOFM:
	return;
	break;

      case TK_ELSE:
      case TK_UNTIL:
      case TK_WHILE:
      case TK_WEND:
	wAddr += 6;
	break;

      case TK_IF:
	wAddr += 10;
	break;

      default:
	wAddr += 2;
	break;
      }
    }
}

static void near GotoNextEOPL()
{
  while (1)
    {
    switch (OpCode(wAddr))
      {
      case TK_EOPL:
	if (wEolLine)
	  {
	  wAddr = wEolAddr;
	  wLine = wEolLine;
	  wEolLine = 0;
	  }
	else
	  {
	  wAddr += 2;
	  wLine++;
	  }
	if (fTrace)
	  TraceLine();
	return;
	break;

      case TK_INTEGER:
	wAddr += 6;
	break;

      case TK_STRING:
	wAddr += (WORD)lpObjectCode[wAddr+2] + 3;
	break;

      case TK_VARIABLE:
	SkipVariable();
	break;

      case TK_EOFM:
	return;
	break;

      case TK_ELSE:
      case TK_UNTIL:
      case TK_WHILE:
      case TK_WEND:
	wAddr += 6;
	break;

      case TK_IF:
	wAddr += 10;
	break;

      default:
	wAddr += 2;
	break;
      }
    }
}

static int near GotoCleanEOL()
{
  switch (OpCode(wAddr))
    {
    case TK_EOLL:
      wAddr += 2;
      break;

    case TK_EOPL:
      wAddr += 2;
      wLine++;
      if (fTrace)
	TraceLine();
      break;

    case TK_EOFM:
      break;

    default:
      return (TRUE);
      break;
    }

  return (FALSE);
}

int IfEndOfLine()
{
  if (OpCode(wAddr) == TK_EOPL || OpCode(wAddr) == TK_EOLL ||
      OpCode(wAddr) == TK_EOVL ||
      OpCode(wAddr) == TK_EOFM || OpCode(wAddr) == TK_ELSE)
    return (TRUE);
  else
    return (FALSE);
}

static int near PopLocation(wToken)
  WORD wToken;
{
  if (nStack && wStackType[nStack-1] == wToken)
    {
    nStack--;
    wAddr = wStackAddr[nStack];
    wLine = wStackLine[nStack];
    return (FALSE);
    }

  return (TRUE);
}

static int near PushLocation(wToken)
  WORD wToken;
{
  if (nStack < sizeof(wStackType) / sizeof(WORD))
    {
    wStackType[nStack] = wToken;
    wStackAddr[nStack] = wAddr;
    wStackLine[nStack] = wLine;
    nStack++;
    return (FALSE);
    }

  return (TRUE);
}

void near SkipVariable()
{
  wAddr += 2;
  while (lpObjectCode[wAddr])
    wAddr++;
  wAddr++;
}

int near SkipComma()
{
  if (OpCode(wAddr) != TK_COMMA)
    return (ERRINT_MISCOMMA);

  wAddr += 2;

  return (0);
}

static void near TraceLine()
{
  char buf[32], fmt[8];

  fmt[0] = '[';
  fmt[1] = '%';
  fmt[2] = '0';
  fmt[3] = '4';
  fmt[4] = 'd';
  fmt[5] = ']';
  fmt[6] = NULL;

  sprintf(buf, fmt, wLine);
  LineToTerminal(buf);
}

#ifdef KENT

void dumpdata(pStart, nLen)
  BYTE *pStart;
  int  nLen;
{
  char buf[256];
  register i;

  return;
  for (i = 0; i < nLen; i++)
    {
    if ((i % 16) == 0)
      write(3, "\r\n", 2);
    sprintf(buf, "%02x ", *pStart++);
    write(3, buf, strlen(buf));
    }
  write(3, "\r\n", 2);
}

void dprint(buf)
  BYTE *buf;
{
  return;
  write(3, buf, strlen(buf));
  write(3, "\r\n", 2);
}

#endif
