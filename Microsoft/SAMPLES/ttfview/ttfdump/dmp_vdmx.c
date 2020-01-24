/***************************************************************/
/*  Copyright 1991 Microsoft Corp.										*/
/*  All Rights Reserved.													*/
/*																					*/
/*  This material is the proprietary and trade secret				*/
/*  information of Microsoft Corporation and may not be			*/
/*  used by companies or individuals without its written			*/
/*  authorization.  Reproduction by or disclosure to				*/
/*  unauthorized personnel is strictly prohibited.					*/
/***************************************************************/
/*                                                             */
/*      Module: dmp_VDMX.c                                     */
/*-------------------------------------------------------------*/
/*      Written by: Dennis Adler               Date: 7/29/91   */
/*                                                             */
/* Description: Dump the 'VDMX' table for a TrueType font      */
/*                                                             */
/* Procedures:                                                 */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*     --------------------- Includes -----------------------  */

#include		<stdio.h>
#include		<io.h>
#include		<malloc.h>
#include		<stdlib.h>

#include    "ttfdump.h"

/*     ------------------ Global Functions ------------------  */

extern float    FixedPrint( long, char * );
extern int		 CheckOneTag( long );
extern unsigned long    FindOffset( long, t2dmp_OffsetTable *, long * );

int VDMX_Dump( int, t2dmp_OffsetTable * );


int VDMX_Dump( nIn, T2Table )
int						nIn;				/* file handle */
t2dmp_OffsetTable		*T2Table;		/* offset table pointer */

{
uint16					i, j;
int32						tag = 0x56444d58;		/* 'VDMX' */
uint32					offset, length, x;
sfnt_VDMX				*VDMX;
vdmx_HgtRecs			**hgts, *tmp;
vdmx_Ratios				*ratios;
uint16					*offsets;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'VDMX' Table - Precomputed Vertical Device Metrics\n--------------------------------------------------\n");
	if( (uint32)NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}

	/* Load Table */
	if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
		{
		printf( "Error seeking to start of table\n" );
		return( DMP_ERR );
		}

	/* Allocate memory */
	if( NULL == (VDMX = malloc( length )) )
		{
		printf( "Error (1) allocating memory for table.\n" );
		return( DMP_ERR );
		}

	if( length != (x= (uint32) read( nIn, (void *)VDMX, (uint16) length )) )
		{
		printf("Error reading table, read %lu bytes, expected %lu\n",
			x, length);
		free( VDMX );
		return( DMP_ERR );
		}

	/* Allocate space for height record pointers */
	VDMX->numRecs = SWAPW(VDMX->numRecs);
	if ( NULL == (hgts = (vdmx_HgtRecs **) malloc( sizeof (vdmx_HgtRecs *) * VDMX->numRecs)) )
		{
		printf( "Error (2) allocating memory for table.\n");
		free( VDMX );
		return( DMP_ERR );
		}

	printf( "  Version:                 %u\n", SWAPW(VDMX->version) );
	printf( "  Number of Hgt Records:   %u\n", VDMX->numRecs );
	printf( "  Number of Ratio Records: %u\n", VDMX->numRatios= SWAPW(VDMX->numRatios) );

	/* Initialize the pointers for the data structures within the VDMX table */
	ratios = (vdmx_Ratios *) &VDMX->data;
	offsets = (uint16 *)( (uint8 *)ratios + sizeof(vdmx_Ratios) * VDMX->numRatios );
	hgts[0] = (vdmx_HgtRecs *)( (uint8 *) offsets + sizeof(uint16) * VDMX->numRatios );

	/* Build the pointer list for the height records */
	for ( i=1 ; i < VDMX->numRecs ; i++)
		hgts[i] = (vdmx_HgtRecs *)( (uint8 *) hgts[i-1] + (SWAPW(hgts[i-1]->numRecs)-1)*
					 sizeof(vdmx_HgtTable) + sizeof(vdmx_HgtRecs) );
	

	/* Loop for each VDMX ratio record */
	for( i=0 ; i < VDMX->numRatios ; i++)
		{
		printf( "\n    Ratio Record #%u\n", i+1);
		printf( "\tCharSetId     %u\n", (uint16) ratios[i].bCharSet );
		printf( "\txRatio        %u\n", (uint16) ratios[i].xRatio );
		printf( "\tyStartRatio   %u\n", (uint16) ratios[i].yStartRatio );
		printf( "\tyEndRatio     %u\n", (uint16) ratios[i].yEndRatio );
		printf( "\tRecord Offset %u ", j=SWAPW(offsets[i]) );
	
		/* Figure out which VDMX height record number this is */
		tmp = (vdmx_HgtRecs *) ( (uint8 *)VDMX + j );
		for (j=0 ; j < VDMX->numRecs ; j++)
			if (hgts[j] == tmp) break;
		if (j < VDMX->numRecs)
			printf("(group #%u)\n\n", j+1);
		else
			printf("* INVALID OFFSET *\n\n");


		}	/* End of FOR on VDMX->numRatios */

	/* Print the VDMX Height Record Groups */
	printf( "   VDMX Height Record Groups\n   -------------------------");
	for( i=0 ; i < VDMX->numRecs ; i++)
		{
		tmp = hgts[i];
		printf( "\n   %u.\tNumber of Hgt Records  %u\n", i+1, tmp->numRecs= SWAPW(tmp->numRecs) );
		printf( "\tStarting Y Pel Height  %u\n", (uint16) tmp->startsz );
		printf( "\tEnding Y Pel Height    %u\n\n", (uint16) tmp->endsz );

		for( j=0 ; j < tmp->numRecs ; j++)
			{
			printf( "\t   %3d. Pel Height= %u\n", j+1, SWAPW(tmp->recs[j].yPelHeight) );
			printf( "\t        yMax=       %d\n", SWAPW(tmp->recs[j].yMax) );
			printf( "\t        yMin=       %d\n\n", SWAPW(tmp->recs[j].yMin) );
			}
		}

	printf( "\n");
	free( VDMX );
	free( hgts );
	return( DMP_OK );
}
