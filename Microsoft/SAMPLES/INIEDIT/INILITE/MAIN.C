//*************************************************************
//  File name: main.c
//
//  Description:
//      WinMain and window procedure routines.
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
// by Microsoft Developer Support, Windows Developer Support
// Copyright (c) 1992 Microsoft Corp.  All rights reserved.
//*************************************************************
#include "precomp.h"
#include "inilite.h"
#include "global.h"

HINSTANCE  ghInst       = NULL;
HWND       ghWndMain    = NULL;
char       szFileName[256];
char       szSection[256];
char       szKey[256];
char       szValue[256];
char       szMainMenu[]    = "MainMenu";
char       szMainClass[]   = "IniliteMain";
char       szInstructions[]= "This is a driver to test the Lite version of the INI file Munge a.k.a. INILite. This will work easiest if you copy a file to your Windows directory named aaa.ini. Then add and delete till blue in face";

//*************************************************************
//
//  WinMain()
//
//  Purpose:
//		Entry point for all Windows applications.
//
//  Parameters:
//      HINSTANCE hInstance     - Handle to current instance
//      HINSTANCE hPrevInstance - Handle to previous instance
//      LPSTR     lpCmdLine     - Command-line arguments
//      int       nCmdShow      - How window is initially displayed
//
//  Return: ( int PASCAL )
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92   JJJ        Created
//
//*************************************************************
int PASCAL WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG msg;

    if ( !hPrevInstance && !InitApplication( hInstance ) )
            return( FALSE );       

    if ( !InitInstance( hInstance, nCmdShow ) )
        return ( FALSE );

    while ( GetMessage( &msg, NULL, NULL, NULL ) )
    {
        TranslateMessage( &msg );      
        DispatchMessage( &msg );       
    }
    return ( msg.wParam );      

} // WinMain() 

//*************************************************************
//
//  MainWndProc()
//
//  Purpose:
//		Main Window procedure.
//
//  Parameters:
//      HWND     hWnd   - Handle to main window
//      UINT     msg    - Message passed to application
//      WPARAM   wParam - Additional message information
//      LPARAM   lParam - Additional message information
//
//  Return: ( LPARAM )
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
//*************************************************************

LRESULT __export CALLBACK MainWndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg ) 
    {

        case WM_COMMAND: 
            switch ( wParam )
            {
                case IDM_ADDKEYS:      
                    DialogBox( ghInst, "WPPSE", hWnd, (DLGPROC)WPPSEProc );
                    break;


                case IDM_DELETEKEYS:      
                    DialogBox( ghInst, "DPP", hWnd, (DLGPROC)DPPProc );
                    break;

                case IDM_ABOUT:
                    DialogBox( ghInst, "AboutBox", hWnd, (DLGPROC)About );    
                    break;

                case IDM_INSTRUCTIONS:
                    DialogBox( ghInst, "Instructions", hWnd, (DLGPROC)Instructions );    
                    break;
 
                case IDM_EXIT:     
                    PostQuitMessage( 0 );
                    break;
            }
        break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
        break;
    }
    return( DefWindowProc( hWnd, msg, wParam, lParam ) );

} // MainWndProc() 

//*************************************************************
//
//  About()
//
//  Purpose:
//		The About dialog box procedure.
//
//  Parameters:
//      HWND     hDlg   - Handle to dialog window
//      UINT     msg    - Message passed to dialog window
//      WPARAM   wParam - Additional message information
//      LPARAM   lParam - Additional message information
//
//  Return: ( BOOL )
//      TRUE  - About dialog procedure handled the message.
//      FALSE - About dialog procedure did not handle the message.
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
//*************************************************************

BOOL __export CALLBACK About ( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg ) 
    {
        case WM_INITDIALOG: 
            SetDlgItemText( hDlg, ID_DESC, "Inilite" );
            SetDlgItemText( hDlg, ID_VER, "Version 1.0" );
            return( TRUE );

        case WM_COMMAND:
            switch ( wParam )
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog( hDlg, TRUE );         
                    return( TRUE );

                default:
                    break;
            }
        break;
    }
    return( FALSE );      // Didn't process a message

} // About() 


//*************************************************************
//
//  WPPSEProc()
//
//  Purpose:
//		The WPPSE dialog box procedure.
//
//  Parameters:
//      HWND     hDlg   - Handle to dialog window
//      UINT     msg    - Message passed to dialog window
//      WPARAM   wParam - Additional message information
//      LPARAM   lParam - Additional message information
//
//  Return: ( BOOL )
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
//*************************************************************

