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
/*      Module: do_timm.c                                      */
/*-------------------------------------------------------------*/
/*      Written by: Dennis Adler               Date: 11/8/91   */
/*                                                             */
/* Description: Decompose a 32 bit count of seconds from       */
/*              Jan 1, 1904 into an actual formatted date.     */
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
#include		<time.h>					/* This is usually in INCLUDE\SYS dir */

#include    "ttfdump.h"

/**********************************************************************

The Microsoft C library routines can deal with elapsed seconds since Jan 1,
1970 at 0:00.  The difference between this and Jan 1, 1904 is 66 years.  There
are 17 leap years in this range (including 1904), for a total of 24,107 days.
This comes down to 2,082,844,800 seconds.  We adjust the Mac time by
subtracting this from the Mac value, then we can simply pass this on to the
Microsoft C library function.

Note that using unsigned long values and ignoring the hi 32 bits of the Mac
time, we can still manage to track time for 136 years from 1904, or until the
year 2040.  Thus, I do not feel bad about ignoring the hi 32 bits!

************************************************************************/

#define		ADJ_SEC			2082844800L

char	date_zero[] = {"(Date stamp is zero)\n"};
char	date_big[60];

char	* do_time( uint32 time1, uint32 time2, int tZone)
{
	if (time2 != 0L) {
		/* Assumes date_big[] is big enuf to hold the string formatted string */
		sprintf( date_big, "Date past 1/1/2040: 0x%08lX,0x%08lX\n", time2, time1);
		return date_big;
		}
	if (time1 != 0L){
		time1 -= (uint32) ADJ_SEC;				/* Convert Mac time to PC time */
		time1 += (uint32) tZone * 3600L;		/* Apply time zone adjustments */
		return ctime( (time_t *) (&time1) );
		}
	else
		return date_zero;
}
