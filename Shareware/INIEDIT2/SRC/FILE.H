/************************************************************************
 *
 *     Project:  
 *
 *      Module:  file.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _FILE_H_
#define _FILE_H_

BOOL WINAPI AddSelectFile( LPSTR lpszFile, BOOL FAR *lpf ) ;
WORD WINAPI AddFile( HLIST hlstFiles, LPINIFILE lpINInew, BOOL FAR *pf ) ;
VOID WINAPI SortFiles( HLIST hlst ) ;
int CALLBACK fnFileCompare( LPINIFILE lp1, LPINIFILE lp2, LONG l ) ;
VOID WINAPI FileDoubleClick( WORD wID, LPINIFILE lpINI ) ;

#endif

/************************************************************************
 * End of File: file.h
 ***********************************************************************/

