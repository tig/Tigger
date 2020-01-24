/*************************************************************\
 ** FILE:  ksearch.h
 **
 ** Copyright (c) Adonis Corporation. 1991
 **
 ** DESCRIPTION:
 **  Include file for keyword search dialog box
 **
 ** HISTORY:   
 **  Lauren Bricker   4/11/91        Created
 **
\*************************************************************/


#ifndef KSEARCH
#define KSEARCH


/* defines */

#define	IT_DOWNLOAD	   0x00000001

#define	IT_COLOR		   0x00000002

#define  IT_PRICEFREE   0x00000004   /* See note below */
#define	IT_PRICE1_10	0x00000008   /* See note below */
#define	IT_PRICE11_20	0x00000010   /* See note below */

#define	IT_EPS			0x00000020
#define	IT_PCX			0x00000040
#define	IT_TIF			0x00000080
#define	IT_CGM		   0x00000100
#define  IT_TBK         0x00000200
#define	IT_CDR			0x00000400
#define	IT_DRW		   0x00000800
#define	IT_AI			   0x00001000
#define	IT_WPG		   0x00002000
#define	IT_GEM		   0x00004000
#define	IT_GED			0x00008000
/*
		0x00010000 - 0x10000000 reserved for future formats
*/


#define NUM_FORMATS     11
#define FORMAT_SHIFT     5       /* Which bit the formats start on... */

#define PC_TYPES 	   (IT_EPS | IT_PCX | IT_TIF | IT_CGM | IT_TBK | IT_CDR | \
                      IT_DRW | IT_AI | IT_WPG | IT_GEM | IT_GED)

#define MAC_TYPES    (0x0)

#define ALL_TYPES 	(DWORD)(PC_TYPES | MAC_TYPES)



/* 
** Note:  Though we won't check for them in version 1.2 -
**        we can check for specific prices as follows:
**
**
** if image_type & IT_PRICEFREE then
**    there is a free image
** else if image_type & IT_PRICE1_10 then
**    there is an image < $10
** else if image_type & IT_PRICE11_20 then
**    there is an image between $11 and $20
** else all images are over $20.
*/

/* Exported variables */

extern BOOL bFixAfterFind;

/* Exported functions */

extern BOOL             StartSearch (HWND);
extern LONG FAR PASCAL  WSSearchDlgProc (HWND, unsigned, WORD, LONG);
extern LONG FAR PASCAL  WSStopSearchDlgProc (HWND, unsigned, WORD, LONG);

#endif

