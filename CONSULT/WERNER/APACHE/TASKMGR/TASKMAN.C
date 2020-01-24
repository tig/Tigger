//***************************************************************************
//
//  Module: taskman.c
//
//  Purpose:
//     Task management functions for the TASKMGR DLL.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              8/19/91   BryanW      Wrote it.
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

/************************************************************************
 *  HTASKLIST FAR PASCAL CreateTaskList( VOID )
 *
 *  Description: 
 *     Creates the data structure for the task list.
 *
 *  Comments:
 *      8/19/91  baw  Wrote it.
 *
 ************************************************************************/

HTASKLIST FAR PASCAL CreateTaskList( VOID )
{
   return ((HTASKLIST) LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                   sizeof( TASKLIST ) )) ;
} /* end of CreateTaskList() */

/************************************************************************
 *  BOOL FAR PASCAL DestroyTaskList( HTASKLIST hTaskList )
 *
 *  Description: 
 *     Destroys the task list.  If any tasks are still in the list,
 *     the task node and the associated task DB is deleted.
 *
 *  Comments:
 *      8/19/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL DestroyTaskList( HTASKLIST hTaskList )
{
   NPTASKLIST  npTaskList ;
   HANDLE      hCurrentTask, hNextTask ;
   NPTASKNODE  npTaskNode ;

   if (NULL == (npTaskList = (NPTASKLIST) LocalLock( hTaskList )))
      return ( FALSE ) ;

   // delete all task DBs in the task list

   hCurrentTask = TASKLIST_HEAD( npTaskList ) ;
   LocalUnlock( hTaskList ) ;
   LocalFree( hTaskList ) ;

   while (NULL != hCurrentTask)
   {
      npTaskNode = (NPTASKNODE) LocalLock( hCurrentTask ) ;
      hNextTask = TASKNODE_NEXT( npTaskNode ) ;
      DestroyTaskDatabase( TASKNODE_TASKDB( npTaskNode ) ) ;
      LocalUnlock( hCurrentTask ) ;
      LocalFree( hCurrentTask ) ;
      hCurrentTask = hNextTask ;
   }

   return ( TRUE ) ;

} /* end of DestroyTaskList() */

/************************************************************************
 *  BOOL FAR PASCAL AddTask( HTASKLIST hTaskList, HTASKDB hTaskDB )
 *
 *  Description:
 *     Adds a task DB to the task list.  Since memory is allocated
 *     with LMEM_ZEROINIT, assumptions are made about NULL handles.
 *     (e.g.  I don't specifically set them!)  This is a doubly-linked
 *     list.
 *
 *  Comments:
 *      8/19/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL AddTask( HTASKLIST hTaskList, HTASKDB hTaskDB )
{
   HANDLE      hTaskNode ;
   NPTASKLIST  npTaskList ;
   NPTASKNODE  npTaskNode ;

   if (NULL == (npTaskList = (NPTASKLIST) LocalLock( hTaskList )))
      return ( FALSE ) ;

   if (NULL == (hTaskNode = LocalAlloc( LMEM_MOVEABLE | LMEM_ZEROINIT,
                                        sizeof( TASKNODE ) )))
   {
      LocalUnlock( hTaskList ) ;
      return ( FALSE ) ;
   }

   npTaskNode = (NPTASKNODE) LocalLock( hTaskNode ) ;
   TASKNODE_TASKDB( npTaskNode ) = hTaskDB ;

   if (NULL == TASKLIST_HEAD( npTaskList ))
      TASKLIST_HEAD( npTaskList ) = hTaskNode ; // new task list
   else
   {
      HANDLE      hHeadNode ;
      NPTASKNODE  npHeadNode ;

      // add to front of list

      hHeadNode = TASKLIST_HEAD( npTaskList ) ;
      npHeadNode = (NPTASKNODE) LocalLock( hHeadNode ) ;
      TASKLIST_HEAD( npTaskList ) = hTaskNode ;
      TASKNODE_PREV( npHeadNode ) = hTaskNode ;
      TASKNODE_NEXT( npTaskNode ) = hHeadNode ;
      LocalUnlock( hHeadNode ) ;
   }
   LocalUnlock( hTaskNode ) ;

   if (NULL == TASKLIST_SCHEDULEDTASK( npTaskList ))
      ScheduleNextTask( hTaskList ) ;
   LocalUnlock( hTaskList ) ;

   return ( TRUE ) ;

} /* end of AddTask() */

