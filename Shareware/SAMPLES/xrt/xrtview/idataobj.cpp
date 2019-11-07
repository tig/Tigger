// WOSA/XRT XRTView Sample Application Version 1.00.003
//
// Copyright (c) 1994 Microsoft Corp.  All Rights Reserved.
//
// idataobj.cpp
//
// Implementation file for the IDataObject interface viewer.
//
// This file serves as the primary example of implementing a WOSA/XRT
// data format decoder.  
// Revisions:
//  August 7, 1993      cek     First implementation.
//  February 19, 1994   cek     Derived from the IDataObject viewer in DEFO2V

#include "precomp.h"
#include "wosaxrt.h"
#include "xrtview.h" 
#include "idataobj.h"  

static HFONT       g_hFont ;
static HFONT       g_hFontBold ;
static UINT        g_cyFont ;
static CLIPFORMAT  g_CF_XRT = 0 ;
WNDPROC            g_pfnEdit = NULL ;

extern "C"
LRESULT EXPORT CALLBACK fnEditSubclass( HWND hwnd, UINT uiMsg, WPARAM wP, LPARAM lP ) ;

// DisplayIDataObject
//
// This is the "DisplayInterface" or "Interface Viewer" function for the
// IDataObject interface.   This implementation simply pops up a dialog
// box that has an ownerdraw listbox in it.
//
// It is expected that this function will be called from the Ole2View
// tool.   See the Ole2View helpfile in the OLE 2.01 and MSVC++ 1.5 kits
// for details on Ole2View interface viewers.
//
// The listbox is filled with the FORMATETCs that are enumerated through 
// the IEnumFORMATETC interface that is retrieved from IDataObject::EnumFormatEtc
//
extern "C"
HRESULT CALLBACK DisplayIDataObject( HWND hwndParent, LPDATAOBJECT lpDO, LPIID lpiid, LPTSTR lpszName )
{
    HRESULT hr = NULL ;

    if (g_CF_XRT == 0)
        g_CF_XRT = RegisterClipboardFormat(szCF_WOSAXRT100) ;
    
    IDataObjDlg FAR* pdlg ;
    pdlg = new FAR IDataObjDlg( hwndParent, lpDO, lpiid, lpszName ) ;
    
    if (pdlg)
    {
        pdlg->DoModal() ;
        delete pdlg ;
    }
    
    return hr ;
}

/////////////////////////////////////////////////////////////////////////////
// IDataObjDlg dialog

IDataObjDlg::IDataObjDlg(HWND hwnd, LPDATAOBJECT lpDO,LPIID lpiid, LPTSTR lpszName )
{         
    m_cbFmtEtc = NULL ;
    m_edtAdvise = NULL ;
    m_MetaFile.hMF = NULL ;
    
    m_hWndParent = hwnd ;
    m_lpDO = lpDO ;
    m_lpiid = lpiid ;
    m_lpszName = lpszName ;

    m_pSink = NULL ;
    m_dwConn = 0 ;
    m_advf = ADVF_PRIMEFIRST ;

#ifdef UNICODE
    m_fetc.cfFormat = CF_UNICODETEXT ;
#else
    m_fetc.cfFormat = CF_TEXT ;
#endif
    m_fetc.dwAspect = DVASPECT_CONTENT ;
    m_fetc.ptd = NULL ;
    m_fetc.tymed = TYMED_HGLOBAL ;
    m_fetc.lindex = - 1 ;  
    
    m_fDoOnGetDataPosted = FALSE ;
    
    m_cchOutput = 0 ;
    m_cLinesOutput = 0 ;
}      

IDataObjDlg::~IDataObjDlg()
{   
    if (m_MetaFile.hMF != NULL)
        DeleteMetaFile( m_MetaFile.hMF ) ;
}

int IDataObjDlg::DoModal( void )
{
    return DialogBoxParam( g_hInst, MAKEINTRESOURCE( IDD_IDATAOBJDLG ), m_hWndParent, (DLGPROC)fnIDataObjDlg, (LONG)this ) ;
}

