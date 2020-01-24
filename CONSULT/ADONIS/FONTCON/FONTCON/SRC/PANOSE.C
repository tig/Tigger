//*************************************************************
//  File name: panose.c
//
//  Description: 
//      Handles the panose DLG and panose sorting
//
//  History:    Date       Author     Comment
//               3/22/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"
#include "global.h"
#include "elsemap.h"
#include "panose.h"

PANOSE  gPanose;
WORD    gwaWeights[10];

extern ELSE_MAP_STATE gElseMapState;
PANOSE_NUM_MEM        gExpPanose;

VOID DoPanoseNumbersForNode(LPDBNODE lpdn);
void FAR PASCAL SetWeights(VOID);

//*************************************************************
//
//  PanoseDlg
//
//  Purpose:
//      Dlg proc for the panose dialog
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

BOOL CALLBACK PanoseDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP hbmPanose = NULL ;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            int i ;

            hbmPanose = LoadBitmap( ghInst, IDB_PANOSE ) ;

            DlgCenter( hDlg, GetParent( hDlg ), FALSE ) ;

            for (i=0; i<=9; i++)
            {
               SetWindowFont( GetDlgItem( hDlg, ID_FAMILY_DIGITS + i ),
                  ghfontSmall, FALSE ) ;

            }

            GetIniPanoseNumbers();
            FillPanoseDialog( hDlg );
        }
        return TRUE;


        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC         hdcMem;
            BITMAP      bm ;
            RECT        rc ;
            int         cx, cy ;

            if (!hbmPanose)
               break ;

            GetObject( hbmPanose, sizeof( BITMAP ), &bm ) ;

            GetWindowRect( GetDlgItem( hDlg, ID_XHEIGHT_WEIGHT ), &rc ) ;
            MapWindowRect( NULL, hDlg, &rc ) ;

            cy = rc.bottom - bm.bmHeight ;

            GetWindowRect( GetDlgItem( hDlg, ID_UPDATE ), &rc ) ;
            MapWindowRect( NULL, hDlg, &rc ) ;

            cx = rc.left + (rc.right - rc.left - bm.bmWidth) / 2 ;

            BeginPaint( hDlg, &ps );
            hdcMem = CreateCompatibleDC( ps.hdc );


            hbmPanose = SelectObject( hdcMem, hbmPanose ) ;

            bmTransBlt( ps.hdc, cx, cy, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0 ) ;

            hbmPanose = SelectObject( hdcMem, hbmPanose ) ;

            DeleteDC( hdcMem );

            EndPaint( hDlg, &ps );
        }
        break;

        case WM_ACTIVATE:
        {
            if (wParam==WA_INACTIVE)
                SuckPanoseDlgData( hDlg );
        }
        break;

        case WM_COMMAND:
        {
            switch (wParam)
            {
                case ID_FAMILY_WEIGHT:
                case ID_SERIF_WEIGHT:   
                case ID_WEIGHT_WEIGHT:  
                case ID_PROP_WEIGHT:    
                case ID_CONTRAST_WEIGHT:
                case ID_STROKE_WEIGHT:  
                case ID_ARMSTYLE_WEIGHT:
                case ID_LTRFORM_WEIGHT: 
                case ID_MIDLINE_WEIGHT: 
                case ID_XHEIGHT_WEIGHT: 
                {
                    if (HIWORD(lParam)==EN_CHANGE)
                    {
                        UINT uI;
                        BOOL bTrans;

                        if (SendDlgItemMessage(hDlg,wParam,WM_GETTEXTLENGTH,0,0L)==0L)
                            return TRUE;

                        uI = GetDlgItemInt( hDlg, wParam, &bTrans, FALSE );

                        if (!bTrans)
                        {
                            MessageBeep(0);
                            SetDlgItemInt( hDlg, wParam, 0, FALSE );
                        }
                    }
                }
                break;

                case IDCANCEL:
                    DestroyWindow( hDlg );
                    return TRUE;
                break;

                case IDOK:
                case ID_UPDATE:
                {
                    SuckPanoseDlgData( hDlg );
                    SetIniPanoseNumbers();
                    DoPanoseNumbers();
                    SetDBELBToSort( ghwndELB, DB_SORT_PANOSE );
                    UpdateSortingButtons();
                    if (wParam==IDOK)
                    {
                        DestroyWindow( hDlg );
                        return TRUE;
                    }
                }
                break;

                case IDB_HELP:
                    return ViewFontshopHelp( IDS_HELP_PANOSE );
                break;                                  
            }
        }
        break;

        case WM_DESTROY:
            if (hbmPanose)
               DeleteObject( hbmPanose ) ;

            ghwndPanoseDlg = NULL;
        break;
    }
    return FALSE;

} //*** PanoseDlg


