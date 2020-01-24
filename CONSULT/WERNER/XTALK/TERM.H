/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Terminal Include File (TERM.H)                                  *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*    1.1  03/01/90 dca00033 MKL added Auto keyborad repeat ON/OFF capability  *
*    1.1  03/01/90 dca00035 MKL added VT102 Cursor enable/disable capability  *
*                                                                             *
\*****************************************************************************/

#define TERM_MAXROW	      24
#define TERM_MAXCOL	     132
#define TERM_MEMORY	    6360
#define TERM_LINE	     265

#define TERM_ATTR_BOLD	    0x08
#define TERM_ATTR_UNDERLINE 0x80
#define TERM_ATTR_BLACK     0x00
#define TERM_ATTR_RED	    0x01
#define TERM_ATTR_GREEN     0x02
#define TERM_ATTR_YELLOW    0x03
#define TERM_ATTR_BLUE	    0x04
#define TERM_ATTR_MAGENTA   0x05
#define TERM_ATTR_CYAN	    0x06
#define TERM_ATTR_WHITE     0x07

#define TERM_VT102	    0
#define TERM_VT52	    1
#define TERM_IBMPC	    2
#define TERM_IBM3101	    3
#define TERM_VIDTEX	    4

#define TERM_CURSOR_BAR     1
#define TERM_CURSOR_LINE    2
#define TERM_CURSOR_BLOCK   3

struct stTerm
  {			/* the value of this structure is set by doing
				Setup.Terminal.More .............MKL */
  char cTerminal;	/* terminal type: TERM_VT102, TERM_IBMPC, etc. */
  char filler1;		/* one byte filler to make data aligned */
  int  nColumns;	/* number of columns: 80 or 132 */
  char cCurShape;	/* Cursor shape: BAR, LINE or BLOCK */
  char fLfAuto;		/* Auto Linefeed: T/F */
  char fNewLine;	/* Enter Key Sends: T=CRLF/F=CR */
  char fAutoWrap;	/* Auto Wrap to next line: 
				 0=not checked/1=checked/2=grayed */
  char fEcho;		/* Echo typed chars locally: 0/1/2 */
  char filler3;		/* one byte filler to make data aligned */
  char fNoColor;	/*  ! Interpret color codes form host: 0/1/2 */
  char fFormFeed;	/* Add form feed to page: 0/1/2 */
  char fPrintExtent;	/* Print Page prints entire screen: 0/1/2 */
  char fSwapDel;	/* Swap BS and Ctrl-BS control codes: 0/1/2 */
  char fWindowWrap;	/* Wrap text to fit resized window: 0/1/2 */
  char szEnter[8];	/* first byte is length, then chars that will be
				send be pressing ENTER key */
  char cTabWidth;	/* Tab width : 1 <= cTabWidth <= 16 */
  char szAnswerback[17];/* answerback string: set by answerback command */
  char filler2[47];	/* 47 bytes reserved */
  };

extern struct stTerm Term;


extern HDC    hTerminalDC;
extern char   fTermFocusActive;
extern char   fTermCaretActive;
extern char   fTermDisableChar;

extern char   szTermName[16];	/* Name of this terminal emulator      */

extern int    nTermChrX;	/* Current Character X cursor position	   */
extern int    nTermChrY;	/*		     Y cursor position	   */
extern char   cTermAttr;	/*		     Attributes 	   */
extern int    nTermCSet;	/* Current Screen Display Character Set    */
extern BOOL   bTermWrap;	/*			  Line Wrap Mode   */
extern BOOL   bTermOrigin;	/*			  Origin Setting   */
extern int    nTermErase;	/*			  Selective Erase  */

extern int    nSaveChrX;	/* Saved Character X cursor position	   */
extern int    nSaveChrY;	/*		   Y cursor position	   */
extern int    nSaveAttr;	/*		   Attributes		   */
extern int    nSaveCSet;	/* Saved Screen Display Character Set	   */
extern BOOL   bSaveWrap;	/*			Line Wrap Mode	   */
extern BOOL   bSaveOrigin;	/*			Origin Setting	   */
extern int    nSaveErase;	/*			Selective Erase    */

