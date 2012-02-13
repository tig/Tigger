/* sched2.c

                sched2.c  (c) KindelCo Software Systems, 1988


                  a scheduler for a time-share system
                  using a heap for Priority Queues.


Author:  Charles E. Kindel, Jr.
         S24498449

Version: 2.2

DESCRIPTION
     This is the Main file for SCEDULER, a scheduler for a time-shared
     computing system.  Each process is identified by its PROCESS ID (pid),
     which is a unique small positive integer.  Each process has an
     associated priority pri.
          .
          .
          .
          .

INPUT
     Input command lines will assume the following form, where brackets
     indicate optional arguments:

         command  [ pid [ pri ] ]

     A command and its arguments are separated by blanks or tabs; a command
     and all its arguments are on the same line.  Process ids (pid) are
     integers in the range 1..64.  The second, argument, if present is an
     integer, possibly negative.  Possible commands are clock, sleep, wake,
     kill, arrive, and depart, which have arguments and interpretations
     described above.  Two other commands are, quit, which terminates the
     program, and snapshot, which prints the status of the data structures.

OUPUT
     The output from this program is produced by the command "snapshot".
     When the snapshot command is issued, the data structures used in
     the program will be printed out.

MESSAGES
     Various messages will be output on error conditions, such as bad
     input, kill_piding a non-existing process, creating an already used
     pid, etc...

IMPLEMENTATION
     The priority queue in this program was implemented using a heap
     implemented with an array.
     There are two priority queues: readyQ and blockedQ.
     An array containing the status of each alive process and the
     location of the corresponding process is used to keep track of
     the status' of the processes.

FACILITY
     MS-DOS:  Borland International TurboC 1.5.

     VAX VMS: Digital Equipment Corp. VAX C.

REVISIONS
         DATE           VERSION     CHANGE
      16 Feb 1988         1.0       Original
      18 Feb 1988         1.1       Data structures, Primitives work.
      23 Feb 1988         1.2       Found original was not abstract enough,
                                    rewrote primitives.
      24 Feb 1988         1.3       Beta Test version.
      25 Feb 1988         1.5       Ported to VAX/VMS.
      26 Mar 1988         2.0       Changed Implementation of ADT to
                                    Heap
      28 Mar 1988         2.1       Beta Test vesion with heap ADT.
      02 Apr 1988         2.2       Ported to VAX/VMS.

*/

/* include files */
#include <stdio.h>
#include <ctype.h>
#include <string.h>


/* Constants */
#define    void
#define    MAXPRI     32767      /* Maximum Priority number possible */
#define    ERROR      -1         /* All error's are denoted by negative numbers */
#define    NOTFOUND   -2
#define    USED       -3
#define    NOTREADY   -4
#define    NOTBLOCK   -5
#define    NORUN      -6
#define    BADPID     -7
#define    EMPTY      -8
#define    BADCMD     -9
#define    OK         0           /* This is an OK result */
#define    RUNNING    1           /* Status' for the lists, etc.. */
#define    BLOCKED    2
#define    READY      3
#define    UNUSED     0
#define    MIN_PID    1
#define    MAX_PID    64          /* Min and maximum PID numbers */
#define    QUANTUM    10          /* Maximum running time for a process */
#define    CLOCK      0           /* Each command has a unique number that */
#define    SLEEP      1           /* represents it.  */
#define    DEPART     2
#define    QUIT       3
#define    SNAPSHOT   4
#define    WAKE       5
#define    KILL       6
#define    ARRIVE     7
#define    TRUE       1
#define    FALSE      0

/* Type Declarations */

typedef short status_type;             /* status_type is used for all result codes, */
                                       /* and for element status (RUNNING, BLOCKED, etc... */
typedef short pid_type;                /* Process ID numbers */
typedef int pri_type;                  /* Priority numbers */

typedef struct element_type
               {    pid_type pid;      /* each element has a pid */
                    pri_type pri;      /* each element has a priority */
               } element_rec, *element_p;

typedef struct heap_type               /* nodes in the Priority Queues */
               {    element_rec element[(MAX_PID+1)*2];
                    short num;
               } heap_rec, *priority_q;

