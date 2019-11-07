// srvritem.h : interface of the CColSampSrvrItem class
//
// CColSampSrvItem definition  
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// Charlie Kindel, Program Manager
// Microsoft Vertical Developer Relations
//
// InterNet   :  ckindel@microsoft.com
// CompuServe :  >INTERNET:ckindel@microsoft.com
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//

#ifndef _SRVRITEM_H_
#define _SRVRITEM_H_

class CColSampSrvrItem : public COleServerItem
{
    DECLARE_DYNAMIC(CColSampSrvrItem)

// Constructors
public:
    CColSampSrvrItem(CColSampDoc* pContainerDoc);

// Attributes
    CColSampDoc* GetDocument() const
        { return (CColSampDoc*)COleServerItem::GetDocument(); }

// Implementation
public:
    ~CColSampSrvrItem();
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
#endif // _SRVRITEM_H_

