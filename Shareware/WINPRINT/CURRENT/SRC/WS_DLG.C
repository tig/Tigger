/************************************************************************
 *
 *     Project:  WinPrint 1.x
 *
 *      Module:  ws_dlg.c
 *
 *     Remarks:  Main window procedure
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "isz.h"
#include "dlgs.h"
#include "winprint.h"
#include "ws_init.h"
#include "ws_dlg.h"
#include "ws_lib.h"
#include "ws_about.h"
#include "ws_files.h"
#include "ws_print.h"
#include "ws_save.h"
#include "ws_open.h"
#include "ws_reg.h"
#include "text.h"
#include "headfoot.h"
#include "dlghelp.h"
#include "wintime.h"
#include "page.h"
#include "selprn.h"
#include "fontutil.h"
#include "dragdrop.h"

VOID NEAR PASCAL HandleFileOps( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL NEAR PASCAL UpdateCurrentInfo (HWND hWnd) ;

static LPSTR      astrchr (LPSTR , int) ;
static LPSTR      astrrchr (LPSTR , int) ;

static HWND hwndSel,
            hwndHid,
            hwndChoose ;

static char szFileName [_MAX_PATH + _MAX_FNAME + _MAX_EXT], 
            szFileSpec [_MAX_FNAME + _MAX_EXT] = "*.*" ;

static UINT   nIndex ;    
static int nMinX, nMinY ;

/****************************************************************
 *  Description: 
 *
 *    Main window procedure.  This window is a private class
 *    modless dialog box.
 *
 *  Comments:
 *
 ****************************************************************/
