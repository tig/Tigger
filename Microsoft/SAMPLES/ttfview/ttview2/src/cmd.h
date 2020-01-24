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
                                       
#define IDCHILD_STAT     512
#define IDCHILD_CLIENT   513

#define IDM_FILE                 0x5000          
#define IDM_FILE_PRINTSETUP      0x5001
#define IDM_FILE_PRINT           0x5002    
#define IDM_FILE_EXIT            0x5003     
#define IDM_OPT                  0x5004           
#define IDM_OPT_FONT             0x5005      
#define IDM_OPT_PREFERENCES      0x5006
#define IDM_OPT_STAT             0x5007      
#define IDM_WINDOW               0x5008        
#define IDM_WINDOW_TILE          0x5009
#define IDM_WINDOW_CASCADE       0x500a
#define IDM_WINDOW_ICONS         0x500b
#define IDM_WINDOW_CLOSEALL      0x500c
#define IDM_WINDOW_CHILD         0x500d
  
#define IDM_HELP                 0x4000          
#define IDM_HELP_CONTENTS        0x4001 
#define IDM_HELP_SEARCH          0x4002   
#define IDM_HELP_ONHELP          0x4003   
#define IDM_HELP_ABOUT           0x4004    


LRESULT WINAPI CmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT WINAPI SysCmdHandler( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

void WINAPI UpdateInfo( void ) ;

#endif

/************************************************************************
 * End of File: cmd.h
 ***********************************************************************/

