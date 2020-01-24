//*************************************************************
//  File name: dbelb.c
//
//  Description: 
//      Code for the DB overlay of the Extended ListBox
//
//  History:    Date       Author     Comment
//               2/23/92   MSM        Created
//
//*************************************************************

#include "PRECOMP.H"

#include "intFCDB.h"

static char szDBELBBuff[120];
static char szDBELBBuff2[120];

//*************************************************************
//
//  FillDBELB
//
//  Purpose:
//      Fills the Extended Listbox with the database
//
//
//  Parameters:
//      HWND hELB
//      LPDB lpDB
//      
//
//  Return: (UINT WINAPI)
//      The actual number of nodes inserted
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

UINT WINAPI FillDBELB( HWND hELB )
{
    LPDB lpDB = ELBGetDatabase( hELB );
    LPELBNODE lpn = ELBGetCurSel( hELB, NULL );

    ELBResetContent( hELB );
    InsertChildrenInto( hELB, lpDB, (LPDBNODE)lpDB, 0xFFFF, 0 );
    SelectByNode( hELB, lpn );

    return TRUE;

} //*** FillDBELB


//*************************************************************
//
//  SetDBELBToSort
//
//  Purpose:
//      Sets the view in the box to the specified sorting method
//
//
//  Parameters:
//      HWND hELB
//      UINT uSort
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 9/92   MSM        Created
//
//*************************************************************

BOOL WINAPI SetDBELBToSort( HWND hELB, UINT uSort)
{
    LPDB lpDB = ELBGetDatabase( hELB );
    LPELBNODE lpn = ELBGetCurSel( hELB, NULL );    

    lpDB->uSort = uSort;
    ELBResetContent( hELB );
    InsertChildrenInto( hELB, lpDB, (LPDBNODE)lpDB, 0xFFFF, 0 );
    SelectByNode( hELB, lpn );
    return TRUE;

} //*** SetDBELBToSort


//*************************************************************
//
//  SetDBELBToView
//
//  Purpose:
//      Sets the view in the box to the specified type
//
//
//  Parameters:
//      HWND hELB
//      UINT uView
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 9/92   MSM        Created
//
//*************************************************************

BOOL WINAPI SetDBELBToView( HWND hELB, UINT uView)
{
    LPDB lpDB = ELBGetDatabase( hELB );
    LPELBNODE lpn = ELBGetCurSel( hELB, NULL );    

    //*** This is because we removed the family group part way
    //*** into the development cycle.  Thanks GERRY!
    uView &= ~DB_VIEW_FAMILIES;

    lpDB->uView = uView;
    ELBResetContent( hELB );
    InsertChildrenInto( hELB, lpDB, (LPDBNODE)lpDB, 0xFFFF, 0 );
    SelectByNode( hELB, lpn );
    return TRUE;

} //*** SetDBELBToView


//*************************************************************
//
//  SetDBELBToFilter
//
//  Purpose:
//      Sets the filter in the box to the specified filter
//
//
//  Parameters:
//      HWND hELB
//      UINT uFilter
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 9/92   MSM        Created
//
//*************************************************************

BOOL WINAPI SetDBELBToFilter( HWND hELB, HFILTER hFilter)
{
    LPDB lpDB = ELBGetDatabase( hELB );
    LPELBNODE lpn = ELBGetCurSel( hELB, NULL );

    lpDB->hFilter = hFilter;
    ELBResetContent( hELB );
    InsertChildrenInto( hELB, lpDB, (LPDBNODE)lpDB, 0xFFFF, 0 );
    SelectByNode( hELB, lpn );
    return TRUE;

} //*** SetDBELBToFilter


//*************************************************************
//
//  SimulateInsertChildrenInto
//
//  Purpose:
//      Checks to see if InsertChildren should be called or not
//
//
//  Parameters:
//      LPDB lpDB       - database
//      LPDBNODE lpdn   - database node
//      
//
//  Return: (BOOL NEAR PASCAL)
//      Number of nodes added, 0 if none, -1 if error
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

