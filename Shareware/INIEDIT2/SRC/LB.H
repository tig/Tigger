/************************************************************************
 *
 *     Project:  Owner Draw Listbox
 *
 *      Module:  lb.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#define MAX_DESCLEN  81

/************************************************************************
 * Data structures
 ***********************************************************************/
BOOL FAR PASCAL lbProcessCmd( HWND hWnd, WORD wCtlMsg ) ;

LONG FAR PASCAL lbOwnerDraw( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;

/* Indexes into the mondo bitmap */
#define BM_SECTION_NORMAL_O      0
#define BM_SECTION_NORMAL_C      1
#define BM_SECTION_HELP_O        2
#define BM_SECTION_HELP_C        3
#define BM_KEY_NORMAL            4
#define BM_KEY_HELP              5
#define BM_FILE_NORMAL_O         6	
#define BM_FILE_NORMAL_C         7
#define BM_FILE_HELP_O           8
#define BM_FILE_HELP_C           9
#define BM_RESERVED1             10
#define BM_RESERVED2     	      11
#define BM_RESERVED3    	      12

/************************************************************************
 * End of File: lb.h
 ***********************************************************************/

