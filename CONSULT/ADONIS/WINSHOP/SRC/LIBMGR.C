/*************************************************************\
 ** FILE:  libmgr.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION: Window Shopper's smart lib manager.
 **              when the drive gets too full to accomodate
 **              the user's libraries, some of them "go away."
 **              Based on Least Recently Used (LRU) algorithm.
 ** HISTORY:   
 **   John Ano  May 23, 1990  Harry Belafonte Dub / Day-o remix
 **   mail all correspondence to /dev/null
\*************************************************************/
//put standard includes here
#include "nowin.h"
#include <windows.h>
#include "libmgr.h"
#include "ws.h"
#include "error.h"
#include <dos.h>
#include "rc_symb.h"
#include "database.h"
#include <fcntl.h>
#include "custinfo.h"
#include "misc.h"
#include "file.h"
#include "busy.h"
#include "settings.h"
#include <io.h>
#include <malloc.h>
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comm.h"
#include "winundoc.h"
#include "libids.h"
#include "viewbox.h"
#include "download.h"
#endif

/* a note about head pointer: head is given a value in some other code segment
   whenever the LRU table is GlobalLocked() .*/

/* global data  */
static LPIBSTNODE head ; /* head points to the root of the tree, root is the top node */
LRUHEADER FileHead;
static WORD nNumLRUsAllocd = 0;
static HANDLE hCursor = NULL ;

static void PASCAL DeleteNode(DWORD lLibInfoFile, WORD FAR *wChildPtr);
static DWORD PASCAL DeleteMin(WORD FAR *wChildPtr);
static void PASCAL TouchLib(WORD NodePtr, BOOL bDelete);

/*------------------------------------------------------------------------
*
*      Function:  InitLRUCache
*
*   Description:  Allocates Global Memory for LRU table.
*   Returns : a handle to a Global Block of memory or NULL if fail
*
*       History:  John Ano May 23, 1990 and it was a bear to write!
*
*------------------------------------------------------------------------*/
GLOBALHANDLE 
InitFileCache(LONG lreq_size) 
{
	return(GlobalAlloc(GHND, lreq_size));
}

/*************************************************************\
 ** RoutineName GrowLRUTable
 **
 ** ARGUMENTS:  GLOBALHANDLE hGlob -- current handle to the LRU Table.
 **             
 ** DESCRIPTION: reallocs the block with STD_GLOB_INCR more entries  
 **              if hGlob is NULL, it creates a block with STD_GLOB_INCR
 **              elements in it.
 ** ASSUMES: 
 **
 ** MODIFIES: REALLOC ONLY -- if the block must be moved to realloc
 **    at the new size, the handle will change.
 **           
 **
 ** RETURNS:  handle to the (re) alloced block
 **
 ** HISTORY:   John Ano  May 23, 1990 (written)
\*************************************************************/

GLOBALHANDLE FAR PASCAL
GrowLRUTable(GLOBALHANDLE hGlob)
// increase table size by standard increments
{
    GLOBALHANDLE hMem;
    DWORD dwNewSize;

    if (hGlob == NULL) 
    {
        /* create a block */
        hMem = GlobalAlloc(GHND, (DWORD)sizeof(IBSTNODE) * STD_GLOB_INCR);
        dwNewSize = GlobalSize(hMem);
        nNumLRUsAllocd = STD_GLOB_INCR ;
    }
    else
    {
        /* add to existing block */
        dwNewSize = GlobalSize(hGlob) + (sizeof(IBSTNODE) * STD_GLOB_INCR);
        hMem = GlobalReAlloc(hGlob, dwNewSize , GHND);
        nNumLRUsAllocd += STD_GLOB_INCR ;
    }
#if DEBUG
    AuxPrint("+LRU table is %ld bytes big", dwNewSize);
#endif
    return (hMem);
}

