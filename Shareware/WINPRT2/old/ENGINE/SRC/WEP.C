/************************************************************************
 *
 *     Project:  
 *
 *      Module:  wep.c
 *
 *     Remarks:  Generic WEP.  Must be in a fixed CS.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "engine.h"


int WINAPI LibMain( HMODULE hModule, UINT wDataSeg, UINT cbHeapSize, LPSTR lpszCmdLine ) 
{
   #ifdef DEBUG
      UINT w ;
   #endif 

   if (cbHeapSize == 0)
      return FALSE ;

   ghmodDLL = hModule ;

   D( hWDB = wdbOpenSession( NULL, "ENGINE", "wdb.ini", WDB_LIBVERSION ) ) ;

   D( w = wdbGetOutput( hWDB, NULL ) ) ;

   D( wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ) ;

   #ifdef DEBUG
   {
      char szVersion[64] ;
      if ( VER_BUILD )
         wsprintf( szVersion, (LPSTR)"%d.%.2d.%.3d",
                  VER_MAJOR, VER_MINOR, VER_BUILD ) ;
      else
         wsprintf( szVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

      DP1( hWDB, "\n**** %s %s %s (Windows Version 0x%04X)",
               (LPSTR)"ENGINE",
               (LPSTR)"Version",
               (LPSTR)szVersion,
               LOWORD(GetVersion()) ) ;

   }
   #endif

    RegisterPreview( ghmodDLL, GRCS( IDS_WNDCLASS_PREVIEW )) ;
    TimeResetInternational() ;

    D( wdbSetOutput( hWDB, w, NULL ) ) ;

    w = Ctl3dRegister( ghmodDLL ) ;
    DP1( hWDB, "w == %d", w ) ;
    
   return TRUE ;
}

/****************************************************************
 *  int WINAPI WEP( int bSystemExit )
 *
 ****************************************************************/
int WINAPI WEP( int bSystemExit )
{
   Ctl3dUnregister( ghmodDLL ) ;
    
   return 1 ;

} /* WEP()  */


/************************************************************************
 * End of File: wep.c
 ***********************************************************************/

