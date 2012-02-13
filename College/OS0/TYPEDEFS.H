/*           typedefs.h - Common routines for the OS/0 kernel (phase1)

               (c) copyright 1990, Charles Kindel and Sandy Smith
*/
#define TYPEDEFS

#include <stdarg.h>
#include "machine.h"
#include "usloss.h"
#ifndef QUEUES
#include "queue.h"
#endif

/* Standard junk */
#define NULL   0
#define TRUE   1
#define FALSE  0

/* Process status' */
#define  DEAD      0
#define  CURRENT   1
#define  READY     2
#define  WAITING   3
#define  ZOMBIE    4
#define  CHILD_WAIT 5

/* Max processes */
#define MAXPROC 20

/* Process priorities */
#define  MAX_PRIORITY 1
#define  MIN_PRIORITY 5
#define  TIME_SLICE   2

#define  CLK_INTERRUPT 5

/* Device stuff */
#define MAX_UNIT 4

/* Error stuff */
/* These identifiers correspond to a string in the function
   finish ().
*/
#define  ER_NORMAL         0 
#define  ER_TOOMANYPROCS   1     /* too many processes were created */
#define  ER_INVALID_V      2     /* invalid V operation */
#define  ER_INVALID_PRI    3     /* invalid priority */
#define  ER_DEADLOCK       4     /* Deadlock was detected */
#define  ER_STACK          5     /* Out of stack space (cannot allocate) */
#define  ER_NOCHILDREN     6     /* Join is called with no children */
#define  ER_SYSCALL        7     /* System Call - not yet implemented */
#define  ER_QUIT           8     /* Quit called with invalid process id */
#define  ER_JOIN           9     /* if join should 'blow up' */
#define  ER_QUITWCHILD     10    /* attempt to quit with children */
#define  ER_QUITDEAD       11    /* attempt to quit a dead process */    
#define  ER_MODE           12    /* in user mode within a kernel routine */

/* PROCESSES */
/* Process Descriptor type definition (pd) */
typedef struct process_desc
{
   QNODEPTR next ;
   QNODEPTR prev ;
   int      key ;


   void     (*f) () ;
   int      pid ;
   int      priority ;
   int      status ;
   int      termstatus ;
   struct process_desc *pdParent;
   int      nChildren ;
   int      nDone ;
   unsigned CPU_time ;
   context  state ;
   int      stack ;
   int      bot_stack ; /* points to base of stack (for freeing) */
   int      sp ;

} process_desc, PD, *PDPTR ;

/* Function prototypes */
/* process control routines */
void     tsXXp1 (void) ;
int      fork (void (*f) (), int stacksize, int priority) ;
int      join (int *status) ;
int      new_stack (int stacksize, int * bot_stack) ;
PDPTR    new_pd (int *) ;
void     quit (int pid, int status) ;
void     free_pid (int pid) ;
void     free_stack (int stack) ; 
void     scheduler (void) ;
void     deadlock (void) ;
void     check_mode (void) ;
int      get_pid (void) ;

void     P (int *semaphore) ;
void     V (int *semaphore) ;

int      begincs (void) ;
void     endcs (int x) ;
int      are_ints_enabled (void) ;

void     waitclock (void) ;
int      waitdevice (int type, int unit) ;
int      readtime (void) ;

void     clock_handler (int type, int unit) ;
void     alarm_handler (int type, int unit) ;
void     syscall_handler (int type, int unit) ;
void     disk_handler (int type, int unit) ;
void     terminal_handler (int type, int unit) ;

char     *malloc () ;
void     free ( char *p) ;
void     outp (int dev, int arg) ;
int      inp (int dev) ;
void     halt (void) ;
void     trace (char *s) ;
void     console (char *s) ;
void     context_switch (context *s1, context *s2) ;
void     int_enable (void) ;
void     int_disable (void) ;
int      in_kernel_mode (void) ;
int      sysclock (void) ;
void     cprint (char *,...) ;
void     tprint (char *,...) ;
void     waitint (void) ;
     
/* user routines */
void     userstartup (void) ;



