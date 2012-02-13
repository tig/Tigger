/* input.c               (c) KindelCo Software Systems, 1987

                      INPUT - Input routines for "make" (In)


/*==========================================================================*/
/* input.c        Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/
#include "include/typedef.h"
#include "include/make.h"
#include "include/input.h"
#include "include/graph.h"
#include <stdio.h>
#include <ctype.h>


/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define READ        "r"                /* read mode for fopen();            */
#define CMND        0                  /* if line is a command...           */
#define MACDEF      1                  /* if line is macro definiton        */
#define DEPEND      2                  /* if line is a dependancy           */
#define NOTHING     -1                 /* if line is a comment or blank     */
#define EQUAL       '='                /* search for equal sign             */
#define SPACE       ' '                /* a blank space                     */
#define MAKEFILE    "makefile"
#define NEWLINE     '\n'

/* MACROS ------------------------------------------------------------------*/

/* TYPEDEFS ----------------------------------------------------------------*/
FILE *f_p, *fopen ();

/* DATA DEFINITIONS --------------------------------------------------------*/
short linetype;
short InLineNum;
char *InFileName;

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                                InInput                                   */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void InInput ()
{    char line [MAXEXPAND];
     int length;
     DEPLISTPTR deplistp;
     InLineNum = 0;

     if ((f_p = fopen (MAKEFILE, READ)) != NULL)
     /* while there are more lines in the file */
     {   while (fgets (line, MAXLINE+1, f_p) != NULL) /* !EOF */
         {    InLineNum++;
              InStrip (line);

              linetype = NOTHING;
              if (line [0] != NULL)
                  if (isspace (line [0]))
                      linetype = CMND;
                  else
                    (strchr (line, EQUAL) != NULL) ?
                         (linetype = MACDEF) : (linetype = DEPEND);

              /* Now we've got a line and we know what it is let's do it */
              if (linetype != NOTHING)
                  MacExpand (line);
              if (strlen (line) >= MAXEXPAND)
              {    MkError (EXPTOOLNG_ERR);
                   exit (FATAL);
              }
              switch (linetype)
              {    case CMND   :  GrCmnd (line);
                                  break;
                    case MACDEF : MacDef (line);
                                  break;
                    case DEPEND : if (MkTargetName == NULL)
                                  {   length = strcspn (line, " :");
                                      MkTargetName = (char*)malloc (length+1);
                                      strncpy (MkTargetName, line, length);
                                      MkTargetName [length] = NULL;
                                  }
                                  deplistp = GrBuild (line);
                                  break;
                    default     : break;
              }
         } /* end of while */

     }
     else
     {    MkError (NOOPEN_ERR);
          exit (FATAL);
     }


     return;

} /* End of input.c */


/*==========================================================================*/
/*                         InStrip                                          */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void InStrip (line)
char line [];
{    short i=0, j;
     char temp [MAXEXPAND];


     while (line [strlen (line)-2] == '\\' || line [strlen (line)-1] == '\\')
     {    if (line [strlen (line)-2] == '\\')
              line [strlen (line)-2] = ' ';
          else 
              line [strlen (line)-1] = ' ';
          if ((fgets (temp, MAXLINE+1, f_p) == NULL) || 
              (strlen (line) >= MAXEXPAND))
          {   MkError (BADCONT_ERR);
              exit (FATAL);
          }
          InLineNum++;
          strcat (line, temp);
     }      

     if (strlen (line) >= MAXEXPAND)
     {   MkError (EXPTOOLNG_ERR);
         exit (FATAL);
     }    

     while (line [i] != NULL)
     {    if (isspace (line [i]))
          {   line [i] = SPACE;
              while ((line [i+1] != NULL) && (isspace (line [i+1])))
                  for (j=i+1; line [j] != NULL; j++)
                      line [j] = line [j+1];
          }
          else
             if (line [i] == '#')
             {    while ((i > 0) && (isspace (line[i - 1])))
                      i--;
                  line [i] = NULL;
                  return;
             }

          i++;
     }
     if (isspace (line [i-1]))
        line [i-1] = NULL;

     return;
}

