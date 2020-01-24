/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  vt220.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Header file for vt220.c
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.002  2/28/91 baw   Moved to self-contained DLL
 *
 ************************************************************************/

#include "version.h"
#include "resource.h"

#include "..\apache\asciidef.h"
#include "..\apache\facelift.h"

/************************************************************************
 *
 * Module definitions
 *    vt220.c
 *    
 ************************************************************************/

// offset in window extra bytes for hECB

#define GWW_HECB        0
#define GWW_EXTRABYTES  2

// event messages

#define WM_EVENT_RXCHAR     (WM_USER + 0x1000)
#define WM_EVENT_TXCHAR     (WM_USER + 0x1001)
#define WM_EVENT_DISCONNECT (WM_USER + 0x1002)

#define WM_CMD_READBLK      (WM_USER + 0x2000)
#define WM_CMD_WRITEBLK     (WM_USER + 0x2001)

// macros

#define ANSWERBACK( x )         (x -> szAnswerBack)
#define ATTR( x )               (x -> bAttr)
#define AUTOWRAP( x )           (x -> fAutoWrap)
#define BLINKSTATE( x )         (x -> fBlinkState)
#define CHARACTERSET( x, y )    (x -> CharSets[ y ])
#define CONTROLSEQ( x, y )      (x -> ControlSeq[ y ])
#define CURCOL( x )             (x -> nCurrentCol)
#define CURROW( x )             (x -> nCurrentRow)
#define CURSORSTATE( x )        (x -> bCursorState)
#define SEQUENCESTATE( x )      (x -> wSequenceState)
#define SEQUENCEFLAG( x )       (x -> wSequenceFlag)
#define DECGL( x )              (x -> nDECGL)
#define DECGR( x )              (x -> nDECGR)
#define DEFAULTFONT( x )        (x -> hDefaultFont)
#define EMULATORFONT(x, y)      (x -> ahEmulatorFonts[ y ])
#define COMPATIBILITYLEVEL( x ) (x -> wCompatibilityLevel)
#define RESPONSELEVEL( x )      (x -> wResponseLevel)
#define HIGHLIGHTFONT( x )      (x -> lfHighlightFont)
#define PARAM( x, y )           (x -> anParams[ y ])
#define PARAMCOUNT( x )         (x -> nParamCt)
#define KEYPADTYPE( x )         (x -> bKeypadType)
#define KEYBOARDSTATE( x )      (x -> KeyboardState)
#define LOCALECHO( x )          (x -> fLocalEcho)
#define NEWLINE( x )            (x -> fNewLine)
#define NORMALFONT( x )         (x -> lfNormalFont)
#define OEMFONT( x )            (x -> lfOEMFont)
#define MAXCOLS( x )            (x -> nColumns)
#define MAXROWS( x )            (x -> nRows)
#define ORIGIN( x )             (x -> fOrigin)
#define SAVECOL( x )            (x -> nSaveCol)
#define SAVEROW( x )            (x -> nSaveRow)
#define SAVESTATE( x )          (x -> csSaveState)
#define CURSORSTATESAVED( x )   (x -> fCursorStateSaved)
#define SCREENBUFFER( x )       (x -> hScreenBuffer)
#define CHARBUF( x )            (x -> lpCharBuf)
#define ATTRBUF( x )            (x -> lpAttrBuf)
#define SCREENDC( x )           (x -> hScreenDC)
#define SCROLLREGIONBOTTOM( x ) (x -> nScrollRegionBottom)
#define SCROLLREGIONTOP( x )    (x -> nScrollRegionTop)
#define SELERASE( x )           (x -> fSelErase)
#define SNGLSHFT( x )           (x -> nSnglShft)
#define TABSTOPS( x )           (x -> TabStops)
#define USENUMLOCK( x )         (x -> fUseNumLock)
#define XCHAR( x )              (x -> nCharX)
#define XOFFSET( x )            (x -> nOffsetX)
#define XSCROLL( x )            (x -> nScrollX)
#define XSIZE( x )              (x -> nSizeX)
#define YCHAR( x )              (x -> nCharY)
#define YOFFSET( x )            (x -> nOffsetY)
#define YSCROLL( x )            (x -> nScrollY)
#define YSIZE( x )              (x -> nSizeY)

