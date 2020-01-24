/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbi.h
 *
 *  DESCRIPTION:
 *      This header file contains *internal* information that WDB
 *      requires to build.  Applications using WDB do not need this
 *      info.
 *
 ** cjp */

#ifndef _WDBI_H_
#define _WDBI_H_

/* make sure we get these header files */
#ifndef RC_INVOKED
    #include <string.h>
    #include <ctype.h>
    #include <stdio.h>
    #include <stdlib.h>
#endif


/* for debugging the debugger! */
    #ifdef WDB_DEBUG
        #define OD(x)       OutputDebugString(x)
    #else
        #define OD(x)
    #endif


/* bilingual... so the resource compiler doesn't puke */
    #ifdef RC_INVOKED
        #define ID(id)      id
    #else
        #define ID(id)      MAKEINTRESOURCE(id)
    #endif


    /*  WDBC_MAX_BIGBUFLEN is the value that decides how long a single
     *  string can be when wdbPrint()'ing.  I know this is cheezy, but
     *  if you require longer strings, increase this from 2kb to the
     *  maximum length you require...  If I had time, this would become
     *  another configuration flag.
     */
    #define WDBC_MAX_BIGBUFLEN  2049    /* safe(?) BIG scratch buffer   */


/* misc. WDB Constants (WDBC_*) */
    #define WDBC_MAX_BUFLEN     256     /* safe(?) scratch buffer size  */
    #define WDBC_MAX_MAXLINES   150     /* maximum window output lines  */
    #define WDBC_MAX_INQUEUE    128     /* max number of input chars 2 Q*/

    #define WDBC_MAX_FONTSIZE   48      /* max font point size allowed  */
    #define WDBC_DEF_FONTSIZE   10      /* default font point size      */
    #define WDBC_DEF_FONTNAME   "System"    /* default font face name   */

    #define WDBC_MAX_ASSERTLINE 120     /* max len of assert info       */

    #define WDBC_GMEM_FLAGS     (GMEM_MOVEABLE | GMEM_ZEROINIT)
    #define WDBC_LMEM_FLAGS     (LMEM_MOVEABLE | LMEM_ZEROINIT)
    #define WDBC_OF_FLAGS       (OF_READWRITE | OF_SHARE_COMPAT)
    #define WDBC_CREAT_FLAGS    (0)

    #define FH_AUX              (3)     /* stdaux file handle           */



/* queue structure for input chars */
    typedef struct tINPQUEUE
    {
        int     iLen;
        char    ach[ WDBC_MAX_INQUEUE ];

    } INQUEUE, *NPINQUEUE, FAR *LPINQUEUE;

    typedef HANDLE  HINQUEUE;


/* WDB window instance data structure */
    typedef struct tWDBLINE
    {
        int     iLen;
        HANDLE  hText;

    } WDBLINE, *NPWDBLINE, FAR *LPWDBLINE;

    typedef struct tWDBTEXT
    {
        HWDB        hWDB;               /* session handle for window    */
        HWND        hWnd;               /* window displaying the text   */
        UINT        wID;                /* ID code, for WM_COMMAND msgs */
        BOOL        fScrollSemaphore;   /* scrolling semaphore          */
        int         iFirst;             /* first line in queue          */
        int         iCount;             /* number of lines in queue     */
        int         iTop;               /* line at top of window        */
        int         iLeft;              /* X offset of the window       */
        int         iMaxLen;            /* len of longest str stored    */
        int         iMaxLines;          /* max number of WDBLINE's      */
        int         iRangeH;            /* horizontal range             */
        int         iRangeV;            /* verticle range               */
        BOOL        fRedraw;            /* redraw flag                  */
        HFONT       hFont;              /* font to draw with            */
        int         dxFont, dyFont;     /* font size                    */
        HINQUEUE    hInQueue;           /* input queue of chars-not used*/
        int         nTabs;              /* yep.  not used yet           */
        int        *pTabs;              /* not used yet                 */
        WDBLINE     rLines[];           /* range of iMaxLines WDBLINE's */

    } WDBTEXT, *NPWDBTEXT, FAR *LPWDBTEXT;

    typedef HANDLE  HWDBTEXT;


