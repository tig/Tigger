/*****************************************************************************
* FILE NAME:  netbios.c                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Comm Routines for Netbios                                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosInit   - Initialize netbios communications                        *
*   NetbiosOpen   - Open netbios communications                              *
*   NetbiosClose  - Close netbios comm.                                      *
*   NetbiosReadCh - Read a character                                         *
*   NetbiosReadStr- read a string                                            *
*   NetbiosWriteCh- Write a character                                        *
*   NetbiosWriteStr-Write a string to the comm device                        *
*   NetbiosFlush  - Flush the input and ouput buffers                        *
*   NetbiosCharReady - Check to see if a character is ready to be read       *
*                                                                            *
*                                                                            *
* CAUTION:                                                                   *
*                                                                            *
*  ! ! ! ! ! ! ! ! ! ! ! !  W A R N I N G ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !    *
*                                                                            *
*  This module  MUST  be compiled with stack checking  OFF.  With            *
*  Microsoft C, the compiler switch to do this is   /Gs.  To ignore          *
*  this warning can cause serious side effects including loss of data,       *
*  workstation lockups, and cancer.                                          *
*                                                                            *
*  ! ! ! ! ! ! ! ! ! ! ! !  W A R N I N G ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !    *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
 ***************************************************************************
 *             M E A D   D A T A   C E N T R A L,   I N C.                 *
 *                             CONFIDENTIAL                                *
 *                                                                         *
 *  This document is the property of Mead Data Central, Inc. (MDC)         *
 *  and its contents are proprietary to MDC.  Reproduction in any form     *
 *  by anyone of the materials combined herein without the permission      *
 *  of MDC is prohibited.  Finders are asked to return this document to    *
 *  Mead Data Central, Inc. at 3445 Newmark Dr.,  Miamisburg, OH  45342    *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *                       Modification Log                                  *
 *  $Log:   N:/src/netbios.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:54:32   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:16:10   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:20:46   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:04:38   andrews
 * No change.
 * 
 *    Rev 4.1   05 Oct 1990 15:32:50   deanm
 * updating comments
 * 
 *    Rev 4.0   01 Oct 1990 13:22:34   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.1   20 Sep 1990 11:37:58   deanm
 * fixing some disconnect problems
 * 
 *    Rev 3.0   17 Sep 1990 10:04:08   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:18:10   andrews
 * iteration 1
 * 
 *    Rev 0.2   04 Aug 1990 12:07:56   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.1   30 Jul 1990  9:22:22   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:38   deanm
 * Initial revision.
 * 
 **************************************************************************/

#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <bkeybrd.h>
#include <butil.h>

#include "globals.h"
#include "devcomm.h"
#include "devdata.h"
#include "netbios.h"
#include "deverror.h"
#include "systool.h"


/* states for nb_dial */
#define STATUS_STATE    0
#define ADD_NAME_STATE  1
#define LISTEN_STATE    2
#define RCV_DG_STATE    3
#define SND_DG_STATE    4
#define POST_STATE      5


TYPE_NB_ISR_GLOB isr_glob;
TYPE_NCB    ncb_send;
TYPE_NCB    ncb_receive_dg;
TYPE_NCB    ncb_listen; 
TYPE_NCB    ncb_send_dg;
TYPE_NCB    general_ncb_rec;
CHAR        rcv_dg_buff[STAT_BUFF_LEN];
CHAR        status_buff[STAT_BUFF_LEN];
CHAR        ncbname[NCB_NAME_LEN + 1];
INT         net_num;
INT         connected;
BOOLEAN     off_hook;



INT  nb_accInp (BOOLEAN);  /* prototype for internal accinp function */



/* For information about NETBIOS application protocols, consult   */
/* IBM Asynchronous Server Communications Protocol and the        */
/* IBM NETBIOS Application Development Guide (little green book). */






