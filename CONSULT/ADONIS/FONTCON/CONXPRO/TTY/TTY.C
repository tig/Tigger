//---------------------------------------------------------------------------
//
//  Module: tty.c
//
//  Purpose:
//     The sample application demonstrates the usage of the COMM
//     API.  It implements the new COMM API of Windows 3.1.
//
//     NOTE:  no escape sequences are translated, only
//            the necessary control codes (LF, CR, BS, etc.)
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//
//              5/ 8/91   BryanW      Wrote it.
//
//             10/18/91   BryanW      Sanitized code & comments.
//
//             10/22/91   BryanW      Minor bug fixes.
//
//              2/10/92   BryanW      Fixed a problem with ReadCommBlock()
//                                    and error detection.  ReadComm() is
//                                    now performed... if the result is < 0
//                                    the error status is read using
//                                    GetCommError() and displayed.  The
//                                    length is adjusted (*= -1).
//
//              2/13/92   BryanW      Updated to provide the option to
//                                    implement/handle CN_RECEIVE
//                                    notifications.
//
//              2/13/92   BryanW      Implemented forced DTR control. DTR
//                                    is asserted on connect and dropped
//                                    on disconnect.
//
//              2/15/92   ckindel     Modified to minimize calls to GWW.
//                                    Made more "strict" (uses WINDOWSX.H.,
//                                    Now uses global mem of tty structure.
//                                    Re-ordered where functions appear
//                                    within this file to allow for easier
//                                    plug and play into other apps.
//                                    Previous versions assumed that
//                                    the tty window had a menu/was the
//                                    top level window.
//
//              2/15/92   ckindel     Macroized memory alloc stuff so
//                                    changing from global to local is
//                                    a breeze.   You can specify
//                                    USE_GLOBALMEM, USE_LOCALMEM, or
//                                    USE_LOCALMEMFIXED.
//
//              4/11/92   ckindel     Added zmodem support
//
//---------------------------------------------------------------------------
//

#include "tty.h"
#include "zmodem.h"

char  lpszFileList[] = { "C:\\A\\FILE1.FIL\0C:\\A\\FILE2.FIL\0C:\\A\\FILE3.FIL\0\0" } ;

// 
// Routines specific to the TTY Sample application that don't 
// really make sense to export.
//
BOOL NEAR InitApplication( HANDLE ) ;
HWND NEAR InitInstance( HANDLE, int ) ;
VOID NEAR GoModalDialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM ) ;
VOID NEAR FillComboBox( HINSTANCE, HWND, int, UINT NEAR *, UINT, UINT ) ;
BOOL NEAR SettingsDlgInit( HWND ) ;
BOOL NEAR SettingsDlgTerm( HWND ) ;

BOOL     NEAR ScrollTTYHorz( HWND, UINT, UINT ) ;
BOOL     NEAR ScrollTTYVert( HWND, UINT, UINT ) ;
BOOL     NEAR SizeTTY( HWND, UINT, UINT ) ;
BOOL     NEAR MoveTTYCursor( HWND ) ;
BOOL     NEAR PaintTTY( HWND ) ;


BOOL     NEAR ProcessCOMMNotification( HWND, UINT, LONG ) ;

// Exported functions

LRESULT  CALLBACK fnTTYWnd( HWND, UINT, WPARAM, LPARAM ) ;
BOOL     CALLBACK fnSettingsDlg( HWND, UINT, WPARAM, LPARAM ) ;
BOOL     CALLBACK fnAboutDlg( HWND, UINT, WPARAM, LPARAM ) ;

UINT CALLBACK fnSend( HWND hwndZmodem, LPBYTE lp, UINT cb ) ;

ZMODEMSENDPROC lpfnSend ;
BOOL           fZmodem = FALSE ;

