/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*  Include: Main Include File (WINLEXIS.H)                                   */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/******************************************************************************/

//#define PENELOPE
//#define HYPERROM

#include "dosos2.h"

#define MAX_PATH	128


#define IDM_MENU		10

#define CMD_NEW                 16
#define CMD_OPEN                17
#define CMD_CLOSE               18
#define CMD_PRINT               19
#define CMD_RECORDING           20
#define CMD_DIARY               21
#define CMD_PRINTER             22
#define CMD_SETUP               23
#define CMD_PAGESETUP           24
#define CMD_PRINTSETUP          25
#define CMD_HYPERROM            26
#define CMD_SIGNON              27
#define CMD_EXIT                28
#define CMD_COPY                29
#define CMD_COPYDIARY           30
#define CMD_PASTE               31
#define CMD_NEWSEARCH           32
#define CMD_SEARCHMODIFY        33
#define CMD_SEARCHLEVEL         34
#define CMD_LIBRARY             35
#define CMD_FILE                36
#define CMD_CLIENT              37
#define CMD_ECLIPSE             38
#define CMD_ECLIPSERECALL       39
#define CMD_SEARCHDISPLAY       40
#define CMD_STOP                41
#define CMD_NEXTPAGE            42
#define CMD_PREVPAGE            43
#define CMD_NEXTDOC             44
#define CMD_PREVDOC             45
#define CMD_GOTOPAGE            46
#define CMD_GOTODOC             47
#define CMD_FIRSTPAGE           48
#define CMD_FIRSTDOC            49
#define CMD_KWIC                50
#define CMD_FULL                51
#define CMD_CITE                52
#define CMD_VARKWIC             53
#define CMD_SEGMENTS            54
#define CMD_SEQUENCE            55
#define CMD_ENTERSTAR           56
#define CMD_EXITSTAR            57
#define CMD_FONTS               58
#define CMD_AUTOCITE            59
#define CMD_SHEPARD             60
#define CMD_LEXSTAT             61
#define CMD_LEXSEE              62
#define CMD_BROWSE              63
#define CMD_RESUME              64
#define CMD_SELECTSERVICE       65
#define CMD_EXITSERVICE         66
#define CMD_PRINTSCREEN         67
#define CMD_PRINTDOC            68
#define CMD_PRINTALL            69
#define CMD_TIME                70
#define CMD_REQUEST             71
#define CMD_PAGES               72
#define CMD_DEBUT               73
#define CMD_GENERAL             74
#define CMD_TUTORIAL            75
#define CMD_LEXISHELP           76
#define CMD_ABOUT               77

  extern void CmdCapture(char *);
  extern void CmdUpload();
  extern void CmdSend();
  extern void CmdReceive();
  extern void CmdCopyPrinter(BOOL fFlush);
  extern void CmdCopyRecording();
  extern void CmdCopySend();
  extern void CmdCopyFile();
  extern void CmdCopyCapture();
  extern void CmdPasteUpload();
  extern void CmdBreak();

  extern void   CmdNew();
  extern void   CmdOpen();
  extern void   CmdClose();
  extern void   CmdPageSetup();
  extern void   CmdPrint();
  extern void   CmdPrintSetup();
  extern void   CmdHyperRom(void);
  extern void   CmdPrinter();
  extern void   CmdDiary();
  extern void   CmdRecord();
  extern USHORT CmdSignon();
  extern void CmdExit();
  extern void CmdCopy();
  extern void CmdCopyDiary();
  extern void CmdPaste();
  extern void CmdSetup();
  extern void CmdVarKwic();
  extern void CmdFonts();
  extern void CmdNextPage(BOOL);
  extern void CmdNextDoc(BOOL);
  extern void CmdPrevPage(BOOL);
  extern void CmdPrevDoc(BOOL);
  extern void CmdGotoPage();
  extern void CmdGotoDoc();
  extern void CmdSearchModify();
  extern void CmdSearchDisplay();
  extern void CmdSearchLevel();
  extern void CmdEclipse();
  extern void CmdEclipseRecall();
  extern void CmdPrintAll();
  extern void CmdAutoCite();
  extern void CmdShepard();
  extern void CmdLexstat();
  extern void CmdLexsee();
  extern void CmdBrowse();
  extern void CmdResume();
  extern void CmdSearch();
  extern void CmdNotepad();
  extern void CmdLexis();
  extern void CmdCommands();
  extern void CmdAbout();
  extern void CmdCitator();

  extern void SearchNewFile();
  extern void SearchOldFile();


