/*****************************************************************************
* FILE NAME:  modem.c                                                        *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Modems                                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemInit      - Initialize modem                                        *
*   ModemOpen      - Open modem                                              *
*   ModemClose     - Close modem                                             *
*   ModemReadCh    - Read a character from the modem                         *
*   ModemReadStr   - read a string from the modem                            *
*   ModemWriteCh   - Write a character to the modem                          *
*   ModemWriteStr  - Write a string to the modem                             *
*   ModemBreak     - Send a break                                            *
*   ModemFlush     - Flush the input and ouput of the modem                  *
*   ModemLineCheck - Check the phone line to see if still active             *
*   ModemCharReady - Check to see if a character is ready to be read         *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   COMMDATA     - General information about the communications              *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
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
 *  $Log:   N:/src/modem.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:55:26   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:17:16   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:21:56   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:05:40   andrews
 * No change.
 * 
 *    Rev 4.1   02 Oct 1990 15:54:50   deanm
 * removed parity, overrun, and framing error checks in reads
 * 
 *    Rev 4.0   01 Oct 1990 13:23:34   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.2   28 Sep 1990 16:14:56   deanm
 * fixed inadvertant 'error reading character' prob.
 * 
 *    Rev 3.1   20 Sep 1990 13:47:30   deanm
 * do a line check even if no character was read (readch, readstr)
 * 
 *    Rev 3.0   17 Sep 1990 10:05:24   andrews
 * Full Iteration 1
 * 
 *    Rev 2.1   05 Sep 1990  8:39:36   joeb
 * drain the buffer in a kinder and gentler way
 * 
 *    Rev 2.0   27 Aug 1990  9:19:20   andrews
 * iteration 1
 * 
 *    Rev 0.4   25 Aug 1990 18:44:50   deanm
 * cleaning up novell and int14
 * 
 *    Rev 0.3   21 Aug 1990 14:10:00   deanm
 * cleaning up modem exceptions
 * 
 *    Rev 0.2   04 Aug 1990 12:08:10   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.1   30 Jul 1990  9:23:10   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:54:06   deanm
 * Initial revision.
 *
*/



#include <string.h>
#include <asynch_1.h>
#include "globals.h"
#include "systool.h"
#include "devcomm.h"
#include "devdata.h"
#include "modem.h"
#include "deverror.h"


#define MDC_STATUS_ERR   0x9E


/*****************************************************************************
* FUNCTION NAME:  ModemInit                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Initializes a modem                                                      *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemInit (commInfo);                                           *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemInit initializes the modem according to the Asynch Manager's        *
*   init_a1 routine.                                                         *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/



INT  ModemInit (commInfo)

COMMDATA *commInfo;

   {
   INT   baud;
   INT   parity;
   INT   data_bits;
   INT   lstat, mstat;    /* line and modem status */


/* set the stop bits for modems */
   commInfo->stop_bits = MODEM_STOP_BITS;


/* get the appropriate parity and data bits for the call to init_a1 */

   switch (commInfo->parity)
      { 
      case NOPARITY :
      case MARKPARITY :
      case SPACEPARITY :
         parity = MODEM_NOPAR;
         data_bits = MODEM_8_BITS;
         break;
     
      case ODDPARITY :
         parity = MODEM_ODDPAR; 
         data_bits = MODEM_7_BITS;
         break;

      case EVENPARITY :
      default :            
         parity = MODEM_EVENPAR;
         data_bits = MODEM_7_BITS;
         break;
      }   



/* get the appropriate baud rate values for the call to init_a1  */

   switch (commInfo->speed)
      {
      case 300  :
         baud = MODEM_300_BAUD;
         break;
      case 1200 :
         baud = MODEM_1200_BAUD;
         break;
      case 2400 :
         baud = MODEM_2400_BAUD;
         break;
      case 4800 :
         baud = MODEM_4800_BAUD;
         break;
      case 9600 :
         baud = MODEM_9600_BAUD;
         break;
      }


   close_a1 (commInfo->portnum+1);

   init_a1 (commInfo->portnum+1, baud, parity, MODEM_STOP_BITS, data_bits, 
            &lstat, &mstat);

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  ModemOpen                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Open a modem                                                             *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemOpen (commInfo);                                           *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemOpen opens the modem according to the Asynch Manager's open_a1      *
*   routine.                                                                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  ModemOpen (commInfo)

