// obj_vw.cpp
//
// Implementation of the CObjTreeView class
//

#include "stdafx.h"
#include "Ole2View.h"
#include "obj_vw.h"
#include "ServerInfoDlg.h"
#include <winnls.h>

#define _SHOW_32BIT_ICONS_
#define _SHOW_TOOLBOXBITMAPS

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CObjTreeView, CTreeView)
IMPLEMENT_DYNCREATE(CObjectData, CObject)
#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CObjTreeView, CTreeView)
    //{{AFX_MSG_MAP(CObjTreeView)
    ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_OBJECT_DELETE, OnObjectDelete)
	ON_COMMAND(ID_OBJECT_VERIFY, OnObjectVerify)
	ON_COMMAND(ID_FILE_BINDTOAFILE, OnFileBind)
	ON_COMMAND(ID_OBJECT_RELEASE, OnObjectRelease)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_RELEASE, OnUpdateObjectRelease)
	ON_COMMAND(ID_OBJECT_CREATE, OnObjectCreate)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_CREATE, OnUpdateObjectCreate)
	ON_COMMAND(ID_OBJECT_VIEW, OnObjectView)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_VIEW, OnUpdateObjectView)
	ON_COMMAND(ID_VIEW_HIDDENCOMCATS, OnViewHiddenComCats)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDDENCOMCATS, OnUpdateViewHiddenComCats)
	ON_COMMAND(ID_OBJECT_CREATE_ON, OnObjectCreateOn)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_CREATE_ON, OnUpdateObjectCreateOn)
	ON_COMMAND(ID_OBJECT_ATSTORAGE, OnObjectAtStorage)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_ATSTORAGE, OnUpdateObjectAtStorage)
	//}}AFX_MSG_MAP
    ON_MESSAGE( WM_COMMANDHELP, OnCommandHelp)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED,OnTreeSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnTreeItemExpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED,OnTreeItemExpanded)
    ON_NOTIFY_REFLECT(TVN_DELETEITEM,OnTreeDeleteItem)
	ON_NOTIFY_REFLECT(NM_RETURN, OnTreeReturn)
    ON_NOTIFY_REFLECT(NM_RCLICK, OnTreeRClick)
END_MESSAGE_MAP()

// This is a global linked list of HTREEITEMs that is
// used by a 2nd thread to update toolbox bitmaps in the
// tree.
//         
CCriticalSection *g_pCSDeferredList=NULL;
CCriticalSection *g_pCSImages =NULL;
CWinThread*       g_pDeferredThread;
CPtrList          g_DeferredList;
BOOL              g_fExitDeferredThread=FALSE;

