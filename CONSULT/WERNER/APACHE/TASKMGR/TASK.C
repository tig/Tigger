//***************************************************************************
//
//  Module: task.c
//
//  Purpose:
//     Task creation/destruction functions for the TASKMGR DLL.
//
//  Description of functions:
//     Descriptions contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              1/11/92   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

#include "precomp.h"  // precompiled stuff

#include "taskpriv.h"

//************************************************************************
//  HTASKDB FAR PASCAL CreateTaskDatabase( WORD wStackSize, WORD wHeapSize,
//                                         WORD wTaskExtraBytes,
//                                         WORD wStyleFlags,
//                                         LPFNTASKPROC lpfnTaskProc )
//
//  Description:
//     Creates a Task.  Performs the local alloc of the TASKDB structure
//     and initializes it.  Allocates the "data segment" for the task.
//     wTaskExtraBytes specifies the number of extra bytes allocated for
//     the TASKDB structure, these bytes are accessed through GetTaskWord()
//     and SetTaskWord().  wStackSize specifies a fixed stack size and
//     wHeapSize is a local heap size (minimum) will grow to satisfy
//     allocations.
//
//  Parameters:
//     WORD wStackSize
//        declared stack size
//
//     WORD wHeapSize
//        declared heap size
//
//     WORD wTaskExtraBytes
//        extra bytes to allocate with task database structure
//
//     WORD wStyleFlags
//        style flags declaring if task manager is to post a
//        notification when the task completes (e.g. task function
//        is in the exit state).
//
//    LPFNTASKPROC lpfnTaskProc
//        associated task procedure.
//
//  History:   Date       Author      Comment
//              8/18/91   BryanW      Wrote it.
//
//************************************************************************

HTASKDB FAR PASCAL CreateTaskDatabase( WORD wStackSize, WORD wHeapSize,
                                       WORD wTaskExtraBytes,
                                       WORD wStyleFlags,
                                       LPFNTASKPROC lpfnTaskProc )
{
   HTASKDB   hTaskDB ;
   NPTASKDB  npTaskDB ;

   if (NULL == (hTaskDB = LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                      sizeof( TASKDB ) + wTaskExtraBytes )))
      return ( NULL ) ;

   npTaskDB = (NPTASKDB) LocalLock( hTaskDB ) ;

   TASKDB_HEAPSIZE( npTaskDB ) = (wHeapSize / 16) * 16 ; // even para boundary
   if (TASKDB_HEAPSIZE( npTaskDB ) < TASK_MINHEAP)
      TASKDB_HEAPSIZE( npTaskDB ) = TASK_MINHEAP ;

   TASKDB_STACKSIZE( npTaskDB ) = (wStackSize / 2) * 2 ; // even word boundary
   if (TASKDB_STACKSIZE( npTaskDB ) < TASK_MINSTACK)
      TASKDB_STACKSIZE( npTaskDB ) = TASK_MINSTACK ;

   if (TASKDB_HEAPSIZE( npTaskDB ) + TASKDB_STACKSIZE( npTaskDB ) + 16 > 0xFFFF)
   {
      LocalUnlock( hTaskDB ) ;
      LocalFree( hTaskDB ) ;
      return ( NULL ) ;
   }

   TASKDB_STYLEFLAGS( npTaskDB ) = wStyleFlags ;
   TASKDB_HEAPSIZE( npTaskDB ) = wHeapSize ;
   TASKDB_STACKSIZE( npTaskDB ) = wStackSize ;
   if (NULL ==
      (TASKDB_DATAHANDLE( npTaskDB ) = GlobalAlloc( GMEM_MOVEABLE |
                                                    GMEM_ZEROINIT,
                                                    wHeapSize +
                                                    wStackSize + 16 )))
   {
      LocalUnlock( hTaskDB ) ;
      LocalFree( hTaskDB ) ;
      return ( NULL ) ;
   }
   TASKDB_DSSEL( npTaskDB ) =
      HIWORD( GlobalLock( TASKDB_DATAHANDLE( npTaskDB ) ) ) ;
   LocalInit( TASKDB_DSSEL( npTaskDB ), wStackSize + 16,
              wStackSize + wHeapSize ) ;

   // unlock the lock placed by LocalInit()
   UnlockSegment( TASKDB_DSSEL( npTaskDB ) ) ;

   TASKDB_SP( npTaskDB ) = wStackSize ;
   TASKDB_RUNTIME( npTaskDB ) = 0 ;
   TASKDB_STARTTIME( npTaskDB ) = GetSystemTime() ;
   TASKDB_FUNCTION( npTaskDB ) = TF_INITTASK ;
   TASKDB_TASKPROC( npTaskDB ) = lpfnTaskProc ;

   LocalUnlock( hTaskDB ) ;
   return ( hTaskDB ) ;

} // end of CreateTaskDatabase()

