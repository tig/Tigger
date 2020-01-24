//*************************************************************
//  File name: intfsmm.h
//
//  Description: 
//      internal header file for the DLL
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#ifndef __INTFSMM
#define __INTFSMM

#ifndef PASCAL
#include <windows.h>
#endif

#define CONXMEM_VER     0x0100

WORD FAR PASCAL fsmmCheckVersion( WORD );

typedef struct tgHeap
{
    struct tgHeap FAR  *hhNextHeap;
    WORD                wType;
    DWORD               dwAllocs;
} HEAP, FAR *HHEAP;

typedef struct tgINTNODE {
    struct tgINTNODE FAR *lpNext;
    struct tgINTNODE FAR *lpPrev;
} INTNODE, FAR *LPINTNODE;

typedef struct
{
    HHEAP       hhHeap;
    WORD        fCreatedHeap;
    LPINTNODE   lpHead;
    LPINTNODE   lpTail;
    LONG        lItems;
} LIST, FAR *HLIST;


/************************* Heap Services ******************************/
// Hungarian is hh (ie.  hhHeap)
typedef DWORD HHANDLE;

#define HT_SHARED    0x0001
#define HT_MULTIPLE  0x0002

extern HHEAP    FAR PASCAL fsmmHeapCreate(WORD, WORD);
extern VOID     FAR PASCAL fsmmHeapDestroy(HHEAP);

extern HHANDLE  FAR PASCAL fsmmHeapAlloc(HHEAP, WORD, WORD);
extern HHANDLE  FAR PASCAL fsmmHeapReAlloc(HHEAP,HHANDLE,WORD,WORD);
extern HHANDLE  FAR PASCAL fsmmHeapFree(HHEAP, HHANDLE);
extern LPSTR    FAR PASCAL fsmmHeapLock(HHEAP, HHANDLE);
extern BOOL     FAR PASCAL fsmmHeapUnlock(HHEAP, HHANDLE);

extern LPSTR    FAR PASCAL fsmmHeapAllocLock(HHEAP, WORD, WORD);
extern LPSTR    FAR PASCAL fsmmHeapAllocString(HHEAP, WORD, LPSTR);
extern LPSTR    FAR PASCAL fsmmHeapUnlockFree(HHEAP, LPSTR);

extern WORD     FAR PASCAL fsmmHeapCompact(HHEAP, WORD);
extern WORD     FAR PASCAL fsmmHeapFlags(HHEAP, HHANDLE);
extern HHANDLE  FAR PASCAL fsmmHeapHandle(HHEAP, LPVOID);
extern HHANDLE  FAR PASCAL fsmmHeapDiscard(HHEAP,HHANDLE);
extern WORD     FAR PASCAL fsmmHeapSize(HHEAP, HHANDLE);
extern HHANDLE  FAR PASCAL fsmmHeapSetDS(HHEAP);
extern DWORD    FAR PASCAL fsmmHeapItems(HHEAP);
extern VOID     FAR PASCAL fsmmHeapDumpDebug(HHEAP);
extern HHEAP               FindHeap( HHEAP, HHANDLE );

/************************** List Services ******************************/
// Hungarian is hlst (ie.  hlstList)

typedef void far *LPNODE;
typedef int (FAR PASCAL *SORTPROC)(LPNODE,LPNODE,LONG);
typedef LPSTR (FAR PASCAL *DUMPPROC)(LPNODE);

#define LIST_INSERTHEAD     ((LPNODE)0L)
#define LIST_INSERTTAIL     ((LPNODE)1L)         
#define LIST_SORT_BUBBLE    0x0000

extern HHEAP    FAR PASCAL fsmmListHeap(HLIST);

