/************************************************************************
 *
 *     Project:  XEDIT 2.0
 *
 *      Module:  XEDIT.c
 *
 *     Remarks:  Main module, contains WinMain and fnMainWnd
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "..\inc\XEDIT.h"
#include "..\inc\version.h"
#include "..\inc\config.h"
#include "..\inc\botstat.h"
#include "..\inc\toolbar.h"
#include "..\inc\init.h"
#include "..\inc\wndmgr.h"
#include "..\inc\menu.h"
#include "..\inc\cmd.h"
#include "..\inc\font.h"
#include "..\inc\dlghelp.h"
#include "..\inc\about.h"
#include "..\inc\subdlgs.h"
#include "..\inc\wintime.h"

/*
 * Global variables extern'd in XEDIT.h
 */
HWND        hwndApp ;
HWND        hwndTool ;
HWND        hwndStat ;
HWND        hwndEdit ;
HWND        hwndHex ;

HINSTANCE   hinstApp ;

BOOL        fWin31 ;

char        szVerNum[32] ;
char        szRegisteredName[MAX_REGNAME_LEN] ;

LPSTR       rglpsz[LAST_IDS - FIRST_IDS + 1] ;

/*
 * Globally managed lists (HLSQSTR.DLL)
 */
HFONT       hfontSmall ;
HFONT       hfontHex ;      // this is the one that is settable
char        szFont[LF_FACESIZE] ;
WORD        wFontStyle ;
WORD        wFontSize ;

/*
 * State variables
 */
BOOL        fSearchAll ;
BOOL        fToolBar ;
BOOL        fToolBarLabels ;
BOOL        fStatLine ;
short       xPos, yPos, nWidth, nHeight ;

BOOL        fDirty ;


/*
 * Currently active file variables
 */
char           szCurFile[256], szCurFileTitle[256] ;

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


      break ;

      case WM_SETFOCUS:

            if (hwndHex)
               SetFocus( hwndHex ) ;
            else
               return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;

      break ;


      case WM_DESTROY:
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
         if (hwndTool)
            SendMessage( hwndTool, wMsg, wParam, lParam ) ;
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

/************************************************************************
 * End of File: XEDIT.c
 ***********************************************************************/

