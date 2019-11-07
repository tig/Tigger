/************************************************************************
 *
 *      Module:  ICEK.H
 *
 *     Remarks:   Internal
 *
 *   Revisions:  
 *
 ***********************************************************************/

#ifndef _ICEK_H_
#define _ICEK_H_

#include "..\res\cek.str"

#ifndef _MEMMAN_H_
#define _MEMMAN_H_

#ifdef WIN32
typedef struct tgHeap
{
    HANDLE              hHeap ;     // points to itself
    UINT                wType;
    DWORD               dwAllocs;
    DWORD               dwAllocated ;
} HEAP, FAR *HHEAP;
#else
typedef struct tgHeap
{
    struct tgHeap FAR  *hhNextHeap;
    UINT                wType;
    DWORD               dwAllocs;
} HEAP, FAR *HHEAP;
#endif

typedef struct tgINTNODE {
    struct tgINTNODE FAR *lpNext;
    struct tgINTNODE FAR *lpPrev;
} INTNODE, FAR *LPINTNODE;

typedef struct
{
    HHEAP       hhHeap;
    BOOL        fCreatedHeap;
    LPINTNODE   lpHead;
    LPINTNODE   lpTail;
    DWORD       lItems;
} LIST, FAR *HLIST;


/************************* Heap Services ******************************/
// Hungarian is hh (ie.  hhHeap)
#ifdef WIN32
#define HHANDLE LPSTR
#else
typedef DWORD HHANDLE;
#endif

#define HT_SHARED    0x0001
#define HT_MULTIPLE  0x0002

extern HHEAP    WINAPI cekmmHeapCreate(UINT, UINT);
extern VOID     WINAPI cekmmHeapDestroy(HHEAP);

extern HHANDLE  WINAPI cekmmHeapAlloc(HHEAP, UINT, UINT);
extern HHANDLE  WINAPI cekmmHeapReAlloc(HHEAP,HHANDLE,UINT,UINT);
extern BOOL  WINAPI cekmmHeapFree(HHEAP, HHANDLE);

extern LPVOID    WINAPI cekmmHeapLock(HHEAP, HHANDLE);
extern BOOL     WINAPI cekmmHeapUnlock(HHEAP, HHANDLE);

extern LPVOID    WINAPI cekmmHeapAllocLock(HHEAP, UINT, UINT);
extern LPSTR    WINAPI cekmmHeapAllocString(HHEAP, UINT, LPSTR);
extern BOOL    WINAPI cekmmHeapUnlockFree(HHEAP, LPVOID);

extern UINT     WINAPI cekmmHeapCompact(HHEAP, UINT);
extern UINT     WINAPI cekmmHeapFlags(HHEAP, HHANDLE);
extern HHANDLE  WINAPI cekmmHeapHandle(HHEAP, LPVOID);
extern HHANDLE  WINAPI cekmmHeapDiscard(HHEAP,HHANDLE);
extern UINT     WINAPI cekmmHeapSize(HHEAP, HHANDLE);
extern HHANDLE  WINAPI cekmmHeapSetDS(HHEAP);
extern DWORD    WINAPI cekmmHeapItems(HHEAP);
extern VOID     WINAPI cekmmHeapDumpDebug(HHEAP);
extern HHEAP               FindHeap( HHEAP, HHANDLE );

/************************** List Services ******************************/
// Hungarian is hlst (ie.  hlstList)

typedef void far *LPNODE;
typedef int (WINAPI *SORTPROC)(LPNODE,LPNODE,DWORD);
typedef LPVOID (WINAPI *DUMPPROC)(LPNODE);

#define LIST_INSERTHEAD     ((LPNODE)0L)
#define LIST_INSERTTAIL     ((LPNODE)1L)         
#define LIST_SORT_BUBBLE    0x0000

