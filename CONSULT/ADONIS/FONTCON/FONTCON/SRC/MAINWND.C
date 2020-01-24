//*************************************************************
//  File name: mainwnd.c
//
//  Description: 
//      Handles the dialog box and it procedure
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include "dde.h"
#include "panose.h"
#include "filter.h"
#include "purchase.h"
#include "newuser.h"
#include "viewtext.h"

#include <dos.h>

DLGPROC glpfnPanoseDlg=NULL;

WORD NEAR PASCAL GetWindowsVersion(VOID) ;
VOID NEAR PASCAL HandleMenuPopup( WPARAM wParam, LPARAM lParam ) ;

BOOL CALLBACK fnFileVersionDlg( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam ) ;

//*************************************************************
//
//  MainWndProc
//
//  Purpose:
//      Window procedure for the main window
//
//
//  Parameters:
//      HWND hWnd
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

LONG CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {

            //*** Create status bar FIRST!!!!
            if (ghwndStat = CreateStatus( hWnd, ID_STATUS ))
            {
                SetWindowText( ghwndStat, GRCS(IDS_STAT_READY) ) ;
                ShowWindow( ghwndStat, SW_NORMAL ) ;
            }
            else
                return -1L;
            
            //*** Create slider
            if (ghwndSlider = CreateSlider( hWnd, ID_SLIDER ))
            {
                ShowWindow( ghwndSlider, SW_NORMAL ) ;
            }
            else
                return -1L;

            //*** Create Font thumb nail Viewer
            if (ghwndFontView = CreateFontView( hWnd, ID_FONTVIEW ))
                ShowWindow( ghwndFontView, SW_NORMAL ) ;
            else
                return -1L;

            //*** Create ExtendedListbox
            if (ghwndELB = CreateELB( hWnd, ID_ELB ))
                ShowWindow( ghwndELB, SW_NORMAL ) ;
            else
                return -1L;

            //*** Create Toolbar
            if (ghwndToolbar = CreateToolbar( hWnd, ID_TOOLBAR ))
                ShowWindow( ghwndToolbar, SW_NORMAL ) ;
            else
                return -1L;

        }
        break;

        case WM_SETFOCUS:
            if (IsWindow(ghwndELB))
               SetFocus( ghwndELB ) ;
        break ;

        case WM_INITMENUPOPUP:
        case WM_INITMENU:
        case WM_MENUSELECT:
            return MenuHandler( msg, wParam, lParam );
        break;

        case WM_GETMINMAXINFO:
        case WM_SIZE:
            SizeHandler( hWnd, msg, wParam, lParam );
        break;

        case WM_COMMAND:
            return CmdHandler(wParam,lParam);
        break;

        case WM_SYSCOLORCHANGE:
           SendMessage( ghwndELB, msg, wParam, lParam ) ;
           SendMessage( ghwndSlider, msg, wParam, lParam ) ;
           SendMessage( ghwndStat, msg, wParam, lParam ) ;
        break ;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            #ifndef DROPSHADOW
            RECT        rc ;
            RECT        rc2 ;
            #endif

            BeginPaint( hWnd, &ps );

            #ifdef DROPSHADOW
            tdDrawDropShadow( ps.hdc, ghwndSlider, SHADOW_THICKNESS );
            tdDrawDropShadow( ps.hdc, ghwndELB, SHADOW_THICKNESS );
            tdDrawDropShadow( ps.hdc, ghwndFontView, SHADOW_THICKNESS );
            #else

            SetBkColor( ps.hdc, GetSysColor( COLOR_BTNFACE ) ) ;
            tdGetChildRect( ghwndSlider, &rc ) ;
            tdGetChildRect( ghwndELB, &rc2 ) ;

            UnionRect( &rc, &rc2, &rc ) ;

            InflateRect( &rc, SHADOW_THICKNESS, SHADOW_THICKNESS ) ;
            tdDraw3DRect( ps.hdc, &rc, SHADOW_THICKNESS, DRAW3D_IN ) ;

            tdGetChildRect( ghwndFontView, &rc ) ;
            InflateRect( &rc, SHADOW_THICKNESS, SHADOW_THICKNESS ) ;
            tdDraw3DRect( ps.hdc, &rc, SHADOW_THICKNESS, DRAW3D_IN ) ;

            #endif

            EndPaint( hWnd, &ps );
        }
        break;

        case WM_CLOSE:
            /*
             * If we detect that we're about to run UPSHOP to
             * upgrade then tell the user by putting up
             * a message box saying "Font Shopper components
             * have been updated.  FontShopper will restart now"
             */
            if (gfUpShop)
            {
               ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                           IDS_APPNAME, IDS_ERR_RESTARTING ) ;
            }

            SaveConfiguration();
        break;

        case WM_DESTROY:
            WinHelp( hWnd, gszHelpFile, HELP_QUIT, 0L );
            PostQuitMessage( 0 );
        break;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );

} //*** MainWndProc


//*************************************************************
//
//  CmdHandler
//
//  Purpose:
//      Handles all the WM_COMMAND messages for the main window
//
//
//  Parameters:
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LRESULT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

