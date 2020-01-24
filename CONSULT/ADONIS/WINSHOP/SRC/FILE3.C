#include <nowin.h>
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


#if 0
/*************************************************************\
 ** EnumMasterFile
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate headers of files referenced in Master file.
 **
 ** ASSUMES:
 **  Name of master file is given in szMasterFile
 **  szMasterFile has full path.
 **  szMasterFile has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  Number of items enumerated.
 **
 ** HISTORY:   Doug Kent  February 20, 1990 (renamed)
\*************************************************************/
WORD FAR PASCAL 
EnumMasterFile(FARPROC lpFunc, HWND hWnd, char *szMasterFile)
{
	int fp= -1;
	WORD nRetval=NULL;
	char szDesc[MAX_STR_LEN];
	WORD count= 0;

    /* open file */
    if ((fp = _lopen(szMasterFile,READ)) == -1) 
        return 0;

    /* skip header */
    //if (ReadToChar(fp,'\n') == FAIL)
        //return 0;

    while (!eof(fp))
    {
        if (ReadStringToChar(fp,szDesc,'\t', sizeof(szDesc)) == FAIL)
            goto end;
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;

        if (((*lpFunc)(szDesc,hWnd)) == FALSE)
            goto end;

        ++count;
    }
    
end:
    if (fp != -1)
        _lclose(fp);
	
   return(count);
}
#endif

/*************************************************************\
 ** EnumListFile
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate names in list file.
 **
 ** ASSUMES:
 **  Name of list file is given in szListFile
 **  szListFile has full path.
 **  szListFile has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  Number of items enumerated.
 **
 ** HISTORY:   Doug Kent  February 20, 1990 (renamed)
\*************************************************************/
WORD FAR PASCAL 
EnumListFile(FARPROC lpFunc, HWND hWnd, char *szListFile)
{
	int fp= -1;
	WORD nRetval=NULL;
	char szTemp[MAX_STR_LEN];
	WORD count= 0;
    
    if (LoadFile(szListFile) != OK)
        return 0;

    /* open file */
    if ((fp = _lopen(szListFile,READ)) == -1) 
        return 0;

    /* skip header */
    //if (ReadToChar(fp,'\n') == FAIL)
        //return 0;

    while (!eof(fp))
    {
        if (ReadStringToChar(fp,szTemp,'\t', sizeof(szTemp)) == FAIL)
            goto end;
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;

        if (((*lpFunc)(szTemp,hWnd)) == FALSE)
            goto end;

        ++count;
    }
    
end:
    if (fp != -1)
        _lclose(fp);
	
   return(count);
}

	
/*************************************************************\
 ** EnumSetFile
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate over library file names in szSetFile.
 **
 ** ASSUMES:
 **  szSetFile has full path.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 21, 1990 (written)
\*************************************************************/
WORD FAR PASCAL 
EnumSetFile(FARPROC lpFunc, HWND hWnd, char *szSetFile) 
{
    int fp= -1;
    char szLibName[_MAX_PATH];
	WORD count=0;
    
    if (LoadFile(szSetFile) != OK)
        return 0;

    if (bNULLFILE(szSetFile))
    {
        SET_NA(szLibName);
        if (((*lpFunc)(szLibName,hWnd)) == FALSE)
            return 0;
        else
            return 1;
    }

    /* open file */
    if ((fp = _lopen(szSetFile,READ)) == -1) 
        return 0;

	/* skip header */
    //if (ReadToChar(fp,'\n') == FAIL)
        //goto end;

    while (!eof(fp))
    {

        /* get lib name */
        if (ReadStringToChar(fp, szLibName, '\n', sizeof(szLibName)) == FAIL)
            goto end;

        if (((*lpFunc)(szLibName,hWnd)) == FALSE)
            goto end;

        ++count;
    }
    
    end:

    if (fp != -1)
        _lclose(fp);
	
   return(count);
}

