// item.h : header file
//

#ifndef _ITEM_H_
#define _ITEM_H_

/////////////////////////////////////////////////////////////////////////////
// CStockItem command target

class CStockItem : public CCmdTarget
{   
    DECLARE_DYNCREATE(CStockItem)
public:
    CStockItem();           // protected constructor used by dynamic creation

// Attributes  
private:
    VARIANT m_vtValue ;
    
public:

// Operations
public:

// Implementation
public:
    virtual ~CStockItem();
    virtual void OnFinalRelease();

protected:
    // Generated message map functions
    //{{AFX_MSG(CStockItem)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CStockItem)
	CString m_szName;
	afx_msg VARIANT GetValue();
	afx_msg void SetValue(const VARIANT FAR& newValue);
	//}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //_ITEM_H_


 