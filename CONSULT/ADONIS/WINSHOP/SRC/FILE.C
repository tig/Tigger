/*************************************************************\
 ** FILE:  file.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **
 ** HISTORY:
 **   Doug Kent  March 1, 1990 (overhauled from Lori's version)
 **
\*************************************************************/

#include "nowin.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ws.h"
#include "misc.h"
#include "comm.h"
#include "database.h"
#include "winundoc.h"
#include "libids.h"
//#include "viewbox.h"
#include "download.h"
#include "rc_symb.h"
#include "error.h"
//#include "custinfo.h"
#include <io.h>      /* for access() and eof() */
//#include "order.h"
#include "libmgr.h"
#include "file.h"
#include "dos.h" // UpdateLocal
#include <string.h> // for memmove
#include "busy.h"
#include "help.h"
#include "messages.h"

/******************************************************************************
*  Stuff for the UptoDate List of database files
*******************************************************************************/

typedef struct
{
   DWORD base;
   WORD  ext;
} FILENODE;
#define FNSIZE sizeof(FILENODE)

typedef struct
{
   DWORD base;
   WORD  ext;
   WORD size;
} FILENODE_WITHSIZE;
#define FNSIZE_SIZE sizeof(FILENODE_WITHSIZE)

HANDLE hUptodate = NULL; // is == -1, shuts down subsystem
FILENODE NEAR *pUptodate = NULL;
static int UptodateCount = 0;
static int nCurNodeIdx = -1;

static BOOL AlreadyCheckedVersion(DWORD lFileNum, WORD* wpNeedExt);
static int AddToUptodateList(DWORD dwFileNum, WORD wExtNum);
static int RemoveFromUptodateList(DWORD dwFileNum);
static void NextNode(FILENODE* fnP);
static int TruncUptodateList(void);

static BOOL GetFileFromAltDirWhileOnLine (DWORD dwFileNum);

static WORD LRC(char *sIn);

char szCategoryFileName[_MAX_PATH];
char szPublishersFileName[_MAX_PATH];

extern BOOL bPromptedForBackup; // from viewbox.c

/*************************************************************\
 ** InitFileSystem
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Set filename variables:
 **     szPublishersFileName (read from MasterFile)
 **     szCategoryFileName  (read from MasterFile)
 **  If any other these three files don't exist or can't be read, then
 **  will try to login (via LoadFile())
 **  This routine should set up enough to run WS without logging
 **  on, if necessary files are present.
 **
 ** ASSUMES:
 **  szMasterFileName is set (via call to GetMasterFileName())
 **
 ** MODIFIES:
 **  Issues error messages.
 **
 ** RETURNS:
 **  OK if successful, FAIL if unable to locate files on disk.
 **
 ** HISTORY:   Doug Kent  February 28, 1990 (written)
\*************************************************************/
int FAR PASCAL	
InitFileSystem(void)
{
    int fp= -1;
    int nRetval=OK;

    /* open master file */
    if (LoadFile(szMasterFileName) != OK)
        END(FAIL)

    if ((fp = _lopen(szMasterFileName,READ)) == -1)
    {
        ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);
        END(FAIL)
    }

    /* read header */
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)

    /* read publishers list file name */
    if (ReadToChar(fp,'\t') == FAIL)
    {
        ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);
        END(FAIL)
    }
    if (ReadStringToChar(fp, szPublishersFileName, '\n', MAX_STR_LEN) == FAIL)
    {
        ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);
        END(FAIL)
    }

    /* read category list file name */
    if (ReadToChar(fp,'\t') == FAIL)
    {
        ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);
        END(FAIL)
    }
    if (ReadStringToChar(fp,szCategoryFileName,'\n', MAX_STR_LEN) == FAIL)
    {
        ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);
        END(FAIL)
    }

    AddFullPath(szPublishersFileName);
    AddFullPath(szCategoryFileName);

    if (LoadFile(szPublishersFileName) != OK)
        END(FAIL)

    if (LoadFile(szCategoryFileName) != OK)
        END(FAIL)

    end:
    if (fp != -1)
        close(fp);

    return nRetval;
}

