/************************************************************************
 *
 *     Project:  CONXLIB
 *
 *      Module:  billing.c
 *
 *     Remarks:  billing info dialogs and support routines.
 *
 * NOTE:  This does not currently encrypt the credit card numbers!
 *
 *   Revisions:  
 *
 *    3/1/92   C.Kindel    Wrote it.
 *
 ***********************************************************************/
#include "PRECOMP.H"
#include "CONXLIBi.h"

/* used in SaveBillingInfo for checking card validity */
#define CARD_LEN        16
#define VISA_LEN        13
#define AMEX_LEN        15

#define TESTMASTER      "5499750000000007"
#define TESTVISA        "4005550000000019"

/****************************************************************
 * BillingInfoDialog
 ****************************************************************/
BOOL WINAPI BillingInfoDialog ( HWND hwndParent, LPSTR szHelpFile, LPSTR lpszINIFile, DWORD dwData2 )
{
   return DoSetupDialog( hwndParent, szHelpFile, DLG_BILLINGINFO, lpszINIFile, dwData2 ) ;
}  

static LPSTR NEAR PASCAL EncryptString(LPSTR unencrypted, LPSTR encrypted, UINT length) ;
static LPSTR NEAR PASCAL UnencryptString( LPSTR encrypted, LPSTR unencrypted, UINT length);
static int NEAR PASCAL   EncryptNumber(UINT unencrypted);
static int NEAR PASCAL   UnencryptNumber( UINT encrypted) ;

static BOOL NEAR PASCAL InitBillingInfo( HWND hDlg, LPSTR lpszINIFile ) ;
static VOID NEAR PASCAL SetupCreditCard( HWND hDlg, UINT w, LPCREDITCARD lpCC ) ;
static BOOL NEAR PASCAL SaveBillingInfo( HWND hDlg, LPSTR lpszINIFile ) ;
static VOID NEAR PASCAL BillingInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam ) ;

/****************************************************************
 *  BOOL CALLBACK fnBillingInfo ( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *
 *
 *
 ****************************************************************/
