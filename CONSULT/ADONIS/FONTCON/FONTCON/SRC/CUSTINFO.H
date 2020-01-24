
#ifndef _CUSTINFO_H_
#define _CUSTINFO_H_

extern CUSTINFO   gCustInfo ;

BOOL WINAPI GetCustInfo( LPCUSTINFO lpCI, LPSTR lpszINIFile ) ;

BOOL WINAPI SavePassword( LPCUSTINFO lpCI, LPSTR lpszNewPassword ) ;
BOOL WINAPI SaveUserID( LPCUSTINFO lpCI, LPSTR lpszNewUserID ) ;

#endif
