//---------------------------------------------------------------------------
//
//  Module: zmodem.c
//
//  Purpose:
//     ZMODEM protocol for APACHE.  Adapted to Windows by
//     Bryan A. Woodruff based on original code from
//     Chuck Forsberg at Omen Technology Inc.
//
//  Description of functions:
//     Function descriptions are contained in function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  *** ***  THIS CODE IS CONFIDENTIAL PROPERTY OF WERNER ASSOCIATES *** *** 
//  Copyright (c) 1992 Werner Associates.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "precomp.h"   // precompiled header shit

#include "zmodem.h"
#include "crctab.h"    // CRC tables

//------------------------------------------------------------------------
//  int FAR PASCAL GetZmodemUseCount( VOID )
//
//  Description:
//     Returns the usage count of this module.  This is the number
//     of active zmodem sessions.
//
//  Parameters:
//     None.
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

int FAR PASCAL GetZmodemUseCount( VOID )
{
   return ( nUseCount ) ;

} // end of GetZmodemUseCount()

//------------------------------------------------------------------------
//  HWND FAR PASCAL ZmodemReceive( HWND hWnd, LPSTR lpFileName )
//
//  Description:
//     Initiates protocol receive of lpFileName.  Returns a handle
//     to the created dialog box.
//
//  Parameters:
//     HWND hWnd
//        handle to parent
//
//     LPSTR lpFileName
//        filename to save
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

HWND FAR PASCAL ZmodemReceive( HWND hWnd, LPSTR lpFileName )
{
   NPZCB     npZCB ;

   if (NULL == (npZCB = CreateZCB()))
   {
      DP1( hWDB, "CreateZCB failed!" ) ;
      SendAbortString( hWnd ) ;
      return( NULL ) ;
   }

   InitZmodem( npZCB ) ;

   HPARENTWND( npZCB ) = hWnd ;
   DIRECTION( npZCB ) = DIR_RECEIVE ;
   STATE( npZCB ) = RECV_INIT ;
   CURFILE( npZCB ) = 0 ;
   FILEBYTES( npZCB ) = 0 ;
   FILESIZE( npZCB ) = 0 ;
   TRANSMITTED( npZCB ) = 0 ;
   RECEIVED( npZCB ) = 0 ;
   TOTALFILES( npZCB ) = 0 ;
   BLOCKSIZE( npZCB ) = 1024 ;
   HFILE( npZCB ) = -1 ;

   if (lpFileName == NULL || *lpFileName == NULL)
      *FILENAME( npZCB ) = NULL ;
   else
      lstrcpy( FILENAME( npZCB ), lpFileName ) ;

   if (NULL != *FILENAME( npZCB ))
   {
      HFILE( npZCB ) = OpenXferFile( npZCB ) ;
      if (-1 == HFILE( npZCB ))
      {
         SendAbortString( hWnd ) ;
         DestroyZCB( npZCB ) ;
         return ( NULL ) ;
      }
   }

   // save starting time

   time( &TIMESTARTED( npZCB ) ) ;

   HDLG( npZCB ) =
      CreateDialogParam( hInstLib, MAKEINTRESOURCE( ZMODEMDLGBOX ),
                         hWnd, ZmodemDlgProc, MAKELONG( npZCB, 0 ) ) ;

   SetTimer( HDLG( npZCB ), TIMERID_ZMODEMFSM, TIMERINTERVAL_ZMODEMFSM,
             NULL ) ;

   // NEED: Check for window's status (e.g. is ICONIC?)

   ShowWindow( HDLG( npZCB ), SW_SHOW ) ;

   return ( HDLG( npZCB ) ) ;
                               
} // end of ZmodemReceive()

//------------------------------------------------------------------------
//  HWND FAR PASCAL ZmodemSend( HWND hWnd, LPSTR lpFileName )
//
//  Description:
//     Initiates protocol send of lpFileName.  Returns a handle
//     to the created dialog box.
//
//  Parameters:
//     HWND hWnd
//        handle to parent window
//
//     LPSTR lpFileName
//        filename to send
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