/*****************************************************************************
* FUNCTION NAME:  NetbiosInit                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Initializes netbios comm.                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosInit (commInfo);                                         *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosInit initializes the commInfo data structure for netbios          *
*   communications.                                                          *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS                                                                  *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT  NetbiosInit (commInfo)

COMMDATA *commInfo;

   {

/* init globals */
   isr_glob.postflag.ncbs[RCV_DG_POST] = NULL;
   isr_glob.postflag.ncbs[LISTEN_POST] = NULL;
   isr_glob.postflag.ncbs[RECEIVE_POST] = NULL;
   isr_glob.postflag.ncbs[SEND_POST] = NULL;
   isr_glob.broken_packet = NCB_OK;
   isr_glob.another_buff[0] = '\0';
   isr_glob.lsnum = NCB_OK;
   connected = FALSE;
   off_hook = TRUE;
   return(NCB_OK);
   }





/*****************************************************************************
* FUNCTION NAME:  NetbiosOpen                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   opens the   netbios comm.                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosOpen (commInfo);                                         *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosOpen opens the netbios comm server for virtual circuit connection.*
*   This open routine returns when the modem is done dialing, the user       *
*   interrupts the dialing process, or an error condition occurs.            *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT  NetbiosOpen (commInfo)

COMMDATA    *commInfo;

   {

   INT         nb_state;
   INT         stat;
   INT         nberr;
   INT         inkey;
   BOOLEAN     remote;
   CHAR        buffer [STAT_BUFF_LEN];
   CHAR        string [COLS + 1];
   TYPE_NCB    ncb;


/* connects to comm. server and dials modem */
   remote = FALSE;
   stat = NCB_OK;
   nb_state = STATUS_STATE;


/* begin connection process */
   while ((stat == NCB_OK) && (nb_state <= POST_STATE))
      {
      switch (nb_state)
         {
         case STATUS_STATE :
            stat = nb_status(&ncb);
            nb_state++;
            break;

         case ADD_NAME_STATE :
            if ((stat = add_name(&ncb)) == NCB_OK)
               net_num = ncb.ncb_num;
            nb_state++;
            break;

         case RCV_DG_STATE :
            stat = rcv_data_gram();
            nb_state++;
            break;

         case LISTEN_STATE :
            stat = listen();
            nb_state++;
            break;

         case SND_DG_STATE :
            command_vector (buffer, CONNECT_REQ, commInfo);
            if ((stat = send_data_gram (buffer, 
                  commInfo->servName)) == NCB_OK)
               isr_glob.postflag.index = LISTEN_POST;
            nb_state++;
            break;

         case POST_STATE :
            stat = post_handler(commInfo, &nberr, string, &remote);

            if (remote == TRUE)
               {
               nb_state++;
               stat = NCB_OK;
               }
            break;

         default :
            stat = NB_UNUSUAL_COND;
            break;
         }

      if (stat == NCB_OK)
         {
         inkey = nb_accInp (FALSE);
         if (inkey == ESCAPE_KEY)
            {
            stat = ESCAPE;
            }
         if (inkey == QUIT_KEY)
            stat = QUIT;
         }
      }

   return(stat);
   }





