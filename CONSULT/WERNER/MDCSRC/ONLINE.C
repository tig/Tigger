/*****************************************************************************
* FILE NAME:  online.c                                                       *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*    online controls                                                         *
*                                                                            *
*                                                                            *
* DESCRIPTION:                                                               *
*                                                                            *
*    Online       - Controls OLS activity.                                   *
*    GetToClientScreen - Attempts to advance OLS session to Client Screen    *
*                                                                            *
*    (The following are private functions.)                                  *
*                                                                            *
*                                                                            *
*    isClient        - recognizes client screen                              *
*    isDeferredPrint - recognizes deferred printing screen                   *
*    isSavedSearches - recognizes saved searches screen                      *
*                                                                            *
*                                                                            *
* DATA STRUCTURES                                                            *
*                                                                            *
*   CONNECTDATA   - General info about connection to OLS                     *
*   OLDATA        - General info about online session from setup.dat         *
*   TRANS_TABLE_ENTRY - Translation Table for function keys                  *
*   TYPE_TS       - Terminal Status                                          *
*                                                                            *
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
 *  $Log:   N:/src/online.c_v  $
 * 
 *    Rev 8.0   18 Dec 1990 11:00:42   joeb
 * TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
 * 
 *    Rev 7.0   06 Dec 1990 15:22:10   joeb
 * revision 7.0 for iteration 5
 * 
 *    Rev 6.2   05 Dec 1990  9:46:46   vincentt
 * removed automatic advice box coming up and modified sign off key to .so;n
 * 
 *    Rev 6.1   28 Nov 1990 11:00:06   joeb
 * FIX ALT Q WHILE PRINTING
 * 
 *    Rev 6.0   19 Nov 1990  9:27:02   joeb
 * revision 6 for interation 4
 * 
 *    Rev 5.2   02 Nov 1990 14:17:58   joeb
 * make online sign on script stop at the lib screen when just client id is sent.
 * 
 *    Rev 5.1   02 Nov 1990  9:43:00   vincentt
 * deleted blank space
 * 
 *    Rev 5.0   24 Oct 1990  9:11:10   andrews
 * No change.
 * 
 *    Rev 4.2   17 Oct 1990 16:01:50   deanm
 * changing tobasco from int9 to int15
 * 
 *    Rev 4.1   02 Oct 1990  9:40:18   deanm
 * fixing up irs comments
 * 
 *    Rev 4.0   01 Oct 1990 13:28:54   andrews
 * ITERATION NUMBER 2
 * 
 *    Rev 3.5   29 Sep 1990 17:06:04   joeb
 * OLS change insulation
 * 
 *    Rev 3.4   27 Sep 1990 15:22:22   joeb
 * correct beep while going online and signoff from lexis id screen
 * 
 *    Rev 3.3   25 Sep 1990 17:13:36   vincentt
 * made status blink
 * 
 *    Rev 3.2   25 Sep 1990 16:52:30   deanm
 * fixes for DARTS 1.037 (alt-q during sign-on
 * 
 *    Rev 3.1   21 Sep 1990 17:35:20   deanm
 * fixing stop, kwic, sign off, focus keys
 * 
 *    Rev 3.0   17 Sep 1990 10:10:56   andrews
 * Full Iteration 1
 * 
 *    Rev 2.5   12 Sep 1990  8:15:20   deanm
 * added checks to return codes of print_close
 * 
 *    Rev 2.4   11 Sep 1990 20:23:38   deanm
 * added tobasco
 * 
 *    Rev 2.3   05 Sep 1990 17:30:08   joeb
 * clear the power-off message that is sent from ols
 * 
 *    Rev 2.2   05 Sep 1990 11:54:18   joeb
 * fix stuff
 * 
 *    Rev 2.1   04 Sep 1990 10:52:54   deanm
 * put define in for stop key in translation table
 * 
 *    Rev 2.0   27 Aug 1990  9:24:36   andrews
 * iteration 1
 * 
 *    Rev 0.6   23 Aug 1990 22:00:46   deanm
 * fixing up document download and status bar
 * 
 *    Rev 0.5   22 Aug 1990 11:58:06   joeb
 * ADD SCREEN HIDING
 * 
 *    Rev 0.4   22 Aug 1990  9:04:36   deanm
 * changed saved searches recognition character string
 * 
 *    Rev 0.3   21 Aug 1990  8:29:26   deanm
 * adding print capabilities to online
 * 
 *    Rev 0.2   15 Aug 1990 18:16:50   deanm
 * adding screen recognition functionality to online
 * 
 *    Rev 0.1   14 Aug 1990 17:07:34   deanm
 * fixing up online
 * 
 *    Rev 0.0   13 Aug 1990 10:07:28   deanm
 * Initial revision.
 * 
 *
 ***************************************************************************/