extern HLIST    FAR PASCAL fsmmListCreate(HHEAP);
extern VOID     FAR PASCAL fsmmListDestroy(HLIST);
extern VOID     FAR PASCAL fsmmListFreeList(HLIST);
extern LPNODE   FAR PASCAL fsmmListAllocNode(HLIST, LPSTR, WORD);
extern VOID     FAR PASCAL fsmmListFreeNode(HLIST, LPNODE); 
extern BOOL     FAR PASCAL fsmmListInsertNode(HLIST,LPNODE,LPNODE);
extern BOOL     FAR PASCAL fsmmListInsertSorted(HLIST,LPNODE,SORTPROC,LONG);
extern LPNODE   FAR PASCAL fsmmListAllocInsertNode(HLIST,LPSTR,WORD,LPNODE);
extern VOID     FAR PASCAL fsmmListRemoveNode(HLIST, LPNODE);
extern VOID     FAR PASCAL fsmmListDeleteNode(HLIST,LPNODE);
extern LPNODE   FAR PASCAL fsmmListNextNode(HLIST, LPNODE);
extern LPNODE   FAR PASCAL fsmmListPrevNode(HLIST, LPNODE);
extern LPNODE   FAR PASCAL fsmmListFirstNode(HLIST);
extern LPNODE   FAR PASCAL fsmmListLastNode(HLIST);
extern LONG     FAR PASCAL fsmmListNode2Item(HLIST, LPNODE);
extern LPNODE   FAR PASCAL fsmmListItem2Node(HLIST, LONG);
extern LONG     FAR PASCAL fsmmListItems(HLIST);
extern VOID     FAR PASCAL fsmmListSwapNodes(HLIST,LPNODE,LPNODE);
extern BOOL     FAR PASCAL fsmmListSort(HLIST,SORTPROC,WORD,LONG);
extern VOID     FAR PASCAL fsmmListDumpDebug(HLIST,DUMPPROC);


/************************** Stack Services *****************************/
// Hungarian is hstk (ie.  hstkStack)
typedef HLIST HSTACK;

extern HSTACK   FAR PASCAL fsmmStackCreate(HHEAP);
extern VOID     FAR PASCAL fsmmStackDestroy(HSTACK);
extern VOID     FAR PASCAL fsmmStackFreeStack(HSTACK);
extern LPNODE   FAR PASCAL fsmmStackAllocNode(HSTACK, LPSTR, WORD);
extern LPNODE   FAR PASCAL fsmmStackFreeNode(HSTACK, LPNODE);
extern BOOL     FAR PASCAL fsmmStackPush(HSTACK, LPNODE);
extern LPNODE   FAR PASCAL fsmmStackAllocPush(HSTACK, LPSTR, WORD);
extern LPNODE   FAR PASCAL fsmmStackPop(HSTACK);
extern LPNODE   FAR PASCAL fsmmStackPeek(HSTACK, LONG);
extern LPNODE   FAR PASCAL fsmmStackRemove(HSTACK, LONG);
extern LONG     FAR PASCAL fsmmStackItems(HSTACK);
extern VOID     FAR PASCAL fsmmStackDumpDebug(HSTACK,DUMPPROC);


/************************* Queue Services *****************************/
// Hungarian is hs (ie.  hqQueue)
typedef HLIST HQUEUE;

extern HQUEUE   FAR PASCAL fsmmQueueCreate(HHEAP);
extern VOID     FAR PASCAL fsmmQueueDestroy(HQUEUE);
extern VOID     FAR PASCAL fsmmQueueFreeQueue(HQUEUE);
extern LPNODE   FAR PASCAL fsmmQueueAllocNode(HQUEUE, LPSTR, WORD);
extern LPNODE   FAR PASCAL fsmmQueueFreeNode(HQUEUE, LPNODE);
extern BOOL     FAR PASCAL fsmmQueueInQ(HQUEUE, LPNODE);
extern LPNODE   FAR PASCAL fsmmQueueAllocInQ(HQUEUE, LPSTR, WORD);
extern LPNODE   FAR PASCAL fsmmQueueDeQ(HQUEUE);
extern LPNODE   FAR PASCAL fsmmQueuePeek(HQUEUE, LONG);
extern LPNODE   FAR PASCAL fsmmQueueRemove(HQUEUE, LONG);
extern LONG     FAR PASCAL fsmmQueueItems(HQUEUE);
extern VOID     FAR PASCAL fsmmQueueDumpDebug(HSTACK,DUMPPROC);


