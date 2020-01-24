//---------------------------------------------------------------------------
//		Copyright (C) 1991-92, Microsoft Corporation
//
// You have a royalty-free right to use, modify, reproduce and distribute
// the Sample Custom Control Files (and/or any modified version) in any way
// you find useful, provided that you agree that Microsoft has no warranty,
// obligation or liability for any Custom Control File.
//---------------------------------------------------------------------------
// Circ1.h
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Resource Information
//---------------------------------------------------------------------------
// Toolbox bitmap resource IDs numbers.
//---------------------------------------------------------------------------
#define IDBMP_LEDVBX		8000
#define IDBMP_LEDVBXDOWN	8001
#define IDBMP_LEDVBXMONO	8003
#define IDBMP_LEDVBXEGA 	8006


#ifndef RC_INVOKED
//---------------------------------------------------------------------------
// Macro for referencing member of structure
//---------------------------------------------------------------------------
#define OFFSETIN(struc,field)	  ((USHORT)&(((struc *)0)->field))

//---------------------------------------------------------------------------
// Control Procedure
//---------------------------------------------------------------------------
LONG FAR PASCAL _export LEDVBXCtlProc(HCTL, HWND, USHORT, USHORT, LONG);

//---------------------------------------------------------------------------
// LED control data and structs
//---------------------------------------------------------------------------
typedef struct tagLED
    {
    RECT  rectDrawInto;
    SHORT LEDVBXShape;
    LONG  FlashColor;
    } LED;

typedef LED FAR * LPLED;

#define LpLEDDEREF(hctl)	((LPLED)VBDerefControl(hctl))


//---------------------------------------------------------------------------
// Property info
//---------------------------------------------------------------------------
PROPINFO Property_LEDVBXShape =
    {
    "LEDVBXShape",
    DT_SHORT | PF_fGetData | PF_fSetMsg | PF_fSaveData,
    OFFSETIN(LED, LEDVBXShape), 0,
    0,
    NULL, 0
    };

PROPINFO Property_FlashColor =
    {
    "FlashColor",
    DT_COLOR | PF_fGetData | PF_fSetData | PF_fSaveData,
    OFFSETIN(LED, FlashColor), 0,
    0,
    NULL, 0
    };


//---------------------------------------------------------------------------
// Property list
//---------------------------------------------------------------------------
// Define the consecutive indicies for the properties
//---------------------------------------------------------------------------
#define IPROP_LEDVBX_CTLNAME              0
#define IPROP_LEDVBX_INDEX                1
#define IPROP_LEDVBX_BACKCOLOR            2
#define IPROP_LEDVBX_LEFT                 3
#define IPROP_LEDVBX_TOP                  4
#define IPROP_LEDVBX_WIDTH                5
#define IPROP_LEDVBX_HEIGHT               6
#define IPROP_LEDVBX_VISIBLE              7
#define IPROP_LEDVBX_PARENT               8
#define IPROP_LEDVBX_DRAGMODE             9
#define IPROP_LEDVBX_DRAGICON            10
#define IPROP_LEDVBX_TAG                 11
#define IPROP_LEDVBX_SHAPE		 12
#define IPROP_LEDVBX_FLASHCOLOR 	 13
#define IPROP_LEDVBX_HWND		 14

PPROPINFO LEDVBX_Properties[] =
    {
    PPROPINFO_STD_CTLNAME,
    PPROPINFO_STD_INDEX,
    PPROPINFO_STD_BACKCOLOR,
    PPROPINFO_STD_LEFT,
    PPROPINFO_STD_TOP,
    PPROPINFO_STD_WIDTH,
    PPROPINFO_STD_HEIGHT,
    PPROPINFO_STD_VISIBLE,
    PPROPINFO_STD_PARENT,
    PPROPINFO_STD_DRAGMODE,
    PPROPINFO_STD_DRAGICON,
    PPROPINFO_STD_TAG,
    &Property_LEDVBXShape,
    &Property_FlashColor,
    PPROPINFO_STD_HWND,
    NULL
    };


//---------------------------------------------------------------------------
// Event info
//---------------------------------------------------------------------------
WORD Paramtypes_ClickIn[] = {ET_R4, ET_R4};

EVENTINFO Event_ClickIn =
    {
    "ClickIn",
    2,
    4,
    Paramtypes_ClickIn,
    "X As Single,Y As Single"
    };

EVENTINFO Event_ClickOut =
    {
    "ClickOut",
    0,
    0,
    NULL,
    NULL
    };


//---------------------------------------------------------------------------
// Event list
//---------------------------------------------------------------------------
// Define the consecutive indicies for the events
//---------------------------------------------------------------------------
#define IEVENT_LEDVBX_CLICKIN		  0
#define IEVENT_LEDVBX_CLICKOUT		  1
#define IEVENT_LEDVBX_DRAGDROP		  2
#define IEVENT_LEDVBX_DRAGOVER		  3


PEVENTINFO LEDVBX_Events[] =
    {
    &Event_ClickIn,
    &Event_ClickOut,
    PEVENTINFO_STD_DRAGDROP,
    PEVENTINFO_STD_DRAGOVER,
    NULL
    };


//---------------------------------------------------------------------------
// Model struct
//---------------------------------------------------------------------------
// Define the control model (using the event and property structures).
//---------------------------------------------------------------------------
MODEL modelLEDVBX =
    {
    VB_VERSION, 			// VB version being used
    0,					// MODEL flags
    (PCTLPROC)LEDVBXCtlProc,		// Control procedure
    CS_VREDRAW | CS_HREDRAW,		// Class style
    0L, 				// Default Windows style
    sizeof(LED),			// Size of LED structure
    IDBMP_LEDVBX,			// Palette bitmap ID
    "LED",				// Default control name
    "LED2",				// Visual Basic class name
    NULL,				// Parent class name
    LEDVBX_Properties,			// Property information table
    LEDVBX_Events,			// Event information table
    IPROP_LEDVBX_SHAPE, 		// Default property
    IEVENT_LEDVBX_CLICKIN,		// Default event
    IPROP_LEDVBX_SHAPE			// Property representing value of ctl
    };




#ifndef _LED_H_
#define _LED_H_

/************************************************************************
 * LED
 ***********************************************************************/

#define SZ_LEDCLASSNAME    "LEDCTRL"
          
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
          
#define LED_SETCOLOR       (WM_USER + 1 + 0x7000)
#define LED_GETCOLOR       (WM_USER + 2 + 0x7000)

#define LED_ROUND          (0x0100)
#define LED_HORZ           (0x0200)
#define LED_VERT           (0x0400)

#endif // _LED_H_

#endif	// RC_INVOKED

//---------------------------------------------------------------------------