BOOL NEAR PASCAL SimulateInsertChildrenInto( LPDB lpDB, LPDBNODE lpdn )
{
    LPDBNODE lpcn;

    if (!lpDB || !lpdn)
        return FALSE;

    if (!lpDB->hFilter || NODETYPE(lpdn)==NT_STYLE)
        return TRUE;

    lpcn = GetFirstChildDBNode( lpdn, lpDB->hFilter );

    while (lpcn)
    {
        if (NODETYPE(lpcn)==NT_STYLE)
            return TRUE;
        if (SimulateInsertChildrenInto( lpDB, lpcn))
            return TRUE;
        lpcn = GetNextDBNode( lpcn, lpDB->hFilter );
    }
    return FALSE;

} //*** SimulateInsertChildrenInto


//*************************************************************
//
//  InsertChildrenInto
//
//  Purpose:
//      Inserts all the children of the node at uIndex after uIndex
//
//
//  Parameters:
//      HWND hELB       - Extended listbox
//      LPDB lpDB       - database
//      LPDBNODE lpdn   - database node
//      UINT uIndex     - -1 for the root, otherwise uIndex [==] lpdn
//      UINT uExp       - Expand all children flag                         
//      
//
//  Return: (UINT NEAR PASCAL)
//      Number of nodes added, 0 if none, -1 if error
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

UINT NEAR PASCAL InsertChildrenInto( HWND hELB, LPDB lpDB, LPDBNODE lpdn, UINT uIndex, UINT uExp )
{
static int   iRecurse = 0;    
    UINT     uNodes=0;
    LPDBNODE lpcn;

    if (!lpDB || !lpdn)
        return 0;

    if (iRecurse==0)
    {
        if (lpDB->hFilter && !SimulateInsertChildrenInto( lpDB, lpdn ))
            return 0;
        SetCursor( LoadCursor(NULL,IDC_WAIT) );
        SendMessage( hELB, WM_SETREDRAW, 0, 0L );
    }

    iRecurse++;

    lpcn = GetFirstChildDBNode( lpdn, lpDB->hFilter );

    while (lpcn)
    {
        int nCIndex;
        BOOL bNodeInView;

        //*** Simulate the insert to see if any nodes pass the filter
        if (!lpDB->hFilter || SimulateInsertChildrenInto( lpDB, lpcn ))
        {
            //*** Force the child to open if requested to
            if (uExp)
                VI_OPENNODE( lpcn );

            if (lpcn->wNodeType==NT_PUB)
                bNodeInView = lpDB->uView & DB_VIEW_PUBLISHERS;
            else
                bNodeInView = (UINT)(lpcn->wNodeType<<1) & lpDB->uView;

            // Add the child
            if ( bNodeInView )
            {
                nCIndex = (int)ELBAddInto( hELB, uIndex, (LPELBNODE)lpcn );
                if (nCIndex<0)
                    break;
            }
            else
                nCIndex = uIndex;

            // If child is open or we didn't add the child, then recurse down
            if (  !bNodeInView || VI_GETOPENSTATUS(lpcn) )
                uNodes += InsertChildrenInto( hELB, lpDB, lpcn, nCIndex, uExp );
            else
                uNodes++;
        }
        lpcn = GetNextDBNode( lpcn, lpDB->hFilter );
    }

    iRecurse--;
    if (!iRecurse)
    {
        SendMessage( hELB, WM_SETREDRAW, 1, 0L );
        InvalidateRect( hELB, NULL, FALSE );

        SetCursor( LoadCursor(NULL,IDC_ARROW) );
    }

    return uNodes;

} //*** InsertChildrenInto


//*************************************************************
//
//  ExpandDBELBNode
//
//  Purpose:
//      Expands an ELB node
//
//
//  Parameters:
//      HWND hELB
//      LPDB lpDB
//      UINT uIndex
//      UINT uExp
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

