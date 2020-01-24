/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Database Include (DATA.H)					      */
/*									      */
/*   Author: Kent David Cedola						      */
/*									      */
/*   System: Microsoft Windows 2.1 using Microsoft C 5.1                      */
/*									      */
/*  Remarks: The database module initializes, reloads and saves various       */
/*	     variables.  It also maintains the profile and permits read and   */
/*	     write access to the profile through use of subroutines.  No much */
/*	     is really done, but we are very important.  All non-working      */
/*	     database variables are automatically save at termination.	      */
/*									      */
/******************************************************************************/


/******************************************************************************/
/*									      */
/*  Global Definitions							      */
/*									      */
/******************************************************************************/

#define DATA_COM1       0           /* Serial Port COM1                     */
#define DATA_COM2       1           /* Serial Port COM2                     */

#define DATA_EVEN       0           /* Even Parity value for wDataParity    */
#define DATA_ODD        1           /* Odd Parity value for wDataParity     */
#define DATA_NONE       2           /* No Parity value for wDataParity      */

#define DATA_TONE       0           /* Tone Dial value for wDataDial        */
#define DATA_PULSE      1           /* Pulse Dial value for wDataDial       */
#define DATA_MANUAL     2           /* Manual Dial value for wDataDial      */


/******************************************************************************/
/*									      */
/*  Database Variables (save and loaded from the profile)		      */
/*									      */
/******************************************************************************/

  extern char   szDataLogon[8];         /* Optional logon userid for access */
  extern char   szDataModem[33];        /* Name of modem that is being used */
  extern char   szDataNetwork[3][17];   /* Networks                         */
  extern char   szDataPrefix[3][25];    /* Dialing prefix for dialing out   */
  extern char   szDataNumber[3][33];    /* Number to dial on modems         */
  extern char   szDataNode[3][17];      /* Access node, used in scripts     */
  extern char   szDataFace[16];         /* Current Font's Facename          */
  extern WORD   fDataBell;              /* If true, activate bell sound     */
  extern WORD   fDataFastPrint;         /* If true, use FAST Print Doc      */
//extern WORD	fDataLexis2000; 	/* If true, use LEXIS 2000 keyboard */
  extern WORD   wDataPort;              /* Serial port to use               */
  extern WORD   fDataHardwire;          /* If true, hardwired connection    */
  extern WORD   fDataPBX;               /* If true, dialing thru PBX        */
  extern WORD   wDataDial;              /* 0 = TONE, 1 = PULSE, 3 = MANUAL  */
  extern WORD   wDataParity;            /* 0 = EVEN, 1 = ODD, 2 = NONE      */
  extern WORD   wDataSpeed[3];          /* Serial port speed                */
  extern WORD   wDataPitchFamily;       /* Current Font's Pitch and Family  */
  extern WORD   wDataCharset;           /* Current Font's Charset           */
  extern WORD   wDataHeight;            /* Current Font's Height            */
  extern WORD   wDataWidth;             /* Current Font's Width             */
  extern char   szDataDirRecord[64];    /* Directory of Recording files     */
  extern char   szDataDirNotepad[64];   /* Directory of Notepads files      */
  extern char   szDataDirMaps[64];      /* Directory of Research map files  */
  extern char   szDataDirDoc[64];       /* Directory of Document files      */
  extern char   szDataHeader[64];       /* Header of Printer Output         */
  extern char   szDataFooter[64];       /* Footer of Printer Output         */
  extern WORD	fDataDisplayStatus;	/* Display the status bar	    */
  extern WORD	fDataDisplayMacro;	/* Display the macro bar	    */
  extern WORD	fDataDisplayInfo;	/* Display the info bar 	    */

  extern char   szSearchClient[33];     /* Current Search Client            */
  extern char   szSearchDesc[65];       /* Current Search Description       */
  extern char   szSearchText[512];      /* Current Search Text              */
  extern char   szSearchLibrary[33];    /* Current Search Library           */
  extern char   szSearchFiles[33];      /* Current Search Files             */
  extern char   szSearchStartDate[9];   /* Current Search Starting Date     */
  extern char   szSearchEndDate[9];     /* Current Search Ending Date       */
  extern char   szSearchDisplay[17];    /* Current Search Display Format    */