BOOL CALLBACK fnBillingInfo ( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   LPDLGDATA    lpDlgData ;

   switch (wMsg)
   {
      case WM_INITDIALOG:
         DP1( hWDB, "WM_INITDIALOG" ) ;
         lpDlgData = (LPDLGDATA)lParam ;

         SET_DLGDATAPROP( hDlg, lpDlgData ) ;

         /*
          * Read data from INI file
          */
         InitBillingInfo( hDlg, lpDlgData->lpszINIFile ) ;
         BillingInfoValidate( hDlg, wMsg, wParam, lParam ) ;

         /*
          * We use the uiExitBtnID member of DlgData to store
          * our currently active radio button.  This is somewhat
          * of a hack, but oh well...
          */
         lpDlgData->uiExitBtnID = DlgWhichRadioButton( hDlg,
                                                       IDD_MASTERCARD,
                                                       IDD_CORPACCT,
                                                       IDD_MASTERCARD ) ;

         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT ), lpDlgData->hfontSmall, FALSE ) ;
         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT1 ), lpDlgData->hfontSmall, FALSE ) ;
         SetWindowFont( GetDlgItem( hDlg, IDD_TEXT2 ), lpDlgData->hfontSmall, FALSE ) ;

         if (lpDlgData->dwData2 & 0x0000001)
         {
            ShowWindow( GetDlgItem( hDlg, DLG_CUSTOMERINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_SHIPPINGINFO ), SW_HIDE ) ;
            ShowWindow( GetDlgItem( hDlg, DLG_COMMSETTINGS ), SW_HIDE ) ;
         }

         DlgCenter( hDlg, GetParent( hDlg ), 0 ) ;
      return TRUE ;

      case WM_COMMAND:
      {
         UINT     idItem   = (UINT)LOWORD( wParam ) ;
         UINT     wNotifyCode = (UINT)HIWORD( lParam ) ;

         lpDlgData = GET_DLGDATAPROP( hDlg ) ;

         switch (idItem)
         {
            case IDOK:
            case IDCANCEL:
            case DLG_CUSTOMERINFO:
            case DLG_SHIPPINGINFO:
            case DLG_BILLINGINFO:
            case DLG_COMMSETTINGS:
               /*
                * These all mean "Exit", and all except IDCANCEL imply
                * "Save changes"...
                *
                * If it's not CANCEL and if IDOK is enabled then we must
                * have valid data.
                * Write that data...
                */
               if (idItem != IDCANCEL &&
                   IsWindowEnabled( GetDlgItem( hDlg, IDOK ) ))
               {
                  UINT uiPrev = lpDlgData->uiExitBtnID - IDD_MASTERCARD ;

                  GetDlgItemText( hDlg, IDD_NAMEONCARD, lpDlgData->CC[uiPrev].szNameOnCard, MAX_NAME+1 ) ;
   
                  GetDlgItemText( hDlg, IDD_CARDNUMBER, lpDlgData->CC[uiPrev].szNumber, MAX_CARDNUM+1 ) ;

                  lpDlgData->CC[uiPrev].wExpMonth = GetDlgItemInt( hDlg, IDD_EXPMONTH, NULL, FALSE ) ;

                  lpDlgData->CC[uiPrev].wExpYear = GetDlgItemInt( hDlg, IDD_EXPYEAR, NULL, FALSE ) ;

                  if (!SaveBillingInfo( hDlg, lpDlgData->lpszINIFile ))
                     break ;
               }

               lpDlgData->uiExitBtnID = idItem ;
               EndDialog( hDlg, idItem ) ;
            break ;

            case IDD_MASTERCARD:
            case IDD_VISA:
            case IDD_DISCOVER:
            case IDD_AMEX:
            case IDD_CORPACCT:
               if (wNotifyCode == BN_CLICKED)
               {
                  UINT uiCur = idItem - IDD_MASTERCARD ;
                  UINT uiPrev = lpDlgData->uiExitBtnID - IDD_MASTERCARD ;

                  GetDlgItemText( hDlg, IDD_NAMEONCARD, lpDlgData->CC[uiPrev].szNameOnCard, MAX_NAME+1 ) ;
   
                  GetDlgItemText( hDlg, IDD_CARDNUMBER, lpDlgData->CC[uiPrev].szNumber, MAX_CARDNUM+1 ) ;

                  lpDlgData->CC[uiPrev].wExpMonth = GetDlgItemInt( hDlg, IDD_EXPMONTH, NULL, FALSE ) ;

                  lpDlgData->CC[uiPrev].wExpYear = GetDlgItemInt( hDlg, IDD_EXPYEAR, NULL, FALSE ) ;

                  SetupCreditCard( hDlg, idItem - IDD_MASTERCARD,
                                   &lpDlgData->CC[uiCur] ) ;

                  lpDlgData->uiExitBtnID = idItem ;
               }
            break ;

            case IDD_NAMEONCARD:
            case IDD_CARDNUMBER:
            case IDD_EXPMONTH:
            case IDD_EXPYEAR:
               if (wNotifyCode == EN_CHANGE)
                  BillingInfoValidate( hDlg, wMsg, wParam, lParam ) ;
            break ;

            case IDD_HELP:
               WinHelp( GetParent( hDlg ), lpDlgData->lpszHelpFile, HELP_KEY,
                        (DWORD)(LPSTR)GRCS( IDS_HELP_BILLING ) );
            break ;

            default :
               return FALSE ;
         }
         break ;
      }

      case WM_PAINT:
         UpdateIndicators( hDlg ) ;
      return FALSE ;

      case WM_INDICATORPAINT:
         lpDlgData = GET_DLGDATAPROP( hDlg ) ;
         lpDlgData->fIndicatorPaint = FALSE ;
      /*
       * Paint our "required data" indicators...
       */
      {
         HDC hDC = GetDC( hDlg ) ;
         HFONT hfont ;

         hfont = SelectObject( hDC, (HFONT)SendMessage( hDlg, WM_GETFONT, 0, 0L ) ) ;
         SetTextColor( hDC, RGBLTRED ) ;
         SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;

         /*
          * Members always required
          */
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_NAMEONCARD ) ) ;
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_CARDNUMBER) ) ;
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_EXPMONTH ) ) ;
         PaintIndicator( hDC, GetDlgItem( hDlg, IDD_EXPYEAR ) ) ;

         SelectObject( hDC, hfont ) ;

         ReleaseDC( hDlg, hDC ) ;
      }
      break ;

      case WM_DESTROY:
         REMOVE_DLGDATAPROP( hDlg ) ;
      return FALSE ;

      default:
         return FALSE ;

   }
   return TRUE ;

} /* fnBillingInfo()  */


