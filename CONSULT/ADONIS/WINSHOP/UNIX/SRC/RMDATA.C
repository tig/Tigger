#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <memory.h>
#include <fcntl.h>
#include "stdlib.h"
#include "stdinc.h"

BOOL IsRightFormat(char *szFileName);
void main(void);

void
main(void)
{
    struct directx
    {
        ino_t d_ino;
        char d_name[DIRSIZ+1];
    } dlink;
    int fd, nread;
	char szInput[80];

	while (TRUE)
	{
		printf("\nDelete all database files from this directory? [y/n]: ");
		gets(szInput);
		switch (szInput[0])
		{
			case 'Y':
			case 'y':
				goto CONTINUE;
				break;
			case 'N':
			case 'n':
				exit(1);
				break;
			default:
				break;
		}
	}
	CONTINUE:
	printf("\nDeleting database files...\n");

	if ((fd = open(".", 0)) == -1)
	{
		printf("Error opening directory\n");
		exit(FAIL);
	}
    
    while ((nread = read(fd, &dlink, sizeof(struct direct))) == 
                               (sizeof(struct direct)))
    {
        if (dlink.d_ino AND IsRightFormat(dlink.d_name))
			unlink(dlink.d_name);
    }
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
