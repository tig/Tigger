// Doc.cpp : implementation of the CDoc class
//

#include "stdafx.h"
#include "OLECam.h"
#include "dataobj.h"
#include "Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDoc

IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
	//{{AFX_MSG_MAP(CDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CDoc, CDocument)
    INTERFACE_AGGREGATE(CDoc, m_lpAggrInner)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoc construction/destruction

CDoc::CDoc()
{
	// TODO: add one-time construction code here
    m_strImagePath = AfxGetApp()->GetProfileString(_T("Settings"), _T("DefaultImage"));
    m_lpAggrInner = NULL;

	EnableAutomation();

	AfxOleLockApp();
}


BOOL CDoc::OnCreateAggregates()
{
    // wire up aggregate with correct controlling unknown
    CGenericDataObject* p = new CGenericDataObject();
    m_lpAggrInner = (IUnknown*)&p->m_xInnerUnknown;
    if (m_lpAggrInner == NULL)
    {
        return FALSE;
    }

    IDataObject* pdo=NULL;
    m_lpAggrInner->QueryInterface(IID_IDataObject, (void**)&pdo);
    pdo->Release();


    p->m_pOuterUnknown = GetControllingUnknown();
    // optionally, create other aggregate objects here
    return TRUE;
}

CDoc::~CDoc()
{
	AfxOleUnlockApp();
}

void CDoc::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDocument::OnFinalRelease();
}

BOOL CDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDoc serialization

void CDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDoc diagnostics

#ifdef _DEBUG
void CDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDoc commands

