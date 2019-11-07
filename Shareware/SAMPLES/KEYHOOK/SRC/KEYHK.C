/************************************************************************
 *
 *     Project:  KEYHK.DLL
 *
 *      Module:  keyhk.c
 *
 *     Remarks:  Main module
 *
 *   Revisions:  
 *
 ***********************************************************************/

#define STRICT
#define _WINDLL
#include <windows.h>
#include <wdb.h>
#include "..\inc\keyhk.h"
#include "..\inc\version.h"


HANDLE   hInstDLL ;
DWORD    dwVersion ;       // HIWORD is build, LOWORD is version * 100
char     szVersion[32] ;
char     szModule[144] ;

HHOOK    hHook ;

HWND     rghWnd[MAX_NUM_WINDOWS] ;
HWND     hWndLast ;

BOOL     fWin30 ;

VOID FAR PASCAL DOS3Call(VOID) ;

HHOOK MySetWindowsHookEx( int idHook, HOOKPROC lpfn, HINSTANCE hinst, HTASK htask ) ;
LRESULT MyUnhookWindowsHookEx( HHOOK hHook ) ;

int FAR PASCAL LibMain( HANDLE hModule, WORD wDS, WORD cbHeap, LPSTR lpszCmdLine )
{
   #ifdef DEBUG
   UINT w ;
   #endif

   if (cbHeap == 0)
      return FALSE ;

   fWin30 = ((BOOL)(LOWORD( GetVersion() ) < 0x0A03)) ;

   hInstDLL    = hModule ;
   dwVersion   = MAKELONG( (VER_MAJOR * 100) + (VER_MINOR), VER_BUILD ) ;

   D( hWDB = wdbOpenSession( NULL, "keyhk", "wdb.ini", WDB_LIBVERSION ) ) ;

   D( w = wdbGetOutput( hWDB, NULL ) ) ;

   D( wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ) ;

   D( khGetVersion( szVersion ) ) ;
   D( GetModuleFileName( hInstDLL, szModule, 144 ) ) ;  
   D( DP( hWDB, "\n*** %s Version %s ***", (LPSTR)szModule, (LPSTR)szVersion ) ) ;

   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;
}

/*************************************************************************
 *  HANDLE FAR PASCAL khGethInst( VOID )
 *
 *  Description: 
 *
 *    Returns the hInstDLL of the keyhk.
 *
 *  Comments:
 *
 *************************************************************************/
HANDLE FAR PASCAL khGethInst( VOID )
{
   return hInstDLL ;

}/* khGethInst() */


BOOL FAR PASCAL khCheckVersion( DWORD dwVer )
{
   return (LOWORD( dwVer ) <= _KEYHK_SPEC_VER_ ) ;
}

