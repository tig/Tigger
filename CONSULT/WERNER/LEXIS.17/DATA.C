/******************************************************************************/
/*									      */
/*  Copyright (C)  Michael J. Werner and Associates  All Rights Reserved.     */
/*									      */
/*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows      */
/*									      */
/*   Module: Database Module (DATA.C)					      */
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

#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include "lexis.h"
#include "library.h"
#include "data.h"
#include "tokens.h"
#include "dialogs.h"


/******************************************************************************/
/*									      */
/*  Database Variables (save and loaded from the profile)		      */
/*									      */
/******************************************************************************/

  char   szDataLogon[8];        /* Optional logon userid for access         */
  char   szDataModem[33];       /* Name of modem that is being used         */
  char   szDataNetwork[3][17];  /* Networks                                 */
  char   szDataPrefix[3][25];   /* Dialing prefix for dialing out           */
  char   szDataNumber[3][33];   /* Number to dial on modems                 */
  char   szDataNode[3][17];     /* Access node, used in scripts             */
  char   szDataFace[16];        /* Current Font's Facename                  */
  WORD   fDataBell;             /* If true, activate bell sound             */
  WORD   fDataFastPrint;        /* If true, use FAST Print Doc              */
//WORD	 fDataLexis2000;	/* If true, use LEXIS 2000 keyboard	    */
  WORD   wDataPort;             /* Serial port to use                       */
  WORD   fDataHardwire;         /* If true, hardwired connection            */
  WORD   fDataPBX;              /* If true, dialing thru PBX                */
  WORD   wDataDial;             /* 0 = TONE, 1 = PULSE, 3 = MANUAL          */
  WORD   wDataParity;           /* 0 = EVEN, 1 = ODD, 2 = NONE              */
  WORD   wDataSpeed[3];         /* Serial port speed                        */
  WORD   wDataPitchFamily;      /* Current Font's Pitch and Family          */
  WORD   wDataCharset;          /* Current Font's Charset                   */
  WORD   wDataHeight;           /* Current Font's Height                    */
  WORD   wDataWidth;            /* Current Font's Width                     */
  char   szDataDirRecord[64];   /* Directory of Recording files             */
  char   szDataDirNotepad[64];  /* Directory of Notepads files              */
  char   szDataDirMaps[64];     /* Directory of Research map files          */
  char   szDataDirDoc[64];      /* Directory of Document files              */
  char   szDataHeader[64];      /* Header of Printer Output                 */
  char   szDataFooter[64];      /* Footer of Printer Output                 */
  WORD	 fDataDisplayStatus;	/* Display the status bar		    */
  WORD	 fDataDisplayMacro;	/* Display the macro bar		    */
  WORD	 fDataDisplayInfo;	/* Display the info bar 		    */

  char   szSearchClient[33];    /* Current Search Client                    */
  char   szSearchDesc[65];      /* Current Search Description               */
  char   szSearchText[512];     /* Current Search Text                      */
  char   szSearchLibrary[33];   /* Current Search Library                   */
  char   szSearchFiles[33];     /* Current Search Files                     */
  char   szSearchStartDate[9];  /* Current Search Starting Date             */
  char   szSearchEndDate[9];    /* Current Search Ending Date               */
  char   szSearchDisplay[17];   /* Current Search Display Format            */


/******************************************************************************/
/*									      */
/*  Global Variables  (not saved at end of session)			      */
/*									      */
/******************************************************************************/

  char   nWorkVersion;          /* Major version of windows                 */
  BOOL   bWorkLogonSent;        /* True if the logon has been sent.         */
  BOOL   bWorkFastPrint;        /* True if Fast Print sent once.            */
  char   szWorkCWD[256];        /* Current Working Directory                */
  int    nWorkCDLostCount;      /* Times ok to lose Carrier Detect          */
  int    nWorkNetwork;          /* Current Network being used               */
  char   szWorkClient[40];      /* Current Client defined (thus charged)    */
  char   szWorkLibrary[129];    /* Current Library and File being used      */
  char   szWorkService[9];      /* Current Service being used               */
  char   szWorkRecord[64];      /* Current Recording file                   */
  char   szWorkDiary[64];       /* Current Diary file                       */
  char   szWorkNotepad[64];     /* Current Notepad file                     */
  char	 szWorkCite[201];	/* Last cite specified			    */
  int    nWorkLastFileType;     /* Value of the last file type used         */
  int    nWorkLastNextDoc;      /* Value of the last user's Next Doc        */
  int    nWorkLastNextPage;     /* Value of the last user's Next Page       */
  int    nWorkLastPrevDoc;      /* Value of the last user's Prev Doc        */
  int    nWorkLastPrevPage;     /* Value of the last user's Prev Page       */
  int    nWorkLastAutoCite;     /* Value of the last user's Auto Cite       */
  int    nWorkLastVarKwic;      /* Value of the last user's Var Kwic        */
  char   szWorkLastSTF[32];     /* Value of the last search file used       */
  int    nWorkLastPrint;        /* Value of the last print file type        */
  int    nWorkLastCopies;       /* Value of the last copies printed         */
  int    nWorkLastDraft;        /* Value of the last draft setting          */
  int    nWorkLastFirstPage;    /* Value of the last first page printed     */
  int    nWorkLastLastPage;     /* Value of the last last page printed      */
  BOOL   bWorkPrintDoc;         /* If TRUE, send document to printer        */
  BOOL   bWorkStoreDoc;         /* If TRUE, send document to disk file      */
  int    hWorkDocument;         /* File handle of document disk file        */
  char   szWorkDocument[32];    /* File name of document disk file          */
  BOOL   fWorkHyperRom;         /* If TRUE, HyperRom option can be used     */
  BOOL   fWorkHyperRomMode;     /* If TRUE, We are in HyperRom mode         */
  BOOL   fWorkHyperRomSwitch;   /* If TRUE, We are in HyperRom mode         */
  char   szWorkHyperRom[16];    /* HyperRom userid from keydisk             */
  char   szWorkPassword[16];    /* HyperRom password from keydisk           */
  USHORT usWorkTimeout;         /* Idle timeout in seconds                  */


