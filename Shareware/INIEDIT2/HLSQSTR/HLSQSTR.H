//*************************************************************
//  File name: hlsqstr.h
//
//  Description: 
//      Header file for (Heap), (L)ist, (S)tack, (Q)ueue, (Str)ing memory DLL
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#ifndef __HLSQSTR
#define __HLSQSTR

#ifndef PASCAL
#include <windows.h>
#endif

#define HLSQSTR_VER     100

/************************* Heap Services ******************************/
// Hungarian is hh (ie.  hhHeap)
typedef DWORD HHEAP;
typedef DWORD HHANDLE;

#define HT_SHARED    0x0001
#define HT_MULTIPLE  0x0002

extern HHEAP    FAR PASCAL hlsqHeapCreate(WORD wSize, WORD wFlags);
extern VOID     FAR PASCAL hlsqHeapDestroy(HHEAP);
extern HHANDLE  FAR PASCAL hlsqHeapAlloc(HHEAP, WORD, WORD);
extern HHANDLE  FAR PASCAL hlsqHeapReAlloc(HHEAP,HHANDLE,WORD,WORD);
extern VOID     FAR PASCAL hlsqHeapFree(HHEAP, HHANDLE);
extern LPSTR    FAR PASCAL hlsqHeapLock(HHEAP, HHANDLE);
extern BOOL     FAR PASCAL hlsqHeapUnlock(HHEAP, HHANDLE);
extern LPSTR    FAR PASCAL hlsqHeapAllocLock(HHEAP, WORD, WORD);
    // Allocs with LPTR
extern LPSTR    FAR PASCAL hlsqHeapAllocString(HHEAP, WORD,LPSTR);
    // Allocs with LPTR
extern VOID     FAR PASCAL hlsqHeapUnlockFree(HHEAP, LPSTR);
extern WORD     FAR PASCAL hlsqHeapCompact(HHEAP hhHeap, WORD wMinFree);
extern WORD     FAR PASCAL hlsqHeapFlags(HHEAP, HHANDLE);
extern HHANDLE  FAR PASCAL hlsqHeapHandle(HHEAP hhHeap, LPVOID lpMem);
extern HHANDLE  FAR PASCAL hlsqHeapDiscard(HHEAP,HHANDLE);
extern WORD     FAR PASCAL hlsqHeapSize(HHEAP, HHANDLE);
extern HHANDLE  FAR PASCAL hlsqHeapSetDS(HHEAP);
extern DWORD    FAR PASCAL hlsqHeapItems(HHEAP);
extern VOID     FAR PASCAL hlsqHeapDumpDebug(HHEAP);


/************************** List Services ******************************/
// Hungarian is hlst (ie.  hlstList)
typedef  DWORD  HLIST;

typedef void far *LPNODE;
typedef int (FAR PASCAL *SORTPROC)(LPNODE,LPNODE,LONG);
typedef LPSTR (FAR PASCAL *DUMPPROC)(LPNODE);

#define LIST_INSERTHEAD     ((LPNODE)0L)
#define LIST_INSERTTAIL     ((LPNODE)1L)         
#define LIST_SORT_BUBBLE    0x0000

extern HLIST  FAR PASCAL hlsqListCreate(HHEAP);
extern VOID   FAR PASCAL hlsqListDestroy(HLIST);
extern VOID   FAR PASCAL hlsqListFreeList(HLIST);
extern LPNODE FAR PASCAL hlsqListAllocNode(HLIST, LPSTR, WORD);
extern VOID   FAR PASCAL hlsqListFreeNode(HLIST, LPNODE); 
extern BOOL   FAR PASCAL hlsqListInsertNode(HLIST,LPNODE,LPNODE);
   // The 2nd LPNODE is the node to insert AFTER or LIST_INSERT?????
extern BOOL   FAR PASCAL hlsqListInsertSorted(HLIST,LPNODE,SORTPROC,LONG);
   // The LONG is user defined data it can be whatever and just get passed on
extern LPNODE FAR PASCAL hlsqListAllocInsertNode(HLIST,LPSTR,WORD,LPNODE);
   // The LPNODE is the node to insert AFTER or LIST_INSERT?????
extern VOID   FAR PASCAL hlsqListRemoveNode(HLIST, LPNODE);
extern VOID   FAR PASCAL hlsqListDeleteNode(HLIST,LPNODE);
extern LPNODE FAR PASCAL hlsqListNextNode(HLIST, LPNODE);
extern LPNODE FAR PASCAL hlsqListPrevNode(HLIST, LPNODE);
extern LPNODE FAR PASCAL hlsqListFirstNode(HLIST);
extern LPNODE FAR PASCAL hlsqListLastNode(HLIST);
extern LONG   FAR PASCAL hlsqListNode2Item(HLIST, LPNODE);
extern LPNODE FAR PASCAL hlsqListItem2Node(HLIST hlstList, LONG lItem); 
    // lItem is zero based!!
extern LONG   FAR PASCAL hlsqListItems(HLIST);
extern VOID   FAR PASCAL hlsqListSwapNodes(HLIST,LPNODE,LPNODE);
extern BOOL   FAR PASCAL hlsqListSort(HLIST,SORTPROC,WORD,LONG);
    // Word is the SORT TYPE, LONG is user data