/*************************************************************\
 ** RoutineName WriteLRUToDisk
 **
 ** ARGUMENTS:  LPIBSTNODE head --pointer to the first element in the LRU struct
 **             
 ** DESCRIPTION:  Writes the LRU table in global mem to disk.  This is done 
 **      after a predefined number of updates made to the LRU table.  A final
 **      write is done when Shopper is closed down.
 **
 ** ASSUMES: 
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **
 ** HISTORY:   John Ano  May 23, 1990 (written)
\*************************************************************/


//ASSUMES: lru header is set and ready to be written.
int FAR PASCAL
WriteLRUToDisk(LPIBSTNODE Tmp)
/* Tmp is head (8.10.90) D. Kent */
{
    HANDLE fhLRU = NULL;
    int nRetval = OK;
    char szLRUPath[_MAX_PATH] ;

    hCursor= SetCursor(LoadCursor(NULL,IDC_WAIT)) ;

    /* open the file  */
    lstrcpy(szLRUPath,szLRUFileName);
    AddFullPath(szLRUPath);
    _fmode = O_BINARY;
    if ( (fhLRU = _lopen(szLRUPath,READ_WRITE)) == FAIL)
    {
        SetErr(IDS_FILE_IO_ERR, ID_ERR_OPENING_FILE, (LPSTR)szLRUFileName) ;
        END(FAIL)
    }

    /* write the header  */
    if (_lwrite(fhLRU, (LPSTR)&FileHead, sizeof(LRUHEADER)) < sizeof(LRUHEADER) )
    {
        SetErr(IDS_FILE_IO_ERR, ID_ERR_UPDATING_FILE, (LPSTR)szLRUFileName) ;
        END(FAIL)
    }

    /* write the table (including head) */
    if (_lwrite(fhLRU, (LPSTR) Tmp, FileHead.NumEntries * sizeof(IBSTNODE)) < (FileHead.NumEntries * sizeof(IBSTNODE)))
    {
        SetErr(IDS_FILE_IO_ERR, ID_ERR_UPDATING_FILE, (LPSTR)szLRUFileName) ;
        END(FAIL)
    }

end:
    if (fhLRU)
        _lclose(fhLRU);
    SetCursor(hCursor);
    _fmode = O_TEXT;

    return (nRetval) ;
}

/*************************************************************\
 ** RoutineName ReadLRUFromDisk
 **
 ** ARGUMENTS:  GLOBALHANDLE *hGlobTable -- handle to the LRU table
 **
 ** DESCRIPTION: read the LRU from disk.   Done at the beginning of each
 **              Window Shopper session in WSINIT.C  A handle to the block
 **              is created and returned in hGlobTable.
 ** ASSUMES: 
 **
 ** MODIFIES: handle to the table may change if it moved in response to realloc
 **
 ** RETURNS: OK if successful, FAIL if not
 **
 ** HISTORY:   John Ano  May 23, 1990 (written)
\*************************************************************/

int FAR PASCAL
ReadLRUFromDisk(GLOBALHANDLE *hGlobTable)
{
    HANDLE fhLRU=NULL;
    int nRetval;
    char szLRUPath[_MAX_PATH] ;

    /* open the file */
    hCursor= SetCursor(LoadCursor(NULL,IDC_WAIT)) ;
    lstrcpy(szLRUPath,szLRUFileName);
    AddFullPath(szLRUPath);
    _fmode = O_BINARY;
    if ((fhLRU = _lopen(szLRUPath,READ)) == FAIL)
    {  /* if fail, punt */
        SetErr(IDS_FILE_IO_ERR,ID_ERR_OPENING_FILE,(LPSTR)szLRUFileName) ;
        END(FAIL)
    }

    /* read the header  */
    if (_lread(fhLRU,(LPSTR) &FileHead,sizeof(LRUHEADER)) < sizeof(LRUHEADER) )
    {   
        SetErr(IDS_FILE_IO_ERR,ID_ERR_READING_FILE,(LPSTR)szLRUFileName);
        END(FAIL)
    }

    /* if there's time, add error checking here */
    /* set up the LRU table */
    /* when we go to final this should not be here anymore */
    /* get all memory alloc stuff in a separate function */

    /* get memory for table plus a few extra LRUs */
    nNumLRUsAllocd = FileHead.NumEntries + STD_GLOB_INCR;
    if ((*hGlobTable=InitFileCache((DWORD)sizeof(IBSTNODE) * nNumLRUsAllocd))
            == NULL)
    { /* if failed to init LRU table, punt  */
        SetErr(IDS_ERR_GMEM_FULL);
        END(FAIL)
    }

    head = (LPIBSTNODE)GlobalLock(*hGlobTable);

    /* read the table (including head) */
    if (_lread(fhLRU, (LPSTR) head, FileHead.NumEntries * sizeof(IBSTNODE)) < (FileHead.NumEntries * sizeof(IBSTNODE)))
    {   
        SetErr(IDS_FILE_IO_ERR,ID_ERR_READING_FILE,(LPSTR)szLRUFileName);
        END(FAIL)
    }

    end:
    GlobalUnlock(*hGlobTable);

    if (fhLRU)
        _lclose(fhLRU);   /* close the LRU table  */

    SetCursor(hCursor);
    _fmode = O_TEXT;
    return nRetval;
}


