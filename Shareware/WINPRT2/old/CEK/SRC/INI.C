/************************************************************************
 *
 *      Module:  ini.c
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "ICEK.h"

#include <ctype.h>
#include <io.h>

//*************************************************************
//
//  ParseOffNumber
//
//  Purpose:
//      Parses the string to the next , and converts the text into     
//      a number.  It then moves the pointer pointed at by lplp to
//      point to the next useful character in the string after the ,.
//
//  Parameters:
//      LPSTR FAR *lplp
//      LPWORD  lpConv  - can be NULL
//
//  Return: (LONG WINAPI)
//      returns the number and sets lpConv to the suceess.
//      If FALSE is set the return contains as much of the
//      number that was converted before failure and *lplp points
//      to the character that caused the function to fail.
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************


LONG WINAPI ParseOffNumber( LPSTR FAR *lplp, LPINT lpConv )
{
    LPSTR lp = *lplp;
    LONG  lValue=0;
    int   sign=1;

    DASSERT( hWDB, lp ) ;

    if (lp == NULL)
        return 0L ;

    if (*lp == '\0')
        return 0L ;

    while( isspace(*lp) )
        lp++;
    if( *lp=='-' )
    {               
        sign = -1;
        lp++;
    }
    if (*lp=='\0')
    {
        *lpConv = FALSE;
        return 0L;
    }

    // Is it a decimal number
    if(!(*lp == '0' && toupper(*(lp+1)) == 'X'))
    {
        while( isdigit(*lp) )
        {
            lValue *= 10;
            lValue += (*lp - '0');
            lp++;
        }
    }    
    else
    {
        lp+=2;

        while( isxdigit(*lp) )
        {
            lValue *= 16;
            if( isdigit(*lp) )
                lValue += (*lp - '0');
            else
                lValue += (toupper(*lp) - 'A' + 10);
            lp++;
        }
    }
    while( isspace(*lp) )
        lp++;

    lValue *= (long)sign;

    if (*lp==',')
    {
        lp++;
        while( isspace(*lp) )
            lp++;
        *lplp = lp;
        if (lpConv)
            *lpConv = (int)TRUE;
    }
    else
    {
        *lplp = lp;
        if (lpConv)
            *lpConv = (int)(*lp=='\0');
    }
    return lValue;

} //*** ParseOffNumber


//*************************************************************
//
//  ParseOffString
//
//  Purpose:
//      Parses of the next string up to a , and copies it into the
//      buffer lpBuff.  It then moves the pointer pointed at by lplp to
//      point to the next useful character in the string after the ,.
//
//  Parameters:
//      LPSTR FAR *lplp
//      LPSTR lpBuff
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************


BOOL WINAPI ParseOffString( LPSTR FAR *lplp, LPSTR lpBuff )
{
    LPSTR lpD = lpBuff, lp = *lplp;

    DASSERT( hWDB, lp ) ;
    if (lp == NULL)
        return 0L ;

    if (*lp == '\0')
        return 0L ;

    while( isspace(*lp) )
        lp++;

    // HACK!  Bug Fix.  See comment below
    if (*lp == ',')
    {
        lp++;
        while ( isspace(*lp) )
            lp++;
        *lplp = lp;

        *lpD = '\0';
        return TRUE ;
    }

    if (*lp=='\0')
    {
        *lpD = 0;
        return FALSE;
    }

    while (*lp != ',' && *lp != '\0')
        *lpD++ = *lp++;

    // Strip trailing spaces
    // BUG! BUG!  There be a bug here:  If *lplp pointed to a ',' then
    // lpD here is the beginning of the string.  Doing a lpD-1 would cause
    // us to exceed segment bounds!  The fix is above where it reads HACK!
    while ( isspace(*(lpD-1)) )
        lpD--;

    if (*lp==',')
    {
        lp++;
        while ( isspace(*lp) )
            lp++;
        *lplp = lp;
    }
    else
        *lplp = lp;

    *lpD = 0;

    return TRUE;

} //*** ParseOffString


//*************************************************************
//
//  StripOffWhiteSpace
//
//  Purpose:
//      Strips off all leading and trailing white space
//
//
//  Parameters:
//      LPSTR lpString
//      
//
//  Return: (WORD WINAPI)
//      Length of the string when done
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

UINT WINAPI StripOffWhiteSpace( LPSTR lpString )
{
    LPSTR lpS, lpD, lpLWS;

    lpS = lpD = lpString;

    while ( isspace(*lpS) )
        lpS++;

    lpLWS = NULL;

    while (*lpS!='\0')
    {
        if (isspace( *lpS ))
            lpLWS = lpS;
        else
            lpLWS = NULL;

        *lpD++ = *lpS++;        
    }

    if (!lpLWS)
        lpLWS = lpD;

    *lpD = '\0';


    return (UINT)(DWORD)(lpD - lpString);

} //*** StripOffWhiteSpace


//*************************************************************
//
//  WritePrivateProfileFont
//
//  Purpose:
//      Writes the font definition to the INI file
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      HFONT hFont
//      LPSTR lpIniFile
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

BOOL WINAPI WritePrivateProfileFont(LPSTR lpApp,LPSTR lpKey,HFONT hFont,LPSTR lpIniFile)
{
    UINT    uPointSize;
    char    szBuffer[ 255 ];
    LPSTR   lp = (LPSTR)szBuffer;
    HDC     hDC = GetDC( NULL );
    
    hFont = SelectObject( hDC, hFont );
    uPointSize = GetTextPointSize( hDC );
    lp += wsprintf( lp, "%d,", uPointSize );

    lp += GetTextFace( hDC, 128, lp );
    *lp++ = ',';
    lp += GetTextStyle( hDC, 128, lp );

    hFont = SelectObject( hDC, hFont );

    ReleaseDC( NULL, hDC );

    return WritePrivateProfileString( lpApp, lpKey, szBuffer, lpIniFile );

} //*** WritePrivateProfileFont


//*************************************************************
//
//  WritePrivateProfileWndPos
//
//  Purpose:
//      Writes the window placement information to the ini file
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      HWND hWnd
//      LPSTR lpIniFile
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

BOOL WINAPI WritePrivateProfileWndPos(LPSTR lpApp,LPSTR lpKey,HWND hWnd,LPSTR lpIniFile)
{
    WINDOWPLACEMENT wp;
    char    szBuffer[ 255 ];

    wp.length = sizeof( WINDOWPLACEMENT );
    GetWindowPlacement( hWnd, &wp );

    wsprintf( szBuffer, "%d, %d, %d, %d, %d, %d, %d, %d, %d",
        wp.showCmd, wp.ptMinPosition.x, wp.ptMinPosition.y,
        wp.ptMaxPosition.x, wp.ptMaxPosition.y,
        wp.rcNormalPosition.left, wp.rcNormalPosition.top,
        wp.rcNormalPosition.right, wp.rcNormalPosition.bottom );

    return WritePrivateProfileString( lpApp, lpKey, szBuffer, lpIniFile );

} //*** WritePrivateProfileWndPos


//*************************************************************
//
//  GetPrivateProfileFont
//
//  Purpose:
//      Reads the font profile from the ini and creates that font
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      LPSTR lpIniFile
//      
//
//  Return: (HFONT WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

HFONT WINAPI GetPrivateProfileFont(LPSTR lpApp, LPSTR lpKey, LPSTR lpIniFile)
{
    char    szBuffer[ 255 ];
    LPSTR   lpStyle, lpP, lp = (LPSTR)szBuffer;
    UINT    uPointSize;
    int     nConv;

    if (!GetPrivateProfileString(lpApp,lpKey,"",lp,255,lpIniFile))
        return NULL;

    DP5( hWDB, "Reading Font: %s", lp );

    lpP = lp;

    if (uPointSize = (UINT)ParseOffNumber( (LPSTR FAR *)&lpP, &nConv ))

    if (!nConv)
        return NULL;

    // The following works a little magic to parse the buffer back into
    // itself.  Not pretty to look at, but saves lots of stack space.


    if (!ParseOffString( (LPSTR FAR *)&lpP, lp ))
        return NULL;
    // lp Now points to the Full Font Name

    lpStyle = lpP;

    if (!ParseOffString( (LPSTR FAR *)&lpP, lpStyle ))
        return NULL;

    DP5( hWDB, "Creating Font: %s, %s, %d", lp, lpStyle, uPointSize );
    return ReallyCreateFontEx( NULL, lp, lpStyle, (short)uPointSize, RCF_NODEFAULT );

} //*** GetPrivateProfileFont


//*************************************************************
//
//  GetPrivateProfileWndPos
//
//  Purpose:
//      Reads in a window placement that was saved to the ini file
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      LPSTR lpIniFile
//      
//
//  Return: (LPWINDOWPLACEMENT WINAPI)
//      Pointer to a static WINDOWPLACEMENT structure
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

LPWINDOWPLACEMENT WINAPI GetPrivateProfileWndPos(LPSTR lpApp,LPSTR lpKey,LPSTR lpIniFile)
{
    static  WINDOWPLACEMENT wpPlacement;
    char    szBuffer[ 255 ];
    LPSTR   lp = (LPSTR)szBuffer;
    int     nConv;

    wpPlacement.length = sizeof( WINDOWPLACEMENT );

    if (!GetPrivateProfileString(lpApp,lpKey,"",lp,255,lpIniFile))
        return NULL;


    wpPlacement.showCmd = (UINT)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;


    wpPlacement.ptMinPosition.x = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;
    wpPlacement.ptMinPosition.y = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    wpPlacement.ptMaxPosition.x = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    wpPlacement.ptMaxPosition.y = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    wpPlacement.rcNormalPosition.left = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    wpPlacement.rcNormalPosition.top = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    wpPlacement.rcNormalPosition.right = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    wpPlacement.rcNormalPosition.bottom = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
    if (!nConv)
        return NULL;

    return (&wpPlacement);

} //*** GetPrivateProfileWndPos


//*************************************************************
//
//  WritePrivateProfileWORD
//
//  Purpose:
//      Writes a word to the ini file
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      WORD w
//      LPSTR lpIniFile
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//    Key=0x1234
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

BOOL WINAPI WritePrivateProfileWORD(LPSTR lpApp,LPSTR lpKey,WORD w,LPSTR lpIniFile)
{
    char szTemp[32];

    wsprintf( szTemp, "%#04X", w );

    return WritePrivateProfileString( lpApp, lpKey, szTemp, lpIniFile );

} //*** WritePrivateProfileWORD


//*************************************************************
//
//  WritePrivateProfileDWORD
//
//  Purpose:
//      writes the dword to the ini file
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      DWORD dw
//      LPSTR lpIniFile
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//    Key=0x12345678
//
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//
//*************************************************************

BOOL WINAPI WritePrivateProfileDWORD(LPSTR lpApp,LPSTR lpKey,DWORD dw,LPSTR lpIniFile)
{
    char szTemp[32];

    wsprintf( szTemp, "%#08lX", dw );

    return WritePrivateProfileString( lpApp, lpKey, szTemp, lpIniFile );

} //*** WritePrivateProfileDWORD


//*************************************************************
//
//  GetPrivateProfileWORD
//
//  Purpose:
//      Retrieves a word from the inifile
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      WORD wDef
//      LPSTR lpIniFile
//      
//
//  Return: (WORD WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

WORD WINAPI GetPrivateProfileWORD(LPSTR lpApp,LPSTR lpKey,WORD wDef,LPSTR lpIniFile)
{
    char szTemp[32];
    WORD w;

    if (!GetPrivateProfileString(lpApp, lpKey, "", szTemp, 32, lpIniFile ))
        return wDef;

    if (!wsscanf( szTemp, "%04X", (LPWORD)&w ))
        return wDef;
    return w;

} //*** GetPrivateProfileWORD


//*************************************************************
//
//  GetPrivateProfileDWORD
//
//  Purpose:
//      Retrieves a DWORD from the ini file
//
//
//  Parameters:
//      LPSTR lpApp
//      LPSTR lpKey
//      DWORD dwDef
//      LPSTR lpIniFile
//      
//
//  Return: (DWORD WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

DWORD WINAPI GetPrivateProfileDWORD(LPSTR lpApp,LPSTR lpKey,DWORD dwDef,LPSTR lpIniFile)
{
    char  szTemp[32];
    DWORD dw;

    if (!GetPrivateProfileString(lpApp, lpKey, "", szTemp, 32, lpIniFile ))
        return dwDef;

    if (!wsscanf( szTemp, "%08lX", (LPWORD)&dw ))
        return dwDef;
    return dw;

} //*** GetPrivateProfileDWORD

LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF ) ;
/****************************************************************
 *  UINT WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, UINT cbMax, LPSTR lpszFile )
 *
 *  Description: 
 *
 *    This function enumerates all of the profile sections in
 *    the given profile.
 *
 *  Comments:
 *
 ****************************************************************/