/******************************************************************************/
/*									      */
/*  Global Tables  (not updated)                                              */
/*									      */
/******************************************************************************/

  CMDS Commands[] =
    {
    "ac",               "Auto-Cite®", FALSE,
    ".ac",              "Auto-Cite®", FALSE,
    "autocite",         "Auto-Cite®", FALSE,
    "b",                "Browse", FALSE,
    ".ss;cai",          "CAI", FALSE,
    ".cf",              "Change File", FALSE,
    ".cl",              "Change Library", FALSE,
    ".ci",              "Cite", FALSE,
    "",                 "Transmit Only", FALSE,
    ".ss;debut",        "Debut", FALSE,
    ".dl",              "Display Diff Level", TRUE,
    ".ss;sav",          "ECLIPSE", FALSE,
    ".ss;rec",          "Recall ECLIPSE", FALSE,
    ".so;y",            "Sign Off and Close Window", FALSE,
    ".es",              "Exit Service", FALSE,
    ".pr;1;y;;",        "Print Document", FALSE,
    ".pr;y;;",          "Print Document", FALSE,
    ".fd",              "First Document", FALSE,
    ".fp",              "First Page", FALSE,
    ".fu",              "Full", FALSE,
    "",                 "Go to Document", TRUE,
    "P*",               "Go to Star Page", TRUE,
    "h",                "Local Help", FALSE,
    ".kw",              "KWIC", FALSE,
    "lexsee",           "LEXSEE", FALSE,
    "lexstat",          "LEXSTAT", FALSE,
    "lxstat",           "LEXSTAT", FALSE,
    "m",                "Modify", TRUE,
    "c",                "New Client", FALSE,
    ".ns",              "New Search", FALSE,
    ".nd",              "Next Document", TRUE,
    ".np",              "Next Page", TRUE,
    "r",                "Request", FALSE,
    "resume",           "Resume Prior", FALSE,
    "resume lexis",     "Resume LEXIS®", FALSE,
    "resume ac",        "Resume Auto-Cite®", FALSE,
    "resume shep",      "Resume Shepard's®", FALSE,
    "resume lexsee",    "Resume LEXSEE", FALSE,
    "resume lxstat",    "Resume LEXSTAT", FALSE,
    "p",                "Pages", FALSE,
    "",                 "(Personal Account ID)", FALSE,
    ".pd",              "Previous Document", TRUE,
    ".pp",              "Previous Page", TRUE,
    ".mi",              "Print All (Mail It)", FALSE,
    ".pr",              "Print Document", FALSE,
    ".sp",              "Print Screen", FALSE,
    ".se",              "Segments", FALSE,
    ".ss",              "Select Service", FALSE,
    "shep",             "Shepard's®", FALSE,
    "shepard",          "Shepard's®", FALSE,
    "shepards",         "Shepard's®", FALSE,
    ".so",              "Sign Off", FALSE,
    ".so;n",            "Sign Off, HyperRom mode", FALSE,
    "r;r",              "Display Search Request", FALSE,
    "s",                "Sequence", FALSE,
    "star",		"Enter Star Mode", TRUE,
    "\x1b=",            "Stop", FALSE,
    "t",                "Time", FALSE,
    ".ss;tutor",        "Tutor", FALSE,
    ".vk",              "Var KWIC", TRUE,
    "xstar",            "Exit Star Mode", FALSE
    };


/******************************************************************************/
/*									      */
/*  Local Constants							      */
/*									      */
/******************************************************************************/

#define MAX_BUFFER	512	/* Size of file buffer for our profile i/o  */


/******************************************************************************/
/*									      */
/*  Local Structures							      */
/*									      */
/******************************************************************************/

