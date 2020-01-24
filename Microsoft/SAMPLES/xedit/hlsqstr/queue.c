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

#include "inthlsq.h"
#include <string.h>

//*************************************************************
//
//  hlsqQueueCreate()
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
                                       
HQUEUE FAR PASCAL hlsqQueueCreate( HHEAP hhHeap )
{
    return hlsqListCreate( hhHeap );

} /* hlsqQueueCreate() */

//*************************************************************
//
//  hlsqQueueDestroy()
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

VOID FAR PASCAL hlsqQueueDestroy(HQUEUE hqQueue)
{
    return hlsqListDestroy( hqQueue );

} /* hlsqQueueDestroy() */

//*************************************************************
//
//  hlsqQueueFreeQueue()
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

VOID FAR PASCAL hlsqQueueFreeQueue(HQUEUE hqQueue)
{
    if (!hqQueue)
        return;

    while (hqQueue->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hqQueue->lpHead + 1);

        hlsqListRemoveNode( hqQueue, lpNode );
        hlsqListFreeNode( hqQueue, lpNode );
    }

} /* hlsqQueueFreeQueue() */

//*************************************************************
//
//  hlsqQueueAllocNode()
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

LPNODE FAR PASCAL hlsqQueueAllocNode(HQUEUE hqQueue, LPSTR lpMem, WORD wSize)
{
    LPINTNODE lpn;

    if (!hqQueue)
        return NULL;

    lpn = (LPINTNODE)hlsqHeapAllocLock(hqQueue->hhHeap,LPTR,wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* hlsqQueueAllocNode() */

//*************************************************************
//
//  hlsqQueueFreeNode()
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

LPNODE FAR PASCAL hlsqQueueFreeNode( HQUEUE hqQueue, LPNODE lpNode )
{
    if (!hqQueue)
        return;

    return hlsqHeapUnlockFree(hqQueue->hhHeap,(LPSTR)(((LPINTNODE)lpNode)-1));

} /* hlsqQueueFreeNode() */

//*************************************************************
//
//  hlsqQueueItems()
//
//  Purpose:
//		Returns the number of items in the hlsqQueue
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

LONG FAR PASCAL hlsqQueueItems(HQUEUE hqQueue)
{
    if (!hqQueue)
        return 0;

    return hqQueue->lItems;

} /* hlsqQueueItems() */

//*************************************************************
//
//  hlsqQueueInQ()
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

BOOL FAR PASCAL hlsqQueueInQ(HQUEUE hqQueue, LPNODE lpNode)
{
    return hlsqListInsertNode(hqQueue,lpNode,LIST_INSERTTAIL);

} /* hlsqQueueInQ() */

//*************************************************************
//
//  hlsqQueueAllocInQ()
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

LPNODE FAR PASCAL hlsqQueueAllocInQ(HQUEUE hqQueue, LPSTR lpMem, WORD wSize)
{
    return hlsqListAllocInsertNode(hqQueue,lpMem,wSize,LIST_INSERTTAIL);

} /* hlsqQueueAllocInQ() */

//*************************************************************
//
//  hlsqQueueDeQ()
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

LPNODE FAR PASCAL hlsqQueueDeQ(HQUEUE hqQueue)
{
    LPNODE lpn = hlsqListFirstNode(hqQueue);

    hlsqListRemoveNode(hqQueue, lpn );
    return lpn;

} /* hlsqQueueDeQ() */

//*************************************************************
//
//  hlsqQueuePeek()
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

LPNODE FAR PASCAL hlsqQueuePeek(HQUEUE hqQueue, LONG lItem)
{
    return hlsqListItem2Node( hqQueue, lItem );

} /* hlsqQueuePeek() */

//*************************************************************
//
//  hlsqQueueRemove()
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

LPNODE FAR PASCAL hlsqQueueRemove(HQUEUE hqQueue, LONG lItem)
{
    LPNODE lpn = hlsqListItem2Node(hqQueue,lItem);

    hlsqListRemoveNode(hqQueue, lpn );
    return lpn;

} /* hlsqQueueRemove() */

//*************************************************************
//
//  hlsqQueueDumpDebug()
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

VOID FAR PASCAL hlsqQueueDumpDebug(HQUEUE hqQueue, DUMPPROC lpfnDump)
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
        hlsqQueueItems(hqQueue), (LPSTR)(hqQueue->fCreatedHeap?"Yes":"No"));
    OutputDebugString( temp );    
    OutputDebugString( "\r\n" );    

// Now dump all the nodes
    lpn = hqQueue->lpHead;
    while (lpn)
    {
        LPSTR lpUser=NULL;

        wsprintf( temp, "  Item#%-6d <%#08lx>  ", lNode, lpn );
        OutputDebugString( temp );    

        if (lpfnDump)
            lpUser = lpfnDump( (LPNODE)(lpn+1) );
        if (lpUser)
            OutputDebugString( lpUser );
        OutputDebugString( "\r\n" );    
        lpn = lpn->lpNext;
        lNode++;
    }
    OutputDebugString( "\r\n" );    

} /* hlsqQueueDumpDebug() */

/*** EOF: hlsqQueue.c ***/

