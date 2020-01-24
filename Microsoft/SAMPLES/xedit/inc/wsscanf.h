/************************************************************************
 *
 *    Copyright (c) 1990 Microsoft Corporation.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  wsscanf.h
 *
 *      Author:  Bryan A. Woodruff (baw)
 *
 *
 *     Remarks:  header file for wsscanf.c
 *
 *   Revisions:  
 *     01.00.000  2/23/91 baw   Initial version, initial build
 *
 ************************************************************************/

#ifndef _WSSCANF_H_
#define _SSCANF_H_
// constant definitions

#define ALLOW_RANGE                        // allow "%[a-z]" - style 
                                           // scansets

#define LEFT_BRACKET  ('[' | ('a' - 'A'))  // 'lowercase' version

#define ASCII         32                   // # of bytes needed to 
                                           // hold 256 bits

// macro definitions

#define INC()            (++nCharCount, Inc( (LPSTR FAR *) &lpPtrBuffer ))
#define UN_INC( chr )    (--nCharCount, UnInc( chr, (LPSTR FAR *) &lpPtrBuffer ))
#define EAT_WHITE()      WhiteOut( (int FAR *) &nCharCount, (LPSTR FAR *) &lpPtrBuffer )
#define HEXTODEC( chr )  HexToDec( chr )

#define MUL10(x)         ( (((x)<<2) + (x))<<1 )

// function definitions

int FAR _cdecl wsscanf( LPSTR, LPSTR, ... ) ;
char NEAR HexToDec( char ) ;
char NEAR Inc( LPSTR FAR * ) ;
VOID NEAR UnInc( char, LPSTR FAR * ) ;
char NEAR WhiteOut( int FAR *, LPSTR FAR * ) ;

#endif
/************************************************************************
 * End of File: wsscanf.h
 ************************************************************************/
