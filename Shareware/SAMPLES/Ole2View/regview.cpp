// regview.cpp : implementation file
//

#include "stdafx.h"
#include "ole2view.h"
#include "regview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL RegToListBox( CTreeCtrl* pTree, const CString& strKey, HKEY hk, HTREEITEM htreeParent ) ;
HICON GetIconFromReg( HKEY hk, LPTSTR szKey ) ;

int const MAX_KEYLEN    =   256 ;
int const MAX_VALLEN    =   (_MAX_PATH + MAX_KEYLEN) ;

class CRegEntry
{
public:
    CRegEntry( const CString& rstrKey,  const CString& rstrValue ) ;
    CRegEntry( const CString& rstrKey,  const CString& rstrValueName, const CString& rstrValue ) ;
    virtual ~CRegEntry() ;

public:
    CString     m_strKey ;
    CString     m_strValueName ;
    CString     m_strValue ;
    TCHAR*      m_pszText ;
} ;

CRegEntry::CRegEntry( const CString& rstrKey,  const CString& rstrValue )
{
    m_strKey = rstrKey ;
    m_strValueName = _T("");
    m_strValue = rstrValue ;
    m_pszText = NULL ;
}

CRegEntry::CRegEntry( const CString& rstrKey,  const CString& rstrValueName, const CString& rstrValue )
{
    m_strKey = rstrKey ;
    m_strValueName = rstrValueName ;
    m_strValue = rstrValue ;
    m_pszText = NULL ;
}

CRegEntry::~CRegEntry( )
{
    if (m_pszText != NULL)
        delete []m_pszText ;
    m_pszText = NULL ;
}

/////////////////////////////////////////////////////////////////////////////
// CRegistryView

IMPLEMENT_DYNCREATE(CRegistryView, CFormView)
#define new DEBUG_NEW

CRegistryView::CRegistryView()
	: CFormView(CRegistryView::IDD)
{
	//{{AFX_DATA_INIT(CRegistryView)
	//}}AFX_DATA_INIT
}

#ifdef _DEBUG

COle2ViewDoc* CRegistryView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COle2ViewDoc)));
    return (COle2ViewDoc*) m_pDocument;
}

#endif //_DEBUG


CRegistryView::~CRegistryView()
{
}

void CRegistryView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegistryView)
	DDX_Control(pDX, IDC_OBJECTNAME, m_Name);
	DDX_Control(pDX, IDC_OBJECTCLSID, m_CLSID);
	DDX_Control(pDX, IDC_OBJICON, m_icon);
	DDX_Control(pDX, IDC_TREEVIEW, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegistryView, CFormView)
	//{{AFX_MSG_MAP(CRegistryView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_UNREGISTER, OnUnregister)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREEVIEW, OnTreeViewItemExpanding)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREEVIEW, OnGetDispInfoTreeView)
	ON_NOTIFY(TVN_SETDISPINFO, IDC_TREEVIEW, OnSetDispInfoTreeView)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREEVIEW, OnDeleteItemTreeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CRegistryView diagnostics

#ifdef _DEBUG
void CRegistryView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRegistryView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRegistryView message handlers

void CRegistryView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

    if (m_tree.GetSafeHwnd())
    {
        CRect rc ;
        m_tree.GetWindowRect( &rc ) ;                                       
        ScreenToClient( &rc ) ;
        m_tree.SetWindowPos( NULL, 4, rc.top, cx-8, cy - rc.top - 4, 
                            SWP_NOACTIVATE|SWP_NOZORDER ) ;
    }
}

void CRegistryView::OnUnregister() 
{
}

void CRegistryView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

    CRect rc ;
    CRect rcClient ;
    GetWindowRect( &rcClient ) ;
    //GetParent()->ScreenToClient( &rcClient ) ;
    m_tree.GetWindowRect( &rc ) ;                                       
    ScreenToClient( &rc ) ;

    if (g_osvi.dwMajorVersion < 4)
    {
        m_tree.SetWindowPos( NULL, 4, rc.top, rcClient.Width() - 10, 
                             rcClient.Height() - rc.top - 6, 
                             SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOZORDER ) ;
    }
    else
    {
        m_tree.SetWindowPos( NULL, 4, rc.top, rcClient.Width() - 8, 
                             rcClient.Height() - rc.top - 4, 
                             SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOZORDER ) ;
    }
    m_tree.SetFocus() ;
    m_tree.SetIndent( 0 ) ;
    SetScrollSizes( MM_TEXT, CSize(1,1) ) ;
}

