
/***********************************************************************/
/*    sandy2.c - Sandy's Routines    
      (c) Copyright 1990, Charles E. Kindel, Jr. and
                          Sandra Smith 
*/

#include <string.h>
#include "typedefs.h"
#include "typedefs2.h"
#include "mbox.h"
#include "queue.h"
#include "term.h"
/***********************************************************************/


#define STK_SIZE 8192      /* constant stack size */

typedef int (*PFI) () ;    /* pointer to a function */

extern  PFI nNewProcess ;   /* pointer to Spawned process */

extern  int spawn ;         /* semaphore for mutual exclusion in Spawn */
extern  int delay ;         /* semaphore for */
extern  int Mutex ;         /* semaphore to protect children in process of
                               Spawning from their parent terminating them
                               before its completed */


int     TerminalNumber ;     /* used in creation of terminal handlers */

int     startup_pid ;        /* to keep a handle on userstartup's pid */

QNODE   User_proc ;          /* queue of user processes created by a 
                                call to spawn */   
QNODE   Delayed_proc ;       /* queue of processes blocked on a call to
                                delay */    
QNODE   Mail ;               /* list of mailboxes created by processes */
QNODE   Drequests ;          /* queue of scheduler requests */

/***********************************************************************/
/* void Delay_Handler (void)
 *
 * This routine is started up in userstartup and runs continuously
 * until userstartup terminates.  It handles any processes which
 * requested a delay and was inserted inot the delay queue.  A process
 * is woken up by sending a message to his private mailbox. 
 */
void Delay_Handler (void) 
{
  QNODE TempQ ;
  DNODEPTR temp ;
  char szMsg [] = "Wake up!" ;


  while (TRUE)
    {
      initqu (&TempQ) ;
      waitclock () ;

      P (&delay) ;
      while (NULL != (temp = (DNODEPTR) remqu (Delayed_proc.next))) 
	{
	  if (--(temp->key) <= 0)
	    {
	      /* this guy is done delaying */
	      mbSend (temp->mbox, szMsg) ;
	      free ((char*)temp) ;
	    }
	  else
	    {
	      /* put him on the temp queue */
	      insqu (&TempQ, (QNODEPTR) temp) ;
	    }
      }
      /* now Delayed_proc should be empty */
      /* Splice in tempQ */
      while (temp = (DNODEPTR) remqu (TempQ.next))
	insqu (&Delayed_proc, (QNODEPTR) temp) ;

      V (&delay) ;
    }
}
	  
/***********************************************************************/
/* int Create_New_Process (int parent, int pid)
 *
 * This routine simply sets up a node for a new process that is
 * being spawned.
 */
int Create_New_Process (int parent, int pid)
{
   TNODEPTR tmp ;
   int success = 1 ;

   if (tmp = (TNODEPTR) malloc (sizeof(TNODE)))
   { 
      /* initialize all fields in the TNODE */
      tmp->key = pid ;
      tmp->parent = parent ;
      tmp->done = 0 ;
	
      /* create a private mailbox for use in delay and disk operations */
      tmp->dbox = mbCreate (1, 50) ;

      /* initialize the queues associated with the new process */
      initqu (&tmp->children ) ;
      initqu (&tmp->mailboxes ) ;

      /* put the new node onto the User_proc queue */
      ordinsqu (&User_proc, (QNODEPTR) tmp ) ;
   }
   else
      success =  0 ;

   return success ;
}

/***********************************************************************/
/* int Update_Parent_Process (int parent, int child) 
 *
 * This routine updates the parent node when a new child is created.
 */
int Update_Parent_Process (int parent, int child)
{
   TNODEPTR tmp ;
   QNODEPTR qn ;
   int success = 1 ;

   if (tmp = (TNODEPTR) ordremqu (&User_proc, parent))
   {
      if ( qn = (QNODEPTR) malloc (sizeof(QNODE)) )
      {  
         qn->key = child ;
         ordinsqu ((QNODEPTR) &tmp->children, qn) ; 
         ordinsqu (&User_proc, (QNODEPTR) tmp ) ;
      }
      else
         success = 0 ;
   }
   else
      if (parent != startup_pid)
         success = 0 ; 

   return success ;
}

/***********************************************************************/
/* int Update_Process_Mbox (int proc, int mbox)
 *
 * This routine is responsible for updating the list of mailboxes
 * a process node maintains within itself, for use in quit.
 */
int Update_Process_Mbox (int proc, int mbox)
{ 
    TNODEPTR tmp ;
    QNODEPTR qn ;
    int success = 1 ;

    if ( tmp = (TNODEPTR) ordremqu (&User_proc, proc))
    {
       if (qn = (QNODEPTR) malloc (sizeof(QNODE)) )
       {
          qn->key = mbox ;
          ordinsqu ((QNODEPTR) &tmp->mailboxes, qn) ;
       }
       else
          success = 0 ;
       ordinsqu (&User_proc, (QNODEPTR) tmp) ;
    }
    else 
       if (proc != startup_pid )
          success = 0;

    return success ; 
  
}

