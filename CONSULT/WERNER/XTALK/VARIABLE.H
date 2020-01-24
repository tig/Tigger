/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Variable Include File (VARIABLE.H)                              *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*             1.1  04/03/90 dca00065 PJL Change "UNKNOWNUNIT" to "BADDRIVE"   *
*                                                                             *
\*****************************************************************************/

#define VAR_MACRO1	0
#define VAR_MACRO24    23
#define VAR_DIRXWP     24
#define VAR_DIRXWS     25
#define VAR_DIRFIL     26
#define VAR_ARGLINE    27


#define ERRCOM_FIRST	       0x0B01
#define ERRCOM_UNKNOWN	       0x0B01	/* 11-01 Unknown Var, func, or sym */
#define ERRCOM_BADCONSTANT     0x0B02	/* 11-02 Incorrectly formed constant */
#define ERRCOM_OPERATOR        0x0B03	/* 11-03 Operator out of place */
#define ERRCOM_OPERAND	       0x0B04	/* 11-04 Missing operand */
#define ERRCOM_EXPRESSION      0x0B05	/* 11-05 Missing expression */
#define ERRCOM_MISRPAR	       0x0B06	/* 11-06 Too many right paren */
#define ERRCOM_MISLPAR	       0x0B07	/* 11-07 Too many left paren */
#define ERRCOM_MISRPARFUNCT    0x0B08	/* 11-08 Missing close funct paren */
#define ERRCOM_MISLPARFUNCT    0x0B09	/* 11-09 Missing open funct paren */
#define ERRCOM_BADFUNCTION     0x0B0A	/* 11-10 Function does not have () */
#define ERRCOM_MISQUOTE        0x0B0B	/* 11-11 Missing terminating quote */
#define ERRCOM_MISWHILE        0x0B0C	/* 11-12 Wend without While */
#define ERRCOM_MISWEND	       0x0B0D	/* 11-13 While without Wend */
#define ERRCOM_MISREPEAT       0x0B0E	/* 11-14 Wend without While */
#define ERRCOM_MISUNITL        0x0B0F	/* 11-15 While without Wend */
#define ERRCOM_MISIF	       0x0B10	/* 11-16 Then without If */
#define ERRCOM_MISELSE	       0x0B11	/* 11-17 Else without Then (or If) */
#define ERRCOM_MISTHEN	       0x0B12	/* 11-18 If without Then */
#define ERRCOM_MK4KEYWORD      0x0B13	/* 11-19 Mk.4 Keyword not supported */
#define ERRCOM_FILEOPEN        0x0B14	/* 11-20 Can not read script file */
#define ERRCOM_MEMORY	       0x0B15	/* 11-21 Not enough global memory */
#define ERRCOM_STKOVR	       0x0B16	/* 11-22 Expression too complex */
#define ERRCOM_NOREAL	       0x0B17	/* 11-23 Real numbers not allowed */
#define ERRCOM_ARRAY	       0x0B18	/* 11-24 Arrays not supported */
#define ERRCOM_NOXWC	       0x0B19	/* 11-25 Unable to write .XWC file */
#define ERRCOM_EOF	       0x0B1A	/* 11-26 Unexpected end of file */
#define ERRCOM_DUPLABEL        0x0B1B   /* 11-27 Duplicate Label */
#define ERRCOM_USERABORT       0x0B1C   /* 11-28 User aborted compile */
#define ERRCOM_OUTOFRANGE      0x0B1D   /* 11-29 Value out of range */

