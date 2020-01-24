/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: AI Data Scanning Module (AISCAN.C)                               */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This module receives pre-formatted line data that is being       */
/*           received and sent through the communication port.  The AISCAN    */
/*           module examines the data and set flags and/or execute routines.  */
/*           Currently, all we do is keep track of the client and search      */
/*           library so it can be displayed on the status line.               */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "lexis.h"
#include "aiscan.h"
#include "comm.h"
#include "session.h"
#include "data.h"
#include "term.h"
#include "dialogs.h"


/******************************************************************************/
/*									      */
/*  Global Variables							      */
/*									      */
/******************************************************************************/

  SHORT sAiState;


/******************************************************************************/
/*									      */
/*  Local Procedures Prototypes 					      */
/*									      */
/******************************************************************************/

  static BOOL near NewCommand(PBYTE pBuffer, SHORT cBuffer);


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommInit();                                                          */
/*									      */
/*  Called in the application's initialize routine so we can get everything   */
/*  in order.  Returns true is successful (for future possiblities).          */
/*									      */
/*----------------------------------------------------------------------------*/
void AIscanReset()
 {
 sAiState = STATE_LOGON;
 }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommInit();                                                          */
/*									      */
/*  Called in the application's initialize routine so we can get everything   */
/*  in order.  Returns true is successful (for future possiblities).          */
/*									      */
/*----------------------------------------------------------------------------*/
void AIscanInpLine(PBYTE pBuffer, SHORT cBuffer)
  {

//{
//char buf[64];

//extern cdecl sprintf();
//extern cdecl write();

//sprintf(buf, "(%d,", sAiState);
//write(3, buf, strlen(buf));
//write(3, pBuffer, cBuffer);
//write(3, ")\r\n", 3);
//}

  switch (sAiState)
    {
    case STATE_LOGON:
      if (*pBuffer == 'T' && cBuffer >= 12 && cBuffer <= 20)
        {
	if (memcmp(pBuffer, "TRANSMIT key", 12) == 0)
	  {
          if (fWorkHyperRomMode)
	    {
	    CommWriteChar('\x02');
            CommWriteData(szWorkHyperRom, -1);
	    CommWriteChar('\r');
	    InfoMessage(IDS_INFO_USERIDP, NULL);
	    }
	  else
	    {
	    if (szDataLogon[0])
	      {
	      CommWriteChar('\x02');
	      CommWriteData(szDataLogon, -1);
	      CommWriteChar('\r');
	      InfoMessage(IDS_INFO_USERID, NULL);
              bWorkLogonSent = TRUE;
	      }
	    }

	  sAiState = STATE_NULL;
          }
        }
      break;

    case STATE_ECLIPSE:
      /*
      ** The client name is delimited by double quotes, lets parse them off.
      */
      while (cBuffer && pBuffer[cBuffer-1] != '\"')
        cBuffer--;
      if (cBuffer)
        cBuffer--;
      while (cBuffer && *pBuffer != '\"')
        {
        cBuffer--;
        pBuffer++;
        }
      if (cBuffer)
        {
        cBuffer--;
        pBuffer++;
        }
      pBuffer[cBuffer-1] = '\0';

      /*
      ** Update the client name in the status line.
      */
      SendMessage(hChildStatus, UM_CLIENT, (WORD)pBuffer, 0L);

      sAiState = STATE_NULL;
      break;

    default:
      if (*pBuffer == '1' && cBuffer == 32)
        {
        if (memcmp(pBuffer, "1...5...10...15...20...25...30..", 32) == 0)
          {
	  sAiState = STATE_CLIENT;
	  SendMessage(hChildStatus, UM_CLIENT, 0, 0L);
          SendMessage(hChildStatus, UM_LIB, 0, 0L);
          bWorkLogonSent = TRUE;
          }
        }
      else if (*pBuffer == 'P' && cBuffer > 40)
        {
        if (memcmp(pBuffer, "Please type your personal identification", 40) == 0)
          {
	  SendMessage(hChildStatus, UM_CLIENT, 0, 0L);
          SendMessage(hChildStatus, UM_LIB, 0, 0L);
          }
        else if (memcmp(pBuffer, "Please TRANSMIT the NAME (only one) of the lib", 46) == 0)
          {
          sAiState = STATE_LIBRARY;
          SendMessage(hChildStatus, UM_LIB, 0, 0L);
          }
	else if (memcmp(pBuffer, "Please TRANSMIT the NAME (only one) of the fil", 46) == 0)
          {
	  sAiState = STATE_FILE;
          SendMessage(hChildStatus, UM_FILE, 0, 0L);
          }
        else if (!memcmp(pBuffer, "Please TRANSMIT, separated by co", 32))
          {
	  sAiState = STATE_FILE;
          SendMessage(hChildStatus, UM_FILE, 0, 0L);
          }
        else if (!memcmp(pBuffer, "Please TRANSMIT the abbreviated NAMES", 37))
          {
	  sAiState = STATE_FILE;
          SendMessage(hChildStatus, UM_FILE, 0, 0L);
          }
        else if (!memcmp(pBuffer, "Please TRANSMIT the NAME of the file ", 37))
          {
	  sAiState = STATE_FILE;
          SendMessage(hChildStatus, UM_FILE, 0, 0L);
          }
        else if (memcmp(pBuffer, "please log in:", 14) == 0)
          {
          SessTellDisconnect(FALSE);
          }
#ifdef HYPERROM
	else if (!memcmp(pBuffer, "Please transmit the private File pass", 37))
          {
	  CommWriteChar('\x02');
	  CommWriteData(szWorkPassword, -1);
	  CommWriteChar('\r');
	  InfoMessage(IDS_INFO_PASSWORDP, NULL);
          }
        else if (!memcmp(pBuffer, "Please transmit the private Library pass", 40))
          {
	  CommWriteChar('\x02');
	  CommWriteData(szWorkPassword, -1);
	  CommWriteChar('\r');
	  InfoMessage(IDS_INFO_PASSWORDP, NULL);
          }
#endif
        }
      else if (*pBuffer == 'L' && cBuffer > 16)
        {
        if (memcmp(pBuffer, "LEXSEE is working on the display", 32) == 0)
          {
          SendMessage(hChildStatus, UM_SERVICE, (WORD)"LEXSEE", 0L);
          }
        }
      else if (*pBuffer == 'N' && cBuffer > 16)
        {
        if (memcmp(pBuffer, "NOTE:  Your Client Information h", 32) == 0)
          sAiState = STATE_ECLIPSE;
        }
      else if (*pBuffer == ' ' && cBuffer > 14)
        {
        if (memcmp(pBuffer, "     CLIENT:   ", 14) == 0)
          {
          SendMessage(hChildStatus, UM_CLIENT, (WORD)(&pBuffer[14]), 0L);
          }
        else if (memcmp(pBuffer, "    LIBRARY:   ", 14) == 0)
          {
          SendMessage(hChildStatus, UM_LIB, (WORD)(&pBuffer[14]), 0L);
          }
        else if (memcmp(pBuffer, "       FILE:   ", 14) == 0)
          {
          SendMessage(hChildStatus, UM_FILE, (WORD)(&pBuffer[14]), 0L);
          }
        }
      else if (*pBuffer == 'I' && cBuffer >= 18)
        {
        if (memcmp(pBuffer, "INACTIVITY TIMEOUT", 18) == 0)
          {
          nTermChrX = 1;
          nTermChrY = 0;
          VidClearPage();

	  SessTellDisconnect(FALSE);  // Was True
          }
        else if (memcmp(pBuffer, "Inactivity Timeout, please log in:", 34) == 0)
          {
          nTermChrX = 1;
          nTermChrY = 0;
          VidClearPage();

	  SessTellDisconnect(FALSE);  // Was True
          }
	else if (memcmp(pBuffer, "If you want to continue this research, press the Y key", 52) == 0)
	  {
          if (fWorkHyperRomSwitch)
	    {
	    CommWriteData("\x02Y\r", -1);
            fWorkHyperRomSwitch = FALSE;
	    }
          }
        }
      else if (*pBuffer == '5' && cBuffer >= 18)
        {
        if (memcmp(pBuffer, "513 30 DISCONNECT", 17) == 0)
          {
	  SessTellDisconnect(FALSE);  // Was True
          }
        }
      else if (*pBuffer == 'n' && cBuffer >= 18)
        {
        if (memcmp(pBuffer, "network: call cleared", 21) == 0)
          {
          SessTellDisconnect(FALSE);
          }
        else if (memcmp(pBuffer, "network call cleared/please log in:", 35) == 0)
          {
          SessTellDisconnect(FALSE);
          }
        }
      else if (*pBuffer == 'T' && cBuffer >= 45)
        {
        if (memcmp(pBuffer, "To exit communication with MDC Services", 39) == 0)
          {
	  SessTellDisconnect(FALSE);  // Was True
          }
        }
    }
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL CommInit();                                                          */
/*									      */
/*  Called in the application's initialize routine so we can get everything   */
/*  in order.  Returns true is successful (for future possiblities).          */
/*									      */
/*----------------------------------------------------------------------------*/
void AIscanOutLine(pBuffer, cBuffer)
  PBYTE pBuffer;
  SHORT cBuffer;
{

//{
//char buf[64];
//
//sprintf(buf, "[%d,", sAiState);
//write(3, buf, strlen(buf));
//write(3, pBuffer, cBuffer);
//write(3, "]\r\n", 3);
//}

  switch (sAiState)
    {
    case STATE_LOGON:
      break;

    case STATE_NULL:
      NewCommand(pBuffer, cBuffer);
      break;

    case STATE_CLIENT:
      if (pBuffer[0] == '.' && cBuffer)
        break;
      SendMessage(hChildStatus, UM_CLIENT, (WORD)pBuffer, 0L);
      sAiState = STATE_LIBRARY;
      break;

    case STATE_LIBRARY:
      if (NewCommand(pBuffer, cBuffer))
	sAiState = STATE_NULL;
      else if (cBuffer > 1 && *pBuffer != '.' && !isdigit(*pBuffer))
        {
        SendMessage(hChildStatus, UM_LIB, (WORD)pBuffer, 0L);
	sAiState = STATE_FILE;
        }
      break;

    case STATE_FILE:
      if (NewCommand(pBuffer, cBuffer))
	sAiState = STATE_NULL;
      else if (cBuffer > 1 && *pBuffer !=  '.' && !isdigit(*pBuffer))
        {
        SendMessage(hChildStatus, UM_FILE, (WORD)pBuffer, 0L);
	sAiState = STATE_NULL;
        }
      break;
    }
}


/******************************************************************************/
/*									      */
/*  Local Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*----------------------------------------------------------------------------*/

static BOOL near NewCommand(PBYTE pBuffer, SHORT cBuffer)
  {
  char buf[256];
  BOOL fCommand;
  register i;


  strcpy(buf, pBuffer);
  strupr(buf);
  fCommand = FALSE;


  /*
  if (buf[0] == 'C')
    {
    if (cBuffer == 1)
      {
      sAiState = STATE_CLIENT;
      fCommand = TRUE;
      }
    }
  else if (buf[0] == '.')
    {
    if (buf[1] == 'C')
      {
      if (buf[2] == 'L')
        {
	sAiState = STATE_LIBRARY;
        fCommand = TRUE;
        }
      else if (buf[2] == 'F')
        {
	sAiState = STATE_FILE;
        fCommand = TRUE;
        }
      else
        {
	sAiState = STATE_NULL;
        fCommand = TRUE;
        }
      }
    else
      {
      sAiState = STATE_NULL;
      fCommand = TRUE;
      }
    }
  */

  if (buf[0] == '.')
    {
    sAiState = STATE_NULL;
    fCommand = TRUE;
    }

  if (memicmp(pBuffer, "RESUME ", 7) == 0)
    {
    cBuffer = strlen(pBuffer);
    }
  else
    {
    for (i = 0; i < cBuffer; i++)
      {
      if (pBuffer[i] == ' ')
        {
        cBuffer = i;
        break;
        }
      }
    }

  for (i = COMMAND_FIRST; i <= COMMAND_LAST; i++)
    {
    if (cBuffer == strlen(Commands[i].szCommand))
      if (memicmp(pBuffer, Commands[i].szCommand, cBuffer) == 0)
        break;
    }

  if (i == COMMAND_AUTOCITE)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"AC", 0L);
  else if (i == COMMAND_LEXSEE)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"LEXSEE", 0L);
  else if (i == COMMAND_LXSTAT)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"LEXSTAT", 0L);
  else if (i == COMMAND_SHEPARD)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"SHEP", 0L);
  else if (i == COMMAND_EXITSERVICE)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"", 0L);
  else if (i == COMMAND_RESUME)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"", 0L);
  else if (i == COMMAND_RESUMELEXIS)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"", 0L);
  else if (i == COMMAND_RESUMEAUTOCITE)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"AC", 0L);
  else if (i == COMMAND_RESUMESHEPARD)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"SHEP", 0L);
  else if (i == COMMAND_RESUMELEXSEE)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"LEXSEE", 0L);
  else if (i == COMMAND_RESUMELXSTAT)
    SendMessage(hChildStatus, UM_SERVICE, (WORD)"LEXSTAT", 0L);

  if (i <= COMMAND_LAST && !fCommand)
    {
    sAiState = STATE_NULL;
    fCommand = TRUE;
    }

  return (fCommand);
  }