void CRegistryView::OnUpdate(CView* /* pSender */, LPARAM lHint, CObject* /*pHint*/)
{
    if (lHint & UPD_NOREGISTRYVIEW)
        return ;

    COle2ViewDoc* pDoc = GetDocument() ;
    HKEY    hk ;
       
    BeginWaitCursor() ; 
    m_tree.SetRedraw(FALSE) ;
    m_tree.DeleteAllItems() ;
    if (IsEqualCLSID( pDoc->m_clsidCur, CLSID_NULL ))
    {
    	if (pDoc->m_nType == CObjectData::typeObjectInstance)
            m_CLSID.SetWindowText(_T("No CLSID available.")) ;
        else
            m_CLSID.SetWindowText(_T("")) ;
        m_icon.SetIcon( theApp.LoadIcon( IDR_MAINFRAME ) ) ;
        m_Name.SetWindowText(pDoc->m_szObjectCur) ;

        /*
           [[ Because we display all sub-keys of every key it takes a loooonnnggg
              time to view big parts of the registry.  This is disabled because it
              makes things way too slow. ]]
        if (pDoc->m_fTypeLib)
        {
            if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("TypeLib"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
            {
				RegToListBox( &m_tree, _T("TypeLib"), hk, TVI_ROOT) ;
                //m_tree.Expand( TVI_ROOT, TVE_EXPAND ) ;
                m_Name.SetWindowText(_T("All HKEY_CLASSES_ROOT\\TypeLib Entries")) ;
				m_icon.SetIcon( theApp.LoadIcon( IDI_AUTOMATION ) ) ;

				RegCloseKey( hk ) ;
            }
        }
        else if (pDoc->m_fInterface)
        {
            if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("Interface"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
            {
				RegToListBox( &m_tree, _T("Interface"), hk, TVI_ROOT) ;
                //m_tree.Expand( TVI_ROOT, TVE_EXPAND ) ;
                m_Name.SetWindowText(_T("All HKEY_CLASSES_ROOT\\Interface Entries")) ;
				m_icon.SetIcon( theApp.LoadIcon( IDI_INTERFACE ) ) ;

				RegCloseKey( hk ) ;
            }
        }
        else
        {
            if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("CLSID"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
            {
				RegToListBox( &m_tree, _T("CLSID"), hk, TVI_ROOT) ;
                //m_tree.Expand( TVI_ROOT, TVE_EXPAND ) ;
                m_Name.SetWindowText(_T("All HKEY_CLASSES_ROOT\\CLSID Entries")) ;
				m_icon.SetIcon( theApp.LoadIcon( IDR_MAINFRAME ) ) ;

				RegCloseKey( hk ) ;
            }
        }
        */

        if (pDoc->m_nType == CObjectData::typeCategories)
        {
            if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("Component Categories"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
            {
				RegToListBox( &m_tree, _T("Component Categories"), hk, TVI_ROOT) ;
                //m_tree.Expand( TVI_ROOT, TVE_EXPAND ) ;
                m_Name.SetWindowText(_T("All HKEY_CLASSES_ROOT\\Component Categories Entries")) ;
				m_icon.SetIcon( theApp.LoadIcon( IDR_MAINFRAME ) ) ;

				RegCloseKey( hk ) ;
            }
        }

        m_tree.SetRedraw(TRUE) ;
        EndWaitCursor() ;
        return ;
    }

    // Fill the listbox with useful info about the class
    //              
    HICON   hicon = NULL ;
    ULONG	cb ;

	USES_CONVERSION;
	OLECHAR szOleCLSID[64];
	::StringFromGUID2( pDoc->m_clsidCur, szOleCLSID, 
		sizeof(szOleCLSID)/sizeof(OLECHAR) ) ;
	LPTSTR lpszCLSID = OLE2T(szOleCLSID);
    m_CLSID.SetWindowText( lpszCLSID ) ;
    m_Name.SetWindowText( pDoc->m_szObjectCur ) ;

    TCHAR           szKey[MAX_VALLEN] ;
    TCHAR           szBuf[MAX_VALLEN] ;

    TV_INSERTSTRUCT tvis ;
    tvis.hParent = TVI_ROOT ;
    tvis.hInsertAfter = TVI_LAST ;
    tvis.item.stateMask = NULL ;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM ;

	// if it is an Component Category then show all the Component Category keys
	//
	if (pDoc->m_nType == CObjectData::typeCategory)
	{
	    m_icon.SetIcon( theApp.LoadIcon( IDR_MAINFRAME ) ) ;

        // HKEY_CLASSES_ROOT\Component Categories\ 
        if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("Component Categories"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
        {                                                
			// TypeLib = OLE (Part 4 of 5)
            cb = sizeof(szBuf) ;
			*szBuf = _T('\0') ;
            RegQueryValueEx( hk, NULL, NULL, NULL, (LPBYTE)szBuf,  &cb) ;
            tvis.hParent = TVI_ROOT ;
			if (*szBuf != '\0')
				wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("Component Categories"), (LPCTSTR)szBuf ) ;
			else
				wsprintf(szKey, _T("%s"), (LPCTSTR)_T("Component Categories") ) ;
            tvis.item.pszText = szKey ;
            tvis.item.lParam = (LPARAM)new CRegEntry( _T("Component Categories"), szBuf ) ;
            HTREEITEM htree = m_tree.InsertItem( &tvis ) ; 
			RegCloseKey( hk ) ;

			//   {catlib id} = Name
			//      ...
            wsprintf( szBuf, _T("Component Categories\\%s"), lpszCLSID ) ;
			if (RegOpenKeyEx( HKEY_CLASSES_ROOT, szBuf, NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
			{                                                
				RegToListBox( &m_tree, lpszCLSID, hk, htree ) ;
				m_tree.Expand( htree, TVE_EXPAND ) ;
				RegCloseKey( hk ) ;
			}
//            goto lblDoCLSID;
        }
	}
	// If a typelib is selected show all registry info we can about
	// that typelib (just the keys under the typelib).
	//
    else if (pDoc->m_nType == CObjectData::typeTypeLib)
    {
	    m_icon.SetIcon( theApp.LoadIcon( IDI_NOICON ) ) ;

        // HKEY_CLASSES_ROOT\TypeLib\ 
        if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("TypeLib"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
        {                                                
			// TypeLib = OLE (Part 4 of 5)
            cb = sizeof(szBuf) ;
			*szBuf = _T('\0') ;
            RegQueryValueEx( hk, NULL, NULL, NULL, (LPBYTE)szBuf,  &cb) ;
            tvis.hParent = TVI_ROOT ;
			if (*szBuf)
				wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("TypeLib"), (LPCTSTR)szBuf ) ;
			else
				wsprintf(szKey, _T("%s"), (LPCTSTR)_T("TypeLib")) ;
            tvis.item.pszText = szKey ;
            tvis.item.lParam = (LPARAM)new CRegEntry( _T("TypeLib"), szBuf ) ;
            HTREEITEM htree = m_tree.InsertItem( &tvis ) ; 
			RegCloseKey( hk ) ;

			//   {typelib id} = Name
			//      ...
            wsprintf( szBuf, _T("TypeLib\\%s"), lpszCLSID ) ;
			if (RegOpenKeyEx( HKEY_CLASSES_ROOT, szBuf, NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
			{                                                
				RegToListBox( &m_tree, lpszCLSID, hk, htree ) ;
				m_tree.Expand( htree, TVE_EXPAND ) ;
				m_icon.SetIcon( theApp.LoadIcon( IDI_AUTOMATION ) ) ;

				RegCloseKey( hk ) ;
			}
        }
    } 
	
	// If an interface is selected show all the registry info we can
	// for that interface.  This includes the interface key entries
	// as well as the CLISD keys for the ProxyStubClsid
	//
    else if (pDoc->m_nType == CObjectData::typeInterface || pDoc->m_nType == CObjectData::typeStaticInterface )
    {
        // HKEY_CLASSES_ROOT\Interface\ 
        if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("Interface"), NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
        {               
			// Interface = OLE (Part 5 of 5)
			cb = sizeof(szBuf);
			*szBuf = _T('\0') ;
            RegQueryValueEx( hk, NULL, NULL, NULL, (LPBYTE)szBuf, &cb) ;
            tvis.hParent = TVI_ROOT ;
            wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("Interface"), (LPCTSTR)szBuf ) ;
            tvis.item.pszText = szKey ;
            tvis.item.lParam = (LPARAM)new CRegEntry( _T("Interface"), szBuf ) ;
            HTREEITEM htree = m_tree.InsertItem( &tvis ) ; 
			RegCloseKey(hk);

			//    {interface id} = IName
			//       ....
	        wsprintf( szBuf, _T("Interface\\%s"), lpszCLSID ) ;
			if (RegOpenKeyEx( HKEY_CLASSES_ROOT, szBuf, NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
			{                                                
				RegToListBox( &m_tree, lpszCLSID, hk, htree ) ;
				m_tree.Expand( htree, TVE_EXPAND ) ;

				// If there is a base interface then display it as well.  We only nest
				// one level deep by design.
				//
				//     {interface id} = IName
				cb = sizeof(szBuf) ;
				if (RegQueryValue( hk, (LPTSTR)_T("BaseInterface"), szBuf, (LONG*)&cb) == ERROR_SUCCESS &&
					*szBuf != '\0')
				{
					HKEY hk2 ; 
					if (RegOpenKeyEx( HKEY_CLASSES_ROOT, CString("Interface\\") + szBuf, NULL, KEY_READ, &hk2) == ERROR_SUCCESS) 
					{
						RegToListBox( &m_tree, szBuf, hk2, htree ) ;
                        //m_tree.Expand( htree, TVE_EXPAND ) ;
						RegCloseKey( hk2 ) ;
					}
				}
        
				// If there is a proxystubclsid then show it too.  
				cb = sizeof(szBuf) ;
				if (RegQueryValue( hk, (LPTSTR)_T("ProxyStubClsid32"), szBuf, (LONG*)&cb) == ERROR_SUCCESS ||
					RegQueryValue( hk, (LPTSTR)_T("ProxyStubClsid"), szBuf, (LONG*)&cb) == ERROR_SUCCESS)
				{
					// CLSID = OLE (Part 1 of 5)
					cb = sizeof(szBuf) ;
					*szBuf = _T('\0') ;
					RegQueryValue( HKEY_CLASSES_ROOT, (LPTSTR)_T("CLSID"), szBuf, (LONG*)&cb) ;
					wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("CLSID"), (LPCTSTR)szBuf ) ;
					tvis.item.pszText = szKey ;
					tvis.item.lParam = (LPARAM)new CRegEntry( _T("CLSID"), szBuf ) ;
					HTREEITEM htreeCLSID = m_tree.InsertItem( &tvis ) ;

					HKEY hk2 ; 
					if (RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("CLSID"), NULL, KEY_READ, &hk2) == ERROR_SUCCESS) 
					{
						cb = sizeof(szBuf) ;
						if (RegQueryValue( hk, (LPTSTR)_T("ProxyStubClsid32"), szBuf, (LONG*)&cb) == ERROR_SUCCESS)
						{
							HKEY hk3 ;
							if (RegOpenKeyEx( hk2, szBuf, NULL, KEY_READ, &hk3) == ERROR_SUCCESS) 
                            {
								RegToListBox( &m_tree, szBuf, hk3, htreeCLSID ) ;
                                //m_tree.Expand( htreeCLSID, TVE_EXPAND ) ;
                            }
							RegCloseKey( hk3 ) ;
						}

						cb = sizeof(szKey) ;
						if (RegQueryValue( hk, (LPTSTR)_T("ProxyStubClsid"), szKey, (LONG*)&cb) == ERROR_SUCCESS)
						{
							if (lstrcmpi(szKey,szBuf) != 0)
							{
								HKEY hk3 ;
								if (RegOpenKeyEx( hk2, szKey, NULL, KEY_READ, &hk3) == ERROR_SUCCESS) 
                                {
									RegToListBox( &m_tree, szKey, hk3, htreeCLSID ) ;
                                    //m_tree.Expand( htreeCLSID, TVE_EXPAND ) ;
                                }
								RegCloseKey( hk3 ) ;
							}
						}

						RegCloseKey( hk2 ) ;
					}
					m_tree.Expand( htreeCLSID, TVE_EXPAND ) ;
				}
			    // Now see if there is a typelib for this interface.
			    //
                TCHAR szTypeLibID[64];
                cb = sizeof(szTypeLibID) ;
                if (RegQueryValue( hk, (LPTSTR)_T("TypeLib"), szTypeLibID, (LONG*)&cb) == ERROR_SUCCESS)
                {
					// CLSID = OLE (Part 1 of 5)
					cb = sizeof(szBuf) ;
					*szBuf = _T('\0') ;
					RegQueryValue( HKEY_CLASSES_ROOT, (LPTSTR)_T("TypeLib"), szBuf, (LONG*)&cb) ;
                    wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("TypeLib"), (LPCTSTR)szBuf ) ;
                    tvis.item.pszText = szKey ;
                    tvis.item.lParam = (LPARAM)new CRegEntry( _T("TypeLib"), szBuf ) ;
                    htree = m_tree.InsertItem( &tvis ) ;

                    HKEY hk2 ; 
                    if (RegOpenKeyEx( HKEY_CLASSES_ROOT, CString("TypeLib\\") + szTypeLibID, NULL, KEY_READ, &hk2) == ERROR_SUCCESS) 
                    {
                        RegToListBox( &m_tree, szTypeLibID, hk2, htree ) ;
                        RegCloseKey( hk2 ) ;
                    }
                    m_tree.Expand( htree, TVE_EXPAND ) ;
                }
                RegCloseKey( hk ) ;
			}
            m_icon.SetIcon( theApp.LoadIcon( IDI_INTERFACE ) ) ;
        }
        else
            m_icon.SetIcon( theApp.LoadIcon( IDI_NOICON ) ) ;
    }

	// If it's not an interface or a typelib it must be a class
	//
    else
    {
//lblDoCLSID:
		// CLSID = OLE (Part 1 of 5)
        LONG cb = sizeof( szBuf );
		*szBuf = _T('\0') ;
        RegQueryValue( HKEY_CLASSES_ROOT, (LPTSTR)_T("CLSID"), szBuf, &cb) ;
        wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("CLSID"), (LPCTSTR)szBuf ) ;
        tvis.item.pszText = szKey ;
        tvis.item.lParam = (LPARAM)new CRegEntry( _T("CLSID"), szBuf) ;
        HTREEITEM htreeCLSID = m_tree.InsertItem( &tvis ) ; 

		// Figure out what icon to display.
        wsprintf( szKey, _T("CLSID\\%s"), lpszCLSID ) ;                           
        if (RegOpenKeyEx( HKEY_CLASSES_ROOT, szKey, NULL, KEY_READ, &hk) == ERROR_SUCCESS) 
        {
            TCHAR    szValue[MAX_VALLEN] ;
        
            cb = sizeof( szBuf );
            if (RegQueryValue( hk, (LPTSTR)_T("DefaultIcon"), szValue, &cb) == ERROR_SUCCESS)
            {
                LPTSTR p ;
                int   nIndex = 0 ;
            
                p = PointerToNthField( szValue, 2, ',' ) ;
                if (p && *p)
                {
                    *(p-1) = '\0' ;
                    nIndex = (int)*p - (int)'0' ;
                }                
                if (*szValue == '"')
                {
                    lstrcpy( szValue, szValue+1 ) ;
                    p = strrchr( szValue, '"' ) ;
                    if (p) *p = '\0' ;
                }
                hicon = ExtractIcon( AfxGetInstanceHandle(), szValue, nIndex ) ;
            }
        
            if ((int)hicon <= 1)
            {
                // try local server
                hicon = GetIconFromReg( hk, _T("LocalServer32") ) ;
                if ((int)hicon <= 1)
                    hicon = GetIconFromReg( hk, _T("LocalServer") ) ;
            }
        
            if ((int)hicon <= 1)
            {
                // try inproc server
                hicon = GetIconFromReg( hk, _T("InprocServer32") ) ;
                if ((int)hicon <= 1)
                    hicon = GetIconFromReg( hk, _T("InprocServer") ) ;
            }

            if ((int)hicon <= 1)
            {
                // try inproc handler
                hicon = GetIconFromReg( hk, _T("InprocHandler32") ) ;
                if ((int)hicon <= 1)
                    hicon = GetIconFromReg( hk, _T("InprocHandler") ) ;
            }
        
            // Now fill list box with all other stuff in the HKEY_CLASSES_ROOT\CLSID key
            //
            RegToListBox( &m_tree, lpszCLSID, hk, htreeCLSID ) ;
            m_tree.Expand( htreeCLSID, TVE_EXPAND ) ;

            HKEY hkNext ;     
                                                   
            // ProgID = Name
            cb = sizeof( szValue ) ;
            if (RegQueryValue( hk, (LPTSTR)_T("ProgID"), szValue, &cb) != ERROR_SUCCESS)
                *szValue = '\0' ;
            if (*szValue && (RegOpenKeyEx( HKEY_CLASSES_ROOT, szValue, NULL, KEY_READ, &hkNext) == ERROR_SUCCESS)) 
            {                                                
                cb = sizeof(szKey) ;
                RegToListBox( &m_tree, szValue, hkNext, TVI_ROOT ) ;
                //m_tree.Expand( TVI_ROOT, TVE_EXPAND ) ;
            
                if ((int)hicon <= 1)
                {           
                    szBuf[0] = '\0' ;
                    cb = sizeof(szBuf) ;
                    if (RegQueryValue( hkNext, (LPTSTR)_T("Protocol\\StdFileEditing\\Server"), szBuf, &cb ) == ERROR_SUCCESS)
                    {
                        LPTSTR p ;
                        if (*szBuf == '"')
                        {
                            lstrcpy( szBuf, szBuf+1 ) ;
                            p = strrchr( szBuf, '"' ) ;
                            if (p) *p = '\0' ;
                        }

                        hicon = ExtractIcon( AfxGetInstanceHandle(), szBuf, 0 ) ;
                    }
                
                }
            
                RegCloseKey( hkNext ) ;
            }

			// FileType = OLE (Part 4 of 5)
            wsprintf( szBuf, _T("FileType\\%s"), lpszCLSID ) ;
            if (lpszCLSID && (RegOpenKeyEx( HKEY_CLASSES_ROOT, szBuf, NULL, KEY_READ, &hkNext) == ERROR_SUCCESS)) 
            {                                                
                tvis.hParent = TVI_ROOT ;
				*szBuf = _T('\0') ;
                cb = sizeof(szBuf) ;
                RegQueryValue( HKEY_CLASSES_ROOT, (LPTSTR)_T("FileType"), szBuf, &cb) ;
                wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("FileType"), (LPCTSTR)szBuf ) ;
                tvis.item.pszText = szKey ;
                tvis.item.lParam = (LPARAM)new CRegEntry( _T("FileType"), szBuf) ;
                HTREEITEM htree = m_tree.InsertItem( &tvis ) ; 
                RegToListBox( &m_tree, lpszCLSID, hkNext, htree ) ;
                m_tree.Expand( htree, TVE_EXPAND ) ;
                RegCloseKey( hkNext ) ;
            }

            // TypeLib = OLE (Part 3 of 5)
            cb = sizeof( szValue ) ;
            if (RegQueryValue( hk, (LPTSTR)_T("TypeLib"), szValue, &cb) == ERROR_SUCCESS)
            {
                wsprintf( szBuf, _T("TypeLib\\%s"), szValue ) ;
                if (RegOpenKeyEx( HKEY_CLASSES_ROOT, szBuf, NULL, KEY_READ, &hkNext) == ERROR_SUCCESS) 
                {                                                
                    tvis.hParent = TVI_ROOT ;
					*szBuf = _T('\0') ;
                    cb = sizeof(szBuf) ;
                    RegQueryValue( HKEY_CLASSES_ROOT, (LPTSTR)_T("TypeLib"), szBuf, &cb) ;
                    wsprintf(szKey, _T("%s = %s"), (LPCTSTR)_T("TypeLib"), (LPCTSTR)szBuf ) ;
                    tvis.item.pszText = szKey ;
                    tvis.item.lParam = (LPARAM)new CRegEntry( "TypeLib", szBuf) ;
                    HTREEITEM htree = m_tree.InsertItem( &tvis ) ; 
                    RegToListBox( &m_tree, szValue, hkNext, htree ) ;
                    m_tree.Expand( htree, TVE_EXPAND ) ;
                    RegCloseKey( hkNext ) ;
                }
            }

            RegCloseKey( hk ) ;

            if ((int)hicon > 1)
                m_icon.SetIcon( hicon ) ;       
            else
                m_icon.SetIcon( theApp.LoadIcon( IDI_NOICON ) ) ;

            m_tree.EnsureVisible( htreeCLSID ) ;
        }
    }

    m_tree.SetRedraw(TRUE) ;
    EndWaitCursor() ;

}

// This routine inserts into the tree view the tree crated by enumarating
// hk. It first inserts the key associated with strKey as a child of htreeParent
// and then inserts the children of strKey as children of that tree.
//
BOOL RegToListBox( CTreeCtrl* pTree, const CString& strKey, HKEY hk, HTREEITEM htreeParent ) 
{
    TCHAR* szKey ;
    TCHAR* szValueName ;
    BYTE*  rgbValueData;
    TCHAR* szValue;
    TCHAR* szBuf;
    DWORD cb ;
    DWORD cb2 ;
    DWORD dwIndex ;   
    HTREEITEM htree ;

    DWORD cchMaxSubKey;
    DWORD cchMaxClass ;
    DWORD cValues ;
    DWORD cchMaxValueName ;
    DWORD cbMaxValueData ;
    DWORD dwType;

#ifndef _MAC
    if (RegQueryInfoKey(hk, NULL, NULL, NULL, NULL, 
            &cchMaxSubKey, &cchMaxClass, &cValues, &cchMaxValueName, &cbMaxValueData, NULL, NULL) != ERROR_SUCCESS)
        return FALSE ;
#else
    cchMaxSubKey = 255 ;
    cchMaxClass = 0 ;
    cValues = 1 ;
    cchMaxValueName = 255 ;
    cbMaxValueData = 255 ;
#endif

    szValueName = new TCHAR[cchMaxValueName+1] ;
    rgbValueData = new BYTE[cbMaxValueData] ;

	// BUGBUG: Not checking for memory allocation errors.  Unlikely, but still bad practice...
	//
    // Big enough to hold binary representation (00 03 BA...)
    szValue = new TCHAR[(cbMaxValueData * (3 * sizeof(TCHAR)))+1+64] ;  // +64 for REG_DWORD
    szBuf = new TCHAR[strKey.GetLength() + 6 + cchMaxSubKey + cchMaxValueName + (cbMaxValueData * (3 * sizeof(TCHAR)))+1+64] ;
	
    TV_INSERTSTRUCT tvis ;
    tvis.hParent = htreeParent ;
    tvis.hInsertAfter = TVI_LAST ;
    tvis.item.stateMask = NULL ;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM ;

	// Registry keys can have multiple named values.  And values may be
    // of a type other than REG_SZ
    //
    cb = cbMaxValueData ;
    cb2 = cchMaxValueName + 1;

    // Start the enumeration
    DWORD iValue = 0 ;
    LONG lErr = RegEnumValue( hk, iValue, szValueName, &cb2, NULL, &dwType, rgbValueData, &cb) ;
    if (lErr == ERROR_SUCCESS || lErr == ERROR_NO_MORE_ITEMS) 
    do
    {
        if (lErr != ERROR_NO_MORE_ITEMS) // There is no value, just a key
        {
            // coerce to a string
            switch (dwType)
			{
			case REG_DWORD_BIG_ENDIAN:
                wsprintf(szValue, _T("%#04X%04X (%lu)"), LOWORD(*((DWORD*)rgbValueData)), HIWORD(*((DWORD*)rgbValueData)), *((DWORD*)rgbValueData)) ;
			break ;

            case REG_DWORD_LITTLE_ENDIAN:
                wsprintf(szValue, _T("%#08X (%lu)"), *((DWORD*)rgbValueData), *((DWORD*)rgbValueData)) ;
			break ;

			case REG_SZ:
			case REG_EXPAND_SZ:
                lstrcpy(szValue, (LPCTSTR)rgbValueData);
			break ;

            case REG_MULTI_SZ:
            {
                // REG_MULTI_SZ is a doubly NULL terminated list of NULL 
                // terminated strings.
                TCHAR* s = (TCHAR*)rgbValueData ;
                TCHAR* d = szValue ;
                while (*s || *(s+1))
                {
                    if (*s)
                        *d = *s ;
                    else 
                        *d = _T(' ') ;
                    d++ ; s++ ;
                }
                *d = '\0' ;
            }
			break ;

			case REG_BINARY:
			{
                // Convert rgbValueData to a HEX string
				memset(szValue, '\0', (cbMaxValueData * (3 * sizeof(TCHAR)))+1);
				for (DWORD i = 0 ; i < cb ; i++)
				{
                    if (rgbValueData[i] < 0x10)
                    {
                        szValue[i*3] = _T('0') ;
                        _itoa( rgbValueData[i], &szValue[i*3+1], 16 );
                    }
                    else
                        _itoa( rgbValueData[i], &szValue[i*3], 16 );

					if (i+1 < cb)
						szValue[i*3+2] = _T(' '); 
				}
			}
			break ;
			
			default:
				lstrcpy(szValue, "<cannot coerce data to string>");
			}

            // Format display string
            if (cValues == 1 && *szValueName == NULL)
                wsprintf( szBuf, _T("%s = %s"), (LPCTSTR)strKey, (LPCTSTR)szValue ) ;
            else
            {
                if (*szValueName == NULL)
                    wsprintf( szBuf, _T("%s [<no name>] = %s"), (LPCSTR)strKey, (LPCTSTR)szValue );
                else
                    wsprintf( szBuf, _T("%s [%s] = %s"), (LPCSTR)strKey, (LPCTSTR)szValueName, (LPCTSTR)szValue );
            }
        }
        else
        {
            *szValueName = _T('\0') ;
            *szValue = _T('\0') ;
            lstrcpy(szBuf, strKey) ;
        }

        // Add to tree view
        tvis.item.pszText = szBuf ;
        tvis.item.lParam = (LPARAM)new CRegEntry( strKey, szValueName, szValue ) ;

        // We call RegToListBox with this htree below.  This ensures that the sub
        // keys are all rooted on the last named value.
        htree = pTree->InsertItem( &tvis ) ;

        // Continue the enumeration
        iValue++ ;
        cb = cbMaxValueData ;
        cb2 = cchMaxValueName + 1;
        lErr = RegEnumValue( hk, iValue, szValueName, &cb2, NULL, &dwType, rgbValueData, &cb) ;
    } while (lErr == ERROR_SUCCESS);

    delete []szValueName; szValueName = NULL;
    delete []rgbValueData; rgbValueData = NULL;
    delete []szValue; szValue = NULL;
    delete []szBuf; szBuf = NULL;

    // For each sub-key of this key recurse
    //
    szKey = new TCHAR[cchMaxSubKey+1] ;
    for ( dwIndex = 0 ; 
          RegEnumKey( hk, dwIndex, szKey, cchMaxSubKey+1) == ERROR_SUCCESS ; 
          ++dwIndex ) 
    {
        HKEY hkNext ;                                                
        if (RegOpenKeyEx( hk, szKey, NULL, KEY_READ, &hkNext) == ERROR_SUCCESS) 
        {
            RegToListBox( pTree, szKey, hkNext, htree ) ;
            pTree->Expand( htree, TVE_EXPAND ) ;
            RegCloseKey( hkNext ) ;
        }
    }
    delete []szKey ;

    return TRUE ;
}

HICON GetIconFromReg( HKEY hk, LPTSTR szKey )
{
    TCHAR szValue[MAX_VALLEN] ;
    LONG cb = MAX_VALLEN ;
    if (RegQueryValue( hk, szKey, szValue, &cb) == ERROR_SUCCESS)
    {
        LPTSTR p = szValue + lstrlen( szValue ) - 1 ;
        if (p && *p == ' ')
            *p = '\0' ;
        if (*szValue == '"')
        {
            lstrcpy( szValue, szValue+1 ) ;
            p = strrchr( szValue, '"' ) ;
            if (p) *p = '\0' ;
        }

        return ExtractIcon( AfxGetInstanceHandle(), szValue, 0 ) ;
    }
    else
        return (HICON)-1 ;
}

void CRegistryView::OnTreeViewItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    if (pNMTreeView->action == TVE_COLLAPSE)
    	*pResult = 1 ; // prevent collapse
    else
	    *pResult = 0 ; 
}

