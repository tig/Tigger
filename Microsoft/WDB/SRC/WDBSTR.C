/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbstr.c
 *
 *  DESCRIPTION:
 *      This module contains code to deal with the string table used by
 *      WDB.  Tigger wrote most of this...
 *
 ** cjp */


/* get the includes we need */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


/* extern */
    PSTR    rgszStrings[ (IDS_LAST - IDS_FIRST) + 1 ];

    #define MAX_STRLEN      32


/* this is a "local" global variable for the routines in this file */
    static LOCALHANDLE      lhStrings;


/** BOOL FAR PASCAL wdbGetStrings( HANDLE hInst )
 *
 *  DESCRIPTION: 
 *      Gets the static strings needed by WDB into a Locally alloc'd
 *      memory block.  Uses LoadString() to get them from the resources.
 *
 *  ARGUMENTS:
 *      HANDLE hInst    :   In
 *
 *  RETURN (BOOL):
 *      TRUE if success.  FALSE if fail.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbGetStrings( HANDLE hInst )
{
    PSTR    psStrings = NULL;
    short   i;
    UINT    cb = 0;
    UINT    wCur = 0;
    char    szTemp[ MAX_STRLEN ];


    /*  Allocate the strings as though they will be moveable.  Because
     *  we are using LocalReAlloc() we need them moveable.  Then lock them
     *  before returning.
     */
    for ( i = IDS_FIRST; i <= IDS_LAST; i++ )
    {
        /* get the string and increment the count of bytes so far */
        if ( !(cb += LoadString( hInst, i, szTemp, MAX_STRLEN )) )
        {
            if ( lhStrings )
                LocalFree( lhStrings );
            OD( "wdbGetStrings(): LoadString failed!\r\n" ) ;
            return ( FALSE );
        }

        /*  Load string does not return the length including the NULL it 
         *  tacks on, so we need to add one to our count
         */
        cb++;

        /* allocate or reallocate the block */
        if ( !lhStrings )
        {
            if ( !(lhStrings = LocalAlloc( LHND, cb )) )
                return ( FALSE );
        }

        else
        {
            if ( !(lhStrings = LocalReAlloc( lhStrings, cb, LHND )) )
            {
                LocalFree( lhStrings );
                return ( FALSE );
            }
        }

        /*  Copy the string obtained from LoadString to the buffer and
         *  increment the current pointer.  The string returned from
         *  LoadString() is NULL terminated.
         */
        if ( psStrings = LocalLock( lhStrings ) )
        {
            lstrcpy( psStrings + wCur, szTemp );

            wCur = cb;
            LocalUnlock( lhStrings );
        }

        else
        {
            LocalFree( lhStrings );
            return ( FALSE );
        }
    }

    /*  Now lock the data down and fix up our pointers.  I could have locked
     *  the data before, and avoid this, but I want to ensure we have enough
     *  space for sure!
     */
    psStrings = LocalLock( lhStrings );
    wCur = 0;

    for ( i = IDS_FIRST; i <= IDS_LAST; i++ )
    {
        rgszStrings[ i - IDS_FIRST ] = psStrings + wCur;
        wCur += lstrlen( psStrings + wCur ) + 1;
//        wsprintf( szTemp, (LPSTR)"%d: %s\n\r", i, (LPSTR)rgszStrings[i - IDS_FIRST] );
//        OD( szTemp );
    }

    /* return success */
    return ( TRUE );
} /* wdbGetStrings() */


/** void FAR PASCAL wdbFreeStrings( void )
 *
 *  DESCRIPTION: 
 *      Frees the strings allocated with wdbGetStrings()
 *
 *  ARGUMENTS:
 *      None.
 *
 *  RETURN (void):
 *      The strings will have been freed.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL wdbFreeStrings( void )
{
    if ( lhStrings )
    {
        LocalUnlock( lhStrings );
        LocalFree( lhStrings );
    }
} /* wdbFreeStrings() */


/** EOF: wdbstr.c **/