typedef struct name_type               /* this structure is for the list pointing to */
               {    short index;       /* each element wheter it is running, blocked or ready */
                    status_type status;
               } name_rec;

typedef name_rec name_array[MAX_PID+1];
typedef short command_type;
typedef short bool;


/* functions */
void         arrive ();
void         clock_tick ();
void         kill_pid ();
void         sleep_pid ();
command_type getcommand ();
void         initialize ();
void         swap ();
void         snapshot ();
void         snooze ();
void         kill_pid ();
status_type  insert ();
void         siftup ();
status_type  deleteany ();
status_type  deletemin ();
void         makenull ();
void         error ();


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Main Program                                                            */
/*-------------------------------------------------------------------------*/
main (void)
{    priority_q   readyQ,
                  blockedQ;
     name_array   names;           /* an array of records containing apointer to an element and */
                                   /* that element's status */
     command_type command;
     element_p    running, work;

     /* Allocate data structures */
     running  = (element_p) malloc (sizeof (element_rec));
     work     = (element_p) malloc (sizeof (element_rec));
     readyQ   = (priority_q) (malloc (sizeof (heap_rec)));
     blockedQ = (priority_q) (malloc (sizeof (heap_rec)));

     /* initialize data structures */
     work->pri = UNUSED;
     work->pid = UNUSED;
     initialize (readyQ, blockedQ, running, names);

     /* main loop */
     while ((command = getcommand (work)) != QUIT)
     {  if ((work->pid < UNUSED) || (work->pid > MAX_PID) ||
            ((work->pid == UNUSED) && (command > SNAPSHOT)))     /* this takes care of pid == 0 */
           error (BADPID);
        else
        switch (command)
        {  case ARRIVE   : arrive (work, running, readyQ, names);
                           break;
           case CLOCK    : clock_tick (running, readyQ, names);
                           break;
           case KILL     : kill_pid (work, running, readyQ, blockedQ, names);
                           break;
           case SLEEP    : sleep_pid (work, running, readyQ, blockedQ, names);
                           break;
           case WAKE     : wake (work, running, readyQ, blockedQ, names);
                           break;
           case DEPART   : depart (running, readyQ, names);
                           break;
           case SNAPSHOT : snapshot (running, readyQ, blockedQ, names);
                           break;
           default       : error (BADCMD);
        }
        work->pid = UNUSED;
        work->pri = UNUSED;
     } /* end of while */
     return;
} /* End of main */




/*=======================================================================*/
/*  Basic Primitives                                                     */
/*     The following routines handle the abstraction of the data         */
/*     structure.  If a new implementation of data structure is required */
/*     changing the following routines should be all that is neccissary. */
/*-----------------------------------------------------------------------*/
/* make_null                                                             */
/*     Used to initialize the priorityQ before first usage.              */
/*     Creates the list header.                                          */
/*                                                                       */
/*     PARAMETERS                                                        */
/*         PQ - pointer to priority queue data structure.                */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void make_null (PQ)
priority_q PQ;
{    PQ->num = 0;
}  /* end of make_nul */




/*-------------------------------------------------------------------------*/
/* insert                                                                  */
/*    Inserts an element into a priorityQ.  The element is inserted to     */
/*    the right of the list header.  If there is no memory left this       */
/*    function returns ERROR (-1) to the caller, else it returns OK (0)    */
/*    The element itself is passed.                                        */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        e - pointer to the element to be inserted.                       */
/*        e_status - value indicating what the status of the new element   */
/*                   should be (RUNNING,READY,BLOCKED, UNUSED...)          */
/*        PQ- pointer to the priority Queue structure.                     */
/*        names - pointer to the array of element status' and indicies.    */
/*                                                                         */
/*    RETURNS Because of the ATD used (heap), if this routine is called    */
/*            it will work, so it always returns OK.                       */
/*            PQ and names are modified.                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
status_type insert (e, e_status, PQ, names)
element_p e;
status_type e_status;
priority_q PQ;
name_array names;
{  status_type err_status = OK;
   int i, j;
   bool  flag;


   PQ->num++;
   j = PQ->num;
   flag = TRUE;
   while (flag && (j > 1))
   {   i = j >> 1;
       if (PQ->element[i].pri < e->pri)
         flag = FALSE;
       else
       {   names[PQ->element[i].pid].index = j;
           PQ->element[j] = PQ->element[i];
           j = i;
       }
    }
    names[e->pid].index = j;
    names[e->pid].status = e_status;
    PQ->element[j] = *e;

    return (err_status);
}  /* end of insert */