/*************************************************************\
 ** EnumSubLibs
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate over sublibraries of szLibFile.
 **
 ** ASSUMES:
 **  szLibFile has full path.
 **  szLibFile has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 21, 1990 (written)
\*************************************************************/
WORD FAR PASCAL 
EnumSubLibs(FARPROC lpFunc, HWND hWnd, char *szLibFile) 
{
	int fp= -1;
	char szSubLib[MAX_STR_LEN];
	WORD count= 0;
    
    if (bNULLFILE(szLibFile))
    {
        SET_NA(szSubLib);
        if (((*lpFunc)(szSubLib,hWnd)) == FALSE)
            return 0;
        else
            return 1;
    }

    if (LoadFile(szLibFile) != OK)
        return 0;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return 0;

    /* skip to sublibs */
    //if (ReadToChar(fp,'\n') == FAIL)
        //return 0;
    if (ReadToChar(fp,'\n') == FAIL)
        return 0;

    while ((szSubLib[0] = (char)readch(fp)) != '')
    {
#ifdef INTERNAL
        //AuxPrint("RS from EnumSubLibs()");
#endif
        if (ReadStringToChar(fp, szSubLib+1, '\t', sizeof(szSubLib) - 1) == FAIL)
            goto end;
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;

        if (((*lpFunc)(szSubLib,hWnd)) == FALSE)
            goto end;

        ++count;
    }
    
end:
    if (fp != -1)
        _lclose(fp);
	
   return(count);
}



/*************************************************************\
 ** EnumMedia
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate over mdeia of szLibFile.
 **
 ** ASSUMES:
 **  szLibFile has full path.
 **  szLibFile has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 21, 1990 (written)
\*************************************************************/
WORD FAR PASCAL 
EnumMedia(FARPROC lpFunc, HWND hWnd, char *szLibFile) 
{
	int fp= -1;
	WORD count= 0;
    MEDIATYPE MType;
    
    if (bNULLFILE(szLibFile))
    {
	    char szMedia[MAX_STR_LEN];
        SET_NA(szMedia);
        if (((*lpFunc)(szMedia,hWnd)) == FALSE)
            return 0;
        else
            return 1;
    }

    if (LoadFile(szLibFile) != OK)
        return 0;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return 0;

    /* skip to media */
    if (ReadToChar(fp,'') == FAIL)
        return 0;
    if (ReadToChar(fp,'\n') == FAIL)
        return 0;

    while (!eof(fp))
    {
        char c;
        if ((c = readch(fp)) == FAIL)
            goto end;
        if (c == '') // all done
            goto end;
        MType = (MEDIATYPE)(c - '0');
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;

        if (((*lpFunc)(szMEDIA(MType),hWnd)) == FALSE)
            goto end;

        ++count;
    }
    
end:
    if (fp != -1)
        _lclose(fp);
	
   return(count);
}

#if 0
/*************************************************************\
 ** EnumClipDesc
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate over clipart descriptions in thumbnail info file.
 **
 ** ASSUMES:
 **  szThumbInfoFile has full path.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  March 20, 1990
\*************************************************************/
WORD FAR PASCAL 
EnumClipDesc(FARPROC lpFunc, HWND hWnd, char *szThumbInfoFile) 
{
    int fp= -1;
    char szDesc[MAX_STR_LEN];
	 WORD count=0;

    if (LoadFile(szThumbInfoFile) != OK)
        return 0;

    if (bNULLFILE(szThumbInfoFile))
    {
        SET_NA(szDesc);
        if (((*lpFunc)(szDesc,hWnd)) == FALSE)
            return 0;
        else
            return 1;
    }

    /* open file */
    if ((fp = _lopen(szThumbInfoFile,READ)) == -1) 
        return 0;

	/* skip header */
    //if (ReadToChar(fp,'\n') == FAIL)
        //goto end;

    while (!eof(fp))
    {
        /* get desc */
        if (ReadStringToChar(fp, szDesc, '\n', sizeof(szDesc)) == FAIL)
            goto end;

        if (((*lpFunc)(szDesc,hWnd)) == FALSE)
            goto end;

	    /* skip info */
        if (ReadToChar(fp,'') == FAIL)
            goto end;
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;

            ++count;
    }

    end:

    if (fp != -1)
        _lclose(fp);

   return(count);
}
#endif
/*************************************************************\
 ** EnumClipVersions
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Enumerate over clipart versions for given clipart in thumbnail info file.
 **
 ** ASSUMES:
 **  szThumbInfoFile has full path.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  March 20, 1990
\*************************************************************/
WORD FAR PASCAL 
EnumClipVersions(int nIndex, FARPROC lpFunc, HWND hWnd, char *szThumbInfoFile) 
{
    int fp= -1;
    char szVersion[MAX_STR_LEN],c;
	WORD count=0;
    
    if (LoadFile(szThumbInfoFile) != OK)
        return 0;

    if (bNULLFILE(szThumbInfoFile))
    {
        SET_NA(szVersion);
        if (((*lpFunc)(szVersion,hWnd)) == FALSE)
            return 0;
        else
            return 1;
    }

    /* open file */
    if ((fp = _lopen(szThumbInfoFile,READ)) == -1) 
        return 0;

	/* skip header */
    //if (ReadToChar(fp,'\n') == FAIL)
        //return 0;

    while (nIndex--)
    {
	    /* skip cliparts */
        if (ReadToChar(fp,'') == FAIL)
            goto end;
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;
    }
    
    /* got to clipart, skip desc */
    if (ReadToChar(fp,'\n') == FAIL)
        goto end;

    while (!eof(fp))
    {
        /* check if done */
        if ((c = readch(fp)) == '')
            break;

        szVersion[0] = c;

        /* read version */
#ifdef INTERNAL
        //AuxPrint("RS from EnumClipVersions()");
#endif
        if (ReadStringToChar(fp, szVersion+1, '\t', sizeof(szVersion)-1) == FAIL)
            goto end;
        if (ReadToChar(fp,'\n') == FAIL)
            goto end;

        if (((*lpFunc)(szVersion,hWnd)) == FALSE)
            goto end;

        ++count;
    }

    end:

    if (fp != -1)
        _lclose(fp);
	
   return(count);
}

