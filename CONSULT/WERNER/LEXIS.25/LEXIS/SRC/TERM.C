/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  LEXIS(R) 2000 Version 2.5 For Windows
 *
 *      Module:  term.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *      System:  Microsoft C 6.00A, Windows 3.00
 *
 *     Remarks:
 *
 *    This module controls the terminal.
 *
 *   Revisions:  
 *     00.00.000  2/28/91 cek   first version
 *
 *
 ************************************************************************/
#include "..\inc\undef.h"
#include <windows.h>
#include <cmndll.h>
#include <memory.h>

#include "..\inc\lexis.h"
#include "..\inc\global.h"
#include "..\inc\term.h"
#include "..\inc\keyboard.h"
#include "..\inc\botstat.h"
#include "..\inc\connect.h"

#define SYSERROR(a)

//#define CHANGECURSOR
/************************************************************************
 * Imported variables
 ************************************************************************/
/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/
LONG FAR PASCAL
fnTerm( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) ;

BOOL NEAR PASCAL SizeTerm( HWND hWnd, short x, short y) ;
void NEAR PASCAL VscrollTerm( HWND hWnd, WORD wParam, LONG lParam ) ;
void NEAR PASCAL HscrollTerm( HWND hWnd, WORD wParam, LONG lParam ) ;
BOOL NEAR PASCAL PaintTerm( HWND hWnd, WORD wParam, LONG lParam ) ;
void NEAR PASCAL ClearTermBuf( PTERMBLOCK pTB ) ;
BOOL NEAR PASCAL SetupTermBlock( PTERMBLOCK pTB ) ;
void NEAR PASCAL SetPixelValues( PTERMBLOCK pTB ) ;
BOOL NEAR PASCAL InitTermBlock( PTERMBLOCK pTB ) ;
void NEAR PASCAL InvertBlock( HWND hWnd, PPOINT pptBeg, PPOINT pptEnd ) ;
void NEAR PASCAL MoveCursorTo( HWND hWnd, LONG lParam ) ;
short NEAR PASCAL PointToTermX( short x, PTERMBLOCK pTB ) ;
short NEAR PASCAL PointToTermY( short y, PTERMBLOCK pTB ) ;
short NEAR PASCAL PointToTermCharX( short x, PTERMBLOCK pTB ) ;
short NEAR PASCAL PointToTermCharY( short y, PTERMBLOCK pTB ) ;

void NEAR PASCAL StartSelection( PTERMBLOCK pTB, short x, short y ) ;
void NEAR PASCAL ChangeSelection( PTERMBLOCK pTB, short x, short y ) ;
void NEAR PASCAL EndSelection( PTERMBLOCK pTB, short x, short y ) ;
void NEAR PASCAL ResetSelection( PTERMBLOCK pTB ) ;
void NEAR PASCAL
HighlightArea( PTERMBLOCK pTB, short x1, short y1, short x2, short y2 ) ;

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * External functions
 ************************************************************************/
/*************************************************************************
 *  BOOL FAR PASCAL InitTerm( HANDLE hInst )
 *
 *  Description:
 *    This function initializes the window class used by
 *    the term window.
 *
 *  Type/Parameter
 *          Description
 *    HINST    hInst
 *    Applciation's instance.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL InitTerm( HANDLE hInst )
{
   WNDCLASS    wc ;

   //
   // Register the window class
   //
   wc.style         = CS_VREDRAW | CS_HREDRAW | CS_CLASSDC ;

   wc.lpfnWndProc   = fnTerm ;  
                                  
   wc.cbClsExtra    = 0 ;             
   wc.cbWndExtra    = NUMWNDEXTRA * sizeof(WORD) ;  
   wc.hInstance     = hInst ;      
   wc.hIcon         = NULL ;
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW ) ;
   wc.hbrBackground = GetStockObject( NULL_BRUSH ) ; 
   wc.lpszMenuName  = NULL ;
   wc.lpszClassName = "term" ;

   /* Register the window class and return success/failure code. */

   return RegisterClass( &wc ) ;

}/* InitTerm() */

/*************************************************************************
 *  HWND FAR PASCAL CreateTerm( HWND hwndParent )
 *
 *  Description: 
 *
 *    This function creates the term window.
 *
 *  Comments:
 *
 *************************************************************************/
HWND FAR PASCAL CreateTerm( HWND hwndParent )
{
   HWND           hwndTerm ;

   if (!(hwndTerm = CreateWindow(
                                 "term",
                                 "",
                                 WS_CHILD | WS_VSCROLL | WS_HSCROLL,
                                 0,
                                 0,
                                 0,
                                 0,
                                 hwndParent,
                                 0,
                                 GETHINST( hwndParent ),
                                 NULL )))
   {
      DP1( hWDB, "Term create failed!" ) ;
      return 0 ;
   }


   return hwndTerm ;

}/* CreateTerm() */

/*************************************************************************
 *  BOOL FAR PASCAL ResetTerm( HWND hwndTerm )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ResetTerm( HWND hwndTerm )
{
   LOCALHANDLE    lhTB ;

   if (lhTB = GETLHTB( hwndTerm ))
   {
      PTERMBLOCK  pTB ;
      RECT        rc ;

      pTB = LOCKTERMBLOCK( lhTB ) ;

      SetupTermBlock( pTB ) ;
      GetClientRect( hwndTerm, &rc ) ;
      SizeTerm( hwndTerm, rc.right - rc.left, rc.bottom - rc.top ) ;
      UNLOCKTERMBLOCK( lhTB ) ;
      SetTermPos( hwndTerm ) ;
   }

   InvalidateRect( hwndTerm, NULL, TRUE ) ;
   UpdateWindow( hwndTerm ) ;

   return TRUE ;

}/* ResetTerm() */

