// iniedvw.cpp : implementation of the CINIeditView class
//

#include "stdafx.h"
#include "iniedit.h"

#include "inieddoc.h"
#include "iniedvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
                        
/////////////////////////////////////////////////////////////////////////////
// CINIeditView

IMPLEMENT_DYNCREATE(CINIeditView, CView)

BEGIN_MESSAGE_MAP(CINIeditView, CView)
    //{{AFX_MSG_MAP(CINIeditView)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CINIeditView construction/destruction

CINIeditView::CINIeditView()
{
    // TODO: add construction code here
}

CINIeditView::~CINIeditView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CINIeditView drawing

void CINIeditView::OnDraw(CDC* pDC)
{
    CINIeditDoc* pDoc = GetDocument();

    // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CINIeditView printing

BOOL CINIeditView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CINIeditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CINIeditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}


/////////////////////////////////////////////////////////////////////////////
// CINIeditView diagnostics

#ifdef _DEBUG
void CINIeditView::AssertValid() const
{
    CView::AssertValid();
}

void CINIeditView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CINIeditDoc* CINIeditView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CINIeditDoc)));
    return (CINIeditDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CINIeditView message handlers