WORD FAR PASCAL 
EnumLibVersions(FARPROC lpFunc, HWND hWnd, char *szLibFile) 
{
	int fp= -1;
	WORD count= 0;
	char szVersion[MAX_STR_LEN];
    
    if (bNULLFILE(szLibFile))
    {
        SET_NA(szVersion);
        if (((*lpFunc)(szVersion,hWnd)) == FALSE)
            return 0;
        else
            return 1;
    }

    if (LoadFile(szLibFile) != OK)
        return 0;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return 0;

    /* skip to versions */
    if (ReadToChar(fp,'') == FAIL)
        return 0;
    if (ReadToChar(fp,'') == FAIL)
        return 0;
    if (ReadToChar(fp,'\n') == FAIL)
        return 0;

    while (!eof(fp))
    {
        if (ReadStringToChar(fp, szVersion, '\n', sizeof(szVersion)) == FAIL)
            goto end;

        if (((*lpFunc)(szVersion,hWnd)) == FALSE)
            goto end;

        ++count;
    }

end:
    if (fp != -1)
        close(fp);

   return(count);
}

int FAR PASCAL 
QueryListFileName(int nIndex, char *szMasterFile, char *szListFileName)
{
	int	fp= -1;
    int nRetval=OK;

    /* open file */
    if ((fp = _lopen(szMasterFile,READ)) == -1) 
        return FAIL;

	/* skip over header */	
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)

    if (szListFileName)
    {
        while (nIndex--)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)

        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)

        if (ReadStringToChar(fp, szListFileName, '\n', MAX_STR_LEN) == FAIL)
            END(FAIL)
        AddFullPath(szListFileName);
    }

    end:

    if (fp != -1)
        _lclose(fp);

    return nRetval;
}

/*************************************************************\
 ** QuerySetFileName
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return SetFileName corresponding to nIndex in szListFile.
 **  Full path is prepended.
 **  szSetFileName may be NULL.
 **
 ** ASSUMES:
 **  szListFile has full path.
 **  szListFile has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 28, 1990 (written)
\*************************************************************/
int FAR PASCAL 
QuerySetFileName(int nIndex, char *szListFile, char *szSetFileName)
{
	int	fp= -1;
    int nRetval=OK;

    if (bNULLFILE(szListFile))
    {
        if (szSetFileName)
            SET_NULL(szSetFileName);
        return OK;
    }

    if (LoadFile(szListFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szListFile,READ)) == -1) 
        return FAIL;

	/* skip over header */	
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)


    if (szSetFileName)
    {
        while (nIndex--)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)

        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)

        if (ReadStringToChar(fp,szSetFileName,'\n', MAX_STR_LEN) == FAIL)
            END(FAIL)
        AddFullPath(szSetFileName);
    }

    end:

    if (fp != -1)
        _lclose(fp);

    return nRetval;
}