UINT DeferredToolBoxUpdates( LPVOID pParam )
{
    // If exit thread is set then exit
    if (g_fExitDeferredThread)
        return 0;

    CObjTreeView* pView = (CObjTreeView*) pParam;

    if (pView == NULL ||
        !pView->IsKindOf(RUNTIME_CLASS(CObjTreeView)))
    return (UINT)-1;	// illegal parameter

    CObjectData* pOD ;
    TV_ITEM tvi ;

    while(1)
    {
        // If exit thread is set then exit
        if (g_fExitDeferredThread || g_pCSDeferredList==NULL||g_pCSImages ==NULL||g_pDeferredThread==NULL)
        {
            //TRACE("Exiting Thread.\n");
            return 0;
        }

        tvi.hItem = NULL;

        ASSERT(g_pCSDeferredList);
        // Protect global
        if (g_pCSDeferredList==NULL)
            return 0;

        g_pCSDeferredList->Lock(0);
        if (g_DeferredList.IsEmpty() == FALSE)
            tvi.hItem = (HTREEITEM)g_DeferredList.RemoveHead();
        g_pCSDeferredList->Unlock();

        if (tvi.hItem == NULL)
        {
            if (g_pDeferredThread==NULL)
                return 0;
            // suspend thread.  It will get resumed when
            // items are added to the list
            //TRACE("Suspended...");
            g_pDeferredThread->SuspendThread();

            // Try again
            continue ;
        }

        while (tvi.hItem != NULL)
        {
            // If exit thread is set then exit
            if (g_fExitDeferredThread)
            {
                //TRACE("Exiting Thread.\n");
                return 0;
            }

            pOD=NULL;

            tvi.mask = TVIF_HANDLE | TVIF_PARAM;
            if (tvi.hItem && pView->GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
                pOD = (CObjectData*)tvi.lParam ;
           
            if (pOD==NULL)
            {
                ASSERT(0);
                continue;
            }

            if (!pOD->IsKindOf(RUNTIME_CLASS(CObjectData)))
            {
                ASSERT(0);
                continue;
            }

            TCHAR* szBuf = pOD->m_strToolBoxBitmap.GetBuffer(_MAX_PATH*2);
            if (szBuf == NULL)
                continue;

            TCHAR * pszBitmapModule = _tcstok(szBuf, _T(","));
            TCHAR * pszBitmapId = _tcstok(NULL, _T(","));
            HINSTANCE hinstBitmap;
            int nBitmapId;
            if ((pszBitmapModule != NULL) && (pszBitmapId != NULL))
            {
                LPTSTR p ;
                if (*pszBitmapModule == '"')
                {
                    lstrcpy( pszBitmapModule, pszBitmapModule+1 ) ;
                    p = strrchr( pszBitmapModule, '"' ) ;
                    if (p) *p = '\0' ;
                }
			    nBitmapId = _ttoi(pszBitmapId);
                if (g_osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
				    hinstBitmap = LoadLibrary(pszBitmapModule);
                else
				    hinstBitmap = LoadLibraryEx(pszBitmapModule, NULL, LOAD_LIBRARY_AS_DATAFILE);
                if (hinstBitmap && (nBitmapId > 0))
                {
                    HBITMAP hbmGlyph = ::LoadBitmap(hinstBitmap, MAKEINTRESOURCE(nBitmapId));
                    if (hbmGlyph)
                    {                                     
                        // Protect global
                        pOD->m_uiBitmap = g_pImages->Add( CBitmap::FromHandle(hbmGlyph), RGB(192,192,192) /*RGBLTGRAY*/ ) ;
                        ::DeleteObject(hbmGlyph) ;

                        tvi.iImage = tvi.iSelectedImage = pOD->m_uiBitmap ;
                        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE ;
                        pView->GetTreeCtrl().SetItem(&tvi) ;
                        //TRACE("Set images\n");
                    }
                    FreeLibrary( hinstBitmap ) ;
                }
            }

            if (g_pCSDeferredList==NULL)
                return 0;
            // Protect global
            g_pCSDeferredList->Lock(0);
            if (!g_DeferredList.IsEmpty())
                tvi.hItem = (HTREEITEM)g_DeferredList.RemoveHead();
            else 
                tvi.hItem = NULL;
            g_pCSDeferredList->Unlock();
        }
    }

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectData
CObjectData::CObjectData()
{
    m_nType = typeUnknown ;
    m_pcf = NULL ;
    m_punk = NULL ;
    m_uiBitmap = 0;
    m_uiCategoryHint = 0;
    m_clsid = CLSID_NULL ;
    m_szProgID[0] = '\0' ;

}

CObjectData::~CObjectData()
{
    Release() ;
}

BOOL CObjectData::Release()
{
    if (m_pcf)
        m_pcf->Release() ;

    if (m_punk)
        m_punk->Release() ;

    m_pcf = NULL ;
    m_punk = NULL ;
   
    CoFreeUnusedLibraries() ;
    return TRUE ;
}

HRESULT CObjectData::CreateInstance( DWORD clsctx, ComServerInfo* pcsi )
{
    Release() ;

    HRESULT hr = NOERROR ;
    CString strError ;
    IUnknown* punk = NULL ;
    try
    {
        if (pcsi)
        {
            hr = CoGetClassObject( m_clsid, CLSCTX_REMOTE_SERVER, pcsi, IID_IUnknown, (void**)&punk ) ;
        }
        else
            hr = CoGetClassObject( m_clsid, clsctx, NULL, IID_IUnknown, (void**)&punk ) ;
        if (FAILED(hr))
        {
            strError = "CoGetClassObject failed." ;
            AfxThrowOleException(hr) ;
        }
        ASSERT(punk) ;

        hr = punk->QueryInterface( IID_IClassFactory, (void**)&m_pcf ) ;
        if (FAILED(hr))
        {
            strError = "QueryInterface on class factory for IClassFactory failed." ;
            AfxThrowOleException(hr) ;
        }
        ASSERT(m_pcf) ;

        punk->Release() ;
        punk = NULL ;

        hr = m_pcf->CreateInstance( NULL, IID_IUnknown, (void**)&m_punk ) ;
        if (FAILED(hr))
        {
            strError = "IClassFactory::CreateInstance failed." ;
            AfxThrowOleException(hr) ;
        }

        m_pcf->Release() ;
        m_pcf = NULL ;
    }
    catch(COleException* pException)
    {
        if (punk)
            punk->Release() ;
        Release() ;
        ErrorMessage( strError, pException->m_sc ) ;
        pException->Delete();
    }


    return hr ;
}

BOOL HasServer32( HKEY hkCLSID )
{
    HKEY hkTemp ;
    if ((RegOpenKey( hkCLSID, _T("InprocServer32"), &hkTemp ) == ERROR_SUCCESS) ||
        (RegOpenKey( hkCLSID, _T("InprocHandler32"), &hkTemp ) == ERROR_SUCCESS) ||
        (RegOpenKey( hkCLSID, _T("LocalServer32"), &hkTemp ) == ERROR_SUCCESS))
    {
        RegCloseKey( hkTemp ) ;
        return TRUE ;
    }

    return FALSE ;
}

BOOL CObjectData::Initialize(const TCHAR* szCLSID)
{
    USES_CONVERSION;

    TCHAR   szBuf[_MAX_PATH] ;
    HKEY    hkClass = NULL ;
    HKEY    hkTemp ;

//    m_nType = typeObject;
    CLSIDFromString(T2OLE(szCLSID), &m_clsid);
//  m_szProgID[0] = '\0' ;

    // Assume it's an object
    m_uiCategoryHint = BMINDEX(IDB_OBJECT) ;

    wsprintf(szBuf, "CLSID\\%s", szCLSID);
    if (RegOpenKey(HKEY_CLASSES_ROOT, szBuf, &hkClass ) == ERROR_SUCCESS)
    {

        // Now find out more about it.
        //
        // #1 Look in CLSID\{clsid} for the \Control key
        // #2 Assume it is an 'non-embeddable' OLE 2.0 object
        // #3 If \<progid>\NotInsertable exists we're done
        // #4 if !#2 then if \<progid>\Insertable it's an insertable OLE 2.0 object
        //    and we're done
        // #5 if !#3 see if it's an OLE 1.0 object by checking Ole1Class
        //
#ifdef _SHOW_32BIT_ICONS_
        BOOL    fHasServer32 = HasServer32( hkClass ) ;
        if (fHasServer32)
            m_uiCategoryHint = BMINDEX(IDB_OBJECT32) ;
        else
            m_uiCategoryHint = BMINDEX(IDB_OBJECT) ;
#else
        BOOL    fHasServer32 = FALSE;
#endif
        // First determine if it is a system component
        //
        LONG cb = sizeof(szBuf) ;
        if (RegQueryValue( hkClass, (LPTSTR)_T("InprocServer32"), szBuf, &cb ) == ERROR_SUCCESS)
        {
            if ((0 == _tcsnicmp(szBuf, _T("ole"), 3)) && 
                ((0 == lstrcmpi( szBuf+3, _T("32.dll") )) ||
                (0 == lstrcmpi( szBuf+3, _T("prx32.dll") )) ||
                (0 == lstrcmpi( szBuf+3, _T("2pr32.dll") )) ||
                (0 == lstrcmpi( szBuf+3, _T("cnv32.dll") )) ||
                (0 == lstrcmpi( szBuf+3, _T("aut32.dll") ))))
            {
                m_uiCategoryHint = BMINDEX(IDB_QUESTION32) ;
            }
        } // Query InprocServer
        else if (RegQueryValue( hkClass, (LPTSTR)_T("InprocServer"), szBuf, &cb ) == ERROR_SUCCESS)
        {
            if ((0 == _tcsnicmp(szBuf, _T("ole"), 3)) && 
                ((0 == lstrcmpi( szBuf+3,_T("2prox.dll") )) ||
                 (0 == lstrcmpi( szBuf+3,_T("2.dll") )) ||
                 (0 == lstrcmpi( szBuf+3,_T("2disp.dll") ))))
            {
                m_uiCategoryHint = BMINDEX(IDB_QUESTION) ;
            }
        } 

        // If it has "Ole1Class" then it's an OLE 1.0 object
        //
        if (RegOpenKey( hkClass, _T("Ole1Class"), &hkTemp ) == ERROR_SUCCESS)
        {
            // Is it 32 bit or 16 bit?
#ifdef _SHOW_32BIT_ICONS_
            if (fHasServer32)
                m_uiCategoryHint = BMINDEX(IDB_OBJECT_INSERTABLE1032) ; // that succeeded.  This means we're insertable
            else
#endif
                m_uiCategoryHint = BMINDEX(IDB_OBJECT_INSERTABLE10) ;   // that succeeded.  This means we're insertable
            RegCloseKey( hkTemp ) ;
            goto lblDone ;
        }
        // We now recognize the "Control" key which indicates it's
        // an OLE Custom Control.
        //
        if (RegOpenKey( hkClass, _T("Control"), &hkTemp ) == ERROR_SUCCESS)
        {
#ifdef _SHOW_32BIT_ICONS_
            // Is it 32 bit or 16 bit?
            if (fHasServer32)
                m_uiCategoryHint = BMINDEX(IDB_OBJECT_CONTROL32) ;
            else
#endif
                m_uiCategoryHint = BMINDEX(IDB_OBJECT_CONTROL) ;
            RegCloseKey( hkTemp ) ;
            goto lblDone ;
        }

        // If it's not a control, but it's insertable it must be a CD object
        //
        if (RegOpenKey( hkClass, _T("Insertable"), &hkTemp ) == ERROR_SUCCESS)
        {
#ifdef _SHOW_32BIT_ICONS_
            // Is it 32 bit or 16 bit?
            if (fHasServer32)
                m_uiCategoryHint = BMINDEX(IDB_OBJECT_INSERTABLE32) ;   // that succeeded.  This means we're insertable
            else
#endif
                m_uiCategoryHint = BMINDEX(IDB_OBJECT_INSERTABLE) ;   // that succeeded.  This means we're insertable
            RegCloseKey( hkTemp ) ;
            goto lblDone ;
        }

        cb = sizeof(m_szProgID)/sizeof(m_szProgID[0]) ;
        if (RegQueryValue( hkClass, (LPTSTR)_T("ProgID"), m_szProgID, &cb) == ERROR_SUCCESS)
        {
            HKEY    hkInsertable ;

            // Does the object force "NotInsertable"?
            //
            wsprintf( szBuf, _T("%s\\NotInsertable"), (LPTSTR)m_szProgID ) ;
            if (RegOpenKey( HKEY_CLASSES_ROOT, szBuf, &hkInsertable ) == ERROR_SUCCESS)
            {
#ifdef _SHOW_32BIT_ICONS_
                if (fHasServer32)
                    m_uiCategoryHint = BMINDEX(IDB_OBJECT32) ;
                else
#endif
                    m_uiCategoryHint = BMINDEX(IDB_OBJECT) ;
                RegCloseKey( hkInsertable ) ;
                goto lblDone ;
            }

            // Is it insertable?
            //
            wsprintf( szBuf, _T("%s\\Insertable"), (LPTSTR)m_szProgID ) ;
            if (RegOpenKey( HKEY_CLASSES_ROOT, szBuf, &hkInsertable ) == ERROR_SUCCESS)
            {
#ifdef _SHOW_32BIT_ICONS_
                if (fHasServer32)
                    m_uiCategoryHint = BMINDEX(IDB_OBJECT_INSERTABLE32) ;   // that succeeded.  This means we're insertable
                else
#endif
                    m_uiCategoryHint = BMINDEX(IDB_OBJECT_INSERTABLE) ;   // that succeeded.  This means we're insertable
                RegCloseKey( hkInsertable ) ;
                goto lblDone ;
            }
        }
lblDone:

        m_uiBitmap = m_uiCategoryHint;
#ifdef _SHOW_TOOLBOXBITMAPS
        cb = sizeof(szBuf) ;
        if (RegQueryValue( hkClass, (LPTSTR)"ToolboxBitmap32", szBuf, &cb ) == ERROR_SUCCESS ||
            RegQueryValue( hkClass, (LPTSTR)"ToolboxBitmap", szBuf, &cb ) == ERROR_SUCCESS)
        {
            m_strToolBoxBitmap = szBuf;
        }
#endif
        if (hkClass)
            RegCloseKey( hkClass ) ;
        return TRUE;
    }
    else
        return FALSE ;
}

/////////////////////////////////////////////////////////////////////////////
// CObjTreeView

/////////////////////////////////////////////////////////////////////////////
// CObjTreeView construction/destruction

CObjTreeView::CObjTreeView()
{
    m_hObjectInstances = NULL ;
    m_hObjectClasses = NULL;
    m_hTypeLibs= NULL;
    m_hInterfaces = NULL;

    m_hInsertable = NULL;
    m_hControls= NULL;
    m_hInternal= NULL;
#ifdef SHOW_CONTAINERS
    m_hContainers= NULL;
#endif
    m_hOLE1= NULL;
    m_hUnclassified= NULL;
    m_hInformalTypes = NULL;

    m_fComCatMgrAvail = FALSE ;

    g_pCSDeferredList = new CCriticalSection;
    g_pCSImages = new CCriticalSection;
    g_fExitDeferredThread=FALSE;
    //TRACE("Created...");
    g_pDeferredThread = AfxBeginThread(DeferredToolBoxUpdates, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
}

CObjTreeView::~CObjTreeView()
{
    if (g_pDeferredThread)
    {
        // Cause thread to exit
        g_fExitDeferredThread=TRUE;
        g_pDeferredThread->ResumeThread();
        //TRACE("Resumed (exit)\n");
    }

    if (g_pCSDeferredList);
    {
        delete g_pCSDeferredList ;
        g_pCSDeferredList = NULL;
    }

    if (g_pCSImages)
    {
        delete g_pCSImages ;
        g_pCSImages = NULL;
    }
}

LRESULT CObjTreeView::OnCommandHelp(WPARAM, LPARAM lParam)
{
    theApp.WinHelp( lParam, HELP_CONTEXT ) ;
    return TRUE ;
}

BOOL CObjTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CTreeView::PreCreateWindow(cs))
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_SHOWSELALWAYS |
		TVS_HASLINES | TVS_HASBUTTONS |	TVS_DISABLEDRAGDROP;
	if (g_osvi.dwMajorVersion == 3)
		cs.style |= WS_BORDER ;

	return TRUE;
}

void CObjTreeView::OnInitialUpdate()
{
    GetTreeCtrl().SetImageList( g_pImages, TVSIL_NORMAL ) ;

    BeginWaitCursor() ;
    GetTreeCtrl().SetRedraw( FALSE ) ;
    GetTreeCtrl().SelectItem( NULL ) ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
    ICatInformation* pci = NULL ;
    HRESULT hr ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_SERVER, IID_ICatInformation, (void**)&pci);
    if (SUCCEEDED(hr))
    {
        m_fComCatMgrAvail = TRUE ;
        pci->Release();
    }
#endif

    TV_INSERTSTRUCT tvis ;
    tvis.hParent = TVI_ROOT ;
    tvis.hInsertAfter = TVI_LAST ;
    tvis.item.stateMask = NULL ;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN ;
    tvis.item.lParam = 0 ;
    tvis.item.cChildren = 1 ;

    tvis.item.pszText = "Object Classes" ;
    tvis.item.iImage = BMINDEX(IDB_OBJECTFOLDER) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_OBJECTFOLDER) ;
    m_hObjectClasses = GetTreeCtrl().InsertItem( &tvis ) ;

    tvis.hParent = m_hObjectClasses ;
    tvis.item.pszText = "Grouped by Component Category" ;
    tvis.item.iImage = BMINDEX(IDB_OBJECTFOLDER) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_OBJECTFOLDER) ;
    tvis.item.lParam = (LPARAM)new CObjectData ;
    if (tvis.item.lParam)
    {
        ((CObjectData*)tvis.item.lParam)->m_nType = CObjectData::typeCategories ;
    }
    m_hInformalTypes = GetTreeCtrl().InsertItem( &tvis ) ;
    tvis.item.lParam = 0 ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
    GetTreeCtrl().Expand( m_hInformalTypes, TVE_EXPAND ) ;
