/************************************************************************
 *
 *     Project:  INIedit 2.0
 *
 *      Module:  iniedit.c
 *
 *     Remarks:  Main module, contains WinMain and fnMainWnd
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "iniedit.h"
#include "version.h"
#include "config.h"
#include "botstat.h"
#include "toolbar.h"
#include "init.h"
#include "wndmgr.h"
#include "menu.h"
#include "cmd.h"
#include "font.h"
#include "dlghelp.h"
#include "about.h"
#include "input.h"
#include "listbox.h"
#include "subdlgs.h"
#include "wintime.h"
#include "fileio.h"
#include "file.h"

/*
 * Global variables extern'd in iniedit.h
 */
HWND        hwndApp ;
HWND        hwndLB ;
HWND        hwndInput ;
HWND        hwndTool ;
HWND        hwndStat ;
HWND        hwndEdit ;

HINSTANCE   hinstApp ;

BOOL        fWin31 ;

char        szVerNum[32] ;
char        szRegisteredName[MAX_REGNAME_LEN] ;

LPSTR       rglpsz[LAST_IDS - FIRST_IDS + 1] ;

/*
 * Globally managed lists (HLSQSTR.DLL)
 */
HLIST       hlstFiles ;
HHEAP       hhOutBuf ;     // output buffer heap (for od listbox)
HHEAP       hhLists ;      // all lists
HHEAP       hhStrings ;    // all strings
HHEAP       hhScratch ;    // scratch heap

HFONT       hfontSmall ;
HFONT       hfontLB ;      // this is the one that is settable
char        szFont[LF_FACESIZE] ;
WORD        wFontStyle ;
WORD        wFontSize ;

SORTPROC    lpfnFileCompare ;
SORTPROC    lpfnSectionCompare ;
SORTPROC    lpfnKeyValueCompare ;

/*
 * State variables
 */
BOOL        fSearchAll ;
BOOL        fToolBar ;
BOOL        fToolBarLabels ;
BOOL        fStatLine ;
short       xPos, yPos, nWidth, nHeight ;
BOOL        fSortSections ;
BOOL        fSortKeys ;

BOOL        fDirty ;


/*
 * Currently active file variables
 */
char           szCurFile[256], szCurFileTitle[256] ;
LPNODEHEADER   lpCurNode ;

/*
 * Find/Replace dialog box stuff
 */
FINDREPLACE fr ;
HWND        hdlgFindReplace ;
WORD        wFindReplaceMsg ;
char        szFindWhat[256] ;
char        szReplaceWith[256] ;


/*
 * Printer setup stuff
 */
GLOBALHANDLE ghDevNames ;
GLOBALHANDLE ghDevMode ;

/*
 * Semaphores
 */
int         semCurFile ;


/*
 * Variables local to this module.
 */ 
static HCURSOR    hcurWait ;
static HCURSOR    hcurNorm ;
static HCURSOR    hcurSave ;
static WORD       fWaitCursor ;

/****************************************************************
 *  LRESULT FAR PASCAL
 *    fnMainWnd( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Main Window procedure.
 *
 *  Comments:
 *
 ****************************************************************/

