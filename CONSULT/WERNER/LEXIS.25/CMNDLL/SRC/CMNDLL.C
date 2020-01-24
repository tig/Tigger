/************************************************************************
 *
 *    Copyright (C) 1990 Werner Associates.  All Rights Reserved.
 *
 *-----------------------------------------------------------------------
 *
 *     Project:  CMNDLL, Common DLL for Werner Apps
 *
 *      Module:  cmndll.c
 *
 *   Author(s):  Bryan A. Woodruff (baw)
 *               Charles E. Kindel, Jr. (cek)
 *
 *
 *     Remarks:  
 *
 *   Revisions:  
 *
 *
 ************************************************************************/

#define WINDLL
#include <windows.h>
#include "..\inc\cmndll.h"

//------------------------------------------------------------
// Exported variables
//------------------------------------------------------------
HANDLE   hInst ;
BOOL     bMonochrome ;

BOOL FAR PASCAL
LocalBmpBtnInit( HANDLE hPrev, HANDLE hInst ) ;

BOOL FAR PASCAL
LocalLEDInit( HANDLE hPrev, HANDLE hInst ) ;

HCURSOR hHourGlass ;

char szVersion[32] ;

//   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
//             the DLL is loaded.  The LibEntry routine is provided in
//             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
//             source LIBENTRY.ASM is also provided.)  
//
//             LibEntry initializes the DLL's heap, if a HEAPSIZE value is
//             specified in the DLL's DEF file.  Then LibEntry calls
//             LibMain.  The LibMain function below satisfies that call.
//             
//             The LibMain function should perform additional initialization
//             tasks required by the DLL.  In this example, no initialization
//             tasks are required.  LibMain should return a value of 1 if
//             the initialization is successful.
//           
int FAR PASCAL LibMain( 
                        HANDLE   hModule,
                        WORD     wDataSeg,
                        WORD     cbHeapSize,
                        LPSTR    lpszCmdLine
                      ) 
{
   HDC  hDC ;

   if (cbHeapSize == 0)
      return FALSE ;

   hInst = hModule ;

   if (hDC = GetDC( NULL ))
   {
      if (GetDeviceCaps( hDC, NUMCOLORS ) <= 4)
         bMonochrome = TRUE ;
      else
         bMonochrome = FALSE ;
      ReleaseDC( NULL, hDC ) ;
   }

   D( hWDB = wdbOpenSession( NULL, "CMNDLL", "wdb.ini", WDB_LIBVERSION ) ) ;

//   D( w = wdbGetOutput( hWDB ) ) ;

   D( wdbSetOutput( hWDB, WDB_OUTPUT_AUX, NULL ) ) ;

   D( VerGetCMNDLLVersion( szVersion ) ) ;

   if (!LocalBmpBtnInit( NULL, hModule ))
      return FALSE ;

   if (!StatusInit( NULL, hModule ))
      return FALSE ;

   if (!LocalLEDInit( NULL, hModule ))
      return FALSE ;

   hHourGlass = LoadCursor( NULL, IDC_WAIT ) ;

   //
   // Set internationalization stuff up...
   // This protects us against applications that call the Time functions
   // without calling this first.
   TimeResetInternational( ) ;

//   D( wdbSetOutput( hWDB, w, NULL ) ) ;

   return TRUE ;
}

/*************************************************************************
 *  HANDLE FAR PASCAL GetCMNDLLhInst( void )
 *
 *  Description: 
 *
 *    Returns the hInst of the CMNDLL.
 *
 *  Comments:
 *
 *************************************************************************/
HANDLE FAR PASCAL GetCMNDLLhInst( void )
{
   return hInst ;
}/* GetCMNDLLhInst() */


/*************************************************************************
 *  BOOL FAR PASCAL SetHourGlass( BOOL f )
 *
 *  Description: 
 *
 *    This function turns on the Hourglass cursor.
 *
 *  Comments:
 *
 *************************************************************************/
HCURSOR FAR PASCAL SetHourGlass( void )
{
   return SetCursor( hHourGlass ) ;
}/* SetHourGlass() */

HCURSOR FAR PASCAL ResetHourGlass( HCURSOR hCur )
{
   return SetCursor( hCur ) ;
}


/************************************************************************
 * End of File: cmndll.c
 ************************************************************************/