BOOL WINAPI ExpandDBELBNode( HWND hELB, UINT uIndex, UINT uExp )
{
    LPDBNODE lpdn = (LPDBNODE)ELBGetItem( hELB, uIndex );

    if (!lpdn || !VI_EXPANDABLE(lpdn))
        return FALSE;

    VI_OPENNODE( lpdn );
    return (InsertChildrenInto( hELB, ELBGetDatabase(hELB), lpdn, uIndex, uExp )!=-1);

} //*** ExpandDBELBNode


//*************************************************************
//
//  CollapseDBELBNode
//
//  Purpose:
//      Collapses a node
//
//
//  Parameters:
//      HWND hELB
//      LPDB lpDB
//      UINT uIndex
//      UINT uAll   - close all children flag
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/24/92   MSM        Created
//
//*************************************************************

BOOL WINAPI CollapseDBELBNode( HWND hELB, UINT uIndex, UINT uAll )
{
    return (BOOL)ELBCloseNode( hELB, uIndex, uAll );

} //*** CollapseDBELBNode


//*************************************************************
//
//  SyncDBELBToSlider
//
//  Purpose:
//      Syncs the view of the slider up with that of the ELB 
//
//
//  Parameters:
//      LPDB lpDB
//      HWND hELB
//      HWND hSlider
//      
//
//  Return: (VOID WINAPI)
//
//
//  Comments:
//      All the rc2.left/right stuff is just to avoid the flicker
//
//  History:    Date       Author     Comment
//               2/26/92   MSM        Created
//
//*************************************************************

VOID WINAPI SyncDBELBToSlider( HWND hELB, HWND hSlider)
{
    LONG lPos;
    RECT rc, rc2;
    UINT uCol;
    LPDB lpDB = ELBGetDatabase( hELB );
    HWND hwndLB ;
    PES pes = (PES)GetWindowWord(hELB, WE_PES);

    if (!lpDB)
        return;

    hwndLB = pes->hLB ;

    GetClientRect( hSlider, &rc );
    GetClientRect( hwndLB, &rc2 );

    // This comes back as a percentage
    lPos = SendMessage( hSlider, SL_GETCOLPOS, PUB_COL, 0L );
    uCol = (UINT)((lPos * (LONG)rc.right)/100L);
    rc2.left = min(lpDB->uPubCol,uCol);
    if (uCol!= lpDB->uPubCol)
    {
        lpDB->uPubCol = uCol;
        InvalidateRect( hwndLB, &rc2, FALSE );
    }

    lPos = SendMessage( hSlider, SL_GETCOLPOS, SIZE_COL, 0L );
    uCol = (UINT)((lPos * (LONG)rc.right)/100L);
    rc2.right = rc2.left;
    rc2.left = min(lpDB->uSizeCol,uCol);
    if (uCol!= lpDB->uSizeCol)
    {
        lpDB->uSizeCol = uCol;
        InvalidateRect( hwndLB, &rc2, FALSE );
    }

    lPos = SendMessage( hSlider, SL_GETCOLPOS, PRICE_COL, 0L );
    uCol = (UINT)((lPos * (LONG)rc.right)/100L);
    rc2.right = rc2.left;
    rc2.left = min(lpDB->uPriceCol,uCol);
    if (uCol!= lpDB->uPriceCol)
    {
        lpDB->uPriceCol = uCol;
        InvalidateRect( hwndLB, &rc2, FALSE );
    }

    UpdateWindow( hwndLB );

} //*** SyncDBELBToSlider


//*************************************************************
//
//  DBELBPaintProc
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

