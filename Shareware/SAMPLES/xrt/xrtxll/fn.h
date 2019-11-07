// fn.h

#ifndef _FN_H_
#define _FN_H_

extern HINSTANCE    g_hinst ;
extern HWND         g_hwndXL ;
extern HINSTANCE    g_hinstXL ;

extern LPDATAOBJECT g_pDataObject ;
extern LPADVISESINK g_pAdviseSink ;
extern UINT         g_xrtCF_XRTSTOCKS  ;
extern DWORD        g_dwUpdateCount  ;

HRESULT InitXRTObject(  LPTSTR lpszObjName ) ;
HRESULT UnInitXRTObject() ;
HRESULT GotData( LPFORMATETC lpfetc, LPSTGMEDIUM lpstm ) ;

extern "C"
void EXPORT FAR PASCAL XRTOnData( ) ;

extern "C"
LPXLOPER EXPORT FAR PASCAL XRTSetRange( LPXLOPER pxVal ) ;

extern "C"
LPXLOPER EXPORT FAR PASCAL XRTQuote( LPXLOPER pxTicker, LPXLOPER pxVal ) ;

extern "C"
LPXLOPER EXPORT FAR PASCAL XRTItemName( LPXLOPER pxTicker ) ;

#endif // _FN_H_

