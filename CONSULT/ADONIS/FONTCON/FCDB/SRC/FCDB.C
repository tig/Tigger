//*************************************************************
//  File name: FCDB.c
//
//  Description: 
//      LibMain and the WEP for the Slider DLL
//
//  History:    Date       Author     Comment
//               2/25/92   MSM        Updated for FontShopper
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"
#include "..\..\inc\version.h"

HINSTANCE ghInstLib = NULL;
BOOL      bMonochrome ;
BOOL      bInit = FALSE;
//HBITMAP   ghbmpFolders = NULL;

BOOL NEAR RegisterClasses( HANDLE hInstance );
BOOL NEAR UnregisterClasses( HANDLE hInstance );


//*************************************************************
//
//  LibMain
//
//  Purpose:
//      This is the entry point for the FontShopper DB DLL
//
//
//  Parameters:
//      HANDLE hInstance
//      WORD wDataSegment
//      WORD wHeapSize
//      LPSTR lpszCmdLine
//      
//
//  Return: (int WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/27/92   MSM        Created
//
//*************************************************************

int WINAPI LibMain( HANDLE hInstance, WORD wDataSegment, WORD wHeapSize, LPSTR lpszCmdLine )
{
    #ifdef DEBUG
    {
        char szVersion[64] ;

        // Install debuging stuff
        D( hWDB = wdbOpenSession( NULL, "FCDB", "wdb.ini", WDB_LIBVERSION ) ) ;

        if ( VER_BUILD )
            wsprintf( szVersion, (LPSTR)"%d.%.2d.%.3d",
                        VER_MAJOR, VER_MINOR, VER_BUILD ) ;
        else
            wsprintf( szVersion, (LPSTR)"%d.%.2d", VER_MAJOR, VER_MINOR ) ;
    }
    #endif

    if (wHeapSize == 0)
       return FALSE ;

    // store lib instance
    ghInstLib = hInstance ;

    return TRUE;

} //*** LibMain

//************************************************************************
//  WEP
//
//  Description:
//    Exit point.  We assume that the DLL is shutting down.
//
//  Parameters:
//     int nSystemExit
//        really not used in this case
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

int WINAPI WEP( int nSystemExit )
{
    if (ghInstLib)
        UnregisterClasses( ghInstLib ) ;

//    if (ghbmpFolders)
//        DeleteObject( ghbmpFolders );

//    D( wdbCloseSession( hWDB ) );

   return (0) ;

} //*** WEP


//*************************************************************
//
//  InitializeFCDB
//
//  Purpose:
//      Does the necessary DB initialization
//
//
//  Parameters:
//      
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/27/92   MSM        Created
//
//*************************************************************

BOOL WINAPI InitializeFCDB()
{

    if (bInit)
        return TRUE;
    bInit = TRUE;

   #if 0
    ghbmpFolders = LoadLBBitmap( ghInstLib, MAKEINTRESOURCE(ELB_FOLDERS) );

    if (!ghbmpFolders)
        return FALSE;
   #endif


    return RegisterClasses( ghInstLib );

} //*** InitializeFCDB



//************************************************************************
//  RegisterClasses
//
//  Description:
//     Registers the custom controls in this DLL.
//
//  Parameters:
//     HANDLE hInstance
//        instance of this DLL
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

BOOL NEAR RegisterClasses( HANDLE hInstance )
{
   WNDCLASS  wc ;

//*** Register the Extended Listbox class
    wc.style = CS_GLOBALCLASS | CS_BYTEALIGNWINDOW | CS_BYTEALIGNCLIENT ;
    wc.lpfnWndProc = ELBWndProc;
                                
    wc.cbClsExtra = 0;           
    wc.cbWndExtra = ELBEXTRA;           
    wc.hInstance  = hInstance;    
    wc.hIcon      = NULL;
    wc.hCursor    = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1) ;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZ_ELBCLASS;

    if (!RegisterClass(&wc))
        return FALSE;

   return TRUE ;
} //*** RegisterClasses

//************************************************************************
//  UnregisterClasses
//
//  Description:
//     Unregisters the custom controls for this library.
//
//  Parameters:
//     HANDLE hInstance
//        DLL instance
//
//  History:   Date       Author      Comment
//               2/25/92   MSM        Updated for FontShopper
//
//************************************************************************

BOOL NEAR UnregisterClasses( HANDLE hInstance )
{
    return ( UnregisterClass( SZ_ELBCLASS, hInstance )) ;

} //*** UnregisterClasses

//*** EOF: FCDB.c
