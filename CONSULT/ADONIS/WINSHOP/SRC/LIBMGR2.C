/*************************************************************\
 ** FILE:  libmgr2.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **
 ** HISTORY:   
 **   Doug Kent  July 27, 1990 (created)
 **
\*************************************************************/

#include <nowin.h>
#include <windows.h>
#include <winundoc.h>
#include <stdio.h>
#include <io.h>
#include "fcntl.h"
#include "ws.h"
#include "error.h"
#include "database.h"
#include "rc_symb.h"
#include "libmgr.h"
#include "file.h"
#include "string.h"
#include "misc.h"
#include <sys\types.h>
#include <sys\stat.h>
#include "download.h"
#include "comm.h"
#include "help.h"
#include "direct.h"

static char szPath[_MAX_PATH];
static BYTE nSelectDirDlgMode;

static int RetrieveThumbnails(char *szLibFile, char *szSrcPath);
static BOOL DecompressFile(char *szSrcPath, char *szDestPath);

static int ExtractLibFromCache(char *szLibFile, char *szDestPath, BOOL bDelete);
static int ExtractThumbFromCache(char *szSrc,char *szDestPath, BOOL bDelete);

/*************************************************************\
 ** ExtractFromCache
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  For either a single thumbres or single thumbinfo or all of
 **  the thumbres & thumbinfos in a libinfo file, 
 **  Copy file from cache or delete file from cache.
 **  if bDelete then delete the said files else copy the files to
 **  szPath.  szPath is set by the user in WSLibDriveDlgProc.
 **  If file with same basename in szPath already exists, it is deleted.
 **
 ** ASSUMES:
 **  szSrcFileName has full path and extension.
 **  szPath is set to destination for !bDelete case (see WSLibDriveDlgProc()
 **    and SetBackupDrive())
 **
 ** MODIFIES:
 **  Cache stuff if bDelete.
 **  Issues error message.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not, IDCANCEL if cancelled by user.
 **
 ** HISTORY:   Doug Kent  August 9, 1990 (written)
\*************************************************************/
int
ExtractFromCache(char *szSrcFileName, BOOL bDelete)
{
    LONG lFileNum;
    BOOL bIsLibInfo;

    lFileNum = GetFileNum(szSrcFileName);
    bIsLibInfo = bIS_LIBINFO_FILE(lFileNum);

    if (bIsLibInfo)
    {
        if (ExtractLibFromCache(szSrcFileName,szPath,bDelete) != OK)
        {
            IssueErr();
            return FAIL;
        }
        else
            return OK; // assume caller is handling LRU/Cache
    }
    else /* is a thumbnail, and is currently in compressed form */
    {
        if (ExtractThumbFromCache(szSrcFileName,szPath,bDelete) != OK)
        {
            IssueErr();
            return FAIL;
        }
        else
            return OK;
    }
}

static int
ExtractLibFromCache(char *szLibFile, char *szDestPath, BOOL bDelete)
{
	 int fp= -1;
    int nRetval=OK;
    char szThumbInfo[_MAX_PATH], szThumbRes[_MAX_PATH];

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
    {
        SetErr(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE,
               (LPSTR)StripPath(szLibFile));
        return FAIL;
    }

    /* skip to sublibs */
    if (ReadToChar(fp,'\n') == FAIL)
        END(FAIL)

    while (readch(fp) != '')
    {
        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from ExtractLibFromCache(), 1");
#endif
        if (ReadStringToChar(fp, szThumbInfo, '\t', sizeof(szThumbInfo)) == FAIL)
            END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from ExtractLibFromCache(), 2");
#endif
        if (ReadStringToChar(fp,szThumbRes,'\n', sizeof(szThumbRes)) == FAIL)
            END(FAIL)

        /** don't worry if can't delete for now (assume its only because it
            doesn't exist rather than somehow got read-only) **/
        ExtractThumbFromCache(szThumbInfo,szDestPath,bDelete);
        ExtractThumbFromCache(szThumbRes,szDestPath,bDelete);
    }

end:
    if (fp != -1)
        close(fp);

    if (NOT InqErr() AND (nRetval != OK))
        SetErr(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
               (LPSTR)StripPath(szLibFile));

   return(nRetval);
}

