//*************************************************************
//  File name: slidpriv.h
//
//  Description: 
//      Header file for the slider control
//
//  History:    Date       Author     Comment
//               2/25/92   MSM        Updated for FontShopper
//
//*************************************************************

#include <ctype.h>
#include <stdlib.h>

extern HINSTANCE hInstLib ;
extern BOOL      bMonochrome ;

// constant definitions
    #define RGBBLACK     RGB(0,0,0)
    #define RGBRED       RGB(128,0,0)
    #define RGBGREEN     RGB(0,128,0)
    #define RGBBLUE      RGB(0,0,128)
    #define RGBBROWN     RGB(128,128,0)
    #define RGBMAGENTA   RGB(128,0,128)
    #define RGBCYAN      RGB(0,128,128)
    #define RGBLTGRAY    RGB(192,192,192)
    #define RGBGRAY      RGB(128,128,128)

    #define RGBLTRED     RGB(255,0,0)
    #define RGBLTGREEN   RGB(0,255,0)
    #define RGBLTBLUE    RGB(0,0,255)
    #define RGBYELLOW    RGB(255,255,0)
    #define RGBLTMAGENTA RGB(255,0,255)
    #define RGBLTCYAN    RGB(0,255,255)
    #define RGBWHITE     RGB(255,255,255)


#define MAXLEN_TEMPSTR      81
#define MAXLEN_FLAGSTR      81

#define SHADOWWIDTH     1 
#define GWW_HSLIDERINFO 0

// data structures

typedef struct tagSLIDERINFO
{
   int      nNumCols ;      // number of columns
   int      nDragCol ;      // column in drag
   int      nFontHeight ;   // computed font height
   BOOL     fDragging ;     // dragging status
   BOOL     fShifted ;      // shift key was down when we started dragging
   BOOL     f3DOut ;        // if TRUE "3D Out" look is used, else "3D In"
   HANDLE   hColumnInfo ;   // handle to column info structure
   HCURSOR  hSplitCursor ;  // handle to split cursor
   HDC      hScreenDC ;     // handle to screen DC
   HFONT    hFont ;         // handle to selected font
   HFONT    hOldFont ;      // handle to old font
   HWND     hNotifyWnd ;    // handle to notification window
   DWORD    rgbFG ;         // foreground color
   DWORD    rgbBG ;         // background color

} SLIDERINFO, NEAR *NPSLIDERINFO ;

DECLARE_HANDLE( HSLIDERINFO ) ;

typedef struct tagCOLUMNINFO
{
   int      nColPos ;         // in percentage (0 - 100)
   BOOL     bDragging ;       // in dragging mode
   WORD     wAlignment ;      // alignment flag
   HANDLE   hColHeader ;      // handle to column header
   
} COLUMNINFO, NEAR *NPCOLUMNINFO ;

DECLARE_HANDLE( HCOLUMNINFO ) ;

// macros

#define GETHINST(hwnd)         ((HINSTANCE) GetWindowWord( hwnd, GWW_HINSTANCE ))
#define GETHSLIDERINFO( hWnd ) ((HSLIDERINFO) GetWindowWord( hWnd, GWW_HSLIDERINFO ))

// function prototypes (private)

BOOL NEAR RegisterClasses( HANDLE ) ;
BOOL NEAR UnregisterClasses( HANDLE ) ;
HSLIDERINFO NEAR CreateSliderInfo( HWND, DWORD ) ;
LRESULT NEAR DestroySliderInfo( HWND ) ;
HCOLUMNINFO NEAR CreateColumnInfo( int ) ;
LRESULT NEAR DestroyColumnInfo( HCOLUMNINFO, int ) ;
LRESULT NEAR SetNumberColumns( HWND, WORD ) ;
LRESULT NEAR SetColumnHeader( HWND, WORD, LPSTR ) ;
LRESULT NEAR SetColumnAlign( HWND, WORD, WORD ) ;
LRESULT NEAR SetFGColor( HWND, DWORD ) ;
LRESULT NEAR SetBGColor( HWND, DWORD ) ;
LRESULT NEAR SetColumnPosition( HWND, WORD, WORD ) ;
LRESULT NEAR SetNotifyWnd( HWND, HWND ) ;
LRESULT NEAR GetNumberColumns( HWND ) ;
LRESULT NEAR GetColumnPosition( HWND, WORD ) ;
LRESULT NEAR GetColumnAlign( HWND, WORD ) ;
HWND NEAR GetNotifyWnd( HWND ) ;
LRESULT NEAR EraseBackground( HWND, HDC ) ;
LRESULT NEAR PaintSlider( HWND ) ;
LRESULT NEAR SetFont( HWND, HFONT, LONG ) ;
LRESULT NEAR SizeSlider( HWND, WORD, LONG ) ;
LRESULT NEAR MouseMove( HWND, WORD, LONG ) ;
LRESULT NEAR MouseLButtonDown( HWND, WORD, LONG ) ;
LRESULT NEAR MouseLButtonUp( HWND, WORD, LONG ) ;
VOID NEAR UpdateSliderPos( HDC, LPRECT, int, int ) ;

VOID NEAR AddStyleString( LPSTR, int ) ;

// function prototypes (exported)
LRESULT FAR PASCAL SliderWndProc( HWND, WORD, WPARAM, LPARAM ) ;

//*** EOF: slidpriv.h
