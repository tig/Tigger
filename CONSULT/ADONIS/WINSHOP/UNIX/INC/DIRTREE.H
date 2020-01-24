#include "stdinc.h"
typedef struct
{
    long basename;
    char extension[4];
} FILENODE;

extern FILENODE * MakeFileTree(char *szDirName);
extern int GetExtFromTree(FILENODE *DirTree, char *szFileName);
extern WORD newLRC(char* sIn);

