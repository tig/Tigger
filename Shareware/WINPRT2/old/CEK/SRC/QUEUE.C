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
#include "iCEK.h"

//*************************************************************
//
//  cekmmQueueCreate()
//
//  Purpose:
//		Creates a queue object
//
//
//  Parameters:
//      HHEAP hHeap - heap to use [optional]
//      
//
//  Return: (HQUEUE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************
                                       
HQUEUE WINAPI cekmmQueueCreate( HHEAP hhHeap )
{
    return (HQUEUE)cekmmListCreate( hhHeap );

} /* cekmmQueueCreate() */

//*************************************************************
//
//  cekmmQueueDestroy()
//
//  Purpose:
//		Destroys a queue object
//
//
//  Parameters:
//      HQUEUE hqQueue - queue to destroy
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

VOID WINAPI cekmmQueueDestroy(HQUEUE hqQueue)
{
    cekmmListDestroy( (HLIST)hqQueue );

} /* cekmmQueueDestroy() */

//*************************************************************
//
//  cekmmQueueFreeQueue()
//
//  Purpose:
//		Free all the elements in the queue, but not the queue itself
//
//
//  Parameters:
//      HQUEUE hqQueue
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

VOID WINAPI cekmmQueueFreeQueue(HQUEUE hqQueue)
{
    if (!hqQueue)
        return;

    while (hqQueue->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hqQueue->lpHead + 1);

        cekmmListRemoveNode( hqQueue, lpNode );
        cekmmListFreeNode( hqQueue, lpNode );
    }

} /* cekmmQueueFreeQueue() */

//*************************************************************
//
//  cekmmQueueAllocNode()
//
//  Purpose:
//		Allocates a node that can be used in a queue object
//
//
//  Parameters:
//      HQUEUE hqQueue - queue to alloc for
//      LPSTR lpMem    - optional memory to copy into node
//      UINT wSize     - size of node
//      
//
//  Return: (LPNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

LPNODE WINAPI cekmmQueueAllocNode(HQUEUE hqQueue, LPSTR lpMem, UINT wSize)
{
    LPINTNODE lpn;

    if (!hqQueue)
        return NULL;

    lpn = (LPINTNODE)cekmmHeapAllocLock(hqQueue->hhHeap,LPTR,wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* cekmmQueueAllocNode() */

//*************************************************************
//
//  cekmmQueueFreeNode()
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
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL WINAPI cekmmQueueFreeNode( HQUEUE hqQueue, LPNODE lpNode )
{
    if (!hqQueue)
        return NULL;

    return cekmmHeapUnlockFree(hqQueue->hhHeap,(LPSTR)(((LPINTNODE)lpNode)-1));

} /* cekmmQueueFreeNode() */

//*************************************************************
//
//  cekmmQueueItems()
//
//  Purpose:
//		Returns the number of items in the cekmmQueue
//
//
//  Parameters:
//
//      HQUEUE hqQueue
//      
//
//  Return: (LONG WINAPI)
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

LONG WINAPI cekmmQueueItems(HQUEUE hqQueue)
{
    if (!hqQueue)
        return 0;

    return hqQueue->lItems;

} /* cekmmQueueItems() */

//*************************************************************
//
//  cekmmQueueInQ()
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
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

BOOL WINAPI cekmmQueueInQ(HQUEUE hqQueue, LPNODE lpNode)
{
    return cekmmListInsertNode(hqQueue,lpNode,LIST_INSERTTAIL);

} /* cekmmQueueInQ() */

//*************************************************************
//
//  cekmmQueueAllocInQ()
//
//  Purpose:
//		Allocates the node then pushes it
//
//
//  Parameters:
//      HQUEUE hqQueue
//      LPSTR lpMem
//      UINT wSize
//      
//
//  Return: (LPNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

LPNODE WINAPI cekmmQueueAllocInQ(HQUEUE hqQueue, LPSTR lpMem, UINT wSize)
{
    return cekmmListAllocInsertNode(hqQueue,lpMem,wSize,LIST_INSERTTAIL);

} /* cekmmQueueAllocInQ() */

//*************************************************************
//
//  cekmmQueueDeQ()
//
//  Purpose:
//		Removes a node from the queue
//
//
//  Parameters:
//      HQUEUE hqQueue
//      
//
//  Return: (LPNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE WINAPI cekmmQueueDeQ(HQUEUE hqQueue)
{
    LPNODE lpn = cekmmListFirstNode(hqQueue);

    cekmmListRemoveNode(hqQueue, lpn );
    return lpn;

} /* cekmmQueueDeQ() */

//*************************************************************
//
//  cekmmQueuePeek()
//
//  Purpose:
//		Returns a pointer to the item to peek at (0 is the top)
//
//
//  Parameters:
//      HQUEUE hqQueue
//      UINT nItem
//      
//
//  Return: (LPNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE WINAPI cekmmQueuePeek(HQUEUE hqQueue, LONG lItem)
{
    return cekmmListItem2Node( hqQueue, lItem );

} /* cekmmQueuePeek() */

//*************************************************************
//
//  cekmmQueueRemove()
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
//  Return: (LPNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE WINAPI cekmmQueueRemove(HQUEUE hqQueue, LONG lItem)
{
    LPNODE lpn = cekmmListItem2Node(hqQueue,lItem);

    cekmmListRemoveNode(hqQueue, lpn );
    return lpn;

} /* cekmmQueueRemove() */

//*************************************************************
//
//  cekmmQueueDumpDebug()
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
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

VOID WINAPI cekmmQueueDumpDebug(HQUEUE hqQueue, DUMPPROC lpfnDump)
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
        cekmmQueueItems(hqQueue), (LPSTR)(hqQueue->fCreatedHeap?"Yes":"No"));
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

} /* cekmmQueueDumpDebug() */

/*** EOF: cekmmQueue.c ***/

