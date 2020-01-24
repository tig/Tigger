/************************************************************************
 *
 *     Project:  
 *
 *      Module:  bmpbtn.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#define BS_STRETCH  0x8000L
#define BS_NOFOCUS  0x4000L
#define BS_STATIC   0x2000L

#define BMPBTN_BMPFLAG  '~'
#define BMPBTN_AUTOSELECTFLAG '%'
#define BMPBTN_RESNUMFLAG '#'
#define BMPBTN_FILEFLAG '^'

//
// User buttons                              
//
#define BB_SETTEXTCOLOR      (WM_USER+1+ 0x7000)
#define BB_SETFACECOLOR      (WM_USER+2+ 0x7000)
#define BB_SETSHADOWCOLOR    (WM_USER+3+ 0x7000)
#define BB_SETFOCUSCOLOR     (WM_USER+4+ 0x7000)

BOOL FAR PASCAL BmpBtnInit( HANDLE hPrev, HANDLE hInst );


/************************************************************************
 * End of File: bmpbtn.h
 ***********************************************************************/

