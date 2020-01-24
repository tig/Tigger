/*****************************************************************************
* FILE NAME:  globals.h                                                      *
*                                                                            *
* DESCRIPTIVE TITLE:                                                         *
*                                                                            *
*   global defines and structures.                                           *
*                                                                            *
*                                                                            *
* DATA STRUCTURES:                                                           *
*                                                                            *
*   OLDATA                                                                   *
*                                                                            *
*                                                                            *
* AUTHOR:  Dean Myers                                                        *
*                                                                            *
* DATE:  07/05/90                                                            *
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



/***************************************************************************
 *  $Log:   N:/src/globals.h_v  $
   
      Rev 8.0   18 Dec 1990 10:57:40   joeb
   TURBO NEXIS VERSION 1.00 CERTIFIED AT ITERATION #5I -- REVISION 8.0
   
      Rev 7.0   06 Dec 1990 15:19:28   joeb
   revision 7.0 for iteration 5
   
      Rev 6.0   19 Nov 1990  9:24:16   joeb
   revision 6 for interation 4
   
      Rev 5.0   24 Oct 1990  9:08:08   andrews
   No change.
   
      Rev 4.0   01 Oct 1990 13:25:58   andrews
   ITERATION NUMBER 2
   
      Rev 3.3   29 Sep 1990 17:05:58   joeb
   OLS change insulation
   
      Rev 3.2   24 Sep 1990 15:13:44   vincentt
   changed PATH_SIZE to 67
   
      Rev 3.1   22 Sep 1990  9:33:12   vincentt
   changed PATH_SIZE to 55
   
      Rev 3.0   17 Sep 1990 10:08:02   andrews
   Full Iteration 1
   
      Rev 2.3   11 Sep 1990 12:11:46   andrews
   changed expert sel list node
   
      Rev 2.2   05 Sep 1990 10:00:50   andrews
   changed tmenu desc size from 64 to 60
   added sscript desc size and max sscript size
   
      Rev 2.1   30 Aug 1990 11:39:52   andrews
   added EXP_SEL_DATA_TYPE
   added TMENU_MAX_SIZE
   added globals for expert
   
      Rev 2.0   27 Aug 1990  9:22:16   andrews
   iteration 1
   
      Rev 0.17   24 Aug 1990 11:28:28   andrews
   changed tmenu desc size from 60 to 64
   
      Rev 0.16   24 Aug 1990  9:16:22   vincentt
   deleted #defines for dlogo
   
      Rev 0.15   22 Aug 1990  9:39:52   deanm
   adding real copy of lexis id in userdefs
   
      Rev 0.14   15 Aug 1990 13:41:30   vincentt
   added #define PRINTDOC_CANCEL, PRINTDOC_APPEND, PRINTDOC_REPLACE
   
      Rev 0.13   14 Aug 1990 14:08:58   vincentt
   added #define for dexit.c, ddelete.c, dlogo.c
   
      Rev 0.12   13 Aug 1990  8:57:38   deanm
   added lfeed and lwrap to userdefs
   
      Rev 0.11   10 Aug 1990 11:02:32   vincentt
   added #define LF, FF
   
      Rev 0.10   10 Aug 1990  9:59:28   vincentt
   added #define CONST const
   
      Rev 0.9   08 Aug 1990 16:04:12   vincentt
   added #define STATIC
   
      Rev 0.4   06 Aug 1990 13:34:24   deanm
   adding in tmenuName in OLDATA for TmenuSave
   
      Rev 0.3   02 Aug 1990 15:32:02   deanm
   adding things for upinit and upsave
   
      Rev 0.2   30 Jul 1990 21:54:52   deanm
   added prep to oldata 
   
      Rev 0.1   30 Jul 1990  9:06:12   deanm
   added entry for client id in OLDATA
   
      Rev 0.0   25 Jul 1990 15:26:10   deanm
   Initial revision.
 *
****************************************************************************/


#ifndef GLOBALS_H
#define GLOBALS_H

#include <stddef.h>     /* standard c definintions */

/* version number of this release */
#define VERSION 100



/* screen size */
#define COLS     80
#define ROWS     25



/* define nicey's */
#define TRUE      1
#define FALSE     0
#define ON        TRUE
#define OFF       FALSE
#define ENABLE    1
#define DISABLE   0