/******************************************************************************/
/*									      */
/*  Global Variables  (not saved at end of session)			      */
/*									      */
/******************************************************************************/

  extern char   nWorkVersion;       /* Major version of windows             */
  extern BOOL   bWorkLogonSent;     /* True if the logon has been sent.     */
  extern BOOL   bWorkFastPrint;     /* True if Fast Print sent once.        */
  extern char   szWorkCWD[256];     /* Current Working Directory            */
  extern int    nWorkCDLostCount;   /* Times ok to lose Carrier Detect      */
  extern int    nWorkNetwork;       /* Current Network being used           */
  extern char   szWorkClient[40];   /* Current Client defined               */
  extern char	szWorkLibrary[129]; /* Current Library and File being used  */
  extern char   szWorkService[9];   /* Current Service being used           */
  extern char   szWorkRecord[64];   /* Current Recording file               */
  extern char   szWorkDiary[64];    /* Current Diary file                   */
  extern char   szWorkNotepad[64];  /* Current Notepad file                 */
  extern char	szWorkCite[201];    /* Last cite specified		    */
  extern int    nWorkLastFileType;  /* Value of the last file type used     */
  extern int    nWorkLastNextDoc;   /* Value of the last user's Next Doc    */
  extern int    nWorkLastNextPage;  /* Value of the last user's Next Page   */
  extern int    nWorkLastPrevDoc;   /* Value of the last user's Prev Doc    */
  extern int    nWorkLastPrevPage;  /* Value of the last user's Prev Page   */
  extern int    nWorkLastAutoCite;  /* Value of the last user's AutoCite    */
  extern int    nWorkLastVarKwic;   /* Value of the last user's VarKwic     */
  extern char   szWorkLastSTF[32];  /* Value of the last search file used   */
  extern int    nWorkLastPrint;     /* Value of the last print file type    */
  extern int    nWorkLastCopies;    /* Value of the last copies printed     */
  extern int    nWorkLastDraft;     /* Value of the last draft setting      */
  extern int    nWorkLastFirstPage; /* Value of the last first page printed */
  extern int    nWorkLastLastPage;  /* Value of the last last page printed  */
  extern BOOL   bWorkUserEnd;       /* User terminated Session Manager      */
  extern BOOL   bWorkPrintDoc;      /* If TRUE, send document to printer    */
  extern BOOL   bWorkStoreDoc;      /* If TRUE, send document to disk file  */
  extern int    hWorkDocument;      /* File handle of document disk file    */
  extern char   szWorkDocument[32]; /* File name of document disk file      */
  extern BOOL   fWorkHyperRom;      /* If TRUE, HyperRom option can be used */
  extern BOOL   fWorkHyperRomMode;  /* If TRUE, We are in HyperRom mode     */
  extern BOOL   fWorkHyperRomSwitch;/* If TRUE, We are in HyperRom mode     */
  extern char   szWorkHyperRom[16]; /* HyperRom userid from keydisk         */
  extern char   szWorkPassword[16]; /* HyperRom password from keydisk       */
  extern USHORT usWorkTimeout;      /* Idle timeout in seconds              */


/******************************************************************************/
/*									      */
/*  Global Tables  (not updated during execution)                             */
/*									      */
/******************************************************************************/

#define COMMAND_FIRST           0
#define COMMAND_AUTOCITE        0
#define COMMAND_AUTOCITE1       1
#define COMMAND_AUTOCITE2       2
#define COMMAND_BROWSE          3
#define COMMAND_CAI             4
#define COMMAND_CHANGEFILE      5
#define COMMAND_CHANGELIBRARY   6
#define COMMAND_CITE            7
#define COMMAND_CR              8
#define COMMAND_DEBUT           9
#define COMMAND_DISPDIF        10
#define COMMAND_ECLIPSE        11
#define COMMAND_ECLIPSERECALL  12
#define COMMAND_EXIT           13
#define COMMAND_EXITSERVICE    14
#define COMMAND_FASTPRINT1     15
#define COMMAND_FASTPRINT2     16
#define COMMAND_FIRSTDOC       17
#define COMMAND_FIRSTPAGE      18
#define COMMAND_FULL           19
#define COMMAND_GOTODOC        20
#define COMMAND_GOTOSTARPAGE   21
#define COMMAND_HELP           22
#define COMMAND_KWIC           23
#define COMMAND_LEXSEE         24
#define COMMAND_LEXSTAT        25
#define COMMAND_LXSTAT         26
#define COMMAND_MODIFY         27
#define COMMAND_NEWCLIENT      28
#define COMMAND_NEWSEARCH      29
#define COMMAND_NEXTDOC        30
#define COMMAND_NEXTPAGE       31
#define COMMAND_REQUEST        32
#define COMMAND_RESUME         33
#define COMMAND_RESUMELEXIS    34
#define COMMAND_RESUMEAUTOCITE 35
#define COMMAND_RESUMESHEPARD  36
#define COMMAND_RESUMELEXSEE   37
#define COMMAND_RESUMELXSTAT   38
#define COMMAND_PAGES          39
#define COMMAND_PASSWORD       40
#define COMMAND_PREVDOC        41
#define COMMAND_PREVPAGE       42
#define COMMAND_PRINTALL       43
#define COMMAND_PRINTDOC       44
#define COMMAND_PRINTSCREEN    45
#define COMMAND_SEGMENTS       46
#define COMMAND_SELECTSERVICE  47
#define COMMAND_SHEPARD        48
#define COMMAND_SHEPARD1       49
#define COMMAND_SHEPARD2       50
#define COMMAND_SIGNOFF        51
#define COMMAND_SIGNOFFP       52
#define COMMAND_SEARCHDISPLAY  53
#define COMMAND_SEQUENCE       54
#define COMMAND_STAR           55
#define COMMAND_STOP           56
#define COMMAND_TIME           57
#define COMMAND_TUTOR          58
#define COMMAND_VARKWIC        59
#define COMMAND_XSTAR          60
#define COMMAND_LAST           60

  typedef struct _CMDS
    {
    PSTR szCommand;
    PSTR szName;
    WORD fNumberParam : 1;
    } CMDS;

  extern CMDS Commands[];


/******************************************************************************/
/*									      */
/*  External Procedure's Prototypes                                           */
/*									      */
/******************************************************************************/

  extern BOOL  DataInit();
  extern BOOL  DataUpdate();
  extern void  DataTerm();
  extern SHORT DataQueryProfileInteger(WORD, WORD, WORD *);
  extern SHORT DataQueryProfileString(WORD, WORD, PSTR, WORD);
  extern SHORT DataQueryProfileKeyword(WORD, PSTR, PSTR, WORD);
  extern SHORT DataWriteProfileInteger(WORD, WORD, WORD);
  extern SHORT DataWriteProfileString(WORD, WORD, PSTR);
  extern SHORT DataLoadProfileScript(PSTR, HANDLE *);