#define ERRINT_FIRST	       0x0C01
#define ERRINT_BADCOMMAND      0x0C01	/* 12-01 Unknown command word */
#define ERRINT_SYMCOMMAND      0x0C02	/* 12-02 Symbol can't be command */
#define ERRINT_EXTRATEXT       0x0C03	/* 12-03 Extra text after command */
#define ERRINT_NEEDCMDINFO     0x0C04	/* 12-04 Command requires more info */
#define ERRINT_BADKEYWORD      0x0C05	/* 12-05 Unknown keyword */
#define ERRINT_NODEFAULT       0x0C06	/* 12-06 No default data type */
#define ERRINT_NORESERVE       0x0C07	/* 12-07 Can't use reserved words */
#define ERRINT_MIXEDKEYWORDS   0x0C08	/* 12-08 Miscombined keywords */
#define ERRINT_MISKEYWORD      0x0C09	/* 12-09 Missing keyword clause */
#define ERRINT_DUPKEYWORD      0x0C0A	/* 12-10 Duplicate keyword clause */
#define ERRINT_INVALIDREF      0x0C0B	/* 12-11 Invalid redefinition */
#define ERRINT_TOOCOMPLEX      0x0C0C	/* 12-12 Statement too complex */
#define ERRINT_MISVARIABLE     0x0C0D	/* 12-13 Bad or missing variable */
#define ERRINT_MISONOFF        0x0C0E	/* 12-14 Bad or missing on/off */
#define ERRINT_WANTNUMBER      0x0C0F	/* 12-15 Number expected */
#define ERRINT_BADOPEN	       0x0C10	/* 12-16 Incorrect syntax in open */
#define ERRINT_BADFILEMODE     0x0C11	/* 12-17 Unknown file mode */
#define ERRINT_BADLABEL        0x0C12	/* 12-18 Bad label name or value */
#define ERRINT_MISCOMMA        0x0C13	/* 12-19 Missing Comma */
#define ERRINT_MISEQ	       0x0C14	/* 12-20 Missing = */
#define ERRINT_WANTSIZE        0x0C15	/* 12-21 Coordnates or size expected */
#define ERRINT_WANTINTEGER     0x0C16	/* 12-22 Boolean or Integer expected */
#define ERRINT_WANTSTRING      0x0C17	/* 12-23 String expected */
#define ERRINT_BADVARTYPE      0x0C18	/* 12-24 Unknown variable type */
#define ERRINT_GETSTRING       0x0C19	/* 12-25 GET only reads into strings */
#define ERRINT_SWAPVARS        0x0C1A	/* 12-26 SWAP only swaps variables */
#define ERRINT_WANTEXP	       0x0C1B	/* 12-27 Expression needed to assign */
#define ERRINT_BADGOTOADDR     0x0C1C	/* 12-28 Invalidate Label */
#define ERRINT_BADTIME	       0x0C1D	/* 12-29 Bad Time Expression */
#define ERRINT_INTERNAL        0x0C1E	/* 12-30 Internal Failure */
#define ERRINT_BADQUIET        0x0C1F	/* 12-31 Invalid Quiet Value */
#define ERRINT_BADKEY	       0x0C20	/* 12-32 Invalid Key Value */
#define ERRINT_BADCOUNT        0x0C21	/* 12-33 Invalid Count Value */
#define ERRINT_BADWATCHPARM    0x0C22	/* 12-34 Invalid Watch Parameter */
#define ERRINT_BADWATCHSTR     0x0C23	/* 12-35 Invalid Watch String */
#define ERRINT_NOWATCHPARM     0x0C24	/* 12-36 No Watch Parameters Found */
#define ERRINT_BADWATCH        0x0C25	/* 12-37 Bad Watch Parameter */
#define ERRINT_BADWAIT	       0x0C26	/* 12-38 Bad Wait Parameter */
#define ERRINT_LONGSTRING      0x0C27	/* 12-39 String too long */
#define ERRINT_MIXTYPES        0x0C28	/* 12-40 Can not mix str and int */
#define ERRINT_NOVARDEF        0x0C29	/* 12-41 Variable not defined */
#define ERRINT_MEMORY	       0x0C2A	/* 12-42 Not enough global memory */
#define ERRINT_OBJECT	       0x0C2B	/* 12-43 Can not find object file */
#define ERRINT_BADEXPRESSION   0x0C2C	/* 12-44 Bad expression */
#define ERRINT_BADSTROP        0x0C2D	/* 12-45 Bad string operation */
#define ERRINT_MISPARAM        0x0C2E	/* 12-46 Missing function parameter */
#define ERRINT_NOLABEL	       0x0C2F	/* 12-47 Label not found */
#define ERRINT_TOOMANYGOSUBS   0x0C30	/* 12-48 GOSUBs nested too deeply */
#define ERRINT_VARIABLESPACE   0x0C31	/* 12-49 Variable space limit */
#define ERRINT_CMDCANCELLED    0x0C32	/* 12-50 Command cancelled */
#define ERRINT_GOSUBUNDER      0x0C33	/* 12-51 Return/Pop Underflow */
#define ERRINT_READONLY        0x0C34   /* 12-52 Read only variable */
#define ERRINT_NOTINDIALOG     0x0C35   /* 12-53 Keyword usable in dialog */
#define ERRINT_BADCONTROL      0x0C36   /* 12-54 Invalid dialog control item */
#define ERRINT_NOENDDIALOG     0x0C37   /* 12-55 No EndDialog found */
#define ERRINT_NOCONTROL       0x0C38   /* 12-56 No dialog controls found */
#define ERRINT_NOTONLINE       0x0C39   /* 12-57 Command only when online */

