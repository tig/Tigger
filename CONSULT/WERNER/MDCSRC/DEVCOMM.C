/*****************************************************************************
* FILE NAME:  devcomm.c                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Modem, Novell ACS2, NETBIOS ACS2, and INT 14 Devices   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Init      - Initialize comm device                                   *
*   Dev_Open      - Open comm device                                         *
*   Dev_Close     - Close comm device                                        *
*   Dev_ReadCh    - Read a character from the comm device                    *
*   Dev_ReadStr   - read a string from the comm device                       *
*   Dev_WriteCh   - Write a character to the comm device                     *
*   Dev_WriteStr  - Write a string to the comm device                        *
*   Dev_Break     - Send a break signal                                      *
*   Dev_Flush     - Flush the input and ouput of the comm device             *
*   Dev_LineCheck - Check the phone line to see if still active              *
*   Dev_CharReady - Check to see if a character is ready to be read          *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   COMMTABLE    - A look-up table containing function pointers              *
*   COMMDATA     - General information about the communications              *
*                                                                            *
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


/*
 *  $Log:   N:/src/devcomm.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 11:03:16   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:24:38   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.1   21 Nov 1990 14:16:30   joeb
 * touch up where phone_flag is declared for the comm lib
 * 
 *    Rev 6.0   19 Nov 1990  9:29:06   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:13:38   andrews
 * No change.
 * 
 *    Rev 4.1   05 Oct 1990 15:34:02   deanm
 * Initial revision.
 * 
 *    Rev 4.0   01 Oct 1990 13:20:24   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.0   17 Sep 1990 10:02:20   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:16:02   andrews
 * iteration 1
 * 
 *    Rev 0.4   25 Aug 1990 18:44:30   deanm
 * cleaning up novell and int14
 * 
 *    Rev 0.3   04 Aug 1990 12:07:34   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.2   30 Jul 1990  9:21:48   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.1   25 Jul 1990 14:52:34   deanm
 * general update
 *
*/



#include <string.h>

#include "globals.h"
#include "devcomm.h"
#include "devdata.h"
#include "modem.h"
#include "int14.h"
#include "novell.h"
#include "netbios.h" 
#include "deverror.h"


/* declare globals for device communications */

COMMDATA   CommInfo;
BOOLEAN  Phone_Flag;

static COMMTABLE  CommFunctions [MAXDEVICES] =
   {  

      /* comm device routines for PAD device */
      ModemInit,   ModemOpen,      ModemClose,
      ModemReadCh, ModemReadStr,   ModemWriteCh,   ModemWriteStr,
      ModemBreak,  ModemFlush,     ModemLineCheck, ModemCharReady,

      /* comm device routines for MODEM device */
      ModemInit,   ModemOpen,      ModemClose,
      ModemReadCh, ModemReadStr,   ModemWriteCh,   ModemWriteStr,
      ModemBreak,  ModemFlush,     ModemLineCheck, ModemCharReady,

      /* comm device routines for NULL device */
      ModemInit,   ModemOpen,      ModemClose,
      ModemReadCh, ModemReadStr,   ModemWriteCh,   ModemWriteStr,
      ModemBreak,  ModemFlush,     ModemLineCheck, ModemCharReady,

      /* comm device routines for NOVELL ACS2 device */
      NovellInit,   NovellOpen,      NovellClose,
      NovellReadCh, NovellReadStr,   NovellWriteCh,   NovellWriteStr,
      stub,         NovellFlush,     NovellLineCheck, NovellCharReady,

      /* comm device routines for NETBIOS ACS2 device */
      NetbiosInit,   NetbiosOpen,    NetbiosClose, 
      NetbiosReadCh, NetbiosReadStr, NetbiosWriteCh, NetbiosWriteStr, 
      stub,          NetbiosFlush,   stub,           NetbiosCharReady,

      /* comm device routines for INT14 (timer tick) device */
      INT14TickInit,   INT14Open,        INT14TickClose, 
      INT14TickReadCh, INT14TickReadStr, INT14WriteCh,   INT14WriteStr, 
      stub,            INT14TickFlush,   INT14LineCheck, INT14TickCharReady,

      /* comm device routines for DEC PCSA device */
      stub,      stub,      stub, 
      rdch_stub, rdst_stub, wtch_stub, wtst_stub,
      stub,      stub,      stub,      bool_stub,

      /* comm device routines for INT14 (polling) device */
      INT14PollInit,   INT14Open,        INT14PollClose, 
      INT14PollReadCh, INT14PollReadStr, INT14WriteCh,   INT14WriteStr, 
      stub,            INT14PollFlush,   INT14LineCheck, INT14PollCharReady
      };





