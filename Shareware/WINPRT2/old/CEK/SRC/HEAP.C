//*************************************************************
//  File name: heap.c
//
//  Description: 
//      Code for the heap stuff
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "ICEK.h"


//*************************************************************
//
//  FindHeap()
//
//  Purpose:
//		Finds which heap its in for MULTIPLE
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hhMem
//      
//
//  Return: (HHEAP)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 7/91   MSM        Created
//
//*************************************************************

HHEAP FindHeap( HHEAP hhHeap, HHANDLE hhMem )
{
#ifdef WIN32
    return hhHeap ;
#else
    while (hhHeap)
    {
        if (HIWORD(hhHeap)==HIWORD(hhMem))
            return hhHeap;
        hhHeap = hhHeap->hhNextHeap;
    }
    return NULL;
#endif
} /* FindHeap() */


//*************************************************************
//
//  cekmmHeapCreate()
//
//  Purpose:
//		This function creates a new heap and returns a handle to it
//
//
//  Parameters:
//      UINT uiSize  - Initial size of the heap
//      UINT uiFlags - Flags to create the heap with: SHARED or MULTIPLE
//      
//
//  Return: (HHEAP WINAPI)
//      Handle to the heap
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//
//*************************************************************

HHEAP WINAPI cekmmHeapCreate(UINT uiSize, UINT uiFlags)
{
#ifdef WIN32
    HANDLE  h = HeapCreate( NULL, uiSize+sizeof(HEAP), NULL ) ;
    HHEAP   lp ;

    if (h)
    {
        if (lp = (HHEAP)HeapAlloc( h, sizeof(HEAP) ))
        {
            lp->hHeap = h ;
            lp->wType = uiFlags ;
            lp->dwAllocs = 0 ;
            lp->dwAllocated = 0 ;
            return lp ;
        }
        else
            HeapDestroy( h ) ;
    }

    return NULL ;
        
#else

    HANDLE  hOldDS, hMem = NULL;
    UINT    uiMemFlags = GMEM_MOVEABLE|GMEM_ZEROINIT;
    LPVOID   lpMem;
    HEAP   *pHeap;

    if (uiFlags & HT_SHARED)
        uiMemFlags |= GMEM_DDESHARE;

    if ( uiSize<2048)
        uiSize = 2048;
    else
        uiSize&=0xFFF0; // round to 16

// Allocate memory for the heap
    if (!(hMem = GlobalAlloc( uiMemFlags, (DWORD)uiSize )))
        return NULL;
    
// Get the selector rights
    if (!(lpMem = GlobalLock( hMem )))
    {
create_failure:
        GlobalFree( hMem );
        return NULL;
    }
// Initialize the heap

    if (!LocalInit( HIWORD((LONG)lpMem), 0, uiSize-1 ))
        goto create_failure;

// Segment is now set up as a LocalHeap
// LocalAlloc a chunk of Memory to use as our HEAP structure
// First Set the DS to the heap

    _asm
    {
        push        ds
        lds         ax,lpMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalAlloc out of the new heap for the HEAP structure
    pHeap = (HEAP *)LocalAlloc( LPTR, sizeof(HEAP) );
    if (pHeap)
        pHeap->wType= uiFlags;

    lpMem = (LPVOID)pHeap;

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (pHeap==NULL)    // Alloc failed
        goto create_failure;

    return (HHEAP)lpMem;

#endif

} /* cekmmHeapCreate() */

//*************************************************************
//
//  cekmmHeapDestroy()
//
//  Purpose:
//		Destroys a heap that was created with cekmmHeapCreate()
//
//
//  Parameters:
//
//      HHEAP hHeap - Handle returned by cekmmHeapCreate()
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
//
//
//*************************************************************

VOID WINAPI cekmmHeapDestroy( HHEAP hhHeap )
{
#ifdef WIN32
    HeapDestroy( hhHeap->hHeap ) ;
#else
    while (hhHeap)  // Walk the chain and kill all the heaps
    {
        HANDLE hMem = (HANDLE)HIWORD( (LONG)hhHeap );
    
        hhHeap = hhHeap->hhNextHeap;
        GlobalUnlock( hMem );
        GlobalFree( hMem );
    }
    return;
#endif
} /* cekmmHeapDestroy() */

//*************************************************************
//
//  cekmmHeapAlloc()
//
//  Purpose:
//		Allocates a piece of memory from the heap
//
//
//  Parameters:
//      HHEAP hhHeap - Heap to alloc from
//      UINT  uiFlags - Flags for allocation
//      UINT  uiSize  - Size of allocation
//      
//
//  Return: (HHANDLE WINAPI)
//      handle to the memory if successful, NULL otherwise
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE WINAPI cekmmHeapAlloc( HHEAP hhHeap, UINT uiFlags, UINT uiSize )
{
#ifdef WIN32
    LPSTR   lp = HeapAlloc( hhHeap->hHeap, uiSize ) ;

    if (lp)
    {
        hhHeap->dwAllocated += uiSize ;
        hhHeap->dwAllocs++ ;

        if (uiFlags & GMEM_ZEROINIT)
            _fmemset( lp, '\0', uiSize ) ;
    }

    return lp ;
#else
    HHANDLE hMem;
    HANDLE  hOldDS;

    if (!hhHeap)
        return NULL;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhHeap
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalAlloc out of the new heap
    hMem = (DWORD)(LPVOID)LocalAlloc( uiFlags, uiSize );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }

    if (hMem)
        hhHeap->dwAllocs++;
    else    // No dice, check for multiple and empty
    {
        if (hhHeap->wType & HT_MULTIPLE)    // try next heap again
        {
            HHEAP hhH;

            if (hhHeap->hhNextHeap) // Recurse the list
                return cekmmHeapAlloc(hhHeap->hhNextHeap, uiFlags, uiSize );

            if (hhHeap->dwAllocs==0)    // Big as it gets, fail the call
                return NULL;

            hhH = cekmmHeapCreate( 0, hhHeap->wType );
        
            if (hhH)
            {
                HHANDLE hhMem;
                hhMem = cekmmHeapAlloc(hhH, uiFlags, uiSize );
                if (hhMem)
                    hhHeap->hhNextHeap = hhH;
                else
                    cekmmHeapDestroy( hhH );
                return hhMem;
            }
        }
        return NULL;
    }

    return MAKELONG(hMem,HIWORD(hhHeap));
#endif
} /* cekmmHeapAlloc() */

