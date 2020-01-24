#ifndef ERRORH
#define ERRORH

#include <stdarg.h>

int far PASCAL ClrErr(void);
int far PASCAL InqErr(void);
void far PASCAL InqDest(HANDLE *hParent, BOOL *bBeep, int *nFlg, int *nCptn, char *szCptn);
int far SetErr(int nCtrlStr,...);
void far SetDest(HANDLE hParent, BOOL bBeep, int nFlg, int nCptn,...);
int far PASCAL IssueErr(void);
void far PASCAL SetDefErr(void);
int FAR ErrMsg(int nCtrlStr,...);
int FAR _cdecl WSvsprintf(char FAR *, const char FAR *, va_list);

#define BEEP TRUE
#define NOBEEP FALSE
#define ES_ACTIVEWND  7000    /* Use GetActiveWnd() */
#define ES_LOWMEMMSG  7001    /* Use static strings in error.c (instead of */
#define ES_LOWMEMCPT  7002    /* resource file) for low memory conditions */
#define ES_DEFCPT     7003
#define ES_MAINWND    hWndWS
#define ES_DEFFLAGS   MB_OK | MB_APPLMODAL
#define ES_LOWMEMFLAGS MB_OK | MB_ICONHAND | MB_APPLMODAL
#define ES_LOWMEMSETDEST() SetDest(ES_ACTIVEWND, BEEP, ES_LOWMEMFLAGS, ES_LOWMEMCPT)
/* NEED TO RENUMBER THESE GUYS */

#define  ID_ERR_FATAL_IO              1
#define  ID_ERR_OPENING_FILE		  2
#define  ID_ERR_CREATING_FILE		  3
#define  ID_ERR_UPDATING_FILE         4
#define  ID_ERR_READING_FILE		  5
#define  ID_ERR_ENTRY_NOT_FOUND	      6
#define  ID_ERR_LOADING_RES_LIB		  7
#define  ID_ERR_INVALID_LIBRARY 	  8
#define  ID_ERR_LOAD_FAILED           9
#define  ID_ERR_TOO_MANY              12
#define  ID_ERR_DECOMPRESSING         13
#define  ID_ERR_DELETING_FILE         14
#define  ID_ERR_MOVING_FILE           15
#define  ID_ERR_MISSING_LRU           16
#define  ID_ERR_ON_SHIPPING_COSTS     17
#define  ID_ERR_IN_SEARCH_FILE        18
                                      
#endif                                  
                                        
