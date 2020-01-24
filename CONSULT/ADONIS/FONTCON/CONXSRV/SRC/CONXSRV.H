/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  CONXSRV.h
 *
 ***********************************************************************/

#ifndef _CONXSRV_H_
#define _CONXSRV_H_

#include "..\res\CONXSRV.str"
#include "res.h"
#include "cmd.h"
#include "dlgs.h"
#include "comm.h"
#include "tty.h"


/**************************************************************************
 * The following #defines specify the default values
 * CONXSRV uses for timing while connected.  These values
 * can be changed by modifying entries of the [Comm Settings] section
 * of the CONXTION.INI file.
 *
 * NOTE!  In early versions of CONXSRV these were stored in [ConXSrv]
 *************************************************************************/

/*
; MaxBlockSize=<count of bytes>
;
; The MaxBlockSize value specifies the maximum number bytes CONXSRV will process 
; during a given comm notification.  Lowering this value will result in better
; multi-tasking; increasing it might help prevent dropping characters at
; at high speeds.
;
; The default value is 8192 bytes.
;
;MaxBlockSize=8192
*/
#define MAXBLOCK        8192            // can be changed via INI setting

/*
;
; RXQueueSize=<count of kilobytes>
;
; Specifies the size of the RX queue that CONXSRV will request from the comm driver.
; Increasing this value might help prevent dropping of characters.
; The default value is 32 Kilobytes.  Max is 32.
;
;RXQueueSize=32
*/
#define RXQUEUE         32     // in K, can be changed via INI setting

/*
;
; TXQueueSize=<count of kilobytes>
;
; Specifies the size of the TX queue that CONXSRV will request from
; the comm driver.
; The default value is 8 Kbytes. Max is 32.
;
;TXQueueSize=8
*/
#define TXQUEUE         8     // in K, can be changed via INI setting


/* UseCN_RECEIVE = 0|1
 *
 * HACK ALERT!!! There is a bug in the Windows 3.10 COMM driver when
 * CN_RECEIVE notifications are used.
 *
 * Essentially what happens is at high speeds, with a lot of data
 * coming in (i.e. a 9600 baud ZMODEM transfer) it is possible
 * for the COMM.DRV to royally fuck up.  In fact, it fucks up so
 * badly that the machine typically reboots!  This by itself is not
 * too bad (ha!).  Unfortunately, while we're downloading we
 * have a file open which we're writing to.  SmartDrv 4.0 does write
 * caching.  Thus the machine reboots with data still in
 * SmartDrv's write cache...and that's an easy way to trash someone's
 * disk in an ugly way (I know it did it to mine several times).
 *
 * CONXSRV will work much better with the following #define enabled.
 * But until we can be sure that the user has the fixed
 * 3.10 COMM.DRV on his system we have to disable it.
 *
 * If USE_CN_RECEIVE is #defined as 1 the code here will use CN_RECEIVE
 * notifications.  Otherwise it will simply use CN_EVENT notifications
 * with the EV_RXCHAR event.
 *
 * The value of this define is the default value for the fUseCN_RECEIVE
 * flag.  fUseCN_RECEIVE can be changed through the CONXION.INI flag
 * "UseCN_RECEIVE".  UseCN_RECEIVE=1 means use CN_RECEIVE notifications,
 * UseCN_RECEIVE=0 means don't
 *
 * IMPORTANT (cek 7/17/92):  After this hack was implemented, it was
 * realized that NOT using CN_RECEIVES actually improves performance
 * and response.  So, by default USE_CN_RECEIVE is FALSE.
 */
#define USE_CN_RECEIVE  FALSE


/* NoYield = 1|0
 *
 * On slow hardware (25MHz 386 and below) at high baud rates
 * (2400 baud and above), setting the entry
 * 'NoYield' to 1 may improve zmodem performance
 * and will eliminate the chance of comm overruns.  The downfall is that
 * we never yield, thus no indication is given that things are working and
 * the user cannot multitask.
 *
 * By default this setting is 0.  This setting has no effect when
 * CN_RECEIVE notifications are used (UseCN_RECEIVE = 1).
 */
#define NO_YIELD_ON_SLOW_MACHINES  FALSE


/* ConnectDriverSpeed=<milliseconds>
 *
 * Specifies how often the connect state machine will cycle.  The
 * default is 250 (4 a second).
 *
 * ConnectDriverSpeed=250
 */
#define CONNECT_DRIVE_MSEC             250

