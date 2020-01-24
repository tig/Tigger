// idataobj.cpp
//
// Implementation file for the IDataObject interface viewer.
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
// August 7, 1993
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  August 7, 1993  cek     First implementation.
//

#include "precomp.h"
#include "xrttest.h" 
#include "idataobj.h"  

static HFONT       g_hFont ;
static HFONT       g_hFontBold ;
static UINT        g_cyFont ;
static UINT        g_CF_XRTDEMO ;

// DisplayIDataObject
//
// This is the "DisplayInterface" or "Interface Viewer" function for the
// IDataObject interface.   This implementation simply pops up a dialog
// box that has an ownerdraw listbox in it.
//
// The listbox is filled with the FORMATETCs that are enumerated through 
// the IEnumFORMATETC interface that is retrieved from IDataObject::EnumFormatEtc
//
extern "C"
HRESULT WINAPI EXPORT DisplayIDataObject( HWND hwndParent, LPDATAOBJECT lpDO, LPIID lpiid, LPSTR lpszName )
{
    HRESULT hr = NULL ;
    
    IDataObjDlg dlg( hwndParent, lpDO, lpiid, lpszName ) ;
    dlg.DoModal() ;

    return hr ;
}

/////////////////////////////////////////////////////////////////////////////
// IDataObjDlg dialog

IDataObjDlg::IDataObjDlg(HWND hwnd, LPDATAOBJECT lpDO,LPIID lpiid, LPSTR lpszName )
{         
    m_lbFmtEtc = NULL ;
    m_hWndParent = hwnd ;
    m_lpDO = lpDO ;
    m_lpiid = lpiid ;
    m_lpszName = lpszName ;

    m_pSink = NULL ;
    m_dwConn = 0 ;
    m_advf = ADVF_PRIMEFIRST ;

    m_fetc.cfFormat = CF_TEXT ;
    m_fetc.dwAspect = DVASPECT_CONTENT ;
    m_fetc.ptd = NULL ;
    m_fetc.tymed = TYMED_HGLOBAL ;
    m_fetc.lindex = - 1 ;  
    
    m_lpUpdateList = NULL ;
    m_fDoOnGetDataPosted = FALSE ;
}      

IDataObjDlg::~IDataObjDlg()
{
    OnKillAdvise() ;
}

int IDataObjDlg::DoModal( void )
{
    return DialogBoxParam( g_hInst, MAKEINTRESOURCE( IDD_IDATAOBJDLG ), m_hWndParent, (DLGPROC)fnIDataObjDlg, (LONG)this ) ;
}

BOOL WINAPI EXPORT fnIDataObjDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LPIDATAOBJDLG pIDOD =(LPIDATAOBJDLG)GetWindowLong( hDlg, DWL_USER ) ;
    
    switch (uiMsg)
    {
        case WM_INITDIALOG: 
            pIDOD=(LPIDATAOBJDLG)lParam ;            
            if (pIDOD==NULL)
            {
                EndDialog( hDlg, 0 ) ;
                return TRUE ;
            }
            SetWindowLong( hDlg, DWL_USER, (LONG)pIDOD ) ;
            pIDOD->m_hDlg = hDlg ;
            return pIDOD->OnInitDialog() ;
        break ;
        
        case WM_DESTROY: 
            if (pIDOD)
                pIDOD->OnDestroy() ;
        break ;
        
        case WM_SIZE: 
            if (pIDOD)
                pIDOD->OnSize( (UINT)wParam, LOWORD( lParam ), HIWORD( lParam ) ) ; 
        break ;
        
        case WM_COMMAND:
        {
            #ifdef WIN32
            WORD wNotifyCode = HIWORD(wParam);  
            WORD wID = LOWORD(wParam);         
            HWND hwndCtl = (HWND) lParam;      
            #else
            WORD wNotifyCode = HIWORD(lParam) ;
            WORD wID = wParam ;
            HWND hwndCtl = (HWND)LOWORD(lParam) ;
            #endif
            
            switch (wID)
            {
                case IDCANCEL:
                    EndDialog( hDlg, IDCANCEL ) ;
                break ;
                
                case IDC_DOGETDATA:
                    pIDOD->OnDoGetData() ;
                break ;
                
                case IDC_SETUPADVISE:
                    if (pIDOD->m_dwConn != 0)
                        pIDOD->OnKillAdvise() ;
                    else
                        pIDOD->OnSetupAdvise() ;
                break ;
                
                case IDC_FORMATETC:
                    if (wNotifyCode == LBN_SELCHANGE)
                        pIDOD->OnSelChangeFormatEtc() ;
                break ;
                
                case IDC_UPDATEDISPLAY:
                    pIDOD->m_fUpdateDisplay = Button_GetCheck( pIDOD->m_chkUpdateDisplay ) ;
                break ;
                
                case IDC_PRIMEFIRST:
                    if (Button_GetCheck( pIDOD->m_chkPrimeFirst ))
                        pIDOD->m_advf = ADVF_PRIMEFIRST ;
                    else
                        pIDOD->m_advf = ADVF_NODATA ;
                break ;
            }
        }
        break ;
        
        case WM_DRAWITEM:
            if (pIDOD)
                pIDOD->OnDrawItem( wParam, (LPDRAWITEMSTRUCT)lParam ) ;
        break ;
        
        case WM_MEASUREITEM:
            if (pIDOD)
                pIDOD->OnMeasureItem( wParam, (LPMEASUREITEMSTRUCT)lParam ) ;
        break ;
        
        default:
           return FALSE ; 
    }    
    return TRUE ;
}