/*************************************************************************
 *  BOOL FAR PASCAL WriteTerm( HWND hwndTerm, LPSTR lpszBuf )
 *
 *  Description: 
 *
 *    This function writes the specified string to the "term" display.
 *    lpszBuf points to a buffer that contains text only.  It may be
 *    big enough to wrap to a new line, and this function will take care
 *    of that.  It does not contain control sequences, these are handled
 *    by other api's such as SetTermHightlight()...
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL WriteTerm( HWND hwndTerm, LPSTR lpszBuf )
{
   LOCALHANDLE    lhTB ;
   char           szBuf[NUMCOLS+1] ;
   PSTR           pszCur ;

   if (lhTB = GETLHTB( hwndTerm ))
   {
      PTERMBLOCK  pTB ;
      short       x, y ;
      short       nCharSizeX ;
      short       nCharSizeY ;
      BYTE        bCurAttr ;
      RECT        rcInvalid ;
      short       OffsetX, OffsetY ;

      pTB = LOCKTERMBLOCK( lhTB ) ;
      //
      // copy buf into temporary local var
      //
      lstrcpy( szBuf, lpszBuf ) ;

      x = pTB->nCurCol ;
      y = pTB->nCurRow ;
      bCurAttr = pTB->bCurAttr ;
      nCharSizeX = pTB->nCharSizeX ;
      nCharSizeY = pTB->nCharSizeY ;

      OffsetY = min( 0, VOFFSET( pTB ) ) ;

      OffsetX = min( 0, HOFFSET( pTB ) ) ;

      SetRectEmpty( &rcInvalid ) ;

      //
      // At the current position put the new text.  Invalidate the
      // rect...
      //
      for (pszCur = szBuf ; *pszCur != '\0' ; pszCur++)
      {
         switch( *pszCur )
         {
            case CHAR_CR:
               // go to beginning of line
               x = 0 ;
            break ;

            case CHAR_LF:
               // go to next line
               y++ ;
            break ;

            case CHAR_BS:
               // go back one char
               if (x)
                  x-- ;
               else
               {
                  x = NUMCOLS - 1 ;
                  if (y)
                     y-- ;                        
               }
            break ;

            default:
            {
               RECT  rc ;

               CHARYX( x, y ) = *pszCur ;
               ATTRYX( x, y ) = bCurAttr ;

               rc.top = y * nCharSizeY + OffsetY ;
               rc.left = x * nCharSizeX + OffsetX ;
               rc.bottom = rc.top + nCharSizeY ;
               rc.right = rc.left + nCharSizeX ;

               UnionRect( &rcInvalid, &rcInvalid, &rc ) ;

               x++ ;
            }
         } // switch

         if ( x == NUMCOLS )
         {
            x = 0 ;
            y++ ;
         }

         //
         // Scroll if new line on bottom
         //
         if (y == NUMROWS)
         {
            RECT  rc ;
            short i ;

            y-- ;
            memcpy( ROW_CHAR( 0 ), ROW_ATTR( 1 ),
                    ((NUMROWS-1) * (NUMCOLS + 1)) ) ;
            for (i = 0 ; i < NUMCOLS ; i++)
            {
               CHARYX( y, i ) = CHAR_SPACE ;
               ATTRYX( y, i ) = bCurAttr ;
            }
            rc.top = nCharSizeY * (NUMROWS - 2) + OffsetY ;
            rc.left = 0 + OffsetX ;
            rc.right = nCharSizeX * NUMCOLS ;
            rc.bottom = nCharSizeY + rc.top ;

            ScrollWindow( hwndTerm, 0, -nCharSizeY, NULL, NULL ) ;

            UnionRect( &rcInvalid, &rcInvalid, &rc ) ;
         }
      }

      pTB->nCurCol = x ;
      pTB->nCurRow = y ;
      pTB->nCurPosX = x * nCharSizeX ;
      pTB->nCurPosY = y * nCharSizeY ;

      UNLOCKTERMBLOCK( lhTB ) ;

      InvalidateRect( hwndTerm, &rcInvalid, FALSE ) ;

      SetTermPos( hwndTerm ) ;
   }


   return TRUE ;
}/* WriteTerm() */


/*************************************************************************
 *  BOOL FAR PASCAL GetTermScreen( HWND hwndTerm, LPSTR lpszBuf )
 *
 *  Description: 
 *
 *    This function fills the buffer pointed to by lpszBuf with the
 *    contents of the screen, minus higlight and underline.  The buffer
 *    must be at least ((NUMROWS * (NUMCOLS+2)) + 1) big.
 *
 *    All lines are terminated with CR/LF pairs and are NOT padded with
 *    spaces.
 *    The end of the buffer is NOT padded with spaces (i.e. it's zero
 *    terminated.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL GetTermScreen( HWND hwndTerm, LPSTR lpszBuf )
{
   return TRUE ;
}/* GetTermScreen() */

/*************************************************************************
 *  BOOL FAR PASCAL GetTermSelected( HWND hwndTerm, LPSTR lpszBuf )
 *
 *  Description: 
 *
 *    This function fills the buffer pointed to by lpszBuf with the
 *    text that is currently highlighted on the term display.  If
 *    there is no selection then the entire screen is returned.
 *    The buffer must be at least ((NUMROWS * (NUMCOLS+2)) + 1) big.
 *
 *    All lines are terminated with CR/LF pairs and are NOT padded with
 *    spaces.
 *    The end of the buffer is NOT padded with spaces (i.e. it's zero
 *    terminated.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL GetTermSelected( HWND hwndTerm, LPSTR lpszBuf )
{
   return TRUE ;
}/* GetTermSelected() */

/*************************************************************************
 *  BOOL FAR PASCAL
 *    GetTermAutoSize( HWND hwndTerm, short FAR *lpX, short FAR *lpY )
 *
 *  Description: 
 *
 *    This function returns the size that the terminal needs to show
 *    80x24 without scrollbars.
 *    
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL
GetTermAutoSize( HWND hwndTerm, short FAR *lpX, short FAR *lpY )
{
   LOCALHANDLE lhTB = GETLHTB( hwndTerm ) ;
   PTERMBLOCK  pTB ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      *lpX = pTB->nFullSizeX ;
      *lpY = pTB->nFullSizeY ;
      UNLOCKTERMBLOCK( lhTB ) ;
   }
   else
      return FALSE ;

   return TRUE ;
   
}/* GetTermAutoSize() */

/*************************************************************************
 *  BOOL FAR PASCAL ClearTerm( HWND hwndTerm )
 *
 *  Description: 
 *
 *    This function clears the term screen and homes the cursor.
 *
 *  Comments:
 *
 *    While this function is exported from this module it typically will
 *    not be called from elsewhere because this module should handle
 *    the clearing the screen via the text passed in WriteTerm().
 *
 *************************************************************************/
BOOL FAR PASCAL ClearTerm( HWND hwndTerm )
{
   LOCALHANDLE    lhTB ;

   if (lhTB = GETLHTB( hwndTerm ))
   {
      PTERMBLOCK  pTB ;

      pTB = LOCKTERMBLOCK( lhTB ) ;

      ClearTermBuf( pTB ) ;

      InvalidateRect( hwndTerm, NULL, TRUE ) ;

      pTB->nCurPosX = pTB->nCurPosY = pTB->nCurRow = pTB->nCurCol = 0 ;

      SetScrollPos( hwndTerm, SB_HORZ, pTB->nHscrollPos = 0, FALSE ) ;
      SetScrollPos( hwndTerm, SB_VERT, pTB->nVscrollPos = 0, FALSE ) ;
   }
   UNLOCKTERMBLOCK( lhTB ) ;

   return TRUE ;

}/* ClearTerm() */

