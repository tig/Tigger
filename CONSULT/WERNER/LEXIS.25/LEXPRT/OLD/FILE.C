/************************************************************************
 *
 *     Project:  WinPrint 2.0
 *
 *      Module:  file.c
 *
 *     Remarks:
 *
 *       This module handles the File I/O for WinPrint 2.0.  
 *
 *   Revisions:  
 *     00.00.000  3/11/91 cek   First Version
 *
 *
 ***********************************************************************/
#include "..\inc\WINPRINT.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

/************************************************************************
 * Local defines
 ***********************************************************************/
typedef struct tagFILEIO
{
   //
   // This structure contains the definition of a file.  The
   // filename, path, length, current pointer, file handle, etc...
   //
   // Someday it might be nice to have a set of "offsets" stored
   // here that could be used to "quickly" get to a specific
   // point in the file (i.e. forward/back n pages)
   // 
   FILE  *pFile ;

   char  szName[_MAX_PATH] ;

   LONG  lLength ;            // length of file in bytes

   LONG  lCurPos ;            // current offset into the file

   LONG  lPrevPos ;           // previous offset into the file

} FILEIO, *PFILEIO, FAR *LPFILEIO ;

#define HFILEIO GLOBALHANDLE

#define ALLOCFILEIO()      (GlobalAlloc( GHND, sizeof( FILEIO )))
#define LOCKFILEIO(gh)     ((LPFILEIO)GlobalLock( gh ))
#define UNLOCKFILEIO(gh)   (GlobalUnlock( gh ))
#define FREEFILEIO(gh)     (GlobalFree( gh ))


/************************************************************************
 * Local Function Prototypes
 ***********************************************************************/

/************************************************************************
 * External Functions
 ***********************************************************************/


/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * End of File: file.c
 ***********************************************************************/