#define GETHINST( hWnd )        ((HINSTANCE) GetWindowWord( hWnd, GWW_HINSTANCE ))
#define GETHECB( hWnd )         ((HECB) GetWindowWord( hWnd, GWW_HECB ))
#define SETHECB( hWnd, hECB )   SetWindowWord( hWnd, GWW_HECB, (WORD) hECB )
#define MAKEWORD( bLow, bHigh ) ((WORD)((WORD) bHigh << 8 | (WORD) bLow))

// constant definitions

#define MAXLEN_FMTSTR          256
#define MAXLEN_KEYBOARDSTATE   256
#define MAXLEN_TEMPSTR         81
#define MAXLEN_CONTROLSEQ      81
#define MAXLEN_ANSWERBACK      81
#define MAXLEN_PARAMS          10
#define MAXLEN_CHARSETS        4
#define MAXLEN_FONTS           12
#define MAXLEN_DEVATTR         32

#define ECS_AVAIL              0x01
#define ECS_SHOW               0x02

#define VT220_COLUMNS_80       80
#define VT220_COLUMNS_132      132
#define VT220_ROWS             24
#define VT220_BYTESPERCHAR     2

#define VT220_DEVATTRPRI       "?62;1;2;6;7;8;9c"
#define VT220_DEVATTRSEC       "1;10;0c"

// character set definitions

#define CHARSET_ASCII         0x01
#define CHARSET_DECSUPPL      0x02
#define CHARSET_DECGRAPHIC    0x03
#define CHARSET_NRCSBRITISH   0x04
#define CHARSET_NRCSDUTCH     0x05
#define CHARSET_NRCSFINNISH   0x06
#define CHARSET_NRCSFRENCH    0x07
#define CHARSET_NRCSCANADIAN  0x08
#define CHARSET_NRCSGERMAN    0x09
#define CHARSET_NRCSITALIAN   0x0A
#define CHARSET_NRCSDANISH    0x0B
#define CHARSET_NRCSSPANISH   0x0C
#define CHARSET_NRCSSWEDISH   0x0D
#define CHARSET_NRCSSWISS     0x0E

#define CHARSET_DEFAULT       0xFF

// keyboard set definitions

#define KEYPAD_APPLICATION  0x01
#define KEYPAD_NUMERIC      0x02

// font definitions

#define FONT_STANDARD        0x00
#define FONT_UNDERLINE       0x01
#define FONT_DBLWIDTH        0x02
#define FONT_DBLWIDTHUL      0x03
#define FONT_DBLHEIGHT       0x04
#define FONT_DBLHEIGHTUL     0x05
#define FONT_STANDARDOEM     0x06
#define FONT_UNDERLINEOEM    0x07
#define FONT_DBLWIDTHOEM     0x08
#define FONT_DBLWIDTHULOEM   0x09
#define FONT_DBLHEIGHTOEM    0x0A
#define FONT_DBLHEIGHTULOEM  0x0B

// attribute definitions

#define ATTRIBUTE_BOLD          0x80
#define ATTRIBUTE_UNDERLINE     0x40
#define ATTRIBUTE_BLINKING      0x20
#define ATTRIBUTE_INVERSE       0x10
#define ATTRIBUTE_ERASEABLE     0x08
#define ATTRIBUTE_OEMCHARSET    0x04
#define ATTRIBUTE_DBLHEIGHTBTM  0x03
#define ATTRIBUTE_DBLHEIGHTTOP  0x02
#define ATTRIBUTE_DBLWIDTH      0x01

#define ATTRIBUTE_DBLSIZE       0x03

// sequence states

#define SEQUENCE_NONE        0x00
#define SEQUENCE_ESCAPE      0x01
#define SEQUENCE_CONTROL     0x02
#define SEQUENCE_LINEATTR    0x03
#define SEQUENCE_CHARSET     0x04