/*************************************************************\
 ** LoadFile
 **
 ** ARGUMENTS:
 **  char *szFileName
 **
 ** DESCRIPTION:
 **  This function does the following:
 **  1) If szFileName has no extension, then the extension is added
 **     by finding a file on disk with the same basename and using
 **     the extension owned by that file.
 **  2) If no such file exists, or the file is out-of-date and we're
 **     connected, then the file is downloaded.
 **  3) If not connected, then user may choose to copy file from floppy.
 **
 **  Basically, we do everything possible to obtain the dogon file and only
 **  fail if all means are exhausted.
 **
 ** ASSUMES:
 **  Full path is prepended to szFileName.
 **
 ** MODIFIES:
 **  Issues error messages.
 **
 ** RETURNS:
 **  OK if successful (either finding file or downloading it), FAIL if not.
 **  will also return FAIL if bNewUser is TRUE.
 **
 ** HISTORY:   Doug Kent  February 28, 1990 (adapted from Lori's version)
 **
\*************************************************************/
int FAR PASCAL
LoadFile(char *szFileName)
{
	 int     nRetval=OK;
    LONG    lFileNum;
    WORD    wExtNum;
    BOOL    bIsThumbFile,bOutOfDate=FALSE;
    char    szOldFileName[_MAX_PATH];

    lFileNum = GetFileNum(szFileName);
    wExtNum = GetExtNum(szFileName);

    bIsThumbFile = (bIS_THUMBNAIL_FILE(lFileNum) OR
                      bIS_THUMBNAIL_INFO_FILE(lFileNum));

    TRYAGAIN:
    /**
        Try again to Load the file.  Note that WSConnect()
        called InitFileSystem() which loads several files.
        If we are here in the first place because of a LoadFile()
        call from InitFileSystem(), then if we try again to Load
        it here we will be repeating the effort already concluded by
        WSConnect()'s call to InitFileSystem().
        Note also that WSConnect() sent a message to the main window
        which resulted in filling the main dialog and the Loading
        all of the files necessary for its display.  The file
        we currently are dealing with may or may
        not be one of those files.
        We will go ahead and do a potentially repetitive
        second try; it should succeed.  If it doesn't, then we'll
        be issuing duplicate error messages!
        Later note: the AlreadyCheckedVersion Function should handle
        this problem nicely, in addition to reducing many other
        unnecessary LatestVersion() calls.

        (10.4.90) D. Kent -- Later note: I added the AlreadyChecked
        subsystem which completely solves the redundancy problem
        described above, though I haven't deleted the old code
        from the previous system.
    **/

    /** check whether this is a "NULL" filename, meaning the file does
        not exist and is not meant to exist. **/
    if (NOT lFileNum)
        return OK; /* enum and query functions will handle this case */

    if (bConnect)
    {
        //check GetExt first to make sure file was not deleted since logging on
        /* this'll fail if filename with extension doesn't exist,  or
           if more than one with basename exist.  Error only for latter. */
        if (GetExt(szFileName) == OK)
        {
            if (AlreadyCheckedVersion(lFileNum, NULL))
               END(OK)

            switch(LatestVersion(szFileName,NULL))
            {
                case FAIL:
                    // bbs isn't responding
                    END(OK)
                break;
                case TRUE:
                    AddToUptodateList(lFileNum, GetExtNum(szFileName));
                    END(OK)
                break;
                // case FALSE, fall through...
            }
            /** else out-of-date file and logged on, always download.
                Fall through to download... **/

            bOutOfDate = TRUE;
            /* save original file name for deleting */
            strcpy(szOldFileName, szFileName);
            /* get ridda extension so download will get most recent version */
            *strrchr(szFileName,'.') = '\0';
        }
        else if (InqErr())
        /** too many versions **/
        {
            IssueErr();
            END(FAIL)
        }
        // else file doesn't exist, fall through to download
    }
    else /* not connected */
    {
        if (GetExt(szFileName) == OK) /* file exists */
            END(OK)
        else
            if (InqErr())
            /** too many versions **/
            {
                IssueErr();
                END(FAIL)
            }
            else // file doesn't exist
            {
                if (bIsThumbFile)
                {
                    /** They have the option to get off of a floppy. Otherwise they
                        must download the file. **/

                    while (1) // try to retrieve...
                    {
                        switch (RetrieveLibFromDrive(szFileName))
                        // tried alternate drive...
                        {
                           case IDB_LOGON:
                               END(FAIL)
                           break;

                           case IDB_CONTINUE_SEARCH:
                               END(IDB_CONTINUE_SEARCH)
                           break;

                           case IDS_ERR_DISK_FULL:
                           case FAIL:
                           case IDCANCEL:    // uSER WANTS TO SKIP IT or cancel search
                           default:
                               // really screwed.  Give up.
                               END(FAIL)

                           case ID_GET_FROM_FLOPPY:
                           case IDS_ERR_MISSING_ALT_DRIVE:
                               // fall through to decision dialog...
                           break;

                           case OK: // got the file from floppy
                               goto TRYAGAIN; /** this invokes AlreadyCheckedVersion() check (see note above) **/
                           break;
                        } //    end switch Retrieve (if want to obtain from alt drive)

                        switch(GetAltDrive())
                        {
                            case IDCANCEL:
                                END(FAIL)
                            break;

                            case IDB_THUMBDIR_A:
                            case IDB_THUMBDIR_B:
                            case IDB_THUMBDIR_C:
                                // loop back up to RetrieveLibFromDrive()
                            break;
                        }
                    } // end of while(1) try to retrieve
                } // end LIBRARY file
                else // database file
                {
                    /** if newuser press NEWUSER **/
                    if (bNewUser)
                    {
                        ErrMsg(IDS_MSG_SUBSCRIBE);
                        END(FAIL)
                    }
                    else
                        ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);
                    /* fall through to connect and download... */
                } // end database file
            } // end file doesn't exist
    } // end not connected

    //EnableWindow(GetActiveWindow(),FALSE);

    CONNECT:
    if (NOT bConnect)
    {
        if (IsWindow(hWndView) OR IsWindow(hWndSearch)) // Can't Login in these modes
           END(FAIL)

        switch(WSConnect(TRUE))
        {
            case IDCANCEL:
                if (NOT bIsThumbFile)
                    ErrMsg(IDS_ERR_MUST_LOGON);
                END(FAIL)
            break;
            case FAIL:
                /* error msg is issued */
                END(FAIL)
            break;
            case OK:
                // fall through to download...
                /*
                   WAIT!! with new Update system we may have downloaded the needed
                   file (if its not a thumbfile file) so lets go back upstairs to
                   TRYAGAIN....
                */
                if (NOT bIsThumbFile)
                  goto TRYAGAIN; //else fall through to download
            break;
        }
    }
    /*
       If the needed file exists in the alt thumb directory (and the alt
       directory is NOT on a floppy, let's check the extension of the file
       in the alt directory and get it from ther if its OK.  This will be
       faster than downloading a new one.
    */

    if (bIsThumbFile AND GetFileFromAltDirWhileOnLine(lFileNum))
    {
        if (GetExt(szFileName) == OK)
        {
           AddToUptodateList(lFileNum, GetExtNum(szFileName));
           goto TRYAGAIN; /** this invokes AlreadyCheckedVersion() check (see note above) **/
        }
     }

    /*---------------------------------------------------------------
       See if wanna back thumbnail files up.
      ----------------------------------------------------------------*/
    if (bIsThumbFile AND !bPromptedForBackup)
    {
		   WORD	Answer;
         SetDest (ES_ACTIVEWND, NOBEEP, MB_ICONQUESTION | MB_YESNOCANCEL, IDS_TITLE_APPNAME);
		   Answer = ErrMsg (IDS_MSG_WANT_TO_BACKUP, (LPSTR)custinfo.altcache);
		   if ( Answer == IDCANCEL )
		   	END(FAIL)
		   else
           	bBackupThumb = (Answer == IDYES);

        if (bBackupThumb && SetBackupDrive() != OK)
            bBackupThumb = FALSE;

        bPromptedForBackup = TRUE;
    }

    /** download posthaste **/
    if ( Download(szFileName, szDataPath, FALSE, FALSE, bIsThumbFile,
                  FALSE, FALSE, &wExtNum) != OK )
    {
        IssueErr();
        //if (NOT bIsThumbFile)
            //ErrMsg(IDS_ERR_MISSING_ADMIN_FILES);

        // else msg will be issued from viewbox
        //EnableWindow(GetActiveWindow(),TRUE);
        END(FAIL)
    }
    else /** successful download **/
    {
        //EnableWindow(GetActiveWindow(),TRUE);
        if (bOutOfDate)
        /* then now we have two versions.  We need to delete the old one */
        {
            if (bIsThumbFile) // delete old guy from cache
                if (HitCache(szOldFileName, TRUE) != OK)
                    IssueErr();
            unlink(szOldFileName); // delete after HitCache()
        }
        else // it was missing altogether
        if (bIsThumbFile)
            GetExt(szFileName); // for HitCache() below

        /* whether bOutOfDate or not, ... */
        if (bIsThumbFile) // add new guy to cache
            if (HitCache(szFileName, FALSE) != OK)
               IssueErr();

        AddToUptodateList(lFileNum, wExtNum);

        goto TRYAGAIN; /** this invokes AlreadyCheckedVersion() filter (see note above) **/
    }

    end:
    return nRetval;
}

