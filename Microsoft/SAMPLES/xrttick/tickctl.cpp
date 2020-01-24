// WOSA/XRT XRTTick Sample OLE Control Version 1.00.001
//
// Copyright (c) 1993-94 Microsoft Corporation, All Rights Reserved.
//
//      This is a part of the Microsoft Source Code Samples. 
//      This source code is only intended as a supplement to 
//      Microsoft Development Tools and/or WinHelp documentation.
//      See these sources for detailed information regarding the 
//      Microsoft samples programs.
//
// tickctl.cpp : Implementation of the CTickCtrl OLE control class.
//
// Revisions:
//  September, 1994   cek       Regenerated with ControlWizard and
//                              added WOSA/XRT compliance
//

#include "stdafx.h"
#include "xrttick.h"
#include "tickctl.h"
#include "xrttippg.h"
#include "offstage.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Numeric constants
const short ID_TIMER = 1;
const short MSEC_CLOCK = 50 ;
const short TICK_WIDTH = 4 ;
const short QUEUE_SIZE = 25 ;
const TCHAR NAMEVALUETOKENS[] = _T("\t") ;
const TCHAR TICKTOKENS[] = _T("\r\n") ;

IMPLEMENT_DYNCREATE(CTickCtrl, COleControl)
#define new DEBUG_NEW 
/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CTickCtrl, COleControl)
    //{{AFX_MSG_MAP(CTickCtrl)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
    ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
    ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CTickCtrl, COleControl)
    //{{AFX_DISPATCH_MAP(CTickCtrl)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "ScrollSpeed", m_nScrollSpeed, OnScrollSpeedChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "ScrollWidth", m_nScrollWidth, OnScrollWidthChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "MaxCacheSize", m_nMaxCacheSize, OnMaxCacheSizeChanged, VT_I4)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "DataObjectName", m_strDataObjectName, OnDataObjectNameChanged, VT_BSTR)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "DataObjectActive", m_fActive, OnDataObjectActiveChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "DataObjectVisible", m_fDataObjectVisible, OnDataObjectVisibleChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "AutoStartDataObject", m_fAutoStartDataObject, OnAutoStartDataObjectChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "CacheOn", m_fCacheOn, OnCacheOnChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "OffsetValues", m_fOffsetValues, OnOffsetValuesChanged, VT_BOOL)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "DataObjectNameProperty", m_strNameProperty, OnDataObjectNamePropertyChanged, VT_BSTR)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "DataObjectValueProperty", m_strValueProperty, OnDataObjectValuePropertyChanged, VT_BSTR)
    DISP_PROPERTY_NOTIFY(CTickCtrl, "DataObjectRequest", m_strRequest, OnDataObjectRequestChanged, VT_BSTR)
    DISP_PROPERTY_EX(CTickCtrl, "DataObject", GetDataObject, SetDataObject, VT_DISPATCH)
    DISP_FUNCTION(CTickCtrl, "Clear", Clear, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION(CTickCtrl, "StartDataObject", StartDataObject, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CTickCtrl, "StopDataObject", StopDataObject, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CTickCtrl, "InitializeDataObject", InitializeDataObject, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CTickCtrl, "Tick", Tick, VT_EMPTY, VTS_I4 VTS_VARIANT VTS_VARIANT)
    DISP_FUNCTION(CTickCtrl, "DataObjectAdvise", DataObjectAdvise, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CTickCtrl, "DataObjectUnAdvise", DataObjectUnAdvise, VT_BOOL, VTS_NONE)
    DISP_STOCKPROP_ENABLED()
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_FORECOLOR()
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_TEXT()
	//}}AFX_DISPATCH_MAP
    DISP_FUNCTION_ID(CTickCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CTickCtrl, COleControl)
    //{{AFX_EVENT_MAP(CTickCtrl)
    EVENT_CUSTOM("CacheOverflow", FireCacheOverflow, VTS_NONE)
    //}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages
BEGIN_PROPPAGEIDS(CTickCtrl, 5)
    PROPPAGEID(CTickPropPage::guid)
    PROPPAGEID(CDataObjectPropPage::guid)
    PROPPAGEID(CXRTPropPage::guid)
    PROPPAGEID(CLSID_CColorPropPage)
    PROPPAGEID(CLSID_CFontPropPage)
END_PROPPAGEIDS(CTickCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CTickCtrl, "XRTTick.Control.1",
    0x5aabe530, 0xdaf0, 0x11cd, 0x84, 0x45, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CTickCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DXRTTicker =
        { 0x5aabe531, 0xdaf0, 0x11cd, { 0x84, 0x45, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10 } };
const IID BASED_CODE IID_DXRTTickerEvents =
        { 0x5aabe532, 0xdaf0, 0x11cd, { 0x84, 0x45, 0x0, 0xaa, 0x0, 0x33, 0x97, 0x10 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwXRTTickerOleMisc =
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CTickCtrl, IDS_XRTTICKER, _dwXRTTickerOleMisc)


static UINT  g_CF_XRT ; // clipboard format

/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::CTickCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CTickCtrl

BOOL CTickCtrl::CTickCtrlFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_XRTTICKER,
            IDB_XRTTICKER,
            TRUE,                       //  Insertable
            _dwXRTTickerOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    else
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::CTickCtrl - Constructor

CTickCtrl::CTickCtrl()
{
    InitializeIIDs(&IID_DXRTTicker, &IID_DXRTTickerEvents);

    SetInitialSize(300, 25);

    g_CF_XRT = (CLIPFORMAT)RegisterClipboardFormat(szCF_WOSAXRT101A);
    m_dwAdvise = 0 ;
    m_fWOSAXRT = FALSE;
    m_pOnstage = NULL ;
    m_pDataObject = NULL ;

    // BUGFIX:  The MFC docs say that COleControl::SetInitialDataFormats
    // is virutal, but it is in fact not.  We want to register
    // CF_TEXT as our first data format, so you'd think we could just over
    // ride ::SetInitialDataFormats (like I did below).  However, since
    // it's not really virtual, it'll never be called.  Thus we hack
    // by emptying the datasource object here (COleControl's constructor
    // has already called SetInitialDataFormats) and then call our own
    // SetIntialDataFormats().
    //
    m_dataSource.Empty();
    SetInitialDataFormats();

    // Must call OnResetState here because if we are
    // ever used by a standard OLE container it is possible
    // that OnResetState won't be called.
    //
    OnResetState() ;

}

void CTickCtrl::SetInitialDataFormats()
{
    // We support CF_TEXT on HGLOBAL, in addition to the onese
    // m_dataSource supports by default.   We also need to 
    // support these on our separate DragDrop source object.
    //
    m_dataSource.DelayRenderData(CF_TEXT);
    m_dataSource.DelaySetData(CF_TEXT);
    m_datasourceDropSource.DelayRenderData(CF_TEXT);
    m_datasourceDropSource.DelaySetData(CF_TEXT);

    // Now let the standard ones be setup.
    COleControl::SetInitialDataFormats();
}

/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::~CTickCtrl - Destructor

CTickCtrl::~CTickCtrl()
{
    // make sure DUnadvise is called
    StopDataObject();

    if (m_pOnstage != NULL)
    {
        delete m_pOnstage ;
        m_pOnstage = NULL ;
    }
    // Clear out both queues
    //
    while (m_qNew.IsEmpty() == FALSE)
        delete (CTick*)m_qNew.RemoveHead() ;
    while (m_qCache.IsEmpty() == FALSE)
        delete (CTick*)m_qCache.RemoveHead() ;

}

void CTickCtrl::OnDrawMetafile(CDC* pdc, const CRect& rcBounds)
{
    ASSERT_VALID(pdc) ;
    ASSERT_VALID(&m_bmDisplay) ;

    pdc->StretchBlt( rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom,
                 &m_dcDisplay, 0, 0, m_sizeDisplay.cx, m_sizeDisplay.cy, SRCCOPY ) ;
}

/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::OnDraw - Drawing function

void CTickCtrl::OnDraw(
            CDC* pdc, const CRect& rcBounds, const CRect& /*rcInvalid*/)
{
    ASSERT_VALID(pdc) ;
    ASSERT_VALID(&m_bmDisplay) ;

    pdc->BitBlt( rcBounds.left, rcBounds.top, m_sizeDisplay.cx, m_sizeDisplay.cy, 
                 &m_dcDisplay, 0, 0, SRCCOPY ) ;
}

//  Function:   CreateDisplayBitmap
//
//  Synopsis:   Creates the bitmap that is the current 'display' bitmap.  That is,
//              m_bmDisplay always contains the image that would be painted to the
//              client area on a WM_PAINT
//
void CTickCtrl::CreateDisplayBitmap( CDC* pdcIn /*= NULL*/)
{
    CRect       rc ;
    CDC*        pdc ;

    //if (pdcIn == NULL && GetSafeHwnd() == NULL)
    //    return ;

    if (m_bmDisplay.m_hObject)
    {
        m_dcDisplay.DeleteDC() ;
        m_bmDisplay.DeleteObject() ;
    }

    if (pdcIn == NULL)
        pdc = CDC::FromHandle(::GetDC(GetSafeHwnd())) ;
    else
        pdc = pdcIn ;

    ASSERT_VALID(pdc) ;    

    // Create a bitmap at least as large as the display, but not larger
    // than the max.
    //
    m_dcDisplay.CreateCompatibleDC( pdc ) ;

    if (GetSafeHwnd() == NULL)
    {
        rc.left = rc.top = 0 ;
        GetControlSize( (int*)&rc.right, (int*)&rc.bottom );
    }
    else
        GetClientRect(&rc) ;   

    m_sizeDisplay.cx = rc.Width() ;
    m_sizeDisplay.cy = rc.Height() ;
    m_bmDisplay.CreateCompatibleBitmap( pdc, rc.Width(), rc.Height() ) ;

    // Draw the text into the bitmap
    //
    m_dcDisplay.SelectObject( &m_bmDisplay ) ;
    
    m_dcDisplay.SetBkColor( TranslateColor( GetBackColor() ) ) ;
    m_dcDisplay.ExtTextOut( 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL ) ;

    // Redraw onstage
    if (m_pOnstage)
        m_pOnstage->CreateImage( pdc ) ;

    // redraw cached images.
    POSITION pos = m_qCache.GetHeadPosition() ;
    while (pos)
    {
        CTick* pTick = (CTick*)m_qCache.GetNext(pos) ;
        ASSERT(pTick) ;
        pTick->CreateImage(pdc) ;
    }

    if (pdcIn == NULL)
        ::ReleaseDC(GetSafeHwnd(), pdc->m_hDC);

    MoveTicker( m_sizeDisplay.cx - (m_sizeDisplay.cx / 8) ) ;
                    
    return ;    
}

//  Function:   MoveTicker
//
//  Synopsis:   Draws a portion of the offstage bitmap into the rhs of the
//              display bitmap, scrolling the display bitmap to the left
//
//  Arguments:  
//
void CTickCtrl::MoveTicker( long nMove )
{
    int         cxSrc, xDest ;
    CRect       rc ;

    if (m_pOnstage == NULL && !m_qNew.IsEmpty())
        m_pOnstage = (CTick*)m_qNew.RemoveHead() ;

    if (m_pOnstage == NULL && !m_qCache.IsEmpty())
        m_pOnstage = (CTick*)m_qCache.RemoveHead() ;
    
    // if there is no offstage bitmap ready, don't do anything
    // 
    if (m_pOnstage == NULL)
        return ;

    CDC* pdc = CDC::FromHandle(::GetDC(GetSafeHwnd())) ;
    ASSERT_VALID(pdc) ;

    // Make sure bitmap has been prepared
    //
    if (m_pOnstage->IsClear())
        m_pOnstage->CreateImage(pdc) ;

    // Scroll display bitmap to left
    //
    m_dcDisplay.BitBlt( 0, 0, m_sizeDisplay.cx - (UINT)nMove, m_sizeDisplay.cy,
                   &m_dcDisplay, (UINT)nMove, 0, SRCCOPY ) ;

    cxSrc = min( (int)nMove, m_pOnstage->m_size.cx - m_pOnstage->m_x ) ;
    xDest = m_sizeDisplay.cx - (UINT)nMove ;

lblTick:
    m_dcDisplay.BitBlt( xDest, 0, cxSrc, m_sizeDisplay.cy,
                   &m_pOnstage->m_dc, m_pOnstage->m_x, 0, SRCCOPY ) ;    
    
    m_pOnstage->m_x += cxSrc ;

    if (m_pOnstage->m_x >= m_pOnstage->m_size.cx)
    {
        if (m_pOnstage->m_bDelete)
        {
            delete m_pOnstage ;
            m_pOnstage = NULL ;
        }
        else
        {
            if (m_qCache.GetCount() >= m_nMaxCacheSize)
            {
                delete (CTick*)m_qCache.RemoveTail() ;
                FireCacheOverflow() ;
            }

            if (m_fCacheOn)
            {
                // Put him at end of cache queue
                m_pOnstage->m_x = 0 ;
                m_pOnstage->m_bCache = TRUE ;
                m_pOnstage->EraseImage() ;
                m_qCache.AddTail( m_pOnstage ) ;
                m_pOnstage = NULL ;
            }
            else
            {
                delete m_pOnstage ;
                m_pOnstage = NULL ;
            }
        }

        if (m_pOnstage == NULL && !m_qNew.IsEmpty())
            m_pOnstage = (CTick*)m_qNew.RemoveHead() ;

        if (m_pOnstage == NULL && !m_qCache.IsEmpty())
            m_pOnstage = (CTick*)m_qCache.RemoveHead() ;

        if (m_pOnstage != NULL)
        {
            // Make sure bitmap has been prepared
            //
            if (m_pOnstage->IsClear())
                m_pOnstage->CreateImage(pdc) ;

            if (cxSrc > 0 && xDest + cxSrc < m_sizeDisplay.cx)
            {
                // We still need more, so use the next offstage bitmap or
                // just erase.
                //
                xDest += cxSrc ;
                cxSrc = min( m_sizeDisplay.cx - xDest, m_pOnstage->m_size.cx - m_pOnstage->m_x ) ;
                goto lblTick ;
            }
        }
    }

    ::ReleaseDC(GetSafeHwnd(), pdc->m_hDC);
}

#ifdef _WIN32
void CTickCtrl::OnSetClientSite()
{
    // BUGFIX CDK Bug
    // 
    // The CDK's implementation of IOleObject::SetClientSite QI's for
    // IDispatch on IOleClientSite and assumes tha the IDispatch returned
    // (if returned) is the IDispatch for the ambient properties.  This is
    // an invalid assumption.  If the container is not a control container
    // and happens to implement an IDispatch on it's client site (like
    // Excel) the CDK blindly does Invokes with negative dispids.  This in
    // itself should not be a big bug, however, Excel32 blows up if it's
    // IDispatch::Invoke is called with invalid dispids.
    //
    // 
    //
    if (m_ambientDispDriver.m_lpDispatch != NULL)
    {
        // HACK!  Is this Excel (or some other non-control container)? 
        // If so, it doesn't support ambients.
        //
        // Note Access 2.0 does not support IOleControlSite, but
        // this bug only impacts XL32 and 32 bit controls don't work
        // in 16 bit containers...  This is why this hack is
        // bracked with #ifdef _WIN32
        //
        HRESULT hr ;
        LPOLECONTROLSITE lpsite = NULL ;
        hr = m_ambientDispDriver.m_lpDispatch->QueryInterface( IID_IOleControlSite, (LPVOID*)&lpsite ) ;
        if (SUCCEEDED(hr) && lpsite != NULL)
        {
            lpsite->Release() ;
        }
        else
        {
            TRACE("HACK: Ignoring ambient properties to avoid Excel32 bug.\n") ;
            m_ambientDispDriver.ReleaseDispatch() ;
        }
    }

    COleControl::OnSetClientSite() ;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::DoPropExchange - Persistence support
void CTickCtrl::DoPropExchange(CPropExchange* pPX)
{
    ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange(pPX);

    PX_Long( pPX, _T("ScrollSpeed"), m_nScrollSpeed,  MSEC_CLOCK ) ;
    PX_Long( pPX, _T("ScrollWidth"), m_nScrollWidth, TICK_WIDTH ) ;
    PX_Long( pPX, _T("MaxCacheSize"), m_nMaxCacheSize, QUEUE_SIZE) ;
    #ifdef _DEBUG
    PX_String( pPX, _T("DataObjectName"), m_strDataObjectName, _T("XRTFrame.DataObject.1") ) ;
    #else
    PX_String( pPX, _T("DataObjectName"), m_strDataObjectName, _T("") ) ;
    #endif

    PX_String( pPX, _T("DataObjectRequest"), m_strRequest, _T("*") ) ;
    PX_String( pPX, _T("DataObjectNameProperty"), m_strNameProperty, _T("Name") ) ;
    PX_String( pPX, _T("DataObjectValueProperty"), m_strValueProperty, _T("Last") ) ;

    PX_Bool( pPX, _T("DataObjectActive"), m_fActive, TRUE) ;
    PX_Bool( pPX, _T("DataObjectVisible"), m_fDataObjectVisible, FALSE ) ;
    PX_Bool( pPX, _T("AutoStartDataObject"), m_fAutoStartDataObject, FALSE ) ;
    PX_Bool( pPX, _T("CacheOn"),  m_fCacheOn, TRUE ) ;
	PX_Bool( pPX, _T("OffsetValues"), m_fOffsetValues, TRUE ) ;
}


/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::OnResetState - Reset control to default state

void CTickCtrl::OnResetState()
{
    StopDataObject() ;
    m_fFeedDown = FALSE ;


    COleControl::OnResetState();  // Resets defaults found in DoPropExchange

    #ifdef _DEBUG
    SetText( _T("XRTTick\tDebug Build\r\nCompiled\t") + CString(__DATE__) + " " + CString(__TIME__) ) ;
    #else
    SetText( _T("WOSA/XRT Ticker Control") ) ;
    #endif
    SetForeColor( (OLE_COLOR)0x80000000 + COLOR_BTNTEXT ) ;  // Same as GetSysColor(COLOR_HIGHLIGHTTEXT)
    SetBackColor( (OLE_COLOR)0x80000000 + COLOR_BTNFACE ) ;

}


/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::AboutBox - Display an "About" box to the user

void CTickCtrl::AboutBox()
{
    CDialog dlgAbout(IDD_ABOUTBOX_XRTTICKER);
    dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CTickCtrl message handlers

void CTickCtrl::OnScrollSpeedChanged() 
{
    if (GetEnabled() && AmbientUserMode()) 
    {
        StopTimer();
        StartTimer();
    }
    SetModifiedFlag();
}

void CTickCtrl::OnScrollWidthChanged() 
{
    SetModifiedFlag();
}

void CTickCtrl::OnMaxCacheSizeChanged() 
{
    if (m_nMaxCacheSize < 1)
        m_nMaxCacheSize = 1 ;

    SetModifiedFlag();
}

int CTickCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (COleControl::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    // Start timer if in user mode and if enabled
    if (AmbientUserMode() && GetEnabled()) 
        StartTimer();

    CreateDisplayBitmap() ;

    m_dropTarget.Register(this);
    DragAcceptFiles();
    return 0;
}

void CTickCtrl::OnDestroy() 
{
    DragAcceptFiles(FALSE);
    m_dropTarget.Revoke();

    if (m_bmDisplay.m_hObject)
    {
        m_dcDisplay.DeleteDC() ;
        m_dcDisplay.m_hDC = NULL ;
        m_dcDisplay.m_hAttribDC = NULL ;
        m_bmDisplay.DeleteObject() ;
    }

    if (m_pOnstage != NULL)
    {
        delete m_pOnstage ;
        m_pOnstage = NULL ;
    }

    while (m_qNew.IsEmpty() == FALSE)
        delete (CTick*)m_qNew.RemoveHead() ;
    while (m_qCache.IsEmpty() == FALSE)
        delete (CTick*)m_qCache.RemoveHead() ;

    StopTimer();

    //StopDataObject() ;
    COleControl::OnDestroy();
}

// Setting the text property causes the que to be cleared
// and CTick objects to be added with the new text.
// tab characters delimit separate objects.
//
void CTickCtrl::OnTextChanged()
{
    // Setup the current image to be nuked as soon as it's
    // done.
    //
    if (m_pOnstage != NULL)
        m_pOnstage->m_bDelete = TRUE ;

    // Clear out both queues
    //
    while (m_qNew.IsEmpty() == FALSE)
        delete (CTick*)m_qNew.RemoveHead() ;
    while (m_qCache.IsEmpty() == FALSE)
        delete (CTick*)m_qCache.RemoveHead() ;

    m_fFeedDown = FALSE ;

    // Pull apart the text string into separate ticks.  
    // Each tick is separated by a CRLF pair
    //
    DWORD   dw = 0 ;
    LPTSTR psz = new TCHAR[InternalGetText().GetLength()+1] ;
    _tcscpy( psz, InternalGetText() ) ;
    LPTSTR p = _tcstok( psz, TICKTOKENS ) ;
    while (p && (m_qNew.GetCount() < m_nMaxCacheSize))
    {
        CTick* pTick = new CTick(this) ;
        pTick->m_dwDataItemID = dw++ ;
        pTick->m_strName = p ;
        m_qNew.AddTail( pTick ) ;
        p = _tcstok( NULL, TICKTOKENS ) ;
    }
    delete [] psz ;

    // Now go through each tick and pull apart the
    // m_strName member.  The name, value, and subsequent
    // values are separated by TAB characters
    // 
    POSITION pos = m_qNew.GetHeadPosition() ;
    while (pos)
    {
        CTick* pTick = (CTick*)m_qNew.GetNext( pos ) ;

        psz = new TCHAR[pTick->m_strName.GetLength()+1] ;
        _tcscpy( psz, pTick->m_strName ) ;
        p = _tcstok( psz, NAMEVALUETOKENS ) ;
        if (p)     
            pTick->m_strName = p ;
        p = _tcstok( NULL, NAMEVALUETOKENS ) ;
        while (p)
        {
            pTick->m_strValue += p ;
            p = _tcstok( NULL, NAMEVALUETOKENS ) ;
            if (p)
                pTick->m_strValue += " " ;
        }
        delete [] psz ;
    }

    if (m_qNew.GetCount() >= m_nMaxCacheSize)
        FireCacheOverflow() ;

    COleControl::OnTextChanged() ;
}

void CTickCtrl::Clear() 
{
    if (m_pOnstage != NULL)
    {
        delete m_pOnstage ;
        m_pOnstage = NULL ;
    }
    // Clear out both queues
    //
    while (m_qNew.IsEmpty() == FALSE)
        delete (CTick*)m_qNew.RemoveHead() ;
    while (m_qCache.IsEmpty() == FALSE)
        delete (CTick*)m_qCache.RemoveHead() ;

    m_fFeedDown = FALSE ;

    CreateDisplayBitmap() ;
}

void CTickCtrl::OnOffsetValuesChanged() 
{
    CreateDisplayBitmap() ;
   
    SetModifiedFlag();
}

void CTickCtrl::OnTimer(UINT nIDEvent) 
{
    if (!(IsWindowVisible() && GetEnabled() && AmbientUserMode()))
        return ; 

    MoveTicker( m_nScrollWidth ) ;

    if (GetSafeHwnd())
    {
        CDC* pdc = GetDC() ;
        CRect rc ;
        GetClientRect(&rc) ;
        OnDraw( pdc, rc, rc ) ;
        ReleaseDC(pdc) ;
    }
        
    COleControl::OnTimer(nIDEvent);
}

void CTickCtrl::OnSize(UINT nType, int cx, int cy) 
{
    if (m_sizeDisplay.cx != cx || m_sizeDisplay.cy != cy)
        CreateDisplayBitmap() ;
    COleControl::OnSize(nType, cx, cy);

}

BOOL CTickCtrl::OnSetExtent(LPSIZEL lpSizeL)
{
	return COleControl::OnSetExtent(lpSizeL);
}

void CTickCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    nFlags ;
    point ;

    CRect rc ;
    GetClientRect(&rc) ;
    OnProperties( 0, this->GetSafeHwnd(), &rc ) ;
}


/////////////////////////////////////////////////////////////////////////////
// ::OnEnabledChanged - Start/stop the timer when the enable state
// has changed.
//
void CTickCtrl::OnEnabledChanged() 
{
    if (AmbientUserMode())
    {
        if (GetEnabled())
        {
            if (GetHwnd() != NULL) 
                StartTimer();
        }        
        else
            StopTimer();
    }        
}

/////////////////////////////////////////////////////////////////////////////
// CTickCtrl::OnAmbientPropertyChange - Start the timer if user mode ambient 
// property has changed and if timer is enabled.  Stop the timer if user
// mode ambient property has changed and if timer is enabled.
//
void CTickCtrl::OnAmbientPropertyChange(DISPID dispid)
{
    if (dispid == DISPID_AMBIENT_USERMODE)
    {
        // Start or stop the timer 
        if (GetEnabled())
        {
            if (AmbientUserMode()) 
                StartTimer();
            else
                StopTimer();
        }
                  
        InvalidateControl();
    }
}

void CTickCtrl::OnFontChanged()
{
    CreateDisplayBitmap() ;
    COleControl::OnFontChanged() ;
}

void CTickCtrl::OnForeColorChanged()
{
    CreateDisplayBitmap() ;
    COleControl::OnForeColorChanged() ;
}

void CTickCtrl::OnBackColorChanged()
{
    CreateDisplayBitmap() ;
    COleControl::OnBackColorChanged() ;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeCtrl::StartTimer - Start the timer.

void CTickCtrl::StartTimer()
{
    SetTimer(ID_TIMER, (int)m_nScrollSpeed, NULL);
}

void CTickCtrl::StopTimer()
{
    KillTimer(ID_TIMER);
}


/////////////////////////////////////////////////////////////////////////////
// DataObject related members
//
void CTickCtrl::OnDataObjectNameChanged() 
{
    // stop DataObject if there's one already
    StopDataObject();   

    if (m_fAutoStartDataObject == TRUE && m_pDataObject == NULL && !m_strDataObjectName.IsEmpty())
        StartDataObject() ;

    SetModifiedFlag();
}

// GetDataObject returns the IDispatch pointer of the
// DataObject object
//
LPDISPATCH CTickCtrl::GetDataObject() 
{
    IDispatch* pdisp = NULL ;
    if (m_pDataObject)
        m_pDataObject->QueryInterface(IID_IDispatch, (void**)&pdisp) ;
        
    return pdisp ;
}

// GetDataObject sets the IDispatch pointer of the
// DataObject object
//
//
void CTickCtrl::SetDataObject(LPDISPATCH newValue) 
{
    if (m_pDataObject)
        StopDataObject();

    if (newValue != NULL)
    {
        HRESULT hr = newValue->QueryInterface(IID_IDataObject, (void**)&m_pDataObject) ;
        if (FAILED(hr))
            AfxThrowOleDispatchException(1, _T("Object does not support Uniform Data Transfer.") ) ;
    }
    else
        AfxThrowOleDispatchException(1, _T("Invalid property value.") ) ;
}


void CTickCtrl::OnDataObjectNamePropertyChanged() 
{
    SetModifiedFlag();
}

void CTickCtrl::OnDataObjectValuePropertyChanged() 
{
    SetModifiedFlag();
}

void CTickCtrl::OnDataObjectRequestChanged() 
{
    SetModifiedFlag();
}

BOOL CTickCtrl::StartDataObject() 
{
    USES_CONVERSION ;

    //  If server is running already, return FAILURE
    //
    HRESULT hr = ResultFromScode(E_FAIL) ;
    if (m_pDataObject != NULL)
        return FALSE ;

    //  The dataobjectname can be a progid, a filename, or a 
    //  displayname for a moniker.  MkParseDisplayName can
    //  handle both the filename & moniker cases, but we have
    //  to treat the progid specially.
    //
    CLSID clsid ;

    // Assume it's a progID first.
    hr = CLSIDFromProgID(T2OLE(m_strDataObjectName), &clsid);
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(clsid, NULL,              // create an instance of the server
                                CLSCTX_SERVER, 
                                IID_IDataObject, 
                                (LPVOID *) &m_pDataObject);
        if (FAILED(hr))
        {
            // We got a valid CLSID, so there's no point in 
            // trying to see if it's a display name for a moniker, so
            // fail now.
            AfxThrowOleDispatchException(0, "Could not create instance.");
        }
    }

    // If that failed, then assume it's a moniker display name
    if (FAILED(hr))
    {
        LPBINDCTX   pbc ;
        CreateBindCtx(0,&pbc) ;
        ULONG ulEaten ;
        IMoniker* pmk = NULL ;
        hr = MkParseDisplayName(pbc, T2OLE(m_strDataObjectName), &ulEaten, &pmk);
        if (SUCCEEDED(hr))
        {
            hr = pmk->BindToObject(pbc, NULL, IID_IDataObject,(void**)&m_pDataObject);
            pmk->Release() ;
        }
        pbc->Release() ;
    }

    // If that failed, we're out of luck.
    if (FAILED(hr))
        AfxThrowOleDispatchException(0, "Could not bind to object.");

    // Determine if it's a WOSA/XRT DataObject
    //
    m_fWOSAXRT = FALSE ;
    COleDispatchDriver drvDataObject ;
    DISPID dispid ;
    drvDataObject.m_bAutoRelease = TRUE ;
    hr = m_pDataObject->QueryInterface(IID_IDispatch, (void**)&drvDataObject.m_lpDispatch);
    if (SUCCEEDED(hr))
    {
        LPWSTR pszDispMethod = L"Requests";                                     
        hr = drvDataObject.m_lpDispatch->GetIDsOfNames(IID_NULL,
                        (LPWSTR FAR*)&pszDispMethod,
                        1, 0,  &dispid);   // get the requests object id                    
        if (SUCCEEDED(hr))
            m_fWOSAXRT = TRUE ;
    }

    if (m_fAutoStartDataObject == TRUE)
        return InitializeDataObject() ;

    return TRUE ;
}


BOOL CTickCtrl::StopDataObject() 
{
    USES_CONVERSION ;
    if (m_pDataObject == NULL)
        return FALSE ;

    COleDispatchDriver drvDataObject ;
    HRESULT hrDisp = E_FAIL;
    DISPID dispid ;
    LPWSTR pszDispMethod = NULL;

    CString strErrorMsg ;
    HRESULT hr ;
    if (m_fWOSAXRT)
    {
        drvDataObject.m_bAutoRelease = TRUE ;
        hrDisp= m_pDataObject->QueryInterface(IID_IDispatch, (void**)&drvDataObject.m_lpDispatch);

        // Turn off Active
        //
        if (SUCCEEDED(hrDisp))
        {
            pszDispMethod = L"Active" ;
            hr = drvDataObject.m_lpDispatch->GetIDsOfNames(IID_NULL,
                            (LPWSTR FAR*)&pszDispMethod,
                            1, 0,  &dispid );
            if (SUCCEEDED(hr))
                drvDataObject.SetProperty(dispid, VT_BOOL, FALSE ); 
        }
    }

    // Kill the advise sink
    //
    KillAdviseSink();

    // Then turn off visible
    //
    if (m_fWOSAXRT && SUCCEEDED(hrDisp))
    {
        pszDispMethod = L"Visible" ;
        hr = drvDataObject.m_lpDispatch->GetIDsOfNames(IID_NULL,
                        (LPWSTR FAR*)&pszDispMethod,
                        1, 0,  &dispid );
        if (SUCCEEDED(hr))
            drvDataObject.SetProperty(dispid, VT_BOOL, FALSE ); 
    }
    m_pDataObject->Release() ;
    m_pDataObject = NULL ;

    return TRUE;
}


// InitializeDataObject makes the XRT requests etc... 
//
BOOL CTickCtrl::InitializeDataObject() 
{
    USES_CONVERSION ;
    if (m_pDataObject == NULL)
        return FALSE ;

    SyncDataObjectVisibleState() ;

    COleDispatchDriver  drvRequests ;
    COleDispatchDriver  drvRequest ;
    LPWSTR              pszDispMethod = NULL ;
    DISPID              dispid ;
    VARIANT             vTemp ;
    VARIANT             vtp1, vtp2;
    HRESULT             hr = ResultFromScode(E_FAIL) ;

    COleDispatchDriver drvDataObject ;
    HRESULT hrDisp = E_FAIL;

    CString strErrorMsg ;
    try
    {
        if (m_fWOSAXRT)
        {
            drvDataObject.m_bAutoRelease = TRUE ;
            hrDisp = m_pDataObject->QueryInterface(IID_IDispatch, (void**)&drvDataObject.m_lpDispatch);
            if (SUCCEEDED(hrDisp))
            {
                VariantInit(&vTemp);
                VariantInit(&vtp1);
                VariantInit(&vtp2);

                // Attach to object's Requests object
                // (Set requests = DataObject.Requests)
                //
                pszDispMethod = L"Requests";                                     
                hr = drvDataObject.m_lpDispatch->GetIDsOfNames(IID_NULL,
                                    (LPWSTR FAR*)&pszDispMethod,
                                    1, 0,  &dispid);   // get the requests object id                    
                if (SUCCEEDED(hr))
                {
                    strErrorMsg = "DataObject does not appear to support WOSA/XRT, cound not access DataObject.Requests." ;
                    LPDISPATCH pdispRequests = NULL;                            
                    drvDataObject.InvokeHelper( dispid, 
                                                DISPATCH_PROPERTYGET,
                                                VT_DISPATCH,        // return type
                                                (LPDISPATCH *)&pdispRequests, 
                                                NULL ) ;

                    ASSERT(pdispRequests) ;
                    drvRequests.AttachDispatch(pdispRequests, TRUE);  // let drvRequests do the Release()
                    pdispRequests = NULL ; // passed to drvRequests

                    // Do "Set p = Requests.CreateProperties("Name", "Last")
                    //
                    pszDispMethod = L"CreateProperties";
                    hr = drvRequests.m_lpDispatch->GetIDsOfNames(IID_NULL,
                                        (LPWSTR FAR*)&pszDispMethod,
                                        1, 0,  &dispid);   // get the requests object id                    
                    if (FAILED(hr))
                    {
                        strErrorMsg = "DataObject does not appear to support WOSA/XRT, the \"" + 
                                    CString(pszDispMethod) + "\" method was not found." ;
                        AfxThrowOleException(hr) ;
                    }

                    vtp1.vt = VT_BSTR;
                    vtp1.bstrVal = m_strNameProperty.AllocSysString();
                    vtp2.vt = VT_BSTR;
                    vtp2.bstrVal = m_strValueProperty.AllocSysString();

                    strErrorMsg = "DataObject.CreateProperties(\"" + m_strNameProperty + "\", \"" + m_strValueProperty + "\") failed." ;
                    LPDISPATCH pdispProps = NULL ; // Request.Add reutrns VT_DISPATCH
                    // InvokeHelper uses this to describe method params.
                    static BYTE BASED_CODE propparms[] = VTS_VARIANT VTS_VARIANT;
                    drvRequests.InvokeHelper(   dispid, 
                                                DISPATCH_METHOD,
                                                VT_DISPATCH,        // return type
                                                (LPDISPATCH *)&pdispProps, 
                                                propparms,
                                                (VARIANT *)&vtp1,   // "Name"
                                                (VARIANT *)&vtp2 ); // "Last"
                    VariantClear(&vtp1);
                    VariantClear(&vtp2);

                    ASSERT(pdispProps) ;

                    //  Do "Requests.Add("*", pdispProps)
                    //
                    pszDispMethod = L"Add";
                    hr = drvRequests.m_lpDispatch->GetIDsOfNames(IID_NULL,
                                        (LPWSTR FAR*)&pszDispMethod,
                                        1, 0,  &dispid);   // get the requests object id                    
                    if (FAILED(hr))
                    {
                        pdispProps->Release() ;
                        strErrorMsg = "DataObject does not appear to support WOSA/XRT, the \"" + 
                                    CString(pszDispMethod) + "\" method was not found." ;
                        AfxThrowOleException(hr) ;
                    }

                    VariantInit(&vtp1);
                    vtp1.vt = VT_BSTR ;
                    vtp1.bstrVal = m_strRequest.AllocSysString() ;  // wildcard
                    ASSERT(pdispProps) ;
                    VariantInit(&vtp2);
                    vtp2.vt = VT_DISPATCH ;
                    vtp2.pdispVal = pdispProps ;
                    pdispProps = NULL ;  // passed to vtp2.pdispVal

                    strErrorMsg = "DataObject.Requests.Add(\""+ m_strRequest + "\", props) failed." ;
                    LPDISPATCH pdispAdd = NULL ; // Request.Add returns VT_DISPATCH
                    // InvokeHelper uses this to describe method params.
                    static BYTE BASED_CODE propparms2[] = VTS_VARIANT VTS_VARIANT ;
                    drvRequests.InvokeHelper(   dispid, 
                                                DISPATCH_METHOD,
                                                VT_DISPATCH,        // return type
                                                (LPDISPATCH *)&pdispAdd, 
                                                propparms2,
                                                (VARIANT*)&vtp1,   // "*"
                                                (VARIANT*)&vtp2 );       // pdispProps

                    VariantClear(&vtp1);
                    VariantClear(&vtp2) ;
                    if (pdispAdd != NULL)
                        pdispAdd->Release() ;

                    drvRequests.ReleaseDispatch();
                }
            }
        }

        // Clear display
        Clear() ;

        strErrorMsg = "" ;
        SyncDataObjectActiveState();        
    }
    catch(CException* pException)
    {
        VariantClear(&vtp1);
        VariantClear(&vtp2);

        drvRequests.ReleaseDispatch() ;
            
        if (pException->IsKindOf(RUNTIME_CLASS(COleException)))
            hr = ((COleException*)pException)->m_sc ;

        ThrowMyDispatchException(pException, strErrorMsg, hr);

    }

    return TRUE ;
}

void CTickCtrl::ThrowMyDispatchException(CException* pException, const CString& strErrorMsg, HRESULT hr)
{
    if (pException->IsKindOf(RUNTIME_CLASS(COleDispatchException)))
        throw pException ;

    USES_CONVERSION;
    // allocate new Dispatch exception, and fill it
    COleDispatchException* pExcept = new COleDispatchException(NULL, 0, 0);
    if (strErrorMsg != "")
        pExcept->m_strDescription = strErrorMsg;
    pExcept->m_strHelpFile = "" ;
    pExcept->m_dwHelpContext = NULL ;

    if (pException->IsKindOf(RUNTIME_CLASS(COleException)))
        pExcept->m_scError = ((COleException*)pException)->m_sc ;
    else            
        pExcept->m_scError = hr ;
    pExcept->m_strSource = SysAllocString(T2OLE(_szProgID_CTickCtrl)) ;

#ifdef _DEBUG
    LPVOID lpMessageBuffer = NULL ;
    if (FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER |
          FORMAT_MESSAGE_FROM_SYSTEM,
          NULL,
          GetScode(hr),
          MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
          (LPTSTR) &lpMessageBuffer,
          0,
          NULL ))
    {
        CString str2((LPTSTR)lpMessageBuffer) ;
        str2 += (LPTSTR)AfxGetFullScodeString(hr) ;
        pExcept->m_strDescription += _T("\n") + str2 ;
        LocalFree(lpMessageBuffer) ;
    }
    else
    {
        CString str2(_T( "<No System Error Message Defined>")) ;
        str2 += (LPTSTR)AfxGetFullScodeString(hr) ;
        pExcept->m_strDescription += _T("\n") + str2 ;
    }
#endif

    pException->Delete();
    // then throw the exception
    throw pExcept;
}

void CTickCtrl::OnDataObjectVisibleChanged() 
{
    SyncDataObjectVisibleState()  ;
    SetModifiedFlag();
}

// Sync up the data object's "Visible" property with
// m_fDataObjectVisible
//
void CTickCtrl::SyncDataObjectVisibleState() 
{
    USES_CONVERSION ;
    if (m_pDataObject== NULL || m_fWOSAXRT == FALSE)
        return ;

    HRESULT hr = ResultFromScode(E_FAIL) ;
    DISPID              dispid ;
    LPWSTR              pszDispMethod = NULL ;
    CString strErrorMsg ;

    COleDispatchDriver drvDataObject ;
    drvDataObject.m_bAutoRelease = TRUE ;
    HRESULT hrDisp = m_pDataObject->QueryInterface(IID_IDispatch, (void**)&drvDataObject.m_lpDispatch);

    pszDispMethod = L"Visible" ;
    hr = drvDataObject.m_lpDispatch->GetIDsOfNames(IID_NULL,
                    (LPWSTR FAR*)&pszDispMethod,
                    1, 0,  &dispid );
    if (SUCCEEDED(hr))
        drvDataObject.SetProperty(dispid, VT_BOOL, m_fDataObjectVisible); 
}


void CTickCtrl::OnDataObjectActiveChanged() 
{
    SyncDataObjectActiveState();        
    SetModifiedFlag();
}

// Sync up the data object's "Active" property with
// m_fDataObjectActive
//
// Sets up or tears down the advise connection as well.
//
void CTickCtrl::SyncDataObjectActiveState()
{
    USES_CONVERSION;
    if (m_pDataObject == NULL)
        return ;

    HRESULT hr = ResultFromScode(E_FAIL) ;
    LPWSTR  pszDispMethod = NULL ;
    DISPID  dispid ;
    CString strErrorMsg ;

    if (m_fActive)
        SetupAdviseSink() ;
    else 
        KillAdviseSink() ;

    if (m_fWOSAXRT)
    {
        COleDispatchDriver drvDataObject ;
        drvDataObject.m_bAutoRelease = TRUE ;
        HRESULT hrDisp = m_pDataObject->QueryInterface(IID_IDispatch, (void**)&drvDataObject.m_lpDispatch);

        // DataObject.Active = m_fActive
        //
        pszDispMethod = L"Active" ;
        hr = drvDataObject.m_lpDispatch->GetIDsOfNames(IID_NULL,
                        (LPWSTR FAR*)&pszDispMethod,
                        1, 0,  &dispid );
        if (SUCCEEDED(hr))
            drvDataObject.SetProperty(dispid, VT_BOOL, m_fActive ); 
    }
}

void CTickCtrl::OnAutoStartDataObjectChanged() 
{
    if (m_fAutoStartDataObject == TRUE && m_pDataObject == NULL && !m_strDataObjectName.IsEmpty())
        StartDataObject() ;

    SetModifiedFlag();
}

void CTickCtrl::OnCacheOnChanged() 
{
    if (m_fCacheOn == FALSE)
        while (m_qCache.IsEmpty() == FALSE)
            delete (CTick*)m_qCache.RemoveHead() ;

    SetModifiedFlag();
}


BOOL CTickCtrl::DataObjectAdvise() 
{
    if (SUCCEEDED(SetupAdviseSink()))
        return TRUE ;
    return FALSE;
}

BOOL CTickCtrl::DataObjectUnAdvise() 
{
    if (SUCCEEDED(KillAdviseSink()))
        return TRUE ;
    return FALSE ;
}

HRESULT CTickCtrl::SetupAdviseSink()
{
    HRESULT hr = S_OK ;
    if (m_dwAdvise == NULL)
    {
        // Get IDataObject* and setup the AdviseSink
        //
        if (m_pDataObject == NULL)
            AfxThrowOleDispatchException(0, _T("The DataObject is not active.")) ;

        FORMATETC etc ;
        memset(&etc, 0, sizeof(FORMATETC));
        etc.cfFormat = (CLIPFORMAT)g_CF_XRT;
        etc.tymed = TYMED_HGLOBAL;
        etc.lindex = -1;
        etc.dwAspect = DVASPECT_CONTENT ;

        hr = m_pDataObject->QueryGetData(&etc) ;
        if (hr != S_OK)
        {
            // Doesn't support XRT.  Try text
            //
            etc.cfFormat = CF_TEXT;
            hr = m_pDataObject->QueryGetData(&etc) ;
            if (FAILED(hr))
                AfxThrowOleDispatchException(0, _T("The DataObject does not support a compatible data format.")) ;
        }

        hr = m_pDataObject->DAdvise(&etc, 
            (etc.cfFormat == CF_TEXT) ? ADVF_PRIMEFIRST : 0, 
            &m_xRTAdviseSink, &m_dwAdvise) ;
        if (FAILED(hr))
            AfxThrowOleDispatchException(0, _T("The DataObject does not support notifications."));
    }
    return hr ;
}

HRESULT CTickCtrl::KillAdviseSink()
{
    HRESULT hr = S_OK ;
    if (m_dwAdvise != NULL)
    {
        // Kill the advise sink
        //
        if (m_pDataObject == NULL)
            AfxThrowOleDispatchException(0, _T("The DataObject is not active."));

        hr = m_pDataObject->DUnadvise(m_dwAdvise) ;
        m_dwAdvise = NULL ;
        if (FAILED(hr))
            AfxThrowOleDispatchException(0, _T("DUnadvise failed")) ;
    }

    return hr ;
}

// new tick
//
void CTickCtrl::Tick( long ID, const VARIANT FAR& Name, const VARIANT FAR& Value) 
{
    VARIANT varName, varValue ;
    VariantInit(&varName) ;
    VariantInit(&varValue) ;
    HRESULT hr ;

    hr = VariantChangeType( &varName, (LPVARIANT)&Name, 0, VT_BSTR ) ;
    if (FAILED(hr))
    {
        varName.vt = VT_BSTR ;
        varName.bstrVal = SysAllocString(L"<Invalid Name>") ;
    }
    hr = VariantChangeType( &varValue, (LPVARIANT)&Value, 0, VT_BSTR );
    if (FAILED(hr))
    {
        varValue.vt = VT_BSTR ;
        varValue.bstrVal = SysAllocString(L"<Invalid Value>") ;
    }
    
    // strName and strValue now have our data.
    // Use pUpdateItem->dwDataItemID to find our item in 
    // the new queue or current item
    //
    if (m_pOnstage && m_pOnstage->m_dwDataItemID == (DWORD)ID)
    {
        m_pOnstage->m_bDelete = TRUE ; // mark for deletion
        CTick *pTick = new CTick(this) ;                            
        pTick->m_strName = varName.bstrVal ;
        pTick->m_strValue = varValue.bstrVal  ;
        pTick->m_dwDataItemID = ID ;
        m_qNew.AddTail( pTick ) ;
    }
    else
    {
        CTick* pTick = FindInQue(m_qNew, ID ) ;
        if (pTick)
        {
            // Just change the value (it's never been painted before)
            //pTick->m_strName = strName ; /*name should never change*/
            pTick->m_strValue = varValue.bstrVal ;
        }                       
        else
        {
            // Look in the cache
            //
            POSITION pos = m_qCache.GetHeadPosition() ;
            while (pos)
            {
                POSITION pos2 = pos ;
                pTick = (CTick*)m_qCache.GetNext(pos) ;
                ASSERT(pTick) ;
                if (pTick->m_dwDataItemID == (DWORD)ID )
                {
                    delete pTick ;
                    m_qCache.RemoveAt( pos2 ) ;
                    break ;
                }
            }
            pTick = new CTick(this) ;                            
            pTick->m_strName = varName.bstrVal ;
            pTick->m_strValue = varValue.bstrVal ;
            pTick->m_dwDataItemID = ID ;
            m_qNew.AddTail( pTick ) ;
        }
    }

    if (m_qNew.GetCount() > m_nMaxCacheSize)
    {
        delete (CTick*)m_qNew.RemoveHead() ;
        FireCacheOverflow() ;
    }
}


/////////////////////////////////////////////////////////////////////////////
// IAdvsiseSink interface implementation
//
// NOTE: We DO NOT use DECLARE/IMPLEMENT_INTERFACE_MAP because we don't
// want this IAdviseSink implementation goofing with the Control object
// itself (otherwise we get goofy reference count problems).
//
// We name this sub-object (X)RTAdviseSink so it does not conflict with
// one that may be implemented on the control itself.
//

/////////////////////////////////////////////////////////////////////////////
// Implementation of IAdviseSink
//
CTickCtrl::XRTAdviseSink::XRTAdviseSink()
{
    m_cRef = 1 ;
}

STDMETHODIMP_(ULONG) CTickCtrl::XRTAdviseSink::AddRef()
{
    //  METHOD_PROLOGUE(CTickCtrl, AdviseSink)
    return (ULONG)++m_cRef ;//pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CTickCtrl::XRTAdviseSink::Release()
{
//  METHOD_PROLOGUE(CTickCtrl, AdviseSink)
//  return (ULONG) pThis->ExternalRelease();
    --m_cRef ;
    if (m_cRef < 0)
    {
        TRACE("AdviseSink ref count went less than zero!\n") ;
    }
    return m_cRef ;
}

STDMETHODIMP CTickCtrl::XRTAdviseSink::QueryInterface(
    REFIID iid, LPVOID far* ppvObj)
{
    //METHOD_PROLOGUE(CTickCtrl, AdviseSink)
    //return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);

    *ppvObj = NULL ;
    if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IAdviseSink))
        *ppvObj = this ;

    if (*ppvObj == NULL)
        return ResultFromScode(E_NOINTERFACE) ;
    
    AddRef() ;
    return S_OK ;
}

STDMETHODIMP_(void) CTickCtrl::XRTAdviseSink::OnDataChange(
    LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium)
{   
    METHOD_PROLOGUE(CTickCtrl, RTAdviseSink)
    ASSERT_VALID(pThis);
        
    if (!(lpFormatEtc->tymed & TYMED_HGLOBAL)) 
        return ;

    if (lpFormatEtc->cfFormat == g_CF_XRT)
    {
        // we've received an update in our private format
        LPMARKETDATA pXRT = (LPMARKETDATA)GlobalLock( lpStgMedium->hGlobal) ;
        if (pXRT == NULL) 
        {           
            return;
        }
        pThis->XRTDataChange(pXRT);                    // deal with the data
        GlobalUnlock(lpStgMedium->hGlobal);         // unlock the memory
    }
    else if (lpFormatEtc->cfFormat == CF_TEXT)
    {
        // we've received an update in text format
        TCHAR* psz = (TCHAR*)GlobalLock( lpStgMedium->hGlobal) ;
        if (psz == NULL) 
            return;

        pThis->TextDataChange(psz);                    // deal with the data
        GlobalUnlock(lpStgMedium->hGlobal);         // unlock the memory
    }
}


STDMETHODIMP_(void) CTickCtrl::XRTAdviseSink::OnViewChange(
    DWORD aspects, LONG lindex)
{
    METHOD_PROLOGUE(CTickCtrl, RTAdviseSink)
    ASSERT_VALID(pThis);

//    pThis->OnChange(OLE_CHANGED, (DVASPECT)aspects);
}

STDMETHODIMP_(void) CTickCtrl::XRTAdviseSink::OnRename(LPMONIKER lpMoniker)
{
    // only interesting to the OLE link object.  Containers ignore this.
}

STDMETHODIMP_(void) CTickCtrl::XRTAdviseSink::OnSave()
{
    METHOD_PROLOGUE(CTickCtrl, RTAdviseSink)
    ASSERT_VALID(pThis);

//    pThis->OnChange(OLE_SAVED, (DVASPECT)0);
}

STDMETHODIMP_(void) CTickCtrl::XRTAdviseSink::OnClose()
{
    METHOD_PROLOGUE(CTickCtrl, RTAdviseSink)
    ASSERT_VALID(pThis);

//    pThis->OnChange(OLE_CLOSED, (DVASPECT)0);
}


/**************************************************************
    FUNCTION:   StockToFraction(double, char *)
    
    PURPOSE:    Converts a double value representing a stock price
                into its integer and fractional representation.
                Character string szVal must be long enough to hold converted value.
                
    RETURNS:    TRUE    -- succeeded
                FALSE   -- failed               
**************************************************************/             
BOOL StockToFraction(double dblVal, TCHAR *szVal)
{
    double f, dn;   
    long n;
    long intpart;
    TCHAR buf[20];
    
    f = modf( dblVal, &dn );
    
    n = (long) (f * (float) 10000.0);
    intpart = (long) dn;
    
    // also deals with rounding (you could also only use the first two digits after the
    // decimal -- since the numbers are still unique given two significant places, i.e.
    // x.3125 -> x.31 is different than x.375 -> x.38.  Should you decide to do so,
    // make sure you round correctly.
    
    switch (n) {
        case 0:                             break;      
        case 625:   strcpy(buf, _T("1/16"));    break;      
        case 1250:  
        case 1300:  strcpy(buf, _T("1/8"));     break;      
        case 1875:  
        case 1900:  strcpy(buf, _T("3/16"));    break;      
        case 2500:  strcpy(buf, _T("1/4"));     break;
        case 3100:
        case 3125:  strcpy(buf, _T("5/16"));    break;
        case 3750:  
        case 3800:  strcpy(buf, _T("3/8"));     break;
        case 4375:  
        case 4400:  strcpy(buf, _T("7/16"));    break;
        case 5000:  strcpy(buf, _T("1/2"));     break;
        case 5600:
        case 5625:  strcpy(buf, _T("9/16"));    break;
        case 6250:
        case 6300:  strcpy(buf, _T("5/8"));     break;
        case 6875:
        case 6900:  strcpy(buf, _T("11/16"));   break;
        case 7500:  strcpy(buf, _T("3/4"));     break;
        case 8100:
        case 8125:  strcpy(buf, _T("13/16"));   break;      
        case 8750:  
        case 8800:  strcpy(buf, _T("7/8"));     break;
        case 9375:  
        case 9400:  strcpy(buf, _T("15/16"));   break;
        default:    return (FALSE);                     
        }   
        
    ltoa(intpart, szVal, 10);       
    if (n > 0) {
        strcat(szVal, _T(" "));
        strcat(szVal, buf);
        }   
        
    return (TRUE);  
}

 /*
    FUNCTION:   CTickCtrl::TextDataChange(TCHAR*)
    
    PURPOSE:    Called by CTickCtrl::XRTAdviseSink::OnDataChange() when new Text data
                arrives.
*/              
void CTickCtrl::TextDataChange(TCHAR* pszText)
{
    SetText(pszText);
}

 /*
    FUNCTION:   CTickCtrl::XRTDataChange(LPMARKETDATA)
    
    PURPOSE:    Called by CTickCtrl::XRTAdviseSink::OnDataChange() when new XRT data
                arrives. Processes XRT stock data.
*/              
void CTickCtrl::XRTDataChange(LPMARKETDATA pMD)
{
    USES_CONVERSION;
    HRESULT             hr ;
    LPUPDATEITEM        pUpdateItem ;
    LPUPDATEITEMPROP    rgPropIDValue ;
    LPPROPERTYSET       pPropSet ;
    DWORD               dwPSO = pMD->dwPropSetOffset ;
    DWORD               dwIO = pMD->dwItemOffset ;
    DWORD               cProps ;
    VARIANT             var ;
    VARIANT FAR *       pvar ;

    VARIANT             varName ;
    CString             strValue ;

    // If we think the feed is down and the server says it isn't
    // then we shouldn't think it is....
    //
    if (m_fFeedDown == TRUE)
    {
        if (m_pOnstage && m_pOnstage->m_dwDataItemID == (DWORD)-1L)
        {
            m_pOnstage->m_bDelete = TRUE ;
        }
        else
        {
            CTick* pTick ;
            pTick = FindInQue( m_qNew, (DWORD)-1L )  ;
            if (pTick != NULL)
            {
                POSITION pos ;
                pos = m_qNew.Find( pTick ) ;
                m_qNew.RemoveAt(pos) ;
                delete pTick ;
            }
            else
            {
                pTick = FindInQue( m_qCache, (DWORD)-1L )  ;
                if (pTick != NULL)
                {
                    POSITION pos ;
                    pos = m_qCache.Find( pTick ) ;
                    m_qCache.RemoveAt(pos) ;
                    delete pTick ;
                }
            }

        }        
        m_fFeedDown = FALSE ;
    }
    
    // If the server says the feed is down, we should add an item
    // telling the user
    //
    if (pMD->dwSubStatus == xrtErrorFeedDown)
    {
        VariantInit( &varName ) ;
        VariantInit( &var ) ;

        varName.vt = VT_BSTR ;
        varName.bstrVal = SysAllocString(L"Data not currently available...");
        var.vt = VT_BSTR ;
        var.bstrVal = SysAllocString(L"");
        Tick( -1, varName, var ) ;
        m_fFeedDown = TRUE ;
    }

    VariantInit( &var ) ;
    while (dwIO != 0) 
    {
        pUpdateItem = (LPUPDATEITEM)((LPBYTE)pMD + dwIO) ;
        dwIO = pUpdateItem->dwNext ;
        pPropSet = (LPPROPERTYSET)((LPBYTE)pMD + pUpdateItem->dwPropertySet) ;
    
        // We now have a pointer to an UPDATEITEM and a PROPERTYSET 
        // use the combination to decipher the properites in the updateitem
        //
        rgPropIDValue = (LPUPDATEITEMPROP)((LPBYTE)(pUpdateItem) + sizeof(UPDATEITEM)) ;
        cProps = pUpdateItem->cProperties;   
        VariantClear( &var ) ;
        VariantInit(&varName );
        strValue.Empty() ;
        for (DWORD n = 0 ; n < cProps ; n++) 
        {   
            // Grab the property and make it a real VARIANT..
            //
            pvar = (VARIANT FAR*)((LPBYTE)pUpdateItem + rgPropIDValue[n].dwValueOffset ) ;
            if (pvar->vt == VT_BSTR)
            {   
                var.vt = VT_BSTR ;
                var.bstrVal = SysAllocString(T2OLE((LPTSTR)pvar + sizeof(VARIANT) + sizeof(DWORD)));
            } 
            else
                var = *pvar ;
            
            // If it's numerical then coerce it to R8.  Otherwise coerce it to a string
            //
            if (var.vt == VT_R4 || var.vt == VT_R8)
            {
                // we've received a single or double
                VariantChangeType( &var, &var, 0, VT_R8 ) ;
                TCHAR szVal[50];
                StockToFraction( var.dblVal, szVal);
                var.vt = VT_BSTR ;
                var.bstrVal = SysAllocString(T2OLE(szVal)) ;
            }
            else if (var.vt != VT_BSTR && var.vt != VT_ERROR)
            {
                hr = VariantChangeType( &var, &var, 0, VT_BSTR ) ;
                if (FAILED(hr))
                {
                    VariantClear(&var) ;
                    var.vt = VT_BSTR ;
                    var.bstrVal = SysAllocString(L"<VALUE>") ;
                }
            }       

            ASSERT(var.vt != VT_ERROR) ;
                        
            // If the property ID is 1 or 0 then this property is "Name"
            //
            if (rgPropIDValue[n].dwPropertyID <= 1) 
            {
                VariantCopy( &varName, &var ) ;
            }
            else
            {
                if (!strValue.IsEmpty())
                    strValue += " " ;
                strValue += var.bstrVal ;
            }

            VariantClear( &var ) ;

        }  // for n < cProps

        VariantClear(&var) ;
        var.vt = VT_BSTR ;
        var.bstrVal = strValue.AllocSysString();
        Tick( pUpdateItem->dwDataItemID, varName, var ) ;
    }       
}

/////////////////////////////////////////////////////////////////////////////
// default callback implementation
//
/*
void CTickCtrl::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
    ASSERT_VALID(this);
    
    switch (nCode)
    {
    case OLE_CLOSED:
        break;  // no default implementation

    case OLE_CHANGED:
    case OLE_SAVED:
        break;

    case OLE_CHANGED_STATE:
    case OLE_CHANGED_ASPECT:
        break;  // no default implementation

    default:
        ASSERT(FALSE);
    }

    ASSERT_VALID(this);
}
*/

void CTickCtrl::ErrorMessageBox( const CString& str, HRESULT hr )
{
#ifdef _DEBUG
    #ifdef WIN32
    LPVOID lpMessageBuffer = NULL ;
    if (FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER |
          FORMAT_MESSAGE_FROM_SYSTEM,
          NULL,
          GetScode(hr),
          MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
          (LPTSTR) &lpMessageBuffer,
          0,
          NULL ))
    {
        CString str2((LPTSTR)lpMessageBuffer) ;
        str2 += (LPTSTR)AfxGetFullScodeString(hr) ;
        AfxMessageBox( str + _T("\n") + str2) ;
        LocalFree(lpMessageBuffer) ;
    }
    else
    {
        CString str2(_T( "<No System Error Message Defined>")) ;
        str2 += (LPTSTR)AfxGetFullScodeString(hr) ;
        AfxMessageBox( str + _T("\n") + str2 ) ;
    }
    #else
    AfxMessageBox( str + _T("\n") + (LPTSTR)AfxGetFullScodeString( GetScode( hr ) ) ) ;
    #endif
#else
    AfxMessageBox( str ) ;
#endif

}

/////////////////////////////////////////////////////////////////////////////
// Display Queue related members
//
// EnQue's an item onto the end (tail) of the offstage queue
//
CTick* CTickCtrl::FindInQue( CObList& que, DWORD dwDataItemID ) 
{
    CTick* pTick ;
    POSITION pos = que.GetHeadPosition() ;
    while (pos)
    {
        pTick = (CTick*)que.GetNext(pos) ;
        ASSERT(pTick) ;
        if (pTick->m_dwDataItemID == dwDataItemID)
        {
            return pTick ;
        }
    }
    return NULL ;
}

// Takes the entire text in both queues and concatenates it
// together, separating everything by tabs
//
CString CTickCtrl::GetQueText() 
{
    CString str ;
    CTick* pTick ;

    if (m_pOnstage)
        str = m_pOnstage->m_strName + "\t" + m_pOnstage->m_strValue ;

    POSITION pos = m_qCache.GetHeadPosition() ;
    if (pos != NULL)
        str += "\r\n" ;
    while (pos)
    {
        pTick = (CTick*)m_qCache.GetNext(pos) ;
        ASSERT(pTick) ;
        str += pTick->m_strName + "\t" + pTick->m_strValue ;
        if (pos != NULL)
            str += "\r\n" ;
    }

    pos = m_qNew.GetHeadPosition() ;
    if (pos != NULL)
        str += "\r\n" ;
    while (pos)
    {
        pTick = (CTick*)m_qNew.GetNext(pos) ;
        ASSERT(pTick) ;
        str += pTick->m_strName + "\t" + pTick->m_strValue ;
        if (pos != NULL)
            str += "\r\n" ;
    }

    return str ;
}

void CTickCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    CPoint  pointHit = point ;
    CRect rect ;
    GetClientRect( &rect ) ;

    // rect must be in screen co-ordinates
    CPoint ptOffset = point - rect.TopLeft();
    ClientToScreen(&rect);  

    m_datasourceDropSource.DoDragDrop();
}

