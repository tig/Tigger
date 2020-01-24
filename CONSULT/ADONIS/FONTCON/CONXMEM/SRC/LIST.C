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

#include "PRECOMP.H"
#include "intfsmm.h"

//*************************************************************
//
//  fsmmListHeap
//
//  Purpose:
//      Returns the heap being used by the list
//
//
//  Parameters:
//      HLIST hlstList
//      
//
//  Return: (HHEAP FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

HHEAP FAR PASCAL fsmmListHeap(HLIST hlstList)
{
    if (!hlstList)
        return NULL;

    return hlstList->hhHeap;

} //*** fsmmListHeap

//*************************************************************
//
//  fsmmListCreate()
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
                                       
HLIST FAR PASCAL fsmmListCreate( HHEAP hhHeap )
{
    HLIST  hlstList;
    WORD   fCreatedHeap = FALSE;

    if (!hhHeap)
    {
       hhHeap = fsmmHeapCreate(2048, HT_MULTIPLE);
        if (!hhHeap)
            return NULL;
        fCreatedHeap = TRUE;
    }

    if (hlstList = (HLIST)fsmmHeapAllocLock(hhHeap, LPTR, sizeof( LIST )) )
    {
        hlstList->hhHeap =hhHeap;
        hlstList->fCreatedHeap = fCreatedHeap;
        hlstList->lItems = 0L;
        hlstList->lpHead = hlstList->lpTail = NULL;
    }
    else
    {
        if (fCreatedHeap)
            fsmmHeapDestroy(hhHeap);
    }

    return hlstList;

} /* fsmmListCreate() */

//*************************************************************
//
//  fsmmListDestroy()
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

VOID FAR PASCAL fsmmListDestroy(HLIST hlstList)
{
    if (!hlstList)
        return;

    if (hlstList->fCreatedHeap)
        fsmmHeapDestroy( hlstList->hhHeap );
    else
    {
        fsmmListFreeList( hlstList );
        fsmmHeapUnlockFree( hlstList->hhHeap, (LPSTR)hlstList );
    }

} /* fsmmListDestroy() */

//*************************************************************
//
//  fsmmListFreeList()
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

VOID FAR PASCAL fsmmListFreeList(HLIST hlstList)
{
    if (!hlstList)
        return;

    while (hlstList->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hlstList->lpHead + 1);

        fsmmListRemoveNode( hlstList, lpNode );
        fsmmListFreeNode( hlstList, lpNode );
    }

} /* fsmmListFreeList() */

//*************************************************************
//
//  fsmmListAllocNode()
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

LPNODE FAR PASCAL fsmmListAllocNode(HLIST hlstList, LPSTR lpMem, WORD wSize)
{
    LPINTNODE lpn;

    if (!hlstList)
        return NULL;

    lpn = (LPINTNODE)fsmmHeapAllocLock(hlstList->hhHeap,LPTR, wSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, wSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* fsmmListAllocNode() */

//*************************************************************
//
//  fsmmListFreeNode()
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

VOID FAR PASCAL fsmmListFreeNode( HLIST hlstList, LPNODE lpNode )
{
    if (!hlstList)
        return;

    fsmmHeapUnlockFree(hlstList->hhHeap,(LPSTR)(((LPINTNODE)lpNode) - 1) );

} /* fsmmListFreeNode() */

//*************************************************************
//
//  fsmmListInsertNode()
//
//  Purpose:
//		Inserts a node into the fsmmList
//
//
//  Parameters:
//
//      HLIST hlstList      - fsmmList to insert into
//      LPNODE lpPrevNode   - Node to insert AFTER!!!  If this paramter
//                            is fsmmList_INSERTHEAD or fsmmList_INSERTTAIL then
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

BOOL FAR PASCAL fsmmListInsertNode(HLIST hlstList, LPNODE lpNode, LPNODE lpPrevNode)
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

    // Insert at the tail of the fsmmList
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

} /* fsmmListInsertNode() */

//*************************************************************
//
//  fsmmListInsertSorted()
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

BOOL FAR PASCAL fsmmListInsertSorted(HLIST hlstList,LPNODE lpNode,SORTPROC lpfnSort,LONG lData)
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

} /* fsmmListInsertSorted() */

