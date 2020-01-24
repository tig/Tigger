/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Robot Automatic Data Retrieval Interface (ROBOT.C)               */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: This module is use to simulate a computer interface under the    */
/*           Lexis/Nexis environment.  The robot processor is given a task    */
/*           to retrieve data from multible pages and store them in a file.   */
/*           Currently, only the DDE interface will make calls to the robot   */
/*           module, but future versions of Lexis may support user requests.  */
/*									      */
/******************************************************************************/

#include <windows.h>
#include <direct.h>
#include <dos.h>
#include <errno.h>
#include <string.h>
#include "lexis.h"
#include "robot.h"
#include "session.h"
#include "term.h"
#include "data.h"
#include "library.h"


/******************************************************************************/
/*									      */
/*  Global Variables							      */
/*									      */
/******************************************************************************/

  BOOL  fRobotWorking;          /* TRUE if robot processor is running.      */
  LONG  lRobotTimeout;          /* Used to timeout if nothing is from MDC.  */
  char  szRobotStatus[8];       /* Status of last ROBOT command.            */


/******************************************************************************/
/*									      */
/*  Local Constants                                                           */
/*									      */
/******************************************************************************/

#define STATE_FIRSTPAGE 0       /* usRobotState set to if on first page.    */
#define STATE_NEXTPAGE  1       /* usRobotState set to if on next page.     */


/******************************************************************************/
/*									      */
/*  Local Variables                                                           */
/*									      */
/******************************************************************************/

  static HFILE  hRobotFile;
  static USHORT usCurPage;
  static USHORT usMaxPages;
  static USHORT usRobotCommand;
  static USHORT usRobotState;
  static char   szRobotArg[64];

