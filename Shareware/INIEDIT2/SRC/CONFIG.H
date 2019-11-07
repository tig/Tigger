/************************************************************************
 *
 *     Project:  
 *
 *      Module:  config.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

VOID FAR PASCAL SaveDefaults( VOID ) ;
VOID WINAPI SaveOptions( VOID ) ;
VOID FAR PASCAL RestoreDefaults( VOID ) ;

VOID FAR PASCAL RestoreWndPos( HWND hWnd, WORD nCmdShow ) ;
VOID FAR PASCAL SaveWndPos( HWND hWnd ) ;
VOID WINAPI SaveOtherFile( LPSTR lpszFile ) ;
VOID WINAPI KillOtherFile( LPSTR lpszFile ) ;

DWORD FAR PASCAL GetProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dwDef ) ;
WORD FAR PASCAL GetProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD wDef ) ;

VOID FAR PASCAL WriteProfileDWORD( LPSTR lpszSec, LPSTR lpszKey, DWORD dw ) ;
VOID FAR PASCAL WriteProfileWORD( LPSTR lpszSec, LPSTR lpszKey, WORD w ) ;

#endif

/************************************************************************
 * End of File: config.h
 ***********************************************************************/


