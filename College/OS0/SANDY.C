/*                           sandy.c - Sandy's routines  

           (c) Copyright 1990, Charles E. Kindel, Jr. and Sandra Smith
*/

/*--------------------- Start of Declarations ----------------------*/

#include "typedefs.h"

/* measure of clock ticks used by a process */
int ticks_used ;         
/******/

/* measure of clock ticks used by the clock */
int tick ;
/******/

/* status matrices */
int   semaphore [NUM_INT] [MAX_UNIT] ;
int   devicestatus [NUM_INT] [MAX_UNIT];
/******/

/* range of addresses of semaphores */
#define MIN_RANGE  (int) (&semaphore[0][0])
#define MAX_RANGE  (int) (&semaphore[NUM_INT][MAX_UNIT])
/******/

/* value used to convert recorded CPU time */
#define CONVERT_TO_MICRO  20000 
/******/

/* the queues used to store processes */
extern QNODE ready ;
extern QNODE waiting ;
extern QNODE zombie ;
/******/

/* macros used to compensate for effects of waitint in scheduler */
#define TIME_TO_WAIT      emptyqu(&ready) && (pdCurrent != NULL &&  \
                          pdCurrent->status != CURRENT) 
#define TIME_TO_SCHEDULE  pdCurrent == NULL || pdCurrent->status != CURRENT  
/******/

/* the current process executing */
extern PDPTR pdCurrent;
/******/

/* the error flag */
extern int Err ;
/******/

/*--------------------- End of Declarations ------------------------*/

/* - Critical Section Routines -------------------------------------*/

/* - Begincs -------------------------------------------------------*/
/*
 *   int begincs (void)
 *
 *   PURPOSE:  To disable interrupts at the start of a critical
 *             section.
 *
 *   USAGE:    begincs () returns the status of interrupts before
 *             the disbale was called.
 *
 *   NOTES:    if begincs returns 1 then interrupts were active at
 *             the time of the call.
 *
 */
int  begincs (void)
{
     int x = are_ints_enabled () ;
     int_disable () ;
     return x ;
}
/* end of begincs */

/* - Endcs ---------------------------------------------------------*/
/* 
 *   void endcs (int x )
 *
 *   PURPOSE:  To enable interrupts at the end of a critical section.
 *
 *   USAGE:    endcs is called with an integer value, the one that
 *             is returned by begincs, to indicate whether the interrupts
 *             should be enabled.
 *
 *   NOTES:    interrupts are only enabled if x = 1.
 *
 */
void endcs (int x)
{
     if (x)
        int_enable () ;
}
/* end of endcs */

/* - End of Critical Section Routines ------------------------------*/

/* - Check mode Operation ------------------------------------------*/
/*
 *   void check_mode ( void )
 *
 *   PURPOSE:  To check to see which mode of operation the kernel
 *             is running in.
 *
 *   USAGE:    check_mode is called with no parameters and returns 
 *             no values, it calls halt if the kernel is in user mode.	
 *
 *   NOTES:    this procedure is specifically designed for use within
 *             kernel routines to ensure the kernel is in the correct
 *             mode.
 *
 */
void check_mode (void)
{
     if (!in_kernel_mode()) 
     {
        Err = ER_MODE ;
        halt () ;
     }
}
/* end of check_mode */

/* - Semaphore Operations ------------------------------------------*/

/* - P (semaphore) -------------------------------------------------*/
/*  
 *   void P ( int * semaphore )
 *
 *   PURPOSE:  To decrement the semaphore.
 *  
 *   USAGE:    P is called with a pointer to an integer value which
 *             is decremented within the procedure.
 *   
 *   NOTES:    P will insert the current process on the waiting queue
 *             if the result of the operation on semaphore makes its
 *             value less than 0; P then calls scheduler.
 *
 */ 
void P (int *semaphore)
{
   int flag ;

   check_mode () ; 
   flag = begincs () ;
   
   if (--(*semaphore) < 0 && pdCurrent != NULL) 
   {
      pdCurrent->status = WAITING ;
      pdCurrent->key = (int) semaphore ;
      ordinsqu (&waiting, (QNODEPTR) pdCurrent) ;
      scheduler () ;
   }
 
   endcs (flag) ;
}
/* end of P */

/* - V (semaphore) -------------------------------------------------*/
/*
 *   void V ( int *semaphore )
 *
 *   PURPOSE:  To increment the value of semaphore.
 *
 *   USAGE:    V takes as a parameter, a pointer to an integer and
 *             increments its value.
 *  
 *   NOTES:    if the result of the increment on semaphore makes its
 *             value <= 0 then processes are waiting on the semaphore.
 *             A waiting process will be removed from the waiting queue
 *             and inserted on the ready queue.  Scheduler is only called
 *             if there is no current process or the awakened process
 *             has a higher priority then current.
 *
 */