#define IDD_ABORT               32
#define IDD_ABOUT               33
#define IDD_ALERT               34
#define IDD_ALTERNATE           35
#define IDD_AUTOCITE            36
#define IDD_CITATOR             37
#define IDD_CLOSE_FILE          38
#define IDD_CONNECT             39
#define IDD_DISCONNECT          40
#define IDD_DISPLAY             41
#define IDD_FONTS               42
#define IDD_GOTO_PAGE           43
#define IDD_GOTO_DOC            44
#define IDD_HELP1               45
#define IDD_HELP2               46
#define IDD_DIARY               47
#define IDD_INTRO               48
#define IDD_LEXSEE              49
#define IDD_LEXSTAT             50
#define IDD_NEW_DOCUMENT        51
#define IDD_NEW_FILE            52
#define IDD_NEW_DIARY           53
#define IDD_NEW_NOTEPAD         54
#define IDD_NEW_RECORD          55
#define IDD_NEW_SEARCH          56
#define IDD_NEXTDOC             57
#define IDD_NEXTPAGE            58
#define IDD_OPEN_FILE           59
#define IDD_OPEN_DIARY          60
#define IDD_OPEN_NOTEPAD        61
#define IDD_OPEN_PRINT          62
#define IDD_OPEN_RECORD         63
#define IDD_PAGESETUP           64
#define IDD_HYPERROM            65
#define IDD_PREVDOC             66
#define IDD_PREVPAGE            67
#define IDD_PRINT               68
#define IDD_PRINTALL            69
#define IDD_PRINTDOC            70
#define IDD_PRINTSETUP          71
#define IDD_REPORT              72
#define IDD_RESUME              73
#define IDD_SAVEAS              74
#define IDD_SEARCH_LEVEL        75
#define IDD_SEARCH_MODIFY       76
#define IDD_SETUP               77
#define IDD_SHEPARD             78
#define IDD_VARKWIC             79


#define IDI_ICON	    32
#define IDI_ACCEL           33

#define IDB_TITLE           32

#define USER		    256
#define IDU_HELP            1


#define     NOSAVE	    0	/* return flags for DlgFnSave() */
#define     NEWSAVE	    1
#define     OLDSAVE	    2

#define     NOOPEN	    0	/* return flags for DlgFnOpen() */
#define     NEWOPEN	    1
#define     OLDOPEN	    2

#define     MAX_STR_LEN     50
#define     MAX_FNAME_LEN   40
#define     CBEXTMAX        6   /* Number of bytes in "\*.ext" */

#define MAX_FNAME_STR	   64
#define ONESEC		 1000	/* 1000 ms equals one second */

#define COMM_TIMER          2

#define     FE_DBOPEN	    100
#define     FE_LMEM	    101
#define     FE_GMEM	    102


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


extern HANDLE hWndInst;
extern HWND   hWnd;
extern HMENU  hWndMenu;
extern HANDLE hAccelTable;
extern HWND   hProcessDlg;


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
#define SS_XFER 	0x0020
#define SS_DIRECT	0x0040
#define SS_DIAL 	0x0080

extern void SetSystemStatus(WORD);
extern void ResetSystemStatus(WORD);
#define IfSystemStatus(x) (wSystem & x)

extern void SystemUpdate();


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


/*----------------------------------------------------------------------------*/
/*  Define the various conditions that can exist                              */
/*----------------------------------------------------------------------------*/
extern BOOL bConnected; 	/* Connection is active, protect line.	    */
extern BOOL bPrinterOn; 	/* Copy all character input/output to print */

/*----------------------------------------------------------------------------*/
extern char szAppName[16];
extern char szAppTitle[40];
extern char szIFN[MAX_STR_LEN];
extern char szFNF[MAX_STR_LEN];
extern char szREF[MAX_STR_LEN];
extern char szSCC[MAX_STR_LEN];
extern char szEOF[MAX_STR_LEN];
extern char szECF[MAX_STR_LEN];
extern char szExt[MAX_STR_LEN];

extern long MainWndProc(HWND, unsigned, WORD, LONG);
extern long StatusWndProc(HWND, unsigned, WORD, LONG);
extern long InfoWndProc(HWND, unsigned, WORD, LONG);
extern long MacroWndProc(HWND, unsigned, WORD, LONG);
extern long DialWndProc(HWND, unsigned, WORD, LONG);
extern BOOL FirstCopyInit(HANDLE);
extern BOOL AnotherCopyInit(HANDLE, HANDLE);
extern BOOL InstanceInit(HANDLE, LPSTR, int);
extern void ContinueInit();
extern BOOL DlgAbout(HWND, unsigned, WORD, LONG);