//---------------------------------------------------------------------------
//  int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
//                      LPSTR lpszCmdLine, int nCmdShow )
//
//  Description:
//     This is the main window loop!
//
//  Parameters:
//     As documented for all WinMain() functions.
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow )
{
   HWND  hTTYWnd ;
   MSG   msg ;

   hWDB = wdbOpenSession( NULL, "TTY", "WDB.INI", 0 ) ;

   if (!hPrevInstance)
      if (!InitApplication( hInstance ))
         return ( FALSE ) ;

   if (NULL == (hTTYWnd = InitInstance( hInstance, nCmdShow )))
      return ( FALSE ) ;

   lpfnSend = (ZMODEMSENDPROC)MakeProcInstance( (FARPROC)fnSend, hInstance ) ;

   while (GetMessage( &msg, NULL, 0, 0 ))
   {
      if (!TranslateAccelerator( hTTYWnd, ghAccel, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }

   FreeProcInstance( (FARPROC)lpfnSend ) ;

   return ( (int) msg.wParam ) ;

} // end of WinMain()

//---------------------------------------------------------------------------
//  BOOL NEAR InitApplication( HANDLE hInstance )
//
//  Description:
//     First time initialization stuff.  This registers information
//     such as window classes.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to this instance of the application.
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//             10/22/91   BryanW      Fixed background color problem.
//
//---------------------------------------------------------------------------

BOOL NEAR InitApplication( HANDLE hInstance )
{
   WNDCLASS  wndclass ;

   // register tty window class

   wndclass.style =         NULL ;
   wndclass.lpfnWndProc =   fnTTYWnd;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    sizeof( LPVOID ) ;     // ckindel 2/15/92
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         LoadIcon( hInstance, MAKEINTRESOURCE( TTYICON ) );
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
   wndclass.lpszMenuName =  MAKEINTRESOURCE( TTYMENU ) ;
   wndclass.lpszClassName = gszTTYClass ;

   return( RegisterClass( &wndclass ) ) ;

} // end of InitApplication()

//---------------------------------------------------------------------------
//  HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
//
//  Description:
//     Initializes instance specific information.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to instance
//
//     int nCmdShow
//        How do we show the window?
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
{
   HWND  hTTYWnd ;

   // load accelerators
   ghAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE( TTYACCEL ) ) ;

   // create the TTY window
   hTTYWnd = CreateWindow( gszTTYClass, gszAppName,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL, NULL, hInstance, NULL ) ;

   if (NULL == hTTYWnd)
      return ( NULL ) ;

   ShowWindow( hTTYWnd, nCmdShow ) ;
   UpdateWindow( hTTYWnd ) ;

   return ( hTTYWnd ) ;

} // end of InitInstance()

//---------------------------------------------------------------------------
//  LRESULT CALLBACK fnTTYWnd( HWND hWnd, UINT wMsg,
//                                 WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the TTY Window Proc.  This handles ALL messages
//     to the tty window.
//
//  Parameters:
//     As documented for Window procedures.
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

LRESULT CALLBACK fnTTYWnd( HWND hWnd, UINT wMsg,
                               WPARAM wParam, LPARAM lParam )
{
   HMENU hMenu ;

   switch (wMsg)
   {
      case WM_CREATE:
         hMenu = GetMenu( hWnd ) ;
         EnableMenuItem( hMenu, IDM_DISCONNECT,
                         MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
         EnableMenuItem( hMenu, IDM_CONNECT, MF_ENABLED | MF_BYCOMMAND ) ;

         Zmodem( NULL, ZM_SHOWSTATUS, 0, 0L ) ;

         return ( CreateTTYInfo( hWnd ) ) ;

      case WM_TIMER:
      {
         LPARAM   lParam1 = NULL ;
         LPARAM   lParam2 = NULL ;
         LRESULT  lRes ;

         Zmodem( hWnd, ZM_TIMER, 0L, 0L ) ;

         switch(lRes = Zmodem( hWnd, ZM_QUERYSTATUS, lParam1, lParam2 ))
         {
            #if 0
            case ZMS_IDLE:
               Zmodem( hwnd, ZM_INIT, 0L, 0L ) ;

               if (ZMS_OK != Zmodem( hwnd, ZM_RECEIVE, NULL/*(LPARAM)lpFSM->lpReceive*/, 0L ))
               {
                  DP1( hWDB, "ZM_RECEIVE failed" ) ;

                  
               }
            break ;
            #endif

            case ZMS_SENDING:
               /*
                * This means we are in the middle of a send transfer.
                * This should never happen here 'cause we're receive!
                */
            break ;

            case ZMS_RECEIVING:
               /*
                * After we've started a zmodem receive we'll get this
                * status message back until the receive is complete...
                */
            break ;

            case ZMS_FILECOMPLETED:
               /*
                  * This indicates that one of the files we requested has
                  * completed successfully.  This status is a lot like
                  * ZMS_RECIEVING in that a transfer is still in progress.
                  *
                  * When we get this status, lParam point to te name of
                  * the file that was received.  We then tell the protocol
                  * that we understand, and thank you, by using the
                  * ZM_ACK_FILECOMPLETED command.
                  */
               Zmodem( hWnd, ZM_ACK_FILECOMPLETED, lParam1, 0L ) ;
            break ;

            case ZMS_COMPLETED:
               /*
                  * This status indicates that all of the files we requested
                  * have been received a-ok!
                  */
               Zmodem( hWnd, ZM_ACK_COMPLETED, 0L, 0L ) ;
               Zmodem( hWnd, ZM_KILL, 0L, 0L ) ;

               hMenu = GetMenu( hWnd ) ;

               EnableMenuItem( hMenu, IDM_DISCONNECT,
                              MF_ENABLED | MF_BYCOMMAND ) ;

               EnableMenuItem( hMenu, IDM_RECEIVEZMODEM,
                               MF_ENABLED | MF_BYCOMMAND ) ;

               EnableMenuItem( hMenu, IDM_SENDZMODEM,
                               MF_ENABLED | MF_BYCOMMAND ) ;

               fZmodem = FALSE ;
            break ;

            case ZMS_ERROR:
            case ZMS_LOCALCANCEL:
            case ZMS_TOOMANYERRORS:
            case ZMS_TIMEOUT:
            case ZMS_HOSTCANCEL:
            case ZMS_DISKERROR:
            case ZMS_DISKFULL:
            case ZMS_NAK:
            default:
               /*
                  * This means the transfer was aborted (i.e. the
                  * protocol detected an abort.  Treat this as an
                  * error in communications.
                  */

               Zmodem( hWnd, ZM_KILL, 0L, 0L ) ;

               hMenu = GetMenu( hWnd ) ;

               EnableMenuItem( hMenu, IDM_DISCONNECT,
                              MF_ENABLED | MF_BYCOMMAND ) ;

               EnableMenuItem( hMenu, IDM_RECEIVEZMODEM,
                               MF_ENABLED | MF_BYCOMMAND ) ;

               EnableMenuItem( hMenu, IDM_SENDZMODEM,
                               MF_ENABLED | MF_BYCOMMAND ) ;

               fZmodem = FALSE ;

               DP1( hWDB, "Killed ZMODEM! status = 0x%08lX", (DWORD)lRes ) ;

            break ;
         }

      }
      break ;

      case WM_COMMAND:
      {
         switch ((UINT) wParam)
         {
            case IDM_CONNECT:
               if (OpenConnection( hWnd ))
               {
                  hMenu = GetMenu( hWnd ) ;
                  EnableMenuItem( hMenu, IDM_CONNECT,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_DISCONNECT, MF_ENABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_RECEIVEZMODEM,
                                 MF_ENABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_SENDZMODEM,
                                 MF_ENABLED | MF_BYCOMMAND ) ;
               }
               else
                  MessageBox( hWnd, "Connection failed.", gszAppName,
                              MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_DISCONNECT:
               if (CloseConnection( hWnd ))
               {
                  hMenu = GetMenu( hWnd ) ;
                  EnableMenuItem( hMenu, IDM_DISCONNECT,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_CONNECT, MF_ENABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_RECEIVEZMODEM,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_SENDZMODEM,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;

               }
               break ;

            case IDM_RECEIVEZMODEM:
               /*
                * The first parameter ot ZM_INIT is a pointer to the
                * function that will be called each time zmodem wants
                * to send data.
                *
                * The function should have the following prototype:
                *
                * UINT CALLBACK fnSendBuffer( HWND hwndZmodem, LPBYTE lp, UINT cb )
                *
                */  

               Zmodem( hWnd, ZM_INIT, (LPARAM)lpfnSend, (LPARAM)0x4242 ) ;

               if (ZMS_OK != Zmodem( hWnd, ZM_RECEIVE,
                                     NULL, (LPARAM) (LPSTR) "D:\\ZMODEM\\TTY\\" ))
               {
                  DP1( hWDB, "ZM_RECEIVE failed" ) ;
               }
               else
               {
                  fZmodem = TRUE ;

                  hMenu = GetMenu( hWnd ) ;
                  EnableMenuItem( hMenu, IDM_DISCONNECT,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_CONNECT,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_RECEIVEZMODEM,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  EnableMenuItem( hMenu, IDM_SENDZMODEM,
                                 MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
               }

            break ;

            case IDM_SENDZMODEM:
            {
               OPENFILENAME  ofn ;
               char          szFile[ MAXLEN_PATH + MAXLEN_FILENAME ] ;

               // Set all structure members to zero.

               _fmemset( &ofn, 0, sizeof( OPENFILENAME ) ) ;

               *szFile = NULL ;
               ofn.lStructSize = sizeof( OPENFILENAME ) ;
               ofn.hwndOwner =   hWnd ;
               ofn.lpstrFile=    szFile ;
               ofn.nMaxFile =    sizeof( szFile ) ;
               ofn.lpstrFilter = "All Files\0*.*\0ZIP files\0*.zip\0\0" ;
               ofn.nFilterIndex = 2 ;
               ofn.lpstrTitle =  "Select file to upload..." ;
               ofn.Flags =       OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                                 OFN_HIDEREADONLY ;

               if (GetOpenFileName( &ofn ))
               {
                  // doubly-null terminate

                  szFile[ lstrlen( szFile ) + 1 ] = NULL ;

                  Zmodem( hWnd, ZM_INIT, (LPARAM)lpfnSend, (LPARAM) 0x4242 ) ;
                  if (ZMS_OK != Zmodem( hWnd, ZM_SEND,
                                        (LPARAM) (LPSTR) szFile, NULL ))
                  {
                     DP1( hWDB, "ZM_RECEIVE failed" ) ;
                  }
                  else
                  {
                     fZmodem = TRUE ;

                     hMenu = GetMenu( hWnd ) ;
                     EnableMenuItem( hMenu, IDM_DISCONNECT,
                                    MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                     EnableMenuItem( hMenu, IDM_CONNECT,
                                    MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                     EnableMenuItem( hMenu, IDM_RECEIVEZMODEM,
                                    MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                     EnableMenuItem( hMenu, IDM_SENDZMODEM,
                                    MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
                  }
               }
            }
            break ;

            case IDM_SETTINGS:
            {
               PTTYINFO  pTTYInfo ;

               pTTYInfo = GETTTYINFO( hWnd ) ;
               GoModalDialogBoxParam( GETHINST( hWnd ),
                                      MAKEINTRESOURCE( SETTINGSDLGBOX ), hWnd,
                                      fnSettingsDlg,
                                      (LPARAM)(LPSTR)hWnd ) ;

               UNGETTTYINFO( pTTYInfo ) ;

               // if fConnected, set new COM parameters

               if (IsTTYConnected( hWnd ))
               {
                  if (!SetupConnection( hWnd ))
                        MessageBox( hWnd, "Settings failed!", gszAppName,
                                    MB_ICONEXCLAMATION ) ;
               }
            }
            break ;

            case IDM_ABOUT:
               GoModalDialogBoxParam ( GETHINST( hWnd ),
                                       MAKEINTRESOURCE( ABOUTDLGBOX ),
                                       hWnd,
                                       fnAboutDlg, NULL ) ;
               break;

            case IDM_EXIT:

               PostMessage( hWnd, WM_CLOSE, NULL, 0L ) ;
               break ;
         }
      }
      break ;

      case WM_COMMNOTIFY:
         ProcessCOMMNotification( hWnd, (UINT) wParam, (LONG) lParam ) ;
         break ;

      case WM_PAINT:
         PaintTTY( hWnd ) ;
         break ;

      case WM_SIZE:
         SizeTTY( hWnd, HIWORD( lParam ), LOWORD( lParam ) ) ;
         break ;

      case WM_HSCROLL:
         ScrollTTYHorz( hWnd, (UINT) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_VSCROLL:
         ScrollTTYVert( hWnd, (UINT) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_CHAR:
         if (!fZmodem)
            ProcessTTYCharacter( hWnd, LOBYTE( wParam ) ) ;
         else
         {
            DP1( hWDB, "WM_CHAR with fZmodem TRUE..." ) ;

            if ((UINT)wParam == VK_ESCAPE)
            {
               DP1( hWDB, "Nuking Zmodem!!!!" ) ;
               Zmodem( hWnd, ZM_ABORT, 0L, 0L ) ;
            }
         }
         break ;

      case WM_SETFOCUS:
         SetTTYFocus( hWnd ) ;
         break ;

      case WM_KILLFOCUS:
         KillTTYFocus( hWnd ) ;
         break ;

      case WM_DESTROY:

         DestroyTTYInfo( hWnd ) ;
         PostQuitMessage( 0 ) ;
         break ;

      case WM_CLOSE:
         if (IsTTYConnected( hWnd ) &&
             (IDOK != MessageBox( hWnd,
                                  "Still Connected!  Ok to close window?",
                                  "TTY Sample",
                                  MB_ICONQUESTION | MB_OKCANCEL )))
            break ;

         Zmodem( NULL, ZM_KILLSTATUS, 0, 0L ) ;

         // fall through

      default:
         return( DefWindowProc( hWnd, wMsg, wParam, lParam ) ) ;
   }
   return 0L ;

} // end of fnTTYWnd()

/****************************************************************
 *  UINT CALLBACK fnSend( HWND hwndZmodem, LPBYTE lp, UINT cb )
 *
 *  Description: 
 *
 *    Callback routine for ZModem sends.
 *
 *  Comments:
 *
 ****************************************************************/
UINT CALLBACK fnSend( HWND hwndZmodem, LPBYTE lp, UINT cb )
{
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hwndZmodem )))
      return FALSE ;

   cb = (UINT)WriteComm( COMDEV( pTTYInfo ), lp, cb ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return cb ;

} /* fnSendBuffer()  */


//*************************************************************************
//
//  TTY specific functions
//
//*************************************************************************

//---------------------------------------------------------------------------
//  LRESULT WINAPI CreateTTYInfo( HWND hWnd )
//
//  Description:
//     Creates the tty information structure and sets
//     menu option availability.  Returns -1 if unsuccessful.
//
//  Parameters:
//     HWND  hWnd
//        Handle to main window.
//
//  History:   Date       Author      Comment
//             10/18/91   BryanW      Pulled from tty window proc.
//              1/13/92   BryanW      Fixed bug with invalid handle
//                                    caused by WM_SIZE sent by
//                                    ResetTTYScreen().
//
//---------------------------------------------------------------------------

LRESULT WINAPI CreateTTYInfo( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;

   // set TTYInfo handle into HWND extra bytes

   SetWindowLong( hWnd, GWL_TTYINFO, (LPARAM)(LPSTR) ALLOCTTYINFO() ) ;

   if (NULL == (pTTYInfo = GETTTYINFO( hWnd )))
      return ( (LRESULT) -1 ) ;

   // initialize TTY info structure

   COMDEV( pTTYInfo )        = 0 ;
   CONNECTED( pTTYInfo )     = FALSE ;
   CURSORSTATE( pTTYInfo )   = CS_HIDE ;
   LOCALECHO( pTTYInfo )     = FALSE ;
   AUTOWRAP( pTTYInfo )      = TRUE ;
   PORT( pTTYInfo )          = 2 ;
   BAUDRATE( pTTYInfo )      = CBR_9600 ;
   BYTESIZE( pTTYInfo )      = 8 ;
   FLOWCTRL( pTTYInfo )      = FC_RTSCTS ;
   PARITY( pTTYInfo )        = NOPARITY ;
   STOPBITS( pTTYInfo )      = ONESTOPBIT ;
   XONXOFF( pTTYInfo )       = FALSE ;
   XSIZE( pTTYInfo )         = 0 ;
   YSIZE( pTTYInfo )         = 0 ;
   XSCROLL( pTTYInfo )       = 0 ;
   YSCROLL( pTTYInfo )       = 0 ;
   XOFFSET( pTTYInfo )       = 0 ;
   YOFFSET( pTTYInfo )       = 0 ;
   COLUMN( pTTYInfo )        = 0 ;
   ROW( pTTYInfo )           = 0 ;
   HTTYFONT( pTTYInfo )      = NULL ;
   FGCOLOR( pTTYInfo )       = RGB( 0, 0, 0 ) ;
   USECNRECEIVE( pTTYInfo )  = TRUE ;
   DISPLAYERRORS( pTTYInfo ) = TRUE ;

   // clear screen space

   _fmemset( SCREEN( pTTYInfo ), ' ', MAXROWS * MAXCOLS ) ;

   // setup default font information

   LFTTYFONT( pTTYInfo ).lfHeight =         12 ;
   LFTTYFONT( pTTYInfo ).lfWidth =          0 ;
   LFTTYFONT( pTTYInfo ).lfEscapement =     0 ;
   LFTTYFONT( pTTYInfo ).lfOrientation =    0 ;
   LFTTYFONT( pTTYInfo ).lfWeight =         0 ;
   LFTTYFONT( pTTYInfo ).lfItalic =         0 ;
   LFTTYFONT( pTTYInfo ).lfUnderline =      0 ;
   LFTTYFONT( pTTYInfo ).lfStrikeOut =      0 ;
   LFTTYFONT( pTTYInfo ).lfCharSet =        OEM_CHARSET ;
   LFTTYFONT( pTTYInfo ).lfOutPrecision =   OUT_DEFAULT_PRECIS ;
   LFTTYFONT( pTTYInfo ).lfClipPrecision =  CLIP_DEFAULT_PRECIS ;
   LFTTYFONT( pTTYInfo ).lfQuality =        DEFAULT_QUALITY ;
   LFTTYFONT( pTTYInfo ).lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
   LFTTYFONT( pTTYInfo ).lfFaceName[0] =    NULL ;

   //
   // If we're using global memory (#define USE_GLOBALMEM) then
   // the following macro is a no-op.
   //
   UNGETTTYINFO( pTTYInfo ) ;

   // reset the character information, etc.

   ResetTTYScreen( hWnd ) ;

   return ( (LRESULT) TRUE ) ;

} // end of CreateTTYInfo()

//---------------------------------------------------------------------------
//  BOOL WINAPI DestroyTTYInfo( HWND hWnd )
//
//  Description:
//     Destroys block associated with TTY window handle.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI DestroyTTYInfo( HWND hWnd )
{
   PTTYINFO pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   // force connection closed (if not already closed)

   if (CONNECTED( pTTYInfo ))
      CloseConnection( hWnd ) ;

   DeleteObject( HTTYFONT( pTTYInfo ) ) ;

   FREETTYINFO( hWnd ) ;

   return ( TRUE ) ;

} // end of DestroyTTYInfo()

//---------------------------------------------------------------------------
//  BOOL WINAPI ResetTTYScreen( HWND hWnd )
//
//  Description:
//     Resets the TTY character information and causes the
//     screen to resize to update the scroll information.
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI ResetTTYScreen( HWND hWnd )
{
   HDC         hDC ;
   PTTYINFO    pTTYInfo ;
   TEXTMETRIC  tm ;
   RECT        rcWindow ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (NULL != HTTYFONT( pTTYInfo ))
      DeleteObject( HTTYFONT( pTTYInfo ) ) ;

   HTTYFONT( pTTYInfo ) = CreateFontIndirect( &LFTTYFONT( pTTYInfo ) ) ;

   hDC = GetDC( hWnd ) ;
   SelectObject( hDC, HTTYFONT( pTTYInfo ) ) ;
   GetTextMetrics( hDC, &tm ) ;
   ReleaseDC( hWnd, hDC ) ;

   XCHAR( pTTYInfo ) = tm.tmAveCharWidth  ;
   YCHAR( pTTYInfo ) = tm.tmHeight + tm.tmExternalLeading ;

   // a slimy hack to force the scroll position, region to
   // be recalculated based on the new character sizes

   GetWindowRect( hWnd, &rcWindow ) ;
   SendMessage( hWnd, WM_SIZE, SIZENORMAL,
                (LPARAM) MAKELONG( rcWindow.right - rcWindow.left,
                                   rcWindow.bottom - rcWindow.top ) ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;
} // end of ResetTTYScreen()

//---------------------------------------------------------------------------
//  BOOL WINAPI SetTTYFocus( HWND hWnd )
//
//  Description:
//     Sets the focus to the TTY window also creates caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI SetTTYFocus( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (CONNECTED( pTTYInfo ) && (CURSORSTATE( pTTYInfo ) != CS_SHOW))
   {
      CreateCaret( hWnd, NULL, XCHAR( pTTYInfo ), YCHAR( pTTYInfo ) ) ;
      ShowCaret( hWnd ) ;
      CURSORSTATE( pTTYInfo ) = CS_SHOW ;
   }
   MoveTTYCursor( hWnd ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of SetTTYFocus()

//---------------------------------------------------------------------------
//  BOOL WINAPI KillTTYFocus( HWND hWnd )
//
//  Description:
//     Kills TTY focus and destroys the caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI KillTTYFocus( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (CONNECTED( pTTYInfo ) && (CURSORSTATE( pTTYInfo ) != CS_HIDE))
   {
      HideCaret( hWnd ) ;
      DestroyCaret() ;
      CURSORSTATE( pTTYInfo ) = CS_HIDE ;
   }

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of KillTTYFocus()

//---------------------------------------------------------------------------
//  BOOL WINAPI IsTTYConnected( HWND hWnd )
//
//  Description:
//    Exported routine to check if we're connected or not.  The TTY 
//    sample app does not call this routine.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              2/15/92   ckindel     Wrote it.
//
//---------------------------------------------------------------------------
BOOL WINAPI IsTTYConnected( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;
   BOOL       f ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   // force connection closed (if not already closed)

   f = CONNECTED( pTTYInfo ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return f ;
}

//---------------------------------------------------------------------------
//  BOOL WINAPI ProcessTTYCharacter( HWND hWnd, BYTE bOut )
//
//  Description:
//     This simply writes a character to the port and echos it
//     to the TTY screen if fLocalEcho is set.  Some minor
//     keyboard mapping could be performed here.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     BYTE bOut
//        byte from keyboard
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI ProcessTTYCharacter( HWND hWnd, BYTE bOut )
{
   PTTYINFO  pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (!CONNECTED( pTTYInfo ))
   {
      UNGETTTYINFO( pTTYInfo ) ;
      return ( FALSE ) ;
   }

   WriteCommByte( hWnd, bOut ) ;

   if (!fZmodem && LOCALECHO( pTTYInfo ))
      WriteTTYBlock( hWnd, &bOut, 1 ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of ProcessTTYCharacter()


//---------------------------------------------------------------------------
//  BOOL WINAPI WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
//
//  Description:
//     Writes block to TTY screen.  Nothing fancy - just
//     straight TTY.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpBlock
//        far pointer to block of data
//
//     int nLength
//        length of block
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//              5/20/91   BryanW      Modified... not character based,
//                                    block based now.  It was processing
//                                    per char.
//
//---------------------------------------------------------------------------

BOOL WINAPI WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
{
   int        i ;
   PTTYINFO   pTTYInfo ;
   RECT       rect ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   for (i = 0 ; i < nLength; i++)
   {
      switch (lpBlock[ i ])
      {
         case ASCII_BEL:
            // Bell
            MessageBeep( 0 ) ;
            break ;
   
         case ASCII_BS:
            // Backspace
            if (COLUMN( pTTYInfo ) > 0)
               COLUMN( pTTYInfo ) -- ;
            MoveTTYCursor( hWnd ) ;
            break ;
   
         case ASCII_CR:
            // Carriage return
            COLUMN( pTTYInfo ) = 0 ;
            MoveTTYCursor( hWnd ) ;
            if (!NEWLINE( pTTYInfo ))
               break;
   
            // fall through
   
         case ASCII_LF:
            // Line feed
            if (ROW( pTTYInfo )++ == MAXROWS - 1)
            {
               _fmemmove( (LPSTR) (SCREEN( pTTYInfo )),
                          (LPSTR) (SCREEN( pTTYInfo ) + MAXCOLS),
                          (MAXROWS - 1) * MAXCOLS ) ;
               _fmemset( (LPSTR) (SCREEN( pTTYInfo ) + (MAXROWS - 1) * MAXCOLS),
                         ' ', MAXCOLS ) ;
               InvalidateRect( hWnd, NULL, FALSE ) ;
               ROW( pTTYInfo )-- ;
            }
            MoveTTYCursor( hWnd ) ;
            break ;

         default:
            *(SCREEN( pTTYInfo ) + ROW( pTTYInfo ) * MAXCOLS +
                COLUMN( pTTYInfo )) = lpBlock[ i ] ;
            rect.left = (COLUMN( pTTYInfo ) * XCHAR( pTTYInfo )) -
                        XOFFSET( pTTYInfo ) ;
            rect.right = rect.left + XCHAR( pTTYInfo ) ;
            rect.top = (ROW( pTTYInfo ) * YCHAR( pTTYInfo )) -
                       YOFFSET( pTTYInfo ) ;
            rect.bottom = rect.top + YCHAR( pTTYInfo ) ;
            InvalidateRect( hWnd, &rect, FALSE ) ;

            // Line wrap
            if (COLUMN( pTTYInfo ) < MAXCOLS - 1)
               COLUMN( pTTYInfo )++ ;
            else if (AUTOWRAP( pTTYInfo ))
               WriteTTYBlock( hWnd, "\r\n", 2 ) ;
            break;
      }
   }

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of WriteTTYBlock()

//---------------------------------------------------------------------------
//  BOOL WINAPI SelectTTYFont( HWND hDlg )
//
//  Description:
//     Selects the current font for the TTY screen.
//     Uses the Common Dialog ChooseFont() API.
//
//  Parameters:
//     HWND hDlg
//        handle to settings dialog
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI SelectTTYFont( HWND hDlg )
{
   CHOOSEFONT  cfTTYFont ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( GET_HWNDTTY_PROP( hDlg ) )))
      return FALSE ;

   cfTTYFont.lStructSize    = sizeof( CHOOSEFONT ) ;
   cfTTYFont.hwndOwner      = hDlg ;
   cfTTYFont.hDC            = NULL ;
   cfTTYFont.rgbColors      = FGCOLOR( pTTYInfo ) ;
   cfTTYFont.lpLogFont      = &LFTTYFONT( pTTYInfo ) ;
   cfTTYFont.Flags          = CF_SCREENFONTS | CF_FIXEDPITCHONLY |
                              CF_EFFECTS | CF_INITTOLOGFONTSTRUCT ;
   cfTTYFont.lCustData      = NULL ;
   cfTTYFont.lpfnHook       = NULL ;
   cfTTYFont.lpTemplateName = NULL ;
   cfTTYFont.hInstance      = GETHINST( hDlg ) ;

   if (ChooseFont( &cfTTYFont ))
   {
     FGCOLOR( pTTYInfo ) = cfTTYFont.rgbColors ;
     ResetTTYScreen( GET_HWNDTTY_PROP( hDlg ) ) ;
   }

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of SelectTTYFont()

//---------------------------------------------------------------------------
//  BOOL NEAR PaintTTY( HWND hWnd )
//
//  Description:
//     Paints the rectangle determined by the paint struct of
//     the DC.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window (as always)
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//             10/22/91   BryanW      Problem with background color
//                                    and "off by one" fixed.
//
//---------------------------------------------------------------------------

BOOL NEAR PaintTTY( HWND hWnd )
{
   int          nRow, nCol, nEndRow, nEndCol, nCount, nHorzPos, nVertPos ;
   HDC          hDC ;
   HFONT        hOldFont ;
   PTTYINFO    pTTYInfo ;
   PAINTSTRUCT  ps ;
   RECT         rect ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   hDC = BeginPaint( hWnd, &ps ) ;
   hOldFont = SelectObject( hDC, HTTYFONT( pTTYInfo ) ) ;
   SetTextColor( hDC, FGCOLOR( pTTYInfo ) ) ;
   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
   rect = ps.rcPaint ;
   nRow = max( 0, (rect.top + YOFFSET( pTTYInfo )) / YCHAR( pTTYInfo ) ) ;
   nEndRow = min( MAXROWS - 1, ((rect.bottom + YOFFSET( pTTYInfo ) ) /
                  YCHAR( pTTYInfo )) - 1 ) ;
   nCol = max( 0, (rect.left + XOFFSET( pTTYInfo )) / XCHAR( pTTYInfo ) ) ;
   nEndCol = min( MAXCOLS - 1, ((rect.right + XOFFSET( pTTYInfo ) ) /
                  XCHAR( pTTYInfo )) - 1 ) ;
   nCount = nEndCol - nCol + 1 ;
   for (; nRow <= nEndRow; nRow++)
   {
      nVertPos = (nRow * YCHAR( pTTYInfo )) - YOFFSET( pTTYInfo ) ;
      nHorzPos = (nCol * XCHAR( pTTYInfo )) - XOFFSET( pTTYInfo ) ;
      rect.top = nVertPos ;
      rect.bottom = nVertPos + YCHAR( pTTYInfo ) ;
      rect.left = nHorzPos ;
      rect.right = nHorzPos + XCHAR( pTTYInfo ) * nCount ;
      SetBkMode( hDC, OPAQUE ) ;
      ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE | ETO_CLIPPED, &rect,
                  (LPSTR)( SCREEN( pTTYInfo ) + nRow * MAXCOLS + nCol ),
                  nCount, NULL ) ;
   }
   SelectObject( hDC, hOldFont ) ;
   EndPaint( hWnd, &ps ) ;
   MoveTTYCursor( hWnd ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of PaintTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR SizeTTY( HWND hWnd, UINT wVertSize, UINT wHorzSize )
//
//  Description:
//     Sizes TTY and sets up scrolling regions.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wVertSize
//        new vertical size
//
//     UINT wHorzSize
//        new horizontal size
//
//  History:   Date       Author      Comment
//              5/ 8/ 91  BryanW      Wrote it
//
//---------------------------------------------------------------------------

BOOL NEAR SizeTTY( HWND hWnd, UINT wVertSize, UINT wHorzSize )
{
   int        nScrollAmt ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   YSIZE( pTTYInfo ) = (int) wVertSize ;
   YSCROLL( pTTYInfo ) = max( 0, (MAXROWS * YCHAR( pTTYInfo )) -
                               YSIZE( pTTYInfo ) ) ;
   nScrollAmt = min( YSCROLL( pTTYInfo ), YOFFSET( pTTYInfo ) ) -
                     YOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;

   YOFFSET( pTTYInfo ) = YOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( pTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_VERT, 0, YSCROLL( pTTYInfo ), TRUE ) ;

   XSIZE( pTTYInfo ) = (int) wHorzSize ;
   XSCROLL( pTTYInfo ) = max( 0, (MAXCOLS * XCHAR( pTTYInfo )) -
                                XSIZE( pTTYInfo ) ) ;
   nScrollAmt = min( XSCROLL( pTTYInfo ), XOFFSET( pTTYInfo )) -
                     XOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   XOFFSET( pTTYInfo ) = XOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( pTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_HORZ, 0, XSCROLL( pTTYInfo ), TRUE ) ;

   InvalidateRect( hWnd, NULL, TRUE ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of SizeTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYVert( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
//
//  Description:
//     Scrolls TTY window vertically.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wScrollCmd
//        type of scrolling we're doing
//
//     UINT wScrollPos
//        scroll position
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYVert( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
{
   int        nScrollAmt ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -YOFFSET( pTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = YSCROLL( pTTYInfo ) - YOFFSET( pTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -YSIZE( pTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = YSIZE( pTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -YCHAR( pTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = YCHAR( pTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - YOFFSET( pTTYInfo ) ;
         break ;

      default:
         UNGETTTYINFO( pTTYInfo ) ;
         return ( FALSE ) ;
   }
   if ((YOFFSET( pTTYInfo ) + nScrollAmt) > YSCROLL( pTTYInfo ))
      nScrollAmt = YSCROLL( pTTYInfo ) - YOFFSET( pTTYInfo ) ;
   if ((YOFFSET( pTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -YOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   YOFFSET( pTTYInfo ) = YOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( pTTYInfo ), TRUE ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of ScrollTTYVert()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYHorz( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
//
//  Description:
//     Scrolls TTY window horizontally.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wScrollCmd
//        type of scrolling we're doing
//
//     UINT wScrollPos
//        scroll position
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYHorz( HWND hWnd, UINT wScrollCmd, UINT wScrollPos )
{
   int        nScrollAmt ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -XOFFSET( pTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = XSCROLL( pTTYInfo ) - XOFFSET( pTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -XSIZE( pTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = XSIZE( pTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -XCHAR( pTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = XCHAR( pTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - XOFFSET( pTTYInfo ) ;
         break ;

      default:
         UNGETTTYINFO( pTTYInfo ) ;
         return ( FALSE ) ;
   }
   if ((XOFFSET( pTTYInfo ) + nScrollAmt) > XSCROLL( pTTYInfo ))
      nScrollAmt = XSCROLL( pTTYInfo ) - XOFFSET( pTTYInfo ) ;
   if ((XOFFSET( pTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -XOFFSET( pTTYInfo ) ;
   ScrollWindow( hWnd, -nScrollAmt, 0, NULL, NULL ) ;
   XOFFSET( pTTYInfo ) = XOFFSET( pTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( pTTYInfo ), TRUE ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of ScrollTTYHorz()

//---------------------------------------------------------------------------
//  BOOL NEAR MoveTTYCursor( HWND hWnd )
//
//  Description:
//     Moves caret to current position.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR MoveTTYCursor( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (CONNECTED( pTTYInfo ) && (CURSORSTATE( pTTYInfo ) & CS_SHOW))
      SetCaretPos( (COLUMN( pTTYInfo ) * XCHAR( pTTYInfo )) -
                   XOFFSET( pTTYInfo ),
                   (ROW( pTTYInfo ) * YCHAR( pTTYInfo )) -
                   YOFFSET( pTTYInfo ) ) ;
   
   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of MoveTTYCursor()


//*************************************************************************
//
//  COMM specific functions
//
//*************************************************************************


//---------------------------------------------------------------------------
//  BOOL WINAPI OpenConnection( HWND hWnd )
//
//  Description:
//     Opens communication port specified in the TTYINFO struct.
//     It also sets the CommState and notifies the window via
//     the fConnected flag in the TTYINFO struct.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI OpenConnection( HWND hWnd )
{
   char       szPort[ 10 ], szTemp[ 10 ] ;
   BOOL       fRetVal ;
   HCURSOR    hOldCursor, hWaitCursor ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   // show the hourglass cursor
   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   // load the COM prefix string and append port number

   LoadString( GETHINST( hWnd ), IDS_COMPREFIX, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szPort, "%s%d", (LPSTR) szTemp, PORT( pTTYInfo ) ) ;

   // open COMM device

   if ((COMDEV( pTTYInfo ) = OpenComm( szPort, RXQUEUE, TXQUEUE )) < 0)
   {
      UNGETTTYINFO( pTTYInfo ) ;
      return ( FALSE ) ;
   }

   fRetVal = SetupConnection( hWnd ) ;

   if (fRetVal)
   {
      CONNECTED( pTTYInfo ) = TRUE ;

      // set up notifications from COMM.DRV

      if (!USECNRECEIVE( pTTYInfo ))
      {
         // In this case we really are only using the notifications
         // for the received characters - it could be expanded to
         // cover the changes in CD or other status lines.

         SetCommEventMask( COMDEV( pTTYInfo ), EV_RXCHAR ) ;

         // Enable notifications for events only.
      
         // NB:  This method does not use the specific
         // in/out queue triggers.

         EnableCommNotification( COMDEV( pTTYInfo ), hWnd, -1, -1 ) ;
      }
      else
      {
         // Enable notification for CN_RECEIVE events.

         EnableCommNotification( COMDEV( pTTYInfo ), hWnd, MAXBLOCK, -1 ) ;
      }

      // assert DTR

      EscapeCommFunction( COMDEV( pTTYInfo ), SETDTR ) ;

      SetTTYFocus( hWnd ) ;

   }
   else
   {
      CONNECTED( pTTYInfo ) = FALSE ;
      CloseComm( COMDEV( pTTYInfo ) ) ;
   }

   // restore cursor
   SetCursor( hOldCursor ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( fRetVal ) ;

} // end of OpenConnection()

//---------------------------------------------------------------------------
//  BOOL WINAPI SetupConnection( HWND hWnd )
//
//  Description:
//     This routines sets up the DCB based on settings in the
//     TTY info structure and performs a SetCommState().
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI SetupConnection( HWND hWnd )
{
   BOOL       fRetVal ;
   BYTE       bSet ;
   DCB        dcb ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   GetCommState( COMDEV( pTTYInfo ), &dcb ) ;

   dcb.BaudRate = BAUDRATE( pTTYInfo ) ;
   dcb.ByteSize = BYTESIZE( pTTYInfo ) ;
   dcb.Parity = PARITY( pTTYInfo ) ;
   dcb.StopBits = STOPBITS( pTTYInfo ) ;

   // setup hardware flow control

   bSet = (BYTE) ((FLOWCTRL( pTTYInfo ) & FC_DTRDSR) != 0) ;
   dcb.fOutxDsrFlow = dcb.fDtrflow = bSet ;
   dcb.DsrTimeout = (bSet) ? 30 : 0 ;

   bSet = (BYTE) ((FLOWCTRL( pTTYInfo ) & FC_RTSCTS) != 0) ;
   dcb.fOutxCtsFlow = dcb.fRtsflow = bSet ;
   dcb.CtsTimeout = (bSet) ? 30 : 0 ;

   // setup software flow control

   bSet = (BYTE) ((FLOWCTRL( pTTYInfo ) & FC_XONXOFF) != 0) ;

   dcb.fInX = dcb.fOutX = bSet ;
   dcb.XonChar = ASCII_XON ;
   dcb.XoffChar = ASCII_XOFF ;
   dcb.XonLim = 100 ;
   dcb.XoffLim = 100 ;

   // other various settings

   dcb.fBinary = TRUE ;
   dcb.fParity = TRUE ;
   dcb.fRtsDisable = FALSE ;
   dcb.fDtrDisable = FALSE ;

   fRetVal = !(SetCommState( &dcb ) < 0) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( fRetVal ) ;

} // end of SetupConnection()

//---------------------------------------------------------------------------
//  BOOL WINAPI CloseConnection( HWND hWnd )
//
//  Description:
//     Closes the connection to the port.  Resets the connect flag
//     in the TTYINFO struct.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI CloseConnection( HWND hWnd )
{
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   // Disable event notification.  Using a NULL hWnd tells
   // the COMM.DRV to disable future notifications.

   EnableCommNotification( COMDEV( pTTYInfo ), NULL, -1, -1 ) ;

   // kill the focus

   KillTTYFocus( hWnd ) ;

   // drop DTR

   EscapeCommFunction( COMDEV( pTTYInfo ), CLRDTR ) ;

   // close comm connection

   CloseComm( COMDEV( pTTYInfo ) ) ;
   CONNECTED( pTTYInfo ) = FALSE ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of CloseConnection()

//---------------------------------------------------------------------------
//  int WINAPI ReadCommBlock( HWND hWnd, LPSTR lpszBlock, int nMaxLength )
//
//  Description:
//     Reads a block from the COM port and stuffs it into
//     the provided block.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpszBlock
//        block used for storage
//
//     int nMaxLength
//        max length of block to read
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

int WINAPI ReadCommBlock( HWND hWnd, LPSTR lpszBlock, int nMaxLength )
{
   char       szError[ 10 ] ;
   int        nLength, nError ;
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   nLength = ReadComm( COMDEV( pTTYInfo ), lpszBlock, nMaxLength ) ;

   if (nLength < 0)
   {
      nLength *= -1 ;
      while (nError = GetCommError( COMDEV( pTTYInfo ), NULL ))
      {
         if (DISPLAYERRORS( pTTYInfo ))
         {
            wsprintf( szError, "<CE-%d>", nError ) ;
            WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
         }
      }
   }

   UNGETTTYINFO( pTTYInfo ) ;

   return ( nLength ) ;

} // end of ReadCommBlock()

//---------------------------------------------------------------------------
//  BOOL WINAPI WriteCommByte( HWND hWnd, BYTE bByte )
//
//  Description:
//     Writes a byte to the COM port specified in the associated
//     TTY info structure.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     BYTE bByte
//        byte to write to port
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL WINAPI WriteCommByte( HWND hWnd, BYTE bByte )
{
   PTTYINFO   pTTYInfo ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   WriteComm( COMDEV( pTTYInfo ), (LPSTR) &bByte, 1 ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of WriteCommByte()

//---------------------------------------------------------------------------
//  BOOL NEAR ProcessCOMMNotification( HWND hWnd, UINT wParam, LONG lParam )
//
//  Description:
//     Processes the WM_COMMNOTIFY message from the COMM.DRV.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     UINT wParam
//        specifes the device (nCid)
//
//     LONG lParam
//        LOWORD contains event trigger
//        HIWORD is NULL
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//             10/18/91   BryanW      Updated to verify the event.
//
//---------------------------------------------------------------------------

BOOL NEAR ProcessCOMMNotification( HWND hWnd, UINT wParam, LONG lParam )
{
   char       szError[ 10 ] ;
   int        nError, nLength ;
   BYTE       abIn[ MAXBLOCK + 1] ;
   COMSTAT    ComStat ;
   PTTYINFO   pTTYInfo ;
   MSG        msg ;

   if (!(pTTYInfo = GETTTYINFO( hWnd )))
      return FALSE ;

   if (!USECNRECEIVE( pTTYInfo ))
   {
      // verify that it is a COMM event specified by our mask

      if (CN_EVENT & LOWORD( lParam ) != CN_EVENT)
      {
         UNGETTTYINFO( pTTYInfo ) ;
         return ( FALSE ) ;
      }

      // reset the event word so we are notified
      // when the next event occurs

      GetCommEventMask( COMDEV( pTTYInfo ), EV_RXCHAR ) ;

      // We loop here since it is highly likely that the buffer
      // can been filled while we are reading this block.  This
      // is especially true when operating at high baud rates
      // (e.g. >= 9600 baud).

      do
      {
         if (nLength = ReadCommBlock( hWnd, (LPSTR) abIn, MAXBLOCK ))
         {

            if (!fZmodem)
            {
               WriteTTYBlock( hWnd, (LPSTR) abIn, nLength ) ;
      
               // force a paint
               UpdateWindow( hWnd ) ;
            }

            Zmodem( hWnd, ZM_RXBLK, MAKELPARAM( nLength, 0 ),
                    (LPARAM) (LPSTR) abIn ) ;

         }
      }
      while (!PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE) ||
             (nLength > 0)) ;

   }
   else
   {
      // verify that it is a receive event

      if (CN_RECEIVE & LOWORD( lParam ) != CN_RECEIVE)
      {
         UNGETTTYINFO( pTTYInfo ) ;
         return ( FALSE ) ;
      }

      do
      {
         if (nLength = ReadCommBlock( hWnd, (LPSTR) abIn, MAXBLOCK ))
         {
            if (!fZmodem)
            {
               WriteTTYBlock( hWnd, (LPSTR) abIn, nLength ) ;

               // force a paint

               UpdateWindow( hWnd ) ;
            }

            Zmodem( hWnd, ZM_RXBLK, MAKELPARAM( nLength, 0 ),
                    (LPARAM) (LPSTR) abIn ) ;
         }
         if (nError = GetCommError( COMDEV( pTTYInfo ), &ComStat ))
         {
            if (DISPLAYERRORS( pTTYInfo ))
            {
               wsprintf( szError, "<CE-%d>", nError ) ;
               WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
            }
         }
      }
      while ((!PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE )) ||
            (ComStat.cbInQue >= MAXBLOCK)) ;
   }

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of ProcessCOMMNotification()

//---------------------------------------------------------------------------
//  VOID NEAR GoModalDialogBoxParam( HINSTANCE hInstance,
//                                   LPCSTR lpszTemplate, HWND hWnd,
//                                   DLGPROC lpDlgProc, LPARAM lParam )
//
//  Description:
//     It is a simple utility function that simply performs the
//     MPI and invokes the dialog box with a DWORD paramter.
//
//  Parameters:
//     similar to that of DialogBoxParam() with the exception
//     that the lpDlgProc is not a procedure instance
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

VOID NEAR GoModalDialogBoxParam( HINSTANCE hInstance, LPCSTR lpszTemplate,
                                 HWND hWnd, DLGPROC lpDlgProc, LPARAM lParam )
{
   DLGPROC  lpProcInstance ;

   lpProcInstance = (DLGPROC) MakeProcInstance( (FARPROC) lpDlgProc,
                                                hInstance ) ;
   DialogBoxParam( hInstance, lpszTemplate, hWnd, lpProcInstance, lParam ) ;
   FreeProcInstance( (FARPROC) lpProcInstance ) ;

} // end of GoModalDialogBoxParam()

//---------------------------------------------------------------------------
//  VOID NEAR FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
//                          UINT NEAR *npTable, UINT wTableLen,
//                          UINT wCurrentSetting )
//
//  Description:
//     Fills the given combo box with strings from the resource
//     table starting at nIDString.  Associated items are
//     added from given table.  The combo box is notified of
//     the current setting.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to application instance
//
//     HWND hCtrlWnd
//        handle to combo box control
//
//     int nIDString
//        first resource string id
//
//     UINT NEAR *npTable
//        near point to table of associated values
//
//     UINT wTableLen
//        length of table
//
//     UINT wCurrentSetting
//        current setting (for combo box selection)
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Pulled from the init procedure.
//
//---------------------------------------------------------------------------

VOID NEAR FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
                        UINT NEAR *npTable, UINT wTableLen,
                        UINT wCurrentSetting )
{
   char  szBuffer[ MAXLEN_TEMPSTR ] ;
   UINT  wCount, wPosition ;

   for (wCount = 0; wCount < wTableLen; wCount++)
   {
      // load the string from the string resources and
      // add it to the combo box

      LoadString( hInstance, nIDString + wCount, szBuffer, sizeof( szBuffer ) ) ;
      wPosition = LOWORD( SendMessage( hCtrlWnd, CB_ADDSTRING, NULL,
                                       (LPARAM) (LPSTR) szBuffer ) ) ;

      // use item data to store the actual table value

      SendMessage( hCtrlWnd, CB_SETITEMDATA, (WPARAM) wPosition,
                   (LPARAM) (LONG) *(npTable + wCount) ) ;

      // if this is our current setting, select it

      if (*(npTable + wCount) == wCurrentSetting)
         SendMessage( hCtrlWnd, CB_SETCURSEL, (WPARAM) wPosition, NULL ) ;
   }

} // end of FillComboBox()

//---------------------------------------------------------------------------
//  BOOL NEAR SettingsDlgInit( HWND hDlg )
//
//  Description:
//     Puts current settings into dialog box (via CheckRadioButton() etc.)
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//             10/20/91   BryanW      Dialog revision.
//             10/24/91   BryanW      Bug in EscapeCommFunction() call.
//
//---------------------------------------------------------------------------

BOOL NEAR SettingsDlgInit( HWND hDlg )
{
   char       szBuffer[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ] ;
   PTTYINFO   pTTYInfo ;
   UINT       wCount, wMaxCOM, wPosition ;

   if (!(pTTYInfo = GETTTYINFO( GET_HWNDTTY_PROP( hDlg ) )))
      return FALSE ;

   wMaxCOM = LOWORD( EscapeCommFunction( NULL, GETMAXCOM ) ) + 1 ;

   // load the COM prefix from resources

   LoadString( GETHINST( hDlg ), IDS_COMPREFIX, szTemp, sizeof( szTemp ) ) ;

   // fill port combo box and make initial selection

   for (wCount = 0; wCount < wMaxCOM; wCount++)
   {
      wsprintf( szBuffer, "%s%d", (LPSTR) szTemp, wCount + 1 ) ;
      SendDlgItemMessage( hDlg, IDD_PORTCB, CB_ADDSTRING, NULL,
                          (LPARAM) (LPSTR) szBuffer ) ;
   }
   SendDlgItemMessage( hDlg, IDD_PORTCB, CB_SETCURSEL,
                       (WPARAM) (PORT( pTTYInfo ) - 1), NULL ) ;

   // disable COM port combo box if connection has already been
   // established (e.g. OpenComm() already successful)

   EnableWindow( GetDlgItem( hDlg, IDD_PORTCB ), !CONNECTED( pTTYInfo ) ) ;

   // fill baud combo box and make initial selection

   FillComboBox( GETHINST( hDlg ), GetDlgItem( hDlg, IDD_BAUDCB ),
                 IDS_BAUD110, gawBaudTable,
                 sizeof( gawBaudTable ) / sizeof( UINT ),
                 BAUDRATE( pTTYInfo ) ) ;

   // fill data bits combo box and make initial selection

   for (wCount = 5; wCount < 9; wCount++)
   {
      wsprintf( szBuffer, "%d", wCount ) ;
      wPosition = LOWORD( SendDlgItemMessage( hDlg, IDD_DATABITSCB,
                                              CB_ADDSTRING, NULL,
                                              (LPARAM) (LPSTR) szBuffer ) ) ;

      // if current selection, tell the combo box

      if (wCount == BYTESIZE( pTTYInfo ))
         SendDlgItemMessage( hDlg, IDD_DATABITSCB, CB_SETCURSEL,
                             (WPARAM) wPosition, NULL ) ;
   }

   // fill parity combo box and make initial selection

   FillComboBox( GETHINST( hDlg ), GetDlgItem( hDlg, IDD_PARITYCB ),
                 IDS_PARITYNONE, gawParityTable,
                 sizeof( gawParityTable ) / sizeof( UINT ),
                 PARITY( pTTYInfo ) ) ;

   // fill stop bits combo box and make initial selection

   FillComboBox( GETHINST( hDlg ), GetDlgItem( hDlg, IDD_STOPBITSCB ),
                 IDS_ONESTOPBIT, gawStopBitsTable,
                 sizeof( gawStopBitsTable ) / sizeof ( UINT ),
                 STOPBITS( pTTYInfo ) ) ;

   // initalize the flow control settings

   CheckDlgButton( hDlg, IDD_DTRDSR,
                   (FLOWCTRL( pTTYInfo ) & FC_DTRDSR) > 0 ) ;
   CheckDlgButton( hDlg, IDD_RTSCTS,
                   (FLOWCTRL( pTTYInfo ) & FC_RTSCTS) > 0 ) ;
   CheckDlgButton( hDlg, IDD_XONXOFF,
                   (FLOWCTRL( pTTYInfo ) & FC_XONXOFF) > 0 ) ;

   // other TTY settings

   CheckDlgButton( hDlg, IDD_AUTOWRAP, AUTOWRAP( pTTYInfo ) ) ;
   CheckDlgButton( hDlg, IDD_NEWLINE, NEWLINE( pTTYInfo ) ) ;
   CheckDlgButton( hDlg, IDD_LOCALECHO, LOCALECHO( pTTYInfo ) ) ;

   // control options

   CheckDlgButton( hDlg, IDD_USECNRECEIVE, USECNRECEIVE( pTTYInfo ) ) ;

   // disable Use CN_RECEIVE option if connection has already been
   // established (e.g. OpenComm() already successful)

   EnableWindow( GetDlgItem( hDlg, IDD_USECNRECEIVE ),
                 !CONNECTED( pTTYInfo ) ) ;

   CheckDlgButton( hDlg, IDD_DISPLAYERRORS, DISPLAYERRORS( pTTYInfo ) ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of SettingsDlgInit()

//---------------------------------------------------------------------------
//  BOOL NEAR SettingsDlgTerm( HWND hDlg )
//
//  Description:
//     Puts dialog contents into TTY info structure.
//
//  Parameters:
//     HWND hDlg
//        handle to settings dialog
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR SettingsDlgTerm( HWND hDlg )
{
   PTTYINFO   pTTYInfo ;
   UINT       wSelection ;

   if (!(pTTYInfo = GETTTYINFO( GET_HWNDTTY_PROP( hDlg ) )))
      return FALSE ;

   // get port selection

   PORT( pTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_PORTCB,
                                          CB_GETCURSEL,
                                          NULL, NULL ) ) + 1 ) ;
   // get baud rate selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BAUDCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   BAUDRATE( pTTYInfo ) =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BAUDCB, CB_GETITEMDATA,
                                  (WPARAM) wSelection, NULL ) ) ;

   // get data bits selection

   BYTESIZE( pTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_DATABITSCB,
                                          CB_GETCURSEL,
                                          NULL, NULL ) ) + 5 ) ;

   // get parity selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_PARITYCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   PARITY( pTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_PARITYCB,
                                          CB_GETITEMDATA,
                                          (WPARAM) wSelection,
                                           NULL ) ) ) ;

   // get stop bits selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_STOPBITSCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   STOPBITS( pTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_STOPBITSCB,
                                          CB_GETITEMDATA,
                                          (WPARAM) wSelection, NULL ) ) ) ;

   // get flow control settings

   FLOWCTRL( pTTYInfo ) = 0 ;
   if (IsDlgButtonChecked( hDlg, IDD_DTRDSR ))
      FLOWCTRL( pTTYInfo ) |= FC_DTRDSR ;
   if (IsDlgButtonChecked( hDlg, IDD_RTSCTS ))
      FLOWCTRL( pTTYInfo ) |= FC_RTSCTS ;
   if (IsDlgButtonChecked( hDlg, IDD_XONXOFF ))
      FLOWCTRL( pTTYInfo ) |= FC_XONXOFF ;

   // get other various settings

   AUTOWRAP( pTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_AUTOWRAP ) ;
   NEWLINE( pTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_NEWLINE ) ;
   LOCALECHO( pTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_LOCALECHO ) ;

   // control options

   USECNRECEIVE( pTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_USECNRECEIVE ) ;
   DISPLAYERRORS( pTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_DISPLAYERRORS ) ;

   UNGETTTYINFO( pTTYInfo ) ;

   return ( TRUE ) ;

} // end of SettingsDlgTerm()

//---------------------------------------------------------------------------
//  BOOL CALLBACK fnSettingsDlg( HWND hDlg, UINT wMsg,
//                                   WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This handles all of the user preference settings for
//     the TTY.
//
//  Parameters:
//     same as all dialog procedures
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//             10/20/91   BryanW      Now uses window properties to
//                                    store TTYInfo handle.  Also added
//                                    font selection.
//
//---------------------------------------------------------------------------

BOOL CALLBACK fnSettingsDlg( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         HWND hwnd ;

         // get & save handle to TTY info structure

         hwnd = (HWND) LOWORD( lParam ) ;
         SET_HWNDTTY_PROP( hDlg, (HANDLE) hwnd ) ;

         return ( SettingsDlgInit( hDlg ) ) ;
      }

      case WM_COMMAND:
         switch ((UINT) wParam)
         {
            case IDD_FONT:
               return ( SelectTTYFont( hDlg ) ) ;

            case IDOK:
               // Copy stuff into structure
               SettingsDlgTerm( hDlg ) ;
               EndDialog( hDlg, TRUE ) ;
               return ( TRUE ) ;

            case IDCANCEL:
               // Just end
               EndDialog( hDlg, TRUE ) ;
               return ( TRUE ) ;
         }
         break;

      case WM_DESTROY:
         REMOVE_HWNDTTY_PROP( hDlg ) ;
         break ;
   }
   return ( FALSE ) ;

} // end of fnSettingsDlg()

//---------------------------------------------------------------------------
//  BOOL CALLBACK fnAboutDlg( HWND hDlg, UINT wMsg,
//                                WPARAM wParam, LPARAM lParam )
//
//  Description:
//     Simulates the Windows System Dialog Box.
//
//  Parameters:
//     Same as standard dialog procedures.
//
//  History:   Date       Author      Comment
//             10/19/91   BryanW      Added this little extra.
//      
//---------------------------------------------------------------------------

BOOL CALLBACK fnAboutDlg( HWND hDlg, UINT wMsg,
                              WPARAM wParam, LPARAM lParam )
{
   switch (wMsg)
   {
      case WM_INITDIALOG:
      {
         int          idModeString ;
         char         szBuffer[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ] ;
         DWORD        dwFreeMemory, dwWinFlags ;
         UINT         wFreeResources, wRevision, wVersion ;

#ifdef ABOUTDLG_USEBITMAP
         // if we are using the bitmap, hide the icon

         ShowWindow( GetDlgItem( hDlg, IDD_ABOUTICON ), SW_HIDE ) ;
#endif
         // sets up the version number for Windows

         wVersion = LOWORD( GetVersion() ) ;
         switch (HIBYTE( wVersion ))
         {
            case 10:
               wRevision = 1 ;
               break ;

            default:
               wRevision = 0 ;
               break;
         }
         wVersion &= 0xFF ;

         GetDlgItemText( hDlg, IDD_TITLELINE, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, wVersion, wRevision ) ;
         SetDlgItemText( hDlg, IDD_TITLELINE, szBuffer ) ;

         // sets up version number for TTY

         GetDlgItemText( hDlg, IDD_VERSION, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, VER_MAJOR, VER_MINOR, VER_BUILD ) ;
         SetDlgItemText( hDlg, IDD_VERSION, (LPSTR) szBuffer ) ;

         // get by-line

         LoadString( GETHINST( hDlg ), IDS_BYLINE, szBuffer,
                     sizeof( szBuffer ) ) ;
         SetDlgItemText( hDlg, IDD_BYLINE, szBuffer ) ;

         // set windows mode information

         dwWinFlags = GetWinFlags() ;
         if (dwWinFlags & WF_ENHANCED)
            idModeString = IDS_MODE_ENHANCED ;
         else if (dwWinFlags & WF_STANDARD)
            idModeString = IDS_MODE_STANDARD ;
         else if (dwWinFlags & WF_WLO)
            idModeString = IDS_MODE_WLO ;
         else
            idModeString = IDS_MODE_UNDEF ;

         LoadString( GETHINST( hDlg ), idModeString, szBuffer,
                     sizeof( szBuffer ) ) ;
         SetDlgItemText( hDlg, IDD_WINDOWSMODE, szBuffer ) ;

         // get free memory information

         dwFreeMemory = GetFreeSpace( 0 ) / 1024L ;
         GetDlgItemText( hDlg, IDD_FREEMEM, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, dwFreeMemory ) ;
         SetDlgItemText( hDlg, IDD_FREEMEM, (LPSTR) szBuffer ) ;

         // get free resources information

         wFreeResources = GetFreeSystemResources( 0 ) ;
         GetDlgItemText( hDlg, IDD_RESOURCES, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, wFreeResources ) ;
         SetDlgItemText( hDlg, IDD_RESOURCES, (LPSTR) szBuffer ) ;
      }
      return ( TRUE ) ;

#ifdef ABOUTDLG_USEBITMAP
      // used to paint the bitmap

      case WM_PAINT:
      {
         HBITMAP      hBitMap ;
         HDC          hDC, hMemDC ;
         PAINTSTRUCT  ps ;

         // load bitmap and display it

         hDC = BeginPaint( hDlg, &ps ) ;
         if (NULL != (hMemDC = CreateCompatibleDC( hDC )))
         {
            hBitMap = LoadBitmap( GETHINST( hDlg ),
                                  MAKEINTRESOURCE( TTYBITMAP ) ) ;
            hBitMap = SelectObject( hMemDC, hBitMap ) ;
            BitBlt( hDC, 10, 10, 64, 64, hMemDC, 0, 0, SRCCOPY ) ;
            DeleteObject( SelectObject( hMemDC, hBitMap ) ) ;
            DeleteDC( hMemDC ) ;
         }
         EndPaint( hDlg, &ps ) ;
      }
      break ;
#endif

      case WM_COMMAND:
         if ((UINT) wParam == IDOK)
         {
            EndDialog( hDlg, TRUE ) ;
            return ( TRUE ) ;
         }
         break;
   }
   return ( FALSE ) ;

} // end of fnAboutDlg()


//---------------------------------------------------------------------------
//  End of File: tty.c
//---------------------------------------------------------------------------