//*************************************************************
//
//  cekmmHeapReAlloc()
//
//  Purpose:
//      Attempts to realloc the chunk of memory in the heap.
//      Will not move to new heap!!!!!
//
//      Under Win32 will always return a new pointer.
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hMem
//      UINT uiSize
//      UINT uiFlags
//      
//
//  Return: (HHANDLE WINAPI)
//      New handle to memory
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE WINAPI cekmmHeapReAlloc(HHEAP hhHeap,HHANDLE hhMem,UINT uiSize, UINT uiFlags)
{
#ifdef WIN32
    LPSTR   lp = HeapAlloc( hhHeap->hHeap, uiSize ) ;

    if (lp)
    {
        hhHeap->dwAllocated -= HeapSize( hhHeap->hHeap, hhMem ) ;
        hhHeap->dwAllocated += uiSize ;

        if (uiFlags & GMEM_ZEROINIT)
            _fmemset( lp, '\0', uiSize ) ;
    }

    _fmemcpy( lp, hhMem, uiSize ) ;

    HeapFree( hhHeap->hHeap, hhMem ) ;

    return lp ;
#else
    HANDLE hOldDS, hMem;

    if (!hhHeap || !hhMem)
        return NULL;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalReAlloc out of the new heap

    hMem = LocalReAlloc( (LOCALHANDLE)LOWORD(hhMem), uiSize, uiFlags );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (hMem)

        return MAKELONG(hMem,HIWORD(hhMem));
    else
        return NULL;
#endif
} /* cekmmHeapReAlloc() */

