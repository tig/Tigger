/************************************************************************
 *
 *     Project:  Tool Palette
 *
 *      Module:  toolpal.h
 *
 *
 ***********************************************************************/


#ifndef _TOOLPAL_H_
#define _TOOLPAL_H_

#ifndef _RGB_H_
#define _RGB_H_

   #define RGBBLACK     RGB(0,0,0)
   #define RGBRED       RGB(128,0,0)
   #define RGBGREEN     RGB(0,128,0)
   #define RGBBLUE      RGB(0,0,128)
   #define RGBBROWN     RGB(128,128,0)
   #define RGBMAGENTA   RGB(128,0,128)
   #define RGBCYAN      RGB(0,128,128)
   #define RGBLTGRAY    RGB(192,192,192)
   #define RGBGRAY      RGB(128,128,128)

   #define RGBLTRED     RGB(255,0,0)
   #define RGBLTGREEN   RGB(0,255,0)
   #define RGBLTBLUE    RGB(0,0,255)
   #define RGBYELLOW    RGB(255,255,0)
   #define RGBLTMAGENTA RGB(255,0,255)
   #define RGBLTCYAN    RGB(0,255,255)
   #define RGBWHITE     RGB(255,255,255)
#endif

/************************************************************************
 * General constants
 ***********************************************************************/
#define TP_SZCLASSNAME     "_42toolpal"

#define TP_ERR             MAKELRESULT( 0xFFFF, 0xFFFF )

/* AutoSelect windows style */
#define TPS_AUTOSELECT     0x0001L
#define TPS_ROWORIENT      0x0002L
#define TPS_TOOLBAR        0x0004L
#define TPS_BORDERS        0x0008L
#define TPS_STRETCHBLT     0x0010L
#define TPS_LIGHTBKGND     0x0020L
#define TPS_ROUNDLOOK      0x0040L

#define TPS_HORZCAPTION    IBS_HORZCAPTION      // 0x4000L
#define TPS_VERTCAPTION    IBS_VERTCAPTION      // 0x8000L

/************************************************************************
 * Messaging constants
 ***********************************************************************/
#define TP_FIRSTMESSAGE    TP_ADDITEM         
#define TP_ADDITEM         (WM_USER + 0x0700)
#define TP_CHANGEITEM      (WM_USER + 0x0701)
#define TP_DELITEM         (WM_USER + 0x0702)
#define TP_ENABLEITEM      (WM_USER + 0x0703)
#define TP_INSERTITEM      (WM_USER + 0x0704)
#define TP_FORCERESIZE     (WM_USER + 0x0705)
#define TP_GETITEMSTATE    (WM_USER + 0x0706)
#define TP_GETSELECTEDITEM (WM_USER + 0x0707)
#define TP_SETITEMSTATE    (WM_USER + 0x0708)
#define TP_SETNUMCOLS      (WM_USER + 0x0709)
#define TP_TOGGLEITEMSTATE (WM_USER + 0x070A)
#define TP_SETITEMSIZE     (WM_USER + 0x070B)
#define TP_SETBORDERWIDTH  (WM_USER + 0x070C)
#define TP_SETNOTIFYMSG    (WM_USER + 0x070D)
#define TP_LASTMESSAGE     TP_SETNOTIFYMSG  

/* TPN_STATECHANGE signals that a state change has taken place */
/* it is passed as LOBYTE(HIWORD( lParam ))                    */
#define TPN_STATECHANGE    0x0001
#define TPN_QUERYCLOSE     0x0002

/* Masks for the TPN_STATECHANGE notification: HIBYTE(HIWORD(lparam)) */
#define TPDEPRESSED        0x0100
//#define TPKEYINPUT         0x0200
#define TPRBUTTON          0x0400
//#define TPDBLCLK           0x0800
#define TPSHIFT            0x1000
#define TPCTRL             0x2000


/************************************************************************
 * Macros
 ***********************************************************************/

/************************************************************************
 * Data structure constants
 ***********************************************************************/

/************************************************************************
 * Data structures
 ***********************************************************************/

typedef struct tagTOOLPALITEM
{
   DWORD       dwSize ;          // sizeof (TOOLPALITEM)
   WORD        wID ;
   HINSTANCE   hModule ;
   LPSTR       lpszUpBitmap  ;
   LPSTR       lpszDnBitmap  ;
   LPSTR       lpszDisBitmap ;
   WORD        wFlags ;
   WORD        cGangLoadItems ;
   BYTE        rgbReserved[18] ;
} TOOLPALITEM, *PTOOLPALITEM, FAR *LPTOOLPALITEM ;

/************************************************************************
 * Function prototypes
 ***********************************************************************/
BOOL WINAPI tpRegister( HINSTANCE hInstance ) ;

#endif 

/************************************************************************
 * End of File: toolpal.h
 ***********************************************************************/

