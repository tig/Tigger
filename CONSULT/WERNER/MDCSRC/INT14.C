/*****************************************************************************
* FILE NAME:  int14.c                                                        *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for int14 devices                                          *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickInit    - Initialize    (timer tick only)                       *
*   INT14PollInit    - Initialize                                            *
*   INT14Open        - Open                                                  *
*   INT14TickClose   - Close         (timer tick only)                       *
*   INT14PollClose   - Close                                                 *
*   INT14TickReadCh  - Read a character      (timer tick only)               *
*   INT14PollReadCh  - Read a character                                      *
*   INT14TickReadStr - read a string         (timer tick only)               *
*   INT14PollReadStr - read a string                                         *
*   INT14WriteCh     - Write a character                                     *
*   INT14WriteStr    - Write a string                                        *
*   INT14TickFlush   - Flush the input and ouput buffers   (timer tick only) *
*   INT14PollFlush   - Flush the input and ouput buffers                     *
*   INT14LineCheck   - Check the phone line to see if still active           *
*   INT14TickCharReady - Check for a character is ready to be read           *
*   INT14PollCharReady - Check for a character is ready to be read           *
*                                                                            *
*   INT14Tick        - INT14 timer tick routine                              *
*                                                                            *
*                                                                            *
*                                                                            *
* PRIVATE FUNCTIONS:                                                         *
*                                                                            *
*   INT14Tick        - timer tick routine                                    *
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
* DATE:  07/09/90                                                            *
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
 *  $Log:   N:/src/int14.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:54:10   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:16:02   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:20:28   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.2   31 Oct 1990 14:35:50   vincentt
 * fixed int14 through novell error
 * 
 *    Rev 4.0   01 Oct 1990 13:22:16   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.2   29 Sep 1990 21:46:00   deanm
 * slow sending down a little bit so that modem dialing does not get confused
 * 
 *    Rev 3.1   20 Sep 1990 11:37:08   deanm
 * changing line checking algorithms
 * 
 *    Rev 3.0   17 Sep 1990 10:04:00   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:17:58   andrews
 * iteration 1
 * 
 *    Rev 0.4   26 Aug 1990 21:26:04   deanm
 * fixing up chaining for int14
 * 
 *    Rev 0.3   25 Aug 1990 18:44:58   deanm
 * cleaning up novell and int14
 * 
 *    Rev 0.2   04 Aug 1990 12:07:46   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.1   30 Jul 1990  9:22:12   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:28   deanm
 * Initial revision.
 *
*/



#include <string.h>
#include <dos.h>

#include <bintrupt.h>
#include <butil.h>

#include "globals.h"
#include "systool.h"
#include "devcomm.h"
#include "devdata.h"
#include "int14.h"
#include "int14dta.h"
#include "modem.h"
#include "deverror.h"


#define  INT14_ERR_MASK       0x02


CHAR     INT14_stack[INT14_STACKSIZ];
ISRCTRL  INT14_cblk;
DWORD    Old_Clock;
extern   INT   Olstatus;            // defined in systool.c



/*****************************************************************************
* FUNCTION NAME:  INT14TickInit                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Initializes INT14 DEVICE                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14TickInit (commInfo);                                       *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickInit establishes the timer tick routine in INT1C.               *
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
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





INT  Int14TickInit (commInfo)

COMMDATA *commInfo;

   {


/* set the stop bits for INT14 */
   commInfo->stop_bits = INT14_STOP_BITS;

   LINKUP (&Old_Clock);
   isinstal (0x1C, INT14Tick, "JL COMM SERVER ", &INT14_cblk, 
             INT14_stack, INT14_STACKSIZ, 5);
   return (SUCCESS);
           
   }




/*****************************************************************************
* FUNCTION NAME:  INT14PollInit                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Initializes INT14 DEVICE                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14PollInit (commInfo);                                       *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14PollInit does any initialization for polling INT14.                 *
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
* DATE:  07/10/90                                                            *
*                                                                            *
*****************************************************************************/





INT  Int14PollInit (commInfo)

COMMDATA *commInfo;

   {

   return (SUCCESS);       /* this function has nothing to do ... for now */
   }