/*************************************************************\
 ** ExtractThumbFromCache
 **
 ** ARGUMENTS:
 **     char *szSrc       :
 **     char *szDestPath  :
 **     BOOL bDelete      :
 **
 ** DESCRIPTION:
 **  Copy (or delete) szSrc from ExePath directory.  If bDelete
 **  then delete, else copy szSrc into szDestPath directory.
 **  It only deletes when it is not copying.
 **  Checks that there exists enuf disk space on szDest.
 **  If deleting, returns OK if file already not there.
 **
 ** ASSUMES:
 **  szDestPath is only a path.
 **  szSrc may be anything as long as it contains at least a basename.
 **  szSrc is a ThumbInfo or a ThumbRes
 **
 ** MODIFIES:
 **  Cache contents stuff if bDelete.
 **  Sets error condition.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  June 12, 1990 (written)
\*************************************************************/
int
ExtractThumbFromCache(char *szSrc,char *szDestPath, BOOL bDelete)
{
    /* if already has an extension, then don't get a new one:  We may be
       operating on one of a set of files with same basename, so GetExt()
       would fail.  If there is an extension, then presume caller knows
       what she wants.  */

    //Better not delete current thumbnail files
    if (bDelete AND
       ((GetFileNum(szSrc) == GetFileNum(CURTHUMBINFO)) OR
        (GetFileNum(szSrc) == GetFileNum(CURTHUMBNAIL))))
    {
#ifdef INTERNAL
        AuxPrint("I Can't delete current thumbfile!!!");
#endif
        return OK;
    }
    AddFullPath(szSrc);
    if (szSrc[strlen(szSrc)-4] != '.')
        if (GetExt(szSrc) != OK)
            /* this gets extension */
            if (InqErr()) // more than one with basename
            {
                //IssueErr();
                return FAIL;
            }
            else // doesn't exist
            {
                if (bDelete)
                    return OK;
                else
                    SetErr(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE,
                           (LPSTR)StripPath(szSrc));
                return FAIL;
            }

    if (bDelete)
    {
        /** !! maybe should turn off read-only attr's just in case? **/

        /* adjust amount in cache first while file still exists */
        HitCache(szSrc,TRUE);

        if (unlink(szSrc))
        {
            HitCache(szSrc,FALSE); // put back in
            return FAIL;
        }
        else
        {
#ifdef INTERNAL
            AuxPrint("Deleted %s from cache!", (LPSTR)StripPath(szSrc));
#endif
            return OK;
        }
    }
    else
    {
        char szDest[_MAX_PATH],
        szFileToDelete[_MAX_PATH];

        strcpy(szDest,szDestPath);

        Append(szDest, StripPath(szSrc));
        strcpy(szFileToDelete, szDest);

        if (GetExt(szFileToDelete) == OK)
            unlink(szFileToDelete); /* if there are any already there
                        with this extension, we delete them first */

        if (ConfirmCopyOK (szSrc, szDestPath, 0L, FALSE) != OK )
        {
            //IssueErr();
            return FAIL;
        }

        if (CopyFile(szSrc,szDest) != OK)
        {
            //IssueErr();
            return FAIL;
        }
        else return OK;
    }
}

int FAR
SetBackupDrive(void)
{
    extern BOOL bConnect;

    /* Can we open alt thumbnail directory? Then set szPath (global) to it.
       If they are logged on, then getting to the backup drive is not as
       important and we will NOT tell them to insert a disk, etc ..pt 7/91 */

    if (DirExists((char*)custinfo.altcache))
       lstrcpy(szPath, Append(custinfo.altcache, NULL)); //need trailing backslash
    else
    {
        if (!bConnect)
            ErrMsg(IDS_ERR_MISSING_ALT_DRIVE, (LPSTR)custinfo.altcache, (LPSTR) CURPUB);
        return(IDS_ERR_MISSING_ALT_DRIVE);
    }

    return (OK);
}