int FAR PASCAL
GetDecision(void)
{
    FARPROC    lpproc;
    int nRetval;

    EnableAllModeless(FALSE);
    lpproc = MakeProcInstance((FARPROC)WSDecisionDlgProc, hInstanceWS);
    nRetval = DialogBox(hInstanceWS, (LPSTR)"MISSINGTHUMB", hWndWS, lpproc);
    FreeProcInstance(lpproc);
    EnableAllModeless(TRUE);
    return nRetval;
}

int FAR PASCAL
GetAltDrive(void)
{
    FARPROC    lpproc;
    int nRetval;
    EnableAllModeless(FALSE);
    lpproc = MakeProcInstance((FARPROC)WSGetAltDriveDlgProc, hInstanceWS);
    nRetval = DialogBox(hInstanceWS, (LPSTR)"GETALTDRIVE", hWndWS, lpproc);
    FreeProcInstance(lpproc);
    EnableAllModeless(TRUE);
    return nRetval;
}

int FAR PASCAL
UpdateLocalDataBase(void)
/* NOTE side-effect of leaving active window disabled!!! */
{
    struct find_t c_file;
    int           whichtype = 0,
                  nRetval   = OK;
    char          filespec[_MAX_PATH];
    DWORD         dwCount = 0;
    DWORD         dwLocalDataFileSum = 0;
    DWORD         dwBBSDataFileSum = 0;
    LONG          lFileNum;
    WORD          wExtNum;
    BOOL          doWhatsNew = FALSE;
    char          szListfile[_MAX_PATH];
    FILE*         fList = (FILE*) -1;
    HCURSOR       hCursor = NULL;
    FARPROC       lpprocStatus  = NULL;
#define NUM_FILESPECS 4
    static char  *file_type[NUM_FILESPECS] =
    {
          "000*.*",      // All below LIBINFO + LIBINFO
          "001*.*",      // remaining LIBINFO
          "10*.*",       // PUBINFO
          "40*.*"        // Whats New
    } ;

//#define LRCSTATS
#ifdef LRCSTATS
    static int LRCStats[256];
    int x;
    char szDebug[5];
    FILE* fLRC;
    fLRC = fopen("LRC.log", "w");
#endif

    CloseFileCheckSystem(AND_RESET);   //Start with clean slate

    if (Send_Message(MSG_QUERY_DATA_LRC, &dwBBSDataFileSum) != OK)
    {
        ErrMsg(IDS_ERR_PLEASE_WAIT);
        END(FAIL)
    }

    for ( whichtype = 0; whichtype < NUM_FILESPECS; ++whichtype )
    {
        lstrcpy(filespec, szDataPath);
        lstrcat (filespec, file_type[whichtype]);

        if (!_dos_findfirst(filespec, _A_NORMAL, &c_file))
        {
            do
            {
                if ((lFileNum = GetFileNum(c_file.name)) AND
                    (NOT bIS_LRU_FILE(lFileNum)))
                {
                    dwLocalDataFileSum += (DWORD)LRC(c_file.name);
                    AddToUptodateList (lFileNum, GetExtNum(c_file.name));
#ifdef LRCSTATS
                    ++LRCStats[LRC(c_file.name)];
                    fprintf(fLRC, "LRC for %s is %d\n", c_file.name, LRC(c_file.name));
#endif
                }
            } while (!_dos_findnext(&c_file));
        }
    }
#ifdef INTERNAL
    AuxPrint("Based on %d local files", UptodateCount);
    AuxPrint("Local Data LRC is = %5ld", dwLocalDataFileSum);
    AuxPrint("  BBS Data LRC is = %5ld", dwBBSDataFileSum);
#endif
#ifdef LRCSTATS
    fclose(fLRC);
    for (x = 0; x < 256; ++x)
    {
       wsprintf((LPSTR)szDebug, (LPSTR)"%4d", LRCStats[x]);
       OutputDebugString((LPSTR)szDebug);
    }
    OutputDebugString((LPSTR)"\r\n");
    Wait(10000L);
#endif

    // Compare FILELIST against Uptodate List, deleting files found only
    // in the UptoDateList, downloading files which are out-of-date,
    // ignoring the rest

    if (dwLocalDataFileSum != dwBBSDataFileSum)
    {
         FILENODE fnCurLocal;
         FILENODE_WITHSIZE fnCurRemote;
         char filename[_MAX_PATH];
         int nNumBlocks = 0;
         long time1,time2;
         WORD wNumTestBlocks = 0;

         // download the Master list of database files from the BBS, first deleting
         // any old copies of it, don't track filesize of listfile for cache

         // delete old copy
         strcpy(szListfile, szFILELIST);
         AddFullPath(szListfile);

         if (GetExt(szListfile) == OK)
            unlink(szListfile);
         RemoveFromUptodateList(FILELIST);

         //for estimated time of update record time it takes to
         //download this file
         time1 = GetCurrentTime();
         if ( Download (szFILELIST, szDataPath,
                        FALSE, FALSE, FALSE, FALSE, FALSE, &wExtNum) != OK )
         {
             IssueErr ();
             END (FAIL)
         }
         time2 = GetCurrentTime();

         AddToUptodateList(FILELIST, wExtNum);

         wsprintf((LPSTR)szListfile, (LPSTR)"%s%s.%03X",
                  (LPSTR)szDataPath, (LPSTR)szFILELIST, wExtNum);

         if ((fList = fopen(szListfile, "rb")) == NULL)
         {
             IssueErr ();
             END (FAIL)
         }

        // Read number of following records from head of fList
        if (fread(&dwCount, sizeof(DWORD), 1, fList) != 1)
        {
             IssueErr ();
             END (FAIL)
        }
        nCurNodeIdx = 0;

/*****************************************************************************
         LOOP TO DETRMINE TOTAL NUMBER OF BLOCKS THAT WILL BE DOWNLOADED
         DURING UPDATE
******************************************************************************/

        //Load the first two files from the local list and the master file list
        if (fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) != 1)
        {
            IssueErr();
            END(FAIL)
        }

        NextNode(&fnCurLocal);
        while (fnCurLocal.base)  //more local filenodes
        {
            if (NOT bConnect)
            {
               IssueErr();
               END(FAIL)
            }
            if (fnCurRemote.base == FILELIST)
               wNumTestBlocks = fnCurRemote.size;
            if (fnCurRemote.base > fnCurLocal.base)   // this file is obsolete
            {
               ++nCurNodeIdx;
            }
            else if (fnCurRemote.base == fnCurLocal.base)
            {
               if (fnCurRemote.ext != fnCurLocal.ext)
                   nNumBlocks += (fnCurRemote.size - 1);

               //if we're out of remote files, any further local files can
               //be deleted, we ensure that by making the remote file 0xFFFFFFFF
               //Deletion will occur at the top of the loop
               if (fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) != 1)
                   fnCurRemote.base = 0xFFFFFFFF;
               ++nCurNodeIdx;
            }
            else // CurRemote.base < CurLocal.base i.e. we're missing one
            {
               nNumBlocks += (fnCurRemote.size - 1);
               if (fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) != 1)
                   fnCurRemote.base = 0xFFFFFFFF;
            }
            NextNode(&fnCurLocal);
        }   //While more local nodes
        do
        {
            if (fnCurRemote.base AND (fnCurRemote.base != 0xFFFFFFFF))
               nNumBlocks += (fnCurRemote.size - 1);

        }  while ( fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) == 1);

        //reset stuff for redoing loop
        nCurNodeIdx = 0;

        //move fileptr just past Number of records header
        fseek(fList, sizeof(DWORD), SEEK_SET);

        //Create the update status dialog
        if (nNumBlocks) //in case we just need to erase obsolete files, don't put up dld dlg
        {
            lpprocStatus = MakeProcInstance((FARPROC)WSDownloadSingleDlgProc, hInstanceWS);
            if (NOT CreateDialog(hInstanceWS, (LPSTR)"DOWNLOAD", hWndWS, lpprocStatus))
            {
#ifdef INTERNAL
                AuxPrint("Could not load Download Dialog");
#endif
                END(FAIL)
            }
            else
            {
                //initialize est time and parts in job
                SendMessage(hDldWnd, MM_SET_DOWNLOAD_TYPE, IDS_MSG_UPDATE_DATABASE, 0L);
                SendMessage(hDldWnd, MM_SET_EST_TIME, 0, nNumBlocks*(time2-time1)/wNumTestBlocks/1000L);
                SendMessage(hDldWnd, MM_SET_PARTS_IN_JOB, nNumBlocks, 0L);
                SendMessage(hDldWnd, MM_SET_DEST_DIR, 0, (LONG)((LPSTR)szDataPath));
            }
        }