/************************************************************************
 *  BOOL FAR PASCAL RemoveTask( HANDLE hTaskList, HANDLE hTaskDB )
 *
 *  Description:
 *     Removes the given task DB from the task list.  This is a brute
 *     force algorithm.  We start at the top and look at the Task DB
 *     handles in each task node.  If we find the handle, we remove the
 *     task node.
 *
 *  Comments:
 *      8/19/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL RemoveTask( HTASKLIST hTaskList, HTASKDB hTaskDB )
{
   HTASKNODE   hTaskNode,
               hNextNode,
               hPrevNode ;
   NPTASKLIST  npTaskList ;
   NPTASKNODE  npTaskNode,
               npNextNode,
               npPrevNode ;

   if (NULL == (npTaskList = (NPTASKLIST) LocalLock( hTaskList )))
      return ( FALSE ) ;

   // search incrementally through the list

   hTaskNode = TASKLIST_HEAD( npTaskList ) ;
   while (hTaskNode)
   {
      npTaskNode = (NPTASKNODE) LocalLock( hTaskNode ) ;
      if (TASKNODE_TASKDB( npTaskNode ) == hTaskDB)
      {
         LocalUnlock( hTaskNode ) ;
         break ;
      }
      hNextNode = TASKNODE_NEXT( npTaskNode ) ;
      LocalUnlock( hTaskNode ) ;
      hTaskNode = hNextNode ;
   }
   if (NULL == hTaskNode)
   {
      LocalUnlock( hTaskList ) ;
      return ( FALSE ) ;
   }

   npTaskNode = (NPTASKNODE) LocalLock( hTaskNode ) ;
   hNextNode = TASKNODE_NEXT( npTaskNode ) ;
   hPrevNode = TASKNODE_PREV( npTaskNode ) ;

   if (NULL != hPrevNode)
   {
      npPrevNode = (NPTASKNODE) LocalLock( hPrevNode ) ;
      TASKNODE_NEXT( npPrevNode ) = hNextNode ;
      LocalUnlock( hPrevNode ) ;
   }
   else
      TASKLIST_HEAD( npTaskList ) = hNextNode ;

   if (NULL != hNextNode)
   {
      npNextNode = (NPTASKNODE) LocalLock( hNextNode ) ;
      TASKNODE_PREV( npNextNode ) = hPrevNode ;
      LocalUnlock( hNextNode ) ;
   }

   // if task being removed is the currently scheduled task,
   // reschedule another task.

   if (TASKLIST_SCHEDULEDTASK( npTaskList ) == hTaskDB)
   {
      TASKLIST_SCHEDULEDTASK( npTaskList ) = NULL ;
      ScheduleNextTask( hTaskList ) ;
   }

   // unlock and free the task node
   LocalUnlock( hTaskNode ) ;
   LocalFree( hTaskNode ) ;

   // unlock the task list
   LocalUnlock( hTaskList ) ;
   return ( TRUE ) ;

} /* end of RemoveTask() */

/************************************************************************
 *  VOID FAR PASCAL TaskManagerSlice( HWND hWnd, HANDLE hTaskList )
 *
 *  Description: 
 *     Handles the processing of the Task Manager time slice.
 *     Executes the current pending task.  Measures the execution time,
 *     updates the TaskDB accordingly and then schedules the next task.
 *     Clears the post flag regardless of execution status.
 *
 *  Comments:
 *      9/ 2/91  baw  This made more sense in the TASKMGR DLL.
 *
 ************************************************************************/