/* window words/longs */
    #define WDB_WW_HTEXT        0

   #ifndef WIN32
    #define GETHTEXT(hwnd)      (GetWindowWord(hwnd,WDB_WW_HTEXT))
    #define SETHTEXT(hwnd,x)    (SetWindowWord(hwnd,WDB_WW_HTEXT,(WORD)x))
   #else
    #define GETHTEXT(hwnd)      (GetWindowLong(hwnd,WDB_WW_HTEXT))
    #define SETHTEXT(hwnd,x)    (SetWindowLong(hwnd,WDB_WW_HTEXT,(LONG)x))
   #endif

/* wdbDebugConfigDlg() defines */
    #define DLG_WDB_CONFIG      ID(10)
    #define IDD_WDB_ENABLE      101
    #define IDD_WDB_ASSERT      103
    #define IDD_WDB_OUTPUT      104
    #define IDD_WDB_LFTOCRLF    105
    #define IDD_WDB_FILENAME    106
    #define IDD_WDB_FONTNAME    107
    #define IDD_WDB_FONTSIZE    109
    #define IDD_WDB_FONTBOLD    110
    #define IDD_WDB_PRELUDE     111
    #define IDD_WDB_MAXLINES    112
    #define IDD_WDB_LEVEL       113


/* flags for wdbReallyCreateFont() */
    #define RCF_NORMAL          0x0000
    #define RCF_ITALIC          0x0001
    #define RCF_UNDERLINE       0x0002
    #define RCF_STRIKEOUT       0x0004
    #define RCF_BOLD            0x0008


    /*  The following ridiculous bunch of defines are to accomodate a string
     *  table after I coded it with static string buffers (remember that this
     *  thing still qualifies as a hack!).  If you feel like fixing this
     *  another way, feel free!  Once you do, give me the code!  As it stands
     *  this code works pretty well.
     */

/* string table id defines */
    #define IDS_FIRST               1024
    #define IDS_gszWDB              IDS_FIRST
    #define IDS_gszDefzYz           (IDS_FIRST + 1)
    #define IDS_gszDefFile          (IDS_FIRST + 2)
    #define IDS_gszDefFontName      (IDS_FIRST + 3)
    #define IDS_gszINI              (IDS_FIRST + 4)
    #define IDS_gszSectionPrefix    (IDS_FIRST + 5)
    #define IDS_gszAssertTitle      (IDS_FIRST + 6)
    #define IDS_gszWindowTitle      (IDS_FIRST + 7)
    #define IDS_gszWindowClass      (IDS_FIRST + 8)

    #define IDS_gszKeyEnable        (IDS_FIRST + 9)
    #define IDS_gszKeyLevel         (IDS_FIRST + 10)
    #define IDS_gszKeyOutput        (IDS_FIRST + 11)
    #define IDS_gszKeyFile          (IDS_FIRST + 12)
    #define IDS_gszKeyWindow        (IDS_FIRST + 13)
    #define IDS_gszKeyAssert        (IDS_FIRST + 14)
    #define IDS_gszKeyPrelude       (IDS_FIRST + 15)
    #define IDS_gszKeyLFtoCRLF      (IDS_FIRST + 16)
    #define IDS_gszKeyMaxLines      (IDS_FIRST + 17)
    #define IDS_gszKeyFontName      (IDS_FIRST + 18)
    #define IDS_gszKeyFontSize      (IDS_FIRST + 19)
    #define IDS_gszKeyFontBold      (IDS_FIRST + 20)
    #define IDS_gszKeyWindowX       (IDS_FIRST + 21)
    #define IDS_gszKeyWindowY       (IDS_FIRST + 22)
    #define IDS_gszKeyWindowW       (IDS_FIRST + 23)
    #define IDS_gszKeyWindowH       (IDS_FIRST + 24)

    #define IDS_gszArgQuery         (IDS_FIRST + 25)
    #define IDS_gszArgWindow        (IDS_FIRST + 26)
    #define IDS_gszArgNo            (IDS_FIRST + 27)
    #define IDS_gszArgYes           (IDS_FIRST + 28)
    #define IDS_gszArgAux           (IDS_FIRST + 29)
    #define IDS_gszArgSysModal      (IDS_FIRST + 30)
    #define IDS_gszArgOff           (IDS_FIRST + 31)
    #define IDS_gszArgBeep          (IDS_FIRST + 32)
    #define IDS_gszArgSystem        (IDS_FIRST + 33)
    #define IDS_gszArgFile          (IDS_FIRST + 34)
    #define IDS_gszArgOds           (IDS_FIRST + 35)
    #define IDS_gszArgMono          (IDS_FIRST + 36)
    #define IDS_LAST                (IDS_FIRST + 36)