/*************************************************************************
 *  BOOL FAR PASCAL HomeTerm( HWND hwndTerm )
 *
 *  Description: 
 *
 *    This function homes the cursor.  It DOES NOT clear the display.
 *
 *  Comments:
 *
 *    While this function is exported from this module it typically will
 *    not be called from elsewhere because this module should handle
 *    the homeing of the cursor via the text passed in WriteTerm().
 *
 *************************************************************************/
BOOL FAR PASCAL HomeTerm( HWND hwndTerm )
{
   return TRUE ;
}/* HomeTerm() */

/*************************************************************************
 *  BOOL FAR PASCAL SetTermHighlight( HWND hwndTerm, BOOL fHL )
 *
 *  Description: 
 *
 *    This function turns highlight on or off.
 *
 *  Comments:
 *
 *    While this function is exported from this module it typically will
 *    not be called from elsewhere because this module should handle
 *    the setting of attributes via the text passed in WriteTerm().
 *
 *************************************************************************/
BOOL FAR PASCAL SetTermHighlight( HWND hwndTerm, BOOL fHL )
{
   return TRUE ;
}/* SetTermHighlight() */

/*************************************************************************
 *  BOOL FAR PASCAL SetTermUnderline( HWND hwndTerm, BOOL fUL )
 *
 *  Description: 
 *
 *    This function turns Underline on or off.
 *
 *  Comments:
 *
 *    While this function is exported from this module it typically will
 *    not be called from elsewhere because this module should handle
 *    the setting of attributes via the text passed in WriteTerm().
 *
 *************************************************************************/
BOOL FAR PASCAL SetTermUnderline( HWND hwndTerm, BOOL fUL )
{
   return TRUE ;
}/* SetTermUnderline() */

/*************************************************************************
 *  BOOL FAR PASCAL LockTermKeyboard( HWND hwndTerm, BOOL fLock )
 *
 *  Description: 
 *
 *    This funciton locks and unlocks the keyboard.  If the bell is
 *    enabled, a beep will sound whenever the terminal is unlocked.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL LockTermKeyboard( HWND hwndTerm, BOOL fLock )
{
   LOCALHANDLE lhTB = GETLHTB( hwndTerm ) ;
   PTERMBLOCK  pTB ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      DP5( hWDB,"Someone has just %s the keyboard", (LPSTR)(fLock ? "LOCKED" : "UNLOCKED") ) ;
      pTB->fKeyLocked = fLock ;
      UNLOCKTERMBLOCK( lhTB ) ;
   }
   return TRUE ;
}/* LockTermKeyboard() */


/*************************************************************************
 *  BOOL FAR PASCAL ConnectTerm( HWND hwndTerm, BOOL fConnected )
 *
 *  Description: 
 *
 *    This function sets the terminal to connected or not-connected
 *    state.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL ConnectTerm( HWND hwndTerm, BOOL fConnected )
{
   LOCALHANDLE lhTB = GETLHTB( hwndTerm ) ;
   PTERMBLOCK  pTB ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      pTB->fConnected = fConnected ;
      UNLOCKTERMBLOCK( lhTB ) ;
   }

   ConnectDialog( hwndTerm ) ;

   return TRUE ;
}/* ConnectTerm() */



/*************************************************************************
 *  BOOL FAR PASCAL DestroyTerm( HWND hwndTerm )
 *
 *  Description: 
 *
 *    This function destroys the term window.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL DestroyTerm( HWND hwndTerm )
{
   if (hwndTerm)
   {
      BOOL b ;

      b = DestroyWindow( hwndTerm ) ;
      return b ;
   }
   else
      return FALSE ;

}/* DestroyTerm() */

/*************************************************************************
 *  BOOL FAR PASCAL HeyTerminalDoYouWantFocus( HWND hwndTerm )
 *
 *  Description: 
 *
 *    the name says it all.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL HeyTerminalDoYouWantFocus( HWND hwndTerm )
{
   LOCALHANDLE lhTB = GETLHTB( hwndTerm ) ;
   PTERMBLOCK  pTB ;
   BOOL        fWantFocus ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      fWantFocus = pTB->fWantFocus ;
      UNLOCKTERMBLOCK( lhTB ) ;
      return fWantFocus ;
   }

   return FALSE ;
}/* HeyTerminalDoYouWantFocus() */

/************************************************************************
 * Internal functions
 ************************************************************************/
/*************************************************************************
 *  LONG FAR PASCAL
 *    fnTerm( HWND hWnd, WORD iMessge, WORD wParam, LONG lParam ) ;
 *
 *  Description:
 *    Window proc for the term widnow
 *
 *
 *  Comments:
 *
 *************************************************************************/