#define ERRIOE_FIRST	       0x0D01
#define ERRIOE_NOFILELIST      0x0D01	/* 13-01 Cmd doesn't do file list */
#define ERRIOE_UPLOADCANCEL    0x0D02	/* 13-02 Upload cancel by local op */
#define ERRIOE_MISFILELIST     0x0D03	/* 13-03 Missing or empty file list */
#define ERRIOE_NOBACKUP        0x0D04	/* 13-04 Can't create backup file */
#define ERRIOE_MISCHANNEL      0x0D05	/* 13-05 Invalid or missing ch # */
#define ERRIOE_CHANNELOPEN     0x0D06	/* 13-06 Channel # already open */
#define ERRIOE_CHANNELNOTOPEN  0x0D07	/* 13-07 Channel # not open */
#define ERRIOE_NOCANREAD       0x0D08	/* 13-08 Can't READ from output file */
#define ERRIOE_NOCANWRITE      0x0D09	/* 13-09 Can't WRITE to input file */
#define ERRIOE_NODOTEXT        0x0D0A	/* 13-10 Can't GET/PUT with txt file */
#define ERRIOE_NODORANDOM      0x0D0B	/* 13-11 Can't READ/WRITE with rnd */
#define ERRIOE_NOCANFINDFILE   0x0D0C	/* 13-12 File not found in path */

#define ERRATH_FIRST	       0x0E01
#define ERRATH_OVERFLOW        0x0E01	/* 14-01 Arithmetic overflow */
#define ERRATH_UNDERFLOW       0x0E02	/* 14-02 Arithmetic underflow */
#define ERRATH_DIVBYZERO       0x0E03	/* 14-03 Division by zero */
#define ERRATH_FKEYRANGE       0x0E04	/* 14-04 Function key out of range */
#define ERRATH_EXPNOTVALUE     0x0E05	/* 14-05 Expression not a value */
#define ERRATH_OUTOFRANGE      0x0E06	/* 14-06 Value out permissible rng */
#define ERRATH_NOTONOROFF      0x0E07	/* 14-07 Value must be ON or OFF */

#define ERRSYS_FIRST	       0x1001
#define ERRSYS_WRITEPROTECT    0x1001	/* 16-01 Device is write protected */
#define ERRSYS_BADDRIVE        0x1002	/* 16-02 Invalid drive (unknown unit) */
#define ERRSYS_DRIVENOTREADY   0x1003	/* 16-03 Drive not ready */
#define ERRSYS_UNKOWNCMD       0x1004	/* 16-04 Unknown command */
#define ERRSYS_DATAERROR       0x1005	/* 16-05 Data error */
#define ERRSYS_BADSTRUCTLEN    0x1006	/* 16-06 Bad request structure len */
#define ERRSYS_SEEKERROR       0x1007	/* 16-07 Seek error */
#define ERRSYS_MEDIATYPE       0x1008	/* 16-08 Unknown media type */
#define ERRSYS_NOSECTOR        0x1009	/* 16-09 Sector not found */
#define ERRSYS_OUTOFPAPER      0x100A	/* 16-10 Out of paper */
#define ERRSYS_WRITEFAULT      0x100B	/* 16-11 Write fault */
#define ERRSYS_READFAULT       0x100C	/* 16-12 Read fault */
#define ERRSYS_GENERALFAILURE  0x100D	/* 16-13 General failure */
#define ERRSYS_OPENFAULT       0x100E	/* 16-14 Open fault */
#define ERRSYS_NOMEMORY        0x100F   /* 16-15 No global memory */

#define ERRDOS_FIRST	       0x1301
#define ERRDOS_UNKNOWN	       0x1301	/* 19-01 Unexpected DOS error */
#define ERRDOS_NOFILE	       0x1302	/* 19-02 File not found */
#define ERRDOS_NOPATH	       0x1303	/* 19-03 Path not found */
#define ERRDOS_TOOMANYFILES    0x1304	/* 19-04 Too many open files */
#define ERRDOS_ACCESSDENIED    0x1305	/* 19-05 Access denied */
#define ERRDOS_NORMDIR	       0x1306	/* 19-06 Can't remove directory */
#define ERRDOS_WRITEPROTECTED  0x1307	/* 19-07 Diskette write protected */
#define ERRDOS_DISKFULL        0x1308	/* 19-08 Disk is full */
#define ERRDOS_BADFILENAME     0x1309	/* 19-09 Invalid chars in file name */