/***********************************************************************/
/* void terminate (void)
 *
 */
void terminate (void)
{
   QNODEPTR child ;
   TNODEPTR t1, t2 ;
   int parent = get_pid () ;
   int stat ;

   if (t1 = (TNODEPTR) ordremqu (&User_proc, parent) )
   {
      /* for each child process belonging to the current process
         we deallocate any resources, then quit that process and
         do a corresponding join operation.
       */
      P (&Mutex) ; 
      while (child = remqu ((QNODEPTR) t1->children.next) )
      {
         /* remove the child from the user's process list */
         if (t2 = (TNODEPTR) ordremqu (&User_proc, child->key))
         {
            /* check to see if process is not already dead */
            if (! t2->done )
            { 
               mbDestroy (t2->dbox) ;
               clean_up_mb ((QNODEPTR) &t2->mailboxes) ;
               quit ((int) t2->key, 0) ;
            }
            join (&stat) ; 
            free ((char*) t2) ;
         }
         free ((char*) child) ;
      }
      if ( !t1->done )
      {
         t1->done = 1 ;
         /* deallocate any resources belonging to the current process */
         mbDestroy (t1->dbox) ;
         clean_up_mb ((QNODEPTR) &t1->mailboxes) ;
         /* insert process back in process list until his parent makes
            a call to terminate - insures all joins are made */
         ordinsqu(&User_proc, (QNODEPTR) t1) ; 
         V (&Mutex) ;
         quit (0,0) ;
      }
   }  
}
 
/***********************************************************************/
/* void clean_up_mb (QNODEPTR q)
 *
 * This routine goes through the given list of mailboxes and calls
 * mbDestroy for each one.
 */
void clean_up_mb (QNODEPTR q)
{
   QNODEPTR tmp_mb ;
   QNODEPTR mb ;
   while ( mb = remqu (q->next) )
   {
      tmp_mb = ordremqu (&Mail, mb->key) ;
      mbDestroy ((MBPTR) mb->key) ;
      free ((char *)tmp_mb) ;
      free ((char *)mb) ; 
   }
   return ;
} 

/***********************************************************************/
/* void ts2XXp1 (void)
 *
 * This is the "wrap-around" process used by Spawn.
 */
void ts2XXp1 (void)
{
   PFI foo = nNewProcess ;
  
   V (&spawn) ; 
   V (&Mutex) ;
   
   user_mode () ; 
   foo() ; 
 
   Terminate () ;
} 

/***********************************************************************/
/* void userstartup (void)
 *
 * This routine is used to start up the user level system.  It is 
 * responsible for forking all the device server routines, initializing
 * all necessary queue structures and insuring all user-level support
 * processes terminate before the final quit.
 */
void userstartup (void)
{
   int i, x ;
   int term_pid [MAX_UNIT];
   int delay_pid ;
   int disk_sched_pid ;
   int disk_serv_pid ;

   tprint ("OS/0 User Support Level Running...\n") ;
   /* delay handler is first process forked so it can 'V' the 
      clock semaphore in time */
   delay_pid = fork (&Delay_Handler, 8024, 1) ;

   /* all queues required are initialized */
   initqu (&User_proc) ;
   initqu (&Delayed_proc) ;
   initqu (&Mail) ;
   initqu (&Drequests) ;
  
   /* the disk processes are forked */ 
   disk_serv_pid = fork (&Disk_Server, 8192*2, 1) ;
   disk_sched_pid = fork (&Disk_Scheduler, 8192*2, 1) ;

   /* userstartup's pid is recorded */
   startup_pid = get_pid() ;

   /* all the terminal processes are forked */
   for (i = 0 ; i < MAX_UNIT ; i++)
   {
      TerminalNumber = i ;
      term_pid [i] = fork (&TerminalInputHandler, 8024, 1) ;
   }

   /* the test routine is created by OurSpawn */
   OurSpawn ((int) Uboot, 8129*2, 4) ; 
   /* wait until the test routine finishes and then shut-down */
   join (&x) ;
  
   /* terminate and join with the terminal processes */ 
   for ( i = 0; i < MAX_UNIT ; i ++)
   {  
      quit (term_pid[i], 0) ;
      join (&x) ;
   }

   /* quit the delay routine */
   quit (delay_pid, 0) ; 
   join (&x) ;

   /* quit the disk routines */
   quit (disk_sched_pid, 0) ;
   join (&x) ;
   quit (disk_serv_pid, 0) ;
   join (&x) ;

   /* quit userstartup */
   quit (0,0) ;
}

/***********************************************************************/
