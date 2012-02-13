/* schedule.c

                schedule.c  (c) KindelCo Software Systems, 1988


                  schedule a scheduler for a time-share system


Author:  Charles E. Kindel, Jr.
         S24498449

Version: 1.5

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
     integers in the range 1..20.  The second, argument, if present is an
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
     The priority queue in this program was implemented using a doubly
     linked circular list with a list header.
     There are tow priority queues: readyQ and blockedQ.
     An array containing the status of each alive process and the
     location of the corresponding process is used to keep track of
     the status' of the processes.

FACILITY
     MS-DOS:  Borland International TurboC 1.0.

     VAX VMS: Digital Equipment Corp. VAX C.

REVISIONS
         DATE           VERSION     CHANGE
      16 Feb 1988         1.0       Original
      18 Feb 1988         1.1       Data structures, Primitives work.
      23 Feb 1988         1.2       Found original was not abstract enough,
                                    rewrote primitives.
      24 Feb 1988         1.3       Beta Test version.
      25 Feb 1988         1.5       Ported to VAX/VMS.

*/

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
#define    NEWLINE    \n
#define    MIN_PID    1
#define    MAX_PID    20          /* Min and maximum PID numbers */
#define    QUANTUM    10          /* Maximum running time for a process */
#define    CLOCK      0           /* Each command has a unique number that */
#define    SLEEP      1           /* represents it.  */
#define    DEPART     2
#define    QUIT       3
#define    SNAPSHOT   4
#define    WAKE       5
#define    KILL       6
#define    ARRIVE     7

/* Type Declarations */

typedef short status_type;             /* status_type is used for all result codes, */
                                       /* and for element status (RUNNING, BLOCKED, etc... */

typedef short pid_type;                /* Process ID numbers */

typedef int pri_type;                  /* Priority numbers */

typedef struct element_type
               {    pid_type pid;      /* each element has a pid */
                    pri_type pri;      /* each element has a priority */
               } element_rec, *element_p;

typedef struct node_type               /* nodes in the Priority Queues */
               {    element_rec element;
                    struct node_type *left;
                    struct node_type *right;
               } node_rec, *node_p;

typedef node_p priority_q;             /* used to point to the header */

typedef struct name_type               /* this structure is for the list pointing to */
               {    node_p node;       /* each element wheter it is running, blocked or ready */
                    status_type status;
               } name_rec, *name_p;

typedef name_rec name_array[MAX_PID+1];

typedef short command_type;


/* functions */
command_type getcommand ();
void         initialize ();
void         swap ();
void         snapshot ();
void         snooze ();
void         kill_pid ();
status_type  insert ();
status_type  deleteany ();
status_type  deletemin ();
void         makenull ();
void         error ();

/* Main Program */
main (void)
{
     priority_q   readyQ = NULL,   /* structure to hold the ready Queue */
                  blockedQ = NULL; /* strucutre to hold the blocked Queue */
     element_p    running =NULL,   /* pointer to the running element */
                  work = NULL;     /* pointer to a temp or working element */
     name_array   names;           /* an array of records containing apointer to an element and */
                                   /* that element's status */
     command_type command;

     readyQ = ((node_p) malloc (sizeof (node_rec)));
     blockedQ = ((node_p) malloc (sizeof (node_rec)));
     running = ((element_p) malloc (sizeof (element_rec)));

     work = ((element_p) malloc (sizeof (element_rec)));
     work->pid = UNUSED;
     work->pri = UNUSED;

     initialize (readyQ, blockedQ, running, names);

     while ((command = getcommand (work)) != QUIT)
     {  if ((work->pid < UNUSED) || (work->pid > MAX_PID) ||
            ((work->pid == UNUSED) && (command > SNAPSHOT)))     /* this takes care of pid == 0 */
           error (BADPID);
        else
        switch (command)
        {  case ARRIVE   : if (names[work->pid].status == UNUSED)
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
                           break;
           case CLOCK    : running->pri += QUANTUM;
                           swap (running, readyQ, names);
                           break;
           case KILL     : switch (names[work->pid].status)
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
                           break;
           case SLEEP    : if (running->pid != UNUSED)
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
                           break;
           case WAKE     : if (names[work->pid].status == BLOCKED)
                           {  deleteany (work, UNUSED, blockedQ, names);
                              insert (work, READY, readyQ, names);
                              swap (running, readyQ, names);
                           }
                           else
                              error (NOTBLOCK);
                           break;
           case DEPART   : if (names[running->pid].status == UNUSED)
                              error (NORUN);
                           else
                           {  names[running->pid].status = UNUSED;
                              running->pid = UNUSED;
                              running->pri = UNUSED;
                              deletemin (running, RUNNING, readyQ, names);
                           }
                           break;
           case SNAPSHOT : snapshot (running, readyQ, blockedQ, names);
                           break;
           default       : error (BADCMD);
        }
        work->pid = UNUSED;
        work->pri = UNUSED;
     }
     return;
}
/* End of main */




/*-----------------------------------------------------------------------*/
/* error                                                                 */
/*    This function handles all errors (I hope).                         */
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
}

/*-----------------------------------------------------------------------*/
/* swap                                                                  */
/*    This routine is aclled on various occasions to reschedule the CPU; */
/*    it reinserts the running process into the readyQ and the does a    */
/*    deletemin on readyQ to find the process of smalles priority, which */
/*    is made to be running.                                             */
/*    If there is no running process then the next process is pulled off */
/*    of the readyQ.                                                     */
/*    RETURNS:  running, and readyQ through the function argument list.  */
/*-----------------------------------------------------------------------*/

