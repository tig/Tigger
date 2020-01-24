/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  cmndll.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

#include <wdb.h>

/************************************************************************
 *                          CMNDLL.C
 ************************************************************************/

HANDLE FAR PASCAL GetCMNDLLhInst( void ) ;

HCURSOR FAR PASCAL SetHourGlass( void ) ;
HCURSOR FAR PASCAL ResetHourGlass( HCURSOR hCur ) ;

/************************************************************************
 *                          SELPRN.C
 ************************************************************************/
// Exported defins
#define SELPRN_STANDARD    0x0001
#define SELPRN_USEDEFAULT  0x0002
#define SELPRN_GETDEFAULT  0x0004
#define SELPRN_ALLOWSETUP  0x0008

#define DLG_CANCEL     IDCANCEL
#define DLG_OK         IDOK
#define DLG_ERROR      -1

int FAR PASCAL
PrintSelPrnDialog( HWND hwndParent,
                       LPSTR lpszTitle,
                       LPSTR lpszDriverName,
                       LPSTR lpszDeviceName,
                       LPSTR lpszPort,                       
                       WORD  wFlags ) ;

int FAR PASCAL
PrintGetDefaultPrn( LPSTR lpszDriverName,
                        LPSTR lpszDeviceName,
                        LPSTR lpszPortName ) ;

HDC FAR PASCAL GetDefaultPrnDC( void ) ;

BOOL FAR PASCAL 
SelPrinterDlgProc( HWND hDlg,
                   unsigned iMessage,
                   WORD wParam,
                   LONG lParam ) ;

/************************************************************************
 *                         SHADOW.C
 ************************************************************************/
/* misc. control flag defines */
#define DRAW3D_IN           0x0100
#define DRAW3D_OUT          0x0200

#define DRAW3D_TOPLINE      0x0400
#define DRAW3D_BOTTOMLINE   0x0800
#define DRAW3D_LEFTLINE     0x1000
#define DRAW3D_RIGHTLINE    0x2000

// Windows.h defines these:
//#define ETO_GRAYED          0x0001
//#define ETO_OPAQUE          0x0002
//#define ETO_CLIPPED         0x0004

WORD FAR PASCAL ChildKind( HWND hwndChild ) ;
#define          CHILD_UNKNOWN         0
#define          CHILD_BUTTON          1
#define          CHILD_COMBO           2
#define          CHILD_DROPDOWNCOMBO   3
#define          CHILD_EDIT            4
#define          CHILD_LISTBOX         5
#define          CHILD_SCROLLBAR       6
#define          CHILD_TEXT            7
#define          CHILD_BLACKFRAME      8
#define          CHILD_BLACKRECT       9
#define          CHILD_GRAYFRAME       10
#define          CHILD_GRAYRECT        11
#define          CHILD_ICON            12
#define          CHILD_WHITEFRAME      13
#define          CHILD_WHITERECT       14
#define          CHILD_GROUPBOX        15

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




/* public function prototypes */
void FAR PASCAL DrawFaceFrame( HDC, LPRECT, WORD );
void FAR PASCAL Draw3DRect( HDC, LPRECT, WORD, WORD );
void FAR PASCAL Draw3DLine( HDC, WORD, WORD, WORD, WORD, WORD );
BOOL FAR PASCAL DrawDropShadow( HDC hDC, HWND hWnd, 
                                short ShadowWidth ) ;
BOOL FAR PASCAL
ExtTextOut3D( HDC hDC, short x, short y, WORD wOptions,
              LPRECT lpRect, LPSTR lpString,
              short nChars, LPINT lpDx, WORD wWidth) ;

BOOL FAR PASCAL
TextOut3D( HDC hDC, short xPos, short yPos, LPSTR lpsz,
           short nCount, WORD wWidth, WORD wStyle ) ;
void FAR PASCAL
Draw3DLine1( HDC hDC, WORD x, WORD y, WORD nLen, WORD wFlags ) ;

