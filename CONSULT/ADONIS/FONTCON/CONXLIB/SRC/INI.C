//*************************************************************
//  File name: ini.c
//
//  Description: 
//    INI routines.  Called by SHOPSRV and FONTSHOPPER.
//
//  History:    Date       Author     Comment
//               2/29/92   MSM        Created
//               4/6/92    cek        Added structure stuff.
//
//*************************************************************

#include "PRECOMP.H"
#include "CONXLIBi.h"

BOOL NEAR PASCAL GetAddress( LPADDRESS lpAddr, LPSTR lpszINIFile, LPSTR lpszSection ) ;


/****************************************************************
 *  BOOL WINAPI GetCustInfo( LPCUSTINFO lpCI, LPSTR lpszAppName )
 *
 *  Description: 
 *
 *    Updates the customer information from the INI file.
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI GetCustInfo( LPCUSTINFO lpCI, LPSTR lpszAppName )
{

   UINT i ;

   /* We don't _fmemset the entire structure 'cause some of it
    * needs to be presevered...fValidPassword in particular
    */
   lpCI->uiShipMethod = 0 ;
   lpCI->uiWhichCard = 0 ;
   lpCI->uiWhichAddress = 0 ;
   lpCI->uiMediaType = DISK14 ;
   _fmemset( &lpCI->CreditCard, '\0', sizeof( CREDITCARD ) ) ;
   _fmemset( &lpCI->CustAddress, '\0', sizeof( ADDRESS ) ) ;
   _fmemset( &lpCI->ShipAddress, '\0', sizeof( ADDRESS )) ;

   GetPrivateProfileString( lpszAppName, rglpsz[IDS_INI_USERID], "",
                            lpCI->szUserID, MAX_PASSWORD+1, rglpsz[IDS_INIFILE] ) ;


   GetPrivateProfileString( lpszAppName, rglpsz[IDS_INI_PASSWORD], "",
                            lpCI->szPassword, MAX_PASSWORD+1, rglpsz[IDS_INIFILE] ) ;

   for (i = 0 ; i < MAX_PASSWORD ; i++)
   {
      if (lpCI->szPassword[i])
         lpCI->szPassword[i] -= 128 ;
   }

   if (!GetAddress( &lpCI->CustAddress, rglpsz[IDS_INIFILE],
                    rglpsz[IDS_INI_CUSTADDRESS] ))
      return FALSE ;

   lpCI->uiWhichAddress = GetPrivateProfileInt( rglpsz[IDS_INI_SHIPINFO],
                                                rglpsz[IDS_INI_WHICHADDRESS],
                                                ADDRESS_CUSTOMER,
                                                rglpsz[IDS_INIFILE] ) ;

   if (lpCI->uiWhichAddress == ADDRESS_SHIP)
      if (!GetAddress( &lpCI->ShipAddress, rglpsz[IDS_INIFILE],
                       rglpsz[IDS_INI_SHIPINFO] ))
         return FALSE ;

   lpCI->uiShipMethod = GetPrivateProfileInt( rglpsz[IDS_INI_SHIPINFO],
                                                rglpsz[IDS_INI_SHIPMETHOD],
                                                SHIPMETHOD_1TO2DAY,
                                                rglpsz[IDS_INIFILE] ) ;

   lpCI->uiMediaType =  GetPrivateProfileInt( rglpsz[IDS_INI_SHIPINFO],
                                                rglpsz[IDS_INI_MEDIA],
                                                DISK14,
                                                rglpsz[IDS_INIFILE] ) ;

   lpCI->uiWhichCard = GetPrivateProfileInt( rglpsz[IDS_INI_BILLINGINFO],
                                                rglpsz[IDS_INI_WHICHCARD],
                                                MASTERCARD,
                                                rglpsz[IDS_INIFILE] ) ;

   if (!GetCreditCard( &lpCI->CreditCard, rglpsz[IDS_INIFILE], lpCI->uiWhichCard ))
      return FALSE ;

   return TRUE ;

} /* GetCustInfo()  */

/****************************************************************
 *  BOOL WINAPI GetAddress( LPADDRESS lpAddr, LPSTR lpszINIFile, LPSTR lpszSection )
 *
 *  Description: 
 *
 *       Reads the address data from the specified section of the
 *       INI file.
 *  Comments:
 *
 ****************************************************************/
