/************************************************************************
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  cmd.c
 *
 *     Remarks:  Command handler
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#include "iniedit.h"
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>         // time shit
#include "cmd.h"
#include "config.h"
#include "menu.h"
#include "about.h"
#include "toolbar.h"
#include "botstat.h"
#include "subdlgs.h"
#include "dlghelp.h"
#include "listbox.h"


/****************************************************************
 *  LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Handles all WM_COMMAND messages
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   BOOL fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;

   if (wMsg != WM_COMMAND)
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   switch( (WORD)wParam )
   {
      /*
       * File Menu
       */
      case IDM_FILE_OPEN:
         if (DoFileOpen( szCurFile, szCurFileTitle ))
            UpdateInfo( ) ;

      break ;

      case IDM_FILE_COMMIT:
         DP( hWDB, "IDM_FILE_COMMIT" ) ;

         fDirty = FALSE ;

      break ;

      case IDM_FILE_EDIT:
      {
         UINT ui ;

         char szEditor[MAX_PATH * 2] ;

         SetWaitCursor() ;
         /*
          * Before we launch the editor, we need to make
          * sure that the cache for the current file is
          * flushed.  Also, if there are any changes that have
          * been made, we need to ask if the user wants them
          * committed or not.
          */



         /*
          * Get current editor from INI file.
          */
         if (!GetPrivateProfileString( rglpsz[IDS_INI_MAIN],
                                       rglpsz[IDS_INI_EDITOR],
                                       "",
                                       szEditor,
                                       MAX_PATH*2,
                                       rglpsz[IDS_INI_FILENAME] ))
            lstrcpy( szEditor, rglpsz[IDS_DEFAULTEDITOR] ) ;

         lstrcat( szEditor, " " ) ;
         lstrcat( szEditor, szCurFile ) ;
         
         /*
          * Turn off error reporting
          */
         SetNormalCursor() ;

         ui = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;

         if (32 >= WinExec( szEditor, SW_SHOWNORMAL ))
            ErrorResBox( hWnd, NULL, MB_ICONEXCLAMATION,
                         IDS_APPTITLE, IDS_ERR_BADEDITOR,
                         (LPSTR)szEditor ) ;

         SetErrorMode( ui ) ;
      }
      break ;

      case IDM_FILE_PRINTSETUP:
         DoPrinterSetup() ;
         SetHelpAvailIndicator( hwndStat, TRUE ) ;
      break ;

      case IDM_FILE_PRINT:
         SetHelpAvailIndicator( hwndStat, FALSE ) ;
      break ;

      case IDM_FILE_EXIT:
         SendMessage (hWnd, WM_CLOSE, 0, 0L) ;
      break ;


      /*
       * Edit Menu
       */
      case IDM_EDIT_COPY:
      break ;

      case IDM_EDIT_COPY_FILE:
      break ;

      case IDM_EDIT_COPY_SECTION:
      break ;

      case IDM_EDIT_COPY_KEYVALUE:
      break ;

      case IDM_EDIT_COPY_KEY:
      break ;

      case IDM_EDIT_COPY_VALUE:
      break ;

      case IDM_EDIT_PASTE:
         fDirty = TRUE ;
      break ;

      case IDM_EDIT_UNDO:
         fDirty = FALSE ;
      break ;


      /*
       * Search Menu
       */
      case IDM_SEARCH_FIND:
         if (hdlgFindReplace)
         {
            DP1( hWDB, "Dialog is already up you geek" ) ;
            break ;
         }

         if (wFindReplaceMsg == 0)
            wFindReplaceMsg = RegisterWindowMessage( (LPSTR)FINDMSGSTRING ) ;

         fr.lStructSize = sizeof(FINDREPLACE);
         fr.hwndOwner = hWnd;
         fr.hInstance = NULL;
         fr.lpstrFindWhat = szFindWhat;
         fr.lpstrReplaceWith = (LPSTR)NULL;
         fr.wFindWhatLen = 256 ;
         fr.wReplaceWithLen = 0;
         fr.lCustData = 0L;
         fr.lpfnHook = (FARPROC)NULL;
         fr.lpTemplateName = (LPSTR)NULL;

         hdlgFindReplace = FindText(&fr) ;

         if (hdlgFindReplace)
            EnableFindReplace( FALSE  ) ;

      break ;

      case IDM_SEARCH_FINDNEXT:
         if (wFindReplaceMsg == 0)
            wFindReplaceMsg = RegisterWindowMessage( (LPSTR)FINDMSGSTRING ) ;

         /*
          * Using the last flags go search...
          */
         fr.lStructSize = sizeof(FINDREPLACE);
         fr.hwndOwner = hWnd;
         fr.hInstance = NULL;
         fr.lpstrFindWhat = szFindWhat;
         fr.lpstrReplaceWith = (LPSTR)NULL;
         fr.wFindWhatLen = 256 ;
         fr.wReplaceWithLen = 0;
         fr.lCustData = 0L;
         fr.lpfnHook = (FARPROC)NULL;
         fr.lpTemplateName = (LPSTR)NULL;

         fr.Flags |= FR_FINDNEXT ;

         SendMessage( hWnd, wFindReplaceMsg, 0, (LPARAM)(LPSTR)&fr ) ;
      break ;
      
      case IDM_SEARCH_REPLACE:
         if (hdlgFindReplace)
         {
            DP1( hWDB, "Dialog is already up you geek" ) ;
            break ;
         }

         if (wFindReplaceMsg == 0)
            wFindReplaceMsg = RegisterWindowMessage( (LPSTR)FINDMSGSTRING ) ;

         fr.lStructSize = sizeof(FINDREPLACE);
         fr.hwndOwner = hWnd;
         fr.hInstance = NULL;
         fr.lpstrFindWhat = szFindWhat;
         fr.lpstrReplaceWith = (LPSTR)szReplaceWith;
         fr.wFindWhatLen = 256 ;
         fr.wReplaceWithLen = 256 ;
         fr.lCustData = 0L;
         fr.lpfnHook = (FARPROC)NULL;
         fr.lpTemplateName = (LPSTR)NULL;

         hdlgFindReplace = ReplaceText(&fr) ;

         if (hdlgFindReplace)
            EnableFindReplace( FALSE ) ;

      break ;

      case IDM_SEARCH_ALL:
         CheckMenuItem( GetMenu( hWnd ), (WORD)wParam,  
            (GetMenuState( GetMenu( hWnd ), (WORD)wParam,
                          MF_BYCOMMAND ) & MF_CHECKED) ?
                        MF_UNCHECKED : MF_CHECKED ) ;
         
      break ;

      /*
       * Options Menu
       */
      case IDM_OPT_FONT:
         if (DoChooseFont())
         {
            if (hwndLB)
            {
               SetLBRedraw( FALSE ) ;
               SendMessage( hwndLB, WM_SETFONT, (WPARAM)hfontLB, (LPARAM)TRUE ) ;
               ResetLBValues() ;
               SetLBRedraw( TRUE ) ;
               InvalidateRect( hwndLB, NULL, TRUE ) ;
            }

         }
      break ;

      case IDM_OPT_EDITOR:
         DoEditor( ) ;
      break ;

      case IDM_OPT_TOOLBARLABELS:
      {
         RECT  rc ;

         SetWaitCursor() ;

         fToolBarLabels = !(GetMenuState( GetMenu( hWnd ), (WORD)wParam,
                                   MF_BYCOMMAND ) & MF_CHECKED)  ;

         ResetToolBar( hwndTool ) ;

         GetClientRect( hwndApp, &rc ) ;

         SendMessage( hwndApp, WM_SIZE, (WPARAM)SIZENORMAL,
            (LPARAM)MAKELONG( rc.right-rc.left, rc.bottom-rc.top ) ) ;

         CheckMenuItem( GetMenu( hWnd ), (WORD)wParam,
                        fToolBarLabels ? MF_CHECKED : MF_UNCHECKED ) ;

         /*
          * Write to INI file
          */
         SaveOptions( ) ;
         UpdateInfo( ) ;

         SetNormalCursor() ;
      }
      break ;

      case IDM_OPT_TOOLBAR:
      {
         RECT  rc ;

         SetWaitCursor() ;

         fToolBar = !(GetMenuState( GetMenu( hWnd ), (WORD)wParam,
                                   MF_BYCOMMAND ) & MF_CHECKED)  ;

         if (fToolBar)
         {
            if (hwndTool)
            {
               SetWindowPos( hwndTool, NULL, 0, 0, 0, 0,
                  SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
                  SWP_NOREDRAW ) ;
            }
            else
            {
               if (hwndTool = CreateToolBar( hWnd ))
               {
                   SetWindowPos( hwndTool, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW | SWP_NOREDRAW ) ;
               }
               else
                  fToolBar = FALSE ;
            }
         }
         else
            if (hwndTool)
               ShowWindow( hwndTool, SW_HIDE ) ;

         GetClientRect( hwndApp, &rc ) ;
         SendMessage( hwndApp, WM_SIZE, (WPARAM)SIZENORMAL,
                     (LPARAM)MAKELONG( rc.right-rc.left, rc.bottom-rc.top ) ) ;

         CheckMenuItem( GetMenu( hWnd ), (WORD)wParam,
                        fToolBar ? MF_CHECKED : MF_UNCHECKED ) ;

         if (hwndStat)
            InvalidateRect( hwndStat, NULL, TRUE ) ;

         /*
          * Write to INI file
          */
         SaveOptions( ) ;
         UpdateInfo( ) ;

         SetNormalCursor() ;

      }
      break ;

      case IDM_OPT_STAT:
      {
         RECT  rc ;

         SetWaitCursor() ;

         fStatLine = !(GetMenuState( GetMenu( hWnd ), (WORD)wParam,
                                   MF_BYCOMMAND ) & MF_CHECKED)  ;
         if (fStatLine)
         {
            if (hwndStat)
               SetWindowPos( hwndStat, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW | SWP_NOREDRAW ) ;
            else
            {
               if (hwndStat = CreateStatus( hWnd ))
                   SetWindowPos( hwndStat, NULL, 0, 0, 0, 0,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE |
                        SWP_SHOWWINDOW | SWP_NOREDRAW ) ;
               else
                  fStatLine = FALSE ;
            }
         }
         else
            if (hwndStat)
               ShowWindow( hwndStat, SW_HIDE ) ;

         GetClientRect( hwndApp, &rc ) ;
         SendMessage( hwndApp, WM_SIZE, (WPARAM)SIZENORMAL,
            (LPARAM)MAKELONG( rc.right-rc.left, rc.bottom-rc.top ) ) ;

         CheckMenuItem( GetMenu( hWnd ), (WORD)wParam,
                        fStatLine ? MF_CHECKED : MF_UNCHECKED ) ;

         /*
          * Write to INI file
          */
         SaveOptions( ) ;
         UpdateInfo( ) ;

         SetNormalCursor() ;

      }
      break ;


      /*
       * Help Menu
       */
      case IDM_HELP_CURRENT:
         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_CONTENTS, 0L ) ;
      break ;

      case IDM_HELP_CONTENTS:
         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_CONTENTS, 0L ) ;
      break ;

      case IDM_HELP_ONHELP:
         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_HELPONHELP, 0L ) ;
      break ;

      case IDCHILD_STAT:
      case IDM_HELP_ABOUT:
         DoAboutBox() ;
      break ;

      /*
       * Child controls
       */
      case IDCHILD_LISTBOX:
         lbProcessCmd( (HWND)LOWORD( lParam ), HIWORD( lParam ) ) ;
      break ;

      case IDCHILD_TOOLBAR:
      break ;

      case IDCHILD_INPUT:
      break ;


      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   SetDirtyIndicator( hwndStat, fDirty ) ;

   return 0L ;

} /* CmdHandler()  */