void CRegistryView::OnGetDispInfoTreeView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO * pdispinfo = (TV_DISPINFO*)pNMHDR;

/*
    CRegEntry* pentry = (CRegEntry*)pdispinfo->item.lParam ;
    if (pentry)
    {
        //if (pentry->m_pszText)
        //{
        //    delete []pentry->m_pszText ;
        //    pentry->m_pszText = NULL ;
        //}
        if (pdispinfo->item.mask & TVIF_TEXT)
        {
            if (pentry->m_pszText)
                pdispinfo->item.pszText = pentry->m_pszText ;
            else
            {
                pentry->m_pszText = new TCHAR[pentry->m_strKey.GetLength() + pentry->m_strKey.GetLength() + 4] ;
                wsprintf( pentry->m_pszText, _T("%s = %s"), (LPCTSTR)pentry->m_strKey, (LPCTSTR)pentry->m_strValue ) ;
                pdispinfo->item.pszText = pentry->m_pszText ;
                pdispinfo->item.mask = TVIF_TEXT ;
            }
        }
    }
*/	
	*pResult = 0;
}

void CRegistryView::OnSetDispInfoTreeView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO * pdispinfo = (TV_DISPINFO*)pNMHDR;
	
	*pResult = 0;
}

void CRegistryView::OnDeleteItemTreeView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    if (pNMTreeView->itemOld.lParam)
    {
        delete (CRegEntry*)pNMTreeView->itemOld.lParam;
    }
    	
	*pResult = 0;
}