#endif

    tvis.hParent = m_hObjectClasses ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
    if (m_fComCatMgrAvail == FALSE)
    {
#endif
        tvis.item.pszText = "OLE Embeddable Objects" ;
        tvis.item.iImage = BMINDEX(IDB_OBJECT_INSERTABLE) ;
        tvis.item.iSelectedImage = BMINDEX(IDB_OBJECT_INSERTABLE) ;
        m_hInsertable = GetTreeCtrl().InsertItem( &tvis ) ;

        tvis.item.pszText = "OLE Controls" ;
        tvis.item.iImage = BMINDEX(IDB_OBJECT_CONTROL) ;
        tvis.item.iSelectedImage = BMINDEX(IDB_OBJECT_CONTROL) ;
        m_hControls = GetTreeCtrl().InsertItem( &tvis ) ;

        tvis.item.pszText = "Unclassified Objects" ;
        tvis.item.iImage = BMINDEX(IDB_OBJECT) ;
        tvis.item.iSelectedImage = BMINDEX(IDB_OBJECT) ;
        m_hUnclassified = GetTreeCtrl().InsertItem( &tvis ) ;
#ifdef __ICatInformation_INTERFACE_DEFINED__
    }
#endif

#ifdef SHOW_CONTAINERS
    tvis.item.pszText = "Containers" ;
    tvis.item.iImage = BMINDEX(IDB_CONTAINER) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_CONTAINER) ;
    m_hContainers = GetTreeCtrl().InsertItem( &tvis ) ;
#endif

    tvis.item.pszText = "OLE 1.0 Objects" ;
    tvis.item.iImage = BMINDEX(IDB_OBJECT_INSERTABLE10) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_OBJECT_INSERTABLE10) ;
    m_hOLE1 = GetTreeCtrl().InsertItem( &tvis ) ;

    tvis.item.pszText = "COM Library Objects" ;
    tvis.item.iImage = BMINDEX(IDB_QUESTION) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_QUESTION) ;
    m_hInternal = GetTreeCtrl().InsertItem( &tvis ) ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
    if (m_fComCatMgrAvail)
    {
#endif
        tvis.item.pszText = "All Objects" ;
        tvis.item.iImage = BMINDEX(IDB_OBJECT) ;
        tvis.item.iSelectedImage = BMINDEX(IDB_OBJECT) ;
        m_hUnclassified = GetTreeCtrl().InsertItem( &tvis ) ;
#ifdef __ICatInformation_INTERFACE_DEFINED__
    }
#endif

    GetTreeCtrl().Expand( m_hObjectClasses, TVE_EXPAND ) ;

    tvis.hParent = TVI_ROOT ;
    tvis.item.pszText = "Type Libraries" ;
    tvis.item.iImage = BMINDEX(IDB_TYPELIBFOLDER) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_TYPELIBFOLDER) ;
    m_hTypeLibs = GetTreeCtrl().InsertItem( &tvis ) ;

    tvis.item.pszText = "Interfaces" ;
    tvis.item.iImage = BMINDEX(IDB_IFACEFOLDER) ;
    tvis.item.iSelectedImage = BMINDEX(IDB_IFACEFOLDER) ;
    m_hInterfaces = GetTreeCtrl().InsertItem( &tvis ) ;

    GetTreeCtrl().SetRedraw( TRUE ) ;
    EndWaitCursor() ;
}

void CObjTreeView::OnDestroy()
{
    CTreeView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CObjTreeView drawing

void CObjTreeView::OnDraw( CDC* /*pdc */ )
{
}

void CObjTreeView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
    pSender; pHint ;
    if (lHint & UPD_NOOBJECTVIEW)
        return ;

    BeginWaitCursor() ;
    GetTreeCtrl().SetRedraw( FALSE ) ;
    GetTreeCtrl().SelectItem( NULL ) ;

    if ((int)GetTreeCtrl().GetCount() > 0)
    {
        BOOL fExpanded = FALSE ;
        TV_ITEM tvi ;
        tvi.mask = TVIF_STATE ;

        tvi.hItem = m_hInformalTypes ;
        if (GetTreeCtrl().GetItem( &tvi ))
        {
            fExpanded = (tvi.state & TVIS_EXPANDED) ;
            GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
            if (fExpanded)
                GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
        }

#ifdef __ICatInformation_INTERFACE_DEFINED__
        if (m_fComCatMgrAvail == FALSE)
        {
#endif
            tvi.hItem = m_hInsertable ;
            if (GetTreeCtrl().GetItem( &tvi ))
            {
                fExpanded = (tvi.state & TVIS_EXPANDED) ;
                GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
                if (fExpanded)
                    GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
            }

            tvi.hItem = m_hControls ;
            if (GetTreeCtrl().GetItem( &tvi ))
            {
                fExpanded = (tvi.state & TVIS_EXPANDED) ;
                GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
                if (fExpanded)
                    GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
            }
        }

        tvi.hItem = m_hInternal ;
        if (GetTreeCtrl().GetItem( &tvi ))
        {
            fExpanded = (tvi.state & TVIS_EXPANDED) ;
            GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
            if (fExpanded)
                GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
        }

        tvi.hItem = m_hOLE1 ;
        if (GetTreeCtrl().GetItem( &tvi ))
        {
            fExpanded = (tvi.state & TVIS_EXPANDED) ;
            GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
            if (fExpanded)
                GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
        }

        tvi.hItem = m_hUnclassified ;
        if (GetTreeCtrl().GetItem( &tvi ))
        {
            fExpanded = (tvi.state & TVIS_EXPANDED) ;
            GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
            if (fExpanded)
                GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
        }
        
        tvi.hItem = m_hTypeLibs ;
        if (GetTreeCtrl().GetItem( &tvi ))
        {
            fExpanded = (tvi.state & TVIS_EXPANDED) ;
            GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
            if (fExpanded)
                GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
        }

        tvi.hItem = m_hInterfaces ;
        if (GetTreeCtrl().GetItem( &tvi ))
        {
            fExpanded = (tvi.state & TVIS_EXPANDED) ;
            GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
            if (fExpanded)
                GetTreeCtrl().Expand( tvi.hItem, TVE_EXPAND ) ;
        }
    }

    GetTreeCtrl().SetRedraw( TRUE ) ;
    EndWaitCursor() ;
}


/////////////////////////////////////////////////////////////////////////////
// CObjTreeView diagnostics

#ifdef _DEBUG
void CObjTreeView::AssertValid() const
{
    CTreeView::AssertValid();
}

void CObjTreeView::Dump(CDumpContext& dc) const
{
    CTreeView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjTreeView message handlers

void CObjTreeView::OnTreeSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    COle2ViewDoc*   pDoc = GetDocument() ;
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
    CObjectData*    lpOD ;

    lpOD = (CObjectData*)pnmtv->itemNew.lParam ;
    TV_ITEM tvi ;

    tvi.hItem = pnmtv->itemNew.hItem ;
    tvi.mask = TVIF_TEXT ;
    tvi.cchTextMax = 256 ;
    tvi.pszText = new TCHAR[256] ;
    if (GetTreeCtrl().GetItem( &tvi ))
    {
        pDoc->m_szObjectCur = tvi.pszText ;
    }
    else
        pDoc->m_szObjectCur = "ERROR" ;
    delete tvi.pszText ;
    // special case typelibs because there may be multiple entries (versions)
    // for a given TYPELIBID
    //
    if (lpOD)
    {
		pDoc->m_nType = lpOD->m_nType ;
        pDoc->m_clsidCur = lpOD->m_clsid ;
    }
    else
    {
		pDoc->m_nType = CObjectData::typeUnknown ;
        pDoc->m_clsidCur =  GUID_NULL;
    }
    pDoc->UpdateAllViews( this, UPD_NOOBJECTVIEW ) ;
	*pResult = 0;
}

void CObjTreeView::OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
    COle2ViewDoc*   pDoc = GetDocument() ;
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
    CObjectData* lpOD = (CObjectData*)pnmtv->itemNew.lParam ;
    if (pnmtv->action == TVE_EXPAND && pnmtv->itemNew.hItem != m_hObjectClasses && pnmtv->itemNew.hItem != m_hObjectInstances)
    {
        if (pnmtv->itemNew.state & TVIS_EXPANDEDONCE)
            return ;

        BeginWaitCursor() ;
        GetTreeCtrl().SetRedraw( FALSE ) ;

        if (pnmtv->itemNew.hItem == m_hInterfaces)
            ExpandInterfaces( pnmtv ) ;
        else if (pnmtv->itemNew.hItem == m_hTypeLibs)
            ExpandTypeLibs( pnmtv ) ;
        else if (pnmtv->itemNew.hItem == m_hInformalTypes)
            ExpandComponentCategories( pnmtv ) ;
        else if (lpOD != NULL && lpOD->m_nType != CObjectData::typeCategory)
            ExpandObject( pnmtv ) ;
        else 
            ExpandClassification( pnmtv ) ;

        GetTreeCtrl().SetRedraw( TRUE ) ;
        EndWaitCursor() ;
    }
}

