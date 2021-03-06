#define STORE_SIZE


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <memory.h>
#include <fcntl.h>
#include "stdlib.h"
#include "stdinc.h"
#include "database.h"
#include "stdcomm.h"

typedef struct
{
    DWORD base;
    WORD ext;
#ifdef STORE_SIZE
	WORD size;	//in number of blocks
#endif
} FILENAME;

static DWORD GetFileNum(char *szFileName);
static WORD  GetExtNum(char *szFileName);
BOOL IsDataFile(char *szFileName);
static int AddFileToTree(void);
static void WriteTree(void);
static WORD newLRC(char* sIn);
void main(void);
#ifdef STORE_SIZE
int GetFileSize(char* szFile);
#endif

/*************************************************************\
 ** MakeFileTree
 **
 ** ARGUMENTS:
 **  char *szDirName
 **
 ** DESCRIPTION:
 **  Add all the files in szDirName to binary search tree.  Return
 **  pointer to tree.  Don't include files that don't have an eight-digit
 **  numerical basename.
 **
 ** ASSUMES:
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  Pointer to tree or NULL if failed.
 **
 ** HISTORY:   Doug Kent  October 19, 1990 (written)
\*************************************************************/

static FILENAME *TreeTop=NULL;
static int TreeSize;
FILENAME CurFileNode;

void
main(void)
{
    struct directx
    {
        ino_t d_ino;
        char d_name[DIRSIZ+1];
    } dlink;

    int fd, nread;
    int nRetval=OK;
	int count;

	if ((fd = open("/usr/bbs/master.bin", 0)) == -1)
	{
		printf("Error opening /usr/bbs/master.bin\n");
		exit(FAIL);
	}
    
    /* allocate the top of the tree, a dummy root */
    if ((TreeTop = (FILENAME *)malloc(sizeof(FILENAME))) == NULL)
    {
        printf("Unable to make directory tree (insufficient memory)\n");
        exit(FAIL);
    }

    memset(TreeTop, 0, sizeof(FILENAME));
    TreeSize = 1;

    dlink.d_name[DIRSIZ] = '\0';
	count = 0;
    while ((nread = read(fd, &dlink, sizeof(struct direct))) == 
                               (sizeof(struct direct)))
    {
		++count;
        if (dlink.d_ino)
        {
            /* is name in proper format for inclusion? */
            if (IsDataFile(dlink.d_name))
			{
#ifdef STORE_SIZE
				GetFileSize(dlink.d_name);
#endif
                /* it is so add to tree */
				if (AddFileToTree() == FAIL)
                    exit(FAIL);
			}
        }
        dlink.d_name[DIRSIZ] = '\0';
    }

	printf("Files read from directory = %d\n", count);
	printf("TreeSize = %d\n", TreeSize - 1);
    close(fd);
	WriteTree();
	printf("Size of FILENAME = %d\n", sizeof (FILENAME));
	if (TreeTop)
		free(TreeTop);
	TreeTop = NULL;
}


int
AddFileToTree(void)
{
    int nRetval=OK;
    int start, end, x;

	if ((TreeTop = (FILENAME *)realloc(TreeTop,
				((TreeSize+1) * sizeof(FILENAME)))) == NULL)
	{
		printf("Unable to make directory tree (insufficient memory)\n");
		return FAIL;
	}

    /* enough, now add the sucker */
    start = 0;              // first item in list
    end = TreeSize-1;  // last  item in list

    //AuxPrint("(BEFORE) end: (%x)=>0x%lx, start: (%x)=>0x%lx, adding: 0x%lx",
        //end,pUptodate[end],start,pUptodate[start],lFileNum);

    while ( end >= start )
    {
        x = (start + end) >> 1;
        if ( CurFileNode.base < TreeTop[x].base)
            end = x - 1;
        else 
		{
			if (CurFileNode.base > TreeTop[x].base)
            	start = x + 1;
			else 
			{
				printf("Duplicate base names found in database: %08lX\n", 
						CurFileNode.base);
				return OK;    // keep going to find any other errors
			}
		}
    }

    /* now insert after end, before start */
    //AuxPrint("(AFTER)  end: (%x)=>0x%lx, start: (%x)=>0x%lx, adding: 0x%lx",
        //end,pUptodate[end],start,pUptodate[start],lFileNum);
	#if 1
	/* can't link memmove, so... (dammit) */
	{
		FILENAME *pprev = TreeTop + TreeSize - 1,
				 *pnext = pprev + 1;
		int count = TreeSize - end;
		while(count--)
			*pnext-- = *pprev--;
	}
	#else
    memmove(TreeTop + start,TreeTop + end,sizeof(FILENAME) * (TreeSize - end));
	#endif

    TreeTop[start].base = CurFileNode.base;
    TreeTop[start].ext = CurFileNode.ext;
#ifdef STORE_SIZE
    TreeTop[start].size = CurFileNode.size;
#endif
    ++TreeSize;

	return OK;
}

