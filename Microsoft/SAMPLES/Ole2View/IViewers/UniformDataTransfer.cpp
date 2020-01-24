// UnifomDataTransfer.cpp : implementation file
//

#include "stdafx.h"
#include "util.h"
#include "iviewers.h"
#include "UniformDataTransfer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUniformDataTransfer dialog


CUniformDataTransfer::CUniformDataTransfer(CWnd* pParent, IDataObject* pdo, REFIID riid, LPCTSTR lpszName) 
	: CDialog(CUniformDataTransfer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUniformDataTransfer)
	//}}AFX_DATA_INIT
    m_pdo = pdo ;
}


void CUniformDataTransfer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUniformDataTransfer)
	DDX_Control(pDX, IDC_DUMMY, m_DummyRichText);
	DDX_Control(pDX, IDC_DATAFORMATS, m_FormatList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUniformDataTransfer, CDialog)
	//{{AFX_MSG_MAP(CUniformDataTransfer)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_DATAFORMATS, OnDblclkDataFormats)
	ON_NOTIFY(NM_CLICK, IDC_DATAFORMATS, OnClickDataFormats)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_DATAFORMATS, OnColumnClickDataFormats)
	ON_NOTIFY(LVN_DELETEITEM, IDC_DATAFORMATS, OnDeleteItemDataFormats)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DATAFORMATS, OnItemChangedDataFormats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUniformDataTransfer message handlers

BOOL CUniformDataTransfer::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CRect rcText ;
    m_DummyRichText.GetWindowRect( &rcText ) ;                                       
    ScreenToClient( &rcText ) ;
    m_RichText.Create(WS_VISIBLE|WS_BORDER, rcText, this, IDC_RICHTEXTCTRL);
    
    if (m_FormatList.GetSafeHwnd())
    {
        CRect rc ;
        CRect rcClient ;
        GetClientRect(&rcClient);
        m_FormatList.GetWindowRect( &rc ) ;                                       
        ScreenToClient( &rc ) ;
        m_FormatList.SetWindowPos( NULL, rc.left, rc.top, 
                            rcClient.right-(2*rc.left), rc.bottom - rc.top, 
                            SWP_NOACTIVATE|SWP_NOZORDER ) ;
    }

    ASSERT(m_pdo);

