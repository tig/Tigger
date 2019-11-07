//*************************************************************
//  File name: stack.c
//
//  Description: 
//      Code for the stack stuff
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#include "inthlsq.h"
#include <string.h>

//*************************************************************
//
//  hlsqStackCreate()
//
//  Purpose:
//		Creates a linked stack object
//
//
//  Parameters:
//      HHEAP hHeap - heap to use [optional]
//      
//
//  Return: (HSTACK FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************
                                       
HSTACK FAR PASCAL hlsqStackCreate( HHEAP hhHeap )
{
    return hlsqListCreate( hhHeap );

} /* hlsqStackCreate() */

//*************************************************************
//
//  hlsqStackDestroy()
//
//  Purpose:
//		Destroys a stack object
//
//
//  Parameters:
//      HSTACK hstkStack - stack to destroy
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

VOID FAR PASCAL hlsqStackDestroy(HSTACK hstkStack)
{
    return hlsqListDestroy( hstkStack );

} /* hlsqStackDestroy() */

//*************************************************************
//
//  hlsqStackFreeStack()
//
//  Purpose:
//		Free all the elements in the stack, but not the stack itself
//
//
//  Parameters:
//      HSTACK hstkStack
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

VOID FAR PASCAL hlsqStackFreeStack(HSTACK hstkStack)
{
    if (!hstkStack)
        return;

    while (hstkStack->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hstkStack->lpHead + 1);

        hlsqListRemoveNode( hstkStack, lpNode );
        hlsqListFreeNode( hstkStack, lpNode );
    }

} /* hlsqStackFreeStack() */

//*************************************************************
//
//  hlsqStackAllocNode()
//
//  Purpose:
//		Allocates a node that can be used in a stack object
//
//
//  Parameters:
//      HSTACK hstkStack - stack to alloc for
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

LPNODE FAR PASCAL hlsqStackAllocNode(HSTACK hstkStack, LPSTR lpMem, WORD wSize)
{
    LPINTNODE lpn;

    if (!hstkStack)
        return NULL;

    lpn = (LPINTNODE)hlsqHeapAllocLock(hstkStack->hhHeap,LPTR,wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* hlsqStackAllocNode() */

//*************************************************************
//
//  hlsqStackFreeNode()
//
//  Purpose:
//		Frees a node that was StackAllocNode'd.  REMOVE FIRST!!!!!
//
//
//  Parameters:
//      HSTACK hstkStack - stack
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

LPNODE FAR PASCAL hlsqStackFreeNode( HSTACK hstkStack, LPNODE lpNode )
{
    if (!hstkStack)
        return;

    return hlsqHeapUnlockFree(hstkStack->hhHeap,(LPSTR)(((LPINTNODE)lpNode)-1));

} /* hlsqStackFreeNode() */

//*************************************************************
//
//  hlsqStackItems()
//
//  Purpose:
//		Returns the number of items in the hlsqStack
//
//
//  Parameters:
//
//      HSTACK hstkStack
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

LONG FAR PASCAL hlsqStackItems(HSTACK hstkStack)
{
    if (!hstkStack)
        return 0;

    return hstkStack->lItems;

} /* hlsqStackItems() */

//*************************************************************
//
//  hlsqStackPush()
//
//  Purpose:
//		Pushes the node onto the stack
//
//
//  Parameters:
//      HSTACK hstkStack
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

BOOL FAR PASCAL hlsqStackPush(HSTACK hstkStack, LPNODE lpNode)
{
    return hlsqListInsertNode(hstkStack,lpNode,LIST_INSERTHEAD);

} /* hlsqStackPush() */

//*************************************************************
//
//  hlsqStackAllocPush()
//
//  Purpose:
//		Allocates the node then pushes it
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE FAR PASCAL hlsqStackAllocPush(HSTACK hstkStack, LPSTR lpMem, WORD wSize)
{
    return hlsqListAllocInsertNode(hstkStack,lpMem,wSize,LIST_INSERTHEAD);

} /* hlsqStackAllocPush() */

//*************************************************************
//
//  hlsqStackPop()
//
//  Purpose:
//		Pops a node off of the stack
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE FAR PASCAL hlsqStackPop(HSTACK hstkStack)
{
    LPNODE lpn = hlsqListFirstNode(hstkStack);

    hlsqListRemoveNode(hstkStack, lpn );
    return lpn;

} /* hlsqStackPop() */

//*************************************************************
//
//  hlsqStackPeek()
//
//  Purpose:
//		Returns a pointer to the item to peek at (0 is the top)
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE FAR PASCAL hlsqStackPeek(HSTACK hstkStack, LONG lItem)
{
    return hlsqListItem2Node( hstkStack, lItem );

} /* hlsqStackPeek() */

//*************************************************************
//
//  hlsqStackRemove()
//
//  Purpose:
//		Removes a element out of sequence
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE FAR PASCAL hlsqStackRemove(HSTACK hstkStack, LONG lItem)
{
    LPNODE lpn = hlsqListItem2Node(hstkStack,lItem);

    hlsqListRemoveNode(hstkStack, lpn );
    return lpn;

} /* hlsqStackRemove() */

//*************************************************************
//
//  hlsqStackDumpDebug()
//
//  Purpose:
//		Dumps the stack to the debug terminal
//
//
//  Parameters:
//      HSTACK hstkStack    - stack to dump
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

VOID FAR PASCAL hlsqStackDumpDebug(HSTACK hstkStack, DUMPPROC lpfnDump)
{
    char temp[80];
    LPINTNODE lpn;
    LONG      lNode=0;

    OutputDebugString( "\r\nStack Manager V1.00 (Debug Info)\r\n" );
    OutputDebugString( "Written by Michael S. McCraw  Copyright @1992\r\n" );
    OutputDebugString("------------------------------------------------\r\n");

    if (!hstkStack)
        return;

// Dump stack header first
    wsprintf( temp, "  HSTACK: %#08lx  Items: %ld  CreatedHeap: %s", hstkStack,
        hlsqStackItems(hstkStack), (LPSTR)(hstkStack->fCreatedHeap?"Yes":"No"));
    OutputDebugString( temp );    
    OutputDebugString( "\r\n" );    

// Now dump all the nodes
    lpn = hstkStack->lpHead;
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

} /* hlsqStackDumpDebug() */

/*** EOF: hlsqStack.c ***/