LPARAM CALLBACK fnMain( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   BOOL  fShiftDown ;

   switch (uiMsg)
   {
      case WM_CREATE :
         DP4( hWDB, "WM_CREATE\n") ;

         hwndMain = hDlg ;
         
         PlaceWindow( hDlg ) ;

         /*
          * Load icons
          */
         hicoNoOrient = LoadIcon( hInst, MAKEINTRESOURCE( IDI_NOORIENT ) ) ;
         hicoPortrait = LoadIcon( hInst, MAKEINTRESOURCE( IDI_PORTRAIT ) ) ;
         hicoLandscape = LoadIcon( hInst, MAKEINTRESOURCE( IDI_LANDSCAPE ) ) ;

         break ;

      case WM_MYINIT:
      {
         RECT  rc ;
         char  szText[128] ; /* maximum lenght of a command line */

         DP4( hWDB, "WM_MYINIT\n") ;

         MyDragAcceptFiles( hDlg, TRUE ) ;

         hwndSel = GetDlgItem (hDlg, IDD_SELECTLIST) ;
         hwndHid = GetDlgItem (hDlg, IDD_HIDDENLIST) ;
         hwndChoose = GetDlgItem (hDlg, IDD_CHOOSELIST) ;

         /*
         GetWindowRect( hwndChoose, &rc ) ;
         SendMessage( hwndChoose, LB_SETCOLUMNWIDTH,
                     (rc.right - rc.left) / 2, 0L ) ;
         */

         /*
          * set the font of some controls
          */
         SendMessage( hwndSel, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         SendMessage( hwndChoose, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         SendDlgItemMessage( hDlg, IDD_FNAME, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;

         //if (!fWin31)
         {
            /*
             * HACK! For Windows 3.0 which does not allow Edit controls
             * as the static path control for DlgDirList()
             */
            RECT  rc ;

            GetWindowRect( GetDlgItem( hDlg, IDD_FPATH ), &rc ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc.left ) ;
            ScreenToClient( hDlg, (LPPOINT)&rc.right ) ;
            DestroyWindow( GetDlgItem( hDlg, IDD_FPATH ) ) ;
            CreateWindow( "static",
                           "",
                           SS_LEFT | WS_CHILD,
                           rc.left,
                           rc.top,
                           rc.right - rc.left,
                           rc.bottom - rc.top,
                           hDlg,
                           (HMENU)IDD_FPATH,
                           hInst,
                           NULL ) ;

         }

         SendDlgItemMessage( hDlg, IDD_FPATH, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         ShowWindow( GetDlgItem( hDlg, IDD_FPATH ), SW_NORMAL ) ;

         SendDlgItemMessage( hDlg, IDD_TEXTFONT, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         SendDlgItemMessage( hDlg, IDD_HEADERFONT, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         SendDlgItemMessage( hDlg, IDD_PRINTER, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         SendDlgItemMessage( hDlg, IDD_CHARSPERLINE, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;
         SendDlgItemMessage( hDlg, IDD_COLUMNS, WM_SETFONT, (WPARAM)hfontSmall, 0L ) ;

         fGo = FALSE ;
         GetPrivateProfileString( GRCS(IDS_INI_MAIN),
                                  GRCS(IDS_INI_MAIN_CURCFG),
                                  GRCS(IDS_FACTORYDEFAULTS),
                                  szCurrentConfig, CFG_NAME_LEN + 1,
                                  GRCS(IDS_INI_FILENAME) ) ;

         DP1( hWDB, "Using config \"%s\"", (LPSTR)szCurrentConfig ) ;

         /*
          * Read command line, setting values in CmdLineOpt.  Set
          * flags for each member indicating whether that member
          * was set on the cmd line or not
          */
         bConfigOnCmdLine = FALSE ;
         if (!DoCmdLine( hDlg, szText ))
         {
            ErrorResBox( NULL, hInst,
                         MB_ICONEXCLAMATION,IDS_APPNAME, 
                         IDS_ERR_COMMANDLINE, (LPSTR)szText) ;
            fGo = FALSE ;
         }

         /*
          * Read the current config in.  If a config was specified
          * on the command line, then szCurrentConfig is it.
          */
         if (!ReadDefaults (szCurrentConfig))
         {
            ErrorResBox( NULL, hInst,
                           MB_ICONEXCLAMATION, IDS_APPNAME, 
                           IDS_ERR_CONFIGNAME, (LPSTR)szCurrentConfig ) ;

            lstrcpy (szCurrentConfig, GRCS(IDS_FACTORYDEFAULTS)) ;
            ReadDefaults (szCurrentConfig) ;
            fGo = FALSE ;
         }

         /*
          * Now 'Options' contains the stuff set in the config and
          * 'CmdLineOptions' contains the stuff that was set on the
          * command line.  Merge them together.
          */
         MergeCmdLine() ;

         SendDlgItemMessage (hDlg, IDD_FNAME, EM_LIMITTEXT, sizeof (szFileName) - 1, 0L) ;
         SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;

         DlgDirList( hDlg, szFileSpec, IDD_CHOOSELIST, IDD_FPATH, 0x4010 ) ;

         EnableWindow (GetDlgItem (hDlg, IDD_REMOVE), FALSE) ;

         EnableWindow( GetDlgItem( hDlg, IDD_PRINT ),
            (BOOL)(nIndex = (UINT)SendMessage( hwndSel,
                                              LB_GETCOUNT, 0, 0L )) ) ;
         UpdateCurrentInfo (hDlg) ;

         GetWindowRect( hDlg, &rc ) ;
         nMinX = rc.right - rc.left ;
         nMinY = rc.bottom - rc.top ;


         if (!CheckRegistration (hInst))
         {
            DoAboutBox() ;
            DoSharewareBox() ;
         }

         /* Do 3D stuff */
         tdShadowControl( hDlg, WM_INITDIALOG, NULL, RGBLTGRAY ) ;
      }
      break ;

      case WM_ERASEBKGND:
      {
         DWORD    dw ;
         #ifdef WIN32
         HBRUSH   hbr = (HBRUSH)SetClassLong( hDlg, GCL_HBRBACKGROUND,
                                      (LONG)CreateSolidBrush( RGBLTGRAY ) ) ;
         dw = DefWindowProc( hDlg, uiMsg, wParam, lParam ) ;

         DeleteObject( (HBRUSH)SetClassLong( hDlg, GCL_HBRBACKGROUND,
                                     (LONG)hbr ) ) ;

         #else
         HBRUSH   hbr = (HBRUSH)SetClassWord( hDlg, GCW_HBRBACKGROUND,
                                      (WORD)CreateSolidBrush( RGBLTGRAY ) ) ;
         dw = DefWindowProc( hDlg, uiMsg, wParam, lParam ) ;

         DeleteObject( (HBRUSH)SetClassWord( hDlg, GCW_HBRBACKGROUND,
                                     (WORD)hbr ) ) ;
         #endif
         return dw ;
      }

      case WM_GO:
         DP1( hWDB, "WM_GO, fGo = %d", fGo ) ;
         if (fGo && (BOOL)SendMessage( hwndSel, LB_GETCOUNT, 0, 0L ))
         {
            DP1( hWDB, "Going!" ) ;
            SendMessage (hDlg, WM_COMMAND, IDD_PRINT, 0L) ;
         }
         else
         {
            DP1( hWDB, "No items to go on" ) ;
         }
         break ;

      case WM_SETFOCUS:
         SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
         break ;

      case WM_LBUTTONDBLCLK:
      {
         RECT    rcWnd ;
         RECT    rcLbl ;
         POINT   pt ;

         #ifdef WIN32

         pt.x = (LONG)LOWORD( lParam ) ;
         pt.y = (LONG)HIWORD( lParam ) ;

         #else
         pt = MAKEPOINT( lParam );
         #endif

         fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;

         ClientToScreen( hDlg, &pt );

         /*
          * Is it the landscape/portrait icon?
          */
         GetWindowRect( GetDlgItem( hDlg, IDD_WINPRINT_ICO ), &rcWnd );
         GetWindowRect( GetDlgItem( hDlg, IDD_ORIENT_LBL ), &rcLbl );

         if ( PtInRect( &rcWnd, pt ) || PtInRect( &rcLbl, pt ) )
         {
            if (ghDevMode)
            {
               LPDEVMODE lpdm ;

               lpdm = (LPDEVMODE)GlobalLock( ghDevMode ) ;

               if (lpdm->dmFields & DM_ORIENTATION)
               {
                  lpdm->dmOrientation =
                     (lpdm->dmOrientation == DMORIENT_PORTRAIT) ?
                        DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT ;

                  SetDlgIcon( hDlg, IDD_WINPRINT_ICO,
                     (lpdm->dmOrientation == DMORIENT_LANDSCAPE) ?
                        hicoLandscape : hicoPortrait ) ;
             
                  if (!fShiftDown)
                     bModify = TRUE ;
   
               }
               GlobalUnlock( ghDevMode ) ;
            }

            UpdateCurrentInfo (hDlg) ;
            break ;
         }

         /*
          * Is it the text font box?
          */
         GetWindowRect( GetDlgItem( hDlg, IDD_TEXTFONT), &rcWnd );
         GetWindowRect( GetDlgItem( hDlg, IDD_TEXTFONT_LBL ), &rcLbl );
         if ( PtInRect( &rcWnd, pt ) || PtInRect( &rcLbl, pt ) )
         {
            SendMessage( hDlg, WM_COMMAND, IDM_OPTIONS, 0L ) ;
            break ;
         }

         /*
          * Is it the header font box?
          */
         GetWindowRect( GetDlgItem( hDlg, IDD_HEADERFONT), &rcWnd );
         GetWindowRect( GetDlgItem( hDlg, IDD_HEADERFONT_LBL ), &rcLbl );
         if ( PtInRect( &rcWnd, pt ) || PtInRect( &rcLbl, pt ) )
         {
            SendMessage( hDlg, WM_COMMAND, IDM_HEADFOOT, 0L ) ;
            break ;
         }


         /*
          * Is it printer box?
          */
         GetWindowRect( GetDlgItem( hDlg, IDD_PRINTER), &rcWnd );
         GetWindowRect( GetDlgItem( hDlg, IDD_PRINTER_LBL ), &rcLbl );
         if ( PtInRect( &rcWnd, pt ) || PtInRect( &rcLbl, pt ) )
         {
            SendMessage( hDlg, WM_COMMAND, IDM_PRINTER, 0L ) ;
            break ;
         }

         /*
          * Is it the other info box?  Or the column stuff?
          */
         GetWindowRect( GetDlgItem( hDlg, IDD_CHARSPERLINE), &rcWnd );
         GetWindowRect( GetDlgItem( hDlg, IDD_OTHERINFO_LBL ), &rcLbl );
         if ( PtInRect( &rcWnd, pt ) || PtInRect( &rcLbl, pt ) )
         {
            SendMessage( hDlg, WM_COMMAND, IDM_PAGELAYOUT, 0L ) ;
            break ;
         }
         GetWindowRect( GetDlgItem( hDlg, IDD_COLUMNS), &rcWnd );
         GetWindowRect( GetDlgItem( hDlg, IDD_COLUMNS_LBL ), &rcLbl );
         if ( PtInRect( &rcWnd, pt ) || PtInRect( &rcLbl, pt ) )
         {
            SendMessage( hDlg, WM_COMMAND, IDM_PAGELAYOUT, 0L ) ;
            break ;
         }

      }
      break ;

      case WM_DROPFILES:
      {
         char  szText[_MAX_PATH] ;
         UINT  cFiles ;
         UINT  a ;

         cFiles = MyDragQueryFile( (HANDLE)wParam, 0xFFFF, NULL, NULL ) ;
         for (a = 0 ; a <cFiles ; a++)
         {
            MyDragQueryFile( (HANDLE)wParam, a, szText, sizeof(szText) ) ;

            if (!fPrinting)
               EnableWindow( GetDlgItem( hDlg, IDD_PRINT ), TRUE ) ;

            SendMessage( hwndHid, LB_ADDSTRING, 0, (LONG)(LPSTR)szText ) ;
            ShortenPath( szText, szText, 32) ;
            SendMessage( hwndSel, LB_ADDSTRING, 0, (LONG)(LPSTR)szText ) ;
         }
         MyDragFinish( (HANDLE)wParam ) ;

         /*
          * if we're iconic then start printing right away
          */
         if (!fPrinting && IsIconic( hDlg ))
            SendMessage (hDlg, WM_COMMAND, IDD_PRINT, 0L) ;
      }
      break ;

      case WM_DEVMODECHANGE:
      {
         if (lParam)
         {
            DEVNAMES FAR *    lpdn ;

            if (ghDevNames &&
                (lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames )))
            {
               if (!lstrcmpi( (LPSTR)lParam,
                              (LPSTR)lpdn+(lpdn->wDeviceOffset) ))
               {
                  bModify = TRUE ;
                  UpdateCurrentInfo( hDlg ) ;
               }

               GlobalUnlock( ghDevNames ) ;
            }
         }
      }
      break ;

      case WM_COMMAND:
      {
         #ifdef WIN32
         WORD wNotifyCode = HIWORD(wParam);  
         WORD wID = LOWORD(wParam);         
         HWND hwndCtl = (HWND) lParam;      
         #else
         WORD wNotifyCode = HIWORD(lParam) ;
         WORD wID = wParam ;
         HWND hwndCtl = (HWND)LOWORD(lParam) ;
         #endif

         fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;

         switch ( wID )
         {
            case IDM_ABOUT:
               DoAboutBox() ;
            break ;

            case IDM_OPEN:
               if (bModify)
                  switch( ErrorResBox( NULL, hInst,
                                       MB_YESNOCANCEL | MB_ICONQUESTION,
                                       IDS_APPNAME, IDS_ERR_SAVECHANGES ))
                  {
                     case IDCANCEL :
                        return 0L ;
                        break ;

                     case IDYES :
                        if (!DoSaveConfig())
                           return 0L ;
                  }

               if (DoOpenConfig())
               {
                  bModify = FALSE ;
                  UpdateCurrentInfo( hDlg ) ;
               }
            break ;

            case IDM_SAVE:
               if (DoSaveConfig())
               {
                  bModify = FALSE ;
                  UpdateCurrentInfo( hDlg ) ;
               }
            break ;

            case IDM_OPTIONS:
               if (DoTextOptions())
               {
                  if (!fShiftDown)
                     bModify = TRUE ;
                  UpdateCurrentInfo (hDlg) ;
               }
            break ;

            case IDM_HEADFOOT:
               if (DoHeadFootOptions())
               {
                  if (!fShiftDown)
                     bModify = TRUE ;
                  UpdateCurrentInfo( hDlg ) ;
               }
            break ;

            case IDM_PAGELAYOUT:
               if (DoPageLayout())
               {
                  if (!fShiftDown)
                     bModify = TRUE ;
                  UpdateCurrentInfo (hDlg) ;
               }
            break ;

            case IDM_PRINTER:
               if (DoPrinterSetup())
               {
                  if (!fShiftDown)
                     bModify = TRUE ;
                  UpdateCurrentInfo( hDlg ) ;
               }
            break ;

            case IDM_SEPARATEJOBS:
               if (!fShiftDown)
                  bModify = TRUE ;  
               Options.bSeparateJobs = !Options.bSeparateJobs ;
               UpdateCurrentInfo( hDlg ) ;
            break ;

            case IDM_MINIMIZEONPRINT:
               if (!fShiftDown)
                  bModify = TRUE ;  
               Options.bMinimize = !Options.bMinimize ;
               UpdateCurrentInfo( hDlg ) ;
            break ;

            case IDM_CLOSEWHENDONE:
               Options.bClose = !Options.bClose ;
               UpdateCurrentInfo (hDlg) ;
            break ;

            case IDM_HELP:
               WinHelp( hDlg, GRCS(IDS_HELPFILE), HELP_CONTENTS, 0L ) ;
            break ;

            case IDM_HELPONHELP:
               WinHelp( hDlg, GRCS(IDS_HELPFILE), HELP_HELPONHELP, 0L ) ;
            break ;

            case IDM_EXIT:
               SendMessage (hDlg, WM_CLOSE, 0, 0L) ;
            break ;

            #ifdef DEBUG
            case IDM_WDB_CONFIG:
               wdbConfigDlg( hWDB ) ;
            break ;
            #endif

            default:
               HandleFileOps( hDlg, uiMsg, wParam, lParam ) ;

         }
      }
      break ;

      /*
       * If we are printing and minimized we've added a "Cancel" item
       * to the system menu.  Process that here.
       */
      case WM_SYSCOMMAND:
         if (fPrinting)
            switch( LOWORD( wParam ) )
            {
               case IDM_CANCEL:
                  DP4( hWDB, "    IDM_CANCEL" ) ;
                  AbortPrintJob() ; /* in ws_print.c */
               return 0L ;

               case SC_RESTORE:
                  DP4( hWDB, "    SC_RESTORE" ) ;
               return 0L ;

               case SC_CLOSE:
                  DP4( hWDB, "    SC_RESTORE" ) ;
               return 0L ;

            }
      return DefDlgProc( hDlg, uiMsg, wParam, lParam ) ;

      case WM_QUERYOPEN:
      return !fPrinting ;

      case WM_INITMENU:
         if (fPrinting && IsIconic( hDlg ))
         {
            DP4( hWDB, "   It's the sysmenu" ) ;
            EnableMenuItem( (HMENU)wParam, SC_CLOSE, MF_GRAYED) ;
            EnableMenuItem( (HMENU)wParam, SC_RESTORE, MF_GRAYED) ;
         }
      return DefDlgProc( hDlg, uiMsg, wParam, lParam ) ;
 
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

      case WM_CLOSE :
         fShiftDown = (GetKeyState( VK_SHIFT ) & 0x8000) ;
         if (!fShiftDown && bModify && !fGo)
            switch( ErrorResBox( NULL, hInst,
                                 MB_YESNOCANCEL | MB_ICONQUESTION,
                                 IDS_APPNAME, IDS_ERR_SAVECHANGES ))
            {
               case IDCANCEL :
                  return 0L ;
                  break ;

               case IDYES :
                  if (!DoSaveConfig())
                     return 0L ;

               case IDNO :
                  return DefWindowProc (hDlg, uiMsg, wParam, lParam) ;
                  break ;

               default:
                  MessageBeep (0) ;
                  return 0L ;
                  break ;
            }
         return DefWindowProc (hDlg, uiMsg, wParam, lParam) ;
      break ;

      case WM_DESTROY:

         DP3( hWDB, "WM_DESTROY..." ) ;
         /*
          * Save the window position
          */
         SaveWindowPos( hDlg ) ;

         tdShadowControl( hDlg, uiMsg, wParam, lParam ) ;

         WinHelp( hDlg, "WINPRINT.HLP", HELP_QUIT, 0L ) ;

         MyDragUnload( ) ;

         PostQuitMessage (0) ;
         break ;

      default:
      {
         DWORD dw ;

         if (dw = tdShadowControl( hDlg, uiMsg, wParam, lParam ))
            return dw ;

         return DefDlgProc (hDlg, uiMsg, wParam, lParam) ;
      }
   }

   return 0L ;

} /* fnMain()  */

/****************************************************************
 *  VOID NEAR PASCAL HandleFileOps( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Hanldes all file operation controls.
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL HandleFileOps( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   char     cLastChar ;
   int    nEditLen ;
   OFSTRUCT of ;

   #ifdef WIN32
   WORD wNotifyCode = HIWORD(wParam);  
   WORD wID = LOWORD(wParam);         
   HWND hwndCtl = (HWND) lParam;      
   #else
   WORD wNotifyCode = HIWORD(lParam) ;
   WORD wID = wParam ;
   HWND hwndCtl = (HWND)LOWORD(lParam) ;
   #endif

   switch (wID)
   {
      case IDD_CHOOSELIST:
         switch (wNotifyCode)
         {
            case LBN_SELCHANGE:
               #ifdef WIN32
               if (DlgDirSelectEx( hDlg, szFileName, lstrlen(szFileName), IDD_CHOOSELIST))
               #else
               if (DlgDirSelect (hDlg, szFileName, IDD_CHOOSELIST))
               #endif
                  lstrcat (szFileName, szFileSpec) ;

               SetDlgItemText (hDlg, IDD_FNAME, szFileName) ;
               break ;

            case LBN_DBLCLK:
               #ifdef WIN32
               if (DlgDirSelectEx( hDlg, szFileName, lstrlen(szFileName), IDD_CHOOSELIST))
               #else
               if (DlgDirSelect (hDlg, szFileName, IDD_CHOOSELIST))
               #endif
               {
                  DP4( hWDB, "DlgDirSelect succeeded" ) ;
                  lstrcat (szFileName, szFileSpec) ;
                  DlgDirList (hDlg, szFileName, IDD_CHOOSELIST, IDD_FPATH, 0x4010) ;
                  SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
               }
               else
               {
                  DP4( hWDB, "DlgDirSelect failed" ) ;
                  if (!(strchr (szFileName, '*') || strchr (szFileName, '?'))) 
                  {
                     DP4( hWDB, "Fname = %s", (LPSTR)szFileName ) ;
                     SetDlgItemText (hDlg, IDD_FNAME, szFileName) ;
                     SendMessage( hDlg, WM_COMMAND,
                                    IDD_SELECT,
                                    MAKELONG( hDlg, BN_CLICKED ) ) ;
                  }
                  else
                     SendMessage( hDlg, WM_COMMAND, IDOK, 
                                    MAKELONG( hDlg, BN_CLICKED ) ) ;
               }
               break ;
         }
         GetDlgItemText (hDlg, IDD_FNAME, szFileName, sizeof (szFileName) - 1) ;
         EnableWindow (GetDlgItem (hDlg, IDOK),
                        (strchr (szFileName, '*') || strchr(szFileName, '?'))) ;
         break ;

      case IDD_SELECTLIST:
         if (wNotifyCode == LBN_SELCHANGE)
            EnableWindow (GetDlgItem (hDlg, IDD_REMOVE),
               LB_ERR != (UINT) SendMessage (hwndSel, LB_GETCURSEL, 0, 0L)) ;
         break ;

      case IDD_FNAME:
         GetDlgItemText (hDlg, IDD_FNAME, szFileName, sizeof (szFileName) - 1) ;
         if (strchr (szFileName, '*') || strchr(szFileName, '?'))
            SendMessage (hwndChoose, LB_SETCURSEL, (WPARAM)(UINT)-1, 0L) ;
         if (wNotifyCode == EN_CHANGE)
         {
            EnableWindow (GetDlgItem (hDlg, IDOK),
               (BOOL) SendMessage (hwndCtl,  WM_GETTEXTLENGTH, 0, 0L)) ;
            EnableWindow (GetDlgItem (hDlg, IDD_SELECT),
               (BOOL) SendMessage (hwndCtl,  WM_GETTEXTLENGTH, 0, 0L)) ;
         }
         break ;

      case IDOK:
         GetDlgItemText (hDlg, IDD_FNAME, szFileName, sizeof (szFileName) - 1) ;
         nEditLen  = lstrlen (szFileName) ;
         cLastChar = *AnsiPrev (szFileName, szFileName + nEditLen) ;
         if (cLastChar == '\\' || cLastChar == ':')
            lstrcat (szFileName, szFileSpec) ;
         if (cLastChar == '\\' || astrchr (szFileName, ':') || astrchr(szFileName, '*') || astrchr(szFileName, '?'))
         {
            if (DlgDirList (hDlg, szFileName, IDD_CHOOSELIST, IDD_FPATH, 0x4010))
            {
               lstrcpy (szFileSpec, szFileName) ;
               SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
            }
            else
            {
               SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
               MessageBeep (0) ;
            }
            break ;
         }
         lstrcat (lstrcat (szFileName, "\\"), szFileSpec) ;
         if (DlgDirList (hDlg, szFileName, IDD_CHOOSELIST, IDD_FPATH, 0x4010))
         {
            lstrcpy (szFileSpec, szFileName) ;
            SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
            break ;
         }
         szFileName [nEditLen] = '\0' ;
         if (-1 == OpenFile (szFileName, &of, OF_READ | OF_EXIST))
         {
            lstrcat (szFileName, szFileSpec) ;
            if (-1 == OpenFile (szFileName, &of, OF_READ | OF_EXIST))
            {
               SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
               MessageBeep (0) ;
               break ;
            }
         }
         break ;

      case IDD_SELECT:
         DP4( hWDB, "IDD_SELECT" ) ;
         GetDlgItemText (hDlg, IDD_FNAME, szFileName, sizeof (szFileName) - 1) ;
         SendMessage (hwndSel, WM_SETREDRAW, FALSE, 0L) ;
         if (!SelectFiles (szFileName))
         {
            SetFocus (GetDlgItem (hDlg, IDD_FNAME)) ;
            MessageBeep (0) ;
         }
         EnableWindow (GetDlgItem (hDlg, IDD_REMOVE),
            LB_ERR != (UINT) SendMessage (hwndSel, LB_GETCURSEL, 0, 0L)) ;
         EnableWindow (GetDlgItem (hDlg, IDD_PRINT),
            (BOOL) SendMessage (hwndSel, LB_GETCOUNT, 0, 0L)) ;
         SendMessage (hwndSel, WM_SETREDRAW, TRUE, 0L) ;
         InvalidateRect (hwndSel, NULL, TRUE) ;
         UpdateWindow (hwndSel) ;
         break ;

      case IDD_PRINT:
         if (Options.bMinimize)
            ShowWindow (hDlg, SW_MINIMIZE) ;

         StartPrintUI() ;

         if (Options.bSeparateJobs)
         {
            int n ;
            char  szFile[_MAX_PATH] ;

            DP3( hWDB, "Printing separate jobs" ) ;
            /*
             *  Go through listbox.
             */

            while ((int)SendMessage( hwndHid, LB_GETCOUNT, 0, 0L))
            {
               SendMessage( hwndSel, LB_SETCURSEL, 0, 0L ) ;          
               SendMessage( hwndHid, LB_GETTEXT, 0,
                            (LONG)(LPSTR)szFile ) ;

               if (n = PrintFiles( hDlg, NULL, NULL, szFile ))
               {
                  DP1( hWDB, "PrintFiles returned %d", n ) ;

                  EndPrintUI() ;
                  break ;
               }

               SendMessage( hwndSel, LB_DELETESTRING, 0, 0L ) ;
               SendMessage( hwndHid, LB_DELETESTRING, 0, 0L ) ;
            }

         }
         else
            if (!PrintFiles (hDlg, hwndSel, hwndHid, NULL ))
            {
               DP4( hWDB, "Back from PrintFiles (!)" ) ;
               SendMessage(hwndSel, LB_RESETCONTENT, 0, 0L) ;
               SendMessage(hwndHid, LB_RESETCONTENT, 0, 0L) ;
            }

         if (!SendMessage( hwndHid, LB_GETCOUNT, 0, 0L ))
         {
            EnableWindow(GetDlgItem (hDlg, IDD_REMOVE), FALSE) ;
            EnableWindow(GetDlgItem (hDlg, IDD_PRINT), FALSE) ;
         }

         EndPrintUI() ;
         DP4( hWDB, "Done Printing" ) ;

         if (Options.bClose)
            SendMessage (hDlg, WM_CLOSE, 0, 0L) ;

      break ;

      case IDD_REMOVE:
      {
         int i ;
         if (LB_ERR !=
               (nIndex = (UINT)SendMessage( hwndSel, LB_GETSELCOUNT,
                                             0, 0L )))
         {

            SendMessage( hwndSel, WM_SETREDRAW, FALSE, 0L ) ;

            for (i = (int)SendMessage( hwndSel, LB_GETCOUNT, 0, 0L ) - 1 ;
                  i >= 0 ;
                  i--)
            {
               if ((int)SendMessage( hwndSel, LB_GETSEL, i, 0L ) > 0)
               {
                  SendMessage( hwndSel, LB_DELETESTRING, i, 0L ) ;
                  SendMessage( hwndHid, LB_DELETESTRING, i, 0L ) ;

                  /* nIndex will be the previous guy in the list */
                  nIndex = i ;
               }
            }
         }

         DP( hWDB, "nIndex = %d" , nIndex ) ;

         i = (int)SendMessage( hwndSel, LB_GETCOUNT, 0, 0L ) ;

         if (nIndex && (int)nIndex >= i)
            (int)nIndex-- ;

         SendMessage( hwndSel, LB_SELITEMRANGE, TRUE,
               MAKELONG( nIndex, nIndex) ) ;

         EnableWindow( GetDlgItem (hDlg, IDD_PRINT ), (BOOL)i ) ;

         EnableWindow( GetDlgItem (hDlg, IDD_REMOVE ), 
            (BOOL)SendMessage( hwndSel, LB_GETSELCOUNT, 0, 0L ) ) ;

         SendMessage( hwndSel, WM_SETREDRAW, TRUE, 0L ) ;
         InvalidateRect( hwndSel, NULL, TRUE ) ;

      }
      break ;
   }
} /* HandleFileOps()  */


/****************************************************************
 *  BOOL NEAR PASCAL UpdateCurrentInfo( HWND hDlg )
 *
 *  Description: 
 *
 *    Updates the status bar on the bottom of the main window.
 *    Also calculates a bunch of global variables.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL UpdateCurrentInfo( HWND hDlg )
{
   HDC            hdc = NULL ;
   char           szText[128] ;

   HFONT          hFont ;
   PAGE           Page ;
   PRINTDLG       pd ;
   DEVNAMES FAR * lpdn ;
   DEVMODE FAR  * lpdm ;

   CheckMenuItem( GetMenu( hwndMain ),
                  IDM_MINIMIZEONPRINT,
                  MF_BYCOMMAND |
                  Options.bMinimize ? MF_CHECKED : MF_UNCHECKED ) ;

   CheckMenuItem( GetMenu( hwndMain ),
                  IDM_CLOSEWHENDONE,
                  MF_BYCOMMAND |
                  Options.bClose ? MF_CHECKED : MF_UNCHECKED ) ;

   CheckMenuItem( GetMenu( hwndMain ),
                  IDM_SEPARATEJOBS,
                  MF_BYCOMMAND |
                  Options.bSeparateJobs ? MF_CHECKED : MF_UNCHECKED ) ;

   memset( &pd, '\0', sizeof( PRINTDLG ) ) ;
   pd.lStructSize = sizeof( PRINTDLG ) ;
   pd.hwndOwner = hDlg ;
   pd.hDevMode = ghDevMode ;
   pd.hDevNames = ghDevNames ;

   if (!ghDevNames)
   {
      pd.Flags = PD_RETURNIC | PD_RETURNDEFAULT ;

      if (!PrintDlg( &pd ))
      {
        /*
         * Set our internal vars to match those returned...
         */
         ghDevMode = pd.hDevMode ;
         ghDevNames = pd.hDevNames ;

         DP1( hWDB, "UpdateCurrentInfo : PrintDlg failed!" ) ;
         return FALSE ;
      }

      hdc = pd.hDC ;

        /*
         * Set our internal vars to match those returned...
         */
      ghDevMode = pd.hDevMode ;
      ghDevNames = pd.hDevNames ;

   }

   DASSERT( hWDB, ghDevNames ) ;

   lpdn = (DEVNAMES FAR *)GlobalLock( ghDevNames ) ;
   lpdm = (DEVMODE FAR *)GlobalLock( ghDevMode ) ;

   DP4( hWDB, "lpdn = %08lX, lpdm = %08lX, hdc = %04X", lpdn, lpdm, hdc ) ;
   if (lpdn && !hdc)
   {
      DP4( hWDB, "UpdateCurrentInfo: CreateDC()..." ) ;
      DP4( hWDB, "   Driver = %s", (LPSTR)lpdn+(lpdn->wDriverOffset) ) ;
      DP4( hWDB, "   Device = %s", (LPSTR)lpdn+(lpdn->wDeviceOffset) ) ;
      DP4( hWDB, "   Output = %s", (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;

      hdc = CreateDC( (LPSTR)lpdn+(lpdn->wDriverOffset),
                            (LPSTR)lpdn+(lpdn->wDeviceOffset),
                            (LPSTR)lpdn+(lpdn->wOutputOffset),
                            (LPVOID)lpdm ) ;
   }

   /*
    * Do any special casing we need for special printers here.  We have
    * a global variable wPrinterBug.
    */
   wPrinterBugs = BUG_NOBUGS ;

   DASSERT( hWDB, hdc ) ;

   if (!hdc)
   {
      if (lpdn)
         GlobalUnlock( ghDevNames ) ;
      if (lpdm)
         GlobalUnlock( ghDevMode ) ;
      return FALSE ;
   }

   /*
    * Hack!!  If we ever come in here with no facename it is probably
    * because the device has no device fonts (i.e. Epson in landscape
    * mode).  So hack in Courier.
    */
   if (!lstrlen( Options.lfCurFont.lfFaceName ))
      lstrcpy( Options.lfCurFont.lfFaceName, GRCS(IDS_DEFAULTFONT) ) ;

   if (!lstrlen( Options.lfHFFont.lfFaceName ))
      lstrcpy( Options.lfHFFont.lfFaceName, GRCS(IDS_DEFAULTFONT) ) ;

   hFont = CreateFontIndirect( &Options.lfCurFont ) ;
   hFont = SelectObject( hdc, hFont ) ;
   GetPageInfo( hdc, &Page ) ;
   hFont = SelectObject( hdc, hFont ) ;
   DeleteObject( hFont ) ;

   nLogicalPixelsY = GetDeviceCaps( hdc, LOGPIXELSY ) ;

   wsprintf( szText, " %s, %dpt", (LPSTR)Options.lfCurFont.lfFaceName,
                      MulDiv( -Options.lfCurFont.lfHeight, 72,
                              nLogicalPixelsY ) ) ;
   SetDlgItemText( hDlg, IDD_TEXTFONT, szText ) ;

   wsprintf( szText, " %s, %dpt", (LPSTR)Options.lfHFFont.lfFaceName,
                      MulDiv( -Options.lfHFFont.lfHeight, 72,
                              nLogicalPixelsY ) ) ;
   SetDlgItemText( hDlg, IDD_HEADERFONT, szText ) ;

   wsprintf( szText+1, GRCS(IDS_MSG_PRINTERONPORT),
             (LPSTR)lpdn+(lpdn->wDeviceOffset),
             (LPSTR)lpdn+(lpdn->wOutputOffset) ) ;
   szText[0] = ' ' ;
   SetDlgItemText( hDlg, IDD_PRINTER, szText ) ;

   wsprintf( szText, " %d", Options.nColumns ) ;
   SetDlgItemText( hDlg, IDD_COLUMNS, szText ) ;

   if (Options.nColumns < 2)
      wsprintf( szText+1, GRCS(IDS_OTHERINFO), Page.nCharsPerLine,
                                               Page.nLinesPerPage ) ;
   else
   {
      int nCPC ;

      nCPC = MulDiv( GetDeviceCaps( hdc, LOGPIXELSX ),
                          Options.nColSpace, 1000 ) ;

      nCPC = Page.nCharsPerLine / Options.nColumns - ((nCPC/2) / Page.xChar) ;

      wsprintf( szText+1, GRCS(IDS_OTHERINFO), nCPC, Page.nLinesPerPage ) ;
   }

   szText[0] = ' ' ;
   SetDlgItemText( hDlg, IDD_CHARSPERLINE, szText ) ;

   if (lpdm && (lpdm->dmFields & DM_ORIENTATION))
   {
      if (lpdm->dmOrientation == DMORIENT_PORTRAIT)
         SetDlgIcon( hDlg, IDD_WINPRINT_ICO, hicoPortrait ) ;
      else
         SetDlgIcon( hDlg, IDD_WINPRINT_ICO, hicoLandscape ) ;
   }
   else
   {
      DP1( hWDB, "Printer does not support dmOrientation!" ) ;
      switch (Escape( hdc, GETSETPRINTORIENT, 0, NULL, NULL))
      {
         case 1:  //portrait
            SetDlgIcon( hDlg, IDD_WINPRINT_ICO, hicoPortrait ) ;
         break ;

         case 2:
            SetDlgIcon( hDlg, IDD_WINPRINT_ICO, hicoLandscape ) ;
         break ;

         default:
            SetDlgIcon( hDlg, IDD_WINPRINT_ICO, hicoNoOrient ) ;
      }
   }

   DeleteDC (hdc) ;

   if (lpdn)
      GlobalUnlock( ghDevNames ) ;

   if (lpdm)
      GlobalUnlock( ghDevMode ) ;

   wsprintf( szText, "%s %c [%s]", (LPSTR)GRCS(IDS_APPNAME),
             bModify ? '*' : '-', (LPSTR)szCurrentConfig) ;
   SetWindowText (hDlg, szText) ;

   return TRUE ;
} /* UpdateCurrentInfo()  */

/****************************************************************
 *  HICON WINAPI SetDlgIcon( HWND hDlg, UINT wID, HICON hIcon )
 *
 *  Description: 
 *
 *    Sets an icon control
 *
 *  Comments:
 *
 ****************************************************************/
HICON WINAPI SetDlgIcon( HWND hDlg, UINT wID, HICON hIcon )
{
   if (fWin31)
      return (HICON)SendDlgItemMessage( hDlg, wID, STM_SETICON, (WPARAM)hIcon, 0L ) ;
   else
      SetDlgItemText( hDlg, wID, MAKEINTRESOURCE( hIcon ) ) ;

   return 0 ;

} /* SetDlgIcon()  */

static LPSTR astrchr (LPSTR str, int ch)
{
   while (*str)
   {
      if (ch == *str)
         return str ;
      str = AnsiNext (str) ;
   }
   return NULL ;
}


static LPSTR astrrchr (LPSTR str, int ch)
{
   LPSTR strl = str + lstrlen (str) ;

   do
   {
      if (ch == *strl)
         return strl ;
      strl = AnsiPrev (str, strl) ;
   }
   while (strl > str) ;
   return NULL ;
}
/************************************************************************
 * End of File: ws_dlg.c
 ***********************************************************************/

