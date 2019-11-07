/************************************************************************
 *
 *  Module:  NEWEXE.C
 *
 *  System:  Microsoft C 6.00A, Windows 3.00
 *
 * Remarks:
 *
 *
 * History:
 *
 *   03/19/91  - First version.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
//
// Define WINDLL so that C6 with the -W3 switch will flag SS!=DS issues
// for us.  Since this code can be put into a DLL, we want to make sure
// we have no SS!=DS issues.
//
#define WINDLL

#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <errno.h>

#include "newexe.h"

/************************************************************************
 * Local Defines
 ************************************************************************/

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Local Variables
 *
 * Remember:  If this code goes into a DLL these variables will reside in
 * the DLLs data segment.  Thus 1) The DLL should NOT be a CODE only DLL,
 * and 2) these variables will be shared by all applications using the DLL.
 *
 ************************************************************************/

/************************************************************************
 * Exported Functions
 ************************************************************************/
/****************************************************************
 *  int FAR PASCAL
 *    newexeOpenNewExe( LPSTR lpszFileName, LPOLDEXEHDR lpOldHdr,
 *                      LPNEWEXEHDR lpNewHdr )
 *
 *  Description: 
 *
 *    This function opens the specified NewExe format file and
 *    fills the buffers pointed to by lpOldHdr and lpNewHdr with
 *    the Old and New EXE headers respectively.
 *
 *    If the file cannot be opened or the file is not a valid NEW EXE file
 *    0 is returned.  Otherwise the return value is a valid file handle
 *    to the file.
 *
 *    The C-Runtime global variable "errno" is set to INVALIDEXE if
 *    the the exe was invalid.
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL
newexeOpenNewExe( LPSTR       lpszFName,
                  LPOLDEXEHDR lpOldHdr,
                  LPNEWEXEHDR lpNewHdr )
{

   int            fh ;                      
   OFSTRUCT       of ;

   //
   // Read old EXE header 
   //
   if ((fh = OpenFile( lpszFName, &of, OF_READ | OF_SHARE_DENY_NONE )) == -1)
      return FALSE ;

   if (_lread( fh, (LPSTR)lpOldHdr, sizeof(OLDEXEHDR)) != sizeof(OLDEXEHDR) )
   {   
      _lclose( fh ) ;                     
      return FALSE  ;
   }

   //
   // Go to beginning of new header, read it in and verify that it is
   // valid.
   //
   lseek( fh, lpOldHdr->wNewExeOffset, SEEK_SET ) ;

   if (_lread( fh, (LPSTR)lpNewHdr, sizeof(NEWEXEHDR) ) != sizeof(NEWEXEHDR) ||
             lpNewHdr->wMagic != NEWEXE_NEWMAGIC) 
   { 
       errno = INVALIDEXE ;
       _lclose( fh );                     
       return FALSE ;
   }

   return TRUE ;

} /* newexeOpenNewExe()  */

/************************************************************************
 * End of File: newexe.c
 ************************************************************************/

