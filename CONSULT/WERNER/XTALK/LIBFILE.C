/*****************************************************************************\
*                                                                             *
*  Copyright (C) 1990 Digital Communications Associates, Inc.                 *
*                All Rights Reserved.                                         *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Library File Routines (LIBFILE.C)                               *
*                                                                             *
*   Author:   Kent David Cedola.                                              *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Remarks:   This module contains common library routines that interface     *
*             with the disk drive in someway.                                 *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.                                  		   *
*  1.01 09/08/89 00029    Apply Paula's fixes to ValidateDiskDrive 		   *
*                         subroutine.  (I just typed them in).     		   *
*  1.01 10/15/89 00047    Remove ^Z from end of the file before    		   *
*                         appending to it.			     					   *
*  1.01 11/20/89 00060    Support ':\' on input for path.					   *
*  1.01 12/08/89 00065    Problem with identifier ending with tab.   		   *
*  1.01 02/03/90 dca00018 PJL Couldn't Send files from a root directory.      *
*  1.01 02/03/90 dca00019 PJL ValidatePath() routine Was stripping slashes    *
*                             from directory names.  Caused ANSWER script to  *
*                             bomb on "chdir DirFil", for instance, if DirFil *
*                             was set to "C:".                                *
*  1.10 03/15/90 dca00049 PJL/JDB Stop GetNextLine from eating chars > 127.   *
*  1.1  04/02/90 dca00065 PJL Fix Invalid/Open disk drive error reporting.	   *
*  1.1  04/03/90 dca00068 PJL Fix Network directory access bug (getfileattr). *
*	1.1  04/08/90 dca00073 PJL Trap 'Invalid Drive'/'Drive not Ready' errors.  *
*	1.1b 05/18/90 dca00098 PJL Fix Modem string save-to-disk problem.		   *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <dos.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "xtalk.h"
#include "library.h"
#include "variable.h"
#include "term.h"  /*debug */


USHORT XlatError(USHORT usDosError)
{

	switch (usDosError)
	{
	case 0:
		return (0);
		break;

	case ESRCH:
		return (ERRSYS_DRIVENOTREADY);
		break;

	case ENOENT:
		return (ERRDOS_NOFILE);
		break;

	case EACCES:
		return (ERRDOS_ACCESSDENIED);
		break;

	case EMFILE:
		return (ERRDOS_TOOMANYFILES);
		break;

	case EBADF:
	case ENOSPC:
		return (ERRDOS_DISKFULL);
		break;

	case 83:
		return (ERRDOS_WRITEPROTECTED);
		break;

	default:
		return (ERRDOS_UNKNOWN);
		break;
	}
}


void GetProfileData(szApp, szKey, vDefault, vString, nSize)
char *szApp;
char *szKey;
char *vDefault;
char *vString;
int  nSize;
{
	char szDefault[128], szString[128];

	LibEnstore(vDefault, szDefault, sizeof(szDefault));
	GetProfileString(szApp, szKey, szDefault, szString, sizeof(szString));
	LibDestore(szString, vString, nSize);
}


void WriteProfileData(szApp, szKey, vString)
char *szApp;
char *szKey;
char *vString;
{
	char szNull[1], szString[128], szBuf[128];

	szNull[0] = 0;
	LibEnstore(vString, szString, sizeof(szString));
	GetProfileString(szApp, szKey, szNull, szBuf, sizeof(szBuf));
	// dca00098 PJL Fix Modem string save-to-disk problem.
	if (strcmp(szString, szBuf) != 0)
		WriteProfileString(szApp, szKey, szString);
}


//
// IsPathAvailable()
//
BOOL IsPathAvailable(short nVarNo)
{
	char szDir[MAX_PATH];

	GetStrVariable(nVarNo, szDir, sizeof(szDir));

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
	switch(IsDriveReady(szDir))
	{
	case 0:
		return(TRUE);
		break;

	case ERRSYS_DRIVENOTREADY:
		GeneralError(ERRSYS_DRIVENOTREADY);
		return(FALSE);
		break;

	default:
		GeneralError(ERRSYS_BADDRIVE);
		return(FALSE);
		break;
	}
}


//
// IsDriveReady()
//
USHORT IsDriveReady(char *szPath)
{
	unsigned curdrive, newdrive, drives;
	register USHORT usResults;
	char chDrive;

	// return if we don't have a drive specifier
	if (szPath[1] != ':')
		return(0);

	// get current drive
	_dos_getdrive(&curdrive);

	chDrive = toupper(szPath[0]);
	newdrive = chDrive - '@';

	// switch to the new drive, if we're not already there 
	if (newdrive != curdrive)
	{
		_dos_setdrive(newdrive, &drives);
		// if we didn't switch drives, user's drive is invalid
		_dos_getdrive(&newdrive);
		if (newdrive == curdrive)
			return(ERRSYS_BADDRIVE);
	}

	// Try and set the path to see if the drive is ready.
	usResults = DosChDir("\\", 0L);

	// switch back to original drive
	_dos_setdrive(curdrive, &drives);

	// dca00065 PJL Fix Invalid/Open disk drive error reporting.
	if (usResults != 0)
		return(ERRSYS_DRIVENOTREADY);

	return(usResults);
}


