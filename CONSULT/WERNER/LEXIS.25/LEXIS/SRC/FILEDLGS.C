/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  filedlgs.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module handles the File Dialog boxes.  It pops up the
 *    dialog box (three entry points, one for each file type). And
 *    Launches Notepad if necissary.  Most functions return a
 *    fully qualified pathname.  t
 *
 *    The "current" directory is kept here so the user will always
 *    come up with the most recent dir for each file type.
 *
 *   Revisions:  
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <stdlib.h>
#include <direct.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\filedlgs.h"
#include "..\inc\global.h"

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

char szDiary[_MAX_PATH] ;
char szNotepad[_MAX_PATH] ;
char szRecord[_MAX_PATH] ;
char szDocument[_MAX_PATH] ;

/************************************************************************
 * External functions
 ************************************************************************/

/*************************************************************************
 *  WORD FAR PASCAL OpenFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType )
 *
 *  Description:
 *
 *    This function pops up a file open dialog box, using the directory
 *    and file type specified in the wType arg.
 *
 *  Type/Parameter
 *          Description
 *
 *    wType WORD
 *          OPEN_DIARY, OPEN_NOTEPAD, OPEN_RECORD
 * 
 *  Return Value
 *    Returns a value indicating the user's choice:
 *
 *    DLG_ERROR - some error.
 *    DLG_OK    - user said OK and the file exists.
 *    DLG_CANCEL - user changed his mind
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL OpenFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType )
{
   int n ;
   char  szTitle[MAX_STRLEN] ;
   char  szDefSpec[_MAX_EXT + _MAX_FNAME] ;
   char  szDirectory[_MAX_PATH] ;

   if (!LoadString( GETHINST( hwndParent ), IDS_FILESPEC + wType,
                    szDefSpec, _MAX_EXT + _MAX_FNAME ))
      return DLG_ERROR ;

   switch (wType)
   {
      case OPEN_DIARY:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szDiary, (LPSTR)szDefSpec ) ;
      break ;

      case OPEN_NOTEPAD:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szNotepad, (LPSTR)szDefSpec ) ;
      break ;

      case OPEN_RECORD:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szRecord, (LPSTR)szDefSpec ) ;
      break ;
   }

   if (!LoadString( GETHINST( hwndParent ), IDS_OPENTITLE + wType,
                    szTitle, MAX_STRLEN ))
      return DLG_ERROR ;

   n = FileOpenDialog( hwndParent, szTitle, szDirectory,
                        DLGOPEN_MUSTEXIST | DLGOPEN_OPEN |
                        DLGOPEN_NOCHANGESPEC,
                        lpszFile, _MAX_PATH ) ;


   if (n == DLG_OK)
      switch (wType)
      {
         case OPEN_DIARY:
            getcwd( szDiary, _MAX_PATH ) ;
         break ;

         case OPEN_NOTEPAD:
            getcwd( szNotepad, _MAX_PATH ) ;
         break ;

         case OPEN_RECORD:
            getcwd( szRecord, _MAX_PATH ) ;
         break ;
      }

   return n ;
}/* OpenFileDialog() */

/*************************************************************************
 *  WORD FAR PASCAL NewFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType )
 *
 *  Description:
 *
 *    This function pops up a file new dialog box, using the directory
 *    and file type specified in the wType arg.
 *
 *  Type/Parameter
 *          Description
 *
 *    wType WORD
 *          NEW_DIARY, NEW_NOTEPAD, NEW_RECORD
 * 
 *  Return Value
 *    Returns a value indicating the user's choice:
 *
 *    DLG_ERROR - some error.
 *    DLG_OK    - user said OK and the file exists.
 *    DLG_CANCEL - user changed his mind
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL NewFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType )
{
   int n ;
   char  szTitle[MAX_STRLEN] ;
   char  szDefSpec[_MAX_EXT + _MAX_FNAME] ;
   char  szDirectory[_MAX_PATH] ;

   if (!LoadString( GETHINST( hwndParent ), IDS_FILESPEC + wType,
                    szDefSpec, _MAX_EXT + _MAX_FNAME ))
      return DLG_ERROR ;

   switch (wType)
   {
      case NEW_DIARY:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szDiary, (LPSTR)szDefSpec ) ;
      break ;

      case NEW_NOTEPAD:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szNotepad, (LPSTR)szDefSpec ) ;
      break ;

      case NEW_RECORD:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szRecord, (LPSTR)szDefSpec ) ;
      break ;

   }
   if (!LoadString( GETHINST( hwndParent ), IDS_NEWTITLE + wType,
                    szTitle, MAX_STRLEN ))
      return DLG_ERROR ;

   n = FileOpenDialog( hwndParent, szTitle, szDirectory,
                        DLGOPEN_OPEN |
                        DLGOPEN_NOCHANGESPEC,
                        lpszFile, _MAX_PATH ) ;


   if (n == DLG_OK)
      switch (wType)
      {
         case NEW_DIARY:
            getcwd( szDiary, _MAX_PATH ) ;
         break ;

         case NEW_NOTEPAD:
            getcwd( szNotepad, _MAX_PATH ) ;
         break ;

         case NEW_RECORD:
            getcwd( szRecord, _MAX_PATH ) ;
         break ;
      }

   return n ;
}/* NewFileDialog() */

