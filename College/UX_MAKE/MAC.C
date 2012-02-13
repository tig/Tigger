/* mac.c               (c) KindelCo Software Systems, 1987

Author:   Charles E. Kindel, Jr. (tigger)

Started:  August 5, 1987
Version:  1.00

/* IMPORTS -----------------------------------------------------------------*/
#include "include/typedef.h"
#include "include/mac.h"
#include "include/sym.h"
#include <stdio.h>
#include <ctype.h>


/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/

/* MACROS ------------------------------------------------------------------*/

/* TYPEDEFS ----------------------------------------------------------------*/
char *malloc ();
/* DATA DEFINITIONS --------------------------------------------------------*/

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                         MacDef                                           */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MacDef (line)
char *line;
{    char *def, *name;
     GRNODEPTR node;
     int length;

     length = strcspn (line, " =");
     name = malloc (length + 1);
     strncpy (name, line, length);
     *(name+length) = NULL;

     while (*line++ != '=')
          ;
     def = line;
     while ((*def != NULL) && (isspace (*def)))
          def++;
     if (*def == NULL)
     {    MkError (BADMACDEF_WAR);
          *--def = ' ';
     }

     node = SymAdd (name, SMAC);
     node->macroval = malloc (strlen (def)+1);
     strcpy (node->macroval, def);
     return;
} /* end of MacDef */

/*==========================================================================*/
/*                            MacExapand                                    */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void MacExpand (line)
char *line;
{    char *loc, *name, *temp;
     GRNODEPTR node;
     int i, length;

     while ((loc = (char*) strchr (line, '$')) != NULL)
     {    strcpy (loc, loc+1); /* nuke the $ */
          if (*loc != '(')
          {   name = malloc (2);
              *name = *loc;
              *(name+1) = NULL;
              strcpy (loc, loc+1);
          }
          else
          {   strcpy (loc, loc+1);
              length = strcspn (loc, ")\0");
              name = malloc (length+1);
              strncpy (name, loc, length);
              *(name+length) = NULL;
              for (i=0; i <= length; i++)
                 strcpy (loc, loc+1);
          }
          if ((node = SymFind (name, SMAC)) != NULL)
          {     temp = malloc (strlen(loc)+1);
                strcpy (temp, loc);
                strcpy (loc, node->macroval);
                strcat (loc, temp);
          }
          else
          {     MkError (NOMACDEF_ERR);
                exit (FATAL);
          }
     }
}





/*==========================================================================*/
/* END OF mac.c                         (C) KindelCo Software Systems, 1987 */
/*==========================================================================*/
