//*************************************************************
//  File name: config.c
//
//  Description: 
//      Handles the saving and restoring of the configuration to the INI
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include <direct.h>
#include "global.h"

static char gszScratch[120] ;

typedef UINT (CALLBACK *LPFNHOOKPROC)(HWND, UINT, WPARAM, LPARAM); 
LRESULT CALLBACK fnOptions( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;

//*************************************************************
//
//  SaveConfiguration
//
//  Purpose:
//      Save the current (global) configuration to the ini file
//
//
//  Parameters:
//      
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

VOID WINAPI SaveConfiguration()
{
    int nCol1, nCol2, nCol3;
    char szTemp[40];
    DWORD dw;

    if (ghfontUser != GetStockObject( SYSTEM_FONT ))
        WritePrivateProfileFont( GRCS(IDS_CONFIG), GRCS(IDS_FONT), 
            ghfontUser, GRCS(IDS_INIFILE) );

    nCol1 = (int)SendMessage( ghwndSlider, SL_GETCOLPOS, PRICE_COL, 0L );
    nCol2 = (int)SendMessage( ghwndSlider, SL_GETCOLPOS, SIZE_COL, 0L );
    nCol3 = (int)SendMessage( ghwndSlider, SL_GETCOLPOS, PUB_COL, 0L );

    wsprintf( szTemp, "%d, %d, %d", nCol1, nCol2, nCol3 );
    WritePrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_SLIDER), 
        szTemp, GRCS(IDS_INIFILE) );

    WritePrivateProfileWndPos( GRCS(IDS_CONFIG), GRCS(IDS_WNDPOS), 
        ghwndMain, GRCS(IDS_INIFILE) );

    if (glpDB)
    {
        //*** Cram mucho info into one DWORD
        dw = MAKELONG( glpDB->uSort, glpDB->uView );

        if (glpDB->hFilter)
            dw += 0x80000000L;

        WritePrivateProfileDWORD( GRCS(IDS_CONFIG), GRCS(IDS_VIEW), dw,
            GRCS(IDS_INIFILE) );
    }

    WritePrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_DOWNLOADDIR), 
        gszDestPath, GRCS(IDS_INIFILE) );
    
    wsprintf( szTemp, "%d", gfInstallFonts ) ;
    WritePrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_INSTALLFONTS), 
        szTemp, GRCS(IDS_INIFILE) );



} //*** SaveConfiguration


//*************************************************************
//
//  RestoreConfiguration
//
//  Purpose:
//      Restores the (global) configuration
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (LPWINDOWPLACEMENT WINAPI)
//      pointer to the main windows placement
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

LPWINDOWPLACEMENT WINAPI RestoreConfiguration(VOID)
{
    char szTemp[40];
    LPSTR lpP = (LPSTR)szTemp;
    int  nCol1, nCol2, nCol3, nConv;

    GetPrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_DOWNLOADDIR), 
        "", gszDestPath, 120, GRCS(IDS_INIFILE) ) ;
    
    if (*gszDestPath == '\0')
      wsprintf( gszDestPath, "%sFONTS\\", (LPSTR)gszExePath ) ;
    else
    {
      nCol1 = lstrlen( gszDestPath ) ;
      if (gszDestPath[nCol1 - 1] != '\\')
      { 
         gszDestPath[nCol1+1] = '\0' ;
         gszDestPath[nCol1] = '\\' ;
      }
    }

    gfInstallFonts = GetPrivateProfileInt( GRCS(IDS_CONFIG),
            GRCS(IDS_INSTALLFONTS), TRUE, GRCS(IDS_INIFILE) );

    ghfontUser  = GetPrivateProfileFont( GRCS(IDS_CONFIG), GRCS(IDS_FONT),
                        GRCS(IDS_INIFILE) );
    if (!ghfontUser)
        ghfontUser = ReallyCreateFontEx( NULL, "MS Sans Serif", NULL, 8, RCF_NORMAL );

    ELBSetFont( ghwndELB, ghfontUser );

    GetPrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_SLIDER), "", szTemp,
        40, GRCS(IDS_INIFILE) );

    nCol1 = (int)ParseOffNumber( (LPSTR FAR *)&lpP, &nConv );
    if (!nConv)
        goto end_rest;
    nCol2 = (int)ParseOffNumber( (LPSTR FAR *)&lpP, &nConv );
    if (!nConv)
        goto end_rest;
    nCol3 = (int)ParseOffNumber( (LPSTR FAR *)&lpP, &nConv );
    if (!nConv)
        goto end_rest;

    SendMessage( ghwndSlider, SL_SETCOLPOS, PRICE_COL, (LONG)nCol1 );
    SendMessage( ghwndSlider, SL_SETCOLPOS, SIZE_COL, (LONG)nCol2 );
    SendMessage( ghwndSlider, SL_SETCOLPOS, PUB_COL, (LONG)nCol3 );

