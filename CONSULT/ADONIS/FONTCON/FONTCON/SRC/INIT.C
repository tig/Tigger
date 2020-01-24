//*************************************************************
//  File name: init.c
//
//  Description: 
//      Initialization Routine
//
//  History:    Date       Author     Comment
//               2/13/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include "panose.h"

#include <ver.h>

extern char    **__argv ;
extern int     __argc ;

BOOL NEAR PASCAL GetExecutableVersions( VOID ) ;
WORD NEAR PASCAL GetVersionOfFile( LPSTR szFile ) ;
BOOL NEAR PASCAL DoCommandLine( VOID ) ;
VOID NEAR PASCAL GetSubscriptionPrice( VOID ) ;

//*************************************************************
//
//  InitApplication
//
//  Purpose:
//              Initializes the application (window classes)
//
//
//  Parameters:
//      HINSTANCE hInstance - hInstance from WinMain
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

BOOL InitApplication (HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
    wc.lpfnWndProc = MainWndProc;
                                 
    wc.cbClsExtra = 0;           
    wc.cbWndExtra = 0;           
    wc.hInstance = hInstance;    
    wc.hIcon = LoadIcon( hInstance, IDI_FSICON );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName  = GetRCString( IDS_MAINMENU, hInstance );  
    wc.lpszClassName = GetRCString( IDS_MAINCLASS, hInstance );

    if ( !RegisterClass(&wc) )
        return FALSE;

    wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
    wc.lpfnWndProc = FVWndProc;
                                 
    wc.cbClsExtra = 0;
    wc.cbWndExtra = FV_WNDEXTRA;
    wc.hInstance = hInstance;    
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZ_FONTVIEWCLASS;

    if ( !RegisterClass(&wc) )
        return FALSE;

    return TRUE;

} //*** InitApplication