LONG FAR PASCAL
fnTerm( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam ) 
{
   static BOOL    fCapturing ;

   switch( wMsg )
   {
      case WM_CREATE:
      {
         LOCALHANDLE    lhTB ;
         if (lhTB = ALLOCTERMBLOCK())
         {
            PTERMBLOCK pTB ;
            SetWindowWord( hWnd, GWW_LHTB, lhTB ) ;

            pTB = LOCKTERMBLOCK( lhTB ) ;

            pTB->hwndTerm = hWnd ;
            InitTermBlock( pTB ) ;
            SetupTermBlock( pTB ) ;

         }
         else
         {
            SetWindowWord( hWnd, GWW_LHTB, 0 ) ;
            DP1( hWDB,"Could not alloc terminal buffer") ;
            SYSERROR(1) ;
            return -1 ;
         }
         UNLOCKTERMBLOCK( lhTB ) ;
      }
      break ;

      case WM_SETFOCUS:
      {
         PTERMBLOCK  pTB ;
         LOCALHANDLE lhTB = GETLHTB( hWnd ) ;

         if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
         {
            if (!pTB->fKeyLocked)
            {
               CreateCaret( hWnd, NULL, pTB->nCharSizeX, pTB->nCharSizeY ) ;
               SetCaretPos( HOFFSET( pTB ) + pTB->nCurPosX,
                            VOFFSET( pTB ) + pTB->nCurPosY ) ;
               ShowCaret( hWnd ) ;
               pTB->fCaret = TRUE ;
               pTB->fWantFocus = TRUE ;
            }
            else
               SetFocus( NULL ) ;

            UNLOCKTERMBLOCK( lhTB ) ;
         }
      }
      break ;

      case WM_KILLFOCUS:
      {
         PTERMBLOCK  pTB ;
         LOCALHANDLE lhTB = GETLHTB( hWnd ) ;

         if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
         {
            if (pTB->fCaret)
            {
               HideCaret( hWnd ) ;
               DestroyCaret() ;
            }

            pTB->fCaret = FALSE ;

            UNLOCKTERMBLOCK( lhTB ) ;
         }
      }
      break ;

      case WM_SIZE:
         SizeTerm( hWnd, LOWORD( lParam ), HIWORD( lParam )) ;
      break ;

      case WM_VSCROLL:
         VscrollTerm( hWnd, wParam, lParam ) ;
      break ;

      case WM_HSCROLL:
         HscrollTerm( hWnd, wParam, lParam ) ;
      break ;

      case WM_KEYDOWN:
         //
         // Handle keystrokes related to terminal manipulation and pass
         // the rest on to command hanlder
         //
         if (!KeyDownTerm( hWnd, wParam, lParam ))
         {
            //KeyDownCommand( wParam, lParam ) ;
         }
      break ;


      case WM_CHAR:
         //
         // Echo keystrokes to screen, then pass them on to command
         // handler.  
         //
         CharTerm( hWnd, wParam, lParam ) ;
//         CharCommand( wParam, lParam ) ;
      break ;

      case WM_LBUTTONDOWN:
      {
         PTERMBLOCK  pTB ;
         LOCALHANDLE lhTB = GETLHTB( hWnd ) ;
      
         if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
         {
            ResetSelection( pTB ) ;

            if (!fCapturing)
            {
               fCapturing = TRUE ;
               SetCapture( hWnd ) ;
               #ifdef CHANGECURSOR
               SetCursor( LoadCursor( NULL, IDC_CROSS )) ;
               #endif
               StartSelection( pTB, PointToTermCharX( LOWORD( lParam ), pTB ),
                                       PointToTermCharY( HIWORD( lParam ), pTB ) ) ;
            }
         }
         UNLOCKTERMBLOCK( lhTB ) ;
      }
      break ;

      case WM_MOUSEMOVE:
         if (fCapturing)
         {
            PTERMBLOCK  pTB ;
            LOCALHANDLE lhTB = GETLHTB( hWnd ) ;
         
            if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
            {
               ChangeSelection( pTB, PointToTermCharX( LOWORD( lParam ), pTB ),
                                      PointToTermCharY( HIWORD( lParam ), pTB ) ) ;
               UNLOCKTERMBLOCK( lhTB ) ;
            }
         }
         else
         {   
            CheckKeyStates() ;
            return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
         }
      return 0 ;

      case WM_LBUTTONUP:
      {
         PTERMBLOCK  pTB ;
         LOCALHANDLE lhTB = GETLHTB( hWnd ) ;
      
         if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
         {
            if (fCapturing)
            {
               fCapturing = FALSE ;
               #ifdef CHANGECURSOR
               SetCursor( LoadCursor( NULL, IDC_ARROW )) ;
               #endif
               ReleaseCapture( ) ;

               EndSelection( pTB, PointToTermCharX( LOWORD( lParam ), pTB ),
                                    PointToTermCharY( HIWORD( lParam ), pTB ) ) ;
            }
            UNLOCKTERMBLOCK( lhTB ) ;
         }
      }
      return 0 ;

      case WM_RBUTTONDOWN:
         MoveCursorTo( hWnd, lParam ) ;
      break ;

      case WM_ERASEBKGND:
      {
         HBRUSH      hTempBr ;
         LONG        lRet ;
         PTERMBLOCK  pTB ;
         LOCALHANDLE lhTB = GETLHTB( hWnd ) ;

         if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
         {
            hTempBr = GetClassWord( hWnd, GCW_HBRBACKGROUND ) ;
            SetClassWord( hWnd, GCW_HBRBACKGROUND, pTB->hBrush ) ;
            lRet = DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
            SetClassWord( hWnd, GCW_HBRBACKGROUND, hTempBr ) ;
            UNLOCKTERMBLOCK( lhTB ) ;
         }
      }
      return 0L ;

      case WM_PAINT:
         PaintTerm( hWnd, wParam, lParam ) ;
      break ;

      case WM_DESTROY:
      {
         PTERMBLOCK  pTB ;
         LOCALHANDLE lhTB = GETLHTB( hWnd ) ;

         if (!lhTB)
            return 0L ;

         if (pTB = LOCKTERMBLOCK( lhTB ))
         {
            if (pTB->hFont)
               DeleteObject( pTB->hFont ) ;

            if (pTB->hBrush)
               DeleteObject( pTB->hBrush ) ;

            UNLOCKTERMBLOCK( lhTB ) ;
         }
         LocalFree( lhTB ) ;
      }
      break ;

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }
   return 0L ;
}/* fnTerm() */


/*************************************************************************
 *  BOOL NEAR PASCAL SizeTerm( HWND hWnd, WORD x, WORD y)
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL SizeTerm( HWND hWnd, short x, short y)
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB = GETLHTB( hWnd ) ;

   if (!lhTB)
   {
      DP1( hWDB,"WM_SIZE: Term ERROR! Could not GETLHTB") ;
      return FALSE ;
   }
   if (pTB = LOCKTERMBLOCK( lhTB ))
   {
      DP5( hWDB,"SizeTerm") ;
      pTB->nCurSizeX = x ;
      pTB->nCurSizeY = y ;

      if (pTB->nCharSizeX && pTB->nCharSizeY)
      {
         pTB->nVscrollMax = max( 0, NUMROWS - (y / pTB->nCharSizeY)) ;
         pTB->nVscrollPos = min( pTB->nVscrollPos, pTB->nVscrollMax ) ;

         SetScrollRange( hWnd, SB_VERT, 0, pTB->nVscrollMax, FALSE ) ;
         SetScrollPos( hWnd, SB_VERT, pTB->nVscrollPos, TRUE ) ;

         pTB->nHscrollMax = max( 0, NUMCOLS - (x / pTB->nCharSizeX)) ;
         pTB->nHscrollPos = min( pTB->nHscrollPos, pTB->nHscrollMax ) ;

         SetScrollRange( hWnd, SB_HORZ, 0, pTB->nHscrollMax, FALSE ) ;
         SetScrollPos( hWnd, SB_HORZ, pTB->nHscrollPos, TRUE ) ;

         if (pTB->fCaret)
            SetCaretPos( HOFFSET( pTB ) + pTB->nCurPosX,
                         VOFFSET( pTB ) + pTB->nCurPosY ) ;
      }
   }
   else
   {
      DP1( hWDB,"WM_SIZE: Term ERROR! Couldnot LocalLock") ;
   }

   UNLOCKTERMBLOCK( lhTB ) ;


   return TRUE ;
}/* SizeTerm() */


