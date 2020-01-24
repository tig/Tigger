// status.c 
//
// VERSION 1.0
// 7/31/90
//

//
// Application for determining the status of a printer port
// Demonstrates the use of GetCommError() to detect the status of a local
// printer.  (i.e. Out of paper, not connected, etc...)
//
// NOTES;
//
//  Current version supports only LPT1, LPT2, LPT3, LPT4 (not any of the
//  COMx ports).
//  This applicaton also demonstrates the fact that GetCommError() will not
//  detect the status of a networked printer.
//


#include "windows.h"	  
#include "status.h"	  
#include "statusd.h" 


HANDLE   hInst;
char     szAppName [] = "PortStatus" ;
char     szPort [10] ;             // Port to play with
#define  DEFAULTPORT "LPT1" 

int PASCAL WinMain(  HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR lpCmdLine,		  
                     int nCmdShow )			  
{
    MSG msg;			  
                        
   if (!hPrevInstance)	
	   if (!InitApplication(hInstance)) 
	       return (FALSE);	  


   if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

   while (GetMessage(&msg, NULL, NULL, NULL))	 
  	{
      TranslateMessage(&msg) ;
     	DispatchMessage(&msg) ;	  
   }
   return (msg.wParam) ;	  
}



BOOL InitApplication( HANDLE hInstance )
{
    WNDCLASS  wc;

    wc.style         = NULL ;      
    wc.lpfnWndProc   = MainWndProc;  
                                   
    wc.cbClsExtra    = 0;             
    wc.cbWndExtra    = 0;             
    wc.hInstance     = hInstance;      
    wc.hIcon         = LoadIcon( hInstance, szAppName );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH ); 
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


BOOL InitInstance(   HANDLE hInstance,
                     int    nCmdShow )
{
    HWND            hWnd;               /* Main window handle.                */

    hInst = hInstance;

    

    hWnd = CreateWindow(   szAppName,
                           szAppName,   
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           GetSystemMetrics( SM_CXSCREEN ) / 2,        
                           GetSystemMetrics( SM_CYSCREEN ) / 4,        
                           NULL,                 
                           NULL,                 
                           hInstance,            
                           NULL                  
                        ) ;

    if (!hWnd)
        return (FALSE);

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */
}