LRESULT WINAPI CmdHandler(WPARAM wParam,LPARAM lParam)
{
   switch (wParam)
   {
      case IDM_ACTION_UPDATE:
      {
         UINT  ui ;
         static BOOL gfUpdatedOnce = FALSE ;

         GetCustInfo( &gCustInfo, GRCS( IDS_APPNAME ) ) ;

         if (!gfAutoLogon &&
             !gfRestarted &&
             gCustInfo.szUserID[0] == '\0')
         {
            switch( SignUpNewUser( ghwndMain, &gCustInfo, FALSE ) )
            {
               case 0:  /* Cancel */
               break ;

               case 1:  /* Entered an ID */
               break ;

               case 2:  /* Bought a subscription */
               case 3:  /* Got a demo user ID */
                  BBSDisConnect() ;
                  return 0L ;
               break ;

            }
         }

         if (gfAutoLogon == TRUE ||
            (gfRestarted == FALSE &&
            (gfUpdatedOnce || IDYES == ErrorResBox( ghwndMain, NULL,
                                 MB_ICONQUESTION | MB_YESNO,
                                 IDS_APPNAME,
                                 IDS_ERR_CONNECTNOW ))))
         {
            /* Updating the local database is transparent to us here.
             * BBSConnect() will update everything.
             */
            if (!IsBBSConnected())
               ui = BBSConnect( ghwndMain ) ;

            BBSDisConnect() ;

            if (ui == 1) /* Everything is up to date */
            {
               if (gfAutoLogon == FALSE)
                  ErrorResBox( ghwndMain, ghInst, MB_ICONINFORMATION,
                              IDS_APPNAME, IDS_ERR_UPTODATE ) ;
            }

            if (gfAutoLogon == TRUE)
            {
               PostMessage( ghwndMain, WM_CLOSE, 0, 0L ) ;
               break ;
            }

            if (ui != 0 && !gfUpShop && gCustInfo.szUserID[0] == '\0')
            {
               SignUpNewUser( ghwndMain, &gCustInfo, FALSE ) ;
               BBSDisConnect() ;//just in case
            }
         }

         gfUpdatedOnce = TRUE ;

      }
      break;

        case IDM_ACTION_PASSWORD:
        {
            char     szNewPassword[MAX_PASSWORD+1] ;
            BYTE     bResult = ASCII_NAK ;

            if (!GetCustInfo( &gCustInfo, GRCS( IDS_APPNAME ) ))
            {
               ErrorResBox( ghwndMain, ghInst, MB_ICONINFORMATION,
                         IDS_APPNAME, IDS_ERR_CUSTINFO ) ;
               break ;
            }

            /*
             * Pop up a dialog saying "Enter your old and
             * new passwords..."
             */
            if (!GetUserIDNewPasswordDlg( ghwndMain, gszHelpFile, gCustInfo.szUserID, gCustInfo.szPassword, szNewPassword ))
               break ;

            if (IsBBSConnected() || BBSConnect( ghwndMain ))
            {
               WORD wActive = 0 ;

               SendBBSMsg( MSG_SET_USER_ID, gCustInfo.szUserID, NULL ) ;

               if (SendBBSMsg( MSG_BEGIN_SESSION, NULL, &bResult ))
               {
                  BBSDisConnect() ;
                  return 0 ;
               }

               if (bResult != ASCII_ACK)
               {
                  switch( bResult )
                  {
                     case ASCII_EOT:
                        /* Database Error: Database is DOWN.
                           */
                        BBSDisConnect() ;
                        ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                              IDS_APPNAME, IDS_ERR_DATABASE_DOWN ) ;
                     break ;

                     case ASCII_NAK:
                     default:
                        /* Error:
                           */
                        BBSDisConnect() ;
                        ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                              IDS_APPNAME, IDS_ERR_ACK_BAD ) ;
                     break ;
                  }

                  ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                            IDS_APPNAME, IDS_ERR_PASSWORDNOTCHANGED ) ;

                  break ;
               }

               if (-1 == CheckAccountStatus( ghwndMain ))
                  break ;

               if (SendBBSMsg( MSG_SET_PASSWD, szNewPassword, NULL ))
               {
                  BBSDisConnect() ;
                  break ;
               }

               if (SendBBSMsg( MSG_COMMIT_PASSWD, NULL, &bResult))
               {
                  BBSDisConnect() ;
                  break ;
               }
               
               BBSDisConnect() ;

               if (bResult == ASCII_ACK)
               {
                  gCustInfo.fValidPassword = TRUE ;
                  SavePassword( &gCustInfo, szNewPassword, GRCS( IDS_APPNAME ) ) ;

                  ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                           IDS_APPNAME, IDS_ERR_PASSWORDCHANGED ) ;
               }
               else
               {
                  ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                           IDS_APPNAME, IDS_ERR_PASSWORDNOTCHANGED ) ;
               }
            }
            else
            {
               ErrorResBox( ghwndMain, NULL, MB_ICONINFORMATION,
                            IDS_APPNAME, IDS_ERR_PASSWORDNOTCHANGED ) ;
               DP1( hWDB, "BBSConnect() returned FALSE!" ) ;
            }
        }
        break;

      case IDM_ACTION_PURCHASE_SUB:
         if (gfExpiredUser == FALSE)
            SignUpNewUser( ghwndMain, &gCustInfo, !gfDemoUser ) ;
         else
            RenewUser( ghwndMain, &gCustInfo ) ;
         BBSDisConnect() ;
      break ;

        case IDM_ACTION_UNMARK:
        {
            LPDBNODE lpdn = GetFirstPurchasedNode( glpDB );

            while (lpdn)
            {
                VI_UNCHECKNODE(lpdn);

                lpdn = GetNextPurchasedNode( glpDB, lpdn );
            }
            glpDB->wFonts  = 0;
            glpDB->wColls  = 0;
            glpDB->dwTotal = 0;
            glpDB->dwSize  = 0;
            UpdateStatLine( ghwndStat, glpDB );
            UpdateToolbar( ghwndToolbar, glpDB );
            InvalidateRect( ghwndELB, NULL, FALSE );
            SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, FALSE);
        }
        break;

        case IDM_ACTION_COLLECTIONS:
        {
            FARPROC lpProc = MakeProcInstance( (FARPROC)CollectionsProc, ghInst );

            if (ELBGetCurSel(ghwndELB, NULL))
                DialogBox(ghInst, MAKEINTRESOURCE(COLLECTIONS_DLG), ghwndMain, (DLGPROC)lpProc);
            FreeProcInstance( lpProc );

            if (glpDB->wFonts || glpDB->wColls)
                SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, TRUE );
            else
                SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, FALSE );
        }
        break;

        case IDM_ACTION_DOWNLOAD:
        {
            FARPROC lpProc ;
            
            if (gfExpiredUser)
            {
               if (!RenewUser( ghwndMain, &gCustInfo ))
                  break ;
            }

            lpProc = MakeProcInstance((FARPROC)PurchaseProc, ghInst);

            DialogBox(ghInst, MAKEINTRESOURCE(PURCHASE_DLG), ghwndMain, (DLGPROC)lpProc);
            FreeProcInstance( lpProc );
            InvalidateRect( ghwndELB, NULL, TRUE );
        }
        break;

        case IDM_ACTION_EXIT:
            PostMessage( ghwndMain, WM_CLOSE, 0, 0L );
        break;

        case IDM_INFO_CUSTOMER:
            CustomerInfoDialog( ghwndMain, gszHelpFile,
                                GRCS(IDS_INIFILE), NULL );
            GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
        break;

        case IDM_INFO_SHIPPING:
            ShippingInfoDialog( ghwndMain, gszHelpFile,
                                GRCS(IDS_INIFILE), NULL );
            GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
        break;

        case IDM_INFO_BILLING: 
            BillingInfoDialog( ghwndMain,  gszHelpFile,
                                GRCS(IDS_INIFILE), NULL );
            GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
        break;

        case IDM_SETTINGS_SETUP:
            CommSettingsDialog( ghwndMain,  gszHelpFile,
                                GRCS(IDS_INIFILE), NULL );
            GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;
        break;

        case IDM_SORT_PANOSE:
        {
            if (!glpfnPanoseDlg)
                glpfnPanoseDlg = (DLGPROC)MakeProcInstance((FARPROC)PanoseDlg,ghInst);

            if (IsWindow(ghwndPanoseDlg))
                SetActiveWindow( ghwndPanoseDlg );
            else
                ghwndPanoseDlg = CreateDialog( ghInst, MAKEINTRESOURCE(PANOSE_DLG), 
                                    ghwndMain, glpfnPanoseDlg );
            UpdateSortingButtons();
        }
        break;

        case IDM_SORT_NAME:
        case IDM_SORT_PRICE:
        case IDM_SORT_SIZE:
            if ( IsWindow(ghwndPanoseDlg) )
                DestroyWindow( ghwndPanoseDlg );