HWND FAR PASCAL ZmodemSend( HWND hWnd, LPSTR lpFileName )
{
   NPZCB       npZCB ;
   FILESTATUS  fsXferFile ;

   if (NULL == (npZCB = CreateZCB()))
   {
      DP1( hWDB, "CreateZCB failed!" ) ;
      SendAbortString( hWnd ) ;
      return( NULL ) ;
   }

   InitZmodem( npZCB ) ;

   HPARENTWND( npZCB ) = hWnd ;
   DIRECTION( npZCB ) = DIR_SEND ;
   STATE( npZCB ) = SENT_INIT ;
   CURFILE( npZCB ) = 0 ;
   FILEBYTES( npZCB ) = 0 ;
   TRANSMITTED( npZCB ) = 0 ;
   RECEIVED( npZCB ) = 0 ;
   TOTALFILES( npZCB ) = 0 ;
   HFILE( npZCB ) = -1 ;
   STREAMING( npZCB ) = FALSE ;
   USESTREAM( npZCB ) = FALSE ;

   if (lpFileName == NULL || *lpFileName == NULL)
   {
      SendAbortString( hWnd ) ;
      DestroyZCB( npZCB ) ;
      return ( NULL ) ;
   }

   lstrcpy( FILENAME( npZCB ), lpFileName ) ;

   if (NULL != *FILENAME( npZCB ))
   {
      HFILE( npZCB ) = OpenXferFile( npZCB ) ;
      if (-1 == HFILE( npZCB ))
      {
         SendAbortString( hWnd ) ;
         DestroyZCB( npZCB ) ;
         return ( NULL ) ;
      }
   }

   // get file information

   GetFileStatus( HFILE( npZCB ), &fsXferFile ) ;
   FILETIME( npZCB ) = fsXferFile.wTime ;
   FILEDATE( npZCB ) = fsXferFile.wDate ;
   FILESIZE( npZCB ) = fsXferFile.dwLength ;

   // save starting time

   time( &TIMESTARTED( npZCB ) ) ;

   HDLG( npZCB ) =
      CreateDialogParam( hInstLib, MAKEINTRESOURCE( ZMODEMDLGBOX ),
                         hWnd, ZmodemDlgProc, MAKELONG( npZCB, 0 ) ) ;

   SetTimer( HDLG( npZCB ), TIMERID_ZMODEMFSM, TIMERINTERVAL_ZMODEMFSM,
             NULL ) ;

   // NEED: Check for parent window's status (e.g. is ICONIC?)

   ShowWindow( HDLG( npZCB ), SW_SHOW ) ;

   return ( HDLG( npZCB ) ) ;
                               
} // end of ZmodemSend()

