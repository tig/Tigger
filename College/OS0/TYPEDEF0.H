/*                typedefs2.h - Common type declarations for OS/0
                                (phase2)

               (c) copyright 1990, Charles Kindel and Sandy Smith
*/
#define TYPEDEFS2  
#include <stdarg.h>
#ifndef QUEUES
#include "queue.h"
#endif
#ifndef TYPEDEFS
#include "typedefs.h"
#endif
#include "mbox.h"

/* General C defines */
#define  NULL       0
#define  TRUE       1
#define  FALSE      0

/* Process priorities */
#define  MIN_PRIORITY   5 
#define  MAX_PRIORITY   1 

/* Input buffer size */
#define  MAX_BUFFER  80
 
/* Maximum number of sectors */
#define  MAX_SECTOR  16

/* The maximum number of units associated with a device */ 
#define  MAX_UNIT    4 

/* QUEUES:  */
/* basic queue definitions.  */

/* the definition of the queue which is used to contain the
   user-level processes.
 */
typedef struct tnode
{
   QNODEPTR next ;
   QNODEPTR prev ;
   int      key ;         /* contains the process' pid */
   int      parent ;      /* contains the parent's pid */
   int      done ;        /* dead indicator */
   MBPTR    dbox ;        /* private mailbox */
   QNODE children ;       /* list of children */
   QNODE mailboxes ;      /* list of created mailboxes */
} tnode, TNODE, *TNODEPTR ;

/* the definition of a queue used to hold processes waiting on
   a delay.
 */
typedef struct dnode
{
   QNODEPTR next ;
   QNODEPTR prev ;
   int      key ;         /* time which to delay a process */
   MBPTR    mbox ;        /* mailbox to use to wake him up */
}  dnode, DNODE, *DNODEPTR ; 

/* the definition of a node used to hold requests to the disk
 */
typedef struct disknode 
{
   QNODEPTR next ;
   QNODEPTR prev ;
   int      key  ;      /* track seeking to */
   MBPTR    mbox ;      /* mailbox to send returned status of operation */ 
   int      opr  ;      /* type of operation to be performed */
   int      sector ;    /* the sector of the given track */
   int      addr ;      /* address the operation is to use for data */   
}  disknode, DISKNODE, *DISKPTR ;

/* -----System Arguements (syscall.h)---- */

typedef struct sysargs {
  int number;
  int arg1, arg2, arg3, arg4;
} sysargs;

/* defines for all of the system calls */
#define READTERM	1
#define WRITETERM	2
#define	SPAWN		3
#define CREATEMB	4
#define SEND		5
#define RECEIVE		6
#define CONDREC		7
#define DELAY		8
#define DISKPUT		9
#define DISKGET		10
#define GETTIMEOFDAY	11
#define TERMINATE	12 

/* phase2 specific routines */
int read_term (int buffer, int size) ;
void term_in (void) ;
int write_term (int buffer, int count) ;
void terminate (void) ;
void clean_up_mb (QNODEPTR q) ;
void ts2XXp1 (void) ;
int OurSpawn (int addr, int size, int pri) ;
int OurCreateMB (int numslots, int size) ;
void OurDelay (int time) ;
void Disk_Scheduler (void) ;
void Disk_Server (void) ;

/* user routines */
void Uboot (void) ;



