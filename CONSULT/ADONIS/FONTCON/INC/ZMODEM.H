/************************************************************************
 *
 *     Project:  Shopper
 *
 *      Module:  zmodem.h
 *
 ***********************************************************************/

/*
 * This is the only entry point into the DLL
 */
LRESULT WINAPI Zmodem( HWND hwnd, UINT wCmd, LPARAM lParam1, LPARAM lParam2 ) ;

#define IDTIMER_ZMODEM           0x4242

/*
 * Zmodem( ZM_INIT ) requires as it's lParam1 a pointer to a
 * callback function of the following form.  This function is
 * called each time Zmodem needs to send a buffer
 */
typedef UINT (CALLBACK*  ZMODEMSENDPROC)(HWND, LPBYTE, UINT );

/*
 * These are the commands (wCmd) for Zmodem()
 */
#define ZM_INIT                  0x0000
#define ZM_KILL                  0x0001
#define ZM_QUERYSTATUS           0x0002
#define ZM_SEND                  0x0003
#define ZM_RECEIVE               0x0004
#define ZM_ACK_FILECOMPLETED     0x0005
#define ZM_ACK_COMPLETED         0x0006
#define ZM_TIMER                 0x0007
#define ZM_ABORT                 0x0008
#define ZM_GETUSECOUNT           0X0009
#define ZM_RXBLK                 0x000a
#define ZM_SHOWSTATUS            0x000b
#define ZM_KILLSTATUS            0x000c
#define ZM_LOCALCANCEL           0x000d

/*
 * These commands are used to get statistics of the transfer...
 * They are also the control ID's of the ZmodemStatus
 * dialog box.
 */
#define ZM_GET_BLOCKCOUNT        0x8001
#define ZM_GET_BLOCKSIZE         0x8002
#define ZM_GET_CANCOUNT          0x8003
#define ZM_GET_CHARCOUNT         0x8004
#define ZM_GET_CONSECERRORS      0x8005
#define ZM_GET_CRCTYPE           0x8006
#define ZM_GET_CURFILE           0x8007
#define ZM_GET_FILEBYTES         0x8008
#define ZM_GET_FILEDATE          0x8009
#define ZM_GET_FILENAME          0x800a
#define ZM_GET_FILESIZE          0x800b
#define ZM_GET_FILETIME          0x800c
#define ZM_GET_INCOMINGCOUNT     0x800d
#define ZM_GET_INCOMINGPOS       0x800e
#define ZM_GET_LASTERROR         0x800f
#define ZM_GET_LENGTH            0x8010
#define ZM_GET_MAXERRORS         0x8011
#define ZM_GET_OUTCOUNT          0x8012
#define ZM_GET_POSITION          0x8013
#define ZM_GET_RECEIVED          0x8014
#define ZM_GET_TIMEREAD          0x8015
#define ZM_GET_TIMESTARTED       0x8016
#define ZM_GET_TOTALBLOCKS       0x8017
#define ZM_GET_TOTALERRORS       0x8018
#define ZM_GET_TOTALFILES        0x8019
#define ZM_GET_TRANSMITTED       0x801a

/*
 * These are the status codes returned by Zmodem().
 */
#define ZMS_OK                   0x00000000
#define ZMS_IDLE                 0x00000001
#define ZMS_SENDING              0x00000002
#define ZMS_RECEIVING            0x00000003
#define ZMS_FILECOMPLETED        0x00000004
#define ZMS_COMPLETED            0x00000005

#define ZMS_ERROR                0xFFFFFFFF
#define ZMS_LOCALCANCEL          0xFFFF0000  // some of these correspond
#define ZMS_TOOMANYERRORS        0xFFFF0001  // to errors defined below
#define ZMS_TIMEOUT              0xFFFF0002
#define ZMS_HOSTCANCEL           0xFFFF0003
#define ZMS_DISKERROR            0xFFFF0004
#define ZMS_DISKFULL             0xFFFF0005
#define ZMS_NAK                  0xFFFF0006

/************************************************************************
 * End of File: zmodem.h
 ***********************************************************************/


