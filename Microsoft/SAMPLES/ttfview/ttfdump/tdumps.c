/***************************************************************/
/*  Copyright 1989, ZSoft Corporation                          */
/*  All Rights Reserved.                                       */
/*                                                             */
/*  This material is the proprietary and trade secret          */
/*  information of ZSoft Corporation and may not be            */
/*  used by companies or individuals without its written       */
/*  authorization.  Reproduction by or disclosure to           */
/*  unauthorized personnel is strictly prohibited.             */
/***************************************************************/
/*                                                             */
/*      Module: tdumps.c                                       */
/*-------------------------------------------------------------*/
/*      Written by: Rich Taylor                Date: 10/5/90   */
/*                                                             */
/* Description: TrueType "simple" table dumping routines       */
/*                                                             */
/* Procedures:                                                 */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*              --------------------- Includes -----------------------  */

#include		<stdio.h>
#include		<io.h>
#include		<malloc.h>
#include		<stdlib.h>

#include        "ttfdump.h"
#include			 "psnames.h"

/*              ---------------------- Defines -----------------------  */

/*              ----------- Local Variables and structures -----------  */
/*                                                      should be declared static                                               */

/*              ------------------ Local Functions -------------------  */
/*                                              should be declared static NEAR                                  */

/*              ------------------ Global Functions ------------------  */
/*                                              should be declared FAR PASCAL                                           */

extern float   FixedPrint( long, char * );
extern char *	do_time( uint32 , uint32, int );
extern unsigned long    FindOffset( long, t2dmp_OffsetTable * , uint32 * );
extern void             TTIUnAsm( unsigned char *, unsigned int );
extern int		CheckOneTag( long );

void HexDump( char *, unsigned long );
int OS2_Dump ( int, t2dmp_OffsetTable * );
int cvt_Dump ( int, t2dmp_OffsetTable * );
int fpgm_Dump( int, t2dmp_OffsetTable * );
int hdmx_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable * );
int head_Dump( int, t2dmp_OffsetTable *, headparm *, int );
int hhea_Dump( int, t2dmp_OffsetTable *, uint16 * );
int hmtx_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable *, uint16 );
int maxp_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable * );
int name_Dump( int, t2dmp_OffsetTable * );
int post_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable * );
int prep_Dump( int, t2dmp_OffsetTable * );
int kern_Dump( int, t2dmp_OffsetTable * );
int LTSH_Dump( int, t2dmp_OffsetTable * );


int OS2_Dump( nIn, T2Table )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
{
int                                     i;
long                                    tag = 0x4F532F32;               /* 'OS/2' */
unsigned long           offset, length;
sfnt_OS2Metrics OS2;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'OS/2' Table - OS/2 and Windows Metrics\n---------------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( sizeof(OS2) != (i=read( nIn, (void *)&OS2, sizeof(OS2) )) )
			{
			printf("Error reading table, read %d bytes, expected %d\n",
				i, sizeof(OS2));
			return( DMP_ERR );
			}

		printf( "Size = %lu bytes (expecting %d bytes)\n", length, sizeof(OS2) );
		printf( "  'OS/2' version:      %u\n", SWAPW(OS2.version) );
		printf( "  xAvgCharWidth:       %d\n", SWAPW(OS2.xAvgCharWidth) );
		printf( "  usWeightClass:       %u\n", SWAPW(OS2.usWeightClass) );
		printf( "  usWidthClass:        %u\n", SWAPW(OS2.usWidthClass) );
		printf( "  fsType:              0x%04X\n", SWAPW(OS2.fsType) );

		printf( "  ySubscriptXSize:     %d\n", SWAPW(OS2.ySubscriptXSize) );
		printf( "  ySubscriptYSize:     %d\n", SWAPW(OS2.ySubscriptYSize) );
		printf( "  ySubscriptXOffset:   %d\n", SWAPW(OS2.ySubscriptXOffset) );
		printf( "  ySubscriptYOffset:   %d\n", SWAPW(OS2.ySubscriptYOffset) );
		printf( "  ySuperscriptXSize:   %d\n", SWAPW(OS2.ySuperscriptXSize) );
		printf( "  ySuperscriptYSize:   %d\n", SWAPW(OS2.ySuperscriptYSize) );
		printf( "  ySuperscriptXOffset: %d\n", SWAPW(OS2.ySuperscriptXOffset) );
		printf( "  ySuperscriptYOffset: %d\n", SWAPW(OS2.ySuperscriptYOffset) );
		printf( "  yStrikeoutSize:      %d\n", SWAPW(OS2.yStrikeoutSize) );
		printf( "  yStrikeoutPosition:  %d\n", SWAPW(OS2.yStrikeoutPosition) );

		printf( "  sFamilyClass:        %d    subclass = %d\n", (int)OS2.sFamilyClass, (int)OS2.sFamilySubclass );

		printf( "  PANOSE:              %u", OS2.panose[0] );
		for( i=1; i<10; i++ )   printf( " %2u", OS2.panose[i] );
		printf( "\n" );

		printf( "  Character Range:     0x" );
		for( i=0; i<4; i++ )            printf( "%08lX", OS2.ulCharRange[i] );
		printf( "\n" );

		if( OS2.achVendID[0] )
			printf( "  achVendID:           '%.4s'\n", (char *)OS2.achVendID );

		printf( "  fsSelection          0x%04X\n", SWAPW(OS2.fsSelection) );
		printf( "  usFirstCharIndex:    0x%04X\n", SWAPW(OS2.usFirstCharIndex) );
		printf( "  usLastCharIndex:     0x%04X\n", SWAPW(OS2.usLastCharIndex) );
		printf( "  sTypoAscender        %d\n", SWAPW(OS2.sTypoAscender) );
		printf( "  sTypoDescender       %d\n", SWAPW(OS2.sTypoDescender) );
		printf( "  sTypoLineGap         %d\n", SWAPW(OS2.sTypoLineGap) );
		printf( "  usWinAscent          %u\n", SWAPW(OS2.usWinAscent) );
		printf( "  usWinDescent         %u\n", SWAPW(OS2.usWinDescent) );

		}

	return( DMP_OK );

} /* OS2_Dump() */


