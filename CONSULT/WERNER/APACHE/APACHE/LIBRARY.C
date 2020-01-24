/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  library.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  DLL control functions
 *
 *               Handles explicit loading / unloading of DLLs
 *
 *   Revisions:  
 *     01.00.001  2/27/91 baw   Initial version, initial build
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "funcdefs.h"
#include "library.h"

/************************************************************************
 *  HINSTANCE OpenLibrary( LPSTR lpszLibrary, LPSTR szExt )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

HINSTANCE OpenLibrary( LPSTR lpszLibrary, LPSTR lpszExt )
{
   char       szLibraryName[ MAXLEN_LIBNAME ] ;
   HINSTANCE  hLibrary ;

   if (NULL == (hLibrary = GetModuleHandle( lpszLibrary )))
   {
      lstrcpy( szLibraryName, lpszLibrary ) ;
      lstrcat( szLibraryName, lpszExt ) ;
      hLibrary = LoadLibrary( szLibraryName ) ;
      if (hLibrary < (HINSTANCE) 32)
         return ( NULL ) ;
   }
   return ( hLibrary ) ;

} /* end of OpenLibrary() */

//************************************************************************
//  VOID CloseLibrary( HINSTANCE hLibrary, LPFNGETUSECOUNT lpGetUseCount )
//
//  Description:
//     Determines if the usage count for the library has gone to
//     zero, if so the library is freed.
//
//  Parameters:
//     HINSTANCE hLibrary
//        handle to library (module)
//
//     LPFNGETUSECOUNT lpGetUseCount
//        procedure address for library count
//
//  History:   Date       Author      Comment
//              1/12/92   BryanW      Remove warning and added new
//                                    comments.
//
//************************************************************************

VOID CloseLibrary( HINSTANCE hLibrary, LPFNGETUSECOUNT lpGetUseCount )
{
   // check use count and remove if 0

   if (lpGetUseCount != NULL)
   {
      if (0 == (*lpGetUseCount)())
         FreeLibrary( hLibrary ) ;
   }
   else
      FreeLibrary( hLibrary ) ;

} // end of CloseLibrary()

/************************************************************************
 * End of File: library.c
 ************************************************************************/
