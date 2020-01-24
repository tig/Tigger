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
 *  $Log:   N:/src/chget.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 11:01:20   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:22:46   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.0   19 Nov 1990  9:27:40   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.0   24 Oct 1990  9:11:48   andrews
 * No change.
 * 
 *    Rev 4.0   01 Oct 1990 13:29:32   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.0   17 Sep 1990 10:11:30   andrews
 * Full Iteration 1
 * 
 *    Rev 2.0   27 Aug 1990  9:25:06   andrews
 * iteration 1
 * 
 *    Rev 0.0   13 Aug 1990 11:20:06   deanm
 * Initial revision.
 * 
 **************************************************************************/


#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "scrntool.h"
#include "online.h"
#include "except.h"
#include "token.h"
#include "devcomm.h"


INT  chget (ovbuf, ovbuf_ind, ch, data_flag)

CHAR      ovbuf[];
INT       *ovbuf_ind;
CHAR      *ch;
BOOLEAN   *data_flag;

   {

   INT  status;
   static INT  ovbuf_pos = 0;

   *data_flag = TRUE;
   if (*ovbuf_ind > 0)
      {
      *ch = ovbuf[ovbuf_pos];
      ovbuf_pos++;
      if (ovbuf_pos == *ovbuf_ind)
         {
         ovbuf[0] = '\0';
         *ovbuf_ind = 0;
         ovbuf_pos = 0;
         }
      }
   else
      {
      if ((status = Dev_ReadCh (ch)) != SUCCESS)
         {
         return (Except_Raise (status, NULL));
         }
      if (*ch == '\0')
        {
        *data_flag = FALSE;
        }
      else 
        {
        *data_flag = TRUE;
        }
      }

   return (SUCCESS);
   }
