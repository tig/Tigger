// srvritem.h : interface of the CStocksSrvrItem class
//

class CStocksSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CStocksSrvrItem)

// Constructors
public:
	CStocksSrvrItem(CStocksDoc* pContainerDoc);

// Attributes
	CStocksDoc* GetDocument() const
		{ return (CStocksDoc*)COleServerItem::GetDocument(); }

// Implementation
public:
	~CStocksSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////
