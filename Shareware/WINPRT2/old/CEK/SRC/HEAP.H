HHEAP    WINAPI cekmmHeapCreate( UINT uiSize, UINT uiFlags ) ;
VOID     WINAPI cekmmHeapDestroy( HHEAP hhHeap  ) ;
HHANDLE  WINAPI cekmmHeapAlloc( HHEAP hhHeap, UINT uiFlags, UINT uiSize  ) ;
HHANDLE  WINAPI cekmmHeapReAlloc( HHEAP hhHeap,HHANDLE hhMem,UINT uiSize, UINT uiFlags ) ;
HHANDLE  WINAPI cekmmHeapFree( HHEAP hhHeap, HHANDLE hhMem ) ;
LPVOID   WINAPI cekmmHeapLock( HHEAP hhHeap, HHANDLE hhMem ) ;
BOOL     WINAPI cekmmHeapUnlock( HHEAP hhHeap, HHANDLE hhMem ) ;
LPVOID   WINAPI cekmmHeapAllocLock( HHEAP hhHeap, UINT uiFlags, UINT uiSize ) ;
LPSTR    WINAPI cekmmHeapAllocString( HHEAP hhHeap, UINT uiFlags,LPSTR lpString ) ;
LPVOID   WINAPI cekmmHeapUnlockFree( HHEAP hhHeap, LPVOID lpMem ) ;
UINT     WINAPI cekmmHeapCompact( HHEAP hhHeap, UINT wMinFree ) ;
UINT     WINAPI cekmmHeapFlags( HHEAP hhHeap, HHANDLE hhMem ) ;
HHANDLE  WINAPI cekmmHeapHandle( HHEAP hhHeap, LPVOID lpMem ) ;
HHANDLE  WINAPI cekmmHeapDiscard( HHEAP hhHeap,HHANDLE hhMem ) ;
UINT     WINAPI cekmmHeapSize( HHEAP hhHeap, HHANDLE hhMem ) ;
HHANDLE  WINAPI cekmmHeapSetDS( HHEAP hhHeap ) ;
DWORD    WINAPI cekmmHeapItems( HHEAP hhHeap ) ;
VOID     WINAPI cekmmHeapDumpDebug( HHEAP hhHeap ) ;