UINT WINAPI GetPrivateProfileSections( LPSTR lpOutBuf, UINT cbMax, LPSTR lpszFile ) 
{
   HFILE          hFile ;
   OFSTRUCT       OF ;

   UINT           wBytes ;       /* number of bytes read from file */
   LPSTR          lpFileBuf ;
   LPSTR          lpCur ;        /* Pointers into the file buffer */
   LPSTR          lpEOL ;
   LPSTR          lpEOF ;
   LPSTR          lpLine ;
   BOOL           fInSec ;

   LPSTR          lpOutBufCur ;  /* pointers into the output buffer */
   UINT           cbCurSec ;
   LPSTR          lpTemp ;
   UINT           wFileSize ;

   if ((HFILE)-1 == (hFile = OpenFile( lpszFile, &OF, OF_READ | OF_SHARE_DENY_NONE )))
   {
      DP1( hWDB, "Could not open ini file %s", (LPSTR)lpszFile ) ;
      return 0 ;
   }

   /*
    * Read the entire file into memory.
    */

   wFileSize = (UINT)_filelength( hFile ) ;

   if (!(lpFileBuf = GlobalAllocPtr( GHND, wFileSize + 1 )))
   {
      DP1( hWDB, "Could not allocate global mem for sections (%d bytes!)",
            wFileSize + 1 ) ;
      _lclose( hFile ) ;
      return 0 ;
   }
   
   if (-1 != (wBytes = _lread( hFile, lpFileBuf, wFileSize )))
   {
      DP3( hWDB, "Read %d bytes into buffer (%d bytes in size)", wBytes,wFileSize+1 ) ;

      //
      // Empty file check
      //
      if (wBytes < 3)
      {
         _lclose( hFile ) ;
         GlobalFreePtr( lpFileBuf ) ;
         return 0 ;
      }

      /*
       * Go through file with lpLine always pointing to the
       * beginning of a line and lpEOL always pointing to the
       * last character of a line.
       *
       * lpCur is our current pointer within the line.
       */
      lpLine = lpFileBuf ;
      lpEOF = lpFileBuf + wBytes ;


      lpTemp = lpOutBufCur = lpOutBuf ;
      lpOutBuf[0] = '\0' ;
      lpOutBuf[1] = '\0' ;

      while (lpEOL = FindEndOfLine( lpLine, lpEOF ))
      {

         /*
          * Scan the current line.  If we find a ';' the line is
          * a comment.  If we find a '[' then we are beginning
          * a section.
          */
         for (lpCur = lpLine, fInSec = FALSE, cbCurSec = 0 ;
              lpCur < lpEOL ;
              lpCur++)
         {
            if (fInSec)
            {
               /*
                * We are in a section
                */
               if (*lpCur == ']')
                  break ;


               *lpOutBufCur++ = *lpCur ;
               cbCurSec++ ;
            }
            else
            {
               if (*lpCur == '[')
                  fInSec = TRUE ;

               if (*lpCur == ';')
                  /*
                   * This line is a comment.  Break out of the for loop,
                   * and go on to the next line.
                   */
                  break ;

               if (*lpCur == '=')
                  /*
                   * This line is key value line.
                   */
                  break ;

            }
         }

         if (cbCurSec)
            *lpOutBufCur++ = '\0' ;

         lpTemp = lpOutBufCur ;

         lpLine = lpEOL ;

      }

      /*
       * Put the second NULL on
       */
      *lpOutBufCur++ = '\0' ;
   }

   _lclose( hFile ) ;

   GlobalFreePtr( lpFileBuf ) ;

   return TRUE ;

} /* GetPrivateProfileSections()  */