/*************************************************************\
 ** QueryLibFileName
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return LibFileName corresponding to nIndex in szSetFileName.
 **  Full path is prepended.
 **  szLibFileName may be NULL.
 **
 ** ASSUMES:
 **  szSetFileName has full path.
 **  szSetFileName has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 28, 1990 (written)
\*************************************************************/
int FAR PASCAL 
QueryLibFileName(int nIndex, char *szSetFileName, char *szLibFileName)
{
	int	fp= -1;
    int nRetval=OK;

    if (bNULLFILE(szSetFileName))
    {
        if (szLibFileName)
            SET_NULL(szLibFileName);
        return OK;
    }

    if (LoadFile(szSetFileName) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szSetFileName,READ)) == -1) 
        return FAIL;

	/* skip over header */	
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)

    if (szLibFileName)
    {
        while (nIndex--)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)

        if (ReadStringToChar(fp,szLibFileName,'\n', MAX_STR_LEN) == FAIL)
            END(FAIL)

        AddFullPath(szLibFileName);
    }

    end:

    if (fp != -1)
        _lclose(fp);

    return nRetval;
}

/*************************************************************\
 ** QueryLibName
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return LibName corresponding to nIndex in szSetFileName.
 **  szLibName may be NULL.
 **
 ** ASSUMES:
 **  szSetFileName has full path.
 **  szSetFileName has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 28, 1990 (written)
\*************************************************************/
#if 0
int FAR PASCAL 
QueryLibName(int nIndex, char *szSetFileName, char *szLibName)
{
	int	fp= -1;
    int nRetval=OK;

    if (bNULLFILE(szSetFileName))
    {
        if (szLibName)
            SET_NA(szLibName);
        return OK;
    }

    if (LoadFile(szSetFileName) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szSetFileName,READ)) == -1) 
        return FAIL;

	/* skip over header */	
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)

    if (szLibName)
    {
	    char szTmp[MAX_STR_LEN];

        while (nIndex--)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)

        if (ReadStringToChar(fp,szTmp,'\t', sizeof(szTmp)) == FAIL)
            END(FAIL)

        /** now get library name from libinfo file **/
        if (QueryLibInfo(szTmp,szLibName,NULL,NULL,NULL,NULL) != OK)
            END(FAIL)
    }

    end:

    if (fp != -1)
        _lclose(fp);

    return nRetval;
}
#endif

/*************************************************************\
 ** QueryLibInfo
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **
 ** ASSUMES:
 **  szLibFile has full path and has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  March 1, 1990 (written)
\*************************************************************/
int FAR PASCAL	
QueryLibInfo(char *szLibFile, 
             char *szLibName,
             char *szPublisherFile, 
             char *szPublisher, 
             BOOL *bDownloadable,
             HWND hDesc,
             char *szDesc,
             char *szNumClips)
{
	int fp= -1;
	int nRetval=OK;
	char szPubFile[_MAX_PATH];
    char bTmpDownloadable;

    if (bNULLFILE(szLibFile))
    {
        if (szLibName)
            SET_NA(szLibName);
        if (szPublisher)
            lstrcpy(szPublisher,"None");
        if (szPublisherFile)
            SET_NULL(szPublisherFile);
        if (szNumClips)
            strcpy(szNumClips,"0");
        if (bDownloadable)
            bTmpDownloadable = FALSE;
        if (hDesc)
            SetWindowText (hDesc, (LPSTR)"");
        if (szDesc)
            szDesc[0] = '\0';
        return nRetval;
    }

    if (LoadFile(szLibFile) != OK)
        return FAIL;

    /* open file */
	if ((fp = _lopen(szLibFile, READ)) == -1) 
        return FAIL;

    if (ReadStringToChar(fp,szLibName,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)

    if (ReadStringToChar(fp,szPubFile,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)
	if (szPublisherFile)
        lstrcpy(szPublisherFile,szPubFile);

    if (bDownloadable)
    {
        if ((bTmpDownloadable = readch(fp)) == FAIL)
            END(FAIL)
        *bDownloadable = (bTmpDownloadable == '1') ? TRUE : FALSE;
    }

    if (ReadToChar(fp,'\t') == FAIL)
        END(FAIL)

	if (szPublisher)
    {
        if (QueryIndirection(szPubFile,szPublisher) != OK)
            END(FAIL)
    }

	if (hDesc)
		FillEditBox(fp,hDesc,'\t');
    else if (ReadStringToChar(fp,szDesc,'\t', MAX_STR_LEN) == FAIL)
            END(FAIL)

    if (ReadStringToChar(fp,szNumClips,'\n', MAX_STR_LEN) == FAIL)
        END(FAIL)

    end:
    if (fp != -1)
        _lclose(fp);

   return nRetval;
}

/*************************************************************\
 ** QueryThumbnail
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return thumbnail file name for sublib nIndex in szLibFile.
 **  szThumbnail may be NULL.
 **
 ** ASSUMES:
 **  szLibFile has full path and has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 21, 1990 (written)
\*************************************************************/
int FAR PASCAL 
QueryThumbnailFileName(int nIndex, char *szLibFile, char *szThumbnail)
{
	int fp= -1;
    int nRetval=OK;
    
    if (bNULLFILE(szLibFile))
    {
        if (szThumbnail)
            SET_NULL(szThumbnail);
        return OK;
    }

    if (LoadFile(szLibFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return FAIL;

    /* skip to sublibs */
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)
    if (ReadToChar(fp,'\n') == FAIL)
        END(FAIL)

    if (szThumbnail)
    {
        while (nIndex--)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)

        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)
        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)

#ifdef INTERNAL
        //AuxPrint("RS from QueryThumbnailFileName()");
#endif
        if (ReadStringToChar(fp,szThumbnail,'\n', MAX_STR_LEN) == FAIL)
            END(FAIL)

        AddFullPath(szThumbnail);
    }

end:
    if (fp != -1)
        _lclose(fp);
	
   return nRetval;
}

