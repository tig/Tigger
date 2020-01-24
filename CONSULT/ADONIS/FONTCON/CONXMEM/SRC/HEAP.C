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
#include "intfsmm.h"


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
    while (hhHeap)
    {
        if (HIWORD(hhHeap)==HIWORD(hhMem))
            return hhHeap;
        hhHeap = hhHeap->hhNextHeap;
    }
    return NULL;

} /* FindHeap() */


//*************************************************************
//
//  fsmmHeapCreate()
//
//  Purpose:
//		This function creates a new heap and returns a handle to it
//
//
//  Parameters:
//      WORD wSize  - Initial size of the heap
//      WORD wFlags - Flags to create the heap with: SHARED or MULTIPLE
//      
//
//  Return: (HHEAP FAR PASCAL)
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

HHEAP FAR PASCAL fsmmHeapCreate(WORD wSize, WORD wFlags)
{
    HANDLE  hOldDS, hMem = NULL;
    WORD    wMemFlags = GMEM_MOVEABLE|GMEM_ZEROINIT;
    LPSTR   lpMem;
    HEAP   *pHeap;

    if (wFlags & HT_SHARED)
        wMemFlags |= GMEM_DDESHARE;

    if ( wSize<2048)
        wSize = 2048;
    else
        wSize&=0xFFF0; // round to 16

// Allocate memory for the heap
    if (!(hMem = GlobalAlloc( wMemFlags, (DWORD)wSize )))
        return NULL;
    
// Get the selector rights
    if (!(lpMem = GlobalLock( hMem )))
    {
create_failure:
        GlobalFree( hMem );
        return NULL;
    }
// Initialize the heap
    if (!LocalInit( HIWORD((LONG)lpMem), 0, wSize-1 ))
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
        pHeap->wType= wFlags;

    lpMem = (LPSTR)pHeap;

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (pHeap==NULL)    // Alloc failed
        goto create_failure;

    return (HHEAP)lpMem;

} /* fsmmHeapCreate() */

//*************************************************************
//
//  fsmmHeapDestroy()
//
//  Purpose:
//		Destroys a heap that was created with fsmmHeapCreate()
//
//
//  Parameters:
//
//      HHEAP hHeap - Handle returned by fsmmHeapCreate()
//      
//
//  Return: (VOID FAR PASCAL)
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

VOID FAR PASCAL fsmmHeapDestroy( HHEAP hhHeap )
{

    while (hhHeap)  // Walk the chain and kill all the heaps
    {
        HANDLE hMem = (HANDLE)HIWORD( (LONG)hhHeap );
    
        hhHeap = hhHeap->hhNextHeap;
        GlobalUnlock( hMem );
        GlobalFree( hMem );
    }
    return;

} /* fsmmHeapDestroy() */

//*************************************************************
//
//  fsmmHeapAlloc()
//
//  Purpose:
//		Allocates a piece of memory from the heap
//
//
//  Parameters:
//      HHEAP hhHeap - Heap to alloc from
//      WORD  wFlags - Flags for allocation
//      WORD  wSize  - Size of allocation
//      
//
//  Return: (HHANDLE FAR PASCAL)
//      handle to the memory if successful, NULL otherwise
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE FAR PASCAL fsmmHeapAlloc( HHEAP hhHeap, WORD wFlags, WORD wSize )
{
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
    hMem = (DWORD)(LPSTR)LocalAlloc( wFlags, wSize );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }

    if (LOWORD(hMem))
        hhHeap->dwAllocs++;
    else    // No dice, check for multiple and empty
    {
        if (hhHeap->wType & HT_MULTIPLE)    // try next heap again
        {
            HHEAP hhH;

            if (hhHeap->hhNextHeap) // Recurse the list
                return fsmmHeapAlloc(hhHeap->hhNextHeap, wFlags, wSize );

            if (hhHeap->dwAllocs==0)    // Big as it gets, fail the call
                return NULL;

            hhH = fsmmHeapCreate( 0, hhHeap->wType );
        
            if (hhH)
            {
                HHANDLE hhMem;
                hhMem = fsmmHeapAlloc(hhH, wFlags, wSize );
                if (hhMem)
                    hhHeap->hhNextHeap = hhH;
                else
                    fsmmHeapDestroy( hhH );
                return hhMem;
            }
        }
        return NULL;
    }
    return MAKELONG(hMem,HIWORD(hhHeap));

} /* fsmmHeapAlloc() */