// BUGFIX 0022
            //*** Don't resort on the same key
            if (glpDB->uSort == (wParam - IDM_SORT_NAME + DB_SORT_NAME))
                break;
// BUGFIX 0022

            SetDBELBToSort( ghwndELB, wParam - IDM_SORT_NAME + DB_SORT_NAME );
            UpdateSortingButtons();
        break;

        case IDM_SET_FILTER:
        {
            FARPROC lpProc = MakeProcInstance( (FARPROC)SetFilterProc, ghInst );

            DialogBox(ghInst, MAKEINTRESOURCE(FILTER_DLG), ghwndMain, (DLGPROC)lpProc);
            FreeProcInstance( lpProc );
        }
        break;

        case IDM_CLEAR_FILTER:
            if (glpDB->hFilter)
                SetDBELBToFilter( ghwndELB, NULL );
            SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SET_FILTER, FALSE );

        break;

        case IDM_GROUP_PUB:
        {
            glpDB->uView ^= DB_VIEW_PUBLISHERS;
            SetDBELBToView( ghwndELB, glpDB->uView );
            UpdateGroupingButtons();
        }
        break;

        case IDM_GROUP_FACE:
        {
            glpDB->uView ^= DB_VIEW_FACES;
            SetDBELBToView( ghwndELB, glpDB->uView );
            UpdateGroupingButtons();
        }
        break;

        case IDM_SETTINGS_FONT:
            DoChooseFont();
        break;

        case IDM_HELP_INDEX:
            ViewFontshopHelp( IDS_HELP_INDEX );
        break;

        case IDM_HELP_SEARCH:  
            WinHelp( ghwndMain, "Winhelp.hlp", HELP_PARTIALKEY, (DWORD)(LPSTR)"" );
        break;

        case IDM_HELP_USINGHELP:
            WinHelp( ghwndMain, NULL, HELP_HELPONHELP, 0 );
        break;

        case IDM_HELP_ABOUT:
        {
            FARPROC lpProc ;
            
            if (GetKeyState( VK_SHIFT ) & 0x8000)
            {
               /* Do file versions */
               lpProc = MakeProcInstance( (FARPROC)fnFileVersionDlg, ghInst );
               DialogBox( ghInst, MAKEINTRESOURCE(FILEVERSIONS_DLG),
                           ghwndMain, (DLGPROC)lpProc);
            }
            else
            {
               lpProc = MakeProcInstance( (FARPROC)AboutBoxProc, ghInst );
               DialogBox( ghInst, MAKEINTRESOURCE(ABOUT_DLG), ghwndMain,
                           (DLGPROC)lpProc);
            }

            FreeProcInstance( lpProc );
        }
        break;

        case ID_SLIDER:
            if (ghwndELB && (HIWORD( lParam ) == SLN_INDRAG) )
                SyncDBELBToSlider( ghwndELB, ghwndSlider );
        break;

        case ID_ELB:
        {
            long l;

            if (HIWORD(lParam)==ELBN_SELCHANGE)
            {
                FVSetCurrentNode( ghwndFontView, 
                    (LPDBNODE)ELBGetCurSel( ghwndELB, NULL ) );
            }

            l = HandleDBELBCommand(ghwndELB, WM_COMMAND, wParam, lParam);
            UpdateStatLine( ghwndStat, glpDB );
            UpdateToolbar( ghwndToolbar, glpDB );

            if (glpDB && (glpDB->wFonts || glpDB->wColls))
                SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, TRUE );
            else
                SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, FALSE);
            return l;
        }
        break;

        case IDM_INFO_WHATSNEW:
        {
            LPSTR lp;
            struct _find_t ft;

            GetDatabaseDir( gszDBPath );
            lp = gszDBPath + lstrlen( gszDBPath );

            lstrcpy( lp, "WHATSNEW.*" );

            if (_dos_findfirst( gszDBPath, _A_NORMAL, &ft )==0)
            {
                lstrcpy( lp, ft.name );
                ViewText( ghwndMain, gszDBPath, GRCS(IDS_WHATSNEW),
                    VT_OK_CANCEL|VT_LOADFILE|VT_NOHELP );
            }
            GetDatabaseDir( gszDBPath );
        }
        break;