end_rest:
    return GetPrivateProfileWndPos( GRCS(IDS_CONFIG), GRCS(IDS_WNDPOS),
                GRCS(IDS_INIFILE) );

} //*** RestoreConfiguration


//*************************************************************
//
//  DoChooseFont
//
//  Purpose:
//      Chooses a new font for the ghfontUser
//
//
//  Parameters:
//      void
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 1/92   MSM        Created
//
//*************************************************************

BOOL WINAPI DoChooseFont( void )
{
    CHOOSEFONT cf;
    LOGFONT    lf;

    GetObject( ghfontUser, sizeof(LOGFONT), (LPSTR)&lf );

    cf.lStructSize = (DWORD)sizeof(CHOOSEFONT);
    cf.hwndOwner = ghwndMain;
    cf.hDC = GetDC(ghwndMain);
    cf.lpLogFont = (LPLOGFONT)&lf;
    cf.rgbColors = 0L;
    cf.Flags = CF_SCREENFONTS|CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT|
               CF_ENABLEHOOK |
               CF_ENABLETEMPLATE |
               CF_SHOWHELP ;
    cf.lpfnHook = (LPFNHOOKPROC)MakeProcInstance( (FARPROC)fnOptions, ghInst ) ;
    cf.lpTemplateName = MAKEINTRESOURCE( OPTIONS_DLG ) ;
    cf.hInstance      = ghInst;

    if( ChooseFont( &cf ) )
    {
        if (ghfontUser != GetStockObject( SYSTEM_FONT ))
            DeleteObject( ghfontUser ) ;

        ghfontUser = CreateFontIndirect( &lf ) ;

        //*** Update the ini file
        WritePrivateProfileFont( GRCS(IDS_CONFIG), GRCS(IDS_FONT), 
            ghfontUser, GRCS(IDS_INIFILE) );

        ELBSetFont( ghwndELB, ghfontUser );
    }

    FreeProcInstance( (FARPROC)cf.lpfnHook ) ;

    ReleaseDC( ghwndMain, cf.hDC );
    return TRUE;

} //*** DoChooseFont

LRESULT CALLBACK fnOptions( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uiMsg)
   {
      case WM_INITDIALOG:
      {
         HWND hwndEdt = GetDlgItem( hDlg, edt1 ) ;

         Edit_LimitText( hwndEdt, 143 ) ;
         Edit_SetText( hwndEdt, gszDestPath ) ;

         CheckDlgButton( hDlg, chx3, gfInstallFonts ) ;

         DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
      }
      break ;

      case WM_COMMAND:
      {
         UINT  idItem = wParam ;
         HWND  hwndCtl = (HWND)LOWORD( lParam ) ;
         WORD  wNotifyCode = HIWORD( lParam ) ;

         switch (wParam)
         {
            case IDOK:
            {
               char szTemp[144] ;
               int   nLen ;

               nLen = GetDlgItemText( hDlg, edt1, szTemp, 143 ) ;

               if (VerifyDestDir( hDlg, szTemp ) == FALSE)
                  return TRUE ;

               lstrcpy( gszDestPath, szTemp ) ;

               nLen = lstrlen( gszDestPath ) ;
               if (gszDestPath[nLen - 1] != '\\')
               { 
                  gszDestPath[nLen+1] = '\0' ;
                  gszDestPath[nLen] = '\\' ;
               }

               gfInstallFonts = IsDlgButtonChecked( hDlg, chx3 ) ;
               return FALSE ;
            }
            break ;

            case edt1:
               if (wNotifyCode == EN_CHANGE)
                  EnableWindow( GetDlgItem( hDlg, IDOK ), Edit_GetTextLength( hwndCtl ) ) ;
            break ;

            case pshHelp:
               ViewFontshopHelp( IDS_HELP_OPTIONS ) ;
            break ;
         }
      }
      break;

   }
   /*
    * return false to pass message on to default
    */
   return FALSE ;

}

 
BOOL WINAPI VerifyDestDir( HWND hDlg, PSTR psz ) 
{
   int  nLen ;

   lstrcpy( gszScratch, psz ) ;
   nLen = lstrlen( gszScratch ) ;

   if (nLen > 3 && gszScratch[nLen-1] == '\\')
                  gszScratch[nLen-1] = '\0' ;

   /* See if the directory exists.
    */
   DP1( hWDB, "Verifying directory '%s'", (LPSTR)gszScratch ) ;
   if (0 != _access( gszScratch, 0 ))
   {
      /* If not, create it...
       */
      DP1( hWDB, "   Dir does not exist!" ) ;
      if (IDYES == ErrorResBox( hDlg, ghInst,
                     MB_ICONQUESTION | MB_YESNO, IDS_APPNAME,
                     IDS_ERR_DIRNOEXIST, (LPSTR)psz ))
      {
         DP1( hWDB, "   Attempting to make dir" ) ;
         if (0 != _mkdir( gszScratch ))
         {
            DP1( hWDB, "   _mkdir failed" ) ;
            ErrorResBox( hDlg, ghInst, MB_ICONEXCLAMATION,
               IDS_APPNAME, IDS_ERR_COULDNOTCREATEDIR,
               (LPSTR)psz ) ;
            return FALSE ;
         }
      }
      else
         return FALSE ;
   }

   return TRUE ;
}



