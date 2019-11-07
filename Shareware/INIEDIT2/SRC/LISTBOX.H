/************************************************************************
 *
 *     Project:  INIEDIT 2.0
 *
 *      Module:  listbox.h
 *
 *     Remarks:  Handles the listbox child window.
 ***********************************************************************/
#ifndef _LISTBOX_H_
#define _LISTBOX_H_


#define NUMICONS           6                              

#define ICON_INIFILE       0
#define ICON_DOSFILE       1
#define ICON_SECTION       2
#define ICON_FILEFOLDER    3
#define ICON_KEY           4
#define ICON_PAPER         5


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

