//*************************************************************
//  File name: conxmem.c
//
//  Description: 
//      Entry and exit points for the HLSQ DLL
//
//  History:    Date       Author     Comment
//
//*************************************************************

#include "PRECOMP.H"

#include "intfsmm.h"

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
//  fsmmCheckVersion
//
//  Purpose:
//      Checks the expected version for compatibility
//
//
//  Parameters:
//      WORD wVersion
//      
//
//  Return: (WORD FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               1/28/92   MSM        Created
//
//*************************************************************

WORD FAR PASCAL fsmmCheckVersion( WORD wVersion )
{
    if (wVersion > CONXMEM_VER)
        return 0;

    return CONXMEM_VER;

} //*** fsmmCheckVersion

/*** EOF: conxmem.c ***/