BOOL
IsDataFile(char *szFileName)
/* this routine isn't perfect in weeding out bad formats, but its good enuf */
{
	int count;
	char* p;

	p = szFileName + (count = strlen(szFileName)) - 1;

	if (count != 12)
		return FALSE;

    while (p >= szFileName)
        if (isxdigit(*p) OR (*p == '.')) 
			--p;
		else 
		{
			printf("Bad Format: %s\n", szFileName);
			return FALSE;
		}

	CurFileNode.base = GetFileNum(szFileName);
	CurFileNode.ext = GetExtNum(szFileName);

	if ((CurFileNode.base == 0L) OR 
		bIS_LRU_FILE(CurFileNode.base) OR	
		bIS_THUMBNAIL_FILE(CurFileNode.base) OR
		bIS_THUMBNAIL_INFO_FILE(CurFileNode.base)) 
		return FALSE;
    return TRUE;
}

DWORD
GetFileNum(char *szFileName)
/** convert file name to it's basename DWORD value **/
{
    char tmp[13],*p;
    strcpy(tmp,szFileName);
    if (p = strchr(tmp,'.'))
        *p = '\0';
    return (strtol(tmp,NULL,16));
}

WORD
GetExtNum(char *szFileName)
/** convert extension to it's WORD value **/
{
    char* p;
    if (p = strchr(szFileName, '.'))
		return (strtol((char*)(p + 1), NULL, 16));
}

