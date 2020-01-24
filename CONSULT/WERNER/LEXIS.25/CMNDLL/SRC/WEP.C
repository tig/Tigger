/************************************************************************
 *
 *     Project:  CMNDLL
 *
 *      Module:  wep.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include <windows.h>

extern HANDLE hInst ;

BOOL FAR PASCAL BmpBtnUnInit( HANDLE hInst ) ;

//    FUNCTION:  WEP(int)
//
//    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
//              called automatically by Windows when the DLL is unloaded (no
//              remaining tasks still have the DLL loaded).  It is strongly
//              recommended that a DLL have a WEP() function, even if it does
//              nothing but returns success (1), as in this example.
//
int FAR PASCAL WEP( int bSystemExit )
{
   WORD v ;

   v = GetVersion() ;

   if (HIBYTE( v ) > 0 && LOBYTE( v ) >= 3)
      BmpBtnUnInit( hInst ) ;

   return 1 ;
}

/************************************************************************
 * End of File: wep.c
 ***********************************************************************/

