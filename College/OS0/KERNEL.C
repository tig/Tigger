 /*           kernel.c - Main kernel routines for OS/0

          (c) Copyright 1990, Charles E. Kindel, Jr. and Sandra Smith

VERSION   2.0
DATE      3/24/90

DESCRIPTION
   This file contains one part of the kernel routines for OS/0.
   The functions included in this file are:
   
        startup
        finish
        fork
        join
        quit
        new_pid
        new_stack
        free_pid
        free_stack

IMPLEMENTATION
   ANSI C (gcc).  Written on a Sequent under dynix.

USAGE
   Each function is documented with usage notes.

REVISION HISTORY
   DATE      VERSION       CHANGE
   2/16/90    0.90         Initial version.
   2/25/90    1.00         phase1 complete 
   3/24/90    2.0          phase2

BUGS
   No known bugs.

*/

#include <stdio.h>
#include "typedefs.h"

typedef int (*PFI) () ;
/* - GLOBAL DATA ---------------------------- */
/* process table  */
PD    process_table [MAXPROC];
PDPTR pdCurrent ;

/* process control queues */
QNODE    waiting ;
QNODE    ready ;
QNODE    zombie ;

/* Error identifier */
int Err ;

extern int semaphore [NUM_INT][MAX_UNIT] ;

/* - STARTUP ROUTINE ------------------------ */
/*
*   void startup (void)
*
*   PURPOSE:  Called by the usloss kernel at startup.  Initialize all
*             inturrupt vectors, then start first process.
*
*   USAGE:    We never call this function, it is called by usloss.
*
*   NOTES:    Interrupts are disabled when startup () is called, thus we can
*             initialize our interrupt vector without worrying about
*             inturrupts.
*             We also need to set all pid's to -1 and initialize our queues.
*/
void startup (void)
{
   int i ;

   tprint ("OS/0 - Copyright (c) 1990, Charles E. Kindel, Jr. and Sandra L. Smith\n\n") ;
   tprint ("OS/0 Kernel Running...\n") ;
   initqu(&ready) ;
   initqu(&waiting) ;
   initqu(&zombie) ;

   for (i = 0 ; i < MAXPROC ; i++) { 
       process_table[i].pid = -1 ;
       process_table[i].status = DEAD ;
   }
   int_vec[CLOCK_DEV] = (PFI) clock_handler ;
   int_vec[ALARM_DEV] = (PFI) alarm_handler ;
   int_vec[SYSCALL]   = (PFI) syscall_handler ;
   int_vec[TERM_DEV]  = (PFI) terminal_handler ;
   int_vec[DISK_DEV]  = (PFI) disk_handler ; 

   semaphore [CLOCK_DEV][0] = -3 ;
   int_enable () ;
   fork (&userstartup, (8192*2), 2) ;
}
/* end of startup */

/* - FINISH ROUTINE ------------------------- */
/*
*   void finish (void)
*
*   PURPOSE:  finish () is called by halt() which resides within usloss.
*             It's purpose is to allow us to shut down the kernel.  
*             There is a global variable, Err, which is set to an error
*             code before halt is called.  finish () uses this error code
*             to display a shutdown message.
*
*             finish () also prints the CPU time that has elapsed since
*             the kernel began.
*
*   USAGE:    We never call this function, it is called by usloss.
*
*   NOTES:    typedefs.h contains identifiers for all of the errors in the 
*             static array errorstr.  Each of these identifiers is prefixed
*             with ER_. 
*/
void  finish ()     /* quesiton?  Does Halt() call finish with a parameter? */
{
   char szText [80] ;
   static char *errorstr [] =
   {
      "Normal OS/0 termination",
      "Attempt to create more than MAXPROC processes",
      "Attempt to V an invalid semaphore",
      "Invalid priority",
      "Deadlock discovered",
      "Unable to allocate stack space" ,
      "Join with no children",
      "Delay System Call not yet implemented",
      "Quit called with invalid pid",
      "End of the world error in join",
      "Quit called on process with children",  
      "Quit called on a process that was not alive",
      "Calling a kernel routine while in user mode",
      "unknown error" 
   } ;
   if (Err == 0)
     {
      sprintf (szText, "%s.\n", errorstr[0]) ;
      tprint (szText) ;
    }
   else
      tprint ("Fatal Error (%d): %s.\n", Err, errorstr [Err]) ;
   trace ("\n") ;
}
/* end of finish */

