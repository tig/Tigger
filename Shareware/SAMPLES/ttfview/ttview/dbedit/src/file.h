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
WORD WINAPI AddFile( HLIST hlstFiles, LPFONTFILE lpFONTnew, BOOL FAR *pf ) ;
VOID WINAPI SortFiles( HLIST hlst ) ;
int CALLBACK fnFileCompare( LPFONTFILE lp1, LPFONTFILE lp2, LONG l ) ;
VOID WINAPI FileDoubleClick( WORD wID, LPFONTFILE lpFONT ) ;

#endif

/************************************************************************
 * End of File: file.h
 ***********************************************************************/

