/*****************************************************************************\
*                                                                             *
*  Copyright (c) 1989 Werner Associates.  All Rights Reserved.                *
*                                                                             *
*  Project:   XTALK, Crosstalk for Windows.                                   *
*                                                                             *
*   Module:   Save Dialog (DLGSAVE.C)                                         *
*                                                                             *
*   System:   Microsoft Windows 2.1d, Microsoft Windows 386 2.1d under        *
*             DOS 3.3 and compiled with Microsoft C 5.1.                      *
*                                                                             *
*  Comments:  Came from the Windows SDK example source code.                  *
*                                                                             *
*  Revisions: 1.00 07/17/89 Initial Version.                                  *
*                                                                             *
\*****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "xtalk.h"
#include "dialogs.h"
#include "global.h"
#include "variable.h"

/* message box strings loaded in sample.c from the stringtable */
extern char szIFN[], szFNF[], szREF[], szSCC[], szEOF[], szECF[];

char szDlgMsg[MAX_STR_LEN+MAX_FNAME_LEN];
char *pszExt;
char *pszAppName;
char *pszNewName;
char szTempName[MAX_FNAME_LEN];
OFSTRUCT *pOfstr;
OFSTRUCT ofstrtemp;
int *pFp;
BOOL fAutoBackup;
BOOL fAutoAppend;

/*=============================================================================
 DLGSAVEAS is used to initialize vars & invoke the specified Save As Dialog Box.
 Return value is either: NOSAVE, OLDSAVE, or NEWSAVE. (described in DlgFnSaveAs).
 Also, pFpIn, pOfstrIn, and pszNewName will be modified to hold info for the
 chosen file.  Inputs are commented below.
=============================================================================*/
int DlgSaveAs(hInstance, hWndParent, idDlgBox, pOfstrIn, pFpIn,
		  pszNewNameIn, pszExtIn, pszAppNameIn, fBackup, fAppend)

HANDLE hInstance;	/* app module instance handle */
HWND   hWndParent;	/* window handle of parent window */
int    idDlgBox;	/* SaveAs dialog box id */
OFSTRUCT *pOfstrIn;	/* ptr to current file OFSTRUCT (->cBytes=0 if no cur. file)*/
int   *pFpIn;		/* ptr to file pointer */
char  *pszNewNameIn;    /* ptr to array which will get new file's name (no path) */
char  *pszExtIn;	/* ptr to current default extension */
char  *pszAppNameIn;    /* ptr to application name */
BOOL  fBackup;
BOOL  fAppend;
{
    FARPROC lpProc;
    int     fResult;

    ofstrtemp	= *pOfstrIn;
    pOfstr	= &ofstrtemp;
    pFp 	= pFpIn;
    strcpy(szTempName, pszNewNameIn);
    pszNewName	= szTempName;
    pszExt	= pszExtIn;
    pszAppName	= pszAppNameIn;
    fAutoBackup = fBackup;
    fAutoAppend = fAppend;

    fResult = DialogBox(hInstance, MAKEINTRESOURCE(idDlgBox), hWndParent,
		      lpProc = MakeProcInstance(DlgFnSaveAs, hInstance));

    FreeProcInstance(lpProc);
    if (fResult != NOSAVE)
	{
        *pOfstrIn = ofstrtemp; /* copy new info into caller's ofstruct */
	strcpy(pszNewNameIn, DlgPFileInPath(szTempName));
	}
    return (fResult);

} /* end dlgsaveas */