#ifdef INTERNAL
#pragma message( "Making internal version of FontCon!" )
      case IDM_INGRES:
      //*** Dump the database to files
      {
         OFSTRUCT of;
         HFILE hfColl, hfFace, hfStyle, hfVendor;
         LPPUB lpPub;
         char  szTemp[256];
         LPSTR lp;

      #define ADDINT(LNG) lp += wsprintf(lp, ",%d", (int)(LNG) );
      #define ADDLONG(LNG) lp += wsprintf(lp, ",%ld", (long)(LNG) );
      #define ADDMONEY(LNG) lp += wsprintf(lp, ",%ld.%02ld", (LNG)/100, (LNG)%100 );
      #define ADDSTRING(LNG) lp += wsprintf(lp, ",%s", (LPSTR)(LNG) );

         SetWindowText( ghwndStat, "Generating Ingres Files..." ) ;
         UpdateWindow( ghwndStat ) ;

         hfColl = OpenFile( "Collxns.ing", &of, OF_WRITE|OF_CREATE );
         hfFace = OpenFile( "Faces.ing", &of, OF_WRITE|OF_CREATE );
         hfStyle = OpenFile( "Styles.ing", &of, OF_WRITE|OF_CREATE );
         hfVendor = OpenFile( "Vendors.ing", &of, OF_WRITE|OF_CREATE );

         if (!hfColl || !hfFace || !hfStyle || !hfVendor)
            break ;

         lpPub = GetFirstPublisher( glpDB, NULL );

         SetCursor( LoadCursor(NULL,IDC_WAIT) );

         while (lpPub)
         {
            char szFile[40];
            LPFAMILY lpFamily = GetFirstFamily( lpPub, NULL );
            LPCOLL lpColl = GetFirstCollection( lpPub );

            wsprintf( szTemp, "%d,%s,FSPUB%03x\r\n", lpPub->fiFSPub, lpPub->lpszPublisher, lpPub->fiFSPub );
            _lwrite( hfVendor, (LPSTR)szTemp, lstrlen(szTemp) );

            LoadPublisherCollections(lpPub);
            lpColl = GetFirstCollection( lpPub );

            while (lpColl)
            {
                  lp = (LPSTR)szTemp;

                  lp += wsprintf( szTemp, "%ld", lpColl->Info.dwID );
                  ADDLONG( lpPub->fiFSPub );
                  ADDMONEY( lpColl->Info.dwPrice );
                  ADDSTRING( lpColl->Info.szName );
                  ADDLONG( lpColl->Info.rgShippingRates[0] );
                  ADDLONG( lpColl->Info.rgShippingRates[1] );
                  ADDLONG( lpColl->Info.rgShippingRates[2] );
                  ADDLONG( lpColl->Info.rgShippingRates[3] );
                  ADDLONG( lpColl->Info.rgShippingRates[4] );

                  wsprintf( lp, "\r\n" );

                  _lwrite( hfColl, (LPSTR)szTemp, lstrlen(szTemp) );

                  lpColl = GetNextCollection( lpColl );
            }
            FreePublisherCollections( lpPub );

            while (lpFamily)
            {
                  LPFACE lpFace = GetFirstFace( lpFamily, NULL );

                  while (lpFace)
                  {
                     LPSTYLE lpStyle = GetFirstStyle( lpFace, NULL );

                     lp = (LPSTR)szTemp;

                     lp += wsprintf( szTemp, "%ld", lpFace->Info.dwID );
                     ADDLONG( lpPub->fiFSPub );
                     ADDMONEY( lpFace->Info.dwPrice );
                     ADDSTRING( lpFace->Info.szName );

                     wsprintf( szFile, "%FF%03X%03X", lpPub->fiFSPub, lpFamily->fiFF );
                     ADDSTRING( szFile );
                     wsprintf( szFile, "%FS%03X%03X", lpPub->fiFSPub, lpFamily->fiFF );
                     ADDSTRING( szFile );

                     wsprintf( lp, "\r\n" );

                     _lwrite( hfFace, (LPSTR)szTemp, lstrlen(szTemp) );

                     while (lpStyle)
                     {
                        lp = (LPSTR)szTemp;

                        lp += wsprintf( szTemp, "%ld", lpStyle->Info.dwID );
                        ADDLONG( lpFace->Info.dwID );
                        ADDMONEY( lpStyle->Info.dwPrice );
                        ADDLONG( lpFace->Info.dwSize );
                        ADDSTRING( lpStyle->Info.szName );
                        ADDSTRING( lpStyle->Info.szTTFFile );

                        lp += wsprintf( lp, ",%03d%03d%03d%03d%03d%03d%03d%03d%03d%03d",
                              (int)lpStyle->Info.Panose.bFamilyType,
                              (int)lpStyle->Info.Panose.bSerifStyle,
                              (int)lpStyle->Info.Panose.bWeight,
                              (int)lpStyle->Info.Panose.bProportion,
                              (int)lpStyle->Info.Panose.bContrast,
                              (int)lpStyle->Info.Panose.bStrokeVariation,
                              (int)lpStyle->Info.Panose.bArmStyle,
                              (int)lpStyle->Info.Panose.bLetterform,
                              (int)lpStyle->Info.Panose.bMidline,
                              (int)lpStyle->Info.Panose.bXHeight );

                        wsprintf( lp, "\r\n" );

                        _lwrite( hfStyle, (LPSTR)szTemp, lstrlen(szTemp) );

                        lpStyle = GetNextStyle( lpStyle, NULL );
                     }
                     lpFace = GetNextFace( lpFace, NULL );
                  }
                  lpFamily = GetNextFamily( lpFamily, NULL );
            }
            lpPub = GetNextPublisher( lpPub, NULL );
         }

         _lclose( hfColl );
         _lclose( hfFace );
         _lclose( hfStyle );
         _lclose( hfVendor );

         SetCursor( LoadCursor(NULL,IDC_ARROW) );
      }    
      break ;
