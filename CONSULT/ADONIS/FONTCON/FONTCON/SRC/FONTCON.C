//*************************************************************
//  File name: FONTCON.c
//
//  Description: 
//      Stub program to simulate Control panel for FSCPL.DLL
//
//  History:    Date       Author     Comment
//               2/14/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include "elsemap.h"

HINSTANCE   ghInst;
HFONT       ghfontSmall;
HFONT       ghfontUser;
HWND        ghwndMain;
HWND        ghwndStat;
HWND        ghwndSlider;
HWND        ghwndELB;
HWND        ghwndToolbar;
HWND        ghwndFontView;
HWND        ghwndPanoseDlg=NULL;
char        gszDBPath[144];
char        gszExePath[144];
char        gszDestPath[144] ;
char        gszHelpFile[144] ;
BOOL        gfInstallFonts = TRUE ;
HBITMAP     ghbmpToolbar=NULL;
HBITMAP     ghbmpFontCon=NULL;
BOOL        gfRestarted = FALSE ;      // TRUE if we've been restarted
BOOL        gfAutoLogon = FALSE ;      // TRUE if we're going to auto logon
BOOL        gfDemoDatabase = FALSE ;   // TRUE if we have the demo database

LPDB        glpDB = NULL;
ELSE_MAP_STATE gElseMapState;

CUSTINFO     gCustInfo ;

WORD         grgOurExeVer[NUM_EXECUTABLES] ; 
WORD         grgBBSExeVer[NUM_EXECUTABLES] ;

BOOL         gfDBChecked = FALSE ;

BOOL         gfUpShop = FALSE ;        // TRUE if we need to WinExec()
                                       // CONXUP.MOD when we're done.

DWORD       gdwSubscriptPrice ;        // If we are a "bundle"  then
                                       // this is taken out of FCBUNDLE.BIN
                                       // otherwise it's 6995

DWORD       gdwRenewalPrice ;          // If we are a "bundle"  then
                                       // this is taken out of FCBUNDLE.BIN
                                       // otherwise it's 3995

BOOL        gfDemoUser = TRUE ;        // TRUE until he has a user ID *and*
                                       // the Status= entry in the [TFC] section
                                       // equals 42 (or 0x1A)

DWORD       gdwBundleID = 0 ;          // Read from FCBUNDLE.BIN


BOOL        gfExpiredUser = FALSE ;    // The guy has expired.  the Status=
                                       // entry will read 43 (or 0x1B) until
                                       // he renews.

