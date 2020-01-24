/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdb.h                                                               **/


#ifndef _WDB_H_
#define _WDB_H_


/* misc. constants */
    #define WDB_LIBVERSION      0x0102  /* V1.02                        */

    #define WDBC_MAX_FILELEN    144     /* MUST be even, path+file      */
    #define WDBC_MAX_PRELUDE    30      /* MUST be even, prelude string */
    #define WDBC_MAX_SECLEN     40      /* max section name len (EVEN!) */


    typedef HANDLE              HWDB;   /* handle for WDB sessions      */


/* WDB configuration structure typedef */
    typedef struct tWDBCONFIG 
    {
        UINT        wEnable;            /* master switch for session    */
        UINT        wAssert;            /* assert type: off, sysmodal.. */
        UINT        wLevel;             /* debugging level              */
        UINT        wOutput;            /* output device: aux, window.. */
        UINT        wLFtoCRLF;          /* convert LF to CR/LF          */
        UINT        wMaxLines;          /* maximum # of window lines    */
        UINT        wFontSize;          /* font point size for window   */
        UINT        wFontBold;          /* bold font flag for window    */
        int         nWindowX;           /* window x offset              */
        int         nWindowY;           /* window y offset              */
        UINT        wWindowW;           /* window width                 */
        UINT        wWindowH;           /* window height                */
        char        szPrelude[ WDBC_MAX_PRELUDE ];
        char        szOutFile[ WDBC_MAX_FILELEN ];
        char        szFontName[ LF_FACESIZE ];

    } WDBCONFIG, *NPWDBCONFIG, FAR *LPWDBCONFIG;


/* WDB session configuration structure--run-time only */
    typedef struct tWDBSESSION
    {
        WDBCONFIG   wdbConfig;          /* configuration for session    */
        HANDLE      hOutput;            /* output handle; wnd, file, etc*/
        HWND        hWnd;               /* parent window handle         */
        HFONT       hFont;              /* logical font handle in use   */
        UINT        wSemaphore;         /* to block recursive creates   */
        char        szPrivate[ WDBC_MAX_FILELEN ];
        char        szSection[ WDBC_MAX_SECLEN ];

    } WDBSESSION, *NPWDBSESSION, FAR *LPWDBSESSION;


/* index values for wdbGetInfo() (WDBI_*) */
    #define WDBI_WENABLE        0       /* session->config.wEnable      */
    #define WDBI_WASSERT        1       /* session->config.wAssert      */
    #define WDBI_WLEVEL         2       /* session->config.wLevel       */
    #define WDBI_WOUTPUT        3       /* session->config.wOutput      */
    #define WDBI_WLFTOCRLF      4       /* session->config.wLFtoCRLF    */
    #define WDBI_WMAXLINES      5       /* session->config.wMaxLines    */
    #define WDBI_WFONTSIZE      6       /* session->config.wFontSize    */
    #define WDBI_WFONTBOLD      7       /* session->config.wFontBold    */
    #define WDBI_NWINDOWX       8       /* session->config.nWindowX     */
    #define WDBI_NWINDOWY       9       /* session->config.nWindowY     */
    #define WDBI_WWINDOWW       10      /* session->config.wWindowW     */
    #define WDBI_WWINDOWH       11      /* session->config.wWindowH     */

    #define WDBI_ENDCONFIG  (sizeof(WDBCONFIG)/sizeof(UINT))
    #define WDBI_HOUTPUT    (WDBI_ENDCONFIG+0)  /* session->hOutput     */
    #define WDBI_HWND       (WDBI_ENDCONFIG+1)  /* session->hWnd        */
    #define WDBI_HFONT      (WDBI_ENDCONFIG+2)  /* session->hFont       */
    #define WDBI_WSEMAPHORE (WDBI_ENDCONFIG+3)  /* session->wSemaphore  */


/* config.wEnable values */
    #define WDB_ENABLE_NO           0   /* WDB disabled for session     */
    #define WDB_ENABLE_YES          1   /* WDB enabled for session      */

/* config.wAssert values */
    #define WDB_ASSERT_OFF          0   /* do nothing with asserts      */
    #define WDB_ASSERT_WINDOW       1   /* just a modaless window       */
    #define WDB_ASSERT_SYSMODAL     2   /* nasty! but useful sometimes  */
    #define WDB_ASSERT_BEEP         3   /* you guessed it--no window    */
    #define WDB_ASSERT_MAX          3   /* maximum assert type value    */

