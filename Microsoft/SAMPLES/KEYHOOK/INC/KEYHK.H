/************************************************************************
 *
 *     Project:  Key hook DLL for State Farm VB project
 *
 *      Module:  3dutil.h
 *
 *     Remarks:  Contains macros, prototypes, and external declarations
 *               for KEYHK.DLL.
 *
 *               All KEYHK APIs are prefixed with 'kh' (Key Hook).
 *
 *   Revisions:  
 *
 ***********************************************************************/

#ifndef _KEYHK_H_
#define _KEYHK_H_

/*
 * The SPEC version can be used by an application to determine if the
 * installed version of this DLL is old or new.  The app should call
 * khCheckVersion( _KEYHK_SPEC_VER_ ).  If this function returns
 * FALSE then the DLL installed is old and should not be called.
 */
#define _KEYHK_SPEC_VER_   (WORD)100

BOOL FAR PASCAL khCheckVersion( DWORD dwVer ) ;
DWORD FAR PASCAL khGetVersion( LPSTR lpszVersion )  ;
HANDLE FAR PASCAL khGethInst( VOID ) ;

BOOL FAR PASCAL khInstallKeyHook( HWND hWnd ) ;
BOOL FAR PASCAL khUnInstallKeyHook( HWND hWnd ) ;

/*
 * Spawn stuff
 */
UINT FAR PASCAL khSpawnApp( LPSTR lpszCmdLine, LPSTR lpszDir, UINT wShowFlags ) ;
UINT FAR PASCAL khSetCurrentDrive( UINT Drive ) ;
BOOL FAR PASCAL khSetCurrentDirectory( LPSTR lpDirName ) ;
UINT FAR PASCAL khGetCurrentDrive( VOID ) ;
BOOL FAR PASCAL khGetCurrentDirectory( UINT wDrive, LPSTR lpDest ) ;

/*
 * Defines some random WM_USER + value for our post message
 */
#define WMU_ESCHIT   (WM_USER + 0x0743)

DWORD CALLBACK PASCAL fnKeyboardHook( int nCode, WORD wParam, DWORD dwParam ) ;

#define MAX_NUM_WINDOWS    32 

#endif // _KEYHK_H_

/************************************************************************
 * End of File: 3dutil.h
 ***********************************************************************/

