/************************************************************************
 *
 *      Module:  struct.h
 *
 *     Remarks:  Contains all structures that are shared between
 *               the bbs/shopsrv/CONXLIB/and the clients.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/
#ifndef _STRUCT_H_
#define _STRUCT_H_

#ifndef RC_INVOKED

#define MAX_COMMDEVICE        15
#define MAX_MODEMTYPE         64
#define MAX_PHONENUMBER       31
#define MAX_SPECIALPREFIX     15
#define MAX_BAUDRATE          6        // "144000"

typedef struct tagCOMMSETTINGS
{
    char             szCommDevice[MAX_COMMDEVICE+1] ;
    char             szModemType[MAX_MODEMTYPE+1] ;
    DWORD            dwBaudRate ;
    UINT             uiDialType ;
    UINT             uiLocationFlags ;
    char             szPrefix[MAX_SPECIALPREFIX+1] ;
    char             szSuffix[MAX_SPECIALPREFIX+1] ;
    
} COMMSETTINGS, *PCOMMSETTINGS, FAR *LPCOMMSETTINGS ;

#define DIALTYPE_TONE      0
#define DIALTYPE_PULSE     1


#define LOCATION_OTHER     0
#define LOCATION_206       1

#define     MAX_NAME             30
#define     MAX_CARDNUM          19
#define     MAX_EXPDATE          4

#ifndef RC_INVOKED
#pragma pack(4)
typedef struct tagCREDITCARD
{
    BOOL           fValid;
    char           szNameOnCard[MAX_NAME + 1];
    char           szNumber[MAX_CARDNUM + 1];
    WORD           wExpMonth;
    WORD           wExpYear;
} CREDITCARD, *PCREDITCARD, FAR *LPCREDITCARD ;
#pragma pack()
#endif


#define     MAX_AREACODE      3
#define     MAX_PHONE         8
#define     MAX_EXT           4

#ifndef RC_INVOKED
#pragma pack(4)
typedef struct tagPHONE
{
    char          szAreaCode[MAX_AREACODE+1] ;
    char          szNumber[MAX_PHONE+1] ;
    char          szExt[MAX_EXT+1] ;
} PHONE, *PPHONE, FAR *LPPHONE ;
#pragma pack()
#endif



#define     MAX_TITLE         30
#define     MAX_ADDRESS       30
#define     MAX_STREET        MAX_ADDRESS   
#define     MAX_CITY          25
#define     MAX_STATE         2
#define     MAX_PROVINCE      25
#define     MAX_COUNTRY       20
#define     MAX_POSTAL        10
#define     MAX_ZIP           MAX_POSTAL

#define     COUNTRY_USA       0
#define     COUNTRY_CANADA    1
#define     COUNTRY_OTHER     2


#ifndef RC_INVOKED
#pragma pack(4)
typedef struct tagADDRESS
{
    char         szName[MAX_NAME + 1];
    char         szTitle[MAX_TITLE + 1];
    char         szCompany[MAX_NAME + 1];
    char         szStreet1[MAX_ADDRESS + 1];
    char         szStreet2[MAX_ADDRESS + 1];
    char         szCity[MAX_CITY + 1];
    char         szState[MAX_STATE + 1];
    char         szProvince[MAX_PROVINCE + 1];
    WORD         wCountry ;
    char         szCountry[MAX_COUNTRY + 1];
    char         szPostalCode[MAX_POSTAL + 1];
    PHONE        Phone;
} ADDRESS, *PADDRESS, FAR *LPADDRESS ;
#pragma pack()
#endif

#define     NUMBER_OF_CARDS     5
#define     MASTERCARD          0
#define     VISA                1
#define     DISCOVER            2
#define     AMERICAN_EXPRESS    3
#define     CORPORATE_CARD      4

#define     ADDRESS_CUSTOMER    0
#define     ADDRESS_SHIP        1

#define     SHIPMETHOD_1TO2DAY  0
#define     SHIPMETHOD_2TO3DAY  1
#define     SHIPMETHOD_UPS      2

#define     MAX_PASSWORD        8      // PW_SIZE

typedef struct tagCUSTINFO
{
    char         szUserID[MAX_PASSWORD+1] ;
    char         szPassword[MAX_PASSWORD+1] ;
    UINT         uiShipMethod ;
    UINT         uiWhichCard ;
    UINT         uiWhichAddress ;
    UINT         uiMediaType ;
    CREDITCARD   CreditCard ;
    ADDRESS      CustAddress ;
    ADDRESS      ShipAddress ;

    BOOL         fValidPassword ;
} CUSTINFO, *PCUSTINFO, FAR *LPCUSTINFO ;


/*********************************************************************
 *  Library MEDIA types.  The values here will correspond to indices
 *  into MediaStrings[]. (6.19.90) D. Kent.
 *********************************************************************/
typedef enum { FLOPPY360, FLOPPY12, DISK720, DISK14 } MEDIATYPE ;

/* for prodType */
#define CLIP    0
#define LIB     1
#define WS      2
#define FONT    3

/* userid statuses */
/* DON'T CHANGE THIS, IT MUST CORRESPOND WITH BBS DEFINITION!!! */
#define WS_NOEXIST                  0 // doesn't exist
#define WS_AUTH                     1 // been paid for
#define WS_ACTIVE                   2 // already used
#define WS_EXPIRED                  3 // done gone
#define WS_AVAIL                    5 // on shelf, waiting to be used