/*****************************************************************************
* FUNCTION NAME:  NetbiosClose                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   closes the  netbios comm.                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosClose (commInfo);                                        *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosClose terminates the session with the OLS, comm server, and       *
*   LAN.                                                                     *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT  NetbiosClose (commInfo)

COMMDATA *commInfo;

   {

   INT   status;


   NetbiosFlush (commInfo);
   status = hang_up();

   if ((status == NB_ADAPTER_MALFUNC) || (status == NB_UNUSUAL_COND))
      {
      return (status);
      }

   status = delete_name();
   status = reset (&general_ncb_rec);

   if ((status == NB_ADAPTER_MALFUNC) || (status == NB_UNUSUAL_COND))
      {
      return (status);
      }

   Sys_snooze (1);
   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NetbiosReadCh                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   reads a character                                                        *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosReadCh (commInfo, ch);                                   *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *ch       - character to return is placed in here               *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosReadCh looks in the commInfo buffer for a character to return.    *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/






INT  NetbiosReadCh  (commInfo, ch)

COMMDATA *commInfo;
CHAR     *ch;

   {


   if (commInfo->buff_head != commInfo->buff_tail)
      {
      *ch = *commInfo->buff_head++;
      if (commInfo->buff_head >= commInfo->buffer + COMM_BUFFER_SIZE)
         {
         commInfo->buff_head = commInfo->buffer;
         }
      }
   else 
      {
      *ch = '\0';
      }

   return (SUCCESS);
   }



   

/*****************************************************************************
* FUNCTION NAME:  NetbiosReadStr                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   reads a character string                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosReadStr (commInfo, string, length);                      *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - character string returned through here              *
*   INT      length    - max number of chars to return through string        *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosReadStr looks in the commInfo buffer for a string to return.      *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/






INT  NetbiosReadStr  (commInfo, string, length)

COMMDATA *commInfo;
CHAR     *string;
INT      length;

   {

   CHAR  *temp;

   temp = string;
   do
      {
      NetbiosReadCh (commInfo, temp);
      } while (*temp++ != '\0'  &&  --length > 0);
   *temp = '\0';

   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NetbiosWriteCh                                             *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   writes a character                                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosWriteCh (commInfo, ch);                                  *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     ch        - character to send                                   *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosWriteCh writes a character to the netbios stuff.                  *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/






INT  NetbiosWriteCh (commInfo, ch)

COMMDATA *commInfo;
CHAR     ch;

   {

   CHAR string[7];

   memcpy (string, "\00\05\357\013", 4);
   string[4] = ch;

   return(send(string));
   }






/*****************************************************************************
* FUNCTION NAME:  NetbiosWriteStr                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   writes a string to the netbios device                                    *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosReadStr (commInfo, string);                              *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*   CHAR     *string   - character string to write                           *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosWriteStr writes the passed in character string to the netbios     *
*   device.                                                                  *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT  NetbiosWriteStr (commInfo, string)

COMMDATA *commInfo;
CHAR     *string;

   {

   INT  len;
   INT  status;
   CHAR temp[RCV_BUFF_LEN + 6];
   CHAR *sptr;
   BOOLEAN moreToSend;


   sptr = string;
   do
      {
      len = (INT) strlen (sptr);
      if (len > RCV_BUFF_LEN)
         {
         len = RCV_BUFF_LEN;
         moreToSend = TRUE;
         }
      else
         {
         moreToSend = FALSE;
         }

      temp[0] = '\0';
      temp[1] = (CHAR)(len + 4);
      temp[2] = '\357';
      temp[3] = '\013';
      temp[4] = '\0';
      strncat (temp+4, sptr, len);
      sptr += len;

      status = send (temp);

      } while (status == SUCCESS  &&  moreToSend == TRUE);

   return (SUCCESS);
   }






/*****************************************************************************
* FUNCTION NAME:  NetbiosFlush                                               *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   flush out the input buffers for netbios                                  *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosFlush (commInfo);                                        *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosFlush resets all of the buffer pointers in commInfo which         *
*   effectively erases all data in the buffers.                              *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





INT  NetbiosFlush (commInfo)

COMMDATA *commInfo;

   {

   commInfo->buff_head = commInfo->buff_tail = commInfo->buffer;
   return (SUCCESS);
   }




/*****************************************************************************
* FUNCTION NAME:  NetbiosCharReady                                           *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   checks for any characters ready to be read                               *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = NetbiosCharReady (commInfo);                                    *
*                                                                            *
*   BOOLEAN status     - TRUE if character pending to read                   *
*                                                                            *
*   COMMDATA *commInfo - comm info define in devcomm.c                       *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   NetbiosCharReady checks the buffers in commInfo for any characters       *
*   ready to be read.                                                        *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT, an error code from DEVERROR.H                     *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/02/90                                                            *
*                                                                            *
*****************************************************************************/





BOOLEAN NetbiosCharReady (commInfo)

COMMDATA *commInfo;

   {

   if (commInfo->buff_head != commInfo->buff_tail)
      {
      return (TRUE);
      }
   else
      {
      return (FALSE);
      }

   }




