/************************************************************************
 *
 *               Copyright (c) 1991-1992, Charles E. Kindel, Jr.
 *               All Rights Reserved.
 *
 *   Revisions:
 *
 ***********************************************************************/

#ifndef _CEK_H_
#define _CEK_H_

/*
 * The SPEC version can be used by an application to determine if the
 * installed version of this DLL is old or new.  The app should call
 * tdCheckVersion( _CEK_SPEC_VER_ ).  If this function returns
 * FALSE then the DLL installed is old and should not be called.
 *
 * The version number is calculated with the following formula:
 *
 *  #define _CEK_SPEC_VER_   (VER_MAJOR * 100)
 *  dwVersion   = MAKELONG( (_CEK_SPEC_VER_) + (VER_MINOR), VER_BUILD ) ;
 *
 */
#define _CEK_SPEC_VER_   ((WORD)200)

/* cekCheckVersion()
 *
 * Returns TRUE if dwVer is less than or equal to the spec version of
 * CEK.  Otherwise it returns false.
 *
 * The dwVer value passed should be _CEK_SPEC_VER_ defined above.
 *
 * The idea here is that CEK.DLL will always retain backwards
 * compatibility. If this function returns FALSE, an *older* version
 * of CEK is running than what the app expects.
 */
BOOL WINAPI cekCheckVersion( DWORD dwVer ) ;

/* cekGetVersion()
 *
 * Returns the version number of CEK and copies a formatted
 * version string into the buffer pointed to by lpszVersion in the
 * form "%d.%02d[.%03d]"
 *
 * Examples:
 *
 *    If the version of CEK is 1.16.256 the return value will
 *    be 0x00FF0074 and the string will be "1.26.256".
 *
 *    If the version of CEK is 2.11.000 the return value will be
 *    0x000000D3 and the string will be "2.11".
 */
DWORD WINAPI cekGetVersion( LPSTR lpszVersion )  ;

#ifndef _INI_H_
#define _INI_H_

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


/* Custom control definitions:
 */
#ifndef _STATUS_H_
#define _STATUS_H_

/*
 * This is the "class name" for the status custom control.  It is
 * important to make your class names unique because there are 1000's
 * of other Windows programmers out there who might create a window
 * class with the same name as yours.
 */
#define SZ_STATUSCLASSNAME    "CEK.Status"


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

//
// Macros for setting stat boxes up. ID's are always USHORT.
//
#define StatusSetStatBoxSize(hWnd,nID,nWidth) SendMessage(hWnd,ST_SETSTATBOXSIZE,(UINT)nID,(LPARAM)nWidth)
#define StatusSetStatBox(hWnd,nID,lpszBuf)    SendMessage(hWnd,ST_SETSTATBOX,(UINT)nID,(LPARAM)(LPSTR)lpszBuf) 
#define StatusGetWidth( hWnd, lpszText )      (int)SendMessage(hWnd,ST_GETSTATUSWIDTH,0,(LPARAM)(LPSTR)lpszText )

#ifndef RGBBLACK
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
#endif
#endif   // _STATUS_H_


#ifndef _LED_H_
#define _LED_H_

#define SZ_LEDCLASSNAME    "CEK.LED"

#define LED_SETCOLOR       (WM_USER + 1 + 0x7000)
#define LED_GETCOLOR       (WM_USER + 2 + 0x7000)

#define LED_ROUND          (0x0100)
#define LED_HORZ           (0x0200)
#define LED_VERT           (0x0400)
#endif // _LED_H_

#ifndef _SLIDER_H_
#define _SLIDER_H_

#define SZ_SLIDERCLASSNAME        "CEK.SLIDER"

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

#endif // _SLIDER_H_

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#define SZ_TOOLBARCLASSNAME "CEK.TOOLBAR"

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


#ifndef _FONTUTIL_H_
#define _FONTUTIL_H_

HFONT WINAPI
   ReallyCreateFontEx( HDC hDC, LPSTR lpszFace, LPSTR lpszStyle, short nPointSize, WORD wFlags ) ;

HFONT WINAPI GetMatchingFont( HDC hdcScreen, HDC hdcPrn, LPSTR lpszFace, LPSTR lpszStyle, short nPointSize, WORD wFlags ) ;

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
UINT WINAPI GetTextFullName( HDC hDC, UINT cbBuffer, LPSTR lpszName ) ;
UINT WINAPI GetTextExtentABCPoint( HDC hDC, LPSTR lpsz, int cb, LPSIZE lpSize ) ;
#endif // _FONTUTIL_H_