/*************************************************************\
 ** RoutineName AddEntryToLRU
 **
 ** ARGUMENTS:
 **            WORD wChild -- 16-bit value representing offset into the global array
 **            LONG lNewFile -- 4-byte value containing the value of the Libinfofilename
 **
 ** DESCRIPTION: recursive routine that adds libraries to tree or
 **              handles "touching" a library already in tree.
 **
 ** ASSUMES: 
 **    head is the pointer to the locked LRU table in global memory
 **
 ** MODIFIES:
 **  Issues and sets *nothing* in regard to error conditions.
 **
 ** RETURNS: 
 **  FAIL if error or index to the new node created. 
 **  Index is actually an offset from the first element in the list
 **
 ** HISTORY:   John Ano  May 23, 1990 (written)
 **  (8.13.90) D. Kent modified.
 **  (9.22.90) D. Kent added stack check to reconstruct on error
 ** 
\*************************************************************/

WORD FAR PASCAL 
AddEntryToLRU(WORD wChildPtr, LONG lNewFile)
{
    LPIBSTNODE p;
    LONG cond;
    WORD nRetval;

    if (stackavail() <= 512) // 512 is somewhat arbitrary
    {
#ifdef INTERNAL
        AuxPrint("%d bytes left on stack", stackavail());
#endif
        return FAIL;
    }

    /* if at the bottom of subtree or making the root */
    if (wChildPtr == NULL)
    {
        p = head + FileHead.NumEntries++;  /* add a new node to the end of the LRU table  */
        p->lLibInfoFile = lNewFile;           /* store libinfo filename */
        p->l=p->r=NULL;                       /* set the child pointers to NULL  */
    }
    else  /* If the node we're adding is already in the tree, then
             we simply 'touch' it by moving it to the end of the array 
             space and updating the affected pointers in the tree.  */
    {
        p = head + wChildPtr;  /* access the node */
        if ((cond = (lNewFile - p->lLibInfoFile)) == 0 )
            /* then search node already is in tree; touch it. */
            TouchLib(wChildPtr,FALSE);
        else if (cond < 0) /* new lib is smaller, go left subtree */
            nRetval = p->l = AddEntryToLRU(p->l,lNewFile);
        else   /* new lib is bigger, go right subtree */
            nRetval = p->r = AddEntryToLRU(p->r,lNewFile);
    }

    if (nRetval == FAIL)
       return FAIL;
    else
        return ((WORD) (p - head));  /* returns the index of the current node  */
}