void swap (running, rQ, names)
element_p running;
priority_q rQ;
name_array names;
{
   if (running->pid == UNUSED)
     deletemin (running, RUNNING, rQ, names);
   else
   {  insert (running, READY, rQ, names);
      deletemin (running, RUNNING, rQ, names);
   }
}



/*-----------------------------------------------------------------------*/
/* make_null                                                             */
/*     Used to initialize the priorityQ before first usage.              */
/*     Creates the list header.                                          */
/*-----------------------------------------------------------------------*/

void make_null (PQ)
priority_q PQ;
{     PQ = ((priority_q) malloc (sizeof (node_rec)));
      PQ->left = PQ;
      PQ->right = PQ;
}
/* end of make_nul */




/*-------------------------------------------------------------------------*/
/* insert                                                                  */
/*    Inserts an element into a priorityQ.  The element is inserted to     */
/*    the right of the list header.  If there is no memory left this       */
/*    function returns ERROR (-1) to the caller, else it returns OK (0)    */
/*    The element itself is passed.                                        */
/*-------------------------------------------------------------------------*/

status_type insert (e, e_status, PQ, names)
element_p e;
status_type e_status;
priority_q PQ;
name_array names;
{
   node_p new_node = NULL;
   status_type err_status;

   if (names [e->pid].node == NULL)
   {  if ((new_node = ((node_p) malloc (sizeof (node_rec)))) != NULL)
      {  new_node->element.pid = e->pid;
         new_node->element.pri = e->pri;
         new_node->left = PQ;
         new_node->right = PQ->right;
         PQ->right->left = new_node;
         PQ->right = new_node;
         names [e->pid].node = new_node;
         names [e->pid].status = e_status;
         err_status = OK;
      }
      else
         err_status = ERROR;
   }
   else
     err_status = USED;

   return (err_status);
}
/* end of insert */




/*-------------------------------------------------------------------------*/
/* deletemin                                                               */
/*    Finds the element s of smallest priority in the PriorityQ and        */
/*    returns the pointer to s through the parameter list, after deleting  */
/*    it from the Q.                                                       */
/*    s must be allocated in the calling routine.                          */
/*    This function returns a status indicating whether it was able to     */
/*    deleteany an element or not:  ERROR means that the list was empty.   */
/*                               OK    means that deletion was successful  */
/*-------------------------------------------------------------------------*/

status_type deletemin (e, e_status, PQ, names)
element_p e;
status_type e_status;
priority_q PQ;
name_array names;
{  node_p   temp = NULL,
            min_e = NULL;
   pri_type min;
   status_type err_status;

   if (PQ->right != PQ)
   {  temp = PQ;
      min = MAXPRI;
      while ((temp = temp->right) != PQ)
        if (min > temp->element.pri)
        { min_e = temp;
          min = temp->element.pri;
        }
      e->pid = min_e->element.pid;
      e->pri = min_e->element.pri;
      err_status = deleteany (e, e_status, PQ, names);
   }
   else
     err_status = EMPTY;
   return (err_status);
}
/* end of deletemin */




/*-------------------------------------------------------------------------*/
/* deleteany                                                               */
/*      Deletes any element e from the priorityQ.  Uses an array of names  */
/*      to quickly find the element to be deleted.                         */
/*      RETURNS:  ERROR on empty Q.   NOTFOUND if pid isn't found in       */
/*                namelist.  OK if deletion was successful.                */
/*-------------------------------------------------------------------------*/

status_type deleteany (e, e_status, PQ, names)
element_p e;
status_type e_status;
priority_q PQ;
name_array names;
{  node_p temp = NULL;
   status_type err_status;

   if (PQ->right != PQ)
   {  if ((temp = names[e->pid].node) == NULL)
         err_status = NOTFOUND;
      else
      {  temp->left->right = temp->right;
         temp->right->left = temp->left;
         e->pid = temp->element.pid;
         e->pri = temp->element.pri;
         names [e->pid].node = NULL;
         names [e->pid].status = e_status;
         free (temp);
         err_status = OK;
      }
   }
   else
     err_status = EMPTY;

   return (err_status);
}
/* end of deleteany */




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
/*                    deletion.
/*-------------------------------------------------------------------------*/

void initialize (rQ, bQ, running, names)
priority_q rQ;
priority_q bQ;
element_p running;
name_array names;
{   int i;
    rQ->right = rQ;
    rQ->left = rQ;
    bQ->right = bQ;
    bQ->left = bQ;
    running->pid = UNUSED;
    running->pri = 0;
    for (i = MIN_PID; i <= MAX_PID; i++)
    {   names[i].node = NULL;
        names[i].status = UNUSED;
    }
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
{    priority_q temp = NULL;
     int i;

     printf ("\nstructure        pid      pri\n");
     printf ("-----------------------------\n");
     printf ("RUNNING\n");
     if (running->pid != UNUSED)
       printf ("                  %d       %d\n", running->pid, running->pri);
     else
       printf ("            no running process\n");
     printf ("\n\nBLOCKED\n");

     temp = blockedQ;
     while ((temp = temp->right) != blockedQ)
       printf ("                  %d       %d\n",
                temp->element.pid, temp->element.pri);

     printf ("\n\nREADY\n");
     temp = readyQ;
     printf ("  first\n");
     while ((temp = temp->right) != readyQ)
       printf ("                  %d       %d\n", temp->element.pid, temp->element.pri);
     printf ("  last\n");

     printf ("\n\nUNUSED PIDS: ");
     for (i = MIN_PID; i <= MAX_PID; i++)
        if (names[i].status == UNUSED)
            printf (" %d", i);

     printf ("\n");
}
/* end of snapshot */




/*-------------------------------------------------------------------------*/
/* getcommand                                                              */
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


/* End of schedule.c ======================================================*/

