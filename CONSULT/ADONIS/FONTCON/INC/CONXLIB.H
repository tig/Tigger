/************************************************************************
 *
 *     Project:  Windows Font Shopper Support Library
 *
 *
 *       Public include file for CONXLIB.DLL.
 *
 *       This include file requires the WINDOWS.H from the 3.1 SDK.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/


/*
 * NOTE:  Must be included after STRUCT.H
 */


#ifndef _CONXLIB_H_
#define _CONXLIB_H_

/*
 * Version stuff.
 */
#define _CONXLIB_SPEC_VER_  0x0100

VOID WINAPI ShopLibInit( VOID ) ;

int WINAPI AtoI(const LPSTR lpszBuf) ;
DWORD WINAPI AtoL(const LPSTR lpszBuf) ;

BOOL WINAPI bmTransBlt( HDC hdcD, short xD, short yD,
                            short dx, short dy, HDC hdcS,
                            short xS, short yS ) ;

VOID WINAPI
      bmColorTranslate( HBITMAP hbmSrc, COLORREF rgbOld,
                        COLORREF rgbNew, WORD wStyle ) ;

VOID WINAPI bmColorTranslateDC( HDC hdcBM, LPBITMAP lpBM,
                                  COLORREF rgbOld, COLORREF rgbNew,
                                  WORD  wStyle ) ;


/************************************************************************
 *                        DLGHELP.C FUNCTIONS
 ************************************************************************/

UINT WINAPI
DlgWhichRadioButton( HWND hDlg, UINT wFirstID, UINT wLastID, UINT wDefault ) ;

void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

short FAR _cdecl ErrorResBox( HWND hWnd, HINSTANCE hInst,
                              UINT wFlags, UINT idAppName,
                              UINT idErrorStr, ... ) ;

/************************************************************************
 *                        STATUS BAR 
 ************************************************************************/

#ifndef _STATUS_H_
#define _STATUS_H_

/*
 * This is the "class name" for the status custom control.  It is
 * important to make your class names unique because there are 1000's
 * of other Windows programmers out there who might create a window
 * class with the same name as yours.
 */
#define SZ_STATUSCLASSNAME    "Shopper.Status"


#define ST_GETSTATUSWIDTH (WM_USER + 0x01 + 0x7000)
#define ST_SETSTATBOXSIZE (WM_USER + 0x02 + 0x7000)
#define ST_SETSTATBOX     (WM_USER + 0x03 + 0x7000)
#define ST_SETBORDER      (WM_USER + 0x04 + 0x7000)
#define ST_SETJUSTIFY     (WM_USER + 0x05 + 0x7000)
#define ST_GETHEIGHT      (WM_USER + 0x06 + 0x7000)
#define ST_SETCOLOR       (WM_USER + 0x07 + 0x7000)
#define ST_SETDEFAULTTEXT (WM_USER + 0x08 + 0x7000)
#define ST_SETOWNERDRAW   (WM_USER + 0x09 + 0x7000)
#define ST_GETSTATBOXRECT (WM_USER + 0x0A + 0x7000)

/*
 * Onwer draw stuff
 */
#define ODT_STATUS   42

BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance, LPSTR lpszClassName ) ;

//
// Macros for setting stat boxes up. ID's are always USHORT.
//
#define StatusSetStatBoxSize(hWnd,nID,nWidth) SendMessage(hWnd,ST_SETSTATBOXSIZE,(UINT)nID,(LPARAM)nWidth)
#define StatusSetStatBox(hWnd,nID,lpszBuf)    SendMessage(hWnd,ST_SETSTATBOX,(UINT)nID,(LPARAM)(LPSTR)lpszBuf) 
#define StatusGetWidth( hWnd, lpszText )      (int)SendMessage(hWnd,ST_GETSTATUSWIDTH,0,(LPARAM)(LPSTR)lpszText )


#define RGBBLACK     RGB(0,0,0)
#define RGBWHITE     RGB(255,255,255)
#define RGBGRAY      RGB(128,128,128)
#define RGBLTGRAY    RGB(192,192,192)

#define RGBBLUE      RGB(0,0,128)
#define RGBLTBLUE    RGB(0,0,255)
#define RGBGREEN     RGB(0,128,0)

#define RGBCYAN      RGB(0,128,128)
#define RGBLTGREEN   RGB(0,255,0)
#define RGBLTCYAN    RGB(0,255,255)

#define RGBRED       RGB(128,0,0)
#define RGBMAGENTA   RGB(128,0,128)
#define RGBBROWN     RGB(128,128,0)

#define RGBLTRED     RGB(255,0,0)
#define RGBLTMAGENTA RGB(255,0,255)
#define RGBYELLOW    RGB(255,255,0)