long FAR PASCAL MainWndProc(  HWND hWnd,
                              unsigned message,
                              WORD wParam,
                              LONG lParam )
{
   static int xChar, yChar ;
   int j, nErrTmp ;
   int nLine ;
   static int nErr ;
   static int nCloseErr ;
   static int nCid ;

   FARPROC  lpProcAbout;		  /* pointer to the "About" function */
   FARPROC  lpProcChange ;
   char     szBuf [80] ;
   
   HDC      hDC ;
   HFONT    hFont, hOldFont ;
   TEXTMETRIC tm ;
   PAINTSTRUCT ps ;

   static char *szOC_Err [] =
   {
      "Zero",
      "IE_BADID    (Invalid or unsupported id)",
      "IE_OPEN     (Device Already Open)",
      "IE_NOPEN    (Device Not Open)",
      "IE_MEMORY   (Unable to allocate queues)",
      "IE_DEFAULT  (Error in default parameters)",
      "IE_HARDWARE (Hardware Not Present)",
      "IE_BYTESIZE (Illegal Byte Size)",
      "IE_BAUDRATE (Unsupported BaudRate)"
   } ;
   static char *szGCE_Err [] =
   {
      "Zero",    
      "CE_RXOVER   (Receive Queue overflow)",
      "CE_OVERRUN  (Receive Overrun Error)",
      "CE_RXPARITY (Receive Parity Error)",
      "CE_FRAME    (Receive Framing error)",
      "CE_BREAK    (Break Detected)",
      "CE_CTSTO    (CTS Timeout)",
      "CE_DSRTO	 (DSR Timeout)",
      "CE_RLSDTO   (RLSD Timeout)",
      "CE_TXFULL   (Queue is full)",
      "CE_PTO      (LPTx Timeout)",
      "CE_IOE      (LPTx I/O Error)",
      "CE_DNS      (LPTx Device not selected)",
      "CE_OOP      (LPTx Out-Of-Paper)",
      "CE_MODE     (Requested mode unsupported)"


   } ;   


   switch (message)
   {
      case WM_CREATE:
         lstrcpy( szPort, DEFAULTPORT ) ;
         hDC = GetDC( hWnd ) ;
         hFont = CreateFont(  6, 6, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                              OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS,
                              PROOF_QUALITY,
                              FIXED_PITCH,
                              "Courier" ) ;

         SelectObject (hDC, hFont) ;
         GetTextMetrics( hDC, &tm ) ;
         xChar = tm.tmAveCharWidth ;
         yChar = tm.tmHeight + tm.tmExternalLeading ;
         DeleteObject (SelectObject (hDC, hFont)) ;
         ReleaseDC( hWnd, hDC ) ;
         nCid = OpenComm( szPort, 0, 0 ) ;
         nErr = GetCommError( nCid, NULL ) ;
         nCloseErr = CloseComm( nCid ) ;

         break ;

	   case WM_COMMAND:	   /* message: command from application menu */
	      switch (wParam)
         {
            case IDM_ABOUT :
               lpProcAbout = MakeProcInstance(About, hInst);
           		DialogBox(  hInst,		
		                  "AboutBox",	 
                        hWnd,			 
                        lpProcAbout );
               FreeProcInstance(lpProcAbout);
               break;

            case IDM_CHECK :
               // !+!+!+!+!+!+!+!+!+!+!+!=!+!+!+!=!+!+!+!+!+!+!+!+!+!
               // -===-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
               //
               // This is where the actual error checking happens
               // We open the port, write to it, and then check for
               // an error.
               //
               //----------------------------------------------------

               if (0 > (nCid = OpenComm( szPort, 0, 0 )))
               {
                  // This should probably never happen
                  MessageBeep(0) ;
                  nErr = GetCommError( nCid, NULL ) ;
               }
               else
               {
                  // This is where we get the error value.
                  WriteComm( nCid, (LPSTR)"\0", 1) ;
                  nErr = GetCommError( nCid, NULL ) ;
                  nCloseErr = CloseComm( nCid ) ;
               }

               InvalidateRect( hWnd, NULL, TRUE ) ;
               break ;

            case IDM_CHANGEPORT :
               lpProcChange = MakeProcInstance(ChangePort, hInst ) ;
               DialogBox( hInst,
                        "Settings",
                        hWnd,
                        lpProcChange ) ;
               FreeProcInstance( lpProcChange ) ;
               break ;

            case IDM_EXIT :
               PostMessage( hWnd, WM_CLOSE, 0, 0L ) ;
               break ;

            default:
               return (DefWindowProc(hWnd, message, wParam, lParam));
         }
         break ;

      case WM_PAINT :
         nLine = 1 ;
         hDC = BeginPaint( hWnd, &ps ) ;
         hFont = CreateFont(  6, 6, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
                        OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS,
                        PROOF_QUALITY,
                        FIXED_PITCH,
                        "Courier" ) ;


         hOldFont = SelectObject (hDC, hFont) ;
         SetTextColor( hDC, RGB( 0, 0, 0 ) ) ;
         TextOut( hDC, xChar, nLine++ * yChar, szBuf,
               wsprintf( (LPSTR)szBuf,
                           (LPSTR)"Printer Port: %s,  nCid: %d",
                           (LPSTR)szPort,
                           nCid ) ) ;

         if (nCid < 1)
         {
            SetTextColor( hDC, RGB( 255, 0, 0 ) ) ;
            TextOut( hDC, xChar, nLine++ * yChar, szBuf,
                  wsprintf( (LPSTR)szBuf,
                              (LPSTR)"OpenComm Error: %s",
                              (LPSTR)szPort,
                              (LPSTR)szOC_Err[-1 * nCid] ) ) ;
         }

         // Tricky stuff to index into array of strings of error
         // descriptions.
         j = 0 ;
         nErrTmp = nErr ;
         while ((nErrTmp >>= 1)) 
            j++ ;
         if (j)
         {
            SetTextColor( hDC, RGB( 255, 0, 0 ) ) ;
            TextOut( hDC, xChar, nLine++ * yChar, szBuf,
               wsprintf( (LPSTR)szBuf,
                           (LPSTR)"CommError: %s (%#06X)",
                           (LPSTR)szGCE_Err[j],
                           nErr) );
         }
         if (nCloseErr < 0)
         {
            SetTextColor( hDC, RGB( 255, 0, 0 ) ) ;
            TextOut( hDC, xChar, nLine++ * yChar, szBuf,
               wsprintf( (LPSTR)szBuf,
                           (LPSTR)"CloseComm Error: %s",
                           nCid,
                           (LPSTR)szOC_Err[-1 * nCloseErr] ) );
         }
         SelectObject( hDC, hOldFont ) ;
         DeleteObject( hFont ) ;
         EndPaint( hWnd, &ps ) ;
         break ;

	   case WM_DESTROY:		  /* message: window being destroyed */
	      PostQuitMessage(0);
	      break;

	   default:			  /* Passes it on if unproccessed    */
	      return (DefWindowProc(hWnd, message, wParam, lParam));
   }
   return (NULL);
}



BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;                               
unsigned message;                        
WORD wParam;                             
LONG lParam;
{
   switch (message)
   {
      case WM_INITDIALOG:
         return (TRUE);

      case WM_COMMAND:	 
   	   if (wParam == IDOK || wParam == IDCANCEL)
         { 
   	   	EndDialog(hDlg, TRUE);	      /* Exits the dialog box	     */
	   	   return (TRUE);
	      }
         break;
    }

    return (FALSE);	  
}




BOOL FAR PASCAL ChangePort(hDlg, message, wParam, lParam)
HWND hDlg;                               
unsigned message;                        
WORD wParam;                             
LONG lParam;
{
   switch (message)
   {
      case WM_INITDIALOG:
         SetDlgItemText( hDlg, IDD_PORTNAME, (LPSTR)szPort ) ;
         return (TRUE);

      case WM_COMMAND:	 
   	   if (wParam == IDOK)
         {
            GetDlgItemText( hDlg, IDD_PORTNAME, (LPSTR)szPort, 8 ) ;
   	   	EndDialog(hDlg, TRUE);	      /* Exits the dialog box	     */
	   	   return (TRUE);
	      }
         break;
    }

    return (FALSE);	  
}
