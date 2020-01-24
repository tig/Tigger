//*************************************************************
//  File name: coll.c
//
//  Description: 
//      Handles the collections stuff
//
//  History:    Date       Author     Comment
//               3/27/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"

ELBPAINTPROC glpfnPaint = NULL;
ELBSORTPROC  glpfnSort = NULL;

LPPUB PreCollectionStartup(VOID);


//*************************************************************
//
//  CollectionsProc
//
//  Purpose:
//      Dialog proc for collections
//
//
//  Parameters:
//      HWND hDlg
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/27/92   MSM        Created
//
//*************************************************************

BOOL FAR PASCAL CollectionsProc(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
    static LPPUB  lpPub=NULL;
    static LPCOLL lpLColl=NULL;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            HWND hWnd;
            LPCOLL lpColl;

            if (!(lpPub=PreCollectionStartup()))
            {
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
            SetWindowText( hDlg, lpPub->lpszPublisher );

            GetCustInfo( &gCustInfo, GRCS(IDS_APPNAME) ) ;

            if (!glpfnPaint)
                glpfnPaint = (ELBPAINTPROC)MakeProcInstance( 
                    (FARPROC)ELBPaintProc, ghInst );

            if (!glpfnSort)
                glpfnSort = (ELBSORTPROC)MakeProcInstance( 
                    (FARPROC)ELBSortProc, ghInst );
                    
            hWnd = GetDlgItem( hDlg, IDL_COLLECTIONS );
            ELBSetPaintProc( hWnd, glpfnPaint );
            ELBSetSortProc( hWnd, glpfnSort );
            ELBSetDatabase( hWnd, glpDB );
            ELBSetFont( hWnd, ghfontSmall );

            lpColl = GetFirstCollection( lpPub );

            SetWindowRedraw( hWnd, 0 );
            while (lpColl)
            {
                ELBInsertNode( hWnd, (UINT)-1, (LPELBNODE)lpColl );
                lpColl = GetNextCollection( lpColl );
            }
            SetWindowRedraw( hWnd, 1 );

            lpLColl = NULL;
            EnableWindow( GetDlgItem(hDlg, IDB_12DAY), FALSE );
            EnableWindow( GetDlgItem(hDlg, IDB_23DAY), FALSE );
            EnableWindow( GetDlgItem(hDlg, IDB_UPS), FALSE );


            SetWindowFont( GetDlgItem( hDlg, IDE_DESC ), ghfontSmall, FALSE ) ;
            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

            return TRUE;
        }
        break;

        case WM_COMMAND:
        {
            switch (wParam)
            {
                case IDCANCEL:
                case IDOK:
                    EndDialog(hDlg, TRUE);
                    lpLColl = GetFirstCollection( lpPub );
                    while (lpLColl)
                    {
                        if (VI_ISCHECKED(lpLColl))
                        {
                            FreePublisherCollectionsText( lpPub );
                            return TRUE;
                        }
                        lpLColl = GetNextCollection( lpLColl );
                    }
                    FreePublisherCollections( lpPub );
                    return TRUE;
                break;

                case IDB_HELP:
                    return ViewFontshopHelp( IDS_HELP_COLLECTION );
                break;

                case IDB_12DAY:
                case IDB_23DAY:
                case IDB_UPS:
                {
                    HWND hWnd = GetDlgItem( hDlg, IDL_COLLECTIONS );
                    LPCOLL lpColl = (LPCOLL)ELBGetCurSel(hWnd, NULL );
                    int i = (int)SendMessage((HWND)LOWORD(lParam),BM_GETCHECK,0,0L);
                    if (lpColl && i==1)
                    {
                        lpColl->wShipping = wParam-IDB_12DAY;
                        if (lpColl->wShipping == gCustInfo.uiShipMethod)
                            lpColl->wShipping = (WORD)-1;
                    }
                }
                break;

                case IDL_COLLECTIONS:
                {
                    HWND hWnd = (HWND)LOWORD(lParam);
                    LPCOLL lpColl;
                    HWND hEdit = NULL;

                    if (HIWORD(lParam)==ELBN_SPACE)
                    {
                        int       nIndex;
                        LPELBNODE lpen = (LPELBNODE)ELBGetCurSel(hWnd, (LPINT)&nIndex );
                        RECT rc, rc2;
                        HWND hLB = GetWindow( hWnd, GW_CHILD );
                        DWORD dwP;
                        LPDB lpdb = ELBGetDatabase( hWnd );

                        if (!lpen)
                            return TRUE;

                        switch (NODETYPE(lpen))
                        {
                            case NT_COLLECTION:
                                dwP = ((LPCOLL)lpen)->Info.dwPrice;
                            break;

                            default:
                                dwP = 0L;
                            break;
                        }


                        if (VI_ISCHECKED(lpen))
                        {
                            VI_UNCHECKNODE(lpen);
                            lpdb->dwTotal -= dwP;
                            lpdb->wColls--;
                        }
                        else
                        {
                            VI_CHECKNODE(lpen);
                            lpdb->dwTotal += dwP;
                            lpdb->wColls++;
                        }

                        SendMessage( hLB, LB_GETITEMRECT, nIndex, (LONG)(LPRECT)&rc );
                        GetClientRect( hLB, &rc2 );
                        rc.bottom = rc2.bottom;
                        InvalidateRect( hLB, &rc, FALSE );
                        UpdateWindow( hLB );
                        UpdateStatLine( ghwndStat, glpDB );
                        UpdateToolbar( ghwndToolbar, glpDB );
                        
                        lpColl = (LPCOLL)lpen;
                        if (lpColl!=lpLColl)
                            hEdit  = GetDlgItem( hDlg, IDE_DESC );
                        lpLColl = NULL;
                        goto set_shipping;
                    }

                    if (HIWORD(lParam)==ELBN_SELCHANGE)
                    {
                        lpColl = (LPCOLL)ELBGetCurSel(hWnd, NULL );
                        if (lpColl!=lpLColl)
                        {
                            hEdit  = GetDlgItem( hDlg, IDE_DESC );
set_shipping:
                            lpLColl = lpColl;
                            if (lpColl)
                            {
                                int i = (int)lpColl->wShipping;

                                if (IsWindow(hEdit))
                                    Edit_SetText( hEdit, (LPSTR)lpColl->lpszText );

                                if (!VI_ISCHECKED(lpColl))
                                {
                                    SendDlgItemMessage(hDlg,IDB_12DAY,BM_SETCHECK,0,0L);
                                    SendDlgItemMessage(hDlg,IDB_23DAY,BM_SETCHECK,0,0L);
                                    SendDlgItemMessage(hDlg,IDB_UPS,BM_SETCHECK,0,0L);

                                    EnableWindow( GetDlgItem(hDlg, IDB_12DAY), FALSE );
                                    EnableWindow( GetDlgItem(hDlg, IDB_23DAY), FALSE );
                                    EnableWindow( GetDlgItem(hDlg, IDB_UPS), FALSE );
                                    return TRUE;
                                }
                                else
                                {
                                    EnableWindow( GetDlgItem(hDlg, IDB_12DAY), (lpColl->Info.rgShippingRates[0]!=(DWORD)-1));
                                    EnableWindow( GetDlgItem(hDlg, IDB_23DAY), (lpColl->Info.rgShippingRates[1]!=(DWORD)-1));
                                    EnableWindow( GetDlgItem(hDlg, IDB_UPS), (lpColl->Info.rgShippingRates[2]!=(DWORD)-1));
                                }

                                if (i==-1)
                                {
                                    i= (int)gCustInfo.uiShipMethod;
                                    if (lpColl->Info.rgShippingRates[i]==(DWORD)-1)
                                    {
                                        //*** Pick new default
                                        for (i=0; i<3; i++)
                                        {
                                            if (lpColl->Info.rgShippingRates[i]!=(DWORD)-1)
                                                break;
                                        }
                                    }
                                }

                                SendDlgItemMessage(hDlg,IDB_12DAY+i,BM_SETCHECK,1,0L);
                                switch (i)
                                {
                                    case 0:
                                        SendDlgItemMessage(hDlg,IDB_23DAY,BM_SETCHECK,0,0L);
                                        SendDlgItemMessage(hDlg,IDB_UPS,BM_SETCHECK,0,0L);
                                    break;

                                    case 1:
                                        SendDlgItemMessage(hDlg,IDB_12DAY,BM_SETCHECK,0,0L);
                                        SendDlgItemMessage(hDlg,IDB_UPS,BM_SETCHECK,0,0L);
                                    break;

                                    case 2:
                                        SendDlgItemMessage(hDlg,IDB_12DAY,BM_SETCHECK,0,0L);
                                        SendDlgItemMessage(hDlg,IDB_23DAY,BM_SETCHECK,0,0L);
                                    break;

                                }
                            }
                        }
                        return TRUE;
                    }
                    break;
                }
                break;
            }
        }
        break;
    }
    return FALSE;

} //*** CollectionsProc