/*-------------------------------------------------------------------------*/
/* deletemin                                                               */
/*    Finds the element s of smallest priority in the PriorityQ and        */
/*    returns the pointer to s through the parameter list, after deleting  */
/*    it from the Q.                                                       */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        e - pointer to the element to be inserted.                       */
/*        e_status - value indicating what the status of the new element   */
/*                   should be (RUNNING,READY,BLOCKED, UNUSED...)          */
/*        PQ- pointer to the priority Queue structure.                     */
/*        names - pointer to the array of element status' and indicies.    */
/*                                                                         */
/*    RETURNS status indicating whether the delete was successful (OK) or  */
/*            the data structure was empty (EMPTY).                        */
/*            PQ and names are modified.                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
status_type deletemin (e, e_status, PQ, names)
element_p e;
status_type e_status;
priority_q PQ;
name_array names;
{  status_type err_status=OK;

   if (PQ->num >= 1)
   {  *e = PQ->element[1];
      names[PQ->element[PQ->num].pid].index = 1;
      PQ->element[1] = PQ->element[PQ->num];
      PQ->num--;
      siftup (PQ, 1, names);
      names[e->pid].index = UNUSED;
      names[e->pid].status = e_status;
   }
   else
      err_status = EMPTY;

   return (err_status);
}  /* end of deletemin */




/*-------------------------------------------------------------------------*/
/* siftup                                                                  */
/*    Re-heaps the heap by shifting a "bad" element up.                    */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        PQ - pointer to the priority Queue.                              */
/*        i - index of element to sift.                                    */
/*        names - array with status' and indicies.                         */
/*                                                                         */
/*    RETURNS                                                              */
/*            PQ and names are modified.                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void siftup (PQ, i, names)
priority_q PQ;
short i;
name_array names;
{   short j;
    element_rec temp;

    while (2 * i <= PQ->num)
    {   j = 2 * i;
        if (j < PQ->num)
           if (PQ->element[j].pri >= PQ->element[j+1].pri)
              j++;
        if (PQ->element[i].pri >= PQ->element[j].pri)
        {   temp = PQ->element[j];
            names[PQ->element[i].pid].index = j;
            names[PQ->element[j].pid].index = i;
            PQ->element[j] = PQ->element[i];
            PQ->element[i] = temp;
            i = j;
        }
        else
          i = PQ->num + 1;
    }
    return;
} /* end of siftup */




/*-------------------------------------------------------------------------*/
/* deleteany                                                               */
/*      Deletes any element e from the priorityQ.  Uses an array of names  */
/*      to quickly find the element to be deleted.                         */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        e - pointer to the element to be deleted.                        */
/*        e_status - value indicating what the status of the  element      */
/*                   should be (RUNNING,READY,BLOCKED, UNUSED...)          */
/*        PQ- pointer to the priority Queue structure.                     */
/*        names - pointer to the array of element status' and indicies.    */
/*                                                                         */
/*      RETURNS:  ERROR on empty Q.   NOTFOUND if pid isn't found in       */
/*                namelist.  OK if deletion was successful.                */
/*                names and PQ are modified.                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/
status_type deleteany (e, e_status, PQ, names)
element_p e;
status_type e_status;
priority_q PQ;
name_array names;
{  status_type err_status = OK;
   int i;

   if (names[e->pid].status == UNUSED)
     err_status = NOTFOUND;
   else
   {   i = names[e->pid].index;
       e->pri = PQ->element[i].pri;
       names[PQ->element[PQ->num].pid].index = i;
       names[e->pid].index = UNUSED;
       names[e->pid].status = e_status;
       PQ->element[i] = PQ->element[PQ->num];
       PQ->num--;
       siftup (PQ, i, names);
   }

   return (err_status);
}  /* end of deleteany */





