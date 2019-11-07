/************************************************************************
 *
 *      Module:  fileopen.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/
#define WINDLL

#include <windows.h>
#include <drivinit.h>
#include <string.h>
#include <stdlib.h>
#include "fileopen.h"


//------------------------------------------------------------------
// Code defines
//------------------------------------------------------------------
#define FOSTRUCT_PROP "FileOpen"
#define MAX_TITLELEN  64 

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------

#ifdef IN_A_DLL
extern HANDLE hInst ;
#endif

//------------------------------------------------------------------
// Module Variables
//------------------------------------------------------------------
typedef struct tagFILEOPENSTRUCT
{
   char szTitle[MAX_TITLELEN] ;
   char szFileName[_MAX_PATH] ;
   WORD wFlags ;
   char szDefSpec[_MAX_FNAME] ;
   WORD wFileLen ;
} FILEOPENSTRUCT, *PFILEOPENSTRUCT ;

BOOL FAR PASCAL fnFileOpen( HWND hDlg,
                                   unsigned iMessage,
                                   WORD wParam,
                                   LONG lParam ) ;


//------------------------------------------------------------------------
//  int FAR PASCAL FileOpenDialog( HWND    hwndParent,
//                                 LPSTR   lpszTitle,
//                                 LPSTR   lpszSpec,
//                                 WORD    wFlags,
//                                 LPSTR   lpszFileBuf,
//                                 WORD    wFileLen ) ;
//
// 
// Type/Parameter
//          Description
// 
// HWND hwndParent
//          Specifies the window to be used as the parent of the dialog box.
//          This must be a valid window handle.
// 
// LPSTR lpszTitle
//          Specifies the title of the dialog box.  May be NULL if no title
//          is desired.  This string is not altered.
//
// LPSTR lpszSpec
//          Points to an ASCIIZ string contianing the default file extension
//          used to fill the file list box of the file open dialog.  This 
//          string should be a DOS wildcard specification.  If lpszExtension
//          is NULL, this parameter defaults to "*.*".  This string is
//          not altered if the DLGOPEN_NOCHANGESPEC flag is set.
// 
// WORD wFlags
//          Specifies flags, and may be any of the following.
// 
//          Value
//                Meaning
// 
//          DLGOPEN_MUSTEXIST
//                The user must select a file that actually exists on the 
//                disk before the dialog box will successfully close. Use 
//                this option for opening existing files, but not for asking 
//                the user for a name to save a file with.
//
//          DLGOPEN_NOSHOWSPEC
//                Causes the dialog box not to show the default search 
//                specification (as given by lpszExtension) in the 
//                dialog's edit box.  The default is to show the 
//                specification.  Note that if a default filename is 
//                initially specified in lpszFileBuf,the search spec will 
//                not be shown until the user changes directory or drive.
//
//          DLGOPEN_SHOWDEFAULT
//                Causes the initial file selection contained in the buffer 
//                lpszBuffer to be displayed in the file dialog edit 
//                control.  This flag is only used to setup the initial 
//                selection.  Once the user chooses another file or 
//                directory, the initial selection is discarded.
//
//          DLGOPEN_SAVE
//                Specifies that the OK button should display "Save".  
//                This flag may not be used in combination with the 
//                DLGOPEN_OPEN flag.
//
//          DLGOPEN_OPEN
//                Specifies that the OK button should display "Open".  
//                This flag may not be used in combination with the 
//                DLGOPEN_SAVE flag.
//
//          DLGOPEN_NOBEEPS
//                Specifies that the file dialog should not beep when the 
//                user enters an invalid entry.  The default is to beep 
//                once.
// 
//          DLGOPEN_SEARCHPATH
//                Specifies that the file dialog should search the 
//                PATH for files that are entered using a relative pathname.  
//                This option should be used with care, as it will find 
//                files in unexpected (from the user's perspective) 
//                locations.
//
//          DLGOPEN_NOCHANGESPEC
//                If this is set then the lpszSpec parameter is not
//                changed.                                 
//
// LPSTR lpszFileBuf 
//          Points to the buffer that is to be used to store the user's 
//          final file selection.  If the DLGOPEN_SHOWDEFAULT flag is 
//          specified, this buffer must initially contain the initial 
//          default selection to be displayed in the edit box of the dialog.
//
// WORD wFileLen 
//          Specifies the length in bytes of lpszFileBuf.  The user is 
//          not allowed to enter a filename longer than this value.
//          If 0 is passed then _MAX_PATH is used for this value.
//
// Return Value
//    Returns one of the following values:
//
//    Value
//          Meaning
//
//    DLG_CANCEL
//          The Cancel button was hit.  The contents of lpszFileBuf are 
//          unchanged.
//
//    DLG_MISSINGFILE
//          The OK button was hit with an filename that does not exist 
//          on the disk.  This value will never be returned if 
//          the DLGOPEN_MUSTEXIST flag was specified in wFlags.  The
//          contents of lpszFileBuf contain the filename that was selected.
// 
//    DLG_OKFILE
//          The OK button was hit with a valid file that exists on the 
//          disk.  The pathname of the file is copied in lpszFileBuf.
//
//
// Comments
//    The current directory is altered by this function as the moves 
//    among directories using the dialog box's directory/drive selection list.  
//    The current drive may also be changed.
//
//    Several of the flags mentioned above are not impelmented yet.  The
//    following are not impeleneted:
//       DLGOPEN_SAVE
//       DLGOEPN_OPEN
//       DLGOPEN_NOBEEPS
//       DLGOPEN_SEARCHPATH
//
//    The functionality providing multiple extensions is also not yet
//    implemented.
//          
//
// Related Functions
//    DefDlgOpenProc
//              
// 
// Modifications
//    CEK     11/13/90     First version.
//
//---------------------------------------------------------------------------
int FAR PASCAL
   FileOpenDialog( HWND    hwndParent,
                   LPSTR   lpszTitle,
                   LPSTR   lpszSpec,
                   WORD    wFlags,
                   LPSTR   lpszFileBuf,
                   WORD    wFileLen ) 
{
   FARPROC           lpfnDlgProc ;
   PFILEOPENSTRUCT   pFOS ;
   LOCALHANDLE       hlFOS ;
   int               n ;

   #ifndef IN_A_DLL
   HANDLE hInst ;
   hInst = GetWindowWord( hwndParent, GWW_HINSTANCE ) ;
   #endif

   if (!(hlFOS = LocalAlloc( LHND, sizeof( FILEOPENSTRUCT ) )))
   {
      return DLG_ERROR ;
   }

   pFOS = (PFILEOPENSTRUCT)LocalLock( hlFOS ) ;

   // Set up struct to pass to dlg box
   if (lpszTitle)
      lstrcpy( pFOS->szTitle, lpszTitle ) ;

   if ((wFlags & DLGOPEN_SHOWDEFAULT) && lpszFileBuf)
      lstrcpy( pFOS->szFileName, lpszFileBuf ) ;

   // Maximum file lenght.  If 0, then use maximum
   pFOS->wFileLen = wFileLen ? wFileLen : _MAX_PATH ;

   if (lpszSpec)
      lstrcpy( pFOS->szDefSpec, lpszSpec ) ;
   else
      lstrcpy( pFOS->szDefSpec, "*.*" ) ;

   pFOS->wFlags = wFlags ;

   LocalUnlock( hlFOS ) ;

   // Now bring up dlg box
   if (lpfnDlgProc = MakeProcInstance( fnFileOpen, hInst ))
   {
      n=  DialogBoxParam( hInst,
                        "FILEOPENBOX",
                        hwndParent,
                        lpfnDlgProc,
                        (LONG)hlFOS ) ;
      FreeProcInstance( lpfnDlgProc ) ;
   }
   else
      n = DLG_ERROR ;

   // If the user didn't hit cancel then copy the filename
   if (n == DLG_OK)
   {
      pFOS = (PFILEOPENSTRUCT)LocalLock( hlFOS ) ;
      lstrcpy( lpszFileBuf, pFOS->szFileName ) ;
      if (!(wFlags & DLGOPEN_NOCHANGESPEC))
         lstrcpy( lpszSpec, pFOS->szDefSpec ) ;
      LocalUnlock( hlFOS ) ;
   }

   LocalFree( hlFOS ) ;
   return n ;
}

//--------------------------------------------------------------------------
// Local functions
//--------------------------------------------------------------------------
BOOL UpdateFiles( HWND hDlg, LPSTR lpszBuf ) ;
BOOL UpdateDirs( HWND hDlg, LPSTR lpszBuf ) ;
BOOL ValidFile( HWND hDlg, LPSTR lpszFile, LPSTR lpszDefSpec ) ;
void SplitFileandPath( LPSTR lpszIn, LPSTR lpszPath, LPSTR lpszFname ) ;

//--------------------------------------------------------------------------
// fnFileOpen
//
// By CEK 11/13/90
//--------------------------------------------------------------------------                   
BOOL FAR PASCAL fnFileOpen( HWND hDlg,
                                   unsigned iMessage,
                                   WORD wParam,
                                   LONG lParam )
{
   char szBuf[_MAX_PATH] ;

   LOCALHANDLE hlFOS = GetProp( hDlg, FOSTRUCT_PROP ) ;

   PFILEOPENSTRUCT pFOS ;
   
   switch (iMessage)
   {
      case WM_INITDIALOG:

         hlFOS = (LOCALHANDLE)LOWORD( lParam ) ;

         if (!(pFOS = (PFILEOPENSTRUCT)LocalLock( hlFOS )))
         {
            EndDialog( hDlg, DLG_ERROR ) ;
            return -1 ;
         }

         // Put these handles in the property list
         if (!SetProp( hDlg, FOSTRUCT_PROP, hlFOS ))
         {
            return -1 ;
         }

         SendDlgItemMessage( hDlg, IDD_FILESPEC_EC, EM_LIMITTEXT,
                        pFOS->wFileLen, 0L ) ;
                        
         SetWindowText( hDlg, pFOS->szTitle ) ;


         DlgDirList( hDlg, pFOS->szDefSpec, IDD_DIR_LB, IDD_CURDIR,
                     0x0010 | 0x4000 | 0x8000 ) ;
         DlgDirList( hDlg, pFOS->szDefSpec, IDD_FILES_LB, 0,
                     0x0000 ) ;

         if (pFOS->wFlags & DLGOPEN_SHOWDEFAULT)
         {
            SetDlgItemText( hDlg, IDD_FILESPEC_EC, pFOS->szFileName ) ;
            SendDlgItemMessage( hDlg, IDD_FILES_LB, LB_SELECTSTRING,
                     -1, (LONG)(LPSTR)pFOS->szFileName ) ;
         }
         else
            if (pFOS->wFlags & DLGOPEN_NOSHOWSPEC)
               SetDlgItemText( hDlg, IDD_FILESPEC_EC, "" ) ;
            else
               SetDlgItemText( hDlg, IDD_FILESPEC_EC, pFOS->szDefSpec ) ;

         LocalUnlock( hlFOS ) ;
         // We might want to do this everytime we update?
         SetFocus( GetDlgItem( hDlg, IDD_FILESPEC_EC ) ) ;

      return FALSE ;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDD_FILES_LB:
               switch (HIWORD( lParam ))
               {
                  case LBN_SELCHANGE:
                     DlgDirSelect( hDlg, szBuf, IDD_FILES_LB ) ;
                     SetDlgItemText( hDlg, IDD_FILESPEC_EC, szBuf ) ;
                  break ;

                  case LBN_DBLCLK:
                     DlgDirSelect( hDlg, szBuf, IDD_FILES_LB ) ;
                     SetDlgItemText( hDlg, IDD_FILESPEC_EC, szBuf ) ;
                     SendMessage( hDlg, WM_COMMAND, IDOK, 0L ) ;
                  break ;
               }
            break ;


            case IDD_DIR_LB:
               switch (HIWORD( lParam ))
               {
                  case LBN_SELCHANGE:
                     if (DlgDirSelect( hDlg, szBuf, IDD_DIR_LB ))
                     {
                        pFOS = (PFILEOPENSTRUCT)LocalLock( hlFOS ) ;
                        if (!(pFOS->wFlags & DLGOPEN_NOSHOWSPEC))
                           lstrcat( szBuf, pFOS->szDefSpec ) ;
                        SetDlgItemText( hDlg, IDD_FILESPEC_EC, szBuf ) ;
                        LocalUnlock( hlFOS ) ;
                     }                     
                  break ;

                  case LBN_DBLCLK:
                     if (DlgDirSelect( hDlg, szBuf, IDD_DIR_LB ))
                     {
                        pFOS = (PFILEOPENSTRUCT)LocalLock( hlFOS ) ;
                        UpdateDirs( hDlg, szBuf ) ;
                        UpdateFiles( hDlg, pFOS->szDefSpec ) ;
                        if (pFOS->wFlags & DLGOPEN_NOSHOWSPEC)
                           SetDlgItemText( hDlg, IDD_FILESPEC_EC, "" ) ;
                        else
                           SetDlgItemText( hDlg, IDD_FILESPEC_EC,
                              pFOS->szDefSpec ) ;
                        LocalUnlock( hlFOS ) ;
                     }
                  break ;
               }
            break ;

            case IDOK:
               GetDlgItemText( hDlg, IDD_FILESPEC_EC, szBuf, _MAX_PATH ) ;
               pFOS = (PFILEOPENSTRUCT)LocalLock( hlFOS ) ;
               if (ValidFile( hDlg, szBuf, pFOS->szDefSpec ))
               {
                  lstrcpy( pFOS->szFileName, szBuf ) ;
                  RemoveProp( hDlg, FOSTRUCT_PROP ) ;
                  EndDialog( hDlg, DLG_OK ) ;
               }
               else
               {
                  UpdateDirs( hDlg, szBuf ) ;
                  UpdateFiles( hDlg, pFOS->szDefSpec ) ;
                  if (pFOS->wFlags & DLGOPEN_NOSHOWSPEC)
                     SetDlgItemText( hDlg, IDD_FILESPEC_EC, "" ) ;
                  else
                     SetDlgItemText( hDlg, IDD_FILESPEC_EC,
                        pFOS->szDefSpec ) ;
               }
               LocalUnlock( hlFOS ) ;
            break ;

            case IDCANCEL:
               RemoveProp( hDlg, FOSTRUCT_PROP ) ;
               EndDialog( hDlg, DLG_CANCEL ) ;
            break ;

            default:
               return FALSE ;
         }
      return FALSE ;

      default:
         return FALSE ;
   }
   return TRUE ;

}


BOOL UpdateFiles( HWND hDlg, LPSTR lpszBuf )
{
   SetDlgItemText( hDlg, IDD_FILESPEC_EC, lpszBuf ) ;
   DlgDirList( hDlg, lpszBuf, IDD_FILES_LB, 0, 0x0000 ) ;
   return TRUE ;
}

BOOL UpdateDirs( HWND hDlg, LPSTR lpszBuf )
{
   if (DlgDirList( hDlg, lpszBuf, IDD_DIR_LB, IDD_CURDIR,
               0x0010 | 0x4000 | 0x8000 ))
      return TRUE ;

   // if there is no : or \ at the end then we need to put
   // a \ because DlgDirList won't work right without it.
   if (lstrlen( lpszBuf ))
      if (lpszBuf[lstrlen( lpszBuf )-1] != '\\' ||
          lpszBuf[lstrlen( lpszBuf )-1] != ':')
         lstrcat( lpszBuf, "\\" ) ;

   DlgDirList( hDlg, lpszBuf, IDD_DIR_LB, IDD_CURDIR,
               0x0010 | 0x4000 | 0x8000 ) ;
   return TRUE ;
}

BOOL ValidFile( HWND hDlg, LPSTR lpszFile, LPSTR lpszDefSpec )
{
   char c ;
   OFSTRUCT  of ;
   LPSTR     lpszExt ;
   char      szPath[_MAX_PATH] ;
   char      szFname[_MAX_FNAME+_MAX_EXT] ;


   SplitFileandPath( lpszFile, szPath, szFname ) ;

   // Look for obvious change dir or drive
   // Things:  '\\', ':'
   c = *AnsiPrev( szPath, szPath + lstrlen( szPath ) ) ;
   if (c == '\\' || c == ':')
      return FALSE ;

   SplitFileandPath( lpszFile, szPath, szFname ) ;
   //
   // Does file spec have wild cards?
   //
   if (_fstrchr( szFname, '*' ) || _fstrchr( szFname, '?' ))
   {
      lstrcpy( lpszDefSpec, szFname ) ;
      return FALSE ;
   }

   //
   // Look for a ".."
   //
   if (_fstrstr( lpszFile, ".." ))
      return FALSE ;

   //
   // Now we're pretty sure it's a filename
   // so try to open it.
   //
   if (-1 == OpenFile( lpszFile, &of, OF_READ | OF_EXIST ))
   {
      // didn't work, so now try adding the default extension
      char szTemp[_MAX_PATH] ;
      lpszExt = _fstrchr( lpszDefSpec, '.' ) ;
      wsprintf( szTemp, "%s%s", (LPSTR)lpszFile, (LPSTR)lpszExt ) ;
      if (-1 == OpenFile( szTemp, &of, OF_READ | OF_EXIST ))
         return FALSE ;
      lstrcpy( lpszFile, szTemp ) ;
   }
   
   return TRUE ;   
}

void SplitFileandPath( LPSTR lpszIn, LPSTR lpszPath, LPSTR lpszFname )
{
   LPSTR lpsCur ;

   if ((lpsCur = _fstrrchr( lpszIn, '\\' )) ||
       (lpsCur = _fstrchr( lpszIn, ':')))
   {
      char c ;

      if (*(lpsCur+1) != '\0')
         lstrcpy( lpszFname, lpsCur+1 ) ;
      c = *(lpsCur+1) ;
      *(lpsCur+1) = '\0' ;
      lstrcpy( lpszPath, lpszIn ) ;
      *(lpsCur+1) = c ;
      return ;
   }
   lstrcpy( lpszFname, lpszIn ) ;
   lstrcpy( lpszPath, "" ) ;
   return ;
}

/************************************************************************
 * End of File: fileopen.c
 ************************************************************************/

