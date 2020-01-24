//***************************************************************************
//
//  Module: facelift.h
//
//  Purpose:
//     This DLL provides additional GUI support functions.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              8/ 6/91   BryanW      Wrote it.
//             12/11/91   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

/************************************************************************
 *                         COLOR.C
 ************************************************************************/

#ifndef RC_INVOKED

// exported macros

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
LPARAM FAR PASCAL ColorCBOwnerDraw( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL ColorCBSetup( HWND ) ;
DWORD FAR PASCAL ColorCBGet( HWND ) ;
void FAR PASCAL ColorCBSetCurSel (HWND, DWORD ) ;

#endif

/************************************************************************
 *                         CONVERT.C
 ************************************************************************/

#ifndef RC_INVOKED

int FAR PASCAL AtoI( const LPSTR lpszBuf ) ;
DWORD FAR PASCAL AtoL( const LPSTR lpszBuf ) ;

#endif

/************************************************************************
 *                         DIALOG.C
 ************************************************************************/

#ifndef RC_INVOKED

// constant definitions

#define DLG_OK      1
#define DLG_CANCEL  0
#define DLG_ERROR   -1

// exported functions

VOID FAR PASCAL DlgCenter( HWND, HWND, BOOL ) ;
int FAR PASCAL GetRadioButton( HWND, int, int ) ;
int FAR PASCAL DoModalDialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC,
                                      DWORD ) ;
HWND FAR PASCAL DoModelessDialogParam( HINSTANCE, LPCSTR, HWND, DLGPROC,
                                       DWORD ) ;
void FAR PASCAL DlgSetItemLength( HWND, WORD, WORD ) ;
WORD FAR PASCAL LBGetCurSel( HWND ) ;
BOOL FAR PASCAL LBGetText( HWND, WORD, LPSTR, WORD ) ;
BOOL FAR PASCAL LBAddString( HWND, LPSTR ) ;
WORD FAR PASCAL LBFindString( HWND, WORD, LPSTR ) ;
WORD FAR PASCAL LBSelectString( HWND, WORD, LPSTR ) ;

#endif

/************************************************************************
 *                         PROFILE.C
 ************************************************************************/

#ifndef RC_INVOKED

// constant definitions

#define SEEK_SET            0
#define SEEK_CUR            1
#define SEEK_END            2

// data types

typedef BOOL (FAR PASCAL *LPENUMPROFSECTPROC)( LPSTR, LPARAM ) ;

// function prototypes

BOOL FAR PASCAL GetPrivateProfileTrueFalse( LPSTR, LPSTR, LPSTR, LPSTR ) ;
WORD FAR PASCAL GetPrivateProfileWord( LPSTR, LPSTR, LPSTR, LPSTR ) ;
DWORD FAR PASCAL GetPrivateProfileDW( LPSTR, LPSTR, LPSTR, LPSTR ) ;
BOOL FAR PASCAL WritePrivateProfileTrueFalse( LPSTR, LPSTR, BOOL, LPSTR ) ;
BOOL FAR PASCAL WritePrivateProfileWord( LPSTR, LPSTR, WORD, LPSTR ) ;
BOOL FAR PASCAL WritePrivateProfileDW( LPSTR, LPSTR, DWORD, LPSTR ) ;
BOOL FAR PASCAL EnumProfileSections( LPSTR, LPENUMPROFSECTPROC, LPARAM ) ;

#endif

/************************************************************************
 *                         SHADOW.C
 ************************************************************************/

#ifndef RC_INVOKED

// function prototypes (exported)

VOID FAR PASCAL DrawFaceFrame( HDC, LPRECT, WORD );
VOID FAR PASCAL Draw3DRect( HDC, LPRECT, WORD, WORD );
VOID FAR PASCAL Draw3DLine( HDC, WORD, WORD, WORD, WORD, WORD );
BOOL FAR PASCAL DrawDropShadow( HDC, HWND, short ) ;
BOOL FAR PASCAL ExtTextOut3D( HDC, short, short, WORD, LPRECT, LPSTR,
                              short, LPINT, WORD ) ;

BOOL FAR PASCAL TextOut3D( HDC, short, short, LPSTR, short, WORD, WORD ) ;
VOID FAR PASCAL Draw3DLine1( HDC, WORD, WORD, WORD, WORD ) ;

BOOL FAR PASCAL ShadowControl( HWND, WORD, WORD, LONG ) ;
VOID FAR PASCAL GetChildRect( HWND, LPRECT ) ;
VOID FAR PASCAL DrawChild3DRect( HDC, HWND, WORD, WORD ) ;
BOOL FAR PASCAL DrawChildren3D( HDC, HWND ) ;
BOOL FAR PASCAL HideSomeChildren( HWND ) ;

