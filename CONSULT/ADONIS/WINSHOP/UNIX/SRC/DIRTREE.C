#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include "dirtree.h"
#include <memory.h>
#include "stdlib.h"
#include "database.h"
#include "util.h"

static FILENODE * GetFileNodeFromTree(FILENODE *DirTree,DWORD lNewFile);
static DWORD GetFileNum(char *szFileName);
static char * GetFileExt(char *szFileName);
BOOL IsRightFormat(char *szFileName);
static int AddFileToTree(DWORD lNewFile, char *Ext);
WORD newLRC(char *sIn);

extern WORD wDataFileCount;	// tracks number of database files

extern DWORD dwDataFileSum; // LRC sum of all database filenames to compare 
							//against local database

/*************************************************************\
 ** MakeFileTree
 **
 ** ARGUMENTS:
 **  char *szDirName
 **
 ** DESCRIPTION:
 **  All all the files in szDirName to binary search tree.  Return
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
static FILENODE *TreeTop=NULL;
static int TreeSize;

FILENODE *
MakeFileTree(char *szDirName)
{
    char *p;

    struct directx
    {
        ino_t d_ino;
        char d_name[DIRSIZ+1];
    } dlink;
    int fd, nread;
    char *dname;
    int nRetval=OK;
	long lFileNum;

	if ((fd = open(szDirName,0)) == -1)
	{
		printf("Error opening %s\n",szDirName);
		return NULL;
	}
    
    /* allocate the top of the tree, a dummy root */
    if ((TreeTop = (FILENODE *)malloc(sizeof(FILENODE))) == NULL)
    {
        printf("Unable to make directory tree (insufficient memory)\n");
        END(FAIL)
    }

    memset(TreeTop,0,sizeof(FILENODE));
    TreeSize = 1;

	wDataFileCount = 0;
    dlink.d_name[DIRSIZ] = '\0';
    while ((nread = read(fd, &dlink, sizeof(struct direct))) == 
                               (sizeof(struct direct)))
    {
        if (dlink.d_ino)
        {
            /* is name in proper format for inclusion? */
            if (IsRightFormat(dlink.d_name))
			{
				lFileNum = GetFileNum(dlink.d_name);
                /* it is so add to tree */
				if (AddFileToTree(lFileNum, GetFileExt(dlink.d_name)) == FAIL)
                    END(FAIL)

				//add LRC vaue of filename to global
				if (NOT (bIS_THUMBNAIL_FILE(lFileNum) OR 
						 bIS_THUMBNAIL_INFO_FILE(lFileNum) OR
						 bIS_LRU_FILE(lFileNum)))
				{
					dwDataFileSum += (DWORD) newLRC(dlink.d_name);
					++wDataFileCount;
				}
			}
        }
        dlink.d_name[DIRSIZ] = '\0';
    }

    end:

    if (nRetval != OK)
    {
        if (TreeTop)
            free(TreeTop);
        TreeTop = NULL;
    }

    close(fd);
    return TreeTop;
}


int
AddFileToTree(DWORD lNewFile, char *Ext)
{
    int nRetval=OK;
    int start, end, x;

#if 0
	WORD wExt;
	WORD wNum;
#endif

	if ((TreeTop = (FILENODE *)realloc(TreeTop,
				((TreeSize+1) * sizeof(FILENODE)))) == NULL)
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
        if ( lNewFile < TreeTop[x].basename )
            end = x - 1;
        else
            start = x + 1;
    }

    /* now insert after end, before start */
    //AuxPrint("(AFTER)  end: (%x)=>0x%lx, start: (%x)=>0x%lx, adding: 0x%lx",
        //end,pUptodate[end],start,pUptodate[start],lFileNum);
	#if 1
	/* can't link memmove, so... (dammit) */
	{
		FILENODE *pprev = TreeTop + TreeSize - 1,
				 *pnext = pprev + 1;
		int count = TreeSize - end;
		while(count--)
			*pnext-- = *pprev--;
	}
	#else
    memmove(TreeTop + start,TreeTop + end,sizeof(FILENODE) * (TreeSize - end));
	#endif

    TreeTop[start].basename = lNewFile;
    strcpy(TreeTop[start].extension,Ext);
    ++TreeSize;

	return OK;
}

BOOL
IsRightFormat(char *szFileName)
/* this routine isn't perfect in weeding out bad formats, but its good enuf */
{
	char *p = szFileName + 7;

    while (p >= szFileName)
        if (!isxdigit(*p)) return FALSE;
        else --p;

    return TRUE;
}

DWORD
GetFileNum(char *szFileName)
/** convert file name to it's basename long value **/
{
    char tmp[13],*p;
    strcpy(tmp,szFileName);
    if (p = strchr(tmp,'.'))
        *p = '\0';
    return (strtol(tmp,NULL,16));
}

char *
GetFileExt(char *szFileName)
{
    return (strchr(szFileName,'.') + 1);
}

int
GetExtFromTree(FILENODE *DirTree, char *szFileName)
/* assume extension is removed */
{
	int nRetval=OK;
	FILENODE *FileNode;
	DWORD lFileNum = strtol(szFileName,NULL,16);

    //printf("Looking for:  %lx\n",lFileNum); 
	if (lFileNum == 0L)
		END(FAIL)

	if ((FileNode = GetFileNodeFromTree(DirTree,lFileNum)) == NULL)
		END(FAIL)
    else
	{
		strcat(szFileName,".");
		strcat(szFileName,FileNode->extension);
		END(OK)
	}
	end:
	//printf("%s  ",nRetval == OK ? "Found":"Not Found\n");
	//if (nRetval == OK)
		//printf("%s\n",szFileName);
	return nRetval;
}

FILENODE *
GetFileNodeFromTree(FILENODE *DirTree, DWORD lFileNum)
{
    int start,end,x;

    start = 0;         // first item in list
    end = TreeSize-1;  // last  item in list

    while ( end >= start )
    {
        x = (start + end) >> 1;
        if ( lFileNum < DirTree[x].basename )
            end = x - 1;
        else
            start = x + 1;
        if ( lFileNum == DirTree[x].basename )
			return (&DirTree[x]);
    }

    return NULL;
}


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