/*************************************************************************
 *  BOOL NEAR PASCAL SetTermPos( HWND hWnd ) 
 *
 *  Description:
 *
 *    This function sets the caret postition within the term.  If the
 *    caret would not be visible (because of scroll bars) this function
 *    scrolls the window so it's visible.
 *
 *    This function returns TRUE if the window was scrolled (and thus
 *    re painted) otherwise it returns FALSE.
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL FAR PASCAL SetTermPos( HWND hwndTerm )
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB ;
   BOOL        fMoved = FALSE ;

   lhTB  = GETLHTB( hwndTerm ) ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      RECT     rcVisible ;
      RECT     rcCaret ;
      RECT     rcTemp ;
      short cxChar, cyChar ;

      cxChar = pTB->nCharSizeX ;
      cyChar = pTB->nCharSizeY ;

      if (!pTB->fCaret || !cxChar || !cyChar)
      {
         UNLOCKTERMBLOCK( lhTB ) ;
         return FALSE ;
      }

      //
      // Is the current position not visible?  If it's not then
      // scrollwindow it into place.
      //
      GetClientRect( hwndTerm, &rcVisible ) ;

      rcCaret.top = (-pTB->nVscrollPos * cyChar) + pTB->nCurPosY ;
      rcCaret.left = (-pTB->nHscrollPos * cxChar) + pTB->nCurPosX ;
      rcCaret.bottom = rcCaret.top + cyChar ;
      rcCaret.right = rcCaret.left + cxChar ;

      //
      // Set the caret.  If we do end up scrolling below, the ScrollWindow
      // function will automatcially scroll the caret for us!
      //
      if (pTB->fCaret)
         SetCaretPos( rcCaret.left, rcCaret.top ) ;

      UnionRect( &rcTemp, &rcVisible, &rcCaret ) ;
      if (!EqualRect( &rcTemp, &rcVisible ))
      {
         short nVscrollAmt, nHscrollAmt ;

         fMoved = TRUE ;
         
         //
         // if the caret is above the visible...
         //
         if (rcCaret.top < 0)
            nVscrollAmt = rcCaret.top / cyChar ;
         else
         {
            if (rcCaret.bottom >= rcVisible.bottom - cyChar)
               nVscrollAmt = (rcCaret.bottom - rcVisible.bottom) /
                              cyChar + 1 ;
            else
               nVscrollAmt = 0 ;
         }
         //
         // if the caret is left of the visibile...
         //
         if (rcCaret.left < 0)
            nHscrollAmt = rcCaret.left / cxChar ;
         else
         {
            if (rcCaret.right >= rcVisible.right)
               nHscrollAmt =  (rcCaret.right - rcVisible.bottom) /
                              cxChar ;
            else
               nHscrollAmt = 0 ;
         }

         //
         // Set vertical scroll stuff
         //
         SetScrollPos( hwndTerm, SB_VERT, pTB->nVscrollPos += nVscrollAmt, TRUE ) ;

         // 
         // Set horiz scroll stuff
         //
         SetScrollPos( hwndTerm, SB_HORZ, pTB->nHscrollPos += nHscrollAmt, TRUE ) ;

         //
         // Scroll the window
         //
         ScrollWindow( hwndTerm, -nHscrollAmt * cxChar,
                                 -nVscrollAmt * cyChar, NULL, NULL ) ;
         UpdateWindow( hwndTerm ) ;
      }

      UNLOCKTERMBLOCK( lhTB ) ;
   }

   return fMoved ;
} /* SetTermPos() */



/*************************************************************************
 *  void NEAR PASCAL VscrollTerm( HWND hWnd, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL VscrollTerm( HWND hWnd, WORD wParam, LONG lParam )
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB ; 
   short  nVscrollInc = 0 ;

   lhTB  = GETLHTB( hWnd ) ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      switch( wParam )
      {
         case SB_TOP:
            nVscrollInc = -(pTB->nVscrollPos) ;
         break ;

         case SB_BOTTOM:
            nVscrollInc = pTB->nVscrollMax - pTB->nVscrollPos ;
         break ;

         case SB_LINEUP:
            nVscrollInc = -1 ;
         break ;

         case SB_LINEDOWN:
            nVscrollInc = 1 ;
         break ;

         case SB_PAGEUP:
            nVscrollInc = min( -1, -(pTB->nCurSizeY / pTB->nCharSizeY) ) ;
         break ;

         case SB_PAGEDOWN:
            nVscrollInc = max( 1, pTB->nCurSizeY / pTB->nCharSizeY ) ;
         break ;

         case SB_THUMBTRACK:
            nVscrollInc = LOWORD( lParam ) - pTB->nVscrollPos ;
         break ;

         default:
            nVscrollInc = 0 ;

      }
      if (nVscrollInc = max( -pTB->nVscrollPos,
                              min( nVscrollInc,
                                    pTB->nVscrollMax - pTB->nVscrollPos )))
      {
         pTB->nVscrollPos += nVscrollInc ;
         ScrollWindow( hWnd, 0,
                        -pTB->nCharSizeY * nVscrollInc, NULL, NULL ) ;
         SetScrollPos( hWnd, SB_VERT, pTB->nVscrollPos, TRUE ) ;
         UpdateWindow( hWnd ) ;
      }

      UNLOCKTERMBLOCK( lhTB ) ;
   }
   else
   {
      DP1( hWDB, "Could not local lock term block" ) ;
   }
}/* VscrollTerm() */


/*************************************************************************
 *  void NEAR PASCAL HscrollTerm( HWND hWnd, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL HscrollTerm( HWND hWnd, WORD wParam, LONG lParam )
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB ;
   short  nHscrollInc = 0 ;

   lhTB  = GETLHTB( hWnd ) ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      switch( wParam )
      {
         case SB_LINEUP:
            nHscrollInc = -1 ;
         break ;

         case SB_LINEDOWN:
            nHscrollInc = 1 ;
         break ;

         case SB_PAGEUP:
            nHscrollInc = -8 ;
         break ;

         case SB_PAGEDOWN:
            nHscrollInc = 8 ;
         break ;

         case SB_THUMBTRACK:
            nHscrollInc = LOWORD( lParam ) - pTB->nHscrollPos ;
         break ;

      }
      if (nHscrollInc = max( -pTB->nHscrollPos,
                              min( nHscrollInc,
                                    pTB->nHscrollMax -
                                    pTB->nHscrollPos )))
      {
         pTB->nHscrollPos += nHscrollInc ;
         ScrollWindow( hWnd, -pTB->nCharSizeX * nHscrollInc, 0, NULL, NULL ) ;
         SetScrollPos( hWnd, SB_HORZ, pTB->nHscrollPos, TRUE ) ;
//         UpdateWindow( hWnd ) ;
      }

      UNLOCKTERMBLOCK( lhTB ) ;
   }
   else
   {

      DP1( hWDB, "Could not local lock term block" ) ;
   }
}/* HscrollTerm() */