//*************************************************************
//
//  GetIniPanoseNumbers
//
//  Purpose:
//      Retrieves the panose numbers from the INI file
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI GetIniPanoseNumbers(VOID)
{
    char szTemp[120];
    int  nConv, i;
    LPSTR lp = szTemp;

#define GETNUM(ELEM) gPanose.ELEM = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );\
                  if (!nConv)\
                     gPanose.ELEM = 0;

    GetPrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_PANOSE), 
        GRCS(IDS_DEFPANOSE), lp, 120, GRCS(IDS_INIFILE) );

    GETNUM(bFamilyType);
    GETNUM(bSerifStyle);
    GETNUM(bWeight);
    GETNUM(bProportion);
    GETNUM(bContrast);
    GETNUM(bStrokeVariation);
    GETNUM(bArmStyle);
    GETNUM(bLetterform);
    GETNUM(bMidline);
    GETNUM(bXHeight);

    GetPrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_WEIGHTS), 
        GRCS(IDS_DEFWEIGHTS), lp, 120, GRCS(IDS_INIFILE) );

    for (i=0; i<=9; i++)
    {
        gwaWeights[i] = (int)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );
        if (!nConv)
            gwaWeights[i] = 0;
    }

} //*** GetIniPanoseNumbers


//*************************************************************
//
//  SetIniPanoseNumbers
//
//  Purpose:
//      Sets the panose numbers to the ini file
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

VOID WINAPI SetIniPanoseNumbers(VOID)
{
    char  szTemp[120];
    LPSTR lp = szTemp;
    int   i;

#define SETNUM(ELEM) lp += wsprintf( lp, "%d,", gPanose.ELEM );

    SETNUM(bFamilyType);
    SETNUM(bSerifStyle);
    SETNUM(bWeight);
    SETNUM(bProportion);
    SETNUM(bContrast);
    SETNUM(bStrokeVariation);
    SETNUM(bArmStyle);
    SETNUM(bLetterform);
    SETNUM(bMidline);
    SETNUM(bXHeight);

    *(lp-1) = 0;
    lp = szTemp;

    WritePrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_PANOSE), 
        lp, GRCS(IDS_INIFILE) );

    for (i=0; i<=9; i++)
        lp += wsprintf( lp, "%d,", gwaWeights[i] );

    *(lp-1) = 0;  // Strip off the last comma
    lp = szTemp;

    WritePrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_WEIGHTS), 
        lp, GRCS(IDS_INIFILE) );

} //*** SetIniPanoseNumbers


//*************************************************************
//
//  GetIniFilter
//
//  Purpose:
//      Stores the gFilter variable
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/26/92   MSM        Created
//
//*************************************************************

VOID WINAPI GetIniFilter(VOID)
{
    char  szTemp[255];
    LPSTR lp = (LPSTR)szTemp;
    int   nConv;

#define GETNUM2(ELEM) gFilter.ELEM = (long)ParseOffNumber( (LPSTR FAR *)&lp, &nConv );\
                  if (!nConv)\
                     gFilter.ELEM = -1;

    GetPrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_FILTER), 
        GRCS(IDS_DEFFILTER), szTemp, 255, GRCS(IDS_INIFILE) );

    if (!ParseOffString( (LPSTR FAR *)&lp, gFilter.szPubName ))
        gFilter.szPubName[0] = 0;

    if (!ParseOffString( (LPSTR FAR *)&lp, gFilter.szFontName ))
        gFilter.szFontName[0] = 0;

    GETNUM2( lPrice1 );
    GETNUM2( lPrice2 );
    GETNUM2( lSize1 );
    GETNUM2( lSize2 );

} //*** GetIniFilter


//*************************************************************
//
//  SetIniFilter
//
//  Purpose:
//      Retrieves the gFilter Variable
//
//
//  Parameters:
//      VOID
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/26/92   MSM        Created
//
//*************************************************************

VOID WINAPI SetIniFilter(VOID)
{
    char  szTemp[255];

    wsprintf( szTemp, "%s,%s,%ld,%ld,%ld,%ld", (LPSTR)gFilter.szPubName,
        (LPSTR)gFilter.szFontName, gFilter.lPrice1, gFilter.lPrice2,
        gFilter.lSize1, gFilter.lSize2 );

    WritePrivateProfileString( GRCS(IDS_CONFIG), GRCS(IDS_FILTER), 
        szTemp, GRCS(IDS_INIFILE) );
             
} //*** SetIniFilter

//*** EOF: config.c
