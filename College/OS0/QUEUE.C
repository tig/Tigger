/*
                        queue.c - queue manipulation


         (c) Copyright 1990, Charles E. Kindel, Jr. and Sandra Smith



Author:  Charles E. Kindel, Jr. (ckindel)

Started: January 17, 1990
Version: 1.00

SYNOPSIS
   This module implements and maintains a number of queues that can be
   maintained in either FIFO or key order.

   The queue elements are of type qnode:

      typedef struct qnode
      {
         struct qnode   *next ;
         struct qnode   *prev ;
         int            key ;
      }  qnode ;

   This module contains the following externally visible functions:

      Initialize the queue header pointed to by queue to be the empty
      queue.  The header node must already exist.

         void  initqu (queue)
         qnode *queue ;

      Return 1 if queue is empty, 0 if it is not.

         int   emptyqu (queue)
         queue *queue ;

      Insert the element pointed to by node into the queue of which
      queue is a member, immediately after the element pointed to by
      queue.

         void  insqu (queue, node)
         qnode *queue, *node ;

      Remove the queue node pointed to by element from whatever queue
      it is in, returning that element itself.  Return NULL if element
      is the only thing in the queue (i.e it is the header node).

         qnode *remqu (element)
         qnode *element ;

      Insert the element pointed to by node into the queue whose header
      is pointed to by queue based on the value of the key field.  Queue
      is maintained in increasing order.

         void  ordinsqu (queue, node)
         qnode *queue, *node

      Remove the first node in the queue whose key value is val and
      return it.  If the queue is empty or a the node does not exist
      return NULL.

         qnode *ordremqu (queue, val)
         qnode *queue ;

FACILITY
   Should work with any C compiler.
   

REVISIONS
   DATE        VERSION  NAME           CHANGE
   1/17/90     1.00     C.E. Kindel    Original version
   1/25/90     1.10     Kindel/Smith   Fixed problem in ordremqu

*/


/* LOCAL SYMBOLIC DEFINITIONS ---------------------------------------*/
#define FALSE  0
#define TRUE   1
#define NULL   0
#define ERROR  -1

/* EXPORTED DECLARATIONS --------------------------------------------*/

#include "typedefs.h"

/* FUNCTIONS -------------------------------------------------------*/

/* void initqu (QNODEPTR queue)

   DESCTIPTION
      Initialize the queue header pointed to by queue to be the empty
      queue.  The header node must already exist.

   ARGUMENTS
      QNODEPTR queue   - points to the previously created header node.

*/
void initqu (queue)
QNODEPTR queue ;
{
   queue->next = queue ;
   queue->prev = queue ;
   queue->key  = -1 ;
}


/* int emptyqu (QNODEPTR queue)

   DESCRIPTION
      Return 1 if queue is empty, 0 if it is not.

   ARGUMENTS
      QNODEPTR queue   - points to the queue to test.

   RETURNS
      int = 1 if queue is emtpy, 0 if it is not.

*/
int   emptyqu (queue)
QNODEPTR queue ;
{
   return (queue->next == queue) ;
}


/* void insqu (QNODEPTR queue, QNODEPTR node)

   DESCRIPTION
      Insert the element pointed to by node into the queue of which
      queue is a member, immediately after the element pointed to by
      queue.

   ARGUMENTS
      QNODEPTR queue   - points to the queue node is to be inserted into.
      QNODEPTR node    - points to the node to be inserted.  node must
                       already exist.

*/
void  insqu (queue, node)
QNODEPTR queue, node ;
{
   node->prev = queue ;
   node->next = queue->next ;
   node->next->prev = node ;
   queue->next = node ;
}


/* QNODEPTR remqu (QNODEPTR element)

   DESCRIPTION
      Remove the queue node pointed to by element from whatever queue
      it is in, returning that element itself.  Return NULL if element
      is the only thing in the queue (i.e it is the header node).

   ARUGUMENTS
      QNODEPTR element - element to be removed.

   RETURNS
      QNODEPTR   - NULL if empty queue, else the pointer to the element
                 itself.

*/
QNODEPTR remqu (element)
QNODEPTR element ;
{
   if (emptyqu (element))    /* element is the only thing in the queue */
      return NULL ;

   element->prev->next = element->next ;
   element->next->prev = element->prev ;
   return element ;
}


/* void ordinsqu (QNODEPTR queue, QNODEPTR node)

   DESCRIPTION
      Insert the element pointed to by node into the queue whose header
      is pointed to by queue based on the value of the key field.  Queue
      is maintained in increasing order.

   ARGUMENTS
      QNODEPTR queue   - queue to be manipulated.
      QNODEPTR node    - pointer to element to be inserted.

   NOTES
      To optimize this for speed, since it is inexpensive to do so,
      and since these routines may be used in a priority queue
      I have added code for the cases where the node to be inserted
      has a smaller key than the rest of the queue, and the case where
      the node has a larger key than the rest of the queue.

*/
void  ordinsqu (queue, node)
QNODEPTR queue, node ;
{
QNODEPTR cur ;

/*   tprint ("ordinsqu (%d, node->key = %d)\n", (int) queue, node->key) ;*/
   for (cur=queue ; (cur->next != queue) && (cur->next->key <= node->key) ; cur=cur->next) 
      ;
   insqu (cur, node) ;
}


/* QNODEPTR ordremqu (QNODEPTR queue, int val)

   DESCRIPTION
      Remove the first node in the queue whose key value is val and
      return it.  If the queue is empty or a the node does not exist
      return NULL.

   ARUGMENTS
      QNODEPTR queue - queue to be mainipulated.
      int val      - value to search for.

*/
QNODEPTR ordremqu (QNODEPTR queue, int val)
{
QNODEPTR cur ;

   /*tprint ("ordremqu (%d,%d)\n", (int) queue, val) ;*/
   cur = queue->next ;
   while (cur != queue && cur->key != val) 
      cur = cur->next ;

   if (cur != queue && cur->key == val)
      return (remqu (cur)) ;

   return ((QNODEPTR) NULL) ;
}


void printqu (QNODEPTR queue)
{
   QNODEPTR cur ;

   tprint ("printqu (%d) = ", (int) queue) ;
   if (!emptyqu (queue))
   {
   cur = queue->next ;
   while (cur != queue)
   {
      tprint ("%d, ", cur->key) ;
      cur = cur->next ;
   }
   }
   tprint ("\n") ;
}
/* END OF queue.c -------------------------------------------------*/