//************************************************************************
//  BOOL FAR PASCAL DestroyTaskDatabase( HTASKDB hTaskDB )
//
//  Description:
//     Destroys the Task DB info structure.
//
//  Parameters:
//     HTASKDB hTaskDB
//        handle to task database
//
//  History:   Date       Author      Comment
//              8/18/91   BryanW      Wrote it.
//              1/12/92   BryanW      Added the new comment block.
//
//************************************************************************

BOOL FAR PASCAL DestroyTaskDatabase( HTASKDB hTaskDB )
{
   NPTASKDB  npTaskDB ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( FALSE ) ;
   GlobalUnlock( TASKDB_DATAHANDLE( npTaskDB ) ) ;
   GlobalFree( TASKDB_DATAHANDLE( npTaskDB ) ) ;
   LocalUnlock( hTaskDB ) ;
   LocalFree( hTaskDB );

   return ( TRUE ) ;

} // end of DestroyTaskDatabase()

/************************************************************************
 *  WORD FAR PASCAL SetTaskWord( HTASKDB hTaskDB, int nIndex, WORD wValue )
 *
 *  Description: 
 *     Sets the word value at index nIndex in the extra byte data area.
 *     Returns the previous value of the specified word.
 *
 *  Comments:
 *      8/18/91  baw  Wrote it.
 *
 ************************************************************************/

WORD FAR PASCAL SetTaskWord( HTASKDB hTaskDB, int nIndex, WPARAM wValue )
{
   NPTASKDB  npTaskDB ;
   WORD      wRetVal ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wRetVal = *(WORD *)((WORD) npTaskDB + sizeof( TASKDB ) + nIndex ) ;
   *(WORD *)((WORD) npTaskDB + sizeof( TASKDB ) + nIndex) = (WORD) wValue ;
   LocalUnlock( hTaskDB ) ;

   return ( wRetVal ) ;

} /* end of SetTaskWord() */

/************************************************************************
 *  WORD FAR PASCAL GetTaskWord( HTASKDB hTaskDB, int nIndex )
 *
 *  Description: 
 *     Returns the value of the specified word at nIndex in the
 *     task's extra bytes data area.
 *
 *  Comments:
 *      8/18/91  baw  Wrote it.
 *
 ************************************************************************/

WORD FAR PASCAL GetTaskWord( HTASKDB hTaskDB, int nIndex )
{
   NPTASKDB  npTaskDB ;
   WORD      wRetVal ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wRetVal = *(WORD *)((WORD) npTaskDB + sizeof( TASKDB ) + nIndex) ;
   LocalUnlock( hTaskDB ) ;

   return ( wRetVal ) ;

} /* end of GetTaskWord() */

/************************************************************************
 *  BOOL FAR PASCAL TaskStackPush( HTASKDB hTaskDB, WORD wValue )
 *
 *  Description: 
 *     Pushes a value on the task's stack.  It also performs
 *     stack checking.  The stack has overflowed when it is attempts
 *     to push beyond SP = 0x10.  The first 16 bytes of a SS/DS
 *     combo is reserved for the local heap.
 *
 *  Comments:
 *      8/18/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL TaskStackPush( HTASKDB hTaskDB, WORD wValue )
{
   NPTASKDB  npTaskDB ;
   LPWORD    lpStackSP ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( FALSE ) ;

   // check for room to push

   if (0x10 <= TASKDB_SP( npTaskDB ))
   {
      LocalUnlock( hTaskDB ) ;
      return ( FALSE ) ;
   }
   lpStackSP = (LPWORD) MAKELONG( TASKDB_SP( npTaskDB ),
                                  TASKDB_DSSEL( npTaskDB ) ) ;
   *--lpStackSP = wValue ;
   TASKDB_SP( npTaskDB ) = LOWORD( (DWORD) lpStackSP ) ;
   LocalUnlock( hTaskDB ) ;

   return ( TRUE ) ;

} /* end of TaskStackPush() */