//*************************************************************
//
//  fsmmHeapReAlloc()
//
//  Purpose:
//		Attempts to realloc the chunk of memory in the heap.
//      Will not move to new heap!!!!!
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hMem
//      WORD wSize
//      WORD wFlags
//      
//
//  Return: (HHANDLE FAR PASCAL)
//      New handle to memory
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE FAR PASCAL fsmmHeapReAlloc(HHEAP hhHeap,HHANDLE hhMem,WORD wSize, WORD wFlags)
{
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
    hMem = LocalReAlloc( (LOCALHANDLE)LOWORD(hhMem), wSize, wFlags );

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

} /* fsmmHeapReAlloc() */

//*************************************************************
//
//  fsmmHeapFree()
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
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE FAR PASCAL fsmmHeapFree(HHEAP hhHeap, HHANDLE hhMem)
{
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
    if (!LocalFree( (LOCALHANDLE)LOWORD(hhMem) ))
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
    return hhMem;

} /* fsmmHeapFree() */

//*************************************************************
//
//  fsmmHeapLock()
//
//  Purpose:
//		Locks the memorys and returns a LPSTR to it
//
//
//  Parameters:
//      HHEAP hhHeap
//      HHANDLE hhMem
//      
//
//  Return: (LPSTR FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPSTR FAR PASCAL fsmmHeapLock(HHEAP hhHeap, HHANDLE hhMem)
{
    HANDLE hOldDS;
    LPSTR  lpMem;
    
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
    lpMem = (LPSTR)LocalLock( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (LOWORD((LONG)lpMem)==0)
        return NULL;
    return lpMem;

} /* fsmmHeapLock() */

//*************************************************************
//
//  fsmmHeapUnlock()
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
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

BOOL FAR PASCAL fsmmHeapUnlock(HHEAP hhHeap, HHANDLE hhMem)
{
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
    bF = LocalUnlock( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return bF;

} /* fsmmHeapUnlock() */

//*************************************************************
//
//  fsmmHeapAllocLock()
//
//  Purpose:
//		Attempts to alloc and lock a piece of memory
//
//
//  Parameters:
//      HHEAP hhHeap
//      WORD wFlags
//      WORD wSize
//      
//
//  Return: (LPSTR FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

LPSTR FAR PASCAL fsmmHeapAllocLock(HHEAP hhHeap, WORD wFlags, WORD wSize)
{
    HHANDLE hhMem = fsmmHeapAlloc(hhHeap, wFlags, wSize);
    if (hhMem)
    {
        LPSTR lp = fsmmHeapLock(hhHeap, hhMem);
        if (lp)
            return lp;
        fsmmHeapFree( hhHeap, hhMem );
    }
    return NULL;

} /* fsmmHeapAllocLock() */

//*************************************************************
//
//  fsmmHeapAllocString()
//
//  Purpose:
//		Attempts allocate memory for the string and \0 and then stores it
//
//
//  Parameters:
//      HHEAP hhHeap
//      WORD  wFlags
//      LPSTR lpString
//      
//
//  Return: (LPSTR FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPSTR FAR PASCAL fsmmHeapAllocString(HHEAP hhHeap, WORD wFlags,LPSTR lpString)
{
    LPSTR lp;

    if (!lpString)
        lpString = (LPSTR)"\0";

    lp = fsmmHeapAllocLock( hhHeap, wFlags, lstrlen(lpString)+1 );
    if (lp)
        _fstrcpy( lp, lpString );
    return lp;

} /* fsmmHeapAllocString() */

//*************************************************************
//
//  fsmmHeapUnlockFree()
//
//  Purpose:
//		Unlocks and then frees some memory
//
//
//  Parameters:
//      HHEAP hhHeap
//      LPSTR lpMem
//      
//
//  Return: (LPSTR FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LPSTR FAR PASCAL fsmmHeapUnlockFree(HHEAP hhHeap, LPSTR lpMem)
{
    HHANDLE hhMem;
    
    if ((hhMem = fsmmHeapHandle( hhHeap, lpMem )))
    {
        if (fsmmHeapUnlock(hhHeap, hhMem ))
            fsmmHeapFree(hhHeap, hhMem );
        else
            return lpMem;
    }
    return NULL;

} /* fsmmHeapUnlockFree() */

//*************************************************************
//
//  fsmmHeapCompact()
//
//  Purpose:
//		Attempts to compact the heap
//
//
//  Parameters:
//      HHEAP hhHeap
//      WORD wMinFree
//      
//
//  Return: (WORD FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

WORD FAR PASCAL fsmmHeapCompact(HHEAP hhHeap, WORD wMinFree)
{
    HANDLE hOldDS;
    WORD   wSize;
    
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
    wSize = LocalCompact( wMinFree );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    if (hhHeap->hhNextHeap)
        return fsmmHeapCompact(hhHeap->hhNextHeap, wMinFree);
    return wSize;

} /* fsmmHeapCompact() */

//*************************************************************
//
//  fsmmHeapFlags()
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
//  Return: (WORD FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

WORD FAR PASCAL fsmmHeapFlags(HHEAP hhHeap, HHANDLE hhMem)
{
    HANDLE hOldDS;
    WORD    wFlags;
    
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
    wFlags = LocalFlags( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return wFlags;

} /* fsmmHeapFlags() */

//*************************************************************
//
//  fsmmHeapHandle()
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
//  Return: (HHANDLE FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE FAR PASCAL fsmmHeapHandle(HHEAP hhHeap, LPVOID lpMem)
{
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

} /* fsmmHeapHandle() */

//*************************************************************
//
//  fsmmHeapDiscard()
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
//  Return: (HHANDLE FAR PASCAL)
//      
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

HHANDLE FAR PASCAL fsmmHeapDiscard(HHEAP hhHeap,HHANDLE hhMem)
{
    return fsmmHeapReAlloc(hhHeap, hhMem, 0, LMEM_MOVEABLE );

} /* fsmmHeapDiscard() */

//*************************************************************
//
//  fsmmHeapSize()
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
//  Return: (WORD FAR PASCAL)
//      size
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

WORD FAR PASCAL fsmmHeapSize(HHEAP hhHeap, HHANDLE hhMem)
{
    HANDLE hOldDS;
    WORD   wSize;
    
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
    wSize = LocalSize( (LOCALHANDLE)LOWORD(hhMem) );

// Restore DS
    _asm
    {
        mov         ax, hOldDS
        mov         ds,ax
    }
    return wSize;

} /* fsmmHeapSize() */

//*************************************************************
//
//  fsmmHeapSetDS()
//
//  Purpose:
//		Sets the current DS to the heap
//
//
//  Parameters:
//      HHEAP hhHeap - either a valid heap or a (HHEAP)hOldDS
//      
//
//  Return: (HHANDLE FAR PASCAL)
//      Old DS    
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

HHANDLE FAR PASCAL fsmmHeapSetDS(HHEAP hhHeap)
{
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

} /* fsmmHeapSetDS() */

//*************************************************************
//
//  fsmmHeapItems()
//
//  Purpose:
//		Returns the number of items that have been fsmmHeapAlloc'ed
//      and not fsmmHeapFree'ed.
//
//  Parameters:
//      HHEAP hhHeap
//      
//
//  Return: (DWORD FAR PASCAL)
//      The number or 0L if invalid heap
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

DWORD FAR PASCAL fsmmHeapItems(HHEAP hhHeap)
{
    DWORD dwItems = 0;

    while (hhHeap)
    {
        dwItems += hhHeap->dwAllocs;
        hhHeap = hhHeap->hhNextHeap;
    }
    return dwItems;

} /* fsmmHeapItems() */

//*************************************************************
//
//  fsmmHeapDumpDebug()
//
//  Purpose:
//		Dumps the debug list to the terminal
//
//
//  Parameters:
//      HHEAP hhHeap
//      
//
//  Return: (VOID FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/10/91   MSM        Created
//
//*************************************************************

VOID FAR PASCAL fsmmHeapDumpDebug(HHEAP hhHeap)
{
    char temp[80];
    int  nHeaps=1;

    OutputDebugString( "\r\nHeap Manager V1.00 (Debug Info)\r\n" );
    OutputDebugString( "Written by Michael S. McCraw  Copyright @1992\r\n" );
    OutputDebugString("------------------------------------------------\r\n");

    if (hhHeap) // Dump first heap and big header
    {
        wsprintf( temp, "  HHEAP: %#08lx  Items: %ld  wType: ", hhHeap,
            fsmmHeapItems(hhHeap) );
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

} /* fsmmHeapDumpDebug() */



/*** EOF: heap.c ***/

