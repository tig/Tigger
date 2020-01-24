/*****************************************************************************
* FILE NAME:  ols.c                                                          *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*    online edit send receive                                                *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*    OLS_EditSendReceive                                                     *
*    OLS_Edit                                                                *
*    OLS_Send                                                                *
*    OLS_Receive                                                             *
*    OLS_Write                                                               *
*    OLS_SendStr                                                             *
*    OLS_Record                                                              *
*    OLS_UpdateBar                                                           *
*    OLS_DocDiskOpen                                                         *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  06/01/90                                                            *
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

/****************************************************************************
 * 
 *  $Log:   N:/src/ols.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 11:00:00   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:21:50   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.1   20 Nov 1990 10:04:38   joeb
 * better control of alt-q in session rec and dexist box
 * 
 *    Rev 6.0   19 Nov 1990  9:26:26   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.2   16 Nov 1990 12:24:26   joeb
 * change declaration of Download from ols.c to help.c
 * 
 *    Rev 5.1   16 Nov 1990 10:45:38   joeb
 * fixed help problem with print dialogue box
 * 
 *    Rev 5.0   24 Oct 1990  9:10:38   andrews
 * No change.
 * 
 *    Rev 4.7   18 Oct 1990  9:57:52   joeb
 * remove second screen save of last from session.rec on time out
 * 
 *    Rev 4.6   17 Oct 1990 16:02:36   deanm
 * changing tobasco from int9 to int15
 * 
 *    Rev 4.5   16 Oct 1990  9:18:12   deanm
 * fixing document download data loss DART 2.059
 * 
 *    Rev 4.4   15 Oct 1990 11:50:32   andrews
 * handle P* special case
 * 
 *    Rev 4.3   05 Oct 1990 10:03:00   deanm
 * updating comments
 * 
 *    Rev 4.2   03 Oct 1990 15:00:58   deanm
 * adding retry and cancel options to the two disk fulls
 * 
 *    Rev 4.1   02 Oct 1990 15:16:20   deanm
 * do not send HTC if no characters to send
 * 
 *    Rev 4.0   01 Oct 1990 13:28:42   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.5   29 Sep 1990 19:30:44   deanm
 * change recording file_print retry flag to FALSE
 * 
 *    Rev 3.4   28 Sep 1990 17:52:36   andrews
 * adding ESC when connecting and screen is disabled
 * 
 *    Rev 3.3   27 Sep 1990 11:17:02   andrews
 * increased size of ols edit field by 1
 * 
 *    Rev 3.2   25 Sep 1990 16:52:22   deanm
 * fixes for DARTS 1.037 (alt-q during sign-on
 * 
 *    Rev 3.1   21 Sep 1990 17:37:34   deanm
 * Initial revision.
 * 
 *    Rev 3.0   17 Sep 1990 10:10:46   andrews
 * Full Iteration 1
 * 
 *    Rev 2.6   12 Sep 1990  8:15:26   deanm
 * added checks to return codes of print_close
 * 
 *    Rev 2.5   11 Sep 1990 20:30:24   deanm
 * adding tobasco
 * 
 *    Rev 2.4   11 Sep 1990 16:56:04   vincentt
 * added sending STOP_KEY if printing cancelled
 * 
 *    Rev 2.3   10 Sep 1990 16:04:20   deanm
 * added 2 parms to dpath
 * 
 *    Rev 2.2   05 Sep 1990 13:56:44   joeb
 * last line of session rec was not being written
 * 
 *    Rev 2.1   05 Sep 1990 11:54:34   joeb
 * fix stuff
 * 
 *    Rev 2.0   27 Aug 1990  9:24:26   andrews
 * iteration 1
 * 
 *    Rev 0.13   25 Aug 1990 18:44:00   deanm
 * removed prints
 * 
 *    Rev 0.11   23 Aug 1990 22:00:42   deanm
 * fixing up document download and status bar
 * 
 *    Rev 0.10   22 Aug 1990  9:02:22   deanm
 * fixing up session recording
 * 
 *    Rev 0.9   21 Aug 1990 11:02:36   deanm
 * fixing up ols print
 * 
 *    Rev 0.8   17 Aug 1990 16:55:02   deanm
 * added return value checking for Print_String
 * 
 *    Rev 0.7   16 Aug 1990 14:29:58   andrews
 * wipe attributes on line when deleting or backspacing
 * 
 *    Rev 0.6   16 Aug 1990 14:23:58   deanm
 * adding printing to online
 * 
 *    Rev 0.5   16 Aug 1990 11:06:28   andrews
 * now we can delete in column 1
 * 
 *    Rev 0.4   16 Aug 1990 10:49:58   andrews
 * changed handling of status from prtoken
 * changed editing functions in OLS_Edit
 * 
 *    Rev 0.3   16 Aug 1990  8:56:28   andrews
 * added edit functions to ols_edit
 * 
 *    Rev 0.2   15 Aug 1990 18:16:54   deanm
 * adding screen recognition functionality to online
 * 
 *    Rev 0.1   14 Aug 1990 17:07:28   deanm
 * fixing up online
 * 
 *    Rev 0.0   13 Aug 1990 10:07:22   deanm
 * Initial revision.
 * 
 *
 ***************************************************************************/



