/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  cmd.h
 *
 ***********************************************************************/
#ifndef _CMD_H_
#define _CMD_H_

/* NOTE!  Remember that the toolbar bitmaps use these ID's, and IDSs */
/* defined in toolbar.h */


#define IDM_FILE                       0
#define IDM_RESETDDE                   1
#define IDM_OPT_FONT                   2
#define IDM_FILE_EXIT                  3

#define IDM_HELP                       4
#define IDM_HELP_CONTENTS              5
#define IDM_HELP_SEARCH                6
#define IDM_HELP_ONHELP                7 
#define IDM_HELP_ABOUT                 8 

#define IDM_OPT                        9
#define IDM_CONNECT                    10
#define IDM_DISCONNECT                 11
                                       
#define IDCHILD_STAT     512
#define IDCHILD_TTY      513

LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

void WINAPI UpdateInfo( void ) ;

#endif

/************************************************************************
 * End of File: cmd.h
 ***********************************************************************/

