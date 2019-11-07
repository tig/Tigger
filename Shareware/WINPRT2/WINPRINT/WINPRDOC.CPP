// winprdoc.cpp : implementation of the CWinPrintDoc class
//

#include "stdafx.h"
#include "winprint.h"

#include "winprdoc.h"
#include "cntritem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc

IMPLEMENT_DYNCREATE(CWinPrintDoc, COleDocument)

BEGIN_MESSAGE_MAP(CWinPrintDoc, COleDocument)
    //{{AFX_MSG_MAP(CWinPrintDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Enable default OLE container implementation
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWinPrintDoc, COleDocument)
    //{{AFX_DISPATCH_MAP(CWinPrintDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //      DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc construction/destruction

CWinPrintDoc::CWinPrintDoc()
{
    // For most containers, using compound files is a good idea.
    EnableCompoundFile();

    EnableAutomation();

    // TODO: add one-time construction code here

    AfxOleLockApp();
}

CWinPrintDoc::~CWinPrintDoc()
{
    AfxOleUnlockApp();
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc serialization
void CWinPrintDoc::Serialize(CArchive& ar)
{
    POSITION pos ;
    if (ar.IsStoring())
    {
        // Go through all configurations and save them
        //
        // serialize all the items in the list
        pos = GetStartPosition();
        while (pos != NULL)
        {
            CWinPrintCntrItem* pDocItem = (CWinPrintCntrItem*)GetNextItem(pos);
            ASSERT_VALID(pDocItem);

            // only write non-blank ones
            if (!pDocItem->IsBlank())
                ar << pDocItem;
        }
    }
    else
    {
        ASSERT( m_lpRootStg ) ;
        // Enumerate all sub-storages of the document's IStorage
        // and see if it's a configuration.  If it is, use the
        // "Contents" stream to load the object
        //
        // Enumerate all sub-streams
        //
        LPENUMSTATSTG pEnum ;
        if (FAILED(m_lpRootStg->EnumElements( 0, NULL, 0, &pEnum )))
        {
            TRACE( "EnumElements failed on storage file.\n") ;
            AfxThrowArchiveException(CArchiveException::generic);
        }                
        
        STATSTG stat ;
        LPMALLOC pMalloc;
        CoGetMalloc(MEMCTX_TASK, &pMalloc);
        while (pEnum->Next( 1, &stat, NULL ) == S_OK)
        {
            if (stat.type == STGTY_STORAGE)
            {
                CWinPrintCntrItem*   pDocItem = NULL ;
                TRY
                {
                    pDocItem = new CWinPrintCntrItem ;
                    ASSERT(pDocItem) ;
                    pDocItem->m_strConfigName = stat.pwcsName ;
                    pDocItem->Serialize(ar) ;
                    pDocItem->m_itemType = CWinPrintCntrItem::typeConfiguration ;
                }
                CATCH_ALL(e)
                {
                    TRACE( "Failed to create and de-serialize item (%s)\n", (LPCSTR)stat.pwcsName ) ;
                    if (pDocItem)
                        delete pDocItem ;
                }
                END_CATCH_ALL
            }
            pMalloc->Free( stat.pwcsName ) ;
        }
        pMalloc->Release();
        pEnum->Release() ;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc diagnostics

#ifdef _DEBUG
void CWinPrintDoc::AssertValid() const
{
    COleDocument::AssertValid();
}

void CWinPrintDoc::Dump(CDumpContext& dc) const
{
    COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinPrintDoc commands
