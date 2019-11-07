/************************************************************************
 *
 *     Project:  
 *
 *      Module:  iniedit.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#ifndef _INIEDIT_H_
#define _INIEDIT_H_

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <time.h>
#include <commdlg.h>
#include <wdb.h>
#include <3dutil.h>
#include <hlsqstr.h>
#include "isz.h"
#include "res.h"
#include "dlgs.h"
//#include "..\help\map.h"

#include <string.h>

#define MAX_INI_LEN        64                       
#define MAX_REGNAME_LEN    64
#define MAX_REGNUM_LEN     8

#define MAX_PATH	      147
#define MAX_DRIVE       3	
#define MAX_DIR         144	
#define MAX_FNAME       9
#define MAX_EXT         4	

#define WMU_CURFILECHANGE  (WM_USER + 0x1000)
#define WMU_TOOLPALNOTIFY  (WM_USER + 0x1001)

/*
 * Typedefs.  All item structures (INIFILE, SECTION, and KEYVALUE)
 * have the same 10 bytes defined by tagNODEHEADER.
 */
typedef struct tagNODEHEADER
{
   DWORD          dwSize ;                // struct size
   WORD           wMagic ;                // Identifies the struct type
   WORD           wFlags ;                // Flags for this item.
   WORD           wIcon ;                 // Icon ID

} NODEHEADER, *PNODEHEADER, FAR *LPNODEHEADER ;

#define CASTNODEHEADER(lp) ((LPNODEHEADER)lp)

#define NODEHEADER_dwSize(lp)         (CASTNODEHEADER(lp)->dwSize)
#define NODEHEADER_wMagic(lp)         (CASTNODEHEADER(lp)->wMagic)
#define NODEHEADER_wFlags(lp)         (CASTNODEHEADER(lp)->wFlags)
#define NODEHEADER_wIcon(lp)          (CASTNODEHEADER(lp)->wIcon)


/*
 * Structure for INI file items.
 *
 *    wMagic for INIFILE structs is INIFILE_TAG
 */

#define INIFILE_TAG  0x0001

typedef struct tagINIFILE
{
   DWORD          dwSize ;                // struct size
   WORD           wMagic ;                // Identifies the struct type
   WORD           wFlags ;                // Flags for this item.
   WORD           wIcon ;                 // Icon ID

   char           lpszFile[MAX_PATH] ;   // fully qualified pathname
   WORD           wOffsetFile ;           // offset to filename.ext
   time_t         tTimeDate ;             // Time of last write to file
   DWORD          dwFileSize ;            // Length of file in bytes

   BOOL           fVisible ;              // Are the items visible?
   BOOL           fOpen ;                 // TRUE if it *should* be visible.

   LPSTR          lpszOutBuf ;            // current output buffer.
                                          // allocated from hhOutBuf

   HLIST          hlstSections ;          // List of sections

} INIFILE, *PINIFILE, FAR *LPINIFILE ;

#define CASTINIFILE(lp) ((LPINIFILE)lp)

#define INIFILE_dwSize(lp)          (CASTINIFILE(lp)->dwSize)
#define INIFILE_wMagic(lp)          (CASTINIFILE(lp)->wMagic)
#define INIFILE_wFlags(lp)          (CASTINIFILE(lp)->wFlags)
#define INIFILE_wIcon(lp)           (CASTINIFILE(lp)->wIcon)
#define INIFILE_lpszFile(lp)        (CASTINIFILE(lp)->lpszFile)
#define INIFILE_wOffsetFile(lp)     (CASTINIFILE(lp)->wOffsetFile)
#define INIFILE_tTimeDate(lp)       (CASTINIFILE(lp)->tTimeDate)
#define INIFILE_dwFileSize(lp)      (CASTINIFILE(lp)->dwFileSize)
#define INIFILE_lpszOutBuf(lp)      (CASTINIFILE(lp)->lpszOutBuf)
#define INIFILE_fVisible(lp)        (CASTINIFILE(lp)->fVisible)
#define INIFILE_fOpen(lp)           (CASTINIFILE(lp)->fOpen)
#define INIFILE_hlstSections(lp)    (CASTINIFILE(lp)->hlstSections)

#define INIFILE_lpszFileName(lp)    (INIFILE_lpszFile(lp)+INIFILE_wOffsetFile(lp))

/*
 * Structure for SECTIONS (i.e. [Windows]).  The lpszSec pointer
 * is a pointer to a string that is allocated out of the heap
 * identified in the INIFILE struct which owns this section.
 *
 *    wMagic for SECTION structs is SECTION_TAG
 *
 */

#define SECTION_TAG  0x0002

typedef struct tagSECTION
{
   DWORD          dwSize ;                // struct size
   WORD           wMagic ;                // Identifies the struct type
   WORD           wFlags ;                // Flags for this item.
   WORD           wIcon ;                 // Icon ID

   LPSTR          lpszSec ;               // pointer to section string.  
                                          // Allocated out of the heap 
                                          // (hhSecStrings) in INIFILE.
   BOOL           fVisible ;              // Is the section visible in the LB?
   BOOL           fOpen ;                 // TRUE if it *should* be visible.

   HLIST          hlstKeys ;              // List of key = value structs

   LPINIFILE      lpParent ;              // pointer to the parent file

} SECTION, *PSECTION, FAR *LPSECTION ;

