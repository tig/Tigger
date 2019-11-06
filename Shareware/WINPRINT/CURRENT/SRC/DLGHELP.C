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

#include "PRECOMP.H"

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
 *  UINT WINAPI
 *  DlgWhichRadioButton( HWND hDlg,
 *                       UINT wFirstID, UINT wLastID, UINT wDefault ) ;
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
 *    UINT  wFirstID
 *          Dialog ID of the first radio button in the group.
 *
 *    UINT  wLastID
 *          Dialog ID of the last radio button in the group.
 *
 *    UINT  wDefault
 *          Dialog ID of the radio button that is to be returned if
 *          no others are selected.
 * 
 *  Return Value
 *    Returns a UINT with the ID of the radio button selected.
 *
 *  Comments:
 *    Stolen from lexis 2000 1.7
 *
 *************************************************************************/
UINT WINAPI
DlgWhichRadioButton( HWND hDlg, UINT wFirstID, UINT wLastID, UINT wDefault ) 
{
   register UINT i;

   for (i = wFirstID; i <= wLastID; i++)
   {
      if (IsDlgButtonChecked( hDlg, i ))
      return i ;
   }

   return wDefault ;

}/* DlgWhichRadioButton() */

#if 0

/*************************************************************************
 *  void WINAPI DlgSetItemLength( HWND hDlg, UINT wID, UINT wLength ) ;
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
 *    UINT  wID
 *          Dialog ID of the edit control.  If wID is 0 then hDlg is assumed
 *          to be the hWnd of the edit control.
 *
 *    UINT  wLength
 *          Specifies the maximum number of bytes that can be entered.  If the
 *          user attempts to enter more, the edit control beeps.  If wLength
 *          is 0, no limit is imposed on the size of the text (until out of
 *          memory.
 * 
 *  Comments:
 *
 *************************************************************************/
void WINAPI DlgSetItemLength( HWND hDlg, UINT wID, UINT wLength ) 
{
   if (wID)
      SendDlgItemMessage( hDlg, wID, EM_LIMITTEXT, wLength, 0L ) ;
   else
      SendMessage( hDlg, EM_LIMITTEXT, wLength, 0L ) ;

}/* DlgSetItemLength() */

/*************************************************************************
 *  UINT WINAPI LBGetCurSel( HWND hLB )
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
UINT WINAPI LBGetCurSel( HWND hLB )
{
   DWORD dw ;

   if ((DWORD)LB_ERR == (dw = SendMessage( hLB, LB_GETCURSEL, 0, 0L )))
      return (UINT)LB_ERR ;
   else
#ifdef WIN32
     return dw ;
#else
     return LOWORD( dw ) ;
#endif

}/* LBGetCurSel() */

/*************************************************************************
 *  BOOL WINAPI LBGetText( HWND hLB, UINT wItem, LPSTR lpszText, UINT wMax )
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
 *    UINT  wItem
 *          Item to get.  if this is 0xFFFF then the current selection is
 *          returned.
 *
 *    LPSTR lpszText
 *          Points to a buffer to receive the text.
 *
 *    UINT  wMax
 *          Maximum length of the buffer.  Buffer must be at least wMax big.
 * 
 *  Return Value
 *    TRUE  Successful.
 *    FALSE wItem does not exist or other error.
 *
 *  Comments:
 *
 *************************************************************************/
BOOL WINAPI LBGetText( HWND hLB, UINT wItem, LPSTR lpszText, UINT wMax )
{
   LOCALHANDLE lhMem ;
   LPSTR lpszBuf ;
   DWORD dwLen ;


   if (wItem == 0xFFFF)
      wItem = LBGetCurSel( hLB ) ;

   if ((DWORD)LB_ERR == (dwLen = SendMessage( hLB, LB_GETTEXTLEN, wItem, 0L )))
      return FALSE ;

   lhMem = LocalAlloc( LHND, LOWORD(dwLen+1) ) ;

   if (lhMem)
   {
      lpszBuf = (LPSTR)LocalLock( lhMem ) ;

      SendMessage( hLB, LB_GETTEXT, wItem, (LONG)lpszBuf ) ;
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
   DWORD dw ;

   if ((DWORD)LB_ERR == (dw = SendMessage( hLB, LB_ADDSTRING,
                                           0, (LONG)lpszText )))
      return LB_ERR ;

   if ((DWORD)LB_ERRSPACE == dw)
      return LB_ERR ;
   else
      return LOWORD( dw ) ;

}/* LBAddString() */

/*************************************************************************
 *  UINT WINAPI LBFindString( HWND hLB, UINT wStart, LPSTR lpszPrefix )
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
 *    wStart   UINT  Index to the item before the first item to be searched.
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
UINT WINAPI LBFindString( HWND hLB, UINT wStart, LPSTR lpszPrefix )
{
   DWORD dw ;

   if ((DWORD)LB_ERR == (dw = SendMessage( hLB, LB_FINDSTRING, wStart,
                                    (LONG)lpszPrefix ) ))
      return (UINT)LB_ERR ;
   else
      return LOWORD( dw ) ;

}/* LBFindString() */

/*************************************************************************
 *  UINT WINAPI LBSelectString( HWND hLB, UINT wStart, LPSTR lpszPrefix )
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
 *    wStart   UINT  Index to the item before the first item to be searched.
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
UINT WINAPI LBSelectString( HWND hLB, UINT wStart, LPSTR lpszPrefix )
{
   DWORD dw ;

   if ((DWORD)LB_ERR == (dw = SendMessage( hLB, LB_SELECTSTRING, wStart,
                                    (LONG)lpszPrefix ) ))
      return (UINT)LB_ERR ;
   else
      return LOWORD( dw ) ;

}/* LBSelectString() */

#endif

/*************************************************************************
 *  short FAR _cdecl ErrorResBox( HWND hWnd, HANDLE hInst, UINT wFlags,
 *                                UINT idAppName, UINT idErrorStr,
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
 *      UINT wFlags
 *            Specifies message box types controlloing the message box
 *            appearance.  All message box types valid for MessageBox are valid.
 *      
 *      UINT idAPpName
 *            Specifies the resource ID of a string that
 *            is to be used as the message box caption.
 * 
 *      UINT idErrorStr
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
short FAR _cdecl ErrorResBox( HWND hwnd, HANDLE hInst, UINT flags,
                        		UINT idAppName, UINT idErrorStr, ...)
{
   PSTR	sz ;
   PSTR	szFmt ;
   UINT	w ;

   DP( hWDB, "ErrorResBox( 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X )",
       hwnd,   hInst,  flags,  idAppName, idErrorStr ) ;
   if (!hInst)
   {
      if (!hwnd)
      {
         MessageBeep( 0 ) ;
         return FALSE ;
      }
      hInst = GetWindowInstance( hwnd ) ;
   }

   sz = (PSTR) LocalAlloc( LPTR, MAX_STRLEN ) ;
   szFmt = (PSTR) LocalAlloc( LPTR, MAX_STRLEN ) ;
   w = 0 ;

   if (!LoadString( hInst, idErrorStr, szFmt, MAX_STRLEN ))
      goto ExitError ;

   wvsprintf( sz, szFmt, (LPSTR)(((UINT FAR *)&idErrorStr) + 1) ) ;

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
       !(GetWindowLong( hwndCenter, GWL_STYLE ) & WS_CHILD ))
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


