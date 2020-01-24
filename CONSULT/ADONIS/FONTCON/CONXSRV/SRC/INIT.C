/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CONXSRV 
 *
 *      Module:  init.c
 *
 *     Remarks:  One time initialization code.
 *
 *   Revisions:  
 *       3/4/92   cek   Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXSRV.h"

#include "..\..\inc\version.h"
#include "config.h"
#include "botstat.h"
#include "init.h"
#include "subdlgs.h"
#include "cmd.h"
#include "dde.h"

extern char    **__argv ;
extern int     __argc ;

#define MAX_RESSTRING_LEN              ((WORD)(1L * 256L))

BOOL NEAR PASCAL DoOption( PSTR psOption ) ;

/*
 * Variables local to this module.
 */ 
static char       szWorkBuf[256] ;

/****************************************************************
 *  int WINAPI
 *    MyWinMain( HANDLE hinst, HANDLE hinstPrev, LPSTR lpszCmd, in nCmdShow )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
int WINAPI MyWinMain( HINSTANCE hinst, HINSTANCE hinstPrev, LPCSTR lpszCmd, int nCmdShow )
{
   GLOBALHANDLE   gh ;
   HACCEL         haccl ;
   int            n ;

   hinstApp = hinst ;

   SetWaitCursor() ;

   D( hWDB = wdbOpenSession( NULL, "CONXSRV", "wdb.ini", 0 ) ) ;

   if (!(gh = GetStrings( hinst )))
   {
      DP1( hWDB, "GetStrings failed!" ) ;

      SetNormalCursor() ;

      ErrorResBox( NULL, hinst,
                   MB_ICONHAND, IDS_APPTITLE, IDS_ERR_STRINGLOAD ) ;
      return FALSE ;
   }


   if ( VER_BUILD )
      wsprintf( szVerNum, (LPSTR)"%d.%.2d.%.3d",
                VER_MAJOR, VER_MINOR, VER_BUILD ) ;
   else
      wsprintf( szVerNum, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

   DP1( hWDB, "\n**** %s %s %s (Windows Version 0x%04X)",
              (LPSTR)rglpsz[IDS_APPNAME],
              (LPSTR)rglpsz[IDS_VERSION],
              (LPSTR)szVerNum,
              LOWORD(GetVersion()) ) ;

   /*
    * If there is already an instance running, bring it to the
    * foreground and close this one.
    *
    * We use the FindWindow method because hPrevInst is always NULL
    * in Windows NT.
    */
   if (hwndApp = FindWindow( rglpsz[IDS_APPNAME], NULL ))
   {
      HWND hwnd2 ;

      DP1( hWDB, "**** Previous Instance exists!  Activating it and closing this one.\n" ) ;

      hwnd2 = GetLastActivePopup( hwndApp ) ;

      //
      // If it's minimized, restore it.
      //
      if (hwndApp == hwnd2 && IsIconic( hwndApp ))
         SendMessage(hwndApp, WM_SYSCOMMAND, SC_RESTORE, 0) ;

      BringWindowToTop(hwnd2) ;
      SetActiveWindow(hwnd2) ;

      GlobalFree( gh ) ;
      return 0;
   }


   /*
    * Validate all DLL's that are version bound
    */
   #ifdef USE_TDUTIL
   if (!tdCheckVersion( _3DUTIL_SPEC_VER_ ))
   {
      SetNormalCursor() ;

      DP1( hWDB, "_3DUTIL_SPEC_VER_ = %04X, Version = %08lX",
               _3DUTIL_SPEC_VER_, tdGetVersion( NULL ) ) ;
      ErrorResBox( NULL, hinst,
                     MB_ICONHAND, IDS_APPTITLE, 
                     IDS_ERR_BADDLLVER,
                     (LPSTR)"TDUTIL.DLL",
                     _3DUTIL_SPEC_VER_ / 100 ) ;

      /* "The version of 3DUTIL.DLL on this system is old.\n"\
         "Please upgrade to version %d.00 or higher.",
         _3DUTIL_SPEC_VER_ / 100 ) ; */

      goto HellInAHandBasket2 ;

   }

   DP4( hWDB, "   TDUTIL.DLL     Version %08lX (Spec = %04X)",
      tdGetVersion( NULL ), _3DUTIL_SPEC_VER_ ) ;

   #endif

   #ifdef USE_LSTUTIL
   if (!lstCheckVersion( _LSTUTIL_SPEC_VER_ ))
   {
      SetNormalCursor() ;

      DP1( hWDB, "_LSTUTIL_SPEC_VER_ = %08lX, Version = %04X",
         _LSTUTIL_SPEC_VER_, lstGetVersion( NULL ) ) ;
      ErrorResBox( NULL, hinst,
                     MB_ICONHAND,IDS_APPTITLE, 
                     IDS_ERR_BADDLLVER,
                     (LPSTR)"LSTUTIL.DLL",
                     _LSTUTIL_SPEC_VER_ / 100 ) ;


      goto HellInAHandBasket2 ;
   }

   DP4( hWDB, "   LSTUTIL.DLL    Version %08lX", lstGetVersion( NULL ) ) ;

   #endif


   /*
    * Initialize secondary heaps.
    */

   if (!hinstPrev)
      if (!InitClass( hinst ))
      {
         DP1( hWDB, "InitClass failed" ) ;

         SetNormalCursor() ;

         ErrorResBox( NULL, hinst,
                      MB_ICONHAND,IDS_APPTITLE, 
                      IDS_ERR_INITCLASS ) ;

         goto HellInAHandBasket2 ;
      }

   /*
    * Read defaults
    */
   RestoreDefaults( ) ;


   /*
    * Read command line.
    */
   if (DoCmdLine( szWorkBuf ))
   {

   }
   else
   {
      SetNormalCursor() ;

      ErrorResBox( hwndApp, hinst, MB_ICONEXCLAMATION,
                   IDS_APPTITLE, IDS_ERR_COMMANDLINE,
                   (LPSTR)szWorkBuf ) ;

   }

   hfontCur = ReallyCreateFontEx( NULL, szFaceName, szFaceStyle, wFaceSize, 0 ) ;

   /*
    * Get a really useful small (Helv, 8) font
    */
   hfontSmall = ReallyCreateFont( NULL, "Helv", 8, RCF_NORMAL ) ;
   if (!(hwndApp = CreateMain()))
   {
      DP1( hWDB, "CreateMain failed" ) ;

      SetNormalCursor() ;

      ErrorResBox( NULL, hinst,
                   MB_ICONHAND, IDS_APPTITLE,  IDS_ERR_CREATEWINDOW ) ;

      goto HellInAHandBasket2 ;
   }

   DP1( hWDB, "hwndApp = %04X, hwndTTY = %04X, hwndStat = %04X",
                     hwndApp, hwndTTY, hwndStat ) ;

   if (!ddeInitDDEServer( ))
   {
      DP1( hWDB, "ddeInitDDEServer() failed!" ) ;

      SetNormalCursor() ;

      ErrorResBox( NULL, hinst,
                   MB_ICONHAND, IDS_APPTITLE, 
                   IDS_ERR_DDESERVERINIT ) ;

      goto HellInAHandBasket2 ;
   }


   UpdateInfo( ) ;

   RestoreWndPos( hwndApp ) ;

   haccl = LoadAccelerators( hinst, MAKEINTRESOURCE( 1 ) ) ;

   SetNormalCursor() ;

   /*
    * Enter message loop.  This is in CONXSRV.C so that it is in
    * a preload segment.
    */
   n = MessageLoop( haccl ) ;
   DP5( hWDB, "Exit from message loop" ) ;

   /*
    * Save defaults
    */
   SetWaitCursor() ;

   ddeUnInitDDEServer( ) ;

   SetNormalCursor() ;

   if (hfontCur)
      DeleteObject( hfontCur ) ;

   if (hfontSmall)
      DeleteObject( hfontSmall ) ;
