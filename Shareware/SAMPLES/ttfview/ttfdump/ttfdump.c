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
/*      Module: ttfdump.c                                          */
/*-------------------------------------------------------------*/
/*      Written by: Rich Taylor                Date: 9/25/90                    */
/*                                                             */
/* Description: TrueType v1.0 Font Dump Program Main           */
/*                                                             */
/* Procedures: main()                                          */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*              --------------------- Includes -----------------------  */

#include        <stdio.h>
#include <io.h>
#include        <fcntl.h>
#include        <stdlib.h>
#include <string.h>

#include        "ttfdump.h"

/*              ---------------------- Defines -----------------------  */

/*              ----------- Local Variables and structures -----------  */
/*                                                      should be declared static                                               */
long *oneTag = NULL;
int  oneTagCount = 0;

/*              ------------------ Local Functions -------------------  */
/*                                              should be declared static NEAR                                  */

void    main( int, char **, char ** );
float   FixedPrint( long, char * );
unsigned long   FindOffset( long, t2dmp_OffsetTable * , uint32 * );
int CheckOneTag (long tag);


/*              ------------------ Global Functions ------------------  */
/*                                              should be declared FAR PASCAL                                           */

extern int OS2_Dump(  int, t2dmp_OffsetTable * );
extern int cvt_Dump(  int, t2dmp_OffsetTable * );
extern int fpgm_Dump( int, t2dmp_OffsetTable * );
extern int hdmx_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable * );
extern int head_Dump( int, t2dmp_OffsetTable *, headparm *, int );
extern int hhea_Dump( int, t2dmp_OffsetTable *, uint16 * );
extern int hmtx_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable *, uint16 );
extern int maxp_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable * );
extern int name_Dump( int, t2dmp_OffsetTable * );
extern int post_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable * );
extern int prep_Dump( int, t2dmp_OffsetTable * );
extern int cmap_Dump( int, t2dmp_OffsetTable * );
extern int glyf_Dump( int, t2dmp_OffsetTable *, sfnt_maxProfileTable *, headparm *, int );
extern int kern_Dump( int, t2dmp_OffsetTable * );
extern int LTSH_Dump( int, t2dmp_OffsetTable * );
extern int VDMX_Dump( int, t2dmp_OffsetTable * );
extern void HexDump ( char *, unsigned long);


