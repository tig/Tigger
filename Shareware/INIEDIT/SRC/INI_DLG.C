// ini_dlg.c
//
// Main Dialog routine for INIedit
//
// Copyright 1990, CE Kindel
//

#include <windows.h>
#include <wdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <ceklib.h>
#include <fcntl.h>
#include <string.h>
#include "..\inc\inieditd.h"
#include "..\inc\iniedit.h"
#include "..\inc\ini_dlg.h"
#include "..\inc\ini_ctrl.h"
#include "..\inc\ini_add.h"

extern HANDLE hInst ;
extern char szFileName [] ;   
extern char szSection [] ;    // Used by Is386Enh()
extern char szAppName [] ;
extern char szVersion [] ;
extern char    **__argv ;
extern int     __argc ;


char szSysDir[INI_MAXPATH+1] ;
char szFileSpec[] = DEFAULT_FILESPEC ;
char szDevice[64] ;
char szDriver[140] ;
char szPort[140] ;
HWND    hwndCombo, hwndName, hwndSection ;

#define SHAREWARE

#ifdef SHAREWARE
BOOL  fRegistered = FALSE ;
#else
BOOL  fRegistered = TRUE ;
#endif

HWND  hMainWnd ;

BOOL Is386Enh( LPSTR szFileName, LPSTR szSection ) ;
void DoAbout( HWND hWnd ) ;
void Print( void) ;

