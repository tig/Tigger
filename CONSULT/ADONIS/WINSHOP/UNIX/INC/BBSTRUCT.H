/****************************************************************************
 from mitchl's STRUCT.H

   Description:
	structure definition file for SQL accounting modules
	replaces all DWORD, WORD, BYTE types with base type usage
	
	MUST CORRESPOND EXACTLY TO TYPEDEFS IN BBS.H!
			
   History:
        8/12/90	   written - mitchl
	8/15/90    modified dug, beng
			struct definitions with base types replace those
			in bbs.h with typedef types.  struct.h is no longer
			needed.  These struct definitions will also be used in
			Window Shopper.
			
			Dug also modified some of the fields in ORDER_STRUCT,
			removing some unessesary cc and clipart fields.

	6/19/91	-	modified beng
			Eliminated libname, clipdesc, filename, pubname from order struct
****************************************************************************/


/* typedef char MONEY[INGRES_MONEYLEN]; */

typedef float MONEY;  /* (?) 8 byte float value */
typedef char DATE[INGRES_DATELEN];

/** 
	PHONE_STRUCT and ADDRESS_STRUCT and ORDER must correspond in
	structure and field size to the definitions at the Window
	Shopper end.  Field names don't matter.  This requirement
	might be avoided if we go to using variable-length fields,
	although we'd still need to know what info we're getting in 
	buffer.
**/

typedef struct {
    char    area[MAXLEN_AREA + 1];
    char    number[MAXLEN_PHONE + 1];
    char    ext[MAXLEN_EXT + 1];
} PHONE_STRUCT;
typedef PHONE_STRUCT PHONE;

typedef struct {
    char         name[MAXLEN_NAME + 1];
    char         title[MAXLEN_TITLE + 1];
    char         company[MAXLEN_NAME + 1];
    char         street[MAXLEN_ADDRESS + 1];
    char         street2[MAXLEN_ADDRESS + 1];
    char         city[MAXLEN_CITY + 1];
    char         state[MAXLEN_STATE + 1];
    char         province[MAXLEN_PROVINCE + 1];
    short         whichCountry;
    char         country[MAXLEN_COUNTRY + 1];
    char         postal_code[MAXLEN_POSTAL + 1];
    PHONE_STRUCT phone;
} ADDRESS_STRUCT;
typedef ADDRESS_STRUCT ADDRESS;

typedef struct
{
    long flags;  /* always have flags field first.  The bbs will use
       this field to indicate to the acct subsystem 
       whether we've gotten their money, whether its a back order,
       etc.  Some of this (like no money) could be scrapped in favor
       of using the info in the ccReq structure.  */

    /* user stuff */
    long userID;						/* User ID */
    long sessionID;						/* Session ID */

    /********************************/
    /* product identification stuff */
    char prodType;						/* WS, LIB, CLIP or FONT */
    /*char clipDesc[MAX_STR_LEN+1];*/		/* Clip file description */
    /*char clipFileName[13];*/				/* Clip file name */
    long clipID;						/* Clip ID */
    char Version[MAX_STR_LEN+1];		/* Format */
    /*char libName[MAX_STR_LEN+1];*/		/* Library name */
    long libID;							/* Product ID */
    char mediaType;						/* If library, need this with
										   productID for full identification. */
    /*char vendorName[MAX_STR_LEN+1];*/		/* Vendor name */
	long pubID;							/* Publisher ID */
    long vendorID;						/* Vendor ID */
    /********************************/
    
    /* shipping stuff */
    ADDRESS ShipAddress;				/* Address */
    char CourierType;					/* Courier type */
  
   /* long corpID; ?? */

    /* order, credit card stuff */
    long orderID;						/* Order ID */
    char ccName[MAX_STR_LEN+1];			/* Name on credit card */
	char ccNum[MAXLEN_CCNUMBER+1];		/* Credit card number */
	char ccExpDate[MAXLEN_EXPDATE+1];	/* Credit card expiration date */
	short ccType;						/* Credit card type */
    
    /* price info */
    long  Quantity;						/* Quantity ordered */
    long  priceTotal; /*  * 100 (should be called 'total'- includes tax...) */
    long  priceEach;  /*  * 100   */
    long  taxamount;
    long  shipping;
    long  surcharge;

} ORDER_STRUCT;


/*
   The structure wherein credit card transaction requests and responses
   are communicated between the BBS and the parent and child credit card
   proceses.

	created 4/90 beng, 
	added to bbsstruct.h 8/15/90
*/

typedef struct 
{
   int   MsgCode;   /* request for transaction - sale, credit, void, inquiry,
                    etc., or response from bank or an update message from a
                    child credit card proces to its parent; see codes in id.h */
   int   InqCode;     /* Code for type of inquiry to Bank */
   char  AcctNum[17]; /* credit card account number */
   char  ExpDate[5];  /* account expiration date */
   int   CCType;      /* Credit card type - VISA, MC, etc. */
   float Amount;      /* Amount of requested debit or credit */
   int   Result;      /* Codified response from Bank i.e. transaction authorized
                         or message to parent from child */
   char  AuthNum[7];/* Authorization number from Bank for transaction */
   char  RefNum[9]; /* transaction reference number from Bank */
   char  BatNum[4]; /* 3 digit batch # which transaction or inquiry refers to*/
   int   BatCnt;    /* Number of active transactions in batch */
   float RunBal;    /* Running Batch Total */
   int   Route;     
   int   Force;     /* 0 = no force, 1 = Force duplicate override */
   int   ReSend;    /* Indicates whether this request is being re-submitted; */
						  /* 0 = first time being sent, 1 = being re-sent */
	int	 ErrFlag;     /* bit field for different error conditions */
} Credit_Struct;

typedef Credit_Struct CCReq;
typedef struct
	{
		short valid;
		int cost;
	} SHIPCOST;
typedef SHIPCOST Shipcost_Ary[MAXNUM_SERVICES];