HHEAP  WINAPI cekmmListHeap( HLIST hlstList ) ;
HLIST  WINAPI cekmmListCreate(  HHEAP hhHeap  ) ;
VOID   WINAPI cekmmListDestroy( HLIST hlstList ) ;
VOID   WINAPI cekmmListFreeList( HLIST hlstList ) ;
LPNODE WINAPI cekmmListAllocNode( HLIST hlstList, LPVOID lpMem, UINT uiSize ) ;
VOID   WINAPI cekmmListFreeNode(  HLIST hlstList, LPNODE lpNode  ) ;
BOOL   WINAPI cekmmListInsertNode( HLIST hlstList, LPNODE lpNode, LPNODE lpPrevNode ) ;
BOOL   WINAPI cekmmListInsertSorted( HLIST hlstList,LPNODE lpNode,SORTPROC lpfnSort, LPARAM lData ) ;
LPNODE WINAPI cekmmListAllocInsertNode(  HLIST hlstList, LPVOID lpMem, UINT uiSize, LPNODE lpPrevNode  ) ;
VOID   WINAPI cekmmListRemoveNode(  HLIST hlstList, LPNODE lpNode  ) ;
VOID   WINAPI cekmmListDeleteNode(  HLIST hlstList, LPNODE lpNode  ) ;
LPNODE WINAPI cekmmListNextNode(  HLIST hlstList, LPNODE lpNode  ) ;
LPNODE WINAPI cekmmListPrevNode(  HLIST hlstList, LPNODE lpNode  ) ;
LPNODE WINAPI cekmmListFirstNode(  HLIST hlstList  ) ;
LPNODE WINAPI cekmmListLastNode(  HLIST hlstList  ) ;
DWORD  WINAPI cekmmListNode2Item( HLIST hlstList, LPNODE lpNode ) ;
LPNODE WINAPI cekmmListItem2Node( HLIST hlstList, DWORD lItem ) ;
DWORD  WINAPI cekmmListItems( HLIST hlstList ) ;
VOID   WINAPI cekmmListSwapNodes(  HLIST hlstList, LPNODE lp1, LPNODE lp2  ) ;
BOOL   WINAPI cekmmListSort( HLIST hlstList, SORTPROC lpfnSort, UINT wType, LPARAM lData ) ;
VOID   WINAPI cekmmListDumpDebug( HLIST hlstList, DUMPPROC lpfnDump ) ;


/************************** Stack Services *****************************/
// Hungarian is hstk (ie.  hstkStack)
typedef HLIST HSTACK;

extern HSTACK   WINAPI cekmmStackCreate(HHEAP);
extern VOID     WINAPI cekmmStackDestroy(HSTACK);
extern VOID     WINAPI cekmmStackFreeStack(HSTACK);
extern LPNODE   WINAPI cekmmStackAllocNode(HSTACK, LPSTR, UINT);
extern LPNODE   WINAPI cekmmStackFreeNode(HSTACK, LPNODE);
extern BOOL     WINAPI cekmmStackPush(HSTACK, LPNODE);
extern LPNODE   WINAPI cekmmStackAllocPush(HSTACK, LPSTR, UINT);
extern LPNODE   WINAPI cekmmStackPop(HSTACK);
extern LPNODE   WINAPI cekmmStackPeek(HSTACK, LONG);
extern LPNODE   WINAPI cekmmStackRemove(HSTACK, LONG);
extern LONG     WINAPI cekmmStackItems(HSTACK);
extern VOID     WINAPI cekmmStackDumpDebug(HSTACK,DUMPPROC);


/************************* Queue Services *****************************/
// Hungarian is hs (ie.  hqQueue)
typedef HLIST HQUEUE;

extern HQUEUE   WINAPI cekmmQueueCreate(HHEAP);
extern VOID     WINAPI cekmmQueueDestroy(HQUEUE);
extern VOID     WINAPI cekmmQueueFreeQueue(HQUEUE);
extern LPNODE   WINAPI cekmmQueueAllocNode(HQUEUE, LPSTR, UINT);
extern BOOL     WINAPI cekmmQueueFreeNode(HQUEUE, LPNODE);
extern BOOL     WINAPI cekmmQueueInQ(HQUEUE, LPNODE);
extern LPNODE   WINAPI cekmmQueueAllocInQ(HQUEUE, LPSTR, UINT);
extern LPNODE   WINAPI cekmmQueueDeQ(HQUEUE);
extern LPNODE   WINAPI cekmmQueuePeek(HQUEUE, LONG);
extern LPNODE   WINAPI cekmmQueueRemove(HQUEUE, LONG);
extern LONG     WINAPI cekmmQueueItems(HQUEUE);
extern VOID     WINAPI cekmmQueueDumpDebug(HSTACK,DUMPPROC);

#endif // _MEMMAN_H_

#include "CEK.h"

#define SPLITCURSOR  1
BOOL WINAPI SliderInit( HINSTANCE hPrev, HINSTANCE hInstance, LPSTR lpsz ) ;
BOOL WINAPI LEDInit( HANDLE hPrev, HANDLE hInst, LPSTR lpsz ) ;
BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance, LPSTR lpszClassName ) ;
BOOL WINAPI ToolbarInit( HINSTANCE hInstPrev, HINSTANCE hInstance, LPSTR lpsz ) ;

#define GRCS(X) GetRCString(X,ghmodDLL)

#endif // _ICEK_H_

/************************************************************************
 * End of File: ICEK.H
 ***********************************************************************/