BOOL CTickCtrl::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
#ifndef _DEBUG
    lpFormatEtc; // unused in retail builds
    phGlobal;    // unused in retail builds
#endif

    ASSERT_VALID(this);
    ASSERT(AfxIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));
    ASSERT(AfxIsValidAddress(phGlobal, sizeof(HGLOBAL)));

    BOOL bSuccess = FALSE ;
    if (lpFormatEtc->cfFormat == CF_TEXT)
    {
        if (*phGlobal == NULL)
        {   
            int n ;
            *phGlobal = GlobalAlloc( GHND, n = GetQueText().GetLength()+1 ) ;
            #ifdef _UNICODE
            wcstombs( (LPSTR)GlobalLock(*phGlobal), GetQueText(), n ) ;
            #else
            memcpy( GlobalLock(*phGlobal), (LPCSTR)GetQueText(), n ) ; 
            #endif
            GlobalUnlock(*phGlobal) ;
            bSuccess = TRUE ;
        }
    }
    #if defined(_WIN32) && defined(_UNICODE)
    else if (lpFormatEtc->cfFormat == CF_UNICODETEXT)
    {
        if (*phGlobal == NULL)
        {   
            int n ;
            *phGlobal = GlobalAlloc( GHND, n = 2 * (GetQueText().GetLength()+1) ) ;
            memcpy( GlobalLock(*phGlobal), (LPCWSTR)GetQueText(), n ) ; 
            GlobalUnlock(*phGlobal) ;
            bSuccess = TRUE ;
        }
    }
    #endif

    if (bSuccess == TRUE) 
        return TRUE ;

    return COleControl::OnRenderGlobalData( lpFormatEtc, phGlobal ) ;
}