/*************************************************************\
 ** ConfirmCopyOK
 **
 ** ARGUMENTS: char * szSrc,   :
 **            char * szDest   : 
 **            DWORD dwCompress :
 **            BOOL bCheckCache :
 **
 ** DESCRIPTION: 
 **  Determines if there is adequate disk space on szDest
 **  for szSrc.
 **  
 **  If szSrc is not NULL then:
 **  If dwCompress == 0 then check for size of szSrc.
 **  
 **  If dwCompress == 1 then check for size of szSrc in its
 **  uncompressed form (ie szSrc is compressed, szSrc * 3).
 **
 **  If szSrc is NULL then dwCompress is the size of the file
 **  that we're checking.
 **  
 **  If bCheckCache, **  then we check against the maximum cache 
 **  limit as well as disk space.
 **  
 ** ASSUMES: 
 **  That the source and dest are fully qualified
 **  with drive,path,and file spec. szDest is just a path, whereas
 **  szSrc contains a filename.
 **  
 **  The szSrc and szDest exist.
 **
 ** MODIFIES:
 **  Sets the error condition.
 **
 ** RETURNS:
 **  OK if successful, error code if not.
 **
 ** HISTORY:   Dug n' John  May 31, 1990 (written)
 ** MODIFIED: John (6.20.90)
 **  (7.2.90) D. Kent -- only check against cache if bCheckCache.
 **  (8.9.90) D. Kent -- calls MakeCacheSpace
 **
\*************************************************************/
int FAR PASCAL
ConfirmCopyOK(char *szSrc, char *szDest, DWORD dwCompress, BOOL bCheckCache)
{  
    DWORD dwFileSize ;
    int nRetval = OK;
    int iWhichDrive;


    /** FIRST GET DWFILESIZE **/
    if (!szSrc)         /* use dwCompress */
        dwFileSize = dwCompress;
    else
    {
        dwFileSize = FileSize(szSrc);
        if (!dwFileSize)
        {
            SetErr(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
                  (LPSTR)StripPath(szSrc)) ;
            END(IDS_FILE_IO_ERR)
        }

        if (dwCompress == 1L)
            dwFileSize *= 3;
    }

    if (bCheckCache)
    {
        /* check that it will not overrun the cache */
        if (QueryCacheLimit(dwFileSize) != OK)
        {
            /* try to free space in the cache */
            if (MakeCacheSpace(dwFileSize) != OK)
            {
                SetErr(IDS_ERR_CACHE_FULL);
                END(IDS_ERR_CACHE_FULL)
            }
        }
    }

    /* what drive is it? */  
    iWhichDrive = tolower(szDest[0]) - 'a' + 1;

    /* check that it will not overrun total unused disk space at the destination */
    /* always do this too! */
    if (dwFileSize > AvailDiskSpace(iWhichDrive))
    {
        SetErr(IDS_ERR_DISK_FULL);
        END(IDS_ERR_DISK_FULL)
    }
    /* if we made it this far, then we are in business */
    /* return OK */

end:
return (nRetval) ;
}


/*************************************************************\
 ** RetrieveLibFromDrive
 **
 ** ARGUMENTS:
 **  char *szDestFileName
 **
 ** DESCRIPTION:
 **  szFileName can be either thumb or libinfo.  If libinfo then
 **  we operate on all thumbs in all sublibs of library.  Otherwise
 **  operate only on thumb.   Operation is to get the said files
 **  from the alternate directory and put them in the cache.  
 **  Those files are assumed to be compressed.
 **
 ** ASSUMES:
 **  szDestFileName must be fully qualified.
 **
 ** MODIFIES:
 **  Issues error msg.
 **  If cache is full then removes thumbnails (library at a time) 
 **  from cache, as many as required to make space.
 **  After retrieving, updates cache and LRU stuff accordingly.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not, IDCANCEL if cancelled by user.
 **
 ** HISTORY:   Doug Kent  May 25, 1990 (written)
\*************************************************************/
int
RetrieveLibFromDrive(char *szDestFileName)
{
    LONG lFileNum;
    int nRetval=OK;
    BOOL bIsLibInfo;

    lFileNum = GetFileNum(szDestFileName);
    bIsLibInfo = bIS_LIBINFO_FILE(lFileNum);

    if (bIsLibInfo) /* if libinfo  */
    {
        if ((nRetval = RetrieveThumbnails(szDestFileName,custinfo.altcache)) != OK)
            return nRetval;
        else /* successful retrieval of all thumbs. */
        {
            /* at resolution of a libinfo, we must update LRU */
            if (SearchLRUTree(szDestFileName, &hLRU,FALSE) != OK)
            {
                IssueErr();
                if (RemakeLRU() != OK)
                    return FAIL;
            }
            return OK;
        }
    }
    else /* retrieve a thumb only. */
    {
        return RetrieveFile(szDestFileName, custinfo.altcache);
    }
}