//*************************************************************
//
//  PreCollectionStartup
//
//  Purpose:
//      Attempts to load the collections determine if there is collection
//
//
//  Parameters:
//      
//      
//
//  Return: (BOOL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/27/92   MSM        Created
//
//*************************************************************

LPPUB PreCollectionStartup()
{
    LPPUB lpPub = (LPPUB)ELBGetCurSel(ghwndELB,NULL);

    //*** Walk to the top
    while (lpPub && NODETYPE(lpPub)!=NT_PUB)
        lpPub = PARENT(LPPUB,lpPub);

    if (!lpPub || lpPub->PubInfo.wCollections==0)
        return NULL;

    if (!LoadPublisherCollections(lpPub))
        return NULL;

    if (!LoadPublisherCollectionsText(lpPub))
    {
        ReportFontshopError( NULL, (LPSTR)"The Font Connection experienced an error while loading the collection descriptions.  If this problem persists, contact Technical Support.",
            GRCS(IDS_APPNAME), MB_ICONSTOP|MB_OK|MB_TECHNUMBER, 0 );
        return NULL;
    }
    return lpPub;

} //*** PreCollectionStartup


//*************************************************************
//
//  ELBPaintProc
//
//  Purpose:
//      Paints the node in the ELB
//
//
//  Parameters:
//      HWND hELB
//      HDC hDC
//      LPELBNODE lpen
//      LPRECT lprc
//      
//
//  Return: (LONG CALLBACK)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/26/92   MSM        Created
//
//*************************************************************