/****************************************************************
 *  LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Handles all WM_SYSCOMMAND messages
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   if (wMsg != WM_SYSCOMMAND)
      return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

   switch( (WORD)wParam )
   {

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* SysCmdHandler()  */


/****************************************************************
 *  void NEAR PASCAL UpdateInfo( void ) ;
 *
 *  Description: 
 *
 *    Updates all status indicators to reflect the current state
 *    of the app.  It sets the window text of the stat bar to indicate
 *    the current filename, section, and key.
 *
 *  Comments:
 *
 ****************************************************************/
void WINAPI UpdateInfo( void )
{
   LPINIFILE   lpINI ;
   LPSTR       lpszBuf ;

   char szFile[MAX_PATH] ;
   struct stat Stat ;

   if (lpCurNode)
   {
      lpszBuf = GALLOC( 1024 ) ;

      switch (NODEHEADER_wMagic(lpCurNode))
      {
         case INIFILE_TAG:
            lpINI = (LPINIFILE)lpCurNode ;
            lstrcpy( szCurFile, INIFILE_lpszFile(lpINI) ) ;

            if (lpszBuf)
               lstrcpy( lpszBuf, szCurFile ) ;

         break ;

         case SECTION_TAG:
            lpINI = (LPINIFILE)SECTION_lpParent(lpCurNode) ;
            lstrcpy( szCurFile, INIFILE_lpszFile(lpINI) ) ;

            if (lpszBuf)
               wsprintf( lpszBuf, "%s : [%s]", (LPSTR)szCurFile,
                  (LPSTR)SECTION_lpszSec(lpCurNode) ) ;
         break ;

         case KEYVALUE_TAG:
            lpINI = (LPINIFILE)
               SECTION_lpParent( KEYVALUE_lpParent( lpCurNode ) ) ;

            if (lpszBuf)
               wsprintf( lpszBuf, "%s : [%s]", (LPSTR)szCurFile,
                  (LPSTR)SECTION_lpszSec( KEYVALUE_lpParent( lpCurNode ) ) ) ;
         break ;
      }

      /*
      * Check the file info and update if needed
      */
      lstrcpy( szFile, INIFILE_lpszFile(lpINI) ) ;
      stat( szFile, &Stat ) ;
      if ((DWORD)Stat.st_size != INIFILE_dwFileSize(lpINI) ||
            Stat.st_atime != INIFILE_tTimeDate(lpINI))
      {
         /*
          * File has changed.  Refresh...
          *
          * We need to nuke all information kept about this file...
          */
//         INIFILE_dwFileSize(lpINI) = (DWORD)Stat.st_size ;
//         INIFILE_tTimeDate(lpINI) = (DWORD)Stat.st_atime ;

//         FREEFROMHEAP( hhOutBuf, INIFILE_lpszOutBuf(lpINI) ) ;
//         INIFILE_lpszOutBuf(lpINI) = NULL ;
      }

      if (lpszBuf)
      {
         SendMessage( hwndStat, ST_SETDEFAULTTEXT, 0, (LPARAM)lpszBuf ) ;
         GFREE( lpszBuf ) ;
         return ;
      }
   }

   SendMessage( hwndStat, ST_SETDEFAULTTEXT, 0, (LPARAM)(LPSTR)szCurFile ) ;


} /* UpdateInfo()  */

/************************************************************************
 * End of File: cmd.c
 ***********************************************************************/