#endif //INTERNAL

        #ifdef DEBUG
        case IDM_WDB_CONFIG:
            wdbConfigDlg( hWDB ) ;
        break ;

        case IDM_ERRORS:
            ReportFontshopError( NULL, (LPSTR)IDS_FATALSTARTUP, GRCS(IDS_APPNAME),
                MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
            ReportFontshopError( NULL, (LPSTR)IDS_DISKERROR, GRCS(IDS_APPNAME),
                MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
            ReportFontshopError( NULL, (LPSTR)IDS_CORRUPTWARN, GRCS(IDS_APPNAME),
                MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0x4f3d );
            ReportFontshopError( NULL, (LPSTR)IDS_RECOVERERROR, GRCS(IDS_APPNAME),
                MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0x4f3d );
            ReportFontshopError( NULL, (LPSTR)IDS_MEMORYONLOAD, GRCS(IDS_APPNAME),
                MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        break ;

        case IDM_RELOAD:
        {
            ReloadDatabase();
        }
        break;

        #endif
    }

    return DefWindowProc( ghwndMain, WM_COMMAND, wParam, lParam );

} //*** CmdHandler

#if 0

/****************************************************************
 *  WORD NEAR PASCAL GetWindowsVersion(VOID)
 *
 *  Description: 
 *
 *  Returns a hex 3-digit number like 210, 200, 300, 310.
 *
 *  Yanked out of MISC.C in WinShopper.  Don't ask *me* why
 *  we don't just use the  return value from GetVersion...
 *
 *  Comments:
 *
 ****************************************************************/
WORD NEAR PASCAL GetWindowsVersion(VOID)
{
    WORD nVersion,wRetval;
    
    nVersion = LOWORD( GetVersion() ) ;
    wRetval = (nVersion & 0xff) << 8;
    wRetval += ((nVersion & 0xff00) >> 8);

    return wRetval;

} /* GetWindowsVersion()  */

#endif


//*************************************************************
//
//  SizeHandler
//
//  Purpose:
//      Handles the sizing of the window
//
//
//  Parameters:
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LRESULT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

LRESULT WINAPI SizeHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if (msg == WM_SIZE)
    {
        HDWP hdwp ;
        short dy = HIWORD( lParam );
        short dx = LOWORD( lParam );
        short dh, dyThumb = THUMB_CY+4;
      #ifdef DROPSHADOW
        short cx = BLANKSPACE ;
      #else
        short cx = BLANKSPACE + SHADOW_THICKNESS ;
      #endif
        short cy = dy;
        short cyTB = TOOLBAR_HEIGHT + BLANKSPACE;

        if (!(hdwp = BeginDeferWindowPos( 1 )))
            return DefWindowProc( hwnd, msg, wParam, lParam ) ;

        if (IsWindow( ghwndStat ))
        {
            dh = (short)SendMessage(ghwndStat,ST_GETHEIGHT,0,(LPARAM)0L);

            //*** Resize Status
            cy -= dh-1;
            hdwp = DeferWindowPos( hdwp, ghwndStat, NULL,
                        -1, cy, dx+2, dh, SWP_NOZORDER ) ;

            #ifdef DROPSHADOW
            dx -= 2*BLANKSPACE + SHADOW_THICKNESS;
            #else
            dx -= 2*BLANKSPACE + 2*SHADOW_THICKNESS;
            #endif
        }

        if (IsWindow( ghwndFontView ))
        {
            //*** Resize FontView
            
         #ifdef DROPSHADOW
            cy -= BLANKSPACE + SHADOW_THICKNESS + dyThumb;
         #else
            cy -= BLANKSPACE + SHADOW_THICKNESS + dyThumb;
         #endif

            hdwp = DeferWindowPos( hdwp, ghwndFontView, NULL,
                    cx, cy, dx, dyThumb, SWP_NOZORDER ) ;
        }

        if (IsWindow( ghwndToolbar))
           //*** Resize Toolbar
           hdwp = DeferWindowPos( hdwp, ghwndToolbar, NULL,
                       -1, -1, LOWORD( lParam )+2, cyTB, SWP_NOZORDER ) ;

        if (IsWindow( ghwndSlider))
           //*** Resize Slider
         #ifdef DROPSHADOW
           hdwp = DeferWindowPos( hdwp, ghwndSlider, NULL,
                       cx, cyTB+BLANKSPACE,dx,dh, SWP_NOZORDER ) ;
         #else
           hdwp = DeferWindowPos( hdwp, ghwndSlider, NULL,
                       cx, cyTB+BLANKSPACE+SHADOW_THICKNESS,dx,dh, SWP_NOZORDER ) ;
         #endif
        
        //*** Resize ELB
        #ifdef DROPSHADOW
        dy = cy - (2*BLANKSPACE) - SHADOW_THICKNESS - dh -cyTB + 1;
        cy = cyTB+BLANKSPACE + dh - 1;
        #else
        dy = cy - (2*BLANKSPACE) - (2*SHADOW_THICKNESS) - dh -cyTB + 1;
        cy = cyTB+BLANKSPACE + SHADOW_THICKNESS + dh - 1;
        #endif
        dh = dy;

        if (IsWindow( ghwndELB))
           hdwp = DeferWindowPos( hdwp, ghwndELB, NULL,
                       cx, cy, dx, dh, SWP_NOZORDER ) ;

        EndDeferWindowPos( hdwp ) ;
        if (IsWindow( ghwndELB) && IsWindow( ghwndSlider ))
           SyncDBELBToSlider( ghwndELB, ghwndSlider );
        return (LRESULT)0L ;
    }

    if (msg==WM_GETMINMAXINFO)
    {
        MINMAXINFO FAR *lpmmi = (MINMAXINFO FAR *)lParam;
        short dh = 0, dw = 0;

        if (IsWindow( ghwndStat ))
            dh = (short)SendMessage(ghwndStat,ST_GETHEIGHT,0,(LPARAM)0L);
        dh = 8*(dh+SHADOW_THICKNESS) + 3*BLANKSPACE + THUMB_CY;
        dw = (GetSystemMetrics(SM_CXMINTRACK ) * 9)/2;

        lpmmi->ptMinTrackSize.x = dw;
        lpmmi->ptMinTrackSize.y = dh;
    }
    return DefWindowProc( hwnd, msg, wParam, lParam ) ;

} //*** SizeHandler


//*************************************************************
//
//  MenuHandler
//
//  Purpose:
//      Handles all the menu help and check marking
//
//
//  Parameters:
//      UINT wMsg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LRESULT FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

LRESULT WINAPI MenuHandler( UINT wMsg, WPARAM wParam, LPARAM lParam )
{
    static HMENU hMenu;

    switch ( wMsg )
    {
        case WM_INITMENU:
            // 
            // Get the handle to the window here, this way we store
            // it a way and dont have to waste time getting it later
            //
            hMenu = (HMENU)wParam ;
        break ;

        case WM_INITMENUPOPUP:
            HandleMenuPopup( wParam, lParam ) ;
            return DefWindowProc( ghwndMain, wMsg, wParam, lParam ) ;
        break ;

        case WM_MENUSELECT:
            if (!IsWindowVisible(ghwndStat))
            {
                break ;
            }

            DPF5( hWDB, "WM_MENUSELECT:   %04X, %08lX", wParam, lParam ) ;

            if (LOWORD(lParam) == 0xFFFF || HIWORD(lParam) == 0 || (WORD)wParam == 0)
            {
                DP5( hWDB, "Noting selected" ) ;
                SetWindowText( ghwndStat, GRCS(IDS_STAT_READY) ) ;
                break ;
            }

            if (LOWORD(lParam) & MF_HILITE)
            {   
                if ((LOWORD(lParam) & MF_POPUP))
                {
                    WORD wIndex;

                    DP5( hWDB, "  MF_POPUP" ) ;
                    DP5( hWDB, ", SubMenu = %04X", GetSubMenu( hMenu, 0 ) ) ;

                    for (wIndex=IDS_POPUPMENUSTART; wIndex<=IDS_POPUPMENUEND; wIndex++)
                    {
                        if (GetSubMenu( hMenu, wIndex-IDS_POPUPMENUSTART ) 
                            == (HMENU)wParam)
                        {
                            SetWindowText( ghwndStat, GRCS(wIndex) ) ;
                            return DefWindowProc( ghwndMain, wMsg, wParam, lParam ) ;
                        }
                    }
                    SetWindowText( ghwndStat, " " ) ;
                }
                else /* if MF_POPUP */
                {
                    if (LOWORD((DWORD)lParam & MF_SYSMENU))
                    {
                        DP5( hWDB, "  MF_SYSMENU" ) ;
                        // 
                        // It is a system menu item.
                        //
                        switch ((WORD)wParam)
                        {
                            case SC_SIZE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_SIZE) ) ;
                            break ;

                            case SC_MOVE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_MOVE) ) ;
                            break ;

                            case SC_MINIMIZE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_MINIMIZE) ) ;
                            break ;

                            case SC_MAXIMIZE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_MAXIMIZE) ) ;
                            break ;

                            case SC_CLOSE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_CLOSE) ) ;
                            break ;

                            case SC_RESTORE:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_RESTORE) ) ;
                            break ;

                            case SC_TASKLIST:
                                SetWindowText( ghwndStat, GRCS(IDS_SMNU_TASKLIST) ) ;
                            break ;
                        }
                    } /* MF_SYSMENU */
                    else
                    {
                        DP5( hWDB, "  Normal menu item" ) ;
                        //
                        // It's a normal menu item, wParam has the IDM
                        //
                        SetWindowText( ghwndStat, GRCS(wParam) ) ;
                    }
                }
            } /* MF_HILITE */

        break ;

      case WM_SYSCOLORCHANGE:
         if (ghbmpFontCon)
            DeleteObject( ghbmpFontCon ) ;

         ghbmpFontCon = LoadBitmap( ghInst, IDB_FONTCON );
         if (ghbmpFontCon)
         {
            bmColorTranslate( ghbmpFontCon, RGBLTMAGENTA, RGBWHITE, 0 ) ;
         }
      break ;
    }
    return DefWindowProc( ghwndMain, wMsg, wParam, lParam ) ;

} //*** MenuHandler


