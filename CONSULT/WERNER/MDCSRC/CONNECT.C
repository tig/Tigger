/*****************************************************************************
* FILE NAME:  connect.c                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Connect to the OLS.                                                      *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Connect       - Connects to the OLS                                      *
*                                                                            *
*                                                                            *
*   (The following are private functions.)                                   *
*                                                                            *
*   Conn_SetupInit - Init structures from setup.dat                          *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   CONNECTDATA   - General info about connection to OLS                     *
*   OLDATA        - General info about online session from setup.dat         *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/03/90                                                            *
*        08/08/90 moved Conn_Except to exptions.c                            *
*                                                                            *
* =========================================================================  *
*           M E A D   D A T A   C E N T R A L ,   I N C .                    *
*                          CONFIDENTIAL                                      *
*                                                                            *
* This document is property of Mead Data Central, Inc. (MDC) and its         *
* contents are proprietary to MDC.  Reproduction in any form by anyone       *
* of the materials combined herein without the permission of MDC is          *
* prohibited.  Finders are asked to return this document to Mead Data        *
* Central, Inc. at P.O. BOX 933, Dayton, OH 45401.                           *
*                                                                            *
*****************************************************************************/

/*
 *  $Log:   N:/src/connect.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:53:58   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:15:48   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:20:14   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.1   24 Oct 1990 10:34:40   vincentt
 * fixed internal error message when alt-q from true netbios
 * 
 *    Rev 5.0   24 Oct 1990  9:04:06   andrews
 * No change.
 * 
 *    Rev 4.1   02 Oct 1990  9:40:08   deanm
 * fixing up irs comments
 * 
 *    Rev 4.0   01 Oct 1990 13:22:04   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.2   29 Sep 1990 17:05:12   joeb
 * OLS change insulation
 * 
 *    Rev 3.1   20 Sep 1990 11:38:36   deanm
 * adding some lines to handle scripting better
 * 
 *    Rev 3.0   17 Sep 1990 10:03:48   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:17:44   andrews
 * iteration 1
 * 
 *    Rev 0.9   24 Aug 1990 16:15:36   deanm
 * do not read the lexisid out of the setup.dat
 * 
 *    Rev 0.8   22 Aug 1990  9:40:20   deanm
 * adding real copy of lexis id in userdefs
 * 
 *    Rev 0.7   21 Aug 1990 11:00:38   andrews
 * better exception handling
 * clean up search scripts and exception handlers if errors occur
 * 
 *    Rev 0.6   21 Aug 1990  8:25:38   deanm
 * adding print init to connect
 * 
 *    Rev 0.5   14 Aug 1990 17:07:00   deanm
 * fixing up online
 * 
 *    Rev 0.4   08 Aug 1990 15:02:08   deanm
 * removed Conn_Except function
 * 
 *    Rev 0.3   06 Aug 1990 17:03:56   andrews
 * added support for Except_DBox
 * 
 *    Rev 0.2   03 Aug 1990 16:54:12   deanm
 * finishing connect
 * 
 *    Rev 0.1   30 Jul 1990  9:22:02   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:20   deanm
 * Initial revision.
 *
*/




#include <string.h>
#include <stdio.h>
#include "globals.h"
#include "connect.h"
#include "lltool.h"
#include "cscript.h"
#include "devcomm.h"
#include "devdata.h"
#include "except.h"
#include "exptions.h"
#include "texttool.h"
#include "prnttool.h"


/*****************************************************************************
* FUNCTION NAME:  Connect                                                    *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Connect to the OLS                                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Connect (path, onlineInfo, scriptFctPtr, lineFctPtr, debug);    *
*                                                                            *
*   INT  status        - returned INT value                                  *
*                                                                            *
*   CHAR *path             - DOS path to find setup.dat and scripts.dat files*
*   OLDATA *onlineInfo     - to be filled in with online info from .dat files*
*   INT  (*scriptFctPtr)() - Function pointer to call before cscript section *
*   INT  (*lineFctPtr)()   - Function pointer to call after cscript line     *
*   BOOLEAN debug          - TRUE if debug mode, FALSE if normal mode        *
*                                                                            *
*                                                                            *
* PSEUDO-CODE                                                                *
*                                                                            *
*   init from setup.dat                                                      *
*   read comm script                                                         *
*   initialize comm device                                                   *
*   open comm device                                                         *
*   if (not holding abort script)                                            *
*      send abort script                                                     *
*   execute comm script                                                      *
*   free comm script                                                         *
*   cleanup                                                                  *
*   return status                                                            *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, RETRY, or EXIT, or ESCAPE                                       *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/05/90                                                            *
*                                                                            *
*****************************************************************************/