/*
    FORMATETC fmt;
    STGMEDIUM stgm;
    fmt.cfFormat = CF_TEXT ;
    fmt.ptd = NULL ;
    fmt.dwAspect = 	DVASPECT_CONTENT;
    fmt.lindex = -1 ;
    fmt.tymed = TYMED_HGLOBAL;

    stgm.tymed = TYMED_HGLOBAL;
    stgm.hGlobal = NULL ;
    stgm.pUnkForRelease = NULL ;

    HRESULT hr = m_pdo->GetData(&fmt, &stgm);
    if (SUCCEEDED(hr) && stgm.tymed == TYMED_HGLOBAL && stgm.hGlobal != NULL)
    {
        TCHAR* pstr = (TCHAR*)::GlobalLock(stgm.hGlobal);
        m_RichText.SetText(pstr) ;

        

        ::GlobalUnlock(stgm.hGlobal);
        ReleaseStgMedium(&stgm);
    }
    else
    {
        ErrorMessage(_T("IDataObject::GetData failed"), hr);
    }
*/
    // Setup columns
    //
    m_FormatList.InsertColumn(0, _T("Format"), LVCFMT_LEFT, 180);
    m_FormatList.InsertColumn(1, _T("Target Device"), LVCFMT_LEFT, 80);
    m_FormatList.InsertColumn(2, _T("Aspect"), LVCFMT_LEFT, 80);
    m_FormatList.InsertColumn(3, _T("Index"), LVCFMT_LEFT, 80);
    m_FormatList.InsertColumn(4, _T("Medium Type"), LVCFMT_LEFT, 80);

    HRESULT hr ;
    FORMATETC fmt;
    IEnumFORMATETC* pEFE = NULL ;
    hr = m_pdo->EnumFormatEtc( DATADIR_GET, &pEFE ) ;
    if (SUCCEEDED(hr) && pEFE)
    {
        ULONG ul ; // number returned
        LV_ITEM lvi ;
        TCHAR sz[256] ;
        while (pEFE->Next(1, &fmt, &ul) == S_OK)
        {
            FORMATETC* pfmt = new FORMATETC ;
            ASSERT(pfmt);
            *pfmt = fmt ;
            lvi.mask = LVIF_TEXT | LVIF_PARAM ;
            lvi.iItem = -1 ;
            lvi.iSubItem = 0 ;
            lvi.pszText = GetNameOfClipboardFormat(fmt.cfFormat);
            lvi.lParam = (LPARAM)pfmt ;
            lvi.iItem = m_FormatList.InsertItem(&lvi);

            lvi.mask = LVIF_TEXT ;
            lvi.iSubItem++ ;
            wsprintf(sz, _T( "%#08X" ), (DWORD)fmt.ptd ) ;
            lvi.pszText = sz ;
            m_FormatList.SetItem(&lvi);

            lvi.iSubItem++ ;
            switch(fmt.dwAspect )
            {
            case DVASPECT_CONTENT:   lvi.pszText = _T("CONTENT"); break ;          
            case DVASPECT_THUMBNAIL: lvi.pszText = _T("THUMBNAIL"); break ;
            case DVASPECT_ICON:      lvi.pszText = _T("ICON"); break ;
            case DVASPECT_DOCPRINT:  lvi.pszText = _T("DOCPRINT"); break ;
            default:  
                wsprintf(sz,  _T( "%#08lX" ), (DWORD)fmt.dwAspect ) ;
                lvi.pszText = sz ;
            break ;
            }
            m_FormatList.SetItem(&lvi);

            lvi.iSubItem++ ;
            wsprintf(sz, _T( "%#08X" ), (DWORD)fmt.lindex ) ;
            lvi.pszText = sz ;
            m_FormatList.SetItem(&lvi);

            lvi.iSubItem++ ;
            *sz = _T('\0') ;
            if (fmt.tymed & TYMED_HGLOBAL)
                lstrcpy(sz, _T("HGLOBAL")) ;

            if (fmt.tymed & TYMED_FILE)
            {
                if (*sz != _T('\0'))
                    lstrcat(sz, _T(" | "));
                lstrcat(sz, _T("FILE"));
            }

            if (fmt.tymed & TYMED_ISTREAM)
            {
                if (*sz != _T('\0'))
                    lstrcat(sz, _T(" | "));
                lstrcat(sz, _T("ISTREAM"));
            }

            if (fmt.tymed & TYMED_ISTORAGE)
            {
                if (*sz != _T('\0'))
                    lstrcat(sz, _T(" | "));
                lstrcat(sz, _T("ISTORAGE"));
            }
            if (fmt.tymed & TYMED_GDI)
            {
                if (*sz != _T('\0'))
                    lstrcat(sz, _T(" | "));
                lstrcat(sz, _T("GDI"));
            }
            if (fmt.tymed & TYMED_MFPICT)
            {
                if (*sz != _T('\0'))
                    lstrcat(sz, _T(" | "));
                lstrcat(sz, _T("MFPICT"));
            }
            if (fmt.tymed & TYMED_ENHMF)
            {
                if (*sz != _T('\0'))
                    lstrcat(sz, _T(" | "));
                lstrcat(sz, _T("ENHMF"));
            }

            if (fmt.tymed == TYMED_NULL)
                lstrcat(sz, _T("ISTORAGE"));

            if (*sz == _T('\0'))
                wsprintf(sz, _T("%#08lX"), (DWORD)fmt.tymed ) ;
            lvi.pszText = sz ;
  
            m_FormatList.SetItem(&lvi);
            lvi.iItem++;  
        }

        pEFE->Release() ;
    }
    else
    {
        if (SUCCEEDED(hr) && pEFE == NULL)
            ErrorMessage(_T("IDataObject::EnumFormatEtc() returned S_OK, but *ppenumFormatetc is NULL!"), hr);
        else
            ErrorMessage(_T("IDataObject::EnumFormatEtc() failed."), hr);
    }

    CenterWindow() ;
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CUniformDataTransfer::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    if (m_FormatList.GetSafeHwnd())
    {
        CRect rc ;
        m_FormatList.GetWindowRect( &rc ) ;                                       
        ScreenToClient( &rc ) ;
        m_FormatList.SetWindowPos( NULL, rc.left, rc.top, cx-(2*rc.left), rc.bottom - rc.top, 
                            SWP_NOACTIVATE|SWP_NOZORDER ) ;
        CRect rcText ;
        m_RichText.GetWindowRect( &rcText ) ;                                       
        ScreenToClient( &rcText ) ;
        m_RichText.SetWindowPos( NULL, rcText.left, rcText.top, cx-(2*rcText.left), cy- rcText.top - rc.left, 
                            SWP_NOACTIVATE|SWP_NOZORDER ) ;
    }
	
}