/*-------------------------------------------------------------------------*/
/* initialize                                                              */
/*    initializes the following data structures:                           */
/*      - readyQ    : a priorityQ used to hold processes which are ready to*/
/*                    be made running.                                     */
/*      - blockedQ  : a Q used to hold processes which are blocked.        */
/*      - running   : the running element (a pid and a pri)                */
/*      - names     : the array of pointers pointing into the blockedQ and */
/*                    the readyQ to determine each pid's location and      */
/*                    status quickly.  deleteany uses it for quick         */
/*                    deletion.                                            */
/*-------------------------------------------------------------------------*/
void initialize (rQ, bQ, running, names)
priority_q rQ;
priority_q bQ;
element_p running;
name_array names;
{   int i;
    for (i = MIN_PID-1; i <= MAX_PID; i++)
    {   names[i].index = UNUSED;
        names[i].status = UNUSED;
    }
    make_null (rQ);
    make_null (bQ);
    running->pri = UNUSED;
    running->pid = UNUSED;

    return;
}
/* end of initialize */




/*-------------------------------------------------------------------------*/
/* snapshot                                                                */
/*-------------------------------------------------------------------------*/
void snapshot (running, readyQ, blockedQ, names)
element_p running;
priority_q readyQ;
priority_q blockedQ;
name_array names;
{    int i;

     printf ("\nstructure        pid      pri\n");
     printf ("-----------------------------\n");
     printf ("RUNNING\n");
     if (running->pid != UNUSED)
       printf ("                  %d       %d\n", running->pid, running->pri);
     else
       printf ("            No Running Process\n");
     printf ("\n\nBLOCKED\n");

     if (blockedQ->num > 0)
       for (i=1; i <= blockedQ->num; i++)
         printf ("                  %d       %d\n",
                blockedQ->element[i].pid, blockedQ->element[i].pri);
     else
       printf ("                  PQ is Empty\n");

     printf ("\n\nREADY\n");

     if (readyQ->num > 0)
     {  printf ("  first\n");
        for (i=1; i <= readyQ->num; i++)
          printf ("                  %d       %d\n",
                readyQ->element[i].pid, readyQ->element[i].pri);
        printf ("  last\n");

     }
     else
       printf ("                  PQ is Empty\n");

     printf ("\n\nUNUSED PIDS:\n");
     for (i = MIN_PID; i <= MAX_PID; i++)
       if (names[i].status == UNUSED)
         printf (" %d", i);

     printf ("\n");
}
/* end of snapshot */



/*=======================================================================*/
/*  Application Routines                                                 */
/*     These routines make up the application code.  The are blind to    */
/*     the implementation of the data structure.                         */
/*-----------------------------------------------------------------------*/
/* swap                                                                  */
/*    This routine is aclled on various occasions to reschedule the CPU; */
/*    it reinserts the running process into the readyQ and the does a    */
/*    deletemin on readyQ to find the process of smalles priority, which */
/*    is made to be running.                                             */
/*    If there is no running process then the next process is pulled off */
/*    of the readyQ.                                                     */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        running - pointer to the running element.                      */
/*        rQ - pointer to the ready Queue structure.                     */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, and readyQ through the function argument list.  */
/*                                                                       */
/*-----------------------------------------------------------------------*/

void swap (running, rQ, names)
element_p running;
priority_q  rQ;
name_array  names;
{
   if (running->pid == UNUSED)
     deletemin (running, RUNNING, rQ, names);
   else
   {  insert (running, READY, rQ, names);
      deletemin (running, RUNNING, rQ, names);
   }
} /* end of swap */




