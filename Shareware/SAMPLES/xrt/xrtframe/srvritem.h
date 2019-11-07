// WOSA/XRT XRTFrame Sample Application Version 1.00.007
//
// Copyright (c) 1993 Microsoft Corporation, All Rights Reserved.
//
// srvritem.h : interface of the CRequestsServerItem class
//
// CXRTFrameSrvItem definition  
//
// Revisions:
//  December 8, 1993   cek     First implementation.
//

#ifndef _SRVRITEM_H_
#define _SRVRITEM_H_

/////////////////////////////////////////////////////////////////////////////

class CDataObjectSvrItem : public COleServerItem
{
    DECLARE_DYNAMIC(CDataObjectSvrItem)

// Constructors
public:
    CDataObjectSvrItem(CDataObject* pContainerDoc);

// Attributes
    CDataObject* GetDocument() const
        { return (CDataObject*)COleServerItem::GetDocument(); }

    BOOL m_fInSendOnDataChange ;

// Operations
public:        

// Implementation
public:
    ~CDataObjectSvrItem();

#if (_MFC_VER <= 0x250)
    BEGIN_INTERFACE_PART(BugFixDataObject, IDataObject)
        STDMETHOD(GetData)(LPFORMATETC, LPSTGMEDIUM);
        STDMETHOD(GetDataHere)(LPFORMATETC, LPSTGMEDIUM);
        STDMETHOD(QueryGetData)(LPFORMATETC);
        STDMETHOD(GetCanonicalFormatEtc)(LPFORMATETC, LPFORMATETC);
        STDMETHOD(SetData)(LPFORMATETC, LPSTGMEDIUM, BOOL);
        STDMETHOD(EnumFormatEtc)(DWORD, LPENUMFORMATETC FAR*);
        STDMETHOD(DAdvise)(LPFORMATETC, DWORD, LPADVISESINK, LPDWORD);
        STDMETHOD(DUnadvise)(DWORD);
        STDMETHOD(EnumDAdvise)(LPENUMSTATDATA FAR*);
    END_INTERFACE_PART(BugFixDataObject)
    
    DECLARE_INTERFACE_MAP()
#endif // BUGFIX DADVISE

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
    virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
    virtual BOOL OnRenderFileData( LPFORMATETC lpFormatEtc, CFile* pFile);
    virtual BOOL OnRenderGlobalData( LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal ) ;
    //virtual COleDataSource* OnGetClipboardData(BOOL bIncludeLink, LPPOINT pptOffset, LPSIZE pSize);
    //virtual void GetNativeClipboardData(COleDataSource *pDataSource);

protected:
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

#endif // _SRVRITEM_H_

