// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// request.h : header file
//

#ifndef _REQUEST_H_
#define _REQUEST_H_

/////////////////////////////////////////////////////////////////////////////
// CRequest command target
class CDataObject ;
//class CRequestServerItem ;
class CProperties ;   
class CDataItem ;

class CRequest : public CCmdTarget
{   
    DECLARE_SERIAL(CRequest)
public:
    CRequest( CDataObject* pDataObject = NULL ) ;
    
// Attributes
public:
    CDataObject*    m_pDataObject ;
    
    CString         m_strRequest ;
    CProperties*    m_pProperties ;
    DWORD           m_dwID ;
    
    CObList         m_lstDataItems ;
    
    // the following support linking to Request items
//    CRequestServerItem* m_pServerItem;     // pointer to active item (may be NULL)

// Operations
public:                 
    BOOL Match( const CString& rstr );
    CDataItem* FindDataItem( const CString& rstr );
    void CalcDisplaySize( CDC* pDC, CSize& sizeItem ) ;  // get bounding rect
    int Draw( CDC* pDC, CPoint ptStart, int n = -1 ) ;

// Implementation
public:
    virtual ~CRequest();
    virtual void OnFinalRelease();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
    void SaveAsText(CArchive& ar);

    // Generated message map functions
    //{{AFX_MSG(CRequest)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CRequest)
    afx_msg BSTR GetRequest();
    afx_msg void SetRequest(LPCSTR lpszNewValue);
    afx_msg LPDISPATCH GetProperties();
    afx_msg void SetProperties(LPDISPATCH newValue);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()

    friend class CRequestServerItem;   // CXRTFrameServerItem is an extension of a CReqeust
};

/////////////////////////////////////////////////////////////////////////////
#endif // _REQUEST_H_