static int
RetrieveThumbnails(char *szLibFile, char *szSrcPath)
{
	int fp= -1;
    int nRetval=OK;
    char szThumbInfo[_MAX_PATH],szThumbRes[_MAX_PATH];
    
    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
    {
        SetErr(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE,
               (LPSTR)StripPath(szLibFile));
        return FAIL;
    }

    /* skip to sublibs */
    //if (ReadToChar(fp,'\n') == FAIL)
        //return 0;
    if (ReadToChar(fp,'\n') == FAIL)
        return FAIL;

    while (readch(fp) != '')
    {
        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from RetrieveThumbnails(), 1");
#endif
        if (ReadStringToChar(fp,szThumbInfo,'\t', sizeof(szThumbInfo)) == FAIL)
            END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from RetrieveThumnbnails(), 2");
#endif
        if (ReadStringToChar(fp,szThumbRes,'\n', sizeof(szThumbRes)) == FAIL)
            END(FAIL)
        if ((nRetval = RetrieveFile(szThumbInfo,szSrcPath)) != OK)
            END(nRetval)
        if ((nRetval = RetrieveFile(szThumbRes,szSrcPath)) != OK)
            END(nRetval)
    }
    
end:
    if (fp != -1)
        close(fp);
	
    if (NOT InqErr() AND (nRetval != OK))
        SetErr(IDS_FILE_IO_ERR, ID_ERR_READING_FILE,
               (LPSTR)StripPath(szLibFile));
	
   return(nRetval);
}

/*************************************************************\
 ** RetrieveFile
 **
 ** ARGUMENTS:
 **   char *szDest    :
 **   char *szSrcPath :
 **
 ** DESCRIPTION:
 **  Tries to get szSrcPath\szDest from szSrcPath into
 **  szExePath.  Decompresses it as it copies.
 **
 ** ASSUMES:
 **  szSrcPath is only a path.  
 **  szDest may be anything as long as it contains at least a basename. szDest
 **  must be declared tp be _MAX_PATH chars big.
 **  szSrcPath\szDest is compressed.
 **
 ** MODIFIES:
 **  Issues error message.
 **  Removes libs from cache if necessary to free up space.
 **  Full path is added to szDest
 **
 ** RETURNS:
 **  OK if successful, error code if not.
 **
 ** HISTORY:   Doug Kent  May 28, 1990 (written)
 **  (9.3.90) D. Kent (redone with new return values, deleted
 **  all prompting dialogs.)
\*************************************************************/
int
RetrieveFile(char *szDest, char *szSrcPath)
{
    char szSrc[_MAX_PATH];
    int nRetval=OK;

    /** append \\ if only a drive **/
    if (!szSrcPath[2])
        Append(szSrcPath,NULL);

    /* can we open src directory? */
    if (NOT DirExists(szSrcPath))
    {
        ErrMsg(IDS_ERR_MISSING_ALT_DRIVE, (LPSTR)szSrcPath, (LPSTR)CURPUB);
        return(IDS_ERR_MISSING_ALT_DRIVE);
    }

    strcpy(szSrc,szSrcPath);

    Append(szSrc, StripPath(szDest));

    if (GetExt(szSrc) == OK)
    /* this gets extension */
    {
        AddFullPath(szDest);

        if ( (nRetval = ConfirmCopyOK(szSrc, szDest, 1L, TRUE)) != OK)
        {
            IssueErr();
            return nRetval;
        }

        /* copy extension to dest */
        strcpy(StripPath(szDest), StripPath(szSrc));
        if ((nRetval = DecompressFile(szSrc, szDest)) != OK)
        {
            IssueErr();
            return nRetval;
        }
        else
        {
            /* update that we have a new file in the cache */
            if ((nRetval = HitCache(szDest,FALSE)) != OK)
            { 
                IssueErr();
                return nRetval;
            }
            return OK;
        }

    }
    else if (InqErr()) // more than one with basename
    {
        IssueErr();
        return IDS_IO_ERR;
    }
    else // doesn't exist
    {
        int nResult = GetDecision();
        return nResult;
    }
}