BOOL __export CALLBACK WPPSEProc ( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg ) 
    {
        case WM_INITDIALOG: 
            DlgDirListComboBox( hDlg, "c:\\windows\\*.ini", ID_FILENAME,0,DDL_READWRITE  );
            SendDlgItemMessage( hDlg, ID_FILENAME, CB_SETCURSEL,(WPARAM)0,(LPARAM)0L );
            return( TRUE );

        case WM_COMMAND:
            switch ( wParam )
            {
                case IDOK:
                {
                    UINT uiFlags=0;
                    GetDlgItemText( hDlg, ID_FILENAME, szFileName, 256 );
                    GetDlgItemText( hDlg, ID_SECTION,  szSection, 256 );
                    GetDlgItemText( hDlg, ID_KEY,      szKey, 256 );
                    GetDlgItemText( hDlg, ID_VALUE,    szValue, 256 );
                    if ( SendDlgItemMessage(hDlg, ID_ALLOWDUPES, BM_GETCHECK, 0, 0L) )
                        uiFlags |= WPPSE_ALLOW_DUPES;     
                    if ( SendDlgItemMessage(hDlg, ID_CREATESEC, BM_GETCHECK, 0, 0L) )
                        uiFlags |=WPPSE_CREATE_SECTION;     
                    WritePrivateProfileStringEx( szFileName, szSection, szKey, szValue, uiFlags);
                    EndDialog( hDlg, TRUE );         
                    return( TRUE );
                }    
                
                case IDCANCEL:
                    EndDialog( hDlg, FALSE );         
                    return( TRUE );

                default:
                    break;
            }
            break;
            
        default:    
            break;
    }
    return( FALSE );      // Didn't process a message
} // WPPSEProc() 

//*************************************************************
//
//  DPPProc()
//
//  Purpose:
//		The DPP dialog box procedure.
//
//  Parameters:
//      HWND     hDlg   - Handle to dialog window
//      UINT     msg    - Message passed to dialog window
//      WPARAM   wParam - Additional message information
//      LPARAM   lParam - Additional message information
//
//  Return: ( BOOL )
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
//*************************************************************

BOOL __export CALLBACK DPPProc ( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg ) 
    {
        case WM_INITDIALOG: 
            DlgDirListComboBox( hDlg, "c:\\windows\\*.ini", ID_FILENAME,0,DDL_READWRITE  );
            SendDlgItemMessage( hDlg, ID_FILENAME, CB_SETCURSEL,(WPARAM)0,(LPARAM)0L );
            return( TRUE );

        case WM_COMMAND:
            switch ( wParam )
            {
                case IDOK:
                {
                    UINT uiFlags=0;
                    GetDlgItemText( hDlg, ID_FILENAME, szFileName, 256 );
                    GetDlgItemText( hDlg, ID_SECTION,  szSection, 256 );
                    GetDlgItemText( hDlg, ID_KEY,      szKey, 256 );
                    GetDlgItemText( hDlg, ID_VALUE,    szValue, 256 );
                    if ( SendDlgItemMessage(hDlg, ID_MATCHKEYS, BM_GETCHECK, 0, 0L) )
                        uiFlags |= DPP_ALL_MATCHING_KEYS;     
                    if ( SendDlgItemMessage(hDlg, ID_ALLSECTIONS, BM_GETCHECK, 0, 0L) )
                        uiFlags |=DPP_IN_ALL_SECTIONS;     
                    if ( SendDlgItemMessage(hDlg, ID_ANYVALUE, BM_GETCHECK, 0, 0L) )
                        uiFlags |=DPP_ANY_VALUE;     
                    DeletePrivateProfile( szFileName, szSection, szKey, szValue, uiFlags);
                    EndDialog( hDlg, TRUE );         
                    return( TRUE );
                }

                case IDCANCEL:
                    EndDialog( hDlg, FALSE );         
                    return( TRUE );

                default:
                    break;
            }
            break;
            
        default:    
            break;
    }
    return( FALSE );      // Didn't process a message
} // DPPProc() 

//*************************************************************
//
//  Instructions()
//
//  Purpose:
//		The instructions dialog box procedure.
//
//  Parameters:
//      HWND     hDlg   - Handle to dialog window
//      UINT     msg    - Message passed to dialog window
//      WPARAM   wParam - Additional message information
//      LPARAM   lParam - Additional message information
//
//  Return: ( BOOL )
//      TRUE  - dialog procedure handled the message.
//      FALSE - dialog procedure did not handle the message.
//
//  Comments:
//
//  History:    Date       Author     Comment
//              1/1/92     JJJ        Created
//
//*************************************************************

BOOL __export CALLBACK Instructions ( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    static HFONT hFont;
    switch ( msg ) 
    {
        case WM_INITDIALOG: 
        {
            //   Height = (points_size/72 points per inch) * 
            //     (GetDeviceCaps(LOGPIXELSY)= pixels per inch)

            HDC hDC;
            int Height;

            hDC = GetDC( hDlg );
            Height = (GetDeviceCaps( hDC, LOGPIXELSY )*16)/72;

            hFont = CreateFont( Height, 0, 0, 0, FW_BOLD, 0, 0, 0, 
                                ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                                FF_DONTCARE, (LPSTR)"Roman" );
            ReleaseDC( hDlg, hDC );
            SetDlgItemText( hDlg, ID_TITLE, "Purpose" );
            SetWindowText( GetDlgItem( hDlg, ID_INSTBOX ), 
                           (LPSTR)szInstructions );
            SendDlgItemMessage( hDlg, ID_TITLE, (UINT)WM_SETFONT, 
                                (WPARAM)hFont, TRUE );
            return( TRUE );
        }

        case WM_COMMAND:
            if ( wParam == IDOK || wParam == IDCANCEL ) 
            {
                EndDialog( hDlg, TRUE );         
                DeleteObject( hFont );
                return( TRUE );
            }
        break;
    }
    return( FALSE );      // Didn't process a message

} // Instructions() 

// EOF: main.c 
