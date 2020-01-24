//*************************************************************
//  File name: filter.c
//
//  Description: 
//      Handles the filter DLG and filter sorting
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include "filter.h"

FILTER gFilter;

BOOL GetFloatValue( LPSTR lpString, LPLONG lplValue );

//*************************************************************
//
//  SetFilterProc
//
//  Purpose:
//      Dlg proc for the filter dialog
//
//
//  Parameters:
//      HWND hDlg
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL CALLBACK)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

BOOL CALLBACK SetFilterProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            GetIniFilter();
            FillFilterDialog(hDlg);
            SendDlgItemMessage(hDlg,IDE_PUB,EM_LIMITTEXT,32,0L);
            SendDlgItemMessage(hDlg,IDE_NAME,EM_LIMITTEXT,32,0L);
            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;
            return TRUE;
        }
        break;

        case WM_COMMAND:
        {
            switch (wParam)
            {
                case IDCANCEL:
                    EndDialog( hDlg, FALSE );

                    if (glpDB->hFilter)
                        SetDBELBToFilter( ghwndELB, NULL );
                    SendMessage( ghwndToolbar, TBM_SETBTNSTATE, IDM_SET_FILTER, FALSE );
                break;

                case IDOK:
                    if (SuckFilterDlgData( hDlg ))
                    {
                        SetIniFilter();
                        EndDialog( hDlg, TRUE );
                        SendMessage(ghwndToolbar,TBM_SETBTNSTATE,IDM_SET_FILTER,TRUE);
                        SetDBELBToFilter( ghwndELB, &gFilter );
                        return TRUE;
                    }
                    else
                        MessageBeep(0);

                break;

                case IDB_HELP:
                    return ViewFontshopHelp( IDS_HELP_FILTER );
                break;
            }
        }
        break;
    }
    return FALSE;

} //*** SetFilterProc


//*************************************************************
//
//  FillFilterDialog
//
//  Purpose:
//      Fills the list box with gFilter
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/26/92   MSM        Created
//
//*************************************************************

VOID FillFilterDialog( HWND hDlg )
{
    char szTemp[40];

    SetDlgItemText( hDlg, IDE_PUB, (LPSTR)gFilter.szPubName );
    SetDlgItemText( hDlg, IDE_NAME, (LPSTR)gFilter.szFontName );

    if (gFilter.lPrice1 >= 0)
    {
        wsprintf(szTemp,"%ld.%d",gFilter.lPrice1/100, gFilter.lPrice1%100);
        SetDlgItemText( hDlg, IDE_PRICE1, (LPSTR)szTemp );
    }
    if (gFilter.lPrice2 >= 0)
    {
        wsprintf(szTemp,"%ld.%d",gFilter.lPrice2/100, gFilter.lPrice2%100);
        SetDlgItemText( hDlg, IDE_PRICE2, (LPSTR)szTemp );
    }

    if (gFilter.lSize1 >= 0)
    {
        wsprintf(szTemp,"%ld.%d",gFilter.lSize1/1024, gFilter.lSize1%1024);
        SetDlgItemText( hDlg, IDE_SIZE1, (LPSTR)szTemp );
    }
    if (gFilter.lSize2 >= 0)
    {
        wsprintf(szTemp,"%ld.%d",gFilter.lSize2/1024, gFilter.lSize2%1024);
        SetDlgItemText( hDlg, IDE_SIZE2, (LPSTR)szTemp );
    }

} //*** FillFilterDialog


//*************************************************************
//
//  SuckFilterDlgData
//
//  Purpose:
//      Sucks the filter dlg data out
//
//
//  Parameters:
//      HWND hDlg
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/26/92   MSM        Created
//
//*************************************************************

BOOL SuckFilterDlgData( HWND hDlg )
{
    char szTemp[40];

    if (!GetDlgItemText( hDlg, IDE_PRICE1, (LPSTR)szTemp, 32 ))
        gFilter.lPrice1 = -1;
    else
    {
        if (!GetFloatValue(szTemp,&gFilter.lPrice1))
        {
            SetFocus( GetDlgItem(hDlg,IDE_PRICE1) );
            SendDlgItemMessage(hDlg,IDE_PRICE1,EM_SETSEL,0,MAKELONG(0,-1));
            return FALSE;
        }
    }

    if (!GetDlgItemText( hDlg, IDE_PRICE2, (LPSTR)szTemp, 32 ))
        gFilter.lPrice2 = -1;
    else
    {
        if (!GetFloatValue(szTemp,&gFilter.lPrice2))
        {
            SetFocus( GetDlgItem(hDlg,IDE_PRICE2) );
            SendDlgItemMessage(hDlg,IDE_PRICE2,EM_SETSEL,0,MAKELONG(0,-1));
            return FALSE;
        }
    }

    if (!GetDlgItemText( hDlg, IDE_SIZE1, (LPSTR)szTemp, 32 ))
        gFilter.lSize1 = -1;
    else
    {
        if (!GetFloatValue(szTemp,&gFilter.lSize1))
        {
            SetFocus( GetDlgItem(hDlg,IDE_SIZE1) );
            SendDlgItemMessage(hDlg,IDE_SIZE1,EM_SETSEL,0,MAKELONG(0,-1));
            return FALSE;
        }

        //*** Convert to Kilobytes
        gFilter.lSize1 *= 1024;
        gFilter.lSize1 /= 100;
    }

    if (!GetDlgItemText( hDlg, IDE_SIZE2, (LPSTR)szTemp, 32 ))
        gFilter.lSize2 = -1;
    else
    {
        if (!GetFloatValue(szTemp,&gFilter.lSize2))
        {
            SetFocus( GetDlgItem(hDlg,IDE_SIZE2) );
            SendDlgItemMessage(hDlg,IDE_SIZE2,EM_SETSEL,0,MAKELONG(0,-1));
            return FALSE;
        }

        //*** Convert to Kilobytes
        gFilter.lSize2 *= 1024;
        gFilter.lSize2 /= 100;
    }

    GetDlgItemText( hDlg, IDE_PUB, (LPSTR)gFilter.szPubName, 32 );
    GetDlgItemText( hDlg, IDE_NAME, (LPSTR)gFilter.szFontName, 32 );
    return TRUE;

} //*** SuckFilterDlgData


//*************************************************************
//
//  GetFloatValue
//
//  Purpose:
//      Retrieves the floating point value of type (###.##)
//      returns as a long*100
//
//  Parameters:
//      LPSTR lpString
//      LPLONG lValue    
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/26/92   MSM        Created
//
//*************************************************************

BOOL GetFloatValue( LPSTR lpString, LPLONG lplValue )
{
    LONG l=0;
    LPSTR lp = lpString;

    //*** Initialize to error cond.
    *lplValue = -1;

    while (isspace(*lp))
        lp++;

    if (!isdigit(*lp))
        return FALSE;

    while (isdigit(*lp))
    {
        l *= 10;
        l += (*lp - '0');
        lp++;
    }

    if (*lp=='.')
    {
        lp++;

        //*** Do tenths
        if (!isdigit(*lp))
            return FALSE;
        l *= 10;
        l += (*lp - '0');
        lp++;

        //*** Do hundreths
        if (isdigit(*lp))
        {
            l *= 10;
            l += (*lp - '0');
            lp++;
        }
        else
            l *= 10;
    }
    else
        l *= 100;

    while (isspace(*lp))
        lp++;
    if (*lp!='\0')
        return FALSE;
    *lplValue = l;
    return TRUE;

} //*** GetFloatValue

//*** EOF: filter.c