//*************************************************************
//
//  InitInstance
//
//  Purpose:
//              Initializes each instance (window creation)
//
//
//  Parameters:
//      HINSTANCE hInstance
//      int nCmdShow
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    LPWINDOWPLACEMENT lpwp;
    HWND  hDlg;
    FARPROC lpProc;
    WORD    wCorrupt = 0;
    DWORD   dw;
    LPSTR   lp ;

    DoCommandLine() ;

    if (!InitializeFCDB())
    {
        ReportFontshopError( NULL, (LPSTR)IDS_FATALSTARTUP, GRCS(IDS_APPNAME),
            MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        return FALSE;
    }

    ghbmpToolbar = LoadBitmap( hInstance, IDB_TOOLBAR );

   ghbmpFontCon = LoadBitmap( hInstance, IDB_FONTCON );
   if (ghbmpFontCon)
   {
      bmColorTranslate( ghbmpFontCon, RGBLTMAGENTA, RGBWHITE, 0 ) ;
   }

    if (!ghbmpToolbar)
    {
        ReportFontshopError( NULL, (LPSTR)IDS_FATALSTARTUP, GRCS(IDS_APPNAME),
            MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        return FALSE;
    }

    GetDatabaseDir( gszDBPath );

    /* Get the path to our dir */
    GetModuleFileName( ghInst, gszExePath, 120 ) ;
    lp = gszExePath + lstrlen( gszExePath ) - 1;

    while (*lp != ':' && *lp != '\\' && lp > gszExePath)
       lp--;
    if (lp>gszExePath)
       *(++lp) = '\0';

    wsprintf( gszHelpFile, "%s%s", (LPSTR)gszExePath, GRCS( IDS_HELPFILE ) ) ;

    /*
     * This font is the "Helv 8, non-bold font" used in the status
     * bar and elsewhere.  We have to create this font before
     * we create the window, because we size things depending
     * on this font
     */
    ghfontSmall = ReallyCreateFontEx( NULL, "MS Sans Serif", NULL, 8, RCF_NORMAL );

    ghwndMain = CreateWindow( GRCS( IDS_MAINCLASS ),
                              GRCS( IDS_APPNAME ),
                              WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                              CW_USEDEFAULT, CW_USEDEFAULT, 
                              CW_USEDEFAULT, CW_USEDEFAULT, 
                              NULL, NULL, hInstance, NULL );

    if (!ghwndMain)
    {
        ReportFontshopError( NULL, (LPSTR)IDS_FATALSTARTUP, GRCS(IDS_APPNAME),
            MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        return FALSE;
    }

    lpwp = RestoreConfiguration();

// BUGFIX #0001 5/23/92
    if (lpwp)
    {
        SetWindowPlacement( ghwndMain, lpwp );
        if (nCmdShow!=SW_SHOWMINIMIZED)
        {
            nCmdShow = lpwp->showCmd;
            if (nCmdShow==SW_SHOWMINIMIZED)
                nCmdShow = SW_SHOWNORMAL;
        }
    }
// BUGFIX #0001 5/23/92

    //*** Show Main window
    ShowWindow( ghwndMain, nCmdShow );

    //*** Bring up about dialog
    lpProc = MakeProcInstance( (FARPROC)AboutBoxProc, ghInst );

    hDlg = CreateDialog(ghInst,ID( ABOUT_DLG ),ghwndMain,(DLGPROC)lpProc);
    if (hDlg)
    {
        //*** Disable main window
        EnableWindow( ghwndMain, NULL );

        //*** Hide OK button
        ShowWindow( GetDlgItem( hDlg, IDCANCEL ), SW_HIDE );
        UpdateWindow( hDlg );
    }
    UpdateWindow( ghwndMain );


    if (!GetExecutableVersions())
    {
#pragma message( "NEED : Better message box" )
        ReportFontshopError( ghwndMain, (LPSTR)IDS_FATALSTARTUP, GRCS(IDS_APPNAME),
            MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        return FALSE;
    } 

load_db:
    //*** Load database
// BUGFIX #0009 5/23/92
    if (!(glpDB = LoadDataBase(gszDBPath, ghwndStat)))
    {
        switch (GetLoadError())
        {
            case IDS_DBCORRUPT:
                wCorrupt++;
                if (wCorrupt<=500)      // Only try 500 times!!
                    goto load_db;
            break;

            case IDS_DBNOTFOUND:
                ReportFontshopError( ghwndMain, (LPSTR)IDS_NODB, 
                    GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
            break;

            case IDS_MEMALLOC:
                ReportFontshopError( ghwndMain, (LPSTR)IDS_MEMORYONLOAD, 
                    GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
                return FALSE;
            break;

            case IDS_FILENOTFOUND:
            case IDS_FILEOPENFAILED:
            case IDS_FILEREADFAILED:
                ReportFontshopError( ghwndMain, (LPSTR)IDS_DISKERROR, 
                    GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
            break;

            case IDS_RENAMEFAILURE:
                ReportFontshopError( ghwndMain, (LPSTR)IDS_RECOVERERROR, 
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

    dw = GetPrivateProfileDWORD( GRCS(IDS_CONFIG), GRCS(IDS_VIEW), 
        MAKELONG(DB_SORT_NAME,DB_VIEW_PUBLISHERS|DB_VIEW_STYLES), GRCS(IDS_INIFILE) );

    glpDB->wDBType = DB_TYPE_FONTSHOP;
    glpDB->uView = HIWORD(dw)&0x7FFF;
    glpDB->uSort = LOWORD(dw);

    if( glpDB->uSort == DB_SORT_PANOSE)
    {
        GetIniPanoseNumbers();
        DoPanoseNumbers();
    }

    if( dw & 0x80000000L )
    {
        GetIniFilter();
        glpDB->hFilter = &gFilter;
    }

    ELBSetDatabase( ghwndELB, glpDB );
    SyncDBELBToSlider( ghwndELB, ghwndSlider );
    UpdateGroupingButtons();
    UpdateSortingButtons();
    if (glpDB->hFilter)
        SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SET_FILTER, TRUE );
    SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_DOWNLOAD, FALSE );

    FillDBELB( ghwndELB );

    GetCustInfo( &gCustInfo, GRCS( IDS_APPNAME ) ) ;

    //*** Get rid of about box
    if (hDlg)
    {
        EnableWindow( ghwndMain, TRUE );
        DestroyWindow( hDlg );
    }
    FreeProcInstance( lpProc );

    //*** any corrupt pieces found?
    if (wCorrupt)
    {
        ReportFontshopError( ghwndMain, (LPSTR)IDS_CORRUPTWARN, 
            GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
    }

    SetFocus( ghwndELB );
    ELBSetCurSel( ghwndELB, 0 );

    /* Get the subscription price for fontcon.  This can be found
     * in one of two places...a file called FCBUNDLE.BIN or
     * as SUBSCRIPT_PRICE
     */
    GetSubscriptionPrice() ;

    /* Is he a demo user?
     */
    switch( GetPrivateProfileWORD( GRCS( IDS_CONFIG ),
                                   GRCS( IDS_INI_STATUS ),
                                   0,
                                   GRCS( IDS_INIFILE ) ))
   {
      case 42:
         DP1( hWDB, "Active User" ) ;
         gfDemoUser = FALSE ;
      break ;

      case 43:
         DP1( hWDB, "Expired User" ) ;
         gfDemoUser = FALSE ;
         gfExpiredUser = TRUE ;
      break ;
       
      default:
         DP1( hWDB, "Demo User" ) ;
         gfDemoUser = TRUE ;
      break ;
   }

   if (gfAutoLogon || gfRestarted)
      SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_UPDATE, FALSE );
   else
      SendMessage( ghwndToolbar, TBM_ENABLEBTN, IDM_ACTION_UPDATE, TRUE );

    DP5( hWDB, "Initialed Instance" );
    return (TRUE);

} //*** InitInstance


//*************************************************************
//
//  CreateStatus
//
//  Purpose:
//      Creates the status bar
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND WINAPI PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

HWND WINAPI PASCAL CreateStatus( HWND hwndParent, UINT uID )
{
    HWND hwndStat;
    LONG lNumStats = 4;
    WORD wW;

    DP5( hWDB, "CreateStatus" ) ;

    if (!(hwndStat = CreateWindow( SZ_STATUSCLASSNAME, "",
                WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS, 0,0,1,1, 
                hwndParent, (HMENU)uID, ghInst, (LPSTR)&lNumStats )))
    {
        DP1( hWDB, "Stat create failed!" ) ;
        return 0 ;
    }

    SendMessage( hwndStat, WM_SETFONT, (WPARAM)ghfontSmall, (LPARAM)TRUE );

    wW = StatusGetWidth( hwndStat,GRCS( IDS_SAMPPRICE ));
    
    StatusSetStatBoxSize( hwndStat, 0, 
        StatusGetWidth( hwndStat,GRCS( IDS_STATTOTAL )) );
    StatusSetStatBox( hwndStat, 0, GRCS( IDS_STATTOTAL ) );
    SendMessage( hwndStat, ST_SETBORDER, 0, 0L );
    SendMessage( hwndStat, ST_SETJUSTIFY, 0, DT_RIGHT );

    StatusSetStatBoxSize( hwndStat, 1,
        StatusGetWidth( hwndStat, "$XXXX.XX " ) );
    StatusSetStatBox( hwndStat, 1, "$0.00" );
    SendMessage( hwndStat, ST_SETJUSTIFY, 1, DT_LEFT );

    StatusSetStatBoxSize( hwndStat, 2,
        StatusGetWidth( hwndStat,GRCS( IDS_STATSIZE )) );
    StatusSetStatBox( hwndStat, 2, GRCS( IDS_STATSIZE ) );
    SendMessage( hwndStat, ST_SETBORDER, 2, 0L );
    SendMessage( hwndStat, ST_SETJUSTIFY, 2, DT_RIGHT );

    StatusSetStatBoxSize( hwndStat, 3,
        StatusGetWidth( hwndStat, "XXXX.XXK " ) );
    StatusSetStatBox( hwndStat, 3, "0.00K" );
    SendMessage( hwndStat, ST_SETJUSTIFY, 3, DT_LEFT );

    return hwndStat;

} //*** CreateStatus


//*************************************************************
//
//  CreateSlider
//
//  Purpose:
//      Creates the slider bar
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

HWND FAR PASCAL CreateSlider( HWND hwndParent, UINT uID )
{
    HWND  hwndSlider ;

    DP5( hWDB, "CreateSlider" );

    if (!(hwndSlider = CreateWindow( SZ_SLIDERCLASS, "", 
                WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | SLS_3DLOOK_OUT,
                0, 0, 1, 1,
                hwndParent, (HMENU)uID, ghInst, NULL )))
    {
        DP1( hWDB, "Slider create failed!" );
        return NULL;
    }

    SendMessage( hwndSlider, WM_SETFONT, (WPARAM)ghfontSmall, (LPARAM)TRUE );

    SendMessage( hwndSlider, SL_SETNUMCOLS, 4, 0L );
    SendMessage( hwndSlider, SL_SETCOLHEADER, NAME_COL, (LONG)GRCS(IDS_SLDRNAME) );
    SendMessage( hwndSlider, SL_SETCOLHEADER, PRICE_COL, (LONG)GRCS(IDS_SLDRPRICE) );
    SendMessage( hwndSlider, SL_SETCOLHEADER, SIZE_COL, (LONG)GRCS(IDS_SLDRSIZE) );
    SendMessage( hwndSlider, SL_SETCOLHEADER, PUB_COL, (LONG)GRCS(IDS_SLDRPUBLISHER) );

    SendMessage( hwndSlider, SL_SETCOLPOS, NAME_COL, (LONG)0L );
    SendMessage( hwndSlider, SL_SETCOLPOS, PRICE_COL, (LONG)40L );
    SendMessage( hwndSlider, SL_SETCOLPOS, SIZE_COL, (LONG)60L );
    SendMessage( hwndSlider, SL_SETCOLPOS, PUB_COL, (LONG)80L );

    SendMessage( hwndSlider, SL_SETCOLALIGN, NAME_COL, (LONG)SA_CENTER );

    return hwndSlider;

} //*** CreateSlider


//*************************************************************
//
//  CreateELB
//
//  Purpose:
//      Creates the ExtendedListbox
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 2/92   MSM        Created
//
//*************************************************************

HWND WINAPI CreateELB( HWND hwndParent, UINT uID )
{
    HWND hwndELB;

    hwndELB = CreateWindow( SZ_ELBCLASS, NULL, WS_VISIBLE|WS_CHILD|WS_BORDER,
            0, 0, 10, 10, hwndParent, (HMENU)uID, ghInst, NULL );

    if (hwndELB)
        ELBSetFont( hwndELB, ghfontUser );
    else
    {
        DP1( hWDB, "ELB failed to create!" );
        return NULL;
    }
    return hwndELB;

} //*** CreateELB


//*************************************************************
//
//  CreateToolbar
//
//  Purpose:
//      Creates the ExtendedListbox
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 2/92   MSM        Created
//
//*************************************************************

HWND WINAPI CreateToolbar( HWND hwndParent, UINT uID )
{
    HWND hwndToolbar;

    hwndToolbar = CreateWindow( SZ_TOOLBARCLASS, NULL, WS_VISIBLE|WS_CHILD|WS_BORDER,
            0, 0, 10, 10, hwndParent, (HMENU)uID, ghInst, NULL );

    if (!hwndToolbar)
    {
        DP1( hWDB, "Toolbar failed to create!" );
        return NULL;
    }
    else
    {
        DIMENS  dm;
        BTNINFO bi;

        dm.nSpacerX = 10;
        dm.nButtonX = 22;
        dm.nButtonY = 20;
        SendMessage( hwndToolbar, TBM_SETDIMENS,0, (LONG)(LPSTR)&dm );

        bi.uiBtnID=IDM_ACTION_UPDATE;
        bi.uiBtnStyle=BTNS_MENUID;
        bi.uiBtnState=BTN_UP|BTN_ENABLED;
        bi.hBitmap = ghbmpToolbar;
        bi.nBmpX = 0;
        bi.nBmpY = 0;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=IDM_ACTION_DOWNLOAD;
        bi.nBmpY = 40;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        SendMessage( hwndToolbar, TBM_ADDBUTTON, 0, (LONG)NULL );

        bi.uiBtnID=IDM_SORT_NAME;
        bi.uiBtnStyle=BTNS_TOGGLEEX|BTNS_MENUID;
        bi.nBmpY = 80;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=IDM_SORT_PRICE;
        bi.nBmpY = 120;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=IDM_SORT_SIZE;
        bi.nBmpY = 160;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=IDM_SORT_PANOSE;
        bi.nBmpY = 200;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        SendMessage( hwndToolbar, TBM_ADDBUTTON, 0, (LONG)NULL );

        bi.uiBtnID=IDM_SET_FILTER;
        bi.uiBtnStyle=BTNS_TOGGLE|BTNS_MENUID;
        bi.nBmpY = 240;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        SendMessage( hwndToolbar, TBM_ADDBUTTON, 0, (LONG)NULL );

        bi.uiBtnID=IDM_GROUP_PUB;
        bi.uiBtnStyle=BTNS_MENUID|BTNS_TOGGLE;
        bi.nBmpX = 0;
        bi.nBmpY = 320;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=IDM_GROUP_FACE;
        bi.uiBtnStyle=BTNS_MENUID|BTNS_TOGGLE;
        bi.nBmpY = 280;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

// RIGHT BUTTONS
        // Cheat and use status calc routines since they use the same font

        bi.uiBtnID=(UINT)-1;
        bi.uiBtnStyle=BTNS_TEXT|BTNS_RIGHTBTN;
        bi.hBitmap = (HBITMAP)ghfontSmall;
        bi.nBmpX = StatusGetWidth( ghwndStat, " Selected: ");
        bi.nBmpY = BTNJ_RIGHT;
        bi.dwData = (LONG)(LPSTR)"Selected:";
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=0;
        bi.nBmpX = 2;
        bi.uiBtnStyle=BTNS_RIGHTBTN;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );

        bi.uiBtnID=IDTB_SELECTED;
        bi.uiBtnStyle=BTNS_TEXTBORDER|BTNS_RIGHTBTN;
        bi.hBitmap = (HBITMAP)ghfontSmall;
        bi.nBmpX = StatusGetWidth( ghwndStat, "999 Fonts / 99 Collections");
        bi.nBmpY = BTNJ_LEFT;
        bi.dwData = 0L;
        SendMessage( hwndToolbar, TBM_ADDBUTTON,0, (LONG)(LPSTR)&bi );
// RIGHT BUTTONS

    }
    return hwndToolbar;

} //*** CreateToolbar


//*************************************************************
//
//  CreateFontView
//
//  Purpose:
//      Creates the Fontview
//
//
//  Parameters:
//      HWND hwndParent
//      UINT uID
//      
//
//  Return: (HWND FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 2/92   MSM        Created
//
//*************************************************************

HWND WINAPI CreateFontView( HWND hwndParent, UINT uID )
{
    HWND hwndFontView;

    hwndFontView = CreateWindow( SZ_FONTVIEWCLASS, NULL, WS_VISIBLE|WS_CHILD|WS_BORDER,
            0, 0, 10, 10, hwndParent, (HMENU)uID, ghInst, NULL );

    if (!hwndFontView)
    {
        DP1( hWDB, "FontView failed to create!" );
        return NULL;
    }
    return hwndFontView;

} //*** CreateFontView

WORD NEAR PASCAL GetVersionOfFile( LPSTR lpszFile )
{
   DWORD             dwHandle ;
   UINT              uiSize ;
   BYTE              rgData[512] ;
   VS_FIXEDFILEINFO FAR * lpFixedFileInfo ;

   uiSize =(UINT)GetFileVersionInfoSize( lpszFile, &dwHandle ) ;

   if (uiSize == 0)
      return 0 ;

   GetFileVersionInfo( lpszFile, dwHandle, uiSize, rgData ) ;
   VerQueryValue( rgData, "\\", (VOID FAR* FAR*)&lpFixedFileInfo, &uiSize ) ;

   if (lpFixedFileInfo)
   {
      DP3( hWDB, "%s %d.%02d.%03d (File Version %03d)",
                (LPSTR)lpszFile,
                HIWORD(lpFixedFileInfo->dwFileVersionMS), 
                LOWORD(lpFixedFileInfo->dwFileVersionMS), 
                HIWORD(lpFixedFileInfo->dwFileVersionLS),
                LOWORD(lpFixedFileInfo->dwFileVersionLS)
               ) ;
      return LOWORD( lpFixedFileInfo->dwFileVersionLS ) ;
   }
   else
      return 0 ;
}

/****************************************************************
 *  BOOL NEAR PASCAL GetExecutableVersions( VOID )
 *
 *  Description: 
 *   Get a list of the version numbers of each of our system
 *   components into grgOurExeVer[].  The version info is pulled out
 *   of the version resources in the exe files.  The fileversion
 *   is stored in Word4 of the 64 bit version field.
 *
 ****************************************************************/
BOOL NEAR PASCAL GetExecutableVersions( VOID )
{
   char              szFile[144] ;
   char              szBuf[81] ;
   PSTR              psz ;
   WORD              wExeID ;

   for (wExeID = 0 ; wExeID < NUM_EXECUTABLES ; wExeID ++ )
   {
      /* We have two special case files: MODEM.INI and
       * FCHELP.HLP.
       */
      if (wExeID == EXE_MODEMINI || wExeID == EXE_HELPFILE)
      {
         HFILE hFile ;
         OFSTRUCT of ;

         if (wExeID == EXE_HELPFILE)
         {
            /* For the helpfile, the version info is stored in
             * a file named FCHELP.BIN.  If this file does not
             * exist the version number will be 1
             */
             wsprintf( szFile, "%sFCHELP.BIN", (LPSTR)gszExePath ) ;
         }
         else
            wsprintf( szFile, "%s%s", (LPSTR)gszExePath, (LPSTR)GRCS( wExeID + IDS_EXECUTABLES ) ) ;

         /* Version number is on the first line which has the form:
          * ; Version 000
          */
         if (HFILE_ERROR != (hFile = OpenFile( szFile, &of, OF_READ )))
         {
            _lread( hFile, szBuf, 80 ) ;

            if (psz = strchr( szBuf, 'n' )) // the 'n' in Version
               grgOurExeVer[wExeID] = AtoI( psz + 2 ) ;

            _lclose( hFile ) ;
         }
         else
            if (wExeID == EXE_HELPFILE)
            {
               DP( hWDB, "VER_HELPFILE = %d", VER_HELPFILE ) ;
               grgOurExeVer[wExeID] = VER_HELPFILE ;
            }

         DP3( hWDB, "%s (File Version %03d)",
               (LPSTR)szFile, grgOurExeVer[wExeID] ) ;
         continue ;
      }

      wsprintf( szFile, "%s%s", (LPSTR)gszExePath, (LPSTR)GRCS( wExeID + IDS_EXECUTABLES ) ) ;
      if (!(grgOurExeVer[wExeID] = GetVersionOfFile( szFile )))
      {
         DP1( hWDB, "Failed to get version info for %s (%d)",
                    (LPSTR)GRCS( wExeID + IDS_EXECUTABLES ),
                    wExeID ) ;

         return FALSE ;
      }

   }

   return TRUE ;

} /* GetExecutableVersions()  */
    
    
/****************************************************************
 *  BOOL WINAPI DoCmdLine( VOID )
 *
 *  Description: 
 *
 *    Processes the command line.
 *
 ****************************************************************/
BOOL NEAR PASCAL DoCommandLine( VOID )
{
   int      i ;
   PSTR     pOpt ;

   for (i = 1 ; i < __argc ; i++)
   {
      DP4( hWDB, "__argv[%d] = '%s'", i, (LPSTR)__argv[i] ) ;
      if ((*__argv[i] == '/') || (*__argv[i] == '-'))
      {
         pOpt = __argv[i] + 1 ;

         if (*pOpt)
         {
            switch ( *pOpt )
            {
               case 'r':      // Restarting, don't ask if he wants to
               case 'R':      // logon.
                  DP1( hWDB, "Restarted switch specified!" ) ;
                  gfRestarted = TRUE ;
               break ;

               case 'a':      // Auto logon.  Update Local database
               case 'A':      // and exit!
                  DP1( hWDB, "AutoLogon switch specified!" ) ;
                  gfAutoLogon = TRUE ;
               break ;
            }
         }
      }
   }

   return TRUE ;
} /* DoCmdLine()  */


/****************************************************************
 *  VOID NEAR PASCAL GetSubscriptionPrice( VOID )
 *
 *  Description: 
 *
 *    Get the subscription price for fontcon.  This can be found
 *    in one of two places...a file called FCBUNDLE.BIN or
 *    in the #define SUBSCRIPT_PRICE
 *
 ****************************************************************/
VOID NEAR PASCAL GetSubscriptionPrice( VOID )
{
   HFILE hFile ;
   OFSTRUCT of ;
   char     szFile[144] ;

   gdwSubscriptPrice = 0 ;

   wsprintf( szFile, "%sFCBUNDLE.BIN", (LPSTR)gszExePath ) ;
   if (HFILE_ERROR != (hFile = OpenFile( szFile, &of, OF_READ )))
   {
      _lread( hFile, szFile, 64 ) ;

      wsscanf( szFile, "%08lX %08lX %08lX", (DWORD FAR*)&gdwBundleID,
            (DWORD FAR*)&gdwSubscriptPrice,
            (DWORD FAR*)&gdwRenewalPrice ) ;

      /* To hide the price from spies it is XOR'd with 0x09876543.
       */
      gdwSubscriptPrice = gdwSubscriptPrice ^ 0x09876543 ;

      gdwRenewalPrice = gdwRenewalPrice ^ 0x09876543 ;

      _lclose( hFile ) ;
   }

   if (gdwSubscriptPrice == 0)
      gdwSubscriptPrice = SUBSCRIPT_PRICE ;
   else
      gfDemoDatabase = TRUE ;             // IMPORTANT!

   if (gdwRenewalPrice == 0)
      gdwRenewalPrice = RENEWAL_PRICE ;

   DP1( hWDB, "Bundle ID = %08lX", gdwBundleID ) ;

   DP1( hWDB, "Subscription Price is $%ld.%02ld", 
                      (DWORD)gdwSubscriptPrice/100L,
                      (DWORD)gdwSubscriptPrice%100L ) ;

   DP1( hWDB, "Renewal Price is $%ld.%02ld", 
                      (DWORD)gdwRenewalPrice/100L,
                      (DWORD)gdwRenewalPrice%100L ) ;

   DP1( hWDB, "Using the %s database", (LPSTR)(gfDemoDatabase ?
      "Demo" : "Real") ) ;

} /* GetSubscriptionPrice()  */

//*** EOF: init.c