void CObjTreeView::ExpandClassification( NM_TREEVIEW* pnmtv )
{
	USES_CONVERSION;
    HKEY    hkCLSID ;
    COle2ViewDoc* pDoc = GetDocument();
    CObjectData* lpOD ;
	BOOL	fComponentCategory = FALSE ;
    TCHAR   szCatID[40] ;

    TV_INSERTSTRUCT tvis ;
    tvis.hInsertAfter = TVI_SORT ;
    tvis.item.stateMask = NULL ;
    tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE |TVIF_CHILDREN ;
    tvis.item.cChildren = 1 ;
	
	if (pnmtv->itemNew.lParam)
	{
		lpOD = (CObjectData*)pnmtv->itemNew.lParam ;
		if (lpOD->m_nType == CObjectData::typeCategory)
        {
            OLECHAR wcs[40] ;
            StringFromGUID2(lpOD->m_clsid, wcs, 40) ;
            lstrcpy( szCatID, OLE2T(wcs)) ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
            ICatInformation* pci = NULL ;
            HRESULT hr ;
            hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_SERVER, IID_ICatInformation, (void**)&pci);
            if (SUCCEEDED(hr))
            {
                IEnumCLSID* penum = NULL ;
                CATID rgcatid[1];
                rgcatid[0] = lpOD->m_clsid;
                if (SUCCEEDED(hr = pci->EnumClassesOfCategories(1, rgcatid, 0, NULL, &penum)))
                {
                    CLSID clsid;

                    tvis.hParent = pnmtv->itemNew.hItem ;

                    while (S_OK == penum->Next(1, &clsid, NULL))
                    {
                        HTREEITEM htree = NULL ;

                        lpOD = new CObjectData;
                        StringFromGUID2(clsid, wcs, 40);
                        if (!lpOD->Initialize(OLE2T(wcs)))
    			            lpOD->m_uiBitmap = BMINDEX(IDB_OBJECT) ;
                        lpOD->m_nType = CObjectData::typeObject;

                        TCHAR   szName[256] ;
                        LONG cb = sizeof(szName)/sizeof(TCHAR);
                        *szName='\0';
                        if ((RegQueryValue(HKEY_CLASSES_ROOT, CString("CLSID\\")+OLE2T(wcs), szName, &cb) == ERROR_SUCCESS) && *szName == '\0')
                            wsprintf(szName, _T("%S <no name>"), wcs);

                        tvis.item.lParam = (LPARAM)(LPVOID)lpOD ;
                        tvis.item.pszText = szName ;
                        tvis.item.iImage = tvis.item.iSelectedImage = lpOD->m_uiBitmap ;
                        HTREEITEM h = GetTreeCtrl().InsertItem( &tvis ) ;
                        if (!lpOD->m_strToolBoxBitmap.IsEmpty())
                        {
                            // Add this guy to the defered list
                            g_pCSDeferredList->Lock(0);
                            g_DeferredList.AddTail(h);
                            g_pCSDeferredList->Unlock();
                        }
                    }
                }
                pci->Release();
                g_pCSDeferredList->Lock(0);
                if (g_DeferredList.IsEmpty() == FALSE)
                {
                    g_pCSDeferredList->Unlock();
                    g_pDeferredThread->ResumeThread();
                    //TRACE("Resumed.\n");
                }
                else
                    g_pCSDeferredList->Unlock();
                return ;
            }
            else
#endif
    			fComponentCategory = TRUE ;
        }
	}

    if (RegOpenKey( HKEY_CLASSES_ROOT, _T("CLSID"), &hkCLSID) == ERROR_SUCCESS)
    {
        DWORD   dwIndex ;
        TCHAR    szCLSID[64] ;

        for ( dwIndex = 0 ;
              RegEnumKey( hkCLSID, dwIndex, szCLSID, sizeof(szCLSID)) == ERROR_SUCCESS ;
              ++dwIndex )
        {
            HTREEITEM htree = NULL ;

			// If we're looking by comcat we can immediately
			// tell if this guy is worth looking at further.  This is purely
			// an performance opimization for when the ComCatMgr isn't available
            if (fComponentCategory == TRUE)
            {
                TCHAR szBuf[256];
                HKEY hkClass;
                wsprintf(szBuf, "%s\\Implemented Categories\\%s", szCLSID, szCatID);
                if (RegOpenKey( hkCLSID, szBuf, &hkClass ) != ERROR_SUCCESS)
                    continue;
                RegCloseKey(hkClass) ;
            }
            // end optimization

            lpOD = new CObjectData ;
            ASSERT(lpOD) ;
            lpOD->Initialize(szCLSID);
            lpOD->m_nType = CObjectData::typeObject;
            if (fComponentCategory == TRUE)
            {
                htree = pnmtv->itemNew.hItem ;
            }
            else
            {
                switch( lpOD->m_uiCategoryHint )
                {
                case BMINDEX(IDB_QUESTION):
                case BMINDEX(IDB_QUESTION32):
                    htree = m_hInternal ;
                break ;
                    
                case BMINDEX(IDB_OBJECT_INSERTABLE10):
                case BMINDEX(IDB_OBJECT_INSERTABLE1032):
                    htree = m_hOLE1 ;
                break ;

                case BMINDEX(IDB_OBJECT_INSERTABLE):
                case BMINDEX(IDB_OBJECT_INSERTABLE32):
#ifdef __ICatInformation_INTERFACE_DEFINED__
                if (m_fComCatMgrAvail)
                    htree = m_hUnclassified;
                else
#endif
                    htree = m_hInsertable ;
                break ;

                case BMINDEX(IDB_OBJECT_CONTROL):
                case BMINDEX(IDB_OBJECT_CONTROL32):
#ifdef __ICatInformation_INTERFACE_DEFINED__
                if (m_fComCatMgrAvail)
                    htree = m_hUnclassified;
                else
#endif
                    htree = m_hControls ;
                break ;

                case BMINDEX(IDB_OBJECT):
                case BMINDEX(IDB_OBJECT32):
                default:
                    htree = m_hUnclassified ;
                break ;
                }
            }

            if (htree && htree == pnmtv->itemNew.hItem)
            {
                ASSERT(lpOD) ;
                TCHAR   szName[256] ;
                LONG cb = sizeof(szName)/sizeof(TCHAR);
                *szName='\0';
                if ((RegQueryValue(hkCLSID, szCLSID, szName, &cb) == ERROR_SUCCESS) && *szName == '\0')
                    wsprintf(szName, _T("%s <no name>"), szCLSID);

                tvis.hParent = htree ;
                tvis.item.lParam = (LPARAM)(LPVOID)lpOD ;
                tvis.item.pszText = szName ;
                tvis.item.iImage = tvis.item.iSelectedImage = lpOD->m_uiBitmap ;
                HTREEITEM h = GetTreeCtrl().InsertItem( &tvis ) ;
                if (!lpOD->m_strToolBoxBitmap.IsEmpty())
                {
                    // Add this guy to the defered list
                    g_pCSDeferredList->Lock(0);
                    g_DeferredList.AddTail(h);
                    g_pCSDeferredList->Unlock();
                }
            }
            else
                delete lpOD ;
        }
        RegCloseKey( hkCLSID ) ;
    }
    g_pCSDeferredList->Lock(0);
    if (g_DeferredList.IsEmpty() == FALSE)
    {
        g_pCSDeferredList->Unlock();
        g_pDeferredThread->ResumeThread();
        //TRACE("Resumed.\n");
    }
    else
        g_pCSDeferredList->Unlock();
    return ;
}

void CObjTreeView::ExpandTypeLibs( NM_TREEVIEW* /* pnmtv */ )
{
	USES_CONVERSION;
    HKEY    hkTypeLib ;
    if (RegOpenKey( HKEY_CLASSES_ROOT, _T("TypeLib"), &hkTypeLib) == ERROR_SUCCESS)
    {
        LONG    cb ;
        TCHAR    szVer[64] ;
        TCHAR    szName[_MAX_PATH] ;
        TCHAR    szGUID[64] ;
        TCHAR    szBuf[_MAX_PATH*2] ;
        DWORD   dwIndex, dwIndex2 ;
        HKEY    hkGUID ;

        // Enum \TypeInfo
        for ( dwIndex = 0 ;
              RegEnumKey( hkTypeLib, dwIndex, szGUID, sizeof(szGUID)) == ERROR_SUCCESS ;
              ++dwIndex )
        {
            // Gotta open \TypeInfo\szGUID
            if (RegOpenKey( hkTypeLib, szGUID, &hkGUID ) == ERROR_SUCCESS)
            {
                // Enum all at this level (major.minor = name)
                for (dwIndex2 = 0 ;
                    RegEnumKey( hkGUID, dwIndex2, szVer, sizeof(szVer)) == ERROR_SUCCESS ;
                    ++dwIndex2 )
                {
                    cb = sizeof(szName);
                    if (RegQueryValue( hkGUID, (LPTSTR)szVer, szName, &cb) == ERROR_SUCCESS)
                    {
                        CObjectData* lpOD = new CObjectData ;
                        lpOD->m_nType = CObjectData::typeTypeLib ;
                        ::CLSIDFromString(T2OLE(szGUID), &lpOD->m_clsid);
                        lpOD->m_szProgID[0] = '\0' ;

                        lpOD->m_uiCategoryHint = lpOD->m_uiBitmap = BMINDEX(IDB_AUTOMATION) ;
                        lpOD->m_wMajorVer = (WORD)atoi( szVer ) ;
                        PTSTR p = strrchr( szVer, '.' ) ;
                        if (p)
                            lpOD->m_wMinorVer = (WORD)atoi( p+1 ) ;
                        else
                            lpOD->m_wMinorVer = 0 ;

                        if (*szName == _T('\0'))
                        {
                            wsprintf(szName, _T("%s <no name>"), szGUID);
                        }

                        wsprintf( szBuf, _T("%s (Ver %s)"), (LPTSTR)szName, (LPTSTR)szVer ) ;

                        TV_INSERTSTRUCT tvis ;
                        tvis.hParent = m_hTypeLibs ;
                        tvis.hInsertAfter = TVI_SORT ;
                        tvis.item.stateMask = NULL ;
                        tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                        tvis.item.iImage = tvis.item.iSelectedImage = lpOD->m_uiBitmap ;
                        tvis.item.lParam = (LPARAM)(LPVOID)lpOD ;
                        tvis.item.pszText = szBuf ;
                        GetTreeCtrl().InsertItem( &tvis ) ;
                    }
                }
                RegCloseKey( hkGUID ) ;
            }
        }
        RegCloseKey( hkTypeLib ) ;
    }
}

