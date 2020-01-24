//*************************************************************
//  File name: queue.c
//
//  Description: 
//      Code for the queue stuff
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "intfsmm.h"

//*************************************************************
//
//  fsmmQueueCreate()
//
//  Purpose:
//		Creates a queue object
//
//
//  Parameters:
//      HHEAP hHeap - heap to use [optional]
//      
//
//  Return: (HQUEUE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************
                                       
HQUEUE FAR PASCAL fsmmQueueCreate( HHEAP hhHeap )
{
    return fsmmListCreate( hhHeap );

} /* fsmmQueueCreate() */

//*************************************************************
//
//  fsmmQueueDestroy()
//
//  Purpose:
//		Destroys a queue object
//
//
//  Parameters:
//      HQUEUE hqQueue - queue to destroy
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

VOID FAR PASCAL fsmmQueueDestroy(HQUEUE hqQueue)
{
    fsmmListDestroy( hqQueue );

} /* fsmmQueueDestroy() */

//*************************************************************
//
//  fsmmQueueFreeQueue()
//
//  Purpose:
//		Free all the elements in the queue, but not the queue itself
//
//
//  Parameters:
//      HQUEUE hqQueue
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

VOID FAR PASCAL fsmmQueueFreeQueue(HQUEUE hqQueue)
{
    if (!hqQueue)
        return;

    while (hqQueue->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hqQueue->lpHead + 1);

        fsmmListRemoveNode( hqQueue, lpNode );
        fsmmListFreeNode( hqQueue, lpNode );
    }

} /* fsmmQueueFreeQueue() */

//*************************************************************
//
//  fsmmQueueAllocNode()
//
//  Purpose:
//		Allocates a node that can be used in a queue object
//
//
//  Parameters:
//      HQUEUE hqQueue - queue to alloc for
//      LPSTR lpMem    - optional memory to copy into node
//      WORD wSize     - size of node
//      
//
//  Return: (LPNODE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