DWORD FAR PASCAL khGetVersion( LPSTR lpszVersion ) 
{
   if ( VER_BUILD )
      wsprintf( lpszVersion, (LPSTR)"%d.%.2d.%.3d",
                VER_MAJOR, VER_MINOR, VER_BUILD ) ;
   else
      wsprintf( lpszVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;

   return dwVersion ;
}

/*
 * Installs the keyboard hook.  The application that calls this function
 * must pass in the window handle of the window it wants to recieve
 * the message indicating the ESC key was pressed
 */
BOOL FAR PASCAL khInstallKeyHook( HWND hWnd )
{
   short i, j = -1 ;
   BOOL  fRet = FALSE ;

   /*
    * Find first avail slot.
    */
   for ( i = 0 ; i < MAX_NUM_WINDOWS ; i++)
      if (rghWnd[i] == NULL)
      {
         j = i ;
         break ;
      }

   if (j == -1)
   {
      //DP1( hWDB, "No more instances allowed" ) ;
      return FALSE ;
   }

   DP1( hWDB, "Installing hook for instance %d", j ) ;
   /*
    * If the hook has not been made, make it
    */
   if (hHook == NULL)
   {
      if (fWin30)
      {
         DP( hWDB, "Calling SetWindowsHook()" ) ;
         hHook = SetWindowsHook( WH_KEYBOARD, (HOOKPROC)fnKeyboardHook ) ;
      }
      else
      {
         DP( hWDB, "Calling SetWindowsHookEx( WH_KEYBOARD, %08lX, %04X, %04X )",
                     (HOOKPROC)fnKeyboardHook, (HINSTANCE)GetWindowWord( hWnd, GWW_HINSTANCE ),
                     GetWindowTask( hWnd ) ) ;
         hHook = MySetWindowsHookEx( WH_KEYBOARD,
                     (HOOKPROC)fnKeyboardHook,
                     (HINSTANCE)GetWindowWord( hWnd, GWW_HINSTANCE ),
                     GetWindowTask( hWnd ) ) ;
      }

      if (hHook == NULL)
         return FALSE ;
   }

   rghWnd[j] = hWnd ;

   return fRet ;
}


BOOL FAR PASCAL khUnInstallKeyHook( HWND hWnd )
{
   short i ;
   BOOL  fRet = FALSE ;

   if (hHook == NULL)
      return FALSE ;

   /*
    * Find hWnd in list and kill it
    */
   for ( i = 0 ; i < MAX_NUM_WINDOWS ; i++)
      if (rghWnd[i] == hWnd)
      {
         rghWnd[i] = NULL ;
         fRet = TRUE ;
         break ;
      }

   /*
    * If we didn't find it fRet will be FALSE so return FALSE
    */
   if (!fRet)
      return FALSE ;

   /*
    * Go through list again to see if there are any non-zero entries.
    * if there are no non-zero entries we can nuke the hook 'cause there
    * are no more windows expecting messagesf from the hook.
    */
   for ( i = 0 ; i < MAX_NUM_WINDOWS ; i++)
      if (rghWnd[i] != NULL)
         return TRUE ;

   /*
    * Free hook
    */
   if (hHook!= NULL)
   {
      if (fWin30)
         UnhookWindowsHook( WH_KEYBOARD, (HOOKPROC)fnKeyboardHook ) ;
      else
      {
         MyUnhookWindowsHookEx( hHook ) ;
      }
    }

   return TRUE ;
}


LRESULT MyUnhookWindowsHookEx( HHOOK hHook )
{
   static BOOL (WINAPI *lpfnUWHE)(HHOOK) = NULL ;

   /*
    * UWHE() is exported from USER.EXE at ordinal # 292
    */
   if (lpfnUWHE == NULL && !fWin30)
      (FARPROC)lpfnUWHE = GetProcAddress( GetModuleHandle( "USER" ),
                                 (LPCSTR)MAKEINTRESOURCE( 292 ) ) ;

   if (lpfnUWHE != NULL)
      return (*lpfnUWHE)( hHook ) ;
   else
      return NULL ;
}


HHOOK MySetWindowsHookEx( int idHook, HOOKPROC lpfn, HINSTANCE hinst, HTASK htask )
{
   static BOOL (WINAPI *lpfnUWHE)(int, HOOKPROC, HINSTANCE, HTASK) = NULL ;

   /*
    * UWHE() is exported from USER.EXE at ordinal # 291
    */
   if (lpfnUWHE == NULL && !fWin30)
      (FARPROC)lpfnUWHE = GetProcAddress( GetModuleHandle( "USER" ),
                                 (LPCSTR)MAKEINTRESOURCE( 291 ) ) ;

   if (lpfnUWHE != NULL)
      return (*lpfnUWHE)( idHook, lpfn, hinst, htask ) ;
   else
      return NULL ;
}


HWND FAR PASCAL khGetLastWindow( VOID )
{
   DP1( hWDB, "khGetLastWindow = %04X", hWndLast ) ;
   return hWndLast ;
}


/****************************************************************
 *  UINT FAR PASCAL khSpawnApp( LPSTR lpszCmdLine, LPSTR lpszDir, UINT wShowFlags )
 *
 *  Description: 
 *
 *    This function does a synchronous spawn of another app.
 *    In otherwords, it WinExec()'s an application and does not
 *    return to the caller until that application has shut down.
 *
 *    If the spawn was succesful the return value is greater or
 *    equal to 32.  Otherwise it is a DOS exec() return code.
 *
 *  Comments:
 *
 ****************************************************************/
UINT FAR PASCAL khSpawnApp( LPSTR lpszCmdLine, LPSTR lpszDir, UINT wShowFlags )
{
   HINSTANCE   hResult;
   HWND        hwndNext;
   MSG         msg;
   char        szClass[80], szClass2[80] ;

   char szOldDir[144] ;

   DP1( hWDB, "khSpawnApp( %s, ... )", (LPSTR)lpszCmdLine ) ;

   /*
    * If a dir was specified change to it.
    */
   if (lpszDir && *lpszDir)
   {
      khGetCurrentDirectory( 0, szOldDir ) ;
      if (!khSetCurrentDirectory( lpszDir ))
      {
         /*
          * Bummer.  We'll just ignore this for now.
          */
      }
   }

   hResult = (HINSTANCE)WinExec( lpszCmdLine, wShowFlags);

   if((UINT)hResult < 32)
   {
      DP1( hWDB, "WinExec( %s, %d ) Failed!  Return code = %d",
                  (LPSTR)lpszCmdLine, wShowFlags, hResult ) ;
      return (UINT)hResult ;
   }

   /*
    * Go ahead and change back to the original dir.
    */
   if (lpszDir && *lpszDir )
   {
      khSetCurrentDirectory( szOldDir ) ;
   }

   /*
    * Get main window handle associated with hInstance in hwndNext
    */
   hwndNext = GetWindow(GetActiveWindow(), GW_HWNDFIRST);
   while(hwndNext)
   {
      if (!GetWindow(hwndNext, GW_OWNER))
      {
         if(hResult == (HINSTANCE)GetWindowWord(hwndNext, GWW_HINSTANCE))
            break;
      }
      hwndNext = GetWindow(hwndNext, GW_HWNDNEXT);
   }

   /*
    * in the loop below we check the window handle's validity using
    * IsWindow().  It is possible, however, that between the time
    * the app is killed and we call IsWindow again, that another
    * window could be created in the system with the same window
    * handle.  Thus we double check by getting the class names and
    * comparing them.
    */

   GetClassName( hwndNext, szClass, 80 ) ;
   GetClassName( hwndNext, szClass2, 80 ) ;

   /*
    * Wait until main window goes away
    */
   while(IsWindow(hwndNext) && !lstrcmpi( szClass, szClass2 ) &&
         ((HINSTANCE)GetWindowWord(hwndNext, GWW_HINSTANCE) == hResult))
   {
      while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }

      GetClassName( hwndNext, szClass2, 80 ) ;
   }

   DP1( hWDB, "Returning from khSpawnApp()" ) ;
   return (UINT)hResult ;

} /* khSpawnApp()  */