extern int    nTermMarginTop;
extern int    nTermMarginBot;

extern BOOL   bTermLFCR;
extern BOOL   bTermEcho;
extern BOOL   bTermSeeC;
extern BOOL   bTermQFlg;
extern BOOL   bTermPFlg;
extern BOOL   bTermXON;
extern BOOL   bTermRevr;
extern BOOL   bTermInsert;
extern BOOL   bTermAltKey;
extern BOOL   bTermAltCur;
extern BOOL   bTermLckKey;
// dca00033 MKL added Auto keyborad repeat ON/OFF capability
extern BOOL   bTermNoAutoRepeat;
// dca00035 MKL added VT102 Cursor enable/disable capability
extern BOOL   bCaretDisabled;

extern char   cTermLEDs;
extern char   fTermVT52Mode;
extern char   fTermGraphicsOn;

extern long   ChrAttrColor[10];


/*---------------------------------------------------------------------------*/
/* Define character sets for G0 through G3				     */
/*---------------------------------------------------------------------------*/

#define TERM_CS_ASCII	    0
#define TERM_CS_DECSUP	    1
#define TERM_CS_DECGPH	    2
#define TERM_CS_BRITISH     3
#define TERM_CS_DUTCH	    4
#define TERM_CS_FINNISH     5
#define TERM_CS_FRENCH	    6
#define TERM_CS_CANADIAN    7
#define TERM_CS_GERMAN	    8
#define TERM_CS_ITALIAN     9
#define TERM_CS_DANISH	   10
#define TERM_CS_SPANISH    11
#define TERM_CS_SWEDISH    12
#define TERM_CS_SWISS	   13


extern BOOL   bTermAutoPrint;	/* Auto Print Switch			   */
extern BOOL   bTermCntlPrint;	/* Media Copy (Printer Controller On)	   */
extern BOOL   bTermModePrint;	/* Print full screen / print margin area   */
extern BOOL   bTermFeedPrint;	/* Add form feed to print screen	   */


extern int    nTermCS[4];

extern char   cTermFontAttr;    /* Last Character's Attributes         */
extern char   cTermFontColor;   /* Last Character's Color              */

extern int    nTermMaxCol;	/* Width of logical screen, 80 or 130  */

extern int    nTermOffX;	/* Screen scroll X character offset    */
extern int    nTermOffY;	/* Screen scroll Y character offset    */
extern int    nTermExtX;	/* Screen scroll X character extent    */
extern int    nTermExtY;	/* Screen scroll Y character extent    */
extern int    nTermWndX;
extern int    nTermWndY;
extern char   fTermPause;

extern char   bTermTabs[TERM_MAXCOL];

extern BOOL   bTermEscape;	/* Escape processing mode */
extern BYTE   cTermBuf[16];
extern int    nTermInx;

/*---------------------------------------------------------------------------*/
/* Define FONT variable used.						     */
/*---------------------------------------------------------------------------*/
extern HFONT   hVT102Fonts[16];

extern RECT    TermClient;


extern BOOL InitTerminal();
extern BOOL TermTerm();
extern void NewTerminalData();
extern void NEAR TermInput(PSTR, int);
extern int  TermNameScan(WORD);
extern void UpdateTerminal();
extern void UpdateNewTerminal();
extern void UpdateTerminalFont();
extern void SelectTerminal(char);

extern long WndVT102Proc(HWND, unsigned, WORD, LONG);
extern long WndVT52Proc(HWND, unsigned, WORD, LONG);
extern long WndIBM3101Proc(HWND, unsigned, WORD, LONG);
extern long WndPCIBMProc(HWND, unsigned, WORD, LONG);
extern long WndVidtexProc(HWND, unsigned, WORD, LONG);
extern long CisWndProc(HWND, unsigned, WORD, LONG);

extern BOOL EmulatorVT102Key(LPMSG);
extern BOOL EmulatorVT52Key(LPMSG);
extern BOOL EmulatorIBM3101Key(LPMSG);
extern BOOL EmulatorPCIBMKey(LPMSG);
extern BOOL EmulatorVidtexKey(LPMSG);