/* config.wLevel values */
    #define WDB_LEVEL_MAX           5   /* maximum level of debug stuff */

/* config.wOutput values */
    #define WDB_OUTPUT_OFF          0   /* output disabled              */
    #define WDB_OUTPUT_WINDOW       1   /* to a window--of course       */
    #define WDB_OUTPUT_AUX          2   /* output to stdaux             */
    #define WDB_OUTPUT_FILE         3   /* log to a file (or append)    */
    #define WDB_OUTPUT_ODS          4
    #define WDB_OUTPUT_MONO         5
    #define WDB_OUTPUT_MAX          5   /* maximum output type value    */

/*
 * wdbGetOutput() will return the following value if WDBLIB.LIB is used
 * and WDB.DLL was not found on the system
 */
    #define WDB_OUTPUT_WDBLIB       0xFFFF


/* config.wEnable values */
    #define WDB_LFTOCRLF_NO         0   /* WDB does not convert LF's    */
    #define WDB_LFTOCRLF_YES        1   /* WDB converts LF's to CR/LF   */


/* WDB window flags */
    #define WDB_CHARINPUT   0x00000001L /* not used yet                 */

/* control messages sent to WDB window */
    #define WDBM_SETNLINES          (WM_USER + 1)
    #define WDBM_GETNLINES          (WM_USER + 2)
    #define WDBM_SETTABSTOPS        (WM_USER + 4)
    #define WDBM_GETTABSTOPS        (WM_USER + 5)
    #define WDBM_GETNUMTABS         (WM_USER + 6)
    #define WDBM_SETOUTPUT          (WM_USER + 7)
    #define WDBM_GETOUTPUT          (WM_USER + 8)
    #define WDBM_CLEARWINDOW        (WM_USER + 9)


/** this is where we get nasty--these macros are *ugly* but necessary **/

#ifdef DEBUG

    HWDB    hWDB;
    UINT    __wEval;

    /* cheap and dirty debug macros */
    #define ODS(x)      OutputDebugString(x)
    #define D(x)        {x;}


    /** this is for a WDB 'Debug' menu **/
    #ifndef IDM_WDB_BASE
        #define IDM_WDB_BASE    900
    #endif
    #define IDM_WDB_CONFIG      (IDM_WDB_BASE + 0)
    #define IDM_WDB_DUMP        (IDM_WDB_BASE + 1)


    #define DASSERT(h,x)        ((x)?0:wdbAssertBox(h,NULL,__FILE__,__LINE__))
    #define DASSERTMSG(h,x,m)   ((x)?0:wdbAssertBox(h,m,__FILE__,__LINE__))
    #define DASSERTEVAL(h,x)    (__wEval=(x),DASSERT(h,__wEval),__wEval)

    #define DLEVEL(h)           wdbGetInfo( h, WDBI_WLEVEL )


    #define wdbPrintf1  wdbPrintf
    #define DPF         wdbPrintf
    #define DPF1        wdbPrintf
    #define DPF2        wdbPrintf2
    #define DPF3        wdbPrintf3
    #define DPF4        wdbPrintf4
    #define DPF5        wdbPrintf5

    #define wdbVPrintf1 wdbVPrintf
    #define DVPF        wdbVPrintf
    #define DVPF1       wdbVPrintf
    #define DVPF2       wdbVPrintf2
    #define DVPF3       wdbVPrintf3
    #define DVPF4       wdbVPrintf4
    #define DVPF5       wdbVPrintf5

    #define wdbPuts1    wdbPuts
    #define DPS         wdbPuts
    #define DPS1        wdbPuts
    #define DPS2        wdbPuts2
    #define DPS3        wdbPuts3
    #define DPS4        wdbPuts4
    #define DPS5        wdbPuts5
                    
    #define wdbPrint1   wdbPrint
    #define DP          wdbPrint
    #define DP1         wdbPrint
    #define DP2         wdbPrint2
    #define DP3         wdbPrint3
    #define DP4         wdbPrint4
    #define DP5         wdbPrint5

    #define wdbVPrint1  wdbVPrint
    #define DVP         wdbVPrint
    #define DVP1        wdbVPrint
    #define DVP2        wdbVPrint2
    #define DVP3        wdbVPrint3
    #define DVP4        wdbVPrint4
    #define DVP5        wdbVPrint5

