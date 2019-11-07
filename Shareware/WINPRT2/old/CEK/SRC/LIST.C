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
#include "iCEK.h"

//*************************************************************
//
//  cekmmListHeap
//
//  Purpose:
//      Returns the heap being used by the list
//
//
//  Parameters:
//      HLIST hlstList
//      
//
//  Return: (HHEAP WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

HHEAP WINAPI cekmmListHeap(HLIST hlstList)
{
    if (!hlstList)
        return NULL;

    return hlstList->hhHeap;

} //*** cekmmListHeap

//*************************************************************
//
//  cekmmListCreate()
//
//  Purpose:
//		Creates a linked list object
//
//
//  Parameters:
//      HHEAP hHeap - heap to use [optional]
//      
//
//  Return: (HLIST WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************
                                       
HLIST WINAPI cekmmListCreate( HHEAP hhHeap )
{
    HLIST  hlstList;
    BOOL   fCreatedHeap = FALSE;

    if (!hhHeap)
    {
       hhHeap = cekmmHeapCreate(2048, HT_MULTIPLE);
        if (!hhHeap)
            return NULL;
        fCreatedHeap = TRUE;
    }

    if (hlstList = (HLIST)cekmmHeapAllocLock(hhHeap, LPTR, sizeof( LIST )) )
    {
        hlstList->hhHeap =hhHeap;
        hlstList->fCreatedHeap = fCreatedHeap;
        hlstList->lItems = 0L;
        hlstList->lpHead = hlstList->lpTail = NULL;
    }
    else
    {
        if (fCreatedHeap)
            cekmmHeapDestroy(hhHeap);
    }

    return hlstList;

} /* cekmmListCreate() */

//*************************************************************
//
//  cekmmListDestroy()
//
//  Purpose:
//		Destroys a list object
//
//
//  Parameters:
//      HLIST hlstList - list to destroy
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

VOID WINAPI cekmmListDestroy(HLIST hlstList)
{
    if (!hlstList)
        return;

    if (hlstList->fCreatedHeap)
        cekmmHeapDestroy( hlstList->hhHeap );
    else
    {
        cekmmListFreeList( hlstList );
        cekmmHeapUnlockFree( hlstList->hhHeap, (LPVOID)hlstList );
    }

} /* cekmmListDestroy() */

//*************************************************************
//
//  cekmmListFreeList()
//
//  Purpose:
//		Free all the elements in the list, but not the list itself
//
//
//  Parameters:
//      HLIST hlstList
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

VOID WINAPI cekmmListFreeList(HLIST hlstList)
{
    if (!hlstList)
        return;

    while (hlstList->lpHead)
    {
        LPNODE lpNode = (LPNODE)(hlstList->lpHead + 1);

        cekmmListRemoveNode( hlstList, lpNode );
        cekmmListFreeNode( hlstList, lpNode );
    }

} /* cekmmListFreeList() */

//*************************************************************
//
//  cekmmListAllocNode()
//
//  Purpose:
//		Allocates a node that can be used in a list object
//
//
//  Parameters:
//      HLIST hlstList - list to alloc for
//      LPVOID lpMem    - optional memory to copy into node
//      UINT uiSize     - size of node
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

LPNODE WINAPI cekmmListAllocNode(HLIST hlstList, LPVOID lpMem, UINT uiSize)
{
    LPINTNODE lpn;

    if (!hlstList)
        return NULL;

    lpn = (LPINTNODE)cekmmHeapAllocLock(hlstList->hhHeap,LPTR, uiSize+sizeof(INTNODE));
    if (lpn )
    {
        lpn->lpPrev = lpn->lpNext = NULL;
        if (lpMem)
            _fmemcpy( (LPSTR)(lpn+1), lpMem, uiSize );
        return (LPNODE)(lpn+1);
    }
    return NULL;

} /* cekmmListAllocNode() */

//*************************************************************
//
//  cekmmListFreeNode()
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
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

VOID WINAPI cekmmListFreeNode( HLIST hlstList, LPNODE lpNode )
{
    if (!hlstList)
        return;

    cekmmHeapUnlockFree(hlstList->hhHeap,(LPVOID)(((LPINTNODE)lpNode) - 1) );

} /* cekmmListFreeNode() */