extern void TermWriteData(char *, int);
extern void TermWriteByte(int);
extern void TermWriteNewLine();
extern void TermWriteClear();
extern void TermMessage(short sMsgNo1, short sErrNo);

extern void LineToTerminal(char *);
extern void CharToTerminal(int);


/*----------------------------------------------------------------------------*/
/*  Define the Video Screen logic's variables and extern definitions          */
/*----------------------------------------------------------------------------*/

#define VID_MAXROW  24
#define VID_MAXCOL  132

#define VID_TYPE_DWID	    0x01
#define VID_TYPE_DTOP	    0x02
#define VID_TYPE_DBOT	    0x04

typedef int * PINT;

extern PINT pPriVidLine[VID_MAXROW];

extern int  InitVideo();
extern int  TermVideo();
extern PINT GetVidLine(int);
extern void VidScrollUp(int, int);
extern void VidScrollDown(int, int);
extern void VidPageUp();
extern void VidPageDown();
extern void VidLineUp();
extern void VidLineDown();
extern void VidInsertChars(int);
extern void VidDeleteChars(int);
extern void VidClearPage();
extern void VidClearArea(int, int, int, int);
extern void VidUpdateSize();
extern void VidSeeMe(int);
extern void VidScroll(unsigned, WORD, LONG);
extern void VidSelectArea(int, int, int, int);
extern void VidSetCaretPos();
extern void VidCreateCaret(HWND);
extern void VidDeleteCaret(HWND);
extern void VidClickAndPick(int, int);

extern void VT102Init();
extern void VT102Term();
extern void VT102CreateFonts();
extern void VT102DeleteFonts();
extern void VT102UpdateFonts();

extern void near VT102CharOut(int);
extern void near VT102CharDbl(int);

extern void VT52Init();
extern void VT52Term();
extern void IBM3101Init();
extern void IBM3101Term();
extern void PCIBMInit();
extern void PCIBMTerm();
extern void VidtexInit();
extern void VidtexTerm();

extern void TermNewBackground();
extern void NEAR TermClearArea(int, int, int, int);
extern void NEAR TermReset();
extern void TermSetup();

/*---------------------------------------------------------------------------*/
/* Define Keyboard Lockout Varible and Routines 			     */
/*---------------------------------------------------------------------------*/
extern	char	fCapsLock;
extern	char	fNumsLock;
extern	char	fPauseLock;
extern	char	fScrollLock;

/*---------------------------------------------------------------------------*/
/* Define Mouse Character Select Varibles and Routines			     */
/*---------------------------------------------------------------------------*/
extern	char fMouseDown;
extern	char fSelectedArea;
extern	int  nSelectX1, nSelectY1;
extern	int  nSelectX2, nSelectY2;
extern	int  nMouseAtX, nMouseAtY;

extern	char fMouseCursor;
extern	int  nCursorToX, nCursorToY;
extern	int  nCursorAtX, nCursorAtY;

extern void MouseSelectMsg(HWND, unsigned, WORD, LONG);
extern void MouseSelectClr();

/*---------------------------------------------------------------------------*/
/* Dialog Variables and Routines					     */
/*---------------------------------------------------------------------------*/

extern  void EditTerminalParams(char *);
extern  void EditFontParams(char *);

/*---------------------------------------------------------------------------*/
/* Define Variables used for Keyboard identifier save logic		     */
/*---------------------------------------------------------------------------*/
extern char sTermFileName[16];
extern char sTermWorkName[16];
extern int  nTermWorkInx;

/*---------------------------------------------------------------------------*/
/* Define Variable to support Zmodem					     */
/*---------------------------------------------------------------------------*/
extern char cZmodemSeq;


extern int  nCurEmulator;


struct PCIBM
  {
  unsigned char  xxx;
  };

struct VIDTEX
  {
  unsigned char  dle;
  unsigned char  trm;
  };

union TERMINAL
  {
  struct PCIBM	pcibm;
  struct VIDTEX vidtex;
  };

extern union TERMINAL terminal;
