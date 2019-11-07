/************************************************************************
 *
 *     Project:  
 *
 *      Module:  FILEIO.H
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _FILEIO_H_
#define _FILEIO_H_

HLIST WINAPI GetDirectoryList( HLIST hlstFiles, LPSTR lpszDir, LPSTR lpszSpec ) ;
BOOL WINAPI ChangeDriveAndDir( LPSTR lpszNew, LPSTR lpszOld ) ;
BOOL WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, WORD cbMax, LPSTR lpszFile, WORD wFileSize ) ;

#endif

/************************************************************************
 * End of File: FILEIO.H
 ***********************************************************************/