//*************************************************************
//
//  AboutBoxProc
//
//  Purpose:
//      Dlg proc for the about box
//
//
//  Parameters:
//      HWND hDlg
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL CALLBACK)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 8/92   MSM        Created
//               4/ 4/92   CEK        Modified so that the size of the
//                                    dialog is derived from the bitmap size.
//
//*************************************************************

BOOL CALLBACK AboutBoxProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    static int nKudos = 0;
    static HBITMAP hbmFontCon = NULL ;
    static HBITMAP hbmAdonis = NULL ;
    static HFONT   hfontReallySmall = NULL ;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            char     szFmt[32] ;
            char     szVer[128] ;
            char     szBuf[128] ;

            hbmFontCon = LoadBitmap( ghInst, IDB_FONTCON ) ;
            if (hbmFontCon)
               bmColorTranslate( hbmFontCon, RGBLTMAGENTA,
                              GetSysColor( COLOR_WINDOW ), 0 ) ;

            hbmAdonis = LoadBitmap( ghInst, IDB_ADONIS ) ;
            if (hbmFontCon)
               bmColorTranslate( hbmAdonis, RGBWHITE,
                              GetSysColor( COLOR_WINDOW ), 0 ) ;

            hfontReallySmall = ReallyCreateFontEx( NULL, "Small Fonts", "Regular", 6, 0 ) ;

            GetDlgItemText( hDlg, IDD_VERSION, szFmt, sizeof(szFmt)-1 ) ;

#ifdef INTERNAL
            if (VER_BUILD)
               wsprintf( szVer, "%d.%02d.%03d - %s", VER_MAJOR, VER_MINOR, VER_BUILD, (LPSTR)"INTERNAL BUILD!" ) ;
            else
               wsprintf( szVer, "%d.%02d - %s", VER_MAJOR, VER_MINOR, (LPSTR)"INTERNAL BUILD!" ) ;
#else
            if (VER_BUILD)
               wsprintf( szVer, "%d.%02d.%03d - %s %s", VER_MAJOR, VER_MINOR, VER_BUILD, (LPSTR)__DATE__, (LPSTR)VER_BETA_STRING ) ;
            else
               wsprintf( szVer, "%d.%02d - %s", VER_MAJOR, VER_MINOR, (LPSTR)__DATE__ ) ;
#endif            

            wsprintf( szBuf, szFmt, (LPSTR)GRCS(IDS_APPNAME), (LPSTR)szVer ) ;

            SetDlgItemText( hDlg, IDD_VERSION, szBuf ) ;

            SetWindowFont( GetDlgItem( hDlg, IDD_VERSION ), ghfontSmall, FALSE ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_COPYRIGHT ), ghfontSmall, FALSE ) ;
            SetWindowFont( GetDlgItem( hDlg, IDD_ALLRIGHTS ), ghfontSmall, FALSE ) ;

            SetWindowFont( GetDlgItem( hDlg, IDD_PANOSE ), hfontReallySmall, FALSE ) ;

            DlgCenter( hDlg, ghwndMain, TRUE );

            ShowWindow( hDlg, SW_NORMAL );
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC         hdcMem;
            BITMAP      bm ;
            RECT        rc ;
            RECT        rc2 ;
            RECT        rc3 ;

            if (!hbmFontCon || !hbmAdonis)
               break ;

            GetObject( hbmFontCon, sizeof( BITMAP ), &bm ) ;

            BeginPaint( hDlg, &ps );

            GetClientRect( hDlg, &rc ) ;

            GetWindowRect( GetDlgItem( hDlg, IDD_ALLRIGHTS ), &rc2 ) ;
            MapWindowRect( NULL, hDlg, &rc2 ) ;

            hdcMem = CreateCompatibleDC( ps.hdc ) ;

            hbmFontCon = SelectObject( hdcMem, hbmFontCon );
            BitBlt( ps.hdc, rc2.left, 10, bm.bmWidth, bm.bmHeight,
                     hdcMem, 0, 0, SRCCOPY );
            hbmFontCon = SelectObject( hdcMem, hbmFontCon );

            GetObject( hbmAdonis, sizeof( BITMAP ), &bm ) ;

            GetWindowRect( GetDlgItem( hDlg, IDD_PANOSE ), &rc3 ) ;
            MapWindowRect( NULL, hDlg, &rc3 ) ;

            hbmAdonis = SelectObject( hdcMem, hbmAdonis ) ;
            BitBlt( ps.hdc, rc2.left, rc2.bottom, bm.bmWidth, bm.bmHeight,
                     hdcMem, 0, 0, SRCCOPY );
            hbmAdonis = SelectObject( hdcMem, hbmAdonis ) ;
            DeleteDC( hdcMem );

            EndPaint( hDlg, &ps );
        }
        break;

        case WM_COMMAND:
        {
           if (wParam==IDOK || wParam ==IDCANCEL)
                EndDialog( hDlg, TRUE );
        }
        break;

      case WM_RBUTTONDBLCLK:
         /* Display versions...
          */

      break ;

        case WM_DESTROY:
           if (hbmFontCon)
               DeleteObject( hbmFontCon );
           if (hbmAdonis)
               DeleteObject( hbmAdonis );
           if (hfontReallySmall)
               DeleteObject( hfontReallySmall );
        break ;

    }

    return FALSE;

} //*** AboutBoxProc