COMMDATA *commInfo;

   {


   open_a1 (commInfo->portnum+1, INPUT_SIZE, OUTPUT_SIZE, 
                     MODEM_DEFAULT_INT, MODEM_DEFAULT_PORT_ADS, 
                     commInfo->buffer);

   setop_a1 (commInfo->portnum+1, MODEM_REMOTE_FLOW,   ENABLE);
   setop_a1 (commInfo->portnum+1, MODEM_LOCAL_FLOW,    ENABLE);
   setop_a1 (commInfo->portnum+1, MODEM_CTS_REQUIRE,   DISABLE);
 
   if (commInfo->parity == MARKPARITY )
      {
      setop_a1 (commInfo->portnum+1, MODEM_BIT7_FORCE, ENABLE);
      }

   if (commInfo->parity == SPACEPARITY )
      {
      setop_a1 (commInfo->portnum+1, MODEM_BIT7_TRIM,  ENABLE);
      }

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  ModemClose                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   close a modem                                                            *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemClose (commInfo);                                          *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemClose closes the modem according to the Asynch Manager's            *
*   close_a1 routine.                                                        *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/





INT  ModemClose (commInfo)

COMMDATA *commInfo;

   {

   close_a1 (commInfo->portnum+1);

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  ModemReadCh                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   read a character from a modem                                            *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemReadCh (commInfo, ch);                                     *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *ch       - character to be read                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemReadCh reads a character from the modem according to the Asynch     *
*   Manager's rdch_a1 routine.                                               *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/





INT  ModemReadCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     *ch;

   {
   INT  status;
   INT  num_remaining;
   INT  lstat;

   status = rdch_a1 (commInfo->portnum+1, ch, &num_remaining, &lstat);
   
   switch (status)
      {
      case IN_Q_EMPTY :
         *ch = '\0';
         status = (INT) ModemLineCheck (commInfo);
         break;

      case INV_PORT :
      case PORT_NOT_OPEN :
      case OUT_Q_FULL :
         *ch = '\0';
         status = MODEM_FAIL_ERROR;
         break;

      case A_OK :
      default :
         *ch &= 0x7F;
         if (Sys_isOnline ())
            {
            status = (INT) ModemLineCheck (commInfo);
//            if (lstat & MDC_STATUS_ERR)
//               {
//               status = MODEM_FAIL_ERROR;
//               }
//            else
//               {
//               status = (INT) ModemLineCheck (commInfo);
//               }
            }
         break;
      }

   return (status);
   }





/*****************************************************************************
* FUNCTION NAME:  ModemReadStr                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   reads a string from a modem                                              *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemReadStr (commInfo, string, length);                        *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - character string to read                            *
*   INT      length    - max length to read                                  *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemReadStr reads a string from a modem according to the Asynch         *
*   Manager's rdst_a1 routine.                                               *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  ModemReadStr (commInfo, string, length)

COMMDATA *commInfo;
CHAR     *string;
INT      length;

   {
   INT   status;
   UNSIGNED pstat;
   INT   num_remaining;
   INT   num_read;
   CHAR  *temp;
   CHAR  *end;

   status = rdst_a1 (commInfo->portnum+1, length, string, &num_read,
                     &num_remaining, &pstat);

   switch (status)
      {
      case IN_Q_EMPTY :
         *string = '\0';
         status = (INT) ModemLineCheck (commInfo);
         break;

      case INV_PORT :
      case PORT_NOT_OPEN :
      case OUT_Q_FULL :
         *string = '\0';
         status = MODEM_FAIL_ERROR;
         break;

      case A_OK :
      default :

         /* remove all null characters in string */
         temp = end = string;
         while (end - string < num_read)
            {
            if (*end != '\0')
               {
               *temp = *end & (CHAR) 0x7F;
               temp++;
               }
            end++;
            }
         *temp = '\0';      /*  add a null terminator to whatever string */

/* quick error check */
         if (Sys_isOnline ())
            {
            status = (INT) ModemLineCheck (commInfo);
//            if (pstat & MDC_STATUS_ERR)
//               {
//               status = MODEM_FAIL_ERROR;
//               }
//            else
//               {
//               status = (INT) ModemLineCheck (commInfo);
//               }
            }
         break;
      }

   return (status);
   }




