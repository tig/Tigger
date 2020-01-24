/************************************************************************
 *
 *     Copyright (c) 1992 Adonis Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  messages.h
 *
 *    This include file is common to SHOPSRV, FONTSHOP, CLIPSHOP, and
 *    the BBS.  Changes made here should also be made in all other
 *    projects.
 *
 ***********************************************************************/
#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#ifndef BBS

typedef BYTE MSGTYPE;

/*
 * The GENERICMSG structure contains all information needed to
 * send a message to the bbs.  The structure is used by
 * SHOPSRV.EXE as part of the DDE server code.  ShopSrv has a
 * global GENERICMSG structure into which values can be
 * DDEPoked by a client.  When the Client DDEExecutes the
 * SendBBSMsg command shopsrv uses the data in the structure
 * to initate the message transaction with the bbs.
 *
 * A client can use the GENERICMSG structure to prepare
 * it's DDE conversation with shopsrv regarding a message.
 */
typedef struct tagGENERICMSG
{
   MSGTYPE  SendMsg ;         // The message being sent
   
   LPBYTE   lpSend ;          // pointer to GMEM_DDESHARED memory
   DWORD    cbSend ;          // number of bytes pointed to by lpSend

   LPBYTE   lpReceive ;       // pointer to GMEM_DDESHARED memory
   DWORD    cbReceive ;       // number of bytes pointed to by lpReceive

   WORD     wFlags  ;         // Message characteristics.  See defines below.

} GENERICMSG, *PGENERICMSG, FAR *LPGENERICMSG ;

/*
 * The LOBYTE of the wFlags member of a GENERICMSG structure specifies
 * flags that affect the way the message is handled by shopsrv.
 *
 * ShopSrv allows a client to DDEPoke the SendBBSMsgFlags item with
 * a WORD specifying the flags and message class (given below).
 *
 * It can be any combination of the following.
 */
#define MSGFLAG_SENDCRC                0x0001
#define MSGFLAG_SENDPAD                0x0002
#define MSGFLAG_RECEIVECRC             0x0004
#define MSGFLAG_RECEIVEPAD             0x0008

/*
 * The HIBYTE of wFlags can be any one of the following values.
 */
#define MSGCLASS_GOIDLE                0x0100
#define MSGCLASS_WAITACK               0x0200
#define MSGCLASS_WAITRECEIVE           0x0300
#define MSGCLASS_SENDDATA_WAITACK      0x0400
#define MSGCLASS_SENDDATA_WAITRECEIVE  0x0500
#define MSGCLASS_CONXPRO_RECEIVE        0x0600
#define MSGCLASS_CONXPRO_SEND           0x0900

#endif

/*
 * Defines that are used by messages
 */
/* For MSG_SEND_EXETYPE
 */
#define EXE_BETA              0x0000      // ClipArt
#define EXE_REGULAR           0x0001
#define EXE_INTERNAL          0x0002

#define EXE_FONTSHOP_BETA     0x0010      // FontShopper
#define EXE_FONTSHOP_REGULAR  0x0011
#define EXE_FONTSHOP_INTERNAL 0x0012


/* File message defines
 */
#define STD_FILENAME_SIZE     12


/***********************************************************************
 * Messages that are used by ClipArt shopper *AND* FontShopper.
 *
 * The MSG_SEND_EXETYPE tells the bbs that we are either ClipArt shopper
 * or we are FontShopper.  The messages below may behave differently
 * depending on what was sent with MSG_SEND_EXETYPE.  For example
 * MSG_RECEIVE_FILE uses XMODEM for ClipArt and CONXPRO for Font.
 *
 ***********************************************************************/

#define MSG_RECEIVE_FILE                        (MSGTYPE)0x1   
#define MSG_LATEST_VERSION                      (MSGTYPE)0x2   /* WS Specific */
#define MSG_LATEST_EXE                          (MSGTYPE)0x3   /* WS Specific */

/* leave 4 open not to be confused with EOT */

#define MSG_QUERY_CUR_FILESIZE                  (MSGTYPE)0x5   

/* leave 6 open not to be confused with ACK */

#define MSG_QUERY_DEST_FILENAME                 (MSGTYPE)0x7   /* WS Specific.  FontShopper has dest filenames stored locally */
#define MSG_SAY_HI                              (MSGTYPE)0x8   /* First message sent, also used for re-synching */
#define MSG_SET_CUR_FILENAME                    (MSGTYPE)0x9   
#define MSG_QUERY_UNCOMPRESSED_FILESIZE         (MSGTYPE)0xA   /* !!! BIG QUESTION !!! */
#define MSG_HANGING_UP                          (MSGTYPE)0xB   /* Tell host we're hanging up */

