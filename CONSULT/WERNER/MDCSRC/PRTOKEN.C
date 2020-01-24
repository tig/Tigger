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
 *  $Log:   N:/src/prtoken.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 11:02:18   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:23:52   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:28:30   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:12:54   andrews
 * No change.
 * 
 *    Rev 4.1   03 Oct 1990 15:01:14   deanm
 * adding retry and cancel options to the two disk fulls
 * 
 *    Rev 4.0   01 Oct 1990 13:30:32   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.1   21 Sep 1990 17:35:32   deanm
 * fixing stop, kwic, sign off, focus keys
 * 
 *    Rev 3.0   17 Sep 1990 10:12:34   andrews
 * Full Iteration 1
 * 
 *    Rev 2.4   12 Sep 1990  8:16:10   deanm
 * moved set_attributes from prnttool to LOCK in prtoken
 * 
 *    Rev 2.3   11 Sep 1990 11:20:36   vincentt
 * added status check for Print_Close
 * 
 *    Rev 2.2   05 Sep 1990  8:35:28   joeb
 * Initial revision.
 * 
 *    Rev 2.1   04 Sep 1990 14:36:38   joeb
 * fix dart 1.009: reset print flags after end of print and close files
 * 
 *    Rev 2.0   27 Aug 1990  9:26:10   andrews
 * iteration 1
 * 
 *    Rev 0.4   23 Aug 1990 22:00:50   deanm
 * fixing up document download and status bar
 * 
 *    Rev 0.3   22 Aug 1990 11:21:16   deanm
 * Initial revision.
 * 
 *    Rev 0.2   21 Aug 1990 10:41:34   deanm
 * ols printing functionality is being worked in
 * 
 *    Rev 0.1   14 Aug 1990 17:07:48   deanm
 * fixing up online
 * 
 *    Rev 0.0   13 Aug 1990 10:07:48   deanm
 * Initial revision.
 * 
 **************************************************************************/


#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "scrntool.h"
#include "online.h"
#include "token.h"
#include "ols.h"
#include "prnttool.h"
#include "except.h"
#include "edit.h"
#include "menu.h"
#include "dbox.h"
#include "devcomm.h"
#include "tobasco.h"


INT  prToken (token, token_str, termStatus, onlineInfo)