/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL RobotReset();                                                        */
/*									      */
/*  Called in the application's initialize routine so we can get everything   */
/*  in order.                                                                 */
/*									      */
/*----------------------------------------------------------------------------*/
void RobotReset()
  {
  fRobotWorking = FALSE;
  strcpy(szRobotStatus, "Ok");
  hRobotFile = NULL;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL RobotTerm();                                                         */
/*									      */
/*  Called in the application's termination routine so we can wrap up nicely. */
/*									      */
/*----------------------------------------------------------------------------*/
void RobotTerm()
  {
  if (hRobotFile)
    DosClose(hRobotFile);

  fRobotWorking = FALSE;
  hRobotFile = NULL;
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL RobotDdeCommand()                                                    */
/*									      */
/*  Parse and process the robot execute request from the DDE logic module.    */
/*  The command send via DDE is "command,arg,path,max-pages)".  Return true   */
/*  if the syntax is ok and the system is ready from more work.               */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL RobotDdeCommand(PSZ pszArg)
  {
  USHORT usCommand;
  char   szCmd[64], szArg[64], szPath[64];
  USHORT usPages;
  int    hFile;
  register i;


  if (fRobotWorking)
    return (FALSE);

  strupr(pszArg);
  usPages = 0;

  for (i = 0; *pszArg != ',' && *pszArg; i++)
    szCmd[i] = *pszArg++;
  szCmd[i] = '\0';
  if (*pszArg)
    pszArg++;

  if (strcmp("SIGNON", szCmd) == 0)
    usCommand = ROBOT_SIGNON;
  else if (strcmp("CLIENT", szCmd) == 0)
    usCommand = ROBOT_CLIENT;
  else if (strcmp("AUTOCITE", szCmd) == 0)
    usCommand = ROBOT_AUTOCITE;
  else if (strcmp("LEXSEE", szCmd) == 0)
    usCommand = ROBOT_LEXSEE;
  else if (strcmp("LEXSTAT", szCmd) == 0)
    usCommand = ROBOT_LEXSTAT;
  else if (strcmp("SHEPARD", szCmd) == 0)
    usCommand = ROBOT_SHEPARD;
  else
    return (FALSE);

  for (i = 0; *pszArg != ',' && *pszArg; i++)
    szArg[i] = *pszArg++;
  szArg[i] = '\0';
  if (*pszArg)
    pszArg++;

  if (szArg[0] == '\0')
    return (FALSE);

  if (usCommand != ROBOT_SIGNON && usCommand != ROBOT_CLIENT)
    {
    for (i = 0; *pszArg != ',' && *pszArg; i++)
      szPath[i] = *pszArg++;
    szPath[i] = '\0';
    if (*pszArg)
      pszArg++;

    if (!ValidatePath(szPath))
      return (FALSE);

    while (*pszArg >= '0' && *pszArg <= '9')
      usPages += usPages * 10 + *pszArg++ - '0';

    if (usPages > 1000)
      return (FALSE);

    if (_dos_creat(szPath, _A_NORMAL, &hFile))
      return (FALSE);

    if (!RobotCommand(usCommand, szArg, hFile, usPages))
      {
      DosClose(hFile);
      return (FALSE);
      }
    }
  else
    {
    if (!RobotCommand(usCommand, szArg, NULL, NULL))
      return (FALSE);
    }

  return (TRUE);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL RobotCommand();                                                      */
/*									      */
/*  If the Robot Processor is idle, start a new automatic data retrieval task */
/*  to store the requested data into the specified file.  The routine will    */
/*  return TRUE if the command has started successfully.                      */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL RobotCommand(USHORT usCommand, PSZ pszArg, HFILE hFile, USHORT usPages)
  {
  char szCmd[64];


  if (fRobotWorking)
    return (FALSE);

  fRobotWorking = TRUE;
  lRobotTimeout = GetCurrentTime();
  strcpy(szRobotStatus, "Ok");
  hRobotFile    = hFile;
  usCurPage     = 0;
  usMaxPages    = usPages;
  usRobotState  = STATE_FIRSTPAGE;

  if (usMaxPages == 0)
    usMaxPages = 20;

  switch (usRobotCommand = usCommand)
    {
    case ROBOT_SIGNON:
      strcpy(szRobotArg, pszArg);
      if (fSessOnline)
        return (FALSE);
      CmdSignon();
      break;

    case ROBOT_CLIENT:
      strcpy(szRobotArg, pszArg);
      SessTellGeneric("c", 1, "L2KIXE Client Command");
      break;

    case ROBOT_AUTOCITE:
      strcpy(szCmd, ".ss;.es;.ac ");
      strcat(szCmd, pszArg);
      SessTellGeneric(szCmd, strlen(szCmd), "L2KIXE AutoCite Command");
      break;

    case ROBOT_LEXSTAT:
      strcpy(szCmd, ".ss;.es;LXSTAT ");
      strcat(szCmd, pszArg);
      SessTellGeneric(szCmd, strlen(szCmd), "L2KIXE LEXSTAT Command");
      break;

    case ROBOT_LEXSEE:
      strcpy(szCmd, ".ss;.es;LEXSEE ");
      strcat(szCmd, pszArg);
      SessTellGeneric(szCmd, strlen(szCmd), "L2KIXE LEXSEE Command");
      break;

    case ROBOT_SHEPARD:
      strcpy(szCmd, ".ss;.es;SHEP ");
      strcat(szCmd, pszArg);
      SessTellGeneric(szCmd, strlen(szCmd), "L2KIXE Shepard's Command");
      break;
    }

  return (TRUE);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL RobotKeyboardUnlock();                                               */
/*									      */
/*  This routine is called anytime the keyboard is unlocked by the LEXIS      */
/*  system.  This tells us that the screen is full and the data flow has      */
/*  stopped.  The routine will process the screen and issue another command   */
/*  to either continue the robot process or terminate it.  This routine       */
/*  returns FALSE if the robot process is not running.                        */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL RobotKeyboardUnlock()
  {
  USHORT usFirst, usLast;
  USHORT usBytes;
  char   szLine[128];


  if (!fRobotWorking)
    return (FALSE);

  /*
  **  MDC is still talking to us to reset the robot timeout value.
  */
  lRobotTimeout = GetCurrentTime();

  if (usCurPage >= usMaxPages)
    {
    DosWrite(hRobotFile, "\r\n*** MaxPages reached ***\r\n", 28, &usBytes);
    SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
    RobotTerm();
    return (FALSE);
    }

  switch (usRobotCommand)
    {
    case ROBOT_SIGNON:
      TermQueryLine(1, szLine);
      if (memcmp(szLine, "1...5...10", 10) == 0)
        {
        SessTellGeneric(szRobotArg, strlen(szRobotArg), "L2KIXE Client");
        usRobotCommand = ROBOT_CLIENT;
        break;
        }
      TermQueryLine(2, szLine);
      if (memcmp(&szLine[29], "LIBRARIES", 9) == 0)
        {
        RobotTerm();
        }
      else
        {
        SessTellGeneric("n", 1, "L2KIXE No Command");
        }
      break;

    case ROBOT_CLIENT:
      if (usCurPage == 0)
        SessTellGeneric(szRobotArg, strlen(szRobotArg), "L2KIXE Client");
      else
        RobotTerm();
      break;

    case ROBOT_AUTOCITE:
      {
      register i;

      if (usCurPage == 0)
        usLast = 20;
      else
        usLast = 21;
      TermQueryLine(usLast, szLine);
      if (memcmp(szLine, " AUTO-CITE information continues", 32) == 0)
        {
        register j;

        if (usCurPage == 0)
          {
          usFirst = 0;
          usLast--;
          }
        else
          usFirst = 4;

        for (i = usFirst; i < usLast; i++)
          {
          j = TermQueryLine(i, szLine);
          DosWrite(hRobotFile, szLine, j, &usBytes);
          DosWrite(hRobotFile, "\r\n", 2, &usBytes);
          }

        if (j && szLine[j-1] == '.')
          DosWrite(hRobotFile, "\r\n", 2, &usBytes);

        SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
        }
      else if (memcmp(szLine, "To check another citation, type ", 32) == 0)
        {
        register j;

        if (usCurPage == 0)
          usFirst = 0;
        else
          usFirst = 4;

        for (i = usFirst; i < 21; i++)
          {
          j = TermQueryLine(i, szLine);
          if (memcmp(szLine, "To search for collateral annotat", 32) == 0)
            break;

          DosWrite(hRobotFile, szLine, j, &usBytes);
          DosWrite(hRobotFile, "\r\n", 2, &usBytes);
          }

        SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");

        RobotTerm();
        }
      else
        {
        if (usCurPage == 0)
          strcpy(szRobotStatus, "Error");

        SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
        RobotTerm();
        }
      }
      break;

    case ROBOT_LEXSTAT:
      if (usCurPage == 0)
        {
        TermQueryLine(2, szLine);
        if (memcmp(szLine, "----------", 10) == 0)
          {
          register i, j;

          for (i = 3; i < 23; i++)
            {
            j = TermQueryLine(i, szLine);
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          if (j = TermQueryLine(23, szLine))
            {
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
          }
        else
          {
          strcpy(szRobotStatus, "Error");
          SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
          RobotTerm();
          }
        }
      else
        {
        TermQueryLine(0, szLine);
        if (memcmp(&szLine[72], "LEXSTAT", 7) == 0)
          {
          register i, j;

          for (i = 1; i < 23; i++)
            {
            j = TermQueryLine(i, szLine);
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          if (j = TermQueryLine(23, szLine))
            {
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
          }
        else
          {
          SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
          RobotTerm();
          }
        }
      break;

    case ROBOT_LEXSEE:
      if (usCurPage == 0)
        {
        TermQueryLine(23, szLine);
        if (memcmp(szLine, "For further expl", 16) != 0)
          {
          register i, j;

          for (i = 0; i < 23; i++)
            {
            j = TermQueryLine(i, szLine);
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          if (j = TermQueryLine(23, szLine))
            {
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
          }
        else
          {
          strcpy(szRobotStatus, "Error");
          SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
          RobotTerm();
          }
        }
      else
        {
        TermQueryLine(0, szLine);
        if (memcmp(&szLine[74], "LEXSEE", 7) == 0)
          {
          register i, j;

          for (i = 1; i < 23; i++)
            {
            j = TermQueryLine(i, szLine);
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          if (j = TermQueryLine(23, szLine))
            {
            DosWrite(hRobotFile, szLine, j, &usBytes);
            DosWrite(hRobotFile, "\r\n", 2, &usBytes);
            }

          SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
          }
        else
          {
          SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
          RobotTerm();
          }
        }
      break;

    case ROBOT_SHEPARD:
      TermQueryLine(2, szLine);
      if (memcmp(szLine, "CITATIONS TO:", 13) == 0)
        {
        register i, j;

        for (i = 0; i < 24; i++)
          {
          j = TermQueryLine(i, szLine);
          if (j > 16 && memcmp(szLine, "----------------", 16) == 0)
            break;
          DosWrite(hRobotFile, szLine, j, &usBytes);
          DosWrite(hRobotFile, "\r\n", 2, &usBytes);
          }

        SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
        break;
        }

      TermQueryLine(3, szLine);
      if (memcmp(szLine, "NUMBER  ANALYSIS", 16) == 0)
        {
        register i, j;

        for (i = 5; i < 24; i++)
          {
          j = TermQueryLine(i, szLine);
          if (j > 16 && memcmp(szLine, "----------------", 16) == 0)
            break;
          DosWrite(hRobotFile, szLine, j, &usBytes);
          DosWrite(hRobotFile, "\r\n", 2, &usBytes);
          }

        SessTellGeneric(".np", 3, "L2KIXE NextPage Command");
        break;
        }

      if (usCurPage == 0)
        strcpy(szRobotStatus, "Error");

      SessTellGeneric(".es", 3, "L2KIXE Exit Service Command");
      RobotTerm();
      break;
    }

  usCurPage++;

  return (fRobotWorking);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void RobotTimeoutCheck()                                                  */
/*									      */
/*  This routine is called by the main WM_TIMER loop when fRobotWorking is    */
/*  true.  If no response has been received from the MDC mainframe within the */
/*  allocated time, then timeout and set the status to an error.              */
/*									      */
/*----------------------------------------------------------------------------*/
void RobotTimeoutCheck()
  {
  if (!fRobotWorking)
    return;

  if (GetCurrentTime() > (lRobotTimeout + 5L * 60L * 1000L))
    {
    strcpy(szRobotStatus, "Timeout");
    SessTellGeneric(".so;.ss;.es", 11, "L2KIXE Timeout Command");
    RobotTerm();
    }
  }
