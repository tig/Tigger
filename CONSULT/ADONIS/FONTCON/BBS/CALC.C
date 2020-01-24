/****************************************************************
 *  DWORD CalcFSDataBaseCRC() ;
 *
 *  Description: 
 *
 *    Calculates a 32 bit CRC for all of the fontshopper database
 *    files.  It uses a sorted list of all the filenames.
 *
 *  Comments:
 *
 ****************************************************************/
DWORD CalcFSDataBaseCRC( void ) 
{
   DWORD          dwCRC ;
   int            nLen = 0 ;
   DIR            *pDir ;
   struct dirent  *pDirEnt ;
   struct stat    Stat ;
   char *         pBuf = NULL ;
   int            n ;

   dwCRC = 0 ;

   if (!(pDir = opendir( FONTSHOPDBDIR )))
   {
      printf( "opendir() failed in CalcFSDataBaseCRC!!!\n" ) ;
      return FALSE ;
   }

   while (pDirEnt = readdir( pDir ))
   {
      stat( pDirEnt->d_name, &Stat ) ;

      /* The st_mode member of the Stat structure tells us what kind
       * of file this is.  We want to ignore everything but
       * "normal" files.
       */
      if (Stat.st_mode & 0x8000)
      {
         printf( "Adding '%s' to CRC list\n" ) ;

         n = strlen( pDirEnt->d_name + 1) ;

         if (pBuf == NULL)
            pBuf = malloc( n + 1 ) ;
         else
            pBuf = realloc( pBuf, nLen + n ) ;

         strcpy( pBuf + nLen, pDirEnt->d_name ) ;

         nLen += n ;
      }
   }

   pBuf + nLen = '\0' ;

   /* Calc the 32 bit CRC for the buffer.
    */
   dwCRC = CalcCRC32( pBuf, nLen ) ;

   free( pBuf ) ;

   return dwCRC ;

} /* CalcFSDataBaseCRC()  */