LRESULT CALLBACK fnMainWnd( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   switch( wMsg )
   {
      case WM_CREATE:

         TimeResetInternational() ;

         #ifdef WANTINPUTBAR
         /*
          * Create input bar
          */
         if (hwndInput = CreateInput( hWnd ))
            ShowWindow( hwndInput, SW_NORMAL ) ;
         else
         {
            DP1( hWDB, "Could not create input window, bailing out.") ;
            return (LRESULT)-1 ;
         }
         #endif

         /*
          * Create listbox
          */
         if (hwndLB = CreateLB( hWnd ))
            ShowWindow( hwndLB, SW_NORMAL ) ;
         else
         {
            DP1( hWDB, "Could not create ListBox, bailing out.") ;
            return (LRESULT)-1 ;
         }


         /*
          * Create status bar
          */
         if (fStatLine)
         {
            if (hwndStat = CreateStatus( hWnd ))
               ShowWindow( hwndStat, SW_NORMAL ) ;
            else
               fStatLine = FALSE ;
         }

         /*
          * Create tool bar
          */
         if (fToolBar)
         {
            DP5( hWDB, "Creating ToolBar...") ;
            if (hwndTool = CreateToolBar( hWnd ))
            {
               ShowWindow( hwndTool, SW_NORMAL ) ;
            }
            else
               fToolBar = FALSE ;
         }

         CheckMenuItem( GetMenu( hWnd ), (WORD)IDM_OPT_STAT,
                        fStatLine ? MF_CHECKED : MF_UNCHECKED ) ;
         CheckMenuItem( GetMenu( hWnd ), (WORD)IDM_OPT_TOOLBAR,
                        fToolBar ? MF_CHECKED : MF_UNCHECKED ) ;
         CheckMenuItem( GetMenu( hWnd ), (WORD)IDM_OPT_TOOLBARLABELS,
                        fToolBarLabels ? MF_CHECKED : MF_UNCHECKED ) ;


         /*
          * Get files in WINDOWS dir.
          */
         hlstFiles = GetDirectoryList( NULL, NULL, NULL ) ;

         /*
          * Get files in INIEDIT.INI
          */

         /*
          * Put in list box
          */
         if (hlstFiles)
         {
            LPINIFILE   lpINI ;
            WORD        w = 0;
            WORD        wCur = -1;

            if (lpINI = hlsqListFirstNode( hlstFiles ))
               do
               {
                  if (0 == lstrcmpi( lpINI->lpszFile, szCurFile ))
                     wCur = w ;

                  LBAddString( hwndLB, (LPSTR)lpINI ) ;
                  w++ ;
               }
               while (lpINI = hlsqListNextNode( hlstFiles, lpINI )) ;

            if (wCur == -1)
            {
               AddSelectFile( szCurFile, &w ) ;
            }
            else
               SendMessage( hwndLB, LB_SETCURSEL, (WPARAM)wCur, 0L ) ;

            /*
             * What ever file is selected should be opened.
             */

         }
      break ;

      case WM_SETFOCUS:
         if (hwndEdit && IsWindowVisible( hwndEdit ))
            SetFocus( hwndEdit ) ;
         else
            if (hwndLB)
               SetFocus( hwndLB ) ;
            else
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      break ;


      case WM_DESTROY:
         if (hlstFiles)
            hlsqListDestroy( hlstFiles ) ;
         /*
          * Destroy status bar
          */
         if (hwndStat)
            DestroyWindow( hwndStat ) ;

         /*
          * Destroy tool bar
          */
         if (hwndTool)
            DestroyWindow( hwndTool ) ;

         /*
          * Destroy input bar
          */
         if (hwndInput)
            DestroyWindow( hwndInput ) ;

         /*
          * Destroy listbox
          */
         DestroyLB() ;

         /*
          * If the find/replace dialog is up, nuke it.
          */
         if (hdlgFindReplace)
         {
            DestroyWindow( hdlgFindReplace ) ;
            hdlgFindReplace = NULL ;
         }

         WinHelp( hWnd, rglpsz[IDS_HELPFILE], HELP_QUIT, 0L ) ;

         /*
          * We already are assured of being SW_RESORE so just
          * get the coords for the save (SaveDefaults)
          */
         PostQuitMessage( 0 ) ;

      break ;

      case WM_CLOSE :
      {
         BOOL fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;

         if (fDirty && !fShiftDown)
            switch( ErrorResBox( hWnd, hinstApp,
                                 MB_YESNOCANCEL | MB_ICONQUESTION,
                                 IDS_APPTITLE, IDS_ERR_SAVECHANGES ))
            {
               case IDCANCEL :
                  return 0L ;
               break ;

               case IDYES :
//                  if (!DoSaveConfig())
//                  return 0L ;
               break ;

               case IDNO :
                  SaveWndPos( hWnd ) ;
                  return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
               break ;

               default:
                  MessageBeep( 0 ) ;
                  return 0L ;
               break ;
            }
         SaveWndPos( hWnd ) ;
         return DefWindowProc (hWnd, wMsg, wParam, lParam) ;
      }
      break ;

      case WM_GETMINMAXINFO:
      case WM_SIZE:
         DP5( hWDB, "WM_SIZE/WM_GETMINMAXINFO" );
         return SizeHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_COMMAND:
         DP5( hWDB, "WM_COMMAND" );
         return CmdHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_SYSCOMMAND:
         DP5( hWDB, "WM_SYSCOMMAND" );
         return SysCmdHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_INITMENU:
      case WM_MENUSELECT:
      case WM_INITMENUPOPUP:
         DP5( hWDB, "WM_*MENU*" ) ;
         return MenuHandler( hWnd, wMsg, wParam, lParam ) ;

      case WM_SYSCOLORCHANGE:
         ResetLBColors() ;
         if (hwndTool)
            SendMessage( hwndTool, wMsg, wParam, lParam ) ;
      break ;

      case WM_DRAWITEM:
      case WM_MEASUREITEM:
      case WM_CHARTOITEM:
      case WM_VKEYTOITEM:
      case WMU_CURFILECHANGE:
      case WM_DELETEITEM:
         return lbOwnerDraw( hWnd, wMsg, wParam, lParam ) ;
      break ;

      case WM_WININICHANGE:
         if (0 == lstrcmpi( "Intl", (LPSTR)lParam ))
         {
            TimeResetInternational() ;

            /*
             * Need to to something that will cause
             * the strings to be re calculated
             */
         }
      break ;

      default:
         if (wMsg && (wMsg == wFindReplaceMsg))
         {

            if ( fr.Flags & FR_DIALOGTERM )
            {
               DP5( hWDB, "Dialog is gone" ) ;
               hdlgFindReplace = NULL ;
               fr.Flags &= ~FR_DIALOGTERM ;
               EnableFindReplace( TRUE ) ;
               return NULL ;
            }

            /*
             * Search for next thing.
             */
            DP5( hWDB, "Search!" ) ;
            //DoFindReplace( ) ;
            return NULL ;
         }
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* fnMainWnd()  */


/****************************************************************
 *  VOID WINAPI SetWaitCursor( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
WORD nCurCount ;

VOID WINAPI SetWaitCursor( VOID )
{
   DASSERT( hWDB, (short)fWaitCursor >= 0 ) ;

   if (!hcurWait)
      hcurWait = LoadCursor( NULL, IDC_WAIT ) ;

   fWaitCursor++ ;

   SetCursor( hcurWait ) ;

} /* SetWaitCursor()  */

/****************************************************************
 *  VOID WINAPI SetNormalCursor( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SetNormalCursor( VOID )
{
   DASSERT( hWDB, (short)fWaitCursor >= 0 ) ;
   if (!hcurNorm)
      hcurNorm = LoadCursor( NULL, IDC_ARROW ) ;

   fWaitCursor-- ;

   if (fWaitCursor)
      return ;

   SetCursor( hcurNorm ) ;

} /* SetNormalCursor()  */

/****************************************************************
 *  int FAR PASCAL MessageLoop( HACCEL haccl )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL MessageLoop( HACCEL haccl )
{
   MSG            msg ;

   DP5( hWDB, "Entering message loop..." ) ;
   while (GetMessage (&msg, NULL, 0, 0))
   {
      if (hdlgFindReplace == 0 || !IsDialogMessage( hdlgFindReplace, &msg ))
      {
         if (!TranslateAccelerator( hwndApp, haccl, &msg ))
         {
            TranslateMessage( &msg ) ;
            DispatchMessage( &msg ) ;
         }
      }
   }

   DP( hWDB, "fWaitCursor = %d", fWaitCursor ) ;
   return (int)msg.wParam ;

} /* MessageLoop()  */

/****************************************************************
 *  int PASCAL
 *    WinMain( HANDLE hinst, HANDLE hinstPrev, LPSTR lpszCmd, in nCmdShow )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
int PASCAL WinMain( HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmd, int nCmdShow )
{
   return MyWinMain( hinst, hinstPrev, lpszCmd, nCmdShow ) ;

} /* WinMain */

#if 0
/****************************************************************
 *  LPSTR WINAPI GlobalAllocLock( DWORD cb ) ;
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
LPSTR WINAPI GlobalAllocLock( DWORD cb )
{
   HGLOBAL hg ;

   if (hg = GlobalAlloc( GHND, cb ))
      return GlobalLock( hg ) ;
   else
      return (LPSTR)NULL ;

} /* GlobalAllocLock()  */


VOID WINAPI GlobalUnlockFree( LPSTR lp ) 
{
   HGLOBAL hg = (HGLOBAL)LOWORD( GlobalHandle( HIWORD( lp ) ) ) ;

   GlobalUnlock( hg ) ;
   GlobalFree( hg ) ;
}
#endif
/************************************************************************
 * End of File: iniedit.c
 ***********************************************************************/

