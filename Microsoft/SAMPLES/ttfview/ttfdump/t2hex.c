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
/*	Module: t2hex.c                                             */
/*-------------------------------------------------------------*/
/*	Written by: Rich Taylor                Date: 9/25/90			*/
/*                                                             */
/* Description: TrueType v1.0 Font Dump Program - hex          */
/*                                                             */
/* Procedures: main()                                          */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*		--------------------- Includes -----------------------	*/

#include	<stdio.h>
#include <io.h>
#include	<fcntl.h>
#include	<stdlib.h>

#include	"zttfdump.h"

/*		---------------------- Defines -----------------------	*/

/*		----------- Local Variables and structures -----------	*/
/*							should be declared static						*/

/*		------------------ Local Functions -------------------	*/
/*						should be declared static NEAR					*/

int	main( int, char *[] );
void	FixedPrint( long, char * );
void	HexDump( char *, unsigned long );
unsigned long	FindOffset( long, sfnt_OffsetTable * , uint32 * );


/*		------------------ Global Functions ------------------	*/
/*						should be declared FAR PASCAL						*/

extern int OS2_Dump( int, sfnt_OffsetTable * );
extern int cmap_Dump( int, sfnt_OffsetTable * );
extern int head_Dump( int, sfnt_OffsetTable * );
extern int hhea_Dump( int, sfnt_OffsetTable *, uint16 * );
extern int name_Dump( int, sfnt_OffsetTable * );


int main( argc, argv )
int	argc;
char	*argv[];
{
register int i;
int			nIn;

sfnt_OffsetTable	*T2Table;
uint16				numHmtx;

	/* Omnipresent banner */
	printf( "\nTrueType v1.0 Hex Dump Program - v1.00, 10/5/90, rrt\n" );
	printf( "Copyright (C) 1990 ZSoft Corporation. All rights reserved.\n\n" );

	/* Check command line args */
	if( 2 > argc )
		{
		printf( "Usage: %s <filename>\n", argv[0] );
		printf( "   <filename>  -  TrueType .T2 (or .ROY) file\n" );
		exit( 10 );
		}

	/* Open file */
	if( -1 == (nIn = open( argv[1], O_RDONLY )) )
		{
		printf( "Error (%d) opening file: %s\n", errno, argv[1] );
		exit( 10 );
		}

	/* Allocate header space */
	if( NULL == (T2Table = malloc(OFFSETTABLESIZE)) )
		{
		printf( "Error allocting Offset Table memory\n" );
		exit( 10 );
		}

	/* Display Offset Table */
	if( -1 == read( nIn, (void *)T2Table, OFFSETTABLESIZE ) )
		{
		printf( "Error (%d) reading Offset Table Header\n", errno );
		exit( 10 );
		}
	T2Table->numOffsets = SWAPW( T2Table->numOffsets );

	printf( "Offset Table\n------ -----\n" );
	printf( "  sfnt version:   " );
		FixedPrint( T2Table->version, "%5.1f\n" );
	printf( "  numTables =     %5u\n", T2Table->numOffsets );
	printf( "  searchRange =   %5u\n", SWAPW(T2Table->searchRange) );
	printf( "  entrySelector = %5u\n", SWAPW(T2Table->entrySelector) );
	printf( "  rangeShift =    %5u\n\n", SWAPW(T2Table->rangeShift) );

	if( NULL == (T2Table = realloc( (void *)T2Table, OFFSETTABLESIZE + (T2Table->numOffsets * sizeof(sfnt_DirectoryEntry)) )) )
		{
		printf( "Error reallocting Offset Table memory\n" );
		exit( 10 );
		}
	if( -1 == read( nIn, (void *)&(T2Table->table[0]), T2Table->numOffsets * sizeof(sfnt_DirectoryEntry) ) )
		{
		printf( "Error (%d) reading Offset Table\n", errno );
		exit( 10 );
		}

	for( i=0; i<T2Table->numOffsets; i++ )
		{										
		T2Table->table[i].checkSum = SWAPL(T2Table->table[i].checkSum);
		T2Table->table[i].offset   = SWAPL(T2Table->table[i].offset);
		T2Table->table[i].length   = SWAPL(T2Table->table[i].length);

		printf( "%2d. '%.4s'", i, (char *)&T2Table->table[i].tag );
		printf( " - checkSum = 0x%08lX", T2Table->table[i].checkSum );
		printf( ", offset = 0x%08lX", T2Table->table[i].offset );
		printf( ", length = %6lu\n", T2Table->table[i].length );
		}

	/* Dump Table Data */
	for( i=0; i<T2Table->numOffsets; i++ )
		{
		char	*buffer;

		/* Allocate block for table */
		if( NULL == (buffer = malloc( (uint16)(T2Table->table[i].length&0xFFFF) )) )
			printf( "Error allocating memory for table #%d\n", i );
		else
			{
			/* Read Table into block */
			if( -1L == lseek( nIn, T2Table->table[i].offset, SEEK_SET ) )
				{
				printf( "Error seeking to start of table #%d\n", i );
				goto cleanup;
				}
			if( -1 == read( nIn, (void *)buffer, (uint16)(T2Table->table[i].length&0xFFFF) ) )
				{
				printf( "Error (%d) reading table #%d\n", errno, i );
				goto cleanup;
				}

			printf( "\n\n\nTable %2d.", i );
			printf( " - '%.4s'", (char *)&T2Table->table[i].tag );
			printf( ", length = %lu\n\n", T2Table->table[i].length );

			if( T2Table->table[i].length > 0 )
				HexDump( buffer, T2Table->table[i].length );

			cleanup:
				free( buffer );
			}
		printf( "\n" );
		}

	/* Cleanup */
	free( T2Table );
	close( nIn );

	return 0;

} /* main() */


void	FixedPrint( fxNum, prntStr )
long	fxNum;
char	*prntStr;
{
float	num;

	num = (float)SWAPW((short)fxNum);

	/* figure out fraction stuff later... */

	printf( prntStr, num );

} /* FixedPrint() */


void HexDump( buf, len )
char 				*buf;
unsigned long	len;
{
unsigned char	ch, chpos;
unsigned long	pos;
char				cbuffer[17];

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
		cbuffer[chpos++] = ch < ' ' || ch > 0x7E ? '.' : ch;
		}

} /* HexDump() */



unsigned long	FindOffset( tag, T2Table , len )
long					tag;
sfnt_OffsetTable	*T2Table;
uint32				*len;
{
register int	i;

	tag = SWAPL(tag);

	for( i=0; i<T2Table->numOffsets; i++ )
		if( tag == T2Table->table[i].tag )
			{
		 	*len = T2Table->table[i].length;
			return( T2Table->table[i].offset );
			}

	return( 0L );

} /* FindOffset() */


