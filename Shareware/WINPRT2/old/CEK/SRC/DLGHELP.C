/************************************************************************
 *
 *      Module:  dlghelp.c
 *
 ************************************************************************/
#include "PRECOMP.H"
#include "CEK.h"

#define USE_MESSAGEBOX

/************************************************************************
 * Local Defines
 ************************************************************************/
#define MAX_STRLEN   2048      // Maximum characters for ErrorResBox

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
short FAR _cdecl ErrorResBox( HWND hwnd, HINSTANCE hInst, UINT flags,
                              UINT idAppName, UINT idErrorStr, ...)
{
   LPSTR	sz = NULL ;
   LPSTR	szFmt = NULL ;
   UINT	w ;

   if (!hInst)
   {
      if (!hwnd)
      {
         MessageBeep( 0 ) ;
         return FALSE ;
      }

      hInst = GetWindowInstance( hwnd ) ;
   }

   if (szFmt = GlobalAllocPtr( GHND, 256 + 1 ))
   {
      if (!LoadString( hInst, idErrorStr, szFmt, 256 ))
         goto ExitError ;

      if (sz = GlobalAllocPtr( GHND, MAX_STRLEN ))
      {
         w = 0 ;

         wvsprintf( sz, szFmt, (LPSTR)(((UINT FAR *)&idErrorStr) + 1) ) ;

         if (!LoadString( hInst, idAppName, szFmt, 256 ))
            goto ExitError ;

         #ifdef USE_MESSAGEBOX
         w = MessageBox( hwnd, sz, szFmt, flags ) ;
         #else
         w = MyMessageBox( hwnd, sz, szFmt, flags ) ;
         #endif

      }
   }

ExitError :
   DASSERT( hWDB, sz ) ;
   DASSERT( hWDB, szFmt ) ;

   if (sz)
      GlobalFreePtr( sz ) ;

   if (szFmt)
      GlobalFreePtr( szFmt ) ;

   return w ;

}/* ErrorResBox() */

#ifndef USE_MESSAGEBOX
/****************************************************************
 *  UINT WINAPI MyMessageBox( HWND hwndParent, LPSTR lpszTitle, LPSTR lpszFmt, UINT uiFlags, HICON hico )
 *
 *  Description: 
 *
 *    Just like MessageBox() but is centered within the parent and
 *    can have a help button or icon of your choice.
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI MyMessageBox( HWND hwndParent, LPSTR lpszTitle, LPSTR lpszFmt, UINT uiFlags, HICON hico )
{


} /* MyMessageBox()  */
#endif

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
   int   dxCenter, dyCenter ;
   int   dxScreen, dyScreen ;

   dxScreen = GetSystemMetrics( SM_CXSCREEN ) ;
   dyScreen = GetSystemMetrics( SM_CYSCREEN ) ;

   if (!IsWindow(hwndCenter))
      return ;

   if (hwndWithin && !IsWindow(hwndWithin))
      return ;

   if (hwndWithin == NULL)
   {
      rcWithin.left = rcWithin.top = 0 ;
      rcWithin.right = dxScreen ;
      rcWithin.bottom = dyScreen ;
   }
   else
   {
      if (fClient)
      {
         /*
          * First get screen coords of rectangle we're going to be
          * centered within.
          */
         GetClientRect( hwndWithin, (LPRECT)&rcWithin ) ;
         ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.left ) ;
         ClientToScreen( hwndWithin, (LPPOINT)&rcWithin.right ) ;
      }
      else
         GetWindowRect( hwndWithin, (LPRECT)&rcWithin ) ;
   }

   GetWindowRect( hwndCenter, (LPRECT)&rcCenter ) ;
   dxCenter = rcCenter.right - rcCenter.left ;
   dyCenter = rcCenter.bottom - rcCenter.top ;

   /*
    * Now we have both the within and center rects in screen coords.
    *
    * SetWindowPos behaves differently for Non child windows
    * than for child windows.  For popups it's coordinates
    * are relative to the upper left corner of the screen.  For
    * children it's coords are relative to the upper left corner
    * of the client area of the parent.
    */
   x = ((rcWithin.right - rcWithin.left) - dxCenter) / 2 ;
   y = ((rcWithin.bottom - rcWithin.top) - dyCenter) / 2 ;

   if (hwndWithin == GetParent( hwndCenter ) &&
       !(GetWindowLong( hwndCenter, GWL_STYLE ) & WS_CHILD ))
   {
      x += rcWithin.left ;
      y += rcWithin.top ;

      if (x + dxCenter > dxScreen )
         x = dxScreen - dxCenter ;

      if (y + dyCenter > dyScreen )
         y = dyScreen - dyCenter ;

      SetWindowPos( hwndCenter, NULL,
                    max(x,0),
                    max(y,0),
                    0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER ) ;

      return ;
   }

   SetWindowPos( hwndCenter, NULL,
                 max(x,0),
                 max(y,0),
                 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER ) ;

   return ;

}/* DlgCenter() */


/************************************************************************
 * End of File: dlghelp.c
 ************************************************************************/