void V (int *semaphore)
{
   PDPTR tmp ;
   int flag ;
   
   check_mode () ;
   flag = begincs() ;

   if (++(*semaphore) <= 0)   
   /* there are processes waiting on the semaphore  */ 
   {
      tmp = (PDPTR) ordremqu(&waiting, (int) semaphore) ;
      if (tmp == NULL)
      {
         /* semaphore indicates a process exists but none found in
            the waiting queue */ 
         Err = ER_INVALID_V ;
         halt () ;
      }

      /* insert process into ready queue */
      tmp->key = tmp->priority ;
      tmp->status = READY ;
      ordinsqu (&ready, (QNODEPTR) tmp) ;

      /* compare priority of tmp with current process, if higher then
         prepare to call scheduler */
      if (( pdCurrent != NULL && pdCurrent->status == CURRENT )
          && (tmp->priority < pdCurrent->priority)) {
         pdCurrent->status = READY ;
         pdCurrent->key = pdCurrent->priority ;
         ordinsqu (&ready, (QNODEPTR) pdCurrent) ; 
      }
      /* scheduler is called only if there is no current process and
         a new one has been inserted on the ready queue, or the new
         process has a higher priority then the current one */
      if (pdCurrent == NULL || pdCurrent->status == READY ) {
         scheduler () ; 
      } 
   }
   endcs (flag) ;
}
/* end S */
/* - End of Semaphore Routines -------------------------------------*/

/* - Interrupt Handler Routines ------------------------------------*/
/*   All interrupt routines assume that the interrupts are disabled */
/*   previous to calling a routine and that the interrupts are      */
/*   re-enabled after execution has completed.                      */


/* - Clock Handler Routine -----------------------------------------*/
/*
 *   void clock_handler ( int type, int unit )
 * 
 *   PURPOSE:  To handle the interrupts generated by the clock.
 *
 *   USAGE:    clock_handler is called through the interrupt vector
 *             table with two parameters specifying the device and
 *             its number.
 * 
 *   NOTES:    this routine takes care of enforcing the time slice;
 *             each time a process exceeds its allotted time it is
 *             inserted on the ready queue and scheduler is called.
 *             A clock semaphore is V'ed after a set number of ticks
 *             have passed.
 * 
 */
void clock_handler (int type, int unit)
{
   ticks_used ++ ;
   tick ++ ; 

   if (tick == CLK_INTERRUPT)
   {
      tick = 0 ;
      V ( (int*) &semaphore[type][unit]) ;                 
   }

   if (ticks_used == TIME_SLICE  && pdCurrent != NULL 
                 && pdCurrent->status == CURRENT ) 
   {
      pdCurrent->status = READY ;
      pdCurrent->key = pdCurrent->priority ;
      ordinsqu (&ready, (QNODEPTR) pdCurrent) ;
      scheduler () ;
   }
}
/* end of clock_handler */

/* - Alarm Handler Routine -----------------------------------------*/
/*
 *   void alarm_handler ( int type, int unit )
 *
 *   PURPOSE:  To generate some output based on an interrupt from
 *             the alarm.
 *  
 *   USAGE:    the alarm must first be set before an alarm interrupt
 *             will even be generated.
 *
 */
void alarm_handler (int type, int unit) 
{
   Err = 13 ;
   halt () ;
}
/* end of alarm_handler */

/* - Disk Handler Routine ------------------------------------------*/
/*
 *   void disk_handler ( int type, int unit )
 * 
 *   PURPOSE:  To handle interrupts generated by the disk.
 *
 *   USAGE:    disk_handler checks to see if the device is ready
 *             and if this is the case, V's the disk semaphore.
 *
 */
void disk_handler (int type, int unit)
{
   devicestatus[type][unit] = lobyte(inp (type)) ;
   V ( (int*) &(semaphore[type][unit])) ;
}
/* end of disk_handler */

/* - Terminal Handler Routine ---------------------------------------*/
/*
 *   void terminal_handler ( int type, int unit )
 *
 *   PURPOSE:  To handle the interrupts generated by the terminals.
 *
 *   USAGE:    terminal_handler determines if the interrupt signals
 *             that a terminal has a character available; if this is 
 *             the case the character is added to the buffer for
 *             that terminal.  When the buffer is full or end of 
 *             line character is received the semaphore for that 
 *             terminal is V'ed.
 *
 */
void terminal_handler (int type, int unit)
{
   int word ;

   word = inp(type) ;
   devicestatus[type][unit] = word ;
   V ( (int*) &(semaphore[type][unit]) ) ; 

}
/* end of terminal_handler */

