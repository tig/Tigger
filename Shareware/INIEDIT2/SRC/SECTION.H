/************************************************************************
 *
 *     Project:  
 *
 *      Module:  section.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _SECTION_H_
#define _SECTION_H_

VOID WINAPI SortSections( HLIST hlst ) ;
int CALLBACK fnSectionCompare( LPSECTION lp1, LPSECTION lp2, LONG l ) ;

BOOL WINAPI UpdateSection( HWND hwnd ) ;
VOID WINAPI SectionDoubleClick( WORD wID, LPSECTION lpSection ) ;

WORD WINAPI ShowSections( LPINIFILE lpINI, WORD wID, BOOL fVis, BOOL fRead ) ;
BOOL WINAPI ReadSections( LPINIFILE lpINI, BOOL fForce ) ;

BOOL WINAPI DestroySection( LPSECTION lpSec ) ;

#endif

/************************************************************************
 * End of File: section.h
 ***********************************************************************/

