/* graph.c               (c) KindelCo Software Systems, 1987

                      GRAPH - "Graph" routines for MAKE (Gr)


*/
/*==========================================================================*/
/* graph.c        Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/
#include "include/typedef.h"
#include "include/graph.h"
#include "include/sym.h"
#include <stdio.h>



/* FUNCTIONS ---------------------------------------------------------------*/
/*==========================================================================*/
/*                           GrBuild                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
DEPLISTPTR GrBuild (line)
char *line;
{
      char *depend, *precurs, *name, *curpre;
      DEPLISTPTR Depend_list, cur, prev, temp;
      GRNODEPTR node;
      int length;

      /* break the line into two parts; the dependency part and the precursor*/
      /* part...                                                             */
      length = strcspn (line, ":");
      depend = (char*) malloc (length+1);
      strncpy (depend, line, length);
      if (*(depend+(length-1)) == ' ')
         *(depend+(length-1)) = NULL;
      else
         *(depend+length) = NULL;

      precurs = line + (length+1);
      if (*precurs == ' ')
         strcpy (precurs, precurs+1);

      /* use SymAdd to add (if needed) all dependencies to the graph */
      /* also attach each dependency in a linked list... */
      Depend_list = NULL;
      while (*depend != NULL)
      {    length = strcspn (depend, " \0");    /* find the len of the depend. */
           name = (char*) malloc (length+1);
           strncpy (name, depend, length);      /* copy the name into name */
           *(name+length) = NULL;               /* put a null on the end */
           if (*(depend + length) == NULL)
              length--;
           depend += length+1;                  /* delete this one from the line */
           node = SymAdd (name, SFILE);         /* add it to the symbol table */
           free (name);
           cur = Depend_list;
           while (cur != NULL)                  /* get to end of linked list */
           {  prev = cur;
              cur = cur->dep_link;
           }
           cur = (DEPLISTPTR) malloc (sizeof (DEPLIST));         /* allocate a link */
           cur->dep_link = NULL;                /* build the link */
           cur->dep_nodep = node;
           if (Depend_list == NULL)
              Depend_list = cur;
           else
              prev->dep_link = cur;
      }

      /* now go through the list again and attach the precursors */
      cur = Depend_list;
      while (cur != NULL)
      {     curpre = precurs;
            while (*curpre != NULL)
            {    length = strcspn (curpre, " \0");    /* find the len of the precur */
                 name = (char*) malloc (length+1);
                 strncpy (name, curpre, length);      /* copy the name into name */
                 *(name+length) = NULL;               /* put a null on the end */
                 if (*(curpre + length) == NULL)
                    length--;
                 curpre += length+1;                  /* delete this one from the line */
                 node = SymAdd (name, SFILE);         /* add it to the symbol table */
                 temp = cur->dep_nodep->prelist;
                 while (temp != NULL)                  /* get to end of linked list */
                 {   prev = temp;
                     temp = temp->dep_link;
                 }
                 temp = (DEPLISTPTR) malloc (sizeof (DEPLIST));         /* allocate a link */
                 temp->dep_link = NULL;                /* build the link */
                 temp->dep_nodep = node;
                 if (cur->dep_nodep->prelist == NULL)
                      cur->dep_nodep->prelist = temp;
                 else
                      prev->dep_link = temp;
                 free (name);
            }
            cur = cur->dep_link;
      }
      return (Depend_list);
}


/*==========================================================================*/
/*                            GrCmnd                                        */
/*                                                                          */
/*  DISCRIPTION                                                             */
/*  ARGUMENTS                                                               */
/*  RETURNS                                                                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void GrCmnd (line, list)
char *line;
DEPLISTPTR list;
{    DEPLISTPTR curdep;
     CMNDLISTPTR curcmnd, prevcmnd;

     line++;
     curdep = list;
     while (curdep != NULL)
     {    curcmnd = curdep->dep_nodep->cmndlist;
          while (curcmnd != NULL)
          {   prevcmnd = curcmnd;
              curcmnd = curcmnd->cmnd_link;
          }
          curcmnd = (CMNDLISTPTR) malloc (sizeof (CMNDLIST));
          curcmnd->cmnd_link = NULL;
          curcmnd->cmnd = (char*) malloc (strlen (line)+1);
          strcpy (curcmnd->cmnd, line);
          if (curdep->dep_nodep->cmndlist == NULL)
               curdep->dep_nodep->cmndlist = curcmnd;
          else
               prevcmnd->cmnd_link = curcmnd;
          curdep = curdep->dep_link;
     }
}