#include <string.h>
#include <stdio.h>


#include "globals.h"
#include "scrntool.h"
#include "systool.h"
#include "online.h"
#include "ols.h"
#include "mmtool.h"
#include "prnttool.h"
#include "tobasco.h"


#define NO_ANSWER       "N"

TRANS_TABLE_ENTRY TTable[TT_SIZE] = {
 { F1_KEY,        ".np",   "Next Page",          TRUE},
 { PAGEDOWN_KEY,  ".np",   "Next Page",          TRUE},
 { F2_KEY,        ".pp",   "Previous Page",      TRUE},
 { PAGEUP_KEY,    ".pp",   "Previous Page",      TRUE},
 { F3_KEY,        ".nd",   "Next Document",      TRUE},
 { F4_KEY,        ".pd",   "Previous Document",  TRUE},
 { F5_KEY,        ".kw",   "KWIC",               FALSE},
 { F6_KEY,        ".fu",   "Full",               FALSE},
 { F7_KEY,        ".ci",   "Cites",              FALSE},
 { F8_KEY,        ".ns",   "New Search",         FALSE},
 { F9_KEY,        ".cf",   "Change File",        FALSE},
 { F10_KEY,       ".cl",   "Change Library",     FALSE},
 { SF1_KEY,       ".ss",   "Select Service",     FALSE},
 { SF2_KEY,       ".es",   "Exit Service",       FALSE},
 { SF3_KEY,       ".pr",   "Print Document",     FALSE},
 { SF4_KEY,       ".mi",   "Mail It",            FALSE},
 { SF5_KEY,       ".sp",   "Screen Print",       FALSE},
 { SF6_KEY,       ".fp",   "First Page",         FALSE},
 { SF7_KEY,       ".fd",   "First Document",     FALSE},
 { SF8_KEY,       ".dl",   "Different Level",    TRUE},
 { SF9_KEY,       ".vk",   "Variable KWIC",      TRUE},
 { SF10_KEY,      ".se",   "Segments",           FALSE},
 { AF9_KEY,       ".so;n", "Sign Off",           FALSE},
 { AF10_KEY,      STOP_SEQ,"STOP",               FALSE},
 { ENTER_KEY,     "",      "",                   TRUE}
};


TRANS_TABLE_ENTRY TobascoTable[TOBASCO_TT_SIZE] = {
 { F1_KEY,        "P*",             "P*",                  TRUE},
 { F2_KEY,        "st",             "Enter Star",          TRUE},
 { F3_KEY,        ".ss;save",       "Save Eclipse",        TRUE},
 { F4_KEY,        "focus",          "Focus",               TRUE},
 { F5_KEY,        "ac",             "Autocite",            TRUE},
 { F6_KEY,        "shep",           "Shepard's",           TRUE},
 { F7_KEY,        "lexsee",         "Lexsee",              TRUE},
 { F8_KEY,        "lexstat",        "Lexstat",             TRUE},
 { F9_KEY,        "resume lexis",   "Resume Lexis",        FALSE},
 { F10_KEY,       ".ss",            "Select Service",      FALSE},
 { F11_KEY,       ".mi",            "Print-All",           FALSE},
 { F12_KEY,       ".pr",            "Print Document",      FALSE},
 { SF2_KEY,       "xst",            "Exit Star",           FALSE},
 { SF3_KEY,       ".ss;rec",        "Recall Eclipse",      FALSE},
 { SF4_KEY,       ".ex",            "Exit Focus",          TRUE},
 { SF5_KEY,       "resume ac",      "Resume Autocite",     FALSE},
 { SF6_KEY,       "resume shep",    "Resume Shepard's",    FALSE},
 { SF7_KEY,       "resume lexsee",  "Resume Lexsee",       FALSE},
 { SF8_KEY,       "resume lexstat", "Resume Lexstat",      FALSE},
 { SF10_KEY,      ".es",            "Exit Service",        FALSE},
 { NEW_PAGEUP_KEY, ".pp",           "Previous Page",       TRUE},
 { NEW_PAGEDOWN_KEY, ".np",         "Next Page",           TRUE},
 { KPSLASH_KEY,   ".fu",            "Full",                TRUE},
 { KPASTERIK_KEY, "m",              "Modify",              TRUE},
 { KP1_KEY,       ".pp",            "Previous Page",       TRUE},
 { KP2_KEY,       ".fp",            "First Page",          FALSE},
 { KP3_KEY,       ".fd",            "First Document",      FALSE},
 { KP4_KEY,       ".se",            "Segments",            FALSE},
 { KP5_KEY,       ".dl",            "Different Level",     TRUE},
 { KP6_KEY,       ".cf",            "Change File",         FALSE},
 { KP7_KEY,       ".ci",            "Cite",                FALSE},
 { KP8_KEY,       ".vk",            "Var KWIC",            TRUE},
 { KP9_KEY,       ".cl",            "Change Library",      FALSE},
 { KPENTER_KEY,   ".nd",            "Next Document",       TRUE},
 { KPINS_KEY,     ".np",            "Next Page",           TRUE},
 { KPDEL_KEY,     ".pd",            "Previous Document",   TRUE},
 { KPPLUS_KEY,    ".ns",            "New Search",          FALSE},
 { NUMLOCK_KEY,   ".kw",            "KWIC",                TRUE},  // NumLk translates to this key
 { SCRLOCK_KEY,   ".so;n",          "Sign Off",            FALSE}, // ScrLk translates to this key
 { PAUSE_KEY,     STOP_SEQ,         "STOP",                FALSE}, // Pause translates to this key
 { ENTER_KEY,     "",               "",                     TRUE}
};


