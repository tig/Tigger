/************************************************************************
 *
 *    Copyright (C) 1990 Charles E. Kindel, Jr.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *      Module:  dlghelp.c
 *
 *   Author(s):  Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:
 *
 *    This module contains dialog box and child window control helper
 *    functions.
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

#define WINDLL
#define STRICT

#include <windows.h>
#include <wdb.h>

#include "dlghelp.h"

/************************************************************************
 * Imported variables
 ************************************************************************/

/************************************************************************
 * Local Defines
 ************************************************************************/
#define MAX_STRLEN   256      // Maximum characters for ErrorResBox

/************************************************************************
 * Macros 
 ************************************************************************/

/************************************************************************
 * Local data structures
 ************************************************************************/

/************************************************************************
 * Internal APIs and data structures
 ************************************************************************/

/************************************************************************
 * Local Variables
 ************************************************************************/

/************************************************************************
 * window class names
 ************************************************************************/

/*************************************************************************
 *  WORD WINAPI
 *  DlgWhichRadioButton( HWND hDlg,
 *                       WORD wFirstID, WORD wLastID, WORD wDefault ) ;
 *
 *  Description:
 *
 *    This function returns the selected radio button in the group
 *    of radio buttons between wFirstID and wLastID inclusive.  If
 *    none are checked then wDefault is returned.
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hDlg
 *          Handle to the dialog box.
 *
 *    WORD  wFirstID
 *          Dialog ID of the first radio button in the group.
 *
 *    WORD  wLastID
 *          Dialog ID of the last radio button in the group.
 *
 *    WORD  wDefault
 *          Dialog ID of the radio button that is to be returned if
 *          no others are selected.
 * 
 *  Return Value
 *    Returns a WORD with the ID of the radio button selected.
 *
 *  Comments:
 *    Stolen from lexis 2000 1.7
 *
 *************************************************************************/
WORD WINAPI
DlgWhichRadioButton( HWND hDlg, WORD wFirstID, WORD wLastID, WORD wDefault ) 
{
   register WORD i;

   for (i = wFirstID; i <= wLastID; i++)
   {
      if (IsDlgButtonChecked( hDlg, i ))
      return i ;
   }

   return wDefault ;

}/* DlgWhichRadioButton() */


/*************************************************************************
 *  void WINAPI DlgSetItemLength( HWND hDlg, WORD wID, WORD wLength ) ;
 *
 *  Description: 
 *
 *    This function limits the length (in bytes) of text a user may enter
 *    into an edit control.
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hDlg
 *          Handle to the dialog box.  If wID is 0 then hDlg is the handle
 *          to the edit control window.
 *
 *    WORD  wID
 *          Dialog ID of the edit control.  If wID is 0 then hDlg is assumed
 *          to be the hWnd of the edit control.
 *
 *    WORD  wLength
 *          Specifies the maximum number of bytes that can be entered.  If the
 *          user attempts to enter more, the edit control beeps.  If wLength
 *          is 0, no limit is imposed on the size of the text (until out of
 *          memory.
 * 
 *  Comments:
 *
 *************************************************************************/
void WINAPI DlgSetItemLength( HWND hDlg, WORD wID, WORD wLength ) 
{
   if (wID)
      SendDlgItemMessage( hDlg, wID, EM_LIMITTEXT, (WPARAM)wLength, 0L ) ;
   else
      SendMessage( hDlg, EM_LIMITTEXT, (WPARAM)wLength, 0L ) ;

}/* DlgSetItemLength() */

/*************************************************************************
 *  WORD WINAPI LBGetCurSel( HWND hLB )
 *
 *  Description:
 *    Gets the current selection of a single selection listbox.
 *
 *  Returns:
 *    LB_ERR if it fails.
 *
 *  Comments:
 *
 *************************************************************************/
WORD WINAPI LBGetCurSel( HWND hLB )
{
   LRESULT dw ;

   if ((LRESULT)LB_ERR == (dw = SendMessage( hLB, LB_GETCURSEL, (WPARAM)0,
                                                               (LPARAM)0L )))
      return LB_ERR ;
   else
      return LOWORD( (DWORD)dw ) ;

}/* LBGetCurSel() */