#endif   // _STATUS_H_

/************************************************************************
 * SHADOW
 ***********************************************************************/

#ifndef _SHADOW_H_
#define _SHADOW_H_

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

/* public function prototypes */

VOID WINAPI tdDraw3DRect(    HDC hdc, LPRECT lprc, UINT wWidth, UINT wFlags );
VOID WINAPI tdDraw3DLine(    HDC hdc, UINT x, UINT y, UINT nLen, UINT wWidth, UINT wFlags );
BOOL WINAPI tdDrawDropShadow(HDC hDC, HWND hWnd, short nShadowWidth ) ;
VOID WINAPI tdDraw3DLine1( HDC hDC, UINT x, UINT y, UINT nLen, UINT wFlags ) ;

VOID WINAPI tdGetChildRect( HWND hwndChild, LPRECT lprc ) ;

DWORD WINAPI tdGetHighlightColor( DWORD rgb ) ;
DWORD WINAPI tdGetShadowColor( DWORD rgb ) ;

#endif // _SHADOW_H_

/************************************************************************
 * FONTUTIL
 ***********************************************************************/

#ifndef _FONTUTIL_H_
#define _FONTUTIL_H_

HFONT WINAPI
ReallyCreateFont( HDC hDC, LPSTR lpszFace, short nPointSize, WORD wFlags ) ;

HFONT WINAPI
   ReallyCreateFontEx( HDC hDC, LPSTR lpszFace, LPSTR lpszStyle, short nPointSize, WORD wFlags ) ;

#define RCF_NORMAL      0x0000
#define RCF_ITALIC      0x0001
#define RCF_UNDERLINE   0x0002
#define RCF_STRIKEOUT   0x0004
#define RCF_BOLD        0x0008
#define RCF_NODEFAULT   0x0010
#define RCF_TRUETYPEONLY 0x0011

int WINAPI
   GetTextMetricsCorrectly( HDC hDC, LPTEXTMETRIC lpTM ) ;

UINT WINAPI GetTextPointSize( HDC hDC ) ;
UINT WINAPI GetTextStyle( HDC hDC, UINT cbBuffer, LPSTR lpszStyle ) ;

UINT WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize );


#endif  // _FONT_H_

/************************************************************************
 * WSSCANF
 ***********************************************************************/

#ifndef _WSSCANF_H_
#define _WSSCANF_H_
// constant definitions

#define ALLOW_RANGE                        // allow "%[a-z]" - style 
                                           // scansets

#define LEFT_BRACKET  ('[' | ('a' - 'A'))  // 'lowercase' version

#define ASCII         32                   // # of bytes needed to 
                                           // hold 256 bits

// macro definitions

#define INC()            (++nCharCount, Inc( (LPSTR FAR *) &lpPtrBuffer ))
#define UN_INC( chr )    (--nCharCount, UnInc( chr, (LPSTR FAR *) &lpPtrBuffer ))
#define EAT_WHITE()      WhiteOut( (int FAR *) &nCharCount, (LPSTR FAR *) &lpPtrBuffer )
#define HEXTODEC( chr )  HexToDec( chr )

#define MUL10(x)         ( (((x)<<2) + (x))<<1 )

// function definitions

int FAR _cdecl wsscanf( LPSTR, LPSTR, ... ) ;
#endif //_WSSCANF_H_

/************************************************************************
 * LED
 ***********************************************************************/

#ifndef _LED_H_
#define _LED_H_

#define SZ_LEDCLASSNAME    "Shopper.LED"

#define LED_SETCOLOR       (WM_USER + 1 + 0x7000)
#define LED_GETCOLOR       (WM_USER + 2 + 0x7000)

#define LED_ROUND          (0x0100)
#define LED_HORZ           (0x0200)
#define LED_VERT           (0x0400)

#endif // _LED_H_

/*****************************************************************
 * DLGS.C
 *****************************************************************/
#ifndef _DLGS_H_
#define _DLGS_H_
BOOL WINAPI CustomerInfoDialog( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINISection, DWORD dwData2 );
BOOL WINAPI ShippingInfoDialog( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINISection, DWORD dwData2 );
BOOL WINAPI BillingInfoDialog ( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINISection, DWORD dwData2 );
BOOL WINAPI CommSettingsDialog( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINISection, DWORD dwData2 );
#endif

/*****************************************************************
 * BILLING.C
 *****************************************************************/
BOOL WINAPI Mod10Validation (LPSTR cardNumber, int count) ;

int WINAPI  StripSpaces (LPSTR oldNumber, LPSTR newNumber, int size) ;
VOID WINAPI StripCardDashes (LPSTR ccNumber) ;

/*****************************************************************
 * MISCDLGS.C
 *****************************************************************/

