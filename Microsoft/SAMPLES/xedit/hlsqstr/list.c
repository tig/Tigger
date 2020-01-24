//*************************************************************
//  File name: list.c
//
//  Description: 
//      Code for the list stuff
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#include "inthlsq.h"
#include <string.h>

//*************************************************************
//
//  hlsqListCreate()
//
//  Purpose:
//		Creates a linked list object
//
//
//  Parameters:
//      HHEAP hHeap - heap to use [optional]
//      
//
//  Return: (HLIST FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************
                                       
HLIST FAR PASCAL hlsqListCreate( HHEAP hhHeap )
{
    HLIST  hlstList;
    WORD   fCreatedHeap = FALSE;

    if (!hhHeap)
    {
       hhHeap = hlsqHeapCreate(2048, HT_MULTIPLE);
        if (!hhHeap)
            return NULL;
        fCreatedHeap = TRUE;
    }

    if (hlstList = (HLIST)hlsqHeapAllocLock(hhHeap, LPTR, sizeof( LIST )) )
    {
        hlstList->hhHeap =hhHeap;
        hlstList->fCreatedHeap = fCreatedHeap;
        hlstList->lItems = 0L;
        hlstList->lpHead = hlstList->lpTail = NULL;
    }
    else
    {
        if (fCreatedHeap)
            hlsqHeapDestroy(hhHeap);
    }

    return hlstList;

} /* hlsqListCreate() */

//*************************************************************
//
//  hlsqListDestroy()
//
//  Purpose:
//		Destroys a list object
//
//
//  Parameters:
//      HLIST hlstList - list to destroy
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

VOID FAR PASCAL hlsqListDestroy(HLIST hlstList)
{
    if (!hlstList)
        return;

    if (hlstList->fCreatedHeap)
        hlsqHeapDestroy( hlstList->hhHeap );
    else
    {
        hlsqListFreeList( hlstList );
        hlsqHeapUnlockFree( hlstList->hhHeap, (LPSTR)hlstList );
    }

} /* hlsqListDestroy() */

//*************************************************************
//
//  hlsqListFreeList()
//
//  Purpose:
//		Free all the elements in the list, but not the list itself
//
//
//  Parameters:
//      HLIST hlstList
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

VOID FAR PASCAL hlsqListFreeList(HLIST hlstList)
{
    if (!hlstList)
        return;

    while (hlstList->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hlstList->lpHead + 1);

        hlsqListRemoveNode( hlstList, lpNode );
        hlsqListFreeNode( hlstList, lpNode );
    }

} /* hlsqListFreeList() */

//*************************************************************
//
//  hlsqListAllocNode()
//
//  Purpose:
//		Allocates a node that can be used in a list object
//
//
//  Parameters:
//      HLIST hlstList - list to alloc for
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

