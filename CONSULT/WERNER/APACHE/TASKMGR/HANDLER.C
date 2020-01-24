//***************************************************************************
//
//  Module: handler.c
//
//  Purpose:
//     System timer handler used for TASKMGR notifications.
//
//  Description of functions:
//     Contained in the function headers.
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
//  VOID FAR PASCAL TimerHandler( VOID )
//
//  Description:
//     Handles the timer event.  This uses a simple block to
//     prevent a barage of notificaiton messages.
//
//  Parameters:
//     NONE.
//
//  History:   Date       Author      Comment
//              8/24/91   BryanW      Wrote it.
//              1/11/92   BryanW      Added this comment block.
//************************************************************************

VOID FAR PASCAL TimerHandler( VOID )
{
   int  i ;

   for (i = 0; i < MAXLEN_TASKEVENTS; i++)
   {
      if (TaskEvents[i].hNotifyWnd != NULL &&
          !TaskEvents[i].fPostFlag)
      {
         TaskEvents[i].fPostFlag = TRUE ;
         PostMessage( TaskEvents[i].hNotifyWnd, WM_TASKTIMR, NULL, NULL ) ;
      }
   }

} // end of TimerHandler()

//***************************************************************************
//  End of File: handler.c
//***************************************************************************

