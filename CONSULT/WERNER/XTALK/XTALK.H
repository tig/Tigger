/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   Crosstalk for Windows.                                          *
*                                                                             *
*   Module:   The Main XTALK Include File (XTALK.H)                           *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.01 01/31/90 dca00010 PJL Added Help support for Window Menu.  *
*             1.1  03/30/90 dca00061 PJL Split up file - was getting too big. *
*                                                                             *
\*****************************************************************************/

#ifdef NOTRC
#include "dosos2.h"
#include "funcdefs.h"
#endif

typedef char  VAR;
typedef VAR  *PVAR;
typedef char  *PSZ;

#define MAX_PATH	128


/* dialog identifiers */

#define IDM_MENU		10
#define IDM_CISG		11

#define CMD_RUNAPP              15
#define CMD_NEW 		16
#define CMD_OPEN		17
#define CMD_SAVE		18
#define CMD_SAVEAS		19
#define CMD_CAPTURE		20
#define CMD_UPLOAD		21
#define CMD_PAGESETUP		22
#define CMD_PRINT		23
#define CMD_PRINTSETUP		24
#define CMD_SEND		25
#define CMD_RECEIVE		26
#define CMD_EXIT                27
#define CMD_COPY                28
#define CMD_COPYFILE            29
#define CMD_COPYCAPTURE         30
#define CMD_COPYNOTES           31
#define CMD_COPYPRINTER         32
#define CMD_COPYSEND            33
#define CMD_PASTE               34
#define CMD_PASTEUPLOAD         35
#define CMD_SCROLL              36
#define CMD_SEARCHSCROLL        37
#define CMD_CONNECT             38
#define CMD_DIAL                39
#define CMD_EXECUTE             40
#define CMD_NOTES               41
#define CMD_KERMIT              42
#define CMD_PLAYBACK            43
#define CMD_BREAK               44
#define CMD_RESET               45
#define CMD_SESSION             46
#define CMD_COMMUNICATIONS      47
#define CMD_MODEM               48
#define CMD_TERMINAL            49
#define CMD_PROTOCOL            50
#define CMD_ASCII               51
#define CMD_MACROS              52
#define CMD_DISPLAY             53
#define CMD_FONTS               54
#define CMD_SYSTEM              55
#define CMD_MENUNULL            56
#define CMD_MENUMACRO1          57
#define CMD_MENUMACRO2          58
#define CMD_MENUMACRO3          59
#define CMD_MENUMACRO4          60
#define CMD_MENUMACRO5          61
#define CMD_MENUMACRO6          62
#define CMD_MENUMACRO7          63
#define CMD_MENUMACRO8          64
#define CMD_MENUMACRO9          65
#define CMD_MENUMACRO10         66
#define CMD_MENUMACRO11         67
#define CMD_MENUMACRO12         68
#define CMD_MENUMACRO13         69
#define CMD_MENUMACRO14         70
#define CMD_MENUMACRO15         71
#define CMD_MENUMACRO16         72
#define CMD_MENUMACRO17         73
#define CMD_MENUMACRO18         74
#define CMD_MENUMACRO19         75
#define CMD_MENUMACRO20         76
#define CMD_MENUMACRO21         77
#define CMD_MENUMACRO22         78
#define CMD_MENUMACRO23         79
#define CMD_MENUMACRO24         80
#define CMD_MENUMACRO25         81
#define CMD_MENUMACRO26         82
#define CMD_MENUMACRO27         83
#define CMD_MENUMACRO28         84
#define CMD_MENUMACRO29         85
#define CMD_MENUMACRO30         86
#define CMD_MENUMACRO31         87
#define CMD_MENUMACRO32         88
#define CMD_MENUMACRO33         89
#define CMD_MENUMACRO34         90
#define CMD_MENUMACRO35         91
#define CMD_MENUMACRO36         92
#define CMD_MENUMACRO37         93
#define CMD_MENUMACRO38         94
#define CMD_MENUMACRO39         95
#define CMD_MENUMACRO40         96
#define CMD_MENUMACRO41         97
#define CMD_MENUMACRO42         98
#define CMD_MENUMACRO43         99
#define CMD_MENUMACRO44        100
#define CMD_MENUMACRO45        101
#define CMD_MENUMACRO46        102
#define CMD_MENUMACRO47        103
#define CMD_MENUMACRO48        104
#define CMD_WINDOW_XFER        105
#define CMD_WINDOW_NOTES       106
#define CMD_INDEX              107
#define CMD_ABOUT              108

/* MKL added Win30 Help */
#define CMD_HELP_PROC		110
#define CMD_HELP_CMMD		111
#define CMD_HELP_MOUSE		112
#define CMD_HELP_KBD		113
#define CMD_HELP_DDE		114
#define CMD_HELP_HELP		115
#define CMD_HELP_LAST		116

