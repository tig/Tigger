/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  tty.h
 *
 ***********************************************************************/
#ifndef _TTY_H_
#define _TTY_H_


// constant definitions

#define GWL_TTYINFO         0       // ckindel 2/15/92

#define ATOM_HWNDTTY       0x100

#define TABSIZE         8 

//
// Define TRUE if you want chars < 0x20 and > 0x7e to be displayed
// in the form <XX>
//
#define DISPLAYCODES    TRUE
#define EXPANDTABS      FALSE
#define EXPANDBELLS     FALSE
#define EXPANDBS        FALSE

// terminal size

#define MAXROWS         100
#define MAXCOLS         80

#define MAXLEN_TEMPSTR  81

// cursor states

#define CS_HIDE         0x00
#define CS_SHOW         0x01

// data structures

typedef struct tagTTYINFO
{
   BYTE     abScreen[ MAXROWS * MAXCOLS ] ;
   BYTE     abAttrib[ MAXROWS * MAXCOLS ] ;
   BOOL     fLocalEcho, fNewLine, fAutoWrap ;
   UINT     wCursorState ;
   HFONT    hTTYFont ;
   int      xSize, ySize, xScroll, yScroll, xOffset, yOffset,
            nColumn, nRow, xChar, yChar ;

} TTYINFO, NEAR *NPTTYINFO, FAR *LPTTYINFO ;

DECLARE_HANDLE( HTTYINFO ) ;

//
// TTYINFO allocation macros.
//
// If you are concerned about absolute speed you will probably want
// to use local memory.  But sometimes an apps DS just gets too
// filled up, and you must resort to global memory.  The macros
// below make it easy to change TTY to use GLOBAL memory.  Simply
// #define USE_GLOBALMEM and party on.
//
// Also, the option is provided to use local fixed memory if you
// are *really* concerned about speed and you don't care about
// a chunk of memory in your DS being fixed.  Use USE_LOCALFIXEDMEM
// for this!
//
#define USE_LOCALFIXEDMEM

#ifdef USE_GLOBALMEM
   //
   // For  global memory define PTTYINFO as LPTTYINFO
   //
   #define PTTYINFO              LPTTYINFO
   #define ALLOCTTYINFO()        (LPTTYINFO)GlobalAllocPtr( GHND, sizeof( TTYINFO ) )
   #define FREETTYINFO(hWnd)     GlobalFreePtr((LPVOID)GetWindowLong( hWnd, GWL_TTYINFO ))
   #define GETTTYINFO(hWnd)      (LPTTYINFO)GetWindowLong( hWnd, GWL_TTYINFO )
   //
   // This is a no-op when using global mem
   //
   #define UNGETTTYINFO(lp)      {lp;}
#else
   #ifdef USE_LOCALFIXEDMEM
      //
      // For  local memory define PTTYINFO as NPTTYINFO so we don't get
      // any segment loads.
      //
      #define PTTYINFO              NPTTYINFO
      #define ALLOCTTYINFO()        MAKELONG((PTTYINFO)LocalAlloc( LPTR, sizeof( TTYINFO ) ),0)
      #define FREETTYINFO(hWnd)     LocalFree((LOCALHANDLE)LOWORD(GetWindowLong( hWnd, GWL_TTYINFO )))
      #define GETTTYINFO(hWnd)      ((NPTTYINFO)(LOWORD(GetWindowLong(hWnd, GWL_TTYINFO))))
      //
      // This is a no-op when using local fixed mem
      //
      #define UNGETTTYINFO(p)       {p;}
   #else
      //
      // For  local memory define PTTYINFO as NPTTYINFO so we don't get
      // any segment loads.
      //
      #define PTTYINFO              NPTTYINFO
      #define ALLOCTTYINFO()        (HTTYINFO)LocalAlloc( LHND, sizeof( TTYINFO ) )
      #define FREETTYINFO(hWnd)     {\
                                       LOCALHANDLE lh = (LOCALHANDLE)LOWORD(GetWindowLong(hWnd, GWL_TTYINFO)) ;\
                                       LocalUnlock(lh) ;\
                                       LocalFree(lh) ;\
                                    }
      #define GETTTYINFO(hWnd)      (NPTTYINFO)LocalLock((LOCALHANDLE)LOWORD(GetWindowLong(hWnd, GWL_TTYINFO)))
      #define UNGETTTYINFO(p)       LocalUnlock(LocalHandle(p))
   #endif