/*************************************************************************
 *  BOOL WINAPI LBGetText( HWND hLB, WORD wItem, LPSTR lpszText, WORD wMax )
 *
 *  Description:
 *    Gets the text of the specified list box item or the current listbox
 *    item if wItem is 0xFFFF.
 *
 *  Type/Parameter
 *          Description
 *    HWND  hLB
 *          Handle to the list box.
 *
 *    WORD  wItem
 *          Item to get.  if this is 0xFFFF then the current selection is
 *          returned.
 *
 *    LPSTR lpszText
 *          Points to a buffer to receive the text.
 *
 *    WORD  wMax
 *          Maximum length of the buffer.  Buffer must be at least wMax big.
 * 
 *  Return Value
 *    TRUE  Successful.
 *    FALSE wItem does not exist or other error.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL WINAPI LBGetText( HWND hLB, WORD wItem, LPSTR lpszText, WORD wMax )
{
   LOCALHANDLE lhMem ;
   LPSTR lpszBuf ;
   DWORD dwLen ;


   if (wItem == 0xFFFF)
      wItem = LBGetCurSel( hLB ) ;

   if ((LRESULT)LB_ERR == ((LRESULT)dwLen = SendMessage( hLB, LB_GETTEXTLEN,
                                               (WPARAM)wItem, 0L )))
      return FALSE ;

   lhMem = LocalAlloc( LHND, LOWORD(dwLen+1) ) ;

   if (lhMem)
   {
      lpszBuf = (LPSTR)LocalLock( lhMem ) ;

      SendMessage( hLB, LB_GETTEXT, (WPARAM)wItem, (LPARAM)lpszBuf ) ;
      lpszBuf[wMax] = '\0' ;
      lstrcpy( lpszText, lpszBuf ) ;
      LocalUnlock( lhMem ) ;
   }

   LocalFree( lhMem ) ;

   return TRUE ;      

}/* LBGetText() */

/*************************************************************************
 *  BOOL WINAPI LBAddString( HWND hLB, LPSTR lpszText )
 *
 *  Description:
 *    Adds a string to the list box.  If the list box is not sorted, the
 *    string is added to the end.  If the list is sorted the string
 *    is inserted in sorted order.
 *
 *  Type/Parameter
 *          Description
 *    hLB   HWND  Handle to the list box.
 *
 *    lpszText LPSTR  Text string to add.
 * 
 *  Return Value
 *
 *    Returns LB_ERR if an error occurs.  Or LB_ERRSPACE if there is not
 *    enough space for the string.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL WINAPI LBAddString( HWND hLB, LPSTR lpszText )
{
   LRESULT dw ;

   if ((LRESULT)LB_ERR == (dw = SendMessage( hLB, LB_ADDSTRING,
                                           (WPARAM)0,
                                           (LPARAM)lpszText )))
      return LB_ERR ;

   if ((DWORD)LB_ERRSPACE == (DWORD)dw)
      return LB_ERR ;
   else
      return LOWORD( (DWORD)dw ) ;

}/* LBAddString() */

/*************************************************************************
 *  WORD WINAPI LBFindString( HWND hLB, WORD wStart, LPSTR lpszPrefix )
 *
 *  Description:
 *    Finds the first string in the list box which matches the given prefix
 *    text.
 *
 *  Type/Parameter
 *          Description
 *
 *    hLB      HWND  Handle to the list box.
 *
 *    wStart   WORD  Index to the item before the first item to be searched.
 *                   The search wraps at the end.  If this param is 0xFFFF
 *                   the entire lb is searched from the beginning.
 *    lpszPrefix LPSTR Text to search for.
 * 
 *  Return Value
 *
 *    Returns the index of the matching item of LB_ERR if the search failed.
 *
 *  Comments:
 *
 *************************************************************************/
WORD WINAPI LBFindString( HWND hLB, WORD wStart, LPSTR lpszPrefix )
{
   LRESULT dw ;

   if ((LRESULT)LB_ERR == (dw = SendMessage( hLB, LB_FINDSTRING,
                                           (WPARAM)wStart,
                                           (LPARAM)lpszPrefix ) ))
      return LB_ERR ;
   else
      return LOWORD( (DWORD)dw ) ;

}/* LBFindString() */