#define ERRDIA_FIRST	       0x1401
#define ERRDIA_CALLCANCEL      0x1401	/* 20-01 Call cancelled by user */
#define ERRDIA_NOANSWER        0x1402	/* 20-02 No answer */
#define ERRDIA_NODIALTONE      0x1403	/* 20-03 No dial tone */
#define ERRDIA_NUMBERBUSY      0x1404	/* 20-04 Number is busy */
#define ERRDIA_VOICE	       0x1405	/* 20-05 Voice answer */
#define ERRDIA_NONUMBER        0x1406	/* 20-06 Number required for connect */
#define ERRDIA_CONNECTED       0x1407	/* 20-07 Connection in progress */
#define ERRDIA_LOSTCONNECT     0x1408	/* 20-08 Lost connection */
#define ERRDIA_MODEMERROR      0x1409	/* 20-09 Modem Error */
#define ERRDIA_MODEMBAD        0x140A	/* 20-10 Modem not responding */

#define ERRMIS_FIRST	       0x1501
#define ERRMIS_SCRIPTFILE      0x1501	/* 21-01 Can't find script file */
#define ERRMIS_SETUPFILE       0x1502	/* 21-02 Can't find setup file */
#define ERRMIS_VARIABLE        0x1503	/* 21-03 Can't find variable */
#define ERRMIS_DEFAULTFILE     0x1504	/* 21-04 Default filename is empty */
#define ERRMIS_NOFILE	       0x1505	/* 21-05 Filename is missing */
#define ERRMIS_BADXWP          0x1506   /* 21-06 Bad parameter file */

#define ERRDDE_FIRST	       0x2001
#define ERRDDE_NOCHANNEL       0x2001	/* 32-01 Missing or no channels */
#define ERRDDE_POST	       0x2002	/* 32-02 Missing or no channels */
#define ERRDDE_NOINIT	       0x2003	/* 32-03 Missing or no channels */
#define ERRDDE_VARBUSY	       0x2004	/* 32-04 Missing or no channels */
#define ERRDDE_NOTADVISED      0x2005	/* 32-05 Missing or no channels */
#define ERRDDE_JUNK	       0x2006	/* 32-06 Missing or no channels */
#define ERRDDE_BUSY	       0x2007	/* 32-07 Missing or no channels */
#define ERRDDE_REJECTED        0x2008	/* 32-08 Missing or no channels */

#define ERRDEV_FIRST	       0x2A01
#define ERRDEV_BADID	       0x2A01	/* 42-01 Invalid or unsupported ID */
#define ERRDEV_OPEN	       0x2A02	/* 42-02 Device already open */
#define ERRDEV_NOPEN	       0x2A03	/* 42-03 Device not open */
#define ERRDEV_MEMORY	       0x2A04	/* 42-04 Unable to allocate queues */
#define ERRDEV_DEFAULT	       0x2A05	/* 42-05 Error in default parameters */
#define ERRDEV_DEFAULT6        0x2A06	/* 42-06 Default -6 */
#define ERRDEV_DEFAULT7        0x2A07	/* 42-06 Default -6 */
#define ERRDEV_DEFAULT8        0x2A08	/* 42-06 Default -6 */
#define ERRDEV_DEFAULT9        0x2A09	/* 42-06 Default -6 */
#define ERRDEV_HARDWARE        0x2A0A	/* 42-10 Hardware not present */
#define ERRDEV_BYTESIZE        0x2A0B	/* 42-11 Invalid byet size */
#define ERRDEV_BAUDRATE        0x2A0C	/* 42-12 Unsupported baud rate */
#define ERRDEV_BADPARAM        0x2A0D   /* 42-13 Bad Communication Parameter */
#define ERRDEV_BADMODEM        0x2A0E   /* 42-14 Bad modem selected */

#define ERRTRM_FIRST	       0x2C01
#define ERRTRM_BADTERMINAL     0x2C01	/* 44-01 Bad Terminal Selection */
#define ERRTRM_BADPARAM        0x2C02	/* 44-02 Bad Terminal Parameter */

