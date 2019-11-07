// header.cpp : implementation file
//

#include "stdafx.h"
#include "winprt20.h"
#include "headerse.h"
#include "header.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeader
IMPLEMENT_DYNCREATE(CHeader, CControlProperty)

// This macro is defined in cprop.h
IMPLEMENT_CONTROLPROP(CHeader,
    0x2a50B, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x46)

CHeader::CHeader() 
{
    m_pControl = NULL ;
    EnableAutomation();
    OnResetState() ;  
    
    m_flHeight = 0.0 ;

    m_Left.m_pHeader = this ; 
    m_Left.SetName("Left");
    m_Center.m_pHeader = this ;
    m_Center.SetName("Center");
    m_Right.m_pHeader = this ;
    m_Right.SetName("Right");
}

CHeader::~CHeader()
{
}

BEGIN_MESSAGE_MAP(CHeader, CControlProperty)
    //{{AFX_MSG_MAP(CHeader)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CHeader, CControlProperty)
    //{{AFX_DISPATCH_MAP(CHeader)
    DISP_PROPERTY_NOTIFY(CHeader, "Height", m_flHeight, OnHeightChanged, VT_R4)
    DISP_PROPERTY_EX(CHeader, "Left", GetLeft, SetLeft, VT_DISPATCH)
    DISP_PROPERTY_EX(CHeader, "Center", GetCenter, SetCenter, VT_DISPATCH)
    DISP_PROPERTY_EX(CHeader, "Right", GetRight, SetRight, VT_DISPATCH)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

void CHeader::DoPropExchange(CPropExchange* pPX)
{
    PX_Float( pPX, "Height", m_flHeight ) ;

//    PX_Property( pPX, "Left", &m_Left ) ;
//    PX_Property( pPX, "Center", &m_Center ) ;
//    PX_Property( pPX, "Right", &m_Right ) ;
}

void CHeader::OnResetState()
{   
    // Set defaults
    m_flHeight = 0.0 ;
    m_iHeight = 0 ;     
    m_Left.OnResetState() ;
    m_Center.OnResetState() ;
    m_Right.OnResetState() ;
}

BOOL CHeader::Recalc( CDC* pdc ) 
{
    m_iHeight = (int)((float)pdc->GetDeviceCaps(LOGPIXELSY) * m_flHeight) ;     // 
    return TRUE ;
}

// Draw left, center, and right sections.  rc defines the rectangle
// the header/footer is drawn within.  CHeaderSection does not know
// how to paint itself because we need to be able to handle clipping
// when one section overlaps another due to extra long text.
//
void CHeader::Draw( CDC *pdc, const CRect& rc )
{
    UNUSED pdc ;
    UNUSED rc ;
    
    //CString strLeft = m_Left.;
    //CString strCenter ;
    //CString strRight ;
    
}        
/////////////////////////////////////////////////////////////////////////////
// CHeader message handlers


void CHeader::OnHeightChanged() 
{
    // TODO: Add notification handler code

}

LPDISPATCH CHeader::GetLeft() 
{
    // REVIEW:  Should we be incrementing ref count here?
    return m_Left.GetIDispatch( TRUE );
}

void CHeader::SetLeft(LPDISPATCH newValue) 
{
    UNUSED newValue ;
    
    // BUGBUG: Replace this with version that get's string
    // from resources.
    //
    AfxThrowOleDispatchException( 0, "Header/Footer Sections are not settable properties" ) ;
}

LPDISPATCH CHeader::GetCenter() 
{
    // REVIEW:  Should we be incrementing ref count here?
    return m_Center.GetIDispatch( TRUE );
}

void CHeader::SetCenter(LPDISPATCH newValue) 
{
    UNUSED newValue ;
    
    // BUGBUG: Replace this with version that get's string
    // from resources.
    //
    AfxThrowOleDispatchException( 0, "Header/Footer Sections are not settable properties" ) ;
}

LPDISPATCH CHeader::GetRight() 
{
    // REVIEW:  Should we be incrementing ref count here?
    return m_Right.GetIDispatch( TRUE );
}

void CHeader::SetRight(LPDISPATCH newValue) 
{
    UNUSED newValue ;
    
    // BUGBUG: Replace this with version that get's string
    // from resources.
    //
    AfxThrowOleDispatchException( 0, "Header/Footer Sections are not settable properties" ) ;
}
