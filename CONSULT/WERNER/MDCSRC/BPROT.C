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
 *  $Log:   N:/src/bprot.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 10:53:54   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:15:42   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:20:10   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:04:00   andrews
 * No change.
 * 
 *    Rev 4.0   01 Oct 1990 13:21:58   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.0   17 Sep 1990 10:03:42   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:17:38   andrews
 * iteration 1
 * 
 *    Rev 0.2   04 Aug 1990 12:07:40   deanm
 * making devcomm a stronger lib
 * 
 *    Rev 0.1   30 Jul 1990  9:21:58   deanm
 * completing comm device drivers and comm scripts
 * 
 *    Rev 0.0   25 Jul 1990 14:51:16   deanm
 * Initial revision.
 * 
 **************************************************************************/

/* bprot.c  --  command_vector,  this routine assebles command vectors  */
/*              look in IBM Asynchronous Communications Server Protocol */
/*              for a complete explaination of the octal character      */
/*              strings found in this file                              */


#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "globals.h"
#include "devcomm.h"
#include "devdata.h"
#include "netbios.h"
#include "deverror.h"

struct 
   {
   CHAR *lan_prty;
   INT   prty_value;
   } lparity[] = 
      {
      "\010",NOPARITY,        /* no parity    */
      "\200",ODDPARITY,       /* odd parity   */
      "\100",EVENPARITY,      /* even parity  */
      "\040",MARKPARITY,      /* mark parity  */
      "\020",SPACEPARITY      /* space parity */
      };

struct 
   {
   CHAR *lanspd;
   INT  spdval;
   INT  spd_len;
   } lspeed[] = 
      {
      "\020\00",300,2,     /*  300 baud    */
      "\04\00",1200,2,     /* 1200 baud    */
      "\01\00",2400,2,     /* 2400 baud    */
      "\00\100",4800,2,    /* 4800 baud    */
      "\00\020",9600,2     /* 9600 baud    */
      };

#define NUM_SPEED 5
#define NUM_PRTY  5

void  command_vector (buffer, command_id, commInfo)

CHAR         *buffer;
INT          command_id;
COMMDATA     *commInfo;
   {
   CHAR      temp[30];
   CHAR      *index;
   void      get_speed(CHAR **, INT);
   void      get_parity(CHAR **, INT);

   memset(buffer, 0x0, STAT_BUFF_LEN);
   index = buffer + 2;     /* position index beyond vector length */
   switch (command_id) 
      {
      case CONNECT_REQ :
         memcpy(index, "\357\01", 2);        /* connection req id */
         index += 2;
         memcpy(index, "\03\01\03", 3);      /* connection id     */
         index += 3;

         if (commInfo->targetName[0] != '\0')
            {
            index[0] = (CHAR)((INT)strlen(commInfo->targetName)+4);  /* len dest. id      */
            index++;
            memcpy(index++, "\02", 1);       /* destination id    */
            index[0] = (CHAR)((INT)strlen(commInfo->targetName)+2);   /* target name len   */
            index++;

            memcpy(index++, "\016", 1);      /* target id & name  */
            memcpy(index, commInfo->targetName, strlen(commInfo->targetName));
            index += strlen(commInfo->targetName);
            }
         else 
            {
            strcpy (temp, commInfo->dialPrefix);
            strcat (temp, commInfo->phoneNumber);
            index[0] = (CHAR)((INT)strlen(temp)+4); /* len dest. id      */
            index++;
            memcpy(index++, "\02", 1);       /* destination id    */
            index[0] = (CHAR)((INT)strlen(temp)+2); /* phone # length    */
            index++;
            memcpy(index++, "\017", 1);      /* phone # id        */
            memcpy(index, temp, strlen(temp));    /* phone number */
            index += strlen(temp);
            memcpy(index++, "\023", 1);   /* asynch line parm len */
            memcpy(index++, "\03", 1);       /* line parm id      */
            get_speed (&index, commInfo->speed);
            memcpy(index++, "\013", 1);      /* len of ascII code */
            memcpy(index++, "\012", 1);      /* id of ascII code  */
            get_parity(&index,commInfo->parity);  /* get parity        */
            memcpy(index,"\03\022\200",3);   /* bits per char     */
            index += 3;
            memcpy(index,"\03\023\200",3);   /* # of stop bits    */
            index += 3;
            memcpy(index, "\02\031", 2);     /* flow control      */
            index += 2;  
            }
         memcpy(index,"\20\04\04\013\00\120\12\14\04\24\00\377\04\25\00\377",16); /* packet size    */
         index += 16;
         memcpy(index, "\03\027\200", 3);    /* queueing is on    */
         index += 3;
         break;
      case CONNECT_ID_RESP :
         memcpy(index, "\357\05", 2);        /* id of id_resp     */
         index += 2;
         memcpy(index, "\06\06\061\060\060\060", 6);  /*  accounting id */
         index += 6;  
         break;
      case CONNECT_RESP :
         memcpy(index, "\357\03\03\05\00", 5); /* ret code        */
         index += 5;
         break;
      case CONNECT_PARM :
         memcpy(index, "\357\10\03\05", 4);  /* vector id         */
         index += 4;
         memcpy(index++, "\023", 1);      /* asynch line parm len */
         memcpy(index++, "\03", 1);          /* line parm id      */
         get_speed(&index, commInfo->speed);
         memcpy(index++, "\013", 1);         /* ascII code len    */
         memcpy(index++,"\012", 1);          /* id of ascII code  */
         get_parity(&index, commInfo->parity);   /* get parity        */
         memcpy(index, "\03\022\200", 3);    /* bits per char     */
         index += 3;
         memcpy(index, "\03\023\200", 3);    /* # of stop bits    */
         index += 3;
         memcpy(index++, "\02\031", 2);      /*  flow control     */
         index += 2;
         memcpy(index, "\010\04\04\013\00\01\02\014", 8); /* pack siz */
         index += 8;
         break;
      case CONNECT_ID :
         memcpy(index, "\357\04\03\01", 4);  /* vector id         */
         index += 4;
         memcpy(index++, "\01", 1);          /* connect id value  */
      }

   buffer[1] = (CHAR)(index - buffer);       /* vector length     */
   }



void
get_speed(index,speed)
   CHAR **index;
   INT  speed;
   {
   INT i;

   i = 0;
   while ((lspeed[i].spdval != speed) && (i < NUM_SPEED))
      i++;
   if (i == NUM_SPEED)
      i = 1;                                 /* default 1200 baud */

   *index[0] = (CHAR)(lspeed[i].spd_len+2);
   (*index)++;
   memcpy((*index)++, "\011", 1);               /* speed id          */
   memcpy((*index), lspeed[i].lanspd, lspeed[i].spd_len);
   (*index) += lspeed[i].spd_len;
   }


void
get_parity(index,parity)
   CHAR **index;
   INT  parity;
   {
   INT  i;

   memcpy((*index), "\03\021", 2);              /* parity id         */
   (*index) +=2;
   i = 0;
   while ((lparity[i].prty_value != parity) && (i < NUM_PRTY))
      i++;
   if (i == NUM_PRTY)
      i = 2;                                 /* default even par  */

   memcpy((*index)++, lparity[i].lan_prty, 1);
   }
