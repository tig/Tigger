//temp.cpp

void CColumnListBox::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // Size contained listbox so it fills the client area
    //    
    m_lb.SetWindowPos( NULL, 0, m_cyFont, cx+1, cy+1 - m_cyFont, SWP_NOZORDER ) ;        
}


void CColumnListBox::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (GetStyle() & (LBS_OWNERDRAWVARIABLE | LBS_OWNERDRAWFIXED))
    {
        return GetParent->OnDrawItem( nIDCtl, lpDIS ) ;
    }

    if (lpDIS->itemID == LB_ERR)
        return ;

    LPROWINFO       lpD ;
    
    CDC             dc ;             
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;

    CString         szItem ;

    dc.Attach( lpDIS->hDC ) ;
    
    lpD = (LPROWINFO)lpDIS->itemData ;
    rcFocus = lpDIS->rcItem ;    

    m_lb.GetText( lpDIS->itemID, szItem ) ;
    
    if (fSelected = (lpDIS->itemState & ODS_SELECTED) ? TRUE : FALSE)
    {
        dc.SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
        dc.SetBkColor( rgbBack = GetSysColor( COLOR_HIGHLIGHT ) ) ;
    }
    else
    {
        dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) ) ;
        dc.SetBkColor( rgbBack = GetSysColor( COLOR_WINDOW ) ) ;
    }
    
    // if we are loosing focus, remove the focus rect before
    // drawing.
    //
    
    if ((lpDIS->itemAction) & (ODA_FOCUS))
        if (!((lpDIS->itemState) & (ODS_FOCUS)))
            dc.DrawFocusRect( &rcFocus ) ;
    
    y = lpDIS->rcItem.top ;
    x = lpDIS->rcItem.left ;
        
    cy = (rcFocus.bottom - rcFocus.top - m_cyFont) / 2 ;

    // draw
    ColumnTextOut( &dc, x + 4, y + cy, &lpDIS->rcItem, szItem ) ;

    // if we are gaining focus draw the focus rect after drawing
    // the text.
    if ((lpDIS->itemAction) & (ODA_FOCUS))
        if ((lpDIS->itemState) & (ODS_FOCUS))
             dc.DrawFocusRect( &rcFocus ) ;
    
    
    if (fSelected)
    {
        dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) ) ;
        dc.SetBkColor( GetSysColor( COLOR_WINDOW ) ) ;
    }

    dc.Detach() ;
}

void CColumnListBox::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    lpMeasureItemStruct->itemHeight = m_cyFont ;
}
 
void CColumnListBox::OnDblClk()
{   
    int i = m_lb.GetCurSel() ;
    LPROWINFO lpD = (LPROWINFO)m_lb.GetItemData( i ) ;
    char szName[64] ;
    
    if (!lpD)
    {
        AfxMessageBox( "Something's wrong.  Could not get internal data structure." ) ;
        return ;
    }
    
    m_lb.GetText( i, szName ) ;
    
}           
 
int CColumnListBox::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
    if (nKey == VK_RETURN)
    {
        OnDblClk() ;
        return (LPARAM)-2 ;
    }
    return (LPARAM)-1 ;
}


// The ColumntTextOut function writes a character string at
// the specified location, using tabs to identify column breaks.  Each
// column is aligned according to the array of COLINFO
//
void CColumnListBox::ColumnTextOut( CDC* pDC, int nX, int nY, LPRECT lprc, CString& rStr )
{

#define ETOFLAGS ETO_CLIPPED | ETO_OPAQUE

    COLINFO         CI ;              // local copy for speed 
    RECT           rc ;              // current cell rect 
    UINT           cIteration = 0 ;  // what column 
    LPSTR          lpNext ;          // next string (current is lpsz) 
    int            cch ;             // count of chars in current string 
    
    if (m_rgColInfo == NULL || m_cColumns <= 1)
    {                                                                      
        int Tab = 15 ;   
        pDC->ExtTextOut( nX, nY, ETO_OPAQUE, lprc, NULL, 0, NULL ) ;
        pDC->TabbedTextOut( nX, nY, rStr, rStr.GetLength(), 1, &Tab, nX ) ;
        return ;
    }

    rc = *lprc ;
    
    // For each sub string (bracketed by a tab)
    //
    LPSTR lpsz = rStr.GetBuffer( rStr.GetLength() + 2 ) ;
    while (*lpsz && cIteration < m_cColumns)
    {
        for (cch = 0, lpNext = lpsz ;
             *lpNext != '\t' && *lpNext ;
             lpNext++, cch++)
            ;
    
        CI = m_rgColInfo[cIteration] ;
        
        if (CI.uiWidth == (UINT)-1)
            CI.uiWidth = lprc->right - lprc->left ;  
         
        // special case left most column
        if (cIteration = 0)
        {                    
            rc.left = lprc->left ;
        }

        rc.right = nX + CI.uiWidth ;
         
        // special case right most column
        if (cIteration == m_cColumns - 1)    
        {
            rc.right = lprc->right ;
        }
    
        // If the width of the column is 0 do nothing
        //
        if (CI.uiWidth > 0)
        {
            switch(CI.uiFormatFlags)
            {
                case DT_CENTER:
                {
                    CSize size = pDC->GetTextExtent( lpsz, cch ) ;
                    pDC->ExtTextOut( nX + ((CI.uiWidth - size.cx) / (int)2),
                                      nY, ETOFLAGS, &rc, lpsz, cch, NULL ) ;
                }                                   
                break ;
                
                case DT_RIGHT:
                {
                    CSize size = pDC->GetTextExtent( lpsz, cch ) ;
                    
                    pDC->ExtTextOut( nX + (CI.uiWidth - size.cx),
                                     nY, ETOFLAGS, &rc, lpsz, cch, NULL ) ;
                }                                  
                break ;
                
                case DT_LEFT:
                default:
                    pDC->ExtTextOut( nX + CI.uiIndent, nY, ETOFLAGS, &rc, lpsz, cch, NULL ) ;
                break ;
            }
        }
        
        nX = rc.left = rc.right ;
        cIteration++ ;
        lpsz = lpNext + 1 ;
    }
    
    rStr.ReleaseBuffer() ;
    
} // ColumnTextOut()  


