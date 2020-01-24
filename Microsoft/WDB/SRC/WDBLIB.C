/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdblib.c
 *
 *  DESCRIPTION: 
 *      This file eventually becomes wdblib.lib.  The purpose is to provide
 *      the ability for an app that calls WDB functions to run
 *      without wdb.dll on the system.  It essentially just provides
 *      stubs for the apis in WDB.
 *
 *      Note that if you link to WDBLIB.LIB instead of WDB.LIB you will
 *      always get the block of global variables defined below in your
 *      DGROUP.  The solution is to link with wdblib.lib only if DEBUG
 *      is defined (which can be a pain).
 *
 *      Also, when using WDBLIB.LIB, WDB.DLL is loaded via a call to
 *      LoadLibrary().  Calling the wdbCloseSession() call will force
 *      a FreeLibary() call which will unload WDB.DLL from memory.
 *      Thus, if your app crashes before calling wdbCloseSession(), or
 *      does not call wdbCloseSession() WDB.DLL will never get freed
 *      from memory.
 *
 *  HISTORY:
 *      3/31/92     ckindel     wrote it
 *
 ** cek */

/* grab the includes we need */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"

/*
 * Define all of our function pointers.  These are in the DGROUP of the
 * app or dll that links with WDBLIB.LIB!!!!
 *
 * We also specify our ordinal numbers here (ODS_)
 */
#define ORD_WDBLIBVERSION              2
static UINT (FAR PASCAL * lpfn_wdbLibVersion)      ( UINT ) ;

#define ORD_WDBREADCONFIGINFO          4
static BOOL (FAR PASCAL * lpfn_wdbReadConfigInfo)  ( LPSTR, LPSTR, LPSTR, LPWDBCONFIG ) ;

#define ORD_WDBWRITECONFIGINFO         5
static BOOL (FAR PASCAL * lpfn_wdbWriteConfigInfo) ( LPSTR, LPSTR, LPWDBCONFIG ) ;

#define ORD_WDBGETINFO                 6
static UINT (FAR PASCAL * lpfn_wdbGetInfo)         ( HWDB, UINT ) ;

#define ORD_WDBSETINFO                 7
static UINT (FAR PASCAL * lpfn_wdbSetInfo)         ( HWDB, UINT, UINT ) ;

#define ORD_WDBGETOUTPUTFILE           8
static BOOL (FAR PASCAL * lpfn_wdbGetOutputFile)   ( HWDB, LPSTR ) ;

#define ORD_WDBGETOUTPUTPRELUDE        9
static BOOL (FAR PASCAL * lpfn_wdbGetOutputPrelude)( HWDB, LPSTR ) ;

#define ORD_WDBGETFONTNAME             10
static BOOL (FAR PASCAL * lpfn_wdbGetFontName)     ( HWDB, LPSTR ) ;

#define ORD_WDBCONFIGDLG               15
static BOOL (FAR PASCAL * lpfn_wdbConfigDlg)       ( HWDB ) ;

#define ORD_WDBDUMPDEBUGINFO           20
static BOOL (FAR PASCAL * lpfn_wdbDumpDebugInfo)   ( HWDB ) ;

#define ORD_WDBOPENSESSION             21
static HWDB (FAR PASCAL * lpfn_wdbOpenSession)     ( HWND, LPSTR, LPSTR, UINT ) ;

#define ORD_WDBCLOSESESSION            22
static HWDB (FAR PASCAL * lpfn_wdbCloseSession)    ( HWDB ) ;

#define ORD_WDBGETOUTPUT               23
static UINT (FAR PASCAL * lpfn_wdbGetOutput)       ( HWDB, LPSTR ) ;

#define ORD_WDBSETOUTPUT               24
static BOOL (FAR PASCAL * lpfn_wdbSetOutput)       ( HWDB, UINT , LPSTR ) ;

#define ORD_WDBASSERTBOX               25
static BOOL (FAR PASCAL * lpfn_wdbAssertBox)       ( HWDB, LPSTR, LPSTR, UINT ) ;

#define ORD_WDBPUTS                    50
static int  (FAR PASCAL * lpfn_wdbPuts)            ( HWDB, LPSTR ) ;

