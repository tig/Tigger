// lexmac.c
//
// LibMain for lexmac.DLL
//
// Copyright 1990, Charles E. Kindel, Jr.
//
//

#include <windows.h>
#include "lexmac.h"

//------------------------------------------------------------
// Exported variables
//------------------------------------------------------------
HANDLE   hInst ;


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
   D( char szVersion[64] ) ;

   if (cbHeapSize == 0)
      return FALSE ;

   hInst = hModule ;

   D( VerGetDLLVersion( szVersion ) ) ;

   return 1 ;
}


//    FUNCTION:  WEP(int)
//
//    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
//              called automatically by Windows when the DLL is unloaded (no
//              remaining tasks still have the DLL loaded).  It is strongly
//              recommended that a DLL have a WEP() function, even if it does
//              nothing but returns success (1), as in this example.
//
int FAR PASCAL WEP( int bSystemExit )
{
    return 1 ;
}