#define CMD_CISG_COPY		17
#define CMD_CISG_INVERT         18

#define IDD_ABOUT		32
#define IDD_OPEN		33
#define IDD_SAVEAS		34
#define IDD_ABORT		35
#define IDD_ALERT		36
#define IDD_NOPHONE		37
#define IDD_CAPTURE		38
#define IDD_TERMINAL		39
#define IDD_COMMUNICATIONS	40
#define IDD_SESSION		41
#define IDD_MACROS		42
#define IDD_PROTOCOL_ASCII	43
#define IDD_SYSTEM		44
#define IDD_MODEM		45
#define IDD_DIAL		46
#define IDD_EXECUTE		47
#define IDD_PROMPT		48
#define IDD_PROTOCOL		49
#define IDD_UPLOAD		50
#define IDD_DOWNLOAD		51
#define IDD_XFER		52
#define IDD_ASCII		54
#define IDD_OPENDEMO		55
#define IDD_DISPLAY		56
#define IDD_NOTES		57
#define IDD_BATCH               58
#define IDD_FILE_REQUEST	59
#define IDD_INTRO		60
#define IDD_FONTS		61
#define IDD_PROTOCOL_CROSSTALK	62
#define IDD_PROTOCOL_DART	63
#define IDD_PROTOCOL_XMODEM	64
#define IDD_PROTOCOL_ZMODEM     65
#define IDD_COMERROR            66
#define IDD_INTERROR            67
#define IDD_DIALING             68
#define IDD_GENERROR            69
#define IDD_PAGESETUP           70
#define IDD_PRINTSETUP          71
#define IDD_HELP1               72
#define IDD_HELP2               73
#define IDD_NEWFILE             74
#define IDD_RUNAPP              75
#define IDD_PROTOCOL_CISB       76
#define IDD_SETTING             77
#define IDD_COMPILE             78

#define IDI_ICON	    32
#define IDI_ACCEL	    33

#define IDB_TITLE1          32
#define IDB_TITLE2          33
#define IDB_FROG1           34

#define IDC_HAND            40

#define USER		    256
#define IDU_TOKENS          1


/* DlgFnOpen/DlgFnSave/DlgFnSend return flags, constants */

/* return flags for DlgFnSave() */
#define     NOSAVE	    0
#define     NEWSAVE	    1
#define     OLDSAVE	    2

/* return flags for DlgFnOpen() */
#define     NOOPEN	    0
#define     NEWOPEN	    1
#define     OLDOPEN	    2
#define     BATCH           3

#define     MAX_STR_LEN     82
#define     MAX_FNAME_LEN   82
#define     CBEXTMAX        6   /* Number of bytes in "\*.ext" */

#define MAX_FNAME_STR	   82
#define ONESEC		 1000	/* 1000 ms equals one second */

#define COMM_TIMER	    1

#define     FE_DBOPEN	    100
#define     FE_LMEM	    101
#define     FE_GMEM	    102


/* character constants */
#define ASC_NUL     0x00
#define ASC_SOH     0x01
#define ASC_STX     0x02
#define ASC_ETX     0x03
#define ASC_EOT     0x04
#define ASC_ENQ     0x05
#define ASC_ACK     0x06
#define ASC_BEL     0x07
#define ASC_BS	    0x08
#define ASC_HT	    0x09
#define ASC_LF	    0x0A
#define ASC_VT	    0x0B
#define ASC_FF	    0x0C
#define ASC_CR	    0x0D
#define ASC_SO	    0x0E
#define ASC_SI	    0x0F
#define ASC_DLE     0x10
#define ASC_DC1     0x11
#define ASC_XON     0x11
#define ASC_DC2     0x12
#define ASC_TON     0x12
#define ASC_DC3     0x13
#define ASC_XOFF    0x13
#define ASC_DC4     0x14
#define ASC_TOFF    0x14
#define ASC_NAK     0x15
#define ASC_SYN     0x16
#define ASC_ETB     0x17
#define ASC_CAN     0x18
#define ASC_EM	    0x19
#define ASC_SUB     0x1A
#define ASC_ESC     0x1B
#define ASC_FS	    0x1C
#define ASC_GS	    0x1D
#define ASC_RS	    0x1E
#define ASC_US	    0x1F
#define ASC_MINUS   0x2D
#define ASC_EQUAL   0x3D
#define ASC_DEL     0x7F


