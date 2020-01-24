/*****************************************************************************
* FILE NAME:  novell.c                                                       *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Novell                                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NovellInit    - Initialize comm device                                   *
*   NovellOpen    - Open comm device                                         *
*   NovellClose   - Close comm device                                        *
*   NovellReadCh  - Read a character from the comm device                    *
*   NovellReadStr - read a string from the comm device                       *
*   NovellWriteCh - Write a character to the comm device                     *
*   NovellWriteStr- Write a string to the comm device                        *
*   NovellFlush   - Flush the input and ouput of the comm device             *
*   NovellLineCheck - Check the phone line to see if still active            *
*   NovellCharReady - Check to see if a character is ready to be read        *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
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


/***************************************************************************
 *                                                                         *
 *                       Modification Log                                  *
 *  $Log:   N:/src/novell.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:54:48   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:16:28   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:21:02   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:04:54   andrews
 * No change.
 * 
 *    Rev 4.3   15 Oct 1990 15:15:12   deanm
 * fixing up disconnect and char ready routines
 * 
 *    Rev 4.2   05 Oct 1990 15:33:04   deanm
 * updating comments
 * 
 *    Rev 4.1   03 Oct 1990 21:06:32   deanm
 * fixed check of registers for multiple versions of ncsi
 * 
 *    Rev 4.0   01 Oct 1990 13:22:46   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.1   20 Sep 1990 11:37:20   deanm
 * changing line checking algorithms
 * 
 *    Rev 3.0   17 Sep 1990 10:04:28   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:18:38   andrews
 * iteration 1
 * 
 *    Rev 0.3   25 Aug 1990 18:44:42   deanm
 * cleaning up novell and int14
 * 
 *    Rev 0.2   04 Aug 1990 12:08:04   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.1   30 Jul 1990  9:23:04   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:58   deanm
 * Initial revision.
 *
 **************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <butil.h>
#include "globals.h"
#include "systool.h"
#include "devcomm.h"
#include "devdata.h"
#include "novell.h"
#include "deverror.h"


CHAR    Sig[]="NCSI";
BYTE    WhichSearch;
VC_TYPE VC;




/*****************************************************************************
* FUNCTION NAME:  NovellInit                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Initializes the novell vitual circuit                                    *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellInit(commInfo);                                           *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellInit initializes commInfo for a novell connection.  It also         *
*  verifies the presence of the NCSI or NASI TSR's.                          *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/



INT  NovellInit (commInfo)

COMMDATA *commInfo;

   {

   commInfo->stop_bits = NOVELL_STOP_BITS;
   WhichSearch         = SEARCH_FIRST;

   if (ncsiLdCk() != SUCCESS)
      {
      return (NO_NCSI);             /*  NO NCSI or error allocating vc */
      }

   if (vcAlloc() != SUCCESS)
      {
      if (VC.vcid == NOT_RESPD)
         {
         return (NO_COMM_SERVER);
         }
      else
         {
         return (NOVELL_ERR);
         }
      }

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellOpen                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Opens the novell virtual circuit                                         *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellOpen (commInfo);                                          *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellOpen establishes the novell virtual circuit with an open port.      *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellOpen (commInfo)