HellInAHandBasket2:
                        
   GlobalUnlock( gh ) ;
   GlobalFree( gh ) ;

   return n ;

} /* MyWinMain()  */


/****************************************************************
 *  BOOL WINAPI InitClass( HINSTANCE hInstance )
 *
 *  Description: 
 *
 *    Registers the window classes.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI InitClass( HINSTANCE hInstance )
{
   WNDCLASS    wc ;
   BOOL        f = TRUE ;

   wc.style             = 0L ;
   wc.lpfnWndProc       = fnMainWnd ;
   wc.cbClsExtra        = 0 ;
   wc.cbWndExtra        = 0 ;
   wc.hInstance         = hInstance ;
   wc.hIcon             = LoadIcon( hInstance,
                                    MAKEINTRESOURCE( IDI_CONXSRV ) ) ;
   wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
   (WORD)wc.hbrBackground     = COLOR_APPWORKSPACE + 1 ;
   wc.lpszMenuName      = rglpsz[IDS_APPNAME] ;
   wc.lpszClassName     = rglpsz[IDS_APPNAME] ;

   if (!RegisterClass (&wc))
      return FALSE ;

   wc.style             = CS_HREDRAW | CS_VREDRAW ;
   wc.lpfnWndProc       = fnTTYWnd ;
   wc.cbClsExtra        = 0 ;
   wc.cbWndExtra        = CBWNDEXTRA_TTY ;
   wc.hInstance         = hInstance ;
   wc.hIcon             = NULL ;
   wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
   (WORD)wc.hbrBackground     = COLOR_WINDOW + 1 ;
   wc.lpszMenuName      = NULL ;
   wc.lpszClassName     = SZ_TTYCLASSNAME ;

   if (!RegisterClass (&wc))
      return FALSE ;

   /*
    * Need to reference a fn in SHOPLIB to force it to
    * link.
    */
   ShopLibInit( ) ;

   return TRUE ;


} /* InitClass()  */