/*************************************************************************
 *  WORD WINAPI LBSelectString( HWND hLB, WORD wStart, LPSTR lpszPrefix )
 *
 *  Description:
 *    Changes the current selection to the first string that has the
 *    specified prefix.
 *
 *  Type/Parameter
 *          Description
 *
 *    hLB      HWND  Handle to the list box.
 *
 *    wStart   WORD  Index to the item before the first item to be searched.
 *                   The search wraps at the end.  If this param is 0xFFFF
 *                   the entire lb is searched from the beginning.
 *    lpszPrefix LPSTR Text to search for.
 * 
 *  Return Value
 *
 *    Returns the index of the matching item of LB_ERR if the search failed.
 *
 *  Comments:
 *
 *************************************************************************/
WORD WINAPI LBSelectString( HWND hLB, WORD wStart, LPSTR lpszPrefix )
{
   LRESULT dw ;

   if ((LRESULT)LB_ERR == (dw = SendMessage( hLB, LB_SELECTSTRING,
                                    (WPARAM)wStart,
                                    (LPARAM)lpszPrefix ) ))
      return LB_ERR ;
   else
      return LOWORD( (DWORD)dw ) ;

}/* LBSelectString() */


/*************************************************************************
 *  short FAR _cdecl ErrorResBox( HWND hWnd, HANDLE hInst, WORD wFlags,
 *                                WORD idAppName, WORD idErrorStr,
 *                                LPSTR lpszParams, ... ) ;
 *
 *  Description:
 *
 *  This function displays a message box using program resource strings.
 *       
 *  Type/Parameter
 *          Description
 *
 *      HWND hWnd
 *            Specifies the message box parent window.
 *
 *      HANDLE hInst
 *            Specifies the handle to the module that contains the resource
 *            strings specified by idAppName and idErrorStr.  If this value
 *            is NULL, the instance handle is obtained from hWnd (in which
 *            case hWnd may not be NULL).
 *
 *      WORD wFlags
 *            Specifies message box types controlloing the message box
 *            appearance.  All message box types valid for MessageBox are valid.
 *      
 *      WORD idAPpName
 *            Specifies the resource ID of a string that
 *            is to be used as the message box caption.
 * 
 *      WORD idErrorStr
 *             Specifies the resource ID of a error message format string.
 *             This string is of the style passed to wsprintf(),
 *             containing the standard C argument formatting
 *             characters.  Any procedure parameters following
 *             idErrorStr will be taken as arguments for this format
 *             string.
 * 
 *      arguments [ arguments, ... ]
 *             Specifies additional arguments corresponding to the
 *             format specification given by idErrorStr.  All
 *             string arguments must be FAR pointers.
 * 
 * 
 *  Return Value
 *    Returns the result of the call to MessageBox().  If an
 *    error occurs, returns zero.
 *
 *  Comment:
 *       This is a variable arguments function, the parameters after
 *       idErrorStr being taken for arguments to the printf() format
 *       string specified by idErrorStr.  The string resources specified
 *       by idAppName and idErrorStr must be loadable using the
 *       instance handle hInst.  If the strings cannot be
 *       loaded, or hwnd is not valid, the function will fail and return
 *       zero.
 *
 *************************************************************************/
short FAR _cdecl ErrorResBox( HWND hwnd, HINSTANCE hInst, WORD flags,
                        		WORD idAppName, WORD idErrorStr, ...)
{
   PSTR	sz ;
   PSTR	szFmt ;
   WORD	w ;

   DP( hWDB, "ErrorResBox( 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X )",
       hwnd,   hInst,  flags,  idAppName, idErrorStr ) ;
   if (!hInst)
   {
      if (!hwnd)
      {
         MessageBeep( 0 ) ;
         return FALSE ;
      }
      hInst = (HINSTANCE)GetWindowWord( hwnd, GWW_HINSTANCE ) ;
   }

   sz = (PSTR) LocalAlloc( LPTR, MAX_STRLEN ) ;
   szFmt = (PSTR) LocalAlloc( LPTR, MAX_STRLEN ) ;
   w = 0 ;

   if (!LoadString( hInst, idErrorStr, szFmt, MAX_STRLEN ))
      goto ExitError ;

   wvsprintf( sz, szFmt, (LPSTR)(((WORD FAR *)&idErrorStr) + 1) ) ;

   if (!LoadString( hInst, idAppName, szFmt, MAX_STRLEN ))
      goto ExitError ;
   w = MessageBox( hwnd, sz, szFmt, flags ) ;

   ExitError :
   LocalFree( (HANDLE) sz) ;
   LocalFree( (HANDLE) szFmt) ;
   return w ;

}/* ErrorResBox() */