/* PostAquirePause=<milliseconds>
 *
 * Specifies how long to pause after aquiring the communications port,
 * but before attempting to initailize the modem.
 *
 * Default is 500 (1/2 a second).
 *
 * PostAquirePause=500
 */
#define POSTAQUIREPAUSE_MSEC           500

/* ModemDialPause=<milliseconds>
 *
 * Specifies the amount to pause for each '~' found in the modem
 * dial string (or init strings). Default is 1/2 a second (500 ms).
 *
 * ModemDialPause=500
 */
#define MODEMDIALPAUSE_MSEC            500

/* ConnectWaitDialTimeout=<seconds>
 *
 * Specifies the number of seconds to wait for the modem to return
 * a result code after a dial.
 * Default is 75 seconds.
 *
 * ConnectWaitDialTimeout=75
 */
#define CONNECT_WAITDIAL_SEC           75

/* OnlineDriverSpeed=<milliseconds>
 *  
 * Specifies how often the online state machine will cycle.  The 
 * default is 125 (8 a second).
 * 
 * OnlineDriverSpeed=125
 */
#define ONLINE_DRIVE_MSEC              (1000/8)   // 1/8 second

/* AckTimout=<seconds>
 *
 * Specifies the number of seconds CONXSRV will wait for an ACK
 * before timing out.  Default is 45 seconds.
 *
 * AckTimout=45
 */
#define ONLINE_WAITACK_SEC             45     // 45 seconds

/* SayHiAckTimout=<seconds>
 *
 * Specifies the number of seconds CONXSRV will wait for an ACK in
 * response to a MSG_SAY_HI message before timing out.  Default is 60 seconds.
 *
 * SayHiAckTimout=60
 */
#define ONLINE_SAYHIWAITACK_SEC        60     // 60 seconds

/* ReceiveTimout=<seconds>
 * 
 * Specifies the number of seconds CONXSRV will wait to receive
 * a buffer before timing out.  The Default is 45 seconds.
 *
 * ReceiveTimeout=120
 */
#define ONLINE_WAITRECEIVE_SEC         120     // 120 seconds

/* Error recovery
 */

/* ReSyncAckTimeOut=<seconds>
 * 
 * Specifies the number of seconds CONXSRV will wait for an ACK
 * in response to a re-sync message.  The default is 2 seconds
 * 
 * ReSyncAckTimeOut=2
 */
#define ONLINE_RESYNC_WAITACK_SEC      2

/* MaxReSyncs=<number>
 * 
 * Specifies the number of times CONXSRV will attempt to resync
 * with the bbs after an error has been detected.  The default
 * is 60
 * 
 * MaxReSyncs=60
 */
#define DEFAULT_RESYNCATTEMPTS         60

/* MaxReSends=<number>
 * 
 * Specifies the number of times CONXSRV will resend a message after
 * an error has occured.  The default is 3
 * 
 * MaxReSends=3
 */
#define DEFAULT_RESENDATTEMPTS         3

/**************************************************************************
 *************************************************************************/

#define CBWNDEXTRA_TTY   (sizeof( LPVOID ))
#define GWW_TTYINFO         0
#define SZ_TTYCLASSNAME  "CONXSRV.TTY"

/*
 * Macros
 */
#define GALLOC(n) GlobalAllocPtr(GHND, n)
#define GFREE(lp) GlobalFreePtr(lp)

/*
 * Global Variables, defined in CONXSRV.c
 */
extern HWND        hwndApp ;
extern HWND        hwndTTY ;
extern HWND        hwndStat ;
extern HWND        hdlgActive ;

extern BOOL        fInternal ;

extern int         iCommDevice ;

extern BOOL  fUserAbort ;

extern HINSTANCE   hinstApp ;

extern char        szVerNum[] ;

extern LPSTR       rglpsz[] ;

extern HFONT       hfontCur ;
extern char        szFaceName[LF_FACESIZE] ;
extern char        szFaceStyle[LF_FACESIZE] ;
extern UINT        wFaceSize ;
extern HFONT       hfontSmall ;

extern BOOL           fStatLine ;
extern COMMSETTINGS   CommSettings ;
extern CUSTINFO       CustInfo ;
extern HWND           hwndClientApp ;

LRESULT FAR PASCAL fnMainWnd( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

VOID WINAPI SetWaitCursor( VOID ) ;
VOID WINAPI SetNormalCursor( VOID ) ;
int FAR PASCAL MessageLoop( HACCEL haccl ) ;


#endif

/************************************************************************
 * End of File: CONXSRV.h
 ***********************************************************************/