#define ERRXFR_FIRST	       0x2D01
#define ERRXFR_TIMEOUT	       0x2D01	/* 45-01 General Timeout */
#define ERRXFR_HOSTNORESP      0x2D02	/* 45-02 Host not responding */
#define ERRXFR_HOSTBAD	       0x2D03	/* 45-03 Incorrect response frm host */
#define ERRXFR_TOOMANYERRORS   0x2D04	/* 45-04 Too many errors */
#define ERRXFR_LOSTCONNECT     0x2D05	/* 45-05 Lost connection */
#define ERRXFR_SEQERROR        0x2D06	/* 45-06 Sequencing failure */
#define ERRXFR_LOCALCANCEL     0x2D07	/* 45-07 Cancelled by local operator */
#define ERRXFR_HOSTCANCEL      0x2D08	/* 45-08 Cancelled by host */
#define ERRXFR_NOWILDCARD      0x2D09	/* 45-09 Can't do wildcards */
#define ERRXFR_NOFILEFOUND     0x2D0A	/* 45-10 File(s) not found */
#define ERRXFR_LOCALDISKFULL   0x2D0B	/* 45-11 Local disk full */
#define ERRXFR_HOSTDISKFULL    0x2D0C	/* 45-12 Host disk full */
#define ERRXFR_NOSERVER        0x2D0D	/* 45-13 No server commands */
#define ERRXFR_NEEDFILENAME    0x2D0E	/* 45-14 File name required */
#define ERRXFR_BUSY	       0x2D0F	/* 45-15 System Busy */
#define ERRXFR_BADPROTOCOL     0x2D10	/* 45-16 Bad Protocol */
#define ERRXFR_BADPARAM        0x2D11   /* 45-17 Bad Xfer Parameter */
#define ERRXFR_TOOLONG         0x2D12   /* 45-18 Received Block too big */

#define ERRKRM_FIRST	       0x2E01
#define ERRKRM_BUSY	       0x2E01	/* 46-01 Kermit is already busy */
#define ERRKRM_NOTBUSY	       0x2E02
#define ERRKRM_ERROR	       0x2E03
#define ERRKRM_PARSE	       0x2E04
#define ERRKRM_CANTBEUSER      0x2E05
#define ERRKRM_MUSTBEUSER      0x2E06
#define ERRKRM_INTERNAL        0x2E07
#define ERRKRM_CONNECT	       0x2E08
#define ERRKRM_NOTOPEN	       0x2E09
#define ERRKRM_FILEREAD        0x2E0A
#define ERRKRM_FILEWRITE       0x2E0B
#define ERRKRM_NOTXFER	       0x2E0C
#define ERRKRM_PACKETLENGTH    0x2E0D
#define ERRKRM_PADDING	       0x2E0E
#define ERRKRM_PAUSE	       0x2E0F
#define ERRKRM_TIMEOUT	       0x2E10
#define ERRKRM_BLOCKCHECK      0x2E11
#define ERRKRM_DELAY	       0x2E12
#define ERRKRM_INITIAL	       0x2E13
#define ERRKRM_NOWILDFILE      0x2E14
#define ERRKRM_NOWILDSTART     0x2E15
#define ERRKRM_PACKET_N        0x2E16
#define ERRKRM_PACKET_Q        0x2E17
#define ERRKRM_PACKET_U        0x2E18
#define ERRKRM_NOKCP           0x2E19


#define LEVEL_SYSTEM	0
#define LEVEL_GLOBAL	1
#define LEVEL_LOCAL	2

#define TYPE_STRING	0
#define TYPE_INTEGER	1
#define TYPE_NULL	3


extern char fHostScript;
extern char fTimeout;
extern char fConnectARQ;
extern int  nConnectSpeed;
extern int  nErrorCode;
extern int  nErrorFlag;
extern int  nErrorSave;
extern char szScriptName[16];
extern char szMatchedString[33];
extern char cDefInputFile;
extern char cDefOutputFile;
extern int  nWorkChoice;


int  InitVariable();
void TermVariable();

int  CreateVariableHandle(char *, int);
void DeleteVariableHandle(int);
int  GetVariableHandle(char *);

short SetIntVariable(int, long);
void GetIntVariable(int, long *);
short SetStrVariable(int, BYTE *, WORD);
BYTE GetStrVariable(int, BYTE *, WORD);
BYTE GetStrLength(int);
BYTE GetVariableLevel(int);
void PurgeVariableLevel(WORD);

void LockVariable();
void UnlockVariable();

int  IsStrVariable(int);
#define IsIntVariable(x) (!IsStrVariable(x))

int  IdentifyVariable(char *);
int  GetSysVariableAny(WORD, BYTE *, BYTE *, long *);
int  PutSysVariableAny(WORD, BYTE *, BYTE, long);