#endif 


// macros ( for easier readability )

#define GETHINST( hWnd )  ((HINSTANCE) GetWindowWord( hWnd, GWW_HINSTANCE ))

#define SCREEN( x ) (x -> abScreen)
#define ATTRIB( x ) (x -> abAttrib)
#define LOCALECHO( x ) (x -> fLocalEcho)
#define NEWLINE( x ) (x -> fNewLine)
#define AUTOWRAP( x ) (x -> fAutoWrap)
#define CURSORSTATE( x ) (x -> wCursorState)
#define HTTYFONT( x ) (x -> hTTYFont)
#define XSIZE( x ) (x -> xSize)
#define YSIZE( x ) (x -> ySize)
#define XSCROLL( x ) (x -> xScroll)
#define YSCROLL( x ) (x -> yScroll)
#define XOFFSET( x ) (x -> xOffset)
#define YOFFSET( x ) (x -> yOffset)
#define COLUMN( x ) (x -> nColumn)
#define ROW( x ) (x -> nRow)
#define XCHAR( x ) (x -> xChar)
#define YCHAR( x ) (x -> yChar )

#define SET_HWNDTTY_PROP( x, y )  SetProp( x, MAKEINTATOM( ATOM_HWNDTTY ), y )
#define GET_HWNDTTY_PROP( x )     GetProp( x, MAKEINTATOM( ATOM_HWNDTTY ) )
#define REMOVE_HWNDTTY_PROP( x )  RemoveProp( x, MAKEINTATOM( ATOM_HWNDTTY ) )

/*
 * The abAttrib member can be any of the following values
 * the WriteTTYBlock() function takes a color as the last param
 */
enum enumColors
{
   CLR_BLACK,    
   CLR_WHITE,    
   CLR_GRAY,     
   CLR_LTGRAY,   
   CLR_BLUE,     
   CLR_LTBLUE,   
   CLR_GREEN,    
   CLR_CYAN,     
   CLR_LTGREEN,  
   CLR_LTCYAN,   
   CLR_RED,      
   CLR_MAGENTA,  
   CLR_BROWN,    
   CLR_LTRED,    
   CLR_LTMAGENTA,
   CLR_YELLOW
}  ;

#define KEYBOARD_COLOR  CLR_BLACK
#define READCOMM_COLOR  CLR_MAGENTA
#define WRITECOMM_COLOR CLR_BLUE
#define COMMERROR_COLOR (CLR_WHITE | 0x80) // set the high bit to get background color

// function prototypes (private)

//
// Routines that could conceivably be called by antoher
// module in a more sophisticated app are WINAPI.
//
// TTY Window specific functions.  (i.e. those dealing with
// input/output to the tty window).
//
LRESULT  WINAPI CreateTTYInfo( HWND ) ;
BOOL     WINAPI SetTTYBinary( BOOL f ) ;
BOOL     WINAPI DestroyTTYInfo( HWND ) ;
BOOL     WINAPI ResetTTYScreen( HWND ) ;
BOOL     WINAPI KillTTYFocus( HWND ) ;
BOOL     WINAPI SetTTYFocus( HWND ) ;
BOOL     WINAPI ProcessTTYCharacter( HWND, BYTE ) ;
BOOL     WINAPI IsTTYConnected( HWND hWnd ) ;
BOOL     WINAPI WriteTTYBlock( HWND, LPSTR, int, int ) ;
BOOL     WINAPI SelectTTYFont( HWND ) ;

LRESULT CALLBACK fnTTYWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;


#endif

/************************************************************************
 * End of File: tty.h
 ***********************************************************************/

