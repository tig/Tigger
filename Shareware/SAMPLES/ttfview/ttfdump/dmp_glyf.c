/***************************************************************/
/*  Copyright 1989, ZSoft Corporation                                                                   */
/*  All Rights Reserved.                                                                                                        */
/*                                                                                                                                                                      */
/*  This material is the proprietary and trade secret                           */
/*  information of ZSoft Corporation and may not be                             */
/*  used by companies or individuals without its written                        */
/*  authorization.  Reproduction by or disclosure to                            */
/*  unauthorized personnel is strictly prohibited.                                      */
/***************************************************************/
/*      Module: dmp_glyf.c                                          */
/*-------------------------------------------------------------*/
/*      Written by: Rich Taylor                Date: 10/8/90                    */
/*                                                             */
/* Description: Dump TrueType 'loca' and 'glyf' tables                  */
/*                                                             */
/* Procedures: glyf_Dump()                                     */
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

/*              ---------------------- Defines -----------------------  */

typedef struct
	{
	int16           numberOfContours;
	FWord           xMin;
	FWord           yMin;
	FWord           xMax;
	FWord           yMax;
	uint16  endPtsOfContours[1];
	} sfnt_glyfChar;

/*              ----------- Local Variables and structures -----------  */
/*                                                      should be declared static                                               */

/*              ------------------ Local Functions -------------------  */
/*                                              should be declared static NEAR                                  */

static int      dump_normalChar( sfnt_glyfChar * );
static int      dump_compChar( char * );
static void     F2Dot14Print( unsigned int );

/*              ------------------ Global Functions ------------------  */
/*                                              should be declared FAR PASCAL                                           */

extern void             HexDump( char *, unsigned long );
extern unsigned long    FindOffset( long, t2dmp_OffsetTable * , uint32 * );
extern void             TTIUnAsm( unsigned char *, unsigned int );
extern int					CheckOneTag( long );

int glyf_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable *, headparm *, int );


