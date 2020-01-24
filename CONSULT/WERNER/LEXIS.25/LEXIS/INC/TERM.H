/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  term.h
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *
 ************************************************************************/
BOOL FAR PASCAL InitTerm( HANDLE hInst ) ;
BOOL FAR PASCAL ResetTerm( HWND hwndTerm ) ;
BOOL FAR PASCAL WriteTerm( HWND hwndTerm, LPSTR lpszBuf );
BOOL FAR PASCAL GetTermScreen( HWND hwndTerm, LPSTR lpszBuf );
BOOL FAR PASCAL GetTermSelected( HWND hwndTerm, LPSTR lpszBuf );
BOOL FAR PASCAL
GetTermAutoSize( HWND hwndTerm, short FAR *lpX, short FAR *lpY ) ;
BOOL FAR PASCAL ClearTerm( HWND hwndTerm );
BOOL FAR PASCAL HomeTerm( HWND hwndTerm );
BOOL FAR PASCAL SetTermHighlight( HWND hwndTerm, BOOL fHL );
BOOL FAR PASCAL SetTermUnderline( HWND hwndTerm, BOOL fUL );
BOOL FAR PASCAL SetTermPos( HWND hwndTerm ) ;
BOOL FAR PASCAL LockTermKeyboard( HWND hwndTerm, BOOL fLock );
BOOL FAR PASCAL ConnectTerm( HWND hwndTerm, BOOL fConnected );
HWND FAR PASCAL CreateTerm( HWND hwndParent ) ;
BOOL FAR PASCAL DestroyTerm( HWND hwndTerm ) ;
BOOL FAR PASCAL HeyTerminalDoYouWantFocus( HWND hwndTerm ) ;


/************************************************************************
 * Local Defines
 ************************************************************************/
#define NUMCOLS   80
#define NUMROWS   24

#define CHAR_SPACE      0x20
#define CHAR_CR         0x0D
#define CHAR_LF         0x0A
#define CHAR_BS         0x08
#define CHAR_ESC        0x1B
#define CHAR_TAB        0x09


#define ATTR_NORMAL     0x00 
#define ATTR_HIGHLIGHT  0x01 
#define ATTR_UNDERLINE  0x02 


#define GWW_LHTB        0 

/************************************************************************
 * Macros 
 ************************************************************************/
#define NUMWNDEXTRA     1

#define GETLHTB(hwnd)   (GetWindowWord(hwnd, GWW_LHTB))

#define CHARYX(y,x)    (pTB->rgcChar[y][x])
#define ATTRYX(y,x)    (pTB->rgcAttr[y][x])

#define ROW_CHAR(y)       (pTB->rgcChar[y])
#define ROW_ATTR(y)       (pTB->rgcAttr[y])

#define LOCKTERMBLOCK(lh)     ((PTERMBLOCK)LocalLock(lh))
#define UNLOCKTERMBLOCK(lh)   (LocalUnlock(lh))
#define ALLOCTERMBLOCK()      (LocalAlloc( LHND, sizeof( TERMBLOCK ) ))

#define HOFFSET( pTB )     (-pTB->nHscrollPos * pTB->nCharSizeX)
#define VOFFSET( pTB )     (-pTB->nVscrollPos * pTB->nCharSizeY)

/************************************************************************
 * Local data structures
 ************************************************************************/

typedef struct tagTERMBLOCK
{
   BOOL  fConnected ;
   BOOL  fKeyLocked ;      // Is keyboard locked?
   BOOL  fCaret ;
   BOOL  fInsert ;
   BOOL  fWantFocus ;

   //
   // The code in this module relies on the fact that static variables
   // are zero'd out and the NUMCOLS+1 element of these arrays is allays
   // zero.
   //
   BYTE  rgcChar[NUMROWS][NUMCOLS+1] ;
   BYTE  rgcAttr[NUMROWS][NUMCOLS+1] ;

   //
   // To speed things up, we have an array of pointers to the lines
   // in the buffer.  This way we never have to do a memcpy to
   // scroll.  [NOTE: NOT implemented because LEXIS very RARELY ever
   // scrolls it's window!]
   //
   //
   // LPSTR lpRow[NUMROWS] ;

   // 
   // CurCol and CurRow are the current row and column.  This means
   // they point to where the next character will be put.
   //
   short nCurCol ;
   short nCurRow ;

   //
   // Current attribute:  ATTR_NORMAL | ATTR_HIGHLIGHT | ATTR_UNDERLINE
   //
   BYTE  bCurAttr ;

   HFONT hFont ;
   HBRUSH hBrush ;            // background color

   //
   // Character size in pixels
   //
   short nCharSizeX, nCharSizeY ;

   //
   // Some duplication here for speed.  i.e. size vs. speed
   // Pixels required for full display
   //
   short nFullSizeX, nFullSizeY ;

   //
   // Current cursor position in pixels.
   //
   short nCurPosX, nCurPosY ;

   //
   // Now for the scroll bar stuff
   //
   short  nCurSizeX, nCurSizeY ;
   short  nVscrollMax, nVscrollPos ;
   short  nHscrollMax, nHscrollPos ;

   //
   // Selected text info
   //
   short  nSelStartX, nSelStartY ;
   short  nSelEndX, nSelEndY ;

   short  nCurSelX, nCurSelY ;

   //
   // We cheat here and put the hwnd to the terminal in the block
   //
   HWND  hwndTerm ;

} TERMBLOCK, *PTERMBLOCK, FAR *LPTERMBLOCK ;


/************************************************************************
 * End of File: term.h
 ************************************************************************/

