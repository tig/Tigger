//***************************************************************************
//
//  Module: taskpriv.h
//
//  Purpose:
//     Private structure declarations for the TASKMGR DLL.
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

#include "version.h"

// exported stuff

#include "taskmgr.h"

// constant definitions

#define MAXLEN_TASKEVENTS   16
#define TICK_TIME           55
#define TASK_TIMESLICE      150/55  // approximately every 150 ms
#define CLOCKROLLOVER       ((DWORD) 1573040)

#define TASK_MINHEAP        256
#define TASK_MINSTACK       256

// data structures

typedef struct tagTASKDB
{
   DWORD         dwRunTime ;       // total run-time
   DWORD         dwStartTime ;     // time when this task started
   GLOBALHANDLE  ghDataHandle ;    // handle to DS
   HWND          hParentWnd ;      // parent window
   LPFNTASKPROC  lpfnTaskProc ;      // task procedure
   WORD          wFunction ;       // current function
   WORD          wDSSel ;          // data segment
   WORD          wHeapSize ;       // heap size
   WORD          wStackSize ;      // stack size
   WORD          wSP ;             // stack pointer 
   WORD          wStyleFlags ;     // style for the TASK
   HANDLE        hChildTask ;      // child task

} TASKDB, NEAR *NPTASKDB ;

typedef struct tagTASKNODE
{
   HANDLE  hTaskDB ;
   HANDLE  hNext ;
   HANDLE  hPrev ;

} TASKNODE, NEAR *NPTASKNODE ;

#if (WINVER < 0x30a)
typedef HANDLE HTASKNODE ;
#else
DECLARE_HANDLE( HTASKNODE ) ;
#endif

typedef struct tagTASKLIST
{
   DWORD   dwTotalRunTime ;     // what is the total run-time for the system
   DWORD   dwScheduledTime ;    // when to execute the next task
   HANDLE  hScheduledTask ;     // this is a handle to a TASK DB!
   HANDLE  hHead ;              // the head of the task list

} TASKLIST, NEAR *NPTASKLIST ;

typedef struct tagTASKEVENT
{
   HWND hNotifyWnd ;
   BOOL fPostFlag ;

} TASKEVENT ;

// macros

#define TASKDB_RUNTIME( x )            (x -> dwRunTime)
#define TASKDB_STARTTIME( x )          (x -> dwStartTime)
#define TASKDB_DATAHANDLE( x )         (x -> ghDataHandle)
#define TASKDB_PARENTWND( x )          (x -> hParentWnd)
#define TASKDB_TASKPROC( x )           (x -> lpfnTaskProc)
#define TASKDB_FUNCTION( x )           (x -> wFunction)
#define TASKDB_DSSEL( x )              (x -> wDSSel)
#define TASKDB_HEAPSIZE( x )           (x -> wHeapSize)
#define TASKDB_STACKSIZE( x )          (x -> wStackSize)
#define TASKDB_SP( x )                 (x -> wSP)
#define TASKDB_STYLEFLAGS( x )         (x -> wStyleFlags)

#define TASKLIST_TOTALRUNTIME( x )     (x -> dwTotalRunTime)
#define TASKLIST_SCHEDULEDTIME( x )    (x -> dwScheduledTime)
#define TASKLIST_SCHEDULEDTASK( x )    (x -> hScheduledTask)
#define TASKLIST_HEAD( x )             (x -> hHead)

#define TASKNODE_TASKDB( x )           (x -> hTaskDB)
#define TASKNODE_NEXT( x )             (x -> hNext)
#define TASKNODE_PREV( x )             (x -> hPrev)

// global variables

#ifdef GLOBALDEFS
int        nIdTimer ;
HINSTANCE  hInstLib ;
TASKEVENT  TaskEvents[ MAXLEN_TASKEVENTS ] ;
#else
extern int        nIdTimer ;
extern HINSTANCE  hInstLib ;
extern TASKEVENT  TaskEvents[ MAXLEN_TASKEVENTS ] ;
#endif

// function prototypes (private)

BOOL NEAR ScheduleNextTask( HTASKLIST ) ;
BOOL NEAR IsTaskScheduled( HTASKLIST ) ;
BOOL NEAR ClearTaskPostFlag( HWND ) ;
DWORD NEAR GetSystemTime( VOID ) ;

// function prototypes (exported)

int FAR PASCAL LibMain( HINSTANCE, WORD, WORD, LPSTR ) ;
VOID FAR PASCAL WEP( int ) ;
VOID FAR PASCAL TimerHandler( VOID ) ;

// function prototypes (imported or externally defined)

WORD FAR PASCAL CreateSystemTimer( int, FARPROC ) ;
WORD FAR PASCAL KillSystemTimer( WORD ) ;

//***************************************************************************
//  End of File: taskpriv.h
//***************************************************************************