BOOL FAR PASCAL
  ShadowControl( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;
void FAR PASCAL GetChildRect( HWND hwndChild, LPRECT lprc ) ;
void FAR PASCAL DrawChild3DRect( HDC hDC, HWND hwndChild, WORD wWidth, WORD wFlags ) ;
BOOL FAR PASCAL DrawChildren3D( HDC hDC, HWND hWnd ) ;
BOOL FAR PASCAL HideSomeChildren( HWND hWnd ) ;


/************************************************************************
 *                         ABOUT.C
 ************************************************************************/
#define ABOUT_STANDARD   0x0001

BOOL FAR PASCAL
AboutDialog( HWND    hwndParent,
             LPSTR   lpszTitle,
             LPSTR   lpszAppName,
             LPSTR   lpszVersion,
             LPSTR   lpszDescription,
             WORD    wFlags ) ;


BOOL FAR PASCAL 
AboutDlgProc( HWND hDlg,
              unsigned iMessage,
              WORD wParam,
              LONG lParam ) ;

/************************************************************************
 *                         VERSION.C 
 ************************************************************************/
LPSTR FAR PASCAL
VerGetCMNDLLVersion( LPSTR lpszVersion ) ;

LPSTR FAR PASCAL
VerMakeVersionNumber( LPSTR   lpszOut,
                      WORD    wMajor,
                      WORD    wMinor,
                      WORD    wBeta ) ;
/************************************************************************
 *                         FILEOPEN.C
 ************************************************************************/
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

/************************************************************************
 *                       BMPBTN.C - bitmap button class
 ************************************************************************/

#define BS_STRETCH  0x8000L
#define BS_NOFOCUS  0x4000L
#define BS_STATIC   0x2000L

#define BMPBTN_BMPFLAG  '~'
#define BMPBTN_AUTOSELECTFLAG '%'
#define BMPBTN_RESNUMFLAG '#'
#define BMPBTN_FILEFLAG '^'

//
// User buttons                              
//
#define BB_SETTEXTCOLOR      (WM_USER+1+ 0x7000)
#define BB_SETFACECOLOR      (WM_USER+2+ 0x7000)
#define BB_SETSHADOWCOLOR    (WM_USER+3+ 0x7000)
#define BB_SETFOCUSCOLOR     (WM_USER+4+ 0x7000)

BOOL FAR PASCAL BmpBtnInit( HANDLE hPrev, HANDLE hInst );

/************************************************************************
 *                          STATUS.C STUFF
 ************************************************************************/
#define ST_GETSTATUSWIDTH (WM_USER + 1 + 0x7000)
#define ST_SETSTATBOXSIZE (WM_USER + 2 + 0x7000)
#define ST_SETSTATBOX     (WM_USER + 3 + 0x7000)
#define ST_SETBORDER      (WM_USER + 4 + 0x7000)
#define ST_SETJUSTIFY     (WM_USER + 5 + 0x7000)
#define ST_GETHEIGHT      (WM_USER + 6 + 0x7000)
#define ST_SETCOLOR       (WM_USER + 7 + 0x7000)

BOOL FAR PASCAL StatusInit( HANDLE hPrev, HANDLE hInstance ) ;

WORD FAR PASCAL StatusGetWidth( HWND hMsgWnd, LPSTR lpszText ) ;

//
// Macros for setting stat boxes up
//
#define StatusSetStatBoxSize(hWnd,nID,nWidth) SendMessage(hWnd,ST_SETSTATBOXSIZE,nID,(LONG)nWidth)
#define StatusSetStatBox(hWnd,nID,lpszBuf)    SendMessage(hWnd,ST_SETSTATBOX,nID,(LONG)(LPSTR)lpszBuf) 


/************************************************************************
 *                          TIME.C STUFF
 ************************************************************************/
LPSTR FAR PASCAL TimeGetCurDate( LPSTR lpszDate, WORD wFlags ) ;
LPSTR FAR PASCAL TimeGetCurTime( LPSTR lpszTime, WORD wFlags ) ;
LPSTR FAR PASCAL TimeFormatDate( LPSTR lpszDate,
                                 struct tm FAR *pTM,
                                 WORD wFlags ) ;

LPSTR FAR PASCAL TimeFormatTime( LPSTR lpszTime,
                                 struct tm FAR *pTM,
                                 WORD wFlags ) ;
void FAR PASCAL TimeResetInternational( void ) ;

#define DATE_SHORTDATE   0x0001
#define DATE_NODAYOFWEEK 0x0002

#define TIME_24HOUR      0x0001
#define TIME_12HOUR      0x0002
#define TIME_NOSECONDS   0x0004

/************************************************************************
 *                       STRING.C FUNCTIONS
 ************************************************************************/

int FAR PASCAL AtoI( const LPSTR lpszBuf ) ;
DWORD FAR PASCAL AtoL(const LPSTR lpszBuf) ;

/************************************************************************
 *                        FILE.C FUNCTIONS
 ************************************************************************/
int FAR PASCAL FileSetPath( const LPSTR lpszPath )  ;


/************************************************************************
 *                        DLGHELP.C FUNCTIONS
 ************************************************************************/

WORD FAR PASCAL
DlgWhichRadioButton( HWND hDlg, WORD wFirstID, WORD wLastID, WORD wDefault ) ;

void FAR PASCAL
DlgSetItemLength( HWND hDlg, WORD wID, WORD wLength ) ;

void FAR PASCAL
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

WORD FAR PASCAL
LBGetCurSel( HWND hLB ) ;

BOOL FAR PASCAL
LBGetText( HWND hLB, WORD wItem, LPSTR lpszText, WORD wMax ) ;

BOOL FAR PASCAL
LBAddString( HWND hLB, LPSTR lpszText ) ;

WORD FAR PASCAL
LBFindString( HWND hLB, WORD wStart, LPSTR lpszPrefix ) ;

WORD FAR PASCAL
LBSelectString( HWND hLB, WORD wStart, LPSTR lpszPrefix ) ;

short FAR _cdecl ErrorResBox( HWND hWnd, HANDLE hInst,
                              WORD wFlags, WORD idAppName,
                              WORD idErrorStr, ... ) ;



/************************************************************************
 *                      FONT.C - FONT SELECTION FUNCTIONS
 ************************************************************************/

#define FONT_MAX_SIZES  20

typedef struct tagEXTLOGFONT
{
   int       lfHeight;
   int       lfWidth;
   int       lfEscapement;
   int       lfOrientation;
   int       lfWeight;
   BYTE      lfItalic;
   BYTE      lfUnderline;
   BYTE      lfStrikeOut;
   BYTE      lfCharSet;
   BYTE      lfOutPrecision;
   BYTE      lfClipPrecision;
   BYTE      lfQuality;
   BYTE      lfPitchAndFamily;
   BYTE      lfFaceName[LF_FACESIZE];
   DWORD     lfFGcolor ;
   DWORD     lfBGcolor ;
} EXTLOGFONT, *PEXTLOGFONT, FAR *LPEXTLOGFONT ;

typedef struct tagENUMFACE
{
   short          nNumFonts ;     // output: number of fonts
   GLOBALHANDLE   ghFaceNames ;   // global handle to list of facenames
   WORD           wFontType ;     // input:  type of fonts
} ENUMFACE, *PENUMFACE, FAR *LPENUMFACE ;

//
// Defines for wFont type are:
//
// #define RASTER_FONTTYPE       0x0001  (in windows.h)
// #define DEVICE_FONTTYPE       0x0002  (in windows.h)

// specify the following to get only those char sets.
#define ANSI_FONTTYPE         0x0004
#define SYMBOL_FONTTYPE       0x0008
#define SHIFTJIS_FONTTYPE     0x0010
#define OEM_FONTTYPE          0x0020      

// specify the following to force fixed/variable pitch only
#define FIXED_FONTTYPE        0x0040
#define VARIABLE_FONTTYPE     0x0080
#define DEFAULT_FONTTYPE      (RASTER_FONTTYPE | DEVICE_FONTTYPE |\
                               ANSI_FONTTYPE | SYMBOL_FONTTYPE |\
                               SHIFTJIS_FONTTYPE | OEM_FONTTYPE |\
                               FIXED_FONTTYPE | VARIABLE_FONTTYPE)