extern "C"
BOOL EXPORT CALLBACK fnIDataObjDlg( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    static BOOL fInErrSpace = FALSE ;    

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
                
                case IDC_CLEAROUTPUT:
                    if (pIDOD)
                    {
                        pIDOD->m_cchOutput = 0 ;
                        pIDOD->m_cLinesOutput = 0 ;
                        if (pIDOD->m_MetaFile.hMF != NULL)
                        {
                            DeleteMetaFile( pIDOD->m_MetaFile.hMF ) ;
                            pIDOD->m_MetaFile.hMF = NULL ;
                        }
                        SetWindowText( pIDOD->m_edtAdvise, "" ) ;
                        InvalidateRect( pIDOD->m_edtAdvise, NULL, TRUE ) ;
                        UpdateWindow( pIDOD->m_edtAdvise ) ;
                    }
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
                       
                case IDC_ADVISEDATA:
                    if (wNotifyCode == EN_ERRSPACE)
                    {
                        #ifdef _DEBUG
                        OutputDebugString("Output Edit Control reports EN_ERRSPACE\r\n") ;
                        #endif
                    }
                break ;
                                       
                case IDC_DUMPTOFILE:
                    // TODO:  Pop up dlg when set to true...
                    
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
        
        case WM_OUTPUTBUFFERHASDATA:
            if (pIDOD)
                pIDOD->OnOutputBufferHasData() ;
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
    m_cbFmtEtc = GetDlgItem( m_hDlg, IDC_FORMATETC ) ;
    
    m_edtAdvise = GetDlgItem( m_hDlg, IDC_ADVISEDATA ) ;
    
    // Sublcass the edit so we can override it's WM_PAINT and
    // draw a metafile if we want to...
    if (g_pfnEdit == NULL)
        g_pfnEdit = SubclassWindow( m_edtAdvise, fnEditSubclass) ;

    m_chkUpdateDisplay = GetDlgItem( m_hDlg, IDC_UPDATEDISPLAY );
    m_fUpdateDisplay = TRUE ;
    Button_SetCheck( m_chkUpdateDisplay, m_fUpdateDisplay ) ;

    m_chkPrimeFirst = GetDlgItem( m_hDlg, IDC_PRIMEFIRST );
    Button_SetCheck( m_chkPrimeFirst, m_advf == ADVF_PRIMEFIRST ) ;


    m_chkDump = GetDlgItem( m_hDlg, IDC_DUMPTOFILE ) ;
    EnableWindow( m_chkDump, FALSE ) ;

    TEXTMETRIC  tm ;   
    HDC hdc = GetDC(NULL);
    g_hFont = ReallyCreateFont( hdc, TEXT( "MS Sans Serif" ), TEXT( "Regular" ), 8, 0 ) ;
    g_hFontBold = ReallyCreateFont( hdc, TEXT( "MS Sans Serif" ), TEXT( "Bold" ), 8, 0 ) ;
    g_hFont = (HFONT)SelectObject( hdc, g_hFont ) ;
    GetTextMetrics( hdc, &tm ) ;
    g_hFont = (HFONT)SelectObject( hdc, g_hFont ) ;
    ReleaseDC( NULL, hdc ) ;
    g_cyFont = tm.tmHeight + tm.tmExternalLeading ;                   
    
    SetWindowFont( m_cbFmtEtc, g_hFont, TRUE ) ;
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
        MessageBox( m_hDlg, TEXT( "Wrong IID" ), TEXT( "Error" ), MB_OK ) ;
    }
    
    DlgCenter( m_hDlg, m_hWndParent, FALSE ) ;    
    RECT rc ;
    GetWindowRect( m_hDlg, &rc ) ;
    SetWindowPos( m_hDlg, NULL, rc.left, rc.top, rc.right - rc.left + 1,
                  rc.bottom - rc.top +1, SWP_NOMOVE|SWP_NOZORDER | SWP_NOACTIVATE ) ;

    SetWindowText( m_hDlg, m_lpszName ) ;

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void IDataObjDlg::OnDestroy()
{   
    OnKillAdvise() ;     

    if (g_pfnEdit != NULL && m_edtAdvise && IsWindow(m_edtAdvise))
    {
        SubclassWindow(m_edtAdvise, g_pfnEdit) ;
        g_pfnEdit = NULL ;
    }
    
    int c = ComboBox_GetCount( m_cbFmtEtc ) ;
    for (int i = 0 ; i < c ; i++)
    {
        LPITEMDATA lpID = (LPITEMDATA)ComboBox_GetItemData( m_cbFmtEtc, i ) ;
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
    if (m_edtAdvise && !IsWindow( m_edtAdvise ) )
        return ;

    RECT    rc ;                    
    RECT    rcWnd ;
    GetClientRect( m_hDlg, &rcWnd ) ; 
    GetWindowRect( m_edtAdvise, &rc ) ;    
    MapWindowPoints( NULL, m_hDlg, (POINT FAR*)&rc, 2 ) ;
    SetWindowPos( m_edtAdvise, NULL, -1, rc.top, cx+2, cy - rc.top + 1,
                SWP_NOZORDER | SWP_NOACTIVATE ) ;
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
    TCHAR           szItem[128] ;

    lpID = (LPITEMDATA)lpDIS->itemData ;
    rcFocus = lpDIS->rcItem ;    
    ComboBox_GetLBText( m_cbFmtEtc, lpDIS->itemID, szItem ) ;
    
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
        g_hFontBold = (HFONT)SelectObject( lpDIS->hDC, g_hFontBold ) ;

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
        g_hFontBold = (HFONT)SelectObject( lpDIS->hDC, g_hFontBold ) ;

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
    
    if (m_MetaFile.hMF != NULL)
    {
        DeleteMetaFile( m_MetaFile.hMF ) ;
        m_MetaFile.hMF = NULL ;
    }
    InvalidateRect( m_edtAdvise, NULL, TRUE ) ;
    SetWindowText( m_edtAdvise, "" ) ;

    int i = ComboBox_GetCurSel( m_cbFmtEtc ) ;
    if (i != LB_ERR)
    {
        LPITEMDATA lpid = (LPITEMDATA)ComboBox_GetItemData( m_cbFmtEtc, i ) ;
        if (lpid && lpid->lpData)
        {
            m_fetc = *(LPFORMATETC)lpid->lpData ;
            return ;
        }
    }
    
#ifdef UNICODE
    m_fetc.cfFormat = CF_UNICODETEXT ;
#else
    m_fetc.cfFormat = CF_TEXT ;
#endif    
    m_fetc.dwAspect = DVASPECT_CONTENT ;
    m_fetc.ptd = NULL ;
    m_fetc.tymed = TYMED_HGLOBAL ;
    m_fetc.lindex = - 1 ;
}


void IDataObjDlg::OnDoGetData()
{
    HRESULT     hr ;
    STGMEDIUM   stm ;
    TCHAR       szBuf[256] ;
    
    if (m_lpDO == NULL)
    {   
        ListBox_AddString( m_lbGetData, TEXT( "m_lpDO == NULL" ) ) ;
        return ;
    }    
    
    hr = m_lpDO->QueryGetData( &m_fetc ) ;
    if (FAILED( hr ))
    {
        wsprintf( szBuf, TEXT( "QueryGetData() failed:  %s" ), 
                    (LPTSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        return ;
    }

    stm.tymed = m_fetc.tymed ;    
    hr = m_lpDO->GetData( &m_fetc, &stm ) ;
    if (FAILED( hr ))
    {
        wsprintf( szBuf, TEXT( "GetData() failed:  %s" ), 
                    (LPTSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        return ;
    }
 
    if (FAILED( hr = GotData( &m_fetc, &stm ) ))
    {
        wsprintf( szBuf, TEXT( "IDataDlg::GotData() failed:  %s" ), 
                    (LPTSTR)HRtoString( hr ) ) ;
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
#ifdef UNICODE    
    if (lpfetc->cfFormat == CF_UNICODETEXT && lpstm->tymed & TYMED_HGLOBAL)
#else
    if (lpfetc->cfFormat == CF_TEXT && lpstm->tymed & TYMED_HGLOBAL)
#endif
    {     
        LPTSTR lpsz = (LPTSTR)GlobalLock( lpstm->hGlobal ) ;
        if (lpsz == NULL)
        {
            ListBox_AddString( m_lbGetData, TEXT( "hGlobal returned by GetData is NULL!" ) ) ;                    
            ReleaseStgMedium( lpstm );
            return ResultFromScode( E_FAIL ) ;
        }
        
        if (m_fUpdateDisplay == TRUE)
            WriteToOutput( lpsz ) ;
            
        GlobalUnlock( lpstm->hGlobal ) ;
    }

    if (lpfetc->cfFormat == CF_METAFILEPICT && lpstm->tymed & TYMED_MFPICT)
    {
        LPMETAFILEPICT pMF = (LPMETAFILEPICT)GlobalLock(lpstm->hGlobal) ;
        if (pMF)
        {
            if (m_MetaFile.hMF)
            {
                DeleteMetaFile( m_MetaFile.hMF ) ;
                m_MetaFile.hMF = NULL ;
            }
            
            if (m_fUpdateDisplay)
            {                
                m_MetaFile = *pMF ;
                m_MetaFile.hMF = CopyMetaFile( pMF->hMF, NULL ) ;
                InvalidateRect( m_edtAdvise, NULL, FALSE ) ;
            }
        }
    }
    
    if (lpfetc->cfFormat == g_CF_XRT && lpstm->tymed & TYMED_HGLOBAL)
    {
        WriteToOutput( TEXT( "Got WOSA/XRT representation!\r\n" ) ) ;
        LPMARKETDATA pXRT = (LPMARKETDATA)GlobalLock( lpstm->hGlobal) ;
        if (pXRT == NULL)
        {
            ListBox_AddString( m_lbGetData, TEXT( "hGlobal returned by GetData is NULL!" ) ) ;                    
            ReleaseStgMedium( lpstm );
            return ResultFromScode( E_FAIL ) ;
        }
        m_cItemChanges += xrtDumpToDebug( pXRT ) ;
        GlobalUnlock( lpstm->hGlobal ) ;
    }
    
    m_cOnDataChanges++ ;
        
    ReleaseStgMedium( lpstm );
    
    return  hr;
}

void IDataObjDlg::OnSetupAdvise()
{
    TCHAR szBuf[128] ;
    OnKillAdvise() ;
    
    m_pSink = new CImpIAdviseSink( this ) ;

    if (m_pSink == NULL)
    {
        ListBox_AddString( m_lbGetData, TEXT( "CImpIAdviseSink constructor failed" ) ) ;                    
        return ;
    }
    
    SetWindowText( m_btnSetupAdvise, TEXT( "&Kill Advise" ) ) ;
#ifdef UNICODE
    if (m_fetc.cfFormat == CF_UNICODETEXT)
    {
        wsprintf( szBuf, TEXT( "Advise started with cfFormat of CF_UNICODETEXT..." ) ) ;
    }
#else
    if (m_fetc.cfFormat == CF_TEXT)
    {
        wsprintf( szBuf, TEXT( "Advise started with cfFormat of CF_TEXT..." ) ) ;
    }
#endif
    else
    {
        wsprintf( szBuf, TEXT( "Advise started..." ) ) ;
    }
    ListBox_AddString( m_lbGetData, szBuf ) ;    

    m_dwTime = GetTickCount() ;
    m_cOnDataChanges = 0 ;
    m_cItemChanges = 0 ;

    HRESULT hr = m_lpDO->DAdvise( &m_fetc, m_advf, m_pSink, &m_dwConn ) ;        
    if (FAILED( hr ))
    {
        wsprintf( szBuf, TEXT( "DAdvise() failed:  %s" ), (LPTSTR)HRtoString( hr ) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;                    
        SetWindowText( m_btnSetupAdvise, TEXT( "&DAdvise" ) ) ;
        return ;
    }

}

void IDataObjDlg::OnKillAdvise()
{
    if (m_dwConn != 0)
    {       
        TCHAR szBuf[128] ;                           
        DWORD dwTime = max( 1000, GetTickCount() - m_dwTime ) ;    // avoid div by zero

        m_lpDO->DUnadvise( m_dwConn ) ;
        m_dwConn = 0 ;
        wsprintf( szBuf, TEXT( "  Advise Killed after %lu milliseconds, and %lu OnDataChanges;" ), dwTime, m_cOnDataChanges ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;
        wsprintf( szBuf, TEXT( "  That's %lu.%lu OnDataChange calls per second." ), 
                        m_cOnDataChanges / (dwTime / 1000L),
                        m_cOnDataChanges % (dwTime / 1000L) ) ;
        ListBox_AddString( m_lbGetData, szBuf ) ;
        if (m_fetc.cfFormat == g_CF_XRT)
        {
            wsprintf( szBuf, TEXT( "  WOSA/XRT Clipboard Format used; %lu total DataItem changes;" ), m_cItemChanges ) ;
            ListBox_AddString( m_lbGetData, szBuf ) ;
            wsprintf( szBuf, TEXT( "  That's %lu.%lu total DataItem changes per second." ), 
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
    
    SetWindowText( m_btnSetupAdvise, TEXT( "&DAdvise" ) ) ;
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
    HRESULT     hr1 ;
    
    hr1 = pIDataObject->EnumFormatEtc( DATADIR_GET, &pEFE ) ;
    if (SUCCEEDED( hr1 ))
    {
        DoIEnumFormatEtc( nLevel, pEFE ) ;
        pEFE->Release() ;
    }
    else
        AddItem( nLevel, TEXT( "<<DATA_DIRGET failed>>" ), NULL, I_LABEL ) ;

    if (SUCCEEDED(hr1))    
    {
        UINT ui = ComboBox_FindString( m_cbFmtEtc, 0, szCF_WOSAXRT100 ) ;
        ComboBox_SetCurSel( m_cbFmtEtc, ui ) ;
        OnSelChangeFormatEtc() ;        
    }
    return TRUE ;
}

// This member takes a pointer to an IEnumFORMATETC and enumerates
// the FORMATETC structures avaialbe, inserting them into our
// listbox.
//  
BOOL IDataObjDlg::DoIEnumFormatEtc( UINT nLevel, LPENUMFORMATETC pIFormatEtc )        
{   
    LPCOLUMNSTRUCT  rgCol = new COLUMNSTRUCT[1] ;
    UINT    cxChar ;
    HDC     hdc  = GetDC( NULL) ;
                        
    // We use the "ColumnTextOut" code in util.c to create nice
    // columns in our list box. 
    //
    g_hFontBold = (HFONT)SelectObject( hdc, g_hFontBold ) ;                        
    cxChar = GetTextMetricsCorrectly( hdc, NULL ) ;
    rgCol[0].nLeft = 0 ;
    rgCol[0].nRight = cxChar * lstrlen( TEXT( "CF_METAFILEPICT_" ) ) ;
    rgCol[0].uiFlags = DT_LEFT ;
    g_hFontBold = (HFONT)SelectObject( hdc, g_hFontBold ) ;                        
    ReleaseDC( NULL, hdc ) ;

    LPFORMATETC     pfetc ;
    FORMATETC       fetc ;
    TCHAR           sz[256] ;
    ULONG           ulFE ; // number returned
            
    while (pIFormatEtc->Next( 1, &fetc, &ulFE ) == S_OK)
    {
        switch( fetc.cfFormat )
        {
        
#ifdef UNICODE
            case CF_UNICODETEXT: lstrcpy( sz, TEXT( "CF_UNICODETEXT" ) ) ; break ;         
#else
            case CF_TEXT: lstrcpy( sz, TEXT( "CF_TEXT" ) ) ; break ;         
#endif
            case CF_BITMAP: lstrcpy( sz, TEXT( "CF_BITMAP" ) ) ; break ;
            case CF_METAFILEPICT: lstrcpy( sz, TEXT( "CF_METAFILEPICT" ) ) ; break ;  
            case CF_SYLK: lstrcpy( sz, TEXT( "CF_SYLK" ) ) ; break ;
            case CF_DIF: lstrcpy( sz, TEXT( "CF_DIF" ) ) ; break ;           
            case CF_TIFF: lstrcpy( sz, TEXT( "CF_TIFF" ) ) ; break ;          
            case CF_OEMTEXT: lstrcpy( sz, TEXT( "CF_OEMTEXT" ) ) ; break ;       
            case CF_DIB: lstrcpy( sz, TEXT( "CF_DIB" ) ) ; break ;           
            case CF_PALETTE: lstrcpy( sz, TEXT( "CF_PALETTE" ) ) ; break ;      
            case CF_PENDATA: lstrcpy( sz, TEXT( "CF_PENDATA" ) ) ; break ;         
            case CF_RIFF: lstrcpy( sz, TEXT( "CF_RIFF" ) ) ; break ;             
            case CF_WAVE: lstrcpy( sz, TEXT( "CF_WAVE" ) ) ; break ;            
            case 0: lstrcpy( sz, TEXT( "\"Wildcard Advise\"" ) ) ; break ;
            default:
                if (!GetClipboardFormatName( (UINT)fetc.cfFormat, sz, 63 ))
                    wsprintf( sz, TEXT( "%#08lX" ), (DWORD)fetc.cfFormat ) ;
            break ;
        }

        // now insert into list
        pfetc = new FORMATETC ;
        *pfetc = fetc ;
        AddItem( nLevel, sz, pfetc, I_LABEL, 1, rgCol ) ;
    }
    
    return TRUE ;
}

UINT DBStrCpy(char* pszDst, LPCSTR pszSrc, int far* pcLines) ;

BOOL  IDataObjDlg::WriteToOutput( LPSTR lpsz ) 
{      
    UINT cch;
    int cLines;

    cch = DBStrCpy(NULL, lpsz, &cLines);

    if (cch > (CCHOUTPUTMAX - m_cchOutput))
    {
        MessageBeep(0);
        return FALSE;
    }

    DBStrCpy(&m_szOutput[m_cchOutput], lpsz, NULL);

    m_cchOutput += cch;
    m_cLinesOutput += cLines;

    // If buffer was originally empty, wake up the main loop.
    //
    if (m_cchOutput == cch)
        PostMessage( m_hDlg, WM_OUTPUTBUFFERHASDATA, 0, 0L);

    return TRUE;
}

// This function performs a string copy (or string length if pszDst is NULL)
// It also ensures the destination string is properly formatted for the
// edit controls: line terminators must be CR followed by LF, in that orderc.
// If pcLines != NULL, returns number of lines in the string in *pcLines
//
// (pulled from the dbwin sample)
//
UINT DBStrCpy(char* pszDst, LPCSTR pszSrc, int far* pcLines)
{
    BYTE ch;
    UINT cch = 0;
    int cLines = 0;

    while (ch = *pszSrc++)
    {
        // If we find a CR or LF, then store a CR/LF in
        // the output string.
        //
        if (ch == 0x0d || ch == 0x0a)
        {
            cch += 2;
            cLines++;
            if (pszDst)
            {
                *pszDst++ = 0x0d;
                *pszDst++ = 0x0a;
            }

            // Skip any other CRs or LFs we find.
            //
            while ((ch = *pszSrc) && (ch == 0x0d || ch == 0x0a))
                ++pszSrc;
        }
        else
        {
            cch++;
            if (pszDst)
                *pszDst++ = ch;
        }
    }
    if (pszDst)
        *pszDst = 0;

    if (pcLines)
        *pcLines = cLines;

    return cch;
}

// Whenever something happens that causes output, it calls the WriteToOutput function
// which puts the data into a buffer (m_szOutput) and posts a message.  This is
// the message handler.  
//
void IDataObjDlg::OnOutputBufferHasData()
{
    if (m_szOutput != NULL && m_cchOutput != 0)
    {
        int cLines = Edit_GetLineCount(m_edtAdvise) + m_cLinesOutput ;
         
        if (cLines > CLINESMAX)
        {
            SetWindowRedraw( m_edtAdvise, FALSE ) ;
            // If the total # of lines is greater than our arbitrary max lines
            // remove some from the top.
            //
            Edit_SetSel( m_edtAdvise, 0, Edit_LineIndex( m_edtAdvise, cLines - CLINESMAX));
            Edit_ReplaceSel( m_edtAdvise, "");
        }

        // Put current output buffer at the end of the edit control
        //
        Edit_SetSel( m_edtAdvise, 32767, 32767 )  ;   // select the end
        Edit_ReplaceSel( m_edtAdvise, m_szOutput ) ;
        Edit_SetSel( m_edtAdvise, 32767, 32767 )  ;   // select the end
        
        if (cLines > CLINESMAX)
            SetWindowRedraw( m_edtAdvise, TRUE ) ;
            
        // Reset the output buffer
        m_cLinesOutput = 0 ;
        m_cchOutput = 0 ;
    }
}


// AddItem does a ListBox_AddString() plus a ListBox_SetItemData().  The
// item data that is set tells our owner-draw code what 'level' this
// item is at (indentation) and whether it is a label (bold), column
// head (column info is stored), or column entry (no column info is
// stored, but previous column head is used in WM_DRAWITEM).
//
int IDataObjDlg::AddItem( UINT nLevel, LPTSTR sz, LPVOID lpData, UINT uiType, int cColumns, LPCOLUMNSTRUCT  rgCol )
{    
    int i ;
    LPITEMDATA      lpID ;

    i = ComboBox_AddString( m_cbFmtEtc,  sz ) ;
    lpID = new ITEMDATA ;        
    lpID->uiType = uiType ;
    lpID->cColumns = cColumns ;
    lpID->rgCol = rgCol ; 
    lpID->nLevel = nLevel ;
    lpID->lpData = lpData ;
    ComboBox_SetItemData( m_cbFmtEtc, i, (DWORD)lpID ) ;
    return i ;
}

int IDataObjDlg::AddItem( UINT nLevel, LPTSTR sz, LPVOID lpData, UINT uiType )
{    
    return AddItem( nLevel, sz, lpData, uiType, 0, NULL ) ;
}

int IDataObjDlg::AddItem( UINT nLevel, LPTSTR sz, LPVOID lpData )
{    
    return AddItem( nLevel, sz, lpData, I_NORMAL, 0, NULL ) ;
}

UINT IDataObjDlg::xrtDumpToDebug( LPMARKETDATA pMD ) 
{
    char sz[1024] ;
    UINT    uiItemChanges = 0 ;

    if (m_fUpdateDisplay) ;
    {
        WriteToOutput("********* xrtDumpToDebug\r\n") ;
        wsprintf( sz, "cbStream = %lu, "
                "dwStatus = %#08lx, "
                "dwSubStatus = %#08lx, "
                "dwVendorData1 = %#08lx, "
                "dwVendorData2 = %#08lx\r\n", 
                (DWORD)pMD->cbStream, 
                (DWORD)pMD->dwStatus,
                (DWORD)pMD->dwSubStatus,
                (DWORD)pMD->dwVendorData1,
                (DWORD)pMD->dwVendorData2 );
        WriteToOutput(sz) ;            
    }
    
    LPUPDATEITEM        pUpdateItem ;
    LPUPDATEITEMPROP    rgPropIDValue ;
    LPPROPERTYSET       pPropSet ;
    DWORD               dwPSO = pMD->dwPropSetOffset ;
    DWORD               dwIO = pMD->dwItemOffset ;
    DWORD               cProps ;
    VARIANT             varTemp ;
    VARIANT             var ;
    VARIANT FAR *       pvar ;

    VariantInit( &varTemp ) ;
    VariantInit( &var ) ;
    
    while (dwIO != 0)
    {
        pUpdateItem = (LPUPDATEITEM)((LPBYTE)pMD + dwIO) ;
        dwIO = pUpdateItem->dwNext ;
        pPropSet = (LPPROPERTYSET)((LPBYTE)pMD + pUpdateItem->dwPropertySet) ;
        
        // We now have a pointer to an UPDATEITEM and a PROPERTYSET 
        // use the combination to decipher the properites in the updateitem
        //
        *sz = '\0' ;
        rgPropIDValue = (LPUPDATEITEMPROP)((LPBYTE)(&pUpdateItem->dwPropertySet) + sizeof(pUpdateItem->dwPropertySet)) ;
        cProps = pUpdateItem->cProperties;
        for (DWORD n = 0 ; n < cProps ; n++)
        {   
            VariantClear( &var ) ;
            pvar = (VARIANT FAR*)((LPBYTE)pUpdateItem + rgPropIDValue[n].dwValueOffset ) ;
            if (pvar->vt == VT_BSTR)
            {
                lstrcat( sz, (LPSTR)pvar + sizeof(VARIANT) + sizeof(DWORD) ) ;
            }
            else if (pvar->vt == (VT_VARIANT | VT_BYREF))
            {                                 
                // TODO: 
                lstrcat( sz, "<<invalid type (VT_VARIANT | VT_BYREF)>>" ); 
            }
            else if (pvar->vt > VT_BOOL)
            {
                lstrcat( sz, "<<invalid type (>VT_BOOL) >>" ) ; // Invalid!  'cause we did the VariantClear above, we're ok.
            }
            else
            {
                if (FAILED(VariantChangeType( &varTemp, pvar, 0, VT_BSTR )))
                {
                    lstrcat( sz, "<<bad type>>" ) ;
                }
                else
                {
                    lstrcat( sz, varTemp.bstrVal ) ;
                    VariantClear( &varTemp ) ;                        
                }                     
            }
                        
            if (n+1 < cProps)
                lstrcat( sz, ",\t" ) ;
            else
                lstrcat( sz, "\r\n" ) ;
        }
        if (m_fUpdateDisplay) ;
            WriteToOutput(sz) ;
        uiItemChanges++ ;
    }

    if (m_fUpdateDisplay) ;
    {
        wsprintf( sz, "**** End of Dump. %d item changes\r\n", uiItemChanges) ;
        WriteToOutput(sz) ;
    }
    
    return uiItemChanges;
}    

extern "C"
LRESULT EXPORT CALLBACK fnEditSubclass( HWND hwnd, UINT uiMsg, WPARAM wP, LPARAM lP )
{   
    if (uiMsg == WM_PAINT)
    {
        HWND hwndParent = GetParent(hwnd) ;
        LPIDATAOBJDLG pIDOD =(LPIDATAOBJDLG)GetWindowLong( hwndParent, DWL_USER ) ;
        if (pIDOD && pIDOD->m_MetaFile.hMF != NULL)
        {
            PAINTSTRUCT ps ;
            BeginPaint( hwnd, &ps ) ;

/*            
            if (pIDOD->m_MetaFile.mm == MM_ISOTROPIC || pIDOD->m_MetaFile.mm == MM_ANISOTROPIC)
            {    
                if (pIDOD->m_MetaFile.xExt != 0 && pIDOD->m_MetaFile.yExt != 0)
                {
                    
                }
            }
            else
            {
                SetMapMode( ps.hdc, MM_ANISOTROPIC ) ;
                SetWindowOrg( ps.hdc, 0, 0 ) ;
                SetWindowExt( ps.hdc, pIDOD->m_MetaFile.xExt, pIDOD->m_MetaFile.yExt ) ;
                SetViewportExt( ps.hdc, pIDOD->m_MetaFile.xExt, pIDOD->m_MetaFile.yExt ) ;
            }
*/            
            PlayMetaFile( ps.hdc, pIDOD->m_MetaFile.hMF ) ;

            EndPaint( hwnd, &ps ) ;
            return 0L ;
        }
    }
    
    return CallWindowProc( g_pfnEdit, hwnd, uiMsg, wP, lP ) ;
}