VOID FAR PASCAL TaskManagerSlice( HWND hWnd, HANDLE hTaskList )
{
   DWORD       dwStartTime ;
   HANDLE      hTaskDB ;
   LONG        lRetVal ;
   NPTASKLIST  npTaskList ;
   NPTASKDB    npTaskDB ;

   DPF5( hWDB, "%04x\r\n", LOWORD( GetCurrentTime() ) ) ;

   if (IsTaskScheduled( hTaskList ))
   {
      if (NULL != (npTaskList = (NPTASKLIST) LocalLock( hTaskList )))
      {
         hTaskDB = TASKLIST_SCHEDULEDTASK( npTaskList ) ;
         npTaskDB = (NPTASKDB) LocalLock( hTaskDB ) ;
         if ((TF_IDLESTATE != TASKDB_FUNCTION( npTaskDB )) &&
             (TF_EXITTASK != TASKDB_FUNCTION( npTaskDB )))
         {
            dwStartTime = GetSystemTime() ;
            lRetVal = ExecuteTask( hTaskDB, TASKDB_FUNCTION( npTaskDB ),
                                   0, 0L ) ;
            TASKDB_RUNTIME( npTaskDB ) +=
               (DWORD)(GetSystemTime() - dwStartTime) ;
            TASKDB_FUNCTION( npTaskDB ) = LOWORD( lRetVal ) ;
            DPF5( hWDB, "Task handle: %04x, runtime: %ul\r\n",
                  hTaskDB, TASKDB_RUNTIME( npTaskDB ) ) ;
         }
         LocalUnlock( hTaskDB ) ;
         LocalUnlock( hTaskList ) ;
         ScheduleNextTask( hTaskList ) ;
      }
   }
   ClearTaskPostFlag( hWnd ) ;

} /* end of TaskManagerSlice() */

/************************************************************************
 *  LONG FAR PASCAL ExecuteTask( HTASKDB hTaskDB, WORD wFunction,
 *                               WORD wParam, LONG lParam )
 *
 *  Description:
 *     Executes the task function with given parameters.
 *
 *  Comments:
 *      8/24/91  baw  Wrote it.
 *
 ************************************************************************/

LONG FAR PASCAL ExecuteTask( HTASKDB hTaskDB, WORD wFunction, WORD wParam,
                             LONG lParam )
{
   LONG        lRetVal ;
   NPTASKDB    npTaskDB ;

   npTaskDB = (NPTASKDB) LocalLock( hTaskDB ) ;
   lRetVal =
      (*TASKDB_TASKPROC( npTaskDB ))( TASKDB_PARENTWND( npTaskDB ),
                                      hTaskDB, wFunction, wParam, lParam ) ;
   LocalUnlock( hTaskDB ) ;
   return ( lRetVal ) ;

} /* end of ExecuteTask() */