/*----------------------------------------------------------------------------*/
/*  Define System State, or what we are doing with our cpu slice.	      */
/*----------------------------------------------------------------------------*/
extern WORD wSystemState;
#define SS_NULL 	0
#define SS_TERMINAL	1
#define SS_PROGRAM	2
#define SS_PROCESS	3
#define SS_DIALING	4
extern HWND hChildTransfer;

/*----------------------------------------------------------------------------*/
/*  Define System Status conditions that would prevent normal terminal I/O    */
/*----------------------------------------------------------------------------*/
extern WORD wSystem;
#define SS_ABORT	0x0001
#define SS_DDE		0x0002
#define SS_INIT 	0x0004
#define SS_SCRIPT	0x0008
#define SS_SCROLL	0x0010
#define SS_XFER 	0x0020
#define SS_DIRECT	0x0040
#define SS_DIAL 	0x0080
#define IfSystemStatus(x) (wSystem & x)


/*----------------------------------------------------------------------------*/
/*  Define the global hardware configuration variables			      */
/*----------------------------------------------------------------------------*/
extern char fMonoDisplay;
extern char fLowResDisplay;
extern char fEnglandCharSet;
extern int  nVertRes;
extern int  nHorzRes;
extern BYTE byScrollBarHeight;
extern BYTE byScrollBarWidth;
extern char fMaximized;


/*----------------------------------------------------------------------------*/
/*  Define the various conditions that can exist under XTALK/WNDS	      */
/*----------------------------------------------------------------------------*/
extern BOOL bConnected; 	/* Connection is active, protect line.	    */
extern BOOL bPrinterOn; 	/* Copy all character input/output to print */
extern BOOL bCaptureOn; 	/* Copy all character input/output to disk  */
extern BOOL bDisplayOn; 	/* Echo characters to the screen	    */

/*----------------------------------------------------------------------------*/
/*	global variables														   */
/*----------------------------------------------------------------------------*/
extern char szAppName[16];
extern char szAppTitle[32];
extern char szIFN[MAX_STR_LEN];
extern char szFNF[MAX_STR_LEN];
extern char szREF[MAX_STR_LEN];
extern char szSCC[MAX_STR_LEN];
extern char szEOF[MAX_STR_LEN];
extern char szECF[MAX_STR_LEN];
extern char szExt[MAX_STR_LEN];

extern int  nDDESessions;

extern HANDLE hWndInst;
extern HWND   hWnd;
extern HMENU  hWndMenu;
extern HANDLE hAccelTable;
extern BOOL   fIsActive;

extern short  sVersion;  // 00020 Make Windows 3.0 compatible.


/*----------------------------------------------------------------------------*/
/*  Support for the Status Child Window 				      */
/*----------------------------------------------------------------------------*/
extern HWND hChildStatus;
extern char fStatusDisplay;

#define UM_EMULATOR WM_USER
#define UM_CONFIG   WM_USER+1
#define UM_LIGHTS   WM_USER+2
#define UM_INSERT   WM_USER+3
#define UM_KEYPAD   WM_USER+4
#define UM_CONNECT  WM_USER+5
#define UM_TIMER    WM_USER+6


/*----------------------------------------------------------------------------*/
/*  Support for the Info Child Window					      */
/*----------------------------------------------------------------------------*/
extern HWND hChildInfo;
extern char fInfoDisplay;
extern char fMenuMode;

#define UM_KEYSTATE WM_USER
#define UM_SCRNMODE WM_USER+1
#define UM_SCROLPOS WM_USER+2
#define UM_INFOMSG  WM_USER+3

#define INFO_AUTO	    0x01
#define INFO_CONTROL	    0x02
#define INFO_XOFFHOLD	    0x04


/*----------------------------------------------------------------------------*/
/*  Support for the Macro Child Window					      */
/*----------------------------------------------------------------------------*/
extern HWND hChildMacro;

#define UM_NEWMACROS	WM_USER


/*----------------------------------------------------------------------------*/
/*  Support for the Input Child Window					      */
/*----------------------------------------------------------------------------*/
extern HWND hChildInput;
extern HWND hChildPrompt;


/*----------------------------------------------------------------------------*/
/*  Support for the Dial Child Window					      */
/*----------------------------------------------------------------------------*/
extern HWND hDlgDialing;


/*----------------------------------------------------------------------------*/
/*  Support for Terminal Processing					      */
/*----------------------------------------------------------------------------*/
extern HWND hChildTerminal;
extern int  nCurEmulator;

#define UM_RESET    WM_USER
#define UM_DATA     WM_USER+1
#define UM_NEWFONT  WM_USER+2


/*----------------------------------------------------------------------------*/
/*  Support for Font Processing 					      */
/*----------------------------------------------------------------------------*/
extern HANDLE  hSysFont;
extern short	 nSysFontWidth;
extern short	 nSysFontHeight;