extern VOID   FAR PASCAL hlsqListDumpDebug(HLIST,DUMPPROC);


/************************** Stack Services *****************************/
// Hungarian is hstk (ie.  hstkStack)
typedef  DWORD  HSTACK;

extern HSTACK   FAR PASCAL hlsqStackCreate(HHEAP);
extern VOID     FAR PASCAL hlsqStackDestroy(HSTACK);
extern VOID     FAR PASCAL hlsqStackFreeStack(HSTACK);
extern LPNODE   FAR PASCAL hlsqStackAllocNode(HSTACK,LPSTR,WORD);
extern LPNODE   FAR PASCAL hlsqStackFreeNode(HSTACK, LPNODE);
extern BOOL     FAR PASCAL hlsqStackPush(HSTACK, LPNODE);
extern LPNODE   FAR PASCAL hlsqStackAllocPush(HSTACK,LPSTR,WORD);

extern LPNODE   FAR PASCAL hlsqStackPop(HSTACK);
extern LPNODE   FAR PASCAL hlsqStackPeek(HSTACK hstkStack, LONG lIndex);
   // Zero based from the top (Peek(0) == Pop() without removing)
extern LPNODE   FAR PASCAL hlsqStackRemove(HSTACK hstkStack, LONG lndex);
   // Zero based from the top (Remove(0) == Pop())
extern LONG     FAR PASCAL hlsqStackItems(HSTACK);
extern VOID     FAR PASCAL hlsqStackDumpDebug(HSTACK,DUMPPROC);


/************************* Queue Services *****************************/
// Hungarian is hs (ie.  hqQueue)
typedef  DWORD  HQUEUE;

extern HQUEUE   FAR PASCAL hlsqQueueCreate(HHEAP);
extern VOID     FAR PASCAL hlsqQueueDestroy(HQUEUE);
extern VOID     FAR PASCAL hlsqQueueFreeQueue(HQUEUE);
extern LPNODE   FAR PASCAL hlsqQueueAllocNode(HQUEUE,LPSTR,WORD);
extern LPNODE   FAR PASCAL hlsqQueueFreeNode(HQUEUE, LPNODE);
extern BOOL     FAR PASCAL hlsqQueueInQ(HQUEUE, LPNODE);
extern LPNODE   FAR PASCAL hlsqQueueAllocInQ(HQUEUE, LPSTR, WORD);
extern LPNODE   FAR PASCAL hlsqQueueDeQ(HQUEUE);
extern LPNODE   FAR PASCAL hlsqQueuePeek(HQUEUE hqQueue, LONG lIndex);
   // Zero based from the top (Peek(0) == DeQ() without removing)
extern LPNODE   FAR PASCAL hlsqQueueRemove(HQUEUE hqQueue, LONG lIndex);
   // Zero based from the top (Remove(0) == DeQ())
extern LONG     FAR PASCAL hlsqQueueItems(HQUEUE);
extern VOID     FAR PASCAL hlsqQueueDumpDebug(HSTACK,DUMPPROC);


/************************ String Services *****************************/
// These are just stub functions that call the real thing

extern LPVOID FAR _cdecl hlsq_fmemccpy(LPVOID, const LPVOID, int, WORD);
extern LPVOID FAR _cdecl hlsq_fmemchr(const LPVOID, int, WORD);
extern int    FAR _cdecl hlsq_fmemcmp(const LPVOID, const LPVOID, WORD);
extern LPVOID FAR _cdecl hlsq_fmemcpy(LPVOID, const LPVOID, WORD);
extern int    FAR _cdecl hlsq_fmemicmp(const LPVOID, const LPVOID,WORD);
extern LPVOID FAR _cdecl hlsq_fmemmove(LPVOID, const LPVOID, WORD);
extern LPVOID FAR _cdecl hlsq_fmemset(LPVOID, int, WORD);
extern LPSTR  FAR _cdecl hlsq_fstrcat(LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrchr(const LPSTR, int);
extern int    FAR _cdecl hlsq_fstrcmp(const LPSTR, const LPSTR);
extern int    FAR _cdecl hlsq_fstricmp(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrcpy(LPSTR, const LPSTR);
extern WORD   FAR _cdecl hlsq_fstrcspn(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrdup(const LPSTR);
extern WORD   FAR _cdecl hlsq_fstrlen(const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrlwr(LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrncat(LPSTR, const LPSTR, WORD);
extern int    FAR _cdecl hlsq_fstrncmp(const LPSTR, const LPSTR, WORD);
extern int    FAR _cdecl hlsq_fstrnicmp(const LPSTR, const LPSTR, WORD);
extern LPSTR  FAR _cdecl hlsq_fstrncpy(LPSTR, const LPSTR, WORD);
extern LPSTR  FAR _cdecl hlsq_fstrnset(LPSTR, int, WORD);
extern LPSTR  FAR _cdecl hlsq_fstrpbrk(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrrchr(const LPSTR, int);
extern LPSTR  FAR _cdecl hlsq_fstrrev(LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrset(LPSTR, int);
extern WORD   FAR _cdecl hlsq_fstrspn(const LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrstr(const LPSTR,const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrtok(LPSTR, const LPSTR);
extern LPSTR  FAR _cdecl hlsq_fstrupr(LPSTR);


#endif

/*** EOF: hlsqstr.h ***/