//*************************************************************
//
//  cekmmHeapFree()
//
//  Purpose:
//		Attempts to free the memory chunk
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hMem
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

BOOL WINAPI cekmmHeapFree(HHEAP hhHeap, HHANDLE hhMem)
{
#ifdef WIN32
    hhHeap->dwAllocs-- ;
    hhHeap->dwAllocated -= HeapSize( hhHeap->hHeap, hhMem ) ;
    return HeapFree( hhHeap->hHeap, hhMem ) ;
#else
    HANDLE hOldDS;

    if (!hhHeap || !hhMem)
        return NULL;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalFree
    if (!LocalFree( (PSTR)LOWORD(hhMem) ))
    {
        if ((hhHeap = FindHeap( hhHeap, hhMem )))
        {
            if ((hhHeap->dwAllocs--)==0)    // If no more then try to shrink
                LocalCompact( 2048 );
        }
        hhMem = NULL;
    }

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return TRUE;
#endif

} /* cekmmHeapFree() */

//*************************************************************
//
//  cekmmHeapLock()
//
//  Purpose:
//		Locks the memorys and returns a LPVOID to it
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hhMem
//      
//
//  Return: (LPVOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPVOID WINAPI cekmmHeapLock(HHEAP hhHeap, HHANDLE hhMem)
{
#ifdef WIN32
    return hhMem ;
#else
    HANDLE hOldDS;
    LPVOID  lpMem;
    
    if (!hhHeap || !hhMem)
        return NULL;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalLock
    lpMem = (LPVOID)LocalLock( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return lpMem;
#endif
} /* cekmmHeapLock() */

//*************************************************************
//
//  cekmmHeapUnlock()
//
//  Purpose:
//		unlocks the memory
//
//
//  Parameters:
//      HHEAP   hhHeap
//      HHANDLE hhMem
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

BOOL WINAPI cekmmHeapUnlock(HHEAP hhHeap, HHANDLE hhMem)
{
#ifdef WIN32
    return TRUE ;
#else
    HANDLE hOldDS;
    BOOL   bF;
    
    if (!hhHeap || !hhMem)
        return FALSE;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalUnlock
    bF = LocalUnlock( (PSTR)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return bF;
#endif

} /* cekmmHeapUnlock() */

//*************************************************************
//
//  cekmmHeapAllocLock()
//
//  Purpose:
//		Attempts to alloc and lock a piece of memory
//
//
//  Parameters:
//      HHEAP hhHeap
//      UINT uiFlags
//      UINT uiSize
//      
//
//  Return: (LPVOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

LPVOID WINAPI cekmmHeapAllocLock(HHEAP hhHeap, UINT uiFlags, UINT uiSize)
{
#ifdef WIN32
    return cekmmHeapAlloc( hhHeap, uiFlags, uiSize ) ;
#else
    HHANDLE hhMem = cekmmHeapAlloc(hhHeap, uiFlags, uiSize);
    if (hhMem)
    {
        LPVOID lp = cekmmHeapLock(hhHeap, hhMem);
        if (lp)
            return lp;
        cekmmHeapFree( hhHeap, hhMem );
    }
    return NULL;
#endif
} /* cekmmHeapAllocLock() */

//*************************************************************
//
//  cekmmHeapAllocString()
//
//  Purpose:
//		Attempts allocate memory for the string and \0 and then stores it
//
//
//  Parameters:
//      HHEAP hhHeap
//      UINT  uiFlags
//      LPSTR lpString
//      
//
//  Return: (LPSTR WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPSTR WINAPI cekmmHeapAllocString(HHEAP hhHeap, UINT uiFlags,LPSTR lpString)
{
    LPSTR lp;

    if (!lpString)
        lpString = (LPSTR)"\0";

    lp = cekmmHeapAllocLock( hhHeap, uiFlags, lstrlen(lpString)+1 );
    if (lp)
        _fstrcpy( lp, lpString );
    return lp;

} /* cekmmHeapAllocString() */

//*************************************************************
//
//  cekmmHeapUnlockFree()
//
//  Purpose:
//		Unlocks and then frees some memory
//
//
//  Parameters:
//      HHEAP hhHeap
//      LPVOID lpMem
//      
//
//  Return: (LPVOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL WINAPI cekmmHeapUnlockFree(HHEAP hhHeap, LPVOID lpMem)
{
#ifdef WIN32
    return cekmmHeapFree( hhHeap, lpMem ) ;
#else
    HHANDLE hhMem;
    
    if ((hhMem = cekmmHeapHandle( hhHeap, lpMem )))
    {
        if (cekmmHeapUnlock(hhHeap, hhMem ))
            cekmmHeapFree(hhHeap, hhMem );
        else
            return lpMem;
    }
    return NULL;
#endif
} /* cekmmHeapUnlockFree() */

//*************************************************************
//
//  cekmmHeapCompact()
//
//  Purpose:
//		Attempts to compact the heap
//
//
//  Parameters:
//      HHEAP hhHeap
//      UINT wMinFree
//      
//
//  Return: (UINT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

UINT WINAPI cekmmHeapCompact(HHEAP hhHeap, UINT wMinFree)
{
#ifdef WIN32
    return (UINT)-1 ;
#else
    HANDLE hOldDS;
    UINT   uiSize;
    
    if (!hhHeap)
        return FALSE;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhHeap
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalUnlock
    uiSize = LocalCompact( wMinFree );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (hhHeap->hhNextHeap)
        return cekmmHeapCompact(hhHeap->hhNextHeap, wMinFree);
    return uiSize;
#endif
} /* cekmmHeapCompact() */

//*************************************************************
//
//  cekmmHeapFlags()
//
//  Purpose:
//		Returns the flags that describe the memory block
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hhMem
//      
//
//  Return: (UINT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

UINT WINAPI cekmmHeapFlags(HHEAP hhHeap, HHANDLE hhMem)
{
#ifdef WIN32
    return hhHeap->wType ;
#else
    HANDLE  hOldDS;
    UINT    uiFlags;
    
    if (!hhHeap || !hhMem)
        return FALSE;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,hhMem
        pop         ax  
        mov         hOldDS, ax
    }

// Now we can do a LocalUnlock
    uiFlags = LocalFlags( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return uiFlags;
#endif
} /* cekmmHeapFlags() */

//*************************************************************
//
//  cekmmHeapHandle()
//
//  Purpose:
//		Returns the handle of the memory block the LPVOID points to
//
//
//  Parameters:
//      HHEAP hhHeap
//      LPVOID lpMem
//      
//
//  Return: (HHANDLE WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE WINAPI cekmmHeapHandle(HHEAP hhHeap, LPVOID lpMem)
{
#ifdef WIN32
    return (HHANDLE)lpMem ;
#else
    HANDLE  hOldDS;
    HANDLE  hMem;
    
    if (!hhHeap || !lpMem)
        return NULL;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax,lpMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalHandle
    hMem = LocalHandle( (PSTR)LOWORD((LONG)lpMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (hMem)

        return MAKELONG(hMem,HIWORD((LONG)lpMem));
    else
        return NULL;
#endif
} /* cekmmHeapHandle() */

//*************************************************************
//
//  cekmmHeapDiscard()
//
//  Purpose:
//		Discards the memory if possible
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hMem
//      
//
//  Return: (HHANDLE WINAPI)
//      
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE WINAPI cekmmHeapDiscard(HHEAP hhHeap,HHANDLE hhMem)
{
#ifdef WIN32
    return hhMem ;
#else
    return cekmmHeapReAlloc(hhHeap, hhMem, 0, LMEM_MOVEABLE );
#endif

} /* cekmmHeapDiscard() */

//*************************************************************
//
//  cekmmHeapSize()
//
//  Purpose:
//		Returns the size of the allocated heap chunk
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hhMem
//      
//
//  Return: (UINT WINAPI)
//      size
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

UINT WINAPI cekmmHeapSize(HHEAP hhHeap, HHANDLE hhMem)
{
#ifdef WIN32
    return HeapSize( hhHeap, hhMem ) ;
#else
    HANDLE hOldDS;
    UINT   uiSize;
    
    if (!hhHeap || !hhMem)
        return FALSE;

// First Set the DS to the heap
    _asm
    {
        push        ds
        lds         ax, hhMem
        pop         ax
        mov         hOldDS, ax
    }

// Now we can do a LocalUnlock
    uiSize = LocalSize( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return uiSize;
#endif

} /* cekmmHeapSize() */

//*************************************************************
//
//  cekmmHeapSetDS()
//
//  Purpose:
//		Sets the current DS to the heap
//
//
//  Parameters:
//      HHEAP hhHeap - either a valid heap or a (HHEAP)hOldDS
//      
//
//  Return: (HHANDLE WINAPI)
//      Old DS    
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

HHANDLE WINAPI cekmmHeapSetDS(HHEAP hhHeap)
{
#ifdef WIN32
    return NULL ;
#else
    HANDLE hOldDS;

    hOldDS = (HANDLE)HIWORD((LONG)hhHeap);
    
    if (!hhHeap)
        return NULL;

// First Set the DS to the heap
    _asm
    {
        push        ds
        mov         ax, hOldDS
        mov         ds,ax
        pop         ax
        mov         hOldDS, ax
    }
    return MAKELONG(hOldDS,0L); 
#endif
} /* cekmmHeapSetDS() */

//*************************************************************
//
//  cekmmHeapItems()
//
//  Purpose:
//		Returns the number of items that have been cekmmHeapAlloc'ed
//      and not cekmmHeapFree'ed.
//
//  Parameters:
//      HHEAP hhHeap
//      
//
//  Return: (DWORD WINAPI)
//      The number or 0L if invalid heap
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

DWORD WINAPI cekmmHeapItems(HHEAP hhHeap)
{
#ifdef WIN32
    return hhHeap->dwAllocs ;
#else
    DWORD dwItems = 0;

    while (hhHeap)
    {
        dwItems += hhHeap->dwAllocs;
        hhHeap = hhHeap->hhNextHeap;
    }
    return dwItems;
#endif
} /* cekmmHeapItems() */

//*************************************************************
//
//  cekmmHeapDumpDebug()
//
//  Purpose:
//		Dumps the debug list to the terminal
//
//
//  Parameters:
//      HHEAP hhHeap
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/10/91   MSM        Created
//
//*************************************************************

VOID WINAPI cekmmHeapDumpDebug(HHEAP hhHeap)
{
#ifdef WIN32
    return ;
#else
    char temp[80];
    int  nHeaps=1;

    OutputDebugString( "\r\nHeap Manager V1.00 (Debug Info)\r\n" );
    OutputDebugString( "Written by Michael S. McCraw  Copyright @1992\r\n" );
    OutputDebugString("------------------------------------------------\r\n");

    if (hhHeap) // Dump first heap and big header
    {
        wsprintf( temp, "  HHEAP: %#08lx  Items: %ld  wType: ", hhHeap,
            cekmmHeapItems(hhHeap) );
        if (hhHeap->wType & HT_SHARED)
            _fstrcat( temp, "SHARED ");
        if (hhHeap->wType & HT_MULTIPLE)
            _fstrcat( temp, "MULTIPLE ");
        OutputDebugString( temp );    
        OutputDebugString( "\r\n" );    
        if ( !(hhHeap->wType&HT_MULTIPLE) )
            return;
    }

    while (hhHeap)
    {
        wsprintf( temp, "      SUBHEAP: %d  HHEAP: %#08lx  Items: %ld\r\n", 
            nHeaps, hhHeap, hhHeap->dwAllocs );
        OutputDebugString( temp );    
        hhHeap = hhHeap->hhNextHeap;
        nHeaps++;
    }
    OutputDebugString( "\r\n" );    
#endif
} /* cekmmHeapDumpDebug() */



/*** EOF: heap.c ***/

