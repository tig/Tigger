//*************************************************************
//  File name: fsmmstr.h
//
//  Description: 
//      Header file for (Heap), (L)ist, (S)tack, (Q)ueue, (Str)ing memory DLL
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#ifndef __FSMEMMAN
#define __FSMEMMAN

#ifndef PASCAL
#include <windows.h>
#endif

#define FSMEMMAN_VER     0x0100

WORD FAR PASCAL fsmmCheckVersion( WORD );

/************************* Heap Services ******************************/
// Hungarian is hh (ie.  hhHeap)
typedef DWORD HHEAP;
typedef DWORD HHANDLE;

#define HT_SHARED    0x0001
#define HT_MULTIPLE  0x0002

extern HHEAP    FAR PASCAL fsmmHeapCreate(WORD wSize, WORD wFlags);
extern VOID     FAR PASCAL fsmmHeapDestroy(HHEAP);
extern HHANDLE  FAR PASCAL fsmmHeapAlloc(HHEAP, WORD, WORD);
extern HHANDLE  FAR PASCAL fsmmHeapReAlloc(HHEAP,HHANDLE,WORD,WORD);
extern VOID     FAR PASCAL fsmmHeapFree(HHEAP, HHANDLE);
extern LPSTR    FAR PASCAL fsmmHeapLock(HHEAP, HHANDLE);
extern BOOL     FAR PASCAL fsmmHeapUnlock(HHEAP, HHANDLE);
extern LPSTR    FAR PASCAL fsmmHeapAllocLock(HHEAP, WORD, WORD);
                        // Allocs with LPTR
extern LPSTR    FAR PASCAL fsmmHeapAllocString(HHEAP, WORD,LPSTR);
                        // Allocs with LPTR
extern VOID     FAR PASCAL fsmmHeapUnlockFree(HHEAP, LPSTR);
extern WORD     FAR PASCAL fsmmHeapCompact(HHEAP hhHeap, WORD wMinFree);
extern WORD     FAR PASCAL fsmmHeapFlags(HHEAP, HHANDLE);
extern HHANDLE  FAR PASCAL fsmmHeapHandle(HHEAP hhHeap, LPVOID lpMem);
extern HHANDLE  FAR PASCAL fsmmHeapDiscard(HHEAP,HHANDLE);
extern WORD     FAR PASCAL fsmmHeapSize(HHEAP, HHANDLE);
extern HHANDLE  FAR PASCAL fsmmHeapSetDS(HHEAP);
extern DWORD    FAR PASCAL fsmmHeapItems(HHEAP);
extern VOID     FAR PASCAL fsmmHeapDumpDebug(HHEAP);


/************************** List Services ******************************/
// Hungarian is hlst (ie.  hlstList)
typedef  DWORD  HLIST;

typedef void far *LPNODE;
typedef int (FAR PASCAL *SORTPROC)(LPNODE,LPNODE,LONG);
typedef LPSTR (FAR PASCAL *DUMPPROC)(LPNODE);

#define LIST_INSERTHEAD     ((LPNODE)0L)
#define LIST_INSERTTAIL     ((LPNODE)1L)         
#define LIST_SORT_BUBBLE    0x0000

extern HHEAP FAR PASCAL fsmmListHeap(HLIST);

extern HLIST  FAR PASCAL fsmmListCreate(HHEAP);
extern VOID   FAR PASCAL fsmmListDestroy(HLIST);
extern VOID   FAR PASCAL fsmmListFreeList(HLIST);
extern LPNODE FAR PASCAL fsmmListAllocNode(HLIST, LPSTR, WORD);
extern VOID   FAR PASCAL fsmmListFreeNode(HLIST, LPNODE); 
extern BOOL   FAR PASCAL fsmmListInsertNode(HLIST,LPNODE,LPNODE);
   // The 2nd LPNODE is the node to insert AFTER or LIST_INSERT?????
extern BOOL   FAR PASCAL fsmmListInsertSorted(HLIST,LPNODE,SORTPROC,LONG);
   // The LONG is user defined data it can be whatever and just get passed on
extern LPNODE FAR PASCAL fsmmListAllocInsertNode(HLIST,LPSTR,WORD,LPNODE);
   // The LPNODE is the node to insert AFTER or LIST_INSERT?????
extern VOID   FAR PASCAL fsmmListRemoveNode(HLIST, LPNODE);
extern VOID   FAR PASCAL fsmmListDeleteNode(HLIST,LPNODE);
extern LPNODE FAR PASCAL fsmmListNextNode(HLIST, LPNODE);
extern LPNODE FAR PASCAL fsmmListPrevNode(HLIST, LPNODE);
extern LPNODE FAR PASCAL fsmmListFirstNode(HLIST);
extern LPNODE FAR PASCAL fsmmListLastNode(HLIST);
extern LONG   FAR PASCAL fsmmListNode2Item(HLIST, LPNODE);
extern LPNODE FAR PASCAL fsmmListItem2Node(HLIST hlstList, LONG lItem); 
    // lItem is zero based!!
