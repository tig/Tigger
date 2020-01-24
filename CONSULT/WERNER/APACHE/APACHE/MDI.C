/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  mdi.c
 *
 *     Remarks:
 *
 *    This module manages the MDI windows.  All MDI processing must
 *    go through here.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/

#include "precomp.h"   // precompiled headers (windows.h, etc.)

#include "apache.h"
#include "misc.h"

/****************************************************************
 *  HWND FAR PASCAL CreateMDIClient( HWND hwnd )
 *
 *  Description: 
 *
 *    Creates the MDI client window.
 *
 *  Comments:
 *
 ****************************************************************/

HWND FAR PASCAL CreateMDIClient( HWND hWnd )
{
   CLIENTCREATESTRUCT  clientcreate ;

   clientcreate.hWindowMenu = hMenuInitWindow ;
   clientcreate.idFirstChild = IDM_FIRSTCHILD ;

   return CreateWindow( "MDICLIENT", NULL,
                        WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                        0, 0, 0, 0, hWnd, (HMENU) 1, GETHINST( hWnd ),
                        (LPVOID) &clientcreate ) ;

} /* CreateMDIClient()  */

/************************************************************************
 * End of File: mdi.c
 ***********************************************************************/