/* Subtypes of various flavors of Window Shopper - to help determin price for */
/* on-line orders of Shopper */
/* DON'T CHANGE THIS, IT MUST CORRESPOND WITH BBS DEFINITION!!! */
#define WS_SUBTYPE_FREE        1     // User has free subscription
#define WS_SUBTYPE_NORMAL      2     // User has normal subscription
#define WS_SUBTYPE_DEMO        3     // User has demo subscription

/* LibID's of various flavors of Window Shopper - for on-line orders of Shopper */
/* DON'T CHANGE THIS, IT MUST CORRESPOND WITH BBS DEFINITION!!! */
#define WS_LIBID_NEW        1     // New ClipArt Shopper subscription
#define WS_LIBID_DEMO       2     // New ClipArt Shopper demo subscription
#define WS_LIBID_RENEWAL   10     // Renewal of ClipArt regular subscription
#define WS_LIBID_UPGRADE  100     // Upgrade from CS demo to regular subscription

#define WS_VEND_ID          1     // Window Shopper vendor ID  - Adonis!!

/* DON'T CHANGE THIS, IT MUST CORRESPOND WITH BBS DEFINITION!!! */
/* modified 6/18/91, removed obsolete fields - beng */

#define MAX_STR_LEN         120
#define MAX_CCNUMBER        16

#pragma pack(4) // for bbs
typedef struct tagORDER
{
    /* always have flags field first.  The bbs will use
       this field to indicate to the acct subsystem 
       whether we've gotten their money, whether its a back order,
       etc.  Some of this (like no money) could be scrapped in favor
       of using the info in the ccReq structure.  */
    long flags;

    /* user stuff, set by server */
    long userID;
    long sessionID;

    /********************************/
    /* product identification stuff */
    char prodType; /* WS, LIB, CLIP or FONT.... */

    long clipID;   

    char Version[MAX_STR_LEN+1];
    long libID;

    /* if library, need this with productID for full 
       identification. */

    char mediaType;

    long pubID;
    long vendorID;

    /********************************/
    /* shipping stuff */
    ADDRESS ShipAddress;
    char CourierType;
  
    /* order, credit card stuff */
    long orderID; // for bbs use only
    char ccName[MAX_STR_LEN+1];    
    char ccNum[MAX_CCNUMBER+1];
    char ccExpDate[MAX_EXPDATE+1];
    short ccType;
    
    /* price info */
    long  Quantity;
    long  priceTotal; /*  * 100 (should be called 'total'- includes tax...) */
    long  priceEach;  /*  * 100   */
    long  taxamount;
    long  shipping;
    long  surcharge;

} ORDER, *PORDER, FAR *LPORDER ;

/* FontShopper's version of the ORDER struct.  Note that the size of
 * this structure must be EXACTLY THE SAME as the real ORDER struct!!!
 */

#define FSHOP        10
#define FONT_STYLE   11
#define FONT_FACE    12
#define FONT_COLLECTION    13

#define FS_LIBID_NEW          21     // New FontCon subscription
#define FS_LIBID_LIMITED_DEMO 26     // New FontCon demo subscription
#define FS_LIBID_RENEWAL      30     // Renewal of FC regular subscription
#define FS_LIBID_UPGRADE      120    // Does not apply

typedef struct tagFSORDER
{
    DWORD   dwFlags;

    /* user stuff, set by server */
    DWORD   dwUserID;
    DWORD   dwSessionID;

    BYTE    bProdType; // FSHOP, FONT_COLL, FONT_FONT. FONT_STYLE

    DWORD   dwID;      // may be faceID, styleID, or collectionID

    char    szVersion[MAX_STR_LEN+1];  // not used

    DWORD   dwLibID ; 

    BYTE    bMediaType;                // used only with Collection

    DWORD   dwPubID;                   // identifies publisher
    DWORD   dwVendorID;                // always adonis

    ADDRESS ShipAddress;
    BYTE    bCourierType;
  
    /* order, credit card stuff */
    DWORD   dwOrderID;                 // for bbs use only
    char    szCCName[MAX_STR_LEN+1];    
    char    szCCNum[MAX_CCNUMBER+1];
    char    szCCExpDate[MAX_EXPDATE+1];
    WORD    wCCType;
    
    /* price info */
    DWORD   dwQuantity;
    DWORD   dwTotal;      /*  * 100 (should be called 'total'- includes tax...) */
    DWORD   dwPriceEach;  /*  * 100   */
    DWORD   dwTaxAmount;
    DWORD   dwShipping;
    DWORD   dwSurcharge;

} FSORDER, *PFSORDER, FAR *LPFSORDER ;

typedef struct {
    WORD   valid;
    DWORD   cost;       // in cents
} SHIPPING_COSTS_STRUCT;
#pragma pack()

#define MAX_NUM_SERVICES    3
//extern SHIPPING_COSTS_STRUCT ShippingCosts[MAX_NUM_SERVICES];

#endif //RC_INVOKED
#endif

/************************************************************************
 * End of File: struct.h
 ***********************************************************************/