int FAR PASCAL 
QueryMediaType(int nIndex, char *szLibFile, MEDIATYPE *MType)
{
	int fp= -1;
    int nRetval=OK;
    
    if (bNULLFILE(szLibFile))
    {
        *MType = -1;
        return OK;
    }

    if (LoadFile(szLibFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return FAIL;

    /* skip to media */
    if (ReadToChar(fp,'') == FAIL)
        END(FAIL)
    if (ReadToChar(fp,'\n') == FAIL)
        END(FAIL)

    while (nIndex--)
        if (ReadToChar(fp,'\n') == FAIL)
            END(FAIL)

    if (*MType = (MEDIATYPE)(readch(fp) - '0') == FAIL)
        END(FAIL)

end:
    if (fp != -1)
        _lclose(fp);
	
   return nRetval;
}

int FAR PASCAL 
QueryThumbInfoFileName(int nIndex, char *szLibFile, char *szThumbInfo)
{
	int fp= -1;
    int nRetval=OK;
    
    if (bNULLFILE(szLibFile))
    {
        if (szThumbInfo)
            SET_NULL(szThumbInfo);
        return OK;
    }

    if (LoadFile(szLibFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return FAIL;

    /* skip to sublibs */
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)
    if (ReadToChar(fp,'\n') == FAIL)
        END(FAIL)

    if (szThumbInfo)
    {
        while (nIndex--)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)

        if (ReadToChar(fp,'\t') == FAIL)
            END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from QueryThumbInfoFileName()");
#endif
        if (ReadStringToChar(fp,szThumbInfo,'\t', MAX_STR_LEN) == FAIL)
            END(FAIL)

        AddFullPath(szThumbInfo);
    }

end:
    if (fp != -1)
        _lclose(fp);
	
   return nRetval;
}

/*************************************************************\
 ** QueryMediaPrices
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return SR and WS prices of media nIndex in library szLibFile.
 **  szSRPrice and/or szWSPrice may be NULL.
 **
 ** ASSUMES:
 **  szLibFile has full path and has been loaded.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  February 21, 1990 (written)
\*************************************************************/
int FAR PASCAL 
QueryMediaPrices(int nIndex, char *szLibFile, char *szSRPrice, char *szWSPrice)
{
	int fp= -1;
    int nRetval=OK;
    
    if (bNULLFILE(szLibFile))
    {
        if (szSRPrice)
            lstrcpy(szSRPrice,"NA");

        if (szWSPrice)
            lstrcpy(szWSPrice,"NA");

        return OK;
    }

    if (LoadFile(szLibFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szLibFile,READ)) == -1) 
        return FAIL;

    /* skip to media */
    if (ReadToChar(fp,'') == FAIL)
        END(FAIL)
    if (ReadToChar(fp,'\n') == FAIL)
        END(FAIL)

    while (nIndex--)
        if (ReadToChar(fp,'\n') == FAIL)
            END(FAIL)

    if (ReadToChar(fp,'\t') == FAIL)
        END(FAIL)

    if (ReadStringToChar(fp,szSRPrice,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)

    if (ReadStringToChar(fp,szWSPrice,'\n', MAX_STR_LEN) == FAIL)
        END(FAIL)

end:
    if (fp != -1)
        _lclose(fp);
	
   return nRetval;
}

/*************************************************************\
 ** QueryClipDesc
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return ClipDesc corresponding to nIndex in szThumbInfoFile
 **  szClipDesc may be NULL.
 **
 ** ASSUMES:
 **  szThumbInfoFile has full path.
 **
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  March 19, 1990 (written)
\*************************************************************/
int FAR PASCAL 
QueryClipDesc(int nIndex, char *szThumbInfoFile, char *szClipDesc)
{
	int	fp= -1;
    int nRetval=OK;

    if (bNULLFILE(szThumbInfoFile))
    {
        if (szClipDesc)
            lstrcpy(szClipDesc,"");
        return OK;
    }

    if (LoadFile(szThumbInfoFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szThumbInfoFile,READ)) == -1) 
        return FAIL;

	/* skip over header */	
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)

    if (szClipDesc)
    {
        while (nIndex--)
        {
            if (ReadToChar(fp,'') == FAIL)
                END(FAIL)
            if (ReadToChar(fp,'\n') == FAIL)
                END(FAIL)
        }

#ifdef INTERNAL
        //AuxPrint("RS from QueryClipDesc()");
#endif
        if (ReadStringToChar(fp,szClipDesc,'\n', MAX_STR_LEN) == FAIL)
            END(FAIL)
    }

    end:

    if (fp != -1)
        _lclose(fp);

    return nRetval;
}