/* - get_pid ROUTINE ------------------------- */
int get_pid (void) 
{
    return pdCurrent->pid ;
}
  
/* - tsXXp1 ROUTINE ------------------------- */
/* 
*   void tsXXp1 (void) 
*
*   PURPOSE:  A 'wrap-around' procedure for any any process created by 
*             fork.
*   USAGE:    when a new process's state is initialized this is
*             the function called at that time. 
*     
*   NOTES:    although we do call this procedure when initializing the
*             state of a process, we have actual access to the original
*             process fork was called with through the process 
*             descriptor.
*/
void tsXXp1 (void)
{
   int_enable () ;
   (*pdCurrent->f)();
   quit (0, 0) ;
}
/* end of tsXXp1 */
 
/* - FORK ROUTINE --------------------------- */
/*
*   int fork (void(*proc)(), int stacksize, int priority)
*
*   PURPOSE:  Create a new process. 
*
*   USAGE:    fork is called with a pointer to the function that is to be
*             created as a process, an integer indicating the amount of
*             memory that is to be allocated for this process' stack in
*             bytes, and the new processes priority level.  
*             fork () returns an integer representing the process id number
*             (pid) of the created process.
*
*   NOTES:    Priority levels range from 1 to 5 where 1 is the highest
*             priority.  fork () traps the following errors:
*              - Invalid priority (ER_INVALID_PRI) 
*             Currently we do not worry about the address space that
*             the new process is going to be using.  We assume that
*             all questions regarding this will be answered in a later
*             phase of the project.
*/
int  fork (void(*proc)(), int stacksize, int priority)
{
   PDPTR pd ;
   int pid ;
   int flag ;


   flag = begincs () ;

   pd = new_pd (&pid) ;
   if (priority > MIN_PRIORITY || priority < MAX_PRIORITY)
   {  
      Err = ER_INVALID_PRI ;
      halt () ;
   }

   *pd = *pdCurrent ;
   pd->f =  proc ;
   pd->pid = pid ;
   init_state (&pd->state, new_stack(stacksize, &pd->bot_stack), &tsXXp1) ;
   pd->stack = pd->state.sp ;
   pd->status = READY ;
   pd->nDone = 0 ;
   pd->nChildren = 0 ;
   pd->priority = priority ; 
   pd->key = priority ;
   ordinsqu (&ready, (QNODEPTR) pd) ;

   if ( pdCurrent != NULL ) 
   {
      pd->pdParent = pdCurrent ;
      pdCurrent->nChildren++ ;
      if (pdCurrent->priority > pd->priority)
      {
         pdCurrent->status = READY ;
         pdCurrent->key = pdCurrent->priority ;
         ordinsqu (&ready, (QNODEPTR) pdCurrent) ;
      }
   }
 
   if (pdCurrent == NULL || pdCurrent->status == READY)
      scheduler () ;

   endcs (flag) ;

   return pd->pid ;
}
/* end of fork */