/****************************************************************
 *
 *  BOOL NEAR PASCAL InitBillingInfo( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL InitBillingInfo( HWND hDlg, LPSTR lpszINIFile )
{
   UINT w ;
   LPDLGDATA lpDlgData = GET_DLGDATAPROP( hDlg ) ;
   
   /*
    * Do the EM_LIMITTEXT thang on the edit controls.
    */
   SendDlgItemMessage( hDlg, IDD_NAMEONCARD, EM_LIMITTEXT, MAX_NAME, 0L ) ;
   SendDlgItemMessage( hDlg, IDD_CARDNUMBER, EM_LIMITTEXT, MAX_CARDNUM, 0L ) ;
   SendDlgItemMessage( hDlg, IDD_EXPMONTH, EM_LIMITTEXT, 2, 0L ) ;
   SendDlgItemMessage( hDlg, IDD_EXPYEAR, EM_LIMITTEXT, 2, 0L ) ;

   /*
    * Read the data
    */
   for (w = MASTERCARD ; w < NUMBER_OF_CARDS ; w++)
      GetCreditCard( &lpDlgData->CC[w], lpszINIFile, w ) ;

   w = GetPrivateProfileInt( rglpsz[IDS_INI_BILLINGINFO], rglpsz[IDS_INI_WHICHCARD], MASTERCARD, lpszINIFile ) ;
   if (w > CORPORATE_CARD)
      w = MASTERCARD ;
   CheckRadioButton( hDlg, IDD_MASTERCARD, IDD_CORPACCT, IDD_MASTERCARD + w ) ;

   SetupCreditCard( hDlg, w, &lpDlgData->CC[w] ) ;

   return TRUE ;
} /* InitBillingInfo()  */


VOID NEAR PASCAL SetupCreditCard( HWND hDlg, UINT w, LPCREDITCARD lpCC )
{
   HDC hDC = GetDC( hDlg ) ;
   HFONT hfont ;

   SetDlgItemText( hDlg, IDD_NAMEONCARD, lpCC->szNameOnCard ) ;
   SetDlgItemText( hDlg, IDD_CARDNUMBER, lpCC->szNumber ) ;

   if (lpCC->wExpMonth)
      SetDlgItemInt( hDlg, IDD_EXPMONTH, lpCC->wExpMonth, FALSE ) ;
   else
      SetDlgItemText( hDlg, IDD_EXPMONTH, "" ) ;

   if (lpCC->wExpYear)
      SetDlgItemInt( hDlg, IDD_EXPYEAR, lpCC->wExpYear, FALSE ) ;
   else
      SetDlgItemText( hDlg, IDD_EXPYEAR, "" ) ;

   hfont = SelectObject( hDC, (HFONT)SendMessage( hDlg, WM_GETFONT, 0, 0L ) ) ;
   SetTextColor( hDC, RGBLTRED ) ;
   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;

   /*
    * Members always required
    */
   PaintIndicator( hDC, GetDlgItem( hDlg, IDD_NAMEONCARD ) ) ;
   PaintIndicator( hDC, GetDlgItem( hDlg, IDD_CARDNUMBER) ) ;
   PaintIndicator( hDC, GetDlgItem( hDlg, IDD_EXPMONTH ) ) ;
   PaintIndicator( hDC, GetDlgItem( hDlg, IDD_EXPYEAR ) ) ;

   SelectObject( hDC, hfont ) ;
   ReleaseDC( hDlg, hDC ) ;

} /* ReadCreditCard()  */


/****************************************************************
 *
 *  BOOL NEAR PASCAL SaveBillingInfo( HWND hDlg, LPSTR lpszINIFile )
 *
 ****************************************************************/