LONG CALLBACK DBELBPaintProc(LPES lpes,HDC hDC,LPELBNODE lpen,LPRECT lprc)
{
    LPSTR lpName;
    DWORD dwSize=0xFFFFFFFF, dwPrice=0xFFFFFFFF;
    char  szPrice[15], szSize[15];
    RECT  rc = *lprc;
    LPDB  lpDB = ELBGetDatabase( lpes->hELB );
    UINT  dy ;

    dy = rc.top ;
    if (lpes->wFontH < lpes->wBmpH)
        dy += (lpes->wBmpH - lpes->wFontH)/2;

    if ( lpen->wNodeType == NT_PUB )
        lpName = ((LPPUB)lpen)->lpszPublisher;
    else
    {
        if ( lpen->wNodeType == NT_STYLE )
        {
            LPFACE lpFace = (LPFACE)((LPDBNODE)lpen)->lpParent;

            if (lpDB->uSort == DB_SORT_PANOSE)
                wsprintf( szDBELBBuff, "%u - %s %s", lpen->wPANOSEInfo, 
                    (LPSTR)lpFace->Info.szName, 
                    (LPSTR)((LPSTYLE)lpen)->Info.szName );
            else
                wsprintf( szDBELBBuff, "%s %s", (LPSTR)lpFace->Info.szName, 
                    (LPSTR)((LPSTYLE)lpen)->Info.szName );

            lpName = (LPSTR)szDBELBBuff;
            dwSize = ((LPSTYLE)lpen)->Info.dwSize;
            dwPrice= ((LPSTYLE)lpen)->Info.dwPrice;
        }
        else
        {
            lpName = (LPSTR)((LPFACE)lpen)->Info.szName;
            dwSize = ((LPFACE)lpen)->Info.dwSize;
            dwPrice= ((LPFACE)lpen)->Info.dwPrice;
        }
    }

    if (dwSize==-1)
        szSize[0] = '\0';
    else
        wsprintf( szSize, "%ld.%02ldK", dwSize/1024L, ((dwSize%1024L)*100)/1024 );

// BUGFIX #0004 5/23/92
// BUGFIX #0005 5/23/92
    if (dwPrice==-1)
        szPrice[0] = '\0';
    else
    {
        if (dwPrice==0)
            lstrcpy(szPrice, "Free");
        else
            wsprintf( szPrice, "$%ld.%02ld", dwPrice/100L, dwPrice%100L );
    }
// BUGFIX #0004 5/23/92
// BUGFIX #0005 5/23/92

    // Set up column and print name
    rc.right = lpDB->uPriceCol;
    ExtTextOut( hDC, rc.left, dy, ETO_CLIPPED|ETO_OPAQUE, &rc,
        lpName, lstrlen(lpName), NULL );


    // Set up column and print price
    if ( (rc.left = rc.right) > lprc->right )
        return 1L;
    rc.right = lpDB->uSizeCol;
    ExtTextOut( hDC, rc.left, dy, ETO_CLIPPED|ETO_OPAQUE, &rc,
        szPrice, lstrlen(szPrice), NULL );


    // Set up column and print size
    if ( (rc.left = rc.right) > lprc->right )
        return 1L;
    rc.right = lpDB->uPubCol;
    ExtTextOut( hDC, rc.left, dy, ETO_CLIPPED|ETO_OPAQUE, &rc,
        szSize, lstrlen(szSize), NULL );


    // Set up column and print publisher
    if ( (rc.left = rc.right) > lprc->right )
        return 1L;
    rc.right = lprc->right;
    if (lpen->wNodeType != NT_PUB)
    {
        while (lpen->wNodeType != NT_PUB)
            lpen = (LPELBNODE)( ((LPDBNODE)lpen)->lpParent );

        lpName = ((LPPUB)lpen)->lpszPublisher;
    }
    else
        lpName = (LPSTR)szSize;  // szSize is already just '\0'

    ExtTextOut( hDC, rc.left, dy, ETO_CLIPPED|ETO_OPAQUE, &rc,
        lpName, lstrlen(lpName), NULL );

    return 1L;

} //*** DBELBPaintProc

//*************************************************************
//
//  DBELBSortProc
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