INT  Connect (path, onlineInfo, scriptFctPtr, lineFctPtr, debug)

CHAR     *path;
OLDATA   *onlineInfo;
INT      (*scriptFctPtr) (void);
INT      (*lineFctPtr)   (void);
BOOLEAN  debug;

   {
   CONNECTDATA  connectInfo;
   CSCRIPT      script;
   INT          speed;
   INT          status;
   CHAR         line [COLS + 1];


   connectInfo.debug = debug;

   Except_Add (Conn_Except);


/* read info out of setup.dat */
   if ((status = Conn_SetupInit (path, &connectInfo, onlineInfo)) != SUCCESS)
      {
      strcpy (line, path);
      strcat (line, "SETUP.DAT");
      status = Except_Raise (status, line);
      Except_Delete();
      return(status);
      }




/* set actual speed value (determine from set speed and max speed) */
   if (connectInfo.maxSpeed < connectInfo.speed)
      {
      speed = connectInfo.maxSpeed;
      }
   else
      { 
      speed = connectInfo.speed;
      }


/* initialize comm device */
   status = Dev_Init (connectInfo.device,     connectInfo.commPort,  speed,
                      connectInfo.parity,     connectInfo.checkCD,
                      connectInfo.servName,   connectInfo.targetName,
                      connectInfo.dialPrefix, connectInfo.phoneNumber);
   if (status != SUCCESS)
      {
      status = Except_Raise (status, NULL);
      Except_Delete();
      return(status);
      }

// if we are opening through netbios, we must call the script function here
   if (connectInfo.device == DEV_NETBIOS)
      {
      scriptFctPtr ();
      }

/* open up the device (check return code) */
   status = Dev_Open ();
   if (status == ESCAPE || status == QUIT)
      {
      return (status);
      }

   if (status != SUCCESS)
      {
      status = Except_Raise (status, NULL);
      Except_Delete();
      return(status);
      }


/* now read the dial, logon, and signon scripts */
   if ((status = Cscript_Read (&script, &connectInfo, path)) != SUCCESS)
      {
      Except_Delete();                          /* exceptions are taken */
      Cscript_Free (&script);                   /* care of in Cscript_Read */
      return(status);
      }


/* execute the comm script */
   Phone_Flag = FALSE;
   status = Cscript_Do (&script, &connectInfo, scriptFctPtr, lineFctPtr);
   if (status != SUCCESS && status != QUIT && status != ESCAPE)
      {
      status = Except_Raise (status, NULL);
      }

   
/* all done */
   Cscript_Free (&script);
   

   Except_Delete ();
   return (status);
   }





/*****************************************************************************
* FUNCTION NAME:  Conn_SetupInit                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   setup initialization for connect                                         *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Conn_SetupInit (path, connectInfo, onlineInfo)                  *
*                                                                            *
*   INT  status - returned function status                                   *
*                                                                            *
*   CHAR        *path        - path of setup.dat                             *
*   CONNECTDATA *connectInfo - connection information                        *
*   OLDATA      *onlineInfo  - online information                            *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Conn_SetupInit reads setup.dat for the connection and online info.       *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or ESCAPE.                                                       *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/05/90                                                            *
*                                                                            *
*****************************************************************************/
 




INT  Conn_SetupInit (path, connectInfo, onlineInfo)