/************************************************************************
 *  WORD FAR PASCAL TaskStackPop( HTASKDB hTaskDB )
 *
 *  Description: 
 *     Pops a value from the task's stack. Returns NULL if nothing is
 *     pushed or other failure.
 *
 *  Comments:
 *      8/18/91  baw  Wrote it.
 *
 ************************************************************************/

WORD FAR PASCAL TaskStackPop( HTASKDB hTaskDB )
{
   NPTASKDB  npTaskDB ;
   LPWORD    lpStackSP ;
   WORD      wRetVal ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   if (TASKDB_SP( npTaskDB ) == TASKDB_STACKSIZE( npTaskDB ))
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   lpStackSP = (LPWORD) MAKELONG( TASKDB_SP( npTaskDB ),
                                  TASKDB_DSSEL( npTaskDB ) ) ;
   wRetVal = *lpStackSP++ ;
   TASKDB_SP( npTaskDB ) = LOWORD( (DWORD) lpStackSP ) ;
   LocalUnlock( hTaskDB ) ;

   return ( wRetVal ) ;

} /* end of TaskStackPop() */

/************************************************************************
 *  HANDLE FAR PASCAL TaskLocalAlloc( HTASKDB hTaskDB, WORD wFlags,
 *                                    WORD wBytes )
 *
 *  Description: 
 *     Allocates a task specific block.  Parameters are similar to
 *     those used for a LocalAlloc().
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

HANDLE FAR PASCAL TaskLocalAlloc( HTASKDB hTaskDB, WORD wFlags, WORD wBytes )
{
   HANDLE    hMem ;
   NPTASKDB  npTaskDB ;
   WORD      wDSSel ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   hMem = LocalAlloc( wFlags, wBytes ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( hMem ) ;

} /* end of TaskLocalAlloc() */

/************************************************************************
 *  WORD FAR PASCAL TaskLocalCompact( HTASKDB hTaskDB, WORD wMinFree )
 *
 *  Description: 
 *     Performs a local compact of the task specific heap.  Parameters
 *     are similar to those used for LocalCompact().
 *
 *  Comments:
 *     9/14/91  baw  Wrote it.
 *
 ************************************************************************/