#endif // _CEK_H_

#ifndef _MEMMAN_H_
#define _MEMMAN_H_

// Hungarian is hh (ie.  hhHeap)
typedef DWORD HHEAP;

typedef DWORD HHANDLE;

#define HT_SHARED    0x0001
#define HT_MULTIPLE  0x0002

HHEAP    WINAPI cekmmHeapCreate( UINT uiSize, UINT uiFlags ) ;
VOID     WINAPI cekmmHeapDestroy( HHEAP hhHeap  ) ;
HHANDLE  WINAPI cekmmHeapAlloc( HHEAP hhHeap, UINT uiFlags, UINT uiSize  ) ;
HHANDLE  WINAPI cekmmHeapReAlloc( HHEAP hhHeap,HHANDLE hhMem,UINT uiSize, UINT uiFlags ) ;
HHANDLE  WINAPI cekmmHeapFree( HHEAP hhHeap, HHANDLE hhMem ) ;
LPVOID   WINAPI cekmmHeapLock( HHEAP hhHeap, HHANDLE hhMem ) ;
BOOL     WINAPI cekmmHeapUnlock( HHEAP hhHeap, HHANDLE hhMem ) ;
LPVOID   WINAPI cekmmHeapAllocLock( HHEAP hhHeap, UINT uiFlags, UINT uiSize ) ;
LPSTR    WINAPI cekmmHeapAllocString( HHEAP hhHeap, UINT uiFlags,LPSTR lpString ) ;
LPVOID   WINAPI cekmmHeapUnlockFree( HHEAP hhHeap, LPVOID lpMem ) ;
UINT     WINAPI cekmmHeapCompact( HHEAP hhHeap, UINT wMinFree ) ;
UINT     WINAPI cekmmHeapFlags( HHEAP hhHeap, HHANDLE hhMem ) ;
HHANDLE  WINAPI cekmmHeapHandle( HHEAP hhHeap, LPVOID lpMem ) ;
HHANDLE  WINAPI cekmmHeapDiscard( HHEAP hhHeap,HHANDLE hhMem ) ;
UINT     WINAPI cekmmHeapSize( HHEAP hhHeap, HHANDLE hhMem ) ;
HHANDLE  WINAPI cekmmHeapSetDS( HHEAP hhHeap ) ;
DWORD    WINAPI cekmmHeapItems( HHEAP hhHeap ) ;
VOID     WINAPI cekmmHeapDumpDebug( HHEAP hhHeap ) ;

/************************** List Services ******************************/
// Hungarian is hlst (ie.  hlstList)
typedef  DWORD  HLIST;

typedef void far *LPNODE;
typedef int       (WINAPI *SORTPROC)(LPNODE,LPNODE,LONG);
typedef LPVOID    (WINAPI *DUMPPROC)(LPNODE);

#define LIST_INSERTHEAD     ((LPNODE)0L)
#define LIST_INSERTTAIL     ((LPNODE)1L)         
#define LIST_SORT_BUBBLE    0x0000

HHEAP  WINAPI cekmmListHeap( HLIST hlstList ) ;
HLIST  WINAPI cekmmListCreate(  HHEAP hhHeap  ) ;
VOID   WINAPI cekmmListDestroy( HLIST hlstList ) ;
VOID   WINAPI cekmmListFreeList( HLIST hlstList ) ;
LPNODE WINAPI cekmmListAllocNode( HLIST hlstList, LPVOID lpMem, UINT uiSize ) ;
VOID   WINAPI cekmmListFreeNode(  HLIST hlstList, LPNODE lpNode  ) ;
BOOL   WINAPI cekmmListInsertNode( HLIST hlstList, LPNODE lpNode, LPNODE lpPrevNode ) ;
BOOL   WINAPI cekmmListInsertSorted( HLIST hlstList,LPNODE lpNode,SORTPROC lpfnSort, LPARAM lData ) ;
LPNODE WINAPI cekmmListAllocInsertNode(  HLIST hlstList, LPVOID lpMem, UINT uiSize, LPNODE lpPrevNode  ) ;
VOID   WINAPI cekmmListRemoveNode(  HLIST hlstList, LPNODE lpNode  ) ;
VOID   WINAPI cekmmListDeleteNode(  HLIST hlstList, LPNODE lpNode  ) ;
LPNODE WINAPI cekmmListNextNode(  HLIST hlstList, LPNODE lpNode  ) ;
LPNODE WINAPI cekmmListPrevNode(  HLIST hlstList, LPNODE lpNode  ) ;
LPNODE WINAPI cekmmListFirstNode(  HLIST hlstList  ) ;
LPNODE WINAPI cekmmListLastNode(  HLIST hlstList  ) ;
DWORD  WINAPI cekmmListNode2Item( HLIST hlstList, LPNODE lpNode ) ;
LPNODE WINAPI cekmmListItem2Node( HLIST hlstList, DWORD lItem ) ;
DWORD  WINAPI cekmmListItems( HLIST hlstList ) ;
VOID   WINAPI cekmmListSwapNodes(  HLIST hlstList, LPNODE lp1, LPNODE lp2  ) ;
BOOL   WINAPI cekmmListSort( HLIST hlstList, SORTPROC lpfnSort, UINT wType, LPARAM lData ) ;
VOID   WINAPI cekmmListDumpDebug( HLIST hlstList, DUMPPROC lpfnDump ) ;