BOOL WINAPI GetUserIDNewPasswordDlg( HWND hwndParent, LPSTR szHelpFile, LPSTR szUserID, LPSTR szOldPassword, LPSTR szNewPassword ) ;
BOOL WINAPI GetUserIDPasswordDlg( HWND hwndParent, LPSTR szHelpFile, LPSTR szUserID, LPSTR szOldPassword );

BOOL WINAPI VerifyOrderInfoDlg( HWND hwndParent, LPSTR szHelpFile, LPSTR szAppName, LPCUSTINFO lpCustInfo, BOOL fVerifyShipping ) ;


/************************************************************************
 * Ini file helper routines
 ***********************************************************************/

#ifndef _INI_H_
#define _INI_H_

/*
 * Non-generic stuff
 */

BOOL WINAPI GetCustInfo( LPCUSTINFO lpCI, LPSTR lpszAppName ) ;

BOOL WINAPI SavePassword( LPCUSTINFO lpCI, LPSTR lpszNewPassword, LPSTR lpszAppName ) ;
BOOL WINAPI SaveUserID( LPCUSTINFO lpCI, LPSTR lpszNewUserID, LPSTR lpszAppName ) ;

BOOL WINAPI ReadCommSettings( LPCOMMSETTINGS lpCS, LPSTR szINIFile ) ;

/*
 * Generic (or semigeneric) stuff
 */
LONG WINAPI ParseOffNumber( LPSTR FAR *, LPINT );
BOOL WINAPI ParseOffString( LPSTR FAR *, LPSTR );
UINT WINAPI StripOffWhiteSpace( LPSTR );

// Same parameter lists as the WinSDK routines with the exception
// of a HFONT or a HWND instead of a LPSTR for the string
BOOL WINAPI WritePrivateProfileFont(LPSTR,LPSTR,HFONT,LPSTR);
BOOL WINAPI WritePrivateProfileWndPos(LPSTR,LPSTR,HWND,LPSTR);

UINT WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, UINT cbMax, LPSTR lpszFile ) ;

HFONT WINAPI GetPrivateProfileFont(LPSTR,LPSTR,LPSTR);
LPWINDOWPLACEMENT WINAPI GetPrivateProfileWndPos(LPSTR,LPSTR,LPSTR);

BOOL WINAPI WritePrivateProfileWORD(LPSTR,LPSTR,WORD,LPSTR);
BOOL WINAPI WritePrivateProfileDWORD(LPSTR,LPSTR,DWORD,LPSTR);

WORD WINAPI GetPrivateProfileWORD(LPSTR,LPSTR,WORD,LPSTR);
DWORD WINAPI GetPrivateProfileDWORD(LPSTR,LPSTR,DWORD,LPSTR);

// Not really a INI file thing, but oh well!

//#define GRCS(X) GETRCSTRING(X,ghInst)

LPSTR WINAPI GetRCString( UINT, HINSTANCE );
VOID WINAPI ExpandStringToPrintable( LPSTR lpString );
VOID WINAPI ShrinkStringFromPrintable( LPSTR lpString );

#endif // _INI_H_

/************************************************************************
 *                          WINTIME.C STUFF
 ************************************************************************/
LPSTR WINAPI TimeGetCurDate( LPSTR lpszDate, WORD wFlags ) ;
LPSTR WINAPI TimeGetCurTime( LPSTR lpszTime, WORD wFlags ) ;
LPSTR WINAPI TimeFormatDate( LPSTR lpszDate,
                                 struct tm FAR *pTM,
                                 WORD wFlags ) ;

LPSTR WINAPI TimeFormatTime( LPSTR lpszTime,
                                 struct tm FAR *pTM,
                                 WORD wFlags ) ;
void WINAPI TimeResetInternational( void ) ;

#define DATE_SHORTDATE   0x0001
#define DATE_NODAYOFWEEK 0x0002

#define TIME_24HOUR      0x0001
#define TIME_12HOUR      0x0002
#define TIME_NOSECONDS   0x0004

/************************************************************************
 *                          SLIDER.C STUFF
 ************************************************************************/

#define SZ_SLIDERCLASS        "SHOPPPER.SLIDER"

    #define SL_SETNUMCOLS       (WM_USER + 0x1000)
    #define SL_SETCOLHEADER     (WM_USER + 0x1001)
    #define SL_SETCOLALIGN      (WM_USER + 0x1002)
    #define SL_SETFGCOLOR       (WM_USER + 0x1003)
    #define SL_SETBGCOLOR       (WM_USER + 0x1004)
    #define SL_SETCOLPOS        (WM_USER + 0x1005)
    #define SL_SETNOTIFYWND     (WM_USER + 0x1006)
    #define SL_GETNUMCOLS       (WM_USER + 0x1007)
    #define SL_GETCOLHEADER     (WM_USER + 0x1008)
    #define SL_GETCOLALIGN      (WM_USER + 0x1009)
    #define SL_GETCOLPOS        (WM_USER + 0x100A)
    #define SL_GETNOTIFYWND     (WM_USER + 0x100B)

    // notification messages

    #define SLN_STARTDRAG       1
    #define SLN_ENDDRAG         2
    #define SLN_INDRAG          3

    // slider text alignment

    #define SA_LEFT             0
    #define SA_RIGHT            1
    #define SA_CENTER           2