/*************************************************************************
 *  BOOL NEAR PASCAL PaintTerm( HWND hWnd, WORD wParam, LONG lParam )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL PaintTerm( HWND hWnd, WORD wParam, LONG lParam )
{
   PAINTSTRUCT ps ;
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB = GETLHTB( hWnd ) ;
   LPLEXISSTATE   lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
      return FALSE ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {
      short cxChar = pTB->nCharSizeX ;
      short cyChar = pTB->nCharSizeY ;

      if (BeginPaint( hWnd, &ps ) && cyChar && cxChar)
      {
         short       i ;
         short xStart ;
         short xEnd ;
         short yStart ;
         short yEnd ;
         short x, y ;
         short nHscrollPos = pTB->nHscrollPos ;
         short nVscrollPos = pTB->nVscrollPos ;
         HFONT hOldFont ;

         hOldFont = SelectObject( ps.hdc, pTB->hFont ) ;
         SetTextColor( ps.hdc, lpLS->lfNormal.lfFGcolor ) ;
         SetBkColor( ps.hdc, lpLS->lfNormal.lfBGcolor ) ;

         //
         // Figure out where in y direction to start
         //
         yStart = max( 0, nVscrollPos + ps.rcPaint.top / cyChar - 1 ) ;
         yEnd = min( NUMROWS, nVscrollPos + ps.rcPaint.bottom / cyChar + 1) ;

         //
         // Now figure out where in the x direction to start
         //
         xStart = max( 0, nHscrollPos + ps.rcPaint.left / cxChar ) ; //- 1 ) ;
         xEnd = min( NUMCOLS, nHscrollPos + ps.rcPaint.right / cxChar + 1 ) ; 

//         if (pTB->nSelStartX != -1)
//            HighlightArea( pTB, pTB->nSelStartX, pTB->nSelStartY,
//                                   pTB->nSelEndX,   pTB->nSelEndY ) ;
         for (i = yStart ; i < yEnd ; i++)
         {
            x = cxChar * ((-nHscrollPos) + xStart) ;

            y = cyChar * ((-nVscrollPos) + i) ;

            //
            // Only draw the parts of the line that are in the invalid
            // rectangle...
            //
            TextOut( ps.hdc, x, y, ROW_CHAR( i ) + xStart, xEnd - xStart ) ;


         }

         //
         // If the current text was selected then invert it
         //
         if (pTB->nSelStartX != -1)
            HighlightArea( pTB, pTB->nSelStartX, pTB->nSelStartY,
                                pTB->nSelEndX,   pTB->nSelEndY ) ;

         SelectObject( ps.hdc, hOldFont ) ;
         EndPaint( hWnd, &ps ) ;

      }
      else
      {
         DP1( hWDB,"BeginPaint failed in PaintTerm") ;
      }

      UNLOCKTERMBLOCK( lhTB ) ;
   }
   else
   {
      GlobalUnlock( ghGlobals ) ;
      return FALSE ;
   }

   GlobalUnlock( ghGlobals ) ;
   return TRUE ;

}/* PaintTerm() */

/*************************************************************************
 *  BOOL NEAR PASCAL SetupTermBlock( PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *
 *
 *  Comments:
 *
 *************************************************************************/
BOOL NEAR PASCAL SetupTermBlock( PTERMBLOCK pTB )
{
   LPLEXISSTATE   lpLS ;

   if (!(lpLS = (LPLEXISSTATE)GlobalLock( ghGlobals )))
      return FALSE ;

   //
   // Create fonts using current settings
   //
   if (pTB->hFont)
      DeleteObject( pTB->hFont ) ;

   pTB->hFont = CreateFontIndirect( (LPLOGFONT)&(lpLS->lfNormal) ) ;

   DP5( hWDB, "hFont = %d", pTB->hFont ) ;

   if (pTB->hBrush)
      DeleteObject( pTB->hBrush ) ;

   pTB->hBrush = CreateSolidBrush( (COLORREF)lpLS->lfNormal.lfBGcolor ) ;

   SetPixelValues( pTB ) ;

   GlobalUnlock( ghGlobals ) ;

   return TRUE ;

}/* SetupTermBlock() */

BOOL NEAR PASCAL InitTermBlock( PTERMBLOCK pTB )
{
   pTB->fConnected = FALSE ;
   pTB->fKeyLocked = TRUE ;
   pTB->fCaret = FALSE;
   pTB->fInsert = FALSE ;
   pTB->fWantFocus = TRUE ;

   pTB->nSelStartX = pTB->nSelStartY = pTB->nSelEndX = pTB->nSelEndY = -1 ;

   return TRUE ;
}
/*************************************************************************
 *  void NEAR PASCAL SetPixelValues( PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *    This guy sets the x and y pixel values for the current font.
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL SetPixelValues( PTERMBLOCK pTB )
{
   HDC         hDC ;
   TEXTMETRIC  tm ;
   HFONT       hOldFont ;

   if (!(hDC = GetDC( pTB->hwndTerm )))
      return ;

   //
   // Figure out character cell size
   //
   hOldFont = SelectObject( hDC, pTB->hFont ) ;
   GetTextMetrics( hDC, &tm ) ;
   pTB->nCharSizeX = tm.tmAveCharWidth ;
   pTB->nCharSizeY = tm.tmHeight + tm.tmExternalLeading ;

   pTB->nFullSizeX = pTB->nCharSizeX * NUMCOLS ;
   pTB->nFullSizeY = pTB->nCharSizeY * NUMROWS ;

   pTB->nCurPosX = pTB->nCharSizeX * pTB->nCurCol ;
   pTB->nCurPosY = pTB->nCharSizeY * pTB->nCurRow ;

   DP5( hWDB, "SetPixelValues: nCharSizeX = %d, nCharSizeY = %d",
              pTB->nCharSizeX, pTB->nCharSizeY ) ;

   SelectObject( hDC, hOldFont ) ;
   ReleaseDC( pTB->hwndTerm, hDC ) ;

}/* SetPixelValues() */


/*************************************************************************
 *  void NEAR PASCAL ClearTermBuf( PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 *************************************************************************/
void NEAR PASCAL ClearTermBuf( PTERMBLOCK pTB )
{
   WORD row ;
   WORD col ;

   pTB->nCurRow = pTB->nCurCol = 0 ;

   for (row = 0 ; row < NUMROWS ; row++)
      for (col = 0 ; col < NUMCOLS ; col++)
      {
         CHARYX( row, col ) = CHAR_SPACE ;
         ATTRYX( row, col ) = ATTR_NORMAL ;
      }

   #ifdef DEBUG
   for (col = 0 ; col < NUMCOLS ; col++)
      CHARYX( 0, col ) = '0' + (char)(col % 10) ;

   for (row = 1 ; row < NUMROWS ; row ++)
      CHARYX(row,0) = '0' + (char)(row % 10) ;
   #endif

}/* ClearTermBuf() */

