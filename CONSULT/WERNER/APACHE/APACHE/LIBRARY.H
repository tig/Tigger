/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  library.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  Header file for library.c
 *
 *   Revisions:  
 *     01.00.001  2/27/91 baw   Initial version, initial build
 *
 ************************************************************************/

// constant definitions

#define MAXLEN_LIBNAME        13

// data structures

typedef LONG FAR _cdecl FNLIBPROC( WORD, ... ) ;
typedef FNLIBPROC FAR *LPFNLIBPROC ;

// function prototypes

HINSTANCE OpenLibrary( LPSTR, LPSTR ) ;
VOID CloseLibrary( HINSTANCE, LPFNGETUSECOUNT ) ;

/************************************************************************
 * End of File: library.h
 ************************************************************************/