#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "scrntool.h"
#include "texttool.h"
#include "prnttool.h"
#include "edit.h"
#include "online.h"
#include "ols.h"
#include "token.h"
#include "help.h"
#include "devcomm.h"
#include "except.h"
#include "menu.h"
#include "dbox.h"
#include "tobasco.h"


int OLS_insert_mode;

extern TYPE_COLOR color[5];    /* defined in scrntool.c */
extern BOOLEAN  Download_help_flg; // decalred in help.c

void OLS_Edit_BS (EDIT_TYPE *edf);
void OLS_Edit_Delete (EDIT_TYPE *edf);
INT OLS_Edit_SR_Open (TYPE_TS *termStatus, OLDATA *onlineInfo);


/*****************************************************************************
* FUNCTION NAME:  OLS_EditSendRcv                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Edits the current OLS screen, sends the user's response, and             *
*   receives the next screen                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_EditSendRcv (termStatus, onlineInfo);                       *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   OLDATA  *onlineInfo - online information                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   Edit/send/receive will in turn call OLS_Edit, OLS_Send, and              *
*   OLS_Receive to edit an OLS screen, send the user's response,             *
*   and receive another screen.                                              *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*      OLS_Edit                                                              *
*      OLS_Send                                                              *
*      OLS_Receive                                                           *
*                                                                            *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/






INT  OLS_EditSendRcv (termStatus, onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   INT                status;
   TRANS_TABLE_ENTRY  *TTEntry;
   

/* call the edit routines to edit the ols screen */
   status = OLS_Edit (termStatus, onlineInfo, termStatus->scrnBuffPtr, &TTEntry);
   if (status != SUCCESS  &&  status != ESCAPE)
      {
      return (status);
      }

/* now send the user's response back to the OLS */
   if (status != ESCAPE  &&  (status = OLS_Send (termStatus, TTEntry)) != SUCCESS)
      {                   
      return (status);
      }
   status = SUCCESS;

/* receive the next screen */
   if ((status = OLS_Receive (termStatus, onlineInfo)) != SUCCESS)
      {
      return (status);
      }


   return (SUCCESS);
   }