extern TYPE_COLOR color[5];   // defined in scrntool.c


/* value shared between online and gettoclientscreen */
BOOLEAN  OldBeep;


/* private functions */
BOOLEAN isClient        (TYPE_TS *, OLDATA *);
BOOLEAN isDeferredPrint (TYPE_TS *, OLDATA *);
BOOLEAN isSavedSearches (TYPE_TS *, OLDATA *);



/*****************************************************************************
* FUNCTION NAME:  Online                                                     *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Online maintains OLS session                                             *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = Online (search, onlineInfo, debug);                             *
*                                                                            *
*   INT  status        - returned INT value                                  *
*                                                                            *
*   CHAR *search           - search to send the OLS                          *
*   OLDATA *onlineInfo     - to be filled in with online info from .dat files*
*   BOOLEAN debug          - TRUE if debug mode, FALSE if normal mode        *
*                                                                            *
*                                                                            *
* PSEUDO-CODE                                                                *
*                                                                            *
*  initialize terminal status                                                *
*  attempt to advance to client screen                                       *
*  send the search (client and then rest of search portion)                  *
*  stuff a help key so advice will pop up                                    *
*  go into the edit/send/receive session                                     *
*  cleanup                                                                   *
*                                                                            *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, EXIT, or ESCAPE                                                 *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/05/90                                                            *
*                                                                            *
*****************************************************************************/




INT   Online (search, onlineInfo, debug)

