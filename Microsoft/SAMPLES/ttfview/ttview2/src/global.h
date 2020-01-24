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

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "..\res\ttview.str"
#include "res.h"
#include "version.h"
#include "config.h"
#include "init.h"
#include "wndmgr.h"
#include "menu.h"
#include "cmd.h"
#include "dlghelp.h"
#include "subdlgs.h"
#include "botstat.h"
#include "listview.h"

#define GRCS(X)   GetRCString(X,ghinstApp)

#define SZ_CLIENTCLASSNAME  "TTVIEW.CLIENT"

/*
 * Macros
 */
#define GALLOC(n) GlobalAllocPtr(GHND, n)
#define GFREE(lp) GlobalFreePtr(lp)

/*
 * Global Variables, defined in TTVIEW.c
 */
extern HWND        ghwndApp ;
extern HWND        ghwndStat ;
extern HWND        ghwndMDIClient ;
extern HWND        ghwndActiveMDIChild ;

extern HINSTANCE   ghinstApp ;

extern LPSTR       rglpsz[] ;

extern HFONT       ghfontCur ;
extern HFONT       ghfontSmall ;

extern BOOL        gfStatLine ;

extern GLOBALHANDLE ghDevNames ;
extern GLOBALHANDLE ghDevMode ;

typedef struct tagFACE
{
   ENUMLOGFONT  elf ;
//   char         szTTFFile[_MAX_PATH] ;

   HLIST        hlStyles ;     

} FACE, FAR *LPFACE ;

extern HLIST         ghlFaces ;     // list of all tt fonts on system

LRESULT CALLBACK fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT CALLBACK fnMDIClient( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT CALLBACK fnMDIChild( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;
int FAR PASCAL MessageLoop( HACCEL haccl ) ;


#endif

/************************************************************************
 * End of File: TTVIEW.h
 ***********************************************************************/

