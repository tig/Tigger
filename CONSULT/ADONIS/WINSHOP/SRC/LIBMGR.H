#ifndef LIBMGRH
#define LIBMGRH

#define STD_GLOB_INCR 10 /* when reallocating space, do it by 10 entries at a time  */
#define MAX_LRU_VAL  65536  /* largest ordinal value allowed in LRU table  */

typedef struct IBST {
    WORD l, r;
    WORD wLRU; // not used
    LONG lLibInfoFile;
}  IBSTNODE;

typedef IBSTNODE FAR *LPIBSTNODE;

// total size is 10 bytes

typedef struct fhead { /* rename sm & lrg to oldest and newest  */
     WORD NumEntries; // includes head
    DWORD CacheUsed;  // diskspace by libs in cache
     WORD smallest,   // ? not used
          nextLRU ;   // not necessary (use NumEntries instead)
} LRUHEADER;

/*                           global data                             */

extern GLOBALHANDLE hLRU ;                      /* handle to the LRU table  */
extern LRUHEADER FileHead ; 
extern char szLRUFileName[13] ;
extern DWORD dwCacheSize ;


/*                              functions                          */

int RemakeLRU(void);
GLOBALHANDLE InitFileCache(LONG lreq_size);
GLOBALHANDLE FAR PASCAL GrowLRUTable(GLOBALHANDLE hGlob) ;
int FAR PASCAL WriteLRUToDisk(LPIBSTNODE head) ;
int FAR PASCAL ReadLRUFromDisk(GLOBALHANDLE *hGlobTable) ;
WORD FAR PASCAL AddEntryToLRU(WORD wChildPtr, LONG lNewFile);
/* new name for SearchLRUTree===> AddToLRU  */
int FAR PASCAL SearchLRUTree(PSTR szLibFile, GLOBALHANDLE *hGlobLRUTable, BOOL bDelete) ;
int FAR PASCAL MakeCacheSpace(long lBytesNeeded);
int FAR PASCAL CreateLRUFile (void) ;
int FAR PASCAL QueryCacheLimit(DWORD dwAmtRequested) ;
int FAR PASCAL CloseLRUTable (GLOBALHANDLE hGlob) ;
int FAR PASCAL LoadCache(int NumEntries);
int FAR PASCAL FillLRUFromExePath(GLOBALHANDLE *hGlob);
int FAR PASCAL HitCache(char *szFileName, BOOL bDelete);
int RetrieveLibFromDrive(char *szFileName);
int RetrieveFile(char *szDest,char *szSrcPath);
extern int FAR PASCAL 
ConfirmCopyOK(char *szSrc, char *szDest, DWORD dwCompress, BOOL bCheckCache);
extern LONG FAR PASCAL WSLibDriveDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam);
int FAR SetBackupDrive(void);

#endif