// sequence flags

#define SEQFLAG_NONE         0x00
#define SEQFLAG_BANG         0x01
#define SEQFLAG_GREATER      0x02
#define SEQFLAG_SPACE        0x03
#define SEQFLAG_QUESTION     0x04
#define SEQFLAG_QUOTE        0x05

// emulation modes

#define LEVEL_VT2008BIT       0x00
#define LEVEL_VT2007BIT       0x01
#define LEVEL_VT100           0x02
#define LEVEL_VT52            0x03

#define LEVEL_S7C1T           0x10
#define LEVEL_S8C1T           0x20

// blink rate and id

#define BLINK_ID    0x0100
#define BLINK_RATE  750

typedef struct tagCURSORSAVE
{
  int   nCurrentRow, nCurrentCol, nDECGL, nDECGR ;
  BOOL  fAutoWrap, fOrigin, fSelErase ;
  BYTE  bAttr ;

} CURSORSAVE ;

typedef struct tagECB
{
   int           nEmulatorType, nColumns, nRows, nParamCt,
                 nScrollRegionTop, nScrollRegionBottom,
                 anParams[ MAXLEN_PARAMS ], nCurrentCol, nCurrentRow, 
                 nSaveRow, nSaveCol, nCharX, nCharY, 
                 nOffsetX, nOffsetY, nScrollX, nScrollY, nSizeX, nSizeY,
                 nDECGL, nDECGR, nSnglShft ;
   char          szAnswerBack[ MAXLEN_ANSWERBACK ] ;
   BOOL          fLocalEcho, fAutoWrap, fNewLine, fUseNumLock,
                 fOrigin, fSelErase, fBlinkState, fCursorStateSaved ;
   BYTE          CharSets[ MAXLEN_CHARSETS ],
                 ControlSeq[ MAXLEN_CONTROLSEQ ],
                 TabStops[ VT220_COLUMNS_132 / 8 + 1 ],
                 KeyboardState[ MAXLEN_KEYBOARDSTATE ], 
                 bAttr, bCursorState, bKeypadType ;
   EXTLOGFONT    lfHighlightFont, lfNormalFont, lfOEMFont ;
   HDC           hScreenDC ;
   FARPROC       lpfnErrorProc ;
   GLOBALHANDLE  hScreenBuffer ;
   HFONT         ahEmulatorFonts[ MAXLEN_FONTS ], hDefaultFont ;
   WORD          wSequenceState, wSequenceFlag,
                 wCompatibilityLevel, wResponseLevel ;
   CURSORSAVE    csSaveState ;
   LPSTR         lpCharBuf, lpAttrBuf ;

} ECB, NEAR *NPECB ;

#ifdef WIN31
DECLARE_HANDLE( HECB ) ;
#else
typedef HANDLE HECB ;
#endif

// global variables

#ifdef GLOBALDEFS
int        nUseCount ;
char       szEmulatorClass[] = "EmulatorClass" ;
HHOOK      hOldHookProc ;
HINSTANCE  hInstLib ;
HWND       hFocusWnd ;
#else
extern int        nUseCount ;
extern char       szEmulatorClass[] ;
extern HHOOK      hOldHookProc ;
extern HINSTANCE  hInstLib ;
extern HWND       hFocusWnd ;
#endif

// function prototypes (public)

int FAR PASCAL GetEmulatorUseCount( VOID ) ;
DWORD FAR PASCAL EmulatorKeyboardHook( int, WORD, DWORD ) ;
LONG FAR PASCAL EmulatorWndProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL SettingsDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL DoEmulatorSetupDlg( HWND, HWND, LPSTR, LPSTR ) ;

BOOL FAR PASCAL SaveEmulatorSettings( HECB, LPSTR, LPSTR ) ;
BOOL FAR PASCAL LoadEmulatorSettings( HWND, LPSTR, LPSTR ) ;

// function prototypes (private)

BOOL RegisterClasses( HINSTANCE ) ;
BOOL UnregisterClasses( HINSTANCE ) ;