/************************** Atom Services *****************************/
// Hungarian is hat (ie.  hatAtomTable)
// Hungarian is ha (ie.   haAtom)
typedef HHEAP HATBL;
typedef DWORD HATOM;

extern HATBL    FAR PASCAL fsmmAtomTableCreate(WORD wInitSize);
extern VOID     FAR PASCAL fsmmAtomTableDestroy(HATBL );
extern HATOM    FAR PASCAL fsmmAddAtom( HATBL, LPSTR );
extern HATOM    FAR PASCAL fsmmDeleteAtom( HATBL, HATOM );
extern HATOM    FAR PASCAL fsmmFindAtom( HATBL, LPSTR );
extern WORD     FAR PASCAL fsmmGetAtomName( HATBL, HATOM, LPSTR, WORD );
extern VOID     FAR PASCAL fsmmAtomTableDumpDebug(HATBL);


/************************ String Services *****************************/
// These are just stub functions that call the real thing

extern LPVOID FAR _cdecl fsmm_fmemccpy(LPVOID, const LPVOID, int, WORD);
extern LPVOID FAR _cdecl fsmm_fmemchr(const LPVOID, int, WORD);
extern int    FAR _cdecl fsmm_fmemcmp(const LPVOID, const LPVOID, WORD);
extern LPVOID FAR _cdecl fsmm_fmemcpy(LPVOID, const LPVOID, WORD);
extern int    FAR _cdecl fsmm_fmemicmp(const LPVOID, const LPVOID,WORD);
extern LPVOID FAR _cdecl fsmm_fmemmove(LPVOID, const LPVOID, WORD);
extern LPVOID FAR _cdecl fsmm_fmemset(LPVOID, int, WORD);
extern LPSTR  FAR _cdecl fsmm_fstrcat(LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrchr(const LPSTR, int);
extern int    FAR _cdecl fsmm_fstrcmp(const LPSTR, const LPSTR);
extern int    FAR _cdecl fsmm_fstricmp(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrcpy(LPSTR, const LPSTR);
extern WORD   FAR _cdecl fsmm_fstrcspn(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrdup(const LPSTR);
extern WORD   FAR _cdecl fsmm_fstrlen(const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrlwr(LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrncat(LPSTR, const LPSTR, WORD);
extern int    FAR _cdecl fsmm_fstrncmp(const LPSTR, const LPSTR, WORD);
extern int    FAR _cdecl fsmm_fstrnicmp(const LPSTR, const LPSTR, WORD);
extern LPSTR  FAR _cdecl fsmm_fstrncpy(LPSTR, const LPSTR, WORD);
extern LPSTR  FAR _cdecl fsmm_fstrnset(LPSTR, int, WORD);
extern LPSTR  FAR _cdecl fsmm_fstrpbrk(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrrchr(const LPSTR, int);
extern LPSTR  FAR _cdecl fsmm_fstrrev(LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrset(LPSTR, int);
extern WORD   FAR _cdecl fsmm_fstrspn(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrstr(const LPSTR,const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrtok(LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl fsmm_fstrupr(LPSTR);

extern BOOL FAR PASCAL fsmmIsUsefulString( LPSTR lpText );
extern BOOL FAR PASCAL fsmmIsNumber( LPSTR lpS );
extern VOID FAR PASCAL fsmmSetExtension( LPSTR lpFile, LPSTR lpExt );
extern void FAR PASCAL fsmmHackSpaces( LPSTR lpszLine );
extern LONG FAR PASCAL fsmmGetValue(LPSTR FAR *lplp);

#endif

/*** EOF: intfsmm.h ***/