CHAR     *search;
OLDATA   *onlineInfo;
BOOLEAN  debug;

   {

   BOOLEAN  searchSent = FALSE;
   BOOLEAN  lock_kbrd  = TRUE;
   INT      status;
   TYPE_TS  termStatus;
   CHAR     screenBuff [(ROWS-1) * COLS + 1];
   CHAR     *temp;

   Sys_setOnline (TRUE);
   set_scr_size (0, 0, ROWS - 2, COLS);

/* initialize terminal status structure */
   termStatus.online       = TRUE;            
   termStatus.kbd_locked   = TRUE; 
   termStatus.first_block  = TRUE;
   termStatus.id_flag      = TRUE;
   termStatus.stop_sent    = FALSE;
   termStatus.status       = SUCCESS;
   termStatus.rec_on       = FALSE;     
   termStatus.disp_data    = TRUE;  
   termStatus.doc_disk_on  = FALSE;
   termStatus.prt_on       = FALSE;
   termStatus.cur_col      = 0;    
   termStatus.cur_row      = 0;    
   termStatus.client       = FALSE;
   termStatus.echo         = TRUE;
   termStatus.scrnBuffPtr  = screenBuff;


   create_window (&termStatus.stat_win, 1, 80, DISP_INFO);
   display_window (&termStatus.stat_win, ROWS - 1, 0);
   OLS_UpdateBar (&termStatus, " Working on your search request.", FALSE);
   viatrect(24, 0, 24, 79, color[DISP_INFO].fore, color[DISP_INFO].back | 8);
   set_attributes(BOLD_OFF, DISP_INFO);

   OldBeep = onlineInfo->beep;

/* turn off debug screen, if in debug mode  */
   if (!debug)
      {  
      Disable_Screen ();
      onlineInfo->beep = FALSE;
      }



/* create status bar clear screen */
   clear_screen ();

   if (UserDefs.tobasco == TRUE)
      {
      TobascoInst ();
      }

/* advance to client screen */
   status = GetToClientScreen (&termStatus, onlineInfo);

   if (status == SUCCESS  &&  search != NULL)
      {
      temp = strtok(search,";");
      status = OLS_SendStr (&termStatus, temp, temp); //send client ID
      
      if (status == SUCCESS  &&  temp != NULL)
         {   // receive next screen 
         termStatus.kbd_locked = TRUE;
         status = OLS_Receive (&termStatus, onlineInfo); //receive next screen

         if ((status == SUCCESS) && (isClient (&termStatus, onlineInfo)))
            {
            //if next screen is client screen, send ID again
            //   (happens when system is going down soon
            status = OLS_SendStr (&termStatus, temp, temp); 
            if (status == SUCCESS)
               {
               termStatus.kbd_locked = TRUE;
               status = OLS_Receive (&termStatus, onlineInfo);
               }
            }

         temp = strtok(NULL,"");  // more out there to send? ie search string 
    
         // send rest of search  
         if ((status == SUCCESS) && (temp != NULL))
            {
            status = OLS_SendStr (&termStatus, temp, temp);
            }
         else
            {
            lock_kbrd = FALSE;  // we just had a client and no search
            }
         } // end for more to send

      if (lock_kbrd)
         {
         termStatus.kbd_locked = TRUE;
         }
      searchSent = TRUE;
      onlineInfo->beep = OldBeep;  
      }


   if (status == SUCCESS)
      {
      status = OLS_Receive (&termStatus, onlineInfo);
      }

/* stuff a help key for advice */
//   if (status == SUCCESS  &&  search != NULL)
//      {
//      Sys_kbstuff (HELP_KEY);
//      }

   if (status == SUCCESS)
      {
      Enable_Screen ();
      onlineInfo->beep = OldBeep;
      }

/* go into edit/send/receive session */
   while (Sys_isOnline ()   &&   termStatus.online   &&   status == SUCCESS)
      {
      status = OLS_EditSendRcv (&termStatus, onlineInfo);
      }


/* clean up */
   if (UserDefs.tobasco == TRUE)
      {
      TobascoDel ();
      }

   if (termStatus.prt_on)
      {
      if (Print_Close () == ESCAPE)
         {
         if (status != QUIT)
            {
            status = SUCCESS;
            }
         }
      termStatus.prt_on = FALSE;
      }

   if (termStatus.doc_disk_on)
      {
      fclose (termStatus.doc_file);
      termStatus.doc_disk_on = FALSE;
      }

   if (termStatus.rec_on)
      {
      fclose (termStatus.rec_file);
      termStatus.rec_on = FALSE;
      }


   set_attributes (BOLD_OFF, DISP_NORMAL);
   clear_line(ROWS-2,0,80);
   delete_window (&termStatus.stat_win);
   set_scr_size (0, 0, ROWS - 1, COLS);

   return (status);
   }







/*****************************************************************************
* FUNCTION NAME:  GetToClientScreen                                          *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   Advance to client screen                                                 *
*                                                                            *
*                                                                            *
* SYNOPSIS:                                                                  *
*                                                                            *
*   status = GetToClientScreen (termStatus, onlineInfo)                      *
*                                                                            *
*   INT  status        - returned INT value                                  *
*                                                                            *
*   TYPE_TS                - terminal status                                 *
*   OLDATA *onlineInfo     - to be filled in with online info from .dat files*
*                                                                            *
*                                                                            *
* PSEUDO-CODE                                                                *
*                                                                            *
*  receive a screen                                                          *
*  send the lexis/nexis id                                                   *
*  receive the next screen                                                   *
*  if needed, send the prep string and receive the next screen               *
*  if screen is deferred printing, answer no and receive next screen         *
*  if screen is saved searches, answerno and receive next screen             *
*  do                                                                        *
*     if client screen is found                                              *
*         return                                                             *
*     else                                                                   *
*         edit/send/receive                                                  *
*  while client screen is not found                                          *
*                                                                            *
*                                                                            *
* CAUTIONS:  none                                                            *
*                                                                            *
* RETURNS:                                                                   *
*                                                                            *
*   SUCCESS, EXIT, or ESCAPE                                                 *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/05/90                                                            *
*                                                                            *
*****************************************************************************/