BOOL NEAR PASCAL GetAddress( LPADDRESS lpAddr, LPSTR lpszINIFile, LPSTR lpszSection )
{
   if (!GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_NAME], "",
                              lpAddr->szName, MAX_NAME+1, lpszINIFile ))
      return FALSE ;

   GetPrivateProfileString( lpszSection, "Company", "", lpAddr->szCompany, MAX_NAME+1, lpszINIFile ) ;

   lstrcpy( lpAddr->szTitle, "" ) ;

   if (!GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_STREET1], "", lpAddr->szStreet1,
                                 MAX_STREET+1, lpszINIFile ))
      return FALSE ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_STREET2], "", lpAddr->szStreet2, MAX_STREET+1, lpszINIFile ) ;

   if (!GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_CITY], "", lpAddr->szCity, MAX_CITY+1, lpszINIFile ))
      return FALSE ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_STATE], "", lpAddr->szState, MAX_STATE+1, lpszINIFile ) ;

   lstrcpy( lpAddr->szProvince, "" ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_ZIP], "", lpAddr->szPostalCode, MAX_POSTAL+1, lpszINIFile ) ;

   lpAddr->wCountry = GetPrivateProfileInt( lpszSection, rglpsz[IDS_INI_COUNTRYCODE], COUNTRY_USA, lpszINIFile ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_COUNTRY], "", lpAddr->szCountry, MAX_COUNTRY+1, lpszINIFile ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_AREACODE], "", lpAddr->Phone.szAreaCode, MAX_AREACODE+1, lpszINIFile ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_NUMBER], "", lpAddr->Phone.szNumber, MAX_PHONE+1, lpszINIFile ) ;

   GetPrivateProfileString( lpszSection, rglpsz[IDS_INI_EXT], "", lpAddr->Phone.szExt, MAX_EXT+1, lpszINIFile ) ;

   return TRUE ;

} /* GetAddress()  */


/****************************************************************
 *  VOID NEAR PASCAL GetCreditCard( LPCREDITCARD lpCC, LPSTR lpszINIFile, UINT uiWhichCard )
 *
 *  Description: 
 *
 ****************************************************************/
BOOL WINAPI GetCreditCard( LPCREDITCARD lpCC, LPSTR lpszINIFile, UINT uiWhichCard )
{
   char szKey[80] ;
   BOOL  f = TRUE ;

   /* Assume valid */
   lpCC->fValid = TRUE ;

   wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_NAMEONCARD], uiWhichCard ) ;
   if (!GetPrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], szKey,
                                 "", lpCC->szNameOnCard,
                                 MAX_NAME+1, lpszINIFile ))
      f = FALSE ;

   wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_CARDNUMBER], uiWhichCard ) ;
   if (!GetPrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], szKey,
                                 "", lpCC->szNumber,
                                 MAX_CARDNUM+1, lpszINIFile ))
      f = FALSE ;

   wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_EXPMONTH], uiWhichCard ) ;
   lpCC->wExpMonth = GetPrivateProfileInt( rglpsz[IDS_INI_BILLINGINFO],
                              szKey, 0, lpszINIFile ) ;

   wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_EXPYEAR], uiWhichCard ) ;
   lpCC->wExpYear = GetPrivateProfileInt( rglpsz[IDS_INI_BILLINGINFO],
                                 szKey, 0, lpszINIFile ) ;

   return f ;

} /* GetCreditCard()  */

VOID WINAPI SaveCreditCards( CREDITCARD FAR rgCC[], LPSTR lpszINIFile )
{
   char  szKey[64] ;
   char  szBuf[32] ;
   WORD w ;

   for( w = MASTERCARD ; w < NUMBER_OF_CARDS ; w++)
   {
      wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_NAMEONCARD], w ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], szKey,
         rgCC[w].szNameOnCard, lpszINIFile ) ;
   
      wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_CARDNUMBER], w ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], szKey,
         rgCC[w].szNumber, lpszINIFile ) ;

      wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_EXPMONTH], w ) ;
      wsprintf( szBuf, "%d", rgCC[w].wExpMonth ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], szKey, szBuf, lpszINIFile ) ;

      wsprintf( szKey, "%s%d", (LPSTR)rglpsz[IDS_INI_EXPYEAR], w ) ;
      wsprintf( szBuf, "%d", rgCC[w].wExpYear ) ;
      WritePrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], szKey, szBuf, lpszINIFile ) ;
   }

} /* SaveCreditCards()  */

