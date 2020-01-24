// flistvw.cpp : implementation file
//

#include "stdafx.h"
#include "iniedit.h"     

#include "flistdoc.h"
#include "flistvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFListView

IMPLEMENT_DYNCREATE(CFListView, CView)


BEGIN_MESSAGE_MAP(CFListView, CView)
    //{{AFX_MSG_MAP(CFListView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CFListView::CFListView()
{

}

CFListView::~CFListView()
{
}


CListBox* CFListView::GetListBox()
{
    return &m_listboxFiles ;
}

/////////////////////////////////////////////////////////////////////////////
// CFListView drawing

void CFListView::OnDraw(CDC* pDC)
{
//    CFListDoc* pDoc = GetDocument();

    // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFListView printing

BOOL CFListView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CFListView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CFListView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}


/////////////////////////////////////////////////////////////////////////////
// CFListView diagnostics

#ifdef _DEBUG
void CFListView::AssertValid() const
{
    CView::AssertValid();
}

void CFListView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CFListDoc* CFListView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFListDoc)));
    return (CFListDoc*) m_pDocument;
}

#endif //_DEBUG

int CFListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
        
    // TODO: Add your specialized creation code here
                                                
    RECT rect ;
    rect.top = lpCreateStruct->y ;
    rect.left = lpCreateStruct->x ;
    rect.bottom = lpCreateStruct->cy ;
    rect.right = lpCreateStruct->cx ;
                                                    
    if (!m_listboxFiles.Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                                LBS_DISABLENOSCROLL | 
                                LBS_NOINTEGRALHEIGHT |
                                LBS_HASSTRINGS, 
                                rect, this, 0 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
                 
    CFListDoc* pDoc = GetDocument() ;                    
    POSITION pos = pDoc->m_pINIfiles->GetHeadPosition() ;
    while (pos != NULL)
    {   
        m_listboxFiles.AddString( ((UINIfile*)pDoc->m_pINIfiles->GetAt( pos ))->GetName() ) ;
        pDoc->m_pINIfiles->GetNext( pos ) ;
    }
    
    return 0;
}

void CFListView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    GetListBox()->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER ) ;    
}
