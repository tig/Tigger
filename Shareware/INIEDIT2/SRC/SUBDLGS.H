/************************************************************************
 *
 *     Project:  
 *
 *      Module:  subdlgs.h
 *
 *
 ***********************************************************************/
#ifndef _SUBDLGS_H_
#define _SUBDLGS_H_

BOOL WINAPI DoFileOpen( LPSTR lpszFileName, LPSTR lpszFileTitle ) ;
BOOL WINAPI DoPrinterSetup( void ) ;
BOOL WINAPI DoChooseFont( void ) ;

BOOL WINAPI DoEditor( void ) ;

void FAR PASCAL GetDevNamesInfo( GLOBALHANDLE ghDevNames, LPSTR szDriver, LPSTR szDevice, LPSTR szOutput ) ;

#endif 

/************************************************************************
 * End of File: subdlgs.h
 ***********************************************************************/