extern void FAR PASCAL initDilute(void FAR *);

/*************************************************************\
 ** DecompressFile
 **
 ** ARGUMENTS:
 **   char *szDest    :
 **   char *szSrcPath :
 **
 ** DESCRIPTION:  Decompress the source file into destination file
 **
 ** ASSUMES:
 **  szSrcPath is only a path.  
 **  szDest may be anything as long as it contains at least a basename.
 **  szSrcPath\szDest is compressed.
 **
 ** MODIFIES:
 **  Sets error condition
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  June 20, 1990 (written)
\*************************************************************/
static BOOL
DecompressFile(char *szSrcPath, char *szDestPath)
{
#define BUFFER_SIZE     128

    int     fpin = -1, fpout = -1;
    int     nNumRead;
    int     nRetval = OK;
    char    inbuf[BUFFER_SIZE];
    HCURSOR hCursor;
    int savefmode = _fmode;

    _fmode = O_BINARY;

    hCursor= SetCursor(LoadCursor(NULL,IDC_WAIT)) ;

    if ( (fpin = _lopen ((LPSTR)szSrcPath, READ)) == -1 ) 
    {
        SetErr(IDS_IO_ERR,ID_ERR_DECOMPRESSING);
        END(IDS_IO_ERR)
    }

    if ( (fpout = _lcreat ((LPSTR)szDestPath, 0)) == -1 )
    {
        SetErr(IDS_IO_ERR,ID_ERR_DECOMPRESSING);
        END(IDS_IO_ERR)
    }

#ifdef INTERNAL
    AuxPrint("Decompressing %s into %s", szSrcPath, szDestPath);
#endif

    /** initialize uncompression memory **/
    if ( init_buffers () != OK ) // SetErr called already
    {
        END(IDS_ERR_GMEM_FULL)
    }
    initDilute (variables);

    while ( NOT eof (fpin) )
    /** read blocks **/
    {
        nNumRead = _lread (fpin, (LPSTR)inbuf, sizeof (inbuf));

#ifdef INTERNAL
        //AuxPrint ("Expanding buffer of size (%d).", nNumRead);
#endif
        expand_buf (inbuf, nNumRead, fpout, nNumRead < sizeof(inbuf));
    } /* end of read block loop */

end:
    if (fpin != -1)
        _lclose(fpin);
    if (fpout != -1)
        _lclose(fpout);

    _fmode = savefmode;
    close_buffers();
    SetCursor(hCursor);
    return nRetval;
    
}