COMMDATA *commInfo;

   {

   QUE_BUF_TYPE query_buf;
   INT          err;


/* setup query buff to find an open port on the comm server */
   query_buf.packet_len = 30;    /* mandatory to be 30 */
   query_buf.protocol   = NOVELL_ASYNCH_PROTOCOL;
   strcpy (query_buf.server_name, "??????????????????????????????");
   memcpy (query_buf.server_name, commInfo->servName, 
           strlen (commInfo->servName) );
   memcpy (query_buf.general_name, commInfo->targetName, 
           strlen (commInfo->targetName));
   memcpy (VC.srvc_name, query_buf.server_name, 
           NAME_LEN + NAME_LEN + SPEC_NMLEN);
   VC.srvc_name [NAME_LEN + NAME_LEN + SPEC_NMLEN] = '\0';


   err = RETRY;
   while (err == RETRY)
      {
      if ((err = query (&query_buf)) == SUCCESS)  /* look for open port */
         {
         err = vcOpen (commInfo);                 /* now open the port  */
         }
      }

   return (err);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellClose                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  Closes the novell virtual circuit                                         *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellClose (commInfo);                                         *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellClose terminates the novell virtual circuit with the workstation.   *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellClose (commInfo)

COMMDATA *commInfo;

   {

   union REGS inregs, outregs;


   while (okToSend () == BUSY);

   inregs.h.al = VC.vcid;
   inregs.h.ah = VC_DISCON;
   int86(NCSI, &inregs, &outregs);

/* no reason to check for error on disconnect
 *
 *   if (outregs.x.cflag == SET)
 *      {
 *      return (NO_COMM_SERVER);
 *      }
*/

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellReadCh                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  read a char                                                               *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellReadCh (commInfo, ch);                                    *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *ch       - read in character                                   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellReadCh reads a character from NASI.                                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellReadCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     *ch;

   {

   union  REGS inregs, outregs;
   struct SREGS segregs;
   CHAR   tempchr;
   CHAR   *temp;


   inregs.h.ah = VC_READ;
   inregs.h.al = VC.vcid;
   inregs.x.cx = 1;        /* only read one character */
   temp        = &tempchr;
   inregs.x.bx = utoff (temp);
   segregs.es  = utseg (temp);
   int86x(NCSI, &inregs, &outregs, &segregs);

   if (outregs.x.cflag == SET)
      {
      return (NOVELL_ERR);
      }

   if (outregs.x.cx > 0)
      {
      *ch = *temp & (CHAR) 0x7F;
      }
   else
      {
      *ch = '\0';
      }

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellReadStr                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  read a char string                                                        *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellReadStr (commInfo, string, length);                       *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - read in character string                            *
*   INT      length    - max length of string                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellReadCh reads a character string from NASI.                          *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellReadStr (commInfo, string, length)

COMMDATA *commInfo;
CHAR     *string;
INT      length;

   {

   union  REGS inregs, outregs;
   struct SREGS segregs;
   CHAR   *temp;


   string [0]  = '\0';

   inregs.h.ah = VC_READ;
   inregs.h.al = VC.vcid;
   inregs.x.cx = length;
   temp        = string;
   inregs.x.bx = utoff (temp);
   segregs.es  = utseg (temp);
   int86x (NCSI, &inregs, &outregs, &segregs);

   if (outregs.x.cflag == SET)
      {
      return (NOVELL_ERR);      /* error condition */
      }

   string [outregs.x.cx] = '\0';
   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellWriteCh                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  write a character                                                         *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellWriteCh (commInfo, ch);                                   *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     ch        - character to send                                   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellWriteCh sends a char to NASI.                                       *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellWriteCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     ch;

   {

   union  REGS inregs, outregs;
   struct SREGS segregs;
   CHAR   *sptr;

   inregs.h.ah = VC_WRITE;
   inregs.h.al = VC.vcid;
   inregs.x.cx = 1;           /* only write one character */
   sptr        = &ch;
   inregs.x.bx = utoff (sptr);
   segregs.es  = utseg (sptr);
   int86x(NCSI, &inregs, &outregs, &segregs);

   if (outregs.x.cflag == SET)
      {
      return (NOVELL_ERR);   /* error condition */
      }

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellWriteStr                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  write a character string                                                  *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellWriteStr (commInfo, string);                              *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - character string to send                            *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellWriteStr sends a character string to NASI.                          *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellWriteStr (commInfo, string)

COMMDATA *commInfo;
CHAR     *string;

   {

   union  REGS inregs, outregs;
   struct SREGS segregs;
   INT    nwrite;
   CHAR   *midptr;


   inregs.h.ah = VC_WRITE;
   inregs.h.al = VC.vcid;
   inregs.x.cx = strlen (string);
   inregs.x.bx = utoff (string);
   segregs.es  = utseg (string);
   int86x (NCSI, &inregs, &outregs, &segregs);

   if (outregs.x.cflag == SET)
      {
      return (NOVELL_ERR);      /* error condition */
      }

   nwrite = outregs.x.cx;
   if (nwrite < (INT) strlen (string))
      {
      midptr = string + nwrite;
      return (NovellWriteStr (commInfo, midptr) );
      }

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellFlush                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  flush the input and output buffers                                        *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellFlush (commInfo);                                         *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellFlush flushes out the NASI input and output buffers.                *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellFlush (commInfo)

COMMDATA *commInfo;

   {
   union  REGS inregs, outregs;


   inregs.h.al = VC.vcid;
   inregs.h.ah = VC_RECV_CLR;
   int86(NCSI, &inregs, &outregs);

   inregs.h.al = VC.vcid;
   inregs.h.ah = VC_XMIT_CLR;
   int86(NCSI, &inregs, &outregs);

   return(NCSI_OK);
   }





/*****************************************************************************
* FUNCTION NAME:  NovellLineCheck                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  checks the modem line for a problem                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellLineCheck (commInfo);                                     *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellLineCheck checks the modem's line for CD.                           *
*                                                                            *
*                                                                            *
* CAUTIONS:  As of version 2.07c of NASI, the external status function       *
*            does not work as novell specifies.  There appears to be a       *
*            NASI documentation problem or software bug.                     *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  NovellLineCheck (commInfo)

COMMDATA *commInfo;

   {
   union  REGS inregs, outregs;

   if (Sys_isOnline ())
      {
      inregs.h.ah = EXTERN_STAT;
      inregs.h.al = VC.vcid;
      int86(NCSI, &inregs, &outregs);

      if (outregs.x.cflag == SET)
         {
         return (MODEM_FAIL_ERROR);
         }
 
      if ((outregs.h.al & DCD_BIT) != DCD_BIT)
         {
         return (MODEM_FAIL_ERROR);
         }
      }

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NovellCharReady                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*  checks NASI for incomming chars                                           *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NovellCharReady (commInfo);                                     *
*                                                                            *
*   BOOLEAN  status    - TRUE if char is ready to be read                    *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*  NovellCharReady checks NASI buffers for any chars to read.                *
*                                                                            *
*                                                                            *
* CAUTIONS:  Version 2.07c of NASI does not perform according to docs.       *
*            The returned status is found in the AL register not the CX      *
*            register.                                                       *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS or an error defined in DEVERROR.H                                *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




BOOLEAN  NovellCharReady (commInfo)

COMMDATA *commInfo;

   {
   union   REGS inregs, outregs;
   BOOLEAN status;

   inregs.h.ah = VC_RECV_STAT;
   inregs.h.al = VC.vcid;
   inregs.x.cx = 0;
   int86 (NCSI, &inregs, &outregs);

   if (outregs.x.cx == DATA_AVAIL  ||  outregs.x.ax)
      {
      status = TRUE;
      }
   else
      {
      status = FALSE;
      }

   return (status);
   }



/* the following are internal routines to NOVELL.C */


INT   getSrvID (vcid, ibptr)

BYTE          vcid;
INIT_BUF_TYPE *ibptr;

   {
   union REGS inregs, outregs;  /*  registers                           */
   struct SREGS segregs;        /*  segment registers                   */
   INT  results;                /*  results of interrupt function       */

   ibptr->reqpktlen = REQUEST_SIZE;
   inregs.h.ah = VC_CONFIG;
   inregs.h.al = vcid;
   inregs.h.cl = BIN_RETRIEVE;
   inregs.x.bx = utoff (ibptr);
   segregs.es  = utseg (ibptr);
   results = int86x(NCSI, &inregs, &outregs, &segregs);
   if (outregs.x.cflag == SET)
      return(NCSI_ERR);
   VC.portnum = (INT) ibptr->srvportid;

   return(NCSI_OK);
   }




/* opens the virtual circuit */

INT  vcOpen (commInfo)

COMMDATA *commInfo;

   {

   union  REGS  inregs, outregs;  /*  registers                           */
   struct SREGS segregs;          /*  segment registers                   */
   INT    results;                /*  results of interrupt function       */
   INIT_BUF_TYPE init_buf;
   INIT_BUF_TYPE *ibptr;
   INT    parity;
   INT    data_bits;
   INT    baud;


   inregs.h.ah = VC_INIT;
   inregs.h.al = VC.vcid;
   inregs.x.bx = 0;
   results = int86x(NCSI, &inregs, &outregs, &segregs);
   if ((outregs.x.cflag == SET) && 
       (outregs.h.al == PORT_UNAVAIL  ||  outregs.h.cl == PORT_UNAVAIL  ||
        outregs.h.al == SUCCESS))
      {
      return(RETRY);
      }
   else if (outregs.x.cflag == SET)
      {
      return(outregs.h.al);
      }

   ibptr = &init_buf;

/* get the complete name from the comm server (server, general, specific) */
   if ((results = getSrvID (VC.vcid, ibptr)) != NCSI_OK)
      {
      return (RETRY);
//      return (NOVELL_ERR);
      }


/* get necessary parity and data bits (normally done at init stage) */
   switch (commInfo->parity)
      { 
      case NOPARITY :
      case MARKPARITY :
      case SPACEPARITY :
         parity = NOVELL_NOPAR;
         data_bits = NOVELL_8_BITS;
         break;
     
      case ODDPARITY :
         parity = NOVELL_ODDPAR; 
         data_bits = NOVELL_7_BITS;
         break;

      case EVENPARITY :
      default :            
         parity = NOVELL_EVENPAR;
         data_bits = NOVELL_7_BITS;
         break;
      }   


/* get appropriate bauds */
   switch (commInfo->speed)
      {
      case 300  :
         baud = NOVELL_300_BAUD;
         break;
      case 1200 :
         baud = NOVELL_1200_BAUD;
         break;
      case 2400 :
         baud = NOVELL_2400_BAUD;
         break;
      case 4800 :
         baud = NOVELL_4800_BAUD;
         break;
      case 9600 :
         baud = NOVELL_9600_BAUD;
         break;
      }



/* initialize the port */

   inregs.h.ah = VC_INIT;
   inregs.h.al = VC.vcid;
   ibptr->reqpktlen  =        REQUEST_SIZE;
   ibptr->srvportid  = (BYTE) VC.portnum;
   ibptr->rcvbaud    = (BYTE) baud;
   ibptr->rcvbytelen = (BYTE) data_bits;
   ibptr->rcvstop    = (BYTE) NOVELL_STOP_BITS;
   ibptr->rcvprty    = (BYTE) parity;
   ibptr->xmtbaud    = (BYTE) baud;
   ibptr->xmtbytelen = (BYTE) data_bits;
   ibptr->xmtstop    = (BYTE) NOVELL_STOP_BITS;
   ibptr->xmtprty    = (BYTE) parity;
   ibptr->dtr_ctrl   =        SET;
   ibptr->rts_ctrl   =        SET;
   ibptr->xon_xoff   =        SET;
   ibptr->reserved   =        0;     /* must ALWAYS be zero            */
   inregs.x.bx = utoff (ibptr);
   segregs.es  = utseg (ibptr);
   results = int86 (NCSI, &inregs, &outregs);

   if (outregs.x.cflag == SET  &&  
             (outregs.h.al == PORT_UNAVAIL  ||  outregs.h.cl == PORT_UNAVAIL))
      {
      return (RETRY);
      }
   else if (outregs.x.cflag == SET)
      {
      return (NOVELL_ERR);
      }
//   if (outregs.x.cflag == RESET  &&  outregs.h.cl != VCONNECT)
//      {
//printf ("\nreturning retry out of vcOpen 1");
//      return (RETRY);
//      }
//   else if (outregs.x.cflag == RESET  &&  VC.portnum == USE_DEF)
//      {
//      return (NO_INIT);    /* NO INIT */
//      }


   return (SUCCESS);
   }






/* checks to make sure it is ok to send NASI data */

INT  okToSend ()

   {
   union REGS inregs, outregs;

   inregs.h.ah = VC_XMIT_STAT;
   inregs.h.al = VC.vcid;
   int86(NCSI, &inregs, &outregs);

   if (outregs.x.cflag == SET)
      {
      return (outregs.h.al);
      }
   else if ((outregs.x.cflag == RESET) && (outregs.h.ah == SERV_DOWN))
      {
      return (SHUTDOWN);
      }
   else if ((outregs.x.cflag == RESET) && (outregs.h.al == SERV_BUSY))
      {
      return (BUSY);
      }

   return (SUCCESS);
   }



/* checks for the presence of NASI */

INT  ncsiLdCk ()

   {
   LONG far *isr_addr;   /*  NCSI interrupt service routine address*/
   CHAR far *temp;
   CHAR *buff = Sig;
   INT  i;

   isr_addr = (LONG far *)(4 * NCSI);
   temp     = (CHAR far *)(*isr_addr + 3l);
   for (i=0; (i<4) && (*temp == *buff); i++,buff++,temp++);
   if (i < 4)
      {
      return (FAILURE);
      }

   return (SUCCESS);
   }




/* rotors through the NASI ports to find an available one */

INT  query (query_buf)

QUE_BUF_TYPE *query_buf;

   {
   union REGS inregs, outregs;  /*  registers                           */
   struct SREGS segregs;        /*  segment registers                   */
   INT  results;
   INT  i;


/* NOTICE ***  Server, general, and specific names are NOT null
 *             terminated and ALL characters reserved for the
 *             server, general, or specific name MUST be filled
 *             with non-null characters.  Question marks "?" should
 *             be used for empty names and underscores "_" should
 *             be used to fill out the remainder of a name.  Also,
 *             query.c ASSUMES the target name is a general name.
 *             This should be altered to allow the user to specify a 
 *             general name or a specific name (this alteration would
 *             require changes in config).
 */

   strupr (query_buf->server_name);
   for (i=0;i<query_buf->packet_len;i++)
      {
      if (query_buf->server_name[i] == ' ')
         {
         query_buf->server_name[i] = '_';
         }
      }

   segread (&segregs);
   inregs.h.ah = VC_QUERY;
   inregs.h.al = VC.vcid;
   inregs.h.cl = WhichSearch;
   inregs.x.bx = utoff ((CHAR _far *)query_buf);
   segregs.es  = utseg  (query_buf);
   results = int86x(NCSI, &inregs, &outregs, &segregs);
   WhichSearch = SEARCH_NEXT;


   if (outregs.x.cflag == SET  &&  WhichSearch == SEARCH_FIRST)
      {
      return (NO_NAME);             /* NO NAME   */
      }
   else if (outregs.x.cflag == SET)
      {
      return (NO_MODEMS);             /* NO MODEMS */
      }

   strcpy (VC.srvc_name, query_buf->server_name);

   return (SUCCESS);
   }



/* allocates a virtual circuit */

INT  vcAlloc ()

   {
   union REGS inregs, outregs;   /*  registers                           */


   inregs.h.ah = VC_ALLOC;
   inregs.h.al = 0;              /* to make compatable with NCSI 1.14    */
   int86(NCSI, &inregs, &outregs);
   VC.vcid = outregs.h.al;
   if (outregs.x.cflag == SET)
      {
      return (FAILURE);
      }

   return (SUCCESS);
   }







/* OLD NOVELL CODE ******************************************************

/* get the complete name from the comm server (server, general, specific) *
   if ((results = getSrvID (VC.vcid, ibptr)) != NCSI_OK)
      {
      return (NOVELL_ERR);
      }



/* get necessary parity and data bits (normally done at init stage) *
   switch (commInfo->parity)
      { 
      case NOPARITY :
      case MARKPARITY :
      case SPACEPARITY :
         parity = NOVELL_NOPAR;
         data_bits = NOVELL_8_BITS;
         break;
     
      case ODDPARITY :
         parity = NOVELL_ODDPAR; 
         data_bits = NOVELL_7_BITS;
         break;

      case EVENPARITY :
      default :            
         parity = NOVELL_EVENPAR;
         data_bits = NOVELL_7_BITS;
         break;
      }   


/* get appropriate bauds *
   switch (commInfo->speed)
      {
      case 300  :
         baud = NOVELL_300_BAUD;
         break;
      case 1200 :
         baud = NOVELL_1200_BAUD;
         break;
      case 2400 :
         baud = NOVELL_2400_BAUD;
         break;
      case 4800 :
         baud = NOVELL_4800_BAUD;
         break;
      case 9600 :
         baud = NOVELL_9600_BAUD;
         break;
      }



/* now re-init the port to values specified by the user *
   VC.srvportid = ibptr->srvportid;

   ibptr->reqpktlen  =        REQUEST_SIZE;
   ibptr->rcvbaud    = (BYTE) baud;
   ibptr->rcvbytelen = (BYTE) data_bits;
   ibptr->rcvstop    = (BYTE) NOVELL_STOP_BITS;
   ibptr->rcvprty    = (BYTE) parity;
   ibptr->xmtbaud    = (BYTE) baud;
   ibptr->xmtbytelen = (BYTE) data_bits;
   ibptr->xmtstop    = (BYTE) NOVELL_STOP_BITS;
   ibptr->xmtprty    = (BYTE) parity;
   ibptr->dtr_ctrl   =        SET;
   ibptr->rts_ctrl   =        SET;
   ibptr->xon_xoff   =        SET;
   ibptr->reserved   =        0;     /* must ALWAYS be zero            *


   inregs.h.ah    = VC_INIT;
   inregs.h.al    = VC.vcid;
   inregs.x.bx    = utoff (ibptr);
   segregs.es     = utseg (ibptr);
   results        = int86x (NCSI, &inregs, &outregs, &segregs);
   VC.portnum     = outregs.h.al;
*******  END OF OLD NOVELL CODE *******************************/
