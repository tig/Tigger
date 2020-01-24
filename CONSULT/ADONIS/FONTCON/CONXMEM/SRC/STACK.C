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

#include "PRECOMP.H"
#include "intfsmm.h"
 
//*************************************************************
//
//  fsmmStackCreate()
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
                                       
HSTACK FAR PASCAL fsmmStackCreate( HHEAP hhHeap )
{
    return fsmmListCreate( hhHeap );

} /* fsmmStackCreate() */

//*************************************************************
//
//  fsmmStackDestroy()
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

VOID FAR PASCAL fsmmStackDestroy(HSTACK hstkStack)
{
    fsmmListDestroy( hstkStack );

} /* fsmmStackDestroy() */

//*************************************************************
//
//  fsmmStackFreeStack()
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

VOID FAR PASCAL fsmmStackFreeStack(HSTACK hstkStack)
{
    if (!hstkStack)
        return;

    while (hstkStack->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hstkStack->lpHead + 1);

        fsmmListRemoveNode( hstkStack, lpNode );
        fsmmListFreeNode( hstkStack, lpNode );
    }

} /* fsmmStackFreeStack() */

//*************************************************************
//
//  fsmmStackAllocNode()
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

LPNODE FAR PASCAL fsmmStackAllocNode(HSTACK hstkStack, LPSTR lpMem, WORD wSize)
{
    LPINTNODE lpn;

    if (!hstkStack)
        return NULL;

    lpn = (LPINTNODE)fsmmHeapAllocLock(hstkStack->hhHeap,LPTR,wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* fsmmStackAllocNode() */

//*************************************************************
//
//  fsmmStackFreeNode()
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

LPNODE FAR PASCAL fsmmStackFreeNode( HSTACK hstkStack, LPNODE lpNode )
{
    if (!hstkStack)
        return NULL;

    return fsmmHeapUnlockFree(hstkStack->hhHeap,(LPSTR)(((LPINTNODE)lpNode)-1));

} /* fsmmStackFreeNode() */

//*************************************************************
//
//  fsmmStackItems()
//
//  Purpose:
//		Returns the number of items in the fsmmStack
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

LONG FAR PASCAL fsmmStackItems(HSTACK hstkStack)
{
    if (!hstkStack)
        return 0;

    return hstkStack->lItems;

} /* fsmmStackItems() */

//*************************************************************
//
//  fsmmStackPush()
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

BOOL FAR PASCAL fsmmStackPush(HSTACK hstkStack, LPNODE lpNode)
{
    return fsmmListInsertNode(hstkStack,lpNode,LIST_INSERTHEAD);

} /* fsmmStackPush() */

//*************************************************************
//
//  fsmmStackAllocPush()
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

LPNODE FAR PASCAL fsmmStackAllocPush(HSTACK hstkStack, LPSTR lpMem, WORD wSize)
{
    return fsmmListAllocInsertNode(hstkStack,lpMem,wSize,LIST_INSERTHEAD);

} /* fsmmStackAllocPush() */

//*************************************************************
//
//  fsmmStackPop()
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

LPNODE FAR PASCAL fsmmStackPop(HSTACK hstkStack)
{
    LPNODE lpn = fsmmListFirstNode(hstkStack);

    fsmmListRemoveNode(hstkStack, lpn );
    return lpn;

} /* fsmmStackPop() */

//*************************************************************
//
//  fsmmStackPeek()
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

LPNODE FAR PASCAL fsmmStackPeek(HSTACK hstkStack, LONG lItem)
{
    return fsmmListItem2Node( hstkStack, lItem );

} /* fsmmStackPeek() */

//*************************************************************
//
//  fsmmStackRemove()
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

LPNODE FAR PASCAL fsmmStackRemove(HSTACK hstkStack, LONG lItem)
{
    LPNODE lpn = fsmmListItem2Node(hstkStack,lItem);

    fsmmListRemoveNode(hstkStack, lpn );
    return lpn;

} /* fsmmStackRemove() */

//*************************************************************
//
//  fsmmStackDumpDebug()
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

VOID FAR PASCAL fsmmStackDumpDebug(HSTACK hstkStack, DUMPPROC lpfnDump)
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
        fsmmStackItems(hstkStack), (LPSTR)(hstkStack->fCreatedHeap?"Yes":"No"));
    OutputDebugString( temp );    
    OutputDebugString( "\r\n" );    

// Now dump all the nodes
    lpn = hstkStack->lpHead;
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

} /* fsmmStackDumpDebug() */

/*** EOF: fsmmStack.c ***/

