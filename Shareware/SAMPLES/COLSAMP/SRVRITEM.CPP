// srvritem.cpp : implementation of the CColSampSrvrItem class
//
// CColSampSrvrItem implementation  
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
// NOTE:
//      As it sits we don't support links, and we don't support
//      UDT either.   This module needs to be filled out to include
//      support for both.   The idea is to have each CItem in
//      the CColSampDoc be a linkable item....
//      

#include "stdafx.h"
#include "colsamp.h"

#include "doc.h"
#include "view.h"
#include "srvritem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColSampSrvrItem implementation

IMPLEMENT_DYNAMIC(CColSampSrvrItem, COleServerItem)
#define new DEBUG_NEW

CColSampSrvrItem::CColSampSrvrItem(CColSampDoc* pContainerDoc)
    : COleServerItem(pContainerDoc, TRUE)
{
    // BUGBUG:  Dataobject stuff not implemented yet.
    //
}

CColSampSrvrItem::~CColSampSrvrItem()
{
    // TODO: add cleanup code here
}

void CColSampSrvrItem::Serialize(CArchive& ar)
{
    // CColSampSrvrItem::Serialize will be called by the framework if
    //  the item is copied to the clipboard.  This can happen automatically
    //  through the OLE callback OnGetClipboardData.  A good default for
    //  the embedded item is simply to delegate to the document's Serialize
    //  function.  If you support links, then you will want to serialize
    //  just a portion of the document.

    TRACE("CColSampSrvrItem::Serialize\r\n" ) ;
    if (!IsLinkedItem())
    {
        TRACE("   Not linked\r\n" ) ;
        CColSampDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);
        pDoc->Serialize(ar);
    }
    else
    {
        TRACE("   Not linked\r\n" ) ;
    }
}

BOOL CColSampSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
    // Most applications, like this one, only handle drawing the content
    //  aspect of the item.  If you wish to support other aspects, such
    //  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
    //  implementation of OnGetExtent should be modified to handle the
    //  additional aspect(s).

    if (dwDrawAspect != DVASPECT_CONTENT)
        return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

    // CColSampSrvrItem::OnGetExtent is called to get the extent in
    //  HIMETRIC units of the entire item.  The default implementation
    //  here simply returns a hard-coded number of units.

    CColSampDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    rSize = CSize(0, 0);
    CClientDC dc(NULL);
    dc.SetMapMode(MM_ANISOTROPIC);

    pDoc->CalcDisplaySize(&dc, rSize);

    dc.LPtoHIMETRIC(&rSize); // convert pixels to HIMETRIC

    return TRUE;
}

BOOL CColSampSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
    CColSampDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // determine extent of this item in the document
    CPoint ptStart(0,0) ;
    rSize = CSize(0, 0);
    pDoc->CalcDisplaySize(pDC, rSize);

    // prepare to draw and remember the extent in himetric units
    pDC->SetWindowOrg(0, 0);
    pDC->SetWindowExt(rSize);
    pDC->SetViewportExt(rSize);  // Note: only affects the m_hAttribDC

    pDC->LPtoHIMETRIC(&rSize);   // convert pixels to HIMETRIC

    pDoc->Draw(pDC, ptStart) ;
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CColSampSrvrItem diagnostics

#ifdef _DEBUG
void CColSampSrvrItem::AssertValid() const
{
    COleServerItem::AssertValid();
}

void CColSampSrvrItem::Dump(CDumpContext& dc) const
{
    COleServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
