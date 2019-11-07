// cntritem.h : interface of the CWinPrintCntrItem class
//

class CWinPrintDoc;
class CConfigView;

class CWinPrintCntrItem : public COleClientItem
{
    DECLARE_SERIAL(CWinPrintCntrItem)

// Constructors
public:
    CWinPrintCntrItem(CWinPrintDoc* pContainer = NULL);
        // Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
        //  IMPLEMENT_SERIALIZE requires the class have a constructor with
        //  zero arguments.  Normally, OLE items are constructed with a
        //  non-NULL document pointer.

// Attributes
public:
    // TypeKind let's us differentiate between items that are
    // configurations, jobs in the queue, or print previews.
    //
    enum TypeFlag
    {
        typeDefault =           0x0000,
        typeConfiguration =     0x0001,
        typeQueueJob =          0x0002
    } ;
    
    CString     m_strConfigName ;
    TypeFlag    m_itemType ;

    CRect       m_rect;   // position within the document

    WINDOWPLACEMENT m_windowPlacement ;
    BOOL            m_fConfigView ;
    BOOL            m_fSaveMinPos ;
    BOOL            m_fTopMost ;
    
    CWinPrintDoc* GetDocument()
        { return (CWinPrintDoc*)COleClientItem::GetDocument(); }
    CConfigView* GetActiveView()
        { return (CConfigView*)COleClientItem::GetActiveView(); }

    void InvalidateItem();
    void UpdateFromServerExtent();

    virtual void GetItemStorage();  // allocate storage for new item
    virtual void ReadItem(CArchive& ar);    // read item from archive
    virtual void WriteItem(CArchive& ar);   // write item to archive
    virtual void CommitItem(BOOL bSuccess); // commit item's storage

// Implementation
public:
    ~CWinPrintCntrItem();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void Serialize(CArchive& ar);
    virtual void OnGetItemPosition(CRect& rPosition);
    virtual void OnDeactivateUI(BOOL bUndoable);

    virtual void OnUpdateFrameTitle();

protected:
    virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
    virtual BOOL OnChangeItemPosition(const CRect& rectPos);
};

/////////////////////////////////////////////////////////////////////////////
