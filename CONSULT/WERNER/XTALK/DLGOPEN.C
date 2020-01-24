/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Open Dialog (DLGOPEN.C)                                         *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Comments:  Came from the Windows SDK example source code.                  *
*                                                                             *
*  Revisions:																   *
*  1.00 07/17/89 Initial Version.											   *
*  1.1  03/30/90 dca00062 PJL Fix extension addition problem with			   *
*							DlgFnOpen for File.Open.						   *
*  1.1  04/02/90 dca00065 PJL Fix Invalid/Open disk drive error reporting.	   *
*	1.1  04/02/90 dca00073 PJL Fix DlgFnSend - duplicate errors after fix #65. *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <string.h>
#include <dos.h>
#include "xtalk.h"
#include "library.h"
#include "term.h"
#include "dialogs.h"
#include "variable.h"

#define ATTRDIRLIST 0x4010  /* include directories and drives in listbox */

/* message box strings loaded in sample.c from stringtable */
extern char szIFN[], szFNF[], szREF[], szSCC[], szEOF[], szECF[];

char szMsg[MAX_STR_LEN+MAX_FNAME_LEN];
char *pszAppname;
char *szExtSave;
char *szFileNameSave;
char szFileNameTemp[MAX_FNAME_LEN];
int  *pfpSave;
int   cbRootNameMax;
OFSTRUCT *rgbOpenSave;
OFSTRUCT rgbOpenTemp;
// dca00062 PJL Fix extension addition problem with XWP File.Open.
static BOOL fCreateNewFile, fAddExtension;


/*=============================================================================
 DLGOPEN is used to initialize vars & invoke the specified Open dialog box.
 Return value is either: NOOPEN, NEWOPEN, or OLDOPEN.  (described in DlgFnOpen).
 Also, rgbOpenIn, pfpIn, and szFileNameIn are changed to hold info about the
 file which is chosen and opened.  Inputs are commented below.
=============================================================================*/
int far DlgOpen(hInstance, hwndParent, idDlgIn, rgbOpenIn, pfpIn,
                szFileNameIn, szExtIn, pszAppnameIn, fMakeFile, fAlwaysAddExt)

HANDLE   hInstance;	   	/* app module instance handle */
HWND     hwndParent;	   	/* window handle of parent window */
int      idDlgIn;			/* open dialog box id */
OFSTRUCT *rgbOpenIn;  		/* ptr to current file's ofstruct */
int      *pfpIn;			/* ptr to file pointer (current file's handle) */
char     *szFileNameIn;   	/* ptr to array which will get new file's name (no path) */
char     *szExtIn;			/* ptr to current default extension */
char     *pszAppnameIn;   	/* ptr to application name */
BOOL     fMakeFile;	   	/* allow file creation? */
// dca00062 PJL
BOOL     fAlwaysAddExt;   	/* always add default file extension? */
{
	BOOL    fResult;
	FARPROC lpProc;

	rgbOpenTemp = *rgbOpenIn;
	rgbOpenSave = &rgbOpenTemp;
	pfpSave = pfpIn;
	lstrcpy((LPSTR)szFileNameTemp, (LPSTR)szFileNameIn);
	szFileNameSave = szFileNameTemp;
	szExtSave = szExtIn;
	pszAppname = pszAppnameIn;
	cbRootNameMax = MAX_FNAME_LEN - CBEXTMAX - 1;	/* leave room for ext. */
	fCreateNewFile = fMakeFile;
	// dca00062 PJL
	fAddExtension = fAlwaysAddExt;

	fResult = DialogBox(hInstance, MAKEINTRESOURCE(idDlgIn), hwndParent,
	lpProc = MakeProcInstance(DlgFnOpen, hInstance));

	FreeProcInstance(lpProc);
	if (fResult != NOOPEN)
	{
		/* copy new info into caller's ofstruct */
		*rgbOpenIn = rgbOpenTemp;
		SetPath(szFileNameSave);
		lstrcpy((LPSTR)szFileNameIn, (LPSTR)DlgPFileInPath(szFileNameSave));
	}

	return fResult;

} /* end DlgOpen */