/*****************************************************************************
* FUNCTION NAME:  OLS_Edit                                                   *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Edits the current OLS screen                                             *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_Edit (termStatus, onlineInfo, scrnBuff, TTEntry);           *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   OLDATA  *onlineInfo - online information                                 *
*   CHAR    *scrnBuff   - all of the text from the OLS screen                *
*   TRANS_TABLE_ENTRY **TTEntry - returned ptr to translation table entry    *
*   (The translation table contains keystrokes and their OLS equivalencies.) *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_Edit edits the OLS screen.  It will also take care of a few special  *
*   edit keystrokes, initiating session recording, and translating OLS       *
*   function keys according to the translation tables.                       *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   read screen into scrnBuff                                                *
*   if necessary, record screen (session record)                             *
*   while (editting)                                                         *
*      key = Edit_Activate                                                   *
*      switch (TobascoOn ())                                                 *
*         case TRUE                                                          *
*             do the keys according to tobasco keys (see switch below)       *
*         case FALSE                                                         *
*             do the keys according to the standard keys (see switch below)  *
*                                                                            *
*   look in translation table for key                                        *
*   end routine                                                              *
*                                                                            *
*                                                                            *
*   The following keys are edit keys or special function keys that Turbo     *
*   must handle.                                                             *
*      switch (key)                                                          *
*         case HELP                                                          *
*             help ()                                                        *
*         case QUIT                                                          *
*             quit                                                           *
*         case BS                                                            *
*             do specialized backspace                                       *
*         case DEL                                                           *
*             do specialized delete                                          *
*         case HOME                                                          *
*             set cursor to BOLN                                             *
*         case END                                                           *
*             set cursor to EOLN                                             *
*         case ALT-F2 (session record)                                       *
*             turn recording off or on                                       *
*         case ESCAPE                                                        *
*             we received a char while editting, escape back to OLS_Receive  *
*         default                                                            *
*             done edtting, now go look in translation table                 *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




INT  OLS_Edit (termStatus, onlineInfo, scrnBuff, TTEntry)

TYPE_TS           *termStatus;
OLDATA            *onlineInfo;
CHAR              *scrnBuff;
TRANS_TABLE_ENTRY **TTEntry;

   {

   INT         status;
   CHAR        temp[(ROWS-1)*COLS + 1];
   EDIT_TYPE   edf;
   BOOLEAN     done;
   INT         i;
   INT         maxTableEntries;

                 
/* setup the edit structure */
   edf.initial    = scrnBuff;
   edf.answer     = temp;
   edf.length     = (ROWS-1)*COLS+1;
   edf.size.row   = ROWS-1;
   edf.size.col   = COLS;
   edf.location.row = 0;
   edf.location.col = 0;
   edf.cursorPosition  = 0;
   edf.helpPtr    = Advice;
   edf.helpIndex  = ADVICE_HELP;
   edf.insertMode = FALSE; 
   edf.box        = FALSE;
   edf.win.bwnptr = NULL;
   edf.win.wntype = DISP_NORMAL;
   edf.highlight  = FALSE;
   edf.options    = 0;
   edf.keyFunct   = Edit_KCF_OLS;
   edf.defKeys    = TobascoOn() ? b_defkeys_tobasco : b_defkeys_online;

/* read the screen for the edit session */
   read_screen (scrnBuff);
   scrnBuff [(ROWS-1)*COLS] = '\0';

/* record the just read screen (if necessary) */
   if (termStatus->rec_on)
      {
      status = OLS_Record (termStatus, onlineInfo);
      if (status == QUIT)
         {
         return (QUIT);
         }
         
      status = SUCCESS;
      }


   done = FALSE;

   OLS_insert_mode = 0;

