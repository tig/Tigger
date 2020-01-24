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

#define STRICT
#define WIN31
#define _WINDLL
#define WINVER 0x030a

#include <windows.h>
#include <windowsx.h>
#include "..\..\inc\wdb.h"

#ifdef RC_INVOKED
    #define ID(id)  id
#else
    #define ID(id)  MAKEINTRESOURCE(id)
#endif

#include "..\..\inc\struct.h"
#include "..\..\inc\conxlib.h"
#include "..\..\inc\fcdb.h"
#include "..\..\inc\conxmem.h"
#include "mainwnd.h"
#include "fontview.h"
#include "res.h"
#include "version.h"

//*** Macros
    #define GRCS(X) GetRCString( X, ghInst )

//*** global defines
    #define MB_ERR  (MB_ICONSTOP|MB_OK)
    #define MB_WARN (MB_ICONINFORMATION|MB_OKCANCEL)

    //*** Extra space to use when creating the thumb
    #define THUMB_PAD   4

//*** Function prototypes
//*** bitmap.c
    BOOL    WINAPI SaveBitmapFile( LPSTR, HBITMAP );
    HBITMAP WINAPI DBMakeThumb( LPFACE, WORD );
                   #define MT_DISPLAY   1
                   #define MT_SAVERLE4  4
                   #define MT_SAVERLE8  8

//*** config.c
    VOID WINAPI SaveConfiguration(VOID);
    LPWINDOWPLACEMENT WINAPI RestoreConfiguration(VOID);
    BOOL WINAPI DoChooseFont(VOID);

//*** init.c
    BOOL InitApplication (HINSTANCE);
    BOOL InitInstance (HINSTANCE, int);
    HWND WINAPI PASCAL CreateStatus( HWND hwndParent, UINT uID );
    HWND WINAPI PASCAL CreateSlider( HWND hwndParent, UINT uID );
    HWND WINAPI CreateELB( HWND hwndParent, UINT uID );
    HWND WINAPI CreateFontView( HWND hwndParent, UINT uID );

//*** pubinfo.c
    LPPUB WINAPI LoadPubInfo( LPDB, LPSTR );
    LPSTR WINAPI GetThumbString( LPSTYLE lpStyle );
    VOID  WINAPI SetThumbString( LPSTYLE lpStyle, LPSTR lpText );

//*** pubsave.c
    BOOL    WINAPI DBSaveStuffToMakeDLLs( LPDB );
    BOOL    WINAPI DBSaveBMP( LPFACE, UINT, LPSTR );
    BOOL    WINAPI DBSaveRC( LPFAMILY, LPSTR );
    BOOL    WINAPI DBSaveDEF( LPFAMILY, LPSTR );
    BOOL    WINAPI DBSaveMAKEALL( LPPUB, LPSTR );
    HFONT   WINAPI DBGetFont( LPSTYLE, UINT );
    

//*** Global Variables
    extern HINSTANCE    ghInst;
    extern HFONT        ghfontSmall;
    extern HFONT        ghfontUser;
    extern HWND         ghwndMain;
    extern HWND         ghwndStat;
    extern HWND         ghwndSlider;
    extern HWND         ghwndELB;
    extern HWND         ghwndFontView;
    extern char         szDBPath[120];

    extern LPDB         glpDB;

                             
//*** EOF: global.h
