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

BOOL WINAPI DoPrinterSetup( void ) ;
BOOL WINAPI DoChooseFont( void ) ;

void FAR PASCAL GetDevNamesInfo( GLOBALHANDLE ghDevNames, LPSTR szDriver, LPSTR szDevice, LPSTR szOutput ) ;

BOOL FAR PASCAL DoAboutBox( VOID ) ;

#endif 

/************************************************************************
 * End of File: subdlgs.h
 ***********************************************************************/

