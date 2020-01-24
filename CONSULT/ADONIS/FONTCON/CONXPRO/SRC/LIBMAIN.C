//***************************************************************************
//
//  Module: libmain.c
//
//  Purpose:
//     This is the LibMain() and WEP() of the ZMODEM DLL.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              3/10/92   BryanW      Wrote it.
//
//***************************************************************************

#include "precomp.h"

// global declarations in this module
#define GLOBALDEFS

#include "izmodem.h"

/************************************************************************
 *  int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
 *                          WORD wHeapSize, LPSTR lpszCmdLine )
 *
 *  Description:
 *     This is the LibMain() for the ZMODEM library.
 *
 *  Comments:
 *      8/ 2/91  baw  Wrote this comment
 *
 ************************************************************************/

int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                        WORD wHeapSize, LPSTR lpszCmdLine )
{
#ifdef DEBUG
   WORD  wOutput ;

   if (!(hWDB=wdbOpenSession( NULL,
                              "ZMODEM",
                              "WDB.INI",
                              0 )))
   {
      ODS( "wdb OpenSession Failed!\n\r" ) ;
   }
   else
   {
      wOutput = wdbGetOutput( hWDB, NULL ) ;
      wdbSetOutput( hWDB, WDB_OUTPUT_ODS, NULL ) ;
      DPF1( hWDB, "\n***** ZMODEM V%02d.%02d.%03d *****\r\n", VER_MAJOR, VER_MINOR,
            VER_BUILD ) ;
      wdbSetOutput( hWDB, wOutput, NULL ) ;
   }
#endif

   // use count starts at zero

   nUseCount = 0 ;

   // build 32-bit CRC table

   BuildCRC32Table() ;

   // store lib instance

   hInstLib = hInstance ;

   return ( TRUE ) ;

} /* end of LibMain() */

//------------------------------------------------------------------------
//  VOID NEAR BuildCRC32Table( VOID )
//
//  Description:
//     Builds the CRC-32 table.
//
//  Parameters:
//     NONE
//
//  History:   Date       Author      Comment
//              5/10/92   BryanW      Wrote it.
//
//------------------------------------------------------------------------

#define CRC32_POLYNOMIAL 0xEDB88320L

VOID NEAR BuildCRC32Table( VOID )
{
   int           i, j ;
   DWORD         dwCRC32 ;
   extern DWORD  CRC32Table[ 256 ] ;

   for (i = 0; i  < 256; i++)
   {
      dwCRC32 = i ;
      for (j = 8; j > 0; j--)
      {
         if (dwCRC32 & 1L)
            dwCRC32 = (dwCRC32 >> 1) ^ CRC32_POLYNOMIAL ;
         else
            dwCRC32 >>= 1 ;
      }
      CRC32Table[ i ] = dwCRC32 ;
   }

} // end of BuildCRC32Table()

// NOTE: DO NOT COMMENT-OUT THIS pragma...
// this forces the WEP into a seperate segment!

#pragma alloc_text( WEP_TEXT, WEP )

/************************************************************************
 *  int FAR PASCAL WEP( int nSystemExit )
 *
 *  Description:
 *     The WEP()... although it is useless if we GP fault.
 *
 *  Comments:
 *      8/ 2/91  baw  Wrote this comment
 *
 ************************************************************************/

int FAR PASCAL WEP( int nSystemExit )
{
   D( wdbCloseSession( NULL ) ) ;

   return (0) ;

} /* end of WEP() */

//***************************************************************************
//  End of File: libmain.c
//***************************************************************************