/*=============================================================================
 DLGFNOPEN controls the Open dialog box.  The dialog box handles input, allows
 user to change directories, checks for legal filenames, opens specified files,
 appends default extension if needed, and returns the file's name and ofstruct.
 This routine correctly parses filenames containing KANJI characters.
 It returns to DlgOpen one of the following:
    NOOPEN  - user hits cancel.
    NEWOPEN - created new file.     (file left open)
    OLDOPEN - opened existing file. (file left open)
=============================================================================*/
int DlgFnOpen(HWND hwnd, unsigned msg, WORD wParam, LONG lParam)
{
	int item;
	char rgch[256];
	int cchFile, cchDir;
	char *pchFile;
	BOOL	  fWild;
	register USHORT usResults;

	switch (msg)
	{
	case WM_INITDIALOG:
		/* Set edit field with default search spec */
		SetDlgItemText(hwnd, ID_EDIT, (LPSTR)(szExtSave+1));
		/* Don't let user type more than cbRootNameMax bytes in edit ctl. */
		SendDlgItemMessage(hwnd, ID_EDIT, EM_LIMITTEXT, cbRootNameMax, 0L);

		/* fill list box with filenames that match spec, and fill static
			field with path name */
		if (!DlgDirList(hwnd, (LPSTR)(szExtSave+1), ID_LISTBOX, ID_PATH, ATTRDIRLIST))
			EndDialog(hwnd, NOOPEN);
		else
		{
			SendDlgItemMessage(hwnd, ID_EDIT, EM_SETSEL, 0, MAKELONG(0, 0x7FFF));
			SetFocus(GetDlgItem(hwnd, ID_EDIT));
		}
		return (FALSE);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
ItsOk:
			if (IsWindowEnabled(GetDlgItem(hwnd, IDOK)))
			{
				// Get contents of edit field, and add search spec if needed.
				GetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave, cbRootNameMax);
				if (!fCreateNewFile)
					if (strchr(szFileNameSave, '.') == 0)
						strcat(szFileNameSave, ".");
				lstrcpy((LPSTR)rgch, (LPSTR)szFileNameSave);

				// Append appropriate extension to user's entry.
				DlgAddCorrectExtension(rgch, TRUE);

				// dca00073 PJL Check drive BEFORE calling DlgDirList.
				if (usResults = IsDriveReady(rgch))
				{
					GeneralError(usResults);
					break;
				}

				// Try to open directory.  If successful, fill listbox with
				// contents of new directory.  Otherwise, open datafile.
				if (DlgSearchSpec(rgch))
				{
					if (DlgDirList(hwnd, (LPSTR)rgch, ID_LISTBOX, ID_PATH, ATTRDIRLIST))
					{
						lstrcpy((LPSTR)szFileNameSave, (LPSTR)rgch);
						SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
						break;
					}
				}

				DlgAddCorrectExtension(szFileNameSave, FALSE);
LoadIt:
				// If no directory list and filename contained search spec,
               // honk and don't try to open.
				if (DlgSearchSpec(szFileNameSave))
				{
					// DlgMergeStrings(szIFN, szFileNameSave, szMsg);
					// MessageBox(hwnd, szMsg, pszAppname,
					//             MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
					break;
				}

				// Make filename upper case and if it's a legal dos
				// name, try to open the file.
				AnsiUpper((LPSTR)szFileNameSave);
				if (usResults = DlgCheckFilename(szFileNameSave))
				{
					if (usResults > 1)
						GeneralError(usResults);
					else
					{
						// illegal filename.
						DlgMergeStrings(szIFN, szFileNameSave, szMsg);
						MessageBox(hwnd, szMsg, pszAppname,
									MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
					}
					break;
				}

				// see if file already exists
				*pfpSave = OpenFile((LPSTR)szFileNameSave,(LPOFSTRUCT)rgbOpenSave,  OF_READ);
				if (*pfpSave != -1)
				{
					EndDialog(hwnd, OLDOPEN);
					break;
				}

				// file doesn't exist; create new one?
				if (fCreateNewFile)
				{
					DlgMergeStrings(szFNF, szFileNameSave, szMsg);
					if (MessageBox(hwnd, szMsg, pszAppname,
									MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES)
					{
						*pfpSave = OpenFile((LPSTR)szFileNameSave,
										(LPOFSTRUCT)rgbOpenSave, OF_CREATE);
						if (*pfpSave != -1)
							EndDialog(hwnd, NEWOPEN);
						else
						{
							DlgMergeStrings(szECF, szFileNameSave, szMsg);
							MessageBox(hwnd, szMsg, pszAppname,
										MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
							break;
						}
					}
				}
				else
				{
					EnableWindow(hwnd, FALSE);
					GeneralError(ERRXFR_NOFILEFOUND);
					EnableWindow(hwnd, TRUE);
					SetFocus(GetDlgItem(hwnd, ID_CANCEL));
				}
				break;
			}
			break;

		case IDCANCEL:
			// User pressed cancel.  Just take down dialog box.
			EndDialog(hwnd, NOOPEN);
			break;

		case ID_MORE:
			GetDlgItemText(hwnd, ID_EDIT, rgch, cbRootNameMax);
			if (usResults = IsDriveReady(rgch))
			{
				GeneralError(usResults);
				break;
			}
			if (SetPathFile(rgch))
			{
				EnableWindow(hwnd, FALSE);
				GeneralError(ERRDOS_NOPATH);
				EnableWindow(hwnd, TRUE);
				SetFocus(GetDlgItem(hwnd, ID_CANCEL));
				break;
			}
			strcpy(szFileNameSave, StripPath(rgch));
			*pfpSave = -1;
			EndDialog(hwnd, BATCH);
			break;

		// User single clicked or doubled clicked in listbox -
		// Single click means fill edit box with selection.
		// Double click means go ahead and open the selection.
		case ID_LISTBOX:
			switch (HIWORD(lParam))
			{
			// Single click case
			case 1:
				GetDlgItemText(hwnd, ID_EDIT, (LPSTR)rgch, cbRootNameMax);

				// Get selection, which may be either a prefix to a new search
				// path or a filename. DlgDirSelect parses selection, and
				// appends a backslash if selection is a prefix.
				if (DlgDirSelect(hwnd, (LPSTR)szFileNameSave, ID_LISTBOX))
				{
					cchDir = lstrlen((LPSTR)szFileNameSave);
					cchFile = lstrlen((LPSTR)rgch);
					pchFile = rgch+cchFile;

					// Now see if there are any wild characters (* or ?) in
					// edit field.  If so, append to prefix. If edit field
					// contains no wild cards append default search spec
					// which is  "*.TXT" for notepad.
					fWild = (*pchFile == '*' || *pchFile == ':');
					while (pchFile > rgch)
					{
						pchFile = (char *)AnsiPrev((LPSTR)(rgch), (LPSTR)pchFile);
						if (*pchFile == '*' || *pchFile == '?')
							fWild = TRUE;
						if (*pchFile == '\\' || *pchFile == ':')
						{
							pchFile = (char *)AnsiNext((LPSTR)pchFile);
							break;
						}
					}
					if (fWild)
						lstrcpy((LPSTR)szFileNameSave + cchDir, (LPSTR)pchFile);
					else
						lstrcpy((LPSTR)szFileNameSave + cchDir, (LPSTR)(szExtSave+1));
				}

				// Set edit field to entire file/path name.
				SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
				break;

			// Double click case - first click has already been processed
			// as single click
			case 2:
				// Basically the same as ok.  If new selection is directory,
				// open it and list it.  Otherwise, open file.
				if (DlgDirList(hwnd, (LPSTR)szFileNameSave, ID_LISTBOX, ID_PATH, ATTRDIRLIST))
				{
					SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
					break;
				}
				// go load it up
				goto LoadIt;
			}
			break;

		case ID_EDIT:
			DlgCheckOkEnable(hwnd, ID_EDIT, HIWORD(lParam));
			break;

		default:
			return(FALSE);
		}
		default:
			return FALSE;
	}

	return(TRUE);

} /* end DlgFnOpen */


/*=============================================================================
 DLGOPEN is used to initialize vars & invoke the specified Open dialog box.
 Return value is either: NOOPEN, NEWOPEN, or OLDOPEN.  (described in DlgFnOpen).
 Also, rgbOpenIn, pfpIn, and szFileNameIn are changed to hold info about the
 file which is chosen and opened.  Inputs are commented below.
=============================================================================*/
int far DlgSend(hInstance, hwndParent, idDlgIn, rgbOpenIn, pfpIn,
		szFileNameIn, szExtIn, pszAppnameIn)

HANDLE   hInstance;		/* app module instance handle */
HWND     hwndParent;		/* window handle of parent window */
int      idDlgIn;			/* open dialog box id */
OFSTRUCT *rgbOpenIn;		/* ptr to current file's ofstruct */
int      *pfpIn;			/* ptr to file pointer (current file's handle) */
char     *szFileNameIn;	/* ptr to array which will get new file's name (no path) */
char     *szExtIn;			/* ptr to current default extension */
char     *pszAppnameIn;	/* ptr to application name */
{
	BOOL    fResult;
	FARPROC lpProc;

	rgbOpenTemp = *rgbOpenIn;
	rgbOpenSave = &rgbOpenTemp;
	pfpSave = pfpIn;
	lstrcpy((LPSTR)szFileNameTemp, (LPSTR)szFileNameIn);
	szFileNameSave = szFileNameTemp;
	szExtSave = szExtIn;
	pszAppname = pszAppnameIn;
	cbRootNameMax = MAX_FNAME_LEN - CBEXTMAX - 1; /* leave room for ext. */
	// dca00073 PJL
	fAddExtension = FALSE;

	fResult = DialogBox(hInstance, MAKEINTRESOURCE(idDlgIn), hwndParent,
	lpProc = MakeProcInstance(DlgFnSend, hInstance));

	FreeProcInstance(lpProc);
	if (fResult != NOOPEN)
	{
		// copy new info into caller's ofstruct
		*rgbOpenIn = rgbOpenTemp;
		SetPath(szFileNameSave);
		lstrcpy((LPSTR)szFileNameIn, (LPSTR)DlgPFileInPath(szFileNameSave));
	}

	return fResult;

} /* end DlgSend */


/*=============================================================================
 DLGFNSEND controls the Send dialog box.  The dialog box handles input, allows
 user to change directories, checks for legal filenames, opens specified files,
 appends default extension if needed, and returns the file's name and ofstruct.
 This routine correctly parses filenames containing KANJI characters.
 It returns to DlgOpen one of the following:
    NOOPEN  - user hits cancel.
    NEWOPEN - created new file.     (file left open)
    OLDOPEN - opened existing file. (file left open)
=============================================================================*/
int DlgFnSend(hwnd, msg, wParam, lParam)
HWND hwnd;
unsigned msg;
WORD wParam;
LONG lParam;
{
	int item;
	char rgch[256];
	int cchFile, cchDir;
	char *pchFile;
	BOOL	  fWild;
	register USHORT usResults;

	char pbuf[100];

	switch (msg)
	{
	case WM_INITDIALOG:
		// Set edit field with default search spec
		SetDlgItemText(hwnd, ID_EDIT, (LPSTR)(szExtSave+1));
		// Don't let user type more than cbRootNameMax bytes in edit ctl.
		SendDlgItemMessage(hwnd, ID_EDIT, EM_LIMITTEXT, cbRootNameMax, 0L);

		// fill list box with filenames that match spec, and fill static
		// field with path name.
		if (!DlgDirList(hwnd, (LPSTR)(szExtSave+1), ID_LISTBOX, ID_PATH, ATTRDIRLIST))
			EndDialog(hwnd, NOOPEN);
		else
		{
			SendDlgItemMessage(hwnd, ID_EDIT, EM_SETSEL, 0, MAKELONG(0, 0x7FFF));
			SetFocus(GetDlgItem(hwnd, ID_EDIT));
		}
		return (FALSE);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (IsWindowEnabled(GetDlgItem(hwnd, IDOK)))
			{
				// Get contents of edit field.
				// Add search spec if it does not contain one.
				GetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave, cbRootNameMax);
				if (strchr(szFileNameSave, '.') == 0)
					strcat(szFileNameSave, ".");
				lstrcpy((LPSTR)rgch, (LPSTR)szFileNameSave);

				// Append appropriate extension to user's entry.
				//  DlgAddCorrectExtension(rgch, TRUE);

				// dca00073 PJL Check drive BEFORE calling DlgDirList.
				if (usResults = IsDriveReady(rgch))
				{
					GeneralError(usResults);
					break;
				}

				// Try to open directory.  If successful, fill listbox with
				// contents of new directory.  Otherwise, open datafile.
				if (DlgSearchSpec(rgch))
				{
					if (DlgDirList(hwnd, (LPSTR)rgch, ID_LISTBOX, ID_PATH, ATTRDIRLIST))
					{
						lstrcpy((LPSTR)szFileNameSave, (LPSTR)rgch);
						SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
						break;
					}
					else
					{
						EnableWindow(hwnd, FALSE);
						GeneralError(ERRDOS_NOPATH);
						EnableWindow(hwnd, TRUE);
						SetFocus(GetDlgItem(hwnd, ID_CANCEL));
					}
				}

				DlgAddCorrectExtension(szFileNameSave, FALSE);

				// If no directory list and filename contained search spec,
				// honk and don't try to open.
				if (DlgSearchSpec(szFileNameSave))
				{
					MessageBeep(0);
					break;
				}
LoadIt:
				// Make filename upper case and if it's a legal dos
				// name, try to open the file.
				AnsiUpper((LPSTR)szFileNameSave);
				if (usResults = DlgCheckFilename(szFileNameSave))
				{
					if (usResults > 1)
						GeneralError(usResults);
					else
					{
						// illegal filename.
						DlgMergeStrings(szIFN, szFileNameSave, szMsg);
						MessageBox(hwnd, szMsg, pszAppname,
									MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
					}
					break;
				}

				// see if file already exists
				*pfpSave = OpenFile((LPSTR)szFileNameSave,(LPOFSTRUCT)rgbOpenSave,  OF_READ);
				if (*pfpSave != -1)
				{
					EndDialog(hwnd, OLDOPEN);
					break;
				}

				EnableWindow(hwnd, FALSE);
				GeneralError(ERRXFR_NOFILEFOUND);
				EnableWindow(hwnd, TRUE);
				SetFocus(GetDlgItem(hwnd, ID_CANCEL));
				break;
			}
			break;

		case IDCANCEL:
			// User pressed cancel.  Just take down dialog box.
			EndDialog(hwnd, NOOPEN);
			break;

		case ID_MORE:
			GetDlgItemText(hwnd, ID_EDIT, rgch, cbRootNameMax);
			if (usResults = IsDriveReady(rgch))
			{
				GeneralError(usResults);
				break;
			}

			if (SetPathFile(rgch))
			{
				EnableWindow(hwnd, FALSE);
				GeneralError(ERRDOS_NOPATH);
				EnableWindow(hwnd, TRUE);
				SetFocus(GetDlgItem(hwnd, ID_CANCEL));
				break;
			}

			strcpy(szFileNameSave, StripPath(rgch));
			*pfpSave = -1;
			EndDialog(hwnd, BATCH);
			break;

		// User single clicked or doubled clicked in listbox -
		// Single click means fill edit box with selection.
		// Double click means go ahead and open the selection.
		case ID_LISTBOX:
			switch (HIWORD(lParam))
			{
			// Single click case
			case 1:
				GetDlgItemText(hwnd, ID_EDIT, (LPSTR)rgch, cbRootNameMax);

				// Get selection, which may be either a prefix to a new search
				// path or a filename. DlgDirSelect parses selection, and
				// appends a backslash if selection is a prefix.
				if (DlgDirSelect(hwnd, (LPSTR)szFileNameSave, ID_LISTBOX))
				{
					cchDir = lstrlen((LPSTR)szFileNameSave);
					cchFile = lstrlen((LPSTR)rgch);
					pchFile = rgch+cchFile;

					// Now see if there are any wild characters (* or ?) in
					// edit field.  If so, append to prefix. If edit field
					// contains no wild cards append default search spec
					// which is  "*.TXT" for notepad.
					fWild = (*pchFile == '*' || *pchFile == ':');
					while (pchFile > rgch)
					{
						pchFile = (char *)AnsiPrev((LPSTR)(rgch), (LPSTR)pchFile);
						if (*pchFile == '*' || *pchFile == '?')
							fWild = TRUE;
						if (*pchFile == '\\' || *pchFile == ':')
						{
							pchFile = (char *)AnsiNext((LPSTR)pchFile);
							break;
						}
					}
					if (fWild)
						lstrcpy((LPSTR)szFileNameSave + cchDir, (LPSTR)pchFile);
					else
						lstrcpy((LPSTR)szFileNameSave + cchDir, (LPSTR)(szExtSave+1));
				}

				// Set edit field to entire file/path name.
				SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
				break;

			// Double click case - first click has already been processed
			// as single click.
			case 2:
				// Basically the same as ok.  If new selection is directory,
				// open it and list it.  Otherwise, open file.
				if (DlgDirList(hwnd, (LPSTR)szFileNameSave, ID_LISTBOX, ID_PATH, ATTRDIRLIST))
				{
					SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
					break;
				}
				// go load it up
				goto LoadIt;
		    }
		    break;

		case ID_EDIT:
		    DlgCheckOkEnable(hwnd, ID_EDIT, HIWORD(lParam));
		    break;

		default:
		    return(FALSE);
		}

	default:
		return FALSE;
	}

	return(TRUE);

} /* end DlgFnSend */