/* - End of Interrupt Handler Routines ------------------------------*/


/* - Wait Clock Routine ---------------------------------------------*/
/*
 *   void waitclock ( void )
 *
 *   PURPOSE:  To cause the clock semaphore to be P'ed.
 *
 *   USAGE:    wait_clock requires no parameters and returns no values.
 *
 *   NOTES:    if a clock tick has already occurred before calling this
 *             procedure it will not block the process calling the wait.
 *
 */
void waitclock ( )
{
   check_mode () ;
   P ( (int*) &semaphore[CLOCK_DEV][0]) ;
}
/* end of wait_clock */

/* - Read Time Routine -----------------------------------------------*/
/*
 *   int readtime ( void )
 *
 *   PURPOSE:  To return the current process' CPU time in microseconds.
 *
 *   USAGE:    readtime returns the CPU time of the current process.
 *
 */
int readtime ( )
{
   check_mode () ;
   return (pdCurrent->CPU_time * CONVERT_TO_MICRO ) ;
}
/* end of readtime */

/* - Wait Device Routine ---------------------------------------------*/
/*
 *   int waitdevice ( int type, int unit )
 *
 *   PURPOSE:  To cause a process to wait on a device.  
 *
 *   USAGE:    waitdevice is called with the type and unit number of
 *             the device.  The semaphore associated with the device is
 *             P'ed and then the most current status for the device is
 *             returned.
 *
 */
int waitdevice (int type, int unit)
{
   check_mode () ;

   P (&semaphore[type][unit]) ;
   return (devicestatus[type][unit]) ;
}
/* end of waitdevice */

/* - Scheduler and deadlock Subprograms ------------------------------*/

/* - Scheduler Routine -----------------------------------------------*/
/*
 *   void scheduler ( void )
 *
 *   PURPOSE:  To schedule a new process for execution.
 *
 *   USAGE:    scheduler is called with no parameters and returns no 
 *             values.  
 *
 *   NOTES:    scheduler will check for deadlock first before proceeding.
 *             If there are processes on the waiting queue and none on
 *             the ready queue scheduler will loop until something comes
 *             available.
 *
 */
void scheduler ( )
{
   int p_level = 1;             /* priority indicator */
   PDPTR old = pdCurrent ;      /* old process is now captured in old */
 
   check_mode () ;

   /* reset the indicator for CPU time used by a process */  
   if ( old != NULL  && old->status == READY ) 
      old->CPU_time += ticks_used ;
   ticks_used = 0 ;

   deadlock () ;
   /* if no processes are ready to execute, suspend execution 
      until something becomes available */
   while ( TIME_TO_WAIT )
   {
       int_enable () ;
       waitint () ;
       int_disable () ;
   }
   if ( TIME_TO_SCHEDULE ) {
   /* retrieve new process from ready queue and set its fields to
      indicate it is the new current process */
      for (  p_level = 1 ; p_level <= MIN_PRIORITY &&
            ( pdCurrent = (PDPTR) ordremqu (&ready, p_level) ) == NULL ;
            p_level ++ ) ; 
      pdCurrent->status = CURRENT;
      /* if no previous current process existed call resume otherwise call
         context_switch */

      if ( old == NULL )  
         resume ( &pdCurrent->state ) ;
      else 
         context_switch ( &old->state, &pdCurrent->state ) ;
   }
}
/* end of scheduler */

/* - Deadlock Routine ----------------------------------------------------*/
/*
 *   void deadlock ( void )
 *
 *   PURPOSE:  To see if deadlock exists in the waiting and ready
 *             queues.
 *  
 *   USAGE:    deadlock is not called with any parameters nor does it
 *             return any values.
 *
 *   NOTES:    deadlock will set the error flag and call halt if it
 *             determines the both ready and waiting queues are empty
 *             or if the ready queue is empty and the waiting queue is
 *             full of processes not blocked on I/O devices.
 *
 */ 
void deadlock (void)
{
   QNODEPTR tmp ;
   
   if ( emptyqu(&ready) ) 
      if ( emptyqu(&waiting) ) {
      /* termination of all processes */   
         Err = ER_NORMAL ;
         halt () ;
      }
      else
      /* check to see if any processes on waiting queue are 
         waiting on I/O */
      {
         for (tmp = waiting.next ; tmp != &waiting &&
              ((tmp->key > MAX_RANGE) || (tmp->key < MIN_RANGE)) ; 
              tmp = tmp->next ) ;
         if (tmp == &waiting)
         {
             Err = ER_DEADLOCK ;
             halt () ;
          }
       }
   /* if we get to this point then no deadlock exists in the kernel */
}
/* end of deadlock */
         
/* - End of File ---------------------------------------------------------*/
