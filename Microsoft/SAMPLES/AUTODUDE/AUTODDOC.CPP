//  autoddoc.cpp : implementation of the CAutoDudeDoc class
//

#include "stdafx.h"
#include "autodude.h"

#include "autoddoc.h"

                
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeDoc

IMPLEMENT_DYNCREATE(CAutoDudeDoc, CDocument)

BEGIN_MESSAGE_MAP(CAutoDudeDoc, CDocument)
    //{{AFX_MSG_MAP(CAutoDudeDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeDoc construction/destruction

CAutoDudeDoc::CAutoDudeDoc()
{
    // TODO: add one-time construction code here
    SetText("") ;
}

CAutoDudeDoc::~CAutoDudeDoc()
{
}

BOOL CAutoDudeDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeDoc serialization

void CAutoDudeDoc::Serialize(CArchive& ar)
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
// CAutoDudeDoc diagnostics

#ifdef _DEBUG
void CAutoDudeDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CAutoDudeDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAutoDudeDoc commands