long FAR PASCAL MainDlgProc(  HWND     hWnd,
                              WORD wMsg,
                              WORD     wParam,
                              LONG     lParam )
{
   FARPROC        lpfnDlgProc ;  // general purpose pointer
   char           szBuf[INI_MAXPATH] ;
   int            i ;
   WORD           wIndex ;
   HWND           hwndCur ;
   HDC            hDC ;
   TEXTMETRIC     tm ;
   PAINTSTRUCT    ps ;
   RECT           rc ;
   static  short  nMinX, nMinY ;
   OFSTRUCT of ;

   switch (wMsg)
   {
      case WM_CREATE :
         //
         // Child controls are not created yet
         //
         break ;

      case WM_MYINITDIALOG :
         //
         //  Disable buttons for now
         //
         hMainWnd = hWnd ;
         hwndCombo   = GetDlgItem (hWnd, IDD_FNAME_CB) ;
         hwndName    = GetDlgItem (hWnd, IDD_NAME_LB) ;
         hwndSection = GetDlgItem (hWnd, IDD_SECTION_LB) ;
         i = GetWindowsDirectory (szSysDir, INI_MAXPATH ) ;
         // if the system dir is the root there is a \ otherwise we need it.
         if (i != 3)
            szSysDir[i++] = '\\' ;
         szSysDir [i] = '\0' ;
         wsprintf (szBuf, (LPSTR) "%s%s", (LPSTR) szSysDir, (LPSTR) szFileSpec) ;

         DlgDirListComboBox( hWnd, szBuf, IDD_FNAME_CB,
                             0, INI_FILETYPE) ;

         DlgDirList( hWnd, szBuf, IDD_DUMMY_LB,
                             IDD_DIRECTORY, INI_FILETYPE) ;

         /*
          * If a filename was specified on the command line then
          * add it to the combo box, then select it.
          * Try to open the file.  If it opens it is at least a file.
          * Use it.  Otherwise let the user know and use WIN.INI.
          */
         if ((__argc > 1) &&
             (OpenFile( __argv[1], &of, OF_EXIST) != HFILE_ERROR))
         {
            if (0 == strnicmp( szSysDir, __argv[1], lstrlen( szSysDir )))
               lstrcpy( szBuf, __argv[1]+lstrlen( szSysDir ) ) ;
            else
               lstrcpy( szBuf, __argv[1] ) ;

            AnsiLower( szBuf ) ;

            if (CB_ERR == ( wIndex = (WORD)SendMessage( hwndCombo,
                                    CB_FINDSTRING,
                                    -1,
                                    (LONG)(LPSTR)szBuf )))
            {
               SendMessage( hwndCombo, CB_ADDSTRING,
                                 0, (LONG)(LPSTR)szBuf ) ;

               wIndex = (WORD) SendMessage( hwndCombo, CB_FINDSTRING,
                                 -1, (LONG)(LPSTR)szBuf ) ;
            }
         }
         else
         {
            if (__argc > 1)
            {
               char szBuf[256] ;
               wsprintf( szBuf,
                  "The filename '%s', which was specified\n"
                  "on the command line is not a valid file.",
                  (LPSTR)__argv[1] ) ;

               MessageBox( hWnd, szBuf, szAppName, MB_ICONEXCLAMATION ) ;

            }

            wIndex = (WORD) SendMessage( hwndCombo,
                                       CB_FINDSTRING,
                                       -1,
                                       (LONG)(LPSTR) DEFAULT_FILENAME ) ;
         }

         SendMessage ( hwndCombo,
                       CB_SETCURSEL,
                       wIndex == CB_ERR ? 0 : wIndex,
                       0L ) ;
         FillSectionLB() ;
         // Get Current Printer
         if (DLG_ERROR == PrintSelPrnDialog( hWnd,
                                                (LPSTR)"Select Printer",
                                                (LPSTR)szDriver,
                                                (LPSTR)szDevice,
                                                (LPSTR)szPort,
                                                SELPRN_GETDEFAULT ))
         {
            lstrcpy( szDriver, "" ) ;
            lstrcpy( szDevice, "" ) ;
            lstrcpy( szPort,   "" ) ;
         }

         #ifdef SHAREWARE
         if (!(fRegistered = IsRegisteredUser( szAppName, 
                                              NULL, NULL, REGISTER_READ )))
         {
            DoAbout( hWnd ) ;
            fRegistered = RegistrationDialog( hWnd, szAppName, 0 ) ;
         }
         #endif

         GetWindowRect( hWnd, &rc ) ;
         nMinX = rc.right - rc.left ;
         nMinY = rc.bottom - rc.top ;
         ShowWindow( GetDlgItem( hWnd, IDD_DUMMY_LB), SW_HIDE ) ;
         ShowWindow( GetDlgItem( hWnd, IDD_DIRECTORY), SW_HIDE ) ;
      break ;

      case WM_WININICHANGE:
         FillSectionLB() ;
      break ;

      case WM_SETFOCUS:
         SetFocus( hwndSection ) ;
      break ;

      case WM_COMMAND :
         switch (wParam)
         {
            // Menu items...
            case IDM_ABOUT :
               hwndCur = GetFocus () ;
               DoAbout( hWnd ) ;
               SetFocus( hwndCur ) ;
               break ;

            case IDM_EXIT :
               SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;

            case IDM_OTHERFILE:  // allow user to specify a file not in windir
               wIndex = FileOpenDialog( hWnd, "Open an INI file", "*.INI",
                                        DLGOPEN_OPEN | DLGOPEN_MUSTEXIST,
                                        szFileName, INI_MAXPATH-1 ) ;
               if (wIndex == DLG_OK)
               {
                  char szBuf[_MAX_PATH] ;

                  AnsiLower( szFileName ) ;
                  wsprintf( szBuf, (LPSTR) "%s%s", (LPSTR)".",
                            (LPSTR)"" ) ;

                  DlgDirListComboBox( hWnd, szBuf, IDD_DUMMY_LB,
                                      IDD_DIRECTORY, 0x8006 ) ;

                  GetDlgItemText( hWnd, IDD_DIRECTORY, szBuf, _MAX_PATH ) ;

                  if (lstrlen( szBuf ) > 3)
                     lstrcat( szBuf, "\\" ) ;

                  if (lstrcmpi( szBuf, szSysDir ))
                  {
                     lstrcat( szBuf, szFileName ) ;
                     lstrcpy( szFileName, szBuf ) ;
                     if (CB_ERR == (int)SendMessage( hwndCombo, CB_FINDSTRING, 0,
                                       (LONG)(LPSTR)szFileName ))
                        SendMessage( hwndCombo, CB_ADDSTRING, 0,
                                     (LONG)(LPSTR)szFileName ) ;
                  }

                  SendMessage( hwndCombo, CB_SHOWDROPDOWN, TRUE, 0L ) ;

                  SendMessage( hwndCombo, CB_SELECTSTRING, -1,
                               (LONG)(LPSTR)szFileName ) ;
                  InvalidateRect( hwndCombo, NULL, TRUE ) ;
                  UpdateWindow( hwndCombo ) ;

                  SendMessage( hwndCombo, CB_SHOWDROPDOWN, FALSE, 0L ) ;

                  FillSectionLB() ;

                  wsprintf( szBuf, (LPSTR) "%s%s", (LPSTR)szSysDir,
                            (LPSTR)"" ) ;

                  DlgDirListComboBox( hWnd, szBuf, IDD_DUMMY_LB,
                                      IDD_DIRECTORY, 0x8006 ) ;

                  GetDlgItemText( hWnd, IDD_DIRECTORY, szBuf, _MAX_PATH ) ;

               }
               else
               {
                  DP1( hWDB, "FileOpen failed! %s", (LPSTR)szFileName ) ;
               }

               break ;

            case IDM_NOTEPAD :
               wsprintf( szBuf, (LPSTR)"NOTEPAD.EXE %s",
                                (LPSTR)szFileName ) ;
               if (32 > WinExec( szBuf, SW_SHOWNORMAL ))
                  MessageBox( hWnd, "Could not execute NotePad",
                                    szAppName, MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_PRINT :
               Print() ;
               break ;

            case IDM_PRINTSETUP :
               if (DLG_ERROR == PrintSelPrnDialog( hWnd,
                                                      (LPSTR)"Select Printer",
                                                      (LPSTR)szDriver,
                                                      (LPSTR)szDevice,
                                                      (LPSTR)szPort,
                                                      SELPRN_STANDARD | SELPRN_ALLOWSETUP))
                  MessageBox( hWnd, "There was a problem selecting a printer.",
                                    szAppName, MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_UNDO :
               break ;

            case IDM_CUT :
               break ;

            case IDM_COPY :
               break ;

            case IDM_PASTE :
               break ;

            case IDM_HELP :
                WinHelp( hWnd, "INIEDIT.HLP", HELP_CONTENTS, NULL ) ;
               break ;

            case IDM_HELPINDEX :
               break ;

            // Dialog controls
            case IDD_FNAME_CB :          // FileName combo
               if (!ProcessFileNameCB (lParam))
                  return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
               break ;
#if 0
            case IDOK :       // This is here because it has to do with the CB
               hwndCur = GetFocus() ;
               if (hwndCur == hwndCombo)
               {
                  SendMessage(hwndCombo, CB_SHOWDROPDOWN, FALSE, 0L ) ;
                  SetFocus( hwndSection ) ;
               }
               if (hwndCur == hwndSection)
                  SetFocus( hwndName ) ;
               if (hwndCur == hwndName)
               {
                  SetFocus( hwndCombo ) ;
                  SendMessage(hwndCombo, CB_SHOWDROPDOWN, TRUE, 0L ) ;
               }

               break ;
#endif
            case IDD_NEWFILE_BTN :       // Create new file button
               break ;

            case IDD_SAVEFILE_BTN :      // Save current file button
               break ;

            case IDD_SECTION_LB :        // Section name list box
               ProcessSectionLB( lParam ) ;
               break ;

            case IDD_SECTIONADD_BTN :    // Add a section button
               hwndCur = GetFocus() ;
               AddSection() ;
               SetFocus( hwndCur ) ;
               break ;

            case IDD_SECTIONDEL_BTN :    // Delete a section button
               DelSection() ;
               break ;

            case IDD_NAME_LB :           // Item Name list box
               ProcessNameLB( lParam ) ;
               break ;

            case IDD_NAMEADD_BTN :       // Add an item button
               hwndCur = GetFocus() ;
               if (!Is386Enh( szFileName, szSection ))
                  AddName () ;
               SetFocus( hwndCur ) ;
               break ;

            case IDD_NAMEDEL_BTN :       // Delete current item button
               hwndCur = GetFocus() ;
               if (!Is386Enh( szFileName, szSection ))
                  DelName () ;
               SetFocus( hwndCur ) ;
               break ;

            case IDD_EDIT_BTN :
               hwndCur = GetFocus () ;
               if (!Is386Enh( szFileName, szSection ))
               {
                  lpfnDlgProc = MakeProcInstance( ValueDlgProc, hInst ) ;
                  if (DialogBox( hInst, "EditValue", hMainWnd, lpfnDlgProc ))
                     FillNameLB() ;
                  FreeProcInstance( lpfnDlgProc ) ;
               }
               SetFocus( hwndCur ) ;
               break ;

            case IDD_COMMENT_EC :        // Comment edit control
               break ;

         } // switch (wParam)
         break ;

      case WM_LBUTTONDOWN:
      {
         //
         // Pop up the about box if user clicks on icon
         //
         RECT    rc;
         POINT   pt;

         pt = MAKEPOINT( lParam );
         GetWindowRect( GetDlgItem( hWnd, IDD_INIICON ), &rc );

         ClientToScreen( hWnd, &pt );

         if ( PtInRect( &rc, pt ) )
         {
            hwndCur = GetFocus () ;
            DoAbout( hWnd ) ;
            SetFocus( hwndCur ) ;
         }
      }
      break ;

      case WM_NCLBUTTONDOWN:
         return DefDlgProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

      case WM_GETMINMAXINFO:
      {
         LPPOINT lppt ;

         if (!nMinX)
            break ;

         lppt = (LPPOINT)lParam ;

         lppt[3].x = nMinX ;
         lppt[3].y = nMinY ;

         lppt[4].x = nMinX ;
         lppt[4].y = nMinY ;

      }
      break ;

      case WM_PAINT :
         hDC = BeginPaint(hWnd, &ps ) ;
         GetTextMetrics( hDC, &tm ) ;
         DrawShadow( hDC, GetDlgItem( hWnd, IDD_SECTION_GRP ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         DrawShadow( hDC, GetDlgItem( hWnd, IDD_NAME_GRP ),
                     GetStockObject( GRAY_BRUSH ), tm.tmAveCharWidth / 2 ) ;
         EndPaint( hWnd, &ps ) ;
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;


      case WM_DESTROY :
         SaveWindowPos( hWnd ) ;
         PostQuitMessage (0) ;
         break ;

      default :
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   } // switch (wMsg)

   return 0L ;
}
// end of MainDlgProc


//-------------------------------------------------------
// Is386Enh( LPSTR szFileName, LPSTR szSection )
//
// Returns TRUE if the filename is SYSTEM.INI and
// section is [386Enh].
// Returns FALSE otherwise.
//
BOOL Is386Enh( LPSTR lpszFileName, LPSTR lpszSection )
{
   char szFileName[144] ;
   char szDrive[_MAX_DRIVE] ;
   char szDir[_MAX_DIR] ;
   char szFName[_MAX_FNAME] ;
   char szExt[_MAX_EXT] ;

   lstrcpy( szFileName, lpszFileName ) ;
   _splitpath( szFileName,
               szDrive,
               szDir,
               szFName,
               szExt ) ;

   if (lstrcmpi( szFName, "SYSTEM" ) || lstrcmpi( lpszSection, "386Enh" ))
      return FALSE ;

   MessageBox( hMainWnd,
   "The [386Enh] section of SYSTEM.INI\ncannot be modifed by INIedit,\nuse 'File.Edit with Notepad' instead.",
   szAppName, MB_ICONINFORMATION ) ;

   return TRUE ;
}



//-------------------------------------------------------
// void DoAbout( hWnd) 
//
void DoAbout( HWND hWnd ) 
{
   char szDescription [] =
"INI file editor for Windows.\n\
The shareware registration fee for INIedit is $25.00." ;

   if (DLG_ERROR == AboutDialog(   hWnd,
                        "About INIedit",
                        szAppName,
                        szVersion,
                        szDescription,
                        fRegistered ? ABOUT_REGISTERED : ABOUT_BIG ))
   {
      DP1( hWDB, "AboutDialog returned DLG_ERROR!" ) ;
      MessageBox( hMainWnd, "Could not open About Box!", szAppName,
                  MB_ICONEXCLAMATION ) ;
   }
}