#define CASTSECTION(lp)             ((LPSECTION)lp)

#define SECTION_dwSize(lp)          (CASTSECTION(lp)->dwSize)
#define SECTION_wMagic(lp)          (CASTSECTION(lp)->wMagic)
#define SECTION_wFlags(lp)          (CASTSECTION(lp)->wFlags)
#define SECTION_wIcon(lp)          (CASTSECTION(lp)->wIcon)
#define SECTION_lpszSec(lp)         (CASTSECTION(lp)->lpszSec)
#define SECTION_fVisible(lp)        (CASTSECTION(lp)->fVisible)
#define SECTION_fOpen(lp)           (CASTSECTION(lp)->fOpen)
#define SECTION_hlstKeys(lp)        (CASTSECTION(lp)->hlstKeys)
#define SECTION_lpParent(lp)        (CASTSECTION(lp)->lpParent)

/*
 * Structure for KEYVALUE (i.e. Key=Value).  The lpszKey and lpszValue
 * pointers are pointers to strings that are allocated out of the heap
 * identified in the SECTION struct which owns this node.
 *
 *    wMagic for KEYVALUE structs is KEYVALUE_TAG
 *
 */

#define KEYVALUE_TAG  0x0003

typedef struct tagKEYVALUE
{
   DWORD          dwSize ;                // struct size
   WORD           wMagic ;                // Identifies the struct type
   WORD           wFlags ;                // Flags for this item.
   WORD           wIcon ;                 // Icon ID

   LPSTR          lpszKey ;               // pointer to key string.
   LPSTR          lpszValue ;             // pointer to value string
                                          // Allocated out of the heap 
                                          // hhKeyStrings in SECTION struct.
   LPSTR          lpszOutBuf ;            // current output buffer.

   LPSECTION      lpParent ;              // pointer to the parent section

} KEYVALUE, *PKEYVALUE, FAR *LPKEYVALUE ;

#define CASTKEYVALUE(lp)             ((LPKEYVALUE)lp)

#define KEYVALUE_dwSize(lp)          (CASTKEYVALUE(lp)->dwSize)
#define KEYVALUE_wMagic(lp)          (CASTKEYVALUE(lp)->wMagic)
#define KEYVALUE_wFlags(lp)          (CASTKEYVALUE(lp)->wFlags)
#define KEYVALUE_wIcon(lp)           (CASTKEYVALUE(lp)->wIcon)
#define KEYVALUE_lpszKey(lp)         (CASTKEYVALUE(lp)->lpszKey)
#define KEYVALUE_lpszValue(lp)       (CASTKEYVALUE(lp)->lpszValue)
#define KEYVALUE_lpszOutBuf(lp)      (CASTKEYVALUE(lp)->lpszOutBuf)
#define KEYVALUE_lpParent(lp)        (CASTKEYVALUE(lp)->lpParent)


/*
 * Macros
 */
#define ALLOCFROMHEAP(h,n) hlsqHeapAllocLock(h,LPTR,n)
#define FREEFROMHEAP(h, lp) hlsqHeapUnlockFree(h,(LPSTR)lp)

#define GALLOC(n) GlobalAllocPtr(GHND,n)
#define GFREE(lp) GlobalFreePtr(lp)

/*
 * Global Variables, defined in iniedit.c
 */
extern HWND        hwndApp ;
extern HWND        hwndLB ;
extern HWND        hwndInput ;
extern HWND        hwndTool ;
extern HWND        hwndStat ;
extern HWND        hwndEdit ;

extern HINSTANCE   hinstApp ;

extern BOOL        fWin31 ;

extern char        szVerNum[] ;
extern char        szRegisteredName[] ;

extern LPSTR       rglpsz[] ;



extern SORTPROC    lpfnFileCompare ;
extern SORTPROC    lpfnSectionCompare ;
extern SORTPROC    lpfnKeyValueCompare ;


/*
 * Globally managed lists 
 */
extern HLIST       hlstFiles ;

extern HHEAP       hhOutBuf ;    // output buffers
extern HHEAP       hhLists ;     // all lists
extern HHEAP       hhStrings ;   // all strings

extern HHEAP       hhScratch ;    // scratch heap


extern HFONT       hfontSmall ;
extern HFONT       hfontLB ;      // this is the one that is settable
extern char        szFont[] ;
extern WORD        wFontStyle ;
extern WORD        wFontSize ;

extern BOOL        fSearchAll ;
extern BOOL        fToolBar ;
extern BOOL        fToolBarLabels ;
extern BOOL        fStatLine ;
extern short       xPos, yPos, nWidth, nHeight ;
extern BOOL        fSortSections ;
extern BOOL        fSortKeys ;

extern BOOL        fDirty ;

extern char          szCurFile[], szCurFileTitle[] ;
extern LPNODEHEADER  lpCurNode ;
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

/*
 * Semaphores
 */
extern int         semCurFile ;


LRESULT FAR PASCAL fnMainWnd( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;
LPVOID WINAPI AllocBytesFromHeap( HHEAP hh, WORD nBytes ) ;
int FAR PASCAL MessageLoop( HACCEL haccl ) ;


#endif

/************************************************************************
 * End of File: iniedit.h
 ***********************************************************************/

