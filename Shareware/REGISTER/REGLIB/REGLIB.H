/************************************************************************
 *
 *     Project:  3D effect DLL
 *
 *      Module:  REGLIB.h
 *
 *     Remarks:  Contains macros, prototypes, and external declarations
 *               for REGLIB.DLL.
 *
 *               All REGLIB APIs are prefixed with 'rl' (Three Dee).
 *
 *   Revisions:  
 *
 ***********************************************************************/

#ifndef _REGLIB_H_
#define _REGLIB_H_

/*
 * The SPEC version can be used by an application to determine if the
 * installed version of this DLL is old or new.  The app should call
 * rlCheckVersion( _REGLIB_SPEC_VER_ ).  If this function returns
 * FALSE then the DLL installed is old and should not be called.
 */
#define _REGLIB_SPEC_VER_   (WORD)100

#define REG_MAX_NAME    128
#define REG_MAX_NUMBER  8     // Keep it short.

BOOL FAR PASCAL rlCheckVersion( DWORD dwVer ) ;
DWORD FAR PASCAL rlGetVersion( LPSTR lpszVersion )  ;
HANDLE FAR PASCAL rlGethInst( VOID ) ;

VOID FAR PASCAL rlGetRegNumber( LPSTR lpszApp, LPSTR lpszName, LPSTR lpszNum ) ;

#endif // _REGLIB_H_

/************************************************************************
 * End of File: REGLIB.h
 ***********************************************************************/