LPNODE FAR PASCAL hlsqListAllocNode(HLIST hlstList, LPSTR lpMem, WORD wSize)
{
    LPINTNODE lpn;

    if (!hlstList)
        return NULL;

    lpn = (LPINTNODE)hlsqHeapAllocLock(hlstList->hhHeap,LPTR, wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* hlsqListAllocNode() */

//*************************************************************
//
//  hlsqListFreeNode()
//
//  Purpose:
//		Frees a node that was ListAllocNode'd.  REMOVE FIRST!!!!!
//
//
//  Parameters:
//      HLIST hlstList - list
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

VOID FAR PASCAL hlsqListFreeNode( HLIST hlstList, LPNODE lpNode )
{
    if (!hlstList)
        return;

    hlsqHeapUnlockFree(hlstList->hhHeap,(LPSTR)(((LPINTNODE)lpNode) - 1) );

} /* hlsqListFreeNode() */

//*************************************************************
//
//  hlsqListInsertNode()
//
//  Purpose:
//		Inserts a node into the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList      - hlsqList to insert into
//      LPNODE lpPrevNode   - Node to insert AFTER!!!  If this paramter
//                            is hlsqList_INSERTHEAD or hlsqList_INSERTTAIL then
//                            that is where the node will go
//      LPNODE lpNode       - Node to insert
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

BOOL FAR PASCAL hlsqListInsertNode(HLIST hlstList, LPNODE lpNode, LPNODE lpPrevNode)
{
    LPINTNODE lpn;
    LPINTNODE lppn;

    if (!hlstList || !lpNode)
        return FALSE;

    lpn  = ((LPINTNODE)lpNode) - 1;

    lpn->lpNext = lpn->lpPrev = NULL;

    hlstList->lItems++;

    if (lpPrevNode == LIST_INSERTHEAD)  // Insert at the head of the List
    {
        if (hlstList->lpHead)                 
            hlstList->lpHead->lpPrev = lpn;
        else                                // Empty List
            hlstList->lpTail = lpn;

        lpn->lpNext = hlstList->lpHead;
        hlstList->lpHead = lpn;
        return TRUE;
    }

    // Insert at the tail of the hlsqList
    if (lpPrevNode==LIST_INSERTTAIL)
    {
        if (hlstList->lpTail)                 
            hlstList->lpTail->lpNext = lpn;
        else                                // Empty List
            hlstList->lpHead = lpn;

        lpn->lpPrev = hlstList->lpTail;
        hlstList->lpTail = lpn;
        return TRUE;
    }
    
    // Insert in the middle
    lppn = ((LPINTNODE)lpPrevNode) - 1;
    lpn->lpPrev = lppn;
    lpn->lpNext = lppn->lpNext;

    if (lppn->lpNext)                       
        (lppn->lpNext)->lpPrev = lpn;
    else                                    // At the tail anyway                                            
        hlstList->lpTail = lpn;

    lppn->lpNext = lpn;
    return TRUE;

} /* hlsqListInsertNode() */

//*************************************************************
//
//  hlsqListInsertSorted()
//
//  Purpose:
//		Inserts into the list in a sorted fashion
//
//
//  Parameters:
//      HLIST hlstList      - list
//      LPNODE lpNode       - node to insert
//      SORTPROC lpfnSort   - sorting function
//      LONG lData          - User defined data to pass to sort func
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//      lpfnSort( lpN1, lpN2, lUserData )
//      <0 if lpN1 < lpN2
//       0 if lpN1 = lpN2
//      >0 if lpN1 > lpN2
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

BOOL FAR PASCAL hlsqListInsertSorted(HLIST hlstList,LPNODE lpNode,SORTPROC lpfnSort,LONG lData)
{
    LPINTNODE lpn;
    LPINTNODE lpn2;

    if (!hlstList || !lpNode || !lpfnSort)
        return FALSE;

    lpn  = ((LPINTNODE)lpNode) - 1;

    lpn->lpNext = lpn->lpPrev = NULL;

    hlstList->lItems++;

    if (!(hlstList->lpHead))    // Empty list
    {
        hlstList->lpHead = lpn;
        hlstList->lpTail = lpn;
        return TRUE;
    }

// For speed make one check at the End Of the List
    if ( lpfnSort( (LPNODE)(hlstList->lpTail+1), (LPNODE)(lpn+1), lData)<0 )
    {
        lpn->lpPrev = hlstList->lpTail;
        hlstList->lpTail->lpNext = lpn;
        hlstList->lpTail = lpn;
        return TRUE;
    }


    lpn2 = hlstList->lpHead;

    while (lpn2)
    {
        if ( lpfnSort( (LPNODE)(lpn2+1), (LPNODE)(lpn+1), lData)>=0 )
        {
            lpn->lpNext = lpn2;
            lpn->lpPrev = lpn2->lpPrev;
            if (lpn2->lpPrev)
                lpn2->lpPrev->lpNext = lpn;
            else
                hlstList->lpHead = lpn;
            lpn2->lpPrev= lpn;
            return TRUE;
        }
        lpn2 = lpn2->lpNext;
    }
// Insert at the tail of the list
    hlstList->lpTail->lpNext = lpn;
    lpn->lpPrev = hlstList->lpTail;
    hlstList->lpTail = lpn;
    return TRUE;

} /* hlsqListInsertSorted() */

//*************************************************************
//
//  hlsqListAllocInsertNode()
//
//  Purpose:
//		Allocates a node, copies the data, then inserts it into the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList         - hlsqList to put the node into
//      LPSTR lpMem         - Memory pointer to copy, can be NULL
//      WORD wSize          - Size of node
//      LPNODE lpPrevNode   - Node to insert AFTER, or hlsqList_INSERTHEAD
//                                                     hlsqList_INSERTTAIL
//
//  Return: (LPNODE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//
//
//*************************************************************

LPNODE FAR PASCAL hlsqListAllocInsertNode( HLIST hlstList, LPSTR lpMem, WORD wSize, LPNODE lpPrevNode )
{
    LPNODE lpn = hlsqListAllocNode( hlstList, lpMem, wSize );

    if (hlstList->hhHeap->dwAllocs >= 0x26a)
    {
        LPNODE lpn2 = lpn;
        lpn = NULL; //Break here
        lpn = lpn2;
    }

    if (lpn)
        hlsqListInsertNode( hlstList, lpn, lpPrevNode );
    return lpn;

} /* hlsqListAllocInsertNode() */

//*************************************************************
//
//  hlsqListRemoveNode()
//
//  Purpose:
//		Removes a node from the List
//
//
//  Parameters:
//
//      HLIST hlstList     - List that the node is in
//      LPNODE lpNode      - Node to remove
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//                              
//*************************************************************

VOID FAR PASCAL hlsqListRemoveNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!hlstList || !lpNode)
        return;

    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpNext)
        (lpn->lpNext)->lpPrev = lpn->lpPrev;
    else
        hlstList->lpTail = lpn->lpPrev;

    if (lpn->lpPrev)
        (lpn->lpPrev)->lpNext = lpn->lpNext;
    else
        hlstList->lpHead = lpn->lpNext;

    lpn->lpPrev = lpn->lpNext = NULL;

    hlstList->lItems--;

} /* hlsqListRemoveNode() */

