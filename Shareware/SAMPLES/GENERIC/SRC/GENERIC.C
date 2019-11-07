/************************************************************************
 *
 *     Copyright (c) 1992 Charles E. Kindel, Jr..  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  GENERIC 
 *
 *      Module:  GENERIC.c
 *
 *   Revisions:  
 *       8/13/92   cek   Wrote it.
 *
 ***********************************************************************/

#include "PRECOMP.H"

#include "version.h"

#include "GENERIC.h"

/*
 * Global variables extern'd in GENERIC.h
 */
HWND        hwndApp ;

HINSTANCE   hinstApp ;

LPSTR       rglpsz[LAST_IDS - FIRST_IDS + 1] ;

char        szVerNum[80] ;

/*
 * Variables local to this module.
 */ 
static HCURSOR    hcurWait ;
static HCURSOR    hcurNorm ;
static HCURSOR    hcurSave ;
static UINT       fWaitCursor ;

int FAR PASCAL MessageLoop( VOID ) ;
BOOL WINAPI InitClass( HINSTANCE hInstance ) ;
HWND WINAPI CreateMain( VOID ) ;
GLOBALHANDLE WINAPI GetStrings( HINSTANCE hInst ) ;
short FAR _cdecl ErrorResBox( HWND hwnd, HINSTANCE hInst, UINT flags,
                                        UINT idAppName, UINT idErrorStr, ...) ;
BOOL WINAPI DoAboutBox( VOID ) ;


/****************************************************************
 *  LRESULT FAR PASCAL
 *    fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *    Main Window procedure.
 *
 *  Comments:
 *
 ****************************************************************/

LRESULT CALLBACK fnMainWnd( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch( uiMsg )
   {
      case WM_CREATE:

         hwndApp = hWnd ;

      break ;

      case WM_COMMAND:
      {
         #ifdef WIN32
         WORD wNotifyCode = HIWORD(wParam);  
         WORD wID = LOWORD(wParam);         
         HWND hwndCtl = (HWND) lParam;      
         #else
         WORD wNotifyCode = HIWORD(lParam) ;
         WORD wID = wParam ;
         HWND hwndCtl = (HWND)LOWORD(lParam) ;
         #endif

         switch (wID)
         {
            case IDM_HELP_ABOUT:
               DoAboutBox() ;
            break ;

            case IDM_FILE_EXIT:
               SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
            break ;

            #ifdef DEBUG
            case IDM_WDB_CONFIG:
               wdbConfigDlg( hWDB ) ;
            break ;
            #endif 
         }
      }
      break ;

      case WM_DESTROY:
         PostQuitMessage( 0 ) ;
      break ;

      default:
         return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
   }

   return 0L ;

} /* fnMainWnd()  */

BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) ;

BOOL WINAPI DoAboutBox( VOID )
{
   DLGPROC  lpfnDlgProc ;

   (FARPROC)lpfnDlgProc = MakeProcInstance( (FARPROC)fnAbout, hinstApp ) ;
   DialogBox( hinstApp,
              MAKEINTRESOURCE( DLG_ABOUTBOX ),
              hwndApp,
              lpfnDlgProc ) ;
   FreeProcInstance( (FARPROC)lpfnDlgProc ) ;

   return TRUE ;
}

