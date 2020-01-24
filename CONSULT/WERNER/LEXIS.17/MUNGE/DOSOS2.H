/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project: WinLexis, Lexis/Nexis Session Manager for Microsoft Windows       *
*                                                                             *
*   Module:   Dos <--> OS/2 Conversion Module (DOSOS2.H)                      *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Revisions: 1.00 10/20/89 Initial Version.				      *
*                                                                             *
\*****************************************************************************/

#define MAX_BUFFER	512

typedef unsigned char	BYTE;
typedef BYTE *		PBYTE;
typedef char *		PSZ;
typedef short		HFILE;
typedef HFILE * 	PHFILE;
typedef short		SHORT;
typedef short * 	PSHORT;
typedef unsigned int    USHORT;
typedef USHORT *	PUSHORT;
typedef USHORT          SHANDLE;
typedef unsigned char * PBYTE;
typedef unsigned int  * PWORD;
typedef unsigned long   ULONG;
typedef ULONG * 	PULONG;
typedef void *		PVOID;

/* File time and date types */

typedef struct _FTIME {         /* ftime */
    unsigned twosecs : 5;
    unsigned minutes : 6;
    unsigned hours   : 5;
} FTIME;
typedef FTIME *PFTIME;

typedef struct _FDATE {         /* fdate */
    unsigned day     : 5;
    unsigned month   : 4;
    unsigned year    : 7;
} FDATE;
typedef FDATE *PFDATE;

/*** File manager */

/* DosChgFilePtr() file position codes */

#define FILE_BEGIN	0x0000
#define FILE_CURRENT	0x0001
#define FILE_END	0x0002

/* Directory handle types */

#define HDIR_SYSTEM  0x0001
#define HDIR_CREATE  0xFFFF

/* Dosopen/DosQFHandState file attributes */

#define FILE_NORMAL	0x0000
#define FILE_READONLY	0x0001
#define FILE_HIDDEN	0x0002
#define FILE_SYSTEM	0x0004
#define FILE_DIRECTORY	0x0010
#define FILE_ARCHIVED	0x0020

/* DosOpen() actions */

#define FILE_EXISTED	0x0001
#define FILE_CREATED	0x0002
#define FILE_TRUNCATED	0x0003

/* DosOpen() open flags */

#define FILE_OPEN      0x0001
#define FILE_TRUNCATE  0x0002
#define FILE_CREATE    0x0010

/* DosOpen/DosSetFHandState flags */

#define OPEN_ACCESS_READONLY      0x0000
#define OPEN_ACCESS_WRITEONLY     0x0001
#define OPEN_ACCESS_READWRITE     0x0002
#define OPEN_SHARE_DENYREADWRITE  0x0010
#define OPEN_SHARE_DENYWRITE	  0x0020
#define OPEN_SHARE_DENYREAD	  0x0030
#define OPEN_SHARE_DENYNONE	  0x0040
#define OPEN_FLAGS_NOINHERIT	  0x0080
#define OPEN_FLAGS_FAIL_ON_ERROR  0x2000
#define OPEN_FLAGS_WRITE_THROUGH  0x4000
#define OPEN_FLAGS_DASD 	  0x8000

/* DosSearchPath() constants */

#define SEARCH_PATH            0x0000
#define SEARCH_CUR_DIRECTORY   0x0001
#define SEARCH_ENVIRONMENT     0x0002


USHORT DosOpen(PSZ, PHFILE, PUSHORT, ULONG, USHORT, USHORT, USHORT, ULONG);
USHORT DosClose(HFILE);
USHORT DosRead(HFILE, PVOID, USHORT, PUSHORT);
USHORT DosWrite(HFILE, PVOID, USHORT, PUSHORT);
USHORT DosDelete(PSZ, ULONG);
USHORT DosMove(PSZ, PSZ, ULONG);

/* File time and date types */

typedef struct _FILESTATUS {    /* fsts */
    FDATE  fdateCreation;
    FTIME  ftimeCreation;
    FDATE  fdateLastAccess;
    FTIME  ftimeLastAccess;
    FDATE  fdateLastWrite;
    FTIME  ftimeLastWrite;
    ULONG  cbFile;
    ULONG  cbFileAlloc;
    USHORT attrFile;
} FILESTATUS;
typedef FILESTATUS *PFILESTATUS;

USHORT DosChgFilePtr(HFILE, LONG, USHORT, PULONG);
USHORT DosQFileInfo(HFILE, USHORT, PFILESTATUS, USHORT);

USHORT DosChDir(PSZ, ULONG);
