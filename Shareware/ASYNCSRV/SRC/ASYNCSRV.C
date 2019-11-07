/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  ASYNCSRV 
 *
 *      Module:  ASYNCSRV.c
 *
 *     Remarks:  Main module, contains WinMain and fnMainWnd
 *
 *   Revisions:  
 *       1/4/93   cek   Yanked from ConXSrv and munged big time to be
 *                      less specific.
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "version.h"

#include "ASYNCSRV.h"
#include "config.h"
#include "init.h"
#include "wndmgr.h"
#include "menu.h"
#include "cmd.h"
#include "subdlgs.h"
#include "botstat.h"
#include "connect.h"

/* Global variables extern'd in ASYNCSRV.h
 */
HWND        hwndApp ;
HWND        hwndTTY ;
HWND        hwndStat ;
HWND        hdlgActive ;

HINSTANCE   hinstApp ;

char        szVerNum[80] ;

BOOL        fInternal ;    // set if ASYNCSRV is being used internally and
                           // we are allowing the tty to be visible.

int         iCommDevice ;

/*
 * 
 */
HFONT       hfontSmall ;

HFONT       hfontCur ;      // this is the one that is settable
char        szFaceName[LF_FACESIZE] ;
char        szFaceStyle[LF_FACESIZE] ;
UINT        wFaceSize ;

/*
 * State variables
 */
BOOL        fStatLine ;
BOOL        fUserAbort ;

/*
 * Server structures
 */
COMMSETTINGS   CommSettings ;
CUSTINFO       CustInfo ;
HWND           hwndClientApp ;



/*
 * Variables local to this module.
 */ 
static HCURSOR    hcurWait ;
static HCURSOR    hcurNorm ;
static HCURSOR    hcurSave ;
static UINT       fWaitCursor ;


/****************************************************************
 *  LRESULT FAR PASCAL
 *    fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Main Window procedure.
 *
 *  Comments:
 *
 ****************************************************************/

LRESULT CALLBACK fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   HMENU    hMenu ;
   static   UINT wm_shutdown = NULL ;

   switch( uiMsg )
   {
      case WM_CREATE:

         hwndApp = hWnd ;

         /*
          * The client app broadcasts this message when it is closing
          * down.  When we get this message (see default: case below)
          * we do a WM_CLOSE.
          */
         wm_shutdown = RegisterWindowMessage( rglpsz[IDS_APPNAME] ) ;


         /*
          * Create status bar
          */
         if (hwndStat = CreateStatus( hWnd ))
         {
            #if 0
            SetWindowText( hwndStat, rglpsz[IDS_STAT_READY] ) ;
            #endif
            if (fStatLine)
               ShowWindow( hwndStat, SW_NORMAL ) ;
         }
         else
            fStatLine = FALSE ;

         /*
          * Create TTY window.  We do all the painting
          * in this guy.
          */
         if (!(hwndTTY = CreateWindow(
                                       SZ_TTYCLASSNAME,
                                       "",
                                       WS_CHILD |
                                       WS_CLIPSIBLINGS |
                                       WS_VISIBLE,
                                       0,
                                       0,
                                       1,
                                       1,
                                       hWnd,
                                       (HMENU)IDCHILD_TTY,
                                       hinstApp,
                                       NULL )))
         {
            DP1( hWDB, "TTY create failed!" ) ;
            return -1 ;
         }

         hMenu = GetMenu( hWnd ) ;
         StatusSetStatBox( hwndStat, IDSTAT_CONNECT, rglpsz[IDS_NOTCONNECT] ) ;
         StatusSetStatBox( hwndStat, IDSTAT_DDE, rglpsz[IDS_DDE_NOTCONNECT] ) ;
      break ;

      case WM_SETFOCUS:
         SetFocus( hwndTTY ) ;
      break ;

      case WM_DESTROY:
         /*
          * Destroy status bar
          */
         if (hwndStat)
         {
            DestroyWindow( hwndStat ) ;
            hwndStat = NULL ;
         }

         /*
          * We already are assured of being SW_RESORE so just
          * get the coords for the save (SaveDefaults)
          */
         PostQuitMessage( 0 ) ;

      break ;

      case WM_CLOSE :
         SaveWndPos( hWnd ) ;

         CloseConnection() ;

         DestroyWindow( hWnd ) ;
      break ;

      case WM_GETMINMAXINFO:
      case WM_SIZE:
         DP5( hWDB, "WM_SIZE/WM_GETMINMAXINFO" );
         return SizeHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_COMMAND:
         DP5( hWDB, "WM_COMMAND" );
         return CmdHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_SYSCOMMAND:
         DP5( hWDB, "WM_SYSCOMMAND" );
         return SysCmdHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_INITMENU:
      case WM_MENUSELECT:
      case WM_INITMENUPOPUP:
         return MenuHandler( hWnd, uiMsg, wParam, lParam ) ;

      case WM_COMMNOTIFY:
         return SendMessage( hwndTTY, uiMsg, wParam, lParam ) ;

      default:
         if (uiMsg == wm_shutdown)
         {
            /*
             * The client app broadcasts this message when
             * it wants us to shut down.
             */
            SendMessage( hwndApp, WM_CLOSE, 0, 0L ) ;
         }
         else
            return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
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
UINT nCurCount ;

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
//      if (!TranslateAccelerator( hwndApp, haccl, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }

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


/************************************************************************
 * End of File: ASYNCSRV.c
 ***********************************************************************/

