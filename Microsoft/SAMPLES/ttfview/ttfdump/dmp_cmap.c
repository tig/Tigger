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
/*																					*/
/*	Module: dmp_cmap.c                                          */
/*-------------------------------------------------------------*/
/*	Written by: Rich Taylor                Date: 10/5/90			*/
/*                                                             */
/* Description: Dump TrueType 'cmap' table                     */
/*                                                             */
/* Procedures:                                                 */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*		--------------------- Includes -----------------------	*/

#include <stdio.h>
#include	<io.h>
#include	<malloc.h>

#include	"ttfdump.h"

/*		---------------------- Defines -----------------------	*/

/*		----------- Local Variables and structures -----------	*/
/*							should be declared static						*/

/*		------------------ Local Functions -------------------	*/
/*						should be declared static NEAR					*/

static int	dump_fmt0( sfnt_mappingTable * );
static int	dump_fmt4( sfnt_mappingTable * );

/*		------------------ Global Functions ------------------	*/
/*						should be declared FAR PASCAL						*/

extern float	FixedPrint( long, char * );
extern void		HexDump( char *, unsigned long );
extern unsigned long	FindOffset( long, t2dmp_OffsetTable * , uint32 * );
extern int CheckOneTag(long);

int cmap_Dump( int, t2dmp_OffsetTable * );


int cmap_Dump( nIn, T2Table )
int					nIn;			/* file handle */
t2dmp_OffsetTable	*T2Table;	/* offset table pointer */
{
uint16				i;
long					tag = tag_CharToIndexMap;
unsigned long		offset, length, x;

sfnt_char2IndexDirectory	*cmap;

	if ( !CheckOneTag(tag) )
		return( DMP_OK );

	printf( "\n'cmap' Table - Character To Index Map\n-------------------------------------\n" );

	if( (uint32) NULL == (offset = FindOffset( tag, T2Table, &length )) )
		{
		printf( "   !!! MISSING !!!\n" );
		return( DMP_NOTPRESENT );
		}
	else
		{
		/* Allocate memory for cmap */
		if( NULL == (cmap = malloc( length )) )
			{
			printf( "Error (1) allocating memory for table.\n" );
			return( DMP_ERR );
			}

		/* Load Table */
		if( offset != (uint32) lseek( nIn, offset, SEEK_SET ) )
			{
			printf( "Error seeking to start of table\n" );
			return( DMP_ERR );
			}
		if( length != (x=(uint32) read( nIn, (void *)cmap, (uint16) length )) )
			{
			printf("Error reading table, read %lu bytes, expected %lu\n", x, length );
			return( DMP_ERR );
			}

		printf( "Size = %d bytes\n", length );
		printf( "  'cmap' version:  %u\n", SWAPW(cmap->version) );

		cmap->numTables = SWAPW(cmap->numTables);
		printf( "  numTables:       %u\n", cmap->numTables );

		for( i=0; i<cmap->numTables; i++ )
			{
			sfnt_mappingTable		*map;

			cmap->platform[i].platformID = SWAPW(cmap->platform[i].platformID);
			cmap->platform[i].specificID = SWAPW(cmap->platform[i].specificID);
			cmap->platform[i].offset = SWAPL(cmap->platform[i].offset);

			printf( "  \nSubtable %2d.   Platform ID:   %u\n", i+1, cmap->platform[i].platformID );
			printf( "               Specific ID:   %u\n", cmap->platform[i].specificID );
			printf( "               'cmap' Offset: 0x%08lX\n", cmap->platform[i].offset );

			map = (sfnt_mappingTable *)((char *)cmap + cmap->platform[i].offset);
			map->format = SWAPW(map->format);
			map->length = SWAPW(map->length);
			map->version = SWAPW(map->version);

			switch( map->format )
				{
				case 0:
					printf( "\t      ->Format:\t%u : Byte encoding table\n", map->format );
					break;

				case 2:
					printf( "\t      ->Format:\t%u : High-byte mapping through table\n", map->format );
					break;

				case 4:
					printf( "\t      ->Format:\t%u : Segment mapping to delta values\n", map->format );
					break;

				case 6:
					printf( "\t      ->Format:\t%u : Trimmed table mapping\n", map->format );
					break;
				}
			printf( "\t\tLength:\t\t%u\n", map->length );
			printf( "\t\tVersion:\t%u\n", map->version );

			switch( map->format )
				{
				case 0:
					dump_fmt0( map );
					break;

				case 4:
					dump_fmt4( map );
					break;

				case 2:
				case 6:
					printf( "Dump not implemented in dmp_cmap.c for this table format...\n" );
					break;
				}

			/* At end of switch, swap things back (in case CMAP subtables resused) */

			map->format = SWAPW(map->format);
			map->length = SWAPW(map->length);
			map->version = SWAPW(map->version);
			}

		free( cmap );
		}

	return( DMP_OK );

} /* cmap_Dump() */


