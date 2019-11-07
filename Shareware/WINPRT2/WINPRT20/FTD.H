// ftd.h
//
#ifndef __FTD_H__
#define __FTD_H__  

HRESULT ftdLoad( LPCSTR szName, LPSTREAM lpStreamInit, REFIID riid, LPVOID FAR * ppvUnk ) ;
HRESULT ftdSave( LPSTREAM lpStreamSave, LPUNKNOWN pUnk ) ;
#endif // __FTD_H__