/*************************************************************\
 ** RoutineName SearchLRUTree >> new name should be HitLRU <<
 **
 ** ARGUMENTS:  char *szLibFile -- name of the LIBINFO file to search for
 **             GLOBALHANDLE hGlobLRUTable -- handle to the LRU table in gmem
 **            
 **            
 **
 ** DESCRIPTION: 
 **  Add a new library, touch or delete an existing library in LRU.
 **  Given a filename, it locks the LRU table,
 **  and goes looking for a match.  If found, it replaces
 **  the old occurence with a new one appended to the       
 **  end of the LRU table.  If not found, it slaps the new
 **  filename at the end of the LRU table.
 **  Every 10th file that gets touched initiates a table save
 **  to disk and an increase in the size of the LRU table.
 **
 ** ASSUMES: 
 **
 ** MODIFIES: 
 **           hGlobTable is changed if realloc had to move the block around.
 **           LRU file header is updated. !!updates CacheUsed variable!!
 **           Sets the error condition
 **
 ** RETURNS:  OK if successful, FAIL if problems 
 **
 ** HISTORY:   John Ano  May 23, 1990 (written)
\*************************************************************/

int FAR PASCAL 
SearchLRUTree(PSTR szLibFile, GLOBALHANDLE *hGlobLRUTable,BOOL bDelete) 
{
    BOOL nRetval = OK;
    LONG lLibFile ;
    
    hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT)) ; /* put up the hourglass */
    lLibFile = GetFileNum(szLibFile) ; /* get the LONG of libinfo file */
    head = (LPIBSTNODE) GlobalLock(*hGlobLRUTable) ; /* lock the Table and point to the top of the table */
    /* head->r is the offset into the root of the LRU tree */
    /* is the libinfo in the LRU?  */

    if (bDelete) // (8.10.90) D. Kent
    {
        LPIBSTNODE p;
        WORD wNodeToDel; // zero-based offset into tree array

        for(p = head+1, wNodeToDel=1; 
            wNodeToDel < FileHead.NumEntries; // NumEntries includes head
            ++wNodeToDel, ++p)
            if (p->lLibInfoFile == lLibFile)
                break;

        if (wNodeToDel >= FileHead.NumEntries) // then node not found
            END(OK)

        DeleteNode(lLibFile,&(head->r));
        TouchLib(wNodeToDel,TRUE); // this'll delete node
        --FileHead.NumEntries;
        END(OK)
    }

    /* see whether need more memory for the guy we're adding */
    if (nNumLRUsAllocd == FileHead.NumEntries)
    {
        /* write the LRU to disk */
        if (WriteLRUToDisk(head) == FAIL) /* write away */
            {
            SetErr(IDS_ERR_DISK_FULL);  /* not true, but my error checking isn't that smart yet */
            END(FAIL)  /* could be outta file handles, etc. */
            }
        GlobalUnlock(*hGlobLRUTable); /* unlock the table */
        head = NULL;

        if ((*hGlobLRUTable = GrowLRUTable(*hGlobLRUTable)) == NULL) /* realloc the table for additional entries */
            {
            SetErr(IDS_ERR_GMEM_FULL); /* memory boo-boo */
            END(FAIL)
            }
        head = (LPIBSTNODE) GlobalLock(*hGlobLRUTable) ; /* lock the Table and point to the top of the table */
    }

    if ((head->r = AddEntryToLRU(head->r,lLibFile)) == FAIL) /* add the node */
    {
        SetErr(IDS_ERR_ADDLRU);
        END(FAIL)
    }


    end:
    SetCursor(hCursor) ; /* restore cursor */
    if (head)
        GlobalUnlock(*hGlobLRUTable) ;
    return (nRetval) ;
}

/*************************************************************\
 ** RoutineName HitCache
 **
 ** ARGUMENTS:BOOL bDelete : if set, dec cache by size of file 
 **           char *szFileName  : the thumbfile file being (de)cached.  
 **
 ** DESCRIPTION: updates the CacheUsed global to reflect 
 **     database additions to exePath.
 **     NOTE: currently does not check for cache overflow 
 **     as of BETA, only thumbnails (!setfiles, !publist, ..)  are being added to cache.
 **     however for release (rev 1 or 2) we want all database files represented
 **     in the cache.
 **
 ** ASSUMES: file has passed all other checks: 
 **  1) File exists
 **  2) szFileName is fully delimited.
 **
 ** MODIFIES: global cache variable is updated.
 **    Sets error condition
 **
 ** RETURNS: OK if success, FAIL if problems.
 **
 ** HISTORY:   John Ano  July 10, 1990 (written)
 **  (8.24.90) D. Kent changed response to overflow.
\*************************************************************/
 