/* - new_pd () ------ */
/*
*   PDPTR new_pd (int *pid)   
*
*   PURPOSE:  Create a new process descriptor for a new process.  This
*             function is called by fork ().   
*
*   USAGE:    pid is a pointer to a process id number.  new_pd returns a
*             pointer to a valid process descriptor (PDPTR) and the 
*             process id number associated with that PD (pid). 
*
*   NOTES:    new_pd get's it's process descriptors from an array of process
*             descriptors.  It finds a valid (unused) PD by doing a linear 
*             search down the array looking for PD's marked as DEAD.  If
*             there are no unused process descriptors it means we've allocated
*             the maximum number of processes (MAXPROC) and halt is called with
*             Err set to ER_TOOMANYPROCS.
*/
PDPTR new_pd (int *pid)
{
   int i ;

   for (i = 0 ; i < MAXPROC && process_table[i].status != DEAD ; i++)
      ;
   if (i >= MAXPROC)
   {
      Err = ER_TOOMANYPROCS ;
      halt () ;
   }
   *pid = i ;
   return (PDPTR) &process_table[i] ;
}

/* - new_stack (stacksize, bot_stack) ----- */
/*
*   int new_stack (int stacksize)   
*
*   PURPOSE:  Create a new stack for a new process.  This
*             function is called by fork ().   
*
*   USAGE:    stacksize is the size of the stack to be allocated
*             in bytes.  new_stack () returns a pointer to the
*             top of the stack.  
*             base of stack is returned trough bot_stack ;
*
*   NOTES:    new_stack uses malloc to get the stack space from
*             Unix.  It is concievable that once we have implemented
*             memory management into OS/0 we will use our own command
*             to do this.  In any case if the memory allocation fails
*             new_stack will call halt () with Err set to ER_STACK. 
*/
int new_stack (int stacksize, int * bot_stack)
{
   char *p ;

   if (NULL == (p = (char *)malloc (stacksize + 1)))
   {
      Err = ER_STACK ;
      halt () ;
   }
   *bot_stack = (int) p ;

   return (int) (p + stacksize - 1) ;
}
/* end of new_stack */

/* - JOIN ROUTINE --------------------------- */
/*
*   int join (int *status)   
*
*   PURPOSE:  Join a parent process with a child process. 
*             This function may be called by user processes.
*
*   USAGE:    Join returns the pid of a child process that has quit.
*             join's only parameter, a pointer to an integer, join returns
*             the status of a child process that has quit.  
*
*   NOTES:    Join always returns the status and pid of the most recently
*             quit child process.  If the process calling join's children
*             are all still running (i.e. they have not quit), join will
*             place the parent on the waiting queue with a status of CHILD_WAIT
*             If the parent has no children halt () will be called with Err
*             set to ER_NOCHILDREN.
*/
int join (int *status) 
{
   PDPTR pd ;
   int   pid ;
   int flag ;

   check_mode () ;
   flag = begincs () ;

   if (pdCurrent->nChildren == 0)
   {
      Err = ER_NOCHILDREN ;
      halt () ;
   }
   if (pdCurrent->nDone == 0)
   {
      pdCurrent->status = CHILD_WAIT ;
      pdCurrent->key = (int) pdCurrent ;
      ordinsqu (&waiting, (QNODEPTR) pdCurrent) ;
      scheduler () ;
   }
   if (NULL != (pd = (PDPTR) ordremqu (&zombie, (int) pdCurrent)))
   {
      *status = pd->termstatus ;
      pid = pd->pid ;
      free_pid (pd->pid) ;
      free_stack (pd->bot_stack) ;
      pdCurrent->nChildren-- ;
      pdCurrent->nDone--;
      endcs (flag) ; 
      return pid ;
   }

   Err = ER_JOIN ;
   halt () ;
   return NULL ;
}
/* end of join */

/*
*   void free_pid (int pid) 
*
*   PURPOSE:  Free up a process descriptor and pid so that it may be reused.
*
*   USAGE:    free_pid is called with the pid of the process descriptor that
*             is to be freed.
*
*   NOTES:    free_pid sets the unused process descriptor's pid to -1 and it's
*             status to DEAD. 
*/
void free_pid (int pid)
{
   process_table[pid].pid = -1 ;
   process_table[pid].status = DEAD ;
}
/* end of free_pid */