/* the following are all internal netbios routines */


/* add_name adds a name to the netbios name table */


INT 
add_name(ncb)
   TYPE_NCB  *ncb;
   {

   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   ncb->ncb_command = NCB_ADD_NAME;
   ncb->ncb_lana_num = PRIM_ADAPT;
   init_ncbname(ncb->ncb_name);

   nb_netbios (ncb);
   return(nb_error(ncb->ncb_retcode));
   }




INT 
call(callname)
   CHAR      *callname;
   {
   TYPE_NCB  *ncb;

   ncb = &ncb_send;
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   memcpy((CHAR *)ncb->ncb_callname, callname,NCB_NAME_LEN);
   get_ncbname(ncb->ncb_name);
   ncb->ncb_command = NCB_CALL;
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   isr_glob.lsnum = ncb->ncb_lsn;
   return(nb_error(ncb->ncb_retcode));
   }



/* delete_name deletes the current name from the name table */

INT
delete_name()
   {
   TYPE_NCB some_ncb;
   TYPE_NCB *ncb;

   if (!off_hook)
      {
      return (NCB_OK);
      }
   off_hook = FALSE;

   ncb = &some_ncb;
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   get_ncbname(ncb->ncb_name);
   ncb->ncb_command = NCB_DEL_NAME;
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   connected = FALSE;
   isr_glob.lsnum = 0;
   return(nb_error(ncb->ncb_retcode));
   }



INT
get_ncb_num(net_num)
   INT *net_num;
   {
   INT num_entry;
   INT buffer_index;
   INT i;

   num_entry = status_buff[58];
   for (i=0; i < num_entry; i++) 
      {
      buffer_index= 70+(18*i);
      if (memcmp(&status_buff[buffer_index],&status_buff[0],6) == 0)
         *net_num = status_buff[buffer_index+6];
      }
   return(SUCCESS);
   }


void
get_ncbname(name)
   CHAR *name;
   {

   memcpy(name, ncbname, NCB_NAME_LEN);
   }



/* hang_up disconnects the session between the server and workstation */

INT
hang_up()
   {
   TYPE_NCB some_ncb;
   TYPE_NCB *ncb;

   if (!off_hook)
      {
      return (NCB_OK);
      }

   ncb = &some_ncb;
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   ncb->ncb_command = NCB_HANG_UP;
   ncb->ncb_lana_num = PRIM_ADAPT;
   ncb->ncb_lsn = isr_glob.lsnum;

   isr_glob.lsnum = NCB_OK;
   nb_netbios (ncb);
   return(nb_error(ncb->ncb_retcode));
   }


void
init_ncbname(name)
   CHAR *name;
   {
   TYPE_NCB    stat_ncb;
   INT         low, middle;
   CHAR        card_num[31];

   memset (ncbname, ' ', NCB_NAME_LEN);
   memcpy (ncbname, "MDC17_", 6);
   nb_status (&stat_ncb);
   memcpy ((CHAR *)&low, status_buff, 2);
   memcpy ((CHAR *)&middle, status_buff+2, 2);
   sprintf (card_num, "%.4X%.4X", middle, low);
   memcpy (ncbname+6, card_num, 8);
   memcpy (name, ncbname, NCB_NAME_LEN);
   }



/* listen posts a listen so that the server knows we are ready to talk  */
/* this routine is only executed when the session is initiated with the */
/* comm server.                                                         */


INT 
listen()
   {
   TYPE_NCB *ncb;

   ncb = &ncb_listen;
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   memset((CHAR *)ncb->ncb_callname, ' ', NCB_NAME_LEN);
   ncb->ncb_callname[0] = '*';
   get_ncbname(ncb->ncb_name);
   ncb->ncb_command = NCB_LISTEN;
   ncb->ncb_rto = NO_TIMEOUT;
   ncb->ncb_sto = NO_TIMEOUT;
   ncb->ncb_lana_num = PRIM_ADAPT;
   ncb->ncb_post = post;

   return(nb_error(nb_netbios(ncb)));
   }



