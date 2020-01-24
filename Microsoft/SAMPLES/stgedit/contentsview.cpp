// ContentsView.cpp : implementation file
//

#include "stdafx.h"
#include "propset.h"
#include "StgEdit.h"

#include "ContentsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContentsView

IMPLEMENT_DYNCREATE(CContentsView, CView)

CContentsView::CContentsView()
{
}

CContentsView::~CContentsView()
{
}


BEGIN_MESSAGE_MAP(CContentsView, CView)
	//{{AFX_MSG_MAP(CContentsView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContentsView drawing

void CContentsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CContentsView diagnostics

#ifdef _DEBUG
void CContentsView::AssertValid() const
{
	CView::AssertValid();
}

void CContentsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CContentsView message handlers

void CContentsView::OnEditCopy() 
{
}

void CContentsView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
}

void CContentsView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
}

void CContentsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CView::OnUpdate( pSender, lHint, pHint );
}