int FAR PASCAL 
HitCache(char *szFileName, BOOL bDelete)
{
    DWORD dwFileSize;

    dwFileSize = FileSize(szFileName);

    if (!bDelete) /* add to cache */
    {
    /* note to myself: when we go to rev2, all database files will be */
    /* included in the cache (*.exe excluded).  */
    /* at that point, use this code and modify CCOK().  */

        /* 1) check for cache overflow.  This may easily happen because
           at this time we only estimate the size of thumbnails before
           decompressing.  Sometimes the estimate is low.  Its not
           fatal, however, but sometimes we may break our promise
           not to exceed cache limit.  We recover it though the next
           time they go to put something in the cache because then 
           we'll get it back below by deleting thumbnails. 
           The whole problem would be solved by putting a header
           into compressed files that provides uncompressed filesize.
           (8.24.90) D. Kent */
        FileHead.CacheUsed += dwFileSize ;

        if (FileHead.CacheUsed > dwCacheSize)
#ifdef INTERNAL
           AuxPrint("HitCache warning:   we've exceeded cache limit.");
#endif

        return OK ;
    }
    else /* remove from cache */
    {
    /* what kind of operations need to be performed here? */
        FileHead.CacheUsed -= dwFileSize ; /* yoiks and away! */
        return OK;
    }
}


/*************************************************************\
 ** RoutineName MakeCacheSpace
 **
 ** ARGUMENTS: 
 **  long lBytesNeeded : (long cause using signed math on it)
 **
 ** DESCRIPTION: Deletes LRU'd libraries until lBytesNeeded are freed.
 **  If lBytesNeeded == -1L then just delete enuf to fit into
 **  the cache.
 **
 ** ASSUMES: 
 **  hLRU is global handle to LRU table
 **
 ** MODIFIES:
 **  Issues error msg.
 **
 ** RETURNS: OK if successful, FAIL if not
 **
 ** HISTORY:   John Ano  May 25, 1990 (written)
 ** MODIFIED June 3, 1990
 **  MODIFIED (8.9.90) D. Kent
\*************************************************************/

int FAR PASCAL 
MakeCacheSpace(long lBytesNeeded)
{
    char szLibInfo[_MAX_PATH] ;
    int nRetval=OK;
    LPIBSTNODE LRUTable , saveroot;

    /* library go bye-bye */
    hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT)) ; /* put up the eon-glass */

    if (FileHead.CacheUsed < 0)
    /* how does it get here ? */
        if (RemakeLRU() != OK)
            END(FAIL)

    saveroot = ((LPIBSTNODE) GlobalLock(hLRU))  + 1 ; /* lock the LRU table  */

    while ((FileHead.NumEntries > 1) AND (QueryCacheLimit(lBytesNeeded) != OK)) /* always want at least one lib in LRU */
    {
        /** LRU'd are at beginning of array.  Deletion may have 
            rearranged, so reset each time. */
        LRUTable = saveroot;

        LTOS(LRUTable->lLibInfoFile, szLibInfo) ;
        AddFullPath(szLibInfo);
        GetExt(szLibInfo) ;
        if (!access(szLibInfo,0)) /* if the library was found and can be accessed */
        { /* found a library so zap it */
#if 0
            SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNOCANCEL,
				IDS_TITLE_APPNAME);
            switch (ErrMsg(IDS_ERR_REMOVE_LIBRARY))
            {
            case IDYES: /* remove the lib if possible */
                FileHead.CacheUsed -= SumLibSize(LRUTable->lLibInfoFile) ;  /* remove it from the LRU */
                if (ExtractFromCache(szLibInfo,TRUE) == OK)   /* second param is DELETE flag */
                    zapped = TRUE ;
                break;
            case IDNO: /* bring up the next candidate */
                break;
            case IDCANCEL: /* abort the whole process */
                END(OK) 
            }
#else
#ifdef INTERNAL
            AuxPrint("Removing library %s from cache!", (LPSTR)StripPath(szLibInfo));
#endif
            /* this will call HitCache() to update CacheUsed */
            if (ExtractFromCache(szLibInfo,TRUE) != OK)   /* second param is DELETE flag */
                END(FAIL)
#endif
        }

            /* now delete entry from LRU table */
            if (SearchLRUTree(szLibInfo,&hLRU,TRUE) != OK)
            {
                IssueErr();
                GlobalUnlock(hLRU) ;
                if (RemakeLRU() != OK)
                    END(FAIL)
            }
            saveroot = ((LPIBSTNODE) GlobalLock(hLRU))  + 1 ; /* lock the LRU table  */
        }

    end:

    GlobalUnlock(hLRU) ; /* unlock the table and split */
    SetCursor(hCursor) ; /* set cursor to normal op */

    if (nRetval != OK)
        return nRetval;

    return (QueryCacheLimit(lBytesNeeded));
}

