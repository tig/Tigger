// cntritem.cpp : implementation of the CWinPrintCntrItem class
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
// CWinPrintCntrItem implementation

IMPLEMENT_SERIAL(CWinPrintCntrItem, COleClientItem, 0)

CWinPrintCntrItem::CWinPrintCntrItem(CWinPrintDoc* pContainer)
    : COleClientItem(pContainer)
{
    m_fConfigView = FALSE ;
    m_windowPlacement.length = 0 ;
    m_fSaveMinPos = FALSE ;
    m_fTopMost = FALSE ;
    
    m_rect.SetRect(0, 0, 100, 100);
}

CWinPrintCntrItem::~CWinPrintCntrItem()
{
    // TODO: add cleanup code here
}

void CWinPrintCntrItem::InvalidateItem()
{
    GetDocument()->UpdateAllViews(NULL, CWinPrintDoc::updateItem, this);
}

void CWinPrintCntrItem::UpdateFromServerExtent()
{
    CSize size;
    if (GetExtent(&size))
    {
        // OLE returns the extent in HIMETRIC units -- we need pixels
        CClientDC dc(NULL);
        dc.HIMETRICtoDP(&size);

        // only invalidate if it has actually changed
        if (size != m_rect.Size())
        {
            // invalidate old, update, invalidate new
            InvalidateItem();
            m_rect.bottom = m_rect.top + size.cy;
            m_rect.right = m_rect.left + size.cx;
            InvalidateItem();

            // mark document as modified
            GetDocument()->SetModifiedFlag();
        }
    }
}

void CWinPrintCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
    ASSERT_VALID(this);

    COleClientItem::OnChange(nCode, dwParam);

    // When an item is being edited (either in-place or fully open)
    //  it sends OnChange notifications for changes in the state of the
    //  item or visual appearance of its content.

    switch (nCode)
    {
        case OLE_CHANGED:
            InvalidateItem();
            UpdateFromServerExtent();
        break;
        
        case OLE_CHANGED_STATE:
        case OLE_CHANGED_ASPECT:
            InvalidateItem();
        break;
    }
}

BOOL CWinPrintCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
    ASSERT_VALID(this);

    // During in-place activation CWinPrintCntrItem::OnChangeItemPosition
    //  is called by the server to change the position on of the in-place
    //  window.  Usually, this is a result of the data in the server
    //  document changing such that the extent has changed or as a result
    //  of in-place resizing.
    //
    // The default here is to call the base class, which will call
    //  COleClientItem::SetItemRects to move the item
    //  to the new position.

    if (!COleClientItem::OnChangeItemPosition(rectPos))
        return FALSE;

    InvalidateItem();
    m_rect = rectPos;
    InvalidateItem();

    // mark document as dirty
    GetDocument()->SetModifiedFlag();

    return TRUE;
}

void CWinPrintCntrItem::OnGetItemPosition(CRect& rPosition)
{
    ASSERT_VALID(this);

    // During in-place activation, CWinPrintCntrItem::OnGetItemPosition
    //  will be called to determine the location of this item.  The default
    //  implementation created from AppWizard simply returns a hard-coded
    //  rectangle.  Usually, this rectangle would reflect the current
    //  position of the item relative to the view used for activation.
    //  You can obtain the view by calling CWinPrintCntrItem::GetActiveView.

    // TODO: return correct rectangle (in pixels) in rectPos

    rPosition = m_rect;
}

void CWinPrintCntrItem::OnDeactivateUI(BOOL bUndoable)
{
    COleClientItem::OnDeactivateUI(bUndoable);

    // Close an in-place active item whenever it removes the user
    //  interface.  The action here should match as closely as possible
    //  to the handling of the escape key in the view.

    Deactivate();   // nothing fancy here -- just deactivate the object
}

void CWinPrintCntrItem::Serialize(CArchive& ar)
{
    ASSERT_VALID(this);

    CDocItem::Serialize(ar);
    ASSERT(m_pDocument != NULL);  // must 'SetDocument' first

    if (ar.IsStoring())
    {
        ASSERT(m_lpObject != NULL);
        WriteItem(ar);
    }
    else
    {
        ASSERT(m_lpObject == NULL);
        ReadItem(ar);

        // finish OLE object creation process, setup advises, etc.
        if (!FinishCreate(NOERROR))
            AfxThrowArchiveException(CArchiveException::generic);
    }
}

void CWinPrintCntrItem::OnUpdateFrameTitle()
{
    // Override and do nothing
}