BOOL CALLBACK fnFileVersionDlg( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
   static HFONT hfontFixed = NULL ;

   switch (msg)
   {
      case WM_INITDIALOG:
      {
         char     szBuf[256] ;
         WORD     wExeID ;

         hfontFixed = ReallyCreateFontEx( NULL, "Courier New", "Regular", 8, NULL ) ;

         SetWindowFont( GetDlgItem( hDlg, IDD_VERSION ), hfontFixed, FALSE ) ;

         for (wExeID = 0 ; wExeID < NUM_EXECUTABLES ; wExeID ++ )
         {
            wsprintf( szBuf, " %-13s Version %03d",
                      (LPSTR)GRCS( wExeID + IDS_EXECUTABLES ),
                      grgOurExeVer[wExeID] ) ;

            ListBox_AddString( GetDlgItem( hDlg, IDD_VERSION ),
                               szBuf ) ;
         }
         
         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), ghfontSmall, FALSE ) ;

         DlgCenter( hDlg, ghwndMain, TRUE );
      }
      break;

      case WM_COMMAND:
      {
         if (wParam==IDOK || wParam ==IDCANCEL)
           EndDialog( hDlg, TRUE );
      }
      break;


      case WM_DESTROY:
         if (hfontFixed)
            DeleteObject( hfontFixed ) ;
      break ;

   }

   return FALSE;

} //*** fnFileVersionDlg


//*************************************************************
//
//  HandleMenuPopup
//
//  Purpose:
//      Initializes the settings menu
//
//
//  Parameters:
//      
//
//  Return: (VOID NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 9/92   MSM        Created
//
//*************************************************************

VOID NEAR PASCAL HandleMenuPopup( WPARAM wParam, LPARAM lParam )
{
    int       nIndex;
    LPELBNODE lpen = (LPELBNODE)ELBGetCurSel(ghwndELB, (LPINT)&nIndex );
    LPPUB     lpPub = (LPPUB)lpen;
    struct _find_t ft;

    HMENU hMenu = GetMenu( ghwndMain ) ;

    if (!glpDB)
        return;

    switch ( glpDB->uSort)
    {
        case DB_SORT_NAME:
            CheckMenuItem( hMenu, IDM_SORT_NAME, MF_BYCOMMAND|MF_CHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PRICE, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_SIZE, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PANOSE, MF_BYCOMMAND|MF_UNCHECKED);
        break;

        case DB_SORT_PRICE:
            CheckMenuItem( hMenu, IDM_SORT_NAME, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PRICE, MF_BYCOMMAND|MF_CHECKED);
            CheckMenuItem( hMenu, IDM_SORT_SIZE, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PANOSE, MF_BYCOMMAND|MF_UNCHECKED);
        break;

        case DB_SORT_SIZE:
            CheckMenuItem( hMenu, IDM_SORT_NAME, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PRICE, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_SIZE, MF_BYCOMMAND|MF_CHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PANOSE, MF_BYCOMMAND|MF_UNCHECKED);
        break;

        case DB_SORT_PANOSE:
            CheckMenuItem( hMenu, IDM_SORT_NAME, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PRICE, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_SIZE, MF_BYCOMMAND|MF_UNCHECKED);
            CheckMenuItem( hMenu, IDM_SORT_PANOSE, MF_BYCOMMAND|MF_CHECKED);
        break;
    }

    if (glpDB->uView & DB_VIEW_PUBLISHERS)
        CheckMenuItem( hMenu, IDM_GROUP_PUB, MF_BYCOMMAND|MF_CHECKED);
    else
        CheckMenuItem( hMenu, IDM_GROUP_PUB, MF_BYCOMMAND|MF_UNCHECKED);

    if (glpDB->uView & DB_VIEW_FACES)
        CheckMenuItem( hMenu, IDM_GROUP_FACE, MF_BYCOMMAND|MF_CHECKED);
    else
        CheckMenuItem( hMenu, IDM_GROUP_FACE, MF_BYCOMMAND|MF_UNCHECKED);

    if (glpDB->hFilter)
        CheckMenuItem( hMenu, IDM_SET_FILTER, MF_BYCOMMAND|MF_CHECKED);
    else
        CheckMenuItem( hMenu, IDM_SET_FILTER, MF_BYCOMMAND|MF_UNCHECKED);

//*** Handle VIEW COLLECTIONS item
    //*** Walk to the top
    while (lpPub && NODETYPE(lpPub)!=NT_PUB)
        lpPub = PARENT(LPPUB,lpPub);

    if (!lpPub || lpPub->PubInfo.wCollections==0)
        EnableMenuItem( hMenu, IDM_ACTION_COLLECTIONS, MF_BYCOMMAND|MF_GRAYED);
    else
        EnableMenuItem( hMenu, IDM_ACTION_COLLECTIONS, MF_BYCOMMAND|MF_ENABLED);


//*** Handle DOWNLOAD/PURCHASE item and UNMARK all
    if (GetFirstPurchasedNode( glpDB ))
    {
        EnableMenuItem( hMenu, IDM_ACTION_DOWNLOAD, MF_BYCOMMAND|MF_ENABLED);
        EnableMenuItem( hMenu, IDM_ACTION_UNMARK, MF_BYCOMMAND|MF_ENABLED);
    }
    else
    {
        EnableMenuItem( hMenu, IDM_ACTION_DOWNLOAD, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem( hMenu, IDM_ACTION_UNMARK, MF_BYCOMMAND|MF_GRAYED);
    }

//*** Don't let him change his password if there's no UserID
    if (gCustInfo.szUserID[0] == '\0')
        EnableMenuItem( hMenu, IDM_ACTION_PASSWORD, MF_BYCOMMAND|MF_GRAYED);
    else
        EnableMenuItem( hMenu, IDM_ACTION_PASSWORD, MF_BYCOMMAND|MF_ENABLED);

   if (gfDemoUser == FALSE)
   {
      if (gfExpiredUser == TRUE)
      {
         ModifyMenu( hMenu, IDM_ACTION_PURCHASE_SUB,
                     MF_BYCOMMAND | MF_STRING, IDM_ACTION_PURCHASE_SUB, (LPCSTR)"&Renew Subscription..." ) ;
      }
      else
         EnableMenuItem( hMenu, IDM_ACTION_PURCHASE_SUB, MF_BYCOMMAND|MF_GRAYED);
   }
   else
      EnableMenuItem( hMenu, IDM_ACTION_PURCHASE_SUB, MF_BYCOMMAND|MF_ENABLED);

//*** Handle What's New item
    GetDatabaseDir( gszDBPath );

    lstrcat( gszDBPath,"WHATSNEW.*" );

    if (_dos_findfirst( gszDBPath, _A_NORMAL, &ft )==0)
        EnableMenuItem( hMenu, IDM_INFO_WHATSNEW, MF_BYCOMMAND|MF_ENABLED);
    else
        EnableMenuItem( hMenu, IDM_INFO_WHATSNEW, MF_BYCOMMAND|MF_GRAYED);

    GetDatabaseDir( gszDBPath );

//*** Handle Update Local Database
   if (gfAutoLogon || gfRestarted || gfDBChecked)
   {
      SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_UPDATE, FALSE );
      EnableMenuItem( hMenu, IDM_ACTION_UPDATE, MF_BYCOMMAND|MF_GRAYED);
   }
   else
   {
      SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_UPDATE, TRUE );
      EnableMenuItem( hMenu, IDM_ACTION_UPDATE, MF_BYCOMMAND|MF_ENABLED);
   }

} //*** InitActionMenu