void    main( argc, argv, envp )
int     argc;
char    *argv[];
char    *envp[];
{
unsigned register    i;
int             nIn, j;
int             nDumpChar;
char				 oneTable[4];
char				 *filenm = NULL;
int				 hFlag = 1;
int             timeZone = 8;

t2dmp_OffsetTable                       *T2Table;
uint16                                          numHmtx;
sfnt_maxProfileTable            maxp;
headparm                                                headParm;

	/* Omnipresent banner */
	printf( "\nTrueType v1.0 Dump Program - v1.46, 1/27/92, rrt, dra\n" );
	printf( "Copyright (C) 1991 ZSoft Corporation. All rights reserved.\n" );
	printf( "Portions Copyright (C) 1991 Microsoft Corporation. All rights reserved.\n\n");

	/* Check command line args */
	if( 2 > argc )
		{
		printf( "Usage: TTFDUMP <filename> [-nNNNN] [-tCCCC] [-h] [-zHHH]\n" 
		   "\t<filename>  - TrueType .TTF (or .T2 or .ROY) filename\n"
		   "\t-nNNNN      - NNNN = glyph index number to dump (decimal value)\n"
		   "\t                     (may also be 'x' to suppress glyph dumping).\n"
		   "\t-tCCCC      - CCCC = table name (1-4 characters) to dump.\n"
		   "\t-h          - Supress the font file header information.\n"
		   "\t-zHHH       - Time zone difference from Greenwich Mean Time; used\n"
         "\t              to account for hours past GMT (local time). Default\n"
         "\t              is 8 (for Pacific Time). 'HHH' is a signed value.\n"
		);
		exit( 10 );
		}

	/* set defaults */
	nDumpChar = -1; /* dump all glyphs (-2 = dump NO glyphs) */

	/* process command line arguments */
	for( i=1; i< (uint16) argc; i++ ) {
		if( (argv[i][0] == '-' || argv[i][0] == '/') && argv[i][1] ){
			/* We've got an option switch */
			switch( argv[i][1] )
				{
				case 'n':
				case 'N':
					if( argv[i][2] == 'x' || argv[i][2] == 'X')
						nDumpChar = -2;
					else
						{
						nDumpChar = atoi( &(argv[i][2]) );
						if( nDumpChar < 0 )
							{
							printf( "ERROR: Invalid dump character request (%d)\n", nDumpChar );
							exit( 10 );
							}
						}
					break;

				case 't':
				case 'T':
					/* Get token and pad with blanks */
					if( strlen(argv[i]) > 6 || strlen(argv[i]) < 3 )
						{
						printf ("ERROR: Table tokens must be 1-4 characters in length");
						exit( 10 );
						}
					for (j=2 ; argv[i][j] ; j++)
						oneTable[j-2] = argv[i][j];
					for (j-=2 ; j<4 ; j++)
						oneTable[j] = ' ';
					oneTag = realloc( oneTag, sizeof( *oneTag )*(1+oneTagCount) );
					if (oneTag == NULL)
						{
						printf( "Error reallocating memory for '-t' arguments.\n");
						exit(10);
						}
					oneTag[oneTagCount] = SWAPL(* (long *) oneTable );   /* Long table tag */
					oneTagCount++;
					break;

				case 'h':
				case 'H':
					/* Suppress the header information */
					hFlag = 0;
					break;

				case 'z':
				case 'Z':
					/* Set time zone */
					timeZone = atoi( argv[i]+2 );
					break;

				default:
					printf( "NOTE: Didn't recognize argument: %s\n", argv[i] );
					break;
				}  /*** End of SWITCH ***/
			}		/*** End of IF for option switches ***/
		 else
		 	{
			/* This is our file name */
			if (filenm != NULL)
				{
				printf ("Please specify only one file name, not both '%s' and '%s'\n",
					filenm, argv[i]);
				exit( 10 );
				}
			filenm = argv[i];
			}     /*** End of ELSE */
		}			/*** End of FOR loop on arguments ***/


	/* Open file */
	if (filenm == NULL)
		{
		printf( "Error: A file name is required!\n");
		exit( 10 );
		}

	if( -1 == (nIn = open( filenm, O_RDONLY|O_BINARY )) )
		{
		printf( "Error (%d) opening file: %s\n", errno, filenm );
		exit( 10 );
		}
	printf( "Dumping file '%s'\n\n", filenm);

	/* Allocate header space */
	if( NULL == (T2Table = malloc(OFFSETTABLESIZE)) )
		{
		printf( "Error allocting Offset Table memory\n" );
		exit( 10 );
		}

	/* Display Offset Table */
	if( OFFSETTABLESIZE != read( nIn, (void *)T2Table, OFFSETTABLESIZE ) )
		{
		printf( "Error (%d) reading Offset Table Header\n", errno );
		exit( 10 );
		}
	T2Table->numOffsets = SWAPW( T2Table->numOffsets );

	if (hFlag)
		{
		printf( "Offset Table\n------ -----\n" );
		printf( "  sfnt version:   " );
			FixedPrint( T2Table->version, "%5.1f\n" );
		printf( "  numTables =     %5u\n", T2Table->numOffsets );
		printf( "  searchRange =   %5u\n", SWAPW(T2Table->searchRange) );
		printf( "  entrySelector = %5u\n", SWAPW(T2Table->entrySelector) );
		printf( "  rangeShift =    %5u\n\n", SWAPW(T2Table->rangeShift) );
		}

	if( NULL == (T2Table = realloc( (void *)T2Table, OFFSETTABLESIZE + (T2Table->numOffsets * sizeof(t2dmp_DirectoryEntry)) )) )
		{
		printf( "Error reallocting Offset Table memory\n" );
		exit( 10 );
		}
	for( i=0; i<T2Table->numOffsets; i++ )
		{
		if( sizeof(sfnt_DirectoryEntry) != read( nIn, (void *)(&(T2Table->table[i])), sizeof(sfnt_DirectoryEntry) ) )
			{
			printf( "Error (%d) reading Offset Table Entry #%d\n", errno, i );
			exit( 10 );
			}

		/* This aparently inert statement fixes table reading for some
			files...  I just don't know - rrt 1/10/91 */
		lseek( nIn, 0L, SEEK_CUR );
		}

	for( i=0; i<T2Table->numOffsets; i++ )
		{                                                                               
		T2Table->table[i].checkSum = SWAPL(T2Table->table[i].checkSum);
		T2Table->table[i].offset   = SWAPL(T2Table->table[i].offset);
		T2Table->table[i].length   = SWAPL(T2Table->table[i].length);
		T2Table->table[i].dumped   = 0; /* clear dumped flag */

		if (hFlag)
			{
			printf( "%2d. '%.4s'", i, (char *)&T2Table->table[i].tag );
			printf( " - chksm = 0x%08lX", T2Table->table[i].checkSum );
			printf( ", off = 0x%08lX", T2Table->table[i].offset );
			printf( ", len = %6lu\n", T2Table->table[i].length );
			}
		}


	/**** Main Table Dumping ****/
	if( head_Dump( nIn, T2Table, &headParm, timeZone ) )
		exit( 10 );

	if( hhea_Dump( nIn, T2Table, &numHmtx ) )
		exit( 10 );

	if( maxp_Dump( nIn, T2Table, &maxp ) )
		exit( 10 );

	if( 0 < name_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < OS2_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < post_Dump( nIn, T2Table, &maxp ) )
		exit( 10 );

	if( 0 < cmap_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < cvt_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < prep_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < fpgm_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < hmtx_Dump( nIn, T2Table, &maxp, numHmtx ) )
		exit( 10 );

	if( 0 < VDMX_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < LTSH_Dump( nIn, T2Table ) )
		exit( 10 );

	if( 0 < hdmx_Dump( nIn, T2Table, &maxp ) )
		exit( 10 );

	if( 0 < kern_Dump( nIn, T2Table ) )
		exit( 10 );

	/* check requested dump char */
	if( nDumpChar >= (int)maxp.numGlyphs )
		{
		printf( "NOTE: Requested dump character (%d) is out of range (0 - %u)\n",
			nDumpChar, maxp.numGlyphs-1 );
		printf( "      All characters will be dumped\n" );
		nDumpChar = -1;
		}

	if( glyf_Dump( nIn, T2Table, &maxp, &headParm, nDumpChar ) )
		exit( 10 );

	/* Hex Dump of Undumped Tables */
	for( i=0; i<T2Table->numOffsets; i++ )
		if( (! T2Table->table[i].dumped) && CheckOneTag( SWAPL(T2Table->table[i].tag)) )
			{
			printf( "\nHex Dump of Remaining Tables\n----------------------------\n\n" );
			break;
			}

	/* start hex dump with the first table we've not dumped... */

	for( ; i<T2Table->numOffsets; i++ )
		if( (! T2Table->table[i].dumped) && CheckOneTag( SWAPL(T2Table->table[i].tag)) )
			{
			char    *buffer;

			/* Allocate block for table */
			if( NULL == (buffer = malloc( (uint16)(T2Table->table[i].length&0xFFFF) )) )
				printf( "Error allocating memory for table #%d\n", i );
			else
				{
				/* Read Table into block */
				if( T2Table->table[i].offset != (uint32) lseek( nIn, T2Table->table[i].offset, SEEK_SET ) )
					{
					printf( "Error seeking to start of table #%d\n", i );
					goto cleanup;
					}
				if( T2Table->table[i].length != (uint32) read( nIn,
					(void *)buffer, (uint16) T2Table->table[i].length ) )
					{
					printf( "Error (%d) reading table #%d\n", errno, i );
					goto cleanup;
					}

				printf( "Table %2d.", i );
				printf( " - '%.4s'", (char *)&T2Table->table[i].tag );
				printf( ", length = %lu\n", T2Table->table[i].length );

				if( T2Table->table[i].length > 0 )
					{
					HexDump( buffer, T2Table->table[i].length );
					printf( "\n\n" );
					}

				cleanup:
					free( buffer );
				}
			printf( "\n" );
			}


	/* Cleanup */
	free( T2Table );
	close( nIn );

	exit( 0 );

} /* main() */


float	FixedPrint( fxNum, prntStr )
long		fxNum;
char		*prntStr;
{
float		num;

	fxNum = SWAPL(fxNum);
	num = (float)(fxNum >> 16);		/* Sign preserved! */
	num += (float) (fxNum & 0xffffL)/65536.;

	printf( prntStr, num );
	return( num );

} /* FixedPrint() */


unsigned long   FindOffset( tag, T2Table , len )
long                                    tag;
t2dmp_OffsetTable       *T2Table;
uint32                          *len;
{
unsigned register    i;

	tag = SWAPL(tag);

	for( i=0; i<T2Table->numOffsets; i++ )
		if( tag == T2Table->table[i].tag )
			{
			*len = T2Table->table[i].length;
			T2Table->table[i].dumped = 1;   /* did this one */
			return( T2Table->table[i].offset );
			}

	return( 0L );

} /* FindOffset() */


int CheckOneTag (long tag)
{
	/* returns: 0 if you should not print this table
					1 if this table was asked for explicitly
				  -1 if all tables are to be shown
	*/

	register int  i;

	if (oneTagCount)
		{
		for (i = 0; i < oneTagCount ; i++)
			if (oneTag[i] == tag)
				return(1);
		return(0);
		}
	else
		return(-1);
}