LONG CALLBACK DBELBSortProc(HWND hELB,LPELBNODE lpen1, LPELBNODE lpen2)
{
    LPDB  lpDB   = ELBGetDatabase( hELB );
    UINT  uSort  = lpDB->uSort;
    UINT  wNodeType = lpen1->wNodeType;

//*** ASSUME database is sorted by name so lpen1 is always 
//*** alphabetically <= lpen2

    if (wNodeType==NT_PUB)
        return -1L;

    switch (uSort)
    {
sort_name:
        case DB_SORT_NAME:
            if(wNodeType==NT_STYLE)
            {
                LPSTR  lpS1, lpS2;
                LPFACE lpFace1 = (LPFACE)((LPDBNODE)lpen1)->lpParent;
                LPFACE lpFace2 = (LPFACE)((LPDBNODE)lpen2)->lpParent;

                if (lpFace1 != lpFace2)
                {
                    lpS1 = (LPSTR)lpFace1->Info.szName;
                    lpS2 = (LPSTR)lpFace2->Info.szName;

                    while (toupper(*lpS1)==toupper(*lpS2))
                    {
                        if (!*lpS1)
                            return -1L;
                        lpS1++,lpS2++;
                    }
                    return (toupper(*lpS1)-toupper(*lpS2));
                }

                lpS1 = ((LPSTYLE)lpen1)->Info.szName;
                lpS2 = ((LPSTYLE)lpen2)->Info.szName;

                while (toupper(*lpS1)==toupper(*lpS2))
                {
                    if (!*lpS1)
                        return -1L;
                    lpS1++,lpS2++;
                }
                return (toupper(*lpS1)-toupper(*lpS2));
            }
            return -1L;
        break;

        case DB_SORT_SIZE:
        {
            DWORD  dwSize1, dwSize2;

            if (wNodeType == NT_STYLE )
            {
                dwSize1 = ((LPSTYLE)lpen1)->Info.dwSize;
                dwSize2 = ((LPSTYLE)lpen2)->Info.dwSize;
                if (dwSize1==dwSize2)
                    goto sort_name;
            }
            else
            {
                dwSize1 = ((LPFACE)lpen1)->Info.dwSize;
                dwSize2 = ((LPFACE)lpen2)->Info.dwSize;
            }

            return (dwSize1<=dwSize2)?-1L:1L;
        }
        break;

        case DB_SORT_PRICE:
        {
            DWORD  dwPrice1, dwPrice2;

            if (wNodeType == NT_STYLE )
            {
                dwPrice1 = ((LPSTYLE)lpen1)->Info.dwPrice;
                dwPrice2 = ((LPSTYLE)lpen2)->Info.dwPrice;
                if (dwPrice1==dwPrice2)
                    goto sort_name;
            }
            else
            {
                dwPrice1 = ((LPFACE)lpen1)->Info.dwPrice;
                dwPrice2 = ((LPFACE)lpen2)->Info.dwPrice;
            }
            return (dwPrice1<=dwPrice2)?-1L:1L;
        }
        break;

        case DB_SORT_PANOSE:
        {
            WORD wPan1, wPan2;

            if (wNodeType==NT_FACE)
                return -1L;
                
            wPan1 = lpen1->wPANOSEInfo;
            wPan2 = lpen2->wPANOSEInfo;

            if (wPan1==wPan2)
                goto sort_name;

            return (wPan1<=wPan2)?-1L:1L;
        }
        break;
    }
    return 0L;

} //*** DBELBSortProc