//------------------------------------------------------------------------
//  BOOL FAR PASCAL ZmodemDlgProc( HWND hDlg, UINT uMsg,
//                                 WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the ZMODEM dialog procedure.  Basically, a timer
//     is used to negotiate the state changes.  LOWORD( lParam )
//     on WM_INITDIALOG is _expected_ to contain the NPZCB.
//
//  Parameters:
//     Same as all standard dialog boxes.
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL FAR PASCAL ZmodemDlgProc( HWND hDlg, UINT uMsg,
                               WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         NPZCB  npZCB ;

         npZCB = (NPZCB) LOWORD( lParam ) ;
         SET_PROP( hDlg, ATOM_NPZCB, (HANDLE) npZCB ) ;

         // NEED: center dialog box and perform shadow
         // control init

         // initiate transfer sequence

         // NEED: change title bar to reflect direction

         if (DIR_RECEIVE == DIRECTION( npZCB ))
         {
            TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;
            XmitZRINITHdr( npZCB ) ;
            ReadAnyHdrInit( npZCB ) ;
         }
         else
         {
            TIMEOUT( npZCB ) = GetCurrentTime() + TIMEOUTINTERVAL( npZCB ) ;
            XmitZRQINIT( npZCB ) ;
            ReadAnyHdrInit( npZCB ) ;
         }

         UpdateDialogBox( npZCB ) ;

      }
      return ( TRUE ) ;

      case WM_MEASUREITEM:
         return ( DlgMeasureItem( hDlg, (LPMEASUREITEMSTRUCT) lParam ) ) ;

      case WM_DRAWITEM:
         return( DlgDrawItem( hDlg, (LPDRAWITEMSTRUCT) lParam ) ) ;

      case WM_CMD_RXBLK:
      {
         NPZCB  npZCB ;
         WORD   wBlkLen, wMaxLen ;

         npZCB = (NPZCB) GET_PROP( hDlg, ATOM_NPZCB ) ;

         wBlkLen = wParam ;
         while (wBlkLen)
         {
            DPF3( hWDB, "INCOMINGCOUNT = %d. INCOMINGPOS = %d\r\n",
                  INCOMINGCOUNT( npZCB ), INCOMINGPOS( npZCB ) ) ;

            // check for wrap-around

            if (INCOMINGCOUNT( npZCB ) < INCOMINGPOS( npZCB ))
               wMaxLen = (INCOMINGPOS( npZCB ) - INCOMINGCOUNT( npZCB )) - 1 ;
            else
               wMaxLen =
                  sizeof( INCOMING( npZCB ) ) - INCOMINGCOUNT( npZCB ) ;

            wMaxLen = min( wBlkLen, wMaxLen ) ;
            if (wMaxLen != 0)
            {
               time( &TIMEREAD( npZCB ) ) ;
               RECEIVED( npZCB ) += wMaxLen ;
               _fmemcpy( &INCOMING( npZCB )[ INCOMINGCOUNT( npZCB ) ],
                         (LPSTR) lParam, wMaxLen ) ;
               ((LPSTR) lParam) += wMaxLen ;
               INCOMINGCOUNT( npZCB ) += wMaxLen ;
               if (INCOMINGCOUNT( npZCB ) == sizeof( INCOMING( npZCB ) ))
                  INCOMINGCOUNT( npZCB ) = 0 ;
               wBlkLen -= wMaxLen ;
            }
            ZmodemFSM( hDlg ) ;
         }
      }
      return ( TRUE ) ;

      case WM_COMMAND:
      {
         switch (wParam)
         {
            case IDCANCEL:
            {
               NPZCB  npZCB ;

               npZCB = (NPZCB) GET_PROP( hDlg, ATOM_NPZCB ) ;
               TerminateZmodem( npZCB, ERROR_LOCALCANCEL ) ;
            }
            break ;
         }
         break ;
      }
      return ( TRUE ) ;

      case WM_TIMER:
         return ( ZmodemFSM( hDlg ) ) ;
             
      case WM_DESTROY:
         // NEED: perform shadow control call

         REMOVE_PROP( hDlg, ATOM_NPZCB ) ;
         break ;
   }
   return ( FALSE ) ;

} // end of ZmodemDlgProc()

//------------------------------------------------------------------------
//  BOOL NEAR DlgMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpMIS )
//
//  Description:
//     Computes the size of the item and fills the structure
//     appropriately.
//
//  Parameters:
//     HWND hWnd
//        handle to window
//
//     LPMEASUREITEMSTRUCT lpMIS
//        pointer to MEASUREITEMSTRUCT
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR DlgMeasureItem( HWND hWnd, LPMEASUREITEMSTRUCT lpMIS )
{
   RECT  rcClient ;

   if (ODT_BUTTON == lpMIS -> CtlType)   // it better be!
   {
      GetClientRect( GetDlgItem( hWnd, lpMIS -> CtlID), &rcClient ) ;
      lpMIS -> itemWidth = rcClient.left - rcClient.right ;
      lpMIS -> itemHeight = rcClient.bottom - rcClient.top ;
   }
   return ( TRUE ) ;

} // end of DlgMeasureItem()

