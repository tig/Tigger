// ceklib.h
//
// Copyright (c) 1990 Charles E. Kindel, Jr.
//
//

// Include the dialog defines
#include "ceklibd.h"

//----------------------------------------------------------------
// SelPrn
//----------------------------------------------------------------
// Exported defins
#define SELPRN_STANDARD    0x0001
#define SELPRN_USEDEFAULT  0x0002
#define SELPRN_GETDEFAULT  0x0004
#define SELPRN_ALLOWSETUP  0x0008

#define DLG_CANCEL     IDCANCEL
#define DLG_OK         IDOK
#define DLG_ERROR      -1

// Oridinal #4
int FAR PASCAL
PrintSelPrnDialog( HWND hwndParent,
                       LPSTR lpszTitle,
                       LPSTR lpszDriverName,
                       LPSTR lpszDeviceName,
                       LPSTR lpszPort,                       
                       WORD  wFlags ) ;

// Oridinal #5
int FAR PASCAL
PrintGetDefaultPrn( LPSTR lpszDriverName,
                        LPSTR lpszDeviceName,
                        LPSTR lpszPortName ) ;

// Oridinal #6
BOOL FAR PASCAL 
SelPrinterDlgProc( HWND hDlg,
                   unsigned iMessage,
                   WORD wParam,
                   LONG lParam ) ;

//-------------------------------------------------------------------
// Ordinal #7
BOOL FAR PASCAL
DrawShadow ( HDC      hDC,           // handle to the Device Context
            HWND     hWnd,          // handle to the window to be shadowed
            HBRUSH   hBrush,        // brush to draw shadow with 
            short    ShadowWidth ) ;// Width (in pixels) of the shadow

//-------------------------------------------------------------------
// About box routines
//-------------------------------------------------------------------
#define ABOUT_STANDARD   0x0001
#define ABOUT_BIG        0x0002
#define ABOUT_REGISTERED 0x0004

// Ordinal #8
BOOL FAR PASCAL
AboutDialog( HWND    hwndParent,
             LPSTR   lpszTitle,
             LPSTR   lpszAppName,
             LPSTR   lpszVersion,
             LPSTR   lpszDescription,
             WORD    wFlags ) ;


// Ordinal #9
BOOL FAR PASCAL 
AboutDlgProc( HWND hDlg,
              unsigned iMessage,
              WORD wParam,
              LONG lParam ) ;

//-------------------------------------------------------------------
// Version routines
//-------------------------------------------------------------------
// Oridinal #10
LPSTR FAR PASCAL
VerGetDLLVersion( LPSTR lpszVersion ) ;

// Ordinal #11
LPSTR FAR PASCAL
VerMakeVersionNumber( LPSTR   lpszOut,
                      WORD    wMajor,
                      WORD    wMinor,
                      WORD    wBeta ) ;
//------------------------------------------------------------------
// FileOpenDialog Box
//------------------------------------------------------------------
// Ordianal #12?
int FAR PASCAL FileOpenDialog( HWND    hwndParent,
                               LPSTR   lpszTitle,
                               LPSTR   lpszExtension,
                               WORD    wFlags,
                               LPSTR   lpszFileBuf,
                               WORD    wFileLen ) ;

#define DLGOPEN_MUSTEXIST	0x0001
#define DLGOPEN_NOSHOWSPEC	0x0002
#define DLGOPEN_SAVE		0x0004
#define DLGOPEN_OPEN		0x0008
#define DLGOPEN_SHOWDEFAULT	0x0020
#define DLGOPEN_NOBEEPS		0x0040
#define DLGOPEN_SEARCHPATH	0x0080
#define DLGOPEN_NOCHANGESPEC 0x0100

#define DLG_MISSINGFILE	DLG_ERROR


//-------------------------------------------------------------------
// Registration routines (register.c)
//-------------------------------------------------------------------
// #13
BOOL FAR PASCAL
IsRegisteredUser( LPSTR szAppName,
                  LPSTR szName,
                  LPSTR szNumber,
                  WORD  wFlags ) ;

// #14
BOOL FAR PASCAL
RegistrationDialog(  HWND  hWnd,
                     LPSTR szAppName,
                     WORD  wFlags ) ;

// 15
BOOL FAR PASCAL
BuildRegNum( LPSTR lpszAppName,
             LPSTR lpszName,
             LPSTR lpszNumber,
             WORD  wFlags ) ;

// #16
BOOL FAR PASCAL
   RegisterDlgProc( HWND hWnd,
                    unsigned iMessage,
                    WORD wParam,
                    LONG lParam ) ;

#define REGISTER_READ   0x0000 
#define REGISTER_WRITE  0x0001 
#define REGISTER_GENERATE  0x0002

#define REG_MAX_NAME    64
#define REG_MAX_NUMBER  8     // Keep it short.

#if 0
//-------------------------------------------------------------------
// Debug
//-------------------------------------------------------------------
// ordinal #2
int far cdecl dp( LPSTR lpszFormat, ... );
// ordinal #3
int far cdecl dprintf( LPSTR lpszFormat, ... ) ;
#ifdef DEBUG
      #define D(a)   {a;}
#else
      #define D(a)   ;
#endif
#endif


                                   
