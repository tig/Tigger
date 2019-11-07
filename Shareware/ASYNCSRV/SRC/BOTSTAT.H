/************************************************************************
 *
 *     Copyright (c) 1992-93 C.E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  botstat.h
 *
 ***********************************************************************/
#ifndef _BOTSTAT_H_
#define _BOTSTAT_H_

HWND FAR PASCAL CreateStatus( HWND hwndParent ) ;
BOOL FAR PASCAL DestroyStatus( HWND hwnd ) ;

#define NUM_STATBOXES 11

#define IDSTAT_RECEIVE_LBL     0
#define IDSTAT_RECEIVE         1
#define IDSTAT_SEND_LBL        2
#define IDSTAT_SEND            3
#define IDSTAT_PORT            4
#define IDSTAT_BAUD            5
#define IDSTAT_CD              6
#define IDSTAT_DSR             7
#define IDSTAT_CTS             8
#define IDSTAT_CONNECT         9
#define IDSTAT_DDE             10


#endif

/************************************************************************
 * End of File: botstat.h
 ***********************************************************************/

