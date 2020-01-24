/************************************************************************
 *
 *     Project:  Windows Font Shopper Support Library
 *
 *               Internal defines.  See CONXLIB.H for publics.
 *
 *    3/1/92   C.Kindel    Wrote it.
 *
 ***********************************************************************/

#ifndef _CONXLIBI_H_
#define _CONXLIBI_H_

#include "PRECOMP.H"

#include "..\..\inc\version.h"
#include "..\..\inc\conxmem.h"

#include "struct.h"
#include "CONXLIB.h"
#include "dlgs.h"
#include "isz.h"

#define GRCS(X) GetRCString(X,hmodDLL)

/*
 * ----------------------------------------------------------------
 * Externs
 * ----------------------------------------------------------------
 */
extern HMODULE    hmodDLL ;

extern LPSTR       rglpsz[LAST_IDS-FIRST_IDS+1] ;

BOOL WINAPI StatusInit( HINSTANCE hPrev, HINSTANCE hInstance, LPSTR ) ;

BOOL WINAPI LocalLEDInit( HANDLE hPrev, HANDLE hInst )  ;
          

/************************************************************************
 *   wsscanf/wstdarg
 ************************************************************************/

typedef char _far *wva_list ;

#define wva_start( ap, v )  (ap = (wva_list) &v + sizeof( v ))
#define wva_arg( ap, t )    (((t _far *)(ap += sizeof( t )))[-1])
#define wva_end( ap )       (ap = NULL)

char NEAR HexToDec( char ) ;
char NEAR Inc( LPSTR FAR * ) ;
VOID NEAR UnInc( char, LPSTR FAR * ) ;
char NEAR WhiteOut( int FAR *, LPSTR FAR * ) ;

/*************************************************************************
 * Dialog box stuff
 ************************************************************************/

BOOL WINAPI DoSetupDialog( HWND hwndParent, LPSTR szHelpFile, UINT uiDlgID, LPSTR lpszINIFile, DWORD dwData2 ) ;

typedef struct tagDLGDATA
{
   UINT     uiExitBtnID ;
   LPSTR    lpszINIFile ;
   DWORD    dwData2 ;
   BOOL     fIndicatorPaint ;
   HFONT    hfontSmall ;

   LPSTR    lpszHelpFile ;

   CREDITCARD  CC[NUMBER_OF_CARDS] ;

} DLGDATA, *PDLGDATA, FAR *LPDLGDATA ;

#define WM_INDICATORPAINT (WM_USER+0x100)
VOID WINAPI UpdateIndicators( HWND hDlg ) ;
VOID WINAPI PaintIndicator( HDC hDC, HWND hwndCtl ) ;
VOID WINAPI ClearIndicator( HDC hDC, HWND hwndCtl ) ;


#define ATOM_DLGDATA_H       0x100     /* HIWORD */
#define ATOM_DLGDATA_L       0x101     /* LOWORD */

#define SET_DLGDATAPROP(hwnd,lp) {\
                                    SetProp( hwnd,\
                                             MAKEINTATOM( ATOM_DLGDATA_H ),\
                                             (HANDLE)HIWORD((DWORD)lp) );\
                                    SetProp( hwnd,\
                                             MAKEINTATOM( ATOM_DLGDATA_L ),\
                                             (HANDLE)LOWORD((DWORD)lp) );\
                                 }

#define GET_DLGDATAPROP(hwnd)    (LPDLGDATA)(MAKELONG( GetProp( hwnd,\
                                           MAKEINTATOM( ATOM_DLGDATA_L ) ),\
                                           GetProp( hwnd,\
                                           MAKEINTATOM( ATOM_DLGDATA_H ) ) ))\

#define REMOVE_DLGDATAPROP(hwnd) {\
                                    RemoveProp( hwnd,\
                                          MAKEINTATOM( ATOM_DLGDATA_H ) ) ;\
                                    RemoveProp( hwnd,\
                                          MAKEINTATOM( ATOM_DLGDATA_L ) ) ;\
                                 }

BOOL CALLBACK fnCustomerInfo( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL CALLBACK fnShippingInfo( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL CALLBACK fnBillingInfo ( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
BOOL CALLBACK fnCommSetup   ( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

BOOL WINAPI GetCreditCard( LPCREDITCARD lpCC, LPSTR lpszINIFile, UINT uiWhichCard ) ;
VOID WINAPI SaveCreditCards( CREDITCARD FAR rgCC[], LPSTR lpszINIFile ) ;

/*** SLIDER  ****/

BOOL WINAPI SliderInit( HINSTANCE hPrev, HINSTANCE hInstance ) ;

#define SPLITCURSOR  1


/*** TOOLBAR ****/

BOOL WINAPI RegisterToolbarClass( HINSTANCE hInstance ) ;

#endif

/************************************************************************
 * End of File: prtcore.h
 ***********************************************************************/

