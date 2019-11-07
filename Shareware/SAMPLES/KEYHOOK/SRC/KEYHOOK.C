/************************************************************************
 *
 *     Project:  State Farm Visual Basic Key Hook DLL
 *
 *      Module:  keyhook.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *   Revisions:  
 *     01.00.001  1/19/91 baw   Initial version, initial build
 *     01.00.000 10/28/91 cek   Modified for State Farm
 *
 ************************************************************************/

#define _WINDLL
#define STRICT
#include <windows.h>
#include <wdb.h>
#include "..\inc\keyhk.h"

extern HHOOK hHook ;

extern HWND rghWnd[] ;  // array of window handles

extern HWND hWndLast ;

extern BOOL fWin30 ;


LRESULT MyCallNextHookEx( HHOOK hHook, int nCode, WPARAM wP, LPARAM lP ) ;

/************************************************************************
 *  DWORD FAR PASCAL fnKeyboardHook( int nCode, WORD wParam,
 *                                         DWORD dwParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 ************************************************************************/

DWORD CALLBACK fnKeyboardHook( int nCode, WORD wParam, DWORD dwParam )
{
   WORD         wRepeat, wCount ;

   static BOOL  fShift = FALSE,
                fMenu = FALSE,
                fControl = FALSE ;

   if ((nCode >= 0) && (nCode != HC_NOREMOVE))
   {
      switch (wParam)
      {
         case VK_SHIFT:
            fShift = !(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80) ;
         break ;
   
         case VK_CONTROL:
            fControl = !(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80) ;
         break ;

         case VK_MENU:
            fMenu = !(HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80) ;
         break ;

         case VK_ESCAPE:
         {
            short i ;

            DP1( hWDB, "VK_ESCAPE" ) ;

            // Check if key is being released, if so ignore the message.
            // Yeah, it seems strange to do this here, but we want to
            // trap both states of the keys we are trapping, otherwise
            // we pass the keys onto the next proc.
   
            if (HIBYTE( HIWORD( dwParam ) ) & (BYTE) 0x80)
               return ( 1 ) ;
   
            wRepeat = LOWORD( dwParam ) ;

            /*
             * Post a message to all windows that called us
             */
            hWndLast = NULL ;
            for ( i = 0 ; i < MAX_NUM_WINDOWS ; i++)
               if (rghWnd[i])
               {
                  hWndLast = GetActiveWindow() ;

                  PostMessage( rghWnd[i], WM_KEYDOWN, 0x88, 0L ) ;
                  PostMessage( rghWnd[i], WM_KEYUP, 0x88, 0L ) ;
               }
         }
         break ;
      }
   }

   if (fWin30)
      return DefHookProc( nCode, wParam, dwParam, &hHook ) ;
   else
   {
      return MyCallNextHookEx( hHook, nCode, wParam, dwParam ) ;
   }

} /* end of ProcessEmulatorKeyDown() */


LRESULT MyCallNextHookEx( HHOOK hHook, int nCode, WPARAM wP, LPARAM lP )
{
   static BOOL (WINAPI *lpfnCNHE)(HHOOK, int, WPARAM, LPARAM) = NULL ;

   /*
    * CNHE() is exported from USER.EXE at ordinal # 293
    */
   if (lpfnCNHE == NULL && !fWin30)
      (FARPROC)lpfnCNHE = GetProcAddress( GetModuleHandle( "USER" ),
                                 (LPCSTR)MAKEINTRESOURCE( 293 ) ) ;

   if (lpfnCNHE != NULL)
      return (*lpfnCNHE)( hHook, nCode, wP, lP ) ;
   else
      return NULL ;
}

/************************************************************************
 * End of File: keyhook.c
 ************************************************************************/