//*************************************************************
//
//  hlsqListDeleteNode()
//
//  Purpose:
//		Removes and then deletes the node from the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList     - hlsqList that the node is in
//      LPNODE lpNode   - Node to remove
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

VOID FAR PASCAL hlsqListDeleteNode( HLIST hlstList, LPNODE lpNode )
{
    if (!hlstList || !lpNode)
        return;

    hlsqListRemoveNode( hlstList, lpNode );
    hlsqListFreeNode( hlstList, lpNode );

} /* hlsqListDeleteNode() */

//*************************************************************
//
//  hlsqListNextNode()
//
//  Purpose:
//		Returns the next node in the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList     - The hlsqList
//      LPNODE lpNode   - The current node
//      
//
//  Return: (LPNODE FAR PASCAL)
//      The next node in the hlsqList
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL hlsqListNextNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!lpNode)
        return NULL;
    
    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpNext)
        return (LPNODE)(lpn->lpNext + 1);
    else
        return NULL;

} /* hlsqListNextNode() */

//*************************************************************
//
//  hlsqListPrevNode()
//
//  Purpose:
//		Returns the previous node in the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList     - The hlsqList
//      LPNODE lpNode   - The current node
//      
//
//  Return: (LPNODE FAR PASCAL)
//      The previous node in the hlsqList
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL hlsqListPrevNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!lpNode)
        return NULL;
    
    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpPrev)
        return (LPNODE)(lpn->lpPrev + 1);
    else
        return NULL;

} /* hlsqListPrevNode() */

//*************************************************************
//
//  hlsqListFirstNode()
//
//  Purpose:
//		Returns the first node in the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList
//      
//
//  Return: (LPNODE FAR PASCAL)
//      The first node or NULL if empty.
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL hlsqListFirstNode( HLIST hlstList )
{
    if (!hlstList)
        return NULL;

    if (hlstList->lpHead)
        return (LPNODE)(hlstList->lpHead + 1);
    else
        return NULL;

} /* hlsqListFirstNode() */

//*************************************************************
//
//  hlsqListLastNode()
//
//  Purpose:
//		Returns the hlsqList node in the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList
//      
//
//  Return: (LPNODE FAR PASCAL)
//      The last node or NULL if empty.
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL hlsqListLastNode( HLIST hlstList )
{
    if (!hlstList)
        return NULL;

    if (hlstList->lpTail)
        return (LPNODE)(hlstList->lpTail + 1);
    else
        return NULL;

} /* hlsqListLastNode() */

//*************************************************************
//
//  hlsqListNode2Item()
//
//  Purpose:
//		Returns the item index value of the node, base 0
//
//
//  Parameters:
//
//      HLIST hlstList     - The hlsqList
//      LPNODE lpNode      - The node
//      
//
//  Return: (LONG FAR PASCAL)
//      Item index, or -1 if error.  ZERO based!!!
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LONG FAR PASCAL hlsqListNode2Item(HLIST hlstList, LPNODE lpNode)
{
    LPINTNODE lpn;
    LONG      lItem=0;

    if (!hlstList || !lpNode)
        return -1;

    lpn = hlstList->lpHead;

    while( lpn )
    {
        if ( (LPNODE)(lpn+1)==lpNode )
            return lItem;
        lItem++;
        lpn = lpn->lpNext;
    }
    return -1L;

} /* hlsqListNode2Item() */

//*************************************************************
//
//  hlsqListItem2Node()
//
//  Purpose:
//		returns the node pointer of the index, base 0
//
//
//  Parameters:
//
//      HLIST hlstList - The hlsqList
//      LONG lItem  - The indexed item
//      
//
//  Return: (LPNODE FAR PASCAL)
//      Node pointer of hlsqList[lItem] or NULL of out of range
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL hlsqListItem2Node(HLIST hlstList, LONG lItem)
{
    LPINTNODE lpn;
    LONG      lItem2=0;

    if (!hlstList)
        return NULL;

    if (lItem>=hlstList->lItems || lItem<0)
        return NULL;

    if (lItem > ((hlstList->lItems)>>1) ) // scan from bottom is faster
    {
        lpn = hlstList->lpTail;

        for(lItem2=hlstList->lItems-1; lItem2>lItem; lItem2--)
        {
            if (!lpn)
                return NULL;
            lpn = lpn->lpPrev;
        }    
        return lpn;
    }

    lpn = hlstList->lpHead;

    for(lItem2=0; lItem2<lItem; lItem2++)
    {
        if (!lpn)
            return NULL;
        lpn = lpn->lpNext;
    }    
    return lpn;

} /* hlsqListItem2Node() */