BOOL CALLBACK fnAbout( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) 
{
   char  szTemp [256] ;

   switch (wMsg)
   {
      case WM_INITDIALOG:

         #ifdef WIN32
         wsprintf( szTemp, "%s %s (Win32) - %s",
                   (LPSTR)rglpsz[IDS_VERSION], (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;
         #else
         wsprintf( szTemp, "%s %s (Win16) - %s",
                   (LPSTR)rglpsz[IDS_VERSION], (LPSTR)szVerNum,
                   (LPSTR)__DATE__ ) ;
         #endif
         SetDlgItemText (hDlg, IDD_VERSION, szTemp ) ;

         break ;

      case WM_COMMAND:
      {
         #ifdef WIN32
         WORD wNotifyCode = HIWORD(wParam);  
         WORD wID = LOWORD(wParam);         
         HWND hwndCtl = (HWND) lParam;      
         #else
         WORD wNotifyCode = HIWORD(lParam) ;
         WORD wID = wParam ;
         HWND hwndCtl = (HWND)LOWORD(lParam) ;
         #endif

         switch (wID)
         {
            case IDOK:
            case IDCANCEL:
               EndDialog (hDlg, wID) ;
            break ;

            default :
               return FALSE ;
         }
         break ;
      }
      break ;

   }
   return FALSE ;
}


/****************************************************************
 *  VOID WINAPI SetWaitCursor( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
UINT nCurCount ;

VOID WINAPI SetWaitCursor( VOID )
{
   DASSERT( hWDB, (short)fWaitCursor >= 0 ) ;

   if (!hcurWait)
      hcurWait = LoadCursor( NULL, IDC_WAIT ) ;

   fWaitCursor++ ;

   SetCursor( hcurWait ) ;

} /* SetWaitCursor()  */

/****************************************************************
 *  VOID WINAPI SetNormalCursor( VOID )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID WINAPI SetNormalCursor( VOID )
{
   DASSERT( hWDB, (short)fWaitCursor >= 0 ) ;
   if (!hcurNorm)
      hcurNorm = LoadCursor( NULL, IDC_ARROW ) ;

   fWaitCursor-- ;

   if (fWaitCursor)
      return ;

   SetCursor( hcurNorm ) ;

} /* SetNormalCursor()  */

/****************************************************************
 *  int FAR PASCAL MessageLoop( HACCEL haccl )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
int FAR PASCAL MessageLoop( VOID )
{
   MSG            msg ;

   DP5( hWDB, "Entering message loop..." ) ;
   while (GetMessage (&msg, NULL, 0, 0))
   {
      TranslateMessage( &msg ) ;
      DispatchMessage( &msg ) ;
   }

   return (int)msg.wParam ;

} /* MessageLoop()  */

/****************************************************************
 *  int PASCAL
 *    WinMain( HANDLE hinst, HANDLE hinstPrev, LPSTR lpszCmd, in nCmdShow )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
int PASCAL WinMain( HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmd, int nCmdShow )
{
   GLOBALHANDLE   gh ;
   int            n ;

   hinstApp = hinst ;

   SetWaitCursor() ;

   D( hWDB = wdbOpenSession( NULL, "GENERIC", "wdb.ini", 0 ) ) ;

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
   if (!(hwndApp = CreateMain()))
   {
      DP1( hWDB, "CreateMain failed" ) ;

      SetNormalCursor() ;

      ErrorResBox( NULL, hinst,
                   MB_ICONHAND, IDS_APPTITLE,  IDS_ERR_CREATEWINDOW ) ;

      goto HellInAHandBasket2 ;
   }

   SetNormalCursor() ;

   ShowWindow( hwndApp, nCmdShow ) ;
   UpdateWindow( hwndApp ) ;

   /*
    * Enter message loop.  This is in GENERIC.C so that it is in
    * a preload segment.
    */
   n = MessageLoop(  ) ;
   DP5( hWDB, "Exit from message loop" ) ;

   /*
    * Save defaults
    */
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
                                    MAKEINTRESOURCE( IDI_GENERIC ) ) ;
   wc.hCursor           = LoadCursor( NULL, IDC_ARROW ) ;
   wc.hbrBackground     = COLOR_APPWORKSPACE + 1 ;
   wc.lpszMenuName      = rglpsz[IDS_APPNAME] ;
   wc.lpszClassName     = rglpsz[IDS_APPNAME] ;

   if (!RegisterClass (&wc))
      return FALSE ;

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

   if (ghMem = GlobalAlloc( GHND, (LAST_IDS - FIRST_IDS) * 512 ))
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
         if (n = LoadString( hInst, i, lpCur, 512 ))
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

#define MAX_STRLEN   2048      // Maximum characters for ErrorResBox


/*************************************************************************
 *  short FAR _cdecl ErrorResBox( HWND hWnd, HANDLE hInst, UINT wFlags,
 *                                UINT idAppName, UINT idErrorStr,
 *                                LPSTR lpszParams, ... ) ;
 *
 *  Description:
 *
 *  This function displays a message box using program resource strings.
 *       
 *  Type/Parameter
 *          Description
 *
 *      HWND hWnd
 *            Specifies the message box parent window.
 *
 *      HANDLE hInst
 *            Specifies the handle to the module that contains the resource
 *            strings specified by idAppName and idErrorStr.  If this value
 *            is NULL, the instance handle is obtained from hWnd (in which
 *            case hWnd may not be NULL).
 *
 *      UINT wFlags
 *            Specifies message box types controlloing the message box
 *            appearance.  All message box types valid for MessageBox are valid.
 *      
 *      UINT idAPpName
 *            Specifies the resource ID of a string that
 *            is to be used as the message box caption.
 * 
 *      UINT idErrorStr
 *             Specifies the resource ID of a error message format string.
 *             This string is of the style passed to wsprintf(),
 *             containing the standard C argument formatting
 *             characters.  Any procedure parameters following
 *             idErrorStr will be taken as arguments for this format
 *             string.
 * 
 *      arguments [ arguments, ... ]
 *             Specifies additional arguments corresponding to the
 *             format specification given by idErrorStr.  All
 *             string arguments must be FAR pointers.
 * 
 * 
 *  Return Value
 *    Returns the result of the call to MessageBox().  If an
 *    error occurs, returns zero.
 *
 *  Comment:
 *       This is a variable arguments function, the parameters after
 *       idErrorStr being taken for arguments to the printf() format
 *       string specified by idErrorStr.  The string resources specified
 *       by idAppName and idErrorStr must be loadable using the
 *       instance handle hInst.  If the strings cannot be
 *       loaded, or hwnd is not valid, the function will fail and return
 *       zero.
 *
 *************************************************************************/
short FAR _cdecl ErrorResBox( HWND hwnd, HINSTANCE hInst, UINT flags,
                                        UINT idAppName, UINT idErrorStr, ...)
{
   LPSTR        sz = NULL ;
   LPSTR        szFmt = NULL ;
   UINT w ;

   DP( hWDB, "ErrorResBox( 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X )",
       hwnd,   hInst,  flags,  idAppName, idErrorStr ) ;
   if (!hInst)
   {
      if (!hwnd)
      {
         MessageBeep( 0 ) ;
         return FALSE ;
      }

      hInst = (HINSTANCE)GetWindowInstance( hwnd ) ;
   }

   if (szFmt = GlobalAllocPtr( GHND, 256 + 1 ))
   {
      if (!LoadString( hInst, idErrorStr, szFmt, 256 ))
         goto ExitError ;

      if (sz = GlobalAllocPtr( GHND, MAX_STRLEN ))
      {
         w = 0 ;

         wvsprintf( sz, szFmt, (LPSTR)(((UINT FAR *)&idErrorStr) + 1) ) ;

         if (!LoadString( hInst, idAppName, szFmt, 256 ))
            goto ExitError ;

         w = MessageBox( hwnd, sz, szFmt, flags ) ;

      }
   }

ExitError :
   DASSERT( hWDB, sz ) ;
   DASSERT( hWDB, szFmt ) ;

   if (sz)
      GlobalFreePtr( sz ) ;

   if (szFmt)
      GlobalFreePtr( szFmt ) ;

   return w ;

}/* ErrorResBox() */


/************************************************************************
 * End of File: GENERIC.c
 ***********************************************************************/