void
WriteTree(void)
{
	int ListFile, count;
	int numread, x;
	DWORD tmpSize;
	DWORD dwDataFileSum = 0;
	char szFileName[13];

#if 0
	BYTE bHi;
	BYTE bLo;
	WORD wHi;
	WORD wLo;
#endif	
	if ((ListFile = open("/usr/bbs/log/00000003.001", 
							O_WRONLY | O_APPEND | O_TRUNC | O_CREAT, 
							432)) == -1)
	{
		printf("Unable to create list file\n");
		exit(FAIL);
	}
	
	tmpSize = (DWORD)TreeSize - 1;
	if (write (ListFile, (char*)&tmpSize, sizeof(DWORD)) == -1)
	{
		printf("Unable to write to list file\n");
		exit(FAIL);
	}

	for (x = 1; x < TreeSize; ++x)
	{
#if 0
		// swap bytes in each word for 68000 cpu's
#ifdef STORE_SIZE
		bHi = HIBYTE(TreeTop[x].size);	
		bLo = LOBYTE(TreeTop[x].size);	
		TreeTop[x].size = MAKESHORT(bHi, bLo);  // this swaps the bytes
#endif

		bHi = HIBYTE(TreeTop[x].ext);	
		bLo = LOBYTE(TreeTop[x].ext);	
		TreeTop[x].ext = MAKESHORT(bHi, bLo);  // this swaps the bytes

		bHi = HIBYTE(LOWORD(TreeTop[x].base));	
		bLo = LOBYTE(LOWORD(TreeTop[x].base));	
		wLo = MAKESHORT(bHi, bLo);  // this swaps the bytes

		bHi = HIBYTE(HIWORD(TreeTop[x].base));	
		bLo = LOBYTE(HIWORD(TreeTop[x].base));	
		wHi = MAKESHORT(bHi, bLo);  // this swaps the bytes

		TreeTop[x].base = MAKELONG(wHi, wLo);  // this swaps the words 
#endif
		if (write (ListFile, (char*)&TreeTop[x], 
					sizeof(FILENAME)) == -1)
		{
			printf("Unable to write to list file\n");
			exit(FAIL);
		}
	}
#if 0
	if (write (ListFile, (char*)TreeTop + 1, 
				TreeSize * sizeof(FILENAME)) == -1)
	{
		printf("Unable to write to list file\n");
		exit(FAIL);
	}
#endif
	close(ListFile);

	if ((ListFile = open("/usr/bbs/log/00000003.001", 0)) == -1)
	{
		printf("Unable to open list file for reading\n");
		exit(FAIL);
	}

	// read header DWORD containing number of FILENAME's to follow
	if (numread = read(ListFile, &tmpSize, sizeof(DWORD)) != sizeof(DWORD))
	{
		printf("Unable to read header from list file\n");
		exit(FAIL);
	}
	printf("Header in Listfile = %ld\n", tmpSize);
	
	count = 0;
	while (numread = read(ListFile, &CurFileNode, sizeof(FILENAME)))
	{
		++count;
#if 0
		// words swapped in DWORD for DOS
		bHi = HIBYTE(CurFileNode.ext);	
		bLo = LOBYTE(CurFileNode.ext);	
		CurFileNode.ext = MAKESHORT(bHi, bLo);  // this swaps the bytes

#ifdef STORE_SIZE
		bHi = HIBYTE(CurFileNode.size);	
		bLo = LOBYTE(CurFileNode.size);	
		CurFileNode.size = MAKESHORT(bHi, bLo);  // this swaps the bytes
#endif
		bHi = HIBYTE(LOWORD(CurFileNode.base));	
		bLo = LOBYTE(LOWORD(CurFileNode.base));	
		wLo = MAKESHORT(bHi, bLo);  // this swaps the bytes

		bHi = HIBYTE(HIWORD(CurFileNode.base));	
		bLo = LOBYTE(HIWORD(CurFileNode.base));	
		wHi = MAKESHORT(bHi, bLo);  // this swaps the bytes

		CurFileNode.base = MAKELONG(wHi, wLo);  // this swaps the words 
#endif
		if (numread != sizeof(FILENAME))
		{
			printf("Incomplete FILENAME struct read in list file\n");
			exit(FAIL);
		}
		else
		{
			sprintf(szFileName,"%08lX.%03X",CurFileNode.base,CurFileNode.ext); 
			printf("%s", szFileName);
		}

#ifdef STORE_SIZE
			printf(" = %5d blocks\n", CurFileNode.size);
#else
			printf("\n");
#endif
		// Calculate CRC for file name and add to checksum
		dwDataFileSum += (DWORD) newLRC(szFileName);
	}
	close(ListFile);
	printf("%d files read from FILELIST_FILE\n", count);
	printf("Database checksum is %ld\n", dwDataFileSum);
}

#ifdef STORE_SIZE
int
GetFileSize(char* szFile)
{
	HANDLE CurFilePtr = -1;
	int nRetval = OK;
	
	char szFilePath[120];
	strcpy(szFilePath, "/usr/bbs/master.bin/");
	strcat(szFilePath, szFile);
	if ((CurFilePtr = open(szFilePath,O_RDONLY)) == -1)
	{
		printf("Unable to open %s\n",szFile);
		nRetval = FAIL;
	}
	lseek(CurFilePtr,3L + TOTALBLOCKNUM_INDEX,SEEK_SET);
	if (read(CurFilePtr,&CurFileNode.size,
				sizeof(CurFileNode.size)) < sizeof(CurFileNode.size))
	{
		nRetval = FAIL;
		printf("Unable to obtain file size for %s\n", szFile);
	}
	if (CurFilePtr != -1)
		close(CurFilePtr);
	return nRetval;
}
#endif


/*************
	COPIED FROM dirtree.c, do not alter independently!!!
**************/
/*
   This function calculates the LRC value for a given string.  The function
   has the side effect of setting the eighth (parity) bit of each character
   in sIn to zero.  For this implemnetation we will skip leading zeros and
   periods in filenames ( if the filename has more than one significant digit).
*/

WORD newLRC(char *sIn)
{
    WORD temp = 0;

	// skip leading zeros
	while (*sIn == '0')
		++sIn;

   //get started, we assume sIn is still longer than 2 chars
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