typedef struct
  {
  int  cnt;
  int  inx;
  int  file;
  BYTE buf[MAX_BUFFER];
  } PROFILE;


/******************************************************************************/
/*									      */
/*  Local Procedures Prototypes 					      */
/*									      */
/******************************************************************************/

  static SHORT OpenProfileInput(PROFILE *);
  static SHORT OpenProfileInput2(PROFILE *);
  static SHORT ReadProfileLine(PROFILE *, PSTR, WORD);
  static SHORT CloseProfileInput(PROFILE *);
  static SHORT OpenProfileOutput(PROFILE *);
  static SHORT WriteProfileLine(PROFILE *, PSTR);
  static SHORT CloseProfileOutput(PROFILE *);
  static BOOL  IfSectionLine(PSTR, PSTR);
  static BOOL  IfKeywordLine(PSTR, PSTR);
  static void  GetArgument(PSTR, PSTR, WORD);
  static BOOL  IfScriptHeader(PSTR, PSTR);
  static BOOL  IfScriptEnd(PSTR);


/******************************************************************************/
/*									      */
/*  Global Procedures							      */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  BOOL DataInit();                                                          */
/*									      */
/*  Called in the application's initialize routine so we can load all the     */
/*  parameters from our profile.                                              */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL DataInit()
  {
  int      hFile;
  USHORT   usBytes;
  OFSTRUCT Ofstruct;
  char	   szProfilePath[32];

  /*
  ** Check to see if we can find the LEXIS.CFG file.
  */
  LoadString(hWndInst, IDS_DATA_CONFIG, szProfilePath, sizeof(szProfilePath));
  if (OpenFile(szProfilePath, &Ofstruct, OF_EXIST) == -1)
    {
    ErrorMessage(IDS_MSG_NOCFGFILE);
    return (FALSE);
    }

  /*
  ** Check to see if we can find the LEXIS.INI file, if not creat it.
  */
  LoadString(hWndInst, IDS_DATA_PROFILE, szProfilePath, sizeof(szProfilePath));
  if (OpenFile(szProfilePath, &Ofstruct, OF_EXIST) == -1)
    {
    register i;

    if (!_dos_creat(szProfilePath, _A_NORMAL, &hFile))
      {
      _dos_write(hFile, "\r\n[DATA]\r\n\r\n", 12, &usBytes);
      DosClose(hFile);
      }
    else
      {
      ErrorMessage(IDS_MSG_BADINIFILE);
      return (FALSE);
      }
    }

  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_LOGON,   szDataLogon,   sizeof(szDataLogon));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_MODEM,   szDataModem,   sizeof(szDataModem));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NETWORK1, szDataNetwork[0], sizeof(szDataNetwork[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NETWORK2, szDataNetwork[1], sizeof(szDataNetwork[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NETWORK3, szDataNetwork[2], sizeof(szDataNetwork[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_PREFIX1,  szDataPrefix[0],  sizeof(szDataPrefix[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_PREFIX2,  szDataPrefix[1],  sizeof(szDataPrefix[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_PREFIX3,  szDataPrefix[2],  sizeof(szDataPrefix[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NUMBER1,  szDataNumber[0],  sizeof(szDataNumber[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NUMBER2,  szDataNumber[1],  sizeof(szDataNumber[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NUMBER3,  szDataNumber[2],  sizeof(szDataNumber[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NODE1,    szDataNode[0],    sizeof(szDataNode[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NODE2,    szDataNode[1],    sizeof(szDataNode[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NODE3,    szDataNode[2],    sizeof(szDataNode[0]));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_FACE,    szDataFace,    sizeof(szDataFace));

  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_BELL,      &fDataBell);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_FASTPRINT, &fDataFastPrint);
//DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_LEXIS2000, &fDataLexis2000);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_PORT,      &wDataPort);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_HARDWIRE,  &fDataHardwire);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_PBX,       &fDataPBX);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_DIAL,      &wDataDial);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_PARITY,    &wDataParity);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_SPEED1,    &wDataSpeed[0]);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_SPEED2,    &wDataSpeed[1]);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_SPEED3,    &wDataSpeed[2]);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_PITCH,     &wDataPitchFamily);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_CHARSET,   &wDataCharset);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_HEIGHT,    &wDataHeight);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_WIDTH,     &wDataWidth);

  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_STATUS, &fDataDisplayStatus);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_MACRO,  &fDataDisplayMacro);
  DataQueryProfileInteger(IDS_DATA_DATA, IDS_DATA_INFO,   &fDataDisplayInfo);

  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_HEADER,  szDataHeader,     sizeof(szDataHeader));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_FOOTER,  szDataFooter,     sizeof(szDataFooter));

  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_RECORD,  szDataDirRecord,  sizeof(szDataDirRecord));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_NOTEPAD, szDataDirNotepad, sizeof(szDataDirNotepad));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_MAPS,    szDataDirMaps,    sizeof(szDataDirMaps));
  DataQueryProfileString(IDS_DATA_DATA, IDS_DATA_DOC,     szDataDirDoc,     sizeof(szDataDirDoc));

  if (szDataDirRecord[0] == NULL)
    getcwd(szDataDirRecord, sizeof(szDataDirRecord));
  if (szDataDirNotepad[0] == NULL)
    getcwd(szDataDirNotepad, sizeof(szDataDirNotepad));
  if (szDataDirMaps[0] == NULL)
    getcwd(szDataDirMaps, sizeof(szDataDirMaps));
  if (szDataDirDoc[0] == NULL)
    getcwd(szDataDirDoc, sizeof(szDataDirDoc));

  getcwd(szWorkCWD, sizeof(szWorkCWD));
  bWorkFastPrint     = FALSE;
  nWorkCDLostCount   = 0;
  memset(szWorkClient, 0, sizeof(szWorkClient));
  memset(szWorkLibrary, 0, sizeof(szWorkLibrary));
  memset(szWorkRecord, 0, sizeof(szWorkRecord));
  memset(szWorkDiary, 0, sizeof(szWorkDiary));
  memset(szWorkNotepad, 0, sizeof(szWorkNotepad));
  memset(szWorkCite, 0, sizeof(szWorkCite));
  nWorkLastFileType  = 0;
  nWorkLastNextDoc   = 1;
  nWorkLastNextPage  = 1;
  nWorkLastPrevDoc   = 1;
  nWorkLastPrevPage  = 1;
  nWorkLastAutoCite  = 1;
  nWorkLastVarKwic   = 30;
  szWorkLastSTF[0]   = NULL;
  nWorkLastPrint     = 0;
  nWorkLastCopies    = 1;
  nWorkLastDraft     = 0;
  nWorkLastFirstPage = 1;
  nWorkLastLastPage  = 9999;
  bWorkPrintDoc      = FALSE;
  bWorkStoreDoc      = FALSE;
  hWorkDocument      = NULL;

  fWorkHyperRom         = FALSE;
  fWorkHyperRomMode     = FALSE;
  fWorkHyperRomSwitch   = FALSE;
  szWorkHyperRom[0]     = '\0';
  szWorkPassword[0]	= '\0';

  usWorkTimeout      = 15;

  return (TRUE);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  void DataUpdate()                                                         */
/*									      */
/*  Called to write out any changed database parameters to our profile.       */
/*									      */
/*----------------------------------------------------------------------------*/
BOOL DataUpdate()
{

  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_LOGON,    szDataLogon);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_MODEM,    szDataModem);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NETWORK1, szDataNetwork[0]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NETWORK2, szDataNetwork[1]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NETWORK3, szDataNetwork[2]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_PREFIX1,  szDataPrefix[0]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_PREFIX2,  szDataPrefix[1]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_PREFIX3,  szDataPrefix[2]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NUMBER1,  szDataNumber[0]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NUMBER2,  szDataNumber[1]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NUMBER3,  szDataNumber[2]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NODE1,    szDataNode[0]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NODE2,    szDataNode[1]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NODE3,    szDataNode[2]);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_FACE,     szDataFace);

  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_BELL,      fDataBell);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_FASTPRINT, fDataFastPrint);
//DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_LEXIS2000, fDataLexis2000);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_PORT,      wDataPort);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_HARDWIRE,  fDataHardwire);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_PBX,       fDataPBX);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_DIAL,      wDataDial);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_PARITY,    wDataParity);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_SPEED1,    wDataSpeed[0]);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_SPEED2,    wDataSpeed[1]);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_SPEED3,    wDataSpeed[2]);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_PITCH,     wDataPitchFamily);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_CHARSET,   wDataCharset);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_HEIGHT,    wDataHeight);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_WIDTH,     wDataWidth);

  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_STATUS, fDataDisplayStatus);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_MACRO,  fDataDisplayMacro);
  DataWriteProfileInteger(IDS_DATA_DATA, IDS_DATA_INFO,   fDataDisplayInfo);

  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_HEADER,  szDataHeader);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_FOOTER,  szDataFooter);

  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_RECORD,  szDataDirRecord);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_NOTEPAD, szDataDirNotepad);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_MAPS,    szDataDirMaps);
  DataWriteProfileString(IDS_DATA_DATA, IDS_DATA_DOC,     szDataDirDoc);

  return (TRUE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  void DataTerm()                                                           */
/*									      */
/*  Called during the application's wrapup routine.                           */
/*									      */
/*----------------------------------------------------------------------------*/
void DataTerm()
{

}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DataQueryProfileInteger(wSection, wKeyword, pInteger)               */
/*    WORD wSection;							      */
/*    WORD wKeyword;							      */
/*    WORD *pInteger;							      */
/*									      */
/*  Read the values of the specified keyword, wKeyword, within the specified  */
/*  section, wSection, and returns it as an integer.			      */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DataQueryProfileInteger(wSection, wKeyword, pInteger)
  WORD  wSection;
  WORD  wKeyword;
  WORD  *pInteger;
{
  char buf[32];
  register i;


  if (i = DataQueryProfileString(wSection, wKeyword, buf, sizeof(buf)))
    return (i);

  *pInteger = atoi(buf);

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DataQueryProfileString(wSection, wKeyword, pBuffer, wMax)           */
/*    WORD wSection;							      */
/*    WORD wKeyword;							      */
/*    PSTR pBuffer;							      */
/*    WORD wMax;							      */
/*									      */
/*  Read the values of the specified keyword, wKeyword, within the specified  */
/*  section, wSection, and returns it as an integer.			      */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DataQueryProfileString(wSection, wKeyword, pBuffer, wMax)
  WORD wSection;
  WORD wKeyword;
  PSTR pBuffer;
  WORD wMax;
{
  char szKeyword[32];

  LoadString(hWndInst, wKeyword, szKeyword, sizeof(szKeyword));

  return (DataQueryProfileKeyword(wSection, szKeyword, pBuffer, wMax));
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DataQueryProfileKeyword(wSection, szKeyword, pBuffer, wMax)         */
/*    WORD wSection;							      */
/*    PSTR szKeyword;                                                         */
/*    PSTR pBuffer;							      */
/*    WORD wMax;							      */
/*									      */
/*  Read the values of the specified keyword, szKeyword, within the specified */
/*  section, wSection, and returns it as an integer.			      */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DataQueryProfileKeyword(WORD wSection, PSTR szKeyword, PSTR pBuffer, WORD wMax)
  {
  PROFILE  PF;
  char     szSection[64], szLine[512];
  int      nFileNo;
  register i;


  *pBuffer = NULL;
  nFileNo  = 0;

nextfile:
  if (nFileNo > 1)
    return (i);

  if (nFileNo++ == 0)
    {
    if (i = OpenProfileInput(&PF))
      goto nextfile;
    }
  else
    {
    if (i = OpenProfileInput2(&PF))
      return (i);
    }

  LoadString(hWndInst, wSection, szSection, sizeof(szSection));

  while (1)
    {
    if (i = ReadProfileLine(&PF, szLine, sizeof(szLine)))
      goto nextfile;
    if (IfSectionLine(szLine, szSection))
      break;
    }

  if (szKeyword)
    {
    while (1)
      {
      if (i = ReadProfileLine(&PF, szLine, sizeof(szLine)))
        goto nextfile;
      if (IfKeywordLine(szLine, szKeyword))
        break;
      }

    GetArgument(szLine, pBuffer, wMax);
    }
  else
    {
    *(pBuffer+1) = NULL;
    while (wMax > 2)
      {
      register PSTR ptr;

      if (i = ReadProfileLine(&PF, szLine, sizeof(szLine)))
	return (i);

      ptr = szLine;
      while (*ptr && *ptr != '=')
	ptr++;

      if (*ptr == '=')
	{
	ptr = szLine;

	while (*ptr && *ptr == ' ')
	  ptr++;

        while (*ptr && wMax > 2 && *ptr != '=')
	  {
          *pBuffer++ = *ptr++;
	  wMax--;
	  }
        *pBuffer++ = NULL;
        *pBuffer = NULL;
        wMax--;
        }
      else
        break;
      }
    }

  return(CloseProfileInput(&PF));
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DataWriteProfileInteger(wSection, wKeyword, wNumber)		      */
/*    WORD wSection;							      */
/*    WORD wKeyword;							      */
/*    WORD wNumber;							      */
/*									      */
/*  Writes the value of wNumber to the parameter file (profile).	      */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DataWriteProfileInteger(wSection, wKeyword, wNumber)
  WORD  wSection;
  WORD  wKeyword;
  WORD  wNumber;
{
  char	szBuf[32];


  if (wNumber)
    itoa(wNumber, szBuf, 10);
  else
    szBuf[0] = NULL;

  return (DataWriteProfileString(wSection, wKeyword, szBuf));
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DataWriteProfileString(wSection, wKeyword, pBuffer)		      */
/*    WORD wSection;							      */
/*    WORD wKeyword;							      */
/*    PSTR pBuffer;							      */
/*									      */
/*  Writes the string specified by pBuffer to the parameter file (profile).   */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DataWriteProfileString(wSection, wKeyword, pBuffer)
  WORD wSection;
  WORD wKeyword;
  PSTR pBuffer;
{
  PROFILE  inpPF, outPF;
  char	   szInpProfile[32], szOutProfile[32];
  char     szSection[32], szKeyword[32], szLine[512];
  char     szBuf[512], szNull[1];
  register i;


  szNull[0] = NULL;

  /*
  **  Strip trailing blanks from the data.
  */
  i = strlen(pBuffer);
  while (i && pBuffer[i-1] == ' ')
    pBuffer[--i] = NULL;

  /*
  **  If the value that we are writing is already there, just return.
  */
  if (DataQueryProfileString(wSection, wKeyword, szLine, sizeof(szLine)) == 0)
    {
    if (strcmp(pBuffer, szLine) == 0)
      return (0);
    }

  if (i = OpenProfileInput(&inpPF))
    return (i);
  if (i = OpenProfileOutput(&outPF))
    {
    CloseProfileInput(&inpPF);
    return (i);
    }

  LoadString(hWndInst, wSection, szSection, sizeof(szSection));
  LoadString(hWndInst, wKeyword, szKeyword, sizeof(szKeyword));

  /*
  **  Read the current profile into the new working profile until we reach the
  **  the section that we are looking for.
  */
  while (1)
    {
    if (i = ReadProfileLine(&inpPF, szLine, sizeof(szLine)))
      {
      if (i != -1)
	{
	CloseProfileOutput(&outPF);
	return (i);
	}
      szLine[0] = '[';
      strcpy(&szLine[1], szSection);
      i = strlen(szLine);
      szLine[i++] = ']';
      szLine[i] = NULL;
      }

    if (*szLine == NULL)
      continue;


    if (*szLine == '[')
      WriteProfileLine(&outPF, szNull);

    if (i = WriteProfileLine(&outPF, szLine))
      {
      CloseProfileInput(&inpPF);
      return (i);
      }

    if (IfSectionLine(szLine, szSection))
      break;
    }

  /*
  **  Search for the keyword to be update, or just add on a blank line.
  */
  while (1)
    {
    if (i = ReadProfileLine(&inpPF, szLine, sizeof(szLine)))
      {
      if (i != -1)
	{
	CloseProfileOutput(&outPF);
	return (i);
	}
      break;
      }

    if (*szLine == NULL)
      break;
    if (szLine[0] == '[')
      break;
    if (IfKeywordLine(szLine, szKeyword))
      break;

    if (i = WriteProfileLine(&outPF, szLine))
      {
      CloseProfileInput(&inpPF);
      return (i);
      }
    }

  /*
  **  Write out the new or updated keyword.
  */
  szBuf[0] = ' ';
  szBuf[1] = ' ';
  strcpy(&szBuf[2], szKeyword);
  i = strlen(szBuf);
  szBuf[i++] = '=';
  strcpy(&szBuf[i], pBuffer);
  WriteProfileLine(&outPF, szBuf);

  if (*szLine == '[')
    {
    WriteProfileLine(&outPF, szNull);
    WriteProfileLine(&outPF, szLine);
    }

  /*
  **  Copy out the rest of the old profile into the new working profile.
  */
  while (1)
    {
    if (i = ReadProfileLine(&inpPF, szLine, sizeof(szLine)))
      break;

    if (*szLine == NULL)
      continue;

    if (stricmp(szLine, "[Networks]") == 0)
      break;

    if (*szLine == '[')
      WriteProfileLine(&outPF, szNull);

    if (i = WriteProfileLine(&outPF, szLine))
      {
      CloseProfileInput(&inpPF);
      return (i);
      }
    }

  CloseProfileInput(&inpPF);
  CloseProfileOutput(&outPF);

  LoadString(hWndInst, IDS_DATA_PROFILE,  szInpProfile, sizeof(szInpProfile));
  LoadString(hWndInst, IDS_DATA_WORKFILE, szOutProfile, sizeof(szOutProfile));

  remove(szInpProfile);

  if (i = rename(szOutProfile, szInpProfile))
    return (i);

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  SHORT DataLoadProfileScript(wScript, phTokens)                            */
/*    WORD   wScript;                                                         */
/*    HANDLE *phTokens;                                                       */
/*									      */
/*									      */
/*									      */
/*									      */
/*----------------------------------------------------------------------------*/
SHORT DataLoadProfileScript(szScript, phTokens)
  PSTR   szScript;
  HANDLE *phTokens;
{
  PROFILE  PF;
  HANDLE   hMemory;
  DWORD    dwMemory;
  LPSTR    lpMemory;
  WORD     wMemory;
  BYTE     bTokens[256];
  char     szSection[32], szLine[256];
  int      nFileNo;
  register i;


  *phTokens = NULL;
  nFileNo   = 0;

nextfile:
  if (nFileNo > 1)
    return (i);

  if (nFileNo++ == 0)
    {
    if (i = OpenProfileInput(&PF))
      goto nextfile;
    }
  else
    {
    if (i = OpenProfileInput2(&PF))
      return (i);
    }


  LoadString(hWndInst, IDS_DATA_SCRIPTS, szSection, sizeof(szSection));

  /*
  **  Search through our profile looking for the start of the scripts.
  */
  while (1)
    {
    if (i = ReadProfileLine(&PF, szLine, sizeof(szLine)))
      goto nextfile;
    if (IfSectionLine(szLine, szSection))
      break;
    }

  /*
  **  Search through the scripts looking for the one requested.
  */
  while (1)
    {
    if (i = ReadProfileLine(&PF, szLine, sizeof(szLine)))
      goto nextfile;

    if (IfScriptHeader(szLine, szScript))
      break;
    }

  /*
  **  Lets allocate a hunk of global memory and put a tokenize version of
  **  the script in it.
  */

  dwMemory = GlobalCompact(16L*1024L);
  if ((hMemory = GlobalAlloc(GMEM_MOVEABLE, dwMemory)) == 0)
    return (-1);

  lpMemory = GlobalLock(hMemory);
   wMemory = 0;

  while (1)
    {
    if (i = ReadProfileLine(&PF, szLine, sizeof(szLine)))
      {
      GlobalUnlock(hMemory);
      GlobalFree(hMemory);
      return (i);
      }

    if (i = TokenAsciiToTokens(szLine, bTokens, sizeof(bTokens)))
      {
      GlobalUnlock(hMemory);
      GlobalFree(hMemory);
      return (i);
      }

    wMemory += *(int *)(&bTokens[0]);
    if ((DWORD)wMemory > dwMemory)
      {
      GlobalUnlock(hMemory);
      GlobalFree(hMemory);
      return (i);
      }

    for (i = 0; i < *(int *)(&bTokens[0]); i++)
      *lpMemory++ = bTokens[i];

    if (IfScriptEnd(szLine))
      break;
    }

  if (wMemory == 0)
    {
    GlobalUnlock(hMemory);
    GlobalFree(hMemory);
    return (-1);
    }

  GlobalUnlock(hMemory);
  GlobalReAlloc(hMemory, (DWORD)wMemory, NULL);

  *phTokens = hMemory;

  CloseProfileInput(&PF);

  return(0);
}


/******************************************************************************/
/*									      */
/*  Local Procedures                                                          */
/*									      */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT OpenProfileInput(pPF)                                        */
/*									      */
/*  Open our own profile for input.                                           */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT OpenProfileInput(PROFILE *pPF)
  {
  OFSTRUCT Ofstruct;
  char	   szProfilePath[32];
  register i;


  LoadString(hWndInst, IDS_DATA_PROFILE, szProfilePath, sizeof(szProfilePath));
  pPF->file = OpenFile(szProfilePath, &Ofstruct, OF_READ);

  if (pPF->file == -1)
    return (-1);

  pPF->cnt  = 0;
  return (0);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT OpenProfileInput(pPF)                                        */
/*									      */
/*  Open our own profile for input.                                           */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT OpenProfileInput2(PROFILE *pPF)
  {
  OFSTRUCT Ofstruct;
  char    szProfilePath[32];
  register i;


  LoadString(hWndInst, IDS_DATA_CONFIG, szProfilePath, sizeof(szProfilePath));
  pPF->file = OpenFile(szProfilePath, &Ofstruct, OF_READ);

  if (pPF->file == -1)
    return (-1);

  pPF->cnt  = 0;
  return (0);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT ReadProfileLine(pPF, pBuffer, cBuffer)                       */
/*									      */
/*  Read the next line from the currently openned profile.                    */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT ReadProfileLine(pPF, pBuffer, cBuffer)
  PROFILE *pPF;
  PSTR	  pBuffer;
  WORD    cBuffer;
{
  register i;


  if (pPF->file == -1)
    return (-1);

  for (i = 0; i < cBuffer - 1; i++)
    {
    if (pPF->cnt == 0)
      {
      register i;

      if (i = _dos_read(pPF->file, pPF->buf, MAX_BUFFER, &(pPF->cnt)))
	{
	_dos_close(pPF->file);
        return (i);
	}

      if (pPF->cnt == 0)
	{
	_dos_close(pPF->file);
        return (-1);
	}

      pPF->inx = 0;
      }

    if (pPF->buf[pPF->inx] == '\r')
      {
      *pBuffer = NULL;
      pPF->inx++;
      pPF->cnt--;
      return (0);
      }

    if (isprint(pPF->buf[pPF->inx]))
      *pBuffer++ = pPF->buf[pPF->inx];

    pPF->inx++;
    pPF->cnt--;
    }

  *pBuffer = NULL;

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT CloseProfileInput(pPF)                                       */
/*									      */
/*  Closes the currently openned input profile.                               */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT CloseProfileInput(pPF)
  PROFILE *pPF;
{

  if (pPF->file == -1)
    return (0);

  return (_dos_close(pPF->file));
}

/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT OpenProfileOutput(pPF)                                       */
/*									      */
/*  Open our own profile for output.                                          */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT OpenProfileOutput(pPF)
  PROFILE *pPF;
{
  char    szProfilePath[32];
  register i;


  LoadString(hWndInst, IDS_DATA_WORKFILE, szProfilePath, sizeof(szProfilePath));
  if (i = _dos_creat(szProfilePath, _A_NORMAL, &(pPF->file)))
    return (i);

  pPF->inx = 0;

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT WriteProfileLine(pPF, pBuffer)                               */
/*									      */
/*  Read the next line from the currently openned profile.                    */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT WriteProfileLine(pPF, pBuffer)
  PROFILE *pPF;
  PSTR	  pBuffer;
{

  while (*pBuffer)
    {
    if (pPF->inx > MAX_BUFFER - 8)
      {
      register i;

      if (i = _dos_write(pPF->file, pPF->buf, pPF->inx, &(pPF->cnt)))
        return (i);
      pPF->inx = 0;
      }

    pPF->buf[pPF->inx++] = *pBuffer++;
    }

  pPF->buf[pPF->inx++] = '\r';
  pPF->buf[pPF->inx++] = '\n';

  return (0);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static SHORT CloseProfileOutput(pPF)                                      */
/*									      */
/*  Closes the currently openned output profile.                              */
/*									      */
/*----------------------------------------------------------------------------*/
static SHORT CloseProfileOutput(pPF)
  PROFILE *pPF;
{
  register i;

  if (pPF->inx)
    {
    if (i = _dos_write(pPF->file, pPF->buf, pPF->inx, &(pPF->cnt)))
      return (i);
    }

  return (_dos_close(pPF->file));
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static BOOL IfSectionLine(szLine, szSection)                              */
/*    PSTR szLine;                                                            */
/*    PSTR szSection;                                                         */
/*									      */
/*  Returns true if the szLine parameter is the section header specified in   */
/*  the szSection parameter.                                                  */
/*									      */
/*----------------------------------------------------------------------------*/
static BOOL IfSectionLine(szLine, szSection)
  PSTR szLine;
  PSTR szSection;
{
  register char *ptr;

  ptr = szLine;
  while (*ptr && *ptr != '[')
    ptr++;

  if (*ptr == '[')
    {
    ptr++;

    while (*ptr && *ptr != ']')
      {
      if (toupper(*ptr) != toupper(*szSection))
        return (FALSE);

      ptr++;
      szSection++;
      }

    return (TRUE);
    }

  return (FALSE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static BOOL IfKeywordLine(szLine, szKeyword)                              */
/*									      */
/*  Return true if the szLine parameter contains the keyword specified in the */
/*  szKeyword parameter.                                                      */
/*									      */
/*----------------------------------------------------------------------------*/
static BOOL IfKeywordLine(PSTR szLine, PSTR szKeyword)
  {
  register PSTR ptr;


  ptr = szLine;
  while (*ptr && *ptr == ' ')
    ptr++;

  while (*ptr && *ptr != '=')
    {
    if (toupper(*ptr) != toupper(*szKeyword))
      return (FALSE);

    ptr++;
    szKeyword++;
    }

  if ((*ptr == '\0' || *ptr == '=') && *szKeyword != '\0')
    return (FALSE);

  return (TRUE);
  }


/*----------------------------------------------------------------------------*/
/*									      */
/*  static void GetArgument(szLine, szArgument, wMax)                         */
/*									      */
/*									      */
/*----------------------------------------------------------------------------*/
static void GetArgument(szLine, szArgument, wMax)
  PSTR szLine;
  PSTR szArgument;
  WORD wMax;
{
  register PSTR ptr;

  ptr = szLine;
  while (*ptr && *ptr != '=')
    ptr++;

  if (*ptr == '=')
    {
    ptr++;

    while (*ptr && --wMax > 0)
      *szArgument++ = *ptr++;
    }

  *szArgument = NULL;
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static BOOL IfScriptHeader(szLine, szScript)                              */
/*									      */
/*  Return true if the szLine parameter contains the script subroutine given  */
/*  in the szScript parameter.                                                */
/*									      */
/*----------------------------------------------------------------------------*/
static BOOL IfScriptHeader(szLine, szScript)
  PSTR szLine;
  PSTR szScript;
{

  if (*szLine++ != ':')
    return (FALSE);

  while (*szLine && *szLine != ' ' && *szScript)
    {
    if (toupper(*szLine) != toupper(*szScript))
      return (FALSE);

    szLine++;
    szScript++;
    }

  if (*szScript == NULL)
    return (TRUE);

  return (FALSE);
}


/*----------------------------------------------------------------------------*/
/*									      */
/*  static BOOL IfScriptEnd(szLine)                                           */
/*									      */
/*  Return true if the szLine parameter contains the script end subroutine.   */
/*									      */
/*----------------------------------------------------------------------------*/
static BOOL IfScriptEnd(szLine)
  PSTR szLine;
{

  while (*szLine && *szLine == ' ')
    szLine++;

  if (*szLine++ != 'E')
    return (FALSE);
  if (*szLine++ != 'n')
    return (FALSE);
  if (*szLine++ != 'd')
    return (FALSE);

  if (*szLine != ' ' && *szLine != NULL)
    return (FALSE);

  return (TRUE);
}