HECB CreateECB( VOID ) ;
BOOL DestroyECB( HECB ) ;
BOOL InitECB( HECB ) ;
VOID ClearTabStops( NPECB ) ;
VOID SetTabStop( NPECB, int ) ;
VOID ClearTabStop( NPECB, int ) ;
BOOL GetTabState( NPECB, int ) ;
BOOL SetEmulatorParams( HWND, LPSTR ) ;
HECB CreateEmulator( HWND ) ;
VOID SoftResetEmulator( NPECB ) ;
VOID HardResetEmulator( NPECB ) ;
BOOL DestroyEmulator( HWND ) ;
BOOL WriteEmulatorBlock( HWND, LPSTR, int ) ;

VOID NEAR StandardCharacter( HWND, NPECB, BYTE ) ;
VOID NEAR ControlCharacter( HWND, NPECB, BYTE ) ;
VOID NEAR ControlSequence( HWND, NPECB, BYTE ) ;
VOID NEAR EscapeSequence( HWND, NPECB, BYTE ) ;
VOID NEAR GetStdCharRect( NPECB, LPRECT, int, int ) ;
VOID NEAR GetDblCharRect( NPECB, LPRECT, int, int ) ;
VOID NEAR LineAttribute( HWND, NPECB, BYTE ) ;
VOID NEAR SelectCharacterSet( HWND, NPECB, BYTE ) ;
VOID NEAR ResetControlSeqParams( NPECB ) ;
VOID ScrollRegionUp( HWND, NPECB, int, int, int ) ;
VOID ScrollRegionDown( HWND, NPECB, int, int, int ) ;
VOID ScrollLineLeft( HWND, NPECB, int, int, int ) ;
VOID ScrollLineRight( HWND, NPECB, int, int, int ) ;
VOID SendControlSequence( HWND, NPECB, LPSTR ) ;
WORD TranslateEmulatorByte( NPECB, BYTE ) ;
int NEAR GetFontNumber( BYTE ) ;
VOID ExpandDoubleWide( HWND, NPECB, BYTE, BYTE ) ;
BOOL WriteEmulatorFormat( HWND, LPSTR, ... ) ;
VOID WriteEmulatorError( HWND, int ) ;
BOOL BlinkEmulator( HWND ) ;
BOOL FAR PASCAL PaintEmulator( HWND ) ;
VOID NEAR PASCAL PaintRun( NPECB, HDC, BYTE, int, int, int, BOOL ) ;
BOOL SizeEmulator( HWND, WORD, WORD ) ;
BOOL ScrollEmulatorVert( HWND, WORD, WORD ) ;
BOOL ScrollEmulatorHorz( HWND, WORD, WORD ) ;
BOOL MoveEmulatorCursor( HWND ) ;
BOOL HideEmulatorCursor( HWND ) ;
BOOL ShowEmulatorCursor( HWND ) ;
VOID CheckCursorPosition( HWND, NPECB, BYTE ) ;
BOOL SetEmulatorFocus( HWND ) ;
BOOL KillEmulatorFocus( HWND ) ;
GLOBALHANDLE CreateScreenBuffer( int, int, int ) ;
BOOL DestroyScreenBuffer( HECB ) ;
BOOL CreateEmulatorFonts( HWND, HECB ) ;
BOOL DestroyEmulatorFonts( HWND, HECB ) ;

// dialog helper functions

BOOL SettingsDlgInit( HWND, HECB, LPEXTLOGFONT, LPEXTLOGFONT ) ;
BOOL SettingsDlgTerm( HWND, HECB, LPEXTLOGFONT, LPEXTLOGFONT ) ;
BOOL DrawSample( HWND, LPEXTLOGFONT, LPEXTLOGFONT ) ;

// save settings helper functions

BOOL WriteFonts( LPSTR, LPEXTLOGFONT, LPEXTLOGFONT, LPSTR ) ;

// load settings helper functions

BOOL GetFonts( LPSTR, LPEXTLOGFONT, LPEXTLOGFONT, LPSTR ) ;

/************************************************************************
 * End of File: vt220.h
 ************************************************************************/