//*************************************************************
//
//  FillPanoseDialog
//
//  Purpose:
//      Fills the panose dialog controls
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
//               3/22/92   MSM        Created
//
//*************************************************************

VOID FillPanoseDialog( HWND hDlg )
{
    int i,x;
    PBYTE pByte = (PBYTE)&gPanose;
    WORD wIDS;

    //*** Fill weights
    for (i=1; i<=9; i++)
        SetDlgItemInt( hDlg, ID_FAMILY_WEIGHT+i, gwaWeights[i], FALSE );

    //*** Fill digits and set selection
    for (i=0; i<=9; i++)
    {
        HWND hCombo = GetDlgItem( hDlg, ID_FAMILY_DIGITS + i );

        ComboBox_ResetContent( hCombo );

        wIDS = IDS_FAMILY_BASE + (i*16);

        for (x=0; x<=15; x++)
        {
            LPSTR lp = GRCS( wIDS+x );

            if (*lp)
                ComboBox_InsertString( hCombo, -1, lp );
            else
                break;
        }
        ComboBox_SetCurSel( hCombo, (int)pByte[i] );
    }

} //*** FillPanoseDialog


//*************************************************************
//
//  SuckPanoseDlgData
//
//  Purpose:
//      Sucks the information out of the panose dlg
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
//               3/22/92   MSM        Created
//
//*************************************************************

VOID SuckPanoseDlgData( HWND hDlg )
{
    int i;
    PBYTE pByte = (PBYTE)&gPanose;

    //*** Get weights
    for (i=1; i<=9; i++)
        gwaWeights[i] = GetDlgItemInt(hDlg, ID_FAMILY_WEIGHT+i, NULL, FALSE);

    //*** get digit selection
    for (i=0; i<=9; i++)
        pByte[i] = (BYTE)ComboBox_GetCurSel(GetDlgItem(hDlg,ID_FAMILY_DIGITS+i));

} //*** SuckPanoseDlgData


//*************************************************************
//
//  DoPanoseNumbers
//
//  Purpose:
//      Walks the database and calculates the panose numbers
//
//
//  Parameters:
//      
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/23/92   MSM        Created
//
//*************************************************************

VOID DoPanoseNumbers()
{
    LPPUB lpPub = GetFirstPublisher( glpDB, NULL );

    SetWeights();

    //*** Converts the PANOSE number to an expanded Panose
    iELSECvt10DigitToExpIndirect( &gExpPanose, sizeof(gExpPanose),
        PANOSE_ANY, (LPEBYTE)&gPanose );

    while (lpPub)
    {
        DoPanoseNumbersForNode( (LPDBNODE)lpPub );
        lpPub = GetNextPublisher( lpPub, NULL );
    }

} //*** DoPanoseNumbers


//*************************************************************
//
//  DoPanoseNumbersForNode
//
//  Purpose:
//      Handles the recursion in walking the DB
//
//
//  Parameters:
//      LPDBNODE lpdn 
//      
//
//  Return: (VOID)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/23/92   MSM        Created
//
//*************************************************************