static int dump_fmt0( map )
sfnt_mappingTable	*map;
{
register	int	i;
uint8	*glyphIdArray;

	glyphIdArray = (uint8 *)(&map[1]);

	for( i=0; i<256; i++ )
		printf( "\n\t\tChar %3u -> Index %u", i, (uint8)glyphIdArray[i] );

	printf( "\n" );

	return( DMP_OK );

} /* dump_fmt0() */


static int dump_fmt4( map )
sfnt_mappingTable	*map;
{
unsigned register	j;
register          i;
int				segCount;
uint16			*endCount, *startCount, *idRangeOffset;
int16				*idDelta;
struct sfnt_cmap4hdr
	{
	uint16	segCountX2;
	uint16	searchRange;
	uint16	entrySelector;
	uint16	rangeShift;
	uint16	endCount[1];
	} *cmap4hdr;

	cmap4hdr = (struct sfnt_cmap4hdr *)(&map[1]);

	segCount = SWAPW(cmap4hdr->segCountX2) >> 1;
	printf( "\t\tsegCount:\t%u  (X2 = %u)\n", segCount, segCount*2 );
	printf( "\t\tsearchRange:\t%u\n", SWAPW(cmap4hdr->searchRange) );
	printf( "\t\tentrySelector:\t%u\n", SWAPW(cmap4hdr->entrySelector) );
	printf( "\t\trangeShift:\t%u\n", SWAPW(cmap4hdr->rangeShift) );

	endCount = (uint16 *)(&(cmap4hdr->endCount[0]));
	startCount = endCount + (segCount + 1);
	idDelta = startCount + segCount;
	idRangeOffset = idDelta + segCount;
	for( i=0; i<segCount; i++ )
		printf( "\t\tSeg %3d : St = %04X, En = %04X, Delta = %4d, RangeOff = %5u\n",
					i+1,
					startCount[i] = SWAPW(startCount[i]),
					endCount[i] = SWAPW(endCount[i]),
					idDelta[i] = SWAPW(idDelta[i]),
					idRangeOffset[i] = SWAPW(idRangeOffset[i]) );

	printf( "\n\t\tWhich Means:\n" );
	for( i=0; i<segCount; i++ )
		{
		if( 0xFFFF == endCount[i] )
			break;

		if( 0 == idRangeOffset[i] )
			{
			printf( "\t\t%3d. Char %04X -> Index %u\n", i+1, startCount[i],
							(int)(startCount[i] + idDelta[i]) );
			for( j=startCount[i]+1; j<=endCount[i]; j++ )
				printf( "\t\t     Char %04X -> Index %u\n", j,
					     (int)(j + idDelta[i]) );
			}
		else
			{
			printf( "\t\t%3d. Char %04X -> Index %u\n", i+1, startCount[i], 
					      SWAPW(*(idRangeOffset[i]/2 + &idRangeOffset[i])) );
			for( j=startCount[i]+1; j<=endCount[i]; j++ )
				printf( "\t\t     Char %04X -> Index %u\n", j,
					      SWAPW(*(idRangeOffset[i]/2 + (j - startCount[i]) + &idRangeOffset[i])) );
			}

		/* Unswap the values in case table is used again */
		startCount[i] = SWAPW(startCount[i]);
		endCount[i] = SWAPW(endCount[i]);
		idDelta[i] = SWAPW(idDelta[i]);
		idRangeOffset[i] = SWAPW(idRangeOffset[i]);
		}

	return( DMP_OK );

} /* dump_fmt4() */