//*************************************************************
//
//  hlsqListItems()
//
//  Purpose:
//		Returns the number of items in the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList
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

LONG FAR PASCAL hlsqListItems(HLIST hlstList)
{
    if (!hlstList)
        return 0;

    return hlstList->lItems;

} /* hlsqListItems() */

//*************************************************************
//
//  hlsqListSwapNodes()
//
//  Purpose:
//		Swaps two nodes in the hlsqList
//
//
//  Parameters:
//
//      HLIST hlstList - The hlsqList
//      LPNODE lp1  - Node1
//      LPNODE lp2  - Node2
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

VOID FAR PASCAL hlsqListSwapNodes( HLIST hlstList, LPNODE lp1, LPNODE lp2 )
{
    LPINTNODE t, ln1 = ((LPINTNODE)lp1) - 1;
    LPINTNODE ln2 = ((LPINTNODE)lp2) - 1;

    if (!hlstList || !lp1 || !lp2)
        return;

    ln1 = ((LPINTNODE)lp1) - 1;
    ln2 = ((LPINTNODE)lp2) - 1;

    
    if (hlstList->lpHead==ln1)    // Make sure and patch the head if nec.
        hlstList->lpHead=ln2;

    if (hlstList->lpHead==ln2)
        hlstList->lpHead=ln1;

    if (hlstList->lpTail==ln1)    // Make sure and patch the tail if nec.
        hlstList->lpTail=ln2;

    if (hlstList->lpTail==ln2)
        hlstList->lpTail=ln1;

    t = ln1->lpNext;
    ln1->lpNext = ln2->lpNext;
    ln2->lpNext = t;

    t = ln1->lpPrev;
    ln1->lpPrev = ln2->lpPrev;
    ln2->lpPrev = t;

} /* hlsqListSwapNodes() */

//*************************************************************
//
//  hlsqListSort()
//
//  Purpose:
//		Sorts the list
//
//
//  Parameters:
//      HLIST hlstList    - list
//      SORTPROC lpfnSort - compare proc
//      WORD wType        - type
//      LONG lData        - User defined data
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 5/91   MSM        Created
//
//*************************************************************

BOOL FAR PASCAL hlsqListSort(HLIST hlstList, SORTPROC lpfnSort, WORD wType, LONG lData)
{
    if (!hlstList || !lpfnSort)
        return FALSE;

    if (wType==LIST_SORT_BUBBLE)
    {
        LPINTNODE lpnC, lpnN;

        lpnC = hlstList->lpHead;

        while (lpnC)
        {
            lpnN = lpnC->lpNext;

            while (lpnN)
            {
                if ( lpfnSort((LPNODE)(lpnN+1),(LPNODE)(lpnC+1),lData)<0 )
                {
                    LPINTNODE lpT;
                    hlsqListSwapNodes( hlstList, (LPNODE)(lpnN+1),(LPNODE)(lpnC+1) );

                    lpT = lpnN;
                    lpnN = lpnC;
                    lpnC = lpT;
                }
                lpnN = lpnN->lpNext;
            }
            lpnC = lpnC->lpNext;
        }
        return TRUE;
    }
    return FALSE;

} /* hlsqListSort() */

//*************************************************************
//
//  hlsqListDumpDebug()
//
//  Purpose:
//		Dumps the list to the debug terminal
//
//
//  Parameters:
//      HLIST hlstList      - list to dump
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

VOID FAR PASCAL hlsqListDumpDebug(HLIST hlstList, DUMPPROC lpfnDump)
{
    char temp[80];
    LPINTNODE lpn;
    LONG      lNode=0;

    OutputDebugString( "\r\nList Manager V1.00 (Debug Info)\r\n" );
    OutputDebugString( "Written by Michael S. McCraw  Copyright @1992\r\n" );
    OutputDebugString("------------------------------------------------\r\n");

    if (!hlstList)
        return;

// Dump list header first
    wsprintf( temp, "  HLIST: %#08lx  Items: %ld  CreatedHeap: %s", hlstList,
        hlsqListItems(hlstList), (LPSTR)(hlstList->fCreatedHeap?"Yes":"No"));
    OutputDebugString( temp );    
    OutputDebugString( "\r\n" );    

// Now dump all the nodes
    lpn = hlstList->lpHead;
    while (lpn)
    {
        LPSTR lpUser=NULL;

        wsprintf( temp, "  Node#%-6d <%#08lx>  ", lNode, lpn );
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

} /* hlsqListDumpDebug() */

/*** EOF: hlsqList.c ***/

