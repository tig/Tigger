/************************************************************************
 *
 *     Project:  
 *
 *      Module:  crc.c
 *
 *     Remarks:
 *       This module implements CRC32 CRCs.
 *
 *   Revisions:  
 *
 *
 ***********************************************************************/


#define UPDCRC32(cp, crc) (CRC32Table[((int) crc ^ cp) & 255] ^ ((crc >> 8) & 0x00FFFFFFL))
#define CRC32_POLYNOMIAL 0xEDB88320L

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


VOID NEAR BuildCRC32Table( DWORD FAR CRC32Table[] )
{
   int           i, j ;
   DWORD         dwCRC32 ;

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


/****************************************************************
 *  DWORD WINAPI CalcCRC32( LPBYTE lpBuf, UINT n)
 *
 *  Description: 
 *
 *    Returns a Crc32 for the buffer passed in.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD WINAPI CalcCRC32( LPBYTE lpBuf, UINT n )
{
   static DWORD rgCRC32[256] ;
   static BOOL  fTableBuilt = FALSE ;
   UINT   i ;
   DWORD  dwCRC32 = 0xFFFFFFFFL ;


   if (fTableBuilt == FALSE)
   {
      BuildCRC32Table( rgCRC32 ) ;
      fTableBuilt = TRUE ;
   }

   for (i = 0 ; i < cb ; i++)
   {
      dwCRC32 = rgCRC32[((int)dwCRC32 ^ lpBuf[i]) & 255] ^ ((dwCRC32 >> 8) & 0x00FFFFFFL) ;
   }
   dwCRC32 = ~dwCRC32 ;

   return dwCRC32 ;

} /* CalcCRC32()  */

/************************************************************************
 * End of File: crc.c
 ***********************************************************************/

