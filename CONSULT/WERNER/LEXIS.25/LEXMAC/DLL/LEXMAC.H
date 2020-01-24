// lexmac.h
//
// Copyright (c) 1990 Charles E. Kindel, Jr.
//
//

// Include the dialog defines
#include "lexmacd.h"

#define DLG_CANCEL     IDCANCEL
#define DLG_OK         IDOK
#define DLG_ERROR      -1

//-------------------------------------------------------------------
// Ordinal #7
BOOL FAR PASCAL
DrawShadow ( HDC      hDC,           // handle to the Device Context
            HWND     hWnd,          // handle to the window to be shadowed
            HBRUSH   hBrush,        // brush to draw shadow with 
            short    ShadowWidth ) ;// Width (in pixels) of the shadow

//-------------------------------------------------------------------
// SelectMacro dialog box routines
//-------------------------------------------------------------------
BOOL FAR PASCAL
SelectMacroDialog( LPSTR   lpszMacro,
                   LPSTR   lpszSelection,
                   LPSTR   lpszClient,
                   LPSTR   lpszPages,
                   LPSTR   lpszTimeOut ) ;
               
BOOL FAR PASCAL 
MainDlgProc( HWND hDlg,
              unsigned iMessage,
              WORD wParam,
              LONG lParam ) ;


//-------------------------------------------------------------------
// About box routines
//-------------------------------------------------------------------
#define ABOUT_STANDARD   0x0001

// Ordinal #8
BOOL FAR PASCAL
AboutDialog( void ) ;


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


//-------------------------------------------------------------------
// Debug
//-------------------------------------------------------------------
#ifdef DEBUG
      // ordinal #2
      int far cdecl dp( LPSTR lpszFormat, ... );
      // ordinal #3
      int far cdecl dprintf( LPSTR lpszFormat, ... ) ;
      #define D(a)   {a;}
#else
      #define D(a)   ;
#endif



                                   