/* using a copy of Sys_accInp here so that we do not have to include */
/* systool.c in the comm library                                     */


INT  nb_accInp  (waiting)

BOOLEAN  waiting;

   {

   INT   retCode = NO_KEY;
   INT   pkey;
   CHAR  pch;

   do
      {
      if (kbready (&pch, &pkey) )
         {
         retCode = kbgetkey (&pkey);
         retCode |= pkey << 8;     /* hi byte is key code, lo byte is char */
         }
      } while (waiting  &&  retCode == NO_KEY);
         
   return (retCode);
   }




/* error parsing routine */

INT
nb_error(err_num)
   INT   err_num;
   {
   INT   status;

   switch(err_num)
      {
      case NCB_OK:
      case BROKEN_PACKET:
      case COMMAND_CANCEL:
      case NAME_DELETED:
      case NCB_WAIT:
         status = SUCCESS;
         break;
      case TIMED_OUT:
      case SESS_CLOSED:
      case NB_ABEND:
         status = NB_TERMINATED;
         connected = FALSE;
         break;
      case BAD_LSN:
         status = NB_TERMINATED;
         break;
      case DUPLICATE_NAME:
      case NAME_IN_USE:
         status = NB_NAME_IN_USE;
         break;
      case NO_ANSWER:
         status = NB_NO_ANSWER;
         break;
      case NAME_TABLE_FULL:
      case SESS_TABLE_FULL:
      case BAD_NAME:
      case NO_RESOURCE:
      case NAME_DEREG:
      case BAD_BUFF_LEN:
      case BAD_COMMAND:
      case BUSTED_ADAPTER:
      case BAD_OPEN:
      case NB_BUSY:
      case BAD_NAME_NUM:
      case NAME_CONFLICT:
      case BAD_PROTOCOL:
      case NB_BACKED_UP:
      case WRONG_ADAPTER:
      case BAD_CANCEL:
      case CANNOT_CANCEL:
      case HOT_CD_FOUND:
      case HOT_CD_SENT:
      case NO_CARRIER:
      case UNUSUAL_NETWORK:
      case BAD_STAT_BIT:
      case EXTRA_STAT_BIT:
         status = NB_UNUSUAL_COND;
         break;
      case NETBIOS_ABSENT:
         status = NB_NOT_LOADED;
         break;
      default:
         status = NB_ADAPTER_MALFUNC;
         break;
      }
   return(status);
   }



/* this routine is a receive call back routine.  I receives the data and */
/* places it into the commInfo buffers.  It then prepares another NCB    */
/* to receive more data and this prepared NCB is passed back to NETCALL  */
/* so that NETCALL can send the NCB to the comm server.  NETCALL is the  */
/* assembler routine that handles stack manipulation.                    */


TYPE_NCB _far *nb_isr (ncbptr, ret_code)

TYPE_NCB _far *ncbptr;
INT           ret_code;

   {
   extern COMMDATA CommInfo;   /* delcared because can't pass through */
   TYPE_NCB *ncb;

   isr_glob.postflag.ncbs[RECEIVE_POST] = ncbptr;
   if (isr_glob.lsnum != 0 && nb_error (ret_code) == NCB_OK)   /* receive if lsnum is valid */
      {
      nb_rcv_str (ncbptr, ret_code);

      ncb = &isr_glob.ncb_receive;
      ncb->ncb_command = NCB_RECEIVE;
      ncb->ncb_lsn = isr_glob.lsnum;
      ncb->ncb_buffer = (CHAR _far *) isr_glob.rcv_buff;
      ncb->ncb_lana_num = PRIM_ADAPT;
      ncb->ncb_length = STAT_BUFF_LEN;
      ncb->ncb_post = post_rcv;
/*      nb_receive();  now done in NETCALL.ASM */
      }
   else
      {
      *CommInfo.buff_tail = '\033';  /* stuff an esc-D in the buffers to allow */
      CommInfo.buff_tail++;          /* rest of software to quit */
      if (CommInfo.buff_tail >= CommInfo.buffer + COMM_BUFFER_SIZE)
         {
         CommInfo.buff_tail = CommInfo.buffer;
         }
      *CommInfo.buff_tail = 'D';
      CommInfo.buff_tail++;

      ncb = NULL;
      }

   return (ncb);
   }