#else

    #define ODS(a)
    #define D(x)

    #define DASSERT(h,x)        0
    #define DASSERTMSG(h,x,m)   0
    #define DASSERTEVAL(h,x)    (x)

    #define DLEVEL(h)           0

    #define DPF     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DPF1    if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DPF2    if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DPF3    if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DPF4    if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DPF5    if (0) ((int (_cdecl *)(int, char *, ...)) 0)

    #define DVPF(x,y,z) 
    #define DVPF1(x,y,z)
    #define DVPF2(x,y,z)
    #define DVPF3(x,y,z)
    #define DVPF4(x,y,z)
    #define DVPF5(x,y,z)

    #define DPS(x,y)    
    #define DPS1(x,y)   
    #define DPS2(x,y)   
    #define DPS3(x,y)   
    #define DPS4(x,y)   
    #define DPS5(x,y)   

    #define DP      if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP1     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP2     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP3     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP4     if (0) ((int (_cdecl *)(int, char *, ...)) 0)
    #define DP5     if (0) ((int (_cdecl *)(int, char *, ...)) 0)

#endif


#if defined( DEBUG ) || defined( WDB_DEBUG ) || defined( WDB_RELEASE )

/* public function prototypes */
    UINT FAR PASCAL wdbLibVersion( UINT wVersion );

    BOOL FAR PASCAL wdbReadConfigInfo( LPSTR, LPSTR, LPSTR, LPWDBCONFIG );
    BOOL FAR PASCAL wdbWriteConfigInfo( LPSTR, LPSTR, LPWDBCONFIG );

    UINT FAR PASCAL wdbGetInfo( HWDB hWDB, UINT wIndex );
    UINT FAR PASCAL wdbSetInfo( HWDB hWDB, UINT wIndex, UINT wValue );
    BOOL FAR PASCAL wdbGetOutputFile( HWDB hWDB, LPSTR lpDest );
    BOOL FAR PASCAL wdbGetOutputPrelude( HWDB hWDB, LPSTR lpDest );
    BOOL FAR PASCAL wdbGetFontName( HWDB hWDB, LPSTR lpDest );

    BOOL FAR PASCAL wdbConfigDlg( HWDB hWDB );
    BOOL FAR PASCAL wdbDumpDebugInfo( HWDB hWDB );

    HWDB FAR PASCAL wdbOpenSession( HWND, LPSTR, LPSTR, UINT );
    HWDB FAR PASCAL wdbCloseSession( HWDB hWDB );

    UINT FAR PASCAL wdbGetOutput( HWDB hWDB, LPSTR lpszFile );
    BOOL FAR PASCAL wdbSetOutput( HWDB hWDB, UINT wOutput, LPSTR lpszFile );

    BOOL FAR PASCAL wdbAssertBox( HWDB, LPSTR, LPSTR, UINT );

    int FAR PASCAL wdbPuts( HWDB hWDB, LPSTR lpsz );
    int FAR PASCAL wdbPuts2( HWDB hWDB, LPSTR lpsz );
    int FAR PASCAL wdbPuts3( HWDB hWDB, LPSTR lpsz );
    int FAR PASCAL wdbPuts4( HWDB hWDB, LPSTR lpsz );
    int FAR PASCAL wdbPuts5( HWDB hWDB, LPSTR lpsz );

    int FAR _cdecl wdbPrintf( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrintf2( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrintf3( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrintf4( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrintf5( HWDB hWDB, LPSTR lpszFormat, ... );

    int FAR _cdecl wdbVPrintf( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrintf2( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrintf3( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrintf4( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrintf5( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );

    int FAR _cdecl wdbPrint( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrint2( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrint3( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrint4( HWDB hWDB, LPSTR lpszFormat, ... );
    int FAR _cdecl wdbPrint5( HWDB hWDB, LPSTR lpszFormat, ... );

    int FAR _cdecl wdbVPrint( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrint2( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrint3( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrint4( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );
    int FAR _cdecl wdbVPrint5( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs );

    BOOL WINAPI wdbMonoQuery(VOID);
    BOOL WINAPI wdbMonoOpen(BOOL);
    BOOL WINAPI wdbMonoClose(VOID);
    BOOL WINAPI wdbMonoOut(LPCSTR lpsz);


#else

    /* this is so the compiler doesn't complain */
    #define hWDB    NULL

#endif


#endif

/** EOF: wdb.h **/