/* call the edit function and take care of special keystrokes */
   while (!done)
      {
      status = Edit_Activate (&edf);
      switch (TobascoOn())
         {

/* TRUE is for tobasco keyboard */
         case TRUE:     
         switch (edf.returnkey)
            {
            case HELP_KEY:          /* Alt-H */
               {
               INT stat;
               stat=Help(edf.helpIndex,NOVICE_HELPFILE,(CHAR _far *) edf.helpPtr);
               if (stat==QUIT)
                  {
                  return (QUIT);
                  }
               }
               break;

            case QUIT_KEY:          /* Alt-Q */
               return (QUIT);
               break;

            case NEW_END_KEY:           /* end of line */
               edf.cursorPosition = ((((edf.cursorPosition / COLS)+1)*COLS)-1);
               break;

            case NEW_HOME_KEY:          /* beginging of line */
               edf.cursorPosition = ((edf.cursorPosition/COLS)*COLS);
               break;

            case BACKSPACE_KEY:     /* backspace, but only to end of word past BOLN */
               (void) OLS_Edit_BS (&edf);
               break;

            case NEW_DELETE_KEY:        /* delete, but only to end of word past EOLN */
               (void) OLS_Edit_Delete (&edf);
               break;

            case (INT)SF11_KEY:          /* session record for tobasco */
               status =  OLS_Edit_SR_Open (termStatus, onlineInfo);
               if (status==QUIT)
                  {
                  return (QUIT);
                  }
               break;

            case ESCAPE_KEY:        /* escape and receive characters coming in */

               termStatus->kbd_locked = TRUE;
               return (ESCAPE);
               break;

            default:                /* translation table */
               scrnBuff[edf.cursorPosition]='\0';
               Sys_cvtstring (scrnBuff);
               done = TRUE;

            }
         break;

/* FALSE is for the standard keyboard */
         case FALSE: /* case for switch on (TobascoOn()) */
         switch (edf.returnkey)
            {
            case HELP_KEY:          /* Alt-H */
               {
               INT stat;
               stat=Help(edf.helpIndex,NOVICE_HELPFILE,(CHAR _far *) edf.helpPtr);
               if (stat==QUIT)
                  {
                  return (QUIT);
                  }
               }
               break;

            case QUIT_KEY:          /* Alt-Q */
               return (QUIT);
               break;

            case END_KEY:           /* end of line */
               edf.cursorPosition = ((((edf.cursorPosition / COLS)+1)*COLS)-1);
               break;

            case HOME_KEY:          /* beginging of line */
               edf.cursorPosition = ((edf.cursorPosition/COLS)*COLS);
               break;

            case BACKSPACE_KEY:     /* backspace, but only to end of word past BOLN */
               (void) OLS_Edit_BS (&edf);
               break;

            case DELETE_KEY:        /* delete, but only to end of word past EOLN */
               (void) OLS_Edit_Delete (&edf);
               break;

            case AF2_KEY:           /* session record (turn it on, or off) */
               status =  OLS_Edit_SR_Open (termStatus, onlineInfo);
               if (status==QUIT)
                  {
                  return (QUIT);
                  }
               break;

            case ESCAPE_KEY:        /* escape and receive characters coming in */
               termStatus->kbd_locked = TRUE;
               return (ESCAPE);
               break;

            default:                /* translation table */
               scrnBuff[edf.cursorPosition]='\0';
               Sys_cvtstring (scrnBuff);
               done = TRUE;

            }
         break;



         }  /* end of switch (TobascoOn ()) */
      }  /* end of while (!done) */


/* if we get this far in OLS_Edit, the keystroke should be an OLS funct. key */
/* look in translation tables to find the OLS function key */
   done = FALSE;
   *TTEntry = (TobascoOn()) ? &TobascoTable[0] : &TTable[0];
   maxTableEntries = (TobascoOn()) ? TOBASCO_TT_SIZE : TT_SIZE;
   i = 0;
   while (!done && i<maxTableEntries)
      {
      if ((*TTEntry)->key==edf.returnkey)
         {
         done=TRUE;
         }
      else
         {
         *TTEntry = (TobascoOn()) ? &TobascoTable[++i] : &TTable[++i];
         }
      }

   if (!done)
      {
      return (FAILURE);
      }

   return (SUCCESS);
   }



/* this is an internal routine for OLS_Edit and it handles the backspace key*/

void OLS_Edit_BS (EDIT_TYPE *edf)
   {
   INT a,b;
   int prow, pcol, phigh, plow;
   a=edf->cursorPosition/COLS;
   if ((a*COLS)!=edf->cursorPosition) /* are we in first col ? */
      {
      b=(a+1)*COLS;
   
      while (edf->initial[b-1] != ' ' && b<edf->length)  b++;  /* copy up to the first space from the eol */

      memmove(edf->initial+edf->cursorPosition-1,
              edf->initial+edf->cursorPosition,
              b-edf->cursorPosition);
      memset(edf->initial+b-1,' ',1);
      edf->cursorPosition--;
      sccurst(&prow, &pcol, &phigh, &plow);
      viatrect(prow,pcol,prow,COLS-1,color[DISP_NORMAL].fore,color[DISP_NORMAL].back);
      }
   }



/* this is an internal routine for OLS_Edit and it handles the delete key */

