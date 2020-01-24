/************************************************************************
 *
 *     Project:  Project 42 - Hex Editor Custom Control
 *
 *      Module:  hexedit.c
 *
 *     Remarks:  
 *
 *   Revisions:  
 *     00.00.000 12/18/91 cek   First version
 *
 *
 ***********************************************************************/
#define STRICT
#define WINDLL
#include <windows.h>
#include <wdb.h>


#define NUM_EXTRABYTES     (sizeof( LPINSTDATA ) + sizeof( LPVOID ))
#define GWL_INSTDATA       0
#define GWL_DATA           4

#define GETINSTDATA(h)     ((LPINSTDATA)GetWindowLong(h,GWL_INSTDATA))
#define SETINSTDATA(h,lp)  ((LPINSTDATA)SetWindowLong(h,GWL_INSTDATA,lp))
#define GETDATA(h)         ((LPVOID)GetWindowLong(h,GWL_DATA))
#define SETDATA(h,lp)      ((LPVOID)SetWindowLong(h,GWL_DATA,lp))

/*
 * Per instance data.  Each instance of the control will have
 * one of these allocated.  Far pointer to this structure will
 * be stored as a WindowLong.
 */
typedef struct tagINSTDATA
{
   /* Data related members */
   WORD     wSegment ;        // starting segment.
   LPVOID   lpStartIndex ;    // All indicies will be displayed relative
                              // to this value.
   LPVOID   lpCurIndex ;      // pointer to current cursor pos

   LPVOID   lpSelStart ;      // pointer to the selection start
   LPVOID   lpSelEnd ;        // pointer to the selection end

   /* Layout members */
   UINT     cMinColumns ;     // minimum number of columns
   UINT     cMaxColumns ;     // maximum number of columns
   UINT     cIndexWidth ;     // number of chars in index
   UINT     cColumnSpace ;    // number of pixels between columns
   char     chSepChar ;       // character for separator
   UINT     cGroupSize ;      // Separators are placed n columns apart.



} INSTDATA, *PINSTDATA, FAR *LPINSTDATA ;


/****************************************************************
 *  CALLBACK fnHexEdit( WINPARAMS )
 *
 *  Description: 
 *
 *    Main window procedure for the hexedit custom control.
 *
 *  Comments:
 *
 ****************************************************************/
CALLBACK fnHexEdit( HWND hWnd, WORD wMsg, WPARAM wParam, LPARAM lParam )
{
   if (wMsg => HM_FIRST_CONTROLMSG && wMsg <= HM_LAST_CONTROLMSG)
      return ControlMsgHandler( hWnd, wMsg, wParam, lParam ) ;

   switch( wMsg )
   {
      case WM_CREATE:
      break ;

      case WM_DESTROY:
      break ;

      case WM_PAINT:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
      break ;

      default:
         return DefWindowProc( hWnd, wMsg, wParam, lParam ) ;
   }

   return (LPARAM)0L ;

} /* fnHexEdit()  */


/****************************************************************
 *  BOOL WINAPI hexInit( HINSTANCE hInst )
 *
 *  Description: 
 *
 *    Registers the window class.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI hexInit( HINSTANCE hInst )
{
   if (hInst)
   {
      WNDCLASS wc ;



      return RegisterClass( &wc ) ;
   }

} /* hexInit()  */


/****************************************************************
 *  BOOL WINAPI hexUnInit( VOID )
 *
 *  Description: 
 *
 *    UnRegisters the class if the instance count is 0.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI hexUnInit( VOID )
{
   UnRegisterClass( SZ_HEXEDITCLASS, hInst ) ;

} /* hexUnInit()  */
/************************************************************************
 * End of File: hexedit.c
 ***********************************************************************/