//*************************************************************
//
//  fsmmListAllocInsertNode()
//
//  Purpose:
//		Allocates a node, copies the data, then inserts it into the fsmmList
//
//
//  Parameters:
//
//      HLIST hlstList         - fsmmList to put the node into
//      LPSTR lpMem         - Memory pointer to copy, can be NULL
//      WORD wSize          - Size of node
//      LPNODE lpPrevNode   - Node to insert AFTER, or fsmmList_INSERTHEAD
//                                                     fsmmList_INSERTTAIL
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

LPNODE FAR PASCAL fsmmListAllocInsertNode( HLIST hlstList, LPSTR lpMem, WORD wSize, LPNODE lpPrevNode )
{
    LPNODE lpn = fsmmListAllocNode( hlstList, lpMem, wSize );

    if (lpn)
        fsmmListInsertNode( hlstList, lpn, lpPrevNode );
    return lpn;

} /* fsmmListAllocInsertNode() */

//*************************************************************
//
//  fsmmListRemoveNode()
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

VOID FAR PASCAL fsmmListRemoveNode( HLIST hlstList, LPNODE lpNode )
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

} /* fsmmListRemoveNode() */

//*************************************************************
//
//  fsmmListDeleteNode()
//
//  Purpose:
//		Removes and then deletes the node from the fsmmList
//
//
//  Parameters:
//
//      HLIST hlstList     - fsmmList that the node is in
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

VOID FAR PASCAL fsmmListDeleteNode( HLIST hlstList, LPNODE lpNode )
{
    if (!hlstList || !lpNode)
        return;

    fsmmListRemoveNode( hlstList, lpNode );
    fsmmListFreeNode( hlstList, lpNode );

} /* fsmmListDeleteNode() */

//*************************************************************
//
//  fsmmListNextNode()
//
//  Purpose:
//		Returns the next node in the fsmmList
//
//
//  Parameters:
//
//      HLIST hlstList     - The fsmmList
//      LPNODE lpNode   - The current node
//      
//
//  Return: (LPNODE FAR PASCAL)
//      The next node in the fsmmList
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL fsmmListNextNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!lpNode)
        return NULL;
    
    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpNext)
        return (LPNODE)(lpn->lpNext + 1);
    else
        return NULL;

} /* fsmmListNextNode() */

//*************************************************************
//
//  fsmmListPrevNode()
//
//  Purpose:
//		Returns the previous node in the fsmmList
//
//
//  Parameters:
//
//      HLIST hlstList     - The fsmmList
//      LPNODE lpNode   - The current node
//      
//
//  Return: (LPNODE FAR PASCAL)
//      The previous node in the fsmmList
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL fsmmListPrevNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!lpNode)
        return NULL;
    
    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpPrev)
        return (LPNODE)(lpn->lpPrev + 1);
    else
        return NULL;

} /* fsmmListPrevNode() */

//*************************************************************
//
//  fsmmListFirstNode()
//
//  Purpose:
//		Returns the first node in the fsmmList
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

LPNODE FAR PASCAL fsmmListFirstNode( HLIST hlstList )
{
    if (!hlstList)
        return NULL;

    if (hlstList->lpHead)
        return (LPNODE)(hlstList->lpHead + 1);
    else
        return NULL;

} /* fsmmListFirstNode() */

//*************************************************************
//
//  fsmmListLastNode()
//
//  Purpose:
//		Returns the fsmmList node in the fsmmList
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

LPNODE FAR PASCAL fsmmListLastNode( HLIST hlstList )
{
    if (!hlstList)
        return NULL;

    if (hlstList->lpTail)
        return (LPNODE)(hlstList->lpTail + 1);
    else
        return NULL;

} /* fsmmListLastNode() */

//*************************************************************
//
//  fsmmListNode2Item()
//
//  Purpose:
//		Returns the item index value of the node, base 0
//
//
//  Parameters:
//
//      HLIST hlstList     - The fsmmList
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

LONG FAR PASCAL fsmmListNode2Item(HLIST hlstList, LPNODE lpNode)
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

} /* fsmmListNode2Item() */

//*************************************************************
//
//  fsmmListItem2Node()
//
//  Purpose:
//		returns the node pointer of the index, base 0
//
//
//  Parameters:
//
//      HLIST hlstList - The fsmmList
//      LONG lItem  - The indexed item
//      
//
//  Return: (LPNODE FAR PASCAL)
//      Node pointer of fsmmList[lItem] or NULL of out of range
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE FAR PASCAL fsmmListItem2Node(HLIST hlstList, LONG lItem)
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
        return (LPNODE)(lpn+1);
    }

    lpn = hlstList->lpHead;

    for(lItem2=0; lItem2<lItem; lItem2++)
    {
        if (!lpn)
            return NULL;
        lpn = lpn->lpNext;
    }    
    return (LPNODE)(lpn+1);

} /* fsmmListItem2Node() */