int cvt_Dump( nIn, T2Table )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
{
unsigned int            i, j;
long                                    tag = tag_ControlValue;
unsigned long           offset, length, x;

sfnt_ControlValue       *cvt;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'cvt ' Table - Control Value Table\n----------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		return( DMP_NOTPRESENT );

	/* Allocate memory for cvt */
	if( NULL == (cvt = malloc( length )) )
		{
		printf( "Error (1) allocating memory for table.\n" );
		return( DMP_ERR );
		}

	/* Load Table */
	if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
		{
		free( cvt);
		printf( "Error seeking to start of table\n" );
		return( DMP_ERR );
		}
	if( length != (x=(uint32) read( nIn, (void *)cvt, (uint16) length )) )
		{
		free( cvt );
		printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
		return( DMP_ERR );
		}

	j = (uint16) (length / sizeof(sfnt_ControlValue));

	printf("Size = %lu bytes, %u entries\n\tValues\n\t------\n", length, j );
	for( i=0; i<j; i++ )
		printf("\t%4u: %d\n", i, SWAPW(cvt[i]) );

	free( cvt );

	return( DMP_OK );

} /* cvt_Dump() */


int fpgm_Dump( nIn, T2Table )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
{
long                                    tag = tag_FontProgram;
unsigned long           offset, length, x;
char                                    *fpgm;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate memory for table */
		if( NULL == (fpgm = malloc( length )) )
			{
			printf( "Error allocating memory for table\n" );
			return( DMP_ERR );
			}

		printf( "\n'fpgm' Table - Font Program\n---------------------------\n" );
		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			free( fpgm );
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)fpgm, (uint16) length )) )
			{
			free( fpgm );
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		printf( "Size = %lu bytes\n", length );

		if( length > 0L )
			{
			TTIUnAsm( (unsigned char *)fpgm, (uint16) length );
			printf( "\n" );
			}

		free( fpgm );
		}

	return( DMP_OK );

} /* fpgm_Dump() */


