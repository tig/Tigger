#include "include/typedef.h"
#include "include/input.h"
#include "include/update.h"
#include "include/make.h"
#include <stdio.h>


void UpDate (node)
GRNODEPTR node;
{    DEPLISTPTR curpre;
     CMNDLISTPTR curcmnd;

     node->status = ACTIVE;
     curpre = node->prelist;
     while (curpre != NULL)
     {   if (curpre->dep_nodep->status == INACTIVE)
             UpDate (curpre->dep_nodep);
         curpre = curpre->dep_link;
     }
     node->time = xmodtime(node->nodename);

     curpre = node->prelist;
     while (curpre != NULL)
     {   if ((curpre->dep_nodep->status != ACTIVE) && 
             (curpre->dep_nodep->time > node->time))
                 node->outdate = TRUE;
         curpre = curpre->dep_link;
     }

     if ((Mktouchf) && (node->outdate))
     {   XSetTime (node->nodename);
         printf ("[%s] has been touched.\n", node->nodename);
         node->time  = xmodtime (node->nodename);
     }
     else
     {     if ((node->outdate == TRUE) || 
               (node->time == 0)       || 
               (node->prelist == NULL) ||
               (Mkupdatef))
           {   curcmnd = node->cmndlist;
               while (curcmnd != NULL)
               {    printf ("%s\n", curcmnd->cmnd);
                    if (!Mkofflinef)
                       xexec (curcmnd->cmnd);
                    else
                       node->time = MkCurTime;
                    curcmnd = curcmnd->cmnd_link;
               }
               if (!Mkofflinef)
                   node->time = xmodtime (node->nodename);
            }
      }

      node->status = PROCESSED;
    
      if ((node->time == 0)       && 
          (node->prelist == NULL) && 
          (node->cmndlist == NULL))
      {   InFileName = node->nodename;
          MkError (DONTKNOW_ERR);
          exit (FATAL);
      }
}