#define ORD_WDBPUTS2                   52
static int  (FAR PASCAL * lpfn_wdbPuts2)           ( HWDB, LPSTR ) ;

#define ORD_WDBPUTS3                   53
static int  (FAR PASCAL * lpfn_wdbPuts3)           ( HWDB, LPSTR ) ;

#define ORD_WDBPUTS4                   54
static int  (FAR PASCAL * lpfn_wdbPuts4)           ( HWDB, LPSTR ) ;

#define ORD_WDBPUTS5                   55
static int  (FAR PASCAL * lpfn_wdbPuts5)           ( HWDB, LPSTR ) ;

#define ORD_WDBPRINTF                  60
static int  (FAR _cdecl * lpfn_wdbPrintf)          ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINTF2                 62
static int  (FAR _cdecl * lpfn_wdbPrintf2)         ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINTF3                 63
static int  (FAR _cdecl * lpfn_wdbPrintf3)         ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINTF4                 64
static int  (FAR _cdecl * lpfn_wdbPrintf4)         ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINTF5                 65
static int  (FAR _cdecl * lpfn_wdbPrintf5)         ( HWDB, LPSTR, ... ) ;

#define ORD_WDBVPRINTF                 70
static int  (FAR _cdecl * lpfn_wdbVPrintf)         ( HWDB, LPSTR, LPSTR ) ;

#define ORD_WDBVPRINTF2                72
static int  (FAR _cdecl * lpfn_wdbVPrintf2)        ( HWDB, LPSTR, LPSTR ) ;

#define ORD_WDBVPRINTF3                73
static int  (FAR _cdecl * lpfn_wdbVPrintf3)        ( HWDB, LPSTR, LPSTR ) ;

#define ORD_WDBVPRINTF4                74
static int  (FAR _cdecl * lpfn_wdbVPrintf4)        ( HWDB, LPSTR, LPSTR ) ;

#define ORD_WDBVPRINTF5                75
static int  (FAR _cdecl * lpfn_wdbVPrintf5)        ( HWDB, LPSTR, LPSTR ) ;

#define ORD_WDBPRINT                   80
static int  (FAR _cdecl * lpfn_wdbPrint)           ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINT2                  82
static int  (FAR _cdecl * lpfn_wdbPrint2)          ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINT3                  83
static int  (FAR _cdecl * lpfn_wdbPrint3)          ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINT4                  84
static int  (FAR _cdecl * lpfn_wdbPrint4)          ( HWDB, LPSTR, ... ) ;

#define ORD_WDBPRINT5                  85
static int  (FAR _cdecl * lpfn_wdbPrint5)          ( HWDB, LPSTR, ... ) ;

#define ORD_WDBMONOQUERY               90
static BOOL (WINAPI * lpfn_wdbMonoQuery)           ( VOID ) ;

#define ORD_WDBMONOOPEN             91
static BOOL (WINAPI * lpfn_wdbMonoOpen)            ( VOID ) ;

#define ORD_WDBMONOCLOSE               92
static BOOL (WINAPI * lpfn_wdbMonoClose)           ( VOID ) ;

#define ORD_WDBMONOOUT              93
static BOOL (WINAPI * lpfn_wdbMonoOut)          ( LPCSTR ) ;


static HMODULE hmodWDB = NULL ;
static BOOL    fWDBIsNotAround = FALSE ;

/****************************************************************
 *  BOOL NEAR PASCAL wdbLoadWDBLIB( VOID )
 *
 *  Description: 
 *
 *    This function loads WDB.DLL and fixes up all of the stubs
 *    in this module with the exported functions in WDB.DLL.
 *    This function is called by the first wdb function
 *    that gets called.  A global HINSTANCE is set to indicate
 *    whether the funtions have been fixed up or not.
 *
 *    wdbCloseSession() will call FreeLibrary() if hmodWDB is
 *    non-zero.  Thus apps using WDBLIB.LIB !!!MUST!!! call
 *    wdbCloseSession() or WDB.DLL will not get freed!
 *
 ****************************************************************/