void OLS_Edit_Delete (EDIT_TYPE *edf)
   {
   INT a,b;
   int prow, pcol, phigh, plow;
   a=edf->cursorPosition/COLS;
   b=(a+1)*COLS;

   while (edf->initial[b-1] != ' ' && b<edf->length)  b++;  /* copy up to the first space from the eol */

   memmove(edf->initial+edf->cursorPosition,
           edf->initial+edf->cursorPosition+1,
           b-edf->cursorPosition-1);
   memset(edf->initial+b-1,' ',1);
   sccurst(&prow, &pcol, &phigh, &plow);
   viatrect(prow,pcol,prow,COLS-1,color[DISP_NORMAL].fore,color[DISP_NORMAL].back);
   }



/* this is an internal routine for OLS_Edit and it handles session record */
/* open        */

INT OLS_Edit_SR_Open (TYPE_TS *termStatus, OLDATA *onlineInfo)
   {
   INT status = SUCCESS;
   if (termStatus->rec_on)
      {
      fclose (termStatus->rec_file);
      termStatus->rec_file = NULL;
      termStatus->rec_on = FALSE;
      OLS_UpdateBar (termStatus, NULL, FALSE);
      }
   else
      {
      CHAR mode[5];
      status = dpath (onlineInfo->sessRec, TRUE, mode, NOVICE_HELPFILE, DOCTODSK_FILE_HELP);
      if (status == QUIT)
         {
         return (QUIT);
         }
      else if (status == SUCCESS)
         {
         if ((termStatus->rec_file = fopen (onlineInfo->sessRec, mode)) == NULL)
            {
            return (Except_Raise (OPEN_ERROR, onlineInfo->sessRec));
            }
         termStatus->rec_on = TRUE;
         status = OLS_Record (termStatus, onlineInfo);
         OLS_UpdateBar (termStatus, NULL, FALSE);
         if (status == QUIT)
            {
            return (QUIT);
            }
         status = SUCCESS;
         }
      }
   return (status);
   }
 


/*****************************************************************************
* FUNCTION NAME:  OLS_Send                                                   *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Sends user response to OLS                                               *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_Send (termStatus, TTEntry);                                 *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   TRANS_TABLE_ENTRY **TTEntry - returned ptr to translation table entry    *
*   (The translation table contains keystrokes and their OLS equivalencies.) *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_Send sends the user's response to the OLS.  This response could be   *
*   from the translation table or a typed response.                          *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   Update status bar with either typed response or TTEntry info.            *
*   send the STX char                                                        *
*   if info to send from tranlation table, send it                           *
*   if info to send from HTC (Home To Cursor), send it                       *
*   send CR                                                                  *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT  OLS_Send (termStatus, TTEntry)

TYPE_TS           *termStatus;
TRANS_TABLE_ENTRY *TTEntry;

   {

   INT  status;


/* update the status bar with appropriate info */
   if (TTEntry->display[0] == '\0')
      {
      OLS_UpdateBar (termStatus, termStatus->scrnBuffPtr, TRUE);
      }
   else
      {
      OLS_UpdateBar (termStatus, TTEntry->display, TRUE);
      }


/* send the STX character */
   if ((status = Dev_WriteCh (STX)) != SUCCESS)
      {
      return (Except_Raise (status, NULL));
      }


/* send the information from the translation table (if needed) */
   if (TTEntry->send[0] != '\0')
      {
      if (!strcmp(TTEntry->send, "P*") && (termStatus->scrnBuffPtr[0]=='*'))
                                             // special case
                                             // only send P if home to cursor
                                             //      starts with a *
         {
         if ((status = Dev_WriteStr ("P")) != SUCCESS)
            {
            return (Except_Raise (status, NULL));
            }
         }
      else
         {
         if ((status = Dev_WriteStr (TTEntry->send)) != SUCCESS)
            {
            return (Except_Raise (status, NULL));
            }
         }
      }


/* if the translation table entry requires Home To Cursor, send that too */
   if (TTEntry->HTC)
      {
      if (termStatus->scrnBuffPtr != NULL && termStatus->scrnBuffPtr[0] != '\0')
         {
         if ((status = Dev_WriteStr (termStatus->scrnBuffPtr)) != SUCCESS)
            {
            return (Except_Raise (status, NULL));
            }
         }
      }
      