/* issue a netbios call */

INT
nb_netbios (ncb)

TYPE_NCB *ncb;
   {
   union    REGS inregs, outregs;
   struct   SREGS segregs;

   segread (&segregs);
   inregs.x.bx = FP_OFF(ncb);
   segregs.es  = utseg (ncb);
   return (int86x(NETBIOS, &inregs, &outregs, &segregs));
   }


/* check for an incoming receive packet to have completed */
/* THIS ROUTINE MAY BE OBSOLETE AS OF 10/5/90            */

BOOLEAN
nb_rcv_ready()
   {

   if ((isr_glob.postflag.ncbs[RECEIVE_POST] == NULL) ||
       (isr_glob.ncb_receive.ncb_cmd_cplt == NCB_WAIT))
      return(FALSE);

   if ((isr_glob.ncb_receive.ncb_cmd_cplt != NCB_OK) &&
       (isr_glob.ncb_receive.ncb_cmd_cplt != BROKEN_PACKET))
      return(isr_glob.ncb_receive.ncb_cmd_cplt);   /* return error code */

   return(TRUE);
   }



/* remove the string from the receive buffer and place in commInfo buffers */

INT
nb_rcv_str (ncbptr, ret_code)

TYPE_NCB _far *ncbptr;
INT           ret_code;

   {
   INT  i=0;
   INT  len;
   extern COMMDATA CommInfo;

   len = ncbptr->ncb_length;

   if (isr_glob.broken_packet != BROKEN_PACKET)
      {
      i = 4;            /* get past header section of receive_buff */
      len -= 4;
      }
   isr_glob.broken_packet = (BYTE) ret_code;

   while (len > 0)
      {
      *CommInfo.buff_tail = isr_glob.rcv_buff[i++];
      if (++CommInfo.buff_tail >= CommInfo.buffer + COMM_BUFFER_SIZE)
         {
         CommInfo.buff_tail = CommInfo.buffer;
         }
      len--;
      }

   return (NCB_OK);
   }



/* start off the receive perpetual motion machine */

INT 
nb_receive()
   {
   TYPE_NCB *ncb;

   ncb = &isr_glob.ncb_receive;
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   ncb->ncb_command = NCB_RECEIVE;
   ncb->ncb_lsn = isr_glob.lsnum;
   ncb->ncb_buffer = (CHAR _far *) isr_glob.rcv_buff;
   ncb->ncb_lana_num = PRIM_ADAPT;
   ncb->ncb_length = STAT_BUFF_LEN;
   ncb->ncb_post = post_rcv;

   return(nb_error(nb_netbios(ncb)));
   }


INT 
nb_status(ncb)
   TYPE_NCB  *ncb;
   {

   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   memset((CHAR *)ncb->ncb_callname, ' ', NCB_NAME_LEN);
   ncb->ncb_command = NCB_STATUS;
   ncb->ncb_callname[0] = '*';
   ncb->ncb_buffer = (CHAR _far *) status_buff;
   ncb->ncb_length = STAT_BUFF_LEN;
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   return(nb_error(ncb->ncb_retcode));
   }



/* this routine is used for debugging purposes only */

void 
printncb(ncb)
   TYPE_NCB  *ncb;
   {
   INT i;

   printf("\nbuffer = ");
   for (i = 0; i < ncb->ncb_length; i++)
      printf("%2x",ncb->ncb_buffer[i]);

   printf("\npost =   %p",ncb->ncb_post);
   printf("\ncommand  %x",ncb->ncb_command);
   printf("\nretcode  %x",ncb->ncb_retcode);
   printf("\nncb_num  %d",ncb->ncb_num);
   printf("\nncb_lsn  %d",ncb->ncb_lsn);
   printf("\nncb_name %s",ncb->ncb_name);
   printf("\ncomplete %d",ncb->ncb_cmd_cplt);
   }


