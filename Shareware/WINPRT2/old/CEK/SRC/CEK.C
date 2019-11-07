/*    PortTool v2.2     CEK.C          */

/************************************************************************
 *
 *     Project:  Project-42, CEK.DLL
 *
 *      Module:  CEK.c
 *
 *     Remarks:  Main module
 *
 *
 *   Revisions:  
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "iCEK.h"
#include "version.h"
#include "muscrdll.h"

BOOL     gfMonochrome = FALSE ;
HMODULE  ghmodDLL ;

DWORD    dwVersion ;       // HIWORD is build, LOWORD is version * 100
char     szVersion[32] ;
char     szModule[144] ;

int FAR PASCAL WEP( int bSystemExit ) ;

#ifndef WIN32
int WINAPI LibMain( HMODULE hModule, WORD wDS, WORD cbHeap, LPSTR lpszCmdLine )
#else
BOOL FAR PASCAL LibMain( HMODULE hModule,
                               DWORD  dwReason,
                               LPVOID lpReserved )
#endif
{
   HDC  hDC ;
   #ifdef DEBUG
   UINT w ;
   #endif

#ifdef WIN32
   switch( dwReason )
   {
      case DLL_PROCESS_ATTACH:
         DP( hWDB, "DLL_PROCESS_ATTACH" ) ;
      break ;

      case DLL_THREAD_ATTACH:
         DP( hWDB, "DLL_THREAD_ATTACH" ) ;
         return 0 ;
      break ;

      case DLL_PROCESS_DETACH:
         DP( hWDB, "DLL_PROCESS_DETATCH" ) ;
         
         /*
          * Call what in Win16 was the WEP to clean up.
          */
         WEP( 0 ) ;

         return 0 ;
      break ;

      case DLL_THREAD_DETACH:
         DP( hWDB, "DLL_THREAD_DETATCH" ) ;
         return 0 ;
      break ;

      default:
         DP( hWDB, "DLL_... default!" ) ;
      break ;
   }

#else

   if (cbHeap == 0)
      return FALSE ;

   UnlockData( 0 );

#endif

   ghmodDLL    = hModule ;
   dwVersion   = MAKELONG( (VER_MAJOR * 100) + (VER_MINOR), VER_BUILD ) ;

   if (hDC = GetDC( NULL ))
   {
      gfMonochrome = (GetDeviceCaps(hDC, BITSPIXEL) *
                     GetDeviceCaps( hDC, PLANES )) < 4 ;

      /* EGA is goofy */
      if (GetSystemMetrics( SM_CYSCREEN ) == 350 &&
          GetSystemMetrics( SM_CXSCREEN ) == 640)
         gfMonochrome = TRUE ;

      ReleaseDC( NULL, hDC ) ;
   }

   D( hWDB = wdbOpenSession( NULL, "CEK", "wdb.ini", WDB_LIBVERSION ) ) ;

   D( w = wdbGetOutput( hWDB, NULL ) ) ;
   D( wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ) ;

   D( cekGetVersion( szVersion ) ) ;
   D( GetModuleFileName( ghmodDLL, szModule, 144 ) ) ;  
   D( DP( hWDB, "\n*** %s Version %s ***", (LPSTR)szModule, (LPSTR)szVersion ) ) ;

   StatusInit( NULL, ghmodDLL, SZ_STATUSCLASSNAME ) ;
   LEDInit( NULL, ghmodDLL, SZ_LEDCLASSNAME ) ;

#ifndef WIN32
   SliderInit( NULL, ghmodDLL, SZ_SLIDERCLASSNAME ) ;
   ToolbarInit( NULL, ghmodDLL, SZ_TOOLBARCLASSNAME ) ;
#endif

   // register MUSCROLL
   FRegisterControl(ghmodDLL);

   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;
}

BOOL WINAPI cekCheckVersion( DWORD dwVer )
{

   return (LOWORD( dwVer ) <= _CEK_SPEC_VER_ ) ;
}

DWORD WINAPI cekGetVersion( LPSTR lpszVersion ) 
{
   if (lpszVersion)
   {
      if ( VER_BUILD )
         wsprintf( lpszVersion, (LPSTR)"%d.%.2d.%.3d",
                   VER_MAJOR, VER_MINOR, VER_BUILD ) ;
      else
         wsprintf( lpszVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;
   }

   return dwVersion ;
}



/************************************************************************
 * End of File: CEK.c
 ************************************************************************/