BOOL NEAR PASCAL SaveBillingInfo( HWND hDlg, LPSTR lpszINIFile )
{
   UINT  w, uiCount ;
   char  szBuf[3] ;
   char  szName[MAX_NAME+1] ;
   char  szCardNum[MAX_CARDNUM+1] ;
   char  szTempNum[MAX_CARDNUM+1] ;
   UINT  uiExpMonth, uiExpYear ;
   BOOL  fStatus ;

   LPDLGDATA lpDlgData = GET_DLGDATAPROP( hDlg ) ;

   if (!lpDlgData)
   {
      DP1( hWDB, "lpDlgData == NULL!!!" ) ;
      return FALSE ;
   }

   w = DlgWhichRadioButton( hDlg, IDD_MASTERCARD, IDD_CORPACCT, IDD_MASTERCARD ) ;

   /*
    * Validate the credit card info that was entered.
    *
    * Get the contents of the current card.  The card number
    * must pass some tests.
    */
   GetDlgItemText( hDlg, IDD_NAMEONCARD, szName, MAX_NAME+1 ) ;
   GetDlgItemText( hDlg, IDD_CARDNUMBER, szCardNum, MAX_CARDNUM+1 ) ;
   uiExpMonth = GetDlgItemInt( hDlg, IDD_EXPMONTH, NULL, FALSE ) ;
   uiExpYear = GetDlgItemInt( hDlg, IDD_EXPYEAR, NULL, FALSE ) ;
    
   w -= IDD_MASTERCARD ;

   if ( w != CORPORATE_CARD ) // Corp acct doesn't have name or exp date
   {
      /* Check the month and year for valid numbers */
      if ( (uiExpMonth < 1) || (uiExpMonth > 12) )
      {
         ErrorResBox( hDlg, hmodDLL,
                      MB_ICONHAND, IDS_APPNAME, IDS_INVALID_EXPMONTH) ;
         return FALSE ;
      }

      if (uiExpYear < 92)
      {
         ErrorResBox( hDlg, hmodDLL,
                      MB_ICONHAND, IDS_APPNAME, IDS_INVALID_EXPYEAR) ;
         return FALSE ;
      }
    }

    /* Check the card number for validity */
    uiCount = StripSpaces (szCardNum, szTempNum, sizeof(szTempNum));

    /* set ourselves up for failure */
    fStatus = FALSE;

    /* Check the length of the card numbers, and the prefixes to make sure
       they are valid */
    switch ( w )
    {
        case MASTERCARD:
            if ( (uiCount == CARD_LEN) && (szTempNum[0] == '5') && 
                 (szTempNum[1] >= '1') && (szTempNum[1] <= '5'))
                fStatus = TRUE;
            break;

        case VISA:
            if ( ((uiCount == CARD_LEN) || (uiCount == VISA_LEN)) &&
                 (szTempNum[0] == '4') )
                fStatus = TRUE;
            break;

        case AMERICAN_EXPRESS:
            if (  (uiCount == AMEX_LEN) && (szTempNum[0] == '3') &&
                 ((szTempNum[1] == '4') || (szTempNum[1] == '7')) )
                fStatus = TRUE;
            break;

        case DISCOVER:
            if (  (uiCount == CARD_LEN) && ( !_fstrncmp(szTempNum, "6011", 4) ) )
                fStatus = TRUE;
            break;

        case CORPORATE_CARD:
            if ( (uiCount == CARD_LEN) && (szTempNum[0] == '9') )
                fStatus = TRUE;
            break;
    }

    /* Do the MOD 10 formula for validiation */
    if ( fStatus )
        fStatus = Mod10Validation (szTempNum, uiCount);

#if !defined(INTERNAL)  /* commented out until we ship */

    /* These two card numbers will always give approval, they are used 
       for tests.  If the user types them in, make it look like an 
       invalid number */
    if ( fStatus )
        if ( !lstrcmp (szTempNum, TESTMASTER) || !lstrcmp (szTempNum, TESTVISA) )
            fStatus = FALSE;
#endif

   if ( fStatus == FALSE )
   {
      ErrorResBox( hDlg, hmodDLL,
                      MB_ICONHAND, IDS_APPNAME, IDS_INVALID_CARDNUMBER ) ;
      return FALSE ;
   }

   wsprintf( szBuf, "%d", w) ;
   WritePrivateProfileString( rglpsz[IDS_INI_BILLINGINFO], rglpsz[IDS_INI_WHICHCARD], szBuf, lpszINIFile ) ;

   DP1( hWDB, "SaveCreditCards..." ) ;

   SaveCreditCards( lpDlgData->CC, lpszINIFile ) ;

   return TRUE ;

} /* SaveBillingInfo()  */


