// page.cpp : implementation file
//

#include "stdafx.h"
#include "winprt20.h"
#include "control.h"
#include "page.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define COL_POS( col )\
    MulDiv( col, m_rcWorkspace.right -\
                 (int)( m_pControl->m_flHorzSeparation * m_iLogPixelsX) * (m_pControl->m_sColumns-1),\
            m_pControl->m_sColumns )

#define ROW_POS( row )\
    MulDiv( row, m_rcWorkspace.bottom -\
                 (int)(m_pControl->m_flVertSeparation * m_iLogPixelsY) * (m_pControl->m_sRows-1),\
            m_pControl->m_sRows )

/////////////////////////////////////////////////////////////////////////////
// CPage
CPage::CPage()
{
    m_pControl = NULL ;
    
    m_iResolutionX = 0 ;
    m_iResolutionY = 0 ;

    m_iLogPixelsX = 0  ;
    m_iLogPixelsY = 0  ;
    
    m_ptPrintOffset.x = 0 ;
    m_ptPrintOffset.y = 0 ;

    m_rcWorkspace.SetRectEmpty() ;

    m_iLogicalPageX = 0  ;
    m_iLogicalPageY = 0  ;
}

CPage::~CPage()
{
}

BOOL CPage::Recalc( CDC* pdc ) 
{
    CDC dc ;
    HDC hdc = NULL ;

    TRACE("CPage::Recalc()\n" ) ;
        
    ASSERT_VALID( m_pControl ) ;
    
    if (pdc == NULL)
    {
        TRACE("    pdc == NULL\n" ) ;
        
        CPrintDialog   pd( FALSE ) ;
        if (m_pControl->m_fDefaultPrinter)
        {
            pd.GetDefaults() ;
            // Normally CPage should not change any of CControl's properties
            // but in the case of the default printer it's ok.  This way the user
            // always has the latest default printer strings in the control
            // properties, which is what is meant by the "Default Printer" setting.
            //
            m_pControl->m_strPrinter = pd.GetDeviceName() ;
            m_pControl->m_strPrinterDriver = pd.GetDriverName() ;
            m_pControl->m_strPrinterOutput = pd.GetPortName() ;
            m_pControl->SetModifiedFlag() ;
        }
        else
        {
            pd.m_pd.hDevNames = wpCreateDevNames( m_pControl->m_strPrinter, m_pControl->m_strPrinterDriver, m_pControl->m_strPrinterOutput ) ;
            pd.m_pd.hDevMode = wpCreateDevMode( m_pControl->m_strPrinter, m_pControl->m_strPrinterDriver, m_pControl->m_strPrinterOutput ) ;
        }
    
        ASSERT(pd.m_pd.hDevNames) ;
    
        if (pd.m_pd.hDevMode)
        {
            if (m_pControl->m_fUsePrinterSettings)
            {
                // Normally CPage should not change any of CControl's properties
                // but in the case of the default printer it's ok.  This way the user
                // always has the latest default printer strings in the control
                // properties, which is what is meant by the "Default Printer" setting.
                //
                m_pControl->m_lOrientation = (long)pd.GetDevMode()->dmOrientation ;
                m_pControl->m_lPaperSize = (long)pd.GetDevMode()->dmPaperSize ;
                m_pControl->m_lPaperLength = (long)pd.GetDevMode()->dmPaperLength ;
                m_pControl->m_lPaperWidth = (long)pd.GetDevMode()->dmPaperWidth ;
                m_pControl->m_lScale = (long)pd.GetDevMode()->dmScale ;
                m_pControl->m_lCopies= (long)pd.GetDevMode()->dmCopies ;
                m_pControl->m_lDefaultSource = (long)pd.GetDevMode()->dmDefaultSource ;
                m_pControl->m_lPrintQuality= (long)pd.GetDevMode()->dmPrintQuality ;
                m_pControl->m_lColor= (long)pd.GetDevMode()->dmColor ;
                m_pControl->m_lDuplex = (long)pd.GetDevMode()->dmDuplex ;
                m_pControl->SetModifiedFlag() ;
            }
            else
            {
                pd.GetDevMode()->dmOrientation = (int)m_pControl->m_lOrientation ;
                pd.GetDevMode()->dmPaperSize = (int)m_pControl->m_lPaperSize ;
                pd.GetDevMode()->dmPaperLength = (int)m_pControl->m_lPaperLength ;
                pd.GetDevMode()->dmPaperWidth = (int)m_pControl->m_lPaperWidth ;
                pd.GetDevMode()->dmScale = (int)m_pControl->m_lScale ;
                pd.GetDevMode()->dmCopies = (int)m_pControl->m_lCopies;
                pd.GetDevMode()->dmDefaultSource = (int)m_pControl->m_lDefaultSource ;
                pd.GetDevMode()->dmPrintQuality = (int)m_pControl->m_lPrintQuality;
                pd.GetDevMode()->dmColor = (int)m_pControl->m_lColor;
                pd.GetDevMode()->dmDuplex = (int)m_pControl->m_lDuplex ;
                pd.GetDevMode()->dmFields = (UINT)-1 ;
            }
        }
        hdc = pd.CreatePrinterDC() ;
    
        ASSERT(hdc) ;
    
        if (pd.m_pd.hDevMode)
            GlobalFree( pd.m_pd.hDevMode ) ;
        GlobalFree( pd.m_pd.hDevNames ) ;
        
        dc.Attach(hdc);
        pdc = &dc ;
    }
    
    ASSERT_VALID( pdc ) ;

    BOOL bSuccess = FALSE ;
    
    m_iResolutionX = pdc->GetDeviceCaps( HORZRES ) ;
    // REVIEW:  Postscript may need to have h-resolution decremented
    // by one.  Figure out if it's the PSCRIPT driver or not...
    m_iResolutionY = pdc->GetDeviceCaps( VERTRES ) ;
    
    m_iLogPixelsX = pdc->GetDeviceCaps( LOGPIXELSX ) ;
    m_iLogPixelsY = pdc->GetDeviceCaps( LOGPIXELSY ) ;

    int n = GETPRINTINGOFFSET ;
    if (!(m_pControl->m_fUsePrintOffset &&
        pdc->Escape( QUERYESCSUPPORT, sizeof(int), (LPCSTR)&n, NULL ) &&
        pdc->Escape( n, NULL, NULL, &m_ptPrintOffset )))
    {
        m_ptPrintOffset.x = 0 ;
        m_ptPrintOffset.y = 0 ;
    }
        
    // Give header a chance to init itself (determine height of header)
    //
    m_pControl->m_Header.Recalc( pdc ) ;
    
    // Give footer a chance to init itself (determine height of footer)
    //
    m_pControl->m_Footer.Recalc( pdc ) ;
      
    // Determine workspace size.  Nothing except headers/footers and borders are
    // ever printed outside of the workspace.
    //
    m_rcWorkspace.left = (int)(m_pControl->m_flLeftMargin * m_iLogPixelsX) - m_ptPrintOffset.x ;
    m_rcWorkspace.right = (m_iResolutionX - m_rcWorkspace.left - (int)(m_pControl->m_flRightMargin * m_iLogPixelsX)) + m_ptPrintOffset.x ;
    m_rcWorkspace.top = m_pControl->m_Header.m_iHeight + (int)(m_pControl->m_flTopMargin * m_iLogPixelsY) - m_ptPrintOffset.y ;
    m_rcWorkspace.bottom = (m_iResolutionY - m_rcWorkspace.top - m_pControl->m_Footer.m_iHeight - (int)(m_pControl->m_flBottomMargin * m_iLogPixelsY)) + m_ptPrintOffset.y ;
    
    // If there's a border, it reduces the size of the workspace.
    //
    if (m_pControl->m_fBorder)
        m_rcWorkspace.InflateRect(-1, -1) ;

    // Figure out the dimensions of a logical page
    // x = (r - (hs * (columns-1))) / columns
    //
    m_iLogicalPageX = (m_rcWorkspace.right - (int)(m_pControl->m_flHorzSeparation * m_iLogPixelsX * (m_pControl->m_sColumns - 1) )) / m_pControl->m_sColumns ;

    // y = (b - (vs * (rows-1))) / rows
    //
    m_iLogicalPageY = (m_rcWorkspace.bottom - (int)(m_pControl->m_flVertSeparation * m_iLogPixelsY * (m_pControl->m_sRows - 1) )) / m_pControl->m_sRows ;
    
    if (hdc != NULL)
       pdc->DeleteDC() ;

    m_pControl->InvalidateControl() ;
           
    return TRUE ;
}