/************************** Stack Services *****************************/
// Hungarian is hstk (ie.  hstkStack)
typedef  DWORD  HSTACK;

HSTACK   WINAPI cekmmStackCreate(HHEAP);
VOID     WINAPI cekmmStackDestroy(HSTACK);
VOID     WINAPI cekmmStackFreeStack(HSTACK);
LPNODE   WINAPI cekmmStackAllocNode(HSTACK,LPSTR,WORD);
LPNODE   WINAPI cekmmStackFreeNode(HSTACK, LPNODE);
BOOL     WINAPI cekmmStackPush(HSTACK, LPNODE);
LPNODE   WINAPI cekmmStackAllocPush(HSTACK,LPSTR,WORD);

LPNODE   WINAPI cekmmStackPop(HSTACK);
LPNODE   WINAPI cekmmStackPeek(HSTACK hstkStack, LONG lIndex);
   // Zero based from the top (Peek(0) == Pop() without removing)
LPNODE   WINAPI cekmmStackRemove(HSTACK hstkStack, LONG lndex);
   // Zero based from the top (Remove(0) == Pop())
LONG     WINAPI cekmmStackItems(HSTACK);
VOID     WINAPI cekmmStackDumpDebug(HSTACK,DUMPPROC);


/************************* Queue Services *****************************/
// Hungarian is hs (ie.  hqQueue)
typedef  DWORD  HQUEUE;

HQUEUE   WINAPI cekmmQueueCreate(HHEAP);
VOID     WINAPI cekmmQueueDestroy(HQUEUE);
VOID     WINAPI cekmmQueueFreeQueue(HQUEUE);
LPNODE   WINAPI cekmmQueueAllocNode(HQUEUE,LPSTR,WORD);
LPNODE   WINAPI cekmmQueueFreeNode(HQUEUE, LPNODE);
BOOL     WINAPI cekmmQueueInQ(HQUEUE, LPNODE);
LPNODE   WINAPI cekmmQueueAllocInQ(HQUEUE, LPSTR, WORD);
LPNODE   WINAPI cekmmQueueDeQ(HQUEUE);
LPNODE   WINAPI cekmmQueuePeek(HQUEUE hqQueue, LONG lIndex);
   // Zero based from the top (Peek(0) == DeQ() without removing)
LPNODE   WINAPI cekmmQueueRemove(HQUEUE hqQueue, LONG lIndex);
   // Zero based from the top (Remove(0) == DeQ())
LONG     WINAPI cekmmQueueItems(HQUEUE);
VOID     WINAPI cekmmQueueDumpDebug(HQUEUE,DUMPPROC);


/************************** Atom Services *****************************/
// Hungarian is hat (ie.  hatAtomTable)
typedef DWORD HATBL;

// Hungarian is ha (ie.   haAtom)
typedef DWORD HATOM;

HATBL    WINAPI cekmmAtomTableCreate(WORD wInitSize);
VOID     WINAPI cekmmAtomTableDestroy(HATBL );
HATOM    WINAPI cekmmAtomTableAddAtom( HATBL, LPSTR );
HATOM    WINAPI cekmmAtomTableDeleteAtom( HATBL, HATOM );
HATOM    WINAPI cekmmAtomTableFindAtom( HATBL, LPSTR );
WORD     WINAPI cekmmAtomTableGetAtomName( HATBL, HATOM, LPSTR, WORD );
VOID     WINAPI cekmmAtomTableDumpDebug(HATBL);