extern HANDLE  hCurFont;
extern short	 nCurFontWidth;
extern short	 nCurFontHeight;
extern BYTE	 bCurFontCharSet;

extern char fVT102Font;

/*----------------------------------------------------------------------------*/
/*  Support for the Icon Window 					      */
/*----------------------------------------------------------------------------*/
extern HANDLE hIconFont;
extern char	szIconText[8];
extern int	nIconTextX;
extern int	nIconTextY;


//
// Help Support
//
#ifndef RC_INVOKED
#define HELP_ITEM_ABOUT 		    0
#define HELP_ITEM_SCREEN		    1
#define HELP_ITEM_MACTION		    2
#define HELP_ITEM_ANSWERING		3
#define HELP_ITEM_ASCIISET		    4
#define HELP_ITEM_BREAK 		    5
#define HELP_ITEM_CAPTURE		    6
#define HELP_ITEM_CONNECT		    7
#define HELP_ITEM_COPY			    8
#define HELP_ITEM_COPYCAPTURE		9
#define HELP_ITEM_COPYFILE	       10
#define HELP_ITEM_COPYHOST	       11
#define HELP_ITEM_COPYNOTES	   12
#define HELP_ITEM_COPYPRINTER	   13
#define HELP_ITEM_DEVICE	       14
#define HELP_ITEM_DIAL		       15
#define HELP_ITEM_DISPLAY	       16
#define HELP_ITEM_DDE		       17
#define HELP_ITEM_MEDIT 	       18
#define HELP_ITEM_MESSAGES	       19
#define HELP_ITEM_EXIT		       20
#define HELP_ITEM_MFILE 	       21
#define HELP_ITEM_TRANSFER	       22
#define HELP_ITEM_FONTS 	       23
#define HELP_ITEM_FUNCTIONS	   24
#define HELP_ITEM_MHELP 	       25
#define HELP_ITEM_TOPICS	       26
#define HELP_ITEM_KCP		       27
#define HELP_ITEM_MODEM 	       28
#define HELP_ITEM_NEW		       29
#define HELP_ITEM_NOTES 	       30
#define HELP_ITEM_OPEN		       31
#define HELP_ITEM_PAGESETUP	   32
#define HELP_ITEM_PASTE 	       33
#define HELP_ITEM_PASTEHOST	   34
#define HELP_ITEM_PHONEBOOK	   35
#define HELP_ITEM_CALLING	       36
#define HELP_ITEM_PLAYBACK	       37
#define HELP_ITEM_PRINT 	       38
#define HELP_ITEM_PRINTERSETUP	   39
#define HELP_ITEM_PROTOCOL	       40
#define HELP_ITEM_RECEIVE	       41
#define HELP_ITEM_RESET 	       42
#define HELP_ITEM_REVIEWING	   43
#define HELP_ITEM_SAVEAS	       44
#define HELP_ITEM_SAVE		       45
#define HELP_ITEM_SCREENCAPTURE   46
#define HELP_ITEM_SCREENFILE	   47
#define HELP_ITEM_SCREENNOTES	   48
#define HELP_ITEM_SCREENPRINTER   49
#define HELP_ITEM_SCRIPT	       50
#define HELP_ITEM_SCRIPTS	       51
#define HELP_ITEM_SCROLL	       52
#define HELP_ITEM_SEND		       53
#define HELP_ITEM_SESSION	       54
#define HELP_ITEM_MSETUP	       55
#define HELP_ITEM_SYSTEM	       56
#define HELP_ITEM_TERMINAL	       57
#define HELP_ITEM_TRANSFERRING	   58
#define HELP_ITEM_UPLOADTEXT	   59
#define HELP_ITEM_MUSER 	       60
#define HELP_ITEM_WINDOW          61
#define HELP_ITEM_MAX		       61
#endif

extern WORD wHelpDialog;
extern WORD wHelpMenu;


/*----------------------------------------------------------------------------*/
/*  Support for the Printer						      */
/*----------------------------------------------------------------------------*/
extern HDC  hPrintDC;
extern int  nPrintLineNo;
extern int  nPrintPageNo;
extern int  nPrintCharLn;


/*----------------------------------------------------------------------------*/
/*  Assertion macro.							      */
/*----------------------------------------------------------------------------*/
#ifndef NDEBUG

static char _win_assertstring[] = "Assertion failed: %s, file %s, line %d\n";

#define win_assert(exp) { \
    if (!(exp)) { \
        char buf[96]; \
        sprintf(buf, _win_assertstring, #exp, __FILE__, __LINE__); \
        MessageBox(NULL, buf, "Assertion Error", MB_OK); \
        } \
    }

#else

#define win_assert(exp)

#endif /* NDEBUG */