int glyf_Dump( nIn, T2Table, maxp, headParm, nDumpChar )
int                                             nIn;                    /* file handle */
t2dmp_OffsetTable               *T2Table;       /* offset table pointer */
sfnt_maxProfileTable    *maxp;
headparm                                        *headParm;
int                                             nDumpChar;
{
int                                     flag;
uint16											 i;
int                                     nRC;
long                                    ltag = tag_IndexToLoc, gtag = tag_GlyphData;
unsigned long           offset, length, x;

char                                    *loca;          /* pointer to 'loca' block */
uint32                          *goffset;       /* pointer to offset table */

	/* First, dump the 'loca' table */

	if (flag = CheckOneTag(ltag) )
		printf( "\n'loca' Table - Index To Location Table\n--------------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( ltag, T2Table, &length )) )
		{
		if (flag)
			printf( "   !!! 'loca' MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate memory for loca */
		if( NULL == (loca = malloc( length )) )
			{
			if (flag)
				printf( "Error (1) allocating memory for table.\n" );
			return( DMP_ERR );
			}

		/* Allocate memory for offset table */
		if( NULL == (goffset = malloc( (maxp->numGlyphs+1)*sizeof(uint32) )) )
			{
			if (flag)
				printf( "Error (2) allocating memory for table.\n" );
			return( DMP_ERR );
			}

		/* Load Table */
		if( offset != (uint32) lseek( nIn, (int32) offset, SEEK_SET ) )
			{
			if (flag)
				printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32)read( nIn, (void *)loca, (uint16) length )) )
			{
			if (flag)
				printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		if (flag) 
			printf( "Size = %lu bytes, %u entries\n", length, maxp->numGlyphs+1 );

		for( i=0; i < maxp->numGlyphs ; i++ )
			{
			if( headParm->indexToLocFormat )
				{       /* Long Format */
				goffset[i] = SWAPL(((uint32 *)loca)[i]);
				if (flag && i)
					if (goffset[i] == goffset[i-1])
						printf( "* No contours *\n");
					else
						printf( "\n" );
				if (flag)
					printf( "\tIdx %3u -> glyfOff 0x%08lX", i, goffset[i] );
				}
			else
				{       /* Short Format */
				goffset[i] = 2L * (uint32) SWAPW(((uint16 *)loca)[i]);
				if (flag && i)
					if (goffset[i] == goffset[i-1])
						printf( "* No contours *\n");
					else
						printf( "\n" );
				if (flag)
					printf( "\tIdx %3u -> glyfOff 0x%05lX", i, goffset[i] );
				}
			}	/* end of FOR */

		/* Compute the LAST value in the offset table */
		if( headParm->indexToLocFormat )
			{
			goffset[i] = SWAPL(((uint32 *)loca)[i]);
			if (flag)
				{
				if (goffset[i] == goffset[i-1])
					printf( "* No contours *\n");
				else
					printf( "\n" );
				printf( "\t           Ends at 0x%08lX\n", goffset[i] );
				}
			}
		else
			{
			goffset[i] = 2L * (uint32) SWAPW(((uint16 *)loca)[i]);
			if (flag)
				{
				if (goffset[i] == goffset[i-1])
					printf( "* No contours *\n");
				else
					printf( "\n" );
				printf( "\t           Ends at 0x%05lX\n", goffset[i] );
				}
			}

		free( loca );
		}

	/* Find and dump 'glyf' table */
	if (nDumpChar != -2)
		printf( "\n'glyf' Table - Glyph Data\n-------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( gtag, T2Table, &length )) )
		{
		printf( "   !!! 'glyf' MISSING !!!\n" );
		free( goffset );
		return( DMP_NOTPRESENT );
		}
	else if( nDumpChar != -2)
		/* If nDumpChar is -2, suppress glyph output */
		/* If it's -1, dump all glyphs */
		/* Otherwise, just dump 1 glyph (if out of range, dump all) */
		{
		printf( "Size = %lu bytes, %u entries\n", length, maxp->numGlyphs );
		/* Last entry in 'loca' is NOT A GLYPH! */
		for( i=0; i < maxp->numGlyphs ; i++ )
			{
			uint32  ulGlyfLen;

			if( nDumpChar != -1  &&  i != (uint16) nDumpChar )
				continue;

			ulGlyfLen = goffset[i+1] - goffset[i];
			if( ulGlyfLen )
				{
				sfnt_glyfChar *glyfChar;

				printf( "\tGlyph %3d: off = 0x%08lX, len = %lu\n", i, goffset[i], ulGlyfLen );

				if( ulGlyfLen > 0x0000FFFF )
					printf( "\t\tCharacter larger than 64K!  Need to rewrite DMP_GLYF.C\n" );
				else
					{
					/* Allocate memory for glyph */
					if( NULL == (glyfChar = (sfnt_glyfChar *)malloc( (uint16)(ulGlyfLen&0x0000FFFF) )) )
						{
						printf( "Error (1) allocating memory for glyph.\n" );
						free( goffset );
						return( DMP_ERR );
						}

					/* Load one glyph */
					if( (offset+goffset[i]) != (uint32) lseek( nIn, offset+goffset[i], SEEK_SET ) )
						{
						printf( "Error seeking to start of glyph.\n" );
						free( goffset );
						free( glyfChar );
						return( DMP_ERR );
						}
					if(ulGlyfLen  != (x=(uint32) read( nIn, (void *)glyfChar, (uint16)ulGlyfLen)) )
						{
						printf("Error reading glyph, read %lu bytes, expected %lu\n", x, ulGlyfLen );
						free( goffset );
						free( glyfChar );
						return( DMP_ERR );
						}

					/* Display Glyph Header */
					glyfChar->numberOfContours = SWAPW(glyfChar->numberOfContours);
					printf( "\t  numberOfContours:\t%d", glyfChar->numberOfContours );
					if( glyfChar->numberOfContours < 0 )
						printf( "  (Composite)\n" );
					else
						printf( "\n" );
					printf( "\t  xMin:\t\t\t%d\n", SWAPW(glyfChar->xMin) );
					printf( "\t  yMin:\t\t\t%d\n", SWAPW(glyfChar->yMin) );
					printf( "\t  xMax:\t\t\t%d\n", SWAPW(glyfChar->xMax) );
					printf( "\t  yMax:\t\t\t%d\n", SWAPW(glyfChar->yMax) );

					if( 0 > glyfChar->numberOfContours )
						dump_compChar( (char *)&(glyfChar->endPtsOfContours[0]) );
					else
						if( nRC = dump_normalChar( glyfChar ) )
							{
							free( glyfChar );
							free( goffset );
							return( nRC );
							}

					printf( "\n" );
					free( glyfChar );
					}
				}
			else
				printf( "\tGlyph %3d: off = 0x%08lX, len = 0\n\n", i, goffset[i] );

			}       /* Glyph Dump Loop */
		}       /* ELSE that starts Glyph Dump loop */

	free( goffset );

	return( DMP_OK );

} /* glyf_Dump() */


static int      dump_normalChar( glyfChar )
sfnt_glyfChar           *glyfChar;
{
unsigned register       i;
uint16                  nPoints, uInst;
uint16                  counter;

uint8                           *pbyFlag, *pbyCurFlag, *pbySrc, *pbyInst;
int16                           *xVal, *yVal, nX, nY;

	/* Dump Endpoints */
	printf( "\n\tEndPoints\n\t---------\n" );
	for( i=0; i < (uint16) glyfChar->numberOfContours; i++ )
		printf( "\t%3d:  %u\n", i, SWAPW(glyfChar->endPtsOfContours[i]) );

	nPoints = SWAPW(glyfChar->endPtsOfContours[i-1]) + 1;   /* last one */

	/* Instructions */
	pbyInst = (uint8 *)(&(glyfChar->endPtsOfContours[glyfChar->numberOfContours]));
	uInst = SWAPW(*(uint16 *)pbyInst);
	printf( "\n\t  Length of Instructions:\t%u\n", uInst );
	pbyInst += 2;

	/* Dis-asm instructions */
	if( uInst )
		TTIUnAsm( pbyInst, uInst );

	printf( "\n" );

	/* Allocate contiguous space for flags and values */
	if( NULL == (pbyFlag = malloc( nPoints * 5 )) ) /* 5 = 1 flag + 2 uint16's */
		{
		printf( "Error (2) allocating memory for glyph.\n" );
		return( DMP_ERR );
		}

	/* parse & print flag data */
	printf( "\tFlags\n\t-----\n" );
	pbySrc = pbyInst + uInst;
	pbyCurFlag = pbyFlag;
	counter = 0;
	while( counter < nPoints )
		{
		pbyCurFlag[0] = *pbySrc++;      /* get next flag */

		printf( "\t%3d:  ", counter );
		counter++;

		if( pbyCurFlag[0] & NEXT_Y_IS_ZERO )
			printf( "YDual " );
		else
			printf( "      " );

		if( pbyCurFlag[0] & NEXT_X_IS_ZERO )
			printf( "XDual " );
		else
			printf( "      " );

		if( pbyCurFlag[0] & REPEAT_FLAGS )
			printf( "Rep-%3u ", (uint16)*pbySrc );
		else
			printf( "        " );

		if( pbyCurFlag[0] & YSHORT )
			printf( "Y-Short " );
		else
			printf( "        " );

		if( pbyCurFlag[0] & XSHORT )
			printf( "X-Short " );
		else
			printf( "        " );

		if( pbyCurFlag[0] & ONCURVE )
			printf( "On\n" );
		else
			printf( "Off\n" );

		if( pbyCurFlag[0] & REPEAT_FLAGS )
			{
			uint8           byFlagCnt = *pbySrc++;

			for( i=0; i < byFlagCnt; i++ )
				pbyCurFlag[i+1] = pbyCurFlag[0];

			pbyCurFlag += (byFlagCnt + 1);
			counter += byFlagCnt;
			}
		else
			pbyCurFlag++;

		}       /* flag decode loop */

	printf( "\n" );

	/* load X coordinates */
	xVal = (int *)&(pbyFlag[nPoints]);
	for( i=0; i < nPoints; i++ )
		if( pbyFlag[i] & XSHORT )
			{
			xVal[i] = *pbySrc++;

			if( !(pbyFlag[i] & NEXT_X_IS_ZERO) )
				xVal[i] *= -1;
			}
		else
			{
			if( pbyFlag[i] & NEXT_X_IS_ZERO )
				xVal[i] = 0;
			else
				{
				xVal[i] = SWAPW(*((int *)pbySrc));
				pbySrc += 2;
				}
			}

	/* load Y coordinates */
	yVal = &(xVal[nPoints]);
	for( i=0; i < nPoints; i++ )
		if( pbyFlag[i] & YSHORT )
			{
			yVal[i] = *pbySrc++;

			if( !(pbyFlag[i] & NEXT_Y_IS_ZERO) )
				yVal[i] *= -1;
			}
		else
			{
			if( pbyFlag[i] & NEXT_Y_IS_ZERO )
				yVal[i] = 0;
			else
				{
				yVal[i] = SWAPW(*((int *)pbySrc));
				pbySrc += 2;
				}
			}

	/* print coordinate values */
	printf( "\tCoordinates\n\t-----------\n" );
	nX = nY = 0;
	for( i=0; i < nPoints; i++ )
		{
		nX += xVal[i];
		nY += yVal[i];
		printf( "\t%3d: Rel (%6d,%6d)  ->  Abs (%6d,%6d)\n", i, xVal[i], yVal[i], nX, nY );
		}

	free( pbyFlag );

	return( DMP_OK );

} /* dump_normalChar() */


static int      dump_compChar( compChar )
char   *compChar;
{
int             i;
uint16  uflags, uglyfindx, uval;

	/* loop to read composite component data */
	i = 0;
	do
		{
		/* get flags and glyph index */
		uflags = SWAPW(*(uint16 *)compChar);
		compChar += 2;
		uglyfindx = SWAPW(*(uint16 *)compChar);
		compChar += 2;

		/* display flags and index */
		printf( "\n\t     %d: Flags:\t\t0x%04X\n", i, uflags );
		printf( "\t\tGlyf Index:\t%u\n", uglyfindx );

		/* get and display arguments */
		uval = SWAPW(*(uint16 *)compChar);
		compChar += 2;
		if( uflags & ARG_1_AND_2_ARE_WORDS )
			{
			if (uflags & ARGS_ARE_XY_VALUES)
				{
				/* We are dealing with signed offsets */
				printf( "\t\tX WOffset:\t%d\n", uval );
				uval = SWAPW(*(uint16 *)compChar);
				printf( "\t\tY WOffset:\t%d\n", uval );
				}
			else
				{
				printf( "\t\tAnchor WPoint 1:\t%u\n", uval );
				uval = SWAPW(*(uint16 *)compChar);
				printf( "\t\tAnchor WPoint 2:\t%u\n", uval );
				}
			compChar += 2;
			}
		else
			{
			if (uflags & ARGS_ARE_XY_VALUES)
				{
				/* We are dealing with signed offsets */
				printf( "\t\tX BOffset:\t%d\n", (int16) uval/256 );
				uval = SWAPW(*(uint16 *)compChar);
				printf( "\t\tY BOffset:\t%d\n", (int16) uval%256 );
				}
			else
				{
				printf( "\t\tAnchor BPoint 1:\t%u\n", uval>>8 );
				uval = SWAPW(*(uint16 *)compChar);
				printf( "\t\tAnchor BPoint 2:\t%u\n", uval&0x00FF );
				}
			}

		/* Handle scale possibilites */
		if( uflags & WE_HAVE_AN_X_AND_Y_SCALE )
			{
			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tX Scale:\t" );
			F2Dot14Print( uval );
			
			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tY Scale:\t" );
			F2Dot14Print( uval );
			}
		else if( uflags & WE_HAVE_A_SCALE )
			{
			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tX,Y Scale:\t" );
			F2Dot14Print( uval );
			}
		else if ( uflags & WE_HAVE_A_TWO_BY_TWO )
			{
			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tX Scale:\t" );
			F2Dot14Print( uval );
			
			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tScale 01:\t" );
			F2Dot14Print( uval );

			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tScale 10:\t" );
			F2Dot14Print( uval );
			
			uval = SWAPW(*(uint16 *)compChar);
			compChar += 2;
			printf( "\t\tY Scale:\t" );
			F2Dot14Print( uval );
			}
		/* Deal with other flags */
		if (uflags & ROUND_XY_TO_GRID)
			printf("\t\tOther:\t\tRound X,Y to Grid");
		else
			printf("\t\tOther:\t\t                 ");
		if (uflags & NON_OVERLAPPING)
			printf("  No Overlap");
		else
			printf("            ");
		if (uflags & USE_MY_METRICS)
			printf("  Use My Metrics\n\n");
		else
			printf("\n\n");

		i++;
		} while( uflags & MORE_COMPONENTS );

	/* glyph instructions - if any */
	if( uflags & WE_HAVE_INSTRUCTIONS )
		{
		printf( "\tInstructions:\n\t-------------\n" );

		uval = SWAPW(*(uint16 *)compChar);
		compChar += 2;

		TTIUnAsm( compChar, uval );
		}

	return( DMP_OK );

} /* dump_compChar() */


static void     F2Dot14Print( val )
unsigned int    val;
{
	
float    mant[4] = {0.0F,1.0F,-2.0F,-1.0F};
float           f;

/**********************
The assumption in here is that the 2 hi bits in the word are the sign/mantissa:

	00 =  0
	01 =  1
	11 = -1
	10 = -2

The low 14 bits are a fraction, expressed in 16384ths.  This fractional portion
is UNSIGNED, and is simply added to the mantissa to yield the final value.  Thus
the most negative value is -2.000 == 0xC000, while the largest value is
1.999939 == 0x7FFF.
***************************/

	f = (float) (val & 0x3FFF) / 16384.;    /* fractional portion */
	f += mant[(val & 0xC000) >> 14];        /* mantissa */

	printf( "%9.6f\n",f);
}