int hdmx_Dump( nIn, T2Table, maxp )
int                                             nIn;                    /* file handle */
t2dmp_OffsetTable               *T2Table;       /* offset table pointer */
sfnt_maxProfileTable    *maxp;
{
unsigned int            i, j;
long                                    tag = tag_HoriDeviceMetrics;
unsigned long           offset, length, x;
sfnt_DeviceMetrics      *hdmx;
typedef struct
	{
	uint8   ppem;
	uint8   maxWidth;
	uint8   width[1];
	}  sfnt_DeviceRecord;
	
sfnt_DeviceRecord         huge *devRec;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		return( DMP_NOTPRESENT );
		}
	else
		{
		printf( "\n'hdmx' Table - Horizontal Device Metrics\n----------------------------------------\n" );

		/* Allocate memory for table */
		if( NULL == (hdmx = malloc( length )) )
			{
			printf( "Error allocating memory for table\n" );
			return( DMP_ERR );
			}

		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			free( hdmx );
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)hdmx, (uint16) length )) )
			{
			free( hdmx );
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		printf( "Size = %lu bytes\n", length);

		hdmx->recordSize = SWAPL(hdmx->recordSize);
		printf( "\t'hdmx' version:\t\t%u\n", SWAPW(hdmx->version) );
		printf( "\t# device records:\t%d\n", hdmx->numRecords = SWAPW(hdmx->numRecords) );
		printf( "\tRecord length:\t\t%ld\n", hdmx->recordSize );

		devRec = (sfnt_DeviceRecord huge *)((char huge *)hdmx +
			(uint32) sizeof(sfnt_DeviceMetrics));

		for( i=0; i < (uint16) hdmx->numRecords; i++ )
			{
			printf( "\tDevRec%3d: ppem = %3u, maxWid = %3u\n\t\t",
							i, (uint16)devRec->ppem, (uint16)devRec->maxWidth );
			for( j=0; j<maxp->numGlyphs; j++ )
				printf( "%3u ", (uint16)devRec->width[j] );
			printf( "\n\n" );
			devRec = (sfnt_DeviceRecord huge *)((char huge *)devRec + hdmx->recordSize);
			}

		free( hdmx );
		}

	return( DMP_OK );

} /* hdmx_Dump() */