INT  GetToClientScreen (termStatus, onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   INT  status;


/* receive next screen and send the lexis/nexis id */
   if ((status = OLS_Receive (termStatus, onlineInfo)) != SUCCESS)
      {
      return (status);
      }

   if ((status = OLS_SendStr (termStatus, onlineInfo->lexisID, "your personal LEXIS/NEXIS ID")) != SUCCESS)
      {
      return (status);
      }

   termStatus->kbd_locked = TRUE;
   if ((status = OLS_Receive (termStatus, onlineInfo)) != SUCCESS)
      {
      return (status);
      }


/* send prep if needed and receive next screen */
   if (Prep[0] != '\0')
      {
      if ((status = OLS_SendStr (termStatus, Prep, Prep)) != SUCCESS)
         {
         return (status);
         }
      termStatus->kbd_locked = TRUE;
      if ((status = OLS_Receive (termStatus, onlineInfo)) != SUCCESS)
         {
         return (status);
         }
      }


/* if screen is deferred printing, answer no */
   if (isDeferredPrint (termStatus, onlineInfo))
      {
      if ((status = OLS_SendStr (termStatus, NO_ANSWER, NO_ANSWER)) != SUCCESS)
         {
         return (status);
         }
      termStatus->kbd_locked = TRUE;
      if ((status = OLS_Receive (termStatus, onlineInfo)) != SUCCESS)
         {
         return (status);
         }
      }

/* if screen is saved searches, answer no */
   if (isSavedSearches (termStatus,onlineInfo))
      {
      if ((status = OLS_SendStr (termStatus, NO_ANSWER, NO_ANSWER)) != SUCCESS)
         {
         return (status);
         }
      termStatus->kbd_locked = TRUE;
      if ((status = OLS_Receive (termStatus, onlineInfo)) != SUCCESS)
         {
         return (status);
         }
      }



/* loop until client screen is found */
   do
      {
      if (isClient (termStatus, onlineInfo))
         {
         termStatus->client = TRUE;
         }
      else
         {
         Enable_Screen ();
         onlineInfo->beep = OldBeep;
         status = OLS_EditSendRcv (termStatus, onlineInfo);
         }
      }  while (status == SUCCESS   &&   termStatus->client == FALSE  && 
                termStatus->online == TRUE);

   if (termStatus->online == FALSE) // did the OLS drop us? (time out, ..etc)
      {
      status = ESCAPE;
      }
   return (status);
   }




/************  isClient  - private function

isClient returns a TRUE value if the current screen is the client
screen.  The recognition is done through a strstr call and is seeded
with the scrnBuffer and the onlineInfo->ruler.  Notice that the client
screen will be recognized no matter where the ruler appears on the 
screen.
*************/



BOOLEAN  isClient (termStatus, onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   read_screen (termStatus->scrnBuffPtr);
   termStatus->scrnBuffPtr [(ROWS-1)*COLS] = '\0';

   if (strstr (termStatus->scrnBuffPtr,onlineInfo->ruler) == NULL)

      {
      return (FALSE);
      }

   return (TRUE);
   }





/************  isDeferredPrint  - private function

isDeferredPrint recognizes the deferred print screen using the strstr
function seeded with scrnBuffer and onlineInfo->deferredPrint.

************/



BOOLEAN  isDeferredPrint (termStatus,onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   read_screen (termStatus->scrnBuffPtr);
   termStatus->scrnBuffPtr [(ROWS-1)*COLS] = '\0';

   if (strstr (termStatus->scrnBuffPtr, onlineInfo->deferredPrint) == NULL)
      {
      return (FALSE);
      }

   return (TRUE);
   }




/********** isSavedSearches  - private function

isSavedSearches recognizes the saved searches screen using the strstr
function seeded with scrnBuffer and onlineInfo->savedSearch.

**********/


BOOLEAN  isSavedSearches (termStatus,onlineInfo)

TYPE_TS  *termStatus;
OLDATA   *onlineInfo;

   {

   read_screen (termStatus->scrnBuffPtr);
   termStatus->scrnBuffPtr [(ROWS-1)*COLS] = '\0';

   if (strstr (termStatus->scrnBuffPtr, onlineInfo->savedSearch) == NULL)
      {
      return (FALSE);
      }

   return (TRUE);
   }