/** macros to fix a stupid decision I made early on **/
    #define gszWDB              (rgszStrings[IDS_gszWDB - IDS_FIRST])
    #define gszDefzYz           (rgszStrings[IDS_gszDefzYz - IDS_FIRST])
    #define gszDefFile          (rgszStrings[IDS_gszDefFile - IDS_FIRST])
    #define gszDefFontName      (rgszStrings[IDS_gszDefFontName - IDS_FIRST])
    #define gszINI              (rgszStrings[IDS_gszINI - IDS_FIRST])
    #define gszSectionPrefix    (rgszStrings[IDS_gszSectionPrefix - IDS_FIRST])
    #define gszAssertTitle      (rgszStrings[IDS_gszAssertTitle - IDS_FIRST])
    #define gszWindowTitle      (rgszStrings[IDS_gszWindowTitle - IDS_FIRST])
    #define gszWindowClass      (rgszStrings[IDS_gszWindowClass - IDS_FIRST])

    #define gszKeyEnable        (rgszStrings[IDS_gszKeyEnable - IDS_FIRST])
    #define gszKeyLevel         (rgszStrings[IDS_gszKeyLevel - IDS_FIRST])
    #define gszKeyOutput        (rgszStrings[IDS_gszKeyOutput - IDS_FIRST])
    #define gszKeyFile          (rgszStrings[IDS_gszKeyFile - IDS_FIRST])
    #define gszKeyWindow        (rgszStrings[IDS_gszKeyWindow - IDS_FIRST])
    #define gszKeyAssert        (rgszStrings[IDS_gszKeyAssert - IDS_FIRST])
    #define gszKeyPrelude       (rgszStrings[IDS_gszKeyPrelude - IDS_FIRST])
    #define gszKeyLFtoCRLF      (rgszStrings[IDS_gszKeyLFtoCRLF - IDS_FIRST])
    #define gszKeyMaxLines      (rgszStrings[IDS_gszKeyMaxLines - IDS_FIRST])
    #define gszKeyFontName      (rgszStrings[IDS_gszKeyFontName - IDS_FIRST])
    #define gszKeyFontSize      (rgszStrings[IDS_gszKeyFontSize - IDS_FIRST])
    #define gszKeyFontBold      (rgszStrings[IDS_gszKeyFontBold - IDS_FIRST])
    #define gszKeyWindowX       (rgszStrings[IDS_gszKeyWindowX - IDS_FIRST])
    #define gszKeyWindowY       (rgszStrings[IDS_gszKeyWindowY - IDS_FIRST])
    #define gszKeyWindowW       (rgszStrings[IDS_gszKeyWindowW - IDS_FIRST])
    #define gszKeyWindowH       (rgszStrings[IDS_gszKeyWindowH - IDS_FIRST])

    #define gszArgQuery         (rgszStrings[IDS_gszArgQuery - IDS_FIRST])
    #define gszArgWindow        (rgszStrings[IDS_gszArgWindow - IDS_FIRST])
    #define gszArgYes           (rgszStrings[IDS_gszArgYes - IDS_FIRST])
    #define gszArgNo            (rgszStrings[IDS_gszArgNo - IDS_FIRST])
    #define gszArgAux           (rgszStrings[IDS_gszArgAux - IDS_FIRST])
    #define gszArgSysModal      (rgszStrings[IDS_gszArgSysModal - IDS_FIRST])
    #define gszArgOff           (rgszStrings[IDS_gszArgOff - IDS_FIRST])
    #define gszArgBeep          (rgszStrings[IDS_gszArgBeep - IDS_FIRST])
    #define gszArgSystem        (rgszStrings[IDS_gszArgSystem - IDS_FIRST])
    #define gszArgFile          (rgszStrings[IDS_gszArgFile - IDS_FIRST])
    #define gszArgOds           (rgszStrings[IDS_gszArgOds  - IDS_FIRST])
    #define gszArgMono          (rgszStrings[IDS_gszArgMono - IDS_FIRST])