BOOL NEAR PASCAL wdbLoadWDBLIB( VOID )
{
   if (fWDBIsNotAround == TRUE)
      return FALSE ;

   if (!hmodWDB)
   {
      SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
      hmodWDB = LoadLibrary( "WDB.DLL" ) ;

      if (hmodWDB <=32)
      {
         fWDBIsNotAround = TRUE ;
         hmodWDB = NULL ;
         return FALSE ;
      }
   }

   #define MIS MAKEINTRESOURCE
   #define GPA GetProcAddress
   
   (FARPROC)lpfn_wdbLibVersion      = GPA( hmodWDB, MIS( ORD_WDBLIBVERSION ) ) ;      
   (FARPROC)lpfn_wdbReadConfigInfo  = GPA( hmodWDB, MIS( ORD_WDBREADCONFIGINFO ) ) ;  
   (FARPROC)lpfn_wdbWriteConfigInfo = GPA( hmodWDB, MIS( ORD_WDBWRITECONFIGINFO ) ) ; 
   (FARPROC)lpfn_wdbGetInfo         = GPA( hmodWDB, MIS( ORD_WDBGETINFO ) ) ;         
   (FARPROC)lpfn_wdbSetInfo         = GPA( hmodWDB, MIS( ORD_WDBSETINFO ) ) ;         
   (FARPROC)lpfn_wdbGetOutputFile   = GPA( hmodWDB, MIS( ORD_WDBGETOUTPUTFILE ) ) ;   
   (FARPROC)lpfn_wdbGetOutputPrelude= GPA( hmodWDB, MIS( ORD_WDBGETOUTPUTPRELUDE ) ) ;
   (FARPROC)lpfn_wdbGetFontName     = GPA( hmodWDB, MIS( ORD_WDBGETFONTNAME ) ) ;     
   (FARPROC)lpfn_wdbConfigDlg       = GPA( hmodWDB, MIS( ORD_WDBCONFIGDLG ) ) ;       
   (FARPROC)lpfn_wdbDumpDebugInfo   = GPA( hmodWDB, MIS( ORD_WDBDUMPDEBUGINFO ) ) ;   
   (FARPROC)lpfn_wdbOpenSession     = GPA( hmodWDB, MIS( ORD_WDBOPENSESSION ) ) ;     
   (FARPROC)lpfn_wdbCloseSession    = GPA( hmodWDB, MIS( ORD_WDBCLOSESESSION ) ) ;    
   (FARPROC)lpfn_wdbGetOutput       = GPA( hmodWDB, MIS( ORD_WDBGETOUTPUT ) ) ;       
   (FARPROC)lpfn_wdbSetOutput       = GPA( hmodWDB, MIS( ORD_WDBSETOUTPUT ) ) ;       
   (FARPROC)lpfn_wdbAssertBox       = GPA( hmodWDB, MIS( ORD_WDBASSERTBOX ) ) ;       
   (FARPROC)lpfn_wdbPuts            = GPA( hmodWDB, MIS( ORD_WDBPUTS ) ) ;            
   (FARPROC)lpfn_wdbPuts2           = GPA( hmodWDB, MIS( ORD_WDBPUTS2 ) ) ;           
   (FARPROC)lpfn_wdbPuts3           = GPA( hmodWDB, MIS( ORD_WDBPUTS3 ) ) ;           
   (FARPROC)lpfn_wdbPuts4           = GPA( hmodWDB, MIS( ORD_WDBPUTS4 ) ) ;           
   (FARPROC)lpfn_wdbPuts5           = GPA( hmodWDB, MIS( ORD_WDBPUTS5 ) ) ;           
   (FARPROC)lpfn_wdbPrintf          = GPA( hmodWDB, MIS( ORD_WDBPRINTF ) ) ;          
   (FARPROC)lpfn_wdbPrintf2         = GPA( hmodWDB, MIS( ORD_WDBPRINTF2 ) ) ;         
   (FARPROC)lpfn_wdbPrintf3         = GPA( hmodWDB, MIS( ORD_WDBPRINTF3 ) ) ;         
   (FARPROC)lpfn_wdbPrintf4         = GPA( hmodWDB, MIS( ORD_WDBPRINTF4 ) ) ;         
   (FARPROC)lpfn_wdbPrintf5         = GPA( hmodWDB, MIS( ORD_WDBPRINTF5 ) ) ;         
   (FARPROC)lpfn_wdbVPrintf         = GPA( hmodWDB, MIS( ORD_WDBVPRINTF ) ) ;         
   (FARPROC)lpfn_wdbVPrintf2        = GPA( hmodWDB, MIS( ORD_WDBVPRINTF2 ) ) ;        
   (FARPROC)lpfn_wdbVPrintf3        = GPA( hmodWDB, MIS( ORD_WDBVPRINTF3 ) ) ;        
   (FARPROC)lpfn_wdbVPrintf4        = GPA( hmodWDB, MIS( ORD_WDBVPRINTF4 ) ) ;        
   (FARPROC)lpfn_wdbVPrintf5        = GPA( hmodWDB, MIS( ORD_WDBVPRINTF5 ) ) ;        
   (FARPROC)lpfn_wdbPrint           = GPA( hmodWDB, MIS( ORD_WDBPRINT ) ) ;           
   (FARPROC)lpfn_wdbPrint2          = GPA( hmodWDB, MIS( ORD_WDBPRINT2 ) ) ;          
   (FARPROC)lpfn_wdbPrint3          = GPA( hmodWDB, MIS( ORD_WDBPRINT3 ) ) ;          
   (FARPROC)lpfn_wdbPrint4          = GPA( hmodWDB, MIS( ORD_WDBPRINT4 ) ) ;          
   (FARPROC)lpfn_wdbPrint5          = GPA( hmodWDB, MIS( ORD_WDBPRINT5 ) ) ;          
   (FARPROC)lpfn_wdbMonoQuery       = GPA( hmodWDB, MIS( ORD_WDBMONOQUERY ) );
   (FARPROC)lpfn_wdbMonoOpen        = GPA( hmodWDB, MIS( ORD_WDBMONOOPEN ) );
   (FARPROC)lpfn_wdbMonoClose       = GPA( hmodWDB, MIS( ORD_WDBMONOCLOSE ) ) ;
   (FARPROC)lpfn_wdbMonoOut         = GPA( hmodWDB, MIS( ORD_WDBMONOOUT ) ) ;

   return TRUE ;

} /* wdbLoadWDBLIB()  */

