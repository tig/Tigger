/************************************************************************
 *
 *     Project:  
 *
 *      Module:  cmd.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _CMD_H_
#define _CMD_H_

/* NOTE!  Remember that the toolbar bitmaps use these ID's, and IDSs */
/* defined in toolbar.h */


#define IDM_FILE                       0
#define IDM_FILE_OPEN                  1
#define IDM_FILE_SAVE                  2
#define IDM_FILE_SAVEAS                3
#define IDM_FILE_EDIT                  4
#define IDM_FILE_PRINTSETUP            5
#define IDM_FILE_PRINT                 6
#define IDM_FILE_EXIT                  7
#define IDM_EDIT                       8
#define IDM_EDIT_UNDO                  9
#define IDM_EDIT_CUT                   10
#define IDM_EDIT_COPY                  11
#define IDM_EDIT_PASTE                 12
#define IDM_EDIT_DELETE                13
#define IDM_SEARCH                     14
#define IDM_SEARCH_FIND                15
#define IDM_SEARCH_FINDNEXT            16
#define IDM_SEARCH_REPLACE             17
#define IDM_OPT                        18
#define IDM_OPT_FONT                   19
#define IDM_OPT_EDITOR                 20
#define IDM_OPT_TOOLBAR                21
#define IDM_OPT_TOOLBARLABELS          22
#define IDM_OPT_STAT                   23
#define IDM_HELP                       24
#define IDM_HELP_CONTENTS              25 
#define IDM_HELP_SEARCH                26
#define IDM_HELP_ONHELP                27 
#define IDM_HELP_ABOUT                 28 


#define IDCHILD_STAT     512
#define IDCHILD_TOOLBAR  513

LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

void WINAPI UpdateInfo( void ) ;

#endif

/************************************************************************
 * End of File: cmd.h
 ***********************************************************************/