/*---------------------------------------------------------------------------*/
/*  ValidateDiskDrive()                                                      */
/*                                                                           */
/*  Description:                                                             */
/*    Examine the specified path and determine if the drive, if given, is    */
/*    a valid disk drive on this system.                                     */
/*                                                                           */
/*  Return:                                                                  */
/*    1+curdrive_no if no drive is specified							      */
/*    0 if drive is valid												      */
/*    -1 if drive is invalid                                                 */
/*---------------------------------------------------------------------------*/
int ValidateDiskDrive(char *szPath)
{
	unsigned curdrive, newdrive, drives;
	char chDrive;

	// get current drive
	_dos_getdrive(&curdrive);

	// missing drive specifier?
	if (szPath[0] == ':' || szPath[0] == '\\' ||
		szPath[1] != ':' || szPath[0] == '\0')
		return(1+curdrive);

	// Make sure that there IS a drive letter
	chDrive = toupper(szPath[0]);
	if (chDrive < 'A' || chDrive > 'Z')
		return(-1);

	switch(IsDriveReady(szPath))
	{
	case 0:
	case ERRSYS_DRIVENOTREADY:
		return(0);
		break;

	default:
		return(-1);
		break;
	}
}


/*---------------------------------------------------------------------------*/
/*  ValidatePath()                                                           */
/*                                                                           */
/*  Description:                                                             */
/*    Examine the specified path and determines if the drive and path are    */
/*    valid on this system.                                                  */
/*                                                                           */
/*  Return:                                                                  */
/*    TRUE if path is valid, else FALSE.                                     */
/*---------------------------------------------------------------------------*/
BOOL ValidatePath(char *szPath)
{
	unsigned attribute;
	register i, j;

	if ((i = ValidateDiskDrive(szPath)) == -1)
		return(FALSE);

	// add drive letter if it's missing
	if (i != 0)
	{
		j = strlen(szPath);
		if (szPath[0] == '\\' || szPath[0] == '/')
		{
			while (j >= 0)
			{
				szPath[j+2] = szPath[j];
				j--;
			}
			szPath[0] = (char)(i + '@' - 1);
			szPath[1] = ':';
			szPath[2] = '\\';
		}
		else if (szPath[0] == ':')	// 00060 Convert ':\' path format.
		{
			while (j >= 0)
			{
				szPath[j+1] = szPath[j];
				j--;
			}
			szPath[0] = (char)(i + '@' - 1);
		}
		else
		{
			while (j >= 0)
			{
				szPath[j+3] = szPath[j];
				j--;
			}
			szPath[0] = (char)(i + '@' - 1);
			szPath[1] = ':';
			szPath[2] = '\\';
		}
	}

	// strip off trailing slashes?
	i = strlen(szPath) - 1;

	// dca00019 PJL don't strip off the slash from root directory names.
	// dca00065 PJL If the path is in C: or C:\ form then just return.
	if (i <= 3)
	{
		szPath[2] = '\\';
		szPath[3] = '\0';
		return(TRUE);
	}

	if (szPath[i] == '\\' || szPath[i] == '/')
		szPath[i] = '\0';

/*
	dca00068 PJL Can't use dos getfileattr on network drives!
	if (_dos_getfileattr(szPath, &attribute) == 0)
		if (attribute & _A_SUBDIR)
			return(TRUE);
*/

	if (DosChDir(szPath, 0L) == 0)
		return(TRUE);

	return(FALSE);
}


int IsEnoughDiskSpace(nDrive, lSpace)
int  nDrive;
long lSpace;
{
	struct diskfree_t drive;
	long	lTotal;

	_dos_getdiskfree(nDrive, &drive);

	lTotal = (long)drive.avail_clusters *
			 (long)drive.sectors_per_cluster *
			 (long)drive.bytes_per_sector ;

	if (lTotal >= lSpace)
		return (TRUE);
	else
		return (FALSE);
}


char * StripPath(path)
char *path;
{
	char *ptr;

	for (ptr = path; *path; path++)
	{
		if (*path == '/' || *path == '\\' || *path == ':')
			ptr = path + 1;
	}

	return (ptr);
}


