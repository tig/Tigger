/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL.DLL, Common DLL for Werner Apps
 *
 *      Module:  file.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:
 *
 *    This module contains library routines for manipulating files and
 *    directories and filenames and directorynames.
 *    You know, things like changing the default dir and the such.
 *
 *   Revisions:  
 *     00.00.000  1/14/91 cek   First version
 *
 *
 ************************************************************************/
#define WINDLL

/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * window class names
 ************************************************************************/

/*************************************************************************
 *  int FAR PASCAL FileSetPath( const LPSTR lpszPath ) ;
 *
 *  Description:
 *    Changes the current working drive and directory.  See 'C' ref.
 *    for docs on chdir and chdrive, which this function uses.
 *
 *  Type/Parameter
 *          Description
 *
 *    LPSTR lpszPath
 *
 *          Directory and path for new working directory.  
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *             0        Current working dir successfully changed.
 *             -1       Specified path not found
 *
 *  Comments:
 *    From lexis 2000 1.7 code.
 *
 *************************************************************************/
int FAR PASCAL FileSetPath( const LPSTR lpszPath ) 
{
   int drive;

   if (lpszPath[0] == 0)
      return 0 ;

   _fstrupr( lpszPath ) ;

   if (lpszPath[1] == ':')
   {
      _getdrive( drive ) ;

      if (drive != lpszPath[0] - '@')
         _chdrive( lpszPath[0] - '@' ) ;

      if (lpszPath[1] == ':' && lpszPath[2] == '\0')
         return chdir( "\\" );
      else
         return chdir( &lpszPath[2] ) ;
    }
   else
      return chdir( lpszPath ) ;

}/* SetFilePath() */


/************************************************************************
 * End of File: file.c
 ************************************************************************/

