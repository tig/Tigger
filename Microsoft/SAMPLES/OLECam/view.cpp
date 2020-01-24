// View.cpp : implementation of the CSrvView class
//

#include "stdafx.h"
#include "OLECam.h"

#include "Doc.h"
#include "View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSrvView

IMPLEMENT_DYNCREATE(CSrvView, CFormView)

BEGIN_MESSAGE_MAP(CSrvView, CFormView)
	//{{AFX_MSG_MAP(CSrvView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_COMMAND(ID_FILE_SETDEFAULTIMAGE, OnFileSetDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSrvView construction/destruction

CSrvView::CSrvView()
	: CFormView(CSrvView::IDD)
{
    //{{AFX_DATA_INIT(CSrvView)
	//}}AFX_DATA_INIT
}

CSrvView::~CSrvView()
{
}

void CSrvView::DoDataExchange(CDataExchange* pDX)
{
    CDoc* pdoc = GetDocument();
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSrvView)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_IMAGE, pdoc->m_strImagePath);
}

BOOL CSrvView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSrvView diagnostics

#ifdef _DEBUG
void CSrvView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSrvView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDoc* CSrvView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSrvView message handlers

int CSrvView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	return 0;
}

void CSrvView::OnApply() 
{
    UpdateData(TRUE);
	
    // Release current image and bind to new one
}

void CSrvView::OnBrowse() 
{
    CDoc* pdoc = GetDocument();
	static TCHAR szFilter[] = _T("Image Files (*.bmp)|*.bmp|AllFiles(*.*)|*.*|") ;

	CFileDialog dlg(TRUE, pdoc->m_strImagePath, NULL,
					OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
					szFilter, this);
	if (IDOK == dlg.DoModal())
	{
		pdoc->m_strImagePath = dlg.GetPathName() ;
        UpdateData(FALSE);
	}
}

// CDefaultImg dialog


CDefaultImg::CDefaultImg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefaultImg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefaultImg)
	m_strImage = _T("");
	//}}AFX_DATA_INIT
}


void CDefaultImg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultImg)
	DDX_Text(pDX, IDC_IMAGE, m_strImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefaultImg, CDialog)
	//{{AFX_MSG_MAP(CDefaultImg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefaultImg message handlers

void CSrvView::OnFileSetDefault() 
{
    CDefaultImg dlg;
    
    dlg.m_strImage = AfxGetApp()->GetProfileString(_T("Settings"), _T("DefaultImage"));
    if (IDOK==dlg.DoModal())
    {
        AfxGetApp()->WriteProfileString(_T("Settings"), _T("DefaultImage"), dlg.m_strImage);
    }	
}

void CDefaultImg::OnBrowse() 
{
	static TCHAR szFilter[] = _T("Image Files (*.bmp)|*.bmp|AllFiles(*.*)|*.*|") ;

	CFileDialog dlg(TRUE, _T(""), NULL,
					OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
					szFilter, this);
	if (IDOK == dlg.DoModal())
	{
		m_strImage = dlg.GetPathName() ;
        UpdateData(FALSE);
	}
}