INT 
rcv_data_gram()
   {
   TYPE_NCB *ncb;

   ncb = &ncb_receive_dg;
   memset(rcv_dg_buff, 0x0, STAT_BUFF_LEN);
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   ncb->ncb_command = NCB_RCV_DG;
   ncb->ncb_lana_num = PRIM_ADAPT;
   ncb->ncb_num = (BYTE)net_num;
   ncb->ncb_buffer = (CHAR _far *) rcv_dg_buff;
   ncb->ncb_length = STAT_BUFF_LEN;
   ncb->ncb_post = post;

   return(nb_error(nb_netbios(ncb)));
   }


INT 
reset(ncb)
   TYPE_NCB  *ncb;
   {

   if (!off_hook)
      {
      return (NCB_OK);
      }

   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   ncb->ncb_command = NCB_RESET;
   ncb->ncb_lsn = 6;  /* MAX_SESSION; */
   ncb->ncb_num = 12; /* MAX_NCB; */
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   return(nb_error(ncb->ncb_retcode));
   }



/* send a character string */

INT 
send(snd_buff)
   CHAR     *snd_buff;
   {
   TYPE_NCB *ncb;

   ncb = &ncb_send;
   if (isr_glob.postflag.ncbs[SEND_POST] != 0)
      isr_glob.postflag.ncbs[SEND_POST] = 0x0;

   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   ncb->ncb_command = NCB_SEND;
   ncb->ncb_lsn = isr_glob.lsnum;
   ncb->ncb_lana_num = PRIM_ADAPT;
   ncb->ncb_buffer = (CHAR _far *) snd_buff;
   ncb->ncb_length = snd_buff[1];

   nb_netbios (ncb);

   return (nb_error(ncb->ncb_retcode));
   }


INT 
send_data_gram(snd_dg_buff,server_name)
   CHAR     *snd_dg_buff;
   CHAR     *server_name;
   {
   TYPE_NCB *ncb;

   ncb = &ncb_send_dg;
   memset((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   memset((CHAR *)ncb->ncb_callname, ' ', NCB_NAME_LEN);
   memcpy((CHAR *)ncb->ncb_callname,server_name,strlen(server_name));
   ncb->ncb_command = NCB_SEND_DG;
   ncb->ncb_num = (BYTE)net_num;
   ncb->ncb_buffer = (CHAR _far *) snd_dg_buff;
   ncb->ncb_length = STAT_BUFF_LEN;
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   return(nb_error (ncb->ncb_retcode));
   }


#ifdef DEBUG

INT  sess_stat ()
   {
   TYPE_NCB *ncb;
   CHAR     sess_buff[STAT_BUFF_LEN];

   ncb = &general_ncb_rec;
   memset ((CHAR *)ncb, 0x0, sizeof (TYPE_NCB));
   ncb->ncb_command = NCB_SESS_STAT;
   get_ncbname (ncb->ncb_name);
   ncb->ncb_buffer = (CHAR _far *) sess_buff;
   ncb->ncb_length = STAT_BUFF_LEN;
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   printncb (ncb);
   return (nb_error (ncb->ncb_retcode));
   }


INT 
remote_stat()
   {
   TYPE_NCB  some_ncb;
   TYPE_NCB  *ncb;

   ncb = &some_ncb;
   memset ((CHAR *)ncb, 0x0, sizeof(TYPE_NCB));
   memcpy ((CHAR *)ncb->ncb_callname, "NAME1           ", NCB_NAME_LEN);
   ncb->ncb_command = NCB_STATUS;
   ncb->ncb_buffer = (CHAR _far *) status_buff;
   ncb->ncb_length = STAT_BUFF_LEN;
   ncb->ncb_lana_num = PRIM_ADAPT;

   nb_netbios (ncb);
   printncb (ncb);
   return (nb_error (ncb->ncb_retcode));
   }

#endif