#endif

/************************************************************************
 *                        SLIDER.C
 ************************************************************************/

#ifndef RC_INVOKED

// control messages

#define SL_SETNUMCOLS       (WM_USER + 0x1000)
#define SL_SETCOLHEADER     (WM_USER + 0x1001)
#define SL_SETCOLALIGN      (WM_USER + 0x1002)
#define SL_SETFGCOLOR       (WM_USER + 0x1003)
#define SL_SETBGCOLOR       (WM_USER + 0x1004)
#define SL_SETCOLPOS        (WM_USER + 0x1005)
#define SL_GETNUMCOLS       (WM_USER + 0x1006)
#define SL_GETCOLHEADER     (WM_USER + 0x1007)
#define SL_GETCOLALIGN      (WM_USER + 0x1008)
#define SL_GETCOLPOS        (WM_USER + 0x1009)

// notification messages

#define SLN_STARTDRAG       1
#define SLN_INDRAG          2
#define SLN_ENDDRAG         3

// slider text alignment

#define SA_LEFT             0
#define SA_RIGHT            1
#define SA_CENTER           2

#endif

// styles for slider

#define SLS_ADJUSTSIZE       0x0001L
#define SLS_3DLOOK_IN        0x0002L
#define SLS_3DLOOK_OUT       0x0004L

/************************************************************************
 *                        STATUS.C
 ************************************************************************/

#ifndef RC_INVOKED

// control messages

#define ST_GETSTATUSWIDTH (WM_USER + 0x1100)
#define ST_SETSTATBOXSIZE (WM_USER + 0x1101)
#define ST_SETSTATBOX     (WM_USER + 0x1102)
#define ST_SETBORDER      (WM_USER + 0x1103)
#define ST_SETJUSTIFY     (WM_USER + 0x1104)
#define ST_GETHEIGHT      (WM_USER + 0x1105)
#define ST_SETCOLOR       (WM_USER + 0x1106)

WORD FAR PASCAL StatusGetWidth( HWND, LPSTR ) ;

// Macros for setting up stat boxes

#define StatusSetStatBoxSize(hWnd,nID,nWidth) SendMessage(hWnd,ST_SETSTATBOXSIZE,nID,(LONG)nWidth)
#define StatusSetStatBox(hWnd,nID,lpszBuf)    SendMessage(hWnd,ST_SETSTATBOX,nID,(LONG)(LPSTR)lpszBuf) 

#endif

// status styles

#define STS_3DLOOK_IN        0x0002L
#define STS_3DLOOK_OUT       0x0004L

/************************************************************************
 *                           BMPBTN.C
 ************************************************************************/

#ifndef RC_INVOKED

// data structures

typedef struct tagBMPBTNINFO
{
   HINSTANCE  hInstance ;
   LPCSTR     bmpUP, bmpDN, lpText ;

} BMPBTNINFO, FAR *LPBMPBTNINFO ;

// control messages

#define BB_SETTEXTCOLOR      (WM_USER + 0x1200)
#define BB_SETFACECOLOR      (WM_USER + 0x1201)
#define BB_SETSHADOWCOLOR    (WM_USER + 0x1202)
#define BB_SETFOCUSCOLOR     (WM_USER + 0x1203)
#define BB_SETBMPBTNINFO     (WM_USER + 0x1204)

#endif

// button styles

// note: these flags must be in the second hibyte... lobyte
// is reserved for standard button styles

#define BS_STRETCH  0x0100L
#define BS_NOFOCUS  0x0200L
#define BS_STATIC   0x0400L

/************************************************************************
 *                            BMUTIL.C
 ************************************************************************/

//
// wStyle flags for Translate functions
//
#define BM_NORMAL    0x0000
#define BM_DISABLED  0x0001

// function prototypes (exported)

HBITMAP FAR PASCAL CopyBitmap( HBITMAP ) ;
HBITMAP FAR PASCAL CopyTranslate( HBITMAP, COLORREF, short, short, WORD ) ;
VOID FAR PASCAL ColorTranslateBitmap( HBITMAP, COLORREF, COLORREF, WORD ) ;
VOID FAR PASCAL ColorTranslateDC( HDC, LPBITMAP, COLORREF, COLORREF, WORD ) ;

/************************************************************************
 *                            ABOUT.C
 ************************************************************************/

#ifndef RC_INVOKED