/*****************************************************************************
* FUNCTION NAME:  INT14Open                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Open the INT14 DEVICE                                                    *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14Open (commInfo);                                           *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14Open opens the port using INT14 calls.                              *
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
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/







INT  INT14Open (commInfo)

COMMDATA *commInfo;

   {
   INT   baud;
   INT   parity;
   INT   data_bits;


/* get the appropriate parity and data bits for the call to int14 */

   switch (commInfo->parity)
      { 
      case NOPARITY :
      case MARKPARITY :
      case SPACEPARITY :
         parity    = INT14_NOPAR;
         data_bits = MODEM_8_BITS;
         break;

      case ODDPARITY :
         parity    = INT14_ODDPAR; 
         data_bits = MODEM_7_BITS;
         break;

      case EVENPARITY :
      default :            
         parity    = INT14_EVENPAR;
         data_bits = MODEM_7_BITS;
         break;
      }   



/* get the appropriate baud rate values for the call to int14  */

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


/* INITPT14 does the init and open */

   INITPT14 (commInfo->portnum, (baud<<5) | (parity<<3) | 
             (MODEM_STOP_BITS<<2) | data_bits);

   return (SUCCESS);
   }





/*****************************************************************************
* FUNCTION NAME:  INT14TickClose                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   close the INT14 DEVICE                                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14TickClose (commInfo);                                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickClose removes the timer tick routine in INT1C.                  *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/





INT  INT14TickClose (commInfo)

COMMDATA *commInfo;

   {

   INT14TickFlush (commInfo);

   DELINK (&Old_Clock);

   return (SUCCESS);
   }







/*****************************************************************************
* FUNCTION NAME:  INT14PollClose                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   close the INT14 DEVICE                                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14PollClose (commInfo);                                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14PollClose flushes the buffers.                                      *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/





INT  INT14PollClose (commInfo)

COMMDATA *commInfo;

   {

   return (INT14PollFlush (commInfo) );
   }







/*****************************************************************************
* FUNCTION NAME:  INT14TickReadCh                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   read a character from the int14 device                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14TickRead Ch (commInfo, ch);                                *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *ch       - character to be read                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickReadCh reads a character out of the commInfo->buffer.  The      *
*   actual character was read at some time during a timer tick.              *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/





INT  INT14TickReadCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     *ch;

   {

   *ch = '\0';
   if (commInfo->buff_cnt > 0)
      {
      *ch = *(commInfo->buff_head);
      commInfo->buff_cnt--;
      commInfo->buff_head++;
      if ((commInfo->buff_head - commInfo->buffer) >= COMM_BUFFER_SIZE)
         {
         commInfo->buff_head = commInfo->buffer;
         }
      }

   return (INT14LineCheck (commInfo) );
   }





/*****************************************************************************
* FUNCTION NAME:  INT14PollReadCh                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   read a character from the int14 device                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14PollReadCh (commInfo, ch);                                 *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *ch       - character to be read                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickPollCh reads a character from the comm server.                  *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/





INT  INT14PollReadCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     *ch;

   {
   INT   temp;
   INT   status;
   

   *ch = '\0';
   if (RCVCHR14RDY (commInfo->portnum))
       {
       RCVCHR14 (commInfo->portnum, &temp, &status);
       *ch = (CHAR) (temp & 0x007F);
       }

   return (INT14LineCheck (commInfo) );
   }





/*****************************************************************************
* FUNCTION NAME:  INT14TickReadStr                                           *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Read a string from an int14 device                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14TickReadStr (commInfo, string, length);                    *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - string to read                                      *
*   INT      length    - max length of string                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickReadStr reads up to length characters and places them in        *
*   string.  The actual reading of the characters off of the INT14 device    *
*   was done during the timer tick routine.                                  *
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
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





INT  INT14TickReadStr (commInfo, string, length)

COMMDATA *commInfo;
CHAR     *string;
INT      length;

   {
   INT   numRead;

   string[0] = '\0';
   numRead = 0;
   while ((numRead < length-1) && (commInfo->buff_cnt > 0))
      {
      if (*commInfo->buff_head != '\0')
         {
         string [numRead++] = *commInfo->buff_head;
         }
      commInfo->buff_cnt--;
      commInfo->buff_head++;
      if ((commInfo->buff_head - commInfo->buffer) >= COMM_BUFFER_SIZE)
         {
         commInfo->buff_head = commInfo->buffer;
         }
      }
   string [numRead] = '\0';

   return (INT14LineCheck (commInfo) );
   }







/*****************************************************************************
* FUNCTION NAME:  INT14PollReadStr                                           *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Read a string from an int14 device                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14PollReadStr (commInfo, string, length);                    *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - string to read                                      *
*   INT      length    - max length of string                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14PollReadStr reads up to length characters and places them in        *
*   string.  The reading is done directly from the comm server.              *
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
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





INT  INT14PollReadStr (commInfo, string, length)

COMMDATA *commInfo;
CHAR     *string;
INT      length;

   {
   INT   status;
   INT   numRead;
   INT   temp;


   string[0] = '\0';
   numRead = 0;
   while (RCVCHR14RDY (commInfo->portnum)  &&  numRead < length-1 )
      {
      RCVCHR14 (commInfo->portnum, &temp, &status);
      temp &= 0x7F;
      if (temp != '\0')
         {
         string [numRead++] = (CHAR) temp;
         }
      }
   string [numRead] = '\0';

   return (INT14LineCheck (commInfo) );
   }







/*****************************************************************************
* FUNCTION NAME:  INT14WriteCh                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   writes a character to the int14 device                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14WriteCh (commInfo, ch);                                    *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     ch        - character to write                                  *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14WriteCh writes a character to the INT14 device.                     *
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
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





INT  INT14WriteCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     ch;

   {
   INT status = SUCCESS;

   utsleep (1);
   if (!SNDCHR14 (commInfo->portnum, ch, INT14_TIMEOUT) )
      {
      status = JL_WRITE_ERR;
      }

   return (status);
   }







/*****************************************************************************
* FUNCTION NAME:  INT14WriteStr                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   write a string to the int14 device                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14WriteStr (commInfo, string);                               *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - string to write                                     *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14Writes a string to the int14 device                                 *
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
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





INT14WriteStr (commInfo, string)

COMMDATA *commInfo;
CHAR     *string;

   {
   INT   status;
   CHAR  *index;


   status = SUCCESS;
   index  = string;

   while (*index != '\0'  &&  status == SUCCESS)
      {
      if (!SNDCHR14 (commInfo->portnum, *index, INT14_TIMEOUT) )
         {
         status = JL_WRITE_ERR;
         }
      index++;
      }

   return (status);
   }








/*****************************************************************************
* FUNCTION NAME:  INT14TickFlush                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   flushes the int14 device                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14TickFlush (commInfo);                                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickFlush removes all characters from the INT14 device.             *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/





INT14TickFlush (commInfo)

COMMDATA *commInfo;

   {
   INT  status;
   CHAR line [COLS+1];

   do
      {
      status = INT14TickReadStr (commInfo, line, COLS);
      } while (strlen (line) != 0  &&  status == SUCCESS);

   return (status);
   }





/*****************************************************************************
* FUNCTION NAME:  INT14PollFlush                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   flushes the int14 device                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14PollFlush (commInfo);                                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14PollFlush removes all characters from the INT14 device.             *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/





INT14PollFlush (commInfo)

COMMDATA *commInfo;

   {
   INT  status;
   INT  temp;

   while (RCVCHR14RDY (commInfo->portnum) )
       {
       RCVCHR14 (commInfo->portnum, &temp, &status);
       }

   return (SUCCESS);
   }





/*****************************************************************************
* FUNCTION NAME:  INT14LineCheck                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   check the line of the int14 device                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14LineCheck (commInfo);                                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14LineCheck checks the phone line of the INT14 device.                *
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
* DATE:  07/09/90                                                           *
*                                                                            *
*****************************************************************************/




