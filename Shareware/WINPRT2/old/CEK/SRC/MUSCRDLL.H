/*
 * MUSCRDLL.H
 *
 * DLL Specific include file.
 *
 * Contains all definitions and and prototypes pertinent ONLY
 * to the DLL.  API related information is contained in MUSCROLL.H.
 * API in any way.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 *
 */

#include <custcntl.h>   //Standard Windows header file for custom controls.

/*
#include "muscroll.h"   //Get interface stuff for the control.
*/


/*
 * In window extra bytes we simply store a local handle to
 * a MUSCROLL data structure.  The local memory is allocated
 * from the control's local heap (either application or DLL)
 * instead of from USER's heap, thereby saving system resources.
 *
 * Note that the window styles that are stored in the regular
 * windwow structure are copied here.  This is to optimize access
 * to these bits, avoiding extra calls to GetWindowLong.
 */

typedef struct tagMUSCROLL
    {
    HWND        hWndAssociate;  //Associate window handle
    DWORD       dwStyle;        //Copy of GetWindowLong(hWnd, GWL_STYLE)
    UINT        iMin;           //Minimum position
    UINT        iMax;           //Maximum position
    UINT        iPos;           //Current position
    UINT        wState;         //State flags
    COLORREF    rgCr[CCOLORS];  //Configurable colors.
    } MUSCROLL ;

typedef MUSCROLL     *PMUSCROLL;
typedef MUSCROLL FAR *LPMUSCROLL;

#define CBMUSCROLL sizeof(MUSCROLL)

// CEK 10/26/92:  Changed to GWL and use of GlobalAllocPtr() to simplify
// WIN32 port
//
//Offsets to use with GetWindowLong
#define GWL_MUSCROLL       0

//Extra bytes for the window if the size of a local handle.
#define CBWINDOWEXTRA       sizeof(LPMUSCROLL)

//Extra Class bytes.
#define CBCLASSEXTRA        0


//Control state flags.
#define MUSTATE_GRAYED      0x0001
#define MUSTATE_HIDDEN      0x0002
#define MUSTATE_MOUSEOUT    0x0004
#define MUSTATE_UPCLICK     0x0008
#define MUSTATE_DOWNCLICK   0x0010
#define MUSTATE_LEFTCLICK   0x0008  //Repeated since MSS_VERTICAL and
#define MUSTATE_RIGHTCLICK  0x0010  //MSS_HORIZONTAL are exclusive.

//Combination of click states.
#define MUSTATE_CLICKED     (MUSTATE_LEFTCLICK | MUSTATE_RIGHTCLICK)

//Combination of state flags.
#define MUSTATE_ALL         0x001F

/*
 * Macros to change the control state given a PMUSCROLL
 * and state flag(s)
 */
#define StateSet(p, wFlags)    (p->wState |=  (wFlags))
#define StateClear(p, wFlags)  (p->wState &= ~(wFlags))
#define StateTest(p, wFlags)   (p->wState &   (wFlags))


//Private functions specific to the control.

//INIT.C
BOOL   WINAPI FRegisterControl(HANDLE);
LONG   WINAPI LMicroScrollCreate(HWND, UINT, LPMUSCROLL, LPCREATESTRUCT);
BOOL   WINAPI FTextParse(LPSTR, LPINT, LPINT, LPINT);
UINT   WINAPI WTranslateUpToChar(LPSTR FAR *, char);

//MSAPI.C
LONG   WINAPI LMicroScrollAPI(HWND, UINT, UINT, LONG, LPMUSCROLL);


//MUSCROLL.C
LONG   CALLBACK MicroScrollWndProc(HWND, UINT, WPARAM, LPARAM);
void   WINAPI PositionChange(HWND, LPMUSCROLL);
void   WINAPI ClickedRectCalc(HWND, LPMUSCROLL, LPRECT);


//PAINT.C
LONG   WINAPI LMicroScrollPaint(HWND, LPMUSCROLL);
void   WINAPI Draw3DButtonRect(HDC, HPEN, HPEN, UINT, UINT,\
                                   UINT, UINT, BOOL);





//Timer identifiers.
#define IDT_FIRSTCLICK      500
#define IDT_HOLDCLICK       501

#define CTICKS_FIRSTCLICK   400
#define CTICKS_HOLDCLICK    50


//Default range and position constants.
#define IDEFAULTMIN         0
#define IDEFAULTMAX         9
#define IDEFAULTPOS         5

/*
 * All that follows in this include file is specific to
 * the dialog editor interface for the MicroScroll control.
 */

//Identifiers for the Style dialog.
#define ID_NULL                 -1
#define ID_IDEDIT               100
#define ID_TEXTEDIT             101
#define ID_RADIOVERTICAL        102
#define ID_RADIOHORIZONTAL      103
#define ID_CHECKTEXTHASRANGE    104
#define ID_CHECKNOPEGSCROLL     105
#define ID_CHECKINVERTRANGE     106



//Dialog box identifiers.
#define IDD_STYLEDIALOG         1

#ifndef WIN32
//Standard dialog editor callbacks.
HANDLE WINAPI HCtlInfo(void);
UINT   WINAPI WTranslateCtlFlags(DWORD, LPSTR, UINT);
BOOL   WINAPI FShowCtlStyleDlg(HWND, HANDLE, LPFNSTRTOID, LPFNIDTOSTR);
BOOL   WINAPI FCtlStyleDlgProc(HWND, UINT, UINT, LONG);
#endif