int head_Dump( nIn, T2Table, headParm, tZone )
int                     nIn;            /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
headparm                *headParm;
int							tZone;
{
unsigned long           offset, length;
sfnt_FontHeader			head;
int							flag, i;

	if (flag = CheckOneTag(tag_FontHeader) )
		printf( "\n'head' Table - Font Header\n--------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag_FontHeader, T2Table, &length )) )
		{
		if (flag)
			printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			if (flag)
				printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( sizeof(head) != (i=read( nIn, (void *)&head, sizeof(head) )) )
			{
			if (flag)
				printf("Error reading table, read %d bytes, expected %d\n", i, sizeof(head));
			return( DMP_ERR );
			}

		/* Needed in other calls! */
		headParm->indexToLocFormat = SWAPW(head.indexToLocFormat);
		headParm->glyphDataFormat = SWAPW(head.glyphDataFormat);

		if (flag)
			{
			printf( "Size = %lu bytes (expecting %u bytes)\n", length, sizeof(head));
			printf( "  'head' version:    " );
				FixedPrint( head.version, "%5.1f\n" );
			printf( "  fontRevision:      " );
				FixedPrint( head.fontRevision, "%5.1f\n" );
			printf( "  checkSumAdjustment:  0x%08lX\n", SWAPL(head.checkSumAdjustment) );
			printf( "  magicNumber:         0x%08lX\n", SWAPL(head.magicNumber) );
			printf( "  flags:               0x%04X\n", SWAPW(head.flags) ); 
			printf( "  unitsPerEm:          %u\n", SWAPW(head.unitsPerEm) );
			printf( "  created:             %s", do_time(SWAPL(head.created.ad), SWAPL(head.created.bc), tZone) );
			printf( "  modified:            %s", do_time(SWAPL(head.modified.ad), SWAPL(head.created.bc), tZone) );
			printf( "  xMin:                %d\n", SWAPW(head.xMin) );
			printf( "  yMin:                %d\n", SWAPW(head.yMin) );
			printf( "  xMax:                %d\n", SWAPW(head.xMax) );
			printf( "  yMax:                %d\n", SWAPW(head.yMax) );
	
			printf( "  macStyle bits:       0x%04X\n", SWAPW(head.macStyle) );
			printf( "  lowestRecPPEM:       %u\n", SWAPW(head.lowestRecPPEM) );
	
			printf( "  fontDirectionHint:   %d\n", SWAPW(head.fontDirectionHint) );
			printf( "  indexToLocFormat:    %d\n", SWAPW(head.indexToLocFormat) );
			printf( "  glyphDataFormat:     %d\n", SWAPW(head.glyphDataFormat) );
			}
		}

	return( DMP_OK );

} /* head_Dump() */


int hhea_Dump( nIn, T2Table, numHmtx )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
uint16                          *numHmtx;
{
long                                                    tag = tag_HoriHeader;
unsigned long                           offset, length;
sfnt_HorizontalHeader   hhea;
int                                                     flag, i;

	if (flag = CheckOneTag(tag) )
		printf( "\n'hhea' Table - Horizontal Header\n--------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		if (flag)
			printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		if( offset != (uint32)lseek( nIn, offset, SEEK_SET ) )
			{
			if (flag)
				printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( sizeof(hhea) != (i=read( nIn, (void *)&hhea, sizeof(hhea) )) )
			{
			if (flag)
				printf("Error reading table, read %d bytes, expected %d\n", i, sizeof(hhea));
			return( DMP_ERR );
			}

		/* Needed in other calls */
		*numHmtx = SWAPW(hhea.numberOf_LongHorMetrics);

		if (flag)
			{
			printf( "Size = %lu bytes (expecting %u bytes)\n", length, sizeof(hhea));
			printf( "\t'hhea' version:       " );
				FixedPrint( hhea.version, "%5.1f\n" );
			printf( "\tyAscender:            %d\n", SWAPW(hhea.yAscender) );
			printf( "\tyDescender:           %d\n", SWAPW(hhea.yDescender) );
			printf( "\tyLineGap:             %d\n", SWAPW(hhea.yLineGap) );
	
			printf( "\tadvanceWidthMax:      %u\n", SWAPW(hhea.advanceWidthMax) );
			printf( "\tminLeftSideBearing:   %d\n", SWAPW(hhea.minLeftSideBearing) );
			printf( "\tminRightSideBearing:  %d\n", SWAPW(hhea.minRightSideBearing) );
			printf( "\txMaxExtent:           %d\n", SWAPW(hhea.xMaxExtent) );
	
			printf( "\thorizCaretSlopeNum:   %d\n", SWAPW(hhea.horizontalCaretSlopeNumerator) );
			printf( "\thorizCaretSlopeDenom: %d\n", SWAPW(hhea.horizontalCaretSlopeDenominator) );
	
			printf( "\treserved0:            %u\n", SWAPW(hhea.reserved0) );
			printf( "\treserved1:            %u\n", SWAPW(hhea.reserved1) );
			printf( "\treserved2:            %u\n", SWAPW(hhea.reserved2) );
			printf( "\treserved3:            %u\n", SWAPW(hhea.reserved3) );
			printf( "\treserved4:            %u\n", SWAPW(hhea.reserved4) );
	
			printf( "\tmetricDataFormat:     %d\n", SWAPW(hhea.metricDataFormat) );
			printf( "\tnumOf_LongHorMetrics: %u\n", *numHmtx );
			}
		}

	return( DMP_OK );

} /* hhea_Dump() */


int hmtx_Dump( nIn, T2Table, maxp, numHmtx )
int                                             nIn;                    /* file handle */
t2dmp_OffsetTable               *T2Table;       /* offset table pointer */
sfnt_maxProfileTable    *maxp;
uint16                                  numHmtx;
{
unsigned int            i;
unsigned int            numShort;
int16                                   *shortPtr;
long                                    tag = tag_HorizontalMetrics;
unsigned long           offset, length, x;
sfnt_HorizontalMetrics  *hmtx;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'hmtx' Table - Horizontal Metrics\n---------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate memory for table */
		if( NULL == (hmtx = malloc( length )) )
			{
			printf( "Error allocating memory for table\n" );
			return( DMP_ERR );
			}

		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			free( hmtx );
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)hmtx, (uint16) length )) )
			{
			free( hmtx );
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		/* Print "Long" Horizontal Metric Pairs */
		printf( "Size = %lu bytes, %u entries\n", length, numHmtx);
		for( i=0; i<numHmtx; i++ )
			printf( "\t%3d. advWid: %4u, LSdBear: %d\n",    i,
				 SWAPW(hmtx[i].advanceWidth), SWAPW(hmtx[i].leftSideBearing) );

		/* Print remaining ("short") side bearing metric entries */
		numShort = maxp->numGlyphs - numHmtx;

		if( numShort )
			{
			shortPtr = (int16 *)(&hmtx[numHmtx]);
			for( i=0; i<numShort; i++ )
				printf( "\tLSdBear %3d: %d\n", i, SWAPW(shortPtr[i]) );
			}

		free( hmtx );
		}

	return( DMP_OK );

} /* hmtx_Dump() */


