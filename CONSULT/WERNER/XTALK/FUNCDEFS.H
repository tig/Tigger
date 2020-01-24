/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*                                                                             *
*  Project:   Crosstalk for Windows.                                   	   *
*                                                                             *
*   Module:   Function Prototypes.  (From original XTALK.H).			       *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 4.01 and compiled with Microsoft C 5.1.                     *
*                                                                             *
*  Revisions: 03/30/90 Initial Version.                                       *
*                                                                             *
\*****************************************************************************/

  extern void CmdRunApp();
  extern void CmdNew();
  extern void CmdOpen();
  extern void CmdSave();
  extern void CmdSaveAs();
  extern int  CmdCapture(char *, int);
  extern void CmdUpload();
  extern void CmdPageSetup();
  extern void CmdPrint();
  extern void CmdPrintSetup();
  extern void CmdSend();
  extern void CmdReceive();
  extern void CmdAbout();
  extern void CmdExit();
  extern void CmdCopy();
  extern void CmdCopyFile();
  extern void CmdCopyCapture();
  extern void CmdCopyNotes();
  extern void CmdCopyPrinter();
  extern void CmdCopySend();
  extern void CmdPaste();
  extern void CmdPasteUpload();
  extern void CmdConnect();
  extern void CmdDial();
  extern void CmdExecute();
  extern void CmdNotes();
  extern void CmdKermit();
  extern void CmdPlayback();
  extern void CmdBreak();
  extern void CmdSession();
  extern void CmdCommunications();
  extern void CmdModem();
  extern void CmdTerminal();
  extern void CmdProtocol();
  extern void CmdAscii();
  extern void CmdMacros();
  extern void CmdDisplay();
  extern void CmdFonts();
  extern void CmdSystem();
  extern void CmdHelpIndex();
  extern void CmdHelpTopic();

extern void SetSystemStatus(WORD);
extern void ResetSystemStatus(WORD);
extern void SystemUpdate();

extern long WndMainProc(HWND, unsigned, WORD, LONG);
extern long WndStatProc(HWND, unsigned, WORD, LONG);
extern long WndInfoProc(HWND, unsigned, WORD, LONG);
extern long WndMacroProc(HWND, unsigned, WORD, LONG);
extern long WndDialProc(HWND, unsigned, WORD, LONG);
extern BOOL FirstCopyInit(HANDLE);
extern BOOL AnotherCopyInit(HANDLE, HANDLE);
extern BOOL InstanceInit(HANDLE, LPSTR, int);
extern void ContinueInit();
extern BOOL DlgAbout(HWND, unsigned, WORD, LONG);
extern BOOL DlgRunApp(HWND, unsigned, WORD, LONG);

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

extern void WndPaint(HWND);
extern BOOL DialCurrent();
extern void UpdateTitle();
extern void ProgramAbort();
extern void ProgramWrapup();

extern void LinkDialog(HWND);
extern void UnLinkDialog(HWND);

extern BOOL TermMacroCheck(unsigned, WORD, char *);

extern BOOL CreateStatusChild(HWND);
extern BOOL DestroyStatusChild();

extern BOOL CreateInfoChild(HWND);
extern BOOL DestroyInfoChild();
extern void InfoDisplay(char *);
extern void InfoDisplayMsg(int);

extern BOOL CreateMacroChild(HWND);
extern BOOL DestroyMacroChild();
extern BOOL ActivateMacro(int);

extern BOOL CreateInputChild(HWND);
extern BOOL DestroyInputChild();

extern HWND hDlgDialing;
extern BOOL DlgDialing(HWND, unsigned, WORD, LONG);
extern void DialNumber();
extern void DialTerm();

extern BOOL CreateTerminalChild(HWND);
extern BOOL DestroyTerminalChild();
extern BOOL TerminalVirtualKey(LPMSG);

extern void ToggleNotes(void);

extern BOOL InitFont();
extern BOOL FontTerm();
extern BOOL FontSelect(int, int, BYTE);
extern BOOL FontNewData();

extern void IconTextChanged(char *);

extern int  ExecuteScript(char *);
extern int  ExecuteCloseScript();


  extern int  XferXModemSendName(int, char *);
  extern int  XferXModemRecvName(int, char *);
  extern int  XferXModemSendFile(int, int);
  extern int  XferXModemRecvFile(int, int);
  extern void XferXModemTerm();
  extern BOOL DlgXModem(HWND, unsigned, WORD, LONG);

  extern int  XferYModemSendName(char *);
  extern int  XferYModemRecvName(char *);
  extern int  XferYModemGSendName(char *, int);
  extern int  XferYModemGRecvName(char *, int);

  extern int  XferASCIIUploadName(char *);
  extern int  XferASCIIUploadFile(int);
  extern void XferASCIITerm();
  extern BOOL DlgASCII(HWND, unsigned, WORD, LONG);

extern void TogglePrinter();
extern void PrintChar(char, int);
extern void PrintLine(char *);
extern BOOL FlushPrinter();
extern BOOL OpenPrinter();
extern BOOL ClosePrinter();
extern BOOL InitPrinter();
extern BOOL TermPrinter();

extern void SetDlgItemStr(HWND, int, int);

extern int   TranslateToken(int, char *);
extern void  ResetScreenDisplay();
extern short AskUserMessageStr(short, char *);
extern short TellUserMessageStr(short, char *);
extern void  ErrorMessage(int);
extern int   WarningMessage(int);
extern int   CancelMessage(int);
extern void  InfoMessage(int);
extern void  FatalError(int);
extern void   GeneralError(int);
extern USHORT ErrorToMsg(USHORT usError);

extern void EditScript(char *);


/*------ dlgsave.c ------*/
extern int FAR DlgSaveAs (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *, BOOL, BOOL);
extern BOOL DlgFnSaveAs (HWND, unsigned, WORD, LONG);
extern VOID DlgInitSaveAs (HWND, OFSTRUCT *);
extern char * FAR DlgPFileInPath (char *);
extern DlgAddExt (char *);
extern BOOL FAR DlgMergeStrings (char *, char *, char *);


/*------ dlgopen.c -------*/
extern int FAR DlgOpen (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *, BOOL, BOOL);
extern int DlgFnOpen (HWND, unsigned, WORD, LONG);
extern VOID FAR DlgCheckOkEnable (HWND, int, unsigned);
extern DlgAddCorrectExtension (char *, BOOL);
extern USHORT DlgCheckFilename(char *pch);
extern BOOL DlgSearchSpec (char *);
extern int FAR DlgSend (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *);
extern int DlgFnSend (HWND, unsigned, WORD, LONG);

/*------ tools.asm -------*/
extern LPSTR lstrcpy(LPSTR, LPSTR);
extern LPSTR lstrcat(LPSTR, LPSTR);
extern int   lstrlen(LPSTR);
