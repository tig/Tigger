#ifndef ORDERH
#define ORDERH

/** 
    Library MEDIA types.  The values here will correspond to indices
    into MediaStrings[]. (6.19.90) D. Kent.
**/
typedef enum { FLOPPY360, FLOPPY12, DISK720, DISK14 } MEDIATYPE;
extern char *MediaStrings[];
/* MType is a variable of type MEDIATTYPE: */
#define szMEDIA(MType) (MediaStrings[(int)MType])

#include "custinfo.h"

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
#define WS_LIBID_NEW        1     // New Window Shopper subscription
#define WS_LIBID_DEMO       2     // New Window Shopper demo subscription
#define WS_LIBID_RENEWAL   10     // Renewal of regular subscription
#define WS_LIBID_UPGRADE  100     // Upgrade from demo to regular subscription

#define WS_VEND_ID          1     // Window Shopper vendor ID  - Adonis!!

/* DON'T CHANGE THIS, IT MUST CORRESPOND WITH BBS DEFINITION!!! */
/* modified 6/18/91, removed obsolete fields - beng */
#pragma pack(4) // for bbs
typedef struct
{
    long flags;  /* always have flags field first.  The bbs will use
       this field to indicate to the acct subsystem 
       whether we've gotten their money, whether its a back order,
       etc.  Some of this (like no money) could be scrapped in favor
       of using the info in the ccReq structure.  */

    /* user stuff, set by server */
    long userID;
    long sessionID;

    /********************************/
    /* product identification stuff */
    char prodType; /* WS, LIB, CLIP or FONT.... */

    //char clipDesc[MAX_STR_LEN+1];
    //char clipFileName[13];
    long clipID;
    char Version[MAX_STR_LEN+1];
    //char libName[MAX_STR_LEN+1];
    long libID;
    char mediaType; /* if library, need this with productID for full 
		       identification. */
    //char vendorName[MAX_STR_LEN+1];
    long pubID;
    long vendorID;
    /********************************/
    
    /* shipping stuff */
    ADDRESS ShipAddress;
    char CourierType;
  
    // long corpID;

    /* order, credit card stuff */
    long orderID; // for bbs use only
    char ccName[MAX_STR_LEN+1];    
    char ccNum[MAXLEN_CCNUMBER+1];
    char ccExpDate[MAXLEN_EXPDATE+1];
    short ccType;
    
    /* price info */
    long  Quantity;
    long  priceTotal; /*  * 100 (should be called 'total'- includes tax...) */
    long  priceEach;  /*  * 100   */
    long  taxamount;
    long  shipping;
    long  surcharge;

} ORDER;

typedef struct {
    WORD   valid;
    DWORD   cost;       // in cents
} SHIPPING_COSTS_STRUCT;
#pragma pack()

#define MAX_NUM_SERVICES    3

extern SHIPPING_COSTS_STRUCT ShippingCosts[MAX_NUM_SERVICES];

/*-------------------------------------------------------------------------*/
/*                         G L O B A L   F U N C T I O N S                 */
/*-------------------------------------------------------------------------*/
extern WORD FAR PASCAL OrderLibrary(char *, WORD, WORD, char *);
extern VOID	FAR PASCAL CreateOrder(void);
extern LONG FAR PASCAL WSOrderFormDlgProc (HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL WSChangePasswordDlgProc(HWND hWndDlg, unsigned message, 
                                               WORD wParam, LONG lParam);
extern LONG FAR PASCAL WSPasswordEditProc(HWND hWnd, unsigned message, 
                                          WORD wParam, LONG lParam);
LONG FAR PASCAL WSNewUserDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam);  
BOOL FAR PASCAL WSVerifyPasswdDlgProc(HWND hWndDlg, unsigned message, WORD wParam, LONG lParam);
extern int TranslateMType(HWND hList, int Index, int *MType) ;
#endif