int maxp_Dump( nIn, T2Table, maxp )
int                                             nIn;                    /* file handle */
t2dmp_OffsetTable               *T2Table;       /* offset table pointer */
sfnt_maxProfileTable    *maxp;
{
long                                    tag = tag_MaxProfile;
unsigned long           offset, length, x;
int                                     flag;

	if (flag = CheckOneTag(tag) )
		printf( "\n'maxp' Table - Maximum Profile\n------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		if (flag)
			printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			if (flag)
				printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)maxp, (uint16) length )) )
			{
			if (flag)
				printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

			maxp->numGlyphs = SWAPW(maxp->numGlyphs);
		if (flag)
			{
			printf( "Size = %lu bytes (expecting %u bytes)\n", length, sizeof(sfnt_maxProfileTable));
			printf( "\t'maxp' version:\t\t" );
				FixedPrint( maxp->version, "%5.1f\n" );
			printf( "\tnumGlyphs:\t\t%u\n", maxp->numGlyphs );
			printf( "\tmaxPoints:\t\t%u\n", SWAPW(maxp->maxPoints) );
			printf( "\tmaxContours:\t\t%u\n", SWAPW(maxp->maxContours) );
			printf( "\tmaxCompositePoints:\t%u\n", SWAPW(maxp->maxCompositePoints) );
			printf( "\tmaxCompositeContours:\t%u\n", SWAPW(maxp->maxCompositeContours) );
			printf( "\tmaxZones:\t\t%u\n", SWAPW(maxp->maxElements) );
			printf( "\tmaxTwilightPoints:\t%u\n", SWAPW(maxp->maxTwilightPoints) );
			printf( "\tmaxStorage:\t\t%u\n", SWAPW(maxp->maxStorage) );
			printf( "\tmaxFunctionDefs:\t%u\n", SWAPW(maxp->maxFunctionDefs) );
			printf( "\tmaxInstructionDefs:\t%u\n", SWAPW(maxp->maxInstructionDefs) );
			printf( "\tmaxStackElements:\t%u\n", SWAPW(maxp->maxStackElements) );
			printf( "\tmaxSizeOfInstructions:\t%u\n", SWAPW(maxp->maxSizeOfInstructions) );
			printf( "\tmaxComponentElements:\t%u\n", SWAPW(maxp->maxComponentElements) );
			printf( "\tmaxComponentDepth:\t%u\n", SWAPW(maxp->maxComponentDepth) );
			}
		}

	return( DMP_OK );

} /* maxp_Dump() */


int name_Dump( nIn, T2Table )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
{
unsigned register       i;
uint16                          ucount, ustroff;
long                                    tag = tag_NamingTable;
unsigned long           offset, length, x;
sfnt_NamingTable        *name;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'name' Table - Naming Table\n---------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate Naming Table memory */
		if( length > 0x0000FFFF )
			{
			printf( "Naming table >64K.... can't handle\n" );
			return( DMP_ERR );
			}
		else if( NULL == (name = (sfnt_NamingTable *)malloc( (size_t)length )) )
			{
			printf( "Couldn't allocate memory for name table.\n" );
			return( DMP_ERR );
			}

		/* Load Naming Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			free( name );
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)name, (uint16) length )) )
			{
			free( name );
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		ucount = SWAPW(name->count);
		ustroff = SWAPW(name->stringOffset);

		/* display header */
		printf( "\tFormat:\t\t%u\n", SWAPW(name->format) );
		printf( "\tCount:\t\t%u\n", ucount );
		printf( "\tstringOffset:\t\%u\n", ustroff );
		printf( "Size = %lu bytes\n", length);

		/* display naming records & string storage */
		for( i=0; i<ucount; i++)
			{
			unsigned char           pos, uCh;
			register uint16         j;
			uint16                          ulen, uoffset, ustr;
			char                                    charStr[11];

			printf( "\t%3d. Platform ID:\t%u\n", i, SWAPW(name->nameRec[i].platformID) );
			printf( "\t     Specific ID:\t%u\n", SWAPW(name->nameRec[i].specificID) );
			printf( "\t     Language ID:\t%u\n", SWAPW(name->nameRec[i].languageID) );
			printf( "\t     Name ID:\t\t%u\n", SWAPW(name->nameRec[i].nameID) );

			ulen = SWAPW(name->nameRec[i].length);
			uoffset = SWAPW(name->nameRec[i].offset);

			printf( "\t     Length:\t\t%u\n", ulen );
			printf( "\t     Offset:\t\t%u\n", uoffset );

			/* Dual (hex/char) dump string data */
			ustr = ustroff + uoffset;
			printf( "\t\tData: " );
			pos = 0;
			for( j=0; j<ulen; j++)
				{
				if( pos == 10 )
					{
					printf( " :  %s\n\t\t      ", charStr );
					pos = 0;
					}

				uCh = ((char *)name)[ustr+j];

				if( uCh > 31 )
					charStr[pos] = uCh;
				else
					charStr[pos] = '.';
				charStr[pos+1] = 0;

				printf( "%2X ", uCh );  /* print byte of hex data */
				pos ++;
				}
			if (ulen)
				/* Only print the last bit if something was there! */
				printf( "%*s :  %s\n\n", 3*(10 - pos), "", charStr );
			else
				printf( "\n\n" );
			}

		free( name );
		}

	return( DMP_OK );

} /* name_Dump() */


