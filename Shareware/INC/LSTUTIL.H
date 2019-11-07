/************************************************************************
 *
 *     Project:  Multiple Local Heaps Manager DLL with List Mgr
 *
 *      Module:  LSTUTIL.h
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
 *    95-99% of the code in LSTUTIL.DLL was written by Dan Quigley.
 *    Code was placed into a DLL by Charlie Kindel for the
 *    Road Runner Project.
 *
 ***********************************************************************/

#ifndef _LSTUTIL_H_
#define _LSTUTIL_H_


/*
 * The SPEC version can be used by an application to determine if the
 * installed version of this DLL is old or new.  The app should call
 * tdCheckVersion( _LSTUTIL_SPEC_VER_ ).  If this function returns
 * FALSE then the DLL installed is old and should not be called.
 */
#define _LSTUTIL_SPEC_VER_   ((WORD)100)


DWORD  WINAPI lstGetVersion( LPSTR ) ;

HANDLE WINAPI lstGethInst( void ) ;

BOOL   WINAPI lstCheckVersion( DWORD ) ;

/*----------------------------------------------------------------------------
	Multiple Local Heap Routines 
----------------------------------------------------------------------------*/

#define HHEAP  GLOBALHANDLE

#define        lstMultLocalDiscard(ghHeap,lh)\
   lstMultLocalReAlloc(ghHeap, lh, 0,LMEM_MOVEABLE)

#define        lstMultLocalShrink(ghHeap,lh)\
   LocalShrink(ghHeap, lh)

LOCALHANDLE    WINAPI lstMultLocalAlloc(HHEAP, WORD, WORD);

WORD           WINAPI lstMultLocalCompact(HHEAP, WORD);

LOCALHANDLE    WINAPI lstMultLocalFree(HHEAP, LOCALHANDLE);

LOCALHANDLE    WINAPI lstMultLocalHandle(HHEAP, LPVOID);

HHEAP          WINAPI lstMultLocalInit(WORD);

LPVOID         WINAPI lstMultLocalLock(HHEAP, LOCALHANDLE);

LOCALHANDLE    WINAPI lstMultLocalReAlloc(HHEAP, LOCALHANDLE, 
                                           WORD, WORD);

WORD           WINAPI lstMultLocalSize(HHEAP, LOCALHANDLE);

BOOL           WINAPI lstMultLocalUnlock(HHEAP, LOCALHANDLE);

WORD           WINAPI lstMultLocalFlags(HHEAP, LOCALHANDLE);

WORD           WINAPI lstMultFreeHeap(HHEAP);

HHEAP          WINAPI lstSetDStoHeap(HHEAP);


// Far Heap
typedef DWORD FHANDLE;
typedef WORD  FLHHANDLE;

#define        lstFarLocalLock(d) lstMultLocalLock(HIWORD(d),LOWORD(d))

#define        lstFarLocalUnlock(d) lstMultLocalUnlock(HIWORD(d),LOWORD(d))

FHANDLE        WINAPI lstFarLocalAlloc(FLHHANDLE, WORD, WORD);

FHANDLE        WINAPI lstFarLocalFree(FLHHANDLE, FHANDLE);

FLHHANDLE      WINAPI lstFarLocalInit(VOID);

BOOL           WINAPI lstFarLocalRemove(FLHHANDLE);

VOID           WINAPI lstFarLocalCompact(FLHHANDLE);

/*----------------------------------------------------------------------------
  Linked List Manager Routines 386 pmode Windows only!!!!!
  Requires FarLocalHeap Manager
----------------------------------------------------------------------------*/

#define LESSTHEN     -1
#define EQUAL         0
#define GREATERTHEN   1

typedef int (CALLBACK* SORTPROC)(LPVOID, LPVOID);

typedef VOID FAR *LPNODE;
typedef DWORD HLIST;

VOID   WINAPI lstDump(HLIST);

LPNODE WINAPI lstAllocNode(HLIST);

LPNODE WINAPI lstAllocAddNode(HLIST, LPVOID);

LPNODE WINAPI lstAllocInsertNode(HLIST, LPNODE, LPVOID);

HLIST  WINAPI lstCreate(WORD);

BOOL   WINAPI lstIsNodeLast(HLIST,LPNODE);

BOOL   WINAPI lstIsNodeFirst(HLIST,LPNODE);

LPNODE WINAPI lstGetFirstNode(HLIST);

LPNODE WINAPI lstGetLastNode(HLIST);

LPNODE WINAPI lstGetNextNode(HLIST, LPNODE);

DWORD  WINAPI lstGetNodeCount(HLIST);

LPNODE WINAPI lstGetPrevNode(HLIST, LPNODE);

LPNODE WINAPI lstGetNode(HLIST, DWORD);

BOOL   WINAPI lstSwapNode(HLIST, LPNODE, LPNODE);

BOOL   WINAPI lstAddNode(HLIST, LPNODE);

BOOL   WINAPI lstDeleteNode(HLIST, LPNODE);

BOOL   WINAPI lstInsertNode(HLIST, LPNODE, LPNODE);

BOOL   WINAPI lstFree(HLIST);

BOOL   WINAPI lstFreeNode(HLIST, LPNODE);

BOOL   WINAPI lstQSort(HLIST, SORTPROC);

#endif

/************************************************************************
 * End of File: LSTUTIL.h
 ***********************************************************************/
