// coltedoc.cpp : implementation of the CColtestDoc class
//

#include "stdafx.h"
#include "coltest.h"

#include "coltedoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColtestDoc

IMPLEMENT_DYNCREATE(CColtestDoc, CDocument)

BEGIN_MESSAGE_MAP(CColtestDoc, CDocument)
    //{{AFX_MSG_MAP(CColtestDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColtestDoc construction/destruction

CColtestDoc::CColtestDoc()
{
    m_strFileSpec = "*.*" ;
}

CColtestDoc::~CColtestDoc()
{
}

BOOL CColtestDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    SetTitle(m_strFileSpec);
    return TRUE;
}

BOOL CColtestDoc::OnOpenDocument(const char* pszPathName)
{
    m_strFileSpec = pszPathName ;

    SetPathName(pszPathName);
    SetModifiedFlag(FALSE);     // start off with unmodified
    SetTitle(pszPathName);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CColtestDoc serialization

void CColtestDoc::Serialize(CArchive& ar)
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
// CColtestDoc diagnostics

#ifdef _DEBUG
void CColtestDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CColtestDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColtestDoc commands