void CWinPrintCntrItem::GetItemStorage()
{
    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    ASSERT(pDoc->m_lpRootStg != NULL);
    ASSERT(pDoc->m_bCompoundFile);

    // create storage for this item
    LPSTORAGE lpStorage;
    HRESULT hr = pDoc->m_lpRootStg->CreateStorage(m_strConfigName,
        STGM_CREATE|STGM_READWRITE|STGM_TRANSACTED|STGM_SHARE_EXCLUSIVE,
        0, 0, &lpStorage);
    if (hr != NOERROR)
    {
        TRACE1("Warning: unable to create child storage %s\n", (LPCSTR)m_strConfigName);
        // upon failure throw file exception (item will be cleaned up)
        AfxThrowOleException(hr);
        ASSERT(FALSE);
    }
    ASSERT(lpStorage != NULL);

    // everything should have worked
    m_lpStorage = lpStorage;
    ASSERT(m_lpStorage != NULL);
}

void CWinPrintCntrItem::ReadItem(CArchive& ar)
{
    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    ASSERT(pDoc->m_lpRootStg != NULL);
    ASSERT(pDoc->m_bCompoundFile);
    ASSERT(m_lpStorage == NULL);
    ASSERT(m_lpLockBytes == NULL);
    ASSERT(!ar.m_bForceFlat) ;

    // open storage for this item
    LPSTORAGE lpStorage;
    HRESULT hr = pDoc->m_lpRootStg->OpenStorage( m_strConfigName, NULL,
        STGM_READWRITE|STGM_TRANSACTED|STGM_SHARE_EXCLUSIVE,
        0, 0, &lpStorage);
    if (hr != NOERROR)
    {
        TRACE1("Warning: unable to open child storage %s\n", (LPCSTR)m_strConfigName);
        // upon failure throw file exception (item will be cleaned up)
        AfxThrowOleException(hr);
    }
    ASSERT(lpStorage != NULL);

    // remember the storage
    m_lpStorage = lpStorage;
    ASSERT(m_lpStorage != NULL);

    // attempt to load the object from the storage
    hr = ::OleLoad(m_lpStorage, IID_IOleObject, GetClientSite(), (LPVOID FAR*)&m_lpObject);
    if (FAILED(hr))
    {
        TRACE1("Warning: OleLoad failed for %s\n", (LPCSTR)m_strConfigName);
        // upon failure throw file exception (item will be cleaned up)
        AfxThrowOleException(hr);
    }
    
    // Now load the WinPrint 2.0 Container information
    //

    // open Contents stream
    COleStreamFile file;
    CFileException fe;
    if (!file.OpenStream(lpStorage, "WinPrint 2.0",
        CFile::modeReadWrite|CFile::shareExclusive, &fe))
    {
        // No error, just bail
        //
        m_windowPlacement.length = 0;
        m_fConfigView = FALSE ;
        return ;
    }

    // load it with CArchive (loads from Contents stream)
    CArchive loadArchive(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    loadArchive.m_pDocument = pDoc ;
    loadArchive.m_bForceFlat = FALSE ;

    TRY
    {
        loadArchive >> (LONG&)m_windowPlacement.length;
        loadArchive >> (LONG&)m_windowPlacement.flags;
        loadArchive >> (LONG&)m_windowPlacement.showCmd;
        loadArchive >> m_windowPlacement.ptMinPosition;
        loadArchive >> m_windowPlacement.ptMaxPosition;
        loadArchive >> m_windowPlacement.rcNormalPosition;

        loadArchive >> (LONG&)m_fConfigView ;

        loadArchive >> (LONG&)m_fSaveMinPos ;
        loadArchive >> (LONG&)m_fTopMost ;

        if (m_fSaveMinPos == FALSE)
            m_windowPlacement.ptMinPosition.x = m_windowPlacement.ptMinPosition.y = -1 ;

        loadArchive.Close();
        file.Close();
    }
    CATCH_ALL(e)
    {
        file.Abort();   // will not throw an exception
        loadArchive.CArchive::~CArchive();
        
        THROW_LAST();
    }
    END_CATCH_ALL
}

void CWinPrintCntrItem::WriteItem(CArchive& ar)
{
    CWinPrintDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    ASSERT(pDoc->m_lpRootStg != NULL);
    ASSERT(pDoc->m_bCompoundFile);
    ASSERT(m_lpNewStorage == NULL);
    ASSERT(!ar.m_bForceFlat);

    // determine destination storage
    ASSERT(m_lpStorage != NULL);
    LPSTORAGE lpStorage = m_lpStorage;
    if (!pDoc->m_bSameAsLoad)
    {
        // need to create new storage for this item
        HRESULT hr = pDoc->m_lpRootStg->CreateStorage(m_strConfigName,
            STGM_CREATE|STGM_READWRITE|STGM_TRANSACTED|STGM_SHARE_EXCLUSIVE,
            0, 0, &lpStorage);
        if (hr != NOERROR)
        {
            TRACE1("Warning: unable to create child storage %s\n", (LPCSTR)m_strConfigName);
            AfxThrowOleException(hr);
        }
        // remember the storage for CommitItem stage
        m_lpNewStorage = lpStorage;
        m_bNeedCommit = TRUE;
    }
    ASSERT(lpStorage != NULL);

    // save dirty object
    LPPERSISTSTORAGE lpPersistStorage = NULL ;
    m_lpObject->QueryInterface( IID_IPersistStorage, (LPVOID FAR*)&lpPersistStorage ) ;
    ASSERT(lpPersistStorage != NULL);

    HRESULT hr = NOERROR;
    if (!pDoc->m_bSameAsLoad || lpPersistStorage->IsDirty() == NOERROR)
    {
        // call OleSave now and IPersistStorage::Save later
        hr = ::OleSave(lpPersistStorage, lpStorage, pDoc->m_bSameAsLoad);
    }
    lpPersistStorage->Release();

    // if it fails, abort the save
    if (FAILED(hr))
        AfxThrowOleException(hr);

    // Now save WinPrint stuff
    //
    COleStreamFile file;
    CFileException fe;
    if (!file.CreateStream(lpStorage, "WinPrint 2.0",
        CFile::modeWrite|CFile::shareExclusive|CFile::modeCreate, &fe))
    {
        AfxThrowFileException(fe.m_cause, fe.m_lOsError);
    }

    // save to Contents stream
    CArchive saveArchive(&file, CArchive::store | CArchive::bNoFlushOnDelete);
    saveArchive.m_pDocument = pDoc;
    saveArchive.m_bForceFlat = FALSE;

    TRY
    {                  
        if (m_pView)
        {
            m_windowPlacement.length = sizeof(WINDOWPLACEMENT) ;
            m_pView->GetParent()->GetWindowPlacement( &m_windowPlacement ) ;
        }
        saveArchive << (LONG)m_windowPlacement.length;
        saveArchive << (LONG)m_windowPlacement.flags;
        saveArchive << (LONG)m_windowPlacement.showCmd;
        saveArchive << m_windowPlacement.ptMinPosition;
        saveArchive << m_windowPlacement.ptMaxPosition;
        saveArchive << m_windowPlacement.rcNormalPosition;

        saveArchive << (LONG)m_fConfigView ;

        saveArchive << (LONG)m_fSaveMinPos ;
        saveArchive << (LONG)m_fTopMost ;
        
        saveArchive.Close();
        file.Close();
    }
    CATCH_ALL(e)
    {
        file.Abort();   // will not throw an exception
        saveArchive.CArchive::~CArchive();

        THROW_LAST();
    }
    END_CATCH_ALL

    // now will need to call CommitItem for this item
    m_bNeedCommit = TRUE;
    lpStorage->Commit(STGC_ONLYIFCURRENT);
}

void CWinPrintCntrItem::CommitItem(BOOL bSuccess)
{
    ASSERT_VALID(this);
    ASSERT(m_lpObject != NULL);

    if (!m_bNeedCommit)
        return;

    LPPERSISTSTORAGE lpPersistStorage = NULL ;
    m_lpObject->QueryInterface( IID_IPersistStorage, (LPVOID FAR*)&lpPersistStorage ) ;
    ASSERT(lpPersistStorage != NULL);

    // forget about new storage if save failed along the way...
    if (m_lpNewStorage != NULL && !bSuccess)
        m_lpNewStorage->Release() ;

    // let the object remember the new storage
    VERIFY(lpPersistStorage->SaveCompleted(m_lpNewStorage) == NOERROR);
    lpPersistStorage->Release();

    // determine/remember new storage
    if (m_lpNewStorage != NULL)
    {
        m_lpStorage->Release();
        m_lpStorage = m_lpNewStorage;
        m_lpNewStorage = NULL;
    }

    m_bNeedCommit = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWinPrintCntrItem diagnostics

#ifdef _DEBUG
void CWinPrintCntrItem::AssertValid() const
{
    COleClientItem::AssertValid();
}

void CWinPrintCntrItem::Dump(CDumpContext& dc) const
{
    COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