/****************************************************************
 *  VOID NEAR PASCAL BillingInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
 *
 *  Description: 
 *
 *  Type/Parameter
 *          Description
 * 
 *  Return Value
 *          Returns one of the following values:
 * 
 *             Value
 *                      Meaning
 *
 *
 *  Comments:
 *
 ****************************************************************/
VOID NEAR PASCAL BillingInfoValidate( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   if (  
         GetWindowTextLength( GetDlgItem( hDlg, IDD_NAMEONCARD ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_CARDNUMBER ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_EXPMONTH ) ) &&
         GetWindowTextLength( GetDlgItem( hDlg, IDD_EXPYEAR ) )
      )
      EnableWindow( GetDlgItem( hDlg, IDOK ), TRUE ) ;
   else
      EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE ) ;

   UpdateIndicators( hDlg ) ;

} /* BillingInfoValidate()  */


/*************************************************************\
 ** StripSpaces
 **
 ** ARGUMENTS:   LPSTR   oldNumber, newNumber
 **              int     size   
 **
 ** DESCRIPTION: Strips the spaces out of oldNumber and puts
 **              the resulting string in newNumber.  Returns
 **              the number of characters copied to newNumber
 **  
 ** ASSUMES:     newNumber is a buffer big enough to hold the 
 **              new string
 **
 ** RETURNS:     TRUE if successful, FALSE if not.
 **
 ** HISTORY:     Lauren Bricker  May 21, 1990 (written)
\*************************************************************/


int WINAPI StripSpaces (LPSTR oldNumber, LPSTR newNumber, int size)
{
    int     count;
    LPSTR   tmpOld, tmpNew;

    count  = 0;
    tmpOld = oldNumber;
    tmpNew = newNumber;

    while (count < size)
    {
        if (*tmpOld)
        {
            if ( (*tmpOld != ' ') && (*tmpOld != '-') )
            {
                *tmpNew++ = *tmpOld++;
                count++;
            }
            else
                tmpOld++;
        }
        else 
            break;
    }
    *tmpNew = '\0';

    return count;
}

/*************************************************************\
 ** StripCardDashes
 **
 ** ARGUMENTS:   LPSTR   ccNumber
 **
 ** DESCRIPTION: Strips the dashes out of credit card number
 **
 ** RETURNS:     nothing
 **
 ** HISTORY:     Lauren Bricker  May 16, 1991 (written)
\*************************************************************/


VOID WINAPI StripCardDashes (LPSTR ccNumber)
{
    LPSTR   ptr = ccNumber;

    while ( *ptr )
    {
        if ( *ptr == '-' )
            *ptr = ' ';
        ptr++;
    }
}

/*************************************************************\
 ** Mod10Validation
 **
 ** ARGUMENTS:   LPSTR   w
 **              int     count;
 **
 ** DESCRIPTION: Returns TRUE if the card passes the Mod 10
 **              formula for validation.  Returns FALSE 
 **              otherwise.
 **
 ** HISTORY:     Lauren Bricker  May 21, 1990 (written)
\*************************************************************/
BOOL WINAPI Mod10Validation (LPSTR w, int count)
{
    int     ii, total, doubleDigit;
    char    tmpDouble[MAX_CARDNUM];
    LPSTR   doublePtr;

    /* first add up every other digit starting with the rightmost */
    ii = count - 1;
    total = 0;
    while ( ii >= 0 )
    {
        total += (w[ii] - '0');
        ii -= 2;
    }

    /* now double every other digit starting with the second from 
       the rightmost, and add the digits of the resulting number */

    ii = count - 2;

    while ( ii >= 0 )
    {
        doubleDigit = (w[ii] - '0') * 2;
        wsprintf( tmpDouble, "%d", doubleDigit ) ;
        doublePtr = tmpDouble;
        while ( *doublePtr )
        {
            total += (*doublePtr - '0');
            doublePtr++;
        }
        ii -= 2;
    }

    if ( total % 10 )
        return FALSE;
    else
        return TRUE;
}

