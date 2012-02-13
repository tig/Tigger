/* sym.c               (c) KindelCo Software Systems, 1987

                       SYMBOL - symbol table routines (Sym)

Author:   Charles E. Kindel, Jr. (tigger)

Started:  August 5, 1987
Version:  1.00

SYNOPSIS

DESCRIPTION

EXAMPLES

MESSAGES

LIMITATIONS

FACILITY

NOTES

REVISIONS




/* IMPORTS -----------------------------------------------------------------*/
#include "include/typedef.h"
#include "include/sym.h"
#include <string.h>
#include <stdio.h>

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/

/* DATA DEFINITIONS --------------------------------------------------------*/
GRNODEPTR SymTable [TABLESIZE];

/* FUNCTION PROTOTYPES -----------------------------------------------------*/

/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                            SymInit                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void SymInit ()
{    int i;

     for (i=0; i < TABLESIZE; i++)
        SymTable [i] = NULL;
} /* end of SymInit */


/*==========================================================================*/
/*                            SymAdd                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
GRNODEPTR SymAdd (name,type)
char *name;
short type;
{    GRNODEPTR node;

     if ((node = SymFind (name, type)) == NULL)
     {    node = (GRNODEPTR) malloc (sizeof (GRNODE));
          node->grph_link = (GRNODEPTR) NULL;
          node->nodetype = type;
          node->nodename = (char*) malloc (strlen (name)+1);
          (void) strcpy (node->nodename, name);
          node->outdate = node->status = node->time = 0;
          node->prelist = (DEPLISTPTR) NULL;
          node->cmndlist = (CMNDLISTPTR) NULL;
          SymHashAdd (node, SymHash (name));
     }

     return (node);
} /* End of SymAdd */


/*==========================================================================*/
/*                            SymFind                                       */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
GRNODEPTR SymFind (name, type)
char *name;
short type;
{    GRNODEPTR node;

     if ((node = SymTable [SymHash (name)]) == NULL)
         return (NULL);
     else
     {   while (node != NULL)
             if ((strcmp (name, node->nodename) == 0) && (type == node->nodetype))
                 return (node);
             else
                 node = node->grph_link;
         return (NULL);
     }
} /* end of SymFind */

/*==========================================================================*/
/*                           SymHash                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int SymHash (name)
char *name;
{   int sum = 0;
     for (;*name != NULL; name++)
        sum += (int) *name;
     return (sum % TABLESIZE);
} /* end of SymHash */


/*==========================================================================*/
/*                              SymHashAdd                                  */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void SymHashAdd (node, location)
GRNODEPTR node;
int location;
{   GRNODEPTR temp;

    temp = SymTable [location];
    if (temp == NULL)
        SymTable [location] = node;
    else
    {   while (temp->grph_link != NULL)
           temp = temp->grph_link;
        temp->grph_link = node;
    }
    return;
} /* end of SymHashAdd */



/*==========================================================================*/
/*                           SymPrnt                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void SymPrnt ()
{    int i;
     GRNODEPTR node;
     DEPLISTPTR temp;
     CMNDLISTPTR temp2;

     for (i=0; i < TABLESIZE; i++)
     {    printf ("\n%2d: ", i);
          node = SymTable [i];
          while (node != NULL)
          {    switch (node->nodetype)
               {   case SMAC : printf ("SMAC  "); break;
                   case DMAC : printf ("DMAC  "); break;
                   case SFILE : printf ("SFILE "); break;
                   default   : printf ("ERR   "); break;
               }
               printf ("%s", node->nodename);
               if (node->nodetype == SMAC)
                   printf (" = %s", node->macroval);
               else
               {   temp = node->prelist;
                   while (temp != NULL)
                   {   printf (" %s,", temp->dep_nodep->nodename);
                       temp = temp->dep_link;
                   }
                   temp2 = node->cmndlist;
                   while (temp2 != NULL)
                   {   printf (" %s,", temp2->cmnd);
                       temp2 = temp2->cmnd_link;
                   }
               }
               node = node->grph_link;
               if (node != NULL)
                   printf ("\n    ");
          }
     }
}


/*==========================================================================*/
/* END OF sym.c                         (C) KindelCo Software Systems, 1987 */
/*==========================================================================*/