#define SelectDisk		   0Eh
#define ChangeCurrentDir	3Bh
#define GetCurrentDisk		19h
#define GetCurrentDir      47h


/****************************************************************
 *  UINT FAR PASCAL khSetCurrentDrive( UINT Drive )
 *
 *  Description: 
 *
 *    Changes drive.
 * 
 *    Returns the number of drives.
 *
 *  Comments:
 *
 ****************************************************************/
UINT FAR PASCAL khSetCurrentDrive( UINT Drive )
{
   UINT nDrives ;

   _asm
   {
	      mov     dx,Drive
	      mov     ah,SelectDisk
         call    DOS3Call
	      sub     ah,ah		; Zero out AH

         mov     nDrives, ax
   }

   return nDrives ;

} /* khSetCurrentDrive()  */


/****************************************************************
 *  BOOL FAR PASCAL khSetCurrentDirectory( LPSTR lpDirName )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL khSetCurrentDirectory( LPSTR lpDirName )
{
   BOOL f ;

   _asm
   {
	         lds     di,lpDirName	; DS:DI = lpDirName

	         ; Is there a drive designator?
	         cmp     byte ptr [di+1],':'
	         jne     SCDNoDrive		; Nope, continue
	         mov     al,byte ptr [di]	; Yup, change to that drive
	         sub     ah,ah
	         sub     al,'A'
	         push    ax
	         call    khSetCurrentDrive
	         inc     di			; Move past the drive letter
	         inc     di			;   and colon

	         ; Are we at the end of the string?
	         mov     al,byte ptr [di]	; Are we at the end of the string?
	         cbw
	         or	     ax,ax
	         jz	     SCDExit		; Yup, exit
      SCDNoDrive:
	         mov     dx,di
	         mov     ah,ChangeCurrentDir
            call    DOS3Call
	         jc	     SCDExit		; Skip on error
	         xor     ax,ax		; Return FALSE if successful
      SCDExit:
            mov     f,ax
   }

   return f ;

} /* khSetCurrentDirectory()  */


/****************************************************************
 *  UINT FAR PASCAL khGetCurrentDrive( VOID )
 *
 *  Description: 
 *
 *    Gets current drive (0='A', 1='B'...)
 *
 *  Comments:
 *
 ****************************************************************/
UINT FAR PASCAL khGetCurrentDrive( VOID )
{
   UINT n ;

   _asm
   {
	    mov     ah,GetCurrentDisk
       call    DOS3Call
	    sub     ah,ah		; Zero out AH
       mov     n,ax
   }

   return n ;
   
} /* khGetCurrentDrive()  */

/****************************************************************
 *  BOOL FAR PASCAL khGetCurrentDirectory( UINT wDrive, LPSTR lpDest )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL FAR PASCAL khGetCurrentDirectory( UINT wDrive, LPSTR lpDest )
{
   BOOL f ;

   _asm
   {
	         push    ds			; Preserve DS

	         mov     ax,wDrive
	         or	     al,al
	         jnz     GCDHaveDrive

	         call    khGetCurrentDrive

	         inc     al			; Convert to logical drive number

      GCDHaveDrive:
	         les     di,lpDest		; ES:DI = lpDest
	         push    es
	         pop     ds			; DS:DI = lpDest
	         cld

	         mov     dl,al		; DL = Logical Drive Number
	         add     al,'@'		; Convert to ASCII drive letter
	         stosb
	         mov     al,':'
	         stosb
	         mov     al,'\\'	; Start string with a backslash
	         stosb
	         mov     byte ptr es:[di],0	; Null terminate in case of error
	         mov     si,di		; DS:SI = lpDest[1]
	         mov     ah,GetCurrentDir
            call    DOS3Call
	         jc	     GCDExit		; Skip if error
	         xor     ax,ax		; Return FALSE if no error
      GCDExit:
	         pop     ds			; Restore DS

            mov     f, ax
   }

   return f ;

} /* khGetCurrentDirectory()  */


/************************************************************************
 * End of File: keyhk.c
 ************************************************************************/

