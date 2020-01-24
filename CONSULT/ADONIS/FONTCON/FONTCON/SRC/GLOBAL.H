//*************************************************************
//  File name: global.h
//
//  Description: 
//      Header for the all things that are global to the project
//
//  History:    Date       Author     Comment
//               2/11/92   MSM        Created
//
//*************************************************************


#ifdef RC_INVOKED
    #define ID(id)  id
#else
    #define ID(id)  MAKEINTRESOURCE(id)
#endif

#include "mainwnd.h"
#include "fontview.h"
#include "res.h"
#include "..\..\inc\version.h"
#include "bbs.h"


/* IMPORTANT!!!!
 *
 * The following #define dictates the price of a fontcon
 * subscription.
 */
#define SUBSCRIPT_PRICE    2995L    // $29.95
#define RENEWAL_PRICE      3500L    // $35.00
#define SHIPPING_PRICE     600L     // $6.00


#define TOOLBAR_HEIGHT  25

//*** Macros
    #define GRCS(X) GetRCString( X, ghInst )

//*** Function prototypes
//*** fontshop.c
    LPSTR WINAPI GetDatabaseDir( LPSTR );
    BOOL ViewFontshopHelp( WORD wTopic );

//*** config.c
    VOID WINAPI SaveConfiguration(VOID);
    LPWINDOWPLACEMENT WINAPI RestoreConfiguration(VOID);
    BOOL WINAPI DoChooseFont(VOID);
    VOID WINAPI GetIniPanoseNumbers(VOID);
    VOID WINAPI SetIniPanoseNumbers(VOID);

    VOID WINAPI GetIniFilter(VOID);
    VOID WINAPI SetIniFilter(VOID);
    BOOL WINAPI VerifyDestDir( HWND hDlg, PSTR psz ) ;

//*** Coll.c
    BOOL FAR PASCAL CollectionsProc(HWND,UINT,WPARAM,LPARAM);
    LONG CALLBACK ELBPaintProc(LPES lpes,HDC hDC,LPELBNODE lpen,LPRECT lprc);
    LONG CALLBACK ELBSortProc(HWND hELB,LPELBNODE lpen1, LPELBNODE lpen2);

//*** init.c
    BOOL InitApplication (HINSTANCE);
    BOOL InitInstance (HINSTANCE, int);
    HWND WINAPI PASCAL CreateStatus( HWND hwndParent, UINT uID );
    HWND WINAPI PASCAL CreateSlider( HWND hwndParent, UINT uID );
    HWND WINAPI CreateELB( HWND hwndParent, UINT uID );
    HWND WINAPI CreateFontView( HWND hwndParent, UINT uID );
    HWND WINAPI CreateToolbar( HWND hwndParent, UINT uID );

//*** pubinfo.c
    LPPUB FAR PASCAL LoadPubInfo( LPDB, LPSTR );


//*** Global Variables
    extern HINSTANCE    ghInst;
    extern HFONT        ghfontSmall;
    extern HFONT        ghfontUser;
    extern HWND         ghwndMain;
    extern HWND         ghwndStat;
    extern HWND         ghwndSlider;
    extern HWND         ghwndELB;
    extern HWND         ghwndToolbar;
    extern HWND         ghwndFontView;
    extern HWND         ghwndPanoseDlg;
    extern char         gszDBPath[144];
    extern char         gszExePath[144];
    extern char         gszDestPath[144] ;
    extern char         gszHelpFile[144] ;
    extern BOOL         gfInstallFonts ;
    extern HBITMAP      ghbmpToolbar;
    extern HBITMAP      ghbmpFontCon;
                   
    extern LPDB         glpDB;

    extern PANOSE       gPanose;
    extern WORD         gwaWeights[10];

    extern FILTER       gFilter;

    extern CUSTINFO     gCustInfo ;

/*
 * Define the number of updatable executables in the product:
 *
 *    FONTCON.EXE
 *    CONXUP.EXE
 *    CONXSRV.EXE
 *    CONXLIB.DLL
 *    FCDB.DLL
 *    CONXMEM.DLL
 *    CONXPRO.DLL  (or whatever it's called).
 *    MODEM.INI (first line contains version info)
 *    FCHELP.HLP (version info is stored in FCHELP.BIN)
 *
 * The stringtable for FONTSHOP.EXE contains the name of
 * the executable that belongs to an EXE_* ID at string
 * table ID # IDS_EXECUTABLES + EXE_.
 */
#define EXE_FONTSHOP       0
#define EXE_CONXUP         1
#define EXE_SHOPSRV        2
#define EXE_SHOPLIB        3
#define EXE_FCDB           4
#define EXE_FSMEMMAN       5
#define EXE_ZMODEM         6
#define EXE_MODEMINI       7
#define EXE_HELPFILE       8

#define NUM_EXECUTABLES    9

    extern WORD         grgOurExeVer[NUM_EXECUTABLES] ;
    extern WORD         grgBBSExeVer[NUM_EXECUTABLES] ;

    extern BOOL         gfUpShop ;
    extern BOOL         gfRestarted ;
    extern BOOL         gfAutoLogon ;
    extern BOOL         gfDemoDatabase ;
    extern DWORD        gdwSubscriptPrice ;
    extern DWORD        gdwRenewalPrice ;          
    extern BOOL         gfDemoUser ;
    extern DWORD        gdwBundleID ;
    extern BOOL         gfExpiredUser ;
    extern BOOL         gfDBChecked ;

//*** EOF: global.h