LONG CALLBACK ELBPaintProc(LPES lpes,HDC hDC,LPELBNODE lpen,LPRECT lprc)
{
    RECT   rc = *lprc;
    UINT   dy ;
    char   szTemp[20];
    DWORD  dwPrice;
    LPCOLL lpColl = (LPCOLL)lpen;

    dwPrice = lpColl->Info.dwPrice;

    dy = rc.top ;
    if (lpes->wFontH < lpes->wBmpH)
        dy += (lpes->wBmpH - lpes->wFontH)/2;

    wsprintf( szTemp, "$%ld.%02ld", dwPrice/100L, dwPrice%100L );

    ExtTextOut( hDC, rc.left, dy, ETO_CLIPPED|ETO_OPAQUE, &rc,
        szTemp, lstrlen(szTemp), NULL );

    rc.left += LOWORD(GetTextExtent(hDC, "$5678.90    ", 12 ));

    ExtTextOut( hDC, rc.left, dy, ETO_CLIPPED|ETO_OPAQUE, &rc,
        lpColl->Info.szName, lstrlen(lpColl->Info.szName), NULL );

    return 1L;

} //*** ELBPaintProc


//*************************************************************
//
//  ELBSortProc
//
//  Purpose:
//      This routine is responsible for sorting the DBNODEs
//
//
//  Parameters:
//      HWND hELB
//      LPELBNODE lpen1
//      LPELBNODE lpen2
//      
//
//  Return: (LONG CALLBACK)
//      returns the same as a lstrcpy type function
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/26/92   MSM        Created
//             
//*************************************************************

LONG CALLBACK ELBSortProc(HWND hELB,LPELBNODE lpen1, LPELBNODE lpen2)
{
    LPSTR lpName1, lpName2;

    lpName1 = ((LPCOLL)lpen1)->Info.szName;
    lpName2 = ((LPCOLL)lpen2)->Info.szName;

    return (LONG)_fstricmp(lpName1, lpName2);

} //*** ELBSortProc

//*** EOF: coll.c
