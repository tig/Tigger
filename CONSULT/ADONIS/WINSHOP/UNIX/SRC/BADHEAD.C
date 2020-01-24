#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <memory.h>
#include <fcntl.h>
#include "stdlib.h"
#include "stdinc.h"
#include "stdcomm.h"

BOOL IsRightFormat(char *szFileName);
char* strupcase(char *sIn);
void main(void);

#define	DATADIR	"/usr/bbs/master.bin"

void
main(void)
{
    struct directx
    {
        ino_t d_ino;
        char d_name[DIRSIZ+1];
    } dlink;

    int fd, nread;
	char	szDestName[STD_FILENAME_SIZE + 1];
	char	szPath[120];
	HANDLE	CurFilePtr = -1;

	if ((fd = open(DATADIR, 0)) == -1)
	{
		printf("Error opening %s\n", DATADIR);
		exit(FAIL);
	}
    
    while ((nread = read(fd, &dlink, sizeof(struct direct))) == 
                               (sizeof(struct direct)))
    {
        if (dlink.d_ino)
        {
            /* is name in proper format for inclusion? */
            if (IsRightFormat(dlink.d_name))
			{
				memset(szDestName, 0, STD_FILENAME_SIZE);
				sprintf(szPath, "%s/%s", DATADIR, dlink.d_name);
				CurFilePtr = open(szPath, O_RDONLY);
                lseek(CurFilePtr, 3L, SEEK_SET);
                read(CurFilePtr, szDestName, STD_FILENAME_SIZE);
				szDestName[STD_FILENAME_SIZE] = '\0';
				if (strcmp(strupcase(szDestName), strupcase(dlink.d_name)) != 0)
					printf("%s is %s\n", dlink.d_name, szDestName);
				close(CurFilePtr);
			}
        }
        dlink.d_name[DIRSIZ] = '\0';
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

char *
strupcase(char *sIn) 
{
   char *szTemp = sIn;
   while (*szTemp) {
      if ((*szTemp >= 97) AND (*szTemp <= 122)) {
         *szTemp -= 32;
      }
      ++szTemp;
   }
   return sIn;
}

