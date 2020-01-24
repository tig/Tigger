// PropSetView.cpp : implementation file
//

#include "stdafx.h"
#include "oleprop.h"
#include "propset.h"
#include "StgEdit.h"

#include "StgEditItem.h"
#include "ContentsView.h"
#include "PropSetView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct tagVT 
{
	VARTYPE	vt ;
	TCHAR*	psz ;
} VT;

static VT grgVT[] = 
{
    VT_EMPTY,   _T("VT_EMPTY"),
	VT_NULL,	_T("VT_NULL"),
	VT_I2,		_T("VT_I2"),
	VT_I4,		_T("VT_I4"),
	VT_R4,		_T("VT_R4"),
	VT_R8,		_T("VT_R8"),
	VT_CY,		_T("VT_CY"),
	VT_DATE,	_T("VT_DATE"),
	VT_BSTR,	_T("VT_BSTR"),
	VT_ERROR,	_T("VT_ERROR"),
	VT_BOOL,	_T("VT_BOOL"),
	VT_VARIANT,	_T("VT_VARIANT"),
	VT_UI1,		_T("VT_UI1"),
	VT_UI2,		_T("VT_UI2"),
	VT_UI4,		_T("VT_UI4"),
	VT_I8,		_T("VT_I8"),
	VT_UI8,		_T("VT_UI8"),
	VT_LPSTR,	_T("VT_LPSTR"),
	VT_LPWSTR,	_T("VT_LPWSTR"),
	VT_FILETIME,_T("VT_FILETIME"),
	VT_BLOB,	_T("VT_BLOB"),
	VT_STREAM,	_T("VT_STREAM"),
	VT_STORAGE,	_T("VT_STORAGE"),
	VT_STREAMED_OBJECT,	_T("VT_STREAMED_OBJECT"),
	VT_STORED_OBJECT,	_T("VT_STORED_OBJECT"),
	VT_BLOB_OBJECT,	_T("VT_BLOB_OBJECT"),
	VT_CF,		_T("VT_CF"),
	VT_CLSID,	_T("VT_CLSID"),
	VT_VECTOR,	_T("VT_VECTOR"),
	VT_ILLEGAL,	_T("VT_ILLEGAL")
};

#define NUM_VT (sizeof(grgVT)/sizeof(VT))

/////////////////////////////////////////////////////////////////////////////
// CPropertySetView

IMPLEMENT_DYNCREATE(CPropertySetView, CContentsView)

CPropertySetView::CPropertySetView()
{
}

CPropertySetView::~CPropertySetView()
{
}


BEGIN_MESSAGE_MAP(CPropertySetView, CContentsView)
	//{{AFX_MSG_MAP(CPropertySetView)
    ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySetView drawing

void CPropertySetView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPropertySetView diagnostics

#ifdef _DEBUG
void CPropertySetView::AssertValid() const
{
	CContentsView::AssertValid();
}

void CPropertySetView::Dump(CDumpContext& dc) const
{
	CContentsView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropertySetView message handlers

BOOL CPropertySetView::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CContentsView::PreCreateWindow(cs);
}

int CPropertySetView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CContentsView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    CRect rect ;
    GetClientRect( &rect ) ;

    DWORD dw = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
								LBS_EXTENDEDSEL |
                                LBS_HASSTRINGS |
                                LBS_DISABLENOSCROLL | 
                                LBS_NOINTEGRALHEIGHT ;
                          
    if (!m_lb.Create( dw, rect, this, 42 ))
    {
        TRACE( "Listbox failed to create!" ) ;
        return -1 ;
    }
    
    CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject( ANSI_FIXED_FONT ) ) ;
    m_lb.SetFont( pFont ) ;
	
	return 0;
}

void CPropertySetView::OnInitialUpdate()
{
	TRACE0("CPropertySetView::OnInitialUpdate()\n") ;
	
}

void CPropertySetView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
    m_lb.SetRedraw( FALSE ) ;            
	m_lb.ResetContent() ;

	if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CStgEditPsetItem)))
	{
		CStgEditPsetItem* pItem = (CStgEditPsetItem*)pHint ;
	}
    
    m_lb.Invalidate( FALSE ) ;   
    m_lb.SetRedraw( TRUE ) ;
}

void CPropertySetView::OnSize(UINT nType, int cx, int cy)
{
    CContentsView::OnSize(nType, cx, cy);
    
    m_lb.SetWindowPos( NULL, -1, -1, cx+2, cy+2, SWP_NOZORDER ) ;        
}

void CPropertySetView::OnEditCopy() 
{
	MessageBeep(0);
}

void CPropertySetView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((BOOL)m_lb.GetCount());	
}
