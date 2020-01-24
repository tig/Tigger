//*************************************************************
//  File name: global.h
//
//  Description: 
//      Global include file for #defines and prototypes
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
// by Microsoft Developer Support, Windows Developer Support
// Copyright (c) 1992 Microsoft Corp. All rights reserved.
//*************************************************************
#if !defined(__GLOBAL_H__)
#define __GLOBAL_H__

// Field Sizes
#define FILENAME_SIZE      13
#define SECTION_SIZE       80
#define KEY_SIZE           80
#define VALUE_SIZE         80

// About box Defines 
    #define ID_DESC          101
    #define ID_VER           102
    
// Instruction box Defines 
    #define ID_INSTBOX       101
    #define ID_FRAME         102
    #define ID_TITLE         103

// Entries box defines    
    #define ID_FILENAME        101
    #define ID_SECTION         102
    #define ID_KEY             103
    #define ID_VALUE           104
                               
    #define ID_FILENAME        101
    #define ID_SECTION         102
    #define ID_KEY             103
    #define ID_VALUE           104
    #define ID_ALLOWDUPES      105
    #define ID_CREATESEC       106
    #define ID_MATCHKEYS       107
    #define ID_ALLSECTIONS     108
    #define ID_ANYVALUE        109

// Menu Defines 
    #define IDM_EXIT           101
    #define IDM_ADDKEYS        102
    #define IDM_DELETEKEYS     103
    #define IDM_INSTRUCTIONS   104
    #define IDM_ABOUT          105


// Function Prototypes 
// Main.c 
    int      PASCAL WinMain                       ( HINSTANCE, HINSTANCE, LPSTR, int );
    LRESULT  __export CALLBACK MainWndProc        ( HWND, UINT, WPARAM, LPARAM );
    BOOL     __export CALLBACK About              ( HWND, UINT, WPARAM, LPARAM );
    BOOL     __export CALLBACK Instructions       ( HWND, UINT, WPARAM, LPARAM );
    BOOL     __export CALLBACK WPPSEProc          ( HWND, UINT, WPARAM, LPARAM );
    BOOL     __export CALLBACK DPPProc            ( HWND, UINT, WPARAM, LPARAM );

// Init.c 
    BOOL InitApplication ( HINSTANCE );
    BOOL InitInstance    ( HINSTANCE, int);

// EXTERNS for Global Variables 
    extern HINSTANCE    ghInst;
    extern HWND         ghWndMain;

    extern char         szMainMenu[];
    extern char         szMainClass[];
    extern char         szInstructions[];

//*** EOF: global.h 
#endif