//*************************************************************
//
//  HandleDBELBCommand
//
//  Purpose:
//      Handles commands that are for the slider or the ELB
//
//
//  Parameters:
//      HWND hELB
//      UINT msg
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (LONG WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               2/26/92   MSM        Created
//
//*************************************************************

LONG WINAPI HandleDBELBCommand(HWND hELB,UINT msg,WPARAM wParam,LPARAM lParam)
{
    UINT uAll = 0;

    if (msg!=WM_COMMAND)    
        return DefWindowProc(hELB,msg,wParam,lParam);

    switch (HIWORD(lParam))
    {
        case ELBN_SPACE:
        {
            int       nIndex;
            LPELBNODE lpen = (LPELBNODE)ELBGetCurSel(hELB, (LPINT)&nIndex );
            RECT      rc, rc2;
            HWND      hLB = GetWindow( hELB, GW_CHILD );
            LPDB      lpdb = ELBGetDatabase( hELB );
            DWORD     dwP, dwS;

            if (!lpen || !lpdb)
                return 0L;

            //*** can't select publishers
            if (lpen->wNodeType == NT_PUB)
                return 0L;

            if (nIndex<0 || IsAncestorChecked( (LPDBNODE)lpen ))
            {
                MessageBeep(0);
                break;
            }

            switch (NODETYPE(lpen))
            {
                case NT_STYLE:
                    dwP = FONTPRICE(((LPSTYLE)lpen));
                    dwS = FONTSIZE(((LPSTYLE)lpen));
                break;

                case NT_FACE:
                {
                    LPSTYLE lpS = GetFirstStyle( (LPFACE)lpen, NULL );

                    dwP = FONTPRICE(((LPFACE)lpen));
                    dwS = FONTSIZE(((LPFACE)lpen));

                    //*** When checking or unchecking a FACE we need
                    //*** to handle the children properly
                    while (lpS)
                    {
                        if (VI_ISCHECKED(lpS))
                        {
                            if (VI_ISCHECKED(lpen))
                            {
                                lpdb->wFonts++;
                                lpdb->dwTotal += FONTPRICE(lpS);
                                lpdb->dwSize += FONTSIZE(lpS);
                            }
                            else
                            {
                                lpdb->wFonts--;
                                lpdb->dwTotal -= FONTPRICE(lpS);
                                lpdb->dwSize -= FONTSIZE(lpS);
                            }
                        }
                        lpS = GetNextStyle( lpS, NULL );
                    }
                }
                break;

                default:
                    dwP = dwS = 0L;
                break;
            }


            if (VI_ISCHECKED(lpen))
            {
                VI_UNCHECKNODE(lpen);
                lpdb->wFonts--;
                lpdb->dwTotal -= dwP;
                lpdb->dwSize -= dwS;
            }
            else
            {
                VI_CHECKNODE(lpen);
                lpdb->wFonts++;
                lpdb->dwTotal += dwP;
                lpdb->dwSize += dwS;
            }

            SendMessage( hLB, LB_GETITEMRECT, nIndex, (LONG)(LPRECT)&rc );
            GetClientRect( hLB, &rc2 );
            rc.bottom = rc2.bottom;
            InvalidateRect( hLB, &rc, FALSE );
            UpdateWindow( hLB );
        }
        break;

        case ELBN_RDBLCLK:
            uAll = 1;
            //*** Fall through

        case ELBN_ENTER:
        case ELBN_DBLCLK:
        {
            int      nIndex;
            LPDBNODE lpdn = (LPDBNODE)ELBGetCurSel(hELB, (LPINT)&nIndex );

            if (lpdn)
            {
                if ( VI_GETOPENSTATUS(lpdn) )
                    CollapseDBELBNode( hELB, nIndex, uAll );
                else
                    ExpandDBELBNode( hELB, nIndex, uAll );
            }        
        }
        break;
    }
    return DefWindowProc(hELB,msg,wParam,lParam);

} //*** HandleDBELBCommand


//*************************************************************
//
//  IsAncestorChecked
//
//  Purpose:
//      Checks to see if anyone in the hierarchy has been checked
//
//
//  Parameters:
//      LPDBNODE lpdb
//      
//
//  Return: (BOOL WINAPI)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//               3/ 9/92   MSM        Created
//
//*************************************************************

BOOL WINAPI IsAncestorChecked( LPDBNODE lpdb )
{
    lpdb = lpdb->lpParent;

    while (lpdb)
    {
        if (lpdb->wViewInfo & VIB_CHECKED)
            return TRUE;
        lpdb = lpdb->lpParent;
    }
    return FALSE;

} //*** IsAncestorChecked

//*** EOF: dbelb.c