#ifdef INTERNAL
        //AuxPrint("Total number of blocks to download is %d", nNumBlocks);
        //AuxPrint("Time to download LISTFILE in secs was %ld", (time2-time1)/1000L);
        //AuxPrint("Estimated download time in secs is %ld", nNumBlocks*(time2-time1)/wNumTestBlocks/1000L);
        //Wait(10000L);
#endif
/******************************************************************************
        THIS LOOP DOWNLOADS THE OUT-OF-DATE FILES
******************************************************************************/

        //Load the first two files from the local list and the master file list
        if (fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) != 1)
        {
            IssueErr();
            END(FAIL)
        }

        /* set to phone cursor to so cursor doesn't flash back to hourglass
            after every call to download.
        */
        if (IsWindow(hDldWnd))
            hCursor = SetCursor(hPhoneCursArr[0]);
        NextNode(&fnCurLocal);
        while (fnCurLocal.base)  //more local filenodes
        {
            if (NOT bConnect)
            {
               IssueErr();
               END(FAIL)
            }
            if (fnCurRemote.base > fnCurLocal.base)   // this file is obsolete
            {
               wsprintf((LPSTR)filename, (LPSTR)"%s%08lX.%03X",
                        (LPSTR)szDataPath, fnCurLocal.base, fnCurLocal.ext);
               unlink(filename);
               RemoveFromUptodateList(fnCurLocal.base);
#ifdef INTERNAL
               AuxPrint("Removed %s from the database", filename);
#endif
            }
            else if (fnCurRemote.base == fnCurLocal.base)
            {
               if (fnCurRemote.ext == fnCurLocal.ext) // these files are OK, get the next ones
               {
                   //if we're out of remote files, any further local files can
                   //be deleted, we ensure that by making the remote file 0xFFFFFFFF
                   //Deletion will occur at the top of the loop
                   if (fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) != 1)
                       fnCurRemote.base = 0xFFFFFFFF;
                   else
                   {
                       if ((nNumBlocks) AND (NOT hDldWnd)) //download terminated
                          END(FAIL)
                   }
                   ++nCurNodeIdx;
               }
               else  //extensions not equal, need current file
               {
                   wsprintf((LPSTR)filename, (LPSTR)"%s%08lX.%03X",
                            (LPSTR)szDataPath, fnCurLocal.base, fnCurLocal.ext);
                   unlink(filename);

                   wsprintf((LPSTR)filename, (LPSTR)"%08lX.%03X",
                              fnCurRemote.base, fnCurRemote.ext);
                   //update download dialog
                   if (IsWindow(hDldWnd))
                     SendMessage(hDldWnd, MM_SET_FILE_NAME, 0, (LONG)((LPSTR)filename));

                   /* get ridda extension to download most recent version */
                   BaseName(filename);

                   if ( Download (filename, szDataPath,
                                  TRUE, FALSE,
                                  FALSE, FALSE,
                                  FALSE, NULL) != OK )
                   {   // we don't know what happened so get rid of both files
                       //sprintf(filename, "%s%08lX.%03X", szDataPath, fnCurRemote.base, fnCurRemote.ext);
                       //unlink(filename);
                       IssueErr();
                       END (FAIL)
                   }
                   //this will just change ext since file is already in list
                   AddToUptodateList(fnCurRemote.base, fnCurRemote.ext);
                   if (bIS_WHATS_NEW_FILE(fnCurRemote.base))
                      doWhatsNew = TRUE;
               }
            }
            else // CurRemote.base < CurLocal.base i.e. we're missing one
            {
               wsprintf((LPSTR)filename, (LPSTR)"%08lX.%03X",
                              fnCurRemote.base, fnCurRemote.ext);

               //update download dialog
               if (IsWindow(hDldWnd))
                   SendMessage(hDldWnd, MM_SET_FILE_NAME, 0, (LONG)((LPSTR)filename));

               BaseName(filename);  //remove extension
               if ( Download (filename, szDataPath,
                              TRUE, FALSE,
                              FALSE, FALSE,
                              FALSE, NULL) != OK )
               {   // we don't know what happened so get rid of new file
                   //sprintf(filename, "%s%08lX.%03X", szDataPath, fnCurRemote.base, fnCurRemote.ext);
                   //unlink(filename);
                   IssueErr();
                   END (FAIL)
               }
               AddToUptodateList (fnCurRemote.base, fnCurRemote.ext);
               //do not increment nodeIdx so we can check file again
               if (bIS_WHATS_NEW_FILE(fnCurRemote.base))
                  doWhatsNew = TRUE;
            }
            NextNode(&fnCurLocal);
        }   //While more local nodes
        do     //if finished local list and items remain on Remote List
        {
            if (fnCurRemote.base AND (fnCurRemote.base != 0xFFFFFFFF))
            {
               if (NOT bConnect)
                   END(FAIL)
               wsprintf((LPSTR)filename, (LPSTR)"%08lX.%03X",
                              fnCurRemote.base, fnCurRemote.ext);
               //update download dialog
               if (IsWindow(hDldWnd))
                   SendMessage(hDldWnd, MM_SET_FILE_NAME, 0, (LONG)((LPSTR)filename));

               /* get ridda extension to download most recent version */
               BaseName(filename);
               if ( Download (filename, szDataPath,
                              TRUE, FALSE,
                              FALSE, FALSE,
                              FALSE, NULL) != OK )
               {   // we don't know what happened so get rid of new file
                   //sprintf(filename, "%s%08lX.%03X", szDataPath, fnCurRemote.base, fnCurRemote.ext);
                   //unlink(filename);
                   IssueErr();
                   END (FAIL)
               }
               AddToUptodateList (fnCurRemote.base, fnCurRemote.ext);
               if (bIS_WHATS_NEW_FILE(fnCurRemote.base))
                  doWhatsNew = TRUE;
               if ((nNumBlocks) AND (NOT hDldWnd)) //download terminated
                  END(FAIL)
            }
        }  while ( fread(&fnCurRemote, sizeof(fnCurRemote), 1, fList) == 1);
    }    //end if Databases !=

    end:


    if (fList != (FILE*)-1)
        fclose(fList);

    if (IsWindow(hDldWnd))
    {
         DestroyWindow( hDldWnd );
         FreeProcInstance( lpprocStatus );
         hDldWnd = NULL;
         if (hCursor)
             SetCursor(hCursor);
    }

    if (nRetval == FAIL)
       --nCurNodeIdx;  // Don't trust current file to be verified
    TruncUptodateList();  //Remove any from list not checked (according to position of nCurNodeIdx)

    //Reset static vars
    nCurNodeIdx = -1;
    dwLocalDataFileSum = dwBBSDataFileSum = 0L;

    if ( doWhatsNew )
        ErrMsg (IDS_MSG_CHECK_WHATS_NEW);

    return nRetval;
}