UINT FAR PASCAL wdbLibVersion( UINT wVersion )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
   {
      /*
       * Might as well return something useful, even though we can't find
       * the dll.
       */
      if ( wVersion )
         return ( (HIBYTE(wVersion) <= HIBYTE(WDB_LIBVERSION) ) ? TRUE : FALSE );

      return ( WDB_LIBVERSION );
   }

   return (*lpfn_wdbLibVersion)( wVersion ) ;
}

BOOL FAR PASCAL wdbReadConfigInfo( LPSTR p1, LPSTR p2, LPSTR p3, LPWDBCONFIG p4 )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbReadConfigInfo)( p1, p2, p3, p4 ) ;
}

BOOL FAR PASCAL wdbWriteConfigInfo( LPSTR p1, LPSTR p2, LPWDBCONFIG p3 )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbWriteConfigInfo)( p1, p2, p3 ) ;
}

UINT FAR PASCAL wdbGetInfo( HWDB hWDB, UINT wIndex )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (UINT)NULL ;

   return (*lpfn_wdbGetInfo)( hWDB, wIndex ) ;
}

UINT FAR PASCAL wdbSetInfo( HWDB hWDB, UINT wIndex, UINT wValue )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (UINT)NULL ;

   return (*lpfn_wdbSetInfo)( hWDB, wIndex, wValue ) ;
}

BOOL FAR PASCAL wdbGetOutputFile( HWDB hWDB, LPSTR lpDest )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbGetOutputFile)( hWDB, lpDest ) ;
}

BOOL FAR PASCAL wdbGetOutputPrelude( HWDB hWDB, LPSTR lpDest )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbGetOutputPrelude)( hWDB, lpDest ) ;
}

BOOL FAR PASCAL wdbGetFontName( HWDB hWDB, LPSTR lpDest )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbGetFontName)( hWDB, lpDest ) ;
}

BOOL FAR PASCAL wdbConfigDlg( HWDB hWDB )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbConfigDlg)( hWDB ) ;
}

BOOL FAR PASCAL wdbDumpDebugInfo( HWDB hWDB )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbDumpDebugInfo)( hWDB ) ;
}

