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

#include "global.h"
#include <commdlg.h>

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

    ghfontSmall = ReallyCreateFontEx( NULL, "MS Sans Serif", NULL, 8, RCF_BOLD );
    ghfontUser  = GetPrivateProfileFont( GRCS(IDS_CONFIG), GRCS(IDS_FONT),
                        GRCS(IDS_INIFILE) );
    if (!ghfontUser)
        ghfontUser = ReallyCreateFontEx( NULL, "MS Sans Serif", NULL, 8, RCF_BOLD );

    ELBSetFont( ghwndELB, ghfontUser );
    SendMessage( ghwndSlider, WM_SETFONT, (WPARAM)ghfontSmall, (LPARAM)TRUE );
    SendMessage( ghwndStat, WM_SETFONT, (WPARAM)ghfontSmall, (LPARAM)TRUE );

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
    cf.Flags = CF_SCREENFONTS|CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT;
    cf.lpfnHook = NULL;
    cf.lpTemplateName = NULL;
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

    ReleaseDC( ghwndMain, cf.hDC );
    return TRUE;

} //*** DoChooseFont

//*** EOF: config.c