/*************************************************************\
 ** AlreadyCheckedVersion
 **
 ** ARGUMENTS:
 **  DWORD dwFileNum, WORD* wpNeedExt
 **
 ** DESCRIPTION:
 **  Only call if logged on.  If hUptodate contains lFileNum, then
 **  file is up-to-date, return TRUE.  Else return FALSE.
 **  If the wpNeedExt is not NULL then the calling function would like to
 **  know the extension of the file as well.
 **
 ** ASSUMES:
 **  hUptodate contains previously-checked lFileNums (see AddToUptodateList())
 **  in sorted order (small to large).
 **  hUptodate == -1 disables subsystem
 **
 ** MODIFIES:
 **          *wpNeedExt
 ** RETURNS:
 **  TRUE or FALSE.
 **
 ** HISTORY:   Doug Kent  September 23, 1990 (written)
\*************************************************************/

static BOOL
AlreadyCheckedVersion(DWORD dwFileNum, WORD* wpNeedExt)
{
    int start,end,x;
    BOOL nRetval;

    if ((hUptodate == NULL) OR (hUptodate == -1))
        END(FALSE)

#ifdef INTERNAL
    //AuxPrint("Pre hUptodate Lock: Local Heap is %d", LocalCompact(0));
#endif
    if ((pUptodate = (FILENODE NEAR *)LocalLock(hUptodate)) == NULL)
    {
        ErrMsg(IDS_ERR_NO_MEMORY);
        CloseFileCheckSystem( AND_DISABLE );
        END(FALSE)
    }

    start = 0;              // first item in list
    end = UptodateCount-1;  // last  item in list

    while ( end >= start )
    {
        x = (start + end) >> 1;
        if ( dwFileNum < pUptodate[x].base )
            end = x - 1;
        else
            start = x + 1;
        if ( dwFileNum == pUptodate[x].base )
        {
            if (wpNeedExt)
               *wpNeedExt = pUptodate[x].ext;
            END(TRUE)
        }
    }

    END(FALSE)

    end:
    if (pUptodate)
        LocalUnlock(hUptodate);

    pUptodate = NULL;

    return nRetval;
}


