/************************************************************************
 *
 *     Project:  
 *
 *      Module:  botstat.h
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _BOTSTAT_H_
#define _BOTSTAT_H_

#include "..\inc\status.h"

HWND FAR PASCAL CreateStatus( HWND hwndParent ) ;
BOOL FAR PASCAL DestroyStatus( HWND hwnd ) ;
void FAR PASCAL SetHelpAvailIndicator( HWND hwndStat, BOOL fOn ) ;
void FAR PASCAL SetDirtyIndicator( HWND hwndStat, BOOL fOn ) ;

#define NUM_STATBOXES 2
#define IDSTAT_DIRTY    0 
#define IDSTAT_HELP    1

#endif

/************************************************************************
 * End of File: botstat.h
 ***********************************************************************/

