/*
 *  File:  libuser.c
 *
 *  Authors:  Alan Condit and Renduo Zhang
 *
 *  Description:  This file contains the interface declarations
 *                to the OS kernel support package.
 *
 */

typedef struct sysargs {
  int number;
  int arg1, arg2, arg3, arg4;
} sysargs;

typedef int (*PFI)();

int Readterm( char *buff, int bsize, int unit_id );        /* syscall 1 */
int Writeterm( char *buff, int bsize, int unit_id );       /* syscall 2 */
int Spawn( PFI func, int priority, int stack_size );       /* syscall 3 */
int Createmb( int numslots, int slotsize );                /* syscall 4 */
void Send( int mbname, char *msg );                         /* syscall 5 */
void Receive( int mbname, char *msg );                      /* syscall 6 */
int Condrec( int mbname, char *msg );                      /* syscall 7 */
int Delay( int seconds );                                  /* syscall 8 */
int DiskPut( char *dbuff, int track, int sector );         /* syscall 9 */
int DiskGet( char *dbuff, int track, int sector );         /* syscall 10 */
int Get_Time_of_Day( void );                               /* syscall 11 */
void Terminate( void );                                     /* syscall 12 */

/*
*  Routine:  Readterm
*
*  Description: This is the call entry point for terminal input.
*
*  Arguments:    char* buff    -- pointer to the input buffer
*                int   bsize   -- size of the input buffer
*                int   unit_id -- terminal unit number
*
*  Return Value: number of characters in buffer
*
*/
int Readterm( char *buff, int bsize, int unit_id )        /* syscall 1 */
{
    sysargs sa;

    sa.number = 1;
    sa.arg1 = ( int ) buff;
    sa.arg2 = bsize;
    sa.arg3 = unit_id;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg2 );
} /* end of Readterm */

/*
*  Routine:  Writeterm
*
*  Description: This is the call entry point for terminal output.
*
*  Arguments:    char* buff    -- pointer to the output buffer
*                int   bsize   -- number of characters to write
*                int   unit_id -- terminal unit number
*
*  Side Effects: none
*
*/
int Writeterm( char *buff, int bsize, int unit_id )       /* syscall 2 */
{
    sysargs sa;

    sa.number = 2;
    sa.arg1 = ( int ) buff;
    sa.arg2 = bsize;
    sa.arg3 = unit_id;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg2 );
} /* end of Writeterm */

/*
*  Routine:  Spawn
*
*  Description: This is the call entry to fork a new user process.
*
*  Arguments:    PFI func      -- pointer to the function to fork
*                int stacksize -- amount of stack to be allocated
*                int priority  -- priority of forked process
*
*  Return Value: process id of the forked process
*
*/
int Spawn( PFI func, int stack_size, int priority )       /* syscall 3 */
{
    sysargs sa;

    sa.number = 3;
    sa.arg1 = ( int ) func;
    sa.arg2 = stack_size;
    sa.arg3 = priority;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of Spawn */

/*
*  Routine:  Createmb
*
*  Description: This is the call entry point to create a new mail box.
*
*  Arguments:    int   numslots -- number of mailbox slots
*                int   slotsize -- size of the mailbox buffer
*
*  Return Value: integer name of the mailbox
*
*/
int Createmb( int numslots, int slotsize )                /* syscall 4 */
{
    sysargs sa;

    sa.number = 4;
    sa.arg1 = numslots;
    sa.arg2 = slotsize;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of Createmb */

/*
*  Routine:  Send
*
*  Description: This is the call entry point mailbox send.
*
*  Arguments:    int mbname -- name of the mailbox to send to
*                char* msg  -- message to send
*
*  Return Value: void
*
*/
void Send( int mbname, char *msg )                         /* syscall 5 */
{
    sysargs sa;

    sa.number = 5;
    sa.arg1 = mbname;
    sa.arg2 = ( int ) msg;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return;
} /* end of Send */

/*
*  Routine:  Receive
*
*  Description: This is the call entry point for terminal input.
*
*  Arguments:    int mbname -- name of the mailbox to receive from
*                char* msg  -- location to receive message
*
*  Return Value: void
*
*/
void Receive( int mbname, char *msg )                      /* syscall 6 */
{
    sysargs sa;

    sa.number = 6;
    sa.arg1 = mbname;
    sa.arg2 = ( int ) msg;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return;
} /* end of Receive */

/*
*  Routine:  Condrec
*
*  Description: This is the call entry point for conditional mailbox receive.
*
*  Arguments:    int mbname -- name of the mailbox to receive from
*                char* msg  -- location to receive message
*
*  Return Value: boolean indicating whether message received
*
*/
int Condrec( int mbname, char *msg )                      /* syscall 7 */
{
    sysargs sa;

    sa.number = 7;
    sa.arg1 = mbname;
    sa.arg2 = ( int ) msg;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of Condrec */

/*
*  Routine:  Delay
*
*  Description: This is the call entry point for timed delay.
*
*  Arguments:    int seconds -- number of seconds to delay
*
*  Return Value: void
*
*/
int Delay( int seconds )                                  /* syscall 8 */
{
    sysargs sa;

    sa.number = 8;
    sa.arg1 = seconds;
    sa.arg2 = 0;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of Delay */

/*
*  Routine:  DiskPut
*
*  Description: This is the call entry point for disk output.
*
*  Arguments:    char* dbuff  -- pointer to the output buffer
*                int   track  -- track to write on
*                int   sector -- sector to write
*
*  Return Value: completion status
*
*/
int DiskPut( char *dbuff, int track, int sector )         /* syscall 9 */
{
    sysargs sa;

    sa.number = 9;
    sa.arg1 = ( int ) dbuff;
    sa.arg2 = track;
    sa.arg3 = sector;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of DiskPut */

/*
*  Routine:  DiskGet
*
*  Description: This is the call entry point for disk input.
*
*  Arguments:    char* dbuff  -- pointer to the input buffer
*                int   track  -- track to read from
*                int   sector -- sector to read
*
*  Return Value: completion status
*
*/
int DiskGet( char *dbuff, int track, int sector )         /* syscall 10 */
{
    sysargs sa;

    sa.number = 10;
    sa.arg1 = ( int ) dbuff;
    sa.arg2 = track;
    sa.arg3 = sector;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of DiskGet */

/*
*  Routine:  Get_Time_of_Day
*
*  Description: This is the call entry point for getting the time of day.
*
*  Arguments:    none
*
*  Return Value: integer time of day
*
*/
int Get_Time_of_Day( void )                               /* syscall 11 */
{
    sysargs sa;

    sa.number = 11;
    sa.arg1 = 0;
    sa.arg2 = 0;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return( sa.arg1 );
} /* end of Get_Time_of_Day */

/*
*  Routine:  Terminate
*
*  Description: This is the call entry point to terminate the current process
*               and its descendants, if any.
*
*  Arguments:    none
*
*  Return Value: none ( it should not return at all )
*
*/
void Terminate( void )                                     /* syscall 12 */
{
    sysargs sa;

    sa.number = 12;
    sa.arg1 = 0;
    sa.arg2 = 0;
    sa.arg3 = 0;
    sa.arg4 = 0;
    syscall( &sa );
    return;
} /* end of Terminate */

/* end of libuser.c */