VOID DoPanoseNumbersForNode(LPDBNODE lpdn)
{
    if (lpdn->wNodeType == NT_STYLE)
    {
        PANOSE_NUM_MEM pnmExpPanose;
        LPSTYLE lpS = (LPSTYLE)lpdn;

        //*** Converts the PANOSE number to an expanded Panose
        iELSECvt10DigitToExpIndirect( &pnmExpPanose, sizeof(pnmExpPanose),
            PANOSE_ANY, (LPEBYTE)&(lpS->Info.Panose) );

        lpS->dbn.wPANOSEInfo = uiELSEMatchFonts( &gElseMapState,
            &gExpPanose, &pnmExpPanose );
    }
    else
    {
        LPDBNODE lpdn2 = GetFirstChildDBNode( lpdn, NULL );

        while (lpdn2)
        {
            DoPanoseNumbersForNode( lpdn2 );
            lpdn2 = GetNextDBNode( lpdn2, NULL );
        }
    }

} //*** DoPanoseNumbersForNode


//*************************************************************
//
//  SetWeights
//
//  Purpose:
//      Sets the weights for the font mapper
//
//
//  Parameters:
//      
//      
//
//  Return: (void FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/23/92   MSM        Created
//
//*************************************************************

void FAR PASCAL SetWeights()
{
  UINT i;
  LPEBYTE lpjWeights;
  LPPANOSE_PDICT_MEM lpPHead;
  LPPANOSE_PINDEX_MEM ind;

  /* Lock down the resource header.
      */
  if (lpPHead = lpELSEResLockHead(&gElseMapState.resState)) 
  {
    if (lpPHead->uiVersion == PANOSE_PENALTY_VERS) 
    {
      /* Search for the Latin script.
      */
      for (i = 0, ind = lpPHead->pindex; i < lpPHead->uiNumDicts;++i,++ind) 
      {
        if (ind->uiScriptA == FAM_SCRIPT_LATIN &&
        ind->uiScriptB == FAM_SCRIPT_LATIN &&
        ind->uiGenreA == FAM_GENRE_TEXTANDDISP &&
        ind->uiGenreB == FAM_GENRE_TEXTANDDISP) 
        {
          /* Get weights array.
              */
          if (lpjWeights = lpELSEResLockPWeightTbl(
              &gElseMapState.resState, ind, ind->uiOffsWeights,
              ind->uiNumAttr * sizeof(BYTE))) 
          {
            for (i = 0; i < 10; i++)
            {
              lpjWeights[i] = (BYTE)gwaWeights[i];
              switch (i)
              {
                case PANOSE_IND_FAMILY:
                  /* No family weight in Expanded PANOSE -- family causes a
                  * completely different set of penalty tables to be used.
                  */
                break;

                case PANOSE_IND_SERIF:
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_SERIF] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_WEIGHT:
                  lpjWeights[PAN_IND_SHARED_WEIGHT] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_PROPORTION:
                  lpjWeights[PAN_IND_SHARED_DISTORT] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_IND_SHARED_MONOSPC] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_RATIO] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_CONTRAST:
                  lpjWeights[PAN_IND_SHARED_CONTRST] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_STROKE:
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_SPEED] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_STRESS] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_ARMSTYLE:
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_ARMTYPE] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_CTERM] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_LTRFORM:
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_SLANT] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_ROUND] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_SYMMETRY] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_MIDLINE:
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_MIDLNPOS] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_APEX] = (BYTE)gwaWeights[i];
                break;

                case PANOSE_IND_XHEIGHT:
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_XHEIGHT] = (BYTE)gwaWeights[i];
                  lpjWeights[PAN_COUNT_SHARED + PAN_IND_LATIN_DIACRIT] = (BYTE)gwaWeights[i];
                break;
              } // Switch
            } // For
            vELSEResUnlockPWeightTbl(&gElseMapState->resState,
                lpjWeights);
          }
          break;
        }
      }
    }
    vELSEResUnlockHead(&gElseMapState->resState, lpPHead);
  }

} //*** SetWeights

//*** EOF: panose.c
