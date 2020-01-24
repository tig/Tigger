#ifndef BBSH
#define BBSH 1

#define STD_WAIT 5

#ifndef SQL
#ifndef ACCTH
#include "acct.h"
#endif
#endif

/* Directory to write log files to */
#define LOG_DIR "/usr/bbs/log"

/* Window shopper executable types as defined by WS */
#define EXE_BETA 0
#define EXE_REGULAR 1
#define EXE_INTERNAL 2

/* password and userid maximum sizes */
#define     PW_SIZE    		8

#define     MAX_STR_LEN       120
#define     MAXLEN_AREA         3
#define     MAXLEN_PHONE        8
#define     MAXLEN_EXT          4
#define     MAXLEN_TELE        24	/* For combined, formatted number */
#define     MAXLEN_CARDNUM     19
#define     MAXLEN_EXPIRE       2
#define     MAXLEN_NAME        30
#define	    MAXLEN_STREET      30
#define     MAXLEN_ADDRESS     MAXLEN_STREET
#define     MAXLEN_CITY        25
#define     MAXLEN_STATE        2
#define     MAXLEN_COUNTRY     20
#define     MAXLEN_POSTAL      10
#define	    MAXLEN_MNTH_DIGITS  2
#define	    MAXLEN_YEAR_DIGITS  2
#define     MAXLEN_EXPDATE MAXLEN_MNTH_DIGITS + MAXLEN_YEAR_DIGITS
#define     MAXLEN_CCNUMBER    16
#define     MAXLEN_PROVINCE    25
#define     MAXLEN_TITLE       30

/* credit authorization code, batch number, and reference number */
#define	    MAXLEN_AUTHCODE     6	
#define	    MAXLEN_BATCHNO      3
#define     MAXLEN_REFNO	8

/* misc. Ingres related definitions */ 
#define     MAXLEN_DOSFILENAME  12 	/* eight.dot.three */
#define	    MAXLEN_SHORTCOMMENT 20
#define	    MAXLEN_VENDORCODE   20	
#define     INGRES_MONEYLEN	8
#define     INGRES_DATELEN     25

/* order flags (for flags field of order structure) */
#define OF_INITIALIZE  		0x1  /* call NormalOrder with this before trying to
								    capture credit */
#define OF_CC_RESULT		0x2  /* call NormalOrder with CC_RESULT to record
									result of credit capture.  Result of
									capture should be stored in ccReq */
#define OF_COMMITED     	0x4  /* call NormalOrder with COMMITED when order
									has been completed as far as the bbs/WS
									interaction goes */
#define OF_SUBMITTED		0x8  /* call NormalOrder with SUBMIT when order
									has been submitted for delivery (like
									a clipart is about to be downloaded) */
#define OF_SHIPPED  		0x10 /* call NormalOrder with SHIPPED when
									as far as we can tell, the customer
									has received his/her purchase (like
									a clipart has been downloaded) */
#define OF_BACK_ORDERED     0x20 /* call NormalOrder with BACK_ORDERED
                                    to mark order as being out-of-stock.
                                    Does not imply that order has been 
                                    commited or anything. */


/* defined as such in Window Shopper: */
#define USA 	0
#define CANADA	1
#define OTHER	2
/* our own added touch: */
#define NON_CONTINENTAL_USA	3


#endif
