/*
   ws_files.c - File I/O routines for WinPrint

   (c) Copyright 1990, Charles E. Kindel, Jr.

*/

/* INCLUDE FILES */
#include "PRECOMP.H"

#include <dos.h>

#include "isz.h"
#include "dlgs.h"
#include "winprint.h"
#include "ws_files.h"

BOOL NEAR PASCAL ChangeDir (char *szNewDir) ;


BOOL WINAPI SelectFiles (char *szFileName)
{
   HWND     hSelectList,
            hHiddenList  ;
   HWND     hTempList ;
   PSTR     szBuffer ;
   char     szName [_MAX_FNAME],
            szDir [_MAX_DIR],
            szOldDir [_MAX_DIR],
            szDrive [_MAX_DRIVE],
            szExt [_MAX_EXT] ;
   UINT     nIndex ;
   OFSTRUCT of ;

   hSelectList = GetDlgItem( hwndMain, IDD_SELECTLIST ) ;
   hHiddenList = GetDlgItem( hwndMain, IDD_HIDDENLIST ) ;

   szBuffer = (PSTR)LocalAlloc (LPTR, _MAX_PATH + _MAX_FNAME + _MAX_EXT) ;
   if (strchr (szFileName, '*') || strchr (szFileName, '?'))
   {
      if (!(hTempList = CreateWindow( "listbox", "hidden",
                           WS_CHILD | LBS_STANDARD,
                           0, 0, 10, 10, hwndMain, (HMENU)1, hInst, NULL )))
      {
         DP1( hWDB, "Could not create hidden listbox!" ) ;
         LocalFree ((LOCALHANDLE) szBuffer) ;
         return FALSE ;
      }

      getcwd (szOldDir, _MAX_PATH) ;
      _splitpath(szFileName, szDrive, szDir, szName, szExt) ;
      wsprintf( szBuffer, "%s%s", (LPSTR)szDrive, (LPSTR)szDir) ;
      ChangeDir( szBuffer ) ;

      wsprintf( szBuffer, "%s%s", (LPSTR)szName, (LPSTR)szExt) ;

      DP( hWDB, "szBuffer = %s, szFileName = %s, szDrive = %s, szDir = %s, szName = %s, szExt = %s", 
          (LPSTR)szBuffer, (LPSTR)szFileName, (LPSTR)szDrive, (LPSTR)szDir, (LPSTR)szName, (LPSTR)szExt ) ;

      if (-1 == (short)SendMessage( hTempList, LB_DIR, 0,
                        (LONG)(LPSTR)szFileName ))
      {
         DP1( hWDB, "Could not SendMessage to hTempList in SelectFiles() (%s)",
            (LPSTR)szBuffer ) ;
         DestroyWindow (hTempList) ;
         ChangeDir (szOldDir) ;
         LocalFree ((LOCALHANDLE) szBuffer) ;
         return FALSE ;
      }
      while (-1 != (UINT) SendMessage (hTempList, LB_GETTEXT, 0, (LONG)(LPSTR) szBuffer)) 
      {
         if (-1 != OpenFile (szBuffer, &of, OF_EXIST | OF_READ))
         {
            OemToAnsi (of.szPathName, szBuffer) ;
            if (LB_ERR == (nIndex = (int)SendMessage (hSelectList, LB_GETCURSEL, 0, 0L)))
               nIndex = (UINT)-1 ;
            SendMessage (hHiddenList, LB_INSERTSTRING, nIndex, (LONG) (LPSTR) szBuffer) ;
            ShortenPath (szBuffer, szBuffer, 32) ;
            SendMessage (hSelectList, LB_INSERTSTRING, nIndex, (LONG) (LPSTR) szBuffer) ;
         }
         SendMessage (hTempList, LB_DELETESTRING, 0, 0L) ;
      }
      ChangeDir (szOldDir) ;
      DestroyWindow (hTempList) ;
   }
   else
   {
      if (-1 != OpenFile (szFileName, &of, OF_EXIST | OF_READ))
      {
            _splitpath(of.szPathName, szDrive, szDir, szName, szExt) ;
            OemToAnsi (of.szPathName, szBuffer) ;
            if (LB_ERR == (nIndex = (int) SendMessage (hSelectList, LB_GETCURSEL, 0, 0L)))
               nIndex = (UINT)-1;
            SendMessage (hHiddenList, LB_INSERTSTRING, nIndex, (LONG) (LPSTR) szBuffer) ;
            ShortenPath (szBuffer, szBuffer, 32) ;
            SendMessage (hSelectList, LB_INSERTSTRING, nIndex, (LONG) (LPSTR) szBuffer) ;
      }
      else
      {
         DP1( hWDB, "Could not OpenFile( %s ) in SelectFiles()", (LPSTR)szFileName ) ;
         LocalFree ((LOCALHANDLE) szBuffer) ;
         return FALSE ;
      }
   }
   EnableWindow (GetDlgItem (hwndMain, IDD_REMOVE),
                (BOOL) SendMessage (hSelectList, LB_GETCOUNT, 0, 0L)) ;
/*   SendMessage (hSelectList, LB_SETCURSEL, 0, 0L) ; */
   LocalFree ((LOCALHANDLE) szBuffer) ;
   return TRUE ;
}


BOOL NEAR PASCAL ChangeDir (char *szNewDir)
{
   #ifdef WIN32
   return (BOOL)SetCurrentDirectory( szNewDir ) ;
   #else
   char *   LastWhack, FirstWhack ;
unsigned maxdrives ;

    char szDir[260] ;
    lstrcpy( szDir, szNewDir ) ;
    if ( LastWhack = strrchr( szDir, '\\' ) )
    {
        if (FirstWhack = strchr( szDir, '\\' ))
        {
            if (FirstWhack != LastWhack && *(LastWhack+1) == '\0')
                *LastWhack = '\0' ;
        }
    }

    DP1( hWDB, "ChangeDir(\"%s\")", (LPSTR)szNewDir ) ;
   if (szDir[0] != '\0')
      _dos_setdrive (toupper(szDir[0]) - 'A' + 1, &maxdrives) ;
   if (-1 == chdir (szDir))
   {
      DP1( hWDB, "chdir (%s) failed", (LPSTR)szDir ) ;
      return FALSE ;
   }

   #endif

   return TRUE ;
}


void WINAPI ShortenPath(LPSTR lpszNew, LPSTR lpszOld, int nLen)
{
   char  szName [_MAX_FNAME],
         szDir [_MAX_DIR],
         szDrive [_MAX_DRIVE],
         szExt [_MAX_EXT] ;
   char  szTmp[_MAX_PATH] ;
   
   lstrcpy( szTmp, lpszOld ) ;

   if (lstrlen (szTmp) > nLen)
   {
      _splitpath( szTmp, szDrive, szDir, szName, szExt ) ;

      if (nLen > 8 + lstrlen (szName) + lstrlen (szExt))
      {
         while (lstrlen (szDrive) + lstrlen(szDir) + 15 > nLen)
         {
            if (!strchr(szDir, '\\'))
               break ;
            lstrcpy (szDir, szDir+1) ;
            lstrcpy (szDir, strchr( szDir, '\\') ) ;
         }
         wsprintf( lpszNew, "%s\\...%s%s%s",
                   (LPSTR)szDrive,
                   (LPSTR)szDir,
                   (LPSTR)szName,
                   (LPSTR)szExt) ; 
      }
      else
         wsprintf( (LPSTR)lpszNew, (LPSTR)"...\\%s%s",
                     (LPSTR)szName, (LPSTR)szExt) ;
   }
   else
      lstrcpy (lpszNew, szTmp) ;
}

