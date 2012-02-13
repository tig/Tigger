/*                  mbox.c - Mailbox routines 

              Copyright (c) 1990, Charles Kindel and
	                          Sandy Smith

*/

/* These routines are a "toolbox" for a queue based */
/* mail box implementation.                         */


#include <strings.h>
#include "mbox.h"
#include "queue.h"
#include "typedefs2.h"

/* MBPTR mbCreate (int maxslots, int slotsize) */
/*
 *
 */
MBPTR mbCreate (int maxslots, int slotsize) 
{
   MBPTR mb ;
   QNODEPTR tmp ;

   if (NULL == (mb = (MBPTR) malloc (sizeof (MB))))
      return (MBPTR) -1 ;
   initqu ((QNODEPTR) mb) ;
   mb->sem_rec = 0 ;
   mb->mutex = 1 ;
   mb->slotsize = slotsize ;
   mb->maxslots = maxslots ;
   mb->sem_send = maxslots ;  /* semaphore to keep track */
                              /* of slot usage */
   return mb ;
}

/* int mbDestroy (MBPTR mb) */
/* 
*
*/
int mbDestroy (MBPTR mb)
{
MBPTR mbTemp ;

   while (mbTemp = (MBPTR) remqu ((QNODEPTR) mb->next))
       free ((MSGPTR) mbTemp->key) ;
   free ((char*) mb) ;

   return TRUE ;
}
   

/* mbSend (MBPTR mb, MSGPTR msg) */
/*
 *
 */
int mbSend (MBPTR mb, MSGPTR msg)
{
   int flag ;
   QNODEPTR newnode, tmp ;

      P(&mb->sem_send) ;

      /* allocate node for message queue */
      if (NULL == (newnode = (QNODEPTR) malloc (sizeof (QNODE))))
          return  FALSE;
   
      /* allocate memory for message, and put on message queue */
      if (NULL != ((MSGPTR)newnode->key = (MSGPTR) malloc (mb->slotsize)))
      {
          bcopy (msg, (MSGPTR) newnode->key, mb->slotsize) ;
          insqu ((QNODEPTR) mb, (QNODEPTR) newnode) ; 
      } 
      else
          return FALSE ;   
      V (&mb->sem_rec) ;
      return TRUE ;
}


/* mbReceive (MBPTR mb, MSGPTR msg) */
void mbReceive (MBPTR mb, MSGPTR msg) 
{
    int flag ;
    QNODEPTR newnode ;

    P (&mb->mutex) ;
    P (&mb->sem_rec) ;
    newnode = remqu ((QNODEPTR) mb->prev) ;
    bcopy ((MSGPTR)newnode->key, msg, mb->slotsize) ;
    free ((MSGPTR) newnode->key) ;
    free ((char*) newnode) ;
    V (&mb->mutex) ;
    V (&mb->sem_send) ;

    return ;
}


/* mbCondReceive (MBPTR mb, MSGPTR msg) */
int mbCondReceive (MBPTR mb, MSGPTR msg)
{
   if (!emptyqu ((QNODEPTR) mb))
   {
      mbReceive (mb, msg) ;
      return TRUE ;
   }
   else
      return FALSE ;
}

/* --- end of file (mbox.c) ---- */