/*************************************************************\
 ** QueryClipVerInfo
 **
 ** ARGUMENTS:
 **
 ** DESCRIPTION:
 **  Return Clip info for clipart corresponding to nClipIndex
 **  and version corresponding to nVerIndex in szThumbInfoFile.
 **  Return parms may be NULL.
 **
 ** ASSUMES:
 **  szThumbInfoFile has full path.
 **  
 **  Also assumes that there are buffers available for szVersion,
 **  szPrice, szFileSize and szClipFileName
 **  
 ** MODIFIES:
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Doug Kent  March 19, 1990 (written)
\*************************************************************/
int FAR PASCAL 
QueryClipVerInfo(int nClipIndex, int nVerIndex, char *szThumbInfoFile, 
        char *szVersion,
        char *szPrice,
        char *szClipFileSize,   // size of file when uncompressed
        char *szClipFileName    // name of file at bbs
        )
{
	int	fp= -1;
    int nRetval=OK;

    if (bNULLFILE(szThumbInfoFile))
    {
        if (szVersion)
            lstrcpy(szVersion,"");
        if (szPrice)
            lstrcpy(szPrice,"");
        if (szClipFileSize)
            lstrcpy(szClipFileSize,"");
        if (szClipFileName)
            lstrcpy(szClipFileName,"");
        return OK;
    }

    if (LoadFile(szThumbInfoFile) != OK)
        return FAIL;

    /* open file */
    if ((fp = _lopen(szThumbInfoFile,READ)) == -1) 
        return FAIL;

	/* skip over header */	
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)

    while (nClipIndex--)
    {
        if (ReadToChar(fp,'') == FAIL)
            END(FAIL)
        if (ReadToChar(fp,'\n') == FAIL)
            END(FAIL)
    }

	/* skip over desc */	
    if (ReadToChar(fp,'\n') == FAIL)
        END(FAIL)

    while (nVerIndex--)
        if (ReadToChar(fp,'\n') == FAIL)
            END(FAIL)

#ifdef INTERNAL
        //AuxPrint("RS from QueryClipVerInfo(), 1");
