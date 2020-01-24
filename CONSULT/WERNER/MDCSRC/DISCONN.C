/*****************************************************************************
* FILE NAME:  disconnect.c                                                   *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   dosconnect from the OLS.                                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Disconnect - Disconnects from the OLS                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Disconnect ();                                                  *
*                                                                            *
*   INT  status        - returned INT value                                  *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   CONNECTDATA   - General info about connection to OLS                     *
*   OLDATA        - General info about online session from setup.dat         *
*                                                                            *
*                                                                            *
* PSEUDO-CODE                                                                *
*                                                                            *
*   execute script (Abort_Script)                                            *
*   free abort script                                                        *
*   close device                                                             *
*   cleanup                                                                  *
*   return status                                                            *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/03/90                                                            *
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
 *  $Log:   N:/src/disconn.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:54:04   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:15:56   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:20:22   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:04:12   andrews
 * No change.
 * 
 *    Rev 4.1   05 Oct 1990 11:33:36   deanm
 * updating comments
 * 
 *    Rev 4.0   01 Oct 1990 13:22:10   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.1   20 Sep 1990 11:37:54   deanm
 * fixing some disconnect problems
 * 
 *    Rev 3.0   17 Sep 1990 10:03:54   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:17:52   andrews
 * iteration 1
 * 
 *    Rev 0.7   22 Aug 1990 10:31:02   joeb
 * set online to false
 * 
 *    Rev 0.6   21 Aug 1990 10:26:26   deanm
 * fixing return codes
 * 
 *    Rev 0.5   21 Aug 1990  8:58:34   andrews
 * free abort script always
 * added Except_Delete
 * 
 *    Rev 0.4   14 Aug 1990 17:07:04   deanm
 * fixing up online
 * 
 *    Rev 0.3   10 Aug 1990  8:16:14   deanm
 * added exptions.h to list of include files 
 * 
 *    Rev 0.2   03 Aug 1990 16:58:00   deanm
 * added connectInfo to disconnect so that null ptr was not used
 * 
 *    Rev 0.1   30 Jul 1990  9:22:04   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:24   deanm
 * Initial revision.
 *
*/





#include "globals.h"
#include "lltool.h"
#include "disconn.h"
#include "connect.h"
#include "cscript.h"
#include "devcomm.h"
#include "except.h"
#include "exptions.h"
#include "systool.h"




INT  Disconnect ()


   {

   INT  err;

   CONNECTDATA connectInfo;

   Sys_setOnline (FALSE);


/* if we have not dialed, do not bother sending the abort script */
   if (Phone_Flag == FALSE)
      {
      Cscript_Free (&AbortScript);
      Dev_Flush ();
      Dev_Close ();
      return (SUCCESS);
      }


/* prepare a connectInfo for abort script */
   connectInfo.debug              = FALSE;
   connectInfo.dialPrefix     [0] = '\0';   
   connectInfo.phoneNumber    [0] = '\0';   
   connectInfo.WANname        [0] = '\0';   
   connectInfo.WANnode        [0] = '\0';   
   connectInfo.WANnodeAddress [0] = '\0';   
   connectInfo.WANpassword    [0] = '\0';   

   Except_Add ( Conn_Except );

   Dev_Flush ();


/* execute the abort script */
   err = Cscript_Do (&AbortScript, &connectInfo, NULL, NULL);
   if (err != SUCCESS  &&  err != QUIT  &&  err != ESCAPE)
      {
      Except_Raise (err, NULL);
      }

   Cscript_Free (&AbortScript);

   Except_Delete();

   Dev_Close ();

   return (err);
   }
   