/* define global return values */
#define FAILURE  -1
#define SUCCESS   0
#define ESCAPE    1
#define RETRY     2
#define QUIT      3
#define NOT_PROCESSED 4



/* define global simple types */

#define BYTE      unsigned char
#define CHAR      char
#define INT       int
#define WORD      unsigned int
#define LONG      long
#define DWORD     unsigned long
#define UNSIGNED  unsigned
#define BOOLEAN   unsigned char
#define STATIC    static
#define CONST     const


/* Andrew */

#define PATH_SIZE         67     // minimum size required by flnorm
#define TMENU_DESC_SIZE   60
#define MAX_TMENU_SIZE    30
#define SSCRIPT_DESC_SIZE   78
#define MAX_SSCRIPT_SIZE    30
#define FILENAME_SIZE     8
#define FILEEXT_SIZE      4

#define PRINT_CONTROL_STRING 40
#define PRINT_NAME_SIZE      20


/* Vincent */

#define STREAM_PRINTER        1  
#define STREAM_DISK           2
#define STREAM_BOTH           3

#define VERIFY_CANCEL         1
#define VERIFY_CONTINUE       2

#define DELETE_YES            1
#define DELETE_NO             2

#define EXIT2DOS_CANCEL       1
#define EXIT2DOS_NOSAVE       2

#define PRINTDOC_CANCEL       1
#define PRINTDOC_APPEND       2
#define PRINTDOC_REPLACE      3

#define FF                    0x0C
#define LF                    0x0A
#define CR                    0x0D


typedef struct      /* SIZE_TYPE */
  {
     INT   row;
     INT   col;
  } SIZE_TYPE;



                 /* TMENU_ITEM_ETYPE - enumerated types of turbo menu items */
typedef enum
  {
     search_et,
     menu_et,
     nomenuitem_et
  } TMENU_ITEM_ETYPE;





typedef struct
  {
  CHAR              filename[FILENAME_SIZE+1];
  TMENU_ITEM_ETYPE  type;
  INT               top;
  INT               current;
  } SELECTION_DATA_TYPE;

typedef struct
  {
  CHAR              path[PATH_SIZE+1];
  CHAR              filename[FILENAME_SIZE+1];
  CHAR              tempname[PATH_SIZE+FILENAME_SIZE+FILEEXT_SIZE+1];
  INT               top;
  INT               current;
  TMENU_ITEM_ETYPE  type;
  BOOLEAN           changed;
  } EXP_SEL_DATA_TYPE;


 typedef struct
   {
   CHAR     lexisID  [8];
   CHAR     clientID [33];
   CHAR     prep [COLS + 1];                /* over ride with /PREP= option */
   CHAR     tmenuName [FILENAME_SIZE + 1];  /* over ride with /TUR= option */
   BOOLEAN  beep;
   CHAR     sessRec  [COLS + 1];
   CHAR     docToDsk [COLS + 1];
   CHAR     ruler         [33];  //for OLS screen recognition
   CHAR     deferredPrint [53];  //for OLS screen recognition
   CHAR     savedSearch   [53];  //for OLS screen recognition
   BOOLEAN  checkCD;         
   } OLDATA;


typedef struct
   {
   INT      tobasco;
   CHAR     prtr_start [PRINT_CONTROL_STRING + 1];
   CHAR     prtr_end   [PRINT_CONTROL_STRING + 1];
   INT      screen_saver;
   CHAR     pr_name    [PRINT_NAME_SIZE + 1];
   CHAR     pr_location[PRINT_NAME_SIZE + 1];
   CHAR     app_name   [PATH_SIZE + FILENAME_SIZE + FILEEXT_SIZE + 1];
   CHAR     lexisID  [8];  /* this is a copy of what is in the data files */
   INT      lfeed;
   INT      lwrap;
   } USERDEFS;




/* global variables */

extern USERDEFS UserDefs;
extern CHAR     Path[];
extern CHAR     PrefPath[];
extern CHAR     Prep[];
extern CHAR     *Advice;


/* global variables - expert */
extern BOOLEAN  EscUp;
extern BOOLEAN  TitleUp;
extern BOOLEAN  MMHelpUp;


#endif