INT  INT14LineCheck (commInfo)

COMMDATA *commInfo;

   {

   return (SUCCESS);
   }

/*
 *    The old line check routine.
 *
 *#define INT14           0x14
 *#define INT14_CTS_MASK  0x0001
 *#define INT14_LINECHECK 3
 *
 *
 *   INT   status;
 *   INT   lstat, mstat;
 *
 *
 *   status = SUCCESS;
 *   if ( commInfo->cd_check && Sys_isOnline ())
 *      {
 *      union REGS inregs, outregs;
 *
 *      inregs.h.ah = INT14_LINECHECK;
 *      inregs.x.dx = commInfo->portnum;
 *      int86 (INT14, &inregs, &outregs);
 *      if (outregs.h.al & INT14_CTS_MASK)
 *         {
 *         status = MODEM_FAIL_ERROR;
 *         }
 *      }
 *
 *   return (status);
 *   }
*/





/*****************************************************************************
* FUNCTION NAME:  INT14TickCharReady                                         *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   checks for character ready                                               *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14TickCharReady (commInfo);                                  *
*                                                                            *
*   BOOLEAN  status    - TRUE if character is ready, FALSE if none ready     *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14TickCharReady checks for any characters to be read.                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   TRUE if character is ready else returns FALSE.                           *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





BOOLEAN  INT14TickCharReady (commInfo)

COMMDATA *commInfo;

   {
   BOOLEAN ready;

   if (commInfo->buff_cnt > 0)
      {
      ready = TRUE;
      }
   else
      {
      ready = FALSE;
      }
   
   return (ready);
   }






/*****************************************************************************
* FUNCTION NAME:  INT14PollCharReady                                         *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   checks for character ready                                               *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = INT14PollCharReady (commInfo);                                  *
*                                                                            *
*   BOOLEAN  status    - TRUE if character is ready, FALSE if none ready     *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14PollCharReady checks for any characters to be read.                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   TRUE if character is ready else returns FALSE.                           *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





BOOLEAN  INT14PollCharReady (commInfo)

COMMDATA *commInfo;

   {
   BOOLEAN ready;

   if (RCVCHR14RDY (commInfo->portnum) )
      {
      ready = TRUE;
      }
   else
      {
      ready = FALSE;
      }
   
   return (ready);
   }






/*****************************************************************************
* FUNCTION NAME:  INT14Tick                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   timer tick routine                                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*            INT14TickCharReady (commInfo);                                  *
*                                                                            *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   INT14Tick will read up to 40 characters per timer tick.  This routine    *
*   needs to be installed in interrupt vector 1CH.                           *
*                                                                            *
*                                                                            *
* CAUTIONS:                                                                  *
*                                                                            *
*   This routine is an ISR and should not be called as a normal routine.     *
*                                                                            *
*                                                                            *
* RETURNS:  void                                                             *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/09/90                                                            *
*                                                                            *
*****************************************************************************/