/*************************************************************\
 ** AddToUptodateList
 **
 ** ARGUMENTS:
 **  DWORD lFileNum, WORD wExtNum
 **
 ** DESCRIPTION:
 **  Add lFileNum to hUptodate, maintaining list in sorted order,
 **  small to large.  Creates hUptodate if not created already.
 **  If dwFileNum is already in list its extension is changed to wExt
 **
 ** ASSUMES:
 **  lFileNum is not already in list.
 **
 ** MODIFIES:
 **  hUptodate array.
 **  Reallocates memory.
 **  Issues error message if out-of-memory.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  September 23, 1990 (written)
\*************************************************************/
static int
AddToUptodateList(DWORD dwFileNum, WORD wExtNum)
{
    int nRetval=OK;
    int start, end, x;

    switch(hUptodate)
    {
        case -1:
            END(OK)  // system is disabled

        case NULL:
            if ((hUptodate = LocalAlloc(LMEM_MOVEABLE, FNSIZE)) == NULL)
            {
                ErrMsg(IDS_ERR_NO_MEMORY);
                END(FAIL)
            }
        break;
        default:
#ifdef INTERNAL
            //AuxPrint("Pre hUptodate ReAlloc: Local Heap is %d", LocalCompact(0));
#endif
            if ((hUptodate = LocalReAlloc(hUptodate,
                                          FNSIZE * (UptodateCount+1),
                                          LMEM_MOVEABLE)) == NULL)
            {
                ErrMsg(IDS_ERR_NO_MEMORY);
                END(FAIL)
            }
#ifdef INTERNAL
            //AuxPrint("Post hUptodate ReAlloc: Local Heap is %d", LocalCompact(0));
#endif
        break;
    }

    if ((pUptodate = (FILENODE NEAR *)LocalLock(hUptodate)) == NULL)
    {
        ErrMsg(IDS_ERR_NO_MEMORY);
        END(FAIL)
    }

    /* enough, now add the sucker */
    start = 0;              // first item in list
    end = UptodateCount-1;  // last  item in list

#ifdef INTERNAL
    //AuxPrint("(BEFORE) end: (%x)=>0x%lx, start: (%x)=>0x%lx, adding: 0x%lx",
        //end,pUptodate[end],start,pUptodate[start],lFileNum);
#endif

    while ( end >= start )
    {
        x = (start + end) >> 1;
        if ( dwFileNum < pUptodate[x].base )
            end = x - 1;
        else if ( dwFileNum > pUptodate[x].base )
            start = x + 1;
        else if ( dwFileNum == pUptodate[x].base )
        {
            pUptodate[x].ext = wExtNum;
            END(OK)
        }
    }

    /* now insert after end, before start */
#ifdef INTERNAL
    //AuxPrint("(AFTER)  end: (%x)=>0x%lx, start: (%x)=>0x%lx, adding: 0x%lx",
        //end,pUptodate[end],start,pUptodate[start],lFileNum);
#endif
    memmove(pUptodate + start, pUptodate + end, FNSIZE * (UptodateCount - end));

    pUptodate[start].base = dwFileNum;
    pUptodate[start].ext = wExtNum;
    ++UptodateCount;

    end:
    if (pUptodate)
        LocalUnlock(hUptodate);

    pUptodate = NULL;

    if (nRetval == FAIL)
    {
        LocalFree(hUptodate);
        hUptodate= -1; // disable this system
    }
    return nRetval;
}

