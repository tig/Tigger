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
/*	Module: ttiua.c                                             */
/*-------------------------------------------------------------*/
/*	Written by:  Rich Taylor                Date: 1/9/91			*/
/*                                                             */
/* Description: TrueType UnAssemble routine							*/
/*                                                             */
/* Procedures:  TTIUnAsm()                                     */
/*                                                             */
/*-------------------------------------------------------------*/
/* Revisions:                                                  */
/***************************************************************/


/*		--------------------- Includes -----------------------	*/

#include	<stdio.h>
#include <io.h>
#include	<fcntl.h>
#include	<stdlib.h>

#include	"ttfdump.h"

/*		---------------------- Defines -----------------------	*/

/*		----------- Local Variables and structures -----------	*/
/*							should be declared static						*/

#include	"ttidata.h"		/* instruction dump array */


/*		------------------ Local Functions -------------------	*/
/*						should be declared static NEAR					*/

/*		------------------ Global Functions ------------------	*/
/*						should be declared FAR PASCAL						*/


void TTIUnAsm( pbyInst, uIlen )
unsigned char 	*pbyInst;
unsigned int	uIlen;
{
register int	i;

unsigned char	byInst;
unsigned int	uloc;
unsigned int	ucnt;
			int 	nval;

unsigned char *pbyIP;

	if( uIlen == 0 )
		{
		printf( "\tNo Instructions!\n" );
		return;
		}

	pbyIP = pbyInst;
	uloc = 0;
	while ( pbyIP < pbyInst+uIlen )
		{
		byInst = *pbyIP++;


		/* format and print instruction */
		printf( "\t%5u: %-10s ", uloc, iStr[byInst] );

		/* deal with any instruction arguments */
		if( byInst >= 0xb0  && byInst <= 0xb7 )			/*	PUSHB[abc] args */
			{
			/* display PUSHB arguments */
			ucnt = byInst - 0xaf;
			for( i=0; i<ucnt; i++)
				{
				nval = (int)*pbyIP++;
				printf( "%5u ", nval );
				uloc++;
				}
			}
		else if( byInst >= 0xb8  &&  byInst <= 0xbf )	/*	PUSHW[abc] args */
			{
			/* display PUSHW arguments */
			ucnt = byInst - 0xb7;
			for( i=0; i<ucnt; i++)
				{
				nval = *pbyIP++;
				nval <<= 8;
				nval += *pbyIP++;
				uloc += 2;
				printf( "%5d ", nval);
				}
			}
		else if( byInst == 0x40 )								/* NPUSHB[] */
			{
			/* display NPUSHB arguments */
			ucnt = *pbyIP++;
			uloc++;
			printf( "(%u): ", ucnt );

			for( i=0; i<ucnt; i++)
				{
				nval = (int)*pbyIP++;
				printf( "%5u ", nval );
				uloc++;
				}
			}
		else if( byInst == 0x41 )				 				/* NPUSHW[] */	
			{
			/* display NPUSHW arguments */
			ucnt = *pbyIP++;
			uloc++;
			printf( "(%u): ", ucnt );

			for( i=0; i<ucnt; i++)
				{
				nval = *pbyIP++;
				nval <<= 8;
				nval += *pbyIP++;
				uloc += 2;
				printf( "%5d ", nval);
				}
			}

		printf( "\n" );
		uloc ++;		/* update location */
		}


} /* TTIUnAsm() */
