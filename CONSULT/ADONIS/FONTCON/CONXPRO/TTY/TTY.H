//---------------------------------------------------------------------------
//
//  Module: tty.h
//
//  Purpose:
//     This is the header file for the TTY sample.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  Written by Microsoft Product Support Services, Windows Developer Support.
//  Copyright (c) 1991 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#define WIN31        // this is a Windows 3.1 application
#define USECOMM      // yes, we need the COMM API
#define STRICT       // be bold!

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <string.h>

#include <wdb.h>

#include "version.h"
#include "resource.h"

// constant definitions

#define GWL_TTYINFO         0       // ckindel 2/15/92

#define ABOUTDLG_USEBITMAP  1

#define ATOM_HWNDTTY       0x100

// terminal size

#define MAXROWS         25
#define MAXCOLS         80

#define MAXBLOCK        255

#define MAXLEN_TEMPSTR  81

#define MAXLEN_PATH     164
#define MAXLEN_FILENAME 32

#define RXQUEUE         4096
#define TXQUEUE         4096

// cursor states

#define CS_HIDE         0x00
#define CS_SHOW         0x01

// Flow control flags

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions

#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

// data structures

typedef struct tagTTYINFO
{
   int      idComDev ;
   BYTE     bPort, abScreen[ MAXROWS * MAXCOLS ] ;
   BOOL     fConnected, fXonXoff, fLocalEcho, fNewLine, fAutoWrap,
            fUseCNReceive, fDisplayErrors;
   BYTE     bByteSize, bFlowCtrl, bParity, bStopBits ;
   UINT     wBaudRate, wCursorState ;
   HFONT    hTTYFont ;
   LOGFONT  lfTTYFont ;
   COLORREF rgbFGColor ;
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
      #define ALLOCTTYINFO()        (PTTYINFO)LocalAlloc( LPTR, sizeof( TTYINFO ) )
      #define FREETTYINFO(hWnd)     LocalFree((LOCALHANDLE)LOWORD(GetWindowLong( hWnd, GWL_TTYINFO )))
      #define GETTTYINFO(hWnd)      (NPTTYINFO)LOWORD(GetWindowLong(hWnd, GWL_TTYINFO))
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


#define COMDEV( x ) (x -> idComDev)
#define PORT( x )   (x -> bPort)
#define SCREEN( x ) (x -> abScreen)
#define CONNECTED( x ) (x -> fConnected)
#define XONXOFF( x ) (x -> fXonXoff)
#define LOCALECHO( x ) (x -> fLocalEcho)
#define NEWLINE( x ) (x -> fNewLine)
#define AUTOWRAP( x ) (x -> fAutoWrap)
#define BYTESIZE( x ) (x -> bByteSize)
#define FLOWCTRL( x ) (x -> bFlowCtrl)
#define PARITY( x ) (x -> bParity)
#define STOPBITS( x ) (x -> bStopBits)
#define BAUDRATE( x ) (x -> wBaudRate)
#define CURSORSTATE( x ) (x -> wCursorState)
#define HTTYFONT( x ) (x -> hTTYFont)
#define LFTTYFONT( x ) (x -> lfTTYFont)
#define FGCOLOR( x ) (x -> rgbFGColor)
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
#define USECNRECEIVE( x ) (x -> fUseCNReceive)
#define DISPLAYERRORS( x ) (x -> fDisplayErrors)

#define SET_HWNDTTY_PROP( x, y )  SetProp( x, MAKEINTATOM( ATOM_HWNDTTY ), y )
#define GET_HWNDTTY_PROP( x )     GetProp( x, MAKEINTATOM( ATOM_HWNDTTY ) )
#define REMOVE_HWNDTTY_PROP( x )  RemoveProp( x, MAKEINTATOM( ATOM_HWNDTTY ) )

// global stuff

char     gszTTYClass[] = "TTYWndClass" ;
char     gszAppName[] = "TTY Sample Application" ;
HANDLE   ghAccel ;
UINT     gawBaudTable[] = { CBR_110, 
                            CBR_300,
                            CBR_600,
                            CBR_1200,
                            CBR_2400,
                            CBR_4800,
                            CBR_9600,
                            CBR_14400,
                            CBR_19200,
                            CBR_38400,
                            CBR_56000,
                            CBR_128000,
                            CBR_256000   } ;

UINT     gawParityTable[] = { NOPARITY,
                              EVENPARITY,
                              ODDPARITY,
                              MARKPARITY,
                              SPACEPARITY } ;

UINT     gawStopBitsTable[] = { ONESTOPBIT,
                                ONE5STOPBITS,
                                TWOSTOPBITS } ;

//
// Routines that could conceivably be called by antoher
// module in a more sophisticated app are WINAPI.
//
// TTY Window specific functions.  (i.e. those dealing with
// input/output to the tty window).
//
LRESULT  WINAPI CreateTTYInfo( HWND ) ;
BOOL     WINAPI DestroyTTYInfo( HWND ) ;
BOOL     WINAPI ResetTTYScreen( HWND ) ;
BOOL     WINAPI KillTTYFocus( HWND ) ;
BOOL     WINAPI SetTTYFocus( HWND ) ;
BOOL     WINAPI ProcessTTYCharacter( HWND, BYTE ) ;
BOOL     WINAPI IsTTYConnected( HWND hWnd ) ;
BOOL     WINAPI WriteTTYBlock( HWND, LPSTR, int ) ;
BOOL     WINAPI SelectTTYFont( HWND ) ;

//
// COMM Specific functions.
//
BOOL     WINAPI OpenConnection( HWND ) ;
BOOL     WINAPI SetupConnection( HWND ) ;
BOOL     WINAPI CloseConnection( HWND ) ;
int      WINAPI ReadCommBlock( HWND, LPSTR, int ) ;
BOOL     WINAPI WriteCommByte( HWND, BYTE ) ;

//---------------------------------------------------------------------------
//  End of File: tty.h
//---------------------------------------------------------------------------