/************************ String Services *****************************/

BOOL WINAPI cekmmIsUsefulString( LPSTR lpText );
BOOL WINAPI cekmmIsNumber( LPSTR lpS );
VOID WINAPI cekmmSetExtension( LPSTR lpFile, LPSTR lpExt );
void WINAPI cekmmHackSpaces( LPSTR lpszLine );
LONG WINAPI cekmmGetValue(LPSTR FAR *lplp);

#endif // _MEMMAN_H_

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

#ifndef _DLGHELP_H_
UINT WINAPI                                                                   
DlgWhichRadioButton( HWND hDlg, UINT wFirstID, UINT wLastID, UINT wDefault ) ;

short FAR _cdecl ErrorResBox( HWND hwnd, HINSTANCE hInst, UINT flags,
                        		UINT idAppName, UINT idErrorStr, ...) ;

void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient ) ;

#endif // _DLGHELP_H_

#ifndef _MUSCROLL_H_
/*
 * MUSCROLL.H
 *
 * Public definitions for application that use the MicroScroll.
 *  - Messages
 *  - Prototypes for Message API Functions
 *  - Notification codes.
 *  - Control Styles.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 *
 */



//Control-specific messages
#define MSM_HWNDASSOCIATESET  (WM_USER+1)
#define MSM_HWNDASSOCIATEGET  (WM_USER+2)
#define MSM_DWRANGESET        (WM_USER+3)
#define MSM_DWRANGEGET        (WM_USER+4)
#define MSM_WCURRENTPOSSET    (WM_USER+5)
#define MSM_WCURRENTPOSGET    (WM_USER+6)
#define MSM_FNOPEGSCROLLSET   (WM_USER+7)
#define MSM_FNOPEGSCROLLGET   (WM_USER+8)
#define MSM_FINVERTRANGESET   (WM_USER+9)
#define MSM_FINVERTRANGEGET   (WM_USER+10)
#define MSM_CRCOLORSET        (WM_USER+11)
#define MSM_CRCOLORGET        (WM_USER+12)


//Message API Functions
HWND     FAR PASCAL MSHAssociateSet(HWND hWnd, HWND hWndAssociate);
HWND     FAR PASCAL MSHAssociateGet(HWND hWnd);
DWORD    FAR PASCAL MSDwRangeSet(HWND hWnd, WORD iMin, WORD iMax);
DWORD    FAR PASCAL MSDwRangeGet(HWND hWnd);
WORD     FAR PASCAL MSWCurrentPosSet(HWND hWnd, WORD iPos);
WORD     FAR PASCAL MSWCurrentPosGet(HWND hWnd);
BOOL     FAR PASCAL MSFNoPegScrollSet(HWND hWnd, BOOL fNoPegScroll);
BOOL     FAR PASCAL MSFNoPegScrollGet(HWND hWnd);
BOOL     FAR PASCAL MSFInvertRangeSet(HWND hWnd, BOOL fInvertRange);
BOOL     FAR PASCAL MSFInvertRangeGet(HWND hWnd);
COLORREF FAR PASCAL MSCrColorSet(HWND hWnd, WORD iColor, COLORREF cr);
COLORREF FAR PASCAL MSCrColorGet(HWND hWnd, WORD iColor);



//Notification codes sent via WM_COMMAND from the control.
#define MSN_ASSOCIATEGAIN   1
#define MSN_ASSOCIATELOSS   2
#define MSN_RANGECHANGE     3


//Color indices for MSM_COLORSET/GET and MSCrColorSet/Get
#define MSCOLOR_FACE        0
#define MSCOLOR_ARROW       1
#define MSCOLOR_SHADOW      2
#define MSCOLOR_HIGHLIGHT   3
#define MSCOLOR_FRAME       4

#define CCOLORS             5

//Control specific styles.
#define MSS_VERTICAL        0x0001L
#define MSS_HORIZONTAL      0x0002L
#define MSS_NOPEGSCROLL     0x0004L
#define MSS_TEXTHASRANGE    0x0008L
#define MSS_INVERTRANGE     0x0010L

#endif // _MUSCROLL_H_


/************************************************************************
 * End of File: CEK.h
 ***********************************************************************/

