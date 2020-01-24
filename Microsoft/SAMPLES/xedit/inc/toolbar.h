/************************************************************************
 *
 *      Module:  toolbar.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/
#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

HWND WINAPI CreateToolBar( HWND hwndParent ) ;
HWND WINAPI ResetToolBar( HWND hwndTool ) ;

#define  SZ_TOOLBARCLASSNAME  "INI_toolbar"
LRESULT CALLBACK fnToolBar( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
#endif

/************************************************************************
 * End of File: toolbar.h
 ************************************************************************/