//*************************************************************
//
//  cekmmListInsertNode()
//
//  Purpose:
//		Inserts a node into the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList      - cekmmList to insert into
//      LPNODE lpPrevNode   - Node to insert AFTER!!!  If this paramter
//                            is cekmmList_INSERTHEAD or cekmmList_INSERTTAIL then
//                            that is where the node will go
//      LPNODE lpNode       - Node to insert
//
//  Return: (BOOL WINAPI)
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

BOOL WINAPI cekmmListInsertNode(HLIST hlstList, LPNODE lpNode, LPNODE lpPrevNode)
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

    // Insert at the tail of the cekmmList
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

} /* cekmmListInsertNode() */

//*************************************************************
//
//  cekmmListInsertSorted()
//
//  Purpose:
//		Inserts into the list in a sorted fashion
//
//
//  Parameters:
//      HLIST hlstList      - list
//      LPNODE lpNode       - node to insert
//      SORTPROC lpfnSort   - sorting function
//      LPARAM lData          - User defined data to pass to sort func
//      
//
//  Return: (BOOL WINAPI)
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

BOOL WINAPI cekmmListInsertSorted(HLIST hlstList,LPNODE lpNode,SORTPROC lpfnSort,LPARAM lData)
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

} /* cekmmListInsertSorted() */

//*************************************************************
//
//  cekmmListAllocInsertNode()
//
//  Purpose:
//		Allocates a node, copies the data, then inserts it into the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList         - cekmmList to put the node into
//      LPVOID lpMem         - Memory pointer to copy, can be NULL
//      UINT uiSize          - Size of node
//      LPNODE lpPrevNode   - Node to insert AFTER, or cekmmList_INSERTHEAD
//                                                     cekmmList_INSERTTAIL
//
//  Return: (LPNODE WINAPI)
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

LPNODE WINAPI cekmmListAllocInsertNode( HLIST hlstList, LPVOID lpMem, UINT uiSize, LPNODE lpPrevNode )
{
    LPNODE lpn = cekmmListAllocNode( hlstList, lpMem, uiSize );

    if (lpn)
        cekmmListInsertNode( hlstList, lpn, lpPrevNode );
    return lpn;

} /* cekmmListAllocInsertNode() */

//*************************************************************
//
//  cekmmListRemoveNode()
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
//  Return: (VOID WINAPI)
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

VOID WINAPI cekmmListRemoveNode( HLIST hlstList, LPNODE lpNode )
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

} /* cekmmListRemoveNode() */

//*************************************************************
//
//  cekmmListDeleteNode()
//
//  Purpose:
//		Removes and then deletes the node from the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList     - cekmmList that the node is in
//      LPNODE lpNode   - Node to remove
//      
//
//  Return: (VOID WINAPI)
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

VOID WINAPI cekmmListDeleteNode( HLIST hlstList, LPNODE lpNode )
{
    if (!hlstList || !lpNode)
        return;

    cekmmListRemoveNode( hlstList, lpNode );
    cekmmListFreeNode( hlstList, lpNode );

} /* cekmmListDeleteNode() */

//*************************************************************
//
//  cekmmListNextNode()
//
//  Purpose:
//		Returns the next node in the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList     - The cekmmList
//      LPNODE lpNode   - The current node
//      
//
//  Return: (LPNODE WINAPI)
//      The next node in the cekmmList
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE WINAPI cekmmListNextNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!lpNode)
        return NULL;
    
    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpNext)
        return (LPNODE)(lpn->lpNext + 1);
    else
        return NULL;

} /* cekmmListNextNode() */

//*************************************************************
//
//  cekmmListPrevNode()
//
//  Purpose:
//		Returns the previous node in the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList     - The cekmmList
//      LPNODE lpNode   - The current node
//      
//
//  Return: (LPNODE WINAPI)
//      The previous node in the cekmmList
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE WINAPI cekmmListPrevNode( HLIST hlstList, LPNODE lpNode )
{
    LPINTNODE lpn;

    if (!lpNode)
        return NULL;
    
    lpn = ((LPINTNODE)lpNode) - 1;

    if (lpn->lpPrev)
        return (LPNODE)(lpn->lpPrev + 1);
    else
        return NULL;

} /* cekmmListPrevNode() */

//*************************************************************
//
//  cekmmListFirstNode()
//
//  Purpose:
//		Returns the first node in the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList
//      
//
//  Return: (LPNODE WINAPI)
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

LPNODE WINAPI cekmmListFirstNode( HLIST hlstList )
{
    if (!hlstList)
        return NULL;

    if (hlstList->lpHead)
        return (LPNODE)(hlstList->lpHead + 1);
    else
        return NULL;

} /* cekmmListFirstNode() */

