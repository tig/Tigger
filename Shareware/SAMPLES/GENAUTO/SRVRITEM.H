// srvritem.h : interface of the CGenAutoSrvrItem class
//
// CGenAutoSrvItem definition  
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

class CGenAutoSrvrItem : public COleServerItem
{
    DECLARE_DYNAMIC(CGenAutoSrvrItem)

// Constructors
public:
    CGenAutoSrvrItem(CGenAutoDoc* pContainerDoc);

// Attributes
    CGenAutoDoc* GetDocument() const
        { return (CGenAutoDoc*)COleServerItem::GetDocument(); }

// Implementation
public:
    ~CGenAutoSrvrItem();
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

