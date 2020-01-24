
#ifdef FILE_MAIN

#define INITIAL_FILE_CACHE_SIZE	4096
static	HANDLE	hCache;


#if 0
/*-------------------------------------------------*/
/*     KNOW MSG ID'S FOR WINSHOP's PRIMARY FILES   */
/*-------------------------------------------------*/
#define BBM_SEND_MASTER_VERSIONS		1
#define BBM_SEND_CATEGORY_LST			2
#define BBM_SEND_PUBLISHER_LST 		3
		
char    szCategorySignature[]="WindowShopperCategoryFile";
#define CAT_SIGNATURE_LEN sizeof(szCategorySignature)/sizeof(char)

char    szPublisherSignature[]="WindowShopperPublisherFile";
#define PUB_SIGNATURE_LEN sizeof(szCategorySignature)/sizeof(char)

typedef struct tCatHeader {
    char    cSignature[CAT_SIGNATURE_LEN];
    char    cVersion[VERSION_LEN];
} CATEGORY_HEADER;


typedef struct tPubHeader {
    char    cSignature[PUB_SIGNATURE_LEN];
    char    cVersion[VERSION_LEN];
} PUBLISHER_HEADER;
#endif
#endif

#ifdef INCL_LOCALPROCS
extern BOOL NEAR PASCAL CategoryFileOK();
extern BOOL NEAR PASCAL PublisherFileOK();
#endif

/*-------------------------------------------------------------------------*/
/*                           L O C A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/


#include "stdlib.h"
#include <order.h>
#define MAX_FILENAME_LEN	(_MAX_FNAME+_MAX_EXT-1)

/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
void CloseFileCheckSystem(int nEndState);
#define  AND_RESET 0
#define  AND_DISABLE -1

extern int FAR PASCAL UpdateLocalDataBase(void);
extern DWORD FAR PASCAL SumLibSize(LONG lLibFile) ;
extern  int pascal far InitFileSystem(void );
extern  int pascal far InitMasterLists(void );
extern  int pascal far LoadFile(char *szFileName);
extern  WORD FAR PASCAL EnumListFile(FARPROC lpFunc,HWND hWnd,char *szListFile);
extern  WORD FAR PASCAL EnumSetFile(FARPROC lpFunc,HWND hWnd,char *szSetFile);
extern  WORD FAR PASCAL EnumSubLibs(FARPROC lpFunc,HWND hWnd,char *szLibFile);
extern  WORD FAR PASCAL EnumMedia(FARPROC lpFunc,HWND hWnd,char *szLibFile);
extern WORD FAR PASCAL EnumLibVersions(FARPROC lpFunc, HWND hWnd, char *szLibFile) ;
extern WORD FAR PASCAL EnumClipVersions(int nIndex, FARPROC lpFunc, HWND hWnd, char *szThumbInfoFile);
extern WORD FAR PASCAL EnumClipDesc(FARPROC lpFunc, HWND hWnd, char *szThumbInfoFile) ;
extern int FAR PASCAL QueryListFileName(int nIndex, char *szMasterFile, char *szListFileName);
extern int FAR PASCAL 
QueryPubInfoFileName(int nIndex, char *szListFile, char *szPubInfoFileName);
extern  int FAR PASCAL QuerySetFileName(int nIndex,char *szListFile,char *szSetFileName);
extern  int FAR PASCAL QueryLibFileName(int nIndex,char *szSetFileName,char *szLibFileName);
extern int FAR PASCAL QueryLibName(int nIndex, char *szSetFileName, char *szLibName);
extern  int FAR PASCAL QueryThumbnailFileName(int nIndex,char *szLibFile,char *szThumbnail);
extern int FAR PASCAL QueryThumbInfoFileName(int nIndex, char *szLibFile, char *szThumbInfo);
extern int FAR PASCAL QueryCopyright(char *szPubFileName, HANDLE hEdit);
extern  int FAR PASCAL QueryLibInfo(char *szLibFile,
    char *szLibName, 
    char *szPublisherFile, 
    char *szPublisher,
    BOOL *bDownloadable,
    HANDLE hDesc,
    char *szDesc,
    char *szNumClips);
extern  int FAR PASCAL QueryMediaPrices(int nIndex,char *szLibFile,char *szSRPrice,char *szWSPrice);
extern int FAR PASCAL QueryMediaType(int nIndex, char *szLibFile, MEDIATYPE *MType);
extern int FAR PASCAL QueryClipDesc(int nIndex, char *szThumbInfoFile, char *szClipDesc);
extern int FAR PASCAL 
QueryClipVerInfo(int nClipIndex, int nVerIndex, char *szThumbInfoFile, 
        char *szVersion,
        char *szPrice,
        char *szClipFileSize,
        char *szClipFileName);
extern  void FAR PASCAL GetMasterFileName(char *szStr);
extern WORD FAR PASCAL EnumMasterFile(FARPROC lpFunc, HWND hWnd, char *szMasterFile);
extern BOOL FAR PASCAL
QueryIndirection (char *szFileName, char *szStr);
extern int FAR PASCAL GetExt(char *szFileName);
int FAR PASCAL ReadStringToChar(int fHandle, char *str, char chartoget, int nSize);
int FAR PASCAL ReadToChar(int fHandle, char chartoget);
unsigned char FAR PASCAL readch(int fHandle);
int FAR PASCAL writech(unsigned char c, int fHandle);
int FAR PASCAL WriteStr(int fHandle, char *str, char termchar);
LONG PASCAL GetFileNum(char *szFileName);
extern WORD FAR PASCAL GetExtNum(char *szFileName);