/*****************************************************************************
* FUNCTION NAME:  Dev_Init                                                   *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Initializes a comm device                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_Init (device, portnum, speed, parity, servName, targetName);*
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   INT  device      - Device number (PAD, MODEM, NULL, NOVELL, NETBIOS,     *
*                                     INT14TICK, INT14POLL, PCSA)            *
*   INT  portnum     - Port number (COM1, COM2, COM3, COM4)                  *
*   INT  speed       - Speed (9600, 4800, 2400, 1200, 300)                   *
*   INT  parity      - Parity (NONE, ODD, EVEN, MARK, SPACE)                 *
*   CHAR *servName   - Comm Server Logical name                              *
*   CHAR *targetName - Comm Server Logical Target name                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Init initializes the CommInfo data structure and, if necessary,      *
*   initializes the communication serial port.                               *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




INT  Dev_Init (device, portnum, speed, parity, cd_check, servName, 
               targetName, dialPrefix, phoneNumber)

INT  device;     
INT  portnum;    
INT  speed;      
INT  parity;     
INT  cd_check;
CHAR *servName;  
CHAR *targetName;
CHAR *dialPrefix;
CHAR *phoneNumber;

   {

   if (device < 0   ||   device >= MAXDEVICES  ||  device == DEV_PCSA)
      {
      return (DEVICE_OUT_OF_RANGE);
      }


   CommInfo.dev_type  = (BYTE)    device;
   CommInfo.portnum   = (BYTE)    portnum;
   CommInfo.speed     =           speed;
   CommInfo.parity    = (BYTE)    parity;
   CommInfo.cd_check  = (BOOLEAN) cd_check;

   strcpy (CommInfo.servName, servName);
   strcpy (CommInfo.targetName, targetName);
   strcpy (CommInfo.dialPrefix, dialPrefix);
   strcpy (CommInfo.phoneNumber, phoneNumber);

   CommInfo.buffer[0] = '\0';
   CommInfo.buff_tail = CommInfo.buffer;
   CommInfo.buff_head = CommInfo.buffer;
   CommInfo.buff_cnt  = 0;

   return ( (CommFunctions[device].init (&CommInfo)) );
   }





/*****************************************************************************
* FUNCTION NAME:  Dev_Open                                                   *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Open a comm device                                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_Open ();                                                    *
*                                                                            *
*   INT  status - returned function status                                   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Open opens the communication device and port specified in the        *
*   Dev_Init function call.                                                  *
*                                                                            *
*                                                                            *
* CAUTIONS: none                                                             *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




INT   Dev_Open ()

   {
   return ( (CommFunctions[CommInfo.dev_type].open) (&CommInfo) );
   }





/*****************************************************************************
* FUNCTION NAME:  Dev_Close                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Closes a comm device                                                     *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_Close ();                                                   *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Close closes the communication device specified in the Dev_Init      *
*   function call.                                                           *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT   Dev_Close ()

   {
   return ( (CommFunctions[CommInfo.dev_type].close) (&CommInfo) );
   }





/*****************************************************************************
* FUNCTION NAME:  Dev_ReadCh                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Reads a character from a comm device                                     *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_ReadCh (ch);                                                *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*   CHAR *ch   - character pointer to return character                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   If a character is ready, Dev_ReadCh will return the character in ch.     *
*   If no character is ready, Dev_ReadCh will return a null character in ch. *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A non-zero return value indicates an error has occurred.  If no          *
*   character is ready, ch will contain a null character and status will     *
*   contain a zero (successful).                                             *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT   Dev_ReadCh (ch)

CHAR *ch;

   {

   return ( (CommFunctions[CommInfo.dev_type].readch) (&CommInfo, ch) );
   }





/*****************************************************************************
* FUNCTION NAME:  Dev_ReadStr                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Read a string from the comm device                                       *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_ReadStr (string, length);                                   *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*   CHAR *string - character string to return the characters                 *
*   INT  length  - maximum number of characters to read                      *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   If there are characters to read, DevReadStr will place at most Length    *
*   number of characters in String and terminate String with a null          *
*   character.  If no characters are ready, String will be a zero length     *
*   character string.                                                        *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT   Dev_ReadStr (string, length)
         
CHAR *string;
INT  length;

   {
   strnset (string, 's', (size_t) length);
   string [length] = '\0';

   return ( (CommFunctions[CommInfo.dev_type].readstr) (&CommInfo, string, 
                                                      length) );
   }


/*****************************************************************************
* FUNCTION NAME:  Dev_WriteCh                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Writes a character to the comm device                                    *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_WriteCh (ch);                                               *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*   CHAR ch    - the character to write                                      *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   This routine will output a character to the comm device.                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




INT   Dev_WriteCh (ch)

CHAR  ch;

   {
   return ( (CommFunctions[CommInfo.dev_type].writech) (&CommInfo, ch) );
   }





/*****************************************************************************
* FUNCTION NAME:  Dev_WriteStr                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Writes a string to the comm device                                       *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_WriteStr (string);                                          *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_WriteStr will write string to the comm device.                       *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT   Dev_WriteStr (string)

CHAR  *string;

   {
   return ( (CommFunctions[CommInfo.dev_type].writestr) (&CommInfo, string) );
   }





/*****************************************************************************
* FUNCTION NAME:  Dev_Break                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Send a Break                                                             *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_Break ();                                                   *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Break will send a break to the comm port                             *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




INT   Dev_Break ()

   {
   return ( (CommFunctions[CommInfo.dev_type].sendbreak) (&CommInfo) );
   }




/*****************************************************************************
* FUNCTION NAME:  Dev_Flush                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Flush all input and output buffers                                       *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_Flush ();                                                   *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_Flush will empty the input and the output communication buffers.     *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




INT   Dev_Flush ()

   {
   return ( (CommFunctions[CommInfo.dev_type].flush) (&CommInfo) );
   }




/*****************************************************************************
* FUNCTION NAME:  Dev_LineCheck                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Check the phone line to make sure it is active                           *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_LineCheck ();                                               *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_LineCheck will check the phone lines to verify carrier detect (CD).  *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   The returned INT value is 0 if the initialization succeeded and          *
*   non-zero if the initialization failed.                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




INT   Dev_LineCheck ()

   {
   return ( (CommFunctions[CommInfo.dev_type].linecheck) (&CommInfo) );
   }




/*****************************************************************************
* FUNCTION NAME:  Dev_CharReady                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Check for any characters ready to be read                                *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Dev_CharReady ();                                               *
*                                                                            *
*   INT status - INT value to return function status                         *
*                                                                            *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Dev_CharReady will check for any characters that have been buffered      *
*   and are ready to be read.                                                *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   A FALSE return value indicates no characters are ready for reading       *
*   while a TRUE return value indicates at least 1 character is ready.       *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/




BOOLEAN   Dev_CharReady ()

   {
   return ( (CommFunctions[CommInfo.dev_type].charready) (&CommInfo) );
   }






/* the routines displayed below are stub routines used only in this module */


/* A comm stub routine */

INT  stub (commInfo)

COMMDATA *commInfo;

   {
   return (SUCCESS);
   }




/* A comm stub routine */

INT  rdch_stub (commInfo, ch)

COMMDATA *commInfo;
CHAR     *ch;

   {
   return (SUCCESS);
   }




/* A comm stub routine */

INT  rdst_stub (commInfo, string, length)

COMMDATA *commInfo;
CHAR     *string;
INT      length;

   {
   return (SUCCESS);
   }




/* A comm stub routine */

INT  wtch_stub (commInfo, ch)

COMMDATA *commInfo;
CHAR     ch;

   {
   return (SUCCESS);
   }




/* A comm stub routine */

INT  wtst_stub (commInfo, string)

COMMDATA *commInfo;
CHAR     *string;

   {
   return (SUCCESS);
   }




/* A comm stub routine */

BOOLEAN  bool_stub (commInfo)

COMMDATA *commInfo;

   {
   return (TRUE);
   }
