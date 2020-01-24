//***************************************************************************
//
//  Module: dialog.h
//
//  Purpose:
//     Private header file for dialog.c.
//
//  Description of functions:
//     Descriptions are contained in function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              1/12/92   BryanW      Added this comment block.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

// constant declarations

#define ATOM_HECB   0x100

// macros

#define SET_HECB_PROP( x, y ) SetProp( x, MAKEINTATOM( ATOM_HECB ), (HANDLE) y )
#define GET_HECB_PROP( x )    GetProp( x, MAKEINTATOM( ATOM_HECB ) )
#define REMOVE_HECB_PROP( x ) RemoveProp( x, MAKEINTATOM( ATOM_HECB ) )

//***************************************************************************
//  End of File: dialog.h
//***************************************************************************