/*************************************************************************
 *  void WINAPI
 *  DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient )
 *
 *  Description:
 *
 *    Centers a window within another window.
 *
 *  Type/Parameter
 *          Description
 *
 *    HWND  hwndCenter
 *          Window to center.  This does not have to be a child of
 *          hwndWithin.
 *    
 *    HWND  hwndWithin
 *          Window to center the above window within.  Can be NULL.
 *
 *    BOOL  fClient
 *          If TRUE the window is centered within the available client
 *          area.  Otherwise it's centered within the entire window area.
 * 
 *  Comments:
 *
 *************************************************************************/
void WINAPI
DlgCenter( HWND hwndCenter, HWND hwndWithin, BOOL fClient )
{
   RECT rcWithin ;
   RECT rcCenter ;
   int   x, y ;

//   DP( hWDB, "DlgCenter: hwndCenter = %d, hwndWithin = %d, fClient = %d",
//       hwndCenter, hwndWithin, fClient ) ;

   if (!IsWindow(hwndCenter))
   {
      DP( hWDB,"DlgCenter: hwndCenter is not a window") ;
      return ;
   }

   if (hwndWithin && !IsWindow(hwndWithin))
   {
      DP( hWDB,"DlgCenter: hwndWithin is not a window or NULL") ;
      return ;
   }

   if (hwndWithin == NULL)
   {
      rcWithin.left = rcWithin.top = 0 ;
      rcWithin.right = GetSystemMetrics( SM_CXSCREEN ) ;
      rcWithin.bottom = GetSystemMetrics( SM_CYSCREEN ) ;
   }
   else
   {
      HWND hwndParent = GetParent( hwndCenter ) ;

      if (fClient)
      {
         GetClientRect( hwndWithin, (LPRECT)&rcWithin ) ;

         //
         // Convert to screen coords
         //
         if (hwndWithin)
         {
            ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.left ) ;
            ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.right ) ;
         }

         //
         // Convert parent coords
         //
         if (hwndParent)
         {
            ScreenToClient( hwndParent, (LPPOINT)&rcWithin.left ) ;
            ScreenToClient( hwndParent, (LPPOINT)&rcWithin.right ) ;
         }
      }
      else
      {
            GetWindowRect( hwndWithin, (LPRECT) &rcWithin ) ;
            //
            // Convert to parent coords
            //
            if (hwndParent)

            {
               ScreenToClient( hwndParent, (LPPOINT)&rcWithin.left ) ;
               ScreenToClient( hwndParent, (LPPOINT)&rcWithin.right ) ;
            }

      }
   }

   GetWindowRect( hwndCenter, (LPRECT)&rcCenter ) ;
   if (hwndWithin)
   {
      ScreenToClient( hwndWithin, (LPPOINT)&rcCenter.left ) ;
      ScreenToClient( hwndWithin, (LPPOINT)&rcCenter.right ) ;
   }

   x = (rcWithin.left + rcWithin.right  + rcCenter.left - rcCenter.right)  / 2 ;
   y = (rcWithin.top  + rcWithin.bottom + rcCenter.top  - rcCenter.bottom) / 2 ;

   //
   // if it's a popup
   //
   if (hwndWithin == GetParent( hwndCenter ) &&
       !((LONG)GetWindowLong( hwndCenter, GWL_STYLE ) & WS_CHILD ))
   {
      GetWindowRect( hwndWithin, (LPRECT) &rcWithin ) ;
      x += rcWithin.left ;
      y += rcWithin.top ;
   }

   SetWindowPos( hwndCenter, NULL, max(x,0), max(y,0),
                  0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER ) ;

   return ;

}/* DlgCenter() */


/************************************************************************
 * End of File: dlghelp.c
 ************************************************************************/