/*-----------------------------------------------------------------------*/
/* arrive                                                                */
/*    This routine handles the ARRIVE command.                           */
/*    If the PID arriving is not used and if there is no running PID,    */
/*    the arriving PID is made running.  If there is a running PID,      */
/*    then the arriving PID is inserted into the ready Q.                */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        work - pointer to the element that is arriving.                */
/*        running - pointer to the running element.                      */
/*        readyQ - pointer to the ready Queue structure.                 */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, and readyQ through the function argument list.  */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void arrive (work, running, readyQ, names)
element_p work, running;
priority_q readyQ;
name_array names;
{    if (names[work->pid].status == UNUSED)
        if (running->pid == UNUSED)
        {  error (insert (work, READY, readyQ, names));
           error (deletemin (running, RUNNING, readyQ, names));
        }
        else
        {  error (insert (work, READY, readyQ, names));
           swap (running, readyQ, names);
        }
     else
        error (USED);
     return;
} /* end of arrive */



/*-----------------------------------------------------------------------*/
/*  clock_tick                                                           */
/*     this routine simulates a tick of the system clock by adding       */
/*     quantum to the running process, then rescheduling.                */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        running - pointer to the running element.                      */
/*        readyQ - pointer to the ready Queue structure.                 */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, and readyQ through the function argument list.  */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void clock_tick (running, readyQ, names)
element_p running;
priority_q readyQ;
name_array names;
{   if (names[running->pid].status != UNUSED)
    {   running->pri += QUANTUM;
        swap (running, readyQ, names);
    }
    else
        error (NORUN);
    return;
} /* end of clock_tick */



/*-----------------------------------------------------------------------*/
/*  kill_pid                                                             */
/*    This routine kills a PID, whether it is running, blocked, or ready */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        work - pointer to the element to be killed.                    */
/*        running - pointer to the running element.                      */
/*        readyQ - pointer to the ready Queue structure.                 */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, blockedQ, readyQ, and names through the         */
/*    function argument list.                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void kill_pid (work, running, readyQ, blockedQ, names)
element_p work, running;
priority_q readyQ, blockedQ;
name_array names;
{   switch (names[work->pid].status)
    {  case RUNNING : names[running->pid].status = UNUSED;
                      running->pid = UNUSED;
                      running->pri = UNUSED;
                      deletemin (running, RUNNING, readyQ, names);
                      break;
       case READY   : deleteany (work, UNUSED, readyQ, names);
                      break;
       case BLOCKED : deleteany (work, UNUSED, blockedQ, names);
                      break;
       case UNUSED  : error (NOTFOUND);
                      break;
    }
    return;
} /* end of kill_pid */




/*-----------------------------------------------------------------------*/
/* sleep_pid                                                             */
/*   This routine puts a PID on the blocked Q, adding the specified      */
/*   priority to its current priority.                                   */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        work - pointer to the element to sleep.                        */
/*        running - pointer to the running element.                      */
/*        readyQ - pointer to the ready Queue structure.                 */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, blockedQ, readyQ, and names through the         */
/*    function argument list.                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void sleep_pid (work, running, readyQ, blockedQ, names)
element_p work, running;
priority_q readyQ, blockedQ;
name_array names;
{   if (running->pid != UNUSED)
    {  work->pri += running->pri;
       work->pid = running->pid;
       insert (work, BLOCKED, blockedQ, names);
       if ((deletemin (running, RUNNING, readyQ, names)) == EMPTY)
       {  running->pid = UNUSED;
          running->pri = UNUSED;
       }
    }
    else
      error (NORUN);
   return;
} /* end of sleep_pid */




/*-----------------------------------------------------------------------*/
/*  wake                                                                 */
/*     This routine takes a PID off of the blocked Q, inserts it into    */
/*     the ready Q and then reschedules the CPU.                         */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        work - pointer to the element to be awakened.                  */
/*        running - pointer to the running element.                      */
/*        readyQ - pointer to the ready Queue structure.                 */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, blockedQ, readyQ, and names through the         */
/*    function argument list.                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void wake (work, running, readyQ, blockedQ, names)
element_p work, running;
priority_q readyQ, blockedQ;
name_array names;
{   if (names[work->pid].status == BLOCKED)
    {  deleteany (work, UNUSED, blockedQ, names);
       insert (work, READY, readyQ, names);
       swap (running, readyQ, names);
    }
    else
       error (NOTBLOCK);
    return;
} /* end of wake */