WORD FAR PASCAL TaskLocalCompact( HTASKDB hTaskDB, WORD wMinFree )
{
   NPTASKDB  npTaskDB ;
   WORD      wDSSel, wRetVal ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   wRetVal = LocalCompact( wMinFree ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( wRetVal ) ;

} /* end of TaskLocalCompact() */

/************************************************************************
 *  HANDLE FAR PASCAL TaskLocalDiscard( HTASKDB hTaskDB, HANDLE hMem )
 *
 *  Description: 
 *     Discards a task specific block.  Parameters are similar to
 *     those use for a LocalDiscard().
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

HANDLE FAR PASCAL TaskLocalDiscard( HTASKDB hTaskDB, HANDLE hMem )
{
   HANDLE    hRetVal ;
   NPTASKDB  npTaskDB ;
   WORD      wDSSel ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   hRetVal = LocalDiscard( hMem ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( hRetVal ) ;

} /* end of TaskLocalDiscard() */

/************************************************************************
 *  WORD FAR PASCAL TaskLocalFlags( HTASKDB hTaskDB, HANDLE hMem )
 *
 *  Description:
 *     Returns information about a specific task allocated block.
 *     Parameters are similar to those used for a LocalFlags().
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

WORD FAR PASCAL TaskLocalFlags( HTASKDB hTaskDB, HANDLE hMem )
{
   NPTASKDB  npTaskDB ;
   WORD      wDSSel, wRetVal ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   wRetVal = LocalFlags( hMem ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( wRetVal ) ;

} /* end of TaskLocalFlags() */

/************************************************************************
 *  HANDLE FAR PASCAL TaskLocalFree( HTASKDB hTaskDB, HANDLE hMem )
 *
 *  Description: 
 *     Frees a task specific block.  Parameters are similar to
 *     those used for a LocalFree().
 *
 *  Comments:
 *
 ************************************************************************/

HANDLE FAR PASCAL TaskLocalFree( HTASKDB hTaskDB, HANDLE hMem )
{
   HANDLE    hRetVal ;
   NPTASKDB  npTaskDB ;
   WORD      wDSSel ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   hRetVal = LocalFree( hMem ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( hRetVal ) ;

} /* end of TaskLocalFree() */

/************************************************************************
 *  LPSTR FAR PASCAL TaskLocalLock( HTASKDB hTaskDB, HANDLE hMem )
 *
 *  Description: 
 *     Locks the local memory block specified by the hMem parameter.
 *     Parameters are similar to those used for a LocalLock().
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

LPSTR FAR PASCAL TaskLocalLock( HTASKDB hTaskDB, HANDLE hMem )
{
   LPSTR     lpPtr ;
   NPTASKDB  npTaskDB ;
   PSTR      npPtr ;
   WORD      wDSSel ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   npPtr = LocalLock( hMem ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }
   LocalUnlock( hTaskDB ) ;

   if (npPtr == NULL)
      lpPtr = NULL ;
   else
      lpPtr = (LPSTR) MAKELONG( npPtr, wDSSel ) ;
   return ( lpPtr ) ;

} /* end of TaskLocalLock() */

/************************************************************************
 *  HANDLE FAR PASCAL TaskLocalReAlloc( HTASKDB hTaskDB, HANDLE hMem,
 *                                      WORD wBytes, WORD wFlags )
 *
 *  Description: 
 *     Changes the size of a task allocated memory block.  Parameters
 *     are similar to those used for a LocalReAlloc().
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

HANDLE FAR PASCAL TaskLocalReAlloc( HTASKDB hTaskDB, HANDLE hMem,
                                    WORD wBytes, WORD wFlags )
{
   HANDLE    hRetVal ;
   NPTASKDB  npTaskDB ;
   WORD      wDSSel ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   hRetVal = LocalReAlloc( hMem, wBytes, wFlags ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( hRetVal ) ;

} /* end of TaskLocalReAlloc() */

/************************************************************************
 *  WORD FAR PASCAL TaskLocalSize( HTASKDB hTaskDB, HANDLE hMem )
 *
 *  Description: 
 *     Returns the current size of the given block.  Parameters
 *     are similar to those used for a LocalSize().
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

WORD FAR PASCAL TaskLocalSize( HTASKDB hTaskDB, HANDLE hMem )
{
   NPTASKDB  npTaskDB ;
   WORD      wDSSel, wRetVal ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( NULL ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( NULL ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   wRetVal = LocalSize( hMem ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( wRetVal ) ;

} /* end of TaskLocalSize() */

/************************************************************************
 *  BOOL FAR PASCAL TaskLocalUnlock( HTASKDB hTaskDB, HANDLE hMem )
 *
 *  Description: 
 *     Unlocks the local memory block specified by the hMem
 *     parameter.
 *
 *  Comments:
 *      9/14/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL TaskLocalUnlock( HTASKDB hTaskDB, HANDLE hMem )
{
   BOOL      fRetVal ;
   NPTASKDB  npTaskDB ;
   WORD      wDSSel ;

   if (NULL == (npTaskDB = (NPTASKDB) LocalLock( hTaskDB )))
      return ( FALSE ) ;

   wDSSel = TASKDB_DSSEL( npTaskDB ) ;
   if (wDSSel == NULL)
   {
      LocalUnlock( hTaskDB ) ;
      return ( FALSE ) ;
   }
   _asm
   {
      ; save DS and set up DS for far local heap

      push  ds
      mov   ds, wDSSel
   }
   fRetVal = LocalUnlock( hMem ) ;
   _asm
   {
      ; restore DS

      pop   ds
   }

   LocalUnlock( hTaskDB ) ;
   return ( fRetVal ) ;

} /* end of TaskLocalUnlock() */

//***************************************************************************
//  End of File: task.c
//***************************************************************************

