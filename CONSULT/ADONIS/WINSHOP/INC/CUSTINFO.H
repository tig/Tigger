/*************************************************************\
 ** FILE:  
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **              Customer infomation structures
 **
 ** HISTORY:   
 **   ??                           (created)
 **   Lauren Bricker  May 11, 1990 (modified)
 **
\*************************************************************/

#ifndef CUSTINFOH
#define CUSTINFOH

#include "ws.h"

#define     MAXLEN_AREA         3
#define     MAXLEN_PHONE        8
#define     MAXLEN_EXT          4
#define     MAXLEN_CARDNUM     19
#define     MAXLEN_EXPIRE       2
#define     MAXLEN_NAME        30
#define     MAXLEN_ADDRESS     30
#define     MAXLEN_CITY        25
#define     MAXLEN_STATE        2
#define     MAXLEN_COUNTRY     20
#define     MAXLEN_POSTAL      10
#define     MAXLEN_CCNUMBER    16
#define     MAXLEN_TITLE       30
#define     MAXLEN_PROVINCE    25
#define     MAXLEN_EXPDATE      4
#define     MAXLEN_MODEMINITSTR    80
#define     MAXLEN_SUFFIX	   25
#define     MAXLEN_PREFIX      25
#define     MAXLEN_SEARCHPARAM   8


/* If any cards are added, the number of cards must be increased */

#define     NUMBER_OF_CARDS     5
#define     MASTERCARD          0
#define     VISA                1
#define     DISCOVER            2
#define     AMERICAN_EXPRESS    3
#define     CORPORATE_CARD      4

#pragma pack(4) // for bbs
typedef struct {
    char    area[MAXLEN_AREA + 1];
    char    number[MAXLEN_PHONE + 1];
    char    ext[MAXLEN_EXT + 1];
} PHONE_STRUCT;

typedef PHONE_STRUCT * PTR_PHONE;

typedef struct {
    BOOL     valid;
    char     name[MAXLEN_NAME + 1];
    char     number[MAXLEN_CARDNUM + 1];
    unsigned exp_month;
    unsigned exp_year;
} CREDIT_CARD_STRUCT;

typedef CREDIT_CARD_STRUCT * PTR_CREDIT_CARD;

/* DON'T CHANGE THIS, IT MUST CORRESPOND WITH BBS DEFINITION!!! */
typedef struct {
    char         name[MAXLEN_NAME + 1];
    char         title[MAXLEN_TITLE + 1];
    char         company[MAXLEN_NAME + 1];
    char         street[MAXLEN_ADDRESS + 1];
    char         street2[MAXLEN_ADDRESS + 1];
    char         city[MAXLEN_CITY + 1];
    char         state[MAXLEN_STATE + 1];
    char         province[MAXLEN_PROVINCE + 1];
    short        whichCountry;
    char         country[MAXLEN_COUNTRY + 1];
    char         postal_code[MAXLEN_POSTAL + 1];
    PHONE_STRUCT phone;
} ADDRESS_STRUCT;

typedef ADDRESS_STRUCT * PTR_ADDRESS;
typedef ADDRESS_STRUCT ADDRESS;

typedef struct  {
    char                userid[PW_SIZE + 1];
    char                password[PW_SIZE + 1];
    char                phonePrefix[MAXLEN_PREFIX + 1];/* Communications Info*/
    char                phoneSuffix[MAXLEN_SUFFIX + 1];/* Communications Info*/
    WORD                comm_port;                    /* Communications Info */
    WORD                baud;                         /* Communications Info */
    WORD                dial_type;                    /* Communications Info */
    WORD                whichCard;                    /* Billing Info/Select */
    WORD                whichShip;                    /* Shipping Info       */
    WORD                updateDB;                     /* Update Database     */
    WORD                updateOR;                     /* Update override     */
    WORD                cacheSize;                    /* Library Management  */
    BOOL                useAltCache;                  /* Menu Item           */
    char                altcache[_MAX_PATH];          /* LibDriveDlgProc     */
    char                downloadDir[_MAX_PATH];       /* Download to Dialog  */
    DWORD               searchParam;                  /* User's default search restrictions*/
    ADDRESS_STRUCT      customer;                     /* Customer Info       */
    ADDRESS_STRUCT      mailing;                      /* Shipping Info       */
    CREDIT_CARD_STRUCT  card[NUMBER_OF_CARDS];        /* Billing Info        */
    char                modemInitString[MAXLEN_MODEMINITSTR];
    char                userModemString[MAXLEN_MODEMINITSTR];
} CUSTINFO_STRUCT;
#pragma pack()

