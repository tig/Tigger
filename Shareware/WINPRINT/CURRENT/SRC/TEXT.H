/************************************************************************
 *
 *     Project:  
 *
 *      Module:  text.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _TEXT_H_
#define _TEXT_H_

BOOL FAR PASCAL DoTextOptions( VOID ) ;

PSTR FAR PASCAL DtoInch (PSTR szInch, double dValue) ;
PSTR FAR PASCAL DtoCM (PSTR szCM, double dValue) ;
double FAR PASCAL GetDlgValue (HWND hDlg, UINT iID, BOOL bInches) ;

#endif

/************************************************************************
 * End of File: text.h
 ***********************************************************************/