// styles for slider

#define SLS_ADJUSTSIZE       0x0001L
#define SLS_3DLOOK_IN        0x0002L
#define SLS_3DLOOK_OUT       0x0004L


/************************************************************************
 *                          TOOLBAR.C STUFF
 ************************************************************************/

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

//#include <windows.h>
//#include <windowsx.h>

BOOL WINAPI RegisterToolbarClass( HINSTANCE );

#define SZ_TOOLBARCLASS "SHOPPER.TOOLBAR"

//*** Messages
        // lParam -> LPDIMENS
    #define TBM_SETDIMENS           (WM_USER+98)

        // lParam -> LPDIMENS
    #define TBM_GETDIMENS           (WM_USER+99)

        // lParam -> LPBTNINFO, NULL for SPACER
    #define TBM_ADDBUTTON           (WM_USER+100)

        // wParam -> wType (1-BTNID,2-INDEX), lParam==ID | Index
    #define TBM_REMOVEBUTTON        (WM_USER+101)

        // wParam==ID, lParam == 0-UP, 1-DOWN
    #define TBM_SETBTNSTATE         (WM_USER+102)

        // wParam==ID, lReturn == 0-UP, 1-DOWN
    #define TBM_GETBTNSTATE         (WM_USER+103)

        // wParam==ID, lParam == TRUE/FALSE
    #define TBM_ENABLEBTN           (WM_USER+104)

        // wParam==ID, lParam == lData, lReturn = lOldData
    #define TBM_SETBTNDATA          (WM_USER+105)

        // wParam==ID, lReturn == lData             
    #define TBM_GETBTNDATA          (WM_USER+106)

        // wParam==ID, lParam == lpText 
    #define TBM_SETBTNTEXT          (WM_USER+107)

        // wParam==ID, lReturn == nLen
    #define TBM_GETBTNTEXTLENGTH    (WM_USER+108)

        // wParam==ID, lParam == lBuffer
    #define TBM_GETBTNTEXT          (WM_USER+109)

    typedef struct
    {
        int     nSpacerX;
        int     nButtonX;
        int     nButtonY;
    } DIMENS, FAR *LPDIMENS;

//*** Button stuff
    typedef struct
    {
        UINT    uiBtnID;        // 0 is a SPACER!
        UINT    uiBtnStyle;
        UINT    uiBtnState;
        HBITMAP hBitmap;        
        int     nBmpX;          // Is width of space if space, 0 use def
        int     nBmpY;
        DWORD   dwData;
    } BTNINFO, FAR *LPBTNINFO;

    typedef struct
    {
        UINT    uiBtnID;
        UINT    uiBtnStyle;
        UINT    uiBtnState;
        HFONT   hFont;          // If NULL use the window's font
        int     nWidth;
        int     nJustify;
        LPSTR   lpText;
    } BTNTEXT, FAR *LPBTNTEXT;


    #define BTNS_TOGGLE     0x0001
    #define BTNS_TOGGLEEX   0x0002
    #define BTNS_MENUID     0x0004
    #define BTNS_RIGHTBTN   0x8000
    #define BTNS_TEXT       0x0008
    #define BTNS_TEXTBORDER 0x0010

    #define BTNJ_LEFT       0x0000
    #define BTNJ_CENTER     0x0001
    #define BTNJ_RIGHT      0x0002

    #define BTN_UP          0x0000
    #define BTN_DOWN        0x0001
    #define BTN_ENABLED     0x0000
    #define BTN_DISABLED    0x0002

//*** hBitmap should look like.  nBmpX & Y are offsets into the bitmap
//        nBmpX
//  nBmpY o---------o----------o-------------o
//        | Up AND  | Down AND | Disable AND |
//        o---------o---------o-------------o
//        | Up OR   | Down OR  | Disabled OR |
//        o---------o----------o-------------o
//****

#endif // _TOOLBAR_H_

///* Print */
//BOOL WINAPI Print( HWND hwndParent, LPSTR szAppName, LPSTR szFileName,
//                       DWORD dw1, DWORD dw2 ) ;


#endif //_CONXLIB_H_

/************************************************************************
 * End of File: CONXLIB.H
 ***********************************************************************/