// We support drag/drop from the control.  However, we only want
// to provide IDataObject to the drop target, so we implement
// an embedded COleDataSource object (m_datasourceDropSource) that provides
// this additional IDataObject interface.   Note that if a client
// QI's on the control's IUnknown for IDataObject he will get the 
// IDataObject implemented by m_sourceData, *not* this one!
//
BOOL CTickCtrl::CDropSourceDataSource::OnRenderGlobalData(
    LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
    ASSERT_VALID(this);
    CTickCtrl* pCtrl = (CTickCtrl*)
        ((BYTE*)this - offsetof(CTickCtrl, m_datasourceDropSource));

    return pCtrl->OnRenderGlobalData(lpFormatEtc, phGlobal);
        // Note: COleDataSource has no implementation
}

BOOL CTickCtrl::CDropSourceDataSource::OnRenderData(
    LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium)
{
    ASSERT_VALID(this);
    CTickCtrl* pCtrl = (CTickCtrl*)
        ((BYTE*)this - offsetof(CTickCtrl, m_datasourceDropSource));
    
    if (pCtrl->OnRenderData(lpFormatEtc, lpStgMedium))
        return TRUE;
    
    return COleDataSource::OnRenderData(lpFormatEtc, lpStgMedium);  
}

BOOL CTickCtrl::CDropSourceDataSource::OnSetData(LPFORMATETC lpFormatEtc,
    LPSTGMEDIUM lpStgMedium, BOOL bRelease)
{
    ASSERT_VALID(this);
    CTickCtrl* pCtrl = (CTickCtrl*)
        ((BYTE*)this - offsetof(CTickCtrl, m_datasourceDropSource));
    
    return pCtrl->OnSetData(lpFormatEtc, lpStgMedium, bRelease);
        // Note: COleDataSource has no implementation
}