/* now send the CR */
   if ((status = Dev_WriteCh (CR)) != SUCCESS)
      {
      return (Except_Raise (status, NULL));
      }


   termStatus->kbd_locked = TRUE;

   return (SUCCESS);
   }






/*****************************************************************************
* FUNCTION NAME:  OLS_Receive                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Receives an OLS screen                                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_Receive (termStatus, onlineInfo);                           *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   OLDATA  *onlineInfo - online information                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_Receive receives the next screen from the OLS.  All info received    *
*   is run through PRTOKEN for processing.                                   *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*    do                                                                      *
*       key = Sys_accInp                                                     *
*       if key is stop key, send stop sequence                               *
*       if key is escape and screen is hidden, stop ols and go to prev. menu *
*       if key is quit, then quit                                            *
*       get a token                                                          *
*       process the token                                                    *
*    while (not quitting, not escaping, still online, and keyboard locked)   *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT  OLS_Receive (termStatus, onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   INT  key;
   INT  status;
   INT  token;
   CHAR token_str [COLS + 1];



   do
      {
      key = Sys_accInp (FALSE);


      if (key == STOP_KEY   ||   key == PAUSE_KEY)
         {
         OLS_SendStr (termStatus, STOP_SEQ, "STOP");
         termStatus->first_block = TRUE;
         termStatus->stop_sent = TRUE;
         termStatus->kbd_locked = TRUE;

         if (termStatus->prt_on == TRUE) 
            {
            Print_Close ();
            termStatus->prt_on = FALSE; 
            } 
         if (termStatus->doc_disk_on == TRUE)  
            {
            fclose (termStatus->doc_file);
            termStatus->doc_disk_on = FALSE;
            termStatus->doc_file = NULL;
            }
         }

      if (key == ESCAPE_KEY  &&  Screen_isDisabled())
         {
         scapage(1);
         scpage (1);         //set page to the one being displayed
         set_attributes (BOLD_OFF, DISP_INFO);
         write_screen (ROWS-1, 0, 
" Disconnecting...                                                          ");
         scpage (0);
         return (ESCAPE);
         }
      
      if (key == QUIT_KEY)
         {
         return (QUIT);
         }
      else
         {
         if ((status = getToken (&token, token_str)) != SUCCESS)
            {
            return (status);
            }
         status = prToken (token, token_str, termStatus, onlineInfo);
         if (status == ESCAPE)
            {
            status = SUCCESS;
            }
         if (status != SUCCESS)
            {
            return (status);
            }
         }
      }  while (status == SUCCESS  &&  termStatus->kbd_locked  &&
                termStatus->status == SUCCESS);

   OLS_UpdateBar (termStatus, " Press Alt-H for Advice and Help.", FALSE);

   return (SUCCESS);
   }





/*****************************************************************************
* FUNCTION NAME:  OLS_Write                                                  *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Writes to printer, disk, or screen                                       *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_Write (termStatus, token_str, onlineInfo);                  *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   CHAR    *token_str  - a tokenized character string                       *
*   OLDATA  *onlineInfo - online information                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_Write will write to the screen, to the printer, or to a doc-to-      *
*   disk file.  termStatus is checked to identify which print methods        *
*   will be used.                                                            *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   if writing to screen                                                     *
*      write to screen                                                       *
*   else if writing to disk or printer                                       *
*      if writing to disk                                                    *
*         write to disk and do appropriate error checking                    *
*      if writing to printer                                                 *
*         write to printer and do appropriate error checking                 *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT  OLS_Write (termStatus, token_str, onlineInfo)

TYPE_TS  *termStatus;
CHAR     *token_str;
OLDATA   *onlineInfo;

   {

   INT   status;
   CHAR  *temp;
   INT   numToWrite;
   INT   total;


/* if disp_data is true, writing to sceen */
   if (termStatus->disp_data)
      {

/* now write the stuff to the screen, changing rows when need */
      temp = token_str;
      total = strlen (temp);
      while (total > 0)
         {
         numToWrite = (termStatus->cur_col + total > COLS) ? 
                      COLS - termStatus->cur_col : total;
         write_screen_n (termStatus->cur_row, termStatus->cur_col, temp,
                         numToWrite);
         total -= numToWrite;
         temp  += numToWrite;
         termStatus->cur_col += numToWrite;
         if (termStatus->cur_col >= COLS)
            {
            termStatus->cur_col = 0;
            termStatus->cur_row = ++termStatus->cur_row % (ROWS - 1);
            }
         }
      }

