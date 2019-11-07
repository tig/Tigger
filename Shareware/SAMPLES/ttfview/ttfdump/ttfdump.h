/***************************************************************/
/*  Copyright 1989, ZSoft Corporation									*/
/*  All Rights Reserved.													*/
/*																					*/
/*  This material is the proprietary and trade secret				*/
/*  information of ZSoft Corporation and may not be				*/
/*  used by companies or individuals without its written			*/
/*  authorization.  Reproduction by or disclosure to				*/
/*  unauthorized personnel is strictly prohibited.					*/
/***************************************************************/
/*	Module: t2dump.h                                            */
/*-------------------------------------------------------------*/
/*	Written by: Rich Taylor                Date: 10/5/90			*/
/*                                                             */
/* Description: Global Defines for T2DUMP.C							*/
/*                                                             */
/* Procedures: <none>                                          */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*		--------------------- Includes -----------------------	*/

/* Apple TrueType defines */
#define	INTEL
#include	"fscdefs.h"
#include	"sfnt.h"


/*		---------------------- Defines -----------------------	*/

#ifdef NULL
	#undef NULL
#endif

#define	NULL	(void *) 0L

/* Table Dump Routine Return Codes */
#define	DMP_NOTPRESENT		-1
#define	DMP_OK				0
#define	DMP_ERR				1

typedef struct
	{
	sfnt_TableTag	tag;
	uint32			checkSum;
	uint32			offset;
	uint32			length;
	uint8				dumped;		/* 0 = no, 1 = yes */
	} t2dmp_DirectoryEntry;

/*
 *      The search fields limits numOffsets to 4096.
 */
typedef struct {
        int32	version;                                  /* 0x10000 (1.0) */
        uint16 numOffsets;                              /* number of tables */
        uint16 searchRange;                             /* (max2 <= numOffsets)*16 */
        uint16 entrySelector;                   /* log2(max2 <= numOffsets) */
        uint16 rangeShift;                              /* numOffsets*16-searchRange*/
        t2dmp_DirectoryEntry table[1];   /* table[numOffsets] */
} t2dmp_OffsetTable;

#define OFFSETTABLESIZE         12      /* not including any entries */

typedef struct
	{
	int16	indexToLocFormat;
	int16	glyphDataFormat;
	} headparm;

typedef struct
	{
	uint16	version;
	int16		xAvgCharWidth;
	uint16	usWeightClass;
	uint16   usWidthClass;
	int16		fsType;
	int16		ySubscriptXSize;
	int16		ySubscriptYSize;
	int16		ySubscriptXOffset;
	int16		ySubscriptYOffset;
	int16		ySuperscriptXSize;
	int16		ySuperscriptYSize;
	int16		ySuperscriptXOffset;
	int16		ySuperscriptYOffset;
	int16		yStrikeoutSize;
	int16		yStrikeoutPosition;
	int8		sFamilyClass;
	int8		sFamilySubclass;
	uint8		panose[10];
	uint32	ulCharRange[4];
	uint8		achVendID[4];
	uint16	fsSelection;
	/* Added by dennisad 5-24-91 */
	uint16	usFirstCharIndex;
	uint16	usLastCharIndex;
	int16		sTypoAscender;
	int16		sTypoDescender;
	int16		sTypoLineGap;
	uint16	usWinAscent;
	uint16	usWinDescent;
	} sfnt_OS2Metrics;


/* Kern table structures */
	
typedef struct
	{
	uint16	version;
	uint16	nTables;
	} sfnt_kernHeader;

typedef struct
	{
	uint16	fmtvers;
	uint16	length;
	uint8		format;		/* Format actually comes AFTER flags in the Motorola */
	uint8		flags;		/* scheme, but we cheat 'cause this is INTEL and we */
	} sfnt_kernSubHead;	/* reverse them in the structure to save loop time work */

typedef struct
	{
	uint16	nPairs;
	uint16	searchRange;
	uint16	entrySelector;
	uint16	rangeShift;
	} sfnt_kern0Header;

typedef struct
	{
	uint16	left;
	uint16	right;
	int16		move;
	} sfnt_kern0Data;

/* LTSH table structures */

typedef struct
	{
	uint16	version;
	uint16	numGlyphs;
	} sfnt_LTSHHead;


/* VDMX table structures */

typedef struct
	{
	uint8		bCharSet;		/* Charset identifier, 0= all glyphs, 1= Windows subset */
	uint8		xRatio;
	uint8		yStartRatio;
	uint8		yEndRatio;
	} vdmx_Ratios;

typedef struct
	{
	uint16	yPelHeight;		/* Pel height for these height values */
	int16		yMax;
	int16		yMin;
	} vdmx_HgtTable;

typedef struct
	{
	uint16			numRecs;			/* Number of hgt records in this group */
	uint8				startsz;			/* Starting y pel height */
	uint8				endsz;			/* Ending y pel height */
	vdmx_HgtTable	recs[1];			/* Records ... */
	} vdmx_HgtRecs;

typedef struct
	{
	uint16			version;			/* Version number */
	uint16			numRecs;			/* Number of VDMX height records present */
	uint16			numRatios;		/* Number of aspect ratio records present */
	char				data;				/* The rest: *vdmx_Ratios, *Offsets, */
											/* *vdmx_HgtRecs. */
	} sfnt_VDMX;

