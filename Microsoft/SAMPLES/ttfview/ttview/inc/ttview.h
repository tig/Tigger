/************************************************************************
 *
 *     Project:  
 *
 *      Module:  TTVIEW.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _TTVIEW_H_
#define _TTVIEW_H_

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <time.h>
#include <commdlg.h>
#include <wdb.h>
#include "..\inc\isz.h"
#include "..\inc\res.h"

#define MAX_PATH	      147
#define MAX_DRIVE       3	
#define MAX_DIR         144	
#define MAX_FNAME       9
#define MAX_EXT         4	

#define WMU_TOOLPALNOTIFY  (WM_USER + 0x1001)

#define SZ_CLIENTCLASSNAME  "TTVIEW.CLIENT"

/*
 * Macros
 */
#define GALLOC(n) GlobalAllocPtr(GHND, n)
#define GFREE(lp) GlobalFreePtr(lp)

/*
 * Global Variables, defined in TTVIEW.c
 */
extern HWND        hwndApp ;
extern HWND        hwndClient ;
extern HWND        hwndStat ;

extern HINSTANCE   hinstApp ;

extern char        szVerNum[] ;

extern LPSTR       rglpsz[] ;

/*
 * Globally managed lists 
 */
extern HFONT       hfontCur ;
extern char        szFaceName[LF_FACESIZE] ;
extern char        szFaceStyle[LF_FACESIZE] ;



extern HFONT       hfontSmall ;
extern char        szFont[] ;
extern WORD        wFontStyle ;
extern WORD        wFontSize ;

extern BOOL        fStatLine ;
extern short       xPos, yPos, nWidth, nHeight ;

extern GLOBALHANDLE ghDevNames ;
extern GLOBALHANDLE ghDevMode ;

LRESULT FAR PASCAL fnMainWnd( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT FAR PASCAL fnClientWnd( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;
int FAR PASCAL MessageLoop( HACCEL haccl ) ;


#endif

/************************************************************************
 * End of File: TTVIEW.h
 ***********************************************************************/

