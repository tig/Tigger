/***************************************************************************
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
 *  $Log:   N:/src/post_han.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:54:42   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:16:22   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:20:58   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:04:48   andrews
 * No change.
 * 
 *    Rev 4.0   01 Oct 1990 13:22:40   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.1   20 Sep 1990 11:38:02   deanm
 * fixing some disconnect problems
 * 
 *    Rev 3.0   17 Sep 1990 10:04:24   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:18:32   andrews
 * iteration 1
 * 
 *    Rev 0.2   04 Aug 1990 12:08:00   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.1   30 Jul 1990  9:23:00   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:52   deanm
 * Initial revision.
 * 
 **************************************************************************/

#include <string.h>
#include <memory.h>
#include "globals.h"
#include "devdata.h"
#include "netbios.h"
#include "deverror.h"


extern BOOLEAN Phone_Flag;  // found in CSCRIPT.H (not included because
                            // CSCRIPT.H brings in several other headers


INT   post_handler (commInfo, nberr, callname, remote)

COMMDATA    *commInfo;
INT         *nberr;
CHAR        *callname;
BOOLEAN     *remote;

   {

   INT  i;
   INT  status;
   INT  agwcom;
   INT  retcode;
   INT  longmsg = FALSE;
   INT  net_num = 0;
   static INT listen_flag = FALSE;
   TYPE_NCB _far *ncbptr;
   CHAR snd_buff[STAT_BUFF_LEN];

   status = SUCCESS;
   switch (isr_glob.postflag.index) 
      {
      case LISTEN_POST:                        /** LISTEN **/
         ncbptr = isr_glob.postflag.ncbs[LISTEN_POST];
         if (ncbptr != NULL) 
            {
            listen_flag = TRUE;
            connected = TRUE;
            Phone_Flag = TRUE;    /* found in cscript.h */
            status = nb_receive();
            isr_glob.postflag.ncbs[LISTEN_POST] = 0x0;
            }
         if (status != SUCCESS)
            return(status);
         break;
      case RCV_DG_POST:                        /** REC DATAGRAM **/
         ncbptr = isr_glob.postflag.ncbs[RCV_DG_POST];
         if (ncbptr != NULL) 
            {
            listen_flag = TRUE;
            for (i = 0; i < 16; i++)
               {
               callname[i] = ncbptr->ncb_callname[i]; /* cannot use memcpy */
               }
            agwcom = ncbptr->ncb_buffer[3];
            switch (agwcom) 
               {
               case 9 :    /**  << Request Queued by Server>>  **/
/*                  set_attributes (REVERSE_ON);
 *                  write_screen (13,27," Request Queued by Server ");
 *                  set_attributes (REVERSE_OFF);
*/
                  break;
               case 1 :
                  status = call(callname);
                  if (status == NCB_OK) 
                     {
                     command_vector(snd_buff,CONNECT_ID,commInfo);
                     status = send(snd_buff);
                     if (status == NCB_OK) 
                        {     /**  << Connected To Server >>  **/
                        connected = TRUE;
                        }
                     }
                  break;
               }
            if (agwcom == 0xf) 
               {    /** << Connected Request Error >>  */
               status = NB_CONN_ERR;
               }
            if (connected == FALSE && *nberr == FALSE) 
               {
               status = rcv_data_gram();
               }
            isr_glob.postflag.ncbs[RCV_DG_POST] = 0x0;
            }
         if (status != SUCCESS)
             return(status);
         break;
      case RECEIVE_POST:                        /** receive **/
         ncbptr = isr_glob.postflag.ncbs[RECEIVE_POST];
         if (ncbptr != NULL) 
            {
            agwcom = ncbptr->ncb_buffer[3];
            switch(agwcom) 
               {
               case 0xb :
                  break;
               case 4 :
                  command_vector(snd_buff,CONNECT_ID_RESP,commInfo);
                  status = send(snd_buff);
                  break;
               case 5 :
                  command_vector(snd_buff,CONNECT_RESP,commInfo);
                  status = send(snd_buff);
                  *remote = TRUE;
                  break;
               case 6 :
               case 7 :
                  command_vector(snd_buff,CONNECT_PARM,commInfo);
                  status = send(snd_buff);
                  break;
               case 0xd :
/*
 *                retcode = ncbptr->ncb_buffer[8];
 *                if ((retcode & 0x20) != 0) 
 *                   printf("<< Server Buffer Overrun >>\n");
 *                if ((retcode & 0x10) != 0)
 *                   printf("<< Break Signal received   >>\n");
 *                if ((retcode & 8) != 0)
 *                   printf("<< Framing Error >>\n");
 *                if ((retcode & 4) != 0)
 *                   printf("<< Parity Error Detected >>\n");
 *                if ((retcode & 2) != 0)
 *                   printf("<< Server Adapter Overrun >>\n");
 *                if ((retcode & 1) != 0)
 *                   printf("<< Carrier Lost on Async Line >>\n");
 */
                  status = NB_SERVER_ERR;
                  break;
               case 0xe :
               case 0x8 :
               case 0x3 :
               case 0x2 :
                  /** << connection terminated by server >> **/
                  if (agwcom == 0xe)
                     status = NB_TERMINATED;

                  if (ncbptr->ncb_buffer[5] == 5) 
                     {
                     retcode = ncbptr->ncb_buffer[6];
                     switch (retcode) 
                        {
                        case 0x0 :
                           *remote = TRUE;
                           break;
                        case 0x1 :
                           /**    connection initiated by server **/
                           break;
                        case 0x6 :
                           /**   << Line Busy >>      **/
                           status = NB_LINE_BUSY;
                           break;
                        }
                     }
                  break;
               }     /** end switch **/
               isr_glob.postflag.ncbs[RECEIVE_POST] = 0x0;
            }   /* end of if listen_flag == true statement */
         if (status != SUCCESS)
            return(status);
         break;
      case SEND_POST:                       /** SEND **/
         ncbptr = isr_glob.postflag.ncbs[SEND_POST];
         if (ncbptr != NULL)
            {
            isr_glob.postflag.ncbs[SEND_POST] = 0x0;
            }
         if (status != SUCCESS)
            return(status);
         break;
      default:
         break;
      }
  
   isr_glob.postflag.index = (++isr_glob.postflag.index % 4);
   return(status);
   }
