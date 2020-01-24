/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  APACHE, Apache For Windows
 *
 *      Module:  dlldesc.h
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

// constant definitions

#define HDR_FTYPE        0x18
#define HDR_TYPEWINDOWS  0x40
#define HDR_NEWOFFSET    0x3C
#define HDR_NONRESIDENT  0x2C

#define MAXLEN_MODULEDESC  256

#define SEEK_SET   0
#define SEEK_CUR   1
#define SEEK_END   2

// function prototypes (private)

int DlgModuleListCB( HWND, int, LPSTR ) ;
BOOL AddModuleDescription( HWND, LPSTR ) ;

/************************************************************************
 * End of File: dlldesc.h
 ************************************************************************/