void cdecl INT14Tick (regs, ctrl, msg)

ALLREG    *regs;
ISRCTRL   *ctrl;
ISRMSG    *msg;
   {
   INT    jl_char;
   INT    status;
   INT    i;
   static BOOLEAN busy = FALSE;
   extern COMMDATA CommInfo;     /* need to declare this as a global here */


   CHAIN (&Old_Clock);

   if (busy == FALSE)
      {
      busy = TRUE;
      status = 0;
      i = 0;

      while (i < 40   &&   RCVCHR14RDY (CommInfo.portnum))
         {
         i++;
         jl_char = 0;
         RCVCHR14 (CommInfo.portnum, &jl_char, &status);

// throw away garbage chars if online
// this code used to fix garbage chars on screen for int14 through novell
// INT14_ERR_MASK seems to be a MAGIC number.  Don't ask why but works.
// We didn't need to do this for 1.70

            if (Olstatus && (status & INT14_ERR_MASK))
               {                                      
               continue;                              
               }

// end of magic code

         if ((*CommInfo.buff_tail = ((CHAR)(jl_char & 0x007F))) != '\0')
            {
            CommInfo.buff_cnt++;
            CommInfo.buff_tail++;
            if ((CommInfo.buff_tail - CommInfo.buffer) >= COMM_BUFFER_SIZE)
               {
               CommInfo.buff_tail = CommInfo.buffer;
               }
            }
         }

      busy = FALSE;
      }   

   }
