/************************************************************************
 *
 *     Project:  Project-42, TDUTIL.DLL
 *
 *      Module:  TDUTIL.c
 *
 *     Remarks:  Main module
 *
 *
 *   Revisions:  
 *
 ***********************************************************************/
#include "PRECOMP.H"

#include "itdutil.h"
#include "version.h"

BOOL     fMonochrome = FALSE ;
HBITMAP  hbmBtn ;
LRESULT  (CALLBACK *lpfnDefButton)(HWND, UINT, WPARAM, LPARAM ) ;
HMODULE  hInstDLL ;
DWORD    dwVersion ;       // HIWORD is build, LOWORD is version * 100
char     szVersion[32] ;
char     szModule[144] ;
UINT     dxBorder;
UINT     dyBorder;
HBRUSH   hbrBkGnd ;

int FAR PASCAL WEP( int bSystemExit ) ;


#ifndef WIN32
BOOL FAR PASCAL LibMain( HANDLE hModule,
                         WORD   wDataSeg,
                         WORD   cbHeapSize,
                         LPSTR  lpszCmdLine )
#else
BOOL FAR PASCAL LibMain( HANDLE hModule,
                               DWORD  dwReason,
                               LPVOID lpReserved )
#endif
{
   HDC  hDC ;
   WNDCLASS wc ;
   #ifdef DEBUG
   WORD w ;
   #endif

#ifdef WIN32
   char szBuf[256] ;
#endif

   D( hWDB = wdbOpenSession( NULL, "TDUTIL", "wdb.ini", WDB_LIBVERSION ) ) ;

   D( w = wdbGetOutput( hWDB, NULL ) ) ;
   D( wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ) ;

   D( tdGetVersion( szVersion ) ) ;
   D( DP( hWDB, "\n*** %s Version %s ***", (LPSTR)"TDUTIL.DLL",
                                       (LPSTR)szVersion ) ) ;
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
         DP( hWDB, "DLL... default!" ) ;
      break ;
   }

   UNREFERENCED_PARAMETER(lpReserved) ;

#else
   if (cbHeapSize == 0)
      return FALSE ;
#endif

   hInstDLL    = hModule ;
   dwVersion   = MAKELONG( (VER_MAJOR * 100) + (VER_MINOR), VER_BUILD ) ;

   if (hDC = GetDC( NULL ))
   {
      fMonochrome = (GetDeviceCaps(hDC, BITSPIXEL) *
                     GetDeviceCaps( hDC, PLANES )) < 4 ;

      /* EGA is goofy */
      if (GetSystemMetrics( SM_CYSCREEN ) == 350 &&
          GetSystemMetrics( SM_CXSCREEN ) == 640)
         fMonochrome = TRUE ;

#ifdef MULTICOLOR
      SetupColorTable( hDC ) ;
#endif

      ReleaseDC( NULL, hDC ) ;
   }
   else
   {
      DP1( hWDB, "Could not GetDC()!!" ) ;
   }

   if (fMonochrome == FALSE)
   {
      DP1( hWDB, "Non Monochrome device" ) ;
   }
   else
   {
      DP1( hWDB, "Monochrome device" ) ;
   }

   /*************************************************************
    * Initialize stuff for subclassing the static class
    *************************************************************/
   wc.style = CS_DBLCLKS |
              CS_VREDRAW |
              CS_HREDRAW |
              CS_GLOBALCLASS ;

   wc.lpfnWndProc = (WNDPROC)fn3DStatic ;

   wc.cbClsExtra = 0 ;

   //
   // cbWndExtra holds a handle to the STATUS structure
   //
   wc.cbWndExtra = sizeof( HFONT ) ;

   wc.hInstance = hModule ;
   wc.hIcon = NULL ;
   wc.hCursor = LoadCursor( NULL, IDC_ARROW ) ;
   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1) ;
   wc.lpszMenuName = NULL ;
   wc.lpszClassName = SZ_TDUTILSTATIC ;

   if (RegisterClass( &wc ) == 0)
   {
      DP1( hWDB, "RegisterClass for tdutil.static failed!!!" ) ;
      return FALSE ;
   }

   /*************************************************************
    * Initialize stuff for subclassing buttons
    *************************************************************/
   if (!fMonochrome)
   {
      if (hbmBtn = LoadBitmap( hInstDLL, MAKEINTRESOURCE( IDB_3DBKGND ) ))
      {
         hbrBkGnd = CreatePatternBrush( hbmBtn ) ;
         DeleteObject( hbmBtn ) ;
      }
      else
      {
         DP1( hWDB, "hInstDLL = %08X, bmp ID = %08X", hInstDLL, MAKEINTRESOURCE( IDB_3DBKGND ) ) ;
      }

      if (hbmBtn = LoadBitmap( hInstDLL, MAKEINTRESOURCE( IDB_3DBTN ) ))
      {
         GetClassInfo( NULL, "button", (LPWNDCLASS)&wc ) ;
         lpfnDefButton = wc.lpfnWndProc ;

         dxBorder = GetSystemMetrics( SM_CXBORDER ) ;
         dyBorder = GetSystemMetrics( SM_CYBORDER ) ;
      }
      else
      {
         DP1( hWDB, "LoadBitmap failed!" ) ;
      }
   }

   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;
}

/*************************************************************************
 *  HMODULE  WINAPI tdGethInst( VOID )
 *
 *  Description: 
 *
 *    Returns the hInstDLL of the TDUTIL.
 *
 *  Comments:
 *
 *************************************************************************/
HMODULE WINAPI tdGethInst( VOID )
{
   return hInstDLL ;

}/* tdGethInst() */


BOOL WINAPI tdCheckVersion( DWORD dwVer )
{
   return (LOWORD( dwVer ) <= _TDUTIL_SPEC_VER_ ) ;
}

DWORD WINAPI tdGetVersion( LPSTR lpszVersion ) 
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
 * End of File: TDUTIL.c
 ************************************************************************/

