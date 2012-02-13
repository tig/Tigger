/*               term.c - User level terminal handler
                          for phase2 of OS/0

                  Copyright (c) 1990, C.Kindel S.Smith

*/

#include <string.h>
#include "mbox.h"
#include "typedefs2.h"

MBPTR  mbTermInput[MAX_UNIT] ;
extern int TerminalNumber ;

char WTbuffer[MAX_UNIT][MAX_BUFFER+1] ;
char *WTcur[MAX_UNIT] ;

/* INPUT TERMINAL STUFF */

/* void TerminalInputHandler (void)
 *
 *  PURPOSE:  UserStartup spawns this function for each terminal.
 *            Its purpose is to handle input buffering from the terminal
 *            devices.  It uses the mailbox facilities to accomplish this.
 *
 *  USAGE:    The global variable TerminalNumber must be set to the terminal
 *            number that is to be handled before spawning this process.
 *
 *
*/
void TerminalInputHandler (void)
{
   int i ;
   int   unit ;
   char  *Buffer ;

   unit = TerminalNumber ;

   Buffer = (char *) malloc (MAX_BUFFER+1) ;
   /* MAX_BUFFER+1 to account for a /0 we add */

   mbTermInput[unit] = mbCreate (MAX_BUFFER, MAX_BUFFER+1) ;

   while (TRUE)
   {
      for (i = 0 ; i <= MAX_BUFFER - 1 ; i++)  
      {  
          Buffer[i+1] = '\0' ;
          Buffer[i] = hibyte (waitdevice (TERM_DEV, unit)) ;
          if (Buffer[i] == '\n') 
	    { i++ ;
	      break ;
	    }
	  if (Buffer[i] == '\b')
	    {
	      if (--i >= 0)
		i-- ;
	    }
      }
      i-- ;

      if (i == MAX_BUFFER - 1  && Buffer[i] != '\n')
	  while (hibyte (waitdevice (TERM_DEV, unit)) != '\n')
	    ;
      Buffer[i+1] = '\0' ;  /* put a NULL char at end to make it a sz */
      mbSend (mbTermInput[unit], Buffer) ;
   }
}


/*  int ReadTerm (char* Buffer,  int num, int unit)
 *
 *  PURPOSE:  This is the function called by the system call handler routine
 *            in syscall.c.
 *
 *  USAGE:    char *Buffer - pointer to a previously allocated string of
 *                           size MAX_BUFFER + 1.
 *            int num      - number of characters to read.
 *            int unit     - unit number of the terminal to be read .
 *
 *  RETURNS:  number of characters read.
*/
int ReadTerm (char* Buffer, int num, int unit)
{
  static int count = 0 ;
  char * tmp ;
  int i ;

  if (!count)
  {
     mbReceive (mbTermInput[unit], WTbuffer[unit]) ;
     WTcur[unit] = WTbuffer [unit] ;
  }

  for (i = 0 ;  i <= num - 1 && *WTcur [unit] != '\0' ; i++, WTcur[unit]++)
  {
     Buffer[i] = *WTcur[unit] ;
     *WTcur[unit] = '\0' ;
     count++ ;
  }

  if (count == MAX_BUFFER - 1 || *WTcur[unit] == '\0')
    count = 0 ;

  return i ;
}


/* TERMINAL OUTPUT ROUTINES */

/*  int WriteTerm (char* Buffer, int num, int unit)
 *
 *  PURPOSE:  This is the function called by the system call handler routine
 *            in syscall.c.
 *
 *  USAGE:    char *Buffer - pointer to a previously allocated string of
 *                           size MAX_BUFFER + 1.
 *            int num      - number of characters to write .
 *            int unit     - unit number of the terminal to be read .
 *
 *  RETURNS:  the number of characters written ;
 *
*/
int WriteTerm (char* Buffer, int num, int unit)
{
   int i ;

   for (i = 1 ; i <= num ; i++)
      outp (TERM_DEV, ((short) (unit << 8) + (short) (Buffer[i-1]))) ;

   return i - 1 ;
}




















              