/****************************************************************
 *  BOOL WINAPI SavePassword( LPCUSTINFO lpCI, LPSTR lpszNewPassword )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI SavePassword( LPCUSTINFO lpCI, LPSTR lpszNewPassword, LPSTR lpszAppName )
{
   UINT  i ;
   char szBuf[ MAX_PASSWORD+1] ;

   lstrcpy( szBuf, lpszNewPassword ) ;

   for (i = 0 ; i < MAX_PASSWORD ; i++)
   {
      if (szBuf[i])
      {
         szBuf[i] = toupper( szBuf[i] ) ;
         szBuf[i] += 128 ;
      }
   }

   WritePrivateProfileString( lpszAppName,
                              rglpsz[IDS_INI_PASSWORD], szBuf,
                              rglpsz[IDS_INIFILE] ) ;

   lstrcpy( lpCI->szPassword, lpszNewPassword ) ;

   return TRUE ;

} /* SavePassword()  */


/****************************************************************
 *  BOOL WINAPI SaveUserID( LPCUSTINFO lpCI, LPSTR lpszNewUserID, LPSTR )
 *
 *  Description: 
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI SaveUserID( LPCUSTINFO lpCI, LPSTR lpszNewUserID, LPSTR lpszAppName )
{

#pragma message( "NEED : decrypt UserID" )
   WritePrivateProfileString( lpszAppName,
                              rglpsz[IDS_INI_USERID], lpszNewUserID,
                              rglpsz[IDS_INIFILE] ) ;

   lstrcpy( lpCI->szUserID, lpszNewUserID ) ;

   return TRUE ;

} /* SaveUserID()  */


/****************************************************************
 *  BOOL WINAPI ReadCommSettings( LPCOMMSETTINGS lpCS, LPSTR szINIFile )
 *
 *
 *  Comments:
 *
 ****************************************************************/
BOOL WINAPI ReadCommSettings( LPCOMMSETTINGS lpCS, LPSTR szINIFile )
{
   char szBuf[128] ;

   if (!GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS],
                                 rglpsz[IDS_INI_PORT],
                                 "",
                                 lpCS->szCommDevice,
                                 MAX_COMMDEVICE+1,
                                 szINIFile ))
   {
      DP1( hWDB, "Could not get [Comm Settings]:Port" ) ;

      return FALSE ;
   }

   if (!GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS],
                                 rglpsz[IDS_INI_MODEM],
                                 "",
                                 lpCS->szModemType,
                                 MAX_MODEMTYPE+1,
                                 szINIFile ))
   {
      DP1( hWDB, "Could not get [Comm Settings]:Modem" ) ;

      return FALSE ;
   }

   if (!GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS],
                                 rglpsz[IDS_INI_BAUD],
                                 "",
                                 szBuf,
                                 64,
                                 szINIFile ))
   {
      DP1( hWDB, "Could not get [Comm Settings]:Baud" ) ;
      return FALSE ;
   }

   lpCS->dwBaudRate = AtoL( szBuf ) ;

   lpCS->uiLocationFlags = GetPrivateProfileInt( rglpsz[IDS_INI_COMMSETTINGS],
                                 rglpsz[IDS_INI_LOCATION],
                                 LOCATION_OTHER,
                                 szINIFile ) ;

   lpCS->uiDialType = GetPrivateProfileInt( rglpsz[IDS_INI_COMMSETTINGS],
                                 rglpsz[IDS_INI_DIALTYPE],
                                 DIALTYPE_TONE,
                                 szINIFile ) ;


   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS],
                            rglpsz[IDS_INI_PREFIX],
                            "",
                            lpCS->szPrefix,
                            MAX_SPECIALPREFIX+1,
                            szINIFile ) ;

   GetPrivateProfileString( rglpsz[IDS_INI_COMMSETTINGS],
                            rglpsz[IDS_INI_SUFFIX],
                            "",
                            lpCS->szSuffix,
                            MAX_SPECIALPREFIX+1,
                            szINIFile ) ;

   return TRUE ;

} /* ReadCommSettings()  */


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
    if( _fstrnicmp( lp, (LPSTR)"0x",2) )
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

    while( isspace(*lp) )
        lp++;

    if (*lp=='\0')
    {
        *lpD = 0;
        return FALSE;
    }

    while (*lp!=',' && *lp!='\0')
        *lpD++ = *lp++;

    // Strip trailing spaces
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

    return (WORD)(DWORD)(lpD - lpString);

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

    wpPlacement.showCmd = (WORD)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
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
   wFileSize = LOWORD(_filelength( hFile )) ;

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
//      MAX strlen is 120!!!
//
//  History:    Date       Author     Comment
//               3/14/92   MSM        Created
//
//*************************************************************

VOID WINAPI ExpandStringToPrintable( LPSTR lpString )
{
    char szTemp[120];
    LPSTR lpS, lpD = lpString;

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

//*** EOF: ini.c