/****************************************************************
 *  LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF )
 *
 *  Description: 
 *
 *    This function scans the line lpLine returning a pointer to
 *    the last character on the line.
 *
 *    It returns NULL when lpLine == lpEOF.
 *
 *  Comments:
 *
 *
 ****************************************************************/
LPSTR NEAR PASCAL FindEndOfLine( LPSTR lpLine, LPSTR lpEOF )
{
   register char  c ;

   while (lpLine <= lpEOF)
   {
      c = *lpLine ;

      if (c == '\n' || c == '\r' || c == '\f' || c == '\x1A')
      {
         /*
          * Scan until you get to a non '\n', '\r', '\f', or '\x1A'
          * character and return that pointer.
          */
         while( lpLine <= lpEOF )
         {
            if (c == '\n' || c == '\r' || c == '\f' || c == '\x1A')
               lpLine++ ;
            else
               return lpLine ;

            c = *lpLine ;
             
         }

       }
      lpLine++ ;
   }

   if (lpLine >= lpEOF)
      return NULL ;

   return lpLine ;

} /* FindEndOfLine()  */

//*************************************************************
//
//  GetRCString
//
//  Purpose:
//      Retrieves a string from the resource STRINGTABLE
//      This routine will read (minimum) up to 384 characters before it
//      begins to overwrite.  MAX length per/read is 128!!
//
//  Parameters:
//      UINT wID
//      
//
//  Return: (LPSTR)
//      Pointer to a static buffer that contains the string
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/12/92   MSM        Created
//
//*************************************************************

