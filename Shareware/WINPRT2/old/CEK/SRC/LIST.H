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