/*************************************************************\
 ** WSLibDriveDlgProc
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return in szPath (a global) a path formatted such tath it may be used
 **  in call to access()/DirExists().
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  IDOK if successful, IDCANCEL if cancelled.
 **
 ** HISTORY:   Doug Kent  July 26, 1990 (revised from Lauren Bricker)
\*************************************************************/
#ifdef NEVER
LONG FAR PASCAL 
WSLibDriveDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) 
{
    static char cur_dir[_MAX_PATH] = "";
    char szTmp[_MAX_PATH+_MAX_PATH]; 

    switch(message) 
    {
        case WM_INITDIALOG:
            CenterPopUp(hWndDlg);

            SetErrorMode(1); // disable int24 message boxes
            if (DirExists(custinfo.altcache))
            {
                strcpy(cur_dir,custinfo.altcache);
                //MakeParentDir(cur_dir);
            }
            SetErrorMode(0); // enable int24 message boxes

            FixSpec(cur_dir);
            DlgDirList(hWndDlg, cur_dir, ID_DIR, ID_CURDIR, 0xC010);
            get_cur_dir(cur_dir);


            /* set edit box to default length */
            SendDlgItemMessage(hWndDlg, ID_EDIT, EM_LIMITTEXT,
                               (WORD)_MAX_PATH, 0L);

            if (bShowUseAltCacheBox)
                CheckDlgButton(hWndDlg,ID_USEALTDIR,custinfo.useAltCache);

            ShowWindow(GetDlgItem(hWndDlg,ID_USEALTDIR),
                    bShowUseAltCacheBox ? SW_SHOW : SW_HIDE);

            if (!bRetrieve)
            {
                char szTitle[MAX_STR_LEN+1];
                LoadString(hInstanceWS,IDS_TITLE_BACKUP_DIR,szTitle,sizeof(szTitle)-1);
                SetWindowText(hWndDlg,szTitle);
            }
            break;

        case WM_COMMAND:
            switch (wParam)
            {
                case ID_HELP:
                    GetHelp(GH_TEXT,IDH_LIBDRIVE);
                    break;

                case ID_DIR:
                    switch(HIWORD(lParam))
                    {
                        case LBN_DBLCLK:
                            /* get selection */
                            DlgDirSelect(hWndDlg, szTmp, ID_DIR);
                            /* reset list box to new directory listing */
                            FixSpec(szTmp);
                            DlgDirList(hWndDlg, szTmp, ID_DIR, ID_CURDIR, 0xC010);
                            SetDlgItemText(hWndDlg, ID_EDIT, "");
                            get_cur_dir(cur_dir);
                            break;

                        case LBN_SELCHANGE:
                            /* append edit box to current selection and place in edit box */
                            DlgDirSelect(hWndDlg, szTmp, ID_DIR);
                            if (szTmp[1] == ':')
                            // give drive a trailing backslash
                                strcat(szTmp,"\\");

                            SetDlgItemText(hWndDlg, ID_EDIT, (LPSTR)szTmp);
                            break;
                        default:
                            return FALSE;
                    }
                    break;

                case IDOK:
                {
                    int count;

                    count = GetDlgItemText(hWndDlg, ID_EDIT, szTmp, sizeof(szTmp));

                    if ( !count ) 
                    /* then DLB clicked to here or used default, cur_dir is it. */
                    {
                        Append(cur_dir,NULL);
                        if ( !stricmp (cur_dir, szDataPath) )
                        {
                            SetDest (hWndDlg, NOBEEP, MB_OK | MB_ICONEXCLAMATION, IDS_TITLE_APPNAME);
                            ErrMsg (IDS_ERR_BACKUP_TO_DIFF);
                        }
                        else
                        {
                            /* szPath is global */
                            lstrcat(cur_dir,"."); // for access()
                            lstrcpy(szPath,cur_dir);

                            if (bShowUseAltCacheBox)
                                custinfo.useAltCache = IsDlgButtonChecked(hWndDlg,ID_USEALTDIR);

                            EndDialog (hWndDlg, IDOK);
                        }
                        break;
                    }
                    else /* there's something in edit box.  Create it if doesn't
                            exist.  Go to it */
                    {
                        Append(szTmp,NULL); // trailing backslash
                        lstrcat(szTmp,"."); // for access()

#if 0
                        /* create subdir if doesn't exist */
                        if (NOT DirExists(szTmp))
                        {
                            // directory doesn't exist so make it...
                            if ( mkdir (szTmp) )
                            {
                                ErrMsg (IDS_ERR_CREATING_DIR, (LPSTR)szPath);
                                SetFocus(GetDlgItem(hWndDlg,ID_EDIT));
                                break;
                            }
                        }
#endif
                        FixSpec(szTmp);
                        DlgDirList(hWndDlg, szTmp, ID_DIR, ID_CURDIR, 0xC010);
                        SetDlgItemText(hWndDlg, ID_EDIT, "");
                        get_cur_dir(cur_dir);
                    }
                    break;
                }
                case IDCANCEL:
                    EndDialog(hWndDlg, IDCANCEL);
                    break;
                default:
                    return FALSE;
            }
            break;
        default:
            return(FALSE);
    } /* end switch message */
    return(TRUE);
}
#endif