char * StripName(char *path)
{
	register char *ptr;

	for (ptr = path; *path; path++)
	{
		if (*path == '/' || *path == '\\' || *path == ':')
			ptr = path;
	}

	if (*ptr == '/' || *ptr == '\\')
		*ptr = '\0';

	return (path);
}


//
// dca00073 PJL Trap ALL 'Invalid Drive'/'Drive not Ready' errors.
// dca00073 PJL Just use one routine (let's clean this stuff up!).
//
//	SetDirPath()
//
//	This routine does a CHDIR to the specified directory.
//	The disk drive is checked, then the file name (if any)
// is stripped before doing the CD if STRIP_FNAME = TRUE.
//
int SetDirPath(char *path, BOOL strip_fname)
{
	char buf[MAX_PATH], *ptr;
	int i;

	if (path[0] == '\0')
		return(0);

	strupr(path);
	if (path[1] == ':')
	{
		// dca00065 PJL Fix Invalid/Open disk drive error reporting.
		if (i = IsDriveReady(path))
		{
			GeneralError(i);
			return(i);
		}
		_dos_setdrive(path[0]-'@', &i);
		strcpy(buf, &path[2]);
	}
	else if (path[0] != '\\' && path[0] != '/')
		return(0);
	else
		strcpy(buf, path);

	if (strip_fname)
	{
		ptr = buf;
		for (i = 0; i < strlen(buf); i++)
		{
			if (buf[i] == '\\' || buf[i] == '/')
				ptr = &buf[i];
		}
		*ptr = NULL;
	}

	if (buf[0] == 0)
	{
		buf[0] = '\\';
		buf[1] = '\0';
	}

	return(DosChDir(buf, 0L));
}


short SetFullPath(nVarNo)
int  nVarNo;
{
	char szDir[MAX_PATH];

	GetStrVariable(nVarNo, szDir, sizeof(szDir));
	// dca00073 PJL Just use one routine.
	return(SetDirPath(szDir, FALSE));
}


int SetFilePath(char *path)
{

	// dca00073 PJL Just use one routine.
	return(SetDirPath(path, FALSE));
}


USHORT SetPathFile(char *path)
{

	// dca00073 PJL Just use one routine.
	return(SetDirPath(path, TRUE));
}


int SetPath(char *path)
{

	// dca00073 PJL Just use one routine.
	return(SetDirPath(path, TRUE));
}


//
// LibNextLine
//
//   This function performs a buffer line read from a file handle.
//
USHORT LibGetLine(PSZ pszLine, USHORT cLine, HFILE hfSrcFile, PUSHORT piBuf,
		      PUSHORT pusBytes, PBYTE pBuf, USHORT cBuf)
{
	USHORT usResults;

	while (1)
	{
		if (*piBuf >= *pusBytes)
		{
			if (usResults = DosRead(hfSrcFile, pBuf, cBuf, pusBytes))
				return (usResults);

			if (*pusBytes == 0)
				return (-1);

			*piBuf = 0;
		}

		if (pBuf[*piBuf] == '\r')
		{
			*pszLine = '\0';
			(*piBuf)++;

			return (0);
		}
		else
		{
			// 00065 We need to pass the tab character through.
			// dca00049 PJL Fix GetNextLine so it doesn't eat chars > 127

			if ((pBuf[*piBuf] >= 0x20 || pBuf[*piBuf] == '\t') && cLine > 1)
			{
				*pszLine++ = pBuf[*piBuf];
				cLine--;
			}
		}

		(*piBuf)++;
	}

	return (0);
}


// 00047
//
// SeekToEndOfFile
//
//   Used to seek to the end of an ascii file and removes any trailing control
// Z (26) characters from the end of the file.	Control-Z is still used by
// older software to define the end of the file.
//
void SeekToEndOfFile(HFILE hf)
{
	FILESTATUS fstsFile;
	ULONG      lFileSize;
	BYTE	     abBuffer[256];
	USHORT     usBytes;

	// Load information about the file.
	DosQFileInfo(hf, 1, &fstsFile, sizeof(fstsFile));
	if (fstsFile.cbFile == 0)
		return;
	else if (fstsFile.cbFile < 256)
		lFileSize = 0;
	else
		lFileSize = fstsFile.cbFile - 256;

	// Set the last 256 bytes or the beginning of the file.
	DosChgFilePtr(hf, lFileSize, FILE_BEGIN, &lFileSize);

	// Read the last 256 bytes (or as much as possible).
	DosRead(hf, abBuffer, sizeof(abBuffer), &usBytes);

	// Ignore any trailing control-Z's.
	while (usBytes > 0 && abBuffer[usBytes-1] == 26)
	{
		usBytes--;
		fstsFile.cbFile--;
	}

	// Set the end of file for textual appending.
	DosChgFilePtr(hf, fstsFile.cbFile, FILE_BEGIN, &lFileSize);
}