#define BUF_SIZE    2048

LPSTR WINAPI GetRCString(UINT wID, HINSTANCE hInst)
{
    static char szString[ BUF_SIZE ];
    static LPSTR lpIndex = (LPSTR)szString;
    static WORD  wEmpty = BUF_SIZE;
    LPSTR lp;
    WORD  wLen;

read_string:
    // Add 1 for the NULL terminator
    wLen = LoadString( hInst, wID, lpIndex, wEmpty ) + 1;

    if (wLen == wEmpty)
    {
        wEmpty = BUF_SIZE;
        lpIndex = (LPSTR)szString;
        goto read_string;
    }

    if (wLen==0)
    {
        DP1( hWDB, "LoadString failed!  ID = %d", wID );
        lpIndex[0]=0;
    }
    lp = lpIndex;

    lpIndex += wLen;
    wEmpty -= wLen;

    return lp;

} //*** GetRCString


//*************************************************************
//
//  ExpandStringToPrintable
//
//  Purpose:
//      Expands a string to printable characters
//
//
//  Parameters:
//      LPSTR lpString
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//  History:    Date       Author     Comment
//               3/14/92   MSM        Created
//
//*************************************************************

VOID WINAPI ExpandStringToPrintable( LPSTR lpString )
{
    LPSTR szTemp ;
    LPSTR lpS, lpD = lpString;
    UINT  nNonPrintable = 0 ;

    for ( lpS = lpString ; *lpS ; lpS++)
    {
        if (!isprint(*lpS) || *lpS=='\\' || isspace(*lpS))
            nNonPrintable++ ;
    }

    if (!(szTemp = GlobalAllocPtr( GHND, lstrlen( lpString ) + 1 + (nNonPrintable*5) )))
        return ;

    lstrcpy( szTemp, lpString );
    lpS = szTemp;

    //*** Add quotes
    *lpD++ = '\"';

    while (*lpS)
    {
        if (!isprint(*lpS) || *lpS=='\\' || isspace(*lpS))
        {
            lpD += wsprintf( lpD, "\\x%02X", (UINT)*((LPBYTE)lpS) );
            lpS++;
        }
        else
            *lpD++ = *lpS++;
    }
    *lpD++ = '\"';
    *lpD = '\0';

} //*** ExpandStringToPrintable


//*************************************************************
//
//  ShrinkStringFromPrintable
//
//  Purpose:
//      Shrinks a string from an expanded printable
//
//
//  Parameters:
//      LPSTR lpString
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/14/92   MSM        Created
//
//*************************************************************

VOID WINAPI ShrinkStringFromPrintable( LPSTR lpString )
{
    LPSTR lpS, lpD;

    lpS = lpD = lpString;

    while (*lpS)
    {
        if (*lpS=='\\')
        {
            int i;

            lpS+=2; // skip \x
            *lpD = '\0';
            
            for (i=1; i<=2; i++)
            {
                *lpD *= 16;
                if (isdigit(*lpS))
                    *lpD += (*lpS++ - '0');
                else
                    *lpD += (*lpS++ - 'A') + 10;
            }
            lpD++;
        }
        else
            *lpD++ = *lpS++;
    }
    *lpD = '\0';

} //*** ShrinkStringFromPrintable

/************************************************************************
 * End of File: ini.c
 ***********************************************************************/