/*************************************************************************
 *  WORD FAR PASCAL
 *    PrintFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType )
 *
 *  Description:
 *
 *    This function pops up a print file dialog box, using the directory
 *    and file type specified in the wType arg.
 *
 *  Type/Parameter
 *          Description
 *
 *    wType WORD
 *          PRINT_DIARY, PRINT_NOTEPAD, PRINT_RECORD
 * 
 *  Return Value
 *    Returns a value indicating the user's choice:
 *
 *    DLG_ERROR - some error.
 *    DLG_OK    - user said OK and the file exists.
 *    DLG_CANCEL - user changed his mind
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL PrintFileDialog( HWND hwndParent, LPSTR lpszFile, WORD wType )
{
   int n ;
   char  szTitle[MAX_STRLEN] ;
   char  szDefSpec[_MAX_EXT + _MAX_FNAME] ;
   char  szDirectory[_MAX_PATH] ;

   if (!LoadString( GETHINST( hwndParent ), IDS_FILESPEC + wType,
                    szDefSpec, _MAX_EXT + _MAX_FNAME ))
      return DLG_ERROR ;

   switch (wType)
   {
      case PRINT_DIARY:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szDiary, (LPSTR)szDefSpec ) ;
      break ;

      case PRINT_NOTEPAD:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szNotepad, (LPSTR)szDefSpec ) ;
      break ;

      case PRINT_RECORD:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szRecord, (LPSTR)szDefSpec ) ;
      break ;

      case PRINT_DOCUMENT:
         wsprintf( szDirectory, "%s\\%s", (LPSTR)szDocument, (LPSTR)szDefSpec ) ;
      break ;
   }

   if (!LoadString( GETHINST( hwndParent ), IDS_PRINTTITLE + wType,
                    szTitle, MAX_STRLEN ))
      return DLG_ERROR ;

   n = FileOpenDialog( hwndParent, szTitle, szDirectory,
                        DLGOPEN_OPEN | DLGOPEN_MUSTEXIST |
                        DLGOPEN_NOCHANGESPEC,
                        lpszFile, _MAX_PATH ) ;


   if (n == DLG_OK)
      switch (wType)
      {
         case PRINT_DIARY:
            getcwd( szDiary, _MAX_PATH ) ;
         break ;

         case PRINT_NOTEPAD:
            getcwd( szNotepad, _MAX_PATH ) ;
         break ;

         case PRINT_RECORD:
            getcwd( szRecord, _MAX_PATH ) ;
         break ;

         case PRINT_DOCUMENT:
            getcwd( szDocument, _MAX_PATH ) ;
         break ;
      }

   return n ;
}/* PrintFileDialog() */

/*************************************************************************
 *  void FAR PASCAL
 *  InitDirectories( LPSTR lpszDiary, LPSTR lpszNotepad,
 *                   LPSTR lpszRecord, LPSTR lpszDoc )
 *
 *  Description:
 *
 *   This function sets the default dirs.
 *
 *
 *  Comments:
 *
 *************************************************************************/
void FAR PASCAL
InitDirectories( LPSTR lpszDiary, LPSTR lpszNotepad,
              LPSTR lpszRecord, LPSTR lpszDoc )
{
   lstrcpy( szDiary, lpszDiary ) ;
   lstrcpy( szNotepad, lpszNotepad ) ;
   lstrcpy( szRecord, lpszRecord ) ;
   lstrcpy( szDocument, lpszDoc ) ;

}/* InitFileDlgs() */
/************************************************************************
 * Internal functions
 ************************************************************************/


/************************************************************************
 * End of File: filedlgs.c
 ************************************************************************/