int post_Dump( nIn, T2Table ,maxp )
int                                             nIn;                    /* file handle */
t2dmp_OffsetTable               *T2Table;       /* offset table pointer */
 sfnt_maxProfileTable * 			maxp;
{
long                                    tag = tag_Postscript;
unsigned long           offset, length, x;
float                                   fPostVer;
sfnt_PostScriptInfo     *post;


	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'post' Table - PostScript Metrics\n---------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate memory for table */
		if( NULL == (post = malloc( length )) )
			{
			printf( "Error allocating memory for table\n" );
			return( DMP_ERR );
			}

		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			free( post );
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)post, (uint16) length )) )
			{
			free( post );
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		printf( "Size = %lu bytes\n", length );
		printf( "\t'post' version:\t" );
			fPostVer = FixedPrint( post->version, "%11.1f\n" );
		printf( "\titalicAngle:\t" );
			FixedPrint( post->italicAngle, "%14.4f\n" );
		printf( "\tunderlinePosition:\t%d\n", SWAPW(post->underlinePosition) );
		printf( "\tunderlineThickness:\t%d\n", SWAPW(post->underlineThickness) );
		printf( "\tisFixedPitch:\t\t%lu\n", SWAPL(post->isFixedPitch) );
		printf( "\tminMemType42:\t\t%lu\n", SWAPL(post->minMemType42) );
		printf( "\tmaxMemType42:\t\t%lu\n", SWAPL(post->maxMemType42) );
		printf( "\tminMemType1:\t\t%lu\n", SWAPL(post->minMemType1) );
		printf( "\tmaxMemType1:\t\t%lu\n", SWAPL(post->maxMemType1) );

		if( 1.0 == fPostVer )
			{
			printf( "\n\tFormat 1.0:  Standard Macintosh TrueType Glyph Set.\n" );
			}
		else if ( 2.0 == fPostVer )
			{
			unsigned register	i;
			uint16				maxN;
			uint16				numGlyphs;
			char					**glyfName;
			struct				sfnt_post20hdr
				{
				uint16  numGlyphs;
				uint16  glyphNameIndex[1];
				} *hdr20;
			unsigned char	*pos;

			printf( "\n\tFormat 2.0:  Non-Standard (for PostScript) TrueType Glyph Set.\n" );

			hdr20 = (struct sfnt_post20hdr *)((char *)&(post->maxMemType1) + sizeof(post->maxMemType1));

			numGlyphs = SWAPW(hdr20->numGlyphs);
			printf( "\tnumGlyphs:\t%u\n", numGlyphs );

			/* Parse Glyph Names Table */
			pos = (unsigned char *)((char *)hdr20 + (sizeof(uint16) * (numGlyphs+1)));
			if( NULL == (glyfName = malloc( (numGlyphs+1) * sizeof(char *) )))
				{
				printf( "Error: Unable to allocate memory for glyph name pointers\n");
				return (DMP_ERR);
				}
			maxN = 0;
			while( pos < (char *)((char *)post + length) )
				{
				uint16    strLen;

				/* get and print next glyph name string */
				if (maxN >= numGlyphs)
					{
					glyfName = realloc( glyfName, (2+maxN) * sizeof(char *));
					if( glyfName == NULL )
						{
						printf( "Error: Realloc of glyph name pointer list failed\n");
					   return (DMP_ERR);
						}
					}
				strLen = (int)*pos;
				glyfName[maxN] = pos;
 
				for( i=0; i<strLen; i++)
					pos[i] = pos[i+1];
				pos[strLen] = '\0';
				maxN++;
				pos += strLen+1;
				}
			 /* Used for invalid name indices */
			 glyfName[maxN] = "*** UNDEFINED NAME INDEX ***";			 

			/* Dump Glyph Name/Index Array */
			for( i=0; i<numGlyphs; i++ )
				{
				unsigned int    index;

				printf( "\tGlyf %3d -> ", i );

				index = SWAPW(hdr20->glyphNameIndex[i]);
				if( index <= 257 )
					printf( "Mac Glyph # %u, '%s'\n", index, g_PSGlyphNames[index].glyphName );
				else if( index >= 32768 )
					printf( "Reserved\n" );
				else    /* 258 -> 32767 */
					{
					index -= 257;
					printf( "PSGlyf Name # %u, name= '%s'\n", index, 
						glyfName[min(maxN,index-1)] );
					}
				}
			printf( "\n\tFull List of PSGlyf Names\n\t-------------------------\n");
			for( i=0 ; i<maxN ; i++)
				printf( "\tPSGlyf Name # %3u: %s\n", i+1, glyfName[i] ); 

			free( *glyfName );

			} /* Format 2.0 dump code */

		else if ( 2.5 == fPostVer )
			{
			unsigned char	   *pos;
			unsigned register i;

			printf( "\n\tFormat 2.5:  Subset or Re-Ordering of Macintosh TrueType Glyph Set.\n" );
			pos = (unsigned char *)&(post->maxMemType1) + sizeof(post->maxMemType1);
			printf( "Glyf\tMac Name Index\n----\t--------------\n");
			for (i=0 ; i<maxp->numGlyphs ; i++)
				printf("%3u.\t%u\n",i, pos[i]);
			}
		else if ( 3.0 == fPostVer )
			{
			printf( "\n\tFormat 3.0:  No PostScript Name Data.\n" );
			}

		free( post );
		}

	return( DMP_OK );

} /* post_Dump() */


