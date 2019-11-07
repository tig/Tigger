/************************************************************************
 *
 *     Project:  3D effect DLL
 *
 *      Module:  ITDUTIL.H
 *
 *     Remarks:   Internal
 *
 *   Revisions:  
 *
 ***********************************************************************/

#ifndef _ITDUTIL_H_
#define _ITDUTIL_H_

#include "td.h"
#include "dlgs.h"

#define          CHILD_UNKNOWN         0
#define          CHILD_BUTTON          1
#define          CHILD_COMBO           2
#define          CHILD_DROPDOWNCOMBO   3
#define          CHILD_EDIT            4
#define          CHILD_LISTBOX         5
#define          CHILD_SCROLLBAR       6
#define          CHILD_TEXT            7
#define          CHILD_BLACKFRAME      8
#define          CHILD_BLACKRECT       9
#define          CHILD_GRAYFRAME       10
#define          CHILD_GRAYRECT        11
#define          CHILD_ICON            12
#define          CHILD_WHITEFRAME      13
#define          CHILD_WHITERECT       14
#define          CHILD_GROUPBOX        15
#define          CHILD_CHECKBOX        16
#define          CHILD_RADIOBUTTON     17


#define ATOM_RGBBG_H       0x100     /* HIWORD for COLORREF */
#define ATOM_RGBBG_L       0x101     /* LOWORD for COLORREF */
#define ATOM_HBRBG         0x102     /* hBrushBG */

#define SET_RGBBG(hwnd,rgb) {\
                            SetProp( hwnd, MAKEINTATOM( ATOM_RGBBG_H ),\
                                     (HANDLE)HIWORD((DWORD)rgb) );\
                            SetProp( hwnd, MAKEINTATOM( ATOM_RGBBG_L ),\
                                     (HANDLE)LOWORD((DWORD)rgb) );\
                           }

#define GET_RGBBG(hwnd)    (COLORREF)(MAKELONG( GetProp( hwnd,\
                                          MAKEINTATOM( ATOM_RGBBG_L ) ),\
                                                GetProp( hwnd,\
                                          MAKEINTATOM( ATOM_RGBBG_H ) ) ))
                           

#define REMOVE_RGBBG(hwnd) {\
                            RemoveProp( hwnd, MAKEINTATOM( ATOM_RGBBG_H ) ) ;\
                            RemoveProp( hwnd, MAKEINTATOM( ATOM_RGBBG_L ) ) ;\
                           }

#define SET_HBRBG(hwnd,hbr) SetProp( hwnd, MAKEINTATOM( ATOM_HBRBG ), (HANDLE)hbr)

#define GET_HBRBG(hwnd)    (HBRUSH)GetProp( hwnd, MAKEINTATOM( ATOM_HBRBG ) )

#define REMOVE_HBRBG(hwnd) (HBRUSH)RemoveProp( hwnd, MAKEINTATOM( ATOM_HBRBG ) ) 


/* Internal functions */

LRESULT CALLBACK fnNewButton( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK fn3DStatic( HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;

VOID NEAR PASCAL Draw3DLine1( HDC hDC, UINT x, UINT y, UINT nLen, UINT wFlags ) ;
BOOL NEAR PASCAL HackButtonsAndFrames( HWND hWnd ) ;
UINT NEAR PASCAL ChildKind( HWND hwndChild ) ;
VOID NEAR PASCAL DrawFrame( HDC hDC, LPRECT lprc ) ;


VOID WINAPI SetupColorTable( HDC hdc ) ;

#endif // _ITDUTIL_H_

/************************************************************************
 * End of File: ITDUTIL.H
 ***********************************************************************/