#define FONTSEL_USEPRINTER    0x0001
#define FONTSEL_NOPRINTER     0x0002
#define FONTSEL_NOFGCOLOR     0x0004
#define FONTSEL_NOBGCOLOR     0x0008
#define FONTSEL_NODEFAULT     0x0010
#define FONTSEL_NOBOLD        0x0020
#define FONTSEL_NOITALIC      0x0040
#define FONTSEL_NOUNDERLINE   0x0080
#define FONTSEL_NOSTRIKEOUT   0x0100
                                 
#define FONTSEL_DEFAULT  0

typedef struct tagENUMSIZE
{
   short       nNumSizes ;
   short       rgnSize[FONT_MAX_SIZES] ;
   short       rgnWidth[FONT_MAX_SIZES] ;
   LOGFONT     lf ;                       // it's nice to have these
   TEXTMETRIC  tm ;                       // two structures available..
   WORD           wFontType ;     // input:  type of fonts
} ENUMSIZE, *PENUMSIZE, FAR *LPENUMSIZE ;

HFONT FAR PASCAL
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags ) ;

#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008

int FAR PASCAL
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;


void FAR PASCAL FontInitLogFont( LPLOGFONT lplf ) ;

BOOL FAR PASCAL FontSelectIndirect( HDC hDC, LPLOGFONT lpLogFont ) ;

WORD FAR PASCAL
FontSelectDialog( HWND hwndParent, LPSTR lpszTitle,
                  HDC  hDC,
                  WORD wFontType,
                  WORD wFlags, LPEXTLOGFONT lpLF, LPEXTLOGFONT lplfDefault ) ;

BOOL FAR PASCAL
   FontFillFaceLB( HWND hLB, HDC hDC, WORD wFontType, WORD wFlags ) ;

BOOL FAR PASCAL
   FontFillSizeLB( HWND hLB, HDC hDC, LPSTR lpszFaceName, WORD wFlags ) ;

/************************************************************************
 *                   COLOR.C - Color selection combo box
 ************************************************************************/

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


DWORD FAR PASCAL GetHighlightColor( DWORD rgb ) ;
DWORD FAR PASCAL GetShadowColor( DWORD rgb ) ;
DWORD FAR PASCAL MapTextColor( DWORD rgb ) ;

LONG FAR PASCAL
ColorCBOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL FAR PASCAL ColorCBSetup( HWND hCB ) ;

DWORD FAR PASCAL ColorCBGet( HWND hCB ) ;

void FAR PASCAL ColorCBSetCurSel( HWND hCB, DWORD dw ) ;
DWORD FAR PASCAL GetHighlightColor( DWORD rgb ) ;
DWORD FAR PASCAL GetShadowColor( DWORD rgb ) ;

/************************************************************************
 *                   LED.C - LED window control
 ************************************************************************/
#define LED_SETCOLOR       (WM_USER + 1 + 0x7000)
#define LED_GETCOLOR       (WM_USER + 2 + 0x7000)

#define LED_ROUND          (0x0100)
#define LED_HORZ           (0x0200)
#define LED_VERT           (0x0400)


/************************************************************************
 * End of File: cmndll.h
 ************************************************************************/