void CObjTreeView::ExpandObject( NM_TREEVIEW* pnmtv )
{
	USES_CONVERSION;
    COle2ViewDoc*   pDoc = GetDocument() ;
    TV_INSERTSTRUCT tvis ;
    CObjectData* pod = (CObjectData*)pnmtv->itemNew.lParam ;

    if (pod->m_punk == NULL)
    {
        HRESULT hr ;
        if (pod->m_nType == CObjectData::typeObject)
        {
            hr = pod->CreateInstance( pDoc->m_dwClsCtx, NULL) ;
        }
        else
        {
            hr = BindToFile(pod->m_strPersistFile, &pod->m_punk) ;
            if (FAILED(hr))
            {
                CString strTemp ;
                strTemp.Format("IMoniker::BindToObject failed on the file moniker created from ( \"%s\" ).", (LPCTSTR)pod->m_strPersistFile ) ;
                ErrorMessage( strTemp, hr ) ;
            }
        }
        if (FAILED(hr))
        {
            // Insert "Instance Released"
            tvis.hParent = pnmtv->itemNew.hItem  ;
            tvis.hInsertAfter = TVI_SORT ;
            tvis.item.mask = TVIF_TEXT ;
            tvis.item.pszText = _T("Instance could not be created.");
            GetTreeCtrl().InsertItem( &tvis ) ;
            return ;
        }
    }

    // Make him bold
    //
    tvis.item.mask = TVIF_STATE ;
    tvis.item.hItem = pnmtv->itemNew.hItem ;
    GetTreeCtrl().GetItem(&tvis.item) ;
    tvis.item.mask = TVIF_STATE ;
    tvis.item.stateMask = TVIS_BOLD ;
    tvis.item.state |= TVIS_BOLD ;
    GetTreeCtrl().SetItem(&tvis.item) ;

    tvis.hParent = pnmtv->itemNew.hItem  ;
    tvis.hInsertAfter = TVI_SORT ;
    tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE ;
    tvis.item.iImage = tvis.item.iSelectedImage = BMINDEX(IDB_INTERFACE) ;

    HKEY hk ;
    TCHAR szIID[80] ;
    IID  iid ;
    IUnknown* pfoo = NULL ;
    TCHAR szValue[256] ;
    if (RegOpenKey( HKEY_CLASSES_ROOT, _T("Interface"), &hk) == ERROR_SUCCESS)
    {
        for ( DWORD dwIndex = 0 ;
              RegEnumKey( hk, dwIndex, szIID, sizeof(szIID)) == ERROR_SUCCESS ;
              ++dwIndex )
        {
            LONG cb = sizeof(szValue);
            if (RegQueryValue( hk, (LPTSTR)szIID, szValue, &cb) == ERROR_SUCCESS)
            {
                if (SUCCEEDED(::CLSIDFromString(T2OLE(szIID), &iid)) && SUCCEEDED(pod->m_punk->QueryInterface( iid, (void**)&pfoo )))
                {
                    pfoo->Release() ;
                    pfoo = NULL ;

                    if (*szValue == _T('\0'))
                        wsprintf(szValue, _T("%s <no name>"), szIID);

                    CObjectData* lpOD = new CObjectData ;
                    lpOD->m_nType = CObjectData::typeInterface ;
                    lpOD->m_clsid = iid ;
                    lpOD->m_szProgID[0] = '\0' ;
                    lpOD->m_uiCategoryHint = lpOD->m_uiBitmap = BMINDEX(IDB_INTERFACE) ;
                    lpOD->m_wMajorVer =  lpOD->m_wMajorVer  = 0 ;
                    tvis.item.lParam = (LPARAM)(LPVOID)lpOD ;
                    tvis.item.pszText = szValue ;
                    GetTreeCtrl().InsertItem( &tvis ) ;
                }
            }
        }
        RegCloseKey( hk ) ;
    }
}

void CObjTreeView::ExpandInterfaces( NM_TREEVIEW* pnmtv )
{
	USES_CONVERSION;
    if (pnmtv->itemNew.hItem == m_hInterfaces)
    {
        TV_INSERTSTRUCT tvis ;
        tvis.hParent = pnmtv->itemNew.hItem  ;
        tvis.hInsertAfter = TVI_SORT ;
        tvis.item.stateMask = NULL ;
        tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE ;
        tvis.item.iImage = tvis.item.iSelectedImage = BMINDEX(IDB_INTERFACE) ;

        HKEY hk ;
        TCHAR szIID[80] ;
        TCHAR szValue[256] ;
        if (RegOpenKey( HKEY_CLASSES_ROOT, _T("Interface"), &hk) == ERROR_SUCCESS)
        {
            for ( DWORD dwIndex = 0 ;
                  RegEnumKey( hk, dwIndex, szIID, sizeof(szIID)) == ERROR_SUCCESS ;
                  ++dwIndex )
            {
                LONG cb = sizeof(szValue);
                if (RegQueryValue( hk, (LPTSTR)szIID, szValue, &cb) == ERROR_SUCCESS)
                {
                    if (*szValue == _T('\0'))
                        wsprintf(szValue, _T("%s <no name>"), szIID);

                    CObjectData* lpOD = new CObjectData ;
                    lpOD->m_nType = CObjectData::typeStaticInterface ;
                    ::CLSIDFromString(T2OLE(szIID), &lpOD->m_clsid );
                    lpOD->m_szProgID[0] = '\0' ;
                    lpOD->m_uiCategoryHint = lpOD->m_uiBitmap = BMINDEX(IDB_INTERFACE) ;
                    lpOD->m_wMajorVer =  lpOD->m_wMajorVer  = 0 ;
    
                    tvis.item.lParam = (LPARAM)(LPVOID)lpOD ;
                    tvis.item.pszText = szValue ;
                    GetTreeCtrl().InsertItem( &tvis ) ;
                }
            }
            RegCloseKey( hk ) ;
        }
        return ;
    }
}

void CObjTreeView::ExpandComponentCategories( NM_TREEVIEW* pnmtv )
{
	USES_CONVERSION;
    TV_INSERTSTRUCT tvis ;
    tvis.hParent = pnmtv->itemNew.hItem  ;
    tvis.hInsertAfter = TVI_SORT ;
    tvis.item.stateMask = NULL ;
    tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE |TVIF_CHILDREN  ;
    tvis.item.cChildren = 1 ;

#ifdef __ICatInformation_INTERFACE_DEFINED__
    ICatInformation* pci = NULL ;
    HRESULT hr ;
    COle2ViewDoc*   pDoc = GetDocument() ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_SERVER, IID_ICatInformation, (void**)&pci);
    if (SUCCEEDED(hr))
    {
        IEnumCATEGORYINFO* penum = NULL ;
        if (SUCCEEDED(hr = pci->EnumCategories(GetUserDefaultLCID(), &penum)))
        {
            CATEGORYINFO catinfo;
            CObjectData* pOD ;
            while (S_OK == penum->Next(1, &catinfo, NULL))
            {
                OLECHAR wszTypeID[80] ;
                StringFromGUID2(catinfo.catid, wszTypeID, 80);
                if (*catinfo.szDescription)
    				tvis.item.pszText = OLE2T(catinfo.szDescription) ;
                else
                {
                    TCHAR szValue[256] ;
					wsprintf(szValue, _T("_%S <no name>"), wszTypeID );
                    tvis.item.pszText = szValue;
                }
                if (tvis.item.pszText[0] == '_' && pDoc->m_fViewHiddenComCats == FALSE)
                    continue;

                pOD = new CObjectData;
                ASSERT(pOD);
                if (!pOD->Initialize(OLE2T(wszTypeID)))
    			    pOD->m_uiBitmap = BMINDEX(IDB_OBJECTFOLDER) ;
                pOD->m_nType = CObjectData::typeCategory;

                tvis.item.lParam = (LPARAM)(LPVOID)pOD ;
                tvis.item.iImage = tvis.item.iSelectedImage = pOD->m_uiBitmap;
				GetTreeCtrl().InsertItem( &tvis ) ;
            }
            penum->Release();
        }
        pci->Release();
    }
    else
#endif
    {
        HKEY hk ;
        TCHAR szTypeID[80] ;
        TCHAR szValue[256] ;
        if (RegOpenKey( HKEY_CLASSES_ROOT, _T("Component Categories"), &hk) == ERROR_SUCCESS)
        {
            for ( DWORD dwIndex = 0 ;
                  RegEnumKey( hk, dwIndex, szTypeID, sizeof(szTypeID)) == ERROR_SUCCESS ;
                  ++dwIndex )
            {
			    HKEY hkType ;
			    if (RegOpenKey(hk, szTypeID, &hkType) == ERROR_SUCCESS)
			    {
				    ULONG cb = sizeof(szValue);
				    if (RegQueryValueEx( hkType, _T("409"), NULL, NULL, (BYTE*)szValue, &cb) == ERROR_SUCCESS)
				    {
					    if (*szValue == _T('\0'))
						    wsprintf(szValue, _T("%s <no name>"), szTypeID);

					    CObjectData* lpOD = new CObjectData ;
					    lpOD->m_nType = CObjectData::typeCategory ;
                        if (!lpOD->Initialize(szTypeID))
    					    lpOD->m_uiBitmap = BMINDEX(IDB_OBJECTFOLDER) ;
					    lpOD->m_wMajorVer =  lpOD->m_wMajorVer  = 0 ;

                        tvis.item.iImage = tvis.item.iSelectedImage = lpOD->m_uiBitmap;
					    tvis.item.lParam = (LPARAM)(LPVOID)lpOD ;
					    tvis.item.pszText = szValue ;
					    GetTreeCtrl().InsertItem( &tvis ) ;
	                }
				    RegCloseKey(hkType) ;
			    }
            }
            RegCloseKey( hk ) ;
        }
    }
    return ;
}