/////////////////////////////////////////////////////////////////////////////
// IDataObjDlg message handlers

BOOL IDataObjDlg::OnInitDialog()
{       
    m_btnDoGetData = GetDlgItem( m_hDlg, IDC_DOGETDATA ) ;
    m_btnSetupAdvise = GetDlgItem( m_hDlg, IDC_SETUPADVISE ) ;
    
    m_lbGetData = GetDlgItem( m_hDlg, IDC_GETDATA ) ;
    m_lbFmtEtc = GetDlgItem( m_hDlg, IDC_FORMATETC ) ;
    
    m_edtAdvise = GetDlgItem( m_hDlg, IDC_ADVISEDATA ) ;

    m_chkUpdateDisplay = GetDlgItem( m_hDlg, IDC_UPDATEDISPLAY );
    m_fUpdateDisplay = TRUE ;
    Button_SetCheck( m_chkUpdateDisplay, m_fUpdateDisplay ) ;

    m_chkPrimeFirst = GetDlgItem( m_hDlg, IDC_PRIMEFIRST );
    Button_SetCheck( m_chkPrimeFirst, m_advf == ADVF_PRIMEFIRST ) ;

    TEXTMETRIC  tm ;   
    HDC hdc = GetDC(NULL);
    g_hFont = ReallyCreateFont( hdc, "MS Sans Serif", "Regular", 8, 0 ) ;
    g_hFontBold = ReallyCreateFont( hdc, "MS Sans Serif", "Bold", 8, 0 ) ;
    g_hFont = SelectObject( hdc, g_hFont ) ;
    GetTextMetrics( hdc, &tm ) ;
    g_hFont = SelectObject( hdc, g_hFont ) ;
    ReleaseDC( NULL, hdc ) ;
    g_cyFont = tm.tmHeight + tm.tmExternalLeading ;                   
    
    SetWindowFont( m_lbFmtEtc, g_hFont, TRUE ) ;
    SetWindowFont( m_lbGetData, g_hFont, TRUE ) ;
    SetWindowFont( m_edtAdvise, g_hFont, TRUE ) ;

    if (IsEqualCLSID( IID_IEnumFORMATETC, *m_lpiid ))
        DoIEnumFormatEtc( 0, (LPENUMFORMATETC)m_lpDO) ;
    else if (IsEqualCLSID( IID_IDataObject, *m_lpiid ))
    {
        DoIDataObject( 0, (LPDATAOBJECT)m_lpDO ) ;   
    }
    else
    {
        MessageBox( m_hDlg, "Wrong IID", "Error", MB_OK ) ;
    }
    
    DlgCenter( m_hDlg, m_hWndParent, FALSE ) ;    
    RECT rc ;
    GetWindowRect( m_hDlg, &rc ) ;
    SetWindowPos( m_hDlg, NULL, rc.left, rc.top, rc.right - rc.left + 1,
                  rc.bottom - rc.top +1, SWP_NOMOVE|SWP_NOZORDER | SWP_NOACTIVATE ) ;

    g_CF_XRTDEMO = RegisterClipboardFormat( "CF_XRTDEMO" ) ;

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void IDataObjDlg::OnDestroy()
{
    int c = ListBox_GetCount( m_lbFmtEtc ) ;
    for (int i = 0 ; i < c ; i++)
    {
        LPITEMDATA lpID = (LPITEMDATA)ListBox_GetItemData( m_lbFmtEtc, i ) ;
        if (lpID)
        {
            if (lpID->uiType == I_COLUMNHEAD)
                delete lpID->rgCol ;
                
            if (lpID->lpData != NULL)
                delete (LPFORMATETC)lpID->lpData ;
            delete lpID ;
        }        
    }

    if (g_hFont)
        DeleteObject( g_hFont );

    if (g_hFontBold)
        DeleteObject (g_hFontBold) ;
}

void IDataObjDlg::OnSize(UINT nType, int cx, int cy)
{
    if (m_lbFmtEtc && !IsWindow( m_lbFmtEtc ) )
        return ;
/*
    RECT    rc ;                    
    RECT    rcWnd ;
    GetClientRect( m_hDlg, &rcWnd ) ; 
    GetWindowRect( m_lbFmtEtc, &rc ) ;    
    MapWindowPoints( NULL, m_hDlg, (POINT FAR*)&rc, 2 ) ;
    rc.left = rcWnd.left + 5 ;
    SetWindowPos( m_lbFmtEtc, NULL, rcWnd.left + 5, rc.top, cx - 10, cy - rc.top - 5,
                SWP_NOZORDER | SWP_NOACTIVATE ) ;
*/
}


void IDataObjDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS )
{ 
    if (lpDIS->itemID == LB_ERR)
        return ;

    LPITEMDATA      lpID ;
    COLORREF        rgbBack ;
    RECT            rcFocus ;
    BOOL            fSelected ;
    int             x, y, cy ;
    char            szItem[128] ;

    lpID = (LPITEMDATA)lpDIS->itemData ;
    rcFocus = lpDIS->rcItem ;    
    ListBox_GetText( m_lbFmtEtc, lpDIS->itemID, szItem ) ;
    
    if (fSelected = (lpDIS->itemState & ODS_SELECTED) ? TRUE : FALSE)
    {
        SetTextColor( lpDIS->hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) ) ;
        SetBkColor( lpDIS->hDC, rgbBack = GetSysColor( COLOR_HIGHLIGHT ) ) ;
    }
    else
    {
        SetTextColor( lpDIS->hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
        SetBkColor( lpDIS->hDC, rgbBack = GetSysColor( COLOR_WINDOW ) ) ;
    }
    
    // if we are loosing focus, remove the focus rect before
    // drawing.
    //
    if ((lpDIS->itemAction) & (ODA_FOCUS))
        if (!((lpDIS->itemState) & (ODS_FOCUS)))
            DrawFocusRect( lpDIS->hDC, &rcFocus ) ;
    
    y = lpDIS->rcItem.top ;
    x = lpDIS->rcItem.left ;

    int cxChar = GetTextMetricsCorrectly( lpDIS->hDC, NULL ) ;
        
    if (lpID && (lpID->uiType == I_COLUMNHEAD || lpID->uiType == I_LABEL))
        g_hFontBold = SelectObject( lpDIS->hDC, g_hFontBold ) ;

    cy = (rcFocus.bottom - rcFocus.top - g_cyFont) / 2 ;
    
    ExtTextOut( lpDIS->hDC, x+2, y + cy, ETO_OPAQUE, &lpDIS->rcItem, NULL, 0, NULL ) ;
    if (lpID)
        ColumnTextOut( lpDIS->hDC, x + 2 + ((cxChar*3) * lpID->nLevel), y + cy, szItem, lpID->cColumns, lpID->rgCol ) ;

    if (lpID && lpID->uiType == I_COLUMNHEAD )
    {
        COLORREF    rgb ;
        RECT        rc = rcFocus ;
        rgb = SetBkColor( lpDIS->hDC, GetTextColor( lpDIS->hDC ) ) ;  
        rc.top = rc.bottom - 1 ; 
        rc.left = x + 2 + ((cxChar*3) * lpID->nLevel) ;
        ExtTextOut( lpDIS->hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;
        SetBkColor( lpDIS->hDC, rgb ) ;  
    }

    if (lpID && (lpID->uiType == I_COLUMNHEAD || lpID->uiType == I_LABEL))
        g_hFontBold = SelectObject( lpDIS->hDC, g_hFontBold ) ;

    // if we are gaining focus draw the focus rect after drawing
    // the text.
    if ((lpDIS->itemAction) & (ODA_FOCUS))
        if ((lpDIS->itemState) & (ODS_FOCUS))
             DrawFocusRect( lpDIS->hDC, &rcFocus ) ;
    
    if (fSelected)
    {
        SetTextColor( lpDIS->hDC, GetSysColor( COLOR_WINDOWTEXT ) ) ;
        SetBkColor( lpDIS->hDC, GetSysColor( COLOR_WINDOW ) ) ;
    }

}

void IDataObjDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    lpMeasureItemStruct->itemHeight = g_cyFont ;
}

