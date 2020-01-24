// items.h : header file
//

#ifndef _ITEMS_H_
#define _ITEMS_H_

/////////////////////////////////////////////////////////////////////////////
// CStockItems command target

class CStockItems : public CCmdTarget
{   
    DECLARE_DYNCREATE(CStockItems)
public:
    CStockItems();         // protected constructor used by dynamic creation

// Attributes
private:
    CObArray    m_rgItems ;
    
public:

// Operations
public:

// Implementation
public:
    virtual ~CStockItems();
    virtual void OnFinalRelease();
    int Find( const VARIANT FAR &vt ) ;

protected:
    // Generated message map functions
    //{{AFX_MSG(CStockItems)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CStockItems)
    afx_msg void Remove(const VARIANT FAR& Item);
    afx_msg VARIANT Add(const VARIANT FAR& ItemNameOrIndex);
	//}}AFX_DISPATCH
    afx_msg long GetCount();
    afx_msg LPUNKNOWN _NewEnum();
    afx_msg LPDISPATCH GetItem(const VARIANT FAR& ItemNameOrIndex);
    afx_msg void SetItem(const VARIANT FAR& ItemNameOrIndex, LPDISPATCH newValue);
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //_ITEMS_H_