/* else if if doc to disk or printing are on, then do them */
   else if (termStatus->doc_disk_on  ||  termStatus->prt_on)
      {
      if (termStatus->doc_disk_on)  /* doc to disk */
         {
         do
            {
            status = file_print (termStatus->doc_file, token_str, 
                                 onlineInfo->docToDsk, TRUE);
            if (status == RETRY)
               {
               status = OLS_DocDiskOpen (termStatus, onlineInfo);
               }
            } while (status == RETRY);

         if (status == ESCAPE)
            {
            if (TobascoOn ())
               {
               Sys_kbstuff (PAUSE_KEY);
               }
            else
               {
               Sys_kbstuff (STOP_KEY);
               }
            Dev_Flush ();
            }


         if (status != SUCCESS)
            {
            return (status);
            }
         }

      if (termStatus->prt_on)       /* printing */
         {
         do {
            status = Print_String (token_str);   /* added */
            if (status != SUCCESS && status != RETRY) 
               {
               if (status == ESCAPE)
                  {
                  if (TobascoOn ())
                     {
                     Sys_kbstuff (PAUSE_KEY);
                     }
                  else
                     {
                     Sys_kbstuff (STOP_KEY);
                     }
                  Dev_Flush ();
                  }
               return (status);
               }
            } while (status == RETRY);
         }
      }


   return (SUCCESS);
   }





/*****************************************************************************
* FUNCTION NAME:  OLS_SendStr                                                *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Sends a string to the ols                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_SendStr (termStatus, string, update);                       *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   CHAR    *string     - string to send to OLS                              *
*   CHAR    *update     - string to update OLS_UpdateBar                     *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_SendStr sends a string to ols.  It does not use the translation      *
*   table like OLS_Send does.                                                *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   update status bar with update char string                                *
*   send STX                                                                 *
*   send string                                                              *
*   send CR                                                                  *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS                                                                  *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT   OLS_SendStr  (termStatus, string, update)

TYPE_TS  *termStatus;
CHAR     *string;
CHAR     *update;

   {

   if (update == NULL)
      {
      OLS_UpdateBar (termStatus, "", FALSE); /* just clear the line */
      }
   else
      {
      OLS_UpdateBar (termStatus, update, TRUE); /* else print it out */
      }

   Dev_WriteCh  (STX);
   Dev_WriteStr (string);
   Dev_WriteCh  (CR);


   return (SUCCESS);
   }






/*****************************************************************************
* FUNCTION NAME:  OLS_Record                                                 *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Record the current screen                                                *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_Record (termStatus, onlineInfo);                            *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   OLDATA  *onlineInfo - online information                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_Record will write out to the session recording file the current      *
*   displayed OLS screen.                                                    *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   write out screen, row by row                                             *
*   write out a FF character                                                 *
*   update status bar                                                        *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT   OLS_Record (termStatus, onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   INT  status = SUCCESS;
   INT  i;
   CHAR temp[COLS + 2];



/* write out the current screen row by row */
   for (i=0; i<ROWS-1  &&  status == SUCCESS; i++)
      {
      strncpy (temp, termStatus->scrnBuffPtr + (COLS * i), COLS);
      temp [COLS] = '\n';
      temp [COLS+1] = '\0';
      status = file_print (termStatus->rec_file, temp,
                              onlineInfo->sessRec, FALSE);
      }


/* if we are still ok, write out a FF character */
   if (status == SUCCESS)
      {
      temp [0] = '\n';
      temp [1] = '\f';
      temp [2] = '\n';
      temp [3] = '\0';
      status = file_print (termStatus->rec_file, temp,
                              onlineInfo->sessRec, FALSE);
      }

/* update the status bar if needed */
   if (status == ESCAPE)
      {
      termStatus->rec_on = FALSE;
      termStatus->rec_file = NULL;
      OLS_UpdateBar (termStatus, NULL, FALSE);
      }


   return (status);
   }