typedef CUSTINFO_STRUCT * PTR_CUSTINFO;


/* Strings for use in win.ini */

/* Temporary kludge */
#define PS_PASSWORD "password"

#define PS_USERID        "userID"
#define PS_PREFIX        "prefix"
#define PS_SUFFIX        "suffix"
#define PS_COMMPORT      "commPort"
#define PS_DIALTYPE      "dialType"
#define PS_BAUD          "baud"
#define PS_WHICHCARD     "which"
#define PS_SHIPPING      "shipping"
#define PS_CACHESIZE     "cacheSize"
#define PS_DOWNLOADDIR   "downloadDir"
#define PS_CNAME         "custName"
#define PS_CCOMPANY      "custCompany"
#define PS_CADDRESS      "custAddress"
#define PS_CCITY         "custCity"
#define PS_CSTATE        "custState"
#define PS_CWHICHCOUNTRY "custWhichCountry"
#define PS_CCOUNTRY      "custCountry"
#define PS_CZIP          "custPostal"
#define PS_CAREA         "custArea"
#define PS_CNUMBER       "custNumber"
#define PS_CEXT          "custExt"
#define PS_MNAME         "mailName"
#define PS_MCOMPANY      "mailCompany"
#define PS_MADDRESS      "mailAddress"
#define PS_MCITY         "mailCity"
#define PS_MSTATE        "mailState"
#define PS_MWHICHCOUNTRY "mailWhichCountry"
#define PS_MCOUNTRY      "mailCountry"
#define PS_MZIP          "mailPostal"
#define PS_MAREA         "mailArea"
#define PS_MNUMBER       "mailNumber"
#define PS_MEXT          "mailExt"
#define PS_USEALTCACHE   "useAltCache"
#define PS_ALTCACHE      "altcache"
#define PS_MODEM_INIT_STRING "modemInitStr"
#define PS_USER_INIT_STRING "userModemStr"
#define PS_SEARCHPARAM "searchKey"

#define WRITE_WHICHCARD 0x0001
#define WRITE_CUSTCOMM  0x0002

#define WRITE_CUSTDB    0x0004
#define WRITE_CUSTCACHE 0x0008
#define WRITE_CUSTINFO  0x0010
#define WRITE_MAILINFO  0x0020
#define WRITE_CARDINFO  0x0040
#define WRITE_USECACHE  0x0080
#define WRITE_DOWNDIR   0x0100
#define WRITE_USERID    0x0200
#define WRITE_SEARCHPARAM    0x0400
#define WRITE_CUSTALL   (WRITE_WHICHCARD  | WRITE_CUSTCOMM  | WRITE_DOWNDIR | \
                         WRITE_CUSTDB   | WRITE_CUSTCACHE | WRITE_CUSTINFO | \
                         WRITE_MAILINFO | WRITE_CARDINFO | WRITE_USECACHE | \
                         WRITE_USERID | WRITE_HIGHEXT)


/*-------------------------------------------------------------------------*/
/*                        G L O B A L    D A T A                           */
/*-------------------------------------------------------------------------*/

extern CUSTINFO_STRUCT custinfo;


/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
int BBSChangeCustInfo(void);
BOOL FAR PASCAL ResetCustInfo (BOOL bAll);
BOOL FAR PASCAL ReadCustInfo (void);
BOOL FAR PASCAL WriteCustInfo (int flags);

BOOL FAR PASCAL EncryptCards (void);
BOOL FAR PASCAL UnencryptCards (void);
BOOL FAR PASCAL UnencryptCardNumber (WORD whichCard, PTR_CREDIT_CARD aCard);

#endif
