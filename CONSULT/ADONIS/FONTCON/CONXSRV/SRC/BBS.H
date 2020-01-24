/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  bbs.h
 *
 ***********************************************************************/
#ifndef _BBS_H_
#define _BBS_H_

#include "..\..\inc\messages.h"

UINT FAR WINAPI SendBBSMsg( MSGTYPE msg, LPVOID lP1, LPVOID lP2 ) ;
UINT WINAPI ProcessSendBBSMsg( LPGENERICMSG lpMsg ) ;

#endif 

/************************************************************************
 * End of File: bbs.h
 ***********************************************************************/