HWDB FAR PASCAL wdbOpenSession( HWND hwnd, LPSTR p1, LPSTR p2, UINT p3)
{
   if (!hmodWDB && !wdbLoadWDBLIB())
   {
      /*
       * We return a non-zero value so that apps that check the
       * return value from wdbOpenSession() think that it worked.
       */
      return (HWDB)0xFFFF ;
   }

   return (*lpfn_wdbOpenSession)( hwnd, p1, p2, p3) ;
}

HWDB FAR PASCAL wdbCloseSession( HWDB hWDB )
{
   HWDB hwdb ;

   if (!hmodWDB && !wdbLoadWDBLIB())
      return (HWDB)0xFFFF ;

   hwdb = (*lpfn_wdbCloseSession)( hWDB ) ;

   FreeLibrary( hmodWDB ) ;
   hmodWDB = NULL ;

   return hwdb ;
}

UINT FAR PASCAL wdbGetOutput( HWDB hWDB, LPSTR lpszFile )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return WDB_OUTPUT_WDBLIB ;       // 0xFFFF

   return (*lpfn_wdbGetOutput)( hWDB, lpszFile ) ;
}

BOOL FAR PASCAL wdbSetOutput( HWDB hWDB, UINT wOutput, LPSTR lpszFile )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbSetOutput)( hWDB, wOutput, lpszFile ) ;
}

BOOL FAR PASCAL wdbAssertBox( HWDB hWDB, LPSTR p1, LPSTR p2, UINT p3 )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (BOOL)NULL ;

   return (*lpfn_wdbAssertBox)( hWDB, p1, p2, p3 ) ;
}

int FAR PASCAL wdbPuts( HWDB hWDB, LPSTR lpsz )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPuts)( hWDB, lpsz ) ;
}

int FAR PASCAL wdbPuts2( HWDB hWDB, LPSTR lpsz )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPuts2)( hWDB, lpsz ) ;
}

int FAR PASCAL wdbPuts3( HWDB hWDB, LPSTR lpsz )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPuts3)( hWDB, lpsz ) ;
}

int FAR PASCAL wdbPuts4( HWDB hWDB, LPSTR lpsz )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPuts4)( hWDB, lpsz ) ;
}

int FAR PASCAL wdbPuts5( HWDB hWDB, LPSTR lpsz )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPuts5)( hWDB, lpsz ) ;

}

int FAR _cdecl wdbPrintf( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrintf)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrintf2( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrintf2)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrintf3( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrintf3)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrintf4( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrintf4)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrintf5( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrintf5)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbVPrintf( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbVPrintf)( hWDB, lpszFormat, pArgs ) ;
}

int FAR _cdecl wdbVPrintf2( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbVPrintf2)( hWDB, lpszFormat, pArgs ) ;
}

int FAR _cdecl wdbVPrintf3( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbVPrintf3)( hWDB, lpszFormat, pArgs ) ;
}

int FAR _cdecl wdbVPrintf4( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbVPrintf4)( hWDB, lpszFormat, pArgs ) ;
}

int FAR _cdecl wdbVPrintf5( HWDB hWDB, LPSTR lpszFormat, LPSTR pArgs )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbVPrintf5)( hWDB, lpszFormat, pArgs ) ;
}

int FAR _cdecl wdbPrint( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrint)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrint2( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrint2)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrint3( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrint3)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrint4( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrint4)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}

int FAR _cdecl wdbPrint5( HWDB hWDB, LPSTR lpszFormat, ... )
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbPrint5)( hWDB, lpszFormat, (LPSTR)(&lpszFormat + 1) ) ;
}


BOOL WINAPI wdbMonoQuery(VOID)
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbMonoQuery)() ;
}

BOOL WINAPI wdbMonoOpen(VOID)
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbMonoOpen)() ;
}

BOOL WINAPI wdbMonoClose(VOID)
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbMonoClose)() ;
}

BOOL WINAPI wdbMonoOut(LPCSTR lpsz)
{
   if (!hmodWDB && !wdbLoadWDBLIB())
      return (int)NULL ;

   return (*lpfn_wdbMonoOut)(lpsz) ;

}


/** EOF: wdblib.c **/