#endif
    if (ReadStringToChar(fp,szVersion,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from QueryClipVerInfo(), 2");
#endif
    if (ReadStringToChar(fp,szPrice,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from QueryClipVerInfo(), 3");
#endif
    if (ReadStringToChar(fp,szClipFileSize,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from QueryClipVerInfo(), 4");
#endif
    if (ReadStringToChar(fp,szClipFileName,'\n', MAX_STR_LEN) == FAIL)
        END(FAIL)

    end:

    if (fp != -1)
        _lclose(fp);

    return nRetval;
}

BOOL FAR PASCAL 
QueryIndirection (char *szFileName, char *szStr)
{
    int fp = -1;
    int nRetval = OK;

    if (bNULLFILE(szFileName))
    {
        SET_NA(szStr);
        return OK;
    }

    AddFullPath(szFileName);
    if (LoadFile(szFileName) != OK)
        return FAIL;

    /* open it */
    if ((fp = _lopen(szFileName,READ)) == -1)
        return FAIL;

    /* read name (note skipping header) */
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)
#ifdef INTERNAL
        //AuxPrint("RS from QueryIndirection(), 1");
#endif
    if (ReadStringToChar(fp,szStr,'\t', MAX_STR_LEN) == FAIL)
        END(FAIL)

    end:
    if (fp != -1)
        _lclose(fp);

	return (nRetval);
}

int FAR PASCAL 
QueryCopyright(char *szPubFileName, HANDLE hEdit)
{
    int fp = -1;
    int nRetval = OK;

    if (bNULLFILE(szPubFileName))
        return FAIL;

    AddFullPath(szPubFileName);
    if (LoadFile(szPubFileName) != OK)
        return FAIL;

    /* open it */
    if ((fp = _lopen(szPubFileName,READ)) == -1)
        return FAIL;

    /* read name (note skipping header) */
    //if (ReadToChar(fp,'\n') == FAIL)
        //END(FAIL)
    if (ReadToChar(fp,'\t') == FAIL)
        END(FAIL)

    FillEditBox(fp,hEdit,EOF);

    end:
    if (fp != -1)
        _lclose(fp);

	return (nRetval);
}

unsigned char FAR PASCAL
readch(int fHandle)
{
    char c;
    if (_lread(fHandle,&c,1) < 1)
        return FAIL;
    return (c);
}

#if 0
int FAR PASCAL
writech(unsigned char c, int fHandle)
{
    if (c == '\n')
    {
        char cr = '\r';
        if (_lwrite(fHandle,&cr,1) < 1)
            return FAIL;
    }
    if (_lwrite(fHandle,&c,1) < 1)
        return FAIL;
    return OK;
}
#endif

int FAR PASCAL
ReadToChar(int fHandle, char chartoget)
{
    char c;
    do
    {
        if (_lread(fHandle,&c,1) < 1)
            return FAIL;
    }
    while (c != chartoget);

    return OK;
}


int FAR PASCAL
ReadStringToChar(int fHandle, char *str, char chartoget, int nSize)
{
#ifdef INTERNAL
    char* p = str;
#endif
    if ((str == NULL) OR (nSize <= 0))
        return ReadToChar(fHandle,chartoget);

    if (chartoget == '\r')    // else infinite loop
       return FAIL;

    while (nSize AND (_lread(fHandle, (LPSTR)str, 1) > 0))
    {
        /** ignore '\r' since these are ascii files **/
        if (*str == '\r')
            continue;

        if (*str == chartoget)
        {
           *str = '\0';
           return OK;
        }
        ++str;
        nSize--;
    }
#ifdef INTERNAL
    if (NOT nSize)
    {
         *(p + (str-p)) = '\0';
         AuxPrint("!!! Maxed out buffer during ReadStringToChar, string read was %s!!!", p);
    }
#endif
    //just in case
    if (nSize)
      *str = '\0';

    return FAIL;
}

#if 0
int FAR PASCAL
WriteStr(int fHandle, char *str, char termchar)
{
    while(*str)
    {
        if (*str == '\n')
        {
            char cr = '\r';
            if (_lwrite(fHandle,&cr,1) < 1)
                return FAIL;
        }
        if (_lwrite(fHandle,str,1) < 1)
            return FAIL;
        ++str;
    }
    if (termchar)
    {
        if (termchar == '\n')
        {
            char cr = '\r';
            if (_lwrite(fHandle,&cr,1) < 1)
                return FAIL;
        }
        if (_lwrite(fHandle,&termchar,1) < 1)
            return FAIL;
    }
    return OK;
}
#endif
