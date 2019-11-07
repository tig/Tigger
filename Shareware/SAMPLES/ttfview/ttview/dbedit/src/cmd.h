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
#define IDM_FILE_NEW                   1
#define IDM_FILE_OPEN                  2   
#define IDM_FILE_SAVE                  3   
#define IDM_FILE_SAVEAS                4   

#define IDM_FILE_EXIT                  5

#define IDM_OPT                        6
#define IDM_OPT_FONT                   7
#define IDM_OPT_PREFERENCES            8
#define IDM_OPT_STAT                   9
#define IDM_HELP                       10
#define IDM_HELP_CONTENTS              11
#define IDM_HELP_SEARCH                12
#define IDM_HELP_ONHELP                13 
#define IDM_HELP_ABOUT                 14 
                                       
                                       
#define IDCHILD_STAT     512
#define IDCHILD_LISTBOX   513

LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

void WINAPI UpdateInfo( void ) ;

#endif

/************************************************************************
 * End of File: cmd.h
 ***********************************************************************/