/*************************************************************\
 ** TruncUptodateList
 **
 ** ARGUMENTS:
 **
 **
 ** DESCRIPTION:
 **  Ends the UptodateList at the last file checked, which is
 **  pointed to by static global nCurNodeIdx.  This function is called when
 **  the user terminates a database update.
 **
 ** ASSUMES:  nCurNodeIdx is valid
 **
 ** MODIFIES:
 **  hUptodate array.
 **  Reallocates memory.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  September 23, 1990 (written)
\*************************************************************/
static int
TruncUptodateList(void)
{
    int nRetval = OK;

    if ((hUptodate == NULL) OR
        (hUptodate == -1) OR
        (nCurNodeIdx < 0) OR
        (nCurNodeIdx >= UptodateCount))
        END(OK)

    // Decrease size of hUptodate buy one node
#ifdef INTERNAL
    //AuxPrint("Pre hUptodate Trunc ReAlloc: Local Heap is %d", LocalCompact(0));
#endif
    if ((hUptodate = LocalReAlloc(hUptodate,
                                  FNSIZE * nCurNodeIdx,
                                  LMEM_MOVEABLE)) == NULL)
       nRetval = FAIL;

#ifdef INTERNAL
    //AuxPrint("Post hUptodate Trunc ReAlloc: Local Heap is %d", LocalCompact(0));
#endif
    end:
    if (nRetval == FAIL)
    {
        LocalFree(hUptodate);
        hUptodate= -1; // disable this system
    }
    return nRetval;
}

/*************************************************************\
 ** RemoveFromUptodateList
 **
 ** ARGUMENTS:
 **  DWORD dwFileNum
 **
 ** DESCRIPTION:
 **  Delete dwFileNum from hUptodate, maintaining list in sorted order.\,
 **  small to large.  If dw FileNum is not in list no action is taken.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **  hUptodate array.
 **  Reallocates memory.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  September 23, 1990 (written)
\*************************************************************/
static int
RemoveFromUptodateList(DWORD dwFileNum)
{
    int start, end, x;
    int nRetval = OK;

    if ((hUptodate == NULL) OR (hUptodate == -1))
        END(OK)

    if ((pUptodate = (FILENODE NEAR *)LocalLock(hUptodate)) == NULL)
    {
        ErrMsg(IDS_ERR_NO_MEMORY);
        END(FAIL)
    }

    /* enough, now remove the sucker */
    start = 0;              // first item in list
    end = UptodateCount-1;  // last  item in list

#ifdef INTERNAL
    //AuxPrint("(BEFORE) end: (%x)=>0x%lx, start: (%x)=>0x%lx, adding: 0x%lx",
        //end,pUptodate[end],start,pUptodate[start],lFileNum);
#endif

    while ( end >= start )
    {
        x = (start + end) >> 1;
        if ( dwFileNum < pUptodate[x].base )
            end = x - 1;
        else if ( dwFileNum > pUptodate[x].base )
            start = x + 1;
        else if ( dwFileNum == pUptodate[x].base )
        {
            /* now move array back one NODE starting at X+1, overwriting X */
            memmove(pUptodate + x, pUptodate + x + 1,
                    FNSIZE * (--UptodateCount - x));
            break;
        }
    }

    end:

    if (pUptodate)
        LocalUnlock(hUptodate);

    // Decrease size of hUptodate buy one node
    if ((hUptodate) AND
        (hUptodate = LocalReAlloc(hUptodate,
                                  FNSIZE * (UptodateCount+1),
                                  LMEM_MOVEABLE)) == NULL)
    nRetval = FAIL;

    pUptodate = NULL;

    if (nRetval == FAIL)
    {
        LocalFree(hUptodate);
        hUptodate= -1; // disable this system
    }
    return nRetval;
}

