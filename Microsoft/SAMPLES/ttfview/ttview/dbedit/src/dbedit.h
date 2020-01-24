/************************************************************************
 *
 *     Project:  
 *
 *      Module:  DBEDIT.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _DBEDIT_H_
#define _DBEDIT_H_

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <time.h>
#include <commdlg.h>
#include <wdb.h>
#include <hlsqstr.h>
#include "isz.h"
#include "res.h"
#include "..\..\inc\shoplib.h"
#include "..\..\inc\db.h"

#define MAX_PATH              147
#define MAX_DRIVE       3       
#define MAX_DIR         144     
#define MAX_FNAME       9
#define MAX_EXT         4       
#define MAX_PUBLISHER         80
#define DEFAULT_HEAPSIZE      (UINT)(2*1024)

#define WMU_CURITEMCHANGE  (WM_USER + 0x1000)

#define SZ_lbCLASSNAME  "LISTBOX"


/*
 * Macros
 */

/*
 * Typedefs.  All item structures (INIFILE, SECTION, and KEYVALUE)
 * have the same 10 bytes defined by tagNODEHEADER.
 */
typedef struct tagNODEHEADER
{
   DWORD          dwSize ;                // struct size
   UINT           wMagic ;                // Identifies the struct type
   UINT           wFlags ;                // Flags for this item.

} NODEHEADER, *PNODEHEADER, FAR *LPNODEHEADER ;

#define CASTNODEHEADER(lp) ((LPNODEHEADER)lp)

#define NODEHEADER_dwSize(lp)         (CASTNODEHEADER(lp)->dwSize)
#define NODEHEADER_wMagic(lp)         (CASTNODEHEADER(lp)->wMagic)
#define NODEHEADER_wFlags(lp)         (CASTNODEHEADER(lp)->wFlags)


/*
 * Structure for font items.
 *
 *    wMagic for FONTFILE structs is FONTFILE_TAG
 */

#define FONTFILE_TAG  0x0001


typedef struct tagFONTFILE
{
   DWORD          dwSize ;                // struct size
   UINT           wMagic ;                // Identifies the struct type
   UINT           wFlags ;                // Flags for this item.

   char           lpszFile[MAX_PATH] ;    // fully qualified pathname to db file
   UINT           wOffsetFile ;           // offset to filename.ext
   time_t         tTimeDate ;             // Time of last write to file
   DWORD          dwFileSize ;            // Length of file in bytes

   LPSTR          lpszOutBuf ;            // current output buffer.
                                          // allocated from hhOutBuf
   
   HBITMAP        hbmThumb ;              // Current thumbnail

   FAMILY         Family ;                


} FONTFILE, *PFONTFILE, FAR *LPFONTFILE ;

#define CASTFONTFILE(lp) ((LPFONTFILE)lp)

#define FONTFILE_lpszFile(lp)        (CASTFONTFILE(lp)->lpszFile)
#define FONTFILE_wOffsetFile(lp)     (CASTFONTFILE(lp)->wOffsetFile)
#define FONTFILE_tTimeDate(lp)       (CASTFONTFILE(lp)->tTimeDate)
#define FONTFILE_dwFileSize(lp)      (CASTFONTFILE(lp)->dwFileSize)
#define FONTFILE_lpszOutBuf(lp)      (CASTFONTFILE(lp)->lpszOutBuf)
#define FONTFILE_hbmThumb(lp)        (CASTFONTFILE(lp)->hbmThumb)

#define FONTFILE_lpszFileName(lp)    (FONTFILE_lpszFile(lp)+FONTFILE_wOffsetFile(lp))

#define FONTFILE_dwFamilyPrice(lp)  (CASTFONTFILE(lp)->Family.dwFamilyPrice)
#define FONTFILE_wFonts(lp)         (CASTFONTFILE(lp)->Family.wFonts)
#define FONTFILE_dwFamilySize(lp)   (CASTFONTFILE(lp)->Family.dwFamilySize)
#define FONTFILE_szFamilyName(lp)   (CASTFONTFILE(lp)->Family.szFamilyName)
#define FONTFILE_dwFontFile(lp)     (CASTFONTFILE(lp)->Family.dwFontFile)
#define FONTFILE_wFlags(lp)         (CASTFONTFILE(lp)->Family.wFlags)
#define FONTFILE_dwReserved(lp)     (CASTFONTFILE(lp)->Family.dwReserved)


/*
 * Macros
 */
#define ALLOCFROMHEAP(h,n) hlsqHeapAllocLock(h,LPTR,n)
#define FREEFROMHEAP(h, lp) hlsqHeapUnlockFree(h,(LPSTR)lp)

#define GALLOC(n) GlobalAllocPtr(GHND, n)
#define GFREE(lp) GlobalFreePtr(lp)

/*
 * Global Variables, defined in DBEDIT.c
 */
extern HWND        hwndApp ;
extern HWND        hwndLB ;
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
extern UINT        wFontStyle ;
extern UINT        wFontSize ;

extern BOOL        fStatLine ;
extern short       xPos, yPos, nWidth, nHeight ;

extern HLIST       hlstFiles ;

extern HHEAP       hhOutBuf ;    // output buffers
extern HHEAP       hhLists ;     // all lists
extern HHEAP       hhStrings ;   // all strings

extern HHEAP       hhScratch ;    // scratch heap

/*
 * Semaphores
 */
extern int         semCurFile ;


LRESULT FAR PASCAL fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
LRESULT FAR PASCAL fnlbWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;
extern SORTPROC    lpfnFileCompare ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;
int FAR PASCAL MessageLoop( HACCEL haccl ) ;


#endif

/************************************************************************
 * End of File: DBEDIT.h
 ***********************************************************************/

