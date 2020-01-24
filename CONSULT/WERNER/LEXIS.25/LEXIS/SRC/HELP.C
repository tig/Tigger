/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  help.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module handles the Windows 3.0 help system and help searches
 *    from lexis.  There is essentially one api that is exported from
 *    this module to the rest of the applciation "Help()" which takes
 *    approropiate action in activating and utilizing the Win3 help
 *    system.
 *
 *   Revisions:  
 *     00.00.000  2/23/91 cek   First Version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>

#include "..\inc\lexis.h"
#include "..\inc\lexdlgd.h"
#include "..\inc\global.h"
#include "..\inc\help.h"


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
void NEAR PASCAL MakeHelpPathName( HANDLE hInst, PSTR szFileName ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * EXternal functions
 ************************************************************************/
/*************************************************************************
 *  WORD FAR PASCAL Help( WORD wCommand, DWORD dwData, WORD wFlags )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 *************************************************************************/
WORD FAR PASCAL Help( WORD wCommand, DWORD dwData, WORD wFlags )
{
   LPLEXISSTATE lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals ) ;
   HANDLE hInst ;
   char   szHelpFile[_MAX_PATH+1] ;

   if (!lpLS)
      return FALSE ;

   hInst = GETHINST( lpLS->hwndMainWnd ) ;

   if (wCommand == 0 && dwData == NULL)
   {
      switch (wFlags)
      {
         case IDM_ABOUT:
         {
            char szTitle[MAX_STRLEN] ;
            char szDesc[MAX_STRLEN] ;

            if (LoadString( hInst, IDS_APPDESC, szDesc, MAX_STRLEN ) &&
                LoadString( hInst, IDS_LONGTITLE, szTitle, MAX_STRLEN ))
            {
                  AboutDialog(   lpLS->hwndMainWnd,
                                 szTitle,
                                 lpLS->szAppName,
                                 lpLS->szVersion,
                                 szDesc,
                                 ABOUT_STANDARD ) ;
            }
            else
            {
               DP1( hWDB, "Could not load about box resource strings." ) ;
            }

         }
         break ;
      }

      GlobalUnlock( ghGlobals ) ;
      return TRUE ;
   }

   MakeHelpPathName( hInst, szHelpFile ) ;

   if (wCommand == HELP_HELPONHELP)
      WinHelp( lpLS->hwndMainWnd, "WINHELP.HLP", HELP_HELPONHELP, NULL ) ;
   else
      WinHelp( lpLS->hwndMainWnd, "WINHELP.HLP", wCommand, dwData ) ;

   GlobalUnlock( ghGlobals ) ;
   return TRUE ;
}/* Help() */

void NEAR PASCAL MakeHelpPathName( HANDLE hInst, PSTR szFileName )
{
   PSTR    pszName;
   int     nFileNameLen;

   nFileNameLen = GetModuleFileName( hInst, szFileName, 144 );
   pszName = szFileName + nFileNameLen;

   while (pszName > szFileName)
   {
      if (*pszName == '\\' || *pszName == ':')
      {
         *(++pszName) = '\0';
         break;
      }
      nFileNameLen--;
      pszName--;
   }

   if ((nFileNameLen+13) < 144 )
   {
       lstrcat(szFileName, "helpex.hlp");
   }
   else
   {
       lstrcat(szFileName, "?");
   }

   return;
}




/************************************************************************
 * End of File: help.c
 ************************************************************************/

