/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Terminal Include File (TERM.H)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

#define TERM_MAXROW	      24
#define TERM_MAXCOL           80
#define TERM_MEMORY         3864
#define TERM_LINE            161

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

#define TERM_CURSOR_BAR     1
#define TERM_CURSOR_LINE    2
#define TERM_CURSOR_BLOCK   3

struct stTerm
  {
  char cTerminal;
  char filler1;
  int  nColumns;
  char cCurShape;
  char fLfAuto;
  char fNewLine;
  char fAutoWrap;
  char fEcho;
  char fEightBit;
  char fNoColor;
  char fFormFeed;
  char fPrintExtent;
  char fSwapDel;
  char fWindowWrap;
  char szEnter[8];
  char cTabWidth;
  char szAnswerback[64];
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
extern char   cTermLEDs;
extern char   fTermVT52Mode;
extern char   fTermGraphicsOn;

extern long   ChrAttrColor[10];


extern BOOL   bTermAutoPrint;	/* Auto Print Switch			   */
extern BOOL   bTermCntlPrint;	/* Media Copy (Printer Controller On)	   */
extern BOOL   bTermModePrint;	/* Print full screen / print margin area   */
extern BOOL   bTermFeedPrint;	/* Add form feed to print screen	   */


extern int    nTermCS[4];

extern char   cTermFontAttr;    /* Last Character's Attributes         */
extern char   cTermFontColor;   /* Last Character's Color              */

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


extern BOOL FAR  PASCAL InitTerminal();
extern BOOL FAR  PASCAL TermTerminal();
extern void FAR  PASCAL NewTerminalData();
extern void NEAR PASCAL TermInput(PSTR, int);
extern void far  pascal UpdateTerminal();
extern void far  pascal UpdateTerminalFont();
extern void far  pascal SelectTerminal(char);

extern long far  pascal TermWndProc(HWND, unsigned, WORD, LONG);

extern void far  pascal LineToTerminal(char *);
extern void far  pascal CharToTerminal(int);


/*----------------------------------------------------------------------------*/
/*  Define the Video Screen logic's variables and extern definitions          */
/*----------------------------------------------------------------------------*/

#define VID_MAXROW  24
#define VID_MAXCOL  80

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
extern void VidDeleteChars(void);
extern void VidClearPage();
extern void VidClearArea(int, int, int, int);
extern void VidUpdateSize();
extern void VidSeeMe(int);
extern void VidScroll(unsigned, WORD, LONG);
extern void VidSelectArea(int, int, int, int);
extern void VidSetCaretPos();
extern void VidCreateCaret(HWND);
extern void VidDeleteCaret(HWND);
extern void VidClickAndPick(int, int, WORD);

extern void far  pascal TermInit();
extern void far  pascal TermTerm();

extern void FAR  PASCAL TermNewBackground();
extern void NEAR PASCAL TermClearArea(int, int, int, int);
extern void NEAR PASCAL TermReset();
extern void FAR  PASCAL TermSetup();

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

extern void MouseSelectMsg(HWND, unsigned, WORD, LONG);
extern void MouseSelectClr();

/*---------------------------------------------------------------------------*/
/* Dialog Variables and Routines					     */
/*---------------------------------------------------------------------------*/

extern  void EditTerminalParams(char *);
extern  void EditFontParams(char *);

extern void  TermWriteData(PBYTE, SHORT, BOOL);
extern short TermWriteFunction(WORD, DWORD);
extern void  TermWriteKeyboard(WORD, WORD);
extern int   TermQueryArgument(void);
extern int   TermQuerySelection(char *);
extern int   TermQueryLine(int, char *);
extern int   TermQueryLength(int);
extern void  TermQueryInput(char *);

extern BOOL fTermInsert;
extern BOOL fTermNoDisplay;
extern BOOL fTermPrinting;
extern BOOL fTermCancel;
