// stockdoc.h : interface of the CStocksDoc class
//
/////////////////////////////////////////////////////////////////////////////


#ifndef _STOCKDOC_H_
#define _STOCKDOC_H_

class CStocksSrvrItem;
class CStockItems ;

class CStocksDoc : public COleServerDoc
{
protected: // create from serialization only
    CStocksDoc();
    DECLARE_DYNCREATE(CStocksDoc)

// Attributes
public:
    CStocksSrvrItem* GetEmbeddedItem()
        { return (CStocksSrvrItem*)COleServerDoc::GetEmbeddedItem(); }
        
    CStockItems*   m_pItems ;
    
// Operations
public:

// Implementation
public:
    virtual ~CStocksDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
    virtual COleServerItem* OnGetEmbeddedItem();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual BOOL OnNewDocument();
    virtual void OnFinalRelease();

// Generated message map functions
protected:
    //{{AFX_MSG(CStocksDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CStocksDoc)
    afx_msg LPDISPATCH GetItems();
    afx_msg void SetItems(LPDISPATCH newValue);
    afx_msg BOOL ShowWindow(BOOL fShow);
    afx_msg LPDISPATCH CreateItem(LPCSTR Name, const VARIANT FAR& Value);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // _STOCKDOC_H_