/****************************************************************
 *  void NEAR PASCAL StartSelection( PTERMBLOCK pTB, short x, short y )
 *
 *  Description: 
 *
 *    This function starts the selection mechanism.  If there
 *    was a selection previously, the selection is turned off.
 *
 *    x and y are character positions
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL StartSelection( PTERMBLOCK pTB, short x, short y )
{
   short x1, x2 ;
   POINT pt1, pt2 ;

   //
   // If we get here, we know that there is no selection.
   // We also know where the selection is supposed to start.
   // So start selecting...
   //
   // First thing to do is do the "click and pick". 
   //
   // Go back and find the beginning of the word.
   //   
#if 0
   for (x1 = x ; x1 >= 0 ; x1-- )
      if (CHARYX( y, x1 ) == CHAR_SPACE)
      {
         if (x1 < NUMCOLS)
            x1++ ;
         break ;
      }

   if (x1 < 0)
      x1 = 0 ;


   //
   // now find end of word
   //
   for (x2 = x ; x2 < NUMCOLS ; x2++ )
      if (CHARYX( y, x2 ) == CHAR_SPACE)
      {
         if (x2 > 0)
            x2-- ;
         break ;
      }

   if (x2 == NUMCOLS)
      x2 = NUMCOLS - 1 ;
#endif
   x2 = x1 = x ;
   pTB->nSelStartX = x1 ;
   pTB->nSelEndX = x2 ;

   pTB->nSelStartY = pTB->nSelEndY = y ;

   //
   // highlight the selection
   //
   HighlightArea( pTB, x1, y, x2, y ) ;

   pTB->nCurSelX = x2 ;
   pTB->nCurSelY = y ;

   return ;

} /* StartSelection()  */

/****************************************************************
 *  void NEAR PASCAL ChangeSelection( PTERMBLOCK pTB, short x, short y )
 *
 *  Description: 
 *
 *    This function extends the current selection.
 *
 *    x and y are character positions
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL ChangeSelection( PTERMBLOCK pTB, short x, short y ) 
{
   if (pTB->nCurSelX == -1)
      return ;

   if (pTB->nCurSelX == x && pTB->nCurSelY == y)
      return ;

//   HighlightArea( pTB, pTB->nCurSelX, pTB->nCurSelY,
//                       pTB->nCurSelX, pTB->nCurSelY ) ;
//   HighlightArea( pTB, x, y, pTB->nCurSelX, pTB->nCurSelY ) ;
   HighlightArea( pTB, pTB->nSelStartX, pTB->nSelStartY,
                       pTB->nCurSelX, pTB->nCurSelY ) ;
   HighlightArea( pTB, pTB->nSelStartX, pTB->nSelStartY, x, y ) ;

   pTB->nCurSelX = x ;
   pTB->nCurSelY = y ;

} /* ChangeSelection()  */


/****************************************************************
 *  void NEAR PASCAL EndSelection( PTERMBLOCK pTB, short x, short y )
 *
 *  Description: 
 *
 *    This function ends the selection mechanism.
 *
 *    x and y are character positions
 *
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL EndSelection( PTERMBLOCK pTB, short x, short y )
{
   if (x >= pTB->nSelStartX && x <= pTB->nSelEndX && 
       y >= pTB->nSelStartY && y <= pTB->nSelEndY)
   {
      DP5( hWDB,"one char") ;
      return ;
   }

   if (y < pTB->nSelStartY)
   {
      pTB->nSelEndY = pTB->nSelStartY ;
      pTB->nSelStartY = y ;
      pTB->nSelEndX = pTB->nSelStartX ;
      pTB->nSelStartX = x ;
      DP5( hWDB,"y < pTB->nSelStartY" ) ;
      return ;
   }

   if (y == pTB->nSelStartY)
   {
      if (x < pTB->nSelStartX)
      {
         pTB->nSelEndX = pTB->nSelStartX ;
         pTB->nSelStartX = x ;
      }
      else
         pTB->nSelEndX = x ;

      pTB->nSelEndY = y ;
      DP5( hWDB,"y == pTB->nSelStartY" ) ;
   }
   else
   {
      pTB->nSelEndX = x ;
      pTB->nSelEndY = y ;
   }

   pTB->nCurSelX = -1 ;
   pTB->nCurSelY = -1 ;

   return ;
} /* EndSelection()  */

/****************************************************************
 *  void NEAR PASCAL ResetSelection( PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *    Unselects all that is selected.
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL ResetSelection( PTERMBLOCK pTB )
{
   POINT pt1, pt2 ;

   if (pTB->nSelStartX == -1)
      return ;

   HighlightArea( pTB, pTB->nSelStartX, pTB->nSelStartY,
                       pTB->nSelEndX, pTB->nSelEndY ) ;

   pTB->nSelStartX = pTB->nSelStartY = pTB->nSelEndX = pTB->nSelEndY = -1 ;
   pTB->nCurSelX = pTB->nCurSelY = -1 ;

   return ;

} /* ResetSelection()  */