//*************************************************************
//
//  fsmmListItems()
//
//  Purpose:
//		Returns the number of items in the fsmmList
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

LONG FAR PASCAL fsmmListItems(HLIST hlstList)
{
    if (!hlstList)
        return 0;

    return hlstList->lItems;

} /* fsmmListItems() */

//*************************************************************
//
//  fsmmListSwapNodes()
//
//  Purpose:
//		Swaps two nodes in the fsmmList
//
//
//  Parameters:
//
//      HLIST hlstList - The fsmmList
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

VOID FAR PASCAL fsmmListSwapNodes( HLIST hlstList, LPNODE lp1, LPNODE lp2 )
{
    LPINTNODE t, ln1;
    LPINTNODE ln2;

    if (!hlstList || !lp1 || !lp2)
        return;

    ln1 = ((LPINTNODE)lp1) - 1;
    ln2 = ((LPINTNODE)lp2) - 1;

    
    if (hlstList->lpHead==ln1)    // Make sure and patch the head if nec.
        hlstList->lpHead=ln2;
    else
        if (hlstList->lpHead==ln2)
            hlstList->lpHead=ln1;

    if (hlstList->lpTail==ln1)    // Make sure and patch the tail if nec.
        hlstList->lpTail=ln2;
    else
        if (hlstList->lpTail==ln2)
            hlstList->lpTail=ln1;



    //*** If next to one another in the list
    if (ln1->lpNext==ln2)
    {
swap_next:
        if (ln1->lpPrev)
            ln1->lpPrev->lpNext = ln2;

        if (ln2->lpNext)
            ln2->lpNext->lpPrev = ln1;

        ln1->lpNext = ln2->lpNext;
        ln2->lpNext = ln1;
        ln2->lpPrev = ln1->lpPrev;
        ln1->lpPrev = ln2;


        return;
    }
    else
    {
        //*** If next to one another in the list
        if (ln1->lpPrev==ln2)
        {
            LPNODE lT = ln1;

            ln1=ln2;
            ln2=lT;
            goto swap_next;
            return;
        }
    }

    //*** Otherwise patch up nodes to the left and right of each swapped node
    if (ln1->lpNext)
        ln1->lpNext->lpPrev = ln2;

    if (ln2->lpNext)
        ln2->lpNext->lpPrev = ln1;

    if (ln1->lpPrev)
        ln1->lpPrev->lpNext = ln2;

    if (ln2->lpPrev)
        ln2->lpPrev->lpNext = ln1;

    t = ln1->lpNext;
    ln1->lpNext = ln2->lpNext;
    ln2->lpNext = t;

    t = ln1->lpPrev;
    ln1->lpPrev = ln2->lpPrev;
    ln2->lpPrev = t;

} /* fsmmListSwapNodes() */

//*************************************************************
//
//  fsmmListSort()
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

BOOL FAR PASCAL fsmmListSort(HLIST hlstList, SORTPROC lpfnSort, WORD wType, LONG lData)
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
                    fsmmListSwapNodes( hlstList, (LPNODE)(lpnN+1),(LPNODE)(lpnC+1) );

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

} /* fsmmListSort() */

//*************************************************************
//
//  fsmmListDumpDebug()
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

VOID FAR PASCAL fsmmListDumpDebug(HLIST hlstList, DUMPPROC lpfnDump)
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
        fsmmListItems(hlstList), (LPSTR)(hlstList->fCreatedHeap?"Yes":"No"));
    OutputDebugString( temp );    
    OutputDebugString( "\r\n" );    

// Now dump all the nodes
    lpn = hlstList->lpHead;
    while (lpn)
    {
        LPSTR lpUser=NULL;

        if (lpfnDump)
            lpUser = lpfnDump( (LPNODE)(lpn+1) );

        if (lpUser)
        {
            wsprintf( temp, "  Node#%-6d <%#08lx>  ", lNode, lpn );
            OutputDebugString( temp );
            OutputDebugString( lpUser );
            OutputDebugString( "\r\n" );    
        }

        lpn = lpn->lpNext;
        lNode++;
    }
    OutputDebugString( "\r\n" );    

} /* fsmmListDumpDebug() */

/*** EOF: fsmmList.c ***/

