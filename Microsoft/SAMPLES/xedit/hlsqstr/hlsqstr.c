//*************************************************************
//  File name: hlsq.c
//
//  Description: 
//      Entry and exit points for the HLSQ DLL
//
//  History:    Date       Author     Comment
//
//*************************************************************

#include <windows.h>
#include <string.h>

int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                                   WORD wHeapSize, LPSTR lpszCmdLine )
{
    if ( wHeapSize != 0 )
        UnlockData( 0 );
    return 1;   // Indicate that the DLL was initialized successfully.
}

int FAR PASCAL WEP ( int bSystemExit )
{
    return 1;
}

//*************************************************************
//
//  dummy()
//
//  Purpose:
//		dummy fuction that forces the linking in of all _fstr* stuff
//
//
//  Parameters:
//      void
//      
//
//  Return: (void)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//              12/ 4/91   MSM        Created
//
//*************************************************************

void dummy(void)
{
    _fmemccpy   (NULL,  NULL,   0,  0);
    _fmemchr    (NULL,  0,      0);
    _fmemcmp    (NULL,  NULL,   0);
    _fmemcpy    (NULL,  NULL,   0);
    _fmemicmp   (NULL,  NULL,   0);
    _fmemmove   (NULL,  NULL,   0);
    _fmemset    (NULL,  0,      0);
    _fstrncat   (NULL,  NULL,   0);
    _fstrncmp   (NULL,  NULL,   0);
    _fstrnicmp  (NULL,  NULL,   0);
    _fstrncpy   (NULL,  NULL,   0);
    _fstrnset   (NULL,  0,      0);
    _fstrcat    (NULL,  NULL);
    _fstrcmp    (NULL,  NULL);
    _fstricmp   (NULL,  NULL);
    _fstrcpy    (NULL,  NULL);
    _fstrcspn   (NULL,  NULL);
    _fstrpbrk   (NULL,  NULL);
    _fstrspn    (NULL,  NULL);
    _fstrstr    (NULL,  NULL);
    _fstrtok    (NULL,  NULL);
    _fstrrchr   (NULL,  0);
    _fstrchr    (NULL,  0);
    _fstrset    (NULL,  0);
    _fstrdup    (NULL);
    _fstrlen    (NULL);
    _fstrlwr    (NULL);
    _fstrupr    (NULL);
    _fstrrev    (NULL);

} /* dummy() */

/*** EOF: hlsq.c ***/
