/************************************************************************
 *
 *     Project:  
 *
 *      Module:  XEDIT.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _XEDIT_H_
#define _XEDIT_H_

#define STRICT
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <commdlg.h>
#include <wdb.h>
#include <3dutil.h>
#include <hlsqstr.h>
#include "..\inc\isz.h"
#include "..\inc\res.h"

#define MAX_REGNAME_LEN    64
#define MAX_REGNUM_LEN     8

#define MAX_PATH	      147
#define MAX_DRIVE       3	
#define MAX_DIR         144	
#define MAX_FNAME       9
#define MAX_EXT         4	

#define WMU_TOOLPALNOTIFY  (WM_USER + 0x1001)


/*
 * Macros
 */
#define ALLOCFROMHEAP(h,n) hlsqHeapAllocLock(h,LPTR,n)
#define FREEFROMHEAP(h, lp) hlsqHeapUnlockFree(h,(LPSTR)lp)

#define GALLOC(n) GlobalAllocLock(n)
LPSTR WINAPI GlobalAllocLock( DWORD cb ) ;
#define GFREE(lp) GlobalUnlockFree(lp)
VOID WINAPI GlobalUnlockFree( LPSTR lp ) ;

/*
 * Global Variables, defined in XEDIT.c
 */
extern HWND        hwndApp ;
extern HWND        hwndTool ;
extern HWND        hwndStat ;
extern HWND        hwndHex ;

extern HINSTANCE   hinstApp ;

extern BOOL        fWin31 ;

extern char        szVerNum[] ;
extern char        szRegisteredName[] ;

extern LPSTR       rglpsz[] ;

extern BOOL       fDirty ;

/*
 * Globally managed lists 
 */
extern HHEAP       hhScratch ;    // scratch heap

extern HFONT       hfontSmall ;
extern HFONT       hfontHex ;
extern char        szFont[] ;
extern WORD        wFontStyle ;
extern WORD        wFontSize ;

extern BOOL        fToolBar ;
extern BOOL        fToolBarLabels ;
extern BOOL        fStatLine ;
extern short       xPos, yPos, nWidth, nHeight ;

extern char          szCurFile[], szCurFileTitle[] ;

/*
 * Find/Replace dialog box stuff
 */
extern FINDREPLACE fr ;
extern HWND        hdlgFindReplace ;
extern WORD        wFindReplaceMsg ;
extern char        szFindWhat[] ;
extern char        szReplaceWith[] ;

extern GLOBALHANDLE ghDevNames ;
extern GLOBALHANDLE ghDevMode ;

LRESULT FAR PASCAL fnMainWnd( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;
LPVOID WINAPI AllocBytesFromHeap( HHEAP hh, WORD nBytes ) ;
int FAR PASCAL MessageLoop( HACCEL haccl ) ;


#endif

/************************************************************************
 * End of File: XEDIT.h
 ***********************************************************************/