#if 0
/*************************************************************\
 ** EncryptString
 **
 ** ARGUMENTS:   LPSTR unencrypted, encrypted;
 **              int   length;
 **
 ** DESCRIPTION: Encrypt the unencrypted string.
 **
 ** ASSUMES:     encrypted is a buffer large enough to hold 
 **              the newly encrypted word
 **
 ** MODIFIES:    encrypted
 **
 ** RETURNS:     Pointer to the encrypted string.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static LPSTR NEAR PASCAL 
EncryptString(unencrypted, encrypted, length)
    LPSTR       unencrypted, encrypted;
    unsigned    length;
{
    int unLen, passLen, ii, ij;

    unLen = lstrlen (unencrypted);
    passLen = lstrlen (custinfo.password);
    ij = 0;

    for ( ii = 0; (ii < unLen) && (ii < (int)length); ii++ )
    {
        encrypted[ii] = (char)(unencrypted[ii] + tolower(custinfo.password[ij]));
        ++ij;
        ij %= passLen;
    }

    encrypted[ii] = '\0';

    return encrypted;
}




/*************************************************************\
 ** UnencryptString
 **
 ** ARGUMENTS:   LPSTR encrypted, unencrypted;
 **              int   length;
 **
 ** DESCRIPTION: Unencrypt the encrypted string.
 **
 ** ASSUMES:     unencrypted is a buffer large enough to hold 
 **              the newly unencrypted word
 **
 ** MODIFIES:    unencrypted
 **
 ** RETURNS:     Pointer to the unencrypted string.
 **
 ** HISTORY:     Lauren Bricker  May 11, 1990 (written)
\*************************************************************/

static LPSTR NEAR PASCAL 
UnencryptString (encrypted, unencrypted, length)
    LPSTR       encrypted, unencrypted;
    unsigned    length;
{
    int enLen, passLen, ii, ij;

    enLen = lstrlen (encrypted);
    passLen = strlen (custinfo.password);
    ij = 0;

    for ( ii = 0; (ii < enLen) && (ii < (int)length); ii++ )
    {
        unencrypted[ii] = (char)(encrypted[ii] - tolower(custinfo.password[ij]));
        ++ij;
        ij %= passLen;
    }

    unencrypted[ii] = '\0';

    return unencrypted;
}

/*************************************************************\
 ** EncryptNumber
 **
 ** ARGUMENTS:   int unencrypted
 **
 ** DESCRIPTION: Encrypt the unencrypted number.
 **
 ** NOTE:        Will not encrypt 0
 **
 ** RETURNS:     the encrypted number.
 **
 ** HISTORY:     Lauren Bricker  June 7, 1990 (written)
\*************************************************************/

static int NEAR PASCAL 
EncryptNumber(unencrypted)
    int unencrypted;
{
    if ( unencrypted )
        return (unencrypted + tolower (custinfo.password[0]));
    else 
        return unencrypted;
}




/*************************************************************\
 ** UnencryptNumber
 **
 ** ARGUMENTS:   int encrypted;
 **
 ** DESCRIPTION: Unencrypt the encrypted number.
 **
 ** NOTE:        Will not unencrypt 0
 **
 ** RETURNS:     the unencrypted number.
 **
 ** HISTORY:     Lauren Bricker  June 7, 1990 (written)
\*************************************************************/

static int NEAR PASCAL 
UnencryptNumber (encrypted)

    int encrypted;
{
    if ( encrypted )
        return (encrypted - tolower (custinfo.password[0])); 
    else 
        return encrypted;
}
#endif

/************************************************************************
 * End of File: billing.c
 ***********************************************************************/

