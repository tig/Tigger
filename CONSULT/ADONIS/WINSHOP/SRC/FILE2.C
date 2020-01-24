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
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include "ws.h"
#include "misc.h"
#include "comm.h"
#include "database.h"
#include "winundoc.h"
#include "libids.h"
#include "download.h"
#include "rc_symb.h"
#include "error.h"
#include "libmgr.h"
#include "file.h"
#include "dos.h" // GetExt

char szCategoryFileName[_MAX_PATH];
char szPublishersFileName[_MAX_PATH];

DWORD FAR PASCAL 
SumLibSize(LONG lLibFile) // library info file
{
	int fp= -1;
    char szLibFile[_MAX_PATH];
	char szThumbInfoFileName[_MAX_PATH];
  	char szThumbnailFileName[_MAX_PATH];
    char *pTI,*pTR; // pointers to basenames so we needn't call AddFullPath repeatedly
    DWORD LibSize = 0L;

    LTOS(lLibFile,szLibFile) ;

    if (bNULLFILE(szLibFile))
        return 0L;

    AddFullPath(szLibFile);
    if (GetExt(szLibFile) != OK)
        if (InqErr())
        return 0L;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return 0L;

    /* \ is guaranteed to be on end */
    lstrcpy(szThumbInfoFileName,szDataPath);
    pTI = szThumbInfoFileName + strlen(szThumbInfoFileName);
    lstrcpy(szThumbnailFileName,szDataPath);
    pTR = szThumbnailFileName + strlen(szThumbnailFileName);

    /* skip to sublibs */
    if (ReadToChar(fp,'\n') == FAIL)
        return 0L;

    while (readch(fp) != '') /* do all sublibs */
    {
        if (ReadToChar(fp,'\t') == FAIL) /* read whatever this is */
            goto end;
#ifdef INTERNAL
        //AuxPrint("RS from SumLibSize(), 1");
#endif
        if (ReadStringToChar(fp, pTI, '\t', _MAX_PATH - strlen(szThumbInfoFileName))) /* get the info file name */
            goto end;
#ifdef INTERNAL
        //AuxPrint("RS from SumLibSize(), 2");
#endif
        if (ReadStringToChar(fp,pTR,'\n', _MAX_PATH - strlen(szThumbnailFileName)) == FAIL) /* get the resource file name */
            goto end;

        if (GetExt(szThumbInfoFileName) != OK)
        {
            if (InqErr())
                goto end;
            // else file don't exist, continue
        }
        else
            LibSize += FileSize(szThumbInfoFileName) ;

        if (GetExt(szThumbnailFileName) != OK)
        {/* problem with specd file, don't count in size  */
            if (InqErr())
                goto end;
            // else file don't exist, continue
        }
        else /* file is OK, so add it to the count  */
            LibSize += FileSize(szThumbnailFileName) ;
    }
    
end:
    if (fp != -1)
        close(fp);
	
   return(LibSize);
}


void FAR PASCAL
GetMasterFileName(char *szStr)
{
    char tmp[_MAX_PATH];
    lstrcpy(szStr,szDataPath);
    LTOS(MASTERLIST,tmp);
    lstrcat(szStr,tmp);
}



/*************************************************************\
 ** GetExt
 **
 ** ARGUMENTS:
 **  char *szFileName
 **
 ** DESCRIPTION:
 **  Find file on disk with same basename and append its extension to
 **  szFileName.  
 **
 ** ASSUMES:
 **  No trailing '\'.  
 **  Basename is eight chars and ext (if one) is 3 chars. 
 **  Full pathname is prepended to szFileName.
 **  
 ** MODIFIES:
 **  Sets error condition only if more than one extension.
 **
 ** RETURNS:
 **  OK if NULLFILE, or extension is successfully added.
 **  If already has extension, overwrite it.
 **  FAIL if extension is not added (basename doesn't exist) or there 
 **  are more than one extensions available to be added.
 **
 ** HISTORY:   Doug Kent  March 15, 1990 (written)
\*************************************************************/
int FAR PASCAL
GetExt(char *szFileName)
{
    char szTmp[_MAX_PATH];
    char *p;
    struct find_t c_file;

    /** don't add path to NULL file **/
    if (bNULLFILE(szFileName))
        return OK;

    p = szFileName + strlen(szFileName); // point to NULL char 
    if (p[-4] == '.') // then already has extension (assume 3 char ext)
    {
        p -= 4;
        *p = '\0';
    }

    lstrcpy(szTmp,szFileName);
    lstrcat(szTmp,".*");
    if (_dos_findfirst(szTmp,_A_NORMAL, &c_file))
    /* none with this basename */
    {
        //SetErr(IDS_ERR_LOAD_FAILED, (LPSTR)StripPath(szTmp));
        return FAIL;
    }

    if (!_dos_findnext(&c_file))
    /* more than one with this basename */
    {
        SetErr(IDS_FILE_IO_ERR,ID_ERR_TOO_MANY, (LPSTR)StripPath(szTmp));
        return FAIL;
    }

    /* point p to basename */
    p -= 8;
    lstrcpy(p,c_file.name);

    return OK;
}


LONG PASCAL
GetFileNum(char *szFileName)
/** convert file name to it's basename long value **/
{
    char tmp[13],*p;
    strcpy(tmp, StripPath(szFileName));
    if (p = strchr(tmp,'.'))
        *p = '\0';
    return (strtol(tmp,NULL,16));
}

WORD PASCAL
GetExtNum(char *szFileName)
/** convert file name extension to it's WORD value **/
{
    char tmp[13], *p;
    strcpy(tmp, StripPath(szFileName));
    if (p = strchr(tmp, '.'))
      return (WORD)(strtol(p+1, NULL, 16));
    else
      return 0;
}