#define MSG_SET_CUR_LIB_ORDER                   (MSGTYPE)0xC   /* WS Specific */
#define MSG_QUERY_LIB_ORDER_OK                  (MSGTYPE)0xD   /* WS SPecific. Checks credit */
#define MSG_COMMIT_LIB_ORDER                    (MSGTYPE)0xE

#define MSG_SET_CUR_CLIP_DLD                    (MSGTYPE)0xF   /* WS SPecific */
#define MSG_QUERY_CLIP_DLD_OK                   (MSGTYPE)0x10  /* WS Specific. Checks credit */
#define MSG_COMMIT_CLIP_DLD                     (MSGTYPE)0x11  /* WS Specific. */

#define MSG_SET_CUST_INFO                       (MSGTYPE)0x12
#define MSG_SET_USER_ID                         (MSGTYPE)0x13
#define MSG_GET_NEW_USER_ID                     (MSGTYPE)0x14

/* leave 0x15 open not to be confused with NAK */

#define MSG_SET_PASSWD                          (MSGTYPE)0x16  /* WS Specific */
#define MSG_QUERY_PASSWD                        (MSGTYPE)0x17  /* WS Specific */

/* leave 0x18 open not to be confused with CTRLX */

#define MSG_QUERY_NEWUSER_OK                    (MSGTYPE)0x19  /* checks credit */
#define MSG_COMMIT_NEWUSER                      (MSGTYPE)0x1A
#define MSG_SET_NEW_USER_ORDER                  (MSGTYPE)0x1B
#define MSG_QUERY_NEW_USERID_OK                 (MSGTYPE)0x1C
#define MSG_BEGIN_SESSION                       (MSGTYPE)0x1D
#define MSG_COMMIT_PASSWD                       (MSGTYPE)0x1E
#define MSG_SEND_WINVER                         (MSGTYPE)0x1F
#define MSG_COMMIT_CUST_INFO                    (MSGTYPE)0x20
#define MSG_QUERY_SHIPPING_COSTS                (MSGTYPE)0x21
#define MSG_SEND_EXETYPE                        (MSGTYPE)0x22
#define MSG_QUERY_USERID_OK                     (MSGTYPE)0x23
#define MSG_QUERY_SUBSCRIPTION_PRICE            (MSGTYPE)0x24
#define MSG_DOWNLOAD_EXE                        (MSGTYPE)0x25  /* re-defined for FS */
#define MSG_QUERY_INSTOCK                       (MSGTYPE)0x26
#define MSG_COMMIT_RENEWAL                      (MSGTYPE)0x27
#define MSG_QUERY_RENEWAL_PRICE                 (MSGTYPE)0x28
#define MSG_QUERY_RENEWAL_OK                    (MSGTYPE)0x29
#define MSG_QUERY_SUBTYPE                       (MSGTYPE)0x2A
#define MSG_QUERY_SHIP_RENEWAL                  (MSGTYPE)0x2B

#if 0
#define MSG_QUERY_HIGH_EXT                           (MSGTYPE)0x2C
#endif

#define MSG_QUERY_DATA_LRC                           (MSGTYPE)0x2C

#define MSG_NEW_GET_NEW_USER_ID                 (MSGTYPE)0x2D
#define MSG_QUERY_SUBSCR_PRICES                 (MSGTYPE)0x2E
#define MSG_COMMIT_SUBSCRIPTION                 (MSGTYPE)0x2F

/****************************************************************
 * New messages defined for FontShopper 1.0
 ****************************************************************/
#define MSG_QUERY_EXEVERSIONS                   (MSGTYPE)0x30
#define MSG_QUERY_ORDERID                       (MSGTYPE)0x31
#define MSG_SET_ORDER                           (MSGTYPE)0x32
#define MSG_DOWNLOAD_FCDBLST                    (MSGTYPE)0x33
#define MSG_DOWNLOAD_FCDBFILE                   (MSGTYPE)0x34
#define MSG_GET_NEW_LIMITED_DEMO_USER_ID        (MSGTYPE)0x35
#define MSG_SET_MEDIA_PREF                      (MSGTYPE)0x36
#endif

/************************************************************************
 * End of File: messages.h
 ***********************************************************************/

