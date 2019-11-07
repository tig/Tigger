// winprdoc.cpp : implementation of the CWinPrintDoc class
//

#include "stdafx.h"
#include "winprint.h"

#include "winprdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc

IMPLEMENT_DYNCREATE(CWinPrintDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinPrintDoc, CDocument)
	//{{AFX_MSG_MAP(CWinPrintDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc construction/destruction

CWinPrintDoc::CWinPrintDoc()
{
	// TODO: add one-time construction code here
}

CWinPrintDoc::~CWinPrintDoc()
{
}

BOOL CWinPrintDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc serialization

void CWinPrintDoc::Serialize(CArchive& ar)
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
// CWinPrintDoc diagnostics

#ifdef _DEBUG
void CWinPrintDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinPrintDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc commands
