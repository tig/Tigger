/*********************************************************/
/* Disk Device routines    
 */
#include "typedefs2.h"

int work_to_do = 0 ;     /* semaphore used to wait up disk server */
int Disk_mutex = 1 ;     /* semaphore used to provide mutual exclusion
                            within disk routines */
extern QNODE Drequests ; /* the queue of requests for disk operations */

MBPTR Disk_req ;  /* the mailbox used for request processing - it
                            is initialized in Disk_Scheduler */
                                                            
int curr_track = 0 ;     /* the current track being accessed by the disk */
 
/*********************************************************/
/* void Disk_Scheduler (void)
 * 
 * This routine is responsible for scheduling any requests to the
 * disk for some operation. 
 */
void Disk_Scheduler (void)
{
   DISKPTR request ; 

   /* Disk_req is the mailbox that the user sends to */
   Disk_req = mbCreate (1, sizeof(DISKNODE) ) ;

   while (TRUE)
   {
      request = (DISKPTR) malloc (sizeof(DISKNODE)) ;
      mbReceive (Disk_req, request) ;
      P (&Disk_mutex) ;
      ordinsqu (&Drequests, (QNODEPTR) request ) ;
      V (&Disk_mutex) ;
      V (&work_to_do) ;
   
   }
}

/*********************************************************/
/* void Disk_Server (void)
 *
 * This routine is concerned with the performing the actual
 * disk operations.  Once the operation has been done the
 * user's process is notified via the mailbox contained in
 * its process node.
 * 
 */
void Disk_Server (void)
{
   int status ;
   dev_req info ;
   DISKPTR tmp ;
   int max_track = disk_ncylinders() ;
   
   while (TRUE)
   {
      /* wait until scheduler indicates it has a process needing
         a service */ 
      P (&work_to_do) ;
     
      /* service the processes in the direction of the scan */ 
      do
	{
	  P(&Disk_mutex) ;
	  if (NULL == (tmp = (DISKPTR) ordremqu (&Drequests, curr_track)))
             if (!emptyqu(&Drequests) )
                if ( ++curr_track >= max_track )
                   curr_track = 0 ;  
	  V( &Disk_mutex) ;
	}
      while (!tmp && !emptyqu (&Drequests)) ;

      /* a valid request has been encountered on the queue and
         the corresponding disk operations are performed.
      */
      if (tmp)
	{
          {
             /* do a seek to the track requested */
	     info.opr = SEEK ;
	     info.reg1 = tmp->key ;
	     outp (DISK_DEV, (int)&info) ;
	     status = waitdevice (DISK_DEV, 0) ;
	   }
         while ( status == DEV_ERROR ) ;
 
          /* carry out the requested operation - sending the result
             to the user through the given mailbox */ 
           {
	     info.opr = tmp->opr ;
       	     info.reg1 = tmp->sector ;
	     info.reg2 = tmp->addr ;
	     outp (DISK_DEV, (int)&info) ;     
	     status = waitdevice (DISK_DEV, 0) ;
            }
          while ( status == DEV_ERROR ) ;
	  mbSend (tmp->mbox, (MSGPTR) &status) ; 

          /* dispose of the request node */
	  free ((char *) tmp) ;
	}
    }
}

/**********************************************************/  