void CUniformDataTransfer::OnDblclkDataFormats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if (pNMListView->lParam)
    {
        FORMATETC* pfmt = (FORMATETC*)pNMListView->lParam ;
        STGMEDIUM  stgm ;
        stgm.tymed = pfmt->tymed ;
        stgm.hGlobal = NULL ;
        stgm.pUnkForRelease = NULL;
        CString str ;
        HRESULT hr = S_OK;
        switch(pfmt->cfFormat)
        {
        case CF_TEXT:
        case CF_OEMTEXT:
            if (SUCCEEDED(hr = m_pdo->GetData(pfmt, &stgm)))
            {
                TCHAR* pstr = (TCHAR*)::GlobalLock(stgm.hGlobal);
                m_RichText.SetWindowText(pstr) ;
                ::GlobalUnlock(stgm.hGlobal);
                ReleaseStgMedium(&stgm);
            }
            break;

        case CF_UNICODETEXT:
            if (SUCCEEDED(hr = m_pdo->GetData(pfmt, &stgm)))
            {
                USES_CONVERSION;
                WCHAR* pstr = (WCHAR*)::GlobalLock(stgm.hGlobal);
                m_RichText.SetWindowText(W2A(pstr)) ; 
                ::GlobalUnlock(stgm.hGlobal);
                ReleaseStgMedium(&stgm);
            }
            break;

        case CF_BITMAP:
        case CF_DIB:
            {
            }
            break ;

        case CF_ENHMETAFILE:
        case CF_METAFILEPICT:
            {
            }
            break ;

            {
            }
            break ;

        case CF_PENDATA:
        case CF_SYLK:
        case CF_DIF:
        case CF_TIFF:
        case CF_RIFF:
        case CF_WAVE:
        case CF_HDROP:
        case CF_LOCALE:
        case CF_MAX:
        case CF_OWNERDISPLAY:
        case CF_DSPTEXT:
        case CF_DSPBITMAP:
        case CF_DSPMETAFILEPICT:
        case CF_DSPENHMETAFILE:
        default:
            str.Format(_T("The viewer does not understand how to display %s data"),  GetNameOfClipboardFormat(pfmt->cfFormat));
            m_RichText.SetWindowText(str);
        }

        if (FAILED(hr))
        {
            m_RichText.SetWindowText(_T(""));
            ErrorMessage(_T("IDataObject::GetData failed"), hr);
        }
    }
	*pResult = 0;
}

void CUniformDataTransfer::OnClickDataFormats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CUniformDataTransfer::OnColumnClickDataFormats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CUniformDataTransfer::OnDeleteItemDataFormats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if (pNMListView->lParam)
    {
        CLIPFORMAT* pfmt = (CLIPFORMAT*)pNMListView->lParam ;
        delete pfmt ;
        pNMListView->lParam = NULL ;
    }

	*pResult = 0;
}

LPTSTR CUniformDataTransfer::GetNameOfClipboardFormat(CLIPFORMAT cf)
{
    static TCHAR sz[256] ;
    switch( cf )
    {

        case CF_UNICODETEXT: lstrcpy( sz, _T( "CF_UNICODETEXT" ) ) ; break ;         
        case CF_ENHMETAFILE: lstrcpy( sz, _T( "CF_ENHMETAFILE" ) ) ; break ;  
        case CF_TEXT: lstrcpy( sz, _T( "CF_TEXT" ) ) ; break ;         
        case CF_BITMAP: lstrcpy( sz, _T( "CF_BITMAP" ) ) ; break ;
        case CF_METAFILEPICT: lstrcpy( sz, _T( "CF_METAFILEPICT" ) ) ; break ;  
        case CF_SYLK: lstrcpy( sz, _T( "CF_SYLK" ) ) ; break ;
        case CF_DIF: lstrcpy( sz, _T( "CF_DIF" ) ) ; break ;           
        case CF_TIFF: lstrcpy( sz, _T( "CF_TIFF" ) ) ; break ;          
        case CF_OEMTEXT: lstrcpy( sz, _T( "CF_OEMTEXT" ) ) ; break ;       
        case CF_DIB: lstrcpy( sz, _T( "CF_DIB" ) ) ; break ;           
        case CF_PALETTE: lstrcpy( sz, _T( "CF_PALETTE" ) ) ; break ;      
        case CF_PENDATA: lstrcpy( sz, _T( "CF_PENDATA" ) ) ; break ;         
        case CF_RIFF: lstrcpy( sz, _T( "CF_RIFF" ) ) ; break ;             
        case CF_WAVE: lstrcpy( sz, _T( "CF_WAVE" ) ) ; break ;            
        case 0: lstrcpy( sz, _T( "\"Wildcard Advise\"" ) ) ; break ;
        default:
            if (!GetClipboardFormatName( (UINT)cf, sz, 254 ))
                wsprintf( sz, _T( "%#08lX" ), (DWORD)cf ) ;
        break ;
    }

    return sz ;
}

