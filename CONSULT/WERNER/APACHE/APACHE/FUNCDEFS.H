//***************************************************************************
//
//  Module: funcdefs.h
//
//  Purpose:
//     Function prototypes for explicitly loaded DLLs.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/ 2/91   BryanW      Initial version, initial build
//              1/12/92   BryanW      Updated to "new" style.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//***************************************************************************

// typedefs for function pointers common to all modular DLLs

typedef int (FAR PASCAL *LPFNGETUSECOUNT)( VOID ) ;

// typedefs for function pointers in connector DLLs

typedef HANDLE (FAR PASCAL *LPFNCREATECCB)( HWND ) ;
typedef BOOL (FAR PASCAL *LPFNDESTROYCCB)( HANDLE ) ;
typedef BOOL (FAR PASCAL *LPFNLOADCONNECTORSETTINGS)( HANDLE, LPSTR, LPSTR ) ;
typedef BOOL (FAR PASCAL *LPFNSAVECONNECTORSETTINGS)( HANDLE, LPSTR, LPSTR ) ;
typedef BOOL (FAR PASCAL *LPFNDOCONNECTORSETUPDLG)( HANDLE, HWND,
                                                    LPSTR, LPSTR ) ;
typedef BOOL (FAR PASCAL *LPFNOPENCONNECTOR)( HANDLE ) ;
typedef BOOL (FAR PASCAL *LPFNOPENCONNECTORCOMPLETE)( HANDLE ) ;
typedef BOOL (FAR PASCAL *LPFNCLOSECONNECTOR)( HANDLE ) ;
typedef int (FAR PASCAL *LPFNREADCONNECTORBLOCK)( HANDLE, int, LPSTR ) ;
typedef BOOL (FAR PASCAL *LPFNWRITECONNECTORBYTE)( HANDLE, BYTE ) ;
typedef BOOL (FAR PASCAL *LPFNWRITECONNECTORBLOCK)( HANDLE, LPSTR, int ) ;
typedef BOOL (FAR _cdecl *LPFNWRITECONNECTORFORMAT)( HANDLE, LPSTR, ... ) ;
typedef LONG (FAR PASCAL *LPFNESCAPECONNECTORFUNCTION)( HANDLE, WORD, 
                                                        LONG, LONG ) ;
// typedefs for function pointers in emulator DLLs

typedef BOOL (FAR PASCAL *LPFNLOADEMULATORSETTINGS)( HWND, LPSTR, LPSTR ) ;
typedef BOOL (FAR PASCAL *LPFNSAVEEMULATORSETTINGS)( HANDLE, LPSTR, LPSTR ) ;
typedef BOOL (FAR PASCAL *LPFNDOEMULATORSETUPDLG)( HWND, HWND, LPSTR, LPSTR ) ;

// typedefs for function pointers in protocol DLLs

typedef HWND (FAR PASCAL *LPFNPROTOCOLRECEIVE)( HWND, LPSTR ) ;
typedef HWND (FAR PASCAL *LPFNPROTOCOLSEND)( HWND, LPSTR ) ;

//***************************************************************************
//  End of File: funcdefs.h
//***************************************************************************

