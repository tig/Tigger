/* make.c               (c) KindelCo Software Systems, 1987

                      MAKE - A file update facility (Mk)


/*==========================================================================*/
/* make.c         Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/
#include "include/typedef.h"
#include "include/make.h"
#include "include/input.h"
#include "include/update.h"
#include <stdio.h>
#include <string.h>
#ifdef TURBOC
#include <process.h>
#include <getopt.h>
#else
extern int optind;
#endif

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define OPTIONS    "tun"               /* possible arguments on command line*/
#define NOPT       'n'                 /* offline option (-n)               */
#define UOPT       'u'                 /* update unconditionally option     */
#define TOPT       't'                 /* touch option                      */

/* DATA DEFINITIONS --------------------------------------------------------*/
short Mkofflinef;       /* flag for -n option (offline) GLOBAL              */
short Mktouchf;         /* flag for touch option GLOBAL                     */
short Mkupdatef;        /* flag for unconditional update                    */
short MkStatus;         /* exit status                                      */
char* MkTargetName;     /* target name (NULL if not exist)                  */
long MkCurTime;

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                               Main                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int main (argc, argv)
int argc;
char *argv[];
{   static char c;
    GRNODEPTR targetp;

    Mkofflinef = FALSE;
    Mktouchf = FALSE;
    Mkupdatef = FALSE;

    while ((c = getopt (argc, argv, OPTIONS)) != EOF)
        switch (c)
        {    case NOPT : if (!Mktouchf) 
                             Mkofflinef = TRUE;
                         else
                         {   MkError (NANDTOPT_ERR);
                             exit (FATAL);
                         }
                         break;
             case TOPT : if (Mkofflinef)
                         {   MkError (NANDTOPT_ERR);
                             exit (FATAL);
                         }
                         else if (Mkupdatef)
                         {   MkError (UANDTOPT_ERR);
                             exit (FATAL);
                         }
                         Mktouchf = TRUE;
                         break;
             case UOPT : if (Mktouchf)
                         {   MkError (UANDTOPT_ERR);
                             exit (FATAL);
                         }
                         Mkupdatef = TRUE;
                         break;
             default   : MkError (BADOPT_ERR);
                         exit (FATAL);
                         break;
        }

    if (optind < argc)                 /* optind is an external from getopt */
    {   MkTargetName = (char*) malloc (strlen (argv[optind])+1);
        strcpy (MkTargetName, argv[optind]);
    }
    else
        MkTargetName = NULL;

    printf ("MAKE : KindelCo Software Systems (C) 1987\n");

    SymInit ();                         /* initialize the hash table */
    InInput ();                         /* read the makefile and build the graph */
    printf ("\n\tusing [%s] for target.\n\n", MkTargetName);

    MkCurTime = time(0);

    if (SymFind (MkTargetName, SFILE) != NULL)
        UpDate (SymFind (MkTargetName, SFILE));
    else 
    {   InFileName = MkTargetName;
        MkError (DONTKNOW_ERR);
        exit (FATAL);
    }
    return (MkStatus);

}

/*==========================================================================*/
/*                              MkError                                     */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MkError (ErrorCode)
short ErrorCode;
{    fprintf (stderr, "make: ");
     switch (ErrorCode)
     {   case BADOPT_ERR :
              fprintf (stderr, "(Fatal) Invalid command option.\n");
              break;
         case NOOPEN_ERR :
              fprintf (stderr, "(Fatal) Can't open makefile.\n");
              break;
         case TOLONG_ERR :
              fprintf (stderr, "(Fatal) Line %d: Line too long.\n", InLineNum);
              break;
         case DONTKNOW_ERR :
              fprintf (stderr, "(Fatal) Don't know how to make [%s].\n", InFileName);
              break;
         case BADMACDEF_WAR :
              fprintf (stderr, "(Warning) Line %d: Bad macro definition.\n", InLineNum);
              break;
         case NOMACDEF_ERR :
              fprintf (stderr, "(Fatal) Line %d: Undefined macro.\n", InLineNum);
              break;
         case TOOLNG_ERR :
              fprintf (stderr, "(Fatal) Line %d: Input line exceeds %d characters.\n", InLineNum, MAXLINE);
              break;
         case EXPTOOLNG_ERR :
              fprintf (stderr, "(Fatal) Line %d: Expanded line exceeds %d characters.\n", InLineNum, MAXEXPAND);
              break;
         case BADCONT_ERR :
              fprintf (stderr, "(Fatal) Line %d: Bad continuation of line.\n", InLineNum);
              break; 
         case NANDTOPT_ERR :
              fprintf (stderr, "(Fatal) -n option and -t option cannot be used together.\n");
              break;
         case UANDTOPT_ERR :
              fprintf (stderr, "(Fatal) -u option and -t option cannot be used together.\n");
              break;
         default :
              fprintf (stderr, "Line %d: unknown error.\n", InLineNum);
              break;
     }
     return;
}

