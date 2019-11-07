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
#include "iCEK.h"
 
//*************************************************************
//
//  cekmmStackCreate()
//
//  Purpose:
//		Creates a linked stack object
//
//
//  Parameters:
//      HHEAP hHeap - heap to use [optional]
//      
//
//  Return: (HSTACK WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************
                                       
HSTACK WINAPI cekmmStackCreate( HHEAP hhHeap )
{
    return cekmmListCreate( hhHeap );

} /* cekmmStackCreate() */

//*************************************************************
//
//  cekmmStackDestroy()
//
//  Purpose:
//		Destroys a stack object
//
//
//  Parameters:
//      HSTACK hstkStack - stack to destroy
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

VOID WINAPI cekmmStackDestroy(HSTACK hstkStack)
{
    cekmmListDestroy( hstkStack );

} /* cekmmStackDestroy() */

//*************************************************************
//
//  cekmmStackFreeStack()
//
//  Purpose:
//		Free all the elements in the stack, but not the stack itself
//
//
//  Parameters:
//      HSTACK hstkStack
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

VOID WINAPI cekmmStackFreeStack(HSTACK hstkStack)
{
    if (!hstkStack)
        return;

    while (hstkStack->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hstkStack->lpHead + 1);

        cekmmListRemoveNode( hstkStack, lpNode );
        cekmmListFreeNode( hstkStack, lpNode );
    }

} /* cekmmStackFreeStack() */

//*************************************************************
//
//  cekmmStackAllocNode()
//
//  Purpose:
//		Allocates a node that can be used in a stack object
//
//
//  Parameters:
//      HSTACK hstkStack - stack to alloc for
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

LPNODE WINAPI cekmmStackAllocNode(HSTACK hstkStack, LPSTR lpMem, UINT wSize)
{
    LPINTNODE lpn;

    if (!hstkStack)
        return NULL;

    lpn = (LPINTNODE)cekmmHeapAllocLock(hstkStack->hhHeap,LPTR,wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* cekmmStackAllocNode() */

//*************************************************************
//
//  cekmmStackFreeNode()
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
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE WINAPI cekmmStackFreeNode( HSTACK hstkStack, LPNODE lpNode )
{
    if (!hstkStack)
        return NULL;

    return cekmmHeapUnlockFree(hstkStack->hhHeap,(LPSTR)(((LPINTNODE)lpNode)-1));

} /* cekmmStackFreeNode() */

//*************************************************************
//
//  cekmmStackItems()
//
//  Purpose:
//		Returns the number of items in the cekmmStack
//
//
//  Parameters:
//
//      HSTACK hstkStack
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

LONG WINAPI cekmmStackItems(HSTACK hstkStack)
{
    if (!hstkStack)
        return 0;

    return hstkStack->lItems;

} /* cekmmStackItems() */

//*************************************************************
//
//  cekmmStackPush()
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

BOOL WINAPI cekmmStackPush(HSTACK hstkStack, LPNODE lpNode)
{
    return cekmmListInsertNode(hstkStack,lpNode,LIST_INSERTHEAD);

} /* cekmmStackPush() */

//*************************************************************
//
//  cekmmStackAllocPush()
//
//  Purpose:
//		Allocates the node then pushes it
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE WINAPI cekmmStackAllocPush(HSTACK hstkStack, LPSTR lpMem, UINT wSize)
{
    return cekmmListAllocInsertNode(hstkStack,lpMem,wSize,LIST_INSERTHEAD);

} /* cekmmStackAllocPush() */

//*************************************************************
//
//  cekmmStackPop()
//
//  Purpose:
//		Pops a node off of the stack
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE WINAPI cekmmStackPop(HSTACK hstkStack)
{
    LPNODE lpn = cekmmListFirstNode(hstkStack);

    cekmmListRemoveNode(hstkStack, lpn );
    return lpn;

} /* cekmmStackPop() */

//*************************************************************
//
//  cekmmStackPeek()
//
//  Purpose:
//		Returns a pointer to the item to peek at (0 is the top)
//
//
//  Parameters:
//      HSTACK hstkStack
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

LPNODE WINAPI cekmmStackPeek(HSTACK hstkStack, LONG lItem)
{
    return cekmmListItem2Node( hstkStack, lItem );

} /* cekmmStackPeek() */

//*************************************************************
//
//  cekmmStackRemove()
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
//  Return: (LPNODE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPNODE WINAPI cekmmStackRemove(HSTACK hstkStack, LONG lItem)
{
    LPNODE lpn = cekmmListItem2Node(hstkStack,lItem);

    cekmmListRemoveNode(hstkStack, lpn );
    return lpn;

} /* cekmmStackRemove() */

//*************************************************************
//
//  cekmmStackDumpDebug()
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

VOID WINAPI cekmmStackDumpDebug(HSTACK hstkStack, DUMPPROC lpfnDump)
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
        cekmmStackItems(hstkStack), (LPSTR)(hstkStack->fCreatedHeap?"Yes":"No"));
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

} /* cekmmStackDumpDebug() */

/*** EOF: cekmmStack.c ***/