// When ::Draw is called, the DC is setup such that 0,0 is the origin of where
// we are supposed to start painting
// 
void CPage::Draw( CDC *pdc ) 
{
    ASSERT_VALID( pdc ) ;
    
    CRect rc ;
    
    DrawFrame( pdc ) ;
    
    // Paint header
    //
    rc.left     = m_rcWorkspace.left - 1 ;
    rc.right    = m_rcWorkspace.left + m_rcWorkspace.right ;
    rc.bottom   = m_rcWorkspace.top  - 1 ;
    rc.top      = rc.bottom - m_pControl->m_Header.m_iHeight ;
    m_pControl->m_Header.Draw( pdc, rc ) ;
    
    // Paint Footer
    //
    rc.top = m_rcWorkspace.top + m_rcWorkspace.bottom - 2 ;
    rc.bottom = rc.top + m_pControl->m_Footer.m_iHeight ;
    m_pControl->m_Footer.Draw( pdc, rc ) ;

} 

void CPage::DrawFrame( CDC *pdc ) 
{
//    CRect   rcClipBox ;
//    pdc->GetClipBox( rcClipBox ) ;
    
    pdc->SelectStockObject( NULL_BRUSH ) ;
    
    // Draw the horz and vert separators
    //
    CRect   rc ;
    CRect   rc2 = m_rcWorkspace ;
    int     iX = m_iLogicalPageX ;
    int     iY = m_iLogicalPageY ;
    int     iHorzSep = (int)(m_pControl->m_flHorzSeparation * m_iLogPixelsX) ;
    int     iVertSep = (int)(m_pControl->m_flVertSeparation * m_iLogPixelsY) ;

    if (m_pControl->m_fBorder)
        pdc->SelectStockObject( BLACK_PEN ) ;
// TODO: m_fPreview (create RGBLTCYAN PS_DASH pen)
//    else if (m_fPreview)
//        pdc->SelectStock

    for (int row = 0 ; row < m_pControl->m_sRows ; row++ )
    {
        for (int col = 0 ; col < m_pControl->m_sColumns ; col++ )
        {
            rc.left = rc2.left + COL_POS(col) + (col * iHorzSep) ;
            rc.right = rc.left + iX - 1 ;

            rc.top = rc2.top + ROW_POS(row) + (row * iVertSep) ;
            rc.bottom = rc.top + iY - 1 ;

            rc.InflateRect( 1, 1 ) ;
            pdc->Rectangle( rc ) ;

            if (m_pControl->m_fShade)
                pdc->ExcludeClipRect( rc.left, rc.top, rc.right+1, rc.bottom+1 ) ;
        }
    }

    // Shade
    //
    if (m_pControl->m_fShade)
    {
        rc.left = rc.top = 0 ;
        rc.right = m_iResolutionX ;
        rc.bottom = m_iResolutionY ;
        pdc->FillRect( rc, CBrush::FromHandle( (HBRUSH)::GetStockObject( LTGRAY_BRUSH ) ) ) ;
    }
    
    
//    pdc->DPtoLP( rcClipBox ) ;
//    CRgn rgn ;
//    rgn.CreateRectRgn( rcClipBox.left, rcClipBox.top, rcClipBox.right, rcClipBox.bottom ) ;
//    pdc->SelectClipRgn( &rgn ) ;
//    rgn.DeleteObject() ;
}

int CPage::GetPhysicalPageNum() 
{
    return m_iPhysicalPage ;
}

int CPage::GetNumPhysicalPages() 
{

    // TODO!: Calculate number of pages

    return m_iNumPhysicalPages ;
}