LPNODE FAR PASCAL fsmmQueueAllocNode(HQUEUE hqQueue, LPSTR lpMem, WORD wSize)
{
    LPINTNODE lpn;

    if (!hqQueue)
        return NULL;

    lpn = (LPINTNODE)fsmmHeapAllocLock(hqQueue->hhHeap,LPTR,wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* fsmmQueueAllocNode() */

//*************************************************************
//
//  fsmmQueueFreeNode()
//
//  Purpose:
//		Frees a node that was QueueAllocNode'd.  REMOVE FIRST!!!!!
//
//
//  Parameters:
//      HQUEUE hqQueue - queue
//      LPNODE lpNode  - node to free
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE FAR PASCAL fsmmQueueFreeNode( HQUEUE hqQueue, LPNODE lpNode )
{
    if (!hqQueue)
        return NULL;

    return fsmmHeapUnlockFree(hqQueue->hhHeap,(LPSTR)(((LPINTNODE)lpNode)-1));

} /* fsmmQueueFreeNode() */

//*************************************************************
//
//  fsmmQueueItems()
//
//  Purpose:
//		Returns the number of items in the fsmmQueue
//
//
//  Parameters:
//
//      HQUEUE hqQueue
//      
//
//  Return: (LONG FAR PASCAL)
//      number of items
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LONG FAR PASCAL fsmmQueueItems(HQUEUE hqQueue)
{
    if (!hqQueue)
        return 0;

    return hqQueue->lItems;

} /* fsmmQueueItems() */

//*************************************************************
//
//  fsmmQueueInQ()
//
//  Purpose:
//		Puts the node into the queue
//
//
//  Parameters:
//      HQUEUE hqQueue
//      LPNODE lpNode
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

BOOL FAR PASCAL fsmmQueueInQ(HQUEUE hqQueue, LPNODE lpNode)
{
    return fsmmListInsertNode(hqQueue,lpNode,LIST_INSERTTAIL);

} /* fsmmQueueInQ() */

//*************************************************************
//
//  fsmmQueueAllocInQ()
//
//  Purpose:
//		Allocates the node then pushes it
//
//
//  Parameters:
//      HQUEUE hqQueue
//      LPSTR lpMem
//      WORD wSize
//      
//
//  Return: (LPNODE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

LPNODE FAR PASCAL fsmmQueueAllocInQ(HQUEUE hqQueue, LPSTR lpMem, WORD wSize)
{
    return fsmmListAllocInsertNode(hqQueue,lpMem,wSize,LIST_INSERTTAIL);

} /* fsmmQueueAllocInQ() */

//*************************************************************
//
//  fsmmQueueDeQ()
//
//  Purpose:
//		Removes a node from the queue
//
//
//  Parameters:
//      HQUEUE hqQueue
//      
//
//  Return: (LPNODE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE FAR PASCAL fsmmQueueDeQ(HQUEUE hqQueue)
{
    LPNODE lpn = fsmmListFirstNode(hqQueue);

    fsmmListRemoveNode(hqQueue, lpn );
    return lpn;

} /* fsmmQueueDeQ() */

//*************************************************************
//
//  fsmmQueuePeek()
//
//  Purpose:
//		Returns a pointer to the item to peek at (0 is the top)
//
//
//  Parameters:
//      HQUEUE hqQueue
//      WORD nItem
//      
//
//  Return: (LPNODE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE FAR PASCAL fsmmQueuePeek(HQUEUE hqQueue, LONG lItem)
{
    return fsmmListItem2Node( hqQueue, lItem );

} /* fsmmQueuePeek() */

//*************************************************************
//
//  fsmmQueueRemove()
//
//  Purpose:
//		Removes a element out of sequence
//
//
//  Parameters:
//      HQUEUE hqQueue
//      LONG lItem
//      
//
//  Return: (LPNODE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE FAR PASCAL fsmmQueueRemove(HQUEUE hqQueue, LONG lItem)
{
    LPNODE lpn = fsmmListItem2Node(hqQueue,lItem);

    fsmmListRemoveNode(hqQueue, lpn );
    return lpn;

} /* fsmmQueueRemove() */

//*************************************************************
//
//  fsmmQueueDumpDebug()
//
//  Purpose:
//		Dumps the queue to the debug terminal
//
//
//  Parameters:
//      HQUEUE hqQueue    - queue to dump
//      DUMPPROC lpfnDump   - user dump proc
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

VOID FAR PASCAL fsmmQueueDumpDebug(HQUEUE hqQueue, DUMPPROC lpfnDump)
{
    char temp[80];
    LPINTNODE lpn;
    LONG      lNode=0;

    OutputDebugString( "\r\nQueue Manager V1.00 (Debug Info)\r\n" );
    OutputDebugString( "Written by Michael S. McCraw  Copyright @1992\r\n" );
    OutputDebugString("------------------------------------------------\r\n");

    if (!hqQueue)
        return;

// Dump queue header first
    wsprintf( temp, "  HQUEUE: %#08lx  Items: %ld  CreatedHeap: %s", hqQueue,
        fsmmQueueItems(hqQueue), (LPSTR)(hqQueue->fCreatedHeap?"Yes":"No"));
    OutputDebugString( temp );    
    OutputDebugString( "\r\n" );    

// Now dump all the nodes
    lpn = hqQueue->lpHead;
    while (lpn)
    {
        LPSTR lpUser=NULL;

        if (lpfnDump)
            lpUser = lpfnDump( (LPNODE)(lpn+1) );

        if (lpUser)
        {
            wsprintf( temp, "  Item#%-6d <%#08lx>  ", lNode, lpn );
            OutputDebugString( temp );    
            OutputDebugString( lpUser );
            OutputDebugString( "\r\n" );    
        }
        lpn = lpn->lpNext;
        lNode++;
    }
    OutputDebugString( "\r\n" );    

} /* fsmmQueueDumpDebug() */

/*** EOF: fsmmQueue.c ***/