/************************************************************************
 *  BOOL NEAR ScheduleNextTask( HTASKLIST hTaskList )
 *
 *  Description:
 *     Walks the task list and determines (by execution time) which
 *     task executes next.  The scheduled time is based on a constant
 *     value in the header file, however, all tasks are executed in
 *     a round-robin fashion based on current run-time values.  Task
 *     with least amount of run-time is executed next.
 *
 *     If a new task has been entered into the list, the start time
 *     will be registered when we walk the list.
 *
 *     Yeah, kinda simplistic, but hell, I'm not writing an operating
 *     system.
 *
 *  Comments:
 *      8/19/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL NEAR ScheduleNextTask( HTASKLIST hTaskList )
{
   DWORD       dwScheduleTime ;
   HANDLE      hCurrentNode, hNextNode, hScheduledNode ;
   NPTASKLIST  npTaskList ;
   NPTASKNODE  npCurrentNode, npScheduledNode ;
   NPTASKDB    npCurrentTaskDB, npScheduledTaskDB ;

   if (NULL == (npTaskList = (NPTASKLIST) LocalLock( hTaskList )))
      return ( FALSE ) ;

   hScheduledNode = TASKLIST_HEAD( npTaskList ) ;  // could be NULL
   hCurrentNode = hScheduledNode ;
   dwScheduleTime = GetSystemTime() ;
   while (NULL != hCurrentNode)
   {
      npScheduledNode =
         (NPTASKNODE) LocalLock( hScheduledNode ) ;
      npScheduledTaskDB =
         (NPTASKDB) LocalLock( TASKNODE_TASKDB( npScheduledNode ) ) ;
      npCurrentNode =
         (NPTASKNODE) LocalLock( hCurrentNode ) ;
      npCurrentTaskDB =
         (NPTASKDB) LocalLock( TASKNODE_TASKDB( npCurrentNode ) ) ;
      if ((dwScheduleTime -
           TASKDB_STARTTIME( npCurrentTaskDB )) -
           TASKDB_RUNTIME( npCurrentTaskDB ) <
          (dwScheduleTime -
           TASKDB_STARTTIME( npScheduledTaskDB )) -
           TASKDB_RUNTIME( npScheduledTaskDB ))
         hScheduledNode = hCurrentNode ;
      hNextNode = TASKNODE_NEXT( npCurrentNode ) ;
      LocalUnlock( TASKNODE_TASKDB( npCurrentNode ) ) ;
      LocalUnlock( hCurrentNode ) ;
      LocalUnlock( TASKNODE_TASKDB( npScheduledNode ) ) ;
      LocalUnlock( hScheduledNode ) ;
      hCurrentNode = hNextNode ;
   }
   if (NULL != hScheduledNode)
   {
      npScheduledNode =
         (NPTASKNODE) LocalLock( hScheduledNode ) ;
      TASKLIST_SCHEDULEDTASK( npTaskList ) =
         TASKNODE_TASKDB( npScheduledNode ) ;
      LocalUnlock( hScheduledNode ) ;
      if (dwScheduleTime + TASK_TIMESLICE > CLOCKROLLOVER)
         dwScheduleTime -= CLOCKROLLOVER ;
      TASKLIST_SCHEDULEDTIME( npTaskList ) = dwScheduleTime + TASK_TIMESLICE ;
      DPF5( hWDB, "Scheduled task handle: %04x, Schedule Time: %ul\r\n",
            TASKNODE_TASKDB( npScheduledNode ),
            TASKLIST_SCHEDULEDTIME( npTaskList ) ) ;
   }
   LocalUnlock( hTaskList ) ;
   return ( TRUE ) ;
   
} /* end of ScheduleNextTask() */

/************************************************************************
 *  BOOL NEAR IsTaskScheduled( HTASKLIST hTaskList )
 *
 *  Description:
 *     Determines if a task is scheduled.  Returns TRUE if a task
 *     is pending execution.  ( Execution is carried out by
 *     electricution, lethal injection or the gas chamber. :-) )
 *
 *  Comments:
 *      8/19/91  baw  Being a smart ass with comments.
 *
 ************************************************************************/

BOOL NEAR IsTaskScheduled( HTASKLIST hTaskList )
{
   BOOL        fRetVal ;
   NPTASKLIST  npTaskList ;

   if (NULL == (npTaskList = (NPTASKLIST) LocalLock( hTaskList )))
      return ( FALSE ) ;

   if (NULL == TASKLIST_SCHEDULEDTASK( npTaskList ))
      fRetVal = FALSE ;
   else
      fRetVal = ((DWORD) GetSystemTime() >=
                 (DWORD) TASKLIST_SCHEDULEDTIME( npTaskList )) ;

   LocalUnlock( hTaskList ) ;
   DPF5( hWDB, "IsTaskScheduled, returning %s\r\n",
         (fRetVal) ? (LPSTR) "TRUE" : (LPSTR) "FALSE" ) ;
   return ( fRetVal ) ;

} /* end of IsTaskScheduled() */