/*****************************************************************************
* FUNCTION NAME:  ModemWriteCh                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   write a character to a modem                                             *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemWriteCh (commInfo, ch);                                    *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     ch        - character to write                                  *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemWriteCh writes to the modem a character according to the Asynch     *
*   Manager's wrtch_a1 routine.                                              *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/






INT  ModemWriteCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     ch;

   {

   wrtch_a1 (commInfo->portnum+1, ch);

   return (SUCCESS);
/*   return (ModemLineCheck (commInfo) );*/
   }






/*****************************************************************************
* FUNCTION NAME:  ModemWriteStr                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   write a string to a modem                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemWriteStr (commInfo, string);                               *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - character string to write to the modem              *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemWriteStr writes a string to the modem according to the Asynch       *
*   Manager's wrtst_a1 routine.                                              *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  ModemWriteStr (commInfo, string)

COMMDATA *commInfo;
CHAR     *string;

   {
   INT  numWritten;
   INT  len;
   INT  total;

   total = numWritten = 0;
   len = strlen (string);
   while (len > 0)
      {
      wrtst_a1 (commInfo->portnum+1, len, string+total, &numWritten);
      drain_a1 (commInfo->portnum+1, 2, 0);
      len -= numWritten;
      total += numWritten;
      }

   return (SUCCESS);
/*   return (ModemLineCheck (commInfo) );    */
   }






/*****************************************************************************
* FUNCTION NAME:  ModemBreak                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Send a Break                                                             *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemBreak (commInfo);                                          *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemBreak sends a break sequence to the comm port according to the      *
*   Asynch Manager's setop_a1 and break_a1 routines.                         *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  ModemBreak (commInfo)

COMMDATA *commInfo;

   {

   break_a1 (commInfo->portnum+1);

   return (SUCCESS);
   }





/*****************************************************************************
* FUNCTION NAME:  ModemFlush                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   flush the input and output buffers                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemFlush (commInfo);                                          *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemFlush flushes the input and output buffers according to the         *
*   Asynch Manager's iflsh_a1 and oflsh_a1 routines.                         *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  ModemFlush (commInfo)

COMMDATA *commInfo;

   {

   iflsh_a1 (commInfo->portnum+1);
   oflsh_a1 (commInfo->portnum+1);

   return (SUCCESS);
   }





/*****************************************************************************
* FUNCTION NAME:  ModemLineCheck                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Checks the modem line for CD                                             *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = ModemLineCheck (commInfo);                                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemLineCheck checks the line status of the modem for CD according      *
*   to the Asynch Manager's stat_a1 routine.                                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A zero value if completed successfully and a non-zero value if function  *
*   failed.                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/





INT  ModemLineCheck (commInfo)

COMMDATA *commInfo;

   {
   INT  status = SUCCESS;
   INT  mstat, lstat;

   if ( commInfo->cd_check && Sys_isOnline() )
      {
      if ((stat_a1(commInfo->portnum+1, &lstat, &mstat) & MODEM_CD_MASK) == 0)
         { 
         status = MODEM_FAIL_ERROR;
         }
      }

   return (status);
   }






/*****************************************************************************
* FUNCTION NAME:  ModemCharReady                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Sees if a character is ready to be read                                  *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   exists = ModemCharReady (commInfo);                                      *
*                                                                            *
*   BOOLEAN  exists    - existance of characters to read                     *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   ModemCharReady checks the input buffer according to the Asynch           *
*   Manager's qsize_a1 routine.                                              *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A TRUE value means characters are ready and a FALSE value indicates no   *
*   characters ready.                                                        *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/





BOOLEAN  ModemCharReady (commInfo)

COMMDATA *commInfo;

   {
   INT  remaining;
   INT  pstat;
   BOOLEAN  status;

   qsize_a1 (commInfo->portnum+1, &remaining, &pstat);

   if (remaining == 0)
      {
      status = FALSE;
      }
   else
      {
      status = TRUE;
      }

   return (status);
   }
