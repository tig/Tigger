/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbmain.c                                                           **/


/* the includes required to build this file */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


/* global variables */
    HANDLE      ghModule;               /* module handle of this .DLL   */
    BOOL        gfInitWindow = FALSE;   /* window class registered?     */


/* temporary buffers--don't assume these are preserved across func calls */
    char        gachTemp[ WDBC_MAX_BUFLEN ];
    char        gachBigTemp[ WDBC_MAX_BIGBUFLEN ];


/* character array for assert display */
    char        gszAssertInfo[ WDBC_MAX_ASSERTLINE ];


/* default session (config) structure */
    WDBSESSION  gWDBDefSession = 
                {
                    /** gWDBDefSession.wdbConfig **/
                    {
                        WDB_ENABLE_YES,         /* wdbConfig.wEnable    */
                        WDB_ASSERT_WINDOW,      /* wdbConfig.wAssert    */
                        WDB_LEVEL_MAX - 1,      /* wdbConfig.wLevel     */
                        WDB_OUTPUT_WINDOW,      /* wdbConfig.wOutput    */
                        WDB_LFTOCRLF_YES,       /* wdbConfig.wLFtoCRLF  */
                        WDBC_MAX_MAXLINES / 2,  /* wdbConfig.wMaxLines  */
                    },

                    NULL,                   /* wdbSession.hInst         */
                    NULL,                   /* wdbSession.hOutput       */
                    NULL,                   /* wdbSession.hWnd          */
                    0,                      /* wdbSession.wSemaphore    */
                };


/** BOOL FAR PASCAL WEP( int nArgument )
 *
 *  DESCRIPTION:
 *      Performs cleanup tasks when the .DLL is unloaded.  The WEP() is
 *      called automatically by Windows when the .DLL is unloaded (no
 *      remaining tasks still have the .DLL loaded).  It is strongly
 *      recommended that a .DLL have a WEP() function, even if it does
 *      nothing but return (1), as in this example.
 *
 *  WEP CAVEATS:
 *      All of this applies to Windows 3.0.  The WEP() was not very
 *      well defined, so there are a lot of different variations of
 *      what a WEP() should look like.  What follows is a list of things
 *      you should be aware of:
 *
 *      o   Make sure that the WEP() is @1 RESIDENTNAME in the EXPORTS
 *          section of the .DEF file.  This ensures that the WEP() can
 *          be called as quickly as possible.  Incidently, this is why
 *          the WEP() is called the WEP() instead of WindowsExitProcedure().
 *          It takes up the minimum amount of space and is quickly located.
 *
 *      o   Because of a problem in Windows 3.0, you should place your
 *          WEP() and all associated data in a FIXED segment.  This is
 *          only a problem under extremely low memory conditions.
 *
 *      o   WEP()'s are *not* called in the '.DLL load order.'
 *
 *      o   WEP()'s are called *after* all dependent .DLL's have
 *          been *freed* so if your WEP() wants to call another .DLL,
 *          it is SOL (VERY MAJOR!).
 *
 *      o   WEP()'s are called on KERNEL's internal stack, with *VERY*
 *          little stack space (should they be called on the current
 *          APP's stack?).
 *
 *      o   If a WEP tries to re-enter KERNEL (ie FreeLibrary()) things
 *          will go south quickly.
 *
 *      o   The return value of most WEP()'s is random because it was
 *          never well defined.  To be safe, always return TRUE (1).
 *
 *      o   If a .DLL is implicitly loaded, the WEP() may not get called
 *          (in Windows 3.0).
 *
 *      o   There have been reports that nArgument is sometimes bogus.
 *          I have never had a problem with it though...  This is 
 *          possibly related to the implicit load problem?
 *
 ** cjp */

/* put WEP into a segment that can be FIXED independently */
    BOOL FAR PASCAL WEP( int nArgument );
   #ifndef WIN32
    #pragma alloc_text( WDBWEP, WEP )
   #endif

