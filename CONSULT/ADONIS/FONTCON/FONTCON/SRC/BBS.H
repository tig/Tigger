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

extern BOOL              gfDBUpdated ;

UINT WINAPI BBSConnect( HWND hwndDisable ) ;
BOOL WINAPI BBSDisConnect( VOID ) ;
BOOL WINAPI IsBBSConnected( VOID ) ;

UINT FAR WINAPI SendBBSMsg( MSGTYPE msg, LPVOID lP1, LPVOID lP2 ) ;


BOOL WINAPI UpdateExecutables( HWND hwndParent ) ;
BOOL WINAPI UpdateDatabaseFiles( HWND hwndParent ) ;
UINT WINAPI CheckAccountStatus( HWND hwndParent ) ;
DWORD WINAPI CalcCRC32( LPBYTE lpBuf, int cb ) ;

/*
 * Each of these exit codes has an error string associated with
 * it.  these strings are defined in shopsrv.str.  Any changes
 * made here must also be made in shopsrv.str (look for the
 * IDS_ERR_EC_* defines.
 */
#define EXITCODE_OK                 0x0000
#define EXITCODE_ACK_BAD            0x0001
#define EXITCODE_ACK_TIMEOUT        0x0002
#define EXITCODE_LOSTCONNECTION     0x0003
#define EXITCODE_RECEIVE_TIMEOUT    0x0004
#define EXITCODE_RECEIVE_CRC        0x0005
#define EXITCODE_GENERALFAILURE     0x0006



#endif 

/************************************************************************
 * End of File: bbs.h
 ***********************************************************************/