INT         token;
CHAR        *token_str;
TYPE_TS     *termStatus;
OLDATA      *onlineInfo;

    {

   CHAR print_str[40];
   static INT return_flag ;
   INT    status;

#ifdef DEBUG
   LONG hours,mins,secs,hund;
   static unsigned LONG lock_time = 0L;
   static unsigned LONG unlock_time = 0L;
   CHAR temp[10];
#endif


   if (token == UNAVAILABLE)
      {
      return (SUCCESS);
      }
 

   switch(token)
      {
      case CLEAR :
         termStatus->cur_row = 0;
         termStatus->cur_col = 0;
         setcursor(termStatus->cur_row,termStatus->cur_col);
         termStatus->scrnBuffPtr[0] = '\0';
         clear_screen();
         break;
      case HOME  :
         termStatus->cur_row = 0;
         termStatus->cur_col = 0; 
         setcursor(termStatus->cur_row,termStatus->cur_col);   
         break;
      case REV_ON :
         set_attributes(REVERSE_ON, DISP_NORMAL);
         break;
      case REV_OFF :
         set_attributes(REVERSE_OFF, DISP_NORMAL);
         break;
      case UND_ON :
         set_attributes(UNDERLINE_ON, DISP_NORMAL);
         break;
      case UND_OFF :
         set_attributes(UNDERLINE_OFF, DISP_NORMAL);
         break;
      case LOCK :
#ifdef DEBUG
         hours = mins = secs = hund = 0;
         ststime (temp,&(int)hours,&(int)mins,&(int)secs,&(int)hund);
         lock_time = (LONG)((((mins * 60) +secs) * 100) + hund);
         setcursor (0, 0);
#endif
         termStatus->kbd_locked = TRUE; 
         termStatus->stop_sent = FALSE;
         termStatus->disp_data = TRUE;
         set_attributes(BOLD_OFF, DISP_NORMAL); /* reset to normal colors */
         cursor_off();
         break;


      case PRINT :
         termStatus->disp_data = FALSE;
         if (termStatus->first_block == TRUE)
            {
            status = OLS_DocDiskOpen (termStatus, onlineInfo);
            if (status == ESCAPE)
               {
               if (TobascoOn())
                  {
                  Sys_kbstuff (PAUSE_KEY);
                  }
               else
                  {
                  Sys_kbstuff (STOP_KEY);
                  }
               Dev_Flush ();
               return (SUCCESS);
               }

            if (status != SUCCESS)
               {
               return (status);
               }
            }

/* update status bar according to download flags */
         if (termStatus->prt_on == TRUE && termStatus->doc_disk_on == TRUE)
            {
            OLS_UpdateBar (termStatus, 
                          " Now printing to the Disk and Printer ", FALSE);
            }
         else if (termStatus->doc_disk_on == TRUE)
            {
            OLS_UpdateBar (termStatus, " Now printing to the Disk ", FALSE);
            }
         else if (termStatus->prt_on)
            {
            OLS_UpdateBar (termStatus, " Now printing to the Printer ", FALSE);
            }
         else
            {
            OLS_UpdateBar (termStatus, " Working on Stop Key ", FALSE);
            }
         break;


      case UNLOCK :
         if (termStatus->disp_data == FALSE)
            {
            termStatus->disp_data = TRUE;
            if (termStatus->stop_sent == FALSE)  /* send ACK/NAK */
               {
               OLS_SendStr(termStatus, ACK_SEQ, NULL);
               }
            else
               {
               termStatus->kbd_locked = FALSE;
               }
            }
         else
            {
            /* take care of house keeping */
            if (termStatus->prt_on)
               {
               if ((status = Print_Close ()) != SUCCESS)
                  {
                  if (TobascoOn())
                     {
                     Sys_kbstuff (PAUSE_KEY);
                     }
                  else
                     {
                     Sys_kbstuff (STOP_KEY);
                     }
                  return (status);
                  }
               termStatus->prt_on = FALSE;
               }
            if (termStatus->doc_disk_on)
               {
               fclose (termStatus->doc_file);
               termStatus->doc_disk_on = FALSE;
               }
            termStatus->first_block = TRUE;

            if (onlineInfo->beep == TRUE)
               beep();
            write_window (&termStatus->stat_win, 
                          " Press ALT-H for the Advice Box.", 0, 0);
            setcursor(termStatus->cur_row,termStatus->cur_col);
            cursor_on();
            termStatus->kbd_locked = FALSE; 
            wset_attributes (NULL,REVERSE_OFF);
            }

         termStatus->stop_sent = FALSE;
#ifdef DEBUG
         hours = mins = secs = hund = 0;
         ststime (temp,&(int)hours,&(int)mins,&(int)secs,&(int)hund);
         unlock_time = (LONG)((((mins * 60) +secs) * 100) + hund);
         printf ("\n%lu - %lu = %lu", unlock_time, lock_time, unlock_time - lock_time);
         unlock_time = 0L;
         lock_time = 0L;
#endif
         break;
      case DISCONNECT :
         termStatus->online = FALSE;
         termStatus->status = ESCAPE;
         Sys_setOnline(FALSE);
         break; 
      case STRING :
         if (termStatus->stop_sent == FALSE)
            {
            status = OLS_Write (termStatus, token_str, onlineInfo);
            if (status != SUCCESS)
               {
               return (status);
               }
            }
         break;
      case RET :
         if (termStatus->stop_sent == FALSE)
            {
            if (termStatus->disp_data == FALSE)
               {
               termStatus->prnt_col = FIRSTCOL;  
               }
            else
               {
               termStatus->cur_col = FIRSTCOL;
               } 
            }
         break;
      case LINEFEED :
         if (termStatus->stop_sent == FALSE)
            if (termStatus->disp_data == FALSE)
               {
               if ((return_flag == FALSE) || (UserDefs.lfeed == FALSE))              
                  {            
                  print_str[0] = LF;
                  print_str[1] = '\0';
                  sprintf(print_str,"%c",LF);
                  status = OLS_Write (termStatus, print_str, onlineInfo);
                  if (status != SUCCESS)
                     {
                     return (status);
                     }
                  termStatus->prnt_col--;  
                  }
               termStatus->prnt_row++;  
               }
            else
               {
               termStatus->cur_row++;
               if (termStatus->cur_row == LASTROW + 1)
                  termStatus->cur_row = FIRSTROW;
               termStatus->cur_col = FIRSTCOL;
               }
         break;
      case FORMFEED :
         if (termStatus->stop_sent == FALSE)
            if (termStatus->disp_data == FALSE)
               {
               sprintf(print_str,"%c",FF);
               status = OLS_Write (termStatus, print_str, onlineInfo);
               if (status != SUCCESS)
                  {
                  return (status);
                  }
               termStatus->prnt_col--;  
               }
         break;

/*    put this back in later.
 *
 *       case SECT_SYM_TOK :
 *          if (termStatus->stop_sent == FALSE)
 *             {
 *             sprintf(print_str,"%c",SECT_SYMBOL);
 *             if (termStatus->disp_data == FALSE)
 *                {
 *                prntstr(termStatus,user_defs,print_str);
 *                }
 *             else
 *                {
 *                dispstr(termStatus,print_str); 
 *                }
 *             }
 *          break;
 *       case ECHO_ON :
 *             termStatus->echo = TRUE;
 *             break;       
 *       case ECHO_OFF :
 *             termStatus->echo = FALSE;
 *             break;       
*/


      case UNKNOWN :
         termStatus->error_count++;
         break;
      default :
         break;          
      }

   if (token == RET)
      return_flag = TRUE;
   else
      return_flag = FALSE; 

   return (SUCCESS);
   }