//*************************************************************
//
//  UpdateStatLine
//
//  Purpose:
//      Updates the status bars price and size
//
//
//  Parameters:
//      HWND hwndStatus
//      LPDB lpdb
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/15/92   MSM        Created
//
//*************************************************************

VOID WINAPI UpdateStatLine( HWND hwndStatus, LPDB lpdb )
{
    char szTemp[40];
    DWORD dw;

    if (!lpdb)
        return;

// BUGFIX 5/29/92 MSM - Changed to ad %02ld
    dw = lpdb->dwTotal;
    wsprintf( szTemp, "$%ld.%02ld", dw/100L, dw%100L );
    StatusSetStatBox( ghwndStat, 1, szTemp );

    dw = lpdb->dwSize;

    wsprintf( szTemp, "%ld.%02ldK", dw/1024L, ((dw%1024L)*100)/1024 );
    StatusSetStatBox( ghwndStat, 3, szTemp );
// BUGFIX 5/29/92 MSM - Changed to ad %02ld

} //*** UpdateStatLine


//*************************************************************
//
//  UpdateToolbar
//
//  Purpose:
//      Updates the status bars price and size
//
//
//  Parameters:
//      HWND hwndToolbar
//      LPDB lpdb
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/15/92   MSM        Created
//
//*************************************************************

VOID WINAPI UpdateToolbar( HWND hwndToolbar, LPDB lpdb )
{
    char szTemp[60];

    if (!lpdb)
        return;

    wsprintf( szTemp, "%d Fonts / %d Collections", lpdb->wFonts, lpdb->wColls );
    SendMessage( hwndToolbar, TBM_SETBTNTEXT, IDTB_SELECTED, (LONG)(LPSTR)szTemp );

} //*** UpdateToolbar


//*************************************************************
//
//  UpdateGroupingButtons
//
//  Purpose:
//      Sets the group buttons to their proper values
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/23/92   MSM        Created
//
//*************************************************************

VOID WINAPI UpdateGroupingButtons(VOID)
{
    if (glpDB->uView & DB_VIEW_PUBLISHERS)
        SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_GROUP_PUB, TRUE );
    else
        SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_GROUP_PUB, FALSE );

    if (glpDB->uView & DB_VIEW_FACES)
        SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_GROUP_FACE, TRUE );
    else
        SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_GROUP_FACE, FALSE );

} //*** UpdateGroupingButtons


//*************************************************************
//
//  UpdateSortingButtons
//
//  Purpose:
//      Sets the sorting buttons to their proper values
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/23/92   MSM        Created
//
//*************************************************************

VOID WINAPI UpdateSortingButtons(VOID)
{
    switch ( glpDB->uSort)
    {
        case DB_SORT_NAME:
            SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SORT_NAME, TRUE );
        break;

        case DB_SORT_PRICE:
            SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SORT_PRICE, TRUE );
        break;

        case DB_SORT_SIZE:
            SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SORT_SIZE, TRUE );
        break;

        case DB_SORT_PANOSE:
            SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SORT_PANOSE, TRUE );
        break;
    }

} //*** UpdateGroupingButtons


//*************************************************************
//
//  ReloadDatabase
//
//  Purpose:
//      Reloads the database
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/28/92   MSM        Created
//
//*************************************************************

BOOL WINAPI ReloadDatabase(VOID)
{
    WORD wCorrupt = 0;
    DB   db;

    db = *glpDB;

    SetCursor( LoadCursor(NULL,IDC_WAIT) );

    FreeDataBase( glpDB );

    SetWindowRedraw( ghwndELB, FALSE );

    GetDatabaseDir( gszDBPath );

load_db:
    //*** Load database
// BUGFIX #0009 5/23/92
    if (!(glpDB = LoadDataBase(gszDBPath, ghwndStat)))
    {
        switch (GetLoadError())
        {
            case IDS_DBCORRUPT:
                wCorrupt = 1;
                goto load_db;
            break;

            case IDS_MEMALLOC:
                ReportFontshopError( NULL, (LPSTR)IDS_MEMORYONLOAD, 
                    GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
                return FALSE;
            break;

            case IDS_FILENOTFOUND:
            case IDS_FILEOPENFAILED:
            case IDS_FILEREADFAILED:
                ReportFontshopError( NULL, (LPSTR)IDS_DISKERROR, 
                    GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
            break;

            case IDS_RENAMEFAILURE:
                ReportFontshopError( NULL, (LPSTR)IDS_RECOVERERROR, 
                    GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
            break;
        }
        //*** Attempt to recover by creating an empty database
        glpDB = CreateDataBase();
        lstrcpy( glpDB->szPathName, gszDBPath );
    }

    //*** any corrupt pieces found?
    if (glpDB->wDBType == DB_TYPE_CORRUPT)
        wCorrupt = 1;

    glpDB->wDBType = db.wDBType;
    glpDB->uView   = db.uView;  
    glpDB->uSort   = db.uSort;

    if (db.hFilter)
        glpDB->hFilter = &gFilter;

    if( glpDB->uSort == DB_SORT_PANOSE)
    {
        GetIniPanoseNumbers();
        DoPanoseNumbers();
    }

    ELBSetDatabase( ghwndELB, glpDB );
    SyncDBELBToSlider( ghwndELB, ghwndSlider );
    UpdateGroupingButtons();
    UpdateSortingButtons();
    if (glpDB->hFilter)
        SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SET_FILTER, TRUE );
    SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, FALSE );

    SetWindowRedraw( ghwndELB, TRUE );
    FillDBELB( ghwndELB );

    //*** any corrupt pieces found?
    if (wCorrupt)
    {
        ReportFontshopError( NULL, (LPSTR)IDS_CORRUPTWARN, 
            GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
    }

    SetFocus( ghwndELB );
    ELBSetCurSel( ghwndELB, 0 );

    SetCursor( LoadCursor(NULL,IDC_ARROW) );
    return TRUE;

} //*** ReloadDatabase

//*** EOF: mainwnd.c