typedef struct tagABOUTCREATESTRUCT
{
   HICON    hAppIcon ;      // handle to application icon (may be NULL)
   HBITMAP  hAppBitmap ;    // handle to bitmap (may be NULL)
   LPSTR    lpszAppName ;   // pointer to appname
   LPSTR    lpszVersion ;   // pointer to version string
   LPSTR    lpszByLine ;    // pointer to byline (two lines possible)

} ABOUTCREATESTRUCT, FAR *LPABOUTCREATESTRUCT ;

// function prototypes (exported)

BOOL FAR PASCAL AboutBox( HWND, LPABOUTCREATESTRUCT ) ;

#endif

/************************************************************************
 *                            FONTS.C
 ************************************************************************/

#ifndef RC_INVOKED

// constant declarations

#define ANSI_FONTTYPE         0x0004
#define SYMBOL_FONTTYPE       0x0008
#define SHIFTJIS_FONTTYPE     0x0010
#define OEM_FONTTYPE          0x0020      

#define FIXED_FONTTYPE        0x0040
#define VARIABLE_FONTTYPE     0x0080
#define DEFAULT_FONTTYPE      (RASTER_FONTTYPE | DEVICE_FONTTYPE |\
                               ANSI_FONTTYPE | SYMBOL_FONTTYPE |\
                               SHIFTJIS_FONTTYPE | OEM_FONTTYPE |\
                               FIXED_FONTTYPE | VARIABLE_FONTTYPE)


#define FONTSEL_DEFAULT       0x0000
#define FONTSEL_USEPRINTER    0x0001
#define FONTSEL_NOPRINTER     0x0002
#define FONTSEL_NOFGCOLOR     0x0004
#define FONTSEL_NOBGCOLOR     0x0008
#define FONTSEL_NODEFAULT     0x0010
#define FONTSEL_NOBOLD        0x0020
#define FONTSEL_NOITALIC      0x0040
#define FONTSEL_NOUNDERLINE   0x0080
#define FONTSEL_NOSTRIKEOUT   0x0100

#define RCF_NORMAL            0x0000
#define RCF_ITALIC            0x0001
#define RCF_UNDERLINE         0x0002
#define RCF_STRIKEOUT         0x0004
#define RCF_BOLD              0x0008

// data structures

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

// function prototypes (exported)

VOID FAR PASCAL FontInitLogFont( LPLOGFONT ) ;
int FAR PASCAL GetTextMetricsCorrectly( HDC, LPTEXTMETRIC ) ;
HFONT FAR PASCAL ReallyCreateFont( HDC, LPSTR, short, WORD ) ;
BOOL FAR PASCAL FontFillSizeLB( HWND, HDC, LPSTR, WORD ) ;
BOOL FAR PASCAL FontFillFaceLB( HWND, HDC, WORD, WORD ) ;
WORD FAR PASCAL FontSelectDialog( HWND, LPSTR, HDC, WORD, WORD,
                                  LPEXTLOGFONT, LPEXTLOGFONT ) ;
BOOL FAR PASCAL FontSelectIndirect( HDC, LPLOGFONT ) ;

#endif

/************************************************************************
 *                            WINTIME.C
 ************************************************************************/

#ifndef RC_INVOKED

#ifndef _INC_TIME
#include <time.h>
#endif

LPSTR FAR PASCAL TimeGetCurDate( LPSTR, WORD ) ;
LPSTR FAR PASCAL TimeGetCurTime( LPSTR, WORD ) ;
LPSTR FAR PASCAL TimeFormatDate( LPSTR, struct tm FAR *, WORD ) ;
LPSTR FAR PASCAL TimeFormatTime( LPSTR, struct tm FAR *, WORD ) ;
void FAR PASCAL TimeResetInternational( void ) ;

#define DATE_SHORTDATE   0x0001
#define DATE_NODAYOFWEEK 0x0002

#define TIME_24HOUR      0x0001
#define TIME_12HOUR      0x0002
#define TIME_NOSECONDS   0x0004

#endif

/************************************************************************
 *                            PRINTER.C
 ************************************************************************/

#ifndef RC_INVOKED

BOOL FAR PASCAL PrintGetDefaultPrn( LPSTR, LPSTR, LPSTR ) ;
HDC FAR PASCAL GetDefaultPrnDC( VOID ) ;

#endif

/************************************************************************
 *                            WSSCANF.C
 ************************************************************************/

#ifndef RC_INVOKED

int FAR _cdecl wsscanf( LPSTR, LPSTR, ... ) ;

#endif

//***************************************************************************
//  End of File: facelift.h
//***************************************************************************
