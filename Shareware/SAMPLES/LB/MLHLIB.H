/************************************************************************
 *
 *    Copyright (c) 1991 Daniel P. Quigley.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  Multiple Local Heaps Manager DLL with List Mgr
 *
 *      Module:  mlhlib.h
 *
 *     Remarks:  
 *
 *    IMPORTANT NOTE:
 *
 *
 *    **** The linked list code will work in 386 Enhanced mode ONLY! ****
 *
 *
 *
 *    95-99% of the code in MLHLIB.DLL was written by Dan Quigley.
 *    Code was placed into a DLL by Charlie Kindel for the
 *    Road Runner Project.
 *
 ***********************************************************************/
/*----------------------------------------------------------------------------
	Multiple Local Heap Routines 
----------------------------------------------------------------------------*/

#define        MultLocalDiscard(ghHeap,lh) MultLocalReAlloc(ghHeap, lh, 0,\
                                    LMEM_MOVEABLE)

#define        MultLocalShrink(ghHeap,lh) LocalShrink(ghHeap, lh)

LOCALHANDLE    FAR PASCAL MultLocalAlloc(GLOBALHANDLE, WORD, WORD);

WORD           FAR PASCAL MultLocalCompact(GLOBALHANDLE, WORD);

LOCALHANDLE    FAR PASCAL MultLocalFree(GLOBALHANDLE, LOCALHANDLE);

LOCALHANDLE    FAR PASCAL MultLocalHandle(GLOBALHANDLE, LPVOID);

HANDLE         FAR PASCAL MultLocalInit(WORD);

LPSTR          FAR PASCAL MultLocalLock(GLOBALHANDLE, LOCALHANDLE);

LOCALHANDLE    FAR PASCAL MultLocalReAlloc(GLOBALHANDLE, LOCALHANDLE, 
                                           WORD, WORD);

WORD           FAR PASCAL MultLocalSize(GLOBALHANDLE, LOCALHANDLE);

BOOL           FAR PASCAL MultLocalUnlock(GLOBALHANDLE, LOCALHANDLE);

WORD           FAR PASCAL MultLocalFlags(GLOBALHANDLE, LOCALHANDLE);

GLOBALHANDLE   FAR PASCAL SetDStoHeap(GLOBALHANDLE);


// Far Heap
typedef DWORD FHANDLE;
typedef WORD  FLHHANDLE;

#define        FarLocalLock(d) MultLocalLock(HIWORD(d),LOWORD(d))

#define        FarLocalUnlock(d) MultLocalUnlock(HIWORD(d),LOWORD(d))

FHANDLE        FAR PASCAL FarLocalAlloc(FLHHANDLE, WORD, WORD);

FHANDLE        FAR PASCAL FarLocalFree(FLHHANDLE, FHANDLE);

FLHHANDLE      FAR PASCAL FarLocalInit(VOID);

BOOL           FAR PASCAL FarLocalRemove(FLHHANDLE);

VOID           FAR PASCAL FarLocalCompact(FLHHANDLE);

/*----------------------------------------------------------------------------
  Linked List Manager Routines 386 pmode Windows only!!!!!
  Requires FarLocalHeap Manager
----------------------------------------------------------------------------*/

#define LESSTHEN     -1
#define EQUAL         0
#define GREATERTHEN   1

typedef VOID FAR *LPNODE;
typedef DWORD HLIST;

VOID   FAR PASCAL ListDump(HLIST);

LPNODE FAR PASCAL ListAllocNode(HLIST);

LPNODE FAR PASCAL ListAllocAddNode(HLIST, LPVOID);

LPNODE FAR PASCAL ListAllocInsertNode(HLIST, LPNODE, LPVOID);

HLIST  FAR PASCAL ListCreate(WORD);

BOOL   FAR PASCAL ListIsNodeLast(HLIST,LPNODE);

BOOL   FAR PASCAL ListIsNodeFirst(HLIST,LPNODE);

LPNODE FAR PASCAL ListGetFirstNode(HLIST);

LPNODE FAR PASCAL ListGetLastNode(HLIST);

LPNODE FAR PASCAL ListGetNextNode(HLIST, LPNODE);

DWORD  FAR PASCAL ListGetNodeCount(HLIST);

LPNODE FAR PASCAL ListGetPrevNode(HLIST, LPNODE);

LPNODE FAR PASCAL ListGetNode(HLIST, DWORD);

BOOL   FAR PASCAL ListSwapNode(HLIST, LPNODE, LPNODE);

BOOL   FAR PASCAL ListAddNode(HLIST, LPNODE);

BOOL   FAR PASCAL ListDeleteNode(HLIST, LPNODE);

BOOL   FAR PASCAL ListInsertNode(HLIST, LPNODE, LPNODE);

BOOL   FAR PASCAL ListFree(HLIST);

BOOL   FAR PASCAL ListFreeNode(HLIST, LPNODE);

BOOL   FAR PASCAL ListQSort(HLIST, FARPROC);

LPSTR  FAR PASCAL GetMLHLIBVersion( LPSTR ) ;

HANDLE FAR PASCAL GetMLHLIBhInst( void ) ;

/************************************************************************
 * End of File: mlhlib.h
 ***********************************************************************/