void CObjTreeView::OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
    if ( pnmtv->itemNew.lParam != NULL && 
         pnmtv->action != TVE_EXPAND)
    {
        CObjectData* pod = (CObjectData*)pnmtv->itemNew.lParam ;
/*
        switch(pod->m_nType)
        {
        case CObjectData::typeObject:
        case CObjectData::typeObjectInstance:
        {
            HTREEITEM hitem ;
            while(NULL != (hitem = GetTreeCtrl().GetChildItem(pnmtv->itemNew.hItem)))
                GetTreeCtrl().DeleteItem(hitem);
            pnmtv->itemNew.mask = TVIF_CHILDREN | TVIF_STATE;
            pnmtv->itemNew.state &= ~TVIS_EXPANDEDONCE;
            pnmtv->itemNew.cChildren = 1;
            GetTreeCtrl().SetItem( &pnmtv->itemNew );
        }
        break ;
        }
*/
    }
 }

BOOL CObjTreeView::CallInterfaceViewer(CObjectData* pObjectData, HTREEITEM hItem)
{
    BOOL f = FALSE ;

    if (pObjectData != NULL)
    {
		ASSERT_KINDOF(CObjectData,pObjectData);

        switch(pObjectData->m_nType)
        {
        case CObjectData::typeStaticInterface:
        case CObjectData::typeInterface:
        {
            CObjectData* podParent = NULL ;
            TV_ITEM tviParent ;
            tviParent.hItem = GetTreeCtrl().GetParentItem(hItem);
            ASSERT(tviParent.hItem) ;
            tviParent.mask = TVIF_PARAM ;
            GetTreeCtrl().GetItem( &tviParent ) ;
            podParent = (CObjectData*)tviParent.lParam ;

            if (podParent)
                ViewInterface( theApp.m_pMainWnd->GetSafeHwnd(), pObjectData->m_clsid, podParent->m_punk ) ;
            else
                ViewInterface( theApp.m_pMainWnd->GetSafeHwnd(), pObjectData->m_clsid, NULL ) ;
            f = TRUE ;
        }            
        break ;

        case CObjectData::typeTypeLib:
        {
            BeginWaitCursor() ;
            ITypeLib* pTypeLib ;
            LCID      lcid = GetUserDefaultLCID() ;
            HRESULT hr = ::LoadRegTypeLib( pObjectData->m_clsid, pObjectData->m_wMajorVer,
                               pObjectData->m_wMinorVer, lcid, &pTypeLib ) ;
            if (FAILED(hr))
            {
				USES_CONVERSION;
                TCHAR szTemp[256] ;
				LPOLESTR lpStr;
				StringFromCLSID(pObjectData->m_clsid, &lpStr);
                wsprintf(szTemp, _T("LoadRegTypeLib(%s, %u, %u, %lu, ...) failed."),
                        OLE2CT(lpStr), pObjectData->m_wMajorVer, pObjectData->m_wMinorVer,
                        lcid);
				CoTaskMemFree(lpStr);
                EndWaitCursor();
                ErrorMessage( szTemp, hr ) ;
            }
            else
            {
                EndWaitCursor();
                ViewInterface( theApp.m_pMainWnd->GetSafeHwnd(), IID_ITypeLib, pTypeLib ) ;
                #ifdef _DEBUG
                ASSERT( 0 == pTypeLib->Release() ) ;
                #else
                pTypeLib->Release() ;
                #endif
                f = TRUE ;
            }
        }    
        break ;
        }
    }
    return f ;
}

void CObjTreeView::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT*) 
{
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;

    if (pnmtv->itemOld.lParam)
    {
        CObjectData* pod = (CObjectData*)pnmtv->itemOld.lParam ;
        ASSERT(pod->IsKindOf(RUNTIME_CLASS(CObjectData)));
        delete (CObjectData*)pnmtv->itemOld.lParam ;
        pnmtv->itemOld.lParam = NULL ;
    }
}

void CObjTreeView::OnObjectDelete() 
{
    if (GetTreeCtrl().GetSafeHwnd() != CWnd::GetFocus()->GetSafeHwnd())
        return ;

    TV_ITEM tvi ;
    TCHAR szText[256] ;
    tvi.hItem = GetTreeCtrl().GetSelectedItem();
    tvi.mask = TVIF_PARAM  | TVIF_TEXT ;
    tvi.pszText = szText ;
    tvi.cchTextMax = sizeof(szText)/sizeof(TCHAR);
    if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
    {
        CObjectData* lpOD = (CObjectData*)tvi.lParam ;
        ASSERT(lpOD);
        if (lpOD)
        {
            CString str = CString("Are you sure you want to delete all registry information related to the '") + tvi.pszText + CString("' ") ;
            switch(lpOD->m_nType)
            {
            case CObjectData::typeObject:
                str += _T("Object?") ;
            break ;

            case CObjectData::typeTypeLib:
                str += _T("Type Library?") ;
            break ;

            case CObjectData::typeInterface:
                str += _T("Interface?") ;
            break ;

            case CObjectData::typeStaticInterface:
                str += _T("Interface?") ;
            break ;

            case CObjectData::typeCategory:
                str += _T("Informal Type?") ;
            break ;

            default:
                str += _T("<unidentified thing>?");
            break ;
            }

            if (IDYES == AfxMessageBox(str, MB_YESNO))
            {
            	AfxMessageBox("Sorry, not implemented yet" ) ;
            }
        }
    }
}

void CObjTreeView::OnObjectVerify() 
{
	AfxMessageBox("Not implemented yet" ) ;
	
}

#ifdef _DEBUG

COle2ViewDoc* CObjTreeView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COle2ViewDoc)));
    return (COle2ViewDoc*) m_pDocument;
}

#endif //_DEBUG

void CObjTreeView::OnUseInProcServer()
{
    COle2ViewDoc*   pDoc = GetDocument() ;

    if (pDoc->m_dwClsCtx & CLSCTX_INPROC_SERVER)
        pDoc->m_dwClsCtx &= ~CLSCTX_INPROC_SERVER ;
    else
        pDoc->m_dwClsCtx |= CLSCTX_INPROC_SERVER ;
    if (pDoc->m_dwClsCtx == NULL)
        pDoc->m_dwClsCtx = CLSCTX_INPROC_SERVER ;
//    OnUpdate( this, NULL, NULL ) ;
}

void CObjTreeView::OnUpdateUseInProcServer(CCmdUI* pCmdUI)
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    pCmdUI->SetCheck( pDoc->m_dwClsCtx & CLSCTX_INPROC_SERVER ? TRUE : FALSE  ) ;
}

void CObjTreeView::OnUseInProcHandler()
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    if (pDoc->m_dwClsCtx & CLSCTX_INPROC_HANDLER)
        pDoc->m_dwClsCtx &= ~CLSCTX_INPROC_HANDLER ;
    else
        pDoc->m_dwClsCtx |= CLSCTX_INPROC_HANDLER ;
    if (pDoc->m_dwClsCtx == NULL)
        pDoc->m_dwClsCtx = CLSCTX_INPROC_HANDLER ;
//    OnUpdate( this, NULL, NULL ) ;
}

void CObjTreeView::OnUpdateUseInProcHandler(CCmdUI* pCmdUI)
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    pCmdUI->SetCheck( pDoc->m_dwClsCtx & CLSCTX_INPROC_HANDLER ? TRUE : FALSE ) ;
}

void CObjTreeView::OnUseLocalServer()
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    if (pDoc->m_dwClsCtx & CLSCTX_LOCAL_SERVER)
        pDoc->m_dwClsCtx &= ~CLSCTX_LOCAL_SERVER ;
    else
        pDoc->m_dwClsCtx |= CLSCTX_LOCAL_SERVER ;

    if (pDoc->m_dwClsCtx == NULL)
        pDoc->m_dwClsCtx = CLSCTX_LOCAL_SERVER ;

//    OnUpdate( this, NULL, NULL ) ;
}

void CObjTreeView::OnUpdateUseLocalServer(CCmdUI* pCmdUI)
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    pCmdUI->SetCheck( pDoc->m_dwClsCtx & CLSCTX_LOCAL_SERVER ? TRUE : FALSE ) ;
}


void CObjTreeView::OnFileBind()
{
    static TCHAR szFilter[] = _T("AllFiles(*.*)|*.*|") ;

    CFileDialog dlg(TRUE, _T("*.*"), NULL,
                    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
                    szFilter, this);
    if (IDOK != dlg.DoModal())
        return ;

    IUnknown* punk = NULL ;
    HRESULT hr ;
    if (SUCCEEDED(hr = BindToFile(dlg.GetPathName(), &punk)))
    {
        AddObjectInstance(punk, dlg.GetPathName()) ;
        punk->Release() ;
    }
    else
    {
        CString strTemp ;
        strTemp.Format("IMoniker::BindToObject failed on the file moniker created from ( \"%s\" ).", (LPCTSTR)dlg.GetPathName() ) ;
        ErrorMessage( strTemp, hr ) ;
    }
}