/*************************************************************\
 ** RoutineName  QueryCacheLimit
 **
 ** ARGUMENTS:  
 **             
 **
 ** DESCRIPTION: compares cache value set by user against current amount used
 **  If dwAmtRequested == -1L then just return whether all
 **  libs are currently fitting into the cache.
 **              
 **
 ** ASSUMES: wAmtRequested is the size of the library to be downloaded
 **
 ** MODIFIES: john's alpha-tangential gravitational constant (s,m,l,xl)
 **
 ** RETURNS: OK if successful, FAIL if the new library would exceed cache limit
 **
 ** HISTORY:   John Ano  May 25, 1990 (written)
 **  (8.9.90) D. Kent added -1L feature.
\*************************************************************/

int FAR PASCAL 
QueryCacheLimit(DWORD dwAmtRequested)
{ 

    /* at some point we want to let the user in on this:
     1) give them a chance to increase the size of the cache and try again
     2) let them decide:
        a) to zap the LRU lib (and have a preselected default to automate this process)
        b) to shut down Window Shopper (and do their own file management.)
        c) to copy the LRU lib to an alternate drive */

#ifdef INTERNAL
    AuxPrint("Cache limit: %ld, used: %ld, requested: %ld",
        dwCacheSize,FileHead.CacheUsed,dwAmtRequested);
#endif

    if (dwAmtRequested == -1L)
        dwAmtRequested = 0;

    return (FileHead.CacheUsed+dwAmtRequested <= dwCacheSize) ? OK : FAIL;
}




/*************************************************************\
 ** RoutineName FillLRUFromExePath()
 **
 ** ARGUMENTS: handle to the global block containing LRU
 **
 ** DESCRIPTION: fills the LRU table with all existing libinfo files
 **  from the executable path.
 **
 ** ASSUMES:
 **
 ** MODIFIES: global handle if block moves.
 **  custinfo.cachesize
 **  win.ini
 **
 ** RETURNS:
 **
 ** HISTORY:   Dug  June 26, 1990 (written)
 **           cloned and chopped from UpdateLocalDatabase()
 **  (8.9.90) D. Kent - modified
 **   8/* 5/* 91 - eliminated 001*.* files from loop - beng
\*************************************************************/

extern WORD CacheOpts[14];

