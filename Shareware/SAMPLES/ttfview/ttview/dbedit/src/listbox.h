/************************************************************************
 *
 *     Project:  DBEDIT 2.0
 *
 *      Module:  listbox.h
 *
 *     Remarks:  Handles the listbox child window.
 ***********************************************************************/
#ifndef _LISTBOX_H_
#define _LISTBOX_H_





HWND WINAPI CreateLB( HWND hwndParent ) ;
VOID WINAPI DestroyLB( void ) ;

VOID WINAPI ResetLBValues( VOID ) ;
VOID WINAPI ResetLBColors( VOID ) ;
VOID WINAPI SetLBRedraw( BOOL f ) ;

BOOL WINAPI lbProcessCmd( HWND hLB, WORD wCtlMsg ) ;
LRESULT WINAPI
   lbOwnerDraw( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

#endif

/************************************************************************
 * End of File: listbox.h
 ***********************************************************************/