/*=============================================================================
 DLGCHECKOKENABLE enables the ok button in dialog box if and only if the edit
 field contains text.  Edit field must have id of ID_EDIT.
=============================================================================*/
VOID FAR DlgCheckOkEnable(hwnd, idEdit, message)
HWND	hwnd;
int	idEdit;
unsigned message;
{

	if (message == EN_CHANGE)
	{
		EnableWindow(GetDlgItem(hwnd, IDOK),
			(SendMessage(GetDlgItem(hwnd, idEdit), WM_GETTEXTLENGTH, 0, 0L)));
	}
}


/*=============================================================================
 DLGADDCORRECTEXTENSION adds appropriate extension to filename, partial filename,
 search spec, or partial search spec.
==============================================================================*/
DlgAddCorrectExtension(szEdit, fSearching)
char	*szEdit;
BOOL	fSearching;
{
	register char    *pchLast;
	register char    *pchT;
	int ichExt;
	BOOL    fDone = FALSE;
	int     cchEdit;

	pchT = pchLast = (char *)AnsiPrev((LPSTR)szEdit, (LPSTR)(szEdit + (cchEdit = lstrlen((LPSTR)szEdit))));

	if ((*pchLast == '.' && *(AnsiPrev((LPSTR)szEdit, (LPSTR)pchLast)) == '.') && cchEdit == 2)
		ichExt = 0;
	else if (*pchLast == '\\' || *pchLast == ':')
		ichExt = 1;
	else
	{
		for (; pchT > szEdit; pchT = (char *)AnsiPrev((LPSTR)szEdit, (LPSTR)pchT))
		{
			/*
				If we're not searching and we encounter a period, don't add
				any extension.  If we are searching, period is assumed to be
				part of directory name, so go ahead and add extension. However,
				if we are searching and find a search spec, do not add any
				extension.
			*/
			if (fSearching)
			{
				if (*pchT == '*' || *pchT == '?')
					return;
			}
			// dca00062 PJL
			else if (*pchT == '.')
				if (fAddExtension && *pchLast == '.')
					break;
				else
					return;

			/* Quit when we get to beginning of last node. */
			if (*pchT == '\\')
				break;
		}

		// Special case hack fix since AnsiPrev can not return value less than
		// szEdit. If first char is wild card, return without appending.
		if (fSearching && (*pchT == '*' || *pchT == '?'))
			return;

		// dca00062 PJL
		if (fSearching)
			ichExt = 0;
		else if (fAddExtension)
			ichExt = 3;
		else
			ichExt = 2;
	}

	lstrcpy((LPSTR)(pchLast+1), (LPSTR)(szExtSave+ichExt));

} /* end DlgAddCorrectExtension */



//
// Check for a valid file name.
//
USHORT DlgCheckFilename(char *pch)
{
	OFSTRUCT ofT;
	register USHORT usResults;

	// dca00065, dca00073 PJL
 	if (usResults = IsDriveReady(pch))
 		return(usResults);

	if (OpenFile(pch, &ofT, OF_PARSE))
		return(1);
	else
		return(0);
}


/*=================*/
BOOL DlgSearchSpec(sz) // return TRUE iff 0 terminated str contains  '*' or '?
/*=================*/
register char	 *sz;
{

	for (; *sz;sz++)
		if (*sz == '*' || *sz == '?')
			return TRUE;

	return FALSE;
}