/** extern a bunch of globals **/
    extern HANDLE       ghModule;
    extern BOOL         gfInitWindow;
    extern char         gachTemp[ WDBC_MAX_BUFLEN ];
    extern char         gachBigTemp[ WDBC_MAX_BIGBUFLEN ];
    extern char         gszAssertInfo[ WDBC_MAX_ASSERTLINE ];
    extern WDBSESSION   gWDBDefSession;

    extern PSTR         rgszStrings[];


/* who said you couldn't! */
    UINT FAR PASCAL DOS3Call( void );


/** wdbfont.c **/
    HFONT FAR PASCAL wdbReallyCreateFont( HDC, LPSTR, int, UINT );


/** wdbdlg.c **/
    BOOL FAR PASCAL wdbConfigDlgProc( HWND, unsigned, UINT, LONG );


/** wdbwnd.c **/
    BOOL FAR PASCAL wdbInitWindow( HANDLE hInstance );
    HWND FAR PASCAL wdbOpenWindow( LPWDBSESSION lpSession );
    HWND FAR PASCAL wdbCloseWindow( LPWDBSESSION lpSession );
    BOOL FAR PASCAL wdbWriteWindow( LPWDBSESSION lpSession, LPSTR lpsz );
    void wdbChangeFont( LPWDBSESSION lpSession, HWND hWnd );


/** wdbsup.c **/
    void FAR PASCAL wdbHourGlass( BOOL fHourGlass );
    BOOL FAR PASCAL wdbOpenOutput( LPWDBSESSION lpSession );
    BOOL FAR PASCAL wdbCloseOutput( LPWDBSESSION lpSession );
    LPWDBSESSION FAR PASCAL wdbValidateSession( HWDB hWDB, UINT wLevel );
    BOOL FAR PASCAL wdbWriteFile( LPWDBSESSION lpSession, LPSTR lpsz );
    int wdbWriteOutput( LPWDBSESSION lpSession, LPSTR lpsz );
    int FAR PASCAL wdbAssertLog( LPWDBSESSION lpSession, LPSTR lpszText );
    BOOL FAR PASCAL wdbWriteMono( LPWDBSESSION lpSession, LPSTR lpsz ) ;


/** wdbcfg.c **/
    BOOL FAR PASCAL wdbInitConfig( void );
    BOOL FAR PASCAL wdbWriteConfigWindowX( LPSTR, LPSTR, LPWDBCONFIG );
    BOOL FAR PASCAL wdbWriteConfigWindowY( LPSTR, LPSTR, LPWDBCONFIG );
    BOOL FAR PASCAL wdbWriteConfigWindowW( LPSTR, LPSTR, LPWDBCONFIG );
    BOOL FAR PASCAL wdbWriteConfigWindowH( LPSTR, LPSTR, LPWDBCONFIG );


/** wdbstr.c **/
    BOOL FAR PASCAL wdbGetStrings( HANDLE hInst );
    void FAR PASCAL wdbFreeStrings( void );

#endif

/** EOF: wdbi.h **/