/*=============================================================================
 DLGFNSAVEAS controls the save dialog box.  It handles input, tests for legal
 filenames, and uses message boxes to report problems.	It returns to DlgSaveAs
 either: NOSAVE  - user hits cancel button.
	 OLDSAVE - user wants to save over existing file. (file left open)
	 NEWSAVE - user wants to save to a new file.	  (file left open)
=============================================================================*/
BOOL DlgFnSaveAs (hDlg, message, wParam, lParam)
  HWND     hDlg;
  unsigned message;
  WORD     wParam;
  LONG     lParam;
{
  register USHORT usResults;


  switch (message)
    {
    case WM_INITDIALOG:
      /*** limit text to max filename length - size for ext ***/
      SendDlgItemMessage(hDlg, ID_EDIT, EM_LIMITTEXT, MAX_FNAME_LEN-CBEXTMAX, 0L);

      /*** initialize dialog box with current filename ***/
      DlgInitSaveAs(hDlg, pOfstr);

      if (szTempName[0] != NULL)
        {
        SetDlgItemText(hDlg, ID_EDIT, szTempName);
        EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
        }
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case IDOK:
          if (IsWindowEnabled(GetDlgItem(hDlg, IDOK)))
            {
	    GetDlgItemText(hDlg, ID_EDIT, pszNewName,
                              MAX_FNAME_LEN-CBEXTMAX);
	    AnsiUpper(pszNewName);
	 // {
         // if (strchr(pszNewName, '.') == 0)
         //   strcat(pszNewName, ".");
	 // }
            DlgAddExt(pszNewName);

	    if (usResults = DlgCheckFilename(pszNewName))
	      {
	      if (usResults > 1)
		GeneralError(usResults);
	      else
		{
		// illegal filename.
		DlgMergeStrings(szIFN, pszNewName, szDlgMsg);
		MessageBox(hDlg, szDlgMsg, pszAppName,
			   MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
		}
	      }
            else   /* see if file already exists; overwrite ok? */
	      {
	      *pFp = OpenFile(pszNewName, pOfstr, OF_EXIST);
              if (*pFp >= 0) /* already exists */
		{
		if (fAutoBackup && !fAutoAppend)
                  {
                  char szBackupName[128];
                  register char *ptr;

                  if (fBackups)
                    {
                    strcpy(szBackupName, pszNewName);
                    if ((ptr = strchr(szBackupName, '.')) == NULL)
                      ptr = &szBackupName[strlen(szBackupName)];
                    *ptr++ = '.';
                    *ptr++ = 'B';
                    *ptr++ = 'A';
                    *ptr++ = 'K';
                    *ptr   = NULL;
		    DosDelete(szBackupName, 0L);
                    rename(pszNewName, szBackupName);
                    }
                  else
		    DosDelete(pszNewName, 0L);

		  *pFp = OpenFile(pszNewName, (LPOFSTRUCT)pOfstr, OF_CREATE);
                  if (*pFp == -1)
		    {
                    DlgMergeStrings(szECF, pszNewName, szDlgMsg);
		    if (XlatError(pOfstr->nErrCode) == ERRDOS_WRITEPROTECTED)
		      GeneralError(ERRDOS_WRITEPROTECTED);
		    else
		      MessageBox(hDlg, szDlgMsg, pszAppName,
                                 MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
                    break;
                    }
                  EndDialog(hDlg, NEWSAVE);
		  break;
                  }
                else
                  {
		  if (fAutoAppend)
		    {
		    USHORT usAction;
		    register USHORT usResult;

		    if (usResult = DosOpen(pszNewName, pFp, &usAction, 0L,
					   FILE_NORMAL, FILE_OPEN,
					   OPEN_ACCESS_READWRITE, 0L))
		      {
		      *pFp = -1;
		      }
		    }
		  else
		    {
		    DlgMergeStrings(szREF, pszNewName, szDlgMsg);
		    if (MessageBox(hDlg, szDlgMsg, pszAppName,
			  MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION
			  | MB_SYSTEMMODAL) == IDNO)
		      return TRUE;   /* don't overwrite */
		    *pFp = OpenFile(pszNewName, (LPOFSTRUCT)pOfstr, OF_READWRITE);
		    }

                  if (*pFp == -1)
                    {
                    DlgMergeStrings(szEOF, pszNewName, szDlgMsg);
		    MessageBox(hDlg, szDlgMsg, pszAppName,
                              MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
                    break;
                    }
                  }

                EndDialog(hDlg, OLDSAVE);
		break;
                }

	      *pFp = OpenFile(pszNewName, (LPOFSTRUCT)pOfstr, OF_CREATE);
              if (*pFp == -1)
                {
                DlgMergeStrings(szECF, pszNewName, szDlgMsg);
		if (XlatError(pOfstr->nErrCode) == ERRDOS_WRITEPROTECTED)
		  GeneralError(ERRDOS_WRITEPROTECTED);
		else
		  MessageBox(hDlg, szDlgMsg, pszAppName,
			     MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
                break;
                }
              EndDialog(hDlg, NEWSAVE);
              }
            }
          break;

        case IDCANCEL:
          EndDialog(hDlg, NOSAVE);
          break;

        case ID_EDIT:
          /* enable save button if edit field is non null */
          DlgCheckOkEnable(hDlg, ID_EDIT, HIWORD(lParam));
          break;

        default:
          return FALSE;
        }
      break;

    default:
          return FALSE;
    } /* end switch message */

  return TRUE;
  }


/*=============================================================================
 DLGINITSAVEAS will initialize the save as dialog box.	It puts the current dir
 string in the ID_PATH field and initializes the edit box with the proposed
 filename to save.  The proposed filename is null if pof->cBytes==0, or if cur-
 rent dir==path in *pof, its the filename part of *pof, otherwise its fully qualified.
=============================================================================*/
VOID DlgInitSaveAs(HWND hDlg, OFSTRUCT *pof)
  {
    char    rgch[150];
    PSTR    lpchFileName;
    PSTR    lpchCmp;
    PSTR    lpchTest;
    int     cchCurDir;

    /* Initialize Path field with current directory */
    DlgDirList(hDlg, 0, 0, ID_PATH, 0);

    /* Initialize EditField */
    if (pof->cBytes != 0) {
        /* rgch gets current directory string, terminated with "\\\0" */
	cchCurDir = GetDlgItemText(hDlg, ID_PATH, rgch, 128);
        if (rgch[cchCurDir-1] != '\\') {
            rgch[cchCurDir] = '\\';
	}
	rgch[++cchCurDir] = 0;

	/* Now see if path in reopen buffer matches current directory. */
	for (lpchFileName = pof->szPathName,
	     lpchTest = DlgPFileInPath(pof->szPathName),
	     lpchCmp = rgch;
	     lpchFileName < lpchTest;
	     lpchFileName = AnsiNext(lpchFileName), lpchCmp = AnsiNext(lpchCmp)) {

	    if (*lpchFileName != *lpchCmp)
		break;
	}
        /* If paths don't match, reset pchFileName to point to fully qualified
	   path. (Otherwise, lpchFileName already points to filename. */
	if (lpchFileName != lpchTest || *lpchCmp)
	    lpchFileName = pof->szPathName;
	SetDlgItemText(hDlg, ID_EDIT, lpchFileName);

    /* pof->cBytes == 0 => edit field empty, so disable ok button. */
    } else {
	EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
    }

} /* end dlginitsaveas */


/*=============================================================================
 DLGPFILEINPATH returns the filename of the given filename which may include path.
=============================================================================*/
char * DlgPFileInPath(sz)
char *sz;
{
    char    *pch;

    /* Strip path/drive specification from name if there is one */
    pch = (char *)AnsiPrev(sz, (sz + strlen(sz)));
    while (pch > sz) {
	pch = (char *)AnsiPrev(sz, pch);
        if (*pch == '\\' || *pch == ':') {
	    pch = (char *)AnsiNext(pch);
	    break;
	}
    }
    return(pch);

} /* end dlgpfileinpath */


DlgAddExt(char *sz)
  {
  char *pch1;
  int  ch;

  pch1 = sz + strlen(sz);

  while ((ch = *pch1) != '.' && ch != '\\' && ch != ':' && pch1 > sz)
      pch1 = (char *)AnsiPrev(sz, pch1);

  if (*pch1 != '.')
      strcat(sz, pszExt+2);
  }


/*=============================================================================
 DLGMERGESTRINGS scans string1 for merge spec (%%). If found, insert string2 at
 that point, and then append remainder of string1.  Result in string3.
==============================================================================*/
BOOL DlgMergeStrings (szSrc, szMerge, szDst)
 char	*szSrc;
 char	*szMerge;
 char	*szDst;
 {
    char *pchSrc;
    char *pchDst;
    unsigned int wMerge;

    wMerge = '%';
    wMerge <<= 8;
    wMerge |= '%';

    pchSrc = szSrc;
    pchDst = szDst;

    /* Find merge spec if there is one. */
    while (*(unsigned *)pchSrc != wMerge) {
	*pchDst++ = *pchSrc;

	/* If we reach end of string before merge spec, just return. */
	if (!*pchSrc++)
	    return FALSE;

    }
    /* If merge spec found, insert sz2 there. (check for null merge string */
    if (szMerge) {
	while (*szMerge)
	    *pchDst++ = *szMerge++;

    }

    /* Jump over merge spec */
    pchSrc++,pchSrc++;


    /* Now append rest of Src String */
    while (*pchDst++ = *pchSrc++);
    return TRUE;

} /* end dlgmergestrings */

