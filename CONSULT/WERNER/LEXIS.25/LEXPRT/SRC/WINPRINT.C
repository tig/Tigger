/************************************************************************
 *
 *     Project:  WINPRINT
 *
 *      Module:  WINPRINT.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *     00.00.000  3/10/91 cek   First version
 *
 ************************************************************************/

#include "..\inc\WINPRINT.h"

#include <wfext.h>



#include "..\inc\menu.h"
#include "..\inc\about.h"

/*
 * Exported variables
 * These are "extern'd" in winprint.h
 */
HMODULE     hmodDLL ;





BOOL WINAPI WinPrintInit( VOID ) ;


DWORD FAR PASCAL fnFileMan( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
static FARPROC lpfnOldFM ;

int WINAPI LibMain( HMODULE hModule, UINT wDataSeg, UINT cbHeapSize, LPSTR lpszCmdLine ) 
{

   if (cbHeapSize == 0)
      return FALSE ;

   hmodDLL = hModule ;

   WinPrintInit() ;

   return TRUE ;
}

/****************************************************************
 *  BOOL WINAPI WinPrintInit( )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI WinPrintInit( VOID )
{
   #ifdef DEBUG
      WORD w ;
   #endif 


   D( hWDB = wdbOpenSession( NULL, "WINPRINT", "wdb.ini", WDB_LIBVERSION ) ) ;

   D( w = wdbGetOutput( hWDB, NULL ) ) ;

   D( wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ) ;

   #ifdef DEBUG
   {
      char szVersion[64] ;
      GetWINPRINTVersion( szVersion ) ;
   }
   #endif

   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;

} /* WinPrintInit()  */


/*************************************************************************
 *  HANDLE WINAPI GetWINPRINThInst( void )
 *
 *  Description: 
 *
 *    Returns the hInst of the WINPRINT.
 *
 *  Comments:
 *
 *************************************************************************/
HMODULE WINAPI GetWINPRINThInst( void )
{
   return hmodDLL ;
}/* GetWINPRINThInst() */

/****************************************************************
 *  LPSTR WINAPI GetWINPRINTVersion( LPSTR lpszVersion )
 *
 *  Description:
 *
 *  Returns the current version number in the form 00.00.00 in the
 *  string passed as lpszVersion.  A pointer to this string is
 *  also returned.
 * 
 *  NOTE:  The address passed must point to a string that is at
 *         least 22 bytes long.
 * 
 *  Comments:
 *
 ****************************************************************/
LPSTR WINAPI GetWINPRINTVersion( LPSTR lpszVersion )
{
   if ( VER_BUILD )
      wsprintf( lpszVersion, (LPSTR)"%d.%.2d.%.3d",
                VER_MAJOR, VER_MINOR, VER_BUILD ) ;
   else
      wsprintf( lpszVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

   DP1( hWDB, "\n**** %s %s %s (Windows Version 0x%04X)",
              (LPSTR)"WinPrint",
              (LPSTR)"Version",
              (LPSTR)lpszVersion,
              LOWORD(GetVersion()) ) ;

   return lpszVersion ;
} /* VerGetWINPRINTVersion()  */

/****************************************************************
 *  LRESULT WINAPI FMExtensionProc( HWND hwnd, UINT wMsg, LPARAM lParam)
 *
 *  Description: 
 *
 *    File Manager extension callback.
 *
 *  Comments:
 *
 ****************************************************************/
LRESULT WINAPI FMExtensionProc( HWND hwnd, UINT wMsg, LPARAM lParam)
{
   /*
   * Filemananger globals
   */
   static   HMENU    hmenu;
   static   UINT     wMenuDelta;
   static   BOOL     bMultiple = FALSE;
   static   BOOL     bLFN = FALSE;

   char     szBuf[200];
   char     szBuf1[200];
   int      count;

   switch (wMsg)
   {
      case FMEVENT_LOAD:
         DP3( hWDB, "FMEVENT_LOAD\r\n" ) ;
         #define lpload  ((LPFMS_LOAD)lParam)

         /* Save the menu-item delta value. */

         wMenuDelta = lpload->wMenuDelta;

         /* Fill the FMS_LOAD structure. */

         lpload->dwSize = sizeof(FMS_LOAD);
         lstrcpy(lpload->szMenuName, "Win&Print");

         GetWindowText( hwnd, szBuf, 200 ) ;
         wsprintf( szBuf1, "Fileman = %s\r\n", (LPSTR)szBuf ) ;

//         lpfnOldFM = SubclassWindow(hwnd, (FARPROC)fnFileMan ) ;

         /* Return the handle of the menu. */

         lpload->hMenu = LoadMenu( hmodDLL, MAKEINTRESOURCE(IDMYMENU) ) ;

         return (LRESULT)lpload->hMenu ;
      break;

      case FMEVENT_UNLOAD:
         DP3( hWDB, "FMEVENT_UNLOAD\r\n" ) ;

         /* Perform any clean-up procedures here. */
         if (lpfnOldFM && (hwnd && IsWindow( hwnd )))
            SetWindowLong( hwnd, GWL_WNDPROC, (LONG)lpfnOldFM ) ;
   
      break;

      case FMEVENT_INITMENU:

         /* Copy the menu-item delta value and menu handle. */

         wMenuDelta = LOWORD(lParam);
         hmenu = (HMENU)HIWORD(lParam);

         /*
          * Add check marks to menu items as appropriate. Add menu-item
          * delta value to menu-item identifiers to specify the menu
          * items to check.
          */

         CheckMenuItem( hmenu, wMenuDelta + IDM_MULTIPLE,
                        bMultiple ? MF_BYCOMMAND | MF_CHECKED :
                                    MF_BYCOMMAND | MF_UNCHECKED);

         CheckMenuItem( hmenu, wMenuDelta + IDM_LFN,
                        bLFN ? MF_BYCOMMAND | MF_CHECKED :
                               MF_BYCOMMAND | MF_UNCHECKED);


         #if 0
         /*
          * Gray out my copyright stuff
          */
         EnableMenuItem( hmenu, wMenuDelta + IDM_COPYRIGHT,
                                MF_BYCOMMAND | MF_GRAYED ) ;
         EnableMenuItem( hmenu, wMenuDelta + IDM_ALLRIGHTSRESERVED,
                                MF_BYCOMMAND | MF_GRAYED ) ;
         EnableMenuItem( hmenu, wMenuDelta + IDM_CURRENTCONFIGLBL,
                                MF_BYCOMMAND | MF_GRAYED ) ;
         EnableMenuItem( hmenu, wMenuDelta + IDM_CURRENTCONFIG,
                                MF_BYCOMMAND | MF_GRAYED ) ;
         #endif
      break;

      case FMEVENT_USER_REFRESH:
         MessageBox(hwnd, "User refresh event", "Hey!", MB_OK);
      break;

      /*
       * The following messages are generated when the user selects
       * items from the WinPrint menu.
       */
      case IDM_PRINT:
         DP3( hWDB, "IDM_PRINT" ) ;
      break ;

      case IDM_PRINTERSETUP:
         DP3( hWDB, "IDM_PRINTERSETUP" ) ;
      break ;

      case IDM_PAGELAYOUT:
         DP3( hWDB, "IDM_PAGELAYOUT" ) ;
      break ;

      case IDM_OPTIONS:
         DP3( hWDB, "IDM_OPTIONS" ) ;
      break ;

      case IDM_OPENCONFIG:
         DP3( hWDB, "IDM_OPENCONFIG" ) ;
      break ;

      case IDM_SAVECONFIG:
         DP3( hWDB, "IDM_SAVECONFIG" ) ;
      break ;

      case IDM_CURRENTCONFIG:
         DP3( hWDB, "IDM_CURRENTCONFIG" ) ;
      break ;



      case IDM_ABOUT:
         DoAboutBox( hwnd );
      break ;


      case IDM_GETFOCUS:
         wsprintf( szBuf, "Focus %d",
                   (int)SendMessage(hwnd, FM_GETFOCUS, 0, 0L ) ) ;
         DP( hWDB, szBuf ) ;
         MessageBox(hwnd, szBuf, "Focus", MB_OK);
      break;

      case IDM_GETCOUNT:
         count = (int)SendMessage(hwnd,
               bLFN ? FM_GETSELCOUNTLFN : FM_GETSELCOUNT, 0, 0L);

         wsprintf(szBuf, "%d files selected", count);
         MessageBox(hwnd, szBuf, "Selection Count", MB_OK);
      break;

      case IDM_GETFILE:
      {
         FMS_GETFILESEL file;

         count = (int) SendMessage(hwnd,
         bLFN ? FM_GETSELCOUNTLFN : FM_GETSELCOUNT,
         FMFOCUS_DIR, 0L);

         while (count >= 1)
         {

            /* Selection indexes are zero-based (0 is first). */

            count--;
            SendMessage(hwnd, FM_GETFILESEL, count,

            (LONG) (LPFMS_GETFILESEL)&file);
            OemToAnsi(file.szName, file.szName);
            wsprintf(szBuf, "file %s\nSize %ld", (LPSTR)file.szName,
            file.dwSize);

            MessageBox(hwnd, szBuf, "File Information", MB_OK);

            if (!bMultiple)
               break;
         }
      }
      break;

      case IDM_GETDRIVE:
      {
         FMS_GETDRIVEINFO drive;

         SendMessage(hwnd, FM_GETDRIVEINFO, 0,

         (LONG) (LPFMS_GETDRIVEINFO)&drive);

         OemToAnsi(drive.szVolume, drive.szVolume);
         OemToAnsi(drive.szShare, drive.szShare);

         wsprintf(szBuf,
               "%s\nFree Space %ld\nTotal Space %ld\nVolume %s\nShare %s",
               (LPSTR) drive.szPath, drive.dwFreeSpace,
               drive.dwTotalSpace, (LPSTR) drive.szVolume,
               (LPSTR) drive.szShare);
         MessageBox(hwnd, szBuf, "Drive Info", MB_OK);

      }
      break;


      case IDM_LFN:
         bLFN = !bLFN;
      break;

      case IDM_MULTIPLE:
         bMultiple = !bMultiple;
      break;

      case IDM_REFRESH:
      case IDM_REFRESHALL:
         SendMessage(hwnd, FM_REFRESH_WINDOWS,
                           wMsg == IDM_REFRESHALL, 0L);
      break;

      case IDM_RELOAD:
         PostMessage(hwnd, FM_RELOAD_EXTENSIONS, 0, 0L);
      break;
   }

   return NULL ;
} /* FMExtensionProc()  */


DWORD FAR PASCAL fnFileMan( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   char szBuf[100] ;

   switch( wMsg )
   {
      case WM_COMMAND:
         wsprintf( szBuf, "WM_COMMAND: %04X, %08lX\r\n", wParam, lParam ) ;

         DP( hWDB,  szBuf ) ;

      break ;
   }

   return CallWindowProc( (WNDPROC)lpfnOldFM, hWnd, wMsg, wParam, lParam ) ;

}



/************************************************************************
 * End of File: WINPRINT.c
 ************************************************************************/