int PASCAL
FillLRUFromExePath(GLOBALHANDLE *hGlob)
{
    struct find_t c_file;
    int           whichtype = 0,
                  nRetval   = OK,
                  ii = 0;
    long hdspace = AvailDiskSpace(-1) / 2;
    static char  filespec[_MAX_PATH] = "00004*.*";       // All LIBINFO
    DWORD LibSize;

    hCursor = SetCursor(LoadCursor (NULL, IDC_WAIT));

    AddFullPath(filespec);
    if (!_dos_findfirst(filespec, _A_NORMAL, &c_file))
    {
        do // loop over libinfo files
        {
            /* add to Cache  */
            if (LibSize = SumLibSize(GetFileNum(c_file.name)))
                FileHead.CacheUsed += LibSize;
            else
                continue; // don't add if nothing in it

            /* add to LRU  */
            if (SearchLRUTree(c_file.name, hGlob, FALSE) != OK)
            {
                IssueErr();
                END(FAIL)
            }

            if (FileHead.CacheUsed > GetCacheSize(ii))
                if (ii == 13)
                    ; // argh, do something!!!
                else
                    ii++;
        } while (!_dos_findnext(&c_file));
    }

    /* get ii which gobbles next .5 of available disk space */
    while (((hdspace -= (GetCacheSize(ii+1) - GetCacheSize(ii))) > 0L) AND
            (ii < 13))
    {
#ifdef INTERNAL
        //AuxPrint("disk avail: %ld",hdspace);
#endif
        ii++;
    }
#ifdef INTERNAL
    //AuxPrint("disk avail: %ld",hdspace);
#endif

    /* write the cache setting to WIN.INI */
    custinfo.cacheSize = ii;
    WriteCustInfo(WRITE_CUSTCACHE);

    end:

    if ( hCursor )
        SetCursor(hCursor);

    return nRetval;
}



/*************************************************************\
 ** RoutineName CreateLRUFile
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION: creates a new LRU with all libinfo files found in
 **  the executable's directory.
 **
 ** ASSUMES:
 **
 ** MODIFIES: may return a different handle to global memory if table gets
 ** big enough.
 **
 ** RETURNS:
 **
 ** HISTORY:   John Ano  May 31, 1990 (written)
 **            John      Jun 28, 1990 modified
 **            dug       (9.3.90) D. Kent (rewrote)
 **            BKing     Aug 21, 1990 added BusyBox message
\*************************************************************/

int FAR PASCAL 
CreateLRUFile(void)
{
    char szLRUPath[_MAX_PATH] ;
    HANDLE fhLRU=NULL;
    int nRetval = OK;

    BusyLoadNewText(NULL, IDS_MSG_INIT_CACHE, 0);

    lstrcpy(szLRUPath,szLRUFileName) ;
    AddFullPath(szLRUPath);
    _fmode = O_BINARY;
    /* note: if we want the LRU file to be hidden, etc., change param 2 */
    if ((fhLRU = _lcreat (szLRUPath,0)) == FAIL)  
        END(FAIL) /* return FAIL if could not create */

    /* write header */
    FileHead.NumEntries = 1; /* 1 for head */
    FileHead.CacheUsed = 0;
    FileHead.smallest = 1; /* ? */
    FileHead.nextLRU = 0;  /* ? */

    if ((hLRU = GrowLRUTable(NULL)) == NULL)
        END(FAIL)

    /* initialize the head (8.13.90) D. Kent */
    head = (LPIBSTNODE) GlobalLock(hLRU) ; /* lock the Table and point to the top of the table */
    head->lLibInfoFile = 0;
    head->l = head->r = 0;
    GlobalUnlock(hLRU); /* unlock the table */

    if (FillLRUFromExePath(&hLRU) != OK)
        END(FAIL)
    
    end:
    if (fhLRU)
        _lclose (fhLRU) ;
    _fmode = O_TEXT;

    BusyClear(TRUE);

    return nRetval ;

}

/*************************************************************\
 ** RoutineName CloseLRUTable
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **
 ** HISTORY:   John Ano  June 1, 1990 (written)
 **  (9.3.90) D. Kent -- added check for NULL hGlob
\*************************************************************/

int FAR PASCAL 
CloseLRUTable (GLOBALHANDLE hGlob)
{
int nRetval = OK;

    if (hGlob == NULL)
        return OK;

    head = (LPIBSTNODE) GlobalLock(hGlob);
    if (WriteLRUToDisk(head)==FAIL)
    {
        SetErr(IDS_FILE_IO_ERR,ID_ERR_UPDATING_FILE,(LPSTR)szLRUFileName);
        nRetval = FAIL;
    }
    GlobalUnlock(hGlob);

    return (nRetval) ;
}