CHAR        *path;
CONNECTDATA *connectInfo;
OLDATA      *onlineInfo;

   {
   FILE     *setfile;
   CHAR     line [COLS + 1];
   CHAR     *temp;
   INT      status;
   INT      i;
   INT      printerSpeed;
   INT      printerParity;
   CHAR     printerControl [45];
   CHAR     junk_ruler [33];   
   INT      junk_rulerRow;   
   INT      junk_rulerCol;   


/* open setup.dat */
   strcpy (line, path);
   strcat (line, "setup.dat");
   if ((setfile = fopen (line, "r")) == NULL)
      {
      return (OPEN_ERROR);
      }


/* get Primary Network's configuration */
   if  ((status = get_chinfo (setfile, connectInfo->WANname)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->servName)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->targetName)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->dialPrefix)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->phoneNumber)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &connectInfo->speed)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->WANnode)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->WANnodeAddress)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->WANpassword)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->ALF)) != SUCCESS)
      {
      return (status);
      }


/* throw away 1st alt. and 2nd alt. networks */
   do
      {
      if  ((status = get_chinfo (setfile, line)) != SUCCESS)
         {
         return (status);
         }
      }  while (*line != '/');
      

/* get comm port information */
   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }
   temp = strchr (line, 'M');
   temp++;
   connectInfo->commPort = *temp - '1';  /* DEV_COM1 thru DEV_COM4 */
   

   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &connectInfo->device)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &connectInfo->maxSpeed)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &connectInfo->parity)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &connectInfo->dialMethod)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, connectInfo->ADF)) != SUCCESS)
      {
      return (status);
      }

/* advance to next section */
   do
      {
      if  ((status = get_chinfo (setfile, line)) != SUCCESS)
         {
         return (status);
         }
      }  while (*line != '/');
      

/* get printer information */
   if  ((status = get_chinfo (setfile, UserDefs.pr_name)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, UserDefs.pr_location)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &printerSpeed)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, printerControl)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &printerParity)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &UserDefs.lfeed)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &UserDefs.lwrap)) != SUCCESS)
      {
      return (status);
      }

   if ((status = Print_Init (UserDefs.pr_name, printerControl,
                             UserDefs.prtr_start, UserDefs.prtr_end,
                             printerSpeed, printerParity)) != SUCCESS)
      {
      return (status);
      }
 

/* get to next section */
   do
      {
      if  ((status = get_chinfo (setfile, line)) != SUCCESS)
         {
         return (status);
         }
      }  while (*line != '/');
      
      

/* get the lexis ID out of setup.dat (put it in UserDefs if necessary) */
   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }
//   if (UserDefs.lexisID[0] == '\0')
//      {
//      strncpy (UserDefs.lexisID, line, 7);
//      UserDefs.lexisID[7] = '\0';
//      }


/* throw out the beep from setup.dat */
   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }


/* throw out a few lines */
   for (i = 0; i < 9; i++)
      {
      if  ((status = get_chinfo (setfile, line)) != SUCCESS)
         {
         return (status);
         }
      }
   

/* get session rec., doc to disk, and keyboard info */
   if  ((status = get_chinfo (setfile, onlineInfo->sessRec)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &UserDefs.tobasco)) != SUCCESS)
      {
      return (status);
      }

   if  ((status = get_chinfo (setfile, onlineInfo->docToDsk)) != SUCCESS)
      {
      return (status);
      }

/* get to next section */
   do
      {
      if  ((status = get_chinfo (setfile, line)) != SUCCESS)
         {
         return (status);
         }
      }  while (*line != '/');
      

   if  ((status = get_chinfo (setfile, line)) != SUCCESS)
      {
      return (status);
      }

   // DISCARD ALL RULER INFORMATION FROM SETUP.DAT  (it will come from research.dat)  
   if  ((status = get_chinfo (setfile, junk_ruler)) != SUCCESS) // toss this 
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &junk_rulerRow)) != SUCCESS) // toss this 
      {
      return (status);
      }

   if  ((status = get_intinfo (setfile, &junk_rulerCol)) != SUCCESS) // toss this 
      {
      return (status);
      }

/* get CD check status */
   if  ((status = get_intinfo (setfile, &i)) != SUCCESS)
      {
      return (status);
      }

   onlineInfo->checkCD = connectInfo->checkCD = (BOOLEAN) i;


   fclose (setfile);

   return (SUCCESS);
   }
