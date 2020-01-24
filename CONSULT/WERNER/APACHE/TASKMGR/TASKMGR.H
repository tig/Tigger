//***************************************************************************
//
//  Module: taskmgr.h
//
//  Purpose:
//     This is the header file for the TASKMGR task management
//     dynamic link library.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              8/24/91   BryanW      Wrote it.
//              1/11/92   BryanW      Added this comment block.
//                                    
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

// constant definitions

#define WM_TASKTIMR         (WM_USER + 0x8000)

/*** predefined task functions ***/

#define TF_INITTASK   0x0000   // initial state
#define TF_IDLESTATE  0x0001   // when task is idle
#define TF_EXITTASK   0x0002   // when a task has completed
#define TF_USER       0x0100   // beginning of user defined functions

/*** task style flags ***/

#define TSF_DEFAULT  0x000
#define TSF_NOTIFY   0x001

// data structures

#if (WINVER < 0x30a)
typedef HANDLE HTASKDB ;
#else
DECLARE_HANDLE( HTASKDB ) ;
#endif

#if (WINVER < 0x30a)
typedef HANDLE HTASKLIST ;
#else
DECLARE_HANDLE( HTASKLIST ) ;
#endif

typedef LRESULT (FAR PASCAL *LPFNTASKPROC)( HWND, HTASKDB, UINT,
                                            WPARAM, LPARAM ) ;

// function prototypes (exported)

BOOL FAR PASCAL AddTask( HTASKLIST, HTASKDB ) ;
HTASKDB FAR PASCAL CreateTaskDatabase( WORD, WORD, WORD, WORD, LPFNTASKPROC ) ;
HTASKLIST FAR PASCAL CreateTaskList( VOID ) ;
BOOL FAR PASCAL DestroyTaskDatabase( HTASKDB ) ;
BOOL FAR PASCAL DestroyTaskList( HTASKLIST ) ;
LONG FAR PASCAL ExecuteTask( HTASKDB, WORD, WORD, LONG ) ;
WORD FAR PASCAL GetTaskWord( HTASKDB, int ) ;
HANDLE FAR PASCAL TaskLocalAlloc( HTASKDB, WORD, WORD ) ;
WORD FAR PASCAL TaskLocalCompact( HTASKDB, WORD ) ;
HANDLE FAR PASCAL TaskLocalDiscard( HTASKDB, HANDLE ) ;
WORD FAR PASCAL TaskLocalFlags( HTASKDB, HANDLE ) ;
HANDLE FAR PASCAL TaskLocalFree( HTASKDB, HANDLE ) ;
LPSTR FAR PASCAL TaskLocalLock( HTASKDB, HANDLE ) ;
HANDLE FAR PASCAL TaskLocalReAlloc( HTASKDB, HANDLE, WORD, WORD ) ;
WORD FAR PASCAL TaskLocalSize( HTASKDB, HANDLE ) ;
BOOL FAR PASCAL TaskLocalUnlock( HTASKDB, HANDLE ) ;
WORD FAR PASCAL TaskStackPop( HTASKDB ) ;
BOOL FAR PASCAL TaskStackPush( HTASKDB, WORD ) ;
BOOL FAR PASCAL KillTaskTimer( HWND ) ;
BOOL FAR PASCAL RemoveTask( HTASKLIST, HTASKDB ) ;
BOOL FAR PASCAL SetTaskTimer( HWND ) ;
WORD FAR PASCAL SetTaskWord( HTASKDB, int, WPARAM ) ;
VOID FAR PASCAL TaskManagerSlice( HWND, HANDLE ) ;

//***************************************************************************
//  End of File: taskmgr.h
//***************************************************************************