extern LONG   FAR PASCAL fsmmListItems(HLIST);
extern VOID   FAR PASCAL fsmmListSwapNodes(HLIST,LPNODE,LPNODE);
extern BOOL   FAR PASCAL fsmmListSort(HLIST,SORTPROC,WORD,LONG);
    // Word is the SORT TYPE, LONG is user data
extern VOID   FAR PASCAL fsmmListDumpDebug(HLIST,DUMPPROC);


/************************** Stack Services *****************************/
// Hungarian is hstk (ie.  hstkStack)
typedef  DWORD  HSTACK;

extern HSTACK   FAR PASCAL fsmmStackCreate(HHEAP);
extern VOID     FAR PASCAL fsmmStackDestroy(HSTACK);
extern VOID     FAR PASCAL fsmmStackFreeStack(HSTACK);
extern LPNODE   FAR PASCAL fsmmStackAllocNode(HSTACK,LPSTR,WORD);
extern LPNODE   FAR PASCAL fsmmStackFreeNode(HSTACK, LPNODE);
extern BOOL     FAR PASCAL fsmmStackPush(HSTACK, LPNODE);
extern LPNODE   FAR PASCAL fsmmStackAllocPush(HSTACK,LPSTR,WORD);

extern LPNODE   FAR PASCAL fsmmStackPop(HSTACK);
extern LPNODE   FAR PASCAL fsmmStackPeek(HSTACK hstkStack, LONG lIndex);
   // Zero based from the top (Peek(0) == Pop() without removing)
extern LPNODE   FAR PASCAL fsmmStackRemove(HSTACK hstkStack, LONG lndex);
   // Zero based from the top (Remove(0) == Pop())
extern LONG     FAR PASCAL fsmmStackItems(HSTACK);
extern VOID     FAR PASCAL fsmmStackDumpDebug(HSTACK,DUMPPROC);


/************************* Queue Services *****************************/
// Hungarian is hs (ie.  hqQueue)
typedef  DWORD  HQUEUE;

extern HQUEUE   FAR PASCAL fsmmQueueCreate(HHEAP);
extern VOID     FAR PASCAL fsmmQueueDestroy(HQUEUE);
extern VOID     FAR PASCAL fsmmQueueFreeQueue(HQUEUE);
extern LPNODE   FAR PASCAL fsmmQueueAllocNode(HQUEUE,LPSTR,WORD);
extern LPNODE   FAR PASCAL fsmmQueueFreeNode(HQUEUE, LPNODE);
extern BOOL     FAR PASCAL fsmmQueueInQ(HQUEUE, LPNODE);
extern LPNODE   FAR PASCAL fsmmQueueAllocInQ(HQUEUE, LPSTR, WORD);
extern LPNODE   FAR PASCAL fsmmQueueDeQ(HQUEUE);
extern LPNODE   FAR PASCAL fsmmQueuePeek(HQUEUE hqQueue, LONG lIndex);
   // Zero based from the top (Peek(0) == DeQ() without removing)
extern LPNODE   FAR PASCAL fsmmQueueRemove(HQUEUE hqQueue, LONG lIndex);
   // Zero based from the top (Remove(0) == DeQ())
extern LONG     FAR PASCAL fsmmQueueItems(HQUEUE);
extern VOID     FAR PASCAL fsmmQueueDumpDebug(HQUEUE,DUMPPROC);


/************************** Atom Services *****************************/
// Hungarian is hat (ie.  hatAtomTable)
typedef DWORD HATBL;

// Hungarian is ha (ie.   haAtom)
typedef DWORD HATOM;

extern HATBL    FAR PASCAL fsmmAtomTableCreate(WORD wInitSize);
extern VOID     FAR PASCAL fsmmAtomTableDestroy(HATBL );
extern HATOM    FAR PASCAL fsmmAtomTableAddAtom( HATBL, LPSTR );
extern HATOM    FAR PASCAL fsmmAtomTableDeleteAtom( HATBL, HATOM );
extern HATOM    FAR PASCAL fsmmAtomTableFindAtom( HATBL, LPSTR );
extern WORD     FAR PASCAL fsmmAtomTableGetAtomName( HATBL, HATOM, LPSTR, WORD );
extern VOID     FAR PASCAL fsmmAtomTableDumpDebug(HATBL);


/************************ String Services *****************************/

extern BOOL FAR PASCAL fsmmIsUsefulString( LPSTR lpText );
extern BOOL FAR PASCAL fsmmIsNumber( LPSTR lpS );
extern VOID FAR PASCAL fsmmSetExtension( LPSTR lpFile, LPSTR lpExt );
extern void FAR PASCAL fsmmHackSpaces( LPSTR lpszLine );
extern LONG FAR PASCAL fsmmGetValue(LPSTR FAR *lplp);

#endif

/*** EOF: fsmmstr.h ***/