/************************************************************************
 *  DWORD NEAR GetSystemTime( VOID )
 *
 *  Description: 
 *     Returns the actual system time returned from INT 1A
 *     (BIOS Tick Counter).
 *
 *     This should use INT 21h.  If INT 1A is used, the date/time
 *     can get hosed.
 *
 *  Comments:
 *      8/18/91  baw  Wrote it.
 *      8/19/91  baw  Doesn't DOS suck?
 *
 ************************************************************************/

DWORD NEAR GetSystemTime( VOID )
{
   DWORD  dwRetVal ;

   _asm
   {
         xor   ax, ax                 ; function for Get CMOS Tick Count
         int   1ah
         mov   word ptr dwRetVal, dx
         mov   word ptr dwRetVal + 2, cx
   }
   return ( dwRetVal ) ;

} /* end of GetSystemTime() */

/************************************************************************
 *  BOOL FAR PASCAL SetTaskTimer( HWND hWnd )
 *
 *  Description: 
 *     Creates a system timer and sets the NotifyWnd handle.
 *
 *  Comments:
 *      8/24/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL SetTaskTimer( HWND hWnd )
{
   int  i ;

   DP5( hWDB, "SetTaskTimer()" ) ;

   if (!nIdTimer)
   {
      DP3( hWDB, "Creating System Timer" ) ;
      nIdTimer = CreateSystemTimer( 10, (FARPROC) TimerHandler ) ;
      if (!nIdTimer)
         return ( FALSE ) ;
   }

   // see if an event timer was already created for this window
   // if so, simply update the post flag

   for (i = 0; i < MAXLEN_TASKEVENTS; i++)
   {
      if (TaskEvents[i].hNotifyWnd == hWnd)
      {
         TaskEvents[i].fPostFlag = FALSE ;
         return ( TRUE ) ;
      }
   }

   // new event timer

   for (i = 0; i < MAXLEN_TASKEVENTS; i++)
   {
      if (TaskEvents[i].hNotifyWnd == NULL)
      {
         TaskEvents[i].hNotifyWnd = hWnd ;
         TaskEvents[i].fPostFlag = FALSE ;
         return ( TRUE ) ;
      }
   }

   // no openings

   return ( FALSE ) ;

} /* end of SetTaskTimer() */

/************************************************************************
 *  BOOL FAR PASCAL KillTaskTimer( HWND hWnd )
 *
 *  Description:
 *     Kills the task timer.
 *
 *  Comments:
 *      8/25/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL FAR PASCAL KillTaskTimer( HWND hWnd )
{
   int  i, j ;

   // find entry in array and erase it

   for (i = 0; i < MAXLEN_TASKEVENTS; i++)
   {
      if (TaskEvents[i].hNotifyWnd == hWnd)
      {
         TaskEvents[i].hNotifyWnd = NULL ;

         // check entire list, kill system timer if no entries

         for (j = 0; j < MAXLEN_TASKEVENTS; j++)
         {
            if (TaskEvents[j].hNotifyWnd != NULL)
               return ( TRUE ) ;
         }
         KillSystemTimer( nIdTimer ) ;
         nIdTimer = 0 ;
         return ( TRUE ) ;
      }
   }
   return ( FALSE ) ;

} /* end of KillTaskTimer() */

/************************************************************************
 *  BOOL NEAR ClearTaskPostFlag( HWND hWnd )
 *
 *  Description:
 *     Clears the task timer post flag.
 *
 *  Comments:
 *      8/24/91  baw  Wrote it.
 *
 ************************************************************************/

BOOL NEAR ClearTaskPostFlag( HWND hWnd )
{
   int  i ;

   for (i = 0; i < MAXLEN_TASKEVENTS; i++)
   {
      if (TaskEvents[i].hNotifyWnd == hWnd)
      {
         TaskEvents[i].fPostFlag = FALSE ;
         DPF5( hWDB, "fPostFlag for hWnd: %04x cleared.\r\n", hWnd ) ;
         return ( TRUE ) ;
      }
   }
   return ( FALSE ) ;

} /* end of ClearTaskPostFlag() */

//***************************************************************************
//  End of File: taskman.c
//***************************************************************************