/****************************************************************
 *  void NEAR PASCAL
 *    HighlightArea( PTERMBLOCK pTB, short x1, short y1,
 *                                        short x2, short y2 )
 *
 *  Description: 
 *
 *    Highlights the selection between characters x1,y1 and x2,y2
 *    inclusive.
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL
HighlightArea( PTERMBLOCK pTB, short x1, short y1, short x2, short y2 )
{
   RECT  rc ;
   HDC   hDC ;

   if (!(hDC = GetDC( pTB->hwndTerm )))
      return ;

   //
   // If the y passed in is less, then he's above so swap
   //
   if (y1 > y2)
   {
      register short temp ;
      temp  =  y1 ;
      y1    =  y2 ;
      y2    =  temp ;
      temp  =  x1 ;
      x1    =  x2 ;
      x2    =  temp ;
   }

   //
   // If y is on same line...
   //
   if (y1 == y2)
   {
      if (x1 > x2)
      {
         register short temp ;
         temp  =  x1 ;
         x1    =  x2 ;
         x2    =  temp ;
      }

      rc.left     = HOFFSET( pTB ) + (x1) * pTB->nCharSizeX ;
      rc.right    = (x2 - x1 + 1) * pTB->nCharSizeX ;

      rc.top      = VOFFSET( pTB ) + y1 * pTB->nCharSizeY ;
      rc.bottom   = pTB->nCharSizeY ;
      PatBlt( hDC, rc.left, rc.top, rc.right, rc.bottom, DSTINVERT ) ;
      DP5( hWDB,"y1 == y2 (%d, %d) (%d, %d)", x1, y1, x2, y2 ) ;
   }
   else
   {
      DP5( hWDB,"y1 != y2 (%d, %d) (%d, %d)", x1, y1, x2, y2 ) ;

      rc.left     = HOFFSET( pTB ) + (x1) * pTB->nCharSizeX ;
      rc.right    = (NUMCOLS - x1) * pTB->nCharSizeX ;

      rc.top      = VOFFSET( pTB ) + y1 * pTB->nCharSizeY ;
      rc.bottom   = pTB->nCharSizeY ;
      PatBlt( hDC, rc.left, rc.top, rc.right, rc.bottom, DSTINVERT ) ;

      if (y2 - y1 > 1)
      {
         rc.left     = 0 ;
         rc.right    = HOFFSET( pTB ) + NUMCOLS * pTB->nCharSizeX ;

         rc.top      = VOFFSET( pTB ) + (y1 + 1) * pTB->nCharSizeY ;
         rc.bottom   = (y2 - y1 - 1) * pTB->nCharSizeY ;
         PatBlt( hDC, rc.left, rc.top, rc.right, rc.bottom, DSTINVERT ) ;
      }
      rc.left     = 0 ;
      rc.right    = HOFFSET( pTB ) + (x2 + 1) * pTB->nCharSizeX ;

      rc.top      = VOFFSET( pTB ) + y2 * pTB->nCharSizeY ;
      rc.bottom   = pTB->nCharSizeY ;
      PatBlt( hDC, rc.left, rc.top, rc.right, rc.bottom, DSTINVERT ) ;
   }

   ReleaseDC( pTB->hwndTerm, hDC ) ;

} /* HighlightArea()  */

/****************************************************************
 *  void NEAR PASCAL InvertBlock( HWND hWnd, PPOINT pptBeg, PPOINT pptEnd )
 *
 *  Description: 
 *
 *    This function inverts a block. 
 *
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL InvertBlock( HWND hWnd, PPOINT pptBeg, PPOINT pptEnd )
{
   HDC hDC ;

   hDC = GetDC( hWnd ) ;

   PatBlt( hDC, pptBeg->x, pptBeg->y, pptEnd->x - pptBeg->x,
                                      pptEnd->y - pptBeg->y,
           DSTINVERT ) ;

   ReleaseDC( hWnd, hDC ) ;

} /* InvertBlock()  */

/****************************************************************
 *  void NEAR PASCAL MoveCursorTo( HWND hWnd, LONG lParam ) 
 *
 *  Description: 
 *
 *    this function moves the cursor to a point on the terminal.
 *    x and y are given in pixels on the visible display.  In other
 *    words, x and y are relative to 0,0 on the terminal window proper.
 *
 *  Comments:
 *
 ****************************************************************/
void NEAR PASCAL MoveCursorTo( HWND hWnd, LONG lParam )
{
   PTERMBLOCK  pTB ;
   LOCALHANDLE lhTB = GETLHTB( hWnd ) ;

   if (lhTB && (pTB = LOCKTERMBLOCK( lhTB )))
   {

      POINT pt ;
      pt = MAKEPOINT( lParam ) ;

      pTB->nCurPosX = PointToTermX( pt.x, pTB ) ;
      pTB->nCurPosY = PointToTermY( pt.y, pTB ) ;

      pTB->nCurCol = pTB->nCurPosX / pTB->nCharSizeX ;
      pTB->nCurRow = pTB->nCurPosY / pTB->nCharSizeY ;

      if (pTB->fCaret)
         SetCaretPos( HOFFSET( pTB ) + pTB->nCurPosX,
                        VOFFSET( pTB ) + pTB->nCurPosY ) ;

      UNLOCKTERMBLOCK( lhTB ) ;
   }

} /* MoveCursorTo()  */

/****************************************************************
 *  short NEAR PASCAL PointToTermX( short X, PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *    This function translates an X point that was recieved as input,
 *    say to a mouse click, and converts it to a x position on
 *    the terminal.  It always converts the point to the nearest
 *    top left point of a character cell.
 *
 *  Comments:
 *
 ****************************************************************/
short NEAR PASCAL PointToTermX( short x, PTERMBLOCK pTB )
{
   short xTemp ;

   xTemp = x - HOFFSET( pTB ) ;

   xTemp = max( 0, min( xTemp, pTB->nFullSizeX - 1) ) ; 
   
   return xTemp - (xTemp % pTB->nCharSizeX) ;

} /* PointToTermX()  */
  
/****************************************************************
 *  short NEAR PASCAL PointToTermY( short Y, PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *    This function translates an Y point that was recieved as input,
 *    say to a mouse click, and converts it to a y position on
 *    the terminal.  It always converts the point to the nearest
 *    top left point of a character cell.
 *
 *  Comments:
 *
 ****************************************************************/
short NEAR PASCAL PointToTermY( short y, PTERMBLOCK pTB )
{
   short yTemp ;

   yTemp = y - VOFFSET( pTB ) ;

   yTemp = max( 0, min( yTemp, pTB->nFullSizeY - 1) ) ;
   
   return yTemp - (yTemp % pTB->nCharSizeY) ;

} /* PointToTermY()  */

/****************************************************************
 *  short NEAR PASCAL PointToTermCharX( short x, PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *    This function translates an x point that was recieved as input
 *    to the nearest character in the terminal.
 *
 *  Comments:
 *
 ****************************************************************/
short NEAR PASCAL PointToTermCharX( short x, PTERMBLOCK pTB )
{
   short s ;
   //
   // First convert it to term pixels.
   //
   s = PointToTermX( x, pTB ) ;

   //
   // Do the division
   //
   return s / (pTB->nCharSizeX) ;

} /* PointToTermCharX()  */

/****************************************************************
 *  short NEAR PASCAL PointToTermCharY( short y, PTERMBLOCK pTB )
 *
 *  Description: 
 *
 *    This function translates an y point that was recieved as input
 *    to the nearest character in the terminal.
 *
 *  Comments:
 *
 ****************************************************************/
short NEAR PASCAL PointToTermCharY( short y, PTERMBLOCK pTB )
{
   short s ;
   //
   // First convert it to term pixels.
   //
   s = PointToTermY( y, pTB ) ;

   //
   // Do the division
   //
   return s / (pTB->nCharSizeY) ;

} /* PointToTermCharY()  */

/************************************************************************
 * End of File: term.c
 ************************************************************************/