void IDataObjDlg::OnDblClkFormatEtc()
{
}

void IDataObjDlg::OnSelChangeFormatEtc()
{
    int i = ListBox_GetCurSel( m_lbFmtEtc ) ;
    if (i != LB_ERR)
    {
        LPITEMDATA lpid = (LPITEMDATA)ListBox_GetItemData( m_lbFmtEtc, i ) ;
        if (lpid && lpid->lpData)
        {
            m_fetc = *(LPFORMATETC)lpid->lpData ;
            return ;
        }
    }
    
    m_fetc.cfFormat = CF_TEXT ;
    m_fetc.dwAspect = DVASPECT_CONTENT ;
    m_fetc.ptd = NULL ;
    m_fetc.tymed = TYMED_HGLOBAL ;
    m_fetc.lindex = - 1 ;
}


void IDataObjDlg::OnDoGetData()
{
    HRESULT     hr ;
    STGMEDIUM   stm ;
    char        szBuf[256] ;
    
    if (m_lpDO == NULL)
    {   
        ListBox_AddString( m_lbGetData, "m_lpDO == NULL" ) ;
        return ;
    }    
    
    hr = m_lpDO->QueryGetData( &m_fetc ) ;
    if (FAILED( hr ))
    {
        wsprintf( szBuf, "QueryGetData() failed:  %s", 
                    (LPSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        return ;
    }

    stm.tymed = m_fetc.tymed ;    
    hr = m_lpDO->GetData( &m_fetc, &stm ) ;
    if (FAILED( hr ))
    {
        wsprintf( szBuf, "GetData() failed:  %s", 
                    (LPSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        return ;
    }
 
    if (FAILED( hr = GotData( &m_fetc, &stm ) ))
    {
        wsprintf( szBuf, "IDataDlg::GotData() failed:  %s", 
                    (LPSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        return ;
    }   
    
    // If we're warm linked, then don't let the queue overfill.  Allow only one
    // message at a time.  This is similar to the way Excel handles it's DDE Links.
    //
    m_fDoOnGetDataPosted = FALSE ;
}


HRESULT IDataObjDlg::GotData( LPFORMATETC lpfetc, LPSTGMEDIUM lpstm )
{
    HRESULT hr = NOERROR ;
    
    if (lpfetc->cfFormat == CF_TEXT && lpstm->tymed & TYMED_HGLOBAL)
    {     
        LPSTR lpsz = (LPSTR)GlobalLock( lpstm->hGlobal ) ;
        if (lpsz == NULL)
        {
            ListBox_AddString( m_lbGetData, "hGlobal returned by GetData is NULL!" ) ;                    
            ReleaseStgMedium( lpstm );
            return ResultFromScode( E_FAIL ) ;
        }
        
        if (m_fUpdateDisplay == TRUE)
            SetWindowText( m_edtAdvise, lpsz ) ;
            
        GlobalUnlock( lpstm->hGlobal ) ;
    }

    if (lpfetc->cfFormat == g_CF_XRTDEMO && lpstm->tymed & TYMED_HGLOBAL)
    {   
        LPXRTSTOCKS lpStocks = (LPXRTSTOCKS)GlobalLock( lpstm->hGlobal ) ;
        LPXRTSTOCK lpQ ;

        if (lpStocks == NULL)
        {
            ListBox_AddString( m_lbGetData, "hGlobal returned by GetData is NULL!" ) ;                    
            ReleaseStgMedium( lpstm );
            return ResultFromScode( E_FAIL ) ;
        }
        
        // if our local update list is empty then this must be the first update. 
        // Assume it contains all items we requested.  Note that when the
        // XRT spec is further along, we will have requested items through
        // IDispatch.  But for now we assume we requested everything.
        //
        if (m_lpUpdateList == NULL)
        {
            UINT n = sizeof(XRTSTOCKS) + (lpStocks->cStocks * sizeof(XRTSTOCK)) ;
            m_lpUpdateList = (LPXRTSTOCKS)GlobalAllocPtr( GHND, n ) ;
            _fmemcpy( m_lpUpdateList, lpStocks, n ) ;
        }
        else
        {
            // Some items are to be updated.
            //
            for ( UINT n = 0 ; n < lpStocks->cStocks ; n ++ )
            {
                lpQ = &lpStocks->rgStocks[0] + n ;
                if (lpQ->uiMembers)
                {
                    // Copy the entire XRTSTOCK structure.  Copying only the members that
                    // changed would be slower (many compares).   The CF_MARKETDATA clipboard
                    // format that the XRT spec eventually will define will make this
                    // unneccessary.
                    //
                    if (lpQ->uiIndex < m_lpUpdateList->cStocks)
                        _fmemcpy( &m_lpUpdateList->rgStocks[0] + lpQ->uiIndex, lpQ, sizeof( XRTSTOCK ) ) ;
                    else
                    {
                        //Assert(0) ;
                        OutputDebugString( "yank!\r\n" ) ;
                    }
                    m_cItemChanges++ ;
                }            
            }
        }
        
        
        // If the display is to be updated on each change...
        //
        if (m_fUpdateDisplay == TRUE)
        {
            LPSTR lpsz = (LPSTR)GlobalAllocPtr( GHND, (1+lpStocks->cStocks) * 64 );
            LPSTR p = lpsz ;
            lstrcpy( p, "change\tSymb\tHigh\tLow\tLast\tVolume\r\n" ) ;
            p += lstrlen( p ) ;
            
            // Display the changed items
            //
            for ( UINT n = 0 ; n < lpStocks->cStocks ; n ++ )
            {
                lpQ = &lpStocks->rgStocks[0] + n ;
                if (lpQ->uiMembers)
                {
                    wsprintf( p, "%04X\t%s\t%lu.%02lu\t%lu.%02lu\t%lu.%02lu\t%lu\r\n",     
                            lpQ->uiMembers,
                            (LPSTR)lpQ->szSymbol,
                            lpQ->ulHigh/1000L, lpQ->ulHigh%1000L,
                            lpQ->ulLow/1000L, lpQ->ulLow%1000L,
                            lpQ->ulLast/1000L, lpQ->ulLast%1000L,
                            lpQ->ulVolume
                             ) ;
                }
                else
                {
                    wsprintf( p, "%04X\t%s\t\t\t\t\t\r\n",
                            lpQ->uiMembers,
                            (LPSTR)lpQ->szSymbol ) ;
                }
                p += lstrlen( p ) ;        
                
                // lpQ->uiMembers indicates if data has changed
                //
                if (lpQ->uiMembers)
                    m_cItemChanges++ ;
            }
            SetWindowText( m_edtAdvise, lpsz ) ;
            GlobalFreePtr( lpsz ) ;    
        }
        
        GlobalUnlock( lpstm->hGlobal ) ;
    }

    if (lpfetc->cfFormat == CF_METAFILEPICT && lpstm->tymed & TYMED_MFPICT)
        SetWindowText( m_edtAdvise, "Got metafile representation!" ) ;

    m_cOnDataChanges++ ;
        
    ReleaseStgMedium( lpstm );
    
    return  hr;
}

void IDataObjDlg::OnSetupAdvise()
{
    char szBuf[128] ;
    OnKillAdvise() ;
    
    m_pSink = new CImpIAdviseSink( this ) ;

    if (m_pSink == NULL)
    {
        ListBox_AddString( m_lbGetData, "CImpIAdviseSink constructor failed" ) ;                    
        return ;
    }
    
    SetWindowText( m_btnSetupAdvise, "&Kill Advise" ) ;
    if (m_fetc.cfFormat == CF_TEXT)
    {
        wsprintf( szBuf, "Advise started with cfFormat of CF_TEXT..." ) ;
    }
    else if (m_fetc.cfFormat == g_CF_XRTDEMO)
    {    
        // This is where we would request items to be updated on using
        // IDispatch...
        //
    
        wsprintf( szBuf, "Advise started with cfFormat of CF_XRTDEMO..." ) ;
    }
    else
    {
        wsprintf( szBuf, "Advise started..." ) ;
    }
    ListBox_AddString( m_lbGetData, szBuf ) ;    
    SetWindowText( m_edtAdvise, szBuf ) ;

    m_dwTime = GetTickCount() ;
    m_cOnDataChanges = 0 ;
    m_cItemChanges = 0 ;

    HRESULT hr = m_lpDO->DAdvise( &m_fetc, m_advf, m_pSink, &m_dwConn ) ;        
    if (FAILED( hr ))
    {
        wsprintf( szBuf, "DAdvise() failed:  %s", (LPSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        SetWindowText( m_btnSetupAdvise, "&Setup Advise" ) ;
        return ;
    }

}

void IDataObjDlg::OnKillAdvise()
{
    if (m_dwConn != 0)
    {       
        char szBuf[128] ;                           
        DWORD dwTime = max( 1000, GetTickCount() - m_dwTime ) ;    // avoid div by zero

        m_lpDO->DUnadvise( m_dwConn ) ;
        m_dwConn = 0 ;
        wsprintf( szBuf, "  Advise Killed after %lu milliseconds, and %lu OnDataChanges;", dwTime, m_cOnDataChanges ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;
        wsprintf( szBuf, "  That's %lu.%lu OnDataChange calls per second.", 
                        m_cOnDataChanges / (dwTime / 1000L),
                        m_cOnDataChanges % (dwTime / 1000L) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;
        if (m_fetc.cfFormat == g_CF_XRTDEMO)
        {
            wsprintf( szBuf, "  CF_XRTDEMO used; %lu total item data changes;", m_cItemChanges ) ;
            ListBox_AddString( m_lbGetData, szBuf ) ;
            wsprintf( szBuf, "  That's %lu.%lu total item data changes per second.", 
                m_cItemChanges / (dwTime /1000L),
                m_cItemChanges % (dwTime /1000L) ) ;
            ListBox_AddString( m_lbGetData, szBuf ) ;
        }
    }
        
    if (m_pSink != NULL)
    {
        m_pSink->Release() ;
        m_pSink = NULL ;
    }
    
    if (m_lpUpdateList != NULL)
    {
        GlobalFreePtr( m_lpUpdateList ) ;
        m_lpUpdateList = NULL ;
    }
    
    SetWindowText( m_btnSetupAdvise, "&Setup Advise" ) ;
}

// DoIDataObject
//
// This is where we start doing IDataObject dirty work.
// This function calls pIDataObject->EnumFormatEtc twice, once
// each for DATADIR_GET and DATADIR_SET, to get a pIEnumFORMATETC.
//
// We then call DoIEnumFormatEtc() with this pointer to actually
// perform the enumaration.
//  
BOOL IDataObjDlg::DoIDataObject( UINT nLevel, LPDATAOBJECT pIDataObject )        
{
    LPENUMFORMATETC pEFE ;
    HRESULT     hr ;
    
    hr = pIDataObject->EnumFormatEtc( DATADIR_GET, &pEFE ) ;
    if (SUCCEEDED( hr ))
    {
        AddItem( nLevel, "FORMATETC avalable through DATA_DIRGET:", NULL, I_LABEL ) ;
        DoIEnumFormatEtc( nLevel+1, pEFE ) ;
        pEFE->Release() ;
    }
    else
        AddItem( nLevel, "EnumFormatEtc(DATA_DIRGET) failed to return an enumerator.", NULL, I_LABEL ) ;

    AddItem( nLevel, " ", NULL, I_LABEL ) ;
    
    hr = pIDataObject->EnumFormatEtc( DATADIR_SET, &pEFE ) ;
    if (SUCCEEDED( hr ))
    {
        AddItem( nLevel, "FORMATETC avalable through DATA_DIRSET::", NULL, I_LABEL ) ;
        DoIEnumFormatEtc( nLevel+1, pEFE ) ;
        pEFE->Release() ;
    }
    else
        AddItem( nLevel, "EnumFormatEtc(DATA_DIRSET) failed to return an enumerator.", NULL, I_LABEL ) ;
        
    ListBox_SetCurSel( m_lbFmtEtc, 2 ) ;
    OnSelChangeFormatEtc() ;        
    return TRUE ;
}

// This member takes a pointer to an IEnumFORMATETC and enumerates
// the FORMATETC structures avaialbe, inserting them into our
// listbox.
//  
BOOL IDataObjDlg::DoIEnumFormatEtc( UINT nLevel, LPENUMFORMATETC pIFormatEtc )        
{   
    LPCOLUMNSTRUCT  rgCol = new COLUMNSTRUCT[5] ;
    UINT    cxChar ;
    HDC     hdc  = GetDC( NULL) ;
                        
    // We use the "ColumnTextOut" code in util.c to create nice
    // columns in our list box. 
    //
    g_hFontBold = SelectObject( hdc, g_hFontBold ) ;                        
    cxChar = GetTextMetricsCorrectly( hdc, NULL ) ;
    rgCol[0].nLeft = 0 ;
    rgCol[0].nRight = cxChar * lstrlen( "CF_METAFILEPICT_" ) ;
    rgCol[0].uiFlags = DT_LEFT ;
        
    rgCol[1].nLeft = rgCol[0].nRight + cxChar ;
    rgCol[1].nRight = rgCol[1].nLeft + cxChar * lstrlen( "0000:0000_" ) ;
    rgCol[1].uiFlags = DT_LEFT ;

    rgCol[2].nLeft = rgCol[1].nRight + cxChar ;
    rgCol[2].nRight = rgCol[2].nLeft + cxChar * lstrlen( "THUMBNAIL_" ) ;
    rgCol[2].uiFlags = DT_LEFT ;

    rgCol[3].nLeft = rgCol[2].nRight + cxChar ;
    rgCol[3].nRight = rgCol[3].nLeft + cxChar * lstrlen( "0000:0000_" ) ;
    rgCol[3].uiFlags = DT_LEFT ;

    rgCol[4].nLeft = rgCol[3].nRight + cxChar ;
    rgCol[4].nRight = rgCol[4].nLeft + cxChar * lstrlen( "_HGLOBAL_" ) ;
    rgCol[4].uiFlags = DT_LEFT ;
        
    g_hFontBold = SelectObject( hdc, g_hFontBold ) ;                        
    ReleaseDC( NULL, hdc ) ;

    AddItem( nLevel, "cfFormat\tptd\tdwAspect\tlindex\ttymed", NULL, I_COLUMNHEAD, 5, rgCol ) ;
    LPFORMATETC     pfetc ;
    FORMATETC       fetc ;
    char            sz[256] ;
    ULONG           ulFE ; // number returned
            
    while (pIFormatEtc->Next( 1, &fetc, &ulFE ) == S_OK)
    {
        switch( fetc.cfFormat )
        {
            case CF_TEXT: lstrcpy( sz, "CF_TEXT\t" ) ; break ;         
            case CF_BITMAP: lstrcpy( sz, "CF_BITMAP\t" ) ; break ;
            case CF_METAFILEPICT: lstrcpy( sz, "CF_METAFILEPICT\t" ) ; break ;  
            case CF_SYLK: lstrcpy( sz, "CF_SYLK\t" ) ; break ;
            case CF_DIF: lstrcpy( sz, "CF_DIF\t" ) ; break ;           
            case CF_TIFF: lstrcpy( sz, "CF_TIFF\t" ) ; break ;          
            case CF_OEMTEXT: lstrcpy( sz, "CF_OEMTEXT\t" ) ; break ;       
            case CF_DIB: lstrcpy( sz, "CF_DIB\t" ) ; break ;           
            case CF_PALETTE: lstrcpy( sz, "CF_PALETTE\t" ) ; break ;      
            case CF_PENDATA: lstrcpy( sz, "CF_PENDATA\t" ) ; break ;         
            case CF_RIFF: lstrcpy( sz, "CF_RIFF\t" ) ; break ;             
            case CF_WAVE: lstrcpy( sz, "CF_WAVE\t" ) ; break ;            
            default:
                if (!GetClipboardFormatName( (UINT)fetc.cfFormat, sz, 63 ))
                    wsprintf( sz, "%#08lX", (DWORD)fetc.cfFormat ) ;
                lstrcat( sz, "\t" ) ;                    
            break ;
        }

        char szTemp[32] ;
        wsprintf( szTemp, "%04lX:%04lX\t", (DWORD)HIWORD( fetc.ptd ), (DWORD)LOWORD( fetc.ptd ) ) ;
        lstrcat( sz, szTemp ) ;
                    
        switch( fetc.dwAspect )
        {
            case DVASPECT_CONTENT: lstrcat( sz, "CONTENT\t" ) ; break ;          
            case DVASPECT_THUMBNAIL: lstrcat( sz, "THUMBNAIL\t" ) ; break ;
            case DVASPECT_ICON: lstrcat( sz, "ICON\t" ) ; break ;
            case DVASPECT_DOCPRINT: lstrcat( sz, "DOCPRINT\t" ) ; break ;
            default:  
                wsprintf( szTemp, "%08lX\t", (DWORD)fetc.dwAspect ) ;
                lstrcat( sz, szTemp ) ; 
            break ;
        }
                    
        wsprintf( szTemp, "%04lX:%04lX\t", (DWORD)HIWORD( fetc.lindex ), (DWORD)LOWORD( fetc.lindex ) ) ;
        lstrcat( sz, szTemp ) ;
                    
        switch( fetc.tymed )
        {
            case TYMED_HGLOBAL: lstrcat( sz, "HGLOBAL\t" ) ; break ;       
            case TYMED_FILE: lstrcat( sz, "FILE\t" ) ; break ;
            case TYMED_ISTREAM: lstrcat( sz, "ISTREAM\t" ) ; break ;
            case TYMED_ISTORAGE: lstrcat( sz, "ISTORAGE\t" ) ; break ;
            case TYMED_GDI: lstrcat( sz, "GDI\t" ) ; break ;
            case TYMED_MFPICT: lstrcat( sz, "MFPICT\t" ) ; break ;
            case TYMED_NULL: lstrcat( sz, "NULL\t" ) ; break ;
            default: 
                wsprintf( szTemp, "%08lX\t", (DWORD)fetc.tymed ) ;
                lstrcat( sz, szTemp ) ; 
            break ;
        }
                    
        // now insert into list
        pfetc = new FORMATETC ;
        *pfetc = fetc ;
        AddItem( nLevel, sz, pfetc, I_COLUMNENTRY, 5, rgCol ) ;
    }
    
    return TRUE ;
}

// AddItem does a ListBox_AddString() plus a ListBox_SetItemData().  The
// item data that is set tells our owner-draw code what 'level' this
// item is at (indentation) and whether it is a label (bold), column
// head (column info is stored), or column entry (no column info is
// stored, but previous column head is used in WM_DRAWITEM).
//
int IDataObjDlg::AddItem( UINT nLevel, LPSTR sz, LPVOID lpData, UINT uiType, int cColumns, LPCOLUMNSTRUCT  rgCol )
{    
    int i ;
    LPITEMDATA      lpID ;
/*            
    {
        LPSTR p = sz.GetBuffer(256) ;            
        if (p)
        {
            TRACE( "AddItem: %s\n", (LPSTR)p ) ;
            sz.ReleaseBuffer() ;
        }
        else
            TRACE( "AddItem: STRING IS NULL!\n" ) ;
    }
*/
    i = ListBox_AddString( m_lbFmtEtc,  sz ) ;
    lpID = new ITEMDATA ;        
    lpID->uiType = uiType ;
    lpID->cColumns = cColumns ;
    lpID->rgCol = rgCol ; 
    lpID->nLevel = nLevel ;
    lpID->lpData = lpData ;
    ListBox_SetItemData( m_lbFmtEtc, i, (DWORD)lpID ) ;
    return i ;
}

int IDataObjDlg::AddItem( UINT nLevel, LPSTR sz, LPVOID lpData, UINT uiType )
{    
    return AddItem( nLevel, sz, lpData, uiType, 0, NULL ) ;
}

int IDataObjDlg::AddItem( UINT nLevel, LPSTR sz, LPVOID lpData )
{    
    return AddItem( nLevel, sz, lpData, I_NORMAL, 0, NULL ) ;
}