//------------------------------------------------------------------------
//  BOOL NEAR DlgDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS )
//
//  Description:
//     Draws the item specified in lpDIS.
//
//  Parameters:
//     HWND hWnd
//        handle to window
//
//     LPDRAWITEMSTRUCT
//        pointer to DRAWITEMSTRUCT
//
//  History:   Date       Author      Comment
//              3/14/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR DlgDrawItem( HWND hWnd, LPDRAWITEMSTRUCT lpDIS )
{
   char   szBuf[ MAXLEN_TEMPSTR ] ;
   DWORD  dwExtent ;
   RECT   rc1, rc2 ;
   WORD   wHeight, wPct, wWidth, wXPos ;

   if (ODT_BUTTON == lpDIS -> CtlType)   // it better be!
   {
      // determine if user is trying to set focus...
      // if so, set focus to IDCANCEL

      if (ODA_FOCUS == lpDIS -> itemAction)
      {
         if (ODS_FOCUS & lpDIS -> itemState)
            SetFocus( GetDlgItem( hWnd, IDCANCEL ) ) ;

         return ( TRUE ) ;
      }

      if (ODA_DRAWENTIRE == lpDIS -> itemAction)
      {
         rc1 = lpDIS -> rcItem ;
         rc2 = lpDIS -> rcItem ;
         wWidth = rc1.right - rc1.left ;
         wHeight = rc1.bottom - rc1.top ;
         GetWindowText( lpDIS -> hwndItem, szBuf, MAXLEN_TEMPSTR ) ;
         wPct = LOWORD( AtoL( szBuf ) ) ;
         wXPos = LOWORD( (DWORD) wWidth * (DWORD) wPct / 100L ) ;
         rc1.right = wXPos ;
         rc2.left = wXPos ;
         dwExtent = GetTextExtent( lpDIS -> hDC, szBuf, lstrlen( szBuf ) ) ;

         SetBkColor( lpDIS -> hDC, GetSysColor( COLOR_HIGHLIGHT ) ) ;
         SetTextColor( lpDIS -> hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
         ExtTextOut( lpDIS -> hDC, (wWidth - LOWORD( dwExtent )) / 2,
                     (wHeight - HIWORD( dwExtent )) / 2,
                     ETO_OPAQUE | ETO_CLIPPED, &rc1, szBuf, lstrlen( szBuf ),
                     NULL ) ;

         SetBkColor( lpDIS -> hDC, GetSysColor( COLOR_WINDOW ) ) ;
         SetTextColor( lpDIS -> hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
         ExtTextOut( lpDIS -> hDC, (wWidth - LOWORD( dwExtent )) / 2,
                     (wHeight - HIWORD( dwExtent )) / 2,
                     ETO_OPAQUE | ETO_CLIPPED, &rc2, szBuf, lstrlen( szBuf ),
                     NULL ) ;
      }
   }
   return ( TRUE ) ;

} // end of DlgDrawItem()

//------------------------------------------------------------------------
//  BOOL NEAR UpdateDialogBox( NPZCB npZCB )
//
//  Description:
//     Updates the information in the ZMODEM transfer dialog box.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR UpdateDialogBox( NPZCB npZCB )
{
   char   szBuf[ MAXLEN_TEMPSTR ] ;
   HWND   hDlg ;
   DWORD  dwElapsedTime ;
   WORD   wCPS, wProgress ;

   hDlg = HDLG( npZCB ) ;

   // file name
   SetDlgItemText( hDlg, IDD_FILENAME, StripPath( FILENAME( npZCB ) ) ) ;

   // file size
   wsprintf( szBuf, "%lu", FILESIZE( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_FILESIZE, szBuf ) ;

   // total blocks

   wsprintf( szBuf, "%u", TOTALBLOCKS( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_TOTALBLOCKS, szBuf ) ;

   // block check

   // block count

   SetDlgItemInt( hDlg, IDD_BLOCKCOUNT, BLOCKCOUNT( npZCB ), FALSE ) ;

   // byte count

   wsprintf( szBuf, "%lu", FILEBYTES( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_BYTECOUNT, szBuf ) ;

   // total received

   wsprintf( szBuf, "%lu", RECEIVED( npZCB ) ) ;
   SetDlgItemText( hDlg, IDD_RECEIVED, szBuf ) ;

   // compute avg cps

   // NEED: Compute wrap-around midnight

   if (DIR_RECEIVE == DIRECTION( npZCB ))
   {
      dwElapsedTime = TIMEREAD( npZCB ) - TIMESTARTED( npZCB ) ;
      wCPS = LOWORD( RECEIVED( npZCB ) / dwElapsedTime ) ;
      wsprintf( szBuf, "%u", wCPS ) ;
      SetDlgItemText( hDlg, IDD_AVERAGECPS, szBuf) ;
   }

   // progress indicator

   if (FILESIZE( npZCB ) > 0)
      wProgress = LOWORD( 100L * FILEBYTES( npZCB) / FILESIZE( npZCB ) ) ;
   else
      wProgress = 0 ;
   wsprintf( szBuf, "%u%%", wProgress ) ;
   SetDlgItemText( hDlg, IDD_PROGRESS, szBuf ) ;

   // update immediately

   UpdateWindow( hDlg ) ;

   return ( TRUE ) ;

} // end of UpdateDialogBox()

//------------------------------------------------------------------------
//  NPZCB NEAR CreateZCB( VOID )
//
//  Description:
//     Creates a ZMODEM control block and returns
//     the near pointer to the ZCB.
//
//  Parameters:
//     None.
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

NPZCB NEAR CreateZCB( VOID )
{
   return ( (NPZCB) LocalAlloc( LPTR, sizeof( ZCB ) ) ) ;

} // end of CreateZCB()

//------------------------------------------------------------------------
//  VOID NEAR DestroyZCB( NPZCB npZCB )
//
//  Description:
//     Destroys (frees) the ZMODEM control block.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR DestroyZCB( NPZCB npZCB )
{
   LocalFree( npZCB ) ;

} // end of DestroyZCB()

//------------------------------------------------------------------------
//  HFILE NEAR OpenXferFile( NPZCB npZCB )
//
//  Description:
//     Opens the xfer file and should create backups if specified.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 9/92   BryanW      Wrote it,
//
//------------------------------------------------------------------------

HFILE NEAR OpenXferFile( NPZCB npZCB )
{
   HFILE     hFile ;
   OFSTRUCT  of ;

   // NEED:  Create backup files

   if (DIRECTION( npZCB ) == DIR_RECEIVE)
      hFile = OpenFile( FILENAME( npZCB ), &of, OF_CREATE ) ;
   else
      hFile = OpenFile( FILENAME( npZCB ), &of, OF_READ ) ;

   return ( hFile ) ;

} // end of OpenXferFile()

//------------------------------------------------------------------------
//  VOID NEAR CloseXferFile( NPZCB npZCB )
//
//  Description:
//     Closes the file specified in the NPZCB
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR CloseXferFile( NPZCB npZCB )
{
   if (-1 == HFILE( npZCB ))
      return ;

   // NEED: check if receiving file and set file date/time

   if (DIR_RECEIVE == DIRECTION( npZCB ))
   {
      FILESTATUS fsXferFile ;

      fsXferFile.wDate = FILEDATE( npZCB ) ;
      fsXferFile.wTime = FILETIME( npZCB ) ;
      SetFileStatus( HFILE( npZCB ), &fsXferFile ) ;
   }

   _lclose( HFILE( npZCB ) ) ;
   HFILE( npZCB ) = -1 ;

} // end of CloseXferFile()

//------------------------------------------------------------------------
//  BOOL NEAR UpdateErrorCount( NPZCB npZCB )
//
//  Description:
//     Updates the error count and checks to see if we
//     have too many consecutive errors.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

BOOL NEAR UpdateErrorCount( NPZCB npZCB )
{
   CONSECERRORS( npZCB )++ ;
   TOTALERRORS( npZCB )++ ;

   UpdateDialogBox( npZCB ) ;
   if (MAXERRORS( npZCB ) != 0 && CONSECERRORS( npZCB ) > MAXERRORS( npZCB ))
   {
      TerminateZmodem( npZCB, ERROR_TOOMANYERRORS ) ;
      return ( FALSE ) ;
   }
   return ( TRUE ) ;

} // end of UpdateErrorCount()

//------------------------------------------------------------------------
//  VOID NEAR InitZmodem( NPZCB npZCB )
//
//  Description:
//     Initializes the ZMODEM protocol information.
//
//  Parameters:
//     NPZCB npZCB
//        pointer to ZMODEM control block
//
//  History:   Date       Author      Comment
//              3/ 8/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

VOID NEAR InitZmodem( NPZCB npZCB )
{
   int  i ;

   // increment use count

   nUseCount++ ;

   BLOCKCOUNT( npZCB ) = 0 ;
   TOTALERRORS( npZCB ) = 0 ;
   CONSECERRORS( npZCB ) = 0 ;
   INCOMINGCOUNT( npZCB ) = 0 ;
   INCOMINGPOS( npZCB ) = 0 ;
   CTLESC( npZCB ) = FALSE ;

   // NEED: get setting for ZMODEM timeout (e.g. TIGHT, NORMAL, LOOSE)

   TIMEOUTINTERVAL( npZCB ) = 10000 ;   // normal is 10 seconds

   // NEED: make sure connector is an 8-bit connection,
   // also set XON/XOFF protocol, and flush ports

   // set up attention string

   for (i = 0; i < 12; i++)
   {
      ATTN( npZCB )[ i ] = ASCII_CAN ;
      ATTN( npZCB )[ i+12 ] = ASCII_BS ;
   }
   ATTN( npZCB )[ 24 ] = NULL ;

} // end of InitZmodem()

//------------------------------------------------------------------------
//  VOID NEAR CompleteZmodem( NP