/*
*   void free_stack (int stack) 
*
*   PURPOSE:  Frees up the stack space used by a process that has quit. 
*
*   USAGE:    free_stack is called with a pointer to the beginning of the
*             stackspace that is to be freed.
*
*   NOTES:    free_stack uses the standard C free () routine to free up the 
*             space that was allocated by malloc within new_stack().  It is 
*             concievable that we will use our own memory management routines
*             in a later phase of OS/0.
*/
void free_stack (int stack)
{
    free ((char*) stack) ;
}
/* end of free_stack */

/* - QUIT ROUTINE ------------------------- */
/*
*   void quit (int pid, int status) 
*
*   PURPOSE:  quit is used to cause an existing process to quit so it
*             may join with it's parent.
*
*   USAGE:    Quit may be called in one of two ways.  In the first and most
*             common case quit will be called with pid == 0.  In this case
*             the currently running process will be quit.  In the second case
*             pid will be nonzero.  When pid is nonzero quit will quit the
*             process identified by that pid.  
*             In both cases quit will set the quitted process's termstatus to
*             the status quit was called with.  When join is called this  status
*             will be returned to the parent.
*
*   NOTES:    If all queues are empty when quit is called it means that
*             it is time for the kernel to shut down normally.   Quit will
*             do this by setting Err to ER_NORMAL and calling halt ().
*             If all queues are empty and quit was called with a nonzero
*             pid then we have an error, so halt will be called with Err
*             set to ER_QUIT.
*             We also have an ER_QUIT error if quit is called with a non
*             zero pid and the process descriptor associated with that pid
*             indicates the process is not alive.
*             Trying to quit a process that is on the zombie list will result
*             in halt() being called with Err set to ER_QUITDEAD.
*/
void quit (int pid, int status)
{
   int *sem ;
   int   i ;
   int flag ;
   PDPTR pd, parent ;

   check_mode () ;
   flag = begincs () ;
   /* check if pid is that of current process */
   if (pid == pdCurrent->pid)
      pid = 0;
   /* check for emptyqu on all queues */
   if (emptyqu (&waiting) && emptyqu (&ready) && emptyqu (&zombie))
   {
      if (pid != 0)
         Err = ER_QUIT ;
      else 
         Err = ER_NORMAL ;
      halt () ;
   }
   /* Check for process we want to terminate other than current */
   /* For example if quit is called with pid != 0 */
   if (pid != 0)
   {
      for (i = 0 ; process_table[i].pid != pid && i < MAXPROC ; i++)
	 ;
      if (i > MAXPROC)
      {  
	 Err = ER_QUIT ;
	 halt () ;       /* someone called quit on a pid without that pid existing! */
      }
      /* i now equals index into process_table and that pd is the one we want to quit */

      /* Check status to see if the process we are quitting is not DEAD or a ZOMBIE */
      if (process_table[i].status == DEAD || process_table[i].status == ZOMBIE)
      {
          Err = ER_QUITDEAD ;
          halt () ;
      }
      /* if waiting on a semaphore, increment its value */
      if ( (pd = (PDPTR) remqu ((QNODEPTR) &process_table[i]))  && 
           (pd->status == WAITING) )
      {
         (*((int *)pd->key))++ ;
      }
      /* end of case where pid != 0 */
   }
   else
       pd = pdCurrent ;

   parent = pd->pdParent ;                      
   
   /* At this point pd points to the process that is to be quit.        */
   /* parent points to it's parent                                      */
   /* pd is either the current process or a specific process determined */
   /* by the case above.                                                */
   if ( !pd->nChildren ) 
   {
      pd->termstatus = status ;
      parent->nDone++ ;
      if (parent->status == CHILD_WAIT)
      {
         parent = (PDPTR) ordremqu (&waiting, (int) parent) ;
         parent->status = READY ;
         parent->key = parent->priority ;
         ordinsqu (&ready, (QNODEPTR) parent) ;
      }
      pd->key = (int) parent ;
      pd->status = ZOMBIE ;
      ordinsqu (&zombie, (QNODEPTR) pd) ;
   }
   else  /* process to quit still has children */
   {
      Err = ER_QUITWCHILD ;
      halt () ;
   }
   if (pid == 0)
      scheduler () ;

   endcs (flag) ;
}
/* end of quit */

