/************************************************************************
 *
 *     Project:  
 *
 *      Module:  mdi.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "PRECOMP.H"
#include "global.h"

LRESULT CALLBACK fnMDIChild( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uiMsg)
   {
      case WM_MDIACTIVATE:
         if (wParam)
            ghwndActiveMDIChild = hWnd ;
         else
            ghwndActiveMDIChild = NULL ;
      break ;

      case WM_SETFOCUS:
      break ;

      case WM_SIZE:
         goto CallDCP ;
      break ;

      default:
CallDCP:
         return DefMDIChildProc( hWnd, uiMsg, wParam, lParam ) ;

   }

   return 0 ;

}


/************************************************************************
 * End of File: mdi.c
 ***********************************************************************/