BOOL FAR PASCAL WEP( int nArgument )
{
    /*  NOTE:  I do NOT put all data and routines associated with this
     *  WEP() in a FIXED segment.  In other words, this code may not
     *  work under low memory conditions.  ...such is life.  -cjp
     */
#ifndef WIN32
    switch ( nArgument )
    {
        case WEP_SYSTEM_EXIT:
            OD( "WDB: WEP_SYSTEM_EXIT\r\n" );
        break;

        case WEP_FREE_DLL:
            OD( "WDB: WEP_FREE_DLL\r\n" );
        break;

        default:
            OD( "WDB: WEP_UNDEFINED!!!\r\n" );
        break;
    }
#else
   UNREFERENCED_PARAMETER(nArgument) ;
#endif

    /* close any crud still open */
    wdbCloseOutput( &gWDBDefSession );

    /* free up the strings */
    wdbFreeStrings();

    /* always return TRUE (1) */
    return ( TRUE );
} /* WEP() */


/** BOOL FAR PASCAL LibMain( HANDLE, WORD, WORD, LPSTR )
 *
 *  DESCRIPTION:
 *      Is called by LibInit().  LibInit() is called by Windows when
 *      the .DLL is loaded.  LibInit() initializes the DLL's heap, if
 *      a HEAPSIZE value is specified in the .DLL's DEF file.  Then
 *      LibInit() calls LibMain().  The LibMain() function below
 *      satisfies that call.
 *
 *      The LibMain() function should perform additional initialization
 *      tasks required by the .DLL.  In this example, no initialization
 *      tasks are required.  LibMain() should return a value of TRUE if
 *      the initialization is successful.
 *
 ** cjp */

#ifndef WIN32
BOOL FAR PASCAL LibMain( HANDLE hModule,
                         WORD   wDataSeg,
                         WORD   cbHeapSize,
                         LPSTR  lpszCmdLine )
{
#else
BOOL FAR PASCAL DllEntryPoint( HANDLE hModule,
                               DWORD  dwReason,
                               LPVOID lpReserved )
{
   char szBuf[256] ;

   wsprintf( szBuf, "hModule = %08X\r\n", hModule ) ;
   OD( szBuf ) ;

   OD( "WDB: In LibMain() \r\n" );

   switch( dwReason )
   {
      case DLL_PROCESS_ATTACH:
         OD( "DLL_PROCESS_ATTACH\r\n" ) ;
      break ;

      case DLL_THREAD_ATTACH:
         OD( "DLL_THREAD_ATTACH\r\n" ) ;
         return 0 ;
      break ;

      case DLL_PROCESS_DETACH:
         OD( "DLL_PROCESS_DETATCH\r\n" ) ;
         
         /*
          * Call what in Win16 was the WEP to clean up.
          */
         WEP( 0 ) ;

         return 0 ;
      break ;

      case DLL_THREAD_DETACH:
         OD( "DLL_THREAD_DETATCH\r\n" ) ;
         return 0 ;
      break ;

      break ;

      default:
         OD( "DLL... default!\r\n" ) ;
         return 0 ;
      break ;
   }

//   UNREFERENCED_PARAMETER(dwReason) ;
   UNREFERENCED_PARAMETER(lpReserved) ;

#endif                        
   

    /* make module handle global */
    ghModule = hModule;

    /* initialize the strings from the stringtable */
    if ( !wdbGetStrings( hModule ) )
    {
        OD( "WDB: wdbGetStrings() FAILED!\r\n" );
        return FALSE ;
    }

    OD( "WDB: AfterGetStings() \r\n" );

    /* try to justify some of the default info */
    if ( !wdbInitConfig() )
    {
        OD( "WDB: wdbInitConfig() FAILED!\r\n" );
        return ( FALSE );
    }
    OD( "WDB: AfterInitConfig() \r\n" );

    /* read the default configuration for WDB from WDB.INI... */
    if ( !wdbReadConfigInfo( gszWDB,
                             gWDBDefSession.szSection,
                             gWDBDefSession.szPrivate,
                             (LPWDBCONFIG)&gWDBDefSession ) )
    {
        OD( "WDB: wdbReadConfigInfo() FAILED!\r\n" );
        return ( FALSE );
    }
                            
    else
    {
        OD( "WDB: LibMain() succeeded!!!\r\n" );
    }

    /* return successful initialization */
    return ( TRUE );
} /* LibMain() */


/** EOF: wdbmain.c **/