void
CloseFileCheckSystem(int nEndState)
{
    if ((pUptodate) AND (hUptodate != -1))
        LocalUnlock(hUptodate);
    pUptodate = NULL;
#ifdef INTERNAL
    //AuxPrint("Pre hUptodate Free: Local Heap is %d", LocalCompact(0));
#endif
    if ((hUptodate) AND (hUptodate != -1))
        LocalFree(hUptodate);
#ifdef INTERNAL
    //AuxPrint("Post hUptodate Free: Local Heap is %d", LocalCompact(0));
#endif
    hUptodate = nEndState; // disable or reset system
    UptodateCount = 0;
}


/*
   This function calculates the LRC value for a given string.
   For this implementation skip leading zeros in filenames (to spread out range
   of resulting values).

   The purpose of this function is to calculate a semi-unique value for each
   filename in the database.  The calling function sums these values up to
   produce (hopefully) a unique value for each state of the database, so that
   when this value is compared to that generated on the Server (using the same
   function) it will serve to tell us if the two databases are the same without
   having to compare them file by file.

   This system will not work very well if this function does not generate values
   over a wide range, different filenames may evaluate to the same value and
   the two databases may look unique when they are not

   ASSUMES that strlen(sIn) is > 2.
      In this case this is assured because we're only dealing with database
      files that have a form of 08X.03X with at least one significant digit in
      both the basename and extension.
*/

WORD LRC(char *sIn)
{
    WORD temp = 0;

   //skip leading zeros
   while (*sIn == '0')
		++sIn;

   //get started, assumes strlen(sIn) is still > 2
   temp = (sIn[0] ^ sIn[1]);
   sIn++;
   sIn++;

   // rest of string
   while (*sIn)
   {
		   temp ^= *sIn;
		   sIn++;
   }
   return temp;
}

static void
NextNode(FILENODE* fnP)
{
    memset(fnP, 0, FNSIZE);

    if (nCurNodeIdx >= UptodateCount)
         return;

    if ((hUptodate == NULL) OR (hUptodate == -1))
        return;

    if (nCurNodeIdx < 0)
        return;

    if ((pUptodate = (FILENODE NEAR *)LocalLock(hUptodate)) == NULL)
    {
        ErrMsg(IDS_ERR_NO_MEMORY);
        CloseFileCheckSystem( AND_DISABLE );
        return;
    }

    fnP->base = pUptodate[nCurNodeIdx].base;
    fnP->ext  = pUptodate[nCurNodeIdx].ext;

    if (pUptodate)
        LocalUnlock(hUptodate);

    pUptodate = NULL;
}


/*************************************************************\
 ** GetFileFromAltDirWhileOnLine (char *szFile)
 **
 ** ARGUMENTS:
 **   char *szFile
 **
 ** DESCRIPTION:
 **  We want to first look in alt directory for a file before we download one
 **  (if its uptodate)
 **  Sees if szFile exists in szExePath\thumb (if IT exists).
 **  If this it does exist than we ask if its the latest version.
 **  If all true than calls RetrieveFile.
 **
 ** ASSUMES:
 **  szFile may be anything as long as it contains at least a basename.
 **  we are connected (on-line)
 **
 ** MODIFIES:
 **  Nothing, cache updating done in RetrieveFile.
 **  Current extension is added to szFile, path (if any) remains intact.
 **
 ** RETURNS:
 **  TRUE if successful, FALSE if not.
 **
 ** HISTORY:   beng 6/26/91
\*************************************************************/
static BOOL
GetFileFromAltDirWhileOnLine (DWORD dwFileNum)
{
    char szSrcPath[_MAX_PATH];
    char szBaseName[_MAX_PATH];
    int nEndOPath;

    // Construct a shopper\thumb directory
    strcpy(szSrcPath, szExePath);
    if (NOT LoadString(hInstanceWS, IDS_THUMB_DIR,
                       (LPSTR)(szSrcPath + strlen(szSrcPath)),
                       sizeof(szSrcPath) - strlen(szExePath)))
    {
       return FALSE;
    }
    /* can we open src directory? */
    if (NOT DirExists(szSrcPath))
        return (FALSE);

    nEndOPath = strlen(szSrcPath);
    // Construct qualified path to file
    wsprintf((LPSTR)szBaseName, (LPSTR)"%08lX", dwFileNum);
    Append(szSrcPath, szBaseName);

    /* this gets extension, szSrcPath should now contain full path to file,
       including extension, of file in alt directory*/
    if (GetExt(szSrcPath) == OK)
    {
        if (LatestVersion(szSrcPath, NULL))
        {
           szSrcPath[nEndOPath] = '\0';  //First arg to RetrieveFile() is just pathname
           if (RetrieveFile(szBaseName, szSrcPath) == OK)
           {
               return TRUE;
           }
        }
    }
    return FALSE;
}