/*
*   void tprint (char *fmt, ...)    
*
*   PURPOSE:  Output formatted messages to the trace device.
*
*   USAGE:    tprint is called in the same manner that the standard C
*             printf function is called.  
*             tprint appends a newline to the end of each string.
*
*   NOTES:    Currently tprint does not correctly handle printf style
*             format strings.   
*/
void tprint (char *fmt, ...)
{
   int ival ;
   char *p, *sval ;
   double dval ;
   char cval ;
   char szText[132] ;
   va_list args ;

   va_start (args, fmt) ;
   szText [0] = '\0' ;
   for (p = fmt; *p ; p++)
   {
      if (*p != '%')
      {
         sprintf (szText, "%s%c", szText, *p) ;
         continue ;
      }
      switch (*++p)
      {
          case 'd' :
               ival = va_arg (args, int) ;
               sprintf (szText, "%s%d", szText, ival) ;
               break ;
     
          case 'x' :
                ival = va_arg (args, int) ;
                sprintf (szText, "%s%x", szText, ival) ;
                break ;

           case 'X' :
                ival = va_arg (args, int) ;
                sprintf (szText, "%s%X", szText, ival) ;
                break ;

          case 'c' :
               cval = va_arg (args, char) ;
               sprintf (szText, "%s%c", szText, cval) ;
               break ;

          case 'f' :
               dval = va_arg (args, double) ;
               sprintf (szText, "%s%f", szText, dval) ;
               break ;

          case 's' :
               sval = va_arg(args, char *) ; 
               sprintf (szText, "%s%s", szText, sval) ;
               break ;

          default:
               sprintf (szText, "%s%c", szText, *p) ;
               break ;
       }
   }
   va_end (args) ;
   trace (szText) ;
}
/* end of tprint */

/*
*   void cprint (char *fmt, ...)    
*
*   PURPOSE:  Output formatted messages to the console device.
*
*   USAGE:    cprint is called in the same manner that the standard C
*             printf function is called.  
*             cprint appends a newline to the end of each string.
*
*   NOTES:    Currently cprint does not correctly handle printf style
*             format strings.   
*/
void cprint (char *fmt, ...)
{
   int ival ;
   char *p, *sval ;
   char cval ;
   double dval ;
   char szText[132] ;
   va_list args ;

   va_start (args, fmt) ;
   szText [0] = '\0' ;
   for (p = fmt; *p ; p++)
   {
      if (*p != '%')
      {
         sprintf (szText, "%s%c", szText, *p) ;
         continue ;
      }
      switch (*++p)
      {
          case 'd' :
               ival = va_arg (args, int) ;
               sprintf (szText, "%s%d", szText, ival) ;
               break ;

          case 'x' :
                ival = va_arg (args, int) ;
                sprintf (szText, "%s%x", szText, ival) ;
                break ;

           case 'X' :
                ival = va_arg (args, int) ;
                sprintf (szText, "%s%X", szText, ival) ;
                break ;

          case 'c' :
               cval = va_arg (args, char) ;
               sprintf (szText, "%s%c", szText, cval) ;
               break ;

          case 'f' :
               dval = va_arg (args, double) ;
               sprintf (szText, "%s%f", szText, dval) ;
               break ;

          case 's' :
               sval = va_arg(args, char *) ; 
               sprintf (szText, "%s%s", szText, sval) ;
               break ;

          default:
               sprintf (szText, "%s%c", szText, *p) ;
               break ;
       }
   }
   va_end (args) ;
   console (szText) ;
}



/* end of file */