extern void ProcessDDE(HWND, unsigned, WORD, LONG);
extern BOOL InitDDE();
extern void TermDDE();
extern int  StatusDDE(int);
extern int  InitiateDDE(char *, char *);
extern int  RequestDDE(int, char *, int);
extern int  ExecuteDDE(int, char *);
extern int  PokeDDE(int, char *, char *);
extern int  AdviseDDE(int, char *, int);
extern int  UnadviseDDE(int, char *);
extern int  TerminateDDE(int);
extern void ScriptEndDDE();
extern int  nDDESessions;

extern void  WndPaint(HWND);
extern BOOL  DialCurrent();
extern void  UpdateTitle();
extern void  ProgramAbort();
extern void  ProgramWrapup();
extern void  MenuPopupInit(WORD);
extern short ClipQuerySelection(char *);


/*----------------------------------------------------------------------------*/
/*  Support for the Status Child Window 				      */
/*----------------------------------------------------------------------------*/
extern BOOL CreateStatusChild(HWND);
extern BOOL DestroyStatusChild();
  extern HWND hChildStatus;

#define UM_DISCONNECT   WM_USER+16
#define UM_CLIENT       WM_USER+17
#define UM_LIB          WM_USER+18
#define UM_FILE         WM_USER+19
#define UM_SERVICE      WM_USER+20


/*----------------------------------------------------------------------------*/
/*  Support for the Info Child Window					      */
/*----------------------------------------------------------------------------*/
extern BOOL CreateInfoChild(HWND);
extern BOOL DestroyInfoChild();
extern void            InfoMessage(SHORT, PSTR);
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
extern HWND CreateMacroChild(HWND);
extern void DestroyMacroChild();
  extern HWND  hChildMacro;
  extern short sActiveMacro;
  extern short sActiveButton;
#define UM_ONLINE       WM_USER+50
#define UM_BUTTON       WM_USER+51
#define UM_EXECUTE      WM_USER+52
#define UM_MANUAL       WM_USER+53


/*----------------------------------------------------------------------------*/
/*  Support for Terminal Processing					      */
/*----------------------------------------------------------------------------*/
extern BOOL CreateTerminalChild(HWND);
extern BOOL DestroyTerminalChild();
extern BOOL TerminalVirtualKey(LPMSG);
  extern HWND hChildTerminal;
  extern int  nCurEmulator;

#define UM_RESET    WM_USER
#define UM_DATA     WM_USER+1
#define UM_NEWFONT  WM_USER+2


/*----------------------------------------------------------------------------*/
/*  Support for Font Processing 					      */
/*----------------------------------------------------------------------------*/
extern BOOL InitFont();
extern BOOL FontTerm();
extern BOOL FontSelect(int, int, BYTE);
extern BOOL FontNewData();

  extern HANDLE  hSysFont;
  extern short	 nSysFontWidth;
  extern short	 nSysFontHeight;

  extern HANDLE  hCurFont;
  extern short	 nCurFontWidth;
  extern short	 nCurFontHeight;
  extern BYTE	 bCurFontCharSet;

  extern char fVT102Font;

/*----------------------------------------------------------------------------*/
/*  Support for the Printer						      */
/*----------------------------------------------------------------------------*/
  extern HDC  hPrintDC;
  extern int  nPrintLineNo;
  extern int  nPrintPageNo;
  extern int  nPrintCharLn;
  extern BOOL fNoPrinter;

extern void PrinterOn();
extern void PrinterOff();
extern void TogglePrinter();
extern void PrintChar(char, int);
extern void PrintLine(char *);
extern void PrintWriteData(char *, int);
extern void PrintWriteNewline();
extern BOOL FlushPrinter();
extern BOOL OpenPrinter();
extern BOOL ClosePrinter();
extern BOOL InitPrinter();
extern BOOL TermPrinter();
extern BOOL PrintDocument();


/*----------------------------------------------------------------------------*/
/*  Misc. Routines							      */
/*----------------------------------------------------------------------------*/
extern void SetDlgItemStr(HWND, int, int);

extern int  TranslateToken(int, char *);
extern void ResetScreenDisplay();
extern void ErrorMessage(int);
extern int  WarningMessage(int);
extern void FatalError(int);
extern void GeneralError(int);
extern WORD MessageDisplay(int, int);

extern void EditScript(char *);
extern BOOL KeyDiskCheck(void);

/*------ dlgopen.c -------*/
extern int FAR DlgOpen (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *);
extern BOOL DlgSearchSpec(register char *sz);

/*------ tools.asm -------*/
extern LPSTR lstrcpy(LPSTR, LPSTR);
extern LPSTR lstrcat(LPSTR, LPSTR);
extern int   lstrlen(LPSTR);
extern VOID  _lclose(int);
extern char * _cdecl itoa(int, char *, int);
extern int  _cdecl atoi(char *);
