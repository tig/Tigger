/*----------------------------------------------------------------------------
	Multiple Local Heap Routines 
	mlocal.h
	danq    
	10-28-90
----------------------------------------------------------------------------*/

#define MultLocalDiscard(hHeap,h) MultLocalReAlloc(hHeap, h, 0, LMEM_MOVEABLE)
#define MultLocalShrink(hHeap,h) LocalShrink(hHeap, h)

HANDLE  FAR PASCAL MultLocalAlloc(HANDLE, WORD, WORD);
WORD    FAR PASCAL MultLocalCompact(HANDLE, WORD);
HANDLE  FAR PASCAL MultLocalFree(HANDLE, HANDLE);
HANDLE  FAR PASCAL MultLocalHandle(HANDLE, LPSTR);
HANDLE  FAR PASCAL MultLocalInit(WORD);
LPSTR   FAR PASCAL MultLocalLock(HANDLE, HANDLE);
HANDLE  FAR PASCAL MultLocalReAlloc(HANDLE, HANDLE, WORD, WORD);
WORD    FAR PASCAL MultLocalSize(HANDLE, HANDLE);
BOOL    FAR PASCAL MultLocalUnlock(HANDLE, HANDLE);
WORD    FAR PASCAL MultLocalFlags(HANDLE, HANDLE);
HANDLE  FAR PASCAL SetDStoHeap(HANDLE);


// Far Heap
typedef DWORD FHANDLE;

#define FarLocalLock(d) MultLocalLock( (HANDLE) HIWORD(d), (HANDLE) LOWORD(d))
#define FarLocalUnlock(d) MultLocalUnlock( (HANDLE) HIWORD(d), (HANDLE) LOWORD(d))
FHANDLE FAR PASCAL FarLocalAlloc(WORD, WORD);
FHANDLE FAR PASCAL FarLocalFree(FHANDLE);