/*-----------------------------------------------------------------------*/
/* depart                                                                */
/*    This routine takes the running PID and kills it, then it re-       */
/*    schedules the CPU.                                                 */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        running - pointer to the running element.                      */
/*        readyQ - pointer to the ready Queue structure.                 */
/*        names - pointer to the array of element status' and indicies.  */
/*                                                                       */
/*    RETURNS:  running, readyQ, and names through the                   */
/*    function argument list.                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void depart (running, readyQ, names)
element_p running;
priority_q readyQ;
name_array names;
{    if (names[running->pid].status == UNUSED)
        error (NORUN);
     else
     {  names[running->pid].status = UNUSED;
        running->pid = UNUSED;
        running->pri = UNUSED;
        deletemin (running, RUNNING, readyQ, names);
     }
     return;
} /* end of depart */





/*-----------------------------------------------------------------------*/
/* error                                                                 */
/*    This function handles all errors (I hope).                         */
/*                                                                       */
/*    PARAMETERS                                                         */
/*        err - value of error.                                          */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void error (err)
status_type err;
{  if (err != OK)
   {  printf ("ERROR: ");
      switch (err)
      {  case ERROR    : printf ("Unidentified error in last command.\n");
                         break;
         case NOTFOUND : printf ("Process not found.\n");
                         break;
         case NOTREADY : printf ("Process not in the ready queue.\n");
                         break;
         case NOTBLOCK : printf ("Process not on the blocked list.\n");
                         break;
         case NORUN    : printf ("No running process.\n");
                         break;
         case USED     : printf ("Process ID is already in use.\n");
                         break;
         case BADPID   : printf ("Process ID out of range.\n");
                         break;
         case BADCMD   : printf ("Error in command.\n");
                         break;
         default       : printf ("Wow, you got a realy weird error.\n");
      }
   }
} /* end of error */



/*-------------------------------------------------------------------------*/
/* getcommand                                                              */
/*                                                                         */
/*    PARAMETERS                                                           */
/*        get_element - pointer to the element that will be manipulated.   */
/*                                                                         */
/*    RETURNS                                                              */
/*        command                                                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
command_type getcommand (get_element)
element_p get_element;
{    char cmd[10];
     command_type command;
     int i;

     command = ERROR;  /* assume command is an error */

     scanf ("%s`", cmd);
     i = 0;
     while (cmd[i++] != NULL)
        cmd[i] = tolower (cmd[i]);

     if (!strcmp (cmd, "clock"))                 /* clock has no args */
        command = CLOCK;

     if (!strcmp (cmd, "sleep"))                 /* snooze needs a pri */
     {   scanf ("%d`", &get_element->pri);
         command = SLEEP;
     };

     if (!strcmp (cmd, "wake"))
     {   scanf ("%d`", &get_element->pid);
         command = WAKE;
     };

     if (!strcmp (cmd, "kill"))
     {   scanf ("%d`", &get_element->pid);
         command = KILL;
     };

     if (!strcmp (cmd, "arrive"))
     {   scanf ("%d %d`", &get_element->pid, &get_element->pri);
         command = ARRIVE;
     };

     if (!strcmp (cmd, "depart"))
         command = DEPART;

     if (!strcmp (cmd, "snapshot"))
         command = SNAPSHOT;

     if (!strcmp (cmd, "quit"))
         command = QUIT;


     /* Now echo command back out... */
     switch (command)
     {  case CLOCK   : printf ("clock\n");
                       break;
        case SLEEP   : printf ("sleep %d\n", get_element->pri);
                       break;
        case WAKE    : printf ("wake %d\n", get_element->pid);
                       break;
        case KILL    : printf ("kill %d\n", get_element->pid);
                       break;
        case ARRIVE  : printf ("arrive %d %d\n", get_element->pid, get_element->pri);
                       break;
        case DEPART  : printf ("depart\n");
                       break;
        case SNAPSHOT: printf ("snapshot\n");
                       break;
        case QUIT    : printf ("quit\n");
                       break;
        default      : printf ("%s <-- ", cmd);
     }
     return (command);
 }
 /* end of getcommand */


/* End of sched2.c ======================================================*/


