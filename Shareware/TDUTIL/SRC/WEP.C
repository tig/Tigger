/************************************************************************
 *
 *     Project:  Project-42
 *
 *      Module:  wep.c
 *
 *     Remarks:  Generic WEP.  Must be in a fixed CS.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"

extern HBITMAP    hbmBtn ;

/****************************************************************
 *  int FAR PASCAL WEP( int bSystemExit )
 *
 ****************************************************************/
int FAR PASCAL WEP( int bSystemExit )
{
   #ifdef WIN32
   UNREFERENCED_PARAMETER(bSystemExit) ;
   #endif

   if (hbmBtn)
   {
      DeleteObject( hbmBtn ) ;
   }

   return 1 ;

} /* WEP()  */


/************************************************************************
 * End of File: wep.c
 ***********************************************************************/