int prep_Dump( nIn, T2Table )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
{
long                                    tag = tag_PreProgram;
unsigned long           offset, length, x;
char                                    *prep;


	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate memory for table */
		printf( "\n'prep' Table - Control Value Program\n------------------------------------\n" );

		if( NULL == (prep = malloc( length )) )
			{
			printf( "Error allocating memory for table\n" );
			return( DMP_ERR );
			}

		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			free( prep );
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)prep, (uint16) length )) )
			{
			free( prep );
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		printf( "Size = %lu bytes\n", length );
		if( length > 0 )
			{
			if (length <= 65535)
				TTIUnAsm( (unsigned char *)prep, (uint16) length );
			else
				printf( "Too large; max size to disassmble is 64K.");
			printf( "\n" );
			}

		free( prep );
		}

	return( DMP_OK );

} /* prep_Dump() */


int kern_Dump( nIn, T2Table )
int                                     nIn;                    /* file handle */
t2dmp_OffsetTable       *T2Table;       /* offset table pointer */
{
long                                    tag = tag_Kerning;
unsigned long           offset, length, x;
uint16                                  i, j;
char                                    *kern;
sfnt_kernHeader *khead;
sfnt_kernSubHead        *kshead;
sfnt_kern0Header        *k0head;
sfnt_kern0Data          *k0data;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		return( DMP_NOTPRESENT );
	printf( "\n'kern' Table - Kerning Data\n---------------------------\n" );
	
	if( NULL == (kern = malloc( length )) )
		{
		printf( "Error allocating memory for table\n" );
		return( DMP_ERR );
		}

	/* Load Table */
	if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
		{
		free( kern );
		printf( "Error seeking to start of table\n" );
		return( DMP_ERR );
		}
	if( length != (x=(uint32) read( nIn, (void *)kern, (uint16) length )) )
		{
		free( kern );
		printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
		return( DMP_ERR );
		}
	khead = (sfnt_kernHeader *) kern;
	kshead = (sfnt_kernSubHead *) ( (char *)kern + sizeof(sfnt_kernHeader) );

	printf( "Size = %lu bytes\n", length);
	printf( " 'kern' Version:           %u\n", SWAPW(khead->version) );
	printf( " Number of subtables:      %u\n", khead->nTables = SWAPW(khead->nTables) );

	/* Loop for each subtable */
	for( i=0 ; i<khead->nTables ; i++ )
		{
		/* kshead is initialized outside the loop and updated in here at the end */
		printf( "\n   Subtable format     %u\n", (uint16) kshead->format );
		printf( "   Subtable version    %u\n", kshead->fmtvers = SWAPW(kshead->fmtvers) );
		printf( "   Bytes in subtable   %u\n", kshead->length = SWAPW(kshead->length) );
		printf( "   Coverage bits       %#2x\n", (uint16) kshead->flags );

		if (kshead->fmtvers)
			{
			/* Format is not zero... hexdump it! */
			printf( "** Hex dump the kerning data follows...\n");
			HexDump( (char *)kshead + sizeof(sfnt_kernSubHead), kshead->length - sizeof(sfnt_kernSubHead) );
			}
		else
			{
			/* Format 0... we can handle this! */
			k0head = (sfnt_kern0Header *) ((char *)kshead + sizeof(sfnt_kernSubHead));
									   
			printf( "   Number of pairs     %u\n", k0head->nPairs = SWAPW(k0head->nPairs) );
			printf( "   Search Range        %u\n", SWAPW(k0head->searchRange) );
			printf( "   Entry Selector      %u\n",  SWAPW(k0head->entrySelector) );
			printf( "   Range Shift         %u\n", SWAPW(k0head->rangeShift) );
			printf( "\n   Left Glyph   Right Glyph   Kern Move\n   ----------   -----------   ---------\n");

			k0data = (sfnt_kern0Data *) ((char *)k0head + sizeof(sfnt_kern0Header));
			for( j=0 ; j<k0head->nPairs ; j++)
				{
				printf("%13u %13u %+11d\n", SWAPW(k0data->left), SWAPW(k0data->right),
					SWAPW(k0data->move) );
				k0data = (sfnt_kern0Data *) ((char *)k0data + sizeof(sfnt_kern0Data));
				}
			}

		/* Point to next kern subtable */
		kshead = (sfnt_kernSubHead *) ((char *)kshead + kshead->length );

	} /* end of FOR on kern subtables */

	free( kern );
	return( DMP_OK );

} /* kern_Dump() */


