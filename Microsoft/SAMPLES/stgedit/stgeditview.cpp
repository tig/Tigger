// StgEditView.cpp : implementation of the CStgEditView class
//

#include "stdafx.h"
#include "propset.h"
#include "StgEdit.h"
#include "mainfrm.h"

#include "StgEditItem.h"

#include "StgEditDoc.h"
#include "ContentsView.h"
#include "StorageView.h"
#include "StreamView.h"
#include "PropSetView.h"
#include "PropView.h"

#include "StgEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStgEditView

int const ID_TREEVIEW = 42 ;

IMPLEMENT_DYNCREATE(CStgEditView, CView)

BEGIN_MESSAGE_MAP(CStgEditView, CView)
	//{{AFX_MSG_MAP(CStgEditView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, ID_TREEVIEW, OnTreeSelchanged)
	ON_NOTIFY(TVN_ITEMEXPANDING, ID_TREEVIEW, OnTreeItemExpanding)
	ON_NOTIFY(NM_RETURN, ID_TREEVIEW, OnTreeItemReturn)
    ON_NOTIFY(TVN_DELETEITEM, ID_TREEVIEW, OnTreeDeleteItem)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStgEditView construction/destruction

CStgEditView::CStgEditView()
{
}

CStgEditView::~CStgEditView()
{
}

BOOL CStgEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CStgEditView drawing

void CStgEditView::OnDraw(CDC* pDC)
{
	CStgEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CStgEditView diagnostics

#ifdef _DEBUG
void CStgEditView::AssertValid() const
{
	CView::AssertValid();
}

void CStgEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStgEditDoc* CStgEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStgEditDoc)));
	return (CStgEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStgEditView message handlers
int CStgEditView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    RECT rect ;
    rect.top = lpCreateStruct->y ;
    rect.left = lpCreateStruct->x ;
    rect.bottom = lpCreateStruct->cy ;
    rect.right = lpCreateStruct->cx ;

    if (!m_tree.Create( WS_CHILD | WS_VISIBLE |
                                WS_BORDER | 
                                TVS_HASLINES | 
                                TVS_LINESATROOT |
                                TVS_SHOWSELALWAYS |
                                TVS_HASBUTTONS |
                                TVS_DISABLEDRAGDROP,
                                rect, this, ID_TREEVIEW ))
    {
        TRACE( _T("Tree control failed to create!") ) ;
        return -1 ;
    }

	return 0;
}

void CStgEditView::OnDestroy() 
{
	m_tree.SelectItem(NULL);
	m_tree.DeleteAllItems() ;
	CView::OnDestroy();
}

void CStgEditView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
    m_tree.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;
}


void CStgEditView::OnInitialUpdate() 
{
	if (m_images.m_hImageList == NULL)
	{
		m_images.Create(IDB_STGEDITIMAGES, 16, 16, RGB(255,0,0));
		m_tree.SetImageList(&m_images, TVSIL_NORMAL) ;
	}

	CView::OnInitialUpdate();
}

void CStgEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CStgEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	HRESULT hr ;
    TV_INSERTSTRUCT tvis ;

	m_tree.DeleteAllItems() ;

	if (pDoc->m_lpRootStg == NULL)
		return ;
	
	try
	{
		// Root as a storage (potentially multiple propsets)
		STATSTG statstg ;
#ifndef _UNICODE
		char	szName[_MAX_PATH] ;
#endif

		if (FAILED(hr = pDoc->m_lpRootStg->Stat(&statstg, STATFLAG_DEFAULT)))
			AfxThrowOleException(hr) ;
		
		HTREEITEM hitem ;
		tvis.hParent = TVI_ROOT ;
		tvis.hInsertAfter = TVI_LAST ;
		tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN ;
		tvis.item.lParam = 0 ;
		tvis.item.cChildren = 1 ;

		tvis.item.iImage = TYPE_TO_ID(statstg.type) ;
		tvis.item.iSelectedImage = TYPE_TO_ID(statstg.type)+1 ;
#ifdef _UNICODE
		tvis.item.pszText = statstg.pwcsName ;
#else
		WideCharToMultiByte(CP_ACP, 0, statstg.pwcsName, wcslen(statstg.pwcsName)+1, szName, _MAX_PATH, NULL, NULL);
		tvis.item.pszText = szName;
#endif
		CStgEditStgItem* pItem = new CStgEditStgItem(&m_tree, pDoc->m_lpRootStg) ;
		tvis.item.lParam = (LPARAM)pItem ;
		hitem = m_tree.InsertItem(&tvis);

		CoTaskMemFree(statstg.pwcsName) ;			
		
		m_tree.Expand(hitem, TVE_EXPAND) ;
	}
	catch(CException* e)
	{
		TRACE0("Exception in Reload\n");
		e->Delete() ;
	};
}

void CStgEditView::OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0 ;
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;

    if ((pnmtv->action != TVE_EXPAND && pnmtv->itemNew.lParam) ||
        (pnmtv->itemNew.state & TVIS_EXPANDEDONCE))
        return ;

    CStgEditItem*  pItem = (CStgEditItem*)pnmtv->itemNew.lParam ;
    if (pItem == NULL)
        return ;
    ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CStgEditItem)));

    BeginWaitCursor() ;
    m_tree.SetRedraw( FALSE ) ;

    *pResult = !pItem->Expand( pnmtv->itemNew.hItem ) ;

    // No children
    if (*pResult == TRUE)
    {
        TV_ITEM item ;
        item.cChildren = 0 ;
        item.mask = TVIF_CHILDREN ;
        item.hItem = pnmtv->itemNew.hItem ;
        m_tree.SetItem( &item ) ;
    }

    m_tree.SetRedraw( TRUE ) ;
    EndWaitCursor() ;
}

void CStgEditView::OnTreeItemReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = (LRESULT)TRUE ;
    HTREEITEM hTreeItemSel = m_tree.GetSelectedItem() ;

    if (hTreeItemSel)
        m_tree.Expand( hTreeItemSel, TVE_TOGGLE ) ;
}

inline void CStgEditView::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = (LRESULT)TRUE ;
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;

    if (pnmtv->itemOld.lParam)
    {
        CStgEditItem* pItem = (CStgEditItem*)pnmtv->itemOld.lParam ;
        ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CStgEditItem)));
        delete pItem ;
        //pnmtv->itemOld.lParam = NULL ;
    }

}

void CStgEditView::OnTreeSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CStgEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
    CStgEditItem*  pItem = (CStgEditItem*)pnmtv->itemNew.lParam ;

	CView* pView = (CView*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(0,1) ;
	if (pItem && pView)
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CStgEditPsetItem)))
		{
			((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.ReplaceView(0,1,RUNTIME_CLASS(CPropertySetView), CSize(0,0)); 
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CStgEditStmItem)))
		{
			((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.ReplaceView(0,1,RUNTIME_CLASS(CStreamView), CSize(0,0)) ;
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CStgEditStgItem)))
		{
			((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.ReplaceView(0,1,RUNTIME_CLASS(CStorageView), CSize(0,0));
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CStgEditPropItem)))
		{
			((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.ReplaceView(0,1,RUNTIME_CLASS(CPropertyView), CSize(0,0));
		}
	}

	pDoc->UpdateAllViews( this, 0, pItem ) ;
}


void CStgEditView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	m_tree.SetFocus();
}