//*************************************************************
//
//  cekmmListLastNode()
//
//  Purpose:
//		Returns the cekmmList node in the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList
//      
//
//  Return: (LPNODE WINAPI)
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

LPNODE WINAPI cekmmListLastNode( HLIST hlstList )
{
    if (!hlstList)
        return NULL;

    if (hlstList->lpTail)
        return (LPNODE)(hlstList->lpTail + 1);
    else
        return NULL;

} /* cekmmListLastNode() */

//*************************************************************
//
//  cekmmListNode2Item()
//
//  Purpose:
//		Returns the item index value of the node, base 0
//
//
//  Parameters:
//
//      HLIST hlstList     - The cekmmList
//      LPNODE lpNode      - The node
//      
//
//  Return: (DWORD WINAPI)
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

DWORD WINAPI cekmmListNode2Item(HLIST hlstList, LPNODE lpNode)
{
    LPINTNODE lpn;
    DWORD      lItem=0;

    if (!hlstList || !lpNode)
        return (DWORD)-1;

    lpn = hlstList->lpHead;

    while( lpn )
    {
        if ( (LPNODE)(lpn+1)==lpNode )
            return lItem;
        lItem++;
        lpn = lpn->lpNext;
    }
    return (DWORD)-1L;

} /* cekmmListNode2Item() */

//*************************************************************
//
//  cekmmListItem2Node()
//
//  Purpose:
//		returns the node pointer of the index, base 0
//
//
//  Parameters:
//
//      HLIST hlstList - The cekmmList
//      DWORD lItem  - The indexed item
//      
//
//  Return: (LPNODE WINAPI)
//      Node pointer of cekmmList[lItem] or NULL of out of range
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              10/19/91   MSM        Created
//              12/ 4/91   MSM        Updated
//
//*************************************************************

LPNODE WINAPI cekmmListItem2Node(HLIST hlstList, DWORD lItem)
{
    LPINTNODE lpn;
    DWORD      lItem2=0;

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

} /* cekmmListItem2Node() */

//*************************************************************
//
//  cekmmListItems()
//
//  Purpose:
//		Returns the number of items in the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList
//      
//
//  Return: (DWORD WINAPI)
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

DWORD WINAPI cekmmListItems(HLIST hlstList)
{
    if (!hlstList)
        return 0;

    return hlstList->lItems;

} /* cekmmListItems() */

//*************************************************************
//
//  cekmmListSwapNodes()
//
//  Purpose:
//		Swaps two nodes in the cekmmList
//
//
//  Parameters:
//
//      HLIST hlstList - The cekmmList
//      LPNODE lp1  - Node1
//      LPNODE lp2  - Node2
//      
//
//  Return: (VOID WINAPI)
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

VOID WINAPI cekmmListSwapNodes( HLIST hlstList, LPNODE lp1, LPNODE lp2 )
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

} /* cekmmListSwapNodes() */

//*************************************************************
//
//  cekmmListSort()
//
//  Purpose:
//		Sorts the list
//
//
//  Parameters:
//      HLIST hlstList    - list
//      SORTPROC lpfnSort - compare proc
//      UINT wType        - type
//      LPARAM lData        - User defined data
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 5/91   MSM        Created
//
//*************************************************************

BOOL WINAPI cekmmListSort(HLIST hlstList, SORTPROC lpfnSort, UINT wType, LPARAM lData)
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
                    cekmmListSwapNodes( hlstList, (LPNODE)(lpnN+1),(LPNODE)(lpnC+1) );

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

} /* cekmmListSort() */

//*************************************************************
//
//  cekmmListDumpDebug()
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

VOID WINAPI cekmmListDumpDebug(HLIST hlstList, DUMPPROC lpfnDump)
{
    char temp[80];
    LPINTNODE lpn;
    DWORD      lNode=0;

    OutputDebugString( "\r\nList Manager V1.00 (Debug Info)\r\n" );
    OutputDebugString( "Written by Michael S. McCraw  Copyright @1992\r\n" );
    OutputDebugString("------------------------------------------------\r\n");

    if (!hlstList)
        return;

// Dump list header first
    wsprintf( temp, "  HLIST: %#08lx  Items: %ld  CreatedHeap: %s", hlstList,
        cekmmListItems(hlstList), (LPSTR)(hlstList->fCreatedHeap?"Yes":"No"));
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

} /* cekmmListDumpDebug() */

/*** EOF: cekmmList.c ***/