//*************************************************************
//
//  WinMain
//
//  Purpose:
//		Entry point for all windows apps
//
//
//  Parameters:
//      HINSTANCE hInstance
//      HINSTANCE hPrevInstance
//      LPSTR lpCmdLine
//      int nCmdShow
//      
//
//  Return: (int PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/12/91   MSM        Created
//
//*************************************************************

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL haccl ;

    #ifdef DEBUG
    {
        char szVersion[64] ;

        // Install debuging stuff
        D( hWDB = wdbOpenSession( NULL, "FONTCON", "wdb.ini", WDB_LIBVERSION ) ) ;

        if ( VER_BUILD )
            wsprintf( szVersion, (LPSTR)"%d.%.2d.%.3d",
                        VER_MAJOR, VER_MINOR, VER_BUILD ) ;
        else
            wsprintf( szVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

        DP1( hWDB, "\n**** %s %s %s (Windows Version %d.%d)",
                    (LPSTR)"FONTCON",
                    (LPSTR)"Version",
                    (LPSTR)szVersion,
                    (WORD)LOBYTE(LOWORD(GetVersion())), 
                    (WORD)HIBYTE(LOWORD(GetVersion())) ) ;

        if (sizeof(FSORDER) != sizeof(ORDER))
        {
           DP1( hWDB, "Sizeof FSORDER struct != sizeof ORDER struct!!!" ) ;
           return 0 ;
        }
    }
    #endif

    /*
     * If there is already an instance running, bring it to the
     * foreground and close this one.
     *
     * We use the FindWindow method because hPrevInst is always NULL
     * in Windows NT.
     *
     * This method also gets around the bug in Windows 3.1 where
     * an app that was launched from within an OLE server always has
     * hPrevInst != 0.  (i.e. Drop WINFILE.EXE into a Write document,
     * and double click on the icon.  If there is already a copy of
     * WinFile running, another one will come up even though WinFile
     * is supposedly single instance!!!).
     */
    if (ghwndMain = FindWindow( GetRCString(IDS_MAINCLASS,hInstance), NULL ))
    {
      HWND hwnd2 ;

      DP1( hWDB, "**** Previous Instance exists!  Activating it and closing this one.\n" ) ;

      hwnd2 = GetLastActivePopup( ghwndMain ) ;

      //
      // If it's minimized, restore it.
      //
      if (ghwndMain == hwnd2 && IsIconic( ghwndMain ))
         SendMessage(ghwndMain, WM_SYSCOMMAND, SC_RESTORE, 0) ;

      BringWindowToTop(hwnd2) ;
      SetActiveWindow(hwnd2) ;

      return 0;
    }

    ghInst = hInstance;

    //*** Map init will alway suceed
    bELSEMapInit(&gElseMapState);

    //*** Set threshold to highest limit and then relax it
    bELSESetMapThreshold(&gElseMapState, (UINT)65535);
    vELSERelaxThreshold(&gElseMapState);

    //*** Set prices and sizes to -1 (not used)
    _fmemset( (LPSTR)&gFilter, 0xFFFF, sizeof(FILTER) );
    gFilter.szPubName[0] = 0;
    gFilter.szFontName[0] = 0;

    if (!InitApplication(hInstance))
    {
        ReportFontshopError( NULL, (LPSTR)IDS_FATALSTARTUP, GRCS(IDS_APPNAME),
            MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        return FALSE;
    }

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

   /* Load keyboard accellerators (F1=Help)
    */
   haccl = LoadAccelerators( hInstance, MAKEINTRESOURCE( 1 ) ) ;

    /* Ask the user if he wants to connect now.
     */
    PostMessage( ghwndMain, WM_COMMAND, IDM_ACTION_UPDATE, 0L ) ;

    DP5( hWDB, "Enter message loop" );
    while (GetMessage(&msg, NULL, NULL, NULL))
    {
      if (!ghwndPanoseDlg || !IsDialogMessage(ghwndPanoseDlg,&msg))
      {
         if (!TranslateAccelerator( ghwndMain, haccl, &msg ))
         {
            TranslateMessage(&msg);      
            DispatchMessage(&msg);
         }
      }
    }

    if (ghfontUser != GetStockObject( SYSTEM_FONT ))
        DeleteObject( ghfontUser );
    DeleteObject( ghfontSmall );

    if (ghbmpToolbar)
        DeleteObject( ghbmpToolbar );

    if (ghbmpFontCon)
        DeleteObject( ghbmpFontCon );


    if (glpDB)
        FreeDataBase( glpDB );

    vELSEMapClose(&gElseMapState);

   if (gfUpShop == TRUE)
   {
      /*
       * If during this run we updated any executables we need
       * to run UpShop to copy them.
       */
      WinExec( GRCS( IDS_EXECUTABLES + EXE_CONXUP ), SW_SHOWNORMAL ) ;
   }

   /*
    * Post a registered message to all other apps saying we're
    * shutting down. If ShopSrv is running it will see this
    * message and know that it should quit.
    *
    * If we started UpShop, it will see this too and know that
    * it's cool to over write our executables.
    */
   PostMessage( HWND_BROADCAST, RegisterWindowMessage( GRCS( IDS_CONXSRV ) ),
                0, 0L ) ;

    return (msg.wParam);      

} //*** WinMain


//*************************************************************
//
//  GetDatabaseDir
//
//  Purpose:
//      Retrieves the database directory
//
//
//  Parameters:
//      LPSTR lpBuff
//      
//
//  Return: (LPSTR WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               4/30/92   MSM        Created
//
//*************************************************************

LPSTR WINAPI GetDatabaseDir( LPSTR lpBuff )
{
    LPSTR lp;

    GetModuleFileName( ghInst, lpBuff, 120 );
    lp = lpBuff + lstrlen( lpBuff ) - 1;

    while (*lp != ':' && *lp != '\\' && lp>lpBuff)
        lp--;
    if (lp>lpBuff)
        *(++lp) = '\0';

    lstrcat( lpBuff, "DATABASE\\" );
    return lpBuff;

} //*** GetDatabaseDir



//*************************************************************
//
//  ViewFontshopHelp
//
//  Purpose:
//      Views a help item for FontShopper
//
//
//  Parameters:
//      WORD wTopic
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               5/ 2/92   MSM        Created
//
//*************************************************************

BOOL ViewFontshopHelp( WORD wTopic )
{
    return WinHelp( ghwndMain,gszHelpFile,HELP_KEY,(DWORD)(LPSTR)GRCS( wTopic ) );

} //*** ViewFontshopHelp

//*** EOF: fontshop.c