void CUniformDataTransfer::OnItemChangedDataFormats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if (pNMListView->lParam && pNMListView->uNewState & LVIS_SELECTED)
    {
        FORMATETC* pfmt = (FORMATETC*)pNMListView->lParam ;
        STGMEDIUM  stgm ;
        stgm.tymed = pfmt->tymed ;
        stgm.hGlobal = NULL ;
        stgm.pUnkForRelease = NULL;
        CString str ;
        HRESULT hr = S_OK;
        if (FAILED(hr = m_pdo->QueryGetData(pfmt)))
        {
            m_RichText.SetWindowText(_T(""));
            ErrorMessage(_T("IDataObject::QueryGetData failed"), hr);
            return ;
        }

        switch(pfmt->cfFormat)
        {
        case CF_TEXT:
        case CF_OEMTEXT:
            if (SUCCEEDED(hr = m_pdo->GetData(pfmt, &stgm)))
            {
                TCHAR* pstr = (TCHAR*)::GlobalLock(stgm.hGlobal);
                m_RichText.SetWindowText(pstr) ;
                ::GlobalUnlock(stgm.hGlobal);
                ReleaseStgMedium(&stgm);
            }
            break;

        case CF_UNICODETEXT:
            if (SUCCEEDED(hr = m_pdo->GetData(pfmt, &stgm)))
            {
                USES_CONVERSION;
                WCHAR* pstr = (WCHAR*)::GlobalLock(stgm.hGlobal);
                m_RichText.SetWindowText(W2A(pstr)) ; 
                ::GlobalUnlock(stgm.hGlobal);
                ReleaseStgMedium(&stgm);
            }
            break;

        case CF_BITMAP:
        case CF_DIB:
            {
            }
            break ;
        case CF_ENHMETAFILE:
        case CF_METAFILEPICT:
            {
            }
            break ;

            {
            }
            break ;

        case CF_PENDATA:
        case CF_SYLK:
        case CF_DIF:
        case CF_TIFF:
        case CF_RIFF:
        case CF_WAVE:
        case CF_HDROP:
        case CF_LOCALE:
        case CF_MAX:
        case CF_OWNERDISPLAY:
        case CF_DSPTEXT:
        case CF_DSPBITMAP:
        case CF_DSPMETAFILEPICT:
        case CF_DSPENHMETAFILE:
        default:
            if ((pfmt->cfFormat == RegisterClipboardFormat(_T("Rich Text Format")) ||
                (pfmt->cfFormat == RegisterClipboardFormat(_T("Rich Text Format Without Objects"))) ||
                (pfmt->cfFormat == RegisterClipboardFormat(_T("RichEdit Text and Objects")))) &&
                (SUCCEEDED(hr = m_pdo->GetData(pfmt, &stgm))))
            {
                TCHAR* pstr = (TCHAR*)::GlobalLock(stgm.hGlobal);
                m_RichText.SetWindowText(pstr) ;
                ::GlobalUnlock(stgm.hGlobal);
                ReleaseStgMedium(&stgm);
            }
            else
            {
                str.Format(_T("The viewer does not understand how to display %s data"),  GetNameOfClipboardFormat(pfmt->cfFormat));
                m_RichText.SetWindowText(str);
            }
        }

        if (FAILED(hr))
        {
            m_RichText.SetWindowText(_T(""));
            ErrorMessage(_T("IDataObject::GetData failed"), hr);
        }
    }
	*pResult = 0;
}