static void PASCAL
DeleteNode(DWORD lLibInfoFile, WORD FAR *wChildPtr)
{
    LPIBSTNODE p;
    p = head + *wChildPtr;

    if (stackavail() <= 512) // 512 is somewhat arbitrary
    {
#ifdef INTERNAL
        AuxPrint("%d bytes left on stack", stackavail());
#endif
        return;
    }

    if (*wChildPtr)
    {
        if ((long)lLibInfoFile < p->lLibInfoFile)
            DeleteNode(lLibInfoFile,&(p->l));
        else if((long)lLibInfoFile > p->lLibInfoFile)
            DeleteNode(lLibInfoFile,&(p->r));
        else if ((p->l == 0) AND (p->r == 0))
            *wChildPtr = 0;
        else if (p->l == 0)
            *wChildPtr = p->r;
        else if (p->r == 0)
            *wChildPtr = p->l;
        else
            p->lLibInfoFile = DeleteMin(&(p->r));
    }
}

static DWORD PASCAL
DeleteMin(WORD FAR *wChildPtr) // side effect is to change wChildPtr
{
    LPIBSTNODE p;
    p = head + *wChildPtr;
    if (p->l == NULL)
    {
        *wChildPtr = p->r;
        return p->lLibInfoFile;
    }
    else
        return DeleteMin(&(p->l));
}

/*************************************************************\
 ** TouchLib
 **
 ** ARGUMENTS:
 **   WORD wNodePtr
 **   BOOL bDelete
 **
 ** DESCRIPTION:
 **  If bDelete then remove wNodePtr from LRU.  Otherwise move
 **  it to the end of the array signifying that it is the most
 **  recently-accessed item in the LRU.
 **  
 **  wNodePtr is a zero-based offset into the LRU array.
 **
 ** ASSUMES:
 **  Head is first element in LRU array.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  September 3, 1990 (written)
\*************************************************************/
static void PASCAL
TouchLib(WORD wNodePtr, BOOL bDelete)
{
    IBSTNODE savenode;
    LPIBSTNODE lpNodePtr = head + wNodePtr;
    int i;

#ifdef INTERNAL
    AuxPrint("TouchLib (%s)", bDelete ? "deleting" : "touching");
#endif

    if (NOT bDelete)
        savenode = *lpNodePtr;

    /* do a memcpy, moving all after wNodePtr up one */
    lmemcpy(lpNodePtr,lpNodePtr+1, sizeof(IBSTNODE) * (FileHead.NumEntries - wNodePtr - 1));

    if (NOT bDelete)
        *(head + FileHead.NumEntries - 1) = savenode;

    /* now update pointers */
    for (lpNodePtr = head, i=FileHead.NumEntries; i--; ++lpNodePtr)
    {
        if (lpNodePtr->r == wNodePtr)
            lpNodePtr->r = FileHead.NumEntries-1;
        else if (lpNodePtr->r > wNodePtr)
            --lpNodePtr->r;
        if (lpNodePtr->l == wNodePtr)
            lpNodePtr->l = FileHead.NumEntries-1;
        else if (lpNodePtr->l > wNodePtr)
            --lpNodePtr->l;
    }
}

int
RemakeLRU()
/* assume hLRU is unlocked.   Issues error message */
{
    char szLRU[_MAX_PATH];

    lstrcpy(szLRU,szLRUFileName);
    AddFullPath(szLRU);
    BusyLoadNewText(NULL,IDS_MSG_INIT_CACHE, 0);
    unlink(szLRU);
    if (hLRU)
        GlobalFree(hLRU) ;
    hLRU = NULL;
    if (CreateLRUFile()==FAIL)
    {
        ErrMsg(IDS_FILE_IO_ERR,ID_ERR_MISSING_LRU,szLRU);
        return FAIL;
    }
    BusyClear(TRUE);
    return OK;
}
