/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  dlldesc.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Retrieves description from DLLs and stores into
 *               given combobox
 *
 *   Revisions:
 *     01.00.001  2/19/91 baw   Wrote it.
 *     01.00.002  6/27/91 baw   Modified to work with combo boxes
 *                              and gave functions a more "Windows"
 *                              API type function.
 *
 ************************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "dlldesc.h"

#include <dos.h>
#include <stdlib.h>
#include <errno.h>

/************************************************************************
 *  int DlgModuleListCB( HWND hDlg, int nIDComboBox, LPSTR lpType )
 *
 *  Description:
 *     Fills the given combo box with module description of type
 *     <lpType>.
 *
 *  Comments:
 *      6/27/91  baw  Wrote this comment block.
 *
 ************************************************************************/

int DlgModuleListCB( HWND hDlg, int nIDComboBox, LPSTR lpType )
{
   char           szFullPath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT],
                  szFullSearchPath[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT],
                  szModuleName[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT],
                  szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME],
                  szExt[_MAX_EXT] ;
   int            nItems ;
   struct find_t  ftFileInfo ;
   HWND           hCtrlWnd ;

   // get combo box handle and clear the entries

   hCtrlWnd = GetDlgItem( hDlg, nIDComboBox ) ;
   SendMessage( hCtrlWnd, CB_RESETCONTENT, NULL, NULL ) ;

   // get path of main executable

   GetModuleFileName( GETHINST( hDlg ), szFullPath, sizeof( szFullPath ) ) ;
   _splitpath( szFullPath, szDrive, szDir, szFname, szExt ) ;
   wsprintf( szFullPath, "%c:%s", *szDrive, (LPSTR) szDir ) ;
   wsprintf( szFullSearchPath, "%s*%s", (LPSTR) szFullPath, lpType ) ;

   // search path

   nItems = 0 ;
   if (0 == _dos_findfirst( szFullSearchPath, _A_NORMAL, &ftFileInfo ))
   {
      wsprintf( szModuleName, "%s%s", (LPSTR) szFullPath,
                (LPSTR) ftFileInfo.name ) ;
      AddModuleDescription( hCtrlWnd, szModuleName ) ;
      nItems++ ;
      while (0 == _dos_findnext( &ftFileInfo ))
      {
         wsprintf( szModuleName, "%s%s", (LPSTR) szFullPath,
                   (LPSTR) ftFileInfo.name ) ;
         AddModuleDescription( hCtrlWnd, szModuleName ) ;
         nItems++ ;
      }

      // set default as first module

      SendMessage( hCtrlWnd, CB_SETCURSEL, 0, NULL ) ;

      return ( nItems ) ;
   }
   else
      return ( -1 ) ;

} /* end of DlgListModuleCB() */

/************************************************************************
 *  BOOL AddModuleDescription( HWND hCB, LPSTR lpFileName )
 *
 *  Description:
 *     Adds the module description of the given module to a
 *     combo box <hCB>.
 *
 *  Comments:
 *      6/25/91  baw  Wrote this comment block.
 *
 ************************************************************************/

BOOL AddModuleDescription( HWND hCB, LPSTR lpFileName )
{
   int       hFile ;
   char      szDesc[ MAXLEN_MODULEDESC ] ;
   ATOM      atomModuleName ;
   BYTE      bStrLen ;
   OFSTRUCT  ofFile ;
   WORD      wItemPos, wTemp ;
   LPSTR     lpFound ;

   if (NULL == (hFile = OpenFile( lpFileName, &ofFile, OF_READ )))
      return ( FALSE ) ;
   _llseek( hFile, (LONG) HDR_FTYPE, SEEK_SET ) ;
   _lread( hFile, (LPSTR) &wTemp, 2 ) ;
   if (wTemp != HDR_TYPEWINDOWS)
      return ( FALSE ) ;
   _llseek( hFile, (LONG) HDR_NEWOFFSET, SEEK_SET ) ;
   _lread( hFile, (LPSTR) &wTemp, 2 ) ;
   _llseek( hFile, (LONG) wTemp, SEEK_SET ) ;
   _llseek( hFile, (LONG) HDR_NONRESIDENT, SEEK_CUR ) ;
   _lread( hFile, (LPSTR) &wTemp, 2 ) ;
   _llseek( hFile, (LONG) wTemp, SEEK_SET ) ;
   _lread( hFile, (LPSTR) &bStrLen, 1 ) ;
   _lread( hFile, szDesc, (WORD) bStrLen ) ;
   szDesc[bStrLen] = NULL ;
   _lclose( hFile ) ;

   // description string should be in format of "name|description"
   // if the bar is found, add the item, create atom for module name
   // and set the item data to the atom value

   if (NULL != (lpFound = _fstrchr( szDesc, '|' )))
   {
      *(lpFound++) = NULL ;
      wItemPos = LOWORD( SendMessage( hCB, CB_ADDSTRING,
                                      NULL, (LONG) (LPSTR) lpFound ) ) ;
      atomModuleName = AddAtom( szDesc ) ;
      SendMessage( hCB, CB_SETITEMDATA, wItemPos, atomModuleName ) ;
   }

   return ( TRUE ) ;

} /* end of AddModuleDescription() */

/************************************************************************
 * End of File: dlldesc.c
 ************************************************************************/