HRESULT CObjTreeView::BindToFile(const CString& strFileName, IUnknown** ppunk)
{
    USES_CONVERSION;
    BeginWaitCursor();
    HRESULT hr ;
    COle2ViewDoc*   pDoc = GetDocument() ;
    CString strTemp ;
    *ppunk = NULL ;

    // If the bind succeeds, then we check if the object 
    // supports IPersist[something].  if it does then we use
    // IPersist::GetCLSID to find the appropriate item in our object view,
    // select it and expand it.
    //
    // If the object does not support IPersist*, then we have no choice
    // but to create a new entry for the object.
    //
    TCHAR szFullFileName[_MAX_PATH] ;
//    TCHAR* p ;
//    GetFullPathName(strFileName, _MAX_PATH, szFullFileName, &p) ;
    lstrcpy(szFullFileName, strFileName) ;
    IMoniker* pmk = NULL ;
    hr = CreateFileMoniker(T2OLE(szFullFileName), &pmk ) ;

    if (FAILED(hr))
    {
        EndWaitCursor();
        strTemp.Format("CreateFileMoniker( \"%s\" ) failed.", (LPCTSTR)szFullFileName ) ;
        ErrorMessage( strTemp, hr ) ;
        return hr;
    }

    IUnknown* punk = NULL ;
    LPBINDCTX   pbc ;
    CreateBindCtx(0,&pbc) ;
    hr = pmk->BindToObject( pbc, NULL, IID_IUnknown,(LPVOID FAR*)&punk) ;
    pbc->Release() ;
    pmk->Release() ;

    *ppunk = punk ;
    EndWaitCursor();
    return hr;
}

BOOL CObjTreeView::AddObjectInstance(IUnknown* punk, const CString& strFileNameOrProgID)
{
    BeginWaitCursor();
    IPersist* ppersist = NULL ;
        // Does the object support IPersist[something]?  Find out the CLISD
        //
    CLSID clsid = CLSID_NULL ;
    HRESULT hr ;
    if ((SUCCEEDED(hr = punk->QueryInterface(IID_IPersist, (void**)&ppersist)) ||
         SUCCEEDED(hr = punk->QueryInterface(IID_IPersistStream, (void**)&ppersist)) ||
         SUCCEEDED(hr = punk->QueryInterface(IID_IPersistStorage, (void**)&ppersist))) &&
        SUCCEEDED(hr = ppersist->GetClassID(&clsid)))
    {
        ppersist->Release() ;
        ppersist=NULL ;
    }
    USES_CONVERSION;
    CObjectData* lpOD = new CObjectData ;
	lpOD->m_nType = CObjectData::typeObjectInstance ;
	lpOD->m_clsid = clsid ;
	lpOD->m_szProgID[0] = '\0' ;
	lpOD->m_uiBitmap = BMINDEX(IDB_OBJECT) ;
	lpOD->m_wMajorVer =  lpOD->m_wMajorVer  = 0 ;
    lpOD->m_punk = punk ;
    lpOD->m_strPersistFile = strFileNameOrProgID ;
    punk->AddRef() ;

    CString str ;
    if (clsid != CLSID_NULL)
    {
        OLECHAR wcs[40] ;
        StringFromGUID2(lpOD->m_clsid, wcs, 40) ;
        TCHAR szCLSID[40];
        lstrcpy( szCLSID, OLE2T(wcs)) ;
        lpOD->Initialize(szCLSID);

        TCHAR szDesc[256] ;
        LONG cb = 256 ;
        str.Format(_T("CLSID\\%s"), szCLSID) ;
        lstrcpy(szDesc, szCLSID);
        RegQueryValue(HKEY_CLASSES_ROOT, str, szDesc, &cb) ;
        str.Format(_T("%s (%s)"), strFileNameOrProgID, szDesc) ;
    }    
    else
        str.Format(_T("%s"), strFileNameOrProgID) ;

    if (!AddObjectInstance(str, lpOD))
    {
        lpOD->Release() ;
        delete lpOD ;
        hr = E_FAIL ;
        CString strTemp;
        strTemp.Format("Could not add item to tree view.  Internal Ole2View error.") ;
        ErrorMessage( strTemp, hr ) ;
    }

    EndWaitCursor();
    return SUCCEEDED(hr) ;
}

BOOL CObjTreeView::AddObjectInstance(LPCTSTR szDesc, CObjectData* pOD)
{
    TV_INSERTSTRUCT tvis ;
    tvis.item.stateMask = NULL ;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN ;
    tvis.item.cChildren = 1 ;
    if (m_hObjectInstances == NULL)
    {
        tvis.hInsertAfter = TVI_FIRST ;
        tvis.hParent = TVI_ROOT ;
        tvis.item.lParam = 0 ;

        tvis.item.pszText = _T("Object Instances") ;
        tvis.item.iImage = BMINDEX(IDB_OBJECTFOLDER) ;
        tvis.item.iSelectedImage = BMINDEX(IDB_OBJECTFOLDER) ;
        m_hObjectInstances = GetTreeCtrl().InsertItem( &tvis ) ;
    }

    tvis.hInsertAfter = TVI_LAST ;
    tvis.hParent = m_hObjectInstances ;
    tvis.item.iImage = pOD->m_uiBitmap  ;
    tvis.item.iSelectedImage = pOD->m_uiBitmap ;
	tvis.item.lParam = (LPARAM)(LPVOID)pOD ;
    tvis.item.pszText = (TCHAR*)((LPVOID)szDesc);
    tvis.item.mask |= TVIF_STATE ;
    tvis.item.stateMask = TVIS_BOLD  ;
    tvis.item.state = TVIS_BOLD ;
    HTREEITEM hitem = GetTreeCtrl().InsertItem( &tvis ) ;
    GetTreeCtrl().Expand( hitem, TVE_EXPAND ) ;
    GetTreeCtrl().Expand( m_hObjectInstances, TVE_EXPAND ) ;
    GetTreeCtrl().SelectItem( hitem ) ;
    return TRUE ;
}

BOOL CObjTreeView::IsValidSel()
{
    if (GetTreeCtrl().GetSafeHwnd() != CWnd::GetFocus()->GetSafeHwnd())
        return FALSE ;

    TV_ITEM tvi ;
    tvi.hItem = GetTreeCtrl().GetSelectedItem();
    tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
    if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        return TRUE;

    return FALSE ;
}

void CObjTreeView::OnTreeRClick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	CMenu Menu;
	CMenu* pPopup = NULL;
	POINT ptMouse;

	DWORD MessagePos = GetMessagePos();
	ptMouse.x = LOWORD(MessagePos);	
	ptMouse.y = HIWORD(MessagePos);
	
    TV_ITEM tvi ;
    UINT Flags ;
    CPoint ptClient = ptMouse;
    ScreenToClient(&ptClient) ;
    tvi.hItem = GetTreeCtrl().HitTest(ptClient, &Flags);
    tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
    if (tvi.hItem)
    {
        GetTreeCtrl().SelectItem(tvi.hItem);
        if (GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        {
            CObjectData* pOD = (CObjectData*)tvi.lParam ;

            switch(pOD->m_nType)
            {
            case CObjectData::typeObject:
            case CObjectData::typeObjectInstance:
    	        Menu.LoadMenu(IDM_OBJECT);
	            pPopup = Menu.GetSubMenu(0);

                pPopup->EnableMenuItem(ID_OBJECT_CREATE,  (pOD->m_punk ? MF_GRAYED : MF_ENABLED));
                pPopup->EnableMenuItem(ID_OBJECT_RELEASE, (pOD->m_punk ? MF_ENABLED : MF_GRAYED ));
                if (g_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
                    g_osvi.dwMajorVersion >= 4)
                {
                    pPopup->EnableMenuItem(ID_OBJECT_CREATE_ON,  (pOD->m_punk ? MF_GRAYED : MF_ENABLED));
                    pPopup->CheckMenuItem(ID_OBJECT_ATSTORAGE, (GetActivateAtStorage(pOD->m_clsid) ? MF_CHECKED : MF_UNCHECKED ));
                }
                else
                {
                    pPopup->EnableMenuItem(ID_OBJECT_CREATE_ON,  MF_GRAYED);
                    pPopup->EnableMenuItem(ID_OBJECT_ATSTORAGE,  MF_GRAYED);
                }
            break ;

            case CObjectData::typeStaticInterface:
            case CObjectData::typeInterface:
            case CObjectData::typeTypeLib:
    	        Menu.LoadMenu(IDM_INTERFACE);
	            pPopup = Menu.GetSubMenu(0);
                pPopup->EnableMenuItem(ID_OBJECT_VIEW, MF_ENABLED);
            break ;

            case CObjectData::typeCategory:
    	        Menu.LoadMenu(IDM_INTERFACE);
	            pPopup = Menu.GetSubMenu(0);
                pPopup->DeleteMenu(ID_OBJECT_VIEW, MF_BYCOMMAND);
            break ;
            }

            if (pPopup)
            {
    	        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,ptMouse.x, ptMouse.y,this);	
	            pPopup->DestroyMenu();
	            Menu.DestroyMenu();	
            }
        }
    }
    else
    {

    }

    *pResult = NULL;
}


void CObjTreeView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{


    UINT hitflags ;
    HTREEITEM hitem ;
    hitem = GetTreeCtrl().HitTest( point, &hitflags ) ;
    if (hitflags & TVHT_ONITEM) 
    {
        LRESULT result ;
        OnTreeReturn( NULL, &result);
    }
    else
    {
        CTreeView::OnLButtonDblClk(nFlags, point);    
    }

}

void CObjTreeView::OnTreeReturn(NMHDR* /*pNMHDR */, LRESULT* pResult) 
{
    BOOL f = FALSE ;
    *pResult = NULL;
    TV_ITEM tvi ;
    tvi.hItem = GetTreeCtrl().GetSelectedItem();
    tvi.mask = TVIF_PARAM  | TVIF_STATE ;
    if (tvi.hItem)
    {
        *pResult = 1;
        if (GetTreeCtrl().GetItem(&tvi) && tvi.lParam)
        {
		    CObjectData* pObjectData = (CObjectData*)tvi.lParam;
		    f = CallInterfaceViewer(pObjectData, tvi.hItem);
        }

        if (f == FALSE)
            GetTreeCtrl().Expand( tvi.hItem, TVE_TOGGLE ) ;
    }
}

void CObjTreeView::OnObjectCreate() 
{
    LRESULT result;
    OnTreeReturn(NULL, &result) ;	
}

