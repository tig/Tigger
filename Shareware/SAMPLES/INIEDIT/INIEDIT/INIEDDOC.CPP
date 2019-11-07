// inieddoc.cpp : implementation of the CINIeditDoc class
//

#include "stdafx.h"
#include "iniedit.h"

#include "inieddoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CINIeditDoc

IMPLEMENT_DYNCREATE(CINIeditDoc, CDocument)

BEGIN_MESSAGE_MAP(CINIeditDoc, CDocument)
    //{{AFX_MSG_MAP(CINIeditDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CINIeditDoc construction/destruction

CINIeditDoc::CINIeditDoc()
{
    // TODO: add one-time construction code here
}

CINIeditDoc::~CINIeditDoc()
{
}

BOOL CINIeditDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
        
    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)
                     
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CINIeditDoc diagnostics

#ifdef _DEBUG
void CINIeditDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CINIeditDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CINIeditDoc commands