// OLE Drag/Drop target support
//
DROPEFFECT CTickCtrl::CMyOleDropTarget::OnDragEnter(CWnd* , COleDataObject* , DWORD , CPoint )
{
    DROPEFFECT de = DROPEFFECT_COPY ;
    return de ;
}

DROPEFFECT CTickCtrl::CMyOleDropTarget::OnDragOver(CWnd*, COleDataObject* , DWORD dwKeyState, CPoint )
{
    DROPEFFECT de = DROPEFFECT_COPY;
    // check for force link
    if ((dwKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
        de = DROPEFFECT_LINK;
    // check for force copy
    else 
        if ((dwKeyState & MK_CONTROL) == MK_CONTROL)
        de = DROPEFFECT_COPY;
    // check for force move
//    else if ((dwKeyState & MK_ALT) == MK_ALT)
//        de = DROPEFFECT_MOVE;
    // default -- recommended action is move
//    else
//        de = DROPEFFECT_MOVE;

    return de;
}

BOOL CTickCtrl::CMyOleDropTarget::OnDrop(CWnd* , COleDataObject* pDataObject, DROPEFFECT de , CPoint )
{
    CTickCtrl*  ptick = (CTickCtrl*)CWnd::FromHandle(m_hWnd) ;

    STGMEDIUM stgm ;
    if (de == DROPEFFECT_COPY)
    {
        stgm.tymed = TYMED_HGLOBAL;
        stgm.hGlobal = NULL ;
        stgm.pUnkForRelease = NULL ;
        if (!pDataObject->GetData(CF_TEXT, &stgm))
            return FALSE;

        TCHAR* psz = (TCHAR*)GlobalLock(stgm.hGlobal) ;
        if (psz == NULL) 
        {
            ReleaseStgMedium(&stgm);
            return FALSE;
        }

        ptick->TextDataChange(psz);         // deal with the data
        GlobalUnlock(stgm.hGlobal);         // unlock the memory
        ReleaseStgMedium(&stgm);
        ptick->m_strDataObjectName = _T("Drag & Drop Data");
    }
    else
    {
        stgm.tymed = TYMED_ISTREAM;
        stgm.pstm = NULL;
        stgm.pUnkForRelease = NULL ;

        if (!pDataObject->GetData((CLIPFORMAT)::RegisterClipboardFormat(_T("Link Source")), &stgm))
            return FALSE;
        ASSERT(stgm.pstm);
        IMoniker* pmk = NULL ;
        if (stgm.pstm && SUCCEEDED(OleLoadFromStream(stgm.pstm, IID_IMoniker, (void**)&pmk)))
        {
            LPBINDCTX   pbc ;
            CreateBindCtx(0,&pbc) ;
            IDataObject* pdo = NULL ;
            HRESULT hr = pmk->BindToObject( pbc, NULL, IID_IDataObject,(void**)&pdo) ;
            if (SUCCEEDED(hr))
            {
                WCHAR* pwsz = NULL ;
                pmk->GetDisplayName(pbc, NULL, &pwsz);
                ASSERT(pwsz);
                ptick->m_strDataObjectName = pwsz ;
                CoTaskMemFree(pwsz);
                ptick->SetDataObject((IDispatch*)pdo);
                pdo->Release();
                ptick->m_fActive = TRUE ;
                ptick->InitializeDataObject();
            }
            pbc->Release() ;
            pmk->Release() ;
        }

        ReleaseStgMedium(&stgm);
    }
    return TRUE;
}


void CTickCtrl::OnDropFiles(HDROP hDropInfo) 
{
	TCHAR szFileName[_MAX_PATH];
	::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);
    m_strDataObjectName = szFileName;
    OnDataObjectNameChanged();
    StartDataObject();
    InitializeDataObject();
	::DragFinish(hDropInfo);
}
