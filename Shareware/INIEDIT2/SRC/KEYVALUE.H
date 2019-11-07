/************************************************************************
 *
 *     Project:  
 *
 *      Module:  keyvalue.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _KEYVALUE_H_
#define _KEYVALUE_H_

VOID WINAPI SortKeyValues( HLIST hlst ) ;
int CALLBACK fnKeyValueCompare( LPKEYVALUE lp1, LPKEYVALUE lp2, LONG l ) ;

BOOL WINAPI UpdateKeyValue( HWND hwnd ) ;

BOOL WINAPI ReadKeyValues( LPSECTION lpSect, BOOL fForce ) ;
WORD WINAPI ShowKeyValues( LPSECTION lpSect, WORD wID, BOOL fVis, BOOL fRead ) ;

#endif

/************************************************************************
 * End of File: keyvalue.h
 ***********************************************************************/

