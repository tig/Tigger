/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  comm.h
 *
 ***********************************************************************/


#ifndef _COMM_H_
#define _COMM_H_

#include  "..\..\inc\ascii.h"

#define CRC WORD

#define DCB_MSRSHADOW  35
#define ACE_RLSD       0x80
#define ACE_DSR        0x20
#define ACE_CTS        0x10

// Flow control flags

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04


// COMM Speicifc functions.
//
int       WINAPI GetCommID(VOID);
BOOL     WINAPI OpenConnection( VOID ) ;
BOOL     WINAPI CloseConnection( VOID ) ;
int      WINAPI ReadCommBlock( LPVOID, int ) ;
BOOL     WINAPI WriteCommByte( BYTE ) ;
BOOL     WINAPI WriteCommBlock( LPVOID, DWORD ) ;
BOOL     WINAPI WriteCommBlockNoEcho( LPVOID lpBlock, DWORD  cb ) ;
BOOL     WINAPI SetCommPort( LPSTR lpszPort ) ;
BOOL     WINAPI SetBaudRate( DWORD dwBaud ) ;
DWORD WINAPI GetBaudRate( VOID ) ;
BOOL     WINAPI IsOnline( VOID ) ;

BOOL WINAPI SetCommNotificationWnd( HWND hwnd ) ;
BOOL WINAPI ProcessCOMMNotification( UINT iCid, LONG lParam ) ;

#define INPUTBUFFER_SIZE      (1024 * 4)
VOID WINAPI ResetInputBuffer( VOID ) ;
BOOL WINAPI PutInBYTE( BYTE b ) ;
DWORD WINAPI PeekInBYTE( LPBYTE lpb ) ;
DWORD WINAPI GetInBYTE( LPBYTE lpb ) ;
DWORD WINAPI GetInBuffer( LPVOID lpBuf, DWORD cb ) ;

BOOL WINAPI FlushInComm( VOID ) ;
BOOL WINAPI FlushOutComm( VOID ) ;

CRC WINAPI CalcCRC( LPVOID lpBuf, DWORD cb ) ;

BOOL WINAPI TellCommToDoZmodem( HWND hwnd ) ;

/*
 * Modem response defines
 */
#define MR_OK              0x0000
#define MR_CONNECT         0x0001
#define MR_CONNECT300      0x0002
#define MR_CONNECT1200     0x0003
#define MR_CONNECT2400     0x0004
#define MR_CONNECT4800     0x0005
#define MR_CONNECT9600     0x0006
#define MR_BUSY            0x0007
#define MR_NODIALTONE      0x0008
#define MR_NOCARRIER       0x0009
#define MR_ERROR           0x000a

#endif
/************************************************************************
 * End of File: comm.h
 ***********************************************************************/