int LTSH_Dump( nIn, T2Table )
int						nIn;                    /* file handle */
t2dmp_OffsetTable		*T2Table;       /* offset table pointer */
{
long						tag = 0x4c545348;               /* 'LTSH' */
unsigned long			offset, length, x;
char						*ltsh;
sfnt_LTSHHead   		*lhead;
uint8						*ypels;
unsigned register		i;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		return( DMP_NOTPRESENT );
	printf( "\n'LTSH' Table - Linear Threshold Table\n-------------------------------------\n" );
	printf( "Size = %lu bytes\n", length);
	
	if( NULL == (ltsh = malloc( length )) )
		{
		printf( "Error allocating memory for table\n" );
		return( DMP_ERR );
		}

	/* Load Table */
	if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
		{
		free( ltsh );
		printf( "Error seeking to start of table\n" );
		return( DMP_ERR );
		}
	if( length != (x=(uint32) read( nIn, (void *)ltsh, (uint16) length )) )
		{
		free( ltsh );
		printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
		return( DMP_ERR );
		}
	
	lhead = (sfnt_LTSHHead *) ltsh;
	printf( " 'LTSH' Version:          %u\n", SWAPW(lhead->version) );
	printf( " Number of Glyphs:        %u\n", lhead->numGlyphs=SWAPW(lhead->numGlyphs) );
	printf( "\n   Glyph #   Threshold\n   -------   ---------\n");

	ypels = (uint8 *) ((char *) lhead + sizeof(sfnt_LTSHHead));
	for (i=0 ; i<lhead->numGlyphs ; i++)
		printf( "%9u. %11u\n", i, (uint16) ypels[i]);

	free( ltsh );
	return( DMP_OK );

} /* LTSH_Dump() */



int LTSH_Show( ltsh)
char                            *ltsh;
{
sfnt_LTSHHead   *lhead;
uint8                           *ypels;
uint16                  i;

	lhead = (sfnt_LTSHHead *) ltsh;
	printf( " 'LTSH' Version:          %u\n", SWAPW(lhead->version) );
	printf( " Number of Glyphs:        %u\n", lhead->numGlyphs=SWAPW(lhead->numGlyphs) );
	printf( "\n   Glyph #   Threshold\n   -------   ---------\n");

	ypels = (uint8 *) ((char *) lhead + sizeof(sfnt_LTSHHead));
	for (i=0 ; i<lhead->numGlyphs ; i++)
		printf( "%9u. %11u\n", i, (uint16) ypels[i]);

	return( DMP_OK );
} /* LTSH_Show() */


void HexDump( buf, len )
char                            *buf;
unsigned long   len;
{
unsigned char   ch, chpos;
unsigned long   pos;
char                            cbuffer[17];

	chpos = 0;
	pos = 0;
	printf( "%04X ", pos );
	
	for( pos=0; pos<len; pos++ )
		{
		ch = buf[pos];
		if( chpos == 16 )
			{
			/* dump last char buffer */
			cbuffer[chpos] = '\0';
			printf( "   %.16s\n", cbuffer );
			chpos = 0;

			/* start next line */
			printf( "%04X ", pos );
			}

		printf("%c%02X", 8 == chpos ? '-' : ' ', ch);
		cbuffer[chpos++] = (unsigned char)(ch < ' ' || ch > 0x7E ? '.' : ch);
		}

} /* HexDump() */