/****************************************************************
 *  HWND WINAPI CreateMain( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
HWND WINAPI CreateMain( VOID )
{
   HWND hwnd ;

   hwnd = CreateWindow
      (
         rglpsz[IDS_APPNAME],
         rglpsz[IDS_APPTITLE],
         WS_OVERLAPPEDWINDOW |
         WS_CLIPCHILDREN,  
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         CW_USEDEFAULT,
         NULL,                 
         NULL,                 
         hinstApp,            
         NULL                  
      ) ;

   return hwnd ;

} /* CreateMain()  */


/****************************************************************
 *  GLOBALHANDLE WINAPI GetStrings( hInst )
 *
 *  Description: 
 *
 *    Fills lprgsz[] with resource strings.
 *
 *  Comments:
 *
 ****************************************************************/
GLOBALHANDLE WINAPI GetStrings( HINSTANCE hInst )
{
   GLOBALHANDLE   ghMem ;
   LPSTR          lpMem ;
   LPSTR          lpCur ;
   short          i ;
   short          n ;

   if (ghMem = GlobalAlloc( GHND, (LAST_IDS - FIRST_IDS) * MAX_RESSTRING_LEN ))
   {
      if (!(lpMem = GlobalLock( ghMem )))
      {
         DASSERT( hWDB, lpMem ) ;
         GlobalFree( ghMem ) ;
         return FALSE ;
      }

      /*
       * Load each string and keep it's pointer
       */
      lpCur = lpMem ;

      for (i = FIRST_IDS ; i <= LAST_IDS ; i++)
      {
         if (n = LoadString( hInst, i, lpCur, MAX_RESSTRING_LEN ))
         {
            rglpsz[i] = lpCur ;
            lpCur += n + 1 ;
         }
         else
         {
            DP1( hWDB, "LoadString 0x%02x failed", i ) ;
            GlobalUnlock( ghMem ) ;
            GlobalFree( ghMem ) ;
            return NULL ;
         }
      }

      /*
       * Now reallocate the block so it is just big enough
       */
      DP3( hWDB, "\n%d bytes in %d pre-loaded strings",
                 (lpCur - lpMem + 16), LAST_IDS - FIRST_IDS ) ;
      GlobalReAlloc( ghMem, (lpCur - lpMem + 16), GHND ) ;
      GlobalLock( ghMem ) ;

      return ghMem ;
   }
   else
   {
      DASSERT( hWDB, ghMem ) ;
      return NULL ;
   }

   return NULL ;

} /* GetStrings()  */

/****************************************************************
 *  BOOL WINAPI DoCmdLine( LPSTR lpszResult )
 *
 *  Description: 
 *
 *    Parses the command line.  if it returns false, the
 *    lpszResult parameter points to the string that caused the
 *    error.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI DoCmdLine( LPSTR lpszResult )
{
   int      i ;

   for (i = 1 ; i < __argc ; i++)
   {
      DP5( hWDB, "__argv[%d] = '%s'", i, (LPSTR)__argv[i] ) ;

      /*
       * Is it a switch?
       */
      if ((*__argv[i] == '/') || (*__argv[i] == '-'))
      {
         if (!DoOption (__argv[i]))
         {
            lstrcpy (lpszResult, __argv[i]) ;
            return FALSE ;
         }
      }
      else
      {
         /*
          * It's not a switch, so it must be a file name...
          */
         /* __argv[i] is a filename */


         /*
          * Verify that it's a valid INI file
          */
         if (1) //ValidINIfile( hFile ))
         {
            /*
             * Make it active and add it to the list
             */
            lstrcpy( lpszResult, __argv[i] ) ;

            return TRUE ;
         }
      }
   }

   return TRUE ;

} /* DoCmdLine()  */


/****************************************************************
 *  BOOL NEAR PASCAL DoOption( PSTR psOption )
 *
 *  Description: 
 *
 *    Given an option, figure out what it does.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL DoOption( PSTR psOption )
{
   PSTR  psString ;

   psString = psOption ;

   /*
    * check to see if it's a +,- option
    */

   if (!strchr( psString, ':' ))
   {
      switch (toupper(*++psString))
      {
         default:
            return FALSE ;
      }
   }
   else  /* We have a colon (:) */
   {
      switch (toupper(*++psString))
      {

         default:
            return FALSE ;

      } /* switch */

   } /* else */

   return TRUE ;

} /* DoOption()  */


/************************************************************************
 * End of File: init.c
 ***********************************************************************/

