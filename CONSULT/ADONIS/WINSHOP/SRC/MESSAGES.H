#include "stdcomm.h"
#define MSG_RECEIVE_FILE        (MSGTYPE)0x1
#define MSG_LATEST_VERSION      (MSGTYPE)0x2
#define MSG_LATEST_EXE          (MSGTYPE)0x3

/* leave 4 open not to be confused with EOT */
#define MSG_QUERY_CUR_FILESIZE  (MSGTYPE)0x5
/* leave 6 open not to be confused with ACK */

#define MSG_QUERY_DEST_FILENAME (MSGTYPE)0x7
#define MSG_SAY_HI              (MSGTYPE)0x8
#define MSG_SET_CUR_FILENAME    (MSGTYPE)0x9
#define MSG_QUERY_UNCOMPRESSED_FILESIZE \
                                (MSGTYPE)0xA
#define MSG_HANGING_UP          (MSGTYPE)0xB

#define MSG_SET_CUR_LIB_ORDER   (MSGTYPE)0xC
#define MSG_QUERY_LIB_ORDER_OK  (MSGTYPE)0xD  /* checks credit */
#define MSG_COMMIT_LIB_ORDER    (MSGTYPE)0xE

#define MSG_SET_CUR_CLIP_DLD    (MSGTYPE)0xF
#define MSG_QUERY_CLIP_DLD_OK   (MSGTYPE)0x10  /* checks credit */
#define MSG_COMMIT_CLIP_DLD     (MSGTYPE)0x11

#define MSG_SET_CUST_INFO       (MSGTYPE)0x12
#define MSG_SET_USER_ID     (MSGTYPE)0x13
#define MSG_GET_NEW_USER_ID     (MSGTYPE)0x14
/* leave 0x15 open not to be confused with NAK */
#define MSG_SET_PASSWD          (MSGTYPE)0x16
#define MSG_QUERY_PASSWD        (MSGTYPE)0x17
/* leave 0x18 open not to be confused with CTRLX */
#define MSG_QUERY_NEWUSER_OK    (MSGTYPE)0x19  /* checks credit */
#define MSG_COMMIT_NEWUSER      (MSGTYPE)0x1A
#define MSG_SET_NEW_USER_ORDER  (MSGTYPE)0x1B
#define MSG_QUERY_NEW_USERID_OK (MSGTYPE)0x1C
#define MSG_BEGIN_SESSION       (MSGTYPE)0x1D
#define MSG_COMMIT_PASSWD       (MSGTYPE)0x1E
#define MSG_SEND_WINVER         (MSGTYPE)0x1F
#define MSG_COMMIT_CUST_INFO    (MSGTYPE)0x20
#define MSG_QUERY_SHIPPING_COSTS (MSGTYPE)0x21
#define MSG_SEND_EXETYPE         (MSGTYPE)0x22
#define MSG_QUERY_USERID_OK      (MSGTYPE)0x23
#define MSG_QUERY_SUBSCRIPTION_PRICE (MSGTYPE)0x24
#define MSG_DOWNLOAD_EXE         (MSGTYPE)0x25
#define MSG_QUERY_INSTOCK        (MSGTYPE)0x26
#define MSG_COMMIT_RENEWAL       (MSGTYPE)0x27
#define MSG_QUERY_RENEWAL_PRICE  (MSGTYPE)0x28
#define MSG_QUERY_RENEWAL_OK     (MSGTYPE)0x29
#define MSG_QUERY_SUBTYPE        (MSGTYPE)0x2A
#define MSG_QUERY_SHIP_RENEWAL   (MSGTYPE)0x2B

#if 0
#define MSG_QUERY_HIGH_EXT	 (MSGTYPE)0x2C
#endif

#define MSG_QUERY_DATA_LRC	 (MSGTYPE)0x2C