void CObjTreeView::OnUpdateObjectCreate(CCmdUI* pCmdUI) 
{
    BOOL f = FALSE ;
    if (GetTreeCtrl().GetSafeHwnd() == CWnd::GetFocus()->GetSafeHwnd())
    {
        TV_ITEM tvi ;
        tvi.hItem = GetTreeCtrl().GetSelectedItem();
        tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
        if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        {
            CObjectData* pOD = (CObjectData*)tvi.lParam ;
            if ((pOD->m_nType == CObjectData::typeObject || pOD->m_nType == CObjectData::typeObjectInstance) &&
                pOD->m_punk == NULL)
                f = TRUE ;
        }
    }
    pCmdUI->Enable( f ) ;
}


void CObjTreeView::OnObjectCreateOn() 
{
    USES_CONVERSION;
    CServerInfoDlg dlg(this);

    dlg.m_strMachine = m_strServerName;
    if (IDOK != dlg.DoModal())
        return ;

	m_strServerName = dlg.m_strMachine;

	ComServerInfo csi;
	memset(&csi, 0, sizeof(csi));
	csi.pszName = T2OLE(m_strServerName );

    TV_ITEM tvi ;
    tvi.hItem = GetTreeCtrl().GetSelectedItem();
    tvi.mask = TVIF_PARAM  | TVIF_STATE ;
	CObjectData* pod=NULL;
    if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam)
        pod= (CObjectData*)tvi.lParam;
    
    COle2ViewDoc*   pDoc = GetDocument() ;
    if (pod && pod->m_punk == NULL)
    {
        HRESULT hr = E_FAIL;
        if (pod->m_nType == CObjectData::typeObject)
            hr = pod->CreateInstance( pDoc->m_dwClsCtx, &csi) ;

        if (FAILED(hr))
            return ;
    }

    GetTreeCtrl().Expand( tvi.hItem, TVE_TOGGLE ) ;
}

void CObjTreeView::OnUpdateObjectCreateOn(CCmdUI* pCmdUI) 
{
    if (g_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && g_osvi.dwMajorVersion >= 4)
        OnUpdateObjectCreate(pCmdUI); 
    else
        pCmdUI->Enable( FALSE ) ;
}

void CObjTreeView::OnObjectRelease() 
{
    TV_ITEM tvi ;
    tvi.hItem = GetTreeCtrl().GetSelectedItem();
    tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
    if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
    {
        CObjectData* pOD = (CObjectData*)tvi.lParam ;
        pOD->Release() ;

        GetTreeCtrl().Expand( tvi.hItem, TVE_COLLAPSERESET | TVE_COLLAPSE) ;
        tvi.mask = TVIF_CHILDREN | TVIF_STATE;
        tvi.stateMask= TVIS_BOLD | TVIS_EXPANDEDONCE ;
        tvi.state &= ~TVIS_BOLD ;
        tvi.state &= ~TVIS_EXPANDEDONCE ;
        tvi.cChildren = 1;
        GetTreeCtrl().SetItem( &tvi );
    }
   
}

void CObjTreeView::OnUpdateObjectRelease(CCmdUI* pCmdUI) 
{
    BOOL f = FALSE ;
    if (GetTreeCtrl().GetSafeHwnd() == CWnd::GetFocus()->GetSafeHwnd())
    {
        TV_ITEM tvi ;
        tvi.hItem = GetTreeCtrl().GetSelectedItem();
        tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
        if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        {
            CObjectData* pOD = (CObjectData*)tvi.lParam ;
            f = (pOD->m_punk) ? TRUE : FALSE ;
        }
    }
    pCmdUI->Enable( f ) ;
}


void CObjTreeView::OnObjectView() 
{
    LRESULT result;
    OnTreeReturn(NULL, &result) ;	
}

void CObjTreeView::OnUpdateObjectView(CCmdUI* pCmdUI) 
{
    BOOL f = FALSE ;
    if (GetTreeCtrl().GetSafeHwnd() == CWnd::GetFocus()->GetSafeHwnd())
    {
        TV_ITEM tvi ;
        tvi.hItem = GetTreeCtrl().GetSelectedItem();
        tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
        if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        {
            CObjectData* pOD = (CObjectData*)tvi.lParam ;
            if ((pOD->m_nType == CObjectData::typeInterface || pOD->m_nType == CObjectData::typeStaticInterface ||pOD->m_nType == CObjectData::typeTypeLib))
                f = TRUE ;
        }
    }
    pCmdUI->Enable( f ) ;
}



void CObjTreeView::OnViewHiddenComCats() 
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    pDoc->m_fViewHiddenComCats = !pDoc->m_fViewHiddenComCats;
    pDoc->UpdateAllViews( NULL, UPD_REFRESH ) ;
}

void CObjTreeView::OnUpdateViewHiddenComCats(CCmdUI* pCmdUI) 
{
    COle2ViewDoc*   pDoc = GetDocument() ;
    pCmdUI->SetCheck( pDoc->m_fViewHiddenComCats ) ;
}

void CObjTreeView::OnObjectAtStorage() 
{
    if (GetTreeCtrl().GetSafeHwnd() == CWnd::GetFocus()->GetSafeHwnd())
    {
        TV_ITEM tvi ;
        tvi.hItem = GetTreeCtrl().GetSelectedItem();
        tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
        if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        {
            CObjectData* pOD = (CObjectData*)tvi.lParam ;
            try
            {
                SetActivateAtStorage(pOD->m_clsid, !GetActivateAtStorage(pOD->m_clsid));
            }
            catch(COleException* e)
            {
                ErrorMessage(_T("Error accessing registry."), e->m_sc ) ;
                e->Delete();
            }
        }
    }
}

void CObjTreeView::OnUpdateObjectAtStorage(CCmdUI* pCmdUI) 
{
    BOOL fCheck = FALSE;
    BOOL fEnable = FALSE;

    if (g_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
        g_osvi.dwMajorVersion >= 4 &&
        (GetTreeCtrl().GetSafeHwnd() == CWnd::GetFocus()->GetSafeHwnd()))
    {
        TV_ITEM tvi ;
        tvi.hItem = GetTreeCtrl().GetSelectedItem();
        tvi.mask = TVIF_HANDLE | TVIF_PARAM ;
        if (tvi.hItem && GetTreeCtrl().GetItem(&tvi) && tvi.lParam != NULL)
        {
            CObjectData* pOD = (CObjectData*)tvi.lParam ;
            if ((pOD->m_nType == CObjectData::typeObject || pOD->m_nType == CObjectData::typeObjectInstance))
            {
                fEnable = TRUE ;
                fCheck = GetActivateAtStorage(pOD->m_clsid);
            }
        }
    }

    pCmdUI->SetCheck( fCheck ) ;
    pCmdUI->Enable( fEnable ) ;
}

BOOL CObjTreeView::SetActivateAtStorage(REFCLSID rclsid, BOOL fSet)
{
    BOOL fPrev = FALSE;
    HKEY hk = NULL;
    HRESULT hr = S_OK;
    WCHAR   wszCLSID[40];
    TCHAR   szBuf[64];

    StringFromGUID2(rclsid, wszCLSID, sizeof(wszCLSID)/sizeof(wszCLSID[0]));
#ifdef _UNICODE
    wsprintf(szBuf, _T("CLSID\\%s\\ActivateAtStorage"), wszCLSID);
#else
    wsprintf(szBuf, _T("CLSID\\%S\\ActivateAtStorage"), wszCLSID);
#endif

    if ((hr = RegOpenKey(HKEY_CLASSES_ROOT, szBuf, &hk )) == ERROR_SUCCESS)
    {
        TCHAR szPrev[64];
        LONG  cb = sizeof(szPrev)/sizeof(szPrev[0]);

        if (RegQueryValue(hk, NULL, szPrev, &cb) == ERROR_SUCCESS)
        {
            if (*szPrev == 'Y' || *szPrev == 'y' || *szPrev == '1')
                fPrev = TRUE;
        }

        if (fPrev != fSet)
        {
            if (fSet == TRUE)
                hr = RegSetValue(hk, NULL, REG_SZ, _T("Yes"), 4 * sizeof(TCHAR));
            else
                hr = RegSetValue(hk, NULL, REG_SZ, _T("No"), 3 * sizeof(TCHAR));
            COle2ViewDoc*   pDoc = GetDocument() ;
            pDoc->UpdateAllViews( this, UPD_NOOBJECTVIEW ) ;
        }

        RegCloseKey(hk);
    }
    else if (fSet == TRUE)
    {
        if ((hr = RegCreateKey(HKEY_CLASSES_ROOT, szBuf, &hk )) == ERROR_SUCCESS)
        {
            hr = RegSetValue(hk, NULL, REG_SZ, _T("Yes"), 4 * sizeof(TCHAR));
            RegCloseKey(hk);
            COle2ViewDoc*   pDoc = GetDocument() ;
            pDoc->UpdateAllViews( this, UPD_NOOBJECTVIEW ) ;
        }
        if (hr != ERROR_SUCCESS)
            AfxThrowOleException(hr);
    }

    return fPrev;

}

BOOL CObjTreeView::GetActivateAtStorage(REFCLSID rclsid)
{
    BOOL f = FALSE;
    HKEY hk = NULL;

    WCHAR   wszCLSID[40];
    TCHAR   szBuf[64];

    StringFromGUID2(rclsid, wszCLSID, sizeof(wszCLSID)/sizeof(wszCLSID[0]));
#ifdef _UNICODE
    wsprintf(szBuf, _T("CLSID\\%s\\ActivateAtStorage"), wszCLSID);
#else
    wsprintf(szBuf, _T("CLSID\\%S\\ActivateAtStorage"), wszCLSID);
#endif

    if (RegOpenKey(HKEY_CLASSES_ROOT, szBuf, &hk ) == ERROR_SUCCESS)
    {
        TCHAR szPrev[64];
        LONG  cb = sizeof(szPrev)/sizeof(szPrev[0]);

        if (RegQueryValue(hk, NULL, szPrev, &cb) == ERROR_SUCCESS)
        {
            if (*szPrev == 'Y' || *szPrev == 'y' || *szPrev == '1')
                f = TRUE;
        }
        RegCloseKey(hk);
    }

    return f;
}