/*****************************************************************************
* FUNCTION NAME:  OLS_UpdateBar                                              *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Updates the status bar                                                   *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_UpdateBar (termStatus, string, header);                     *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   CHAR    *string     - string to place in status bar                      *
*   BOOLEAN header      - TRUE if status bar header required                 *
*   (The status bar header is " Working on " message)                        *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_UpdateBar will update the status bar with string.  If the status     *
*   bar header is required (working on ... message), then header should be   *
*   set to TRUE.                                                             *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   parse string and header into the actual status bar message               *
*   display the status bar message                                           *
*   display session recording info                                           *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS                                                                  *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT  OLS_UpdateBar (termStatus, string, header)

TYPE_TS  *termStatus;
CHAR     *string;
BOOLEAN  header;

   {

   CHAR  line [COLS + 1];


/* if string to parse, clear the status bar and continue */
   if (string != NULL)
      {
      write_window (&termStatus->stat_win, 
"                                                                           ",
                    0, 0);
      if (header)      /* include "Working on ..." header */
         {
         if (strlen (string) > 50)
            {
            sprintf (line, " Working on \"%.50s...\".", string);
            }
         else if (strlen (string) == 0)
            {
            strcpy (line, " Working on \"Transmit Key\".");
            }
         else
            {
            sprintf (line, " Working on \"%s\".", string);
            }
         }
      else           /* do not include "Working on ..." header */
         {
         strcpy (line, string);
         }

/* display the parsed status bar message */
      write_window (&termStatus->stat_win, line, 0, 0);
      }


/* take care of session recording indicator */
   if (termStatus->rec_on)
      {
      write_window (&termStatus->stat_win, "RC", 0, 76);
      }
   else
      {
      write_window (&termStatus->stat_win, "  ", 0, 76);
      }

   cursor_off();

   return (SUCCESS);
   }







/*****************************************************************************
* FUNCTION NAME:  OLS_DocDiskOpen                                            *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Open the document to disk file                                           *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = OLS_DocDiskOpen (termStatus, onlineInfo);                       *
*                                                                            *
*   INT  status - returned completion code                                   *
*                                                                            *
*   TYPE_TS *termStatus - terminal status                                    *
*   OLDATA  *onlineInfo - online information                                 *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*   OLS_DocDiskOpen will open the document download file. It also displays   *
*   the dialogue boxes needed to get the doc to disk information.  Notice    *
*   that this file could be a printer as well as an actual disk file.        *
*                                                                            *
*                                                                            *
* PSUEDO-CODE                                                                *
*                                                                            *
*   update status bar                                                        *
*   display the document download dialogue boxes                             *
*   if opening a disk file, then open it                                     *
*   if downloading to printer, open the printer                              *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, ESCAPE, QUIT                                                    *
*                                                                            *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/06/90                                                            *
*                                                                            *
*****************************************************************************/




  
INT  OLS_DocDiskOpen (termStatus, onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   CHAR openMode [5];
   INT  status = SUCCESS;


/* update the status bar */
   OLS_UpdateBar (termStatus, " Enter document download information.  Press Alt-H for Help.", FALSE);
   termStatus->first_block = FALSE;


/* display the dialogue boxes */
   Download_help_flg = TRUE;
   status = dstream (&termStatus->doc_disk_on, &termStatus->prt_on,
                     onlineInfo->docToDsk, openMode);
   Download_help_flg = FALSE;


/* open doc to disk, if turned on */
   if (status == SUCCESS)
      {
      if (termStatus->doc_disk_on)
         {
         if ((termStatus->doc_file = fopen (onlineInfo->docToDsk, openMode)) == NULL)
            {
            status = Except_Raise (OPEN_ERROR, onlineInfo->docToDsk);
            termStatus->doc_disk_on = FALSE;
            termStatus->prt_on = FALSE;
            termStatus->first_block = TRUE;
            }
         }


/* open printer if turned on */
      if (termStatus->prt_on)
         {
         if ((status = Print_Open ()) != SUCCESS)
            {
            termStatus->prt_on = FALSE;
            }
         }
      }

   return (status);
   }

