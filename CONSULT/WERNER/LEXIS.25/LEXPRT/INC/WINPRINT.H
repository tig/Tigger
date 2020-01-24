/************************************************************************
 *
 *     Project:  
 *
 *      Module:  prtcore.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#define _WINDLL
#define STRICT
#include <windows.h>

#include <wdb.h>
#include "..\inc\version.h"

#ifndef DM_SPECVERSION
#include <print.h>
#endif

/*
 * ----------------------------------------------------------------
 * Externs
 * ----------------------------------------------------------------
 */
extern HMODULE    hmodDLL ;


LPSTR WINAPI GetWINPRINTVersion( LPSTR lpszVersion ) ;
HMODULE WINAPI GetWINPRINThInst( void ) ;



/************************************************************************
 * End of File: prtcore.h
 ***********************************************************************/

